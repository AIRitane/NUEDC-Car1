#include "labyrinth.h"
#include "infrared_led.h"

#include "oled.h"
#include "stdio.h"
#include "string.h"



#define GO_SPEED			9
#define TURN_TIME  		500
#define TURN_SPEED  		7
labyrinth_t labyrinth;

float labyrinth_pwm_pid[2][3]={{100,0,0},
															 {100,0,0},
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

//���ø������
static uint8_t set_labyrinth_wd(void)
{
	uint8_t infrared = 0;
	infrared = infrared_sensor_read();
	labyrinth.l_infra_num = get_bit_num(infrared>>4);
	labyrinth.r_infra_num = get_bit_num(infrared&0x0f);
	labyrinth.infra_num = labyrinth.l_infra_num+labyrinth.r_infra_num;
	
	labyrinth.l_infra = infrared>>4;
	labyrinth.r_infra = ((infrared<<3)&0x10)|((infrared<<1)&0x08)|((infrared>>3)&0x01)|((infrared>>1)&0x02);
	
	labyrinth.wd = labyrinth.l_infra - labyrinth.r_infra;
	
	return labyrinth.infra_num;
}

#include "delay.h"
//���������ٶ�
static void set_labyrinth_wheel_speed(void)
{
	
	float Scale = 1;
	static int8_t left_flag = 0;

	//ʮ�ֺ�T��
	if(left_flag <= 0) left_flag = 0;
	if(labyrinth.l_infra_num>=3&&labyrinth.r_infra_num>=3)
	{
		left_flag--;
		car_turn_r_90_degree(100);
	}
	//-|��·��
	else if(labyrinth.l_infra_num>=4)
	{
		left_flag = 8;
		car_go_straight(25);
	}
	//��·
	else if(labyrinth.l_infra_num==0&& labyrinth.r_infra_num==0 && left_flag>=1)
	{
		car_turn_l_90_degree(90);
		left_flag = 0;
	}
	//|-��·��
	else if(labyrinth.r_infra_num>=3)
	{
		left_flag--;
		car_turn_r_90_degree(100);
		
	}
	else
	{
		left_flag--;
		//�ܷ�ԭ��תȦ
		if(labyrinth.infra_num == 0)
		{
			car_turn_l_90_degree(220);
			labyrinth.wheel_speed[0] = 0;
			labyrinth.wheel_speed[1] = 0;
			return;
		}
		if(labyrinth.infra_num <=2 && (labyrinth.l_infra_num &0x01 || labyrinth.r_infra_num&0x01)) Scale = 1;
		else if(labyrinth.infra_num<=4) Scale = 0.9;
		else if(labyrinth.infra_num>=5 && labyrinth.infra_num <= 6) Scale = 0.7;
		else Scale = 0.2;
		if(!(labyrinth.l_infra_num &0x01 || labyrinth.r_infra_num&0x01)) Scale *= 0.85;
		
		labyrinth.wheel_speed[0] = -GO_SPEED*Scale - labyrinth.wd*0.5;
		labyrinth.wheel_speed[1] = -(-GO_SPEED*Scale + labyrinth.wd*0.5);
		return;
	}

	labyrinth.wheel_speed[0] = 0;
	labyrinth.wheel_speed[1] = 0;
}

//��ʼ������
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

//����ѭ������
uint8_t labyrinth_loop(uint8_t stop_flag)
{
	uint8_t ret = 0;
	ret = set_labyrinth_wd();
	static uint8_t stop = 0;
	
	stop &= stop_flag;
	if(labyrinth.r_infra_num ==3 && stop!=2)
		stop=1;
	else if(stop==1 && (labyrinth.l_infra==0x0e || labyrinth.l_infra==0x0f || labyrinth.l_infra==0x0c || labyrinth.l_infra&0x08 || labyrinth.l_infra_num>=3))
	{
			car_turn_l_90_degree(130);
			stop = 2;
			labyrinth.wheel_pwm[0] = 0;
			labyrinth.wheel_pwm[1] = 0;
			return 128;
	}
	else if(stop != 2)
	{
		stop = 0;
	}
	
	if(stop == 2)
	{
		labyrinth.wheel_pwm[0] = 0;
		labyrinth.wheel_pwm[1] = 0;
		return 128;
	}
	
	set_labyrinth_wheel_speed();
	set_labyrinth_wheel_pwm();
	return ret;
}

const labyrinth_t* get_labyrinth_info(void)
{
	return &labyrinth;
}
