/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：    通信模块其他功能：外部消息收发\配置操作头文件
* @date：     2020-3-7
* @history： 
*********************************************************************
*/
#ifndef	__COMM_MSG_H
#define __COMM_MSG_H

#ifdef __cplusplus
        extern "C" {
#endif


/* 定时器处理函数 精度1s */
typedef void (*TimeFUN)(int, void*);  //入参: socked, arg
typedef struct tagCommTimer
{
    uint16     nTime;      /* 定时器时长 */
    uint16     nTimeCnt;   /* 定时器内部计数 */
    TimeFUN    pTimeFunc;
    void      *pArg;
}COMM_TIMER_T;


typedef int (*MsgFUN)(uint8*, uint16, void*);  //入参: msg, msglen, pArg
typedef struct tagCommMsg
{
    uint32     msgid;
    uint16     label;
    MsgFUN     pMsgFunc;
    void      *pArg;
}COMM_MSG_T;


/* 对外消息处理线程句柄 */
typedef struct tagCommOutMsgHandle
{
    uint8              nTimerNum;
    uint8              nMsgNum;
    uint8              res[2];
    COMM_TIMER_T       tTImer[16];
    COMM_MSG_T         tMsg[48];
    
    int                socketfd;
    int                syncsocket;
    uint8             *pRunFlag;
}COMM_OUTMSG_HANDLE_T;


void comm_outtimer_add(COMM_OUTMSG_HANDLE_T *ptHandle, uint16 outTime, TimeFUN pFunc, void *pArg);
void comm_outmsg_add(COMM_OUTMSG_HANDLE_T *ptHandle, uint32 msgid, uint16 label, MsgFUN pFunc, void *pArg);
void *comm_outmsg_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif

