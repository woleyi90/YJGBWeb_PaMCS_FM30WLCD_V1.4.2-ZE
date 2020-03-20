/*******************************************************************************
 *
 * Filename  : const.c
 *
 * Function  : Implememt CONST function form for this workbench.
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



/* 网络配置信息 */
const u8 cIP_Server[] = {192, 168, 1, 2};				// 目标IP
const u8 cIP_Addr[] = {192, 168, 1, 207};				// 源IP
const u8 cIP_Mask[] = {255, 255, 255, 0};				// 子网掩码
const u8 cIP_GateWay[] = {192, 168, 1, 1};				// 默认网关
const u8 cIP_DNS[] = {192, 168, 1, 1};					// DNS服务器
const u8 cMAC_ADDR[] = {0x04,0x02,0x35,0x00,0x00,0x01};// MAC地址
const u8 cSN[] = "DZTMT20160301001    ";
const u8 cST[] ="00000000";
const u8 cStationInfo[17] = "GX_FM_NanNingNo01";
const u8 cDeviceName[32] =  "GX_FM30W_00001                  ";
const u8 cManuFactory[32] = "SHANGHAI_DZLD                   ";
const u8 cPartNo[32] =      "00000000000000000000000000000000";
const u8 cSerialNo[16] = "GXFM30W000000001";
const u8 cIP_LOCAL1[4]= {192, 168, 1, 2};		// 本地管理1-IP地址	 
const u8 cIP_LOCAL2[4]= {192, 168, 1, 50};		// 本地管理2-IP地址   
const u8 cIP_INSERVER1[4] = {0, 0, 0, 0};		// 内部联动1-IP地址	 
const u8 cIP_INSERVER2[4] = {0, 0, 0, 0};		// 内部联动2-IP地址	 
const u8 cIP_RemoteHost1[4] = {10, 170, 1, 66};		// 远程主机1-IP地址
const u8 cIP_RemoteHost2[4] = {0, 0, 0, 0};		// 远程主机2-IP地址
const u8 cIP_RemoteHost3[4] = {0, 0, 0, 0};		// 远程主机3-IP地址
const u16 cAutoMsg_Cfg1[50] = {0x0201,0x0202,0x0206,0x0210,0x0211};	//第1组自动报文设置,50个参数地址，一个参数地址2字节
const u16 cAutoMsg_Cfg2[50] = {0x0203,0x0204,0x0205,0x0206,0x0207,0x0210,0x0211,0x0212,0x0213,0x0214,0x0215};	//第2组自动报文设置
const u16 cAutoMsg_Cfg3[50] = {0x0201,0x0202,0x0206,0x0207,0x0210,0x0211};	//第3组自动报文设置
const u16 cAutoMsg_INCfg1[50]= {0};	//内部联动报文1设置
const u16 cAutoMsg_INCfg2[50]= {0};	//内部联动报文2设置
