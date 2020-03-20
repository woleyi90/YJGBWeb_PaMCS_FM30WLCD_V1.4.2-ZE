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
* ��    �ƣ�int main(void)
* ��    �ܣ����������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
int main(void)
{ 

	SystemInit(); 	
	//SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	Sys_delay(500);  //ϵͳ��ʱ����,��ʱ

	CPU_IntDis();  //��ֹ�����ж� 
   	OSInit();  
   	BSP_Init();   

	
  	OSTaskCreate((void (*) (void *)) App_TaskStart,               		   
                           (void *) 0,											
               		       (OS_STK *) &App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                           (INT8U) APP_TASK_START_PRIO);							
             
   OSTimeSet(0);  //ucosII�Ľ��ļ�������0,���ļ�������0-4294967295  
   OSStart();     //����ucosII�ں�  

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
 * ��    �ƣ�static  void App_TaskStart(void* p_arg)
 * ��    �ܣ���ʼ������
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 *******************************************************************************/
static  void App_TaskStart(void* p_arg)
{ 
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
		}

		if(Sys_Flag.Bit.fSec == TRUE)			// �붨ʱ�¼�����
		{
			Sys_Flag.Bit.fSec = FALSE ;
			Sys_Flag.Bit.fSenEvent = TRUE;   	// ���¼���־,MODEM

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
	u8 KEY_TxLen = 0, err = 0;
	u8 KEY_SendBuf[20];

	pdata = pdata ;

	while(1)
	{
	
	
		if(Sys_Flag.Bit.fPaON == 1)  //������
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
		else if(Sys_Flag.Bit.fPaOFF == 1)  //�ػ���
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
 * �������ƣ�DoMSEvent()
 * �������ܣ�10���붨ʱ�������
 * ��ڲ�������
 * ���ڲ�������
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
			 Sys_Flag.Bit.fPaRtudata = TRUE ;		// ���¼���־,RTU
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
			 Sys_Flag.Bit.fEitRtudata = TRUE ;		// ���¼���־,RTU
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
	 if((g_DI.Pa[0].Data.DetState & 0x04) == 0x04)
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
			Led_Display |= 0x04;
			SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
		}
		else
		{
//			LED_WORK_OFF;
			Led_Display &= 0xFB;
			SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
		}
	 }
	 else	//��״̬��ָʾ����
	 {
//	 	LED_WORK_ON;
		Led_Display |= 0x04;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
	 }     
	 
	/* 
	Get_ADC1();	*/
  	Inpow_dect();
	PowerGet();			// ��Ƶ���ʻ�ȡ	 
	PaCurGet();			// ���ŵ�����ȡ
	OtherGet();         // ����״̬��ȡ
	
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
	g_TMTState[0] = ((g_DI.Pa[0].Data.State)& 0xfe);


	/* ����ָʾ��*/
	if((g_TMTState[0])||(g_TMTLink[0]==0)||(g_TMTLink[1]==0))
   	{
	 	//�й��ϣ��澯ָʾ�Ƴ���
//	   	LED_FAULT_ON;
		Led_Display |= 0x02;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����	
   	}
	else
	{
//	  	LED_FAULT_OFF;
		Led_Display &= 0xFD;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
	} 

	if((g_TMTLink[0]==1)&&(g_TMTLink[1]==1))	//������빦�š�������������
   	{
		Led_Display |= 0x01;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����	
   	}
	else if((g_TMTLink[0]==1)||(g_TMTLink[1]==1)) //ֻ��һ��������˸��ʾ
	{
		if(ch)
		{
			Led_Display |= 0x01;
			SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
		}
		else
		{
			Led_Display &= 0xFE;
			SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
		}
	}  
	else //�������ߵĻ���ָʾ����ʾ��
	{
		Led_Display &= 0xFE;
		SM1668_DisplayLED(0xC0,Led_Display);//����ָʾ����
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
 * �������ƣ�PaCurGet()
 * �������ܣ�����ȷ��ֱ��ȡֵ
 * ��ڲ�����dBm1:����X1;Vol1:����Y1;dBm2:����X2;Vol2:����Y2;Vol:��֪Y2
 * ���ڲ�������
 *******************************************************************************/
void PaCurGet(void)
{
	
//bit7				bit6				bit5		bit4		bit3		bit2		bit1		bit0 
//�ⲿ������������	�ⲿ�����������	���併����	�¶Ƚ�����	����������	�������	�¶ȳ���	��������	
	
	
	if(( g_DI.Pa[0].Data.ProState & 0x08 ) == 0x08)
	{

		g_DI.Pa[0].State.OCRED_Alm = 1;	   // ���������ʸ澯
	}
	else
	{
		g_DI.Pa[0].State.OCRED_Alm = 0;
		
	}

	if(( g_DI.Pa[0].Data.ProState & 0x01 ) == 0x01)
	{
		g_DI.Pa[0].State.OC_Alm = 1;		// �������澯
	
	}
	else
	{
		g_DI.Pa[0].State.OC_Alm = 0;
	}
	 				    
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

	/* �������¶�����¹ػ������¶��ж� */
	if( ( g_DI.Pa[0].Data.ProState & 0x10 ) == 0x10 )
	{
	 	g_DI.Pa[0].State.TPRED_Alm = 1;		//�¶Ƚ����ʸ澯
	}
	else
	{
		g_DI.Pa[0].State.TPRED_Alm = 0;
	}
 
	if( ( g_DI.Pa[0].Data.ProState & 0x02 ) == 0x02 )
	{
	 	g_DI.Pa[0].State.TPT_Alm = 1;		// ���¸澯
	}
	else
	{
		g_DI.Pa[0].State.TPT_Alm = 0;
	}
  
	
	/* �����������ж� */
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

	if(g_DI.Pa[0].Data.PA_TPT >= g_DI.Cfg.OpenFan2_TPTlimit)//��������2�¶�����
	{
		SW_FAN2_ON;
		SW_FAN1_ON;
	}
	else if(g_DI.Pa[0].Data.PA_TPT >= g_DI.Cfg.OpenFan1_TPTlimit)//��������1�¶�����
	{
		SW_FAN1_ON;
	}
	else if(g_DI.Pa[0].Data.PA_TPT <= (g_DI.Cfg.CloseFan_TPTlimit - 1))//�رշ����¶�����
	{
		SW_FAN1_OFF;
		SW_FAN2_OFF;
	}

	
}		


/*******************************************************************************
 * �������ƣ�Inpow_dect()
 * �������ܣ����빦�ʼ������ϴ���
 * ��ڲ�����
 * ���ڲ�������
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















