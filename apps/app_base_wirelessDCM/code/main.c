/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号主线程功能
* @date：    2021-3-22
* @history： 
*********************************************************************
*/
#include "baseCommon.h"

#include "task_template.h"


extern int wireless_app_init(void **ppPublicData);
extern int wireless_version_print(char *buf, uint32 buflen);
extern int wireless_mqttproc_init(void *pPublic, void **ppPrivate);
extern int wireless_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate);
extern int wireless_manager_init(void *pPublic, void **ppPrivate);
extern int wireless_manager_proc(void *pArg, void *pPublic, void *pPrivate);
extern int wireless_gui_init(void *pPublic, void **ppPrivate);
extern int wireless_gui_start(void *pArg, void *pPublic, void *pPrivate);
extern int wireless_ppp0_init(void *pPublic, void **ppPrivate);
extern int wireless_ppp1_init(void *pPublic, void **ppPrivate);
extern int wireless_ppp_proc(void *pArg, void *pPublic, void *pPrivate);
extern int wireless_at0_init(void *pPublic, void **ppPrivate);
extern int wireless_at1_init(void *pPublic, void **ppPrivate);
extern int wireless_at_proc(void *pArg, void *pPublic, void *pPrivate);
extern int wireless_ccu_init(void *pPublic, void **ppPrivate);
extern int wireless_ccu_proc(void *pArg, void *pPublic, void *pPrivate);


/* 初始化函数 */
versionfunc g_pVersionPrint = wireless_version_print;
initfunc    g_pProcessInitFunc = wireless_app_init;
char       *g_APPLOGPATH = "/data/app/wirelessDCM/log";

/* 基础任务 */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,             调度函数,              释放函数 */
    {"MQTT",     0,      0,      60,       wireless_mqttproc_init, wireless_mqttmsg_proc, NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* 一般任务 */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,            调度函数,            释放函数 */
    {"MANAGER",  0,      0,      0,        wireless_manager_init, wireless_manager_proc, NULL},
    {"CCU",      0,      0,      0,        wireless_ccu_init,     wireless_ccu_proc,     NULL},
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    {"GUI",      0,      0,      0,        wireless_gui_init,     wireless_gui_start,    NULL},
//#endif
    {"AT1",      0,      0,      0,        wireless_at0_init,     wireless_at_proc,      NULL},
    {"PPP1",     0,      0,      0,        wireless_ppp0_init,    wireless_ppp_proc,     NULL},
    {"AT2",      0,      0,      0,        wireless_at1_init,     wireless_at_proc,      NULL},
    {"PPP2",     0,      0,      0,        wireless_ppp1_init,    wireless_ppp_proc,     NULL},
};
int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);


/* 最后调用模板 */
#include "template.h"

