/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   线程操作基础
* @date：    2020-6-29
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef    __TASK_BASE_H
#define __TASK_BASE_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define TASK_STATUS_ERROR   (-1)
#define TASK_STATUS_INIT    0
#define TASK_STATUS_WORK    1
#define TASK_STATUS_STOP    2

#define TASK_USERLOG_BUF   (1024)

/* 线程句柄 */
typedef struct tagTaskHandle
{
    pthread_t  threadid;       //线程id
    pid_t      tid;            //线程Tid
    time_t     starttime;      //任务处理开始时间
    int        status;         //线程状态
    int        baseOk;         //基础APP设置启动正常
    int        halftime;       //线程运行时间过半(超时时间)

    void      *pPArg;          //进程共有数据(进程初始化后返回)
    void      *pTArg;          //线程私有数据(线程初始化后返回)
    char       userlog[TASK_USERLOG_BUF];   //记录线程当前工作, 用户自己调用接口设置, 任务超时打印出来
}TASK_HANDLE_T;

/* 获取进程共有数据 */
#define TASK_PUBLIC_DATA(pp) (NULL == (pp) ? NULL : ((TASK_HANDLE_T *)(pp))->pPArg)
/* 获取线程私有数据 */
#define TASK_PRIVATE_DATA(pp) (NULL == (pp) ? 0 : ((TASK_HANDLE_T *)(pp))->pTArg)

/* 获取设置线程状态 */
#define TASK_GET_STATUS(pp) (NULL == (pp) ? 0 : ((TASK_HANDLE_T *)(pp))->status)
#define TASK_SET_STATUS(pp, ss) \
    if(NULL != pp)\
    {\
        ((TASK_HANDLE_T *)(pp))->status = ss;\
    }

#define BASE_TASK_SET_OK(pp) \
    if(NULL != pp)\
    {\
        ((TASK_HANDLE_T *)(pp))->baseOk = 1;\
    }

/* 设置任务运行开始事件 */
#define TASK_SET_STARTTIME(pp) \
    do{\
        struct timespec curtime = {0};\
        clock_gettime(CLOCK_MONOTONIC, &curtime);\
        ((TASK_HANDLE_T *)(pp))->starttime = curtime.tv_sec;\
    }while(0);

#define TASK_SET_USERLOG(pp, x...)  snprintf(((TASK_HANDLE_T *)(pp))->userlog, TASK_USERLOG_BUF, x);

/* 线程监控 */
typedef struct tagTaskMonitor
{
    int        basenum;
    void      *pBase;          //基础任务

    int        commnum;
    void      *pComm;          //一般任务
}TASK_MONITOR_T;


/*
********************************************
* 功能: 基础任务处理入口
* 入参: 日志路径, 进程初始化操作函数
* 出参: 进程共有句柄
* 返回: 0-成功/其他, 失败
********************************************
*/
int process_init(char *logpath, void *pfunc, void **ppData);

void *task_entry_thread(void *pArg);
void *task_monitor_thread(void *pArg);

#ifdef __cplusplus
}
#endif

#endif

