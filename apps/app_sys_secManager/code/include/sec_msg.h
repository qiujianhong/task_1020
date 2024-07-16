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
#ifndef	__SEC_MSG_H
#define __SEC_MSG_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define SEC_STATUS_LOGIN   0
#define SEC_STATUS_SEEK    1
#define SEC_STATUS_WORK    2

#define SEC_LOST_HEARTBEAT_TIME  300

#define SEC_TOPIC_LEN (256)

/* 消息线程私有数据 */
typedef struct tag_SEC_MSGDATA
{
    uint8         appStep;
    uint8         res[3];
    time_t        timeflag;  //定时查询标记
    int           tasksocket;
    char          topic[SEC_TOPIC_LEN];
    uint8         recvBuf[TASK_MSG_MAX_LEN];
    uint8         isNoFree; //消息结构不释放，有些放入队列的，不能在该线程释放
}SEC_MSGDATA_T;

/* 入口函数申明 */
typedef int (*MsgProcFun)(MESSAGE_INFO_T*, void *, void *);
typedef struct tagLcMsgFun
{
    uint16             IID;
    uint16             IOP;
    MsgProcFun         pProcMsg; /* 处理函数 */
}SEC_MSGFUN_T;

/* 入口函数申明 */
typedef int (*TaskMsgProcFun)(TASK_MSG_t *);
typedef struct tagLcTaskMsgFun
{
    uint32             msgid;
    TaskMsgProcFun     pProcMsg; /* 处理函数 */
}SEC_TASKMSGFUN_T;

#ifdef __cplusplus
}
#endif

#endif


