#include "motor.h"
#include "math.h"

motor_t motor;

#define ABS(x) ((x)>=0?(x):-(x))

//PWM设置为1KHz 占空比0-1000可调
#define MOTOR_PSC	48				//电机PWM预分频
#define MOTOR_CCR0	1000		//电机PWM重装载计数值
#define MOTOR_PERIMETER  1	//轮子周长
#define CYCLE_TURNS 10			//单位圈数内编码器的脉冲数量
#define TA2_FREQ	100				//测频法频率

static void coder_init(void);

static void motor_pwm_init(uint16_t ccr0, uint16_t psc)
{
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
	MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

	Timer_A_PWMConfig TimA0_PWMConfig;
	/*定时器PWM初始化*/
	TimA0_PWMConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;             //时钟源
	TimA0_PWMConfig.clockSourceDivider = psc;                            //时钟分频 范围1-64
	TimA0_PWMConfig.timerPeriod = ccr0;                                  //自动重装载值（ARR）
	TimA0_PWMConfig.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3; //通道一 （引脚定义）
	TimA0_PWMConfig.compareOutputMode = TIMER_A_OUTPUTMODE_TOGGLE_SET;   //输出模式
	TimA0_PWMConfig.dutyCycle = 0;                                    //这里是改变占空比的地方 默认100%

	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TimA0_PWMConfig); /* 初始化比较寄存器以产生 PWM1 */
	
	TimA0_PWMConfig.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4; //通道一 （引脚定义）
	MAP_Timer_A_generatePWM(TIMER_A0_BASE, &TimA0_PWMConfig); /* 初始化比较寄存器以产生 PWM1 */
	
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
    // 1.增计数模式初始化
    Timer_A_UpModeConfig upConfig;
    upConfig.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;                                      //时钟源
    upConfig.clockSourceDivider = 48;                                                      //时钟分频 范围1-64
    upConfig.timerPeriod = 1000000/TA2_FREQ;                                                          //自动重装载值（ARR）
    upConfig.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;                   //禁用 tim溢出中断
    upConfig.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE; //启用 ccr0更新中断
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
* @brief 初始化电机相关
* @param 
* @retval 
* @note pwm最高为250KHz 设置为1KHz
*/  
void motor_init(void)
{
	//设置为1KHz，占空比0-1000可调
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
* @brief 设置电机速度
* @param set_speed 速度设定值，范围[-1000,1000]
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
* @brief 初始化编码器
* @param 
* @retval 
* @note 
*/ 
static void coder_init(void)
{
	GPIO_setAsInputPin(GPIO_PORT_P9, GPIO_PIN2);
	GPIO_setAsInputPin(GPIO_PORT_P10, GPIO_PIN5);
	
		 // 1.复用输出
		MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN2 , GPIO_PRIMARY_MODULE_FUNCTION);
		MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10, GPIO_PIN4 , GPIO_PRIMARY_MODULE_FUNCTION);

    /* 定时器配置参数*/
    Timer_A_ContinuousModeConfig continuousModeConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,      // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_48, // SMCLK/48 = 1MHz
        TIMER_A_TAIE_INTERRUPT_DISABLE,  // 关闭定时器溢出中断
        TIMER_A_DO_CLEAR               	// Clear Counter
    };
    // 3.将定时器初始化为连续计数模式
    MAP_Timer_A_configureContinuousMode(TIMER_A3_BASE, &continuousModeConfig);

    // 4.配置捕捉模式结构体 */
    Timer_A_CaptureModeConfig captureModeConfig_TA3 = {
        TIMER_A_CAPTURECOMPARE_REGISTER_0,           //在这里改引脚
        TIMER_A_CAPTUREMODE_RISING_EDGE, 						 //上升沿捕获
        TIMER_A_CAPTURE_INPUTSELECT_CCIxA,           //CCIxA:外部引脚输入  （CCIxB:与内部ACLK连接(手册)
        TIMER_A_CAPTURE_SYNCHRONOUS,                 //同步捕获
        TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,     //开启CCRN捕获中断
        TIMER_A_OUTPUTMODE_OUTBITVALUE               //输出位值
    };
    // 5.初始化定时器的捕获模式
    MAP_Timer_A_initCapture(TIMER_A3_BASE, &captureModeConfig_TA3);
		captureModeConfig_TA3.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
		MAP_Timer_A_initCapture(TIMER_A3_BASE, &captureModeConfig_TA3);
		
    // 6.选择连续模式计数开始计数
    MAP_Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_CONTINUOUS_MODE);

    // 7.清除中断标志位
    MAP_Timer_A_clearInterruptFlag(TIMER_A3_BASE);                                   //清除定时器溢出中断标志位
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0); //清除 CCR1 更新中断标志位
		MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2); //清除 CCR1 更新中断标志位
		
    // 8.开启定时器端口中断
    MAP_Interrupt_enableInterrupt(INT_TA3_N); //开启定时器A2端口中断
		MAP_Interrupt_enableInterrupt(INT_TA3_0); //开启定时器A2端口中断
	
}



/**
* @brief A3 0通道中断服务函数
* @param 
* @retval 
* @note 用于计算圈数
*/ 
void TA3_0_IRQHandler(void)
{
	if(MAP_Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0)) //捕获中断
	{
		MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0); //清除 CCR1 更新中断标志位
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
* @brief A3 2通道中断服务函数
* @param 
* @retval 
* @note 用于计算圈数
*/ 
void TA3_N_IRQHandler(void)
{
	if (MAP_Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2)) //捕获中断
	{
			MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_2); //清除 CCR1 更新中断标志位
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




