/*******************************************************************************
 *
 * Filename  : flash_api.h
 *
 * Function  : Reference head file for "FLASH_API" library.
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


#ifndef __FLASH_H 
#define __FLASH_H 

#include  <stm32f10x.h>

//#define StartAddr          ((u32)0x08037000)	//数据存储起始地址（220K程序+30K数据）
//#define	MaxAddress         ((u32)0x0803E800)    //数据存储结束地址	
//#define FLASH_PAGE_SIZE    ((u16)0x400)         //1页大小,经测试得出定义为0x400才能保存数据 	

#define StartAddr          ((u32)0x0807D000)	//数据存储起始地址（500K程序+10K数据）
#define	MaxAddress         ((u32)0x0807F800)    //数据存储结束地址	
#define FLASH_PAGE_SIZE    ((u16)0x800)         //1页大小


bool Write_Flash(u8 *Data, u32 NumByteToWrite);
bool CheckICON(u8 icon, u8 *addr);
u8  FlashToRam(u8 dst[], u8 *Psrc, u32 no);


//根据所选择的芯片来设置
#define STM32_FLASH_SIZE 512 	 			//RCT6所选STM32的FLASH容量大小(单位为K)
//#define STM32_FLASH_SIZE        128 	 	//RBT6 所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN        1           //使能FLASH写入(0，不是能;1，使能)
#define STM32_FLASH_BASE       0x08000000 	//STM32 FLASH的起始地址
#define STM32_FLASH_SAVE_TOUCH_ADDR  0x08020000 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
#define STM32_FLASH_SAVE_RESOLUATION_ADDR  0X08030000 
 
u16 STMFLASH_ReadHalfWord(u32 faddr);  //读出半字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据

//测试写入
void STMFLASH_WtiteU16DataToFlash(u32 WriteAddr,u16 WriteData);								   



#endif /* __FLASH_H */
