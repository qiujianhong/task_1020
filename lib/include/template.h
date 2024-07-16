/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   进程/线程模板
* @date：    2020-6-29
* @history： 
* @User deatil:
1. 先定义进程初始化操作函数和日志路径(用于异常日志记录)
   versionfunc g_pVersionPrint = ...;
   initfunc g_pProcessInitFunc = ...;
   char    *g_APPLOGPATH = ...;

2. 基础任务按照定义顺序优先启动, 所有基础任务设置工作标记后执行后续启动(一般任务)
   基础任务需要定义结构体及其数量
   TAKS_TEMPLATE_T g_tBaseTask[] = {...};
   int BASE_TASK_NUM = sizeof(g_tBaseTask)/sizeof(TAKS_TEMPLATE_T);

3. 后启动一般任务, 需要定义结构体及其数量
   TAKS_TEMPLATE_T g_tCommonTask[] = {...};
   int COMM_TASK_NUM = sizeof(g_tCommonTask)/sizeof(TAKS_TEMPLATE_T);

4. 最后本头文件
#include "template.h"
*********************************************************************
*/

#include "task_base.h"
extern int TASK_ERROR_LOG(char *fmt, ...);

/**********************************************************************
* @name      : main
* @brief     ：进程入口函数
* @param[in] ：int argc
               char** argv
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-29
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
        printf("APP没有设置异常日志路径\n");
        goto end;
    }

    if(0 == BASE_TASK_NUM + COMM_TASK_NUM)
    {
        printf("APP设置没有任务\n");
        goto end;
    }

    if(argc >= 2)
    {
        /* 查询版本号 */
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

    /* 进程初始化, 返回全局变量pPublocData */
    ret = process_init(g_APPLOGPATH, g_pProcessInitFunc, &pPublocData);
    if(0 != ret)
    {
        printf("APP初始化失败\n");
        goto end;
    }

    TASK_ERROR_LOG("\n---------------------------\n程序初始化结束, 启动工作线程!\n---------------------------\n");
    {
        char bufprint[128] = {0};
        g_pVersionPrint(bufprint, 127);
        TASK_ERROR_LOG("%s\n", bufprint);
    }
    pthread_attr_init(&attr); /*初始化线程属性*/
    /* 基础task启动 */
    for(i = 0; i < BASE_TASK_NUM; i++)
    {
        pHandle = (TASK_HANDLE_T *)malloc(sizeof(TASK_HANDLE_T));
        if(NULL == pHandle)
        {
            TASK_ERROR_LOG("任务%s申请内存失败\n", g_tBaseTask[i].name);
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
            printf("启动任务%s失败\n", g_tBaseTask[i].name);
            goto end;
        }
    }

    /* 等待基础task状态 */
    while(bwait)
    {
        for(i = 0; i < BASE_TASK_NUM; i++)
        {
            pHandle = (TASK_HANDLE_T *)g_tBaseTask[i].handle;
            if(TASK_STATUS_ERROR == pHandle->status)
            {
                TASK_ERROR_LOG("任务%s执行失败\n", g_tBaseTask[i].name);
                goto end;
            }
            if(1 == pHandle->baseOk)
            {
                bwait = 0;
            }
        }
        sleep(1);
    }

    /* 一般task启动 */
    for(i = 0; i < COMM_TASK_NUM; i++)
    {
        pHandle = (TASK_HANDLE_T *)malloc(sizeof(TASK_HANDLE_T));
        if(NULL == pHandle)
        {
            TASK_ERROR_LOG("任务%s申请内存失败\n", g_tCommonTask[i].name);
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
            TASK_ERROR_LOG("启动任务%s失败\n", g_tCommonTask[i].name);
            goto end;
        }
    }
    pthread_attr_destroy(&attr);

    /* 启动监控任务 */
    tMonitor.basenum = BASE_TASK_NUM;
    tMonitor.pBase = g_tBaseTask;
    tMonitor.commnum = COMM_TASK_NUM;
    tMonitor.pComm = g_tCommonTask;
    ret = pthread_create(&threadid, NULL, task_monitor_thread, &tMonitor);
    if(0 != ret)
    {
        TASK_ERROR_LOG("启动任务监控失败\n");
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

