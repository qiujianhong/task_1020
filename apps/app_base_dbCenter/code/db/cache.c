/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 数据缓存机制实现代码
* @date： 2019-11-9
*********************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "baseType.h"

#include "cache.h"
#include "qlibc.h"

/**
*********************************************************************
* @brief： 获取时间戳
* @param[in]： 
               none 
* @param[out]：
               none
* @return： 时间戳
*********************************************************************
*/
static __time_t get_timestamp(void)
{
    struct timeval stamp;
    gettimeofday(&stamp, NULL);
    return stamp.tv_sec;
}

/**
*********************************************************************
* @brief： 自动删除缓存
* @param[in]： 
               cache 句柄
* @param[out]：
               none
* @return： 返回释放资源的个数
*********************************************************************
*/
static sint32 auto_delete_cache(CACHE_T *cache)
{
    sint32 freeNum = 0;
    qhashtbl_obj_t obj = {0};
    qhashtbl_t *tbl = cache->tbl;

    memset((void *)&obj, 0, sizeof(obj)); // must be cleared before cal
    __time_t now = get_timestamp();
    while (tbl->getnext(tbl, &obj, true) == true) //遍历哈希表dump data对象
    {
        CACHE_NODE_T *node = obj.data;
        if (now - node->timestamp > cache->defaultTtl) //如果对象生命周期结束则释放对象
        {
            tbl->remove(tbl, obj.name);
            freeNum++;
        }
        free(obj.name); //必须释放哈希表的名称及data指向区域
        free(obj.data);
    }
    return freeNum != 0;
}

/**
*********************************************************************
* @brief： 获取自由缓存空间
* @param[in]： 
               cache       
* @param[out]：
               none
* @return： 
*********************************************************************
*/
static sint32 _get_free_cache_space(CACHE_T *cache)
{
    qhashtbl_t *tbl = cache->tbl;
    return cache->maxCount - tbl->size(tbl);
}

CACHE_T *new_cache(sint32 maxCount, sint32 defaultTtl)
{
    CACHE_T *cache = (CACHE_T *)malloc(sizeof(CACHE_T));
    //使用外部线程锁同步 串行Hash数组的容量最好为最大数量的两倍
    cache->tbl = qhashtbl(maxCount << 1, 0);
    cache->type = TTL;              //设置TTL类型
    cache->maxCount = maxCount;     //设置最大容量
    cache->defaultTtl = defaultTtl; //设置默认TTL值
    if (cache == NULL)
    {
        goto err;
    }

    if (cache->tbl == NULL)
    {
        goto err;
    }

    if (pthread_mutex_init(&cache->lock, NULL) != 0) //初始化线程锁
    {
        goto err;
    }

    return cache;

err:
    if (cache->tbl)
    {
        qhashtbl_free(cache->tbl); //异常必须释放
    }

    if (cache)
    {
        free(cache);
    }

    return NULL;
}

/**
*********************************************************************
* @brief： 删除缓存
* @param[in]： 
               cache       
* @param[out]：
               none
* @return： 
*********************************************************************
*/
void delete_cache(CACHE_T *cache)
{
    qhashtbl_t *tbl = cache->tbl;

    tbl->clear(tbl);                     //清除哈希表
    pthread_mutex_destroy(&cache->lock); //释放线程锁
    free(cache);                         //释放cache本身存储空间
}

sint32 get_free_cache_space(CACHE_T *cache)
{
    sint32 freespace = 0;

    qhashtbl_t *tbl = cache->tbl;
    //获取准确的数量需要使用线程锁
    pthread_mutex_lock(&cache->lock);
    freespace = cache->maxCount - tbl->size(tbl);
    pthread_mutex_unlock(&cache->lock);
    return freespace;
}

/**
*********************************************************************
* @brief： 设置缓存节点
* @param[in]： 
               cache       
               key       
               obj       
               objSize       
* @param[out]：
               none
* @return： 
*********************************************************************
*/
sint32 set_cache_node(CACHE_T *cache, sint8 *key, void *obj, size_t objSize)
{
    sint32 freeSpace = 0;
    CACHE_NODE_T *node = NULL;
    qhashtbl_t *tbl = cache->tbl;

    pthread_mutex_lock(&cache->lock);
    if (_get_free_cache_space(cache) < 1) //判断当前cache剩余数量
    {
        if (auto_delete_cache(cache) > 0) //执行自动删除过程
        {
            freeSpace = 1; //有空闲空间置位freeSpace
        }
    }
    else
    {
        freeSpace = 1; //有空闲空间置位freeSpace
    }

    if (freeSpace)
    {
        node = malloc(sizeof(CACHE_NODE_T) + objSize);
        node->objLen = objSize;
        node->timestamp = get_timestamp();
        memcpy((sint8 *)node + sizeof(CACHE_NODE_T), obj, objSize);
        tbl->put(tbl, key, node, sizeof(CACHE_NODE_T) + objSize);
        free(node);
    }

    pthread_mutex_unlock(&cache->lock);
    //tbl->debug(tbl,stdout);
    return freeSpace;
}

/**
*********************************************************************
* @brief： 获取缓存节点
* @param[in]： 
               cache       
               key       
               obj       
               objSize       
* @param[out]：
               获取节点返回 1 ，未获取返回0
* @return： 
*********************************************************************
*/
sint32 get_cache_node(CACHE_T *cache, sint8 *key, void **obj, size_t *objSize, sint32 clone)
{
    pthread_mutex_lock(&cache->lock);
    qhashtbl_t *tbl = cache->tbl;

    CACHE_NODE_T *node = tbl->get(tbl, key, objSize, 0);
    if (node != NULL)
    {
        node->timestamp = get_timestamp(); //刷新被读取节点的时间戳
        *objSize = node->objLen;
        if (0 != clone) //如果clone为非零则为返回数据分配空间
        {
            *obj = malloc(node->objLen);
            if (*obj != NULL)
            {
                memcpy(*obj, (sint8 *)node + sizeof(CACHE_NODE_T), node->objLen);
            }
        }
        else
        {
            *obj = (sint8 *)node + sizeof(CACHE_NODE_T); //无需cloen返回哈希表内部数据
        }
    }
    else
    {
        *objSize = 0;
        *obj = NULL;
    }
    pthread_mutex_unlock(&cache->lock);

    return *obj == NULL ? 0 : 1;
}

/**
*********************************************************************
* @brief： 释放缓存节点
* @param[in]： 
               obj 
* @param[out]：
               none
* @return： 
*********************************************************************
*/
void free_cache_node(void *obj)
{
    CACHE_NODE_T *node = obj - sizeof(CACHE_NODE_T);
    free(node);
}

/**
*********************************************************************
* @brief： 移除缓存节点
* @param[in]： 
               cache 
               key 
* @param[out]：
               none
* @return： 
*********************************************************************
*/
sint32 remove_cache_node(CACHE_T *cache, sint8 *key)
{
    pthread_mutex_lock(&cache->lock);
    qhashtbl_t *tbl = cache->tbl;
    tbl->remove(tbl, key);
    pthread_mutex_unlock(&cache->lock);

    return 0;
}
