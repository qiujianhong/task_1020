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
#include "msg_id.h"
#include "task_id.h"

#include "comm.h" 
#include "app_comm.h"

extern uint8 gdelay;

/**********************************************************************
* @name      : comm_eth_eventmsg
* @brief     ：以太网口连接通知消息解析
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2022-5-7
* @Update    : 
**********************************************************************/
int comm_app_trans_msg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_APP_TRANS_HANDLE_T *ptHandle = (COMM_APP_TRANS_HANDLE_T *)pArg;
    MESSAGE_INFO_T *pMsgIn = (MESSAGE_INFO_T *)playload;
    uint8   dataBuf[4096 + 4] = {0}; 
    uint32  bufLen = 0;
    uint8   bytesnum = 0;
    uint16  len = 0;

    if(FALSE == ptHandle->bOpen)
    {
        return 1;
    }
    
    memcpy(&(ptHandle->tMsgInfo), pMsgIn, sizeof(MESSAGE_INFO_T));
    
    GET_LEN_OFFSET(len, pMsgIn->playload, bytesnum);
    if(len + bytesnum != pMsgIn->msglen)
    {
        COMM_BUF_DEBUG(pMsgIn->playload, pMsgIn->msglen, "APP透传(%s)内容错误", pMsgIn->souraddr);
        return 1;
    }
    memcpy(dataBuf, pMsgIn->playload + bytesnum, MIN(4096, len));
    comm_add_channel(CHL_APP, dataBuf, len);
    bufLen = len + 4;

    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
    {
        COMM_FMT_DEBUG("Msg Queue is full\n");
    }
    COMM_BUF_DEBUG(dataBuf + 4, bufLen - 4, "APP透传接收(%s)", pMsgIn->souraddr);
    return 0;
}

/**********************************************************************
* @name      : comm_app_trans_start
* @brief     ：APP透传 启动判断
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-5-7
* @Update    :
**********************************************************************/
BOOLEAN comm_app_trans_start(void *pArg)
{
    COMM_APP_TRANS_HANDLE_T *ptHandle = (COMM_APP_TRANS_HANDLE_T *)pArg;

    if(0 == *ptHandle->pIPCok)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************
* @name      : comm_app_trans_entry
* @brief     ：APP透传 线程入口
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-5-7
* @Update    :
**********************************************************************/
void *comm_app_trans_entry(void *pArg)
{
    uint8   dataBuf[4096 + 4] = {0}; 
    uint32  dataLen = 0;
    uint8   tempBuf[4096 + 512] = {0};
    MESSAGE_INFO_T *pMsgOut = (MESSAGE_INFO_T *)tempBuf;
    CHANNEL_INFO_T *ptChanInfo = NULL;
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    COMM_APP_TRANS_HANDLE_T *ptHandle = (COMM_APP_TRANS_HANDLE_T *)pArg;
    int     ret = 0;
    uint8   byte[5] = {0};
    uint8   bytenum = 0;
    
    /* 清空队列 */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptSendList);

    /* 通知主应用调度线程通道打开 */
    dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
    ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
    ptChanInfo->workmode = 2;
    ptChanInfo->medium = COMM_MEDIA_APP;
    ptChanInfo->connType = 0;
    ptChanInfo->bFirst = TRUE;
    ptChanInfo->heart = 0;
    ptChanInfo->dailcnt = 3;
    ptChanInfo->dailtime = 60;
    ptIPCMsg->id = CHL_APP;
    ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
    ptChanInfo->ip = 0;
    ptChanInfo->port = 0;
    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
    COMM_FMT_DEBUG("APP透传通道%d打开\n", ptIPCMsg->id);

    sleep(2);
    ptHandle->bOpen = TRUE;
    while(1)
    {
        *ptHandle->pRunFlag = 0;
        dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);
        if(dataLen > 4)
        {
            pMsgOut->rpm = 0;
            pMsgOut->priority = ptHandle->tMsgInfo.priority;

            pMsgOut->sourlen = ptHandle->tMsgInfo.destlen;
            memcpy(pMsgOut->souraddr, ptHandle->tMsgInfo.destaddr, ptHandle->tMsgInfo.destlen);
            pMsgOut->destlen = ptHandle->tMsgInfo.sourlen;
            memcpy(pMsgOut->destaddr, ptHandle->tMsgInfo.souraddr, ptHandle->tMsgInfo.sourlen);
            
            pMsgOut->index = ptHandle->tMsgInfo.index;
            pMsgOut->label = ptHandle->tMsgInfo.label;
            pMsgOut->IID = ptHandle->tMsgInfo.IID;
            pMsgOut->IOP = ptHandle->tMsgInfo.IOP;

            SET_LEN_OFFSET(dataLen - 4, byte, bytenum);
            pMsgOut->msglen = dataLen - 4 + bytenum;
            
            memcpy(pMsgOut->playload, byte, bytenum);
            memcpy(pMsgOut->playload + bytenum, dataBuf + 4, dataLen - 4);

            ret = comm_send_label_msg(MSG_COMM_APP_TRANS, pMsgOut->label, (uint8 *)pMsgOut, sizeof(MESSAGE_INFO_T) + pMsgOut->msglen);
            COMM_BUF_DEBUG(dataBuf + 4, dataLen - 4, "APP透传应答(%s)%s", pMsgOut->destaddr, 0 == ret ? "" : "失败");
            memset(dataBuf, 0 , dataLen);
            memset(tempBuf, 0, 4096 + 512);
        }
        usleep(100000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);
}


