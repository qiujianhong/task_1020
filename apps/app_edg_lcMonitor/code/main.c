/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·������̹߳���
* @date��    2020-6-30
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"

#include "task_template.h"

extern int lc_app_init(void **ppPublicData);
extern int lc_version_print(char *buf, uint32 buflen);
extern int lc_mqttmsg_init(void *pPublic, void **ppPrivate);
extern int lc_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate);
extern int lc_taskmsg_init(void *pPublic, void **ppPrivate);
extern int lc_taskmsg_proc(void *pArg, void *pPublic, void *pPrivate);
extern int lc_monitor_init(void *pPublic, void **ppPrivate);
extern int lc_monitor_proc(void *pArg, void *pPublic, void *pPrivate);
extern int lc_gui_init(void *pPublic, void **ppPrivate);
extern int lc_gui_start(void *pArg, void *pPublic, void *pPrivate);


/* ��ʼ������ */
versionfunc g_pVersionPrint = lc_version_print;
initfunc    g_pProcessInitFunc = lc_app_init;
#ifdef PRODUCT_SCU
char       *g_APPLOGPATH = "/data/app/lcMonitor/logFile";
char       *g_APPCFGFILE = "/data/app/lcMonitor/configFile/lcMonitor.json";
#else
char       *g_APPLOGPATH = "/data/app/lcMonitor/log";
char       *g_APPCFGFILE = "/data/app/lcMonitor/data/lcMonitor.json";
#endif

/* �������� */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"mqttIn",   0,      0,      30,       lc_mqttmsg_init, lc_mqttmsg_proc,  NULL},
    {"mqttOut",  0,      0,      60,       lc_taskmsg_init, lc_taskmsg_proc,  NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"monitor",  0,      0,      60,       lc_monitor_init, lc_monitor_proc,  NULL},
#if PRODUCT_ECU
    {"lcgui",    0,      0,      0,        lc_gui_init,     lc_gui_start,     NULL},
#endif
};
int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);


/* ������ģ�� */
#include "template.h"

