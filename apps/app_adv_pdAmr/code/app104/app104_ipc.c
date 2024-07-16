/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief��   ��Ӧ�õ��Ƚ���IPC��ںͳ���
* @date��    2019-12-3
* @history��
*********************************************************************
*/
#include "baseCommon.h"

#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "framecomm.h"

#include "comm_port.h"
#include "app104_ipc.h"
#include "app104_base.h"
#include "security.h"

int  g_App104SockFd = 0;

/**********************************************************************
* @name      : advPd_appipc_init
* @brief     ��IPC��Ϣ��ʼ��
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/-1ʧ��
* @Create    : ����
* @Date      ��2020-3-9
* @Update    :
**********************************************************************/
int advPd_appipc_init()
{
    uint32 sender = 0;
    int ret;
    uint8 uMsgBuf[48] = {0};
    TASK_MSG_t *pmsg = (TASK_MSG_t *)uMsgBuf;
    COMM_IPCMSG_T *pMsg = NULL;

    g_App104SockFd = task_msg_init(PD_APP104_IPC);
    if(g_App104SockFd <= 0)  /*��ʧ�ܣ���ʱ1S���³��Դ򿪽��̼�ͨѶ*/
    {
        APP104_FMT_DEBUG("appmain task_msg_init error!\n");
        sleep(1);
        return -1;
    }
    task_msg_settime(g_App104SockFd, 0, 10000);

    while(1)
    {
        uMsgBuf[1] = PROT_APP104_START;
        if(0 != task_send_msg(g_App104SockFd, MSG_COMM_APPAMIN, uMsgBuf, 2, PD_COMM_IPC))
        {
            APP104_FMT_TRACE("Appmain Send start failed\n");
            sleep(5);
            continue;
        }
        sleep(1);
        uMsgBuf[1] = 0;
        ret = task_recv_msg(g_App104SockFd, uMsgBuf, 48, &sender);
        if(ret > 0 && PD_COMM_IPC == sender && MSG_COMM_APPAMIN == pmsg->msg_id)
        {
            pMsg = (COMM_IPCMSG_T *)pmsg->msg;
            if(PROT_COMM_START == pMsg->protocol)
            {
                APP104_FMT_DEBUG("Appmain connect with commucation\n");
                return 0;
            }
        }
        sleep(1);
    }

    return -1;
}

/**********************************************************************
* @name      : advPd_send_104msg
* @brief     ��������Ϣ
* @param[in] ��uint8  id        ͨ��id
               uint8  protocol  ˽��Э��
               uint8  appType   ��protocol104.h
               uint8 *pMsg      ��Ϣ����
               uint8  msgLen    ��Ϣ����
* @param[out]��0-�ɹ�/������
* @return    ��
* @Create    : ����
* @Date      ��2019-12-4
* @Update    :
**********************************************************************/
int advPd_send_104msg(uint8 id, uint8 protocol, uint8 appType, uint8 *pMsg, uint8 msgLen)
{
    uint8  uEmergMsg[ADVPD_MSG_BUF_LENGTH + 4] = {0};

    uEmergMsg[0] = id;
    uEmergMsg[1] = protocol;
    uEmergMsg[2] = msgLen & 0x00ff;
    uEmergMsg[3] = (msgLen & 0xff00) >> 8;

	memcpy(uEmergMsg + 4, pMsg, msgLen);
    return task_send_msg(g_App104SockFd, MSG_COMM_APPAMIN, uEmergMsg, msgLen + 4, PD_COMM_IPC);
}

/**********************************************************************
* @name      : app_ipc_recv
* @brief     ��IPC������Ϣ
* @param[in] ��void *arg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-12-3
* @Update    :
**********************************************************************/
void *advPd_appipc_entry(void *pArg)
{
    IPC_THREAD_T  *pHandle = (IPC_THREAD_T *)pArg;
    int            rcvLen = 0;
    COMM_IPCMSG_T *pMsg = NULL;
    uint8          id = 0;
    uint32         sender = 0;
    uint8          msgBuf[ADVPD_MSG_BUF_LENGTH] = {0};
    //uint8          newmsgBuf[ADVPD_MSG_BUF_LENGTH] = {0};
    //int            newmsglen = 0;
    TASK_MSG_t    *pmsg = (TASK_MSG_t *)msgBuf;

    /* �޸ĳ�����ʽ(����ʱ�䳤) */
    task_msg_settime(g_App104SockFd, 1, 0);

    while(1)
    {
        *pHandle->pRunFlag = 0;

        rcvLen = task_recv_msg(g_App104SockFd, msgBuf, ADVPD_MSG_BUF_LENGTH, &sender);
        if(rcvLen < sizeof(COMM_IPCMSG_T) || PD_COMM_IPC != sender || MSG_COMM_APPAMIN != pmsg->msg_id)
        {
            continue;
        }

        pMsg = (COMM_IPCMSG_T *)pmsg->msg;
        rcvLen = pmsg->msglen;
        id = pMsg->id;
        switch(pMsg->protocol)
        {
            case PRO_DATA_TRANSMIT:  //�������ݴ���
            {
                if(MAX_CHANNEL_NUM <= id)
                {
                    break;
                }

                APP104_FMT_TRACE("channel id %d transmit\n", id);
                advPd_add_commmsg(id, PRO_DATA_TRANSMIT, pMsg->msg, rcvLen - sizeof(COMM_IPCMSG_T));
                break;
            }
            case PROT_OPEN_CHANNLE:  //ͨ������
            {
                APP104_FMT_DEBUG("channel id %d open\n", id);
                advPd_add_commmsg(id, PROT_OPEN_CHANNLE, pMsg->msg, rcvLen - sizeof(COMM_IPCMSG_T));
                break;
            }
            case PRO_PARA_MOD:  //�����޸�
            {
                APP104_FMT_TRACE("channel id %d change\n", id);
                advPd_add_commmsg(id, PRO_PARA_MOD, pmsg->msg, rcvLen);
                break;
            }
            case PROT_CLOSE_CHANNLE: //ͨ���ر�
            {
                APP104_FMT_DEBUG("channel id %d close\n", id);
                advPd_add_commmsg(id, PROT_CLOSE_CHANNLE, pmsg->msg, rcvLen);
                break;
            }
            default: break;
        }
        memset(msgBuf, 0, ADVPD_MSG_BUF_LENGTH);
    }
}


