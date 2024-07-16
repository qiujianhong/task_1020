
#ifndef __CTSREDEF_H__
#define __CTSREDEF_H__

#include "db_api.h"

#define MAX_QUEUE_CNT           50              //最大队列数
#define MAX_OAD_NUM             64              //单个方案支持的最大OAD数量
#define MAX_AMR_COLLECT_CNT     1000            //最大抄表汇总个数
#define MAX_SPECIAL_TMP_LEN     40               //最大数据合并暂存区长度
#define MAX_EVENT_ROAD_NUM      22              //单个事件采集对应的成员数量
#define SCHEME_MAX_NUM          80              //最大支持的方案数
#define MTR_MAX_NUM             2040                //最大测量点个数
#define Search_MTR_MAX_NUM      5000                //最大测量点个数

#define MAX_MSG_LEN       8192      //消息最大长度
#define RELAY_MAX_LEN           1200
#define PLCTASK_MAXNUM                            50

#define PATCH_FILE         "CtsPatchTask"               //补抄文件名称CtsPatchTask任务号_上几天_PIPE端口号
#define PATCH_FILE_TMP  "CtsPatchTaskTmp"
#define TASK_RECORD_FILE    "CtsTaskRecord"             //记录每个任务最近一次抄表状态
//#define TASK_Need_Again_FILE    "TaskNeedAgain"             //记录每个任务最近一次抄表状态
#define MAX_SEARCH_METER_CNT       32


#define FPGJ    1
#define JFPG    2
#define NOPD    0
#if ZHEJIANG_FUN == 1      //浙江功能
#define ERR_FRAME_MAX       10      //抄表失败的总帧数（相同报文重发不算）
#else
#define ERR_FRAME_MAX       1       //抄表失败的总帧数（相同报文重发不算）
#endif
#define ERR_TIMES_MAX       1       //同一帧的重试次数
#define MAX_CHANNEL_NUM     5       //每个模组最多有5个通道 0默认管理通道

//跑分测试会一次性上报20个
#define MAX_EVENT_MTR       25     //最多同时处理的电表个数

#define MAX_EVENT_PLAN_NUM          10
#define BUFF_SIZE_RCV               512         //数据上传的最大长度


typedef enum
{
    RSS_IDLE = 0,                               //空闲
    RSS_SET_STUDY_MODE,                         //设置路由学习模式
    RSS_GET_RUN_STATE,                          //读取运行状态
    RSS_DELAY,                                  //延时状态
    RSS_SET_AMR_MODE,                           //设置路由抄表模式
    RSS_STUDY_END,                              //学习结束
}ROUTER_STUDY_STATE_T;                          //路由学习处理状态字
typedef enum
{
    RCS_NULL = 0,                               //无效
    RCS_RESTART,                                //重启
    RCS_PAUSE,                                  //暂停
    RCS_RESUME,                                 //恢复
    RCS_RESUME_EXEC,                            //恢复执行
}ROUTER_CTRL_STATE_T;                           //路由表控制状态
typedef enum
{
    MS_IDLE = 0,                                //空闲
    MS_DELAY_DEAL,                              //1 延时处理
    MS_GET_MANU_ID,                             //2 获取厂商标识
    MS_POWER_ON,                                //3 上电处理
    MS_FILE_CMP,                                //4 电表档案比对
    MS_FILE_MODIFY,                             //5 电表档案修改处理
    MS_DEL_SLAVE_DEAL,                          //6 删除从节点处理
    MS_CUS_TASK_DEAL,                           //7 自定义单发任务处理
    MS_SWITCH_CHANNEL,                          //8 1切换信道处理
    MS_ROUTER_RESTART,                          //9 重启
    MS_ROUTER_PAUSE,                            //10 暂停
    MS_ROUTER_RESUME,                           //11 恢复
    MS_UPDATE_DEAL,                             //12 载波模块本地文件升级处理
    MS_WORK_MODE,                               //13 设置路由工作模式
    MS_NET_STUDY_DEAL,                          //14 微功率无线组网处理
    MS_ADDR_SEARCH,                             //15 检测表号搜索
    MS_RELAY_TRANS,                             //16 检测透明转发
    MS_ROUTER_DEAL,                             //17 抄表操作
    MS_HOUR_AMR,
    MS_ROUTER_STUDY,                            //19 路由学习
    MS_NORMAL_AMR,                              //20 日常抄表
    MS_PATCH_AMR,                       //21补抄任务
    MS_EVENT_REPORT,                        //事件上报处理
    MS_BROAD_TIMING,                //广播对时任务
    //MS_GET_HPLC_INFO,               //查询HPLC信息
    MS_RENETWORKING,                //重组网任务
    MS_FILE_TRANSFER,           //文件代发任务
    MS_NODE_VERSION_GET,        //获取节点模块信息
    MS_METER_TIME_MANAGER,      //电表报上电事件后的时钟管理
    MS_METER_TIME_DELAY,        //避开整点5分钟校时
#ifdef AREA_HUNAN
    MS_STA_CURVE_STATUS_GET,        //获取STA模块配置状态
#endif
}MAIN_CYC_STATE_T;                              //主循环状态字
typedef enum
{
    TASK_INIT = 0,                      //任务初始化
    ROUTERCTRL_DEAL,                    //路由控制
    ROUTERCTRL_DELAY,                   //延时执行
    FUN_DEAL,                           //功能函数执行
    ROUTERRECOVER_DELAY,                //路由恢复延时
    ROUTERRECOVER_DEAL,                 //路由恢复执行
}TASKDEAL_STAT;
typedef enum
{
    PLCRELAY_TRANSFRAMR = 0,          //透明帧(回码不判断)
    PLCRELAY_DATAREAD,                //数据召测(必须判断返回的数据内容)
    PLCRELAY_REQUERY_METERCLOCKDELAY, //请求电表时钟延时(下发时AFN=14H ,F1需要带延时标志)
    PLCRELAY_SET_METERCLOCKDELAY,     //设置电表时钟(AFN=14H ,F3直接下发)
    PLCRELAY_BROADCAST,               //广播数据帧,不需要判断回码
    PLCRELAY_METERDEAL,               //电表操作帧仅判断控制码
    PLCRELAY_OOP_MERER,               //面向对象电表处理
    PLCRELAY_OOP_TRANSFRAMR,          //面向对象电表透明转发处理
}RELAY_TASK_TYPE;

typedef enum
{
    TASK_CFG_UNINIT,              //未初始化
    TASK_CFG_MODIFY,              //修改
    TASK_CFG_MODIFY_DELAY,        //修改延时
    TASK_CFG_INITED,              //已初始化
    TASK_CFG_REBUILD,             //重建采样文件
    TASK_CFG_PATCH,     //创建补抄文件
} TASK_CFG_STATE;//方案档案状态


typedef enum _TASK_CYCLE_FLAG
{
    TASK_CYCLE_FLAG_SEC = 0,        /*-秒-*/
    TASK_CYCLE_FLAG_MIN,            /*-分-*/
    TASK_CYCLE_FLAG_HOUR,           /*-时-*/
    TASK_CYCLE_FLAG_DAY,            /*-日-*/
    TASK_CYCLE_FLAG_MON,            /*-月-*/
    TASK_CYCLE_FLAG_YEAR,           /*-年-*/
}TASK_CYCLE_FLAG;
typedef enum _TASK_RUN_STATE
{
    SCHEME_UNDONE,                  //未完成
    SCHEME_FINISHED,                //本轮已完成
    SCHEME_DOING,
    SCHEME_FAIL,            //执行失败 等下一轮
}RUN_RESULT;

typedef enum _BC_RUN_STATE
{
    TASKBC_START=0,
    TASKBC_DOING,
    TASKBC_FINISHED,                //本补抄已完成

}BC_RESULT;

//表库的返回值
typedef enum
{
    CB_RET_FAIL=0,         //抄表失败
    CB_RET_OK,         //抄表成功
    CB_RET_END,            //抄表结束，所有的表已抄完
    CB_RET_RESTART,        //抄表重启（主台的控制命令）
    CB_RET_NULL,           //未抄读(任务中全部OAD都不支持，未发送抄表帧就完成)
}CB_RET;

typedef enum
{
    ACQTYP_NULL=0,              //采集当前数据
    ACQTYP_LASTN,               //采集上第N
    ACQTYP_FRZTM,               //冻结时标采集
    ACQTYP_TI,                  //按时标间隔采集
    ACQTYP_BC,                  //补抄
}AcqType_t;
typedef enum
{
    STRTMTYP_NULL=0,            //未定义
    STRTMTYP_STARTTM,           //任务启动时间
    STRTMTYP_DAYSTART,          //相对当日0点0分
    STRTMTYP_LASTDAYEND,        //相对上日23点59分
    STRTMTYP_LASTDAYSTART,      //相对上日0点0分
    STRTMTYP_MONTHSTART,        //相对当月1日0点0分
    STRTMTYP_FRZTIME,           //数据冻结时标
    STRTMTYP_LASTMONTHEND,      //相对上月月末23点59分
}StoreTimeType_t;
typedef struct
{
    pthread_rwlock_t              rwLock;                         //读写锁 多线程访问
    TASK_CFG_STATE                FileState;                 //档案状态
    int                           ModifyDelaySec;     //方案配置修改延时
    time_t                        FunLastRunTime;            //方案上次修改时间
    uint16                        MeterCount;                //电表总个数
    OOP_METER_T                   MeterFile[MTR_MAX_NUM];       //每个电表的档案
    uint8                         MeterLogicid[MTR_MAX_NUM];      //电表的逻辑设备号 默认0 配网7
}Meter_File_T;
typedef struct
{
    uint8 num;
    uint8 meteraddr[32][6];  //电表地址
}Meter_Info_T;

typedef struct
{
    uint8       addr[6];      //采集器地址
    uint16 pn;
    uint8 prtl;
    Meter_Info_T meterinfo;
}Collect_File_T;

/** @brief 电能表档案  */
typedef struct tag_OOP_COLLECTO
{
    uint8            count;     /**< 采集器数量   */
    Collect_File_T    collectaddr[100];
}OOP_COLLECTOR_T;

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
typedef enum
{
    AMR_PRE_PERIOD = 0,  //尚未达到允许的抄表时间
    AMR_IN_PERIOD,       //处于允许的抄表时间
    AMR_AFT_PERIOD,      //超过了允许抄表的时间
}AMR_PERIOD_STAT;//抄表时段状态
typedef union
{
    PastTimeFlagBit_T   BitSect;
    uint8               Word;
} PastTimeFlag_T;                               //过时间标志

typedef struct
{
    MAIN_CYC_STATE_T        TaskName;              //任务名
    uint8                   Priority;              //任务优先级
    ROUTER_CTRL_STATE_T     RoutrtCtrl;            //任务路由操作
    uint8                   DelaySec;              //任务延时时间
    int                     (*pPlcTaskFun)(uint8);  //任务功能函数

}PLCTASK_FUNINFO;//载波任务功能信息

typedef enum
{
    MODEFY_F10 = 0,                           //档案修改
    MODEFY_CLEAR,                             //档案全清的方式
}MODEFY_TYPE_T;                                  //抄表结果
typedef struct MODULE_METADDRHEAD_              //需要删除或者添加的从节点模块信息
{
    uint16                      RouterSn;             //如果是添加的从节点那么需要获取可以使用的RouterSn
    uint8                           Module_MetAddr[6];    //从节点信息
    uint8                       Prtl;//协议类型
    struct MODULE_METADDRHEAD_ *PNext;

}MODULE_METADDRHEAD;

typedef struct Node
{
    uint8                 TaskStat;              //任务有效性
    MAIN_CYC_STATE_T      TaskName;              //任务名
    int8                  Priority;              //任务优先级
    ROUTER_CTRL_STATE_T   RoutrtCtrl;            //路由控制操作
    uint8                 DelaySec;              //延时
    void                 *gInnerStat;           //任务内部处理状态
    int                   (*pPlcTaskFun)(uint8);  //任务功能函数
    TASKDEAL_STAT         PlcTaskDeal_Stat;      //载波任务执行状态
    struct Node           *next;                 //指向下一个任务的指针
}PLCTASK_QUEUE;//载波任务队列
//队列
#define MAX_QUEUE_SIZE  2200

typedef struct
{
    int32  QueueLen;                           //队列长度
    uint8   QueueBuf[MAX_QUEUE_SIZE];     //队列存储缓冲区
} Queue_T;

//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                          //队列头
    int16  QueueTail;                          //队列尾
    Queue_T Queue[MAX_QUEUE_CNT];               //通讯数据队列
} LockQueue_T;

typedef struct{
    uint8 unit;             // 秒（0）分（1）时（2）日（3）月（4）年（5）
    uint16 value;
}TI_t;


typedef enum
{
    MFMS_IDLE = 0,                              //空闲
    MFMS_PAUSE,                                 //暂停
    MFMS_DELET_METERFILE_FLAG,    //忽略档案修改//start Common Problem
    MFMS_FILE_CLEAR,                            //档案清除
    MFMS_FILE_ADD,                              //档案添加
    MFMS_FILE_DELETE,                           //档案删除
    MFMS_RESTART,                               //重启
}METER_FILE_MODIFY_STATE_T;                     //电表档案修改状态字
typedef enum
{
    INNER_RTS_IDLE = 0,                               //空闲
    INNER_RTS_PIPE_INIT,
    INNER_RTS_485_INIT,                               //485串口初始化
    INNER_RTS_BROASTDELAY,                            //广播对时获取通道的延时时间
    INNER_RTS_SEND,                                   //发送
    INNER_RTS_DELAY_WAIT,                   //等待上报广播延时
    INNER_RTS_WAIT_NEXT,                    //等待下一个具体任务
    INNER_RTS_RECV,                                   //接收
    INNER_RTS_RETRY,                                  //重试
    INNER_RTS_INVALID,                      //代理的表无效或者超时
    INNER_RTS_END,                                    //结束
}INNER_RELAY_TASK_STATE_T;                            //内部处理穿透的逻辑

typedef enum
{
    CB_STATE_INIT=0,           //初始化
    CB_STATE_CHECK,            //检查信道状态
    CB_STATE_SEND,         //发送
    CB_STATE_RECV,         //接收
    CB_STATE_RELAY,            //中继
    CB_STATE_FINISH,           //结束抄表，准备退出
    CB_STATE_PAUSE,            //暂停抄表,正在中继
    CB_STATE_CONTINUE,     //继续抄表
    CB_STATE_RESTART,          //重新抄表
    CB_STATE_HANDSEND,         //进口表握手发送
    CB_STATE_HANDRECV,         //进口表握手接收

    CB_STATE_FRZSEND,           //DLMS表冻结参数和数据召测发送
    CB_STATE_FRZRECV,           //DLMS表冻结参数和数据召测接收
}CB_STATE;

typedef struct
{
    mtrnum_t   MtrNum;                 //测量点号
    uint8      ValidFlag;              //计量点是否有效1:有效；0:无效
    uint8      MtrType;                //1：485表、02：模拟量、03脉冲量、04计算值、05：交采
    mtrnum_t   TurnNumID;              //电能表/交流采样装置序号
    uint8      ComId;                  //表示该计量点接在哪个通道上
    uint8      PrtlType;               //电表为485电表的哪种类型，主要指电表规约
    uint8      MetAddr[6];             //电表通讯地址
    uint8      CjqAddr[6];             //采集器地址
    uint8      LineMode;               //电表接线方式:1单相、3三相三线、4三相四线
    uint8      PeriodSeq;              //表征费率顺序
    uint8     Speed;                  //通讯速率
    uint8      FailTimes;              //失败次数
    OOP_TSA_T      MtrCommAddr;            //表通信地址
    OOP_TSA_T      CjqCommAddr;            //采集器通信地址
    //uint8    MtrEsamSerialNo[8];     //ESAM序列号
}cbMtrPara;

typedef struct{
    time_t          startTime;  //起始时间用于负荷曲线抄读
    time_t          endTime;    //起始时间用于负荷曲线抄读
    OOP_TI_T            tmInv;      //时间间隔用于负荷曲线抄读
    uint8           startNum;   //起始搜索次数
    uint8           endNum;     //最大搜索次数
    DateTimeBcd_t   storeTm;    //存储时间
}cbLPPara_t;

typedef enum
{
    TASK_RELAY,                //0        中继直抄命令    由规约模块触发
    TASK_DATA1_RELAY,       //1     点抄任务 中继线程处理 不在485抄表线程处理
    TASK_CURVE,             //2     曲线任务 每15min启动，抄上个15min数据
    TASK_CBDAY,             //3     抄表日任务    设定的抄表日时间
    TASK_MONTH,             //4     月任务    当前时间为23点50分
    TASK_MIN1440,           //5        1440分钟抄表数据项预抄    23时55分钟
    TASK_MIN60,                //6        60分钟抄表数据项预抄    抄读07电能表的整点数据
    TASK_MIN30,                //7        30分钟抄表数据项预抄    当前分钟为30或5分钟
    TASK_POINTDAY,          //8     日任务 00:05启动
    TASK_MIN1,              //9        1分钟抄表数据项预抄    当前分钟为5的倍数
    TASK_FORM_CB,           //10    定制实时任务    时间为设定时间的倍数
    TASK_PRE_MIN15,            //11    15分钟前任务，暂定为前一分钟任务，增加曲线数据的精度
    TASK_ESP_CB,            //12    特殊任务    针对一些特殊要求的电表   交采任务 5秒抄一次
    TASK_MIN15,             //13     15分钟抄表数据项预抄    当前分钟为15的倍数
    TASK_POINTDAYBC,        //14    日冻结任务对应07电表的日数据补抄功能 0:15启动
    TASK_EVENT_LOSS_VOL,    //14   失压事件
    TASK_NO_MET,            //16    无抄表任务状态    当前没有抄表任务
}enum_cbtask_id;


//记录对象列表object_list. 连接 SN（Association SN）(class_id: 12)
typedef struct
{
    uint8       oBis[6];        //OBIS
    uint16      sn;      //短名,base_name,是对象的基地址
    uint8       class_id;        //类号
    uint8       version;
    OOP_OAD_U   oad;
}__attribute__ ((packed))DLMS_Association_OBJ;

typedef struct
{
    uint16 objNum;                  //对象数量
    DLMS_Association_OBJ obj[512];  //暂时按512，现场看到有300多
}DLMS_Association_OBJ_LIST ;

//dmls冻结数据（曲线、日冻结及月冻结）所用到的基本参数及最近一次召测到的数据
typedef struct
{
    uint32 period;//捕获周期

    uint8  SN_itemNum;//dlms表内配置的数据项经短名处理后的数量,
    uint16 SN_item[64];//数据项短名
    OOP_OAD_U frzoad[64];
    int dataLen;//缓存召测到的数据长度
    uint8 dataBuf[3000];//数据按SN_item的顺序排列,为数组格式

}DLMS_FRZ_CAPTURE_PARA_DATA;

////dlms表驱动内部需要的通信相关信息，专用的信息尽可能放在这里，避免与其他表驱动耦合
//typedef struct _DLMS_METER_COMM_INFO
//{
//  uint8   FrzFrmTypeId;           //--冻结帧种类序号--
//  uint8   FrzFrmTypeTotal;        //--取冻结帧种类总数--
//  uint16  FrzFrmType[20];         //--冻结帧种类--
//
//  BOOL    bFrzValid;              //是否要从DLMS表中取冻结数据
//  uint8   frzType;                //冻结数据类型,目前有日、月、曲线1、曲线2，见 eDLMS_FRZ_CAPTURE_PARA
//  uint8   curveID;                //当前曲线编号，0第一条，1第二条
//
//
//  uint8   getFrzDataPdu[512];         //缓存的召测冻结数据的报文，这个报文用于自解析并从冻结数据缓冲区中抽取所需数据（仿真表计回码）
//  uint8   getFrzDataPduLen;           //缓存的召测冻结数据的报文长度
//  DLMS_FRZ_CAPTURE_PARA_DATA dlmsFrzCaptureParaAndData[4];    //依次为：日冻结捕获、月冻结捕获、/曲线1冻结捕获、曲线2冻结捕获
//  DLMS_Association_OBJ_LIST dlmsAssociation_OBJ_LIST;     //对象列表，这里存放的是全集
//
//  //收发帧序号
//  uint8 RECV_SEQ;
//  uint8 SEND_SEQ;
//}DLMS_METER_COMM_INFO;

//各种表通讯信息
 typedef struct
 {
     uint8   HDSKFraNum;           //--握手帧序号--
     uint8   HDSKFraTotal;         //--握手帧总数--
     uint8   IsLastSeq;            //-是否为该命令的最后一帧-
     uint8   ErrFrame;             //--失败总帧数 相同报文重发不算--
     uint8   ErrTimes;             //--当前帧失败重试次数--
     uint8   recvseq;
     uint8   sendseq;
     uint8   ComCount;
     #if OS_METDLMS_EN == 1
     uint8   CmdDI;                 /*--命令标识:表征读、写、重读等--*/
     uint8   FraTimes;              /*--帧序数:有的数据ID，需要几条读取命令--*/
     uint16  SN_DLMS;               // DLMS短名
     uint8   CLASS_DLMS;            // DLMS类号
     #endif
     uint16  pHDSKCont[20];         //--握手帧内容--
     uint8   ComFrame;             //--当前通讯帧数--
     uint16  Response;             //--电表返回第一个字节时间--
     uint16  RcvTime;             //--字节之间的间隔时间--
     CB_STATE MetStat;             //抄表过程的状态机
     CB_RET      AutkStat;         //本次抄表的返回值
     uint8      SndLen;        //发送报文的长度
     uint8      pSndBuf[300];      //发送、接收缓冲区
     uint16     RcvLen;            //--缓冲区中实际接收数据长度--
     uint8  pRcvBuf[3000];          //接收缓冲区
     enum_cbtask_id TaskType;
     uint16 bCurTaskIDIndex;        //当前处理的pCbTaskID的序号
     uint16 bCurMapIDIndex;                     //当前处理的pMap的序号
     uint16  FraDI;                //--帧标识:(DSSD25、STD)帧标识--
     uint8 SucOADNum;                 //成功的子OAD数量主要用于主OAD为0的任务的补抄
     uint8   PeriodSeq;            //-jfpg  fpgj-
     cbMtrPara* pMtrPara;
     uint8          subOADIndex;
     cbLPPara_t      cbLPPara;
     uint8 sendnum; //485实时抄读一次发送的OAD数量；
     uint8 mapindex;  //用于智能电容器的抄读
     uint8   IsKnownMonFrm;
     uint8   Class12Frm;
     uint8   LastMonReg;
     uint8   LastMonNonData;
     uint8  IsSendSn;
     uint8  DlmsIndex;
     uint8  DlmsNum;
     uint16 shortname[20];
     uint8  combinbuf[500];//需要合并的数据项，数据开头存放合并的数据项个数，然后是长度加每个数据项的内容
     uint16 combinoffset;
     uint32 rsv;
     BOOL    bFrzValid;             //是否要从DLMS表中取冻结数据
     uint8   frzType;                //冻结数据类型,目前有日、月、曲线1、曲线2，见 eDLMS_FRZ_CAPTURE_PARA
     uint8  FrzFrmTypeId;           //--冻结帧种类序号--
     uint8  FrzFrmTypeTotal;        //--取冻结帧种类总数--
     uint16 FrzFrmType[20];         //--冻结帧种类--
     uint8   curveID;                //当前曲线编号，0第一条，1第二条
     uint8   getFrzDataPdu[512];         //缓存的召测冻结数据的报文，这个报文用于自解析并从冻结数据缓冲区中抽取所需数据（仿真表计回码）
     uint8   getFrzDataPduLen;           //缓存的召测冻结数据的报文长度
     DLMS_FRZ_CAPTURE_PARA_DATA dlmsFrzCaptureParaAndData[4]; //依次为：日冻结捕获、月冻结捕获、/曲线1冻结捕获、曲线2冻结捕获
     DLMS_Association_OBJ_LIST dlmsAssociation_OBJ_LIST;      //对象列表，这里存放的是全集
 }S_COMM_INFO_MET;
// 结构体的声明

typedef struct
{
    RECORD_OAD_T    recordOAD ;
    OOP_OCTETSTR6_T MAC;         //附加校验,现在放的是表地址
    uint32          colStartTm;  //采集起始时间
    uint32          colEndTm;    //采集结束时间
    uint32          colStoreTm;  //采集存储时间

}OadStoreInfo;
typedef struct{
    time_t          startTime;  //起始时间用于负荷曲线抄读
    time_t          endTime;    //起始时间用于负荷曲线抄读
    OOP_TI_T            tmInv;      //时间间隔用于负荷曲线抄读
    uint8           startNum;   //起始搜索次数
    uint8           endNum;     //最大搜索次数
    DateTimeBcd_t   storeTm;    //存储时间
}ProfilePara_t;

typedef enum
{
    ENCRYPT_INIT =0,
    ENCRYPT_COUNTER_GET,    //获取计数器
    ENCRYPT_CONNECT,    //建立应用连接
    ENCRYPT_TIMING,     //校时
    ENCRYPT_ESAM_SET,   //设置esam参数
    ENCRYPT_END,    //一块表抄完
    ENCRYPT_NORMAL_TASK,    //普通任务数据
}ENCRYPT_STATUS_E;  //转加密任务所处的状态

//运行时任务信息
typedef struct CTS_RUN_TASK_T_
{
    uint8 logicid;
    uint16 TaskId;      //任务编号
    uint8 TaskState;        //任务状态 1正常 2停用
    uint8 Priority;     //优先级
    OOP_OAD_U MainOad;  //主oad
    uint16 NeedMtrNum;  //该任务在本线程需要抄读的电表个数
    uint16 SucNum;      //采集成功个数
    uint8 MinFrozenRebootCnt;           //分钟冻结任务重启次数 最大三次
    uint16 MinFrozenFailCnt;        //分钟冻结失败表个数
    uint16 DayFrozenFailCnt;        //日冻结失败表个数
    uint16 MonFrozenFailCnt;        //月冻结失败个数
    uint16 CbNum;      //已经抄读的个数
    uint16 MeterIndex;  //当前抄表电表索引
    uint16 Patchindex;  //当前抄表电表索引
    RUN_RESULT  RunResult;   //任务执行结果
    char    ThisTurnBeginTime[7];    //本轮执行开始时间 带延时
    char    ThisTurnEndTime[7];     //本轮执行结束时间
    char    NextTurnBeginTime[7];   //下轮执行开始时间
    char    LastTurnBeginTime[7];   //上一轮执行开始时间 不带延时
    char    ThisTurnRealBeginTime[7];    //本轮任务正在开始执行时间
    AMR_PERIOD_STAT               ThisTurnPeriodStat;             //是否处于抄表时段内
    time_t              StartTime;      //用于每轮抄读组帧的起始时间
    ProfilePara_t   ProfilePara;
    uint8 MtrResult[128];        //按位表示每个电表索引是否抄读成功 1表示抄完 顺序为任务中的电表顺序
    //uint8 CbResult[128];        //按位表示本次每个电否抄读成功,1表示没有抄读成功，需要后面继续抄读，0表示抄读成功。
    BOOLEAN                          NeedAgain;        //该任务是否需要补抄
    BC_RESULT                          IsAlreadyBc;               //是否已经补抄过了
    uint16 CsdIndex;             //485专用 一个表一个表抄的
    uint16 SubOadIndex;         //485专用 一个表一个表抄的
    uint8   IsNewTurn;      //标记是否到新一轮
    uint16 TotalMtrNum;  //该任务所要抄读的所有电表总数
    //OOP_TRANSRES_T     TransResult[OOP_MAX_TRANS_INFO]; //存储透明方案抄回来的记录内容
    time_t ACFrozenTime[8];        //交采冻结时间
    uint8 encryptFailCnt;       //转加密任务一块表失败次数
    ENCRYPT_STATUS_E encryptStatus;//转加密任务一块表所处的状态
    uint16 encryptMsgIndex;     //发送的消息索引
    uint8 encryptMetNo[8];      //表号
    uint32 encryptCounter;      //计数器
    OOP_OCTETVAR512_T key;      //密钥包
    OOP_OCTETVAR512_T sessionKey;//会话密钥包
    uint8 encryptR2[4];     //645表身份认证后获取到的R2
    uint8 Ispatch;          //是否是补抄方案
    uint32 patchtime;
    uint8 PatchNextTurn;
    struct CTS_RUN_TASK_T_  *next;
}CTS_RUN_TASK_T;
//线程参数结构
typedef struct
{
    LockQueue_T* RecvBuf;                       //接收缓冲区结构指针
    LockQueue_T* SendBuf;                       //发送缓冲区结构指针
} ThreadArg_T;
typedef struct
{
    LockQueue_T* RecvBuf;                       //接收缓冲区结构指针
    LockQueue_T* SendBuf;                       //发送缓冲区结构指针
    LockQueue_T* CbSendBuf;                     //接收抄表数据缓冲区结构指针

} Down_ThreadArg_T;

typedef enum
{
    MFCS_IDLE = 0,                              //空闲
    MFCS_PAUSE,                                 //暂停
    MFCS_PAUSE_DELAY,                           //暂停延时
    MFCS_SET_TIME,                              //设置时间
    MFCS_READ_AMOUNT_FILE,                      //读取从节点数量
    MFCS_READ_INFO_FILE,                        //读取从节点信息
    MFCS_RESULT_DEAL,                           //结果处理
} METER_FILE_CMP_STATE_T;                       //电表档案比对状态字
typedef enum
{
    RTS_IDLE = 0,                               //空闲
    RTS_PAUSE,                                  //暂停
    RTS_PAUSE_DELAY,                            //暂停延时
    RTS_SET_TIME,                               //设置时间
    RTS_READ_MODULE_STATE,                      //查询模块状态
    RTS_READ_DELAY,                             //查询延时
    RTS_SEND,                                   //发送
    RTS_RECV,                                   //接收
    RTS_RETRY,                                  //重试
    RTS_RESUME,                                 //恢复
    RTS_485_SET,                     //新增的485参数设置状态
}RELAY_TRANS_STATE_T;

typedef enum
{
    EVENT_INIT =0,
    EVENT_GET_STATE,        //645抄读状态字
    EVENT_GET_EVENTLIST,        //698抄读3320对象 获取事件列表
    EVENT_GET_EVENT,        //依次抄读每个事件
    EVENT_SAVE_DATA,            //数据存储
    EVENT_RESET_STATE,      //645复位上报状态字
    EVENT_ONEMTR_END,       //一块表结束 抄下一块表的事件
}EVENT_REPORT_STATE_T;

typedef enum
{
    TMN_TO_METER_IDLE = 0,
    TMN_TO_METER_NEW,
    TMN_TO_METER_GET_DELAY,          //获取广播延时
    TMN_TO_METER_RDINIT_FIRST,         //终端首次对电表查询时钟初始化
    TMN_TO_METER_RDWAIT_FIRST,         //终端首次对电表查询时钟的等待
    TMN_TO_METER_FIRST_OVERTIME,       //终端首次对电表查询时钟超时等待
    TMN_TO_METER_FIRST_SUC,            //终端首次对电表查询时钟成功标志

    TMN_TO_METER_SETTIME_INIT,         //终端对电表设置时钟的初始化
    TMN_TO_METER_SETTIME_WAIT,         //终端对电表设置时钟的等待
    TMN_TO_METER_SETTIME_OVERTIME,     //终端对电表设置时钟的超时
    TMN_TO_METER_SETTIME_SUC,          //终端对电表设置时钟的成功

    TMN_TO_METER_RDINIT_SEC,           //终端再次对电表查询时钟初始化
    TMN_TO_METER_RDWAIT_SEC,           //终端再次对电表查询时钟的等待
    TMN_TO_METER_SEC_OVERTIME,         //终端再次对电表查询时钟超时等待
    TMN_TO_METER_SEC_SUC,              //终端再次对电表查询时钟成功标志
    TMN_TO_METER_SETTIME_CIPHER,        //对物联网表密文广播校时
} TMN_TO_METER_STAT; //终端对电表对时任务状态

#ifdef AREA_FUJIAN
typedef enum
{
    BROAD_TASK_IDLE = 0,
    BROAD_TASK_END_TASK,           //结束当前任务
    BROAD_TASK_GET_DELAY,          //获取广播延时
    BROAD_TASK_REPORT_DELAY,       //上报广播延时
    BROAD_TASK_REVISE_MESS,        //下发修正后的延时
    BROAD_TASK_REPORT_FINISH,      //上报广播完成
} BROAD_TASK_STAT; //广播任务状态
#endif

typedef enum
{
    MDL_METER_IDLE=0,
    MDL_METER_SEND,
    MDL_METER_RESPONSE,
    MDL_METER_RETRY,
    MDL_METER_END,
}MDL_METER_DEAL_T;

typedef struct
{
    METER_FILE_CMP_STATE_T      MeterFileCmp;       //电表档案比对状态字
    METER_FILE_MODIFY_STATE_T   MeterFileModify;//电表档案修改状态字

    INNER_RELAY_TASK_STATE_T    RelayTrans;      // 中继任务转发
    INNER_RELAY_TASK_STATE_T     EventReportState;    //事件上报状态字
    RELAY_TRANS_STATE_T         NormalAmr;      //日常抄表
    RELAY_TRANS_STATE_T         PatchAmr;       //补抄
    MDL_METER_DEAL_T            MdlGetDataDeal;
    INNER_RELAY_TASK_STATE_T           SyncTimeState;      //广播对时任务状态
    METER_FILE_MODIFY_STATE_T       ReNetworkingState;      //重新组网任务状态
    INNER_RELAY_TASK_STATE_T    FileTransfer;           //文件代发状态
    INNER_RELAY_TASK_STATE_T   GetNodeVersionInfoState;     //获取模块版本信息状态
    INNER_RELAY_TASK_STATE_T    meterTimeState;     //电表上报上电事件后的时钟管理状态机
    INNER_RELAY_TASK_STATE_T    meterTimeDelay;
#ifdef AREA_HUNAN
    INNER_RELAY_TASK_STATE_T   GetSTACurveState;     //获取STA模块配置状态状态
#endif
}TOTAL_STATE_T;                                 //总状态字结构体
typedef enum
{
    METER_NULL = 0,
    METER_PLC,                                  //PLC电表
    METER_485,                                  //485电表
}MTR_TYPE_T;        //电表类型

typedef struct
{
    uint8 dicount;
    uint16 csdindex;
    uint16 mapindex[4];
    uint32 diinfo[4];
}DI_645_INFO_T;

#define MAX_DI_645_NUM  7    //定义多帧645抄读 一次性最多的数据项个数

typedef struct
{
    uint8        DI[4];
    OOP_OAD_U    MainOad;
    uint8        SubOadNum;
    OOP_OAD_U         SubOad[MAX_OAD_NUM];
     DI_645_INFO_T diinfo[MAX_DI_645_NUM];    //一次最多抄5个oad
#ifdef AREA_HUNAN
    uint8  stacurveflag;        //标记是否使用模块曲线抄读
#endif
}DI_INFO_T;

typedef struct
{
    uint8       ValidFlag;                 //1:有效; 0:无效
    uint8       NeedSendSeg;                //1 需要发送分帧确认帧
    uint8       FrameSeq;                  //F1-01帧序号
    uint16      Pn;                        //该帧对应的测量点号
    uint8       bleport;                    //蓝牙端口
    int         MsIndex;                   //该帧对应的方案MS索引
    uint8       PrtlMode;                  //对应的表规约
    MTR_TYPE_T  Mettype;                   //节点类型
    uint8       MeterAddr[6];              //电表地址
    uint8       AcqAddr[6];                //采集器地址
    time_t      CommBeginTime;             //该帧通讯开始时间
    DI_INFO_T   DiInfo;
}FRAME_SEQ_INFO_T;                    //帧序号信息
#define PARALLEL_QUEUE_NUM  10   //国网并发个数
typedef struct
{
    uint8   QueueCnt;                                //队列有效个数
    uint8   usetype;            //0表示正常抄表使用 1表示补抄使用
    FRAME_SEQ_INFO_T Queue[PARALLEL_QUEUE_NUM];     //通讯数据队列
}SendFrameQueue_T;

typedef struct
{
    uint8   AmrBusyFlag         :1;             //抄表忙/闲标志
    uint8   AmrFinishFlag       :1;             //抄表任务时段完成标志
    uint8   ReserveBit3         :1;             //保留
    uint8   ReserveBit4         :1;             //保留
    uint8   ReserveBit5         :1;             //保留
    uint8   ReserveBit6         :1;             //保留
    uint8   ReserveBit7         :1;             //保留
    uint8   ReserveBit8         :1;             //保留
}AMR_WORK_STATE_BIT_T;

 typedef union
{
    AMR_WORK_STATE_BIT_T    BitSect;
    uint8                   Word;
}AMR_WORK_STATE_T;                              //抄表工作状态


typedef struct
{
    uint16        SlaveCnt;                     //从节点数量
    uint16        MaxAllowSlaveCnt;             //最大允许从节点数量
    uint16        MtrCnt;                       //电表总数
    uint16        MtrPlcCnt;                    //PLC电表总数
    uint16        Mtr485Cnt;                    //485电表总数
    uint16        CurMtrIndex;                  //当前电表索引
    uint8         AcqUnitFailureFlag;           //采集器失败标志
    uint16        AmrSucceedCnt;                //抄表成功块数
    uint8         OftenPriority;                //频繁任务执行最低优先级
    AMR_WORK_STATE_T    AmrWorkState;           //抄表工作状态
    uint8         SucRateState;                 //日冻结成功率统计状态 0:全部完成 1: >=95% 2: <95%
    uint8         CurrentTaskId;                //当前抄读的任务
    uint8         CurrentLogicid;
    uint16        collectorIndex;               //采集器索引
} TmnAmrInfo_T;

//抄读信息
typedef struct
{
    uint8   nRelayRouterLevel;  //中继路由级数
    uint8   nChannelIdentifier;  //信道标识
    uint8   nCarrierReadPhase;  //载波抄读相位
    uint8   nCarrierSignalQuality;  //载波信号品质
    uint8   nCarrierSymbol;  //载波标志
    uint8   aReserveForOtherUse[3];  //保留
}AMR_PLC_READING_INFO_T;


typedef struct
{
    uint8   logicid;
    uint16  Pn;                                 //测量点号
    uint8   blePort;         //对应的蓝牙端口号
    uint8   SpecialFlag;                        //特殊标志，有2个功能:
                                                    //1:下发到路由板标志(0:未设置 / 1:已设置)
                                                    //2:抄表成功标志(0:失败 / 1:成功)
    uint8   flag645;         //标记645表该用什么方式抄读 （单相表为1表示返回时标不对，此时用实时转 2表示表不支持）
    uint8   flag645change;  //当645表由于通信问题偶尔不通时会触发实时转冻结逻辑 如果转完实时还是失败 就再切回来
    uint16  LastTaskId;                         //最近一次抄读数据所属的方案号
    BOOL    IsNeedGetSafeMode;                  //是否需要读取面向对象安全模式开关
    uint8   BroadFailCnt;                       //抄读失败计数
    uint8   CSDIndex;
    uint8   tmpcsdindex;
    uint8   SigFrameNum;
    uint8   ROADIndex;
    uint16   MapIndex;
    time_t  StartTime;                             //曲线遍历起始时间
    uint8   CurNum;                               //时段搜索当前次数
    uint8   EndNum;                     //
    DateTimeBcd_t  storeTm;    //645冻结时标
    uint8       SpecialTmpBuf[MAX_SPECIAL_TMP_LEN]; //合并645数据时，临时存放第一个标识的数据
    uint8   SpecialTmpLen;
    uint8   IsNeedPatch;            //是否需要补抄
    BOOL    LastResult;             //最近一次抄表成功状态
    uint16  AllFailCnt;             //连续抄读失败次数
    uint16  roundFailCnt;           /* 连续轮次（任务周期）失败次数 */
#ifdef AREA_SHANDONG
    uint16  calcTimeOffsetCnt;      /* 计算电表时钟偏差次数 */
    time_t  timeOffset;             /* 电表时钟偏差 */
    uint8   NILMModuleInfo;         //该电表是否查询过40310200 0没查过 1说明有非介入式负荷辨识模组要用逻辑地址3 2说明没有不用抄48000300和48000400
    uint8   logicid_5_info;         //改电表是否有逻辑地址5的模块 抄谐波电压电流用 1 有 2没有这个模块
#endif
    time_t  LastSendTime;           //最近一次发送时间
    time_t  LastRecvTime;           //最近一次接收时间
    time_t  LastFailTime;           //最近一次失败时间
    AMR_PLC_READING_INFO_T  ReadingInfo;    //抄读信息
    uint8  phase;       //相位   enum{未知（0），A相（1），B相（2），C相（3）}，
    uint8  phaseseq;        //相序异常  enum 正常（0），LN互易（1），逆相序（2
#ifdef AREA_JIANGXI
    uint8  reversePhase;        //通过抄读电表运行状态字的方式判断是否为电压/电流逆相序, enum{正常(0), 逆相序(1)}
    uint8  zeroCurrentFlag;     //零线电流超差标志
    uint8  zeroCurrentAbnormal; //零火线不平衡计数
#endif
    uint32  TimeError;      //电表时钟误差
    uint8  GetTimeIndex;    //645表查询时钟需要发两帧
    //uint8   MeterTime[6];  //645电表时钟 年月日时分秒
    uint16  TimeDelay;      //每个电表的从节点监控延时
    OOP_DATETIME_S_T  MeterTime;
    time_t  powerofftime;       //如果有上报停电事件 则记录下来 以供上电事件使用
    uint32  powereventseq;      //停上电序号 收到上电事件时要和停电事件一个序号
    uint8   powerstate;         //停电状态 如果发生过停电则置1 收到上电事件后判断是否停电过
    uint8   ReNetworkingCount;     //重新组网次数 最高3次
    time_t  LastReNetwokingTime;      //上次重新组网时间
    uint8   DayFrozenFlag;      //该表日冻结成功标志
#ifdef AREA_HUNAN
    DateTimeHex_t  temperaturetime;    //湖南断路器温度告警事件发生时间
    uint32  temperatureseq;     //事件序号
    uint8  temperaturestate;    //发生1 恢复后0
    uint8   stacurvestate;      //节点STA配置状态 1成功 0失败
#endif
} MtrAmrInfo_T;                                 //测量点抄表信息
typedef struct
{
    AcqType_t Acqtype;
    uint8     LastN;
    OOP_TI_T      TI;
}AcqInfo_t;





typedef struct _MS_INFO
{
    uint8       Prtl;          //规约类型
    uint8       Pipe;          //端口号
    uint16      Sn;            //序号
    uint8       CSDIndex;      //CSD索引信息
    uint8       SigFrameNum;   //CSD索引信息
    uint16      MapIndex;      //列表搜索索引记录(645表队列索引)
    uint8       RoadIndex;     //任务中多个ROAD时，当前索引记录(事件)
    uint8       CurNum;        //时段搜索当前次数
    uint8       EndNum;        //时段搜索最大次数，为0代表时标错误，为FF代表需要实时转存
    time_t      StartTime;     //曲线遍历起始时间
    DateTimeBcd_t  storeTm;    //645冻结时标
    uint8       SpecialTmpBuf[MAX_SPECIAL_TMP_LEN]; //合并645数据时，临时存放第一个标识的数据
    OOP_OCTETVAR256_T key;  //转加密用 秘钥密文
}MS_INFO;//电表相关信息参数
typedef struct _CSD_TASK_INFO
{
    OOP_OAD_U            MainOad;                  //任务类型(00000000 实时任务 50040200 日任务等)
    uint16          SubOadNum;                //子OAD数量(主要用于抄读OOP类型表信息使用,以及存储07,97时用OAD进行存储)
    OOP_OAD_U     OadListInfo[MAX_OAD_NUM]; //
}CSD_TASK_INFO;//方案任务参数
typedef struct _OAD_ID_INFO
{
    OOP_OAD_U            SubOad;           //SubOad源数据
    uint16          OAD_ID;           //对应的OAD字典信息
}OAD_ID_INFO;//OAD对应的字典枚举信息

typedef struct _TASK_INFO
{
    uint16            TotalSubOadNum;                          //子oad总数量
    uint16            RoadNum;              //ROAD数目
    CSD_TASK_INFO     RoadTask[MAX_EVENT_ROAD_NUM];     //ROAD信息
}TASK_INFO;//事件方案任务参数


typedef struct _SCHEME_TASK_RUN_INFO
{
    uint8                         logicid;
    uint8                         MinTaskFlag;      //分钟级任务标记
    uint8                         FiveMinTaskFlag;      //5分钟任务标记
    uint16                        TaskId;                         //任务编号
    ePlanType                     SchemeType;                     //采集方案类型
    uint8                         SchemeId;                       //方案编号
    uint8                         Priority;                       //任务优先级
    AcqInfo_t                     AcqInfo;                        //采集类型(6014协议定义)
    RUN_RESULT                    RunResult;                      //任务执行结果
    StoreTimeType_t               StoreTimeType;                  //存储时间类型，其他采集方案使用未定义(0)
    //DateTimeBcd_t                 storeTm;                        //存储时间
    //uint16                        SucPlcNum;      //采集成功测量点个数
    uint16                        TotalNum;                       //测量点总个数
    uint16                        MeterIndex;     //当前执行电表索引
    MS_INFO                       MsInfo[MAX_AMR_COLLECT_CNT];    //电表集合信息参数
    TASK_INFO               CsdTask;                        //普通方案电表任务
    OOP_TRANS_INFO_T        TransInfo[OOP_MAX_TRANS_INFO];      //透明方案内容 和MsInfo对应
    uint8                   EncryptPlanID;
    uint8                   EncryptPlanType;    //转加密方案类型 0：普通任务数据；1：对时；2：设参；3：升级；其他：保留。
    OOP_ENCRYPT_DATA_T      EncryptInfo;           //转加密方案内容
    uint16                        DelayTime;                      //延时时间数值
    uint8                         DelayTimeUnit;                  //延时时间单位
    BOOL                          rptFlag;                        //事件采集方案上报标识
    uint8                    transindex;                         //透明方案新添加的一组报文索引;
    #ifdef AREA_SHANDONG
    BOOL                    NILMFlag;           //任务是否有抄负荷辨识模块
    BOOL Isenergyoad;
    uint8 energynum;
    BOOL IsFreezeTime;
    #endif
    //struct _SCHEME_TASK_RUN_INFO  *next;                          //指向下一个任务的指针
}SCHEME_TASK_RUN_INFO;//方案任务的结构

typedef struct _PRO698_TASK_CFG
{
    uint8   logicid;
    uint8   TaskId;                 //任务id
    uint16  RunCycle;               //执行频率数值
    uint8   RunCycleUnit;           //执行频率单位
    ePlanType SchemeType;           //采集方案类型
    uint8   SchemeId;               //方案编号
    OOP_DATETIME_S_T    RunBeginTime;        //任务执行开始时间
    OOP_DATETIME_S_T    RunEndTime;          //任务执行结束时间
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
}OOP_TASK_CFG;

typedef struct
{
    pthread_rwlock_t    rwLock;                  //读写锁 多线程访问
    TASK_CFG_STATE      TaskCfgState;         //任务档案状态
    int                 ProCfgModifyDelaySec;  //方案配置修改延时
    time_t              FunLastRunTime;        //方案上次修改时间
    uint8               TaskNum;               //任务总个数
    OOP_TASK_CFG        OopTaskCfg[255];       //任务具体参数
}OOP_TASK_CFG_LIST;




typedef enum
{
    FRONTCLOSE_BACKOPEN = 0,  //前闭后开
    FRONTOPEN_BACKCLOSE,      //前开后闭
    FRONTCLOSE_BACKCLOSE,     //前闭后闭
    FRONTOPEN_BACKOPEN,       //前开后开
}REGION_TYPE;//区间状态



typedef enum
{
    PRTL_NULL = 0,
    PRTL_97,                                    //DLT/645-1997规约类型
    PRTL_07,                                    //DLT/645-2007规约类型
    PRTL_OOP,                                   //面向对象规约电表类型
    PRTL_188,                                   //面向对象规约188协议
    PRTL_3762,
    PRTL_MBUS,
    PRTL_Dlms=201,
    PRTL_Cap =251,
    PRTL_Protector = 252,
    PRTL_20_IOT,            //20版和物联网表
    PRTL_UNKNOWN,           //未知协议
}PRTL_MODE_T;

typedef enum
{
    ACT_COMM_IDLE = 0,                          //空闲
    ACT_COMM_SEND,                              //发送
    ACT_COMM_RECV,                              //接收
    ACT_COMM_RETRY,                             //重试
    ACT_COMM_DELAY,                             //延时
}ACTIVE_COMM_STATE_T;                           //主动通讯状态字



typedef enum
{
    SEARCH_SUCCESS = 0,  //检索到要抄读的DI
    SEARCH_FINISH,       //所有任务中未检索到要抄读的DI
    SEARCH_FAIL,         //情况1:当前时段任务中未检索到要抄读的DI，但后续时段任务中有
                         //情况2:高优先级任务成功率未达到，暂时不允许切换其他任务
}DI_SEARCH_STAT;//DI检索结果

typedef struct
{
    int result;
    uint16 len;
    uint8 buf[200];
}DATA_PRTL_645_T;

typedef struct
{

    uint8    DelayFlag;                         //组帧时的通信延时标志
    uint16   DelayCnt;                          //通信延时时常
    MTR_TYPE_T   MtrType;                       //电表类型(1:plc/2:485)
    uint8   PrtlMode;                           //规约类型
    uint8   AcqUnitAddr[6];                     //采集器地址
    uint8   MtrAddr[6];                         //电表地址
    uint8   CtrlCode;                           //控制码
    uint16   DataLen;                            //数据域长度
    uint8   DataBuff[MAX_MSG_LEN];                      //数据域内容
    uint8   Istasktable;                         //
    uint16  Pn;
    uint8   framenum;                           //645多帧抄读 回来的报文个数
    DATA_PRTL_645_T frameinfo[10];              //对应每个报文的数据
}MeterPrtlFmt_T;                                //电表规约格式


//上报抄表数据
typedef struct
{
    uint32   nPortID;  //端口号
    uint16   nMeterFilesID; //档案序号
    uint8   addr[6];
    uint8   nProtocolType;  //协议类型
    uint16  nDataBufLen;    //数据帧长度
    uint8   pDataBuf[MAX_MSG_LEN];  //数据帧
    uint8   isExitReadingInfo;      //是否有抄读信息 1 有 0没有
    AMR_PLC_READING_INFO_T stPlcReadingInfo;  //抄读信息
}AMR_PLC_INFO_REPORT_T;




//一个电表的若干个对象属性
typedef struct
{
    OOP_TSA_T Mtraddr;
    BOOL IsNeedGetSafeMode;    //面向对象表是否需要获取安全模式
    uint16 OverTime;
    uint16 OadNum;
    OOP_OAD_U SubOad[OOP_MAX_CSDS];
    //uint8 Result[OOP_MAX_CSDS];  //每个数据项穿透结果 1表示成功 0表示失败
    time_t SendTime;     //该表第一帧发出的时间
}AMR_NORMAL_LIST_T;

//一个电表的若干个对象属性
typedef struct
{
    OOP_TSA_T Mtraddr;
    uint16 OadNum;
    OOP_OAD_U SubOad[OOP_MAX_CSDS];
    OOP_OAD_U SetOad[10];
    OOP_OAD_U GetOad[10];
    //uint8 Result[OOP_MAX_CSDS];  //每个数据项穿透结果 1表示成功 0表示失败
    uint8  setaction[2000];
    uint16 setlen;
    uint8  relaydata[2000];
    uint16 relaydatalen;
    BOOLEAN VliadMeter;       //该表是否有效,有效置1，无效置0;
    uint16 overtime;        //超时时间
    uint8 sendcnt;              //发送次数
    uint32 firstSendtime;       //该表首次发送时间
}PROXY_METER_LIST_T;

typedef struct
{
    uint8 isValid;  //标记当前是否有穿透任务 0没有 1 有
    uint8 RelayResult;  //结果 0失败 1 成功
    uint32 IP;        //发送方ip和端口
    uint16 port;

    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID;
    uint16 IOP;
    uint16 DownIOP;
    uint16 Sendindex;   //下行发送的序号
    OOP_COMDCB_T comdcb;//端口通信控制块
    uint16 OverTime;     //报文超时时间 秒
    uint16 ByteOverTime;    //字节超时时间 毫秒
    uint8  MtrAddr[6];     //电表地址
    uint8 isBroad;          //是否为广播校时帧
    uint8 prtl;         //广播帧的规约
    time_t  BroadTime;      //广播校时的时间点
    uint16  BroadDelay;         //广播延时
    TMN_TO_METER_STAT   BroadStat;  //广播校时状态

    uint8 piid; //代理的piid
    uint8 proxychoice;//代理的类型
    uint16 AmrNum;           //0x0010 电表个数
    uint16 TotalNum;           //0x0010 电表个数
    PROXY_METER_LIST_T METER_LIST[100];
    uint8 ValidNum;             //有效的的电表数量
    AMR_NORMAL_LIST_T AmrNormal[100];    //0x0010 请求代理读取若干个服务器的若干个对象属性 暂支持100个
    uint8 AmrIndex;    //穿透到的电表索引
    uint8 OadIndex;     //oad索引
    uint16 MapIndex;     //对应645的索引
    uint8 OopSendNum;   //oop表发送的oad个数
    uint8 SendDI[4];     //645发送的的di
    uint8 SpecialTmpBuf[MAX_SPECIAL_TMP_LEN];
    uint8 SpecialTmpLen;
    CSD_TASK_INFO Road;
    uint32 RelayLen;
    uint8 RelayData[MAX_MSG_LEN];
    uint32 OadPort;//代理的端口，可以用于0010消息和0907的代理和福建的0950智能代理
    uint8 priority; //智能代理优先级 用于福建的智能代理
    uint8 frametype;//用于代理的0907，标记发送的报文是698还07;
    OOP_TSA_T tsa;//用于福建的智能代理通信地址
    uint8 objiecttype; //通信对象类型 用于福建的智能代理
    uint8 reqframeformat;//下行报文格式字 用于福建的智能代理
    uint8 resframeformat;//响应报文格式字 用于福建的智能代理
} RELAY_TRANS_T;

//搜表结果存储用
typedef struct
{
    uint16 infonum;
    uint8 prtl;
    uint8 Addr[6];
    uint8 pipe;
}METER_ADDR_AND_PRTL_T;

//搜表结果存储用
typedef struct
{
    uint16 infonum;
    uint8 Addr[6];
    uint8 pipe;
}CCO_STA_VER_T;


typedef struct
{
    uint8    IsInSearch;            //当前是否在搜表中 1表示在搜表 0表示没有;
    uint32   SearchTime;                     //搜表持续时间 分钟 如果持续时间为0则不需要下发停止搜表命令 但需要主动查询模块的搜表状态
    time_t   SearchStartTime;                   //搜表开始时间
    //uint16   MeterCount;                    //已经搜到的电表个数
    //METER_ADDR_AND_PRTL_T SearchResult[MTR_MAX_NUM];  //已经搜到的电表 用来去重用
}SEARCH_METER_T;//搜表结构体
//补抄用结构体
typedef struct
{
    uint16  Pn;             //电表测量点号
    uint8   flag;       //是否需要补抄 默认为0 1为抄读成功无需补抄
}PATCH_METER_T;

typedef struct
{
    time_t  PatchTime;       //补抄时间点
    uint16  MeterNum;
    //PATCH_METER_T MeterInfo[MAX_AMR_COLLECT_CNT];
    uint16  Pn[MAX_AMR_COLLECT_CNT];
    uint8   flag[128];//按位表示每个测量点的成功状态 默认为0 1为抄读成功无需补抄
}PATCH_TIME_T;

typedef struct
{
    uint8 Time[3]; //年月日BCD码 0x190x110x09
    char FileVersion[5];      //标记补抄文件版本 版本不一样的会删除 "XL-1"
    uint8 TaskId;
    uint8 TimeNum;
    PATCH_TIME_T TimeInfo[96];
}PATCH_TASK_T;

typedef struct
{
    uint16 TaskId;
    uint16 NeedMtrNum;
    uint8 TimeIndex;    //该任务抄到的时间index 对于补抄任务一天所有需要抄的时间点
    uint16 SucNum;
    uint16 MeterIndex;      //对应补抄结构中的索引
    RUN_RESULT RunResult;
    time_t StartTime;
    uint8 MtrResult[128];//按位表示每个电表索引是否抄读成功 1表示抄完 顺序为补抄结构中的电表顺序

}PATCH_TASK_RUN_T;//当前补抄任务运行参数

typedef struct
{
    uint8 logicid;
    uint16 Pn;

    uint8 FailCnt;
    uint8 CSDIndex;
    uint8 SigFrameNum;
    //uint8 ROADIndex;
    uint8 MapIndex;
    uint8 CurNum;
    uint8 EndNum;
    time_t StartTime; //补抄起始时间
    time_t EndTime;
    DateTimeBcd_t  storeTm;    //645冻结时标
    uint8   SpecialTmpBuf[MAX_SPECIAL_TMP_LEN]; //合并645数据时，临时存放第一个标识的数据
    uint8   SpecialTmpLen;
}PATCH_MTR_INFO_T;//补抄时电表信息

typedef struct
{
    uint8 count;    //每个事件需要抄读的数据项总个数
    OOP_OAD_U EventDI;     //事件主DI
    uint8 SubOadNum;
    OOP_OAD_U SubOad[MAX_OAD_NUM];
    uint8 CurDIIndex;   //当前抄到的索引
    uint32 AllDI[200];  //该事件需要抄读的所有数据项 和count对应
    time_t SendTime;        //发送时间
}EVENT_INFO_T;

typedef struct
{
    uint16 Pn;
    uint8 prtl;
    uint8 FailCnt;      //失败次数 超过三次就不抄
    uint8 ResetStateCount;        //645表复位状态字的次数 超过三次就不继续
    EVENT_REPORT_STATE_T state;//当前处于的状态
    //OOP_OAD_U Oad[200];        //3320中需要抄读的oad列表
    uint8 EventCount;   //需要抄读的事件个数
    EVENT_INFO_T EventInfo[96]; //每个事件需要抄读的数据项
    uint8 CurIndex;     //当前抄到的事件索引
    uint8 ReportState[12];     //645存储上报的状态字 供复位主动上报状态字时用
    uint8 DataBuf[MAX_MSG_LEN];     //存到数据库中的数据 645为 SEQUENCE OF octet-string格式 octet-string为完整的645帧 第一个是状态字
    uint16 DataLen;
}MTR_EVENT_REPORT_T;//电表事件上报时使用 记录每个电表的状态

typedef struct
{
    uint8 AllCount;     //当日事件上报总次数 过日清零 一天最多处理200个上报
    uint16 CurIndex;        //当前处理到的电表index
    MTR_EVENT_REPORT_T  MtrEvent[MAX_EVENT_MTR];  //记录每个上报事件的电表信息
    uint16 SendIndex;
}EVENT_REPORT_T;        //事件上报结构体

typedef struct
{
    uint16 Pn;
    uint8 prtl;
    uint16 datalen;
    uint8 data[MAX_MSG_LEN];
}EVENT_MSG_T;

typedef struct
{
    uint8 TimingType;   //0 空闲 1 广播校时 2 单地址广播校时
    int8 limit;         //单地址广播校时误差阈值
    uint16 MtrIndex;    //线程中电表索引 当前对时的电表
    TMN_TO_METER_STAT state;//所处状态
    uint8 BroadPrtl;    //广播时使用的规约 判断当前档案中是否有该规约
    uint16 FrameIndex;      //发送帧序号
    uint16 BroadDelay;      //广播延时
    uint8 FailCnt;      //失败次数
}BROAD_TIMING_T;    //广播校时参数

#ifdef AREA_FUJIAN
typedef struct
{
    BROAD_TASK_STAT state;//所处状态
    uint16 FrameIndex;      //发送帧序号
    uint16 BroadDelay;      //广播延时
    uint8 FailCnt;         //失败次数
    BOOL  GET_DELAY;
    time_t sendTime;

}BROAD_TASK_T;            //广播任务参数
#endif
//对应设备文件代发 1003-0020
#define MAX_UPDATE_METER_NUM   20
typedef enum
{
    ADDR_SINGLE = 0,    //单地址
    ADDR_GROUP,         //组地址
    ADDR_BROAD          //广播地址

}ADDR_FLAG;

typedef enum
{
    UPDATE_INIT = 0,
    UPDATE_WAITDOWNLOAD,
    UPDATE_DOWNLOADING,
    UPDATE_DOWNLOAD_SUCC,
    UPDATE_CHECK_SUCC,
    UPDATE_WAITING,
    UPDATE_SUCC,
    UPDATE_HARD_NOT_MATCH = 20,
    UPDATE_SOFT_NOT_MATCH,
    UPDATE_WRITE_FLASH_ERROR,
    UPDATE_READ_FLASH_ERROR,
    UPDATE_WRITE_PROM_FLASH_ERROR,
    UPDATE_COMPLETE_CHECK_ERROR,
    UPDATE_TRUE_CHECK_ERROR,
    UPDATE_TIME_ERROR,
    UPDATE_UNKNOW = 254,
    UPDATE_FAIL_OTHER_REASON

}UPDATE_RESULT;
typedef struct
{
    ADDR_FLAG AddrFlag; //地址类型
    uint8 Addr[6];      //设备地址
    uint8 pipe;     //端口
    UPDATE_RESULT UpdateResult; //升级结果
    uint8 CheckResult;  //所有文件是否传输成功 1表示全部成功 查询F0010400后获取
    uint16 FrameResendCount;
    uint16 FrameResendSeq[100];
    uint16 FrameResendIndex;
    uint16 FrameIndex;
    uint8 FainCnt;      //每块表的失败次数 超过3次不再重试
    uint8 CheckCnt;     //以防每次检查都有没传完的 只检查3次
}UPDATE_METER_INFO_T;

typedef enum
{
    FILE_INIT = 0,
    FILE_START, //启动传输
    FILE_SEND , //文件发送
    FILE_CHECK,     //发送完毕后检查状态字
    FILE_RESEND,     //补发
    FILE_READ,      //读取电能表升级结果
    FILE_ENCRYPT_INIT,  //转加密初始化
    FILE_ENCRYPT_COUNTER,   //读取计数器
    FILE_ENCRYPT_CONNECT,   //应用连接
    FILE_ENCRYPT_UPDATE,    //启动升级
    FILE_DELAY,             //延时
}FILE_TRANSFER_STATE;

typedef struct
{
    //OOP_VISIBLEVAR128_T FilePath;       //文件路径
    //uint32  FileSize;                   //文件大小 字节
    //uint32  FrameSize;                  //每帧传输大小 字节
    uint16  MeterNum;                   //设备个数
    UPDATE_METER_INFO_T MeterInfo[MAX_UPDATE_METER_NUM];//  设备集合

    OOPM_STARTTRANS_T FileInfo;          //文件信息
    BOOL              bEncode;           //是否安全传输

    FILE_TRANSFER_STATE FileState;      //文件发送的状态
    uint16  MeterIndex;  //根据所处状态判断设备索引
    uint16  FrameCount;     //根据文件大小和每帧大小计算出需要发多少包
    uint16  SendMsgIndex;   //发送的消息索引用于判断回码
    //uint16 FrameIndex;    //发送的文件索引
    uint8 isBroad;      //是否广播 485用
    uint8 startcnt;     //启动传输要发三遍
}UPDATE_INFO_T;

typedef struct
{
    uint16 BeginIndex;      //起始序号
    uint8 FailCnt;      //失败次数
    uint16 SendSeq;     //发送消息序号
}GET_NODE_VERSION_INFO_T;

typedef struct tag_STA_Curve_State_Get_Info
{
    uint16 beginseq;    //查询起始序号
    uint8 failcnt;      //失败次数
    uint16 sendseq;     //发送消息序号
}STA_CURVE_STATE_GET_INFO_T;

typedef struct
{
    uint16 datalen;
    uint8 databuf[1000];
}DATA_BUF_T;

typedef struct
{
    uint8 isValid;
    uint16 pn;
    uint32  starttime;
    uint32   storeTm;
    OOP_OAD_U mainOad;
    uint8  subnum;
    OOP_OAD_U subOad[JS_OOP_MAX_OADS];
    DATA_BUF_T dataBuf[JS_OOP_MAX_OADS];
}DATA_CACHE_T;      //缓存每个表的数据

#ifdef AREA_FUJIAN
#define MAX_TRANS_TASK_NUM  50    //最多支持的全局透明任务方案个数
#define MAX_SPECIFIC_ADDR_NUM 255   //每个具体任务支持的最大表个数
#define MAX_SPECIFIC_TASK_NUM  500  //最多支持的具体任务个数
#define MAX_METER_NUM         1000      //一个任务方案下最多支持的所有的表个数
typedef struct
{
    uint16 num;
    OOP_TSA_T addr[MAX_SPECIFIC_ADDR_NUM];
    uint8 result[MAX_SPECIFIC_ADDR_NUM];       //记录下每个表的抄表结果
    uint8 retryCnt[MAX_SPECIFIC_ADDR_NUM];    //立刻重试次数
    uint8 isReport[MAX_SPECIFIC_ADDR_NUM];      //当前轮次是否上报过
}FUJIAN_ADDR_LIST_T;

//自定义的具体任务单元 会用链表来找到每个任务方案对应的所有具体任务
typedef struct tag_FUJIAN_SPECIFIC_TASK
{
    uint16              transTaskid;    /**< 透明任务方案号                    */
    uint16              nodeid;         /**< 具体任务序号                    */
    FUJIAN_ADDR_LIST_T  addrAll;        /**< 通信地址集合          */
    OOP_COMDCB_T        uartParam;      /**< 端口通信控制块          */
    uint8               reqMessType;    /**< 请求报文格式字 OOP_MESS_FORMAT_E          */
    OOP_OCTETVAR2K_T    reqMess;        /**< 请求报文内容                             */
    uint8               resMessType;    /**< 响应报文格式字 OOP_MESS_FORMAT_E          */
    uint16              resMaxLen;      /**< 响应报文预计最长长度                    */
    uint8               resMinLen;      /**< 成功响应报文最短有效长度                  */
    OOP_RECV_MESS_PROC_T resMessProc;   /**< 返回报文验证处理方式                    */
    struct tag_FUJIAN_SPECIFIC_TASK *next;  //指向同一个任务方案的下一个具体任务
}FUJIAN_SPECIFIC_TASK_T;

typedef struct
{

    OOP_TSA_T meterAddr;
    uint16 specificTaskIndex;     //该表抄到的具体任务索引
    uint8  meterResult;         //这表有没有抄完 0初始态 1已经预告过了 2抄完了 3失败了
    uint8  failCnt;         //并发抄表用 记录下失败次数
    uint8  triggerFlag;         //专为触发方案任务触发后执行的具体任务对象为1时使用 标记哪些表被触发了
    time_t  execTime;       //记录下抄表时间 如果该表执行失败 则后面的具体任务里这块表的透明结果里抄表时间也填这个
    uint8  multiBufIndex;    //专为多帧645用
}FUJIAN_METER_INFO_T;

typedef enum
{
    NOTICE_INIT = 0,    //初始态
    NOTICE_OBJECT,      //预告抄读对象
    //NOTICE_START,       //启动预告任务执行
    NOTICE_WAIT,        //等申请通信对象和上报结果

}FUJIAN_NOTICE_STATE_E;
//每个线程记录下的抄表信息 对应每一个任务
typedef struct tag_FUJIAN_TASK_RUN
{
    uint16 taskid;
    uint8  changeFlag;           //任务线程如果收到变更的会给置位 抄表线程不会执行该任务 1表示正常 2表示修改 3表示删除
    uint8  priority;            //任务优先级
    uint16 specificTaskIndex;   //记录下当前运行到表的第几个具体任务
    uint16 meterIndex;          //记录下当前运行到第几个表 对应meterNum
    uint8   isNewTurn;          //是否到新一轮
    uint16  sendMsgIndex;           //记录下发送的消息序号
    time_t  lastSendTime;           //上一次消息发送时间
    uint8  failFlag;            //标记有没有失败的表
    uint16 meterSucNum;         //并发抄表 成功的表个数
    uint16 meterNum;        //一个任务方案下所有电表个数 预告抄表和并发抄表用
    FUJIAN_METER_INFO_T meterInfo[MAX_METER_NUM]; //对应每个电表的信息 预告抄表和并发抄表用
    FUJIAN_NOTICE_STATE_E noticeState;          //预告抄表所处状态
    time_t                lastNoticeRecvTime;   //最近一次预告抄表上报的时间
    uint8                 noticeFailCnt;        //预告抄表失败次数
    RUN_RESULT  result;              //当前轮次是否执行成功
    AMR_PERIOD_STAT periodStat;       //当前时间是否允许任务执行
    OOP_DATETIME_S_T  thisTurnBeginTime;        //本轮开始时间
    OOP_DATETIME_S_T  thisTurnEndTime;          //本轮结束时间
    OOP_DATETIME_S_T  nextTurnBeginTime;          //下一轮开始时间 用来判断是否需要刷新时间
    OOP_DATETIME_S_T  thisTurnRetryTime;        //本轮失败重试时间
    OOP_DATETIME_S_T  triggerTime;              //针对触发任务 如果被触发了 就记录下触发时间 然后会计算任务开始和结束时间
    OOP_DATETIME_S_T  storageTime;              //任务存储时标
    //uint8             thisTurnRetryCnt;         //本轮失败重试次数 对应任务中的每轮失败重试次数
    uint8             retryCycle;               //失败后重试的轮次 对应任务中的最大失败重试轮次
    uint16            storageCnt;               //存储次数 从1开始 每轮次加一 到任务限定的存储次数后再从1开始 0表示不存储
    uint8          pauseFlag;       //扩展oad6E11FFFF 如果任务优先级改为254 则任务暂停
    OOP_TRANS_TASK_T *transTask;        //指向任务方案内容
    uint16 specificTaskNum;             //具体任务个数
    FUJIAN_SPECIFIC_TASK_T *specificTask;   //指向对应的第一个具体任务
    struct tag_FUJIAN_TASK_RUN *nextTask;   //指向下一个优先级的任务
}FUJIAN_TASK_RUN_T;

//每个线程的抄表信息
typedef struct
{
    uint8  isInited;        //是否已经初始化过
    uint16 taskNum;
    FUJIAN_TASK_RUN_T taskList[MAX_TRANS_TASK_NUM];
}FUJIAN_TASK_RUN_LIST_T;

#endif

typedef enum
{
    SS_READY_CHECK = 0,                                 //空闲
    SS_SEND_CHECK_PRE,                      //搜索前上次档案中电表测试发送
    SS_RECV_CHECK_PRE,                      //搜索前上次档案中电表测试接收
    SS_INIT_SEARCH,                         //搜表前初始化
    SS_READY_SEARCH,                            //搜索准备
    SS_SEND_SEARCH,                             //搜索发送
    SS_RECV_SEARCH,                             //搜索接收
    SS_OVER,                              //
} SEARCH_METER_STATE_T;
typedef enum
{
    Order_One = 0,                          //1
    Order_Two                               //2
} SEARCH_ORDER; //搜索轮次
typedef struct
{
    uint8 SerchNum;                             //正在进行到的搜索表数
    uint8 ClashMetNum;                          //有冲突的电表总的数目
    uint8 DoClashNum;                           //进行到第几个
    uint8 MetClash[MAX_SEARCH_METER_CNT];       //记录存在有重复地址的电表地址低位字节
    //INT8U MetStore[MAX_SEARCH_METER_CNT];     //记录已经存在的电表地址低位字节
    //INT8U FindMetNum;                           //找到合格报文的总个数
} SerchMet;

typedef struct
{
    SEARCH_ORDER    SOrder;                     //表示第几轮
    SerchMet        S_One;
    SerchMet        S_Two;
} SerchStruct;
typedef struct
{
    uint8       MeterAddr[6];               //电表地址
    uint8       PrtlId;                     //规约信息(1:97 /2:07/5:从模块/6:电源采集模块)
    uint8       Port;                       //抄表端口
    uint8       ValidFlag;                  //电表有效标志
    uint8       EventStat;                  //电表事件状态字
    uint8       MeterCheckFails;            //验表失败次数
    uint8       Option;                    //选项字段，对于交采来说，表示交采类型；0-内部自带交采，1-扩展模块中的交采
//    INT16U        MeterCommFails;             //电表通讯失败次数
    uint32       BaudRate;                   //波特率，只用于识别面向对象表
} METER_FILE_T;                            //电表档案

typedef struct
{
    uint8   MeterCheckIndex;                    //验表索引
    uint8   SearchMeterRspFlag;                 //搜表回码标志
    uint16  OopBaudRate;                        //面向对象协议波特率
} SEARCH_Rs485_METER_T;

typedef struct
{
    uint8 msindex;
    uint8 updateflag;       //是否已经启动升级完
    uint8 encryptMetNo[8];      //获取到的表号
    uint32 encryptCounter;      //计数器
    OOP_OCTETVAR256_T key;      //密钥包
    OOP_OCTETVAR256_T sessionKey;   //会话密钥包
    time_t sendtime;    //启动升级的时间
    uint8 delay;
    uint8 failcnt;
}FILE_UPDATE_ENCRYPT_INFO_T;               //每个线程转加密升级的过程记录
/*定义下收到上电事件后的时钟管理电表结构体  */
typedef struct tag_meter_timemanager
{
    uint16 pn;
    uint8 prtl;
    uint8 addr[6];  //收到上电事件的电表地址
    uint8 getIndex; //645表查询时钟得发两帧
    OOP_DATETIME_S_T metertime; //查询到的电表时钟
    uint32 diff;
    time_t sendtime;
    uint16 sendIndex;
    TMN_TO_METER_STAT status; //表查询的状态机 先查时钟 再对时
    uint8 failcnt;          //这个表查询失败的次数
    uint8 speed;            //485用的 把波特率记下来
    struct tag_meter_timemanager *next;
}METER_TIMEMANAGER_T;


//自动抄表线程参数结构
typedef struct
{
    uint8                    CommPipe;                           //对应的不同的端口
    int                     MsgSock;        //线程间ipc通信用
    DB_CLIENT               DBClientHandle;

    uint8                    MtrParaModifyFlag;                  //档案修改标识(/x55全清 /xaa部分修改)
    ThreadArg_T*             UpArg;                           //上行通信进程锁结构指针
    Down_ThreadArg_T*        DownArg;                        //下行通信进程锁结构指针
    TOTAL_STATE_T            TotalState;                         //总状态字
    SendFrameQueue_T         AmrSendFrameInfo;                   //记录抄表发送帧信息队列
    TmnAmrInfo_T             TmnAmrInfo;                         //终端抄表信息
    MtrAmrInfo_T             pMtrAmrInfo[MAX_AMR_COLLECT_CNT];                        //电表抄表信息
    TASK_CFG_STATE           TaskModifyFlag;            //任务状态标识
    uint8                    TaskNeedInit;
    uint8                    TaskNum;
    CTS_RUN_TASK_T           pTaskRunInfo[SCHEME_MAX_NUM];          //记录每个线程的任务执行状态
    CTS_RUN_TASK_T           *pTaskHead;                    //优先级最高的任务

    PLCTASK_QUEUE            PlcTaskQueue[PLCTASK_MAXNUM];       //载波任务队列
    PLCTASK_QUEUE*           PlcTask_RunLink;                    //当前载波运行的任务
    uint8                    TaskRunDelaySec;
    uint8                    CmpResult;                          //档案比对结果
    uint16                   CmpCurIndex;                         //档案比对当前读取的电表索引
    MODULE_METADDRHEAD       *pNeedDeleteMetHead ; //需要删除的模块从节点地址
    MODULE_METADDRHEAD       *pNeedAddMetHead ;     //需要增加的模块从节点地址
    RELAY_TRANS_T            RelayTrans;                         //中继数据
    //RELAY_DEAL_STR           Relay_DealStu;                      //穿透中继处理
    uint8                    IsNeedSearch;               //是否需要产生搜表任务 1表示命令搜表 2表示周期搜表
    SEARCH_METER_T           MeterSearch;

    PATCH_TASK_T            PatchTask;          //补抄任务结构体
    PATCH_MTR_INFO_T        PatchMtrInfo[MAX_AMR_COLLECT_CNT];
    PATCH_TASK_RUN_T        PatchRunTask;

    EVENT_REPORT_T          EventReport;    //电表事件上报处理
    BROAD_TIMING_T          BroadTiming;        //广播校时
    uint8                   MaintainPipe;
    uint8                   Isadjusttime;
    int                     ProCfgModifyDelaySec;
    time_t                  FunLastRunTime;

    uint8                   IsReportHPLCInfo;       //记录是否上报了0051HPLC信息，如果没有则过一分钟再查
    time_t                  HPLCInfoSendTime;
    MODULE_METADDRHEAD       *pReNetworkingDelHead ; //需要重新组网的电表链表
    MODULE_METADDRHEAD       *pReNetworkingAddHead ;
    uint8      ReNetworkingDelay;           //删除档案后经过延时添加档案
    uint8       IsResetCCO;        //当日是否重启过CCO一天最多干一次

    UPDATE_INFO_T  UpdateInfo;  //设备文件代发使用
    uint8           IsUpdating; //当前是否在文件发送中

    FILE_UPDATE_ENCRYPT_INFO_T UpdateEncryptInfo;       //转加密升级相关信息记录

    GET_NODE_VERSION_INFO_T      NodeVersionInfo;   //获取模块节点信息
    uint8 SendQueueFlag;            //发送队列是否有数据 当本轮次启动时检查是否发送队列中有补抄发送的数据
#ifdef AREA_HUNAN
    STA_CURVE_STATE_GET_INFO_T   STACurveInfo;
#endif
    DATA_CACHE_T  meterData[PARALLEL_QUEUE_NUM];     //缓存5个表的数据，对应5个并发通道
    DATA_CACHE_T  patchMeterData[PARALLEL_QUEUE_NUM];     //缓存5个表的数据，对应5个并发通道
    METER_TIMEMANAGER_T    *meterTimeManager;   //收到电表上电事件后进行时钟管理的电表链表
    METER_TIMEMANAGER_T    *meterTimeDelay;    //如果要对时的时候发现在整点5分钟内 就添加到这个队列里 等5分钟后再校时
#ifdef AREA_FUJIAN
    uint8   taskPowerOnFlag;        //给上电触发任务用的
    FUJIAN_TASK_RUN_LIST_T  taskRunList;        //运行中的任务信息
    FUJIAN_TASK_RUN_T      *taskHead;          //指向优先级最高的任务
    uint16      currentTaskId;        //记录下当前正在执行的任务
    //UART_DEVICE_T* rs485Dev;
    uint8    rs485baud;             //波特率
    uint8    maxNoticeNum;          //每一轮预告的最大对象数量
    uint8    noticeMode;            //抄读方式 0旧模式 1新模式
    BROAD_TASK_T BroadTask;
    OOP_CHAN_RLUESTATE_T collectionState;   //通道采集状态


    SEARCH_METER_STATE_T    SearchMeterState;                  //搜表状态字
    SerchStruct             Rs485Smet;
    METER_FILE_T            SearchMeterFile;
    uint8                   CurPrtlMode;
   // uint8                   searchRetryCnt;    // 搜表重试次数
   // uint8                   SerchNumRec;       // 记录下一帧的搜表索引
    //uint8                   SecSendFlag;       //二次发送标志 特殊处理泰瑞杰电表BUG
    SEARCH_Rs485_METER_T          SearchMeter;
    //uint8                   uartErrFlag;
#endif
}AmrThreadArg_T;


typedef struct
{
    DB_CLIENT               DBClientHandle;
    uint8                    MtrParaModifyFlag;
    TASK_CFG_STATE           TaskModifyFlag;            //任务状态标识
    uint8                   TaskNeedInit;
    uint16 Pn;      //交采测量点号 如果没配则线程不运行
    uint16 TaskNum;
    CTS_RUN_TASK_T           pTaskRunInfo[SCHEME_MAX_NUM];          //记录每个线程的任务执行状态
    CTS_RUN_TASK_T           *pTaskHead;                    //优先级最高的任务
    ThreadArg_T*             pBufArg;       //交采线程要增加代理透传功能

}TotalGroupThreadArg;//总加组线程使用结构体

typedef struct
{
    OOP_OAD_U EventMainOad;     //事件oad
    uint32 ReportSeq;                   //已经上报的事件序号
    uint8   isHappen;           //事件的发生状态，TRUE：发生，FALSE：恢复
}EVENT_REPORT_SEQ_T;        //记录每个事件已经上报的序号
//任务记录文件中每个电表的记录信息
typedef struct
{
    uint16 Pn;
    uint8  Result;//抄表结果 0 未抄 1 抄成功
    EVENT_REPORT_SEQ_T EventReportSeq[MAX_EVENT_ROAD_NUM];
}Meter_Record_T;

//每个任务记录文件结构体信息
typedef struct
{
    uint16 TaskId;   //任务id
    char  FileVersion[5];      //标记任务记录文件版本 版本不一样的会删除 "TR-1"
    char  ThisTurnBeginTime[7];//记录的最近一个抄表开始时间
    uint16 MeterCount;  //电表个数
    Meter_Record_T MeterRecord[MAX_AMR_COLLECT_CNT];  //和AmrThreadArg_T中pMtrAmrInfo对应

}Task_Record_T;


typedef struct
{
    uint8 ChannelType;//接口类型    enum{CDC-ACM(1), CDC-ECM(2)},
    uint8 ChannelFunc;//功能配置    enum{管理通道(0), 远程通信(1)，PLC/RF通信(2)，RS-485通信(3)，遥信脉冲(4)，CAN通信(5)，M-Bus通信(6)，控制(7)，模拟量采集(8)，RS232通信(9)}
}CHANNEL_INFO_T;  //模组信息中的通道信息


typedef struct
{
    pthread_rwlock_t   ModuleLock;  //读写模组信息所用读写锁
    uint8 IsValid;//是否有效 0无效 1有效
    uint8 DeviceType[32];//设备型号
    uint8 DeviceID[64];//设备id
    uint32 SoftVersion;//软件版本
    uint8 SoftDate[16];//软件发布日期
    uint32 HardVersion;//硬件版本
    uint8 HardDate[16];//硬件发布日期
    uint8 ManufacturerCode[10];//厂商编码
    uint8 ChannelNum;//通道个数
    CHANNEL_INFO_T ChannelInfo[MAX_CHANNEL_NUM];//通道配置

}MODULE_INFO_T;//模组信息

typedef struct
{
    uint8   PeriodSeq;   /*-表征费率顺序-*/
    uint16  Speed;       /*波特率*/
    uint8   byByteSize;  /*数据位, 4-8 */
    uint8   byParity;    /*校验 0-4=None,Odd,Even,Mark,Space*/
    uint8   byStopBits;  /*停止位 0,1,2 = 1, 1.5, 2*/
    uint8   Sync;        /*-同步头--*/
    uint8   EndByte1;    /*结束字节1*/
    uint8   EndByte2;    /*结束字节2*/
    uint8   Len1;        /*长度1    */
    uint8   Len2;        /*长度2    */
    uint8   Append;      /*附加长度,*/
    uint16  RspTime;     /*--电表反应时间:命令码发出后，收到电表返回的第一个字节的时间--*/
    uint16  SndTime;     /*--命令发送时间:发送命令报文的时间--*/
    uint16  RcvTime;     /*--字节之间的间隔时间--*/
    uint8   HDSKFraTotal;/*-给有握手过程的电表使用,握手帧总数-*/
    uint8   CmdFraTotal; /*-给有握手过程的电表使用,读取数据帧总数-*/
}cbMetPara;
typedef struct

{
    uint16  DataId;         /*--考虑正反向数据根据数据符号判断用--*/
    uint16  DataId2;
    uint8   *pSrcBuf;         /*--源数据缓冲区--*/
    uint8   SrcDecLen;      /*--源数据小数位数--*/
    uint8   SrcDataLen;     /*--源具体数据字节数--*/
    uint8   SrcDataType;    /*--源数据类型--*/
    uint8   DesDataType;    /*--目的数据类型--*/
    uint8   Rate;         /*--*/
    uint8   DataNum;      //数据项偏移（一个数据ID由多个数据组成）好像都是1
    uint8   DesBuf[200];   /*-目的缓冲区-*/
    uint8   pipe;         /*-通道号-*/
    uint16   MetSn;
    uint8   exbuf[5];     /*-暂时不用，为了扩展-*/
//#if CBTASK_NEW == 1
    uint8   type;       //ID类别 RA_DATA ...
    uint8   desDataLen; //目标数据长度
    uint8   srcTodes;       /*--源数据类型转换成目的数据类型(UBCD_TO_BIN等)--*/
    uint8   DataType;       /*--数据类型(实时、历史)--*/
//#endif
#if CBTASK_OOP == 1
   int8 scaler;
  uint8 desOffset;//数据偏移地址，用于1个源ID对应多个数据项时使用
#endif
}__attribute__ ((packed)) dataFormat;

typedef struct
{
    uint8 logicid;
    uint16 infonum;     //更新的信息点号
    OOP_OAD_U oad;      //更新的数据标识
#ifdef AREA_FUJIAN
    OOP_OAD_U MasterOad;
#endif
}DB_UPDATE_T;                   //数据中心上报数据变更事件后发送给线程的结构体

typedef struct
{
    uint8               planID;             /**< 方案编号       */
    OOP_EVENTSELECT_T   events;             /**< 需抄读的事件 */
    uint16              msNum;          //电表个数
    MS_INFO             msInfo[MAX_AMR_COLLECT_CNT];
}PLAN_EVENT_INTO_T;

typedef struct
{
    pthread_rwlock_t    rwLock;                  //读写锁 多线程访问
    uint16 plancount;
    PLAN_EVENT_INTO_T    PlanEvent[MAX_EVENT_PLAN_NUM];

}EVENT_PLAN_T;      //

//用于端口和从节点描述符“type=value;mfrs=value;idformat=value;id=value;mmfrs=value;midformat=value;mid=value”
typedef struct
{
    char type[10];      //本地通讯模块类型 value包括“PLC”、“WIRELESS”等 节点设备类型
    char mfrs[4];       //芯片厂商代码 2个字符的ASCII码
    uint8 idformat;     //芯片ID格式类型，其value为以下几种：“0”为组合格式；“1”为BCD；“2”为BIN ；“3”为ASCII
    char id[100];       //芯片ID信息，其value为对应的字符串,根据idformat的格式进行相应转换
    char mmfrs[4];   //模块厂商代码，其value为2个字符的ASCII码
    uint8 midformat;    //模块ID格式类型，其的value为以下几种：“0”为组合格式；“1”为BCD；“2”为BIN ；“3”为ASCII
    char mid[200];  //块ID信息，其value为对应的字符串,根据midformat的格式进行相应转换。
}DESCRIPTOR_T;

typedef enum{
    UDTASK_INIT,      //任务监控数据初始化
    UDTASK_START,     //任务启动
    UDTASK_END,       //任务结束
    UDTASK_STATE,     //更新任务状态
    UDTASK_SUCCESSNUM,//更新任务成功数量+1
    UDTASK_SENDFRMNUM,//更新已发送报文条数+1
    UDTASK_RECVFRMNUM,//更新已接收报文条数+1
    UDTASK_FIRFINSH,  //第一次抄表完成
}UpdateTaskCmd_t;
typedef struct
{
    pthread_mutex_t MonitorLock;
    uint8 TaskCount;
    OOP_ACQ_MONITOR_T TaskMonitorInfo[SCHEME_MAX_NUM];
    uint8 logicid[SCHEME_MAX_NUM];
}TASK_MONITOR_T;    //用于任务监控

typedef struct
{
    pthread_mutex_t Lock;
    uint8 isvalid;  //是否设置了周期搜表参数
    OOP_TIMINGSEARCH_T Param;
}SEARCH_METER_CYCLICITY_T;//周期搜表参数配置

typedef struct
{
    pthread_mutex_t lock;
    uint8 isvalid;
    OOP_SEARCHPARAM_T searchparam;
}SEARCH_METER_CLEAR_T;


typedef struct
{
    pthread_mutex_t Lock;
    OOP_SYNCTIME_T  SyncTime;           //广播校时参数配置 42040200
    OOP_P2PSYNCTIME_T   P2PSycnTime;       //单地址广播校时参数 42040300
}SYNC_TIMING_T;//广播对时参数配置

typedef struct
{
    pthread_mutex_t Lock;
    uint32 Ti;              //单位分钟
    uint8 isSet;        //标记下主站有没有设置过 0的时候表示是默认参数
}NODE_UPDATE_CYCLE_T;//更新周期0XF2090600

typedef struct
{
    uint16 infonum;
    uint8 Addr[6];
}ACROSS_AERA_RES_T;

#define MAX_ACROSS_AREA_RESULT 2000
typedef struct
{
    pthread_mutex_t Lock;
    uint16 infonum;
    uint16 count;
    ACROSS_AERA_RES_T result[MAX_ACROSS_AREA_RESULT];
}ACROSS_AREA_T;         //非本台区电能表信息上报

#define MAX_SEARCH_METER_RESULT 2000
typedef struct
{
    pthread_mutex_t lock;
    uint16 infonum;
    uint16 count;
    METER_ADDR_AND_PRTL_T result[MAX_SEARCH_METER_RESULT];

}METER_SEARCH_RES_T;

typedef struct
{
    pthread_mutex_t lock;
    uint16 infonum;
    uint16 count;
    CCO_STA_VER_T result[MAX_SEARCH_METER_RESULT];

}CCO_STA_INFO_T;


//消息0060对应的抄表状态结构体
typedef struct
{
    uint32 lastrecvtime;    //最近一次接收时间
    int amrstate;       //模块抄表状态
    int metersearchstate;   //模块搜表状态
    int autoamrstate;   //自动抄表状态
    uint16 routeclearcnt;   //清除路由次数
    uint16 dataclearcnt;    //数据初始化次数
    uint16 paramclearcnt;   //参数初始化次数
    uint16 rebootcnt;   //复位模块次数
}AMR_STATE_T;

#define MAX_DBUPDATE_QUEUE_CNT 2000
typedef struct
{
    uint8   isvalid;                           //是否有效
    DB_UPDATE_T   dbupdate;     //队列存储缓冲区
}DB_UPDATE_T_T;
//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                          //队列头
    int16  QueueTail;                          //队列尾
    DB_UPDATE_T_T Queue[MAX_DBUPDATE_QUEUE_CNT];               //通讯数据队列
} DBUPDATE_QUEUE_T;

typedef  struct
{
    uint8   AFN;                                //应用功能码
    uint16  Fn;                                 //信息类标识
    uint8   ModuleID;                           //通信模块标识(0:集中器 / 1:载波表)
    uint8   DesAddr[6];                         //目的地址
    uint8   RInfo[6];                           //信息域
    uint16  DataUnitLen;                        //应用数据长度
    uint8   DataUnitBuff[1000];   //应用数据缓冲区
} AppData_T;

typedef struct
{
    OOP_OAD_U mainoad;      //事件主oad
    uint8 oadnum;
    OOP_OAD_U oads[OOP_MAX_EVTOADS];
    OOP_DATATYPE_E oadtype[OOP_MAX_EVTOADS];
    uint8 oadlen[OOP_MAX_EVTOADS];
    uint8 oadbuf[OOP_MAX_EVTOADS][200];
}EVENTDATA_T;

typedef struct tag_APPURTENANT_METER_INFO
{
   uint8 pipeaddr[6];//虚拟通道地址
   uint8 meteraddr[6];//电能表地址
   uint8 phaseinfo;
   uint16 pn;


}OOP_APPURTENANT_METER_INFO_T;

typedef struct tag_APPURTENANT_METER_LIST
{
   uint8 num;//电表数量
   OOP_APPURTENANT_METER_INFO_T meterinfo[12];

}OOP_APPURTENANT_METER_LIST_T;

typedef struct tag_PERCEPTION_INFO
{
   uint8 terminaladdr[6];//感知终端地址
   OOP_APPURTENANT_METER_LIST_T meterlist;

}OOP_PERCEPTION_TERMINAL_INFO_T;

typedef struct tag_PERCEPTION_TERMINAL
{
   //pthread_mutex_t lock;
   uint8 num;//感知终端数量
   OOP_PERCEPTION_TERMINAL_INFO_T terminalinfo[96];
   uint16 totalpn;

}OOP_PERCEPTION_TERMINAL_LIST_T;
typedef struct tag_Power_Characteristic_Record
{
   uint8 phase;
   uint8 type;
   int32 power;
   uint8 startminute;
   uint8 startsecond;

}OOP_Power_Characteristic_Record_Unit;

typedef struct tag_Power_Characteristic
{
   uint8 devtype;
   uint8 num;//
   OOP_Power_Characteristic_Record_Unit powerunit[12];

}OOP_Power_Characteristic_Record;

typedef struct tag_Vlotage
{
   uint8 num;
   uint16 voltageuint[3];

}OOP_Vlotage_Unit;

typedef struct tag_Energy
{
   uint8 phase;
   int32 energy;
   int32 activepower;
   int32 reactivepower;
   int32 current;
   int32 currentoddhaimonic;
   int32 currentevenhaimonic;
}OOP_Energy_Unit;

typedef struct tag_Energy_Record
{
   uint8 num;
   OOP_Energy_Unit energyuint[3];

}OOP_Energy_Record_Unit;

typedef struct tag_Energy_Characteristic_Record
{
   uint8 devtype;
   OOP_Vlotage_Unit vlotage;
   OOP_Energy_Record_Unit energyuint;

}OOP_Energy_Characteristic_Record_Unit;

/** @brief 透明采集方案数据缓存结构*/
typedef struct
{
    //pthread_mutex_t        mutex;              //锁 多线程访问
    uint16                 oopPlanTransparentNum;   //方案个数
    //BOOLEAN                   isChanged;          //变更标记
    //BOOLEAN                   isVaild[256];       //方案是否有效
    OOP_PLAN_TRANS_T       oopPlanTransparent[256]; //方案参数
}OOP_PLAN_Transparent_LIST_T;

typedef struct
{
    uint32 negoCnt; //单地址协商计数器
    uint32 reportCnt;   //主动上报计数器
    uint32 seq;     //应用广播通信序列号
}OOP_ESAM_COUNT_T;

typedef struct
{
    OOP_VISIBLEVAR128_T filepath;
    uint8 fileflag;
}HPLC_UPDATE_T;

typedef struct
{
    pthread_mutex_t lock;
    OOP_PLAN_ENCRYPT_T planinfo;
    uint8   msnum;
    MS_INFO msinfo[OOP_MAX_ENCRYPT_NUM];
}FILE_UPDATE_ENCRYPT_T;

typedef struct
{
    uint8 num;
    uint8 speed[100];
}SPEED_LIST_485_T;

/*用于打高分 电表停上电事件上报缓存  */
typedef struct
{
    uint8 count;
    uint16 buflenList[5];
    uint8 bufList[5][2048];
    uint32 lastReportTime;          //记录下最后一次上报的时间 如果和当前时间超过9秒则全部上报 以防台体异常 没有上报100条
}METER_POWER_CACHE_T;

/*定义终端带电状态 上电默认有电 收到停电事件后变为停电 收到上电事件后变为上电  */
typedef enum
{
    POWER_ON = 0,
    POWER_OFF,
}TMN_POWER_STATE_E;

/*定义终端带电状态  */
typedef struct
{
    TMN_POWER_STATE_E powerState;   //终端带电状态
    OOP_DATETIME_S_T powerOnTime;   //终端上电时间
    OOP_DATETIME_S_T powerOffTime;  //终端停电时间
}TMN_POWER_STATE_T;

/*定义电表停上电状态写入数据中心的结构体  */
typedef struct
{
    uint16 pn;          //电表信息点号
    uint8 addr[6];      //表地址
    TMN_POWER_STATE_E powerState;//电表带电状态
    time_t powerOffTime;    //电表停电时间
    uint32 eventSeq;        //最新的停上电事件序号
}METER_POWER_STATE_T;

/*定义一个结构体存储每个表组出来的停上电事件报文  */
typedef struct
{
    uint16 pn;      //表信息点号
    uint8 addr[6];  //表地址
    TMN_POWER_STATE_E powerState;//电表带电状态
    uint8 databuf[256];        //组出来的报文
    uint16 buflen;              //报文长度
}METER_POWER_BUF_T;
/*最多缓存100个表的停电事件 实际上会报500个  */
#define MAX_METER_POWER_BUF_NUM     500
typedef struct
{
    uint16 meterNum;     //缓存的电表个数
    METER_POWER_BUF_T powerBufList[MAX_METER_POWER_BUF_NUM]; //每个电表对应的报文
    uint32 firstTime;       //收到首个表停电事件的时间 最多缓存20秒
}METER_POWER_BUF_LIST_T;

#endif
