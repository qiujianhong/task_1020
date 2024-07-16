/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ������̹߳���
* @date��    2020-6-30
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"

#include "task_template.h"
#include "iot_init.h"

extern int iot_app_init(void **ppPublicData);
extern int iot_version_print(char *buf, uint32 buflen);

extern int iot_mqtt_init(void *pPublic, void **ppPrivate);
extern int iot_mqtt_proc(void *pArg, void *pPublic, void *pPrivate);

extern int iot_jsoninit(void *pPublic, void **ppPrivate);
extern int iot_jsonproc(void *pArg, void *pPublic, void *pPrivate);


extern int iot_msginit(void *pPublic, void **ppPrivate);
extern int iot_msgproc(void *pArg,void *pPublic, void *pPrivate);

extern int iot_updateinit(void *pPublic, void **ppPrivate);
extern int iot_updateproc(void *pArg,void *pPublic, void *pPrivate);


/* ��ʼ������ */
versionfunc g_pVersionPrint = iot_version_print;
initfunc    g_pProcessInitFunc = iot_app_init;
char       *g_APPLOGPATH = "/data/app/iotManager/log";

/* �������� */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"wuguan_mqtt",   0,      0,      30,     iot_mqtt_init, iot_mqtt_proc,  NULL},

};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"wuguan_json",     0,      0,      30,     iot_jsoninit, iot_jsonproc,  NULL},
    {"wuguan_msg",      0,      0,      30,     iot_msginit,  iot_msgproc,  NULL},
    {"wuguan_update",   0,      0,      30,     iot_updateinit,  iot_updateproc,  NULL},
};

int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);

/* ������ģ�� */
#include "template.h"

