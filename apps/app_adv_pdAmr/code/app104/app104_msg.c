/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ����ⲿͨ���߳�
* @date��    2020-3-9
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "framecomm.h"
#include "appmsg_split.h"

#include "app104_msg.h"
#include "pw_common.h"

int g_APP104Msg = 0;
pthread_mutex_t g_fAPP104MsgSync;
int g_APP104MsgSync = 0;

/* ģ�鷵��0-��,698��Լ1-��,���չ�Լ���� */
pthread_mutex_t g_fadvPdRmt;
RST_RMT_T       g_tadvPdRmt;
uint8           g_advPdRmtInput = 0;

extern BOOLEAN advPd_mqtt_rspok();

/**********************************************************************
* @name      : advPd_proc_rmt
* @brief     ��ң�Ŵ���
* @param[in] ��uint8 id                  �豸�� Ϊ0����д��һ�����ݿ�
               uint8 *pMsg               ��Ϣ����
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-2
* @Update    :
**********************************************************************/
void advPd_proc_rmt(uint8 id, uint8 *pMsg, uint16 msglen)
{
    uint8 i;
    RST_RMT_T tRmt;

    if(1 != id && 2 != id)
    {
        return;
    }

    APP104_BUF_DEBUG(pMsg, msglen, "Dev rmt %d msg\n", id);
    memset(&tRmt, 0, sizeof(RST_RMT_T));
    if(msglen > 0)
    {
        appmsg_rsprmt_split(pMsg, &tRmt);
    }
    else
    {
        for(i = 0; i < 4; i++)
        {
            tRmt.time[i].year = 0xffff;
            tRmt.time[i].month = 0xff;
            tRmt.time[i].day = 0xff;
        }
    }

    /* ģ�鷵��0-��,698��Լ1-��,��Ҫת�� */
    tRmt.ST = ~tRmt.ST;
    pthread_mutex_lock(&g_fadvPdRmt);
    if(1 == id && tRmt.num > 0)
    {
        g_tadvPdRmt.ST = (g_tadvPdRmt.ST & 0xf0) + (tRmt.ST & 0x0f);
        g_tadvPdRmt.CT = (g_tadvPdRmt.CT & 0xf0) + (tRmt.CT & 0x0f);
        g_advPdRmtInput = (g_advPdRmtInput & 0xf0);
        for(i = 0; i < tRmt.num; i++)
        {
            memcpy(&(g_tadvPdRmt.time[i]), &(tRmt.time[i]), sizeof(Date_Time));
            if(0xffff != tRmt.time[i].year && 0xff != tRmt.time[i].month && 0xff != tRmt.time[i].day)
            {
                g_advPdRmtInput |= (0x01 << i);
            }
        }
    }
    else if(2 == id && tRmt.num > 0)
    {
        g_tadvPdRmt.ST = (g_tadvPdRmt.ST & 0x0f) + ((tRmt.ST << 4) & 0xf0);
        g_tadvPdRmt.CT = (g_tadvPdRmt.CT & 0x0f) + ((tRmt.CT << 4) & 0xf0);
        g_advPdRmtInput = (g_advPdRmtInput & 0x0f);
        for(i = 0; i < tRmt.num; i++)
        {
            memcpy(&(g_tadvPdRmt.time[i + 4]), &(tRmt.time[i]), sizeof(Date_Time));
            if(0xffff != tRmt.time[i].year && 0xff != tRmt.time[i].month && 0xff != tRmt.time[i].day)
            {
                g_advPdRmtInput |= (0x01 << (i + 4));
            }
        }
    }

#ifdef AREA_ZHEJIANG
    yx_real_signal_update_zhj(g_tadvPdRmt, g_advPdRmtInput);
#endif
    
    APP104_FMT_DEBUG("Dev %d, num %d ST 0x%02x, CT 0x%02x. Total ST 0x%02x, CT 0x%02x; Input 0x%02x\n", id, tRmt.num,
                      tRmt.ST, tRmt.CT, g_tadvPdRmt.ST, g_tadvPdRmt.CT, g_advPdRmtInput);
    pthread_mutex_unlock(&g_fadvPdRmt);
}

/**********************************************************************
* @name      : advPd_get_rmt
* @brief     ����ȡң������
* @param[in] ��
* @param[out]��RST_RMT_T *pRmt ң��״̬ (ST bit0....bit7��ʾ1...8·; 0-��;1-��)
* @return    ��ң�Ž���״̬ (bit0....bit7��ʾ1...8·; 0-δ����;1-����)
* @Create    : ����
* @Date      ��2020-6-2
* @Update    :
**********************************************************************/
uint8 advPd_get_rmt(RST_RMT_T *pRmt)
{
    if(NULL == pRmt)
    {
        return 0;
    }
    uint8 ststus = 0;
    pthread_mutex_lock(&g_fadvPdRmt);
    memcpy(pRmt, &g_tadvPdRmt, sizeof(RST_RMT_T));
    ststus = g_advPdRmtInput;
    pthread_mutex_unlock(&g_fadvPdRmt);

    return ststus; 
}

/**********************************************************************
* @name      : advPd_rmt_init
* @brief     ����ȡң�����ݳ�ʼ��
* @param[in] ��
* @param[out]��
* @return    ��ң������
* @Create    : ����
* @Date      ��2020-6-2
* @Update    :
**********************************************************************/
void advPd_rmt_init()
{
    pthread_mutex_init(&g_fadvPdRmt, NULL);
    memset(&g_tadvPdRmt, 0, sizeof(RST_RMT_T));
    g_tadvPdRmt.num = 8;
}

/**********************************************************************
* @name      : advPd_app104msg_entry
* @brief     �������ڲ�ͨ���߳�
* @param[in] ��void *arg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-3-9
* @Update    :
**********************************************************************/
void *advPd_app104msg_entry(void *pArg)
{
    APPOUTMSG_THREAD_T *ptMsgHandle = (APPOUTMSG_THREAD_T*)pArg;
    uint8               recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t         *pMsg = (TASK_MSG_t *)recvBuf;
    int                 recvlen = 0;
    uint32              sender = 0;
    uint8               bRspOK = FALSE;
    uint8               pulseNum = 0;
    int                 ret = 0;

    if(g_APP104Msg <= 0)
    {
        g_APP104Msg = task_msg_init(PD_APP104_MSG);
        if(g_APP104Msg <= 0)
        {
            APP104_FMT_DEBUG("Appmain task msg init failed!\n");
            return 0;
        }
    }
    task_msg_settime(g_APP104Msg, 0, 200000);  //����������ʱʱ��Ϊ0.2s

    if(g_APP104MsgSync <= 0)
    {
        g_APP104MsgSync = task_msg_init(PD_APP104_SYNC);
        if(g_APP104MsgSync <= 0)
        {
            APP104_FMT_DEBUG("Appmain task msg init failed!\n");
            return 0;
        }
        pthread_mutex_init(&g_fAPP104MsgSync, NULL);
    }
    while(1)
    {
        *ptMsgHandle->pRunFlag = 0;
        recvlen = task_recv_msg(g_APP104Msg, recvBuf, TASK_MSG_MAX_LEN, &sender);
        if(recvlen > 0)
        {
            if(MSG_PULSE_GET_RMT_ENENT == pMsg->msg_id)
            {
                pulseNum = *pMsg->msg;
                advPd_proc_rmt(pulseNum, pMsg->msg + 1, pMsg->msglen - 1);
            }
            else if(MSG_PULSE_GET_RMT == pMsg->msg_id)
            {
                pulseNum = (uint8)pMsg->label;
                advPd_proc_rmt(pulseNum, pMsg->msg, pMsg->msglen);
            }
            memset(recvBuf, 0, recvlen);
        }

        /* ��ȡ�״�ң��״̬ */
        if(FALSE == bRspOK && TRUE == advPd_mqtt_rspok())
        {
            bRspOK = TRUE;
            pulseNum = 1;
            ret = task_send_labelmsg(g_APP104Msg, MSG_PULSE_GET_RMT, 1, &pulseNum, 1, PD_MSG_MONITOR);
            APP104_FMT_DEBUG("Set port 1 send get emt %s\n", 0 == ret ? "success" : "failed");
        
            pulseNum = 2;
            ret = task_send_labelmsg(g_APP104Msg, MSG_PULSE_GET_RMT, 2, &pulseNum, 1, PD_MSG_MONITOR);
            APP104_FMT_DEBUG("Set port 2 send get emt %s\n", 0 == ret ? "success" : "failed");
        }
    }
    return 0;
}


/**********************************************************************
* @name      : appPd_send_syncmsg
* @brief     ��APPAMIN����ͬ����Ϣ��Ϣ, ������Ϣ�������
* @param[in] ��uint32 msgid    ��ϢID
               uint8 *sendbuf  ��������
               uint16 sendlen  ���ͳ���
               uint16 buflen   ���ջ����С
* @param[out]��uint8 *recvbuf  ��������
               uint16 *recvLen ���ճ���
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-11-19
* @Update    :
**********************************************************************/
int appPd_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen)
{
    int    ret = 0;
    uint8  recvBuf[TASK_MSG_MAX_LEN] = {0};
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvBuf;

    PTHREAD_LOCK_SAFE(&g_fAPP104MsgSync);
    ret = task_syncmsg_safe(g_APP104MsgSync, msgid, sendbuf, sendlen, PD_MSG_MONITOR, recvBuf, TASK_MSG_MAX_LEN, 4);
    PTHREAD_UNLOCK_SAFE(&g_fAPP104MsgSync);

    if(0 == ret)
    {
        if(buflen < pMsg->msglen)
        {
            return -100;
        }
        memcpy(recvbuf, pMsg->msg, pMsg->msglen);
        *recvLen = pMsg->msglen;
    }

    return ret;
}

/**********************************************************************
* @name      : appPd_reboot_to_smiOS
* @brief     ����ϵͳ������������������
* @param[in] ��
* @return    ��0-�ɹ�/������
* @Date      ��2020-11-19
* @Update    :
**********************************************************************/
int appPd_reboot_to_smiOS(void)
{
    uint8  recvbuf[32] = {0};
    uint16 recLen = 0;
    int    ret = 0;
    int    nRet = 0;

    ret = appPd_send_syncmsg(MSG_APPAMIN_REBOOT, NULL, 0, recvbuf, 32, &recLen);
    if ((ret != ERR_OK) || (recLen != 1) || (recvbuf[0] != 1))
    {
        PRTL_FMT_DEBUG("reboot_sent_to_smiOS err! ret = %d\n", ret);
        nRet = ERR_NORMAL;
    }
    else
    {
        nRet = ERR_OK;
    }
    
    return nRet;
}



