#ifndef _DPD_H
#define _DPD_H






#define RD_CMD	0x01		// DPD模块读命令
#define WR_CMD	0x02		// DPD模块写命令

#define DPD_RSInPWR_L		0
#define DPD_RSInPWR_H		1
#define DPD_IN_PWR			2

#define DPD_RS485_HEAD      0x4E  //485命令头
#define DPD_RS485_TAIL      0x4E  //485命令尾

#define DPD_MOD_GD          0x30  //广电DPD模块

#define DPD_TYPE_ISDB       0x70  //ISDB-T
#define DPD_TYPE_CMMB       0x80  //CMMB
#define DPD_TYPE_DTMB       0xB0  //DTMB



#define DPD_QUERY_DPDSTATE  0x11  // 查询DPD模块状态
#define DPD_RW_DPDATT       0X51  // 读写ATT	  
#define DPD_SET_CH          0x54  // 设置信道号
#define DPD_DP_SW           0xAA  // 数字预失真开关
#define DPD_DPD_SW          0xbb  // 数字预失真自动校正开关
#define DPD_CFR_SET         0xAE  // 波峰消除参数设置
#define DPD_RW_DPDLP        0x58  // 设置输入信号欠功率告警门限命令







#endif
