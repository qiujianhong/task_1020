/*
*********************************************************************
* Copyright(C) 2022 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2022-3-20
*********************************************************************
*/
#ifndef __RSP_MSG_H__
#define __RSP_MSG_H__


#ifdef __cplusplus
extern "C" {
#endif


#define RSP_MQTT_MSG_LEN      5120

typedef struct tagRspMsgHandle
{
    uint8    bReg;
    uint8    res[3];
    time_t   timeflag; //定时查询标记
    char     topic[256];
    uint8    recvBuf[RSP_MQTT_MSG_LEN];
}RSP_MSGHANDLE_T;

/* 入口函数申明 */
typedef int (*MsgProcFun)(MESSAGE_INFO_T *, void *, void *);
typedef struct tagRspMsgFun
{
    uint16             IID;      /* 线程ID */
    uint16             IOP;      /* 线程入口函数 */
    uint32             rpm;      /* rpm */
    MsgProcFun         pProcMsg; /* 函数入参 */
}RSP_MSGFUN_T;


#ifdef __cplusplus
}
#endif

#endif // __RSP_MQTT_H__

