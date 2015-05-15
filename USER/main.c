#include "led.h"
#include "delay.h"
#include "beep.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	
#include "usmart.h"	 
#include "rtc.h" 


u32 TimeMode=1;//ʱ������ģʽ��һ�ΰ����Ӽ�TimeMode�룬ȫ�ֱ���

//LCD����ʾʱ����Ӻ���
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
	//ֱ������Unixʱ������Ա����ֶ�����ʱ���λ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
	PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
	RTC_SetCounter(TimeStamp);	//����RTC��������ֵ
	RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
}
//ʱ��Ӽ�ģʽ���л���һ�μӼ�TimeMode��
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
	u32 CurrentTime;//��������ǰ����ǰUnixʱ�����ջ
	u32 AlarmTime;//��Ҫ���õ�����Unixʱ���

	delay_init();	    	 //��ʱ������ʼ��	  
	 
	KEY_Init();//��ʼ������
	BEEP_Init();//��ʼ���ƱƱƣ�������
	 
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	
	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
	RTC_Init();	  			//RTC��ʼ��
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"STM32");	
	LCD_ShowString(60,70,200,16,16,"Alarm Clock");	
	LCD_ShowString(60,90,200,16,16,"Canbin Lin");
	LCD_ShowString(60,110,200,16,16,"2015/5/15");		
	//��ʾʱ��
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,130,200,16,16,"    -  -  ");	   
	LCD_ShowString(60,162,200,16,16,"  :  :  ");
	//����RTC����
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
				delay_ms(500);//ȥ������������ֹ����
				break;
			}
			}
		}
		
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
			LED0=!LED0;
			//�޸�ʱ�䲿��
			if(KEY_Scan(0)==1){
				LCD_ShowString(60,188,200,16,16,"Set Time Start         ");
				while(1){
					if(KEY_Scan(0)==2){
						BEEP=1;
						//�޸Ĳ�ˢ����Ļ�ϵ�ʱ��
						MyRTC_SetCounter(RTC_GetCounter()+TimeMode);
						display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
						BEEP=0;
					}else if(KEY_Scan(0)==3){
						BEEP=1;
						MyRTC_SetCounter(RTC_GetCounter()-TimeMode);
						//�޸Ĳ�ˢ����Ļ�ϵ�ʱ��
						display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
						BEEP=0;
					}else if(KEY_Scan(0)==4){
						//����ʱ�ӵ�ʱ�򣬰�KEY UP�����л�һ�ΰ���Ҫ�Ӽ���ʱ��
						SwitchTimeMode();
					}else if(KEY_Scan(0)==1){
						LCD_ShowString(60,188,200,16,16,"Set Time End       ");
						
						break;
					}
				}
			}else if(KEY_Scan(1)==4){//�������Ӳ���
				LCD_ShowString(60,188,200,16,16,"Set Alarm Start         ");
				CurrentTime = RTC_GetCounter();//��ǰʱ����ջ
				while(1){
					if(KEY_Scan(0)==2){
						BEEP=1;
						//���� �� ��ʾ����ʱ��
						MyRTC_SetCounter(RTC_GetCounter()+TimeMode);
						AlarmTime = RTC_GetCounter() + TimeMode;
						//�޸Ĳ�ˢ����Ļ�ϵ�ʱ��
						display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
						BEEP=0;
					}else if(KEY_Scan(0)==3){
						MyRTC_SetCounter(RTC_GetCounter()-TimeMode);
						//�޸Ĳ�ˢ����Ļ�ϵ�ʱ��
						AlarmTime = RTC_GetCounter() - TimeMode;
						display_time(calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec, calendar.week);
						BEEP=0;
					}else if(KEY_Scan(0)==1){
						//�������ӵ�ʱ�򣬰�KEY0�����л�һ�ΰ���Ҫ�Ӽ���ʱ��
						SwitchTimeMode();
					}else if(KEY_Scan(0)==4){
						LCD_ShowString(60,188,200,16,16,"Set Alarm End       ");
						//��ǰʱ���ջ���ָ���ʱ
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
						PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
						RTC_SetCounter(CurrentTime);	//����RTC��������ֵ
						RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������  
						delay_ms(500);//ȥ������������ֹ����
						break;
					}
				}
			}
		}	
		delay_ms(10);								  
	};  
 }

