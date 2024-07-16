/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： msgHandler.h
           消息处理接口头文件
* @date： 2019-11-9
*********************************************************************
*/

#ifndef __MSG_SERVICE_BASE_H__
#define __MSG_SERVICE_BASE_H__

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #include "commonConfig.h"
#include "baseConstant.h"
#include "baseType.h"
#include "msgHelper.h"
#include "msgPqueue.h"
#include "qlibc.h"
#include "threadpool.h"
#include "timeHelper.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief:声明msg客端描述象  */
typedef struct _msg_client_obj MSG_CLIENT_T;

/* @brief:声明客户端共用体  */
typedef union _acp_client CLIENT_U;

/* @brief:声明选择消息接口处理函数  */
typedef sint32 (*SEND_MSG_T)(MSG_CLIENT_T *client, uint8 *msg, uint32 msgLen);

/* @brief:通用事件标识  */
typedef enum _general_event {

    EMERGENCY_BLACKOUT = 0x00000001,
    SYSTEM_BLACKOUT    = 0x00000002,
    SYSTEM_RESET       = 0x00000003,
    SYSTEM_POWER_ON    = 0x00000004,
    SYSTEM_CLOCK_CHECK = 0x00000005,

    DB_UPDATE = 0x00050001,
    DB_INIT   = 0x00050002

} GENEERAL_EVENT_E;

/* @brief:声明TCP客户端  */
struct _msg_tcp_client
{
    sint8  ip[IP_LEN];
    uint16 port;
};

/* @brief:声明UDP客户端  */
struct _msg_udp_client
{
    sint8  ip[IP_LEN];
    uint16 port;
};

/* @brief:声明IPC客户端  */
struct _msg_ipc_client
{
    sint8 unixSockertPath[UNIX_SOCKET_PATH_LEN];
};

/* @brief:声明MQTT客户端  */
struct _msg_mqtt_client
{
};

/* @brief:声明客户端共用体  */
union _acp_client
{
    struct _msg_tcp_client  tClient;
    struct _msg_udp_client  uClient;
    struct _msg_ipc_client  iClient;
    struct _msg_mqtt_client mclient;
};

/* @brief:msg 客户端短息  */
struct _msg_client_obj
{
    MSG_TYPE_E type;                          //客户端类型
    sint8      clientName[MAX_APP_NAME_LEN];  //客户端名称
    CLIENT_U   client;                        //客户端共用体
                                              // SEND_MSG_T msg_send_fp;
};

/* @brief:功能描述  */
typedef struct _msg_hander_obj
{
    void *obj;  //由TCP UDP MQTT IPC 传输的上下文目标对象
    sint32 (*getReqDataMsgFp)(void *obj, MSG_OBJ_T **msgObj);
    // 从目标对象中获取来自APP的请求消息实体的指针
    sint32 (*getReqDataFp)(void *obj, RAW_OBJ_T **raw_buf);
    //获取回复MSG对象地址
    sint32 (*getRspDataMsgFp)(void *obj, MSG_OBJ_T **msgObj);
    // 消息处理完后设置目标对象需要回复的数据报文
    sint32 (*getRspDataFp)(void *obj, RAW_OBJ_T **raw_buf);
    //设置异步消息接口//保留
    sint32 (*setAsycnFp)(void *obj, sint8 *source, sint8 *des);
    //执行目标对象中数据回复流程
    sint32 (*handerFp)(void *obj);
    //获取客户端信息
    sint32 (*getClientInfo)(void *obj, MSG_CLIENT_T *client);
    //数据包优先级
    sint32 priority;

#ifdef XL_TIME_TRACE
    struct timeval recvDone;
    struct timeval parseDone;
    struct timeval queueDone;
    struct timeval dequeueDone;
    struct timeval handerStart;
    struct timeval handerDone;
#endif

} MSG_HANDER_OBJ_T;

sint32 base_queue_message(MSG_HANDER_OBJ_T *msgHanderObj);

void set_queue_message_fp(sint32 (*fp)(MSG_HANDER_OBJ_T *msgHanderObj));

void msg_life_cycle_dump(MSG_HANDER_OBJ_T *msgHanderObj);

#ifdef __cplusplus
}
#endif

#endif