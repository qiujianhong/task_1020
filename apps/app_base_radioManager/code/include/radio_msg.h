/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP 消息处理
* @date：    2021-7-8
* @history： 
*********************************************************************
*/

#ifndef __RADIO_MSG_H__
#define __RADIO_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif
extern uint32 g_RadioSpeakerFlag;

#define RADIO_MQTT_MSG_LEN      8192	/*缓冲区大小，共用缓冲区及各线程缓冲区大小*/

typedef struct tagRADIOMsgHandle
{
    time_t   timeflag; //定时查询标记
    char     topic[256];
    uint8    recvBuf[RADIO_MQTT_MSG_LEN];
}RADIO_MSGHANDLE_T;

/* 入口函数申明 */
typedef int (*MsgProcFun)(MESSAGE_INFO_T *, RADIO_MSGHANDLE_T *, RADIO_DATA_T *);
typedef struct tagRADIOMsgFun
{
    uint16             IID;      /* 线程ID */
    uint16             IOP;      /* 线程入口函数 */
    MsgProcFun         pProcMsg; /* 函数入参 */
}RADIO_MSGFUN_T;

typedef struct tagLongSend
{
    uint8  sendbuf;     //发送的内容 默认0x55
    uint16 sendtime;    //持续发送的时间
}RADIO_LONG_SEND_T;

//检定消息是否先回码
#define CHECK_METER_ASYNC 1

#ifdef __cplusplus
}
#endif

#endif //__RADIO_MSG_H__

