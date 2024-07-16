/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块监听
* @date：    2021-2-3
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
#include "listen_comm.h"
#include "eth_comm.h"
#include "ppp_comm.h"

#define  COMM_LISTEN_PORT         5100  //复用104的网口


/**********************************************************************
* @name      : eth_listen_start
* @brief     ：以太网监听 启动判断
* @param[in] ：
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
BOOLEAN comm_listen_start(void *pArg)
{
    COMM_LISTEN_HANDLE_T *ptHandle = (COMM_LISTEN_HANDLE_T *)pArg;
    return (1 == *ptHandle->pIPCok) ? TRUE : FALSE;
}

/**********************************************************************
* @name      : comm_listen_entry
* @brief     ：以太网监听入口函数
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void *comm_listen_entry(void *pArg)
{
    uint8  stateList;                  /*以太网监听信道运行状态*/
    uint8  dataBuf[BUF_LENGTH] = {0}; /*以太网监听信道接收缓冲区*/
    int    recvLen = 0;               /*以太网监听信道接收数据长度*/
    int    sendLen = 0;
    uint32 dataLen;
    uint32 clientIP = 0;            //客户端IP
    uint16 clientPort = 0;            //客户端端口号
    int    fdClientNew = -1;        //客户端连接的句柄
    uint8  protocol = 0xff;
    uint16 heartbeat = 0;
    uint16 newbeat = 0;
    CHANNEL_INFO_T *ptChanInfo = NULL;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    uint32 applyRxFlow = 0;   /* 应用层流量 */
    uint32 applyTxFlow = 0;   /* 应用层流量 */
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    
    COMM_LISTEN_HANDLE_T *ptHandle = (COMM_LISTEN_HANDLE_T *)pArg;
    
    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }

    if(ptHandle->fdListen > 0)
    {
        close(ptHandle->fdListen);
        ptHandle->fdListen = -1;
    }
    if(ptHandle->fdClient > 0)
    {
        close(ptHandle->fdClient);
        ptHandle->fdClient = -1;
    }

    /* 清空队列 */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptSendList);

    clock_gettime(CLOCK_MONOTONIC, &oldtime);

    stateList = CHANNEL_STATE_INIT;
    while(1)
    {
        *ptHandle->pRunFlag = 0;

        if(NULL != ptHandle->pAppReStart && 1 == *ptHandle->pAppReStart)
        {
            if(ptHandle->fdClient > 0)
            {
                close(ptHandle->fdClient);
                ptHandle->fdClient = -1;
            }
            if(ptHandle->fdListen > 0)
            {
                close(ptHandle->fdListen);
                ptHandle->fdListen = -1;
            }

            /* 关闭通道 */
            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
            ptIPCMsg->id = COMM_LISTEN;
            ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
            dataLen = sizeof(COMM_IPCMSG_T);
            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
            {
                COMM_FMT_DEBUG("Msg Queue is full\n");
            }
            COMM_FMT_DEBUG("Listen close channel id %d\n", ptIPCMsg->id);
            return 0;
        }

        clock_gettime(CLOCK_MONOTONIC, &curtime);
        /*过秒流量统计*/
        if (curtime.tv_sec != oldtime.tv_sec)
        {
            if(*ptHandle->pppHeart > 0)
            {
                /* 混合模式下 写入以太网1的流量 */
                comm_flow_add(TRUE, 0, applyTxFlow, applyRxFlow);
            }
            applyRxFlow = 0;
            applyTxFlow = 0;
            oldtime = curtime;
        }

        switch(stateList)
        {
            case CHANNEL_STATE_INIT:
            {
                if(ptHandle->fdListen > 0)
                {
                    stateList = CHANNEL_STATE_RECV;
                    break;
                }

                ptHandle->fdListen = comm_listen_socket(COMM_LISTEN_PORT);
                if(ptHandle->fdListen <= 0)
                {
                    sleep(1);
                    break;
                }
                
                /* 通知主应用调度线程通道打开 */
                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = 2;
                ptChanInfo->medium = COMM_MEDIA_LISTEN;
                ptChanInfo->connType = 0;
                ptChanInfo->bFirst = TRUE;
                ptChanInfo->heart = 0;
                ptChanInfo->dailcnt = 3;
                ptChanInfo->dailtime = 60;
                ptChanInfo->ip = 0;
                ptChanInfo->port = 0;
                ptIPCMsg->id = COMM_LISTEN;
                ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                COMM_FMT_DEBUG("Listen open channel id %d\n", ptIPCMsg->id);
                memset(dataBuf, 0, BUF_LENGTH);
                dataLen = 0;
                
                stateList = CHANNEL_STATE_RECV;
                break;
            }
            case CHANNEL_STATE_RECV:
            {
                if(ptHandle->fdListen <= 0)
                {
                    stateList = CHANNEL_STATE_INIT;
                    break;
                }
                fdClientNew = comm_accept_socket(ptHandle->fdListen, &clientIP, &clientPort);
                newbeat = MAX(*ptHandle->ethHeart, *ptHandle->pppHeart);  //更新心跳周期, 取最大值
                if(fdClientNew > 0)
                {
                    /* 同一个客户端有新的连接，说明旧的连接可能已经失效了，需要使用新的连接来代替 */
                    if(fdClientNew > 0)
                    {
                        if(ptHandle->fdClient > 0)
                        {
                            close(ptHandle->fdClient);
                        }
                        ptHandle->fdClient = fdClientNew;
                        fdClientNew = -1;
                        COMM_FMT_DEBUG("Listen appect new: Ip %d.%d.%d.%d, port = %d\n", (clientIP >> 24) & 0xff, 
                                       (clientIP >> 16) & 0xff, (clientIP >> 8) & 0xff, clientIP & 0xff, clientPort);
                    }
                    
                    heartbeat = newbeat;
                    /* 通知主应用调度线程通道修改 */
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = 2;
                    ptChanInfo->medium = COMM_MEDIA_LISTEN;
                    ptChanInfo->connType = 0;
                    ptChanInfo->bFirst = TRUE;
                    ptChanInfo->heart = heartbeat;
                    ptChanInfo->dailcnt = 3;
                    ptChanInfo->dailtime = 60;
                    ptChanInfo->ip = clientIP;
                    ptChanInfo->port = clientPort;
                    ptIPCMsg->id = COMM_LISTEN;
                    ptIPCMsg->protocol = PROT_LISTEN_WORK;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("Listen start work, channel id %d\n", ptIPCMsg->id);
                    memset(dataBuf, 0, dataLen);
                    dataLen = 0;
                }
                if(newbeat != heartbeat && ptHandle->fdClient > 0)
                {
                    /* 心跳0表示退出混合模式, 心跳大于0且存在连接才修改心跳(存在连接才执行心跳检测，只检测失败一次) */
                    heartbeat = newbeat;
                    /* 通知主应用调度线程通道修改 */
                    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                    ptChanInfo->workmode = 2;
                    ptChanInfo->medium = COMM_MEDIA_LISTEN;
                    ptChanInfo->connType = 0;
                    ptChanInfo->bFirst = TRUE;
                    ptChanInfo->heart = heartbeat;
                    ptChanInfo->dailcnt = 3;
                    ptChanInfo->dailtime = 60;
                    ptChanInfo->ip = clientIP;
                    ptChanInfo->port = clientPort;
                    ptIPCMsg->id = COMM_LISTEN;
                    ptIPCMsg->protocol = PRO_PARA_MOD;
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("Listen change, channel id %d\n", ptIPCMsg->id);
                    memset(dataBuf, 0, dataLen);
                    dataLen = 0;
                }

                stateList = CHANNEL_STATE_SEND;

                if(ptHandle->fdClient <= 0)
                {
                    sleep(1);
                    break;
                }
        
                recvLen = recv(ptHandle->fdClient, dataBuf, BUF_LENGTH - 4, 0);
                if(recvLen > 0)
                {
                    applyRxFlow += recvLen;
                    comm_add_channel(COMM_LISTEN, dataBuf, recvLen);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, recvLen + 4))
                    {
                        COMM_FMT_TRACE("Listen queue is full\n");
                    }
                    COMM_BUF_DEBUG(dataBuf, recvLen + 4, "Listen recv:");
                    memset(dataBuf, 0, recvLen + 4);
                }
                else if(recvLen == 0)
                {
                    /* 通道暂停 */
                    if(ptHandle->fdClient > 0)
                    {
                        close(ptHandle->fdClient);
                        ptHandle->fdClient = -1;
                    }
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = COMM_LISTEN;
                    ptIPCMsg->protocol = PROT_LISTEN_PAUSE;
                    dataLen = sizeof(COMM_IPCMSG_T);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                    {
                        COMM_FMT_TRACE("Msg Queue is full\n");
                    }
                    COMM_FMT_DEBUG("Client close the connect, channel id %d\n", ptIPCMsg->id);
                }
                break;
            }
            case CHANNEL_STATE_SEND:
            {
                memset(dataBuf, 0, BUF_LENGTH);
                dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
                if(dataLen > 0)
                {
                    protocol = comm_get_protocol(dataBuf);
                    if(PROT_CLOSE_CHANNLE == protocol)
                    {
                        stateList = CHANNEL_STATE_CLOSE;
                        break;
                    }
                    if(PRO_BIG_MSG == protocol)
                    {
                        /* 超大消息 */
                        struct timespec newtime = {0};
                        clock_gettime(CLOCK_MONOTONIC, &newtime);
                        COMM_BIGMSG_T *ptBigMsg = (COMM_BIGMSG_T *)(dataBuf + 4);
                        if(newtime.tv_sec > ptBigMsg->packtime + 10)
                        {
                            /* 超过10s就不发送了 */
                            goto next;
                        }
                        COMM_FMT_DEBUG("Listen big msg send len %d\n", ptBigMsg->msglen);

                        if(ptHandle->fdClient > 0)
                        {
                            uint32 singlelen = 0;
                            uint32 msgLeft = ptBigMsg->msglen;
                            uint8 *pTemp = ptBigMsg->msgaddr;
                            singlelen = MIN(msgLeft, 512);
                            while(singlelen > 0)
                            {
                                COMM_BUF_DEBUG(pTemp, singlelen, "Listen big msg send");
                                sendLen = send(ptHandle->fdClient, pTemp, singlelen, 0);
                                pTemp += singlelen;
                                msgLeft -= singlelen;
                                singlelen = MIN(msgLeft, 512);
                                applyTxFlow += sendLen > 0 ? sendLen : 0;
                                usleep(10000);
                            }

                            ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                            ptIPCMsg->id = COMM_LISTEN;
                            ptIPCMsg->protocol = PRO_BIG_MSG;
                            memcpy(dataBuf + 4, ptBigMsg, sizeof(COMM_BIGMSG_T));
                            dataLen = sizeof(COMM_IPCMSG_T) + sizeof(COMM_BIGMSG_T);
                            if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                            {
                                COMM_FMT_TRACE("Msg Queue is full\n");
                            }
                        }
                        goto next;
                    }
                    if(PRO_HEARTBEAT_ERROR == protocol)
                    {
                        /* 丢失心跳 重新登录 */
                        COMM_FMT_DEBUG("Listen channel lost heartbeat, restart eth0 ppp0!\n");
                        if(*ptHandle->ethHeart > 0)
                        {
                            *ptHandle->ethReStart = 1;
                        }
                        if(*ptHandle->pppHeart > 0)
                        {
                            *ptHandle->pppReStart = 1;
                        }
                        goto next;
                    }
                    if(ptHandle->fdClient > 0)
                    {
                        sendLen = send(ptHandle->fdClient, dataBuf + 4, dataLen - 4, 0);
                        COMM_BUF_DEBUG(dataBuf, dataLen, sendLen > 0 ? "Listen send" : "Listen send failed");
                        applyTxFlow += sendLen > 0 ? sendLen : 0;
                    }
next:
                    memset(dataBuf, 0, dataLen);
                }
                stateList = CHANNEL_STATE_RECV;
                break;
            }
            case CHANNEL_STATE_CLOSE:
            {
                if(ptHandle->fdClient > 0)
                {
                    close(ptHandle->fdClient);
                    ptHandle->fdClient = -1;
                }
                if(ptHandle->fdListen > 0)
                {
                    close(ptHandle->fdListen);
                    ptHandle->fdListen = -1;
                }

                /* 关闭通道 */
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->id = COMM_LISTEN;
                ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                dataLen = sizeof(COMM_IPCMSG_T);
                if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                {
                    COMM_FMT_TRACE("Msg Queue is full\n");
                }
                COMM_FMT_DEBUG("Listen close channel id %d\n", ptIPCMsg->id);
                memset(dataBuf, 0, dataLen);
                stateList = CHANNEL_STATE_INIT;
                sleep(5);
                break;
            }
            default:
            {
                stateList = CHANNEL_STATE_INIT;
                break;
            }
        }
        usleep(10000);
    }
    pthread_exit(NULL);
}

