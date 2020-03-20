/*******************************************************************************
 *
 * Filename  : uart.h
 *
 * Function  : Reference head file for "UART" library.
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
#ifndef __UART_H
#define __UART_H




void UART1Write( u8* Data, u16 Len );
void UART2Write( u8* Data, u16 Len );
void UART3Write( u8* Data, u16 Len );
void UART4Write( u8* Data, u16 Len );
void UART5Write( u8* Data, u16 Len );
bool UART1WriteBuf( u8* Data );
bool UART3WriteBuf( u8* Data );
bool UART5WriteBuf( u8* Data );

void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);

 


#endif /* __UART_H */


