#ifndef _MDL_MGR_H_
#define _MDL_MGR_H_



#include "common.h"
#include "commfun.h"
#include "OopAll.h"
#include "power.h"
#include "commdrv.h"

#ifdef __cplusplus
extern "C"{
#endif

#define CONFIG_FILE                 "/tmp/mapManager.ini"

#define MDL_PRTL_VER0               0
#define MDL_PRTL_VER1               1

#define TEMP_DINGXIN_MDL            1                   //临时鼎信模块
#ifdef  TEMP_DINGXIN_MDL            
    #define    DINGXING_TYPE_NAME   "GBH32-DX001"       //临时类型名                   
#endif

#define MDL_LOG_DIR                 "/data/app/mapManager/log"      //日志路径

#define MAX_QUEUE_CNT               64
#define MAX_INFO_QUEUE_CNT          64

#define MAX_QUEUE_BUFF_LEN          5000        //通讯数据buff长度
#define MAX_MSG_PAYLOAD_LEN         2040        //消息MSG的payload最大长度

#define mdlnum                      5

/* 帧结构偏移 */
#define H_SITE_M                    0           // 第一个0x7e
#define L_LOW_M                     1           // 数据长度低字节
#define L_HIGH_M                    2           // 数据长度高字节
#define C_SITE_M                    3           // 控制码
#define D_SITE_M                    5           // 数据域

/* 模块控制码 */
#define ACK_F                       0           // 确认帧
#define NAK_F                       1           // 否认帧
#define NGT_F                       2           // 链路协商帧
#define INF_F                       2           // 信息帧

/*   功能码   */
#define GET_N                       0           // 查询数据
#define CTL_N                       1           // 控制命令
#define RPT_N                       2           // 状态通知
#define ASK_N                       3           // 请求抄读


/* 函数返回 */
//#define RET_OK                      0
#define RET_ERR                    -1

#define MSG_MQTT  1    

#define  MQTT_INIT           0               //初始化mqtt连接
#define  MQTT_RECV           1               //mqtt数据接收
#define  MQTT_SEND           2               //mqtt数据发送
#define  MQTT_CLOSE          3               //关闭mqtt连接
#define  MQTT_LOGIN          4               //登录到系统管理器
#define  MQTT_LOGIN_WAIT     5               //登录等待回复

#define  MAX_BUF_LEN       3000

#define     MAX_CHAR_LEN        256
#define     MAX_VIRTURL_CNT     10

typedef struct
{
    uint8   sDeviceName[50];    // 设备名，如"/dev/USB0"
    uint32  baud;               // 波特率
    uint32  databits;           // 数据位, 4-8 
    uint32  stopbits;           // 校验 'n'  'e'  'o'  's' = None,Odd,Even,Space
    uint32  parity;             // 停止位 0,1,2 = 1, 1.5, 2
    int32   fdUart;             
} COMM_PARA_T;

typedef enum
{
    IDLE_STATE = 0,             // 空闲
    INIT_STATE,                 // 初始化
    NGTT_SEND_STATE,            // 协商
    NGTT_RECV_STATE,            // 协商接收
    SEND_STATE,                 // 发送 
    RECV_STATE,                 // 接收   
    WAIT_STATE,                 // 待机 
    CLOSE_STATE,                // 关闭
    OTHER_STATE,                // 其他状态
} MODULE_STATE_E;

typedef enum
{
    UNKNOW_STATE,               // 未知状态
    ON_STATE,                   // 插入的状态
    OFF_STATE,                  // 未插的状态 
} USB_STATE_E;


//typedef struct
//{
//    uint8   seq;
//    uint8   cmd_h;                  // 控制码
//    uint8   afn;
//    uint16  fn;
//    uint8   pDataBuf[2048];
//    int16   dataLen;
//}FRAME_INFO_T;


typedef struct
{
    uint16  prtlVersion;                //协议版本号
    uint8   tmnModeLen;                 //终端设备型号长度
    char    tmnMode[MAX_CHAR_LEN];      //终端设备型号
    uint8   tmnIDLen;                   //终端设备ID长度
    char    tmnID[MAX_CHAR_LEN];        //终端设备ID
    uint16  sendBufLen;                 //最大发送缓冲区长度
    uint16  recvBufLen;                 //最大接收缓冲区长度
    uint8   conCurrWDs;                 //并发传输窗口数
}LINKREQUEST_T;                         //链路请求

typedef enum
{
    CDC_ACM = 1,        
    CDC_ECM,
    HID
}INTERFACE_TYPE_T;

typedef struct
{
    INTERFACE_TYPE_T    interf;
    uint8               funcConfig;
    char                driveName[MAX_CHAR_LEN];
}VIRTURL_PIPE_MODE_T;


typedef struct
{
    uint16  prtlVersion;                //协议版本号
    uint8   modModeLen;                 //模块设备型号长度
    char    modMode[MAX_CHAR_LEN];      //模块设备型号
    uint8   modIDLen;                   //模块设备ID长度
    char    modID[MAX_CHAR_LEN];        //模块设备ID
    uint16  sendBufLen;                 //最大发送缓冲区长度
    uint16  recvBufLen;                 //最大接收缓冲区长度
    uint8   conCurrWDs;                 //并发传输窗口数
    uint8   virPipeCnt;                 //虚拟通道数
    VIRTURL_PIPE_MODE_T     virturMode[MAX_VIRTURL_CNT];    //虚拟通道工作模式
}LINKRESPONSE_T;            //链路响应信息


typedef struct
{
    uint16  year;
    uint8   month;
    uint8   dayofmonth;
    uint8   dayofweek;
}DATE_T;

typedef struct
{
    uint8   DevTypeLen;             //设备型号长度
    char    DevType[MAX_CHAR_LEN];  //设备型号
    uint8   DevIDLen;               //设备ID长度
    char    DevID[MAX_CHAR_LEN];    //设备ID
    uint32  softVersion;            //软件版本
    DATE_T  date_soft;              //软件发布日期
    uint32  hardwVersion;           //硬件版本
    DATE_T  date_hard;              //硬件发布日期
    uint8   VendorCodeLen;          //厂商编码长度
    char    VendorCode[MAX_CHAR_LEN];   //厂商编码

    //新协议
    uint8   type;                   //协议标记
    char    softVer[MAX_CHAR_LEN];  //软件版本
    char    softDate[MAX_CHAR_LEN]; //软件版本
    char    hardVer[MAX_CHAR_LEN];  //硬件版本
    char    hardDate[MAX_CHAR_LEN]; //硬件版本
    char    VendorExt[MAX_CHAR_LEN];//厂商扩展
}MODULE_INF_T;

typedef struct
{
    uint32  packCnt;            //总包数
    uint16  packState;          //包传输状态
}FILE_TRANS_STATE_T;

typedef enum
{   
    powerOn,                //上电
    powerOff                //断电
}POWER_STATE_E;             //电源状态

//typedef struct
//{
//  uint8   fileID;             //文件标识
//  uint32  segNo;              //段号
//  uint32  segCnt;             //总段数
//  uint8   fileData[1024];     //文件数据
//}FILE_TRANS_SET_T;                //文件传输

//typedef struct
//{
//  uint8           validFlag;      //有效性标志
//  MODULE_INF_T    mod_Info;       //各槽位模组信息
//}MODULE_INF_F_T;


typedef struct
{
    uint8               modInfoFlag;    //模组信息标志(0x55:有效;0xAA:插入未识别;)
    MODULE_INF_T        moduleInfo;     //模块信息
    LINKRESPONSE_T      linkInfo;       //链路信息
    uint16              protocolVer;    //协议版本
    POWER_STATE_E       powerSta;       //电源状态
    //FILE_TRANS_STATE_T    fileState;      //文件传输状态
    Date_Time_t         clock;          //时钟
    //FILE_TRANS_SET_T  fileTrans;      //文件传输
}MODE_ALL_INFO_T;               //模块通用信息体

typedef struct
{
    uint8           YX_state;           //遥信状态
    uint8           YX_change;          //遥信变位
    Date_Time_t     date_time[4];       //遥信变位时间            //目前是4路遥信
}REMOTE_STATE_T;    

typedef struct
{
    uint32      pulseCnt[4];        //脉冲数
    uint32      time[4];            //统计时间
    
}PULSE_CNT_T;               //脉冲计数


typedef struct
{
    REMOTE_STATE_T      remote_state;       //遥信状态
    PULSE_CNT_T         pulse_cnt_last;     //上一次脉冲计数
    PULSE_CNT_T         pulse_cnt;          //脉冲计数
    uint8               YX_FuncSet;         //遥信或脉冲功能配置（选择）
    uint16              YX_AntiShakeTime;   //遥信防抖时间
    
}REMOTE_PULSE_T;    

typedef enum
{
    PULSEMODE = 1,
    LEVELMODE,
}RELAY_OUTMODE_E;               //继电器输出模式
    
typedef enum
{
    /* LINK-APDU */
    LINKREQUEST = 1,       /* 预连接请求 */
    GETREQUEST,          /* 读取请求 */
    SETREQUEST,            /* 设置请求 */
    REPORTREQUEST,         /* 上报通知 */

    /* LINK-APDU  */
    LINKRESPONSE = 129,    /* 预连接响应 */
    GETRESPONSE,           /* 读取响应 */
    SETRESPONSE,           /* 设置响应 */
    REPORTRESPONSE,     /* 上报应答 */
}ENUM_FRAME_FUNC_T;

typedef enum                //Data Access Result
{
    ACTSUCCESS,
    REFUSED,
    NODEFINITION,
    
    OTHERS = 255
}DAR_T;

typedef struct
{
    uint8           plugEvent;      //是否产生插拔事件
    int32           usbNo;          //模组序号
    USB_STATE_E     plugStatus;     //模组插拔状态
}PLUG_MODE_T;               //模块插拔事件

typedef struct
{
    USB_STATE_E     plugStatus;         //模组插入状态
    uint8           changeStatus;       //模组状态变位
    time_t          time;               //变位时间
}PLUG_STATUS_T;             //模组插拔状态

typedef struct
{
    uint8   modeTypeLen;                    //模组型号长度
    char    modeType[MAX_CHAR_LEN];         //模组型号
    int32   usbNo;                          //槽位序号
}SLOT_INFO_T;

typedef struct 
{
    char channelName[256];                  //模组通道信息
    char channelPath[128];                  //模组通道信息路径
    char channelInfo[64];                   //模组通道端点名称
}MDL_CHANNEL_CFG_T;

typedef union MDL_CNTL
{
    struct
    {
        uint8   ctlcode:6;
        uint8   prm:1;
        uint8   dir:1;
    }bit;
    uint8   all;
}MDL_CNTL_U;                    // 控制码 

typedef struct
{
    ENUM_FRAME_FUNC_T frameFunc;
    uint16 dataLen;                         //完整帧长度
    MDL_CNTL_U cntl;
    uint8   seq;
    uint8  pDataBuf[2048];                  //完整帧报文
    uint16 fcs;
}MDL_FRAME_INFO_T;

//typedef struct
//{
//  ENUM_FRAME_FUNC_T   frameFunc;  //信息帧功能码
//  uint16              dataLen;    //数据域长度
//  uint8               crtlCode;   //控制码
//  uint8               prm;        //启动标志
//  uint8               dir;        //传输方向
//  uint8               fid;        //帧序号
//  char                apdu[2048]; //数据域
//  uint16              fcs;        //校验码
//}MDL_FRAME_INFO_T;

typedef union SFP_CMD_L
{
    struct
    {
        uint8   seq:6;
        uint8   prm:1;
        uint8   dir:1;
    }bit;
    uint8   all;
}SFP_CMD_L_U;                   // 控制码 

#define VM_COMM_NUM_MAX       8 // 虚拟通道最大数
typedef struct
{
    uint16  ver_id;                             // 协议版本号
    uint8   tmn_tp[32];                         // 终端设备型号
    uint8   tmn_id[32];                         // 终端设备型号
    uint8   mdl_tp[32];                         // 模块设备型号
    uint8   mdl_id[32];                         // 模块设备ID
    uint16  mx_sd_len;                          // 最大发送缓冲区长度（字节）
    uint16  mx_rv_len;                          // 最大接收缓冲区长度（字节）
    uint8   tx_wid_nm;                          // 并发传输窗口数
    uint8   vm_comm_nm;                         // 最大虚拟通道数
    uint8   vm_comm_tp[VM_COMM_NUM_MAX];        // 虚拟通道接口模式
}FRAME_NGT_T;

/* 考虑字节对齐, 没有严格按照报文格式, 收到的消息解析成如下格式 */
typedef struct tagInfoData
{
    uint32 IP;        //目的端ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID_ID;
    uint16 IOP_ID;

    uint32 nPayloadLength; 
    uint8  aPayloadData[MAX_MSG_PAYLOAD_LEN];
}INFO_DATA_T;

typedef struct tagMsgInfo
{
    uint32 IP;        //目的端ip
    uint16 port;
    uint8  res[2];
    
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

    uint32 MsgLen; 
    uint8  MsgData[0];
}MSG_INFO_T;


/** @brief 组件初始化配置    */
//typedef struct
//{
//  int8            name[MQT_NAME_LEN];     /**< 名称                     */
//  uint32          eventMask;              /**< 订阅事件                   */
//  uint32          msgQueLen;              /**< 消息队列长度(0为缺省)     */
//  uint32          msgReadLen;             /**< 消息读取缓存长度(0为缺省)   */
//  VERSION_T       version;                /**< 版本号                      */
//} MQT_PROFILE_T;


//队列
typedef struct
{
    int32  QueueLen;                            //队列长度
    uint8   QueueBuf[MAX_QUEUE_BUFF_LEN];       //队列存储缓冲区
}Queue_T;

//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                           //队列头
    int16  QueueTail;                           //队列尾
    Queue_T Queue[MAX_QUEUE_CNT];               //通讯数据队列
}LockQueue_T;

//线程参数结构
typedef struct
{
    uint8        nArrIndex;                     //数组下标
    LockQueue_T *RecvBuf;                       //接收缓冲区结构指针
    LockQueue_T *SendBuf;                       //发送缓冲区结构指针
}ThreadArg_T;


typedef struct
{
    pthread_mutex_t FrameLock;                      //互斥锁
    uint8           flag[MAX_INFO_QUEUE_CNT];       //有效性标志
    INFO_DATA_T     InfoData[MAX_INFO_QUEUE_CNT];   //mqtt消息帧
}InfoDataQueue_T;                               //保存已处理的mqtt帧信息
/********************************************************************************
 * @brief   线程维护的次数和线程ID号 
 * @note    ntid:线程id号
 *          cnt_thread_msg:线程维护次数
 *******************************************************************************/
typedef struct
{
    pthread_t ntid; 
    int16 cnt_thread_msg;       
} MDL_THREAD_DEFEND_T;

/********************************************************************************
 * @brief   线程维护的次数和标志信息    
 * @note    ntid:线程id号
 *          port:通道号
 *          cnt_thread_msg:线程维护次数
 *******************************************************************************/
typedef struct
{
    int32 port;
    int16 flag_thread_msg;
} MDL_THREAD_INFO_T;

/**
 ******************************************************************************
 * @brief       注册模组管理器相关参数
 * @param[in]   pProfile    :   组件配置参数
 * @return      成功返回TRUE, 失败返回FALSE
 * @note                    
 ******************************************************************************
 */
//BOOLEAN mdl_regist_init(const MQT_PROFILE_T *pProfile);


/* 控制域 */
typedef union _CMT_FIELD_U
{
    struct
    {
        uint8 fid :6;   //帧序号
        uint8 frm :1;   //启动标识
        uint8 dir :1;   //传输方向
        uint16 cmd :8;  //控制码
    };
    uint16 wcmd;
}CMT_FIELD_U;

typedef struct
{
    uint16 len;
    CMT_FIELD_U cmd_field;
    uint8 afn;
    uint8 dt1;
    uint8 dt2;
    int16 dataLen;
    uint8 *pDataBuf;
}__attribute__ ((packed)) FRAMEOFMODULE_INFO;

typedef struct _MDL_MSG_T
{
    char mdlType[32];
    char mdlId[32];
    char softVer[4];
    char softDate[10];
    char hardVer[4];
    char hardDate[10];
    char facVer[4];
}__attribute__ ((packed)) MDL_MSG_T;

typedef enum
{
    MDL_UPDATE_IDLE = 0,                               //空闲
    MDL_UPDATE_INIT,
    MDL_UPDATE_DELAY,                              //延时
    MDL_UPDATE_READ_FILE_DELAY,
    MDL_UPDATE_OPEN_FILE,                          //打开升级文件
    MDL_UPDATE_READ_FILE,                          //读取升级文件
    MDL_UPDATE_SEND,                               //发送
    MDL_UPDATE_RECV,                               //接收
    MDL_UPDATE_RETRY,                              //重试
    MDL_UPDATE_AGAIN,                          //整体重试
    MDL_UPDATE_GET_RUN_STATE,                      //获取路由运行状态
    MDL_UPDATE_REBOOT_DEV,
    MDL_UPDATE_GET_VER,                            //版本信息
    MDL_UPDATE_END,                                //结束处理
}MDL_UPDATE_DEAL_T;                      //载波模块升级状态字

typedef struct
{
   uint32  TotalPackageNum;
   uint32  CurrentPackageNo;
   uint16  PackageLen;
}MdlUpdateInfoState;     //模组模块本地升级功能记录结构体

typedef enum
{
    MDL_COMM_IDLE = 0,                          //空闲
    MDL_COMM_SEND,                              //发送
    MDL_COMM_RECV,                              //接收
    MDL_COMM_RETRY,                             //重试
    MDL_COMM_DELAY,                             //延时
}MDL_COMM_STATE_T;                           //主动通讯状态字

typedef enum _MDL_UPDATE_STEP_E
{
    UPDATE_IDLE = -1,
    UPDATE_START = 0,
    UPDATE_RUN,
    UPDATE_END,
}MDL_UPDATE_STEP_E;

#define L_SIZE_M                    1           //数据长度-
#define AFN_SITE_M                  5           //afn
#define DT1_SITE_M                  6           //dt1
#define DT2_SITE_M                  7           //dt2
#define DATA_SITE_M                 8           //数据开始位置

// 错误信息码
#define COMM_ERROR                  -1          //一般性错误
#define MODULE_TYPE_ERROR           -2          //模块类型与回码中的模块类型不一致错误
#define MODULE_CHANGE               -3          //模块变更

#define COMM_OK                     1           //一般性正确

// 模块功能码
#define GET_M                       0           //模块抄读命令
#define CTRL_M                      1           //模块控制命令
#define NOTIFY_M                    2           //状态通知
#define READ_M                      3           //请求抄读   

#define MOD_CTR_OK   0       // 确认
#define MOD_CTR_NO   1       // 否认
#define MOD_CTR_LIK  2       // 链路协商
#define MOD_CTR_MSG  3       // 信息帧

#define UPDATE_PACKAGE_LEN 256
#define MAX_UPDATE_DELAY_TIME 6
#define MAX_MDL_COMM_DELAY_TIME 6

#define MDL_UPDATE_BUF_LOG               0    //模组升级日志

#define     MAX_INFO_DATA_NUM   500                         //消息MSG缓存列表

//消息列表缓存区
typedef  struct
{
    int16 InfoDataNum;  //消息列表缓存区的数量
    int16 InfoDataReadIndex;  //获取索引（数组下标，从0开始）
    INFO_DATA_T InfoDataList[MAX_INFO_DATA_NUM];  //消息MSG列表缓存区
} INFO_DATA_LIST_T;  //消息列表缓存区

typedef struct
{
    uint8   DTA;
    uint8   DTB;
    uint8   seq;                            //序列号
    uint8   clkSetFlag;                     //是否是设置
    uint16  dataLen;                        //数据长度
    uint8   setData[1024];                  //需要设置的数据
}TRANS_INFO_T;              //组模块帧信息体


typedef struct
{
    uint8       fileID;                 //文件标识
    uint16      packDataLen;            //数据包长度
    uint32      packNo;                 //当前包
    uint32      packCnt;                //总包数
    uint8       packData[512];          //文件数据
} __attribute__((packed)) UPDATE_INFO_T;

typedef struct
{
    MODULE_INF_T mdl_up_info[mdlnum];                       
}UPDATE_INFO_ALL_T;         //升级总信息结构体

typedef struct
{
    uint8   offpowerflag;   //断电标志
    time_t  offpowertime;   //断电秒数
    INFO_DATA_T infotemp;
}MOD_OFF_POWER_T;

extern  LockQueue_T LockSendBuf[mdlnum];

//extern char       *MDL_MGR_RECD_LOG;

extern int  g_MqttThreadFlag;            //用于检测mqtt线程是否正常

extern uint8            Event_Flag[mdlnum] ;        //用于检测是否产生事件

extern PLUG_STATUS_T    plug_status[mdlnum];        //模组插拔状态

extern SLOT_INFO_T      slotInfo[mdlnum];           //模组槽位信息

extern MODE_ALL_INFO_T  moduleInfo_all[mdlnum];     //模组通用信息

extern REMOTE_PULSE_T   YX_pulse_info;              //遥信脉冲模块信息体

extern RELAY_OUTMODE_E  relayMode;                  //继电器输出模式

extern uint16           pulseWidth;                 //继电器输出脉冲宽度

extern UPDATE_INFO_ALL_T    updateAllInfo;          //升级数据结构体

extern uint8            globleSeq;                  //全局序列号

extern MOD_OFF_POWER_T  modPowerCtl[mdlnum];        //模组断电控制信息

extern POWER_DEVICE_T*  dev_power;

extern BOOL             gBuzzerEn;

extern  int32   makeInfoData(uint8 usbNo,MODULE_INF_T   *pModuleInfo, INFO_DATA_T *pRecvInfo,      uint8 seq, INFO_DATA_T *pSendInfo);

extern  int32   makeInfoData_new(uint8 usbNo,MODULE_INF_T   *pModuleInfo, INFO_DATA_T *pRecvInfo,      uint8 seq, INFO_DATA_T *pSendInfo);
    
extern int8     AddInfoDataIntoList(INFO_DATA_T RecvInfoData);
    
extern int32    MDLWriteInfo(InfoDataQueue_T *pInfoDataQueue, INFO_DATA_T *pInfoData);
    
extern int32    MDLReadInfo(InfoDataQueue_T *pInfoDataQueue, uint8 frameIndex, INFO_DATA_T *pInfoData);
    
extern int32    MakeModeFrame(MDL_FRAME_INFO_T* frameInfo, uint8* pSendBuf);
    
extern int32    MakeFrame(int32 usbNo, TRANS_INFO_T* pTransInfo, uint8* pSendBuf);
    
extern int32    PlugEventInform(uint8 usbNo);
    
extern int      GetHeadInfo(INFO_DATA_T InfoMsgData, MSG_INFO_T *ptMsgInfo);

extern  void    ClearNgtMsg(int32 usbNo);

extern  int32   AnalysisFrameOfModule(int32 usbNo, MDL_FRAME_INFO_T *desFrameInfo, uint8 *pOutBuf,BOOL isupdate);

extern  int32 AskLocalInfo(int32 usbNo, uint8 *czBufIn, int16 len,uint8 seq);

extern  int32 GetmoduleClock(int32 usbNo);

extern char *get_logname_by_pipe(char* filepath,int8 pipe);

extern char mdlLog[100];

extern char *gDownRecordPath[mdlnum + 1];

#define MDL_LOG           ((mdlLog[0]!=0)?mdlLog:"/data/app/mapManager/log/mdlmgr.log")    //日志名称

//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define MDLMGR_FMT_TRACE(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 28, gDownRecordPath[pipe], 0, NULL, 0, fmt, ##__VA_ARGS__)
#define MDLMGR_FMT_DEBUG(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 28, gDownRecordPath[pipe], 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define MDLMGR_BUF_TRACE(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 28, gDownRecordPath[pipe], 0, buf, len, fmt, ##__VA_ARGS__)
#define MDLMGR_BUF_DEBUG(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 28, gDownRecordPath[pipe], 1, buf, len, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
