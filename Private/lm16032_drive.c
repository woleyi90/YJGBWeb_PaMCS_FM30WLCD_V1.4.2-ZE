#include "includes.h"
#include "global.h" 


void Delayus( int time )
{
	while(time--);
}						 


void DelayLCD( int time )
{
	while(time--)
	{
	 	Delayus(10);
	}
}

void LCD_Display_SW(unsigned char lcdbl)
{
	if(lcdbl == 1)
	{
		GPIO_ResetBits(GPIOF,GPIO_Pin_10);	
	}
	else
	{
		GPIO_SetBits(GPIOF,GPIO_Pin_10);		
	}
}
	
/***********************LCD_Write_Command*******************/
void LCD_Write_Command(unsigned char Command)
{
	unsigned char i = 0 , j = 0;
	unsigned char Cmd = 0;
	unsigned char buffur[3];
	unsigned char DataByte = 0;

	LCD_SCLK_Clr();
	LCD_CS_Clr();
	DelayLCD(5);
	LCD_CS_Set();
	DelayLCD(5);
	buffur[0] = 0xF8;
	buffur[1] = Command & 0xF0;
	buffur[2] = (Command & 0x0F)<<4;

	for(j=0;j<3;j++)
	{
		DataByte = buffur[j];
		for(i=0; i<8; i++)
		{
			LCD_SCLK_Clr();
			DelayLCD(50);
			if(DataByte&0x80)
			   LCD_SDIN_Set();
			else 
			   LCD_SDIN_Clr();
			DelayLCD(50);
			LCD_SCLK_Set();
			DataByte<<=1; 
			DelayLCD(50);  	
		}
	}
	
	LCD_CS_Clr();
	DelayLCD(5);
}
/***********************LCD_Write_Data*******************/
void LCD_Write_Data(unsigned char RegisterValue)
{
	unsigned char i = 0 , j = 0;
	unsigned char Cmd = 0;
	unsigned char buffur[3];
	unsigned char DataByte = 0;

	LCD_SCLK_Clr();
	LCD_CS_Clr();
	DelayLCD(5);
	LCD_CS_Set();
	DelayLCD(5);
	buffur[0] = 0xFA;
	buffur[1] = RegisterValue & 0xF0;
	buffur[2] = (RegisterValue & 0x0F)<<4;

	for(j=0;j<3;j++)
	{
		DataByte = buffur[j];
		for(i=0; i<8; i++)
		{
			LCD_SCLK_Clr();
			DelayLCD(50);
			if(DataByte&0x80)
			   LCD_SDIN_Set();
			else 
			   LCD_SDIN_Clr();
			DelayLCD(50);
			LCD_SCLK_Set();
			DataByte<<=1; 
			DelayLCD(50);  	
		}
	}
	
	LCD_CS_Clr();
	DelayLCD(5);		
}

/**********************Initial_Code*******************/
void LCD_Initial(void)
{

	LCD_Write_Command(0x30);//8 bit 传输,基础指令
	DelayLCD(100);
	LCD_Write_Command(0x06);
	DelayLCD(100);
	LCD_Write_Command(0x01);//清屏
	DelayLCD(100);
	LCD_Write_Command(0x0c);//Display on，光标显示关，光标反白显示关
	DelayLCD(100);
	//LCD_Write_Command(0x02);
	//DelayLCD(100);

}
//--------------------------------------//    
//设定字符显示位置函数    
//--------------------------------------    
void SetCharCursor(unsigned char CharX,unsigned char CharY)    
{    
	unsigned char CharSite;    
	CharX=CharX & 0x0f; //取低四位   
	CharY=CharY & 0x0f;    
	CharY=CharY<<4; //低四位变为高四位，低四位清零   
	CharY=CharY & 0x30; //只取第五位，因为该显示屏只能显示两行文字 当多于两行时无显示，因而为保证显示屏始终有显示~   
	CharY=CharY | 0x80; //最高位置1   
	CharSite=CharY | CharX;    
	LCD_Write_Command(CharSite);   
} 
//--------------------------------------//   
//显示字符子函数    
//--------------------------------------//    
void DisplayChar(unsigned char CharX,unsigned char CharY,unsigned char *ASC_GB)    
{   
	SetCharCursor(CharX,CharY);    
	while(*ASC_GB>0) //按顺序一个一个字符的写   
	{
		LCD_Write_Data(*ASC_GB);
		ASC_GB++;   
	}   
} 
void PrintGB(unsigned char x, unsigned char y, unsigned char *pstr)
{
  unsigned char addr;
  y=y<<4;
  addr=y|0x80+x;
  LCD_Write_Command(addr);
  while(*pstr>0)
  {
  	LCD_Write_Data(*pstr++);
  }
}
 void SetCursor(unsigned char x, unsigned char y)
{
  unsigned char addr;
  y=y<<4;
  addr=y|0x80+x;
  LCD_Write_Command(addr);
} /* 
void LcdMachinePAState(void)
{
	u8 display[50];
	int intdata = 0, pointdata = 0;	
	int intdataTwo = 0, pointdataTwo = 0;
	int intdataSan = 0, pointdataSan = 0;	

	//LCD_Write_Command(0x1C);
	g_DI.Pa[0].Data.Power_FR = 30.567;
	g_DI.Pa[0].Data.PA_TPT = 29.35;
	g_DI.Pa[0].Data.Power_RV = 0.568;

	intdata = (int)g_DI.Pa[0].Data.Power_FR;
	pointdata = (int)((g_DI.Pa[0].Data.Power_FR-intdata)*10);
	
	intdataTwo = (int)g_DI.Pa[0].Data.Power_RV;
	pointdataTwo = (int)((g_DI.Pa[0].Data.Power_RV-intdataTwo)*100);

	intdataSan = (int)g_DI.Pa[0].Data.PA_TPT;
	pointdataSan = (int)((g_DI.Pa[0].Data.PA_TPT-intdataSan)*10);
	
	sprintf(display, "功率  反射功率  温度");
	PrintGB(0,0,display);


	sprintf(display, "%2d.%1dW", intdata, pointdata);
	PrintGB(0,1,display);

	if(pointdataTwo < 10)
	{
		sprintf(display, "%2d.%1dW", intdataTwo, pointdataTwo);
	}
	else
	{
		sprintf(display, "%2d.%2dW", intdataTwo, pointdataTwo);
	}	
	PrintGB(3,1,display);
	
	sprintf(display, "%2d.%1d℃", intdataSan, pointdataSan);				
	PrintGB(7,1,display);

} 
void LcdMachineJLQState(void)
{
	u8 display[50];
	LCD_Write_Command(0x01);//clear
	g_DI.Excitdata.FMFRQ = 10015;
	g_DI.Excitdata.Modulation = 70;
	g_DI.Excitdata.Volu = 10; 
	
	sprintf(display, "  频率  调制度  音量");
	PrintGB(0,0,display);


	sprintf(display, "%d.%dMHz", g_DI.Excitdata.FMFRQ/100, g_DI.Excitdata.FMFRQ%100);
	PrintGB(0,1,display);


	sprintf(display, "%d%%", g_DI.Excitdata.Modulation);		
	PrintGB(5,1,display);
	
	sprintf(display, "%d", g_DI.Excitdata.Volu);				
	PrintGB(8,1,display);

}  
void SetSystemParamOne(void)
{
	
	u8 display[50];
	
	sprintf(display, "1.发射机开关");
	PrintGB(1,0,display);
	
	sprintf(display, "*");
	PrintGB(0,0,display);


	sprintf(display, "2.输出功率设置");
	PrintGB(1,1,display);

	sprintf(display, "*");
	PrintGB(0,1,display);
	
//	sprintf(display, "[ ]");
//	PrintGB(8,1,display);

	LCD_Write_Command(0x0e);
	SetCursor(7,1);

}
*/



