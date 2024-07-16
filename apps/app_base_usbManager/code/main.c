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

extern int usb_app_init(void **ppPublicData);
extern int usb_version_print(char *buf, uint32 buflen);

extern int usb_map_init(void *pPublic, void **ppPrivate);
extern int usb_map_proc(void *pArg, void *pPublic, void *pPrivate);

extern int usb_upg_init(void *pPublic, void **ppPrivate);
extern int usb_upg_proc(void *pArg, void *pPublic, void *pPrivate);

/* ��ʼ������ */
versionfunc g_pVersionPrint = usb_version_print;
initfunc    g_pProcessInitFunc = usb_app_init;
char       *g_APPLOGPATH = "/data/app/usbManager/logFile";

/* �������� */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"map",   0,      0,      30,       usb_map_init, usb_map_proc,  NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    //{"upg",  0,      0,      60,       usb_upg_init, usb_upg_proc,  NULL},
};

int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);

/* ������ģ�� */
#include "template.h"

