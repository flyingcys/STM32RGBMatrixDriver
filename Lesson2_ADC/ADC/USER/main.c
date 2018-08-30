#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

#include "stdlib.h"
#include "oled.h" 
#include "adc.h"

int main(void)
{ 
    uint16_t Advalue;   //ADת��ֵ
	delay_init(168);		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
    Adc_Init();             //��ʼ��ADC
	OLED_Init();			//��ʼ��OLED  

	OLED_ShowString(0,0,"   ADC TEST",16);
    OLED_ShowString(0,16,"Advalue:",16);
    while(1)
    {
        Advalue = Get_Adc_Average(0,10);
        OLED_ShowNum(64, 16, Advalue, 4, 16);
        LED2 = 0;
        LED3 = 1;
        delay_ms(500);
        LED2 = 1;
        LED3 = 0;
        delay_ms(500);
     }
}


	

 



