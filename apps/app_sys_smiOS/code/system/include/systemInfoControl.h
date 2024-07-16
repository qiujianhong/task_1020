/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 头文件
* @date： 2019-11-20
*********************************************************************
*/
#ifndef __SYSTEM_INFO_CONTROL_H__
#define __SYSTEM_INFO_CONTROL_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "msgHelper.h"
#include "systemInfo.h"
#include "systemInfodef.h"
#include "systemLog.h"
#include "systemWatchDog.h"

#if 0
/* @brief:app osversion结构体定义  */
typedef struct 
{
  sint8 appname[20];
  sint8 osversion[40];
} APPOSVERSION_T;

/* @brief:app cpu使用率结构体定义  */
typedef struct 
{
  sint8 appname[20];
  sint8 cpurate;
} APPCPURATE_T;

/* @brief:app 内存使用率结构体定义  */
typedef struct 
{
  sint8 appname[20];
  sint8 ramrate;
} APPRAMRATE_T;

/* @brief:app storage使用率结构体定义  */
typedef struct 
{
  sint8 appname[20];
  sint8 storagerate;
} APPSTORAGERATE_T;

/* @brief:app osversion，cpu/内存/硬盘使用率结构体定义  */
typedef struct 
{
  sint8 appname[20];
  sint8 osversion[40];
  sint8 cpurate;
  sint8 ramrate;
  sint8 strogagerate;
} APPINFO_T;
#endif

typedef void (*syscallback)(void *);
typedef enum _sys_callback_type {
  CB_EVENTDT,
  CB_KILLAPP,
} SYS_CALLBACK_TYPE;



/* @brief:检测内存使用率  */
void sys_checkmemrate_cb(void);

/* @brief:检测CPU使用率  */
void sys_checkcpurate_cb(void);

/* @brief:检测存储使用率  */
void sys_checkstorgerate_cb(void);

/* @brief:检测flash坏块  */
void sys_checkflash_cb(void);

/* @brief:检测时钟错误  */
void sys_checkclock_cb(void);

/* @brief:检测ram错误  */
void sys_checkram_cb(void);

/* @brief:检测esam错误  */
void sys_checkesam_cb(void);


/* @brief:初始化  */
sint32 sys_control_init(void);

/* @brief:启动  */
sint32 sys_control_start(void);

/* @brief:暂停 */
void sys_control_stop(void);

#if 0
/*==========APP关联========================*/
/* @brief: APP的OS版本情报取得 */
sint32 sys_app_osversion_get(APPOSVERSION_T *, sint32 *);

/* @brief:  APP的CPU使用率取得 */
sint32 sys_app_cpurate_get(APPCPURATE_T *, sint32 *);

/* @brief:  APP的RAM使用率取得 */
sint32 sys_app_ramrate_get(APPRAMRATE_T *, sint32 *);

/* @brief:  APP的storage使用率取得 */
sint32 sys_app_storagerate_get(APPSTORAGERATE_T *, sint32 *);

/* @brief:  APP的os版本 CPU/RAM/STORAGE的使用率取得 */
sint32 sys_app_info_get(APPINFO_T *, sint32 *);
#endif
/* @brief:  消息处理重启接口 */
sint32 sys_msg_requestreboot(void);

/* @brief:  消息处理接口：事件处理 */
sint32 sys_msg_eventdistribution(MSG_OBJ_T *pmsgobj);

/* @brief:  注册callback函数 */
sint32 sys_register_callback(SYS_CALLBACK_TYPE cb_type, syscallback funtioncb);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_INFO_CONTROL_H__ */