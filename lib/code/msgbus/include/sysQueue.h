/**
*********************************************************************
* @copyright Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved.
* @file    sysQueue.h
* @brief   IPC���нӿ�
* @note    ��
* @warning ��
* @date    2018/12/13
*********************************************************************
*/

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief    IPC���о��            */
typedef HANDLE    HQUEUE;

/**
 *********************************************************************
 * @brief       ����IPC����
 * @param[in]   szName        : ��������
 * @param[in]   nQueSize    : ����������ߴ�
 * @param[in]   nMaxBlock    : ����д��ߴ�
 * @return      ���о��
 * @note        ʹ��ʾ��
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
HQUEUE sys_queue_create(const char* szName, uint32 nQueSize, uint32 nMaxBlock);

/**
 *********************************************************************
 * @brief       ����IPC����
 * @param[in]   h            : ���о��
 * @return      none
 * @note        ʹ��ʾ��
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
void sys_queue_destroy(HQUEUE h);

/**
 *********************************************************************
 * @brief       �򿪷����IPC����
 * @param[in]   szName        : ��������
 * @return      ���о��
 * @note        ʹ��ʾ��
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
HQUEUE sys_queue_open(const char* szName);

/**
 *********************************************************************
 * @brief       �����Ƿ��Ѵ�
 * @param[in]   szName        : ��������
 * @return      TRUE-�����Ѵ�;FALSE-���йر�
 * @note        ʹ��ʾ��
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
BOOLEAN sys_queue_isopen(HQUEUE);

/**
 *********************************************************************
 * @brief       �Ӷ��ж�����
 * @param[in]   h            : ���о��
 * @param[in]   pData        : ���ݻ�����
 * @param[in]   len            : ����������
 * @param[in]   nWaittime    : �ȴ�ʱ��
 * @return      ���յ����ݳ��� 
 * @note        ʹ��ʾ��
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
int32 sys_queue_read(HQUEUE h, void *pData, int32 len, int32 nWaittime);

/**
 *********************************************************************
 * @brief       �����д����
 * @param[in]   h            : ���о��
 * @param[in]   pData        : ���ݻ�����
 * @param[in]   len            : ���ݳ���
 * @return      TRUE-�ɹ�;FALSE-ʧ��
 * @note        ʹ��ʾ��
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
BOOLEAN sys_queue_write(HQUEUE h, void *pData, int32 len);

/**
 *********************************************************************
 * @brief       ����ģ���ʼ��
 * @param[in]   none
 * @return      TRUE-�ɹ�;FALSE-ʧ��
 * @note        ʹ��ʾ��
 * @code
 * @endcode 
 * @warning     
 *********************************************************************
 */
BOOLEAN sys_queue_init(void);

/**
 *********************************************************************
 * @brief       ����ģ���˳�
 * @param[in]   none
 * @return      none
 * @note        ʹ��ʾ��
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
