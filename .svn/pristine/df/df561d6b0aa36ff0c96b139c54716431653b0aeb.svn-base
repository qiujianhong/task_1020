/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控消息处理
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef	__LC_MSG_H
#define __LC_MSG_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define  LC_STATUS_LOGIN   0
#define  LC_STATUS_SEEK    1
#define  LC_STATUS_WORK    2

#define  LC_LOST_HEARTBEAT_TIME  300

#define  LC_TOPIC_LEN (256)

/* 回路监控公共数据 */
typedef struct taglcMsgData
{
    uint8    appStep;
    uint8    res[3];
    time_t   hearttime;  
    time_t   timeflag;  //定时查询标记    
    uint64   procnum;  //MQTT消息总数
    int      tasksocket;
    char     topic[LC_TOPIC_LEN];    
    uint8    recvBuf[TASK_MSG_MAX_LEN];
    int      recvlen;
}LC_MSGDATA_T;

/* 入口函数申明 */
typedef int (*MsgProcFun)(MESSAGE_INFO_T*, void *, void *);
typedef struct tagLcMsgFun
{
    uint16             IID;
    uint16             IOP;
    MsgProcFun         pProcMsg; /* 处理函数 */
}LC_MSGFUN_T;

/* 入口函数申明 */
typedef int (*TaskMsgProcFun)(TASK_MSG_t *);
typedef struct tagLcTaskMsgFun
{
    uint32             msgid;
    TaskMsgProcFun     pProcMsg; /* 处理函数 */
}LC_TASKMSGFUN_T;

#ifdef __cplusplus
}
#endif

#endif


