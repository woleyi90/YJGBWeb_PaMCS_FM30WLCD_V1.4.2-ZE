#include "includes.h"



unsigned char x5323_Read_SR(void);

void Delaynus( u16 time )
{
	u16 i;
	while(time--)
	{
		//i = 20;
		//while(i--);
	}
}										 

/****************************************************************************
* 名    称：void SPI2_Init(void)
* 功    能：SPI2初始化
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void SPI1_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 使能SPI1时钟 */ 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);  //将SPI1局部重映射  
	/* 配置SPI1的SCK,MISO MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //复用功能
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* 配置SPI1的片选 */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_6); 

	/* SPI2配置 */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);
	
	/* 使能SPI2  */
	SPI_Cmd(SPI1, ENABLE); 
}
/*******************************************************************************
 * 名    称：unsigned char	SPI2ReadWrite(unsigned char writedat)
 * 功    能：SPI2读写一字节程序
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
unsigned char SPI1_ReadWrite(unsigned char writedat)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(SPI1, writedat);
	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}
/*******************************************************************************
 * 名    称：void X5323_Write(unsigned char writedat)
 * 功    能：SPI1写一字节程序
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void X5323_Write(unsigned char writedat)
{
	X5323_CSL();
	SPI1_ReadWrite(writedat);
	X5323_CSH();  
}
/*******************************************************************************
 * 名    称：unsigned char	WriteByteToX5323(unsigned int addr,unsigned char dat)
 * 功    能：向x5323写一字节程序
 * 入口参数：unsigned int addr,unsigned char dat
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void WriteByteToX5323(unsigned int addr,unsigned char dat)
{
	unsigned char addrH;
	unsigned char addrL;
	
	addrL = (addr) & 0xff;
	addrH = (addr) >> 8;

	X5323_Write(X5323_WREN);	
	Delaynus(20);	
	
	X5323_CSL();
	SPI1_ReadWrite(X5323_WRITE);
	SPI1_ReadWrite(addrH);
	SPI1_ReadWrite(addrL);	
	SPI1_ReadWrite(dat);
	X5323_CSH();  	

	Wait_For_RDY();

		 
}
/*******************************************************************************
 * 名    称：void WritebufToX5323(unsigned int addr,unsigned int len,u8 *buf)
 * 功    能：向x5323写数组
 * 入口参数：unsigned int addr,unsigned int len,u8 *buf
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
u8 WritebufToX5323(unsigned int addr,unsigned int len,u8 *buf)
{
	unsigned int i=0;
	unsigned char ucCnt = 0;
	unsigned char addrH;
	unsigned char addrL;
	if(addr+len >= 0x1000)  return 0;
	for(i=0;i<len;i++)
	{

		addrL = (unsigned char)(addr & 0xff);
		addrH = (unsigned char)(addr >> 8);
	
		X5323_Write(X5323_WREN);	
		Delaynus(15);	
		
		X5323_CSL();
		SPI1_ReadWrite(X5323_WRITE);
		SPI1_ReadWrite(addrH);
		SPI1_ReadWrite(addrL);	
		SPI1_ReadWrite(*buf++);
		X5323_CSH();  	
		Delaynus(10);  
		ucCnt = 0;
		while(x5323_Read_SR() & 0x01)
		{
			Delaynus(10);
			
			if(ucCnt++ > 250)  
			{
				
//				printf("write error i:%d\r\n",i);
				return 0;
			}
			
		}
		addr++;
	}
	X5323_CSH();  	
	Delaynus(10);
	return 1;	 
}
/*******************************************************************************
 * 名    称：unsigned char	ReadByteFromX5323(unsigned int addr)	
 * 功    能：从x5323读一字节程序
 * 入口参数：unsigned int addr
 * 出口参数：shuju
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
unsigned char ReadByteFromX5323(unsigned int addr)	
{
	unsigned char addrH;
	unsigned char addrL;
	unsigned char shuju;
	
	addrL = (addr) & 0xff;
	addrH = (addr) >> 8;
		
	X5323_CSL();	
	SPI1_ReadWrite(X5323_READ);		
    SPI1_ReadWrite(addrH);    	
    SPI1_ReadWrite(addrL);
	shuju = SPI1_ReadWrite(0x00);
	X5323_CSH(); 
	
	Delaynus(30);
	
	return shuju;
	
}
/*******************************************************************************
 * 名    称：unsigned char	ReadByteFromX5323(unsigned int addr)	
 * 功    能：从x5323读一字节程序
 * 入口参数：unsigned int addr
 * 出口参数：shuju
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void ReadbufFromX5323(unsigned int addr,unsigned int len,u8 *pbuf)	
{
	unsigned char ucCnt = 0;
	unsigned int  uiLoop;
	unsigned char addrH,addrL;
	if(addr+len >= 0x1000)  return;
	while(x5323_Read_SR() & 0x01)
	{
		Delaynus(17);
		if(ucCnt++ > 250)  return;
	}
	for(uiLoop = 0; uiLoop < len; uiLoop++)
	{

		addrH = (unsigned char)(addr >> 8);
		addrL = (unsigned char)(addr & 0xff);
		X5323_CSL();
		SPI1_ReadWrite(X5323_READ);
		SPI1_ReadWrite(addrH);
		SPI1_ReadWrite(addrL);
		*pbuf++ = SPI1_ReadWrite(0xA5);
		X5323_CSH(); 
		addr++;
	}
	
}
/*******************************************************************************
 * 名    称：int Wait_For_RDY(void)	
 * 功    能：读取忙状态
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
char Wait_For_RDY(void)
{
	int nTimeout = 500;
	unsigned char iTest;

	while(nTimeout-- > 1)
	{
	
		X5323_CSL();	
		SPI1_ReadWrite(X5323_RDSR);
		Delaynus(15);
		iTest = SPI1_ReadWrite(0xff);		
		X5323_CSH();
		if( (iTest & 0x01) == 0x00 ) //bit0: 1为正在写  0为空闲状态
		{
			Delaynus(15);
//			printf("  iTest: %d\n",iTest); 
//			printf("  nTimeout: %d\n",(500-nTimeout));	
			return 0; //返回芯片处于空闲状态
		}
	}

	return 1; //返回正在进行写操作
} 
unsigned char x5323_Read_SR(void)
{
	unsigned char Byte;

	X5323_CSL();
	Delaynus(10);	
	SPI1_ReadWrite(X5323_RDSR);
	Delaynus(10);
	Byte = 0;	
	Byte = SPI1_ReadWrite(0xff);
	Delaynus(10);
	X5323_CSH();
	return Byte;
} 







