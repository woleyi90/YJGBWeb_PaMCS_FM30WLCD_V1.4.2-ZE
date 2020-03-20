#ifndef __IAP_H__
#define __IAP_H__
#include  <stm32f10x.h> 
	
typedef  void (*iapfun)(void);				//定义一个函数类型的参数.



void Iap_Load_App(u32 appxaddr);			//执行flash里面的app程序
	  
#endif







































