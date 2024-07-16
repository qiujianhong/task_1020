/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� ͷ�ļ�
* @date�� 2019-11-20
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
/* @brief:app osversion�ṹ�嶨��  */
typedef struct 
{
  sint8 appname[20];
  sint8 osversion[40];
} APPOSVERSION_T;

/* @brief:app cpuʹ���ʽṹ�嶨��  */
typedef struct 
{
  sint8 appname[20];
  sint8 cpurate;
} APPCPURATE_T;

/* @brief:app �ڴ�ʹ���ʽṹ�嶨��  */
typedef struct 
{
  sint8 appname[20];
  sint8 ramrate;
} APPRAMRATE_T;

/* @brief:app storageʹ���ʽṹ�嶨��  */
typedef struct 
{
  sint8 appname[20];
  sint8 storagerate;
} APPSTORAGERATE_T;

/* @brief:app osversion��cpu/�ڴ�/Ӳ��ʹ���ʽṹ�嶨��  */
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



/* @brief:����ڴ�ʹ����  */
void sys_checkmemrate_cb(void);

/* @brief:���CPUʹ����  */
void sys_checkcpurate_cb(void);

/* @brief:���洢ʹ����  */
void sys_checkstorgerate_cb(void);

/* @brief:���flash����  */
void sys_checkflash_cb(void);

/* @brief:���ʱ�Ӵ���  */
void sys_checkclock_cb(void);

/* @brief:���ram����  */
void sys_checkram_cb(void);

/* @brief:���esam����  */
void sys_checkesam_cb(void);


/* @brief:��ʼ��  */
sint32 sys_control_init(void);

/* @brief:����  */
sint32 sys_control_start(void);

/* @brief:��ͣ */
void sys_control_stop(void);

#if 0
/*==========APP����========================*/
/* @brief: APP��OS�汾�鱨ȡ�� */
sint32 sys_app_osversion_get(APPOSVERSION_T *, sint32 *);

/* @brief:  APP��CPUʹ����ȡ�� */
sint32 sys_app_cpurate_get(APPCPURATE_T *, sint32 *);

/* @brief:  APP��RAMʹ����ȡ�� */
sint32 sys_app_ramrate_get(APPRAMRATE_T *, sint32 *);

/* @brief:  APP��storageʹ����ȡ�� */
sint32 sys_app_storagerate_get(APPSTORAGERATE_T *, sint32 *);

/* @brief:  APP��os�汾 CPU/RAM/STORAGE��ʹ����ȡ�� */
sint32 sys_app_info_get(APPINFO_T *, sint32 *);
#endif
/* @brief:  ��Ϣ���������ӿ� */
sint32 sys_msg_requestreboot(void);

/* @brief:  ��Ϣ����ӿڣ��¼����� */
sint32 sys_msg_eventdistribution(MSG_OBJ_T *pmsgobj);

/* @brief:  ע��callback���� */
sint32 sys_register_callback(SYS_CALLBACK_TYPE cb_type, syscallback funtioncb);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_INFO_CONTROL_H__ */