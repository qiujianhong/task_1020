/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP 消息处理
* @date：    2021-7-8
* @history： 
*********************************************************************
*/

#ifndef __BLE_MSG_H__
#define __BLE_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MQTT_MSG_LEN      8192	/*缓冲区大小，共用缓冲区及各线程缓冲区大小*/

typedef struct tagBLEMsgHandle
{
    time_t   timeflag; //定时查询标记
    char     topic[256];
    uint8    recvBuf[BLE_MQTT_MSG_LEN];
}BLE_MSGHANDLE_T;

/* 入口函数申明 */
typedef int (*MsgProcFun)(MESSAGE_INFO_T *, BLE_MSGHANDLE_T *, BLE_DATA_T *);
typedef struct tagBLEMsgFun
{
    uint16             IID;      /* 线程ID */
    uint16             IOP;      /* 线程入口函数 */
    MsgProcFun         pProcMsg; /* 函数入参 */
}BLE_MSGFUN_T;

//检定消息是否先回码
#define CHECK_METER_ASYNC 1


#ifdef __cplusplus
}
#endif

#endif //__BLE_MSG_H__

