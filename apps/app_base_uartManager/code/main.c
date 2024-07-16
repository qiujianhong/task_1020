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

extern int uart_app_init(void **ppPublicData);
extern int uart_version_print(char *buf, uint32 buflen);
extern int uart_mqttmsg_init(void *pPublic, void **ppPrivate);
extern int uart_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate);
extern int uart_taskmsg_init(void *pPublic, void **ppPrivate);
extern int uart_taskmsg_proc(void *pArg, void *pPublic, void *pPrivate);
#ifdef PRODUCT_SCU
extern int uart_rs485_1_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_2_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_3_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_4_init(void *pPublic, void **ppPrivate);
#else
extern int uart_rs485_1_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_2_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_3_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_4_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_5_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_6_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_7_init(void *pPublic, void **ppPrivate);
extern int uart_rs485_8_init(void *pPublic, void **ppPrivate);
extern int uart_virtual_proc(void *pArg, void *pPublic, void *pPrivate);

#endif
extern int uart_work_schedule(void *pArg, void *pPublic, void *pPrivate);

/* ��ʼ������ */
versionfunc g_pVersionPrint = uart_version_print;
initfunc    g_pProcessInitFunc = uart_app_init;
char       *g_APPLOGPATH = "/data/app/uartManager/log";

/* �������� */
TAKS_TEMPLATE_T g_tBaseTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"mqttIn",   0,      0,      30,       uart_mqttmsg_init, uart_mqttmsg_proc,  NULL},
    {"mqttOut",  0,      0,      60,       uart_taskmsg_init, uart_taskmsg_proc,  NULL},
};
int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

#ifdef PRODUCT_SCU
/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"comm_1",  0,      0,      60,       uart_rs485_1_init, uart_work_schedule,  NULL},
    {"comm_2",  0,      0,      60,       uart_rs485_2_init, uart_work_schedule,  NULL},
    {"comm_3",  0,      0,      60,       uart_rs485_3_init, uart_work_schedule,  NULL},
    {"comm_4",  0,      0,      60,       uart_rs485_4_init, uart_work_schedule,  NULL},
};
#else
/* һ������ */
TAKS_TEMPLATE_T g_tCommonTask[] = 
{
  /* ����,       ���ȼ�, ջ��С, ��ʱʱ��, ��ʼ������,      ���Ⱥ���,         �ͷź��� */
    {"comm_1",  0,      0,      60,       uart_rs485_1_init, uart_work_schedule,  NULL},
    {"comm_2",  0,      0,      60,       uart_rs485_2_init, uart_work_schedule,  NULL},
    {"comm_3",  0,      0,      60,       uart_rs485_3_init, uart_work_schedule,  NULL},
    
#ifdef PRODUCT_ZCU_1
    {"comm_4",  0,      0,      60,       uart_rs485_4_init, uart_work_schedule,  NULL},
#endif

#if defined PRODUCT_ECU || defined PRODUCT_ECU_G
    {"comm_5",  0,      0,      60,       uart_rs485_5_init, uart_virtual_proc,  NULL},
    {"comm_6",  0,      0,      60,       uart_rs485_6_init, uart_virtual_proc,  NULL},
    {"comm_7",  0,      0,      60,       uart_rs485_7_init, uart_virtual_proc,  NULL},
    {"comm_8",  0,      0,      60,       uart_rs485_8_init, uart_virtual_proc,  NULL},

#endif
};
#endif

int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);

/* ������ģ�� */
#include "template.h"

