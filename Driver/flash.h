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

//#define StartAddr          ((u32)0x08037000)	//���ݴ洢��ʼ��ַ��220K����+30K���ݣ�
//#define	MaxAddress         ((u32)0x0803E800)    //���ݴ洢������ַ	
//#define FLASH_PAGE_SIZE    ((u16)0x400)         //1ҳ��С,�����Եó�����Ϊ0x400���ܱ������� 	

#define StartAddr          ((u32)0x0807D000)	//���ݴ洢��ʼ��ַ��500K����+10K���ݣ�
#define	MaxAddress         ((u32)0x0807F800)    //���ݴ洢������ַ	
#define FLASH_PAGE_SIZE    ((u16)0x800)         //1ҳ��С


bool Write_Flash(u8 *Data, u32 NumByteToWrite);
bool CheckICON(u8 icon, u8 *addr);
u8  FlashToRam(u8 dst[], u8 *Psrc, u32 no);


//������ѡ���оƬ������
#define STM32_FLASH_SIZE 512 	 			//RCT6��ѡSTM32��FLASH������С(��λΪK)
//#define STM32_FLASH_SIZE        128 	 	//RBT6 ��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN        1           //ʹ��FLASHд��(0��������;1��ʹ��)
#define STM32_FLASH_BASE       0x08000000 	//STM32 FLASH����ʼ��ַ
#define STM32_FLASH_SAVE_TOUCH_ADDR  0x08020000 	//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)
#define STM32_FLASH_SAVE_RESOLUATION_ADDR  0X08030000 
 
u16 STMFLASH_ReadHalfWord(u32 faddr);  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void STMFLASH_WtiteU16DataToFlash(u32 WriteAddr,u16 WriteData);								   



#endif /* __FLASH_H */
