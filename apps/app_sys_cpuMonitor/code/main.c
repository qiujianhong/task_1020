/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP
* @date：    2021-7-8
* @history： 
*********************************************************************
*/
#include "baseCommon.h"


#include "task_template.h"
#include "cm_fun.h"

/* 初始化函数 */
versionfunc g_pVersionPrint = cm_version_print;
initfunc    g_pProcessInitFunc = cm_app_init;
char       *g_APPLOGPATH = "/data/app/cpuMonitor/log";

/* 基础任务 */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
    {"cpuMonitor",      0,      0,      0,       cm_thread_init,          cm_thread_proc,          NULL},

};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* 一般任务 */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* 名称,       优先级, 栈大小, 超时时间, 初始化函数,            调度函数,              释放函数 */

};
int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);


/* 最后调用模板 */
#include "template.h"


