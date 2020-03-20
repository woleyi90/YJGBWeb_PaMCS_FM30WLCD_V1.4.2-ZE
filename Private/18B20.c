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
 * 函数名称：readTemp(void) 
 * 函数功能：读取温度
 * 入口参数：无
 * 出口参数：无
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
        temp=0;//温度为负  
    }
	else 
	{
		temp=1;//温度为正
	}
		  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    realtpt=(int)tem*0.625;//转换   
   
	if(temp)
	{
		return realtpt; //返回温度值
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
 * 函数名称：convertDs18b20(void) 
 * 函数功能：温度转换
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
void convertDs18b20(void) 
{ 
    resetOnewire(); 
    wOnewire(0xcc); 
    wOnewire(0x44); 
}


/*******************************************************************************
 * 函数名称：readID(void)
 * 函数功能：读取ID
 * 入口参数：无
 * 出口参数：无
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

