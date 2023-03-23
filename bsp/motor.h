#ifndef __MOTOR_H
#define __MOTOR_H

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

typedef struct
{
	int32_t speed[2];			//�ٶ�
	int32_t turns[2];			//Ȧ����
	int32_t distance[2];	//���·��
	uint32_t abs_turns[2];	//����Ȧ��
	uint32_t abs_distance[2];	//����·��
}motor_t;

void motor_init(void);
void motor_set_speed(int32_t set_speed[2]);
const motor_t* get_motor_info(void);
void car_turn_r_90_degree(void);
void car_turn_l_90_degree(void);
void car_go_straight(void);
void car_back_straight(void);
void car_stop(void);
#endif
