#ifndef __LCD_MENU_H
#define __LCD_MENU_H


#include  <stm32f10x.h>

extern void (*current_operation_index)(); //ִ�е�ǰ��ʾ����
extern u8 func_index;
extern u8 key_flag;
//�˵��ṹ�嶨��
typedef struct Key_Menu
{			
	volatile u8 current;	// ��ǰ״̬	
	volatile u8 up;	    // �ϼ�
	volatile u8	down;	// �¼�	
	volatile u8 enter;	// ȷ�ϼ�
	volatile u8 quit;	// ȡ��/�˳���
	volatile void(*current_operation)(int s);// ��ǰ״ִ̬�еĺ���

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


