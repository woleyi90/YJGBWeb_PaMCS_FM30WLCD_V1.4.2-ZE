/*******************************************************************************
 *
 * Filename  : structure.h
 *
 * Function  : Reference head file for "STRUCTURE" library.
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

#ifndef __STRUCTURE_H
#define __STRUCTURE_H
   



#define	MAX_UART_BUF_LEN	255				// 串口接收buf大小
#define	MAX_DMA_BUFLEN		500				// DMA接收buf大小
#define	MAX_UART_TimeOut	3				// 串口接收限时,单位 30ms
#define	MAX_UART2_TimeOut	20				// 串口2接收限时,单位 50ms
#define	MAX_UART3_TimeOut	20				// 串口3接收限时,单位 50ms
#define Max_Pa				1				// 功放个数	  

#define MAX_PrArg_LEN		25				// 功率表最大长度 
#define MAX_TptArg_LEN	    25
#define DATALEN_MAX			200				// RS485命令体最大长度
#define NO_FUNID			0xFF            // 无RTU　ID需要进行处理
#define MB_DevProNONE		0xFF            // MODBUS无操作标志

#define	RSphone_MAX	    	5		// 查询设置电话号码个数
#define	Phone_LEN		    20		// 电话号码最大长度
#define Save_Size		sizeof(DI_CFG)		// 保存到FLASH大小
#define AutoMsgSave_Size		sizeof(AutoMsg_CFG)		// 保存到FLASH大小
  

/* 功率参数结构化 */
typedef struct
{
	s8 dBm;		// 功率dBm值
	u16 Vol;	// 对应电压AD值
}Pr_Struct;

/* 温度参数结构化 */ 
typedef struct
{
	s8 temp;		// 温度值
	u16 Vol;		// 对应电压AD值
}Tp_Struct;

/* 查询设置上报电话号码 */
typedef struct
{
	u8	RPphone[Phone_LEN];					// 上报告警电话
	u8	RSphone[RSphone_MAX][Phone_LEN];	// 查询设置电话
}Phone_Struct;


typedef struct
{
	u8  En;
	u16 Year;
	u8  Mon;
	u8  Day;
	u8  Hor;
	u8  Min;
	u8  Sec;

}TimeOnOff_Struct;


/* 系统配置结构体 存放系统的配置信息,该部分信息需保存到EEPROM或FLASH */
typedef struct
{
	volatile u8 	ICON;					// 系统标志字
	volatile u8		FunID;					// 功能代码
	volatile u8		ID;						// ID	   
	volatile u8		OLDID;						// ID	 	 
	
	volatile u8		IP_Server[4];			// 目标服务器通信IP地址
	volatile u8		IP_Addr[4];				// 源网络通信IP地址
	volatile u8		IP_Mask[4];				// 子网掩码
	volatile u8		IP_GateWay[4];			// 默认网关
	volatile u8		IP_DNS[4];				// DNS服务器IP
	volatile u16		Net_Port;				// 网络通信端口号	 
	
	volatile u8		IP_LOCAL1[4];		// 本地管理1-IP地址	 
	volatile u16		LOCAL1NetPort1;		// 本地管理1-端口号
	
	volatile u8		IP_LOCAL2[4];		// 本地管理2-IP地址	 
	volatile u16		LOCAL1NetPort2;		// 本地管理2-端口号
	
	volatile u8		IP_INSERVER1[4];		// 内部联动1-IP地址	 
	volatile u16		INSERVERNetPort1;		// 内部联动1-端口号
	
	volatile u8		IP_INSERVER2[4];		// 内部联动2-IP地址	 
	volatile u16		INSERVERNetPort2;		// 内部联动3-端口号
	
	volatile u8		IP_RemoteHost1[4];		// 远程主机1-IP地址
	volatile u16		RemoteHostNetPort1;		// 远程主机1-端口号
	volatile u16		RemoteHostDelay1_1;		// 远程主机1-第 1 组主动间隔
	volatile u16		RemoteHostDelay1_2;		// 远程主机1-第 2 组主动间隔
	volatile u16		RemoteHostDelay1_3;		// 远程主机1-第 3 组主动间隔
	
	volatile u8		IP_RemoteHost2[4];		// 远程主机2-IP地址
	volatile u16		RemoteHostNetPort2;		// 远程主机2-端口号
	volatile u16		RemoteHostDelay2_1;		// 远程主机2-第 1 组主动间隔
	volatile u16		RemoteHostDelay2_2;		// 远程主机2-第 2 组主动间隔
	volatile u16		RemoteHostDelay2_3;		// 远程主机2-第 3 组主动间隔
	
	
	volatile u8		IP_RemoteHost3[4];		// 远程主机3-IP地址
	volatile u16		RemoteHostNetPort3;		// 远程主机3-端口号
	volatile u16		RemoteHostDelay3_1;		// 远程主机3-第 1 组主动间隔
	volatile u16		RemoteHostDelay3_2;		// 远程主机3-第 2 组主动间隔
	volatile u16		RemoteHostDelay3_3;		// 远程主机3-第 3 组主动间隔
	
	volatile u8		PA_IPHead;				// 功放设备命令首码
	volatile u8		Excit_IPHead;			// 激励器设备命令首码
	
	volatile u8    	CloseFan_TPTlimit;        //关闭风扇温度
	volatile u8    	OpenFan1_TPTlimit;        //开启风扇1温度
	volatile u8    	OpenFan2_TPTlimit;        //开启风扇2温度
	volatile u8    	PWRRED_TPTlimit;          //降功率温度
	
	volatile float     PWRRED_CURlimit;          //降功率电流
	volatile float     PWROFF_CURlimit;          //过保护电流
	volatile float     PWR_INlimit;         	  //输入最小功率
	
	volatile float     PWR_INlimitMan;         //输入最大功率
	volatile float     PWR_InitOut;         	  //功率初始化输出
	
	volatile u8       	PWR_RateOut;         	  //输出功率
	volatile u8       	PWR_state;         	  //输出额定功率
	volatile u8   		PWROFF_TPTlimit;          //过温关机保护温度 
	volatile u8     	PWR_RVlimit;         	  //反射保护门限
	
	float P_FmFrq;					  		//输出频率值
	float P_InCal;						  //输入功率校正值
	float P_OutCal;						 //输出功率校正系数
	float P_RefCal;						  //反射功率校正系数
	
	//新协议添加
	volatile u8		DeviceID;	//主监控ID，即IP地址的IP_Addr[3]
	volatile u8 	CurRstNum; //过流重启次数
	volatile u8    P_Res_Front_Target;
	volatile u8    PWR_SetRateOut;//输出额定功率
	
	volatile u16 	CurRstDelay; //过流重启判断延时
	volatile u16 	TPTRstDelay; //过温判断延时
	

	
	volatile u16   Temp_ReducePowerInterval;//  = 300 ;      //降功率间隔时间 秒
	volatile u16   Temp_ReducePowerMaxCount;// = 3;	        //降低功率最大次数
	
	volatile u8    P_AutoContrl ;// //自动功率控制  0手动   1自动	
	volatile u8    P_state[2] ;
	volatile u8		StationInfo[17]; //地点(台站)编码
	

	volatile u8		DeviceName[32];  //设备名称
	volatile u8		ManuFactory[32]; //生产厂家
	volatile u8		PaItt[32]; 	//功放模块说明
	volatile u8		ExcitDirection[32]; //激励器模块说明
	volatile u8		PartNo[32];		 //产品型号
	volatile u8		SerialNo[16];	 //产品序列号
	

	
}DI_CFG ;
typedef struct
{	
	volatile u8		AutoMsgICON;
	volatile u8		AutoMsg_Cfg1_Len;
	volatile u8		AutoMsg_Cfg2_Len;
	volatile u8		AutoMsg_Cfg3_Len;
		
	volatile u8		AutoMsg_INCfg1_Len;
	volatile u8		AutoMsg_INCfg2_Len;
	volatile u8		BAK[2]; 

	volatile u16	AutoMsg_Cfg1[50];	//第1组自动报文设置,50个参数地址，一个参数地址2字节
	volatile u16	AutoMsg_Cfg2[50];	//第2组自动报文设置
	volatile u16	AutoMsg_Cfg3[50];	//第3组自动报文设置
	volatile u16	AutoMsg_INCfg1[50];	//内部联动报文1设置
	volatile u16	AutoMsg_INCfg2[50];	//内部联动报文2设置

}AutoMsg_CFG ;		   

/* 整机数据结构体 */			   
	typedef union		// 查询数据联合体
	{
		struct								// 模块状态
		{
			volatile	u32		EN  		:1;			// 开关状态
			volatile    u32		CH	     	:1;			// 主备通道状态	
			volatile    u32		LOAD		:1;			// 负载天馈连接状态
			volatile    u32		RF_Alm 		:1;			// 整机过功率告警 
			volatile    u32		RV_Alm		:1;			// 整机过反射告警					
			volatile    u32		ECT_Alm 	:1;			// 激励器告警
			volatile    u32		FAN_Alm   	:1;			// 风机告警状态
			volatile    u32		POW_Alm     :1;			// 电源告警状态
		}State;
		
		struct
		{
			volatile    u8			State;					// 模块状态	
			volatile    int8_t		ALL_TPT;	      	    // 整机温度			
			volatile    u8          Input_SMA_Flag;         // 联锁开关标志位，0x01表示有效	
			volatile    u8          Input_SMA_ALM;          // 联锁开关告警，0x01表示有效
			volatile    int16_t		Power_OUT;				// 前向功率
			volatile    int16_t		Power_RV;				// 反射功率
			volatile    u32         Freq;                   // 信道中心频率	 		
			volatile    u16			ALL_CUR;				// 功放电流
			volatile    u16			ALL_VOL;				// 功放电压       	
		
		}Data;
	}ALL_Data;
/* 功放模块数据结构体 */
	typedef union		// 查询数据联合体
	{
		struct								// 模块状态
		{
		 	volatile	u32		Enable		:1;			// 功放状态,开关  
			volatile    u32		POW_Alm		:1;			// 输出告警	
			volatile    u32		TPT_Alm		:1;			// 过温告警
			volatile    u32		RV_Alm		:1;			// 过反射告警 
			volatile    u32		TPRED_Alm	:1;			// 温度降功率告警					
			volatile    u32		OC_Alm		:1;			// 过电流告警
			volatile    u32		LP_Alm		:1;			// 输入告警
			volatile    u32		OCRED_Alm  	:1;			// 过流降功率告警
		}State;

		struct
		{
			volatile    u8			State;					// 模块状态	
			volatile    u8			FanState;				// 风扇状态
			volatile    u8			pState0;				// 保留
			volatile    u8			pState1;				// 保留
			volatile    u8			ID;						// 模块地址
			volatile	u8			FunID;					// 功能编码
			volatile	u8			Fun0;					// 编码0
			volatile	u8			Fun1;					// 编码1
			volatile    u8			WorkState;				// 工作模块状态
			volatile    u8			DetState;				// 检测模块状态
			volatile    u8			ProState;				// 保护模块状态
			volatile    u8			ResState;				// 保留位
			volatile    float		Power_FR;				// 入射功率
			volatile    float		Power_RV;				// 反射功率
			volatile    float		CPU_TPT;				// CPU温度
			volatile    float		PA_TPT;					// 功放温度
			volatile    float		ENV_TPT;				// 环境温度
			volatile    float		Power_IN;				// 输入功率		
			volatile    float		PA_VOL;					// 功放电压
			volatile    float		PA_BECUR;				// 前级电流
			volatile    float		PA_CUR1;				// 功放电流1
			volatile    float		PA_CUR2;				// 功放电流2				
			volatile    float		Power_SET;				// 功率设置
			volatile    u16			Serial_Num;			   	//序列号
			volatile    u8			ATT_POT;			   	//第一个衰减电位器
			volatile    u8			ATT_POT1;				//第二个衰减电位器
			volatile 	u16			CRC16_Val;				// CRC校验值  
			volatile    u8			pState2;				// 保留
			volatile    u8			pState3;				// 保留
			volatile    float		PA_ALLCUR;				// 前级电流
			
			volatile    float 		P_InVal;	
			volatile    float 		P_OutVal;
			volatile    float 		P_ReflexVal;
			
			volatile	u8 	I_Status;      //电流状态
			volatile	u8 	I_StatusTrig;  //电流状态触发
			volatile	u8 	I_StatusCont;  //电流状态持续
			volatile	u8 	I_StatusRelease;//电流状态释放 

			volatile	u8 	TEMP_Status;	  //温度状态
			volatile	u8 	TEMP_StatusTrig;	  //温度状态触发
			volatile	u8 	TEMP_StatusCont;	  //温度状态持续
			volatile	u8 	TEMP_StatusRelease;	  //温度状态释放

			volatile	u8 	PAVCC_Status;      //电源状态
			volatile	u8 	PAVCC_StatusTrig;  //电源状态触发
			volatile	u8 	PAVCC_StatusCont;  //电源状态持续
			volatile	u8 	PAVCC_StatusRelease;//电源状态释放 
			
			volatile	u8  RF_Status;
			volatile	u8  RF_StatusTrig;
			volatile	u8  RF_StatusCont;
			volatile	u8 Temp_ReducePowerCount ;//	 = 0;   //温度保护已降低功率次数 
			
			volatile	u8 I_OverLoadRestartCount;//    = 0;	//已过流保护重启计数值
			volatile	u8 Protect_Shutdown_Cause;// = 0;  //保护关机原因;
			volatile	int8_t RES_Front; 
			volatile 	u8 RES_Rear;
			
			volatile 	float P_DownOffset;
			

		

		}Data;
	}PA_Data;


/* 调频激励器数据结构 */
typedef	struct				  
{
	volatile u8		ExcID;					// 模块地址
	volatile u8		FunID;					// 功能编码
	volatile u8		Fun0;					// 编码0
	volatile u8		Fun1;					// 编码1

	volatile u8		Fun2;					// 编码2
	volatile u16	FMFRQ;	//输出频率	76MHz~108MHz
	volatile u8		AssFrqInt;	// 工作辅助频率整数部分

	volatile u8		AssFrqPoint;// 工作辅助频率小数点部分
	volatile u8		FMMODULATIONMODE;	//调制模式 0:关闭 1:立体声 2:L单声道 4:R单声道 6:L+R声道
	volatile u8		FMPre_Emphasis;	//预加重 00：0us  01:50us 
	volatile u8		FMGain_RF;	//RF增益 0~63

	volatile u8		FMMODULATIONVAL;//调制度0~255
	volatile u8		FMRDS_CHIN;	//RDS输入信号选择  0:串口输入；1:模拟信号输入
	volatile u8		FMRDSVAL;	//RDS幅度 0~15 0:无载波
	volatile u8		FMCALFRQ_ENABLE;	//频率校正使能。1:有效  0:无效

	volatile int	FMOSCFRQ;//晶振频率	
	volatile int	FMCALFRQ_VAL;//频率校正值
	volatile u8		RDSDATA[8];
	volatile int	FMFRQDCT;	//输出频率检测
	volatile int	FMOSCFRQDCT;	//晶振频率检测
	volatile int	FMMainChannelMean;//主信道均值
	
	volatile u8		FMCALFRQ_SIGN;//频率字校正符号位 0：加；1：减
	volatile u16	ModulationDCT;				//监测调制度
	volatile u8		RDSID;
	
	volatile u16	DifModulation;		// 副信道（差项）调制度
	volatile u16	LeftChnModuVal;		// 左声道调制度
	
	volatile u16	RightChnModuVal;	// 右声道调制度
	volatile u16	CRC_Val;	
	
	volatile u8		FMSIGNAL_VAL;				// 保留
	volatile u8		eState0;				// 保留
	volatile u16	ChnModuState;
}Excit_Data;	
/* 时间状态结构体 */
typedef struct
{
	u32 HoCnt;		// 小时计数
	u32 SeCnt;		// 秒计数
	u32 MiCnt;		// 分计数
}T_Struct;



/* 切换器模块查询数据结构体 */
typedef union		// 查询数据联合体
{
	struct									// 模块状态
	{
		u32		Alm			:1;			// 模块状态
		u32		BState		:1;			// 通道B状态
		u32		AState		:1;			// 通道A状态
		u32		ECT_CH		:1;			// 激励器通道A/B
		u32		ECT_MOD		:1;			// 激励器控制模式,自动/手动
		u32		State6		:1;			// 保留
		u32		State7		:1;			// 保留
		u32		State8		:1;			// 保留
	}State;
		
	struct
	{
		u8			State;				// 模块状态	
		u8			swiState0;				// 模块状态
		u8			swiState1;				// 模块状态
		u8			swiState2;				// 模块状态	
		int16_t		Power_In;			// 输入功率
		int16_t		Power_Out;			// 输出功率		 
	}Data;
}SWI_Data;


/* 系统状态 联合体 */
typedef union
{
	u32		ALL ;						// 用于全部赋值
	struct
	{ 
		volatile    u32     fWdgClr     : 1	;     // 喂狗标志															
		volatile	u32		fMS 		: 1 ;     // 10毫秒标志 每秒置位该位一次
		volatile	u32		fSec 		: 1 ;	  // 秒标志 每秒置位该位一次
		volatile	u32		fMin 		: 1 ;	  // 分标志 每分置位该位一次
		volatile	u32		fHour 		: 1 ;	  // 小时标志 每小时置位该位一次
		volatile	u32		fDOM 		: 1 ;	  // 天标志 每天置位该位一次 
		volatile    u32     fNetLink    : 1 ;     // 网络连接标志	
		volatile    u32     fSaveStart  : 1 ;     // 开机标志
				
		volatile    u32     fNetRecom   : 1 ;     // 重连标志
		volatile    u32     fInputAlm   : 1 ;     // 输入故障标志
		volatile    u32     fNetTime05s : 1 ;     // 网络0.5秒事件标志
		volatile    u32     fNetTime10s : 1 ;     // 网络10秒事件标志
		volatile    u32     fETH_INT    : 1 ;     // 网络接收定时标志  
		volatile    u32     fAlmOFF	    : 1 ;     // 故障关机标志

		volatile	u32		fDOMEvent	: 1 ;	  // 天处理事件标志
		volatile	u32		fSenEvent	: 1 ;     // MODEM秒事件
		volatile	u32		fMinEvent	: 1 ;	  // 分钟处理标志
		volatile	u32		fHourEvent	: 1 ;	  // 小时处理标志
		volatile    u32     fAutoMsgSave : 1 ;    // 数据保存标志
		
		volatile    u32     fPaRtudata    : 1 ;     // 功放RTU数据发送标志
		volatile    u32     fSave       : 1 ;     // 数据保存标志
		volatile    u32     fEitRtudata : 1 ;   // 调制板RTU数据发送标志
		volatile    u32     f3sec       : 1 ;     // 3秒定时标志		
		volatile    u32     fPaON      : 1 ;   // 功放开机标志
		volatile    u32     fPaOFF      : 1 ;   // 功放关机标志
		volatile    u32     fPaTimer      : 1 ;   // 功放时间处理
		volatile    u32     fPaCheck      : 1 ;   // 功放命令首码校验位
		volatile    u32     fPanetQue     : 1 ;   // 功放以太网参数查询标志位
		volatile    u32     fExcitCheck   : 1 ;   // 激励器命令首码校验位
		volatile    u32     fstatebk   : 1 ;   	// 保留位

	}Bit ;	
}Sys_State ;
typedef union
{
	u32		ALL ;						// 用于全部赋值
	struct
	{ 
		volatile    u32     fNetConfig    : 1 ;   // 网络配置标志
		volatile    u32     fUdpRecom    : 1 ;   // UDP重新配置标志
		volatile    u32     fWebsave    : 1 ;   // web页面修改参数标志
		volatile    u32     fPaOutCrt    : 1 ;   // 功放输出校正标志
		volatile    u32     fPaInCrt    : 1 ;   // 功放输入校正标志
		volatile    u32     fPaRvCrt    : 1 ;   // 功放反射校正标志
	}Bit ;	
}SysNet_State ;


/* 系统数据结构体 */
typedef struct
{          	
	 volatile DI_CFG		Cfg ;			    // DI配置
	 volatile AutoMsg_CFG	AutoMsgCfg ;		// DI配置
	 volatile ALL_Data      ALL;                // 整机数据
	 volatile PA_Data		Pa[Max_Pa];			// 功放数据
	 volatile Excit_Data	Excitdata;			// 调制板数据
	 volatile u8            MAC[6];             // MAC地址
	 volatile u8         	ID_Cur;             // 485通信当前ID号
	 volatile u8         	CMD_Cur;            // 当前命令	  
 	
}System ; 
/*状态数据结构*/
typedef struct DeviceDatas
{
	u32 rtctime;
	
	//主动包定时发送变量
	u32 S1G1ST;
	u32 S1G2ST;
	u32 S1G3ST;
	
	u32 S2G1ST;
	u32 S2G2ST;
	u32 S2G3ST;
	
	u32 S3G1ST;
	u32 S3G2ST;
	u32 S3G3ST;
}DeviceDatas;

/* 串口接收结构体 */
typedef struct
{
	u8 Buff[MAX_UART_BUF_LEN] ;			// 接收缓冲区
	u16 Len ;							// 接收长度
	u16 TimeOut ;						// 接收超时限制,该计数值为0时,标志接收结束
	bool fEnd	;						// 接收结束标志 1,接收结束  0,接收中
}UART_RBuf ;

/* RS485通信数据结构体 */
typedef struct
{
	u8 Head_Code;	// 头码
	u8	FunID;		// 功能编码
	u8	ID;			// 模块ID
	u8	CMD;		// 命令
	u8	ACK;		// 应答标志
	u8	DataLen;	// 命令体长度
	u8	DataBuf[DATALEN_MAX];	// 命令体,校检,尾码(变长)
}Dev_Data;

typedef struct
{
	u8 	Device_ID;	// 设备ID
	u8	FunID;		// 功能编码
	u8	DataLenL;	// 数据长度低八位
	u8	DataLenH;	// 数据长度高八位
	u8	DataBuf[DATALEN_MAX];	// 数据体,校检码
}Dev_GXData;
/* RS485通信数据包结构体 */
typedef struct
{
	Dev_Data 	Dev_Dat;
	u8		    Len;			// 数据包长度
}RS485_Dat;

/* MODEM状态结构体 */
typedef struct
{
	u8 HourCnt;		// 小时计数
	u8 SecCnt;		// 秒计数
	u8 MinCnt;		// 分计数
	u8 ModemState;	// MODEM状态
	bool  fInitStart;	// 启动一次初始化标志
	bool  fReadModem;	// 定时读取短信标志
	u8 cnt_CSQ;
}Modem_Struct;

/*自定义软件定时器*/
typedef struct MYTIMER
{
    u8 Timer_Enable ;
	u16 Timer_Count ;
	u32 Timer_Out_Count ;
	u8 Timer_Timeout;
}MYTIMER;



#endif

