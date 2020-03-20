/*******************************************************************************
 *
 * Filename  : uart.c
 *
 * Function  : Implememt UART0 function form for this workbench.
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

#include "includes.h"




/* 
 *******************************************************************************
 *
 * private routines prototype
 * 
 *******************************************************************************
 */
char *itoa(int value, char *string, int radix);


 
/*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      

//	while((USART2->SR&0x40)==0);//循环发送,直到发送完毕   
//    USART2->DR = (u8) ch;      
	return ch;
}



 
/*
 *******************************************************************************
 * 函数名称: UART1Write
 * 功能描述: UART1发送函数
 * 输　入:   Data : UART1发送数据首地址  Len: 发送数据长度
 * 输　出:   发送结果 TRUE:发送成功  FALSE:发送失败
 *         
 *******************************************************************************/ 
void UART1Write( u8* Data, u16 Len )
{ 	
	while(Len--)
	{  
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif
		USART_SendData(USART1, (*Data++)); 	
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	 //等待发送完成	
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}	
    } 
}

/*
 *******************************************************************************
 * 函数名称: UART1WriteBuf()
 * 功能描述: UART1发送函数
 * 输　入:   Data : UART0发送数据首地址  Len: 发送数据长度
 * 输　出:   发送结果 TRUE:发送成功  FALSE:发送失败
 *         
 *******************************************************************************/
bool UART1WriteBuf( u8* Data )
{

	while (*Data)
	{
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif 	
		USART_SendData(USART1, (*Data++)); 	
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	 //等待发送完成	

    }
    return TRUE ;
}
/*
 *******************************************************************************
 * 函数名称: UART1WriteBuf()
 * 功能描述: UART1发送函数
 * 输　入:   Data : UART0发送数据首地址  Len: 发送数据长度
 * 输　出:   发送结果 TRUE:发送成功  FALSE:发送失败
 *         
 *******************************************************************************/
bool UART3WriteBuf( u8* Data )
{

	while (*Data)
	{
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif 	
		USART_SendData(USART3, (*Data++)); 	
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	 //等待发送完成	

    }
    return TRUE ;
}

/*
 *******************************************************************************
 * 函数名称: UART2Write
 * 功能描述: UART2发送函数
 * 输　入:   Data : UART2发送数据首地址  Len: 发送数据长度
 * 输　出:   发送结果 TRUE:发送成功  FALSE:发送失败
 *         
 *******************************************************************************/
void UART2Write( u8* Data, u16 Len )
{ 
	while(Len--)
	{ 
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif 
		USART_SendData(USART2, (*Data++)); 	
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);//等待发送完成
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}			
    }
}


/*
 *******************************************************************************
 * 函数名称: UART2Write
 * 功能描述: UART2发送函数
 * 输　入:   Data : UART2发送数据首地址  Len: 发送数据长度
 * 输　出:   发送结果 TRUE:发送成功  FALSE:发送失败
 *         
 *******************************************************************************/
void UART3Write( u8* Data, u16 Len )
{  
	while(Len--)
	{  
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif
		USART_SendData(USART3, (*Data++)); 	
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	 //等待发送完成	
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}		
    }
} 
/*
 *******************************************************************************
 * 函数名称: UART2Write
 * 功能描述: UART2发送函数
 * 输　入:   Data : UART2发送数据首地址  Len: 发送数据长度
 * 输　出:   发送结果 TRUE:发送成功  FALSE:发送失败
 *         
 *******************************************************************************/
void UART4Write( u8* Data, u16 Len )
{  
	while(Len--)
	{  
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif
		USART_SendData(UART4, (*Data++)); 	
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);	 //等待发送完成	
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}		
    }
}
/*
 *******************************************************************************
 * 函数名称: UART2Write
 * 功能描述: UART2发送函数
 * 输　入:   Data : UART2发送数据首地址  Len: 发送数据长度
 * 输　出:   发送结果 TRUE:发送成功  FALSE:发送失败
 *         
 *******************************************************************************/
void UART5Write( u8* Data, u16 Len )
{  
	while(Len--)
	{  
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif
		USART_SendData(UART5, (*Data++)); 	
		while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);	 //等待发送完成	
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}		
    }          

}
/*
 *******************************************************************************
 * 函数名称: UART1WriteBuf()
 * 功能描述: UART1发送函数
 * 输　入:   Data : UART0发送数据首地址  Len: 发送数据长度
 * 输　出:   发送结果 TRUE:发送成功  FALSE:发送失败
 *         
 *******************************************************************************/
bool UART5WriteBuf( u8* Data )
{

	while (*Data)
	{
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif 	
		USART_SendData(UART5, (*Data++)); 	
		while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);	 //等待发送完成	

    }
    return TRUE ;
}
/*******************************************************************************
 * 名    称：void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
 * 功    能：格式化串口输出函数
 * 入口参数：USARTx:  指定串口
			Data：   发送数组
			...:     不定参数
 * 出口参数：无
 * 说    明：格式化串口输出函数
        	"\r"	回车符	   USART_OUT(USART1, "abcdefg\r")   
			"\n"	换行符	   USART_OUT(USART1, "abcdefg\r\n")
			"%s"	字符串	   USART_OUT(USART1, "字符串是：%s","abcdefg")
			"%d"	十进制	   USART_OUT(USART1, "a=%d",10)
 * 调用方法：无 
 *******************************************************************************/
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
{ 
//	const char *s;
//    int d;
//    char buf[16];
//    va_list ap;
//    va_start(ap, Data);

//	while(*Data!=0){				                          //判断是否到达字符串结束符
//		if(*Data==0x5c){									  //'\'
//			switch (*++Data){
//				case 'r':							          //回车符
//					USART_SendData(USARTx, 0x0d);	   

//					Data++;
//					break;
//				case 'n':							          //换行符
//					USART_SendData(USARTx, 0x0a);	
//					Data++;
//					break;
//				
//				default:
//					Data++;
//				    break;
//			}
//			
//			 
//		}
//		else if(*Data=='%'){									  //
//			switch (*++Data){				
//				case 's':										  //字符串
//                	s = va_arg(ap, const char *);
//                	for ( ; *s; s++) {
//                    	USART_SendData(USARTx,*s);
//						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
//                	}
//					Data++;
//                	break;
//            	case 'd':										  //十进制
//                	d = va_arg(ap, int);
//                	itoa(d, buf, 10);
//                	for (s = buf; *s; s++) {
//                    	USART_SendData(USARTx,*s);
//						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
//                	}
//					Data++;
//                	break;
//				default:
//					Data++;
//				    break;
//			}		 
//		}
//		else USART_SendData(USARTx, *Data++);
//		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
//	}
}


/*
 *******************************************************************************
 * 函数名称: itoa()
 * 功能描述: 整形数据转字符串函数
 * 输　入:   
 * 输　出:   
 *         
 *******************************************************************************/
char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

}




