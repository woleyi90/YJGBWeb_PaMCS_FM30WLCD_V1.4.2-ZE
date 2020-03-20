/*******************************************************************************
 *
 * Filename  : RS485.h
 *
 * Function  : Reference head file for "RS485" library.
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

#ifndef __RS485_H
#define __RS485_H


#define HEAD_CODE	0x7e
#define TAIL_CODE	0x7e

#define MODEM_HEAD_CODE 0x3A
#define MODEM_TAIL_CODE 0x3A

/* 设备 状态枚举 */
typedef enum
{
	eDev_NO_ERR = 0x00,			// 成功
	eDev_CRC_ERR = 0x01,		// CRC错
	eDev_CMD_ERR = 0x02,		// 命令错								
	eDev_Dat_ERR = 0x03,		// 数据错															
	eDev_Opr_ERR = 0x04,		// 操作失败
	eDev_TimeOut_ERR = 0x05,	// 连接超时
	eDev_RxNone_ERR = 0x06		// 无接收到数据
}eDev_Err ;

/* RS485协议功能编码 */
#define DEV_FUN			0x00	// 配置模块
#define ALL_FUN         0x01    // 整机模块
#define PA_FUN			0x08	// 功放
#define SWI_FUN         0x22    // 切换器
#define POW_FUN			0x23	// 电源模块
#define DPD_FUN         0x24    // DPD模块
#define EXCIT_FUN		0x27    // 调频激励器
#define CTTB0_FUN       0x21    // 激励器
#define CTTB1_FUN       0x26    // 激励器


/* RS485协议命令 */
#define CMD_SETID		0x10	// 设置模块地址
#define CMD_QUERY_STATE	0x11	// 查询模块状态
#define CMD_QUERY_VER	0x12	// 查询模块软件版本号
#define CMD_QUERY_ID    0x13    // 查询模块地址
#define CMD_RW_SN       0x14    // 读写产品序列号 

#define CMD_QUERY_INFO  0x15    // 查询设备信息
#define CMD_SET_INFO    0x16    // 设置设备信息

#define CMD_SET_ATT		0x20	// 设置ATT
#define CMD_SET_FQC		0x21	// 设置频率
#define CMD_SET_SW		0x22	// 设置开关
#define CMD_RW_TIME		0x23	// 定时开关机设置
#define CMD_RW_POW		0x31	// 读取/设置前/反向功率表
#define CMD_RW_POW_LEN	0x32	// 读取/设置前/反向功率表长度

#define CMD_RST			0x88	// 模块复位

#define CMD_SET_PLIMIT	0xE0	// 设置功率门限
#define CMD_RD_PLIMIT	0xE1	// 查询功率门限
#define CMD_SET_OCLIMIT 0xE4    // 设置过电流门限
#define CMD_RD_OCLIMIT  0xE5    // 查询过电流门限
#define CMD_RW_OTLIMIT  0xE9    // 读写过温度门限
#define CMD_RW_SWVAL   	0xDF      // 驻波关断参考电压设置

#define CMD_RD_TPTCMPST		0xEE	// 读温度补偿表
#define CMD_GAIN_TPTCMPST	0xEF	// 增益温补
#define CMD_FAN_ONF			0xF5	// 风扇与风机开启关闭指令

#define CMD_RD_PVOL		0xE2	// 查询功率电压值
#define CMD_WR_PDATA	0xE3	// 设置功率表
#define CMD_RD_PDATA	0xE8	// 读取功率表
#define CMD_SET_ECT_MOD	0xF0	// 设置激励器控制模式
#define CMD_SET_FANSW	0xF1	// 设置风机开关
#define CMD_RF_BOUND	0x52	// 读/写总输出和反射功率门限
#define CMD_SET_ECTSFNDT	0x58	// 设置激励器SFN延时时间
#define CMD_SET_ECTFQC	0x59	// 设置激励器中心频率
#define CMD_SET_ECTPOW	0x5A	// 设置激励器输出功率
#define CMD_RW_PHONE	0x5C	// 读/写电话号码
#define CMD_RW_COM		0x5D	// 读/写通信参数
#define CMD_RW_COMMODE	0x5E	// 读/写远程通信方式

#define CMD_RW_ALC_VOL  0x50    // 读/写ALC电压值
#define CMD_RW_ATT_VOL  0x51    // 读/写增益电压值
#define CMD_RF_BOUND    0x52    // 读/写功率门限
#define CMD_RW_ATT      0x54    // 读/写ATT值
#define CMD_RW_ALC      0x55    // 读写ALC值

#define CMD_RW_GAINMAX  0x4D    // 读/写标称增益

#define CMD_QUERY_DPDSTATE  0x11  // 查询DPD模块状态
#define CMD_RW_DPDATT       0X20  // 读写ATT
#define CMD_RW_DPDMOD       0X70  // 读写DPD信息型号
#define CMD_DP_SW           0x71  // 数字预失真开关
#define CMD_DPD_SW          0x72  // 数字预失真自动校正开关
#define CMD_CFR_SET         0x73  // 波峰消除参数设置
#define CMD_RW_DPDLP        0x74  // 设置输入信号欠功率告警门限命令

#define CMD_RF_AGCONF	0x53	// 整机AGC功能的开关命令

#define CMD_INIT_PWR		0x61	// 初始化功率
#define CMD_SET_WORKMODEL		0x62	// 设置功放工作模式
#define CMD_SET_OPRDATA		0x63	// 设置功放运行参数

#define PARS232_SendBuf(Data, Len)		UART5Write(Data, Len)

eDev_Err PA_OptPmt_ProcessAgain(void);		  
void CMD_BC_Analys(char *recdata,u8 reclen,char *outdata,u16 *outlen);
u16 Protocol_Pack_Process(u8 *PackBuf, u16 PackLen, u8 Head);
eDev_Err RS485_Tx(u8 *txBuf, u8 Buflen, u16 timeout);
eDev_Err PA_RS232_GXTx(u8 *tDev_Data,u8 length,u16 timeout);
eDev_Err Excit_RS232_Send(u8 *tDev_Data,u8 length,u16 timeout); 
bool EXCIT_Wait_RxEnd(u16 timeout);
u16 CMD_GetParmaData(u16 parCode, u8 *TXBuf);
#endif

