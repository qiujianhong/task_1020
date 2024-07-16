/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： udpService.c
           实现udp通讯接口
* @date： 2019-11-9
*********************************************************************
*/

#include <arpa/inet.h>

#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "baseType.h"
#include "event2/event-config.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/util.h"


// #include "constant.h"
#include "msgServiceBase.h"
#include "udpService.h"
#include "olog.h"

//udp服务器句柄
static UDP_SERVICE_T *s_udpServer = NULL;
//日志输出
static void *         ocat        = NULL;

/**
*********************************************************************
* @brief：                  获取udp 传递给 msg层的原始数据
* @param[in]：
               obj          UdpClientCtx保留了从udp接收到的数据
* @param[out]：
               rawBuffer    将UdpClientCtx赋值给msg层rawBuffer
* @return： 返回值说明
*********************************************************************
*/
static sint32 udp_get_req_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    UDP_CLIENT_CTX_T *ctx = (UDP_CLIENT_CTX_T *)obj;
    //将udp 收到的数据传址给MSG层
    *rawBuffer = &ctx->reqRawObj;
    return 0;
}

/**
*********************************************************************
* @brief： 设置udp响应buf
* @param[in]：
               rawBuffer    udp需要回复的消息
* @param[out]：
               obj          UDP_CLIENT_CTX_T
* @return： 返回值说明
*********************************************************************
*/
static sint32 udp_get_rsp_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    UDP_CLIENT_CTX_T *ctx = (UDP_CLIENT_CTX_T *)obj;
    *rawBuffer            = &ctx->rspRawObj;
    return 0;
}

static sint32 udp_get_req_msg(void *obj, MSG_OBJ_T **msg)
{
    UDP_CLIENT_CTX_T *ctx = (UDP_CLIENT_CTX_T *)obj;
    // //将tcp 收到的数据传址给MSG层
    *msg = &ctx->reqMsg;
    return 0;
}

static sint32 udp_get_rsp_msg(void *obj, MSG_OBJ_T **msg)
{
    UDP_CLIENT_CTX_T *ctx = (UDP_CLIENT_CTX_T *)obj;
    // //将tcp 收到的数据传址给MSG层
    *msg = &ctx->rspMsg;
    return 0;
}

/**
*********************************************************************
* @brief： udp服务消息处理
* @param[in]： obj     UDP_CLIENT_CTX_T
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static sint32 udp_msg_handel_cb(void *obj)
{
    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)obj;
    UDP_CLIENT_CTX_T *ctx          = (UDP_CLIENT_CTX_T *)msgHanderObj->obj;
    socklen_t         server_sz    = sizeof(struct sockaddr_in);

    // olog_debug(ocat,"udp_msg_handel_cb sendto");
    if (ctx->rspRawObj.length > 0)
    {
        // holog_debug(ocat,ctx->rspRawObj.payload, ctx->rspRawObj.length);
        sendto(ctx->serverSock, ctx->rspRawObj.payload, ctx->rspRawObj.length,
               0, (struct sockaddr *)&ctx->client_sin, server_sz);
    }

#ifdef XL_TIME_TRACE
    getSysTime(&msgHanderObj->handerDone);
#endif

#ifdef XL_TIME_TRACE_DUMP
    msg_life_cycle_dump(msgHanderObj);
#endif

    //释放由udp_cb 中构建的ctx 及 msgHanderObj对象
    free(ctx);
    free(msgHanderObj);
    return 0;
}

static sint32 udp_get_client_info(void *obj, MSG_CLIENT_T *client)
{
    UDP_CLIENT_CTX_T *ctx = (UDP_CLIENT_CTX_T *)obj;

    client->type = UDP;
    strncpy(client->clientName, ctx->reqMsg.source, sizeof(client->clientName));
    strncpy(client->client.uClient.ip, inet_ntoa(ctx->client_sin.sin_addr),
            IP_LEN);
    client->client.uClient.port = ntohs(ctx->client_sin.sin_port);
    return 0;
}

/**
*********************************************************************
* @brief： event超时处理
* @param[in]： fd          事件描述符
               event      事件的类型及属性
               arg        回调参数
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void timeout_cb(evutil_socket_t fd, sint16 event, void *arg)
{
    struct event *timeout = arg;

    (void)event;
    (void)fd;
    if (!s_udpServer->stop)
    {
        struct timeval tv;
        evutil_timerclear(&tv);
        tv.tv_sec = UDP_TIMER_POLL_INTERVAL;
        //再次注册Timer
        event_add(timeout, &tv);
    }
    else
    {
        //停止libeven
        event_base_loopbreak(s_udpServer->base);
        olog_info(ocat, "udp server stop now");
    }
}

/**
*********************************************************************
* @brief： udp 接受数据回调
* @param[in]： sock        libevent监听描述符
               which       事件类型及属性
               arg         回调参数
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void udp_cb(const sint32 sock, sint16 which, void *arg)
{
    sint32             recvLen  = 0;
    sint32             rxBufLen = 0;
    struct sockaddr_in client_sin;
    socklen_t          server_sz    = sizeof(client_sin);
    MSG_HANDER_OBJ_T * msgHanderObj = NULL;
    UDP_CLIENT_CTX_T * ctx          = NULL;
#ifdef XL_TIME_TRACE
    struct timeval recvDone;
    struct timeval parseDone;
#endif

    (void)arg;
    (void)which;
    do
    {
        ctx = calloc(1, sizeof(UDP_CLIENT_CTX_T));
        if (NULL == ctx)
        {
            olog_error(ocat, "alloc UDP ctx fial");
            break;
        }

        recvLen = recvfrom(sock, ctx->reqRawObj.payload,
                           sizeof(ctx->reqRawObj.payload), 0,
                           (struct sockaddr *)&client_sin, &server_sz);
        if (recvLen < MIN_RAW_BUF_LEN)  //当收到的数据小于最小数据包时 丢弃
        {
            olog_error(ocat, "UDP recv packetlen:[%d] < MIN_RAW_BUF_LEN", recvLen);
            break;
        }
#ifdef XL_TIME_TRACE
        getSysTime(&recvDone);
#endif

        rxBufLen =
                pro_parser_msg(ctx->reqRawObj.payload, recvLen, &ctx->reqMsg);
        if (rxBufLen < 0)
        {
            olog_error(ocat, "pro_parser_msg fail");
            break;
        }
        else
        {
#ifdef XL_TIME_TRACE
            getSysTime(&parseDone);
#endif
            // holog_debug(ocat,ctx->reqRawObj.payload, rxBufLen);
            msgHanderObj =
                (MSG_HANDER_OBJ_T *)calloc(1, sizeof(MSG_HANDER_OBJ_T));
#ifdef XL_TIME_TRACE
            msgHanderObj->recvDone = recvDone;
            msgHanderObj->parseDone = parseDone;
#endif
            msgHanderObj->obj     = ctx;  //将CTX与msgHanderObj绑定
            ctx->reqRawObj.length = rxBufLen;
            ctx->client_sin       = client_sin;
            ctx->serverSock       = sock;
            msgHanderObj->getRspDataFp =
                udp_get_rsp_raw_buf;  //设置响应报文回调函数
            msgHanderObj->getReqDataFp =
                udp_get_req_raw_buf;  //设置请求报文获取的回调函数
            msgHanderObj->getReqDataMsgFp = udp_get_req_msg;
            msgHanderObj->getRspDataMsgFp = udp_get_rsp_msg;
            msgHanderObj->handerFp =
                udp_msg_handel_cb;  //在hander处理后必须释放 msgHanderObj
                                    // msgHanderObj->obj
            msgHanderObj->priority      = ctx->reqMsg.priority;
            msgHanderObj->getClientInfo = udp_get_client_info;
            if(0 != base_queue_message(msgHanderObj))
            {
                olog_error(ocat, "queue msg fail");
            }

        }

        return;
    } while (0);

    if (NULL != ctx)
    {
        free(ctx);
    }
}

/**
*********************************************************************
* @brief： udp服务进程
* @param[in]： arg         线程参数当前未使用
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void *udp_service(void *arg)
{
    struct sockaddr_in sin;
    struct event       timeout;
    struct event       udp_recv;
    struct timeval     tv;
    struct event_base *base;
    sint32             flags;

    flags = 0;
    (void)arg;
    //初始化 Libevent句柄
    base              = event_base_new();
    s_udpServer->base = base;

    //初始化 timeout 事件 event_assign
    event_assign(&timeout, base, -1, flags, timeout_cb, (void *)&timeout);
    evutil_timerclear(&tv);
    tv.tv_sec = UDP_TIMER_POLL_INTERVAL;
    event_add(&timeout, &tv);

    // udp 服务的socket初始化
    s_udpServer->serverSock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&sin, 0, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = inet_addr(s_udpServer->serverIp);  //绑定IP初始化
    sin.sin_port = htons(s_udpServer->serverPort);  //绑定端口初始化

    if (bind(s_udpServer->serverSock, (struct sockaddr *)&sin, sizeof(sin)))
    {
        olog_fatal(ocat, "UDP server Bind Port:[%d] fail.", s_udpServer->serverPort);
        exit(EXIT_FAILURE);  //端口绑定失败退出程序
    }
    else
    {
        olog_info(ocat, "UDP server bind on ip:[%s] port:[%u]",
                   s_udpServer->serverIp, s_udpServer->serverPort);
    }

    int nRecvBuf = 10 * 1024* 1024;  //设置为10M
    setsockopt(s_udpServer->serverSock, SOL_SOCKET, SO_RCVBUF, &nRecvBuf,
               sizeof(nRecvBuf));

    int nSendBuf = 10 * 1024 * 1024;  //设置为10M
    setsockopt(s_udpServer->serverSock, SOL_SOCKET, SO_SNDBUF, &nSendBuf,
               sizeof(nSendBuf));

    //初始化 udp receive 事件
    // olog_debug(ocat,"s_udpServer->serverSock = %d", s_udpServer->serverSock);
    event_assign(&udp_recv, base, s_udpServer->serverSock, EV_READ | EV_PERSIST,
                 udp_cb, NULL);
    event_add(&udp_recv, NULL);

    // event loop event 线程循环执行直到循环被取消
    event_base_dispatch(base);

    // event base对象需要被正确释放
    event_base_free(base);
    close(s_udpServer->serverSock);
    return (0);
}

/**
*********************************************************************
* @brief： udp通讯服务系统初始化
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 udp_init_system()
{
    sint32 ret = -1;

    ocat = olog_get_category("udpS");
    olog_info(ocat, "udp_init_system");
    s_udpServer = (UDP_SERVICE_T *)malloc(sizeof(UDP_SERVICE_T));
    if (s_udpServer != NULL)
    {
        memset(s_udpServer, 0, sizeof(UDP_SERVICE_T));
        // //从全局配置文件获取IP及端口的配置
        // strncpy(s_udpServer->serverIp, g_configObj.serviceCfg.udpServerIP,
        //         IP_LEN);
        // s_udpServer->serverPort = g_configObj.serviceCfg.udpServerPort;
        ret                     = 0;
    }
    return ret;
}

/**
*********************************************************************
* @brief： udp通讯服务清理系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void udp_clean_up_system()
{
    olog_info(ocat, "udp_clean_up_system");
}

/**
*********************************************************************
* @brief： udp通讯服务启动系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 udp_start_system()
{
    sint32 ret = -1;
    olog_info(ocat, "udp_start_system");
    //启动UDP线程
    ret = pthread_create(&(s_udpServer->tid), NULL, udp_service, NULL);
    return ret;
}

/**
*********************************************************************
* @brief： udp通讯服务停止系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void udp_stop_system()
{
    olog_info(ocat, "udp_stop_system");
}

/**
*********************************************************************
* @brief： udp消息发送
* @param[in]： ip
               port
               data
               dataLen
* @param[out]：none
* @return： 返回值说明   -1 发送失败
                        0 发送成功
*********************************************************************
*/
sint32 udp_sendto(sint8 ip[IP_LEN], uint16 port, void *data, uint32 dataLen)
{
    struct sockaddr_in client_sin = {0};
    sint32             sendLen    = 0;

    client_sin.sin_family      = AF_INET;
    client_sin.sin_addr.s_addr = inet_addr(ip);  //
    client_sin.sin_port        = htons(port);    //绑定端口初始化
    sendLen = sendto(s_udpServer->serverSock, data, dataLen, 0,
                     (struct sockaddr *)&client_sin, sizeof(client_sin));
    olog_debug(ocat, "send_to %s:%u sendLen:[%u]", ip, port, sendLen);
    if  (sendLen != -1) {
        sendLen = 0;
    }
    return sendLen;
}

/**
*********************************************************************
* @brief： 设置IP
* @param[in]： ip

* @param[out]：none
* @return： 返回值说明   NONE
*********************************************************************
*/
void set_udp_bind_ip(sint8 ip[IP_LEN])
{
    strncpy(s_udpServer->serverIp, ip, IP_LEN);
}

/**
*********************************************************************
* @brief： 设置端口
* @param[in]： port
* @param[out]：none
* @return： 返回值说明   none
*********************************************************************
*/
void set_udp_bind_port(uint16 port)
{
    s_udpServer->serverPort = port;
}