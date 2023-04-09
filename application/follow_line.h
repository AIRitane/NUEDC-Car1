#ifndef __FOLLOW_LINE_H
#define __FOLLOW_LINE_H

#include <stdint.h>
#include "pid.h"
#include "motor.h"

typedef struct
{
	float wd; //²îËÙ
	const uint16_t * adc14_vaule;
	
	float wheel_speed[2];
	float wheel_pwm[2];
	const motor_t *motor_info;
	pid_type_def pwm_pid[2];
	pid_type_def pwm_pid_wd;
	float set_wd;
}follow_line_t;


void follow_line_loop(void);
void follow_line_init(void);
const follow_line_t* get_follow_line_info(void);
#endif
