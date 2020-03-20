#ifndef __GXCOMMAND_H
#define __GXCOMMAND_H


//设备功能命令
#define CMD_READ	0x03	//读命令
#define CMD_WRITE	0x10	//写命令
#define CMD_SEARCH	0x5A	//搜索命令
#define CMD_WRITEERR	0x90	//写错误响应帧体
#define CMD_AUTOSEND	0x65	//主动上报帧
#define CMD_REMOTELOGOIN	0x6A	//远程登录
//设备基础信息								    
#define	INFO_STATIONINFO		0x0001 	//地点(台站)编码			
#define INFO_DEVICE_NAME		0x0002 	//设备名称
#define INFO_HARDWARE_VERSION	0x0003 	//设备硬件版本号
#define INFO_SOFT_VERSION	0x0004 	//设备软件版本号
#define INFO_MANUFACTORY		0x0005	//生产厂家
#define INFO_PART_NO		0x0006	//产品型号		
#define INFO_SERIAL_NO		0x0007	//产品序列号		
#define INFO_MCSCPU_SerialNO 	0x000C	//主控CPU序列号
//设备通信接口
#define INFO_DEVICE_ID		0x0020	//本设备ID
#define INFO_MACADDR		0x0021	//本设备MAC地址
#define INFO_DHCPEN			0x0022	//DHCP使能
#define INFO_IP_ADDR		0x0023	//本设备IP地址
#define INFO_MASK_ADDR		0x0024	//子网掩码
#define INFO_GATEWAY_ADDR	0x0025	//默认网关
#define INFO_IP_LOCAL1	0x0026	//本地管理1-IP		 
#define INFO_IP_LOCAL2	0x0027	//本地管理2-IP

#define INFO_IP_INSERVER1	0x0028	//内部联动1-IP
#define INFO_IP_INPORT1	0x0029	//内部联动1-端口号
#define INFO_IP_INSERVER2	0x002A	//内部联动2-IP
#define INFO_IP_INPORT2	0x002B	//内部联动2-端口号
	
#define INFO_IP_SERVER1	0x0030	//远程主机1-IP
#define INFO_Net_Port1	0x0031	//远程主机1-端口号
#define INFO_FActive_Time1	0x0032	//第1组主动间隔1
#define INFO_FActive_Time2	0x0033	//第1组主动间隔2	
#define INFO_FActive_Time3	0x0034	//第1组主动间隔3
		 
#define INFO_IP_SERVER2	0x0035	//远程主机2-IP
#define INFO_Net_Port2	0x0036	//远程主机2-端口号
#define INFO_SActive_Time1	0x0037	//第2组主动间隔1
#define INFO_SActive_Time2	0x0038	//第2组主动间隔2	
#define INFO_SActive_Time3	0x0039	//第2组主动间隔3

#define INFO_IP_SERVER3	0x003A	//远程主机3-IP
#define INFO_Net_Port3	0x003B	//远程主机3-端口号
#define INFO_TActive_Time1	0x003C	//第3组主动间隔1
#define INFO_TActive_Time2	0x003D	//第3组主动间隔2	
#define INFO_TActive_Time3	0x003E	//第3组主动间隔3
#define INFO_IPFilteringe_En	0x003F	//IP过滤使能
//设备整机控制
#define INFO_MACH_SETON	0x0101	//开机(专用)
#define INFO_MACH_SETOFF	0x0102	//关机(专用)
#define INFO_MACH_BITOP	0x0103	//位操作(开\合\置位)

#define INFO_MACH_ATT1	0x0121	//衰减器1
#define INFO_MACH_ATT2	0x0122	//衰减器2
#define INFO_MACH_NETINIT	0x0123	//网络初始化
#define INFO_MACH_CTR	0x0128	//整机控制位
#define INFO_MACH_FANCTR	0x0129	//整机风扇控制位
#define INFO_MACH_COMCTR	0x012A	//整机通信控制位
#define INFO_MACH_RSTFACTORYDATA	0x012F	//整机恢复出厂设置 1字节 0x01:生效

//报文设置
#define INFO_AutoMsg_Cfg1	0x01F8	//第1组自动报文设置
#define INFO_AutoMsg_Cfg2	0x01F9	//第2组自动报文设置
#define INFO_AutoMsg_Cfg3	0x01FA	//第3组自动报文设置
#define INFO_AutoMsg_INCfg1	0x01FB	//内部联动报文1设置
#define INFO_AutoMsg_INCfg2	0x01FC	//内部联动报文2设置

//整机状态数据和参数设置
#define INFO_AllState_cWork	0x0200	//整机工作状态
#define INFO_PWR_RFOTU	0x0201	//发射机输出功率,W
#define INFO_PWR_RV		0x0202	//发射机反射功率,W
#define INFO_ALL_VOL	0x0203	//发射机功放电压,V
#define INFO_ALL_CUR	0x0204	//发射机功放电流,A
#define INFO_PWR_SWR	0x0205	//发射机驻波比
#define INFO_TPT_CPU	0x0206	//CPU温度,℃
#define INFO_TPT_EVM	0x0207	//环境温度,℃
#define INFO_AllState_Work	0x0210	//整机工作状态
#define INFO_AllState_Det	0x0211	//整机检测状态
#define INFO_AllState_Pro	0x0212	//整机保护状态
#define INFO_AllPWR_Int	0x0220	//功率初始化,W
#define INFO_SETPWR_Rated	0x0221	//设定输出功率,W
#define INFO_INPWRLimit_Min	0x0222	//输入功率最小值,dBm
#define INFO_INPWRLimit_Max	0x0223	//输入功率最大值,dBm
#define INFO_RVPWRLimit_Max	0x0224	//反射功率最大值,W
#define INFO_CURLimit_RedPwr	0x0225	//降功率功放电流门限,A
#define INFO_CURLimit_OFFPwr	0x0226	//关机功放电流门限,A
#define INFO_CURPWRRST_Time	0x0227	//过流重启判断延时,秒
#define INFO_CURPWRRST_Num	0x0228	//过流重启次数,次
#define INFO_TPTLimit_RedPwr	0x0229	//降功率功放温度门限,℃
#define INFO_TPTLimit_OFFPwr	0x022A	//关机功放温度门限,℃
#define INFO_TPT_OpenFan1	0x022B	//开风扇 1 的温度,℃
#define INFO_TPT_OpenFan2	0x022C	//开风扇 2 的温度,℃
#define INFO_TPT_CloseFan	0x022D	//关风扇的温度,℃
#define INFO_TPTRST_Time	0x022E	//过温重启判断延时,秒
#define INFO_INPWR_CRTVAL	0x023C	//输入功率校正值
#define INFO_OUTPWR_CRTVAL	0x023E	//输出功率校正值
#define INFO_RVPWR_CRTVAL	0x023F	//反射功率校正值


#define INFO_ExciterCom_State	0x0213	//激励器模块通信状态 bit0:主激励器通信， 1 通信正常， 0 为通信中断
										//bit1:备激励器通信， 1 通信正常， 0 为通信中断
#define INFO_PaCom_State	0x0214	//功放模块通信状态，bit0:模块1，1 通信正常，0为通信中断
										//bit1:模块2，1 通信正常，0为通信中断	...
#define INFO_PowerSupplyCom_State	0x0215	//电源模块通信状态
//0号功放模块数据
#define INFO_PA_ITT		0x1000	//功放模块说明
#define INFO_PAPWR_RFOTU		0x1001	//功放输出功率,W, 2字节
#define INFO_PAPWR_INOTU		0x1002	//功放输入功率,W或dBm, 2字节
#define INFO_PATPT		0x1003	//功放温度,℃,float
#define INFO_PA_State		0x1004	//功放状态 bit0:过载 ->0:否 1：是   bit1:过激励 ->0:否 1：是
									//bit2:过热 ->0:否 1：是
#define INFO_PA_TDVCC		0x101E	//推动管工作电压, V, float
#define INFO_PA_TDCUR		0x101F	//推动管电流, A, float
#define INFO_PA_GLVCC		0x1020	//功率管工作电压, V, float
#define INFO_PA_GLCUR1		0x1021	//功率管电流1, A, float
#define INFO_PA_GLCUR2		0x1022	//功率管电流2, A, float
#define INFO_PA_CPUNO		0x103E	//CPU序列号
#define INFO_PA_FanSpeed1		0x103F	//风扇转速1
#define INFO_PA_FanSpeed3		0x1040	//风扇转速2
#define INFO_PA_FanSpeed5		0x1041	//风扇转速3
#define INFO_PA_FanSpeed4		0x1042	//风扇转速4
//主用激励器数据（ 0xB000-0xB02F）
#define INFO_ExciterDirection 	0xB000	//激励模块说明
#define INFO_ExciterPwr_OUT 	0xB001	//激励器输出功率,W
#define INFO_ExciterPwr_RV 	0xB002	//激励器反射功率,W
#define INFO_ExciterOut_Frq 	0xB003	//激励器输出频率,KHz
#define INFO_Exciter_TPT 	0xB004	//激励器温度,℃
#define INFO_ExciterL_Mod 	0xB005	//L调制度,%
#define INFO_ExciterR_Mod 	0xB006	//R调制度,%
#define INFO_Exciter_ModMode 	0xB007	//调制模式，0:关闭 1:立体声 2:L单声道 3:R单声道 4:L+R单声道
#define INFO_Exciter_PreEmp 	0xB008	//预加重 0:0us 1:50us 2:75us
#define INFO_ExciterL_Gain 	0xB009	//L 声道音频增益
#define INFO_ExciterR_Gain 	0xB00D	//R 声道音频增益
#define INFO_ExciterPWR_Gain 	0xB00E	//功率增益
#define INFO_ExciterPWR_SW 	0xB010	//功放开关 0:关 1： 开
#define INFO_ExciterCPU_SerialNO 	0xB01E	//CPU序列号
#define INFO_ExciterMODULATIONVAL 	0xB020	//调制度0~255
#define INFO_ExciterRDSVAL 	0xB021	//RDS幅度
#define INFO_ExciterCRMODULATIONVAL 	0xB022	//调制度0~7
#define INFO_ExciterOut_FrqMHZ 	0xB023	//激励器输出频率,MHz
#define INFO_ExciterRDSDATA 	0xB025	//RDS数据 8Byte
#define INFO_ExciterRDSBLOCKDATA	0xB026	//RDS块数据 8*nByte
#define INFO_ExciterRDSCHN	0xB027	//RDS通道选择 1Byte
#define INFO_ExciterMODULDCT	0xB028	//调制度监测 float
#define INFO_ExciterDIFMODULDCT	0xB029	//差项调制度 float
#endif






