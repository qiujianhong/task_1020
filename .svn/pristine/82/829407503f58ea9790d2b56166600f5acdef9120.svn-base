/**
*********************************************************************
* @copyright Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved.
* @file    sysQueue.h
* @brief   IPC队列接口
* @note    无
* @warning 无
* @date    2018/12/13
*********************************************************************
*/

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief    IPC队列句柄            */
typedef HANDLE    HQUEUE;

/**
 *********************************************************************
 * @brief       创建IPC队列
 * @param[in]   szName        : 队列名称
 * @param[in]   nQueSize    : 队列数据域尺寸
 * @param[in]   nMaxBlock    : 最大读写块尺寸
 * @return      队列句柄
 * @note        使用示例
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
HQUEUE sys_queue_create(const char* szName, uint32 nQueSize, uint32 nMaxBlock);

/**
 *********************************************************************
 * @brief       销毁IPC队列
 * @param[in]   h            : 队列句柄
 * @return      none
 * @note        使用示例
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
void sys_queue_destroy(HQUEUE h);

/**
 *********************************************************************
 * @brief       打开服务端IPC队列
 * @param[in]   szName        : 队列名称
 * @return      队列句柄
 * @note        使用示例
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
HQUEUE sys_queue_open(const char* szName);

/**
 *********************************************************************
 * @brief       队列是否已打开
 * @param[in]   szName        : 队列名称
 * @return      TRUE-队列已打开;FALSE-队列关闭
 * @note        使用示例
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
BOOLEAN sys_queue_isopen(HQUEUE);

/**
 *********************************************************************
 * @brief       从队列读数据
 * @param[in]   h            : 队列句柄
 * @param[in]   pData        : 数据缓冲区
 * @param[in]   len            : 缓冲区长度
 * @param[in]   nWaittime    : 等待时长
 * @return      接收到数据长度 
 * @note        使用示例
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
int32 sys_queue_read(HQUEUE h, void *pData, int32 len, int32 nWaittime);

/**
 *********************************************************************
 * @brief       向队列写数据
 * @param[in]   h            : 队列句柄
 * @param[in]   pData        : 数据缓冲区
 * @param[in]   len            : 数据长度
 * @return      TRUE-成功;FALSE-失败
 * @note        使用示例
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
BOOLEAN sys_queue_write(HQUEUE h, void *pData, int32 len);

/**
 *********************************************************************
 * @brief       队列模块初始化
 * @param[in]   none
 * @return      TRUE-成功;FALSE-失败
 * @note        使用示例
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
BOOLEAN sys_queue_init(void);

/**
 *********************************************************************
 * @brief       队列模块退出
 * @param[in]   none
 * @return      none
 * @note        使用示例
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
void sys_queue_exit(void);

#ifdef __cplusplus
}
#endif

#endif // __QUEUE_H__
