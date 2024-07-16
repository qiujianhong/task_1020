/*
*********************************************************************
* @file    monit_stat.c
* @brief： 监测统计实现文件
* @author：贺宁
* @date：   2020.11.26
* @copyright 2020 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#include "pw_redef.h"
#include "pw_lib.h"
#include "pw_common.h"
#ifdef AREA_ZHEJIANG
#include "pw_protocol_zhj.h"
#endif
#include "framecomm.h"
#include "oopType.h"
#include "monit_stat.h"
#include "hal.h"
#include "power.h"

//极值统计：按照面向对象极值统计思路定义统计参数
MT_EXTREME_OBJ_T g_extreme_objs[] = 
{
    /*mainOAD     subOAD         periCnt  freqCnt  period   freq   mtAdapt  srcData */ 
    {{0x21220200},  {0x20000200},    0,   0,    1,     {TI_MIN,1}, mt_volt_adapt,          NULL,    0},  //电压
    {{0x21220200},  {0x20010200},    0,   0,    1,     {TI_MIN,1}, mt_curr_adapt,          NULL,    0},  //电流
    {{0x21220200},  {0x20010600},    0,   0,    1,     {TI_MIN,1}, mt_zerocurr_adapt,      NULL,    0},  //零序电流
    {{0x21220200},  {0x200F0200},    0,   0,    1,     {TI_MIN,1}, mt_pwrfreq_adapt,       NULL,    0},  //电网频率
    {{0x21220200},  {0x20260200},    0,   0,    1,     {TI_MIN,1}, mt_imbalance_adapt,     NULL,    0},  //电压不平衡率
    {{0x21220200},  {0x20270200},    0,   0,    1,     {TI_MIN,1}, mt_imbalance_adapt,     NULL,    0},  //电流不平衡率
    {{0x21220200},  {0x200D0200},    0,   0,    1,     {TI_MIN,1}, mt_distortion_adapt,    NULL,    0},  //A相电压总畸变率 
    {{0x21220200},  {0x200D0300},    0,   0,    1,     {TI_MIN,1}, mt_distortion_adapt,    NULL,    0},  //B相电压总畸变率 
    {{0x21220200},  {0x200D0400},    0,   0,    1,     {TI_MIN,1}, mt_distortion_adapt,    NULL,    0},  //C相电压总畸变率 
#if 0
    {{0x21230200},  {0x20000200},    0,   0,    1,     {TI_MIN,1}, mt_volt_adapt,          NULL,    0},  //电压
    {{0x21230200},  {0x20010200},    0,   0,    1,     {TI_MIN,1}, mt_curr_adapt,          NULL,    0},  //电流
    {{0x21230200},  {0x20010600},    0,   0,    1,     {TI_MIN,1}, mt_zerocurr_adapt,      NULL,    0},  //零序电流
    {{0x21230200},  {0x200F0200},    0,   0,    1,     {TI_MIN,1}, mt_pwrfreq_adapt,       NULL,    0},  //电网频率
    {{0x21230200},  {0x20260200},    0,   0,    1,     {TI_MIN,1}, mt_imbalance_adapt,     NULL,    0},  //电压不平衡率
    {{0x21230200},  {0x20270200},    0,   0,    1,     {TI_MIN,1}, mt_imbalance_adapt,     NULL,    0},  //电流不平衡率
    {{0x21230200},  {0x200D0200},    0,   0,    1,     {TI_MIN,1}, mt_distortion_adapt,    NULL,    0},  //A相电压总畸变率    
    {{0x21230200},  {0x200D0300},    0,   0,    1,     {TI_MIN,1}, mt_distortion_adapt,    NULL,    0},  //B相电压总畸变率    
    {{0x21230200},  {0x200D0400},    0,   0,    1,     {TI_MIN,1}, mt_distortion_adapt,    NULL,    0},  //C相电压总畸变率
#endif
};

const uint32 g_extreme_objs_max = sizeof(g_extreme_objs)/sizeof(g_extreme_objs[0]);

//合格率统计：按照面向对象合格率统计思路定义统计参数,定义2192,2193为日月合格率统计
MT_QR_OBJ_T g_qr_objs[] = 
{
    /*mainOAD     subOAD         periCnt  freqCnt  period   freq     upLimit         upRecov  dwLimit   dwRecov  mtAdapt  srcData  nIndex*/ 
    {{0x21920200},  {0x20000200},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_UP_LIMIT,   MT_VOLT_UP_RECOV,   MT_VOLT_DW_LIMIT, MT_VOLT_DW_RECOV, mt_volt_adapt,         NULL,    0},  //电压
    {{0x21920200},  {0x200F0200},    0,   0,    1,     {TI_MIN,1}, MT_PWRF_UP_LIMIT,   MT_PWRF_UP_RECOV,   MT_PWRF_DW_LIMIT, MT_PWRF_DW_RECOV, mt_pwrfreq_adapt,      NULL,    0},  //电网频率
    {{0x21920200},  {0x20260200},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_UNB_LIMIT,  MT_VOLT_UNB_RECOV,  MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_imbalance_adapt,    NULL,    0},  //电压不平衡率
    {{0x21920200},  {0x20270200},    0,   0,    1,     {TI_MIN,1}, MT_CURR_UNB_LIMIT,  MT_CURR_UNB_RECOV,  MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_imbalance_adapt,    NULL,    0},  //电流不平衡率
    {{0x21920200},  {0x200D0200},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_DIST_LIMIT, MT_VOLT_DIST_RECOV, MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_distortion_adapt,   NULL,    0},  //A相电压总畸变率 
    {{0x21920200},  {0x200D0300},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_DIST_LIMIT, MT_VOLT_DIST_RECOV, MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_distortion_adapt,   NULL,    0},  //B相电压总畸变率 
    {{0x21920200},  {0x200D0400},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_DIST_LIMIT, MT_VOLT_DIST_RECOV, MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_distortion_adapt,   NULL,    0},  //C相电压总畸变率 
#if 0
    {{0x21930200},  {0x20000200},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_UP_LIMIT,   MT_VOLT_UP_RECOV,   MT_VOLT_DW_LIMIT, MT_VOLT_DW_RECOV, mt_volt_adapt,         NULL,    0},  //电压
    {{0x21930200},  {0x200F0200},    0,   0,    1,     {TI_MIN,1}, MT_PWRF_UP_LIMIT,   MT_PWRF_UP_RECOV,   MT_PWRF_DW_LIMIT, MT_PWRF_DW_RECOV, mt_pwrfreq_adapt,      NULL,    0},  //电网频率
    {{0x21930200},  {0x20260200},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_UNB_LIMIT,  MT_VOLT_UNB_RECOV,  MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_imbalance_adapt,    NULL,    0},  //电压不平衡率
    {{0x21930200},  {0x20270200},    0,   0,    1,     {TI_MIN,1}, MT_CURR_UNB_LIMIT,  MT_CURR_UNB_RECOV,  MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_imbalance_adapt,    NULL,    0},  //电流不平衡率
    {{0x21930200},  {0x200D0200},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_DIST_LIMIT, MT_VOLT_DIST_RECOV, MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_distortion_adapt,   NULL,    0},  //A相电压总畸变率    
    {{0x21930200},  {0x200D0300},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_DIST_LIMIT, MT_VOLT_DIST_RECOV, MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_distortion_adapt,   NULL,    0},  //B相电压总畸变率    
    {{0x21930200},  {0x200D0400},    0,   0,    1,     {TI_MIN,1}, MT_VOLT_DIST_LIMIT, MT_VOLT_DIST_RECOV, MT_INVALID_LIMIT, MT_INVALID_LIMIT, mt_distortion_adapt,   NULL,    0},  //C相电压总畸变率
#endif
};

const uint32 g_qr_objs_max = sizeof(g_qr_objs)/sizeof(g_qr_objs[0]);

//区间统计：按照面向对象区间统计思路定义统计参数
MT_REGION_OBJ_T g_region_objs[] = 
{
    /*mainOAD     subOAD         periCnt  freqCnt  period   freq    nNum           parm             mtAdapt  srcData  nIndex*/ 
    {{0x21020200},  {0x200A0200},    0,   0,    1,     {TI_MIN,1},  2,   {{16,{.lVal = MT_FWR_FAC1_LIMIT}},{16,{.lVal = MT_FWR_FAC2_LIMIT}},{0}},  mt_pwrfact_adapt,  NULL,    0},  //功率因数

    {{0x21030200},  {0x200A0200},    0,   0,    1,     {TI_MIN,1},  2,   {{16,{.lVal = MT_FWR_FAC1_LIMIT}},{16,{.lVal = MT_FWR_FAC2_LIMIT}},{0}},  mt_pwrfact_adapt,  NULL,    0},  //功率因数
};

const uint32 g_region_objs_max = sizeof(g_region_objs)/sizeof(g_region_objs[0]);

//数据冻结：按照面向对象冻结思路定义冻结参数
MT_FROZEN_OBJ_T g_frozen_objs[] = 
{
    /*mainOAD     subOAD         periCnt   period  */
    {{0x50060200},  {0x10100200},    0,     1},  //正向有功最大需量
    {{0x50060200},  {0x10200200},    0,     1},  //反向有功最大需量
};

const uint32 g_frozen_objs_max = sizeof(g_frozen_objs)/sizeof(g_frozen_objs[0]);

//停电统计:按照面向对象冻结思路定义停电时间
MT_POWERCUT_OBJ_T g_powercut_objs[]=
{
    {{0x21940200},  0,   0,    1,     {TI_MIN,1}},  //日停电时间
    {{0x21950200},  0,   0,    1,     {TI_MIN,1}},  //月停电时间
};
const uint32 g_powercut_objs_max = sizeof(g_powercut_objs)/sizeof(g_powercut_objs[0]);

#if DESC("公共函数",1)

/**********************************************************************
* @name      : get_power_state
* @brief     ：获取电源状态
* @param[in] ：  
* @param[out]：state: 电源状态，1有电，0 掉电
* @return    ：0 成功， <0 失败
* @Create    : 贺宁
* @Date      ：2020-07-27
* @Update    :
**********************************************************************/
int get_power_state(int *state)
{
    int ret = ERR_OK;
    
    POWER_DEVICE_T* dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
    if(NULL == dev)
    {
        STAT_FMT_DEBUG("dev is null!\n");
        return ERR_PNULL;
    }

    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    ret |= dev->power_power_state_get(dev,POWER_MAIN,state);
    if(ret < 0)
    {
        STAT_FMT_DEBUG("get power state failed, ret(%d).\n", ret);
        return ret;
    }
    
    hal_device_release((HW_DEVICE *)dev);
    
    return ret;
}

/**
*********************************************************************
* @brief：      统计周期处理函数提供统一的输入
* @param[in]： oad       待分析的OAD
* @param[out]：interval 分析出的间隔
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void mt_period_analyse(OOP_OAD_U oad, OOP_TI_SPAN_E *interval)
{

    if(oad.nObjID == MT_FROZEN_SEC)
    {
        *interval = TI_SEC; 
    }

    if((oad.nObjID == MT_FROZEN_MIN)||(oad.nObjID == MT_REGION_MIN)
        ||(oad.nObjID == MT_TOTALAVG_MIN)||(oad.nObjID == MT_EXTREME_MIN))
    {
        *interval = TI_MIN;    
    }

    if((oad.nObjID == MT_FROZEN_HOUR)||(oad.nObjID == MT_REGION_HOUR)
        ||(oad.nObjID == MT_TOTALAVG_HOUR)||(oad.nObjID == MT_EXTREME_HOUR))
    {
        *interval = TI_HOUR;    
    } 

    if((oad.nObjID == MT_FROZEN_DAY)||(oad.nObjID == MT_REGION_DAY)
        ||(oad.nObjID == MT_TOTALAVG_DAY)||(oad.nObjID == MT_EXTREME_DAY)
        ||(oad.nObjID == MT_FROZEN_SETTLING_DAY) || (oad.nObjID == MT_QR_DAY)
        ||(oad.nObjID == MT_POWERCUT_DAY))
    {
        *interval =TI_DAY; 
    }

    if((oad.nObjID == MT_FROZEN_MON)||(oad.nObjID == MT_REGION_MON)
        ||(oad.nObjID == MT_TOTALAVG_MON)||(oad.nObjID == MT_EXTREME_MON)
        ||(oad.nObjID == MT_QR_MON)||(oad.nObjID == MT_POWERCUT_MON))
    {
        *interval = TI_MON;    
    }

    if((oad.nObjID == MT_FROZEN_YEAR)||(oad.nObjID == MT_REGION_YEAR)
        ||(oad.nObjID == MT_TOTALAVG_YEAR)||(oad.nObjID == MT_EXTREME_YEAR))
    {
        *interval = TI_YEAR;    
    }      
}

/**
*********************************************************************
* @brief：      周期计数更新
* @param[in]： interval 周期单位
* @param[in]： period   周期
* @param[out]：count     周期计数数
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     1-新周期 0-非新周期
*********************************************************************
*/
BOOLEAN mt_pericnt_update(OOP_TI_SPAN_E span, uint32 period, uint32 *periCnt, uint32 *freqCnt)
{
    BOOLEAN isUpdate = FALSE;

    switch(span)
    {
        case TI_SEC: break;
        case TI_MIN: if(g_eMtPastTimeFlag.BitSect.Minute)  {(*periCnt)++;}break;
        case TI_HOUR:if(g_eMtPastTimeFlag.BitSect.Hour) {(*periCnt)++;}break;
        case TI_DAY: if(g_eMtPastTimeFlag.BitSect.Day)  {(*periCnt)++;}break;
        case TI_MON: if(g_eMtPastTimeFlag.BitSect.Month)  {(*periCnt)++;}break;
        default:break;
    }
    
    //达到周期则清零计数
    if(*periCnt == period)
    {
        STAT_FMT_DEBUG("span(%d), periCnt(%d) == period(%d)\n", span, *periCnt, period);
        
        *periCnt = 0;
        
        if(freqCnt != NULL) 
        {
            *freqCnt = 0;
        }

        isUpdate = TRUE;
    }

    return isUpdate;
}

/**
*********************************************************************
* @brief      ：读记录型数据
* @param[in]  ：clientid   数据中心句柄
* @param[in] ：recordOAD 记录型OAD信息
* @param[out] ：storeTm    存储时间
* @param[out] ：outData    输出的数据
* @param[out] ：outLen     输出缓冲区长度

* @Create     : 贺宁
* @Date       ：2020-11-26
* @return     ：void
*********************************************************************
*/
int mt_record_read(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 *storeTm, void *outData, uint32 outLen)
{
    int ret = ERR_OK;
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};

    memcpy(&readRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    readRecord.cType = COL_TM_STORE;
    readRecord.sortType = DOWN_SORT;
    readRecord.cStart = 0;
    readRecord.cEnd = time(NULL) + COMPENSATION_TIME + 1; // 注：+8个小时(28800)转成格林时间
    
    ret = db_read_record(clientid, &readRecord, outLen, &recordBuf);
    if(ret == ERR_OK)
    {
        if(outLen != recordBuf.record[0].outDatalen)
        {
            STAT_FMT_DEBUG("len(%d) != outDatelen(%d), oadMain(0x%08x), oadCols(0x%08x)\n", 
                outLen, recordBuf.record[0].outDatalen, readRecord.recordOAD.road.oadMain.value, readRecord.recordOAD.road.oadCols.oad[0].value);
        }

        *storeTm = recordBuf.colStoreTm;
            
        memcpy(outData, recordBuf.record[0].outDataBuf, outLen);
    }

    return ret;
}

/**
*********************************************************************
* @brief：      写记录型数据
* @param[in] ：clientid  数据中心句柄
* @param[in] ：recordOAD 记录型OAD信息
* @param[in] ：storeTm   存储时间
* @param[in] ：inData     写入的数据
* @param[in] ：inLen      数据长度

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
int mt_record_write(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 storeTm, void *inData, uint32 inLen)
{
    WRITE_RECORD_T writeRecord;
    uint32 storeNo;

    MEMZERO(&writeRecord, sizeof(writeRecord));
    memcpy(&writeRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));
    
    writeRecord.inDatalen = inLen;
    writeRecord.colStoreTm = storeTm + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
    
    //传出时，按结构体强转
    memcpy(writeRecord.inDataBuf, (uint8*)inData, inLen);
    
    return db_write_record(clientid, &writeRecord, &storeNo);   
}

/**
*********************************************************************
* @brief：      更新记录型数据
* @param[in] ：clientid  数据中心句柄
* @param[in] ：recordOAD 记录型OAD信息
* @param[in] ：storeTm   存储时间
* @param[in] ：inData     写入的数据
* @param[in] ：inLen      数据长度

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
int mt_record_update(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 storeTm, void *inData, uint32 inLen)
{
    UPDATE_RECORD_T updateRecord;

    MEMZERO(&updateRecord, sizeof(updateRecord));
    memcpy(&updateRecord.recordOAD, recordOAD, sizeof(RECORD_OAD_T));

    updateRecord.filter.choice = 1;
    updateRecord.inDatalen = inLen;
    updateRecord.filter.storeTm = storeTm;
    
    //传出时，按结构体强转
    memcpy(updateRecord.inDataBuf, (uint8*)inData, inLen);
    
    return db_update_record(clientid, &updateRecord);   
}

#endif

#if DESC("数据适配函数",1)
/**
*********************************************************************
* @brief：      电压源数据适配
* @param[in]： clientid  数据中心句柄
* @param[in]： oad        数据ID
* @param[out]：units      子项数
* @param[out]：digit      适配后的数据

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int mt_volt_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_WORD3V_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = oad.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(OOP_WORD3V_T), (uint8*)&rtData, &len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x),.\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value);
    
        return ret;
    }

    //3个分项
    *units = rtData.nNum;

    for(i = 0; i < *units; i++)
    {
        digit[i].choice = DT_LONG_UNSIGNED;
        digit[i].wVal = rtData.nValue[i];
    }

    return ret;
}

/**
*********************************************************************
* @brief：      电流源数据适配
* @param[in]： clientid  数据中心句柄
* @param[in]： oad        数据ID
* @param[out]：units      子项数
* @param[out]：digit      适配后的数据

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int mt_curr_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_INT3V_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = oad.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(OOP_INT3V_T), (uint8*)&rtData, &len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x),.\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value);
    
        return ret;
    }

    //3个分项
    *units = rtData.nNum;

    for(i = 0; i < *units; i++)
    {
        digit[i].choice = DT_DOUBLE_LONG;
        digit[i].nVal = rtData.nValue[i];
    }

    return ret;
}

/**
*********************************************************************
* @brief：      零序电流源数据适配
* @param[in]： clientid  数据中心句柄
* @param[in]： oad        数据ID
* @param[out]：units      子项数
* @param[out]：digit      适配后的数据

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int mt_zerocurr_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    uint32 rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = oad.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(uint32), (uint8*)&rtData, &len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x),.\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value);
    
        return ret;
    }

    //没有分项
    *units = 0;

    digit[i].choice = DT_DOUBLE_LONG;
    digit[i].nVal = rtData;
    
    return ret;
}

/**
*********************************************************************
* @brief：      电网频率源数据适配
* @param[in]： clientid  数据中心句柄
* @param[in]： oad        数据ID
* @param[out]：units      子项数
* @param[out]：digit      适配后的数据

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int mt_pwrfreq_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    uint16 rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = oad.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(uint16), (uint8*)&rtData, &len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x),.\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value);
    
        return ret;
    }

    //没有分项
    *units = 0;

    digit[i].choice = DT_LONG_UNSIGNED;
    digit[i].wVal = rtData;

    return ret;
}

/**
*********************************************************************
* @brief：      电压/电流不平衡率源数据适配
* @param[in]： clientid  数据中心句柄
* @param[in]： oad        数据ID
* @param[out]：units      子项数
* @param[out]：digit      适配后的数据

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int mt_imbalance_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
#ifndef AREA_ZHEJIANG
    NOMAL_OAD_T nomalOAD = {0};
	uint32 len = 0;
#endif
    uint16 rtData;     //当前实时数据
    int i = 0;

#ifndef AREA_ZHEJIANG
    nomalOAD.logicId =1;
    nomalOAD.oad.value = oad.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(uint16), (uint8*)&rtData, &len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x),.\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value);
    
        return ret;
    }
#else
    /* 电压电流不平衡率使用估算的方法 */
    uint32 ArrIdx  = 0;//一维数组索引(起始)
    ArrIdx = g_aYcDataIndexZhj[1][0][0];

    if (oad.value == 0x20260200)
        rtData = (uint16)g_aYcDataSendZhj[ArrIdx + g_nYcVoltUnbalance].fNewValue;   /* 电压不平衡度 */
    else
        rtData = (uint16)g_aYcDataSendZhj[ArrIdx + g_nYcVoltUnbalance + 1].fNewValue;   /* 电流不平衡度 */
#endif
    
    //没有分项
    *units = 0;

    digit[i].choice = DT_LONG_UNSIGNED;
    digit[i].wVal = rtData;

    return ret;
}

/**
*********************************************************************
* @brief：      电压总畸变率数据适配
* @param[in]： clientid  数据中心句柄
* @param[in]： oad        数据ID
* @param[out]：units      子项数
* @param[out]：digit      适配后的数据

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int mt_distortion_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_HARMONIC_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = oad.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(OOP_HARMONIC_T), (uint8*)&rtData, &len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x),.\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value);
    
        return ret;
    }

    //只取1个分项
    *units = 1;

    digit[i].choice = DT_LONG;
    digit[i].lVal = rtData.nValue[0];

    return ret;
}

/**
*********************************************************************
* @brief：      功率因数数据适配
* @param[in]： clientid  数据中心句柄
* @param[in]： oad        数据ID
* @param[out]：units      子项数
* @param[out]：digit      适配后的数据

* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
int mt_pwrfact_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};
    OOP_LONG4V_T rtData;     //当前实时数据
    uint32 len = 0;
    int i = 0;

    nomalOAD.logicId = 1;
    nomalOAD.oad.value = oad.value;

    /* 获取统计数据源 */
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(OOP_LONG4V_T), (uint8*)&rtData, &len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("db_read_nomal failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x),.\n",
            ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value);
    
        return ret;
    }

    //只取1个分项
    *units = 1;

    digit[i].choice = DT_LONG;
    digit[i].lVal = rtData.nValue[0];

    return ret;
}

#endif

#if DESC("合格率统计函数",1)
#ifdef AREA_ZHEJIANG
/* 填充电压统计遥测点 */
static void mt_qr_fill_yc_volt(uint32 ArrIdx, uint32 start, uint8 offset, MT_QR_RES_T *result)
{
    g_aYcDataSendZhj[ArrIdx + start + offset + 0].fNewValue = result->mtTime;     /* 监测时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 0].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 1].fNewValue = result->upTime;     /* 超上限时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 1].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 2].fNewValue = result->dwTime;     /* 超下限时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 2].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 3].fNewValue = result->qrTime;     /* 合格时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 3].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 4].fNewValue = result->qrRate * 0.01;     /* 合格率 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 4].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 5].fNewValue = result->upRate * 0.01;     /* 超上限率 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 5].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 6].fNewValue = result->dwRate * 0.01;     /* 超下限率 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 6].bNewValidFlag = TRUE;
}

/* 填充谐波、不平衡度统计遥测点 */
static void mt_qr_fill_yc_harmony(uint32 ArrIdx, uint32 start, uint8 offset, MT_QR_RES_T *result)
{
    g_aYcDataSendZhj[ArrIdx + start + offset + 0].fNewValue = result->mtTime;     /* 监测时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 0].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 1].fNewValue = result->upTime;     /* 超上限时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 1].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 2].fNewValue = result->qrTime;     /* 合格时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 2].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 3].fNewValue = result->qrRate * 0.01;     /* 合格率 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 3].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 4].fNewValue = result->upRate * 0.01;     /* 超上限率 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 4].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 5].fNewValue = result->mtAvg;     /* 平均值 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 5].bNewValidFlag = TRUE;
}

/* 填充频率统计遥测点 */
static void mt_qr_fill_yc_frequency(uint32 ArrIdx, uint32 start, uint8 offset, MT_QR_RES_T *result)
{
    g_aYcDataSendZhj[ArrIdx + start + offset + 0].fNewValue = result->mtTime;     /* 监测时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 0].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 1].fNewValue = result->upTime + result->dwTime;     /* 超限时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 1].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 2].fNewValue = result->qrTime;     /* 合格时间 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 2].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 3].fNewValue = result->qrRate * 0.01;     /* 合格率 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 3].bNewValidFlag = TRUE;
    g_aYcDataSendZhj[ArrIdx + start + offset + 4].fNewValue = result->upRate * 0.01 + result->dwRate * 0.01;  /* 超限率 */
    g_aYcDataSendZhj[ArrIdx + start + offset + 4].bNewValidFlag = TRUE;
}


/* 把合格率数据直接赋值给遥测点 */
static void mt_qt_yc(MT_QR_OBJ_T *object, MT_QR_RES_T *result)
{
    uint32 subOADValue = object->subOAD.value + object->nIndex;
    uint32 ArrIdx  = 0;//一维数组索引(起始)

    ArrIdx = g_aYcDataIndexZhj[1][0][0];

    if  (0x21920200 == object->mainOAD.value)
    {
        switch (subOADValue)
        {
            case 0x20000201:  /* 当日A相电压统计 */
                mt_qr_fill_yc_volt(ArrIdx, g_nYcStatVoltDay, 0, result);                
                break;
            case 0x20000202:  /* 当日B相电压统计 */           
                mt_qr_fill_yc_volt(ArrIdx, g_nYcStatVoltDay, 7, result);  
                break;
            case 0x20000203:  /* 当日C相电压统计 */            
                mt_qr_fill_yc_volt(ArrIdx, g_nYcStatVoltDay, 14, result);  
                break;

            case 0x200D0201:  /* 当日A相电压谐波总畸变率 */
                mt_qr_fill_yc_harmony(ArrIdx, g_nYcStatHarmonyDay, 0, result);
                break;
            case 0x200D0301:  /* 当日B相电压谐波总畸变率 */
                mt_qr_fill_yc_harmony(ArrIdx, g_nYcStatHarmonyDay, 6, result);
                break;
            case 0x200D0401:  /* 当日C相电压谐波总畸变率 */
                mt_qr_fill_yc_harmony(ArrIdx, g_nYcStatHarmonyDay, 12, result);
                break;

            case 0x20260200:  /* 当日电压不平衡度统计 */
                mt_qr_fill_yc_harmony(ArrIdx, g_nYcStatVoltUnbalanceDay, 0, result);
                break;

            case 0x20270200:  /* 当日电流不平衡度统计 */
                mt_qr_fill_yc_harmony(ArrIdx, g_nYcStatCurrUnbalanceDay, 0, result);
                break;

            case 0x200F0200: /* 当日频率统计 */
                mt_qr_fill_yc_frequency(ArrIdx, g_nYcStatFrequencyDay, 0, result);
                break;
            default:
                break;
        }


    }
}
#endif
/**
*********************************************************************
* @brief：      合格率计算
* @param[in]： rtData   数据
* @param[in]： object   统计对象
* @param[out]: result   计算结果
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void mt_qr_calc(OOP_DIGIT_T rtData, MT_QR_OBJ_T *object, MT_QR_RES_T *result)
{
    STAT_FMT_TRACE("qrstat: rtData(%llu), upLimit(%d), upRecov(%d), dwLimit(%d), dwRecov(%d)\n", 
        rtData.ullVal, object->upLimit, object->upRecov, object->dwLimit, object->dwRecov);

    //时间单位都是分钟
    result->mtTime += object->freq.value;

#ifdef AREA_ZHEJIANG
    if (object->subOAD.value == 0x20260200) /* 电压不平衡度 */
    {
        /* 电压电流不平衡率使用估算的方法 */
        uint32 ArrIdx  = 0;//一维数组索引(起始)
        ArrIdx = g_aYcDataIndexZhj[1][0][0];
        float32 value;

        value = g_aYcDataSendZhj[ArrIdx + g_nYcVoltUnbalance].fNewValue;
        result->mtTotal += value;

        if((value >= 4.0)
            || ((object->upRecov != MT_INVALID_LIMIT)&&(result->mtState == MT_QSTATE_U)&&(value >= 4.0)))
        {
            result->upTime += object->freq.value;
            result->mtState = MT_QSTATE_U;

            STAT_FMT_TRACE("qrstat: over upper limit.\n");
        }
        else if((object->dwLimit != MT_INVALID_LIMIT)&&((value <= 3.8)
            ||((object->dwRecov != MT_INVALID_LIMIT)&&(result->mtState == MT_QSTATE_D)&&(value <= 3.8))))
        {
            result->dwTime += object->freq.value;
            result->mtState = MT_QSTATE_D;

            STAT_FMT_TRACE("qrstat: over lower limit.\n");
        }
        else
        {
            result->qrTime += object->freq.value;
            result->mtState = MT_QSTATE_Q;
            
            STAT_FMT_TRACE("qrstat: qualified.\n");
        }
    }
    else
    {
#endif
        //数据全部取最大类型
        result->mtTotal += rtData.ullVal;

        //合格率和越限率四舍五入，精确到小数点后2位，首次读到的默认状态为合格
        if((rtData.ullVal >= object->upLimit)
            || ((object->upRecov != MT_INVALID_LIMIT)&&(result->mtState == MT_QSTATE_U)&&(rtData.ullVal >= object->upRecov)))
        {
            result->upTime += object->freq.value;
            result->mtState = MT_QSTATE_U;

            STAT_FMT_TRACE("qrstat: over upper limit.\n");
        }
        else if((object->dwLimit != MT_INVALID_LIMIT)&&((rtData.ullVal <= object->dwLimit)
            ||((object->dwRecov != MT_INVALID_LIMIT)&&(result->mtState == MT_QSTATE_D)&&(rtData.ullVal <= object->dwRecov))))
        {
            result->dwTime += object->freq.value;
            result->mtState = MT_QSTATE_D;

            STAT_FMT_TRACE("qrstat: over lower limit.\n");
        }
        else
        {
            result->qrTime += object->freq.value;
            result->mtState = MT_QSTATE_Q;
            
            STAT_FMT_TRACE("qrstat: qualified.\n");
        }
#ifdef AREA_ZHEJIANG
   }
#endif

    result->upRate = (uint16)((float32)result->upTime*10000/result->mtTime + 0.5);   
    result->dwRate = (uint16)((float32)result->dwTime*10000/result->mtTime + 0.5);  
    result->qrRate = (uint16)((float32)result->qrTime*10000/result->mtTime + 0.5);    
    result->mtAvg = result->mtTotal/(object->freqCnt+1);
    
    return;
}

/**
*********************************************************************
* @brief：      合格率统计记录
* @param[in]： clientid   数据中心句柄
* @param[in]： object      统计对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void mt_qr_record(DB_CLIENT clientid, MT_QR_OBJ_T *object)
{
    int ret = ERR_OK;
    MT_QR_RES_T result = {0};
    uint32 len = sizeof(MT_QR_RES_T);
    bool isUpdate = FALSE;
    OOP_DIGIT_T rtData = object->srcData[MT_ATTR_INDEX(object->nIndex)];
    uint32 storeTm = 0;
    time_t curTime = hex_time_to_sec(&g_stMtCurBinTime);
    RECORD_OAD_T recordOAD;

    MEMZERO(&recordOAD, sizeof(recordOAD));

    recordOAD.logicId = 7;
    recordOAD.road.oadMain.value = object->mainOAD.value;
    recordOAD.road.oadCols.nNum = 1;
    recordOAD.road.oadCols.oad[0].value = object->subOAD.value + object->nIndex;

    /* 首次达到统计周期，先清历史数据 */    
    if(object->freqCnt == 0)
    {
        ret = mt_record_write(clientid, &recordOAD, curTime, &result, len);

        STAT_FMT_DEBUG("qrstat: new cycle begin. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
           ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
    }
    
    //读取历史数据
    ret = mt_record_read(clientid, &recordOAD, &storeTm, &result, len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("qrstat: history read failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
           ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(result));

        object->freqCnt = 0; //失败计数重新开始
    }
    else
    {
        isUpdate = TRUE;
    }

    //计算合格率结果
    mt_qr_calc(rtData, object, &result);

    STAT_FMT_TRACE("\r\n freqCnt(%d): oadMain(0x%08x), oadCols(0x%08x)\
                    \r\n mtTime(%d), qrTime(%d) upTime(%d), dwTime(%d)\
                    \r\n qrRate(%d), upRate(%d) dwRate(%d),\
                    \r\n mtTotal(%llf), mtAvg(%llf), mtState(%d)\n", 
                    object->freqCnt, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, 
                    result.mtTime,result.qrTime,result.upTime,result.dwTime,
                    result.qrRate,result.upRate,result.dwRate,
                    result.mtTotal,result.mtAvg, result.mtState);

    //累积完成后回写
    if(isUpdate)
    {
        ret = mt_record_update(clientid, &recordOAD, storeTm, &result, len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("qrstat: update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
               ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
        }
    }
    else
    {
        ret = mt_record_write(clientid, &recordOAD, curTime, &result, len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("qrstat: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
               ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
        }
    }

#ifdef AREA_ZHEJIANG
    mt_qt_yc(object, &result);
#endif
    return;     
}

/**
*********************************************************************
* @name      : mt_qr_dispatch
* @brief     ：分派合格率统计任务
* @param[in] ：clientid   数据中心句柄
* @param[in] ：object      统计对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_qr_dispatch(DB_CLIENT clientid, MT_QR_OBJ_T *object)
{
    int ret = ERR_OK;
    uint32 units = 0;
    uint32 i = 0;
    OOP_DIGIT_T srcData[128];

    object->srcData = srcData;

    //数据格式归一化接口
    if(object->mtAdapt != NULL)
    {
        ret = object->mtAdapt(clientid, object->subOAD, &units, srcData);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("mtAdapt failed. ret(%d), oad(0x%08x)\n", ret, object->subOAD.value);
            return;
        }
    }
    else
    {
        STAT_FMT_DEBUG("oad(0x%08x) qr calc not support.\n", object->subOAD.value);
    }
    
    //分项需要拆开计算
    if(units == 0)
    {
        mt_qr_record(clientid, object);
    }
    else
    {
        for(i = 0; i < units; i++)
        {
            object->nIndex = i+1;
            mt_qr_record(clientid, object);
        }
    }
    
    return;  
}

/**
*********************************************************************
* @name      : mt_qr_entry
* @brief     ：合格率统计入口
* @param[in] ：clientid 数据中心句柄
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_qr_entry(DB_CLIENT clientid)
{
    uint32 i = 0;
    MT_QR_OBJ_T *tmpNode = NULL;
    OOP_TI_SPAN_E time_span;

    STAT_FMT_TRACE("****** mt_qr_entry start ****** \n");
    
    for(i = 0; i < g_qr_objs_max; i++)
    {
        tmpNode = &g_qr_objs[i];
        
        mt_period_analyse(tmpNode->mainOAD, &time_span);

        mt_qr_dispatch(clientid, tmpNode);
        
        tmpNode->freqCnt++;

        //刷新周期数
        mt_pericnt_update(time_span, tmpNode->period, &tmpNode->periCnt, &tmpNode->freqCnt);           
    }

    STAT_FMT_TRACE("****** mt_qr_entry end ****** \n\n");
    
    return;
}

#endif

#if DESC("区间统计函数",1)
#ifdef AREA_ZHEJIANG
/* 把功率因数统计数据直接赋值给遥测点 */
static void mt_region_yc(MT_REGION_OBJ_T *object, OOP_REGION_T *result)
{
    uint32 subOADValue = object->subOAD.value + object->nIndex;
    uint32 ArrIdx  = 0;//一维数组索引(起始)

    ArrIdx = g_aYcDataIndexZhj[1][0][0];

    if  (0x21020200 == object->mainOAD.value)
    {
        switch (subOADValue)
        {
            case 0x200A0201:  /* 当日功率因数越限统计 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatPwrFactDay + 0].fNewValue = result->value[0].dwTimes;     /* 越下限时间 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatPwrFactDay + 0].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatPwrFactDay + 1].fNewValue = result->value[1].dwTimes;     /* 合格时间 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatPwrFactDay + 1].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatPwrFactDay + 2].fNewValue = result->value[2].dwTimes;     /* 越上限时间 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatPwrFactDay + 2].bNewValidFlag = TRUE;              
                break;
            
            default:
                break;
        }

    }
}
#endif

void mt_region_calc(OOP_DIGIT_T rtData, MT_REGION_OBJ_T *object, OOP_REGION_T *result)
{
    uint8 changedFlag = 0;
    int i = 0;

    STAT_FMT_TRACE("region: rtData(%llu), validNum(%d)~{point1(%llu), ponit2(%llu), ponit3(%llu)}\n", 
        rtData.ullVal, object->nNum, object->parm[0].ullVal, object->parm[1].ullVal, object->parm[2].ullVal);
    
    //从最大上限开始，大于则说明数据落在比它大的区间，记录后直接结束这个数据的判断
    for(i = object->nNum-1; i >= 0; i--)
    {
        switch(rtData.choice) 
        {
            case DT_DOUBLE_LONG: //double long/int32
            {
                if(rtData.nVal >= object->parm[i].nVal)
                {
                    changedFlag = 1;
                }
            }break;
            case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
            {
                if(rtData.dwVal >= object->parm[i].dwVal)
                {
                    changedFlag = 1;
                }
            }break;     
            case DT_INTEGER: //integer/char
            {
                if(rtData.cVal >= object->parm[i].cVal)
                {
                    changedFlag = 1;
                }
            }break;
            case DT_LONG: //long/int16
            {
                if(rtData.lVal >= object->parm[i].lVal)
                {
                    changedFlag = 1;
                }
            }break; 
            case DT_UNSIGNED: //unsigned/uint8
            {
                if(rtData.bVal >= object->parm[i].bVal)
                {
                    changedFlag = 1;
                }
            }break;   
            case DT_LONG_UNSIGNED: //long unsigned/uint16
            {
                if(rtData.wVal >= object->parm[i].wVal)
                {
                    changedFlag = 1;
                }
            }break;   
            case DT_LONG64: //long64/int64
            {
                if(rtData.llVal >= object->parm[i].llVal)
                {
                    changedFlag = 1;
                }                   
            }break;   
            case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
            {
                if(rtData.ullVal >= object->parm[i].ullVal)
                {
                    changedFlag = 1;
                }           
            }break;
            default:break;           
        }
        
        if(changedFlag)
        {
            result->value[i+1].dwCount++;                     //此区间内越限次数累加
            result->value[i+1].dwTimes += object->freq.value; //累积时间，分钟
            break;
        }
    }
    
    //没有任何越限,记录在最小限[0]
    if(i == -1)
    {
        result->value[i+1].dwCount++;                     //此区间内越限次数增加累加
        result->value[i+1].dwTimes += object->freq.value; //累积时间，分钟
        
        STAT_FMT_TRACE("region[%d]: parm(%llu)\n", i+1, object->parm[i].ullVal);
    }
    
    result->nNum = object->nNum+1;
}

/**
*********************************************************************
* @brief：      区间统计计算(通用计算)
* @param[in]： clientid   数据中心句柄
* @param[in]： object      统计对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void mt_region_record(DB_CLIENT clientid, MT_REGION_OBJ_T *object)
{
    int ret = ERR_OK;
    OOP_REGION_T result = {{0}};  //一个区间统计结果
    uint32 len = sizeof(OOP_REGION_T);    
    BOOLEAN isUpdate = FALSE;
    uint8 i = 0;
    OOP_DIGIT_T rtData = object->srcData[MT_ATTR_INDEX(object->nIndex)];
    uint32 storeTm = 0;
    time_t curTime = hex_time_to_sec(&g_stMtCurBinTime);
    RECORD_OAD_T recordOAD;

    MEMZERO(&recordOAD, sizeof(recordOAD));

    recordOAD.logicId = 7;
    recordOAD.road.oadMain.value = object->mainOAD.value;
    recordOAD.road.oadCols.nNum = 1;
    recordOAD.road.oadCols.oad[0].value = object->subOAD.value + object->nIndex;

    result.obj.value = recordOAD.road.oadCols.oad[0].value;

    /* 首次达到统计周期，先清历史数据 */    
    if(object->freqCnt == 0)
    {
        ret = mt_record_write(clientid, &recordOAD, curTime, &result, len);

        STAT_FMT_DEBUG("region: new cycle begin. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
           ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
    }

    //读取历史统计数据
    ret = mt_record_read(clientid, &recordOAD, &storeTm, &result, len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("region: history read failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
           ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(result));

        object->freqCnt = 0; //失败计数重新开始
    }
    else
    {
        isUpdate = TRUE;
    }

    mt_region_calc(rtData, object, &result);

    STAT_FMT_TRACE("freqCnt(%d): mainOAD(0x%08x), subOAD(0x%08x)\n", 
        object->freqCnt, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value);
    
    for(i = 0; i < result.nNum; i ++)
    {
        STAT_FMT_TRACE("region[%d]: dwTimes(%d), dwCount(%d)\n", 
            i, result.value[i].dwTimes, result.value[i].dwCount);
    }

    //累积完成后回写
    if(isUpdate)
    {
        ret = mt_record_update(clientid, &recordOAD, storeTm, &result, len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region: update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
               ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
        }
    }
    else
    {
        ret = mt_record_write(clientid, &recordOAD, curTime, &result, len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("region: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
               ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
        }
    }

#ifdef AREA_ZHEJIANG
    mt_region_yc(object, &result);
#endif
    return; 
}

/**
*********************************************************************
* @name      : mt_region_dispatch
* @brief     ：分派区间统计任务
* @param[in] ：clientid   数据中心句柄
* @param[in] ：object      统计对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_region_dispatch(DB_CLIENT clientid, MT_REGION_OBJ_T *object)
{
    int ret = ERR_OK;
    uint32 units = 0;
    uint32 i = 0;
    OOP_DIGIT_T srcData[128];

    object->srcData = srcData;

    //数据格式归一化接口
    if(object->mtAdapt != NULL)
    {
        ret = object->mtAdapt(clientid, object->subOAD, &units, srcData);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("mtAdapt failed. ret(%d), oad(0x%08x)\n", ret, object->subOAD.value);
            return;
        }
    }
    else
    {
        STAT_FMT_DEBUG("oad(0x%08x) region calc not support.\n", object->subOAD.value);
    }
    
    //分项需要拆开计算
    if(units == 0)
    {
        mt_region_record(clientid, object);
    }
    else
    {
        for(i = 0; i < units; i++)
        {
            object->nIndex = i+1;
            mt_region_record(clientid, object);
        }
    }
    
    return;  
}

/**
*********************************************************************
* @name      : mt_region_entry
* @brief     ：区间统计入口
* @param[in] ：clientid 数据中心句柄
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_region_entry(DB_CLIENT clientid)
{
    uint32 i = 0;
    MT_REGION_OBJ_T *tmpNode = NULL;
    OOP_TI_SPAN_E time_span;

    STAT_FMT_TRACE("****** mt_region_entry start ****** \n");
    
    for(i = 0; i < g_region_objs_max; i++)
    {
        tmpNode = &g_region_objs[i];
        
        mt_period_analyse(tmpNode->mainOAD, &time_span);

        mt_region_dispatch(clientid, tmpNode);
        
        tmpNode->freqCnt++;

        //刷新周期数
        mt_pericnt_update(time_span, tmpNode->period, &tmpNode->periCnt, &tmpNode->freqCnt);           
    }

    STAT_FMT_TRACE("****** mt_region_entry end ****** \n\n");
    
    return;
}

#endif

#if DESC("极值统计函数",1)
#ifdef AREA_ZHEJIANG
/* 把极值统计数据直接赋值给遥测点 */
static void mt_extreme_yc(MT_EXTREME_OBJ_T *object, OOP_EXTREMERES_T *result)
{
    uint32 subOADValue = object->subOAD.value + object->nIndex;
    uint32 ArrIdx  = 0;//一维数组索引(起始)

    ArrIdx = g_aYcDataIndexZhj[1][0][0];

    if (0x21220200 == object->mainOAD.value)
    {
        switch (subOADValue)
        {
            case 0x20260200:    /* 当日电压不平衡度极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 0].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 0].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 0 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 0 + 10].bNewValidFlag = TRUE;
                break;
            case 0x20270200:    /* 当日电流不平衡度极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 1].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 1].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 1 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 1 + 10].bNewValidFlag = TRUE;
                break;
            case 0x200F0200:    /* 当日频率极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 2].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 2].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 2 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 2 + 10].bNewValidFlag = TRUE;
                break;
            case 0x200D0201:    /* 当日A相电压谐波总畸变率极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 3].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 3].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 3 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 3 + 10].bNewValidFlag = TRUE;
                break;
            case 0x200D0301:    /* 当日B相电压谐波总畸变率极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 4].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 4].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 4 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 4 + 10].bNewValidFlag = TRUE;
                break;
            case 0x200D0401:    /* 当日C相电压谐波总畸变率极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 5].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 5].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 5 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 5 + 10].bNewValidFlag = TRUE;
                break;
            case 0x20010600:    /* 当日零序电流极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 6].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 6].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 6 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 6 + 10].bNewValidFlag = TRUE;
                break;
            case 0x20000201:    /* 当日A相电压极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 7].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 7].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 7 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 7 + 10].bNewValidFlag = TRUE;
                break;
            case 0x20000202:    /* 当日B相电压极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 8].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 8].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 8 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 8 + 10].bNewValidFlag = TRUE;
                break;
            case 0x20000203:    /* 当日C相电压极值 */
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 9].fNewValue = result->dbMaxValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 9].bNewValidFlag = TRUE;

                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 9 + 10].fNewValue = result->dbMinValue.wVal;
                g_aYcDataSendZhj[ArrIdx + g_nYcStatExtremeDay + 9 + 10].bNewValidFlag = TRUE;
                break;
            default:
                break;
        }
    }
}
#endif

/**
*********************************************************************
* @brief：      极大值计算
* @param[in]： rtData   数据
* @param[in]： curTime  当前时间
* @param[out]: result   计算结果
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void mt_extreme_max_calc(OOP_DIGIT_T rtData, time_t curTime, OOP_DIGIT_T *value, OOP_DATETIME_S_T *time)
{
    uint8 changedFlag = 0;

    switch(rtData.choice)
    {  
        case DT_DOUBLE_LONG: //double long/int32
        {
            if(rtData.nVal > value->nVal)
            {
                changedFlag = 1;
                value->nVal = rtData.nVal;
            }       
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            if(rtData.dwVal > value->dwVal)
            {
                changedFlag = 1;
                value->dwVal = rtData.dwVal;
            }

        }break;     
        case DT_INTEGER: //integer/char
        {
            if(rtData.cVal > value->cVal)
            {
                changedFlag = 1;
                value->cVal = rtData.cVal;
            }

        }break;
        case DT_LONG: //long/int16
        {
            if(rtData.lVal > value->lVal)
            {
                changedFlag = 1;
                value->lVal = rtData.lVal;
            }

        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            if(rtData.bVal > value->bVal)
            {
                changedFlag = 1;
                value->bVal = rtData.bVal;
            }

        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            if(rtData.wVal > value->wVal)
            {
                changedFlag = 1;
                value->wVal = rtData.wVal;
            }      
        }break;   
        case DT_LONG64: //long64/int64
        {
            if(rtData.llVal > value->llVal)
            {
                changedFlag = 1;
                value->llVal = rtData.llVal;
            }          
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {
            if(rtData.ullVal > value->ullVal)
            {
                changedFlag = 1;
                value->ullVal = rtData.ullVal;
            }       
        }break;
        default:break;
    }
    
    if(changedFlag)
    {
        DT_Time2DateTimeShort(curTime, time);  
    }
    
    return;      
}

/**
*********************************************************************
* @brief：      极小值计算
* @param[in]： rtData   数据
* @param[in]： curTime  当前时间
* @param[out]: result   计算结果
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void mt_extreme_min_calc(OOP_DIGIT_T rtData, time_t curTime, OOP_DIGIT_T *value, OOP_DATETIME_S_T *time)
{
    uint8 changedFlag = 0;

    switch(rtData.choice)
    {  
        case DT_DOUBLE_LONG: //double long/int32
        {
            if(rtData.nVal < value->nVal)
            {
                changedFlag = 1;
                value->nVal = rtData.nVal;
            }       
        }break;
        case DT_DOUBLE_LONG_UNSIGNED: //double long unsigned/uint32
        {
            if(rtData.dwVal < value->dwVal)
            {
                changedFlag = 1;
                value->dwVal = rtData.dwVal;
            }

        }break;     
        case DT_INTEGER: //integer/char
        {
            if(rtData.cVal < value->cVal)
            {
                changedFlag = 1;
                value->cVal = rtData.cVal;
            }

        }break;
        case DT_LONG: //long/int16
        {
            if(rtData.lVal < value->lVal)
            {
                changedFlag = 1;
                value->lVal = rtData.lVal;
            }

        }break; 
        case DT_UNSIGNED: //unsigned/uint8
        {
            if(rtData.bVal < value->bVal)
            {
                changedFlag = 1;
                value->bVal = rtData.bVal;
            }

        }break;   
        case DT_LONG_UNSIGNED: //long unsigned/uint16
        {
            if(rtData.wVal < value->wVal)
            {
                changedFlag = 1;
                value->wVal = rtData.wVal;
            }      
        }break;   
        case DT_LONG64: //long64/int64
        {
            if(rtData.llVal < value->llVal)
            {
                changedFlag = 1;
                value->llVal = rtData.llVal;
            }          
        }break;   
        case DT_LONG64_UNSIGNED: //long64 unsigned/uint64
        {
            if(rtData.ullVal < value->ullVal)
            {
                changedFlag = 1;
                value->ullVal = rtData.ullVal;
            }       
        }break;
        default:break;
    }
    
    if(changedFlag)
    {
        DT_Time2DateTimeShort(curTime, time);
    }
    
    return;      
}

/**
*********************************************************************
* @brief：      极值计算
* @param[in]： rtData   数据
* @param[in]： curTime  当前时间
* @param[out]: result   计算结果
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void mt_extreme_calc(OOP_DIGIT_T rtData, time_t curTime, OOP_EXTREMERES_T *result)
{
    STAT_FMT_TRACE("extreme: rtData(%llu), lstMax(%llu), lstMin(%llu)\n", 
        rtData.ullVal, result->dbMaxValue.ullVal, result->dbMinValue.ullVal);

    //构造极大值结构
    mt_extreme_max_calc(rtData, curTime, &result->dbMaxValue, &result->tmMax);

    //构造极小值结构
    mt_extreme_min_calc(rtData, curTime, &result->dbMinValue, &result->tmMin);

    return;
}

/**
*********************************************************************
* @brief：      极值统计计算(通用计算)
* @param[in]： clientid   数据中心句柄
* @param[in]： object      统计对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     void
*********************************************************************
*/
void mt_extreme_record(DB_CLIENT clientid, MT_EXTREME_OBJ_T *object)
{
    int ret = ERR_OK;
    OOP_EXTREMERES_T result = {{0}};
    uint32 len = sizeof(OOP_EXTREMERES_T);
    bool isUpdate = FALSE;
    OOP_DIGIT_T rtData = object->srcData[MT_ATTR_INDEX(object->nIndex)];
    uint32 storeTm = 0;
    time_t curTime = hex_time_to_sec(&g_stMtCurBinTime);
    RECORD_OAD_T recordOAD;

    MEMZERO(&recordOAD, sizeof(recordOAD));

    recordOAD.logicId = 7;
    recordOAD.road.oadMain.value = object->mainOAD.value;
    recordOAD.road.oadCols.nNum = 1;
    recordOAD.road.oadCols.oad[0].value = object->subOAD.value + object->nIndex;

    result.obj.value = recordOAD.road.oadCols.oad[0].value;

    /* 首次达到统计周期，先清历史数据 */    
    if(object->freqCnt == 0)
    {
        //初始，最大最小值都设为当前值
        memcpy(&result.dbMaxValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, &result.tmMax);
        memcpy(&result.dbMinValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, &result.tmMin); 
        
        ret = mt_record_write(clientid, &recordOAD, curTime, &result, len);

        STAT_FMT_DEBUG("extreme: new cycle begin. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
           ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
    }

    //读取历史统计数据
    ret = mt_record_read(clientid, &recordOAD, &storeTm, &result, len);
    if(ret != ERR_OK)
    {
        STAT_FMT_DEBUG("extreme: history read failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
           ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, sizeof(result));

        //初始，最大最小值都设为当前值
        memcpy(&result.dbMaxValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, &result.tmMax);
        memcpy(&result.dbMinValue, &rtData, sizeof(OOP_DIGIT_T));
        DT_Time2DateTimeShort(curTime, &result.tmMin);  
        
        object->freqCnt = 0; //失败计数重新开始
    }
    else
    {
        isUpdate = TRUE;
    }

    //计算极值结果
    mt_extreme_calc(rtData, curTime, &result);

    STAT_FMT_TRACE("\r\n freqCnt(%d): oadMain(0x%08x), oadCols(0x%08x)\
                    \r\n dbMaxValue(%d), tmMax(%4d-%02d-%02d %02d:%02d:%02d)\
                    \r\n dbMinValue(%d), tmMin(%4d-%02d-%02d %02d:%02d:%02d)\n", 
                    object->freqCnt, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, 
                    result.dbMaxValue.wVal,
                    result.tmMax.year,result.tmMax.month,result.tmMax.day,
                    result.tmMax.hour,result.tmMax.minute,result.tmMax.second,
                    result.dbMinValue.wVal,
                    result.tmMin.year,result.tmMin.month,result.tmMin.day,
                    result.tmMin.hour,result.tmMin.minute,result.tmMin.second);

    //累积完成后回写
    if(isUpdate)
    {
        ret = mt_record_update(clientid, &recordOAD, storeTm, &result, len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("extreme: update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
               ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
        }
    }
    else
    {
        ret = mt_record_write(clientid, &recordOAD, curTime, &result, len);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("extreme: write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxLen(%d)\n", 
               ret, recordOAD.road.oadMain.value, recordOAD.road.oadCols.oad[0].value, len);
        }
    }

#ifdef AREA_ZHEJIANG
    mt_extreme_yc(object, &result);
#endif
    return;     
}

/**
*********************************************************************
* @name      : mt_extreme_dispatch
* @brief     ：分派极值统计任务
* @param[in] ：clientid   数据中心句柄
* @param[in] ：object      统计对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_extreme_dispatch(DB_CLIENT clientid, MT_EXTREME_OBJ_T *object)
{
    int ret = ERR_OK;
    uint32 units = 0;
    uint32 i = 0;
    OOP_DIGIT_T srcData[128];

    object->srcData = srcData;

    //数据格式归一化接口
    if(object->mtAdapt != NULL)
    {
        ret = object->mtAdapt(clientid, object->subOAD, &units, srcData);
        if(ret != ERR_OK)
        {
            STAT_FMT_DEBUG("mtAdapt failed. ret(%d), oad(0x%08x)\n", ret, object->subOAD.value);
            return;
        }
    }
    else
    {
        STAT_FMT_DEBUG("oad(0x%08x) extreme calc not support.\n", object->subOAD.value);
    }
    
    //分项需要拆开计算
    if(units == 0)
    {
        mt_extreme_record(clientid, object);
    }
    else
    {
        for(i = 0; i < units; i++)
        {
            object->nIndex = i+1;
            mt_extreme_record(clientid, object);
        }
    }
    
    return;  
}

/**
*********************************************************************
* @name      : mt_extreme_entry
* @brief     ：极值统计入口
* @param[in] ：clientid 数据中心句柄
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_extreme_entry(DB_CLIENT clientid)
{
    uint32 i = 0;
    MT_EXTREME_OBJ_T *tmpNode = NULL;
    OOP_TI_SPAN_E time_span;

    STAT_FMT_TRACE("****** mt_extreme_entry start ****** \n");
    
    for(i = 0; i < g_extreme_objs_max; i++)
    {
        tmpNode = &g_extreme_objs[i];
        
        mt_period_analyse(tmpNode->mainOAD, &time_span);

        mt_extreme_dispatch(clientid, tmpNode);
        
        tmpNode->freqCnt++;

        //刷新周期数
        mt_pericnt_update(time_span, tmpNode->period, &tmpNode->periCnt, &tmpNode->freqCnt);           
    }

    STAT_FMT_TRACE("****** mt_extreme_entry end ****** \n\n");
    
    return;      
}

#endif

#if DESC("数据冻结函数",1)

/**
*********************************************************************
* @brief：      读取冻结列对象数据
* @param[in]： ColOAD 冻结关联对象OAD
* @param[out]：outData 输出缓冲区
* @param[in]： outLen  输出缓大小
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     真实数据长度
*********************************************************************
*/
int mt_frozen_objdata_read(DB_CLIENT clientid, OOP_OAD_U colOAD, uint8 *outData, uint32 outLen, uint32 *realLen)
{
    int ret = ERR_OK;
    NOMAL_OAD_T nomalOAD = {0};   

    //块数据直接读
    if(colOAD.nIndex == 0)
    {
        nomalOAD.logicId = 1; //交采
        nomalOAD.oad.value = colOAD.value;
        
        ret = db_read_nomal(clientid, &nomalOAD, outLen, outData, realLen);
        if(ret != 0)
        { 
            nomalOAD.logicId = 0; //终端

            ret = db_read_nomal(clientid, &nomalOAD, outLen, outData, realLen);
            if(ret != 0)
            {
                STAT_FMT_DEBUG("frozen data read failed.ret(%d), logicId(%d), infoNum(%d), oad(0x%08x), inMaxLen(%d)\n",
                    ret, nomalOAD.logicId, nomalOAD.infoNum, nomalOAD.oad.value, outLen);
                
                return ret;
            }            
        }
    }
    else
    {
        //Nothing to do
    }

    return ret;
}

/**
*********************************************************************
* @brief：      写一条冻结记录
* @param[in]： ColOAD 冻结关联对象OAD
* @param[out]：outData 输出缓冲区
* @param[in]： outLen  输出缓大小
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return：     真实数据长度
*********************************************************************
*/
uint16 mt_frozen_record_write(DB_CLIENT clientid, OOP_OAD_U mainOAD, OOP_OAD_U subOAD, const uint8 *dataBuf, const uint32 dataLen)
{
    int ret = ERR_OK;
    WRITE_RECORD_T inData;
    uint32 storeNo = 0;
    //uint8 *pAddr = NULL;
    time_t storeTime = hex_time_to_sec(&g_stMtCurBinTime);

    MEMZERO(&inData, sizeof(WRITE_RECORD_T));
    
    //获取终端地址
    //app_get_localaddr(&pAddr);

    // 校验码，通信地址的前6个字节
    //inData.MAC.nNum = 6;
    //memcpy(inData.MAC.value, pAddr, 6);

    // 注：+8个小时(28800)转成格林时间
    inData.recordOAD.logicId = 7;
    inData.colStoreTm =  storeTime + COMPENSATION_TIME;
    inData.recordOAD.classtag = CLASS_DATA_NORMAL;
    inData.recordOAD.road.oadMain.value = mainOAD.value;
    inData.recordOAD.road.oadCols.nNum = 1;
    inData.recordOAD.road.oadCols.oad[0].value = subOAD.value;
    
    inData.inDatalen = dataLen;
    memcpy(inData.inDataBuf, dataBuf, dataLen);


    ret = db_write_record(clientid, &inData, &storeNo);    
    if(ret != ERR_OK)
    {
        return ret;
    }

    STAT_FMT_TRACE("tmn frozen record write: oadCols.oad[0] = %lu(0x%08x)\n", 
        inData.recordOAD.road.oadCols.oad[0].value, inData.recordOAD.road.oadCols.oad[0].value);

    return ret;
}

/**
*********************************************************************
* @name      : mt_frozen_exec
* @brief     ：执行冻结
* @param[in] ：clientid 冻结参数
* @param[in] ：object 冻结对象
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_frozen_exec(DB_CLIENT clientid, MT_FROZEN_OBJ_T *object)
{
    int ret = ERR_OK;
    uint8 frozenData[300];
    uint32 frozenDataLen;

    /*读出当前实时数据*/
    ret = mt_frozen_objdata_read(clientid, object->subOAD, frozenData, sizeof(frozenData), &frozenDataLen);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("frozen: objdata read failed. ret(%d) OAD(0x%08x)\n", ret, object->subOAD.value);
        return;   
    }
    
    /*转存为冻结数据*/
    ret = mt_frozen_record_write(clientid, object->mainOAD, object->subOAD, frozenData, frozenDataLen);
    if(ret != 0)
    {
        STAT_FMT_DEBUG("frozen: record write failed. ret(%d), mainOAD(0x%08x), subOAD(0x%08x)\n", ret, object->mainOAD.value, object->subOAD.value);
        return;   
    }

    return;
}

/**
*********************************************************************
* @name      : mt_frozen_entry
* @brief     ：数据冻结入口
* @param[in] ：clientid 数据中心句柄
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_frozen_entry(DB_CLIENT clientid)
{
    uint32 i = 0;
    MT_FROZEN_OBJ_T *tmpNode = NULL;
    OOP_TI_SPAN_E time_span;

    STAT_FMT_TRACE("****** mt_frozen_entry start ****** \n");
    
    for(i = 0; i < g_frozen_objs_max; i++)
    {
        tmpNode = &g_frozen_objs[i];
        
        mt_period_analyse(tmpNode->mainOAD, &time_span);

        if(mt_pericnt_update(time_span, tmpNode->period, &tmpNode->periCnt, NULL) == TRUE)
        {
            mt_frozen_exec(clientid, tmpNode);   
        }
    }
    
    STAT_FMT_TRACE("****** mt_frozen_entry end ****** \n\n");
    
    return;      
}

#endif

#if DESC("监测统计函数",1)

/**********************************************************************
* @name      : mt_powercut_calc
* @brief     ：停电监测
* @param[in] ：clientid    数据中心句柄
* @param[in] ：mainOAD     主OAD
* @return    ：void
* @Create    : 温占礼
* @Date      ：2020-12-04
* @Update    :
**********************************************************************/
void mt_powercut_calc(DB_CLIENT clientid, OOP_OAD_U mainOAD, uint32 freqCnt)
{
    int ret = ERR_OK;
    READ_RECORD_T readRecord = {{0}};
    RECORD_UNIT_T recordBuf = {0};
    WRITE_RECORD_T writeRecord;
    UPDATE_RECORD_T updateRecord;
    MT_POWERCUT_RES_T result={{0}};
    uint32 len = sizeof(MT_POWERCUT_RES_T);
    result.obj.value = mainOAD.value;
    uint32 storeNo;
    uint32 temptime=0;
    int state;
    result.pwctFLag=TRUE;
    
    MEMZERO(&readRecord, sizeof(readRecord));
    MEMZERO(&writeRecord, sizeof(writeRecord));
    MEMZERO(&updateRecord, sizeof(updateRecord));
    
    get_power_state(&state);
    
    STAT_FMT_DEBUG("get_power_state state(%d)\n",state);
    if(state==0)                  //掉电
    {
        if(freqCnt!=0)                  //未到一个周期重复掉电
        {
        readRecord.recordOAD.logicId = 7;
        readRecord.recordOAD.road.oadMain.value = 0x00000000;
        readRecord.recordOAD.road.oadCols.nNum = 1;
        readRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
        ret = db_read_record(clientid, &readRecord, sizeof(result), &recordBuf);  //读上条记录
        if(ret==ERR_OK)
        {
        result.pwctFLag=FALSE;
        result.tmstart=time(NULL) + COMPENSATION_TIME;
		result.tmstop = time(NULL) + COMPENSATION_TIME;
        updateRecord.filter.choice = 1;
        updateRecord.recordOAD.logicId = 7;
        updateRecord.recordOAD.road.oadMain.value = 0x00000000;
        updateRecord.recordOAD.road.oadCols.nNum = 1;
        updateRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
        updateRecord.inDatalen = len;
        updateRecord.filter.storeTm = recordBuf.colStoreTm;                     //更新到上条记录
        memcpy(updateRecord.inDataBuf, (uint8*)&result, len);
        ret = db_update_record(clientid, &updateRecord);   
        if(ret != ERR_OK)
        {
        STAT_FMT_DEBUG("region update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
        ret, updateRecord.recordOAD.road.oadMain.value, updateRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
        STAT_FMT_DEBUG("freqCnt!=0 update success oadMain(0x%08x),counttime(%d)\n",updateRecord.recordOAD.road.oadCols.oad[0].value,result.countTime);
        return;
          
        }
       
        }
        else
        {
        result.pwctFLag=FALSE;       //置flag标记为False 停电
        result.tmstart=time(NULL) + COMPENSATION_TIME;  //当前时间 
        result.tmstop = time(NULL) + COMPENSATION_TIME;
        writeRecord.recordOAD.logicId = 7;
        writeRecord.recordOAD.road.oadCols.nNum = 1;
        writeRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
        writeRecord.inDatalen = len;
        writeRecord.colStoreTm = result.tmstart;
        memcpy(writeRecord.inDataBuf, (uint8*)&result, len);
        ret = db_write_record(clientid, &writeRecord, &storeNo);   
        if(ret != ERR_OK)
        {
        STAT_FMT_DEBUG("region write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
        ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
        }
        STAT_FMT_DEBUG("freqCntsystem powercut write success!ret(%d),oadMain(0x%08x),result.tmstart(%ld)\n",ret,writeRecord.recordOAD.road.oadCols.oad[0].value,result.tmstart);
        }  
    }
    else
    {
        readRecord.recordOAD.logicId = 7;
        readRecord.recordOAD.road.oadMain.value = 0x00000000;
        readRecord.recordOAD.road.oadCols.nNum = 1;
        readRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
        ret = db_read_record(clientid, &readRecord, sizeof(result), &recordBuf);  //读上条记录
        if(ret==ERR_OK)
        {
        	memcpy((uint8*)&result,&recordBuf.record[0].outDataBuf,len);
        	STAT_FMT_DEBUG("last powercut tile read success oadMain(0x%08x),flag(%d),counttime(%d)\n",mainOAD.value,result.pwctFLag,result.countTime);
 			if((time(NULL) + COMPENSATION_TIME)-result.tmstop>180)//三分钟超时，没有接受到掉电state
 			{
				 
				STAT_FMT_DEBUG("result.tmstart(%ld),result.tmstop(%ld)\n",result.tmstart,result.tmstop);
				updateRecord.filter.choice = 1;
				updateRecord.recordOAD.logicId = 7;
				updateRecord.recordOAD.road.oadMain.value = 0x00000000;
				updateRecord.recordOAD.road.oadCols.nNum = 1;
				updateRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
				updateRecord.inDatalen = len;
				updateRecord.filter.storeTm = recordBuf.colStoreTm;
				result.pwctFLag=TRUE;                           //置flag为在电
				temptime=(result.tmstop-result.tmstop)/60;              //时间差
				result.countTime += temptime;
				result.tmstart=time(NULL) + COMPENSATION_TIME;
				memcpy(updateRecord.inDataBuf, (uint8*)&result, len);
				ret = db_update_record(clientid, &updateRecord);   
				if(ret != ERR_OK)
				{
				STAT_FMT_DEBUG("region update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
				ret, updateRecord.recordOAD.road.oadMain.value, updateRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
				}
				
 			}
			else
			{
	            if(result.pwctFLag==FALSE)           //确认发生停电
	            {
	            //更新结尾时间
	            result.tmstop=time(NULL) + COMPENSATION_TIME;  //当前时间 
	            STAT_FMT_DEBUG("result.tmstart(%ld),result.tmstop(%ld)\n",result.tmstart,result.tmstop);
	            updateRecord.filter.choice = 1;
	            updateRecord.recordOAD.logicId = 7;
	            updateRecord.recordOAD.road.oadMain.value = 0x00000000;
	            updateRecord.recordOAD.road.oadCols.nNum = 1;
	            updateRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
	            updateRecord.inDatalen = len;
	            updateRecord.filter.storeTm = recordBuf.colStoreTm;
	            result.pwctFLag=TRUE;                           //置flag为在电
	            temptime=(result.tmstop-result.tmstart)/60;              //时间差，简单计算需要优化
	            result.countTime += temptime;
				result.tmstart=time(NULL) + COMPENSATION_TIME;
	            memcpy(updateRecord.inDataBuf, (uint8*)&result, len);
	            ret = db_update_record(clientid, &updateRecord);   
	            if(ret != ERR_OK)
	            {
	            STAT_FMT_DEBUG("region update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
	            ret, updateRecord.recordOAD.road.oadMain.value, updateRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
	            }
	            STAT_FMT_DEBUG("update yes oadMain(0x%08x),counttime(%d)\n",updateRecord.recordOAD.road.oadCols.oad[0].value,result.countTime);

	            return;

	            }
				
				else			//没有发生停电，一直在电，每次更新末尾时间
				{

				if(freqCnt==0)
				{
				result.pwctFLag=TRUE;       //置flag标记为0xaa 在电
				result.tmstart=time(NULL) + COMPENSATION_TIME;
				result.tmstop=time(NULL) + COMPENSATION_TIME;
				result.countTime=0;
				writeRecord.recordOAD.logicId = 7;
				writeRecord.recordOAD.road.oadCols.nNum = 1;
				writeRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
				writeRecord.inDatalen = len;
				writeRecord.colStoreTm = time(NULL) + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
				memcpy(writeRecord.inDataBuf, (uint8*)&result, len);
				ret = db_write_record(clientid, &writeRecord, &storeNo);   
				if(ret != ERR_OK)
				{
				STAT_FMT_DEBUG("region write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
				ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
				}
				STAT_FMT_DEBUG("freqCnt=0,write a new oadMain(0x%08x),storeNo(%d),counttime(%d)\n",writeRecord.recordOAD.road.oadCols.oad[0].value,storeNo,result.countTime);
				}
				else
				{
				result.tmstop=time(NULL) + COMPENSATION_TIME;  //当前时间 
				STAT_FMT_DEBUG("result.tmstart(%ld),result.tmstop(%ld)\n",result.tmstart,result.tmstop);
				updateRecord.filter.choice = 1;
				updateRecord.recordOAD.logicId = 7;
				updateRecord.recordOAD.road.oadMain.value = 0x00000000;
				updateRecord.recordOAD.road.oadCols.nNum = 1;
				updateRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
				updateRecord.inDatalen = len;
				updateRecord.filter.storeTm = recordBuf.colStoreTm;
				result.pwctFLag=TRUE;                           //置flag为在电
				result.tmstart=time(NULL) + COMPENSATION_TIME;
				memcpy(updateRecord.inDataBuf, (uint8*)&result, len);
				ret = db_update_record(clientid, &updateRecord);   
				if(ret != ERR_OK)
				{
				STAT_FMT_DEBUG("region update failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
				ret, updateRecord.recordOAD.road.oadMain.value, updateRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
				}				
				}
			
				}
        	}
            
            }
        else
            {
             
            STAT_FMT_DEBUG("First have no title powercut!!\n");
            
            result.pwctFLag=TRUE;
            
			result.tmstart=time(NULL) + COMPENSATION_TIME;
			result.tmstop=time(NULL) + COMPENSATION_TIME;
            result.countTime=0;         //第一次停电时间初始化为0
            writeRecord.recordOAD.logicId = 7;
            writeRecord.recordOAD.road.oadCols.nNum = 1;
            writeRecord.recordOAD.road.oadCols.oad[0].value = mainOAD.value;
            writeRecord.inDatalen = len;
            writeRecord.colStoreTm = time(NULL) + COMPENSATION_TIME; // 注：+8个小时(28800)转成格林时间
            
            memcpy(writeRecord.inDataBuf, (uint8*)&result, len);
            ret = db_write_record(clientid, &writeRecord, &storeNo);   
            if(ret != ERR_OK)
            {
            STAT_FMT_DEBUG("region write failed. ret(%d) oadMain(0x%08x), oadCols(0x%08x), maxBufLen(%d)\n", 
            ret, writeRecord.recordOAD.road.oadMain.value, writeRecord.recordOAD.road.oadCols.oad[0].value, sizeof(result));
            }
            STAT_FMT_DEBUG("First db_write  sucuss ret(%d) storeNo(%d) oadMain(0x%08x)\n",ret,storeNo,writeRecord.recordOAD.road.oadMain.value);
            }
        }
        
            STAT_FMT_DEBUG("*******powercut done*******\n");
       return;
}

/**
*********************************************************************
* @name      : mt_powercut_entry
* @brief     ：停电监测入口
* @param[in] ：clientid 数据中心句柄
* @Create    : 贺宁
* @Date      ：2020-11-26
* @return    ：void
*********************************************************************
*/
void mt_powercut_entry(DB_CLIENT clientid)
{
    uint32 i = 0;
    MT_POWERCUT_OBJ_T *tmpNode = NULL;
    OOP_TI_SPAN_E time_span;

    STAT_FMT_TRACE("******* mt_powercut_entry start *******\n");
    
    for(i = 0; i < g_powercut_objs_max; i++)
    {
        tmpNode = &g_powercut_objs[i];
        
        mt_period_analyse(tmpNode->mainOAD, &time_span);

        mt_powercut_calc(clientid,tmpNode->mainOAD,tmpNode->freqCnt);
 
        tmpNode->freqCnt++;

        //刷新周期数
        mt_pericnt_update(time_span, tmpNode->period, &tmpNode->periCnt, &tmpNode->freqCnt);           
    }

    STAT_FMT_TRACE("****** mt_powercut_entry end ****** \n\n");
    
    return;    
}

#endif

/**********************************************************************
* @name      : mt_calc_entry
* @brief     ：监测统计入口
* @param[in] ：clientid    数据中心句柄
* @param[out]：NA
* @return    ：void
* @Create    : 贺宁
* @Date      ：2020-11-26
* @Update    :
**********************************************************************/
void mt_calc_entry(DB_CLIENT clientid)
{
    mt_qr_entry(clientid);
    mt_region_entry(clientid);
    mt_extreme_entry(clientid);
    //mt_frozen_entry(clientid);
    //mt_powercut_entry(clientid);

    return;
}

