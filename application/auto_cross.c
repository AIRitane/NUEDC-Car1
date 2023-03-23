#include "auto_cross.h"
#include "collide_key.h"
#include "delay.h"

//默认左转
//void car_turn(uint32_t set_turns,int8_t scale);

#define GO_SPEED			13

auto_cross_t auto_cross;
float auto_cross_pwm_pid[2][3]={{100,1,0},
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
static void set_auto_cross_wd(void)
{
	uint8_t collide_key  = 0;
	collide_key = get_collide_key_status();
	auto_cross.l_key = ((collide_key>>1)&0x04)|((collide_key>>3)&0x02)|((collide_key>>5)&0x01);
	auto_cross.r_key = collide_key&0x07;
}
/*********************************这里需要场地！！！！********************************************/
//设置轮子速度
static void set_auto_cross_wheel_speed(void)
{
	static uint8_t flag = 0;
	if(auto_cross.l_key || auto_cross.r_key)
	{
		if(flag == 0)
		{
			car_back_straight(50);
			car_turn_l_90_degree(140);
			flag = 1;
		}
		else if(flag == 1)
		{
			car_back_straight(50);
			car_turn_r_90_degree(140);
			flag = 0;
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

//跟随循环任务
void auto_cross_loop(void)
{
	set_auto_cross_wd();
	set_auto_cross_wheel_speed();
	set_auto_cross_wheel_pwm();
}


/*************************对外接口*******************************/
const auto_cross_t* get_auto_cross_info(void)
{
	return &auto_cross;
}

