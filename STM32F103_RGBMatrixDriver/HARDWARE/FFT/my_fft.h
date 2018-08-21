#ifndef __MY_FFT_H
#define  __MY_FFT_H


#include "math.h" 
#include "sys.h"


#define PI2  6.28318530717959
#define Fs   512 //ģ�������40KHz


#define NPT_256		 256 	//ѡ����ٸ���

#define NPT NPT_256

extern void cr4_fft_256_stm32(void *pssOUT, void *pssIN, u16 Nbin);//������
//pssIN����һ��Ҫָ��32λ���飬��16λΪ�鲿��ǰ16λΪʵ����ǰ16λ��16λ��Ϊ�����ŵĲ��롣
extern u32 lBUFIN[NPT] ;        /* Complex input vector */
extern u32 lBUFMAG[NPT/2];        /* Magnitude vector */

extern u8 table_0_31[NPT];




void my_fft_test(void);
void my_fft(void);




#endif





