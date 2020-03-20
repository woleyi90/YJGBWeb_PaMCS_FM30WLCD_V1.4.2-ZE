/*******************************************************************************
 *
 * Filename  : flash.c
 *
 * Function  : Implememt FLASH function form for this workbench.
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
 * 函数名称：Write_Flash()
 * 函数功能：向FLASH写入数据。
 * 入口参数：*Data RAM起始地址, Address FLASH地址,
 *           count 复制字节个数，必须能被4整除
 * 出口参数：TRUE OR FLASE
 *******************************************************************************/
bool Write_Flash(u8 *Data, u32 NumByteToWrite)
{   
	OS_CPU_SR  cpu_sr;

	FLASH_Status FLASHStatus = FLASH_COMPLETE;

	u32 datasource;
	u32 Address = 0x00;
	u32 EraseCounter = 0x00	; //页数
	vu32 NbrOfPage = 0x00;
	u32 EndAddr = StartAddr + NumByteToWrite; 		

	/* Define the number of page to be erased */
 	NbrOfPage = NumByteToWrite / FLASH_PAGE_SIZE; 
		
	if(Address > MaxAddress)
    {
    	return FALSE;
    }
			
	OS_ENTER_CRITICAL();  	
	FLASH_Unlock();   
	
 	/* Clear All pending flags */
  	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	/* Erase the FLASH pages */
  	for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  	{
    	FLASHStatus = FLASH_ErasePage(StartAddr + (FLASH_PAGE_SIZE * EraseCounter));
#if IWDG_EN
		IWDG_ReloadCounter(); //手动喂狗  
#endif 		
  	} 
	
	/*  FLASH Word program of data 0x15041979 at addresses defined by StartAddr and EndAddr*/
  	Address = StartAddr;
	datasource = (u32)Data;

  	while((Address < EndAddr) && (FLASHStatus == FLASH_COMPLETE))
  	{
    	FLASHStatus = FLASH_ProgramWord(Address, *(u32 *)datasource);
    	Address = Address + 4;
		datasource +=4;	 	    
#if IWDG_EN
		IWDG_ReloadCounter(); //手动喂狗  
#endif  	
 	} 
	FLASH_Lock();
	OS_EXIT_CRITICAL();

	return TRUE;

}

/*******************************************************************************
 * 函数名称：CheckICON()
 * 函数功能：检查标志位,由此判断FLASH是否已经初始化过。
 * 入口参数：icon 标志数据
 * 出口参数：TRUE RO FALSE
 *******************************************************************************/
bool CheckICON(u8 icon, u8 *addr)
{
	u8 FLASH_ICON;
	FLASH_ICON =  *addr;
	if (icon == FLASH_ICON) 
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

/*******************************************************************************
 * 函数名称：FlashToRam()
 * 函数功能：读取FLASH的数据到RAM。
 * 入口参数：dst  目标存储区，
 *           Psrc 源地址，即FLASH起始地址。
 *           no   复制字节个数
 * 出口参数：TRUE OR FALSE
 *******************************************************************************/
u8  FlashToRam(u8 dst[], u8 *Psrc, u32 no)
{  
	u32 i;
	for (i=0; i<no; i++)
	{
		dst[i] = *(Psrc++);
	}
    
    return TRUE; // 返回状态码	
} 


   

/*
	函数功能: 	不检查的写入
  	输入参数:	WriteAddr: 起始地址
				pBuffer: 数据指针
				NumToWrite: 半字(16位)数  
*/
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)   
{ 			 		 
	u16 i;
	
	for(i = 0; i < NumToWrite; i++)
	{
		FLASH_ProgramHalfWord(WriteAddr, pBuffer[i]);  //在指定的地址写入半字
	    WriteAddr += 2;  //地址增加2.
	}  
} 

//看手册可知大容量芯片的每个扇区是2K大小，而小容量产品每个扇区是1K大小
#if STM32_FLASH_SIZE < 256

	#define STM_SECTOR_SIZE 1024  //字节
	
#else 

	#define STM_SECTOR_SIZE	2048

#endif		 

u16 STMFLASH_BUF[STM_SECTOR_SIZE / 2];  //定义数组最大为2K字节

/*
	函数功能: 	从指定地址开始写入指定长度的数据
  	输入参数:	WriteAddr: 起始地址(此地址必须为2的倍数!!)
				pBuffer: 数据指针
				NumToWrite:半字(16位)数 (就是要写入的16位数据的个数.) 
*/
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)	
{
	u16 i;
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	       
	u32 offaddr;   //去掉0X08000000后的地址

	//先判断地址是否合法
	if(WriteAddr < STM32_FLASH_BASE || (WriteAddr >= (STM32_FLASH_BASE + 1024 * STM32_FLASH_SIZE)))
	{
		return;  
	}
	
	FLASH_Unlock();	 //解锁
	
	offaddr = WriteAddr - STM32_FLASH_BASE;		//实际偏移地址.
	secpos = offaddr / STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff = (offaddr % STM_SECTOR_SIZE) / 2;   //在扇区内的偏移(u16占两个字节，故以2个字节为基本单位.)
	secremain = STM_SECTOR_SIZE / 2 -secoff;	//扇区剩余空间大小(u16 占2个字节)  
	
	if(NumToWrite <= secremain)
	{
		secremain = NumToWrite;  //不大于该扇区范围
	}
	
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);  //读出整个扇区的内容
		
		for(i = 0; i < secremain; i++)  //校验数据是否为0xffff,STM32内部FLASH在写扇区前也要保证所写扇区被擦除，即值为0xffff
		{
			if(STMFLASH_BUF[secoff + i] != 0XFFFF)
			{
				break;  //需要擦除  	 
			} 
		}
		
		if(i < secremain)  //需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE);  //擦除这个扇区
			
			for(i = 0; i < secremain; i++)  //复制
			{
				STMFLASH_BUF[i+secoff] = pBuffer[i];	  
			}
			
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE + STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE / 2);  //写入整个扇区  
		}
		else 
		{
			STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);  //扇区已经擦除了的,直接写入扇区剩余区间. 	
		}
		
		if(NumToWrite == secremain)
		{
			break;  //写入结束了
		}
		else  //写入未结束
		{
			secpos++;	//扇区地址增1
			secoff = 0;  //偏移位置为0 	 
		   	pBuffer += secremain;  	//指针偏移
			WriteAddr += secremain;	 //写地址偏移	   
		   	NumToWrite -= secremain;  //字节(16位)数递减
			
			if(NumToWrite > (STM_SECTOR_SIZE / 2))
			{
				secremain = STM_SECTOR_SIZE / 2;  //下一个扇区还是写不完
			}
			else 
			{
				secremain = NumToWrite;  //下一个扇区可以写完了
			}
		}	 
	}	
	
	FLASH_Lock();  //上锁
}


/*
	函数功能:	读取指定地址的半字(16位数据)
  	输入参数:	faddr:读地址(此地址必须为2的倍数!!)
	返回值: 	对应数据
*/
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

/*
	函数功能: 	从指定地址开始读出指定长度的数据
  	输入参数:	ReadAddr: 起始地址
				pBuffer: 数据指针
				NumToWrite:半字(16位)数 
*/
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)   	
{
	u16 i;
	
	for(i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);  //读取2个字节.
		ReadAddr += 2;  //偏移2个字节.	
	}
}

/*
	函数功能: 	测试函数
  	输入参数:	WriteAddr:起始地址
				WriteData:要写入的数据
*/
void STMFLASH_WtiteU16DataToFlash(u32 WriteAddr, u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData, 1);  //写入一个字 
}




