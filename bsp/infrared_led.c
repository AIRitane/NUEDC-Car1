#include "infrared_led.h"
#include "stdint.h"
#include "delay.h"

#define INF_DEBUG_TIME 500


/**
* @brief ��ȡ����Թ�
* @param 
* @retval ����Թ�״̬
* @note �˸�����Թ�ӳ�䵽8BIT
*/  
uint8_t infrared_sensor_read(void)
{
	uint8_t result;
	
	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
	//P5.3����ߵ�ƽ,�򿪺��ⷢ���
	GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
	//P7.0-7.7��������Ϊ���
	GPIO_setAsOutputPin(GPIO_PORT_P7, PIN_ALL8);
	//����P7.0-7.7����ߵ�ƽ
	GPIO_setOutputHighOnPin(GPIO_PORT_P7, PIN_ALL8);
	//��ʱ10us
	delay_us(10);
	//P7.0-7.7��������Ϊ����
	GPIO_setAsInputPin(GPIO_PORT_P7, PIN_ALL8);
	//��ʱ�ض�ʱ��
	delay_us(INF_DEBUG_TIME);
	//��ȡP7.0-7.7������ֵ
	result = P7->IN;

	//P5.3����͵�ƽ,�رպ��ⷢ���
	GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
	//���ض˿ڶ�ȡֵ
	return result;
}
