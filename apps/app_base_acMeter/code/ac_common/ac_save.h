
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#ifndef __AC_METER_SAVE_H__
#define __AC_METER_SAVE_H__

#include "ac_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


/*******************************************************************************
* 全局变量
*******************************************************************************/
extern COMMON_DATA_TYPE         gSaveCommonData;             // 公共交互变量 数据中心交互计时器
extern PRIVATE_DATA_TYPE        gSavePrivateData;            // 私有数据变量 数据中心交互计时器

extern const uint32 gSaveTaskAll[];
extern uint16 gSaveTaskAllNum;
extern const uint32 gSaveTaskPara[];
extern uint16 gSaveTaskParaNum;
extern const uint32 gActionPara[];
extern uint16 gActionParaNum;
extern const uint32 gRecordData[];
extern uint16 gRecordDataNum;
extern const uint32 gSubOadData[];
extern uint16 gSubOadDataNum;
extern const uint32 gSaveTaskEnergy[];
extern uint16 gSaveTaskEnergyNum;
extern const uint32 gSaveTaskDemand[];
extern uint16 gSaveTaskDemandNum;
extern const uint32 gSaveTaskRealtime[];
extern uint16 gSaveTaskRealtimeNum;
extern const uint32 gSaveTaskHarmonic[];
extern uint16 gSaveTaskHarmonicNum;
extern const uint32 gSaveTaskSpecial[];
extern uint16 gSaveTaskSpecialNum;
extern const uint32 gSaveTaskContainer[];
extern uint16 gSaveTaskContainerNum;

#define AC_TOPO_EVENT_NUM_MAX    5000  //终端本体拓扑识别事件最大支持个数

extern EVENT_COMM_INFO     gEventCommInfo;              //公共事件信息
extern char       *gEventParaPath;  //磁场干扰事件参数私有文件
extern uint16     gAcTopoEventNum;  //终端本体拓扑识别事件个数

extern TOTAL_EVENT_INFO gTotalEventInfo;//所有事件状态汇总
extern EVENT_300F_INFO event_300F;  //终端相序异常事件
extern void InitEventRptPara(void);

/*******************************************************************************
* 枚举和结构体
*******************************************************************************/

#define SAVE_TYPE_NULL    0  //无
#define SAVE_TYPE_ENERGY    1  //电量
#define SAVE_TYPE_DEMAND    2  //需量
#define SAVE_TYPE_REALTIME    3  //实时数据
#define SAVE_TYPE_HARMONIC    4  //谐波
#define SAVE_TYPE_SPECIAL    5  //特殊数据

#define STA_FROZEN_OBJNUMS    (8)   /*冻结对象个数（瞬时，秒，分，时，日，结算日，月，年）8个*/

/** @brief 冻结对象OI枚举   */
typedef enum tag_STA_FROZEN_OI
{
    STA_FROZEN_TRAN = 0x5000,
    STA_FROZEN_SEC = 0x5001,
    STA_FROZEN_MIN = 0x5002,
    STA_FROZEN_HOUR = 0x5003,
    STA_FROZEN_DAY = 0x5004,
    STA_FROZEN_SETTLING_DAY = 0x5005,
    STA_FROZEN_MON = 0x5006,
    STA_FROZEN_YEAR = 0x5007
}STA_FROZEN_OI_E;

typedef enum tag_STA_CALC_INTERVAL
{
    STA_CALC_DAY = 0,
    STA_CALC_MON = 1,
    STA_CALC_MAX
}STA_CALC_INTERVAL_E;

typedef struct tag_STA_VOLT_ACCTIME_CFG
{
    OOP_WORD3V_T phaseVolt;     //分相电压
    uint8 powerType;             //接线方式,单相表为 1，三相三线表为 2，三相四线表为 3
    OOP_VOLTPARAM_T voltParam;   //电压合格率参数
    uint16 addTime;              //到此次统计时累积时间
    STA_CALC_INTERVAL_E interval;//统计间隔
}STA_VOLT_ACCTIME_CFG_T;

/** @brief 冻结关联对象集合*/
typedef struct
{
    uint16 period;      //存储周期
    uint16 storeDepth;  //存储深度
    time_t lastFrzTm;   //上一次冻结时间
    uint16 oadNum;      //关联对象个数
    OOP_OAD_U *pColOad;      //关联对象
}FrzRunPara_t;

typedef struct str_FrzRunParaList
{
    OOP_OAD_U frzOAD;               /*冻结类型OAD（秒，分，时，日，月，年）*/
    FrzRunPara_t frzRunPara;   /* 冻结参数（包含关联的对象属性和冻结方案）*/
    struct str_FrzRunParaList *pNext;
}FrzRunParaList_t;

typedef struct tag_STA_PRAM_UPDATE
{
    pthread_mutex_t mutex;
    uint8 frzInitFlag;
    uint8 regionInitFlag;
    uint8 avgInitFlag;
    uint8 extremeInitFlag; 
    uint8 timeUpdate; 
}STA_PARAM_UPDATE;

typedef struct tag_SAVE_FLAG
{
    BOOL bFirstSaveEnergy;    //程序启动, 第一次存储标志, 电量
    BOOL bFirstSaveDemand;    //程序启动, 第一次存储标志, 需量
    BOOL bFirstSaveRealtime;  //程序启动, 第一次存储标志, 实时数据
    BOOL bFirstSaveHarmonic;  //程序启动, 第一次存储标志, 谐波
    BOOL bFirstSaveSpecial;   //程序启动, 第一次存储标志, 特殊数据
    BOOL bFirstSampEnergy;    //程序启动, 第一次采集标志, 电量
    BOOL bFirstSampDemand;    //程序启动, 第一次采集标志, 需量
    BOOL bFirstSampRealtime;  //程序启动, 第一次采集标志, 实时数据
    BOOL bFirstSampHarmonic;  //程序启动, 第一次采集标志, 谐波
    BOOL bFirstSampSpecial;   //程序启动, 第一次采集标志, 特殊数据
}SAVE_FLAG_T;
extern SAVE_FLAG_T gSaveFlag; //存储公共标记

/*******************************************************************************
* 函数声明
*******************************************************************************/
extern void InitEventPara(void);
extern int SaveTopoEventOccur();
extern int SaveTopoResultRecord();
extern const Map645ToOop_NEW* FindDetailInfoByOad(uint32 OAD);
extern int Find07AcDataByOad(uint8 *pDesbuffer, uint16 *nDesLen, const Map645ToOop_NEW *pOadInfo);
extern uint16 Save07AcIntoDataCenter(uint8 type);
extern void InitEventPara(void);
extern int SaveMagneticEventOccur();
extern void sta_volt_qualifyrate_calc(void);
extern void sta_frozen_proc();
extern void sta_frozen_lasttime_reinit(void);

#ifdef __cplusplus
}
#endif

#endif // __AC_METER_SAVE_H__

