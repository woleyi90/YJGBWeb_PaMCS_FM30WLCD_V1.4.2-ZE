#ifndef __LEDKEY_SM1668_H
#define __LEDKEY_SM1668_H			  	 

   
#include  <stm32f10x.h>

#define SM1668_SCLK_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define SM1668_SCLK_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_11)

#define SM1668_SDIN_Clr()	GPIO_ResetBits(GPIOC,GPIO_Pin_10)
#define SM1668_SDIN_Set()	GPIO_SetBits(GPIOC,GPIO_Pin_10)

#define SM1668_SDIN_READ()	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10);  

#define SM1668_STB_Clr()	GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define SM1668_STB_Set()	GPIO_SetBits(GPIOA,GPIO_Pin_12)

void SM1668_DisplayLED(u8 Register,u8 Data);
void SM1668_ReadKey(u8 *keybuf);
#endif  








