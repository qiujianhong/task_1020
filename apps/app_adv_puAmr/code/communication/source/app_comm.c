/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ������ͨ��
* @date��    2019-10-31
* @history�� 
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
* @brief     ����̫��������֪ͨ��Ϣ����
* @param[in] ��void *playload    ��Ϣ����
               uint16 msglen     ��Ϣ����
               void *pArg        ˽������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2022-5-7
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
        COMM_BUF_DEBUG(pMsgIn->playload, pMsgIn->msglen, "APP͸��(%s)���ݴ���", pMsgIn->souraddr);
        return 1;
    }
    memcpy(dataBuf, pMsgIn->playload + bytesnum, MIN(4096, len));
    comm_add_channel(CHL_APP, dataBuf, len);
    bufLen = len + 4;

    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, bufLen))
    {
        COMM_FMT_DEBUG("Msg Queue is full\n");
    }
    COMM_BUF_DEBUG(dataBuf + 4, bufLen - 4, "APP͸������(%s)", pMsgIn->souraddr);
    return 0;
}

/**********************************************************************
* @name      : comm_app_trans_start
* @brief     ��APP͸�� �����ж�
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2022-5-7
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
* @brief     ��APP͸�� �߳����
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2022-5-7
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
    
    /* ��ն��� */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptSendList);

    /* ֪ͨ��Ӧ�õ����߳�ͨ���� */
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
    COMM_FMT_DEBUG("APP͸��ͨ��%d��\n", ptIPCMsg->id);

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
            COMM_BUF_DEBUG(dataBuf + 4, dataLen - 4, "APP͸��Ӧ��(%s)%s", pMsgOut->destaddr, 0 == ret ? "" : "ʧ��");
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


