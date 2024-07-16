/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   线程定义
* @date：    2020-6-29
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __TASK_TEMPLATE_H
#define __TASK_TEMPLATE_H
    
#ifdef __cplusplus
        extern "C" {
#endif


/*
********************************************
* 功能: 界面版本号打印
* 入参: uint32 缓存长度
* 出参: char*  输出日志
* 返回: 实际长度
********************************************
*/
typedef int (*versionfunc)(char*, uint32);

/*
********************************************
* 功能: 进程初始化注册操作函数
* 入参: 无
* 出参: 进程共有句柄, 在每个线程中可以获取到
* 返回: 0-成功/其他, 失败
********************************************
*/
typedef int (*initfunc)(void**);

/*
********************************************
* 功能: 任务初始化
* 入参: 进程公共数据
* 出参: 私有句柄
* 返回: 0-成功/其他, 失败
********************************************
*/
typedef int (*taskinit)(void* , void**);

/*
********************************************
* 功能: 任务去初始化
* 入参: 私有句柄
* 出参: 无
* 返回: 无
********************************************
*/
typedef void (*taskuninit)(void*);

/*
********************************************
* 功能: 任务处理入口; 基础任务中需要执行 BASE_TASK_SET_OK 设置状态后才能启动一般任务
* 入参: 线程句柄, 进程公有数据, 线程私有数据
* 出参: 
* 返回: 下次调度延时时间 us
********************************************
*/
typedef int (*taskproc)(void *, void*, void*);

#define TASK_NAME_LEN    16
/* 任务定义 */
typedef struct tagTaskTemplate
{
    char           name[TASK_NAME_LEN];    //名称
    int16          priority;               //优先级 nice(priority) 0-默认值, 不设置
    unsigned short stacksize;              //堆栈大小, 单位KB, 0-默认值8MB
    unsigned int   timeout;                //超时时间 0-表示不监控运行超时

    taskinit       initfunc;               //初始化函数(生成私有数据)
    taskproc       procfunc;               //任务调度函数
    taskuninit     uninitfunc;             //任务资源回收函数

    void        *handle;                 //线程句柄
}TAKS_TEMPLATE_T;


#ifdef __cplusplus
}
#endif

#endif

