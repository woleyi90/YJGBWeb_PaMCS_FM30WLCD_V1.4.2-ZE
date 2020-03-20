#ifndef __LCD_MENU_H
#define __LCD_MENU_H


#include  <stm32f10x.h>

extern void (*current_operation_index)(); //执行当前显示函数
extern u8 func_index;
extern u8 key_flag;
//菜单结构体定义
typedef struct Key_Menu
{			
	volatile u8 current;	// 当前状态	
	volatile u8 up;	    // 上键
	volatile u8	down;	// 下键	
	volatile u8 enter;	// 确认键
	volatile u8 quit;	// 取消/退出键
	volatile void(*current_operation)(int s);// 当前状态执行的函数

}Key_MenuStruct;

void MachineStateFun1(void);   
void MachineStateFun2(void);   
void SetSystemParameter1a(void);
void SetSystemParameter1b(void);
void SetSystemParameter1c(void); 
void SetSystemParameter2a(void); 
void SetSystemParameter2b(void);
void SetSystemParameter2c(void);
void SetSystemParameter3(void); 
void KEY_MENU_DEAL(void);

  

#endif


