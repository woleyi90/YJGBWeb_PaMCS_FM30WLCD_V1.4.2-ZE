#include "includes.h"
#include "global.h"	

/*******************************************************************************
 * �������ƣ�SM1668_Delay()
 * �������ܣ���ʱ
 * ��ڲ�����  
 * ���ڲ�����
 *******************************************************************************/
void SM1668_Delay(void	)
{	
	u32 i=10;
	
    while(i--);
}
/*******************************************************************************
 * �������ƣ�SM1668_Senddata()
 * �������ܣ�SM1668��������
 * ��ڲ�����senddata �跢�͵�����  
 * ���ڲ�����
 *******************************************************************************/
void SM1668_Senddata(u8 senddata)
{
	u8 i;
	SM1668_SCLK_Set();
	SM1668_SDIN_Set();
	SM1668_STB_Clr();
	SM1668_Delay();	
	for (i=0;i<8;i++)
	{	
			
		SM1668_SCLK_Clr();         //SCL = 0
		SM1668_Delay();	
		if ((senddata & 0x01) != 0x01)			
			SM1668_SDIN_Clr();     //SDA = 0
		else			
			SM1668_SDIN_Set();     //SDA = 1
		senddata = senddata >> 1;
		SM1668_Delay();		
        SM1668_SCLK_Set();         //SCL = 1;
		SM1668_Delay();
	}
	SM1668_Delay();
}
/*******************************************************************************
 * �������ƣ�I2C_Senddata()
 * �������ܣ�I2C��������
 * ��ڲ�����senddata �跢�͵�����  
 * ���ڲ�����
 *******************************************************************************/
u8 SM1668_Receivedata(void)
{
	u8 i,temp,receivedata=0;

	SM1668_STB_Clr();
	SM1668_Delay();
	for (i=0;i<8;i++)
	{
		SM1668_SCLK_Clr(); 	 //SCL = 0
		receivedata = receivedata << 1;
		SM1668_Delay();		
		SM1668_SCLK_Set();      //SCL = 1
		SM1668_Delay();		
        temp = SM1668_SDIN_READ();    //temp = SDA		                  
		receivedata = receivedata | temp;
	}
	SM1668_Delay();
	return(receivedata);	
}
/*******************************************************************************
 * �������ƣ�SM1668_DisplayLED()
 * �������ܣ�SM1668��ʾLED
 * ��ڲ�����senddata �跢�͵�����  
 * ���ڲ�����  
 *******************************************************************************/
void SM1668_DisplayLED(u8 Register,u8 Data)
{
	SM1668_STB_Set();
	SM1668_Delay();
	SM1668_Senddata(0x03);	  //������ʾģʽ
	SM1668_STB_Set();
	SM1668_Delay();
	SM1668_Senddata(0x44);	//����д�Դ������������ù̶���ַ
	SM1668_STB_Set();
	SM1668_Delay();
   	SM1668_Senddata(Register);	//���õ�ַ
	SM1668_Senddata(Data); //��������
	SM1668_STB_Set();
	SM1668_Delay();
	SM1668_Senddata(0x8C); //������ʾ��������,����������11/16
	SM1668_STB_Set();
	
}
/*******************************************************************************
 * �������ƣ�SM1668_ReadKey()
 * �������ܣ�SM1668��ȡ����ֵ
 * ��ڲ�����  
 * ���ڲ�����  
 *******************************************************************************/
void SM1668_ReadKey(u8 *keybuf)
{
	u8 buf[5]={0};
	u8 i=0;
	GPIO_InitTypeDef GPIO_InitStructure;

	SM1668_STB_Set();
	SM1668_Delay();
	SM1668_Senddata(0x42);	  //���ö�ȡ����ָ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   //IO   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;            // ��©���     
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	SM1668_Delay();
	for(i=0;i<5;i++)
	{
		buf[i] = SM1668_Receivedata();
		//printf(" buf[%d]: 0x%x \n",i,buf[i]);		
		SM1668_Delay();
	}

	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_10  ; 
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	SM1668_STB_Set();
	memcpy(&keybuf[0],(u8 *)&buf[0], 5);  
}
