/*******************************************************************************
 *
 * Filename  : App.c
 *
 * Function  : Implememt APP function form for this workbench.
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

#include "includes.h"


#define GLOBALS	
#include "global.h"	

#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "httpd.h"
#include "udp_demo.h"

/*
 *******************************************************************************
 *
 * private definitions
 *
 *******************************************************************************
 */ 



/*
 *******************************************************************************
 *
 * private object
 *
 *******************************************************************************
 */	
static  OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE]; 
static  OS_STK Task_ETHStk[Task_ETH_STK_SIZE];
static  OS_STK Task_PCStk[Task_PC_STK_SIZE]; 
static  OS_STK Task_RTUStk[Task_RTU_STK_SIZE]; 
static  OS_STK Task_PAONStk[Task_PAON_STK_SIZE];
static  OS_STK Task_OLEDStk[Task_OLED_STK_SIZE];
u8  StatusFault=0;
 
 	
/* 
 *******************************************************************************
 *
 * private routines prototype
 * 
 *******************************************************************************
 */ 
static void App_TaskStart(void* p_arg);
static void DoMSEvent( void );
static void DoSecEvent( void );	
static void DoMinEvent( void ) ;
static void DoHourEvent( void ); 
static void DoDomEvent( void );
void Inpow_dect(void);
void PowerGet(void);			
void PaCurGet(void);
void OtherGet(void);
void SlowOn(void);

	

/*
 *******************************************************************************
 *
 * MAIN routines
 *
 *******************************************************************************
 */
void Sys_delay(int ms)
{
	int i = 0,j= 0;
	for(i=0;i<ms;i++)
		for(j=0;j<20000;j++);
}
/****************************************************************************
* 名    称：int main(void)
* 功    能：主函数入口
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
int main(void)
{ 

	SystemInit(); 	
	//SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	Sys_delay(500);  //系统延时调整,延时

	CPU_IntDis();  //禁止所有中断 
   	OSInit();  
   	BSP_Init();   

	
  	OSTaskCreate((void (*) (void *)) App_TaskStart,               		   
                           (void *) 0,											
               		       (OS_STK *) &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                           (INT8U) APP_TASK_START_PRIO);							
             
   OSTimeSet(0);  //ucosII的节拍计数器清0,节拍计数器是0-4294967295  
   OSStart();     //启动ucosII内核  

   return (0);

}
 



/* 
 *******************************************************************************
 *
 * private routines
 * 
 *******************************************************************************
 */
/*******************************************************************************
 * 名    称：static  void App_TaskStart(void* p_arg)
 * 功    能：开始任务建立
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 *******************************************************************************/
static  void App_TaskStart(void* p_arg)
{ 
  	(void) p_arg;


   	//TCP_NEWSEM=OSSemCreate(1);		//建立网络的信号量
	//UISETMSG   = OSQCreate(&UISetArrayOfMsg[0], MSG_SIZE); 
	RS485SEM = OSSemCreate(1); 			// RTU485信号量
	PARS232SEM = OSSemCreate(1); 			// 功放RTU互斥信号量
	JLQRS232SEM = OSSemCreate(1); 			// 激励器RTU互斥信号量
   

   	/* 初始化ucosII时钟节拍	*/
   	OS_CPU_SysTickInit();  
      
   	/* 建立网络处理任务 */ 	   
   	OSTaskCreateExt(thread_udp_broadcast_server,
   					(void *)0,
					(OS_STK *)&Task_ETHStk[Task_ETH_STK_SIZE-1],
					Task_ETH_PRIO,
					Task_ETH_PRIO,
					(OS_STK *)&Task_ETHStk[0],
                    Task_ETH_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	/* 建立PC处理任务 */    
   	OSTaskCreateExt(TaskPC,
   					(void *)0,
					(OS_STK *)&Task_PCStk[Task_PC_STK_SIZE-1],
					Task_PC_PRIO,
					Task_PC_PRIO,
					(OS_STK *)&Task_PCStk[0],
                    Task_PC_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	/* 建立RTU处理任务 */    
   	OSTaskCreateExt(TaskRTU,
   					(void *)0,
					(OS_STK *)&Task_RTUStk[Task_RTU_STK_SIZE-1],
					Task_RTU_PRIO,
					Task_RTU_PRIO,
					(OS_STK *)&Task_RTUStk[0],
                    Task_RTU_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	
	/* 建立OLED处理任务 */    
   	OSTaskCreateExt(TaskOLED,
   					(void *)0,
					(OS_STK *)&Task_OLEDStk[Task_OLED_STK_SIZE-1],
					Task_OLED_PRIO,
					Task_OLED_PRIO,
					(OS_STK *)&Task_OLEDStk[0],
                    Task_OLED_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	/* 建立功放缓开机处理任务 */    
   	OSTaskCreateExt(TaskPAON,
   					(void *)0,
					(OS_STK *)&Task_PAONStk[Task_PAON_STK_SIZE-1],
					Task_PAON_PRIO,
					Task_PAON_PRIO,
					(OS_STK *)&Task_PAONStk[0],
                    Task_PAON_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);


			   
 
   	while (1)
   	{  	 

#if IWDG_EN
	  IWDG_ReloadCounter(); //手动喂狗  
#endif	
		if(Sys_Flag.Bit.fMS == TRUE)			// 10毫秒定时事件处理
		{
			Sys_Flag.Bit.fMS = FALSE ;
			DoMSEvent();
		}

		if(Sys_Flag.Bit.fSec == TRUE)			// 秒定时事件处理
		{
			Sys_Flag.Bit.fSec = FALSE ;
			Sys_Flag.Bit.fSenEvent = TRUE;   	// 秒事件标志,MODEM

			DoSecEvent() ;
		}
		
		if(Sys_Flag.Bit.fMin == TRUE)			// 分定时事件处理
		{
			Sys_Flag.Bit.fMin = FALSE ;
			Sys_Flag.Bit.fMinEvent = TRUE;
			DoMinEvent() ;
		}
		
		if(Sys_Flag.Bit.fHour == TRUE)			// 时定时事件处理
		{
			Sys_Flag.Bit.fHour = FALSE ;
			Sys_Flag.Bit.fHourEvent = TRUE;
			DoHourEvent() ;
		}

		if(Sys_Flag.Bit.fDOM == TRUE)			// 天定时事件处理
		{
			Sys_Flag.Bit.fDOM = FALSE ;
			Sys_Flag.Bit.fDOMEvent = TRUE;
			DoDomEvent() ;
		}		
	
    
	  	OSTimeDlyHMSM(0, 0, 0, 15);
   	}
}
 

/*
 *******************************************************************************
 * 函数名称：TaskOLED()
 * 函数功能：功放缓开机任务
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
 void  TaskOLED(void *pdata)
{ 
	u8 KeyValBuf[10] = {0};

	pdata = pdata ;
	key_flag = 1;

	while(1)
	{
		SM1668_ReadKey(&KeyValBuf[0]);	
		if(KeyValBuf[0] == 0x80)  //开机键
		{
			do
			{
				SM1668_ReadKey(&KeyValBuf[0]);
				OSTimeDlyHMSM(0, 0, 0, 20);
			}while(KeyValBuf[0] == 0x80);
			//printf(" RF SW ON \n");	
			Sys_Flag.Bit.fPaON = 1; 
		}
		else if(KeyValBuf[0] == 0x10)  //关机键
		{
			do
			{
				SM1668_ReadKey(&KeyValBuf[0]);
				OSTimeDlyHMSM(0, 0, 0, 20);
			}while(KeyValBuf[0] == 0x10);
			//printf(" RF SW OFF \n");
			Sys_Flag.Bit.fPaOFF = 1;	
		}
		if((KeyValBuf[0] == 0x00)&&(KeyValBuf[1] == 0x10)) //退出键
		{
			do
			{
				SM1668_ReadKey(&KeyValBuf[0]);
				OSTimeDlyHMSM(0, 0, 0, 20);
			}while(KeyValBuf[1] == 0x10);
			//printf(" OLED EXIT \n");
			key_flag = 4;	
		}
		KEY_MENU_DEAL();
		OSTimeDlyHMSM(0, 0, 0, 50);
	}

}
/*
 *******************************************************************************
 * 函数名称：TaskPAON()
 * 函数功能：功放缓开机任务
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
 void  TaskPAON(void *pdata)
{ 
	u8 KEY_TxLen = 0, err = 0;
	u8 KEY_SendBuf[20];

	pdata = pdata ;

	while(1)
	{
	
	
		if(Sys_Flag.Bit.fPaON == 1)  //开机键
		{
			Sys_Flag.Bit.fPaON = 0;
			//printf(" RF SW ON...... \n");	
			fPaRTU = TRUE;
			OSSemPend(PARS232SEM, 0, &err);
			KEY_TxLen = 0;
			KEY_SendBuf[KEY_TxLen++]= g_DI.Cfg.PA_IPHead; 
			KEY_SendBuf[KEY_TxLen++]= 0x35;
			KEY_SendBuf[KEY_TxLen++]= 0xC3;
			KEY_SendBuf[KEY_TxLen++]= 0xFC;
			KEY_SendBuf[KEY_TxLen++]= 0xC1;
			KEY_SendBuf[KEY_TxLen++]= 0xEE;
			KEY_SendBuf[KEY_TxLen++]= 0xBF;
			KEY_SendBuf[KEY_TxLen++]= 0xAA;
			KEY_SendBuf[KEY_TxLen++]= 0xBB;
			KEY_SendBuf[KEY_TxLen++]= 0xFA;
			PA_RS232_GXTx(KEY_SendBuf,KEY_TxLen, 10); 
			OSSemPost(PARS232SEM);
			fPaRTU = FALSE;	 
		}
		else if(Sys_Flag.Bit.fPaOFF == 1)  //关机键
		{
			Sys_Flag.Bit.fPaOFF = 0;
			//printf(" RF SW OFF...... \n");
			fPaRTU = TRUE;
			OSSemPend(PARS232SEM, 0, &err);
			KEY_TxLen = 0;
			KEY_SendBuf[KEY_TxLen++]= g_DI.Cfg.PA_IPHead; 
			KEY_SendBuf[KEY_TxLen++]= 0x35;
			KEY_SendBuf[KEY_TxLen++]= 0xC3;
			KEY_SendBuf[KEY_TxLen++]= 0xFC;
			KEY_SendBuf[KEY_TxLen++]= 0xC1;
			KEY_SendBuf[KEY_TxLen++]= 0xEE;
			KEY_SendBuf[KEY_TxLen++]= 0xB9;
			KEY_SendBuf[KEY_TxLen++]= 0xD8;
			KEY_SendBuf[KEY_TxLen++]= 0xBB;
			KEY_SendBuf[KEY_TxLen++]= 0xFA;
			PA_RS232_GXTx(KEY_SendBuf,KEY_TxLen, 10); 
			OSSemPost(PARS232SEM);
			fPaRTU = FALSE;	
		}

		OSTimeDlyHMSM(0, 0, 0, 100);
	}

}
/*
 *******************************************************************************
 *
 * Private routines
 *
 *******************************************************************************/
/******************************************************************************
 * 函数名称：DoMSEvent()
 * 函数功能：10毫秒定时处理程序
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
static void DoMSEvent( void )
{ 	
	static u16 delay_300MS =0;

	delay_300MS ++;
	if(delay_300MS >= 30)
	{
		delay_300MS = 0;
	//	UDP_AutoSendLoop();
	}
	return ;
}

/******************************************************************************
 * 函数名称：DoSecEvent()
 * 函数功能：秒定时处理程序
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
static void DoSecEvent( void )
{

	 //u16 temp;
	 static u8 ch ; 
	 static u8 net_cnt=0; 
	 static u8 Collect_cnt  =0; 
	 static u8 EitCollect_cnt  =0;
	 static u8 startnet_cnt =0;
	 static u8 tri_cnt =0;
	 static u8 delay_cnt =0;


	 Sys_Flag.Bit.f3sec =TRUE;

	 if(Sys_Flag.Bit.fPaTimer == FALSE )
	 {
		 if(tri_cnt >= 2)
		 {
			 tri_cnt =0;
			 Sys_Flag.Bit.fPaTimer = TRUE;
		 }
		 else
		 {
		  	tri_cnt++;
		 }
	 }

	 if(Sys_Flag.Bit.fPaRtudata == FALSE )
	 {
		 if(Collect_cnt >= 2)
		 {
			 Collect_cnt =0;
			 Sys_Flag.Bit.fPaRtudata = TRUE ;		// 秒事件标志,RTU
		 }
		 else
		 {
		 	 Collect_cnt++;
		 }	 
	 }
	 if(Sys_Flag.Bit.fEitRtudata == FALSE )
	 {
		 if(EitCollect_cnt >= 2)
		 {
			 EitCollect_cnt =0;
			 Sys_Flag.Bit.fEitRtudata = TRUE ;		// 秒事件标志,RTU
		 }
		 else
		 {
		 	 EitCollect_cnt++;
		 }	 
	 }
	 devdata.rtctime ++;
	 if(devdata.rtctime >= 2500000)
	 {
		devdata.rtctime = 0;
		devdata.S1G1ST = 0;
		devdata.S1G2ST = 0;
		devdata.S1G3ST = 0;
		devdata.S2G1ST = 0;
		devdata.S2G2ST = 0;
		devdata.S2G3ST = 0;
		devdata.S3G1ST = 0;
		devdata.S3G2ST = 0;
		devdata.S3G3ST = 0;
	 }
	 //UDP_INLinkAutoSend(1);
	 OSTimeDlyHMSM(0, 0, 0, 100);
	 //UDP_INLinkAutoSend(2);

	 /* 监控指示灯 */
	 ch =~ch;
  	 if(ch)
	 {
	  	 LED_RUN_ON;
	
	 }
	 else
	 {
		 LED_RUN_OFF;
		 
	 }
	 if((g_DI.Pa[0].Data.DetState & 0x04) == 0x04)
	 {
	 	g_DI.Pa[0].State.Enable = 1;	 
	 }
	 else
	 {
	  	g_DI.Pa[0].State.Enable = 0;
	 }
	 /* 工作指示灯 */
	 if( g_DI.Pa[0].State.Enable == 0 ) //为关状态，工作指示灯闪烁
	 {
	   	if(ch)
		{
//		  	LED_WORK_ON;
			Led_Display |= 0x04;
			SM1668_DisplayLED(0xC0,Led_Display);//工作指示灯亮
		}
		else
		{
//			LED_WORK_OFF;
			Led_Display &= 0xFB;
			SM1668_DisplayLED(0xC0,Led_Display);//工作指示灯灭
		}
	 }
	 else	//开状态，指示灯亮
	 {
//	 	LED_WORK_ON;
		Led_Display |= 0x04;
		SM1668_DisplayLED(0xC0,Led_Display);//工作指示灯亮
	 }     
	 
	/* 
	Get_ADC1();	*/
  	Inpow_dect();
	PowerGet();			// 射频功率获取	 
	PaCurGet();			// 功放电流获取
	OtherGet();         // 其它状态获取
	
/*							 
	g_DI.Pa[0].Data.State =  (g_DI.Pa[0].State.OCRED_Alm<<7)// 过流降功率告警
						   | (g_DI.Pa[0].State.LP_Alm<<6)	// 输入告警	 
					  	   | (g_DI.Pa[0].State.OC_Alm<<5)	// 过电流告警 
						   | (g_DI.Pa[0].State.TPRED_Alm<<4) // 温度降功率告警
					       | (g_DI.Pa[0].State.RV_Alm<<3)	 // 过反射告警
					       | (g_DI.Pa[0].State.TPT_Alm<<2)	 // 过温告警
					       | (0<<1)
					       | (g_DI.Pa[0].State.Enable<<0);// 功放状态
 */

	 
	
   	/* 告警状态:功放告警(输入、输出、反射、过温、过电流)*/	
	g_TMTState[0] = ((g_DI.Pa[0].Data.State)& 0xfe);


	/* 故障指示灯*/
	if((g_TMTState[0])||(g_TMTLink[0]==0)||(g_TMTLink[1]==0))
   	{
	 	//有故障，告警指示灯常亮
//	   	LED_FAULT_ON;
		Led_Display |= 0x02;
		SM1668_DisplayLED(0xC0,Led_Display);//工作指示灯亮	
   	}
	else
	{
//	  	LED_FAULT_OFF;
		Led_Display &= 0xFD;
		SM1668_DisplayLED(0xC0,Led_Display);//工作指示灯灭
	} 

	if((g_TMTLink[0]==1)&&(g_TMTLink[1]==1))	//主监控与功放、激励器都在线
   	{
		Led_Display |= 0x01;
		SM1668_DisplayLED(0xC0,Led_Display);//在线指示灯亮	
   	}
	else if((g_TMTLink[0]==1)||(g_TMTLink[1]==1)) //只有一个在线闪烁提示
	{
		if(ch)
		{
			Led_Display |= 0x01;
			SM1668_DisplayLED(0xC0,Led_Display);//在线指示灯亮
		}
		else
		{
			Led_Display &= 0xFE;
			SM1668_DisplayLED(0xC0,Led_Display);//在线指示灯灭
		}
	}  
	else //都不在线的话，指示灯提示灭
	{
		Led_Display &= 0xFE;
		SM1668_DisplayLED(0xC0,Led_Display);//在线指示灯灭
	}  

		

	return ;
}


/******************************************************************************
 * 函数名称：DoMinEvent()
 * 函数功能：分定时处理程序
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
static void DoMinEvent( void )
{ 	    
	

	return ;
}


/******************************************************************************
 * 函数名称：DoHourEvent()
 * 函数功能：时定时处理程序
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
static void DoHourEvent( void )
{ 

	return ;
}

/******************************************************************************
 * 函数名称：DoDOMEvent()
 * 函数功能：天定时处理程序
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
static void DoDomEvent( void )
{
	
	return ;
}








/*******************************************************************************
 * 函数名称：PowerGet()
 * 函数功能：两点确定直线取值
 * 入口参数：dBm1:坐标X1;Vol1:坐标Y1;dBm2:坐标X2;Vol2:坐标Y2;Vol:已知Y2
 * 出口参数：无
 *******************************************************************************/
void PowerGet(void) 
{    	
		 
	if(( g_DI.Pa[0].Data.ProState & 0x04 ) == 0x04)
	{
		g_DI.Pa[0].State.RV_Alm = 1;		
	}
	else 
	{
		g_DI.Pa[0].State.RV_Alm = 0;
	}


}													   

/*******************************************************************************
 * 函数名称：PaCurGet()
 * 函数功能：两点确定直线取值
 * 入口参数：dBm1:坐标X1;Vol1:坐标Y1;dBm2:坐标X2;Vol2:坐标Y2;Vol:已知Y2
 * 出口参数：无
 *******************************************************************************/
void PaCurGet(void)
{
	
//bit7				bit6				bit5		bit4		bit3		bit2		bit1		bit0 
//外部保护电流过高	外部保护输入过高	反射降功率	温度降功率	过流降功率	反射过高	温度超限	电流过流	
	
	
	if(( g_DI.Pa[0].Data.ProState & 0x08 ) == 0x08)
	{

		g_DI.Pa[0].State.OCRED_Alm = 1;	   // 过流降功率告警
	}
	else
	{
		g_DI.Pa[0].State.OCRED_Alm = 0;
		
	}

	if(( g_DI.Pa[0].Data.ProState & 0x01 ) == 0x01)
	{
		g_DI.Pa[0].State.OC_Alm = 1;		// 过电流告警
	
	}
	else
	{
		g_DI.Pa[0].State.OC_Alm = 0;
	}
	 				    
}

/*******************************************************************************
 * 函数名称：PaCurGet()
 * 函数功能：两点确定直线取值
 * 入口参数：dBm1:坐标X1;Vol1:坐标Y1;dBm2:坐标X2;Vol2:坐标Y2;Vol:已知Y2
 * 出口参数：无
 *******************************************************************************/
void OtherGet(void)
{
//bit7				bit6				bit5		bit4		bit3		       bit2		  bit1		bit0 
//外部保护电流过高	外部保护输入过高	反射降功率	温度降功率	过流降功率	       反射过高	  温度超限	电流过流	
//		      输出功率检测停播位	   功放温度过高	主电压异常	外部保护反射过高   栅压状态	  风扇2	    风扇1

	/* 降功率温度与过温关机保护温度判断 */
	if( ( g_DI.Pa[0].Data.ProState & 0x10 ) == 0x10 )
	{
	 	g_DI.Pa[0].State.TPRED_Alm = 1;		//温度降功率告警
	}
	else
	{
		g_DI.Pa[0].State.TPRED_Alm = 0;
	}
 
	if( ( g_DI.Pa[0].Data.ProState & 0x02 ) == 0x02 )
	{
	 	g_DI.Pa[0].State.TPT_Alm = 1;		// 过温告警
	}
	else
	{
		g_DI.Pa[0].State.TPT_Alm = 0;
	}
  
	
	/* 开风扇条件判断 */
/*	
	if( ( g_DI.Pa[0].Data.DetState & 0x01 ) == 0x01 )
	{
	 	g_DI.Pa[0].Data.FanState = g_DI.Pa[0].Data.FanState | 0x01;
	}
	else
	{
		g_DI.Pa[0].Data.FanState = g_DI.Pa[0].Data.FanState & 0xFE;
	}
	
	if( ( g_DI.Pa[0].Data.DetState & 0x02 ) == 0x02 )
	{
	 	g_DI.Pa[0].Data.FanState = g_DI.Pa[0].Data.FanState | 0x02;
	}
	else
	{
		g_DI.Pa[0].Data.FanState = g_DI.Pa[0].Data.FanState & 0xFD;
	}
	*/

	if(g_DI.Pa[0].Data.PA_TPT >= g_DI.Cfg.OpenFan2_TPTlimit)//开启风扇2温度条件
	{
		SW_FAN2_ON;
		SW_FAN1_ON;
	}
	else if(g_DI.Pa[0].Data.PA_TPT >= g_DI.Cfg.OpenFan1_TPTlimit)//开启风扇1温度条件
	{
		SW_FAN1_ON;
	}
	else if(g_DI.Pa[0].Data.PA_TPT <= (g_DI.Cfg.CloseFan_TPTlimit - 1))//关闭风扇温度条件
	{
		SW_FAN1_OFF;
		SW_FAN2_OFF;
	}

	
}		


/*******************************************************************************
 * 函数名称：Inpow_dect()
 * 函数功能：输入功率检测与故障处理
 * 入口参数：
 * 出口参数：无
 *******************************************************************************/
void Inpow_dect(void)
{
 	if(( g_DI.Pa[0].Data.ProState & 0x40 ) == 0x40)
	{
		g_DI.Pa[0].State.LP_Alm = 1;		
	}
	else
	{
		g_DI.Pa[0].State.LP_Alm = 0;
	}
}	















