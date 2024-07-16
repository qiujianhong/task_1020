/*
*********************************************************************
* @file    AutoTask.h
* @brief： 任务上报模块头文件
* @author：贺宁
* @date：   2019.11.11
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#ifndef __AUTO_TASK_H__
#define __AUTO_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_ID_NUM         256       //-每个任务中支持的最大ID数量-
#define AUTOTASK_MAX_NUM    64     //终端支持的每种类型中最大任务总数

#define TMN_TIME_TICK_BEGIN     0
#define TMN_TIME_TICK_END       24

/** @brief 上报类型 */
typedef enum
{
    RPT_TYPE_TASK   = 0,   /* 任务上报 */
    RPT_TYPE_EVENT  = 1,   /* 事件上报 */
    RPT_TYPE_MTREVT = 2,   /* 电能表事件上报 */    
    RPT_TYPE_RETRY =  3,   /* 补报 */     
    RPT_TYPE_UNDEF =  4,   /* 未定义 */      
}RPT_TYPE_E;

/** @brief 主站响应状态 */
typedef enum
{
    RPT_RES_INITIAL = 0, /* 初始化 */
    RPT_RES_ALREADY = 1, /* 已经响应 */
    RPT_RES_WAITING = 2, /* 等待响应 */
    RPT_RES_NONEED =  3  /* 无需响应 */
}RPT_RES_STATUS_E;

/** @brief 上报配置 */
typedef struct 
{
    RPT_TYPE_E       type;         /* 上报类型 */
    RPT_RES_STATUS_E *resStatus;   /* 上报响应状态 */
    const char       *filename;    /* 记录文件名*/
    uint16           maxRecord;    /* 最大记录数*/
    BOOLEAN             retryTag;     /* 重发标记*/  
    uint8  *piid;                  /* piid*/
    time_t  lstTime;               /* 上次时间*/
    time_t  curTime;               /* 本次时间*/
}RPT_CFG_T;

/** @brief 重试配置 */
typedef struct 
{
    uint16       timeOut;      /* 超时时间 */
    uint8        retryCnt;     /* 重试次数*/
}RPT_RETRY_T;


/** @brief 响应处理*/
typedef enum
{
    RPT_RES_PORC_NEXT = 0,     /* 发下一条 */
    RPT_RES_PORC_RETRY = 1,    /* 重发本条 */
    RPT_RES_PORC_WAITING =  2  /* 等待超时 */
}RPT_RES_PROC_E;

typedef struct 
{
    uint8  taskIndex;  //上报用的任务内部ID
    uint8 *pOutBuf;   //上报内容缓冲区
    uint16 bufLen;    //上报内容缓冲区长度
    uint16 msgLen;    //上报内容实际长度
}RPT_RESULT_T;

/*判断是否针对本次上报的响应*/
#define RPT_TASK_IS_MY_RES(piid)  ((piid&0x3f) == (TaskGLBpiid&0x3f))

#define RPT_EVENT_IS_MY_RES(piid)  ((piid&0x3f) == (EventGLBpiid&0x3f))


/** @brief 任务数据结构*/
typedef struct                      
{
    uint8 bConflag;                     //任务确认标志    0 不需要确认   1 确认
    long  timeOut;                      //超时时间
    uint8 retryTime;                    //重发次数
    uint16 bLen;                        //-记录长度-
    uint8  piid;                        //-piid序号,用来判断确认帧-
    uint8  taskIndex;                      //任务号
    uint8 bData[8192];  //-记录数据内容-

}AUTO_DATA;
/*
        NEW_TASKDATAFILE 文件存储格式：
        ┌───────────────────────┐
        │  记录总数（2字节）            │
        ├───────────────────────┤
        │ 已上报条数（2字节）            │
        ├───────────────────────┤
        │       任务数据1           │
        ├───────────────────────┤
        │       任务数据2           │
        ├───────────────────────┤
        │         ......        │
        ├───────────────────────┤
        │       任务数据N           │
        └───────────────────────┘
*/
#define RPT_RECORD_FILE     "rpt_record_file"            //任务数据的存储文件
#define RPT_RECORD_MAX       1200                        //任务数据存放的最多条数
#define RPT_RECORD_CNT_OFFSET    0                       //记录总数的偏移位置
#define RPT_REPORT_CNT_OFFSET    2                       //已上报总数的偏移位置
#define RPT_TASK_MIN_TIMEOUT     5                       //任务上报的最小时间

//在最大值约束下增加值
#define RPT_ADD_VALUE_CLEAR(count, mcount, value) ((count) + (value) >= (mcount) ? 0 : (count) + (value))
//数据索引
#define RPT_RECORD_DATA_OFFSET(index)   (4 + (index) * sizeof(AUTO_DATA))

//电能表事件上报
#define RPT_MTREVT_IS_MY_RES(piid)  ((piid&0x3f) == (EventGLBpiid_met&0x3f))

#define RPT_MTREVT_REC_FILE     "rpt_mtrevt_rec_file"   

#define RPT_MTREVT_REC_MAX       200         //事件上报最大记录数量

#define RPT_MTREVT_RECBUF_MAX    2048        //事件上报数据最大长度
        
typedef struct
{
    uint32              dataLen;                          //数据长度
    uint8               rptType;                          //上报类型
    uint8               pdataBuf[RPT_MTREVT_RECBUF_MAX];  //上报数据
}RPT_MTREVT_INFO;

#define RPT_MTREVT_REC_OFFSET(index)   (4 + (index) * sizeof(RPT_MTREVT_INFO))

//终端事件上报
#define EVENT_RECORD_MAX_LEN    1024        //事件上报数据最大长度

#define EVENT_RECORD_FILE     "event_record_file"  //事件主动上报的存储文件

#define EVENT_RECORD_MAX_NUM    200         //事件上报最大记录数量

#define RPT_EVENT_RECORD_OFFSET(index)   (4 + (index) * sizeof(EVENT_REPORT_INFO))

/** @brief 上报方案信息 */
typedef struct tag_RPT_PLAN_CFG
{
    OOP_OAD_U  serviceOAD;
    uint16  apduLength;//传入的apdu长度
    uint8  apduValue[500];//传入的apdu
    OOP_PLAN_REPORT_T plan;
    BOOLEAN   isNeedUpdateTIme;//曲线时间是否需要人为修改时间
    uint16 TimeOffset;//需要修改时间的偏移
    CHANNEL_CACHE_T chann; //召测通道信息
    BOOLEAN   isModify;    //是否被修改
}RPT_PLAN_CFG_T;

typedef enum
{
    TASK_CFG_UNINIT,              //未初始化
    TASK_CFG_MODIFY,              //修改
    TASK_CFG_MODIFY_DELAY,        //修改延时
    TASK_CFG_INITED,              //已初始化
    TASK_CFG_REBUILD,             //重建采样文件
    TASK_CFG_PATCH,     //创建补抄文件
} TASK_CFG_STATE;//方案档案状态

typedef struct _PRO698_TASK_CFG
{
    uint8   TaskId;                 //任务id
    uint16  RunCycle;               //执行频率数值
    uint8   RunCycleUnit;           //执行频率单位
    ePlanType SchemeType;           //采集方案类型
    uint8   SchemeId;               //方案编号
    char    RunBeginTime[7];        //任务执行开始时间
    char    RunEndTime[7];          //任务执行结束时间
    uint16  DelayTime;              //延时时间数值
    uint8   DelayTimeUnit;          //延时时间单位
    uint8   Priority;               //任务优先级
    uint8   TaskState;              //状态 正常(1),停用(2)
    uint16  PreRunScript;           //任务开始前脚本id
    uint16  OverRunScript;          //任务结束后脚本id
    uint8   TimePeriordType;        //时段类型
    uint8   TimePeriordNum;         //时段个数
    uint8   Periordhhmm[192];       //48*4字节,任务运行时段hh:mm~hh:mm
    char    ThisTurnBeginTime[7];    //本轮执行开始时间
    char    ThisTurnEndTime[7];     //本轮执行结束时间
    char    NextTurnBeginTime[7];   //下轮执行开始时间
    char    LastTurnBeginTime[7];   //上一轮执行开始时间
    BOOL    thisTurnReportFlag;     //本轮上报标志
    BOOL    NeedToRun;              //本轮上报数据是否执行
    BOOL    isNowCb;                //是否立即抄表--按键抄表用
    time_t   lastEndTime;           //用成功时间招数据的结束时间
#if defined(AREA_SHANDONG) || defined(AREA_ANHUI) 
    uint16 normalTaskId;            //对应的普通采集任务id
    uint8  checkTaskFinish;          //在本轮开始到延时时间之间 需要检查任务是否抄完 0代表不需要 1代表需要 2代表已经查过了
    time_t lastCheckTime;           //上次发送查询60340200的时间
#endif
}OOP_TASK_CFG;

typedef struct
{
    pthread_rwlock_t    rwLock;                  //读写锁 多线程访问
    TASK_CFG_STATE      TaskCfgState;         //任务档案状态
    int                 ProCfgModifyDelaySec;  //方案配置修改延时
    time_t              FunLastRunTime;        //方案上次修改时间
    uint8               TaskNum;               //任务总个数
    OOP_TASK_CFG        OopTaskCfg[256];       //任务具体参数
}OOP_TASK_CFG_LIST;

#define RPT_BUF_MAX  (2048)

/** @brief 上报队列 */
typedef struct 
{
    uint16   timeOut;      /* 超时时间 */
    uint32   taskID;       /* 上报任务号*/
    uint16   dataLen;      /* 上报报文长度 */ 
    uint8    planID;       /* 上报方案号*/
    uint8    priority;     /* 上报优先级*/
    uint8    retryCnt;     /* 重试次数*/    
    uint16   mergeNum;     /* 合并条数*/
    OOP_DATETIME_S_T    storeTime;      /**< 采集存储时标       */
    uint8    databuf[0];   /* 上报报文内容 */
}RPT_FIFO_T;

typedef struct 
{
    uint8  idxNum;             /* 本次上报索引数*/
    uint32 index[256];         /* 本次上报索引*/ 
    uint16 timeOut;            /* 设定的超时时间 */
    uint32 taskID;             /* 本次上报任务号(或为透明事件记录单元)*/
    uint16 rtrlen;             /* 本次上报长度*/
    uint16 rptlen;             /* 本次上报长度*/
    uint8  planID;             /* 本次上报方案号*/
    uint8  priority;           /* 上报优先级*/
    uint8  retryCnt;           /* 设定的重试次数*/    
    uint16 mergeNum;           /* 本次上报合并的透明结果数*/
    OOP_DATETIME_S_T   storeTime; /**< 采集存储时标       */
    uint8  rtrbuf[RPT_BUF_MAX];   /* 本次上报内容（用于存补报）*/
    uint8  rptbuf[RPT_BUF_MAX];   /* 本次上报内容（完整698报文）*/
}RPT_DATA_T;

/** @brief 上报状态*/
typedef enum
{
    RPT_NEXT = 0,     /* 发下一条 */
    RPT_RETRY = 1,    /* 重发本条 */
    RPT_WAIT =  2     /* 等待超时 */
}RPT_STATUS_E;

/** @brief 上报配置 */
typedef struct 
{
    RPT_TYPE_E        type;         /* 上报类型 */    
    RPT_STATUS_E      status;       /* 调度状态 */
    RPT_RES_STATUS_E *resStatus;    /* 上报响应状态 */
    BOOLEAN           sendTag;      /* 发送标记*/      
    BOOLEAN           retryTag;      /*重试标记*/  
    uint8             *piid;        /* 当前piid*/
    time_t            lstTime;      /* 上次时间*/
    time_t            curTime;      /* 本次时间*/
    DB_CLIENT         dbClient;     /* 数据中心句柄*/
    RPT_DATA_T        rptData;      /* 本次上报数据*/
    uint16            timeOut;      /* 当前超时时间 */
    uint8             retryCnt;     /* 当前重试次数*/    
    uint32            listID;       /* 队列ID*/ 
    uint32            sublistID;    /* 子队列ID*/ 
    uint8             chID;         /* 上报通道ID*/ 
    RPT_TYPE_E        realType;     /* 实际上报类型 */
}RPT_ENTRY_T;

#ifdef AREA_FUJIAN

/** @brief 事件上报策略配置 */
typedef struct 
{
    OOP_OADS_T          channel;        /**< 上报通道                       */
    uint8               priority;       /**< 上报优先级                      */
    OOP_TI_T            timeout;        /**< 上报响应超时时间                   */
    uint8               retry;          /**< 最大上报次数                     */
}RPT_EVENT_PLAN_T;

/** @brief 透明任务方案采集状态更新 */
typedef struct 
{
    uint16              taskid;         /**< 透明任务方案号                    */
    OOP_DATETIME_S_T    storeTime;      /**< 存储时间                       */
    uint16              reportNum;      /**< 上报条数                       */
    uint16              confirmNum;     /**< 确认条数                       */
}RPT_TRANSTASK_STATE_T;

#define RPT_REPORT_LISTID (0xFF200A00)  //上报队列
#define RPT_RETRYP_LISTID (0xFF200B00)  //补报队列
#define RPT_EVT_SUBLISTID (0x6E310200)  //事件子队列

/** @brief 日志补报信息*/
typedef struct
{
    uint16                 transTaskid;
    OOP_DATETIME_S_T       storeTime;
    OOP_DATETIME_S_T       readTime;
    char                   filename[128];    //日志文件名
}OOP_RPT_LOG_T;

/** @brief 日志补报信息*/
typedef struct
{
    uint16                 num;            //需要补报的个数
    OOP_RPT_LOG_T          fileinfo[256];      //需补报的日志信息
}OOP_RPT_LOGS_T;

/** @brief 日志补报*/
typedef struct
{
    uint16                 rdptr;                 //读索引
    uint16                 wtptr;                 //写索引
    OOP_RPT_LOG_T          fileinfo[256];      //需补报的日志信息
}RPT_LOGFIFO_T;

/** @brief 日志补报*/
typedef struct
{
    pthread_mutex_t        mutex;                 //锁 多线程访问
    RPT_LOGFIFO_T          fifo;
}OOP_REPORT_LOG_T;

#define RPT_LOGREC_FILE     "rpt_logrec_file"           //通信日志信息队列
#define RPT_LOGREC_MAX       256                        //通信日志队列深度

/** @brief 日志补报*/
typedef struct
{
    uint16                 valid;                 //是否有效
    uint16                 evtconfirm;            //事件确认数
}RPT_STAT_T;

#define RPT_STAT_FILE     "rpt_stat_file"           //上报统计信息私有文件


/** @brief 日志补报*/
typedef enum
{
    RPT_CHECK_NO = 0,
    RPT_CHECK_OK = 1,
    RPT_CHECK_IG = 2,
}RPT_CHECK_E;

void rpt_task_init_fj(void);
void rpt_task_fj_entry(time_t curTime, time_t lstTime);
void rpt_retry_fj_entry(time_t curTime, time_t lstTime);
int rpt_evtstat_read(uint16 *confirmNum);
void rpt_evtstat_clear(void);
void rpt_commlog_qclr(void);

#endif

//将批量下发的档案缓存住 另外一个线程写数据中心 提高效率
typedef struct
{
    pthread_mutex_t  lock;
    uint8 delay;    //下发档案后设置一个延时 这个时间后再写数据中心
    uint16 meterNum;
    OOP_METER_T meterList[NO_VIR_MTR_MAX_NUM];
}CACHE_METER_LIST_T;

//补报上报
#define RPT_RETRY_IS_MY_RES(piid)  ((piid&0x3f) == (suppGLBpiid&0x3f))

void rpt_task_detect(void);                      //--自动任务生成主接口--
void rpt_response_check(RPT_TYPE_E type, uint8 seq);                         //清除确认标志
int rpt_record_write(AUTO_DATA *save_data);
uint32 rpt_task_period_get(OOP_TASK_CFG OopTaskCfg);
void rpt_task_init(void);
void rpt_event_entry(time_t curTime, time_t lstTime);
void rpt_mtr_evt_entry(time_t curTime, time_t lstTime);
void rpt_task_entry(time_t curTime, time_t lstTime);
uint8 rpt_taskstate(int8 RunStat,float  *ClockLimt);
void rpt_timetask_entry(time_t lasttime);
void rpt_ansertime_deal(uint8 *buf,uint16 buflen);
int rpt_channel_check(OOP_OAD_U chOAD, CHANNEL_CACHE_T *ptChannel);
int rpt_event_write(int clientid, void* data, uint16 datalen);
BOOLEAN rpt_channel_search(uint8 *chID);

extern BOOL systime_set(DateTimeHex_t *pDateTime);
void cache_meter_entry(DB_CLIENT client,time_t curtime,time_t lasttime);
    int cache_meter_add(OOP_METER_T *meterFile,uint16 num);
    void cache_meter_init();

#ifdef __cplusplus
}
#endif

#endif /* __AUTO_TASK_H__ */
