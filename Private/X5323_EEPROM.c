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
* ��    �ƣ�void SPI2_Init(void)
* ��    �ܣ�SPI2��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void SPI1_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* ʹ��SPI1ʱ�� */ 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);  //��SPI1�ֲ���ӳ��  
	/* ����SPI1��SCK,MISO MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //���ù���
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* ����SPI1��Ƭѡ */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_6); 

	/* SPI2���� */ 
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
	
	/* ʹ��SPI2  */
	SPI_Cmd(SPI1, ENABLE); 
}
/*******************************************************************************
 * ��    �ƣ�unsigned char	SPI2ReadWrite(unsigned char writedat)
 * ��    �ܣ�SPI2��дһ�ֽڳ���
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 * ���÷������� 
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
 * ��    �ƣ�void X5323_Write(unsigned char writedat)
 * ��    �ܣ�SPI1дһ�ֽڳ���
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 * ���÷������� 
 *******************************************************************************/
void X5323_Write(unsigned char writedat)
{
	X5323_CSL();
	SPI1_ReadWrite(writedat);
	X5323_CSH();  
}
/*******************************************************************************
 * ��    �ƣ�unsigned char	WriteByteToX5323(unsigned int addr,unsigned char dat)
 * ��    �ܣ���x5323дһ�ֽڳ���
 * ��ڲ�����unsigned int addr,unsigned char dat
 * ���ڲ�������
 * ˵    ����
 * ���÷������� 
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
 * ��    �ƣ�void WritebufToX5323(unsigned int addr,unsigned int len,u8 *buf)
 * ��    �ܣ���x5323д����
 * ��ڲ�����unsigned int addr,unsigned int len,u8 *buf
 * ���ڲ�������
 * ˵    ����
 * ���÷������� 
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
 * ��    �ƣ�unsigned char	ReadByteFromX5323(unsigned int addr)	
 * ��    �ܣ���x5323��һ�ֽڳ���
 * ��ڲ�����unsigned int addr
 * ���ڲ�����shuju
 * ˵    ����
 * ���÷������� 
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
 * ��    �ƣ�unsigned char	ReadByteFromX5323(unsigned int addr)	
 * ��    �ܣ���x5323��һ�ֽڳ���
 * ��ڲ�����unsigned int addr
 * ���ڲ�����shuju
 * ˵    ����
 * ���÷������� 
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
 * ��    �ƣ�int Wait_For_RDY(void)	
 * ��    �ܣ���ȡæ״̬
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 * ���÷������� 
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
		if( (iTest & 0x01) == 0x00 ) //bit0: 1Ϊ����д  0Ϊ����״̬
		{
			Delaynus(15);
//			printf("  iTest: %d\n",iTest); 
//			printf("  nTimeout: %d\n",(500-nTimeout));	
			return 0; //����оƬ���ڿ���״̬
		}
	}

	return 1; //�������ڽ���д����
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







