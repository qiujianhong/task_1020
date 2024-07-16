/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块IPC通信
* @date：    2019-10-31
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"

#include "comm.h"
#include "ipc_comm.h"
extern uint8 gdelay;
extern uint8 ssal_get_status();

/**********************************************************************
* @name      : comm_channel_event_add
* @brief     ：主站连接确认处理
* @param[in] ：uint8 channeltype   通道类型
               ChannelEvent func   处理函数
               void *pArg          入参
               void *ptHandle      句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-1
* @Update    :
**********************************************************************/
int comm_channel_event_add(uint8 channeltype, ChannelEvent func, void *pArg, void *ptHandle)
{
    COMM_IPC_HANDLE_T *pIPCHandle = (COMM_IPC_HANDLE_T *)ptHandle;

    if(NULL == func || NULL == pArg || NULL == ptHandle)
    {
        return -1;
    }
    
    if(pIPCHandle->eventnum >= COMM_IPC_EVENT_NAX)
    {
        return -2;
    }
    pIPCHandle->event[pIPCHandle->eventnum].type = channeltype;
    pIPCHandle->event[pIPCHandle->eventnum].func = func;
    pIPCHandle->event[pIPCHandle->eventnum].pArg = pArg;
    pIPCHandle->eventnum += 1;
    
    return 0;
}

/**********************************************************************
* @name      : ipc_write_queue
* @brief     ：收到的消息写入其他线程的发送队列
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void ipc_write_queue(uint8 *pBuf, uint32 bufLen, COMM_IPC_HANDLE_T *ptHandle)
{
    uint8 bigChannel = CHL_RESERVE; /*通讯通道大类标志*/
    COMM_QUEUE_LIST_T *pQueue = NULL;
    
    bigChannel = comm_get_bigchannel(pBuf);
    if(CHL_GPRS_1 == bigChannel)
    {
        pQueue = ptHandle->ptGPRS0Send;
    }
    else if(CHL_GPRS_2 == bigChannel)
    {
        pQueue = ptHandle->ptGPRS1Send;
    }
    else if(LOCATE_BLUE == bigChannel)
    {
        pQueue = ptHandle->ptBlueSend;
    }
    else if(CHL_RADIO == bigChannel)
    {
        pQueue = ptHandle->ptRadioSend;
    }
    else if(LOCATE_232 == bigChannel)
    {
        pQueue = ptHandle->pt232Send;
    }
    else if(COMM_LISTEN == bigChannel)
    {
        pQueue = ptHandle->ptListenSend;
    }
    else if(CHL_ETH_1 == bigChannel)
    {
        pQueue = ptHandle->ptETH0Send;
    }
    else if(CHL_ETH_2 == bigChannel)
    {
        pQueue = ptHandle->ptETH1Send;
    }
    else if(JOIN485_1 == bigChannel || CHL_ACMETER == bigChannel)
    {
        pQueue = ptHandle->pt485Send;
    }
    else if(CHL_GSM_1 == bigChannel)
    {
        pQueue = ptHandle->ptSMS0Send;
    }
    else if(CHL_GSM_2 == bigChannel)
    {
        pQueue = ptHandle->ptSMS1Send;
    }
    else if(INFRARED == bigChannel)
    {
        pQueue = ptHandle->ptIRSend;
    }
    else if(CHL_APP == bigChannel)
    {
        pQueue = ptHandle->ptAppTransSend;
    }
    else
    {
        return;
    }

    if(0 != comm_write_queue(pQueue, pBuf, bufLen))
    {
        COMM_FMT_TRACE("comm_write_queue failed channel %d\n", pBuf[0]);
    }
    COMM_FMT_TRACE("Bigchannel %d send\n", bigChannel);
}

/**********************************************************************
* @name      : ipc_write_queue
* @brief     ：收到的消息写入其他线程的发送队列
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void ipc_send_msg(int sockfd,COMM_IPC_HANDLE_T *ptHandle)
{
    uint32            dataLen = 0;
    uint8 dataBuf[BUF_LENGTH + 20];
    int   ret = 0;

    //bzero(dataBuf, BUF_LENGTH);
    if(ptHandle->ptGPRS0Recv)
    {
        dataLen = comm_read_queue(ptHandle->ptGPRS0Recv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("PPP 0 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }
    if(ptHandle->ptGPRS0RecvBak)
    {
        dataLen = comm_read_queue(ptHandle->ptGPRS0RecvBak, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("PPP 0 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }
    
    if(ptHandle->ptGPRS1Recv)
    {
        dataLen = comm_read_queue(ptHandle->ptGPRS1Recv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("PPP 1 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }
    if(ptHandle->ptGPRS1RecvBak)
    {
        dataLen = comm_read_queue(ptHandle->ptGPRS1RecvBak, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("PPP 1 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->ptETH0Recv)
    {
        dataLen = comm_read_queue(ptHandle->ptETH0Recv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("ETH 0 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }
    if(ptHandle->ptETH0RecvBak)
    {
        dataLen = comm_read_queue(ptHandle->ptETH0RecvBak, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("ETH 0 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->ptETH1Recv)
    {
        dataLen = comm_read_queue(ptHandle->ptETH1Recv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("ETH 1 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }
    if(ptHandle->ptETH1RecvBak)
    {
        dataLen = comm_read_queue(ptHandle->ptETH1RecvBak, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("ETH 1 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->ptListenRecv)
    {
        dataLen = comm_read_queue(ptHandle->ptListenRecv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Listen faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->ptBlueRecv)
    {
        dataLen = comm_read_queue(ptHandle->ptBlueRecv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Blue faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->ptRadioRecv)
    {
        dataLen = comm_read_queue(ptHandle->ptRadioRecv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Radio faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->pt485Recv)
    {
        dataLen = comm_read_queue(ptHandle->pt485Recv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("Rs485 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->ptSMS0Recv)
    {
        dataLen = comm_read_queue(ptHandle->ptSMS0Recv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("SMS0 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->ptSMS1Recv)
    {
        dataLen = comm_read_queue(ptHandle->ptSMS1Recv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("SMS1 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->pt232Recv)
    {
        dataLen = comm_read_queue(ptHandle->pt232Recv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("232 faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    if(ptHandle->ptIRRecv)
    {
        dataLen = comm_read_queue(ptHandle->ptIRRecv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("IR faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }
    
    if(ptHandle->ptAppTransRecv)
    {
        dataLen = comm_read_queue(ptHandle->ptAppTransRecv, dataBuf);
        if(dataLen > 0)
        {
            ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
            if(0 != ret)
            {
                COMM_FMT_DEBUG("AppTrans faild send to appmain, ret %d, try again\n", ret);
                goto tryagain;
            }
        }
    }

    return;
tryagain:
    usleep(10000);
    ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, APPMAIN_IPC);
    if(0 != ret)
    {
        COMM_FMT_DEBUG("Send to appmain again faild, ret %d\n", ret);
    }
}

/**********************************************************************
* @name      : ipc_sock_entry
* @brief     ：IPC线程入口
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
void *ipc_sock_entry(void *pArg)
{
    uint8 stateSock = INIT_SOCK;   /*环回socket运行状态*/
    uint32   sender = 0;          /*消息发送端*/
    int     dataLen = 0;
    uint8 dataBuf[TASK_MSG_MAX_LEN] = {0};
    COMM_IPC_HANDLE_T *ptHandle = (COMM_IPC_HANDLE_T *)pArg;
    TASK_MSG_t *pmsg = (TASK_MSG_t *)dataBuf;
    uint8   protocol = 0;
    uint8   msgcheck = TRUE;

    nice(-5); /*提高优先级*/

    while(1)
    {
        *ptHandle->pRunFlag = 0;
        switch(stateSock)
        {
            case INIT_SOCK:
            {
                if(ptHandle->sockfd <= 0)
                {
                    ptHandle->sockfd = task_msg_init(COMM_IPC);
                    if (ptHandle->sockfd <= 0)  /*打开失败，延时1S重新尝试打开进程间通讯*/
                    {
                        COMM_FMT_TRACE("comm task_msg_init error!\n");
                        sleep(1);
                        break;
                    }
                    task_msg_settime(ptHandle->sockfd, 0, 2000);
                }
                stateSock = RECV_SOCK;
                break;
            }
            case RECV_SOCK:
            {
                dataLen = task_recv_msg(ptHandle->sockfd, dataBuf, TASK_MSG_MAX_LEN, &sender);
                if(dataLen > 0 && APPMAIN_IPC == sender && MSG_COMM_APPAMIN == pmsg->msg_id)
                {
                    protocol = comm_get_protocol(pmsg->msg);
                    if(PROT_APPMAIN_START == protocol)
                    {
                        *ptHandle->pIPCok = 1;
                        *ptHandle->pGprs0ReStart = 1;
                        *ptHandle->pGprs1ReStart = 1;
                        *ptHandle->pEth0ReStart = 1;
                        *ptHandle->pEth1ReStart = 1;
                        *ptHandle->pBlueReStart = 1;
                        *ptHandle->pRadioReStart = 1;
                        *ptHandle->pEthListenReStart = 1;
                        *ptHandle->p485ReStart = 1;
                        *ptHandle->pSMS0ReStart = 1;
                        *ptHandle->pSMS1ReStart = 1;

                        COMM_FMT_DEBUG("Recv appmain start\n");
                        
                        /* 返回确认 */
                        dataBuf[1] = PROT_COMM_START;
                        /* 返回 */
                        if(0 != task_send_msg(ptHandle->sockfd, MSG_COMM_APPAMIN, dataBuf, 2, APPMAIN_IPC))
                        {
                            COMM_FMT_TRACE("Send MSG_COMM_APPAMIN start failed\n");
                        }
                    }
                    else if(PROT_OPEN_ACK == protocol || PROT_CLOSE_ACK == protocol)
                    {
                        uint8 i;
                        uint8 type;
                        type = comm_get_bigchannel(pmsg->msg);

                        for(i = 0; i < ptHandle->eventnum; i++)
                        {
                            if(type == ptHandle->event[i].type && NULL != ptHandle->event[i].func)
                            {
                                ptHandle->event[i].func(*pmsg->msg, protocol, ptHandle->event[i].pArg);
                            }
                        }
                    }
                    else
                    {
                        /* 消息有效性检查 */
                        if(0 == ssal_get_status())
                        {
                            if(LOCATE_232 != pmsg->msg[0] && (PRO_DATA_TRANSMIT == protocol || PRO_LOGIN_FRAME == protocol
                                || PRO_EXIT_LOGIN_FRAME == protocol || PRO_HEART_FRAME == protocol))
                            {
                                msgcheck = FALSE;
                                if(pmsg->msglen > 10 && 0x68 == pmsg->msg[4] && 0x16 == pmsg->msg[pmsg->msglen-1])
                                {
                                    msgcheck = TRUE;
                                }
                            }
                        }
                        if(TRUE == msgcheck)
                        {
                            ipc_write_queue(pmsg->msg, (uint32)pmsg->msglen, ptHandle);
                        }
                        else
                        {
                            COMM_BUF_DEBUG(pmsg->msg, pmsg->msglen, "Error msg, channel %d\n", pmsg->msg[0]);
                        }
                        msgcheck = TRUE;
                    }
                    memset(dataBuf, 0, dataLen);
                }
                stateSock = SEND_SOCK;
                break;
            }
            case SEND_SOCK:
            {
                ipc_send_msg(ptHandle->sockfd, ptHandle);
                stateSock = RECV_SOCK;
                break;
            }
            default:
            {
                stateSock = INIT_SOCK;
                break;
            }
        }
        //usleep(1500);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    pthread_exit(NULL);
}

