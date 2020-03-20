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

/* �豸 ״̬ö�� */
typedef enum
{
	eDev_NO_ERR = 0x00,			// �ɹ�
	eDev_CRC_ERR = 0x01,		// CRC��
	eDev_CMD_ERR = 0x02,		// �����								
	eDev_Dat_ERR = 0x03,		// ���ݴ�															
	eDev_Opr_ERR = 0x04,		// ����ʧ��
	eDev_TimeOut_ERR = 0x05,	// ���ӳ�ʱ
	eDev_RxNone_ERR = 0x06		// �޽��յ�����
}eDev_Err ;

/* RS485Э�鹦�ܱ��� */
#define DEV_FUN			0x00	// ����ģ��
#define ALL_FUN         0x01    // ����ģ��
#define PA_FUN			0x08	// ����
#define SWI_FUN         0x22    // �л���
#define POW_FUN			0x23	// ��Դģ��
#define DPD_FUN         0x24    // DPDģ��
#define EXCIT_FUN		0x27    // ��Ƶ������
#define CTTB0_FUN       0x21    // ������
#define CTTB1_FUN       0x26    // ������


/* RS485Э������ */
#define CMD_SETID		0x10	// ����ģ���ַ
#define CMD_QUERY_STATE	0x11	// ��ѯģ��״̬
#define CMD_QUERY_VER	0x12	// ��ѯģ������汾��
#define CMD_QUERY_ID    0x13    // ��ѯģ���ַ
#define CMD_RW_SN       0x14    // ��д��Ʒ���к� 

#define CMD_QUERY_INFO  0x15    // ��ѯ�豸��Ϣ
#define CMD_SET_INFO    0x16    // �����豸��Ϣ

#define CMD_SET_ATT		0x20	// ����ATT
#define CMD_SET_FQC		0x21	// ����Ƶ��
#define CMD_SET_SW		0x22	// ���ÿ���
#define CMD_RW_TIME		0x23	// ��ʱ���ػ�����
#define CMD_RW_POW		0x31	// ��ȡ/����ǰ/�����ʱ�
#define CMD_RW_POW_LEN	0x32	// ��ȡ/����ǰ/�����ʱ���

#define CMD_RST			0x88	// ģ�鸴λ

#define CMD_SET_PLIMIT	0xE0	// ���ù�������
#define CMD_RD_PLIMIT	0xE1	// ��ѯ��������
#define CMD_SET_OCLIMIT 0xE4    // ���ù���������
#define CMD_RD_OCLIMIT  0xE5    // ��ѯ����������
#define CMD_RW_OTLIMIT  0xE9    // ��д���¶�����
#define CMD_RW_SWVAL   	0xDF      // פ���ضϲο���ѹ����

#define CMD_RD_TPTCMPST		0xEE	// ���¶Ȳ�����
#define CMD_GAIN_TPTCMPST	0xEF	// �����²�
#define CMD_FAN_ONF			0xF5	// �������������ر�ָ��

#define CMD_RD_PVOL		0xE2	// ��ѯ���ʵ�ѹֵ
#define CMD_WR_PDATA	0xE3	// ���ù��ʱ�
#define CMD_RD_PDATA	0xE8	// ��ȡ���ʱ�
#define CMD_SET_ECT_MOD	0xF0	// ���ü���������ģʽ
#define CMD_SET_FANSW	0xF1	// ���÷������
#define CMD_RF_BOUND	0x52	// ��/д������ͷ��书������
#define CMD_SET_ECTSFNDT	0x58	// ���ü�����SFN��ʱʱ��
#define CMD_SET_ECTFQC	0x59	// ���ü���������Ƶ��
#define CMD_SET_ECTPOW	0x5A	// ���ü������������
#define CMD_RW_PHONE	0x5C	// ��/д�绰����
#define CMD_RW_COM		0x5D	// ��/дͨ�Ų���
#define CMD_RW_COMMODE	0x5E	// ��/дԶ��ͨ�ŷ�ʽ

#define CMD_RW_ALC_VOL  0x50    // ��/дALC��ѹֵ
#define CMD_RW_ATT_VOL  0x51    // ��/д�����ѹֵ
#define CMD_RF_BOUND    0x52    // ��/д��������
#define CMD_RW_ATT      0x54    // ��/дATTֵ
#define CMD_RW_ALC      0x55    // ��дALCֵ

#define CMD_RW_GAINMAX  0x4D    // ��/д�������

#define CMD_QUERY_DPDSTATE  0x11  // ��ѯDPDģ��״̬
#define CMD_RW_DPDATT       0X20  // ��дATT
#define CMD_RW_DPDMOD       0X70  // ��дDPD��Ϣ�ͺ�
#define CMD_DP_SW           0x71  // ����Ԥʧ�濪��
#define CMD_DPD_SW          0x72  // ����Ԥʧ���Զ�У������
#define CMD_CFR_SET         0x73  // ����������������
#define CMD_RW_DPDLP        0x74  // ���������ź�Ƿ���ʸ澯��������

#define CMD_RF_AGCONF	0x53	// ����AGC���ܵĿ�������

#define CMD_INIT_PWR		0x61	// ��ʼ������
#define CMD_SET_WORKMODEL		0x62	// ���ù��Ź���ģʽ
#define CMD_SET_OPRDATA		0x63	// ���ù������в���

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

