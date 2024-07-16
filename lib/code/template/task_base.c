/*
*********************************************************************
* @filename: task_base.c
* @brief   : 任务操作
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2020-6-29     create
*********************************************************************
*/
#include "baseCommon.h"
#include <sys/syscall.h>
#include <sys/prctl.h>

#include "trace_back.h"
#include "task_base.h"
#include "task_template.h"

#include "task_proc.h"
#include "task_log.h"

#define TASK_DELAY(dd) \
    if(dd >= 1000000)\
    {\
        int ss = dd % 1000000;\
        sleep(dd / 1000000);\
        if(ss > 0)\
        {\
            usleep(ss);\
        }\
    }\
    else if(dd > 0)\
    {\
        usleep(dd);\
    }

extern void diag_sig_handler(int signo);

/**********************************************************************
* @name      : process_init
* @brief     ：进程初始化函数
* @param[in] ：char *logpath 日志路径
               initfunc func 注册的初始化函数
* @param[out]：void **ppData 进程共有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-29
* @Update    : 
**********************************************************************/
int process_init(char *logpath, void *pfunc, void **ppData)
{
    char   filepath[256] = {0};
    initfunc func = (initfunc)pfunc;

    if(NULL == logpath)
    {
        return -1;
    }

    if(0 != access(logpath, F_OK))
    {
        if(0 != mkdir(logpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            printf("Create data filepath %s, failed\n", logpath);
        }
    }

    sprintf(filepath, "%s/%s", logpath, "TaskInfo.log");

    /* 信号处理 包含日志打印 */
    init_sig_action(filepath);

    if(pfunc)
    {
        return func(ppData);
    }
    *ppData = NULL;
    return 0;
}

/**********************************************************************
* @name      : task_entry_thread
* @brief     ：任务调度线程
* @param[in] ：void *pArg   线程入参
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    : 
**********************************************************************/
void *task_entry_thread(void *pArg)
{
    TAKS_TEMPLATE_T *ptTask  = (TAKS_TEMPLATE_T *)pArg;
    TASK_HANDLE_T   *ptHandle = (TASK_HANDLE_T *)ptTask->handle;
    void            *pPrivate = NULL;
    int              ret = 0;
    struct timespec  curtime = {0};

    TASK_ERROR_LOG("任务%s启动\n", ptTask->name);
    prctl(PR_SET_NAME, ptTask->name);
    ptHandle->tid = syscall(SYS_gettid);

    /* 优先级 */
    if(ptTask->priority)
    {
        nice(ptTask->priority);
    }

    /* 初始化 */
    if(NULL != ptTask->initfunc)
    {
        ret = ptTask->initfunc(ptHandle->pPArg, &pPrivate);
        if(0 != ret)
        {
            goto errend;
        }
        ptHandle->pTArg = pPrivate;
    }

    if(NULL == ptTask->procfunc)
    {
        goto errend;
    }
    TASK_ERROR_LOG("任务%s开始工作\n", ptTask->name);

    /* 调度操作 */
    for(;;)
    {
        if(ptTask->timeout > 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            ptHandle->starttime = curtime.tv_sec;;
        }

        /* 任务调度 */
        ptHandle->status = TASK_STATUS_WORK;
        ret = ptTask->procfunc(ptHandle, ptHandle->pPArg, pPrivate);
        if(TASK_STATUS_ERROR == ptHandle->status)
        {
            goto errend; //调度中设置了错误 需要退出线程
        }
        ptHandle->status = TASK_STATUS_STOP;

        /* 调度延时 */
        TASK_DELAY(ret);
    }

errend:
    if(pPrivate && ptTask->uninitfunc)
    {
        ptTask->uninitfunc(pPrivate);
        ptHandle->pTArg = NULL;
    }
    /* 线程出错报错 */
    TASK_SET_STATUS(ptHandle, TASK_STATUS_ERROR);
    return 0;
}

/**********************************************************************
* @name      : task_process_timeout
* @brief     ：线程卡死
* @param[in] ：TAKS_TEMPLATE_T *pTask    任务配置
               TASK_HANDLE_T *ptHandle   任务线程句柄
               int timeout               超时时间
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-1
* @Update    : 
**********************************************************************/
void task_process_timeout(TAKS_TEMPLATE_T *pTask, TASK_HANDLE_T *ptHandle, int timeout)
{
    char buf[2048] = {0};
    int  offset = 0;

    offset = sprintf(buf, "\n-------------------TimeOut------------------------\n"
                          "Task   : %s(TID %d)\n", pTask->name, (int)ptHandle->tid);
    offset += sprintf(buf + offset, "Time   : %ds(MAX %ds)\n", timeout, pTask->timeout);
    offset += sprintf(buf + offset, "UserLog: %s\n", ptHandle->userlog);
    offset += sprintf(buf + offset, "--------------------------------------------------\n");
    TASK_ERROR_LOG("%s", buf);
    
    /* 想办法获取调用栈 */
    pthread_kill(ptHandle->threadid, SIGDEADLOOP);
}

/**********************************************************************
* @name      : task_monitor_thread
* @brief     ：任务监控线程
* @param[in] ：void *pArg   线程入参
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    : 
**********************************************************************/
void *task_monitor_thread(void *pArg)
{
    int i = 0;
    TASK_MONITOR_T  *pMonitor = (TASK_MONITOR_T *)pArg;
    TAKS_TEMPLATE_T *pTask = NULL;
    TASK_HANDLE_T   *ptHandle = NULL;
    struct timespec  curtime = {0};
    int              timeout = 0;

    prctl(PR_SET_NAME, "Task_Monitor");
    while(1)
    {
        clock_gettime(CLOCK_MONOTONIC, &curtime);

        /* 基础任务 */
        for(i = 0; i < pMonitor->basenum; i++)
        {
            pTask = (TAKS_TEMPLATE_T *)pMonitor->pBase;
            ptHandle = (TASK_HANDLE_T *)pTask[i].handle;

            if(0 == pTask[i].timeout || NULL == ptHandle)
            {
                continue;
            }

            /* 监控报错 */
            if(TASK_STATUS_ERROR == ptHandle->status)
            {
                TASK_ERROR_LOG("任务%s错误, 退出程序\n", pTask->name);
                goto endprocess;
            }

            if(TASK_STATUS_WORK != ptHandle->status || curtime.tv_sec <= ptHandle->starttime)
            {
                continue;
            }

            timeout = curtime.tv_sec - ptHandle->starttime;
            if(timeout > pTask[i].timeout)
            {
                task_process_timeout(&(pTask[i]), ptHandle, timeout);
                goto endprocess;
            }
            else if(ptHandle->halftime == 0 && timeout >= pTask[i].timeout / 2)
            {
                /* 时间过半 告警一次打印 */
                ptHandle->halftime = 1;
                task_process_timeout(&(pTask[i]), ptHandle, timeout);
            }
            else if(timeout < pTask[i].timeout / 2)
            {
                ptHandle->halftime = 0;
            }
        }

        /* 一般任务 */
        for(i = 0; i < pMonitor->commnum; i++)
        {
            pTask = (TAKS_TEMPLATE_T *)pMonitor->pComm;
            ptHandle = (TASK_HANDLE_T *)pTask[i].handle;

            if(0 == pTask[i].timeout || NULL == ptHandle)
            {
                continue;
            }

            /* 监控报错 */
            if(TASK_STATUS_ERROR == ptHandle->status)
            {
                TASK_ERROR_LOG("任务%s错误, 退出程序\n", pTask->name);
                goto endprocess;
            }

            if(TASK_STATUS_WORK != ptHandle->status || curtime.tv_sec <= ptHandle->starttime)
            {
                continue;
            }

            timeout = curtime.tv_sec - ptHandle->starttime;
            if(timeout > pTask[i].timeout)
            {
                task_process_timeout(&(pTask[i]), ptHandle, timeout);
                goto endprocess;
            }
            else if(ptHandle->halftime == 0 && timeout >= pTask[i].timeout / 2)
            {
                /* 时间过半 告警一次打印 */
                ptHandle->halftime = 1;
                task_process_timeout(&(pTask[i]), ptHandle, timeout);
            }
            else if(timeout < pTask[i].timeout / 2)
            {
                ptHandle->halftime = 0;
            }
        }

        sleep(1);
    }

endprocess:
    /* 超时检测退出进程 */
    diag_sig_handler(0);  //保存日志
    sleep(1);
    exit(0);
    return 0;
}




