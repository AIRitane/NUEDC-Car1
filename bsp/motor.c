#include "motor.h"
#include "math.h"

motor_t motor;

#define ABS(x) ((x)>=0?(x):-(x))

//PWM����Ϊ1KHz ռ�ձ�0-1000�ɵ�
#define MOTOR_PSC	48				//���PWMԤ��Ƶ
#define MOTOR_CCR0	1000		//���PWM��װ�ؼ���ֵ
#define MOTOR_PERIMETER  1	//�����ܳ�
#define CYCLE_TURNS 10			//��λȦ���ڱ���������������
#define TA2_FREQ	100				//��Ƶ��Ƶ��

static void coder_init(void);

static void motor_pwm_init(uint16_t ccr0, uint16_t psc)
{
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

	Timer_A_PWMConfig TimA0_PWMConfig;
	/*��ʱ��PWM��ʼ��*/
	TimA0_PWMConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;             //ʱ��Դ
	TimA0_PWMConfig.clockSourceDivider = psc;                            //ʱ�ӷ�Ƶ ��Χ1-64
	TimA0_PWMConfig.timerPeriod = ccr0;                                  //�Զ���װ��ֵ��ARR��
	TimA0_PWMConfig.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3; //ͨ��һ �����Ŷ��壩
	TimA0_PWMConfig.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_SET;   //���ģʽ
	TimA0_PWMConfig.dutyCycle = 0;                                    //�����Ǹı�ռ�ձȵĵط� Ĭ��100%

	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TimA0_PWMConfig); /* ��ʼ���ȽϼĴ����Բ��� PWM1 */
	
	TimA0_PWMConfig.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4; //ͨ��һ �����Ŷ��壩
	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TimA0_PWMConfig); /* ��ʼ���ȽϼĴ����Բ��� PWM1 */
	
	GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
}


static void motor_distance_loop(void)
{
	motor.abs_distance[0] = motor.abs_turns[0] * MOTOR_PERIMETER/CYCLE_TURNS;
	motor.abs_distance[1] = motor.abs_turns[1] * MOTOR_PERIMETER/CYCLE_TURNS;
	motor.distance[0] = motor.turns[0] * MOTOR_PERIMETER/CYCLE_TURNS;
	motor.distance[1] = motor.turns[1] * MOTOR_PERIMETER/CYCLE_TURNS;
}

static void TimA2_Int_Init(void)
{
    // 1.������ģʽ��ʼ��
    Timer_A_UpModeConfig upConfig;
    upConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;                                      //ʱ��Դ
    upConfig.clockSourceDivider = 48;                                                      //ʱ�ӷ�Ƶ ��Χ1-64
    upConfig.timerPeriod = 1000000/TA2_FREQ;                                                          //�Զ���װ��ֵ��ARR��
    upConfig.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;                   //���� tim����ж�
    upConfig.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE; //���� ccr0�����ж�
    upConfig.timerClear = TIMER_A_DO_CLEAR;                                                // Clear value

    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    MAP_Interrupt_enableInterrupt(INT_TA2_0);
}

void TA2_0_IRQHandler(void)
{
	static int32_t pre_turns[2] = {0};
	MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
	BITBAND_PERI(TIMER_A_CMSIS(TIMER_A2_BASE)->CCTL[0], TIMER_A_CCTLN_COV_OFS) = 0;
	
	motor.speed[0] = (motor.turns[0] - pre_turns[0])*TA2_FREQ/CYCLE_TURNS;
	motor.speed[1] = (motor.turns[1] - pre_turns[1])*TA2_FREQ/CYCLE_TURNS;
	
	pre_turns[0] = motor.turns[0];
	pre_turns[1] = motor.turns[1];
	motor_distance_loop();
}


/**
* @brief ��ʼ��������
* @param 
* @retval 
* @note pwm���Ϊ250KHz ����Ϊ1KHz
*/  
void motor_init(void)
{
	//����Ϊ1KHz��ռ�ձ�0-1000�ɵ�
	motor_pwm_init(1000, 48);
	GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN6);
	GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN7);
	GPIO_setAsOutputPin(GPIO_PORT_P3,GPIO_PIN6);
	GPIO_setAsOutputPin(GPIO_PORT_P3,GPIO_PIN7);
	
	GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN6);
	GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
	
	coder_init();
	TimA2_Int_Init();
}


/**
* @brief ���õ���ٶ�
* @param set_speed �ٶ��趨ֵ����Χ[-1000,1000]
* @retval 
* @note
*/  
void motor_set_speed(int32_t set_speed[2])
{
	for(uint8_t i = 0;i<2;i++)
	{
		if(set_speed[i] < 0)
		{
			MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6 + i*0x40);
			MAP_Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_3+i*0x02,ABS(set_speed[i]));
		}
		else
		{
			MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6 + i*0x40);
			MAP_Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_3+i*0x02,ABS(set_speed[i]));
		}
	}
}

/**
* @brief ��ʼ��������
* @param 
* @retval 
* @note 
*/ 
static void coder_init(void)
{
	GPIO_setAsInputPin(GPIO_PORT_P9, GPIO_PIN2);
	GPIO_setAsInputPin(GPIO_PORT_P10, GPIO_PIN5);
	
		 // 1.�������
		MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN2 , GPIO_PRIMARY_MODULE_FUNCTION);
		MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10, GPIO_PIN4 , GPIO_PRIMARY_MODULE_FUNCTION);

    /* ��ʱ�����ò���*/
    Timer_A_ContinuousModeConfig continuousModeConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,      // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_48, // SMCLK/48 = 1MHz
        TIMER_A_TAIE_INTERRUPT_DISABLE,  // �رն�ʱ������ж�
        TIMER_A_DO_CLEAR               	// Clear Counter
    };
    // 3.����ʱ����ʼ��Ϊ��������ģʽ
    MAP_Timer_A_configureContinuousMode(TIMER_A3_BASE, &continuousModeConfig);

    // 4.���ò�׽ģʽ�ṹ�� */
    Timer_A_CaptureModeConfig captureModeConfig_TA3 = {
        TIMER_A_CAPTURECOMPARE_REGISTER_0,           //�����������
        TIMER_A_CAPTUREMODE_RISING_EDGE, 						 //�����ز���
        TIMER_A_CAPTURE_INPUTSELECT_CCIxA,           //CCIxA:�ⲿ��������  ��CCIxB:���ڲ�ACLK����(�ֲ�)
        TIMER_A_CAPTURE_SYNCHRONOUS,                 //ͬ������
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,     //����CCRN�����ж�
        TIMER_A_OUTPUTMODE_OUTBITVALUE               //���λֵ
    };
    // 5.��ʼ����ʱ���Ĳ���ģʽ
    MAP_Timer_A_initCapture(TIMER_A3_BASE, &captureModeConfig_TA3);
		captureModeConfig_TA3.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
		MAP_Timer_A_initCapture(TIMER_A3_BASE, &captureModeConfig_TA3);
		
    // 6.ѡ������ģʽ������ʼ����
    MAP_Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_CONTINUOUS_MODE);

    // 7.����жϱ�־λ
    MAP_Timer_A_clearInterruptFlag(TIMER_A3_BASE);                                   //�����ʱ������жϱ�־λ
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0); //��� CCR1 �����жϱ�־λ
		MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2); //��� CCR1 �����жϱ�־λ
		
    // 8.������ʱ���˿��ж�
    MAP_Interrupt_enableInterrupt(INT_TA3_N); //������ʱ��A2�˿��ж�
		MAP_Interrupt_enableInterrupt(INT_TA3_0); //������ʱ��A2�˿��ж�
	
}



/**
* @brief A3 0ͨ���жϷ�����
* @param 
* @retval 
* @note ���ڼ���Ȧ��
*/ 
void TA3_0_IRQHandler(void)
{
	if(MAP_Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0)) //�����ж�
	{
		MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0); //��� CCR1 �����жϱ�־λ
		BITBAND_PERI(TIMER_A_CMSIS(TIMER_A3_BASE)->CCTL[0], TIMER_A_CCTLN_COV_OFS) = 0;		
		motor.abs_turns[0]++;
		if( GPIO_getInputPinValue(GPIO_PORT_P10, GPIO_PIN5))
		{
			motor.turns[0]++;
		}
		else
		{
			motor.turns[0]--;
		}
	}
}

/**
* @brief A3 2ͨ���жϷ�����
* @param 
* @retval 
* @note ���ڼ���Ȧ��
*/ 
void TA3_N_IRQHandler(void)
{
	if (MAP_Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2)) //�����ж�
	{
			MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2); //��� CCR1 �����жϱ�־λ
			BITBAND_PERI(TIMER_A_CMSIS(TIMER_A3_BASE)->CCTL[2], TIMER_A_CCTLN_COV_OFS) = 0;
		motor.abs_turns[1]++;
			if( GPIO_getInputPinValue(GPIO_PORT_P9, GPIO_PIN2))
			{
				motor.turns[1]++;
			}
			else
			{
				motor.turns[1]--;
			}
	}
}

const motor_t* get_motor_info(void)
{
	return &motor;
}




