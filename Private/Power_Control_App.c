#include "global.h"	
#include "includes.h"
#include "Power_Control_App.h"


u16 I_Couter[2]={0};
u16 TEMP_Couter[2]={0};
u16 RF_Counter = 0;
u16 PaVcc_Counter = 0;
/*外部硬件保护状态*/
u8 ProtectRead = 0;
u8 ProtectTrg = 0;
u8 ProtectCont = 0;

/*正在调节功率*/
u8 PowerChanging;
u8 ManualChanging = 0;
 /*上电自动控制延时*/
u8 StartupCtrlDelay = 0;
u8 StartupGateDelay = 0;
u16 FAN_TEMP_Counter=0;
/*手动开启风扇延时*/
u8  FAN_ManCtrl_Delay=0;
MYTIMER User_Timer[3];
/*******************************************************************************
 * 函数名称：
 * 函数功能：
 * 入口参数：  
 * 出口参数：
 *******************************************************************************/
void SetGateCtrl(u8 onoff)
{
	 if( onoff )
	 {
	      GPIO_ResetBits(GPIOG,GPIO_Pin_8);
	 }
	 else
	 {
	      GPIO_SetBits(GPIOG,GPIO_Pin_8);
	 }
}
//栅压检测
u8 GateVolCheck(void)
{
    
	 if( GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) )
	 {
		return 1;
	 } 
	 else
	 {	
		 return 0;
	 }
	     	
    //u8 ret = GPIO_ReadOutputDataBit(GATE_CHECK_PORT,GATE_CHECK_PIN);
	//return !ret;

}
u8 SelfTest(void)
{
	u8 i;
	ATT_Senddata(0);
	//关断栅压
	SetGateCtrl(1);
	OSTimeDlyHMSM(0, 0, 0, 30);
	if( GateVolCheck() )
	{
		SetGateCtrl(0);
		
	}
	else
	{
		
		return 1;
	}
	OSTimeDlyHMSM(0, 0, 0, 30);
	if( GateVolCheck())
	{
	  
		return 1;
	}

	ATT_Senddata(0);

	return 0;
}
/*关机*/
void AP_Shutdown(void)
{
	SetGateCtrl(0);
	User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
	User_Timer[POWERAUTOTIMER].Timer_Enable = 0;

}
/*开机*/
void AP_Startup(void)
{
	SetGateCtrl(1);
	User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
	User_Timer[POWERAUTOTIMER].Timer_Out_Count = 30;
	User_Timer[POWERAUTOTIMER].Timer_Enable = 1;

	
	if( g_DI.Cfg.P_AutoContrl ==0 )
	{
		ManualChanging = 0;	
		
	}
}
u8 SwControlGet(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	u8 ret = GPIO_ReadOutputDataBit(GPIOx,GPIO_Pin);
	return ret;
}
void Temp_CheckProcess(void)
{
	/*温度状态分析*/
	if( g_DI.Pa[0].Data.PA_TPT > g_DI.Cfg.PWROFF_TPTlimit )
	{
	    if( TEMP_Couter[0]<TEMP_CHECK_DELAY )
		    TEMP_Couter[0]++;
		TEMP_Couter[1] = 0;
		
	}
	else if( g_DI.Pa[0].Data.PA_TPT > g_DI.Cfg.PWRRED_TPTlimit )
	{
	    if( TEMP_Couter[1]<TEMP_CHECK_DELAY )
		    TEMP_Couter[1]++;
		TEMP_Couter[0] = 0;
		
	}
	else
	{
	    TEMP_Couter[0] = 0;
		TEMP_Couter[1] = 0;
		g_DI.Pa[0].Data.TEMP_Status = 0x00;
	}
    if( TEMP_Couter[0]>=TEMP_CHECK_DELAY)
	    g_DI.Pa[0].Data.TEMP_Status = 0x01 << I_PROTECT_HTIGHT; 
	if( TEMP_Couter[1]>=TEMP_CHECK_DELAY)
		g_DI.Pa[0].Data.TEMP_Status = 0x01 << I_HEIGHT; 
	if( TEMP_Couter[0] == 0 && TEMP_Couter[1] == 0 )
	    g_DI.Pa[0].Data.TEMP_Status = 0;

	g_DI.Pa[0].Data.TEMP_StatusTrig = g_DI.Pa[0].Data.TEMP_Status & (g_DI.Pa[0].Data.TEMP_Status ^ g_DI.Pa[0].Data.TEMP_StatusCont); 
	g_DI.Pa[0].Data.TEMP_StatusRelease = (g_DI.Pa[0].Data.TEMP_Status ^ g_DI.Pa[0].Data.TEMP_StatusTrig ^ g_DI.Pa[0].Data.TEMP_StatusCont);   
	g_DI.Pa[0].Data.TEMP_StatusCont	= g_DI.Pa[0].Data.TEMP_Status;

}
void Main_Process(void)
{
	float pwwc = 0;
	float rcha=0, rcfs = 0;
	
	/*电源状态分析*/
	if( g_DI.Pa[0].Data.PA_VOL > 51.0 )
	{
		if( PaVcc_Counter<I_CHECK_DELAY )
		{
			PaVcc_Counter++;	
		}
		    	
	}
	else if( g_DI.Pa[0].Data.PA_VOL < 51.0 && g_DI.Pa[0].Data.PA_VOL > 45.0)
	{
		PaVcc_Counter = 0;

	}
	if( PaVcc_Counter>=I_CHECK_DELAY)
	{
	    g_DI.Pa[0].Data.PAVCC_Status = 0x01 << I_HEIGHT ;   //电源异常状态
	}

	if( PaVcc_Counter == 0)
	    g_DI.Pa[0].Data.PAVCC_Status = 0x00;   //电源正常状态

	//电源状态变化
	g_DI.Pa[0].Data.PAVCC_StatusTrig = g_DI.Pa[0].Data.PAVCC_Status & (g_DI.Pa[0].Data.PAVCC_Status ^ g_DI.Pa[0].Data.PAVCC_StatusCont); 
    g_DI.Pa[0].Data.PAVCC_StatusRelease = (g_DI.Pa[0].Data.PAVCC_Status ^ g_DI.Pa[0].Data.PAVCC_StatusTrig ^ g_DI.Pa[0].Data.PAVCC_StatusCont);   
	g_DI.Pa[0].Data.PAVCC_StatusCont = g_DI.Pa[0].Data.PAVCC_Status;
	
	/*电流状态分析*/

	if( g_DI.Pa[0].Data.PA_CUR1 > g_DI.Cfg.PWROFF_CURlimit  )
	{
		if( I_Couter[0]<I_CHECK_DELAY )
		    I_Couter[0]++;
		I_Couter[1] = 0;
		
	}
	else if( g_DI.Pa[0].Data.PA_CUR1 > g_DI.Cfg.PWRRED_CURlimit )
	{
		if( I_Couter[1]<I_CHECK_DELAY )
		    I_Couter[1]++;
		I_Couter[0] = 0;
	}
	else
	{
		I_Couter[0] = 0;
		I_Couter[1] = 0;
	}
	if( I_Couter[0]>=I_CHECK_DELAY)
	{
	    g_DI.Pa[0].Data.I_Status = 0x01 << I_PROTECT_HTIGHT;   //电流过流状态
	}
	if( I_Couter[1]>=I_CHECK_DELAY)
	{
		g_DI.Pa[0].Data.I_Status = 0x01 << I_HEIGHT;   //电流高状态
	}
	if( I_Couter[0]==0 && I_Couter[1]==0)
	    g_DI.Pa[0].Data.I_Status = 0x00;   //电流正常

	//电流状态变化
	g_DI.Pa[0].Data.I_StatusTrig = g_DI.Pa[0].Data.I_Status & (g_DI.Pa[0].Data.I_Status ^ g_DI.Pa[0].Data.I_StatusCont); 
    g_DI.Pa[0].Data.I_StatusRelease = (g_DI.Pa[0].Data.I_Status ^ g_DI.Pa[0].Data.I_StatusTrig ^ g_DI.Pa[0].Data.I_StatusCont);   
	g_DI.Pa[0].Data.I_StatusCont = g_DI.Pa[0].Data.I_Status;
	
	
	if( g_DI.Cfg.PWR_RVlimit !=0 && g_DI.Pa[0].Data.Power_RV > g_DI.Cfg.PWR_RVlimit)
	{
	    
		if( RF_Counter > I_CHECK_DELAY)
		{
		    g_DI.Pa[0].Data.RF_Status = 0x01 << I_PROTECT_HTIGHT;  
		}
		else
		{
			RF_Counter++;
		}
	}
	else
	{
	    RF_Counter = 0;
		g_DI.Pa[0].Data.RF_Status = 0x00; 
	}
	g_DI.Pa[0].Data.RF_StatusTrig = g_DI.Pa[0].Data.RF_Status & (g_DI.Pa[0].Data.RF_Status ^ g_DI.Pa[0].Data.RF_StatusCont); 
	g_DI.Pa[0].Data.RF_StatusCont = g_DI.Pa[0].Data.RF_Status;



	if( g_DI.Pa[0].Data.Power_IN < g_DI.Cfg.PWR_INlimit )
	    g_DI.Pa[0].State.LP_Alm = 1;
	else
	    g_DI.Pa[0].State.LP_Alm = 0;



	if( !((g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_OVERLOAD) ||
	(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_OVERTEMP)	||
	(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_RFHEIGHT) ||
	(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_PAVCCERR) ) 
	)
	{	
		if( User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout == 1 )
		{
			
			User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
			ATT_Senddata( 0 );
	
			AP_Startup();
		}
	}



	/*开机状态下的电流,温度降功率*/
	if( GateVolCheck() )	/*有栅压*/
	{
		
		/*电源关机保护*/
		if( g_DI.Pa[0].Data.PAVCC_StatusTrig & (0x01 << I_HEIGHT)) //电源保护,从正常到保护一次
		{
			
			ATT_Senddata(0);
			g_DI.Pa[0].Data.Protect_Shutdown_Cause |= 0x01<<PS_PAVCCERR;
			AP_Shutdown();		   //关功放电源
			
			User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Enable = 1;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Count =0 ;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;

		}
		
		/*电流关机保护*/
		if( g_DI.Pa[0].Data.I_StatusTrig & (0x01 << I_PROTECT_HTIGHT)) //电流过流保护,从正常到保护一次
		{
			ATT_Senddata(0);
			g_DI.Pa[0].Data.Protect_Shutdown_Cause |= 0x01<<PS_OVERLOAD;
			AP_Shutdown();		   //关功放电源

			User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Enable = 1;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Count =0 ;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;//过流重启时间
		}
	
		/*温度关机保护*/
		if( g_DI.Pa[0].Data.TEMP_StatusTrig & (0x01 << I_PROTECT_HTIGHT))
		{
			ATT_Senddata(0);
			g_DI.Pa[0].Data.Protect_Shutdown_Cause |= 0x01<<PS_OVERTEMP;
			AP_Shutdown();		   //关功放电源

			User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Enable = 1;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Count =0 ;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Out_Count = g_DI.Cfg.Temp_ReducePowerInterval;
		}
		
		/*反射功率保护*/
		if( g_DI.Pa[0].Data.RF_StatusTrig & (0x01 << I_PROTECT_HTIGHT))
		{
			ATT_Senddata(0);
			g_DI.Pa[0].Data.Protect_Shutdown_Cause |= 0x01<<PS_RFHEIGHT;
			AP_Shutdown();		   //关功放电源

			User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Enable = 1;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Count =0 ;
			User_Timer[OVERLOADRESTARTTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;
		}
/**********************过温度降功率 过流降功率*****************************/
		if( (g_DI.Pa[0].Data.TEMP_StatusTrig & 0x01<<I_HEIGHT ) )		//过温度降功率
		{
			g_DI.Pa[0].Data.ProState |= 0x01 <<CHK2_TEMP_POWERDOWN;
			
			if( g_DI.Cfg.P_AutoContrl ==1)
			{
			    if( g_DI.Pa[0].Data.P_DownOffset > -15 )
				{
					g_DI.Pa[0].Data.P_DownOffset -= 5;
					
				}
			        
			}
			else
			{
				if(g_DI.Pa[0].Data.RES_Front > -10)
				{
					g_DI.Pa[0].Data.RES_Front = g_DI.Pa[0].Data.RES_Front-2;
							
				}			
				if(g_DI.Cfg.P_Res_Front_Target >= (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front))
				{
					ATT_Senddata( (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front) ); 
					
					ManualChanging = 1;					
				}
					 		
			}

			User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.Temp_ReducePowerInterval;
		}

		if( (g_DI.Pa[0].Data.TEMP_StatusRelease & 0x01<<I_HEIGHT)  )		//温度恢复
		{
			
			if( g_DI.Cfg.P_AutoContrl == 1 )
			{
				
				if( g_DI.Pa[0].Data.P_DownOffset <0 )
				{
					g_DI.Pa[0].Data.P_DownOffset += 5;
					
				}	
			    else
				{
					g_DI.Pa[0].Data.P_DownOffset = 0;
					
				}
					
				
				if( g_DI.Pa[0].Data.P_DownOffset != 0)
				{
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.Temp_ReducePowerInterval;
					
				}
				else if(g_DI.Pa[0].Data.P_DownOffset == 0)
				{
					
					g_DI.Pa[0].Data.P_DownOffset = 0;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 0;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.Temp_ReducePowerInterval;
				}
			}
			else
			{
				if(g_DI.Pa[0].Data.RES_Front < 0)
				{
					g_DI.Pa[0].Data.RES_Front = g_DI.Pa[0].Data.RES_Front+2;
				
					if(g_DI.Cfg.P_Res_Front_Target >= (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front))
					{
						ATT_Senddata( (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front) ); 
						 
						ManualChanging = 1;	
					}
					if( g_DI.Pa[0].Data.RES_Front != 0)
					{
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;
			
					}	
										
				}
				else
				{
					
						 
						
						g_DI.Pa[0].Data.P_DownOffset = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;
					    ManualChanging = 0;	
				}	
				
			}
	
		}

		
		
		if( (g_DI.Pa[0].Data.I_StatusTrig & 0x01<<I_HEIGHT ) )		//过流降功率
		{
			g_DI.Pa[0].Data.ProState |= 0x01 <<CHK2_I_POWERDOWN;
//			printf("过流降功率....... \n");

			if( g_DI.Cfg.P_AutoContrl ==1)
			{
			    if( g_DI.Pa[0].Data.P_DownOffset > -15 )
				{
					g_DI.Pa[0].Data.P_DownOffset -= 5;
					
				}
			        
			}
			else
			{
				if(g_DI.Pa[0].Data.RES_Front > -10)
				{
					g_DI.Pa[0].Data.RES_Front = g_DI.Pa[0].Data.RES_Front-2;
//					printf(" 手动  过流降功率 g_DI.Pa[0].Data.RES_Front -= 2....... \n");			
				}
				if(g_DI.Cfg.P_Res_Front_Target >= (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front))
				{
					ATT_Senddata( (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front) ); 
//					printf(" 手动  过温度降功率 q ATT_Senddata....... \n");	
					ManualChanging = 1;						
				}
				
			}

			User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;
		}

		if( (g_DI.Pa[0].Data.I_StatusRelease &0x01<<I_HEIGHT)  )		//电流恢复
		{
			if( g_DI.Cfg.P_AutoContrl == 1 )
			{
				
				if( g_DI.Pa[0].Data.P_DownOffset <0 )
				{
					g_DI.Pa[0].Data.P_DownOffset += 5;
					
				}	
			    else
				{
					g_DI.Pa[0].Data.P_DownOffset = 0;
					
				}
					
				
				if( g_DI.Pa[0].Data.P_DownOffset != 0)
				{
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;
					
				}
				else if(g_DI.Pa[0].Data.P_DownOffset == 0)
				{
					
					g_DI.Pa[0].Data.P_DownOffset = 0;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 0;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
					User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;
				}

			}
			else
			{
				if(g_DI.Pa[0].Data.RES_Front < 0)
				{
					g_DI.Pa[0].Data.RES_Front = g_DI.Pa[0].Data.RES_Front+2;
//					printf(" 手动  电流降功率 g_DI.Pa[0].Data.RES_Front += 2....... \n");
					if(g_DI.Cfg.P_Res_Front_Target >= (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front))
					{
						ATT_Senddata( (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front) ); 
//						printf(" 手动  过温度降功率 w ATT_Senddata....... \n");
						ManualChanging = 1;							
					}
					if( g_DI.Pa[0].Data.RES_Front != 0)
					{
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;
//						printf(" 手动  电流降功率 恢复  P_DownOffset  != 0....... \n");
					}	
										
				}
				else
				{
					
						ManualChanging = 0;	 
//						printf(" 手动  电流降功率 恢复  P_DownOffset  == 0....... \n");
						g_DI.Pa[0].Data.P_DownOffset = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Out_Count = g_DI.Cfg.CurRstDelay;
					
				}	
				
			}		
		}
		
	if( ((g_DI.Pa[0].Data.TEMP_StatusCont & 0x01<<I_HEIGHT )||(g_DI.Pa[0].Data.I_StatusCont &0x01<<I_HEIGHT ))
			&& (User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable == 0))
		{
//			printf("降功率 定时器 Timer_Enable = 1 ...........\n");
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;	
			
		}

		if( User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout == 1)
		{
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Timeout = 0;
			User_Timer[CHECKPOWERDOWNTIMER].Timer_Count =0 ;
//			printf(" 定时器 降功率 Timer_Timeout = 1....... \n");
			
			if( (g_DI.Pa[0].Data.I_StatusCont &0x01<<I_HEIGHT) )	 
			{			
//				printf("定时器 电流将功率 : %d....... \n",g_DI.Pa[0].Data.I_StatusCont);
				if( g_DI.Cfg.P_AutoContrl == 1 )
				{
				    if( g_DI.Pa[0].Data.P_DownOffset > -15 )		
					{
					  
					   g_DI.Pa[0].Data.P_DownOffset -= 5;			
					}
				     
				}
				else
				{
					if(g_DI.Pa[0].Data.RES_Front > -10)
					{
						g_DI.Pa[0].Data.RES_Front = g_DI.Pa[0].Data.RES_Front-2;
//						printf(" 手动  过流降功率 e g_DI.Pa[0].Data.RES_Front -= 2....... \n");			
					}
					if(g_DI.Cfg.P_Res_Front_Target >= (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front))
					{
						ATT_Senddata( (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front) ); 
//						printf(" 手动  过温度降功率 r ATT_Senddata....... \n");	 
					}
				}
				User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;  
			}
			else if( (g_DI.Pa[0].Data.TEMP_StatusCont & 0x01<<I_HEIGHT) )	 
			{			
//				printf("定时器 温度将功率 : %d....... \n",g_DI.Pa[0].Data.TEMP_StatusCont);
				if( g_DI.Cfg.P_AutoContrl == 1 )
				{
				    if( g_DI.Pa[0].Data.P_DownOffset > -15 )
					{
						
						g_DI.Pa[0].Data.P_DownOffset -= 5;
					}
				        
				}
				else
				{
					if(g_DI.Pa[0].Data.RES_Front > -10)
					{
						g_DI.Pa[0].Data.RES_Front = g_DI.Pa[0].Data.RES_Front-2;
//						printf(" 手动  过流降功率 t g_DI.Pa[0].Data.RES_Front -= 2....... \n");			
					}
					if(g_DI.Cfg.P_Res_Front_Target >= (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front))
					{
						ATT_Senddata( (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front) ); 
//						printf(" 手动  过温度降功率 y ATT_Senddata....... \n");	 
					}
					
				}
				User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;  
			}
			else
			{
			    if( g_DI.Cfg.P_AutoContrl == 1 )
				{
					if( g_DI.Pa[0].Data.P_DownOffset <0 )
					{
						
						g_DI.Pa[0].Data.P_DownOffset += 5;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1;  
					}				    
					else
					{
						
						g_DI.Pa[0].Data.P_DownOffset = 0;
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 0;  
					}
					    
				}
				else
				{
					if(g_DI.Pa[0].Data.RES_Front < 0)
					{
						g_DI.Pa[0].Data.RES_Front = g_DI.Pa[0].Data.RES_Front+2;
//						printf(" 手动  电流降功率 g_DI.Pa[0].Data.RES_Front += 2....... \n");
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 1; 
						if(g_DI.Cfg.P_Res_Front_Target >= (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front))
						{
							ATT_Senddata( (g_DI.Cfg.P_Res_Front_Target + g_DI.Pa[0].Data.RES_Front) ); 
//							printf(" 手动  过温度降功率 u ATT_Senddata....... \n");
							ManualChanging = 1;							
						}
						 						
					}
					else
					{
						ManualChanging = 0;
						g_DI.Pa[0].Data.RES_Front = 0;	
						User_Timer[CHECKPOWERDOWNTIMER].Timer_Enable = 0;
						 
					}

				}
			}
			
			
		}
		
	}

UPDATESTATUS:
	/*开机状态下自动功率控制*/
    pwwc = g_DI.Pa[0].Data.Power_FR - (g_DI.Cfg.PWR_RateOut+3 + g_DI.Pa[0].Data.P_DownOffset);
	if( pwwc >130 )
	    pwwc = 0;
	if( pwwc <-130 )
	    pwwc = 0;
	rcha = g_DI.Cfg.PWR_RateOut*0.1; 
	if( rcha > 10 )
	    rcha = 10;
	else if( rcha < 3 )
	    rcha = 3;
	
	rcfs = 0 - rcha;

	if( ( g_DI.Cfg.P_AutoContrl ==0 ) && (ManualChanging == 0) &&
		 StartupCtrlDelay >= STARTUPDELAYCONTER )
	{
		 
		 if( User_Timer[POWERAUTOTIMER].Timer_Timeout == 1 )
		 {
				
			 if( (g_DI.Cfg.P_Res_Front_Target >= g_DI.Pa[0].Data.RES_Rear) && GateVolCheck())
				 {
									
					 if( g_DI.Pa[0].Data.RES_Rear <63 )
					 {
						 
						 if( (pwwc > rcfs && pwwc < (rcha*0.7 )) )
						 {
							 
							 User_Timer[POWERAUTOTIMER].Timer_Out_Count = 30; 
							 User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
							 User_Timer[POWERAUTOTIMER].Timer_Enable = 0;
							 

							 if( PowerChanging == 1 )
							 {
								 if( g_DI.Cfg.P_Res_Front_Target != g_DI.Pa[0].Data.RES_Rear)
								 {
										g_DI.Cfg.P_Res_Front_Target = g_DI.Pa[0].Data.RES_Rear;	
										Sys_Flag.Bit.fSaveStart = TRUE;
										Sys_Flag.Bit.fSave = TRUE;
								 }
								 
								 PowerChanging = 0;	
							 }
							 ManualChanging = 1;

							 goto UPDATEEND;
						 }
						 else if(pwwc > rcha )
						 {
							ATT_Senddata( g_DI.Pa[0].Data.RES_Rear-1 );
							 
						 }
						 else if((pwwc > (rcfs*2) && pwwc < rcfs ) )
						 {
							ATT_Senddata( g_DI.Pa[0].Data.RES_Rear+1 );
							
						 }
						 else if(pwwc < (rcfs*2) )
						 {
							ATT_Senddata( g_DI.Pa[0].Data.RES_Rear+3 );
							
						 }
						
					 }
					 else
					 {
						ATT_Senddata( 62 );	
						 
					 }	
							
				 }
				 else if( g_DI.Cfg.P_Res_Front_Target <g_DI.Pa[0].Data.RES_Rear )
				 {
					 ATT_Senddata( g_DI.Cfg.P_Res_Front_Target );
					 ManualChanging = 1;
					 User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
					 User_Timer[POWERAUTOTIMER].Timer_Enable = 0;
					 PowerChanging = 0;
					 
				 }	
			
			
		 }
		 
		 if( g_DI.Cfg.P_Res_Front_Target != g_DI.Pa[0].Data.RES_Rear )
		 {
			if( fabs(g_DI.Pa[0].Data.RES_Rear-g_DI.Cfg.P_Res_Front_Target)>15)
			    User_Timer[POWERAUTOTIMER].Timer_Out_Count = 30; 
			else
			    User_Timer[POWERAUTOTIMER].Timer_Out_Count = 60; 

			 PowerChanging = 1;

			 User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
			 User_Timer[POWERAUTOTIMER].Timer_Enable = 1;
			
		 }
		 else
		 {
		 	 User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
			 User_Timer[POWERAUTOTIMER].Timer_Enable = 0;
			 PowerChanging = 0;
			 
		 }
	}

	if(  g_DI.Cfg.P_AutoContrl == 1  && 	 
	     GateVolCheck() &&
		 StartupCtrlDelay >= STARTUPDELAYCONTER	
	)
	{
		 
		 if( User_Timer[POWERAUTOTIMER].Timer_Timeout == 1 )
		 {
			 User_Timer[POWERAUTOTIMER].Timer_Out_Count = 30; 
			 
		     if(  pwwc < rcfs )
			 {
			    
				 if(pwwc < (rcfs*2))
				 {
					
					if( g_DI.Pa[0].Data.RES_Rear <63)
					{
						
						ATT_Senddata( g_DI.Pa[0].Data.RES_Rear+2 );	
						
					}					 
				 }
				 else
				 {
					 if( g_DI.Pa[0].Data.RES_Rear <63)
					 {
						ATT_Senddata( g_DI.Pa[0].Data.RES_Rear+1 );	
					 }
				 }
				 
				    
				PowerChanging = 1;
			 }
			 else if( pwwc > rcha )
			 {
				
				 if( pwwc > (rcha*2 ))
				 {
					ATT_Senddata( g_DI.Pa[0].Data.RES_Rear-2 );
				 }
				 else
				 {
					ATT_Senddata( g_DI.Pa[0].Data.RES_Rear-1 );
				 }
				
				PowerChanging = 1;
			 }
		 }

		 if( (pwwc < (rcfs*1.2) || pwwc > rcha)  && GateVolCheck() )
		 {
			 PowerChanging = 1;
		 }

		 if( PowerChanging == 1 )
		 {
		 	 User_Timer[POWERAUTOTIMER].Timer_Out_Count = 30; 
		     User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
			 User_Timer[POWERAUTOTIMER].Timer_Enable = 1;
		 }

		 if( (pwwc > rcfs && pwwc < (rcha*0.7) ) )
		 {
		 	 User_Timer[POWERAUTOTIMER].Timer_Out_Count = 30; 
		     User_Timer[POWERAUTOTIMER].Timer_Timeout = 0;
			 User_Timer[POWERAUTOTIMER].Timer_Enable = 0;
			 

  			 if( PowerChanging == 1 )
			 {
			     g_DI.Cfg.P_Res_Front_Target = g_DI.Pa[0].Data.RES_Rear;
				 PowerChanging = 0;	
			 }

		 }
		 

	} 

UPDATEEND:

	if( ((g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_OVERLOAD) ||
		(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_OVERTEMP)	||
		(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_RFHEIGHT) 
		) 
//		&&g_DI.Cfg.P_AutoContrl !=0
	) //如果已有电流保护关机
	{
		 //延时开机定时器到则重新开机
		if( (g_DI.Pa[0].Data.I_OverLoadRestartCount < g_DI.Cfg.CurRstNum) &&   //小于重启最大次数
		     User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout  &&	   //重启时间到
		     g_DI.Pa[0].Data.PA_TPT < g_DI.Cfg.OpenFan1_TPTlimit		   //功放温度小于门限1温度，重新开
		)					  
		{
		     User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
		     AP_Startup();
			 g_DI.Pa[0].Data.I_OverLoadRestartCount++;
			 g_DI.Pa[0].Data.Protect_Shutdown_Cause = 0x00;

		}
	}
	else if(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_PAVCCERR)
	{
		 //延时开机定时器到则重新开机
		if( (g_DI.Pa[0].Data.I_OverLoadRestartCount < g_DI.Cfg.CurRstNum) &&   //小于重启最大次数
		     (User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout)  &&	   //重启时间到
		     (g_DI.Pa[0].Data.PAVCC_Status == 0x00)		   //功放电压正常，重新开
		)					  
		{
		     User_Timer[OVERLOADRESTARTTIMER].Timer_Timeout = 0;
		     AP_Startup();
			 g_DI.Pa[0].Data.I_OverLoadRestartCount++;
			 g_DI.Pa[0].Data.Protect_Shutdown_Cause = 0x00;

		}
	}


	/*风扇控制*/ 
	if( FAN_ManCtrl_Delay == 0 )
	{  
		if(g_DI.Pa[0].Data.PA_TPT > g_DI.Cfg.OpenFan1_TPTlimit)//开启风扇1温度条件
		{
		    if( FAN_TEMP_Counter<20)
			    FAN_TEMP_Counter++;
		}
		else if(g_DI.Pa[0].Data.PA_TPT <= g_DI.Cfg.CloseFan_TPTlimit)//关闭风扇温度条件
		{
			FAN_TEMP_Counter = 0;	
		} 

		if( FAN_TEMP_Counter>=20)
		{
			if( g_DI.Pa[0].Data.PA_TPT > g_DI.Cfg.OpenFan1_TPTlimit)
			{
				SW_FAN1_ON;
				if(g_DI.Pa[0].Data.PA_TPT > g_DI.Cfg.OpenFan2_TPTlimit)//开启风扇2温度条件
				{
					SW_FAN2_ON;
				}
			}
		}
		else if( FAN_TEMP_Counter==0)
		{
		    SW_FAN1_OFF;
			SW_FAN2_OFF;
		}
	}
	
	
	/*工作状态*/
	if( g_DI.Cfg.P_AutoContrl ==1 )
	{
		g_DI.Pa[0].Data.WorkState |= 0x01<<WS_POWER_AUTO;
	}
	else
	{
		g_DI.Pa[0].Data.WorkState &= ~(0x01<<WS_POWER_AUTO);
	}

	if( g_DI.Cfg.P_AutoContrl ==0 )
		g_DI.Pa[0].Data.WorkState |= 0x01<<WS_HALF_AUTO;
	else
		g_DI.Pa[0].Data.WorkState &= ~(0x01<<WS_HALF_AUTO);
	
	
	/////////////////////////////
	if( StartupCtrlDelay < STARTUPDELAYCONTER)
	    g_DI.Pa[0].Data.WorkState |= 0x01<<WS_STARTUP_ING;
	else
	    g_DI.Pa[0].Data.WorkState &= ~(0x01<<WS_STARTUP_ING);

	if( PowerChanging == 1 )
	    g_DI.Pa[0].Data.WorkState |= 0x01<<WS_PWCHG_ING;
	else
	    g_DI.Pa[0].Data.WorkState &= ~(0x01<<WS_PWCHG_ING);

	if( g_DI.Pa[0].Data.Power_FR < 5 )
	    g_DI.Pa[0].Data.WorkState |= 0x01<<WS_NO_RF_OUT;
	else
	    g_DI.Pa[0].Data.WorkState &= ~(0x01<<WS_NO_RF_OUT);
	/////////////////////////////

	if( SwControlGet(SW_FAN_PORT,SW_FAN1_PIN) )
	    g_DI.Pa[0].Data.DetState |= 0x01 <<CHK1_FAN1_START;		
	else
	    g_DI.Pa[0].Data.DetState &= ~(0x01 <<CHK1_FAN1_START);

	if( SwControlGet(SW_FAN_PORT,SW_FAN2_PIN) )
	    g_DI.Pa[0].Data.DetState |= 0x01 <<CHK1_FAN2_START;		
	else
	    g_DI.Pa[0].Data.DetState &= ~(0x01 <<CHK1_FAN2_START);

	if( GateVolCheck() == 1 )
	    g_DI.Pa[0].Data.DetState |= 0x01<<CHK1_GATE_STATUS;
	else
	    g_DI.Pa[0].Data.DetState &= ~(0x01<<CHK1_GATE_STATUS);

	if(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_PAVCCERR)
	    g_DI.Pa[0].Data.DetState |= 0x01 <<CHK1_POWER_ERROR;		
	else
	    g_DI.Pa[0].Data.DetState &= ~(0x01 <<CHK1_POWER_ERROR);

	if( g_DI.Pa[0].Data.PA_TPT > g_DI.Cfg.PWRRED_TPTlimit )
	    g_DI.Pa[0].Data.DetState |= 0x01 <<CHK1_PATEMP_HEIGHT;		
	else
	    g_DI.Pa[0].Data.DetState &= ~(0x01 <<CHK1_PATEMP_HEIGHT);

	if( g_DI.Pa[0].Data.Power_FR < (g_DI.Cfg.PWR_RateOut/2) )
	    g_DI.Pa[0].Data.DetState |= 0x01 <<CHK1_PWLOW_REAL;	
	else
	    g_DI.Pa[0].Data.DetState &= ~(0x01 <<CHK1_PWLOW_REAL);
 
   //////////////////////////////////////

    if(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_OVERLOAD)
	    g_DI.Pa[0].Data.ProState |= 0x01 <<CHK2_I_PROTECTED;
	else
	    g_DI.Pa[0].Data.ProState &= ~(0x01 <<CHK2_I_PROTECTED);
		
	if(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_OVERTEMP)
	    g_DI.Pa[0].Data.ProState |= 0x01 <<CHK2_TEMP_PROTECTED;
	else
	    g_DI.Pa[0].Data.ProState &= ~(0x01 <<CHK2_TEMP_PROTECTED);	
		
	if(g_DI.Pa[0].Data.Protect_Shutdown_Cause & 0x01<<PS_RFHEIGHT)
	    g_DI.Pa[0].Data.ProState |= 0x01 <<CHK2_FR_PROTECTED;
	else
	    g_DI.Pa[0].Data.ProState &= ~(0x01 <<CHK2_FR_PROTECTED); 

	if( g_DI.Pa[0].Data.P_DownOffset == 0)
	{
	    g_DI.Pa[0].Data.ProState &= ~(0x01 <<CHK2_I_POWERDOWN);
	    g_DI.Pa[0].Data.ProState &= ~(0x01 <<CHK2_TEMP_POWERDOWN);
		g_DI.Pa[0].Data.ProState &= ~(0x01 <<CHK2_FR_POWERDOWN);
	}

	g_DI.Pa[0].Data.ProState &= ~(0x01<<CHK2_HARD_IN_HEIGHT);
	g_DI.Pa[0].Data.ProState &= ~(0x01<<CHK2_HARD_I_HEIGHT);
	g_DI.Pa[0].Data.DetState &= ~(0x01<<CHK1_HARD_RF_HEIGHT);

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
	if(( g_DI.Pa[0].Data.ProState & 0x04 ) == 0x04)
	{
		g_DI.Pa[0].State.RV_Alm = 1;		
	}
	else 
	{
		g_DI.Pa[0].State.RV_Alm = 0;
	}
}













