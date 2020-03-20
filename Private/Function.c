/*******************************************************************************
 *
 * Filename  : Function.c
 *
 * Function  : Implememt FUCTION function form for this workbench.
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
#include "global.h"	


/*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */


/*******************************************************************************
 * 函数名称 ：HI()
 * 函数功能 ：取双字节高8位
 * 入口参数 ：unsigned int num_16
 * 出口参数 ：HI8
 *******************************************************************************/
u8 HI(u16 num_16)
{
	unsigned int tt;
	tt=num_16&0xff00;
	tt=tt>>8;
	return ((unsigned char )tt);
}

/*******************************************************************************
 * 函数名称 ：LOW()
 * 函数功能 ：取双字节低8位
 * 入口参数 ：unsigned int num_16
 * 出口参数 ：LOW8
 *******************************************************************************/
u8 LOW(u16 num_16)
{
	unsigned char tt;
	tt=num_16 & 0x00ff;
	return tt;
}
/*******************************************************************************
 * 函数名称 ：StrToU16()
 * 函数功能 ：把双字节转化成u16字节,高八位在前，低八位在后
 * 入口参数 ：u8* buf
 * 出口参数 ：u16
 *******************************************************************************/
u16 StrToU16(u8* buf, u8 num)
{						  
	u8 i=0,dataL=0,dataH=0;
	u16 val;
	dataL = buf[num];
	dataH = buf[num+1];

	val = (dataH << 8) | dataL;
	//printf("  val: 0x%x \r\n",val);

	return val;
}


/*******************************************************************************
 * 函数名称 ：in_Str()
 * 函数功能 ：在指定字符串 buff 中寻找指定字符串. 
 * 入口参数 ：字符串首地址, 待寻找的字符串,
 * 出口参数 ：找到匹配字符串在源字符串中的偏移地址
 *         <=0: 查找失败, 没有找到指定字符串, > 0 : 找到
 *		   源字符串指针也会移动
 *******************************************************************************/
u16 In_Str( char* buff, char* Refer)
{
	u16	   	i ;
	bool   fOK =FALSE;	// 是否找到, 默认没有找到. 便于循环启动工作
	char*  		p ;			// 源字符串地址 (指针)
	char*  		pp ;		// 指向参考字符串 地址( 指针)

	for( i =0 ; (fOK == FALSE) && (buff[i] !=0x0) && ( i <0xFF ) ; i++)		// 源字符串不能到尾
	{
		p =&buff[i] ;	// 源字符串后移, 重新定位 源字符串
		pp =Refer ;		// 参考子字符串重新定位
		fOK = TRUE ;	// 默认找到. 会中止循环
		for( ; (*pp !='\0') ; p++, pp++)	// '\0' 表示字符串结束
		{
			if( *p !=*pp)					//  比较 是否相等
			{
				fOK =FALSE ;				// 不相等, 设置标志位退出
			  	break ;
			}
		}
	}
	
	
	if( (fOK == FALSE) || (i == 0xFF)) 	
		return 0 ;		// 寻找失败, 返回 0x0
	return (i) ;		// 否则, 查找成功, 返回 偏移址
}

// 在buff中查找Refer字符串,串长度为Len
// 返回查找到的在原串中的偏移位置
u16 In_nStr( char* buff, char* Refer, u8 Len)
{
	u16	   	i ;
	bool   fOK =FALSE;	// 是否找到, 默认没有找到. 便于循环启动工作
	char*  		p ;			// 源字符串地址 (指针)
	char*  		pp ;		// 指向参考字符串 地址( 指针)

	for( i =0 ; (fOK == FALSE) && (i<Len); i++)		// 源字符串不能到尾
	{
		p =&buff[i] ;	// 源字符串后移, 重新定位 源字符串
		pp =Refer ;		// 参考子字符串重新定位
		fOK = TRUE ;	// 默认找到. 会中止循环
		for( ; (*pp !='\0') ; p++, pp++)	// '\0' 表示字符串结束
		{
			if( *p !=*pp)					//  比较 是否相等
			{
				fOK =FALSE ;				// 不相等, 设置标志位退出
			  	break ;
			}
		}
	}
	
	
	if( (fOK == FALSE) || (i == 0xFF)) 	
		return 0 ;		// 寻找失败, 返回 0x0
	return (i) ;		// 否则, 查找成功, 返回 偏移址
}


/*******************************************************************************
 * 函数名称 ：DataCopy()
 * 函数功能 ：数据复制. 
 * 入口参数 ：从From复制长度为len 的数据块到To
 * 出口参数 ：
 *   
 *******************************************************************************/
void DataCopy( u8* To, u8* From, u16 Len )
{
	while(Len--)
	{
		*To++ = *From++ ;
	}
} 

/*******************************************************************************
 * 函数名称 ：Compared()
 * 函数功能 ：比较两个串是否相等.
 * 入口参数 ：串1, 串2, 比较长度
 * 出口参数 ：返回值 =TRUE,两串相同, FALSE 不同
 *   
 *******************************************************************************/
bool  Compared( u8* p1, u8* p2, u8 length)
{
	for( ; length >0; length--)
	{
		if( *p1 != *p2)	return FALSE ;		// 如果两个单字节不相等, 则退出 FALSE
		p1 ++ ;								// 否则指针后移,继续比较
		p2 ++ ;
	}
	return TRUE ;
}

/*******************************************************************************
 * 函数名称 ：cal_crc（）
 * 函数功能 ：crc校验
 * 入口参数 ：unsigned char *ptr, unsigned char len
 * 出口参数 ：CRC
 *******************************************************************************/
u16 cal_crc(u8 *ptr, u8 len) 
{
	u8 i;
	u16	crc=0;
	
	while(len--) 
	{ 
		for(i=0x80; i!=0; i>>=1) 
		{ 
			if((crc&0x8000)!=0) 
			{
				crc<<=1; crc^=0x1021;
			} 
			else crc<<=1; 
			if((*ptr&i)!=0) crc^=0x1021; 
		} 
		ptr++; 
	} 
	return(crc);  

}

/*
 *******************************************************************************
 * 函数名称 ：XRL_ChkSum（）
 * 函数功能 ：ASC 码字符串校验码 异或操作
 * 入口参数 ：源字符串:buff, 计算的字符串长度:length
 * 出口参数 ：异或操作结果
 *******************************************************************************/
u8 XRL_ChkSum(u8* buff, u16 length)
{
	u8 result ;	// 保存异或结果

	for( result =0x0; length > 0 ; length --, buff++)
		result =result ^ (*buff) ;	
		
	return result ;	
}

/******************************************************************************
 * 函数名称：Check_SUM
 * 函数功能：累加校验()
 * 入口参数：
 * 出口参数：
 *******************************************************************************/
u8 Check_SUM(u8 *Buf, u8 len)
{
	u8 i;
	u16 SUMData=0;
	
	for(i=0; i<len; i++)
	{
		SUMData += Buf[i];
	}
	
	return (u8)SUMData;
}

/******************************************************************************* 
 * 函数名称 ：lrc()
 * 函数功能 ：对字符串进行LRC校验
 * 入口参数 ：*str：需要校验的字符串 lenth：字符串长度
 * 出口参数 ：LRC校验的值
 *******************************************************************************/
u8 lrc(u8 *str,u8 lenth)
{
	u8 tmp;
	tmp=0;
	
	while(lenth-- )
	{
		tmp+=*str++;
	}
	return (tmp);
}

/*******************************************************************************
 * 函数名称 ：Packet_LRC
 * 函数功能 ：
 * 入口参数 ：
 * 出口参数 ：
 *******************************************************************************/
u8 Packet_LRC(u8 *Buf, u8 Len)
{
	u8 crccheck,rcvcrc,crc_err;
	crccheck = lrc(Buf,Len);
	rcvcrc = *(Buf + Len);
	crc_err = crccheck ^ rcvcrc;
    if(crc_err)
    {
		return 1;
	}		
	return 0;
}

/*******************************************************************************
 * 函数名称 ：CRC16()
 * 函数功能 ：CRC 校验计算程序
 * 入口参数 ：需要计算 CRC 数据的缓冲区首地址.  需要计算的长度
 * 出口参数 ：计算得到的 int CRC 结果
 *******************************************************************************/
u16 CRC16(unsigned char* pDatabuf,u16 length)
{ 
	u8 i;
	u16	crc=0;
	
	while(length--) 
	{ 
		for(i=0x80; i!=0; i>>=1) 
		{ 
			if((crc&0x8000)!=0) 
			{
				crc<<=1; crc^=0x1021;
			} 
			else crc<<=1; 
			if((*pDatabuf&i)!=0) crc^=0x1021; 
		} 
		pDatabuf++; 
	} 
	return(crc);  

} 
/*******************************************************************************
 * 函数名称 ：CRC16_GX_MODBUS()
 * 函数功能 ：CRC 校验计算程序
 * 入口参数 ：需要计算 CRC 数据的缓冲区首地址.  需要计算的长度
 * 出口参数 ：计算得到的 int CRC 结果
 *******************************************************************************/
u16 CRC16_GX_MODBUS(unsigned char* pDatabuf,u16 len)
{ 

	unsigned int  xda, xdapoly;
	unsigned char i, j, xdabit;
	xda = 0xFFFF;
	xdapoly = 0xA001;
	for (i = 0; i < len; i++)
	{
		xda ^= pDatabuf[i];
		for (j = 0; j < 8; j++)
		{
			xdabit = (unsigned char)(xda & 0x01);
			xda >>= 1;
			if (xdabit == 1)
				xda ^= xdapoly;
		}
	}      
	return xda; 
      
} 

/******************************************************************************
 * 函数名称：Byte_To_ASC()
 * 函数功能：2字节拆分,若其数字为0x00-0x09，则加上0x30， 若其数字为0x0A-0x0F，则加上0x37
 * 入口参数：*ASCBuf:拆分后的数据,Byte:要拆分的数据
 * 出口参数：无
 *******************************************************************************/
void Byte_To_ASC(u8 *ASCBuf, u8 Byte)
{
	u8 Hbyte, Lbyte;		// 字节高四位和低四位
	
	Lbyte = (Byte & 0x0F);
	Hbyte = (Byte & 0xF0) >> 4;
	
	if (Lbyte >= 0x0A)
	{
		Lbyte += 0x37;
	}
	else
	{
		Lbyte += 0x30;
	}
	
	if (Hbyte >= 0x0A)
	{
		Hbyte += 0x37;
	}
	else
	{
		Hbyte += 0x30;
	}
	
	*ASCBuf = Hbyte;
	*(ASCBuf+1) = Lbyte;
}

/******************************************************************************
 * 函数名称：ASC_To_Byte()
 * 函数功能：2字节拆分的数据合并
 * 入口参数：Hbyte:高字节数据,Lbyte:低字节数据
 * 出口参数：合并后的数据
 *******************************************************************************/
u8 ASC_To_Byte(u8 Hbyte, u8 Lbyte)
{
	if (Lbyte >= 'A')
	{
		Lbyte = (Lbyte-0x37) & 0x0F;
	}
	else
	{
		Lbyte = (Lbyte-0x30) & 0x0F;
	}
	
	if (Hbyte >= 'A')
	{
		Hbyte = (Hbyte-0x37) & 0x0F;
	}
	else
	{
		Hbyte = (Hbyte-0x30) & 0x0F;
	}
	
	return ((Hbyte<<4) | Lbyte);
}  

/******************************************************************************
 * 函数名称：AscStrToHex()
 * 函数功能：数字字符串转换成十六进制
 * 入口参数：*dec:源字符串
 * 出口参数：转换后的结果
 *******************************************************************************/
u32 AscStrToHex(char* dec)
{
	u8 i;
	u32 result =0x0;
	
	for (i=0; i<10; i++, dec++)					// 数字字符最长不超过10
	{
		if ((*dec > '9') || (*dec < '0'))		// 遇到非数字字符返回
			return result;
		
		result =result *10 + ( (*dec) & 0x0F) ;	// * 10 的目的 是为了移动字节
	}
	
	return result;
}

/******************************************************************************
 * 函数名称：In_nCmp()
 * 函数功能：比较数据串是否部分匹配
 * 入口参数：p1:串1,p1:串2,p1Len:串1长度,p2Len:串2长度,CmpLen:匹配长度
 * 出口参数：TRUE:比较通过,FALSE:比较失败
 *******************************************************************************/
bool In_nCmp(u8* p1, u8 p1Len, u8* p2, u8 p2Len, u8 CmpLen)
{
	u8 i, j;
	
	if ((p1Len < CmpLen) || (p2Len < CmpLen) || (CmpLen >= 200))	// 数据串长度不对
	{
		return FALSE;
	}
			
	for (i=0; i<=(p2Len-CmpLen); i++)								// 查询匹配
	{
		for (j=0; j<=(p1Len-CmpLen); j++)
		{
			if(Compared(p1+j, p2+i, CmpLen) == TRUE)				// 比较两数据串是否相同
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}


/******************************************************************************
 * 函数名称：floatTOuint32()
 * 函数功能：浮点数转成4字节数据
 * 入口参数：fdata
 * 出口参数：databyte
 *******************************************************************************/
void floatTOuint32(float fdata, u8 *datbyte)
{ 
	union Conv 	{ 
		float f; 
		u8 b[4]; 
	}Data; 
	Data.f = fdata;	  

	*datbyte++ = Data.b[0]; 
	*datbyte++ = Data.b[1]; 
	*datbyte++ = Data.b[2];
	*datbyte = Data.b[3];	 
}
void FloatToByte(float floatNum, unsigned char* byteArry)//浮点数转十六进制
{
	int i;
	unsigned char* pchar=(unsigned char*)&floatNum;
	for(i=0;i<4;i++)
	{
		*byteArry=*pchar;
		pchar++;
		byteArry++;
	}
	
}
float Hex_To_Decimal(unsigned char *Byte,int num)//十六进制转浮点数
{
	char table[4];
	int i=0;
	for(i=0;i<4;i++)
	{
		table[i]=Byte[num+i];
	}

	return *((float*)table);
}





