/*
*********************************************************************
* Copyright(C) 2024 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���ɿ���APP���ݳ�ʼ��
* @date��    2024-06-05
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "db_api.h"
#include "framecomm.h"
#include "task_id.h"

#include "loadCtrller_data.h"
#include "loadCtrller_event.h"

int    g_dbEvent = 0;


/**********************************************************************
* @name      : loadctrl_event_init
* @brief     ���¼������ʼ��
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2024-7-1
* @Update    :
**********************************************************************/
int loadctrl_event_init(void *pPublic, void **ppPrivate)
{
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;

    g_dbEvent = db_client_create(DB_LOADCTRLLER_AGENT, DB_LOADCTRLLER_EVENT);
    if(g_dbEvent <= 0)
    {
        printf("db_client_create failed!\n");
        return -1;
    }

    /* �ȴ��������� */
    while(FALSE == ptLCData->hasDB)
    {
        usleep(100000);
    }

    /* �¼���ʼ�� */

    *ppPrivate = NULL;
    return 0;
}

/**********************************************************************
* @name      : loadctrl_event_proc
* @brief     ���¼�����
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : 
* @Date      ��2024-7-1
* @Update    :
**********************************************************************/
int loadctrl_event_proc(void *pArg, void *pPublic, void *pPrivate)
{
    //LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    
    

    
    
    return 1000000;
}

