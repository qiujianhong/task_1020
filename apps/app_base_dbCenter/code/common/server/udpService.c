/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� udpService.c
           ʵ��udpͨѶ�ӿ�
* @date�� 2019-11-9
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

//udp���������
static UDP_SERVICE_T *s_udpServer = NULL;
//��־���
static void *         ocat        = NULL;

/**
*********************************************************************
* @brief��                  ��ȡudp ���ݸ� msg���ԭʼ����
* @param[in]��
               obj          UdpClientCtx�����˴�udp���յ�������
* @param[out]��
               rawBuffer    ��UdpClientCtx��ֵ��msg��rawBuffer
* @return�� ����ֵ˵��
*********************************************************************
*/
static sint32 udp_get_req_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    UDP_CLIENT_CTX_T *ctx = (UDP_CLIENT_CTX_T *)obj;
    //��udp �յ������ݴ�ַ��MSG��
    *rawBuffer = &ctx->reqRawObj;
    return 0;
}

/**
*********************************************************************
* @brief�� ����udp��Ӧbuf
* @param[in]��
               rawBuffer    udp��Ҫ�ظ�����Ϣ
* @param[out]��
               obj          UDP_CLIENT_CTX_T
* @return�� ����ֵ˵��
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
    // //��tcp �յ������ݴ�ַ��MSG��
    *msg = &ctx->reqMsg;
    return 0;
}

static sint32 udp_get_rsp_msg(void *obj, MSG_OBJ_T **msg)
{
    UDP_CLIENT_CTX_T *ctx = (UDP_CLIENT_CTX_T *)obj;
    // //��tcp �յ������ݴ�ַ��MSG��
    *msg = &ctx->rspMsg;
    return 0;
}

/**
*********************************************************************
* @brief�� udp������Ϣ����
* @param[in]�� obj     UDP_CLIENT_CTX_T
* @param[out]��none
* @return�� ����ֵ˵��
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

    //�ͷ���udp_cb �й�����ctx �� msgHanderObj����
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
* @brief�� event��ʱ����
* @param[in]�� fd          �¼�������
               event      �¼������ͼ�����
               arg        �ص�����
* @param[out]��none
* @return�� ����ֵ˵��
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
        //�ٴ�ע��Timer
        event_add(timeout, &tv);
    }
    else
    {
        //ֹͣlibeven
        event_base_loopbreak(s_udpServer->base);
        olog_info(ocat, "udp server stop now");
    }
}

/**
*********************************************************************
* @brief�� udp �������ݻص�
* @param[in]�� sock        libevent����������
               which       �¼����ͼ�����
               arg         �ص�����
* @param[out]��none
* @return�� ����ֵ˵��
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
        if (recvLen < MIN_RAW_BUF_LEN)  //���յ�������С����С���ݰ�ʱ ����
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
            msgHanderObj->obj     = ctx;  //��CTX��msgHanderObj��
            ctx->reqRawObj.length = rxBufLen;
            ctx->client_sin       = client_sin;
            ctx->serverSock       = sock;
            msgHanderObj->getRspDataFp =
                udp_get_rsp_raw_buf;  //������Ӧ���Ļص�����
            msgHanderObj->getReqDataFp =
                udp_get_req_raw_buf;  //���������Ļ�ȡ�Ļص�����
            msgHanderObj->getReqDataMsgFp = udp_get_req_msg;
            msgHanderObj->getRspDataMsgFp = udp_get_rsp_msg;
            msgHanderObj->handerFp =
                udp_msg_handel_cb;  //��hander���������ͷ� msgHanderObj
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
* @brief�� udp�������
* @param[in]�� arg         �̲߳�����ǰδʹ��
* @param[out]��none
* @return�� ����ֵ˵��
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
    //��ʼ�� Libevent���
    base              = event_base_new();
    s_udpServer->base = base;

    //��ʼ�� timeout �¼� event_assign
    event_assign(&timeout, base, -1, flags, timeout_cb, (void *)&timeout);
    evutil_timerclear(&tv);
    tv.tv_sec = UDP_TIMER_POLL_INTERVAL;
    event_add(&timeout, &tv);

    // udp �����socket��ʼ��
    s_udpServer->serverSock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&sin, 0, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = inet_addr(s_udpServer->serverIp);  //��IP��ʼ��
    sin.sin_port = htons(s_udpServer->serverPort);  //�󶨶˿ڳ�ʼ��

    if (bind(s_udpServer->serverSock, (struct sockaddr *)&sin, sizeof(sin)))
    {
        olog_fatal(ocat, "UDP server Bind Port:[%d] fail.", s_udpServer->serverPort);
        exit(EXIT_FAILURE);  //�˿ڰ�ʧ���˳�����
    }
    else
    {
        olog_info(ocat, "UDP server bind on ip:[%s] port:[%u]",
                   s_udpServer->serverIp, s_udpServer->serverPort);
    }

    int nRecvBuf = 10 * 1024* 1024;  //����Ϊ10M
    setsockopt(s_udpServer->serverSock, SOL_SOCKET, SO_RCVBUF, &nRecvBuf,
               sizeof(nRecvBuf));

    int nSendBuf = 10 * 1024 * 1024;  //����Ϊ10M
    setsockopt(s_udpServer->serverSock, SOL_SOCKET, SO_SNDBUF, &nSendBuf,
               sizeof(nSendBuf));

    //��ʼ�� udp receive �¼�
    // olog_debug(ocat,"s_udpServer->serverSock = %d", s_udpServer->serverSock);
    event_assign(&udp_recv, base, s_udpServer->serverSock, EV_READ | EV_PERSIST,
                 udp_cb, NULL);
    event_add(&udp_recv, NULL);

    // event loop event �߳�ѭ��ִ��ֱ��ѭ����ȡ��
    event_base_dispatch(base);

    // event base������Ҫ����ȷ�ͷ�
    event_base_free(base);
    close(s_udpServer->serverSock);
    return (0);
}

/**
*********************************************************************
* @brief�� udpͨѶ����ϵͳ��ʼ��
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
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
        // //��ȫ�������ļ���ȡIP���˿ڵ�����
        // strncpy(s_udpServer->serverIp, g_configObj.serviceCfg.udpServerIP,
        //         IP_LEN);
        // s_udpServer->serverPort = g_configObj.serviceCfg.udpServerPort;
        ret                     = 0;
    }
    return ret;
}

/**
*********************************************************************
* @brief�� udpͨѶ��������ϵͳ
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
void udp_clean_up_system()
{
    olog_info(ocat, "udp_clean_up_system");
}

/**
*********************************************************************
* @brief�� udpͨѶ��������ϵͳ
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
sint32 udp_start_system()
{
    sint32 ret = -1;
    olog_info(ocat, "udp_start_system");
    //����UDP�߳�
    ret = pthread_create(&(s_udpServer->tid), NULL, udp_service, NULL);
    return ret;
}

/**
*********************************************************************
* @brief�� udpͨѶ����ֹͣϵͳ
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
void udp_stop_system()
{
    olog_info(ocat, "udp_stop_system");
}

/**
*********************************************************************
* @brief�� udp��Ϣ����
* @param[in]�� ip
               port
               data
               dataLen
* @param[out]��none
* @return�� ����ֵ˵��   -1 ����ʧ��
                        0 ���ͳɹ�
*********************************************************************
*/
sint32 udp_sendto(sint8 ip[IP_LEN], uint16 port, void *data, uint32 dataLen)
{
    struct sockaddr_in client_sin = {0};
    sint32             sendLen    = 0;

    client_sin.sin_family      = AF_INET;
    client_sin.sin_addr.s_addr = inet_addr(ip);  //
    client_sin.sin_port        = htons(port);    //�󶨶˿ڳ�ʼ��
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
* @brief�� ����IP
* @param[in]�� ip

* @param[out]��none
* @return�� ����ֵ˵��   NONE
*********************************************************************
*/
void set_udp_bind_ip(sint8 ip[IP_LEN])
{
    strncpy(s_udpServer->serverIp, ip, IP_LEN);
}

/**
*********************************************************************
* @brief�� ���ö˿�
* @param[in]�� port
* @param[out]��none
* @return�� ����ֵ˵��   none
*********************************************************************
*/
void set_udp_bind_port(uint16 port)
{
    s_udpServer->serverPort = port;
}