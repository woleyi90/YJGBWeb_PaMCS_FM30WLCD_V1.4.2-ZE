/*******************************************************************************
 *
 * Filename  : TaskRTU.c
 *
 * Function  : Implememt TASKRTU function form for this workbench.
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



/*
 *******************************************************************************
 *
 * private defines
 *
 *******************************************************************************
 */
#define		DEVNUM_MAX	    1 	    // ��ѯ�豸��
#define     RSTIMEOUT		2		// RS485���ӳ�ʱ����




/*
 *******************************************************************************
 *
 * private routines prototype
 *
 *******************************************************************************
 */

u8 PAPack_Collect_RS(void);	
u8 ExciterPack_Collect_RS(void);  
void PADevPoll(void);
void ExciterDevPoll(void);

static int	EitCollect_cnt = 0;

/*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */
/*******************************************************************************
 * �������ƣ�TaskRTU()
 * �������ܣ�RTU�������� ����RTU�����ݲɼ� ,���� ���� ������
 * ��ڲ�������
 * ���ڲ������� 
 *******************************************************************************/
void  TaskRTU(void *pdata)
{
	u8 err;				 
	u8 patrynum = 0,paresult = 0; 
	
	pdata = pdata;
	
	OSTimeDlyHMSM(0, 0, 2, 0);	 
	USART3_Config(115200);          //��ʼ������3 ���ư崮��RS232ͨ��
	
	while (1)
	{
	
		EitCollect_cnt++;
		if(EitCollect_cnt >= 25)
		{
			EitCollect_cnt = 0;
				
			if(fEitRTU == FALSE)
			{
				ExciterDevPoll(); 
				OSTimeDlyHMSM(0, 0, 0, 30);
				OSSemPend(JLQRS232SEM, 0, &err);	
				PADevPoll();
				OSSemPost(JLQRS232SEM);				

			}
		}	
			
		OSTimeDlyHMSM(0, 0, 0, 50);	  		//�����������ݰ�	

	}
}	



/*
 ******************************************************************************
 * �������ƣ�PADevPoll()
 * �������ܣ�PA�豸��Ѳ
 * ��ڲ�������
 * ���ڲ�����TRUE �ɹ�,FALSE:ʧ��
 *******************************************************************************/
 void PADevCheckPoll(void)
 {
  	u8 err,rslen;
    u8 trynum;
    u8 result;
    
    static u8 PaCnt;	   // ���ų�ʱ����	    	
	trynum = 3;

	while (trynum--)
	{ 	    			
		rslen = PAPack_Collect_RS();	//����ģ��״̬��ѯ	 				
		result = PA_RS232_GXTx(GX_TXData,rslen, TIMEOUT);	//����ģ��״̬��ѯ���ݰ�  	
		if(result == eDev_NO_ERR)
		{
		  	
			trynum = 0;		    // ���յ���ȷ��Ӧ,�ط���������	
		} 		
	}
		
	/* ͨ�ų�ʱ��ģ��״̬�ṹ������ȫ������ */	
	if (result != eDev_NO_ERR)
	{
		  
			if(PaCnt < 200)  //
			{
				PaCnt++;		 
			}
			if(PaCnt >= 2)  //
			{
				//��ʱ��������0
				g_TMTLink[0] =0;		 
				memset((u8*)&g_DI.Pa[0], 0, sizeof(PA_Data));

			}
		
	}
	else
	{
		PaCnt = 0;			// ��������	
		g_TMTLink[0] =1;	// ����ͨ��״̬
	}	
 }
/*
 ******************************************************************************
 * �������ƣ�PADevPoll()
 * �������ܣ�PA�豸��Ѳ
 * ��ڲ�������
 * ���ڲ�����TRUE �ɹ�,FALSE:ʧ��
 *******************************************************************************/
void PADevPoll(void)
{
	char  outdata[500]; 
  	u16 outlen = 0; 

	USART_AutoSend(&outdata[0],&outlen);
	   
	UART3Write(&outdata[0], outlen); 
	
	EXCIT_Wait_RxEnd(1);	

	UART3_RBuf.Len = 0;
	UART3_RBuf.fEnd = FALSE;		
		
}
/*
 ******************************************************************************
 * �������ƣ�ExciterDevPoll()
 * �������ܣ��������豸��Ѳ
 * ��ڲ�������
 * ���ڲ�����TRUE �ɹ�,FALSE:ʧ��
 *******************************************************************************/
 void ExciterDevPoll(void)
 {
  	u8 err,rslen;
    u8 trynum;
    u8 result;
    
    static u8 ExCnt;	   // ���ų�ʱ����	    	
	trynum = 3;

	while (trynum--)
	{ 	    			
		rslen = ExciterPack_Collect_RS();	//����ģ��״̬��ѯ	 			
		OSSemPend(JLQRS232SEM, 0, &err);	
		result = Excit_RS232_Send(GX_TXData,rslen, TIMEOUT);	//����ģ��״̬��ѯ���ݰ�  
		OSSemPost(JLQRS232SEM);			
		if(result == eDev_NO_ERR)
		{
		  	
			trynum = 0;		    // ���յ���ȷ��Ӧ,�ط���������	
		} 		
	}
		
	/* ͨ�ų�ʱ��ģ��״̬�ṹ������ȫ������ */	
	if (result != eDev_NO_ERR)
	{
		  
			if(ExCnt < 200)  //
			{
				ExCnt++;		 
			}
			if(ExCnt >= 2)  //
			{
				//��ʱ��������0
				g_TMTLink[1] =0;		 
				memset((u8*)&g_DI.Excitdata, 0, sizeof(Excit_Data));

			}
		
	}
	else
	{
		ExCnt = 0;			// ��������	
		g_TMTLink[1] =1;	// ����ͨ��״̬
	}	
 } 

/*
 ******************************************************************************
 * �������ƣ�Dev_Pro()
 * �������ܣ�ģ���豸���Ʋ���
 * ��ڲ�������
 * ���ڲ�����TRUE �ɹ�,FALSE:ʧ��
 *******************************************************************************/
bool Dev_Opr(void)
{
	u8 trynum;	    // �ط�����
	u8 Head;
	u8 result;
	u8 Opr_type =0;	//���ò�����������   
	u8 ATTV[4];
	u8 Channal;

	

	
	if ((g_DevPro == MB_DevProNONE)||(g_DevPro == 0x00))		// �޲���
	{
		return FALSE;
	}
	
	trynum = 3;
	while (trynum--)
	{
		switch (g_DevPro & 0x7F)
		{									

/***********************�����RS485���ڸ�������ģ���ͨ��***********************************/
			/* �������÷��������*/
			case MB_BIT_PA0SW: // �����0����
				
				if((g_DevPro&0x80) == 0x80) //����
				{
						memset(GX_TXData,0,100);
						GX_TXData[0]= g_DI.Cfg.PA_IPHead;
						GX_TXData[1] = 0x35;
						GX_TXData[2] = 0xc3;
						GX_TXData[3] = 0xfc;
						GX_TXData[4] = 0xc1;
						GX_TXData[5] = 0xee;
						GX_TXData[6] = 0xbf;
						GX_TXData[7] = 0xaa;
						GX_TXData[8] = 0xbb;
						GX_TXData[9] = 0xfa;
						GX_TXDatalen = 10;																						
				}
				else
				{  				 	
				 		memset(GX_TXData,0,100);
						GX_TXData[0]= g_DI.Cfg.PA_IPHead;
						GX_TXData[1] = 0x35;
						GX_TXData[2] = 0xc3;
						GX_TXData[3] = 0xfc;
						GX_TXData[4] = 0xc1;
						GX_TXData[5] = 0xee;
						GX_TXData[6] = 0xb9;
						GX_TXData[7] = 0xd8;
						GX_TXData[8] = 0xbb;
						GX_TXData[9] = 0xfa;
						GX_TXDatalen = 10;																		
				} 		  						
				
				Opr_type =0;

				break;				

		 	

		

			default:  
				Opr_type = 2;
				break;
		}
				
		 if(Opr_type == 0)
		{ 		 	
			RS485_Tx(&GX_TXData[0], GX_TXDatalen, 1);			
			trynum = 0 ;		// ���յ���ȷ��Ӧ,�ط���������
			g_DevPro = MB_DevProNONE;		// ����ʧ�ܱ�־���	 
			
			return TRUE;
			
		} 
		else
		{
		 	g_DevPro = MB_DevProNONE;		// ����ʧ�ܱ�־���
			return FALSE; 
		
		}

	} 
	g_DevPro = MB_DevProNONE;		// ����ʧ�ܱ�־���
	return FALSE;  		
}
/*******************************************************************************
 * �������ƣ�PAPack_Collect_RS()
 * �������ܣ����RS485���ݰ�
 * ��ڲ�����
 * ���ڲ�����
 *******************************************************************************/ 
u8 PAPack_Collect_RS(void)
{
	u8 txlen;

	if(Sys_Flag.Bit.fPaCheck == FALSE)//�ڶ�ʱ��ѯ��ѯ����״̬֮ǰ��У��һ�¹�����������
	{
	
		GX_TXData[0]= 0x00;
		GX_TXData[1]= 0x32;
		txlen = 2;
	
	}
	else
	{
	  	GX_TXData[0]= g_DI.Cfg.PA_IPHead;
		GX_TXData[1]= 0x03;
		GX_TXData[2]= 0x00;
		GX_TXData[3]= 0x00;
		GX_TXData[4]= 0x00;
		GX_TXData[5]= 0x46;
		txlen = 6;
		
	}
	return txlen;
}
/*******************************************************************************
 * �������ƣ�ExciterPack_Collect_RS()
 * �������ܣ�������ݰ�
 * ��ڲ�����
 * ���ڲ�����
 *******************************************************************************/ 
u8 ExciterPack_Collect_RS(void)
{
	u8 txlen;
//	Sys_Flag.Bit.fExcitCheck = TRUE;
//	g_DI.Cfg.Excit_IPHead = 0x33;
	if(Sys_Flag.Bit.fExcitCheck == FALSE)//�ڶ�ʱ��ѯ��ѯ����״̬֮ǰ��У��һ�¹�����������
	{
	
		GX_TXData[0]= 0x00;
		GX_TXData[1]= 0x32;
		txlen = 2;
	
	}
	else
	{
	  	GX_TXData[0]= g_DI.Cfg.Excit_IPHead; 
		GX_TXData[1]= 0x03;
		GX_TXData[2]= 0x00;
		GX_TXData[3]= 0x00;
		GX_TXData[4]= 0x00;
		GX_TXData[5]= 0x46;
		txlen = 6;
		
	}
	return txlen;
}


