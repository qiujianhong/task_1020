#ifndef __TCP_SERVICE_H__
#define __TCP_SERVICE_H__

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

#define TCP_SERVER_NAME "tcp_server"

/* @brief:tcp通讯服务结构体封装  */
typedef struct _tcp_server_obj_
{
    sint32             listenFd;         //创建的TCP服务SockerFd
    uint32             stop;             // TCP服务的停止请求标志位
    struct event_base *base;             // event基础句柄
    struct event       eventAccept;      // 客户端accet数据
    struct event       notify_event;     // event 异步通知事件
    sint8              serverIp[16];     // TCP服务绑定的IP地址
    uint16             serverPort;       // TCP服务邦迪的服务端口
    sint32             notifyReceiveFd;  //异步处理接收文件描述符
    sint32             notifySendFd;     //异步处理发送描述符
    qhashtbl_t *       connectPool;      //客户端连接池
    pthread_t          tid;              // TCP服务线程的ThreadIP
} TCP_SERVICE_T;

/* @brief:tcp通讯客户端结构体封装  */
typedef struct _tcp_client
{
    int                 clientFd;          //客户端描述符
    sint8               clientIP[IP_LEN];  //客户端IP
    uint16              clientPort;        //客户端端口
    struct bufferevent *bufev;             //客户端缓存buffer对象
} TCP_CLIENT_T;

/* @brief:一次消息请求的上下文对象  */
typedef struct _tcp_client_ctx
{
    MSG_OBJ_T reqMsg;            //请求消息头
    RAW_OBJ_T reqRawObj;         //请求信息对象
    MSG_OBJ_T rspMsg;            //回复消息头
    RAW_OBJ_T rspRawObj;         //回复信息对象
    sint8     clientIP[IP_LEN];  //消息发送者IP
    uint16    clientPort;        //消息发送者端口
} TCP_CLIENT_CTX_T;

/* @brief:异步线程写buffer_event参数 */
typedef struct
{
    struct bufferevent *bufev;    // buffer_event对象
    void *              data;     //需要发送的数据
    uint32              dataLen;  //发送数据的长度
} WRITE_BUFFER_EVENT_PARAM_T;

/* @brief:断开链接请求参数 */
typedef struct
{
    sint8  ip[IP_LEN];  //需要断开客户端IP
    uint16 port;        //需要断开客户端端口
} DISCONNECT_CLIENT_PARAM_T;

/* @brief:tcp通讯服务系统初始化  */
sint32 tcp_init_system(void);

/* @brief:tcp通讯服务清理系统  */
void tcp_clean_up_system(void);

/* @brief:tcp通讯服务启动系统  */
sint32 tcp_start_system(void);

/* @brief:tcp通讯服务停止系统  */
void tcp_stop_system(void);

void set_tcp_bind_ip(sint8 ip[IP_LEN]);

void set_tcp_bind_port(uint16 port);

/* @brief:tcp数据发送接口  */
sint32 tcp_sendto(sint8 ip[IP_LEN], uint16 port, void *data, uint32 dataLen);

#ifdef __cplusplus
}
#endif

#endif