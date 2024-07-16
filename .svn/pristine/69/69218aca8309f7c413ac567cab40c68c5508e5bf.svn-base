/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief： channelStatus.h
* @date： 2020-6-10
*********************************************************************
*/
#ifndef __CHANNEL_STATUS_H__
#define __CHANNEL_STATUS_H__

#ifdef __cplusplus
extern "C" {
#endif

//客户端协议类型
typedef enum _client_type {
    CLIENT_UDP = 0,
    CLIENT_TCP,
    CLIENT_MQTT,
    CLIENT_IPC
} CLIENT_TYPE_E;

//客户端通道状态
typedef enum _channel_status {
    CH_INIT = 0,
    CH_CONNECTING,
    CH_CONNECT_FAIL,
    CH_CONNECT_SUCC
} XL_CHANNEL_CONNECT_STATUS_E;

#ifdef __cplusplus
}
#endif

#endif
