/*
*********************************************************************
* Copyright(C) 2020南京新联电子股份有限公司
* All rights reserved.
* @brief＿tcpClient.c
* @date＿2020-6-10
*********************************************************************
*/

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "baseConstant.h"
#include "baseType.h"
#include "channelStatus.h"
#include "socketHelper.h"
#include "tcpClient.h"
#include "xlClient.h"
#include "xlDebug.h"

/**
*********************************************************************
* @brief：tcp客户端关闿* @param[in]：c 客户端信恿* @param[out]：NONE

* @return＿NONE
*********************************************************************
*/
static void tcp_client_close(XL_CLIENT_T *c)
{
    if (c->uc.tclient.tfd > 0)
    {
        close(c->uc.tclient.tfd);
        c->uc.tclient.tfd = -1;
    }
}

/**
*********************************************************************
* @brief：tcp客户端初始化
* @param[in]：NONE
* @param[out]：c 客户端信恿
* @return＿0
*********************************************************************
*/
static sint32 tcp_client_init(XL_CLIENT_T *c)
{
    c->status = CH_INIT;
    return 0;
}

/**
*********************************************************************
* @brief：tcp客户端连掿* @param[in]：NONE
* @param[out]：c 客户端信恿
* @return＿0
*********************************************************************
*/
static sint32 tcp_client_connect(XL_CLIENT_T *c)
{
    sint32             ret = -1;
    struct sockaddr_in dst_addr;

    do
    {
        c->uc.tclient.tfd = socket(AF_INET, SOCK_STREAM, 0);
        if (c->uc.tclient.tfd < 0)
        {
            XL_ERROR( "Create tcp socket fail :%s", strerror(errno));
            break;
        }

        // memset(&(dst_addr.sin_zero), 0, sizeof(dst_addr.sin_zero));

        if (0 != setnonblock(c->uc.tclient.tfd))
        {
            XL_ERROR( "setnonblock tcp socket fail :%s",
                       strerror(errno));
            break;
        }

        int enable = 1;
        if (setsockopt(c->uc.tclient.tfd, IPPROTO_TCP, TCP_NODELAY,
            (void *)&enable, sizeof(enable)) < 0)
        {
            XL_ERROR( "ERROR: TCP_NODELAY SETTING ERROR!\n");
            break;
        }

        dst_addr.sin_family      = AF_INET;
        dst_addr.sin_port        = htons(c->uc.tclient.server_port);
        dst_addr.sin_addr.s_addr = inet_addr(c->uc.tclient.server_ip);

        // printf("====================>CONNECT START\n");
        ret = connect(c->uc.tclient.tfd, (struct sockaddr *)&dst_addr,
                      sizeof(dst_addr));

        if (ret == -1 && errno != EINPROGRESS)
        {
            XL_WARN( "error in tcp connect: %d,%s\n", errno,
                      strerror(errno));
            tcp_client_close(c);
            c->status = CH_CONNECT_FAIL;
            break;
        }
        c->status = CH_CONNECTING;
        ret = 0;

    } while (0);
    

    
    return ret;
}

/**
*********************************************************************
* @brief：tcp客户端状态获叿* @param[in]：NONE
* @param[out]：c 客户端信恿
* @return＿客户端状怿*********************************************************************
*/
static CLIENT_STATUS_E tcp_client_get_status(XL_CLIENT_T *c)
{
    // CLIENT_STATUS_E ret   = CH_CONNECT_FAIL;
    sint32         error = -1;
    socklen_t      len   = sizeof(int);
    struct timeval tm;
    fd_set         writeSet;
    tm.tv_sec  = 0;
    tm.tv_usec = 100;

    if (c->status == CH_CONNECTING || c->status == CH_CONNECT_SUCC)
    {
        FD_ZERO(&writeSet);
        FD_SET(c->uc.tclient.tfd, &writeSet);
        if (select(c->uc.tclient.tfd + 1, NULL, &writeSet, NULL, &tm) > 0)
        {
            if (!FD_ISSET(c->uc.tclient.tfd, &writeSet))
            {
                // printf("===>!FD_ISSET \n");
            }
            else
            {
                // printf("===>FD_ISSET \n");
                if (0 == getsockopt(c->uc.tclient.tfd, SOL_SOCKET, SO_ERROR,
                                    (char *)&error, &len))
                {
                    // printf("error = %d\n", error);
                    if (error == 0)
                    {
                        // ret = CH_CONNECT_SUCC;
                        c->status = CH_CONNECT_SUCC;
                    }
                    else if (error == EINPROGRESS)
                    {
                         XL_DEBUG( "connecting Result: %d,%s\n", error,
                                   strerror(error));
                        c->status = CH_CONNECTING;
                    }
                    else
                    {
                        // ret = CH_CONNECT_FAIL;
                        XL_ERROR( "connect Fail Result: %d,%s\n", error,
                                   strerror(error));
                        c->status = CH_CONNECT_FAIL;
                    }
                }
                else
                {
                    XL_WARN( "Error in getsockopt: %d,%s\n", errno,
                              strerror(errno));
                    c->status = CH_CONNECT_FAIL;
                }
            }
        }
        else
        {
            // ret = CH_CONNECTING;
            c->status = CH_CONNECTING;
        }
    }

    // printf("c->status = %d\n", c->status);

    return c->status;
}

/**
*********************************************************************
* @brief：tcp客户端写接口
* @param[in]：c 客户端信恿              data 数据
              dataLen 数据长度
* @param[out]：c 客户端信恿
* @return＿客户端状怿*********************************************************************
*/
static sint32 tcp_clien_write(XL_CLIENT_T *c, const char *data, sint32 dataLen)
{
    sint32 sendLen = -1;

    if (c->status == CH_CONNECT_SUCC)
    {
        // printf("====>send\n");
        sendLen = send(c->uc.tclient.tfd, data, dataLen, MSG_NOSIGNAL);
        // printf("<====send sendLen:[%d] \n", sendLen);
        
        if (sendLen < 0)
        {
            XL_WARN( "tcp_clien_write Fail Result: %d,%s\n", errno,
                      strerror(errno));
            c->status = CH_CONNECT_FAIL;
        }

        // if (sendLen != dataLen)
        // {
        //     fprintf(stderr,
        //             "tcp_clien_write sendLen != dataLen  Result: %d,%s\n",
        //             errno, strerror(errno));
        // }

        // if (sendLen != 259)
        // {
        //     // printf("######sendLen != 259\n");
        // }

    }
    else if (c->status == CH_CONNECTING)
    {
        sendLen = 0;
    }

    return sendLen;
}

/**
*********************************************************************
* @brief：tcp客户端读接口
* @param[in]：c 客户端信恿* @param[out]：data 数据
              dataLen 数据长度

* @return＿recvLen
*********************************************************************
*/
static sint32 tcp_clien_read(XL_CLIENT_T *c, char *data, sint32 dataLen)
{
    sint32 recvLen = -1;

    if (c->status == CH_CONNECT_SUCC)
    {
        // printf("====>recv\n");
        recvLen = recv(c->uc.tclient.tfd, data, dataLen, 0);
        // printf("<====recv recvLen:[%d] \n", recvLen);

        if (recvLen < 0)
        {
            if (errno == 0 || errno == EAGAIN)
            {
                // printf("errno == EAGAIN\n");
                recvLen = 0;
            }
            else
            {
                XL_WARN( "tcp_clien_read Fail Result: %d,%s\n", errno,
                          strerror(errno));
                c->status = CH_CONNECT_FAIL;
            }
        }
    }
    else if (c->status == CH_CONNECTING)
    {
        recvLen = 0;
    }

    return recvLen;
}

/**
*********************************************************************
* @brief：断开连接
* @param[in]：c 客户端信恿* @param[out]：none

* @return＿0
*********************************************************************
*/
static sint32 tcp_client_disconnect(XL_CLIENT_T *c)
{
    tcp_client_close(c);
    return 0;
}

/**
*********************************************************************
* @brief：删除客户端
* @param[in]：c 客户端信恿* @param[out]：none

* @return＿0
*********************************************************************
*/
void del_tcp_client(XL_CLIENT_T *c)
{
    // do any
    tcp_client_close(c);
    free(c);
}

/**
*********************************************************************
* @brief：新建客户端
* @param[in]：none
* @param[out]：none

* @return＿XL_CLIENT_T*
*********************************************************************
*/
XL_CLIENT_T *new_tcp_client(void)
{
    XL_CLIENT_T *client = calloc(1, sizeof(XL_CLIENT_T));
    do
    {
        if (NULL == client)
        {
            break;
        }

        client->initFp             = tcp_client_init;
        client->connectFp          = tcp_client_connect;
        client->getConnectStatusFp = tcp_client_get_status;
        client->readFp             = tcp_clien_read;
        client->writeFp            = tcp_clien_write;
        client->disconnectFp       = tcp_client_disconnect;

    } while (0);

    return client;
}