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


// MODBUSʵ�ʵ�ַΪ���µ�ַ+1
#define	MB_BIT_CTRMODE		    2		 // ����������ģʽ,�ֶ�/�Զ�
#define	MB_BIT_ECTCH		    3		 // ������ͨ��
#define	MB_BIT_CTRMODE_MODEM	4		 // ����������ģʽ,�ֶ�/�Զ�
#define	MB_BIT_ECTCH_MODEM		5		 // ������ͨ��

#define MB_BIT_PA0SW		    6
#define MB_BIT_PA1SW		    7
#define MB_BIT_PA2SW		    8
#define MB_BIT_PA3SW            9

#define	MB_BIT_PASW		    10		// ���ſ���
#define	MB_BIT_PAATT		11		// ����ATT

#define MB_BIT_DPDATT       12      // DPDATT����
#define MB_BIT_DPSW         13      // ����Ԥʧ�濪��
#define MB_BIT_DPDSW        14      // ����Ԥʧ���Զ�У������
#define MB_BIT_DPDCFR       15      // DPD CFR����
#define MB_BIT_DPDLP        16      // DPD�����ź�Ƿ������������

#define MB_BIT_DPDPOLL      17      // DPD��ѯ
#define MB_BIT_CFREQ        19      // ����Ƶ������

#define MB_BIT_PA0SW_MODEM      26
#define MB_BIT_PA1SW_MODEM      27
#define MB_BIT_PA2SW_MODEM      28
#define MB_BIT_PA3SW_MODEM      29

#define	MB_BIT_PASETID		0x20	// ��������ID
#define MB_BIT_PASWON		0x21	// ���ù��ſ�
#define MB_BIT_PASWOFF		0x22	// ���ù��Ź�
#define MB_BIT_ALC_VOL  	0x50    // ��/дALC��ѹֵ
#define MB_BIT_ATT_VOL  	0x51    // ��/д�����ѹֵ
#define MB_BIT_ATT      	0x54    // ��/дATTֵ
#define MB_BIT_ALC      	0x55    // ��дALCֵ
#define MB_BIT_SWVAL   		0x5F    // פ���ضϲο���ѹ����

#define MB_BIT_ECTATTADD      	0x25	//�л���ATT��ѹ+����
#define MB_BIT_ECTATTRED      	0x2F	//�л���ATT��ѹ-����

#endif

























