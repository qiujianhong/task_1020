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

typedef XL_CHANNEL_CONNECT_STATUS_E CLIENT_STATUS_E;

typedef union _u_client XL_CLIENT_U;

typedef struct _xlClient XL_CLIENT_T;

typedef struct _xl_mqtt_topic
{
    char   topic[MAX_TOPIC_LEN];
    sint32 qos;
} MQTT_TOPIC_T;

typedef struct _client_tcp
{
    sint8  server_ip[IP_LEN];
    uint16 server_port;
    sint8  local_ip[IP_LEN];
    uint16 local_port;
    sint32 tfd;
} CLIENT_TCP_T;

typedef struct _XL_CLIENT_Udp
{
    sint8  server_ip[IP_LEN];
    uint16 server_port;
    sint8  local_ip[IP_LEN];
    uint16 local_port;
    uint16 connect_timeout_ms;
    sint32 ufd;
} CLIENT_UDP_T;

typedef struct _client_mqtt
{
    sint8  server_ip[IP_LEN];
    uint16 server_port;
    sint8  local_ip[IP_LEN];
    uint16 local_port;
    sint8  user_id[MAX_USER_ID_LEN];
    sint8  user_name[MAX_USER_NAME_LEN];
    sint8  user_passwd[MAX_PASSWD_LEN];
    sint8  default_send_topic[MAX_TOPIC_LEN];
    MQTT_TOPIC_T sub_topics[MAX_SUB_NUM];
    uint16 connect_timeout_ms;
    void * mqttHandler;
    void * queue;
    sem_t  recvSem;
} CLIENT_MQTT_T;

typedef struct _client_ipc
{
    sint8  server_path[UNIX_SOCKET_PATH_LEN];
    sint8  loacl_path[UNIX_SOCKET_PATH_LEN];
    uint16 connect_timeout_ms;
    sint32 ifd;
} CLIENT_IPC_T;

typedef sint32 (*CLIENT_INIT_FP)(XL_CLIENT_T *c);

typedef sint32 (*CLIENT_START_CONNECT_FP)(XL_CLIENT_T *c);

typedef CLIENT_STATUS_E (*CLIENT_GET_CONNECT_STATUS_FP)(XL_CLIENT_T *c);

typedef sint32 (*CLIENT_WRITE_FP)(XL_CLIENT_T *c,
                                  const char * data,
                                  sint32       dataLen);

typedef sint32 (*CLIENT_READ_FP)(XL_CLIENT_T *c, char *data, sint32 dataLen);

typedef sint32 (*CLIENT_START_DISCONNECT_FP)(XL_CLIENT_T *c);

union _u_client
{
    CLIENT_TCP_T  tclient;
    CLIENT_UDP_T  uclient;
    CLIENT_MQTT_T mclient;
    CLIENT_IPC_T  iclient;
};

struct _xlClient
{
    CLIENT_STATUS_E              status;
    CLIENT_INIT_FP               initFp;
    CLIENT_START_CONNECT_FP      connectFp;
    CLIENT_GET_CONNECT_STATUS_FP getConnectStatusFp;
    CLIENT_READ_FP               readFp;
    CLIENT_WRITE_FP              writeFp;
    CLIENT_START_DISCONNECT_FP   disconnectFp;
    XL_CLIENT_U                  uc;
};

#endif