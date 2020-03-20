/*******************************************************************************
 *
 * Filename  : Function.h
 *
 * Function  : Reference head file for "FUCTION" library.
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

#ifndef	__PUBLIC_H
#define	__PUBLIC_H


#include  <stm32f10x.h>

 

u8 HI(u16 num_16);
u8 LOW(u16 num_16);
u16 StrToU16(u8* buf, u8 num);  
u16 In_Str( char* buff, char* Refer);
u16 In_nStr( char* buff, char* Refer, u8 Len);
void DataCopy( u8* To, u8* From, u16 Len );
bool  Compared( u8* p1, u8* p2, u8 length);
u16 cal_crc(u8 *ptr, u8 len) ;
u8 XRL_ChkSum(u8* buff, u16 length);
u8 Check_SUM(u8 *Buf, u8 len);
u8 lrc(u8 *str,u8 lenth);
u8 Packet_LRC(u8 *Buf, u8 Len);
u16 CRC16(unsigned char* pDatabuf,u16 length);
u16 CRC16_GX_MODBUS(unsigned char* pDatabuf,u16 len);
void Byte_To_ASC(u8 *ASCBuf, u8 Byte);
u8 ASC_To_Byte(u8 Hbyte, u8 Lbyte);
u32 AscStrToHex(char* dec);
bool In_nCmp(u8* p1, u8 p1Len, u8* p2, u8 p2Len, u8 CmpLen);
void floatTOuint32(float fdata, u8 *datbyte);
void FloatToByte(float floatNum, unsigned char* byteArry);//浮点数转十六进制
float Hex_To_Decimal(unsigned char *Byte,int num);//十六进制转浮点数



#endif




