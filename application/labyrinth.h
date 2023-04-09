#ifndef __LABYRINTH_H
#define __LABYRINTH_H

#include <stdint.h>
#include "motor.h"
#include "pid.h"

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
}labyrinth_t;

const labyrinth_t* get_labyrinth_info(void);
uint8_t labyrinth_loop(uint8_t stop_flag);
void labyrinth_init(void);

#endif
