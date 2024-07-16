/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块无线通信
* @date：    2019-10-31
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include <net/if.h>
#include <error.h>
#include "msgUdp.h"
#include "storage.h"
#include <linux/tcp.h>
#include "task_msg.h"
#include "db_api.h"
#include "msg_id.h"
#include "task_id.h"

#include "comm.h" 
#include "ppp_comm.h"
#include "proxy_comm.h"
#include "area.h"

uint8         *g_PPPDiagFlag[2] = {NULL};
COMM_PPP_HANDLE_T *ptPPPHandle[2] = {NULL};
uint8          g_commrediag1 = 0;
uint8          g_commrediag2 = 0;
extern BOOLEAN app_comm_proxy_isuse();
extern BOOLEAN g_proxyOK;

uint8   g_PPPdataBuf[2][BUF_LENGTH + 8] = {{0}, {0}};   /* 缓冲区 */
extern uint8 gdelay;
extern uint8 ssal_get_status();

/**********************************************************************
* @name      : ppp_get_channeloffset
* @brief     ：获取内部通道偏移
* @param[in] ：uint8 channle    通道ID
* @param[out]：
* @return    ：偏移
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
int ppp_get_channeloffset(uint8 channle)
{
    int offset = -1;
    if(channle >= CHL_ETH_1 && channle < CHL_GPRS_1)
    {
        return offset;
    }
    
    if(channle >= CHL_GPRS_2)
    {
        channle -= CHL_GPRS_2;
    }
    else
    {
        channle -= CHL_GPRS_1;
    }
    offset = channle;
    return offset;
}

/**********************************************************************
* @name      : ppp_find_clientfd
* @brief     ：获取client_fd
* @param[in] ：uint8 channle    通道ID
               int client_fd[]  句柄列表
* @param[out]：uint8 *pIndex    内部编号
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
int ppp_find_clientfd(uint8 channle, int client_fd[], uint8 *pIndex)
{
    if(channle >= CHL_ETH_1 && channle < CHL_GPRS_1)
    {
        return -1;
    }
    
    if(channle >= CHL_GPRS_2)
    {
        channle -= CHL_GPRS_2;
    }
    else
    {
        channle -= CHL_GPRS_1;
    }
    if(channle < OOP_MAX_HOST)
    {
        *pIndex = channle;
        return client_fd[channle];
    }
    return -1;
}

/**********************************************************************
* @name      : ppp_find_clientfd
* @brief     ：关闭client_fd
* @param[in] ：uint8 channle    通道ID
               int client_fd[]  句柄列表
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
void ppp_close_clientfd(uint8 channle, int client_fd[])
{
    if(channle >= CHL_ETH_1 && channle < CHL_GPRS_1)
    {
        return;
    }
    
    if(channle >= CHL_GPRS_2)
    {
        channle -= CHL_GPRS_2;
    }
    else
    {
        channle -= CHL_GPRS_1;
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
* @name      : ppp_find_acceptfd
* @brief     ：获取accept_fd
* @param[in] ：uint8 channle    通道ID
               int accept_fd[]  句柄列表
* @param[out]：
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
int ppp_find_acceptfd(uint8 channle, int accept_fd[])
{
    if(channle >= CHL_ETH_1 && channle < CHL_GPRS_1)
    {
        return -1;
    }
    
    if(channle >= CHL_GPRS_2)
    {
        channle -= (CHL_GPRS_2 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    else
    {
        channle -= (CHL_GPRS_1 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    if(channle < OOP_MAX_LISTENS * PPP_CONNECT_NUM)
    {
        return accept_fd[channle];
    }
    return -1;
}

/**********************************************************************
* @name      : ppp_close_acceptfd
* @brief     ：关闭accept_fd
* @param[in] ：uint8 channle    通道ID
               int accept_fd[]  句柄列表
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
void ppp_close_acceptfd(uint8 channle, int accept_fd[])
{
    if(channle >= CHL_ETH_1 && channle < CHL_GPRS_1)
    {
        return;
    }
    
    if(channle >= CHL_GPRS_2)
    {
        channle -= (CHL_GPRS_2 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    else
    {
        channle -= (CHL_GPRS_1 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    if(channle < OOP_MAX_LISTENS * PPP_CONNECT_NUM)
    {
        if(accept_fd[channle] > 0)
        {
            close(accept_fd[channle]);
            accept_fd[channle] = -1;
        }
    }
    return;
}

/**********************************************************************
* @name      : ppp_acceptfd_connect
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
uint8 ppp_acceptfd_connect(uint8 channle, uint8 bStatus[], uint8 bConnect)
{
    if(channle >= CHL_ETH_1 && channle < CHL_GPRS_1)
    {
        return 0xff;
    }
    
    if(channle >= CHL_GPRS_2)
    {
        channle -= (CHL_GPRS_2 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    else
    {
        channle -= (CHL_GPRS_1 + OOP_MAX_HOST);  //前4个分给客户机混合模式
    }
    if(channle < OOP_MAX_LISTENS * PPP_CONNECT_NUM)
    {
        bStatus[channle] = bConnect;
    }
    return channle;
}

/**********************************************************************
* @name      : ppp_find_apn
* @brief     ：查询当前APN
* @param[in] ：COMM_PPP_HANDLE_T *ptHandle
* @param[out]：
* @return    ：IP地址
* @Create    : 王津剑
* @Date      ：2021-9-10
* @Update    :
**********************************************************************/
OOP_APN_T *ppp_find_apn(COMM_PPP_HANDLE_T *ptHandle)
{
    if(NULL == ptHandle)
    {
        return NULL;
    }

    if(0 == ptHandle->apnIndex)
    {
        return &(ptHandle->tGprsCfg.tOopGprs.apn);
    }
    else if(ptHandle->apnIndex <= ptHandle->tGprsCfg.tAPN.nNum)
    {
        return &(ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex - 1].apn);
    }    
    return 0;
}

/**********************************************************************
* @name      : ppp_find_master
* @brief     ：ppp网口工作 查询主站列表
* @param[in] ：char *ifName      网口名称
               uint16 listenPort[]  监听端口
* @param[out]：COMM_PPP_HANDLE_T *ptHandle
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void *ppp_find_master(COMM_PPP_HANDLE_T *ptHandle)
{
    if(NULL == ptHandle)
    {
        return NULL;
    }

    if(0 == ptHandle->apnIndex)
    {
        return &(ptHandle->tGprsCfg.tOopMaster);
    }
    else if(ptHandle->apnIndex <= ptHandle->tGprsCfg.tAPN.nNum)
    {
        return &(ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex - 1].master);
    }    
    return NULL;
}

/**********************************************************************
* @name      : ppp_open_listen
* @brief     ：ppp网口工作 服务器模式
* @param[in] ：char *ifName      网口名称
               uint16 listenPort[]  监听端口
* @param[out]：COMM_PPP_HANDLE_T *ptHandle
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
int ppp_open_listen(char *ifName, uint16 listenPort[], COMM_PPP_HANDLE_T *ptHandle)
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
        COMM_FMT_TRACE("%s Create listem port %d\n", ifName, listenPort[i]);
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0)
        {
            COMM_FMT_TRACE("get socket error\n");
            goto closefd;
        }
    
        /* 设置SOCKET文件句柄为非阻塞模式 */
        if((flags_block = fcntl(listenfd, F_GETFL, 0))<0) 
        {   
            COMM_FMT_TRACE("ERROR F_GETFL\n");
            close(listenfd);
            goto closefd;
        }
        
        /* socket绑定网口 */
        /*bzero(&interf, sizeof(interf));
        strncpy(interf.ifr_ifrn.ifrn_name, ifName, strlen(ifName));
        if (setsockopt(listenfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interf, sizeof(interf)) < 0) 
        {
            COMM_FMT_TRACE(" SO_BINDTODEVICE failed\n");
            goto closefd;
        }*/
        
        if(fcntl(listenfd, F_SETFL, flags_block | O_NONBLOCK))
        {   
            COMM_FMT_TRACE("ERROR set mode\n"); 
            close(listenfd);
            goto closefd;
        }

        /* 关闭子进程无用文件描述符 */
        val = fcntl(listenfd, F_GETFD);
        if (val == -1)
        {
            COMM_FMT_TRACE("ERROR F_GETFD\n"); 
            close(listenfd);
            goto closefd;
        }

        val |= FD_CLOEXEC;
        if (fcntl(listenfd, F_SETFD, val) == -1)
        {
            COMM_FMT_TRACE("ERROR set F_SETFD\n"); 
            close(listenfd);
            goto closefd;
        }

        //SO_REUSEADDR 允许在bind（）过程中本地地址可重复使用
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
        {
             COMM_FMT_TRACE("setsockopt error SO_REUSEADDR\n");
             goto closefd;
        }
        
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family      = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port        = htons(listenPort[i]);
        
        if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0)
        {
            close(listenfd);
            COMM_FMT_TRACE("bind error\n");
            goto closefd;
        }

        if (listen(listenfd, PPP_CONNECT_NUM) < 0)
        {
            close(listenfd);
            COMM_FMT_TRACE("listen error\n");
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
* @name      : ppp_close_listen
* @brief     ：ppp网口工作 服务器模式
* @param[in] ：COMM_PPP_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void ppp_close_listen(COMM_PPP_HANDLE_T *ptHandle)
{
    uint8 i, j;

    for(i = 0; i < OOP_MAX_LISTENS; i++)
    {
        for(j = 0; j < PPP_CONNECT_NUM; j++)
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
* @name      : ppp_next_diagAPN
* @brief     ：设置下一个拨号
* @param[in] ：COMM_PPP_HANDLE_T *ptHandle 句柄
* @param[out]：
* @return    ：0-成功/其他-错误
* @Create    : 
* @Date      ：2020-4-26
* @Update    :
**********************************************************************/
int ppp_next_diagAPN(COMM_PPP_HANDLE_T *ptHandle)
{
    uint8 i = 0;
    uint8 index = 0;
     
    if(NULL == ptHandle)
    {
        return -1;
    }

    if(ptHandle->apnIndex >= ptHandle->tGprsCfg.tAPN.nNum && 0 != ptHandle->tGprsCfg.tOopMaster.nNum)
    {
        ptHandle->apnIndex = 0;
        return 0;
    }

    if(ptHandle->apnIndex >= ptHandle->tGprsCfg.tAPN.nNum)
    {
        ptHandle->apnIndex = 0;
    }

    if(ptHandle->apnIndex > 0)
    {
        for(i = ptHandle->apnIndex; i < ptHandle->tGprsCfg.tAPN.nNum; i++)
        {
            if((ptHandle->operat == ptHandle->tGprsCfg.tAPN.net[i].factory ||
               255 == ptHandle->tGprsCfg.tAPN.net[i].factory) && 
               (ptHandle->nettype == ptHandle->tGprsCfg.tAPN.net[i].network ||
                255 == ptHandle->tGprsCfg.tAPN.net[i].network))
            {
                index = i + 1;
                break;
            }
        }
    }
    else
    {
        for(i = 0; i < ptHandle->tGprsCfg.tAPN.nNum; i++)
        {
            if((ptHandle->operat == ptHandle->tGprsCfg.tAPN.net[i].factory ||
                255 == ptHandle->tGprsCfg.tAPN.net[i].factory) && 
               (ptHandle->nettype == ptHandle->tGprsCfg.tAPN.net[i].network ||
                255 == ptHandle->tGprsCfg.tAPN.net[i].network))
            {
                index = i + 1;
                break;
            }
        }
    }
    if(i >= ptHandle->tGprsCfg.tAPN.nNum)
    {
        ptHandle->apnIndex = 0;
        return 0;
    }
    else
    {
        ptHandle->apnIndex = index;
        return 0;
    }
}

/**********************************************************************
* @name      : ppp_check_ethip
* @brief     ：ppp网口检查ip地址是否和以太网冲突
* @param[in] ：uint32 ipaddr               地址
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：TRUE-重复; FALSE-不冲突
* @Create    : 王津剑
* @Date      ：2020-8-13
* @Update    : 
**********************************************************************/
uint8 ppp_check_same_ethip(uint32 ipaddr, COMM_PPP_HANDLE_T *ptHandle)
{
    uint8 i;

    if(TRUE == *ptHandle->pETHStatus)
    {
        for(i = 0; i < ptHandle->pETHMaster->nNum; i++)
        {
            if(ipaddr == ntohl(*(uint32 *)ptHandle->pETHMaster->master[i].ip))
            {
                return TRUE;
            }
        }
    }
    
    return FALSE;

}

/**********************************************************************
* @name      : comm_ppp_send
* @brief     ：ppp网口发消息 非服务器方式
* @param[in] ：uint8 ifuint                网口编号
               char *ifName                网口名称
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：发送长度/-1报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    : 2019-12-10
**********************************************************************/
int comm_ppp_send(uint8 ifuint, char *ifName, COMM_PPP_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 index;
    int   fdSocket = 0;
    uint8  channel = 0;
    uint8 protocol = 0;         /* 内部协议 */
    int    sendLen = 0;
    int     outlen = 0;
    uint16  dataLen = 0;
    uint8 *dataBuf = NULL;   /* 缓冲区 */
    COMM_IPCMSG_T *ptIPCMsg = NULL;
    struct timespec curtime = {0};

    OOP_GPRSCONFIG_T *ptGprs = &(ptHandle->tGprsCfg.tOopGprs);

    *pNextState = PPP_RECV_DATA;

    if(TYPE_TCP == ptGprs->contype && MODE_SERVER == ptGprs->workmode) //服务器模式不支持
    {
        return 0;
    }
    dataBuf = g_PPPdataBuf[ifuint];
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
        fdSocket = ppp_find_clientfd(channel, ptHandle->client_fd, &index);

        if(PRO_LOGIN_ERROR == protocol)
        {
            COMM_FMT_DEBUG("%s登录失败, 关闭通道%d\n", ifName, channel);

            ppp_close_clientfd(channel, ptHandle->client_fd);
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            if(MODE_ZB == ptGprs->conmode)
            {
                /* 主备模式切换下一个主站链接 */
                ptHandle->masterStart += 1;
            }
            else
            {
                /* 重启无线拨号 */
                uint8 devnum = ifuint + 1;
                comm_send_msg(MSG_COMM_REDIAG, &devnum, 1);
                COMM_FMT_DEBUG("无线模块重新拨号\n");
            }
            *pNextState = PPP_CLOSE_DEVICE;
            *ptHandle->pAppReStart = 1;
            goto end;
        }
        if(PRO_HEARTBEAT_ERROR == protocol)
        {
            /* 丢失心跳 重启无线 */
            COMM_FMT_DEBUG("%s丢失心跳, 关闭通道%d\n", ifName, channel);
            uint8 devnum = ifuint + 1;

            ppp_close_clientfd(channel, ptHandle->client_fd);
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }

            comm_send_msg(MSG_COMM_REDIAG, &devnum, 1);
            COMM_FMT_DEBUG("无线模块重新拨号\n");
            *pNextState = PPP_CLOSE_DEVICE;
            *ptHandle->pAppReStart = 1;
            goto end;
        }

        if(PROT_CLOSE_CHANNLE == protocol)
        {
            ppp_close_clientfd(channel, ptHandle->client_fd);
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = MODE_MIX == ptGprs->workmode ? PROT_PAUSE_CHANNLE : PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("%s%s通道%d\n", ifName, MODE_MIX == ptGprs->workmode ? "暂停" : "关闭", channel);
            goto cont;
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
            COMM_FMT_DEBUG("%s发送大消息, 长度%d\n", ifName, ptBigMsg->msglen);
            if(fdSocket > 0)
            {
                uint32 singlelen = 0;
                uint32 msgLeft = ptBigMsg->msglen;
                uint8 *pTemp = ptBigMsg->msgaddr;
                singlelen = MIN(msgLeft, 512);
                sendLen = 0;
                while(singlelen > 0)
                {
                    COMM_BUF_DEBUG(pTemp, singlelen, "%s发送", ifName);
                    sendLen += send(fdSocket, pTemp, singlelen, 0);
                    pTemp += singlelen;
                    msgLeft -= singlelen;
                    singlelen = MIN(msgLeft, 512);
                    usleep(20000);
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
                    *pNextState = PPP_CONNECT_MASTER;
                    ppp_close_clientfd(channel, ptHandle->client_fd);
                    /* 关闭通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = channel;
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    dataLen = sizeof(COMM_IPCMSG_T);
                    COMM_FMT_DEBUG("%s关闭通道%d\n", ifName, channel);
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
            if(MODE_MIX == ptGprs->workmode && fdSocket < 0)
            {
                clock_gettime(CLOCK_MONOTONIC, &curtime);
                ptHandle->client_fd[index] = comm_connect_socket(ifName, ptHandle->masterIP[index], 
                                                                 ptHandle->masterPort[index]);  /*连接指定IP的端口号*/
                if(ptHandle->client_fd[index] <= 0)
                {
                    COMM_FMT_DEBUG("%s打开通道%d失败\n", ifName, channel);
                    goto end;
                }
                fdSocket = ptHandle->client_fd[index];
                ptHandle->connet_time[index] = (uint32)curtime.tv_sec;

                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = channel;
                ptIPCMsg->protocol = PROT_CONTINUE_CHANNLE;
                dataLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("%s设置通道%d工作\n", ifName, channel);
            }

            goto end;
        }

        if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
            && PRO_HEART_FRAME != protocol)
        {
            goto cont;  //其他方式不发送
        }

        /* 防止60s临界时间关闭导致不发送 */
        if(fdSocket < 0 && MODE_MIX == ptGprs->workmode)
        {
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            ptHandle->client_fd[index] = comm_connect_socket(ifName, ptHandle->masterIP[index], 
                                                             ptHandle->masterPort[index]);  /*连接指定IP的端口号*/
            if(ptHandle->client_fd[index] <= 0)
            {
                COMM_FMT_DEBUG("%s打开通道%d失败\n", ifName, channel);
                *pNextState = PPP_CONNECT_MASTER;
                goto end;
            }
            fdSocket = ptHandle->client_fd[index];
            ptHandle->connet_time[index] = (uint32)curtime.tv_sec;

            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CONTINUE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("%s设置通道%d工作\n", ifName, channel);
        }

        COMM_BUF_DEBUG(dataBuf + 4, dataLen - 4, "%s(%d)%s", ifName, channel, fdSocket > 0 ? "发送" : "丢弃");
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
                    COMM_FMT_DEBUG("%s(%d)发送失败, %s(%d)\n", ifName, channel, strerror(errno), errno);
                }
            }
            if(MODE_MIX == ptGprs->workmode)
            {
                clock_gettime(CLOCK_MONOTONIC, &curtime);
                ptHandle->connet_time[index] = (uint32)curtime.tv_sec;
            }
        }
cont:
        memset(dataBuf, 0, dataLen);
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    }
end:
    return outlen;
}

/**********************************************************************
* @name      : comm_ppp_send_proxy
* @brief     ：ppp网口代理发消息 非服务器方式
* @param[in] ：uint8 ifuint                网口编号
               char *ifName                网口名称
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：发送长度/-1报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    : 2019-12-10
**********************************************************************/
int comm_ppp_send_proxy(uint8 ifuint, char *ifName, COMM_PPP_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 index;
    int   fdSocket = 0;
    uint8  channel = 0;
    uint8 protocol = 0;         /* 内部协议 */
    int    sendLen = 0;
    int     outlen = 0;
    uint16  dataLen = 0;
    uint8 *dataBuf = NULL;   /* 缓冲区 */
    COMM_IPCMSG_T *ptIPCMsg = NULL;

    OOP_GPRSCONFIG_T *ptGprs = &(ptHandle->tGprsCfg.tOopGprs);

    *pNextState = PPP_RECV_DATA;

    if(TYPE_TCP == ptGprs->contype && MODE_CLIENT != ptGprs->workmode) //服务器模式不支持
    {
        return 0;
    }
    dataBuf = g_PPPdataBuf[ifuint];
    if(!g_proxyOK)
    {
        *pNextState = PPP_CLOSE_DEVICE;
        /* 关闭通道 */
        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
        ptIPCMsg->id = channel;
        ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
        dataLen = sizeof(COMM_IPCMSG_T);
        COMM_FMT_DEBUG("%s close channel %d\n", ifName, channel);
        if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
        {
            COMM_FMT_TRACE("Msg Queue is full\n");
        }

        return outlen;
    }
            
    dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    while(dataLen)
    {
        if(!g_proxyOK)
        {
            *pNextState = PPP_CLOSE_DEVICE;
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = channel;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            COMM_FMT_DEBUG("%s close channel %d\n", ifName, channel);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }

            return outlen;
        }
            
        if(dataLen <= 4)
        {
            COMM_FMT_TRACE("comm_ppp_send msg less then 4\n");
            goto cont;
        }
        channel = *dataBuf;
        protocol = comm_get_protocol(dataBuf);

        if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
            && PRO_HEART_FRAME != protocol)
        {
            goto cont;  //其他方式不发送
        }

        fdSocket = ppp_find_clientfd(channel, ptHandle->client_fd, &index);
        COMM_BUF_DEBUG(dataBuf, dataLen, "%s send:", ifName);

        //将报文发给通信代理,去掉4字节的内部协议
        sendLen = comm_proxy_send(fdSocket, dataBuf+4, dataLen-4, 0);
        if(sendLen > 0)
        {
            outlen += sendLen;
        }
cont:
        memset(dataBuf, 0, dataLen);
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    }
    return outlen;
}

/**********************************************************************
* @name      : comm_pppserver_send
* @brief     ：ppp网口发消息 服务器方式
* @param[in] ：uint8 *pBuffer                缓冲区
               COMM_QUEUE_LIST_T *ptSendList 消息列表
               COMM_PPP_HANDLE_T *ptHandle   线程私有数据
* @param[out]：uint32 *pSendLen              发送数据量
* @return    ：0成功/错误码, 报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    : 2019-12-9
**********************************************************************/
int comm_pppserver_send(char *ifName, COMM_QUEUE_LIST_T *ptSendList, COMM_PPP_HANDLE_T *ptHandle, uint32 *pSendLen)
{
    int     sendLen = 0;
    uint16  dataLen = 0;
    uint8  *dataBuf = NULL;   /* 缓冲区 */
    uint8   protocol = -1;               /* 内部协议 */
    uint8   channel = 0;
    int     fd = 0;
    COMM_IPCMSG_T *ptIPCMsg = NULL;
    
    dataBuf = g_PPPdataBuf[ptHandle->no];
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
            ppp_close_acceptfd(channel, &(ptHandle->accept_fd[0][0]));
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

        fd = ppp_find_acceptfd(channel, &(ptHandle->accept_fd[0][0]));
        if(fd > 0)
        {
            sendLen = send(fd, dataBuf+4, dataLen-4, 0);
            if(sendLen > 0)
            {
                *pSendLen += sendLen;
            }
            COMM_BUF_DEBUG(dataBuf, dataLen, "%s send(%d):", ifName, sendLen);
        }
cont:
        memset(dataBuf, 0, BUF_LENGTH);
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
    }
    
    return 0;
}

/**********************************************************************
* @name      : comm_ppp_udpsend
* @brief     ：ppp网口发消息 UDP方式
* @param[in] ：int fd                      socket句柄
               uint8 *pBuffer              缓冲区
               uint16 bufLen               缓冲区大小
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：发送长度/-1报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
int comm_ppp_udpsend(int fd, uint8 *pBuffer, uint16 bufLen, COMM_PPP_HANDLE_T *ptHandle)
{
    uint8 protocol = 0;         /* 内部协议 */
    int    sendLen = 0;
    int    offset = 0;
    struct sockaddr_in address;

    if(bufLen <= 4)
    {
        COMM_FMT_TRACE("comm_ppp_send msg less then 4\n");
        return 0;
    }

    protocol = comm_get_protocol(pBuffer);
    
    /*如果是数据中转传送、心跳帧、登陆帧、退出登陆帧, 将数据发送出去*/
    if(PRO_DATA_TRANSMIT != protocol && PRO_LOGIN_FRAME != protocol && PRO_EXIT_LOGIN_FRAME != protocol
        && PRO_HEART_FRAME != protocol)
    {
        return 0;  //其他方式不发送
    }

    offset = ppp_get_channeloffset(*pBuffer);
    if(offset < 0 || offset >= ptHandle->masterNUM)
    {
        return 0;
    }

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
* @name      : comm_ppp_recv
* @brief     ：ppp网口收消息 非服务器方式
* @param[in] ：uint8 ifuint                   网口编号
               char *ifName                   网口名称
               COMM_PPP_HANDLE_T *ptHandle    线程私有数据
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：数据长度/-1 网口异常
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    : 2019-12-10
**********************************************************************/
int comm_ppp_recv(uint8 ifuint, char *ifName, COMM_PPP_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 i;
    int recvLen = 0;
    int outlen = 0;
    uint8  *dataBuf = NULL;   /* 缓冲区 */
    uint16  bufLen = 0;
    struct timespec curtime = {0};
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    OOP_GPRSCONFIG_T *ptGprs = &(ptHandle->tGprsCfg.tOopGprs);
    uint8   channel = 0;
    
    *pNextState = PPP_SEND_DATA;

    if(MODE_SERVER == ptGprs->workmode)            //服务器模式不处理
    {
        return 0;
    }
    dataBuf = g_PPPdataBuf[ifuint];
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        recvLen = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(ptHandle->client_fd[i] <= 0 && MODE_CLIENT == ptGprs->workmode)
        {
            if(curtime.tv_sec - ptHandle->connet_time[i] >= 5)
            {
                /* 掉线的主站, 5s连接一次 */
                *pNextState = PPP_CONNECT_MASTER;
            }
            continue;
        }
        channel = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + i;
        if(ptHandle->client_fd[i] > 0 && MODE_MIX == ptGprs->workmode)
        {
            /* 混合模式 没有通信 1min主动关闭链接 */
            if(curtime.tv_sec - ptHandle->connet_time[i] >= 70)
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
                COMM_FMT_DEBUG("%s暂停通道%d\n", ifName, channel);
                return outlen;
            }
        }
        recvLen = recv(ptHandle->client_fd[i], dataBuf, BUF_LENGTH, 0);    /*socket进行数据接收*/
        if(recvLen > 0)
        {
            outlen += recvLen;
            comm_add_channel(channel, dataBuf, recvLen);
            bufLen = recvLen + 4;
            COMM_BUF_DEBUG(dataBuf + 4, bufLen - 4, "%s(%d)接收", ifName, channel);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
            {
                COMM_FMT_DEBUG("Msg Queue is full\n");
            }
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
        if(recvLen <= 0 && (curtime.tv_sec - ptHandle->connet_time[i] > 5) && MODE_CLIENT == ptGprs->workmode)
        {
            struct tcp_info info;
            int len = sizeof(info);
            getsockopt(ptHandle->client_fd[i], IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
            //printf("tcp state %d, reclen %d\n", info.tcpi_state, recvLen);
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
                COMM_FMT_DEBUG("服务器断开通道%d连接(status %d)\n", channel, info.tcpi_state);
            }
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
    }
    
    return outlen;
}

/**********************************************************************
* @name      : comm_ppp_recv_proxy
* @brief     ：ppp网口代理收消息 非服务器方式
* @param[in] ：uint8 ifuint                   网口编号
               char *ifName                   网口名称
               COMM_PPP_HANDLE_T *ptHandle    线程私有数据
* @param[out]：uint8 *pNextState           下一个状态
* @return    ：数据长度/-1 网口异常
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    : 2019-12-10
**********************************************************************/
int comm_ppp_recv_proxy(uint8 ifuint, char *ifName, COMM_PPP_HANDLE_T *ptHandle, uint8 *pNextState)
{
    uint8 i;
    int recvLen = 0;
    int outlen = 0;
    uint8  *dataBuf = NULL;   /* 缓冲区 */
    uint16  bufLen = 0;
    struct timespec curtime = {0};
    COMM_IPCMSG_T  *ptIPCMsg = NULL;
    OOP_GPRSCONFIG_T *ptGprs = &(ptHandle->tGprsCfg.tOopGprs);

    *pNextState = PPP_SEND_DATA;

    if(MODE_SERVER == ptGprs->workmode)            //服务器模式不处理
    {
        return 0;
    }
    dataBuf = g_PPPdataBuf[ifuint];
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        recvLen = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(!g_proxyOK)
        {
            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + i;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            bufLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            
            COMM_FMT_DEBUG("Server close the connect, channel %d\n", ptIPCMsg->id);
                
            *pNextState = PPP_CONNECT_MASTER;
            continue;
        }
        
        recvLen = comm_proxy_recv(ptHandle->client_fd[i], dataBuf, BUF_LENGTH, 0);    /*socket进行数据接收*/
        if(recvLen > 0)
        {
            outlen += recvLen;
            comm_add_channel((0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + i, dataBuf, recvLen);
            bufLen = recvLen + 4;
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
            {
                COMM_FMT_TRACE("Msg Queue is full\n");
            }
            COMM_BUF_DEBUG(dataBuf, bufLen, "%s recv:", ifName);
            ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
        }
        if(recvLen <= 0)
        {
            if(!g_proxyOK)
            {
                /* 关闭通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + i;
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
* @name      : comm_pppserver_recv
* @brief     ：ppp网口收消息 服务器方式
* @param[in] ：uint8 ifuint                   网口编号
               char *ifName                   网口名称
               COMM_QUEUE_LIST_T *ptRecvList  消息列表
               COMM_QUEUE_LIST_T *ptRecvList1 消息列表1
               COMM_PPP_HANDLE_T *ptHandle    线程私有数据
* @param[out]：uint32 *pRecvLen               接收数据量
* @return    ：0成功/错误码, 报错需要关闭通道
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    : 2019-12-9
**********************************************************************/
int comm_pppserver_recv(uint8 ifuint, char *ifName, COMM_QUEUE_LIST_T *ptRecvList, COMM_QUEUE_LIST_T *ptRecvList1,
                        COMM_PPP_HANDLE_T *ptHandle, uint32 *pRecvLen)
{
    int     recvLen = 0;
    uint8   i,j;
    uint8   offset = 0;
    fd_set  fdsr;
    int     maxsock;
    int     ret = 0;
    struct timeval tv = {0,0};      //超时时间设为0 是非阻塞
    struct sockaddr_in client_addr; //客户端地址
    uint8  *dataBuf = NULL;   /* 缓冲区 */
    int     new_fd = -1;
    socklen_t sin_size = sizeof(client_addr);
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T  *ptIPCMsg = NULL;

    dataBuf = g_PPPdataBuf[ifuint];

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
        for (j = 0; j < PPP_CONNECT_NUM; j++) 
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
        for (j = 0; j < PPP_CONNECT_NUM; j++) 
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
                    ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + offset;
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
                    comm_add_channel((0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + offset, dataBuf, recvLen);
                    recvLen += 4;
                    if(0 != comm_write_queue(ptRecvList, dataBuf, recvLen))
                    {
                        if(0 != comm_write_queue(ptRecvList1, dataBuf, recvLen))
                        {
                            COMM_FMT_TRACE("Msg Queue is full\n");
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
            for(j = 0; j < PPP_CONNECT_NUM; j++)
            {
                if(ptHandle->accept_fd[i][j] <= 0)
                {
                    COMM_FMT_TRACE("%s client[%d %d] open, ip 0x%x, port %d\n", ifName, i, j, 
                              ntohl(client_addr.sin_addr.s_addr), ntohs(client_addr.sin_port));
                    ptHandle->accept_fd[i][j] = new_fd;
                    
                    /* 开启通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    offset = i *j + OOP_MAX_HOST;
                    ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + offset;
                    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                    ptChanInfo->ip = ntohl(client_addr.sin_addr.s_addr);
                    ptChanInfo->port = ntohs(client_addr.sin_port);
                    ptChanInfo->workmode = MODE_SERVER;
                    ptChanInfo->medium = COMM_MEDIA_GPRS;
                    ptChanInfo->connType = TYPE_TCP;
                    ptChanInfo->bFirst = TRUE;
                    ptChanInfo->heart = ptHandle->tGprsCfg.tOopGprs.heart;
                    ptChanInfo->dailcnt = ptHandle->tGprsCfg.tOopGprs.dail.mask.dailcnt;
                    ptChanInfo->dailtime = ptHandle->tGprsCfg.tOopGprs.dail.mask.dailtime;
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
            if(PPP_CONNECT_NUM == j)
            {
                close(new_fd);
            }
        }
    }

    return 0;
}

/**********************************************************************
* @name      : comm_ppp_udprecv
* @brief     ：ppp网口收消息 UDP方式
* @param[in] ：int fd                   socket句柄
               uint16 bufLen            缓冲区大小
               OOP_GPRSCONFIG_T *ptGprs 配置
* @param[out]：uint8 *pBuffer           缓冲区
               uint8 *pOffset           通道偏移
* @return    ：数据长度/-1 网口异常
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
int comm_ppp_udprecv(int fd, uint8 *pBuffer, uint16 bufLen, COMM_PPP_HANDLE_T *ptHandle, uint8 *pOffset)
{
    uint8  i;
    int    recvLen = 0;
    int    sin_len;
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
        return 0;
    }
    return -1;
}

/**********************************************************************
* @name      : comm_ppp_addflow
* @brief     ：ppp网口增加流量统计
* @param[in] ：COMM_PORTFLOW_T *ptFlow   统计结构
               uint32 applyRx            应用层新增接收流量
               uint32 applyTx            应用层新增发送流量
               uint64 ifnetRx            网口层新增接收流量
               uint64 ifnetTx            网口层新增发送流量

* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-7
* @Update    :
**********************************************************************/
void comm_ppp_addflow(COMM_PORTFLOW_T *ptFlow, uint32 applyRx, uint32 applyTx, uint64 ifnetRx, uint64 ifnetTx)
{
    ptFlow->applyRxFlow += applyRx;
    ptFlow->applyTxFlow += applyTx;

    ptFlow->ifnetRxFlow += ifnetRx;
    ptFlow->ifnetTxFlow += ifnetTx;
}

/**********************************************************************
* @name      : comm_ppp_resetflow
* @brief     ：ppp网口清空流量统计
* @param[in] ：COMM_PORTFLOW_T *ptFlow   统计结构
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-7
* @Update    :
**********************************************************************/
void comm_ppp_resetflow(COMM_PORTFLOW_T *ptFlow)
{
    ptFlow->applyRxFlow = 0;
    ptFlow->applyTxFlow = 0;

    ptFlow->ifnetRxFlow = 0;
    ptFlow->ifnetTxFlow = 0;
}

/**********************************************************************
* @name      : comm_ppp_free_allfd
* @brief     ：清空所有文件/sicket句柄
* @param[in] ：COMM_PPP_HANDLE_T *ptHandle
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-13
* @Update    :
**********************************************************************/
void comm_ppp_free_allfd(COMM_PPP_HANDLE_T *ptHandle)
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
* @name      : comm_ppp_working
* @brief     ：ppp网口工作 非服务器模式
* @param[in] ：uint8 ifuint                网口编号
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-31
* @Update    : 2019-12-10
**********************************************************************/
int comm_ppp_working(uint8 ifuint, COMM_PPP_HANDLE_T *ptHandle)
{
    uint8  i, j;
    int    fdSocket = -1;
    char   ifName[8] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    OOP_GPRSCONFIG_T     *ptGprs = NULL; 
    OOP_MASTERPARAMS_T *ptMaster = NULL;
    
    CHANNEL_INFO_T   *ptChanInfo = 0;
    COMM_IPCMSG_T      *ptIPCMsg = NULL;
    uint8              dev_state = PPP_SEND_DATA;   /* 状态机 */
    uint16               dataLen = 0;
    uint8    dataBuf[BUF_LENGTH] = {0};             /* 缓冲区 */

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */
    uint32 ipArddr = 0;
    uint8  masterleft = 0;
    uint8  connecttime = 0;
    uint32   waitTime = 0;

    if(NULL == ptHandle || ifuint > 1)
    {
        return -1;
    }

    /* 先释放上次没有来及释放的句柄 */
    comm_ppp_free_allfd(ptHandle);

    ptGprs = &(ptHandle->tGprsCfg.tOopGprs);
    ptMaster = (OOP_MASTERPARAMS_T *)ppp_find_master(ptHandle);

    if(MODE_SERVER == ptGprs->workmode && TYPE_TCP == ptGprs->contype)
    {
        COMM_FMT_DEBUG("comm_ppp_working not support server\n");
        return -1;    
    }

    if(NULL == ptMaster)
    {
        if(0 == ppp_next_diagAPN(ptHandle))
        {
            COMM_FMT_DEBUG("ppp %d get no master with ppp apn, set next apn\n");
            ptHandle->bChangeApn = TRUE;
            ptHandle->bGetDCM = FALSE;
        }
        else
        {
            COMM_FMT_DEBUG("ppp %d get no master with current apn\n");
            ptHandle->bGetDCM = FALSE;
        }
        return -1;
    }
    
    sprintf(ifName, "ppp%d", ifuint);

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    ptHandle->masterNUM = 0;
    if(MODE_ZB == ptGprs->conmode)  //主备模式
    {
        if(ptHandle->masterStart >= ptMaster->nNum)
        {
            ptHandle->masterStart = 0;
            /* 重启无线拨号 */
            uint8 devnum = ifuint + 1;
            comm_send_msg(MSG_COMM_REDIAG, &devnum, 1);
        }

        for(i = ptHandle->masterStart; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ipArddr = ntohl(*(uint32 *)ptMaster->master[i].ip);
            if(0 == ipArddr || 0 == ptMaster->master[i].port)
            {
                COMM_FMT_DEBUG("%s连接错误%d.%d.%d.%d:%d\n", ifName, COMM_PRINT_IPADDR(ipArddr), 
                                                                     ptMaster->master[i].port);
                continue;
            }
            if(TRUE == ppp_check_same_ethip(ipArddr, ptHandle))
            {
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d与以太网主站相同\n", ifName, COMM_PRINT_IPADDR(ipArddr));
                comm_del_route_net(ifName, ptMaster->master[i].ip);
                continue;
            }
            ptHandle->masterIP[0] = ipArddr;
            ptHandle->masterPort[0] = ptMaster->master[i].port;
            ptHandle->masterNUM++;
            comm_add_route_net(ifName, ptMaster->master[i].ip);
            usleep(500000);
            for(j = 0; j < 3; j++)
            {
                waitTime = 30 * j;
                while(waitTime > 0)
                {
                    *ptHandle->pRunFlag = 0;
                    waitTime--;
                    sleep(1);
                }
                fdSocket = comm_connect_socket(ifName, ptHandle->masterIP[0], ptHandle->masterPort[0]);
                if(fdSocket > 0)
                {
                    break;
                }
            }
            if(fdSocket > 0)
            {
                ptHandle->client_fd[0] = fdSocket;
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d成功\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[0]), 
                                                             ptHandle->masterPort[0]);
                break;
            }
            ptHandle->masterNUM--;
            comm_del_route_net(ifName, ptMaster->master[i].ip);
        }
        if(i == ptMaster->nNum)
        {
            for(i = 0; i < ptHandle->masterStart; i++)
            {
                *ptHandle->pRunFlag = 0;
                ipArddr = ntohl(*(uint32 *)ptMaster->master[i].ip);
                if(0 == ipArddr || 0 == ptMaster->master[i].port)
                {
                    COMM_FMT_DEBUG("%s连接错误%d.%d.%d.%d:%d\n", ifName, COMM_PRINT_IPADDR(ipArddr), 
                                                                 ptMaster->master[i].port);
                    continue;
                }
                if(TRUE == ppp_check_same_ethip(ipArddr, ptHandle))
                {
                    COMM_FMT_DEBUG("%s连接%d.%d.%d.%d与以太网主站相同\n", ifName, COMM_PRINT_IPADDR(ipArddr));
                    comm_del_route_net(ifName, ptMaster->master[i].ip);
                    continue;
                }
                ptHandle->masterIP[0] = ipArddr;
                ptHandle->masterPort[0] = ptMaster->master[i].port;
                ptHandle->masterNUM++;
                comm_add_route_net(ifName, ptMaster->master[i].ip);
                usleep(500000);
                usleep(500000);
                for(j = 0; j < 3; j++)
                {
                    waitTime = 30 * j;
                    while(waitTime > 0)
                    {
                        *ptHandle->pRunFlag = 0;
                        waitTime--;
                        sleep(1);
                    }
                    fdSocket = comm_connect_socket(ifName, ptHandle->masterIP[0], ptHandle->masterPort[0]);
                    if(fdSocket > 0)
                    {
                        break;
                    }
                }
                if(fdSocket > 0)
                {
                    ptHandle->client_fd[0] = fdSocket;
                    COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d成功\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[0]), 
                                                                 ptHandle->masterPort[0]);
                    break;
                }
                ptHandle->masterNUM--;
                comm_del_route_net(ifName, ptMaster->master[i].ip);
            }
        }
    }
    else
    {
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ipArddr = ntohl(*(uint32 *)ptMaster->master[i].ip);
            if(0 == ipArddr || 0 == ptMaster->master[i].port)
            {
                COMM_FMT_DEBUG("%s连接错误%d.%d.%d.%d:%d\n", ifName, COMM_PRINT_IPADDR(ipArddr), 
                                                             ptMaster->master[i].port);
                continue;
            }
            if(TRUE == ppp_check_same_ethip(ipArddr, ptHandle))
            {
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d与以太网主站相同\n", ifName, COMM_PRINT_IPADDR(ipArddr));
                comm_del_route_net(ifName, ptMaster->master[i].ip);
                continue;
            }
            ptHandle->masterIP[ptHandle->masterNUM] = ipArddr;
            ptHandle->masterPort[ptHandle->masterNUM] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptMaster->master[i].ip);
            usleep(500000);
            for(j = 0; j < 3; j++)
            {
                fdSocket = comm_connect_socket(ifName, ptHandle->masterIP[ptHandle->masterNUM], 
                                               ptHandle->masterPort[ptHandle->masterNUM]);
                if(fdSocket > 0)
                {
                    break;
                }
                sleep(1);
            }
            if(fdSocket > 0)
            {
                ptHandle->client_fd[ptHandle->masterNUM] = fdSocket;
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d成功\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[ptHandle->masterNUM]), 
                                                                 ptHandle->masterPort[ptHandle->masterNUM]);
            }
            else
            {
                COMM_FMT_DEBUG("%s连接%d.%d.%d.%d:%d失败\n", ifName, COMM_PRINT_IPADDR(ptHandle->masterIP[ptHandle->masterNUM]), 
                                                             ptHandle->masterPort[ptHandle->masterNUM]);
            }
            ptHandle->connet_time[ptHandle->masterNUM] = (uint32)oldtime.tv_sec;
            ptHandle->masterNUM++;
        }
    }

    if(0 == ptHandle->masterNUM)
    {
        if(0 == ppp_next_diagAPN(ptHandle))
        {
            COMM_FMT_DEBUG("%s连接主站失败, 设置下一个APN\n", ifName);
            ptHandle->bChangeApn = TRUE;
            ptHandle->bGetDCM = FALSE;
        }
        else
        {
            COMM_FMT_DEBUG("%s连接主站失败, 重试\n", ifName);
            ptHandle->bGetDCM = FALSE;
        }
        return -1;
    }

    /* 设置socket */
#ifndef PRODUCT_ECU
    appmain_trans_AT(ifuint + 1, "AT$MYSOCKETLED=1\r\n", dataBuf, BUF_LENGTH);
#endif

    memset(dataBuf, 0, BUF_LENGTH);
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = ptGprs->workmode;
    ptChanInfo->medium = COMM_MEDIA_GPRS;
    ptChanInfo->connType = ptGprs->contype;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = ptGprs->heart;
    ptChanInfo->dailcnt = ptGprs->dail.mask.dailcnt;
    ptChanInfo->dailtime = ptGprs->dail.mask.dailtime;
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + i;
        ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
        ptChanInfo->ip = ptHandle->masterIP[i];
        ptChanInfo->port = ptHandle->masterPort[i];
        if(ptHandle->client_fd[i] > 0)
        {
            ptHandle->connet_time[i] = oldtime.tv_sec;
            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
        }
        COMM_FMT_DEBUG("%s打开通道%d\n", ifName, ptIPCMsg->id);
    }
    memset(dataBuf, 0, BUF_LENGTH);
    dataLen = 0;
    ptHandle->bChangeParam = FALSE;

    if(NULL != ptHandle->MixHeart)
    {
        *(ptHandle->MixHeart) = MODE_MIX == ptGprs->workmode ? ptGprs->heart : 0;
    }

    while(1)
    {
        *ptHandle->pRunFlag = 0;
        
        if(FALSE == ptHandle->bDiagOK || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
        {
            uint8 routeAddr[4] = {0};
            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            COMM_FMT_DEBUG("%s退出工作, 重新连接\n", ifName);
            for(i = 0; i < ptHandle->masterNUM; i++)
            {
                if(ptHandle->client_fd[i] > 0 || MODE_MIX == ptGprs->workmode)
                {
                    /* 关闭所有通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + i;
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
                if(0 == ptHandle->masterIP[i])
                {
                    continue;
                }
                routeAddr[0] = (ptHandle->masterIP[i] >> 24) & 0xff;
                routeAddr[1] = (ptHandle->masterIP[i] >> 16) & 0xff;
                routeAddr[2] = (ptHandle->masterIP[i] >> 8) & 0xff;
                routeAddr[3] = ptHandle->masterIP[i] & 0xff;
                comm_del_route_net(ifName, routeAddr);
            }
            *(ptHandle->MixHeart) = 0;
            comm_ppp_free_allfd(ptHandle);
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
                    ptChanInfo->workmode = ptGprs->workmode;
                    ptChanInfo->medium = COMM_MEDIA_GPRS;
                    ptChanInfo->connType = ptGprs->contype;
                    ptChanInfo->bFirst = FALSE;
                    ptChanInfo->heart = ptGprs->heart;
                    ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                    ptIPCMsg->protocol = PRO_PARA_MOD;
                    ptChanInfo->ip = ptHandle->masterIP[i];
                    ptChanInfo->port = ptHandle->masterPort[i];
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%通道%d参数变化\n", ifName, ptIPCMsg->id);
                    memset(dataBuf, 0, BUF_LENGTH);
                    usleep(100000);
                }
            }
            if(NULL != ptHandle->MixHeart)
            {
                *(ptHandle->MixHeart) = MODE_MIX == ptGprs->workmode ? ptGprs->heart : 0;
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            comm_flow_add(TRUE, ifuint, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;

            memcpy(&oldtime, &curtime, sizeof(struct timespec));

            /* 是否与以太网ip相同 */
            if(TRUE == *ptHandle->pETHStatus && ptHandle->pETHMaster->nNum > 0)
            {
                masterleft = ptHandle->masterNUM;
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(FALSE == ppp_check_same_ethip(ptHandle->masterIP[i], ptHandle))
                    {
                        if(ptHandle->client_fd[i] <= 0)
                        {
                            /* 以太网断开则重新连接 */
                            dev_state = PPP_CONNECT_MASTER;
                        }
                        continue;
                    }
                    COMM_FMT_DEBUG("%s主站%d.%d.%d.%d与以太网主站相同\n", ifName, 
                                                                          COMM_PRINT_IPADDR(ptHandle->masterIP[i]));

                    if(ptHandle->client_fd[i] > 0)
                    {
                        /* 关闭通道 */
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                        ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                        dataLen = sizeof(COMM_IPCMSG_T);
                        if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                        {
                            COMM_FMT_TRACE("Msg Queue is full\n");
                        }
                        COMM_FMT_DEBUG("%s关闭通道%d\n", ifName, ptIPCMsg->id);
                        memset(dataBuf, 0, BUF_LENGTH);
                        usleep(10000);
                        close(ptHandle->client_fd[i]);
                        ptHandle->client_fd[i] = -1;
                    }
                    ptHandle->client_fd[i] = 0;
                    if(masterleft > 0)
                    {
                        masterleft--;
                    }
                    comm_del_iproute_net(ifName, ptHandle->masterIP[i]);
                }
                if(0 == masterleft)
                {
                    ptHandle->masterNUM = 0;
                    *(ptHandle->MixHeart) = 0;
                    return -1;
                }
            }
            else
            {
                /* 以太网断开则重新连接 */
                dev_state = PPP_CONNECT_MASTER;
            }
        }
        
        memset(dataBuf, 0, BUF_LENGTH);
        switch(dev_state)
        {
            case PPP_CONNECT_MASTER:
            {
                if(MODE_MIX == ptGprs->workmode)
                {
                    dev_state = PPP_RECV_DATA;
                    break;
                }
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->client_fd[i] <= 0 && FALSE == ppp_check_same_ethip(ptHandle->masterIP[i], ptHandle))
                    {
                        ptHandle->client_fd[i] = comm_connect_socket_noblock(ifName, ptHandle->masterIP[i],
                                                                             ptHandle->masterPort[i],
                                                                             &(ptHandle->temp_fd[i]),
                                                                             &(ptHandle->temp_time[i]));
                        if(ptHandle->client_fd[i] > 0)
                        {
                            /* 通知主应用调度线程通道打开 */
                            dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                            ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                            ptChanInfo->workmode = ptGprs->workmode;
                            ptChanInfo->medium = COMM_MEDIA_GPRS;
                            ptChanInfo->connType = ptGprs->contype;
                            ptChanInfo->bFirst = FALSE;
                            ptChanInfo->heart = ptGprs->heart;
                            ptChanInfo->dailcnt = ptGprs->dail.mask.dailcnt;
                            ptChanInfo->dailtime = ptGprs->dail.mask.dailtime;
                            ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                            ptIPCMsg->protocol = (MODE_CLIENT == ptGprs->workmode)?PROT_OPEN_CHANNLE : PRO_PARA_MOD;
                            ptChanInfo->ip = ptHandle->masterIP[i];
                            ptChanInfo->port = ptHandle->masterPort[i];
                            comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                            COMM_FMT_DEBUG("%s打开通道%d\n", ifName, ptIPCMsg->id);
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
                        dev_state = PPP_SEND_DATA;
                        connecttime = 0;
                        goto nextstatus;
                    }
                }

                /* socket全0 */
                if(connecttime++ < 30)
                {
                    sleep(2);
                    dev_state = PPP_CONNECT_MASTER; /* 全0 2s链接一次主站 */
                }
                else
                {
                    *ptHandle->pAppReStart = 1;
                    COMM_FMT_DEBUG("%s连接失败%d次, 重连!\n", ifName, connecttime);
                }
nextstatus:
                break;
            }
            case PPP_SEND_DATA:
            {
                applyTxFlow += comm_ppp_send(ifuint, ifName, ptHandle, &dev_state);
                dev_state = PPP_RECV_DATA;
                break;
            }
            
            case PPP_RECV_DATA:
            {
                applyRxFlow += comm_ppp_recv(ifuint, ifName, ptHandle, &dev_state);
                break;
            }
            
            case PPP_CLOSE_DEVICE:
            {
                dev_state = PPP_CONNECT_MASTER;
                return -1;
            }
            default:
            {
                dev_state = PPP_SEND_DATA;
                break;
            }
        }
        usleep(20000); /* 网络延时20ms */
    }
    
}

/**********************************************************************
* @name      : comm_ppp_proxy_working
* @brief     ：ppp网口工作 ssal打开
* @param[in] ：uint8 ifuint                网口编号
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-9-10
* @Update    : 
**********************************************************************/
int comm_ppp_proxy_working(uint8 ifuint, COMM_PPP_HANDLE_T *ptHandle)
{
    int    fdSocket = -1;
    char   ifName[8] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    OOP_GPRSCONFIG_T     *ptGprs = NULL; 
    OOP_MASTERPARAMS_T *ptMaster = NULL;
    OOP_APN_T             *ptApn = NULL;
    CHANNEL_INFO_T   *ptChanInfo = 0;
    COMM_IPCMSG_T      *ptIPCMsg = NULL;
    uint8              dev_state = PPP_SEND_DATA;   /* 状态机 */
    uint16               dataLen = 0;
    uint8    dataBuf[BUF_LENGTH] = {0};             /* 缓冲区 */

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */
    uint8  masterleft = 0;
    uint8  connecttime = 0;

    if(NULL == ptHandle || ifuint > 1)
    {
        return -1;
    }

    /* 先释放上次没有来及释放的句柄 */
    comm_ppp_free_allfd(ptHandle);
    
    ptHandle->bProxySSAL = 0;
    ptGprs = &(ptHandle->tGprsCfg.tOopGprs);
    ptMaster = (OOP_MASTERPARAMS_T *)ppp_find_master(ptHandle);
    ptApn = ppp_find_apn(ptHandle);

    if((MODE_SERVER == ptGprs->workmode && TYPE_TCP == ptGprs->contype) || MODE_ZB != ptGprs->conmode)
    {
        COMM_FMT_DEBUG("comm_ppp_proxy_working not support server\n");
        return -1;    
    }

    if(NULL == ptMaster)
    {
        if(0 == ppp_next_diagAPN(ptHandle))
        {
            COMM_FMT_DEBUG("ppp %d get no master with ppp apn, set next apn\n");
            ptHandle->bChangeApn = TRUE;
            ptHandle->bGetDCM = FALSE;
        }
        else
        {
            COMM_FMT_DEBUG("ppp %d get no master with current apn\n");
            ptHandle->bGetDCM = FALSE;
        }
        return -1;
    }
    
    sprintf(ifName, "ppp-%d", ifuint);

    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    ptHandle->masterNUM = 0;

    /* 连接前置机(代理) */
    ptHandle->proxyip = htonl(*((uint32 *)ptApn->proxyip));
    ptHandle->proxyport = ptApn->proxyport;
    if(TRUE == *ptHandle->pETHStatus && ptHandle->pETHMaster->nNum > 0)
    {
        if(TRUE == ppp_check_same_ethip(ptHandle->proxyip, ptHandle))
        {
            return -1;
        }
    }
    comm_add_route_net(ifName, ptApn->proxyip);
    fdSocket = comm_connect_socket(ifName, ptHandle->proxyip, ptHandle->proxyport);
    if(fdSocket > 0)
    {
        ptHandle->client_fd[0] = fdSocket;
        COMM_FMT_DEBUG("%s connect procy 0x%x, port %d\n", ifName, ptHandle->proxyip, ptHandle->proxyport);
    }
    else
    {
        fdSocket = comm_connect_socket(ifName, ptHandle->proxyip, ptHandle->proxyport);
        if(fdSocket > 0)
        {
            ptHandle->client_fd[0] = fdSocket;
            COMM_FMT_DEBUG("%s connect procy 0x%x, port %d\n", ifName, ptHandle->proxyip, ptHandle->proxyport);
        }
        else
        {
            comm_del_route_net(ifName, ptApn->proxyip);
            COMM_FMT_DEBUG("%s connect procy failed\n", ifName);
            ppp_next_diagAPN(ptHandle);
            ptHandle->bChangeApn = TRUE;
            ptHandle->bGetDCM = FALSE;
            return -1;
        }
    }

    ptHandle->bProxySSAL = 1;

    /* 首先选择一个主 */
    if(ptHandle->masterStart >= ptMaster->nNum)
    {
        ppp_next_diagAPN(ptHandle);
        COMM_FMT_DEBUG("%s connect no master, set next apn\n", ifName);
        ptHandle->bChangeApn = TRUE;
        ptHandle->bGetDCM = FALSE;
        ptMaster->nNum = 0;
        return -1;
    }
    ptHandle->masterIP[0] = htonl(*((uint32 *)ptMaster->master[ptHandle->masterStart].ip));
    ptHandle->masterPort[0] = ptMaster->master[ptHandle->masterStart].port;
    ptHandle->masterNUM = 1;
    
    /* 连接前置机 */
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = ptGprs->workmode;
    ptChanInfo->medium = COMM_MEDIA_GPRS;
    ptChanInfo->connType = ptGprs->contype;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = ptGprs->heart;
    ptChanInfo->dailcnt = ptGprs->dail.mask.dailcnt;
    ptChanInfo->dailtime = ptGprs->dail.mask.dailtime;
    ptChanInfo->proxyip = ptHandle->proxyip;
    ptChanInfo->proxyPort = ptHandle->proxyport;
    ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2);
    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
    ptChanInfo->ip = ptHandle->masterIP[0];
    ptChanInfo->port = ptHandle->masterPort[0];
    if(ptHandle->client_fd[0] > 0)
    {
        ptHandle->connet_time[0] = oldtime.tv_sec;
        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
    }
    COMM_FMT_DEBUG("%s id %d open\n", ifName, ptIPCMsg->id);

    memset(dataBuf, 0, BUF_LENGTH);
    dataLen = 0;
    ptHandle->bChangeParam = FALSE;

    while(1)
    {
        *ptHandle->pRunFlag = 0;
        
        if(FALSE == ptHandle->bDiagOK || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
        {
            *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
            COMM_FMT_DEBUG("%s is not exist or appmain restart\n", ifName);
            if(ptHandle->client_fd[0] > 0 || MODE_MIX == ptGprs->workmode)
            {
                /* 关闭所有通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2);
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
            comm_del_iproute_net(ifName, ptHandle->proxyip);
            *(ptHandle->MixHeart) = 0;
            comm_ppp_free_allfd(ptHandle);
            return -1;
        }
        if(TRUE == ptHandle->bChangeParam)
        {
            ptHandle->bChangeParam = FALSE;
            /* 发送数据更新 */
            if(ptHandle->client_fd[0] > 0)
            {
                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = ptGprs->workmode;
                ptChanInfo->medium = COMM_MEDIA_GPRS;
                ptChanInfo->connType = ptGprs->contype;
                ptChanInfo->bFirst = FALSE;
                ptChanInfo->heart = ptGprs->heart;
                ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + 0;
                ptIPCMsg->protocol = PRO_PARA_MOD;
                ptChanInfo->ip = ptHandle->masterIP[0];
                ptChanInfo->port = ptHandle->masterPort[0];
                ptChanInfo->proxyip = ptHandle->proxyip;
                ptChanInfo->proxyPort = ptHandle->proxyport;
                comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                COMM_FMT_DEBUG("%s channel id %d change\n", ifName, ptIPCMsg->id);
                memset(dataBuf, 0, BUF_LENGTH);
                usleep(100000);
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            comm_flow_add(TRUE, ifuint, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;

            memcpy(&oldtime, &curtime, sizeof(struct timespec));

            /* 是否与以太网ip相同 */
            if(TRUE == *ptHandle->pETHStatus && ptHandle->pETHMaster->nNum > 0)
            {
                if(FALSE == ppp_check_same_ethip(ptHandle->proxyip, ptHandle))
                {
                    if(ptHandle->client_fd[0] <= 0)
                    {
                        /* 以太网断开则重新连接 */
                        dev_state = PPP_CONNECT_MASTER;
                    }
                    continue;
                }
                COMM_FMT_DEBUG("%s connect IP 0x%x is the same with ETH\n", ifName, ptHandle->masterIP[0]);

                if(ptHandle->client_fd[0] > 0)
                {
                    /* 关闭通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2);
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    dataLen = sizeof(COMM_IPCMSG_T);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                    {
                        COMM_FMT_TRACE("Msg Queue is full\n");
                    }
                    COMM_FMT_DEBUG("%s channel id %d close\n", ifName, ptIPCMsg->id);
                    memset(dataBuf, 0, BUF_LENGTH);
                    usleep(10000);
                    close(ptHandle->client_fd[0]);
                    ptHandle->client_fd[0] = -1;
                }
                
                comm_del_iproute_net(ifName, ptHandle->proxyip);
                if(0 == masterleft)
                {
                    ptHandle->masterNUM = 0;
                    *(ptHandle->MixHeart) = 0;
                    return -1;
                }
            }
            else
            {
                /* 以太网断开则重新连接 */
                dev_state = PPP_CONNECT_MASTER;
            }
        }
        
        memset(dataBuf, 0, BUF_LENGTH);
        switch(dev_state)
        {
            case PPP_CONNECT_MASTER:
            {
                if(ptHandle->client_fd[0] <= 0 && FALSE == ppp_check_same_ethip(ptHandle->proxyip, ptHandle))
                {
                    ptHandle->client_fd[0] = comm_connect_socket_noblock(ifName, ptHandle->proxyip, ptHandle->proxyport,
                                                                         &(ptHandle->temp_fd[0]),
                                                                         &(ptHandle->temp_time[0]));
                    if(ptHandle->client_fd[0] > 0)
                    {
                        /* 通知主应用调度线程通道打开 */
                        dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                        ptChanInfo->workmode = ptGprs->workmode;
                        ptChanInfo->medium = COMM_MEDIA_GPRS;
                        ptChanInfo->connType = ptGprs->contype;
                        ptChanInfo->bFirst = FALSE;
                        ptChanInfo->heart = ptGprs->heart;
                        ptChanInfo->dailcnt = ptGprs->dail.mask.dailcnt;
                        ptChanInfo->dailtime = ptGprs->dail.mask.dailtime;
                        ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2);
                        ptIPCMsg->protocol = (MODE_CLIENT == ptGprs->workmode)?PROT_OPEN_CHANNLE : PRO_PARA_MOD;
                        ptChanInfo->ip = ptHandle->masterIP[0];
                        ptChanInfo->port = ptHandle->masterPort[0];
                        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                        COMM_FMT_DEBUG("%s channel id %d open\n", ifName, ptIPCMsg->id);
                        memset(dataBuf, 0, BUF_LENGTH);
                        usleep(100000);
                    }
                    ptHandle->connet_time[0] = (uint32)curtime.tv_sec;
                }

                /* socket全0 */
                if(connecttime++ < 60)
                {
                    sleep(1);
                    dev_state = PPP_CONNECT_MASTER; /* 全0 1s链接一次主站 */
                }
                else
                {
                    *ptHandle->pAppReStart = 1;
                    COMM_FMT_DEBUG("%s connect failed for 60 times, restart!\n", ifName);
                }
                
                break;
            }
            case PPP_SEND_DATA:
            {
                applyTxFlow += comm_ppp_send(ifuint, ifName, ptHandle, &dev_state);
                dev_state = PPP_RECV_DATA;
                break;
            }
            
            case PPP_RECV_DATA:
            {
                applyRxFlow += comm_ppp_recv(ifuint, ifName, ptHandle, &dev_state);
                break;
            }
            
            case PPP_CLOSE_DEVICE:
            {
                dev_state = PPP_CONNECT_MASTER;
                return -1;
            }
            default:
            {
                dev_state = PPP_SEND_DATA;
                break;
            }
        }
        usleep(10000); /* 网络延时10ms */
    }
}

/**********************************************************************
* @name      : comm_ppp_proxy
* @brief     ：ppp网口代理 非服务器模式
* @param[in] ：uint8 ifuint                网口编号
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-10-31
* @Update    : 2019-12-10
**********************************************************************/
int comm_ppp_proxy(uint8 ifuint, COMM_PPP_HANDLE_T *ptHandle)
{
    uint8   i;
    char  ifName[8] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    OOP_GPRSCONFIG_T     *ptGprs = NULL; 
    
    CHANNEL_INFO_T *ptChanInfo = 0;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    uint8           dev_state = PPP_SEND_DATA;   /* 状态机 */
    uint8 dataBuf[BUF_LENGTH] = {0};             /* 缓冲区 */
    uint16            dataLen = 0;

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */

    if(NULL == ptHandle || ifuint > 1)
    {
        return -1;
    }
    
    ptGprs = &(ptHandle->tGprsCfg.tOopGprs);
    sprintf(ifName, "ppp%d", ifuint);
    clock_gettime(CLOCK_MONOTONIC, &oldtime);

    /* 先释放上次没有来及释放的句柄 */
    comm_ppp_free_allfd(ptHandle);

    //等待通信代理连接到主站
    while(g_proxyOK == FALSE)
    {
        *ptHandle->pRunFlag = 0;
        usleep(100000);       
    }

    ptHandle->connet_time[ptHandle->masterNUM] = (uint32)oldtime.tv_sec;

    //当前方案，无法获取连接的主站数量，默认写1
    ptHandle->masterNUM = 1;

    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = ptGprs->workmode;
    ptChanInfo->medium = COMM_MEDIA_GPRS;
    ptChanInfo->connType = ptGprs->contype;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = ptGprs->heart;
    ptChanInfo->dailcnt = ptGprs->dail.mask.dailcnt;
    ptChanInfo->dailtime = ptGprs->dail.mask.dailtime;
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + i;
        ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
        ptChanInfo->ip = ptHandle->masterIP[i];
        ptChanInfo->port = ptHandle->masterPort[i];

        comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
        COMM_FMT_DEBUG("%s id %d open\n", ifName, ptIPCMsg->id);
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
            comm_flow_add(TRUE, ifuint, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;

            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(FALSE == ptHandle->bDiagOK || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
            {
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
                COMM_FMT_DEBUG("%s is not exist or appmain restart\n", ifName);

                /* 关闭所有通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + 0;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                dataLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("%s channel id %d close\n", ifName, ptIPCMsg->id);
                memset(dataBuf, 0, BUF_LENGTH);
                usleep(10000);
                    
                comm_ppp_free_allfd(ptHandle);
                return -1;
            }
            if(TRUE == ptHandle->bChangeParam)
            {
                ptHandle->bChangeParam = FALSE;
                /* 发送数据更新 */
                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = ptGprs->workmode;
                ptChanInfo->medium = COMM_MEDIA_GPRS;
                ptChanInfo->connType = ptGprs->contype;
                ptChanInfo->bFirst = FALSE;
                ptChanInfo->heart = ptGprs->heart;
                ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + 0;
                ptIPCMsg->protocol = PRO_PARA_MOD;
                ptChanInfo->ip = ptHandle->masterIP[i];
                ptChanInfo->port = ptHandle->masterPort[i];
                comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                COMM_FMT_DEBUG("%s channel id %d change\n", ifName, ptIPCMsg->id);
                memset(dataBuf, 0, BUF_LENGTH);
                usleep(100000);
            }
        }
        
        memset(dataBuf, 0, BUF_LENGTH);
        switch(dev_state)
        {
            case PPP_CONNECT_MASTER:
            {
                if(g_proxyOK == TRUE)
                {
                    /* 通知主应用调度线程通道打开 */
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = ptGprs->workmode;
                    ptChanInfo->medium = COMM_MEDIA_GPRS;
                    ptChanInfo->connType = ptGprs->contype;
                    ptChanInfo->bFirst = FALSE;
                    ptChanInfo->heart = ptGprs->heart;
                    ptChanInfo->dailcnt = ptGprs->dail.mask.dailcnt;
                    ptChanInfo->dailtime = ptGprs->dail.mask.dailtime;
                    ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + 0;
                    ptIPCMsg->protocol = (MODE_CLIENT == ptGprs->workmode)?PROT_OPEN_CHANNLE : PRO_PARA_MOD;
                    ptChanInfo->ip = ptHandle->masterIP[i];
                    ptChanInfo->port = ptHandle->masterPort[i];
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("%s channel id %d open\n", ifName, ptIPCMsg->id);
                    memset(dataBuf, 0, BUF_LENGTH);
                    usleep(100000);

                    ptHandle->connet_time[i] = (uint32)curtime.tv_sec;
                    dev_state = PPP_SEND_DATA;
                }
                else
                {
                    dev_state = PPP_CONNECT_MASTER;
                }
                break;
            }
            case PPP_SEND_DATA:
            {
                applyTxFlow += comm_ppp_send_proxy(ifuint, ifName, ptHandle, &dev_state);
                break;
            }
            
            case PPP_RECV_DATA:
            {
                applyRxFlow += comm_ppp_recv_proxy(ifuint, ifName, ptHandle, &dev_state);
                break;
            }
            
            case PPP_CLOSE_DEVICE:
            {
                dev_state = PPP_CONNECT_MASTER;
                return -1;
            }
            default:
            {
                dev_state = PPP_SEND_DATA;
                break;
            }
        }
        usleep(10000); /* 网络延时10ms */
    }
    
}

/**********************************************************************
* @name      : comm_ppp_serverworking
* @brief     ：ppp网口工作 服务器模式
* @param[in] ：uint8 ifuint                网口编号
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    : 2019-12-9
**********************************************************************/
int comm_ppp_serverworking(uint8 ifuint, COMM_PPP_HANDLE_T *ptHandle)
{
    char  ifName[8] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    uint8          dev_state = PPP_SEND_DATA;   /* 状态机 */
    uint32           rxtxLen = 0;
    OOP_GPRSCONFIG_T *ptGprs = NULL; 
    uint8  i, j;
    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */
    
    if(NULL == ptHandle || ifuint > 1)
    {
        return -1;
    }

    ptGprs   = &(ptHandle->tGprsCfg.tOopGprs);
    sprintf(ifName, "ppp%d", ifuint);

    ptHandle->listenNum = ptGprs->listenNum;
    *ptHandle->pRunFlag = 0;

    /* 先释放上次没有来及释放的句柄 */
    ppp_close_listen(ptHandle);

    /* 建立监听句柄 */
    if(0 != ppp_open_listen(ifName, ptGprs->listen, ptHandle))
    {
        *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
        COMM_FMT_DEBUG("%s open listen failed\n", ifName);
        return -1;
    }
    ptHandle->bChangeParam = FALSE;
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        *ptHandle->pRunFlag = 0;
        
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            comm_flow_add(TRUE, ifuint, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;

            memcpy(&oldtime, &curtime, sizeof(struct timespec));

            if(FALSE == ptHandle->bDiagOK || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
            {
                /* Appmain重启后 主要通信线程也重启一下 */
                COMM_FMT_DEBUG("%s is not exist or appmain restart\n", ifName);
                dev_state = PPP_CLOSE_DEVICE;
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
                    for(j = 0; j < PPP_CONNECT_NUM; j++)
                    {
                        if(ptHandle->accept_fd[i][j] > 0)
                        {
                            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                            ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                            offset = i *j + OOP_MAX_HOST;
                            ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + offset;
                            ptIPCMsg->protocol = PRO_PARA_MOD;
                            ptChanInfo->ip = ptHandle->accept_ip[i][j];
                            ptChanInfo->port = ptHandle->accept_port[i][j];
                            ptChanInfo->workmode = MODE_SERVER;
                            ptChanInfo->medium = COMM_MEDIA_GPRS;
                            ptChanInfo->connType = TYPE_TCP;
                            ptChanInfo->bFirst = TRUE;
                            ptChanInfo->heart = ptHandle->tGprsCfg.tOopGprs.heart;
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
        
        switch(dev_state)
        {
            case PPP_SEND_DATA:
            {
                /* 需要将所有的消息都发送出去 */
                if(0 != comm_pppserver_send(ifName, ptHandle->ptSendList, ptHandle, &rxtxLen))
                {
                    dev_state = PPP_CLOSE_DEVICE;
                    break;
                }
                applyTxFlow += rxtxLen;
                dev_state = PPP_RECV_DATA;
                break;
            }
            case PPP_RECV_DATA:
            {
                /* 需要将所有的消息都发送出去 */
                if(0 != comm_pppserver_recv(ifuint, ifName, ptHandle->ptRecvList, 
                                            ptHandle->ptRecvListBak, ptHandle, &rxtxLen))
                {
                    dev_state = PPP_CLOSE_DEVICE;
                    break;
                }
                applyRxFlow += rxtxLen;
                dev_state = PPP_SEND_DATA;
                break;
            }
            case PPP_CLOSE_DEVICE:
            {
                uint8   i,j;
                uint8   dataBuf[32] = {0};   /* 缓冲区 */
                COMM_IPCMSG_T  *ptIPCMsg = NULL;
                uint8   offset;
                uint16  recvLen;
                for(i = 0; i < OOP_MAX_LISTENS; i++)
                {
                    for(j = 0; j < PPP_CONNECT_NUM; j++)
                    {
                        if(ptHandle->accept_fd[i][j] <= 0)
                        {
                            continue;
                        }
                        /* 关闭通道 */
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        offset = i *j + OOP_MAX_HOST;
                        ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + offset;
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
                ppp_close_listen(ptHandle);
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
                return -1;
            }
            default:
            {
                dev_state = PPP_SEND_DATA;
                break;
            }
            
        }
        usleep(10000); /* 网络延时10ms */
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    
}

/**********************************************************************
* @name      : comm_ppp_serverworking
* @brief     ：ppp网口工作 UDP连接方式
* @param[in] ：uint8 ifuint                网口编号
               COMM_PPP_HANDLE_T *ptHandle 线程句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-9
* @Update    :
**********************************************************************/
int comm_ppp_udpworking(uint8 ifuint, COMM_PPP_HANDLE_T *ptHandle)
{
    uint8 i, j;
    int   fdSocket = -1;
    char  ifName[8] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    OOP_GPRSCONFIG_T     *ptGprs = NULL; 
    OOP_MASTERPARAMS_T *ptMaster = NULL;
    
    CHANNEL_INFO_T    *ptChanInfo = 0;
    COMM_IPCMSG_T       *ptIPCMsg = NULL;

    uint8              offset = 0;               //通道偏移
    uint8           dev_state = PPP_SEND_DATA;   /* 状态机 */
    uint8 dataBuf[BUF_LENGTH] = {0};             /* 缓冲区 */
    uint16            dataLen = 0;
    int           sendRecvLen = 0;

    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */

    if(NULL == ptHandle || ifuint > 1)
    {
        return -1;
    }

    /* 先释放上次没有来及释放的句柄 */
    comm_ppp_free_allfd(ptHandle);

    ptGprs = &(ptHandle->tGprsCfg.tOopGprs);
    ptMaster = (OOP_MASTERPARAMS_T *)ppp_find_master(ptHandle);
    if(NULL == ptMaster)
    {
        if(0 == ppp_next_diagAPN(ptHandle))
        {
            COMM_FMT_DEBUG("%s connect no master, set next apn\n", ifName);
            ptHandle->bChangeApn = TRUE;
            ptHandle->bGetDCM = FALSE;
        }
        else
        {
            COMM_FMT_DEBUG("%s connect no master, retry later...\n", ifName);
            ptHandle->bGetDCM = FALSE;
        }
        return -1;
    }
    
    sprintf(ifName, "ppp%d", ifuint);

    if(TYPE_TCP == ptGprs->contype)
    {
        COMM_FMT_DEBUG("comm_ppp_udpworking not support tcp\n");
        return -1;    
    }
    
    if(MODE_ZB == ptGprs->conmode)  //主备模式
    {
        ptHandle->masterNUM = 1;
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ptHandle->masterIP[0] = ntohl(*(uint32 *)ptMaster->master[i].ip);
            ptHandle->masterPort[0] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptMaster->master[i].ip);
            for(j = 0; j < 3 && fdSocket <=0; j++)
            {
                fdSocket = comm_init_udp(ifName);
                usleep(100000);
            }
            if(fdSocket > 0)
            {
                COMM_FMT_DEBUG("%s connect IP 0x%x, Port %d", ifName, ptHandle->masterIP, ptHandle->masterPort);
                break;
            }
            comm_del_route_net(ifName, ptMaster->master[i].ip);
        }

    }
    else    //多连接模式
    {
        for(i = 0; i < ptMaster->nNum; i++)
        {
            *ptHandle->pRunFlag = 0;
            ptHandle->masterNUM++;
            ptHandle->masterIP[i] = htonl(*(uint32 *)ptMaster->master[i].ip);
            ptHandle->masterPort[i] = ptMaster->master[i].port;
            comm_add_route_net(ifName, ptMaster->master[i].ip);
        }
        for(j = 0; j < 3 && fdSocket <=0; j++)
        {
            fdSocket = comm_init_udp(ifName);
            usleep(100000);
        }
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
    ptChanInfo->workmode = ptGprs->workmode;
    ptChanInfo->medium = COMM_MEDIA_GPRS;
    ptChanInfo->connType = ptGprs->contype;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = ptGprs->heart;
    ptChanInfo->dailcnt = ptGprs->dail.mask.dailcnt;
    ptChanInfo->dailtime = ptGprs->dail.mask.dailtime;
    for(i = 0; i < ptHandle->masterNUM; i++)
    {
        ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + i;
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
            comm_flow_add(TRUE, ifuint, applyTxFlow, applyRxFlow);
            applyRxFlow = 0;
            applyTxFlow = 0;

            memcpy(&oldtime, &curtime, sizeof(struct timespec));
            if(FALSE == ptHandle->bDiagOK || (NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart))
            {
                comm_ppp_free_allfd(ptHandle);
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
                COMM_FMT_DEBUG("%s is not exist or appmain restart\n", ifName);
                for(i = 0; i < ptHandle->masterNUM; i++)
                {
                    if(ptHandle->client_fd[i] > 0)
                    {
                        /* 关闭所有通道 */
                        ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                        ptIPCMsg->id = (0 == ifuint ? CHL_GPRS_1 : CHL_GPRS_2) + i;
                        ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                        dataLen = sizeof(COMM_IPCMSG_T);
                        if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                        {
                            COMM_FMT_TRACE("Msg Queue is full\n");
                        }
                        close(ptHandle->client_fd[i]);
                        ptHandle->client_fd[i] = 0;
                        COMM_FMT_DEBUG("%s channel id %d close\n", ifName, ptIPCMsg->id);
                        memset(dataBuf, 0, BUF_LENGTH);
                        usleep(100000);
                    }
                }

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
                        ptChanInfo->workmode = ptGprs->workmode;
                        ptChanInfo->medium = COMM_MEDIA_GPRS;
                        ptChanInfo->connType = ptGprs->contype;
                        ptChanInfo->bFirst = TRUE;
                        ptChanInfo->heart = ptGprs->heart;
                        ptIPCMsg->id = ((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + i;
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

        switch(dev_state)
        {
            case PPP_CONNECT_MASTER:
            {
                fdSocket = comm_init_udp(ifName);
                COMM_FMT_DEBUG("%s comm_init_udp %d\n", ifName, fdSocket);
                if(fdSocket > 0)
                {
                    ptHandle->client_fd[0] = fdSocket;
                    dev_state = PPP_SEND_DATA;
                }
                else
                {
                    sleep(1);
                }
                break;
            }
            case PPP_SEND_DATA:
            {
                dataLen = (uint16)comm_read_queue(ptHandle->ptSendList, dataBuf);
                if(dataLen > 0)
                {
                    sendRecvLen = comm_ppp_udpsend(fdSocket, dataBuf, dataLen, ptHandle);
                    COMM_BUF_DEBUG(dataBuf, dataLen, "%s send(%d):", ifName, sendRecvLen);
                    memset(dataBuf, 0, BUF_LENGTH);
                    dataLen = 0;
                    if(sendRecvLen <0)
                    {
                        dev_state = PPP_CLOSE_DEVICE;
                        break;
                    }
                    applyTxFlow += sendRecvLen;
                }
                dev_state = PPP_RECV_DATA;
                break;
            }
            
            case PPP_RECV_DATA:
            {
                sendRecvLen = comm_ppp_udprecv(fdSocket, dataBuf, BUF_LENGTH-4, ptHandle, &offset);
                if(sendRecvLen > 0)  //写入接受队列
                {
                    dataLen = (uint16)sendRecvLen;
                    applyRxFlow += dataLen;

                    /*给数据加上会话协议头部*/
                    comm_add_channel(((0 == ifuint) ? CHL_GPRS_1 : CHL_GPRS_2) + offset, dataBuf, dataLen);
                    dataLen += 4;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_BUF_DEBUG(dataBuf, dataLen, "%s recv:", ifName);
                    memset(dataBuf, 0, BUF_LENGTH);
                    dataLen = 0;
                }
                dev_state = PPP_SEND_DATA;
                break;
            }
            
            case PPP_CLOSE_DEVICE:
            {
                close(ptHandle->client_fd[0]);
                ptHandle->client_fd[0] = -1;
                dev_state = PPP_CONNECT_MASTER;
                break;
            }
            default:
            {
                dev_state = PPP_SEND_DATA;
                break;
            }
        }
        usleep(10000); /* 网络延时10ms */
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

}

/**********************************************************************
* @name      : comm_getcheck_pppcfg
* @brief     ：获取并检查ppp配置是否发生变化
* @param[in] ：uint8 ifuint      接口编号
               void *dbHandle    数据中心句柄
               void *pCfg        配置存储地址
* @param[out]：uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
int comm_getcheck_pppcfg(uint8 ifuint, int sockdbfd, void *pCfg, uint8 *pRestart)
{
    uint8 i;
    int retGprs = 0;
    int retMaster = 0;
    int retAPN = 0;
    uint32 readLen = 0;
    uint8  bChange = FALSE;
    uint8  bRestart = FALSE;
    NOMAL_OAD_T        tOAD;
    OOP_GPRSCONFIG_T   tGPRS;
    OOP_MASTERPARAMS_T tMaster;
    OOP_APNGROUP_T     tAPN;
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pCfg;
    static uint8 s_pppcfg[2][4] = {{0}, {0}};  //首次获取失败打印，防止日志过多

    if(ifuint > 1 || sockdbfd <= 0 || NULL == pCfg)
    {
        return -1;
    }
    
    bzero(&tOAD, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = (0 == ifuint) ? 0x45000200 : 0x45010200;
    memset(&tGPRS, 0, sizeof(OOP_GPRSCONFIG_T));
    retGprs = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_GPRSCONFIG_T), (uint8 *)&tGPRS, &readLen);
    if(0 == retGprs)
    {
        if(readLen != sizeof(OOP_GPRSCONFIG_T))
        {
            retGprs = -1;
        }
        else if(0 != memcmp(&(ptHandle->tGprsCfg.tOopGprs), &tGPRS, readLen))
        {
            bChange = TRUE;
        }
    }
    else if(FALSE == s_pppcfg[ifuint][0])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retGprs);
        s_pppcfg[ifuint][0] = TRUE;
    }

    readLen = 0;
    tOAD.oad.value = (0 == ifuint) ? 0x45000300 : 0x45010300;
    memset(&tMaster, 0, sizeof(OOP_MASTERPARAMS_T));
    retMaster = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_MASTERPARAMS_T), (uint8 *)&tMaster, &readLen);
    if(0 == retMaster)
    {
        if(readLen != sizeof(OOP_MASTERPARAMS_T))
        {
            retMaster = -1;
        }
        else if(0 != memcmp(&(ptHandle->tGprsCfg.tOopMaster), &tMaster, readLen))
        {
            bChange = TRUE;
        }
    }
    else if(FALSE == s_pppcfg[ifuint][1])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retMaster);
        s_pppcfg[ifuint][1] = TRUE;
    }

    readLen = 0;
    tOAD.oad.value = (0 == ifuint) ? 0x45000E00 : 0x45010E00;
    memset(&tAPN, 0, sizeof(OOP_APNGROUP_T));
    retAPN = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_APNGROUP_T), (uint8 *)&tAPN, &readLen);
    if(0 == retAPN)
    {
        /* 698规约和APP协议关于运营商 联通、电信是反的 */
        for(i = 0; i < tAPN.nNum; i++)
        {
            if(1 == tAPN.net[i].factory)
            {
                tAPN.net[i].factory = 2;
            }
            else if(2 == tAPN.net[i].factory)
            {
                tAPN.net[i].factory = 1;
            }
        }
        if(readLen != sizeof(OOP_APNGROUP_T))
        {
            retAPN = -1;
            memset(&tAPN, 0, sizeof(OOP_APNGROUP_T));
        }
        else if(0 != memcmp(&(ptHandle->tGprsCfg.tAPN), &tAPN, readLen))
        {
            bChange = TRUE;
        }
    }
    else if(FALSE == s_pppcfg[ifuint][2])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retAPN);
        s_pppcfg[ifuint][2] = TRUE;
    }
    
#if defined(AREA_JIANGSU)&&defined(ADV_ST_AMR)
    //江苏专变有默认配置 写到多网络参数中
    if(retAPN != 0)
    {
        apn_group_default_set(&tAPN);
        COMM_FMT_TRACE("江苏使用默认参数 \n");
        retAPN = 0;
        if(0 != memcmp(&(ptHandle->tGprsCfg.tAPN), &tAPN, sizeof(OOP_APNGROUP_T)))
        {
            bChange = TRUE;
        }
    }
#endif

    if(TRUE == bChange)
    {
        /* 查看是否需要重启 */
        if(0 == retGprs)
        {
            if(ptHandle->tGprsCfg.tOopGprs.workmode != tGPRS.workmode || 
               ptHandle->tGprsCfg.tOopGprs.contype != tGPRS.contype ||
               ptHandle->tGprsCfg.tOopGprs.conmode != tGPRS.conmode ||
               (0 == ptHandle->apnIndex && 0 != memcmp(&(ptHandle->tGprsCfg.tOopGprs.apn), &(tGPRS.apn), sizeof(OOP_APN_T))))
            {
                bRestart = TRUE;
            }
            else
            {
                ptHandle->bChangeParam = TRUE;
            }

            memcpy(&(ptHandle->tGprsCfg.tOopGprs), &tGPRS, sizeof(tGPRS));
        }
        if(0 == retMaster)
        {
            if(0 == ptHandle->apnIndex && 0 != memcmp(&(ptHandle->tGprsCfg.tOopMaster), &tMaster, sizeof(tMaster)))
            {
                bRestart = TRUE;
            }
            memcpy(&(ptHandle->tGprsCfg.tOopMaster), &tMaster, sizeof(tMaster));
        }
        if(0 == retAPN)
        {
            /* 查看拨号APN */
            if(0 != memcmp(&(ptHandle->tGprsCfg.tAPN), &tAPN, sizeof(tAPN)))
            {
                if(ptHandle->apnIndex > 0)
                {
                    i = ptHandle->apnIndex - 1;
                    if(ptHandle->apnIndex > tAPN.nNum || 
                       0 != memcmp(&(ptHandle->tGprsCfg.tAPN.net[i]), &(tAPN.net[i]), sizeof(OOP_NETCONFIG_T)))
                    {
                        bRestart = TRUE;
                    }
                }
                else if(0 == tMaster.nNum)
                {
                    bRestart = TRUE;
                }
            }
            memcpy(&(ptHandle->tGprsCfg.tAPN), &tAPN, sizeof(tAPN));
        }
    }

    if(TRUE == bRestart)
    {
        sleep(10); //等待10s后重启
        COMM_FMT_DEBUG("无线%d参数变化, 需要重新登录.\n", ifuint);
        uint8 routeAddr[4] = {0};
        /* 清理工作 例如路由 */
        for(i = 0; i < ptHandle->masterNUM; i++)
        {
            if(0 == ptHandle->masterIP[i])
            {
                continue;
            }
            routeAddr[0] = (ptHandle->masterIP[i] >> 24) & 0xff;
            routeAddr[1] = (ptHandle->masterIP[i] >> 16) & 0xff;
            routeAddr[2] = (ptHandle->masterIP[i] >> 8) & 0xff;
            routeAddr[3] = ptHandle->masterIP[i] & 0xff;
            comm_del_route_net((0 == ifuint) ? "ppp0" : "ppp1", routeAddr);
        }
        //不能清空路由了
        //comm_del_route_net((0 == ifuint) ? "ppp0" : "ppp1", NULL);

        if(TRUE == ptHandle->bGetCfg)
        {
            *ptHandle->pAppReStart = 1;
        }
        sleep(2);
        if(pRestart)
        {
            *pRestart = TRUE;
        }
        ptHandle->getApnTime = 0; //初始化获取时间
        ptHandle->bGetDCM = FALSE;
        ptHandle->bGetApn = TRUE;
    }

    ptHandle->bSingleCfg = (0 == retGprs && 0 == retMaster) ? TRUE : FALSE;
    if(0 == tMaster.nNum)
    {
        ptHandle->bSingleCfg = FALSE;
    }
    ptHandle->bMoreCfg = (0 == retGprs && 0 == retAPN) ? TRUE : FALSE;
    if(0 == tAPN.nNum)
    {
        ptHandle->bMoreCfg = FALSE;
    }
    ptHandle->bGetCfg = (FALSE == ptHandle->bSingleCfg && FALSE == ptHandle->bMoreCfg) ? FALSE : TRUE;

    return retGprs | retMaster | retAPN;
}

/**********************************************************************
* @name      : comm_getcheck_ppp0cfg
* @brief     ：获取并检查ppp0配置是否发生变化
* @param[in] ：int sockdbfd      通信句柄
               void *pCfg        配置存储地址
* @param[out]：uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
int comm_getcheck_ppp0cfg(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    return comm_getcheck_pppcfg(0, sockdbfd, pCfg, pRestart);
}

/**********************************************************************
* @name      : comm_getcheck_ppp1cfg
* @brief     ：获取并检查ppp1配置是否发生变化
* @param[in] ：int sockdbfd      通信句柄
* @param[out]：void *pCfg        配置存储地址
               uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-5
* @Update    :
**********************************************************************/
int comm_getcheck_ppp1cfg(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    return comm_getcheck_pppcfg(1, sockdbfd, pCfg, pRestart);
}

/**********************************************************************
* @name      : comm_set_rediag
* @brief     ：设置拨号标记
* @param[in] ：uint8 id  设备id 1,2
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-7-25
* @Update    : 
**********************************************************************/
void comm_set_rediag(uint8 id)
{
    if(1 == id)
    {
        g_commrediag1 = 1;
    }
    if(2 == id)
    {
        g_commrediag2 = 1;
    }
}

/**********************************************************************
* @name      : comm_timer_rediag
* @brief     ：重新拨号操作
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-7-25
* @Update    : 
**********************************************************************/
void comm_timer_rediag(int socketfd, void *pArg)
{
    uint8 id = 0;

    /* 重新拨号 */
    if(0 != g_commrediag1)
    {
        id = 1;
        sleep(1);
        comm_send_msg(MSG_COMM_REDIAG, &id, 1);
        COMM_FMT_DEBUG("comm_timer_rediag 1\n");
        g_commrediag1 = 0;
    }
    if(0 != g_commrediag2)
    {
        id = 2;
        sleep(1);
        comm_send_msg(MSG_COMM_REDIAG, &id, 1);
        COMM_FMT_DEBUG("comm_timer_rediag 2\n");
        g_commrediag2 = 0;
    }
    return;
}

/**********************************************************************
* @name      : comm_timer_getDCM
* @brief     ：DCM查询消息定时器(查询拨号参数)
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-1-7
* @Update    : 2020-7-24
**********************************************************************/
void comm_timer_getDCM(int socketfd, void *pArg)
{
    int  ret = 0;
    uint8 id = 0;
    struct timespec curtime = {0};
    static uint8 s_getDCMFirst[2] = {TRUE, TRUE};
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;

    if(socketfd <= 0 || NULL == ptHandle)
    {
        COMM_FMT_DEBUG("comm_timer_getDCM input null\n");
        return;
    }
    
    id = ptHandle->no + 1;
    if(TRUE == s_getDCMFirst[ptHandle->no])
    {
        g_PPPDiagFlag[ptHandle->no] = &ptHandle->bDiagOK;
        ptPPPHandle[ptHandle->no] = ptHandle;
        ptHandle->bFirstRun = TRUE;
        ret = task_send_labelmsg(socketfd, MSG_COMM_GET_MODULEINFO, id, &id, sizeof(uint8), MSG_MONITOR);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Comm send get dial %d msg, ret %d\n", id, ret);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    /* 首次执行需要查询一下拨号状态 2min没有拨号成功 重新查询 */
    if(TRUE == s_getDCMFirst[ptHandle->no] || TRUE == ptHandle->bGetApn || curtime.tv_sec >= ptHandle->getDiagTime + 60)
    {
        ret = task_send_labelmsg(socketfd, MSG_COMM_GETDIAL, id, &id, sizeof(uint8), MSG_MONITOR);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Comm send get dial %d msg, ret %d\n", id, ret);
        }
        s_getDCMFirst[ptHandle->no] = FALSE;
        ptHandle->bGetApn = FALSE;
        ptHandle->getDiagTime = curtime.tv_sec;
    }
    return;
}

/**********************************************************************
* @name      : comm_timer_getMode
* @brief     ：DCM查询消息定时器(查询模组信息)
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-11-4
* @Update    : 2020-7-24
**********************************************************************/
void comm_timer_getMode(int socketfd, void *pArg)
{
    int  ret = 0;
    uint8 id = 0;
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;

    if(socketfd <= 0 || NULL == ptHandle)
    {
        COMM_FMT_DEBUG("comm_timer_getDCM input null\n");
        return;
    }
    
    id = ptHandle->no + 1;
    /* 2min没有拨号成功 重新查询 */
    if(FALSE == ptHandle->bDiagOK)
    {
        ret = task_send_labelmsg(socketfd, MSG_COMM_GET_MODULEINFO, id, &id, sizeof(uint8), MSG_MONITOR);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Comm send get dial %d msg, ret %d\n", id, ret);
        }
    }
}


/**********************************************************************
* @name      : comm_timer_nextapn
* @brief     ：无线模块切换APN
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-1-16
* @Update    :
**********************************************************************/
void comm_timer_nextapn(int socketfd, void *pArg)
{
    int  ret = 0;
    uint8 id = 0;
    WIRE_DIAG_APN_T tDiagAPN;
    OOP_APN_T *ptApn = NULL;
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    char  *operatename[] = {"移动", "联通", "电信", "专网"};
    char  *authkname[] = {"null", "chap", "pap", "pap&chap"};
    char  *networkname[] = {"auto", "2G", "3G", "4G", "5G"};

    if(socketfd <= 0 || NULL == pArg || FALSE == ptHandle->bChangeApn)
    {
        return;
    }

    uint8  msgbuf[2048]; /* 不需要初始化 */
    uint16 msglen = 0;

    id = ptHandle->no + 1;
    tDiagAPN.id = id;
    tDiagAPN.diagnum = 1;
    tDiagAPN.tDiagApn[0].operat = ptHandle->operat;

    if(ptHandle->apnIndex > ptHandle->tGprsCfg.tAPN.nNum)
    {
        ptHandle->apnIndex = 0;
    }
    if(0 == ptHandle->apnIndex)
    {
        if(0 == ptHandle->tGprsCfg.tOopGprs.apn.apn.nNum)
        {
            ppp_next_diagAPN(ptHandle);
            if(0 == ptHandle->apnIndex)
            {
                ptHandle->bChangeApn = FALSE;
            }
            return;
        }
        memcpy(&(tDiagAPN.tDiagApn[0].apn), &(ptHandle->tGprsCfg.tOopGprs.apn.apn), sizeof(OOP_VISIBLEVAR_T));
        memcpy(&(tDiagAPN.tDiagApn[0].username), &(ptHandle->tGprsCfg.tOopGprs.apn.username), sizeof(OOP_VISIBLEVAR_T));
        memcpy(&(tDiagAPN.tDiagApn[0].pwd), &(ptHandle->tGprsCfg.tOopGprs.apn.pwd), sizeof(OOP_VISIBLEVAR_T));
        tDiagAPN.authtype = 255;
        tDiagAPN.network = 0;
    }
    else if(ptHandle->apnIndex <= ptHandle->tGprsCfg.tAPN.nNum)
    {
        ptApn = &(ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].apn);

        memcpy(&(tDiagAPN.tDiagApn[0].apn), &(ptApn->apn), sizeof(OOP_VISIBLEVAR_T));
        memcpy(&(tDiagAPN.tDiagApn[0].username), &(ptApn->username), sizeof(OOP_VISIBLEVAR_T));
        memcpy(&(tDiagAPN.tDiagApn[0].pwd), &(ptApn->pwd), sizeof(OOP_VISIBLEVAR_T));
        tDiagAPN.authtype = ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].auth;
        if(ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].network <= 3)
        {
            tDiagAPN.network = ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].network + 1;
        }
    }
    else
    {
        ptHandle->bChangeApn = FALSE;
    }
    if(0 == tDiagAPN.tDiagApn[0].apn.nNum)
    {
        COMM_FMT_DEBUG("PPP %d not set apn %d msg\n", id-1);
        return;
    }

#ifdef AREA_FUJIAN
     /*
        APN用户名格式为：移动GPRS为A@fjep.fj，电信CDMA为A@fjep.vpdn.fj。
        当APN为fjep.fj时，设备应能够根据模块类型和设备编码自动修改用户名，APN用户名为固定参数，不能清除或修改，也不随软件升级而变动；
        当APN为其他名称时可手动设置用户名和密码。
        A是终端地址
    */
    if(0 == strcmp("fjep.fj", tDiagAPN.tDiagApn[0].apn.value) || 0 == strcmp("FJEP.FJ", tDiagAPN.tDiagApn[0].apn.value))
    {
        uint8  addrlen = 0;
        uint8 *pAddr = NULL;
        uint8  i = 0;

        /* 密码取终端地址 */
        memset(&(tDiagAPN.tDiagApn[0].pwd), 0, sizeof(OOP_VISIBLEVAR_T));
        addrlen = app_get_clientNO(&pAddr);
        if(0 == addrlen)
        {
            memset(tDiagAPN.tDiagApn[0].pwd.value, '0', 12);
            tDiagAPN.tDiagApn[0].pwd.nNum = 12;
        }
        else
        {
            for(i = 0; i < addrlen; i++)
            {
                tDiagAPN.tDiagApn[0].pwd.value[2 * (addrlen - i) - 2] = (pAddr[i] & 0x0f) + '0';
                tDiagAPN.tDiagApn[0].pwd.value[2 * (addrlen - i) - 1] = ((pAddr[i] >> 4) & 0x0f) + '0';
                tDiagAPN.tDiagApn[0].pwd.nNum += 2;
            }
        }
        
        /* 用户名增加后缀 */
        memset(&(tDiagAPN.tDiagApn[0].username), 0, sizeof(OOP_VISIBLEVAR_T));
        MEMCPY_R(tDiagAPN.tDiagApn[0].username.value, tDiagAPN.tDiagApn[0].pwd.value, tDiagAPN.tDiagApn[0].pwd.nNum);
        tDiagAPN.tDiagApn[0].username.nNum = tDiagAPN.tDiagApn[0].pwd.nNum;
        if(OOP_CUCC == ptHandle->operat && OOP_2G == ptHandle->nettype)
        {
            /* CDMA */
            memcpy(tDiagAPN.tDiagApn[0].username.value + tDiagAPN.tDiagApn[0].username.nNum, "@fjep.vpdn.fj",
                   strlen("@fjep.vpdn.fj"));
            tDiagAPN.tDiagApn[0].username.nNum += strlen("@fjep.vpdn.fj");
        }
        else
        {
            /* 其他 */
            memcpy(tDiagAPN.tDiagApn[0].username.value + tDiagAPN.tDiagApn[0].username.nNum, "@fjep.fj",
                   strlen("@fjep.fj"));
            tDiagAPN.tDiagApn[0].username.nNum += strlen("@fjep.fj");
        }
    }
#endif

    msglen = appmsg_wireapn_package(&tDiagAPN, msgbuf, 2048);
    if(msglen > 0)
    {
        memcpy(ptHandle->targetApn, tDiagAPN.tDiagApn[0].apn.value, tDiagAPN.tDiagApn[0].apn.nNum);
        ret = task_send_labelmsg(socketfd, MSG_COMM_SETAPN, id, msgbuf, msglen, MSG_MONITOR);
        COMM_FMT_DEBUG("无线%d设置APN[%s-%s-%s], 运营商%d(%s), 认证方式%d(%s), 网络%d(%s), ret = %d\n", 
                       id, tDiagAPN.tDiagApn[0].apn.value, tDiagAPN.tDiagApn[0].username.value,
                       tDiagAPN.tDiagApn[0].pwd.value, tDiagAPN.tDiagApn[0].operat, 
                       tDiagAPN.tDiagApn[0].operat <= 3 ? operatename[tDiagAPN.tDiagApn[0].operat] : "未知",
                       tDiagAPN.authtype, tDiagAPN.authtype <= 3 ? authkname[tDiagAPN.authtype] : "自适应",
                       tDiagAPN.network, tDiagAPN.network <= 4 ? networkname[tDiagAPN.network] : "未知", ret);
    }
}

/**********************************************************************
* @name      : comm_timer_pppmastergui
* @brief     ：主站链接定时同时显示
* @param[in] ：int socketfd    task消息句柄
               void *pArg      入参
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-7-1
* @Update    :
**********************************************************************/
void comm_timer_pppmastergui(int socketfd, void *pArg)
{
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    uint8              buf[8] = {0};
    uint16             len = 0;
    int                ret = 0;
    static uint8       s_FirstStatus[2] = {1, 1};

    if(socketfd <= 0 || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_timer_mastergui input null\n");
        return;
    }

    if(1 == ptHandle->bMasterGui)
    {
        buf[len++] = 1;
        buf[len++] = ptHandle->no;
        buf[len++] = 2;
        buf[len++] = 1;
    }
    else if(2 == ptHandle->bMasterGui || 1 == s_FirstStatus[ptHandle->no])
    {
        buf[len++] = 1;
        buf[len++] = ptHandle->no;
        buf[len++] = 2;
        buf[len++] = 0;
        s_FirstStatus[ptHandle->no] = 0;
    }
    else
    {
        return;
    }

    ret = comm_send_label_msg(MSG_COMM_GUITOOLS, MSG_COMM_FLAG + 0x10 + ptHandle->no, buf, len);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("Send gui tool %d failed\n", buf[1]);
    }
}

/**********************************************************************
* @name      : comm_ppp_dcmmsg
* @brief     ：DCM查询消息解析(重点是校验APN)
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-11-5
* @Update    : 2020-7-24
**********************************************************************/
int comm_ppp_dcmmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    struct timespec curtime = {0};

    WIRE_DIAG_INFO_T tDiagInfo;
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    char  *operatename[] = {"移动", "联通", "电信", "专网"};
    char  *networkname[] = {"2G", "3G", "4G", "5G"};

    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_ppp_dcmmsg input null\n");
        return -1;
    }

    appmsg_wirediag_split(playload, &tDiagInfo);
   
    if(0 == tDiagInfo.diagstatus)
    {
        ptHandle->bDiagOK = TRUE;
    }
    else
    {
        ptHandle->bDiagOK = FALSE;
    }
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    
    ptHandle->operat = tDiagInfo.operat;
    ptHandle->nettype = tDiagInfo.networkType;

    /* 没有SIM卡或者没有配置 */
    if(1 != tDiagInfo.simstatus || FALSE == ptHandle->bGetCfg)
    {
        ptHandle->getApnTime = curtime.tv_sec;
        memset(ptHandle->curApn, 0, 64);
        ptHandle->curApn[0] = ',';
        return 0;
    }

    if(0 == tDiagInfo.APN.nNum || 0 != strcmp(ptHandle->curApn, tDiagInfo.APN.value) || 
       ptHandle->operat != tDiagInfo.operat || ptHandle->nettype != tDiagInfo.networkType)
    {
        COMM_FMT_DEBUG("查询无线%dAPN [%s]，运营商%d(%s), 网络%d(%s)\n", label, tDiagInfo.APN.value, tDiagInfo.operat, 
                       tDiagInfo.operat <= 3 ? operatename[tDiagInfo.operat] : "未知", tDiagInfo.networkType,
                       tDiagInfo.networkType <= 3 ? networkname[tDiagInfo.networkType] : "未知");
    }
    memcpy(ptHandle->curApn, tDiagInfo.APN.value, MIN(63, tDiagInfo.APN.nNum));
    ptHandle->operat = tDiagInfo.operat;
    ptHandle->nettype = tDiagInfo.networkType;

    /* 检查APN是否符合当前APN */
    if(0 == ptHandle->apnIndex)
    {
        if(0 == ptHandle->tGprsCfg.tOopGprs.apn.apn.nNum || 0 == strlen(ptHandle->tGprsCfg.tOopGprs.apn.apn.value)
           || 0 == strcmp(ptHandle->tGprsCfg.tOopGprs.apn.apn.value, tDiagInfo.APN.value))
        {
            if(TRUE == ptHandle->bFirstRun)
            {
                ptHandle->bFirstRun = FALSE;
                ptHandle->apnIndex = 0;
                ptHandle->bChangeApn = TRUE;
                COMM_FMT_DEBUG("首次运行, 设置APN\n");
            }
            else
            {
                ptHandle->bChangeApn = FALSE;
                ptHandle->bGetDCM = TRUE;
            }
            ptHandle->getApnTime = curtime.tv_sec;
            return 0;
        }
    }
    else if(ptHandle->apnIndex <= ptHandle->tGprsCfg.tAPN.nNum)
    {
        /* 自适应 */
        if(0 == strcmp(ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].apn.apn.value, tDiagInfo.APN.value)
            && (tDiagInfo.operat == ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].factory || 
               255 == ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].factory)
            && (tDiagInfo.networkType == ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].network ||
               255 == ptHandle->tGprsCfg.tAPN.net[ptHandle->apnIndex-1].network))
        {
            if(TRUE == ptHandle->bFirstRun)
            {
                ptHandle->bFirstRun = FALSE;
                ptHandle->apnIndex = 0;
                ptHandle->bChangeApn = TRUE;
                COMM_FMT_DEBUG("首次运行, 设置APN\n");
            }
            {
                ptHandle->bChangeApn = FALSE;
                ptHandle->bGetDCM = TRUE;
            }
            ptHandle->getApnTime = curtime.tv_sec;
            return 0;
        }
    }

    ptHandle->bGetDCM = FALSE;
    if(TRUE == ptHandle->bFirstRun)
    {
        ptHandle->bFirstRun = FALSE;
        ptHandle->apnIndex = 0;
        ptHandle->bChangeApn = TRUE;
        COMM_FMT_DEBUG("首次运行, 设置APN\n");
        ptHandle->getApnTime = curtime.tv_sec;
    }
    else if(curtime.tv_sec > ptHandle->getApnTime + 300)
    {
        /* 5min没有完成 才切换APN */
        ppp_next_diagAPN(ptHandle);
        /* 不存在, 发送切换APN */
        COMM_FMT_DEBUG("无线%dAPN[%s]与配置不一致\n", label, tDiagInfo.APN.value);
        ptHandle->bChangeApn = TRUE;
        ptHandle->getApnTime = curtime.tv_sec;
    }
    else
    {
        COMM_FMT_DEBUG("无线%dAPN切换不满5min, 等待\n", label);
        ptHandle->bGetApn = TRUE;
    }
    return 0;

}

extern void comm_add_writedb(uint32 oad, uint8 *buf, uint16 len);

/**********************************************************************
* @name      : comm_ppp_modemsg
* @brief     ：DCM查询模块消息解析
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-6-9
* @Update    : 
**********************************************************************/
int comm_ppp_modemsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    WIRELESS_INFO_T  tModeInfo;
    OOP_VISIBLEVAR_T dev; 
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    OOP_VERSION_T data;
    char         temp[400] = {0}; 

    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_ppp_dcmmsg input null\n");
        return -1;
    }

    appmsg_wireinfo_split(playload, &tModeInfo);
   
    memset(&data, 0, sizeof(OOP_VERSION_T));
    memcpy(&data.szFactory.value[0], tModeInfo.version.manuNum, 4);
    memcpy(&data.szSoftVer.value[0], tModeInfo.version.softversion, 4);
    memcpy(&data.szSoftDate.value[0], tModeInfo.version.softdata, 6);
    memcpy(&data.szHardVer.value[0], tModeInfo.version.hwversion, 4);
    memcpy(&data.szHardDate.value[0], tModeInfo.version.hwdata, 6);
    memcpy(&data.szExtend.value[0], tModeInfo.version.manuExten, 8);

    if(0 == tModeInfo.IMEI.num)
    {
        return 0;
    }

    if(0 != memcmp(&(ptHandle->version), &data, sizeof(OOP_VERSION_T)) && 0 != tModeInfo.version.manuNum[0])
    {
        memcpy(&(ptHandle->version), &data, sizeof(OOP_VERSION_T));
        comm_add_writedb(1 == label ? 0x45000500 : 0x45010500, (uint8*)&data, sizeof(OOP_VERSION_T));
        COMM_FMT_DEBUG("UPDATE version, 0x%x!\n", 1 == label ? 0x45000500 : 0x45010500);
    }

    if(2 != tModeInfo.typeinfo.num)
    {
        dev.nNum = sprintf(temp, "type=3F;id=%s", tModeInfo.IMEI.value);
    }
    else
    {
        dev.nNum = sprintf(temp, "type=%s;id=%s", tModeInfo.typeinfo.value, tModeInfo.IMEI.value);
    }
    memcpy(dev.value, temp, dev.nNum);
    if(0 == memcmp(&(ptHandle->DevIMEI), &dev, sizeof(OOP_VISIBLEVAR_T)))
    {
        return 0;
    }
    memcpy(&(ptHandle->DevIMEI), &dev, sizeof(OOP_VISIBLEVAR_T));
    comm_add_writedb(1 == label ? 0x45000C00 : 0x45010C00, (uint8*)&(ptHandle->DevIMEI), sizeof(OOP_VISIBLEVAR_T));

    /* 模块变更检查 */
    if(tModeInfo.IMEI.num > 0)
    {
        if(ptHandle->OldDescript.nNum > 0 && 0 != memcmp(&(ptHandle->OldDescript), &dev, sizeof(OOP_VISIBLEVAR_T)))
        {
            OOP_VISIBLEVAR_T devdesc[2];
            memcpy(&(devdesc[0]), &(ptHandle->OldDescript), sizeof(OOP_VISIBLEVAR_T));
            memcpy(&(devdesc[1]), &dev, sizeof(OOP_VISIBLEVAR_T));
            comm_add_writedb(1 == label ? 0x45000000 : 0x45010000, (uint8*)devdesc, 2 * sizeof(OOP_VISIBLEVAR_T));
        }
        memcpy(&(ptHandle->OldDescript), &dev, sizeof(OOP_VISIBLEVAR_T));
    }
    
    return 0;    
}

/**********************************************************************
* @name      : comm_ppp_eventmsg
* @brief     ：无线模块通知消息解析
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-3-18
* @Update    : 
**********************************************************************/
int comm_ppp_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    uint8 port;
    uint8 status;
    int   ret = 0;

    if(NULL == playload)
    {
        COMM_FMT_DEBUG("comm_ppp_eventmsg input null\n");
        return -1;
    }

    port = *playload;
    status = *(playload + 1);
    if(0 == port || port > 2)
    {
        COMM_FMT_DEBUG("comm_ppp_eventmsg port %d err\n", port);
        return -1;
    }
    COMM_FMT_DEBUG("comm get event port %d status, %d\n", port, status);

    if(0 == status)
    {
        if(g_PPPDiagFlag[port-1])
        {
            *(g_PPPDiagFlag[port-1]) = TRUE;
        }
    }
    else
    {
        if(g_PPPDiagFlag[port-1])
        {
            *(g_PPPDiagFlag[port-1]) = FALSE;
        }
    }

    /* 上下线查询一下 */
    sleep(5);
    comm_send_get_pppmode(port);
    ret = comm_send_label_msg(MSG_COMM_GETDIAL, port, &port, sizeof(uint8));
    if(0 != ret)
    {
        COMM_FMT_DEBUG("Comm send get dial %d msg, ret %d\n", port, ret);
    }

    return 0;    
}

/**********************************************************************
* @name      : comm_ppp_apnmsg
* @brief     ：收到APN设置答复
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-4-26
* @Update    : 
**********************************************************************/
int comm_ppp_apnmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;

    if(NULL == playload || 0 == msglen || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_ppp_apnmsg input null\n");
        return -1;
    }

    if(0 != *playload)
    {
        ptHandle->bChangeApn = FALSE;
        ptHandle->bGetApn = TRUE;
    }
    return 0;
}

/**********************************************************************
* @name      : comm_ppp_guishow
* @brief     ：连接状态更新显示
* @param[in] ：uint8 id       通道
               uint8 protocol 内部协议     
               void *pArg     私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-4-1
* @Update    : 2020-4-27
**********************************************************************/
int comm_ppp_guishow(uint8 id, uint8 protocol, void *pArg)
{
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    OOP_GPRSCONFIG_T  *ptGprs = NULL;
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

    ptGprs = &(ptHandle->tGprsCfg.tOopGprs);

    /* 刷新状态栏 前4个字节填写120 是时间 */
    appmsg_memcpy_r(buf, &outtime, sizeof(uint32));

    if(PROT_OPEN_ACK == protocol)
    {
        len = sprintf((char *)buf + 5, "无线%d连接主站", ptHandle->no + 1);
    }
    else if(PROT_CLOSE_ACK == protocol)
    {
        len = sprintf((char *)buf + 5, "无线%d断开主站连接", ptHandle->no + 1);
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
        buf[len++] = ptHandle->no;
        buf[len++] = 2;
        buf[len++] = 1;
    }
    else if(PROT_CLOSE_ACK == protocol)
    {
        if(MODE_SERVER == ptGprs->workmode)  //服务器模式
        {
            /* 获取连接数量 */
            for(i = 0; i < OOP_MAX_LISTENS; i++)
            {
                for(j = 0; j < PPP_CONNECT_NUM; j++)
                {
                    if(ptHandle->accept_fd[i][j] > 0)
                    {
                        num++;
                    }
                }
            }
        }
        else if(MODE_CLIENT == ptGprs->workmode)
        {
            for(i = 0; i < ptHandle->masterNUM; i++)
            {
                if(ptHandle->client_fd[i] > 0)
                {
                    num++;
                }
            }
        }
        else if(MODE_MIX == ptGprs->workmode)
        {
            if(ptHandle->masterNUM > 0)
            {
                num++;
            }
        }

        if(0 < num) //还存在连接不刷新工具栏
        {
            return 0;
        }
        buf[len++] = 1;
        buf[len++] = ptHandle->no;
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
            COMM_FMT_DEBUG("Send gui tool %d failed\n", buf[1]);
        }
    }
    
    return ret;
}

/**********************************************************************
* @name      : comm_ppp_start
* @brief     ：检查是否需要启动ppp线程
* @param[in] ：
* @param[out]：
* @return    ：TRUE-启动/FALSE-不启动
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
BOOLEAN comm_ppp_start(void *pArg)
{
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;

    if(NULL == pArg)
    {
        return FALSE;
    }

    if(FALSE == ptHandle->bGetCfg || FALSE == ptHandle->bGetDCM || 0 == *ptHandle->pIPCok || FALSE == ptHandle->bDiagOK)
    {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************
* @name      : comm_ppp0_entry
* @brief     ：ppp0端口消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-10-31
* @Update    :
**********************************************************************/
void *comm_ppp0_entry(void *pArg)
{
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    OOP_GPRSCONFIG_T *ptGprs = NULL;

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

    ptGprs = &(ptHandle->tGprsCfg.tOopGprs);

    COMM_FMT_DEBUG("ppp0 start: contype(%d), workmode(%d).\n", ptGprs->contype, ptGprs->workmode);
    if(1 == ssal_get_status())
    {
        comm_ppp_proxy_working(0, ptHandle);
    }
    else
    {
        if(TYPE_UDP == ptGprs->contype)
        {
            comm_ppp_udpworking(0, ptHandle);
        }
        else if(MODE_SERVER == ptGprs->workmode)  //服务器模式
        {
            comm_ppp_serverworking(0, ptHandle);
        }
        else
        {
            if(MODE_CLIENT == ptGprs->workmode)
            {
                if(app_comm_proxy_isuse() == FALSE) //通信代理只关注客户机模式
                {
                    comm_ppp_working(0, ptHandle);
                }
                else
                {
                    comm_ppp_proxy(0, ptHandle);
                }
            }
            else
            {
                comm_ppp_working(0, ptHandle);    
            }
        }
    }
    //ptHandle->bGetDCM = FALSE;
    return 0;
}

/**********************************************************************
* @name      : comm_ppp1_entry
* @brief     ：ppp1端口消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-10-31
* @Update    :
**********************************************************************/
void *comm_ppp1_entry(void *pArg)
{
    COMM_PPP_HANDLE_T *ptHandle = (COMM_PPP_HANDLE_T *)pArg;
    OOP_GPRSCONFIG_T *ptGprs = NULL;

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

    ptGprs = &(ptHandle->tGprsCfg.tOopGprs);

    COMM_FMT_DEBUG("ppp1 start: contype(%d), workmode(%d).\n", ptGprs->contype, ptGprs->workmode);
    
    if(1 == ssal_get_status())
    {
        comm_ppp_proxy_working(1, ptHandle);
    }
    else
    {
        if(TYPE_UDP == ptGprs->contype)
        {
            comm_ppp_udpworking(1, ptHandle);
        }
        else if(MODE_SERVER == ptGprs->workmode)  //服务器模式
        {
            comm_ppp_serverworking(1, ptHandle);
        }
        else
        {
            comm_ppp_working(1, ptHandle);
        }
    }
    //ptHandle->bGetDCM = FALSE;
    return 0;
}

