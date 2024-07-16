/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ���ݻ������ͷ�ļ�
* @date�� 2019-11-9
*********************************************************************
*/

#ifndef __CACHE_H__
#define __CACHE_H__
#include "qlibc.h"
#include <pthread.h>
#include "baseType.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* @brief:��������ö�ٶ���  */
typedef enum _cache_type
{
    TTL = 0
} CACHE_TYPE_T;

/* @brief:����ڵ�ö�ٶ���  */
typedef struct _cache_node
{
    __time_t timestamp; //ʱ���
    uint32 objLen;      //cache�����С
} CACHE_NODE_T;

/* @brief:���湹��ö�ٶ���  */
typedef struct _cache
{
    CACHE_TYPE_T type;    //cache����
    uint32 maxCount;      //���cache����
    uint32 defaultTtl;    //Ĭ�ϵ�TTLֵ ����cache��Чʱ��
    pthread_mutex_t lock; //cache�߳���
    qhashtbl_t *tbl;      //cache��ϣ��
} CACHE_T;

/* @brief:����һ���»���  */
CACHE_T *new_cache(sint32 maxCount, sint32 defaultTtl);

/* @brief:ɾ������  */
void delete_cache(CACHE_T *cache);

/* @brief:���û���ڵ�  */
sint32 set_cache_node(CACHE_T *cache, sint8 *key, void *obj, size_t objSize);

/* @brief:��ȡ����ڵ�  */
sint32 get_cache_node(CACHE_T *cache, sint8 *key, void **obj, size_t *objSize, sint32 clone);

/* @brief:�ͷŻ���ڵ�  */
void free_cache_node(void *obj);

/* @brief:�Ƴ�����ڵ�  */
sint32 remove_cache_node(CACHE_T *cache, sint8 *key);

/* @brief:��ȡ���ɻ���ռ�  */
sint32 get_free_cache_space(CACHE_T *cache);

#ifdef __cplusplus
}
#endif

#endif
