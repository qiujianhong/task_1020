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

extern int sec_app_init(void **ppPublicData);
extern int sec_version_print(char *buf, uint32 buflen);

extern int sec_mqttmsg_init(void *pPublic, void **ppPrivate);
extern int sec_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate);
extern int sec_taskmsg_init(void *pPublic, void **ppPrivate);
extern int sec_taskmsg_proc(void *pArg, void *pPublic, void *pPrivate);

extern int sec_manager_init(void *pPublic, void **ppPrivate);
extern int sec_manager_proc(void *pArg, void *pPublic, void *pPrivate);

extern int sec_packetcap_init(void *pPublic, void **ppPrivate);
extern int sec_packetcap_proc(void *pArg, void *pPublic, void *pPrivate);

extern int sec_procmonitor_init(void *pPublic, void **ppPrivate);
extern int sec_procmonitor_proc(void *pArg, void *pPublic, void *pPrivate);

/* ��ʼ������ */
versionfunc g_pVersionPrint = sec_version_print;
initfunc    g_pProcessInitFunc = sec_app_init;
char       *g_APPLOGPATH = "/data/app/scsMonitor/logFile";

/* �������� */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"mqttIn",   0,      0,      60,       sec_mqttmsg_init, sec_mqttmsg_proc,  NULL},
    {"mqttOut",  0,      0,      60,       sec_taskmsg_init, sec_taskmsg_proc,  NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"mgr",  0,      0,      60,       sec_manager_init,     sec_manager_proc,     NULL},
    {"pcap", 0,      0,       0,       sec_packetcap_init,   sec_packetcap_proc,   NULL},
//    {"proc", 0,      0,      3600,       sec_procmonitor_init, sec_procmonitor_proc, NULL},
};

int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);

/* ������ģ�� */
#include "template.h"

