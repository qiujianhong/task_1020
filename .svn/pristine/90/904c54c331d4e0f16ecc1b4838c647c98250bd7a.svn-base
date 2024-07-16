/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程IPC入口和出口
* @date：    2020-3-9
* @history： 
*********************************************************************
*/
#include "baseCommon.h"

#include "comm_port.h"
#include "app104_base.h"

COMM_MSG_LIST_T  g_AppCommMsg;

/**********************************************************************
* @name      : advPd_appcomm_msginit
* @brief     ：IPC缓存消息初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-9
* @Update    :
**********************************************************************/
void advPd_comm_msginit()
{
    memset(&g_AppCommMsg, 0, sizeof(COMM_MSG_LIST_T));
    pthread_mutex_init(&g_AppCommMsg.f_lock, NULL);
}

/**********************************************************************
* @name      : advPd_read_commmsg
* @brief     ：读队列
* @param[in] ：COMM_MSG_LIST_T *pQueue  缓冲区结构指针
* @param[out]：uint8 *pMsg              读数据缓冲区
               uint8 *pId               通道ID
               uint8 *pProtocol         内部协议
* @return    ：数据长度
* @Create    : 王津剑
* @Date      ：2020-3-9
* @Update    :
**********************************************************************/
uint16 advPd_read_commmsg(COMM_MSG_LIST_T *pLIst, uint8 *pId, uint8 *pProtocol, uint8 *pMsg)
{
    uint16 len;
    uint8 *pdataBuf;
    
    pthread_mutex_lock(&pLIst->f_lock);
    len = 0;

    /*若队列中有数据，则取到buf中，解开锁*/
    if ((pLIst->head != pLIst->tail)||((pLIst->head == pLIst->tail)&&(pLIst->queue[pLIst->head].msglen != 0)))
    {
        pdataBuf = pLIst->queue[pLIst->head].msgbuf;
        len = pLIst->queue[pLIst->head].msglen;
        memcpy(pMsg, pdataBuf, len);  /*取队列中的数据*/
        memset(pdataBuf, 0, len);     /*清数据*/
        *pId = pLIst->queue[pLIst->head].id;
        *pProtocol = pLIst->queue[pLIst->head].protocol;
        pLIst->queue[pLIst->head].id = CHL_RESERVE;
        pLIst->queue[pLIst->head].msglen = 0;
        pLIst->head = (pLIst->head+1) % COMM_MSG_LIST_SIZE; /*队列头向后移动一位*/
    }

    pthread_mutex_unlock(&pLIst->f_lock);
    return len;
}

/**********************************************************************
* @name      : advPd_write_commmsg
* @brief     ：写队列, 若队列满，丢弃
* @param[in] ：COMM_MSG_LIST_T *pQueue  缓冲区结构指针
               uint8 id                 通道id
               uint8 protocol           内部协议
               uint8 *pMsg              消息内容
               uint32 msgLen            消息长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-9
* @Update    :
**********************************************************************/
int advPd_write_commmsg(COMM_MSG_LIST_T *pLIst, uint8 id, uint8 protocol, uint8 *pMsg, uint32 msgLen)
{
    int flags;
    flags = -1;
    if(NULL == pMsg || 0 == msgLen)
    {
        return flags;
    }
    pthread_mutex_lock(&pLIst->f_lock);

   /*判队列中是否还有空的位置*/
    if (pLIst->queue[pLIst->tail].msglen == 0)
    {
        pLIst->queue[pLIst->tail].id = id;
        pLIst->queue[pLIst->tail].protocol = protocol;
        pLIst->queue[pLIst->tail].msglen = msgLen;
        memcpy(pLIst->queue[pLIst->tail].msgbuf, pMsg, msgLen);
        flags = 0;
        pLIst->tail = (pLIst->tail+1) % COMM_MSG_LIST_SIZE;
    }
    pthread_mutex_unlock(&pLIst->f_lock);
    return flags;
}

/**********************************************************************
* @name      : advPd_get_commmsg
* @brief     ：读队列
* @param[in] ：
* @param[out]：uint8 *pMsg              读数据缓冲区
               uint8 *pId               通道ID
               uint8 *pProtocol         内部协议
* @return    ：数据长度
* @Create    : 王津剑
* @Date      ：2020-3-9
* @Update    :
**********************************************************************/
uint16 advPd_get_commmsg(uint8 *pId, uint8 *pProtocol, uint8 *pMsg)
{
    return advPd_read_commmsg(&g_AppCommMsg, pId, pProtocol, pMsg);
}

/**********************************************************************
* @name      : advPd_add_commmsg
* @brief     ：写队列, 若队列满，丢弃
* @param[in] ：uint8 id                 通道id
               uint8 protocol           内部协议
               uint8 *pMsg              消息内容
               uint32 msgLen            消息长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-9
* @Update    :
**********************************************************************/
int advPd_add_commmsg(uint8 id, uint8 protocol, uint8 *pMsg, uint32 msgLen)
{
    if(msgLen > ADVPD_104MSG_BUF_LENGTH)
    {
        return -2;
    }
    return advPd_write_commmsg(&g_AppCommMsg, id, protocol, pMsg, msgLen);
}


