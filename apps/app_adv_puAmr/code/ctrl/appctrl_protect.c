/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块 保电控制
* @date：    2020-4-11
* @history： 
*********************************************************************
*/
#include "baseCommon.h"

#include "appctrl_comm.h"
#include "appctrl_protect.h"
#include "framecomm.h"


uint8  g_ProetctEffect = FALSE; //主站投入和自动保电时段
uint16 g_FirstTimeLeft = 0;
int    g_NoMasterTimeLeft = -1;

extern uint8 app_check_master_connect();

void appctrl_protect_first()
{
    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    /* 上电自保电时间 */
    g_FirstTimeLeft = ptCtrlArg->tProtect.FMTimer;
}

/**********************************************************************
* @name      : appctrl_protect_status
* @brief     ：保电控制是否生效
* @param[in] ：
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
uint8 appctrl_protect_status()
{
    if(FALSE == g_ProetctEffect && 0 == g_FirstTimeLeft && -1 == g_NoMasterTimeLeft)
    {
        return FALSE;
    }
    return TRUE;
}

/**********************************************************************
* @name      : appctrl_protect_ctrl
* @brief     ：保电控制
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
void appctrl_protect_ctrl()
{
    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计
    CTRL_PROTECT_ARG_T *ptProtect = NULL;   //保电参数
    static uint8     bLastMaster = TRUE;  //上次执行是否有主站
    uint8            bCurMaster = FALSE;   //本次执行是否有主站

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    ptProtect = &(ptCtrlArg->tProtect);

    /* 检测是否无通信 */
    bCurMaster = app_check_master_connect();
    if(FALSE == bCurMaster && TRUE == bLastMaster)
    {
        CTRL_FMT_DEBUG("检测到没有连接主站!\n");
        if(ptProtect->NMTimer > 0)
        {
            g_NoMasterTimeLeft = ptProtect->NMTimer;
            CTRL_FMT_DEBUG("%dmin后开始保电\n", g_NoMasterTimeLeft);
        }
        else
        {
            g_NoMasterTimeLeft = -1;
            CTRL_FMT_DEBUG("无通信不保电!\n");
        }
        bLastMaster = FALSE;
    }
    else if(TRUE == bCurMaster && FALSE == bLastMaster)
    {
        CTRL_FMT_DEBUG("检测到连接主站!\n");
        g_NoMasterTimeLeft = -1;
        bLastMaster = TRUE;
    }

    /* 主站投入保电 主站设置数据中心 */
    if(1 == get_guard_state())
    {
        g_ProetctEffect = TRUE;
        appctrl_set_protect_status(1);
        return;
    }
    else if(2 == get_guard_state())  /* 主站下发保电解除 */
    {
        g_FirstTimeLeft = 0;
        g_NoMasterTimeLeft = -1;
        g_ProetctEffect = FALSE;
        appctrl_set_protect_status(0);
        set_guard_state(0); //设置状态为 0
        return;
    }
    
    if(0 == g_NoMasterTimeLeft || 0 != g_FirstTimeLeft)
    {
        /* 调用接口修改保电状态为自动保电 规约解析写数据中心 */
        appctrl_set_protect_status(2);
        return;
    }

    if(0 != ptProtect->autoprotect.nNum)
    {
        /* 自动保电 */
        uint32           i;
        time_t           now;
        OOP_DATETIME_S_T curTime = {0};
        now = time(NULL);
        DT_Time2DateTimeShort(now, &curTime);
        for(i = 0; i < ptProtect->autoprotect.nNum; i++)
        {
            if(ptProtect->autoprotect.item[i].start <= curTime.hour && curTime.hour < ptProtect->autoprotect.item[i].end)
            {
                /* 调用接口修改保电状态为自动保电 规约解析写数据中心 */
                g_ProetctEffect = TRUE;
                appctrl_set_protect_status(2);
                return;
            }
        }
    }
    
    /* 调用接口修改保电状态为无保电 规约解析写数据中心 */
    appctrl_set_protect_status(0);
    g_ProetctEffect = FALSE;
}

/**********************************************************************
* @name      : appctrl_protect_ctrl_min
* @brief     ：保电控制 分钟调度
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
void appctrl_protect_ctrl_min()
{
    if(0 != g_FirstTimeLeft)
    {
        g_FirstTimeLeft--;
    }

    if(g_NoMasterTimeLeft > 0)
    {
        g_NoMasterTimeLeft--;
    }
}


