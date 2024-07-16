/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   遥信脉冲模块头文件
* @date：    2019-11-19
* @history： 
*********************************************************************
*/

#ifndef __PULSE_ADVH__
#define __PULSE_ADVH__

#include "appmsg_split.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PULSE_DB_QUEUE_SIZE       40
typedef struct tagDBNotify 
{
	pthread_mutex_t    f_lock;	
	uint32             num;
	uint32             OAD[PULSE_DB_QUEUE_SIZE];
}DB_NOTIFY_T;

typedef struct tagPulseTime
{
    uint8 year;  /*年份0代表1900年*/
    uint8 month; /*月份0代表1月*/
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;

    uint16 milliseconds;
}TIME_STRUCT_T;    /*时间结构*/

typedef struct tagPulseCount
{
    uint32    val;             /*脉冲数*/
    uint32    interval;        /*间隔时间*/
    uint32    caluleft;        /*计算余数*/

    uint8     powerNum;
    uint8     res[3];
    uint32    powerList[DM_POWER_MAX]; /*需量计算存储的功率*/
}PULSE_COUNT_T;


typedef struct tagPulseMeasure
{
    OOP_VISIBLEVAR_T     tAddr;         //通信地址
    OOP_PTCT_T           tPTCT;
    OOP_PULSESETS_T      tPulseCfg;     //脉冲配置
    int32                nActivePower;  //有功功率
    int32                nReactivePower;//无功功率
#if 0
    OOP_ENERGY_T         tDayForwActEn; //当日正向有功电量
    OOP_ENERGY_T         tMonForwActEn; //当月正向有功电量
    OOP_ENERGY_T         tDayBackActEn; //当日反向有功电量
    OOP_ENERGY_T         tMonBackActEn; //当月反向有功电量
    OOP_ENERGY_T         tDayForwRactEn; //当日正向无功电量
    OOP_ENERGY_T         tMonForwRactEn; //当月正向无功电量
    OOP_ENERGY_T         tDayBackRactEn; //当日反向无功电量
    OOP_ENERGY_T         tMonBackRactEn; //当月反向无功电量
#endif
    OOP_ENERGY_T         tShowForwActEn; //正向有功电能示值
    OOP_ENERGY_T         tShowForwRactEn;//正向无功电能示值
    OOP_ENERGY_T         tShowBackActEn; //反向有功电能示值
    OOP_ENERGY_T         tShowBackRactEn;//反向无功电能示值

    OOP_PULSESETS_B_T    tBigCfg;
}PULSE_Measure_T;

typedef struct tagPulseTemp
{
    uint32     count;
    uint32     timems;
    uint32     totalcou;
    uint32     totals;
    uint16     totalms;
    uint8      res[2];
    Date_Time  start;
}PULSE_TEMP_T;

#define PULSE_CHG_MAX                  20
typedef struct tagPulseChgNode
{
    uint32               happentmes;
    uint32               systemtime;
    OOP_SWITCHIN_STATE_T sc; //变位状态
}PULSE_CHG_NODE_T;


typedef struct tagPulseChgList
{
    uint8                head;
    uint8                tail;
    PULSE_CHG_NODE_T     list[PULSE_CHG_MAX]; //变位状态
}PULSE_CHG_LIST_T;

/* 遥信脉冲数据中心相关配置与数据 */
typedef struct tagPulseOAD
{
    DB_CLIENT            dbclient;    //数据中心客户端
    void                *pHalDev;
    
    uint8                nSwitchNUM;  //遥信数量 0xf2030300
    uint8                nPulseNum;   //脉冲设备数量 
    uint8                bChangeTime; //遥信防抖时间变化
    uint8                bInitSwitch; //初始化是否存在遥信

    OOP_SWITCHIN_STATE_T tSwitchState; //遥信状态 0xf2030200
    OOP_SWITCHIN_FLAG_T  tSwitchFlag;  //遥信接入状态 0xf2030400
    OOP_STATUS_SIGNAL_T  tStatusSignal;//信号节点
    uint32               rmtTime;      //遥信防抖时间 0x49080200

    OOP_PULSE_T          tPulseInfo;   //脉冲输入设备(目前没用到) 0xf20a0200
    PULSE_Measure_T      tPulse[OOP_MAX_PULSE]; //脉冲配置与数据 2401~2408

    OOP_DAYPERIOD_T      tDayPeriod;   //当前套日时段表 0x40160200

    uint8                bPowerClose;  //停电
    uint8                bFirstGetRmt1;
    uint8                bFirstGetRmt2;
    PULSE_CHG_LIST_T     tList;
}PULSE_OAD_T;

#define PULSE_MODEL_MAX                      3
typedef struct tagPulseSet
{
    uint8    no;                                    //序号;0-无效
    uint8    bSetCfg;                               //是否支持设置配置
    uint8    res[1];
    uint8    portNum;                               //端口数；0-无效
    uint8    config[OOP_MAX_SWITCHIN + OOP_MAX_PULSE]; //配置: 最大8路遥信、8路脉冲
    uint8    realcfg[OOP_MAX_SWITCHIN + OOP_MAX_PULSE];//实际配置
}PULAE_SET_T;

typedef struct tagPulseCFG
{
    uint8         bHasPulse;
    uint8         mapSTStart;           //模组遥信开始位置(0开始, 表示1路)同终端本地遥信数量
    PULAE_SET_T   cfg[PULSE_MODEL_MAX]; //遥信脉冲配置

    char          datafile[256];             /* 脉冲数据保存路径 */
}PULSE_CFG_T;

/* 遥信、脉冲计算中间过程数据(当前按照遥信、脉冲最大8路实现) */
/* 遥信按照模组 */
typedef struct tagPulseCalculate
{
    uint8         curST[OOP_MAX_SWITCHIN];    //遥信脉冲APP返回的遥信状态
    uint8         realST[OOP_MAX_SWITCHIN];   //结合常开常闭和遥信接入综合汇总的情况
    PULSE_CFG_T  *pCfg;                    //遥信脉冲配置
    
    uint8         pulseNum;                  /* 模块实际接入的脉冲数量 */
    PULSE_COUNT_T Pulse[OOP_MAX_PULSE];      //脉冲计算原始数据
    PULSE_TEMP_T  tPulseTemp[OOP_MAX_PULSE]; //模块采集到的原始数据
    char          datafile[256];             /* 脉冲数据保存路径 */
}PULSE_CALCULATE_T;

uint32 read_oad_notify(DB_NOTIFY_T *pQueue);
void *pulse_outmsg_entry(void *pArg);
int pulse_write_OAD(uint32 uOAD, uint8 *pData, uint32 dataLen);
int pulse_write_INITOAD(uint32 uOAD, uint16 classtag, void *pData, uint32 dataLen);
int pulse_get_OAD(uint32 uOAD, uint8 *pBuf, uint32 bufLen);
int pulse_dbCenter_init(PULSE_OAD_T *ptCfg);
void pulse_get_curtime_s(TIME_STRUCT_T *pTime);
int pulse_calculate_init(PULSE_CALCULATE_T *ptCalu, TIME_STRUCT_T *pTime);
void pulse_proc_calculate(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu, TIME_STRUCT_T *pTime);
int app_set_configure(int taskid, PULSE_CFG_T *pPluseCfg);
void app_set_shaketime(int taskid, uint32 timelong);
void app_get_rmt(int taskid, PULSE_CFG_T *pPluseCfg);
void app_reproc_rmt(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu, uint8 bExit);
void app_proc_rmt_later(PULSE_OAD_T *ptCfg);
void app_proc_rmt(uint8 id, uint8 *pMsg, uint16 msglen, uint8 bReport, PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu);
void app_get_pulse(int taskid, PULSE_CALCULATE_T *ptCalu);
void pulse_change_K(int taskid, uint32 OAD, PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu);
void pulse_check_status(PULSE_OAD_T *ptCfg);

int pulse_update_db(uint32 uOAD, PULSE_OAD_T *ptCfg);
void pulse_write_normal_init(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu);
void pulse_arg_arg_init(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu);
void pulse_write_pulse(PULSE_CALCULATE_T *ptCalu);

#ifdef __cplusplus
}
#endif


#endif

