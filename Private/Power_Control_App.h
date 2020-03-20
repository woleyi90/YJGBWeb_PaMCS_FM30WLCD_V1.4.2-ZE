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

/*保护关机*/
#define PS_OVERLOAD	 1	//电流过载
#define PS_OVERTEMP  2	//温度过载	
#define PS_RFHEIGHT  3  //反射功率高
#define PS_PAVCCERR  4	//功放电源异常
/*工作状态位*/
#define WS_PWCHG_ING      0   //正在调节功率 
#define WS_HALF_AUTO      1	  //半自动
#define WS_POWER_AUTO     2	  //自动
//#define WS_UNCONFIRMED    3	  //远程主机未确认
#define WS_STARTUP_ING    4	  //开机延时
#define WS_UNCONFIRMED    5	  //远程主机未确认
#define WS_TESTMODE       6	  //测试模式
//#define WS_OUTER_PROTECT_RF  6	  //外部主播保护
#define WS_NO_RF_OUT      7	  //输出停播


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

