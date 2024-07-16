/*
*********************************************************************
* @file    AutoTask.c
* @brief： 统计模块实现文件
* @author：贺宁
* @date：   2019.11.11
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#ifndef __STAT_LIB_H__
#define __STAT_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

/*--计量点类型定义--*/
#define MTR_485       1   /*-485表-*/
#define MTR_SIM       2   /*-模拟表-*/
#define MTR_PULSE     3   /*-脉冲-*/
#define MTR_VIR       4   /*-虚拟-*/
#define MTR_AC        5   /*-交采-*/
#define MTR_PLC       6   /*-载波-*/
#define MTR_PLCMUI    7   /*-载波多功能-*/

#define UPUP_EN          (1)       //电压、电流、视在功率越上上限（下下限）时，是否认为越上限（下限）时间
#define SMALL_EN         (1)       //在底数比较小的时候，是否应该将该底数记入电量中 1 不计  0 计入
#define TOTAL_EN_CHECK   (0)       //对总数据进行校验，查看是不是等于分费率相加

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

typedef struct tag_STA_MTR_INFO
{
    uint8     type;       /**< MTR_AC... */
    uint16    nIndex;     /**< 485表的配置序号   */
    OOP_TSA_T tsa;        /**< 485表的地址，用于计算MAC   */
    OOP_OAD_U pulseOAD;   /**< 脉冲表的OAD    */    
    uint8     pwrType;    /**< 接线方式... */
}STA_MTR_INFO_T;

//私有文件存储结构
/*
        ┌───────────────────────┐
        │上次实时电量（160字节）          │
        ├───────────────────────┤
        │上次分钟电量（160字节）          │
        ├───────────────────────┤
        │分总加电量（80字节）            │
        ├───────────────────────┤
        │日总加电量（80字节）            │
        ├───────────────────────┤
        │月总加电量（80字节）            │
        └───────────────────────┘
*/

//每个分钟电量单元大小（总，尖，峰，平，谷 5*8 sizeof(uint64)）
#define STA_MIN_ENERGY_UNIT_SIZE 40 

//一个SEG包含正向有功，反向有功，正向无功，反向无功4个单元
#define STA_MIN_ENERGY_SEG_SIZE  4*STA_MIN_ENERGY_UNIT_SIZE

//分钟电量基数段
#define STA_MIN_ENERGY_BASE_SEG  0

//分钟电量数据段
#define STA_MIN_ENERGY_DATA_SEG  STA_MIN_ENERGY_BASE_SEG+STA_MIN_ENERGY_SEG_SIZE 

//段内各单元偏移量
#define STA_POS_ACT_ENERGY_OFFSET  0*STA_MIN_ENERGY_UNIT_SIZE  /**< 正向有功电能 */
#define STA_NEG_ACT_ENERGY_OFFSET  1*STA_MIN_ENERGY_UNIT_SIZE  /**< 反向有功电能 */        
#define STA_POS_RCT_ENERGY_OFFSET  2*STA_MIN_ENERGY_UNIT_SIZE  /**< 正向无功电能 */   
#define STA_NEG_RCT_ENERGY_OFFSET  3*STA_MIN_ENERGY_UNIT_SIZE  /**< 反向无功电能 */ 

//新加校验接口
//每个分钟电量单元大小（总，尖，峰，平，谷 5*8 sizeof(uint64)+1字节校验）
#define STA_MINCS_ENERGY_UNIT_SIZE 41 

//一个SEG包含正向有功，反向有功，正向无功，反向无功4个单元
#define STA_MINCS_ENERGY_SEG_SIZE  4*STA_MINCS_ENERGY_UNIT_SIZE

//分钟电量基数段
#define STA_MINCS_ENERGY_BASE_SEG  0

//分钟电量数据段
#define STA_MINCS_ENERGY_DATA_SEG  STA_MINCS_ENERGY_BASE_SEG+STA_MINCS_ENERGY_SEG_SIZE 

//段内各单元偏移量
#define STA_POS_ACTCS_ENERGY_OFFSET  0*STA_MINCS_ENERGY_UNIT_SIZE  /**< 正向有功电能 */
#define STA_NEG_ACTCS_ENERGY_OFFSET  1*STA_MINCS_ENERGY_UNIT_SIZE  /**< 反向有功电能 */        
#define STA_POS_RCTCS_ENERGY_OFFSET  2*STA_MINCS_ENERGY_UNIT_SIZE  /**< 正向无功电能 */   
#define STA_NEG_RCTCS_ENERGY_OFFSET  3*STA_MINCS_ENERGY_UNIT_SIZE  /**< 反向无功电能 */ 

//分总加电量数据段
#define STA_TGMINCS_ENERGY_DATA_SEG  STA_MINCS_ENERGY_DATA_SEG+STA_MINCS_ENERGY_SEG_SIZE

#define STA_ACTCS_ENERGY_OFFSET  0*STA_MINCS_ENERGY_UNIT_SIZE  /**< 有功电能 */
#define STA_RCTCS_ENERGY_OFFSET  1*STA_MINCS_ENERGY_UNIT_SIZE  /**< 无功电能 */

#define STA_SLIPPERIOD_MAX (255)  //最大滑差周期（因为滑差周期是8位无符号数）

/** @brief 滑差功率缓存 */
typedef struct tag_STA_SLIPPOWER_FIFO
{
    uint32 nCount;                       //成员数量 
    int64 slipAct[STA_SLIPPERIOD_MAX+1]; //有功功率队列 
    int64 slipRct[STA_SLIPPERIOD_MAX+1]; //无功功率队列    
}STA_SLIPPOWER_FIFO_T;

/** @brief 数据删除*/
typedef struct tag_STA_TGDEL
{
    pthread_mutex_t flock;                      //锁 多线程访问
    uint8           delVaild;                   //有效标识
}STA_TGDEL_T;

/** @brief 授权状态   */
typedef enum tag_STA_PORT_AUTHORIZE
{
    STA_PORT_AUTH_PERMANENT = 0,   //永久授权
    STA_PORT_AUTH_PROVISIONAL = 1, //临时授权
    STA_PORT_AUTH_FORBIDDEN = 2,   //关闭授权
}STA_PORT_AUTHORIZE_E;

typedef struct tag_STA_PORT_AUTHORIZE_INFO
{
    STA_PORT_AUTHORIZE_E status; //授权状态
    BOOLEAN isAuthChg;   //授权状态变化           1：是 0：否
    BOOLEAN isAuthedcmd; //安全认证交互命令 1：是 0：否
    BOOLEAN isSuccLocal; //本地安全认证通过         1：是 0：否
    uint8 authTime;      //授权时长
}STA_PORT_AUTHORIZE_INFO;

#define AUTH_STATUS_GET(time) ((time==255)?STA_PORT_AUTH_PERMANENT:(time==0)?STA_PORT_AUTH_FORBIDDEN:STA_PORT_AUTH_PROVISIONAL)

#define abs_int64(vari)  ((vari>=(int64)0) ? (vari):-(vari))  /*-int64型数据的绝对值-*/

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
    MT_REGION_DAY  = 0x219A,
    MT_REGION_MON  = 0x219B,
}MT_REGION_OI_E;

/** @brief 累加平均统计OI枚举   */
typedef enum tag_MT_TOTALAVG_OI
{
    MT_TOTALAVG_DAY  = 0x2198,
    MT_TOTALAVG_MON  = 0x2199,
}MT_TOTALAVG_OI_E;

/** @brief 极值统计OI枚举   */
typedef enum tag_MT_EXTREME_OI
{
    MT_EXTREME_DAY  = 0x2196,
    MT_EXTREME_MON  = 0x2197,
}MT_EXTREME_OI_E;

/** @brief 合格率统计OI枚举   */
typedef enum tag_MT_QR_OI
{
    MT_QR_DAY  = 0x2192,
    MT_QR_MON  = 0x2193,
}MT_QR_OI_E;

/**@brief越限统计OI枚举*/
typedef enum tag_MT_POWERCUT_OI
{
    MT_POWERCUT_DAY  = 0x2194,
    MT_POWERCUT_MON  = 0x2195,
}MT_POWERCUT_OI_E;

/** @brief 合格状态枚举   */
typedef enum tag_STA_QSTATE
{
    MT_QSTATE_QR  = 0, //合格   
    MT_QSTATE_UP  = 1, //越上限
    MT_QSTATE_DW  = 2  //越下限
}STA_QSTATE_E;

/** @brief 合格率统计结果 */
typedef struct tag_STA_QR_RES
{
    uint32       mtTime;  //监测时间 
    uint32       qrTime;  //合格时间 
    uint32       upTime;  //超上限时间
    uint32       dwTime;  //超下限时间
    uint32       upupTime;//超上上限时间
    uint32       dwdwTime;//超下下限时间
    uint32       tmpupupTime;//临时超上上限时间
    uint32       tmpdwdwTime;//临时超下下限时间  
    uint32       tmpupTime;//临时超上限时间
    uint32       tmpdwTime;//临时超下限时间  
    uint16       qrRate;  //合格率
    uint16       upRate;  //超上限率
    uint16       dwRate;  //超下限率
    STA_QSTATE_E mtState; //当前状态
}OOP_QR_RESULT_T;

/** @brief 合格率统计参数 */
typedef struct tag_STA_QR_PARA
{
    uint32          upupLimit;        /* 上上限越限值*/
    uint32          upLimit;          /* 上限越限值*/   
    uint32          upupcontime;      /* 越上上限持续时间*/
    uint32          upcontime;        /* 越上限持续时间*/
    uint32          upRecov;          /* 上限恢复值*/
    uint32          dwRecov;          /* 下限恢复值*/
    uint32          dwLimit;          /* 下限越限值*/
    uint32          dwdwLimit;        /* 下下限越限值*/    
    uint32          dwcontime;        /* 越下限持续时间*/
    uint32          dwdwcontime;      /* 越下下限持续时间*/
    uint32          spantime;         /* 此轮消耗时间*/
}STA_QR_PARA_T;

typedef int (*STA_CALC_ADAPT)(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, void *object);
typedef int (*STA_DATA_ADAPT)(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, OOP_OAD_U oad, uint32 *units, OOP_DIGIT_T *digit);

/** @brief 合格率统计对象 */
typedef struct tag_STA_QR_OBJ
{
    OOP_OAD_U       dstOAD;          /* 主OAD(据此决定period的单位) */
    OOP_OAD_U       srcOAD;           /* 子OAD(统计对象)*/
    uint32          periCnt;          /* 周期计数(经历的周期数)*/
    uint32          freqCnt;          /* 间隔计数(经历的间隔数)*/      
    OOP_TI_T        period;           /* 统计周期数 */
    OOP_TI_T        freq;             /* 采样点间隔 */
    STA_QR_PARA_T   param;            /* 统计参数 */    
    OOP_DIGIT_T    *srcData;          /* 源数据指针*/
    uint32          srcUnits;         /* 源数据单元数*/
    STA_CALC_ADAPT  paraAdapt;        /* 参数适配函数*/
    STA_DATA_ADAPT  dataAdapt;        /* 数据适配函数*/
    STA_CALC_ADAPT  calcAdapt;        /* 计算适配函数*/    
    uint32          srcValid;         /* 源数据有效性*/ 
}STA_QR_OBJ_T;

void sta_qr_entry(DB_CLIENT clientid);
int sta_qr_comm_calc(DB_CLIENT clientid, STA_MTR_INFO_T *mtrInfo, void *pObject);

void sta_volt_qualifyrate_calc(void);
void sta_curr_imbalance_calc(void);
void sta_volt_imbalance_calc(void);
void sta_mpoint_entry(void);
void sta_pulse_energy_calc(void);
void sta_tgsets_data_calc(void);

void sta_tmn_power_acctime(void);
void sta_tmn_reset_acctimes(void);
void sta_tmn_comm_traffic(void);
void sta_tmn_port_authorize_tmcnt(void);
void sta_tmn_port_authorize_status(uint8 tmAuth, STA_PORT_AUTHORIZE_INFO *status);
void sta_tmn_port_authorize_operate(void);
int sta_ssh_service_authorize_operate(STA_PORT_AUTHORIZE_E status);
void sta_past_data_clear(void);
void sta_tg_data_clear(DB_CLIENT fd, OOP_OAD_U tgOAD);

void sta_tg_min_elec_init();
int sta_tg_min_acte_read(OI tgOI, int64 *acteTotal, uint32 *storeNo);

extern int appmain_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen);
int sta_record_read_new(DB_CLIENT clientid, RECORD_OAD_T *recordOAD, uint32 *storeTm, void *outData, uint32 outLen);


#ifdef __cplusplus
}
#endif

#endif  /* __STAT_LIB_H__ */

