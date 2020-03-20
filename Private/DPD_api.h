#ifndef _DPD_H
#define _DPD_H






#define RD_CMD	0x01		// DPDģ�������
#define WR_CMD	0x02		// DPDģ��д����

#define DPD_RSInPWR_L		0
#define DPD_RSInPWR_H		1
#define DPD_IN_PWR			2

#define DPD_RS485_HEAD      0x4E  //485����ͷ
#define DPD_RS485_TAIL      0x4E  //485����β

#define DPD_MOD_GD          0x30  //���DPDģ��

#define DPD_TYPE_ISDB       0x70  //ISDB-T
#define DPD_TYPE_CMMB       0x80  //CMMB
#define DPD_TYPE_DTMB       0xB0  //DTMB



#define DPD_QUERY_DPDSTATE  0x11  // ��ѯDPDģ��״̬
#define DPD_RW_DPDATT       0X51  // ��дATT	  
#define DPD_SET_CH          0x54  // �����ŵ���
#define DPD_DP_SW           0xAA  // ����Ԥʧ�濪��
#define DPD_DPD_SW          0xbb  // ����Ԥʧ���Զ�У������
#define DPD_CFR_SET         0xAE  // ����������������
#define DPD_RW_DPDLP        0x58  // ���������ź�Ƿ���ʸ澯��������







#endif
