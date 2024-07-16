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
#include "oopType.h"
#include "db_api.h"
#include "framecomm.h"
#include "task_id.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"

#include "loadCtrller_data.h"
#include "loadCtrller_stat.h"

int           g_dbStat;
LCTRL_SATA_T  g_StatData;


/**********************************************************************
* @name      : lctrl_stat_time_init
* @brief     ：统计时间获取
* @param[in] ：LCTRL_PULSE_T *pPulseData
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
void lctrl_stat_time_init(OOP_DATETIME_S_T *pLastTime)
{
    uint32              readlen = 0;
    int                 ret = 0;
    OOP_DATETIME_S_T    lastTime;
    time_t              now;

    ret = lctrl_read_ctrl(CTRL_TIME_TYPE, (uint8*)&lastTime, sizeof(OOP_DATETIME_S_T), &readlen);
    if(0 == ret && readlen != sizeof(OOP_DATETIME_S_T))
    {
        PULSE_FMT_DEBUG("复位前脉冲统计时间: %04-%02d-%02d %02d:%02d:%02d\n", lastTime.year, lastTime.month,
                         lastTime.day, lastTime.hour, lastTime.minute, lastTime.second);
        memcpy(pLastTime, &lastTime, sizeof(lastTime));
    }
    else
    {
        PULSE_FMT_DEBUG("复位前脉冲统计时间: 取当前时间\n");

        now = time(NULL);
        DT_Time2DateTimeShort(now, pLastTime);
    }
}

#if DESC("脉冲采集", 1)

/**********************************************************************
* @name      : lctrl_pulse_write_OAD
* @brief     ：写入OAD
* @param[in] ：uint32 uOAD
               uint8 *pData
               uint16 dataLen
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
int lctrl_pulse_write_OAD(uint32 uOAD, uint8 *pData, uint32 dataLen)
{
    NOMAL_OAD_T tOAD;
    int         ret = 0;
    if(g_dbStat <= 0 || NULL == pData || 0 == dataLen)
    {
        return -1;
    }

    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = uOAD;
    tOAD.classtag = CLASS_DATA_NORMAL;
    ret = db_write_nomal(g_dbStat, &tOAD, pData, dataLen);
    if(0 != ret)
    {
        ret = db_write_nomal(g_dbStat, &tOAD, pData, dataLen);
        PULSE_FMT_DEBUG("pulse_write_OAD 0x%x failed, try again new ret %d\n", uOAD, ret);
        return ret;
    }
    return 0;
}

/**********************************************************************
* @name      : lctrl_pulse_read_OAD
* @brief     ：读取OAD
* @param[in] ：uint32 uOAD
               uint8 *pBuf
               uint32 bufLen
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
int lctrl_pulse_read_OAD(uint32 uOAD, uint8 *pBuf, uint32 bufLen)
{
    NOMAL_OAD_T tOAD;
    int         ret = 0;
    uint32      readLen = 0;
    
    if(g_dbStat <= 0 || NULL == pBuf || 0 == bufLen)
    {
        return -1;
    }
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = uOAD;
    ret = db_read_nomal(g_dbStat, &tOAD, bufLen, pBuf, &readLen);
    if(0 != ret || readLen != bufLen)
    {
        return -1;
    }
    return 0;
}


/**********************************************************************
* @name      : lctrl_pulse_oop_init
* @brief     ：脉冲初始化数据中心配置和数据
* @param[in] ：LCTRL_PULSE_CFG_T *pPulseOOP     脉冲
               OOP_DAYPERIOD_T *pDayPeriod      日时段表
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-6-27
* @Update    :
**********************************************************************/
void lctrl_pulse_oop_init(LCTRL_PULSE_CFG_T *pPulseOOP, OOP_DAYPERIOD_T *pDayPeriod)
{
    int                ret = 0;
    NOMAL_OAD_T        tOAD;
    uint32             readLen = 0;
    uint8              i = 0;
    LCTRL_PULSE_CFG_T *ptPM = NULL;

    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    for(i = 1; i <= OOP_MAX_PULSE; i++)
    {
        ptPM = &(pPulseOOP[i-1]);

        tOAD.oad.value = 0x24000300 + i * 0x10000; //互感器倍率
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_PTCT_T), (uint8 *)&(ptPM->tPTCT), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PTCT_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        }

        tOAD.oad.value = 0x24000400 + i * 0x10000; //脉冲配置
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_PULSESETS_T), (uint8 *)&(ptPM->tPulseCfg), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PULSESETS_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tPulseCfg), 0, sizeof(OOP_PULSESETS_T));
            continue;
        }

        tOAD.oad.value = 0x24001C00 + i * 0x10000; //脉冲配置
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_PULSESETS_B_T), (uint8 *)&(ptPM->tBigCfg), &readLen);
        if(0 != ret || readLen != sizeof(OOP_PULSESETS_B_T))
        {
            PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
        }
        
        tOAD.oad.value = 0x24000500 + i * 0x10000; //有功功率
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(int32), (uint8 *)&(ptPM->nActivePower), &readLen);
        if(readLen != sizeof(int32))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nActivePower = 0;
        }

        tOAD.oad.value = 0x24000600 + i * 0x10000; //无功功率
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(int32), (uint8 *)&(ptPM->nReactivePower), &readLen);
        if(0 != ret || readLen != sizeof(int32))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            ptPM->nReactivePower = 0;
        }

        tOAD.oad.value = 0x24000700 + i * 0x10000; //当日正向有功电量
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tDayForwActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tDayForwActEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24000800 + i * 0x10000; //当月正向有功电量
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tMonForwActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tMonForwActEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24000900 + i * 0x10000; //当日反向有功电量
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tDayBackActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tDayBackActEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24000A00 + i * 0x10000; //当月反向有功电量
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tMonBackActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tMonBackActEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24000B00 + i * 0x10000; //当日正向无功电量
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tDayForwRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tDayForwRactEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24000C00 + i * 0x10000; //当月正向无功电量
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tMonForwRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tMonForwRactEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24000D00 + i * 0x10000; //当日反向无功电量
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tDayBackRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tDayBackRactEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24000E00 + i * 0x10000; //当月反向无功电量
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tMonBackRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tMonBackRactEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24000f00 + i * 0x10000; //正向有功电能示值
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowForwActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tShowForwActEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24001000 + i * 0x10000; //正向无功电能示值
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowForwRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tShowForwRactEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24001100 + i * 0x10000; //反向有功电能示值
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowBackActEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tShowBackActEn), 0, sizeof(OOP_ENERGY_T));
        }

        tOAD.oad.value = 0x24001200 + i * 0x10000; //反向无功电能示值
        readLen = 0;
        ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_ENERGY_T), (uint8 *)&(ptPM->tShowBackRactEn), &readLen);
        if(0 != ret || readLen != sizeof(OOP_ENERGY_T))
        {
            PULSE_FMT_TRACE("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
            memset(&(ptPM->tShowBackRactEn), 0, sizeof(OOP_ENERGY_T));
        }
    }

    /* 当前套日时段表 */
    tOAD.oad.value = 0x40160200; //当前套日时段表
    readLen = 0;
    ret = db_read_nomal(g_dbStat, &tOAD, sizeof(OOP_DAYPERIOD_T), (uint8 *)pDayPeriod, &readLen);
    if(0 != ret  || readLen != sizeof(OOP_DAYPERIOD_T))
    {
        PULSE_FMT_DEBUG("Get 0x%x failed %d, len %d\n", tOAD.oad.value, ret, readLen);
    }
}

/**********************************************************************
* @name      : lctrl_pulse_arg_clear
* @brief     ：数据恢复出厂设置
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
void lctrl_pulse_arg_clear(LCTRL_PULSE_CFG_T *pPulseOOP, LCTRL_PULSE_T *pPulseData, OOP_DAYPERIOD_T *pDayPeriod)
{
    uint8 i;
    LCTRL_PULSE_CFG_T *ptMeasure = NULL;

    if(NULL == pPulseOOP || NULL == pDayPeriod)
    {
        return;
    }

    PULSE_FMT_DEBUG("参数初始化\n");

    for(i = 1; i <= OOP_MAX_PULSE; i++)
    {
        pPulseData->Pulse[i-1].caluleft = 0;
        ptMeasure = &(pPulseOOP[i-1]);

        memset(ptMeasure, 0, sizeof(LCTRL_PULSE_CFG_T));

        //互感器倍率
        memset(&(ptMeasure->tPTCT), 0, sizeof(OOP_PTCT_T));
        lctrl_pulse_read_OAD(0x24000300 + i * 0x10000, (uint8 *)&(ptMeasure->tPTCT), sizeof(OOP_PTCT_T));

        //脉冲配置
        memset(&(ptMeasure->tPulseCfg), 0, sizeof(OOP_PULSESETS_T));
        lctrl_pulse_read_OAD(0x24000400 + i * 0x10000, (uint8 *)&(ptMeasure->tPulseCfg), sizeof(OOP_PULSESETS_T));

        memset(&(ptMeasure->tBigCfg), 0, sizeof(OOP_PULSESETS_B_T));
        lctrl_pulse_read_OAD(0x24001C00 + i * 0x10000, (uint8 *)&(ptMeasure->tBigCfg), sizeof(OOP_PULSESETS_B_T));
    }

    /* 当前套日时段表 */
    memset(&pDayPeriod, 0, sizeof(OOP_DAYPERIOD_T));
    lctrl_pulse_read_OAD(0x40160200, (uint8 *)&pDayPeriod, sizeof(OOP_DAYPERIOD_T));
}

/**********************************************************************
* @name      : pulse_write_normal_init
* @brief     ：数据初始化后重写
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-6
* @Update    :
**********************************************************************/
void lctrl_pulse_data_clear(LCTRL_PULSE_CFG_T *pPulseOOP, LCTRL_PULSE_T *pPulseData)
{
    uint8 i;
    LCTRL_PULSE_CFG_T *ptMeasure = NULL;

    PULSE_FMT_DEBUG("数据初始化\n");

    /* 脉冲数据初始化 */
    for(i = 0; i < OOP_MAX_PULSE; i++)
    {
        pPulseData->Pulse[i].caluleft = 0;
        ptMeasure = &(pPulseOOP[i]);
        if(0 == ptMeasure->tPulseCfg.nNum)
        {
            continue;
        }
        ptMeasure->nActivePower = 0;
        lctrl_pulse_write_OAD(0x24010500 + i*0x10000, (uint8 *)&(ptMeasure->nActivePower), sizeof(int32));

        ptMeasure->nReactivePower = 0;
        lctrl_pulse_write_OAD(0x24010600 + i*0x10000, (uint8 *)&(ptMeasure->nReactivePower), sizeof(int32));

        memset(&(ptMeasure->tDayForwActEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tDayForwActEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tDayForwActEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tMonForwActEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tMonForwActEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tMonForwActEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tDayBackActEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tDayBackActEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tDayBackActEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tMonBackActEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tMonBackActEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tMonBackActEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tDayForwRactEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tDayForwRactEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tDayForwRactEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tMonForwRactEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tMonForwRactEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tMonForwRactEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tDayBackRactEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tDayBackRactEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tDayBackRactEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tMonBackRactEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tMonBackRactEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tMonBackRactEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tShowForwActEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tShowForwActEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24010f00 + i*0x10000, (uint8 *)&(ptMeasure->tShowForwActEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tShowBackActEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tShowBackActEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24011000 + i*0x10000, (uint8 *)&(ptMeasure->tShowBackActEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tShowForwRactEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tShowForwRactEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24011100 + i*0x10000, (uint8 *)&(ptMeasure->tShowForwRactEn), sizeof(OOP_ENERGY_T));

        memset(&(ptMeasure->tShowBackRactEn), 0, sizeof(OOP_ENERGY_T));
        ptMeasure->tShowBackRactEn.nNum = OOP_FULL_RATES;
        lctrl_pulse_write_OAD(0x24011200 + i*0x10000, (uint8 *)&(ptMeasure->tShowBackRactEn), sizeof(OOP_ENERGY_T));
    }
}

/**********************************************************************
* @name      : lctrl_pulse_update
* @brief     ：脉冲数据更新
* @param[in] ：LCTRL_PULSE_T *pPulseData
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-6-27
* @Update    :
**********************************************************************/
void lctrl_pulse_update(uint32 uOAD, uint16 info, LCTRL_PULSE_CFG_T *pPulseOOP, OOP_DAYPERIOD_T *pDayPeriod)
{
    uint8 dev = 0;

    if(0x24000300 == (uOAD & 0xfff0ff00)) //互感器倍率
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            lctrl_pulse_read_OAD(uOAD, (uint8 *)&(pPulseOOP[dev-1].tPTCT), sizeof(OOP_PTCT_T));
        }
    }
    else if(0x24000400 == (uOAD & 0xfff0ff00)) //脉冲配置
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            lctrl_pulse_read_OAD(uOAD, (uint8 *)&(pPulseOOP[dev-1].tPulseCfg), sizeof(OOP_PULSESETS_T));
        }
    }
    else if(0x24001C00 == (uOAD & 0xfff0ff00)) //脉冲配置
    {
        dev = (uint8)((uOAD & 0x000f0000) >> 16);
        if(dev <= OOP_MAX_PULSE && dev > 0)
        {
            lctrl_pulse_read_OAD(uOAD, (uint8 *)&(pPulseOOP[dev-1].tBigCfg), sizeof(OOP_PULSESETS_B_T));
        }
    }
    else if(0x40160200 == uOAD) //当前套日时段表
    {
        lctrl_pulse_read_OAD(0x40160200, (uint8 *)pDayPeriod, sizeof(OOP_DAYPERIOD_T));
    }
}

/**********************************************************************
* @name      : lctrl_pulse_file_init
* @brief     ：脉冲初数据初始化
* @param[in] ：LCTRL_PULSE_T *pPulseData
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-6-27
* @Update    :
**********************************************************************/
void lctrl_pulse_file_init(LCTRL_PULSE_T *pPulseData)
{
    LCTRL_PULSE_TEMP_T  tPulseTemp[OOP_MAX_PULSE];
    uint32              readlen = 0;
    int                 ret = 0;
    uint8               i = 0;

    ret = lctrl_read_ctrl(CTRL_PULSE_TYPE,  (uint8*)tPulseTemp, OOP_MAX_PULSE * sizeof(LCTRL_PULSE_TEMP_T), &readlen);
    if(0 == ret && readlen != OOP_MAX_PULSE * sizeof(LCTRL_PULSE_TEMP_T))
    {
        memcpy(&(pPulseData->tPulseTemp[0]), &(tPulseTemp[0]), OOP_MAX_PULSE * sizeof(LCTRL_PULSE_TEMP_T));
        char  strtemp[2048] = {0};
        uint16 offset = 0;
        /* 设备返回数据压缩 */
        for(i = 0; i < OOP_MAX_PULSE; i++)
        {
            pPulseData->Pulse[i].val += pPulseData->tPulseTemp[i].count;
            pPulseData->Pulse[i].interval += pPulseData->tPulseTemp[i].timems;
            offset += snprintf(strtemp + offset, 2048 - offset,
                              " %d路: total, %u-%u.%03u(s), single %u-%u(ms)\n", i + 1,
                              tPulseTemp[i].totalcou, tPulseTemp[i].totals, tPulseTemp[i].totalms,
                              tPulseTemp[i].count, tPulseTemp[i].timems);
        }
        PULSE_FMT_DEBUG("复位前脉冲数据:\n%s\n", strtemp);
    }
    else
    {
        PULSE_FMT_DEBUG("复位前脉冲数据: 无\n");
    }
}

/**********************************************************************
* @name      : lctrl_get_pulse_rateNO
* @brief     ：获取当前费率号
* @param[in] ：uint8 hours
               uint8 mins
* @param[out]：
* @return    ：费率号
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
uint8 lctrl_get_pulse_rateNO(uint8 hours, uint8 mins)
{
    uint8 i;
    uint32 mintotal = 0;
    DAYPERIOD *ptPeriod = NULL;
    
    if(hours >= 24 || mins >= 60)
    {
        return 0;
    }

    ptPeriod = &(g_StatData.tDayPeriod.dPeriod[0]); //只需要取第0个
    if(0 == ptPeriod->nNum)
    {
        return EN_LEVEL; //默认费率号
    }
    else if(1 == ptPeriod->nNum)
    {
        return ptPeriod->period[0].rateNO;
    }
    
    mintotal = hours * 60 + mins;
    for(i = 0; i < ptPeriod->nNum - 1; i++)
    {
        if(ptPeriod->period[i].hour * 60 + ptPeriod->period[i].min <= mintotal &&
            mintotal < ptPeriod->period[i+1].hour * 60 + ptPeriod->period[i+1].min)
        {
            return ptPeriod->period[i].rateNO;
        }
    }
    return ptPeriod->period[i].rateNO;
}

/**********************************************************************
* @name      : lctrl_get_pulse
* @brief     ：获取脉冲统计数据
* @param[in] ：LCTRL_PULSE_T *pPulseData     脉冲数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
void lctrl_get_pulse(LCTRL_PULSE_T *pPulseData)
{
    int    ret = 0;
    uint8  no = 0xFF;
    uint8  recvBuf[512] = {0};
    uint16 recvlen = 0;
    uint8  i = 0;
    uint8  j;
    uint8  index = 0;  //脉冲数量
    uint8  pulseNum = 0;
    RST_PULSE_STATIC_T tPulse;
    char   strtemp[1200] = {0};
    uint16 len = 0;
    uint64 lastms = 0;
    uint64 endms = 0;
    
    PULSE_FMT_DEBUG("获取所有脉冲\n");

    ret = lctrl_send_recv_appmsg(0, MESSAGE_IID_RSPSAMPLE, RSPSAMPLE_IOP_PULSE_STATIC,  &no, sizeof(uint8),
                                 recvBuf, 512, &recvlen, 3);
    if(0 != ret)
    {
        PULSE_FMT_DEBUG("获取所有脉冲失败\n");
    }
    else
    {
        /* 脉冲数据解析 */
        memset(&tPulse, 0, sizeof(RST_PULSE_STATIC_T));
        PULSE_BUF_DEBUG(recvBuf, recvlen, "获取消息");
        appmsg_rsppulse_split(recvBuf, &tPulse);

        for(j = 0; j < tPulse.num; j++)
        {
            if(0 == tPulse.item[j].flag)
            {
                continue;
            }
            /* 判断是否重启 携带启动时间的接口 */
            lastms = pPulseData->tPulseTemp[index].totals;
            lastms = lastms * 1000 + pPulseData->tPulseTemp[index].totalms;
            endms = tPulse.item[j].timesec;
            endms = endms * 1000 + tPulse.item[j].timems;

            if(0 != memcmp(&(pPulseData->tPulseTemp[index].start), &(tPulse.item[j].start), sizeof(Date_Time)))
            {
                /* 如果采集到统计时间过大（1天），就需要放弃当前采集到的数据 */
                if(tPulse.item[j].timesec < 1 * 24 * 3600)
                {
                    pPulseData->tPulseTemp[index].count += tPulse.item[j].count;
                    pPulseData->tPulseTemp[index].timems += (uint32)endms;
                }
                else
                {
                    PULSE_FMT_DEBUG("端口%d统计超过1天,丢弃\n", j + 1);
                }
                pPulseData->tPulseTemp[index].start = tPulse.item[j].start;
            }
            else if(lastms <= endms)
            {
                if(tPulse.item[j].count >= pPulseData->tPulseTemp[index].totalcou)
                {
                    pPulseData->tPulseTemp[index].count += tPulse.item[j].count - pPulseData->tPulseTemp[index].totalcou;
                }
                else
                {
                    /* 反转 */
                    pPulseData->tPulseTemp[index].count += (0xFFFFFFFF - pPulseData->tPulseTemp[index].totalcou) +
                                                       tPulse.item[j].count + 1;

                    /* 数量太多（1千6百万），认为无效 */
                    if(pPulseData->tPulseTemp[index].count >= 0xFFFFFF)
                    {
                        PULSE_FMT_DEBUG("端口%d数量溢出无效, %u\n", j + 1, pPulseData->tPulseTemp[index].count);
                        pPulseData->tPulseTemp[index].count = 0;
                    }
                }
                pPulseData->tPulseTemp[index].timems += (uint32)(endms - lastms);

                /* 1s超多200个脉冲无效 */
                if(pPulseData->tPulseTemp[index].count / (pPulseData->tPulseTemp[index].timems/1000 + 1) > 200)
                {
                    PULSE_FMT_DEBUG("端口%d增加数量无效, 增加%u %ums\n", j + 1, pPulseData->tPulseTemp[index].count,
                                                                 pPulseData->tPulseTemp[index].timems);
                    pPulseData->tPulseTemp[index].count = 0;
                }
            }
            else
            {
                PULSE_FMT_DEBUG("端口%d统计时间溢出\n", j + 1);
            }

            pPulseData->tPulseTemp[index].totalcou = tPulse.item[j].count;
            pPulseData->tPulseTemp[index].totals = tPulse.item[j].timesec;
            pPulseData->tPulseTemp[index].totalms = tPulse.item[j].timems;

            len += snprintf(strtemp + len, 1200 - len,"端口%d: 总数%u, 时间 %u.%03us, 增加%u, 时间%ums\n",
                           j + 1, tPulse.item[j].count, tPulse.item[j].timesec, tPulse.item[j].timems,
                           pPulseData->tPulseTemp[index].count, pPulseData->tPulseTemp[index].timems);
            index++;
        }
        PULSE_FMT_DEBUG("脉冲数量%d\n%s\n", tPulse.num, strtemp);
    }

    /* 设备返回数据压缩 */
    for(i = 0; i < OOP_MAX_PULSE && i < index; i++)
    {
        pPulseData->Pulse[pulseNum].val = pPulseData->tPulseTemp[i].count;
        pPulseData->Pulse[pulseNum].interval = pPulseData->tPulseTemp[i].timems;
        pulseNum++;
    }

    pPulseData->pulseNum = pulseNum;
    if(pPulseData->pulseNum < OOP_MAX_PULSE)
    {
        memset(&(pPulseData->Pulse[pulseNum]), 0, (OOP_MAX_PULSE - pulseNum) * sizeof(LCTRL_PULSE_COUNT_T));
    }
}

#endif

#if DESC("脉冲计算", 1)
uint8       g_LastRate = 0;

/**********************************************************************
* @name      : cal_pls_powA
* @brief     ：采用脉冲功率算法A计算实时功率(算法A:利用一分钟内的脉冲数来计算)
               计算公式:P =3600*(10000×n)/(电表常数K×Δt)  
               单位:kw, 10000代表4位小数
* @param[in] ：uint32 plsNum  累计的脉冲数
               uint32 delta   累计时间
               uint32 metK    电表常数
* @param[out]：
* @return    ：1分钟功率
* @Create    : 王津剑
* @Date      ：2019-11-21
* @Update    :
**********************************************************************/
uint32 cal_pls_powA(uint32 plsNum, uint32 delta, uint32 metK)
{
    uint32 P;
    double f;
    //uint32 maxamend;
    uint32 newdeltime = 0;
    if(0 == metK || metK == 0xffff)
    {
        metK  = 5000;
    }
    if(metK == 0 || plsNum == 0)
    {
        P = 0;
        return P;
    }
    if(plsNum == 1)    /*一分钟内只有一个脉冲*/
    {
        delta = 60000;/* 1min的毫秒时间 */
    }
#if 0
    //统计方式计算 不能减去1
    else
    {
        plsNum--;
    }
#endif
    newdeltime = delta;
#if 0
    /* 统计时间修正 最大调整时间是MIN(1s, 四分之一个周期). 58-62s统计时间内进行调整 */
    maxamend = MIN(1000, delta/(4 * plsNum));
    if(delta > 60000)
    {
        if(delta <= 60000 + maxamend)
        {
            newdeltime = 60000;
        }
        else if(delta <= 62000)
        {
            newdeltime = delta - maxamend;
        }
    }
    else if(delta < 60000)
    {
        if(delta >= 60000 - maxamend)
        {
            newdeltime = 60000;
        }
        else if(delta > 58000)
        {
            newdeltime = delta + maxamend;
        }
    }
    PULSE_FMT_TRACE("Static time %u, amend time %u\n", delta, newdeltime);
#endif
    if((newdeltime > 0)&&(metK > 0))
    {
        f = (10000.0 / metK);
        f = (f / newdeltime) * 1000;
        P = (3600L) * plsNum * f;    /*时间转换为20ms*//*模组化平台不需要转换20ms */
    }
    else
        P = 0;
    return P;
}

/**********************************************************************
* @name      : cal_pls_en
* @brief     ：脉冲电量计算
* @param[in] ：uint32 plsNum     脉冲数
               uint32 *pleftNum 上次计算余数
               uint32 metK      电表常数
* @param[out]：uint32 *pleftNum 没有参与计算余数
* @return    ：电量en 单位 0.1W
* @Create    : 王津剑
* @Date      ：2019-11-21
* @Update    :
* @Drief     :电量en = （（脉冲数Pls  + 上次计算余数）/ 电表常数K）×10000 取整
              余数rem= （（脉冲数Pls  + 上次计算余数）/ 电表常数K）×10000 取余
              此处10000代表4位小数
**********************************************************************/
uint32 cal_pls_en(uint32 plsNum, uint32 *pleftNum, uint32 metK)
{
    uint32  en = 0;
    
    if(NULL == pleftNum)
    {
        return 0;
    }

    if(0 == metK || metK == 0xffff)
    {
        metK = 5000;
    }
    if(metK > 0)
    {
        en  =(uint32)(plsNum*10000+*pleftNum)/metK;       /*--计算电量--*/
        *pleftNum = (uint32)((plsNum*10000+*pleftNum)%metK);        /*--剩余脉冲取整--*/

//        en = (uint64)((plsNum + *rem) * 10000L / metK);       /*计算电量*/
//        *rem = (uint16)(((plsNum + *rem) * 10000L % metK) / 10000L);/*剩余脉冲取整*/
    }
    else
    {
        *pleftNum = 0;
           en = 0;
    }
    return en;
}


/**********************************************************************
* @name      : lctrl_pulse_pow_en
* @brief     ：功率电量计算(利用一分钟内的脉冲数来计算) 计算功率电量
* @param[in] ：uint32 count     脉冲数
               uint32 timems    持续时间ms 
               uint32* pleftNum 剩余计算余数
               uint32 K         电表常数
* @param[out]：uint32* pleftNum 剩余计算余数
               uint32 *pEn      电量
* @return    ：功率
* @Create    : 王津剑
* @Date      ：2019-11-21
* @Update    :
**********************************************************************/
uint32 lctrl_pulse_pow_en(uint32 count, uint32 timems, uint32 K, uint32* pleftNum, uint32 *pEn)
{
    uint32             p = 0;     //功率

    if(NULL == pleftNum || NULL == pEn)
    {
        return 0;
    }

    if(0 != count)  //通道无数据
    {
        p = cal_pls_powA(count, timems, K);           /*计算功率,采用算法A*/
    }
    
    *pEn = cal_pls_en(count, pleftNum, K);
    
    return p;
}

/**********************************************************************
* @name      : lctrl_pulse_calculate
* @brief     ：脉冲计算
* @param[in] : LCTRL_PULSE_CFG_T *pPulseOOP   OAD数据
               LCTRL_PULSE_T *pPulseData      计算数据
               TIME_STRUCT_T *pTime  当前时间
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
void lctrl_pulse_calculate(LCTRL_PULSE_CFG_T *pPulseOOP, LCTRL_PULSE_T *pPulseData, OOP_DATETIME_S_T *pTime,
                                      uint8 bDay, uint8 bMonth)
{
    uint8  i = 0, j = 0;
    uint8  rateNO = 0;
    uint8  pulseLocate = 0;
    uint8  powerid = 0;
    uint32 metK = 5000; //电表常数 默认是5000
    LCTRL_PULSE_CFG_T *ptMeasure = NULL;
    uint32 power;
    uint32 en;
    int    showEner = 0;
    uint32 showEnerTotal = 0;
    struct tm systime;
    struct timeval tv;
    struct timezone tz;
    uint8  rateNOLast = 0;
    char  *posname[NEG_RE_Tag + 1] = {"正向有功", "正向无功", "反向有功", "反向无功"};

    if(0 == pPulseData->pulseNum || pPulseData->pulseNum > OOP_MAX_PULSE)
    {
        return;
    }

    /* 获取当前费率 */
    rateNO = lctrl_get_pulse_rateNO(pTime->hour, pTime->minute);
    PULSE_FMT_DEBUG("%02d:%02d:%02d 费率%d\n", pTime->hour, pTime->minute, pTime->second, rateNO);
    if(0 == rateNO || rateNO > OOP_MAX_RATES)
    {
        goto end;
    }

    if(0 == g_LastRate)
    {
        g_LastRate = rateNO;
    }

    /* 获取上一分钟的费率 防止对时导致电量统计到上一个时段内 */
    gettimeofday(&tv, &tz);
    tv.tv_sec -= 60;
    FastSecondToDate(&tv, &tz, &systime);
    rateNOLast = lctrl_get_pulse_rateNO(systime.tm_hour, systime.tm_min);
    if(rateNOLast == rateNO)
    {
        g_LastRate = rateNO;
    }

    for(i = 0; i < OOP_MAX_PULSESETS; i++)
    {
        ptMeasure = &(pPulseOOP[i]);
        if(0 == ptMeasure->tPulseCfg.nNum)
        {
            /* 没有配置功率写0 */
            ptMeasure->nActivePower = 0;
            ptMeasure->nReactivePower = 0;
            lctrl_pulse_write_OAD(0x24010500+i*0x10000, (uint8 *)&(ptMeasure->nActivePower), sizeof(int32));
            lctrl_pulse_write_OAD(0x24010600+i*0x10000, (uint8 *)&(ptMeasure->nReactivePower), sizeof(int32));
            continue;
        }

        ptMeasure->nActivePower = 0;
        ptMeasure->nReactivePower = 0;
        for(j = 0; j < ptMeasure->tPulseCfg.nNum; j++)
        {
            /* port.value = 0xf20a02XX */
            pulseLocate = ptMeasure->tPulseCfg.pulse[j].port.nIndex;
            powerid = ptMeasure->tPulseCfg.pulse[j].property;
            metK = ptMeasure->tPulseCfg.pulse[j].K;
            if(0 != ptMeasure->tPulseCfg.rsv && j < ptMeasure->tBigCfg.nNum)
            {
                /* 江苏地区使用大脉冲 */
                metK = ptMeasure->tBigCfg.pulse[j].K;
            }
            if(metK==0)
            {
                metK = 5000;
            }
            if(pulseLocate > pPulseData->pulseNum || 0 == pulseLocate)
            {
                continue;
            }

            pulseLocate -= 1;   //下标从0开始

            /*脉冲计量点功率和电量计算*/
            power = lctrl_pulse_pow_en(pPulseData->Pulse[pulseLocate].val, pPulseData->Pulse[pulseLocate].interval, metK,
                                      &(pPulseData->Pulse[pulseLocate].caluleft), &en);

            /* 需量计算 虽然没有OAD存放 */
            if(g_LastRate != rateNO)
            {
                /*不在同一个费率时段,则清空需量滑差区*/
                memset(pPulseData->Pulse[pulseLocate].powerList, 0, DM_POWER_MAX * sizeof(uint32));
                pPulseData->Pulse[pulseLocate].powerNum = 0;
            }
            //powerDM = cal_pls_dm(power, &(ptCalu->Pulse[pulseLocate].powerNum), ptCalu->Pulse[pulseLocate].powerList);
            //PULSE_FMT_TRACE("%d dev index %d DM power is %u\n", i, pulseLocate + 1, powerDM);

            switch(powerid)
            {
                case POS_AE_Tag://正向有功
                {
                    ptMeasure->nActivePower += (int)power;
                    ptMeasure->tShowForwActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowForwActEn.nValue[0] += en;
                    ptMeasure->tShowForwActEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowForwActEn.nValue[0];
                    showEner = ptMeasure->tShowForwActEn.nValue[g_LastRate];

                    ptMeasure->tDayForwActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tDayForwActEn.nValue[0] += en;
                    ptMeasure->tDayForwActEn.nValue[g_LastRate] += en;
                    ptMeasure->tMonForwActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tMonForwActEn.nValue[0] += en;
                    ptMeasure->tMonForwActEn.nValue[g_LastRate] += en;
                    break;
                }
                case POS_RE_Tag://正向无功
                {
                    ptMeasure->nReactivePower += (int)power;
                    ptMeasure->tShowForwRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowForwRactEn.nValue[0] += en;
                    ptMeasure->tShowForwRactEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowForwRactEn.nValue[0];
                    showEner = ptMeasure->tShowForwRactEn.nValue[g_LastRate];

                    ptMeasure->tDayForwRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tDayForwRactEn.nValue[0] += en;
                    ptMeasure->tDayForwRactEn.nValue[g_LastRate] += en;
                    ptMeasure->tMonForwRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tMonForwRactEn.nValue[0] += en;
                    ptMeasure->tMonForwRactEn.nValue[g_LastRate] += en;
                    break;
                }
                case NEG_AE_Tag://反向有功
                {
                    ptMeasure->nActivePower -= ((int)power);
                    ptMeasure->tShowBackActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowBackActEn.nValue[0] += en;
                    ptMeasure->tShowBackActEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowBackActEn.nValue[0];
                    showEner = ptMeasure->tShowBackActEn.nValue[g_LastRate];

                    ptMeasure->tDayBackActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tDayBackActEn.nValue[0] += en;
                    ptMeasure->tDayBackActEn.nValue[g_LastRate] += en;
                    ptMeasure->tMonBackActEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tMonBackActEn.nValue[0] += en;
                    ptMeasure->tMonBackActEn.nValue[g_LastRate] += en;
                    break;
                }
                case NEG_RE_Tag://反向无功
                {
                    ptMeasure->nReactivePower -= ((int)power);
                    ptMeasure->tShowBackRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tShowBackRactEn.nValue[0] += en;
                    ptMeasure->tShowBackRactEn.nValue[g_LastRate] += en;
                    showEnerTotal = ptMeasure->tShowBackRactEn.nValue[0];
                    showEner = ptMeasure->tShowBackRactEn.nValue[g_LastRate];

                    ptMeasure->tDayBackRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tDayBackRactEn.nValue[0] += en;
                    ptMeasure->tDayBackRactEn.nValue[g_LastRate] += en;
                    ptMeasure->tMonBackRactEn.nNum = OOP_FULL_RATES;
                    ptMeasure->tMonBackRactEn.nValue[0] += en;
                    ptMeasure->tMonBackRactEn.nValue[g_LastRate] += en;
                    break;
                }
                default: break;
            }

            PULSE_FMT_DEBUG("脉冲%d 端口%d(%d%s): 功率%u, 电量%u, K %u, 剩余%u; 总电量%u, 费率%d电量%u\n",
                            i + 1, pulseLocate + 1, powerid, powerid <= NEG_RE_Tag ? posname[powerid] : "未知",
                            power, en, metK, pPulseData->Pulse[pulseLocate].caluleft,
                            showEnerTotal, g_LastRate, showEner);
        }
        
        lctrl_pulse_write_OAD(0x24010500+i*0x10000, (uint8 *)&(ptMeasure->nActivePower), sizeof(int32));
        lctrl_pulse_write_OAD(0x24010600+i*0x10000, (uint8 *)&(ptMeasure->nReactivePower), sizeof(int32));

        if(TRUE == bDay)
        {
            /* 清理日数据 */
            if(ptMeasure->tDayForwActEn.nNum > 0)
            {
                memset(&(ptMeasure->tDayForwActEn), 0, sizeof(OOP_ENERGY_T));
                ptMeasure->tDayForwActEn.nNum = OOP_FULL_RATES;
            }
            if(ptMeasure->tDayBackActEn.nNum > 0)
            {
                memset(&(ptMeasure->tDayBackActEn), 0, sizeof(OOP_ENERGY_T));
                ptMeasure->tDayBackActEn.nNum = OOP_FULL_RATES;
            }
            if(ptMeasure->tDayForwRactEn.nNum > 0)
            {
                memset(&(ptMeasure->tDayForwRactEn), 0, sizeof(OOP_ENERGY_T));
                ptMeasure->tDayForwRactEn.nNum = OOP_FULL_RATES;
            }
            if(ptMeasure->tDayBackRactEn.nNum > 0)
            {
                memset(&(ptMeasure->tDayBackRactEn), 0, sizeof(OOP_ENERGY_T));
                ptMeasure->tDayBackRactEn.nNum = OOP_FULL_RATES;
            }
        }
        if(TRUE == bMonth)
        {
            /* 清理月数据 */
            if(ptMeasure->tMonForwActEn.nNum > 0)
            {
                memset(&(ptMeasure->tMonForwActEn), 0, sizeof(OOP_ENERGY_T));
                ptMeasure->tMonForwActEn.nNum = OOP_FULL_RATES;
            }
            if(ptMeasure->tMonBackActEn.nNum > 0)
            {
                memset(&(ptMeasure->tMonBackActEn), 0, sizeof(OOP_ENERGY_T));
                ptMeasure->tMonBackActEn.nNum = OOP_FULL_RATES;
            }
            if(ptMeasure->tMonForwRactEn.nNum > 0)
            {
                memset(&(ptMeasure->tMonForwRactEn), 0, sizeof(OOP_ENERGY_T));
                ptMeasure->tMonForwRactEn.nNum = OOP_FULL_RATES;
            }
            if(ptMeasure->tMonBackRactEn.nNum > 0)
            {
                memset(&(ptMeasure->tMonBackRactEn), 0, sizeof(OOP_ENERGY_T));
                ptMeasure->tMonBackRactEn.nNum = OOP_FULL_RATES;
            }
        }
        lctrl_pulse_write_OAD(0x24010700+i*0x10000, (uint8 *)&(ptMeasure->tDayForwActEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24010800+i*0x10000, (uint8 *)&(ptMeasure->tMonForwActEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24010900+i*0x10000, (uint8 *)&(ptMeasure->tDayBackActEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24010A00+i*0x10000, (uint8 *)&(ptMeasure->tMonBackActEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24010B00+i*0x10000, (uint8 *)&(ptMeasure->tDayForwRactEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24010C00+i*0x10000, (uint8 *)&(ptMeasure->tMonForwRactEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24010D00+i*0x10000, (uint8 *)&(ptMeasure->tDayBackRactEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24010E00+i*0x10000, (uint8 *)&(ptMeasure->tMonBackRactEn), sizeof(OOP_ENERGY_T));
        
        lctrl_pulse_write_OAD(0x24010f00+i*0x10000, (uint8 *)&(ptMeasure->tShowForwActEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24011000+i*0x10000, (uint8 *)&(ptMeasure->tShowForwRactEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24011100+i*0x10000, (uint8 *)&(ptMeasure->tShowBackActEn), sizeof(OOP_ENERGY_T));
        lctrl_pulse_write_OAD(0x24011200+i*0x10000, (uint8 *)&(ptMeasure->tShowBackRactEn), sizeof(OOP_ENERGY_T));
    }

    g_LastRate = rateNO;
end:
    for(i = 0; i < OOP_MAX_PULSE; i++)
    {
        pPulseData->Pulse[i].val = 0;
        pPulseData->Pulse[i].interval = 0;
        pPulseData->tPulseTemp[i].count = 0;
        pPulseData->tPulseTemp[i].timems = 0;
    }
}

/**********************************************************************
* @name      : lctrl_pulse_change_K
* @brief     ：电表常数发生变化, 计算后取新的K值
* @param[in] ：uint32 OAD   OAD
               LCTRL_PULSE_CFG_T *pPulseOOP   OAD数据
               LCTRL_PULSE_T *pPulseData      计算数据
               TIME_STRUCT_T *pTime  当前时间
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2024-6-28
* @Update    :
**********************************************************************/
void lctrl_pulse_change_K(uint32 OAD, LCTRL_PULSE_CFG_T *pPulseOOP, LCTRL_PULSE_T *pPulseData, 
                                    OOP_DATETIME_S_T *pTime)
{
    uint8  j;
    uint8  pulseLocate = 0;
    uint8  powerid = 0;
    uint32 metK = 5000; //电表常数 默认是5000
    uint32 newK = 0;    //电表常数
    LCTRL_PULSE_CFG_T *ptMeasure = NULL;
    uint32 power;
    //uint32 powerDM;
    uint32 en;
    uint8 i = 0, k = 0;
    int    showEner = 0;
    uint32 showEnerTotal = 0;
    uint8 dev = 0;
    OOP_PULSESETS_T      tnewPulseCfg;     //脉冲配置
    OOP_PULSESETS_B_T    tnewBigCfg;
    int retS = 1;
    int retB = 1;
    char  *posname[NEG_RE_Tag + 1] = {"正向有功", "正向无功", "反向有功", "反向无功"};

    j = (uint8)((OAD & 0x000f0000) >> 16);
    dev = j - 1;

    if(dev >= OOP_MAX_PULSE)
    {
        return;
    }

    if(0 == pPulseData->pulseNum || pPulseData->pulseNum > OOP_MAX_PULSE)
    {
        goto end;
    }

    if(0x24000400 == (OAD & 0xfff0ff00)) //脉冲配置
    {
         retS = lctrl_pulse_read_OAD(OAD, (uint8 *)&tnewPulseCfg, sizeof(OOP_PULSESETS_T));
    }
    else if(0x24001C00 == (OAD & 0xfff0ff00)) //脉冲配置
    {
        retB = lctrl_pulse_read_OAD(OAD, (uint8 *)&tnewBigCfg, sizeof(OOP_PULSESETS_B_T));
    }

    /* 获取最新的脉冲 */
    lctrl_get_pulse(pPulseData);

    /* 取历史费率 */
    if(0 == g_LastRate || g_LastRate > OOP_MAX_RATES)
    {
        g_LastRate = lctrl_get_pulse_rateNO(pTime->hour, pTime->minute);
        PULSE_FMT_TRACE("hour %d, min %d, sec %d, rate %d\n", pTime->hour, pTime->minute, pTime->second, g_LastRate);
        if(0 == g_LastRate || g_LastRate > OOP_MAX_RATES)
        {
            goto end;
        }
    }

    ptMeasure = &(pPulseOOP[dev]);
    if(0 == ptMeasure->tPulseCfg.nNum)
    {
        goto end;
    }

    for(j = 0; j < ptMeasure->tPulseCfg.nNum; j++)
    {
        pulseLocate = ptMeasure->tPulseCfg.pulse[j].port.value & 0x000000ff;
        powerid = ptMeasure->tPulseCfg.pulse[j].property;
        metK = ptMeasure->tPulseCfg.pulse[j].K;
        if(0 != ptMeasure->tPulseCfg.rsv && j < ptMeasure->tBigCfg.nNum)
        {
            /* 江苏地区使用大脉冲 */
            metK = ptMeasure->tBigCfg.pulse[j].K;
        }
        if(metK==0)
        {
            metK = 5000;
        }
        if(pulseLocate > pPulseData->pulseNum || 0 == pulseLocate || pulseLocate > OOP_MAX_PULSE)
        {
            continue;
        }

        pulseLocate -= 1;   //下表从0开始

        /*脉冲计量点功率和电量计算*/
        ptMeasure->nActivePower = 0;
        ptMeasure->nReactivePower = 0;
        power = lctrl_pulse_pow_en(pPulseData->Pulse[pulseLocate].val, pPulseData->Pulse[pulseLocate].interval, metK,
                                  &(pPulseData->Pulse[pulseLocate].caluleft), &en);
        PULSE_FMT_DEBUG("端口%d: 功率%u, 电量%u, K %u, 剩余%u\n",
                        pulseLocate + 1, power, en, metK, pPulseData->Pulse[pulseLocate].caluleft);

        /* 需量计算 虽然没有OAD存放 */
//        powerDM = cal_pls_dm(power, &(ptCalu->Pulse[pulseLocate].powerNum), ptCalu->Pulse[pulseLocate].powerList);
//        PULSE_FMT_TRACE("DM power is %u\n", powerDM);

        /* K值变化, 剩余计算变化 */
        if(0x24000400 == (OAD & 0xfff0ff00))
        {
            if(0 != retS)
            {
                pPulseData->Pulse[pulseLocate].caluleft = 0;
            }
            else if(0 == tnewPulseCfg.rsv)
            {
                for(k = 0; k < tnewPulseCfg.nNum; k++)
                {
                    if(pulseLocate + 1 == (tnewPulseCfg.pulse[k].port.value & 0x000000ff))
                    {
                        newK = tnewPulseCfg.pulse[k].K;
                        break;
                    }
                }
                if(newK > 0)
                {
                    pPulseData->Pulse[pulseLocate].caluleft = (pPulseData->Pulse[pulseLocate].caluleft * 1L * newK)/metK;

                }
            }
        }
        if(0x24001C00 == (OAD & 0xfff0ff00))
        {
            if(0 != retB)
            {
                pPulseData->Pulse[pulseLocate].caluleft = 0;
            }
            else
            {
                for(k = 0; k < tnewBigCfg.nNum; k++)
                {
                    if(pulseLocate + 1 == (tnewBigCfg.pulse[k].port.value & 0x000000ff))
                    {
                        newK = tnewBigCfg.pulse[k].K;
                        break;
                    }
                }
                if(newK > 0)
                {
                    pPulseData->Pulse[pulseLocate].caluleft = (pPulseData->Pulse[pulseLocate].caluleft * 1L * newK)/metK;

                }
            }
        }

        PULSE_FMT_DEBUG("端口%d更新剩余%u\n", pulseLocate + 1, pPulseData->Pulse[pulseLocate].caluleft);
        switch(powerid)
        {
            case POS_AE_Tag:
            {
                ptMeasure->nActivePower = (int)power;
                ptMeasure->tShowForwActEn.nNum = 5;
                ptMeasure->tShowForwActEn.nValue[0] += en;
                ptMeasure->tShowForwActEn.nValue[g_LastRate] += en;
                showEnerTotal = ptMeasure->tShowForwActEn.nValue[0];
                showEner = ptMeasure->tShowForwActEn.nValue[g_LastRate];

                ptMeasure->tDayForwActEn.nNum = OOP_FULL_RATES;
                ptMeasure->tDayForwActEn.nValue[0] += en;
                ptMeasure->tDayForwActEn.nValue[g_LastRate] += en;
                ptMeasure->tMonForwActEn.nNum = OOP_FULL_RATES;
                ptMeasure->tMonForwActEn.nValue[0] += en;
                ptMeasure->tMonForwActEn.nValue[g_LastRate] += en;
                break;
            }
            case POS_RE_Tag:
            {
                ptMeasure->nReactivePower = (int)power;
                ptMeasure->tShowForwRactEn.nNum = 5;
                ptMeasure->tShowForwRactEn.nValue[0] += en;
                ptMeasure->tShowForwRactEn.nValue[g_LastRate] += en;
                showEnerTotal = ptMeasure->tShowForwRactEn.nValue[0];
                showEner = ptMeasure->tShowForwRactEn.nValue[g_LastRate];

                ptMeasure->tDayForwRactEn.nNum = OOP_FULL_RATES;
                ptMeasure->tDayForwRactEn.nValue[0] += en;
                ptMeasure->tDayForwRactEn.nValue[g_LastRate] += en;
                ptMeasure->tMonForwRactEn.nNum = OOP_FULL_RATES;
                ptMeasure->tMonForwRactEn.nValue[0] += en;
                ptMeasure->tMonForwRactEn.nValue[g_LastRate] += en;
                break;
            }
            case NEG_AE_Tag:
            {
                ptMeasure->nActivePower = -((int)power);
                ptMeasure->tShowBackActEn.nNum = 5;
                ptMeasure->tShowBackActEn.nValue[0] += en;
                ptMeasure->tShowBackActEn.nValue[g_LastRate] += en;
                showEnerTotal = ptMeasure->tShowBackActEn.nValue[0];
                showEner = ptMeasure->tShowBackActEn.nValue[g_LastRate];

                ptMeasure->tDayBackActEn.nNum = OOP_FULL_RATES;
                ptMeasure->tDayBackActEn.nValue[0] += en;
                ptMeasure->tDayBackActEn.nValue[g_LastRate] += en;
                ptMeasure->tMonBackActEn.nNum = OOP_FULL_RATES;
                ptMeasure->tMonBackActEn.nValue[0] += en;
                ptMeasure->tMonBackActEn.nValue[g_LastRate] += en;
                break;
            }
            case NEG_RE_Tag:
            {
                ptMeasure->nReactivePower = -((int)power);
                ptMeasure->tShowBackRactEn.nNum = 5;
                ptMeasure->tShowBackRactEn.nValue[0] += en;
                ptMeasure->tShowBackRactEn.nValue[g_LastRate] += en;
                showEnerTotal = ptMeasure->tShowBackRactEn.nValue[0];
                showEner = ptMeasure->tShowBackRactEn.nValue[g_LastRate];

                ptMeasure->tDayBackRactEn.nNum = OOP_FULL_RATES;
                ptMeasure->tDayBackRactEn.nValue[0] += en;
                ptMeasure->tDayBackRactEn.nValue[g_LastRate] += en;
                ptMeasure->tMonBackRactEn.nNum = OOP_FULL_RATES;
                ptMeasure->tMonBackRactEn.nValue[0] += en;
                ptMeasure->tMonBackRactEn.nValue[g_LastRate] += en;
                break;
            }
            default: break;
        }
        PULSE_FMT_DEBUG("脉冲%d 端口%d(%d%s): 功率%u, 电量%u, K %u, 剩余%u; 总电量%u, 费率%d电量%u\n",
                        i + 1, pulseLocate + 1, powerid, powerid <= NEG_RE_Tag ? posname[powerid] : "未知",
                        power, en, metK, pPulseData->Pulse[pulseLocate].caluleft,
                        showEnerTotal, g_LastRate, showEner);

        pPulseData->Pulse[pulseLocate].val = 0;
        pPulseData->Pulse[pulseLocate].interval = 0;

        for(i = 0; i < OOP_MAX_PULSE; i++)
        {
            if(k == pulseLocate)
            {
                pPulseData->tPulseTemp[i].count = 0;
                pPulseData->tPulseTemp[i].timems = 0;
                break;
            }
            k++;
        }
    }
    lctrl_pulse_write_OAD(0x24010500+i*0x10000, (uint8 *)&(ptMeasure->nActivePower), sizeof(int32));
    lctrl_pulse_write_OAD(0x24010600+i*0x10000, (uint8 *)&(ptMeasure->nReactivePower), sizeof(int32));

    lctrl_pulse_write_OAD(0x24010700+i*0x10000, (uint8 *)&(ptMeasure->tDayForwActEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24010800+i*0x10000, (uint8 *)&(ptMeasure->tMonForwActEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24010900+i*0x10000, (uint8 *)&(ptMeasure->tDayBackActEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24010A00+i*0x10000, (uint8 *)&(ptMeasure->tMonBackActEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24010B00+i*0x10000, (uint8 *)&(ptMeasure->tDayForwRactEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24010C00+i*0x10000, (uint8 *)&(ptMeasure->tMonForwRactEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24010D00+i*0x10000, (uint8 *)&(ptMeasure->tDayBackRactEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24010E00+i*0x10000, (uint8 *)&(ptMeasure->tMonBackRactEn), sizeof(OOP_ENERGY_T));
        
    lctrl_pulse_write_OAD(0x24010f00+i*0x10000, (uint8 *)&(ptMeasure->tShowForwActEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24011000+i*0x10000, (uint8 *)&(ptMeasure->tShowForwRactEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24011100+i*0x10000, (uint8 *)&(ptMeasure->tShowBackActEn), sizeof(OOP_ENERGY_T));
    lctrl_pulse_write_OAD(0x24011200+i*0x10000, (uint8 *)&(ptMeasure->tShowBackRactEn), sizeof(OOP_ENERGY_T));

end:
    return;
}

#endif


#if DESC("总加组计算", 1)



#endif


/**********************************************************************
* @name      : loadctrl_stat_init
* @brief     ：统计初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2024-6-27
* @Update    :
**********************************************************************/
int loadctrl_stat_init(void *pPublic, void **ppPrivate)
{
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;

    g_dbStat = db_client_create(DB_LOADCTRLLER_AGENT, DB_LOADCTRLLER_STAT);
    if(g_dbStat <= 0)
    {
        printf("db_client_create failed!\n");
        return -1;
    }

    memset(&g_StatData, 0, sizeof(LCTRL_SATA_T));

    /* 等待数据中心 */
    while(FALSE == ptLCData->hasDB)
    {
        sleep(1);
    }

    lctrl_stat_time_init(&g_StatData.lastTime);

    /* 脉冲初始化 */
    lctrl_pulse_oop_init(g_StatData.tPulseOOP, &g_StatData.tDayPeriod);
    lctrl_pulse_file_init(&g_StatData.tPulseData);

    /* 总加组初始化 */

    *ppPrivate = &g_StatData;
    return 0;
}

/**********************************************************************
* @name      : la_mqttmsg_proc
* @brief     ：mqtt消息处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2024-6-6
* @Update    :
**********************************************************************/
int loadctrl_stat_proc(void *pArg, void *pPublic, void *pPrivate)
{
    time_t           now;
    OOP_DATETIME_S_T curtime;
    LCTRL_DATA_T    *ptLCData = (LCTRL_DATA_T*)pPublic;
    LCTRL_SATA_T    *ptHandle = (LCTRL_SATA_T*)pPrivate;
    uint8            bDay = FALSE;
    uint8            bMonth = FALSE;
    uint32           oad = 0;
    uint16           info = 0;

    time(&now);
    DT_Time2DateTimeShort(now, &curtime);

    /* 初始化 */
    if(TRUE == lctrl_get_dataflag(&(ptLCData->statDbChg)))
    {
        lctrl_pulse_arg_clear(ptHandle->tPulseOOP, &ptHandle->tPulseData, &ptHandle->tDayPeriod);
    }
    if(TRUE == lctrl_get_argflag(&(ptLCData->statDbChg)))
    {
        lctrl_pulse_data_clear(ptHandle->tPulseOOP, &ptHandle->tPulseData);
    }

    /* OAD变化读取 */
    oad = lctrl_read_oad_notify(&(ptLCData->statDbChg), &info);
    while(0 != oad)
    {
        /* 脉冲更新 */
        if(0x24000400 == (oad & 0xfff0ff00) || 0x24001C00 == (oad & 0xfff0ff00))
        {
            lctrl_pulse_change_K(oad, ptHandle->tPulseOOP, &ptHandle->tPulseData, &curtime);
        }
        lctrl_pulse_update(oad, info, ptHandle->tPulseOOP, &ptHandle->tDayPeriod);

        /* 总加组更新 */

        
        oad = lctrl_read_oad_notify(&(ptLCData->statDbChg), &info);
    }

    /* 判断是否过日 过月 */
    if(curtime.year != ptHandle->lastTime.year || curtime.month != ptHandle->lastTime.month || 
       curtime.day != ptHandle->lastTime.day)
    {
        bDay = TRUE;
    }
    if(curtime.year != ptHandle->lastTime.year || curtime.month != ptHandle->lastTime.month)
    {
        bMonth = TRUE;
        bDay = TRUE;
    }

    /* 过分执行 */
    if(curtime.year != ptHandle->lastTime.year || curtime.month != ptHandle->lastTime.month || 
       curtime.day != ptHandle->lastTime.day || curtime.hour != ptHandle->lastTime.hour ||
       curtime.minute != ptHandle->lastTime.minute)
    {
        usleep(200000);  //过分延时200ms，等待rspSample完成统计

        /* 脉冲采集计算 */
        lctrl_get_pulse(&ptHandle->tPulseData);
        lctrl_pulse_calculate(ptHandle->tPulseOOP, &ptHandle->tPulseData, &curtime, bDay, bMonth);
        lctrl_write_ctrl(CTRL_PULSE_TYPE, (uint8*)&ptHandle->tPulseData, sizeof(LCTRL_PULSE_T));

        /* 总加组过分统计 */


        /* 存储统计时间 */
        lctrl_write_ctrl(CTRL_TIME_TYPE, (uint8*)&curtime, sizeof(OOP_DATETIME_S_T));

        ptHandle->lastTime = curtime;
    }

    
    
    return 100000;
}

