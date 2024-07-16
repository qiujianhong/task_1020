/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：    通信模块其他功能：外部消息收发\配置操作头文件
* @date：     2019-10-31
* @history： 
*********************************************************************
*/
#ifndef	__MSG_COMM_H
#define __MSG_COMM_H

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_OUTMSG_DEFAULR_LABEL     0xffff

typedef int (*CheckFUN)(int, void *, uint8 *);

typedef struct tagCommCheck
{
    CheckFUN   pCheckFunc;
    void      *pCfg;
    uint8     *pRestart;
}COMM_CHECK_T;

/* 数据中心消息处理线程句柄 */
typedef struct tagCommCfgMsgHandle
{
    int                dbclient;
    uint8              unNum;
    uint8              res[3];
    COMM_CHECK_T       tCheck[12];

    uint8             *pRunFlag;
}COMM_CFGMSG_HANDLE_T;

/* 定时器处理函数 精度1s */
typedef void (*TimeFUN)(int, void*);  //入参: socked, arg
typedef struct tagCommTimer
{
    uint16     nTime;      /* 定时器时长 */
    uint16     nTimeCnt;   /* 定时器内部计数 */
    TimeFUN    pTimeFunc;
    void      *pArg;
}COMM_TIMER_T;


typedef int (*MsgFUN)(uint8*, uint16, uint16, void*);  //入参: msg, msglen, label, pArg
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
    COMM_TIMER_T       tTImer[16];  /* 定时器回调函数 */
    COMM_MSG_T         tMsg[48];    /* 消息注册回调函数 */
    
    int                socketfd;    /* 异步消息句柄 */
    int                syncsocket;  /* 同步消息句柄 */
    uint8             *pRunFlag;
}COMM_OUTMSG_HANDLE_T;


void comm_cfgmsg_add(COMM_CFGMSG_HANDLE_T *ptHandle, CheckFUN pFunc, void *pArg, uint8 *pRestart);
void comm_add_writedb(uint32 oad, uint8 *buf, uint16 len);
void *comm_cfgmsg_entry(void *pArg);

void comm_outtimer_add(COMM_OUTMSG_HANDLE_T *ptHandle, uint16 outTime, TimeFUN pFunc, void *pArg);
void comm_outmsg_add(COMM_OUTMSG_HANDLE_T *ptHandle, uint32 msgid, uint16 label, MsgFUN pFunc, void *pArg);
void *comm_outmsg_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif
