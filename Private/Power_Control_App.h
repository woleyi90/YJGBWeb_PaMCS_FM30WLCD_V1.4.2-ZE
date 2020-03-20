#ifndef __Power_Control_App_H
#define __Power_Control_App_H

#include "includes.h"

#define STARTUPDELAYCONTER 3

#define PROTECT_INPUT_HEIGHT 0
#define PROTECT_REF_HEIGHT   1
#define PROTECT_INPUT_LOW    2
#define PROTECT_I_HEIGHT     3

#define I_HEIGHT         0
#define I_PROTECT_HTIGHT 1

#define I_CHECK_DELAY    7
#define TEMP_CHECK_DELAY 3
#define TEMP_FAN_DELAY   10 

/*�����ػ�*/
#define PS_OVERLOAD	 1	//��������
#define PS_OVERTEMP  2	//�¶ȹ���	
#define PS_RFHEIGHT  3  //���书�ʸ�
#define PS_PAVCCERR  4	//���ŵ�Դ�쳣
/*����״̬λ*/
#define WS_PWCHG_ING      0   //���ڵ��ڹ��� 
#define WS_HALF_AUTO      1	  //���Զ�
#define WS_POWER_AUTO     2	  //�Զ�
//#define WS_UNCONFIRMED    3	  //Զ������δȷ��
#define WS_STARTUP_ING    4	  //������ʱ
#define WS_UNCONFIRMED    5	  //Զ������δȷ��
#define WS_TESTMODE       6	  //����ģʽ
//#define WS_OUTER_PROTECT_RF  6	  //�ⲿ��������
#define WS_NO_RF_OUT      7	  //���ͣ��


#define CHK1_FAN1_START     0
#define CHK1_FAN2_START     1
#define CHK1_GATE_STATUS	2
#define CHK1_HARD_RF_HEIGHT 3
#define CHK1_POWER_ERROR    4
#define CHK1_PATEMP_HEIGHT  5
#define CHK1_PWLOW_REAL     6
#define CHK1_PWLOW_DELAY    7

#define CHK2_I_PROTECTED    0
#define CHK2_TEMP_PROTECTED	1
#define CHK2_FR_PROTECTED	2
#define CHK2_I_POWERDOWN    3
#define CHK2_TEMP_POWERDOWN 4
#define CHK2_FR_POWERDOWN   5 
#define CHK2_HARD_IN_HEIGHT 6
#define CHK2_HARD_I_HEIGHT  7

void AP_Shutdown(void);
void AP_Startup(void);
u8 SelfTest(void);
u8 GateVolCheck(void);
void SetGateCtrl(u8 onoff);
void Main_Process(void);
void Temp_CheckProcess(void);
#endif

