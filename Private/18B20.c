/*******************************************************************************
 *
 * Filename  : adc.c
 *
 * Function  : Implememt ADC function form for this workbench.
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
 ******************************************************************************/

#include "includes.h"
#include "global.h"	




void convertDs18b20(void);
u8* readID(void) ;

/*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */
/*******************************************************************************
 * �������ƣ�readTemp(void) 
 * �������ܣ���ȡ�¶�
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
int readTemp(void) 
{ 
	u8 TL,TH,temp;
	short tem;
	int realtpt=0;
	
	convertDs18b20();
	resetOnewire(); 
	wOnewire(0xcc); 
	wOnewire(0xbe); 	
	
	TL=rOnewire();  // LSB   
    TH=rOnewire();  // MSB  

//	UART0Write(&TH,1);
//	UART0Write(&TL,1);
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//�¶�Ϊ��  
    }
	else 
	{
		temp=1;//�¶�Ϊ��
	}
		  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    realtpt=(int)tem*0.625;//ת��   
   
	if(temp)
	{
		return realtpt; //�����¶�ֵ
	}
	else 
	{
		return -realtpt;
	}
}

 
/*
 *******************************************************************************
 *
 * Private routines
 *
 *******************************************************************************/
/*******************************************************************************
 * �������ƣ�convertDs18b20(void) 
 * �������ܣ��¶�ת��
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
void convertDs18b20(void) 
{ 
    resetOnewire(); 
    wOnewire(0xcc); 
    wOnewire(0x44); 
}


/*******************************************************************************
 * �������ƣ�readID(void)
 * �������ܣ���ȡID
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
u8* readID(void) 
{ 
	u8 ID[8],i,*p;
	resetOnewire(); 
	wOnewire(0x33);
	for(i=0;i<8;i++)
	{ID[i]=rOnewire();}
	p=ID;
	return p;
} 

