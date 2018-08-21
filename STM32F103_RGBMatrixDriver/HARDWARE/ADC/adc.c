#include "adc.h"
#include "led.h"
#include "my_fft.h"


void ADC_GPIO_Configuration(void)        //ADC���ú���  
{  
  GPIO_InitTypeDef GPIO_InitStructure;  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��ADC��GPIOAʱ��                        
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;        //�ܽ�2  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;    //ģ������ģʽ  
  GPIO_Init(GPIOA, &GPIO_InitStructure);     //GPIO��  
}  


//��ʱ����ʼ��  
void TIM2_Configuration(void)  
{   
 TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;   
 TIM_OCInitTypeDef TIM_OCInitStructure;   
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);   
 TIM_TimeBaseStructure.TIM_Period = 24*4;//����25usһ��TIM2�Ƚϵ�����  40KHz
 TIM_TimeBaseStructure.TIM_Prescaler = 71;//ϵͳ��Ƶ72M�������Ƶ71���൱��1000K�Ķ�ʱ��2ʱ��   
 TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;   
 TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
 TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);  
      
 TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//������ϸ˵��   
 TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//TIM_OutputState_Disable;   
 TIM_OCInitStructure.TIM_Pulse = 12*4;   
 TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//�����PWM1ҪΪLow��PWM2��ΪHigh   
 TIM_OC2Init(TIM2, & TIM_OCInitStructure);     
// TIM_InternalClockConfig(TIM2);  
// TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);   
// TIM_UpdateDisableConfig(TIM2, DISABLE);  
}  
//ADC_DMA��ʼ������  
void ADC_DMA_Config(void)  
{  
  DMA_InitTypeDef DMA_InitStructure; // ע��ADCΪ12λģ��ת������ֻ��ADCConvertedValue�ĵ�12λ��Ч  
	NVIC_InitTypeDef NVIC_InitStructure; 
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//ʹ��DMAʱ��  
  DMA_DeInit(DMA1_Channel1);//����DMA1�ĵ�һͨ��   
  DMA_InitStructure.DMA_PeripheralBaseAddr = (unsigned long)(&ADC1->DR);//DMA��Ӧ���������ַ  
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)lBUFIN; //�ڴ�洢����ַ  
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�  
  DMA_InitStructure.DMA_BufferSize = 256;							//DMA�����С��256��  
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //����һ�����ݺ��豸��ַ��ֹ����  
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //����һ�����ݺ�Ŀ���ڴ��ַ����  
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�����������ݿ��Ϊ16λ  
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word; //DMA�������ݳߴ磬HalfWord����Ϊ16λ  
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ��ת��ģʽ   
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA���ȼ���  
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//M2Mģʽ����  
	
	//�ж�����    
	NVIC_InitStructure.NVIC_IRQChannel =DMA1_Channel1_IRQn;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  
	
	
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);           
  DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);//ʹ�ܴ�������ж�  
}  
//ADC��ʼ��  
void ADC_DMA_TIM_Init(void)  
{  
//���ⲿ�����źű�ѡΪADC�����ע��ת��ʱ��ֻ�����������ؿ�������ת��       
  ADC_InitTypeDef ADC_InitStructure;  
  ADC_GPIO_Configuration();//IO������  
  TIM2_Configuration(); //��ʱ������  
  ADC_DMA_Config();//ADC_DMA����  
	
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //������ת��ģʽ ADC_DUALMOD[3:0]=0000;  
  ADC_InitStructure.ADC_ScanConvMode =DISABLE;//�ر�ɨ��ģʽ ��Ϊֻ��һ��ͨ��  
  ADC_InitStructure.ADC_ContinuousConvMode =DISABLE;//�ر�����ת��ģʽ ����ֻҪ����һ�Σ�  
  //������ת���ͻ�����ͣЪ������CONT��0����������һ���Ժ��ADC���Ͳ�����TIM2_CC2��������  
  ADC_InitStructure.ADC_ExternalTrigConv =ADC_ExternalTrigConv_T2_CC2;//���ת��ģʽ  
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//���뷽ʽ,ADCΪ12λ�У��Ҷ��뷽ʽ ADC_ALIGN=0;  
  ADC_InitStructure.ADC_NbrOfChannel = 1;//����ͨ������1��  ADC_SQR1[23:20]=0000;  
  //ADC_SQR1[23:20] ����ͨ����Ŀ��ѡ��  
  ADC_Init(ADC1, &ADC_InitStructure);  
 // RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ʱ��(12MHz),��RCC���滹Ӧ����APB2=AHBʱ��72MHz  
   
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);  
  //ADC_SMPR2 ADC_SMPR1 ����ÿ��ͨ���Ĳ���ʱ��   
  //ADC_SQR1[19:0]�ADC_SQR1[29:0]�ADC_SQR3[29:0]  ���ö�Ӧͨ����ת��˳��  �����ڶ�ͨ������  
  //ADCͨ���飬 ��3��ͨ�� ����˳��1��ת��ʱ��  
  ADC_ExternalTrigConvCmd(ADC1, ENABLE);//�����ⲿ����ģʽʹ�ܣ�������ⲿ����ʵ��������//����ADCģ����ⲿ��  
   
  ADC_DMACmd(ADC1, ENABLE);     
   
  ADC_Cmd(ADC1, ENABLE);  //ADC���ʹ��  ADC_ADON=1  
      
  ADC_ResetCalibration(ADC1);   //����У׼  
   
  while(ADC_GetResetCalibrationStatus(ADC1));  //�ȴ�����У׼���  
   
  ADC_StartCalibration(ADC1);  //��ʼУ׼  ADC_RSTCAL=1; ��ʼ��У׼�Ĵ���  
   
  while(ADC_GetCalibrationStatus(ADC1));    //�ȴ�У׼���  ADC_CAL=0;    
   
   //ADC_SoftwareStartConvCmd(ADC1, ENABLE); //����ת����ʼ��ADCͨ��DMA��ʽ���ϵĸ���RAM����  
  //ADC_SWSTART=1 ��ʼ����ת�� �м� �������Ҳ�����ⲿ�¼�  Ҫ����  ADC_EXTTRIG=1  
////  //ʵ���ϻ�����STM32�ڲ���  
  TIM_Cmd(TIM2, ENABLE);//�����򿪶�ʱ��ʹ��  
  DMA_Cmd(DMA1_Channel1, ENABLE);//ʹ��DMA      
}  
extern u8 fft_complet_flag;

//�жϴ�����  
void  DMA1_Channel1_IRQHandler(void)  
{  
   if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)
		 {  
				 //�Լ����жϴ������ ���Ǽ�ס����Ҫ̫����  ��ò�Ҫ�����ж�ʱ��  
					fft_complet_flag = 1; //������ɱ�־��λ
			   LED0=!LED0;
         DMA_ClearITPendingBit(DMA1_IT_TC1);  
		 }  
 }  


 
