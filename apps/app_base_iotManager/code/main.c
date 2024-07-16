/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监测主线程功能
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"

#include "task_template.h"

extern int iot_app_init(void **ppPublicData);
extern int iot_version_print(char *buf, uint32 buflen);

extern int iot_mqttmsg_init(void *pPublic, void **ppPrivate);
extern int iot_mqtt_proc(void *pArg, void *pPublic, void *pPrivate);

extern int iot_data_upDate_init(void *pPublic, void **ppPrivate);
extern int iot_data_upDate_proc(void *pArg,void *pPublic, void *pPrivate);

extern int iot_data_inspect_init(void *pPublic, void **ppPrivate);
extern int iot_data_inspect_proc(void *pArg,void *pPublic, void *pPrivate);


/* 初始化函数 */
versionfunc g_pVersionPrint = iot_version_print;
initfunc    g_pProcessInitFunc = iot_app_init;
char       *g_APPLOGPATH = "/data/app/iotManager/log";

/* 基础任务 */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,      调度函数,         释放函数 */
    {"iot_mqtt",   0,      0,      30,     iot_mqttmsg_init, iot_mqtt_proc,  NULL},
    //{"iot_updata",   0,      0,      60,   iot_data_upDate_init, iot_data_upDate_proc,  NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* 一般任务 */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,      调度函数,         释放函数 */
    {"iot_updata",   0,      0,      0,   iot_data_upDate_init, iot_data_upDate_proc,    NULL},
    {"iot_inspect",  0,      0,      0,   iot_data_inspect_init, iot_data_inspect_proc,  NULL},
};

int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);

/* 最后调用模板 */
#include "template.h"

