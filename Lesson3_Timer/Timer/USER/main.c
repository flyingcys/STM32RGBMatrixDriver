#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"

#include "stdlib.h"
#include "oled.h" 
#include "adc.h"
#include "timer.h"

int main(void)
{ 
    uint16_t Advalue;   //ADת��ֵ
    uint16_t i,j;
	delay_init(168);		  //��ʼ����ʱ����
	LED_Init();		        //��ʼ��LED�˿�
    Adc_Init();             //��ʼ��ADC
	OLED_Init();			//��ʼ��OLED  
    TIM3_Int_Init(5000-1, 8400-1);  //��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=1000Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms   
	OLED_ShowString(0,0,"   Timer TEST",16);

    while(1)
    {   

            
    }
    
}


	

 



