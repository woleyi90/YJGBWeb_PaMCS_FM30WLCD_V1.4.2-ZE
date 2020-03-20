/*******************************************************************************
 *
 * Filename  : app.cfg.h
 *
 * Function  : Reference head file for "CONST" library.
 *
 * Created   : 2010-01-29 08:30:00
 *           : Mr. Liangbao Fan
 * Modified  : 2010-01-29
 *           : Mr. Liangbao Fan
 *
 * Check In  : $Date: 2013-05-11 11:26:49 +0800 (周六, 11 五月 2013) $
 *           : $Author: flb $
 * Revision  : $Revision: 8 $
 *
 * Copyright(c) 2010 by Fujian Newland Communication Science Technologies Co.,Ltd.
 *                       All Rights Reserved.
 * 
 *******************************************************************************/

#ifndef  __APP_CFG_H
#define  __APP_CFG_H



#define  APP_TASK_START_PRIO                    15 
#define  Task_PC_PRIO                           12
#define  Task_RTU_PRIO                          11
#define  Task_ETH_PRIO						    					10
#define  Task_OLED_PRIO                         13 
#define  Task_PAON_PRIO                         14 
							  
#define  APP_TASK_START_STK_SIZE                500
#define  Task_PC_STK_SIZE                       300
#define  Task_RTU_STK_SIZE                      200
#define  Task_ETH_STK_SIZE                      1500
#define  Task_OLED_STK_SIZE                     200
#define  Task_PAON_STK_SIZE                     200



#endif
