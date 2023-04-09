#include "bsp_key.h"

task_t TASK_STATUS;

void key_init(void) //IO≥ı ºªØ
{
  MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN4);
	MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN0);
}

void key_loop(void)
{
	static uint8_t flag = 1;
	if(!(MAP_GPIO_getInputPinValue(GPIO_PORT_P6,GPIO_PIN0)) && flag)
	{
		TASK_STATUS = NONE_TASK;
	}
	else
	{
		flag = 0;
		if(MAP_GPIO_getInputPinValue(GPIO_PORT_P6,GPIO_PIN4))
		{
			TASK_STATUS = TASK1;
		}
		else
		{
			TASK_STATUS = TASK2;
		}
	}
	
}
task_t get_task_status(void)
{
		return TASK_STATUS;
}
