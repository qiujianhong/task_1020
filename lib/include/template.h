/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����/�߳�ģ��
* @date��    2020-6-29
* @history�� 
* @User deatil:
1. �ȶ�����̳�ʼ��������������־·��(�����쳣��־��¼)
   versionfunc g_pVersionPrint = ...;
   initfunc g_pProcessInitFunc = ...;
   char    *g_APPLOGPATH = ...;

2. ���������ն���˳����������, ���л����������ù�����Ǻ�ִ�к�������(һ������)
   ����������Ҫ����ṹ�弰������
   TAKS_TEMPLATE_T g_tBaseTask[] = {...};
   int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

3. ������һ������, ��Ҫ����ṹ�弰������
   TAKS_TEMPLATE_T g_tCommonTask[] = {...};
   int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);

4. ���ͷ�ļ�
#include "template.h"
*********************************************************************
*/

#include "task_base.h"
extern int TASK_ERROR_LOG(char *fmt, ...);

/**********************************************************************
* @name      : main
* @brief     ��������ں���
* @param[in] ��int argc
               char** argv
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-29
* @Update    :
**********************************************************************/
int main(int argc, char** argv)
{
    void *pPublocData = NULL;
    int   ret = 0;
    int   i = 0;
    TASK_HANDLE_T *pHandle = NULL;
    pthread_attr_t attr;
    int   stacksize = 0;
    uint8 bwait  = 1;
    TASK_MONITOR_T tMonitor;
    pthread_t threadid;

    if(NULL == g_APPLOGPATH)
    {
        printf("APPû�������쳣��־·��\n");
        goto end;
    }

    if(0 == BASE_TASK_NUM + COMM_TASK_NUM)
    {
        printf("APP����û������\n");
        goto end;
    }

    if(argc >= 2)
    {
        /* ��ѯ�汾�� */
        if(0 == memcmp(argv[1], "version", strlen("version")) && g_pVersionPrint)
        {
            char bufprint[128] = {0};
            g_pVersionPrint(bufprint, 127);
            printf("%s\n", bufprint);
            return 0;
        }

        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("wait 20s\n");
            sleep(20);
        }
    }

    /* ���̳�ʼ��, ����ȫ�ֱ���pPublocData */
    ret = process_init(g_APPLOGPATH, g_pProcessInitFunc, &pPublocData);
    if(0 != ret)
    {
        printf("APP��ʼ��ʧ��\n");
        goto end;
    }

    TASK_ERROR_LOG("\n---------------------------\n�����ʼ������, ���������߳�!\n---------------------------\n");
    {
        char bufprint[128] = {0};
        g_pVersionPrint(bufprint, 127);
        TASK_ERROR_LOG("%s\n", bufprint);
    }
    pthread_attr_init(&attr); /*��ʼ���߳�����*/
    /* ����task���� */
    for(i = 0; i < BASE_TASK_NUM; i++)
    {
        pHandle = (TASK_HANDLE_T *)malloc(sizeof(TASK_HANDLE_T));
        if(NULL == pHandle)
        {
            TASK_ERROR_LOG("����%s�����ڴ�ʧ��\n", g_tBaseTask[i].name);
            goto end;
        }
        memset(pHandle, 0, sizeof(TASK_HANDLE_T));
        pHandle->pPArg = pPublocData;
        pHandle->status = TASK_STATUS_INIT;
        g_tBaseTask[i].handle = pHandle;

        stacksize = g_tBaseTask[i].stacksize;
        if(stacksize > 0)
        {
            pthread_attr_setstacksize(&attr, stacksize * 1024);
            ret = pthread_create(&pHandle->threadid, &attr, task_entry_thread, &(g_tBaseTask[i]));
        }
        else
        {
            ret = pthread_create(&pHandle->threadid, NULL, task_entry_thread, &(g_tBaseTask[i]));
        }
        if(0 != ret)
        {
            printf("��������%sʧ��\n", g_tBaseTask[i].name);
            goto end;
        }
    }

    /* �ȴ�����task״̬ */
    while(bwait)
    {
        for(i = 0; i < BASE_TASK_NUM; i++)
        {
            pHandle = (TASK_HANDLE_T *)g_tBaseTask[i].handle;
            if(TASK_STATUS_ERROR == pHandle->status)
            {
                TASK_ERROR_LOG("����%sִ��ʧ��\n", g_tBaseTask[i].name);
                goto end;
            }
            if(1 == pHandle->baseOk)
            {
                bwait = 0;
            }
        }
        sleep(1);
    }

    /* һ��task���� */
    for(i = 0; i < COMM_TASK_NUM; i++)
    {
        pHandle = (TASK_HANDLE_T *)malloc(sizeof(TASK_HANDLE_T));
        if(NULL == pHandle)
        {
            TASK_ERROR_LOG("����%s�����ڴ�ʧ��\n", g_tCommonTask[i].name);
            goto end;
        }
        memset(pHandle, 0, sizeof(TASK_HANDLE_T));
        pHandle->pPArg = pPublocData;
        pHandle->status = TASK_STATUS_INIT;
        g_tCommonTask[i].handle = pHandle;

        stacksize = g_tCommonTask[i].stacksize;
        if(stacksize > 0)
        {
            pthread_attr_setstacksize(&attr, stacksize * 1024);
            ret = pthread_create(&pHandle->threadid, &attr, task_entry_thread, &(g_tCommonTask[i]));
        }
        else
        {
            ret = pthread_create(&pHandle->threadid, NULL, task_entry_thread, &(g_tCommonTask[i]));
        }
        if(0 != ret)
        {
            TASK_ERROR_LOG("��������%sʧ��\n", g_tCommonTask[i].name);
            goto end;
        }
    }
    pthread_attr_destroy(&attr);

    /* ����������� */
    tMonitor.basenum = BASE_TASK_NUM;
    tMonitor.pBase = g_tBaseTask;
    tMonitor.commnum = COMM_TASK_NUM;
    tMonitor.pComm = g_tCommonTask;
    ret = pthread_create(&threadid, NULL, task_monitor_thread, &tMonitor);
    if(0 != ret)
    {
        TASK_ERROR_LOG("����������ʧ��\n");
        goto end;
    }

    while(1)
    {
        sleep(2);
    }
end:
    exit(0);
    return 0;
}

