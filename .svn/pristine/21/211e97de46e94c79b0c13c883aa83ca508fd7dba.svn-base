/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程IPC入口和出口
* @date：    2019-12-3
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appcomm.h"

#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"

#include "appipc.h"
#include "framecomm.h"

int  g_AppSockFd = 0;
uint8 uIpcBuf[APP_RECV_IPC_MAXLEN] = {0};
uint8 uSendBuf[LOCAL_SEND_FRAM_MAXLEN+4] = {0};
extern uint8 gdelay;

/**********************************************************************
* @name      : app_ipc_recv
* @brief     ：IPC接收消息
* @param[in] ：
* @param[out]：
* @return    ：0-成功/-1失败
* @Create    : 王津剑
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
int app_ipc_init()
{
    uint32 sender = 0;
    int ret;
    TASK_MSG_t *pmsg = (TASK_MSG_t *)uIpcBuf;
    COMM_IPCMSG_T *pMsg = NULL;

    g_AppSockFd = task_msg_init(APPMAIN_IPC);
    if(g_AppSockFd <= 0)  /*打开失败，延时1S重新尝试打开进程间通讯*/
    {
        APP698_FMT_TRACE("appmain task_msg_init error!\n");
        sleep(1);
        return -1;
    }
    task_msg_settime(g_AppSockFd, 0, 1000);

    while(1)
    {
        uIpcBuf[1] = PROT_APPMAIN_START;
        if(0 != task_send_msg(g_AppSockFd, MSG_COMM_APPAMIN, uIpcBuf, 2, COMM_IPC))
        {
            APP698_FMT_TRACE("Appmain Send start failed\n");
            sleep(5);
            continue;
        }
        sleep(1);
        uIpcBuf[1] = 0;
        ret = task_recv_msg(g_AppSockFd, uIpcBuf, APP_RECV_IPC_MAXLEN, &sender);
        if(ret > 0 && COMM_IPC == sender && MSG_COMM_APPAMIN == pmsg->msg_id)
        {
            pMsg = (COMM_IPCMSG_T *)pmsg->msg;
            if(PROT_COMM_START == pMsg->protocol)
            {
                APP698_FMT_DEBUG("Appmain connect with commucation\n");
                return 0;
            }
        }
        sleep(1);
    }
    
    return -1;
}

/**********************************************************************
* @name      : send_emergency_ipcmsg
* @brief     ：发送紧急消息 不走通道
* @param[in] ：uint8  id        通道id
               uint8  protocol  私有协议
               uint8 *pMsg      消息内容
               uint16 msgLen    消息长度 不能超过4096
* @param[out]：0-成功/错误码
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-4
* @Update    :
**********************************************************************/
int send_emergency_ipcmsg(uint8 id, uint8 protocol, uint8 *pMsg, uint16 msgLen)
{
    CHANNEL_CACHE_T *ptChannel = NULL;
    uint8 uEmergMsg[LOCAL_RECV_FRAM_MAXLEN + 4] = {0};
    int errcode = APP698_SECCESS;
    
    ptChannel = app_get_channel(id);
    if(NULL == ptChannel)
    {
        return APP698_ERR_ID_NOTEXIST;
    }
    if(FALSE == ptChannel->bOpen)
    {
        return APP698_ERR_NOT_OPEN;
    }

    if(msgLen > LOCAL_RECV_FRAM_MAXLEN)
    {
        return APP698_ERR_MSG_TOO_LONG;
    }

    uEmergMsg[0] = id;
    uEmergMsg[1] = protocol;
    uEmergMsg[2] = msgLen & 0x00ff;
    uEmergMsg[3] = (msgLen & 0xff00) >> 8;
    if(msgLen > 0)
    {
        memcpy(uEmergMsg + 4, pMsg, msgLen);
    }
    if(0 != task_send_msg(g_AppSockFd, MSG_COMM_APPAMIN, uEmergMsg, msgLen + 4, COMM_IPC))
    {
        usleep(50000);
        if(0 != task_send_msg(g_AppSockFd, MSG_COMM_APPAMIN, uEmergMsg, msgLen + 4, COMM_IPC))
        {
            errcode = APP698_ERR_IPC_RECONNECT;
        }
    }

    return errcode;
}

/**********************************************************************
* @name      : send_bigpackage_master
* @brief     ：发送超大消息, 使用同步消息发送
* @param[in] ：uint8  id        通道id
               uint8 *pMsg      消息内容
               uint32 msgLen    消息长度
* @param[out]：0-成功/错误码
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-22
* @Update    :
**********************************************************************/
int send_bigpackage_master(uint8 id, uint8 *pMsg, uint32 msgLen)
{
    CHANNEL_CACHE_T *ptChannel = NULL;
    uint8 sendMsg[100] = {0};
    COMM_IPCMSG_T *ptIpcmsg = (COMM_IPCMSG_T *)sendMsg;
    COMM_BIGMSG_T *pBigmsg = (COMM_BIGMSG_T *)ptIpcmsg->msg;
    uint16 sendlen = 0;
    int errcode = APP698_SECCESS;
    struct timespec curtime = {0};
    struct timespec oldtime = {0};


    ptChannel = app_get_channel(id);
    if(NULL == ptChannel)
    {
        return APP698_ERR_ID_NOTEXIST;
    }
    if(FALSE == ptChannel->bOpen)
    {
        return APP698_ERR_NOT_OPEN;
    }
    if(NULL == pMsg || 0 == msgLen)
    {
        return APP698_ERR_INPUT;
    }

    ptChannel->extOutMsglen = msgLen;
    clock_gettime(CLOCK_MONOTONIC, &oldtime);

    sendlen = sizeof(COMM_IPCMSG_T) + sizeof(COMM_BIGMSG_T);
    ptIpcmsg->id = id;
    ptIpcmsg->protocol= PRO_BIG_MSG;
    pBigmsg->msglen = msgLen;
    pBigmsg->msgaddr = pMsg;
    pBigmsg->packtime = oldtime.tv_sec;

    ptChannel->extIndex += 1;
    pBigmsg->msgindex = ptChannel->extIndex;
    ptChannel->extSendFlag = FALSE;

    errcode = task_send_msg(g_AppSockFd, MSG_COMM_APPAMIN, sendMsg, sendlen, COMM_IPC);
    if(0 != errcode)
    {
        return errcode;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);

    /* 20s超时 */
    while(curtime.tv_sec < oldtime.tv_sec + 20)
    {
        if(TRUE == ptChannel->extSendFlag)
        {
            pBigmsg->msglen = 0;
            return 0;
        }
        usleep(100000);
        clock_gettime(CLOCK_MONOTONIC, &curtime);
    }
    pBigmsg->msglen = 0;
    return APP698_ERR_SEND_ERRORMSG;
}

/**********************************************************************
* @name      : app_ipc_recv
* @brief     ：IPC接收消息
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void *app_ipc_recv(void *pArg)
{
    IPC_THREAD_T  *pHandle = (IPC_THREAD_T *)pArg;
    int            ret = 0;
    int            rcvLen = 0;
    COMM_IPCMSG_T *pMsg = NULL;
    uint8          id = 0;
    int            errCode = 0;
    uint16         msgLen = 0;
    uint32         sender = 0;
    TASK_MSG_t    *pmsg = (TASK_MSG_t *)uIpcBuf;
    uint8          closeFlag[MAX_CHANNEL_NUM] = {0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    CHANNEL_CACHE_T *ptChannel = NULL;
    int             msgret = 0;

    /* 线程重启后 智能解锁 防止死锁 */
    for(id = 0; id < MAX_CHANNEL_NUM; id++)
    {
        app_unlock_channel(id, APP698_RIPC_LOCK);
    }

    /* 修改成阻塞式(阻塞时间长) */
    task_msg_settime(g_AppSockFd, 0, 100000);

    clock_gettime(CLOCK_MONOTONIC, &oldtime);

    while(1)
    {
        *pHandle->pRunFlag = 0;

        ret = task_recv_msg(g_AppSockFd, uIpcBuf, APP_RECV_IPC_MAXLEN, &sender);
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(curtime.tv_sec != oldtime.tv_sec)
        {
            /* 过s处理延时关闭通道的消息 */
            oldtime = curtime;

            for(id = 0; id < MAX_CHANNEL_NUM; id++)
            {
                if(FALSE == closeFlag[id])
                {
                    continue;
                }
                if(0 == app_set_channelworklock(id, TRUE))
                {
                    APP698_FMT_DEBUG("%u channel close\n", id);
                    send_emergency_ipcmsg(id, PROT_CLOSE_ACK, &id, sizeof(uint8));
                    app_set_channelworklock(id, FALSE);
                    app_close_channel(id);
                    closeFlag[id] = FALSE;
                }
                else
                {
                    APP698_FMT_DEBUG("%u channel is work, wait to close\n", id);
                }
            }
        }
        
        if(ret < sizeof(COMM_IPCMSG_T) || MSG_COMM_APPAMIN != pmsg->msg_id)
        {
            continue;
        }

        pMsg = (COMM_IPCMSG_T *)pmsg->msg;
        rcvLen = pmsg->msglen;
        id = pMsg->id;
        switch(pMsg->protocol)
        {
            case PRO_DATA_TRANSMIT:  //正常数据传输
            {
                if(CHANNEL_DEFAULT == id)
                {
                    break;
                }
                
                ptChannel = app_get_channel(id);

                msgLen = rcvLen - sizeof(COMM_IPCMSG_T);
                
                /* 超大消息提前处理 */
                msgret = app_externmsg_reproc(id, pMsg->msg, msgLen, APP698_RIPC_LOCK);
                if(APP698_SECCESS == msgret)
                {
                    APP698_FMT_DEBUG("%u insert externmsg len %d, recv %d\n", id, msgLen, ptChannel->recvExtLen);
                    break;
                }
                else if(APP698_ERR_EXT_MSG_OVER == msgret)
                {
                    APP698_FMT_DEBUG("%u insert last externmsg len %d, recv %d\n", id, msgLen, ptChannel->recvExtLen);

                    errCode = protocol_698_addpool(id);
                    if(0 != errCode)
                    {
                        APP698_BUF_DEBUG(pMsg->msg, msgLen, "%u insert pool failed:", id);
                    }
                    break;
                }
                else if(APP698_ERR_EXT_MSG_FIRST == msgret)
                {
                    APP698_BUF_DEBUG(pMsg->msg, MIN(100, msgLen), "%u first externmsg msglen %d, recv %d\n", id, 
                                     ptChannel->extMsgLen, ptChannel->recvExtLen);
                    break;
                }

                /* 消息插入通道 在通道中检查 */
                errCode = app_insert_recvmsg(id, pMsg->msg, msgLen, APP698_RIPC_LOCK);
                if(0 != errCode)
                {
                    APP698_BUF_DEBUG(pMsg->msg, msgLen, "%u insert channel failed, ret %d:", id, errCode);
                }
                else
                {
                    errCode = protocol_698_addpool(id);
                    if(0 != errCode)
                    {
                        APP698_BUF_DEBUG(pMsg->msg, msgLen, "%u insert pool failed:", id);
                    }
                }
                break;
            }
            case PROT_OPEN_CHANNLE:  //通道开启
            {
                CHANNEL_INFO_T *ptInfo = (CHANNEL_INFO_T *)pMsg->msg;
                APP698_FMT_DEBUG("%u channel open: mode %d, heartbeat %d\n", id, ptInfo->workmode, ptInfo->heart);
                closeFlag[id] = FALSE;
                app_open_channel(id, ptInfo);
                break;
            }
            case PRO_PARA_MOD:  //属性修改
            {
                CHANNEL_INFO_T *ptInfo = (CHANNEL_INFO_T *)pMsg->msg;
                APP698_FMT_DEBUG("%u channel change: mode %d, heartbeat %d\n", id, ptInfo->workmode, ptInfo->heart);
                app_set_channel(id, ptInfo);
                break;
            }
            case PROT_CLOSE_CHANNLE: //通道关闭
            {
                ptChannel = app_get_channel(id);
                
                if(NULL == ptChannel || FALSE == ptChannel->bOpen)
                {
                    APP698_FMT_DEBUG("%u channel close\n", id);
                    send_emergency_ipcmsg(id, PROT_CLOSE_ACK, &id, sizeof(uint8));
                    app_close_channel(id);
                    closeFlag[id] = FALSE;
                    break;
                }

                if(0 == app_set_channelworklock(id, TRUE))
                {
                    APP698_FMT_DEBUG("%u channel close\n", id);
                    send_emergency_ipcmsg(id, PROT_CLOSE_ACK, &id, sizeof(uint8));
                    app_set_channelworklock(id, FALSE);
                    app_close_channel(id);
                    closeFlag[id] = FALSE;
                }
                else
                {
                    ptChannel->bNeedClose = TRUE;
                    if(id < MAX_CHANNEL_NUM)
                    {
                        closeFlag[id] = TRUE;
                    }
                    /* 正在处理规约, 等待一段时间处理 */
                    APP698_FMT_DEBUG("%u channel is work, wait to close\n", id);
                }
                break;
            }
            case PRO_BIG_MSG:  //扩展长度消息返回发送结果
            {
                COMM_BIGMSG_T *ptBigMsg = (COMM_BIGMSG_T *)pMsg->msg;

                ptChannel = app_get_channel(id);
                if(NULL != ptChannel && ptChannel->extIndex == ptBigMsg->msgindex)
                {
                    ptChannel->extSendFlag = TRUE;
                }
                break;
            }
            case PROT_PAUSE_CHANNLE:  //暂停通道
            {
                APP698_FMT_DEBUG("channel id %d pause work\n", id);
                ptChannel = app_get_channel(id);
                if(NULL != ptChannel)
                {
                    ptChannel->bConnect = FALSE;
                }
                break;
            }
            case PROT_CONTINUE_CHANNLE:  //通道继续工作
            {
                APP698_FMT_DEBUG("channel id %d continue work\n", id);
                ptChannel = app_get_channel(id);
                if(NULL != ptChannel && TRUE == ptChannel->bOpen)
                {
                    ptChannel->bConnect = TRUE;
                }
                break;
            }
            case PROT_LISTEN_WORK: //通道监听开始
            {
                CHANNEL_INFO_T *ptInfo = (CHANNEL_INFO_T *)pMsg->msg;
                if(COMM_MEDIA_LISTEN == ptInfo->medium)
                {
                    ptChannel = app_get_channel(id);
                    APP698_FMT_DEBUG("%u channel work: mode %d, heartbeat %d, master %d.%d.%d.%d\n", 
                                     id, ptInfo->workmode, ptInfo->heart, (ptInfo->ip >> 24) & 0xff, 
                                     (ptInfo->ip >> 16) & 0xff, (ptInfo->ip >> 8) & 0xff, ptInfo->ip & 0xff);
                    app_set_channel(id, ptInfo);
                    ptChannel->linkState = LINK_HEART;
                    ptChannel->lostHBCount = 0;
                    ptChannel->lostHBtime = 0;
                    /* 设置最小的应用连接权限 */
                    app_connect_init(id);
                }
                break;
            }
            case PROT_LISTEN_PAUSE: //通道监听停止
            {
                APP698_FMT_DEBUG("%u channel stop work\n", id);
                ptChannel = app_get_channel(id);
                ptChannel->linkState = LINK_QUIT;
            }
            default: break;
        }
        memset(uIpcBuf, 0, ret);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}

/**********************************************************************
* @name      : app_ipc_recv
* @brief     ：IPC接收消息
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-3
* @Update    :
**********************************************************************/
void *app_ipc_send(void *pArg)
{
    uint8  id = 0;
    uint16 msglen = 0;
    uint16 maxlen = 0;
    uint16 maxapdulen = 0;  //帧头部大小
    uint16 startAdpu = 0;   //发送APDU起始
    uint8 *temp = NULL;
    IPC_THREAD_T    *pHandle = (IPC_THREAD_T *)pArg;
    CHANNEL_CACHE_T *ptChannel = NULL;
    FRAME_HEAD_T     tFrame;
    uint8  maxnum = 0;
    uint8  i;

    /* 线程重启后 智能解锁 防止死锁 */
    for(id = 0; id < MAX_CHANNEL_NUM; id++)
    {
        app_unlock_channel(id, APP698_SIPC_LOCK);
    }

    while(1)
    {
        *pHandle->pRunFlag = 0;

        maxnum = app_get_opencha_num();

        for(i = 0; i < maxnum; i++)
        {
            id = app_get_opencha_id(i);
            ptChannel = app_get_channel(id);
            if(CHANNEL_CHECK_NOTOK(ptChannel) || CHANNEL_NOT_NEED_SEND(ptChannel))
            {
                continue;
            }
            CAHNNEL_LOCK_SEND(ptChannel, APP698_SIPC_LOCK);
            /* 判断链路分帧 */
            maxlen = channel_get_break_frame_size(ptChannel);
            if(ptChannel->sendMessLen > maxlen)
            {
                if(0 == ptChannel->permSend)
                {
                    CAHNNEL_UNLOCK_SEND(ptChannel); 
                    continue;
                }
                /* 链路分帧操作 */
                memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
                app_get_frame(ptChannel->sendMess, ptChannel->sendMessLen, &tFrame, FALSE);
                
                memcpy(uSendBuf + 4, ptChannel->sendMess, 1 + tFrame.headlen);

                maxapdulen = maxlen - 1 - tFrame.headlen - 3 - 2;  //还需要减去2 作为apdu头
                startAdpu = maxapdulen * ptChannel->curblock;
                if(startAdpu > tFrame.apduLen)
                {
                    CHANNEL_EMPTY_SEND_MESS_NOLOCK(ptChannel);
                    CAHNNEL_UNLOCK_SEND(ptChannel); 
                    goto finish;
                }
                tFrame.separate = 1;
                tFrame.tSepara.block = ptChannel->curblock % (LINK_SEPARATE_MAX_BLOCK + 1); 
                tFrame.pApdu += startAdpu;
                tFrame.apduLen = MIN(maxapdulen, tFrame.apduLen - startAdpu);
                if(0 == ptChannel->curblock)
                {
                    tFrame.tSepara.type = LINK_SEPARATE_START;
                }
                else if(tFrame.apduLen < maxapdulen)
                {
                    tFrame.tSepara.type = LINK_SEPARATE_END;
                }
                else
                {
                    tFrame.tSepara.type = LINK_SEPARATE_MIDDLE;
                }
                msglen = app_frame_to_package(&tFrame, uSendBuf + 4, LOCAL_SEND_FRAM_MAXLEN, FALSE);
                if(tFrame.sc)
                {
                    /* 分帧头扰码处理 */
                    temp = uSendBuf + 4 + 1 + tFrame.headlen;
                    *temp += 0x33;
                    *(temp+1) += 0x33;
                }
                if(LINK_SEPARATE_END == tFrame.tSepara.type)
                {
                    memset(ptChannel->sendMess, 0, APP_SEND_MSG_MAXLEN);
                    ptChannel->sendMessLen = 0;
                    ptChannel->curblock = 0;
                }
            }
            else
            {
                memcpy(uSendBuf+4, ptChannel->sendMess, ptChannel->sendMessLen);
                memset(ptChannel->sendMess, 0, APP_SEND_MSG_MAXLEN);
                msglen = ptChannel->sendMessLen;
                ptChannel->sendMessLen = 0;
            }
            ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel); //消息收发表明在通信 刷新心跳计时

            /* 添加私有头信息 */
            uSendBuf[0] = id;
            uSendBuf[1] = ptChannel->protocol;
            uSendBuf[2] = msglen & 0x00ff;
            uSendBuf[3] = (msglen & 0xff00) >> 8;
            msglen += 4;
            ptChannel->permSend = 0;
            CAHNNEL_UNLOCK_SEND(ptChannel); 

            /* 退出登录 处理 */
            if(PRO_EXIT_LOGIN_FRAME == ptChannel->protocol)
            {
                ptChannel->linkState = LINK_QUIT;
            }
            /* 发送 */
            if(0 != task_send_msg(g_AppSockFd, MSG_COMM_APPAMIN, uSendBuf, msglen, COMM_IPC))
            {
                APP698_BUF_DEBUG(uSendBuf, msglen, "App send msg error:");
            }
finish:
            memset(uSendBuf, 0, msglen);
            msglen = 0;
            usleep(200);
        }
        usleep(10000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
}


