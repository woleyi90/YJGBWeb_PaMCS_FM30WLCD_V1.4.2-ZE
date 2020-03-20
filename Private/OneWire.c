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
#include "delay.h"



/*******************************************************************************
 * 名    称：Onewire_Enable_GPIO_Port(void)
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
/*void Onewire_Enable_GPIO_Port(void)
{
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
}*/

/*******************************************************************************
 * 名    称：Onewire_OUT_PULL_UP(void)
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void Onewire_OUT_PULL_UP(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
}

/*******************************************************************************
 * 名    称：Onewire_OUT_FLOATING(void)
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void Onewire_OUT_FLOATING(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
}

/*******************************************************************************
 * 名    称：Onewire_IN_FLOATING(void)
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void Onewire_IN_FLOATING(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
}

/*******************************************************************************
 * 名    称：Onewire_IN_PULL_UP
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void Onewire_IN_PULL_UP(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
}


/*******************************************************************************
 * 名    称：mymydelay_us()
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void mydelay_us(u16 us)
{

	int deycnt = 0;

	deycnt=17*us;
	while(--deycnt);


}


/*******************************************************************************
 * 名    称：resetOnewire()
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
/* Reset */
void resetOnewire(void) 
{
	Onewire_OUT_PULL_UP();
	CLR_DQ();
	mydelay_us(450);
	SET_DQ();
	mydelay_us(60);
	Onewire_IN_PULL_UP(); 
	mydelay_us(10);
	while(!(GET_DQ()));
	Onewire_OUT_PULL_UP();
	SET_DQ();
}

/*******************************************************************************
 * 名    称：rOnewire()
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/ 
u8 rOnewire(void)
{
    u8 data=0,i=0;
    for(i=0;i<8;i++)
    {
	Onewire_OUT_PULL_UP();
	CLR_DQ();
	data=data>>1;
	SET_DQ();
	Onewire_IN_PULL_UP();
	mydelay_us(8);
	if(GET_DQ())data|=0x80;
	Onewire_OUT_PULL_UP();
	SET_DQ();
	mydelay_us(60);
    }
    return(data);
}


/*******************************************************************************
 * 名    称：wOnewire()
 * 功    能：
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void wOnewire(u8 data)
{
	u8 i=0;
	Onewire_OUT_PULL_UP();
	SET_DQ();
	mydelay_us(16);
    	for(i=0;i<8;i++)
    	{
		CLR_DQ();
		if(data&0x01)
		{
			SET_DQ();
		}
   	 	else
   	 	{
			CLR_DQ();
    		}
    		data=data>>1;
		mydelay_us(40);  //65
		SET_DQ();
    }
}
