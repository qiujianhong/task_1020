/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ���������ܣ��ⲿ��Ϣ�շ�\���ò���
* @date��    2020-3-6
* @history�� 
*********************************************************************
*/

#include "baseCommon.h"
#include "msgUdp.h"
#include "task_msg.h"
#include "db_api.h"
#include "task_id.h"
#include "msg_id.h"

#include "common_base.h"
#include "comm_msg.h"

pthread_mutex_t  g_fCommMsg;
int              g_CommTaskSock = 0;
int              g_CommTaskSync = 0;


/**********************************************************************
* @name      : comm_outtimer_add
* @brief     ������ͨ���̶߳�ʱ��ע��
* @param[in] ��COMM_OUTMSG_HANDLE_T *ptHandle   �߳̾��
               uint16 outTime                   ��ʱ��ʱ��, ��λs
               pFunc pFunc                      ��ʱ����ں���
               void *pArg                       ��Ϣ˽������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-5
* @Update    :
**********************************************************************/
void comm_outtimer_add(COMM_OUTMSG_HANDLE_T *ptHandle, uint16 outTime, TimeFUN pFunc, void *pArg)
{
    if(ptHandle->nTimerNum >= 16 || NULL == pFunc)
    {
        COMM_FMT_DEBUG("comm_outmsg_add failed\n");
        return;
    }

    ptHandle->tTImer[ptHandle->nTimerNum].nTime = outTime;
    ptHandle->tTImer[ptHandle->nTimerNum].pTimeFunc = pFunc;
    ptHandle->tTImer[ptHandle->nTimerNum].pArg = pArg;

    ptHandle->nTimerNum++;
}

/**********************************************************************
* @name      : comm_outmsg_add
* @brief     �������첽��Ϣ������ע��
* @param[in] ��COMM_OUTMSG_HANDLE_T *ptHandle   �߳̾��
               uint16 IID
               uint16 IOP
               uint16 label                     ��д0xffff��ʾ����
               MsgFUN pFunc                     ��Ϣ��ں���
               void *pArg                       ��Ϣ˽������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-5
* @Update    :
**********************************************************************/
void comm_outmsg_add(COMM_OUTMSG_HANDLE_T *ptHandle, uint32 msgid, uint16 label, MsgFUN pFunc, void *pArg)
{
    if(ptHandle->nMsgNum >= 24 || NULL == pFunc)
    {
        COMM_FMT_DEBUG("comm_outmsg_add failed\n");
        return;
    }
    ptHandle->tMsg[ptHandle->nMsgNum].msgid = msgid;
    ptHandle->tMsg[ptHandle->nMsgNum].label = label;
    ptHandle->tMsg[ptHandle->nMsgNum].pMsgFunc = pFunc;
    ptHandle->tMsg[ptHandle->nMsgNum].pArg = pArg;

    ptHandle->nMsgNum++;
}

/**********************************************************************
* @name      : comm_cfgmsg_entry
* @brief     �������첽��Ϣ�����߳�
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2019-11-5
* @Update    :
**********************************************************************/
void *comm_outmsg_entry(void *pArg)
{
    uint8  i;
    uint8       recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;
    int         recvlen = 0;
    uint32      sender = 0;
    time_t      connetTime; 
    time_t      lastTime;
    uint32      offsetTime = 0;
    
    COMM_MSG_T             *ptMsgProc = NULL;
    COMM_TIMER_T              *pTimer = NULL;
    COMM_OUTMSG_HANDLE_T *ptMsgHandle = (COMM_OUTMSG_HANDLE_T*)pArg;

    /* ��ʼ�� */
    if(ptMsgHandle->socketfd <= 0)
    {
        ptMsgHandle->socketfd = task_msg_init(PD_COMM_MSG);
        if(ptMsgHandle->socketfd <= 0)
        {
            COMM_FMT_DEBUG("COMM_MSG task msg init failed\n");
            return 0;
        }
    }
    task_msg_settime(ptMsgHandle->socketfd, 0, 10000);  //����������ʱʱ��Ϊ0.1s
    g_CommTaskSock = ptMsgHandle->socketfd;

    if(ptMsgHandle->syncsocket <= 0)
    {
        ptMsgHandle->syncsocket = task_msg_init(PD_COMM_SYNCMSG);
        if(ptMsgHandle->syncsocket <= 0)
        {
            COMM_FMT_DEBUG("COMM_MSG task msg init failed\n");
            return 0;
        }
        pthread_mutex_init(&g_fCommMsg, NULL);
    }
    g_CommTaskSync = ptMsgHandle->syncsocket;

    lastTime = time(NULL);
    for(;;)
    {
        *ptMsgHandle->pRunFlag = 0;

        /* 1 ����Ϣ���� */
        recvlen = task_recv_msg(ptMsgHandle->socketfd, recvBuf, TASK_MSG_MAX_LEN, &sender);
        if(recvlen > 0)
        {
            COMM_BUF_TRACE(pMsg->msg, pMsg->msglen, "COMM recv msg 0x%x, label %d", pMsg->msg_id, pMsg->label);
            for(i = 0; i < ptMsgHandle->nMsgNum; i++)
            {
                ptMsgProc = &(ptMsgHandle->tMsg[i]);
                if(ptMsgProc->msgid == pMsg->msg_id && (ptMsgProc->label == pMsg->label || 0xffff == ptMsgProc->label)
                    && NULL != ptMsgProc->pMsgFunc)
                {
                    ptMsgProc->pMsgFunc(pMsg->msg, pMsg->msglen, ptMsgProc->pArg);
                }
            }
            memset(recvBuf, 0, recvlen);
        }
        
        /* 2 ��ʱ�� */
        connetTime = time(NULL);
        offsetTime = connetTime - lastTime;
        if(offsetTime > 0)
        {
            for(i = 0; i < ptMsgHandle->nTimerNum; i++)
            {
                pTimer = &(ptMsgHandle->tTImer[i]);
                pTimer->nTimeCnt += (uint16)offsetTime;
                if(pTimer->nTimeCnt >= pTimer->nTime && NULL != pTimer->pTimeFunc)
                {
                    pTimer->pTimeFunc(ptMsgHandle->socketfd, pTimer->pArg);
                    pTimer->nTimeCnt = 0;
                }
            }
            lastTime = connetTime;
        }
    }
}

/**********************************************************************
* @name      : comm_add_route_net
* @brief     ������·��
* @param[in] ��char *pNetName    ��������
               uint8 *pRouteAddr  ·�ɵ�ַ
* @return    ��
* @Create    : ����
* @Date      ��2020-3-18
* @Update    :
**********************************************************************/
void comm_add_route_net(const char *pNetName, uint32 ip)
{
    uint8  sysCom[100] = {0};
    uint8  recvbuf[32] = {0};
    uint16 offset = 0;
    int    ret = 0;
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvbuf;

    sysCom[offset++] = strlen(pNetName);
    memcpy(sysCom + offset, pNetName, sysCom[0]);
    offset += sysCom[0];
    
    sysCom[offset++] = 1;
    sysCom[offset++] = 4;
    appmsg_memcpy_r(sysCom + offset, &ip, sizeof(uint32));
    offset += sizeof(uint32);

    if(0 == ip)
    {
        COMM_FMT_DEBUG("%s add route 0x%08x error\n", pNetName, ip);
        return;
    }

    PTHREAD_LOCK_SAFE(&g_fCommMsg);
    ret = task_syncmsg_safe(g_CommTaskSync, MSG_COMM_ADDROUTE, sysCom, offset, PD_MSG_MONITOR, recvbuf, 32, 2);  
    PTHREAD_UNLOCK_SAFE(&g_fCommMsg);

    if(0 == ret)
    {
        if(0 == pMsg->msglen || FALSE == *pMsg->msg)
        {
            ret = 1;
        }
    }
    COMM_FMT_DEBUG("%s add route 0x%x, ret %d\n", pNetName, ip, ret);
    
}

/**********************************************************************
* @name      : comm_del_route_net
* @brief     ��ɾ��·��
* @param[in] ��char *pNetName    ��������
               uint8 *pRouteAddr  ·�ɵ�ַ
* @return    ��
* @Create    : ����
* @Date      ��2020-3-19
* @Update    :
**********************************************************************/
void comm_del_route_net(const char *pNetName, uint32 ip)
{
    uint8  sysCom[100] = {0};
    uint8  recvbuf[32] = {0};
    uint16 offset = 0;
    int    ret = 0;
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvbuf;

    sysCom[offset++] = strlen(pNetName);
    memcpy(sysCom + offset, pNetName, sysCom[0]);
    offset += sysCom[0];
    
    sysCom[offset++] = 1;
    sysCom[offset++] = 4;
    appmsg_memcpy_r(sysCom + offset, &ip, sizeof(uint32));
    offset += sizeof(uint32);

    if(0 == ip)
    {
        COMM_FMT_DEBUG("%s del route 0x%08x error\n", pNetName, ip);
        return;
    }

    PTHREAD_LOCK_SAFE(&g_fCommMsg);
    ret = task_syncmsg_safe(g_CommTaskSync, MSG_COMM_DELROUTE, sysCom, offset, PD_MSG_MONITOR, recvbuf, 32, 2);  
    PTHREAD_UNLOCK_SAFE(&g_fCommMsg);

    if(0 == ret)
    {
        if(0 == pMsg->msglen || FALSE == *pMsg->msg)
        {
            ret = 1;
        }
    }
    COMM_FMT_DEBUG("%s del route 0x%x, ret %d\n", pNetName, ip, ret);
}

/**********************************************************************
* @name      : comm_send_label_msg
* @brief     ������Я��label��Ϣ
* @param[in] ��uint32 msgid    ��ϢID
               uint16 label    label
               uint8 *msgdata  ��Ϣ����
               uint16 datalen  ��Ϣ����
* @return    ��
* @Create    : ����
* @Date      ��2020-8-29
* @Update    :
**********************************************************************/
int comm_send_label_msg(uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen)
{
    return task_send_labelmsg(g_CommTaskSock, msgid, label, msgdata, datalen, PD_MSG_MONITOR);
}

