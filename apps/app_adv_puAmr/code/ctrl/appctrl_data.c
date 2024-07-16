/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��չ����ģ�� ����
* @date��    2021-5-30
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "iniparser.h"
#include "framecomm.h"
#include "appctrl_data.h"

//������־·��
char *gLogPath[MAX_PORT_NUM] = {"/data/app/stAmr/log/secTrans1.log", "/data/app/stAmr/log/secTrans2.log", 
                                "/data/app/stAmr/log/secTrans3.log", "/data/app/stAmr/log/secTrans4.log", 
                                "/data/app/stAmr/log/secTrans5.log", "/data/app/stAmr/log/secTrans6.log", 
                                "/data/app/stAmr/log/secTrans7.log", "/data/app/stAmr/log/secTrans8.log"};  //������־·��


/**********************************************************************
* @name      : ctrl_clean_queue
* @brief     �������
* @param[in] ��pQueue  �������ṹָ��
* @param[out]��
* @return    ��
**********************************************************************/
void ctrl_clean_queue(CTRL_QUEUE_T *pQueue)
{
    CTRL_DEV_MSG_T *pTemp = NULL;
    pthread_mutex_lock(&pQueue->f_lock);
    pTemp = pQueue->pHead;
    while(pTemp)
    {
        pQueue->pHead = pTemp->pNext;
        free(pTemp);
        pTemp = pQueue->pHead;
    }
    pthread_mutex_unlock(&pQueue->f_lock);
}

/**********************************************************************
* @name      : ble_read_queue
* @brief     ��������
* @param[in] ��pQueue      �������ṹָ��
* @param[out]��
* @return    ��BLE_DEV_MSG_T *��Ϣ��������Ϣ
**********************************************************************/
CTRL_DEV_MSG_T *ctrl_read_queue(CTRL_QUEUE_T *pQueue)
{
    CTRL_DEV_MSG_T *pHead = NULL;
    
    pthread_mutex_lock(&pQueue->f_lock);

    pHead = pQueue->pHead;
    if(pHead)
    {
        pQueue->pHead = pHead->pNext;
        pHead->pNext = NULL;
    }

    pthread_mutex_unlock(&pQueue->f_lock);
    return pHead;
}

/**********************************************************************
* @name      : ble_write_queue
* @brief     ��д���� �����Ⱥ�˳��
* @param[in] ��pQueue        �������ṹָ��
               pMsgProc      ��Ҫд����е�����(��Ҫ��ǰ�����ڴ�)
               uint8 bOrder  �Ƿ�����
* @param[out]��
* @return    ��0-�ɹ�/������
**********************************************************************/
void ctrl_write_queue(CTRL_QUEUE_T *pQueue, CTRL_DEV_MSG_T *pMsgProc, uint8 bOrder)
{
    CTRL_DEV_MSG_T *pPre = NULL;
    CTRL_DEV_MSG_T *pNext = NULL;

    /* ���ò������Ⱥ�˳�� */
    pthread_mutex_lock(&pQueue->f_lock);

    pPre = pQueue->pHead;
    if(FALSE == bOrder)
    {
        if(NULL == pPre)
        {
            pQueue->pHead = pMsgProc;
            pMsgProc->pNext = NULL;
        }
        else
        {
            pNext = pPre->pNext;
            while(pNext)
            {
                pPre = pNext;
                pNext = pPre->pNext;
            }
            pPre->pNext = pMsgProc;
            pMsgProc->pNext = NULL;
        }

        goto end;
    }

    /* ��Ϣ�������ȼ����� */
    if(NULL == pPre)
    {
        pQueue->pHead = pMsgProc;
        pMsgProc->pNext = NULL;
    }
    else if(pPre->priority > pMsgProc->priority)
    {
        pMsgProc->pNext = pQueue->pHead;
        pQueue->pHead = pMsgProc;
    }
    else
    {
        pNext = pPre->pNext;
        while(pNext)
        {
            if(pNext->priority > pMsgProc->priority)
            {
                break;
            }
            pPre = pNext;
            pNext = pPre->pNext;
        }
        pPre->pNext = pMsgProc;
        pMsgProc->pNext = pNext;
    }

end:
    pthread_mutex_unlock(&pQueue->f_lock);
}



