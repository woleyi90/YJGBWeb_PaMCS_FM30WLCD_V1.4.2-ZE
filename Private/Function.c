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
 * �������� ��HI()
 * �������� ��ȡ˫�ֽڸ�8λ
 * ��ڲ��� ��unsigned int num_16
 * ���ڲ��� ��HI8
 *******************************************************************************/
u8 HI(u16 num_16)
{
	unsigned int tt;
	tt=num_16&0xff00;
	tt=tt>>8;
	return ((unsigned char )tt);
}

/*******************************************************************************
 * �������� ��LOW()
 * �������� ��ȡ˫�ֽڵ�8λ
 * ��ڲ��� ��unsigned int num_16
 * ���ڲ��� ��LOW8
 *******************************************************************************/
u8 LOW(u16 num_16)
{
	unsigned char tt;
	tt=num_16 & 0x00ff;
	return tt;
}
/*******************************************************************************
 * �������� ��StrToU16()
 * �������� ����˫�ֽ�ת����u16�ֽ�,�߰�λ��ǰ���Ͱ�λ�ں�
 * ��ڲ��� ��u8* buf
 * ���ڲ��� ��u16
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
 * �������� ��in_Str()
 * �������� ����ָ���ַ��� buff ��Ѱ��ָ���ַ���. 
 * ��ڲ��� ���ַ����׵�ַ, ��Ѱ�ҵ��ַ���,
 * ���ڲ��� ���ҵ�ƥ���ַ�����Դ�ַ����е�ƫ�Ƶ�ַ
 *         <=0: ����ʧ��, û���ҵ�ָ���ַ���, > 0 : �ҵ�
 *		   Դ�ַ���ָ��Ҳ���ƶ�
 *******************************************************************************/
u16 In_Str( char* buff, char* Refer)
{
	u16	   	i ;
	bool   fOK =FALSE;	// �Ƿ��ҵ�, Ĭ��û���ҵ�. ����ѭ����������
	char*  		p ;			// Դ�ַ�����ַ (ָ��)
	char*  		pp ;		// ָ��ο��ַ��� ��ַ( ָ��)

	for( i =0 ; (fOK == FALSE) && (buff[i] !=0x0) && ( i <0xFF ) ; i++)		// Դ�ַ������ܵ�β
	{
		p =&buff[i] ;	// Դ�ַ�������, ���¶�λ Դ�ַ���
		pp =Refer ;		// �ο����ַ������¶�λ
		fOK = TRUE ;	// Ĭ���ҵ�. ����ֹѭ��
		for( ; (*pp !='\0') ; p++, pp++)	// '\0' ��ʾ�ַ�������
		{
			if( *p !=*pp)					//  �Ƚ� �Ƿ����
			{
				fOK =FALSE ;				// �����, ���ñ�־λ�˳�
			  	break ;
			}
		}
	}
	
	
	if( (fOK == FALSE) || (i == 0xFF)) 	
		return 0 ;		// Ѱ��ʧ��, ���� 0x0
	return (i) ;		// ����, ���ҳɹ�, ���� ƫ��ַ
}

// ��buff�в���Refer�ַ���,������ΪLen
// ���ز��ҵ�����ԭ���е�ƫ��λ��
u16 In_nStr( char* buff, char* Refer, u8 Len)
{
	u16	   	i ;
	bool   fOK =FALSE;	// �Ƿ��ҵ�, Ĭ��û���ҵ�. ����ѭ����������
	char*  		p ;			// Դ�ַ�����ַ (ָ��)
	char*  		pp ;		// ָ��ο��ַ��� ��ַ( ָ��)

	for( i =0 ; (fOK == FALSE) && (i<Len); i++)		// Դ�ַ������ܵ�β
	{
		p =&buff[i] ;	// Դ�ַ�������, ���¶�λ Դ�ַ���
		pp =Refer ;		// �ο����ַ������¶�λ
		fOK = TRUE ;	// Ĭ���ҵ�. ����ֹѭ��
		for( ; (*pp !='\0') ; p++, pp++)	// '\0' ��ʾ�ַ�������
		{
			if( *p !=*pp)					//  �Ƚ� �Ƿ����
			{
				fOK =FALSE ;				// �����, ���ñ�־λ�˳�
			  	break ;
			}
		}
	}
	
	
	if( (fOK == FALSE) || (i == 0xFF)) 	
		return 0 ;		// Ѱ��ʧ��, ���� 0x0
	return (i) ;		// ����, ���ҳɹ�, ���� ƫ��ַ
}


/*******************************************************************************
 * �������� ��DataCopy()
 * �������� �����ݸ���. 
 * ��ڲ��� ����From���Ƴ���Ϊlen �����ݿ鵽To
 * ���ڲ��� ��
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
 * �������� ��Compared()
 * �������� ���Ƚ��������Ƿ����.
 * ��ڲ��� ����1, ��2, �Ƚϳ���
 * ���ڲ��� ������ֵ =TRUE,������ͬ, FALSE ��ͬ
 *   
 *******************************************************************************/
bool  Compared( u8* p1, u8* p2, u8 length)
{
	for( ; length >0; length--)
	{
		if( *p1 != *p2)	return FALSE ;		// ����������ֽڲ����, ���˳� FALSE
		p1 ++ ;								// ����ָ�����,�����Ƚ�
		p2 ++ ;
	}
	return TRUE ;
}

/*******************************************************************************
 * �������� ��cal_crc����
 * �������� ��crcУ��
 * ��ڲ��� ��unsigned char *ptr, unsigned char len
 * ���ڲ��� ��CRC
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
 * �������� ��XRL_ChkSum����
 * �������� ��ASC ���ַ���У���� ������
 * ��ڲ��� ��Դ�ַ���:buff, ������ַ�������:length
 * ���ڲ��� �����������
 *******************************************************************************/
u8 XRL_ChkSum(u8* buff, u16 length)
{
	u8 result ;	// ���������

	for( result =0x0; length > 0 ; length --, buff++)
		result =result ^ (*buff) ;	
		
	return result ;	
}

/******************************************************************************
 * �������ƣ�Check_SUM
 * �������ܣ��ۼ�У��()
 * ��ڲ�����
 * ���ڲ�����
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
 * �������� ��lrc()
 * �������� �����ַ�������LRCУ��
 * ��ڲ��� ��*str����ҪУ����ַ��� lenth���ַ�������
 * ���ڲ��� ��LRCУ���ֵ
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
 * �������� ��Packet_LRC
 * �������� ��
 * ��ڲ��� ��
 * ���ڲ��� ��
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
 * �������� ��CRC16()
 * �������� ��CRC У��������
 * ��ڲ��� ����Ҫ���� CRC ���ݵĻ������׵�ַ.  ��Ҫ����ĳ���
 * ���ڲ��� ������õ��� int CRC ���
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
 * �������� ��CRC16_GX_MODBUS()
 * �������� ��CRC У��������
 * ��ڲ��� ����Ҫ���� CRC ���ݵĻ������׵�ַ.  ��Ҫ����ĳ���
 * ���ڲ��� ������õ��� int CRC ���
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
 * �������ƣ�Byte_To_ASC()
 * �������ܣ�2�ֽڲ��,��������Ϊ0x00-0x09�������0x30�� ��������Ϊ0x0A-0x0F�������0x37
 * ��ڲ�����*ASCBuf:��ֺ������,Byte:Ҫ��ֵ�����
 * ���ڲ�������
 *******************************************************************************/
void Byte_To_ASC(u8 *ASCBuf, u8 Byte)
{
	u8 Hbyte, Lbyte;		// �ֽڸ���λ�͵���λ
	
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
 * �������ƣ�ASC_To_Byte()
 * �������ܣ�2�ֽڲ�ֵ����ݺϲ�
 * ��ڲ�����Hbyte:���ֽ�����,Lbyte:���ֽ�����
 * ���ڲ������ϲ��������
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
 * �������ƣ�AscStrToHex()
 * �������ܣ������ַ���ת����ʮ������
 * ��ڲ�����*dec:Դ�ַ���
 * ���ڲ�����ת����Ľ��
 *******************************************************************************/
u32 AscStrToHex(char* dec)
{
	u8 i;
	u32 result =0x0;
	
	for (i=0; i<10; i++, dec++)					// �����ַ��������10
	{
		if ((*dec > '9') || (*dec < '0'))		// �����������ַ�����
			return result;
		
		result =result *10 + ( (*dec) & 0x0F) ;	// * 10 ��Ŀ�� ��Ϊ���ƶ��ֽ�
	}
	
	return result;
}

/******************************************************************************
 * �������ƣ�In_nCmp()
 * �������ܣ��Ƚ����ݴ��Ƿ񲿷�ƥ��
 * ��ڲ�����p1:��1,p1:��2,p1Len:��1����,p2Len:��2����,CmpLen:ƥ�䳤��
 * ���ڲ�����TRUE:�Ƚ�ͨ��,FALSE:�Ƚ�ʧ��
 *******************************************************************************/
bool In_nCmp(u8* p1, u8 p1Len, u8* p2, u8 p2Len, u8 CmpLen)
{
	u8 i, j;
	
	if ((p1Len < CmpLen) || (p2Len < CmpLen) || (CmpLen >= 200))	// ���ݴ����Ȳ���
	{
		return FALSE;
	}
			
	for (i=0; i<=(p2Len-CmpLen); i++)								// ��ѯƥ��
	{
		for (j=0; j<=(p1Len-CmpLen); j++)
		{
			if(Compared(p1+j, p2+i, CmpLen) == TRUE)				// �Ƚ������ݴ��Ƿ���ͬ
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}


/******************************************************************************
 * �������ƣ�floatTOuint32()
 * �������ܣ�������ת��4�ֽ�����
 * ��ڲ�����fdata
 * ���ڲ�����databyte
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
void FloatToByte(float floatNum, unsigned char* byteArry)//������תʮ������
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
float Hex_To_Decimal(unsigned char *Byte,int num)//ʮ������ת������
{
	char table[4];
	int i=0;
	for(i=0;i<4;i++)
	{
		table[i]=Byte[num+i];
	}

	return *((float*)table);
}




