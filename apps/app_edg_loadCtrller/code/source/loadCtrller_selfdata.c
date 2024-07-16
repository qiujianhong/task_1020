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
#include "hal.h"
#include "devinfo.h"
#include "iniparser.h"
#include "appmsg_split.h"
#include "framecomm.h"
#include "storage.h"

#include "loadCtrller_data.h"
#include "loadCtrller_selfdata.h"

//����״̬
PROTECT_CTRL_T g_oopGuard;

//ң��״̬
REMOTE_CTRL_T  g_oopRemoteCtrl;

//�߷Ѹ澯
FEE_CTRL_T     g_oopFeeCtrl;


#if DESC("����״̬���" ,1)
/**********************************************************************
* @name      : ipaddr_set_lock_init
* @brief     ��
* @param[out]��
* @return    ��
**********************************************************************/
void guard_set_lock_init(void)
{
    /* ��ʼ���� */
    memset(&g_oopGuard, 0, sizeof(g_oopGuard));
    pthread_mutex_init(&g_oopGuard.mutex, NULL);
}

/**********************************************************************
* @name      : get_guard_state
* @brief     ����ȡ����״̬
* @param[in] ��NULL
* @param[out]��
* @return    ��0 - ��ʼ��״̬
               1 - ����
               2 - ��� 
**********************************************************************/
uint8 get_guard_state(void)
{
    uint8 guardState = 0;

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_oopGuard.mutex);

    guardState = g_oopGuard.guardState;

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_oopGuard.mutex);

    return guardState;
}

/**********************************************************************
* @name      : set_guard_state
* @brief     �����ñ���״̬
* @param[in] ��guardState - ����״̬ 0 - ��״̬ 1- Ͷ�� 2 - ���, 3-����Զ�����
* @param[out]��NULL
* @return    ��0          - �ɹ�
              !0          - ���� 
**********************************************************************/
int set_guard_state(uint8 guardState)
{
    if (guardState > 3)
    {
        return ERR_O_RANGE;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_oopGuard.mutex);

    g_oopGuard.guardState = guardState;

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_oopGuard.mutex);

    return ERR_OK;
}

#endif

#if DESC("ң��״̬���" ,1)
/**********************************************************************
* @name      : ipaddr_set_lock_init
* @brief     ��
* @param[out]��
* @return    ��
**********************************************************************/
void remote_set_lock_init(void)
{
    /* ��ʼ���� */
    memset(&g_oopRemoteCtrl, 0, sizeof(g_oopRemoteCtrl));
    pthread_mutex_init(&g_oopRemoteCtrl.mutex, NULL);
}

/**********************************************************************
* @name      : get_remote_state
* @brief     ����ȡң��״̬
* @param[in] ��turn  - �ִ�
* @param[out]��type  - ң������ (1-��բ, 2-�����բ, 3-ֱ�Ӻ�բ)
* @return    ��state - ң��״̬ (0-��Ч��1-��Ч)
**********************************************************************/
uint8 get_remote_state(uint8 turn, uint8 *type, TELECON_TRIP *pTrip, TELECON_CLOSE *pClose)
{
    uint8 rmState = 0;

    if (type == NULL)
    {
        PRTL_FMT_DEBUG("get_remote_state err type[NULL]\n");
        return 0;
    }

    if ((turn > OOP_MAX_TRIP) || (turn < 1))
    {
        return 0;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_oopRemoteCtrl.mutex);

    rmState = g_oopRemoteCtrl.state[turn - 1];
    *type = g_oopRemoteCtrl.type[turn - 1];

    if(pTrip)
    {
        memcpy(pTrip, &(g_oopRemoteCtrl.trip[turn - 1]), sizeof(TELECON_TRIP));
    }
    if(pClose)
    {
        memcpy(pClose, &(g_oopRemoteCtrl.close[turn - 1]), sizeof(TELECON_CLOSE));
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_oopRemoteCtrl.mutex);

    return rmState;
}

/**********************************************************************
* @name      : set_remote_state_invalid
* @brief     �����ң��״̬
* @param[in] ��turn   - �ִ�
* @param[out]��NULL
* @return    ��
**********************************************************************/
void set_remote_state_invalid(uint8 turn)
{
    if ((turn > OOP_MAX_TRIP) || (turn < 1))
    {
        PRTL_FMT_DEBUG("set_remote_state_invalid err turn[%d]\n", turn);
        return;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_oopRemoteCtrl.mutex);

    g_oopRemoteCtrl.state[turn - 1] = 0;

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_oopRemoteCtrl.mutex);

    return;
}

/**********************************************************************
* @name      : set_remote_state_valid
* @brief     ������ң��״̬ 
* @param[in] ��turn   - �ִ�
* @param[out]��NULL
* @return    ��
**********************************************************************/
void set_remote_state_valid(uint8 turn, uint8 type, TELECON_TRIP *pTrip, TELECON_CLOSE *pClose)
{
    if ((turn > OOP_MAX_TRIP) || (turn < 1))
    {
        PRTL_FMT_DEBUG("set_remote_state_valid err turn[%d]\n", turn);
        return;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_oopRemoteCtrl.mutex);

    g_oopRemoteCtrl.state[turn - 1] = 1;
    g_oopRemoteCtrl.type[turn - 1] = type;
    if(pTrip)
    {
        memcpy(&(g_oopRemoteCtrl.trip[turn - 1]), pTrip, sizeof(TELECON_TRIP));
    }
    if(pClose)
    {
        memcpy(&(g_oopRemoteCtrl.close[turn - 1]), pClose, sizeof(TELECON_CLOSE));
    }
    
    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_oopRemoteCtrl.mutex);

    return;
}

#endif

#if DESC("�߷Ѹ澯" ,1)
/**********************************************************************
* @name      : fee_set_lock_init
* @brief     ��
* @param[out]��
* @return    ��
**********************************************************************/
void fee_set_lock_init(void)
{
    /* ��ʼ���� */
    memset(&g_oopFeeCtrl, 0, sizeof(g_oopFeeCtrl));
    pthread_mutex_init(&g_oopFeeCtrl.mutex, NULL);
}

/**********************************************************************
* @name      : get_guard_state
* @brief     ����ȡ״̬
* @param[in] ��NULL
* @param[out]��
* @return    ��0 - ��ʼ��״̬
               1 - Ͷ��
               2 - ��� 
**********************************************************************/
uint8 get_fee_state(OOP_CALLFEE_T *pFee)
{
    uint8 state = 0;

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_oopFeeCtrl.mutex);

    state = g_oopFeeCtrl.status;
    memcpy(pFee, &(g_oopFeeCtrl.param), sizeof(OOP_CALLFEE_T));

    /* ���ʺ���� */
    g_oopFeeCtrl.status = 0;
    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_oopFeeCtrl.mutex);

    return state;
}

/**********************************************************************
* @name      : set_fee_state
* @brief     ������״̬
* @param[in] ��state( 0 - ��״̬ 1- Ͷ�� 2 - ��� )
* @param[out]��NULL
* @return    ��0          - �ɹ�
              !0          - ���� 
**********************************************************************/
int set_fee_state(uint8 state, OOP_CALLFEE_T *pFee)
{
    if (state > 2)
    {
        return ERR_O_RANGE;
    }

    /* ʹ�û��������м��� */
    pthread_mutex_lock(&g_oopFeeCtrl.mutex);

    g_oopFeeCtrl.status = state;
    if(pFee)
    {
        memcpy(&(g_oopFeeCtrl.param), pFee, sizeof(OOP_CALLFEE_T));
    }

    /* ʹ�û��������н��� */
    pthread_mutex_unlock(&g_oopFeeCtrl.mutex);

    return ERR_OK;
}

#endif


