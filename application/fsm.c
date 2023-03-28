#include "fsm.h"
#include <stdio.h>
#include "oled.h"
//static void fsm_set_mode();
static void fsm_set_speed(void);
//static void fsm_data_loop();

void fsm_init(void);
void fsm_loop(void);

car_t car;
char str_buf[200]={'\0'};

void fsm_init(void)
{
	motor_init();
	follow_line_init();
	labyrinth_init();
	auto_cross_init();
	
	//pid初始化
	
	car.mode = NOMODE;
	car.motor_status = NOFORCE;
	car.motor_info = get_motor_info();
	car.follow_line_info = get_follow_line_info();
	car.labyrinth_info = get_labyrinth_info();
	car.auto_cross_info = get_auto_cross_info();
	
	car.set_pwm[0] = 0;
	car.set_pwm[1] = 0;
}

#include "delay.h"
void fsm_loop(void)
{
	static uint8_t mode_in  = 0;
	static uint8_t mode_stop = 0;
	uint8_t infra_num = 0;
	static uint8_t flash_num = 0;
	static uint32_t first_time = 0,first_l_distance = 0,first_r_distance = 0;
	
	if(car.mode == NOMODE)
	{
		car.set_pwm[0] = 0;
		car.set_pwm[1] = 0;
	}
	else if(car.mode == FOLLOW_LINE)
	{
		//第一次进入
		if(mode_in != 1)
		{
			first_time = get_motor_sec();
			first_l_distance = car.motor_info->abs_distance[0];
			first_r_distance = car.motor_info->abs_distance[1];
			
			mode_stop = 0;
			car_go_straight(40);
			mode_in = 1;
		}
		
	
		
		//判断停止位
		infra_num = follow_line_loop();
		if(infra_num == 8 || mode_stop)
		{
			car.set_pwm[0] = 0;
			car.set_pwm[1] = 0;
			mode_stop = 1;
		}
		else
		{
			flash_num++;
			
			if(flash_num == 100)
			{
				sprintf(str_buf,"Time:%d s ", get_motor_sec()- first_time);
				OLED_ShowString(0,0,(uint8_t*)str_buf,8);
				
				sprintf(str_buf,"Velocity:%d cm/s ", ((car.motor_info->speed[0]+car.motor_info->speed[1]))/2);
				OLED_ShowString(0,1,(uint8_t*)str_buf,8);
				
				sprintf(str_buf,"Distance:%d cm ", (car.motor_info->abs_distance[0]+car.motor_info->abs_distance[1])/2-(first_l_distance+first_r_distance)/2);
				OLED_ShowString(0,2,(uint8_t*)str_buf,8);
			}
			
			
			car.set_pwm[0] = car.follow_line_info->wheel_pwm[0];
			car.set_pwm[1] = car.follow_line_info->wheel_pwm[1];
		}
	}
	else if(car.mode == LABYRINTH)
	{
		if(mode_in != 2)
		{
			first_time = get_motor_sec();
			first_l_distance = car.motor_info->abs_distance[0];
			first_r_distance = car.motor_info->abs_distance[1];
			
			mode_stop = 0;
			car_go_straight(40);
			mode_in = 2;
		}
		
		infra_num = labyrinth_loop(0xff);
		if(infra_num == 128)
		{
			car.set_pwm[0] = 0;
			car.set_pwm[1] = 0;
		}
		else
		{
			flash_num++;
			if(flash_num == 100)
			{
				flash_num = 0;
				sprintf(str_buf,"Time:%d s ", get_motor_sec()- first_time);
				OLED_ShowString(0,0,(uint8_t*)str_buf,8);
				
				sprintf(str_buf,"Velocity:%d cm/s ", ((car.motor_info->speed[0]+car.motor_info->speed[1]))/2);
				OLED_ShowString(0,1,(uint8_t*)str_buf,8);
				
				sprintf(str_buf,"Distance:%d cm ", (car.motor_info->abs_distance[0]+car.motor_info->abs_distance[1])/2-(first_l_distance+first_r_distance)/2);
				OLED_ShowString(0,2,(uint8_t*)str_buf,8);
			}
			
			
			
			car.set_pwm[0] = car.labyrinth_info->wheel_pwm[0];
			car.set_pwm[1] = car.labyrinth_info->wheel_pwm[1];
		}
	}
	else if(car.mode == AUTOCROSS)
	{
		if(mode_in !=3)
		{
			first_time = get_motor_sec();
			first_l_distance = car.motor_info->abs_distance[0];
			first_r_distance = car.motor_info->abs_distance[1];
			
			mode_stop = 0;
			car_go_straight(800);
			car_stop();
			car_turn_l_90_degree(50);
			mode_in = 3;
		}
		
		infra_num = auto_cross_loop();
		if(infra_num >=3)
		{
			mode_stop = 1;
		}
		
		if(mode_stop)
		{
			car.set_pwm[0] = 0;
			car.set_pwm[1] = 0;
		}
		else
		{
			flash_num++;
			if(flash_num == 100)
			{
				flash_num = 0;
				sprintf(str_buf,"Time:%d s ", get_motor_sec()- first_time);
				OLED_ShowString(0,0,(uint8_t*)str_buf,8);
				
				sprintf(str_buf,"Velocity:%d cm/s ", ((car.motor_info->speed[0]+car.motor_info->speed[1]))/2);
				OLED_ShowString(0,1,(uint8_t*)str_buf,8);
				
				sprintf(str_buf,"Distance:%d cm ", (car.motor_info->abs_distance[0]+car.motor_info->abs_distance[1])/2-(first_l_distance+first_r_distance)/2);
				OLED_ShowString(0,2,(uint8_t*)str_buf,8);
			}
			car.set_pwm[0] = 700;//car.auto_cross_info->wheel_pwm[0];
			car.set_pwm[1] = 700;//car.auto_cross_info->wheel_pwm[1];
		}
	}
	else if(car.mode == ABOVE_3)
	{
		static uint8_t count = 0;
		if(mode_in != 4)
		{
			first_time = get_motor_sec();
			first_l_distance = car.motor_info->abs_distance[0];
			first_r_distance = car.motor_info->abs_distance[1];
			
			mode_stop = 0;
			mode_in = 4;
			car_go_straight(40);
		}
		if(count == 0)
		{
			infra_num = follow_line_loop();
			if(infra_num == 8 || mode_stop)
			{
				car_go_straight(40);
				count +=1;
			}
			
			car.set_pwm[0] = car.follow_line_info->wheel_pwm[0];
			car.set_pwm[1] = car.follow_line_info->wheel_pwm[1];
		}
		else if(count == 1)
		{
			infra_num = follow_line_loop();
			if(infra_num == 8 || mode_stop)
			{
				car_go_straight(40);
				count +=1;
			}
			
			car.set_pwm[0] = car.follow_line_info->wheel_pwm[0];
			car.set_pwm[1] = car.follow_line_info->wheel_pwm[1];
		}
		else if(count == 2)
		{
			static uint8_t flag = 0;
			if(flag == 0)
			{
				labyrinth_loop(0x00);
				flag = 1;
			}
			else
				infra_num = labyrinth_loop(0xff);
			
			
			if(infra_num == 128)
			{
				car_go_straight(40);
				count +=1;
			}
			
			car.set_pwm[0] = car.labyrinth_info->wheel_pwm[0];
			car.set_pwm[1] = car.labyrinth_info->wheel_pwm[1];
		}
		else if(count == 3)
		{
			static uint8_t flag_3 = 0;
			if(flag_3 == 0)
			{
				car_go_straight(1000);
				car_stop();
				car_turn_l_90_degree(57);
				flag_3 = 1;
			}
			
			infra_num = auto_cross_loop();
			if(infra_num >=3)
			{
				mode_stop = 1;
			}
			
			if(mode_stop)
			{
				car.set_pwm[0] = 0;
				car.set_pwm[1] = 0;
			}
			else
			{
				flash_num++;
			if(flash_num == 100)
			{
				flash_num = 0;
				sprintf(str_buf,"Time:%d s ", get_motor_sec()- first_time);
				OLED_ShowString(0,0,(uint8_t*)str_buf,8);
				
				sprintf(str_buf,"Velocity:%d cm/s ", ((car.motor_info->speed[0]+car.motor_info->speed[1]))/2);
				OLED_ShowString(0,1,(uint8_t*)str_buf,8);
				
				sprintf(str_buf,"Distance:%d cm ", (car.motor_info->abs_distance[0]+car.motor_info->abs_distance[1])/2-(first_l_distance+first_r_distance)/2);
				OLED_ShowString(0,2,(uint8_t*)str_buf,8);
			}
				
				car.set_pwm[0] = 700;//car.auto_cross_info->wheel_pwm[0];
				car.set_pwm[1] = 700;//car.auto_cross_info->wheel_pwm[1];
			}
		}
		if(count == 0 || count == 1 || count == 2)
		{
			flash_num++;
			if(flash_num == 100)
			{
				flash_num = 0;
			sprintf(str_buf,"Time:%d s ", get_motor_sec()- first_time);
			OLED_ShowString(0,0,(uint8_t*)str_buf,8);
			
			sprintf(str_buf,"Velocity:%d cm/s ", ((car.motor_info->speed[0]+car.motor_info->speed[1]))/2);
			OLED_ShowString(0,1,(uint8_t*)str_buf,8);
			
			sprintf(str_buf,"Distance:%d cm ", (car.motor_info->abs_distance[0]+car.motor_info->abs_distance[1])/2-(first_l_distance+first_r_distance)/2);
			OLED_ShowString(0,2,(uint8_t*)str_buf,8);
			}
		}
		
	}
		
	else if(car.mode == CHARGE)
	{
		static uint8_t stay_in = 0;
		if(mode_in != 1)
		{
			first_time = get_motor_sec();
			first_l_distance = car.motor_info->abs_distance[0];
			first_r_distance = car.motor_info->abs_distance[1];
			
			mode_stop = 0;
			car_go_straight(40);
			mode_in = 1;
		}
		//判断停止位
		infra_num = follow_line_loop();
		if(infra_num == 8 || mode_stop)
		{
			car.set_pwm[0] = 0;
			car.set_pwm[1] = 0;
			mode_stop = 1;
			
			if((get_motor_sec() - first_time) >= 10 && !stay_in)
			{
				mode_stop = 0;
				stay_in = 1;
			}
		}
		
		else
		{
			sprintf(str_buf,"Time:%d s ", get_motor_sec()- first_time);
			OLED_ShowString(0,0,(uint8_t*)str_buf,8);
			
			sprintf(str_buf,"Velocity:%d cm/s ", ((car.motor_info->speed[0]+car.motor_info->speed[1]))/2);
			OLED_ShowString(0,1,(uint8_t*)str_buf,8);
			
			sprintf(str_buf,"Distance:%d cm ", (car.motor_info->abs_distance[0]+car.motor_info->abs_distance[1])/2-(first_l_distance+first_r_distance)/2);
			OLED_ShowString(0,2,(uint8_t*)str_buf,8);
			
			car.set_pwm[0] = car.follow_line_info->wheel_pwm[0];
			car.set_pwm[1] = car.follow_line_info->wheel_pwm[1];
		}
	}
	else
	{
		car.set_pwm[0] = 0;
		car.set_pwm[1] = 0;
	}
	fsm_set_speed();
}

static void fsm_set_speed(void)
{
	motor_set_speed(car.set_pwm);
}













