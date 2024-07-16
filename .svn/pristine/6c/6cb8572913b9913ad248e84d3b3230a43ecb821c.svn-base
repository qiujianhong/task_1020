/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 数据缓存机制头文件
* @date： 2019-11-9
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

/* @brief:缓存类型枚举定义  */
typedef enum _cache_type
{
    TTL = 0
} CACHE_TYPE_T;

/* @brief:缓存节点枚举定义  */
typedef struct _cache_node
{
    __time_t timestamp; //时间戳
    uint32 objLen;      //cache对象大小
} CACHE_NODE_T;

/* @brief:缓存构造枚举定义  */
typedef struct _cache
{
    CACHE_TYPE_T type;    //cache类型
    uint32 maxCount;      //最大cache数量
    uint32 defaultTtl;    //默认的TTL值 用于cache有效时间
    pthread_mutex_t lock; //cache线程锁
    qhashtbl_t *tbl;      //cache哈希表
} CACHE_T;

/* @brief:定义一个新缓存  */
CACHE_T *new_cache(sint32 maxCount, sint32 defaultTtl);

/* @brief:删除缓存  */
void delete_cache(CACHE_T *cache);

/* @brief:设置缓存节点  */
sint32 set_cache_node(CACHE_T *cache, sint8 *key, void *obj, size_t objSize);

/* @brief:获取缓存节点  */
sint32 get_cache_node(CACHE_T *cache, sint8 *key, void **obj, size_t *objSize, sint32 clone);

/* @brief:释放缓存节点  */
void free_cache_node(void *obj);

/* @brief:移除缓存节点  */
sint32 remove_cache_node(CACHE_T *cache, sint8 *key);

/* @brief:获取自由缓存空间  */
sint32 get_free_cache_space(CACHE_T *cache);

#ifdef __cplusplus
}
#endif

#endif
