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

#include "common_base.h"
#include "comm_ipc.h"

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
    else if(CHL_ETH_1 == bigChannel)
    {
        pQueue = ptHandle->ptETH0Send;
    }
    else if(CHL_ETH_2 == bigChannel)
    {
        pQueue = ptHandle->ptETH1Send;
    }
    else if(CHL_LISTEN == bigChannel)
    {
        pQueue = ptHandle->ptListenSend;
    }
    else
    {
        return;
    }

    if(0 != comm_write_queue(pQueue, pBuf, bufLen))
    {
        COMM_FMT_TRACE("comm_write_queue failed channel %d\n", pBuf[0]);
    }
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
    uint8 dataBuf[BUF_LENGTH] = {0};
    int   ret = 0;

    dataLen = comm_read_queue(ptHandle->ptGPRS0Recv, dataBuf);
    if(dataLen > 0)
    {
        ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, PD_APP104_IPC);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("PPP 0 faild send to app104, ret %d\n", ret);
        }
        memset(dataBuf, 0, BUF_LENGTH);
    }
    
    dataLen = comm_read_queue(ptHandle->ptGPRS1Recv, dataBuf);
    if(dataLen > 0)
    {
        ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, PD_APP104_IPC);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("PPP 1 faild send to app104, ret %d\n", ret);
        }
        memset(dataBuf, 0, BUF_LENGTH);
    }

    dataLen = comm_read_queue(ptHandle->ptETH0Recv, dataBuf);
    if(dataLen > 0)
    {
        ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, PD_APP104_IPC);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("ETH0 0 faild send to app104, ret %d\n", ret);
        }
        memset(dataBuf, 0, BUF_LENGTH);
    }

    dataLen = comm_read_queue(ptHandle->ptETH1Recv, dataBuf);
    if(dataLen > 0)
    {
        ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, PD_APP104_IPC);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("ETH 1 faild send to app104, ret %d\n", ret);
        }
        memset(dataBuf, 0, BUF_LENGTH);
    }

    dataLen = comm_read_queue(ptHandle->ptListenRecv, dataBuf);
    if(dataLen > 0)
    {
        ret = task_send_msg(sockfd, MSG_COMM_APPAMIN, dataBuf, (uint16)dataLen, PD_APP104_IPC);
        if(0 != ret)
        {
            COMM_FMT_DEBUG("Listen faild send to app104, ret %d\n", ret);
        }
        memset(dataBuf, 0, BUF_LENGTH);
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
                    ptHandle->sockfd = task_msg_init(PD_COMM_IPC);
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
                if(dataLen > 0 && PD_APP104_IPC == sender && MSG_COMM_APPAMIN == pmsg->msg_id)
                {
                    if(PROT_APP104_START == comm_get_protocol(pmsg->msg))
                    {
                        *ptHandle->pIPCok = 1;
                        *ptHandle->pGprs0ReStart = 1;
                        *ptHandle->pGprs1ReStart = 1;
                        *ptHandle->pEth0ReStart = 1;
                        *ptHandle->pEth1ReStart = 1;
                        *ptHandle->pListenReStart = 1;
                        
                        COMM_FMT_TRACE("Recv appmain start\n");
                        
                        /* 返回确认 */
                        dataBuf[1] = PROT_COMM_START;
                        /* 返回 */
                        if(0 != task_send_msg(ptHandle->sockfd, MSG_COMM_APPAMIN, dataBuf, 2, PD_APP104_IPC))
                        {
                            COMM_FMT_TRACE("Send MSG_COMM_APPAMIN start failed\n");
                        }
                    }
                    else
                    {
                        ipc_write_queue(pmsg->msg, (uint32)pmsg->msglen, ptHandle);
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
    }
    pthread_exit(NULL);
}

