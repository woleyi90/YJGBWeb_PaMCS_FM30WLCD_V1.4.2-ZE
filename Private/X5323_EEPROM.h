#ifndef __X5323_EEPROM_H
#define __X5323_EEPROM_H


#include "includes.h"

#define		X5323_WREN	0x06
#define		X5323_WRDI	0x04
#define		X5323_RDSR	0x05
#define		X5323_WRSR	0x01
#define		X5323_READ	0x03
#define		X5323_WRITE	0x02			 

#define 	X5323_CS	 		 GPIO_Pin_6
#define 	X5323_CSL()		     GPIOB->BRR = X5323_CS;
#define 	X5323_CSH()		     GPIOB->BSRR = X5323_CS;
	 


void SPI1_Init(void);
unsigned char SPI1_ReadWrite(unsigned char writedat);
void X5323_Write(unsigned char writedat);
void WriteByteToX5323(unsigned int addr,unsigned char dat);
unsigned char ReadByteFromX5323(unsigned int addr);
char Wait_For_RDY(void);
void ReadbufFromX5323(unsigned int addr,unsigned int len,u8 *pbuf);
u8 WritebufToX5323(unsigned int addr,unsigned int len,u8 *buf);
#endif

