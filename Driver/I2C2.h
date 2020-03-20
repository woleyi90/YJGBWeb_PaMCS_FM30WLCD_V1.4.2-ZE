/*******************************************************************************
 *
 * Filename  : I2C2.h
 *
 * Function  : Reference head file for "I2C2" library.
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
 
#ifndef __I2C2_H
#define __I2C2_H


#include "stm32f10x.h"
					  



#define MCP4461
#define ADDR_MCP4461         0x58


#define ADDR_PORT0_SWR       0x00
#define ADDR_PORT0_SWR_NV    0x02

#define ADDR_PORT1_TDALC     0x01
#define ADDR_PORT1_TDALC_NV  0x03

#define ADDR_PORT2_ATT       0X06
#define ADDR_P0RT2_ATT_NV    0X08

#define ADDR_PORT3_ALC       0X07
#define ADDR_PORT3_ALC_NV    0X08


void I2C_Configuration(void);
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);

bool I2C_SetVal(uint8_t addr, uint16_t value);
uint16_t I2C_ReadVal(uint8_t addr);
 



#endif 

