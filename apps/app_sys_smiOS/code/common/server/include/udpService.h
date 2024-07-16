/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： udpService.h
           udp通讯头文件
* @date： 2019-11-9
*********************************************************************
*/

#ifndef __UDPSERVICE_H__
#define __UDPSERVICE_H__

#include "event2/event-config.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/util.h"
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>

#include "baseType.h"
#include "msgServiceBase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief:udp通讯服务结构体封装  */
typedef struct _udp_server_obj_
{
    uint32             serverSock;    //创建的UDP服务SockerFd
    uint32             stop;          // UDP服务的停止请求标志位
    struct event_base* base;          // event base句柄
    sint8              serverIp[16];  // UDP服务绑定的IP地址
    sint16             serverPort;    // UDP服务邦迪的服务端口
    pthread_t          tid;           // UDP服务线程的ThreadIP
} UDP_SERVICE_T;

/* @brief:udp通讯客户端结构体封装  */
typedef struct _udp_client_ctx
{
    sint32             serverSock;  //创建的UDP服务SockerFd
    struct sockaddr_in client_sin;  //需要回复的客户端地址
    MSG_OBJ_T          reqMsg;      //请求消息头
    RAW_OBJ_T          reqRawObj;   //请求信息对象
    MSG_OBJ_T          rspMsg;      //回复消息头
    RAW_OBJ_T          rspRawObj;   //回复信息对象
} UDP_CLIENT_CTX_T;

/* @brief:udp通讯服务系统初始化  */
sint32 udp_init_system(void);

/* @brief:udp通讯服务清理系统  */
void udp_clean_up_system(void);

/* @brief:udp通讯服务启动系统  */
sint32 udp_start_system(void);

/* @brief:udp通讯服务停止系统  */
void udp_stop_system(void);

void set_udp_bind_ip(sint8 ip[IP_LEN]);

void set_udp_bind_port(uint16 port);

/* @brief:udp 客户端数据发送接口 */
sint32 udp_sendto(sint8 ip[IP_LEN], uint16 port, void* data, uint32 dataLen);

#ifdef __cplusplus
}
#endif
#endif
