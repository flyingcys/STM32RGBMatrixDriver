#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stdlib.h"
#include "oled.h" 
#include "adc.h"
#include "timer.h"
#include "adc_timer_dma.h"


int main(void)
{ 
    uint32_t Advalue;   //ADת��ֵ
    uint16_t i,j;
    uint8_t res;
	delay_init(168);		  //��ʼ����ʱ����
    uart_init(115200);
	LED_Init();		        //��ʼ��LED�˿�
    adc_timer_dma_init();             //��ʼ��ADC
	OLED_Init();			//��ʼ��OLED  
    TIM3_Int_Init(5000-1, 8400-1);  //��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=1000Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms   
	OLED_ShowString(0,0," Timer ADC DMA ",16);
    OLED_ShowString(0,16,"Advalue:",16);
    while(1)
    {   
        res = DMA_GetCurrentMemoryTarget(DMA2_Stream0) ;
        if(res)
        {
            for(i = 0 ; i < 256; i ++)
            {
                Advalue += ADC3ConvertedValue1[i];
            }
            Advalue >>= 8;
        }
        else
         {
            for(i = 0 ; i < 256; i ++)
            {
                Advalue += ADC3ConvertedValue2[i];
            }
            Advalue >>= 8;
        }           
         OLED_ShowNum(64, 16, Advalue, 4, 16);   
        Advalue = 0;
    }
    
}


	

 



