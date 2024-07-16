/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块以太网通信
* @date：    2019-10-31
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/tcp.h>
#include "task_msg.h"
#include "db_api.h"
#include "msg_id.h"
#include "task_id.h"

#include "comm.h"
#include "eth_comm.h"
#include "proxy_comm.h"

#define  ETH_LISTEN_PORT         5100  //复用104的网口

uint8     g_ETHName[2][24] = {{0},{0}};
uint8     *g_ETHStatus[2] = {NULL};
extern BOOLEAN app_comm_proxy_isuse();
extern BOOLEAN g_proxyOK;
uint8     g_ETHOnlineCHeck[2] = {0};

uint8   g_ETHdataBuf[2][BUF_LENGTH + 8] = {{0}, {0}};   /* 缓冲区 */

extern int ipconfig_send_to_smiOS(char* Name, uint8 type, uint8* IP, uint8* NetMask, uint8* NetGw);
extern uint8 gdelay;
extern uint8 ssal_get_status();

/**********************************************************************
* @name      : eth_get_channeloffset
* @brief     ：获取内部通道偏移
* @param[in] ：uint8 channle    通道ID
* @param[out]：
* @return    ：偏移
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
int eth_get_channeloffset(uint8 channle)
{
    int offset = -1;
    if(channle >= CHL_RESERVE && channle < CHL_ETH_1)
    {
        return offset;
    }
    
    if(channle >= CHL_ETH_2)
    {
        channle -= CHL_ETH_2;
    }
    else
    {
        channle -= CHL_ETH_1;
    }
    offset = channle;
    return offset;
}

/**********************************************************************
* @name      : eth_find_clientfd
* @brief     ：获取client_fd
* @param[in] ：uint8 channle    通道ID
               int client_fd[]  句柄列表
* @param[out]：uint8 *pIndex    内部编号
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
int eth_find_clientfd(uint8 channle, int client_fd[], uint8 *pIndex)
{
    if(channle >= CHL_RESERVE && channle < CHL_ETH_1)
    {
        return -1;
    }
    
    if(channle >= CHL_ETH_2)
    {
        channle -= CHL_ETH_2;
    }
    else
    {
        channle -= CHL_ETH_1;
    }
    if(channle < OOP_MAX_HOST)
    {
        *pIndex = channle;
        return client_fd[channle];
    }
    return -1;
}

/**********************************************************************
* @name      : eth_close_clientfd
* @brief     ：关闭client_fd
* @param[in] ：uint8 channle    通道ID
               int client_fd[]  句柄列表
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
void eth_close_clientfd(uint8 channle, int client_fd[])
{
    if(channle >= CHL_RESERVE && channle < CHL_ETH_1)
    {
        return;
    }
    
    if(channle >= CHL_ETH_2)
    {
        channle -= CHL_ETH_2;
    }
    else
    {
        channle -= CHL_ETH_1;
    }
    if(channle < OOP_MAX_HOST)
    {
        if(client_fd[channle] > 0)
        {
            close(client_fd[channle]);
            client_fd[channle] = -1;
        }
    }
}

/**********************************************************************
* @name      : eth_find_acceptfd
* @brief     ：获取accept_fd
* @param[in] ：uint8 channle    通道ID
               int accept_fd[]  句柄列表
* @param[out]：
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
int eth_find_acceptfd(uint8 channle, int accept_fd[])
{
    if(channle < CHL_ETH_1 || channle >= CHL_RESERVE)
    {
        return -1;
    }
    
    if(channle >= CHL_ETH_2)
    {
        channle -= (CHL_ETH_2 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    else
    {
        channle -= (CHL_ETH_1 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    
    if(channle < OOP_MAX_LISTENS * ETH_CONNECT_NUM)
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
void eth_close_acceptfd(uint8 channle, int accept_fd[])
{
    if(channle >= CHL_RESERVE && channle < CHL_ETH_1)
    {
        return;
    }
    
    if(channle >= CHL_ETH_2)
    {
        channle -= (CHL_ETH_2 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    else
    {
        channle -= (CHL_ETH_1 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    if(channle < OOP_MAX_LISTENS * ETH_CONNECT_NUM)
    {
        if(accept_fd[channle] > 0)
        {
            close(accept_fd[channle]);
            accept_fd[channle] = -1;
        }
    }
}

/**********************************************************************
* @name      : eth_acceptfd_connect
* @brief     ：设置服务器模式连接状态
* @param[in] ：uint8 channle    通道ID
               uint8 bStatus[]  状态列表
               uint8 bConnect   状态
* @param[out]：
* @return    ：偏移
* @Create    : 王津剑
* @Date      ：2020-4-1
* @Update    :
**********************************************************************/
uint8 eth_acceptfd_connect(uint8 channle, uint8 bStatus[], uint8 bConnect)
{
    if(channle >= CHL_RESERVE && channle < CHL_ETH_1)
    {
        return 0xff;
    }
    
    if(channle >= CHL_ETH_2)
    {
        channle -= (CHL_ETH_2 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    else
    {
        channle -= (CHL_ETH_1 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    if(channle < OOP_MAX_LISTENS * ETH_CONNECT_NUM)
    {
        bStatus[channle] = bConnect;
    }
    return channle;
}

/**********************************************************************
* @name      : eth_open_listen
* @brief     ：eth网口工作 服务器模式
* @param[in] ：char *ifName         网口名称
               uint16 listenPort[]  监听端口
* @param[out]：COMM_ETH_HANDLE_T *ptHandle
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
int eth_open_listen(char *ifName, uint16 listenPort[], COMM_ETH_HANDLE_T *ptHandle)
{
    uint8 i;
    int listenfd;  
    struct sockaddr_in servaddr;
    //struct ifreq interf;
    int flags_block;
    int opt = 1;
    int val;

    for(i = 0; i < ptHandle->listenNum; i++)
    {
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0)
        {
            COMM_FMT_TRACE("%s get socket error\n", ifName);
            goto closefd;
        }
        
        /* socket绑定网口 */
        /*bzero(&interf, sizeof(interf));
        strncpy(interf.ifr_ifrn.ifrn_name, ifName, strlen(ifName));
        if (setsockopt(listenfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interf, sizeof(interf)) < 0) 
        {
            COMM_FMT_TRACE("%s SO_BINDTODEVICE failed\n", ifName);
            goto closefd;
        }*/
    
        /* 设置SOCKET文件句柄为非阻塞模式 */
        if((flags_block = fcntl(listenfd, F_GETFL, 0))<0) 
        {   
            COMM_FMT_TRACE("%s ERROR F_GETFL\n", ifName);
            close(listenfd);
            goto closefd;
        }   
        
        if(fcntl(listenfd, F_SETFL, flags_block | O_NONBLOCK))
        {   
            COMM_FMT_TRACE("%s ERROR set mode\n", ifName);
            close(listenfd);
            goto closefd;
        }

        /* 关闭子进程无用文件描述符 */
        val = fcntl(listenfd, F_GETFD);
        if (val == -1)
        {
            COMM_FMT_TRACE("%s ERROR F_GETFD\n", ifName);
            close(listenfd);
            goto closefd;
        }

        val |= FD_CLOEXEC;
        if (fcntl(listenfd, F_SETFD, val) == -1)
        { 
            COMM_FMT_TRACE("%s ERROR set F_SETFD\n", ifName);
            close(listenfd);
            goto closefd;
        }

        //SO_REUSEADDR 允许在bind（）过程中本地地址可重复使用
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
        {
             COMM_FMT_TRACE("%s setsockopt error SO_REUSEADDR\n", ifName);
             goto closefd;
        }
        
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family      = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port        = htons(listenPort[i]);
        
        if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0)
        {
            close(listenfd);
            COMM_FMT_TRACE("%s bind %d error\n", ifName, listenPort[i]);
            goto closefd;
        }

        if (listen(listenfd, ETH_CONNECT_NUM) < 0)
        {
            close(listenfd);
            COMM_FMT_TRACE("%s listen %d num error\n", ifName, listenPort[i]);
            goto closefd;
        }
        ptHandle->listen_fd[i] = listenfd;
    }

    return 0;
closefd:
    for(i = 0; i < ptHandle->listenNum; i++)
    {
        if(ptHandle->listen_fd[i] > 0)
        {
            close(ptHandle->listen_fd[i]);
            ptHandle->listen_fd[i] = -1;
        }
    }
    return -1;
}

/**********************************************************************
* @name      : eth_close_listen
* @brief     ：eth网口关闭句柄 服务器模式
* @param[in] ：COMM_ETH_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
void eth_close_listen(COMM_ETH_HANDLE_T *ptHandle)
{
    uint8 i, j;

    for(i = 0; i < OOP_MAX_LISTENS; i++)
    {
        for(j = 0; j < ETH_CONNECT_NUM; j++)
        {
            if(ptHandle->accept_fd[i][j] > 0)
            {
                close(ptHandle->accept_fd[i][j]);
            }
            ptHandle->accept_fd[i][j] = -1;
        }
        if(ptHandle->listen_fd[i] > 0)
        {
            close(ptHandle->listen_fd[i]);
        }
        ptHandle->listen_fd[i] = -1;
    }
}

/**********************************************************************
* @name      : eth_add_route
* @brief     ：eth网口设置路由, 同网段设置路由
* @param[in] ：char *pNetName    网口名称
               uint8 *pRouteAddr  路由地址
               COMM_ETH_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-7-1
* @Update    :
**********************************************************************/
void eth_add_route(const char *pNetName, uint8 *pRouteAddr, COMM_ETH_HANDLE_T *ptHandle)
{
    OOP_ETHIP_T *ptAddr = (OOP_ETHIP_T *)&(ptHandle->tEth.tEthIP);
    uint32 iphost = 0;
    uint32 ipmaster = 0;
    uint32 ipmask = 0;
    appmsg_memcpy_r(&iphost, ptAddr->ip, 4);
    appmsg_memcpy_r(&ipmaster, pRouteAddr, 4);
    appmsg_memcpy_r(&ipmask, ptAddr->mask, 4);
        
    if(0 == iphost || (iphost & ipmask) == (ipmaster & ipmask))
    {
        comm_add_route_net(pNetName, pRouteAddr);
    }
}

/**********************************************************************
* @name      : comm_eth_send
* @brief     ：eth网口发消息 非服务器方式
* @param[in] ：uint8 ifuint                网口编号
               char *ifName                网口名称
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：发送长度/-1报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2019-12-10
**********************************************************************/
int comm_eth_send(uint8 ifuint, char *ifName, COMM_ETH_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 index;
    int   fdSocket = 0;
    uint8  channel = 0;
    uint8 protocol = -1;         /* 内部协议 */
    int    sendLen = 0;
    int     outlen = 0;
    uint16  dataLen = 0;
    uint8  *dataBuf = NULL;   /* 缓冲区 */
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    struct timespec curtime = {0};

    OOP_ETHCONFIG_T *ptEthCfg = &(ptHandle->tEth.tEthCfg);

    *pNextState = ETH_RECV_DATA;

    if(TYPE_TCP == ptEthCfg->contype && MODE_SERVER == ptEthCfg->workmode) //服务器模式不支持
    {
        return 0;
    }

    dataBuf = g_ETHdataBuf[ifuint];
    dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    while(dataLen)
    {
        if(dataLen <= 4)
        {
            COMM_FMT_TRACE("comm_eth_send msg less then 4\n");
            goto cont;
        }
        channel = *dataBuf;
        protocol = comm_get_protocol(dataBuf);

        fdSocket = eth_find_clientfd(channel, ptHandle->client_fd, &index);
        if(PRO_LOGIN_ERROR == protocol)
        {
            COMM_FMT_DEBUG("%s登录失败, 关闭通道%d\n", ifName, channel);
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            if(MODE_ZB == ptEthCfg->conmode)
            {
                /* 主备模式切换下一个主站链接 */
                ptHandle->masterStart += 1;
                *pNextState = ETH_CLOSE_DEVICE;
            }
            else
            {
                *pNextState = ETH_CONNECT_MASTER;
            }
            eth_close_clientfd(channel, ptHandle->client_fd);
            goto end;
        }

        if(PRO_HEARTBEAT_ERROR == protocol)
        {
            COMM_FMT_DEBUG("%s丢失心跳, 关闭通道%d\n", ifName, channel);
            eth_close_clientfd(channel, ptHandle->client_fd);
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            *pNextState = ETH_CONNECT_MASTER;
            goto end;
        }

        if(PROT_CLOSE_CHANNLE == protocol)
        {
            eth_close_clientfd(channel, ptHandle->client_fd);
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = MODE_MIX == ptEthCfg->workmode ? PROT_PAUSE_CHANNLE : PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("%s %s channel %d\n", ifName, MODE_MIX == ptEthCfg->workmode ? "pause" : "close", channel);
            goto end;
        }
        if(PRO_BIG_MSG == protocol)
        {
            /* 超大消息 */
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            COMM_BIGMSG_T *ptBigMsg = (COMM_BIGMSG_T *)(dataBuf + 4);
            if(curtime.tv_sec > ptBigMsg->packtime + 10)
            {
                /* 超过10s就不发送了 */
                goto cont;
            }
            COMM_FMT_DEBUG("%s big msg send len %d\n", ifName, ptBigMsg->msglen);
            if(fdSocket > 0)
            {
                uint32 singlelen = 0;
                uint32 msgLeft = ptBigMsg->msglen;
                uint8 *pTemp = ptBigMsg->msgaddr;
                singlelen = MIN(msgLeft, 512);
                sendLen = 0;
                while(singlelen > 0)
                {
                    COMM_BUF_DEBUG(pTemp, singlelen, "%s big msg send", ifName);
                    sendLen += send(fdSocket, pTemp, singlelen, 0);
                    pTemp += singlelen;
                    msgLeft -= singlelen;
                    singlelen = MIN(msgLeft, 512);
                    usleep(10000);
                }
                if(sendLen > 0)
                {
                    outlen += sendLen;

                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = channel;
                    ptIPCMsg->protocol = PRO_BIG_MSG;
                    memcpy(dataBuf + 4, ptBigMsg, sizeof(COMM_BIGMSG_T));
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(COMM_BIGMSG_T);
                }
                else
                {
                    *pNextState = ETH_CONNECT_MASTER;
                    eth_close_clientfd(channel, ptHandle->client_fd);
                    /* 关闭通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = channel;
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    dataLen = sizeof(COMM_IPCMSG_T);
                    COMM_FMT_DEBUG("%s close channel %d\n", ifName, channel);
                }

                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
            }
            goto end;
        }

        if(PROT_CONTINUE_CHANNLE == protocol)
        {
            if(MODE_MIX == ptEthCfg->workmode && fdSocket < 0)
            {
                ptHandle->client_fd[index] = comm_connect_socket(ifName, ptHandle->masterIP[index], 
                                                                 ptHandle->masterPort[index]);  /*连接指定IP的端口号*/
                if(ptHandle->client_fd[index] <= 0)
                {
                    COMM_FMT_DEBUG("%s connect channel %d failed\n", ifName, channel);
                    goto end;
                }
                fdSocket = ptHandle->client_fd[index];
                clock_gettime(CLOCK_MONOTONIC, &curtime);
                ptHandle->connet_time[index] = (uint32)curtime.tv_sec;

                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = channel;
                ptIPCMsg->protocol = PROT_CONTINUE_CHANNLE;
                dataLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("%s set channel %d work\n", ifName, channel);
            }

            goto end;
        }

        if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
            && PRO_HEART_FRAME != protocol)
        {
            goto cont;  //其他方式不发送
        }

        /* 防止60s临界时间关闭导致不发送 */
        if(MODE_MIX == ptEthCfg->workmode && fdSocket < 0)
        {
            ptHandle->client_fd[index] = comm_connect_socket(ifName, ptHandle->masterIP[index], 
                                                             ptHandle->masterPort[index]);  /*连接指定IP的端口号*/
            if(ptHandle->client_fd[index] <= 0)
            {
                COMM_FMT_DEBUG("%s connect channel %d failed\n", ifName, channel);
                *pNextState = ETH_CONNECT_MASTER;
                goto end;
            }
            fdSocket = ptHandle->client_fd[index];
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            ptHandle->connet_time[index] = (uint32)curtime.tv_sec;

            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CONTINUE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("%s set channel %d work\n", ifName, channel);
        }
            
        COMM_BUF_DEBUG(dataBuf, dataLen, "%s send%s", ifName, fdSocket > 0 ? "" : " drop");
        if(fdSocket > 0)
        {
            sendLen = send(fdSocket, dataBuf+4, dataLen-4, 0);
            if(sendLen > 0)
            {
                outlen += sendLen;
            }
            else
            {
                usleep(10000);
                sendLen = send(fdSocket, dataBuf+4, dataLen-4, 0);
                if(sendLen > 0)
                {
                    outlen += sendLen;
                }
                else
                {
                    COMM_FMT_DEBUG("%s channel %d send msg failed, ret %d\n", ifName, channel, sendLen);
                }
            }
            if(MODE_MIX == ptEthCfg->workmode)
            {
                ptHandle->connet_time[index] = (uint32)curtime.tv_sec;
            }
        }
        
cont:
        bzero(dataBuf, dataLen);
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    }
end:
    return outlen;
}

/**********************************************************************
* @name      : comm_eth_send_proxy
* @brief     ：eth网口发消息 非服务器方式
* @param[in] ：uint8 ifuint                网口编号
               char *ifName                网口名称
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：发送长度/-1报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2019-12-10
**********************************************************************/
int comm_eth_send_proxy(uint8 ifuint, char *ifName, COMM_ETH_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 index;
    int   fdSocket = 0;
    uint8  channel = 0;
    uint8 protocol = -1;         /* 内部协议 */
    int    sendLen = 0;
    int     outlen = 0;
    uint16  dataLen = 0;
    uint8  *dataBuf = NULL;   /* 缓冲区 */
    COMM_IPCMSG_T  *ptIPCMsg = NULL;

    OOP_ETHCONFIG_T *ptEthCfg = &(ptHandle->tEth.tEthCfg);

    *pNextState = ETH_RECV_DATA;

    if(TYPE_TCP == ptEthCfg->contype && MODE_SERVER == ptEthCfg->workmode) //服务器模式不支持
    {
        return 0;
    }
    dataBuf = g_ETHdataBuf[ifuint];
    dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    while(dataLen)
    {
        if(dataLen <= 4)
        {
            COMM_FMT_TRACE("comm_ppp_send msg less then 4\n");
            goto cont;
        }
        channel = *dataBuf;
        protocol = comm_get_protocol(dataBuf);

        if(PROT_CLOSE_CHANNLE == protocol)
        {
            COMM_FMT_DEBUG("%s close channel %d\n", ifName, channel);
            
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }

            goto cont;
        }

        if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
            && PRO_HEART_FRAME != protocol)
        {
            goto cont;  //其他方式不发送
        }

        fdSocket = eth_find_clientfd(channel, ptHandle->client_fd, &index);
        COMM_BUF_DEBUG(dataBuf, dataLen, "%s send:", ifName);

        sendLen = comm_proxy_send(fdSocket, dataBuf+4, dataLen-4, 0);
        if(sendLen > 0)
        {
            outlen += sendLen;
        }
        else if(!g_proxyOK)
        {
            *pNextState = ETH_CONNECT_MASTER;
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("%s close channel %d\n", ifName, channel);
        }
cont:
        bzero(dataBuf, dataLen);
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    }
    return outlen;
}

/**********************************************************************
* @name      : comm_ethserver_send
* @brief     ：eth网口发消息 服务器方式
* @param[in] ：uint8 *pBuffer                缓冲区
               COMM_QUEUE_LIST_T *ptSendList 消息列表
               COMM_ETH_HANDLE_T *ptHandle   线程私有数据
* @param[out]：uint32 *pSendLen              发送数据量
* @return    ：0成功/错误码, 报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
int comm_ethserver_send(char *ifName, COMM_QUEUE_LIST_T *ptSendList, COMM_ETH_HANDLE_T *ptHandle, uint32 *pSendLen)
{
    int     sendLen = 0;
    uint16  dataLen = 0;
    uint8  *dataBuf = NULL;   /* 缓冲区 */
    uint8   protocol = -1;               /* 内部协议 */
    uint8   channel = 0;
    int     fd = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    
    dataBuf = g_ETHdataBuf[ptHandle->no];
    dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    while(dataLen)
    {
        if(dataLen <= 4)
        {
            COMM_FMT_TRACE("comm_ppp_send msg less then 4\n");
            goto cont;
        }
                
        channel  = dataBuf[0];
        protocol = comm_get_protocol(dataBuf);
        if(PROT_CLOSE_CHANNLE == protocol)
        {
            eth_close_acceptfd(channel, &(ptHandle->accept_fd[0][0]));
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            goto cont;
        }
        if(PRO_DATA_TRANSMIT != protocol)
        {
            /* 服务器模式, 不主动发送 预连接操作 */
            goto cont;
        }
        
        fd = eth_find_acceptfd(channel, &(ptHandle->accept_fd[0][0]));
        if(fd > 0)
        {
            sendLen = send(fd, dataBuf+4, dataLen-4, 0);
            COMM_BUF_DEBUG(dataBuf, dataLen, "%s send(%d)", ifName, sendLen);
            if(sendLen > 0)
            {
                *pSendLen += sendLen;
            }
        }
cont:
        bzero(dataBuf, dataLen);
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    }
    return 0;
}

/**********************************************************************
* @name      : comm_eth_udpsend
* @brief     ：eth网口发消息 udp方式
* @param[in] ：int fd                      socket句柄
               uint8 *pBuffer              缓冲区
               uint16 bufLen               缓冲区大小
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：发送长度/-1报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
int comm_eth_udpsend(int fd, uint8 *pBuffer, uint16 bufLen, COMM_ETH_HANDLE_T *ptHandle)
{
    uint8   offset = -1;
    uint8 protocol = -1;         /* 内部协议 */
    int    sendLen = 0;
    struct sockaddr_in address;
    
    if(bufLen <= 4)
    {
        COMM_FMT_TRACE("comm_eth_udpsend msg less then 4\n");
        return 0;
    }

    protocol = comm_get_protocol(pBuffer);
    /*如果是数据中转传送、心跳帧、登陆帧、退出登陆帧, 将数据发送出去*/
    if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
        && PRO_HEART_FRAME != protocol)
    {
        return 0;  //其他方式不发送
    }

    offset = eth_get_channeloffset(*pBuffer);
    bzero(&address, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = htonl(ptHandle->masterIP[offset]);
    address.sin_port        = htons(ptHandle->masterPort[offset]);
    sendLen = sendto(fd, pBuffer+4, bufLen-4, 0, (struct sockaddr *)&address, sizeof(address));
    if(sendLen < 0)
    {
        return -1;
    }
    return sendLen;
}

/**********************************************************************
* @name      : comm_eth_recv
* @brief     ：eth网口收消息 非服务器方式
* @param[in] ：uint8 ifuint                网口编号
               char *ifName                网口名称
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：数据长度/-1 网口异常
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    : 2019-12-10
**********************************************************************/
int comm_eth_recv(uint8 ifuint, char *ifName, COMM_ETH_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 i;
    int recvLen = 0;
    int outlen = 0;
    uint8  *dataBuf = NULL;   /* 缓冲区 不进行初始化, 提高性能 */
    uint16  bufLen = 0;
    struct timespec curtime = {0};
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    OOP_ETHCONFIG_T *ptEthCfg = &(ptHandle->tEth.tEthCfg);
    uint8   channel = 0;
    
    *pNextState = ETH_SEND_DATA;
    if(MODE_SERVER == ptEthCfg->workmode)            //服务器模式不处理
    {
        return 0;
    }
    dataBuf = g_ETHdataBuf[ifuint];
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        recvLen = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(ptHandle->client_fd[i] <= 0 && MODE_CLIENT == ptEthCfg->workmode)
        {
            if(curtime.tv_sec - ptHandle->connet_time[i] >= 5)
            {
                /* 掉线的主站, 5s连接一次 */
                *pNextState = ETH_CONNECT_MASTER;
            }
            continue;
        }
        channel = (0 == ifuint ? CHL_ETH_1 : CHL_ETH_2) + i;
        
        if(ptHandle->client_fd[i] > 0 && MODE_MIX == ptEthCfg->workmode)
        {
            /* 混合模式 没有通信 1min主动关闭链接 */
            if(curtime.tv_sec - ptHandle->connet_time[i] >= 60)
            {
                close(ptHandle->client_fd[i]);
                ptHandle->client_fd[i] = -1;
                
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = channel;
                ptIPCMsg->protocol = PROT_PAUSE_CHANNLE;
                bufLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("%s %s channel %d\n", ifName, "pause", channel);
                return outlen;
            }
        }
        recvLen = recv(ptHandle->client_fd[i], dataBuf, BUF_LENGTH, 0);    /*socket进行数据接收*/
        if(recvLen > 0)
        {
            outlen += recvLen;
            comm_add_channel(channel, dataBuf, recvLen);
            bufLen = recvLen + 4;
            COMM_BUF_DEBUG(dataBuf, bufLen, "%s recv", ifName);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
            {
                COMM_FMT_DEBUG("Msg Queue is full\n");
            }
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
        else if(recvLen <= 0 && (curtime.tv_sec - ptHandle->connet_time[i] > 5) && MODE_CLIENT == ptEthCfg->workmode)
        {
            struct tcp_info info;
            int len = sizeof(info);
            getsockopt(ptHandle->client_fd[i], IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
            //printf("tcp state %d\n", info.tcpi_state);
            if(TCP_CLOSE_WAIT == info.tcpi_state || TCP_CLOSE == info.tcpi_state)
            {
                close(ptHandle->client_fd[i]);
                ptHandle->client_fd[i] = -1;
                /* 关闭通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = channel;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                bufLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("Server close the connect, channel %d\n", channel);
            }
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
    }

    return outlen;
}

/**********************************************************************
* @name      : comm_eth_recv_proxy
* @brief     ：eth网口收消息 非服务器方式
* @param[in] ：uint8 ifuint                网口编号
               char *ifName                网口名称
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：数据长度/-1 网口异常
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    : 2019-12-10
**********************************************************************/
int comm_eth_recv_proxy(uint8 ifuint, char *ifName, COMM_ETH_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 i;
    int recvLen = 0;
    int outlen = 0;
    uint8  *dataBuf = NULL;   /* 缓冲区 不进行初始化, 提高性能 */
    uint16  bufLen = 0;
    struct timespec curtime = {0};
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    OOP_ETHCONFIG_T *ptEthCfg = &(ptHandle->tEth.tEthCfg);

    *pNextState = ETH_SEND_DATA;
    if(MODE_SERVER == ptEthCfg->workmode)            //服务器模式不处理
    {
        return 0;
    }
    dataBuf = g_ETHdataBuf[ifuint];
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        recvLen = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(!g_proxyOK)
        {
            if(curtime.tv_sec - ptHandle->connet_time[i] > 30)
            {
                /* 掉线的主站, 30s连接一次 */
                *pNextState = ETH_CONNECT_MASTER;
            }
            continue;
        }
        recvLen = comm_proxy_recv(ptHandle->client_fd[i], dataBuf, BUF_LENGTH, 0);    /*socket进行数据接收*/
        if(recvLen > 0)
        {
            outlen += recvLen;
            comm_add_channel((0 == ifuint ? CHL_ETH_1 : CHL_ETH_2) + i, dataBuf, recvLen);
            bufLen = recvLen + 4;
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_BUF_DEBUG(dataBuf, bufLen, "%s recv:", ifName);
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
        else if(recvLen <= 0 && (curtime.tv_sec - ptHandle->connet_time[i] > 5))
        {
            if(!g_proxyOK)
            {
                /* 关闭通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = (0 == ifuint ? CHL_ETH_1 : CHL_ETH_2) + i;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                bufLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("Server close the connect, channel %d\n", ptIPCMsg->id);
            }
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
    }

    return outlen;
}


/**********************************************************************
* @name      : comm_ethserver_recv
* @brief     ：eth网口收消息 服务器方式
* @param[in] ：uint8 ifunit                   网口编号
               char *ifName                   网口名称
               COMM_QUEUE_LIST_T *ptRecvList  消息列表
               COMM_QUEUE_LIST_T *ptRecvList1 消息列表1
               COMM_ETH_HANDLE_T *ptHandle    线程私有数据
* @param[out]：uint32 *pRecvLen               接收数据量
* @return    ：0成功/错误码, 报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
int comm_ethserver_recv(uint8 ifunit, char *ifName, COMM_QUEUE_LIST_T *ptRecvList, COMM_QUEUE_LIST_T *ptRecvList1,
                        COMM_ETH_HANDLE_T *ptHandle, uint32 *pRecvLen)
{
    int     recvLen = 0;
    uint8   i,j;
    fd_set  fdsr;
    int     maxsock;
    int     ret = 0;
    struct timeval tv = {0,0};      //超时时间设为0 是非阻塞
    struct sockaddr_in client_addr; // connector's address information
    uint8  *dataBuf = NULL;   /* 缓冲区 */
    int     new_fd = -1;
    socklen_t sin_size = sizeof(client_addr);
    uint8   offset = 0;
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;

    dataBuf = g_ETHdataBuf[ifunit];
    for(i = 0; i < ptHandle->listenNum; i++)
    {
        if(ptHandle->listen_fd[i] <= 0)
        {
            continue;
        }
        maxsock = ptHandle->listen_fd[i];

        /* 0 初始化 */
        FD_ZERO(&fdsr);
        FD_SET(maxsock, &fdsr);
        
        /* 1 向fd数组中增加连接 */
        for (j = 0; j < ETH_CONNECT_NUM; j++) 
        {
            if(ptHandle->accept_fd[i][j] > 0) 
            {
                FD_SET(ptHandle->accept_fd[i][j], &fdsr);
                if(maxsock < ptHandle->accept_fd[i][j])
                {
                    maxsock = ptHandle->accept_fd[i][j];
                }
            }
        }
        ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
        if (ret <= 0) 
        {
            continue; //错误或超时继续
        }

        /* 2 接收数据 */
        for (j = 0; j < ETH_CONNECT_NUM; j++) 
        {
            if(ptHandle->accept_fd[i][j] <= 0)
            {
                continue;
            }
            if(FD_ISSET(ptHandle->accept_fd[i][j], &fdsr))
            {
                memset(dataBuf, 0, BUF_LENGTH);
                recvLen = recv(ptHandle->accept_fd[i][j], dataBuf, BUF_LENGTH, 0);
                if(recvLen <= 0)
                {
                    close(ptHandle->accept_fd[i][j]);
                    FD_CLR(ptHandle->accept_fd[i][j], &fdsr);
                    ptHandle->accept_fd[i][j] = -1;

                    /* 关闭通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    offset = i *j + OOP_MAX_HOST;
                    ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + offset;
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    recvLen = sizeof(COMM_IPCMSG_T);
                    if(0 != comm_write_queue(ptRecvList, dataBuf, recvLen))
                    {
                        if(0 != comm_write_queue(ptRecvList1, dataBuf, recvLen))
                        {
                            COMM_FMT_TRACE("Msg Queue is full\n");
                        }
                    }
                    COMM_FMT_DEBUG("client[%d %d], channel id %d close\n", i, j, ptIPCMsg->id);
                }
                else
                {
                    *pRecvLen += recvLen;
                    offset = i *j + OOP_MAX_HOST;
                    comm_add_channel((0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + offset, dataBuf, recvLen);
                    recvLen += 4;
                    if(0 != comm_write_queue(ptRecvList, dataBuf, recvLen))
                    {
                        if(0 != comm_write_queue(ptRecvList1, dataBuf, recvLen))
                        {
                            COMM_FMT_TRACE("Eth Msg Queue is full\n");
                        }
                    }
                    COMM_BUF_DEBUG(dataBuf, recvLen, "%s recv:", ifName);
                }
                /* 延时 */
                usleep(10);
            }
        }

        /* 3 查看是否有新的连接 */
        if(FD_ISSET(ptHandle->listen_fd[i], &fdsr))
        {
            new_fd = accept(ptHandle->listen_fd[i], (struct sockaddr *)&client_addr, &sin_size);
            if (new_fd <= 0) 
            {
                COMM_FMT_TRACE("%s accept error\n", ifName);
                continue;
            }
            for(j = 0; j < ETH_CONNECT_NUM; j++)
            {
                if(ptHandle->accept_fd[i][j] <= 0)
                {
                    ptHandle->accept_fd[i][j] = new_fd;
                    /* 开启通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    offset = i *j + OOP_MAX_HOST;
                    ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + offset;
                    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                    ptChanInfo->ip = ntohl(client_addr.sin_addr.s_addr);
                    ptChanInfo->port = ntohs(client_addr.sin_port);
                    ptChanInfo->workmode = MODE_SERVER;
                    ptChanInfo->medium = COMM_MEDIA_ETH;
                    ptChanInfo->connType = TYPE_TCP;
                    ptChanInfo->bFirst = TRUE;
                    ptChanInfo->heart = ptHandle->tEth.tEthCfg.heart;
                    ptChanInfo->dailcnt = ptHandle->tEth.tEthCfg.dail.mask.dailcnt;
                    ptChanInfo->dailtime = ptHandle->tEth.tEthCfg.dail.mask.dailtime;
                    recvLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    if(0 != comm_write_queue(ptRecvList, dataBuf, recvLen))
                    {
                        if(0 != comm_write_queue(ptRecvList1, dataBuf, recvLen))
                        {
                            COMM_FMT_TRACE("Msg Queue is full\n");
                        }
                    }
                    COMM_FMT_DEBUG("client[%d %d], channel id %d open\n", i, j, ptIPCMsg->id);
                    break;
                }
            }
            if(ETH_CONNECT_NUM == j)
            {
                close(new_fd);
            }
        }
    }

    return 0;
}

/**********************************************************************
* @name      : comm_eth_udprecv
* @brief     ：eth网口收消息 udp方式
* @param[in] ：int fd                      socket句柄
               uint16 bufLen               缓冲区大小
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pBuffer              缓冲区
               uint8 *pOffset              通道偏移
* @return    ：数据长度/-1 网口异常
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
int comm_eth_udprecv(int fd, uint8 *pBuffer, uint16 bufLen, COMM_ETH_HANDLE_T *ptHandle, uint8 *pOffset)
{
    uint8 i;
    int recvLen = 0;
    int sin_len;
    uint32 ip = 0;
    uint16 port = 0;
    struct sockaddr_in sourceAddr;

    /* UDP模式 */
    sin_len = sizeof(struct sockaddr);
    recvLen = recvfrom(fd, pBuffer, bufLen, 0, (struct sockaddr *)&sourceAddr, (socklen_t *)&sin_len);
    if(recvLen > 0)
    {
        ip = ntohl(sourceAddr.sin_addr.s_addr);
        port = ntohs(sourceAddr.sin_port);
        /* UDP模式 检查主站 */
        for(i = 0; i < ptHandle->masterNUM; i++)
        {
            if(ptHandle->masterIP[i] == ip && ptHandle->masterPort[i] == port)
            {
                *pOffset = i;
                return recvLen;
            }
        }
    }
    return -1;
}

/**********************************************************************
* @name      : comm_eth_free_allfd
* @brief     ：清空所有文件/sicket句柄
* @param[in] ：COMM_ETH_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-13
* @Update    :
**********************************************************************/
void comm_eth_free_allfd(COMM_ETH_HANDLE_T *ptHandle)
{
    uint8 i;
    
    for(i = 0; i < OOP_MAX_HOST; i++)
    {
        if(ptHandle->client_fd[i] > 0)
        {
            close(ptHandle->client_fd[i]);
            ptHandle->client_fd[i] = -1;
        }
        if(ptHandle->temp_fd[i] > 0)
        {
            close(ptHandle->temp_fd[i]);
            ptHandle->temp_fd[i] = -1;
        }
    }
    ptHandle->masterNUM = 0;
}

/**********************************************************************
* @name      : comm_eth_working
* @brief     ：以太网线程收发  非服务器模式
* @param[in] ：uint8 ifuint                网口编号
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2019-12-10
**********************************************************************/
int comm_eth_working(uint8 ifunit, COMM_ETH_HANDLE_T *ptHandle)
{
    uint8  i;
    int    fdSocket = -1;             /* socket 连接操作符 */    
    uint8  stateEth = ETH_SEND_DATA; /* 状态机 */
    char  ifName[24] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    OOP_ETHCONFIG_T *ptEthCfg = NULL;
    OOP_MASTERPARAMS_T *ptMaster = NULL;
    uint8 dataBuf[BUF_LENGTH] = {0};  /* 缓冲区 */
    uint16            dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */
    uint8  connecttime = 0;

    if(NULL == ptHandle || ifunit > 1)
    {
        return -1;
    }

    /* 先释放上次没有来及释放的句柄 */
    comm_eth_free_allfd(ptHandle);

    ptEthCfg = &(ptHandle->tEth.tEthCfg);
    ptMaster = &(ptHandle->tEth.tMaster);

    memcpy(ifName, g_ETHName[ifunit] + 1, g_ETHName[ifunit][0]);

    if(MODE_SERVER == ptEthCfg->workmode && TYPE_TCP == ptEthCfg->contype)
    {
        COMM_FMT_TRACE("comm_eth_working not support server\n");
        return -1;    
    }
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    /* 初始化 */
    ptHandle->masterNUM = 0;
    if(MODE_ZB == ptEthCfg->conmode)  //主备模式
    {
        if(ptHandle->masterStart >= ptMaster->nNum)
        {
            ptHandle->masterStart = 0;
        }
        for(i = ptHandle->masterStart; i < ptMaster->nNum; i++)
        {  //顺序链接
            *ptHandle->pRunFlag = 0;
            ptHandle->masterIP[0] = htonl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->masterPort[0] = ptMaster->master[i].port;
            if(0 == ptHandle->masterIP[0] || 0 == ptHandle->masterPort[0])
            {
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d错误\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[0]), 
                                                             ptHandle->masterPort[0]);
                continue;
            }
            ptHandle->masterNUM++;
            //以太网不设置路由 同网段设置正确掩码,跨网段使用网关
            eth_add_route(ifName, ptMaster->master[i].ip, ptHandle);  
            fdSocket = comm_connect_socket(ifName, ptHandle->masterIP[0], ptHandle->masterPort[0]);
            if(fdSocket > 0)
            {
                ptHandle->client_fd[0] = fdSocket;
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d成功\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[0]), 
                                                             ptHandle->masterPort[0]);
                break;
            }
            usleep(100000);
            ptHandle->masterNUM--;
            comm_del_route_net(ifName, ptMaster->master[i].ip);    
        }
        if(i == ptMaster->nNum)
        {  //从头开始链接
            for(i = 0; i < ptHandle->masterStart; i++)
            {
                *ptHandle->pRunFlag = 0;
                ptHandle->masterIP[0] = htonl(*((uint32 *)ptMaster->master[i].ip));
                ptHandle->masterPort[0] = ptMaster->master[i].port;
                if(0 == ptHandle->masterIP[0] || 0 == ptHandle->masterPort[0])
                {
                    COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d错误\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[0]), 
                                                                 ptHandle->masterPort[0]);
                    continue;
                }
                ptHandle->masterNUM++;
                eth_add_route(ifName, ptMaster->master[i].ip, ptHandle);
                fdSocket = comm_connect_socket(ifName, ptHandle->masterIP[0], ptHandle->masterPort[0]);
                if(fdSocket > 0)
                {
                    ptHandle->client_fd[0] = fdSocket;
                    COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d成功\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[0]), 
                                                             ptHandle->masterPort[0]);
                    break;
                }
                usleep(100000);
                ptHandle->masterNUM--;
                comm_del_route_net(ifName, ptMaster->master[i].ip);    
            }
        }
        if(0 == ptHandle->masterNUM)
        {
            COMM_FMT_DEBUG("%s connect no master\n", ifName);
            return -1;
        }
    }
    else
    {
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ptHandle->masterIP[ptHandle->masterNUM] = htonl(*(uint32 *)ptMaster->master[i].ip);
            ptHandle->masterPort[ptHandle->masterNUM] = ptMaster->master[i].port;
            if(0 == ptHandle->masterIP[ptHandle->masterNUM] || 0 == ptHandle->masterPort[ptHandle->masterNUM])
            {
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d错误\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[ptHandle->masterNUM]), 
                                                                 ptHandle->masterPort[ptHandle->masterNUM]);
                continue;
            }
            eth_add_route(ifName, ptMaster->master[i].ip, ptHandle);
            fdSocket = comm_connect_socket(ifName, ptHandle->masterIP[ptHandle->masterNUM], 
                                           ptHandle->masterPort[ptHandle->masterNUM]);
            usleep(100000);
            if(fdSocket > 0)
            {
                ptHandle->client_fd[ptHandle->masterNUM] = fdSocket;
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d成功\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[ptHandle->masterNUM]), 
                                                             ptHandle->masterPort[ptHandle->masterNUM]);
            }
            ptHandle->connet_time[ptHandle->masterNUM] = (uint32)oldtime.tv_sec;
            ptHandle->masterNUM++;
        }
        if(ptHandle->client_fd[0] <= 0 && ptHandle->client_fd[1] <= 0 && 
           ptHandle->client_fd[2] <= 0 && ptHandle->client_fd[3] <= 0)
        {
            COMM_FMT_DEBUG("%s连接主站失败\n", ifName);
            return -1;
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = ptEthCfg->workmode;
    ptChanInfo->medium = COMM_MEDIA_ETH;
    ptChanInfo->connType = ptEthCfg->contype;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = ptEthCfg->heart;
    ptChanInfo->dailcnt = ptEthCfg->dail.mask.dailcnt;
    ptChanInfo->dailtime = ptEthCfg->dail.mask.dailtime;
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
        ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
        ptChanInfo->ip = ptHandle->masterIP[i];
        ptChanInfo->port = ptHandle->masterPort[i];
        if(ptHandle->client_fd[i] > 0)
        {
            ptHandle->connet_time[i] = oldtime.tv_sec;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("%s打开通道%d\n", ifName, ptIPCMsg->id);
        }
    }
    memset(dataBuf, 0, BUF_LENGTH);
    dataLen = 0;
    ptHandle->bChangeParam = FALSE;

    /* 设置心跳周期 */
    if(NULL != ptHandle->MixHeart)
    {
        *(ptHandle->MixHeart) = MODE_MIX == ptEthCfg->workmode ? ptEthCfg->heart : 0;
    }

    while(1)
    {
        *ptHandle->pRunFlag = 0;

        if(FALSE == ptHandle->bEthLink || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
        {
            COMM_FMT_DEBUG("%s退出工作，需要重连\n", ifName);
            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            for(i = 0; i < ptHandle->masterNUM; i++)
            {
                if(ptHandle->client_fd[i] > 0 || MODE_MIX == ptEthCfg->workmode)
                {
                    /* 关闭所有通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    dataLen = sizeof(COMM_IPCMSG_T);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                    {
                        COMM_FMT_TRACE("Msg Queue is full\n");
                    }
                    COMM_FMT_DEBUG("%s关闭通道%d\n", ifName, ptIPCMsg->id);
                    memset(dataBuf, 0, BUF_LENGTH);
                    usleep(10000);
                }
                if(0 != ptHandle->masterIP[i])
                {
                    comm_del_iproute_net(ifName, ptHandle->masterIP[i]);
                }
            }
            *(ptHandle->MixHeart) = 0;
            comm_eth_free_allfd(ptHandle);
            return -1;
        }
        if(TRUE == ptHandle->bChangeParam)
        {
            ptHandle->bChangeParam = FALSE;
            /* 发送数据更新 */
            for(i = 0; i < ptHandle->masterNUM; i++)
            {
                if(ptHandle->client_fd[i] > 0)
                {
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = ptEthCfg->workmode;
                    ptChanInfo->medium = COMM_MEDIA_ETH;
                    ptChanInfo->connType = ptEthCfg->contype;
                    ptChanInfo->bFirst = FALSE;
                    ptChanInfo->heart = ptEthCfg->heart;
                    ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
                    ptIPCMsg->protocol = PRO_PARA_MOD;
                    ptChanInfo->ip = ptHandle->masterIP[i];
                    ptChanInfo->port = ptHandle->masterPort[i];
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s通道%d参数变化\n", ifName, ptIPCMsg->id);
                    memset(dataBuf, 0, BUF_LENGTH);
                    usleep(100000);
                }
            }

            if(NULL != ptHandle->MixHeart)
            {
                *(ptHandle->MixHeart) = MODE_MIX == ptEthCfg->workmode ? ptEthCfg->heart : 0;
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            comm_flow_add(FALSE, ifunit, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;
            
            memcpy(&oldtime, &curtime, sizeof(struct timespec));
        }
        
        switch(stateEth)
        {
            case ETH_CONNECT_MASTER:
            {
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->client_fd[i] <= 0)
                    {
                        ptHandle->client_fd[i] = comm_connect_socket_noblock(ifName, ptHandle->masterIP[i], 
                                                                             ptHandle->masterPort[i], 
                                                                             &(ptHandle->temp_fd[i]),
                                                                             &(ptHandle->temp_time[i]));
                        if(ptHandle->client_fd[i] > 0)
                        {
                            /* 通知主应用调度线程通道重新打开 */
                            dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                            ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                            ptChanInfo->workmode = ptEthCfg->workmode;
                            ptChanInfo->medium = COMM_MEDIA_ETH;
                            ptChanInfo->connType = ptEthCfg->contype;
                            ptChanInfo->bFirst = FALSE;
                            ptChanInfo->heart = ptEthCfg->heart;
                            ptChanInfo->dailcnt = ptEthCfg->dail.mask.dailcnt;
                            ptChanInfo->dailtime = ptEthCfg->dail.mask.dailtime;
                            ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
                            ptIPCMsg->protocol = (MODE_CLIENT == ptEthCfg->workmode)?PROT_OPEN_CHANNLE : PRO_PARA_MOD;
                            ptChanInfo->ip = ptHandle->masterIP[i];
                            ptChanInfo->port = ptHandle->masterPort[i];
                            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                            COMM_FMT_DEBUG("%s channel id %d open\n", ifName, ptIPCMsg->id);
                            memset(dataBuf, 0, BUF_LENGTH);
                            usleep(100000);
                        }
                        ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
                    }
                }
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->client_fd[i] > 0)
                    {
                        stateEth = ETH_SEND_DATA;
                        connecttime = 0;
                        goto nextstatus;
                    }
                }
                /* socket全0 */
                if(connecttime++ < 30)
                {
                    sleep(2);
                    stateEth = ETH_CONNECT_MASTER;
                }
                else
                {
                    *ptHandle->pAppReStart = 1;
                    COMM_FMT_DEBUG("%s connect failed for 30 times, restart!\n", ifName);
                }
nextstatus:
                break;
            }
            case ETH_SEND_DATA:
            {
                applyTxFlow += comm_eth_send(ifunit, ifName, ptHandle, &stateEth);
                break;
            }
            case ETH_RECV_DATA:
            {
                applyRxFlow += comm_eth_recv(ifunit, ifName, ptHandle, &stateEth);
                break;
            }
            case ETH_CLOSE_DEVICE:
            {
                *ptHandle->pAppReStart = 1;
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX-3;
                return -1;
            }
            default:
            {
                stateEth = ETH_SEND_DATA;
                break;
            }
        }
        usleep(1000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

/**********************************************************************
* @name      : comm_eth_proxy
* @brief     ：ppp网口代理 非服务器模式
* @param[in] ：uint8 ifuint                网口编号
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-31
* @Update    : 2019-12-10
**********************************************************************/
int comm_eth_proxy(uint8 ifunit, COMM_ETH_HANDLE_T *ptHandle)
{
    uint8  i;
    uint8  stateEth = ETH_SEND_DATA; /* 状态机 */
    char  ifName[24] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    OOP_ETHCONFIG_T *ptEthCfg = NULL;
    uint8 dataBuf[BUF_LENGTH] = {0};  /* 缓冲区 */
    uint16            dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */

    if(NULL == ptHandle || ifunit > 1)
    {
        return -1;
    }

    ptEthCfg = &(ptHandle->tEth.tEthCfg);
    memcpy(ifName, g_ETHName[ifunit] + 1, g_ETHName[ifunit][0]);
    
    /* 先释放上次没有来及释放的句柄 */
    comm_eth_free_allfd(ptHandle);

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    //等待通信代理连接到主站
    while(g_proxyOK == FALSE)
    {
        *ptHandle->pRunFlag = 0;
        usleep(100000);       
    }

    ptHandle->connet_time[ptHandle->masterNUM] = (uint32)oldtime.tv_sec;

    //TODO 当前方案，无法获取连接的主站数量，默认写1
    ptHandle->masterNUM = 1;
    
    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = ptEthCfg->workmode;
    ptChanInfo->medium = COMM_MEDIA_ETH;
    ptChanInfo->connType = ptEthCfg->contype;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = ptEthCfg->heart;
    ptChanInfo->dailcnt = ptEthCfg->dail.mask.dailcnt;
    ptChanInfo->dailtime = ptEthCfg->dail.mask.dailtime;
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
        ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
        ptChanInfo->ip = ptHandle->masterIP[i];
        ptChanInfo->port = ptHandle->masterPort[i];

       comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
    }
    memset(dataBuf, 0, BUF_LENGTH);
    dataLen = 0;
    ptHandle->bChangeParam = FALSE;

    while(1)
    {
        *ptHandle->pRunFlag = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            comm_flow_add(FALSE, ifunit, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;
            
            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(FALSE == ptHandle->bEthLink || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
            {
                COMM_FMT_DEBUG("%s no link or appmain restart\n", ifName);
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;

                /* 关闭所有通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + 0;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                dataLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("%s channel id %d close\n", ifName, ptIPCMsg->id);
                memset(dataBuf, 0, BUF_LENGTH);
                usleep(10000);
    
                comm_eth_free_allfd(ptHandle);
                return -1;
            }
            if(TRUE == ptHandle->bChangeParam)
            {
                ptHandle->bChangeParam = FALSE;
                /* 发送数据更新 */

                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = ptEthCfg->workmode;
                ptChanInfo->medium = COMM_MEDIA_ETH;
                ptChanInfo->connType = ptEthCfg->contype;
                ptChanInfo->bFirst = FALSE;
                ptChanInfo->heart = ptEthCfg->heart;
                ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + 0;
                ptIPCMsg->protocol = PRO_PARA_MOD;
                ptChanInfo->ip = ptHandle->masterIP[i];
                ptChanInfo->port = ptHandle->masterPort[i];
                comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                COMM_FMT_DEBUG("%s channel id %d change\n", ifName, ptIPCMsg->id);
                memset(dataBuf, 0, BUF_LENGTH);
                usleep(100000);
            }
        }
        
        switch(stateEth)
        {
            case ETH_CONNECT_MASTER:
            {
                if(g_proxyOK == TRUE)
                {
                    /* 通知主应用调度线程通道重新打开 */
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = ptEthCfg->workmode;
                    ptChanInfo->medium = COMM_MEDIA_ETH;
                    ptChanInfo->connType = ptEthCfg->contype;
                    ptChanInfo->bFirst = FALSE;
                    ptChanInfo->heart = ptEthCfg->heart;
                    ptChanInfo->dailcnt = ptEthCfg->dail.mask.dailcnt;
                    ptChanInfo->dailtime = ptEthCfg->dail.mask.dailtime;
                    ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + 0;
                    ptIPCMsg->protocol = (MODE_CLIENT == ptEthCfg->workmode)?PROT_OPEN_CHANNLE : PRO_PARA_MOD;
                    ptChanInfo->ip = ptHandle->masterIP[i];
                    ptChanInfo->port = ptHandle->masterPort[i];
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s channel id %d open\n", ifName, ptIPCMsg->id);
                    memset(dataBuf, 0, BUF_LENGTH);
                    usleep(100000);

                    ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
                    stateEth = ETH_SEND_DATA;
                }
                else
                {
                    stateEth = ETH_CONNECT_MASTER;
                }
                break;
            }

            case ETH_SEND_DATA:
            {
                applyTxFlow += comm_eth_send_proxy(ifunit, ifName, ptHandle, &stateEth);
                break;
            }
            case ETH_RECV_DATA:
            {
                applyRxFlow += comm_eth_recv_proxy(ifunit, ifName, ptHandle, &stateEth);
                break;
            }
            case ETH_CLOSE_DEVICE:
            {
                stateEth = ETH_CONNECT_MASTER;
                return -1;

            }
            default:
            {
                stateEth = ETH_SEND_DATA;
                break;
            }
        }
        usleep(10000);
    }
}

/**********************************************************************
* @name      : comm_eth_proxy_working
* @brief     ：以太网线程收发 通信代理SSAL
* @param[in] ：uint8 ifuint                网口编号
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-9-10
* @Update    : 
**********************************************************************/
int comm_eth_proxy_working(uint8 ifunit, COMM_ETH_HANDLE_T *ptHandle)
{
    uint8  i;
    int    fdSocket = -1;             /* socket 连接操作符 */    
    uint8  stateEth = ETH_SEND_DATA; /* 状态机 */
    char  ifName[24] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    OOP_ETHCONFIG_T *ptEthCfg = NULL;
    OOP_MASTERPARAMS_T *ptMaster = NULL;
    uint8 dataBuf[BUF_LENGTH] = {0};  /* 缓冲区 */
    uint16            dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */
    uint8  connecttime = 0;

    if(NULL == ptHandle || ifunit > 1)
    {
        return -1;
    }

    ptHandle->bProxySSAL = 0;
    /* 先释放上次没有来及释放的句柄 */
    comm_eth_free_allfd(ptHandle);

    ptEthCfg = &(ptHandle->tEth.tEthCfg);
    ptMaster = &(ptHandle->tEth.tMaster);

    memcpy(ifName, g_ETHName[ifunit] + 1, g_ETHName[ifunit][0]);

    if((MODE_SERVER == ptEthCfg->workmode && TYPE_TCP == ptEthCfg->contype) || MODE_ZB != ptEthCfg->conmode)
    {
        COMM_FMT_TRACE("comm_eth_proxy_working not support \n");
        return -1;    
    }
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    /* 初始化 */
    ptHandle->masterNUM = 0;

    /* 连接前置机(代理) */
    ptHandle->proxyip = htonl(*((uint32 *)ptEthCfg->proxyip));
    ptHandle->proxyport = ptEthCfg->proxyport;
    
    eth_add_route(ifName, ptEthCfg->proxyip, ptHandle);  
    fdSocket = comm_connect_socket(ifName, ptHandle->proxyip, ptHandle->proxyport);
    if(fdSocket > 0)
    {
        ptHandle->client_fd[0] = fdSocket;
        COMM_FMT_DEBUG("%s connect procy 0x%x, port %d\n", ifName, ptHandle->proxyip, ptHandle->proxyport);
    }
    else
    {
        COMM_FMT_DEBUG("%s connect procy failed\n", ifName);
        comm_del_route_net(ifName, ptEthCfg->proxyip);    
        return -1;
    }

    ptHandle->bProxySSAL = 1;

    /* 首先选择一个主 */
    if(ptHandle->masterStart >= ptMaster->nNum)
    {
        ptHandle->masterStart = 0;
    }
    ptHandle->masterIP[0] = htonl(*((uint32 *)ptMaster->master[ptHandle->masterStart].ip));
    ptHandle->masterPort[0] = ptMaster->master[ptHandle->masterStart].port;
    ptHandle->masterNUM = 1;

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = ptEthCfg->workmode;
    ptChanInfo->medium = COMM_MEDIA_ETH;
    ptChanInfo->connType = ptEthCfg->contype;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = ptEthCfg->heart;
    ptChanInfo->dailcnt = ptEthCfg->dail.mask.dailcnt;
    ptChanInfo->dailtime = ptEthCfg->dail.mask.dailtime;
    ptChanInfo->proxyip = ptHandle->proxyip;
    ptChanInfo->proxyPort = ptHandle->proxyport;
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
        ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
        ptChanInfo->ip = ptHandle->masterIP[i];
        ptChanInfo->port = ptHandle->masterPort[i];
        if(ptHandle->client_fd[i] > 0)
        {
            ptHandle->connet_time[i] = oldtime.tv_sec;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
        }
    }
    memset(dataBuf, 0, BUF_LENGTH);
    dataLen = 0;
    ptHandle->bChangeParam = FALSE;

    while(1)
    {
        *ptHandle->pRunFlag = 0;

        if(FALSE == ptHandle->bEthLink || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
        {
            COMM_FMT_DEBUG("%s no link or app need restart\n", ifName);
            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            for(i = 0; i < ptHandle->masterNUM; i++)
            {
                if(ptHandle->client_fd[i] > 0)
                {
                    /* 关闭所有通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    dataLen = sizeof(COMM_IPCMSG_T);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                    {
                        COMM_FMT_TRACE("Msg Queue is full\n");
                    }
                    COMM_FMT_DEBUG("%s channel id %d close\n", ifName, ptIPCMsg->id);
                    memset(dataBuf, 0, BUF_LENGTH);
                    usleep(10000);
                }
            }
            comm_del_iproute_net(ifName, ptHandle->proxyip);
            *(ptHandle->MixHeart) = 0;
            comm_eth_free_allfd(ptHandle);
            return -1;
        }
        if(TRUE == ptHandle->bChangeParam)
        {
            ptHandle->bChangeParam = FALSE;
            /* 发送数据更新 */
            for(i = 0; i < ptHandle->masterNUM; i++)
            {
                if(ptHandle->client_fd[i] > 0)
                {
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = ptEthCfg->workmode;
                    ptChanInfo->medium = COMM_MEDIA_ETH;
                    ptChanInfo->connType = ptEthCfg->contype;
                    ptChanInfo->bFirst = FALSE;
                    ptChanInfo->heart = ptEthCfg->heart;
                    ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
                    ptIPCMsg->protocol = PRO_PARA_MOD;
                    ptChanInfo->ip = ptHandle->masterIP[i];
                    ptChanInfo->port = ptHandle->masterPort[i];
                    ptChanInfo->proxyip = ptHandle->proxyip;
                    ptChanInfo->proxyPort = ptHandle->proxyport;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s channel id %d change\n", ifName, ptIPCMsg->id);
                    memset(dataBuf, 0, BUF_LENGTH);
                    usleep(100000);
                }
            }

            if(NULL != ptHandle->MixHeart)
            {
                *(ptHandle->MixHeart) = MODE_MIX == ptEthCfg->workmode ? ptEthCfg->heart : 0;
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            comm_flow_add(FALSE, ifunit, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;
            
            memcpy(&oldtime, &curtime, sizeof(struct timespec));
        }
        
        switch(stateEth)
        {
            case ETH_CONNECT_MASTER:
            {
                if(ptHandle->client_fd[0] <= 0)
                {
                    ptHandle->client_fd[0] = comm_connect_socket_noblock(ifName, ptHandle->proxyip, ptHandle->proxyport,
                                                                         &(ptHandle->temp_fd[0]),
                                                                         &(ptHandle->temp_time[0]));
                    if(ptHandle->client_fd[0] > 0)
                    {
                        /* 通知主应用调度线程通道重新打开 */
                        dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                        ptChanInfo->workmode = ptEthCfg->workmode;
                        ptChanInfo->medium = COMM_MEDIA_ETH;
                        ptChanInfo->connType = ptEthCfg->contype;
                        ptChanInfo->bFirst = FALSE;
                        ptChanInfo->heart = ptEthCfg->heart;
                        ptChanInfo->dailcnt = ptEthCfg->dail.mask.dailcnt;
                        ptChanInfo->dailtime = ptEthCfg->dail.mask.dailtime;
                        ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + 0;
                        ptIPCMsg->protocol = (MODE_CLIENT == ptEthCfg->workmode)?PROT_OPEN_CHANNLE : PRO_PARA_MOD;
                        ptChanInfo->ip = ptHandle->masterIP[0];
                        ptChanInfo->port = ptHandle->masterPort[0];
                        ptChanInfo->proxyip = ptHandle->proxyip;
                        ptChanInfo->proxyPort = ptHandle->proxyport;
                        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                        COMM_FMT_DEBUG("%s channel id %d open\n", ifName, ptIPCMsg->id);
                        memset(dataBuf, 0, BUF_LENGTH);
                        usleep(100000);
                    }
                    ptHandle->connet_time[0] = (uint32)curtime.tv_sec;
                }
                if(ptHandle->client_fd[0] > 0)
                {
                    stateEth = ETH_SEND_DATA;
                    connecttime = 0;
                    goto nextstatus;
                }
                /* socket全0 */
                if(connecttime++ < 60)
                {
                    sleep(1);
                    stateEth = ETH_CONNECT_MASTER;
                }
                else
                {
                    *ptHandle->pAppReStart = 1;
                    COMM_FMT_DEBUG("%s connect failed for 60 times, restart!\n", ifName);
                }
nextstatus:
                break;
            }
            case ETH_SEND_DATA:
            {
                applyTxFlow += comm_eth_send(ifunit, ifName, ptHandle, &stateEth);
                break;
            }
            case ETH_RECV_DATA:
            {
                applyRxFlow += comm_eth_recv(ifunit, ifName, ptHandle, &stateEth);
                break;
            }
            case ETH_CLOSE_DEVICE:
            {
                *ptHandle->pAppReStart = 1;
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX-3;
                return -1;
            }
            default:
            {
                stateEth = ETH_SEND_DATA;
                break;
            }
        }
        usleep(10000);
    }
}


/**********************************************************************
* @name      : comm_eth_serverworking
* @brief     ：以太网线程收发  服务器模式
* @param[in] ：uint8 ifuint                网口编号
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
int comm_eth_serverworking(uint8 ifunit, COMM_ETH_HANDLE_T *ptHandle)
{
    uint8  stateEth = ETH_SEND_DATA; /* 状态机 */
    char  ifName[24] = {0};
    OOP_ETHCONFIG_T *ptEthCfg = NULL;
    uint32           rxtxLen = 0;
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    uint8  i, j;
    uint64 applyRxFlow = 0;   /* 应用层流量 */
    uint64 applyTxFlow = 0;   /* 应用层流量 */

    if(NULL == ptHandle || ifunit > 1)
    {
        return -1;
    }
    
    /* 先释放上次没有来及释放的句柄 */
    eth_close_listen(ptHandle);

    ptEthCfg = &(ptHandle->tEth.tEthCfg);
    memcpy(ifName, g_ETHName[ifunit] + 1, g_ETHName[ifunit][0]);

    ptHandle->listenNum = ptEthCfg->listenNum;
    *ptHandle->pRunFlag = 0;

    /* 建立监听句柄 */
    if(0 != eth_open_listen(ifName, ptEthCfg->listen, ptHandle))
    {
        *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
        return -1;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            comm_flow_add(FALSE, ifunit, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;
            
            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(FALSE == ptHandle->bEthLink || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
            {
                COMM_FMT_DEBUG("%s no link or appmain restart\n", ifName);
                stateEth = ETH_CLOSE_DEVICE;
            }

            if(TRUE == ptHandle->bChangeParam)
            {
                ptHandle->bChangeParam = FALSE;
                uint8           dataBuf[100];   /* 缓冲区 */
                uint8           offset = 0;
                uint8           recvLen = 0;
                CHANNEL_INFO_T *ptChanInfo = 0;
                COMM_IPCMSG_T  *ptIPCMsg = NULL;
                /* 发送数据更新 */
                for(i = 0; i < OOP_MAX_LISTENS; i++)
                {
                    for(j = 0; j < ETH_CONNECT_NUM; j++)
                    {
                        if(ptHandle->accept_fd[i][j] > 0)
                        {
                            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                            ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                            offset = i *j + OOP_MAX_HOST;
                            ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + offset;
                            ptIPCMsg->protocol = PRO_PARA_MOD;
                            ptChanInfo->ip = ptHandle->accept_ip[i][j];
                            ptChanInfo->port = ptHandle->accept_port[i][j];
                            ptChanInfo->workmode = MODE_SERVER;
                            ptChanInfo->medium = COMM_MEDIA_ETH;
                            ptChanInfo->connType = TYPE_TCP;
                            ptChanInfo->bFirst = TRUE;
                            ptChanInfo->heart = ptHandle->tEth.tEthCfg.heart;
                            recvLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, recvLen))
                            {
                                if(0 != comm_write_queue(ptHandle->ptRecvListBak, dataBuf, recvLen))
                                {
                                    COMM_FMT_TRACE("Msg Queue is full\n");
                                }
                            }
                            COMM_FMT_DEBUG("client[%d %d], channel id %d change\n", i, j, ptIPCMsg->id);
                        }
                    }
                }
            }

        }

        switch(stateEth)
        {
            case ETH_SEND_DATA:
            {
                /* 需要将所有的消息都发送出去 */
                if(0 != comm_ethserver_send(ifName, ptHandle->ptSendList, ptHandle, &rxtxLen))
                {
                    stateEth = ETH_CLOSE_DEVICE;
                    break;
                }
                applyTxFlow += rxtxLen;
                stateEth = ETH_RECV_DATA;
                break;
            }
            case ETH_RECV_DATA:
            {
                if(0 != comm_ethserver_recv(ifunit, ifName, ptHandle->ptRecvList, 
                                            ptHandle->ptRecvListBak, ptHandle, &rxtxLen))
                {
                    stateEth = ETH_CLOSE_DEVICE;
                    break;
                }
                applyRxFlow += rxtxLen;
                stateEth = ETH_SEND_DATA;
                break;
            }
            case ETH_CLOSE_DEVICE:
            {
                uint8   i,j;
                uint8   dataBuf[32] = {0};   /* 缓冲区 */
                COMM_IPCMSG_T  *ptIPCMsg = NULL;
                uint8   offset;
                uint16  recvLen;
                for(i = 0; i < OOP_MAX_LISTENS; i++)
                {
                    for(j = 0; j < ETH_CONNECT_NUM; j++)
                    {
                        if(ptHandle->accept_fd[i][j] <= 0)
                        {
                            continue;
                        }
                        /* 关闭通道 */
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        offset = i *j + OOP_MAX_HOST;
                        ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + offset;
                        ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                        recvLen = sizeof(COMM_IPCMSG_T);
                        if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, recvLen))
                        {
                            if(0 != comm_write_queue(ptHandle->ptRecvListBak, dataBuf, recvLen))
                            {
                                COMM_FMT_TRACE("Msg Queue is full\n");
                            }
                        }
                        COMM_FMT_DEBUG("%s channel id %d close\n", ifName, ptIPCMsg->id);
                        usleep(10000);
                    }
                }

                eth_close_listen(ptHandle);
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
                return -1;
            }
            default:
            {
                stateEth = ETH_SEND_DATA;
                break;
            }
        }
        usleep(10000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

/**********************************************************************
* @name      : comm_eth_udpworking
* @brief     ：以太网线程收发  UDP方式
* @param[in] ：uint8 ifuint                网口编号
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
int comm_eth_udpworking(uint8 ifunit, COMM_ETH_HANDLE_T *ptHandle)
{
    uint8  i;
    int    fdSocket = -1;             /* socket 连接操作符 */    
    uint8  stateEth = ETH_SEND_DATA; /* 状态机 */
    char  ifName[24] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    OOP_ETHCONFIG_T *ptEthCfg = NULL;
    OOP_MASTERPARAMS_T *ptMaster = NULL;
    uint8 dataBuf[BUF_LENGTH] = {0};  /* 缓冲区 */
    uint16            dataLen = 0;
    int           sendRecvLen = 0;
    uint8              offset = -1;

    CHANNEL_INFO_T    *ptChanInfo = 0;
    COMM_IPCMSG_T       *ptIPCMsg = NULL;
    
    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */

    if(NULL == ptHandle || ifunit > 1)
    {
        return -1;
    }

    /* 先释放上次没有来及释放的句柄 */
    comm_eth_free_allfd(ptHandle);

    ptEthCfg = &(ptHandle->tEth.tEthCfg);
    ptMaster = &(ptHandle->tEth.tMaster);
    memcpy(ifName, g_ETHName[ifunit] + 1, g_ETHName[0][0]);

    if(TYPE_TCP == ptEthCfg->contype)
    {
        COMM_FMT_TRACE("comm_eth_working not support TCP\n");
        return -1;    
    }
    
    /* 初始化 */
    ptHandle->masterNUM = 0;
    if(MODE_ZB == ptEthCfg->conmode)  //主备模式
    {
        ptHandle->masterNUM = 1;
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ptHandle->masterIP[0] = ntohl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->masterPort[0] = ptMaster->master[i].port;
            eth_add_route(ifName, ptMaster->master[i].ip, ptHandle);
            
            fdSocket = comm_init_udp(ifName);
            if(fdSocket > 0)
            {
                COMM_FMT_DEBUG("%s connect master 0x%x, port %d\n", ifName, ptHandle->masterIP, ptHandle->masterPort);
                break;
            }
            usleep(100000);
        }
    }
    else  //多连接方式
    {
        for(i = 0; i < ptMaster->nNum; i++)
        {
            ptHandle->masterNUM++;
            *ptHandle->pRunFlag = 0;
            ptHandle->masterIP[i] = ntohl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->masterPort[i] = ptMaster->master[i].port;
            eth_add_route(ifName, ptMaster->master[i].ip, ptHandle);
        }
        fdSocket = comm_init_udp(ifName);
        usleep(100000);
    }

    if(fdSocket <= 0)
    {
        COMM_FMT_DEBUG("%s Create socekt failed\n", ifName);
        *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
        return -1;
    }
    ptHandle->client_fd[0] = fdSocket;

    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = ptEthCfg->workmode;
    ptChanInfo->medium = COMM_MEDIA_ETH;
    ptChanInfo->connType = ptEthCfg->contype;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = ptEthCfg->heart;
    ptChanInfo->dailcnt = ptEthCfg->dail.mask.dailcnt;
    ptChanInfo->dailtime = ptEthCfg->dail.mask.dailtime;
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
        ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
        ptChanInfo->ip = ptHandle->masterIP[i];
        ptChanInfo->port = ptHandle->masterPort[i];
        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
        COMM_FMT_DEBUG("%s channel id %d open\n", ifName, ptIPCMsg->id);
    }
    memset(dataBuf, 0, BUF_LENGTH);
    dataLen = 0;
    ptHandle->bChangeParam = FALSE;

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;

        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            comm_flow_add(FALSE, ifunit, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;

            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(FALSE == ptHandle->bEthLink || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
            {
                COMM_FMT_DEBUG("%s no link or appmain restart\n", ifName);
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->client_fd[i] > 0)
                    {
                        /* 关闭所有通道 */
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
                        ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                        dataLen = sizeof(COMM_IPCMSG_T);
                        if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                        {
                            COMM_FMT_TRACE("Msg Queue is full\n");
                        }
                        COMM_FMT_DEBUG("%s channel id %d close\n", ifName, ptIPCMsg->id);
                        memset(dataBuf, 0, BUF_LENGTH);
                        usleep(10000);
                    }
                }
                comm_eth_free_allfd(ptHandle);
                return -1;
            }
            if(TRUE == ptHandle->bChangeParam)
            {
                ptHandle->bChangeParam = FALSE;
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->client_fd[i] > 0)
                    {
                        dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                        ptChanInfo->workmode = ptEthCfg->workmode;
                        ptChanInfo->medium = COMM_MEDIA_ETH;
                        ptChanInfo->connType = ptEthCfg->contype;
                        ptChanInfo->bFirst = TRUE;
                        ptChanInfo->heart = ptEthCfg->heart;
                        ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
                        ptIPCMsg->protocol = PRO_PARA_MOD;
                        ptChanInfo->ip = ptHandle->masterIP[i];
                        ptChanInfo->port = ptHandle->masterPort[i];
                        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                        COMM_FMT_DEBUG("%s channel id %d change\n", ifName, ptIPCMsg->id);
                        usleep(100000);
                    }
                }
            }
        }

        switch(stateEth)
        {
            case ETH_CONNECT_MASTER:
            {
                fdSocket = comm_init_udp(ifName);
                COMM_FMT_TRACE("%s comm_init_udp %d\n", ifName, fdSocket);
                if(fdSocket > 0)
                {
                    ptHandle->client_fd[0] = fdSocket;
                    stateEth = ETH_SEND_DATA;
                }
                else
                {
                    sleep(1);
                }
                break;
            }
            case ETH_SEND_DATA:
            {
                dataLen = (uint16)comm_read_queue(ptHandle->ptSendList, dataBuf);
                if(dataLen > 0)
                {
                    sendRecvLen = comm_eth_udpsend(fdSocket, dataBuf, dataLen, ptHandle);
                    COMM_BUF_DEBUG(dataBuf, dataLen, "%s send(%d):", ifName, sendRecvLen);
                    if(sendRecvLen <0)
                    {
                        stateEth = ETH_CLOSE_DEVICE;
                        break;
                    }
                    applyTxFlow += sendRecvLen;
                    memset(dataBuf, 0, BUF_LENGTH);
                }
                stateEth = ETH_RECV_DATA;
                break;
            }
            case ETH_RECV_DATA:
            {
                sendRecvLen = comm_eth_udprecv(fdSocket, dataBuf, BUF_LENGTH-4, ptHandle, &offset);
                if(sendRecvLen > 0)  //写入接受队列
                {
                    dataLen = (uint16)sendRecvLen;
                    applyRxFlow += dataLen;
                    /*给数据加上会话协议头部*/
                    comm_add_channel(((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + offset, dataBuf, dataLen);
                    dataLen += 4;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_BUF_DEBUG(dataBuf, dataLen, "%s recv:", ifName);
                    memset(dataBuf, 0, BUF_LENGTH);
                }
                stateEth = ETH_SEND_DATA;
                break;
            }
            case ETH_CLOSE_DEVICE:
            {
                close(ptHandle->client_fd[0]);
                ptHandle->client_fd[0] = -1;
                stateEth = ETH_CONNECT_MASTER;
                break;
            }
            default:
            {
                stateEth = ETH_SEND_DATA;
                break;
            }
        }
        usleep(10000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

/**********************************************************************
* @name      : comm_set_ethname
* @brief     ：设置以太网口名称
* @param[in] ：uint8 ifno    网口编号
               char *ifname  名称
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-3-17
* @Update    :
**********************************************************************/
void comm_set_ethname(uint8 ifno, char *ifname)
{
    if(ifno < 2 && NULL != ifname)
    {
        g_ETHName[ifno][0] = MIN(23, strlen(ifname));
        memcpy(g_ETHName[ifno] + 1, ifname, MIN(23, strlen(ifname)));
    }
}

/**********************************************************************
* @name      : comm_timer_getETH
* @brief     ：网口插拔查询消息定时器
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-3-17
* @Update    :
**********************************************************************/
void comm_timer_getETH(int socketfd, void *pArg)
{
    int  ret = 0;
    uint8 len = 0;
    static uint8 s_ethFirst[2] = {TRUE, TRUE};
    static time_t s_ethTime[2] = {0};
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;
    struct timespec curtime = {0};

    if(socketfd <= 0 || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_timer_getETH input null\n");
        return;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    if(g_ETHName[ptHandle->no][0] > 0 && (TRUE == s_ethFirst[ptHandle->no] || curtime.tv_sec > s_ethTime[ptHandle->no] + 30))
    {
        g_ETHStatus[ptHandle->no] = &(ptHandle->bEthLink);

        len = g_ETHName[ptHandle->no][0] + 1;
        ret = task_send_labelmsg(socketfd, MSG_COMM_GETETH, ptHandle->no+1, g_ETHName[ptHandle->no], len, MSG_MONITOR);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Comm send get eth status %d msg, ret %d\n", ptHandle->no+1, ret);
        }
        else
        {
            s_ethFirst[ptHandle->no] = FALSE;
        }
        s_ethTime[ptHandle->no] = curtime.tv_sec;

    }
    else if(1 == g_ETHOnlineCHeck[ptHandle->no])
    {
        len = g_ETHName[ptHandle->no][0] + 1;
        ret = task_send_labelmsg(socketfd, MSG_COMM_GETETH, ptHandle->no+1, g_ETHName[ptHandle->no], len, MSG_MONITOR);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Comm send get eth status %d msg, ret %d\n", ptHandle->no+1, ret);
        }
        g_ETHOnlineCHeck[ptHandle->no] = 0;
        s_ethTime[ptHandle->no] = curtime.tv_sec;
    }
}

/**********************************************************************
* @name      : comm_timer_mastergui
* @brief     ：主站链接定时同时显示
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-7-1
* @Update    :
**********************************************************************/
void comm_timer_mastergui(int socketfd, void *pArg)
{
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;
    uint8              buf[8] = {0};
    uint16             len = 0;
    int                ret = 0;
    static uint8       s_eFirstStatus = 1;

    if(socketfd <= 0 || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_timer_mastergui input null\n");
        return;
    }

    if(1 == ptHandle->bMasterGui)
    {
        buf[len++] = 1;
        buf[len++] = 2;
        buf[len++] = 1;
    }
    else if(2 == ptHandle->bMasterGui || 1 == s_eFirstStatus)
    {
        buf[len++] = 1;
        buf[len++] = 2;
        buf[len++] = 0;
        s_eFirstStatus = 0;
    }
    else
    {
        return;
    }

    ret = comm_send_label_msg(MSG_COMM_GUITOOLS, MSG_COMM_FLAG + ptHandle->no, buf, len);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("Send gui tool 2 failed\n");
    }
}

/**********************************************************************
* @name      : comm_eth_statusmsg
* @brief     ：以太网口连接消息解析
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-3-17
* @Update    : 
**********************************************************************/
int comm_eth_statusmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    int status;
    uint8 bLink;
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;
    
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_eth_statusmsg input null\n");
        return -1;
    }

    appmsg_memcpy_r(&status, playload, sizeof(int));
   
    if(1 == status)
    {
        bLink = TRUE;
    }
    else
    {
        bLink = FALSE;
    }
    if(bLink != ptHandle->bEthLink)
    {
        ptHandle->bEthLink = bLink;
        COMM_FMT_DEBUG("comm_eth_statusmsg eth %d status %d\n", label, status);
    }
    return 0;
}

/**********************************************************************
* @name      : comm_eth_eventmsg
* @brief     ：以太网口连接通知消息解析
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-3-19
* @Update    : 
**********************************************************************/
int comm_eth_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    uint8 status;
    uint8 ifname[24] = {0};
    uint8 offset = 0;
    uint8 i;

    if(NULL == playload)
    {
        COMM_FMT_DEBUG("comm_eth_eventmsg input null\n");
        return -1;
    }

    ifname[offset++] = *playload;
    memcpy(ifname + offset, playload + offset, ifname[0]);
    offset += ifname[0];
    status = playload[offset];

    COMM_FMT_DEBUG("comm_eth_eventmsg %s status %d\n", ifname + 1, status);
    for(i = 0; i < 2; i++)
    {
        if(0 == memcmp(ifname, g_ETHName[i], 24) && NULL != g_ETHStatus[i])
        {
            if(1 == status)
            {
                *(g_ETHStatus[i]) = TRUE;
            }
            else
            {
                *(g_ETHStatus[i]) = FALSE;
            } 
        }
    }
    
    return 0;
}

extern void comm_add_writedb(uint32 oad, uint8 *buf, uint16 len);

/**********************************************************************
* @name      : comm_eth_eventmsg
* @brief     ：以太网口ip修改
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-3-19
* @Update    : 
**********************************************************************/
int comm_eth_ipchangemsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    uint8 ifname[24] = {0};
    uint8 offset = 0;
    uint8 ip[4] = {0};
    uint8 mask[4] = {0};
    uint8 getway[4] = {0};
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;

    if(NULL == playload)
    {
        COMM_FMT_DEBUG("comm_eth_eventmsg input null\n");
        return -1;
    }

    ifname[offset++] = *playload;
    memcpy(ifname + offset, playload + offset, ifname[0]);
    offset += ifname[0];
    
    if(4 != playload[offset++])
    {
        COMM_FMT_DEBUG("%s change ip msg error\n", ifname + 1);
        return -2;
    }
    memcpy(ip, playload + offset, 4);
    offset += 4;

    if(4 != playload[offset++])
    {
        COMM_FMT_DEBUG("%s change ip msg error\n", ifname + 1);
        return -3;
    }
    memcpy(mask, playload + offset, 4);
    offset += 4;

    if(4 != playload[offset++])
    {
        COMM_FMT_DEBUG("%s change ip msg error\n", ifname + 1);
        return -5;
    }
    memcpy(getway, playload + offset, 4);
    offset += 4;

    COMM_FMT_DEBUG("%s change ip %u.%u.%u.%u, mask %u.%u.%u.%u, getway %u.%u.%u.%u\n", ifname + 1, ip[0], ip[1], ip[2], 
                   ip[3], mask[0], mask[1], mask[2], mask[3], getway[0], getway[1], getway[2], getway[3]);

    OOP_ETHIP_T newIP;
    memset(&newIP, 0, sizeof(OOP_ETHIP_T));
    newIP.ipConfig = ptHandle->tEth.tEthIP.ipConfig;
    memcpy(newIP.ip, ip, 4);
    memcpy(newIP.mask, mask, 4);
    memcpy(newIP.ipGateway, getway, 4);

    if(0 == memcmp(ifname, g_ETHName[0], 24) && NULL != g_ETHStatus[0])
    {
        uint8 flag = 0;
        *(g_ETHStatus[0]) = FALSE;
        /* 等待断开 重新查询一下以太网插拔状态 */
        //comm_add_writedb(0x45100400, (uint8 *)&newIP, sizeof(OOP_ETHIP_T));
        sleep(4);
        comm_add_writedb(0x451004FF, &flag, sizeof(uint8));
        g_ETHOnlineCHeck[0] = 1;
    }
    
    return 0;
}

/**********************************************************************
* @name      : comm_eth_guishow
* @brief     ：连接状态更新通知
* @param[in] ：uint8 id       通道
               uint8 protocol 内部协议     
               void *pArg     私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-4-1
* @Update    : 2020-4-27
**********************************************************************/
int comm_eth_guishow(uint8 id, uint8 protocol, void *pArg)
{
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;
    OOP_ETHCONFIG_T   *ptEthCfg = NULL;
    uint8              num = 0;
    uint8              buf[52] = {0};
    uint16             len = 0;
    int                ret = 0;
    uint8 i, j;
    uint32             outtime = 120;

    if(NULL == pArg)
    {
        return -1;
    }

    if(0 != ptHandle->no)
    {
        /* 当前没有以太网2的连接状态 */
        return 0;
    }
    ptEthCfg = &(ptHandle->tEth.tEthCfg);

    /* 刷新状态栏 前4个字节填写120 是时间 */
    appmsg_memcpy_r(buf, &outtime, sizeof(uint32));
    if(PROT_OPEN_ACK == protocol)
    {
        len = sprintf((char *)buf + 5, "%s", "以太网连接主站");
    }
    else if(PROT_CLOSE_ACK == protocol)
    {
        len = sprintf((char *)buf + 5, "%s", "以太网断开主站连接");
    }
    buf[4] = len;
    if(len > 0)
    {
        ret = comm_send_msg(MSG_COMM_GUISTATUS, buf, len + 5);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Send gui \"%s\" failed\n", buf + 5);
        }
    }


    /* 刷新工具栏 */
    sleep(1); //等待句柄释放
    memset(buf, 0, 48);
    len = 0;
    if(PROT_OPEN_ACK == protocol)
    {
        buf[len++] = 1;
        buf[len++] = 2;
        buf[len++] = 1;
    }
    else if(PROT_CLOSE_ACK == protocol)
    {
        if(MODE_SERVER == ptEthCfg->workmode)  //服务器模式
        {
            /* 获取连接数量 */
            for(i = 0; i < OOP_MAX_LISTENS; i++)
            {
                for(j = 0; j < ETH_CONNECT_NUM; j++)
                {
                    if(ptHandle->accept_fd[i][j] > 0)
                    {
                        num++;
                    }
                }
            }
        }
        else
        {
            for(i = 0; i < ptHandle->masterNUM; i++)
            {
                if(ptHandle->client_fd[i] > 0)
                {
                    num++;
                }
            }
        }

        if(0 < num) //还存在连接不刷新工具栏
        {
            return 0;
        }
        buf[len++] = 1;
        buf[len++] = 2;
        buf[len++] = 0;
    }

    if(len > 0)
    {
        /* 链接上主站 持续发送 */
        ptHandle->bMasterGui = (PROT_OPEN_ACK == protocol) ? 1 : 0;
        ret = comm_send_msg(MSG_COMM_GUITOOLS, buf, len);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Send gui tool %d failed\n", buf[len - 1]);
        }
    }
    
    return ret;
}

/**********************************************************************
* @name      : comm_getcheck_ethcfg
* @brief     ：获取并检查eth配置是否发生变化
* @param[in] ：uint8 ifuint      接口编号
               void *dbHandle    数据中心句柄
               void *pCfg        配置存储地址
* @param[out]：uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
int comm_getcheck_ethcfg(uint8 ifuint, int sockdbfd, void *pCfg, uint8 *pRestart)
{
    uint8 i;
    int retEthIP = 0;
    int retEth = 0;
    int retMaster = 0;
    uint32 readLen = 0;
    uint8  bChange = FALSE;
    uint8  bRestart = FALSE;
    uint8  bEthSet = FALSE;
    NOMAL_OAD_T        tOAD;
    OOP_ETHCONFIG_T    tEth;
    OOP_MASTERPARAMS_T tMaster;
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pCfg;
    static uint8 s_ethcfg[2][3] = {{0}, {0}};  //首次获取失败打印，防止日志过多
    static uint8 s_setIP[2] = {FALSE};

    if(ifuint > 1 || sockdbfd < 0 || NULL == pCfg)
    {
        return -1;
    }

    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = (0 == ifuint) ? 0x451004FE : 0x451104FE;
    db_read_nomal(sockdbfd, &tOAD, sizeof(uint8), &bEthSet, &readLen);
    
    tOAD.oad.value = (0 == ifuint) ? 0x45100400 : 0x45110400;
    memset(&(ptHandle->tEth.tEthIP), 0, sizeof(OOP_ETHIP_T));
    retEthIP = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_ETHIP_T), (uint8 *)&(ptHandle->tEth.tEthIP), &readLen);
    if(0 == retEthIP && readLen == sizeof(OOP_ETHIP_T) && (FALSE == s_setIP[ifuint] || FALSE != bEthSet))
    {
        uint8 typeFlag = 0;
        tOAD.oad.value = (0 == ifuint) ? 0x451004FF : 0x451104FF;
        db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_ETHIP_T), &typeFlag, &readLen);
        if(1 == typeFlag)
        {
            char *iptype[3] = {"DHCP", "static", "PPPoE"};
            sleep(3); /* 主站操作延时一段时间 */
            *ptHandle->pAppReStart = 1;
            sleep(2);
            COMM_FMT_DEBUG("Set %s ip(%s) addr %d.%d.%d.%d\n", g_ETHName[ifuint] + 1, 
                           ptHandle->tEth.tEthIP.ipConfig <= 2 ? iptype[ptHandle->tEth.tEthIP.ipConfig] : "unknow",
                           ptHandle->tEth.tEthIP.ip[0], ptHandle->tEth.tEthIP.ip[1], ptHandle->tEth.tEthIP.ip[2],
                           ptHandle->tEth.tEthIP.ip[3]);
            ipconfig_send_to_smiOS((char *)g_ETHName[ifuint] + 1, ptHandle->tEth.tEthIP.ipConfig, ptHandle->tEth.tEthIP.ip, 
                                   ptHandle->tEth.tEthIP.mask, ptHandle->tEth.tEthIP.ipGateway);
        }
        tOAD.classtag = CLASS_DATA_UNINIT;
        typeFlag = 0;
        db_write_nomal(sockdbfd, &tOAD, &typeFlag, 1);
        tOAD.oad.value -= 1;
        db_write_nomal(sockdbfd, &tOAD, &typeFlag, 1);
    }
    s_setIP[ifuint] = TRUE;

    tOAD.oad.value = (0 == ifuint) ? 0x45100200 : 0x45110200;
    memset(&tEth, 0, sizeof(OOP_ETHCONFIG_T));
    retEth = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_ETHCONFIG_T), (uint8 *)&tEth, &readLen);
    if(0 == retEth)
    {
        if(readLen != sizeof(OOP_ETHCONFIG_T))
        {
            retEth = -1;
        }
        else if(0 != memcmp(&(ptHandle->tEth.tEthCfg), &tEth, readLen))
        {
            bChange = TRUE;
        }
    }
    else if(FALSE == s_ethcfg[ifuint][0])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retEth);
        s_ethcfg[ifuint][0] = TRUE;
    }

    tOAD.oad.value = (0 == ifuint) ? 0x45100300 : 0x45110300;
    memset(&tMaster, 0, sizeof(OOP_MASTERPARAMS_T));
    retMaster = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_MASTERPARAMS_T), (uint8 *)&tMaster, &readLen);
    if(0 == retMaster)
    {
        if(readLen != sizeof(OOP_MASTERPARAMS_T))
        {
            //DPRINT("Read 0x%x error\n", tOAD.oad.value);
            retMaster = -1;
        }
        else if(0 != memcmp(&(ptHandle->tEth.tMaster), &tMaster, readLen))
        {
            bChange = TRUE;
            /* 删除以太网的路由配置, 防止残留 */
            for(i = 0; i < tMaster.nNum; i++)
            {
                comm_del_route_net((char *)g_ETHName[ifuint] + 1, tMaster.master[i].ip);
            }
        }
    }
    else if(FALSE == s_ethcfg[ifuint][1])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retMaster);
        s_ethcfg[ifuint][1] = TRUE;
    }

    if(TRUE == bChange)
    {
        if(0 == retEth)
        {
            if(ptHandle->tEth.tEthCfg.workmode != tEth.workmode || 
               ptHandle->tEth.tEthCfg.contype != tEth.contype ||
               ptHandle->tEth.tEthCfg.conmode != tEth.conmode)
            {
                bRestart = TRUE;
            }
            else
            {
                ptHandle->bChangeParam = TRUE;
            }
            memcpy(&(ptHandle->tEth.tEthCfg), &tEth, sizeof(tEth));
        }
        if(0 == retMaster)
        {
            if(0 != memcmp(&(ptHandle->tEth.tMaster), &tMaster, sizeof(tMaster)))
            {
                bRestart = TRUE;
            }
            memcpy(&(ptHandle->tEth.tMaster), &tMaster, sizeof(tMaster));
        }
    }

    if(TRUE == bRestart)
    {
        sleep(5); //等待5s后重启
        if(TRUE == ptHandle->bGetCfg)
        {
            *ptHandle->pAppReStart = 1;
            sleep(5);
        }
        if(pRestart)
        {
            *pRestart = TRUE;
        }
    }

    if(0 == retEth && 0 == retMaster)
    {
        ptHandle->bGetCfg = TRUE;
    }
    return retEth | retMaster;
}

/**********************************************************************
* @name      : comm_getcheck_eth0cfg
* @brief     ：获取并检查eth0配置是否发生变化
* @param[in] ：int sockdbfd      通信句柄
               void *pCfg        配置存储地址
* @param[out]：uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
int comm_getcheck_eth0cfg(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    return comm_getcheck_ethcfg(0, sockdbfd, pCfg, pRestart);
}

/**********************************************************************
* @name      : comm_getcheck_eth1cfg
* @brief     ：获取并检查eth1配置是否发生变化
* @param[in] ：int sockdbfd      通信句柄
* @param[out]：void *pCfg        配置存储地址
               uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
int comm_getcheck_eth1cfg(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    return comm_getcheck_ethcfg(1, sockdbfd, pCfg, pRestart);
}

/**********************************************************************
* @name      : comm_eth_start
* @brief     ：以太网口启动判断
* @param[in] ：
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2020-3-17
**********************************************************************/
BOOLEAN comm_eth_start(void *pArg)
{
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;

    if(NULL == pArg || 0 == *ptHandle->pIPCok)
    {
        return FALSE;
    }

    if(FALSE == ptHandle->bGetCfg || FALSE == ptHandle->bEthLink) //没有获取到配置 或 没有接网线
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
* @name      : comm_eth0_entry
* @brief     ：eth0端口消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2019-12-9
**********************************************************************/
void *comm_eth0_entry(void *pArg)
{
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;
    OOP_ETHCONFIG_T   *ptEthCfg = NULL;
    //OOP_ETHIP_T       *ptEthIP = NULL;
    //OOP_ETHMAC_T      *ptEthMac = NULL;
    if(NULL == pArg)
    {
        return 0;
    }

    nice(-5); /*提高优先级*/

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    /* 清空队列 */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptRecvListBak);
    comm_clean_queue(ptHandle->ptSendList);

    ptEthCfg = &(ptHandle->tEth.tEthCfg);
    //ptEthIP  = &(ptHandle->tEth.tEthIP);
    //ptEthMac = &(ptHandle->tEth.tEthMac);
    
    /* 设置网口配置 */
    //comm_set_ifnet("eth0", ptEthIP->ip, ptEthIP->mask, ptEthIP->ipGateway, ptEthMac->data);

    COMM_FMT_DEBUG("eth0 start: contype(%d), workmode(%d).\n", ptEthCfg->contype, ptEthCfg->workmode);
//    if(1 == ssal_get_status())
//    {
//        comm_eth_proxy_working(0, ptHandle);
//    }
//    else
    {
        if(TYPE_UDP == ptEthCfg->contype)
        {
            comm_eth_udpworking(0, ptHandle);
        }
        else if(MODE_SERVER == ptEthCfg->workmode)  //服务器模式
        {
            comm_eth_serverworking(0, ptHandle);
        }
        else
        {
            comm_eth_working(0, ptHandle);
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : comm_eth1_entry
* @brief     ：eth1端口消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2019-12-9
**********************************************************************/
void *comm_eth1_entry(void *pArg)
{
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;
    OOP_ETHCONFIG_T   *ptEthCfg = NULL;
    //OOP_ETHIP_T       *ptEthIP = NULL;
    //OOP_ETHMAC_T      *ptEthMac = NULL;
    if(NULL == pArg)
    {
        return 0;
    }

    nice(-5); /*提高优先级*/
    
    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    /* 清空队列 */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptRecvListBak);
    comm_clean_queue(ptHandle->ptSendList);
     
    ptEthCfg = &(ptHandle->tEth.tEthCfg);
    //ptEthIP  = &(ptHandle->tEth.tEthIP);
    //ptEthMac = &(ptHandle->tEth.tEthMac);
    
    /* 设置网口配置 */
    //comm_set_ifnet("eth1", ptEthIP->ip, ptEthIP->mask, ptEthIP->ipGateway, ptEthMac->data);

    COMM_FMT_DEBUG("eth1 start: contype(%d), workmode(%d).\n", ptEthCfg->contype, ptEthCfg->workmode);
//    if(1 == ssal_get_status())
//    {
//        comm_eth_proxy_working(1, ptHandle);
//    }
//    else
    {
        if(TYPE_UDP == ptEthCfg->contype)
        {
            comm_eth_udpworking(1, ptHandle);
        }
        else if(MODE_SERVER == ptEthCfg->workmode)  //服务器模式
        {
            comm_eth_serverworking(1, ptHandle);
        }
        else
        {
            comm_eth_working(1, ptHandle);
        }
    }
    return 0;
}

