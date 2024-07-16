/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��̨APP
* @date��    2021-7-8
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"


#include "task_template.h"
#include "cm_fun.h"

/* ��ʼ������ */
versionfunc g_pVersionPrint = cm_version_print;
initfunc    g_pProcessInitFunc = cm_app_init;
char       *g_APPLOGPATH = "/data/app/cpuMonitor/log";

/* �������� */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
    {"cpuMonitor",      0,      0,      0,       cm_thread_init,          cm_thread_proc,          NULL},

};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,            ���Ⱥ���,              �ͷź��� */

};
int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);


/* ������ģ�� */
#include "template.h"


