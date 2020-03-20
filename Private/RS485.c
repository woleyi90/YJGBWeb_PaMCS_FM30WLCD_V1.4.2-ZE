/*******************************************************************************
 *
 * Filename  : RS485.c
 *
 * Function  : Implememt RS485 function form for this workbench.
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

#define cerror0        ":0000000000000000:"

/*
 *******************************************************************************
 *
 * static private routines prototype
 *
 *******************************************************************************
 */
extern u8 ManualChanging;
extern u8  FAN_ManCtrl_Delay;

u8 Char_unTransfer(u8 *Rx_Data, u8 *pDat, u8 Len, u8 Head);
u8 Char_Transfer(u8 *Buf, u8 *Dat, u8 Len, u8 Head);
void SoftReset(void);
u8 Pack485_Apart0(u8 *ApBuf, u8 *Buf, u8 BufLen);
eDev_Err GXData_Process(u8 *DataBuf, u8 Len);
u8 PaRS232Pack_PT(u8 *Buf, u8 Len, u16 timeout,u8 *RxBuf);
bool EXCIT_Wait_RxEnd(u16 timeout);
eDev_Err ExcitData_Process(u8 *DataBuf, u8 Len);
u16 CMD_ReadProcess(u8 *Buf, u16 PackLen, u8 Head);
u16 CMD_SetProcess(u8 *Buf, u16 PackLen, u8 Head);

u8 Pack485_Apart0(u8 *ApBuf, u8 *Buf, u8 BufLen)
{
	u8 Len=0;
	u8 i;
	
	if (*Buf != MODEM_HEAD_CODE)			// ���ݲ��Է���
	{
		return 0;
	}
	
	*ApBuf++ = *Buf++;
	Len++;
	
	for (i=1; i<BufLen; i++)
	{
		if (*Buf == MODEM_TAIL_CODE)		// �԰�β�жϽ���
		{
			*ApBuf = *Buf;
			Len++;
			break;
		}
		else
		{
			Byte_To_ASC(ApBuf, *Buf);
			ApBuf += 2;
			Buf++;
			Len += 2;
		}
	}
	
	if (*ApBuf != MODEM_TAIL_CODE)		// ���ݲ���
	{
		return 0;
	}
	
	return Len;
}
 /*
 *******************************************************************************
 *
 * API routines
 *
 *******************************************************************************
 */	

/******************************************************************************
 * �������ƣ�Protocol_Pack_Process()
 * �������ܣ���ѯ�����ʱЭ�鴦��
 * ��ڲ�����*PackBuf:Э�����ݰ�,PackLen:���ݰ�����
 * ���ڲ��������ص����ݳ���
 *******************************************************************************/
u16 Protocol_Pack_Process(u8 *PackBuf, u16 PackLen, u8 Head)
{
	u16 len,j;
	u8 crcH, crcL;
	u16 CRC_Value;


	crcL = PackBuf[PackLen - 2];
	crcH = PackBuf[PackLen - 1];

	CRC_Value = CRC16_GX_MODBUS((u8 *)PackBuf, PackLen-2);			// ����CRC
	
										 
	if ((crcL != (u8)(CRC_Value&0xFF)) || (crcH != (u8)((CRC_Value>>8)&0xFF)))  // CRCУ���ж�
	{	   					
		return 0;
	}
	
	switch(PackBuf[1])
	{
		case CMD_READ: 	//��ȡ			    	
			len = CMD_ReadProcess(PackBuf, PackLen, Head);	  			
			break;
		case CMD_WRITE: //����			
			len = CMD_SetProcess(PackBuf, PackLen, Head);
			break;
		default:
		    len =0;	
			break;
	}
	
  
	return len;
}

//�������в�����ѯ 
 eDev_Err PA_OptPmt_ProcessAgain(void)
{
	u8 Result;
	u8 err; 
	u8 GxSendBuf[20];
	

	GxSendBuf[0]= g_DI.Cfg.PA_IPHead;
	GxSendBuf[1]= 0x35;
	GxSendBuf[2]= 0xB6;
	GxSendBuf[3]= 0xC1;
	GxSendBuf[4]= 0xB2;
	GxSendBuf[5]= 0xCE;
	GxSendBuf[6]= 0xCA;
	GxSendBuf[7]= 0xFD;	    

	Result = PA_RS232_GXTx(GxSendBuf,8, 50);	//����ģ��״̬��ѯ���ݰ�  



	return Result; 
} 
u16 CMD_GetParmaData(u16 parCode, u8 *TXBuf)
{	
	
	u8 a = 0;
	//u8 GetParOperData = 0;
	u8 Result;
	int16_t pwruot = 0,pwrin = 0;
	u8 pastate = 0,fanstate = 0;
	u16 TxLen = 0;

	switch(parCode)
	{
		case INFO_STATIONINFO:	//�ص�(̨վ)����
			TXBuf[TxLen++] = (u8)(INFO_STATIONINFO);
			TXBuf[TxLen++] = (u8)(INFO_STATIONINFO>>8);
			TXBuf[TxLen++] = 17; 
			DataCopy((u8 *)&TXBuf[TxLen], (u8 *)&g_DI.Cfg.StationInfo,17);
			TxLen = TxLen + 17;
			break;
						
		case INFO_DEVICE_NAME:	//�豸����
			TXBuf[TxLen++] = (u8)(INFO_DEVICE_NAME);
			TXBuf[TxLen++] = (u8)(INFO_DEVICE_NAME>>8);
			TXBuf[TxLen++] = 32; 
			DataCopy((u8 *)&TXBuf[TxLen], (u8 *)&g_DI.Cfg.DeviceName,32);
			TxLen = TxLen + 32;
			break;
		case INFO_MCSCPU_SerialNO:	//CPU���к�
			TXBuf[TxLen++] = (u8)(INFO_MCSCPU_SerialNO);
			TXBuf[TxLen++] = (u8)(INFO_MCSCPU_SerialNO>>8);
			TXBuf[TxLen++] = 12; 
			TXBuf[TxLen++] = (u8)(CpuID[0]);
			TXBuf[TxLen++] = (u8)(CpuID[0]>>8);
			TXBuf[TxLen++] = (u8)(CpuID[0]>>16);
			TXBuf[TxLen++] = (u8)(CpuID[0]>>24);
			TXBuf[TxLen++] = (u8)(CpuID[1]);
			TXBuf[TxLen++] = (u8)(CpuID[1]>>8);
			TXBuf[TxLen++] = (u8)(CpuID[1]>>16);
			TXBuf[TxLen++] = (u8)(CpuID[1]>>24);
			TXBuf[TxLen++] = (u8)(CpuID[2]);
			TXBuf[TxLen++] = (u8)(CpuID[2]>>8);
			TXBuf[TxLen++] = (u8)(CpuID[2]>>16);
			TXBuf[TxLen++] = (u8)(CpuID[2]>>24);
			break;					  
		case INFO_HARDWARE_VERSION:	//�豸Ӳ���汾��
			TXBuf[TxLen++] = (u8)(INFO_HARDWARE_VERSION);
			TXBuf[TxLen++] = (u8)(INFO_HARDWARE_VERSION>>8);
			TXBuf[TxLen++] = 2;
			TXBuf[TxLen++] = (u8)(Hard_VERSION);
			TXBuf[TxLen++] = (u8)(Hard_VERSION>>8); 
			break;

		case INFO_SOFT_VERSION:	//�豸����汾��
			TXBuf[TxLen++] = (u8)(INFO_SOFT_VERSION);
			TXBuf[TxLen++] = (u8)(INFO_SOFT_VERSION>>8);
			TXBuf[TxLen++] = 2;
			TXBuf[TxLen++] = (u8)(SOFT_VERSION);
			TXBuf[TxLen++] = (u8)(SOFT_VERSION>>8);
			break;

		case INFO_MANUFACTORY:	//��������
			TXBuf[TxLen++] = (u8)(INFO_MANUFACTORY);
			TXBuf[TxLen++] = (u8)(INFO_MANUFACTORY>>8);
			TXBuf[TxLen++] = 32; 
			DataCopy((u8 *)&TXBuf[TxLen], (u8 *)&g_DI.Cfg.ManuFactory,32);
			TxLen = TxLen + 32;
			break;

		case INFO_PART_NO:	//��Ʒ�ͺ�	
			TXBuf[TxLen++] = (u8)(INFO_PART_NO);
			TXBuf[TxLen++] = (u8)(INFO_PART_NO>>8);
			TXBuf[TxLen++] = 32; 
			DataCopy((u8 *)&TXBuf[TxLen], (u8 *)&g_DI.Cfg.PartNo,32);
			TxLen = TxLen + 32;
			break;	

		case INFO_SERIAL_NO:	//��Ʒ���к�
			TXBuf[TxLen++] = (u8)(INFO_SERIAL_NO);
			TXBuf[TxLen++] = (u8)(INFO_SERIAL_NO>>8);
			TXBuf[TxLen++] = 32; 
			DataCopy((u8 *)&TXBuf[TxLen], (u8 *)&g_DI.Cfg.SerialNo,32);
			TxLen = TxLen + 32;
			break;
		
		case INFO_DEVICE_ID:
			TXBuf[TxLen++] = (u8)(INFO_DEVICE_ID);
			TXBuf[TxLen++] = (u8)(INFO_DEVICE_ID>>8);
			TXBuf[TxLen++] = 1; 
			TXBuf[TxLen++] = g_DI.Cfg.DeviceID;
		 	break;
		
		case INFO_MACADDR:		 //���豸MAC��ַ

			TXBuf[TxLen++] = (u8)(INFO_MACADDR);
			TXBuf[TxLen++] = (u8)(INFO_MACADDR>>8);
			TXBuf[TxLen++] = 6; 
			TXBuf[TxLen++] = g_DI.MAC[0];
			TXBuf[TxLen++] = g_DI.MAC[1];
			TXBuf[TxLen++] = g_DI.MAC[2];
			TXBuf[TxLen++] = g_DI.MAC[3];
			TXBuf[TxLen++] = g_DI.MAC[4];
			TXBuf[TxLen++] = g_DI.MAC[5];
			break;
		
		case INFO_IP_ADDR:	  //���豸IP��ַ

			TXBuf[TxLen++] = (u8)(INFO_IP_ADDR);
			TXBuf[TxLen++] = (u8)(INFO_IP_ADDR>>8);
			TXBuf[TxLen++] = 4; 
			TXBuf[TxLen++] = g_DI.Cfg.IP_Addr[0];
			TXBuf[TxLen++] = g_DI.Cfg.IP_Addr[1];
			TXBuf[TxLen++] = g_DI.Cfg.IP_Addr[2];
			TXBuf[TxLen++] = g_DI.Cfg.IP_Addr[3];
			break;

		case INFO_MASK_ADDR:	//��������

			 TXBuf[TxLen++] = (u8)(INFO_MASK_ADDR);
			 TXBuf[TxLen++] = (u8)(INFO_MASK_ADDR>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_Mask[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_Mask[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_Mask[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_Mask[3];
			 break;

		case INFO_GATEWAY_ADDR:	 //Ĭ������

			 TXBuf[TxLen++] = (u8)(INFO_GATEWAY_ADDR);
			 TXBuf[TxLen++] = (u8)(INFO_GATEWAY_ADDR>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_GateWay[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_GateWay[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_GateWay[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_GateWay[3];
			 break;

		case INFO_IP_LOCAL1:	//���ع���1-IP	
			 TXBuf[TxLen++] = (u8)(INFO_IP_LOCAL1);
			 TXBuf[TxLen++] = (u8)(INFO_IP_LOCAL1>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_LOCAL1[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_LOCAL1[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_LOCAL1[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_LOCAL1[3];
			 g_DI.Cfg.LOCAL1NetPort1 = g_DI.Cfg.IP_LOCAL1[3]+2000;		// ���ع���1-�˿ں�
	
			break;
				 
		case INFO_IP_LOCAL2:	//���ع���2-IP
			 TXBuf[TxLen++] = (u8)(INFO_IP_LOCAL2);
			 TXBuf[TxLen++] = (u8)(INFO_IP_LOCAL2>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_LOCAL2[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_LOCAL2[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_LOCAL2[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_LOCAL2[3];
			 g_DI.Cfg.LOCAL1NetPort2 = g_DI.Cfg.IP_LOCAL2[3]+2000;		// ���ع���2-�˿ں�
			break;

		case INFO_IP_INSERVER1:	//�ڲ�����1-IP
			 TXBuf[TxLen++] = (u8)(INFO_IP_INSERVER1);
			 TXBuf[TxLen++] = (u8)(INFO_IP_INSERVER1>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_INSERVER1[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_INSERVER1[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_INSERVER1[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_INSERVER1[3];
			break;

		case INFO_IP_INPORT1:	//�ڲ�����1-IP
			 TXBuf[TxLen++] = (u8)(INFO_IP_INPORT1);
			 TXBuf[TxLen++] = (u8)(INFO_IP_INPORT1>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.INSERVERNetPort1);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.INSERVERNetPort1 >> 8);			
			break;

		case INFO_IP_INSERVER2:	//�ڲ�����2-IP
			 TXBuf[TxLen++] = (u8)(INFO_IP_INSERVER2);
			 TXBuf[TxLen++] = (u8)(INFO_IP_INSERVER2>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_INSERVER2[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_INSERVER2[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_INSERVER2[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_INSERVER2[3];
			break;

		case INFO_IP_INPORT2:	//�ڲ�����2-IP
			 TXBuf[TxLen++] = (u8)(INFO_IP_INPORT2);
			 TXBuf[TxLen++] = (u8)(INFO_IP_INPORT2>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.INSERVERNetPort2);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.INSERVERNetPort2 >> 8);
			break;


		case INFO_IP_SERVER1:	//Զ������1-IP
			 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER1);
			 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER1>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost1[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost1[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost1[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost1[3];
			break;

		case INFO_Net_Port1:	//Զ������1-�˿ں�
			 TXBuf[TxLen++] = (u8)(INFO_Net_Port1);
			 TXBuf[TxLen++] = (u8)(INFO_Net_Port1>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostNetPort1);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostNetPort1 >> 8);
			break;
		case INFO_FActive_Time1:	//��1���������1
			 TXBuf[TxLen++] = (u8)(INFO_FActive_Time1);
			 TXBuf[TxLen++] = (u8)(INFO_FActive_Time1>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay1_1);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay1_1 >> 8);		
			break;

		case INFO_FActive_Time2:	//��1���������2
			 TXBuf[TxLen++] = (u8)(INFO_FActive_Time2);
			 TXBuf[TxLen++] = (u8)(INFO_FActive_Time2>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay1_2);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay1_2 >> 8);						
			break;

		case INFO_FActive_Time3:	//��1���������3
			 TXBuf[TxLen++] = (u8)(INFO_FActive_Time3);
			 TXBuf[TxLen++] = (u8)(INFO_FActive_Time3>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay1_3);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay1_3 >> 8);		
			break;
					 
		case INFO_IP_SERVER2:	//Զ������2-IP
			 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER2);
			 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER2>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost2[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost2[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost2[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost2[3];					
			break;

		case INFO_Net_Port2:	//Զ������2-�˿ں�
			 TXBuf[TxLen++] = (u8)(INFO_Net_Port2);
			 TXBuf[TxLen++] = (u8)(INFO_Net_Port2>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostNetPort2);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostNetPort2 >> 8);					
			break;

		case INFO_SActive_Time1:	//��2���������1
			 TXBuf[TxLen++] = (u8)(INFO_SActive_Time1);
			 TXBuf[TxLen++] = (u8)(INFO_SActive_Time1>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay2_1);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay2_1 >> 8);						
			break;

		case INFO_SActive_Time2:	//��2���������2	
			 TXBuf[TxLen++] = (u8)(INFO_SActive_Time2);
			 TXBuf[TxLen++] = (u8)(INFO_SActive_Time2>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay2_2);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay2_2 >> 8);					
			break;

		case INFO_SActive_Time3:	//��2���������3
			 TXBuf[TxLen++] = (u8)(INFO_SActive_Time3);
			 TXBuf[TxLen++] = (u8)(INFO_SActive_Time3>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay2_3);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay2_3 >> 8);				
			break;
	
		case INFO_IP_SERVER3:	//Զ������3-IP
			 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER3);
			 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER3>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost3[0];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost3[1];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost3[2];
			 TXBuf[TxLen++] = g_DI.Cfg.IP_RemoteHost3[3];					
			break;

		case INFO_Net_Port3:	//Զ������3-�˿ں�
			 TXBuf[TxLen++] = (u8)(INFO_Net_Port3);
			 TXBuf[TxLen++] = (u8)(INFO_Net_Port3>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostNetPort3);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostNetPort3 >> 8);					
			break;

		case INFO_TActive_Time1:	//��3���������1
			 TXBuf[TxLen++] = (u8)(INFO_TActive_Time1);
			 TXBuf[TxLen++] = (u8)(INFO_TActive_Time1>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay3_1);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay3_1 >> 8);					
			break;

		case INFO_TActive_Time2:	//��3���������2	
			 TXBuf[TxLen++] = (u8)(INFO_TActive_Time2);
			 TXBuf[TxLen++] = (u8)(INFO_TActive_Time2>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay3_2);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay3_2 >> 8);						
			break;

		case INFO_TActive_Time3:	//��3���������3
		 	 TXBuf[TxLen++] = (u8)(INFO_TActive_Time3);
			 TXBuf[TxLen++] = (u8)(INFO_TActive_Time3>>8);
			 TXBuf[TxLen++] = 2; 
		  	 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay3_3);
			 TXBuf[TxLen++] = (u8)(g_DI.Cfg.RemoteHostDelay3_3 >> 8);							
			break;

		case INFO_AutoMsg_Cfg1:	//��1���Զ���������
		
		 	 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg1);
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg1>>8);
			 TXBuf[TxLen++] = g_DI.AutoMsgCfg.AutoMsg_Cfg1_Len;
			 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_Cfg1_Len/2; a++) 
			 {
			   	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_Cfg1[a]);
			 	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_Cfg1[a] >> 8);				
			 }	  	 
			break;
		
		case INFO_AutoMsg_Cfg2:	//��2���Զ���������
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg2);
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg2>>8);
			 TXBuf[TxLen++] = g_DI.AutoMsgCfg.AutoMsg_Cfg2_Len;
			 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_Cfg2_Len/2; a++) 
			 {
			   	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_Cfg2[a]);
			 	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_Cfg2[a] >> 8);
			 }
			break;
		
		case INFO_AutoMsg_Cfg3:	//��3���Զ���������
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg3);
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg3>>8);
			 TXBuf[TxLen++] = g_DI.AutoMsgCfg.AutoMsg_Cfg3_Len;
			 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_Cfg3_Len/2; a++) 
			 {
			   	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_Cfg3[a]);
			 	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_Cfg3[a] >> 8);
			 }
			break;
		
		case INFO_AutoMsg_INCfg1:	//�ڲ���������1����
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_INCfg1);
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_INCfg1>>8);
			 TXBuf[TxLen++] = g_DI.AutoMsgCfg.AutoMsg_INCfg1_Len;
			 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_INCfg1_Len/2; a++) 
			 {
			   	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_INCfg1[a]);
			 	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_INCfg1[a] >> 8);
			 }
			break;
		
		case INFO_AutoMsg_INCfg2:	//�ڲ���������2����
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_INCfg2);
			 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_INCfg2>>8);
			 TXBuf[TxLen++] = g_DI.AutoMsgCfg.AutoMsg_INCfg2_Len;
			 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_INCfg2_Len/2; a++) 
			 {
			   	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_INCfg2[a]);
			 	TXBuf[TxLen++] = (u8)(g_DI.AutoMsgCfg.AutoMsg_INCfg2[a] >> 8);
			 }
			break;
			
		case INFO_PWR_RFOTU:	 //������������,W

			 TXBuf[TxLen++] = (u8)(INFO_PWR_RFOTU);
			 TXBuf[TxLen++] = (u8)(INFO_PWR_RFOTU>>8);
			 TXBuf[TxLen++] = 4; 
		  	 FloatToByte(g_DI.Pa[0].Data.Power_FR,&TXBuf[TxLen]);
			 TxLen = TxLen + 4;

			 break;

		case INFO_PWR_RV:	   //��������书��,W

			 TXBuf[TxLen++] = (u8)(INFO_PWR_RV);
			 TXBuf[TxLen++] = (u8)(INFO_PWR_RV>>8);
			 TXBuf[TxLen++] = 4; 
		  	 FloatToByte(g_DI.Pa[0].Data.Power_RV,&TXBuf[TxLen]);
			 TxLen = TxLen + 4;
			 break;

		case INFO_ALL_VOL:	   //��������ŵ�ѹ,V

			 TXBuf[TxLen++] = (u8)(INFO_ALL_VOL);
			 TXBuf[TxLen++] = (u8)(INFO_ALL_VOL>>8);
			 TXBuf[TxLen++] = 4; 
		  	 FloatToByte(g_DI.Pa[0].Data.PA_VOL,&TXBuf[TxLen]);
			 TxLen = TxLen + 4;
			 break;
	  	
		case INFO_ALL_CUR:	   //��������ŵ���,A

			 TXBuf[TxLen++] = (u8)(INFO_ALL_CUR);
			 TXBuf[TxLen++] = (u8)(INFO_ALL_CUR>>8);
			 TXBuf[TxLen++] = 4; 
			 g_DI.Pa[0].Data.PA_ALLCUR = g_DI.Pa[0].Data.PA_BECUR + g_DI.Pa[0].Data.PA_CUR1 + g_DI.Pa[0].Data.PA_CUR2;
		  	 FloatToByte(g_DI.Pa[0].Data.PA_ALLCUR,&TXBuf[TxLen]);
			 TxLen = TxLen + 4;
			 break;

		case INFO_TPT_CPU:	   //CPU�¶�,��

			 TXBuf[TxLen++] = (u8)(INFO_TPT_CPU);
			 TXBuf[TxLen++] = (u8)(INFO_TPT_CPU>>8);
			 TXBuf[TxLen++] = 4; 
		  	 FloatToByte(g_DI.Pa[0].Data.CPU_TPT,&TXBuf[TxLen]);
			 TxLen = TxLen + 4;
			 break;

		case INFO_TPT_EVM:	   //�����¶�,��

			 TXBuf[TxLen++] = (u8)(INFO_TPT_EVM);
			 TXBuf[TxLen++] = (u8)(INFO_TPT_EVM>>8);
			 TXBuf[TxLen++] = 4; 
		  	 FloatToByte(g_DI.Pa[0].Data.ENV_TPT,&TXBuf[TxLen]);
			 TxLen = TxLen + 4;
			 break;
		case INFO_AllState_cWork:	   //��������״̬

			 TXBuf[TxLen++] = (u8)(INFO_AllState_cWork);
			 TXBuf[TxLen++] = (u8)(INFO_AllState_cWork>>8);
			 TXBuf[TxLen++] = 1; 
		  	 TXBuf[TxLen++] = g_DI.Pa[0].Data.WorkState;
			 break;		
		case INFO_AllState_Work:	   //��������״̬

			 TXBuf[TxLen++] = (u8)(INFO_AllState_Work);
			 TXBuf[TxLen++] = (u8)(INFO_AllState_Work>>8);
			 TXBuf[TxLen++] = 1; 
		  	 TXBuf[TxLen++] = g_DI.Pa[0].Data.WorkState;
			 break;

	   case INFO_AllState_Det:	   //�������״̬

			 TXBuf[TxLen++] = (u8)(INFO_AllState_Det);
			 TXBuf[TxLen++] = (u8)(INFO_AllState_Det>>8);
			 TXBuf[TxLen++] = 1; 
		  	 TXBuf[TxLen++] = g_DI.Pa[0].Data.DetState;
			 break;

		case INFO_AllState_Pro:	   //��������״̬

			 TXBuf[TxLen++] = (u8)(INFO_AllState_Pro);
			 TXBuf[TxLen++] = (u8)(INFO_AllState_Pro>>8);
			 TXBuf[TxLen++] = 1; 
		  	 TXBuf[TxLen++] = g_DI.Pa[0].Data.ProState;
			 break;
		case INFO_ExciterCom_State:	//������ģ��ͨ��״̬ bit0:��������ͨ�ţ� 1 ͨ�������� 0 Ϊͨ���ж�
									//bit1:��������ͨ�ţ� 1 ͨ�������� 0 Ϊͨ���ж�
			 TXBuf[TxLen++] = (u8)(INFO_ExciterCom_State);
			 TXBuf[TxLen++] = (u8)(INFO_ExciterCom_State>>8);
			 TXBuf[TxLen++] = 1; 
		  	 TXBuf[TxLen++] = 0x01;		 //��������ͨ��״̬
			break;						
		case INFO_PaCom_State:	//����ģ��ͨ��״̬��bit0:ģ��1��1 ͨ��������0Ϊͨ���ж�
								//bit1:ģ��2��1 ͨ��������0Ϊͨ���ж�	...
			 TXBuf[TxLen++] = (u8)(INFO_PaCom_State);
			 TXBuf[TxLen++] = (u8)(INFO_PaCom_State>>8);
			 TXBuf[TxLen++] = 4; 
		  	 TXBuf[TxLen++] = 0x01;	  // ����ģ��1ͨ��״̬
			 TXBuf[TxLen++] = 0x00;
			 TXBuf[TxLen++] = 0x00;
			 TXBuf[TxLen++] = 0x00;
			break;					
		case INFO_AllPWR_Int:	   //���ʳ�ʼ��,W

			 TXBuf[TxLen++] = (u8)(INFO_AllPWR_Int);
			 TXBuf[TxLen++] = (u8)(INFO_AllPWR_Int>>8);
			 TXBuf[TxLen++] = 4; 
		  	 FloatToByte(g_DI.Cfg.PWR_RateOut*1.0,&TXBuf[TxLen]);	//g_DI.Cfg.PWR_InitOut 
			 TxLen = TxLen + 4;
			 break;
		
		case INFO_SETPWR_Rated:	//�趨�������,W

			TXBuf[TxLen++] = (u8)(INFO_SETPWR_Rated);
			TXBuf[TxLen++] = (u8)(INFO_SETPWR_Rated>>8);
			TXBuf[TxLen++] = 4; 
			
			FloatToByte((float)(g_DI.Cfg.PWR_SetRateOut*1.0),&TXBuf[TxLen]);
			
			TxLen = TxLen + 4;
			break;
		case INFO_INPWRLimit_Min:	//���빦����Сֵ,dBm
		  
			TXBuf[TxLen++] = (u8)(INFO_INPWRLimit_Min);
			TXBuf[TxLen++] = (u8)(INFO_INPWRLimit_Min>>8);
			TXBuf[TxLen++] = 4; 			  				
			
			FloatToByte(g_DI.Cfg.PWR_INlimit,&TXBuf[TxLen]);
			
			TxLen = TxLen + 4; 
			break;
		case INFO_RVPWRLimit_Max:	//���书�����ֵ,W
			
			TXBuf[TxLen++] = (u8)(INFO_RVPWRLimit_Max);
			TXBuf[TxLen++] = (u8)(INFO_RVPWRLimit_Max>>8);
			TXBuf[TxLen++] = 4; 
			
			FloatToByte((float)g_DI.Cfg.PWR_RVlimit,&TXBuf[TxLen]);
			
			TxLen = TxLen + 4; 
			break;

		case INFO_CURLimit_RedPwr:	//�����ʹ��ŵ�������,A
		
			TXBuf[TxLen++] = (u8)(INFO_CURLimit_RedPwr);
			TXBuf[TxLen++] = (u8)(INFO_CURLimit_RedPwr>>8);
			TXBuf[TxLen++] = 4; 			  				
			
			FloatToByte(g_DI.Cfg.PWRRED_CURlimit,&TXBuf[TxLen]);
			
			TxLen = TxLen + 4; 
			break;
		case INFO_CURLimit_OFFPwr:	//�ػ����ŵ�������,A
			
			TXBuf[TxLen++] = (u8)(INFO_CURLimit_OFFPwr);
			TXBuf[TxLen++] = (u8)(INFO_CURLimit_OFFPwr>>8);
			TXBuf[TxLen++] = 4; 			  				
			
			 FloatToByte(g_DI.Cfg.PWROFF_CURlimit,&TXBuf[TxLen]);
			
			TxLen = TxLen + 4; 
			break;
		case INFO_CURPWRRST_Time:	//���������ж���ʱ,��
			
			TXBuf[TxLen++] = (u8)(INFO_CURPWRRST_Time);
			TXBuf[TxLen++] = (u8)(INFO_CURPWRRST_Time>>8);
			TXBuf[TxLen++] = 2; 
			
			TXBuf[TxLen++] = (u8)(g_DI.Cfg.CurRstDelay);
			TXBuf[TxLen++] = (u8)(g_DI.Cfg.CurRstDelay >> 8);
	
			break;
		case INFO_CURPWRRST_Num:	//������������,��
		
			TXBuf[TxLen++] = (u8)(INFO_CURPWRRST_Num);
			TXBuf[TxLen++] = (u8)(INFO_CURPWRRST_Num>>8);
			TXBuf[TxLen++] = 1; 
			
			TXBuf[TxLen++] = g_DI.Cfg.CurRstNum;
			
			break;
		case INFO_TPTLimit_RedPwr:	//�����ʹ����¶�����,��
			
			TXBuf[TxLen++] = (u8)(INFO_TPTLimit_RedPwr);
			TXBuf[TxLen++] = (u8)(INFO_TPTLimit_RedPwr>>8);
			TXBuf[TxLen++] = 1; 
			
			TXBuf[TxLen++] = g_DI.Cfg.PWRRED_TPTlimit;
			
			break;
		case INFO_TPTLimit_OFFPwr:	//�ػ������¶�����,��
			
			TXBuf[TxLen++] = (u8)(INFO_TPTLimit_OFFPwr);
			TXBuf[TxLen++] = (u8)(INFO_TPTLimit_OFFPwr>>8);
			TXBuf[TxLen++] = 1; 
			
			TXBuf[TxLen++] = g_DI.Cfg.PWROFF_TPTlimit;
			
			break;
		case INFO_TPT_OpenFan1:	//������ 1 ���¶�,��
			
			TXBuf[TxLen++] = (u8)(INFO_TPT_OpenFan1);
			TXBuf[TxLen++] = (u8)(INFO_TPT_OpenFan1>>8);
			TXBuf[TxLen++] = 1; 
			
			TXBuf[TxLen++] = g_DI.Cfg.OpenFan1_TPTlimit;
			
			break;
		case INFO_TPT_OpenFan2:	//������ 2 ���¶�,��
			
			TXBuf[TxLen++] = (u8)(INFO_TPT_OpenFan2);
			TXBuf[TxLen++] = (u8)(INFO_TPT_OpenFan2>>8);
			TXBuf[TxLen++] = 1; 
			
			TXBuf[TxLen++] = g_DI.Cfg.OpenFan2_TPTlimit;
		
			break;
		case INFO_TPT_CloseFan:	//�ط��ȵ��¶�,��
			
			TXBuf[TxLen++] = (u8)(INFO_TPT_CloseFan);
			TXBuf[TxLen++] = (u8)(INFO_TPT_CloseFan>>8);
			TXBuf[TxLen++] = 1; 
			
			TXBuf[TxLen++] = g_DI.Cfg.CloseFan_TPTlimit;
			
			break;
		case INFO_PA_ITT://����ģ��˵��
			TXBuf[TxLen++] = (u8)(INFO_PA_ITT);
			TXBuf[TxLen++] = (u8)(INFO_PA_ITT>>8);
			TXBuf[TxLen++] = 32; 
			DataCopy((u8 *)&TXBuf[TxLen], (u8 *)&g_DI.Cfg.PaItt,32);
			TxLen = TxLen + 32;
			break;
		case INFO_PAPWR_RFOTU://�����������,W, 2�ֽ�
			TXBuf[TxLen++] = (u8)(INFO_PAPWR_RFOTU);
			TXBuf[TxLen++] = (u8)(INFO_PAPWR_RFOTU>>8);
			TXBuf[TxLen++] = 2; 
			pwruot = (int16_t)g_DI.Pa[0].Data.Power_FR;
			TXBuf[TxLen++] = (u8)(pwruot);
			TXBuf[TxLen++] = (u8)(pwruot >> 8);


			break;
		case INFO_PAPWR_INOTU://�������빦��,W��dBm, 2�ֽ�
			TXBuf[TxLen++] = (u8)(INFO_PAPWR_INOTU);
			TXBuf[TxLen++] = (u8)(INFO_PAPWR_INOTU>>8);
			TXBuf[TxLen++] = 2; 				
			pwrin = (int16_t)g_DI.Pa[0].Data.Power_IN;
			TXBuf[TxLen++] = (u8)(pwrin);
			TXBuf[TxLen++] = (u8)(pwrin >> 8);

			break;
		case INFO_PATPT://�����¶�,��,float
			TXBuf[TxLen++] = (u8)(INFO_PATPT);
			TXBuf[TxLen++] = (u8)(INFO_PATPT>>8);
			TXBuf[TxLen++] = 4; 
			FloatToByte(g_DI.Pa[0].Data.PA_TPT,&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;
		case INFO_PA_State://����״̬ bit0:���� ->0:�� 1����   bit1:������ ->0:�� 1����
							//bit2:���� ->0:�� 1����
			TXBuf[TxLen++] = (u8)(INFO_PA_State);
			TXBuf[TxLen++] = (u8)(INFO_PA_State>>8);
			TXBuf[TxLen++] = 1;
			
			if((g_DI.Pa[0].Data.ProState & 0x02) == 0x02)
			{
				pastate = 0x04;
			}					
			if((g_DI.Pa[0].Data.ProState & 0x40) == 0x40)
			{
				pastate |= 0x02;
			}
			TXBuf[TxLen++] = pastate;
			break;									
		case INFO_PA_TDVCC://�ƶ��ܹ�����ѹ, V, float
			TXBuf[TxLen++] = (u8)(INFO_PA_TDVCC);
			TXBuf[TxLen++] = (u8)(INFO_PA_TDVCC>>8);
			TXBuf[TxLen++] = 4; 
			FloatToByte(5.0,&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;
		case INFO_PA_TDCUR:	//�ƶ��ܵ���, A, float
			TXBuf[TxLen++] = (u8)(INFO_PA_TDCUR);
			TXBuf[TxLen++] = (u8)(INFO_PA_TDCUR>>8);
			TXBuf[TxLen++] = 4; 
			FloatToByte(g_DI.Pa[0].Data.PA_BECUR,&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			
			break;
		case INFO_PA_GLVCC://���ʹܹ�����ѹ, V, float
			TXBuf[TxLen++] = (u8)(INFO_PA_GLVCC);
			TXBuf[TxLen++] = (u8)(INFO_PA_GLVCC>>8);
			TXBuf[TxLen++] = 4; 
			FloatToByte(g_DI.Pa[0].Data.PA_VOL,&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;
		case INFO_PA_GLCUR1://���ʹܵ���1, A, float
		 	TXBuf[TxLen++] = (u8)(INFO_PA_GLCUR1);
			TXBuf[TxLen++] = (u8)(INFO_PA_GLCUR1>>8);
			TXBuf[TxLen++] = 4; 
			FloatToByte(g_DI.Pa[0].Data.PA_CUR1,&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;
		case INFO_PA_GLCUR2://���ʹܵ���2, A, float
			TXBuf[TxLen++] = (u8)(INFO_PA_GLCUR2);
			TXBuf[TxLen++] = (u8)(INFO_PA_GLCUR2>>8);
			TXBuf[TxLen++] = 4; 
			FloatToByte(g_DI.Pa[0].Data.PA_CUR2,&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;

		case INFO_MACH_ATT1:	//˥����1
			TXBuf[TxLen++] = (u8)(INFO_MACH_ATT1);
			TXBuf[TxLen++] = (u8)(INFO_MACH_ATT1>>8);
			TXBuf[TxLen++] = 1; 
		   	TXBuf[TxLen++] = g_DI.Pa[0].Data.RES_Rear;
			break;
		
		case INFO_MACH_ATT2:	//˥����2
			TXBuf[TxLen++] = (u8)(INFO_MACH_ATT2);
			TXBuf[TxLen++] = (u8)(INFO_MACH_ATT2>>8);
			TXBuf[TxLen++] = 1; 
		   	TXBuf[TxLen++] = g_DI.Pa[0].Data.ATT_POT1;
			break;
		
		case INFO_MACH_FANCTR:	//����
			TXBuf[TxLen++] = (u8)(INFO_MACH_FANCTR);
			TXBuf[TxLen++] = (u8)(INFO_MACH_FANCTR>>8);
			TXBuf[TxLen++] = 1; 
		
			if( SwControlGet(SW_FAN_PORT,SW_FAN1_PIN) )
				fanstate |= 0x01;		
			else
				fanstate &= 0xFE;

			if( SwControlGet(SW_FAN_PORT,SW_FAN2_PIN) )
				fanstate |= 0x04;		
			else
				fanstate &= 0xFB;

		   	TXBuf[TxLen++] = fanstate;
			break;
		case INFO_ExciterDirection://����ģ��˵��			
			TXBuf[TxLen++] = (u8)(INFO_ExciterDirection);
			TXBuf[TxLen++] = (u8)(INFO_ExciterDirection>>8);
			TXBuf[TxLen++] = 32; 
			DataCopy((u8 *)&TXBuf[TxLen], (u8 *)&g_DI.Cfg.ExcitDirection,32);
			TxLen = TxLen + 32;
			break;
		case INFO_ExciterOut_Frq:	//���������Ƶ��,Hz
			TXBuf[TxLen++] = (u8)(INFO_ExciterOut_Frq);
			TXBuf[TxLen++] = (u8)(INFO_ExciterOut_Frq>>8);
			TXBuf[TxLen++] = 4;		 
			FloatToByte((float)(g_DI.Excitdata.FMFRQ * 100.0),&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;
		case INFO_ExciterOut_FrqMHZ:	//���������Ƶ��,MHz
			TXBuf[TxLen++] = (u8)(INFO_ExciterOut_FrqMHZ);
			TXBuf[TxLen++] = (u8)(INFO_ExciterOut_FrqMHZ>>8);
			TXBuf[TxLen++] = 4;		 
			FloatToByte((float)(g_DI.Excitdata.FMFRQ/10.0),&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;
		case INFO_Exciter_ModMode://����ģʽ��0:�ر� 1:������ 2:L������ 3:R������ 4:L+R������
			TXBuf[TxLen++] = (u8)(INFO_Exciter_ModMode);
			TXBuf[TxLen++] = (u8)(INFO_Exciter_ModMode>>8);
			TXBuf[TxLen++] = 1; 
			if(g_DI.Excitdata.FMMODULATIONMODE == 0)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R������
		   	{
				TXBuf[TxLen++] = 0;				
			}
			else if(g_DI.Excitdata.FMMODULATIONMODE == 1)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R������
		   	{
				TXBuf[TxLen++] = 1;				
			}
			else if(g_DI.Excitdata.FMMODULATIONMODE == 2)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R������
		   	{
				TXBuf[TxLen++] = 2;				
			}
			else if(g_DI.Excitdata.FMMODULATIONMODE == 4)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R������
		   	{
				TXBuf[TxLen++] = 3;				
			}
			else if(g_DI.Excitdata.FMMODULATIONMODE == 6)//0:�ر� 1:������ 2:L������ 4:R������ 6:L+R������
		   	{
				TXBuf[TxLen++] = 4;				
			}
			else
			{
				TXBuf[TxLen++] = 5;				
			}
			break;
		case INFO_Exciter_PreEmp://Ԥ���� 0:0us 1:50us 2:75us
			TXBuf[TxLen++] = (u8)(INFO_Exciter_PreEmp);
			TXBuf[TxLen++] = (u8)(INFO_Exciter_PreEmp>>8);
			TXBuf[TxLen++] = 1; 
		   	TXBuf[TxLen++] = g_DI.Excitdata.FMPre_Emphasis;
			break;
		case INFO_ExciterPWR_Gain://��������
			TXBuf[TxLen++] = (u8)(INFO_ExciterPWR_Gain);
			TXBuf[TxLen++] = (u8)(INFO_ExciterPWR_Gain>>8);
			TXBuf[TxLen++] = 4; 
		   	TXBuf[TxLen++] = (u8)g_DI.Excitdata.FMGain_RF;
			TXBuf[TxLen++] = (u8)(g_DI.Excitdata.FMGain_RF>>8);
			TXBuf[TxLen++] = (u8)(g_DI.Excitdata.FMGain_RF>>16);
			TXBuf[TxLen++] = (u8)(g_DI.Excitdata.FMGain_RF>>24);
			break;	   

		case INFO_ExciterMODULATIONVAL:	//���ƶ�0~255
			TXBuf[TxLen++] = (u8)(INFO_ExciterMODULATIONVAL);
			TXBuf[TxLen++] = (u8)(INFO_ExciterMODULATIONVAL>>8);
			TXBuf[TxLen++] = 1; 
		   	TXBuf[TxLen++] = g_DI.Excitdata.FMMODULATIONVAL;
			break;
		case INFO_ExciterRDSVAL:	//RDS����
			TXBuf[TxLen++] = (u8)(INFO_ExciterRDSVAL);
			TXBuf[TxLen++] = (u8)(INFO_ExciterRDSVAL>>8);
			TXBuf[TxLen++] = 1; 
		   	TXBuf[TxLen++] = g_DI.Excitdata.FMRDSVAL;
			break;
		case INFO_ExciterCRMODULATIONVAL://���ƶȴֵ�0~7
			TXBuf[TxLen++] = (u8)(INFO_ExciterCRMODULATIONVAL);
			TXBuf[TxLen++] = (u8)(INFO_ExciterCRMODULATIONVAL>>8);
			TXBuf[TxLen++] = 1; 
		   	TXBuf[TxLen++] = g_DI.Excitdata.FMSIGNAL_VAL;
			break;
		case INFO_ExciterRDSCHN: //RDSͨ��ѡ�� 1Byte
			TXBuf[TxLen++] = (u8)(INFO_ExciterRDSCHN);
			TXBuf[TxLen++] = (u8)(INFO_ExciterRDSCHN>>8);
			TXBuf[TxLen++] = 1; 
		   	TXBuf[TxLen++] = g_DI.Excitdata.FMRDS_CHIN;
			break;
		case INFO_ExciterMODULDCT:	///������ƶ� float
			TXBuf[TxLen++] = (u8)(INFO_ExciterMODULDCT);
			TXBuf[TxLen++] = (u8)(INFO_ExciterMODULDCT>>8);
			TXBuf[TxLen++] = 4;		 
			FloatToByte((float)(g_DI.Excitdata.ModulationDCT/10.0),&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;
		case INFO_ExciterDIFMODULDCT:	//������ƶ� float
			TXBuf[TxLen++] = (u8)(INFO_ExciterDIFMODULDCT);
			TXBuf[TxLen++] = (u8)(INFO_ExciterDIFMODULDCT>>8);
			TXBuf[TxLen++] = 4;		 
			FloatToByte((float)(g_DI.Excitdata.DifModulation/10.0),&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;	
		case INFO_ExciterL_Mod:	//L���ƶ�,% float
			TXBuf[TxLen++] = (u8)(INFO_ExciterL_Mod);
			TXBuf[TxLen++] = (u8)(INFO_ExciterL_Mod>>8);
			TXBuf[TxLen++] = 4;		 
			FloatToByte((float)(g_DI.Excitdata.LeftChnModuVal/10.0),&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;	
		case INFO_ExciterR_Mod:	//R���ƶ�,% float
			TXBuf[TxLen++] = (u8)(INFO_ExciterR_Mod);
			TXBuf[TxLen++] = (u8)(INFO_ExciterR_Mod>>8);
			TXBuf[TxLen++] = 4;		 
			FloatToByte((float)(g_DI.Excitdata.RightChnModuVal/10.0),&TXBuf[TxLen]);
			TxLen = TxLen + 4;
			break;
		default:  
			TXBuf[TxLen++] = (u8)(parCode);
			TXBuf[TxLen++] = (u8)(parCode>>8);
			TXBuf[TxLen++] = 0; //�޴˲���,��������0
			break;		
	
	}

	return TxLen;


}
/********************************************************************************************************
** �������ƣ�CMD_PAProcess()
** �������ܣ��������ݴ���
** ��ڲ�����cmd:����
** ���ڲ��������ݳ���
********************************************************************************************************/
u16 CMD_ReadProcess(u8 *Buf, u16 PackLen, u8 Head)
{
	u16 i=0,a;
	u16 lenth = 0, datalen = 0;
	u8 OutBuf[500];
	u16 OutLen,CRC_Value;

	u16 ReadRegister = 0;
	datalen = PackLen - 6;

	lenth = StrToU16(Buf, 2);	


	if( datalen != lenth )
	{
//		printf("	CMD_ReadProcess datalen != lenth \r\n");
		return 0;	
	}

	OutLen = 0;
	OutBuf[OutLen++] = Head;
	OutBuf[OutLen++] = CMD_READ;
	OutBuf[OutLen++] = (u8)OutLen;
	OutBuf[OutLen++] = (u8)(OutLen>>8);

	for(i=0; i<lenth/2; i++)
	{
#if IWDG_EN
	  IWDG_ReloadCounter(); //�ֶ�ι��  
#endif		
		ReadRegister = StrToU16(Buf, 4+2*i);
		//printf("ReadRegister: 0x%x   OutLen: %d  \r\n",ReadRegister,OutLen);
		OutLen += CMD_GetParmaData(ReadRegister, &OutBuf[OutLen]);
		/*for(a=0;a<OutLen;a++)
		{
			printf("OutBuf[%d] : 0x%x \r\n",a,OutBuf[a]);
		}	
		*/
	
	}
	//printf("OutLen : %d \r\n",OutLen);
	OutBuf[2] = (u8)(OutLen-4);
	OutBuf[3] = (u8)((OutLen-4)>>8);

	CRC_Value = CRC16_GX_MODBUS((u8 *)OutBuf, OutLen);

	OutBuf[OutLen++] = (u8)(CRC_Value);
	OutBuf[OutLen++] = (u8)(CRC_Value>>8);	
	DataCopy( g_UART_TBuf, OutBuf, OutLen );	
	//printf("	CMD_ReadProcess OutLen = %d \r\n",OutLen);
	//OSTimeDlyHMSM(0, 0, 0, 200);
	return OutLen;
}
/********************************************************************************************************
** �������ƣ�CMD_SetProcess()
** �������ܣ��������ݴ���
** ��ڲ�����cmd:����
** ���ڲ��������ݳ���
********************************************************************************************************/
u16 CMD_SetProcess(u8 *Buf, u16 PackLen, u8 Head)
{
	u16 i=0,a=0;
	float Crtval;
	float fin,fout,ftmp;
	u16 lenth = 0, datalen = 0;
	u8 TXBuf[200];
	u8 TXErrBuf[200];
	u8 SendBuffer[200];
	u16 TxErrLen = 0,Errorflag = 0;
	u16 TxLen,CRC_Value;
	u8 PaOperData = 0;
	u16 PaTxLen = 0;
	u8 Result;
	u8 pastate = 0;
	u16 Registerval = 0;
	u16 ctmep = 0;
	u8  writelenth = 0, parlenth = 0 ;
	u8  curlenth = 0;
	u8 err;
	u8 MachFanCtr = 0;
	u8 BlockNum = 0;
	
	Sys_Flag.Bit.fSave = FALSE;	
	Sys_Flag.Bit.fAutoMsgSave = FALSE;
	Sys_Flag.Bit.fSaveStart = FALSE;
	SysNet_Flag.Bit.fPaOutCrt = FALSE;
	SysNet_Flag.Bit.fPaInCrt = FALSE;
	SysNet_Flag.Bit.fPaRvCrt = FALSE;
	
	datalen = PackLen - 6;

	lenth = StrToU16(Buf, 2);	


	if( datalen != lenth )
	{
//		printf("	CMD_SetProcess datalen != lenth \r\n");
		return 0;	
	}
	Errorflag = 0;
	TxErrLen = 0;
	TXErrBuf[TxErrLen++] = Head;
	TXErrBuf[TxErrLen++] = CMD_WRITEERR;
	TXErrBuf[TxErrLen++] = (u8)TxErrLen;
	TXErrBuf[TxErrLen++] = (u8)(TxErrLen>>8);

	TxLen = 0;
	TXBuf[TxLen++] = Head;
	TXBuf[TxLen++] = CMD_WRITE;
	TXBuf[TxLen++] = (u8)TxLen;
	TXBuf[TxLen++] = (u8)(TxLen>>8);
	curlenth = 4;
	//for(i=0; i<lenth/2; i++)
	while( curlenth < (PackLen -2) )
	{
		    
#if IWDG_EN
	  IWDG_ReloadCounter(); //�ֶ�ι��  
#endif				
		Registerval = StrToU16(Buf, curlenth);
		//printf("Registerval: %d  \r\n",Registerval);
	
		switch(Registerval)
		{
			case INFO_STATIONINFO:	//�ص�(̨վ)����
				TXBuf[TxLen++] = (u8)(INFO_STATIONINFO);
				TXBuf[TxLen++] = (u8)(INFO_STATIONINFO>>8);
				writelenth = Buf[curlenth + 2]; //д��������
						
				DataCopy((u8 *)&g_DI.Cfg.StationInfo,(u8 *)&Buf[curlenth + 3], writelenth);
			
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;
				//printf("parlenth: %d  \r\n",parlenth);
				break;
							
			case INFO_DEVICE_NAME:	//�豸����
				TXBuf[TxLen++] = (u8)(INFO_DEVICE_NAME);
				TXBuf[TxLen++] = (u8)(INFO_DEVICE_NAME>>8);
				writelenth = Buf[curlenth + 2]; //д��������

				DataCopy( (u8 *)&g_DI.Cfg.DeviceName,(u8 *)&Buf[curlenth + 3],writelenth);
				
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;
				break;
			
			
			case INFO_DEVICE_ID:
				TXBuf[TxLen++] = (u8)(INFO_DEVICE_ID);
				TXBuf[TxLen++] = (u8)(INFO_DEVICE_ID>>8);
				writelenth = Buf[curlenth + 2]; //д�������� 
				g_DI.Cfg.DeviceID = Buf[curlenth + 3];
			
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;
			 	break;
			
			case INFO_IP_ADDR:	  //���豸IP��ַ

				TXBuf[TxLen++] = (u8)(INFO_IP_ADDR);
				TXBuf[TxLen++] = (u8)(INFO_IP_ADDR>>8);
				writelenth = Buf[curlenth + 2]; //д�������� 
				DataCopy( (u8 *)&g_DI.Cfg.IP_Addr,(u8 *)&Buf[curlenth + 3],writelenth);
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fNetRecom = TRUE;
				Sys_Flag.Bit.fSave = TRUE;

				break;

			case INFO_MASK_ADDR:	//��������

				 TXBuf[TxLen++] = (u8)(INFO_MASK_ADDR);
				 TXBuf[TxLen++] = (u8)(INFO_MASK_ADDR>>8);
				 writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_Mask,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
 
				 break;

			case INFO_GATEWAY_ADDR:	 //Ĭ������

				 TXBuf[TxLen++] = (u8)(INFO_GATEWAY_ADDR);
				 TXBuf[TxLen++] = (u8)(INFO_GATEWAY_ADDR>>8);
				 writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_GateWay,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;

				 break;

			case INFO_IP_LOCAL1:	//���ع���1-IP	
				 TXBuf[TxLen++] = (u8)(INFO_IP_LOCAL1);
				 TXBuf[TxLen++] = (u8)(INFO_IP_LOCAL1>>8);
				 writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_LOCAL1,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;

				break;
					 
			case INFO_IP_LOCAL2:	//���ع���2-IP
				 TXBuf[TxLen++] = (u8)(INFO_IP_LOCAL2);
				 TXBuf[TxLen++] = (u8)(INFO_IP_LOCAL2>>8);
				 writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_LOCAL2,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;

				break;

			case INFO_IP_INSERVER1:	//�ڲ�����1-IP
				 TXBuf[TxLen++] = (u8)(INFO_IP_INSERVER1);
				 TXBuf[TxLen++] = (u8)(INFO_IP_INSERVER1>>8);
				 writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_INSERVER1,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
	
				break;

			case INFO_IP_INPORT1:	//�ڲ�����1-IP
				 TXBuf[TxLen++] = (u8)(INFO_IP_INPORT1);
				 TXBuf[TxLen++] = (u8)(INFO_IP_INPORT1>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.INSERVERNetPort1 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
			  	 		
				break;

			case INFO_IP_INSERVER2:	//�ڲ�����2-IP
				 TXBuf[TxLen++] = (u8)(INFO_IP_INSERVER2);
				 TXBuf[TxLen++] = (u8)(INFO_IP_INSERVER2>>8);
				 writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_INSERVER2,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
	
				break;

			case INFO_IP_INPORT2:	//�ڲ�����2-IP
				 TXBuf[TxLen++] = (u8)(INFO_IP_INPORT2);
				 TXBuf[TxLen++] = (u8)(INFO_IP_INPORT2>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.INSERVERNetPort2 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE; 
				break;


			case INFO_IP_SERVER1:	//Զ������1-IP
				 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER1);
				 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER1>>8);
				 writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_RemoteHost1,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;

				break;

			case INFO_Net_Port1:	//Զ������1-�˿ں�
				 TXBuf[TxLen++] = (u8)(INFO_Net_Port1);
				 TXBuf[TxLen++] = (u8)(INFO_Net_Port1>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostNetPort1 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE; 
			  	 
				break;
			case INFO_FActive_Time1:	//��1���������1
				 TXBuf[TxLen++] = (u8)(INFO_FActive_Time1);
				 TXBuf[TxLen++] = (u8)(INFO_FActive_Time1>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay1_1 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
				  
		
				break;

			case INFO_FActive_Time2:	//��1���������2
				 TXBuf[TxLen++] = (u8)(INFO_FActive_Time2);
				 TXBuf[TxLen++] = (u8)(INFO_FActive_Time2>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay1_2 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE; 
			  	 						
				break;
	
			case INFO_FActive_Time3:	//��1���������3
				 TXBuf[TxLen++] = (u8)(INFO_FActive_Time3);
				 TXBuf[TxLen++] = (u8)(INFO_FActive_Time3>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay1_3 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE; 
	
				break;
						 
			case INFO_IP_SERVER2:	//Զ������2-IP
				 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER2);
				 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER2>>8);
				  writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_RemoteHost2,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;					
				break;
	
			case INFO_Net_Port2:	//Զ������2-�˿ں�
				 TXBuf[TxLen++] = (u8)(INFO_Net_Port2);
				 TXBuf[TxLen++] = (u8)(INFO_Net_Port2>>8);
				  writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostNetPort2 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
					
				break;
	
			case INFO_SActive_Time1:	//��2���������1
				 TXBuf[TxLen++] = (u8)(INFO_SActive_Time1);
				 TXBuf[TxLen++] = (u8)(INFO_SActive_Time1>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay2_1 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
								
				break;
	
			case INFO_SActive_Time2:	//��2���������2	
				 TXBuf[TxLen++] = (u8)(INFO_SActive_Time2);
				 TXBuf[TxLen++] = (u8)(INFO_SActive_Time2>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay2_2 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE; 
					
				break;
	
			case INFO_SActive_Time3:	//��2���������3
				 TXBuf[TxLen++] = (u8)(INFO_SActive_Time3);
				 TXBuf[TxLen++] = (u8)(INFO_SActive_Time3>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay2_3 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
				
				break;
		
			case INFO_IP_SERVER3:	//Զ������3-IP
				 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER3);
				 TXBuf[TxLen++] = (u8)(INFO_IP_SERVER3>>8);
				 writelenth = Buf[curlenth + 2]; //д�������� 
				 DataCopy( (u8 *)&g_DI.Cfg.IP_RemoteHost3,(u8 *)&Buf[curlenth + 3],writelenth);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
					
				break;
	
			case INFO_Net_Port3:	//Զ������3-�˿ں�
				 TXBuf[TxLen++] = (u8)(INFO_Net_Port3);
				 TXBuf[TxLen++] = (u8)(INFO_Net_Port3>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostNetPort3 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE; 
					
				break;
	
			case INFO_TActive_Time1:	//��3���������1
				 TXBuf[TxLen++] = (u8)(INFO_TActive_Time1);
				 TXBuf[TxLen++] = (u8)(INFO_TActive_Time1>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay3_1 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
					
				break;
	
			case INFO_TActive_Time2:	//��3���������2	
				 TXBuf[TxLen++] = (u8)(INFO_TActive_Time2);
				 TXBuf[TxLen++] = (u8)(INFO_TActive_Time2>>8);
				 
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay3_2 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;
						
				break;
	
			case INFO_TActive_Time3:	//��3��������3
			 	 TXBuf[TxLen++] = (u8)(INFO_TActive_Time3);
				 TXBuf[TxLen++] = (u8)(INFO_TActive_Time3>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.Cfg.RemoteHostDelay3_3 = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8);
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fSave = TRUE;  
							
				break;

			case INFO_AutoMsg_Cfg1:	//��1���Զ���������
			 	 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg1);
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg1>>8);
				 
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.AutoMsgCfg.AutoMsg_Cfg1_Len = writelenth ;
				 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_Cfg1_Len/2; a++) 
				 {
				   	g_DI.AutoMsgCfg.AutoMsg_Cfg1[a] = Buf[curlenth + 3 + 2*a] | (Buf[curlenth + 3 + 2*a +1]<<8);

				 }
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fAutoMsgSave = TRUE;	  	 
				break;
			
			case INFO_AutoMsg_Cfg2:	//��2���Զ���������
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg2);
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg2>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.AutoMsgCfg.AutoMsg_Cfg2_Len = writelenth ;
				 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_Cfg2_Len/2; a++) 
				 {
				   	g_DI.AutoMsgCfg.AutoMsg_Cfg2[a] = Buf[curlenth + 3 + 2*a] | (Buf[curlenth + 3 + 2*a +1]<<8);

				 }
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fAutoMsgSave = TRUE;
				break;
			
			case INFO_AutoMsg_Cfg3:	//��3���Զ���������
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg3);
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_Cfg3>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.AutoMsgCfg.AutoMsg_Cfg3_Len = writelenth ;
				 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_Cfg3_Len/2; a++) 
				 {
				   	g_DI.AutoMsgCfg.AutoMsg_Cfg3[a] = Buf[curlenth + 3 + 2*a] | (Buf[curlenth + 3 + 2*a +1]<<8);

				 }
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fAutoMsgSave = TRUE;
				break;
			
			case INFO_AutoMsg_INCfg1:	//�ڲ���������1����
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_INCfg1);
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_INCfg1>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.AutoMsgCfg.AutoMsg_INCfg1_Len = writelenth ;
				 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_INCfg1_Len/2; a++) 
				 {
				   	g_DI.AutoMsgCfg.AutoMsg_INCfg1[a] = Buf[curlenth + 3 + 2*a] | (Buf[curlenth + 3 + 2*a +1]<<8);

				 }
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fAutoMsgSave = TRUE;
				break;
			
			case INFO_AutoMsg_INCfg2:	//�ڲ���������2����
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_INCfg2);
				 TXBuf[TxLen++] = (u8)(INFO_AutoMsg_INCfg2>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 g_DI.AutoMsgCfg.AutoMsg_INCfg2_Len = writelenth ;
				 for(a=0; a<g_DI.AutoMsgCfg.AutoMsg_INCfg2_Len/2; a++) 
				 {
				   	g_DI.AutoMsgCfg.AutoMsg_INCfg2[a] = Buf[curlenth + 3 + 2*a] | (Buf[curlenth + 3 + 2*a +1]<<8);

				 }
				 parlenth = writelenth + 3;
				 Sys_Flag.Bit.fAutoMsgSave = TRUE;
				break;
									
			case INFO_AllPWR_Int:	   //���ʳ�ʼ��,W

				 TXBuf[TxLen++] = (u8)(INFO_AllPWR_Int);
				 TXBuf[TxLen++] = (u8)(INFO_AllPWR_Int>>8);
				 writelenth = Buf[curlenth + 2]; //д��������
				 parlenth = writelenth + 3;

				 ATT_Senddata(0);
				 AP_Startup();
				 g_DI.Cfg.PWR_RateOut = Hex_To_Decimal(Buf,curlenth + 3);
				 
				 if(g_DI.Cfg.PWR_RateOut >= (g_DI.Cfg.PWR_SetRateOut*1.1))
				 {
					 g_DI.Cfg.PWR_RateOut = g_DI.Cfg.PWR_SetRateOut*1.1;
				 }
				 g_DI.Cfg.P_AutoContrl = 1;
				 g_DI.Pa[0].Data.RES_Front = 0;
			     g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;
				 Sys_Flag.Bit.fSaveStart = TRUE;
				 Sys_Flag.Bit.fSave = TRUE;	
				 break;
			case INFO_INPWR_CRTVAL:	//���빦��У��ֵ
					TXBuf[TxLen++] = (u8)(INFO_INPWR_CRTVAL);
					TXBuf[TxLen++] = (u8)(INFO_INPWR_CRTVAL>>8);
					writelenth = Buf[curlenth + 2]; //д��������
//					printf("writelenth: %d  \n",writelenth);
					Crtval = Hex_To_Decimal(Buf,curlenth + 3);
					parlenth = writelenth + 3;
					
					PaInfout =  Crtval;
					SysNet_Flag.Bit.fPaInCrt = TRUE;
				break;
			case INFO_OUTPWR_CRTVAL:	//�������У��ֵ
					TXBuf[TxLen++] = (u8)(INFO_OUTPWR_CRTVAL);
					TXBuf[TxLen++] = (u8)(INFO_OUTPWR_CRTVAL>>8);
					writelenth = Buf[curlenth + 2]; //д��������
//					printf("writelenth: %d  \n",writelenth);
					Crtval = Hex_To_Decimal(Buf,curlenth + 3);
					parlenth = writelenth + 3;
					
					PaOutfout =  Crtval;
					SysNet_Flag.Bit.fPaOutCrt = TRUE;		
				break;
			case INFO_RVPWR_CRTVAL:	//���书��У��ֵ
					TXBuf[TxLen++] = (u8)(INFO_RVPWR_CRTVAL);
					TXBuf[TxLen++] = (u8)(INFO_RVPWR_CRTVAL>>8);
					writelenth = Buf[curlenth + 2]; //д��������
//					printf("writelenth: %d  \n",writelenth);
					Crtval = Hex_To_Decimal(Buf,curlenth + 3);
					parlenth = writelenth + 3;
					
					PaRvfout =  Crtval;
					SysNet_Flag.Bit.fPaRvCrt = TRUE;
				break;
			case INFO_MACH_SETON:	//����(ר��)  4F 4E 
				TXBuf[TxLen++] = (u8)(INFO_MACH_SETON);
				TXBuf[TxLen++] = (u8)(INFO_MACH_SETON>>8);
				writelenth = Buf[curlenth + 2]; //д��������
				parlenth = writelenth + 3;
				
				AP_Startup();
			    g_DI.Pa[0].Data.P_DownOffset = 0;
			    g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;
				if(g_DI.Cfg.P_AutoContrl == 0)
				{
					ManualChanging = 0;
				}
				break;			
			case INFO_MACH_SETOFF:	//�ػ�(ר��) 4F 46 46 
				TXBuf[TxLen++] = (u8)(INFO_MACH_SETOFF);
				TXBuf[TxLen++] = (u8)(INFO_MACH_SETOFF>>8);
				writelenth = Buf[curlenth + 2]; //д��������
				parlenth = writelenth + 3;
				AP_Shutdown();
				ATT_Senddata(0);
				g_DI.Pa[0].Data.P_DownOffset = 0;
				break;
			case INFO_MACH_CTR:	//��������λ,�Զ�
				TXBuf[TxLen++] = (u8)(INFO_MACH_CTR);
				TXBuf[TxLen++] = (u8)(INFO_MACH_CTR>>8);
				writelenth = Buf[curlenth + 2]; //д��������
				parlenth = writelenth + 3;
				if((Buf[curlenth + 3] & 0x01) == 0x01)
				{
					g_DI.Pa[0].Data.P_DownOffset = 0;
					AP_Startup();	
					g_DI.Cfg.P_AutoContrl = 1;
					g_DI.Pa[0].Data.RES_Front = 0;
					Sys_Flag.Bit.fSaveStart = TRUE;
					Sys_Flag.Bit.fSave = TRUE;			
					g_DI.Pa[0].Data.I_OverLoadRestartCount = 0;
				}

				break;
			case INFO_SETPWR_Rated:	//�趨�������,W
				
				TXBuf[TxLen++] = (u8)(INFO_SETPWR_Rated);
				TXBuf[TxLen++] = (u8)(INFO_SETPWR_Rated>>8);
							
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.PWR_SetRateOut = (u8)(Hex_To_Decimal(Buf,curlenth + 3));
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			case INFO_INPWRLimit_Min:	//���빦����Сֵ,dBm
			   
				TXBuf[TxLen++] = (u8)(INFO_INPWRLimit_Min);
				TXBuf[TxLen++] = (u8)(INFO_INPWRLimit_Min>>8);
			  				
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.PWR_INlimit = Hex_To_Decimal(Buf,curlenth + 3);
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			case INFO_RVPWRLimit_Max:	//���书�����ֵ,W
			
				TXBuf[TxLen++] = (u8)(INFO_RVPWRLimit_Max);
				TXBuf[TxLen++] = (u8)(INFO_RVPWRLimit_Max>>8);
				
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.PWR_RVlimit = Hex_To_Decimal(Buf,curlenth + 3);
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;

			case INFO_CURLimit_RedPwr:	//�����ʹ��ŵ�������,A
			
				TXBuf[TxLen++] = (u8)(INFO_CURLimit_RedPwr);
				TXBuf[TxLen++] = (u8)(INFO_CURLimit_RedPwr>>8);
			
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.PWRRED_CURlimit = Hex_To_Decimal(Buf,curlenth + 3);
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;
	  				
				break;
			case INFO_CURLimit_OFFPwr:	//�ػ����ŵ�������,A
			
				TXBuf[TxLen++] = (u8)(INFO_CURLimit_OFFPwr);
				TXBuf[TxLen++] = (u8)(INFO_CURLimit_OFFPwr>>8);

 			  	writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.PWROFF_CURlimit = Hex_To_Decimal(Buf,curlenth + 3);
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;
						
				break;
			case INFO_CURPWRRST_Time:	//���������ж���ʱ,��
				
				TXBuf[TxLen++] = (u8)(INFO_CURPWRRST_Time);
				TXBuf[TxLen++] = (u8)(INFO_CURPWRRST_Time>>8);
			
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.CurRstDelay = Buf[curlenth + 3 ] | (Buf[curlenth + 4]<<8);
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			case INFO_CURPWRRST_Num:	//������������,��
			
				TXBuf[TxLen++] = (u8)(INFO_CURPWRRST_Num);
				TXBuf[TxLen++] = (u8)(INFO_CURPWRRST_Num>>8);
			
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.CurRstNum = Buf[curlenth + 3 ] ;
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			case INFO_TPTLimit_RedPwr:	//�����ʹ����¶�����,��
				
				TXBuf[TxLen++] = (u8)(INFO_TPTLimit_RedPwr);
				TXBuf[TxLen++] = (u8)(INFO_TPTLimit_RedPwr>>8);
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.PWRRED_TPTlimit = Buf[curlenth + 3 ] ;
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			case INFO_TPTLimit_OFFPwr:	//�ػ������¶�����,��
			
				TXBuf[TxLen++] = (u8)(INFO_TPTLimit_OFFPwr);
				TXBuf[TxLen++] = (u8)(INFO_TPTLimit_OFFPwr>>8);
				
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.PWROFF_TPTlimit = Buf[curlenth + 3 ] ;
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			case INFO_TPT_OpenFan1:	//������ 1 ���¶�,��
				
				TXBuf[TxLen++] = (u8)(INFO_TPT_OpenFan1);
				TXBuf[TxLen++] = (u8)(INFO_TPT_OpenFan1>>8);
			
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.OpenFan1_TPTlimit = Buf[curlenth + 3 ] ;
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			case INFO_TPT_OpenFan2:	//������ 2 ���¶�,��
				
				TXBuf[TxLen++] = (u8)(INFO_TPT_OpenFan2);
				TXBuf[TxLen++] = (u8)(INFO_TPT_OpenFan2>>8);
				
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.OpenFan2_TPTlimit = Buf[curlenth + 3 ] ;
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			case INFO_TPT_CloseFan:	//�ط��ȵ��¶�,��
				
				TXBuf[TxLen++] = (u8)(INFO_TPT_CloseFan);
				TXBuf[TxLen++] = (u8)(INFO_TPT_CloseFan>>8);

				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.CloseFan_TPTlimit = Buf[curlenth + 3 ] ;
				parlenth = writelenth + 3; 
				Sys_Flag.Bit.fSave = TRUE;

				break;
			
			case INFO_MACH_ATT1:	//˥����1
				
				TXBuf[TxLen++] = (u8)(INFO_MACH_ATT1);
				TXBuf[TxLen++] = (u8)(INFO_MACH_ATT1>>8);

				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.P_Res_Front_Target = Buf[curlenth + 3 ] ;
				parlenth = writelenth + 3; 

				if(g_DI.Cfg.P_Res_Front_Target <=0)
				{
					g_DI.Cfg.P_Res_Front_Target = 0;
				}
				else if(g_DI.Cfg.P_Res_Front_Target >=62)
				{
					g_DI.Cfg.P_Res_Front_Target = 62;
				}
				
				ATT_Senddata( g_DI.Cfg.P_Res_Front_Target  );

				g_DI.Cfg.P_AutoContrl =0;
				Sys_Flag.Bit.fSave = TRUE;
				break;

			case INFO_MACH_FANCTR:	//�������ȿ���λ
				
				TXBuf[TxLen++] = (u8)(INFO_MACH_FANCTR);
				TXBuf[TxLen++] = (u8)(INFO_MACH_FANCTR>>8);

				writelenth = Buf[curlenth + 2]; //д��������
			  	MachFanCtr = Buf[curlenth + 3 ] ;
				parlenth = writelenth + 3; 

				if(MachFanCtr & 0x01) //����1 1:�� 0:��
				{
					SW_FAN1_ON;
					FAN_ManCtrl_Delay = 60;
				}
				else
				{
					SW_FAN1_OFF;
					FAN_ManCtrl_Delay = 60;
				}
				
				if(MachFanCtr & 0x04) //����2 1:�� 0:��
				{
					SW_FAN2_ON;
					FAN_ManCtrl_Delay = 60;
				}
				else
				{
					SW_FAN2_OFF;
					FAN_ManCtrl_Delay = 60;
				}

				break;
				
			case INFO_PA_ITT://����ģ��˵��
				TXBuf[TxLen++] = (u8)(INFO_PA_ITT);
				TXBuf[TxLen++] = (u8)(INFO_PA_ITT>>8);
				
				writelenth = Buf[curlenth + 2]; //д��������
						
				DataCopy((u8 *)&g_DI.Cfg.PaItt,(u8 *)&Buf[curlenth + 3], writelenth);
			
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			
			case INFO_ExciterDirection://����ģ��˵��			
				TXBuf[TxLen++] = (u8)(INFO_ExciterDirection);
				TXBuf[TxLen++] = (u8)(INFO_ExciterDirection>>8);

				writelenth = Buf[curlenth + 2]; //д��������
						
				DataCopy((u8 *)&g_DI.Cfg.ExcitDirection,(u8 *)&Buf[curlenth + 3], writelenth);
			
				parlenth = writelenth + 3;
				Sys_Flag.Bit.fSave = TRUE;

				break;
			
			case INFO_ExciterOut_FrqMHZ:	//���������Ƶ��,MHz
				TXBuf[TxLen++] = (u8)(INFO_ExciterOut_FrqMHZ);
				TXBuf[TxLen++] = (u8)(INFO_ExciterOut_FrqMHZ>>8);
				
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Cfg.P_FmFrq = (Hex_To_Decimal(Buf,curlenth + 3) +0.0002);
				parlenth = writelenth + 3;
				
				g_DI.Excitdata.FMFRQ = (u16)(g_DI.Cfg.P_FmFrq*10.0) ;			
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x00;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMFRQ;
				SendBuffer[10]= (u8)(g_DI.Excitdata.FMFRQ>>8);	
				Excit_RS232_Send(SendBuffer,11, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;

				Sys_Flag.Bit.fSaveStart = TRUE;
				Sys_Flag.Bit.fSave = TRUE;
				break;
				
			case INFO_ExciterOut_Frq:	//���������Ƶ��,KHz
				TXBuf[TxLen++] = (u8)(INFO_ExciterOut_Frq);
				TXBuf[TxLen++] = (u8)(INFO_ExciterOut_Frq>>8);
				
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Excitdata.FMFRQ = (u16)(Hex_To_Decimal(Buf,curlenth + 3)/100);
				parlenth = writelenth + 3;
				
				g_DI.Cfg.P_FmFrq = (float)(g_DI.Excitdata.FMFRQ/10.0);			
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x00;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMFRQ;
				SendBuffer[10]= (u8)(g_DI.Excitdata.FMFRQ>>8);	
				Excit_RS232_Send(SendBuffer,11, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;

				Sys_Flag.Bit.fSaveStart = TRUE;
				Sys_Flag.Bit.fSave = TRUE;
				break;

			case INFO_Exciter_ModMode://����ģʽ��0:������ 1:L+R ������ 2:L ������ 3:������
				TXBuf[TxLen++] = (u8)(INFO_Exciter_ModMode);
				TXBuf[TxLen++] = (u8)(INFO_Exciter_ModMode>>8);
				writelenth = Buf[curlenth + 2]; //д��������
				if(Buf[curlenth + 3] == 0)
				{
					g_DI.Excitdata.FMMODULATIONMODE = 0;
				}
				else if(Buf[curlenth + 3] == 1)
				{
					g_DI.Excitdata.FMMODULATIONMODE =1;
				}	
				else if(Buf[curlenth + 3] == 2)
				{
					g_DI.Excitdata.FMMODULATIONMODE = 2;
				}
				else if(Buf[curlenth + 3] == 3)
				{
					g_DI.Excitdata.FMMODULATIONMODE = 4;
				}
				else if(Buf[curlenth + 3] == 4)
				{
					g_DI.Excitdata.FMMODULATIONMODE = 6;
				}
			  	g_DI.Excitdata.FMMODULATIONMODE = Buf[curlenth + 3];
				parlenth = writelenth + 3;
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x01;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMMODULATIONMODE;	
				Excit_RS232_Send(SendBuffer,10, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;	
				break;
			case INFO_Exciter_PreEmp://Ԥ���� 0:0us 1:50us 2:75us
				TXBuf[TxLen++] = (u8)(INFO_Exciter_PreEmp);
				TXBuf[TxLen++] = (u8)(INFO_Exciter_PreEmp>>8);
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Excitdata.FMPre_Emphasis = Buf[curlenth + 3];
				parlenth = writelenth + 3; 
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x02;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMPre_Emphasis;	
				Excit_RS232_Send(SendBuffer,10, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;
				break;

			case INFO_ExciterPWR_Gain://��������
				TXBuf[TxLen++] = (u8)(INFO_ExciterPWR_Gain);
				TXBuf[TxLen++] = (u8)(INFO_ExciterPWR_Gain>>8);
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Excitdata.FMGain_RF = Buf[curlenth + 3] | (Buf[curlenth + 4]<<8)
											| (Buf[curlenth + 5]<<16)| (Buf[curlenth + 6]<<24);
				parlenth = writelenth + 3; 
				if(g_DI.Excitdata.FMGain_RF >= 63)
				{
					g_DI.Excitdata.FMGain_RF = 63;
				}
				//���͵������� 
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x09;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMGain_RF;	
				Excit_RS232_Send(SendBuffer,10, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE; 
				break;

			case INFO_ExciterMODULATIONVAL://���ƶ�0~255
				TXBuf[TxLen++] = (u8)(INFO_ExciterMODULATIONVAL);
				TXBuf[TxLen++] = (u8)(INFO_ExciterMODULATIONVAL>>8);
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Excitdata.FMMODULATIONVAL = Buf[curlenth + 3];
				parlenth = writelenth + 3; 
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x0C;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMMODULATIONVAL;	
				Excit_RS232_Send(SendBuffer,10, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;
				break;

			case INFO_ExciterRDSVAL://RDS����
				TXBuf[TxLen++] = (u8)(INFO_ExciterRDSVAL);
				TXBuf[TxLen++] = (u8)(INFO_ExciterRDSVAL>>8);
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Excitdata.FMRDSVAL = Buf[curlenth + 3];
				parlenth = writelenth + 3; 
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x0A;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMRDSVAL;	
				Excit_RS232_Send(SendBuffer,10, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;
				break;
				
			case INFO_ExciterCRMODULATIONVAL://RDS����
				TXBuf[TxLen++] = (u8)(INFO_ExciterCRMODULATIONVAL);
				TXBuf[TxLen++] = (u8)(INFO_ExciterCRMODULATIONVAL>>8);
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Excitdata.FMSIGNAL_VAL = Buf[curlenth + 3];
				parlenth = writelenth + 3; 
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x11;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMSIGNAL_VAL;	
				Excit_RS232_Send(SendBuffer,10, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;
				break;
				
			case INFO_ExciterRDSCHN: //RDSͨ��ѡ�� 1Byte  0:�������룻1:ģ���ź�����
				TXBuf[TxLen++] = (u8)(INFO_ExciterRDSCHN);
				TXBuf[TxLen++] = (u8)(INFO_ExciterRDSCHN>>8);
				writelenth = Buf[curlenth + 2]; //д��������
			  	g_DI.Excitdata.FMRDS_CHIN = Buf[curlenth + 3];
				parlenth = writelenth + 3; 
				if(g_DI.Excitdata.FMRDS_CHIN >=2)
				{
					g_DI.Excitdata.FMRDS_CHIN = 0;
				}
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x0B;
				SendBuffer[9]= (u8)g_DI.Excitdata.FMRDS_CHIN;	
				Excit_RS232_Send(SendBuffer,10, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;
				break;
				
		 	case INFO_ExciterRDSDATA: //RDS���� 8Byte
				TXBuf[TxLen++] = (u8)(INFO_ExciterRDSDATA);
				TXBuf[TxLen++] = (u8)(INFO_ExciterRDSDATA>>8);
				writelenth = Buf[curlenth + 2]; //д��������
				parlenth = writelenth + 3; 
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x07;
				DataCopy((u8 *)&SendBuffer[9], (u8 *)&Buf[curlenth + 3],8);
				Excit_RS232_Send(SendBuffer,17, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;
				break;
				
			case INFO_ExciterRDSBLOCKDATA: //RDS������ 8*nByte
				TXBuf[TxLen++] = (u8)(INFO_ExciterRDSBLOCKDATA);
				TXBuf[TxLen++] = (u8)(INFO_ExciterRDSBLOCKDATA>>8);
				writelenth = Buf[curlenth + 2]; //д��������
				parlenth = writelenth + 3; 
				BlockNum = (u8)(writelenth/8);
				//���͵�������
				fEitRTU = TRUE;
				OSSemPend(JLQRS232SEM, 0, &err);
				SendBuffer[0]= g_DI.Cfg.Excit_IPHead; 
				SendBuffer[1]= 0x35;
				SendBuffer[2]= 0xC3;
				SendBuffer[3]= 0xFC;
				SendBuffer[4]= 0xC1;
				SendBuffer[5]= 0xEE;
				SendBuffer[6]= 0xC9;
				SendBuffer[7]= 0xE8;
				SendBuffer[8]= 0x1E;
				SendBuffer[9]= BlockNum;
				DataCopy((u8 *)&SendBuffer[10], (u8 *)&Buf[curlenth + 3],8*BlockNum);
				Excit_RS232_Send(SendBuffer,8*BlockNum+10, TIMEOUT); 
				OSSemPost(JLQRS232SEM);
				fEitRTU = FALSE;
				break;	
			case INFO_MACH_RSTFACTORYDATA:	//�����ָ���������
			   
				TXBuf[TxLen++] = (u8)(INFO_MACH_RSTFACTORYDATA);
				TXBuf[TxLen++] = (u8)(INFO_MACH_RSTFACTORYDATA>>8);
			  				
				writelenth = Buf[curlenth + 2]; //д��������
			  	if(Buf[curlenth + 3] == 0x01)
				{

					ReadbufFromX5323(2000,Save_Size,(u8*)&g_DI.Cfg);
					OSTimeDlyHMSM(0, 0, 0, 70);
					WritebufToX5323(10,Save_Size+50,(u8*)&g_DI.Cfg);
					OSTimeDlyHMSM(0, 0, 0, 70);
					ReadbufFromX5323(10,Save_Size,(u8*)&g_DI.Cfg);
				}
				parlenth = writelenth + 3;


				break;
			default:			

				Errorflag = 1;
						
				TXErrBuf[TxErrLen++] = (u8)(Registerval);
				TXErrBuf[TxErrLen++] = (u8)(Registerval>>8);
			
				writelenth = Buf[curlenth + 2]; //д��������
				parlenth = writelenth + 3;

				break;		
		
		}
		curlenth = curlenth + parlenth;
		
	}
		
	if((Sys_Flag.Bit.fAutoMsgSave == TRUE)||(Sys_Flag.Bit.fSave == TRUE))
	{
		Sys_Flag.Bit.fSaveStart = TRUE;
	}
 
	if(Errorflag == 1)
	{
		TXErrBuf[2] = (u8)(TxErrLen-4);
		TXErrBuf[3] = (u8)((TxErrLen-4)>>8);

		CRC_Value = CRC16_GX_MODBUS((u8 *)TXErrBuf, TxErrLen);
		 
		TXErrBuf[TxErrLen++] = (u8)(CRC_Value);
		TXErrBuf[TxErrLen++] = (u8)(CRC_Value>>8);	
		DataCopy( g_UART_TBuf, TXErrBuf, TxErrLen );	

		return TxErrLen;
	}
	else
	{
	 	TXBuf[2] = (u8)(TxLen-4);
		TXBuf[3] = (u8)((TxLen-4)>>8);
	
		CRC_Value = CRC16_GX_MODBUS((u8 *)TXBuf, TxLen);
	
		TXBuf[TxLen++] = (u8)(CRC_Value);
		TXBuf[TxLen++] = (u8)(CRC_Value>>8);	
		DataCopy( g_UART_TBuf, TXBuf, TxLen );	
	
		return TxLen;
	
	}
	

	

}

void CMD_BC_Analys(char *recdata,u8 reclen,char *outdata,u16 *outlen)
{
	u8 cmd,devaddr;  
	u16 datalen,regIndex,i;
	u16 crctmp = CRC16_GX_MODBUS((u8 *)recdata, reclen-2);			// ����CRC
	(*outlen) = 0;
	if( (recdata[reclen-2] != (crctmp%256)) || (recdata[reclen-1] != (crctmp/256)))
		return;
	
	cmd = recdata[1];
	regIndex = 2;
	if( cmd == 0x5A )
	{
		datalen = recdata[regIndex] + recdata[regIndex+1]*256; 
		if( datalen == 0x0000 )
		{
			outdata[(*outlen)++] = g_DI.Cfg.DeviceID ;
			outdata[(*outlen)++] = 0x5A;
			outdata[(*outlen)++] = 0x00;
			outdata[(*outlen)++] = 0x00;

			(*outlen) += CMD_GetParmaData(INFO_MACADDR,&outdata[(*outlen)]); // mac��ַ
			(*outlen) += CMD_GetParmaData(INFO_DHCPEN,&outdata[(*outlen)]); // DHCP
			(*outlen) += CMD_GetParmaData(INFO_IP_ADDR,&outdata[(*outlen)]); // �豸IP��ַ
			(*outlen) += CMD_GetParmaData(INFO_MASK_ADDR,&outdata[(*outlen)]); // ��������
			(*outlen) += CMD_GetParmaData(INFO_GATEWAY_ADDR,&outdata[(*outlen)]); // Ĭ������
			(*outlen) += CMD_GetParmaData(INFO_SERIAL_NO,&outdata[(*outlen)]); //���к�
			(*outlen) += CMD_GetParmaData(INFO_IP_LOCAL1,&outdata[(*outlen)]); //���ع���1-IP
			(*outlen) += CMD_GetParmaData(INFO_IP_LOCAL2,&outdata[(*outlen)]); //���ع���2-IP
			(*outlen) += CMD_GetParmaData(INFO_IP_SERVER1,&outdata[(*outlen)]); //Զ��IP1 
			(*outlen) += CMD_GetParmaData(INFO_Net_Port1,&outdata[(*outlen)]); //Զ��IP1�˿�
			(*outlen) += CMD_GetParmaData(INFO_IP_SERVER2,&outdata[(*outlen)]); //Զ��IP2 
			(*outlen) += CMD_GetParmaData(INFO_Net_Port2,&outdata[(*outlen)]); //Զ��IP2�˿�
			(*outlen) += CMD_GetParmaData(INFO_IP_SERVER3,&outdata[(*outlen)]); //Զ��IP3 
			(*outlen) += CMD_GetParmaData(INFO_Net_Port3,&outdata[(*outlen)]); //Զ��IP3�˿�
			(*outlen) += CMD_GetParmaData(INFO_DEVICE_ID,&outdata[(*outlen)]); //�豸ID			
			(*outlen) += CMD_GetParmaData(INFO_MCSCPU_SerialNO,&outdata[(*outlen)]); //����CPU���к�						
			
			outdata[2] = ((*outlen)-4)%256;
			outdata[3] = ((*outlen)-4)/256;
			crctmp = CRC16_GX_MODBUS( (u8 *)outdata,(*outlen));
			outdata[(*outlen)++] = crctmp %256;
			outdata[(*outlen)++] = crctmp /256;
		
		}
	}
}
/*
 *******************************************************************************
 *
 * Private routines
 *
 *******************************************************************************/
/******************************************************************************
 * �������ƣ�Data_Analysis()
 * �������ܣ����ݽ���
 * ��ڲ�����*Rx_Data:ԭ������,*pDat:�������������,Len:���ݳ���
 * ���ڲ���������������ݳ���
 *******************************************************************************/
u8 Char_unTransfer(u8 *Rx_Data, u8 *pDat, u8 Len, u8 Head )
{
	u8 i, j;
	u8 *pBuf;
	
	for (i=0; i<Len; i++)
	{
		if (*(Rx_Data+i) == Head)	// ͷ���ж�	  Char_unTransfer
		{
			pBuf = Rx_Data+i;
			*pDat = *pBuf++;
			for (j=i+1; j<Len; j++)
			{
				
				if ((*pBuf == 0x5E) && (Head ==HEAD_CODE))	// �ַ�ת��:���Ų�ת��
				{
					if (*(pBuf+1) == 0x5D)		// ��0X5E��0X5D������0X5E
					{
						*(pDat+j) = 0x5E;
						pBuf += 2;
					}
					else if (*(pBuf+1) == 0x39)	// ��0X5E��0X39������07E
					{
						*(pDat+j) = Head ;	//TAIL_CODE
						pBuf += 2;
					}
				}
				else
				{
					*(pDat+j) = *pBuf++;
					
					if (*(pDat+j) == Head) 		// β���ж�TAIL_CODE
					{
						return (j-i+1);					// �������ݳ���
					}
				}               
			}
		}
	}
	
	return 0;
}

/********************************************************************************************************
** �������ƣ�Char_Transfer()
** �������ܣ�����ת�塣
** ��ڲ�����*Buf:ת�������ݻ�����;*Dat:ԭ����; Len:ԭ���ݳ���
** ���ڲ�����ת�������ݳ���
********************************************************************************************************/
u8 Char_Transfer(u8 *Buf, u8 *Dat, u8 Len, u8 Head)
{
	u8 i;
	u8 BufLen=0;
	
	*Buf++ = *Dat;
	BufLen++;
	for (i=1; i<Len-1; i++)
	{
		if ((*(Dat+i) == 0x5E) && (Head ==HEAD_CODE))		// ��0X5E��0X5D������0X5E :���Ų�ת��
		{
			*Buf++ = 0x5E;
			*Buf++ = 0x5D;
			BufLen += 2;
		}
		else if ((*(Dat+i) == Head ) && (Head ==HEAD_CODE))	// ��0X5E��0X7D������0X7E��	 HEAD_CODE
		{
			*Buf++ = 0x5E;
			*Buf++ = 0x39;
			BufLen += 2;
		}
		else
		{
			*Buf++ = *(Dat+i);
			BufLen++;
		}
	}
	*Buf = *(Dat+i);
	BufLen++;
	
	return BufLen;
}

/******************************************************************************
 * �������ƣ�Pack_PT()
 * �������ܣ����ݰ�͸��
 * ��ڲ�����*Buf:͸�����ݰ�,Len:͸�����ݰ�����,timeout:��ʱʱ��
 * ���ڲ�����ret���ص����ݳ���
 *******************************************************************************/
u8 PaRS232Pack_PT(u8 *Buf, u8 Len, u16 timeout,u8 *RxBuf)
{
	u8 err;
	u8 Lenth;
	u16 crc_value;
	u8 crcH,crcL;
	//��ʱ�Դ�ʩ��ǿ�л�ȡ�ź�������ֹ�����޷��ϴ���
	fPaRTU = TRUE;
	OSSemPend(PARS232SEM, 0, &err);		// 0:��Զ�ȴ�ֱ����Դ��ɿ���

	UART5Write(Buf, Len);

	if (Pa_Wait_RxEnd(timeout) == FALSE)			// ��ʱ����
	{
	
		UART5_RBuf.Len = 0;
		UART5_RBuf.fEnd = FALSE;
		OSSemPost(PARS232SEM);
		fPaRTU = FALSE;
		return FALSE;
	}

	crcL = UART5_RBuf.Buff[UART5_RBuf.Len-2];
	crcH = UART5_RBuf.Buff[UART5_RBuf.Len-1];
	
 	crc_value = CRC16_GX_MODBUS((u8 *)UART5_RBuf.Buff,UART5_RBuf.Len-2);	// ����CRC

	if ((crcL != (u8)(crc_value&0xFF)) || (crcH != (u8)((crc_value>>8)&0xFF)))  // CRCУ��
	{
	
		return FALSE;		// ����CRC��
	}	
	
	DataCopy((u8 *)&RxBuf[0], UART5_RBuf.Buff, UART5_RBuf.Len-2);
	Lenth = UART5_RBuf.Len-2;
	
	UART5_RBuf.Len = 0;
	UART5_RBuf.fEnd = FALSE;
		
	OSSemPost(PARS232SEM);
	fPaRTU = FALSE;
	return Lenth;
}   
/********************************************************************************************************
** �������ƣ�SoftReset()
** �������ܣ������λ��
** ��ڲ�����
** ���ڲ�����
********************************************************************************************************/
void SoftReset(void)
{
	__set_FAULTMASK(1); // �ر������ж�
	NVIC_SystemReset(); // ��λ
}

/*******************************************************************************
 * �������ƣ�RS485_Tx()
 * �������ܣ�����RS485���ݡ�
 * ��ڲ�����*tDev_Data:Ҫ���͵����ݽṹ��,Addr:��ַ ,timeout:��ʱʱ��,����
 * ���ڲ��������������嶨���ö����eDev_Err
 *******************************************************************************/ 
eDev_Err RS485_Tx(u8 *txBuf, u8 Buflen, u16 timeout)
{
	u16 CRC_Value;
	u8 fERR;

	
	CRC_Value = CRC16_GX_MODBUS((u8 *)txBuf,Buflen);			// ����CRC

	txBuf[Buflen++] = (u8)CRC_Value;
	txBuf[Buflen++] = (u8)(CRC_Value>>8);	
	
	PARS232_SendBuf(txBuf, Buflen);	   
/*	
	if (Pa_Wait_RxEnd(timeout) == FALSE)			// ��ʱ����
	{
		UART2_RBuf.Len = 0;
		UART2_RBuf.fEnd = FALSE;
		printf("...pa.....eDev_TimeOut_ERR..........\r\n");
		return eDev_TimeOut_ERR;
	}
*/		
	UART2_RBuf.Len = 0;
	UART2_RBuf.fEnd = FALSE;
	
	return eDev_NO_ERR;
}
/*******************************************************************************
 * �������ƣ�GXData_Process()
 * �������ܣ��������ݰ�
 * ��ڲ�����*DataBuf:���յ���������;Len:ԭ���ݳ���
 * ���ڲ��������������嶨���ö����eDev_Err
 *******************************************************************************/
eDev_Err GXData_Process(u8 *DataBuf, u8 Len)
{
 	u16 crc_value;
	u8 crcH,crcL;
	u8 temp;
	u8 i = 0;
	crcL = DataBuf[Len-2];
	crcH = DataBuf[Len-1];

//	if(Sys_Flag.Bit.fPanetQue == TRUE)
//	{
//	 	Sys_Flag.Bit.fPanetQue = FALSE;
//		temp = DataBuf[3];
//		DataBuf[3] = 0x55;
//		crc_value = CRC16_GX_MODBUS((u8 *)DataBuf,Len-2);	// ����CRC
//		DataBuf[3] = temp;
//	}
//	else
//	{
	 	crc_value = CRC16_GX_MODBUS((u8 *)DataBuf,Len-2);	// ����CRC
//	}
	
	if ((crcL != (u8)(crc_value&0xFF)) || (crcH != (u8)((crc_value>>8)&0xFF)))  // CRCУ��
	{
		
		return eDev_CRC_ERR;		// ����CRC��
	}	    

  	if(DataBuf[1] == 0x32)
	{
		Sys_Flag.Bit.fPaCheck = TRUE;
//			Sys_Flag.Bit.fPanetQue = TRUE;
		g_DI.Cfg.PA_IPHead = DataBuf[0];
		DataCopy((u8 *)&cpaitt[0], &DataBuf[65], 16);

		return eDev_NO_ERR;
	}
	
	if((DataBuf[0] == g_DI.Cfg.PA_IPHead)&&(DataBuf[1] == 0x03))
	{
	
		DataCopy((u8 *)&GX_RXData[0], DataBuf, Len-2);
		GX_RXDatalen = Len - 2;
		
		DataCopy((u8 *)&g_DI.Pa[0].Data.ID, DataBuf, Len);
		/*
		g_DI.Pa[0].Data.WorkState =	DataBuf[4];	// ����ģ��״̬
		g_DI.Pa[0].Data.DetState = DataBuf[5];	// ���ģ��״̬
		g_DI.Pa[0].Data.ProState = DataBuf[6];	// ����ģ��״̬
		g_DI.Pa[0].Data.Power_FR = Hex_To_Decimal(DataBuf,8);	//���书��						
		g_DI.Pa[0].Data.Power_RV = Hex_To_Decimal(DataBuf,12);	//���书��
		g_DI.Pa[0].Data.CPU_TPT = Hex_To_Decimal(DataBuf,16);	//CPU�¶�	
		g_DI.Pa[0].Data.PA_TPT = Hex_To_Decimal(DataBuf,20);	//�����¶�	
		g_DI.Pa[0].Data.ENV_TPT = Hex_To_Decimal(DataBuf,24);	//�����¶�
		g_DI.Pa[0].Data.Power_IN = Hex_To_Decimal(DataBuf,28);	//���빦��
		g_DI.Pa[0].Data.PA_VOL = Hex_To_Decimal(DataBuf,32);	//���ŵ�ѹ
		g_DI.Pa[0].Data.PA_BECUR = Hex_To_Decimal(DataBuf,36);	//ǰ������
		g_DI.Pa[0].Data.PA_CUR1 = Hex_To_Decimal(DataBuf,40);	//���ŵ���1
		g_DI.Pa[0].Data.PA_CUR2 = Hex_To_Decimal(DataBuf,44);	//���ŵ���2
		g_DI.Pa[0].Data.Power_SET = Hex_To_Decimal(DataBuf,48);	//��������				
		g_DI.Pa[0].Data.ATT_POT = DataBuf[51];	//��һ��˥����λ��
		g_DI.Pa[0].Data.ATT_POT1 = DataBuf[52];	//�ڶ���˥����λ��				   
		*/
		 

		/*
		printf("................................start..................................\r\n");
		printf("	g_DI.Pa[0].Data.ID = 0x%x \r\n",g_DI.Pa[0].Data.ID);
		printf("	g_DI.Pa[0].Data.FunID = 0x%x \r\n",g_DI.Pa[0].Data.FunID);
		printf("	g_DI.Pa[0].Data.Fun0 = 0x%x \r\n",g_DI.Pa[0].Data.Fun0);
		printf("	g_DI.Pa[0].Data.Fun1 = 0x%x \r\n",g_DI.Pa[0].Data.Fun1);
		printf("	g_DI.Pa[0].Data.WorkState = 0x%x \r\n",g_DI.Pa[0].Data.WorkState);
		printf("	g_DI.Pa[0].Data.DetState = 0x%x \r\n",g_DI.Pa[0].Data.DetState);
		printf("	g_DI.Pa[0].Data.ProState = 0x%x \r\n",g_DI.Pa[0].Data.ProState);
		printf("	g_DI.Pa[0].Data.ResState = 0x%x \r\n",g_DI.Pa[0].Data.ResState);

		printf("	g_DI.Pa[0].Data.Power_FR = %f \r\n",g_DI.Pa[0].Data.Power_FR);
		printf("	g_DI.Pa[0].Data.Power_RV = %f \r\n",g_DI.Pa[0].Data.Power_RV);
		printf("	g_DI.Pa[0].Data.CPU_TPT = %f \r\n",g_DI.Pa[0].Data.CPU_TPT);
		printf("	g_DI.Pa[0].Data.PA_TPT = %f \r\n",g_DI.Pa[0].Data.PA_TPT);
		printf("	g_DI.Pa[0].Data.ENV_TPT = %f \r\n",g_DI.Pa[0].Data.ENV_TPT);
		printf("	g_DI.Pa[0].Data.Power_IN = %f \r\n",g_DI.Pa[0].Data.Power_IN);
		printf("	g_DI.Pa[0].Data.PA_VOL = %f \r\n",g_DI.Pa[0].Data.PA_VOL); 

		printf("	g_DI.Pa[0].Data.PA_BECUR = %f \r\n",g_DI.Pa[0].Data.PA_BECUR);
		printf("	g_DI.Pa[0].Data.PA_CUR1 = %f \r\n",g_DI.Pa[0].Data.PA_CUR1);
		printf("	g_DI.Pa[0].Data.PA_CUR2 = %f \r\n",g_DI.Pa[0].Data.PA_CUR2);
		printf("	g_DI.Pa[0].Data.Power_SET = %f \r\n",g_DI.Pa[0].Data.Power_SET);

		printf("	g_DI.Pa[0].Data.ATT_POT = 0x%x \r\n",g_DI.Pa[0].Data.ATT_POT);
		printf("	g_DI.Pa[0].Data.ATT_POT1 = 0x%x \r\n",g_DI.Pa[0].Data.ATT_POT1);
		printf("	g_DI.Pa[0].Data.CRC16_Val = 0x%x \r\n",g_DI.Pa[0].Data.CRC16_Val);
	 

		printf("...............................end...................................\r\n");  
	   	 */
		
	}
	else if((DataBuf[0] == g_DI.Cfg.PA_IPHead)&&(DataBuf[1] == 0x35)) //���в�����ѯ�������ݴ���
	{
		 
		if((DataBuf[2] == 0xB6)&&(DataBuf[3] == 0xC1))
		{
		 	g_DI.Cfg.CloseFan_TPTlimit = DataBuf[8];        //�رշ����¶�
			g_DI.Cfg.OpenFan1_TPTlimit = DataBuf[9];        //��������1�¶�
			g_DI.Cfg.OpenFan2_TPTlimit = DataBuf[10];        //��������2�¶�
			g_DI.Cfg.PWRRED_TPTlimit = DataBuf[11];          //�������¶�
			g_DI.Cfg.PWROFF_TPTlimit = DataBuf[12];          //���¹ػ������¶� 
			g_DI.Cfg.PWRRED_CURlimit = Hex_To_Decimal(DataBuf,13);          //�����ʵ���
			g_DI.Cfg.PWROFF_CURlimit = Hex_To_Decimal(DataBuf,17);          //����������
			g_DI.Cfg.CurRstDelay = (DataBuf[21] | (DataBuf[22] << 8));; //���������ж���ʱ
			g_DI.Cfg.CurRstNum = DataBuf[23]; //������������
			g_DI.Cfg.PWR_RateOut = (u8)(DataBuf[24]);         	  //��������
			g_DI.Cfg.PWR_INlimit = Hex_To_Decimal(DataBuf,25);         	  //������С����
			g_DI.Cfg.PWR_RVlimit = DataBuf[29];         	  //���䱣������
				
/*
		   	printf("-------------------------start-------------------------\r\n");
			printf("	g_DI.Cfg.CloseFan_TPTlimit = %d \r\n",g_DI.Cfg.CloseFan_TPTlimit);
			printf("	g_DI.Cfg.OpenFan1_TPTlimit = %d \r\n",g_DI.Cfg.OpenFan1_TPTlimit);
			printf("	g_DI.Cfg.OpenFan2_TPTlimit = %d \r\n",g_DI.Cfg.OpenFan2_TPTlimit);
			printf("	g_DI.Cfg.PWRRED_TPTlimit = %d \r\n",g_DI.Cfg.PWRRED_TPTlimit);
			printf("	g_DI.Cfg.PWROFF_TPTlimit = %d \r\n",g_DI.Cfg.PWROFF_TPTlimit);
			printf("	g_DI.Cfg.PWRRED_CURlimit = %f \r\n",g_DI.Cfg.PWRRED_CURlimit);
			printf("	g_DI.Cfg.PWROFF_CURlimit = %f \r\n",g_DI.Cfg.PWROFF_CURlimit);
			printf("	g_DI.Cfg.CurRstDelay = %d \r\n",g_DI.Cfg.CurRstDelay);	
			printf("	g_DI.Cfg.CurRstNum = %d \r\n",g_DI.Cfg.CurRstNum);
			printf("	g_DI.Cfg.PWR_RateOut = %d \r\n",g_DI.Cfg.PWR_RateOut);
			printf("	g_DI.Cfg.PWR_INlimit = %f \r\n",g_DI.Cfg.PWR_INlimit);
			printf("	g_DI.Cfg.PWR_RVlimit = %d \r\n",g_DI.Cfg.PWR_RVlimit);	
			printf("-------------------------end------------------------\r\n");
			//OSTimeDlyHMSM(0, 0, 0, 100);
*/			
		}
		
	}
		
	return eDev_NO_ERR;
}
/*******************************************************************************
 * �������ƣ�PA_RS232_GXTx()
 * �������ܣ�����RS485���ݡ�
 * ��ڲ�����*tDev_Data:Ҫ���͵�����,length:���� ,timeout:��ʱʱ��,����
 * ���ڲ��������������嶨���ö����eDev_Err
 *******************************************************************************/ 
eDev_Err PA_RS232_GXTx(u8 *tDev_Data,u8 length,u16 timeout)
{
	u16 CRC_Value;
	u8 fERR;
	u8 a;
	if (length == 0)
	{
		return eDev_RxNone_ERR;		// �޽��յ�����
	}
	CRC_Value = CRC16_GX_MODBUS((u8 *)tDev_Data,length);			// ����CRC
	
	tDev_Data[length++] = (u8)CRC_Value;
	tDev_Data[length++] = (u8)(CRC_Value>>8);

	PARS232_SendBuf(tDev_Data, length);

	if (Pa_Wait_RxEnd(timeout) == FALSE)			// ��ʱ����
	{
		UART5_RBuf.Len = 0;
		UART5_RBuf.fEnd = FALSE;
		
		return eDev_TimeOut_ERR;
	}
	
	fERR = GXData_Process(UART5_RBuf.Buff, UART5_RBuf.Len);	// ���ݴ���
			   
	UART5_RBuf.Len = 0;
	UART5_RBuf.fEnd = FALSE;

	return fERR;
}
/*******************************************************************************
 * �������ƣ�Data_Process()
 * �������ܣ��������ж������ݴ滺������������ȷ�����ݰ�
 * ��ڲ�����*tDat:��ȡ�����ݻ�����,*DataBuf:���յ���������;Len:ԭ���ݳ���
 * ���ڲ��������������嶨���ö����eDev_Err
 *******************************************************************************/	   
eDev_Err Excit_RS232_Send(u8 *tDev_Data,u8 length,u16 timeout)
{
 	u16 CRC_Value;
	u8 fERR,i;

	if (length == 0)
	{
		return eDev_RxNone_ERR;		// �޽��յ�����
	}
	CRC_Value = CRC16_GX_MODBUS((u8 *)tDev_Data,length);			// ����CRC
	
	tDev_Data[length++] = (u8)CRC_Value;
	tDev_Data[length++] = (u8)(CRC_Value>>8);

	UART3Write(tDev_Data, length);	   
	
	if (EXCIT_Wait_RxEnd(timeout) == FALSE)			// ��ʱ����
	{
		UART3_RBuf.Len = 0;
		UART3_RBuf.fEnd = FALSE;		
		return eDev_TimeOut_ERR;
	}

	fERR = ExcitData_Process(UART3_RBuf.Buff, UART3_RBuf.Len);	// ���ݴ���
			   
	UART3_RBuf.Len = 0;
	UART3_RBuf.fEnd = FALSE;

	return fERR;
}
/******************************************************************************
 * �������ƣ�EXCIT_Wait_RxEnd()
 * �������ܣ��ȴ����������ݽ������
 * ��ڲ�����timeout:�ȴ��ĳ�ʱ���� ��λ ����
 * ���ڲ������ϲ�������ݳ���
 *******************************************************************************/
bool EXCIT_Wait_RxEnd(u16 timeout)
{
	while (timeout--)				// ��ʱ����
	{
		if((UART3_RBuf.Len>=9) && UART3_RBuf.fEnd)	// �ȴ����ݽ������
		{
			return TRUE;
		}
		OSTimeDlyHMSM(0, 0, 0, 30);
	}

	return FALSE;				    
}
/*******************************************************************************
 * �������ƣ�GXData_Process()
 * �������ܣ��������ݰ�
 * ��ڲ�����*DataBuf:���յ���������;Len:ԭ���ݳ���
 * ���ڲ��������������嶨���ö����eDev_Err
 *******************************************************************************/
eDev_Err ExcitData_Process(u8 *DataBuf, u8 Len)
{
 	u16 crc_value;
	u8 crcH,crcL;
	u8 temp;
	static u8 ExcitErrNum = 0;
	crcL = DataBuf[Len-2];
	crcH = DataBuf[Len-1];

	crc_value = CRC16_GX_MODBUS((u8 *)DataBuf,Len-2);	// ����CRC
	
	
	if ((crcL != (u8)(crc_value&0xFF)) || (crcH != (u8)((crc_value>>8)&0xFF)))  // CRCУ��
	{
		
	
		return eDev_CRC_ERR;		// ����CRC��
	}	    
	if(Sys_Flag.Bit.fExcitCheck == FALSE)
	{
	  	if(DataBuf[1] == 0x32)
		{
			Sys_Flag.Bit.fExcitCheck = TRUE;
			g_DI.Cfg.Excit_IPHead = DataBuf[0];
//			DataCopy((u8 *)&cExcitDirection[0], &DataBuf[65], 16);
			
			return eDev_NO_ERR;
		}
		return eDev_Dat_ERR;		// �������ݴ�
	}
	if(DataBuf[0] != g_DI.Cfg.Excit_IPHead)
	{
		ExcitErrNum++; 
		if(ExcitErrNum >=3)
		{
		 	ExcitErrNum = 0;
			Sys_Flag.Bit.fExcitCheck = FALSE;
		}
		return eDev_Dat_ERR;			
	}
	if((DataBuf[0] == g_DI.Cfg.Excit_IPHead)&&(DataBuf[1] == 0x03))
	{
	

//		DataCopy((u8 *)&g_DI.Excitdata.ExcID, (u8 *)&DataBuf[0], Len);
		g_DI.Excitdata.FMFRQ = (DataBuf[5]|(DataBuf[6]<<8));
		g_DI.Excitdata.FMMODULATIONMODE = DataBuf[9];
		g_DI.Excitdata.FMPre_Emphasis = DataBuf[10];
		g_DI.Excitdata.FMGain_RF = DataBuf[11];
		g_DI.Excitdata.FMMODULATIONVAL = DataBuf[12];
		g_DI.Excitdata.FMRDS_CHIN = DataBuf[13];
		g_DI.Excitdata.FMRDSVAL = DataBuf[14]; 
		g_DI.Excitdata.FMCALFRQ_ENABLE = DataBuf[15];
		g_DI.Excitdata.FMOSCFRQ = (DataBuf[16]|(DataBuf[17]<<8)|(DataBuf[18]<<16)|(DataBuf[19]<<24));
		g_DI.Excitdata.FMCALFRQ_VAL = (DataBuf[20]|(DataBuf[21]<<8)|(DataBuf[22]<<16)|(DataBuf[23]<<24));
		g_DI.Excitdata.FMSIGNAL_VAL = DataBuf[24];
		g_DI.Excitdata.FMFRQDCT = (DataBuf[33]|(DataBuf[34]<<8)|(DataBuf[35]<<16)|(DataBuf[36]<<24));
		g_DI.Excitdata.FMOSCFRQDCT = (DataBuf[37]|(DataBuf[38]<<8)|(DataBuf[39]<<16)|(DataBuf[40]<<24));
		g_DI.Excitdata.FMMainChannelMean = (DataBuf[41]|(DataBuf[42]<<8)|(DataBuf[43]<<16)|(DataBuf[44]<<24));

		g_DI.Excitdata.FMCALFRQ_SIGN = DataBuf[45];
		g_DI.Excitdata.ModulationDCT = (DataBuf[46]|(DataBuf[47]<<8));
		g_DI.Excitdata.DifModulation = (DataBuf[49]|(DataBuf[50]<<8));
		g_DI.Excitdata.LeftChnModuVal = (DataBuf[51]|(DataBuf[52]<<8));
		g_DI.Excitdata.RightChnModuVal = (DataBuf[53]|(DataBuf[54]<<8));
//		printf(".....................Excitdata.............................................\r\n");
//		printf("	g_DI.Excitdata.FMFRQ = %d MHz \r\n",g_DI.Excitdata.FMFRQ);	 
//		printf("	g_DI.Excitdata.FMMODULATIONMODE = %d  \r\n",g_DI.Excitdata.FMMODULATIONMODE);	 
//		printf("	g_DI.Excitdata.FMPre_Emphasis = %d  \r\n",g_DI.Excitdata.FMPre_Emphasis);
//		printf("	g_DI.Excitdata.FMGain_RF = %d  \r\n",g_DI.Excitdata.FMGain_RF);
//		printf("	g_DI.Excitdata.FMMODULATIONVAL = %d  \r\n",g_DI.Excitdata.FMMODULATIONVAL);
//		printf("	g_DI.Excitdata.FMRDS_CHIN = %d  \r\n",g_DI.Excitdata.FMRDS_CHIN);
//		printf("	g_DI.Excitdata.FMRDSVAL = %d  \r\n",g_DI.Excitdata.FMRDSVAL);
//		printf("	g_DI.Excitdata.FMCALFRQ_ENABLE = %d  \r\n",g_DI.Excitdata.FMCALFRQ_ENABLE);
//		printf("	g_DI.Excitdata.FMOSCFRQ = %d  \r\n",g_DI.Excitdata.FMOSCFRQ);
//		printf("	g_DI.Excitdata.FMCALFRQ_SIGN = %d  \r\n",g_DI.Excitdata.FMCALFRQ_SIGN);
//		printf("	g_DI.Excitdata.FMCALFRQ_VAL = %d  \r\n",g_DI.Excitdata.FMCALFRQ_VAL);
//		printf("	g_DI.Excitdata.ModulationDCT = %d  \r\n",g_DI.Excitdata.ModulationDCT);
//		printf("	g_DI.Excitdata.FMFRQDCT = %d  \r\n",g_DI.Excitdata.FMFRQDCT);
//		printf("	g_DI.Excitdata.FMOSCFRQDCT = %d  \r\n",g_DI.Excitdata.FMOSCFRQDCT);
//		printf("	g_DI.Excitdata.FMMainChannelMean = %d  \r\n",g_DI.Excitdata.FMMainChannelMean);
//		printf("..................................................................\r\n");
//		

		
	}
	else if(DataBuf[0] == g_DI.Cfg.Excit_IPHead)
	{
		if((DataBuf[1] == 0x35)&&(DataBuf[6] == 0xC9)&&(DataBuf[7] == 0xE8))
		{
			switch(DataBuf[8])
			{
				case 0:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setFMFRQ ok!\r\n");
					}
					break;
				case 1:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setMod_Mode ok!\r\n");
					}
					break;
				case 2:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setPre_Emphasis ok!\r\n");
					}
					break;
				case 3:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setAudioLO ok!\r\n");
					}
					break;
				case 4:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setAudioRO ok!\r\n");
					}
					break;
				case 7:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setRDSdsta ok!\r\n");
					}
					break;
				case 9:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setPWR_Gain ok!\r\n");
					}
					break;
				case 0x0A:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setRDSVAL ok!\r\n");
					}
					break;
				case 0x0B:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setRDSCHN ok!\r\n");
					}
					break;
				case 0x0C:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setMODUXT ok!\r\n");
					}
					break;
				case 0x11:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setMODUCT ok!\r\n");
					}
					break;
				case 0x1E:
					if(DataBuf[9] == 0x01)
					{
					 	//printf("Excit setMODUCT ok!\r\n");
					}
					break;
				default:
					//printf("Excit set : 0x%x \r\n",DataBuf[8]);
					break;
			}
		}		
	}		
	return eDev_NO_ERR;
}





