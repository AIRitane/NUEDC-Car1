#include "collide_key.h"

uint8_t collide_key = 0;

static void key_delay(uint16_t t);

void collide_key_init(void) //IO初始化
{
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0 + GPIO_PIN2+ GPIO_PIN3 + 
																													 GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7);

		/* Configuring P1.1 P1.4 as an input and enabling interrupts */
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4,  GPIO_PIN0); //清理中断标志
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4 , GPIO_PIN2); //清理中断标志
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4,  GPIO_PIN3); //清理中断标志
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4 , GPIO_PIN5); //清理中断标志
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4,  GPIO_PIN6); //清理中断标志
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4 , GPIO_PIN7); //清理中断标志

		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN0);    //使能中断端口
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN2);    //使能中断端口
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN3);    //使能中断端口
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN5);    //使能中断端口
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN6);    //使能中断端口
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN7);    //使能中断端口
	
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN0,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN2,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN3,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN5,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN6,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN7,GPIO_LOW_TO_HIGH_TRANSITION);
	
		MAP_Interrupt_enableInterrupt(INT_PORT4);             //端口总使能

		/* Enabling SRAM Bank Retention */
		MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK4);
}

void PORT4_IRQHandler(void)
{
	uint32_t status;

	status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P4); //获取中断状态
	MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4, status);         //清理中断标志
	key_delay(25);                                             //去抖动

	if (status & GPIO_PIN0) //对应P1.1
	{
			if(BITBAND_PERI(P4IN, 0) == 0)
			{
				collide_key|=0x01;
			}
	}
	if (status & GPIO_PIN2) //对应P1.4
	{
		if(BITBAND_PERI(P4IN, 2) == 0)
		{
			collide_key|=0x02;
		}
	}
	if (status & GPIO_PIN3) //对应P1.4
	{
		if(BITBAND_PERI(P4IN, 3) == 0)
		{
			collide_key|=0x04;
		}
	}
	if (status & GPIO_PIN5) //对应P1.4
	{
		if(BITBAND_PERI(P4IN, 5) == 0)
		{
			collide_key|=0x08;
		}
	}
	if (status & GPIO_PIN6) //对应P1.4
	{
		if(BITBAND_PERI(P4IN, 6) == 0)
		{
			collide_key|=0x10;
		}
	}
	if (status & GPIO_PIN7) //对应P1.4
	{
		if(BITBAND_PERI(P4IN, 7) == 0)
		{
			collide_key|=0x20;
		}
	}
}

static void key_delay(uint16_t t)
{
    volatile uint16_t x;
    while (t--)
        for (x = 0; x < 1000; x++)
            ;
}

uint8_t get_collide_key_status(void)
{
	uint8_t temp = collide_key; 
	collide_key = 0;
	return temp;
}