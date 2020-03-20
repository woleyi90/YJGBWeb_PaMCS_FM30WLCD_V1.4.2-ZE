/*******************************************************************************
 *
 * Filename  : TaskRTU.c
 *
 * Function  : Implememt TASKRTU function form for this workbench.
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



/*
 *******************************************************************************
 *
 * private defines
 *
 *******************************************************************************
 */
#define		DEVNUM_MAX	    1 	    // 轮询设备数
#define     RSTIMEOUT		2		// RS485连接超时次数




/*
 *******************************************************************************
 *
 * private routines prototype
 *
 *******************************************************************************
 */

u8 PAPack_Collect_RS(void);	
u8 ExciterPack_Collect_RS(void);  
void PADevPoll(void);
void ExciterDevPoll(void);

static int	EitCollect_cnt = 0;

/*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */
/*******************************************************************************
 * 函数名称：TaskRTU()
 * 函数功能：RTU处理任务 负责RTU的数据采集 ,处理 管理 报警等
 * 入口参数：无
 * 出口参数：无 
 *******************************************************************************/
void  TaskRTU(void *pdata)
{
	u8 err;				 
	u8 patrynum = 0,paresult = 0; 
	
	pdata = pdata;
	
	OSTimeDlyHMSM(0, 0, 2, 0);	 
	USART3_Config(115200);          //初始化串口3 调制板串口RS232通信
	
	while (1)
	{
	
		EitCollect_cnt++;
		if(EitCollect_cnt >= 25)
		{
			EitCollect_cnt = 0;
				
			if(fEitRTU == FALSE)
			{
				ExciterDevPoll(); 
				OSTimeDlyHMSM(0, 0, 0, 30);
				OSSemPend(JLQRS232SEM, 0, &err);	
				PADevPoll();
				OSSemPost(JLQRS232SEM);				

			}
		}	
			
		OSTimeDlyHMSM(0, 0, 0, 50);	  		//处理网络数据包	

	}
}	



/*
 ******************************************************************************
 * 函数名称：PADevPoll()
 * 函数功能：PA设备轮巡
 * 入口参数：无
 * 出口参数：TRUE 成功,FALSE:失败
 *******************************************************************************/
 void PADevCheckPoll(void)
 {
  	u8 err,rslen;
    u8 trynum;
    u8 result;
    
    static u8 PaCnt;	   // 功放超时计数	    	
	trynum = 3;

	while (trynum--)
	{ 	    			
		rslen = PAPack_Collect_RS();	//功放模块状态查询	 				
		result = PA_RS232_GXTx(GX_TXData,rslen, TIMEOUT);	//发送模块状态查询数据包  	
		if(result == eDev_NO_ERR)
		{
		  	
			trynum = 0;		    // 接收到正确回应,重发次数清零	
		} 		
	}
		
	/* 通信超时，模块状态结构体数据全部清零 */	
	if (result != eDev_NO_ERR)
	{
		  
			if(PaCnt < 200)  //
			{
				PaCnt++;		 
			}
			if(PaCnt >= 2)  //
			{
				//超时，参数清0
				g_TMTLink[0] =0;		 
				memset((u8*)&g_DI.Pa[0], 0, sizeof(PA_Data));

			}
		
	}
	else
	{
		PaCnt = 0;			// 计数清零	
		g_TMTLink[0] =1;	// 功放通信状态
	}	
 }
/*
 ******************************************************************************
 * 函数名称：PADevPoll()
 * 函数功能：PA设备轮巡
 * 入口参数：无
 * 出口参数：TRUE 成功,FALSE:失败
 *******************************************************************************/
void PADevPoll(void)
{
	char  outdata[500]; 
  	u16 outlen = 0; 

	USART_AutoSend(&outdata[0],&outlen);
	   
	UART3Write(&outdata[0], outlen); 
	
	EXCIT_Wait_RxEnd(1);	

	UART3_RBuf.Len = 0;
	UART3_RBuf.fEnd = FALSE;		
		
}
/*
 ******************************************************************************
 * 函数名称：ExciterDevPoll()
 * 函数功能：激励器设备轮巡
 * 入口参数：无
 * 出口参数：TRUE 成功,FALSE:失败
 *******************************************************************************/
 void ExciterDevPoll(void)
 {
  	u8 err,rslen;
    u8 trynum;
    u8 result;
    
    static u8 ExCnt;	   // 功放超时计数	    	
	trynum = 3;

	while (trynum--)
	{ 	    			
		rslen = ExciterPack_Collect_RS();	//功放模块状态查询	 			
		OSSemPend(JLQRS232SEM, 0, &err);	
		result = Excit_RS232_Send(GX_TXData,rslen, TIMEOUT);	//发送模块状态查询数据包  
		OSSemPost(JLQRS232SEM);			
		if(result == eDev_NO_ERR)
		{
		  	
			trynum = 0;		    // 接收到正确回应,重发次数清零	
		} 		
	}
		
	/* 通信超时，模块状态结构体数据全部清零 */	
	if (result != eDev_NO_ERR)
	{
		  
			if(ExCnt < 200)  //
			{
				ExCnt++;		 
			}
			if(ExCnt >= 2)  //
			{
				//超时，参数清0
				g_TMTLink[1] =0;		 
				memset((u8*)&g_DI.Excitdata, 0, sizeof(Excit_Data));

			}
		
	}
	else
	{
		ExCnt = 0;			// 计数清零	
		g_TMTLink[1] =1;	// 功放通信状态
	}	
 } 

/*
 ******************************************************************************
 * 函数名称：Dev_Pro()
 * 函数功能：模块设备控制操作
 * 入口参数：无
 * 出口参数：TRUE 成功,FALSE:失败
 *******************************************************************************/
bool Dev_Opr(void)
{
	u8 trynum;	    // 重发次数
	u8 Head;
	u8 result;
	u8 Opr_type =0;	//设置操作作的类型   
	u8 ATTV[4];
	u8 Channal;

	

	
	if ((g_DevPro == MB_DevProNONE)||(g_DevPro == 0x00))		// 无操作
	{
		return FALSE;
	}
	
	trynum = 3;
	while (trynum--)
	{
		switch (g_DevPro & 0x7F)
		{									

/***********************主监控RS485用于各个功放模块的通信***********************************/
			/* 本地设置发射机开关*/
			case MB_BIT_PA0SW: // 发射机0开关
				
				if((g_DevPro&0x80) == 0x80) //开机
				{
						memset(GX_TXData,0,100);
						GX_TXData[0]= g_DI.Cfg.PA_IPHead;
						GX_TXData[1] = 0x35;
						GX_TXData[2] = 0xc3;
						GX_TXData[3] = 0xfc;
						GX_TXData[4] = 0xc1;
						GX_TXData[5] = 0xee;
						GX_TXData[6] = 0xbf;
						GX_TXData[7] = 0xaa;
						GX_TXData[8] = 0xbb;
						GX_TXData[9] = 0xfa;
						GX_TXDatalen = 10;																						
				}
				else
				{  				 	
				 		memset(GX_TXData,0,100);
						GX_TXData[0]= g_DI.Cfg.PA_IPHead;
						GX_TXData[1] = 0x35;
						GX_TXData[2] = 0xc3;
						GX_TXData[3] = 0xfc;
						GX_TXData[4] = 0xc1;
						GX_TXData[5] = 0xee;
						GX_TXData[6] = 0xb9;
						GX_TXData[7] = 0xd8;
						GX_TXData[8] = 0xbb;
						GX_TXData[9] = 0xfa;
						GX_TXDatalen = 10;																		
				} 		  						
				
				Opr_type =0;

				break;				

		 	

		

			default:  
				Opr_type = 2;
				break;
		}
				
		 if(Opr_type == 0)
		{ 		 	
			RS485_Tx(&GX_TXData[0], GX_TXDatalen, 1);			
			trynum = 0 ;		// 接收到正确回应,重发次数清零
			g_DevPro = MB_DevProNONE;		// 操作失败标志清除	 
			
			return TRUE;
			
		} 
		else
		{
		 	g_DevPro = MB_DevProNONE;		// 操作失败标志清除
			return FALSE; 
		
		}

	} 
	g_DevPro = MB_DevProNONE;		// 操作失败标志清除
	return FALSE;  		
}
/*******************************************************************************
 * 函数名称：PAPack_Collect_RS()
 * 函数功能：打包RS485数据包
 * 入口参数：
 * 出口参数：
 *******************************************************************************/ 
u8 PAPack_Collect_RS(void)
{
	u8 txlen;

	if(Sys_Flag.Bit.fPaCheck == FALSE)//在定时轮询查询功放状态之前先校验一下功放首字命令
	{
	
		GX_TXData[0]= 0x00;
		GX_TXData[1]= 0x32;
		txlen = 2;
	
	}
	else
	{
	  	GX_TXData[0]= g_DI.Cfg.PA_IPHead;
		GX_TXData[1]= 0x03;
		GX_TXData[2]= 0x00;
		GX_TXData[3]= 0x00;
		GX_TXData[4]= 0x00;
		GX_TXData[5]= 0x46;
		txlen = 6;
		
	}
	return txlen;
}
/*******************************************************************************
 * 函数名称：ExciterPack_Collect_RS()
 * 函数功能：打包数据包
 * 入口参数：
 * 出口参数：
 *******************************************************************************/ 
u8 ExciterPack_Collect_RS(void)
{
	u8 txlen;
//	Sys_Flag.Bit.fExcitCheck = TRUE;
//	g_DI.Cfg.Excit_IPHead = 0x33;
	if(Sys_Flag.Bit.fExcitCheck == FALSE)//在定时轮询查询功放状态之前先校验一下功放首字命令
	{
	
		GX_TXData[0]= 0x00;
		GX_TXData[1]= 0x32;
		txlen = 2;
	
	}
	else
	{
	  	GX_TXData[0]= g_DI.Cfg.Excit_IPHead; 
		GX_TXData[1]= 0x03;
		GX_TXData[2]= 0x00;
		GX_TXData[3]= 0x00;
		GX_TXData[4]= 0x00;
		GX_TXData[5]= 0x46;
		txlen = 6;
		
	}
	return txlen;
}


