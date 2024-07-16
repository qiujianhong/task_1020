/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块 调度线程
* @date：    2020-3-31
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "hal.h"
#include "uart.h"
#include "db_api.h"
#include "framecomm.h"

#include "appctrl_comm.h"
#include "loadctrl_appctrl.h"
#include "appctrl_cfg.h"
#include "appctrl_power.h"
#include "appctrl_elec.h"
#include "appctrl_remote.h"
#include "appctrl_protect.h"
#include "appctrl_dev.h"
#include "appctrl_alarm.h"


DB_CLIENT        g_CtrlDBClient = 0;
CTRL_CONDIG_T    g_CtrlCfg;          //控制模块物理配置

SWITCH_INTERFACE_T g_curSwitch;  //当前控制模块动作状态


/**********************************************************************
* @name      : appctrl_dev_input
* @brief     ：控制模块接入判断
* @param[in] ：uint8 turnno  轮次序号1-8
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2020-11-6
* @Update    :
**********************************************************************/
uint8 appctrl_dev_input(uint8 turnno)
{
    /* 默认有4轮 */
    if(turnno > 0 && turnno <= g_CtrlCfg.portNum)
    {
        return TRUE;
    }

    if(0 != g_CtrlCfg.nACM2)
    {
        if(turnno > g_CtrlCfg.portNum && turnno <= 4 * g_CtrlCfg.portNum)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**********************************************************************
* @name      : lctrl_set_ctrl_mode
* @brief     ：控制模组设置
* @param[in] ：uint8 port     端口
               uint8 subport  子端口
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2024-7-1
* @Update    :
**********************************************************************/
void lctrl_set_ctrl_mode(uint8 port, uint8 subport)
{
    g_CtrlCfg.nACM1 = port;
    g_CtrlCfg.nsubACM1 = subport;
    g_CtrlCfg.bModeOk = TRUE;
}

/**********************************************************************
* @name      : appctrl_effect_proc
* @brief     ：控制投入/解除处理
* @param[in] ：
* @param[out]：uint8 *ctrlType 控制类型
* @return    ：TRUE-控制投入;FALSE-控制解除
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
uint8 appctrl_effect_proc(uint8 *ctrlType)
{
    uint8    effectType = 0;
    uint8    effectStatus = 0;
    uint16   group = 0;
    char     buf[64] = {0};
    char     alarm[400] = {0};
    uint16   buflen = 0;
    char    *ctrlName[] = {"下浮控", "营业报停控", "厂休控", "时段控", "购电控", "月电控", "遥控", "保电"};
    uint8    i = 0;
    char    *turnName[] = {"一", "二", "三", "四", "五", "六", "七", "八"};
    char     bufPc[40] = {0};
    uint8    pclen = 0;
    char     bufEc[40] = {0};
    uint8    eclen = 0;
    int      indexno = 0;
    
    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计
    uint8            PwTurn = 0;
    uint8            EcTurn = 0;

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    effectStatus = appctrl_get_neweffect(&effectType, &group, &indexno);
    if(0 == effectStatus || effectType > CTRL_PROTCET_TYPE || 0 == group || group > OOP_MAX_TG)
    {
        *ctrlType = CTRL_NULL_TYPE;
        return FALSE;
    }
    appctrl_no_neweffect();
    appctrl_ctrl_buzzer(0, TRUE);
    appctrl_alarm_stop();                    //清除老告警
#if defined PRODUCT_ZCU_1 || defined PRODUCT_SEU
    appctrl_alarm_data(effectType, 0, 3, indexno, NULL);
#endif

    buflen = sprintf(buf, "%s%s", ctrlName[effectType], 1 == effectStatus ? "投入" : "解除");
    appctrl_show_guistatus(120, (uint8 *)buf, buflen);  //状态栏

    PwTurn = ptSubGroups[group-1].ctrlPwTurn.nValue;
    EcTurn = ptSubGroups[group-1].ctrlEcTurn.nValue;

    /* 显示新的告警界面 */
#ifdef PRODUCT_ZCU_1
    buflen = sprintf(alarm, "\n              总加组 %d 控制\n", group);
    buflen += sprintf(alarm + buflen, "         总");
#else
    buflen = sprintf(alarm, "      总加组 %d 控制\n\n", group);
    buflen += sprintf(alarm + buflen, "         总");
#endif
    for(i = 0; i < g_CtrlCfg.turnMAX; i++)
    {
        buflen += sprintf(alarm + buflen, " %s", turnName[i]);
        if(i < g_CtrlCfg.turnNum)
        {
            pclen += sprintf(bufPc + pclen, " %s", 0 == ((PwTurn >> i) & 1) ? "解" : "投");
            eclen += sprintf(bufEc + eclen, " %s", 0 == ((EcTurn >> i) & 1) ? "解" : "投");
        }
        else
        {
            pclen += sprintf(bufPc + pclen, " %s", "--");
            eclen += sprintf(bufEc + eclen, " %s", "--");
        }
    }
    buflen += sprintf(alarm + buflen, "\n");

    buflen += sprintf(alarm + buflen, " 下浮控  %s%s\n", TRUE == ptCtrlArg->tDown.bEffect[group-1] ? "投" : "解", bufPc);
    buflen += sprintf(alarm + buflen, " 报停控  %s%s\n", TRUE == ptCtrlArg->tStop.bEffect[group-1] ? "投" : "解", bufPc);
    buflen += sprintf(alarm + buflen, " 厂休控  %s%s\n", TRUE == ptCtrlArg->tRest.bEffect[group-1] ? "投" : "解", bufPc);
    buflen += sprintf(alarm + buflen, " 时段控  %s%s\n", TRUE == ptCtrlArg->tPeriod.bEffect[group-1] ? "投" : "解", bufPc);
    buflen += sprintf(alarm + buflen, " 月电控  %s%s\n", TRUE == ptCtrlArg->tMonth.bEffect[group-1] ? "投" : "解", bufEc);
    buflen += sprintf(alarm + buflen, " 购电控  %s%s\n", TRUE == ptCtrlArg->tBuy.bEffect[group-1] ? "投" : "解", bufEc);
    
    appctrl_show_guihint(120, (uint8*)alarm, buflen);
    CTRL_BUF_TRACE(alarm, buflen, "Send hint:");
    usleep(100000);
    appctrl_ctrl_buzzer(0, FALSE);
    
    *ctrlType = effectType;
    return 1 == effectStatus ? TRUE : FALSE;
}


/**********************************************************************
* @name      : appctrl_effect_proc
* @brief     ：控制投入/解除处理
* @param[in] ：
* @param[out]：uint8 *ctrlType 控制类型
* @return    ：TRUE-控制投入;FALSE-控制解除
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
uint8 appctrl_effect_proc_jiangsu(uint8 *ctrlType)
{
    uint8    effectType = 0;
    uint8    effectStatus = 0;
    uint16   group = 0;
    char     buf[64] = {0};
    char     alarm[400] = {0};
    uint16   buflen = 0;
    char    *ctrlName[] = {"下浮控", "营业报停控", "厂休控", "时段控", "购电控", "月电控", "遥控", "保电"};
    //uint8    i = 0;
    //char    *turnName[] = {"一", "二", "三", "四", "五", "六", "七", "八"};
    char     bufPc[40] = {0};
    //uint8    pclen = 0;
    char     bufEc[40] = {0};
    //uint8    eclen = 0;
    int      indexno = 0;
    
    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计
    //uint8            PwTurn = 0;
    //uint8            EcTurn = 0;

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    effectStatus = appctrl_get_neweffect(&effectType, &group, &indexno);
    if(0 == effectStatus || effectType > CTRL_PROTCET_TYPE || 0 == group || group > OOP_MAX_TG)
    {
        *ctrlType = CTRL_NULL_TYPE;
        return FALSE;
    }
    appctrl_no_neweffect();
    appctrl_ctrl_buzzer(0, TRUE);
    appctrl_alarm_stop();                    //清除老告警
#if defined PRODUCT_ZCU_1 || defined PRODUCT_SEU
    appctrl_alarm_data(effectType, 0, 3, indexno, NULL);
#endif

    buflen = sprintf(buf, "%s%s", ctrlName[effectType], 1 == effectStatus ? "投入" : "解除");
    appctrl_show_guistatus(120, (uint8 *)buf, buflen);  //状态栏

    //PwTurn = ptSubGroups[group-1].ctrlPwTurn.nValue;
    //EcTurn = ptSubGroups[group-1].ctrlEcTurn.nValue;

    /* 显示新的告警界面 */
#ifdef PRODUCT_ZCU_1
    buflen = sprintf(alarm, "\n              总加组 %d 控制\n", group);
    //buflen += sprintf(alarm + buflen, "         总");
#else
    buflen = sprintf(alarm, "      总加组 %d 控制\n\n", group);
    buflen += sprintf(alarm + buflen, "         总");
#endif
//    for(i = 0; i < g_CtrlCfg.turnMAX; i++)
//    {
//        buflen += sprintf(alarm + buflen, " %s", turnName[i]);
//        if(i < g_CtrlCfg.turnNum)
//        {
//            pclen += sprintf(bufPc + pclen, " %s", 0 == ((PwTurn >> i) & 1) ? "解" : "投");
//            eclen += sprintf(bufEc + eclen, " %s", 0 == ((EcTurn >> i) & 1) ? "解" : "投");
//        }
//        else
//        {
//            pclen += sprintf(bufPc + pclen, " %s", "--");
//            eclen += sprintf(bufEc + eclen, " %s", "--");
//        }
//    }
    buflen += sprintf(alarm + buflen, "\n");

    buflen += sprintf(alarm + buflen, " 下浮控             %s%s\n", TRUE == ptCtrlArg->tDown.bEffect[group-1] ? "投" : "解", bufPc);
    buflen += sprintf(alarm + buflen, " 报停控             %s%s\n", TRUE == ptCtrlArg->tStop.bEffect[group-1] ? "投" : "解", bufPc);
    buflen += sprintf(alarm + buflen, " 厂休控             %s%s\n", TRUE == ptCtrlArg->tRest.bEffect[group-1] ? "投" : "解", bufPc);
    buflen += sprintf(alarm + buflen, " 时段控             %s%s\n", TRUE == ptCtrlArg->tPeriod.bEffect[group-1] ? "投" : "解", bufPc);
    buflen += sprintf(alarm + buflen, " 月电控             %s%s\n", TRUE == ptCtrlArg->tMonth.bEffect[group-1] ? "投" : "解", bufEc);
    buflen += sprintf(alarm + buflen, " 购电控             %s%s\n", TRUE == ptCtrlArg->tBuy.bEffect[group-1] ? "投" : "解", bufEc);
    
    appctrl_show_guihint(120, (uint8*)alarm, buflen);
    CTRL_BUF_TRACE(alarm, buflen, "Send hint:");
    usleep(100000);
    appctrl_ctrl_buzzer(0, FALSE);
    
    *ctrlType = effectType;
    return 1 == effectStatus ? TRUE : FALSE;
}



/**********************************************************************
* @name      : loadctrl_ctrl_init
* @brief     ：控制初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-7-1
* @Update    :
**********************************************************************/
int loadctrl_ctrl_init(void *pPublic, void **ppPrivate)
{
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    uint8            i;

    memset(&g_CtrlCfg, 0, sizeof(CTRL_CONDIG_T));
    g_CtrlCfg.turnMAX = ptLCData->turnMax;
    g_CtrlCfg.portNum = ptLCData->modeturn;

    memset(&g_curSwitch, 0, sizeof(SWITCH_INTERFACE_T));
    for(i = 0; i < OOP_MAX_TURN; i++)
    {
        g_curSwitch.TURN_CTRL[i] = 1;
    }
    g_curSwitch.ALARM_OUTPUT = 1;
    g_curSwitch.portMax = ptLCData->turnMax;

    appctrl_switch_init(ptLCData->modeturn, ptLCData->turnMax);
    appctrl_power_init();
    appctrl_elec_init();
    appctrl_remote_init();
    appctrl_alarm_init(ptLCData->turnMax);

    /* 等待数据中心 */
    while(FALSE == ptLCData->hasDB)
    {
        usleep(100000);
    }

#ifdef PRODUCT_ZCU_1
    appctrl_switch_init_zcu_1();
    lctrl_set_ctrl_mode(1, 1);
#endif
#ifdef PRODUCT_ZCU_3
    lctrl_set_ctrl_mode(1, 1);
#endif

    CTRL_FMT_DEBUG("控制模块启动!\n");

    *ppPrivate = NULL;
    return 0;
}

/**********************************************************************
* @name      : loadctrl_ctrl_proc
* @brief     ：控制处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2024-7-1
* @Update    :
**********************************************************************/
int loadctrl_ctrl_proc(void *pArg, void *pPublic, void *pPrivate)
{
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    time_t           now;
    OOP_DATETIME_S_T curTime = {0};
    OOP_DATETIME_S_T lastTime = {0};
    uint8            bPwCtrlflag = FALSE;
    uint8            bEcCtrlflag = FALSE;
    uint8            bDayPass = FALSE;
    uint8            bPwDayPass = FALSE;
    uint8            bMonthPass = FALSE;
    uint32           Oad = 0;
    uint16           infoNum = 0;
    static uint8     monthPassTime = 0;
    uint8            ctrlType = CTRL_NULL_TYPE;
    uint8            effectType = 0;
    uint8            pwctrldeal = FALSE;
    uint8            ecctrldeal = FALSE;
    uint8            powerCheckTime = 5;
    uint8            elceCheckTime = 5;

    g_CtrlDBClient = db_client_create(DB_LOADCTRLLER_AGENT, DB_LOADCTRLLER_CTRL);
    if(g_CtrlDBClient <= 0)
    {
        printf("db_client_create failed!\n");
        return -1;
    }

    /* 数据中心更新清空 */
    Oad = lctrl_read_oad_notify(&(ptLCData->ctrlDbChg), &infoNum);
    while(0 != Oad)
    {
        Oad = lctrl_read_oad_notify(&(ptLCData->ctrlDbChg), &infoNum);
    }

    /* 控制参数首次读取 */
    appctrl_get_arg(g_CtrlDBClient);

    /* 上电保电处理 */
    appctrl_protect_first();
    fee_set_lock_init();

    appctrl_ctrl_led(EMLED_ALARM, FALSE);
    appctrl_ctrl_buzzer(g_CtrlDBClient, FALSE);

    appctrl_get_last_time(&lastTime);

    appctrl_remote_decress(&lastTime);
    appctrl_down_init();

    CTRL_FMT_DEBUG("控制模块开始工作!\n");
    while(1)
    {
        time(&now);
        DT_Time2DateTimeShort(now, &curTime);

        /* 0. 初始化 */
        if(TRUE == lctrl_get_dataflag(&(ptLCData->ctrlDbChg)))
        {
            appctrl_init_data();
        }
        if(TRUE == lctrl_get_argflag(&(ptLCData->ctrlDbChg)))
        {
            appctrl_init_arg();
            appctrl_remote_passday_init(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX) * 2); //参数初始化允许合闸
        }

        /* 1. 数据中心更新 */
        Oad = lctrl_read_oad_notify(&(ptLCData->ctrlDbChg), &infoNum);
        while(0 != Oad)
        {
#ifdef AREA_HEILONGJIANG
            /* 黑龙江在月电量更新的情况下, 下s执行电控 */
            if((Oad & 0xFFF0FFFF) == 0x23000900)
            {
                elceCheckTime = (curTime.second + 1) % 60;
                bEcCtrlflag = FALSE;
            }
#endif
            appctrl_update_db(Oad, infoNum);
            Oad = lctrl_read_oad_notify(&(ptLCData->ctrlDbChg), &infoNum);
        }

        /* 2. 清理 */
        bDayPass = FALSE;
        if(curTime.day != lastTime.day || curTime.month != lastTime.month || curTime.year != lastTime.year)
        {
            /* 过日清理 */
            appctrl_clear_daystatic();
            bDayPass = TRUE;
            bPwDayPass = TRUE;
        }
        if(curTime.month != lastTime.month || curTime.year != lastTime.year)
        {
            CTRL_FMT_DEBUG("过月处理!\n");
            appctrl_clear_monstatic();
            bMonthPass = TRUE;
            monthPassTime = 0;
        }

        /* 3. 保电时间维护 */
        if(curTime.minute != lastTime.minute)
        {
            appctrl_protect_ctrl_min();
        }

        /* 4. 规约数据整理 */
        if(curTime.second != lastTime.second)
        {
            appctrl_assign_status(g_CtrlDBClient);  //控制状态更新(投入、轮次)

            /* 保电 */
            appctrl_protect_ctrl();

            /* 投入解除告警 */
            pwctrldeal = FALSE;
            ecctrldeal = FALSE;
#ifdef AREA_JIANGSU
            effectType = appctrl_effect_proc_jiangsu(&ctrlType);
#else
            effectType = appctrl_effect_proc(&ctrlType);
#endif
            if(FALSE == effectType && ctrlType <= CTRL_PERIOD_TYPE)
            {
                pwctrldeal = TRUE;  //功控解除需要立刻执行
            }
            else if(FALSE == effectType && (CTRL_BUY_TYPE == ctrlType || CTRL_MONTH_TYPE == ctrlType))
            {
                ecctrldeal = TRUE;  //电控解除需要立刻执行
            }
#ifdef AREA_HEILONGJIANG
            else if(TRUE == effectType && ctrlType <= CTRL_PERIOD_TYPE)
            {
                powerCheckTime = (curTime.second > 5) ? curTime.second - 1 : 5;
                bPwCtrlflag = TRUE;
                CTRL_FMT_DEBUG("控制投入，调整功控检查时间%ds\n", powerCheckTime);
            }
#endif

            /* 功控显示计时 */
            appctrl_power_show_decress(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));

            /* 功控轮次投入解除 */
            appctrl_pcturn_change(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
        }

        /* 5. 功率控制 每分钟5s后执行电量控制 */
        if(TRUE == pwctrldeal || (curTime.second >= powerCheckTime && FALSE == bPwCtrlflag))
        {
            appctrl_power_ctrl(g_CtrlDBClient, bPwDayPass, CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            bPwCtrlflag = TRUE;
            bPwDayPass = FALSE;
            pwctrldeal = FALSE;
        }
        if(curTime.second < powerCheckTime)
        {
            bPwCtrlflag = FALSE;
        }

        /* 6. 电控 过分后5s或者过月后20s执行(过月统计刷新慢) */
        if(TRUE == ecctrldeal || (FALSE == bMonthPass && curTime.second >= elceCheckTime && FALSE == bEcCtrlflag) || (TRUE == bMonthPass && monthPassTime > 20))
        {
            appctrl_elec_ctrl(g_CtrlDBClient, bMonthPass, CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            bMonthPass = FALSE;
            bEcCtrlflag = TRUE;
            ecctrldeal = FALSE;
        }
        if(curTime.second < elceCheckTime)
        {
            bEcCtrlflag = FALSE;
        }

        if(curTime.second != lastTime.second)
        {
            monthPassTime++; //过月计时

            /* 7. 遥控 遥控加上合闸需要增加1倍 */
            appctrl_remote_ctrl(g_CtrlDBClient, bDayPass, CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX) * 2);

            /* 8. 催费告警 */
            appctrl_callfee_ctrl();

            /* 9. 控制中间状态汇集 处理告警，记录相关日志  */
            appctrl_power_status(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            appctrl_elec_status(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            appctrl_remote_status(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));
            appctrl_control_state(g_CtrlCfg.nACM1, g_CtrlCfg.nACM2, &g_curSwitch); //-获取总控制状态,并填写告警信息-

            /* 10. 写中间数据 */
            appctrl_power_write(curTime.day != lastTime.day);
            appctrl_elec_write(curTime.day != lastTime.day);
            appctrl_remote_write(curTime.day != lastTime.day);
            appctrl_status_write(curTime.day != lastTime.day);
            appctrl_write_last_time(&curTime);

            /* 11. 灯、蜂鸣器、显示控制 */
            appctrl_system_alarm(CTRL_MAX_TURN(g_CtrlCfg.turnNum, g_CtrlCfg.turnMAX));

            /* 12. 控制实施 */
            if(TRUE == g_CtrlCfg.bModeOk)
            {
#ifdef PRODUCT_ZCU_1
                appctrl_set_switch_zcu1(&g_curSwitch);
#endif
#ifdef PRODUCT_ZCU_3
                appctrl_set_switch_zcu3(g_CtrlCfg.nACM1, g_CtrlCfg.nsubACM1, &g_curSwitch);
#endif
#if defined PRODUCT_ECU || defined PRODUCT_ECU_G
                appctrl_set_switch(g_CtrlCfg.nACM1, g_CtrlCfg.nsubACM1, g_CtrlCfg.nACM2, g_CtrlCfg.nsubACM1, &g_curSwitch);
#endif
            }

#if defined PRODUCT_ECU || defined PRODUCT_ZCU_3 || defined PRODUCT_ECU_G
            appctrl_set_switch_close(&g_curSwitch);
#endif

            /* 13. 控制回路断线检测 */
#ifdef PRODUCT_ZCU_1
            appctrl_get_switch_input_zcu1(g_CtrlCfg.nACM1, g_CtrlCfg.nACM2, &g_curSwitch);
#else
            appctrl_get_switch_input(g_CtrlCfg.nACM1, g_CtrlCfg.nsubACM1, g_CtrlCfg.nACM2, g_CtrlCfg.nsubACM1, &g_curSwitch);
#endif
        } 

        lastTime = curTime;
        usleep(10000);
    }
}


