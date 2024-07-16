/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   检查告警事务进程
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __IOT_INSPECT_H
#define __IOT_INSPECT_H
    
#ifdef __cplusplus
        extern "C" {
#endif
#include "iot_update.h" 

//规约解析模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define IOTMONTIOR_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define IOTMONTIOR_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define IOTMONTIOR_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define IOTMONTIOR_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define IOTMONTIOR_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define IOTMONTIOR_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

#define DOCKERMAXLEN 20
#define datalenMax 1024
#define MAX_QUEUE_CNT  100       //最大队列个数
#define APP_IN_DOCK_MAX 10       //每个容器内最多有10个app

typedef struct
{
    int32  QueueLen;                           //队列长度
    uint8   QueueBuf[1400];     //队列存储缓冲区
} Queue_T;
//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                          //队列头
    int16  QueueTail;                          //队列尾
    Queue_T Queue[MAX_QUEUE_CNT];               //通讯数据队列
} LockQueue_T;

LockQueue_T        LockWarringMsg;
LockQueue_T        LockRecvMsg;


typedef struct
{   
    uint8 cpu_limit;
    uint8 mem_limit;
}iot_app_limit;


typedef struct
{   
    uint16 cpu_limit;
    uint16 mem_limit;
    uint16 disk_limit;
//    iot_app_limit app_limit[APP_IN_DOCK_MAX] ;
}iot_limit;


typedef struct
{   
    uint8  Appname[100];
    uint16 mem_limit;
    uint16 cpu_limit;
    uint16 mem_usage;
    uint16 cpu_usage;
    uint8 cputype;        //触发事件置标志 1
    uint8 memtype;        //触发事件置标志 1
    time_t thistime;
    time_t lasttime;
    uint8 status;           //容器状态 true:运行中 false:容器停止了 4
    uint8 enable;
}iot_app_info;


/*判断容器是否重启需要为一个容器提供一个结构体*/
typedef struct 
{
    
    uint8  name[100];
    uint16 cpu_usage;        // cpu占用率 0  越限置标志位1
    uint16 mem_usage;        //内存占用率 1 
    uint16 disk_usage;       //磁盘占用率 2
    uint8 cputype;        //触发事件置标志 1
    uint8 memtype;        //触发事件置标志 1
    uint8 disktype;
    uint32 runtime;        //开始时间不一致则容器重启 3
    uint32 Lastruntime;    //上一轮的runtime
    uint8 status;           //容器状态 true:运行中 false:容器停止了 4
    uint8 appNum;
    uint8 appindex;
         //
    iot_app_info apprun[APP_IN_DOCK_MAX];     //容器所有app的运行信息，每个容器最多有10个APP
    iot_limit runlimit;
}iot_docker_info;

typedef struct
{
    uint8 conlen;
    uint8 conName[100];
}appList;

typedef struct
{
    uint8 conlen;
    uint8 conName[100];
    uint8 appnum;
    appList app_t[APP_IN_DOCK_MAX];
}containerList;

typedef struct
{
    uint8 conNum;
    containerList conList[DOCKERMAXLEN];
}iot_inspec_conList;

typedef struct
{
    uint8 cputype;
    uint8 memtype;
    uint8 disktype;
    uint8 cpuTUtype;
    uint8 cpuTDtype;
}dev_run_type;

typedef struct
{
  dev_run_info dev_run;
  dev_run_type dev_type;
}iot_run_info;

typedef struct 
{
  uint8 InitFLag;
  uint8 dockernum;
  uint32 MonitorTime;
  //runstatus run_status;
  //monstatus moniitortype;
  //uint8 dockerindex;
  iot_docker_info docker_runinfo[DOCKERMAXLEN];
  iot_run_info dev_info_t;
  iot_inspec_conList inspect_conlist;
}IOT_INSPECT_T;





#ifdef __cplusplus
}
#endif

#endif /* __IOT_INSPECT_H */


