#ifndef __AREA_H__
#define __AREA_H__

#ifdef AREA_FUJIAN

#pragma pack(1)

typedef struct tagCCO_AMR_FLAG_T
{
    uint32  initFlag;
    uint32  queryParaFlag;
    uint32  queryParaAckFlag;
    uint32  queryAddrFlag;
    uint32  queryNodeInfoFlag;
    uint32  queryNodeInfoAckFlag;
    uint32  queryNodePhaseFlag;
    uint32  queryUartParaFlag;
    uint32  queryUartParaAckFlag;
    uint32  queryPrtlModeFlag;
    uint32  setAddrFlag;
    uint32  setStaReportFlag;
    uint32  startGetBroadDelayFlag;
    uint32  broadcastFlag;
    uint32  startStaRegFlag;
    uint32  endCurrentTaskFlag;
    uint32  startPreviewExecFlag;
    uint32  previewMeterObjFlag;
    uint32  setPrtlFlag;
    uint32  setRateFlag;
    uint32  setRestoreDurationFlag;
    uint32  setMaxRateFlag;
    uint32  dataFwdFlag;
    uint32  dataTransFlag;
    uint32  reportStaInfoFlag;
    uint32  reportEventFlag;
    uint32  reportMeterReqFlag;
    uint32  reportMeterAckFlag;
    uint32  reportPreviewObjFlag;
    uint32  reportPreviewObjAckFlag;
    uint32  reportBroadDelayFlag;
    uint32  reportBroadDelayAckFlag;
    uint32  reportBroadFinishAckFlag;
}CCO_AMR_FLAG_T;

#define CCO_ADDR_LEN    6

typedef struct tagCCO_QUERY_PARA_ACK
{
    uint16  taskObjNum;     /* 总预告任务对象数量 */
    uint8   meterObjNum;    /* 预告抄读对象数量 */
}CCO_QUERY_PARA_ACK;

typedef struct tagCCO_QUERY_NODE_INFO_REQ
{
    uint32  port;
    uint8   type;
    uint8   addr[CCO_ADDR_LEN];
    uint16  label;
    uint16  index;
}CCO_QUERY_NODE_INFO_REQ;

typedef struct tag_CCO_DATE_T
{
    uint16  year;       /**< 年            */
    uint8   month;      /**< 月            */
    uint8   mday;       /**< 日            */
    uint8   wday;       /**< 周            */
}CCO_DATE_T;

typedef struct tagCCO_QUERY_NODE_INFO_ACK
{
    char    manu[2]; /* 没有\0 */
    char    chip[2]; /* 没有\0 */
    CCO_DATE_T date;
    uint16  version;
}CCO_QUERY_NODE_INFO_ACK;

typedef struct tagCCO_QUERY_NODE_INFO_ACK_53F4
{
    uint8   type;
    uint8   addr[CCO_ADDR_LEN];
    char    manu[2]; /* 没有\0 */
    char    chip[2]; /* 没有\0 */
    uint8   day;
    uint8   month;
    uint8   year;
    uint16  version;
}CCO_QUERY_NODE_INFO_ACK_53F4;

typedef struct tagCCO_QUERY_NODE_INFO_ACK_03F1
{
    char    manu[2]; /* 没有\0 */
    char    chip[2]; /* 没有\0 */
    uint8   day;
    uint8   month;
    uint8   year;
    uint16  version;
}CCO_QUERY_NODE_INFO_ACK_03F1;

typedef struct tagCCO_QUERY_NODE_PHASE_REQ
{
    uint32  port;
    uint8   type;
    uint8   addr[CCO_ADDR_LEN];
    uint16  label;
    uint16  index;
}CCO_QUERY_NODE_PHASE_REQ;

typedef struct tagCCO_QUERY_NODE_PHASE_REQ_53F5
{
    uint8   type;
    uint8   addr[CCO_ADDR_LEN];
}CCO_QUERY_NODE_PHASE_REQ_53F5;

typedef struct tagCCO_QUERY_NODE_PHASE_ACK_53F5
{
    uint8   type;
    uint8   addr[CCO_ADDR_LEN];
    uint16  phase;
}CCO_QUERY_NODE_PHASE_ACK_53F5;

typedef struct tagCCO_QUERY_UART_PARA_REQ
{
    uint16  label;
}CCO_QUERY_UART_PARA_REQ;

typedef struct tagCCO_QUERY_UART_PARA_ACK
{
    uint8   currentRate; /* 当前速率 */
    uint8   maxRate;    /* 最高速率 */
    uint16  duration;   /* 恢复默认时长 */
}CCO_QUERY_UART_PARA_ACK;

typedef struct tagCCO_SET_ADDR_REQ
{
    uint32  port;
    uint8   addr[CCO_ADDR_LEN];
}CCO_SET_ADDR_REQ;

typedef struct tagCCO_SET_STA_REPORT_CONFIG
{
    uint8   type;       /* 通信对象类型 */
    uint8   addr[CCO_ADDR_LEN];    /* 任务对象地址 */
    uint8   flag;       /* 上报状态标志（0禁止，1允许） */
}CCO_SET_STA_REPORT_CONFIG;

#define CCO_MAX_CONFIG_NUM  200
typedef struct tagCCO_SET_STA_REPORT_REQ
{
    uint32  port;
    uint8   num;        /* 设置数量 */
    CCO_SET_STA_REPORT_CONFIG config[CCO_MAX_CONFIG_NUM]; /* 配置列表 */
}CCO_SET_STA_REPORT_REQ;

typedef struct tagCCO_START_GET_BROAD_DELAY_REQ
{
    uint32  port;       /* 端口号 */
    uint8   type;       /* 通信对象类型 */
    uint8   duration;   /* 广播时长（单位：分钟） */
}CCO_START_GET_BROAD_DELAY_REQ;

typedef struct tagCCO_BROADCAST_REQ
{
    uint32  port;       /* 端口号 */
    uint8   type;       /* 通信对象类型 */
    uint8   duration;   /* 广播时长（单位：分钟） */
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 报文内容（校正过时标的报文） */
}CCO_BROADCAST_REQ;

typedef struct tagCCO_START_STA_REG_REQ
{
    uint32  port;       /* 端口号 */
    uint16  duration;   /* 允许执行时间（单位：分钟） */
}CCO_START_STA_REG_REQ;

typedef struct tagCCO_START_PREVIEW_EXEC_REQ
{
    uint32  port;       /* 端口号 */
    uint8   type;       /* 预告类型:短预告      （0），（数据长度为1字节）;长预告      （1），（数据长度为2字节） */
}CCO_START_PREVIEW_EXEC_REQ;

typedef struct tagCCO_AMR_TASK
{
    uint16  id;         /* 任务序号 */
    uint8   type;       /* 通讯对象类型 */
    uint8   addr[CCO_ADDR_LEN];    /* 任务对象地址 */
}CCO_AMR_TASK;

#define CCO_MAX_TASK_NUM    500
typedef struct tagCCO_PREVIEW_METER_OBJ_REQ
{
    uint32  port;       /* 端口号 */
    uint16  num;        /* 预告数量 */
    uint8   modify;     /* 延时修改 */
    //uint16  taskNum;    /* 任务配置数量 */
    CCO_AMR_TASK    task[CCO_MAX_TASK_NUM];     /* 任务配置 */
}CCO_PREVIEW_METER_OBJ_REQ;

typedef struct tagCCO_SET_PRTL_REQ
{
    uint32  port;       /* 端口号 */
    uint8   type;       /* 模式字:国网（1）,福建     （2） */
}CCO_SET_PRTL_REQ;

typedef struct tagCCO_SET_RATE_REQ
{
    uint32  port;       /* 端口号 */
    uint8   rate;       /* 波特率:   300bps(0)，600bps(1)，1200bps(2)，2400bps(3)，4800bps(4)，7200bps(5)，9600bps(6)，19200bps(7)，38400bps(8)，57600bps(9)，115200bps(10)           */
}CCO_SET_RATE_REQ;

typedef struct tagCCO_SET_RESTORE_DURATION_REQ
{
    uint32  port;       /* 端口号 */
    uint16  duration;   /* 时长（单位：分钟） */
}CCO_SET_RESTORE_DURATION_REQ;

typedef struct tagCCO_SET_MAX_RATE_REQ
{
    uint32  port;       /* 端口号 */
    uint8   rate;       /* 波特率:   300bps(0)，600bps(1)，1200bps(2)，2400bps(3)，4800bps(4)，7200bps(5)，9600bps(6)，19200bps(7)，38400bps(8)，57600bps(9)，115200bps(10)           */
}CCO_SET_MAX_RATE_REQ;

typedef struct tagCCO_DATA_FWD_REQ
{
    uint8   seq3762;    /* 数据转发携带的3762报文seq */
    uint8   newSeq;     /* 本地APP实际下发的3762报文seq */
    uint16  index;
    uint16  label;
    uint32  port;       /* 端口号 */
    uint8   type;       /* 通信对象类型 */
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 发送数据内容 */
}CCO_DATA_FWD_REQ;

typedef struct tagCCO_DATA_FWD_ACK_02F1
{
    uint8   type;       /* 规约类型 */
    uint8   bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 报文内容 */
}CCO_DATA_FWD_ACK_02F1;

typedef struct tagCCO_DATA_TRANS_REQ
{
    uint8   seq3762;    /* 数据转发携带的3762报文seq */
    uint8   newSeq;     /* 本地APP实际下发的3762报文seq */
    uint16  index;
    uint16  label;
    uint32  port;       /* 端口号 */
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 通信地址 */
    uint8   ctrl;       /* 通讯控制字 */
    uint16  frameOvertime;  /* 帧超时时间 */
    uint16  charOvertime;   /* 字符超时时间 */
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 发送数据内容 */
}CCO_DATA_TRANS_REQ;

typedef struct tagCCO_DATA_TRANS_ACK_52F1
{
    uint8   type;       /* 本次返回报文的通信对象类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 通信地址 */
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 报文内容 */
}CCO_DATA_TRANS_ACK_52F1;

#define CCO_ADDR_MAX_NUM    200
typedef struct tagCCO_REPORT_STA_INFO
{
    uint32  port;
    uint16  num;
    uint8   addr[CCO_ADDR_MAX_NUM][CCO_ADDR_LEN];
}CCO_REPORT_STA_INFO;

typedef struct tagCCO_REPORT_STA_INFO_REQ_56F1
{
    uint8   num;
    uint8   addr[CCO_ADDR_MAX_NUM][CCO_ADDR_LEN];
}CCO_REPORT_STA_INFO_REQ_56F1;

typedef struct tagCCO_REPORT_EVENT
{
    uint32  port;
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 数据帧 */
}CCO_REPORT_EVENT;

typedef struct tagCCO_REPORT_EVENT_56F2
{
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint8   bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 数据帧 */
}CCO_REPORT_EVENT_56F2;

typedef struct tagCCO_REPORT_BROAD_DELAY
{
    uint32  port;
    uint16  delay;      /* 通信延时 */
}CCO_REPORT_BROAD_DELAY;

typedef struct tagCCO_REPORT_BROAD_DELAY_56F5
{
    uint16  delay;      /* 通信延时 */
}CCO_REPORT_BROAD_DELAY_56F5;

typedef struct tagCCO_REPORT_BROAD_DELAY_ACK
{
    uint32  port;
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 数据帧 */
}CCO_REPORT_BROAD_DELAY_ACK;

typedef struct tagCCO_REPORT_METER_REQ
{
    uint32  port;
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint16  delay;      /* 信道延时时间（单位：10ms） */
}CCO_REPORT_METER_REQ;

typedef struct tagCCO_REPORT_METER_REQ_56F3F13
{
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint16  delay;      /* 信道延时时间（单位：10ms） */
}CCO_REPORT_METER_REQ_56F3F13;

typedef struct tagCCO_REPORT_METER_ACK
{
    uint32  port;
    uint16  schemeId;   /* 方案号 */
    uint16  taskId;     /* 任务序号 */
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint8   prtl;       /* 协议类型 */
    uint8   lengthType; /* 抄读类型（0:数据长度为1字节）（1:数据长度为2字节） */
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 数据帧 */
}CCO_REPORT_METER_ACK;

typedef struct tagCCO_REPORT_PREVIEW_OBJ
{
    uint32  port;
    uint16  schemeId;   /* 方案号 */
    uint16  taskId;     /* 任务序号 */
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[2048];  /* 数据帧 */
}CCO_REPORT_PREVIEW_OBJ;

typedef struct tagCCO_REPORT_PREVIEW_OBJ_56F4
{
    uint16  schemeId;   /* 方案号 */
    uint16  taskId;     /* 任务序号 */
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint8   bufLen;     /* 报文长度 */
    uint8   buf[0];     /* 数据帧 */
}CCO_REPORT_PREVIEW_OBJ_56F4;

typedef struct tagCCO_REPORT_PREVIEW_OBJ_56F14
{
    uint16  schemeId;   /* 方案号 */
    uint16  taskId;     /* 任务序号 */
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint16  bufLen;     /* 报文长度 */
    uint8   buf[0];     /* 数据帧 */
}CCO_REPORT_PREVIEW_OBJ_56F14;

typedef struct tagCCO_REPORT_PREVIEW_OBJ_ACK
{
    uint32  port;
    uint16  schemeId;   /* 方案号 */
    uint16  taskId;     /* 任务序号 */
    uint8   type;       /* 通信类型 */
    uint8   addr[CCO_ADDR_LEN]; /* 任务对象地址 */
    uint8   lengthType; /* 状态类型（0:短数据,数据长度为1字节）（1:长数据,数据长度为2字节） */
    uint8   status;     /* 状态确认:否认(0),确认(1),有后续任务(2) */
    uint8   errorFlag;
    uint8   error;        /* 错误信息 */
}CCO_REPORT_PREVIEW_OBJ_ACK;

typedef struct tagCCO_AMR_REQ_T
{
    CCO_QUERY_NODE_INFO_REQ     queryNodeInfoReq;
    CCO_QUERY_NODE_PHASE_REQ    queryNodePhaseReq;
    CCO_QUERY_UART_PARA_REQ     queryUartParaReq;
    CCO_SET_ADDR_REQ            setAddrReq;
    CCO_SET_STA_REPORT_REQ      setStaReportReq;
    CCO_START_GET_BROAD_DELAY_REQ startGetBroadDelayReq;
    CCO_BROADCAST_REQ           broadcastReq;
    CCO_START_STA_REG_REQ       startStaRegReq;
    CCO_START_PREVIEW_EXEC_REQ  startPreviewExecReq;
    CCO_PREVIEW_METER_OBJ_REQ   previewMeterObjReq;
    CCO_SET_PRTL_REQ            setPrtlReq;
    CCO_SET_RATE_REQ            setRateReq;
    CCO_SET_RESTORE_DURATION_REQ setRestoreDurationReq;
    CCO_SET_MAX_RATE_REQ        setMaxRateReq;
    CCO_DATA_FWD_REQ            dataFwdReq;
    CCO_DATA_TRANS_REQ          dataTransReq;
}CCO_AMR_REQ_T;

typedef struct tagCCO_AMR_REPORT_T
{
    CCO_REPORT_STA_INFO         reportStaInfo;
    CCO_REPORT_EVENT            reportEvent;
    CCO_REPORT_METER_REQ        reportMeterReq;
    CCO_REPORT_PREVIEW_OBJ      reportPreviewObj;
    CCO_REPORT_BROAD_DELAY      reportBroadDelay;
}CCO_AMR_REPORT_T;

typedef struct tagCCO_AMR_ACK_T
{
    CCO_QUERY_PARA_ACK          queryParaAck;
    CCO_QUERY_NODE_INFO_ACK     queryNodeInfoAck;
    CCO_QUERY_UART_PARA_ACK     queryUartParaAck;
    CCO_REPORT_METER_ACK        reportMeterAck;
    CCO_REPORT_PREVIEW_OBJ_ACK  reportPreviewObjAck;
    CCO_REPORT_BROAD_DELAY_ACK  reportBroadDelayAck;
}CCO_AMR_ACK_T;

typedef struct tagCCO_AMR_T
{
    CCO_AMR_FLAG_T      flag;
    CCO_AMR_REQ_T       req;
    CCO_AMR_REPORT_T    report;
    CCO_AMR_ACK_T       ack;
}CCO_AMR_T;

typedef struct tagCCO_QUICK_DECODE_T
{
    uint8   AFN;                                //应用功能码
    uint16  Fn;                                 //信息类标识
    uint8   RInfo[6];                           //信息域
}CCO_QUICK_DECODE_T;

#pragma pack()

#define CCO_HW_INIT         1   /* 硬件初始化（复位） */
#define CCO_PARA_INIT       2   /* 参数区初始化（恢复至出厂设置） */
#define CCO_DATA_INIT       3   /* 数据区初始化（节点侦听信息） */

#define CCO_OBJ_TMN         0   /* 终端本体 */
#define CCO_OBJ_COMM        1   /* 本地通信模块通信芯片模块 */
#define CCO_OBJ_METER       3   /* 电能表类设备 */

#define CCO_MAX_ATTEMPT_CNT 3   /* 最大重试次数 */

#define CCO_UDP_PORT        15000
#define CCO_UDP_PORT_MAX    15500    /* UDP指定最大端口号 */

typedef enum tagCCO_COMM_RATE_E
{
    CCO_COMM_RATE_300   = 0,
    CCO_COMM_RATE_600   = 1,
    CCO_COMM_RATE_1200  = 2,
    CCO_COMM_RATE_2400  = 3,
    CCO_COMM_RATE_4800  = 4,
    CCO_COMM_RATE_7200  = 5,
    CCO_COMM_RATE_9600  = 6,
    CCO_COMM_RATE_19200 = 7,
    CCO_COMM_RATE_38400 = 8,
    CCO_COMM_RATE_57600 = 9,
    CCO_COMM_RATE_115200 = 10,
    CCO_COMM_RATE_MAX
}CCO_COMM_RATE_E;

uint8 set_len_offset(uint16 eleLen, uint8 *indata);
void CcoInitMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoQueryParaMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoQueryAddrMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoQueryNodeInfoMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoQueryNodePhaseMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoQueryUartParaMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoQueryPrtlModeMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoSetAddrMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoSetStaReportMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoStartGetBroadDelayMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoBroadcastMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoStartStaRegMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoEndCurrentTaskMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoStartPreviewExecMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoPreviewMeterObjMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoSetPrtlMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoSetRateMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoSetRestoreDurationMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoSetMaxRateMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoDataFwdMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoDataTransMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportStaInfoMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportEventMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportMeterReqMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportMeterAck(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportPreviewObjMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportPreviewObjAckMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportBroadDelayMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportBroadDelayAckMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportBroadFinishMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoReportBroadFinishAckMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
void CcoQueryReadModeMsg(INFO_DATA_T *infoMsgData, CCO_AMR_T *amr);
int CcoCommFunDeal(AppData_T *SendAppData,AppData_T *RecvAppData, uint8 nArrIndex);
uint32 CcoUnPackFrameData(uint8 *FrameBuff, uint16 FrameLen, AppData_T *AppData);
uint16 CcoPackFrameData(AppData_T *appData, uint8 index);
uint16 CcoChangeFrameSeq(AppData_T *appData, uint8 *inBuf, uint16 inBufLen, uint8 *seq, uint8 index);
uint32 CcoPickFrame(CCO_AMR_T *amr, uint8 *buf, uint16 bufLen, uint8 index);
uint32 CcoModeNegotiateProc(uint8 index);
uint32 CcoFuJianAmr(CCO_AMR_T *amr, uint8 index);

extern CCO_AMR_T   g_CcoAmr;

#endif

#endif
