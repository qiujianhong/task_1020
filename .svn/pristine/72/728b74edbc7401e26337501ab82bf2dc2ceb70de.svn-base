#ifndef __AREA_H__
#define __AREA_H__

#include "commClass.h"
#include "EventFun.h"

extern FACTORY_VER_TMP_T g_tVer;

typedef struct
{
    uint8  factory;  //运营商 移动CMCC（0），电信CTCC（1），联通CUCC（2），未知（255）
    uint8  network;  //网络制式 2G（0），3G（1），4G（2），5G（3），未知（255）
    uint8  ip1[4];   //主用ip
    uint32 port1;    //主用端口
    uint8  ip2[4];
    uint32 port2;
    char apn[64];   
    char username[64];
    char password[64];
}GRPS_PARAM_T;  //江苏默认的一些网络参数

#ifdef AREA_ZHEJIANG
extern OOP_DAR_E area_buf_to_data_oop_meter(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
extern OOP_DAR_E area_buf_to_data_oop_meter_extend_object(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
extern OOP_DAR_E area_class11_invoke_act_oop_meter(OAD_INFO_T *pOdaInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
extern OOP_DAR_E area_data_to_buf_oop_meter(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
extern int class_zhejiang_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo);
extern int class_zhejiang_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo);
extern OOP_DAR_E class8_invoke_get_cbir_result(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, RESULT_DATA_T *outData, uint16 outDataMax);
void app_setdefault_cfg_zhejiang(int sockdbfd);

#endif
#ifdef AREA_JIANGXI

#endif

#ifdef AREA_SHAANXI
typedef struct
{
    EVENT_COMM_INFO 	 eventCommInfo;
	OOP_VISIBLEVAR32_T   iccidBef;   //事件变更前 ICCID 
	OOP_VISIBLEVAR32_T	 iccidCur;   //事件变更后 ICCID
    uint8             	 isFirst;    //第一次连接主站标记
}EVENT_3199_INFO;
EVENT_3199_INFO event_3199;   //线路开关变位事件

int Init3199Para();
int CheckERC_3199(CHANNEL_CACHE_T *ptChannel);

void sta_frozen_wireless_proc();

OOP_DAR_E data_to_buf_ICCID(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
int event_tmn_ICCID(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen);

void app_setdefault_cfg_shaanxi(int sockdbfd);

#endif

#ifdef AREA_HUNAN
#include "../../communication/include/sms_comm.h"
extern uint16 g_val_a[OOP_MAX_POWERCUT_MET];
extern int read_pdata_xram(const char *filename, char *buf, unsigned offset, unsigned len);
extern int write_pdata_xram(const char *filename, const char *buf, unsigned offset, unsigned len);
typedef struct
{
    EVENT_COMM_INFO 	 eventCommInfo;
    uint8       breakstate;      //线路开关状态

    uint8             	 initflag;                     //参数初始化标记，标识参数是否初始化
}EVENT_3360_INFO;

typedef struct
{
    uint8 isvalid;
    uint16 pn;  //pn0表示终端
    //uint8 eventflag;    //事件标识 1表示已经发生了判断恢复事件 0表示正常
    OOP_DATETIME_S_T begintime;   //事件发生时间
    //uint32 endtime;     //事件结束时间
}EVENT_3361_METER;

#define EVENT_3361_RECORD_COUNT  100
typedef struct
{
    EVENT_COMM_INFO 	 eventCommInfo;
    OOP_TemperatureWarningParam_T     param;      //环境温度告警事件配置参数
    uint8             	 initflag;                     //参数初始化标记，标识参数是否初始化
    EVENT_3361_METER  info[EVENT_3361_RECORD_COUNT];        //记录温度告警事件是否发生以及发生时间
}EVENT_3361_INFO;

typedef struct
{
    EVENT_COMM_INFO 	 eventCommInfo;
    uint8 state;        //告警状态 0正常 1告警
    uint8 laststate;
    uint8             	 initflag;                     //参数初始化标记，标识参数是否初始化
}EVENT_3362_INFO;

EVENT_3360_INFO event_3360;   //线路开关变位事件
EVENT_3361_INFO event_3361;     //环境温度告警事件
EVENT_3362_INFO event_3362;     //烟雾告警事件
EVENT_3362_INFO event_3363;     //浸水告警事件 和烟雾告警事件共用结构体
int Init3360Para();
int CheckERC_3360(int clientid,uint8* databuf,uint16 datalen);
int32 data_to_buf_evt_oad_hunan(OOP_OAD_U oad, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
int Init3361Para();
int CheckERC_3361(int clientid,uint8 logicid,uint16 mtr_id, OOP_METER_T oopMeter,uint8* databuf,uint16 datalen);
int Init3362Para();
int CheckERC_3362(int clientid,uint8* databuf,uint16 datalen);
int Init3363Para();
int CheckERC_3363(int clientid,uint8* databuf,uint16 datalen);
int class_hunan_oadinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo);
int class_hunan_omdinfo_get(OOP_OAD_U oad, OAD_INFO_T *pstOdaInfo);
int get_smoke_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int get_water_state(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

OOP_DAR_E data_to_buf_class3_oop_temp(OAD_INFO_T *pOadInfo, DATA_CONVERT_T *dsdata);
OOP_DAR_E data_to_buf_impeover(const OAD_INFO_T *pOadInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E evt_cfg3361_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset);
OOP_DAR_E evt_cfg31f2_set(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 inLen, uint32 *offset);
OOP_DAR_E evt_cfg3361_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E evt_cfg31f2_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class11_nodetopology_get(const OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_act_topology(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
int event_tmn_impeover(OAD_INFO_T *pOadInfo, RECORD_UNIT_T unit, RELY_INFO_T *relyInfo, uint8 **ppOutBuf, uint32 *outLen);
OOP_DAR_E data_to_buf_evt_impe(const OAD_INFO_T *pOdaInfo, void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
OOP_DAR_E class8_invoke_set_oia_60(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class8_invoke_get_oia_60(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E buf_to_data_class8_sensor_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
OOP_DAR_E data_to_buf_class8_sensor_param(const OAD_INFO_T *pOadInfo, DATA_CONVERT_T *convert);
int sms_msg_unpack(COMM_SMS_HANDLE_T *ptHandle,uint8 *buf,uint16 buflen);
#endif

#define APDU_DEF_CFG_MAX 128
typedef struct
{
    uint8       ifSetOrAct;
    uint8       *apduBuf;
    uint16      *apduBufLen;
    uint32      *apduNum;
}APDU_DEF_CFG_T;


#if AREA_ANHUI

void app_setdefault_cfg_anhui(int sockdbfd);
uint16 rpt_plan_normal_taskid_get(uint16 value,uint8 unit,REPORT_CONTEXT *rptContext);

#endif

#if AREA_JIANGSU
void app_setdefault_cfg_jiangsu(int sockdbfd);
void apn_group_default_set(OOP_APNGROUP_T *apnGroup);

#endif

#if AREA_SHANDONG

void app_setdefault_cfg_shandong(int sockdbfd);
uint16 rpt_plan_normal_taskid_get(uint16 value,uint8 unit,REPORT_CONTEXT    *rptContext);
BOOL rpt_normal_task_finish_check(DB_CLIENT client,uint16 taskid,time_t *lasttime);
#endif

#ifdef AREA_HUBEI

void app_setdefault_cfg_hubei(int sockdbfd);

#endif

#ifdef AREA_FUJIAN
//福建规约中群组档案缓存
#define     GROUPMETER_NUM_ALL        1500
#define     TRANS_TASK_NUM           (256)  //可以加载的透明方案最大值
#define     REPORT_PLAN_NUM          (256) //可以加载的上报方案最大值

typedef struct tag_OOP_GROUPMETER_ALL
{
    OOP_TSA_T  groupAddr;                      //群组地址
    uint8      choice;                         //ms类型
    uint32     num;                            //关联档案数量
    OOP_ADDR_T addr[GROUPMETER_NUM_ALL];       //关联档案
}OOP_GROUPMETER_ALL_T;

typedef struct LINK_GROUPMETER_ALL
{
    uint16                      no;
    OOP_GROUPMETER_ALL_T        oopGroup;
    struct LINK_GROUPMETER_ALL *next;
}LINK_GROUPMETER_ALL_T;

//福建规约中方案配置统计
typedef struct LINK_CONFIG_STAT
{
    uint16                  no;
    OOP_CONFIG_STAT_T       oopConfigStat;
    struct LINK_CONFIG_STAT *next;
}LINK_CONFIG_STAT_T;

//福建规约中任务方案
typedef struct LINK_TRANS_TASK
{
    uint16                 no;
    OOP_TRANS_TASK_T       oopTask;
    struct LINK_TRANS_TASK *next;
}LINK_TRANS_TASK_T;

//福建规约中具体任务
typedef struct LINK_TRANS_NODE
{
    uint16                 no;
    OOP_TRANS_TASK_NODE_T  oopNode;
    struct LINK_TRANS_NODE *next;
}LINK_TRANS_NODE_T;

/*记录型档案数据刷选条件*/
typedef struct _READ_INF_T
{
    uint32           startTask;                      //信息点
    uint32           endTask;                        //信息点
    uint32           startNode;                      //信息点
    uint32           endNode;                        //信息点
    OOP_MSP_T        ms;                             //ms集合
    OOP_DATETIME_S_T startTime;                      //存储时标
    OOP_DATETIME_S_T endTime;                        //存储时标
    OOP_TI_T         ti;                             //时间间隔
    OOP_TSA_T        tsa;                            //通信地址筛选
}READ_INF_T;


extern OOP_GATHER_E		gGather;							//外部设备数据采集模式
extern OOP_TRANS_EVENT_STATUS_T gTransEvtStatus;				//透明事件当前状态
uint16 appmain_get_singal_info(uint8 no, uint8* recvbuf, uint16 buflen);
uint16 appmain_send_powerup_evnet(void);
int check_battery_status(int clientid);
int check_battery_status_run(int clientid);
int check_power_status(int clientid, int state);
int evt_write_data_FJ(DB_CLIENT fd, mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType);
int evt3106_write_data_FJ(DB_CLIENT fd, uint32 tmnDownAddr);
int get_trans_event_status(DB_CLIENT hUdp, OOP_TRANS_EVENT_STATUS_T *pStatus, uint16 *pCurNum, uint32 *pMaxStorageNo);

OOP_DAR_E class8_invoke_get_oia_4e_fj(OAD_INFO_T *pOadInfo, APDU_INFO_T *pApduInfo, const uint8 *inData, uint16 InDataLen, RESULT_DATA_T *outData, uint16 outDataMax);
OOP_DAR_E class8_invoke_set_oia_6e_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class8_invoke_act_oia_6e_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class8_invoke_get_oia_6e_fj(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 InDataLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_transparent_results(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_transparent_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_transparent_task_node(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_group_meter(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_report_plan_fujian(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_acq_status(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_transparent_event(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_get_transparent_acq_status(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);

OOP_DAR_E class11_invoke_act_group_meter(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_act_transparent_task(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_act_transparent_task_node(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_act_transparent_report(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);
OOP_DAR_E class11_invoke_act_transparent_event(OAD_INFO_T* pOadInfo, APDU_INFO_T* pApduInfo, const uint8* inData, uint16 inLen, RESULT_DATA_T* outData, uint16 outDataMax);

OOP_DAR_E buf_to_data_evt_asobjattr(const OAD_INFO_T* pOadInfo, const void* srcBuf, uint32 srcLen, uint32* pOffset, void* dstData, uint32 dstLen);
OOP_DAR_E buf_to_data_battery_status(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E buf_to_data_singal_info(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E buf_to_data_group_meter(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E buf_to_data_report_plan_fujian(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E buf_to_data_report_event_fujian(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E buf_to_data_group_meter_num(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E buf_to_data_trans_task(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E buf_to_data_trans_task_node(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E buf_to_data_acq_status(OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);

OOP_DAR_E data_to_buf_transparent_results(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_transparent_event(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_group_meter(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_report_plan_fujian(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_report_event_fujian(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_trans_task_node(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_group_meter_num(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_trans_task(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_acq_status(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_config_stat(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E data_to_buf_transparent_acq_status(const OAD_INFO_T* pOadInfo, DATA_CONVERT_T* convert);
OOP_DAR_E basic_ms_fromfmt698_fj(const void *srcBuf, uint32 srcLen, uint32 *pOffset, void *dstData, uint32 dstLen);
OOP_DAR_E basic_ms_tofmt698_fj(void *dstBuf, uint32 dstLen, uint32 *pOffset, void *srcData, uint32 srcLen);
LINK_GROUPMETER_ALL_T *get_meter_link_stat(DB_CLIENT hUdp);
LINK_GROUPMETER_ALL_T* link_group_destroy(LINK_GROUPMETER_ALL_T* pGroupHead);
LINK_TRANS_NODE_T* link_node_destroy(LINK_TRANS_NODE_T* pNodeHead);
LINK_TRANS_NODE_T *get_node_link_stat(uint16 taskId, uint16 nodeId, DB_CLIENT hUdp);
void set_default_group_meter_fj(int clientid, OOP_OCTETVAR16_T *addr);

/** @brief 任务数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;          //锁 多线程访问
    uint16                 oopTaskNum;     //任务个数
    BOOLEAN                isChanged[TRANS_TASK_NUM]; //变更标记
    uint16                 innerID[TRANS_TASK_NUM];   //内部索引
    BOOLEAN                isVaild[TRANS_TASK_NUM];   //任务是否有效
    OOP_TRANS_TASK_T       oopTask[TRANS_TASK_NUM];   //任务参数
}OOP_TASK_LIST_FJ_T;

extern OOP_TASK_LIST_FJ_T oopTaskList_fj;

/** @brief 上报方案数据缓存结构*/
typedef struct
{
    pthread_mutex_t         mutex;              //锁 多线程访问
    uint16                  oopPlanReportNum;   //方案个数
    BOOLEAN                 isChanged[REPORT_PLAN_NUM];     //变更标记
    uint16                  innerID[REPORT_PLAN_NUM];       //内部索引    
    BOOLEAN                 isVaild[REPORT_PLAN_NUM];       //方案是否有效
    OOP_TRANS_REPORT_T      oopPlanReport[REPORT_PLAN_NUM]; //方案参数
}OOP_PLAN_REPORT_LIST_FJ_T;

extern OOP_PLAN_REPORT_LIST_FJ_T oopPlanReportList_fj;

/** @brief 补报信息*/
typedef struct
{
    uint16                 num;            //需要补报的个数
    uint16                 taskId[REPORT_PLAN_NUM];    //需要补报的方案号，顺序排列
}OOP_RPT_INF_T;

/** @brief 触发补报*/
typedef struct
{
    pthread_mutex_t        mutex;          //锁 多线程访问
    uint16                 rdptr;           //读索引
    uint16                 wtptr;           //写索引
    uint16                 taskId[REPORT_PLAN_NUM];    //需要补报的方案号，顺序排列
}OOP_REPORT_TRIGGER_T;

extern OOP_REPORT_TRIGGER_T oopReportTri;

/** @brief 触发补报*/
typedef struct
{
    pthread_mutex_t        mutex;     //锁 多线程访问
    uint8                  triNum;    //需要补报的事件次数
}OOP_EVTRPT_TRIGGER_T;

extern OOP_EVTRPT_TRIGGER_T oopEventRptTri;

void set_report_trigger(OOP_RPT_INF_T *pRptInf);
int get_report_trigger(OOP_RPT_INF_T *pRptInf);
void set_event_rpt_trigger(void);
uint8 get_event_rpt_trigger(void);
BOOL meter_addr_is_group(uint8 *addr);
void appdb_task_list_record_init_fj(int clientid);
void appdb_plan_report_list_record_init_fj(int clientid);
int app_get_task_list_fj_by_id(uint16 innerID, OOP_TRANS_TASK_T *oopTask);
int app_get_task_list_fj(uint16 taskID, OOP_TRANS_TASK_T *oopTask);
int app_get_plan_report_list_fj(uint8 planID, OOP_TRANS_REPORT_T *oopPlanReport);
int transparent_task_sch_read(DB_CLIENT            hUdp, uint16 taskId, OOP_TRANS_TASK_T *data);
uint16 rsd_get_fujian(DB_CLIENT hUdp, const uint8* inData, int InDataLen, RSD_T* pRSD);
int32 general_param_get_node(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_INF_T *pInReadInf, RELY_INFO_T *pRelyInfo);
int32 general_param_get_acq(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_INF_T *pInReadInf, RELY_INFO_T *pRelyInfo);
int32 general_param_get_acq_status(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_INF_T *pInReadInf, RELY_INFO_T *pRelyInfo);
int32 general_param_get_fj(OOP_PRSD_T *pOopRsd, OOP_RCSD_T *pOopRcsd, APDU_INFO_T *pApduInfo, READ_INF_T *pInReadInf, RELY_INFO_T *pRelyInfo);
uint8 slicing_is_comtinue_ms_fujian(BP_INFO_T* bpInfo, uint16 metsn, READ_RECORD_T* inReadRecord, uint32* oopRecNumPn, uint32 *index);
uint8 slicing_is_comtinue_task_fujian(BP_INFO_T* bpInfo, uint16 metsn, READ_RECORD_T* inReadRecord, uint32* oopRecNumPn);
void app_setdefault_cfg_fujian(int sockdbfd);
BOOL meter_addr_is_legally(uint64 *addr);
OOP_GATHER_E get_tmn_gather();
void area_write_protect_init(DB_CLIENT fd);
int area_write_protect_reduce(DB_CLIENT fd, uint8 type, uint8 reducemin);
void area_write_protect_leftproc(DB_CLIENT fd);
uint8 area_check_set_protect(uint32 uOAD);
uint8 area_check_act_protect(uint32 uOAD);
#endif

#endif
