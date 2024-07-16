/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ң������APP������
* @date��    2022-3-20
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"

#include "task_template.h"

extern int rsp_version_print(char *buf, uint32 buflen);
extern int rsp_app_init(void **ppPublicData);
extern int rsp_mqttproc_init(void *pPublic, void **ppPrivate);
extern int rsp_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate);
extern int rsp_dev_init(void *pPublic, void **ppPrivate);
extern int rsp_dev_proc(void *pArg, void *pPublic, void *pPrivate);
extern int rsp_uart_init(void *pPublic, void **ppPrivate);
extern int rsp_uart_proc(void *pArg, void *pPublic, void *pPrivate);
extern int rsp_map_init(void *pPublic, void **ppPrivate);
extern int rsp_map_proc(void *pArg, void *pPublic, void *pPrivate);
extern int rsp_model1_init(void *pPublic, void **ppPrivate);
extern int rsp_model2_init(void *pPublic, void **ppPrivate);
extern int rsp_model_proc(void *pArg, void *pPublic, void *pPrivate);
extern int rsp_report_init(void *pPublic, void **ppPrivate);
extern int rsp_report_proc(void *pArg, void *pPublic, void *pPrivate);

/* ��ʼ������ */
versionfunc g_pVersionPrint = rsp_version_print;
initfunc    g_pProcessInitFunc = rsp_app_init;
char       *g_APPLOGPATH = "/data/app/rspSample/log";

/* �������� */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,                         ���Ⱥ���,                 �ͷź��� */
    {"MQTT",   0,      0,      60,       rsp_mqttproc_init,   rsp_mqttmsg_proc,     NULL},
    {"DEV",  -20,      0,      60,       rsp_dev_init,        rsp_dev_proc,         NULL},
#ifdef BOARD_3358
    {"UART",   0,      0,      60,       rsp_uart_init,       rsp_uart_proc,        NULL},
#endif
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,                         ���Ⱥ���,                  �ͷź��� */
#ifdef PRODUCT_ECU
    {"MAP",    0,      0,      60,       rsp_map_init,        rsp_map_proc,          NULL},
    {"MODULE1",0,      0,      0,        rsp_model1_init,     rsp_model_proc,        NULL},
#endif
  //{"MODULE2",0,      0,      0,        rsp_model2_init,     rsp_model_proc,        NULL},
    {"REPORT", 0,      0,      60,       rsp_report_init,     rsp_report_proc,       NULL},
};
int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);


/* ������ģ�� */
#include "template.h"



