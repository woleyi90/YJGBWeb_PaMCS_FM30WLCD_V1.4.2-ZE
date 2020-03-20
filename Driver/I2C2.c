/*******************************************************************************
 *
 * Filename  : I2C2.c
 *
 * Function  : Implememt I2C2 function form for this workbench.
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




/* 
 *******************************************************************************
 *
 * private routines prototype
 * 
 *******************************************************************************
 */
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value);
uint8_t I2C_WriteTwoByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint16_t value);
static void I2C_delay(uint16_t cnt);




 
 /*
 *******************************************************************************
 *
 * Private routines
 *
 *******************************************************************************/
/*******************************************************************************
* Function Name  : I2C_SetVal
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
bool I2C_SetVal(uint8_t addr, uint16_t value)
{
  	uint16_t data;
	uint16_t data_read;
	uint8_t re_readtimes;

	data = value *32 /63; 
    
	if(I2C_WriteTwoByte(I2C1, ADDR_MCP4461, addr, data)==FALSE)
	{
		printf( "MCP4461 write fault!\n");
		return FALSE;
	}	
	data_read =	I2C_ReadVal(addr);	

	for(re_readtimes=0; re_readtimes<3; re_readtimes++)
	{ 	

		if(data_read == data)
		{
	 		return TRUE;
		}
		else
		{
	  		if(I2C_WriteTwoByte(I2C1, ADDR_MCP4461, addr, data)==FALSE)
	  		{
				return FALSE;
	  		}  			
			data_read =	I2C_ReadVal(addr);
		}
	}
	return FALSE;	
}  


/*******************************************************************************
* Function Name  : I2C_ReadVal
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
uint16_t I2C_ReadVal(uint8_t addr)
{
   	uint16_t data16; 

	I2C_Read(I2C1,ADDR_MCP4461, addr, (uint8_t *)&data16, 2); 
	data16 = ((data16 & 0x0001) <<8) | (data16>>8) ;   //高低字节对换
	
	return data16;
} 
 


/*******************************************************************************
* Function Name  : I2C_delay
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_delay(uint16_t cnt)
{
	while(cnt--);
}




/* 
 *******************************************************************************
 *
 * private routines
 * 
 *******************************************************************************
 */
/*******************************************************************************
* Function Name  : I2C_AcknowledgePolling
* Description    : I2C待机模式
* Input          : I2C_TypeDef * , uint8_t
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2Cx, ENABLE);

    SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);

#ifdef MCP4461

	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
#else

	I2C_Send7bitAddress(I2Cx, 0, I2C_Direction_Transmitter);
#endif

  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
  
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
    
  I2C_GenerateSTOP(I2Cx, ENABLE);
}


/*******************************************************************************
* Function Name  : I2C_Read
* Description    : 
* Input          : 
* Output         : 
* Return         : 
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
    if(num==0)
	return 1;
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
		
	I2C_AcknowledgeConfig(I2Cx, ENABLE);


    I2C_GenerateSTART(I2Cx, ENABLE);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

#ifdef MCP4461
    I2C_Send7bitAddress(I2Cx,  I2C_Addr, I2C_Direction_Transmitter); //发送器件地址
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	addr=(addr<<4) | 0x0c;  //AD3-AD0 11 XX
	I2C_SendData(I2Cx, addr);	// 发送控制命令
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
	I2C_GenerateSTART(I2Cx, ENABLE);  //发送重启位
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Receiver); //接收模式
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		
#else
	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
#endif
	
    while (num)
    {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2Cx, DISABLE);
    		I2C_GenerateSTOP(I2Cx, ENABLE);
		}
	    
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */

#ifdef MCP4461	 //接收数据为两字节  
	    *buf = I2C_ReceiveData(I2Cx);
	    buf++;
#else
		*buf = I2C_ReceiveData(I2Cx);
	    buf++;
#endif
	    /* Decrement the read bytes counter */
	    num--;
    }

	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	I2C_delay(1000);

	return 0;
}	

/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : 
* Input          : 
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value)
{
  	
	I2C_GenerateSTART(I2Cx, ENABLE);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef MCP4461
  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
	addr=(addr<<4) ;  //AD3-AD0 00 xx
  	I2C_SendData(I2Cx, addr);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Transmitter);
 	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

  	I2C_SendData(I2Cx, value); 
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	I2C_GenerateSTOP(I2Cx, ENABLE);
  
  	I2C_AcknowledgePolling(I2Cx,I2C_Addr);

	I2C_delay(1000);

	return 0;
}


/*******************************************************************************
* Function Name  : I2C_WriteOneByte
* Description    : 
* Input          : 
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
uint8_t I2C_WriteTwoByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint16_t value)
{
  	uint8_t D8;
	uint8_t data;

	uint8_t timeout;

	D8   = (uint8_t)((value & 0x0100)>>8);
	data =  (uint8_t)value;

	I2C_GenerateSTART(I2Cx, ENABLE);
    for(timeout=0; timeout<30; timeout++)
    {
     	if (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT) !=0)
     	{
			break;
     	}
		OSTimeDlyHMSM(0,0,0,10);
    }
	if(timeout==30)
	{
		return FALSE;
	}
	
  	//while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));  

#ifdef MCP4461
  	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
	
	for(timeout=0; timeout<30; timeout++)
    {
		if(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=0)
		{
			break;
		}
		OSTimeDlyHMSM(0,0,0,10);
    }
	if(timeout==30)
	{
		return FALSE;
	}
	//while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
	addr=(addr<<4) | D8  ;  //AD3-AD0 00 xx
  	I2C_SendData(I2Cx, addr);

	for(timeout=0; timeout<30; timeout++)
    {
		if(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=0)
		{
			break;
		}
		OSTimeDlyHMSM(0,0,0,10);	
    }
	if(timeout==30)
	{
		return FALSE;
	}
	//while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

#else	
	I2C_Send7bitAddress(I2Cx, addr<<1, I2C_Direction_Transmitter);
		
	for(timeout=0; timeout<30; timeout++)
    {
		if(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=0)
		{
			break;
		}
		OSTimeDlyHMSM(0,0,0,10);	
    }
	if(timeout==30)
	{
		return FALSE;
	}
	//SW_WWDOG_ON;
	//while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
#endif

  	I2C_SendData(I2Cx, data);  

    for(timeout=0; timeout<30; timeout++)
    {
		if(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)!=0)
		{
			break;
		}
		OSTimeDlyHMSM(0,0,0,10);	
    }
	if(timeout==30)
	{
		return FALSE;
	} 
  	//while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  	I2C_GenerateSTOP(I2Cx, ENABLE);
  
  	I2C_AcknowledgePolling(I2Cx,I2C_Addr);

	I2C_delay(1000);

	return TRUE;
}


/*******************************************************************************
* Function Name  : I2C_Write
* Description    : 
* Input          : 
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
	uint8_t err=0;
	
	while(num--)
	{
		if(I2C_WriteOneByte(I2Cx, I2C_Addr,addr++,*buf++))
		{
			err++;
		}
	}
	if(err)
		return 1;
	else 
		return 0;	
}

