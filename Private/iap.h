#ifndef __IAP_H__
#define __IAP_H__
#include  <stm32f10x.h> 
	
typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.



void Iap_Load_App(u32 appxaddr);			//ִ��flash�����app����
	  
#endif







































