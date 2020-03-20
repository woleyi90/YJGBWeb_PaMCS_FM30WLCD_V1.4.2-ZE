#include "global.h"
#include "includes.h"
#include "Digital_ATT.h"

/*******************************************************************************
 * 函数名称：ATT_Delay()
 * 函数功能：延时
 * 入口参数：  
 * 出口参数：
 *******************************************************************************/
void ATT_Delay(void	)
{	
	u32 i=5;
	
    while(i--);
}
/*******************************************************************************
 * 函数名称：ATT_Senddata()
 * 函数功能：ATT发送数据
 * 入口参数：senddata 需发送的数据 最大值为62 
 * 参数说明：衰减dBm = (63 - senddata)*0.5    
 *******************************************************************************/
void ATT_Senddata(u8 senddata)
{
	u8 i;
	
	if(senddata >=62)
	{
		senddata = 62;
	}
	else
	{
		senddata = senddata & 0x3F;	
	}	
	
	g_DI.Pa[0].Data.RES_Rear = senddata; 
	
	LEATT_CLR();
	ATT_Delay();
	SCLK_CLR();         //SCL = 0;
	ATT_Delay();
	for (i=0;i<8;i++)
	{	
		if ((senddata & 0x80) != 0x80)			
			SDIN_CLR();     //SDA = 0;
		else			
			SDIN_SET();     //SDA = 1;
		
		senddata = senddata << 1;
		ATT_Delay();		
        SCLK_SET();         //SCL = 1;
		ATT_Delay();		
		SCLK_CLR();         //SCL = 0;
		ATT_Delay();		
	}
	ATT_Delay();
	LEATT_SET();
	ATT_Delay();
	LEATT_CLR();
	ATT_Delay();
}













