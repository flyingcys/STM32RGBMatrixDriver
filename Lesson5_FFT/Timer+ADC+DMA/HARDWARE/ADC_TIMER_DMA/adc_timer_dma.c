
#include "adc_timer_dma.h"
#include "led.h"

/* define ---------------------------------------------------------------------*/
#define ADC3_DR_ADDRESS    ((uint32_t)0x4001224C)

/* ���� ----------------------------------------------------------------------*/
__IO uint16_t ADC3ConvertedValue1[FFT_NUM];
__IO uint16_t ADC3ConvertedValue2[FFT_NUM];
__IO uint32_t ADC3ConvertedVoltage = 0;

/* �����ļ�ʹ�� --------------------------------------------------------------*/
static void TIM2_Config(void);
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitADC
*	����˵��: ADC��ʼ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void adc_timer_dma_init(void)
{  
    ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    /* ʹ�� ADC3, DMA2 �� GPIO ʱ�� ****************************************/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

    /* DMA2 Stream0 channel2 ���� **************************************/
    DMA_InitStructure.DMA_Channel = DMA_Channel_2;  //ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;   //DMA�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)ADC3ConvertedValue1;   //DMA�洢����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; //���赽�洢��ģʽ
    DMA_InitStructure.DMA_BufferSize = FFT_NUM;     //���ݴ�����
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;    //���������ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�洢������ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //�������ݳ��� 16λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //�洢�����ݳ��� 16λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //ѭ��ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; //�����ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;          
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; //�洢��ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //����ͻ�����δ���
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    
    /*DMA double buffer mode*/
    DMA_DoubleBufferModeConfig(DMA2_Stream0,(uint32_t)ADC3ConvertedValue2,DMA_Memory_0);  //DMA_Memory_0�ȿ�ʼ����
    DMA_DoubleBufferModeCmd(DMA2_Stream0,ENABLE);

    	//�ж�����    
	NVIC_InitStructure.NVIC_IRQChannel =DMA2_Stream0_IRQn;    
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  
    DMA_ITConfig(DMA2_Stream0 , DMA_IT_TC, ENABLE);//ʹ�ܴ�������ж�  
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /* ����ADC3ͨ��0����Ϊģ������ģʽ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /****************************************************************************   
	  PCLK2 = HCLK / 2 
	  ����ѡ�����2��Ƶ
	  ADCCLK = PCLK2 /2 = HCLK / 4 = 168 / 4 = 42M
      ADC����Ƶ�ʣ� Sampling Time + Conversion Time = 3 + 12 cycles = 15cyc
                    Conversion Time = 42MHz / 15cyc = 2.8Mbps. 
	*****************************************************************************/
    /* ADC�������ֳ�ʼ��**********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC3 ��ʼ�� ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2; //
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC3, &ADC_InitStructure);
    
    /* ADC3 ���� channel0 ���� *************************************/
    ADC_RegularChannelConfig(ADC3, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);

    /* ʹ��DMA����(��ADCģʽ) */
    ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

    /* ʹ�� ADC3 DMA */
    ADC_DMACmd(ADC3, ENABLE);

    /* ʹ�� ADC3 */
    ADC_Cmd(ADC3, ENABLE);

    TIM2_Config();
}

/*
*********************************************************************************************************
*	�� �� ��: TIM1_Config
*	����˵��: ���ö�ʱ��1�����ڴ���ADC, ����Ƶ��1Msps
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			   			//ʹ�ܶ�ʱ��1ʱ��
    
    TIM_Cmd(TIM2, DISABLE);	
    
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 				   //��ʼ����ʱ��1�ļĴ���Ϊ��λֵ
    TIM_TimeBaseStructure.TIM_Period = 24;        // ARR�Զ���װ�ؼĴ������ڵ�ֵ(��ʱʱ�䣩������Ƶ�ʺ���������»����ж�(Ҳ��˵��ʱʱ�䵽)
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;   						   //PSCʱ��Ԥ��Ƶ�� ���磺ʱ��Ƶ��=TIM1CLK/(ʱ��Ԥ��Ƶ+1)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    				   //CR1->CKDʱ��ָ�ֵ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	   //CR1->CMS[1:0]��DIR��ʱ��ģʽ ���ϼ���
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				   //CCMR2�����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;      //CCER ���ʹ��          
    TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period / 2;//CCR3ͬ������TIMx_CNT�ıȽϣ�����OC4�˿��ϲ�������ź� 
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    	   //CCER�����������	�ߵ�ƽ��Ч     
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_Cmd(TIM2, ENABLE);											   
}

//�жϴ�����  
void  DMA2_Stream0_IRQHandler(void)  
{  
    uint8_t res = 0;
        if(DMA_GetITStatus( DMA2_Stream0, DMA_IT_TCIF0)!=RESET)
		 {  
            
            DMA_ClearITPendingBit( DMA2_Stream0,  DMA_IT_TCIF0);  
		 }  
 }  


