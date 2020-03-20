/*******************************************************************************
 *
 * Filename  : Task_api.h
 *
 * Function  : Reference head file for "TASK_API" library.
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


#ifndef  __TASK_API_H
#define  __TASK_API_H


#define MOC_TYPE     0	  // ¼à¿Ø¿Ú
#define POLL_TYPE    1   // ÂÖÑ¯´®¿Ú


			   
void TaskPC(void *pdata);

void TaskModem(void *pdata);
void TaskRTU(void *pdata);
void TaskOLED(void *pdata);
bool Pa_Wait_RxEnd(u16 timeout);
u16 PC_Protocol(u8 *dat, u16 len ,u8 type);
bool Dev_Opr(void);
void PADevCheckPoll(void);
void TaskPAON(void *pdata);
void USART_AutoSend(char *outdata,u16 *outlen);


#endif
