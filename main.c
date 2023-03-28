/****************************************************/
// MSP432P401R
// 配置Keil独立空工程 (已将ti固件库打包至工程)
// Keil工程已配置开启FPU
// Bilibili：m-RNA
// E-mail:m-RNA@qq.com
// 创建日期:2021/9/28
/****************************************************/

#include "sysinit.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "tim32.h"

#include "infrared_led.h"
#include "motor.h"
#include "key.h"
#include "fsm.h"
#include "follow_line.h"
#include "oled.h"

uint8_t infrared = 0;
int32_t set_speed[2] = {1000,1000};
int main(void)
{
    SysInit();         // 第3讲 时钟配置
    uart_init(115200); // 第7讲 串口配置
    delay_init();      // 第4讲 滴答延时

    /*开始填充初始化代码*/
		KEY_Init(1);//开中断
		LED_Init();
		fsm_init();
		OLED_Init();
		//OLED_ShowString(0, 8,"hello",8);
	
    /*停止填充初始化代码*/
		
    printf("Hello,MSP432!\r\n");
    MAP_Interrupt_enableMaster(); // 开启总中断
    while (1)
    {
			fsm_loop();
			delay_ms(1);
    }
}


//函数功能：延时
void key_delay(uint16_t t)
{
    volatile uint16_t x;
    while (t--)
        for (x = 0; x < 1000; x++)
            ;
}

void PORT1_IRQHandler(void)
{
    uint32_t status;
		static uint32_t press_count = 0;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1); //获取中断状态
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);         //清理中断标志
    key_delay(25);                                           	 //去抖动
		
    if (status & GPIO_PIN1) //对应P1.1
    {
			press_count++;
			/*开始填充用户代码*/
			car.mode = (mode_e)(press_count%6);
			LED_Show_Staus(car.mode);
    }
    if (status & GPIO_PIN4) //对应P1.4
    {
			/*开始填充用户代码*/
			press_count = 0;
			car.mode = (mode_e)(press_count%6);
			LED_Show_Staus(car.mode);
			/*结束填充用户代码*/
    }
}
