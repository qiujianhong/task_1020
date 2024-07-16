/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief��   ��Ϣ�����ӿ�
* @date��    2018/09/10
* @history�� 
*********************************************************************
*/

#ifndef __MSB_APP_H__
#define __MSB_APP_H__

#include "msg.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MQT_NAME_LEN             64           /**< ������Ƴ���            */
#define MQT_DEFAULT_QUELEN         0x2000         /**< ȱʡ��Ϣ���г���        */
#define MQT_DEFAULT_READLEN     4000         /**< ȱʡ��ȡ���泤��        */

/** @brief ����汾��Ϣ    */
typedef union 
{
    uint32             number;
    struct 
    {
        uint32        build      : 8;            /**< Build��                */
        uint32         evolution : 8;             /**< �ݽ��汾��                */
        uint32         expansion : 8;             /**< ��չ�汾��                */
        uint32         major     : 8;             /**< ���汾��                */
    };
} VERSION_T;

/**
 ******************************************************************************
 * @brief    �����ʼ������    
 * @note     ������ƹ���
 *            if( name[0] )
 *            {
 *                '_'    :    -> Base inner thread    exp:"_monitor"
 *                '/'    :    -> Base TCP-Socket        exp:"/db"
 *                '$'    :    -> Base UDP-Socket        exp:"$ppdc:192.168.8.2[9000]"
 *            }
 *            else
 *            {
 *                        -> Base IPC                exp:"smiOS"
 *            }
 *            ��Ϣ���ģ�smiOS:192.168.8.1[8000][8001] -- [TCP�˿�][UDP�˿�]
 ******************************************************************************
 */
typedef struct
{
    char             myName[MQT_NAME_LEN];     /**< ���������                    */
    char            mcName[MQT_NAME_LEN];    /**< ��Ϣ��������                */
    uint32             eventMask;                 /**< �����¼�                    */
    uint32             msgQueLen;                 /**< ��Ϣ���г���(0Ϊȱʡ)        */
    uint32             msgReadLen;             /**< ��Ϣ��ȡ���泤��(0Ϊȱʡ)    */
    VERSION_T         version;                 /**< �汾��                        */
} MQT_PROFILE_T;

/**
 ******************************************************************************
 * @brief         ��ʼ���������
 * @param[in]    pProfile    :    ������ò���
 * @return         �ɹ�����TRUE, ʧ�ܷ���FALSE
 * @note                     
 ******************************************************************************
 */
BOOLEAN app_profile_init(const MQT_PROFILE_T *pProfile);

/**
 ******************************************************************************
 * @brief         ���ɴ�����Ϣ���е��̲߳��Զ�������Ϣ
 * @param[in]    pName             :    ��Ϣ����/�߳�����
 * @param[in]    nQueSize        :    ���л����С(Ϊ0��ʾֻ�ܽ����¼�)
 * @param[in]    pHandler         :    ��Ϣ����ص�����ָ��
 * @param[in]    arg             :    ��Ϣ����ص���������
 * @param[in]    subEventMask    :     ��Ҫ���ĵ��¼�����
 * @return         �ɹ�����TRUE, ʧ�ܷ���FALSE
 * @note         �̳߳�ʱ�̶�Ϊ1Сʱ
 ******************************************************************************
 */
BOOLEAN app_mqt_create(const char *pName, uint32 nQueSize, MSG_HANDLER pHandler, void *arg, uint32 subEventMask);

/**
 ******************************************************************************
 * @brief         ���ɴ�����Ϣ���е��߳�(���ֹ�������Ϣ)
 * @param[in]    pName             :    ��Ϣ����/�߳�����
 * @param[in]    nQueSize        :    ���л����С(Ϊ0��ʾֻ�ܽ����¼�)
 * @param[in]    pHandler         :    ��Ϣ����ص�����ָ��
 * @param[in]    arg             :    ��Ϣ����ص���������
 * @param[in]    subEventMask    :     ��Ҫ���ĵ��¼�����
 * @return         �ɹ�����TRUE, ʧ�ܷ���FALSE
 * @note         �̳߳�ʱ�̶�Ϊ1Сʱ
 ******************************************************************************
 */
BOOLEAN app_mqt_mix_create(const char *pName, uint32 nQueSize, void* (*pHandler)(void*), void *arg, uint32 subEventMask);

/**
 ******************************************************************************
 * @brief         ��ѯ��Ϣ���о��
 * @param[in]    pName        :    ��Ϣ��������
 * @return         �ɹ�������Ϣ���о��, ʧ�ܷ���NULL
 * @note                 
 ******************************************************************************
 */
HMQT app_mqt_find(const char *pName);

/**
 ******************************************************************************
 * @brief         ��ȡ���̵߳ľ��
 * @param[in]     none
 * @return         �ɹ�����ʵ�ʾ��, ʧ�ܷ���NULL
 * @note                 
 ******************************************************************************
 */
HMQT app_mqt_self(void);

/**
 ******************************************************************************
 * @brief         ��ȡ�����̵ľ��
 * @param[in]    none
 * @return         �ɹ�����ʵ�ʾ��, ʧ�ܷ���NULL
 * @note                 
 ******************************************************************************
 */
HMQT app_self(void);

/**
 ******************************************************************************
 * @brief         ��ʼ���пͻ������(��������˳����쳣���᷵��)
 * @param[in]    pHandler    :    ��Ϣ����ص�����
 * @param[in]    arg         :    �ص���������
 * @return         none
 * @note         �ú��������������˳�
 ******************************************************************************
 */
void app_run(MSG_HANDLER pHandler, void *arg);

/**
 ******************************************************************************
 * @brief         �����˳�
 * @param[in]    none
 * @return         none
 * @note                 
 ******************************************************************************
 */
void app_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* __MSB_APP_H__ */
