#include "infrared_led.h"
#include "stdint.h"
#include "delay.h"

#define INF_DEBUG_TIME 500


/**
* @brief 读取红外对管
* @param 
* @retval 红外对管状态
* @note 八个红外对管映射到8BIT
*/  
uint8_t infrared_sensor_read(void)
{
	uint8_t result;
	
	GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
	//P5.3输出高电平,打开红外发射管
	GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
	//P7.0-7.7方向设置为输出
	GPIO_setAsOutputPin(GPIO_PORT_P7, PIN_ALL8);
	//设置P7.0-7.7输出高电平
	GPIO_setOutputHighOnPin(GPIO_PORT_P7, PIN_ALL8);
	//延时10us
	delay_us(10);
	//P7.0-7.7方向设置为输入
	GPIO_setAsInputPin(GPIO_PORT_P7, PIN_ALL8);
	//延时特定时间
	delay_us(INF_DEBUG_TIME);
	//读取P7.0-7.7的输入值
	result = P7->IN;

	//P5.3输出低电平,关闭红外发射管
	GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
	//返回端口读取值
	return result;
}
