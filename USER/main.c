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
 * private object
 *
 *******************************************************************************
 */	
extern u8 StartupCtrlDelay;
extern u8 FAN_ManCtrl_Delay;
static  OS_STK App_TaskStartStk[APP_TASK_START_STK_SIZE]; 
static  OS_STK Task_ETHStk[Task_ETH_STK_SIZE];
static  OS_STK Task_PCStk[Task_PC_STK_SIZE]; 
static  OS_STK Task_RTUStk[Task_RTU_STK_SIZE]; 
static  OS_STK Task_PAONStk[Task_PAON_STK_SIZE];
static  OS_STK Task_OLEDStk[Task_OLED_STK_SIZE];
u8  StatusFault=0;

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


void Sys_delay(int ms)
{
	int i = 0,j= 0;
	for(i=0;i<ms;i++)
		for(j=0;j<20000;j++);
}
/****************************************************************************
* ��    �ƣ�int main(void)
* ��    �ܣ����������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
int main(void)
{	 
	//SystemInit(); 
	//SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */	
	delay_init();	    	 	//��ʱ������ʼ��	  	
	Sys_delay(300);  //ϵͳ��ʱ����,��ʱ

	OSInit();				//UCOS��ʼ��
	BSP_Init();   

	OSTaskCreate((void (*) (void *)) App_TaskStart,               		   
                         (void *) 0,											
               		       (OS_STK *) &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                         (INT8U) APP_TASK_START_PRIO);
	OSStart(); //����UCOS
}

//start����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//��ʼ��ͳ������
	OS_ENTER_CRITICAL();  	//���ж�
	
	//OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO); 	//����LED����
 
	OSTaskSuspend(OS_PRIO_SELF); //����start_task����
	OS_EXIT_CRITICAL();  //���ж�
}

/* 
 *******************************************************************************
 *
 * private routines
 * 
 *******************************************************************************
 */
/*******************************************************************************
 * ��    �ƣ�static  void App_TaskStart(void* p_arg)
 * ��    �ܣ���ʼ������
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 *******************************************************************************/
static  void App_TaskStart(void* p_arg)
{ 
  	u8 TimerIndex;
	(void) p_arg;


   	//TCP_NEWSEM=OSSemCreate(1);		//����������ź���
	//UISETMSG   = OSQCreate(&UISetArrayOfMsg[0], MSG_SIZE); 
	RS485SEM = OSSemCreate(1); 			// RTU485�ź���
	PARS232SEM = OSSemCreate(1); 			// ����RTU�����ź���
	JLQRS232SEM = OSSemCreate(1); 			// ������RTU�����ź���
   

   	/* ��ʼ��ucosIIʱ�ӽ���	*/
   	OS_CPU_SysTickInit();  
      
   	/* �������紦������ */ 	   
   	OSTaskCreateExt(thread_udp_broadcast_server,
   					(void *)0,
					(OS_STK *)&Task_ETHStk[Task_ETH_STK_SIZE-1],
					Task_ETH_PRIO,
					Task_ETH_PRIO,
					(OS_STK *)&Task_ETHStk[0],
                    Task_ETH_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	/* ����PC�������� */    
   	OSTaskCreateExt(TaskPC,
   					(void *)0,
					(OS_STK *)&Task_PCStk[Task_PC_STK_SIZE-1],
					Task_PC_PRIO,
					Task_PC_PRIO,
					(OS_STK *)&Task_PCStk[0],
                    Task_PC_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	/* ����RTU�������� */    
   	OSTaskCreateExt(TaskRTU,
   					(void *)0,
					(OS_STK *)&Task_RTUStk[Task_RTU_STK_SIZE-1],
					Task_RTU_PRIO,
					Task_RTU_PRIO,
					(OS_STK *)&Task_RTUStk[0],
                    Task_RTU_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	
	/* ����OLED�������� */    
   	OSTaskCreateExt(TaskOLED,
   					(void *)0,
					(OS_STK *)&Task_OLEDStk[Task_OLED_STK_SIZE-1],
					Task_OLED_PRIO,
					Task_OLED_PRIO,
					(OS_STK *)&Task_OLEDStk[0],
                    Task_OLED_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK|OS_TASK_OPT_STK_CLR);

	/* �������Ż������������� */    
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
	  IWDG_ReloadCounter(); //�ֶ�ι��  
#endif	
		if(Sys_Flag.Bit.fMS == TRUE)			// 10���붨ʱ�¼�����
		{
			Sys_Flag.Bit.fMS = FALSE ;
			DoMSEvent();
			if( User_Timer[2].Timer_Enable )
			{
				User_Timer[2].Timer_Count++;
				if( User_Timer[2].Timer_Count >=  User_Timer[2].Timer_Out_Count)
				{
					User_Timer[2].Timer_Enable = 0;
					User_Timer[2].Timer_Count  = 0;
					User_Timer[2].Timer_Timeout = 1;
				}
			} 
		} 

		if(Sys_Flag.Bit.fSec == TRUE)			// �붨ʱ�¼�����
		{
			Sys_Flag.Bit.fSec = FALSE ;
			Sys_Flag.Bit.fSenEvent = TRUE;   	// ���¼���־,MODEM
			if( StartupCtrlDelay < (STARTUPDELAYCONTER*5) )
			{
				StartupCtrlDelay++;
			}

			/*�Զ��嶨ʱ������*/
			for(TimerIndex=0;TimerIndex<2; TimerIndex++)
			{
				if( User_Timer[TimerIndex].Timer_Enable )
				{
					User_Timer[TimerIndex].Timer_Count++;
					
					if( User_Timer[TimerIndex].Timer_Count >=  User_Timer[TimerIndex].Timer_Out_Count)
					{
					
						User_Timer[TimerIndex].Timer_Enable = 0;
						User_Timer[TimerIndex].Timer_Count  = 0;
						User_Timer[TimerIndex].Timer_Timeout = 1;
					}
				}    
			}
			/*�����ֶ�������ʱ*/
			if( FAN_ManCtrl_Delay >0 )
			    FAN_ManCtrl_Delay--;
			DoSecEvent() ;
		}
		
		if(Sys_Flag.Bit.fMin == TRUE)			// �ֶ�ʱ�¼�����
		{
			Sys_Flag.Bit.fMin = FALSE ;
			Sys_Flag.Bit.fMinEvent = TRUE;
			DoMinEvent() ;
		}
		
		if(Sys_Flag.Bit.fHour == TRUE)			// ʱ��ʱ�¼�����
		{
			Sys_Flag.Bit.fHour = FALSE ;
			Sys_Flag.Bit.fHourEvent = TRUE;
			DoHourEvent() ;
		}

		if(Sys_Flag.Bit.fDOM == TRUE)			// �춨ʱ�¼�����
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
 * �������ƣ�TaskOLED()
 * �������ܣ����Ż���������
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
 void  TaskOLED(void *pdata)
{ 
	u8 KeyValBuf[10] = {0};

	pdata = pdata ;
	key_flag = 1;

	while(1)
	{
		SM1668_ReadKey(&KeyValBuf[0]);	
		if(KeyValBuf[0] == 0x80)  //������
		{
			do
			{
				SM1668_ReadKey(&KeyValBuf[0]);
				OSTimeDlyHMSM(0, 0, 0, 20);
			}while(KeyValBuf[0] == 0x80);
			//printf(" RF SW ON \n");	
			Sys_Flag.Bit.fPaON = 1; 
		}
		else if(KeyValBuf[0] == 0x10)  //�ػ���
		{
			do 
			{
				SM1668_ReadKey(&KeyValBuf[0]);
				OSTimeDlyHMSM(0, 0, 0, 20);
			}while(KeyValBuf[0] == 0x10);
			//printf(" RF SW OFF \n");
			Sys_Flag.Bit.fPaOFF = 1;	
		}
		if((KeyValBuf[0] == 0x00)&&(KeyValBuf[1] == 0x10)) //�˳���
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
 * �������ƣ�TaskPAON()
 * �������ܣ����Ż���������
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
 void  TaskPAON(void *pdata)
{ 
    static int tmpdelay = 0;
	float temp_value;		
	short temp_now;
	short temp_nowbak;
	OS_CPU_SR  cpu_sr;
	
	pdata = pdata ;
	
	if( SelfTest() )
	{
	    LED_WORK_ON;
		OSTimeDlyHMSM(0, 0, 1, 0); 
		LED_WORK_OFF;
		
	}
	SetGateCtrl(1);
	while(1)
	{
	
		Get_ADC1();
		Caculate_ADC();
		tmpdelay ++;
		if(tmpdelay >20)
		{ 
			tmpdelay = 0;
			OS_ENTER_CRITICAL();  //�����ٽ���
			temp_now = DS18B20_Get_Temp();
			OS_EXIT_CRITICAL();  //�˳��ٽ���
//			printf("temp_now : %d \n",temp_now);
//			if(temp_now == 0)
//				printf("temp_now NOK 0\n");
//			if((temp_now >(temp_nowbak+20))||(temp_now <(temp_nowbak-20)))			
//				printf("temp_now NOK 1 \n");
			if(temp_now !=0)
			{
				if((temp_now > -350)&&(temp_now < 990))
				{
					g_DI.Pa[0].Data.PA_TPT = (float)(temp_now/10.0);
					temp_nowbak = temp_now;
				}
				
			}
			Temp_CheckProcess();			
		}		
		Main_Process();
		OSTimeDlyHMSM(0, 0, 0, 70);
	}

}
/*
 *******************************************************************************
 *
 * Private routines
 *
 *******************************************************************************/
/******************************************************************************
 * �������ƣ�DoMSEvent()
 * �������ܣ�10���붨ʱ�������
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
static void DoMSEvent( void )
{ 	
/*	static u16 delay_300MS =0;

	delay_300MS ++;
	if(delay_300MS >= 30)
	{
		delay_300MS = 0;
	//	UDP_AutoSendLoop();
	}
*/

	
	return ;
}

/******************************************************************************
 * �������ƣ�DoSecEvent()
 * �������ܣ��붨ʱ�������
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
static void DoSecEvent( void )
{

	 //u16 temp;
	 static u8 ch ; 
	 static u8 net_cnt=0; 
	 static u8 Collect_cnt  =0; 
	 static u8 tri_cnt =0;
//	 static u8 delay_cnt =0;


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
			 Sys_Flag.Bit.fPaRtudata = TRUE ;		// ���¼���־,RTU
		 }
		 else
		 {
		 	 Collect_cnt++;
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
	 //OSTimeDlyHMSM(0, 0, 0, 100);
	 //UDP_INLinkAutoSend(2);

	 /* ���ָʾ�� */
	 ch =~ch;
  	 if(ch)
	 {
	  	 LED_RUN_ON;
	
	 }
	 else
	 {
		 LED_RUN_OFF;
		 
	 }
//	 if((g_DI.Pa[0].Data.DetState & 0x04) == 0x04)
	 if(GateVolCheck())
	 {
	 	g_DI.Pa[0].State.Enable = 1;	 
	 }
	 else
	 {
	  	g_DI.Pa[0].State.Enable = 0;
	 }
	 /* ����ָʾ�� */
	 if( g_DI.Pa[0].State.Enable == 0 ) //Ϊ��״̬������ָʾ����˸
	 {
	   	if(ch)
		{
//		  	LED_WORK_ON;
			Led_Display |= 0x02;
			SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
		}
		else
		{
//			LED_WORK_OFF;
			Led_Display &= 0xFD;
			SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
		}
	 }
	 else	//��״̬��ָʾ����
	 {
//	 	LED_WORK_ON;
		Led_Display |= 0x02;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
	 }     
	 

//  Inpow_dect();
//	PowerGet();			// ��Ƶ���ʻ�ȡ	 
//	PaCurGet();			// ���ŵ�����ȡ
//	OtherGet();         // ����״̬��ȡ
	
/*							 
	g_DI.Pa[0].Data.State =  (g_DI.Pa[0].State.OCRED_Alm<<7)// ���������ʸ澯
						   | (g_DI.Pa[0].State.LP_Alm<<6)	// ����澯	 
					  	   | (g_DI.Pa[0].State.OC_Alm<<5)	// �������澯 
						   | (g_DI.Pa[0].State.TPRED_Alm<<4) // �¶Ƚ����ʸ澯
					       | (g_DI.Pa[0].State.RV_Alm<<3)	 // ������澯
					       | (g_DI.Pa[0].State.TPT_Alm<<2)	 // ���¸澯
					       | (0<<1)
					       | (g_DI.Pa[0].State.Enable<<0);// ����״̬
 */

	 
	
   	/* �澯״̬:���Ÿ澯(���롢��������䡢���¡�������)*/	
	g_TMTState[0] = (g_DI.Pa[0].State.OCRED_Alm<<7)// ���������ʸ澯
				   | (g_DI.Pa[0].State.LP_Alm<<6)	// ����澯	 
				   | (g_DI.Pa[0].State.OC_Alm<<5)	// �������澯 
				   | (g_DI.Pa[0].State.TPRED_Alm<<4) // �¶Ƚ����ʸ澯
				   | (g_DI.Pa[0].State.RV_Alm<<3)	 // ������澯
				   | (g_DI.Pa[0].State.TPT_Alm<<2)	 // ���¸澯
				   | (0<<1)
				   | (0<<0);// ����״̬


	/* ����ָʾ��*/
	if((g_TMTState[0])||(g_TMTLink[1]==0))
   	{
	 	//�й��ϣ��澯ָʾ�Ƴ���
//	   	LED_FAULT_ON;
		Led_Display |= 0x04;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����	
   	}
	else
	{
//	  	LED_FAULT_OFF;
		Led_Display &= 0xFB;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
	} 

	if(g_TMTLink[1]==1)	//������뼤����������
   	{
		Led_Display |= 0x08;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����	
   	}
	else //�������ߵĻ���ָʾ����ʾ��
	{
		Led_Display &= 0xF7;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
	}  
	if(g_DI.Cfg.P_AutoContrl == 1)//�Զ�
	{
		  LED_WORK_ON; 
		
	}
	else
	{
		LED_WORK_OFF;	
	}
	
	if(g_DI.Cfg.P_AutoContrl == 1)//�Զ�
	{
		  LED_WORK_ON; 
		
	}
	else
	{
		LED_WORK_OFF;	
	}
	if(g_DI.Pa[0].State.LP_Alm == 1)//����͸澯
	{
		 LED_PWRIN_ALM; 
		
	}
	else
	{
		LED_PWRIN_NORMAL;	
	}
	if(g_DI.Pa[0].State.LP_Alm == 1)//����͸澯
	{
		 LED_PWRIN_ALM; 
		
	}
	else
	{
		LED_PWRIN_NORMAL;	
	} 
	if(g_DI.Pa[0].State.RV_Alm == 1)//����͸澯
	{
		 LED_PWRRV_ALM; 
		
	}
	else
	{
		LED_PWRRV_NORMAL;	
	}
	if(g_DI.Pa[0].State.OC_Alm == 1) 	
	{
		LED_PWRCUR_ALM;
	}
	else
	{
		if(g_DI.Pa[0].State.OCRED_Alm == 1)	   // ���������ʸ澯
		{
			if(ch)
			{
				LED_PWRCUR_ALM;
			}
			else
			{
				LED_PWRCUR_NORMAL;
			}
		}
		else
		{
			LED_PWRCUR_NORMAL;
		}
	}
	
	return ;
}


/******************************************************************************
 * �������ƣ�DoMinEvent()
 * �������ܣ��ֶ�ʱ�������
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
static void DoMinEvent( void )
{ 	    
	

	return ;
}


/******************************************************************************
 * �������ƣ�DoHourEvent()
 * �������ܣ�ʱ��ʱ�������
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
static void DoHourEvent( void )
{ 

	return ;
}

/******************************************************************************
 * �������ƣ�DoDOMEvent()
 * �������ܣ��춨ʱ�������
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
static void DoDomEvent( void )
{
	
	return ;
}








/*******************************************************************************
 * �������ƣ�PowerGet()
 * �������ܣ�����ȷ��ֱ��ȡֵ
 * ��ڲ�����dBm1:����X1;Vol1:����Y1;dBm2:����X2;Vol2:����Y2;Vol:��֪Y2
 * ���ڲ�������
 *******************************************************************************/
void PowerGet(void) 
{    	
		 



}													   

/*******************************************************************************
 * �������ƣ�PaCurGet()
 * �������ܣ�����ȷ��ֱ��ȡֵ
 * ��ڲ�����dBm1:����X1;Vol1:����Y1;dBm2:����X2;Vol2:����Y2;Vol:��֪Y2
 * ���ڲ�������
 *******************************************************************************/
void PaCurGet(void)
{
	

	 				    
}

/*******************************************************************************
 * �������ƣ�PaCurGet()
 * �������ܣ�����ȷ��ֱ��ȡֵ
 * ��ڲ�����dBm1:����X1;Vol1:����Y1;dBm2:����X2;Vol2:����Y2;Vol:��֪Y2
 * ���ڲ�������
 *******************************************************************************/
void OtherGet(void)
{
//bit7				bit6				bit5		bit4		bit3		       bit2		  bit1		bit0 
//�ⲿ������������	�ⲿ�����������	���併����	�¶Ƚ�����	����������	       �������	  �¶ȳ���	��������	
//		      ������ʼ��ͣ��λ	   �����¶ȹ���	����ѹ�쳣	�ⲿ�����������   դѹ״̬	  ����2	    ����1


	
}		


/*******************************************************************************
 * �������ƣ�Inpow_dect()
 * �������ܣ����빦�ʼ������ϴ���
 * ��ڲ�����
 * ���ڲ�������
 *******************************************************************************/
void Inpow_dect(void)
{

}	

