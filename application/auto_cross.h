#ifndef __AUTO_CROSS_H
#define __AUTO_CROSS_H

#include <stdint.h>
#include "motor.h"
#include "pid.h"

typedef struct
{
	float wd; //差速
	uint8_t l_key;//红外对管数目
	uint8_t r_key;//红外对管数目
	uint8_t infra_num;
	
	float wheel_speed[2];
	float wheel_pwm[2];
	const motor_t *motor_info;
	pid_type_def pwm_pid[2];
}auto_cross_t;

void auto_cross_init(void);
uint8_t auto_cross_loop(void);
const auto_cross_t* get_auto_cross_info(void);

#endif
