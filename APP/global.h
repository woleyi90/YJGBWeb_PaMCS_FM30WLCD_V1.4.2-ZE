/*******************************************************************************
 *
 * Filename  : global.h
 *
 * Function  : Reference head file for "GLOBAL" library.
 *
 * Created   : 2016-10-09 15:23:00
 *           : Mr. WQH
 *
 * Check In  : $Date: 2016-10-09 15:23:00 +0800  $
 *           : $Author: wqh $
 * Revision  : $Revision: * $
 *
 * Copyright(c) by ShangHai DongZhou Lawton Telecom Technologies Co.,Ltd.  
 *              		All Rights Reserved         
 *
 *******************************************************************************/
 
#ifndef __GLOBAL_H 
#define __GLOBAL_H


#include "includes.h"

#ifdef GLOBALS 
	#define EXT
#else
	#define EXT extern 
#endif


#define DEBUG_EN            0	  // 调试开关
#define IWDG_EN             1	  // 独立看门狗使能开关
#define WWDG_EN             0	  // 窗口看门狗使能开关


#define cICON				0x23
#define cAutoMsgICON		0x21			
#define	cFunID				0x08		
#define	cID					0x00

#define cNet_Port			8001				// 默认端口
#define clocal_port			7000
#define BROCASTPORT 		8888
#define cGAIN               6500
#define IMax                10
#define	IMin                -40
#define OMax                59
#define RVMAX               44
#define ALLOMax             49
#define ALLRVMAX            37

#define ALLOUt              4750

#define cCurMax             3000
#define cTemMax             60  
#define cTempCutOff         70   //关功放门限
#define cVccMax             5200
#define cVccMin             4000

#define PA_ALC_VOL_DEF      330
#define PA_ALC_DEF          6000   //60dbm
#define TD_ALC_VOL_DEF      200
#define ATT_VOL_DEF         500
#define SWR_VOL_DEF         167	 

#define SOFT_VERSION	142 				// 软件版本号


#define Hard_VERSION	50				// 硬件版本号

#define AGREEMENT_VERSION		0x01	// 协议版本号 V1.07:0x01
#define cDevTypeCode	0x0002	//0x0002:调频发射机
#define cMfrCode		0x000A	//0x000A:东洲罗顿	
#define cDeviceID		0x02	//0x02:本设备地址

											
#define cWaitNum            3         	    //故障重开次数	
#define cWaitSec			180				// 故障重开间隔时间

#define SETTIMEOUT	    	70				// RS485设置超时时间2s
#define PT_TIMEOUT	        300			    // 透传数据超时时间：3s
#define TIMEOUT			    70			    // RS485查询超时时间1s

#define	MSG_SIZE	     	4		        // 消息队列长度	

#define ATT3DB_VALUE        150             //3DB对应的电压值

#define cDPDType            0x80           //初始为DTMB
											   

#define	cto_limit_max	 	55     
#define	cto_limit_middle1 	53
#define	ctl_limit_middle1 	43
#define	cto_limit_middle0 	41
#define	ctl_limit_middle0 	32
#define	ctl_limit_min 	 	30

#define CALIBRATIONCOUNT 3

#define CHECKPOWERDOWNTIMER   0
#define OVERLOADRESTARTTIMER  1
#define POWERAUTOTIMER  2

#define ETH_rec_f       GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)  //ENC28J60接收帧完成中断脚
   
#define Encoder_KEY_A       GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)  //旋转编码器A相 

#define Encoder_KEY_B       GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)  //旋转编码器B相 

#define Encoder_KEY_OK      GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)  //旋转编码器OK 

#define LED_RUN_ON     GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define LED_RUN_OFF    GPIO_ResetBits(GPIOB,GPIO_Pin_7)

#define LED_WORK_ON     GPIO_SetBits(GPIOE,GPIO_Pin_5)
#define LED_WORK_OFF    GPIO_ResetBits(GPIOE,GPIO_Pin_5)	   

#define LED_PWRIN_ALM    GPIO_SetBits(GPIOE,GPIO_Pin_4)
#define LED_PWRIN_NORMAL   GPIO_ResetBits(GPIOE,GPIO_Pin_4)

#define LED_PWRCUR_ALM    GPIO_SetBits(GPIOE,GPIO_Pin_2)
#define LED_PWRCUR_NORMAL   GPIO_ResetBits(GPIOE,GPIO_Pin_2)

#define LED_PWRRV_ALM    GPIO_SetBits(GPIOE,GPIO_Pin_3)
#define LED_PWRRV_NORMAL   GPIO_ResetBits(GPIOE,GPIO_Pin_3)

#define SW_FAN1_OFF     GPIO_ResetBits(GPIOD,GPIO_Pin_7)
#define SW_FAN1_ON      GPIO_SetBits(GPIOD,GPIO_Pin_7)	  //风扇控制，高电平开

#define SW_FAN2_OFF     GPIO_ResetBits(GPIOD,GPIO_Pin_6)
#define SW_FAN2_ON      GPIO_SetBits(GPIOD,GPIO_Pin_6)	  //风扇控制，高电平开

/*风扇控制*/
#define SW_FAN_PORT   GPIOD
#define SW_FAN1_PIN   GPIO_Pin_7       //风扇1
#define SW_FAN2_PIN   GPIO_Pin_6	   //风扇2

//#define SW_WWDOG_OFF    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG , DISABLE)
//#define SW_WWDOG_ON     RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG , ENABLE)

#define Modem_Rbuf		UART1_RBuf		 
#define DPD_Rbuf        UART3_RBuf

	


#define DPD_SendBuf(Data, Len)		UART3Write(Data, Len)


extern MYTIMER User_Timer[3]; 

EXT volatile u16 g_NetTimer05S;
EXT volatile u16 g_NetTimer10S;	 


EXT u8		     g_DevPro;			// 模块控制操作标志,最高位表示位状态,低七位是MODBUS地址
EXT u8           g_UIPro;
EXT u8		     g_DevSet;

EXT Sys_State	 Sys_Flag;			// 标志
EXT  __align(4)  System  g_DI ;		// 系统全局运行变量	     
EXT RS485_Dat	 g_RTU;				// 模块接收到的数据结构体
EXT Dev_Data	 g_Dev;				// 模块发送数据结构体

EXT DeviceDatas	 devdata;
EXT SysNet_State SysNet_Flag;			// 标志    
EXT volatile    u16    g_ADC[ADC_CH_NUM];    // ADC计算结果 
EXT volatile    u16    AD_Value[ADCTIMES][ADC_CH_NUM]; //用来存放ADC转换结果，也是DMA的目标地址

EXT UART_RBuf 	UART1_RBuf ;		// UART1接收结构体
EXT UART_RBuf 	UART2_RBuf ;    	// UART2接收结构体
EXT UART_RBuf 	UART3_RBuf ;		// UART3接收结构体

EXT UART_RBuf 	UART5_RBuf ;		// UART5接收结构体

EXT u8 		g_UART_TBuf[500] ;	    // 透传数据 UART发送缓冲区
EXT u8 		g_DMA_TBuf[500] ;	    // 透传数据 UART发送缓冲区	

EXT  bool   ffirst;
 
EXT u8  g_ActiveAlm_bak;

EXT  u8 g_SWRAlm_ReSendNum;
EXT  u8 g_SWRAlm_WaitSec; 
EXT  u8 g_SWRAlmRst_WaitSec; //驻波告警故障重开机，正常维持时间
EXT  u8 netLink_Mcnt; //网络连接计数器，超时自动断开
		
EXT  u8     g_TMTLink[2];
EXT  u8     g_TMTState[2];
EXT  bool   fRS485; 	  //通信防干扰标志位
EXT  bool   fPaRTU; 	  //通信防干扰标志位
EXT  bool   fEitRTU; 	  //通信防干扰标志位
EXT  u8     flag_TLink[3];
EXT	 u8		GX_TXData[200];
EXT	 u8		GX_TXDatalen;
EXT	 u8		GX_RXData[200];
EXT	 u8		GX_RXDatalen;

EXT	 u32	CpuID[3];
EXT	 u8		cpaitt[32];	//功放说明
EXT	 u8		cExcitDirection[32];	//激励器说明
EXT	 u32	CurTimes;	//激励器说明
EXT	 u8		Encoder_CW;	
EXT	 u8		Encoder_CCW;
EXT	 u8		Led_Display;
extern float PowerCal_Out[2][CALIBRATIONCOUNT];
extern float PowerCal_Ref[2][CALIBRATIONCOUNT];
extern float PowerCal_In[2][CALIBRATIONCOUNT];
EXT	 float		PaOutfout;
EXT	 float		PaInfout;
EXT	 float		PaRvfout;
/*
 *******************************************************************************
 *
 * OS Semeget defines
 *
 *******************************************************************************
 */
EXT         OS_EVENT  *RS485SEM;	 // RS485互斥信号量
EXT         OS_EVENT  *PARS232SEM;	 // 功放互斥信号量
EXT         OS_EVENT  *JLQRS232SEM;	 // 激励器互斥信号量
EXT         OS_EVENT  *UISETMSG;		 // 设置消息队列指针	
EXT         OS_EVENT  *TCP_NEWSEM;	 // LWIP TCP

EXT	u8 		QMsg[MSG_SIZE];		     // 队列消息

EXT void    *UISetArrayOfMsg[MSG_SIZE]; // 指向消息数组的指针
EXT	u8 		UIQMsg[MSG_SIZE];		     // 队列消息
		


#endif





