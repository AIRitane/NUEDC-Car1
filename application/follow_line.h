#ifndef __FOLLOW_LINE_H
#define __FOLLOW_LINE_H

#include <stdint.h>
#include "pid.h"
#include "motor.h"

typedef struct
{
	float wd; //差速
	uint8_t l_infra;//左侧红外对管映射
	uint8_t r_infra;//右侧红外对管映射
	uint8_t l_infra_num;//红外对管数目
	uint8_t r_infra_num;//红外对管数目
	uint8_t infra_num;
	
	float wheel_speed[2];
	float wheel_pwm[2];
	const motor_t *motor_info;
	pid_type_def pwm_pid[2];
}follow_line_t;


uint8_t follow_line_loop(void);
void follow_line_init(void);
const follow_line_t* get_follow_line_info(void);

#endif
