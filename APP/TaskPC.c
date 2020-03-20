/*******************************************************************************
 *
 * Filename  : TaskPC.c
 *
 * Function  : Implememt TASKPC function form for this workbench.
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
#include "global.h"	
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "..\Private\iap.h"
#include "..\Private\ConstValue.h"



/*
 *******************************************************************************
 *
 * private defines
 *
 *******************************************************************************
 */




  
  		
/* 
 *******************************************************************************
 *
 * private routines prototype
 * 
 *******************************************************************************
 */ 
 u8 Pack_PT(u8 *Buf, u8 Len, u16 timeout);
 bool Pa_Wait_RxEnd(u16 timeout);
void USART_LoginReply(u8 *PackBuf, u16 PackLen,char *outdata,u16 *outlen);



/*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */
 /*
 ******************************************************************************
 * 函数名称：TaskPC()
 * 函数功能：PC调试终端任务处理
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
  void USART_AutoSend(char *outdata,u16 *outlen)
{
	u8 cmd,devaddr;  
	u16 crctmp;		
	(*outlen) = 0;

	outdata[(*outlen)++] = 0x65;
	outdata[(*outlen)++] = 0x00;
	outdata[(*outlen)++] = 0x00;
 
	(*outlen) += CMD_GetParmaData(INFO_AllState_cWork,&outdata[(*outlen)]); //整机工作状态
	(*outlen) += CMD_GetParmaData(INFO_AllState_Det,&outdata[(*outlen)]); //整机检测状态
	(*outlen) += CMD_GetParmaData(INFO_AllState_Pro,&outdata[(*outlen)]); //整机保护状态
	(*outlen) += CMD_GetParmaData(INFO_PWR_RFOTU,&outdata[(*outlen)]); //发射机输出功率,W
	(*outlen) += CMD_GetParmaData(INFO_PWR_RV,&outdata[(*outlen)]); // 发射机反射功率,W
	(*outlen) += CMD_GetParmaData(INFO_ALL_VOL,&outdata[(*outlen)]); //发射机功放电压,V
	(*outlen) += CMD_GetParmaData(INFO_ALL_CUR,&outdata[(*outlen)]); //发射机功放电流,A
	(*outlen) += CMD_GetParmaData(INFO_TPT_CPU,&outdata[(*outlen)]); //CPU温度,℃
	(*outlen) += CMD_GetParmaData(INFO_PATPT,&outdata[(*outlen)]); //功放温度,℃  
	(*outlen) += CMD_GetParmaData(INFO_AllPWR_Int,&outdata[(*outlen)]); //功率初始化,W 
			 			
	
	(*outlen) += CMD_GetParmaData(INFO_ExciterOut_FrqMHZ,&outdata[(*outlen)]); //激励器输出频率,MHz
	(*outlen) += CMD_GetParmaData(INFO_Exciter_ModMode,&outdata[(*outlen)]); //调制模式，0:关闭 1:立体声 2:L单声道 3:R单声道 4:L+R单声道	
	(*outlen) += CMD_GetParmaData(INFO_Exciter_PreEmp,&outdata[(*outlen)]); //预加重 0:0us 1:50us 2:75us
	(*outlen) += CMD_GetParmaData(INFO_ExciterPWR_Gain,&outdata[(*outlen)]); //功率增益
	(*outlen) += CMD_GetParmaData(INFO_ExciterMODULDCT,&outdata[(*outlen)]); //调制度监测 float
	(*outlen) += CMD_GetParmaData(INFO_ExciterRDSVAL,&outdata[(*outlen)]); //RDS幅度
	(*outlen) += CMD_GetParmaData(INFO_ExciterDIFMODULDCT,&outdata[(*outlen)]); //差项调制度 %
	(*outlen) += CMD_GetParmaData(INFO_ExciterL_Mod,&outdata[(*outlen)]); //L调制度,%
	(*outlen) += CMD_GetParmaData(INFO_ExciterR_Mod,&outdata[(*outlen)]); //R调制度,%
	
	outdata[1] = ((*outlen)-3)%256;
	outdata[2] = ((*outlen)-3)/256;
	crctmp = CRC16_GX_MODBUS( (u8 *)outdata,(*outlen));
	outdata[(*outlen)++] = crctmp %256;
	outdata[(*outlen)++] = crctmp /256;
}
void MYDMA_Enable(void)
{ 
	DMA_Cmd(DMA1_Channel5, DISABLE ); //关闭USART1 TX DMA1 所指示的通道 
	DMA_SetCurrDataCounter(DMA1_Channel5,MAX_UART_BUF_LEN);//从新设置缓冲大小,指向数组0
	DMA_Cmd(DMA1_Channel5, ENABLE); //使能USART1 TX DMA1 所指示的通道 
}  
 
u8 ParCRCDEAL(u8 *dat, u16 PackLen)	
{
	u16 CRC_Value;
	u8 crcL,crcH;
	
	crcL = dat[PackLen - 2];
	crcH = dat[PackLen - 1];

	CRC_Value = CRC16_GX_MODBUS((u8 *)dat, PackLen-2);			// 计算CRC
	
										 
	if ((crcL != (u8)(CRC_Value&0xFF)) || (crcH != (u8)((CRC_Value>>8)&0xFF)))  // CRC校验判断
	{	   					
		return 0;
	}
	return 1;	
	
}
int My_Strlen(u8 *dat)
{
	u16 i=0,j=0,len=0;
	u16 CRC_Value;
	for(i=0;i<250;i++)
	{
		len = 0;
		if((dat[i] == g_DI.Cfg.DeviceID)&&((dat[i+1] == CMD_READ) || (dat[i+1] == CMD_WRITE)))
		{
			for(j=0;j<(250-i);j++)
			{
				if(ParCRCDEAL(&dat[i], i+j+4) == 1)
				{
					UART1_RBuf.Len = len +4;
					return i;			
				}
				else
				{
					len++;
					
				}
			}
		}
//		else if((dat[i] == 0x00)&&(dat[i+1] == CMD_SEARCH))
		else if(dat[i+1] == CMD_SEARCH)
		{
			for(j=0;j<(250-i);j++)
			{
				if(ParCRCDEAL(&dat[i], i+j+4) == 1)
				{
					UART1_RBuf.Len = len +4;
					return i;			
				}
				else
				{
					len++;
					
				}
			}
		}
		else if((dat[i+1] == 0x35)&&(dat[i+2] == 0xC3)&&(dat[i+3] == 0xFC)&&(dat[i+4] == 0xC1))
		{
			for(j=0;j<(250-i);j++)
			{
				if(ParCRCDEAL(&dat[i], i+j+2) == 1)
				{
					UART1_RBuf.Len = len +2;
					return i;			
				}
				else
				{
					len++;
					
				}
			}
		}
		
	}
	return -1;
}
/*
 ******************************************************************************
 * 函数名称：TaskPC()
 * 函数功能：PC调试终端任务处理
 * 入口参数：无
 * 出口参数：无
 *******************************************************************************/
 void  TaskPC(void *pdata)
{

	u8 len = 0;
   	static int AutoSendCnt = 0;
	u8 RxDat[300];
	u8 RxDatLen = 0;
	
	OS_CPU_SR  cpu_sr;
	
	pdata = pdata ;	
	
	while(1)			   
	{  	
  		
		/* PC数据处理 */
		if( UART1_RBuf.fEnd == TRUE )								
		{					

			len = PC_Protocol((u8*)&UART1_RBuf.Buff[0], UART1_RBuf.Len, MOC_TYPE);
				
			UART1Write(&g_UART_TBuf[0], len); 			
			UART1_RBuf.fEnd = FALSE ;
			UART1_RBuf.Len = 0 ;			
						
		} 
		if( UART2_RBuf.fEnd == TRUE )								
		{					

			len = PC_Protocol((u8*)&UART2_RBuf.Buff[0], UART2_RBuf.Len, MOC_TYPE);
				
			UART2Write(&g_UART_TBuf[0], len); 			
			UART2_RBuf.fEnd = FALSE ;
			UART2_RBuf.Len = 0 ;
				
		}
		
		if(Sys_Flag.Bit.fSaveStart == TRUE)
		{
			Sys_Flag.Bit.fSaveStart = FALSE;

			if (Sys_Flag.Bit.fSave == TRUE)
			{  			
#if IWDG_EN
		IWDG_ReloadCounter(); //手动喂狗  
#endif
				Sys_Flag.Bit.fSave = FALSE;
				//printf("Sys_Flag.Bit.fSave: %d \r\n",Sys_Flag.Bit.fSave);
				//printf("g_DI.Cfg WritebufToX5323 CurTimes:%d .....\r\n",CurTimes);
				WritebufToX5323(10,Save_Size+50,(u8*)&g_DI.Cfg); // 写成功之后,再读出数据,保证数据正确
				//printf("g_DI.Cfg WritebufToX5323ok CurTimes:%d .....\r\n",CurTimes);
				OSTimeDlyHMSM(0, 0, 0, 100);
				//printf("g_DI.Cfg ReadbufFromX5323 CurTimes:%d .....\r\n",CurTimes);	  
				ReadbufFromX5323(10,Save_Size,(u8*)&g_DI.Cfg);
				//printf("g_DI.Cfg ReadbufFromX5323ok CurTimes:%d .....\r\n",CurTimes);

	
			}
			
			if (Sys_Flag.Bit.fAutoMsgSave == TRUE)
			{  			
#if IWDG_EN
		IWDG_ReloadCounter(); //手动喂狗  
#endif
				Sys_Flag.Bit.fAutoMsgSave = FALSE;
				//printf("Sys_Flag.Bit.fAutoMsgSave: %d \r\n",Sys_Flag.Bit.fAutoMsgSave);
				//printf("AutoMsgCfg WritebufToX5323 CurTimes:%d .....\r\n",CurTimes);
				WritebufToX5323(1300,AutoMsgSave_Size+50,(u8*)&g_DI.AutoMsgCfg);// 写成功之后,再读出数据,保证数据正确		
				//printf("AutoMsgCfg WritebufToX5323ok CurTimes:%d .....\r\n",CurTimes);
				OSTimeDlyHMSM(0, 0, 0, 100);
				//printf("AutoMsgCfg ReadbufFromX5323 CurTimes:%d .....\r\n",CurTimes);	  
				ReadbufFromX5323(1300,AutoMsgSave_Size,(u8*)&g_DI.AutoMsgCfg);
				//printf("AutoMsgCfg ReadbufFromX5323ok CurTimes:%d .....\r\n",CurTimes);
			
			}
		}
		/* 重新修改网络参数后，启动重新连接 */
		if(Sys_Flag.Bit.fNetRecom == TRUE)
		{
			Sys_Flag.Bit.fNetRecom = FALSE;

//			printf("main fNetRecom \r\n");		
			InitNet();
			Reset_lwipnetif();	
			SysNet_Flag.Bit.fUdpRecom = TRUE;
		} 

		/***************串口数据回传*****************/
//		AutoSendCnt ++;
//		if(AutoSendCnt >= 60)	
//		{
//			
//			AutoSendCnt = 0;
//		
//			USART_AutoSend(&outdata[0],&outlen);
//			UART1Write(&outdata[0], outlen); 			
//			UART1_RBuf.fEnd = FALSE ;
//			UART1_RBuf.Len = 0 ;
//		}
					
		OSTimeDlyHMSM(0, 0, 0, 50);	  		//处理网络数据包	
	}
}



 
 
 /*
 *******************************************************************************
 *
 * Private routines
 *
 *******************************************************************************/
		   
void ReceiveUsartData(u8 *dat, u16 len)
{
  u8 CheckSum = 0;
  u8 CodeData = 0;
  int i = 0;   

    CodeData = dat[4];		
    for(i = 0; i < len; ++i)
    {
      CheckSum = CheckSum + dat[i];
    }
    if((CheckSum == 0) && (dat[0] == 0x55) && (dat[1] == 0xaa) && (dat[2] == 0x01))
    {
      switch(CodeData)
      {
        case SERIAL_CODE_SET_STM32_TO_BOOT:
        {
			STMFLASH_WtiteU16DataToFlash(FLASH_ADDR_UPDATE_FLAG, FLAG_TO_RESET);  
			if(((*(vu32*)(FLASH_ADDR_BOOT + 4)) & 0xFF000000) == 0x08000000)//判断是否为0X08XXXXXX.
			{	 
			 	//CPU_IntDis();  //禁止所有中断 
				Iap_Load_App(FLASH_ADDR_BOOT);//执行FLASH APP代码
			}

        }break;
      	default: break;
      }				
    }
}
void USART_LoginReply(u8 *PackBuf, u16 PackLen,char *outdata,u16 *outlen)
{
	u8 cmd,devaddr;  
	u16 crctmp;		
	u8 crcH, crcL;
	u16 CRC_Value;


	crcL = PackBuf[PackLen - 2];
	crcH = PackBuf[PackLen - 1];

	CRC_Value = CRC16_GX_MODBUS((u8 *)PackBuf, PackLen-2);			// 计算CRC
	
										 
	if ((crcL != (u8)(CRC_Value&0xFF)) || (crcH != (u8)((CRC_Value>>8)&0xFF)))  // CRC校验判断
	{	   					
		return;
	}
	
	(*outlen) = 0;

	outdata[(*outlen)++] = g_DI.Cfg.DeviceID;
	outdata[(*outlen)++] = 0x5A;
	outdata[(*outlen)++] = 0x00;
	outdata[(*outlen)++] = 0x00;

	(*outlen) += CMD_GetParmaData(INFO_MACADDR,&outdata[(*outlen)]); //本设备MAC地址
	(*outlen) += CMD_GetParmaData(INFO_IP_ADDR,&outdata[(*outlen)]); //本设备IP地址
	(*outlen) += CMD_GetParmaData(INFO_MASK_ADDR,&outdata[(*outlen)]); //子网掩码
	(*outlen) += CMD_GetParmaData(INFO_GATEWAY_ADDR,&outdata[(*outlen)]); //默认网关
	(*outlen) += CMD_GetParmaData(INFO_IP_LOCAL1,&outdata[(*outlen)]); // 本地管理1-IP
	(*outlen) += CMD_GetParmaData(INFO_IP_LOCAL2,&outdata[(*outlen)]); //本地管理2-IP
	(*outlen) += CMD_GetParmaData(INFO_IP_SERVER1,&outdata[(*outlen)]); //远程主机1-IP
	(*outlen) += CMD_GetParmaData(INFO_Net_Port1,&outdata[(*outlen)]); //远程主机1-端口号

	outdata[2] = ((*outlen)-3)%256;
	outdata[3] = ((*outlen)-3)/256;
	crctmp = CRC16_GX_MODBUS( (u8 *)outdata,(*outlen));
	outdata[(*outlen)++] = crctmp %256;
	outdata[(*outlen)++] = crctmp /256; 

}
 /******************************************************************************
 * 函数名称：PC_Protocol()
 * 函数功能：PC协议处理
 * 入口参数：*PackBuf:协议数据包,PackLen:数据包长度
 * 出口参数：返回的数据长度
 *******************************************************************************/
u16 PC_Protocol(u8 *dat, u16 len, u8 type)
{
	u16 i;
	u16 TxLen = 0;
	char  logindata[500]; 
  	u16 loginlen = 0;
	float ftemp = 0;
	
	if(type == 2)
	{
	 	if(dat[0] == 0x26)
		{
			//if(In_nStr(uip_appdata,g_DI.Cfg.StationInfo,17) != 0)
			//if(strstr(uip_appdata,g_DI.Cfg.StationInfo) != NULL)
			{
			 	for (i=20; i<len; i++)
				{
			
					if (dat[i+1] == CMD_SEARCH) //设备搜索
					{
					   
					}			 
					else if((dat[i] == g_DI.Cfg.DeviceID)&&((dat[i+1] == CMD_READ) || (dat[i+1] == CMD_WRITE)))
					{
						 
						TxLen = Protocol_Pack_Process(&dat[i], len-i, g_DI.Cfg.DeviceID );
						
						if(SysNet_Flag.Bit.fPaOutCrt == TRUE)
						{
							SysNet_Flag.Bit.fPaOutCrt = FALSE;	
							if( PaOutfout == 0 )
							{
								g_DI.Cfg.P_OutCal = 0;
							}
							else
							{
								ftemp = GetPwr( g_DI.Pa[0].Data.P_OutVal,PowerCal_Out[0],PowerCal_Out[1],0);
								
								PaOutfout = 10*log10(PaOutfout*1000);
								PaOutfout = sqrt(50*pow(10,PaOutfout/10));
								if( ftemp != 0 )
									g_DI.Cfg.P_OutCal = PaOutfout*PaOutfout/(ftemp*ftemp*50000);
								else
									  g_DI.Cfg.P_OutCal = 0;
							}
							Sys_Flag.Bit.fSaveStart = TRUE;
							Sys_Flag.Bit.fSave = TRUE;
							
							PaOutfout = 0;
									
						}
						if(SysNet_Flag.Bit.fPaInCrt == TRUE)
						{
							SysNet_Flag.Bit.fPaInCrt = FALSE;	
							if( PaInfout == 0 )
							{
								g_DI.Cfg.P_InCal = 0;
							}
							else
							{
								ftemp = GetPwr( g_DI.Pa[0].Data.P_InVal,PowerCal_In[0],PowerCal_In[1],0);
								
								//fout = 10*log10(fout*1000);
								PaInfout = sqrt(50*pow(10,PaInfout/10));
								if( ftemp != 0 )
									g_DI.Cfg.P_InCal = PaInfout*PaInfout/(ftemp*ftemp*50000);
								else
									  g_DI.Cfg.P_InCal = 0;
							}
							Sys_Flag.Bit.fSaveStart = TRUE;
							Sys_Flag.Bit.fSave = TRUE;
							PaInfout = 0;		
						}
						
						if(SysNet_Flag.Bit.fPaRvCrt == TRUE)
						{
							SysNet_Flag.Bit.fPaRvCrt = FALSE;
							if( PaRvfout == 0 )
							{
								g_DI.Cfg.P_RefCal = 0;
							}
							else
							{
								ftemp = GetPwr( g_DI.Pa[0].Data.P_ReflexVal,PowerCal_Ref[0],PowerCal_Ref[1],0);
								
								PaRvfout = 10*log10(PaRvfout*1000);
								PaRvfout = sqrt(50*pow(10,PaRvfout/10));
								if( ftemp != 0 )
									g_DI.Cfg.P_RefCal = PaRvfout*PaRvfout/(ftemp*ftemp*50000);
								else
									  g_DI.Cfg.P_RefCal = 0;
							}
							Sys_Flag.Bit.fSaveStart = TRUE;
							Sys_Flag.Bit.fSave = TRUE;
							PaRvfout = 0;
						}
						
						
						return TxLen;		
					}
							
				}
			}
		}
		
	}
	else
	{
		for (i=0; i<len; i++)
		{
	
			if (dat[i+1] == CMD_SEARCH) //设备搜索
			{
				USART_LoginReply(&dat[i], len-i, &logindata[0],&loginlen);
				DataCopy( g_UART_TBuf, logindata, loginlen );	
				return loginlen;
			}			 
			else if((dat[i] == g_DI.Cfg.DeviceID)&&((dat[i+1] == CMD_READ) || (dat[i+1] == CMD_WRITE)))
			{

				return Protocol_Pack_Process(&dat[i], len-i, g_DI.Cfg.DeviceID );			
			}
					
		}
	
	}
//	ReceiveUsartData(dat,len);	
	


   
	return 0;
}

/******************************************************************************
 * 函数名称：Pack_PT()
 * 函数功能：数据包透传
 * 入口参数：*Buf:透传数据包,Len:透传数据包长度,timeout:限时时间
 * 出口参数：ret返回的数据长度
 *******************************************************************************/
u8 Pack_PT(u8 *Buf, u8 Len, u16 timeout)
{
	u8 err;	    
	u8 Lenth;
	//临时性措施：强行获取信号量，防止数据无法上传。
	
	fEitRTU = TRUE;
	OSSemPend(JLQRS232SEM, 0, &err);		// 信号量100ms超时
	
	UART3Write(Buf, Len);
		
	if (EXCIT_Wait_RxEnd(timeout) == FALSE)			// 限时接收
	{
		
		UART3_RBuf.Len = 0;
		UART3_RBuf.fEnd = FALSE;
		OSSemPost(JLQRS232SEM);
		fEitRTU = FALSE;
		return FALSE;
	}
	
	DataCopy((u8 *)&g_UART_TBuf[0], UART3_RBuf.Buff, UART3_RBuf.Len);
	Lenth = UART3_RBuf.Len;
		
	UART3_RBuf.Len = 0;
	UART3_RBuf.fEnd = FALSE;
		
	OSSemPost(JLQRS232SEM);
	fEitRTU = FALSE;
	
	return Lenth;
} 

/*
 *******************************************************************************
 *
 * Private routines
 *
 *******************************************************************************/


/******************************************************************************
 * 函数名称：Pa_Wait_RxEnd()
 * 函数功能：等待RS485数据接收完成
 * 入口参数：timeout:等待的超时限制 单位 毫秒
 * 出口参数：合并后的数据长度
 *******************************************************************************/
bool Pa_Wait_RxEnd(u16 timeout)
{
	while (timeout--)				// 限时接收
	{

#if IWDG_EN
	  IWDG_ReloadCounter(); //手动喂狗  
#endif
		if((UART5_RBuf.Len>=9) && UART5_RBuf.fEnd)	// 等待数据接收完成
		{
		
			return TRUE;
		}
		OSTimeDlyHMSM(0, 0, 0, 30);
	}
	
	return FALSE;
}
 
