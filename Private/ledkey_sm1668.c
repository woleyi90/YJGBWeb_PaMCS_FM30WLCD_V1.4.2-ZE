#include "includes.h"
#include "global.h"	

/*******************************************************************************
 * 函数名称：SM1668_Delay()
 * 函数功能：延时
 * 入口参数：  
 * 出口参数：
 *******************************************************************************/
void SM1668_Delay(void	)
{	
	u32 i=10;
	
    while(i--);
}
/*******************************************************************************
 * 函数名称：SM1668_Senddata()
 * 函数功能：SM1668发送数据
 * 入口参数：senddata 需发送的数据  
 * 出口参数：
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
 * 函数名称：I2C_Senddata()
 * 函数功能：I2C发送数据
 * 入口参数：senddata 需发送的数据  
 * 出口参数：
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
 * 函数名称：SM1668_DisplayLED()
 * 函数功能：SM1668显示LED
 * 入口参数：senddata 需发送的数据  
 * 出口参数：  
 *******************************************************************************/
void SM1668_DisplayLED(u8 Register,u8 Data)
{
	SM1668_STB_Set();
	SM1668_Delay();
	SM1668_Senddata(0x03);	  //设置显示模式
	SM1668_STB_Set();
	SM1668_Delay();
	SM1668_Senddata(0x44);	//设置写显存的数据命令，采用固定地址
	SM1668_STB_Set();
	SM1668_Delay();
   	SM1668_Senddata(Register);	//设置地址
	SM1668_Senddata(Data); //传送数据
	SM1668_STB_Set();
	SM1668_Delay();
	SM1668_Senddata(0x8C); //传送显示控制命令,设置脉冲宽度11/16
	SM1668_STB_Set();
	
}
/*******************************************************************************
 * 函数名称：SM1668_ReadKey()
 * 函数功能：SM1668读取按键值
 * 入口参数：  
 * 出口参数：  
 *******************************************************************************/
void SM1668_ReadKey(u8 *keybuf)
{
	u8 buf[5]={0};
	u8 i=0;
	GPIO_InitTypeDef GPIO_InitStructure;

	SM1668_STB_Set();
	SM1668_Delay();
	SM1668_Senddata(0x42);	  //设置读取按键指令

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   //IO   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;            // 开漏输出     
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
