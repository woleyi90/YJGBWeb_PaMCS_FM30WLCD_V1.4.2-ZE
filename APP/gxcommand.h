#ifndef __GXCOMMAND_H
#define __GXCOMMAND_H


//�豸��������
#define CMD_READ	0x03	//������
#define CMD_WRITE	0x10	//д����
#define CMD_SEARCH	0x5A	//��������
#define CMD_WRITEERR	0x90	//д������Ӧ֡��
#define CMD_AUTOSEND	0x65	//�����ϱ�֡
#define CMD_REMOTELOGOIN	0x6A	//Զ�̵�¼
//�豸������Ϣ								    
#define	INFO_STATIONINFO		0x0001 	//�ص�(̨վ)����			
#define INFO_DEVICE_NAME		0x0002 	//�豸����
#define INFO_HARDWARE_VERSION	0x0003 	//�豸Ӳ���汾��
#define INFO_SOFT_VERSION	0x0004 	//�豸����汾��
#define INFO_MANUFACTORY		0x0005	//��������
#define INFO_PART_NO		0x0006	//��Ʒ�ͺ�		
#define INFO_SERIAL_NO		0x0007	//��Ʒ���к�		
#define INFO_MCSCPU_SerialNO 	0x000C	//����CPU���к�
//�豸ͨ�Žӿ�
#define INFO_DEVICE_ID		0x0020	//���豸ID
#define INFO_MACADDR		0x0021	//���豸MAC��ַ
#define INFO_DHCPEN			0x0022	//DHCPʹ��
#define INFO_IP_ADDR		0x0023	//���豸IP��ַ
#define INFO_MASK_ADDR		0x0024	//��������
#define INFO_GATEWAY_ADDR	0x0025	//Ĭ������
#define INFO_IP_LOCAL1	0x0026	//���ع���1-IP		 
#define INFO_IP_LOCAL2	0x0027	//���ع���2-IP

#define INFO_IP_INSERVER1	0x0028	//�ڲ�����1-IP
#define INFO_IP_INPORT1	0x0029	//�ڲ�����1-�˿ں�
#define INFO_IP_INSERVER2	0x002A	//�ڲ�����2-IP
#define INFO_IP_INPORT2	0x002B	//�ڲ�����2-�˿ں�
	
#define INFO_IP_SERVER1	0x0030	//Զ������1-IP
#define INFO_Net_Port1	0x0031	//Զ������1-�˿ں�
#define INFO_FActive_Time1	0x0032	//��1���������1
#define INFO_FActive_Time2	0x0033	//��1���������2	
#define INFO_FActive_Time3	0x0034	//��1���������3
		 
#define INFO_IP_SERVER2	0x0035	//Զ������2-IP
#define INFO_Net_Port2	0x0036	//Զ������2-�˿ں�
#define INFO_SActive_Time1	0x0037	//��2���������1
#define INFO_SActive_Time2	0x0038	//��2���������2	
#define INFO_SActive_Time3	0x0039	//��2���������3

#define INFO_IP_SERVER3	0x003A	//Զ������3-IP
#define INFO_Net_Port3	0x003B	//Զ������3-�˿ں�
#define INFO_TActive_Time1	0x003C	//��3���������1
#define INFO_TActive_Time2	0x003D	//��3���������2	
#define INFO_TActive_Time3	0x003E	//��3���������3
#define INFO_IPFilteringe_En	0x003F	//IP����ʹ��
//�豸��������
#define INFO_MACH_SETON	0x0101	//����(ר��)
#define INFO_MACH_SETOFF	0x0102	//�ػ�(ר��)
#define INFO_MACH_BITOP	0x0103	//λ����(��\��\��λ)

#define INFO_MACH_ATT1	0x0121	//˥����1
#define INFO_MACH_ATT2	0x0122	//˥����2
#define INFO_MACH_NETINIT	0x0123	//�����ʼ��
#define INFO_MACH_CTR	0x0128	//��������λ
#define INFO_MACH_FANCTR	0x0129	//�������ȿ���λ
#define INFO_MACH_COMCTR	0x012A	//����ͨ�ſ���λ
#define INFO_MACH_RSTFACTORYDATA	0x012F	//�����ָ��������� 1�ֽ� 0x01:��Ч

//��������
#define INFO_AutoMsg_Cfg1	0x01F8	//��1���Զ���������
#define INFO_AutoMsg_Cfg2	0x01F9	//��2���Զ���������
#define INFO_AutoMsg_Cfg3	0x01FA	//��3���Զ���������
#define INFO_AutoMsg_INCfg1	0x01FB	//�ڲ���������1����
#define INFO_AutoMsg_INCfg2	0x01FC	//�ڲ���������2����

//����״̬���ݺͲ�������
#define INFO_AllState_cWork	0x0200	//��������״̬
#define INFO_PWR_RFOTU	0x0201	//������������,W
#define INFO_PWR_RV		0x0202	//��������书��,W
#define INFO_ALL_VOL	0x0203	//��������ŵ�ѹ,V
#define INFO_ALL_CUR	0x0204	//��������ŵ���,A
#define INFO_PWR_SWR	0x0205	//�����פ����
#define INFO_TPT_CPU	0x0206	//CPU�¶�,��
#define INFO_TPT_EVM	0x0207	//�����¶�,��
#define INFO_AllState_Work	0x0210	//��������״̬
#define INFO_AllState_Det	0x0211	//�������״̬
#define INFO_AllState_Pro	0x0212	//��������״̬
#define INFO_AllPWR_Int	0x0220	//���ʳ�ʼ��,W
#define INFO_SETPWR_Rated	0x0221	//�趨�������,W
#define INFO_INPWRLimit_Min	0x0222	//���빦����Сֵ,dBm
#define INFO_INPWRLimit_Max	0x0223	//���빦�����ֵ,dBm
#define INFO_RVPWRLimit_Max	0x0224	//���书�����ֵ,W
#define INFO_CURLimit_RedPwr	0x0225	//�����ʹ��ŵ�������,A
#define INFO_CURLimit_OFFPwr	0x0226	//�ػ����ŵ�������,A
#define INFO_CURPWRRST_Time	0x0227	//���������ж���ʱ,��
#define INFO_CURPWRRST_Num	0x0228	//������������,��
#define INFO_TPTLimit_RedPwr	0x0229	//�����ʹ����¶�����,��
#define INFO_TPTLimit_OFFPwr	0x022A	//�ػ������¶�����,��
#define INFO_TPT_OpenFan1	0x022B	//������ 1 ���¶�,��
#define INFO_TPT_OpenFan2	0x022C	//������ 2 ���¶�,��
#define INFO_TPT_CloseFan	0x022D	//�ط��ȵ��¶�,��
#define INFO_TPTRST_Time	0x022E	//���������ж���ʱ,��
#define INFO_INPWR_CRTVAL	0x023C	//���빦��У��ֵ
#define INFO_OUTPWR_CRTVAL	0x023E	//�������У��ֵ
#define INFO_RVPWR_CRTVAL	0x023F	//���书��У��ֵ


#define INFO_ExciterCom_State	0x0213	//������ģ��ͨ��״̬ bit0:��������ͨ�ţ� 1 ͨ�������� 0 Ϊͨ���ж�
										//bit1:��������ͨ�ţ� 1 ͨ�������� 0 Ϊͨ���ж�
#define INFO_PaCom_State	0x0214	//����ģ��ͨ��״̬��bit0:ģ��1��1 ͨ��������0Ϊͨ���ж�
										//bit1:ģ��2��1 ͨ��������0Ϊͨ���ж�	...
#define INFO_PowerSupplyCom_State	0x0215	//��Դģ��ͨ��״̬
//0�Ź���ģ������
#define INFO_PA_ITT		0x1000	//����ģ��˵��
#define INFO_PAPWR_RFOTU		0x1001	//�����������,W, 2�ֽ�
#define INFO_PAPWR_INOTU		0x1002	//�������빦��,W��dBm, 2�ֽ�
#define INFO_PATPT		0x1003	//�����¶�,��,float
#define INFO_PA_State		0x1004	//����״̬ bit0:���� ->0:�� 1����   bit1:������ ->0:�� 1����
									//bit2:���� ->0:�� 1����
#define INFO_PA_TDVCC		0x101E	//�ƶ��ܹ�����ѹ, V, float
#define INFO_PA_TDCUR		0x101F	//�ƶ��ܵ���, A, float
#define INFO_PA_GLVCC		0x1020	//���ʹܹ�����ѹ, V, float
#define INFO_PA_GLCUR1		0x1021	//���ʹܵ���1, A, float
#define INFO_PA_GLCUR2		0x1022	//���ʹܵ���2, A, float
#define INFO_PA_CPUNO		0x103E	//CPU���к�
#define INFO_PA_FanSpeed1		0x103F	//����ת��1
#define INFO_PA_FanSpeed3		0x1040	//����ת��2
#define INFO_PA_FanSpeed5		0x1041	//����ת��3
#define INFO_PA_FanSpeed4		0x1042	//����ת��4
//���ü��������ݣ� 0xB000-0xB02F��
#define INFO_ExciterDirection 	0xB000	//����ģ��˵��
#define INFO_ExciterPwr_OUT 	0xB001	//�������������,W
#define INFO_ExciterPwr_RV 	0xB002	//���������书��,W
#define INFO_ExciterOut_Frq 	0xB003	//���������Ƶ��,KHz
#define INFO_Exciter_TPT 	0xB004	//�������¶�,��
#define INFO_ExciterL_Mod 	0xB005	//L���ƶ�,%
#define INFO_ExciterR_Mod 	0xB006	//R���ƶ�,%
#define INFO_Exciter_ModMode 	0xB007	//����ģʽ��0:�ر� 1:������ 2:L������ 3:R������ 4:L+R������
#define INFO_Exciter_PreEmp 	0xB008	//Ԥ���� 0:0us 1:50us 2:75us
#define INFO_ExciterL_Gain 	0xB009	//L ������Ƶ����
#define INFO_ExciterR_Gain 	0xB00D	//R ������Ƶ����
#define INFO_ExciterPWR_Gain 	0xB00E	//��������
#define INFO_ExciterPWR_SW 	0xB010	//���ſ��� 0:�� 1�� ��
#define INFO_ExciterCPU_SerialNO 	0xB01E	//CPU���к�
#define INFO_ExciterMODULATIONVAL 	0xB020	//���ƶ�0~255
#define INFO_ExciterRDSVAL 	0xB021	//RDS����
#define INFO_ExciterCRMODULATIONVAL 	0xB022	//���ƶ�0~7
#define INFO_ExciterOut_FrqMHZ 	0xB023	//���������Ƶ��,MHz
#define INFO_ExciterRDSDATA 	0xB025	//RDS���� 8Byte
#define INFO_ExciterRDSBLOCKDATA	0xB026	//RDS������ 8*nByte
#define INFO_ExciterRDSCHN	0xB027	//RDSͨ��ѡ�� 1Byte
#define INFO_ExciterMODULDCT	0xB028	//���ƶȼ�� float
#define INFO_ExciterDIFMODULDCT	0xB029	//������ƶ� float
#endif






