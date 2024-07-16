/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级app用采对外消息处理
* @date：    2020-1-6
* @history： 
*********************************************************************
*/

#ifndef __MSG_PROC_H__
#define __MSG_PROC_H__

#include "message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _STATUS_BAR_INFO
{
    uint32            disptime;    //显示时长，时长为0表示一直显示
    time_t            dispEnd;    //显示结束时间
    char              text[24];    //显示内容
}STATUS_BAR_INFO_T;

//弹出信息提示框
typedef struct _POP_MES_BOX
{    
    BOOL              is_pop;
    uint32            disptime;
    time_t            dispEnd;    //显示结束时间
    uint32            delayTime;
    char              text[512];
}POP_MES_BOX_T;

 typedef enum
{
    SIG_STRENGTH,      //信号强度，long_unsigned
    NET_FORMAT,        //网络制式，unsigned
    CON_STATUS,        //联机状态，bool
}REMOTE_COM_STATE_E;

typedef enum
{
    CHANNEL1,        //通道1，远程通信状态
    CHANNEL2,        //通道2，远程通信状态            
    ETH_CON_STATUS,  //以太网联机            bool
    WARN,            //告警        bool
    INFO_ID,         //信息点        long-unsigned
    KEY_STATUS,      //密钥状态        unsigned
    BLUE_STATUS,	 //蓝牙状态
    RADIO_STATUS,		//电台状态
}TOOL_BAR_FIELD_E;

typedef struct
{
    uint16     signalStrengh;    //信号强度
    uint8      netFormat;        //网络制式
    bool       conStatus;        //联机状态
}REMOTE_COM_STATE_T;

typedef struct 
{
    REMOTE_COM_STATE_T    channel1;        //通道1
    REMOTE_COM_STATE_T    channel2;        //通道2
    bool                  ethConStatus;    //以太网联机
    bool                  warning;        //告警
    uint16                infoID;            //信息点号
    uint8                 keyStatus;        //密钥状态
    uint8                 blueStatus;       //蓝牙状态
    uint8                 radioStatus;      //电台状态
}TOOL_BAR_INFO_T;

extern STATUS_BAR_INFO_T     statusInfoFresh;
extern POP_MES_BOX_T         g_pop_mes_Fresh;
extern TOOL_BAR_INFO_T        toolInfoFresh;

int desktop_msg_handle(MESSAGE_INFO_T *pInMsg);

#ifdef __cplusplus
}
#endif

#endif //__MQTT_MSG_H__

