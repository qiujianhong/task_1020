/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/


#ifndef __CCO_MAIN_H__
#define __CCO_MAIN_H__

#include "pw_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************************************************************************
* 全局变量声明
*******************************************************************************/
extern THREAD_ARG_T g_sUpRecvDealThreadArg; 		//上行主站控制任务线程参数
extern THREAD_ARG_T g_sUpSendDealThreadArg; 		//上行主站上报任务线程参数
extern THREAD_ARG_T g_sDownDeviceThreadArg; 		//下行设备通信线程参数
extern THREAD_ARG_T g_sEventCheckThreadArg;		    //交采模块事件检测线程参数
extern THREAD_ARG_T g_sMonitStatThreadArg;          //监测统计线程参数

extern uint8 g_nUpRecvDealThreadFlag;               //主站控制任务处理线程标志
extern uint8 g_nUpSendDealThreadFlag;               //上报主站任务处理线程标志
extern uint8 g_nDownDeviceThreadFlag;               //下行设备线程标志
extern uint8 g_nEventCheckThreadFlag;               //交采模块事件检测线程标志
extern uint8 g_nMonitStatThreadFlag;                //监测统计线程标志

/*******************************************************************************
* 全局函数声明
*******************************************************************************/
extern void *event_check_thread(void *arg);
extern void *up_recv_deal_thread(void *arg);
extern void *down_device_thread(void *arg);
extern void *monit_stat_thread(void *arg);
extern int pw_main_init();




#ifdef __cplusplus
}
#endif

#endif // __CCO_MAIN_H__
