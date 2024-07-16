/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP
* @date：    2021-7-8
* @history： 
*********************************************************************
*/
#include "baseCommon.h"

#include "task_template.h"


extern int ble_version_print(char *buf, uint32 buflen);
extern int ble_app_init(void **ppPublicData);
extern int ble_mqttproc_init(void *pPublic, void **ppPrivate);
extern int ble_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate);
extern int ble_dev_init(void *pPublic, void **ppPrivate);
extern int ble_dev_proc(void *pArg, void *pPublic, void *pPrivate);

/* 初始化函数 */
versionfunc g_pVersionPrint = ble_version_print;
initfunc    g_pProcessInitFunc = ble_app_init;
char       *g_APPLOGPATH = "/data/app/btManager/logFile";

/* 基础任务 */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,             调度函数,              释放函数 */
    {"MQTT",     0,      0,      60,       ble_mqttproc_init,      ble_mqttmsg_proc,      NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* 一般任务 */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,            调度函数,              释放函数 */
    {"BLE",      0,      0,      120,       ble_dev_init,          ble_dev_proc,          NULL},

};
int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);


/* 最后调用模板 */
#include "template.h"

