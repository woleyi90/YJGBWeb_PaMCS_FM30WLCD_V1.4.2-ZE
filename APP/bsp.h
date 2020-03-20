/*******************************************************************************
 *
 * Filename  : bsp.h
 *
 * Function  : Reference head file for "BSP" library.
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

#ifndef __BSP_H	
#define __BSP_H




void RCC_Configuration(void); 
void GPIO_Configuration(void);
void NVIC_Configuration(void);	
void  OS_CPU_SysTickInit(void);

void USART1_Config(u32 baud);
void USART2_Config(u32 baud);
void USART3_Config(u32 baud);
void USART4_Config(u32 baud);
void USART5_Config(u32 baud);
void BSP_Init(void);
void InitNet(void);


#ifdef  DEBUG
void assert_failed(u8* file, u32 line)
#endif




#endif
