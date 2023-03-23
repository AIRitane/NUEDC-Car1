#include "collide_key.h"

uint8_t collide_key = 0;

static void key_delay(uint16_t t);

void collide_key_init(void) //IO��ʼ��
{
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0 + GPIO_PIN2+ GPIO_PIN3 + 
																													 GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7);

		/* Configuring P1.1 P1.4 as an input and enabling interrupts */
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4,  GPIO_PIN0); //�����жϱ�־
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4 , GPIO_PIN2); //�����жϱ�־
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4,  GPIO_PIN3); //�����жϱ�־
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4 , GPIO_PIN5); //�����жϱ�־
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4,  GPIO_PIN6); //�����жϱ�־
		MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4 , GPIO_PIN7); //�����жϱ�־

		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN0);    //ʹ���ж϶˿�
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN2);    //ʹ���ж϶˿�
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN3);    //ʹ���ж϶˿�
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN5);    //ʹ���ж϶˿�
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN6);    //ʹ���ж϶˿�
		MAP_GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN7);    //ʹ���ж϶˿�
	
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN0,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN2,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN3,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN5,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN6,GPIO_LOW_TO_HIGH_TRANSITION);
		GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN7,GPIO_LOW_TO_HIGH_TRANSITION);
	
		MAP_Interrupt_enableInterrupt(INT_PORT4);             //�˿���ʹ��

		/* Enabling SRAM Bank Retention */
		MAP_SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK4);
}

void PORT4_IRQHandler(void)
{
	uint32_t status;

	status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P4); //��ȡ�ж�״̬
	MAP_GPIO_clearInterruptFlag(GPIO_PORT_P4, status);         //�����жϱ�־
	key_delay(25);                                             //ȥ����

	if (status & GPIO_PIN0) //��ӦP1.1
	{
			if(BITBAND_PERI(P4IN, 0) == 0)
			{
				collide_key|=0x01;
			}
	}
	if (status & GPIO_PIN2) //��ӦP1.4
	{
		if(BITBAND_PERI(P4IN, 2) == 0)
		{
			collide_key|=0x02;
		}
	}
	if (status & GPIO_PIN3) //��ӦP1.4
	{
		if(BITBAND_PERI(P4IN, 3) == 0)
		{
			collide_key|=0x04;
		}
	}
	if (status & GPIO_PIN5) //��ӦP1.4
	{
		if(BITBAND_PERI(P4IN, 5) == 0)
		{
			collide_key|=0x08;
		}
	}
	if (status & GPIO_PIN6) //��ӦP1.4
	{
		if(BITBAND_PERI(P4IN, 6) == 0)
		{
			collide_key|=0x10;
		}
	}
	if (status & GPIO_PIN7) //��ӦP1.4
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