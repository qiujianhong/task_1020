/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块 配置参数获取
* @date：    2020-4-3
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "sys_cfg.h"
#include "audio.h"

#include "appctrl_comm.h"
#include "appctrl_power.h"
#include "framecomm.h"

POWER_INFO_T g_PowerInfo;
PCSEG_INFO_T g_PCSegData[48]; //-8个功控时段的分布 保险一点翻倍-
uint8        g_bPCSegNo = 0; //-功控时段数量-
uint8        g_PowerTurnStop[OOP_MAX_TG] = {0};  //轮次延时


//该函数处理功率电量等值显示,防止大数值跃屏
void valueDisplayProcess_strUnitPower(int64 energy, char* str, size_t n,uint32 weight,const char* unit)
{
    double val_kWh = energy;

    if (str == NULL || n <= 0)
    {
        return;
    }

    for(uint32 i =1;i<=weight;i++)
    {
        val_kWh = val_kWh *0.1;
    }

    //单位自适应来解决数值过大不能显示全部的问题
    //求位数
    int64 val_autoAdap=(int64)val_kWh;
    uint8 digitsCnt=0;
    while(val_autoAdap)
    {
        val_autoAdap = val_autoAdap/10;
        digitsCnt++;
    }
    if(digitsCnt<=6)
    {
        //val_kWh = energy * 0.0001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fk%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
    else if(digitsCnt>6 && digitsCnt<=9)
    {
        val_kWh = val_kWh*0.001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fM%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
    else
    {
        val_kWh = val_kWh*0.001*0.001;//数据字典中约定内部按缩放10000倍存储电能量
        snprintf(str, n - 1, "%6.4fG%s", val_kWh,unit);
        str[n - 1] = '\0';
    }
}

/**********************************************************************
* @name      : appctrl_get_power_type
* @brief     ：功率控制类型
* @param[in] ：uint8 grp   总加组
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2023-6-2
* @Update    :
**********************************************************************/
uint8 appctrl_get_power_type(uint8 grp)
{
    if(grp > 0 && grp <= OOP_MAX_TG)
    {
        return g_PowerInfo.bPcKind[grp -1];
    }
    else
    {
        return CTRL_NULL_TYPE;
    }
}

/**********************************************************************
* @name      : appctrl_power_init
* @brief     ：功率控制变量初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
void appctrl_power_init()
{
    uint8  i;
    char   buf[4096] = {0};
    uint32 len = 0;
    int    ret = 0;
    char  *ctrlName[] = {"下浮控", "报停控", "厂休控", "时段控"};
    
    memset(&g_PowerInfo, 0, sizeof(POWER_INFO_T));
    ret = lctrl_read_ctrl(CTRL_POWER_TYPE, (uint8*)buf, 4096, &len);
    if(0 == ret && sizeof(g_PowerInfo) == len)
    {
        memcpy(&g_PowerInfo, buf, len);

        memset(buf, 0, 4096);
        len = 0;

        for(i = 0; i < OOP_MAX_TG; i++)
        {
            len += sprintf(buf + len, "总加%d  ", i + 1);
        }
        len += sprintf(buf + len, "\n");
        
        for(i = 0; i < OOP_MAX_TG; i++)
        {
            len += sprintf(buf + len, "%s ", 
                           g_PowerInfo.bPcKind[i] <= CTRL_PERIOD_TYPE ? ctrlName[g_PowerInfo.bPcKind[i]] : "未生效");
        }
        CTRL_FMT_DEBUG("重启前功控生效种类:\n%s\n", buf);
        return;
    }

    CTRL_FMT_DEBUG("文件中没有合法功控数据 %d, 数据长度 %d\n", ret, len);
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        g_PowerInfo.bLastPCtrl[i] = CTRL_NULL_TYPE;
        g_PowerInfo.bCurrentSeg[i] = 0xFF;
    }
}

/**********************************************************************
* @name      : appctrl_down_init
* @brief     ：下浮控初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2023-1-13
* @Update    :
**********************************************************************/
void appctrl_down_init()
{
    uint8 i = 0;
    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    for(i = 0; i < OOP_MAX_TG; i++)
    {
        if(g_PowerInfo.bPcKind[i] == CTRL_DOWN_TYPE)
        {
            ptSubGroups[i].slipPower = g_PowerInfo.wPowerSetValue[i];
            CTRL_FMT_DEBUG("重启前总加组%d, 下浮控功率定值%llu\n", i + 1, g_PowerInfo.wPowerSetValue[i]);
        }
    }
}

/**********************************************************************
* @name      : appctrl_pcclose_break
* @brief     ：根据功控类型清除跳闸状态
* @param[in] ：uint8 pctype    功控类型
               uint8 grp       总加组序号
               uint8 turn      轮次
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-8
* @Update    :
**********************************************************************/
void appctrl_power_SetAlarm(uint8 pctype, uint8 grp, uint8 turn)
{
    if(CTRL_DOWN_TYPE == pctype || CTRL_STOP_TYPE == pctype || CTRL_REST_TYPE == pctype ||
       CTRL_PERIOD_TYPE == pctype)
    {
        appctrl_set_switch_status(pctype, CTRL_ALARM_STORE, turn, grp, TURN_ON);
        appctrl_set_switch_status(pctype, CTRL_STATUS_STORE, turn, grp, TURN_OFF);
        appctrl_set_switch_status(CTRL_POWER_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_ON);
        appctrl_set_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_OFF);

        //-定值告警-
        appctrl_alarm_data(pctype, grp, 3, EMAUDIO_INDEX_17, appctrl_power_guishow);
    }
}

/**********************************************************************
* @name      : appctrl_pcclose_break
* @brief     ：根据功控类型进行功控拉闸告警
* @param[in] ：uint8 pctype    功控类型
               uint8 grp       总加组序号
               uint8 turn      轮次
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_power_OpenAlarm(uint8 pctype, uint8 grp, uint8 turn)
{
    switch(pctype)
    {
        case CTRL_DOWN_TYPE:
            appctrl_set_switch_status(CTRL_DOWN_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_OFF);
            appctrl_set_switch_status(CTRL_DOWN_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_ON);
            
            //-当前功率下浮控拉闸告警-
            CTRL_FMT_DEBUG("总加组%d, 轮次%d, 下浮控拉闸\n", grp + 1, turn + 1);
            appctrl_alarm_data(CTRL_DOWN_TYPE, grp, 3, EMAUDIO_INDEX_22, appctrl_power_guishow);
            break;
        case CTRL_STOP_TYPE:
            appctrl_set_switch_status(CTRL_STOP_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_OFF);
            appctrl_set_switch_status(CTRL_STOP_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_ON);

            //-营业报停控拉闸告警-
            CTRL_FMT_DEBUG("总加组%d, 轮次%d, 报停控拉闸\n", grp + 1, turn + 1);
            appctrl_alarm_data(CTRL_STOP_TYPE, grp, 3, EMAUDIO_INDEX_22, appctrl_power_guishow);
            break;
        case CTRL_REST_TYPE:
            appctrl_set_switch_status(CTRL_REST_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_OFF);
            appctrl_set_switch_status(CTRL_REST_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_ON);

            //-厂休控拉闸告警-
            CTRL_FMT_DEBUG("总加组%d, 轮次%d, 厂休控拉闸\n", grp + 1, turn + 1);
            appctrl_alarm_data(CTRL_REST_TYPE, grp, 3, EMAUDIO_INDEX_22, appctrl_power_guishow);
            break;
        case CTRL_PERIOD_TYPE:
            appctrl_set_switch_status(CTRL_PERIOD_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_OFF);
            appctrl_set_switch_status(CTRL_PERIOD_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_ON);

            //-时段控拉闸告警-
            CTRL_FMT_DEBUG("总加组%d, 轮次%d, 时段控拉闸\n", grp + 1, turn + 1);
            appctrl_alarm_data(CTRL_PERIOD_TYPE, grp, 3, EMAUDIO_INDEX_22, appctrl_power_guishow);
            break;
        default:
            break;
    }
}

/**********************************************************************
* @name      : appctrl_power_close
* @brief     ：根据功控类型清除清除告警状态和跳闸状态
* @param[in] ：uint8 pctype    功控类型
               uint8 grp       总加组序号
               uint8 turn      轮次
               uint8 bReason   原因
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
void appctrl_power_close(uint8 pctype, uint8 grp, uint8 turn, uint8 bReason)
{
    if(grp >= OOP_MAX_TG || turn >= OOP_MAX_TURN)
    {
        return;
    }

    appctrl_pcstop_alarm(pctype, grp, turn); /*-告警状态清除-*/
    appctrl_pcclose_break(pctype, grp, turn); /*-跳闸状态清除-*/

    g_PowerInfo.bFirstInit[grp][turn] = 0;          //-告警延时计数器首次赋初值复位-
    g_PowerInfo.wPowerAlarmDelay[grp][turn] = 0;    //-轮次间延时计数器清零-
    g_PowerInfo.wPcAlarmDelayss[grp][turn] = 0;
    g_PowerInfo.bFirstPassPower[grp][turn] = NOT_PASS_SETVAL;   //-首次超定值标志复位-

    g_PowerInfo.bTwoMin[grp][turn] = 0;     //-2分钟冻结计时复位-
    g_PowerInfo.bfgFirstPrtl[grp][turn]=0;  //-首次补充2分钟后功控记录标志复位-
    g_PowerInfo.wPowerONValue[grp][turn] = 0;
}

/**********************************************************************
* @name      : appctrl_power_status_clear
* @brief     ：根据功控类型清除清除告警状态和跳闸状态
* @param[in] ：uint8 pctype    当前功控类型
               uint8 grp       总加组序号
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-3-18
* @Update    :
**********************************************************************/
void appctrl_power_status_clear(uint8 pctype, uint8 grp)
{
    uint8 type = CTRL_DOWN_TYPE;
    uint8 turn = 0;
    for(type = CTRL_DOWN_TYPE; type <= CTRL_PERIOD_TYPE; type++)
    {
        if(pctype == type)
        {
            continue;
        }
        //合闸，相关变量初始化
        for (turn = 0; turn < OOP_MAX_TURN; turn++)
        {
            appctrl_pcstop_alarm(type, grp, turn); /*-告警状态清除-*/
            appctrl_pcclose_break(type, grp, turn); /*-跳闸状态清除-*/
        }
    }

    /* 非功控清理其他数据 */
    if(pctype > CTRL_PERIOD_TYPE)
    {
        for (turn = 0; turn < OOP_MAX_TURN; turn++)
        {
            g_PowerInfo.bFirstInit[grp][turn] = 0;          //-告警延时计数器首次赋初值复位-
            g_PowerInfo.wPowerAlarmDelay[grp][turn] = 0;    //-轮次间延时计数器清零-
            g_PowerInfo.wPcAlarmDelayss[grp][turn] = 0;
            g_PowerInfo.bFirstPassPower[grp][turn] = NOT_PASS_SETVAL;   //-首次超定值标志复位-

            g_PowerInfo.bTwoMin[grp][turn] = 0;     //-2分钟冻结计时复位-
            g_PowerInfo.bfgFirstPrtl[grp][turn]=0;  //-首次补充2分钟后功控记录标志复位-
            g_PowerInfo.wPowerONValue[grp][turn] = 0;
        }
    }
    if(pctype != CTRL_PERIOD_TYPE)
    {
        g_PowerInfo.bCurrentSeg[grp] = 0xFF;
    }
}

/**********************************************************************
* @name      : appctrl_power_passday
* @brief     ：功控过日处理
* @param[in] ：uint8 tg      总加组序号
               uint8 turnmax 最大轮次数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
int appctrl_power_passday(CTRL_ARG_ALL_T *ptCtrlArg, uint8 group, uint8 turnmax)
{
    uint8 t;//-轮次-
    if(CTRL_DOWN_TYPE == g_PowerInfo.bPcKind[group])
    {
        ptCtrlArg->tDown.bEffect[group] = FALSE; //下浮控制 直接接触
        appctrl_set_down_close(group + 1);
        ptCtrlArg->tDown.starttime[group] = 0;
        g_PowerInfo.tDownStart[group] = 0;
    }
    if (CTRL_NULL_TYPE != g_PowerInfo.bPcKind[group] && CTRL_STOP_TYPE != g_PowerInfo.bPcKind[group])
    {   //-下浮控、时段控和厂休控的合闸,并告警-
        for(t = 0; t < OOP_MAX_TURN; t++)
        {
            appctrl_power_close(g_PowerInfo.bPcKind[group], group, t, OUTOFTIME_CLOSE); //-合闸-
            g_PowerInfo.bFirstInit[group][t] = 0;   //-告警延时计数器首次赋初值复位-
            g_PowerInfo.bTwoMin[group][t] = 0;      //-2分钟冻结计时复位-
            g_PowerInfo.bfgFirstPrtl[group][t]=0;   //-首次补充2分钟后功控记录标志复位-
        }
        return 0;
    }
    return -1;
}

/**********************************************************************
* @name      : appctrl_cal_power
* @brief     ：记录总加组1min功率60次，根据滑差功率和功率定值的大小进行控制
* @param[in] ：int cid         数据中心id
               uint8 grp     总加组序号
               CTRL_SUBGROUP_T *ptSubGroup 总加组配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
void appctrl_cal_power(int cid, uint8 grp, CTRL_SUBGROUP_T *ptSubGroup)
{
    uint8  i;

    /* 滑差计算准备 存储功率 */
    if(ptSubGroup->slipnum < SUBGROUP_SLIP_MAX)
    {
        ptSubGroup->actPowerList[ptSubGroup->slipnum] = ptSubGroup->actPower;
        ptSubGroup->slipnum += 1;
    }
    else
    {
        for(i = 0; i < SUBGROUP_SLIP_MAX - 1; i++)
        {
            ptSubGroup->actPowerList[i] = ptSubGroup->actPowerList[i + 1];
        }
        ptSubGroup->actPowerList[SUBGROUP_SLIP_MAX - 1] = ptSubGroup->actPower;
        ptSubGroup->slipnum = SUBGROUP_SLIP_MAX;

    }

    CTRL_FMT_TRACE("SubGroup %d, current active power %lld, slip num %u\n", grp + 1, ptSubGroup->actPower, ptSubGroup->slipnum);
}

/**********************************************************************
* @name      : appctrl_cal_power_clear
* @brief     ：记录总加组1min功率清空
* @param[in] ：uint8 grp     总加组序号
               CTRL_SUBGROUP_T *ptSubGroup 总加组配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2023-3-3
* @Update    :
**********************************************************************/
void appctrl_cal_power_clear(uint8 grp, CTRL_SUBGROUP_T *ptSubGroup)
{
    ptSubGroup->slipnum = 0;
    memset(ptSubGroup->actPowerList, 0, SUBGROUP_SLIP_MAX * sizeof(int64));
    CTRL_FMT_DEBUG("总加组%d变更，清空每分钟功率缓存\n", grp + 1);
}

/**********************************************************************
* @name      : appctrl_cal_power_2min
* @brief     ：2分钟功率
* @param[in] ：int cid         数据中心id
               uint8 grp     总加组序号
               CTRL_SUBGROUP_T *ptSubGroup 总加组配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
void appctrl_cal_power_2min(int cid, uint8 grp, CTRL_SUBGROUP_T *ptSubGroup)
{
    uint8  t;
    //OOP_BITSTR8_T bstatus;

    for (t = 0; t < OOP_MAX_TURN; t++)
    {
        /* 获取跳闸后两分钟总加功率 */
        if (g_PowerInfo.bTwoMin[grp][t] != 0)
        {
            g_PowerInfo.bfgFirstPrtl[grp][t] = 1;//-置位需要记录两分钟功率的标志-
            g_PowerInfo.bTwoMin[grp][t]--;
        }
        if(0 == g_PowerInfo.bTwoMin[grp][t] && 1 == g_PowerInfo.bfgFirstPrtl[grp][t])
        {
            //-获取跳闸后两分钟总加功率 更新事件
            //bstatus.nValue = (0x01 << t); 
            //CheckERC_3200(cid, appctrl_get_object(g_PowerInfo.bPcKind[grp]), 0x2301 + grp, bstatus, 
            //              g_PowerInfo.wPowerSetValue[grp], ptSubGroup->actPower, g_PowerInfo.wPowerONValue[grp][t], 2);
            g_PowerInfo.bfgFirstPrtl[grp][t] = 0;
            CTRL_FMT_DEBUG("总加组%d 轮次%d, 补充跳闸2min后功率%lld, 跳闸功率%lld\n", grp + 1, t + 1,
                            ptSubGroup->actPower, g_PowerInfo.wPowerONValue[grp][t]);
        }
    }
}


/**********************************************************************
* @name      : appctrl_cal_period
* @brief     ：计算功控时段
* @param[in] ：OOP_POWCON_TIME_T *ptPowconTime 时段
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
uint8 appctrl_cal_period(OOP_POWCON_TIME_T *ptPowconTime)//-计算功控时段-
{
    PCSEG_INFO_T PCSegData[48]; //-8个功控时段的分布 保险一点翻倍-
    uint8        PCSegNo = 0;
    uint8 aBuf[12] = {0};
    uint8 ctrlState[48] = {0};
    uint8 i,j;        //-循环变量-
    uint8 k = 0;
    char temp[400] = {0};
    uint16 offset = 0;

    memcpy(aBuf, ptPowconTime->period, sizeof(ptPowconTime->period));
    for(i = 0; i < 12; i++)
    {
        for(j = 0; j < 4; j++)
        {
            ctrlState[k] = ((aBuf[i] >> (2 * j)) & 0x03);
            if(3 == ctrlState[k])
            {
                ctrlState[k] = 0;
            }
            k++;
        }
    }

    memset(PCSegData ,0x0,sizeof(PCSegData));
    PCSegNo = 0;

    PCSegData[0].home = 0;
    PCSegData[0].end = 30;
    PCSegData[0].index = ctrlState[0];

    for(k = 0; k < 48; k++)
    {
        if(PCSegData[PCSegNo].index != ctrlState[k])
        {
            PCSegData[PCSegNo].end = 30 * k;
            PCSegNo++;
            PCSegData[PCSegNo].home = 30 * k;
            PCSegData[PCSegNo].index = ctrlState[k];
        }
    }
    PCSegData[PCSegNo].end = 30 * k;
    PCSegNo++;

    /* 过滤当中的0（不受控不记录） */
    g_bPCSegNo = 0;
    memset(g_PCSegData, 0x0, sizeof(g_PCSegData));
    for(i = 0; i < PCSegNo; i++)
    {
        if(0 != PCSegData[i].index)
        {
            g_PCSegData[g_bPCSegNo] = PCSegData[i];
            g_bPCSegNo++;
        }
    }

    for(i = 0; i < g_bPCSegNo; i++)
    {
        offset += sprintf(temp + offset, " (%d %d~%d, %d)", i + 1, g_PCSegData[i].home, g_PCSegData[i].end, 
                          g_PCSegData[i].index);
    }
    CTRL_FMT_DEBUG("Period ctrl num %d:%s\n", g_bPCSegNo, temp);
    return g_bPCSegNo;

}

/**********************************************************************
* @name      : appctrl_check_whichSeg
* @brief     ：判断当前时间属于哪个时段
* @param[in] ：
* @param[out]：
* @return    ：时段号
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
uint8 appctrl_check_whichSeg()
{
    uint8            i;
    uint32           wPcInter;    //-当前时间距当日起始时间的间隔时间-
    time_t           now;
    time_t           start;
    OOP_DATETIME_S_T curTime = {0};
    OOP_DATETIME_S_T startime = {0};

    now = time(NULL);
    DT_Time2DateTimeShort(now, &curTime);
    DT_Time2DateTimeShort(now, &startime);

    startime.hour = 0x0;
    startime.minute = 0x0;
    startime.second = 0x0;

    DT_DateTimeShort2Time(&startime, &start);

    wPcInter = now - start;
    //-判断当前时间属于哪个时段-
    for (i = 0; i < g_bPCSegNo; i++)
    { 
        if (wPcInter < ((uint32)g_PCSegData[i].end) * 60  && wPcInter >= ((uint32)g_PCSegData[i].home) * 60)
        {
            break;
        }
    }
    if(0 == g_PCSegData[i].index)
    {
        i = g_bPCSegNo;
    }
    return i;
}

/**********************************************************************
* @name      : appctrl_pcturn_change
* @brief     ：有未投入的轮次或轮次重新投入，状态为告警或拉闸，要合闸/取消告警
* @param[in] ：uint8 turnMax              最大轮次
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-1-26
* @Update    :
**********************************************************************/
void appctrl_pcturn_change(uint8 turnMax)
{
    uint8 i, j;
    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    /* 总加组功率控制 */
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        for(j = 0; j < OOP_MAX_TURN; j++)
        {
            if (CTRL_UNEFFECT == ((ptSubGroups[i].ctrlStatus.pcTurn.nValue >> j) & 0x1))
            {    //-判断是否有未投入的轮次，状态为功控告警或拉闸，如果有则合闸或取消告警-
                if(TURN_ON == appctrl_get_switch_status(g_PowerInfo.bPcKind[i], CTRL_STATUS_STORE, j, i))
                {
                    CTRL_FMT_DEBUG("功控轮次%d解除投入, 合闸操作\n", j + 1);
                }
                else if(TURN_ON == appctrl_get_switch_status(g_PowerInfo.bPcKind[i], CTRL_ALARM_STORE, j, i))
                {
                    CTRL_FMT_DEBUG("功控轮次%d解除投入, 取消告警\n", j + 1);
                }
                appctrl_power_close(g_PowerInfo.bPcKind[i], i, j, 0);
            }
            else if(CTRL_UNEFFECT == g_PowerInfo.bPcTurnSet[i][j]
                && TURN_OFF == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, j, i))
            {    //-如果前面轮次重新投入，则把当前告警轮次清除-
                appctrl_pcstop_alarm(g_PowerInfo.bPcKind[i], i, j); /*-告警状态清除-*/
            }
            g_PowerInfo.bPcTurnSet[i][j] = ((ptSubGroups[i].ctrlStatus.pcTurn.nValue >> j) & 0x1);
        }
    }
}

/**********************************************************************
* @name      : appctrl_period_change
* @brief     ：时段控时段序号变化先合闸
* @param[in] ：uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_ARG_ALL_T *ptCtrlArg  控制配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-1-26
* @Update    :
**********************************************************************/
void appctrl_period_change(uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_ARG_ALL_T *ptCtrlArg)
{
    uint8 k,m;
    uint8 idxPeriod = 0;  //时段功控定值方案号
    uint8 j;
    OOP_TGSETTING_T *ptCtrlStatus = &(ptGroup->ctrlStatus);
    OOP_8103_CONFIG_T *ptPeriodArg = &(ptCtrlArg->tPeriod.param);
    OOP_CFGUNIT_8109_T *ptPeriodCfg = NULL;

    /* 时段控制 */
    if(CTRL_PERIOD_TYPE == g_PowerInfo.bPcKind[grp] && CTRL_EFFECT == (ptCtrlStatus->pcState.nValue & 0x01))
    {
        idxPeriod = ptCtrlStatus->idxPeriod;
        m = appctrl_check_whichSeg();

        for(j = 0; j < ptPeriodArg->nNum; j++)
        {
            if(ptPeriodArg->item[j].object == 0x2301 + grp)
            {
                ptPeriodCfg = &(ptPeriodArg->item[j]);
                break;
            }
        }
        //-当前时间不处于时段控任何投入的时段,则合闸-
        if(NULL == ptPeriodCfg || m != g_PowerInfo.bCurrentSeg[grp] || m >= g_bPCSegNo ||
           idxPeriod != g_PowerInfo.curPeriod[grp] || 
           idxPeriod > 2 || 0 == (ptPeriodCfg->value[idxPeriod].valid & (0x01 << m)) )
        {
            CTRL_FMT_DEBUG("总加组%d时段控方案%d, 旧方案%d\n", grp + 1, idxPeriod + 1, g_PowerInfo.curPeriod[grp] + 1);
            CTRL_FMT_DEBUG("总加组%d时段控当前时段序号%d, 之前时段序号%d\n", grp + 1, m, g_PowerInfo.bCurrentSeg[grp]);
            for (k = 0; k < OOP_MAX_TURN; k++)
            {
                appctrl_power_close(g_PowerInfo.bPcKind[grp],grp, k, OUTOFTIME_CLOSE);
            }
            g_PowerInfo.bCurrentSeg[grp] = m;   //-记录所处当前时段号-
            g_PowerInfo.curPeriod[grp] = idxPeriod;
        }
    }
}

/**********************************************************************
* @name      : appctrl_get_DownPara
* @brief     ：获取功率下浮控参数
* @param[in] ：uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_DOWN_ARG_T *ptArg     控制配置
               int64 safevalue            保安定值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
int appctrl_get_DownPara(uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_DOWN_ARG_T *ptArg, int64 safevalue)
{
    uint8             k;
    int64             power = 0;
    OOP_POWCON_LOW_T *pProject = &(ptArg->project[grp]);
    time_t            starttime = 0;
    time_t            curtime;
    uint8             slipTime = pProject->scheme.slipTime; //-当前功率下浮控滑差时间-
    int8              efficient = pProject->scheme.floatCoefficient;

    g_PowerInfo.bDownCtrlCoe[grp] = efficient;

    /* 合法性判断 */
    if(FALSE == ptArg->bEffect[grp])
    {
        return -1;
    }
    if(pProject->object != 0x2301 + grp)
    {
        return -2;
    }

    if(ptArg->starttime[grp] > 0)
    {
        g_PowerInfo.tDownStart[grp] = ptArg->starttime[grp];
    }
    starttime = g_PowerInfo.tDownStart[grp];

    /* 判断是否在下浮控时间内 */
    curtime = time(NULL);
    if(curtime < starttime || curtime - starttime > ((uint32)pProject->scheme.conTime) * 1800)
    {
        ptArg->freezeTime[grp] = 0;
        ptArg->starttime[grp] = 0;
        ptArg->bEffect[grp] = FALSE; //过时间自己解除
        appctrl_set_down_close(grp + 1);
        return -3;
    }

    if(TRUE == ptArg->bFirst[grp])
    {
        //有新的命令下发,计算新的功率定值
        if(0 == slipTime)
        {
            slipTime = 1;
        }
        if(slipTime > SUBGROUP_SLIP_MAX)
        {
             slipTime = SUBGROUP_SLIP_MAX;
        }

        if (ptGroup->slipnum <= slipTime)
        {
            slipTime = ptGroup->slipnum;
            for (k = 0; k < ptGroup->slipnum; k++)
            {
                power += ptGroup->actPowerList[k];
            }
        }
        else
        {
            for (k = ptGroup->slipnum - slipTime; k < ptGroup->slipnum; k++)
            {
                power += ptGroup->actPowerList[k];
            }
        }
        //-根据实时滑差功率计算出下浮控功率定值-
        g_PowerInfo.wPowerSetValue[grp] = (power * (100 + efficient))/(slipTime * 100);

        CTRL_FMT_DEBUG("总加组%d下浮控(%lld, %d, %d)计算定值%lld, 保安定值%lld\n", grp + 1, power, efficient, slipTime,
                       g_PowerInfo.wPowerSetValue[grp], safevalue);
        if(g_PowerInfo.wPowerSetValue[grp] < safevalue)
        {
            g_PowerInfo.wPowerSetValue[grp] = safevalue;//如果功率定值小于保安定值,则取保安定值-
        }
        
        ptGroup->slipPower = g_PowerInfo.wPowerSetValue[grp];
        if (g_PowerInfo.wPowerSetValue[grp] <= 0)
        {
            //功率定值为0，不进行控制
            return -4;
        }
        ptArg->bFirst[grp] = FALSE;
        g_PowerInfo.bFirstDownCtrl[grp] = 0;

        /* 重新投入刷新冻结数据 */
        appctri_write_freezepower(grp, 0);
    }

    //防止启动后从文件读出的0值控制
    if (g_PowerInfo.wPowerSetValue[grp] <= 0)
    {
        //功率定值为0，不进行控制
        return -5;
    }
    else if(g_PowerInfo.wPowerSetValue[grp] < safevalue)
    {
        g_PowerInfo.wPowerSetValue[grp] = safevalue;//如果功率定值小于保安定值,则取保安定值-
    }

    if (0 == g_PowerInfo.bFirstDownCtrl[grp])       //首次执行下浮控，则读取下浮控冻结时间-*/
    {
        ptArg->freezeTime[grp] = pProject->scheme.delayTime;
        if(0 == ptArg->freezeTime[grp])
        {
            ptArg->freezeTime[grp] = 2;             //如果未设置此冻结时间，默认2分钟后冻结当前功率-
        }

        g_PowerInfo.bFirstWriteFreeze[grp] = 1;     //清下浮控冻结当前功率标志
        g_PowerInfo.bFirstDownCtrl[grp] = 1;        //清首次执行下浮控标志
    }

    if (0 == ptArg->freezeTime[grp])                //冻结时间到，获取下浮控功率冻结值
    {
        if (1 == g_PowerInfo.bFirstWriteFreeze[grp])//是首次执行下浮控逻辑
        {
            appctri_write_freezepower(grp, ptGroup->actPower);
            g_PowerInfo.bFirstWriteFreeze[grp] = 0; //清首次执行下浮控逻辑标志
        }
    }
    else
    {
        g_PowerInfo.bFirstWriteFreeze[grp] = 1;
    }

    return 0;
}

/**********************************************************************
* @name      : appctrl_get_DownPara
* @brief     ：获取营业报停控参数
* @param[in] ：uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_DOWN_ARG_T *ptArg     控制配置
               int64 safevalue            保安定值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-8
* @Update    :
**********************************************************************/
int appctrl_get_StopPara(uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_STOP_ARG_T *ptArg, int64 safevalue)
{
    uint8 j;
    OOP_CFGUNIT_810B_T *pParam = NULL;
    OOP_DATETIME_S_T temp = {0};
    uint8 bStart = FALSE;
    uint8 bEnd = FALSE;

    /* 合法性判断 */
    if(FALSE == ptArg->bEffect[grp])
    {
        return -1;
    }
    for(j = 0; j < ptArg->param.nNum; j++)
    {
        if(ptArg->param.item[j].object == 0x2301 + grp)
        {
            pParam = &(ptArg->param.item[j]);
            break;
        }
    }
    if(NULL == pParam)
    {
        return -2;
    }

    /* 判断当前时间是否在营业报停控时间段内 */
    DT_Time2DateTimeShort(time(NULL), &temp);
    if(temp.year > pParam->startTime.year || 
       (temp.year == pParam->startTime.year && temp.month > pParam->startTime.month) ||
       (temp.year == pParam->startTime.year && temp.month == pParam->startTime.month && temp.day >= pParam->startTime.day))
    {
        bStart = TRUE;
    }

    if(temp.year < pParam->endTime.year ||
       (temp.year == pParam->endTime.year && temp.month < pParam->endTime.month) ||
       (temp.year == pParam->endTime.year && temp.month == pParam->endTime.month && temp.day <= pParam->endTime.day))
    {
        bEnd = TRUE;
    }

    if(FALSE == bStart || FALSE == bEnd)
    {
        return -3;
    }

    if(0 == safevalue)
    {
        safevalue = DEFAULT_PW_PARA;//-送检-
    }

    g_PowerInfo.wPowerSetValue[grp] = pParam->defValue;

    if(g_PowerInfo.wPowerSetValue[grp] < safevalue)
    {
        g_PowerInfo.wPowerSetValue[grp] = safevalue;//-如果功率定值小于保安定值,则取保安定值-
    }
    if(g_PowerInfo.wPowerSetValue[grp] <= 0)
    {
        return -4;//-功率定值为0,不进行控制-
    }

    g_PowerInfo.bFirstDownCtrl[grp] = 0;  //-不是下浮控，则首次投入下浮控标志清零-
    return 0;
}

/**********************************************************************
* @name      : appctrl_get_DownPara
* @brief     ：获取厂休控参数
* @param[in] ：uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_REST_ARG_T *ptArg     控制配置
               int64 safevalue            保安定值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-8
* @Update    :
**********************************************************************/
int appctrl_get_RestPara(uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_REST_ARG_T *ptArg, int64 safevalue)
{
    uint8 j;
    OOP_CFGUNIT_810A_T *pParam = NULL;
    time_t              now;
    time_t              start;
    OOP_DATETIME_S_T    curTime = {0};
    OOP_DATETIME_S_T    tempTime = {0};

    /* 合法性判断 */
    if(FALSE == ptArg->bEffect[grp])
    {
        return -1;
    }
    for(j = 0; j < ptArg->param.nNum; j++)
    {
        if(ptArg->param.item[j].object == 0x2301 + grp)
        {
            pParam = &(ptArg->param.item[j]);
            break;
        }
    }
    if(NULL == pParam)
    {
        return -2;
    }

    /* 判断当前时间是否在厂休控时间段内 */
    now = time(NULL);
    DT_Time2DateTimeShort(now, &curTime);
    if(0 == curTime.week)
    {
        curTime.week = 7;  //周日处理
    }
    if (0 == (pParam->powLimit.nValue & (0x01 << curTime.week)))
    {
        return -3;
    }
    tempTime.year = curTime.year;
    tempTime.month = curTime.month;
    tempTime.day = curTime.day;
    tempTime.hour = pParam->startTime.hour;
    tempTime.minute = pParam->startTime.minute;
    tempTime.second = pParam->startTime.second;
    DT_DateTimeShort2Time(&tempTime, &start);
    if(now < start || now > start + pParam->duration * 60)
    {
        return -4;
    }
    CTRL_FMT_TRACE("Subgroup %d, rest ctrl left time is %ds\n", grp + 1, start + pParam->duration * 60 - now);

    g_PowerInfo.wPowerSetValue[grp] = pParam->defValue;

    if(0 == safevalue)
    {
        safevalue = DEFAULT_PW_PARA;//-送检-
    }

    if(g_PowerInfo.wPowerSetValue[grp] < safevalue)
    {
        g_PowerInfo.wPowerSetValue[grp] = safevalue;//-如果功率定值小于保安定值,则取保安定值-
    }

    if (g_PowerInfo.wPowerSetValue[grp] <= 0)
    {
        return -5;//-功率定值为0，不进行控制-
    }

    g_PowerInfo.bFirstDownCtrl[grp] = 0;  //-不是下浮控，则首次投入下浮控标志清零-
    return 0;
}

/**********************************************************************
* @name      : appctrl_get_DownPara
* @brief     ：获取时段功控参数
* @param[in] ：uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_PERIOD_ARG_T *ptArg   控制配置
               int64 safevalue            保安定值
               OOP_POWCON_TIME_T *ptPowconTime 时段配置 
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-8
* @Update    :
**********************************************************************/
int appctrl_get_SegPara(uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_PERIOD_ARG_T *ptArg, 
                          int64 safevalue, OOP_POWCON_TIME_T *ptPowconTime)
{
    uint8 j, m;
    uint8 idxPeriod = 0;  //时段功控定值方案号

    OOP_CFGUNIT_8109_T  *pParam = NULL;

    /* 合法性判断 */
    if(FALSE == ptArg->bEffect[grp])
    {
        return -1;
    }
    for(j = 0; j < ptArg->param.nNum; j++)
    {
        if(ptArg->param.item[j].object == 0x2301 + grp)
        {
            pParam = &(ptArg->param.item[j]);
            break;
        }
    }
    if(NULL == pParam)
    {
        return -2;
    }

    /* 判断方案是否投入 */
    idxPeriod = ptGroup->ctrlStatus.idxPeriod;
    if(0 == (pParam->plan & (0x01 << idxPeriod)))
    {
        return -3;
    }
    
    /* 判断时段是否投入 */
    m = appctrl_check_whichSeg();
    if(m >= g_bPCSegNo)
    {
        return -4;
    }
    g_PowerInfo.wPowerSetValue[grp] = DEFAULT_PW_PARA;
    if(0 == (pParam->value[idxPeriod].valid & (0x01 << m)))
    {
        return -5;// 时段没有投入，不进行控制
    }

    g_PowerInfo.wPowerSetValue[grp] = pParam->value[idxPeriod].defValue[m];
    g_PowerInfo.wPowerSetValue[grp] = pParam->value[idxPeriod].defValue[m] * (100 + pParam->floatCoe)/100;
    if (0 == safevalue)
    {
        safevalue = DEFAULT_PW_PARA;//-送检-
    }
    CTRL_FMT_DEBUG("Period ctrl set value: %lld, safevalue %lld\n", g_PowerInfo.wPowerSetValue[grp], safevalue);
    if (g_PowerInfo.wPowerSetValue[grp] < safevalue)
    {   
        g_PowerInfo.wPowerSetValue[grp] = safevalue;//-如果功率定值小于保安定值，则取保安定值-
    }
    
    g_PowerInfo.bFirstDownCtrl[grp] = 0;  //-不是下浮控，则首次投入下浮控标志清零-

    return 0;
}

/**********************************************************************
* @name      : appctrl_get_pcpara
* @brief     ：获取功控参数,依据功控优先级的判断
* @param[in] ：uint8 grp                   总加组
               uint8 turnMax               最大轮次
               CTRL_SUBGROUP_T *ptSubGroup 总加组配置
               CTRL_ARG_ALL_T *ptCtrlArg   控制配置
* @param[out]：
* @return    ：控制类型
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    : 2021-4-15
**********************************************************************/
uint8 appctrl_get_pcpara(uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_ARG_ALL_T *ptCtrlArg)
{
    int   ret;
    uint8 bType;
    
    for (bType = CTRL_DOWN_TYPE; bType <= CTRL_PERIOD_TYPE; bType++)
    {
        //按优先级取当前投入的功控类型
        if(0 == (ptGroup->ctrlStatus.pcState.nValue & (0x8 >> bType)))
        {
            continue;
        }
        if(CTRL_DOWN_TYPE == bType)
        {   //下浮控投入判断
            ret = appctrl_get_DownPara(grp, turnMax, ptGroup, &(ptCtrlArg->tDown), ptCtrlArg->safevalue);
            if(0 != ret)
            {
                g_PowerInfo.tDownStart[grp] = 0;
            }
        }
        else if(CTRL_STOP_TYPE == bType)
        {   //营业报停控投入判断
            ret = appctrl_get_StopPara(grp, turnMax, ptGroup, &(ptCtrlArg->tStop), ptCtrlArg->safevalue);
        }
        else if(CTRL_REST_TYPE == bType)
        {   //厂休控投入判断
            ret = appctrl_get_RestPara(grp, turnMax, ptGroup, &(ptCtrlArg->tRest), ptCtrlArg->safevalue);
        }
        else if(CTRL_PERIOD_TYPE == bType)
        {   //时段功控投入判断
            ret = appctrl_get_SegPara(grp, turnMax, ptGroup, &(ptCtrlArg->tPeriod), ptCtrlArg->safevalue, 
                                      &(ptCtrlArg->tPowconTime));
        }
        if(0 == ret) //符合条件 退出
        {
            break;
        }
    }
    return bType;
}

/**********************************************************************
* @name      : appctrl_power_low_setvalue
* @brief     ：功率连续低于定值10分钟则合闸,下浮控立即合闸
* @param[in] ：uint8 grp                   总加组
               uint8 turnMax               最大轮次
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_power_low_setvalue(uint8 grp, uint8 turnMax)
{
    uint8 turn;

    for (turn = 0; turn < turnMax; turn++)
    {
        if(TURN_ON == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_ALARM_STORE, turn, grp))
        {
            appctrl_pcstop_alarm(g_PowerInfo.bPcKind[grp], grp, turn); /*-告警状态清除-*/
            g_PowerInfo.bFirstInit[grp][turn] = 0;          //-告警延时计数器首次赋初值复位-
            g_PowerInfo.wPowerAlarmDelay[grp][turn] = 0;    //-轮次间延时计数器清零-
            g_PowerInfo.wPcAlarmDelayss[grp][turn] = 0;
            g_PowerInfo.bFirstPassPower[grp][turn] = NOT_PASS_SETVAL;   //-首次超定值标志复位-
        }
        g_PowerInfo.bFirstInit[grp][turn] = 0;    //-告警延时计数器首次赋初值标志-
    }

    g_PowerInfo.bFirstDownCtrl[grp] = 0;

/* 甘肃需求: 在功控跳闸之后，如实时功率小于功率控制定值则允许合闸 */
#if defined AREA_GANSU
    for (turn = 0; turn < turnMax; turn++)
    {
        //-状态为功控拉闸，则合上功控拉的闸-
        if (TURN_ON == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, turn, grp))
        {
            CTRL_FMT_DEBUG("总加组%d轮次%d合闸\n", grp + 1, turn + 1);
            appctrl_pcclose_break(g_PowerInfo.bPcKind[grp], grp, turn); /*-跳闸状态清除-*/
            g_PowerInfo.bTwoMin[grp][turn] = 0;     //-2分钟冻结计时复位-
            g_PowerInfo.bfgFirstPrtl[grp][turn]=0;  //-首次补充2分钟后功控记录标志复位-
        }
    }
#endif

#if DOWN_POWER_CLOSE_EN == 1 //-功控中实时功率小于定值,允许合闸使能-
    if(CTRL_PERIOD_TYPE == g_PowerInfo.bPcKind[grp])
    {
        if(g_PowerInfo.bFirstDownPower[grp] == 0)
        {    //-如功率首次低于定值，则清5分钟计数器，开始计时-
            g_PowerInfo.bDownTenMin[grp] = 0;
            g_PowerInfo.bFirstDownPower[grp] = 1;
        }
        else
        {    //-5分钟计数器累加-
            if(g_PowerInfo.bDownTenMin[grp] < 5)
                g_PowerInfo.bDownTenMin[grp]++;
        }
        if(g_PowerInfo.bDownTenMin[grp] > =3)//-因为控制实际运行比统计快，功率小于定值时间不准，这里需要调试-
        {    //-当前功率持续5分钟低于功率定值，则功控合闸-
            g_PowerInfo.bFirstDownPower[grp] = 0;
            for (turn = 0; turn < turnMax; turn++)
            {
                //-状态为功控拉闸，则合上功控拉的闸-
                if (TURN_ON == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, turn, grp))
                {    
                    appctrl_pcclose_break(g_PowerInfo.bPcKind[grp], grp, turn); /*-跳闸状态清除-*/
                    g_PowerInfo.bTwoMin[grp][turn] = 0;     //-2分钟冻结计时复位-
                    g_PowerInfo.bfgFirstPrtl[grp][turn]=0;  //-首次补充2分钟后功控记录标志复位-
                }
            }
        }
    }
    else
    {
        g_PowerInfo.bFirstDownPower[grp] = 0;
    }
#elif DOWN_POWER_CLOSE_EN == 2 //-其他地区下浮控小于定值立即合闸，其他功控小于定值10分钟合闸-
    g_PowerInfo.bFirstDownPower[grp] = 0;
    if(0 == g_PowerInfo.bPcKind[grp])
    {
        g_PowerInfo.bDownTenMin[grp] = 10;   //-下浮控小于定值,10分钟计数到，立即合闸-
    }
    else
    {
        if (g_PowerInfo.bFirstDownPower[grp] == 0)
        {    //-如功率首次低于定值，则清10分钟计数器，开始计时-
            g_PowerInfo.bDownTenMin[grp] = 0;
            g_PowerInfo.bFirstDownPower[grp] = 1;
        }
        else
        {    //-10分钟计数器累加-
            g_PowerInfo.bDownTenMin[grp]++;
        }
    }

    if (g_PowerInfo.bDownTenMin[grp]>=10)
    {    //-当前功率持续10分钟低于功率定值，则功控合闸-
        g_PowerInfo.bFirstDownPower[grp] = 0;
        for (turn = 0; turn<TURN_NUM; turn++)
        {
            //-状态为功控拉闸，则合上功控拉的闸-
            if (TURN_ON == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, turn, grp))
            {   
                appctrl_pcclose_break(g_PowerInfo.bPcKind[grp], grp, turn); /*-跳闸状态清除-*/
                g_PowerInfo.bTwoMin[grp][turn] = 0;     //-2分钟冻结计时复位-
                g_PowerInfo.bfgFirstPrtl[grp][turn]=0;  //-首次补充2分钟后功控记录标志复位-
            }
        }
    }
    #endif
}

/**********************************************************************
* @name      : appctrl_save_record
* @brief     ：功控时间到利用事件模块的写纪录接口纪录控制状态
* @param[in] ：int cid                     数据中心客户端ID
               uint8 grp                   总加组
               uint8 turn                  轮次
               CTRL_SUBGROUP_T *ptSubGroup 总加组配置

* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_save_record(int cid, uint8 grp, uint8 turn, CTRL_SUBGROUP_T *ptSubGroup)
{
    //OOP_BITSTR8_T bstatus;

    if (TURN_OFF == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, turn, grp))
    {
        //-拉闸告警-
        appctrl_power_OpenAlarm(g_PowerInfo.bPcKind[grp], grp, turn);

        //-填写功控跳闸事件-
        //bstatus.nValue = (0x01 << turn);
        //CheckERC_3200(cid, appctrl_get_object(g_PowerInfo.bPcKind[grp]), 0x2301 + grp, bstatus, 
        //              g_PowerInfo.wPowerSetValue[grp], 0, ptSubGroup->actPower, 1);
        
        g_PowerInfo.wPowerONValue[grp][turn] =  ptSubGroup->actPower;
        g_PowerInfo.bTwoMin[grp][turn] = 2;  /*-第一次跳闸后开始计时，等待两分钟后补充功率-*/
        g_PowerInfo.bFirstPassPower[grp][turn] = NOT_PASS_SETVAL;  /*-首次超定值标志复位-*/
        g_PowerInfo.bFirstInit[grp][turn] = 0;
    }
}

/**********************************************************************
* @name      : appctrl_step_deal
* @brief     ：根据控制进程步的不同进行控制
* @param[in] ：int cid                     数据中心客户端ID
               uint8 grp                   总加组
               uint8 turnMax               最大轮次
               CTRL_SUBGROUP_T *ptSubGroup 总加组配置
               CTRL_ARG_ALL_T *ptCtrlArg   控制配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-8
* @Update    : 2021-4-15
**********************************************************************/
void appctrl_step_deal(int cid, uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_ARG_ALL_T *ptCtrlArg)
{
    uint8 t = 0;
    uint8 tmore = 0;

    CTRL_FMT_DEBUG("总加组%d: 滑差功率 %lld, 实时功率 %lld, 定值 %lld\n", 
                    grp + 1, ptGroup->aveActPower, ptGroup->actPower, g_PowerInfo.wPowerSetValue[grp]);

    /* 低于控制值 */
    if(ptGroup->aveActPower <= g_PowerInfo.wPowerSetValue[grp] || 
       ptGroup->actPower <= g_PowerInfo.wPowerSetValue[grp])
    {
        appctrl_power_low_setvalue(grp, turnMax); //处于告警状态则清除告警状态
        g_PowerTurnStop[grp] = 0;
        return;
    }

    for(t = 0; t < turnMax; t++)
    {
        /* 配置总加组 没有投入次轮控制 */
        if(CTRL_UNEFFECT == g_PowerInfo.bPcTurnSet[grp][t])
        {
            continue;
        }
        /* 该轮次已经投入控制动作 */
        if(TURN_ON == appctrl_get_switch_status(g_PowerInfo.bPcKind[grp], CTRL_STATUS_STORE, t, grp))
        {
            continue;//-继续下个轮次-
        }

#ifdef AREA_HEILONGJIANG
        /* 黑龙江跳闸后等2min继续下一轮*/
        if(0 != g_PowerTurnStop[grp])
        {
            g_PowerTurnStop[grp] -= 1;
            break;
        }
#endif

        if (0 == g_PowerInfo.bFirstInit[grp][t])    //-首次告警延时-
        {
            /* 告警延时计算 */
            if(CTRL_DOWN_TYPE == g_PowerInfo.bPcKind[grp])
            {
                g_PowerInfo.wPowerAlarmDelay[grp][t] = ptCtrlArg->tDown.project[grp].scheme.waringTime[t];
            }
            else
            {
                g_PowerInfo.wPowerAlarmDelay[grp][t] = ptCtrlArg->tPowconWarn.time[t];
            }
            if(0 == g_PowerInfo.wPowerAlarmDelay[grp][t])
            {
                g_PowerInfo.wPowerAlarmDelay[grp][t] = 1;
            }
            g_PowerInfo.wPcAlarmDelayss[grp][t] = g_PowerInfo.wPowerAlarmDelay[grp][t] * 60;
            g_PowerInfo.bFirstInit[grp][t] = 1;//-告警延时计数器首次赋初值标志-
            CTRL_FMT_DEBUG("总加组%d, 轮次%d, 功控类型%d, 告警时间%dmin\n", grp + 1, t + 1, 
                            g_PowerInfo.bPcKind[grp], g_PowerInfo.wPowerAlarmDelay[grp][t]);
            appctrl_power_SetAlarm(g_PowerInfo.bPcKind[grp], grp, t); //注册告警显示页面

            /* 后面如果处于告警的轮次需要清空 */
            for(tmore = t + 1; tmore < OOP_MAX_TURN; tmore++)
            {
                if(TURN_ON == appctrl_get_switch_status(g_PowerInfo.bPcKind[grp], CTRL_ALARM_STORE, tmore, grp))
                {
                    g_PowerInfo.wPowerAlarmDelay[grp][tmore] = 0;
                    g_PowerInfo.wPcAlarmDelayss[grp][tmore] = 0;
                    appctrl_set_switch_status(g_PowerInfo.bPcKind[grp], CTRL_ALARM_STORE, tmore, grp, TURN_OFF);
                }
            }
        }
        if (g_PowerInfo.wPowerAlarmDelay[grp][t] != 0)
        {    //-告警延时时间没结束-
            g_PowerInfo.wPcAlarmDelayss[grp][t] = g_PowerInfo.wPowerAlarmDelay[grp][t] * 60;
            g_PowerInfo.wPowerAlarmDelay[grp][t]--;
            //-每分钟告警-
            appctrl_alarm_data(g_PowerInfo.bPcKind[grp], grp, 3, EMAUDIO_INDEX_17, appctrl_power_guishow);
            g_PowerTurnStop[grp] = 0;
        }
        else
        {
            g_PowerInfo.wPcAlarmDelayss[grp][t] = 0;
            appctrl_save_record(cid, grp, t, ptGroup); //功控拉闸处理
#ifdef AREA_HEILONGJIANG
            /* 黑龙江跳闸后等2min继续下一轮（厂休功控和时段功控）  */
            if(CTRL_PERIOD_TYPE == g_PowerInfo.bPcKind[grp] || CTRL_REST_TYPE == g_PowerInfo.bPcKind[grp])
            {
                g_PowerTurnStop[grp] = 1;
                CTRL_FMT_DEBUG("总加组%d: 跳闸后延时%dmin控制下一轮\n", grp + 1, g_PowerTurnStop[grp] + 1);
            }
            else
            {
                g_PowerTurnStop[grp] = 0;
            }
#endif
        }
        break;
    }
}

/**********************************************************************
* @name      : appctrl_power_decress_time
* @brief     ：功控拉闸轮次间延时递减1
* @param[in] ：uint8  turnmax  最大轮次数
               CTRL_DOWN_ARG_T *ptArg  下浮控参数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_power_decress_time(uint8 turnmax, CTRL_DOWN_ARG_T *ptArg)
{
    uint8 i;
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        if(0 != ptArg->freezeTime[i])
        {
            ptArg->freezeTime[i]--;
        }
    }
}

/**********************************************************************
* @name      : appctrl_power_show_decress
* @brief     ：功控时间递减1s
* @param[in] ：uint8  turnmax  最大轮次数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-11-6
* @Update    :
**********************************************************************/
void appctrl_power_show_decress(uint8 turnmax)
{
    uint8 i,j;
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        for (j = 0; j < turnmax; j++)
        {
            if(g_PowerInfo.wPcAlarmDelayss[i][j] != 0)
            {
                g_PowerInfo.wPcAlarmDelayss[i][j]--;
            }
        }
    }
}

/**********************************************************************
* @name      : appctrl_power_ctrl
* @brief     ：功率控制, 只记录功控合闸、告警中间状态
* @param[in] ：int cid         数据中心id
               uint8 bDayPass  是否过日
               uint8 turnmax   最大轮次数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
void appctrl_power_ctrl(int cid, uint8 bDayPass, uint8 turnmax)
{
    uint8 i, j;
    uint8 pcType = 0;
    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计
    char           * ctrlname[4] = {"下浮控", "报停控", "厂休控", "时段控"};

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    /* 功控拉闸轮次间延时递减1 */
    appctrl_power_decress_time(turnmax, &(ptCtrlArg->tDown));

    /* 总加组功率控制 */
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        appctrl_cal_power(cid, i, &(ptSubGroups[i])); //记录总加组功率
        
        if(0 == ptSubGroups[i].ctrlStatus.pcState.nValue || 0 == ptSubGroups[i].ctrlPwTurn.nValue)
        {
            //-无功控投入，如有功控拉闸，则合闸-
            for (j = 0; j < OOP_MAX_TURN; j++)
            {
                appctrl_power_close(g_PowerInfo.bPcKind[i], i, j, CTRLOFF_CLOSE);
            }
            g_PowerInfo.bFirstDownCtrl[i] = 0;  //-首次投入下浮控标志置0-
            g_PowerInfo.bPcKind[i] = CTRL_NULL_TYPE;
            g_PowerInfo.bLastPCtrl[i] = CTRL_NULL_TYPE;
            g_PowerTurnStop[i] = 0;
            continue;
        }

        appctrl_cal_power_2min(cid, i, &(ptSubGroups[i])); //获取功控滑差功率 2min后功率
        /* 过日处理 */
        if(TRUE == bDayPass)
        {
            if(0 == appctrl_power_passday(ptCtrlArg, i, turnmax))
            {
                g_PowerTurnStop[i] = 0;
                continue;
            }
        }
        
        /* 获取功控参数，确定当前功控类型，并处理出控制时间的合闸 */
        pcType = appctrl_get_pcpara(i, turnmax, &(ptSubGroups[i]), ptCtrlArg);
        
        /* 控制状态清理 */
        appctrl_power_status_clear(pcType, i);
        
        if(pcType > CTRL_PERIOD_TYPE)
        {
            g_PowerInfo.bPcKind[i] = CTRL_NULL_TYPE;
            g_PowerInfo.bLastPCtrl[i] = CTRL_NULL_TYPE;
            g_PowerInfo.wPowerSetValue[i] = 0;
            g_PowerTurnStop[i] = 0;
            continue;
        }
        g_PowerInfo.bPcKind[i] = pcType;

        /* 时段控时段发生变化的处理 */
        appctrl_period_change(i, turnmax, &(ptSubGroups[i]), ptCtrlArg);
        
        if(pcType != g_PowerInfo.bLastPCtrl[i])
        {
            uint8 bFirst = FALSE; //首次告警 不是告警切换
            if(g_PowerInfo.bLastPCtrl[i] > CTRL_PERIOD_TYPE)
            {
                bFirst = TRUE;
            }
            g_PowerTurnStop[i] = 0;
            //合闸，相关变量初始化
            for (j = 0; j < OOP_MAX_TURN; j++)
            {
                appctrl_power_close(g_PowerInfo.bLastPCtrl[i], i, j, OUTOFTIME_CLOSE);  //-合闸-
                g_PowerInfo.bFirstInit[i][j] = 0;   //-告警延时计数器首次赋初值复位-
                g_PowerInfo.bTwoMin[i][j] = 0;      //-2分钟冻结计时复位-
                g_PowerInfo.bfgFirstPrtl[i][j]=0;   //-首次补充2分钟后功控记录标志复位-
            }
            CTRL_FMT_DEBUG("总加组%d功控投入变化: 上次投入%s, 本次投入 %s\n", i + 1, 
                           g_PowerInfo.bLastPCtrl[i] <= CTRL_PERIOD_TYPE ? ctrlname[g_PowerInfo.bLastPCtrl[i]] : "无功控",
                           pcType <= CTRL_PERIOD_TYPE ? ctrlname[pcType] : "无功控");
            g_PowerInfo.bLastPCtrl[i] = pcType;//-保存即将进入的控制类型-
            if(FALSE == bFirst)
            {
                continue;
            }
        }

        /* 功控处理 */
        appctrl_step_deal(cid, i, turnmax, &(ptSubGroups[i]), ptCtrlArg);
    }
}

/**********************************************************************
* @name      : appctrl_power_status
* @brief     ：根据下浮控,报停控,厂休控,时段控等告警和跳闸输出状态计算出总控制状态
* @param[in] ：uint8 turnmax   最大轮次数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
void appctrl_power_status(uint8 turnmax)
{
    uint8 turn, grp;
    uint8 i;
    uint8 iDown, iStop, iRest, iPeriod;

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    OOP_TGCURSTATE_T curCtrlStatus; //0x230x1100总加组当前控制状态 
    OOP_TURNSTATES_T outputDown;//控制输出状态
    OOP_TGSTATES_T   alarmDown; //越限告警状态
    OOP_TURNSTATES_T outputStop;//控制输出状态
    OOP_TGSTATES_T   alarmStop; //越限告警状态
    OOP_TURNSTATES_T outputRest;//控制输出状态
    OOP_TGSTATES_T   alarmRest; //越限告警状态
    OOP_TURNSTATES_T outputPeriod;//控制输出状态
    OOP_TGSTATES_T   alarmPeriod; //越限告警状态

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    /* 控制状态汇总 */
    for(turn = 0; turn < turnmax; turn++)
    {
        for(grp = 0; grp < OOP_MAX_TG; grp++)
        {
            if(TURN_ON == appctrl_get_switch_status(CTRL_DOWN_TYPE, CTRL_ALARM_STORE, turn, grp)
               || TURN_ON == appctrl_get_switch_status(CTRL_STOP_TYPE, CTRL_ALARM_STORE, turn, grp)
               || TURN_ON == appctrl_get_switch_status(CTRL_REST_TYPE, CTRL_ALARM_STORE, turn, grp)
               || TURN_ON == appctrl_get_switch_status(CTRL_PERIOD_TYPE, CTRL_ALARM_STORE, turn, grp))
            {    //-任意一种功控类型处于告警状态-
                appctrl_set_switch_status(CTRL_POWER_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_ON);
            }
            else
            {
                appctrl_set_switch_status(CTRL_POWER_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_OFF);
            }

            if(TURN_ON == appctrl_get_switch_status(CTRL_DOWN_TYPE, CTRL_STATUS_STORE, turn, grp)
               || TURN_ON == appctrl_get_switch_status(CTRL_STOP_TYPE, CTRL_STATUS_STORE, turn, grp)
               || TURN_ON == appctrl_get_switch_status(CTRL_REST_TYPE, CTRL_STATUS_STORE, turn, grp)
               || TURN_ON == appctrl_get_switch_status(CTRL_PERIOD_TYPE, CTRL_STATUS_STORE, turn, grp))
            {   //-任意一种功控类型处于跳闸状态-
                appctrl_set_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_ON);
            }
            else
            {
                appctrl_set_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_OFF);
            }
        }
    }

    /* 控制输出状态 越限告警状态初始化 */
    memset(&outputDown, 0, sizeof(OOP_TURNSTATES_T));
    memset(&alarmDown, 0, sizeof(OOP_TGSTATES_T));
    outputDown.nNum = ptCtrlArg->tDown.effectStatus.nNum;
    alarmDown.nNum = ptCtrlArg->tDown.effectStatus.nNum;
    for(i = 0; i < ptCtrlArg->tDown.effectStatus.nNum; i++)
    {
        outputDown.item[i].obj = ptCtrlArg->tDown.effectStatus.item[i].obj;
        alarmDown.item[i].obj = ptCtrlArg->tDown.effectStatus.item[i].obj;
    }

    memset(&outputStop, 0, sizeof(OOP_TURNSTATES_T));
    memset(&alarmStop, 0, sizeof(OOP_TGSTATES_T));
    outputStop.nNum = ptCtrlArg->tStop.effectStatus.nNum;
    alarmStop.nNum = ptCtrlArg->tStop.effectStatus.nNum;
    for(i = 0; i < ptCtrlArg->tStop.effectStatus.nNum; i++)
    {
        outputStop.item[i].obj = ptCtrlArg->tStop.effectStatus.item[i].obj;
        alarmStop.item[i].obj = ptCtrlArg->tStop.effectStatus.item[i].obj;
    }

    memset(&outputRest, 0, sizeof(OOP_TURNSTATES_T));
    memset(&alarmRest, 0, sizeof(OOP_TGSTATES_T));
    outputRest.nNum = ptCtrlArg->tRest.effectStatus.nNum;
    alarmRest.nNum = ptCtrlArg->tRest.effectStatus.nNum;
    for(i = 0; i < ptCtrlArg->tRest.effectStatus.nNum; i++)
    {
        outputRest.item[i].obj = ptCtrlArg->tRest.effectStatus.item[i].obj;
        alarmRest.item[i].obj = ptCtrlArg->tRest.effectStatus.item[i].obj;
    }

    memset(&outputPeriod, 0, sizeof(OOP_TURNSTATES_T));
    memset(&alarmPeriod, 0, sizeof(OOP_TGSTATES_T));
    outputPeriod.nNum = ptCtrlArg->tPeriod.effectStatus.nNum;
    alarmPeriod.nNum = ptCtrlArg->tPeriod.effectStatus.nNum;
    for(i = 0; i < ptCtrlArg->tPeriod.effectStatus.nNum; i++)
    {
        outputPeriod.item[i].obj = ptCtrlArg->tPeriod.effectStatus.item[i].obj;
        alarmPeriod.item[i].obj = ptCtrlArg->tPeriod.effectStatus.item[i].obj;
    }

    /* 单个控制状态整理 总加组 功控 */
    for(grp = 0; grp < OOP_MAX_TG; grp++)
    {
        //先拷贝出来，不修改电控状态
        memcpy(&curCtrlStatus, &(ptSubGroups[grp].curCtrlStatus), sizeof(OOP_TGCURSTATE_T));
        curCtrlStatus.pwSetting = g_PowerInfo.bLastPCtrl[grp] > CTRL_PERIOD_TYPE ? ptCtrlArg->safevalue : g_PowerInfo.wPowerSetValue[grp];
        curCtrlStatus.pwFall = (TRUE == ptCtrlArg->tDown.bEffect[grp]) ? ptCtrlArg->tDown.project[grp].scheme.floatCoefficient : 0;
        curCtrlStatus.pcTrip.nValue = 0;
        curCtrlStatus.pcWarn.nValue = 0;

        if(0 == ptSubGroups[grp].ctrlStatus.pcTurn.nValue)
        {
            appctrl_write_subgroupstatus(grp, &curCtrlStatus);
            continue;
        }

        for(iDown = 0; iDown < OOP_MAX_TG; iDown++)
        {
            if(ptCtrlArg->tDown.effectStatus.item[iDown].obj == 0x2301 + grp)
            {
                break;
            }
        }
        for(iStop = 0; iStop < OOP_MAX_TG; iStop++)
        {
            if(ptCtrlArg->tStop.effectStatus.item[iStop].obj == 0x2301 + grp)
            {
                break;
            }
        }
        for(iRest = 0; iRest < OOP_MAX_TG; iRest++)
        {
            if(ptCtrlArg->tRest.effectStatus.item[iRest].obj == 0x2301 + grp)
            {
                break;
            }
        }
        for(iPeriod = 0; iPeriod < OOP_MAX_TG; iPeriod++)
        {
            if(ptCtrlArg->tPeriod.effectStatus.item[iPeriod].obj == 0x2301 + grp)
            {
                break;
            }
        }

        for(turn = 0; turn < turnmax; turn++)
        {
            /* 当前功率下浮控状态 */
            if(iDown < OOP_MAX_TG)
            {
                if(TURN_ON == appctrl_get_switch_status(CTRL_DOWN_TYPE, CTRL_STATUS_STORE, turn, grp))
                {
                    outputDown.item[iDown].state.nValue |= (0x01 << turn);
                }
                if(TURN_ON == appctrl_get_switch_status(CTRL_DOWN_TYPE, CTRL_ALARM_STORE, turn, grp))
                {
                    alarmDown.item[iDown].state = 1;
                }
            }
            /* 营业报停控状态 */
            if(iStop < OOP_MAX_TG)
            {
                if(TURN_ON == appctrl_get_switch_status(CTRL_STOP_TYPE, CTRL_STATUS_STORE, turn, grp))
                {
                    outputStop.item[iStop].state.nValue |= (0x01 << turn);
                }
                if(TURN_ON == appctrl_get_switch_status(CTRL_STOP_TYPE, CTRL_ALARM_STORE, turn, grp))
                {
                    alarmStop.item[iStop].state = 0x01;
                }
            }
            /* 厂休控状态 */
            if(iRest < OOP_MAX_TG)
            {
                if(TURN_ON == appctrl_get_switch_status(CTRL_REST_TYPE, CTRL_STATUS_STORE, turn, grp))
                {
                    outputRest.item[iRest].state.nValue |= (0x01 << turn);
                }
                if(TURN_ON == appctrl_get_switch_status(CTRL_REST_TYPE, CTRL_ALARM_STORE, turn, grp))
                {
                    alarmRest.item[iRest].state = 0x01;
                }
            }
            /* 时段控状态 */
            if(iPeriod < OOP_MAX_TG)
            {
                if(TURN_ON == appctrl_get_switch_status(CTRL_PERIOD_TYPE, CTRL_STATUS_STORE, turn, grp))
                {
                    outputPeriod.item[iPeriod].state.nValue |= (0x01 << turn);
                }
                if(TURN_ON == appctrl_get_switch_status(CTRL_PERIOD_TYPE, CTRL_ALARM_STORE, turn, grp))
                {
                    alarmPeriod.item[iPeriod].state = 0x01;
                }
            }

            /* 总加组状态 */
            if(TURN_ON == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_STATUS_STORE, turn, grp))
            {
                curCtrlStatus.pcTrip.nValue |= (0x01 << turn);
            }
            if(TURN_ON == appctrl_get_switch_status(CTRL_POWER_TYPE, CTRL_ALARM_STORE, turn, grp))
            {
                switch(g_PowerInfo.bPcKind[grp])
                {
                    case CTRL_DOWN_TYPE:
                    {
                        curCtrlStatus.pcWarn.nValue |= (0x01 << 3);
                        break;
                    }
                    case CTRL_STOP_TYPE:
                    {
                        curCtrlStatus.pcWarn.nValue |= (0x01 << 2);
                        break;
                    }
                    case CTRL_REST_TYPE:
                    {
                        curCtrlStatus.pcWarn.nValue |= (0x01 << 1);
                        break;
                    }
                    case CTRL_PERIOD_TYPE:
                    {
                        curCtrlStatus.pcWarn.nValue |= 0x01;
                        break;
                    }
                    default: break;
                }
            }
        }
        appctrl_write_subgroupstatus(grp, &curCtrlStatus);
    }
    appctrl_write_outstatus(CTRL_DOWN_TYPE, &outputDown);
    appctrl_write_alarmstatus(CTRL_DOWN_TYPE, &alarmDown);
    appctrl_write_outstatus(CTRL_STOP_TYPE, &outputStop);
    appctrl_write_alarmstatus(CTRL_STOP_TYPE, &alarmStop);
    appctrl_write_outstatus(CTRL_REST_TYPE, &outputRest);
    appctrl_write_alarmstatus(CTRL_REST_TYPE, &alarmRest);
    appctrl_write_outstatus(CTRL_PERIOD_TYPE, &outputPeriod);
    appctrl_write_alarmstatus(CTRL_PERIOD_TYPE, &alarmPeriod);
}

/**********************************************************************
* @name      : appctrl_down_guishow
* @brief     ：下浮控告警和跳闸输出桌面显示
* @param[in] ：uint8 group     总加组
               uint8 turnmax   最大轮次数
               uint16 buflen   缓存大小
* @param[out]：char* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
uint8 appctrl_down_guishow(uint8 group, uint8 turnmax, char* pBuf, uint16 buflen, uint16* msglen, int pinx)
{
    uint16 offset = 0;
    uint8  devStatus = 0;
    uint8  i = 0;
    char  *turnName[] = {"一轮", "二轮", "三轮", "四轮", "五轮", "六轮", "七轮", "八轮"};
    char   buftrip[100] = {0};
    uint16 offtrip = 0;
    char   buftime[100] = {0};
    uint16 offtime = 0;
    uint8  bShow = 1;
    time_t now;
    uint8  linenum = 2;
    uint32 timetotal = 0; //用于判断是否画面显示结束

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    time(&now);
    bShow = 1 - (now % 2);

    if(pinx <= 160)
    {
        offset = sprintf(pBuf, "      下浮控  控制组 %d\n", group + 1);

        for(i = 0; i < turnmax; i++)
        {
            if(0 != (i % linenum))
            {
                offtrip += sprintf(buftrip + offtrip, "    ");
                offtime += sprintf(buftime + offtime, "    ");
            }
            
            offtrip += sprintf(buftrip + offtrip, "%s:", turnName[i]);
            offtime += sprintf(buftime + offtime, "     ");
            if(CTRL_EFFECT == g_PowerInfo.bPcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
            {
                /* 状态显示 */
                devStatus = appctrl_get_switch_status(CTRL_DOWN_TYPE, CTRL_STATUS_STORE, i, group);
                if(TURN_ON == devStatus)
                {
                    offtrip += sprintf(buftrip + offtrip, "%s", "拉闸");
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                else
                {
                    if(0 == g_PowerInfo.wPcAlarmDelayss[group][i] || 1 == bShow)
                    {
                        offtrip += sprintf(buftrip + offtrip, "%s", "合闸");
                    }
                    else
                    {
                        offtrip += sprintf(buftrip + offtrip, "    ");
                    }
                }
                if(FALSE == ptCtrlArg->tDown.bEffect[group])
                {
                    /* 控制解除 */
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                /* 时间显示 */
                offtime += sprintf(buftime + offtime, "%02d:%02d", g_PowerInfo.wPcAlarmDelayss[group][i] / 60,
                                   g_PowerInfo.wPcAlarmDelayss[group][i] % 60);
                timetotal += g_PowerInfo.wPcAlarmDelayss[group][i];
            }
            else
            {
                offtrip += sprintf(buftrip + offtrip, "----");
                offtime += sprintf(buftime + offtime, "     ");
     
            }
            if(linenum - 1 == (i % linenum))
            {
                offset += sprintf(pBuf + offset, "\n%s", buftrip);
                offset += sprintf(pBuf + offset, "\n%s", buftime);
                bzero(buftrip, 100);
                bzero(buftime, 100);
                offtrip = 0;
                offtime = 0;
            }
        }
    }
    else
    {
        linenum = 4;
        offset = sprintf(pBuf, "\n            下浮控  控制组 %d", group + 1);
    
        for(i = 0; i < turnmax; i++)
        {
            if(0 != (i % linenum))
            {
                offtrip += sprintf(buftrip + offtrip, " ");
                offtime += sprintf(buftime + offtime, "    ");
            }
            
            offtrip += sprintf(buftrip + offtrip, "%s:", turnName[i]);
            offtime += sprintf(buftime + offtime, " ");
            if(CTRL_EFFECT == g_PowerInfo.bPcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
            {
                /* 状态显示 */
                devStatus = appctrl_get_switch_status(CTRL_DOWN_TYPE, CTRL_STATUS_STORE, i, group);
                if(TURN_ON == devStatus)
                {
                    offtrip += sprintf(buftrip + offtrip, "%s", "拉闸");
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                else
                {
                    if(0 == g_PowerInfo.wPcAlarmDelayss[group][i] || 1 == bShow)
                    {
                        offtrip += sprintf(buftrip + offtrip, "%s", "合闸");
                    }
                    else
                    {
                        offtrip += sprintf(buftrip + offtrip, "    ");
                    }
                }
                if(FALSE == ptCtrlArg->tDown.bEffect[group])
                {
                    /* 控制解除 */
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                /* 时间显示 */
                offtime += sprintf(buftime + offtime, "%02d:%02d", g_PowerInfo.wPcAlarmDelayss[group][i] / 60,
                                   g_PowerInfo.wPcAlarmDelayss[group][i] % 60);
                timetotal += g_PowerInfo.wPcAlarmDelayss[group][i];
            }
            else
            {
                offtrip += sprintf(buftrip + offtrip, "---");
                offtime += sprintf(buftime + offtime, "   ");
    
            }
            if(linenum - 1 == (i % linenum))
            {
                offset += sprintf(pBuf + offset, "\n%s", buftrip);
                offset += sprintf(pBuf + offset, "\n%s", buftime);
                bzero(buftrip, 100);
                bzero(buftime, 100);
                offtrip = 0;
                offtime = 0;
            }
        }
    }

    if(0 != offtrip)
    {
        offset += sprintf(pBuf + offset, "\n%s", buftrip);
        offset += sprintf(pBuf + offset, "\n%s", buftime);
    }

    /* 定值 */
    if(linenum < 3)
    {
        offset += sprintf(pBuf + offset, "\n");
    }
    if(CTRL_DOWN_TYPE != g_PowerInfo.bPcKind[group])
    {
        offset += sprintf(pBuf + offset, "\n定值    : ---- kW\n");
    }
    else
    {
        offset += sprintf(pBuf + offset, "\n定值    : %lld.%04lldkW\n", 
                          g_PowerInfo.wPowerSetValue[group]/10000, g_PowerInfo.wPowerSetValue[group]%10000);
    }
    offset += sprintf(pBuf + offset, "滑差功率: %lld.%04lldkW", 
                      ptSubGroups[group].aveActPower/10000, ptSubGroups[group].aveActPower%10000);

    *msglen = offset;
    return timetotal > 0 ? TRUE : FALSE;
}

/**********************************************************************
* @name      : appctrl_stop_guishow
* @brief     ：营业报停控告警和跳闸输出桌面显示
* @param[in] ：uint8 group     总加组
               uint8 turnmax   最大轮次数
               uint16 buflen   缓存大小
* @param[out]：char* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
uint8 appctrl_stop_guishow(uint8 group, uint8 turnmax, char* pBuf, uint16 buflen, uint16* msglen, int pinx)
{
    uint16 offset = 0;
    uint8  devStatus = 0;
    uint8  i = 0;
    char  *turnName[] = {"一轮", "二轮", "三轮", "四轮", "五轮", "六轮", "七轮", "八轮"};
    char   buftrip[100] = {0};
    uint16 offtrip = 0;
    char   buftime[100] = {0};
    uint16 offtime = 0;
    uint8  bShow = 1;
    time_t now;
    uint8  linenum = 2;
    uint32 timetotal = 0; //用于判断是否画面显示结束

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    time(&now);
    bShow = 1 - (now % 2);

    if(pinx <= 160)
    {
        offset = sprintf(pBuf, "      报停控  控制组 %d\n", group + 1);
        /* 获得月电控输出状态 */
        for(i = 0; i < turnmax; i++)
        {
            if(0 != (i % linenum))
            {
                offtrip += sprintf(buftrip + offtrip, "    ");
                offtime += sprintf(buftime + offtime, "    ");
            }
            
            offtrip += sprintf(buftrip + offtrip, "%s: ", turnName[i]);
            offtime += sprintf(buftime + offtime, "     ");
            if(CTRL_EFFECT == g_PowerInfo.bPcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
            {
                /* 状态显示 */
                devStatus = appctrl_get_switch_status(CTRL_STOP_TYPE, CTRL_STATUS_STORE, i, group);
                if(TURN_ON == devStatus)
                {
                    offtrip += sprintf(buftrip + offtrip, "%s", "拉闸");
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                else
                {
                    if(0 == g_PowerInfo.wPcAlarmDelayss[group][i] || 1 == bShow)
                    {
                        offtrip += sprintf(buftrip + offtrip, "%s", "合闸");
                    }
                    else
                    {
                        offtrip += sprintf(buftrip + offtrip, "    ");
                    }
                }
                if(FALSE == ptCtrlArg->tStop.bEffect[group])
                {
                    /* 控制解除 */
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
    
                /* 时间显示 */
                offtime += sprintf(buftime + offtime, "%02d:%02d", g_PowerInfo.wPcAlarmDelayss[group][i] / 60,
                                   g_PowerInfo.wPcAlarmDelayss[group][i] % 60);
                timetotal += g_PowerInfo.wPcAlarmDelayss[group][i];
            }
            else
            {
                offtrip += sprintf(buftrip + offtrip, "----");
                offtime += sprintf(buftime + offtime, "     ");
    
            }
            if(linenum - 1 == (i % linenum))
            {
                offset += sprintf(pBuf + offset, "\n%s", buftrip);
                offset += sprintf(pBuf + offset, "\n%s", buftime);
                bzero(buftrip, 100);
                bzero(buftime, 100);
                offtrip = 0;
                offtime = 0;
            }
        }
        if(offtrip > 0)
        {
            offset += sprintf(pBuf + offset, "\n%s", buftrip);
            offset += sprintf(pBuf + offset, "\n%s", buftime);
        }
    
    }
    else
    {
        linenum = 4;
        offset = sprintf(pBuf, "\n            报停控  控制组 %d", group + 1);
        /* 获得月电控输出状态 */
        for(i = 0; i < turnmax; i++)
        {
            if(0 != (i % linenum))
            {
                offtrip += sprintf(buftrip + offtrip, " ");
                offtime += sprintf(buftime + offtime, "    ");
            }
            
            offtrip += sprintf(buftrip + offtrip, "%s:", turnName[i]);
            offtime += sprintf(buftime + offtime, " ");
            if(CTRL_EFFECT == g_PowerInfo.bPcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
            {
                /* 状态显示 */
                devStatus = appctrl_get_switch_status(CTRL_STOP_TYPE, CTRL_STATUS_STORE, i, group);
                if(TURN_ON == devStatus)
                {
                    offtrip += sprintf(buftrip + offtrip, "%s", "拉闸");
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                else
                {
                    if(0 == g_PowerInfo.wPcAlarmDelayss[group][i] || 1 == bShow)
                    {
                        offtrip += sprintf(buftrip + offtrip, "%s", "合闸");
                    }
                    else
                    {
                        offtrip += sprintf(buftrip + offtrip, "    ");
                    }
                }
                if(FALSE == ptCtrlArg->tStop.bEffect[group])
                {
                    /* 控制解除 */
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
    
                /* 时间显示 */
                offtime += sprintf(buftime + offtime, "%02d:%02d", g_PowerInfo.wPcAlarmDelayss[group][i] / 60,
                                   g_PowerInfo.wPcAlarmDelayss[group][i] % 60);
                timetotal += g_PowerInfo.wPcAlarmDelayss[group][i];
            }
            else
            {
                offtrip += sprintf(buftrip + offtrip, "----");
                offtime += sprintf(buftime + offtime, "    ");
    
            }
            if(linenum - 1 == (i % linenum))
            {
                offset += sprintf(pBuf + offset, "\n%s", buftrip);
                offset += sprintf(pBuf + offset, "\n%s", buftime);
                bzero(buftrip, 100);
                bzero(buftime, 100);
                offtrip = 0;
                offtime = 0;
            }
        }
        if(offtrip > 0)
        {
            offset += sprintf(pBuf + offset, "\n%s", buftrip);
            offset += sprintf(pBuf + offset, "\n%s", buftime);
        }
    }

    /* 定值 */
    char tmpBuf[30]={0};
    if(linenum < 3)
    {
        offset += sprintf(pBuf + offset, "\n");
    }
    if(CTRL_STOP_TYPE != g_PowerInfo.bPcKind[group])
    {
        offset += sprintf(pBuf + offset, "\n定值    : ---- kW\n");
    }
    else
    {
        valueDisplayProcess_strUnitPower(g_PowerInfo.wPowerSetValue[group], tmpBuf, 30, 4,"W");
        offset += sprintf(pBuf + offset, "\n定值    : %s\n",tmpBuf);
                          
    }
    memset(tmpBuf,0,30);
    valueDisplayProcess_strUnitPower(ptSubGroups[group].actPower, tmpBuf, 30, 4,"W");
    offset += sprintf(pBuf + offset, "当前功率: %s\n",tmpBuf);

    *msglen = offset;
    return timetotal > 0 ? TRUE : FALSE;
}

/**********************************************************************
* @name      : appctrl_rest_guishow
* @brief     ：厂休控告警和跳闸输出桌面显示
* @param[in] ：uint8 group     总加组
               uint8 turnmax   最大轮次数
               uint16 buflen   缓存大小
* @param[out]：char* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
uint8 appctrl_rest_guishow(uint8 group, uint8 turnmax, char* pBuf, uint16 buflen, uint16* msglen, int pinx)
{
    uint16 offset = 0;
    uint8  devStatus = 0;
    uint8  i = 0;
    char  *turnName[] = {"一轮", "二轮", "三轮", "四轮", "五轮", "六轮", "七轮", "八轮"};
    char   buftrip[100] = {0};
    uint16 offtrip = 0;
    char   buftime[100] = {0};
    uint16 offtime = 0;
    uint8  bShow = 1;
    time_t now;
    uint8  linenum = 2;
    uint32 timetotal = 0; //用于判断是否画面显示结束

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    time(&now);
    bShow = 1 - (now % 2);

    if(pinx <= 160)
    {
        offset = sprintf(pBuf, "      厂休控  控制组 %d\n", group + 1);
        /* 获得月电控输出状态 */
        for(i = 0; i < turnmax; i++)
        {
            if(0 != (i % linenum))
            {
                offtrip += sprintf(buftrip + offtrip, "    ");
                offtime += sprintf(buftime + offtime, "    ");
            }
            
            offtrip += sprintf(buftrip + offtrip, "%s: ", turnName[i]);
            offtime += sprintf(buftime + offtime, "     ");
            if(CTRL_EFFECT == g_PowerInfo.bPcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
            {
                /* 状态显示 */
                devStatus = appctrl_get_switch_status(CTRL_REST_TYPE, CTRL_STATUS_STORE, i, group);
                if(TURN_ON == devStatus)
                {
                    offtrip += sprintf(buftrip + offtrip, "%s", "拉闸");
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                else
                {
                    if(0 == g_PowerInfo.wPcAlarmDelayss[group][i] || 1 == bShow)
                    {
                        offtrip += sprintf(buftrip + offtrip, "%s", "合闸");
                    }
                    else
                    {
                        offtrip += sprintf(buftrip + offtrip, "    ");
                    }
                }
                if(FALSE == ptCtrlArg->tRest.bEffect[group])
                {
                    /* 控制解除 */
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                /* 时间显示 */
                offtime += sprintf(buftime + offtime, "%02d:%02d", g_PowerInfo.wPcAlarmDelayss[group][i] / 60,
                                   g_PowerInfo.wPcAlarmDelayss[group][i] % 60);
                timetotal += g_PowerInfo.wPcAlarmDelayss[group][i];
            }
            else
            {
                offtrip += sprintf(buftrip + offtrip, "----");
                offtime += sprintf(buftime + offtime, "     ");
     
            }
            if(linenum - 1 == (i % linenum))
            {
                offset += sprintf(pBuf + offset, "\n%s", buftrip);
                offset += sprintf(pBuf + offset, "\n%s", buftime);
                bzero(buftrip, 100);
                bzero(buftime, 100);
                offtrip = 0;
                offtime = 0;
            }
        }
    }
    else
    {
        linenum = 4;
        offset = sprintf(pBuf, "\n            厂休控  控制组 %d", group + 1);
        /* 获得月电控输出状态 */
        for(i = 0; i < turnmax; i++)
        {
            if(0 != (i % linenum))
            {
                offtrip += sprintf(buftrip + offtrip, " ");
                offtime += sprintf(buftime + offtime, "     ");
            }
            
            offtrip += sprintf(buftrip + offtrip, "%s:", turnName[i]);
            offtime += sprintf(buftime + offtime, " ");
            if(CTRL_EFFECT == g_PowerInfo.bPcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
            {
                /* 状态显示 */
                devStatus = appctrl_get_switch_status(CTRL_REST_TYPE, CTRL_STATUS_STORE, i, group);
                if(TURN_ON == devStatus)
                {
                    offtrip += sprintf(buftrip + offtrip, "%s", "拉闸");
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                else
                {
                    if(0 == g_PowerInfo.wPcAlarmDelayss[group][i] || 1 == bShow)
                    {
                        offtrip += sprintf(buftrip + offtrip, "%s","合闸");
                    }
                    else
                    {
                        offtrip += sprintf(buftrip + offtrip, "    ");
                    }
                }
                if(FALSE == ptCtrlArg->tRest.bEffect[group])
                {
                    /* 控制解除 */
                    g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
                }
                /* 时间显示 */
                offtime += sprintf(buftime + offtime, "%02d:%02d", g_PowerInfo.wPcAlarmDelayss[group][i] / 60,
                                   g_PowerInfo.wPcAlarmDelayss[group][i] % 60);
                timetotal += g_PowerInfo.wPcAlarmDelayss[group][i];
            }
            else
            {
                offtrip += sprintf(buftrip + offtrip, "---");
                offtime += sprintf(buftime + offtime, "    ");
     
            }
            if(linenum - 1 == (i % linenum))
            {
                offset += sprintf(pBuf + offset, "\n%s", buftrip);
                offset += sprintf(pBuf + offset, "\n%s", buftime);
                bzero(buftrip, 100);
                bzero(buftime, 100);
                offtrip = 0;
                offtime = 0;
            }
        }
    }

    if(offtrip > 0)
    {
        offset += sprintf(pBuf + offset, "\n%s", buftrip);
        offset += sprintf(pBuf + offset, "\n%s", buftime);
    }

    /* 定值 */
    if(linenum < 3)
    {
        offset += sprintf(pBuf + offset, "\n");
    }
    
    if(CTRL_REST_TYPE != g_PowerInfo.bPcKind[group])
    {
        offset += sprintf(pBuf + offset, "\n定值    : ---- kW\n");
    }
    else
    {
        offset += sprintf(pBuf + offset, "\n定值    : %lld.%04lldkW\n", 
                          g_PowerInfo.wPowerSetValue[group]/10000, g_PowerInfo.wPowerSetValue[group]%10000);
    }
    offset += sprintf(pBuf + offset, "当前功率: %lld.%04lldkW\n", 
                      ptSubGroups[group].actPower/10000, ptSubGroups[group].actPower%10000);

    *msglen = offset;
    return timetotal > 0 ? TRUE : FALSE;
}

/**********************************************************************
* @name      : appctrl_period_guishow
* @brief     ：时段功控告警和跳闸输出桌面显示
* @param[in] ：uint8 group     总加组
               uint8 turnmax   最大轮次数
               uint16 buflen   缓存大小
* @param[out]：char* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
uint8 appctrl_period_guishow(uint8 group, uint8 turnmax, char* pBuf, uint16 buflen, uint16* msglen, int pinx)
{
    uint16 offset = 0;
    uint8  devStatus = 0;
    uint8  i = 0;
    char  *turnName[] = {"一轮", "二轮", "三轮", "四轮", "五轮", "六轮", "七轮", "八轮"};
    char   buftrip[100] = {0};
    uint16 offtrip = 0;
    char   buftime[100] = {0};
    uint16 offtime = 0;
    uint8  bShow = 1;
    time_t now;
    uint8  linenum = 2;
    uint32 timetotal = 0; //用于判断是否画面显示结束

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    time(&now);
    bShow = 1 - (now % 2);

    if(pinx <= 160)
    {
        offset = sprintf(pBuf, "      时段控  控制组 %d\n", group + 1);
    }
    else
    {
        linenum = 4;
        offset = sprintf(pBuf, "\n            时段控  控制组 %d", group + 1);
    }
    
    /* 获得月电控输出状态 */
    for(i = 0; i < turnmax; i++)
    {
        if (pinx > 160){
            if(0 != (i % linenum))
            {
                offtrip += sprintf(buftrip + offtrip, " ");
                offtime += sprintf(buftime + offtime, " ");
            }
            offtrip += sprintf(buftrip + offtrip, "%s:", turnName[i]);
            offtime += sprintf(buftime + offtime, "    ");
        }else{
            if(0 != (i % linenum))
            {
                offtrip += sprintf(buftrip + offtrip, "    ");
                offtime += sprintf(buftime + offtime, "    ");
            }
            offtrip += sprintf(buftrip + offtrip, "%s: ", turnName[i]);
            offtime += sprintf(buftime + offtime, "     ");
        }
        if(CTRL_EFFECT == g_PowerInfo.bPcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
        {
            /* 状态显示 */
            devStatus = appctrl_get_switch_status(CTRL_PERIOD_TYPE, CTRL_STATUS_STORE, i, group);
            if(TURN_ON == devStatus)
            {
                offtrip += sprintf(buftrip + offtrip, "%s", "拉闸");
                g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
            }
            else
            {
                if(0 == g_PowerInfo.wPcAlarmDelayss[group][i] || 1 == bShow)
                {
                    offtrip += sprintf(buftrip + offtrip, "%s", "合闸");
                }
                else
                {
                    offtrip += sprintf(buftrip + offtrip, "    ");
                }
            }
            if(FALSE == ptCtrlArg->tPeriod.bEffect[group])
            {
                /* 控制解除 */
                g_PowerInfo.wPcAlarmDelayss[group][i] = 0;
            }
            /* 时间显示 */
            offtime += sprintf(buftime + offtime, "%02d:%02d", g_PowerInfo.wPcAlarmDelayss[group][i] / 60,
                               g_PowerInfo.wPcAlarmDelayss[group][i] % 60);
            timetotal += g_PowerInfo.wPcAlarmDelayss[group][i];
        }
        else
        {
            offtrip += sprintf(buftrip + offtrip, "----");
            offtime += sprintf(buftime + offtime, "     ");

        }
        if(linenum - 1 == (i % linenum))
        {
            offset += sprintf(pBuf + offset, "\n%s", buftrip);
            offset += sprintf(pBuf + offset, "\n%s", buftime);
            bzero(buftrip, 100);
            bzero(buftime, 100);
            offtrip = 0;
            offtime = 0;
        }
    }
    if(offtrip > 0)
    {
        offset += sprintf(pBuf + offset, "\n%s", buftrip);
        offset += sprintf(pBuf + offset, "\n%s", buftime);
    }

    /* 定值 */
    if(linenum < 3)
    {
        offset += sprintf(pBuf + offset, "\n");
    }
    if(CTRL_PERIOD_TYPE != g_PowerInfo.bPcKind[group])
    {
        offset += sprintf(pBuf + offset, "\n定值    : ---- kW\n");
    }
    else
    {
        offset += sprintf(pBuf + offset, "\n定值    : %lld.%04lldkW\n", 
                          g_PowerInfo.wPowerSetValue[group]/10000, g_PowerInfo.wPowerSetValue[group]%10000);
    }
    offset += sprintf(pBuf + offset, "当前功率: %lld.%04lldkW\n", 
                      ptSubGroups[group].actPower/10000, ptSubGroups[group].actPower%10000);

    *msglen = offset;
    return timetotal > 0 ? TRUE : FALSE;
}

/**********************************************************************
* @name      : appctrl_power_guishow
* @brief     ：功率控制告警和跳闸输出桌面显示
* @param[in] ：uint8 ctrltype  控制类型
               uint8 group     总加组
               uint8 turnmax   最大轮次数
               uint16 buflen   缓存大小
* @param[out]：uint8* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-4-29
* @Update    :
**********************************************************************/
uint8 appctrl_power_guishow(uint8 ctrltype, uint8 group, uint8 turnmax, uint8* pBuf, uint16 buflen, uint16* msglen)
{
    uint8 status = FALSE;
#ifdef PRODUCT_ZCU_1
    int   pinx = 240;
#else
    int   pinx = 160;
#endif
    
    switch(ctrltype)
    {
        case CTRL_DOWN_TYPE:
        {
            status = appctrl_down_guishow(group, turnmax, (char *)pBuf, buflen, msglen, pinx);
            break;
        }
        case CTRL_STOP_TYPE:
        {
            status = appctrl_stop_guishow(group, turnmax, (char *)pBuf, buflen, msglen, pinx);
            break;
        }
        case CTRL_REST_TYPE:
        {
            status = appctrl_rest_guishow(group, turnmax, (char *)pBuf, buflen, msglen, pinx);
            break;
        }
        case CTRL_PERIOD_TYPE:
        {
            status = appctrl_period_guishow(group, turnmax, (char *)pBuf, buflen, msglen, pinx);
            break;
        }
        default: break;
    }
    return status;
}

/**********************************************************************
* @name      : appctrl_power_write
* @brief     ：功率控制中间状态写入私有数据
* @param[in] ：uint8 bDay  过日
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-5-7
* @Update    :
**********************************************************************/
void appctrl_power_write(uint8 bDay)
{
    int ret = 0;
    static uint8 s_PowerInfo[sizeof(POWER_INFO_T)] = {0};

    if(0 == memcmp(s_PowerInfo, &g_PowerInfo, sizeof(POWER_INFO_T)) && FALSE == bDay)
    {
        return;
    }
    memcpy(s_PowerInfo, &g_PowerInfo, sizeof(POWER_INFO_T));
    ret = lctrl_write_ctrl(CTRL_POWER_TYPE, (uint8 *)&g_PowerInfo, sizeof(POWER_INFO_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("保存功控中间数据错误, %d\n", ret);
    }
}

