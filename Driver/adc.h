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




#define     ADC_CH_NUM            7      // 12个ADC通道
#define		ADCTIMES		      20	  // AD累加10次
#define     DMA_BUFFER_SIZE       ADC_CH_NUM * ADCTIMES 





#define    ADC_ENV_TPT            0		//环境温度
#define    ADC_PA_PWRIN           1		//输入功率
#define    ADC_PA_CUR             2		//末级电流
#define    ADC_PA_PWROUT          3		//入射功率
#define    ADC_PA_PWRRV           4		//反射功率
#define    ADC_PA_VOL             5		//输入电压
#define    ADC_CPU_TPT            6		//CPU温度


extern void Get_ADC1(void);
void Caculate_ADC(void);
float GetPwr(float dy,float *V,float *W,float valmin);


#endif

