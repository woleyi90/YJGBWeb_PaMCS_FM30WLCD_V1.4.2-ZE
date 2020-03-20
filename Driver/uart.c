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
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      

//	while((USART2->SR&0x40)==0);//ѭ������,ֱ���������   
//    USART2->DR = (u8) ch;      
	return ch;
}



 
/*
 *******************************************************************************
 * ��������: UART1Write
 * ��������: UART1���ͺ���
 * �䡡��:   Data : UART1���������׵�ַ  Len: �������ݳ���
 * �䡡��:   ���ͽ�� TRUE:���ͳɹ�  FALSE:����ʧ��
 *         
 *******************************************************************************/ 
void UART1Write( u8* Data, u16 Len )
{ 	
	while(Len--)
	{  
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif
		USART_SendData(USART1, (*Data++)); 	
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	 //�ȴ��������	
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}	
    } 
}

/*
 *******************************************************************************
 * ��������: UART1WriteBuf()
 * ��������: UART1���ͺ���
 * �䡡��:   Data : UART0���������׵�ַ  Len: �������ݳ���
 * �䡡��:   ���ͽ�� TRUE:���ͳɹ�  FALSE:����ʧ��
 *         
 *******************************************************************************/
bool UART1WriteBuf( u8* Data )
{

	while (*Data)
	{
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif 	
		USART_SendData(USART1, (*Data++)); 	
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	 //�ȴ��������	

    }
    return TRUE ;
}
/*
 *******************************************************************************
 * ��������: UART1WriteBuf()
 * ��������: UART1���ͺ���
 * �䡡��:   Data : UART0���������׵�ַ  Len: �������ݳ���
 * �䡡��:   ���ͽ�� TRUE:���ͳɹ�  FALSE:����ʧ��
 *         
 *******************************************************************************/
bool UART3WriteBuf( u8* Data )
{

	while (*Data)
	{
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif 	
		USART_SendData(USART3, (*Data++)); 	
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	 //�ȴ��������	

    }
    return TRUE ;
}

/*
 *******************************************************************************
 * ��������: UART2Write
 * ��������: UART2���ͺ���
 * �䡡��:   Data : UART2���������׵�ַ  Len: �������ݳ���
 * �䡡��:   ���ͽ�� TRUE:���ͳɹ�  FALSE:����ʧ��
 *         
 *******************************************************************************/
void UART2Write( u8* Data, u16 Len )
{ 
	while(Len--)
	{ 
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif 
		USART_SendData(USART2, (*Data++)); 	
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);//�ȴ��������
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}			
    }
}


/*
 *******************************************************************************
 * ��������: UART2Write
 * ��������: UART2���ͺ���
 * �䡡��:   Data : UART2���������׵�ַ  Len: �������ݳ���
 * �䡡��:   ���ͽ�� TRUE:���ͳɹ�  FALSE:����ʧ��
 *         
 *******************************************************************************/
void UART3Write( u8* Data, u16 Len )
{  
	while(Len--)
	{  
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif
		USART_SendData(USART3, (*Data++)); 	
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);	 //�ȴ��������	
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}		
    }
} 
/*
 *******************************************************************************
 * ��������: UART2Write
 * ��������: UART2���ͺ���
 * �䡡��:   Data : UART2���������׵�ַ  Len: �������ݳ���
 * �䡡��:   ���ͽ�� TRUE:���ͳɹ�  FALSE:����ʧ��
 *         
 *******************************************************************************/
void UART4Write( u8* Data, u16 Len )
{  
	while(Len--)
	{  
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif
		USART_SendData(UART4, (*Data++)); 	
		while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);	 //�ȴ��������	
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}		
    }
}
/*
 *******************************************************************************
 * ��������: UART2Write
 * ��������: UART2���ͺ���
 * �䡡��:   Data : UART2���������׵�ַ  Len: �������ݳ���
 * �䡡��:   ���ͽ�� TRUE:���ͳɹ�  FALSE:����ʧ��
 *         
 *******************************************************************************/
void UART5Write( u8* Data, u16 Len )
{  
	while(Len--)
	{  
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif
		USART_SendData(UART5, (*Data++)); 	
		while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);	 //�ȴ��������	
		if(Len==100)
		{
		  	OSTimeDlyHMSM(0, 0, 0, 10);
		}		
    }          

}
/*
 *******************************************************************************
 * ��������: UART1WriteBuf()
 * ��������: UART1���ͺ���
 * �䡡��:   Data : UART0���������׵�ַ  Len: �������ݳ���
 * �䡡��:   ���ͽ�� TRUE:���ͳɹ�  FALSE:����ʧ��
 *         
 *******************************************************************************/
bool UART5WriteBuf( u8* Data )
{

	while (*Data)
	{
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif 	
		USART_SendData(UART5, (*Data++)); 	
		while(USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);	 //�ȴ��������	

    }
    return TRUE ;
}
/*******************************************************************************
 * ��    �ƣ�void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
 * ��    �ܣ���ʽ�������������
 * ��ڲ�����USARTx:  ָ������
			Data��   ��������
			...:     ��������
 * ���ڲ�������
 * ˵    ������ʽ�������������
        	"\r"	�س���	   USART_OUT(USART1, "abcdefg\r")   
			"\n"	���з�	   USART_OUT(USART1, "abcdefg\r\n")
			"%s"	�ַ���	   USART_OUT(USART1, "�ַ����ǣ�%s","abcdefg")
			"%d"	ʮ����	   USART_OUT(USART1, "a=%d",10)
 * ���÷������� 
 *******************************************************************************/
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
{ 
//	const char *s;
//    int d;
//    char buf[16];
//    va_list ap;
//    va_start(ap, Data);

//	while(*Data!=0){				                          //�ж��Ƿ񵽴��ַ���������
//		if(*Data==0x5c){									  //'\'
//			switch (*++Data){
//				case 'r':							          //�س���
//					USART_SendData(USARTx, 0x0d);	   

//					Data++;
//					break;
//				case 'n':							          //���з�
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
//				case 's':										  //�ַ���
//                	s = va_arg(ap, const char *);
//                	for ( ; *s; s++) {
//                    	USART_SendData(USARTx,*s);
//						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
//                	}
//					Data++;
//                	break;
//            	case 'd':										  //ʮ����
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
 * ��������: itoa()
 * ��������: ��������ת�ַ�������
 * �䡡��:   
 * �䡡��:   
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




