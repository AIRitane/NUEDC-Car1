#include "fsm.h"
//static void fsm_set_mode();
static void fsm_set_speed(void);
//static void fsm_data_loop();

void fsm_init(void);
void fsm_loop(void);

car_t car;

void fsm_init(void)
{
	motor_init();
	follow_line_init();
	labyrinth_init();
	
	//pid³õÊ¼»¯
	
	car.mode = NOMODE;
	car.motor_status = NOFORCE;
	car.motor_info = get_motor_info();
	car.follow_line_info = get_follow_line_info();
	car.labyrinth_info = get_labyrinth_info();
	car.set_pwm[0] = 0;
	car.set_pwm[1] = 0;
}

#include "delay.h"
void fsm_loop(void)
{
	if(car.mode == NOMODE)
	{
		car.set_pwm[0] = 0;
		car.set_pwm[1] = 0;
	}
	else if(car.mode == FOLLOW_LINE)
	{
		follow_line_loop();
		car.set_pwm[0] = car.follow_line_info->wheel_pwm[0];
		car.set_pwm[1] = car.follow_line_info->wheel_pwm[1];
	}
	else if(car.mode == LABYRINTH)
	{
		labyrinth_loop();
		car.set_pwm[0] = car.labyrinth_info->wheel_pwm[0];
		car.set_pwm[1] = car.labyrinth_info->wheel_pwm[1];
	}
	else
	{
		car.set_pwm[0] = 0;
		car.set_pwm[1] = 0;
	}
	fsm_set_speed();
}

static void fsm_set_speed(void)
{
	
	motor_set_speed(car.set_pwm);
}













