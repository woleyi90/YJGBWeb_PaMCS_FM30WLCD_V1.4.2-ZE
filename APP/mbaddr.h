/*******************************************************************************
 *
 * Filename  : Mbaddr.h
 *
 * Function  : Reference head file for "MBADDR" library.
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

#ifndef __MBADDR_H 
#define __MBADDR_H


// MODBUS实际地址为以下地址+1
#define	MB_BIT_CTRMODE		    2		 // 激励器控制模式,手动/自动
#define	MB_BIT_ECTCH		    3		 // 激励器通道
#define	MB_BIT_CTRMODE_MODEM	4		 // 激励器控制模式,手动/自动
#define	MB_BIT_ECTCH_MODEM		5		 // 激励器通道

#define MB_BIT_PA0SW		    6
#define MB_BIT_PA1SW		    7
#define MB_BIT_PA2SW		    8
#define MB_BIT_PA3SW            9

#define	MB_BIT_PASW		    10		// 功放开关
#define	MB_BIT_PAATT		11		// 功放ATT

#define MB_BIT_DPDATT       12      // DPDATT设置
#define MB_BIT_DPSW         13      // 数字预失真开关
#define MB_BIT_DPDSW        14      // 数据预失真自动校正开关
#define MB_BIT_DPDCFR       15      // DPD CFR设置
#define MB_BIT_DPDLP        16      // DPD输入信号欠功率门限设置

#define MB_BIT_DPDPOLL      17      // DPD论询
#define MB_BIT_CFREQ        19      // 中心频率设置

#define MB_BIT_PA0SW_MODEM      26
#define MB_BIT_PA1SW_MODEM      27
#define MB_BIT_PA2SW_MODEM      28
#define MB_BIT_PA3SW_MODEM      29

#define	MB_BIT_PASETID		0x20	// 功放设置ID
#define MB_BIT_PASWON		0x21	// 设置功放开
#define MB_BIT_PASWOFF		0x22	// 设置功放关
#define MB_BIT_ALC_VOL  	0x50    // 读/写ALC电压值
#define MB_BIT_ATT_VOL  	0x51    // 读/写增益电压值
#define MB_BIT_ATT      	0x54    // 读/写ATT值
#define MB_BIT_ALC      	0x55    // 读写ALC值
#define MB_BIT_SWVAL   		0x5F    // 驻波关断参考电压设置

#define MB_BIT_ECTATTADD      	0x25	//切换器ATT电压+设置
#define MB_BIT_ECTATTRED      	0x2F	//切换器ATT电压-设置

#endif

























