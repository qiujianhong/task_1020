/*
*********************************************************************
* Copyright(C) 2024 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   负荷调控数据
* @date：    2024-06-27
* @history： 
*********************************************************************
*/
#ifndef __LOADCTRLLER_STAT_H__
#define __LOADCTRLLER_STAT_H__

#ifdef __cplusplus
    extern "C" {
#endif

#define EN_SHARP		1		/*电量尖段*/
#define EN_TOP			2		/*电量峰段*/
#define EN_LEVEL		3		/*电量平段*/
#define EN_BOTTOM		4		/*电量谷段*/

#define POS_AE_Tag      0		/*00正向有功*/
#define POS_RE_Tag	    1		/*10正向无功*/
#define NEG_AE_Tag	    2		/*01反向有功*/
#define NEG_RE_Tag	    3		/*11反向无功*/

#define DM_POWER_MAX    15      /*需量计算数据量*/
typedef struct tagLctrlPulseCount
{
    uint32    val;             /*脉冲数*/
    uint32    interval;        /*间隔时间*/
    uint32    caluleft;        /*计算余数*/

    uint8     powerNum;
    uint8     res[3];
    uint32    powerList[DM_POWER_MAX]; /*需量计算存储的功率*/
}LCTRL_PULSE_COUNT_T;

typedef struct tagLctrPulseTemp
{
    uint32     count;      //本次统计增加数量，计算后清0
    uint32     timems;     //本次统计增加统计时间，计算后清0
    uint32     totalcou;   //上次统计总数
    uint32     totals;     //上次统计总时间秒
    uint16     totalms;    //上次统计总时间毫秒
    uint8      res[2];
    Date_Time  start;      //上次统计启动时间
}LCTRL_PULSE_TEMP_T;


/* 遥信、脉冲计算中间过程数据(当前按照遥信、脉冲最大8路实现) */
/* 遥信按照模组 */
typedef struct tagLcPulse
{
    uint8               pulseNum;                  /* 模块实际接入的脉冲数量 */
    LCTRL_PULSE_COUNT_T Pulse[OOP_MAX_PULSE];      //脉冲计算原始数据
    LCTRL_PULSE_TEMP_T  tPulseTemp[OOP_MAX_PULSE]; //模块采集到的原始数据
}LCTRL_PULSE_T;

typedef struct tagLcPulseCFG
{
    OOP_VISIBLEVAR_T     tAddr;         //通信地址
    OOP_PTCT_T           tPTCT;
    OOP_PULSESETS_T      tPulseCfg;     //脉冲配置
    int32                nActivePower;  //有功功率
    int32                nReactivePower;//无功功率

    OOP_ENERGY_T         tDayForwActEn; //当日正向有功电量
    OOP_ENERGY_T         tMonForwActEn; //当月正向有功电量
    OOP_ENERGY_T         tDayBackActEn; //当日反向有功电量
    OOP_ENERGY_T         tMonBackActEn; //当月反向有功电量
    OOP_ENERGY_T         tDayForwRactEn; //当日正向无功电量
    OOP_ENERGY_T         tMonForwRactEn; //当月正向无功电量
    OOP_ENERGY_T         tDayBackRactEn; //当日反向无功电量
    OOP_ENERGY_T         tMonBackRactEn; //当月反向无功电量

    OOP_ENERGY_T         tShowForwActEn; //正向有功电能示值
    OOP_ENERGY_T         tShowForwRactEn;//正向无功电能示值
    OOP_ENERGY_T         tShowBackActEn; //反向有功电能示值
    OOP_ENERGY_T         tShowBackRactEn;//反向无功电能示值

    OOP_PULSESETS_B_T    tBigCfg;
}
LCTRL_PULSE_CFG_T;

typedef struct tagLCtrlSATA
{
    OOP_DATETIME_S_T     lastTime;                  /* 计算时间 */
    
    /* 脉冲参数与数据 */
    OOP_DAYPERIOD_T      tDayPeriod;                   //当前套日时段表 0x40160200
    LCTRL_PULSE_CFG_T    tPulseOOP[OOP_MAX_PULSE];     //脉冲数据中心配置与数据2401~2408
    LCTRL_PULSE_T        tPulseData;                   //脉冲接口

    /* 统计参数与数据 */
    
}LCTRL_SATA_T;



#ifdef __cplusplus
}
#endif

#endif //__LA_MSG_H__

