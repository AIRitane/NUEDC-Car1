#ifndef __FSM_H
#define __FSM_H

#include <stdint.h>
#include "motor.h"
#include "follow_line.h"

typedef enum
{
	NOMODE = 0,
	FOLLOW_LINE,
	LABYRINTH,
	AUTOCROSS,
	ABOVE_3,
	CHARGE,
}mode_e;

typedef enum
{
	NOFORCE,
	NORMAL,
	STOP,
}motor_status_e;

typedef struct
{
	mode_e mode;
	motor_status_e motor_status;
	const motor_t *motor_info;
	const follow_line_t* follow_line_info;
	
	int32_t set_pwm[2];
}car_t;

extern car_t car;

void fsm_init(void);
void fsm_loop(void);

#endif
