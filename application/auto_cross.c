#include "auto_cross.h"
#include "collide_key.h"
#include "delay.h"
#include "infrared_led.h"

//默认左转
//void car_turn(uint32_t set_turns,int8_t scale);

#define GO_SPEED			10

auto_cross_t auto_cross;
float auto_cross_pwm_pid[2][3]={{100,0,0},
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

//初始化参数
void auto_cross_init(void)
{
	collide_key_init();
	auto_cross.wd = 0;
	auto_cross.wheel_speed[0] = 0;
	auto_cross.wheel_speed[1] = 0;
	auto_cross.motor_info = get_motor_info();
	
	PID_init(&auto_cross.pwm_pid[0], PID_POSITION, auto_cross_pwm_pid[0], 1000, 400);
	PID_init(&auto_cross.pwm_pid[1], PID_POSITION, auto_cross_pwm_pid[1], 1000, 400);
}

//设置跟随差速
static uint8_t set_auto_cross_wd(void)
{
	uint8_t collide_key  = 0;
	collide_key = get_collide_key_status();
	auto_cross.l_key = ((collide_key>>1)&0x04)|((collide_key>>3)&0x02)|((collide_key>>5)&0x01);
	auto_cross.r_key = collide_key&0x07;
	auto_cross.infra_num = get_bit_num(infrared_sensor_read());
	
	return auto_cross.infra_num;
}
/*********************************这里需要场地！！！！********************************************/
//设置轮子速度
static void set_auto_cross_wheel_speed(void)
{
	if(auto_cross.l_key || auto_cross.r_key)
	{
		if(auto_cross.l_key & 0x04)
		{
			car_back_straight(50);
			car_turn_r_90_degree(180);
		}
		else if(auto_cross.l_key & 0x02)
		{
			car_back_straight(50);
			car_turn_r_90_degree(140);
		}
		else if(auto_cross.l_key & 0x01)
		{
			car_back_straight(130);
			car_turn_r_90_degree(100);
		}
		else if(auto_cross.r_key & 0x04)
		{
			car_back_straight(50);
			car_turn_l_90_degree(180);
		}
		else if(auto_cross.r_key & 0x02)
		{
			car_back_straight(50);
			car_turn_l_90_degree(140);
		}
		else if(auto_cross.r_key & 0x01)
		{
			car_back_straight(130);
			car_turn_l_90_degree(100);
		}
	}
	else
	{
		auto_cross.wheel_speed[0] = -GO_SPEED;
		auto_cross.wheel_speed[1] = GO_SPEED;
	}
}

static void set_auto_cross_wheel_pwm(void)
{
	auto_cross.wheel_pwm[0] = PID_calc(&auto_cross.pwm_pid[0], auto_cross.wheel_speed[0], auto_cross.motor_info->speed[0]*1.0/0.52);//13/(10+0xf)
	auto_cross.wheel_pwm[1] = -PID_calc(&auto_cross.pwm_pid[1], auto_cross.wheel_speed[1], auto_cross.motor_info->speed[1]*1.0/0.52);
}

//循环任务
uint8_t auto_cross_loop(void)
{
	uint8_t ret = 0;
	ret = set_auto_cross_wd();
	set_auto_cross_wheel_speed();
	set_auto_cross_wheel_pwm();
	
	return ret;
}


/*************************对外接口*******************************/
const auto_cross_t* get_auto_cross_info(void)
{
	return &auto_cross;
}

