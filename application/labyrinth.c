#include "labyrinth.h"
#include "infrared_led.h"

#include "oled.h"
#include "stdio.h"
#include "string.h"



#define GO_SPEED			15
#define TURN_TIME  		500
#define TURN_SPEED  		7
labyrinth_t labyrinth;

float labyrinth_pwm_pid[2][3]={{100,1,0},
															 {100,1,0},
															 };


static uint8_t get_bit_num(uint8_t num)
{
	uint8_t count =0;
	
	for(uint8_t i =0;i<8;i++)
	{
		if(num&0x01)
			count++;
		num = num>>1;
	}
	
	return count;
}

//设置跟随差速
static void set_labyrinth_wd(void)
{
	uint8_t infrared = 0;
	infrared = infrared_sensor_read();
	labyrinth.l_infra_num = get_bit_num(infrared>>4);
	labyrinth.r_infra_num = get_bit_num(infrared&0x0f);
	labyrinth.infra_num = labyrinth.l_infra_num+labyrinth.r_infra_num;
	
	labyrinth.l_infra = infrared>>4;
	labyrinth.r_infra = ((infrared<<3)&0x0f)|((infrared<<1)&0x08)|((infrared>>3)&0x01)|((infrared>>1)&0x02);
	
	labyrinth.wd = labyrinth.l_infra - labyrinth.r_infra;
}

#include "delay.h"
//设置轮子速度
static void set_labyrinth_wheel_speed(void)
{
	char buffer[100] = {0};
	
	float Scale = 1;
	static uint8_t left_flag = 0;
//	
//	sprintf(buffer,"L = %d R = %d ALL = %d",labyrinth.l_infra_num,labyrinth.r_infra_num,labyrinth.infra_num);
//	OLED_ShowString(0, 0,(uint8_t*)buffer,8);
	
	//十字和T字
	if(labyrinth.l_infra_num>=3&&labyrinth.r_infra_num>=3)
	{
		car_turn_r_90_degree();
	}
	//-|字路口
	else if(labyrinth.l_infra_num>=40)
	{
		left_flag = 1;
		car_go_straight();
	}
	//无路
	else if(labyrinth.l_infra_num==0&& labyrinth.r_infra_num==0 && left_flag)
	{
		left_flag = 0;
		car_turn_l_90_degree();
		car_stop();
	}
	//|-字路口
	else if(labyrinth.r_infra_num>=3)
	{
		car_turn_r_90_degree();
	}
	else
	{
		//跑飞原地转圈
		if(labyrinth.infra_num == 0)
		{
			//car_back_straight();
			car_turn_l_90_degree();
			//delay_ms(1000);
			return;
		}
		else if(labyrinth.infra_num>=3 || labyrinth.infra_num <= 5) Scale = 0.4;
		else if(labyrinth.infra_num<=2) Scale = 1;
		else Scale = 0.2;
		
		labyrinth.wheel_speed[0] = -GO_SPEED*Scale - labyrinth.wd*0.5;
		labyrinth.wheel_speed[1] = -(-GO_SPEED*Scale + labyrinth.wd*0.5);
		return;
	}
	labyrinth.wheel_speed[0] = 0;
	labyrinth.wheel_speed[1] = 0;
}

//初始化参数
void labyrinth_init(void)
{
	labyrinth.wd = 0;
	labyrinth.wheel_speed[0] = 0;
	labyrinth.wheel_speed[1] = 0;
	labyrinth.l_infra_num = 0;
	labyrinth.r_infra_num = 0;
	labyrinth.motor_info = get_motor_info();
	
	PID_init(&labyrinth.pwm_pid[0], PID_POSITION, labyrinth_pwm_pid[0], 1000, 400);
	PID_init(&labyrinth.pwm_pid[1], PID_POSITION, labyrinth_pwm_pid[1], 1000, 400);
}


static void set_labyrinth_wheel_pwm(void)
{
	labyrinth.wheel_pwm[0] = PID_calc(&labyrinth.pwm_pid[0], labyrinth.wheel_speed[0], labyrinth.motor_info->speed[0]*1.0/0.52);//13/(10+0xf)
	labyrinth.wheel_pwm[1] = -PID_calc(&labyrinth.pwm_pid[1], labyrinth.wheel_speed[1], labyrinth.motor_info->speed[1]*1.0/0.52);
}

//跟随循环任务
void labyrinth_loop(void)
{
	set_labyrinth_wd();
	set_labyrinth_wheel_speed();
	set_labyrinth_wheel_pwm();
}

const labyrinth_t* get_labyrinth_info(void)
{
	return &labyrinth;
}
