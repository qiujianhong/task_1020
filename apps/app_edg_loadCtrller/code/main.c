/*
*********************************************************************
* Copyright(C) 2024 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ר�为�ɿ���
* @date��    2024-6-27
* @history�� 
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


/* ��ʼ������ */
versionfunc g_pVersionPrint = loadctrl_version_print;
initfunc    g_pProcessInitFunc = loadctrl_app_init;
char       *g_APPLOGPATH = "/data/app/loadCtrller/log";

/* �������� */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"MQTT",   0,      0,      30,       loadctrl_mqttmsg_init, loadctrl_mqttmsg_proc,  NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"STAT",   0,      0,      60,       loadctrl_stat_init,    loadctrl_stat_proc,     NULL},
    {"EVENT",  0,      0,      60,       loadctrl_event_init,   loadctrl_event_proc,    NULL},
    {"CTRL",   0,      0,      0,        loadctrl_ctrl_init,    loadctrl_ctrl_proc,     NULL},
    {"gui",    0,      0,      0,        loadctrl_gui_init,     loadctrl_gui_start,     NULL},
};
int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);


/* ������ģ�� */
#include "template.h"


