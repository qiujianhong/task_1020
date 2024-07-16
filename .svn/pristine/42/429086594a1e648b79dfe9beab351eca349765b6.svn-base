/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号消息处理
* @date：    2021-3-22
* @history： 
*********************************************************************
*/

#ifndef __MSG_PROC_H__
#define __MSG_PROC_H__

#include "dataType.h"
#include "oopType.h"
#include "framecomm.h"

#ifdef __cplusplus
extern "C" {
#endif


#define WIRELESS_MQTT_MSG_LEN      5120	/*缓冲区大小，共用缓冲区及各线程缓冲区大小*/

typedef struct tagWirelessMsg
{
    uint8    AppStep;
    uint8    res[3];
    time_t   timeflag; //定时查询标记
    uint8    recvBuf[WIRELESS_MQTT_MSG_LEN];
}WIRELESS_MSGHANDLE_T;

/* 入口函数申明 */
typedef int (*MsgProcFun)(MESSAGE_INFO_T *, void *, void *);
typedef struct tagWirelessMsgFun
{
    uint16             IID;      /* IID */
    uint16             IOP;      /* IOP */
    uint32             rpm;      /* rpm */
    MsgProcFun         pProcMsg; /* 函数入参 */
}WIRELESS_MSGFUN_T;

typedef struct tagWirelessMana
{
    time_t   timeFlag;    /* 定时器标记 */
    
    uint8    GetStatus;   /* 拨号模组状态查询 */
    
    uint8    DCM0Status;  /* 设备1查询状态 */
    uint8    DCM0Port;    /* 设备1模组的位置 槽位+子模块 */
    uint8    DCM0SubPort; /* 设备1子模组位置0表示无子模组, 从1开始(用于双模块) */
    
    uint8    DCM1Status;  /* 设备2查询状态 */
    uint8    DCM1Port;    /* 设备2模组的位置 槽位+子模块 */
    uint8    DCM1SubPort; /* 设备2子模组位置0表示无子模组, 从1开始(用于双模块) */

    uint8    lastPort;    /* 上次复位模组位置(Port和SubPort的合并值) */
    time_t   lastRestart; /* 上次复位模组时间 */

    uint8    DCMRestore[2]; /* 设备复位工作状态 TRUE-FALSE */
    uint8    res[2];
    time_t   tDCMStart[2];  /* 模组开始复位时间 */
}WIRELESS_MANAGER_T;


#ifdef __cplusplus
}
#endif

#endif //__MSG_PROC_H__


