/*
*********************************************************************
* @file    AutoTask.c
* @brief： 统计模块实现文件
* @author：贺宁
* @date：   2019.11.11
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#ifndef __STAT_MAIN_H__
#define __STAT_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tag_STA_PRAM_UPDATE
{
    pthread_mutex_t mutex;
    uint8 frzInitFlag;
    uint8 regionInitFlag;
    uint8 avgInitFlag;
    uint8 extremeInitFlag; 
    uint8 timeUpdate; 
}STA_PARAM_UPDATE;

/*-各类数据统计模块的使能-*/
#define STAT_485_EN     1   /*-485统计模块使能-*/
#define STAT_AC_EN      1   /*-交采统计模块使能-*/
#define STAT_PULSE_EN   1   /*-脉冲统计模块使能-*/
#define STAT_VIR_EN     1   /*-总加统计模块使能-*/
#define STAT_TMN_EN     1   /*-终端统计模块使能-*/

/* 冻结最大关联对象个数 是否换 OOP_MAX_FROZENOBJ? */
#define  F_OAD_MAX_NUM   (50)

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

#define STA_REGION_OBJNUMS    (5)   /*区间统计类型（分，时，日，月，年）5种*/
#define STA_TOTALAVG_OBJNUMS  (5)   /*累加平均统计类型（分，时，日，月，年）5种*/
#define STA_EXTREME_OBJNUMS   (5)   /*极值统计类型（分，时，日，月，年）5种*/

/** @brief 区间统计OI枚举   */
typedef enum tag_STA_REGION_OI
{
    STA_REGION_MIN  = 0x2100,
    STA_REGION_HOUR = 0x2101,
    STA_REGION_DAY  = 0x2102,
    STA_REGION_MON  = 0x2103,
    STA_REGION_YEAR = 0x2104
}STA_REGION_OI_E;

/** @brief 累加平均统计OI枚举   */
typedef enum tag_STA_TOTALAVG_OI
{
    STA_TOTALAVG_MIN  = 0x2110,
    STA_TOTALAVG_HOUR = 0x2111,
    STA_TOTALAVG_DAY  = 0x2112,
    STA_TOTALAVG_MON  = 0x2113,
    STA_TOTALAVG_YEAR = 0x2114
}STA_TOTALAVG_OI_E;

/** @brief 极值统计OI枚举   */
typedef enum tag_STA_EXTREME_OI
{
    STA_EXTREME_MIN  = 0x2120,
    STA_EXTREME_HOUR = 0x2121,
    STA_EXTREME_DAY  = 0x2122,
    STA_EXTREME_MON  = 0x2123,
    STA_EXTREME_YEAR = 0x2124
}STA_EXTREME_OI_E;

/** @brief 区间统计关联对象   */
typedef struct tag_STA_REGIONOBJ
{
    OOP_OAD_U       obj;
    uint32          nNum;
    OOP_DOMAIN_T    parm[OOP_MAX_REGION];
}STA_REGIONOBJ_T;

/** @brief 区间统计关联对象集合 */
typedef struct tag_STA_REGIONS
{
    time_t          lastTime;              /* 上一次统计时间 */
    uint32          count;                 /* 统计次数 */   
    uint32          freqCnt;               /* 间隔计数（按间隔的运行次数） */      
    uint32          nNum;                  /* 关联对象数目 */
    uint32          period;                /* 统计周期 */
    OOP_TI_T        freq;                  /* 采样点间隔 */
    STA_REGIONOBJ_T objs[OOP_REGION_OBJS]; /* 关联对象 */
}STA_REGIONS_T;

/** @brief 区间统计参数集合*/
typedef struct tag_STA_REGIONS_PARA
{
    OOP_OAD_U oad;                         /*统计类型（分，时，日，月，年）*/
    uint32 blockNum;                       /*子节点个数，数组有效数目个数*/
    STA_REGIONS_T para[OOP_REGION_OBJS];
}STA_REGIONS_PARA_T;


/** @brief 累加平均统计关联对象集合 */
typedef struct tag_STA_TOTALAVGS
{
    time_t          lastTime;         /* 上一次统计时间 */
    uint32          count;            /* 周期计数（计数达到period清统计结果） */
    uint32          freqCnt;          /* 间隔计数（按间隔的运行次数） */     
    uint32          nNum;
    uint32          period;           /* 统计周期 */
    OOP_TI_T        freq;             /* 采样点间隔 */
    OOP_OAD_U objs[OOP_COGNATE_OBJS]; /* 关联对象OAD */
}STA_TOTALAVGS_T;

/** @brief 累加平均统计参数集合*/
typedef struct tag_STA_TOTALAVGS_PARA
{
    OOP_OAD_U oad;                   /*统计类型（分，时，日，月，年）*/
    uint32 blockNum;                 /*子节点个数，数组有效数目个数*/    
    STA_TOTALAVGS_T para[OOP_COGNATE_OBJS];
}STA_TOTALAVGS_PARA_T;

/** @brief 极值统计关联对象集合 */
typedef struct tag_STA_EXTREMES
{
    time_t          lastTime;         /* 上一次统计时间 */
    uint32          count;            /* 统计次数 */
    uint32          freqCnt;          /* 间隔计数（按间隔的运行次数） */      
    uint32          nNum;             /* 关联对象数目 */
    uint32          period;           /* 统计周期 */
    OOP_TI_T        freq;             /* 采样点间隔 */
    OOP_OAD_U objs[OOP_COGNATE_OBJS]; /* 关联对象OAD */
}STA_EXTREMES_T;

/** @brief 极值统计参数集合*/
typedef struct tag_STA_EXTREMES_PARA
{
    OOP_OAD_U oad;                   /*统计类型（分，时，日，月，年）*/
    uint32 blockNum;                 /*子节点个数，数组有效数目个数*/    
    STA_EXTREMES_T para[OOP_COGNATE_OBJS];
}STA_EXTREMES_PARA_T;

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

typedef struct
{
    OOP_OAD_U   sOad;              // OAD
    uint16 frozenPeriod;      // 冻结周期                   
    uint16 storeDepth;        // 存储深度
    
}FrzOadInfo_t;

typedef struct 
{  
    uint16 PhaAVolt;           // A相电压 
    uint16 PhaBVolt;           // B相电压
    uint16 PhaCVolt;           // C相电压
}STA_ACDATA_T;

/* G-功能模块; M-模拟量采集模块 X-功能类型无补充属性 
   见《能源控制器功能模组技术规范》1 模块分类和类型标识代码*/
#define MOD_PT100_TYPE_PRFIX       "GMX"   

/*功能模块的设备描述符前缀 描述符=uart_acm[模组序号/槽位号]_[虚拟通道号]
  由HAL提供的LIB定义，参见framework/include/hal/uart.h*/
#define MOD_DEV_DESC_PRFIX        "uart_acm"

//最大槽位数
#define MOD_SLOT_MAX        5

/** @brief 直流模拟量采集状态   */
typedef enum tag_STA_DC_ANALOG
{
    STA_DC_INIT  = 0, //初始态
    STA_DC_SEND =  1, //发送态
    STA_DC_GETM  = 2, //获取态
    STA_DC_RECV =  3, //接收态
}STA_DC_ANALOG_E;

typedef struct 
{  
    int value;
    OOP_SCALER_UNIT_T scaler;
}STA_DCANALOG_T;

typedef struct 
{  
    uint32 nNum;
    STA_DCANALOG_T data[OOP_MAX_DCANALOG];
}STA_DCANALOGS_T;

void sta_frozen_lasttime_reinit(void);
void sta_region_lasttime_reinit(void);
void sta_totalavg_lasttime_reinit(void);
void sta_extreme_lasttime_reinit(void);
void sta_last_time_reinit(void);
void sta_time_update_fset(BOOLEAN isUpdate);

#ifdef __cplusplus
}
#endif

#endif /* __STAT_MAIN_H__ */

