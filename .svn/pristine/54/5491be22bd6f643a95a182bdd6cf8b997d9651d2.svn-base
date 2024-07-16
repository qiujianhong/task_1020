/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块以太网通信
* @date：    2020-3-19
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/tcp.h>
#include "db_api.h"

#include "common_base.h"
#include "comm_listen.h"


/**********************************************************************
* @name      : listen_find_acceptfd
* @brief     ：获取accept_fd
* @param[in] ：uint8 channle    通道ID
               int accept_fd[]  句柄列表
* @param[out]：
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int listen_find_acceptfd(uint8 channle, int accept_fd[])
{
    if(channle < COMM_LISTEN_MAX)
    {
        return accept_fd[channle];
    }
    return -1;
}

/**********************************************************************
* @name      : eth_close_acceptfd
* @brief     ：关闭accept_fd
* @param[in] ：uint8 channle    通道ID
               int accept_fd[]  句柄列表
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
void listen_close_acceptfd(uint8 channle, COMM_LISTEN_HANDLE_T *ptHandle)
{
    if(channle < COMM_LISTEN_MAX)
    {
        if(ptHandle->accept_fd[channle] > 0)
        {
            close(ptHandle->accept_fd[channle]);
            ptHandle->accept_fd[channle] = -1;
            ptHandle->cIP[channle] = 0;
            ptHandle->cPort[channle] = 0;
            if(ptHandle->accept_num > 0)
            {
                ptHandle->accept_num--;
            }
        }
    }
}

/**********************************************************************
* @name      : listen_close_allfd
* @brief     ：关闭句柄
* @param[in] ：COMM_LISTEN_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-7
* @Update    :
**********************************************************************/
void listen_close_allfd(COMM_LISTEN_HANDLE_T *ptHandle)
{
    uint8 i;

    for(i = 0; i < COMM_LISTEN_MAX; i++)
    {
        if(ptHandle->accept_fd[i] > 0)
        {
            close(ptHandle->accept_fd[i]);
            ptHandle->accept_fd[i] = -1;
            ptHandle->cIP[i] = 0;
            ptHandle->cPort[i] = 0;
        }
    }
    close(ptHandle->listen_fd);
    ptHandle->listen_fd = -1;
    ptHandle->accept_num = 0;
}

/**********************************************************************
* @name      : comm_listen_send
* @brief     ：监听方式发送
* @param[in] ：COMM_QUEUE_LIST_T    *ptSendList 消息列表
               COMM_LISTEN_HANDLE_T *ptHandle   线程私有数据
* @param[out]：uint32 *pSendLen                 发送数据量
* @return    ：0成功/错误码, 报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int comm_listen_send(COMM_QUEUE_LIST_T *ptSendList, COMM_LISTEN_HANDLE_T *ptHandle, uint32 *pSendLen)
{
    int     sendLen = 0;
    uint16  dataLen = 0;
    uint8   dataBuf[BUF_LENGTH] = {0};   /* 缓冲区 */
    uint8   protocol = -1;               /* 内部协议 */
    uint8   channel = 0;
    int     fd = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    
    dataLen = comm_read_queue(ptSendList, dataBuf);
    while(dataLen)
    {
        if(dataLen < 4)
        {
            COMM_FMT_TRACE("comm_listen_send msg less then 4\n");
            goto cont;
        }
                
        channel  = dataBuf[0];
        protocol = comm_get_protocol(dataBuf);
        if(PROT_CLOSE_CHANNLE == protocol)
        {
            listen_close_acceptfd(channel, ptHandle);
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("Listen close channel %d\n", channel);
            goto cont;
        }
        if(PRO_DATA_TRANSMIT != protocol)
        {
            goto cont;
        }
        
        fd = listen_find_acceptfd(channel, ptHandle->accept_fd);
        if(fd > 0)
        {
            sendLen = send(fd, dataBuf+4, dataLen-4, 0);
            COMM_BUF_DEBUG(dataBuf+4, dataLen-4, "Listen channel %d send", channel);
            if(sendLen > 0)
            {
                *pSendLen += sendLen;
            }
        }
cont:
        memset(dataBuf, 0, BUF_LENGTH);
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    }
    return 0;
}

/**********************************************************************
* @name      : comm_listen_recv
* @brief     ：eth网口收消息 服务器方式
* @param[in] ：COMM_QUEUE_LIST_T *ptRecvList   消息列表
               COMM_LISTEN_HANDLE_T *ptHandle  线程私有数据
* @param[out]：uint32 *pRecvLen                接收数据量
* @return    ：0成功/错误码, 报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2020-3-19
* @Update    :
**********************************************************************/
int comm_listen_recv(COMM_QUEUE_LIST_T *ptRecvList, COMM_LISTEN_HANDLE_T *ptHandle, uint32 *pRecvLen)
{
    int     recvLen = 0;
    uint8   i;
    fd_set  fdsr;
    int     maxsock;
    int     ret = 0;
    struct timeval tv = {0,0};      //超时时间设为0 是非阻塞
    struct sockaddr_in client_addr; // connector's address information
    uint8   dataBuf[BUF_LENGTH] = {0};   /* 缓冲区 */
    int     new_fd = -1;
    socklen_t sin_size = sizeof(client_addr);
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    uint32          ip;
    uint16          port;

    maxsock = ptHandle->listen_fd;

    /* 0 初始化 */
    FD_ZERO(&fdsr);
    FD_SET(maxsock, &fdsr);
    
    /* 1 向fd数组中增加连接 */
    for (i = 0; i < ptHandle->num; i++) 
    {
        if(ptHandle->accept_fd[i] > 0) 
        {
            FD_SET(ptHandle->accept_fd[i], &fdsr);
            if(maxsock < ptHandle->accept_fd[i])
            {
                maxsock = ptHandle->accept_fd[i];
            }
             
        }
    }
    ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
    if (ret <= 0) 
    {
        return 0;
    }

    /* 2 接收数据 */
    for (i = 0; i < ptHandle->num; i++) 
    {
        if(ptHandle->accept_fd[i] <= 0)
        {
            continue;
        }
        if(FD_ISSET(ptHandle->accept_fd[i], &fdsr))
        {
            memset(dataBuf, 0, BUF_LENGTH);
            recvLen = recv(ptHandle->accept_fd[i], dataBuf, BUF_LENGTH, 0);
            if(recvLen <= 0)
            {
                FD_CLR(ptHandle->accept_fd[i], &fdsr);
                close(ptHandle->accept_fd[i]);
                ptHandle->cIP[i] = 0;
                ptHandle->cPort[i] = 0;
                ptHandle->accept_fd[i] = -1;
                if(ptHandle->accept_num > 0)
                {
                    ptHandle->accept_num--;
                }
                /* 关闭通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = i;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                recvLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptRecvList, dataBuf, recvLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("client %d, channel id %d close\n", i, ptIPCMsg->id);
            }
            else
            {
                *pRecvLen += recvLen;
                comm_add_channel(i, dataBuf, recvLen);
                recvLen += 4;
                if(0 != comm_write_queue(ptRecvList, dataBuf, recvLen))
                {
                    COMM_FMT_TRACE("Eth Msg Queue is full\n");
                }
                COMM_BUF_DEBUG(dataBuf + 4, recvLen - 4, "Listen channel %d recv:", i);
            }
            /* 延时 */
            usleep(10);
        }
    }

    /* 3 查看是否有新的连接 */
    if(FD_ISSET(ptHandle->listen_fd, &fdsr))
    {
        new_fd = accept(ptHandle->listen_fd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_fd <= 0) 
        {
            COMM_FMT_TRACE("Listen accept error\n");
            return 0;
        }
        ip = ntohl(client_addr.sin_addr.s_addr);
        port = ntohs(client_addr.sin_port);
        for(i = 0; i < ptHandle->num; i++)
        {
            if(ptHandle->accept_fd[i] <= 0)
            {
                ptHandle->accept_fd[i] = new_fd;
                ptHandle->cIP[i] = ip;
                ptHandle->cPort[i] = port;
                if(ptHandle->accept_num < ptHandle->num)
                {
                    ptHandle->accept_num++;
                }

                /* 开启通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptIPCMsg->id = i;
                ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                ptChanInfo->ip = ip;
                ptChanInfo->port = port;
                ptChanInfo->workmode = MODE_SERVER;
                ptChanInfo->medium = COMM_MEDIA_LISTEN;
                recvLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                if(0 != comm_write_queue(ptRecvList, dataBuf, recvLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("Listen open channel %d, ip 0x%x, port %d\n", i, ip,port);
                break;
            }
        }
        if(ptHandle->num == i)
        {
            close(new_fd);
            COMM_FMT_DEBUG("Listen accept num max then %d, ip 0x%x, port %d\n", i, ip, port);
        }
    }
    return 0;
}

/**********************************************************************
* @name      : comm_listen_start
* @brief     ：监听启动判断
* @param[in] ：
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2020-3-6
* @Update    :
**********************************************************************/
BOOLEAN comm_listen_start(void *pArg)
{
    COMM_LISTEN_HANDLE_T *ptHandle = (COMM_LISTEN_HANDLE_T *)pArg;

    if(NULL == pArg || 0 == *ptHandle->pIPCok || 0 == ptHandle->port)
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
* @name      : comm_eth_entry
* @brief     ：eth0端口消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2019-12-9
**********************************************************************/
void *comm_listen_entry(void *pArg)
{
    uint8    stateEth = LISTEN_RECV_DATA; /* 状态机 */
    uint32    rxtxLen = 0;
    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */
    struct timespec curtime = {0};
    struct timespec oldtime = {0};

    COMM_LISTEN_HANDLE_T *ptHandle = (COMM_LISTEN_HANDLE_T *)pArg;

    if(NULL == pArg)
    {
        return 0;
    }

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }
    
    /* 先释放上次没有来及释放的句柄 */
    listen_close_allfd(ptHandle);

    ptHandle->accept_num = 0;
    *ptHandle->pRunFlag = 0;

    /* 建立监听句柄 */
    ptHandle->listen_fd = comm_open_listen(ptHandle->port, ptHandle->num);
    if(ptHandle->listen_fd < 0)
    {
        COMM_FMT_DEBUG("Listen open listen %d port failed\n", ptHandle->port);
        return 0;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart)
            {
                COMM_FMT_DEBUG("Listen no link or appmain restart\n");
                stateEth = LISTEN_CLOSE_DEVICE;
            }
        }

        switch(stateEth)
        {
            case LISTEN_SEND_DATA:
            {
                /* 需要将所有的消息都发送出去 */
                if(0 != comm_listen_send(ptHandle->ptSendList, ptHandle, &rxtxLen))
                {
                    stateEth = LISTEN_CLOSE_DEVICE;
                    break;
                }
                applyTxFlow += rxtxLen;
                stateEth = LISTEN_RECV_DATA;
                break;
            }
            case LISTEN_RECV_DATA:
            {
                if(0 != comm_listen_recv(ptHandle->ptRecvList, ptHandle, &rxtxLen))
                {
                    stateEth = LISTEN_CLOSE_DEVICE;
                    break;
                }
                applyRxFlow += rxtxLen;
                stateEth = LISTEN_SEND_DATA;
                break;
            }
            case LISTEN_CLOSE_DEVICE:
            {
                uint8   i;
                uint8   dataBuf[20] = {0};   /* 缓冲区 */
                COMM_IPCMSG_T  *ptIPCMsg = NULL;
                uint16  len = 0;
                /* 关闭通道 */
                for(i = 0; i < ptHandle->num; i++)
                {
                    if(ptHandle->accept_fd[i] <= 0)
                    {
                        continue;
                    }
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = i;
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    len = sizeof(COMM_IPCMSG_T);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, len))
                    {
                        COMM_FMT_TRACE("Msg Queue is full\n");
                    }
                    COMM_FMT_DEBUG("Listen channel id %d close\n", i);
                }
                listen_close_allfd(ptHandle);
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
                return 0;
            }
            default:
            {
                stateEth = LISTEN_SEND_DATA;
                break;
            }
        }
        usleep(10000);
    }
}

