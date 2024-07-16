/*
*********************************************************************
* Copyright(C) 2024 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   专变负荷控制
* @date：    2024-6-27
* @history： 
*********************************************************************
*/
#include "baseCommon.h"

#include "task_template.h"

extern int loadctrl_app_init(void **ppPublicData);
extern int loadctrl_version_print(char *buf, uint32 buflen);
extern int loadctrl_mqttmsg_init(void *pPublic, void **ppPrivate);
extern int loadctrl_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate);
extern int loadctrl_stat_init(void *pPublic, void **ppPrivate);
extern int loadctrl_stat_proc(void *pArg, void *pPublic, void *pPrivate);
extern int loadctrl_event_init(void *pPublic, void **ppPrivate);
extern int loadctrl_event_proc(void *pArg, void *pPublic, void *pPrivate);
extern int loadctrl_ctrl_init(void *pPublic, void **ppPrivate);
extern int loadctrl_ctrl_proc(void *pArg, void *pPublic, void *pPrivate);
extern int loadctrl_gui_init(void *pPublic, void **ppPrivate);
extern int loadctrl_gui_start(void *pArg, void *pPublic, void *pPrivate);


/* 初始化函数 */
versionfunc g_pVersionPrint = loadctrl_version_print;
initfunc    g_pProcessInitFunc = loadctrl_app_init;
char       *g_APPLOGPATH = "/data/app/loadCtrller/log";

/* 基础任务 */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,      调度函数,         释放函数 */
    {"MQTT",   0,      0,      30,       loadctrl_mqttmsg_init, loadctrl_mqttmsg_proc,  NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* 一般任务 */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,      调度函数,         释放函数 */
    {"STAT",   0,      0,      60,       loadctrl_stat_init,    loadctrl_stat_proc,     NULL},
    {"EVENT",  0,      0,      60,       loadctrl_event_init,   loadctrl_event_proc,    NULL},
    {"CTRL",   0,      0,      0,        loadctrl_ctrl_init,    loadctrl_ctrl_proc,     NULL},
    {"gui",    0,      0,      0,        loadctrl_gui_init,     loadctrl_gui_start,     NULL},
};
int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);


/* 最后调用模板 */
#include "template.h"


