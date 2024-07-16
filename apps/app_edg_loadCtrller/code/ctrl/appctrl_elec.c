/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块 电量控制
* @date：    2020-4-9
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "audio.h"
#include "sys_cfg.h"

#include "appctrl_comm.h"
#include "appctrl_elec.h"
#include "framecomm.h"

EC_INFO_T  g_EcBuf;
uint8      g_CallFeeAlarm = FALSE;
uint8      g_CallHour = 0xff;

#define DEFAULT_EN_PARA        99999990000000LL


//该函数处理功率电量等值显示,防止大数值跃屏
void valueDisplayProcess_strUnitElec(int64 energy, char* str, size_t n,uint32 weight,const char* unit)
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
* @name      : appctrl_elec_init
* @brief     ：电量控制变量初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_elec_init()
{
    int ret = 0;
    char buf[4096] = {0};
    uint32 len = 0;
    
    memset(&g_EcBuf, 0, sizeof(EC_INFO_T));

    ret = lctrl_read_ctrl(CTRL_ELEC_TYPE, (uint8*)buf, 4096, &len);
    if(0 == ret && sizeof(g_EcBuf) == len)
    {
        memcpy(&g_EcBuf, buf, len);
        return;
    }
    CTRL_FMT_DEBUG("文件中没有合法电控数据 %d, 数据长度 %d\n", ret, len);
}

/**********************************************************************
* @name      : appctrl_month_close
* @brief     ：获取电控轮次信息
* @param[in] ：CTRL_SUBGROUP_T *ptGroups   总加组配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    : 2021-4-16
**********************************************************************/
void appctrl_elec_turnset(CTRL_SUBGROUP_T *ptGroups)
{
    uint8 i, j;
    for(i = 0;i < OOP_MAX_TG; i++)
    {
        for(j = 0; j < OOP_MAX_TURN; j++)
        {
            g_EcBuf.EcTurnSet[i][j] = ((ptGroups[i].ctrlStatus.ecTurn.nValue >> j) & 0x1);
        }
        g_EcBuf.EcTurnAllSet[i] = ptGroups[i].ctrlStatus.ecTurn.nValue;
    }
}

/**********************************************************************
* @name      : appctrl_month_close
* @brief     ：月电控取消告警或拉闸状态
* @param[in] ：uint8 grp       总加组序号
               uint8 turn      轮次
               uint8 bReason   合闸原因
               uint8 bType     类型(0 - 清除告警、越限; 1 - 没有轮次投入清除告警、越限; 2 - 不清除)
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_month_close(uint8 grp,uint8 turn, uint8 bReason, uint8 bType)
{
    if(bType == 0)
    {
        g_EcBuf.bEcMonPassWarn[grp] = 0;
        g_EcBuf.HavePassMonSet[grp] = 0;
    }
    else if(bType == 1)
    {
        if (0 == g_EcBuf.EcTurnAllSet[grp])
        {
            g_EcBuf.bEcMonPassWarn[grp] = 0;
            g_EcBuf.HavePassMonSet[grp] = 0;
        }
    }
    appctrl_set_switch_status(CTRL_MONTH_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_OFF);
    appctrl_set_switch_status(CTRL_MONTH_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_OFF);
}

/**********************************************************************
* @name      : appctrl_month_close
* @brief     ：购电控取消告警或拉闸状态
* @param[in] ：uint8 grp       总加组序号
               uint8 turn      轮次
               uint8 bReason   合闸原因
               uint8 bType     类型(0 - 清除告警、越限; 1 - 没有轮次投入清除告警、越限; 2 - 不清除)
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_buy_close(uint8 grp, uint8 turn, uint8 bReason, uint8 bType)
{
    if(bType == 0)
    {
        g_EcBuf.bEcBuyPassWarn[grp] = 0;//-购电控越限标志清零-
        g_EcBuf.HavePassBuySet[grp] = 0;//-购电控首次超定值标志清零-
    }
    else if (bType == 1)
    {
        if(0 == g_EcBuf.EcTurnAllSet[grp])
        {    //-如果购电控越限且轮次未投入-
            g_EcBuf.bEcBuyPassWarn[grp] = 0;//-购电控越限标志清零-*
            g_EcBuf.HavePassBuySet[grp] = 0;//-购电控首次超定值标志清零-
        }
    }
    appctrl_set_switch_status(CTRL_BUY_TYPE, CTRL_ALARM_STORE, turn, grp, TURN_OFF);
    appctrl_set_switch_status(CTRL_BUY_TYPE, CTRL_STATUS_STORE, turn, grp, TURN_OFF);
}

/**********************************************************************
* @name      : appctrl_elec_alarm
* @brief     ：电控告警
* @param[in] ：uint8 grp      总加组序号
               uint8 ECType   电控类型
               uint8 EAType   告警类型,SET_ALARM_TYPE:超告警定值告警,BREAK_ALARM_TYPE:拉闸告警
               uint8 times    时间
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_elec_alarm(uint8 grp, uint8 ECType, uint8 EAType, uint8 times)
{
    switch(ECType)
    {
        case CTRL_MONTH_TYPE:
        {
            if(EAType==SET_ALARM_TYPE)
            {
                //-月电控超告警值告警-
                appctrl_alarm_data(CTRL_MONTH_TYPE, grp, times, EMAUDIO_INDEX_18, appctrl_elec_guishow);
            }
            else if(EAType==BREAK_ALARM_TYPE)
            {
                //-月电控拉闸告警-
                appctrl_alarm_data(CTRL_MONTH_TYPE, grp, times, EMAUDIO_INDEX_19, appctrl_elec_guishow);
            }
            break;
        }
        case CTRL_BUY_TYPE:
        {
            if(EAType==SET_ALARM_TYPE)
            {
                //-购电控超告警值告警-
                appctrl_alarm_data(CTRL_BUY_TYPE, grp, times, EMAUDIO_INDEX_18, appctrl_elec_guishow);
            }
            else if(EAType==BREAK_ALARM_TYPE)
            {
                //-购电控拉闸告警-
                appctrl_alarm_data(CTRL_BUY_TYPE, grp, times, EMAUDIO_INDEX_19, appctrl_elec_guishow);
            }
            break;
        }
        default:
            break;
    }
}

/**********************************************************************
* @name      : appctrl_get_MonPara
* @brief     ：获取月电控参数
* @param[in] ：uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_MONTH_ARG_T *ptArg    控制配置
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
int appctrl_get_MonPara(uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_MONTH_ARG_T *ptArg)
{
    uint8 i;
    OOP_CFGUNIT_810D_T *pParam = NULL;

    /* 是否投入 */
    if(FALSE == ptArg->bEffect[grp] || 0 == ptGroup->ctrlEcTurn.nValue)
    {
        //CTRL_FMT_TRACE("Month grp %d not effect\n", grp + 1);
        return -1;
    }

    /* 总加组没有投入月电控 */
    if(0 == (ptGroup->ctrlStatus.ecState.nValue & 0x01))
    {
        return -2;
    }

    for(i = 0; i < ptArg->param.nNum; i++)
    {
        if(ptArg->param.item[i].object == 0x2301 + grp)
        {
            pParam = &(ptArg->param.item[i]);
            break;
        }
    }
    if(NULL == pParam)
    {
        return -3;
    }

    /* 月电量定值为零，则不进行控制 */
    if(0 == pParam->defValue)
    {
        return -4;
    }
    return 0;
}

/**********************************************************************
* @name      : appctrl_get_BuyPara
* @brief     ：获取购电控参数
* @param[in] ：uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_BUY_ARG_T   *ptArg    控制配置
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-10
* @Update    :
**********************************************************************/
int appctrl_get_BuyPara(uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_BUY_ARG_T *ptArg)
{
    uint8 j;
    OOP_CFGUNIT_810C_T *pParam = NULL;
    int64 leften = 0;
    uint32 price = 0;


    /* 检查配置 */
    for(j = 0; j < ptArg->param.nNum; j++)
    {
        if(ptArg->param.item[j].object == 0x2301 + grp)
        {
            pParam = &(ptArg->param.item[j]);
            break;
        }
    }
    if(NULL == pParam || 0 == ptGroup->ctrlEcTurn.nValue)
    {
        return -1;
    }
    
    if(0 == (ptGroup->ctrlStatus.ecState.nValue & 0x02) && 0 == pParam->mode)
    {   //远程模式未投入也控制
        return -2;
    }
    
    if(0 == pParam->number)
    {
        /* 购电单号为0 */
        CTRL_FMT_DEBUG("总加组%d购电单号是0\n", grp + 1);
        return -3;
    }

    remain_energy_lock(); //修改剩余电量 先加锁
    if(0 != appctrl_get_leften(grp))
    {
        remain_energy_unlock();  //修改剩余电量 解锁
        return -4;
    }

    /* 计算剩余电量 写入数据中心 */
    leften = ptGroup->leftEn;

    /* 本地模式进行分钟电量扣减，远程模式不扣减 */
    if(0 == pParam->mode)
    {   //-一分钟电费,电价和电量都括大了10000倍要除掉一个-
        if(1 == pParam->type) //电费型
        {
            price = appctrl_get_enprice();
            CTRL_FMT_DEBUG("当前电价是%u, 总加组%d上1min内电量%lld\n", price, grp + 1, ptGroup->last1minEn);
            leften -= (ptGroup->last1minEn * price)/10000;
        }
        else
        {
            leften -= ptGroup->last1minEn;
        }
    }

    if (leften < -DEFAULT_EN_PARA)//-防止剩余电量\费减至溢出-
    {
        leften = -DEFAULT_EN_PARA;
    }
    ptGroup->leftEn = leften;

    /* 写回终端数据中心 */
    if(0 == pParam->mode)
    {
        appctrl_write_leften(grp, leften);
    }
    remain_energy_unlock();  //修改剩余电量 解锁

    g_EcBuf.BuyType[grp] = pParam->type;
    g_EcBuf.BuyLeftValue[grp] = leften;
    
    CTRL_FMT_DEBUG("总加组%d(%s模式)购电%s剩余%lld, 上1min值%lld, 告警门限%lld, 定值%lld\n", grp + 1,
                   pParam->mode ? "远程" : "本地", pParam->type ? "费" : "量", 
                   leften, ptGroup->last1minEn, pParam->alarmLimit, pParam->tripLimit);
    return 0;
}

/**********************************************************************
* @name      : appctrl_month_deal
* @brief     ：根据主站设置的月电量定值，对月用电量进行监控
* @param[in] ：int cid                      数据中心cid
               uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_MONTH_ARG_T *ptArg    控制配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_month_deal(int cid, uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_MONTH_ARG_T *ptArg)
{
    uint8    j;
    OOP_CFGUNIT_810D_T *pParam = NULL;
    int64   alarmValue = 0;
    int64   usedValue = 0;
    int64   limitValue = 0;
    time_t  curtime;
    OOP_DATETIME_S_T curTime = {0};
    BOOL    bSave = FALSE;                     //是否需要记录月电控跳闸事件

    for(j = 0; j < OOP_MAX_TURN; j++)
    {
        //-判断是否有未投入的轮次，状态为月电控告警或拉闸-
        if(0 == g_EcBuf.EcTurnSet[grp][j])
        {
            appctrl_month_close(grp , j, 0, 1);
        }
    }

    /* 获取参数 */
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
        return;
    }

    if(0 == pParam->limitCoe)
    {
        alarmValue = (pParam->defValue * 80) / 100; //没有设置值默认是80%
    }
    else
    {
        alarmValue = (pParam->defValue * pParam->limitCoe) / 100;
    }
    usedValue = ptGroup->monActEn.nValue[0];
    limitValue = (pParam->defValue * (100 + pParam->floatCoe))/100;

    g_EcBuf.MonUsedValue[grp] = usedValue;
    g_EcBuf.MondefValue[grp] = pParam->defValue;
    g_EcBuf.MonlimitCoe[grp] = pParam->limitCoe;
    g_EcBuf.MonfloatCoe[grp] = pParam->floatCoe;

    CTRL_FMT_DEBUG("月电控, 总加组%d用电量%lld, 告警值%lld, 定值%lld,\n", 
                    grp + 1, usedValue, alarmValue, limitValue);

    /* 月用电量未超跳闸门限值，则合闸 */
    if(usedValue <= limitValue)
    {
        //-月用电量未超跳闸门限值，则合闸-
        if(usedValue < alarmValue)
        {
            g_EcBuf.bEcMonPassWarn[grp] = 0;    //-月电控越限标志清0-
            g_EcBuf.HavePassMonSet[grp] = 0;    //-月电控越限标志清0-
        }
        for (j = 0; j < OOP_MAX_TURN; j++)
        {
            appctrl_month_close(grp, j, LOWSETVAL_CLOSE, 2);
        }
    }

    /* 月用电量超告警值，但未超定值 */
    if (usedValue >= alarmValue && usedValue <= limitValue)
    {
        for(j = 0; j < turnMax; j++)
        {
            //-有轮次投入的情况下，且未处于电控告警状态
            if(0 != g_EcBuf.EcTurnSet[grp][j] && 
               TURN_OFF == appctrl_get_switch_status(CTRL_MONTH_TYPE, CTRL_ALARM_STORE, j, grp))
            {
                appctrl_set_switch_status(CTRL_MONTH_TYPE, CTRL_ALARM_STORE, j, grp, TURN_ON);
                appctrl_set_switch_status(CTRL_MONTH_TYPE, CTRL_STATUS_STORE, j, grp, TURN_OFF);
            }
        }
        
        /* 首次超告警值，告警 */
        if (g_EcBuf.bEcMonPassWarn[grp] == 0)
        {
            CTRL_FMT_DEBUG("总加组%d剩余电量在告警门限和跳闸门限之间\n",grp + 1);
            //CheckERC_3203(cid, appctrl_get_object(CTRL_MONTH_TYPE), 0x2301 + grp, alarmValue);
            appctrl_elec_alarm(grp, CTRL_MONTH_TYPE, SET_ALARM_TYPE, 3);
            g_EcBuf.MonShowLeftTimes[grp] = 120;
        }
        else
        {
            curtime = time(NULL);
            DT_Time2DateTimeShort(curtime, &curTime);
            if ((10 == curTime.hour || 15 == curTime.hour) && 0 == curTime.minute)
            {    //-首次超定值告警后，分别在10点和15点再次告警-
                appctrl_elec_alarm(grp, CTRL_MONTH_TYPE, SET_ALARM_TYPE, 3);
                g_EcBuf.MonShowLeftTimes[grp] = 120;
            }
        }
        g_EcBuf.bEcMonPassWarn[grp] = HAVE_PASS_SETVAL;  //-月电控越限标志置位-

    }
    else if(usedValue > limitValue)
    {
        OOP_BITSTR8_T turnState;

        g_EcBuf.bEcMonPassWarn[grp] = 1;    //-月电控越限标志置1-
        g_EcBuf.HavePassMonSet[grp] = 1;
        g_EcBuf.MonShowLeftTimes[grp] = 0;
        turnState.nValue = 0;

        for(j = 0; j < turnMax; j++)
        {
            //-轮次投入并且该轮次为月电控合闸，则跳闸-
            if (0 != g_EcBuf.EcTurnSet[grp][j] &&
                TURN_OFF == appctrl_get_switch_status(CTRL_MONTH_TYPE, CTRL_STATUS_STORE, j, grp))
            {
                bSave = TRUE;
                appctrl_set_switch_status(CTRL_MONTH_TYPE, CTRL_STATUS_STORE, j, grp, TURN_ON);
                appctrl_set_switch_status(CTRL_MONTH_TYPE, CTRL_ALARM_STORE, j, grp, TURN_OFF);
                appctrl_elec_alarm(grp, CTRL_MONTH_TYPE, BREAK_ALARM_TYPE, 3);
                turnState.nValue |= (0x01 << j);
            }
        }
        
        if(bSave)
        {
            //写电控跳闸事件
            CTRL_FMT_DEBUG("总加组%d剩余电量达到跳闸门限\n",grp + 1);
            //CheckERC_3201(cid, appctrl_get_object(CTRL_MONTH_TYPE), 0x2301 + grp, turnState, limitValue, usedValue);
            bSave = FALSE;
        }
    }
}

/**********************************************************************
* @name      : appctrl_buy_deal
* @brief     ：根据主站设置的购电量定值，对月用电量进行监控
* @param[in] ：int cid                    数据中心cid
               uint8 grp                  总加组序号
               uint8 turnMax              最大轮次
               CTRL_SUBGROUP_T *ptGroup   总加组配置
               CTRL_BUY_ARG_T   *ptArg    控制配置
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-10
* @Update    :
**********************************************************************/
void appctrl_buy_deal(int cid, uint8 grp, uint8 turnMax, CTRL_SUBGROUP_T *ptGroup, CTRL_BUY_ARG_T *ptArg)
{
    uint8 j;
    OOP_CFGUNIT_810C_T *pParam = NULL;
    time_t              curtime;
    OOP_DATETIME_S_T    curTime = {0};
    uint8               bValue = FALSE;
    BOOL                bSave = FALSE; 

    /* 检查配置 */
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
        return;
    }

    g_EcBuf.BuyAlarmValue[grp] = pParam->alarmLimit;
    g_EcBuf.BuySetValue[grp] = pParam->tripLimit;

    if(FALSE == ptArg->bEffect[grp])
    {
        /* 远程模式 产生告警  */
        if(1 == pParam->mode)
        {
            if(ptGroup->leftEn > pParam->alarmLimit)//-剩余电量大于告警限值，就取消告警
            {
                g_EcBuf.TelBuyAlarm[grp] = 0; //-首次超告警值标志清零-
            }
            if(ptGroup->leftEn <= pParam->alarmLimit)
            {   //-剩余电量低于告警门限时，开始告警通知用户-
                if (0 == g_EcBuf.TelBuyAlarm[grp])
                {   //-首次超告警值，告警-
                    g_EcBuf.TelBuyAlarm[grp] = 1;
                    appctrl_elec_alarm(grp, CTRL_BUY_TYPE, SET_ALARM_TYPE, 3);  //-购电控超定值告警-
                    g_EcBuf.BuyShowLeftTimes[grp] = 120;
                }
                else
                {
                    curtime = time(NULL);
                    DT_Time2DateTimeShort(curtime, &curTime);
                    if(0 == curTime.minute)//-剩余购电量小于告警门限后，每小时告警一次-
                    {
                        appctrl_elec_alarm(grp, CTRL_BUY_TYPE, SET_ALARM_TYPE, 3);  //-购电控超定值告警-
                        g_EcBuf.BuyShowLeftTimes[grp] = 120;
                    }
                }
            }
        }
        
        for(j = 0; j < OOP_MAX_TURN; j++) //-无购电控投入，但有购电控告警或拉闸，则取消-
        {
            appctrl_buy_close(grp, j, 0, 0);
        }
        return;
    }

    /* 投入的情况 */
    for(j = 0; j < OOP_MAX_TURN; j++)
    {    //-判断是否有未投入的轮次，有则取消告警或拉闸-
        if(0 == g_EcBuf.EcTurnSet[grp][j])
        {   
            appctrl_buy_close(grp, j, 0, 1);
        }
    }

    //-剩余电量大于跳闸门限，则合闸-
    g_EcBuf.TelBuyAlarm[grp] = 0;
    if(ptGroup->leftEn > pParam->alarmLimit)
    {
        if(1 == g_EcBuf.bEcBuyPassWarn[grp] || 1 == g_EcBuf.HavePassBuySet[grp])
        {
            CTRL_FMT_DEBUG("总加组%d剩余电量大于告警门限, 合闸\n",grp + 1);
        }
        for (j = 0; j < OOP_MAX_TURN; j++)
        {
            appctrl_buy_close(grp, j, 0, 0);
        }
    }

    //-剩余电量在告警门限和跳闸门限之间时，开始告警通知用户-
    if (ptGroup->leftEn <= pParam->alarmLimit && ptGroup->leftEn > pParam->tripLimit)
    {
        for(j = 0; j < turnMax; j++)
        {
            //-有轮次投入的情况下，且未处于购电控告警状态
            bValue = (0 != g_EcBuf.EcTurnSet[grp][j]) &&
                     (TURN_OFF == appctrl_get_switch_status(CTRL_BUY_TYPE, CTRL_ALARM_STORE, j, grp));
            if(TRUE == bValue && HAVE_PASS_SETVAL != g_EcBuf.bEcBuyPassWarn[grp])
            {
                bSave = TRUE;
            }
            if(TRUE == bValue)
            {
                appctrl_set_switch_status(CTRL_BUY_TYPE, CTRL_ALARM_STORE, j, grp, TURN_ON);
                appctrl_set_switch_status(CTRL_BUY_TYPE, CTRL_STATUS_STORE, j, grp, TURN_OFF);
            }
        }

        if(bSave)
        {
            //-存储购电控告警记录 -
            CTRL_FMT_DEBUG("总加组%d剩余电量在告警门限和跳闸门限之间\n",grp + 1);
            //CheckERC_3203(cid, appctrl_get_object(CTRL_BUY_TYPE), 0x2301 + grp, pParam->alarmLimit);
            bSave = FALSE;
        }

        if(g_EcBuf.bEcBuyPassWarn[grp] == 0)
        {    //-首次超告警值，告警-
            appctrl_elec_alarm(grp, CTRL_BUY_TYPE, SET_ALARM_TYPE, 3);    //-购电控超定值告警-
            g_EcBuf.BuyShowLeftTimes[grp] = 120;
        }
        else
        {
            curtime = time(NULL);
            DT_Time2DateTimeShort(curtime, &curTime);
            if (curTime.minute == 0)//-剩余购电量小于告警门限后，每小时告警一次-
            {
                appctrl_elec_alarm(grp, CTRL_BUY_TYPE, SET_ALARM_TYPE, 3);    //-购电控超定值告警-
                g_EcBuf.BuyShowLeftTimes[grp] = 120;
            }
        }
        g_EcBuf.bEcBuyPassWarn[grp] = HAVE_PASS_SETVAL;   //-购电控越限标志置1-
        g_EcBuf.HavePassBuySet[grp] = 0;
    }
    else if (ptGroup->leftEn <= pParam->tripLimit)
    {
        OOP_BITSTR8_T turnState;

        turnState.nValue = 0;

        g_EcBuf.bEcBuyPassWarn[grp] = 0;
        g_EcBuf.HavePassBuySet[grp] = 1;
        g_EcBuf.BuyShowLeftTimes[grp] = 0;

        for (j =0; j < turnMax; j++)
        {
            if(0 != g_EcBuf.EcTurnSet[grp][j] && 
               TURN_OFF == appctrl_get_switch_status(CTRL_BUY_TYPE, CTRL_STATUS_STORE, j, grp) )
            {
                bSave = TRUE;
                appctrl_set_switch_status(CTRL_BUY_TYPE, CTRL_ALARM_STORE, j, grp, TURN_OFF);
                appctrl_set_switch_status(CTRL_BUY_TYPE, CTRL_STATUS_STORE, j, grp, TURN_ON);
                appctrl_elec_alarm(grp, CTRL_BUY_TYPE, BREAK_ALARM_TYPE, 3);    //-购电控拉闸告警-
                turnState.nValue |= (0x1 << j);
            }
        }

        if(bSave)
        {
            CTRL_FMT_DEBUG("总加组%d剩余电量达到跳闸门限\n",grp + 1);
            //填写购电控跳闸事件
            //CheckERC_3201(cid, appctrl_get_object(CTRL_BUY_TYPE), 0x2301 + grp, turnState,
            //              pParam->tripLimit, ptGroup->leftEn);
            bSave = FALSE;
        }
    }
}

/**********************************************************************
* @name      : appctrl_month_ctrl
* @brief     ：根据主站设置的月电量定值，对月用电量进行监控
* @param[in] ：int cid                      数据中心cid
               uint8 turnmax                最大轮次数
               CTRL_SUBGROUP_T *ptSubGroups 总加组配置
               CTRL_MONTH_ARG_T *ptMonth    控制配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_month_ctrl(int cid, uint8 turnmax, CTRL_SUBGROUP_T *ptSubGroups, CTRL_MONTH_ARG_T *ptMonth)
{
    uint8   i, j;
    int     ret = 0;
    
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        /* 获取月电控参数 */
        ret = appctrl_get_MonPara(i, turnmax, &(ptSubGroups[i]), ptMonth);
        if(0 != ret)
        {
            for (j = 0; j < OOP_MAX_TURN; j++) //-无月电控投入，但有月电控拉闸或告警，则取消-
            {
                appctrl_month_close(i, j, CTRLOFF_CLOSE, 0);
            }
            CTRL_FMT_TRACE("Sub group %d get month ctrl param filed, ret %d\n", i + 1, ret);
            continue;
        }
        else
        {
            appctrl_month_deal(cid, i, turnmax, &(ptSubGroups[i]), ptMonth);
        }
    }
}

/**********************************************************************
* @name      : appctrl_buy_ctrl
* @brief     ：根据主站设置的购电量定值，对月用电量进行监控
* @param[in] ：int cid                      数据中心cid
               uint8 turnmax                最大轮次数
               CTRL_SUBGROUP_T *ptSubGroups 总加组配置
               CTRL_BUY_ARG_T *ptBuyArg     控制配置
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-10
* @Update    :
**********************************************************************/
void appctrl_buy_ctrl(int cid, uint8 turnmax, CTRL_SUBGROUP_T *ptSubGroups, CTRL_BUY_ARG_T *ptBuyArg)
{
    uint8 i, j;
    int   ret = 0;

    for (i = 0; i < OOP_MAX_TG; i++)
    {
        ret = appctrl_get_BuyPara(i, turnmax, &(ptSubGroups[i]), ptBuyArg);
        if(0 != ret)
        {
            for (j = 0; j < OOP_MAX_TURN; j++)
            {
                appctrl_buy_close(i, j, CTRLOFF_CLOSE, 0);
            }
            CTRL_FMT_TRACE("Sub group %d get buy ctrl param filed, ret %d\n", i + 1, ret);
            continue;
        }
        else
        {
            appctrl_buy_deal(cid, i, turnmax, &(ptSubGroups[i]), ptBuyArg);
        }
    }
}

/**********************************************************************
* @name      : appctrl_callfee_ctrl
* @brief     ：催费告警 一个小时告警一次
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    : 2020-5-21
**********************************************************************/
void appctrl_callfee_ctrl()
{
    time_t              curtime;
    OOP_DATETIME_S_T    curTime = {0};
    CTRL_CALLFEE_ARG_T *ptParam = NULL;
    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计
    uint8            status;
    OOP_CALLFEE_T    parma;

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    status = get_fee_state(&parma);
    if(0 != status)
    {
        /* 识别到有催费告警 */
        ptParam->bEffect = 1 == status ? TRUE : FALSE;
        memcpy(&ptParam->param, &parma, sizeof(OOP_CALLFEE_T));
        appctrl_set_callfee_data(ptParam->bEffect, &parma);
    }

    ptParam = &(ptCtrlArg->tCallFee);

    if(FALSE == ptParam->bEffect)
    {
        g_CallFeeAlarm = FALSE;
        g_CallHour = 0xff;
        appctrl_set_callfee_status(0);
        return;
    }

    curtime = time(NULL);
    DT_Time2DateTimeShort(curtime, &curTime);

    if(curTime.hour <= 7) //-0点到7点之间的告警-
    {
        if(ptParam->param.period[0] & (0x01 << curTime.hour))
        {
            appctrl_set_callfee_status(1);
            if(FALSE == g_CallFeeAlarm || g_CallHour != curTime.hour)
            {
                appctrl_alarm_data(CTRL_CALLFEE_TYPE, 0, 3, EMAUDIO_INDEX_21, appctrl_elec_guishow);
                g_CallFeeAlarm = TRUE;
                g_CallHour = curTime.hour;
            }
            return;
        }
    }
    else if(curTime.hour <= 15) //-8点到15点之间的告警-
    {
        if(ptParam->param.period[1] & (0x01 << (curTime.hour - 8)))
        {
            appctrl_set_callfee_status(1);
            if(FALSE == g_CallFeeAlarm || g_CallHour != curTime.hour)
            {
                appctrl_alarm_data(CTRL_CALLFEE_TYPE, 0, 3, EMAUDIO_INDEX_21, appctrl_elec_guishow);
                g_CallFeeAlarm = TRUE;
                g_CallHour = curTime.hour;
            }
            return;
        }
    }
    else
    {
        if(ptParam->param.period[2] & (0x01 << (curTime.hour - 16)))
        {
            appctrl_set_callfee_status(1);
            if(FALSE == g_CallFeeAlarm || g_CallHour != curTime.hour)
            {
                appctrl_alarm_data(CTRL_CALLFEE_TYPE, 0, 3, EMAUDIO_INDEX_21, appctrl_elec_guishow);
                g_CallFeeAlarm = TRUE;
                g_CallHour = curTime.hour;
            }
            return;
        }
    }
    g_CallFeeAlarm = FALSE;
    g_CallHour = 0xff;
    appctrl_set_callfee_status(0);
}

/**********************************************************************
* @name      : appctrl_elec_ctrl
* @brief     ：电量控制, 只记录功控合闸、告警中间状态
* @param[in] ：int cid         数据中心cid
               uint8 bPassMon  是否过月
               uint8 turnmax   最大轮次数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_elec_ctrl(int cid, uint8 bPassMon, uint8 turnmax)
{
    uint8 i, j;
    uint32 storeno = 0;
    int64  en1min = 0;

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    /* 电控轮次信息刷新 */
    appctrl_elec_turnset(ptSubGroups);

    /* 获取前一分钟电量 */
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        if(0 == sta_tg_min_acte_read(0x2301 + i, &en1min, &storeno))
        {
            if(ptSubGroups[i].storeNo != storeno)
            {
                ptSubGroups[i].storeNo = storeno;
                ptSubGroups[i].last1minEn = en1min;
            }
            else
            {
                ptSubGroups[i].last1minEn = 0; //防止重复计算
            }
        }
    }

    /* 过月处理 */
    if(TRUE == bPassMon)
    {
        for(i = 0; i < OOP_MAX_TG; i++)
        {
            for (j = 0; j < OOP_MAX_TURN; j++)
            {
                /* 过月月电控合闸 */
                appctrl_month_close(i , j, OUTOFTIME_CLOSE, 0);
                g_EcBuf.MonUsedValue[i] = 0;
            }
        }
    }
    else
    {
        /* 月电控 */
        appctrl_month_ctrl(cid, turnmax, ptSubGroups, &(ptCtrlArg->tMonth));
    }

    /* 购电控 */
    appctrl_buy_ctrl(cid, turnmax, ptSubGroups, &(ptCtrlArg->tBuy));
}

/**********************************************************************
* @name      : appctrl_elec_status
* @brief     ：月电控、购电控告警和跳闸输出状态汇总
* @param[in] ：uint8 turnmax   最大轮次数
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
void appctrl_elec_status(uint8 turnmax)
{
    uint8 turn, grp;
    uint8 i;
    uint8 iBuy, iMonth;

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    OOP_TGCURSTATE_T curCtrlStatus; //0x230x1100总加组当前控制状态 
    OOP_TURNSTATES_T outputBuy;//控制输出状态
    OOP_TGSTATES_T   alarmBuy; //越限告警状态
    OOP_TURNSTATES_T outputMonth;//控制输出状态
    OOP_TGSTATES_T   alarmMonth; //越限告警状态

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    memset(&outputBuy, 0, sizeof(OOP_TURNSTATES_T));
    memset(&alarmBuy, 0, sizeof(OOP_TGSTATES_T));
    outputBuy.nNum = ptCtrlArg->tBuy.effectStatus.nNum;
    alarmBuy.nNum = ptCtrlArg->tBuy.effectStatus.nNum;
    for(i = 0; i < ptCtrlArg->tBuy.effectStatus.nNum; i++)
    {
        outputBuy.item[i].obj = ptCtrlArg->tBuy.effectStatus.item[i].obj;
        alarmBuy.item[i].obj = ptCtrlArg->tBuy.effectStatus.item[i].obj;
    }
    memset(&outputMonth, 0, sizeof(OOP_TURNSTATES_T));
    memset(&alarmMonth, 0, sizeof(OOP_TGSTATES_T));
    outputMonth.nNum = ptCtrlArg->tMonth.effectStatus.nNum;
    alarmMonth.nNum = ptCtrlArg->tMonth.effectStatus.nNum;
    for(i = 0; i < ptCtrlArg->tMonth.effectStatus.nNum; i++)
    {
        outputMonth.item[i].obj = ptCtrlArg->tMonth.effectStatus.item[i].obj;
        alarmMonth.item[i].obj = ptCtrlArg->tMonth.effectStatus.item[i].obj;
    }

    /* 单个控制状态整理 总加组 功控 */
    for(grp = 0; grp < OOP_MAX_TG; grp++)
    {
        //先拷贝出来，不修改功控状态
        memcpy(&curCtrlStatus, &(ptSubGroups[grp].curCtrlStatus), sizeof(OOP_TGCURSTATE_T));
        curCtrlStatus.mecTrip.nValue = 0;
        curCtrlStatus.becTrip.nValue = 0;
        curCtrlStatus.ecWarn.nValue = 0;

        if(0 == ptSubGroups[grp].ctrlStatus.ecTurn.nValue)
        {
            appctrl_write_subgroupstatus(grp, &curCtrlStatus);
            continue;
        }

        for(iBuy = 0; iBuy < OOP_MAX_TG; iBuy++)
        {
            if(ptCtrlArg->tBuy.effectStatus.item[iBuy].obj == 0x2301 + grp)
            {
                break;
            }
        }
        for(iMonth = 0; iMonth < OOP_MAX_TG; iMonth++)
        {
            if(ptCtrlArg->tMonth.effectStatus.item[iMonth].obj == 0x2301 + grp)
            {
                break;
            }
        }

        for(turn = 0; turn < turnmax; turn++)
        {
            /* 购电控 */
            if(TURN_ON == appctrl_get_switch_status(CTRL_BUY_TYPE, CTRL_STATUS_STORE, turn, grp))
            {
                if(iBuy < OOP_MAX_TG)
                {
                    outputBuy.item[iBuy].state.nValue |= (0x01 << turn);
                }
                curCtrlStatus.becTrip.nValue |= (0x01 << turn);
            }
            if(TURN_ON == appctrl_get_switch_status(CTRL_BUY_TYPE, CTRL_ALARM_STORE, turn, grp))
            {
                if(iBuy < OOP_MAX_TG)
                {
                    alarmBuy.item[iBuy].state= 1;
                }
                curCtrlStatus.ecWarn.nValue |= 0x02;
            }

            /* 月电控状态 */
            if(TURN_ON == appctrl_get_switch_status(CTRL_MONTH_TYPE, CTRL_STATUS_STORE, turn, grp))
            {
                if(iMonth < OOP_MAX_TG)
                {
                    outputMonth.item[iMonth].state.nValue |= (0x01 << turn);
                }
                curCtrlStatus.mecTrip.nValue |= (0x01 << turn);
            }
            if(TURN_ON == appctrl_get_switch_status(CTRL_MONTH_TYPE, CTRL_ALARM_STORE, turn, grp))
            {
                if(iMonth < OOP_MAX_TG)
                {
                    alarmMonth.item[iMonth].state = 0x01;
                }
                curCtrlStatus.ecWarn.nValue |= 0x01;
            }
        }
        appctrl_write_subgroupstatus(grp, &curCtrlStatus);
    }
    appctrl_write_outstatus(CTRL_BUY_TYPE, &outputBuy);
    appctrl_write_alarmstatus(CTRL_BUY_TYPE, &alarmBuy);
    appctrl_write_outstatus(CTRL_MONTH_TYPE, &outputMonth);
    appctrl_write_alarmstatus(CTRL_MONTH_TYPE, &alarmMonth);
}

/**********************************************************************
* @name      : appctrl_month_guishow
* @brief     ：月电控告警和跳闸输出桌面显示
* @param[in] ：uint8 group     总加组
               uint8 turnmax   最大轮次数
               uint16 buflen   缓存大小
* @param[out]：char* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-4-30
* @Update    :
**********************************************************************/
uint8 appctrl_month_guishow(uint8 group, uint8 turnmax, char* pBuf, uint16 buflen, uint16* msglen, int pinx)
{
    uint16 offset = 0;
    uint8  devStatus = 0;
    uint8  i = 0;
    uint8  linenum = 2;
    char  *turnName[] = {"一轮", "二轮", "三轮", "四轮", "五轮", "六轮", "七轮", "八轮"};

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    if(pinx <= 160)
    {
        offset = sprintf(pBuf, "      月电控  控制组 %d\n", group + 1);
    }
    else
    {
        linenum = 3;
        offset = sprintf(pBuf, "\n            月电控  控制组 %d", group + 1);
    }
    /* 获得月电控输出状态 */
    for(i = 0; i < turnmax; i++)
    {
        if(0 == (i % linenum))
        {
            offset += sprintf(pBuf + offset, "\n");
        }
        else
        {
            offset += sprintf(pBuf + offset, "    ");
        }

        if(FALSE == ptCtrlArg->tMonth.bEffect[group])
        {
            /* 控制解除 */
            g_EcBuf.bEcMonPassWarn[group] = 0;
            g_EcBuf.HavePassMonSet[group] = 0;
            g_EcBuf.MonShowLeftTimes[group] = 0;
        }

        offset += sprintf(pBuf + offset, "%s: ", turnName[i]);
        if(1 == g_EcBuf.EcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
        {
            devStatus = appctrl_get_switch_status(CTRL_MONTH_TYPE, CTRL_STATUS_STORE, i, group);
            if(1 == g_EcBuf.bEcMonPassWarn[group] && 0 == g_EcBuf.HavePassMonSet[group]) //显示告警
            {
                if(0 == g_EcBuf.MonShowLeftTimes[group] % 2)
                {
                    offset += sprintf(pBuf + offset, "%s", "    ");
                }
                else
                {
                    offset += sprintf(pBuf + offset, "%s", TURN_OFF == devStatus ? "合闸" : "拉闸");
                }
            }
            else
            {
                offset += sprintf(pBuf + offset, "%s", TURN_OFF == devStatus ? "合闸" : "拉闸");
            }
        }
        else
        {
            offset += sprintf(pBuf + offset, "----");
        }
    }
    if(linenum < 3)
    {
        offset += sprintf(pBuf + offset, "\n");
    }
    /* 定值 */
    char tmpBuf[30] ={0};
    valueDisplayProcess_strUnitElec(g_EcBuf.MondefValue[group], tmpBuf, 30, 4, "Wh");
    offset += sprintf(pBuf + offset, "\n定值      : %s\n", tmpBuf);
    offset += sprintf(pBuf + offset, "浮动系数  : %d%%\n", g_EcBuf.MonfloatCoe[group]);
    memset(tmpBuf,0,30);
    valueDisplayProcess_strUnitElec(g_EcBuf.MonUsedValue[group], tmpBuf, 30, 4, "Wh");
    offset += sprintf(pBuf + offset, "当月总电量: %s",tmpBuf);

    *msglen = offset;
    if(g_EcBuf.MonShowLeftTimes[group] > 0)
    {
        g_EcBuf.MonShowLeftTimes[group]--;
    }

    return g_EcBuf.MonShowLeftTimes[group] > 0 ? TRUE : FALSE;
}

/**********************************************************************
* @name      : appctrl_buy_guishow
* @brief     ：购电控告警和跳闸输出桌面显示
* @param[in] ：uint8 group     总加组
               uint8 turnmax   最大轮次数
               uint16 buflen   缓存大小
* @param[out]：uint8* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-4-30
* @Update    :
**********************************************************************/
uint8 appctrl_buy_guishow(uint8 group, uint8 turnmax, char* pBuf, uint16 buflen, uint16* msglen, int pinx)
{
    uint16 offset = 0;
    uint8  devStatus = 0;
    uint8  i = 0;
    uint8  linenum = 2;
    char  *turnName[] = {"一轮", "二轮", "三轮", "四轮", "五轮", "六轮", "七轮", "八轮"};

    /* 参数内存指针 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    if(pinx <= 160)
    {
        offset = sprintf(pBuf, "      购电控  控制组 %d\n", group + 1);
    }
    else
    {
        linenum = 3;
        offset = sprintf(pBuf, "            购电控  控制组 %d", group + 1);
    }
    
    /* 获得月电控输出状态 */
    for(i = 0; i < turnmax; i++)
    {
        if(0 == (i % linenum))
        {
            offset += sprintf(pBuf + offset, "\n");
        }
        else
        {
            offset += sprintf(pBuf + offset, "    ");
        }

        if(FALSE == ptCtrlArg->tBuy.bEffect[group])
        {
            /* 控制解除 */
            g_EcBuf.bEcBuyPassWarn[group] = 0;
            g_EcBuf.HavePassBuySet[group] = 0;
            g_EcBuf.BuyShowLeftTimes[group] = 0;
        }

        offset += sprintf(pBuf + offset, "%s: ", turnName[i]);
        if(1 == g_EcBuf.EcTurnSet[group][i] && TRUE == appctrl_dev_input(i + 1))
        {
            devStatus = appctrl_get_switch_status(CTRL_BUY_TYPE, CTRL_STATUS_STORE, i, group);
            if(1 == g_EcBuf.bEcBuyPassWarn[group] && 0 == g_EcBuf.HavePassBuySet[group]) //显示告警
            {
                if(0 == g_EcBuf.BuyShowLeftTimes[group] % 2)
                {
                    offset += sprintf(pBuf + offset, "%s", "    ");
                }
                else
                {
                    offset += sprintf(pBuf + offset, "%s", TURN_OFF == devStatus ? "合闸" : "拉闸");
                }
            }
            else
            {
                offset += sprintf(pBuf + offset, "%s", TURN_OFF == devStatus ? "合闸" : "拉闸");
            }
        }
        else
        {
            offset += sprintf(pBuf + offset, "----");
        }
    }
    offset += sprintf(pBuf + offset, "\n");

    /* 定值 */
    if(0 == g_EcBuf.BuyType[group])
    {
        if(g_EcBuf.BuyLeftValue[group] < 0)
        {
            int64 leftValue = 0 - g_EcBuf.BuyLeftValue[group];
            offset += sprintf(pBuf + offset, "剩余电量: -%lld.%04lldkWh\n", leftValue/10000, leftValue%10000);
        }
        else
        {
            offset += sprintf(pBuf + offset, "剩余电量: %lld.%04lldkWh\n", 
                          g_EcBuf.BuyLeftValue[group]/10000, g_EcBuf.BuyLeftValue[group]%10000);
        }
        if(g_EcBuf.BuyAlarmValue[group] < 0)
        {
            int64 tempValue = 0 - g_EcBuf.BuyLeftValue[group];
            offset += sprintf(pBuf + offset, "告警电量: -%lld.%04lldkWh\n", tempValue/10000, tempValue%10000);
        }
        else
        {
            offset += sprintf(pBuf + offset, "告警电量: %lld.%04lldkWh\n", 
                              g_EcBuf.BuyAlarmValue[group]/10000, g_EcBuf.BuyAlarmValue[group]%10000);
        }
        if(g_EcBuf.BuySetValue[group] < 0)
        {
            int64 tempValue = 0 - g_EcBuf.BuySetValue[group];
            offset += sprintf(pBuf + offset, "跳闸电量: -%lld.%04lldkWh", tempValue/10000, tempValue%10000);
        }
        else
        {
            offset += sprintf(pBuf + offset, "跳闸电量: %lld.%04lldkWh", 
                              g_EcBuf.BuySetValue[group]/10000, g_EcBuf.BuySetValue[group]%10000);
        }
    }
    else
    {
        if(g_EcBuf.BuyLeftValue[group] < 0)
        {
            int64 leftValue = 0 - g_EcBuf.BuyLeftValue[group];
            offset += sprintf(pBuf + offset, "剩余金额: -%lld.%02lld元\n", leftValue/10000, (leftValue%10000)/100);
        }
        else
        {
            offset += sprintf(pBuf + offset, "剩余金额: %lld.%02lld元\n", 
                              g_EcBuf.BuyLeftValue[group]/10000, (g_EcBuf.BuyLeftValue[group]%10000)/100);
        }
        if(g_EcBuf.BuyAlarmValue[group] < 0)
        {
            int64 tempValue = 0 - g_EcBuf.BuyAlarmValue[group];
            offset += sprintf(pBuf + offset, "告警金额: -%lld.%02lld元\n", tempValue/10000, (tempValue%10000)/100);
        }
        else
        {
            offset += sprintf(pBuf + offset, "告警金额: %lld.%02lld元\n", 
                              g_EcBuf.BuyAlarmValue[group]/10000, (g_EcBuf.BuyAlarmValue[group]%10000)/100);
        }
        if(g_EcBuf.BuySetValue[group] < 0)
        {
            int64 tempValue = 0 - g_EcBuf.BuySetValue[group];
            offset += sprintf(pBuf + offset, "跳闸金额: -%lld.%02lld元", tempValue/10000, (tempValue%10000)/100);
        }
        else
        {
            offset += sprintf(pBuf + offset, "跳闸金额: %lld.%02lld元", 
                              g_EcBuf.BuySetValue[group]/10000, (g_EcBuf.BuySetValue[group]%10000)/100);
        }
    }

    *msglen = offset;
    if(g_EcBuf.BuyShowLeftTimes[group] > 0)
    {
        g_EcBuf.BuyShowLeftTimes[group]--;
    }

    return g_EcBuf.BuyShowLeftTimes[group] > 0 ? TRUE : FALSE;
}

/**********************************************************************
* @name      : appctrl_callfee_guishow
* @brief     ：催费告警输出桌面显示
* @param[in] ：uint16 buflen   缓存大小
* @param[out]：uint8* pBuf     显示内容
               uint16* msglen  缓存长度
* @return    ：TRUE-下1s继续处理/FALSE-下1s不处理
* @Create    : 王津剑
* @Date      ：2020-5-21
* @Update    :
**********************************************************************/
uint8 appctrl_callfee_guishow(char* pBuf, uint16 buflen, uint16* msglen, int pinx)
{
    /* 参数内存指针 */
    char             info[256] = {0};
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计
    uint8 n = 0;
    uint8 m = 0;
    uint8 len_of_line = 0;
    uint8 single_char = 0;
#ifdef PRODUCT_ZCU_1
    len_of_line = 40;
#else
    len_of_line = 26;
#endif

    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    for (n = 0; n < ptCtrlArg->tCallFee.param.nLen; n++)
    {   
        if (ptCtrlArg->tCallFee.param.info[n] == 0)
            break;

        if((uint8)ptCtrlArg->tCallFee.param.info[n] <= 0x7f)
        {
            //小于127，说明是一个单字节的内容，如空格，英文，数字等
            single_char++;
        }

        info[n+m] = ptCtrlArg->tCallFee.param.info[n];
        if ((n+1) % len_of_line == 0)
        {
            if(single_char % 2 != 0)
            {
                //奇数个单字节字符，说明汉字被截断了，加1
                n++;
                info[n+m] = ptCtrlArg->tCallFee.param.info[n];
            }
            info[n+m+1] = 0x0a;
            m +=1;
            single_char = 0;
        }
    }

    if(pinx <= 160)
    {    
        *msglen = sprintf(pBuf, "        催费告警\n%s\n", info);
    }
    else
    {
        *msglen = sprintf(pBuf, "\n                催费告警\n\n%s\n", info);
    }

    return FALSE;
}

/**********************************************************************
* @name      : appctrl_elec_guishow
* @brief     ：电控告警和跳闸输出桌面显示
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
uint8 appctrl_elec_guishow(uint8 ctrltype, uint8 group, uint8 turnmax, uint8* pBuf, uint16 buflen, uint16* msglen)
{
    uint8 status = FALSE;
#ifdef PRODUCT_ZCU_1
    int   pinx = 240;
#else
    int   pinx = 160;
#endif

    switch(ctrltype)
    {
        case CTRL_MONTH_TYPE:
        {
            status = appctrl_month_guishow(group, turnmax, (char*)pBuf, buflen, msglen, pinx);
            break;
        }
        case CTRL_BUY_TYPE:
        {
            status = appctrl_buy_guishow(group, turnmax, (char*)pBuf, buflen, msglen, pinx);
            break;
        }
        case CTRL_CALLFEE_TYPE:
        {
            status = appctrl_callfee_guishow((char*)pBuf, buflen, msglen, pinx);
            break;
        }
        default: break;
    }
    return status;
}

/**********************************************************************
* @name      : appctrl_elec_write
* @brief     ：电量控制中间状态写入私有数据
* @param[in] ：uint8 bDay  过日
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-5-7
* @Update    :
**********************************************************************/
void appctrl_elec_write(uint8 bDay)
{
    int ret = 0;
    static uint8 s_EcBuf[sizeof(EC_INFO_T)] = {0};

    if(0 == memcmp(s_EcBuf, &g_EcBuf, sizeof(EC_INFO_T)) && FALSE == bDay)
    {
        return;
    }
    memcpy(s_EcBuf, &g_EcBuf, sizeof(EC_INFO_T));
    ret = lctrl_write_ctrl(CTRL_ELEC_TYPE, (uint8 *)&g_EcBuf, sizeof(EC_INFO_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("保存电控中间数据错误, %d\n", ret);
    }
}

