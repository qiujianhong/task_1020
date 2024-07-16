#ifndef __MONIT_STAT_H__
#define __MONIT_STAT_H__

/** @brief 越限判断阈值 */
#define MT_VOLT_UP_LIMIT (2354) //电压上限越限值 1.07*220，扩大10倍
#define MT_VOLT_UP_RECOV (2354) //电压上限恢复值 1.07*220（上限恢复值一定要小于上限），扩大10倍

#define MT_VOLT_DW_LIMIT (2046) //电压下限越限值 0.93*220，扩大10倍
#define MT_VOLT_DW_RECOV (2046) //电压下限恢复值 0.93*220（下限恢复值一定要大于下限），扩大10倍

#define MT_VOLT_DIST_LIMIT (500) //电压畸变率越限值%，百分数扩大100倍，数值扩大10000倍
#define MT_VOLT_DIST_RECOV (470) //电压畸变率恢复值%，百分数扩大100倍，数值扩大10000倍

#define MT_VOLT_UNB_LIMIT (4) //电压不平衡度越限值%，百分数扩大100倍，数值扩大10000倍
#define MT_VOLT_UNB_RECOV (3.8) //电压不平衡度恢复值%，百分数扩大100倍，数值扩大10000倍

#define MT_CURR_UNB_LIMIT (30) //电流不平衡度越限值%，百分数扩大100倍，数值扩大10000倍
#define MT_CURR_UNB_RECOV (25) //电流不平衡度恢复值%，百分数扩大100倍，数值扩大10000倍

#define MT_PWRF_UP_LIMIT (5100) //电网频率上限越限值，扩大100倍
#define MT_PWRF_UP_RECOV (5100) //电网频率上限恢复值，扩大100倍

#define MT_PWRF_DW_LIMIT (4900) //电网频率下限越限值，扩大100倍
#define MT_PWRF_DW_RECOV (4900) //电网频率下限恢复值，扩大100倍

#define MT_FWR_FAC1_LIMIT (930) //功率因素1段，扩大1000倍
#define MT_FWR_FAC2_LIMIT (980) //功率因素2段，扩大1000倍

#define MT_INVALID_LIMIT (0xffffffff) //无效的限值

#define MT_ATTR_INDEX(nIndex) ((nIndex == 0)?nIndex:(nIndex-1))

/** @brief 冻结对象OI枚举   */
typedef enum tag_MT_FROZEN_OI
{
    MT_FROZEN_TRAN = 0x5000,
    MT_FROZEN_SEC = 0x5001,
    MT_FROZEN_MIN = 0x5002,
    MT_FROZEN_HOUR = 0x5003,
    MT_FROZEN_DAY = 0x5004,
    MT_FROZEN_SETTLING_DAY = 0x5005,
    MT_FROZEN_MON = 0x5006,
    MT_FROZEN_YEAR = 0x5007
}MT_FROZEN_OI_E;

/** @brief 区间统计OI枚举   */
typedef enum tag_MT_REGION_OI
{
    MT_REGION_MIN  = 0x2100,
    MT_REGION_HOUR = 0x2101,
    MT_REGION_DAY  = 0x2102,
    MT_REGION_MON  = 0x2103,
    MT_REGION_YEAR = 0x2104
}MT_REGION_OI_E;

/** @brief 累加平均统计OI枚举   */
typedef enum tag_MT_TOTALAVG_OI
{
    MT_TOTALAVG_MIN  = 0x2110,
    MT_TOTALAVG_HOUR = 0x2111,
    MT_TOTALAVG_DAY  = 0x2112,
    MT_TOTALAVG_MON  = 0x2113,
    MT_TOTALAVG_YEAR = 0x2114
}MT_TOTALAVG_OI_E;

/** @brief 极值统计OI枚举   */
typedef enum tag_MT_EXTREME_OI
{
    MT_EXTREME_MIN  = 0x2120,
    MT_EXTREME_HOUR = 0x2121,
    MT_EXTREME_DAY  = 0x2122,
    MT_EXTREME_MON  = 0x2123,
    MT_EXTREME_YEAR = 0x2124
}MT_EXTREME_OI_E;

/** @brief 合格率统计OI枚举   */
typedef enum tag_MT_QR_OI
{
    MT_QR_DAY  = 0x2192,
    MT_QR_MON  = 0x2193,
}MT_QR_OI_E;

/**@brief停电监测OI枚举*/
typedef enum tag_MT_POWERCUT_OI
{
    MT_POWERCUT_DAY  = 0x2194,
    MT_POWERCUT_MON  = 0x2195,
}MT_POWERCUT_OI_E;

typedef int (*MT_ADAPT)(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);

/** @brief 极值统计对象 */
typedef struct tag_MT_EXTREME_OBJ
{
    OOP_OAD_U       mainOAD;          /* 主OAD(据此决定period的单位) */
    OOP_OAD_U       subOAD;           /* 子OAD(统计对象)*/
    uint32          periCnt;          /* 周期计数(经历的周期数)*/
    uint32          freqCnt;          /* 间隔计数(经历的间隔数)*/      
    uint32          period;           /* 统计周期数 */
    OOP_TI_T        freq;             /* 采样点间隔 */
    MT_ADAPT        mtAdapt;          /* 数据处理函数*/
    OOP_DIGIT_T    *srcData;          /* 源数据存储*/
    uint8           nIndex;           /* 块数据的当前索引*/
}MT_EXTREME_OBJ_T;

/** @brief 合格率统计对象 */
typedef struct tag_MT_QR_OBJ
{
    OOP_OAD_U       mainOAD;          /* 主OAD(据此决定period的单位) */
    OOP_OAD_U       subOAD;           /* 子OAD(统计对象)*/
    uint32          periCnt;          /* 周期计数(经历的周期数)*/
    uint32          freqCnt;          /* 间隔计数(经历的间隔数)*/      
    uint32          period;           /* 统计周期数 */
    OOP_TI_T        freq;             /* 采样点间隔 */
    uint32          upLimit;          /* 上限越限值*/    
    uint32          upRecov;          /* 上限恢复值*/
    uint32          dwLimit;          /* 下限越限值*/
    uint32          dwRecov;          /* 下限恢复值*/
    MT_ADAPT        mtAdapt;          /* 数据处理函数*/
    OOP_DIGIT_T    *srcData;          /* 源数据指针*/
    uint8           nIndex;           /* 块数据的当前索引*/
}MT_QR_OBJ_T;

/** @brief 合格状态枚举   */
typedef enum tag_MT_QSTATE
{
    MT_QSTATE_Q  = 0, //合格   
    MT_QSTATE_U  = 1, //越上限
    MT_QSTATE_D  = 2  //越下限
}MT_QSTATE_E;

/** @brief 合格率统计结果 */
typedef struct tag_MT_QR_RES
{
    uint32       mtTime;  //监测时间 
    uint32       qrTime;  //合格时间 
    uint32       upTime;  //超上限时间
    uint32       dwTime;  //超下限时间  
    uint16       qrRate;  //合格率
    uint16       upRate;  //超上限率
    uint16       dwRate;  //超下限率
    float64      mtTotal; //累加和
    float64      mtAvg;   //平均值
    MT_QSTATE_E  mtState; //当前状态
}MT_QR_RES_T;

/** @brief 区间统计对象 */
typedef struct tag_MT_REGION_OBJ
{
    OOP_OAD_U       mainOAD;              /* 主OAD(据此决定period的单位) */
    OOP_OAD_U       subOAD;               /* 子OAD(统计对象)*/
    uint32          periCnt;              /* 周期计数(经历的周期数)*/
    uint32          freqCnt;              /* 间隔计数(经历的间隔数)*/      
    uint32          period;               /* 统计周期数 */
    OOP_TI_T        freq;                 /* 采样点间隔 */
    uint32          nNum;                 /* 定值个数 */
    OOP_DIGIT_T     parm[OOP_MAX_REGION]; /* 区间定值 */   
    MT_ADAPT        mtAdapt;              /* 数据处理函数*/
    OOP_DIGIT_T    *srcData;              /* 源数据指针*/
    uint8           nIndex;               /* 块数据的当前索引*/
}MT_REGION_OBJ_T;

/** @brief 冻结对象 */
typedef struct tag_MT_FROZEN_OBJ
{
    OOP_OAD_U       mainOAD;          /* 主OAD(据此决定period的单位) */
    OOP_OAD_U       subOAD;           /* 子OAD(统计对象)*/
    uint32          periCnt;          /* 周期计数(经历的周期数)*/
    uint32          period;           /* 统计周期数 */
}MT_FROZEN_OBJ_T;

/** @brief 停电监测对象 */
typedef struct tag_MT_POWERCUT_OBJ
{
    OOP_OAD_U       mainOAD;
    uint32          periCnt;
    uint32          freqCnt;
    uint32          period;
    OOP_TI_T        freq;
}MT_POWERCUT_OBJ_T;

/** @brief 停电监测统计*/
typedef struct tag_MT_POWERCUT_RES
{
    OOP_OAD_U       obj;
    BOOLEAN         pwctFLag;
    uint32          countTime;
    time_t          tmstart;
    time_t          tmstop;    
}MT_POWERCUT_RES_T;


int mt_volt_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_curr_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_zerocurr_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_pwrfreq_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_imbalance_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_distortion_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);
int mt_pwrfact_adapt(DB_CLIENT clientid, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);

#endif
