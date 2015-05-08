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
	u32 CurrentTime;//��������ǰ����ǰʱ����ջ
	u32 AlarmTime;//��������ʱ��
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
	LCD_ShowString(60,110,200,16,16,"2015/5/8");		
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
			//�޸�ʱ�䲿��
			if(KEY_Scan(0)==1){
				LCD_ShowString(60,188,200,16,16,"Set Time");
				while(1){
					if(KEY_Scan(1)==2){
						BEEP=1;
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
						PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
						RTC_SetCounter(RTC_GetCounter()+60);	//����RTC��������ֵ

						RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
						//ˢ��ʱ��
						LCD_ShowNum(60,130,calendar.w_year,4,16);									  
						LCD_ShowNum(100,130,calendar.w_month,2,16);									  
						LCD_ShowNum(124,130,calendar.w_date,2,16);	
						LCD_ShowNum(60,162,calendar.hour,2,16);									  
						LCD_ShowNum(84,162,calendar.min,2,16);									  
						LCD_ShowNum(108,162,calendar.sec,2,16);
						BEEP=0;
					}else if(KEY_Scan(1)==3){
						BEEP=1;
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
						PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
						RTC_SetCounter(RTC_GetCounter()-60);	//����RTC��������ֵ
						//ˢ��ʱ��
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
			}else if(KEY_Scan(1)==4){//�������Ӳ���
				LCD_ShowString(60,188,200,16,16,"Set Alarm");
				CurrentTime = RTC_GetCounter();//��ǰʱ����ջ
				while(1){
					if(KEY_Scan(0)==2){
						BEEP=1;
						//���� �� ��ʾ����ʱ��
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
						PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
						RTC_SetCounter(RTC_GetCounter()+2);	//����RTC��������ֵ
						RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
						AlarmTime = RTC_GetCounter() + 10;
						//ˢ��ʱ��
						LCD_ShowNum(60,130,calendar.w_year,4,16);									  
						LCD_ShowNum(100,130,calendar.w_month,2,16);									  
						LCD_ShowNum(124,130,calendar.w_date,2,16);	
						LCD_ShowNum(60,162,calendar.hour,2,16);									  
						LCD_ShowNum(84,162,calendar.min,2,16);									  
						LCD_ShowNum(108,162,calendar.sec,2,16);
						BEEP=0;
						break;//debug
					}else if(KEY_Scan(0)==3){
						//���� �� ��ʾ����ʱ��
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
						PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
						RTC_SetCounter(RTC_GetCounter()-2);	//����RTC��������ֵ
						RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
						AlarmTime = RTC_GetCounter() - 10;
						//ˢ��ʱ��
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
						//��ǰʱ���ջ
						RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//ʹ��PWR��BKP����ʱ��  
						PWR_BackupAccessCmd(ENABLE);	//ʹ��RTC�ͺ󱸼Ĵ������� 
						RTC_SetCounter(CurrentTime);	//����RTC��������ֵ
						RTC_WaitForLastTask();	//�ȴ����һ�ζ�RTC�Ĵ�����д�������  	
						break;
					}
				}
			}
		}	
		delay_ms(10);								  
	};  
 }

