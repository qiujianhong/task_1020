/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   扩展控制模块 数据
* @date：    2021-5-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "iniparser.h"
#include "framecomm.h"
#include "appctrl_data.h"

//调节日志路径
char *gLogPath[MAX_PORT_NUM] = {"/data/app/stAmr/log/secTrans1.log", "/data/app/stAmr/log/secTrans2.log", 
                                "/data/app/stAmr/log/secTrans3.log", "/data/app/stAmr/log/secTrans4.log", 
                                "/data/app/stAmr/log/secTrans5.log", "/data/app/stAmr/log/secTrans6.log", 
                                "/data/app/stAmr/log/secTrans7.log", "/data/app/stAmr/log/secTrans8.log"};  //调节日志路径


/**********************************************************************
* @name      : ctrl_clean_queue
* @brief     ：清队列
* @param[in] ：pQueue  缓冲区结构指针
* @param[out]：
* @return    ：
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
* @brief     ：读队列
* @param[in] ：pQueue      缓冲区结构指针
* @param[out]：
* @return    ：BLE_DEV_MSG_T *消息队列首消息
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
* @brief     ：写队列 按照先后顺序
* @param[in] ：pQueue        缓冲区结构指针
               pMsgProc      需要写入队列的数据(需要提前申请内存)
               uint8 bOrder  是否排序
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
void ctrl_write_queue(CTRL_QUEUE_T *pQueue, CTRL_DEV_MSG_T *pMsgProc, uint8 bOrder)
{
    CTRL_DEV_MSG_T *pPre = NULL;
    CTRL_DEV_MSG_T *pNext = NULL;

    /* 配置不区分先后顺序 */
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

    /* 消息按照优先级排序 */
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



