#ifndef __LM16032_DRIVE_H
#define __LM16032_DRIVE_H			  	 

   
#include  <stm32f10x.h>

//mode:0,反白显示;1,正常显示
#define  NormalDisplay	1
#define  SelectDisplay	0
		    						  
//-----------------LCD端口定义----------------  					   
//#define LCD_CS_Clr()  GPIO_ResetBits(GPIOC,GPIO_Pin_9)
//#define LCD_CS_Set()  GPIO_SetBits(GPIOC,GPIO_Pin_9)
//
//
//#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_11)
//#define LCD_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_11)
//
//#define LCD_SDIN_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_10)
//#define LCD_SDIN_Set() GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define LCD_CS_Clr() 
#define LCD_CS_Set()  		  
#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_11)
#define LCD_SCLK_Set() GPIO_SetBits(GPIOC,GPIO_Pin_11)

#define LCD_SDIN_Clr() GPIO_ResetBits(GPIOC,GPIO_Pin_9)
#define LCD_SDIN_Set() GPIO_SetBits(GPIOC,GPIO_Pin_9)	     

//OLED控制用函数
void LCD_Display_SW(unsigned char lcdbl);
void LCD_Write_Data(unsigned char RegisterValue);
void LCD_Write_Command(unsigned char Command);
void LCD_Initial(void);
void DisplayChar(unsigned char CharX,unsigned char CharY,unsigned char *ASC_GB);
void PrintGB(unsigned char x, unsigned char y, unsigned char *pstr);


#endif  












	 