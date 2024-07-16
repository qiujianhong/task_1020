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
    uint16  taskObjNum;     /* ��Ԥ������������� */
    uint8   meterObjNum;    /* Ԥ�泭���������� */
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
    uint16  year;       /**< ��            */
    uint8   month;      /**< ��            */
    uint8   mday;       /**< ��            */
    uint8   wday;       /**< ��            */
}CCO_DATE_T;

typedef struct tagCCO_QUERY_NODE_INFO_ACK
{
    char    manu[2]; /* û��\0 */
    char    chip[2]; /* û��\0 */
    CCO_DATE_T date;
    uint16  version;
}CCO_QUERY_NODE_INFO_ACK;

typedef struct tagCCO_QUERY_NODE_INFO_ACK_53F4
{
    uint8   type;
    uint8   addr[CCO_ADDR_LEN];
    char    manu[2]; /* û��\0 */
    char    chip[2]; /* û��\0 */
    uint8   day;
    uint8   month;
    uint8   year;
    uint16  version;
}CCO_QUERY_NODE_INFO_ACK_53F4;

typedef struct tagCCO_QUERY_NODE_INFO_ACK_03F1
{
    char    manu[2]; /* û��\0 */
    char    chip[2]; /* û��\0 */
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
    uint8   currentRate; /* ��ǰ���� */
    uint8   maxRate;    /* ������� */
    uint16  duration;   /* �ָ�Ĭ��ʱ�� */
}CCO_QUERY_UART_PARA_ACK;

typedef struct tagCCO_SET_ADDR_REQ
{
    uint32  port;
    uint8   addr[CCO_ADDR_LEN];
}CCO_SET_ADDR_REQ;

typedef struct tagCCO_SET_STA_REPORT_CONFIG
{
    uint8   type;       /* ͨ�Ŷ������� */
    uint8   addr[CCO_ADDR_LEN];    /* ��������ַ */
    uint8   flag;       /* �ϱ�״̬��־��0��ֹ��1���� */
}CCO_SET_STA_REPORT_CONFIG;

#define CCO_MAX_CONFIG_NUM  200
typedef struct tagCCO_SET_STA_REPORT_REQ
{
    uint32  port;
    uint8   num;        /* �������� */
    CCO_SET_STA_REPORT_CONFIG config[CCO_MAX_CONFIG_NUM]; /* �����б� */
}CCO_SET_STA_REPORT_REQ;

typedef struct tagCCO_START_GET_BROAD_DELAY_REQ
{
    uint32  port;       /* �˿ں� */
    uint8   type;       /* ͨ�Ŷ������� */
    uint8   duration;   /* �㲥ʱ������λ�����ӣ� */
}CCO_START_GET_BROAD_DELAY_REQ;

typedef struct tagCCO_BROADCAST_REQ
{
    uint32  port;       /* �˿ں� */
    uint8   type;       /* ͨ�Ŷ������� */
    uint8   duration;   /* �㲥ʱ������λ�����ӣ� */
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* �������ݣ�У����ʱ��ı��ģ� */
}CCO_BROADCAST_REQ;

typedef struct tagCCO_START_STA_REG_REQ
{
    uint32  port;       /* �˿ں� */
    uint16  duration;   /* ����ִ��ʱ�䣨��λ�����ӣ� */
}CCO_START_STA_REG_REQ;

typedef struct tagCCO_START_PREVIEW_EXEC_REQ
{
    uint32  port;       /* �˿ں� */
    uint8   type;       /* Ԥ������:��Ԥ��      ��0���������ݳ���Ϊ1�ֽڣ�;��Ԥ��      ��1���������ݳ���Ϊ2�ֽڣ� */
}CCO_START_PREVIEW_EXEC_REQ;

typedef struct tagCCO_AMR_TASK
{
    uint16  id;         /* ������� */
    uint8   type;       /* ͨѶ�������� */
    uint8   addr[CCO_ADDR_LEN];    /* ��������ַ */
}CCO_AMR_TASK;

#define CCO_MAX_TASK_NUM    500
typedef struct tagCCO_PREVIEW_METER_OBJ_REQ
{
    uint32  port;       /* �˿ں� */
    uint16  num;        /* Ԥ������ */
    uint8   modify;     /* ��ʱ�޸� */
    //uint16  taskNum;    /* ������������ */
    CCO_AMR_TASK    task[CCO_MAX_TASK_NUM];     /* �������� */
}CCO_PREVIEW_METER_OBJ_REQ;

typedef struct tagCCO_SET_PRTL_REQ
{
    uint32  port;       /* �˿ں� */
    uint8   type;       /* ģʽ��:������1��,����     ��2�� */
}CCO_SET_PRTL_REQ;

typedef struct tagCCO_SET_RATE_REQ
{
    uint32  port;       /* �˿ں� */
    uint8   rate;       /* ������:   300bps(0)��600bps(1)��1200bps(2)��2400bps(3)��4800bps(4)��7200bps(5)��9600bps(6)��19200bps(7)��38400bps(8)��57600bps(9)��115200bps(10)           */
}CCO_SET_RATE_REQ;

typedef struct tagCCO_SET_RESTORE_DURATION_REQ
{
    uint32  port;       /* �˿ں� */
    uint16  duration;   /* ʱ������λ�����ӣ� */
}CCO_SET_RESTORE_DURATION_REQ;

typedef struct tagCCO_SET_MAX_RATE_REQ
{
    uint32  port;       /* �˿ں� */
    uint8   rate;       /* ������:   300bps(0)��600bps(1)��1200bps(2)��2400bps(3)��4800bps(4)��7200bps(5)��9600bps(6)��19200bps(7)��38400bps(8)��57600bps(9)��115200bps(10)           */
}CCO_SET_MAX_RATE_REQ;

typedef struct tagCCO_DATA_FWD_REQ
{
    uint8   seq3762;    /* ����ת��Я����3762����seq */
    uint8   newSeq;     /* ����APPʵ���·���3762����seq */
    uint16  index;
    uint16  label;
    uint32  port;       /* �˿ں� */
    uint8   type;       /* ͨ�Ŷ������� */
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* ������������ */
}CCO_DATA_FWD_REQ;

typedef struct tagCCO_DATA_FWD_ACK_02F1
{
    uint8   type;       /* ��Լ���� */
    uint8   bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* �������� */
}CCO_DATA_FWD_ACK_02F1;

typedef struct tagCCO_DATA_TRANS_REQ
{
    uint8   seq3762;    /* ����ת��Я����3762����seq */
    uint8   newSeq;     /* ����APPʵ���·���3762����seq */
    uint16  index;
    uint16  label;
    uint32  port;       /* �˿ں� */
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ͨ�ŵ�ַ */
    uint8   ctrl;       /* ͨѶ������ */
    uint16  frameOvertime;  /* ֡��ʱʱ�� */
    uint16  charOvertime;   /* �ַ���ʱʱ�� */
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* ������������ */
}CCO_DATA_TRANS_REQ;

typedef struct tagCCO_DATA_TRANS_ACK_52F1
{
    uint8   type;       /* ���η��ر��ĵ�ͨ�Ŷ������� */
    uint8   addr[CCO_ADDR_LEN]; /* ͨ�ŵ�ַ */
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* �������� */
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
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* ����֡ */
}CCO_REPORT_EVENT;

typedef struct tagCCO_REPORT_EVENT_56F2
{
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint8   bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* ����֡ */
}CCO_REPORT_EVENT_56F2;

typedef struct tagCCO_REPORT_BROAD_DELAY
{
    uint32  port;
    uint16  delay;      /* ͨ����ʱ */
}CCO_REPORT_BROAD_DELAY;

typedef struct tagCCO_REPORT_BROAD_DELAY_56F5
{
    uint16  delay;      /* ͨ����ʱ */
}CCO_REPORT_BROAD_DELAY_56F5;

typedef struct tagCCO_REPORT_BROAD_DELAY_ACK
{
    uint32  port;
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* ����֡ */
}CCO_REPORT_BROAD_DELAY_ACK;

typedef struct tagCCO_REPORT_METER_REQ
{
    uint32  port;
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint16  delay;      /* �ŵ���ʱʱ�䣨��λ��10ms�� */
}CCO_REPORT_METER_REQ;

typedef struct tagCCO_REPORT_METER_REQ_56F3F13
{
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint16  delay;      /* �ŵ���ʱʱ�䣨��λ��10ms�� */
}CCO_REPORT_METER_REQ_56F3F13;

typedef struct tagCCO_REPORT_METER_ACK
{
    uint32  port;
    uint16  schemeId;   /* ������ */
    uint16  taskId;     /* ������� */
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint8   prtl;       /* Э������ */
    uint8   lengthType; /* �������ͣ�0:���ݳ���Ϊ1�ֽڣ���1:���ݳ���Ϊ2�ֽڣ� */
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* ����֡ */
}CCO_REPORT_METER_ACK;

typedef struct tagCCO_REPORT_PREVIEW_OBJ
{
    uint32  port;
    uint16  schemeId;   /* ������ */
    uint16  taskId;     /* ������� */
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[2048];  /* ����֡ */
}CCO_REPORT_PREVIEW_OBJ;

typedef struct tagCCO_REPORT_PREVIEW_OBJ_56F4
{
    uint16  schemeId;   /* ������ */
    uint16  taskId;     /* ������� */
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint8   bufLen;     /* ���ĳ��� */
    uint8   buf[0];     /* ����֡ */
}CCO_REPORT_PREVIEW_OBJ_56F4;

typedef struct tagCCO_REPORT_PREVIEW_OBJ_56F14
{
    uint16  schemeId;   /* ������ */
    uint16  taskId;     /* ������� */
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint16  bufLen;     /* ���ĳ��� */
    uint8   buf[0];     /* ����֡ */
}CCO_REPORT_PREVIEW_OBJ_56F14;

typedef struct tagCCO_REPORT_PREVIEW_OBJ_ACK
{
    uint32  port;
    uint16  schemeId;   /* ������ */
    uint16  taskId;     /* ������� */
    uint8   type;       /* ͨ������ */
    uint8   addr[CCO_ADDR_LEN]; /* ��������ַ */
    uint8   lengthType; /* ״̬���ͣ�0:������,���ݳ���Ϊ1�ֽڣ���1:������,���ݳ���Ϊ2�ֽڣ� */
    uint8   status;     /* ״̬ȷ��:����(0),ȷ��(1),�к�������(2) */
    uint8   errorFlag;
    uint8   error;        /* ������Ϣ */
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
    uint8   AFN;                                //Ӧ�ù�����
    uint16  Fn;                                 //��Ϣ���ʶ
    uint8   RInfo[6];                           //��Ϣ��
}CCO_QUICK_DECODE_T;

#pragma pack()

#define CCO_HW_INIT         1   /* Ӳ����ʼ������λ�� */
#define CCO_PARA_INIT       2   /* ��������ʼ�����ָ����������ã� */
#define CCO_DATA_INIT       3   /* ��������ʼ�����ڵ�������Ϣ�� */

#define CCO_OBJ_TMN         0   /* �ն˱��� */
#define CCO_OBJ_COMM        1   /* ����ͨ��ģ��ͨ��оƬģ�� */
#define CCO_OBJ_METER       3   /* ���ܱ����豸 */

#define CCO_MAX_ATTEMPT_CNT 3   /* ������Դ��� */

#define CCO_UDP_PORT        15000
#define CCO_UDP_PORT_MAX    15500    /* UDPָ�����˿ں� */

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
