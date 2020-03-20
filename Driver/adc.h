/*******************************************************************************
 *
 * Filename  : adc.h
 *
 * Function  : Reference head file for "AD" library.
 *
 * Created   : 2016-10-09 15:23:00
 *           : Mr. WQH
 *
 * Check In  : $Date: 2016-10-09 15:23:00 +0800  $
 *           : $Author: wqh $
 * Revision  : $Revision: * $
 *
 * Copyright(c) by ShangHai DongZhou Lawton Telecom Technologies Co.,Ltd.  
 *              		All Rights Reserved         
 *
 *******************************************************************************/
 
#ifndef __AD_H
#define __AD_H




#define     ADC_CH_NUM            7      // 12��ADCͨ��
#define		ADCTIMES		      20	  // AD�ۼ�10��
#define     DMA_BUFFER_SIZE       ADC_CH_NUM * ADCTIMES 





#define    ADC_ENV_TPT            0		//�����¶�
#define    ADC_PA_PWRIN           1		//���빦��
#define    ADC_PA_CUR             2		//ĩ������
#define    ADC_PA_PWROUT          3		//���书��
#define    ADC_PA_PWRRV           4		//���书��
#define    ADC_PA_VOL             5		//�����ѹ
#define    ADC_CPU_TPT            6		//CPU�¶�


extern void Get_ADC1(void);
void Caculate_ADC(void);
float GetPwr(float dy,float *V,float *W,float valmin);


#endif

