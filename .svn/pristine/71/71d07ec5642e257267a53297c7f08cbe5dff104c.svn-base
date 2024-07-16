#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "event2/buffer.h"
#include "event2/buffer_compat.h"
#include "event2/bufferevent.h"
#include "event2/event.h"
#include "event2/listener.h"

#include "baseConstant.h"
#include "baseType.h"
#include "constant.h"
#include "dbCenterConfig.h"
#include "globalConfiguration.h"
#include "mqttService.h"
#include "msgService.h"
#include "msgServiceBase.h"
#include "smiOSClient.h"
#include "threadpool.h"
#include "timeHelper.h"
#include "udpService.h"
#include "olog.h"

//smios连接信息
static SMIOS_CONNECT_T s_smios_connect;

//tcp信息处理回调函数
static sint32 tcp_msg_handel_cb(void *obj);
//日志输出
extern void *     db_msg_ocat ;

//@brief 异步处理回调函数类型
typedef void (*SAFETY_CALL_FP_T)(void *obj);

//TCP消息处理
typedef struct _tcp_cmd
{
    SAFETY_CALL_FP_T func;
    void *           obj;
} TCP_CMD_T;

typedef struct _smiOS_ctx
{
    MSG_OBJ_T reqMsg;            //请求消息头
    RAW_OBJ_T reqRawObj;         //请求信息对象
    MSG_OBJ_T rspMsg;            //回复消息头
    RAW_OBJ_T rspRawObj;         //回复信息对象
    sint8     clientIP[IP_LEN];  //消息发送者IP
    uint16    clientPort;        //消息发送者端口

} SMIOS_CTX_T;

/* @brief:异步线程写buffer_event参数 */
typedef struct
{
    struct bufferevent *bufev;    // buffer_event对象
    void *              data;     //需要发送的数据
    uint32              dataLen;  //发送数据的长度
} SMIOS_WRITE_BUFFER_EVENT_PARAM_T;

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
    SMIOS_CTX_T *ctx = (SMIOS_CTX_T *)obj;
    //将tcp 收到的数据传址给MSG层
    *rawBuffer = &ctx->reqRawObj;
    return 0;
}

/**
*********************************************************************
* @brief：                  获取tcp 传递给 msg层的原始数据
* @param[in]：
               obj          SMIOS_CTX_T保留了从tcp接收到的数据
* @param[out]：
               msg          保存请求信息
* @return： 返回值说明
*********************************************************************
*/
static sint32 tcp_get_req_msg(void *obj, MSG_OBJ_T **msg)
{
    SMIOS_CTX_T *ctx = (SMIOS_CTX_T *)obj;
    // //将tcp 收到的数据传址给MSG层
    *msg = &ctx->reqMsg;
    return 0;
}

static sint32 tcp_get_rsp_msg(void *obj, MSG_OBJ_T **msg)
{
    SMIOS_CTX_T *ctx = (SMIOS_CTX_T *)obj;
    // //将tcp 收到的数据传址给MSG层
    *msg = &ctx->rspMsg;
    return 0;
}

/**
*********************************************************************
* @brief： 设置tcp响应buf
* @param[in]：
               obj          SMIOS_CTX_T
* @param[out]：
               rawBuffer    tcp需要回复的消息buffer
* @return： 返回值说明
*********************************************************************
*/
static sint32 tcp_get_rsp_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    SMIOS_CTX_T *ctx = (SMIOS_CTX_T *)obj;
    *rawBuffer       = &ctx->rspRawObj;
    return 0;
}

/**
*********************************************************************
* @brief： 获取客户端信息
* @param[in]：
               obj          SMIOS_CTX_T
* @param[out]：
               client       客户端信息
* @return： 返回值说明 0
*********************************************************************
*/
static sint32 tcp_get_client_info(void *obj, MSG_CLIENT_T *client)
{
    SMIOS_CTX_T *ctx = (SMIOS_CTX_T *)obj;

    client->type = TCP;
    strncpy(client->clientName, ctx->reqMsg.source, sizeof(client->clientName));
    strncpy(client->client.tClient.ip, ctx->clientIP, IP_LEN);
    client->client.tClient.port = ctx->clientPort;
    return 0;
}

/**
*********************************************************************
* @brief： 客户端读buffer
* @param[in]：
               bev          the bufferevent from which to get the evbuffer
               arg          unused
* @param[out]：

* @return： 返回值说明 0
*********************************************************************
*/
int client_buffered_on_read(struct bufferevent *bev, void *arg)
{
    struct evbuffer * input        = NULL;
    uint8 *           rxBuf        = NULL;
    SMIOS_CTX_T *     ctx          = NULL;
    MSG_HANDER_OBJ_T *msgHanderObj = NULL;
    ev_ssize_t        ebTotallen   = 0;
    sint32            rxBufLen     = 0;

    do
    {
        input = bufferevent_get_input(bev);

        ctx = (SMIOS_CTX_T *)calloc(1, sizeof(SMIOS_CTX_T));
        if (ctx == NULL)
        {
            olog_warn(db_msg_ocat,"TCP_CLIENT_CTX_T calloc fail");
            break;
        }

        ebTotallen = EVBUFFER_LENGTH(input);
        if (ebTotallen < MIN_RAW_BUF_LEN)
        {
            olog_warn(db_msg_ocat,"tcp recv data[%zu] < MIN_RAW_BUF_LEN[%d]", ebTotallen,
                       MIN_RAW_BUF_LEN);
            break;
        }

        // dzlog_debug("recv ebTotallen:[%zu] from [%s]:[%u]", ebTotallen,
        //             s_smios_connect.smiosIP, s_smios_connect.smiosPort);

        rxBuf = (uint8 *)EVBUFFER_DATA(input);
        memcpy(ctx->reqRawObj.payload, rxBuf,
               ebTotallen > MAX_MSG_LEN ? MAX_MSG_LEN : ebTotallen);
        rxBufLen =
            pro_parser_msg(ctx->reqRawObj.payload, ebTotallen, &ctx->reqMsg);
        if (rxBufLen < 0)
        {
            olog_error(db_msg_ocat,"pro_parser_msg fail");
            break;
        }
        else
        {
            // hdzlog_debug(rxBuf, rxBufLen);
            msgHanderObj =
                (MSG_HANDER_OBJ_T *)calloc(1, sizeof(MSG_HANDER_OBJ_T));

            msgHanderObj->obj = ctx;  //将CTX与msgHanderObj绑定
            // strncpy(ctx->clientIP, client->clientIP, sizeof(ctx->clientIP));
            // ctx->clientPort       = client->clientPort;
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
        }
        // printf("Drained %lu\n", (unsigned long)ebTotallen);
    } while (0);

    if (ctx != NULL)
    {
        free(ctx);
    }

    return 0;
}

static void client_buffered_on_write(struct bufferevent *bev, void *arg)
{
    (void)bev;
    UNUSED(arg);
    // dzlog_debug("buffered_on_write");
}

static void
client_buffered_on_error(struct bufferevent *bev, short what, void *arg)
{
    UNUSED(bev);
    if (what & BEV_EVENT_EOF)
    {
        /* Client disconnected, remove the read event and the
         * free the client structure. */
        olog_info(db_msg_ocat,"disconnected [%s]:[%u].", s_smios_connect.smiosIP,
                   s_smios_connect.smiosPort);
    }
    else
    {
        olog_warn(db_msg_ocat,"socket error what[0x%x], disconnecting [%s]:[%u].",
                   what,s_smios_connect.smiosIP, s_smios_connect.smiosPort);
    }
    // i_disconnect_client(client->clientIP, client->clientPort);
    // bufferevent_free(client->bufev);
    // close(client->clientFd);
    // free(client);
}

static sint32 thread_safety_call(SAFETY_CALL_FP_T func, void *obj)
{
    TCP_CMD_T cmd = {0};

    cmd.func = func;
    cmd.obj  = obj;
    int ret  = write(s_smios_connect.notifySendFd, &cmd, sizeof(TCP_CMD_T));
    if (ret != sizeof(TCP_CMD_T))
    {
        olog_error(db_msg_ocat,"thread_safety_call fail");
        return -1;
    }
    return 0;
}

static void tcp_msg_handel_thread_safety_cb(void *obj)
{
    //sint8  name[15] = {0};
    //size_t size     = 0;

    //void *data                       = NULL;
    //sint8 key[IP_LEN + PORT_LEN + 5] = {0};

    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)obj;
    SMIOS_CTX_T *     ctx          = (SMIOS_CTX_T *)msgHanderObj->obj;
    // prctl(PR_GET_NAME, name);

    // dzlog_info("===>call %s in %s", "tcp_msg_handel_thread_safety_cb",name);
    // bufferevent_write(ctx->tcpClientCtx->bufev,ctx->rspRawObj.payload,ctx->rspRawObj.length);

    // snprintf(key, sizeof(key), "%s:%u", ctx->clientIP, ctx->clientPort);
    // data =
    //     s_tcpServer->connectPool->get(s_tcpServer->connectPool, key, &size,
    //     0);
    // if (data != NULL)
    // {
    // memcpy(&client, data, size);
    bufferevent_write(s_smios_connect.bev, ctx->rspRawObj.payload,
                      ctx->rspRawObj.length);
    // }

    free(ctx);
    free(msgHanderObj);
}

/**
*********************************************************************
* @brief： tcp服务消息处理
* @param[in]： obj     TCP_CLIENT_CTX_T
* @param[out]：none
* @return： 返回值说明 0
*********************************************************************
*/
static sint32 tcp_msg_handel_cb(void *obj)
{
    //sint8 name[15];
    // dzlog_debug("tcp_msg_handel_cb");
    // prctl(PR_GET_NAME, name);
    // dzlog_info("===>call %s in %s", "tcp_msg_handel_cb", name);
    // MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)obj;

    // thread_safety_call(tcp_msg_handel_thread_safety_cb, msgHanderObj);
    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)obj;

    thread_safety_call(tcp_msg_handel_thread_safety_cb, msgHanderObj);
    return 0;
}

static void tcp_sendto_safety_cb(void *obj)
{
    SMIOS_WRITE_BUFFER_EVENT_PARAM_T *param =
        (SMIOS_WRITE_BUFFER_EVENT_PARAM_T *)obj;
    if (param->bufev)
    {
        bufferevent_write(param->bufev, param->data, param->dataLen);
    }

    free(param->data);
    free(param);
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

static void* smiOS_tcp_thread(void *p)
{
    evutil_socket_t    fd;
    struct event_base *base;
    struct sockaddr_in s_addr;
    // struct bufferevent *bev = NULL;

    base = event_base_new();
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family      = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(s_smios_connect.smiosIP);
    s_addr.sin_port        = htons(s_smios_connect.smiosPort);
    fd                     = socket(AF_INET, SOCK_STREAM, 0);
    s_smios_connect.bev    = bufferevent_socket_new(
        base, fd, BEV_OPT_CLOSE_ON_FREE /*| BEV_OPT_THREADSAFE*/);

    int enable = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable,
                   sizeof(enable)) < 0)
        printf("ERROR: TCP_NODELAY SETTING ERROR!\n");

    bufferevent_enable(s_smios_connect.bev, EV_WRITE);
    if (bufferevent_socket_connect(s_smios_connect.bev,
                                   (struct sockaddr *)&s_addr,
                                   sizeof(s_addr)) == 0)
    {
        olog_info(db_msg_ocat,"connect success\n");
    }
    else
    {
        olog_error(db_msg_ocat,"connect Fail\n");
    }

    bufferevent_setcb(s_smios_connect.bev, (bufferevent_data_cb)client_buffered_on_read,
                      (bufferevent_data_cb)client_buffered_on_write, (bufferevent_event_cb)client_buffered_on_error, NULL);
    bufferevent_enable(s_smios_connect.bev, EV_READ);
    bufferevent_setwatermark(s_smios_connect.bev, EV_READ, MIN_RAW_BUF_LEN, 0);

    if (0 != event_assign(&s_smios_connect.notify_event, base,
                          s_smios_connect.notifyReceiveFd, EV_READ | EV_PERSIST,
                          on_pipe_recv_event, NULL))
    {
        olog_error(db_msg_ocat,"event_assign fail");
    }

    if (0 != event_add(&s_smios_connect.notify_event, NULL))
    {
        olog_error(db_msg_ocat,"event_add fail");
    }

    // // start to send data
    // bufferevent_write(bev, mesg, length);
    // check the output evbuffer
    // struct evbuffer *output = bufferevent_get_output(bev);
    // int              len    = 0;
    // len                     = evbuffer_get_length(output);
    // printf("output buffer has %d bytes left\n", len);

    event_base_dispatch(base);
    bufferevent_free(s_smios_connect.bev);
    event_base_free(base);

    printf("Client program is over\n");
    return NULL;
}

static sint32 init_pipe(void)
{
    int fds[2];
    if (0 != pipe(fds))
    {
        printf("Can't create notify pipe");
        return -1;
    }
    s_smios_connect.notifyReceiveFd = fds[0];
    s_smios_connect.notifySendFd    = fds[1];
    return 0;
}

/**
*********************************************************************
* @brief：   初始化smiOS客户端线程
* @param[in]： type    消息类型
               ip      ip地址
               port    端口号
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 init_smiOS_connect(MSG_TYPE_E type, sint8 *ip, uint16 port)
{
    sint32 ret = 0;

    s_smios_connect.type      = type;
    s_smios_connect.smiosPort = port;
    strncpy(s_smios_connect.smiosIP, ip, IP_LEN);
    switch (type)
    {
        case UDP:
            break;
        case MQTT:
            break;
        case TCP:
            init_pipe();
            ret = pthread_create(&(s_smios_connect.tid), NULL, smiOS_tcp_thread,
                                 NULL);
            break;
        default:
            olog_error(db_msg_ocat,"not support connect type :[%s]", str_msg_type(type));
            break;
    }
    return ret;
}

void tcp_send(void *data, uint32 dataLen)
{
    SMIOS_WRITE_BUFFER_EVENT_PARAM_T *param = NULL;

    param = calloc(1, sizeof(SMIOS_WRITE_BUFFER_EVENT_PARAM_T));
    if (NULL != param)
    {
        param->bufev = s_smios_connect.bev;
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
        olog_debug(db_msg_ocat,"calloc tcp_sendto cmd fail");
    }
}

/**
*********************************************************************
* @brief：   发送数据至smiOS请求
* @param[in]：data     数据缓冲区
              dataLen  数据缓冲区长度
* @param[out]：none
* @return： 返回值说明 返回0成功 -1失败
*********************************************************************
*/
sint32 sendto_smiOS(sint8 *data, uint32 dataLen)
{
    switch (s_smios_connect.type)
    {
        case UDP:
            udp_sendto(s_smios_connect.smiosIP, s_smios_connect.smiosPort, data,
                       dataLen);
            break;
        case MQTT:
            mqtt_sendto("M-smiOS", data, dataLen);
            break;
        case TCP:
            // xl_channel_write(s_msgService->ctx, data, dataLen);
            tcp_send(data, dataLen);
            break;
        default:
            olog_error(db_msg_ocat,"not support connect type :[%s]",
                        str_msg_type(s_smios_connect.type));
            break;
    }

    return 0;
}
