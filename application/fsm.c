#include "fsm.h"
#include <stdio.h>
#include "oled.h"
#include "bsp_key.h"

//static void fsm_set_mode();
static void fsm_set_speed(void);
//static void fsm_data_loop();

void fsm_init(void);
void fsm_loop(void);

car_t car;

void fsm_init(void)
{
	key_init();
	motor_init();
	follow_line_init();
	
	//pid³õÊ¼»¯
	
	car.mode = FOLLOW_LINE;
	car.motor_status = NOFORCE;
	car.motor_info = get_motor_info();
	car.follow_line_info = get_follow_line_info();
	
	car.set_pwm[0] = 0;
	car.set_pwm[1] = 0;
}

void fsm_loop(void)
{
	key_loop();
	if(get_task_status() == NONE_TASK)
	{
		car.set_pwm[0] = 0;
		car.set_pwm[1] = 0;
	}
	else if(get_task_status() == TASK1)
	{
		if(car.mode == FOLLOW_LINE)
		{
			follow_line_loop();
			car.set_pwm[0] = car.follow_line_info->wheel_pwm[0];
			car.set_pwm[1] = car.follow_line_info->wheel_pwm[1];
			if(car.follow_line_info->adc14_vaule[0]*3.3/16384 <=1.4 && car.follow_line_info->adc14_vaule[1]*3.3/16384 <=1.4)
			{
				car.mode = NOMODE;
				car.set_pwm[0] = 0;
				car.set_pwm[1] = 0;
			}
		}
	}
	else if(get_task_status() == TASK2)
	{
		if(car.mode == FOLLOW_LINE)
		{
			follow_line_loop();
			
			car.set_pwm[0] = car.follow_line_info->wheel_pwm[0];
			car.set_pwm[1] = car.follow_line_info->wheel_pwm[1];
		}
	}
	
	fsm_set_speed();
}

static void fsm_set_speed(void)
{
	motor_set_speed(car.set_pwm);
}













