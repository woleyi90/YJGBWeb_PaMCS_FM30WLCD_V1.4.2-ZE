#ifndef __SRAM_H
#define __SRAM_H															    
#include  "stm32f10x_conf.h" 

											  
void FSMC_SRAM_Init(void);
void FSMC_SRAM_WriteBuffer(uint16_t* pBuffer,u32 WriteAddr,u32 NumHalfwordToWrite);
void FSMC_SRAM_ReadBuffer(uint16_t* pBuffer,u32 ReadAddr,u32 NumHalfwordToRead);


void fsmc_sram_test_write(uint16_t data,u32 addr);
uint16_t fsmc_sram_test_read(u32 addr);
void Fill_Buffer(uint16_t *pBuffer, uint16_t BufferLenght, uint32_t Offset);

#endif

