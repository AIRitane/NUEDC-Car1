#include "follow_line.h"
#include "motor.h"
#include "infrared_led.h"


#define FOLLOW_SPEED	1
#define GO_SPEED			10

follow_line_t follow_line;
float pwm_pid[2][3]={{100,1,0},
										 {100,1,0},
										 };

//设置跟随差速
static void set_follow_line_wd(void)
{
	uint8_t infrared = 0;
	infrared = infrared_sensor_read();
	
	follow_line.l_infra = infrared>>4;
	follow_line.r_infra = ((infrared<<3)&0x0f)|((infrared<<1)&0x08)|((infrared>>3)&0x01)|((infrared>>1)&0x02);
	
	follow_line.wd = follow_line.l_infra - follow_line.r_infra;
}

//设置轮子速度
static void set_follow_line_wheel_speed(void)
{
	follow_line.wheel_speed[0] = (-GO_SPEED - follow_line.wd)*FOLLOW_SPEED;
	follow_line.wheel_speed[1] = (-(-GO_SPEED + follow_line.wd))*FOLLOW_SPEED;
}

//初始化参数
void follow_line_init(void)
{
	follow_line.wd = 0;
	follow_line.wheel_speed[0] = 0;
	follow_line.wheel_speed[1] = 0;
	follow_line.motor_info = get_motor_info();
	
	PID_init(&follow_line.pwm_pid[0], PID_POSITION, pwm_pid[0], 1000, 400);
	PID_init(&follow_line.pwm_pid[1], PID_POSITION, pwm_pid[1], 1000, 400);
}

static void set_follow_line_wheel_pwm(void)
{
	follow_line.wheel_pwm[0] = PID_calc(&follow_line.pwm_pid[0], follow_line.wheel_speed[0], follow_line.motor_info->speed[0]*1.0/0.52);//13/(10+0xf)
	follow_line.wheel_pwm[1] = -PID_calc(&follow_line.pwm_pid[1], follow_line.wheel_speed[1], follow_line.motor_info->speed[1]*1.0/0.52);
//follow_line.wheel_pwm[0] = PID_calc(&follow_line.pwm_pid[0], 3, follow_line.motor_info->speed[0]*1.0/0.52);//13/(10+0xf)
//	follow_line.wheel_pwm[1] = -PID_calc(&follow_line.pwm_pid[1], 3, follow_line.motor_info->speed[1]*1.0/0.52);
}


//跟随循环任务
void follow_line_loop(void)
{
	set_follow_line_wd();
	set_follow_line_wheel_speed();
	set_follow_line_wheel_pwm();
}


/*************************对外接口*******************************/
const follow_line_t* get_follow_line_info(void)
{
	return &follow_line;
}