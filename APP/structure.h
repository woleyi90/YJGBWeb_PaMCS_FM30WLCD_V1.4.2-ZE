/*******************************************************************************
 *
 * Filename  : structure.h
 *
 * Function  : Reference head file for "STRUCTURE" library.
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

#ifndef __STRUCTURE_H
#define __STRUCTURE_H
   



#define	MAX_UART_BUF_LEN	255				// ���ڽ���buf��С
#define	MAX_DMA_BUFLEN		500				// DMA����buf��С
#define	MAX_UART_TimeOut	3				// ���ڽ�����ʱ,��λ 30ms
#define	MAX_UART2_TimeOut	20				// ����2������ʱ,��λ 50ms
#define	MAX_UART3_TimeOut	20				// ����3������ʱ,��λ 50ms
#define Max_Pa				1				// ���Ÿ���	  

#define MAX_PrArg_LEN		25				// ���ʱ���󳤶� 
#define MAX_TptArg_LEN	    25
#define DATALEN_MAX			200				// RS485��������󳤶�
#define NO_FUNID			0xFF            // ��RTU��ID��Ҫ���д���
#define MB_DevProNONE		0xFF            // MODBUS�޲�����־

#define	RSphone_MAX	    	5		// ��ѯ���õ绰�������
#define	Phone_LEN		    20		// �绰������󳤶�
#define Save_Size		sizeof(DI_CFG)		// ���浽FLASH��С
#define AutoMsgSave_Size		sizeof(AutoMsg_CFG)		// ���浽FLASH��С
  

/* ���ʲ����ṹ�� */
typedef struct
{
	s8 dBm;		// ����dBmֵ
	u16 Vol;	// ��Ӧ��ѹADֵ
}Pr_Struct;

/* �¶Ȳ����ṹ�� */ 
typedef struct
{
	s8 temp;		// �¶�ֵ
	u16 Vol;		// ��Ӧ��ѹADֵ
}Tp_Struct;

/* ��ѯ�����ϱ��绰���� */
typedef struct
{
	u8	RPphone[Phone_LEN];					// �ϱ��澯�绰
	u8	RSphone[RSphone_MAX][Phone_LEN];	// ��ѯ���õ绰
}Phone_Struct;


typedef struct
{
	u8  En;
	u16 Year;
	u8  Mon;
	u8  Day;
	u8  Hor;
	u8  Min;
	u8  Sec;

}TimeOnOff_Struct;


/* ϵͳ���ýṹ�� ���ϵͳ��������Ϣ,�ò�����Ϣ�豣�浽EEPROM��FLASH */
typedef struct
{
	volatile u8 	ICON;					// ϵͳ��־��
	volatile u8		FunID;					// ���ܴ���
	volatile u8		ID;						// ID	   
	volatile u8		OLDID;						// ID	 	 
	
	volatile u8		IP_Server[4];			// Ŀ�������ͨ��IP��ַ
	volatile u8		IP_Addr[4];				// Դ����ͨ��IP��ַ
	volatile u8		IP_Mask[4];				// ��������
	volatile u8		IP_GateWay[4];			// Ĭ������
	volatile u8		IP_DNS[4];				// DNS������IP
	volatile u16		Net_Port;				// ����ͨ�Ŷ˿ں�	 
	
	volatile u8		IP_LOCAL1[4];		// ���ع���1-IP��ַ	 
	volatile u16		LOCAL1NetPort1;		// ���ع���1-�˿ں�
	
	volatile u8		IP_LOCAL2[4];		// ���ع���2-IP��ַ	 
	volatile u16		LOCAL1NetPort2;		// ���ع���2-�˿ں�
	
	volatile u8		IP_INSERVER1[4];		// �ڲ�����1-IP��ַ	 
	volatile u16		INSERVERNetPort1;		// �ڲ�����1-�˿ں�
	
	volatile u8		IP_INSERVER2[4];		// �ڲ�����2-IP��ַ	 
	volatile u16		INSERVERNetPort2;		// �ڲ�����3-�˿ں�
	
	volatile u8		IP_RemoteHost1[4];		// Զ������1-IP��ַ
	volatile u16		RemoteHostNetPort1;		// Զ������1-�˿ں�
	volatile u16		RemoteHostDelay1_1;		// Զ������1-�� 1 ���������
	volatile u16		RemoteHostDelay1_2;		// Զ������1-�� 2 ���������
	volatile u16		RemoteHostDelay1_3;		// Զ������1-�� 3 ���������
	
	volatile u8		IP_RemoteHost2[4];		// Զ������2-IP��ַ
	volatile u16		RemoteHostNetPort2;		// Զ������2-�˿ں�
	volatile u16		RemoteHostDelay2_1;		// Զ������2-�� 1 ���������
	volatile u16		RemoteHostDelay2_2;		// Զ������2-�� 2 ���������
	volatile u16		RemoteHostDelay2_3;		// Զ������2-�� 3 ���������
	
	
	volatile u8		IP_RemoteHost3[4];		// Զ������3-IP��ַ
	volatile u16		RemoteHostNetPort3;		// Զ������3-�˿ں�
	volatile u16		RemoteHostDelay3_1;		// Զ������3-�� 1 ���������
	volatile u16		RemoteHostDelay3_2;		// Զ������3-�� 2 ���������
	volatile u16		RemoteHostDelay3_3;		// Զ������3-�� 3 ���������
	
	volatile u8		PA_IPHead;				// �����豸��������
	volatile u8		Excit_IPHead;			// �������豸��������
	
	volatile u8    	CloseFan_TPTlimit;        //�رշ����¶�
	volatile u8    	OpenFan1_TPTlimit;        //��������1�¶�
	volatile u8    	OpenFan2_TPTlimit;        //��������2�¶�
	volatile u8    	PWRRED_TPTlimit;          //�������¶�
	
	volatile float     PWRRED_CURlimit;          //�����ʵ���
	volatile float     PWROFF_CURlimit;          //����������
	volatile float     PWR_INlimit;         	  //������С����
	
	volatile float     PWR_INlimitMan;         //���������
	volatile float     PWR_InitOut;         	  //���ʳ�ʼ�����
	
	volatile u8       	PWR_RateOut;         	  //�������
	volatile u8       	PWR_state;         	  //��������
	volatile u8   		PWROFF_TPTlimit;          //���¹ػ������¶� 
	volatile u8     	PWR_RVlimit;         	  //���䱣������
	
	float P_FmFrq;					  		//���Ƶ��ֵ
	float P_InCal;						  //���빦��У��ֵ
	float P_OutCal;						 //�������У��ϵ��
	float P_RefCal;						  //���书��У��ϵ��
	
	//��Э�����
	volatile u8		DeviceID;	//�����ID����IP��ַ��IP_Addr[3]
	volatile u8 	CurRstNum; //������������
	volatile u8    P_Res_Front_Target;
	volatile u8    PWR_SetRateOut;//��������
	
	volatile u16 	CurRstDelay; //���������ж���ʱ
	volatile u16 	TPTRstDelay; //�����ж���ʱ
	

	
	volatile u16   Temp_ReducePowerInterval;//  = 300 ;      //�����ʼ��ʱ�� ��
	volatile u16   Temp_ReducePowerMaxCount;// = 3;	        //���͹���������
	
	volatile u8    P_AutoContrl ;// //�Զ����ʿ���  0�ֶ�   1�Զ�	
	volatile u8    P_state[2] ;
	volatile u8		StationInfo[17]; //�ص�(̨վ)����
	

	volatile u8		DeviceName[32];  //�豸����
	volatile u8		ManuFactory[32]; //��������
	volatile u8		PaItt[32]; 	//����ģ��˵��
	volatile u8		ExcitDirection[32]; //������ģ��˵��
	volatile u8		PartNo[32];		 //��Ʒ�ͺ�
	volatile u8		SerialNo[16];	 //��Ʒ���к�
	

	
}DI_CFG ;
typedef struct
{	
	volatile u8		AutoMsgICON;
	volatile u8		AutoMsg_Cfg1_Len;
	volatile u8		AutoMsg_Cfg2_Len;
	volatile u8		AutoMsg_Cfg3_Len;
		
	volatile u8		AutoMsg_INCfg1_Len;
	volatile u8		AutoMsg_INCfg2_Len;
	volatile u8		BAK[2]; 

	volatile u16	AutoMsg_Cfg1[50];	//��1���Զ���������,50��������ַ��һ��������ַ2�ֽ�
	volatile u16	AutoMsg_Cfg2[50];	//��2���Զ���������
	volatile u16	AutoMsg_Cfg3[50];	//��3���Զ���������
	volatile u16	AutoMsg_INCfg1[50];	//�ڲ���������1����
	volatile u16	AutoMsg_INCfg2[50];	//�ڲ���������2����

}AutoMsg_CFG ;		   

/* �������ݽṹ�� */			   
	typedef union		// ��ѯ����������
	{
		struct								// ģ��״̬
		{
			volatile	u32		EN  		:1;			// ����״̬
			volatile    u32		CH	     	:1;			// ����ͨ��״̬	
			volatile    u32		LOAD		:1;			// ������������״̬
			volatile    u32		RF_Alm 		:1;			// ���������ʸ澯 
			volatile    u32		RV_Alm		:1;			// ����������澯					
			volatile    u32		ECT_Alm 	:1;			// �������澯
			volatile    u32		FAN_Alm   	:1;			// ����澯״̬
			volatile    u32		POW_Alm     :1;			// ��Դ�澯״̬
		}State;
		
		struct
		{
			volatile    u8			State;					// ģ��״̬	
			volatile    int8_t		ALL_TPT;	      	    // �����¶�			
			volatile    u8          Input_SMA_Flag;         // �������ر�־λ��0x01��ʾ��Ч	
			volatile    u8          Input_SMA_ALM;          // �������ظ澯��0x01��ʾ��Ч
			volatile    int16_t		Power_OUT;				// ǰ����
			volatile    int16_t		Power_RV;				// ���书��
			volatile    u32         Freq;                   // �ŵ�����Ƶ��	 		
			volatile    u16			ALL_CUR;				// ���ŵ���
			volatile    u16			ALL_VOL;				// ���ŵ�ѹ       	
		
		}Data;
	}ALL_Data;
/* ����ģ�����ݽṹ�� */
	typedef union		// ��ѯ����������
	{
		struct								// ģ��״̬
		{
		 	volatile	u32		Enable		:1;			// ����״̬,����  
			volatile    u32		POW_Alm		:1;			// ����澯	
			volatile    u32		TPT_Alm		:1;			// ���¸澯
			volatile    u32		RV_Alm		:1;			// ������澯 
			volatile    u32		TPRED_Alm	:1;			// �¶Ƚ����ʸ澯					
			volatile    u32		OC_Alm		:1;			// �������澯
			volatile    u32		LP_Alm		:1;			// ����澯
			volatile    u32		OCRED_Alm  	:1;			// ���������ʸ澯
		}State;

		struct
		{
			volatile    u8			State;					// ģ��״̬	
			volatile    u8			FanState;				// ����״̬
			volatile    u8			pState0;				// ����
			volatile    u8			pState1;				// ����
			volatile    u8			ID;						// ģ���ַ
			volatile	u8			FunID;					// ���ܱ���
			volatile	u8			Fun0;					// ����0
			volatile	u8			Fun1;					// ����1
			volatile    u8			WorkState;				// ����ģ��״̬
			volatile    u8			DetState;				// ���ģ��״̬
			volatile    u8			ProState;				// ����ģ��״̬
			volatile    u8			ResState;				// ����λ
			volatile    float		Power_FR;				// ���书��
			volatile    float		Power_RV;				// ���书��
			volatile    float		CPU_TPT;				// CPU�¶�
			volatile    float		PA_TPT;					// �����¶�
			volatile    float		ENV_TPT;				// �����¶�
			volatile    float		Power_IN;				// ���빦��		
			volatile    float		PA_VOL;					// ���ŵ�ѹ
			volatile    float		PA_BECUR;				// ǰ������
			volatile    float		PA_CUR1;				// ���ŵ���1
			volatile    float		PA_CUR2;				// ���ŵ���2				
			volatile    float		Power_SET;				// ��������
			volatile    u16			Serial_Num;			   	//���к�
			volatile    u8			ATT_POT;			   	//��һ��˥����λ��
			volatile    u8			ATT_POT1;				//�ڶ���˥����λ��
			volatile 	u16			CRC16_Val;				// CRCУ��ֵ  
			volatile    u8			pState2;				// ����
			volatile    u8			pState3;				// ����
			volatile    float		PA_ALLCUR;				// ǰ������
			
			volatile    float 		P_InVal;	
			volatile    float 		P_OutVal;
			volatile    float 		P_ReflexVal;
			
			volatile	u8 	I_Status;      //����״̬
			volatile	u8 	I_StatusTrig;  //����״̬����
			volatile	u8 	I_StatusCont;  //����״̬����
			volatile	u8 	I_StatusRelease;//����״̬�ͷ� 

			volatile	u8 	TEMP_Status;	  //�¶�״̬
			volatile	u8 	TEMP_StatusTrig;	  //�¶�״̬����
			volatile	u8 	TEMP_StatusCont;	  //�¶�״̬����
			volatile	u8 	TEMP_StatusRelease;	  //�¶�״̬�ͷ�

			volatile	u8 	PAVCC_Status;      //��Դ״̬
			volatile	u8 	PAVCC_StatusTrig;  //��Դ״̬����
			volatile	u8 	PAVCC_StatusCont;  //��Դ״̬����
			volatile	u8 	PAVCC_StatusRelease;//��Դ״̬�ͷ� 
			
			volatile	u8  RF_Status;
			volatile	u8  RF_StatusTrig;
			volatile	u8  RF_StatusCont;
			volatile	u8 Temp_ReducePowerCount ;//	 = 0;   //�¶ȱ����ѽ��͹��ʴ��� 
			
			volatile	u8 I_OverLoadRestartCount;//    = 0;	//�ѹ���������������ֵ
			volatile	u8 Protect_Shutdown_Cause;// = 0;  //�����ػ�ԭ��;
			volatile	int8_t RES_Front; 
			volatile 	u8 RES_Rear;
			
			volatile 	float P_DownOffset;
			

		

		}Data;
	}PA_Data;


/* ��Ƶ���������ݽṹ */
typedef	struct				  
{
	volatile u8		ExcID;					// ģ���ַ
	volatile u8		FunID;					// ���ܱ���
	volatile u8		Fun0;					// ����0
	volatile u8		Fun1;					// ����1

	volatile u8		Fun2;					// ����2
	volatile u16	FMFRQ;	//���Ƶ��	76MHz~108MHz
	volatile u8		AssFrqInt;	// ��������Ƶ����������

	volatile u8		AssFrqPoint;// ��������Ƶ��С���㲿��
	volatile u8		FMMODULATIONMODE;	//����ģʽ 0:�ر� 1:������ 2:L������ 4:R������ 6:L+R����
	volatile u8		FMPre_Emphasis;	//Ԥ���� 00��0us  01:50us 
	volatile u8		FMGain_RF;	//RF���� 0~63

	volatile u8		FMMODULATIONVAL;//���ƶ�0~255
	volatile u8		FMRDS_CHIN;	//RDS�����ź�ѡ��  0:�������룻1:ģ���ź�����
	volatile u8		FMRDSVAL;	//RDS���� 0~15 0:���ز�
	volatile u8		FMCALFRQ_ENABLE;	//Ƶ��У��ʹ�ܡ�1:��Ч  0:��Ч

	volatile int	FMOSCFRQ;//����Ƶ��	
	volatile int	FMCALFRQ_VAL;//Ƶ��У��ֵ
	volatile u8		RDSDATA[8];
	volatile int	FMFRQDCT;	//���Ƶ�ʼ��
	volatile int	FMOSCFRQDCT;	//����Ƶ�ʼ��
	volatile int	FMMainChannelMean;//���ŵ���ֵ
	
	volatile u8		FMCALFRQ_SIGN;//Ƶ����У������λ 0���ӣ�1����
	volatile u16	ModulationDCT;				//�����ƶ�
	volatile u8		RDSID;
	
	volatile u16	DifModulation;		// ���ŵ���������ƶ�
	volatile u16	LeftChnModuVal;		// ���������ƶ�
	
	volatile u16	RightChnModuVal;	// ���������ƶ�
	volatile u16	CRC_Val;	
	
	volatile u8		FMSIGNAL_VAL;				// ����
	volatile u8		eState0;				// ����
	volatile u16	ChnModuState;
}Excit_Data;	
/* ʱ��״̬�ṹ�� */
typedef struct
{
	u32 HoCnt;		// Сʱ����
	u32 SeCnt;		// �����
	u32 MiCnt;		// �ּ���
}T_Struct;



/* �л���ģ���ѯ���ݽṹ�� */
typedef union		// ��ѯ����������
{
	struct									// ģ��״̬
	{
		u32		Alm			:1;			// ģ��״̬
		u32		BState		:1;			// ͨ��B״̬
		u32		AState		:1;			// ͨ��A״̬
		u32		ECT_CH		:1;			// ������ͨ��A/B
		u32		ECT_MOD		:1;			// ����������ģʽ,�Զ�/�ֶ�
		u32		State6		:1;			// ����
		u32		State7		:1;			// ����
		u32		State8		:1;			// ����
	}State;
		
	struct
	{
		u8			State;				// ģ��״̬	
		u8			swiState0;				// ģ��״̬
		u8			swiState1;				// ģ��״̬
		u8			swiState2;				// ģ��״̬	
		int16_t		Power_In;			// ���빦��
		int16_t		Power_Out;			// �������		 
	}Data;
}SWI_Data;


/* ϵͳ״̬ ������ */
typedef union
{
	u32		ALL ;						// ����ȫ����ֵ
	struct
	{ 
		volatile    u32     fWdgClr     : 1	;     // ι����־															
		volatile	u32		fMS 		: 1 ;     // 10�����־ ÿ����λ��λһ��
		volatile	u32		fSec 		: 1 ;	  // ���־ ÿ����λ��λһ��
		volatile	u32		fMin 		: 1 ;	  // �ֱ�־ ÿ����λ��λһ��
		volatile	u32		fHour 		: 1 ;	  // Сʱ��־ ÿСʱ��λ��λһ��
		volatile	u32		fDOM 		: 1 ;	  // ���־ ÿ����λ��λһ�� 
		volatile    u32     fNetLink    : 1 ;     // �������ӱ�־	
		volatile    u32     fSaveStart  : 1 ;     // ������־
				
		volatile    u32     fNetRecom   : 1 ;     // ������־
		volatile    u32     fInputAlm   : 1 ;     // ������ϱ�־
		volatile    u32     fNetTime05s : 1 ;     // ����0.5���¼���־
		volatile    u32     fNetTime10s : 1 ;     // ����10���¼���־
		volatile    u32     fETH_INT    : 1 ;     // ������ն�ʱ��־  
		volatile    u32     fAlmOFF	    : 1 ;     // ���Ϲػ���־

		volatile	u32		fDOMEvent	: 1 ;	  // �촦���¼���־
		volatile	u32		fSenEvent	: 1 ;     // MODEM���¼�
		volatile	u32		fMinEvent	: 1 ;	  // ���Ӵ����־
		volatile	u32		fHourEvent	: 1 ;	  // Сʱ�����־
		volatile    u32     fAutoMsgSave : 1 ;    // ���ݱ����־
		
		volatile    u32     fPaRtudata    : 1 ;     // ����RTU���ݷ��ͱ�־
		volatile    u32     fSave       : 1 ;     // ���ݱ����־
		volatile    u32     fEitRtudata : 1 ;   // ���ư�RTU���ݷ��ͱ�־
		volatile    u32     f3sec       : 1 ;     // 3�붨ʱ��־		
		volatile    u32     fPaON      : 1 ;   // ���ſ�����־
		volatile    u32     fPaOFF      : 1 ;   // ���Źػ���־
		volatile    u32     fPaTimer      : 1 ;   // ����ʱ�䴦��
		volatile    u32     fPaCheck      : 1 ;   // ������������У��λ
		volatile    u32     fPanetQue     : 1 ;   // ������̫��������ѯ��־λ
		volatile    u32     fExcitCheck   : 1 ;   // ��������������У��λ
		volatile    u32     fstatebk   : 1 ;   	// ����λ

	}Bit ;	
}Sys_State ;
typedef union
{
	u32		ALL ;						// ����ȫ����ֵ
	struct
	{ 
		volatile    u32     fNetConfig    : 1 ;   // �������ñ�־
		volatile    u32     fUdpRecom    : 1 ;   // UDP�������ñ�־
		volatile    u32     fWebsave    : 1 ;   // webҳ���޸Ĳ�����־
		volatile    u32     fPaOutCrt    : 1 ;   // �������У����־
		volatile    u32     fPaInCrt    : 1 ;   // ��������У����־
		volatile    u32     fPaRvCrt    : 1 ;   // ���ŷ���У����־
	}Bit ;	
}SysNet_State ;


/* ϵͳ���ݽṹ�� */
typedef struct
{          	
	 volatile DI_CFG		Cfg ;			    // DI����
	 volatile AutoMsg_CFG	AutoMsgCfg ;		// DI����
	 volatile ALL_Data      ALL;                // ��������
	 volatile PA_Data		Pa[Max_Pa];			// ��������
	 volatile Excit_Data	Excitdata;			// ���ư�����
	 volatile u8            MAC[6];             // MAC��ַ
	 volatile u8         	ID_Cur;             // 485ͨ�ŵ�ǰID��
	 volatile u8         	CMD_Cur;            // ��ǰ����	  
 	
}System ; 
/*״̬���ݽṹ*/
typedef struct DeviceDatas
{
	u32 rtctime;
	
	//��������ʱ���ͱ���
	u32 S1G1ST;
	u32 S1G2ST;
	u32 S1G3ST;
	
	u32 S2G1ST;
	u32 S2G2ST;
	u32 S2G3ST;
	
	u32 S3G1ST;
	u32 S3G2ST;
	u32 S3G3ST;
}DeviceDatas;

/* ���ڽ��սṹ�� */
typedef struct
{
	u8 Buff[MAX_UART_BUF_LEN] ;			// ���ջ�����
	u16 Len ;							// ���ճ���
	u16 TimeOut ;						// ���ճ�ʱ����,�ü���ֵΪ0ʱ,��־���ս���
	bool fEnd	;						// ���ս�����־ 1,���ս���  0,������
}UART_RBuf ;

/* RS485ͨ�����ݽṹ�� */
typedef struct
{
	u8 Head_Code;	// ͷ��
	u8	FunID;		// ���ܱ���
	u8	ID;			// ģ��ID
	u8	CMD;		// ����
	u8	ACK;		// Ӧ���־
	u8	DataLen;	// �����峤��
	u8	DataBuf[DATALEN_MAX];	// ������,У��,β��(�䳤)
}Dev_Data;

typedef struct
{
	u8 	Device_ID;	// �豸ID
	u8	FunID;		// ���ܱ���
	u8	DataLenL;	// ���ݳ��ȵͰ�λ
	u8	DataLenH;	// ���ݳ��ȸ߰�λ
	u8	DataBuf[DATALEN_MAX];	// ������,У����
}Dev_GXData;
/* RS485ͨ�����ݰ��ṹ�� */
typedef struct
{
	Dev_Data 	Dev_Dat;
	u8		    Len;			// ���ݰ�����
}RS485_Dat;

/* MODEM״̬�ṹ�� */
typedef struct
{
	u8 HourCnt;		// Сʱ����
	u8 SecCnt;		// �����
	u8 MinCnt;		// �ּ���
	u8 ModemState;	// MODEM״̬
	bool  fInitStart;	// ����һ�γ�ʼ����־
	bool  fReadModem;	// ��ʱ��ȡ���ű�־
	u8 cnt_CSQ;
}Modem_Struct;

/*�Զ��������ʱ��*/
typedef struct MYTIMER
{
    u8 Timer_Enable ;
	u16 Timer_Count ;
	u32 Timer_Out_Count ;
	u8 Timer_Timeout;
}MYTIMER;



#endif

