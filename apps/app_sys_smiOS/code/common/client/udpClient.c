/*
*********************************************************************
* Copyright(C) 2020南京新联电子股份有限公司
* All rights reserved.
* @brief： udpClient.c
* @date： 2020-6-10
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

#include "baseConstant.h"
#include "baseType.h"
#include "channelStatus.h"

#include "socketHelper.h"
#include "udpClient.h"
#include "xlClient.h"

#include "xlDebug.h"

/**
*********************************************************************
* @brief：udp客户端关闭
* @param[in]：c 客户端信息
* @param[out]：NONE

* @return： NONE
*********************************************************************
*/
static void udp_client_close(XL_CLIENT_T *c)
{
    if (c->uc.uclient.ufd > 0)
    {
        close(c->uc.uclient.ufd);
        c->uc.uclient.ufd = -1;
    }
}

/**
*********************************************************************
* @brief：udp客户端初始化
* @param[in]：c 客户端信息
* @param[out]：NONE

* @return： NONE
*********************************************************************
*/
static sint32 udp_client_init(XL_CLIENT_T *c)
{
    // c->uc.tclient.local_ip
    sint32 ret = -1;
    XL_DEBUG("udp_client_init");
    do
    {
        c->uc.uclient.ufd = socket(AF_INET, SOCK_DGRAM, 0);
        if (c->uc.uclient.ufd < 0)
        {
             XL_ERROR( "Create tcp socket fail :%s", strerror(errno));
            break;
        }

        // memset(&(dst_addr.sin_zero), 0, sizeof(dst_addr.sin_zero));
        int nRecvBuf = 10 * 1024 * 1024;  //设置为10M
        setsockopt(c->uc.uclient.ufd, SOL_SOCKET, SO_RCVBUF, &nRecvBuf,
                   sizeof(nRecvBuf));

        int nSendBuf = 10 * 1024 * 1024;  //设置为10M
        setsockopt(c->uc.uclient.ufd, SOL_SOCKET, SO_SNDBUF, &nSendBuf,
                   sizeof(nSendBuf));

        c->status = CH_INIT;
        return 0;
    } while (0);

    c->status = CH_CONNECT_FAIL;
    return ret;
}

/**
*********************************************************************
* @brief：udp客户端连接
* @param[in]：c 客户端信息
* @param[out]：NONE

* @return： -1      fail
            0       success
*********************************************************************
*/
static sint32 udp_client_connect(XL_CLIENT_T *c)
{
    sint32             ret = -1;
    struct sockaddr_in dst_addr;
    dst_addr.sin_family      = AF_INET;
    dst_addr.sin_port        = htons(c->uc.uclient.server_port);
    dst_addr.sin_addr.s_addr = inet_addr(c->uc.uclient.server_ip);

    ret = connect(c->uc.uclient.ufd, (struct sockaddr *)&dst_addr,
                  sizeof(dst_addr));

    if (ret == -1 && errno != EINPROGRESS)
    {
         XL_WARN( "Error in connect: %d,%s\n", errno, strerror(errno));
        udp_client_close(c);
        c->status = CH_CONNECT_FAIL;
        return -1;
    }

    if (0 != setnonblock(c->uc.uclient.ufd))
    {
         XL_WARN( "setnonblock tcp socket fail :%s", strerror(errno));
        return -1;
    }

    c->status = CH_CONNECT_SUCC;
    return 0;
}

/**
*********************************************************************
* @brief：获取udp客户端状态
* @param[in]：c 客户端信息
* @param[out]：NONE

* @return： CLIENT_STATUS_E
*********************************************************************
*/
static CLIENT_STATUS_E udp_client_get_status(XL_CLIENT_T *c)
{
    return c->status;
}

/**
*********************************************************************
* @brief：udp客户端写接口
* @param[in]：c 客户端信息
              data 数据
              datalen 数据长度
* @param[out]：NONE

* @return： sendlen
*********************************************************************
*/
static sint32 udp_clien_write(XL_CLIENT_T *c, const char *data, sint32 dataLen)
{
    sint32 sendlen = -1;
    sendlen        = send(c->uc.uclient.ufd, data, dataLen, 0);
    // fprintf(stderr, "udp_clien_write :%d\n", sendlen);

    if (sendlen != dataLen)
    {
        c->status = CH_CONNECT_FAIL;
         XL_WARN( "udp send fail errno:%s\n", strerror(errno));
    }
    return sendlen;
}

/**
*********************************************************************
* @brief：udp客户端读接口
* @param[in]：c 客户端信息
* @param[out]：data 数据
              datalen 数据长度

* @return： 收到消息长度
*********************************************************************
*/
static sint32 udp_clien_read(XL_CLIENT_T *c, char *data, sint32 dataLen)
{
    sint32 rn = recv(c->uc.uclient.ufd, data, dataLen, 0);

    return rn;
}

/**
*********************************************************************
* @brief：udp客户端断开连接
* @param[in]：c 客户端信息
* @param[out]：none

* @return： 0
*********************************************************************
*/
static sint32 udp_client_disconnect(XL_CLIENT_T *c)
{
    udp_client_close(c);
    return 0;
}

/**
*********************************************************************
* @brief：删除udp客户端
* @param[in]：c 客户端信息
* @param[out]：none

* @return： 0
*********************************************************************
*/
void del_udp_client(XL_CLIENT_T *c)
{
    // do any
    udp_client_close(c);
    free(c);
}

/**
*********************************************************************
* @brief：创建udp客户端
* @param[in]：NONE
* @param[out]：none

* @return： XL_CLIENT_T*
*********************************************************************
*/
XL_CLIENT_T *new_udp_client(void)
{
    XL_CLIENT_T *client = calloc(1, sizeof(XL_CLIENT_T));
    do
    {
        if (NULL == client)
        {
            break;
        }

        client->initFp             = udp_client_init;
        client->connectFp          = udp_client_connect;
        client->getConnectStatusFp = udp_client_get_status;
        client->readFp             = udp_clien_read;
        client->writeFp            = udp_clien_write;
        client->disconnectFp       = udp_client_disconnect;

    } while (0);

    return client;
}