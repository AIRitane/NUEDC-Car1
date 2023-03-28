#ifndef __FOLLOW_LINE_H
#define __FOLLOW_LINE_H

#include <stdint.h>
#include "pid.h"
#include "motor.h"

typedef struct
{
	float wd; //����
	uint8_t l_infra;//������Թ�ӳ��
	uint8_t r_infra;//�Ҳ����Թ�ӳ��
	uint8_t l_infra_num;//����Թ���Ŀ
	uint8_t r_infra_num;//����Թ���Ŀ
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
