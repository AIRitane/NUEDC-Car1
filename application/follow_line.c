#include "follow_line.h"
#include "motor.h"
#include "adc.h"

//#define GO_SPEED			20
float GO_SPEED = -16;
follow_line_t follow_line;
float follow_line_speed_pid[2][3]={{100,0,0},
																 {100,0,0},
																 };

float follow_line_wd_pid[3]={8,0,0};

//设置跟随差速
static void set_follow_line_wd(void)
{	
	follow_line.wd = ((follow_line.adc14_vaule[0] - follow_line.adc14_vaule[1])* 3.3)/16384+0.01;
}

//设置轮子速度
static void set_follow_line_wheel_speed(void)
{
	float scale = 0;
	
	follow_line.set_wd = PID_calc(&follow_line.pwm_pid_wd, follow_line.wd, 0);
	scale = 1.0;///(1+0.03*__fabs(follow_line.set_wd));
	follow_line.wheel_speed[0] = -(GO_SPEED*scale + follow_line.set_wd);
	follow_line.wheel_speed[1] = (GO_SPEED*scale - follow_line.set_wd);
}

//初始化参数
void follow_line_init(void)
{
	
	follow_line.wd = 0;
	follow_line.wheel_speed[0] = 0;
	follow_line.wheel_speed[1] = 0;
	follow_line.motor_info = get_motor_info();
	follow_line.adc14_vaule = get_adc14_vaule();
	
	PID_init(&follow_line.pwm_pid[0], PID_POSITION, follow_line_speed_pid[0], 1000, 400);
	PID_init(&follow_line.pwm_pid[1], PID_POSITION, follow_line_speed_pid[1], 1000, 400);
	PID_init(&follow_line.pwm_pid_wd, PID_POSITION, follow_line_wd_pid, __fabs(GO_SPEED), 0);
}

static void set_follow_line_wheel_pwm(void)
{
	follow_line.wheel_pwm[0] = PID_calc(&follow_line.pwm_pid[0], follow_line.wheel_speed[0], follow_line.motor_info->speed[0]*1.0/0.52);
	follow_line.wheel_pwm[1] = -PID_calc(&follow_line.pwm_pid[1], follow_line.wheel_speed[1], follow_line.motor_info->speed[1]*1.0/0.52);
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
