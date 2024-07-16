#ifndef __PDS_COMMON_H__
#define __PDS_COMMON_H__

#define MTR_MAX_COUNT  2040   //最大电表个数
#define MAX_QUEUE_CNT  100       //最大队列个数
#define TOPO_DELAY_TIME    100            //拓扑识别延时
#define TOPO_DELAY_TIME_METER  600            //搜表延时

#define TOPO_INTERVAL       50          //判断为上下级关系的信号发送和记录时间间隔
#define TOPO_INFOLIMIT      10          //一次读取的拓扑数量
#define LINE_LOSS_CALC_MIN_DELAY   2            //计算线损的延时 默认5分钟
#define LINE_LOSS_CALC_HOUR_DELAY   10
#define LINE_LOSS_CALC_DAY_DELAY   35
#define LINE_LOSS_CALC_MON_DELAY   60
#define READ_DATA_TIME_ADD    (8*60*60)           //从数据中心读数据要加8小时

#define LOT_METER_SPRDT  (83) //单相居民用户物联网表
#define LOT_METER_TPIND  (84) //三相工业用户物联网表
#define LOT_METER_TPCHK  (99) //三相计量考核物联网表
#define mastermeter  6


/** @brief 普通采集方案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint16                 oopPlanNormalNum;   //方案个数
    BOOLEAN                   isChanged;          //变更标记
    BOOLEAN                   isVaild[256];       //方案是否有效
    OOP_PLAN_NORMAL_T      oopPlanNormal[256]; //方案参数
}OOP_PLAN_NORMAL_LIST_T;

/** @brief 任务数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;          //锁 多线程访问
    uint16                 oopTaskNum;     //任务个数
    BOOLEAN                   isChanged[256]; //变更标记
    BOOLEAN                   isVaild[256];   //任务是否有效
    OOP_TASK_T             oopTask[256];   //任务参数
}OOP_TASK_LIST_T;

typedef struct
{
    pthread_rwlock_t              rwLock;                         //读写锁 多线程访问

    uint16                        MeterCount;                //电表总个数
    OOP_METER_T                   MeterFile[MTR_MAX_COUNT];       //每个电表的档案
    //uint8                         MeterLogicid[NO_VIR_MTR_MAX_NUM];      //电表的逻辑设备号 默认0 配网7
}Meter_File_T;

typedef struct
{


    uint16                  Pn;                //参与线损的电表信息点号
    uint16                  lineword;          //线损特征字
    uint8                   lastdatavalid;
    uint8                   curdatavalid;
    uint32                  lastpositiveenergy;
	uint32                  curpositiveenergy;
	uint32                  lastnegetiveenergy;
	uint32                  curnegetiveenergy;
 }Line_Loss_File_T;

typedef struct
{
    uint16                        MeterCount;                //电表总个数
    uint32                        savetime;                  //存储次数
    Line_Loss_File_T              MeterFile[MTR_MAX_COUNT];       //每个电表的档案
 }Line_Loss_Info_T;

typedef struct
{
    uint8                        taskid;
	uint16                       delaytime;
	OOP_TI_T                     runti;

 }Task_Info_T;

typedef struct
{
    uint8                        tasknum;                //电表总个数
    Task_Info_T                  taskid[256];
 }Line_Loss_Task_Info_T;

typedef struct
{
   Line_Loss_Task_Info_T LineMinute;
   Line_Loss_Task_Info_T LineDay;
   Line_Loss_Task_Info_T LineMonth;
}Line_Loss_ALL_Task_Info_T;

typedef enum
{
    PDS_INIT =0,        //初始态
    PDS_SEND,           //发送
    PDS_RECV,           //接收
    PDS_RETRY,          //重试
    PDS_DELAY,         //延时
}Send_Recv_State_T;

typedef enum
{
    TOPO_INIT=0,        //初始
    TOPO_TIMING,        //广播校时
    TOPO_CLEAR,         //清空拓扑记录   
    TOPO_TIME_OFFSET,   //时间补偿
    TOPO_START,         //启动拓扑
    TOPO_START_METER,   //启动搜表
    TOPO_READ,          //读取拓扑记录
    TOPO_GETMETER,      //读取一级分支和表箱断路器下接表档案信息
    TOPO_FINISH,        //结束
    
}Topoloty_State_T;

typedef struct
{
    uint16 breakID; //断路器索引
    uint8 phase;//这个相位是收到信号的相位，实际是儿子所在的相
    uint8 addr[6];
}Break_Son_T;   //断路器子节点信息

typedef enum
{
    NODE_ROOT = 0,  //根
    NODE_BODY = 1,  //躯干
    NODE_LEAF = 2,  //叶子
}NodeType_E;

typedef struct
{
    uint16 Pn;          
    uint8 usertype;     //用户类型 213 分支断路器 220 表箱断路器
    uint8 branchtype;       //分支类型 1表示一级分支 2表示二级分支
    uint8 Addr[6];      //地址
    uint8 baud;
    uint32 port;
    uint8 failcnt;          //失败次数
    uint32 starttime;           //启动拓扑的时间 
    uint8   ParentNum;             //亲代数量，包含所有祖先节点
    Break_Son_T ParentInfo[MAX_BREAK_COUNT];  //断路器亲代节点信息
    uint8   SonNum;             //儿子数量
    Break_Son_T breakSonInfo[MAX_BREAK_COUNT];  //只有表箱断路器搜表结果存在此    
    uint8 isMeterSon; //其儿子是否为表档案
    NodeType_E nodeType; //节点类型
    uint32 topoInfoNum; //收到的拓扑记录条数
    int timeoffset;   //补偿时间
}Break_Info_T;

typedef struct
{
    uint8 isInTopo;                     //当前是否在拓扑识别过程中 0表示空闲 1表示正在拓扑工作中
    Send_Recv_State_T  sendRecvState;   //拓扑识别的收发状态
    Topoloty_State_T    topoState;      //拓扑识别所处状态
    uint16   delay;              //发送延时
    uint16 breakCount;      //断路器个数
    uint16 breakIndex;           //当前发送的索引
    uint16 msgIndex;            //发送的消息序号
    Break_Info_T    breakInfo[MAX_BREAK_COUNT];     //断路器信息
    uint32 overtime;
    uint8  isFinish;        //拓扑识别过程是否结束
}Topology_Arg_T;

typedef struct
{
    uint8 isNeedCalc;       
    uint16 delay;
    OOP_OAD_U  mainOad;     //估计统计周期确定是什么冻结类型
    time_t  lastStoreTime[2];      //需要获取的上两次的存储时间
} Line_Loss_Calc_T;  //线损计算结构体


typedef struct
{
    uint16 Pn;
    uint8 Addr[6];
    uint32 Pt;
    uint32 Ct;
    OOP_WORD3V_T vol;       //电压
    OOP_INT3V_T  cur;       //电流
    OOP_WORD3V_T lastVol;       //上一次电压
    OOP_INT3V_T  lastCur;       //上一次电流
    OOP_DWORD3V_T lastImpedance;    //上一次阻抗数据
}Meter_Vol_Cur_T;      //存储每个档案的电压电流冻结数据 上次以及上上次

typedef struct
{
    uint16 count;
    Meter_Vol_Cur_T data[MTR_MAX_COUNT];
}All_Vol_Cur_t;

typedef struct
{
    uint8 isNeedCalc;       
    uint16 delay;
    time_t storeTime[2];       //需要获取电压电流数据的冻结时间
    
}Line_Impedance_Calc_T;//线路阻抗计算结构体
typedef struct
{
    DB_CLIENT clientid;
    Topology_Arg_T topologyArg;             //拓扑识别
    Line_Loss_Calc_T lineLossArg;           //线损
    Line_Impedance_Calc_T lineImpedanceArg; //阻抗
}Calc_Thread_Arg;


/* 总及分相电能量*/
typedef struct tag_PDS_ENERGY4V
{
    uint32    nNum;
    uint32    rsv;
    uint32    nValue[4];
}PDS_ENERGY4V_T;


//存储每个档案的正向有功总电能数据 上次以及上上次
typedef struct
{
    uint16 Pn;
    uint8 Addr[6];
    uint32 Pt;
    uint32 Ct;    
    PDS_ENERGY4V_T EleData[2]; //正向有功总电能数据 0上一次 1上上次
    uint8 phaseinfo;  //相位信息
}Meter_Data_T;      

typedef struct 
{
    uint16 count;
    Meter_Data_T  data[MTR_MAX_COUNT];
}All_Meter_Data_T;

typedef struct
{
    uint8   Second              :1;             //过秒标志
    uint8   Minute              :1;             //过分标志
    uint8   Hour                :1;             //过时标志
    uint8   Day                 :1;             //过日标志
    uint8   Month               :1;             //过月标志
    uint8   ReserveBit6         :1;             //保留
    uint8   ReserveBit7         :1;             //保留
    uint8   ReserveBit8         :1;             //保留
} PastTimeFlagBit_T;
typedef union
{
    PastTimeFlagBit_T   BitSect;
    uint8               Word;
} PastTimeFlag_T;                               //过时间标志

typedef struct
{

    uint8    DelayFlag;                         //组帧时的通信延时标志
    uint16   DelayCnt;                          //通信延时时常
    uint8   PrtlMode;                           //规约类型
    uint8   AcqUnitAddr[6];                     //采集器地址
    uint8   MtrAddr[6];                         //电表地址
    uint8   CtrlCode;                           //控制码
    uint8   DataLen;                            //数据域长度
    uint8   DataBuff[256];                      //数据域内容
}MeterPrtlFmt_T;                                //电表规约格式

typedef struct
{
    int32  QueueLen;                           //队列长度
    uint8   QueueBuf[1400];     //队列存储缓冲区
} Queue_T;
//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                          //队列头
    int16  QueueTail;                          //队列尾
    Queue_T Queue[MAX_QUEUE_CNT];               //通讯数据队列
} LockQueue_T;

typedef struct 
{
    pthread_mutex_t lock;
    OOP_TI_T  ti;     //线损计算周期 
    
}Line_Loss_Period_T;    //线损计算周期

typedef struct
{
    pthread_mutex_t lock;
    uint32 limit; //阻抗限值
}Line_Impedance_Limit_T;
void mainInit();
uint16 MeterFileInit(DB_CLIENT clientid);
void checkTopology(uint8 *buf,uint16 len);
void calcThreadInit();
void TimeManage();
void topologyManager();
uint16 getAllBreaks();
int PackMeterPrtl(uint8 *PrtlData, MeterPrtlFmt_T MeterData);
int32 TransWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);
int32 TransReadQueue(LockQueue_T *pLockQueue, uint8 *buf);
int topologyRecv(uint8 *Recvbuf,uint16 Recvlen);
int topologySend();
int UnpackProtocol_645(uint8 *buf,uint16 len,MeterPrtlFmt_T *data);
int DelayDeal(uint16 *delaysec);
uint16 GetNextBreakIndex(uint8 usertype);
uint8 UnpackTopoInfo(uint8 *buf,uint16 len);
uint16 GetNextBreakIndexReadMeter();
void UnpackMeterInfo(uint8 *buf,uint16 len);
void SaveTopoResult();
int WriteNormalData(DB_CLIENT clientid,uint8* DataBuf,uint16 DataLen,OOP_OAD_U Oad,uint16 infonum);
int ReadLocalAddr(DB_CLIENT clientid,uint8 *addr);
int ReloadLocalAddr(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int DBGetMeterFile(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
void topology_start(MESSAGE_INFO_T *pMsg);
int32 TransWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);
int GetLineLossPeriod(DB_CLIENT clientid);
int ReloadLineLossPeriod(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
BOOL CheckLineLossCalc();
void LoadTopoInfoFile(Topology_Arg_T *topoArg);
void SaveTopoInfoFile(Topology_Arg_T *topoArg);
int LineLossCalc();
void CalcTaskCheck();
int GetMeterEnergyData(Meter_Data_T *meterData,OOP_OAD_U mainoad,uint32 storetime,PDS_ENERGY4V_T * data);
uint16 GetPnFromAddr(uint8* Addr);
uint16 GetMasterBreak(uint8 *Addr);
void LineImpedanceCalc();
int GetImpedanceLimit(DB_CLIENT clientid);
 Meter_Vol_Cur_T * GetVolAndCurIndex(uint16 pn);
 uint32 GetEventIndex(DB_CLIENT clientid,uint32 oad);
 int ReloadImpedenceLimit(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
 OOP_NODEPHASEINFO_T gettmpphase(uint16 Pn);
 void CalcLineLossData(Line_Loss_Calc_T *lineLossArg, uint16 pn, uint8 *addr, PDS_ENERGY4V_T *parentdata, PDS_ENERGY4V_T *sondata);

 typedef void (*SWAPFUN)(void *s1, int id);
 void CalcTaskCheckNew();
int appdb_get_task_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
void task_list_lock_init(void);
void plan_normal_list_lock_init(void);
int appdb_get_plan_normal_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
void appdb_task_list_record_init(int clientid);
void appdb_plan_normal_list_record_init(int clientid);


#endif

