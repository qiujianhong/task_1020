/*
*********************************************************************
* Copyright(C) 2024 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   负荷控制APP数据初始化
* @date：    2024-06-05
* @history： 
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

//保电状态
PROTECT_CTRL_T g_oopGuard;

//遥控状态
REMOTE_CTRL_T  g_oopRemoteCtrl;

//催费告警
FEE_CTRL_T     g_oopFeeCtrl;


#if DESC("保电状态变更" ,1)
/**********************************************************************
* @name      : ipaddr_set_lock_init
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void guard_set_lock_init(void)
{
    /* 初始化锁 */
    memset(&g_oopGuard, 0, sizeof(g_oopGuard));
    pthread_mutex_init(&g_oopGuard.mutex, NULL);
}

/**********************************************************************
* @name      : get_guard_state
* @brief     ：获取保电状态
* @param[in] ：NULL
* @param[out]：
* @return    ：0 - 初始化状态
               1 - 保电
               2 - 解除 
**********************************************************************/
uint8 get_guard_state(void)
{
    uint8 guardState = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_oopGuard.mutex);

    guardState = g_oopGuard.guardState;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_oopGuard.mutex);

    return guardState;
}

/**********************************************************************
* @name      : set_guard_state
* @brief     ：设置保电状态
* @param[in] ：guardState - 保电状态 0 - 无状态 1- 投入 2 - 解除, 3-解除自动保电
* @param[out]：NULL
* @return    ：0          - 成功
              !0          - 错误 
**********************************************************************/
int set_guard_state(uint8 guardState)
{
    if (guardState > 3)
    {
        return ERR_O_RANGE;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_oopGuard.mutex);

    g_oopGuard.guardState = guardState;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_oopGuard.mutex);

    return ERR_OK;
}

#endif

#if DESC("遥控状态变更" ,1)
/**********************************************************************
* @name      : ipaddr_set_lock_init
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void remote_set_lock_init(void)
{
    /* 初始化锁 */
    memset(&g_oopRemoteCtrl, 0, sizeof(g_oopRemoteCtrl));
    pthread_mutex_init(&g_oopRemoteCtrl.mutex, NULL);
}

/**********************************************************************
* @name      : get_remote_state
* @brief     ：获取遥控状态
* @param[in] ：turn  - 轮次
* @param[out]：type  - 遥控种类 (1-跳闸, 2-允许合闸, 3-直接合闸)
* @return    ：state - 遥控状态 (0-无效，1-有效)
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

    /* 使用互斥量进行加锁 */
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

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_oopRemoteCtrl.mutex);

    return rmState;
}

/**********************************************************************
* @name      : set_remote_state_invalid
* @brief     ：清除遥控状态
* @param[in] ：turn   - 轮次
* @param[out]：NULL
* @return    ：
**********************************************************************/
void set_remote_state_invalid(uint8 turn)
{
    if ((turn > OOP_MAX_TRIP) || (turn < 1))
    {
        PRTL_FMT_DEBUG("set_remote_state_invalid err turn[%d]\n", turn);
        return;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_oopRemoteCtrl.mutex);

    g_oopRemoteCtrl.state[turn - 1] = 0;

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_oopRemoteCtrl.mutex);

    return;
}

/**********************************************************************
* @name      : set_remote_state_valid
* @brief     ：设置遥控状态 
* @param[in] ：turn   - 轮次
* @param[out]：NULL
* @return    ：
**********************************************************************/
void set_remote_state_valid(uint8 turn, uint8 type, TELECON_TRIP *pTrip, TELECON_CLOSE *pClose)
{
    if ((turn > OOP_MAX_TRIP) || (turn < 1))
    {
        PRTL_FMT_DEBUG("set_remote_state_valid err turn[%d]\n", turn);
        return;
    }

    /* 使用互斥量进行加锁 */
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
    
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_oopRemoteCtrl.mutex);

    return;
}

#endif

#if DESC("催费告警" ,1)
/**********************************************************************
* @name      : fee_set_lock_init
* @brief     ：
* @param[out]：
* @return    ：
**********************************************************************/
void fee_set_lock_init(void)
{
    /* 初始化锁 */
    memset(&g_oopFeeCtrl, 0, sizeof(g_oopFeeCtrl));
    pthread_mutex_init(&g_oopFeeCtrl.mutex, NULL);
}

/**********************************************************************
* @name      : get_guard_state
* @brief     ：获取状态
* @param[in] ：NULL
* @param[out]：
* @return    ：0 - 初始化状态
               1 - 投入
               2 - 解除 
**********************************************************************/
uint8 get_fee_state(OOP_CALLFEE_T *pFee)
{
    uint8 state = 0;

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_oopFeeCtrl.mutex);

    state = g_oopFeeCtrl.status;
    memcpy(pFee, &(g_oopFeeCtrl.param), sizeof(OOP_CALLFEE_T));

    /* 访问后清空 */
    g_oopFeeCtrl.status = 0;
    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_oopFeeCtrl.mutex);

    return state;
}

/**********************************************************************
* @name      : set_fee_state
* @brief     ：设置状态
* @param[in] ：state( 0 - 无状态 1- 投入 2 - 解除 )
* @param[out]：NULL
* @return    ：0          - 成功
              !0          - 错误 
**********************************************************************/
int set_fee_state(uint8 state, OOP_CALLFEE_T *pFee)
{
    if (state > 2)
    {
        return ERR_O_RANGE;
    }

    /* 使用互斥量进行加锁 */
    pthread_mutex_lock(&g_oopFeeCtrl.mutex);

    g_oopFeeCtrl.status = state;
    if(pFee)
    {
        memcpy(&(g_oopFeeCtrl.param), pFee, sizeof(OOP_CALLFEE_T));
    }

    /* 使用互斥量进行解锁 */
    pthread_mutex_unlock(&g_oopFeeCtrl.mutex);

    return ERR_OK;
}

#endif


