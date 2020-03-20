/*******************************************************************************
 *
 * Filename  : const.h
 *
 * Function  : Reference head file for "CONST" library.
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

#ifndef __CONST_H  
#define __CONST_H


#include "structure.h"	


extern const u8 cIP_Server[];
extern const u8 cIP_Addr[];
extern const u8 cIP_Mask[];
extern const u8 cIP_GateWay[];
extern const u8 cIP_DNS[];
extern const u8 cMAC_ADDR[];
extern const u8 cSN[];
extern const u8 cST[];	
extern const u8 cStationInfo[17];
extern const u8 cDeviceName[32];
extern const u8 cManuFactory[32];
extern const u8 cPartNo[32];
extern const u8 cSerialNo[16];
extern const u8 cIP_LOCAL1[4];		// 本地管理1-IP地址	 
extern const u8 cIP_LOCAL2[4];		// 本地管理2-IP地址   
extern const u8 cIP_INSERVER1[4];		// 内部联动1-IP地址	 
extern const u8 cIP_INSERVER2[4];		// 内部联动2-IP地址	 
extern const u8 cIP_RemoteHost1[4];		// 远程主机1-IP地址
extern const u8 cIP_RemoteHost2[4];		// 远程主机2-IP地址
extern const u8 cIP_RemoteHost3[4];		// 远程主机3-IP地址	   
extern const u16 cAutoMsg_Cfg1[50];	//第1组自动报文设置,50个参数地址，一个参数地址2字节
extern const u16 cAutoMsg_Cfg2[50];	//第2组自动报文设置
extern const u16 cAutoMsg_Cfg3[50];	//第3组自动报文设置
extern const u16 cAutoMsg_INCfg1[50];	//内部联动报文1设置
extern const u16 cAutoMsg_INCfg2[50];	//内部联动报文2设置

#endif
