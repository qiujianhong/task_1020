/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块 远方控制
* @date：    2020-4-10
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "audio.h"

#include "appctrl_comm.h"
#include "appctrl_remote.h"
#include "framecomm.h"

char        *g_TurnName[] = {"一轮", "二轮", "三轮", "四轮", "五轮", "六轮", "七轮", "八轮"};

REMOTE_INFO_T g_RemoteInfo;
extern uint8 get_remote_state(uint8 turn, uint8 *type);
extern void set_remote_state_invalid(uint8 turn);
extern void set_remote_state_valid(uint8 turn, uint8 type);

/**********************************************************************
* @name      : appctrl_remote_init
* @brief     ：遥控初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
void appctrl_remote_init()
{
    int ret = 0;
    char buf[4096] = {0};
    uint32 len = 0;
    uint8 i = 0;
    char *stepname[] = {"等待", "开始", "告警", "限电"};
    memset(&g_RemoteInfo, 0, sizeof(REMOTE_INFO_T));

    ret = appctrl_read_ctrl(CTRL_REMOTE_TYPE, (uint8*)buf, 4096, &len);
    if(0 == ret && sizeof(g_RemoteInfo) == len)
    {
        memcpy(&g_RemoteInfo, buf, len);
        memset(buf, 0, 4096);
        len = 0;
        for(i = 0; i < OOP_MAX_TURN * 2; i++)
        {
            len += sprintf(buf + len, "轮次%d  %s, 告警剩余时间 %02d:%02d, 限电剩余时间%02d:%02d\n",
                           i + 1, 
                           g_RemoteInfo.bSwitchStep[i] <= SWITCH_OPEN_STEP ? stepname[g_RemoteInfo.bSwitchStep[i]] : "无效",
                           g_RemoteInfo.wOpenBrakeDelay[i]/60, g_RemoteInfo.wOpenBrakeDelay[i]%60,
                           g_RemoteInfo.wLimitEcTime[i]/60, g_RemoteInfo.wLimitEcTime[i]%60);
        }
        CTRL_FMT_DEBUG("重启之前遥控中间数据:\n%s", buf);
    }
    else
    {
        CTRL_FMT_DEBUG("文件中没有合法遥控数据 %d, 数据长度 %d\n", ret, len);
    }

#if defined PRODUCT_ZCU_1 || defined PRODUCT_SEU
    g_RemoteInfo.turnNO[0] = 0;
    g_RemoteInfo.turnNO[1] = 1;
    g_RemoteInfo.turnNO[2] = 2;
    g_RemoteInfo.turnNO[3] = 3;
    g_RemoteInfo.turnNO[4] = 4;
    g_RemoteInfo.turnNO[5] = 5;
    g_RemoteInfo.turnNO[6] = 6;
    g_RemoteInfo.turnNO[7] = 7;
    
    g_RemoteInfo.turnNO[8] = 0;
    g_RemoteInfo.turnNO[9] = 1;
    g_RemoteInfo.turnNO[10] = 2;
    g_RemoteInfo.turnNO[11] = 3;
    g_RemoteInfo.turnNO[12] = 4;
    g_RemoteInfo.turnNO[13] = 5;
    g_RemoteInfo.turnNO[14] = 6;
    g_RemoteInfo.turnNO[15] = 7;

    g_RemoteInfo.switchtype[0] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[1] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[2] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[3] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[4] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[5] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[6] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[7] = SWITCH_TYPE_OPEN;

    g_RemoteInfo.switchtype[8] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[9] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[10] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[11] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[12] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[13] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[14] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[15] = SWITCH_TYPE_CLOSE;
#else
    g_RemoteInfo.turnNO[0] = 0;
    g_RemoteInfo.turnNO[1] = 1;
    g_RemoteInfo.turnNO[2] = 2;
    g_RemoteInfo.turnNO[3] = 3;
    g_RemoteInfo.turnNO[4] = 0;
    g_RemoteInfo.turnNO[5] = 1;
    g_RemoteInfo.turnNO[6] = 2;
    g_RemoteInfo.turnNO[7] = 3;
    
    g_RemoteInfo.turnNO[8] = 4;
    g_RemoteInfo.turnNO[9] = 5;
    g_RemoteInfo.turnNO[10] = 6;
    g_RemoteInfo.turnNO[11] = 7;
    g_RemoteInfo.turnNO[12] = 4;
    g_RemoteInfo.turnNO[13] = 5;
    g_RemoteInfo.turnNO[14] = 6;
    g_RemoteInfo.turnNO[15] = 7;

    g_RemoteInfo.switchtype[0] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[1] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[2] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[3] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[4] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[5] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[6] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[7] = SWITCH_TYPE_CLOSE;

    g_RemoteInfo.switchtype[8] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[9] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[10] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[11] = SWITCH_TYPE_OPEN;
    g_RemoteInfo.switchtype[12] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[13] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[14] = SWITCH_TYPE_CLOSE;
    g_RemoteInfo.switchtype[15] = SWITCH_TYPE_CLOSE;
#endif

    for(i = 0; i < OOP_MAX_TURN * 2; i++)
    {
        g_RemoteInfo.oad[i] = 0xF2050201 + i;
    }
}

/**********************************************************************
* @name      : appctrl_twomin_power
* @brief     ：根据主站下发的遥控参数，判断是否补充两分钟后功率
* @param[in] ：int cid                    数据中心cid
               uint8 nomax               最大序号
               CTRL_SUBGROUP_T *ptGroups  总加组配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
void appctrl_twomin_power(int cid, uint8 nomax, CTRL_SUBGROUP_T *ptSubGroups)
{
    uint8 t, g;
    
    for(t = 0; t < nomax; t++)    //-判断是否补充两分钟后功率-
    {
        if(0 != g_RemoteInfo.bTwoMin[t])
        {
            g_RemoteInfo.bfgFirstRtrl[t] = 1;
            g_RemoteInfo.bTwoMin[t]--;
        }

        if(0 == g_RemoteInfo.bTwoMin[t] && 1 == g_RemoteInfo.bfgFirstRtrl[t])
        {
            for(g = 0; g < OOP_MAX_TG; g++)
            {
                g_RemoteInfo.TwoMinPower[t][g] = ptSubGroups[g].actPower;
                //CTRL_FMT_DEBUG("Remote trip switch 2 mins sub gruop %d power is %lld\n", g + 1, ptSubGroups[g].actPower);
            }
            /* 补充遥控事件 2min总加组有功功率 */
            CTRL_FMT_DEBUG("远程遥控: 继电器%d-%08X, 补充2min后总加组有功功率\n", t + 1, g_RemoteInfo.oad[t]);
            CheckERC_3115(cid, (OOP_OAD_U)g_RemoteInfo.oad[t], 0, g_RemoteInfo.actPower[t], g_RemoteInfo.TwoMinPower[t], 2);

            g_RemoteInfo.bfgFirstRtrl[t] = 0;
        }
    }
}

/**********************************************************************
* @name      : appctrl_remote_passday
* @brief     ：过日自动恢复允许合闸状态
* @param[in] ：uint8 nomax   最大序号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
void appctrl_remote_passday(uint8 nomax)
{
    uint8 i;
    //-过日自动恢复允许合闸状态-
    CTRL_FMT_DEBUG("过日取消遥控告警和拉闸状态\n");
    for(i = 0; i < nomax; i++)
    {
        if(SWITCH_TYPE_OPEN == g_RemoteInfo.switchtype[i])
        {
            if(TURN_ON == appctrl_get_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[i], 0))
            {    //-是遥控告警状态，则取消告警状态-
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[i], 0, TURN_OFF);
            }

            if(TURN_ON == appctrl_get_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[i], 0))
            {    //-是遥控拉闸状态，则取消拉闸状态-
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[i], 0, TURN_OFF);
            }
        }
        else
        {
            appctrl_set_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[i], 0, TURN_OFF);
        }
        
        g_RemoteInfo.bSwitchStep[i] = SWITCH_IDLE_STEP;    //-遥控进程步初始化-
        g_RemoteInfo.bLimitForever[i] = FALSE;
    }
}

/**********************************************************************
* @name      : appctrl_get_RemotePara
* @brief     ：获取遥控参数
* @param[in] ：uint8 no                    继电器单元-1
               uint8 nomax                最大继电器数量
               CTRL_REMOTE_ARG_T *pArg    远方控制参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
int appctrl_get_RemotePara(uint8 no, uint8 nomax, CTRL_REMOTE_ARG_T *pArg)
{
    uint8 i = 0;
    uint8 bFirst = FALSE;
    uint8 remoteType = 0;
    char *typename[3] = {"跳闸", "允许合闸", "直接合闸"};

    bFirst = get_remote_state(no + 1, &remoteType);
    if(0 == remoteType || remoteType > 3)
    {
        return -1;
    }

    else if(no >= nomax)
    {
        //-判断要执行的轮次是否对应正确-
        return -2;
    }

    if(TRUE == bFirst)//-主站有新遥控命令-
    {
        set_remote_state_invalid(no+1);

        CTRL_FMT_DEBUG("远程控制: 继电器%d, %s\n", no + 1, typename[remoteType - 1]);
        if(1 == remoteType)
        {
            for(i = 0; i < pArg->trip.nNum; i++)
            {
                if(no + 1 == pArg->trip.trip[i].nRelayIdx.nIndex)
                {
                    break;
                }
            }
            if(i == pArg->trip.nNum)
            {
                return -3;
            }
            g_RemoteInfo.btripNo[no] = i;
            g_RemoteInfo.wOpenBrakeDelay[no] = pArg->trip.trip[i].nWarnDelay;    //-获取拉闸延时时间(告警时间)-
            g_RemoteInfo.wOpenBrakeDelay[no] *= 60;                            //分钟转换为秒
            g_RemoteInfo.wLimitEcTime[no] = pArg->trip.trip[i].nTripTime;     //-获取计划限电时间-
            g_RemoteInfo.wLimitEcTime[no] *= 60;                                //分钟转换为秒
            g_RemoteInfo.bLimitForever[no] = (0 == pArg->trip.trip[i].nTripTime) ? TRUE : FALSE;
        }
        g_RemoteInfo.bSwitchStep[no] = SWITCH_START_STEP;                     //-置遥控预处理进程步-
    }

    return 0;
}

/**********************************************************************
* @name      : appctrl_remote_deal
* @brief     ：按获取的参数判断延时告警（遥控跳闸）
* @param[in] ：int cid                     数据中心cid
               uint8 no                   序号
               uint8 nomax                最大轮次数
               CTRL_REMOTE_ARG_T *pArg    远方控制参数
               CTRL_SUBGROUP_T *ptGroups  总加组

* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
void appctrl_remote_deal(int cid, uint8 no, uint8 nomax, CTRL_REMOTE_ARG_T *pArg, CTRL_SUBGROUP_T *ptGroups)
{
    uint8 index, j, g;
    uint8 remoteType = 0;

    //-判断要执行的轮次是否对应正确-
    if(no >= nomax || SWITCH_TYPE_CLOSE == g_RemoteInfo.switchtype[no])
    {
        return;
    }

    index = g_RemoteInfo.btripNo[no];
    get_remote_state(no + 1, &remoteType);

#if 0 /* 研究代码 发现这个没有用处 先不用 */
    if (SWITCH_START_STEP != g_RemoteInfo.bSwitchStep[turn])
    {
        if(0 != g_RemoteInfo.wOpenBrakeDelay[turn])
        {
            /* 在wSwitchDelay规定的时间内每20秒进行声光画面告警提示 */
            if (SWITCH_OPEN_ALARM_STEP == g_RemoteInfo.bSwitchStep[turn] && 
                g_RemoteInfo.wOpenBrakeDelay[turn] <= (((pArg->trip.trip[no].nTripTime >> 8)-1)*60))
            {    //-拉闸延时显示递减1-
                g_RemoteInfo.wAlarmCnt[turn] = g_RemoteInfo.wOpenBrakeDelay[turn];    //-声光画面告警时间-
                if (0 == g_RemoteInfo.wAlarmCnt[turn] % 20)
                {
                    /* 告警 */
                    appctrl_alarm_data(CTRL_REMOTE_TYPE, OOP_MAX_TG, 1, EMAUDIO_INDEX_15, appctrl_remote_guishow);
                }
            }
        }
    }
#endif

    switch(g_RemoteInfo.bSwitchStep[no])
    {
        case SWITCH_IDLE_STEP:
        {
            break;
        }
        case SWITCH_START_STEP:
        {
            if(1 == remoteType)
            {    //-拉闸命令-
                if (g_RemoteInfo.wOpenBrakeDelay[no] == 0)
                {
                    //-填写遥控显示信息结构-
                    appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);
                    appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0, TURN_ON);
                    /* 告警 */
                    appctrl_alarm_data(CTRL_REMOTE_TYPE, 0, 3, EMAUDIO_INDEX_15, appctrl_remote_guishow);
                    //-填写遥控跳闸记录-
                    for(g = 0; g < OOP_MAX_TG; g++)
                    {
                        g_RemoteInfo.actPower[no][g] = ptGroups[g].actPower;
                        g_RemoteInfo.TwoMinPower[no][g] = 0;
                    }
                    g_RemoteInfo.oad[no] = (uint32)pArg->trip.trip[index].nRelayIdx.value;
                    CheckERC_3115(cid, (OOP_OAD_U)g_RemoteInfo.oad[no], 0, g_RemoteInfo.actPower[no], g_RemoteInfo.TwoMinPower[no], 1);

                    g_RemoteInfo.bTwoMin[no] = 120;    //-第一次跳闸后开始计时，等待两分钟后补充功率-
                    g_RemoteInfo.wOpenBrakeDelay[no] = 0;

                    g_RemoteInfo.bSwitchStep[no] =  SWITCH_OPEN_STEP;    //-直接置拉闸进程步- 
                    CTRL_FMT_DEBUG("远程遥控: 继电器%d-0x%08X, 无告警时间, 拉闸动作!\n", g_RemoteInfo.turnNO[no] + 1, g_RemoteInfo.oad[no]);

                    /* 合闸继电器 */
                    CTRL_FMT_DEBUG("关联合闸继电器%d分闸!\n", g_RemoteInfo.turnNO[no] + 1);
                    set_remote_state_valid(g_RemoteInfo.turnNO[no] + 1 + OOP_MAX_TRIP / 2, 1);
                }
                else
                {
                    //-填写遥控显示信息结构-
                    CTRL_FMT_DEBUG("远程遥控: 继电器%d, 拉闸告警!\n", g_RemoteInfo.turnNO[no] + 1);
                    appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[no], 0, TURN_ON);

                    //-首次声光画面告警-
                    appctrl_alarm_data(CTRL_REMOTE_TYPE, 0, 3, EMAUDIO_INDEX_15, appctrl_remote_guishow);

                    g_RemoteInfo.bSwitchStep[no] =  SWITCH_ALARM_STEP;  //-置拉闸延时处理进程-
                }
            }
            else
            {    //-合闸命令-
                g_RemoteInfo.wOpenBrakeDelay[no]=0x0;
                CTRL_FMT_DEBUG("远程遥控: 继电器%d, 取消告警, 合闸操作!\n", g_RemoteInfo.turnNO[no] + 1);
                if(TURN_ON == appctrl_get_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0))
                {
                    /* 合闸操作 */
                    for(g = 0; g < OOP_MAX_TG; g++)
                    {
                        g_RemoteInfo.actPower[no][g] = ptGroups[g].actPower;
                        g_RemoteInfo.TwoMinPower[no][g] = 0;
                    }
                    g_RemoteInfo.bTwoMin[no] = 0;  /* 只有跳闸有2min后功率 */
                    g_RemoteInfo.bfgFirstRtrl[no] = 0;
                    CheckERC_3115(cid, (OOP_OAD_U)g_RemoteInfo.oad[no], 1, g_RemoteInfo.actPower[no], g_RemoteInfo.TwoMinPower[no], 1);
                }
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);

                //从老代码保持一致 不知道为什么要这样操作 合闸在appctrl_control_state也能生效
                BOOL bClose = TRUE;
                for(j = 0; j < OOP_MAX_TG; j++)
                {
                    if(TURN_OFF == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], j)
                       || TURN_OFF == appctrl_get_switch_status(CTRL_MONTH_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], j)
                       || TURN_OFF == appctrl_get_switch_status(CTRL_BUY_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], j))
                    {
                        bClose = FALSE;
                        break;
                    }
                }
                if(bClose == TRUE)
                {
                    /* 告警 */
                    appctrl_alarm_data(CTRL_REMOTE_TYPE, 0, 3, EMAUDIO_INDEX_16, appctrl_remote_guishow);
                }
                g_RemoteInfo.bSwitchStep[no] = SWITCH_IDLE_STEP;  //-置初始化进程步-
            }
            break;
        }
        case SWITCH_ALARM_STEP:
        {
            if(g_RemoteInfo.wOpenBrakeDelay[no] == 0)
            {    //-填写遥控显示信息结构-
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0, TURN_ON);

                /* 告警 */
                appctrl_alarm_data(CTRL_REMOTE_TYPE, 0, 3, EMAUDIO_INDEX_15, appctrl_remote_guishow);

                /* 同路数的合闸继电器需要取消 */
                appctrl_set_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);
                
                //-填写遥控跳闸记录-
                for(g = 0; g < OOP_MAX_TG; g++)
                {
                    g_RemoteInfo.actPower[no][g] = ptGroups[g].actPower;
                    g_RemoteInfo.TwoMinPower[no][g] = 0;
                }
                g_RemoteInfo.oad[no] = (uint32)pArg->trip.trip[index].nRelayIdx.value;

                CheckERC_3115(cid, (OOP_OAD_U)g_RemoteInfo.oad[no], 0, g_RemoteInfo.actPower[no], g_RemoteInfo.TwoMinPower[no], 1);

                g_RemoteInfo.bTwoMin[no] = 120;  //-第一次跳闸后开始计时，等待两分钟后补充功率-
                g_RemoteInfo.bSwitchStep[no] =  SWITCH_OPEN_STEP;  //-直接置拉闸进程步-

                CTRL_FMT_DEBUG("远程遥控: 继电器%d-0x%08X, 拉闸动作!\n", g_RemoteInfo.turnNO[no] + 1, g_RemoteInfo.oad[no]);
                /* 合闸继电器 */
                CTRL_FMT_DEBUG("关联合闸继电器%d分闸!\n", g_RemoteInfo.turnNO[no] + 1);
                set_remote_state_valid(g_RemoteInfo.turnNO[no] + 1 + OOP_MAX_TRIP / 2, 1);
            }
            else
            {
                g_RemoteInfo.wOpenBrakeDelay[no]--;
#if defined PRODUCT_SEU || defined PRODUCT_SEU_1
                if(0 == g_RemoteInfo.wOpenBrakeDelay[no] % 60 && g_RemoteInfo.wOpenBrakeDelay[no] > 0)
                {
                    appctrl_alarm_data(CTRL_REMOTE_TYPE, 0, 3, EMAUDIO_INDEX_15, appctrl_remote_guishow);
                }
#endif
            }
            break;
        }
        case SWITCH_OPEN_STEP:
        {
            if(0 == g_RemoteInfo.wLimitEcTime[no] && FALSE == g_RemoteInfo.bLimitForever[no])
            {
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);
                g_RemoteInfo.bSwitchStep[no] =  SWITCH_IDLE_STEP;   //-置初始化进程步-
                CTRL_FMT_DEBUG("远程遥控: 继电器%d, 限电时间结束, 允许合闸!\n", g_RemoteInfo.turnNO[no] + 1);
                //CheckERC_3115(cid, (OOP_OAD_U)g_RemoteInfo.oad[no], 1, g_RemoteInfo.actPower[no], g_RemoteInfo.TwoMinPower[no], 1);
            }
            else
            {
                if(0 != g_RemoteInfo.wLimitEcTime[no])
                {
                    g_RemoteInfo.wLimitEcTime[no]--;
                }
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0, TURN_ON);
            }
            break;
        }
        default:
            break;
    }
}

/**********************************************************************
* @name      : appctrl_remote_close_deal
* @brief     ：遥控合闸操作
* @param[in] ：int cid                     数据中心cid
               uint8 no                   序号
               uint8 nomax                最大轮次数
               CTRL_REMOTE_ARG_T *pArg    远方控制参数
               CTRL_SUBGROUP_T *ptGroups  总加组

* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
void appctrl_remote_close_deal(int cid, uint8 no, uint8 nomax, CTRL_REMOTE_ARG_T *pArg, CTRL_SUBGROUP_T *ptGroups)
{
    uint8 remoteType = 0;
    uint8 g = 0;
    
    //-判断要执行的轮次是否对应正确-
    if(no >= nomax || SWITCH_TYPE_OPEN == g_RemoteInfo.switchtype[no])
    {
        return;
    }

    get_remote_state(no + 1, &remoteType);

    switch(g_RemoteInfo.bSwitchStep[no])
    {
        case SWITCH_IDLE_STEP:
        {
            break;
        }
        case SWITCH_START_STEP:
        {
            if(3 == remoteType && 
               TURN_OFF == appctrl_get_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0))
            {   
                /* 直接合闸 */
                g_RemoteInfo.wOpenBrakeDelay[no]=0x0;
                CTRL_FMT_DEBUG("远程遥控: 合闸继电器%d, 直接合闸操作!\n", g_RemoteInfo.turnNO[no] + 1);
                appctrl_set_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0, TURN_ON);

                for(g = 0; g < OOP_MAX_TG; g++)
                {
                    g_RemoteInfo.actPower[no][g] = ptGroups[g].actPower;
                    g_RemoteInfo.TwoMinPower[no][g] = 0;
                }
                /* 3115事件 需要修改 */
                CheckERC_3115(cid, (OOP_OAD_U)g_RemoteInfo.oad[no], 2, g_RemoteInfo.actPower[no], 
                               g_RemoteInfo.TwoMinPower[no], 1);

                /* 告警 */
                appctrl_alarm_data(CTRL_CLOSE_TYPE, OOP_MAX_TG, 3, EMAUDIO_INDEX_25, appctrl_remote_guishow);
            }
            else if(1 == remoteType)
            {    
                //-取消合闸-
                CTRL_FMT_DEBUG("远程遥控: 合闸继电器%d, 取消直接合闸!\n", g_RemoteInfo.turnNO[no] + 1);
                appctrl_set_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[no], 0, TURN_OFF);
            }
            g_RemoteInfo.bSwitchStep[no] = SWITCH_IDLE_STEP;
            break;
        }
        default:
            g_RemoteInfo.bSwitchStep[no] = SWITCH_IDLE_STEP;
            break;
    }

}

/**********************************************************************
* @name      : appctrl_remote_ctrl
* @brief     ：根据主站下发的遥控参数，判断是立即拉闸还是有延时，并维护拉闸控制的时间
* @param[in] ：int cid          数据中心cid
               uint8 bDayPass  是否过日
               uint8 nomax     最大序号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-11
* @Update    :
**********************************************************************/
void appctrl_remote_ctrl(int cid, uint8 bDayPass, uint8 nomax)
{
    uint8 i;

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    /* 判断是否补充两分钟后功率 */
    appctrl_twomin_power(cid, nomax, ptSubGroups);

    /* 过日处理 (国网测试不需要过日清除) */
#ifndef AREA_DEFAULT
    if(TRUE == bDayPass)
    {
        appctrl_remote_passday(nomax);
    }
#endif
    for (i = 0; i < nomax; i++)
    {
        if(0 != appctrl_get_RemotePara(i, nomax, &(ptCtrlArg->tRemote)))
        {    //-读取参数失败-
            g_RemoteInfo.bSwitchStep[i] = SWITCH_IDLE_STEP;
            if(SWITCH_TYPE_OPEN == g_RemoteInfo.switchtype[i])
            {
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, g_RemoteInfo.turnNO[i], 0, TURN_OFF);
                appctrl_set_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[i], 0, TURN_OFF);
            }
            else
            {
                appctrl_set_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, g_RemoteInfo.turnNO[i], 0, TURN_OFF);
            }
            continue;
        }
        else
        {    //-延时处理-
            appctrl_remote_deal(cid, i, nomax, &(ptCtrlArg->tRemote), ptSubGroups);
#ifndef PRODUCT_SEU
            appctrl_remote_close_deal(cid, i, nomax, &(ptCtrlArg->tRemote), ptSubGroups);
#endif
        }
    }
}

/**********************************************************************
* @name      : appctrl_remote_status
* @brief     ：远程控制告警和跳闸输出状态汇总
* @param[in] ：uint8 turnmax   最大轮次数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-21
* @Update    : 2020-5-20
**********************************************************************/
void appctrl_remote_status(uint8 turnmax)
{
    uint8 t = 0;

#ifdef PRODUCT_ZCU_1
    OOP_BITSTR16_T   alarmstatus;  //告警状态
    OOP_BITSTR16_T   orderstatus; //命令状态
#else
    OOP_BITSTR8_T    alarmstatus;  //告警状态
    OOP_BITSTR8_T    orderstatus; //命令状态
#endif

    orderstatus.bitValue = 0;
    for(t = 0; t < turnmax; t++)
    {
        if(TURN_ON == appctrl_get_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, t, 0))
        {
            orderstatus.bitValue |= (0x01 << t);
        }
    }
    for(t = 0; t < turnmax; t++)
    {
        if(TURN_ON == appctrl_get_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, t, 0))
        {
            orderstatus.bitValue |= (0x01 << (t + turnmax));
        }
    }

    alarmstatus.bitValue = 0;
    for(t = 0; t < turnmax; t++)
    {
        if(TURN_ON == appctrl_get_switch_status(CTRL_REMOTE_TYPE, CTRL_ALARM_STORE, t, 0))
        {
            alarmstatus.bitValue |= (0x01 << t);
        }
    }
    for(t = 0; t < turnmax; t++)
    {
        if(TURN_ON == appctrl_get_switch_status(CTRL_CLOSE_TYPE, CTRL_ALARM_STORE, t, 0))
        {
            alarmstatus.bitValue |= (0x01 << (t + turnmax));
        }
    }
    
    appctrl_set_remote_status(alarmstatus.bitValue, orderstatus.bitValue);
}

/**********************************************************************
* @name      : appctrl_remote_turnshow1
* @brief     ：构造专变轮次显示内容
* @param[in] ：uint8 turn       轮次
               uint16 buflen   缓存大小
* @param[out]：char* pBuf       显示内容
* @return    ：缓存占用大小
* @Create    : 王津剑
* @Date      ：2022-8-1
* @Update    :
**********************************************************************/
uint16 appctrl_remote_turnshow(uint8 turn, char* pBuf, uint16 buflen, uint32 *timeshow, int pinx)
{
    uint16 offset = 0;

    /* 左边轮次处理 */
    if(pinx <= 160)
    {
        offset = sprintf(pBuf, "\n %s: ", g_TurnName[turn]);
        if(FALSE == appctrl_dev_input(turn + 1))
        {
            offset += sprintf(pBuf + offset, "--------");
        }
        else
        {
            if(SWITCH_ALARM_STEP == g_RemoteInfo.bSwitchStep[turn])
            {
                //告警阶段 剩余时间是偶数, 显示合闸状态, 剩余时间是奇数, 不显示状态
                if(0 == g_RemoteInfo.wOpenBrakeDelay[turn] % 2)
                {
                    offset += sprintf(pBuf + offset, "合闸      ");
                }
                else
                {
                    offset += sprintf(pBuf + offset, "          ");
                }
            }
            else if(SWITCH_OPEN_STEP == g_RemoteInfo.bSwitchStep[turn])
            {
                //跳闸阶段 剩余时间是偶数, 显示跳闸状态, 剩余时间是奇数, 不显示状态；永久限电一直显示
                if(TRUE == g_RemoteInfo.bLimitForever[turn] || 0 == g_RemoteInfo.wLimitEcTime[turn] % 2)
                {
                    offset += sprintf(pBuf + offset, "跳闸      ");
                }
                else
                {
                    offset += sprintf(pBuf + offset, "          ");
                }
            }
            else
            {
                if(TURN_ON == appctrl_get_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, turn, 0))
                {
                    offset += sprintf(pBuf + offset, "直接合闸  ");
                }
                else
                {
                    offset += sprintf(pBuf + offset, "允许合闸  ");
                }
            }
        }
    }
    else
    {
        if (turn%2 == 0)
            offset = sprintf(pBuf, "\n %s:", g_TurnName[turn]);
        else
            offset = sprintf(pBuf, " %s:", g_TurnName[turn]);
        if(FALSE == appctrl_dev_input(turn + 1))
        {
            offset += sprintf(pBuf + offset, "---");
        }
        else
        {
            if(SWITCH_ALARM_STEP == g_RemoteInfo.bSwitchStep[turn])
            {
                //告警阶段 剩余时间是偶数, 显示合闸状态, 剩余时间是奇数, 不显示状态
                if(0 == g_RemoteInfo.wOpenBrakeDelay[turn] % 2)
                {
                    offset += sprintf(pBuf + offset, "合闸 ");
                }
                else
                {
                    offset += sprintf(pBuf + offset, "     ");
                }
            }
            else if(SWITCH_OPEN_STEP == g_RemoteInfo.bSwitchStep[turn])
            {
                //跳闸阶段 剩余时间是偶数, 显示跳闸状态, 剩余时间是奇数, 不显示状态；永久限电一直显示
                if(TRUE == g_RemoteInfo.bLimitForever[turn] || 0 == g_RemoteInfo.wLimitEcTime[turn] % 2)
                {
                    offset += sprintf(pBuf + offset, "跳闸 ");
                }
                else
                {
                    offset += sprintf(pBuf + offset, "     ");
                }
            }
            else
            {
                if(TURN_ON == appctrl_get_switch_status(CTRL_CLOSE_TYPE, CTRL_STATUS_STORE, turn, 0))
                {
                    offset += sprintf(pBuf + offset, "合闸 ");
                }
                else
                {
                    offset += sprintf(pBuf + offset, "合闸 ");
                }
            }
        }
    }

    if(SWITCH_ALARM_STEP == g_RemoteInfo.bSwitchStep[turn])
    {
        *timeshow += g_RemoteInfo.wOpenBrakeDelay[turn];
        offset += sprintf(pBuf + offset, "00:%02d:%02d", g_RemoteInfo.wOpenBrakeDelay[turn]/60, 
                          g_RemoteInfo.wOpenBrakeDelay[turn]%60);
    }
    else if(SWITCH_OPEN_STEP == g_RemoteInfo.bSwitchStep[turn])
    {
        *timeshow += g_RemoteInfo.wLimitEcTime[turn];
        offset += sprintf(pBuf + offset, "%02d:%02d:%02d", g_RemoteInfo.wLimitEcTime[turn]/3600, 
                          (g_RemoteInfo.wLimitEcTime[turn]%3600)/60, 
                          (g_RemoteInfo.wLimitEcTime[turn]%3600)%60);
    }
    else
    {
        offset += sprintf(pBuf + offset, "00:00:00");
    }
    
    return offset;
}

/**********************************************************************
* @name      : appctrl_remote_guishow
* @brief     ：远程控制告警和跳闸输出桌面显示
* @param[in] ：uint8 ctrltype  控制类型
               uint8 group     总加组
               uint8 turnmax   最大轮次数
               uint16 buflen   缓存大小
* @param[out]：uint8* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-4-30
* @Update    :
**********************************************************************/
uint8 appctrl_remote_guishow(uint8 ctrltype, uint8 group, uint8 turnmax, uint8* pBuf, uint16 buflen, uint16* msglen)
{
    /* 发送interface */
#ifdef PRODUCT_SEU
    uint8 i= 0;
    uint8 devStatus = 0;
    IN_CTRL_INFO_T ctrlInfo;
    memset(&ctrlInfo, 0, sizeof(IN_CTRL_INFO_T));

    ctrlInfo.group = 0;
    ctrlInfo.type = CTRL_REMOTE_TYPE;
    ctrlInfo.times = 60;

    sprintf(ctrlInfo.head, "%s", "远程控制");
    for(i = 0; i < 4; i++)
    {
        ctrlInfo.turn[ctrlInfo.num].no = i + 1;
        
        devStatus = appctrl_get_switch_status(CTRL_REMOTE_TYPE, CTRL_STATUS_STORE, i, 0);
        if(SWITCH_ALARM_STEP == g_RemoteInfo.bSwitchStep[i])
        {
            ctrlInfo.turn[ctrlInfo.num].curstatus = 0;
            ctrlInfo.turn[ctrlInfo.num].futurestatus = 1;
            ctrlInfo.turn[ctrlInfo.num].balarm = 1;
            ctrlInfo.turn[ctrlInfo.num].hour = 0;
            ctrlInfo.turn[ctrlInfo.num].min = g_RemoteInfo.wOpenBrakeDelay[i]/60;
            ctrlInfo.turn[ctrlInfo.num].sec = g_RemoteInfo.wOpenBrakeDelay[i]%60;
        }
        else if(SWITCH_OPEN_STEP == g_RemoteInfo.bSwitchStep[i])
        {
            ctrlInfo.turn[ctrlInfo.num].curstatus = 1;
            ctrlInfo.turn[ctrlInfo.num].futurestatus = TRUE ==  g_RemoteInfo.bLimitForever[i] ? 1 : 0;
            ctrlInfo.turn[ctrlInfo.num].balarm = g_RemoteInfo.wLimitEcTime[i] > 0 ? 1 : 0;
            ctrlInfo.turn[ctrlInfo.num].hour = g_RemoteInfo.wLimitEcTime[i]/3600;
            ctrlInfo.turn[ctrlInfo.num].min = (g_RemoteInfo.wLimitEcTime[i]%3600)/60;
            ctrlInfo.turn[ctrlInfo.num].sec = (g_RemoteInfo.wLimitEcTime[i]%3600)%60;
        }
        else
        {
            ctrlInfo.turn[ctrlInfo.num].curstatus = 1 - devStatus;
            ctrlInfo.turn[ctrlInfo.num].futurestatus = 1 - devStatus;
            ctrlInfo.turn[ctrlInfo.num].balarm = 0;
        }
        ctrlInfo.num += 1;
    }
    appctrl_show_ifhint(&ctrlInfo);
    return FALSE;
#else
    uint16 offset = 0;
    char   buf[450] = {0};
    uint8  t = 0;
    uint32 time_show = 0;

#ifdef PRODUCT_ZCU_1
    offset = sprintf(buf, "\n                %s", "遥控状态");
    for(t = 0; t < turnmax; t++)
    {
        offset += appctrl_remote_turnshow(t, buf + offset, 450 - offset, &time_show, 224);
    }
#else
    offset = sprintf(buf, "        %s", "遥控状态");
    for(t = 0; t < turnmax; t++)
    {
        offset += appctrl_remote_turnshow(t, buf + offset, 450 - offset, &time_show, 160);
    }
#endif
    if(offset > buflen)
    {
        CTRL_FMT_DEBUG("Gui show buf %d too small %d\n", buflen, offset);
        return FALSE;
    }
    *msglen = offset;
    memcpy(pBuf, buf, offset);
    return 0 == time_show ? FALSE : TRUE;
#endif
}

/**********************************************************************
* @name      : appctrl_remote_write
* @brief     ：远程控制中间状态写入私有数据
* @param[in] ：uint8 bDay  过日
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-5-7
* @Update    :
**********************************************************************/
void appctrl_remote_write(uint8 bDay)
{
    int ret = 0;
    static uint8 s_RemoteInfo[sizeof(REMOTE_INFO_T)] = {0};

    if(0 == memcmp(s_RemoteInfo, &g_RemoteInfo, sizeof(REMOTE_INFO_T)) && FALSE == bDay)
    {
        return;
    }
    memcpy(s_RemoteInfo, &g_RemoteInfo, sizeof(REMOTE_INFO_T));
    ret = appctrl_write_ctrl(CTRL_REMOTE_TYPE, (uint8 *)&g_RemoteInfo, sizeof(REMOTE_INFO_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("保存遥控中间数据错误, %d\n", ret);
    }
}

