/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V1.1
* Date               : 05/30/2008
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED 
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

#include "includes.h"
#include "global.h"
 



/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMIException(void)
{
 
}

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFaultException(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManageException(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFaultException(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFaultException(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMonitor(void)
{
}

/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVCHandler(void)
{
}

/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSVC(void)
{
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void MySysTick_Handler(void)
//{
//  static u8 ms_cnt  =0;  // 毫秒计数器 
//	static u8 sec_cnt =0; 
//	static u8 min_cnt =0;
//	static u8 day_cnt =0;	
//	static u8 iwdgrc_cnt =0;

//   	OS_CPU_SR  cpu_sr;
//   	OS_ENTER_CRITICAL();  //保存全局中断标志,关总中断
//   	OSIntNesting++;
//   	OS_EXIT_CRITICAL();	  //恢复全局中断标志


//   
//	Sys_Flag.Bit.fMS = TRUE;

//	if(iwdgrc_cnt ++ >= 5)
//	{
//	 	iwdgrc_cnt =0;
//#if IWDG_EN
//	  IWDG_ReloadCounter(); //手动喂狗  
//#endif
//	}
//	CurTimes++;
//	/* 以下是时间计数标志 */
//	if(ms_cnt ++ >=100)
//	{
//	  	ms_cnt =0;
//		Sys_Flag.Bit.fSec = TRUE;  //1s  标志
//		if(sec_cnt++ >=60)
//		{
//		  	sec_cnt =0;
//			Sys_Flag.Bit.fMin = TRUE;  //分标志
//			if(min_cnt++ >=60)
//			{
//			  	min_cnt =0;
//				Sys_Flag.Bit.fHour =TRUE; //   时标志
//				if(day_cnt >=24)
//				{
//				 	day_cnt =0;
//					Sys_Flag.Bit.fDOM = TRUE ;	 //天标志
//				}
//			}
//		}	 	 		
//	}


//	/* 以下是网络定时器 */
//   	if(g_NetTimer05S++ >=50)    //0.5秒溢出标志
//	{			    
//	   	g_NetTimer05S = 0;
//	   	Sys_Flag.Bit.fNetTime05s =TRUE;	
//   	}  
//   	if(g_NetTimer10S++ >=1000) 	 //10秒溢出标志
//	{			
//	   	g_NetTimer10S =0;
//	   	Sys_Flag.Bit.fNetTime10s =TRUE;	
//   	}  

//	
//	// 串口1接收超时限制
//	if( UART1_RBuf.TimeOut>0 )				
//	{
//		UART1_RBuf.TimeOut-- ;
//		if( UART1_RBuf.TimeOut==0 )
//		{
//			UART1_RBuf.fEnd = TRUE ;   //接收超时
//		}
//	}

//	// 串口2接收超时限制
//	if( UART2_RBuf.TimeOut>0 )				
//	{
//		UART2_RBuf.TimeOut-- ;
//		if( UART2_RBuf.TimeOut==0 )
//		{
//			UART2_RBuf.fEnd = TRUE ;   //接收超时
//		}
//	}
//	// 串口3接收超时限制
//	if( UART3_RBuf.TimeOut>0 )				
//	{
//		UART3_RBuf.TimeOut-- ;
//		if( UART3_RBuf.TimeOut==0 )
//		{
//			UART3_RBuf.fEnd = TRUE ;   //接收超时
//		}
//	}

//	// 串口5接收超时限制
//	if( UART5_RBuf.TimeOut>0 )				
//	{
//		UART5_RBuf.TimeOut-- ;
//		if( UART5_RBuf.TimeOut==0 )
//		{
//			UART5_RBuf.fEnd = TRUE ;   //接收超时
//		}
//	}
//   	OSTimeTick(); //主要判断延时的任务是否计时到*/ 
//   	OSIntExit();  //如果有更高优先级的任务就绪了,则执行一次任务切换     
//}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
  	OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL(); 	/* Tell uC/OS-II that we are starting an ISR */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

	/* Update WWDG counter */
	if(Sys_Flag.Bit.fWdgClr == 1)			//	Sys_Flag.Bit.fWdgClr
	{
  		WWDG_SetCounter(0x7F);
		Sys_Flag.Bit.fWdgClr= 0;
	}
	
  	/* Clear EWI flag */
  	WWDG_ClearFlag();

	OSIntExit();         /* Tell uC/OS-II that we are leaving the ISR  */

}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
 
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
  
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
}

/****************************************************************************
* 名    称：void EXTI1_IRQHandler(void)
* 功    能：ENC28J60 接收完成 EXTI1中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void EXTI1_IRQHandler(void)	
{
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
     
}
/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{ 
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{  
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel1_IRQHandler(void)
{

}

/*******************************************************************************
* Function Name  : DMA1_Channel2_IRQHandler
* Description    : This function handles DMA1 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel3_IRQHandler
* Description    : This function handles DMA1 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel4_IRQHandler
* Description    : This function handles DMA1 Channel 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel5_IRQHandler
* Description    : This function handles DMA1 Channel 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{
 
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
 
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SCE_IRQHandler(void)
{
}
/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
//外部中断线6的中断服务函数
void EXTI9_5_IRQHandler(void)
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	EXTI_ClearITPendingBit(EXTI_Line6); //清除中断线6挂起标志位
	while(ENC28J60_INT == 0)
	{
		ENC28J60_ISRHandler();
	}
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}
*/
void EXTI9_5_IRQHandler(void)
{
  static u8 int_nu = 0, Key_Flag = 0;
  OS_CPU_SR  cpu_sr;
  	
	OSIntEnter();    
#if IWDG_EN
	  IWDG_ReloadCounter(); //手动喂狗  
#endif
  if(EXTI_GetITStatus(EXTI_Line8) != RESET)				 
  {
		Sys_Flag.Bit.fETH_INT=TRUE;							  //设置接收完成标志
		EXTI_ClearITPendingBit(EXTI_Line8);					  //清除中断请求标志
		while(ENC28J60_INT == 0)
		{
			ENC28J60_ISRHandler();
		}
  }

  if(EXTI_GetITStatus(EXTI_Line6) != RESET)				 
  {
		if(int_nu==0 && Encoder_KEY_A==0)   //第一次中断，并且A相是下降沿
		{
			Key_Flag = 0;
			if(Encoder_KEY_B)	Key_Flag = 1;  //根据B相，设定标志
			int_nu=1;             //中断计数
			
		}
		else if(int_nu && Encoder_KEY_A)      //第二次中断，并且A相是上升沿
		{
			if(Encoder_KEY_B == 0 && Key_Flag == 1)	Encoder_CW = 1;
			if(Encoder_KEY_B && Key_Flag == 0)	Encoder_CCW = 1;
			int_nu=0;  //中断计数复位，准备下一次
		}
		

			EXTI_ClearITPendingBit(EXTI_Line6);					  //清除中断请求标志
	
  }
  OSIntExit();  //在os_core.c文件里定义,如果有更高优先级的任务就绪了,则执行一次任务切换     
  
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
  /* Clear the TIM1 Update pending bit */
 

}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{

}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
 
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void USART1_IRQHandler(void)
//{ 
//    OS_CPU_SR  cpu_sr;

//	OS_ENTER_CRITICAL(); 	/* Tell uC/OS-II that we are starting an ISR */
//    OSIntNesting++;
//    OS_EXIT_CRITICAL();

//	
//	if( UART1_RBuf.fEnd == TRUE )			// 上一次接收完的数据未处理结束,退出
//	{
//		OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//		return ;		
//	} 	

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//	{
//		/* Read one byte from the receive data register */
//		UART1_RBuf.Buff[UART1_RBuf.Len] = USART_ReceiveData(USART1); 
//		UART1_RBuf.Len++ ; 
//		UART1_RBuf.TimeOut = MAX_UART_TimeOut ;   
//		
//		/* Clear the USART1 Receive interrupt */
//	//	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//		
//		if(UART1_RBuf.Len >= MAX_UART_BUF_LEN )	// 接收缓冲区满,申请一次处理
//		{
//			UART1_RBuf.fEnd = TRUE ; 

//		//	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 	
//		//	OSSemPost(Com1Sem);					
//		}
//	}
//	OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
//}
void USART1_IRQHandler(void)
{ 
    OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL(); 	/* Tell uC/OS-II that we are starting an ISR */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		USART_ReceiveData(USART1);//读取数据 注意：这句必须要，否则不能够清除中断标志位。
		DMA_Cmd(DMA1_Channel5,DISABLE);//关闭USART1 TX DMA1 所指示的通道

		if(UART1_RBuf.fEnd == FALSE)
		{
			 
			UART1_RBuf.Len = MAX_DMA_BUFLEN - DMA_GetCurrDataCounter(DMA1_Channel5); //算出接本帧数据长度

			if(UART1_RBuf.Len >= MAX_UART_BUF_LEN)
			{
				memcpy(UART1_RBuf.Buff, g_DMA_TBuf, MAX_UART_BUF_LEN);
				UART1_RBuf.Len = MAX_UART_BUF_LEN;
			}
			else
			{
				memcpy(UART1_RBuf.Buff, g_DMA_TBuf, UART1_RBuf.Len);
			}
			UART1_RBuf.fEnd = TRUE;	
		}

		USART_ClearITPendingBit(USART1, USART_IT_IDLE);         //清除中断标志
		//设置传输数据长度
		DMA_SetCurrDataCounter(DMA1_Channel5,MAX_DMA_BUFLEN);//从新设置缓冲大小,指向数组0
		DMA_Cmd(DMA1_Channel5, ENABLE);                 //恢复DMA指针，等待下一次的接收

	} 
	
	OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
}
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
	OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL(); 	/* Tell uC/OS-II that we are starting an ISR */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

//	
	if( UART2_RBuf.fEnd == TRUE )			// 上一次接收完的数据未处理结束,退出
	{
		OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		return ;		
	} 	

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		UART2_RBuf.Buff[UART2_RBuf.Len] = USART_ReceiveData(USART2); 
		UART2_RBuf.Len++ ; 
		UART2_RBuf.TimeOut = MAX_UART_TimeOut ;   
		
		/* Clear the USART3 Receive interrupt */
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		
		if(UART2_RBuf.Len >= MAX_UART_BUF_LEN )	// 接收缓冲区满,申请一次处理
		{
			UART2_RBuf.fEnd = TRUE ; 						
		}
	}
	OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
   	OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL(); 	/* Tell uC/OS-II that we are starting an ISR */
    OSIntNesting++;
    OS_EXIT_CRITICAL();
	
	if( UART3_RBuf.fEnd == TRUE )			// 上一次接收完的数据未处理结束,退出
	{
		OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
		
		/* Clear the USART3 Receive interrupt */
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);

		return ;		
	} 	

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		UART3_RBuf.Buff[UART3_RBuf.Len] = USART_ReceiveData(USART3); 
		UART3_RBuf.Len++ ; 
		UART3_RBuf.TimeOut = MAX_UART_TimeOut ;   
		
		/* Clear the USART3 Receive interrupt */
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		
		if(UART3_RBuf.Len >= MAX_UART_BUF_LEN )	// 255接收缓冲区满,申请一次处理
		{
			UART3_RBuf.fEnd = TRUE ; 

		//	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 	
		//	OSSemPost(Com1Sem);					
		}
	}
	if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_ORE);
		UART3_RBuf.Len = 0;		
	} 
	OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
 
}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
 
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_UP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_TRG_COM_IRQHandler
* Description    : This function handles TIM8 Trigger and commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM8_CC_IRQHandler
* Description    : This function handles TIM8 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_CC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC3_IRQHandler
* Description    : This function handles ADC3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : FSMC_IRQHandler
* Description    : This function handles FSMC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{

}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{
    OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL(); 	/* Tell uC/OS-II that we are starting an ISR */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

	
	if( UART5_RBuf.fEnd == TRUE )			// 上一次接收完的数据未处理结束,退出
	{
		OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
		USART_ClearITPendingBit(UART5, USART_IT_RXNE);
		return ;		
	} 	

	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		UART5_RBuf.Buff[UART5_RBuf.Len] = USART_ReceiveData(UART5); 
		UART5_RBuf.Len++ ; 
		UART5_RBuf.TimeOut = MAX_UART_TimeOut ;   
		
		/* Clear the USART1 Receive interrupt */
		USART_ClearITPendingBit(UART5, USART_IT_RXNE);
		
		if(UART5_RBuf.Len >= MAX_UART_BUF_LEN )	// 接收缓冲区满,申请一次处理
		{
			UART5_RBuf.fEnd = TRUE ; 

		//	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 	
		//	OSSemPost(Com1Sem);					
		}
	}
	OSIntExit();         	/* Tell uC/OS-II that we are leaving the ISR */	
}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : This function handles DMA2 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : This function handles DMA2 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : This function handles DMA2 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMA2_Channel4_5_IRQHandler
* Description    : This function handles DMA2 Channel 4 and DMA2 Channel 5
*                  interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel4_5_IRQHandler(void)
{
}


