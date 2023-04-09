#include "spwm.h"

//����2.5 2.6 �ܽ�
//ccr0 ��0�������趨ֵ��ע��+1
//ccr0 >= ����ֵΪ�͵�ƽ
void SPWM_Init(uint16_t ccr0, uint16_t psc,uint16_t death)
{
	/*��ʼ������*/
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

	/*��ʼ��ʱ�Ӳ���*/
	Timer_A_UpDownModeConfig TimA0_UpDown_PWMConfig;
	TimA0_UpDown_PWMConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
	TimA0_UpDown_PWMConfig.clockSourceDivider = psc;
	TimA0_UpDown_PWMConfig.timerPeriod = ccr0;
	TimA0_UpDown_PWMConfig.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
	TimA0_UpDown_PWMConfig.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
	TimA0_UpDown_PWMConfig.timerClear = TIMER_A_SKIP_CLEAR;
	Timer_A_configureUpDownMode(TIMER_A0_BASE,&TimA0_UpDown_PWMConfig);
	
	Timer_A_CompareModeConfig TimerA0_CompareModeConfig;
	TimerA0_CompareModeConfig.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
	TimerA0_CompareModeConfig.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
	TimerA0_CompareModeConfig.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_SET;
	TimerA0_CompareModeConfig.compareValue = 500+1;//ռ�ձ�50%
	
	Timer_A_initCompare(TIMER_A0_BASE,&TimerA0_CompareModeConfig);
	TimerA0_CompareModeConfig.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_RESET;
	TimerA0_CompareModeConfig.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;
	
	TimerA0_CompareModeConfig.compareValue = 500+1-death; //����40ns
	Timer_A_initCompare(TIMER_A0_BASE,&TimerA0_CompareModeConfig);
	
	Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UPDOWN_MODE);
}
