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

extern int usb_app_init(void **ppPublicData);
extern int usb_version_print(char *buf, uint32 buflen);

extern int usb_map_init(void *pPublic, void **ppPrivate);
extern int usb_map_proc(void *pArg, void *pPublic, void *pPrivate);

extern int usb_upg_init(void *pPublic, void **ppPrivate);
extern int usb_upg_proc(void *pArg, void *pPublic, void *pPrivate);

/* 初始化函数 */
versionfunc g_pVersionPrint = usb_version_print;
initfunc    g_pProcessInitFunc = usb_app_init;
char       *g_APPLOGPATH = "/data/app/usbManager/logFile";

/* 基础任务 */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,      调度函数,         释放函数 */
    {"map",   0,      0,      30,       usb_map_init, usb_map_proc,  NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* 一般任务 */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,      调度函数,         释放函数 */
    //{"upg",  0,      0,      60,       usb_upg_init, usb_upg_proc,  NULL},
};

int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);

/* 最后调用模板 */
#include "template.h"

