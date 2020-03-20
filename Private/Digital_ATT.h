#ifndef __Digital_ATT_H
#define __Digital_ATT_H

#include "includes.h"


#define LEATT_SET()         GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define LEATT_CLR()         GPIO_ResetBits(GPIOA,GPIO_Pin_6)

#define SCLK_SET()         GPIO_SetBits(GPIOC,GPIO_Pin_4)
#define SCLK_CLR()         GPIO_ResetBits(GPIOC,GPIO_Pin_4)

#define SDIN_SET()         GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define SDIN_CLR()         GPIO_ResetBits(GPIOA,GPIO_Pin_7)

void ATT_Delay(void);
void ATT_Senddata(u8 senddata);
#endif

