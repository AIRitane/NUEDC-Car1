#include "follow_line.h"
#include "motor.h"
#include "infrared_led.h"

#define GO_SPEED			20

follow_line_t follow_line;
float follow_line_pwm_pid[2][3]={{100,0,0},
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

//设置跟随差速
static uint8_t set_follow_line_wd(void)
{
	uint8_t infrared = 0;
	infrared = infrared_sensor_read();
	follow_line.l_infra_num = get_bit_num(infrared>>4);
	follow_line.r_infra_num = get_bit_num(infrared&0x0f);
	follow_line.infra_num = follow_line.l_infra_num+follow_line.r_infra_num;
	
	follow_line.l_infra = infrared>>4;
	follow_line.r_infra = ((infrared<<3)&0x10)|((infrared<<1)&0x08)|((infrared>>3)&0x01)|((infrared>>1)&0x02);
	
	follow_line.wd = follow_line.l_infra - follow_line.r_infra;
	
	return follow_line.infra_num;
}

//设置轮子速度
static void set_follow_line_wheel_speed(void)
{
	float Scale_speed = 1,Scale_wd = 1;
	
	//跑飞原地转圈
	if(follow_line.infra_num == 0)
	{
		Scale_speed = -1;
	}
	else if(follow_line.infra_num <=2 && (follow_line.l_infra_num &0x01 || follow_line.r_infra_num&0x01))
	{
		Scale_speed = 0.8;
		Scale_wd = 2;
	}
	else
	{
		Scale_speed = 0.6;
		Scale_wd = 2;
	}
	
	follow_line.wheel_speed[0] = -GO_SPEED*Scale_speed - follow_line.wd*Scale_wd;
	follow_line.wheel_speed[1] = -(-GO_SPEED*Scale_speed + follow_line.wd*Scale_wd);
}

//初始化参数
void follow_line_init(void)
{
	follow_line.wd = 0;
	follow_line.wheel_speed[0] = 0;
	follow_line.wheel_speed[1] = 0;
	follow_line.motor_info = get_motor_info();
	
	PID_init(&follow_line.pwm_pid[0], PID_POSITION, follow_line_pwm_pid[0], 1000, 400);
	PID_init(&follow_line.pwm_pid[1], PID_POSITION, follow_line_pwm_pid[1], 1000, 400);
}

static void set_follow_line_wheel_pwm(void)
{
	follow_line.wheel_pwm[0] = PID_calc(&follow_line.pwm_pid[0], follow_line.wheel_speed[0], follow_line.motor_info->speed[0]*1.0/0.52);//13/(10+0xf)
	follow_line.wheel_pwm[1] = -PID_calc(&follow_line.pwm_pid[1], follow_line.wheel_speed[1], follow_line.motor_info->speed[1]*1.0/0.52);
}

//跟随循环任务
uint8_t follow_line_loop(void)
{
	uint8_t ret = 0;
	ret = set_follow_line_wd();
	set_follow_line_wheel_speed();
	set_follow_line_wheel_pwm();
	return ret;
}


/*************************对外接口*******************************/
const follow_line_t* get_follow_line_info(void)
{
	return &follow_line;
}
