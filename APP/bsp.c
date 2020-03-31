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
 * 名    称：void BSP_Init(void)
 * 功    能：奋斗板初始化函数
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 *******************************************************************************/  
void BSP_Init(void)
{ 	
//	u8 i = 0;
	u8 readbuf[10];
	Ini_SysRam(); 

  	RCC_Configuration();  	       //系统时钟初始化	
  		
	NVIC_Configuration();		   //中断源配置  
   
  	GPIO_Configuration();		   //GPIO配置
	DS18B20_Init();
  	USART1_Config(115200);          //初始化串口1 PC
	USART1_DMA_Config();
	USART2_Config(115200);          //初始化串口2 
//    USART3_Config(115200);          //初始化串口3 调制板串口RS232通信 
    USART5_Config(19200);          //初始化串口5 RS485通信 功放串口RS232通信 
	FSMC_SRAM_Init();		//初始化外部SRAM	
//	my_mem_init(SRAMEX);		//初始化内部内存池
	my_mem_init(SRAMEX);		//初始化外部内存池
	
	SPI1_Init();				   //X5323 SPI 初始化   
	SPI2_Init();                   //ENC28J60 SPI 初始化 
	ADC1_Init();
	DMA1_Init();
		
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //启动ADC	
	Get_ChipID();//获取CPU唯一ID
	 /* 检测FLASH中是否有系统标志,若无,存入系统默认配置数据*/	  	
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
	/* 初始化网络设备以及UIP协议栈，配置IP地址 */ 
	printf("	InitNet ! \r\n");//lwip3?ê??ˉ3é1|
	InitNet();	
#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
#endif
   while(lwip_comm_init()) 
	{

		printf("	LWIP Init Falied! \r\n");
	}
	printf("	SRAMEX USED: %d \r\n",my_mem_perused(SRAMEX));
	httpd_init(); 	 		 

	

#if IWDG_EN
	IWDG_ReloadCounter(); //手动喂狗  
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
	SM1668_DisplayLED(0xC0,Led_Display);//PWR指示灯亮
	 
	printf("bsp web ok 20200331...........\r\n"); 


} 				 
   
   

/*******************************************************************************
 * 名    称：void  OS_CPU_SysTickInit(void)
 * 功    能：ucos 系统节拍时钟初始化  初始设置为10ms一次节拍
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;
    RCC_GetClocksFreq(&rcc_clocks);		                        //获得系统时钟的值	 
    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;	//算出时钟节拍的值	
	SysTick_Config(cnts);										//设置时钟节拍	     
}



/* 
 *******************************************************************************
 *
 * private routines
 * 
 *******************************************************************************/
/*******************************************************************************
 * 函数名称：Ini_SysRam()
 * 函数功能：初始化系统变量
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
void Ini_SysRam(void)
{
    
   *(u32 *)&Sys_Flag = 0x00000000;			// 系统标志清零	
   *(u32 *)&SysNet_Flag = 0x00000000;			// 系统标志清零	
	//初始化 串口接收结构体
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
	g_DI.ALL.Data.Input_SMA_Flag = 0x00;// 联锁开关标志位，0x00表示无效

	fRS485 = FALSE;
	
	fPaRTU = FALSE;
	fEitRTU = FALSE;
	flag_TLink[0] = 1;	// 功放通信状态
	flag_TLink[1] = 1;	// 功放通信状态
	flag_TLink[2] = 1;	// 功放通信状态
	Sys_Flag.Bit.fPaON = 0;
	Sys_Flag.Bit.fPaOFF = 0;
	Sys_Flag.Bit.fPaCheck = FALSE;
	Sys_Flag.Bit.fPanetQue = FALSE;
	Sys_Flag.Bit.fExcitCheck = FALSE;
	Sys_Flag.Bit.fAutoMsgSave = FALSE;
	Sys_Flag.Bit.fSaveStart = FALSE; 
	g_DI.ALL.Data.Input_SMA_ALM = 0x00;	//联锁无告警
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

	g_DI.Pa[0].Data.TEMP_Status = 0;	  //温度状态
	g_DI.Pa[0].Data.TEMP_StatusTrig = 0;	  //温度状态触发
	g_DI.Pa[0].Data.TEMP_StatusCont = 0;	  //温度状态持续
	g_DI.Pa[0].Data.TEMP_StatusRelease = 0;	  //温度状态释放

	g_DI.Pa[0].Data.RF_Status = 0;
	g_DI.Pa[0].Data.RF_StatusTrig = 0;
	g_DI.Pa[0].Data.RF_StatusCont = 0;
	
	g_DI.Pa[0].Data.Temp_ReducePowerCount  = 0;   //温度保护已降低功率次数 
	g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;	//已过流保护重启计数值
	g_DI.Pa[0].Data.Protect_Shutdown_Cause = 0;
	g_DI.Pa[0].Data.P_DownOffset = 0;
	g_DI.Pa[0].Data.RES_Front = 0;
	g_DI.Pa[0].Data.RES_Rear = 0;
	/*功率控制延时检测定时器*/
	User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 0;
	User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
	User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count =30; //2秒
	User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
	/*功率自动控制定时器*/
	User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
	User_Timer[POWERAUTOTIMER].Timer_Enable = 1;
	User_Timer[POWERAUTOTIMER].Timer_Count = 0 ;
	User_Timer[POWERAUTOTIMER].Timer_Out_Count = 50;
	
	User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
	User_Timer[OVERLOADRESTARTTIMER].Timer_Enable = 0;
	User_Timer[OVERLOADRESTARTTIMER].Timer_Count =0 ;
	User_Timer[OVERLOADRESTARTTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;//过流重启时间;
}


/*******************************************************************************
 * 函数名称：Write_Default()
 * 函数功能：向FLASH写入默认数据。
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
void Write_Default(u8 partype)
{	
	u8 i = 0;
	g_DI.Cfg.ICON  = cICON;
	g_DI.Cfg.FunID = cFunID;
	g_DI.Cfg.ID    = cID;
	

	/* 初始化IP地址 */	
	memcpy(g_DI.Cfg.IP_Server, (u8 *)&cIP_Server, 4);
	memcpy(g_DI.Cfg.IP_Addr, (u8 *)&cIP_Addr, 4);
	memcpy(g_DI.Cfg.IP_Mask, (u8 *)&cIP_Mask, 4);
	memcpy(g_DI.Cfg.IP_GateWay, (u8 *)&cIP_GateWay, 4);
	memcpy(g_DI.Cfg.IP_DNS, (u8 *)&cIP_DNS, 4);	  
	g_DI.Cfg.Net_Port = cNet_Port;	 
	    
	g_DI.Cfg.PA_IPHead = 0x25;
	g_DI.Cfg.Excit_IPHead = 0x33;		 

	g_DI.Cfg.CloseFan_TPTlimit = 45;//关闭风扇温度
	g_DI.Cfg.OpenFan1_TPTlimit = 48;//开启风扇1温度
	g_DI.Cfg.OpenFan2_TPTlimit = 53;//开启风扇2温度
	g_DI.Cfg.PWRRED_TPTlimit = 65;//降功率温度
	g_DI.Cfg.PWROFF_TPTlimit = 75;//过温关机保护温度
	g_DI.Cfg.PWR_RVlimit = 5;//反射保护门限
	g_DI.Cfg.PWRRED_CURlimit = 1.6;//降功率电流
	g_DI.Cfg.PWROFF_CURlimit = 1.8;//过保护电流
	g_DI.Cfg.CurRstNum = 5;  
	g_DI.Cfg.CurRstDelay = 100;
	g_DI.Cfg.PWR_INlimit = -40.0;//输入最小功率
	g_DI.Cfg.PWR_RateOut = 30;
	 
	/***********************新添加**************************/
	g_DI.Cfg.P_FmFrq = 87.0;					  		//输出频率值
	g_DI.Cfg.P_InCal = 0.465;						  //输入功率校正值
	g_DI.Cfg.P_OutCal= 3600.867;						 //输出功率校正系数
	g_DI.Cfg.P_RefCal = 1665.186;						  //反射功率校正系数
	g_DI.Cfg.P_AutoContrl = 1;
	g_DI.Cfg.P_Res_Front_Target = 50;
	g_DI.Cfg.PWR_SetRateOut = 30;
	
	g_DI.Cfg.Temp_ReducePowerInterval = 180;//  = 30 ;      //降功率间隔时间 秒
	g_DI.Cfg.Temp_ReducePowerMaxCount = 3;// = 3;	        //降低功率最大次数
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


	g_DI.Cfg.LOCAL1NetPort1 = g_DI.Cfg.IP_LOCAL1[3]+2000;		// 本地管理1-端口号
	g_DI.Cfg.LOCAL1NetPort2 = g_DI.Cfg.IP_LOCAL2[3]+2000;		// 本地管理2-端口号

	g_DI.Cfg.INSERVERNetPort1 = 0;		// 内部联动1-端口号
	g_DI.Cfg.INSERVERNetPort2 = 0;		// 内部联动2-端口号


	g_DI.Cfg.RemoteHostNetPort1 = 6666;	// 远程主机1-端口号
	g_DI.Cfg.RemoteHostDelay1_1 = 2;		// 远程主机1-第 1 组主动间隔
	g_DI.Cfg.RemoteHostDelay1_2 = 15;		// 远程主机1-第 2 组主动间隔
	g_DI.Cfg.RemoteHostDelay1_3 = 0;		// 远程主机1-第 3 组主动间隔


	g_DI.Cfg.RemoteHostNetPort2 = 0;		// 远程主机2-端口号
	g_DI.Cfg.RemoteHostDelay2_1 = 0;		// 远程主机2-第 1 组主动间隔
	g_DI.Cfg.RemoteHostDelay2_2 = 0;		// 远程主机2-第 2 组主动间隔
	g_DI.Cfg.RemoteHostDelay2_3 = 0;		// 远程主机2-第 3 组主动间隔

    
	g_DI.Cfg.RemoteHostNetPort3 = 0;		// 远程主机3-端口号
	g_DI.Cfg.RemoteHostDelay3_1 = 0;		// 远程主机3-第 1 组主动间隔
	g_DI.Cfg.RemoteHostDelay3_2 = 0;		// 远程主机3-第 2 组主动间隔
	g_DI.Cfg.RemoteHostDelay3_3 = 0;		// 远程主机3-第 3 组主动间隔
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

	//报文相关配置
	g_DI.AutoMsgCfg.AutoMsgICON  = cAutoMsgICON;
	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_Cfg1[i] = cAutoMsg_Cfg1[i];	  //第1组自动报文设置,
	}
	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_Cfg2[i] = cAutoMsg_Cfg2[i];	  //第2组自动报文设置
	}
	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_Cfg3[i] = cAutoMsg_Cfg3[i];	//第3组自动报文设置
	}

	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_INCfg1[i] = cAutoMsg_INCfg1[i];	//内部联动报文1设置
	}

	for(i=0; i<50; i++)
	{
		g_DI.AutoMsgCfg.AutoMsg_INCfg2[i] = cAutoMsg_INCfg2[i];	//内部联动报文2设置
	}   
	g_DI.AutoMsgCfg.AutoMsg_Cfg1_Len = 10;
	g_DI.AutoMsgCfg.AutoMsg_Cfg2_Len = 22;
	g_DI.AutoMsgCfg.AutoMsg_Cfg3_Len = 12;	
	g_DI.AutoMsgCfg.AutoMsg_INCfg1_Len = 0;
	g_DI.AutoMsgCfg.AutoMsg_INCfg2_Len = 0;

	WritebufToX5323(1300,AutoMsgSave_Size+50,(u8*)&g_DI.AutoMsgCfg);		 
}
	 
/*******************************************************************************
 * 名    称：void RCC_Configuration(void)
 * 功    能：系统时钟配置为72MHZ， 外设时钟配置
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
 *******************************************************************************/ 
void RCC_Configuration(void){

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//复用功能使能
}

/*******************************************************************************
 * 名    称：void GPIO_Configuration(void)
 * 功    能：通用IO口配置
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：
 *******************************************************************************/  
void GPIO_Configuration(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_GPIOE |RCC_APB2Periph_GPIOF |
						   RCC_APB2Periph_GPIOG, ENABLE);	
						     	
  	  
	/* 输入配置*/
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;	         	 	//ENC28J60接收完成中断引脚 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//内部上拉输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;	         	 	//栅压 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;   	 		//内部下拉输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	 
	/* 输出配置 */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	 

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_11 | GPIO_Pin_12 ; // LEATT/CLK/STB	
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7 ; // SDIN	
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;//开漏输出
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
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;//开漏输出
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 	
	
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_9| GPIO_Pin_10 | GPIO_Pin_11; // CS/IO/RS
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_6| GPIO_Pin_7; // FAN2_CT/FAN1_CT
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; //I_ALM/SWR_ALM/输入灯/工作灯 
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
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;            // 开漏输出     
//    GPIO_Init(GPIOC, &GPIO_InitStructure);			   
}

/*******************************************************************************
 * 名    称：void NVIC_Configuration(void)
 * 功    能：中断源配置
 * 入口参数：无
 * 出口参数：无
 * 说    明：
 * 调用方法：无 
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
  	/* 优先级组 说明了抢占优先级所用的位数，和子优先级所用的位数   在这里是1， 7 */    
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
  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  	     //外部中断2
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 //抢占优先级 0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			 //子优先级0  
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //使能
  	NVIC_Init(&NVIC_InitStructure);

  	//用于配置AFIO外部中断配置寄存器AFIO_EXTICR1，用于选择EXTI2外部中断的输入源是PE2。
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);     //外部中断配置AFIO--ETXI2

  	EXTI_InitStructure.EXTI_Line = EXTI_Line8;					  
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //中断模式
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

	//用于配置外部中断，输入源是PC6。
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);     //外部中断配置PC6

  	EXTI_InitStructure.EXTI_Line = EXTI_Line6;					  
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //中断模式
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;		  //下降沿触发	
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
}


/****************************************************************************
* 名    称：void NVIC_Configuration(void)
* 功    能：中断源配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
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
* 名    称：void IWDG_Init(void)
* 功    能：独立看门狗初始化
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void IWDG_Init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);      /* 写入0x5555,用于允许狗狗寄存器写入功能 */
    IWDG_SetPrescaler(IWDG_Prescaler_256);      /* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
    IWDG_SetReload(781);        /* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
    IWDG_ReloadCounter();
    IWDG_Enable();
}


void USART1_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //开启DMA时钟

	DMA_DeInit(DMA1_Channel5);                                             // 配置DMA1通道5,接收
      
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);    // 外设地址USART1
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_DMA_TBuf;      // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     // 外设到内存
    DMA_InitStructure.DMA_BufferSize = MAX_DMA_BUFLEN;                      // 设置DMA内存长度 
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;       // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                // 设置DMA的内存递增模式 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;// 外设数据字长8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;        // 内存数据字长8位 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						   //工作在正常缓存模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 				   //DMA通道 x拥有中优先级 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                           // 禁止M2M 
    DMA_Init(DMA1_Channel5,&DMA_InitStructure); 
    DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,DISABLE);//DMA接收不产生中断
	DMA_Cmd(DMA1_Channel5,ENABLE);   
}
/****************************************************************************
* 名    称：void USART_Config(u32 baud)
* 功    能：串口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART1_Config(u32 baud)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
 
  	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE);	 	//使能串口1时钟

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 	//USART1 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//复用推挽输出
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A端口  

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//复用开漏输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A端口 

  	USART_InitStructure.USART_BaudRate = baud;						
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
  	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式 	

	/* Configure USART1 */
  	USART_Init(USART1, &USART_InitStructure);							//配置串口参数函数   
   	/* Enable the USART1 */	 
	/*USART1 receive data register is not empty */
// 	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_ITConfig(USART1, USART_IT_IDLE,ENABLE); //开启空闲中断
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);   //使能串口1 DMA接收

  	USART_Cmd(USART1, ENABLE);	
}

/****************************************************************************
* 名    称：void USART2_Config(u32 baud)
* 功    能：串口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART2_Config(u32 baud)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;  	
  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能UART2所在GPIOA的时钟 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  //使能串口1时钟	  

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         	//USART2 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		//复用推挽输出
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 //c端口 

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         	 //USART2 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //复用开漏输入
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 
  
  	USART_InitStructure.USART_BaudRate   = baud;			
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位8位
  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;	//停止位1位
  	USART_InitStructure.USART_Parity     = USART_Parity_No;		//无校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
  	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//收发模式	
  
  	/* Configure USART2 */
  	USART_Init(USART2, &USART_InitStructure);				//配置串口参数函数 
  
  	/*USART2 receive data register is not empty */
  	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  
  	/* Enable the USART2 */
  	USART_Cmd(USART2, ENABLE);	  
}


/****************************************************************************
* 名    称：void USART_Config(u32 baud)
* 功    能：串口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART3_Config(u32 baud)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;  
	
	
  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //使能UART3所在GPIOB的时钟 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  //使能串口3时钟

//	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);  //将USART3局部重映射到PC10，PC11  

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	//USART3 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		//复用推挽输出
  	GPIO_Init(GPIOB, &GPIO_InitStructure);		    		 //c端口 

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         	 //USART3 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //复用开漏输入
  	GPIO_Init(GPIOB, &GPIO_InitStructure);		         	 //c端口 
  
  	USART_InitStructure.USART_BaudRate   = baud;			
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位8位
  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;	//停止位1位
  	USART_InitStructure.USART_Parity     = USART_Parity_No;		//无校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
  	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//收发模式	
  
  	/* Configure USART3 */
  	USART_Init(USART3, &USART_InitStructure);				//配置串口参数函数 
  
  	/*USART3 receive data register is not empty */
  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  
  	/* Enable the USART3 */
  	USART_Cmd(USART3, ENABLE);	  
}
/****************************************************************************
* 名    称：void USART_Config(u32 baud)
* 功    能：串口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART4_Config(u32 baud)
{
//  	GPIO_InitTypeDef GPIO_InitStructure;
//  	USART_InitTypeDef USART_InitStructure;  
//	
//	
//  	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);  //使能UART4所在GPIOC的时钟 
//  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);  //使能串口4时钟
//
//	//GPIO_PinRemapConfig(GPIO_PartialRemap_UART4, ENABLE);  //将USART3局部重映射到PC10，PC11  
//
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	//USART4 TX
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		//复用推挽输出
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);		    		 //c端口 
//
//  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         	 //USART4 RX
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //复用开漏输入
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);		         	 //c端口 
//  
//  	USART_InitStructure.USART_BaudRate   = baud;			
//  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位8位
//  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;	//停止位1位
//  	USART_InitStructure.USART_Parity     = USART_Parity_No;		//无校验位
//  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
//  	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//收发模式	
//  
//  	/* Configure USART3 */
//  	USART_Init(UART4, &USART_InitStructure);				//配置串口参数函数 
//  
//  	/*USART3 receive data register is not empty */
//  	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);  
//  	/* Enable the USART3 */
//  	USART_Cmd(UART4, ENABLE);	  
}
/****************************************************************************
* 名    称：void USART_Config(u32 baud)
* 功    能：串口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void USART5_Config(u32 baud)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;  
	
	
  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);  //使能UART5所在GPIOC的时钟 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);  //使能串口5时钟

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	         	//USART5 TX
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		//复用推挽输出
  	GPIO_Init(GPIOC, &GPIO_InitStructure);		    		 //c端口 

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         	 //USART5 RX
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //复用开漏输入
  	GPIO_Init(GPIOD, &GPIO_InitStructure);		         	 //D端口 
  
  	USART_InitStructure.USART_BaudRate   = baud;			
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位8位
  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;	//停止位1位
  	USART_InitStructure.USART_Parity     = USART_Parity_No;		//无校验位
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控
  	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	//收发模式	
  
  	/* Configure USART3 */
  	USART_Init(UART5, &USART_InitStructure);				//配置串口参数函数 
  
  	/*USART3 receive data register is not empty */
  	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);  
  	/* Enable the USART3 */
  	USART_Cmd(UART5, ENABLE);	  
}

/****************************************************************************
* 名    称：void SPI2_Init(void)
* 功    能：SPI2初始化
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/ 
void SPI2_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 使能SPI2时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); //APB1工作频率36MHZ,APB2工作频率72MHZ
	
	/* 配置SPI2的SCK,MISO MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //复用功能
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* 配置SPI2的ENC28J60片选 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  	
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_12);   

	/* SPI2配置 */ 
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
	
	/* 使能SPI2  */
	SPI_Cmd(SPI2, ENABLE); 
}


/*******************************************************************************
 * 函数名称：ADC1_Init()
 * 函数功能：AD端口初始化。
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
void ADC1_Init(void)  
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef  ADC_InitStructure;

	 /*使能各个外设时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 ,ENABLE ); //使能ADC1通道时钟，各个管脚时钟 

	 //PA 4/5 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);



	ADC_DeInit(ADC1);  //将外设 ADC1 的全部寄存器重设为缺省值
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode =ENABLE;	        //模数转换工作在扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//模数转换工作在连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//外部触发转换关闭
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = ADC_CH_NUM;	      //顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
	
	/* ADC1 regular channel11 configuration */ 
	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	//ADC1,ADC通道x,规则采样顺序值为y,采样时间为239.5周期

		
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  1, ADC_SampleTime_239Cycles5 );//环境温度
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  2, ADC_SampleTime_239Cycles5 );//输入功率
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 3, ADC_SampleTime_239Cycles5 );//末级电流
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 4, ADC_SampleTime_239Cycles5 );//入射功率
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 5, ADC_SampleTime_239Cycles5 );//发射功率
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 6, ADC_SampleTime_239Cycles5 );//输入电压
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 7, ADC_SampleTime_239Cycles5);  //TM3 CPU温度
	
	 ADC_TempSensorVrefintCmd(ENABLE); //开启内部温度传感器
	 // 开启ADC的DMA支持（要实现DMA功能，还需独立配置DMA通道等参数）
    ADC_DMACmd(ADC1, ENABLE);	
	
	/* Enable ADC1 */
	 ADC_Cmd(ADC1, ENABLE);	   //使能指定的ADC1
	 /* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	  //复位指定的ADC1的校准寄存器
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));	//获取ADC1复位校准寄存器的状态,设置状态则等待
	
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		//开始指定ADC1的校准状态
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));		//获取指定ADC1的校准程序,设置状态则等待	   

}	

/*******************************************************************************
 * 函数名称：DMA1_Init()
 * 函数功能：AD端口初始化。
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
void DMA1_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	/* ADC1  DMA1 Channel Config */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		//使能DMA传输 

	DMA_DeInit(DMA1_Channel1);      //将DMA的通道1寄存器重设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA外设ADC基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_Value;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //内存作为数据传输的目的地
	DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //工作在循环缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道	
	//DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);	 //启动DMA通道 	
	
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
*	函数名：InitNet
*	输  入:
*	输  出:
*	功能说明：初始化网络硬件、UIP协议栈、配置本机IP地址
************************************************************/
void InitNet(void)
{	
	u8 i = 0;
	u32 temp;
  
	//初始化MAC地址
	temp=*(vu32*)(0x1FFFF7E8);	//获取STM32的唯一ID的前24位作为MAC地址后三字节
	g_DI.MAC[0]=0x44;
	g_DI.MAC[1]=0x5a;
	g_DI.MAC[2]=0;
	g_DI.MAC[3]=(temp>>16)&0xFF;	//低三字节用STM32的唯一ID
	g_DI.MAC[4]=(temp>>8)&0xFFF;
	g_DI.MAC[5]=temp&0xFF;

}
//获取CPU唯一ID
void Get_ChipID(void)
{	
	//地址从小到大,先放低字节，再放高字节：小端模式
	//地址从小到大,先放高字节，再放低字节：大端模式	 
	CpuID[0]=*(vu32*)(0x1ffff7e8);	// 低字节
	CpuID[1]=*(vu32*)(0x1ffff7ec);
	CpuID[2]=*(vu32*)(0x1ffff7f0);	// 高字节
//	printf(" CpuID[0]: 0x%x \r\n",CpuID[0]);
//	printf(" CpuID[1]: 0x%x \r\n",CpuID[1]);
//	printf(" CpuID[2]: 0x%x \r\n",CpuID[2]);
}







