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



/* ����������Ϣ */
const u8 cIP_Server[] = {192, 168, 1, 2};				// Ŀ��IP
const u8 cIP_Addr[] = {192, 168, 1, 207};				// ԴIP
const u8 cIP_Mask[] = {255, 255, 255, 0};				// ��������
const u8 cIP_GateWay[] = {192, 168, 1, 1};				// Ĭ������
const u8 cIP_DNS[] = {192, 168, 1, 1};					// DNS������
const u8 cMAC_ADDR[] = {0x04,0x02,0x35,0x00,0x00,0x01};// MAC��ַ
const u8 cSN[] = "DZTMT20160301001    ";
const u8 cST[] ="00000000";
const u8 cStationInfo[17] = "GX_FM_NanNingNo01";
const u8 cDeviceName[32] =  "GX_FM30W_00001                  ";
const u8 cManuFactory[32] = "SHANGHAI_DZLD                   ";
const u8 cPartNo[32] =      "00000000000000000000000000000000";
const u8 cSerialNo[16] = "GXFM30W000000001";
const u8 cIP_LOCAL1[4]= {192, 168, 1, 2};		// ���ع���1-IP��ַ	 
const u8 cIP_LOCAL2[4]= {192, 168, 1, 50};		// ���ع���2-IP��ַ   
const u8 cIP_INSERVER1[4] = {0, 0, 0, 0};		// �ڲ�����1-IP��ַ	 
const u8 cIP_INSERVER2[4] = {0, 0, 0, 0};		// �ڲ�����2-IP��ַ	 
const u8 cIP_RemoteHost1[4] = {10, 170, 1, 66};		// Զ������1-IP��ַ
const u8 cIP_RemoteHost2[4] = {0, 0, 0, 0};		// Զ������2-IP��ַ
const u8 cIP_RemoteHost3[4] = {0, 0, 0, 0};		// Զ������3-IP��ַ
const u16 cAutoMsg_Cfg1[50] = {0x0201,0x0202,0x0206,0x0210,0x0211};	//��1���Զ���������,50��������ַ��һ��������ַ2�ֽ�
const u16 cAutoMsg_Cfg2[50] = {0x0203,0x0204,0x0205,0x0206,0x0207,0x0210,0x0211,0x0212,0x0213,0x0214,0x0215};	//��2���Զ���������
const u16 cAutoMsg_Cfg3[50] = {0x0201,0x0202,0x0206,0x0207,0x0210,0x0211};	//��3���Զ���������
const u16 cAutoMsg_INCfg1[50]= {0};	//�ڲ���������1����
const u16 cAutoMsg_INCfg2[50]= {0};	//�ڲ���������2����
