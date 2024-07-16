/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块以太网通信
* @date：    2020-3-6
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

#include "common_base.h"
#include "comm_eth.h"

uint8     g_ETHName[2][24] = {{0},{0}};
uint8     *g_ETHStatus[2] = {NULL};

uint8     g_ETHdataBuf[2][BUF_LENGTH + 4] = {{0}, {0}};   /* 缓冲区 */

/**********************************************************************
* @name      : eth_set_name
* @brief     ：设置以太网口名称
* @param[in] ：uint8 ifno    网口编号
               char *ifname  名称
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-8-28
* @Update    :
**********************************************************************/
void eth_set_name(uint8 ifno, char *ifname)
{
    if(ifno < 2 && NULL != ifname)
    {
        g_ETHName[ifno][0] = MIN(23, strlen(ifname));
        memcpy(g_ETHName[ifno] + 1, ifname, MIN(23, strlen(ifname)));
    }
}

/**********************************************************************
* @name      : eth_find_connectfd
* @brief     ：获取connect_fd
* @param[in] ：uint8 channle     通道ID
               int connect_fd[]  句柄列表
* @param[out]：uint8 *pIndex     内部编号
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2020-3-7
* @Update    :
**********************************************************************/
int eth_find_connectfd(uint8 channle, int connect_fd[], uint8 *pIndex)
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
        return connect_fd[channle];
    }
    return -1;
}

/**********************************************************************
* @name      : eth_close_allfd
* @brief     ：eth网口关闭句柄
* @param[in] ：COMM_ETH_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-7
* @Update    :
**********************************************************************/
void eth_close_allfd(COMM_ETH_HANDLE_T *ptHandle)
{
    uint8 i;

    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptHandle->ip[i] = 0;
        ptHandle->port[i] = 0;
        if(ptHandle->connect_fd[i] > 0)
        {
            close(ptHandle->connect_fd[i]);
            ptHandle->connect_fd[i] = -1;
        }
    }
    ptHandle->masterNUM = 0;
}

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
* @name      : comm_eth_send
* @brief     ：eth网口发消息 非服务器方式
* @param[in] ：uint8 ifnum                 序号0-1
               char *ifName                网口名称
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：发送长度/-1报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2020-8-28
**********************************************************************/
int comm_eth_send(uint8 ifnum, char *ifName, COMM_ETH_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 index;
    int   fdSocket = 0;
    uint8  channel = 0;
    uint8 protocol = -1;         /* 内部协议 */
    int    sendLen = 0;
    int     outlen = 0;
    uint16  dataLen = 0;
    uint8   dataBuf[200] = {0};   /* 缓冲区 */
    COMM_IPCMSG_T  *ptIPCMsg = NULL;

    *pNextState = ETH_RECV_DATA;

    dataLen = comm_read_queue(ptHandle->ptSendList, g_ETHdataBuf[ifnum]);
    while(dataLen)
    {
        if(dataLen < 4)
        {
            COMM_FMT_TRACE("comm_ppp_send msg less then 4\n");
            goto cont;
        }
        
        channel = *g_ETHdataBuf[ifnum];
        protocol = comm_get_protocol(g_ETHdataBuf[ifnum]);
        fdSocket = eth_find_connectfd(channel, ptHandle->connect_fd, &index);

        if(PROT_CLOSE_CHANNLE == protocol)
        {
            if(fdSocket > 0)
            {
                close(fdSocket);
                ptHandle->connect_fd[index] = -1;
            }
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("%s close channel %d\n", ifName, ptIPCMsg->id);
            goto cont;
        }

        if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
            && PRO_HEART_FRAME != protocol)
        {
            goto cont;  //其他方式不发送
        }

        COMM_BUF_DEBUG(g_ETHdataBuf[ifnum] + 4, dataLen - 4, "%s channel %d send:", ifName, channel);

        if(fdSocket > 0)
        {
            sendLen = send(fdSocket, g_ETHdataBuf[ifnum]+4, dataLen-4, 0);
            if(sendLen > 0)
            {
                outlen += sendLen;
            }
            else
            {
                *pNextState = ETH_CONNECT_MASTER;
                close(fdSocket);
                ptHandle->connect_fd[index] = -1;
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
        }
cont:
        memset(g_ETHdataBuf[ifnum], 0, BUF_LENGTH + 4);
        dataLen = comm_read_queue(ptHandle->ptSendList, g_ETHdataBuf[ifnum]);
    }
    return outlen;
}

/**********************************************************************
* @name      : comm_eth_recv
* @brief     ：eth网口收消息 非服务器方式
* @param[in] ：uint8 ifnum                 序号0-1
               char *ifName                网口名称
               COMM_ETH_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：数据长度/-1 网口异常
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    : 2019-12-10
**********************************************************************/
int comm_eth_recv(uint8 ifnum, char *ifName, COMM_ETH_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 i;
    int recvLen = 0;
    int outlen = 0;
    uint8   dataBuf[200] = {0};   /* 缓冲区 */
    uint16  bufLen = 0;
    struct timespec  curtime = {0};
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    uint8   channel = 0;

    *pNextState = ETH_SEND_DATA;
    clock_gettime(CLOCK_MONOTONIC, &curtime);

    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        if(ptHandle->connect_fd[i] <= 0)
        {
            if(curtime.tv_sec - ptHandle->connet_time[i] >= 5)
            {
                /* 掉线的主站, 5s连接一次 */
                *pNextState = ETH_CONNECT_MASTER;
            }
            continue;
        }
        recvLen = 0;
        recvLen = recv(ptHandle->connect_fd[i], g_ETHdataBuf[ifnum], BUF_LENGTH, 0);    /*socket进行数据接收*/
        if(recvLen > 0)
        {
            outlen += recvLen;
            channel = (0 == ifnum ? CHL_ETH_1 : CHL_ETH_2) + i;
            comm_add_channel(channel, g_ETHdataBuf[ifnum], recvLen);
            bufLen = recvLen + 4;
            if(0 != comm_write_queue(ptHandle->ptRecvList, g_ETHdataBuf[ifnum], bufLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_BUF_DEBUG(g_ETHdataBuf[ifnum] + 4, recvLen, "%s channel %d recv:", ifName, channel);
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
        else if(recvLen <= 0 && (curtime.tv_sec - ptHandle->connet_time[i] > 10))
        {
            struct tcp_info info;
            int len = sizeof(info);
            getsockopt(ptHandle->connect_fd[i], IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
            //printf("tcp state %d\n", info.tcpi_state);
            if(TCP_CLOSE_WAIT == info.tcpi_state || TCP_CLOSE == info.tcpi_state)
            {
                close(ptHandle->connect_fd[i]);
                ptHandle->connect_fd[i] = -1;
                /* 关闭通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = (0 == ifnum ? CHL_ETH_1 : CHL_ETH_2) + i;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                bufLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("%s Server close the connect, channel %d\n", ifName, ptIPCMsg->id);
            }
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
    }

    return outlen;
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
* @Date      ：2020-8-28
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
    address.sin_addr.s_addr = htonl(ptHandle->ip[offset]);
    address.sin_port        = htons(ptHandle->port[offset]);
    sendLen = sendto(fd, pBuffer+4, bufLen-4, 0, (struct sockaddr *)&address, sizeof(address));
    if(sendLen < 0)
    {
        return -1;
    }
    return sendLen;
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
* @Date      ：2020-8-28
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
            if(ptHandle->ip[i] == ip && ptHandle->port[i] == port)
            {
                *pOffset = i;
                return recvLen;
            }
        }
    }
    return -1;
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
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;

    if(socketfd <= 0 || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_timer_getETH input null\n");
        return;
    }
    
    if(TRUE == s_ethFirst[ptHandle->no])
    {
        g_ETHStatus[ptHandle->no] = &(ptHandle->bEthLink);
    }
    else
    {
        return;
    }
    
    len = g_ETHName[ptHandle->no][0] + 1;
    ret = task_send_labelmsg(socketfd, MSG_COMM_GETETH, ptHandle->no+1, g_ETHName[ptHandle->no], len, PD_MSG_MONITOR);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("Comm send get eth status %d msg, ret %d\n", ptHandle->no+1, ret);
    }
    else
    {
        s_ethFirst[ptHandle->no] = FALSE;
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
int comm_eth_statusmsg(uint8 *playload, uint16 msglen, void *pArg)
{
    int status;
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;
    
    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_eth_statusmsg input null\n");
        return -1;
    }

    appmsg_memcpy_r(&status, playload, sizeof(int));
    
    if(0 == status)
    {
        ptHandle->bEthLink = FALSE;
    }
    else
    {
        ptHandle->bEthLink = TRUE;
    }
    COMM_FMT_DEBUG("comm_eth_statusmsg get eth %d, status %d\n", ptHandle->no, status);

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
int comm_eth_eventmsg(uint8 *playload, uint16 msglen, void *pArg)
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

/**********************************************************************
* @name      : comm_getcheck_ethcfg
* @brief     ：获取并检查eth配置是否发生变化
* @param[in] ：void *dbHandle    数据中心句柄
               void *pCfg        配置存储地址
* @param[out]：uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-8-28
* @Update    :
**********************************************************************/
int comm_getcheck_ethcfg(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    uint8 i;
    int retEth = 0;
    int retMaster = 0;
    uint32 readLen = 0;
    uint8  bChange = FALSE;
    uint8  bRestart = FALSE;
    NOMAL_OAD_T        tOAD;
    OOP_ETHCONFIG_T    tEth;
    OOP_MASTERPARAMS_T tMaster;
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pCfg;
    uint8 ifuint = ptHandle->no;

    static uint8 s_ethcfg[2][2] = {{0}, {0}};  //首次获取失败打印，防止日志过多

    if(ifuint > 1 || sockdbfd < 0 || NULL == pCfg)
    {
        return -1;
    }
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.logicId = PD_LOGIC_ID;

#ifdef AREA_ZHEJIANG
    OOP_ZJ_CONNECT_T   tType;
    tOAD.oad.value = 0x45300200;
    tType.type = ZJ_CONNECT_NULL;
    db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_ZJ_CONNECT_T), (uint8 *)&tType, &readLen);
    if(ZJ_CONNECT_ETH == tType.type)
    {
        ptHandle->bClose = FALSE;
    }
    else
    {
        ptHandle->bClose = TRUE;
    }
#endif

    tOAD.oad.value = (0 == ifuint) ? 0x45100200 : 0x45110200;
    memset(&tEth, 0, sizeof(OOP_ETHCONFIG_T));
    retEth = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_ETHCONFIG_T), (uint8 *)&tEth, &readLen);
    if(0 == retEth)
    {
        if(readLen != sizeof(OOP_ETHCONFIG_T))
        {
            retEth = -1;
        }
        else if(0 != memcmp(&(ptHandle->tEthCfg.tEthCfg), &tEth, readLen))
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
            retMaster = -1;
        }
        else if(0 != memcmp(&(ptHandle->tEthCfg.tMaster), &tMaster, readLen))
        {
            bChange = TRUE;
        }
    }
    else if(FALSE == s_ethcfg[ifuint][1])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retMaster);
        s_ethcfg[ifuint][1] = TRUE;
    }

    if(TRUE == bChange)
    {
        sleep(5); //等待5s后重启
        if(0 == retEth)
        {
            if(ptHandle->tEthCfg.tEthCfg.workmode != tEth.workmode || 
               ptHandle->tEthCfg.tEthCfg.contype != tEth.contype ||
               ptHandle->tEthCfg.tEthCfg.conmode != tEth.conmode)
            {
                bRestart = TRUE;
            }
            else
            {
                ptHandle->bChangeParam = TRUE;
            }
            memcpy(&(ptHandle->tEthCfg.tEthCfg), &tEth, sizeof(tEth));
        }
        if(0 == retMaster)
        {
            if(0 != memcmp(&(ptHandle->tEthCfg.tMaster), &tMaster, sizeof(tMaster)))
            {
                bRestart = TRUE;
            }
            memcpy(&(ptHandle->tEthCfg.tMaster), &tMaster, sizeof(tMaster));
        }
    }

    if(TRUE == bRestart)
    {
        COMM_FMT_DEBUG("%s change configure, need to restart.\n", g_ETHName[ifuint] + 1);

        /* 清理工作 例如路由 */
        for(i = 0; i < ptHandle->masterNUM; i++)
        {
            if(0 == ptHandle->ip[i] || 0 == g_ETHName[ifuint][0])
            {
                continue;
            }
            comm_del_route_net((char *)g_ETHName[ifuint] + 1, ptHandle->ip[i]);
        }

        if(TRUE == ptHandle->bGetCfg)
        {
            *ptHandle->pAppReStart = 1;
            sleep(2);
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
    else
    {
        ptHandle->bGetCfg = FALSE;
    }
    return retEth | retMaster;
}

/**********************************************************************
* @name      : comm_eth_start
* @brief     ：eth 启动判断
* @param[in] ：
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2020-3-6
* @Update    : 2020-8-28
**********************************************************************/
BOOLEAN comm_eth_start(void *pArg)
{
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;

    if(NULL == pArg || 0 == *ptHandle->pIPCok || FALSE == ptHandle->bGetCfg || FALSE == ptHandle->bEthLink
       || TRUE == ptHandle->bClose || 0 == ptHandle->tEthCfg.tMaster.nNum 
       || MODE_CLIENT != ptHandle->tEthCfg.tEthCfg.workmode)
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
* @name      : comm_eth_tcpwork
* @brief     ：tcp方式
* @param[in] ：COMM_ETH_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-8-28
* @Update    : 
**********************************************************************/
void comm_eth_tcpwork(COMM_ETH_HANDLE_T *ptHandle)
{
    uint8  i;    
    uint8  stateEth = ETH_SEND_DATA; /* 状态机 */
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    int         fdSocket = -1;
    uint8       dataBuf[200] = {0};  /* 缓冲区 */
    uint16      dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    char            ifName[24] = {0};
    uint8           ifunit = ptHandle->no;
    uint8           conCnt[OOP_MAX_HOST] = {0};

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */

    OOP_ETHCONFIG_T *ptEthCfg   = &(ptHandle->tEthCfg.tEthCfg);
    OOP_MASTERPARAMS_T *ptMaster = &(ptHandle->tEthCfg.tMaster);

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    *ptHandle->pRunFlag = 0;
    memcpy(ifName, g_ETHName[ifunit] + 1, g_ETHName[ifunit][0]);

    /* 连接主站 */
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
            ptHandle->ip[0] = htonl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->port[0] = ptMaster->master[i].port;
            ptHandle->masterNUM++;
            comm_add_route_net(ifName, ptHandle->ip[0]);
            sleep(1);
            fdSocket = comm_connect_socket(ptHandle->ip[0], ptHandle->port[0]);
            if(fdSocket > 0)
            {
                ptHandle->connect_fd[0] = fdSocket;
                COMM_FMT_DEBUG("%s connect master 0x%x, port %d\n", ifName, ptHandle->ip[0], ptHandle->port[0]);
                break;
            }
            ptHandle->masterNUM--;
            comm_del_route_net(ifName, ptHandle->ip[0]);
        }
        if(i == ptMaster->nNum)
        {  //从头开始链接
            for(i = 0; i < ptHandle->masterStart; i++)
            {
                *ptHandle->pRunFlag = 0;
                ptHandle->ip[0] = htonl(*((uint32 *)ptMaster->master[i].ip));
                ptHandle->port[0] = ptMaster->master[i].port;
                ptHandle->masterNUM++;
                comm_add_route_net(ifName, ptHandle->ip[0]);
                sleep(1);
                fdSocket = comm_connect_socket(ptHandle->ip[0], ptHandle->port[0]);
                if(fdSocket > 0)
                {
                    ptHandle->connect_fd[0] = fdSocket;
                    COMM_FMT_DEBUG("%s connect master 0x%x, port %d\n", ifName, ptHandle->ip[0], ptHandle->port[0]);
                    break;
                }
                ptHandle->masterNUM--;
                comm_del_route_net(ifName, ptHandle->ip[0]);    
            }
        }
    }
    else
    {
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ptHandle->ip[ptHandle->masterNUM] = htonl(*(uint32 *)ptMaster->master[i].ip);
            ptHandle->port[ptHandle->masterNUM] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptHandle->ip[ptHandle->masterNUM]);
            fdSocket = comm_connect_socket(ptHandle->ip[ptHandle->masterNUM], ptHandle->port[ptHandle->masterNUM]);
            usleep(100000);
            COMM_FMT_DEBUG("%s connect IP 0x%x, Port %d, fd %d\n", ifName, ptHandle->port[ptHandle->masterNUM],
                                                     ptHandle->port[ptHandle->masterNUM], fdSocket);
            if(fdSocket > 0)
            {
                ptHandle->connect_fd[ptHandle->masterNUM] = fdSocket;
            }
            ptHandle->connet_time[ptHandle->masterNUM] = (uint32)oldtime.tv_sec;
            ptHandle->masterNUM++;
        }
    }

    if(0 == ptHandle->masterNUM)
    {
        COMM_FMT_DEBUG("%s connect no master\n", ifName);
        return;
    }

    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = MODE_CLIENT;
    ptChanInfo->medium = COMM_MEDIA_ETH;
    ptChanInfo->heart = ptEthCfg->heart;
    
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        if(ptHandle->connect_fd[i] > 0)
        {
            ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
            ptChanInfo->ip = ptHandle->ip[i];
            ptChanInfo->port = ptHandle->port[i];
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("%s open channel %d\n", ifName, ptIPCMsg->id);
        }
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
            if(FALSE == ptHandle->bEthLink || TRUE == *ptHandle->pAppReStart || TRUE == ptHandle->bClose 
               || FALSE == ptHandle->bGetCfg || 0 == ptHandle->tEthCfg.tMaster.nNum)
            {               
                COMM_FMT_DEBUG("%s no link or appmain restart or cfg change\n", ifName);
                stateEth = ETH_CLOSE_DEVICE;
            }
            if(TRUE == ptHandle->bChangeParam)
            {
                ptHandle->bChangeParam = FALSE;

                /* 配置变化更新 */
                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->protocol = PRO_PARA_MOD;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = MODE_CLIENT;
                ptChanInfo->medium = COMM_MEDIA_ETH;
                ptChanInfo->heart = ptEthCfg->heart;
                
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->connect_fd[i] > 0)
                    {
                        ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
                        ptChanInfo->ip = ptHandle->ip[i];
                        ptChanInfo->port = ptHandle->port[i];
                        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                        COMM_FMT_DEBUG("%s change channel %d params\n", ifName, ptIPCMsg->id);
                    }
                } 
            }
        }
        
        switch(stateEth)
        {
            case ETH_CONNECT_MASTER:
            {
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->connect_fd[i] > 0 || curtime.tv_sec < ptHandle->connet_time[i] + 2)
                    {
                        continue;
                    }
                    ptHandle->connect_fd[i] = comm_connect_socket(ptHandle->ip[i], ptHandle->port[i]);
                    if(ptHandle->connect_fd[i] > 0)
                    {
                        /* 通知主应用调度线程通道打开 */
                        dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                        ptChanInfo->workmode = MODE_CLIENT;
                        ptChanInfo->medium = COMM_MEDIA_ETH;
                        ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
                        ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                        ptChanInfo->ip = ptHandle->ip[i];
                        ptChanInfo->port = ptHandle->port[i];
                        ptChanInfo->heart = ptEthCfg->heart;
                        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                        COMM_FMT_DEBUG("%s open channel %d\n", ifName, ptIPCMsg->id);
                        conCnt[i] = 0;
                    }
                    else
                    {
                        conCnt[i] += 1;
                    }
                    usleep(10000);
                    ptHandle->connet_time[i] = curtime.tv_sec;
                }
                stateEth = ETH_CLOSE_DEVICE;
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->connect_fd[i] > 0 || conCnt[i] < 5)
                    {
                        stateEth = ETH_SEND_DATA;
                    }
                }
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
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->connect_fd[i] <= 0)
                    {
                        continue;
                    }
                    /* 通知主应用调度线程通道关闭 */
                    dataLen = sizeof(COMM_IPCMSG_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = ((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i;
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s close channel %d\n", ifName, ptIPCMsg->id);
                }
                eth_close_allfd(ptHandle);
                return;
            }
            default:
            {
                stateEth = ETH_SEND_DATA;
                break;
            }
        }
        usleep(10000);
    }
    COMM_FMT_DEBUG("%s Send %lluB, Recv %lluB\n", ifName, applyTxFlow, applyRxFlow);
    return;
}

/**********************************************************************
* @name      : comm_eth_tcpwork
* @brief     ：udp方式
* @param[in] ：COMM_ETH_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-8-28
* @Update    : 
**********************************************************************/
void comm_eth_udpwork(COMM_ETH_HANDLE_T *ptHandle)
{
    uint8  i;    
    uint8  stateEth = ETH_SEND_DATA; /* 状态机 */
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    int         fdSocket = -1;
    uint8       dataBuf[200] = {0};  /* 缓冲区 */
    uint16      dataLen = 0;
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    char            ifName[24] = {0};
    uint8           ifunit = ptHandle->no;

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */
    int    sendRecvLen = 0;

    OOP_ETHCONFIG_T *ptEthCfg   = &(ptHandle->tEthCfg.tEthCfg);
    OOP_MASTERPARAMS_T *ptMaster = &(ptHandle->tEthCfg.tMaster);

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    *ptHandle->pRunFlag = 0;
    memcpy(ifName, g_ETHName[ifunit] + 1, g_ETHName[ifunit][0]);

    /* 主站链接 */
    ptHandle->masterNUM = 0;
    if(MODE_ZB == ptEthCfg->conmode)  //主备模式
    {
        ptHandle->masterNUM = 1;
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ptHandle->ip[0] = ntohl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->port[0] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptHandle->ip[0]);
            
            fdSocket = comm_init_udp(ifName);
            if(fdSocket > 0)
            {
                COMM_FMT_DEBUG("%s connect master 0x%x, port %d\n", ifName, ptHandle->ip[0], ptHandle->port[0]);
                break;
            }
            usleep(100000);
            comm_del_route_net(ifName, ptHandle->ip[0]);    
        }
    }
    else  //多连接方式
    {
        for(i = 0; i < ptMaster->nNum; i++)
        {
            ptHandle->masterNUM++;
            *ptHandle->pRunFlag = 0;
            ptHandle->ip[i] = ntohl(*((uint32 *)ptMaster->master[i].ip));
            ptHandle->port[i] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptHandle->ip[i]);
        }
        fdSocket = comm_init_udp(ifName);
        usleep(100000);
    }

    if(fdSocket <= 0)
    {
        COMM_FMT_DEBUG("%s Create socekt failed\n", ifName);
        return;
    }
    ptHandle->connect_fd[0] = fdSocket;

    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = MODE_CLIENT;
    ptChanInfo->medium = COMM_MEDIA_ETH;
    ptChanInfo->heart = ptEthCfg->heart;
    
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        if(ptHandle->connect_fd[i] > 0)
        {
            ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
            ptChanInfo->ip = ptHandle->ip[i];
            ptChanInfo->port = ptHandle->port[i];
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
            COMM_FMT_DEBUG("%s open channel %d\n", ifName, ptIPCMsg->id);
        }
    } 

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;

        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(FALSE == ptHandle->bEthLink || TRUE == *ptHandle->pAppReStart || TRUE == ptHandle->bClose 
               || FALSE == ptHandle->bGetCfg || 0 == ptHandle->tEthCfg.tMaster.nNum)
            {               
                COMM_FMT_DEBUG("%s no link or appmain restart or cfg change\n", ifName);
                stateEth = ETH_CLOSE_DEVICE;
            }
            if(TRUE == ptHandle->bChangeParam)
            {
                ptHandle->bChangeParam = FALSE;

                /* 配置变化更新 */
                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->protocol = PRO_PARA_MOD;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = MODE_CLIENT;
                ptChanInfo->medium = COMM_MEDIA_ETH;
                ptChanInfo->heart = ptEthCfg->heart;
                
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
                    ptChanInfo->ip = ptHandle->ip[i];
                    ptChanInfo->port = ptHandle->port[i];
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s change channel %d params\n", ifName, ptIPCMsg->id);
                } 
            }
        }

        switch(stateEth)
        {
            case ETH_SEND_DATA:
            {
                dataLen = (uint16)comm_read_queue(ptHandle->ptSendList, g_ETHdataBuf[ifunit]);
                if(dataLen > 0)
                {
                    sendRecvLen = comm_eth_udpsend(fdSocket, g_ETHdataBuf[ifunit], dataLen, ptHandle);
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
                sendRecvLen = comm_eth_udprecv(fdSocket, g_ETHdataBuf[ifunit], BUF_LENGTH, ptHandle, &i);
                if(sendRecvLen > 0)  //写入接受队列
                {
                    dataLen = (uint16)sendRecvLen;
                    applyRxFlow += dataLen;
                    /*给数据加上会话协议头部*/
                    comm_add_channel(((0 == ifunit) ? CHL_ETH_1 : CHL_ETH_2) + i, dataBuf, dataLen);
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
                dataLen = sizeof(COMM_IPCMSG_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    ptIPCMsg->id = (0 == ifunit ? CHL_ETH_1 : CHL_ETH_2) + i;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s change channel %d params\n", ifName, ptIPCMsg->id);
                    comm_del_route_net(ifName, ptHandle->ip[i]);  
                } 
                eth_close_allfd(ptHandle);
                return;
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
* @name      : comm_eth_entry
* @brief     ：以太网端口消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    : 2020-8-28
**********************************************************************/
void *comm_eth_entry(void *pArg)
{
    COMM_ETH_HANDLE_T *ptHandle = (COMM_ETH_HANDLE_T *)pArg;
    COMM_ETH_T        *ptEthCfg = NULL;

    if(NULL == pArg)
    {
        return 0;
    }

    /* 先释放上次没有来及释放的句柄 */
    eth_close_allfd(ptHandle);

    ptEthCfg = &ptHandle->tEthCfg;
    if(TYPE_UDP == ptEthCfg->tEthCfg.contype)
    {
        comm_eth_udpwork(ptHandle);
    }
    else if(TYPE_TCP == ptEthCfg->tEthCfg.contype)
    {
        comm_eth_tcpwork(ptHandle);
    }
    
    return 0;
}

