#include "led.h"
#include "delay.h"
#include "beep.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	
#include "usmart.h"	 
#include "rtc.h" 


u32 TimeMode=1;//时间增长模式，一次按键加减TimeMode秒，全局变量

//LCD上显示时间的子函数
 void display_time(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec,u8 week){
	LCD_ShowNum(60,130,syear,4,16);									  
	LCD_ShowNum(100,130,smon,2,16);									  
	LCD_ShowNum(124,130,sday,2,16);	 
	LCD_ShowNum(60,162,hour,2,16);									  
	LCD_ShowNum(84,162,min,2,16);									  
	LCD_ShowNum(108,162,sec,2,16);
	switch(week)
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
}

void MyRTC_SetCounter(u32 TimeStamp){
	//直接设置Unix时间戳，以避免手动处理时间进位
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
	PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
	RTC_SetCounter(TimeStamp);	//设置RTC计数器的值
	RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  	
}
//时间加减模式的切换，一次加减TimeMode秒
void SwitchTimeMode(){
	BEEP=1;
	switch(TimeMode){
		case 1:
			TimeMode = 60;
			LCD_ShowString(60,208,200,16,16,"Minute Mode    ");
			break;
		case 60:
			TimeMode = 3600;
			LCD_ShowString(60,208,200,16,16,"Hour   Mode    ");
			break;
		case 3600:
			TimeMode = 86400;
			LCD_ShowString(60,208,200,16,16,"Day    Mode    ");
			break;
		case 86400:
			TimeMode = 2592000;
			LCD_ShowString(60,208,200,16,16,"Month  Mode    ");
			break;
		case 2592000:
			TimeMode = 31536000;
			LCD_ShowString(60,208,200,16,16,"Year  Mode     ");
			break;
		case 31536000:
			TimeMode = 1;
			LCD_ShowString(60,208,200,16,16,"Second  Mode   ");
			break;
		default:
			TimeMode = 60;
			LCD_ShowString(60,208,200,16,16,"Minute Mode    ");
			break;
	}
	BEEP=0;
}
 int main(void)
 {	 
 	u8 t=0;
	u32 CurrentTime;//设置闹钟前将当前Unix时间戳入栈
	u32 AlarmTime;//想要设置的闹钟Unix时间戳

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
	LCD_ShowString(60,110,200,16,16,"2015/5/15");		
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
			if(KEY_Scan(0)==4){
				BEEP = 0;
				delay_ms(500);//去按键抖动，防止连按
				break;
			}
			}
		}
		
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
			LED0=!LED0;
			//修改时间部分
			if(KEY_Scan(0)==1){
				LCD_ShowString(60,188,200,16,16,"Set Time Start         ");
				while(1){
					if(KEY_Scan(0)==2){
						BEEP=1;
						//修改并刷新屏幕上的时间
						MyRTC_SetCounter(RTC_GetCounter()+TimeMode);
						display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
						BEEP=0;
					}else if(KEY_Scan(0)==3){
						BEEP=1;
						MyRTC_SetCounter(RTC_GetCounter()-TimeMode);
						//修改并刷新屏幕上的时间
						display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
						BEEP=0;
					}else if(KEY_Scan(0)==4){
						//设置时钟的时候，按KEY UP可以切换一次按键要加减的时间
						SwitchTimeMode();
					}else if(KEY_Scan(0)==1){
						LCD_ShowString(60,188,200,16,16,"Set Time End       ");
						
						break;
					}
				}
			}else if(KEY_Scan(1)==4){//设置闹钟部分
				LCD_ShowString(60,188,200,16,16,"Set Alarm Start         ");
				CurrentTime = RTC_GetCounter();//当前时间入栈
				while(1){
					if(KEY_Scan(0)==2){
						BEEP=1;
						//设置 与 显示闹钟时间
						MyRTC_SetCounter(RTC_GetCounter()+TimeMode);
						AlarmTime = RTC_GetCounter() + TimeMode;
						//修改并刷新屏幕上的时间
						display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
						BEEP=0;
					}else if(KEY_Scan(0)==3){
						MyRTC_SetCounter(RTC_GetCounter()-TimeMode);
						//修改并刷新屏幕上的时间
						AlarmTime = RTC_GetCounter() - TimeMode;
						display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
						BEEP=0;
					}else if(KEY_Scan(0)==1){
						//设置闹钟的时候，按KEY0可以切换一次按键要加减的时间
						SwitchTimeMode();
					}else if(KEY_Scan(0)==4){
						LCD_ShowString(60,188,200,16,16,"Set Alarm End       ");
						//当前时间出栈，恢复计时
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//使能PWR和BKP外设时钟  
						PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问 
						RTC_SetCounter(CurrentTime);	//设置RTC计数器的值
						RTC_WaitForLastTask();	//等待最近一次对RTC寄存器的写操作完成  
						delay_ms(500);//去按键抖动，防止连按
						break;
					}
				}
			}
		}	
		delay_ms(10);								  
	};  
 }

