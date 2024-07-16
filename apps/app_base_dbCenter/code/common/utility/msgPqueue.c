/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief��������Ϣ���ȼ�����
* @date�� 2019-11-15
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
* @brief�� ȡ�ýڵ����ȼ���������ʼ��ʱ�趨
* @param[in]��node �ڵ�ָ��
* @param[out]��none
* @return�� none
*********************************************************************
*/
static pqueue_pri_t msg_pqueue_get_pri(void *pnode)
{
    return ((MSG_QUEUE_NODE_T *)pnode)->pri;
}

/**
*********************************************************************
* @brief�� �趨�ڵ�����ȼ���������ʼ��ʱ���趨
* @param[in]��
               nPri ���ȼ�
* @param[out]��
               pnode
* @return�� none
*********************************************************************
*/
static void msg_pqueue_set_pri(void *pnode, pqueue_pri_t nPri)
{
    ((MSG_QUEUE_NODE_T *)pnode)->pri = nPri;
}

/**
*********************************************************************
* @brief�� ���ȼ��ߵ��жϺ�������ʼ��ʱ���趨(Ӱ������е��Ⱥ�)
* @param[in]��
               curr ��ǰ�ڵ�����ȼ�
               next ��һ�ڵ�����ȼ�
* @param[out]��
            none
* @return�� none
*********************************************************************
*/
static int msg_pqueue_cmp_pri(pqueue_pri_t nNext, pqueue_pri_t nCurr)
{
    return (nNext > nCurr);
}

/**
*********************************************************************
* @brief�� ȡ��λ�ú�������ʼ��ʱ���趨
* @param[in]��pnode
* @param[out]��
            none
* @return�� nodeλ��
*********************************************************************
*/
static size_t msg_pqueue_get_pos(void *pnode)
{
    return ((MSG_QUEUE_NODE_T *)pnode)->pos;
}

/**
*********************************************************************
* @brief�� �趨λ�ú�������ʼ��ʱ���趨
* @param[in]�� npos λ��
* @param[out]��
            pnode
* @return�� none
*********************************************************************
*/
static void msg_pqueue_set_pos(void *pnode, size_t npos)
{
    ((MSG_QUEUE_NODE_T *)pnode)->pos = npos;
}

/**
*********************************************************************
* @brief�� �Ƴ�������Ϣ
* @param[in]��
               PQUEUE_T
* @param[out]��none
* @return�� 0 �ɹ�
            <0 ʧ��
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
* @brief�� ��Ϣ���г�ʼ��
* @param[in]��
               void
* @param[out]��
               none
* @return�� PQUEUE_T *
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
* @brief�� ��Ϣ���г�ʼ��
* @param[in]��
               �����������
* @param[out]��
               none
* @return�� none
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
* @brief�� ����Ϣ��������У�������
* @param[in]��
               pqueue ��Ϣ����   
               psrc ������е���Ϣ�ṹ��
* @param[out]��
               none
* @return�� 0 �ɹ�
            <0 ʧ��
*********************************************************************
*/
sint32 msg_queue_obj_push(PQUEUE_T *pqueue, MSG_QUEUE_NODE_T *psrc)
{
    MSG_QUEUE_NODE_T *pnode = NULL;
    int               nRet  = 0;
    do
    {
        //����Ƿ��Ѿ���ʼ��,�������
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
            //�����쳣
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
* @brief�� ������Ϣ������������ȼ�����Ϣ������������
* @param[in]��
               pqueue ��Ϣ����
* @param[out]��
               pdest �������е���Ϣ�ṹ��
* @return�� 0 �ɹ�
            <0 ʧ��
*********************************************************************
*/
sint32 msg_queue_obj_pop(PQUEUE_T *pqueue, MSG_QUEUE_NODE_T *pdest)
{
    MSG_QUEUE_NODE_T *pnode = NULL;
    sint32            nRet  = 0;
    do
    {
        //�ж��Ƿ��Ѿ���ʼ��
        CHECK_NULL_PARA_TO_RETURN(pqueue, -1);
        CHECK_NULL_PARA_TO_RETURN(pqueue->pq, -1);
        //�ж���ηǿ�
        CHECK_NULL_PARA_TO_RETURN(pdest, -1);
        pthread_mutex_lock(&(pqueue->lock));
        pnode = pqueue_pop(pqueue->pq);  //������Ϣ������
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
* @brief�� �����Ϣ���У��ͷ���Դ
* @param[in]��
               pqueue ��Ϣ����
* @param[out]��
               none
* @return�� none
*********************************************************************
*/
void msg_queue_deinit(PQUEUE_T *pqueue)
{
    if ((NULL == pqueue) || (NULL == pqueue->pq))
    {
        return;
    }
    msg_queue_clear(pqueue);  //�����Ϣ����
    pthread_mutex_destroy(&(pqueue->lock));
    pqueue_free(pqueue->pq);
    free(pqueue);

    pqueue = NULL;  // deinit��s_pqueue������ΪNULL,�Ա�֤�ٴ�init�ܳɹ�
}

/**
*********************************************************************
* @brief�� �����Ϣ����
* @param[in]��
               pqueue ��Ϣ����
* @param[out]��
               none
* @return�� none
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
* @brief�� ����Ϣ��������У�������
* @param[in]��
               pqueue ��Ϣ����
* @param[out]��
               pri   ��Ϣ���ȼ�
               pmsg  ��Ϣָ��
               len   ��Ϣ����
* @return�� 0 �ɹ�
            <0 ʧ��
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
* @brief�� ������Ϣ������������ȼ�����Ϣ������������
* @param[in]��
               pqueue ��Ϣ����
* @param[out]��
               pri   ��Ϣ���ȼ�
               pmsg  ��Ϣָ��
               len   ��Ϣ����
* @return�� 0 �ɹ�
            <0 ʧ��
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
