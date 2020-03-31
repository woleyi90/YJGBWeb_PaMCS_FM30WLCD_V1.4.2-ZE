/*****************************************************************************
 * Function  : Implememt BSP function form for this workbench.
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
	
#include "global.h"	
#include "includes.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "httpd.h"
#include "udp_demo.h"
#include "arch/sys_arch.h"
#include "lwip/lwip_sys.h"

				 

/* 
 *******************************************************************************
 *
 * private routines prototype
 * 
 *******************************************************************************
 */
void Ini_SysRam(void); 
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void USART1_Config(u32 baud);
void USART2_Config(u32 baud);  
void USART3_Config(u32 baud);
void USART4_Config(u32 baud);  
void USART5_Config(u32 baud);
void SPI2_Init(void);
void DMA1_Init(void);
void ADC1_Init(void);
void I2C1_Configuration(void);
void Wdg_Init(void);
void IWDG_Init(void);

void InitNet(void);	

char *itoa(int value, char *string, int radix);
void Ini_SysRam(void);
void Write_Default(u8 partype);
void Get_ChipID(void);	
void WriteAutoMsg_Default(void);
void USART1_DMA_Config(void);

extern MYTIMER User_Timer[3];
/*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */


/*******************************************************************************
 * ��    �ƣ�void BSP_Init(void)
 * ��    �ܣ��ܶ����ʼ������
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 *******************************************************************************/  
void BSP_Init(void)
{ 	
//	u8 i = 0;
	u8 readbuf[10];
	Ini_SysRam(); 

  	RCC_Configuration();  	       //ϵͳʱ�ӳ�ʼ��	
  		
	NVIC_Configuration();		   //�ж�Դ����  
   
  	GPIO_Configuration();		   //GPIO����
	DS18B20_Init();
  	USART1_Config(115200);          //��ʼ������1 PC
	USART1_DMA_Config();
	USART2_Config(115200);          //��ʼ������2 
//    USART3_Config(115200);          //��ʼ������3 ���ư崮��RS232ͨ�� 
    USART5_Config(19200);          //��ʼ������5 RS485ͨ�� ���Ŵ���RS232ͨ�� 
	FSMC_SRAM_Init();		//��ʼ���ⲿSRAM	
//	my_mem_init(SRAMEX);		//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		//��ʼ���ⲿ�ڴ��
	
	SPI1_Init();				   //X5323 SPI ��ʼ��   
	SPI2_Init();                   //ENC28J60 SPI ��ʼ�� 
	ADC1_Init();
	DMA1_Init();
		
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //����ADC	
	Get_ChipID();//��ȡCPUΨһID
	 /* ���FLASH���Ƿ���ϵͳ��־,����,����ϵͳĬ����������*/	  	
	/*	
	if (CheckICON(cICON, (u8*)StartAddr) == FALSE)
	{
		Write_Default(); 			
	}  
	FlashToRam((u8*)&g_DI.Cfg, (u8*)StartAddr, Save_Size);	
	*/
	ReadbufFromX5323(2000,10,(u8*)&readbuf);
	printf("factorycfg readbuf[0]: %d \r\n",readbuf[0]);
	if(readbuf[0] != cICON)
	{
		printf("factorycfg write..... \r\n");
		Write_Default(1);
		printf("factorycfg write ok..... \r\n");	    
	}
	 
	ReadbufFromX5323(10,10,(u8*)&readbuf);
	printf("readbuf[0]: %d \r\n",readbuf[0]);
	if(readbuf[0] != cICON)
	{
		printf("write..... \r\n");
		Write_Default(0);
		printf("write ok..... \r\n");	    
	}
	ReadbufFromX5323(10,Save_Size,(u8*)&g_DI.Cfg);
	
	ReadbufFromX5323(1300,10,(u8*)&readbuf);
	printf("readbuf[0]: %d \r\n",readbuf[0]);
	if(readbuf[0] != cAutoMsgICON)
	{
		printf("write..... \r\n");
		WriteAutoMsg_Default();	
		printf("write ok..... \r\n");    
	}
	ReadbufFromX5323(1300,AutoMsgSave_Size,(u8*)&g_DI.AutoMsgCfg);

	IWDG_Init();    
	/* ��ʼ�������豸�Լ�UIPЭ��ջ������IP��ַ */ 
	printf("	InitNet ! \r\n");//lwip3?��??��3��1|
	InitNet();	
#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif
   while(lwip_comm_init()) 
	{

		printf("	LWIP Init Falied! \r\n");
	}
	printf("	SRAMEX USED: %d \r\n",my_mem_perused(SRAMEX));
	httpd_init(); 	 		 

	

#if IWDG_EN
	IWDG_ReloadCounter(); //�ֶ�ι��  
#endif	
	printf("	DZYJGB 100W:SW=%d.%d.%d,  HW=GXMCS_DTV&FM_%d.%d \r\n",SOFT_VERSION/100,SOFT_VERSION%100/10,SOFT_VERSION%10,Hard_VERSION/10,Hard_VERSION%10);
	printf("	OK! \r\n");
//	printf("	Save_Size: %d \r\n",Save_Size);	
//	printf("	AutoMsgSave_Size: %d \r\n",AutoMsgSave_Size);
	
//	printf("g_DI.Cfg.LOCAL1NetPort2= %d \r\n", g_DI.Cfg.LOCAL1NetPort2);
	printf("IP: %d.%d.%d.%d \r\n", g_DI.Cfg.IP_Addr[0],g_DI.Cfg.IP_Addr[1],g_DI.Cfg.IP_Addr[2],g_DI.Cfg.IP_Addr[3]);
	printf("g_DI.Cfg.Net_Port= %d \r\n", g_DI.Cfg.Net_Port);
	//printf("g_DI.Cfg.ManuFactory: %s ..\r\n", g_DI.Cfg.ManuFactory); 
/*	
	printf("--------------------------------\r\n");


	printf("g_DI.AutoMsgCfg.AutoMsg_Cfg1_Len= %d \r\n", g_DI.AutoMsgCfg.AutoMsg_Cfg1_Len);
	printf("g_DI.AutoMsgCfg.AutoMsg_Cfg2_Len= %d \r\n", g_DI.AutoMsgCfg.AutoMsg_Cfg2_Len);
	printf("g_DI.AutoMsgCfg.AutoMsg_Cfg3_Len= %d \r\n", g_DI.AutoMsgCfg.AutoMsg_Cfg3_Len);
	printf("g_DI.AutoMsgCfg.AutoMsg_INCfg1_Len= %d \r\n", g_DI.AutoMsgCfg.AutoMsg_INCfg1_Len);
	printf("g_DI.AutoMsgCfg.AutoMsg_INCfg2_Len= %d \r\n", g_DI.AutoMsgCfg.AutoMsg_INCfg2_Len);
	for(i=0; i<50; i++)
	{
		IWDG_ReloadCounter();  printf("g_DI.AutoMsgCfg.AutoMsg_Cfg1[%d]= 0x%x \r\n", i, g_DI.AutoMsgCfg.AutoMsg_Cfg1[i]);
	}
	printf("--------------------------------\r\n");
	for(i=0; i<50; i++)
	{
		IWDG_ReloadCounter();printf("g_DI.AutoMsgCfg.AutoMsg_Cfg2[%d]= 0x%x \r\n", i, g_DI.AutoMsgCfg.AutoMsg_Cfg2[i]);
	}  
	printf("--------------------------------\r\n");
	for(i=0; i<50; i++)
	{
	IWDG_ReloadCounter();	printf("g_DI.AutoMsgCfg.AutoMsg_Cfg3[%d]= 0x%x \r\n", i, g_DI.AutoMsgCfg.AutoMsg_Cfg3[i]);
	}
	printf("--------------------------------\r\n");
	for(i=0; i<50; i++)
	{
	IWDG_ReloadCounter();	printf("g_DI.AutoMsgCfg.AutoMsg_INCfg1[%d]= 0x%x \r\n", i, g_DI.AutoMsgCfg.AutoMsg_INCfg1[i]);

	}
	printf("--------------------------------\r\n");
	for(i=0; i<50; i++)
	{
		IWDG_ReloadCounter();	printf("g_DI.AutoMsgCfg.AutoMsg_INCfg2[%d]= 0x%x \r\n", i, g_DI.AutoMsgCfg.AutoMsg_INCfg2[i]);
	}
	*/	 
	LCD_Initial();
	printf("LCD_Initial ok.............\r\n");     


	Led_Display = 0x01;
	SM1668_DisplayLED(0xC0,Led_Display);//PWRָʾ����
	 
	printf("bsp web ok 20200331...........\r\n"); 


} 				 
   
   

/*******************************************************************************
 * ��    �ƣ�void  OS_CPU_SysTickInit(void)
 * ��    �ܣ�ucos ϵͳ����ʱ�ӳ�ʼ��  ��ʼ����Ϊ10msһ�ν���
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 * ���÷������� 
 *******************************************************************************/
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;
    RCC_GetClocksFreq(&rcc_clocks);		                        //���ϵͳʱ�ӵ�ֵ	 
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;	//���ʱ�ӽ��ĵ�ֵ	
	SysTick_Config(cnts);										//����ʱ�ӽ���	     
}



/* 
 *******************************************************************************
 *
 * private routines
 * 
 *******************************************************************************/
/*******************************************************************************
 * �������ƣ�Ini_SysRam()
 * �������ܣ���ʼ��ϵͳ����
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
void Ini_SysRam(void)
{
    
   *(u32 *)&Sys_Flag = 0x00000000;			// ϵͳ��־����	
   *(u32 *)&SysNet_Flag = 0x00000000;			// ϵͳ��־����	
	//��ʼ�� ���ڽ��սṹ��
	UART1_RBuf.Len = 0 ;
	UART1_RBuf.TimeOut = 0 ;
	UART1_RBuf.fEnd = FALSE ;

	g_DI.Cfg.ID =0;

	UART2_RBuf.Len = 0 ;
	UART2_RBuf.TimeOut = 0 ;
	UART2_RBuf.fEnd = FALSE ;
	
	UART3_RBuf.Len = 0 ;
	UART3_RBuf.TimeOut = 0 ;
	UART3_RBuf.fEnd = FALSE ;
	
	UART5_RBuf.Len = 0 ;
	UART5_RBuf.TimeOut = 0 ;
	UART5_RBuf.fEnd = FALSE ;  	


	g_ActiveAlm_bak=0;
	g_SWRAlm_ReSendNum = cWaitNum;
	g_SWRAlm_WaitSec   = cWaitSec; 
	g_SWRAlmRst_WaitSec= 0;
	netLink_Mcnt =0;


	Sys_Flag.Bit.fPaRtudata = FALSE;
	Sys_Flag.Bit.fEitRtudata = FALSE;
	Sys_Flag.Bit.fAlmOFF = 1;
	g_DI.ALL.Data.Input_SMA_Flag = 0x00;// �������ر�־λ��0x00��ʾ��Ч

	fRS485 = FALSE;
	
	fPaRTU = FALSE;
	fEitRTU = FALSE;
	flag_TLink[0] = 1;	// ����ͨ��״̬
	flag_TLink[1] = 1;	// ����ͨ��״̬
	flag_TLink[2] = 1;	// ����ͨ��״̬
	Sys_Flag.Bit.fPaON = 0;
	Sys_Flag.Bit.fPaOFF = 0;
	Sys_Flag.Bit.fPaCheck = FALSE;
	Sys_Flag.Bit.fPanetQue = FALSE;
	Sys_Flag.Bit.fExcitCheck = FALSE;
	Sys_Flag.Bit.fAutoMsgSave = FALSE;
	Sys_Flag.Bit.fSaveStart = FALSE; 
	g_DI.ALL.Data.Input_SMA_ALM = 0x00;	//�����޸澯
	Sys_Flag.Bit.fPaTimer = FALSE;
	
	memset((u8*)&cpaitt, 0, 32);
	memset((u8*)&cExcitDirection, 0, 32);
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
    CurTimes = 0;
	Sys_Flag.Bit.fSave = FALSE;
	SysNet_Flag.Bit.fNetConfig = FALSE;
	SysNet_Flag.Bit.fUdpRecom = FALSE;
	SysNet_Flag.Bit.fPaOutCrt = FALSE;
	SysNet_Flag.Bit.fPaInCrt = FALSE;
	SysNet_Flag.Bit.fPaRvCrt = FALSE;
	PaOutfout = 0.0;
	PaInfout = 0.0;
	PaRvfout = 0.0;
//	memset((u8*)&Pa_WorkState, 0, sizeof(PAState_Data));
	Encoder_CW = 0;
	Encoder_CCW = 0;

  
//	cn_temp = 0;
	g_DI.Pa[0].Data.I_Status = 0;
	g_DI.Pa[0].Data.I_StatusTrig = 0; 
	g_DI.Pa[0].Data.I_StatusRelease = 0;   
	g_DI.Pa[0].Data.I_StatusCont = 0;

	g_DI.Pa[0].Data.TEMP_Status = 0;	  //�¶�״̬
	g_DI.Pa[0].Data.TEMP_StatusTrig = 0;	  //�¶�״̬����
	g_DI.Pa[0].Data.TEMP_StatusCont = 0;	  //�¶�״̬����
	g_DI.Pa[0].Data.TEMP_StatusRelease = 0;	  //�¶�״̬�ͷ�

	g_DI.Pa[0].Data.RF_Status = 0;
	g_DI.Pa[0].Data.RF_StatusTrig = 0;
	g_DI.Pa[0].Data.RF_StatusCont = 0;
	
	g_DI.Pa[0].Data.Temp_ReducePowerCount  = 0;   //�¶ȱ����ѽ��͹��ʴ��� 
	g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;	//�ѹ���������������ֵ
	g_DI.Pa[0].Data.Protect_Shutdown_Cause = 0;
	g_DI.Pa[0].Data.P_DownOffset = 0;
	g_DI.Pa[0].Data.RES_Front = 0;
	g_DI.Pa[0].Data.RES_Rear = 0;
	/*���ʿ�����ʱ��ⶨʱ��*/
	User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 0;
	User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
	User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count =30; //2��
	User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
	/*�����Զ����ƶ�ʱ��*/
	User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
	User_Timer[POWERAUTOTIMER].Timer_Enable = 1;
	User_Timer[POWERAUTOTIMER].Timer_Count = 0 ;
	User_Timer[POWERAUTOTIMER].Timer_Out_Count = 50;
	
	User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
	User_Timer[OVERLOADRESTARTTIMER].Timer_Enable = 0;
	User_Timer[OVERLOADRESTARTTIMER].Timer_Count =0 ;
	User_Timer[OVERLOADRESTARTTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;//��������ʱ��;
}


/*******************************************************************************
 * �������ƣ�Write_Default()
 * �������ܣ���FLASHд��Ĭ�����ݡ�
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
void Write_Default(u8 partype)
{	
	u8 i = 0;
	g_DI.Cfg.ICON  = cICON;
	g_DI.Cfg.FunID = cFunID;
	g_DI.Cfg.ID    = cID;
	

	/* ��ʼ��IP��ַ */	
	memcpy(g_DI.Cfg.IP_Server, (u8 *)&cIP_Server, 4);
	memcpy(g_DI.Cfg.IP_Addr, (u8 *)&cIP_Addr, 4);
	memcpy(g_DI.Cfg.IP_Mask, (u8 *)&cIP_Mask, 4);
	memcpy(g_DI.Cfg.IP_GateWay, (u8 *)&cIP_GateWay, 4);
	memcpy(g_DI.Cfg.IP_DNS, (u8 *)&cIP_DNS, 4);	  
	g_DI.Cfg.Net_Port = cNet_Port;	 
	    
	g_DI.Cfg.PA_IPHead = 0x25;
	g_DI.Cfg.Excit_IPHead = 0x33;		 

	g_DI.Cfg.CloseFan_TPTlimit = 45;//�رշ����¶�
	g_DI.Cfg.OpenFan1_TPTlimit = 48;//��������1�¶�
	g_DI.Cfg.OpenFan2_TPTlimit = 53;//��������2�¶�
	g_DI.Cfg.PWRRED_TPTlimit = 65;//�������¶�
	g_DI.Cfg.PWROFF_TPTlimit = 75;//���¹ػ������¶�
	g_DI.Cfg.PWR_RVlimit = 5;//���䱣������
	g_DI.Cfg.PWRRED_CURlimit = 1.6;//�����ʵ���
	g_DI.Cfg.PWROFF_CURlimit = 1.8;//����������
	g_DI.Cfg.CurRstNum = 5;  
	g_DI.Cfg.CurRstDelay = 100;
	g_DI.Cfg.PWR_INlimit = -40.0;//������С����
	g_DI.Cfg.PWR_RateOut = 30;
	 
	/***********************�����**************************/
	g_DI.Cfg.P_FmFrq = 87.0;					  		//���Ƶ��ֵ
	g_DI.Cfg.P_InCal = 0.465;						  //���빦��У��ֵ
	g_DI.Cfg.P_OutCal= 3600.867;						 //�������У��ϵ��
	g_DI.Cfg.P_RefCal = 1665.186;						  //���书��У��ϵ��
	g_DI.Cfg.P_AutoContrl = 1;
	g_DI.Cfg.P_Res_Front_Target = 50;
	g_DI.Cfg.PWR_SetRateOut = 30;
	
	g_DI.Cfg.Temp_ReducePowerInterval = 180;//  = 30 ;      //�����ʼ��ʱ�� ��
	g_DI.Cfg.Temp_ReducePowerMaxCount = 3;// = 3;	        //���͹���������
	/*************************************************/
	g_DI.Cfg.DeviceID =  g_DI.Cfg.IP_Addr[3];

	DataCopy((u8 *)g_DI.Cfg.StationInfo, (u8 *)&cStationInfo,17);
	DataCopy((u8 *)g_DI.Cfg.DeviceName, (u8 *)&cDeviceName,32);
	DataCopy((u8 *)g_DI.Cfg.ManuFactory, (u8 *)&cManuFactory,32);
	DataCopy((u8 *)g_DI.Cfg.PartNo, (u8 *)&cPartNo,32);
	DataCopy((u8 *)g_DI.Cfg.SerialNo, (u8 *)&cSerialNo,16);	
	DataCopy((u8 *)g_DI.Cfg.PaItt, (u8 *)&cpaitt,32);
	DataCopy((u8 *)g_DI.Cfg.ExcitDirection, (u8 *)&cExcitDirection,32);

	DataCopy((u8 *)g_DI.Cfg.IP_LOCAL1, (u8 *)&cIP_LOCAL1,4);
	DataCopy((u8 *)g_DI.Cfg.IP_LOCAL2, (u8 *)&cIP_LOCAL2,4);

	DataCopy((u8 *)g_DI.Cfg.IP_INSERVER1, (u8 *)&cIP_INSERVER1,4);
	DataCopy((u8 *)g_DI.Cfg.IP_INSERVER2, (u8 *)&cIP_INSERVER2,4);

	DataCopy((u8 *)g_DI.Cfg.IP_RemoteHost1, (u8 *)&cIP_RemoteHost1,4);
	DataCopy((u8 *)g_DI.Cfg.IP_RemoteHost2, (u8 *)&cIP_RemoteHost2,4);
	DataCopy((u8 *)g_DI.Cfg.IP_RemoteHost3, (u8 *)&cIP_RemoteHost3,4);


	g_DI.Cfg.LOCAL1NetPort1 = g_DI.Cfg.IP_LOCAL1[3]+2000;		// ���ع���1-�˿ں�
	g_DI.Cfg.LOCAL1NetPort2 = g_DI.Cfg.IP_LOCAL2[3]+2000;		// ���ع���2-�˿ں�

	g_DI.Cfg.INSERVERNetPort1 = 0;		// �ڲ�����1-�˿ں�
	g_DI.Cfg.INSERVERNetPort2 = 0;		// �ڲ�����2-�˿ں�


	g_DI.Cfg.RemoteHostNetPort1 = 6666;	// Զ������1-�˿ں�
	g_DI.Cfg.RemoteHostDelay1_1 = 2;		// Զ������1-�� 1 ���������
	g_DI.Cfg.RemoteHostDelay1_2 = 15;		// Զ������1-�� 2 ���������
	g_DI.Cfg.RemoteHostDelay1_3 = 0;		// Զ������1-�� 3 ���������


	g_DI.Cfg.RemoteHostNetPort2 = 0;		// Զ������2-�˿ں�
	g_DI.Cfg.RemoteHostDelay2_1 = 0;		// Զ������2-�� 1 ���������
	g_DI.Cfg.RemoteHostDelay2_2 = 0;		// Զ������2-�� 2 ���������
	g_DI.Cfg.RemoteHostDelay2_3 = 0;		// Զ������2-�� 3 ���������

    
	g_DI.Cfg.RemoteHostNetPort3 = 0;		// Զ������3-�˿ں�
	g_DI.Cfg.RemoteHostDelay3_1 = 0;		// Զ������3-�� 1 ���������
	g_DI.Cfg.RemoteHostDelay3_2 = 0;		// Զ������3-�� 2 ���������
	g_DI.Cfg.RemoteHostDelay3_3 = 0;		// Զ������3-�� 3 ���������
	if(partype == 0)
	{
		WritebufToX5323(10,Save_Size+50,(u8*)&g_DI.Cfg);
	}
	if(partype == 1)
	{
		WritebufToX5323(2000,Save_Size+50,(u8*)&g_DI.Cfg);
	}	
	 
}
void WriteAutoMsg_Default(void)
{	
	u8 i = 0;

	//�����������
	g_DI.AutoMsgCfg.AutoMsgICON  = cAutoMsgICON;
	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_Cfg1[i] = cAutoMsg_Cfg1[i];	  //��1���Զ���������,
	}
	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_Cfg2[i] = cAutoMsg_Cfg2[i];	  //��2���Զ���������
	}
	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_Cfg3[i] = cAutoMsg_Cfg3[i];	//��3���Զ���������
	}

	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_INCfg1[i] = cAutoMsg_INCfg1[i];	//�ڲ���������1����
	}

	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_INCfg2[i] = cAutoMsg_INCfg2[i];	//�ڲ���������2����
	}   
	g_DI.AutoMsgCfg.AutoMsg_Cfg1_Len = 10;
	g_DI.AutoMsgCfg.AutoMsg_Cfg2_Len = 22;
	g_DI.AutoMsgCfg.AutoMsg_Cfg3_Len = 12;	
	g_DI.AutoMsgCfg.AutoMsg_INCfg1_Len = 0;
	g_DI.AutoMsgCfg.AutoMsg_INCfg2_Len = 0;

	WritebufToX5323(1300,AutoMsgSave_Size+50,(u8*)&g_DI.AutoMsgCfg);		 
}
	 
/*******************************************************************************
 * ��    �ƣ�void RCC_Configuration(void)
 * ��    �ܣ�ϵͳʱ������Ϊ72MHZ�� ����ʱ������
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 * ���÷������� 
 *******************************************************************************/ 
void RCC_Configuration(void){

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//���ù���ʹ��
}

/*******************************************************************************
 * ��    �ƣ�void GPIO_Configuration(void)
 * ��    �ܣ�ͨ��IO������
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 * ���÷�����
 *******************************************************************************/  
void GPIO_Configuration(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_GPIOE |RCC_APB2Periph_GPIOF |
						   RCC_APB2Periph_GPIOG, ENABLE);	
						     	
  	  
	/* ��������*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;	         	 	//ENC28J60��������ж����� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//�ڲ���������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;	         	 	//դѹ 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   	 		//�ڲ���������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	 
	/* ������� */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	 

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_11 | GPIO_Pin_12 ; // LEATT/CLK/STB	
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 ; // SDIN	
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;//��©���
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9; // RUN/ALM/WORK	
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 |GPIO_Pin_7 | GPIO_Pin_8; // DAT_KEY_A/DAT_KEY_B/DAT_KEY
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 	
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 ; // SCLK
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;//��©���
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 	
	
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_9| GPIO_Pin_10 | GPIO_Pin_11; // CS/IO/RS
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6| GPIO_Pin_7; // FAN2_CT/FAN1_CT
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; //I_ALM/SWR_ALM/�����/������ 
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_10; // DisplayLCD
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOF, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_8; // k_control
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOG, &GPIO_InitStructure); 
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   //IO   
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;            // ��©���     
//    GPIO_Init(GPIOC, &GPIO_InitStructure);			   
}

/*******************************************************************************
 * ��    �ƣ�void NVIC_Configuration(void)
 * ��    �ܣ��ж�Դ����
 * ��ڲ�������
 * ���ڲ�������
 * ˵    ����
 * ���÷������� 
 *******************************************************************************/
void NVIC_Configuration(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
  	EXTI_InitTypeDef EXTI_InitStructure;

	/* Set the Vector Table base location at 0x08010000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);  

  	/* Set the Vector Table base location at 0x08010000 */ 
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10000);   
	//SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
  	/* Configure one bit for preemption priority */
  	/* ���ȼ��� ˵������ռ���ȼ����õ�λ�����������ȼ����õ�λ��   ��������1�� 7 */    
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
 
  	/* Enable the USART1 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 
//	NVIC_Init(&NVIC_InitStructure);

	 /* Enable the USART2 Interrupt */
 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 
	NVIC_Init(&NVIC_InitStructure);	

	/* Enable the USART3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);	
	

	/* Enable the USART5 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);	


	/* Enable the WDG Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 
//	NVIC_Init(&NVIC_InitStructure);	
  
  	/* Enable the EXTI Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  	     //�ⲿ�ж�2
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 //��ռ���ȼ� 0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			 //�����ȼ�0  
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //ʹ��
  	NVIC_Init(&NVIC_InitStructure);

  	//��������AFIO�ⲿ�ж����üĴ���AFIO_EXTICR1������ѡ��EXTI2�ⲿ�жϵ�����Դ��PE2��
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);     //�ⲿ�ж�����AFIO--ETXI2

  	EXTI_InitStructure.EXTI_Line = EXTI_Line8;					  
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //�ж�ģʽ
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

	//���������ⲿ�жϣ�����Դ��PC6��
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);     //�ⲿ�ж�����PC6

  	EXTI_InitStructure.EXTI_Line = EXTI_Line6;					  
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //�ж�ģʽ
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;		  //�½��ش���	
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
}


/****************************************************************************
* ��    �ƣ�void NVIC_Configuration(void)
* ��    �ܣ��ж�Դ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void Wdg_Init(void)
{
	/* Enable WDG clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG , ENABLE);

	/* PCKL1: 36MHZ */
	/* WWDG clock counter = (PCLK1/4096)/8 = 488 Hz (~2 ms)  */
  	WWDG_SetPrescaler(WWDG_Prescaler_8);
  	/* Set Window value to 65 */
  	WWDG_SetWindowValue(0x41);
  	/* Enable WWDG and set counter value to 127, WWDG timeout = ~2 ms * 64 = 130 ms */
  	WWDG_Enable(0x7F);

	/* Clear EWI flag */
  	WWDG_ClearFlag();

  	/* Enable EW interrupt */
  	WWDG_EnableIT();
}


/****************************************************************************
* ��    �ƣ�void IWDG_Init(void)
* ��    �ܣ��������Ź���ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void IWDG_Init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);      /* д��0x5555,�����������Ĵ���д�빦�� */
    IWDG_SetPrescaler(IWDG_Prescaler_256);      /* ����ʱ�ӷ�Ƶ,40K/256=156HZ(6.4ms)*/
    IWDG_SetReload(781);        /* ι��ʱ�� 5s/6.4MS=781 .ע�ⲻ�ܴ���0xfff*/
    IWDG_ReloadCounter();
    IWDG_Enable();
}


void USART1_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //����DMAʱ��

	DMA_DeInit(DMA1_Channel5);                                             // ����DMA1ͨ��5,����
      
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);    // �����ַUSART1
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_DMA_TBuf;      // �ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // ���赽�ڴ�
    DMA_InitStructure.DMA_BufferSize = MAX_DMA_BUFLEN;                      // ����DMA�ڴ泤�� 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       // �����ַ������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                // ����DMA���ڴ����ģʽ 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;// ���������ֳ�8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;        // �ڴ������ֳ�8λ 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						   //��������������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 				   //DMAͨ�� xӵ�������ȼ� 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                           // ��ֹM2M 
    DMA_Init(DMA1_Channel5,&DMA_InitStructure); 
    DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,DISABLE);//DMA���ղ������ж�
	DMA_Cmd(DMA1_Channel5,ENABLE);   
}
/****************************************************************************
* ��    �ƣ�void USART_Config(u32 baud)
* ��    �ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART1_Config(u32 baud)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
 
  	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE);	 	//ʹ�ܴ���1ʱ��

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 	//USART1 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//�����������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A�˿�  

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//���ÿ�©����
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A�˿� 

  	USART_InitStructure.USART_BaudRate = baud;						
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
  	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ 	

	/* Configure USART1 */
  	USART_Init(USART1, &USART_InitStructure);							//���ô��ڲ�������   
   	/* Enable the USART1 */	 
	/*USART1 receive data register is not empty */
// 	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_ITConfig(USART1, USART_IT_IDLE,ENABLE); //���������ж�
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);   //ʹ�ܴ���1 DMA����

  	USART_Cmd(USART1, ENABLE);	
}

/****************************************************************************
* ��    �ƣ�void USART2_Config(u32 baud)
* ��    �ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART2_Config(u32 baud)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;  	
  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //ʹ��UART2����GPIOA��ʱ�� 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  //ʹ�ܴ���1ʱ��	  

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         	//USART2 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		//�����������
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //c�˿� 

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         	 //USART2 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //���ÿ�©����
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 
  
  	USART_InitStructure.USART_BaudRate   = baud;			
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ8λ
  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;	//ֹͣλ1λ
  	USART_InitStructure.USART_Parity     = USART_Parity_No;		//��У��λ
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
  	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ	
  
  	/* Configure USART2 */
  	USART_Init(USART2, &USART_InitStructure);				//���ô��ڲ������� 
  
  	/*USART2 receive data register is not empty */
  	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  
  	/* Enable the USART2 */
  	USART_Cmd(USART2, ENABLE);	  
}


/****************************************************************************
* ��    �ƣ�void USART_Config(u32 baud)
* ��    �ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART3_Config(u32 baud)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;  
	
	
  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //ʹ��UART3����GPIOB��ʱ�� 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  //ʹ�ܴ���3ʱ��

//	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);  //��USART3�ֲ���ӳ�䵽PC10��PC11  

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	//USART3 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		//�����������
  	GPIO_Init(GPIOB, &GPIO_InitStructure);		    		 //c�˿� 

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         	 //USART3 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //���ÿ�©����
  	GPIO_Init(GPIOB, &GPIO_InitStructure);		         	 //c�˿� 
  
  	USART_InitStructure.USART_BaudRate   = baud;			
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ8λ
  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;	//ֹͣλ1λ
  	USART_InitStructure.USART_Parity     = USART_Parity_No;		//��У��λ
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
  	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ	
  
  	/* Configure USART3 */
  	USART_Init(USART3, &USART_InitStructure);				//���ô��ڲ������� 
  
  	/*USART3 receive data register is not empty */
  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
  	/* Enable the USART3 */
  	USART_Cmd(USART3, ENABLE);	  
}
/****************************************************************************
* ��    �ƣ�void USART_Config(u32 baud)
* ��    �ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART4_Config(u32 baud)
{
//  	GPIO_InitTypeDef GPIO_InitStructure;
//  	USART_InitTypeDef USART_InitStructure;  
//	
//	
//  	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);  //ʹ��UART4����GPIOC��ʱ�� 
//  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);  //ʹ�ܴ���4ʱ��
//
//	//GPIO_PinRemapConfig(GPIO_PartialRemap_UART4, ENABLE);  //��USART3�ֲ���ӳ�䵽PC10��PC11  
//
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	//USART4 TX
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		//�����������
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);		    		 //c�˿� 
//
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         	 //USART4 RX
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //���ÿ�©����
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);		         	 //c�˿� 
//  
//  	USART_InitStructure.USART_BaudRate   = baud;			
//  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ8λ
//  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;	//ֹͣλ1λ
//  	USART_InitStructure.USART_Parity     = USART_Parity_No;		//��У��λ
//  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
//  	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ	
//  
//  	/* Configure USART3 */
//  	USART_Init(UART4, &USART_InitStructure);				//���ô��ڲ������� 
//  
//  	/*USART3 receive data register is not empty */
//  	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  
//  	/* Enable the USART3 */
//  	USART_Cmd(UART4, ENABLE);	  
}
/****************************************************************************
* ��    �ƣ�void USART_Config(u32 baud)
* ��    �ܣ���������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART5_Config(u32 baud)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;  
	
	
  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);  //ʹ��UART5����GPIOC��ʱ�� 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);  //ʹ�ܴ���5ʱ��

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	         	//USART5 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		//�����������
  	GPIO_Init(GPIOC, &GPIO_InitStructure);		    		 //c�˿� 

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         	 //USART5 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //���ÿ�©����
  	GPIO_Init(GPIOD, &GPIO_InitStructure);		         	 //D�˿� 
  
  	USART_InitStructure.USART_BaudRate   = baud;			
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ8λ
  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;	//ֹͣλ1λ
  	USART_InitStructure.USART_Parity     = USART_Parity_No;		//��У��λ
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������
  	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ	
  
  	/* Configure USART3 */
  	USART_Init(UART5, &USART_InitStructure);				//���ô��ڲ������� 
  
  	/*USART3 receive data register is not empty */
  	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);  
  	/* Enable the USART3 */
  	USART_Cmd(UART5, ENABLE);	  
}

/****************************************************************************
* ��    �ƣ�void SPI2_Init(void)
* ��    �ܣ�SPI2��ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void SPI2_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* ʹ��SPI2ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //APB1����Ƶ��36MHZ,APB2����Ƶ��72MHZ
	
	/* ����SPI2��SCK,MISO MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //���ù���
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* ����SPI2��ENC28J60Ƭѡ */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_12);   

	/* SPI2���� */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	
	/* ʹ��SPI2  */
	SPI_Cmd(SPI2, ENABLE); 
}


/*******************************************************************************
 * �������ƣ�ADC1_Init()
 * �������ܣ�AD�˿ڳ�ʼ����
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
void ADC1_Init(void)  
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef  ADC_InitStructure;

	 /*ʹ�ܸ�������ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 ,ENABLE ); //ʹ��ADC1ͨ��ʱ�ӣ������ܽ�ʱ�� 

	 //PA 4/5 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);



	ADC_DeInit(ADC1);  //������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode =ENABLE;	        //ģ��ת��������ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ģ��ת������������ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//�ⲿ����ת���ر�
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = ADC_CH_NUM;	      //˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���
	
	/* ADC1 regular channel11 configuration */ 
	//����ָ��ADC�Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	//ADC1,ADCͨ��x,�������˳��ֵΪy,����ʱ��Ϊ239.5����

		
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  1, ADC_SampleTime_239Cycles5 );//�����¶�
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  2, ADC_SampleTime_239Cycles5 );//���빦��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_239Cycles5 );//ĩ������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_239Cycles5 );//���书��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_239Cycles5 );//���书��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_239Cycles5 );//�����ѹ
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 7, ADC_SampleTime_239Cycles5);  //TM3 CPU�¶�
	
	 ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����
	 // ����ADC��DMA֧�֣�Ҫʵ��DMA���ܣ������������DMAͨ���Ȳ�����
    ADC_DMACmd(ADC1, ENABLE);	
	
	/* Enable ADC1 */
	 ADC_Cmd(ADC1, ENABLE);	   //ʹ��ָ����ADC1
	 /* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	  //��λָ����ADC1��У׼�Ĵ���
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));	//��ȡADC1��λУ׼�Ĵ�����״̬,����״̬��ȴ�
	
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		//��ʼָ��ADC1��У׼״̬
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));		//��ȡָ��ADC1��У׼����,����״̬��ȴ�	   

}	

/*******************************************************************************
 * �������ƣ�DMA1_Init()
 * �������ܣ�AD�˿ڳ�ʼ����
 * ��ڲ�������
 * ���ڲ�������
 *******************************************************************************/
void DMA1_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	/* ADC1  DMA1 Channel Config */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		//ʹ��DMA���� 

	DMA_DeInit(DMA1_Channel1);      //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //�ڴ���Ϊ���ݴ����Ŀ�ĵ�
	DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��	
	//DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);	 //����DMAͨ�� 	
	
}	

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void I2C1_Configuration(void)
{
   I2C_InitTypeDef  I2C_InitStructure;
   GPIO_InitTypeDef  GPIO_InitStructure; 

   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
   GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

   /* Configure I2C1 pins: PB8->SCL and PB9->SDA */
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
		
   I2C_DeInit(I2C1);
   I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
   I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
   I2C_InitStructure.I2C_OwnAddress1 = 0x30;
   I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
   I2C_InitStructure.I2C_ClockSpeed = 100000;
    
   I2C_Cmd(I2C1, ENABLE);
   I2C_Init(I2C1, &I2C_InitStructure);

   I2C_AcknowledgeConfig(I2C1, ENABLE);
}
/*******************************************************************************
*	��������InitNet
*	��  ��:
*	��  ��:
*	����˵������ʼ������Ӳ����UIPЭ��ջ�����ñ���IP��ַ
************************************************************/
void InitNet(void)
{	
	u8 i = 0;
	u32 temp;
  
	//��ʼ��MAC��ַ
	temp=*(vu32*)(0x1FFFF7E8);	//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
	g_DI.MAC[0]=0x44;
	g_DI.MAC[1]=0x5a;
	g_DI.MAC[2]=0;
	g_DI.MAC[3]=(temp>>16)&0xFF;	//�����ֽ���STM32��ΨһID
	g_DI.MAC[4]=(temp>>8)&0xFFF;
	g_DI.MAC[5]=temp&0xFF;

}
//��ȡCPUΨһID
void Get_ChipID(void)
{	
	//��ַ��С����,�ȷŵ��ֽڣ��ٷŸ��ֽڣ�С��ģʽ
	//��ַ��С����,�ȷŸ��ֽڣ��ٷŵ��ֽڣ����ģʽ	 
	CpuID[0]=*(vu32*)(0x1ffff7e8);	// ���ֽ�
	CpuID[1]=*(vu32*)(0x1ffff7ec);
	CpuID[2]=*(vu32*)(0x1ffff7f0);	// ���ֽ�
//	printf(" CpuID[0]: 0x%x \r\n",CpuID[0]);
//	printf(" CpuID[1]: 0x%x \r\n",CpuID[1]);
//	printf(" CpuID[2]: 0x%x \r\n",CpuID[2]);
}







