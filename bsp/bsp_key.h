#ifndef __KEY_H
#define __KEY_H

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

typedef enum
{
	NONE_TASK,
	TASK1,
	TASK2
}task_t;

void key_init(void);
void key_loop(void);
task_t get_task_status(void);

#endif
