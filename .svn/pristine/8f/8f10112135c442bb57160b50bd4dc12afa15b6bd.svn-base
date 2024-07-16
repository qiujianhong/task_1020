/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief：控制消息优先级队列
* @date： 2019-11-15
*********************************************************************
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "baseType.h"
#include "msgPqueue.h"
#include "pqueue.h"
#include "olog.h"

#define CHECK_NULL_PARA_TO_RETURN(X, Y)                                        \
    ({                                                                         \
        if (NULL == X)                                                         \
        {                                                                      \
            nRet = Y;                                                          \
            break;                                                             \
        }                                                                      \
    })
#define CHECK_NOTPLUS_PARA_TO_RETURN(X, Y)                                     \
    ({                                                                         \
        if (X < 0)                                                             \
        {                                                                      \
            nRet = Y;                                                          \
            break;                                                             \
        }                                                                      \
    })
#define CHECK_ZERO_PARA_TO_RETURN(X, Y)                                        \
    ({                                                                         \
        if (0 == X)                                                            \
        {                                                                      \
            nRet = Y;                                                          \
            break;                                                             \
        }                                                                      \
    })

/**
*********************************************************************
* @brief： 取得节点优先级函数，初始化时设定
* @param[in]：node 节点指针
* @param[out]：none
* @return： none
*********************************************************************
*/
static pqueue_pri_t msg_pqueue_get_pri(void *pnode)
{
    return ((MSG_QUEUE_NODE_T *)pnode)->pri;
}

/**
*********************************************************************
* @brief： 设定节点的优先级函数，初始化时候设定
* @param[in]：
               nPri 优先级
* @param[out]：
               pnode
* @return： none
*********************************************************************
*/
static void msg_pqueue_set_pri(void *pnode, pqueue_pri_t nPri)
{
    ((MSG_QUEUE_NODE_T *)pnode)->pri = nPri;
}

/**
*********************************************************************
* @brief： 优先级高低判断函数，初始化时候设定(影响出队列的先后)
* @param[in]：
               curr 当前节点的优先级
               next 下一节点的优先级
* @param[out]：
            none
* @return： none
*********************************************************************
*/
static int msg_pqueue_cmp_pri(pqueue_pri_t nNext, pqueue_pri_t nCurr)
{
    return (nNext > nCurr);
}

/**
*********************************************************************
* @brief： 取出位置函数。初始化时候设定
* @param[in]：pnode
* @param[out]：
            none
* @return： node位置
*********************************************************************
*/
static size_t msg_pqueue_get_pos(void *pnode)
{
    return ((MSG_QUEUE_NODE_T *)pnode)->pos;
}

/**
*********************************************************************
* @brief： 设定位置函数，初始化时候设定
* @param[in]： npos 位置
* @param[out]：
            pnode
* @return： none
*********************************************************************
*/
static void msg_pqueue_set_pos(void *pnode, size_t npos)
{
    ((MSG_QUEUE_NODE_T *)pnode)->pos = npos;
}

/**
*********************************************************************
* @brief： 移除所有消息
* @param[in]：
               PQUEUE_T
* @param[out]：none
* @return： 0 成功
            <0 失败
*********************************************************************
*/
static sint32 msg_queue_remove_all(PQUEUE_T *pqueue)
{
    MSG_QUEUE_NODE_T *head = NULL;
    sint32            nRet = 0;

    do
    {
        if (!(pqueue->pq) || (pqueue->pq->size) == 1)
        {
            nRet = -1;
            break;
        }
        head             = (MSG_QUEUE_NODE_T *)pqueue->pq->d[1];
        pqueue->pq->d[1] = pqueue->pq->d[--(pqueue->pq->size)];

        free(head->msgobj);
        free(head);
    } while (pqueue->pq->size > 1);
    return nRet;
}
/**
*********************************************************************
* @brief： 消息队列初始化
* @param[in]：
               void
* @param[out]：
               none
* @return： PQUEUE_T *
*********************************************************************
*/
PQUEUE_T *msg_queue_init()
{
    PQUEUE_T *pqueue = malloc(sizeof(PQUEUE_T));

    pqueue->pq =
        pqueue_init(MSG_PQUEUE_MAX_SIZE, msg_pqueue_cmp_pri, msg_pqueue_get_pri,
                    msg_pqueue_set_pri, msg_pqueue_get_pos, msg_pqueue_set_pos);
    pqueue->maxsize = MSG_PQUEUE_MAX_SIZE;
    if (pthread_mutex_init(&(pqueue->lock), NULL) != 0)
    {
        free(pqueue->pq);
        free(pqueue);
        pqueue = NULL;
    }
    return pqueue;
}

/**
*********************************************************************
* @brief： 消息队列初始化
* @param[in]：
               队列最大数量
* @param[out]：
               none
* @return： none
*********************************************************************
*/
PQUEUE_T *msg_queue_init_by_max_count(sint32 maxCount)
{
    PQUEUE_T *pqueue = malloc(sizeof(PQUEUE_T));

    pqueue->pq =
        pqueue_init(maxCount, msg_pqueue_cmp_pri, msg_pqueue_get_pri,
                    msg_pqueue_set_pri, msg_pqueue_get_pos, msg_pqueue_set_pos);
    pqueue->maxsize = maxCount;
    if (pthread_mutex_init(&(pqueue->lock), NULL) != 0)
    {
        free(pqueue->pq);
        free(pqueue);
        pqueue = NULL;
    }
    return pqueue;
}

/**
*********************************************************************
* @brief： 将消息放入队列中，并排序
* @param[in]：
               pqueue 消息队列   
               psrc 放入队列的消息结构体
* @param[out]：
               none
* @return： 0 成功
            <0 失败
*********************************************************************
*/
sint32 msg_queue_obj_push(PQUEUE_T *pqueue, MSG_QUEUE_NODE_T *psrc)
{
    MSG_QUEUE_NODE_T *pnode = NULL;
    int               nRet  = 0;
    do
    {
        //检查是否已经初始化,输入参数
        CHECK_NULL_PARA_TO_RETURN(pqueue->pq, -1);
        CHECK_NULL_PARA_TO_RETURN(psrc, -1);
        CHECK_NOTPLUS_PARA_TO_RETURN(psrc->len, -1);

        pnode = (MSG_QUEUE_NODE_T *)malloc(sizeof(MSG_QUEUE_NODE_T));
        CHECK_NULL_PARA_TO_RETURN(pnode, -1);
        pnode->msgobj = malloc(psrc->len);
        memcpy(pnode->msgobj, psrc->msgobj, psrc->len);
        pnode->pri = psrc->pri;
        pnode->len = psrc->len;

        pthread_mutex_lock(&(pqueue->lock));
        if (((pqueue->pq)->size) > (pqueue->maxsize))
        {
            dzlog_warn("pqueue is FULL,can't push.[pri:%d][len:%d]", psrc->pri, psrc->len);
            free(pnode->msgobj);
            free(pnode);
            pthread_mutex_unlock(&(pqueue->lock));
            nRet = -1;
            break;
        }
        if (0 != pqueue_insert(pqueue->pq, pnode))
        {
            //插入异常
            dzlog_debug(
                "push msg failure!pri:[%d],msgobj:[%p],queue size is [%zu]\n",
                pnode->pri, pnode->msgobj, ((pqueue->pq->size) - 1));
            free(pnode->msgobj);
            free(pnode);
            nRet = -1;
        }
        if (nRet > -1)
        {
            // dzlog_debug("push msg sucess pri:[%d],msgobj:[%p]\n", pnode->pri,
            //             pnode->msgobj);
        }

        pthread_mutex_unlock(&(pqueue->lock));
    } while (0);
    psrc->pos = pnode->pos;
    return nRet;
}

/**
*********************************************************************
* @brief： 弹出消息队列中最高优先级的消息，并重新排列
* @param[in]：
               pqueue 消息队列
* @param[out]：
               pdest 弹出队列的消息结构体
* @return： 0 成功
            <0 失败
*********************************************************************
*/
sint32 msg_queue_obj_pop(PQUEUE_T *pqueue, MSG_QUEUE_NODE_T *pdest)
{
    MSG_QUEUE_NODE_T *pnode = NULL;
    sint32            nRet  = 0;
    do
    {
        //判断是否已经初始化
        CHECK_NULL_PARA_TO_RETURN(pqueue, -1);
        CHECK_NULL_PARA_TO_RETURN(pqueue->pq, -1);
        //判断入参非空
        CHECK_NULL_PARA_TO_RETURN(pdest, -1);
        pthread_mutex_lock(&(pqueue->lock));
        pnode = pqueue_pop(pqueue->pq);  //弹出消息并排序
        pthread_mutex_unlock(&(pqueue->lock));
        CHECK_NULL_PARA_TO_RETURN(pnode, -1);
        CHECK_NULL_PARA_TO_RETURN(pnode->msgobj, -1);
        CHECK_NOTPLUS_PARA_TO_RETURN(pnode->len, -1);

        memcpy(pdest->msgobj, pnode->msgobj, pnode->len);
        pdest->pri = pnode->pri;
        pdest->len = pnode->len;
        free(pnode->msgobj);
        free(pnode);
    } while (0);

    return nRet;
}

/**
*********************************************************************
* @brief： 清空消息队列，释放资源
* @param[in]：
               pqueue 消息队列
* @param[out]：
               none
* @return： none
*********************************************************************
*/
void msg_queue_deinit(PQUEUE_T *pqueue)
{
    if ((NULL == pqueue) || (NULL == pqueue->pq))
    {
        return;
    }
    msg_queue_clear(pqueue);  //清空消息队列
    pthread_mutex_destroy(&(pqueue->lock));
    pqueue_free(pqueue->pq);
    free(pqueue);

    pqueue = NULL;  // deinit后，s_pqueue必须设为NULL,以保证再次init能成功
}

/**
*********************************************************************
* @brief： 清空消息队列
* @param[in]：
               pqueue 消息队列
* @param[out]：
               none
* @return： none
*********************************************************************
*/
void msg_queue_clear(PQUEUE_T *pqueue)
{
    if ((NULL == pqueue) || (NULL == pqueue->pq))
    {
        return;
    }
    pthread_mutex_lock(&(pqueue->lock));
    msg_queue_remove_all(pqueue);
    pthread_mutex_unlock(&(pqueue->lock));
}
/**
*********************************************************************
* @brief： 将消息放入队列中，并排序
* @param[in]：
               pqueue 消息队列
* @param[out]：
               pri   消息优先级
               pmsg  消息指针
               len   消息长度
* @return： 0 成功
            <0 失败
*********************************************************************
*/
sint32 msg_queue_push(PQUEUE_T *pqueue, sint32 pri, void *pmsg, sint32 len)
{
    sint32            nRet  = -1;
    MSG_QUEUE_NODE_T *pnode = malloc(sizeof(MSG_QUEUE_NODE_T));
    pnode->pri              = pri;
    pnode->len              = len;
    pnode->msgobj           = malloc(sizeof(void *));
    memcpy(pnode->msgobj, pmsg, len);
    nRet = msg_queue_obj_push(pqueue, pnode);

    free(pnode->msgobj);
    free(pnode);
    return nRet;
}

/**
*********************************************************************
* @brief： 弹出消息队列中最高优先级的消息，并重新排列
* @param[in]：
               pqueue 消息队列
* @param[out]：
               pri   消息优先级
               pmsg  消息指针
               len   消息长度
* @return： 0 成功
            <0 失败
*********************************************************************
*/
sint32 msg_queue_pop(PQUEUE_T *pqueue, sint32 *pri, void *pmsg, sint32 *len)
{
    sint32            nRet  = -1;
    MSG_QUEUE_NODE_T *pnode = malloc(sizeof(MSG_QUEUE_NODE_T));
    pnode->msgobj           = malloc(sizeof(void *));
    nRet                    = msg_queue_obj_pop(pqueue, pnode);
    memcpy(pmsg, pnode->msgobj, pnode->len);
    *pri = pnode->pri;
    *len = pnode->len;
    free(pnode->msgobj);
    free(pnode);
    return nRet;
}
