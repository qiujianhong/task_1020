#ifndef __SMIOS_CLIENT_H__
#define __SMIOS_CLIENT_H__

#include "baseConstant.h"
#include "event2/event-config.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/util.h"
#include "msgHelper.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _smios_connect
{
    MSG_TYPE_E type;                //连接类型
    sint8      smiosIP[IP_LEN];     //smios IP地址
    uint16     smiosPort;           //smios端口号
    struct bufferevent *bev ;
    pthread_t tid;                  // smiOS连接线程的ThreadIP
    struct event notify_event;      // event 异步通知事件
    struct event recv_event;        // event 异步通知事件
    sint32     notifyReceiveFd;     //异步处理接收文件描述符
    sint32     notifySendFd;        //异步处理发送描述符
} SMIOS_CONNECT_T;

//初始化smiOS客户端线程
sint32 init_smiOS_connect(MSG_TYPE_E type, sint8 *ip, uint16 port);
//发送数据至smiOS请求
sint32 sendto_smiOS(sint8 *data, uint32 dataLen);

#ifdef __cplusplus
}
#endif

#endif