#include "follow_line.h"
#include "motor.h"
#include "infrared_led.h"

#define GO_SPEED			20

follow_line_t follow_line;
float follow_line_pwm_pid[2][3]={{100,1,0},
																 {100,1,0},
																 };

static uint8_t get_bit_num(uint8_t num)
{
	uint8_t count =0;
	
	for(uint8_t i =0;i<8;i++)
	{
		if(num&0x01)
			count++;
		num = num>>1;
	}
	
	return count;
}

//���ø������
static void set_follow_line_wd(void)
{
	uint8_t infrared = 0;
	infrared = infrared_sensor_read();
	follow_line.l_infra_num = get_bit_num(infrared>>4);
	follow_line.r_infra_num = get_bit_num(infrared&0x0f);
	follow_line.infra_num = follow_line.l_infra_num+follow_line.r_infra_num;
	
	follow_line.l_infra = infrared>>4;
	follow_line.r_infra = ((infrared<<3)&0x10)|((infrared<<1)&0x08)|((infrared>>3)&0x01)|((infrared>>1)&0x02);
	
	follow_line.wd = follow_line.l_infra - follow_line.r_infra;
}

//���������ٶ�
static void set_follow_line_wheel_speed(void)
{
	float Scale = 1;
	
	//�ܷ�ԭ��תȦ
	if(follow_line.infra_num == 0)
	{
		Scale = -1;
	}
	else if(follow_line.infra_num>=3 || follow_line.infra_num <= 5) Scale = 0.6;
	else if(follow_line.infra_num<=2) Scale = 1;
	else Scale = 0.3;
	
	follow_line.wheel_speed[0] = -GO_SPEED*Scale - follow_line.wd*2;
	follow_line.wheel_speed[1] = -(-GO_SPEED*Scale + follow_line.wd*2);
}

//��ʼ������
void follow_line_init(void)
{
	follow_line.wd = 0;
	follow_line.wheel_speed[0] = 0;
	follow_line.wheel_speed[1] = 0;
	follow_line.motor_info = get_motor_info();
	
	PID_init(&follow_line.pwm_pid[0], PID_POSITION, follow_line_pwm_pid[0], 1000, 400);
	PID_init(&follow_line.pwm_pid[1], PID_POSITION, follow_line_pwm_pid[1], 1000, 400);
}

static void set_follow_line_wheel_pwm(void)
{
	follow_line.wheel_pwm[0] = PID_calc(&follow_line.pwm_pid[0], follow_line.wheel_speed[0], follow_line.motor_info->speed[0]*1.0/0.52);//13/(10+0xf)
	follow_line.wheel_pwm[1] = -PID_calc(&follow_line.pwm_pid[1], follow_line.wheel_speed[1], follow_line.motor_info->speed[1]*1.0/0.52);
}

//����ѭ������
void follow_line_loop(void)
{
	set_follow_line_wd();
	set_follow_line_wheel_speed();
	set_follow_line_wheel_pwm();
}


/*************************����ӿ�*******************************/
const follow_line_t* get_follow_line_info(void)
{
	return &follow_line;
}
