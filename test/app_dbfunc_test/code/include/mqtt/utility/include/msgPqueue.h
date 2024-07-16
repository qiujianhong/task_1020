/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 优先级队列头文件
* @date： 2019-11-15
*********************************************************************
*/
#ifndef __MSG_PQUEUE_H__
#define __MSG_PQUEUE_H__
#include <pthread.h>

#include "pqueue.h"
#include "baseType.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _msg_queue_node
{
	uint32       pri;
	void         *msgobj;
    sint32       len;
	size_t       pos;
} MSG_QUEUE_NODE_T;

typedef struct  _msg_pqueue{
	pqueue_t        *pq;
    pthread_mutex_t lock;
	uint32          maxsize;
}PQUEUE_T;

#define MSG_PQUEUE_MAX_SIZE 1000

/* @brief:初始化优先级队列  */
PQUEUE_T* msg_queue_init(void);
PQUEUE_T *msg_queue_init_by_max_count(sint32 maxCount);

sint32 msg_queue_push(PQUEUE_T *pqueue, sint32 pri, void *pmsg, sint32 len);
sint32 msg_queue_pop(PQUEUE_T *pqueue,sint32 *pri,void* pmsg,sint32 *len);

/* @brief:将消息放入优先级队列中  */
sint32 msg_queue_obj_push(PQUEUE_T *pqueue,MSG_QUEUE_NODE_T *psrc);

/* @brief:弹出优先级最高的消息  */
sint32 msg_queue_obj_pop(PQUEUE_T *pqueue,MSG_QUEUE_NODE_T *pdest);

/* @brief:优先级队列非初始化  */
void msg_queue_deinit(PQUEUE_T *pqueue);

/* @brief:清空队列  */
void msg_queue_clear(PQUEUE_T *pqueue);

#ifdef __cplusplus
}
#endif

#endif /* __MSG_PQUEUE_H__ */