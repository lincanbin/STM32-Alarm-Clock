#include "led.h"
#include "delay.h"
#include "beep.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	
#include "usmart.h"	 
#include "rtc.h" 

 int main(void)
 {	 
 	u8 t=0;
	u32 CurrentTime;//设置闹钟前将当前时间入栈
	u32 AlarmTime;//设置闹钟时间
	delay_init();	    	 //延时函数初始化	  
	 
	KEY_Init();//初始化按键
	BEEP_Init();//初始化逼逼逼，蜂鸣器
	 
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
 	LED_Init();			     //LED端口初始化
	LCD_Init();			 	
	usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART	
	RTC_Init();	  			//RTC初始化
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,50,200,16,16,"STM32");	
	LCD_ShowString(60,70,200,16,16,"Alarm Clock");	
	LCD_ShowString(60,90,200,16,16,"Canbin Lin");
	LCD_ShowString(60,110,200,16,16,"2015/5/8");		
	//显示时间
	POINT_COLOR=BLUE;//设置字体为蓝色
	LCD_ShowString(60,130,200,16,16,"    -  -  ");	   
	LCD_ShowString(60,162,200,16,16,"  :  :  ");
	//允许RTC报警
	RTC_ITConfig(RTC_IT_ALR, ENABLE);
	RTC_WaitForLastTask();
	while(1)
	{
		if(AlarmTime == RTC_GetCounter()){
			while(1){
			BEEP = 0;
			delay_ms(200);
			BEEP = 1;
			delay_ms(200);
			}
		}
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			LCD_ShowNum(60,130,calendar.w_year,4,16);									  
			LCD_ShowNum(100,130,calendar.w_month,2,16);									  
			LCD_ShowNum(124,130,calendar.w_date,2,16);	 
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(60,148,200,16,16,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(60,148,200,16,16,"Monday   ");
					break;
				case 2:
					LCD_ShowString(60,148,200,16,16,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(60,148,200,16,16,"Wednesday");
					break;
				case 4:
					LCD_ShowString(60,148,200,16,16,"Thursday ");
					break;
				case 5:
					LCD_ShowString(60,148,200,16,16,"Friday   ");
					break;
				case 6:
					LCD_ShowString(60,148,200,16,16,"Saturday ");
					break;  
			}
			LCD_ShowNum(60,162,calendar.hour,2,16);									  
			LCD_ShowNum(84,162,calendar.min,2,16);									  
			LCD_ShowNum(108,162,calendar.sec,2,16);
			LED0=!LED0;
			//修改时间部分
			if(KEY_Scan(0)==1){
				LCD_ShowString(60,188,200,16,16,"Set Time");
				while(1){
					if(KEY_Scan(1)==2){
						BEEP=1;
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
						PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
						RTC_SetCounter(RTC_GetCounter()+60);	//设置RTC计数器的值

						RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  	
						//刷新时间
						LCD_ShowNum(60,130,calendar.w_year,4,16);									  
						LCD_ShowNum(100,130,calendar.w_month,2,16);									  
						LCD_ShowNum(124,130,calendar.w_date,2,16);	
						LCD_ShowNum(60,162,calendar.hour,2,16);									  
						LCD_ShowNum(84,162,calendar.min,2,16);									  
						LCD_ShowNum(108,162,calendar.sec,2,16);
						BEEP=0;
					}else if(KEY_Scan(1)==3){
						BEEP=1;
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
						PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
						RTC_SetCounter(RTC_GetCounter()-60);	//设置RTC计数器的值
						//刷新时间
						LCD_ShowNum(60,130,calendar.w_year,4,16);									  
						LCD_ShowNum(100,130,calendar.w_month,2,16);									  
						LCD_ShowNum(124,130,calendar.w_date,2,16);	
						LCD_ShowNum(60,162,calendar.hour,2,16);									  
						LCD_ShowNum(84,162,calendar.min,2,16);									  
						LCD_ShowNum(108,162,calendar.sec,2,16);
						BEEP=0;
					}else if(KEY_Scan(0)==1){
						LCD_ShowString(60,188,200,16,16,"SetT End");
						
						break;
					}
				}
			}else if(KEY_Scan(1)==4){//设置闹钟部分
				LCD_ShowString(60,188,200,16,16,"Set Alarm");
				CurrentTime = RTC_GetCounter();//当前时间入栈
				while(1){
					if(KEY_Scan(0)==2){
						BEEP=1;
						//设置 与 显示闹钟时间
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
						PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
						RTC_SetCounter(RTC_GetCounter()+2);	//设置RTC计数器的值
						RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  	
						AlarmTime = RTC_GetCounter() + 10;
						//刷新时间
						LCD_ShowNum(60,130,calendar.w_year,4,16);									  
						LCD_ShowNum(100,130,calendar.w_month,2,16);									  
						LCD_ShowNum(124,130,calendar.w_date,2,16);	
						LCD_ShowNum(60,162,calendar.hour,2,16);									  
						LCD_ShowNum(84,162,calendar.min,2,16);									  
						LCD_ShowNum(108,162,calendar.sec,2,16);
						BEEP=0;
						break;//debug
					}else if(KEY_Scan(0)==3){
						//设置 与 显示闹钟时间
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
						PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
						RTC_SetCounter(RTC_GetCounter()-2);	//设置RTC计数器的值
						RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  	
						AlarmTime = RTC_GetCounter() - 10;
						//刷新时间
						LCD_ShowNum(60,130,calendar.w_year,4,16);									  
						LCD_ShowNum(100,130,calendar.w_month,2,16);									  
						LCD_ShowNum(124,130,calendar.w_date,2,16);	
						LCD_ShowNum(60,162,calendar.hour,2,16);									  
						LCD_ShowNum(84,162,calendar.min,2,16);									  
						LCD_ShowNum(108,162,calendar.sec,2,16);
						BEEP=0;
						break;//debug
					}else if(KEY_Scan(0)==1){
						LCD_ShowString(60,188,200,16,16,"SetAL End");
						//当前时间出栈
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
						PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
						RTC_SetCounter(CurrentTime);	//设置RTC计数器的值
						RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  	
						break;
					}
				}
			}
		}	
		delay_ms(10);								  
	};  
 }

