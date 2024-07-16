/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： tcpService.c
           实现tcp通讯接口
* @date： 2019-11-9
*********************************************************************
*/

#include <sys/types.h>

#include <arpa/inet.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#ifdef EVENT__HAVE_SYS_TIME_H
#    include <sys/time.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "event2/buffer.h"
#include "event2/buffer_compat.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/listener.h"

#include "baseType.h"
// #include "constant.h"
#include "msgServiceBase.h"
#include "socketHelper.h"
#include "tcpService.h"
#include "olog.h"

static TCP_SERVICE_T *s_tcpServer = NULL;

// olog_category_t *tcps = NULL;
static void *ocat = NULL;

// static sint8 s_tcpBindIP[IP_LEN] = {0};

// static uint16 s_tcpBindPort = 0;

//@brief 异步处理回调函数类型
typedef void (*SAFETY_CALL_FP_T)(void *obj);

typedef struct _tcp_cmd
{
    SAFETY_CALL_FP_T func;
    void *           obj;
} TCP_CMD_T;

static sint32 thread_safety_call(SAFETY_CALL_FP_T func, void *obj)
{
    TCP_CMD_T cmd = {0};

    cmd.func = func;
    cmd.obj  = obj;
    int ret  = write(s_tcpServer->notifySendFd, &cmd, sizeof(TCP_CMD_T));
    if (ret != sizeof(TCP_CMD_T))
    {
        olog_error(ocat, "thread_safety_call fail");
        return -1;
    }
    return 0;
}

static void i_disconnect_client(sint8 *ip, uint16 port)
{
    size_t        size   = 0;
    TCP_CLIENT_T *client = NULL;
    void *        data   = NULL;
    sint8         key[IP_LEN + PORT_LEN + 5];

    snprintf(key, sizeof(key), "%s:%u", ip, port);
    data =
        s_tcpServer->connectPool->get(s_tcpServer->connectPool, key, &size, 0);
    if (data != NULL)
    {
        memcpy(&client, data, size);
        s_tcpServer->connectPool->remove(s_tcpServer->connectPool, key);
        // olog_debug(ocat, "remove tcp client[%s] from pool", key);
        olog_info(ocat, "remove tcp client[%s] from pool", key);

        bufferevent_free(client->bufev);
        close(client->clientFd);
        free(client);
    }
}

/**
*********************************************************************
* @brief：                  获取tcp 传递给 msg层的原始数据
* @param[in]：
               obj          UdpClientCtx保留了从tcp接收到的数据
* @param[out]：
               rawBuffer    将UdpClientCtx赋值给msg层rawBuffer
* @return： 返回值说明
*********************************************************************
*/
static sint32 tcp_get_req_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    TCP_CLIENT_CTX_T *ctx = (TCP_CLIENT_CTX_T *)obj;
    //将tcp 收到的数据传址给MSG层
    *rawBuffer = &ctx->reqRawObj;
    return 0;
}

/**
*********************************************************************
* @brief：                  获取tcp 传递给 msg层的原始数据
* @param[in]：
               obj          UdpClientCtx保留了从tcp接收到的数据
* @param[out]：
               rawBuffer    将UdpClientCtx赋值给msg层rawBuffer
* @return： 返回值说明
*********************************************************************
*/
static sint32 tcp_get_req_msg(void *obj, MSG_OBJ_T **msg)
{
    TCP_CLIENT_CTX_T *ctx = (TCP_CLIENT_CTX_T *)obj;
    // //将tcp 收到的数据传址给MSG层
    *msg = &ctx->reqMsg;
    return 0;
}

static sint32 tcp_get_rsp_msg(void *obj, MSG_OBJ_T **msg)
{
    TCP_CLIENT_CTX_T *ctx = (TCP_CLIENT_CTX_T *)obj;
    // //将tcp 收到的数据传址给MSG层
    *msg = &ctx->rspMsg;
    return 0;
}

/**
*********************************************************************
* @brief： 设置tcp响应buf
* @param[in]：
               rawBuffer    tcp需要回复的消息
* @param[out]：
               obj          TCP_CLIENT_CTX_T
* @return： 返回值说明
*********************************************************************
*/
static sint32 tcp_get_rsp_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    TCP_CLIENT_CTX_T *ctx = (TCP_CLIENT_CTX_T *)obj;
    *rawBuffer            = &ctx->rspRawObj;
    return 0;
}

/**
*********************************************************************
* @brief： 获取客户端信息
* @param[in]：
               obj     tcp客户端信息
* @param[out]：
               client   
* @return： 返回值说明
*********************************************************************
*/
static sint32 tcp_get_client_info(void *obj, MSG_CLIENT_T *client)
{
    TCP_CLIENT_CTX_T *ctx = (TCP_CLIENT_CTX_T *)obj;

    client->type = TCP;
    strncpy(client->clientName, ctx->reqMsg.source, sizeof(client->clientName));
    strncpy(client->client.tClient.ip, ctx->clientIP, IP_LEN);
    client->client.tClient.port = ctx->clientPort;
    return 0;
}

static void tcp_msg_handel_thread_safety_cb(void *obj)
{
    sint8         name[15]                   = {0};
    size_t        size                       = 0;
    TCP_CLIENT_T *client                     = NULL;
    void *        data                       = NULL;
    sint8         key[IP_LEN + PORT_LEN + 5] = {0};

    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)obj;
    TCP_CLIENT_CTX_T *ctx          = (TCP_CLIENT_CTX_T *)msgHanderObj->obj;
    prctl(PR_GET_NAME, name);

    // olog_info(ocat, "===>call %s in %s", "tcp_msg_handel_thread_safety_cb",name);
    // bufferevent_write(ctx->tcpClientCtx->bufev,ctx->rspRawObj.payload,ctx->rspRawObj.length);

    snprintf(key, sizeof(key), "%s:%u", ctx->clientIP, ctx->clientPort);
    data =
        s_tcpServer->connectPool->get(s_tcpServer->connectPool, key, &size, 0);
    if (data != NULL)
    {
        memcpy(&client, data, size);

        bufferevent_write(client->bufev, ctx->rspRawObj.payload,
                          ctx->rspRawObj.length);
    }

    free(ctx);
    free(msgHanderObj);
}

static void tcp_sendto_safety_cb(void *obj)
{
    WRITE_BUFFER_EVENT_PARAM_T *param = (WRITE_BUFFER_EVENT_PARAM_T *)obj;
    bufferevent_write(param->bufev, param->data, param->dataLen);
    free(param->data);
    free(param);
}

static void tcp_disconnect_safety_cb(void *obj)
{
    DISCONNECT_CLIENT_PARAM_T *param = (DISCONNECT_CLIENT_PARAM_T *)obj;
    i_disconnect_client(param->ip, param->port);
    free(param);
}

/**
*********************************************************************
* @brief： tcp服务消息处理
* @param[in]： obj     TCP_CLIENT_CTX_T
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static sint32 tcp_msg_handel_cb(void *obj)
{
    sint8 name[15];
    prctl(PR_GET_NAME, name);
    olog_info(ocat, "===>call %s in %s", "tcp_msg_handel_cb", name);
    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)obj;
    TCP_CLIENT_CTX_T *ctx          = (TCP_CLIENT_CTX_T *)msgHanderObj->obj;

    if (ctx->rspRawObj.length > 0)
    {
        thread_safety_call(tcp_msg_handel_thread_safety_cb, msgHanderObj);
    }
    else
    {
        free(ctx);
        free(msgHanderObj);
    }
    return 0;
}

/**
*********************************************************************
* @brief： tcp服务进程
* @param[in]： arg         线程参数当前未使用
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void *tcp_service(void *arg)
{
    UNUSED(arg);
    prctl(PR_SET_NAME, TCP_SERVER_NAME, 0, 0, 0);
    // event loop event 线程循环执行直到循环被取消
    event_base_dispatch(s_tcpServer->base);

    // event base对象需要被正确释放
    event_base_free(s_tcpServer->base);
    close(s_tcpServer->listenFd);
    return (0);
}

static void on_pipe_recv_event(int fd, short ev, void *arg)
{
    int       recvlen = 0;
    TCP_CMD_T cmd     = {0};

    UNUSED(ev);
    UNUSED(arg);
    recvlen = read(fd, &cmd, sizeof(TCP_CMD_T));
    if (recvlen == sizeof(TCP_CMD_T))
    {
        cmd.func(cmd.obj);
    }
}

static void client_buffered_on_read(struct bufferevent *bev, void *arg)
{
    struct evbuffer * input        = NULL;
    uint8 *           rxBuf        = NULL;
    MSG_HANDER_OBJ_T *msgHanderObj = NULL;
    TCP_CLIENT_CTX_T *ctx          = NULL;
    TCP_CLIENT_T *    client       = (TCP_CLIENT_T *)arg;
    ev_ssize_t        ebTotallen   = 0;
    sint32            rxBufLen     = 0;
    sint32            cpBufLen     = 0;
    input                          = bufferevent_get_input(bev);

#if 0
    do
    {

        ctx = (TCP_CLIENT_CTX_T *)calloc(1, sizeof(TCP_CLIENT_CTX_T));
        if (ctx == NULL)
        {
            olog_warn(ocat, "TCP_CLIENT_CTX_T calloc fail");
            break;
        }

        ebTotallen = EVBUFFER_LENGTH(input);
        if (ebTotallen < MIN_RAW_BUF_LEN)
        {
            olog_warn(ocat, "tcp recv data[%zu] < MIN_RAW_BUF_LEN[%d]", ebTotallen,
                       MIN_RAW_BUF_LEN);
            break;
        }

        olog_debug(ocat, "recv ebTotallen:[%zu] from [%s]:[%u]", ebTotallen,
                    client->clientIP, client->clientPort);

        rxBuf = (uint8 *)EVBUFFER_DATA(input);
        memcpy(ctx->reqRawObj.payload, rxBuf,
               ebTotallen > MAX_MSG_LEN ? MAX_MSG_LEN : ebTotallen);
        rxBufLen =
            pro_parser_msg(ctx->reqRawObj.payload, ebTotallen, &ctx->reqMsg);
        if (rxBufLen < 0)
        {
            olog_error(ocat, "pro_parser_msg fail");
            break;
        }
        else
        {
            holog_debug(ocat, rxBuf, rxBufLen);
            msgHanderObj =
                (MSG_HANDER_OBJ_T *)calloc(1, sizeof(MSG_HANDER_OBJ_T));

            msgHanderObj->obj = ctx;  //将CTX与msgHanderObj绑定
            strncpy(ctx->clientIP, client->clientIP, sizeof(ctx->clientIP));
            ctx->clientPort       = client->clientPort;
            ctx->reqRawObj.length = rxBufLen;
            msgHanderObj->getRspDataFp =
                tcp_get_rsp_raw_buf;  //设置响应报文回调函数
            msgHanderObj->getReqDataFp =
                tcp_get_req_raw_buf;  //设置请求报文获取的回调函数
            msgHanderObj->getReqDataMsgFp = tcp_get_req_msg;
            msgHanderObj->getRspDataMsgFp = tcp_get_rsp_msg;
            msgHanderObj->handerFp =
                tcp_msg_handel_cb;  //在hander处理后必须释放 msgHanderObj
                                    // msgHanderObj->obj
            msgHanderObj->priority      = ctx->reqMsg.priority;
            msgHanderObj->getClientInfo = tcp_get_client_info;
            base_queue_message(msgHanderObj);

            evbuffer_drain(input, rxBufLen);

            ebTotallen = EVBUFFER_LENGTH(input);
            if (ebTotallen > MIN_RAW_BUF_LEN)
            {
            }
            ctx = NULL;
        }
        printf("Drained %lu\n", (unsigned long)ebTotallen);

    } while (0);

    if (ctx != NULL)
    {
        free(ctx);
    }
#else

    do
    {
        ebTotallen = EVBUFFER_LENGTH(input);
        if (ebTotallen < MIN_RAW_BUF_LEN)
        {
            olog_warn(ocat, "tcp recv data[%zu] < MIN_RAW_BUF_LEN[%d]", ebTotallen,
                       MIN_RAW_BUF_LEN);
            break;
        }

        ctx = (TCP_CLIENT_CTX_T *)calloc(1, sizeof(TCP_CLIENT_CTX_T));
        if (ctx == NULL)
        {
            olog_warn(ocat, "TCP_CLIENT_CTX_T calloc fail");
            break;
        }

        olog_debug(ocat, "recv ebTotallen:[%zu] from [%s]:[%u]", ebTotallen,
                    client->clientIP, client->clientPort);

        rxBuf = (uint8 *)EVBUFFER_DATA(input);

        cpBufLen = ebTotallen > MAX_MSG_LEN ? MAX_MSG_LEN : ebTotallen;
        memcpy(ctx->reqRawObj.payload, rxBuf, cpBufLen);
        rxBufLen =
            pro_parser_msg(ctx->reqRawObj.payload, cpBufLen, &ctx->reqMsg);
        olog_info(ocat, "tcp rxBufLen =%d", rxBufLen);

        if (rxBufLen < 0)
        {
            olog_error(ocat, "pro_parser_msg fail");
            free(ctx);
            break;
        }
        else
        {
            //
            if (ctx->reqMsg.tag == 0x11111111)
            {
                static uint32 iii = 0;
                olog_info(ocat, "tcp 0x11111111 =%d", ++iii);
            }

            if (rxBufLen < 259)
            {
                olog_info(ocat, "tcp ebTotallen =%zd", ebTotallen);

                holog_info(ocat, ctx->reqRawObj.payload, rxBufLen);
            }

            holog_debug(ocat, rxBuf, rxBufLen);
            msgHanderObj =
                (MSG_HANDER_OBJ_T *)calloc(1, sizeof(MSG_HANDER_OBJ_T));

            if (msgHanderObj == NULL)
            {
                olog_warn(ocat, "TCP_CLIENT_CTX_T calloc msgHanderObj fail");
                free(ctx);
                break;
            }

            msgHanderObj->obj = ctx;  //将CTX与msgHanderObj绑定
            strncpy(ctx->clientIP, client->clientIP, sizeof(ctx->clientIP));
            ctx->clientPort       = client->clientPort;
            ctx->reqRawObj.length = rxBufLen;
            msgHanderObj->getRspDataFp =
                tcp_get_rsp_raw_buf;  //设置响应报文回调函数
            msgHanderObj->getReqDataFp =
                tcp_get_req_raw_buf;  //设置请求报文获取的回调函数
            msgHanderObj->getReqDataMsgFp = tcp_get_req_msg;
            msgHanderObj->getRspDataMsgFp = tcp_get_rsp_msg;
            msgHanderObj->handerFp =
                tcp_msg_handel_cb;  //在hander处理后必须释放 msgHanderObj
                                    // msgHanderObj->obj
            msgHanderObj->priority      = ctx->reqMsg.priority;
            msgHanderObj->getClientInfo = tcp_get_client_info;
            base_queue_message(msgHanderObj);

            evbuffer_drain(input, rxBufLen);

            ebTotallen = EVBUFFER_LENGTH(input);
            if (ebTotallen > MIN_RAW_BUF_LEN)
            {
                olog_debug(ocat, "######## recv ebTotallen:[%zu]", ebTotallen);
                continue;
            }
        }
        printf("Drained %lu\n", (unsigned long)ebTotallen);

    } while (1);

#endif
}

static void client_buffered_on_write(struct bufferevent *bev, void *arg)
{
    (void)bev;
    UNUSED(arg);
    olog_debug(ocat, "buffered_on_write");
}

static void
client_buffered_on_error(struct bufferevent *bev, short what, void *arg)
{
    TCP_CLIENT_T *client = (TCP_CLIENT_T *)arg;

    UNUSED(bev);
    if (what & BEV_EVENT_EOF)
    {
        /* Client disconnected, remove the read event and the
         * free the client structure. */
        olog_info(ocat, "Client disconnected [%s]:[%u].", client->clientIP,
                   client->clientPort);
    }
    else
    {
        olog_warn(ocat, "Client socket error, disconnecting [%s]:[%u].",
                   client->clientIP, client->clientPort);
    }
    i_disconnect_client(client->clientIP, client->clientPort);
    // bufferevent_free(client->bufev);
    // close(client->clientFd);
    // free(client);
}

static void on_accept(int fd, short ev, void *arg)
{
    int                clientFd = -1;
    TCP_CLIENT_T *     client   = NULL;
    struct sockaddr_in client_addr;
    sint8              key[IP_LEN + PORT_LEN + 5];
    sint32             ret        = -1;
    socklen_t          client_len = sizeof(client_addr);

    (void)ev;
    (void)arg;
    do
    {
        clientFd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
        if (clientFd < 0)
        {
            olog_warn(ocat, "accept failed fd[%d]  error:%s", fd, strerror(errno));
            break;
        }

        /* Set the client socket to non-blocking mode. */
        if (setnonblock(clientFd) < 0)
        {
            olog_warn(ocat, "failed to set client socket non-blocking error:%s",
                       strerror(errno));
            break;
        }

        int enable = 1;
        if (setsockopt(clientFd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable,
                       sizeof(enable)) < 0)
        {
            printf("ERROR: TCP_NODELAY SETTING ERROR!\n");
            break;
        }

        /* We've accepted a new client, create a client object. */
        client = calloc(1, sizeof(*client));
        if (client == NULL)
        {
            olog_error(ocat, "malloc client failed error:%s", strerror(errno));
            break;
        }

        client->clientFd = clientFd;
        client->bufev = bufferevent_socket_new(s_tcpServer->base, clientFd, 0);
        if (client->bufev == NULL)
        {
            olog_error(ocat, "bufferevent_socket_new tcp fail");
            break;
        }

        bufferevent_setcb(client->bufev, client_buffered_on_read,
                          client_buffered_on_write, client_buffered_on_error,
                          client);
        bufferevent_enable(client->bufev, EV_READ);
        bufferevent_setwatermark(client->bufev, EV_READ, MIN_RAW_BUF_LEN, 0);

        strncpy(client->clientIP, inet_ntoa(client_addr.sin_addr), IP_LEN);
        client->clientPort = ntohs(client_addr.sin_port);
        olog_info(ocat, "Accepted connection from [%s]:[%d]", client->clientIP,
                   client->clientPort);

        snprintf(key, sizeof(key), "%s:%u", client->clientIP,
                 client->clientPort);
        ret = s_tcpServer->connectPool->put(s_tcpServer->connectPool, key,
                                            &client, sizeof(client));
        if (ret != 0)
        {
            olog_debug(ocat, "%s put tcp connect pool", key);
        }
        else
        {
            /* 内存不足反向释放所有数据 */
        }

        return;
    } while (0);

    if (client != NULL)
    {
        free(client);
    }

    if (clientFd != -1)
    {
        close(clientFd);
    }
}

static sint32 tcp_sock_init(void)
{
    int                ret = -1;
    int                reuseaddr;
    int                opt         = 1;
    struct sockaddr_in listen_addr = {0};

    do
    {
        s_tcpServer->listenFd = socket(AF_INET, SOCK_STREAM, 0);
        if (s_tcpServer->listenFd < 0)
        {
            olog_error(ocat, "create tcp socket fial");
            break;
        }
        olog_info(ocat, "s_tcpServer->listenFd  = %d", s_tcpServer->listenFd);

        memset(&listen_addr, 0, sizeof(listen_addr));
        listen_addr.sin_family      = AF_INET;
        listen_addr.sin_addr.s_addr = inet_addr(s_tcpServer->serverIp);
        listen_addr.sin_port        = htons(s_tcpServer->serverPort);

        // sockfd为需要端口复用的套接字
        if (setsockopt(s_tcpServer->listenFd, SOL_SOCKET, SO_REUSEADDR,
                       (const void *)&opt, sizeof(opt)) < 0)
        {
            olog_error(ocat, "setsockopt SO_REUSEADDR  s_tcpServer->listenFd fial");
            break;
        }

        if (bind(s_tcpServer->listenFd, (struct sockaddr *)&listen_addr,
                 sizeof(listen_addr)) < 0)
        {
            olog_error(ocat, "bind socket:[%d] fial ip:[%s] port:[%d]",
                        s_tcpServer->listenFd, s_tcpServer->serverIp,
                        s_tcpServer->serverPort);
            break;
        }
        else
        {
            olog_info(ocat, "TCP server bind on ip:[%s] port:[%u]",
                       s_tcpServer->serverIp, s_tcpServer->serverPort);
        }

        if (setsockopt(s_tcpServer->listenFd, SOL_SOCKET, SO_REUSEADDR,
                       &reuseaddr, sizeof(reuseaddr)) < 0)
        {
            olog_error(ocat, "setsockopt s_tcpServer->listenFd fial");
            break;
        }

        if (listen(s_tcpServer->listenFd, TCP_LISTEN_NUM) < 0)
        {
            olog_error(ocat, "listen failed");
            break;
        }

        if (setnonblock(s_tcpServer->listenFd) < 0)
        {
            olog_error(ocat, "setnonblock socket:[%d] fial", s_tcpServer->listenFd);
            break;
        }

        s_tcpServer->connectPool = qhashtbl(0, QHASHTBL_THREADSAFE);
        if (NULL == s_tcpServer->connectPool)
        {
            olog_error(ocat, "s_tcpServer->connectPool init fial");
            break;
        }

        ret = 0;

    } while (0);

    return ret;
}

static sint32 libevent_init(void)
{
    int                ret  = -1;
    struct event_base *base = NULL;

    do
    {
        //初始化 Libevent句柄
        base = event_base_new();
        if (base == NULL)
        {
            olog_error(ocat, "event_base_new fail");
            break;
        }

        s_tcpServer->base = base;

        //初始化 tcp accept
        if (0 != event_assign(&s_tcpServer->eventAccept, base,
                              s_tcpServer->listenFd, EV_READ | EV_PERSIST,
                              on_accept, NULL))
        {
            olog_error(ocat, "event_assign fail");
            break;
        }

        if (0 != event_add(&s_tcpServer->eventAccept, NULL))
        {
            olog_error(ocat, "event_add fail");
            break;
        }

        if (0 != event_assign(&s_tcpServer->notify_event, base,
                              s_tcpServer->notifyReceiveFd,
                              EV_READ | EV_PERSIST, on_pipe_recv_event, NULL))
        {
            olog_error(ocat, "event_assign fail");
            break;
        }

        if (0 != event_add(&s_tcpServer->notify_event, NULL))
        {
            olog_error(ocat, "event_add fail");
            break;
        }

        ret = 0;
    } while (0);

    return ret;
}

static sint32 init_pipe(void)
{
    int fds[2];
    if (0 != pipe(fds))
    {
        printf("Can't create notify pipe");
        return -1;
    }
    s_tcpServer->notifyReceiveFd = fds[0];
    s_tcpServer->notifySendFd    = fds[1];
    return 0;
}
/**
*********************************************************************
* @brief： tcp通讯服务系统初始化
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 tcp_init_system(void)
{
    sint32 ret = -1;

    do
    {
        ocat = olog_get_category("tcpS");

        olog_info(ocat, "tcp_init_system");
        s_tcpServer = (TCP_SERVICE_T *)calloc(1, sizeof(TCP_SERVICE_T));
        if (s_tcpServer == NULL)
        {
            olog_error(ocat, "malloc TCP_SERVICE_T fail");
            break;
        }

        // memset(s_tcpServer, 0, sizeof(TCP_SERVICE_T));
        // //从全局配置文件获取IP及端口的配置
        // strncpy(s_tcpServer->serverIp, g_configObj.serviceCfg.tcpServerIP,
        //         IP_LEN);
        // s_tcpServer->serverPort = g_configObj.serviceCfg.tcpServerPort;



        ret = 0;
    } while (0);

    return ret;
}

/**
*********************************************************************
* @brief： tcp通讯服务清理系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void tcp_clean_up_system()
{
    olog_info(ocat, "tcp_clean_up_system");
}

/**
*********************************************************************
* @brief： tcp通讯服务启动系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 tcp_start_system()
{
    sint32 ret = -1;
    olog_info(ocat, "tcp_start_system");

    do
    {
        if (0 != init_pipe())
        {
            olog_error(ocat, "tcp server init pipe file");
            break;
        }

        if (0 != tcp_sock_init())
        {
            olog_error(ocat, "tcp_sock_init fail");
            break;
        }

        if (0 != libevent_init())
        {
            olog_error(ocat, "libevent_init fail");
            break;
        }

        //启动TCP线程
        if (0 != pthread_create(&(s_tcpServer->tid), NULL, tcp_service, NULL))
        {
            olog_error(ocat, "pthread_create s_tcpServer fail");
            break;
        }
        ret = 0;

    } while (0);

    return ret;
}

/**
*********************************************************************
* @brief： tcp通讯服务停止系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void tcp_stop_system()
{
    olog_info(ocat, "tcp_stop_system");
}

sint32 tcp_sendto(sint8 ip[IP_LEN], uint16 port, void *data, uint32 dataLen)
{
    size_t                      size   = 0;
    TCP_CLIENT_T *              client = NULL;
    void *                      val    = NULL;
    sint8                       key[IP_LEN + PORT_LEN + 5];
    WRITE_BUFFER_EVENT_PARAM_T *param = NULL;

    snprintf(key, sizeof(key), "%s:%u", ip, port);
    val =
        s_tcpServer->connectPool->get(s_tcpServer->connectPool, key, &size, 0);
    if (val != NULL)
    {
        memcpy(&client, val, size);

        // bufferevent_write(client->bufev,data,dataLen);
        param = calloc(1, sizeof(WRITE_BUFFER_EVENT_PARAM_T));
        if (NULL != param)
        {
            param->bufev = client->bufev;
            param->data  = calloc(1, dataLen);
            if (NULL != param->data)
            {
                memcpy(param->data, data, dataLen);
                param->dataLen = dataLen;
                thread_safety_call(tcp_sendto_safety_cb, param);
            }
            else
            {
                free(param);
            }
        }
        else
        {
            olog_debug(ocat, "calloc tcp_sendto cmd fail");
        }
    }

    return 0;
}

void tcp_disconnect_client(sint8 ip[IP_LEN], uint16 port)
{
    DISCONNECT_CLIENT_PARAM_T *param = NULL;

    param       = calloc(1, sizeof(DISCONNECT_CLIENT_PARAM_T));
    param->port = port;
    strncpy(param->ip, ip, sizeof(param->ip));
    thread_safety_call(tcp_disconnect_safety_cb, param);
}

void set_tcp_bind_ip(sint8 ip[IP_LEN])
{
    strncpy(s_tcpServer->serverIp, ip, IP_LEN);
}

void set_tcp_bind_port(uint16 port)
{
    s_tcpServer->serverPort = port;
}