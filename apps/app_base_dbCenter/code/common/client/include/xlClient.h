/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief： xlClient.h
* @date： 2020-6-10
*********************************************************************
*/
#ifndef __XL_CLIENT_H__
#define __XL_CLIENT_H__

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "baseConstant.h"
#include "baseType.h"
#include "channelStatus.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef XL_CHANNEL_CONNECT_STATUS_E CLIENT_STATUS_E;

typedef union _u_client XL_CLIENT_U;

typedef struct _xlClient XL_CLIENT_T;

typedef struct _xl_mqtt_topic
{
    char   topic[MAX_TOPIC_LEN];            
    sint32 qos;
} MQTT_TOPIC_T;

//tcp client
typedef struct _client_tcp
{
    sint8  server_ip[IP_LEN];                   //服务器ip
    uint16 server_port;                         //服务器port
    sint8  local_ip[IP_LEN];                    //client ip
    uint16 local_port;                          //client port
    sint32 tfd;
} CLIENT_TCP_T;

//udp client
typedef struct _XL_CLIENT_Udp
{
    sint8  server_ip[IP_LEN];                   //服务器IP
    uint16 server_port;                         //服务器port
    sint8  local_ip[IP_LEN];                    //client ip
    uint16 local_port;                          //client port
    uint16 connect_timeout_ms;                  //连接超时
    sint32 ufd;
} CLIENT_UDP_T;

//mqtt client
typedef struct _client_mqtt
{
    sint8  server_ip[IP_LEN];                   //服务器地址
    uint16 server_port;                         //服务器端口
    sint8  local_ip[IP_LEN];                    //client ip
    uint16 local_port;                          //client port
    sint8  user_id[MAX_USER_ID_LEN];            //用户ID
    sint8  user_name[MAX_USER_NAME_LEN];        //用户名
    sint8  user_passwd[MAX_PASSWD_LEN];         //用户密码
    sint8  default_send_topic[MAX_TOPIC_LEN];   //默认发送主题
    sint8  default_send_topic_qos;              //默认发送主题qos
    MQTT_TOPIC_T sub_topics[MAX_SUB_NUM];       //订阅主题
    uint16 connect_timeout_s;                   //连接超时函数
    void * mqttHandler;
    void * queue;
    //sem_t  recvSem;
    sint32 pipe_fd[2]; //fd[0]: read fd, fd[1]: write fd
} CLIENT_MQTT_T;

//IPC client
typedef struct _client_ipc
{
    sint8  server_path[UNIX_SOCKET_PATH_LEN];
    sint8  loacl_path[UNIX_SOCKET_PATH_LEN];
    uint16 connect_timeout_ms;
    sint32 ifd;
} CLIENT_IPC_T;

//初始化函数
typedef sint32 (*CLIENT_INIT_FP)(XL_CLIENT_T *c);
//连接函数
typedef sint32 (*CLIENT_START_CONNECT_FP)(XL_CLIENT_T *c);
//获取连接状态函数
typedef CLIENT_STATUS_E (*CLIENT_GET_CONNECT_STATUS_FP)(XL_CLIENT_T *c);
//发送消息函数
typedef sint32 (*CLIENT_WRITE_FP)(XL_CLIENT_T *c,
                                  const char * data,
                                  sint32       dataLen);
//接收消息函数
typedef sint32 (*CLIENT_READ_FP)(XL_CLIENT_T *c, char *data, sint32 dataLen);
//断开连接函数
typedef sint32 (*CLIENT_START_DISCONNECT_FP)(XL_CLIENT_T *c);

union _u_client
{
    CLIENT_TCP_T  tclient;                                  //tcp client
    CLIENT_UDP_T  uclient;                                  //udp client
    CLIENT_MQTT_T mclient;                                  //mqtt client
    CLIENT_IPC_T  iclient;                                  //ipc client
};

struct _xlClient
{
    CLIENT_STATUS_E              status;                    //客户端状态
    CLIENT_INIT_FP               initFp;                    //初始化函数
    CLIENT_START_CONNECT_FP      connectFp;                 //连接函数
    CLIENT_GET_CONNECT_STATUS_FP getConnectStatusFp;        //获取连接状态函数
    CLIENT_READ_FP               readFp;                    //接收消息函数
    CLIENT_WRITE_FP              writeFp;                   //发送消息函数
    CLIENT_START_DISCONNECT_FP   disconnectFp;              //断开连接函数
    XL_CLIENT_U                  uc;
};

#ifdef __cplusplus
}
#endif

#endif