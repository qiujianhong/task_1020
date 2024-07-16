#ifndef __AREA_H__
#define __AREA_H__

#if AREA_ZHEJIANG
extern uint8 gMeterfileSyncFlag;
void taskmng_meter_search_result_write_area(uint8 pipe,uint8* buf,uint16 len);
BOOL taskmng_meter_file_in_search_result_check(OOP_METER_T *meter);
BOOL taskmng_meter_file_cmp_check(uint8 pipe);

#endif

#ifdef AREA_HUNAN
typedef enum tagSensorType
{
    SENSOR_H_AND_T, /* 湿度温度传感器 */
    SENSOR_WATER,   /* 水浸传感器 */
    SENSOR_SMOKE,   /* 烟感传感器 */
    SENSOR_645_T,   /* 645温感 */
    SENSOR_MAX,
}SENSOR_TYPE_E;

typedef enum tagSensorPrtl
{
    SENSOR_PRTL_MODBUS,  /* Modbus协议 */
    SENSOR_PRTL_645,     /* 645协议 */
    SENSOR_PRTL_MAX,
}SENSOR_PRTL_E;

typedef struct tagHAndTResult
{
    uint16  hum; /* 湿度 */
    int16   tem; /* 温度 */
}H_AND_T_RESULT_T;

typedef union tagSensorResult
{
    H_AND_T_RESULT_T hAndTResult;
    uint8   u8Result;
    int16   a645TResult[3];
    //uint16  u16Result;
    //int16   i16Result;
}SENSOR_RESULT_U;

#define RS485_PORT1 2
#define RS485_PORT2 3
#define RS485_PORT3 4
#define RS485_PORT4 5
#define RS485_PORT5 6
#define RS485_PORT6 7
#define RS485_PORT7 8
#define RS485_PORT8 9

typedef union tagSensorAddr
{
    uint8   modbusAddr;   /* Modbus使用的地址 */
    uint8   mtrAddr[6]; /* 645使用的地址 */
}SENSOR_ADDR_U;

typedef struct tagSensorDevInfo
{
    SENSOR_ADDR_U   addr;
    uint16  speed;
    uint8   needWriteFlag;
    uint8   needMtrFlag; /* 重抄标记 */
    uint8   pipe; /* 抄读口,2到9对应8路485 */
    PARITY_TYPE_E parity; /* 校验方式 */
    SENSOR_RESULT_U result;
    SENSOR_TYPE_E type;
}SENSOR_DEV_INFO_T;

#define SENSOR_DEV_MAX_NUM 8

typedef struct tagLoRaAmr
{
    uint32  devNum;
    //uint32  curMtrIndex;
    //uint32  needMtrFlag; /* 比特位数不能小于SENSOR_DEV_MAX_NUM */
    SENSOR_DEV_INFO_T   dev[SENSOR_DEV_MAX_NUM];
    RS485_RS232_PARA    commPara;
    DB_CLIENT           DBClientHandle;
}LORA_AMR_T;

typedef struct tagSensorAmr
{
    uint8   pipe;
    uint32  devNum;
    //uint32  curMtrIndex;
    //uint32  needMtrFlag; /* 比特位数不能小于SENSOR_DEV_MAX_NUM */
    SENSOR_DEV_INFO_T   dev[SENSOR_DEV_MAX_NUM];
    RS485_RS232_PARA   *commPara;
    DB_CLIENT           DBClientHandle;
}SENSOR_AMR_T;


typedef struct tagSensorSendAttr
{
    SENSOR_PRTL_E prtl; /* 协议类型 */
    uint8   code; /* 功能码 */
    uint16  dataAddr; /* 数据起始地址 */
    uint16  dataNum; /* 数据数量 */
    uint32  di; /* 645DI */
}SENSOR_SEND_ATTR_T;

#pragma pack(1)
typedef struct tagSensorSendPduModbus
{
    uint8   addr; /* 设备地址 */
    uint8   code; /* 功能码 */
    uint16  dataAddr; /* 数据起始地址 */
    uint16  dataNum; /* 数据数量 */
}SENSOR_SEND_PDU_MODBUS_T;

typedef struct tagSensorSendFrameModbus
{
    SENSOR_SEND_PDU_MODBUS_T pdu;
    uint16  checkCode; /* 校验码 */
}SENSOR_SEND_FRAME_MODBUS_T;

typedef union tagSensorSendFrame
{
    SENSOR_SEND_FRAME_MODBUS_T modbusFrame;
    uint8 buf[256];
}SENSOR_SEND_FRAME_U;

typedef struct tagSensorRecvModbus
{
    uint8   addr; /* 设备地址 */
    uint8   code; /* 功能码 */
    uint8   dataLen; /* 数据长度 */
    uint8   data[32];
}SENSOR_RECV_MODBUS_T;

typedef union tagSensorRecvPdu
{
    SENSOR_RECV_MODBUS_T modbusPdu;
    uint8 buf[256];
}SENSOR_RECV_PDU_U;
#pragma pack()

typedef uint32 (*FillFunc)(uint8, SENSOR_DEV_INFO_T *, SENSOR_RECV_PDU_U *);
typedef uint32 (*SaveFunc)(uint8, SENSOR_DEV_INFO_T *, DB_CLIENT);

typedef struct tagSensorRecvAttr
{
    SENSOR_PRTL_E prtl; /* 协议类型 */
    uint8       code; /* 功能码 */
    uint8       dataLen; /* 数据长度 */
    uint32      di; /* 645DI */
    FillFunc    fillFunc; /* 提取字段函数 */
    SaveFunc    saveFunc; /* 存数据中心函数 */
}SENSOR_RECV_ATTR_T;



uint32 FillHAndTResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu);
uint32 FillWaterResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu);
uint32 FillSmokeResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu);
uint32 Fill645TResult(uint8 pipe, SENSOR_DEV_INFO_T *dev, SENSOR_RECV_PDU_U *pdu);

uint32 SaveHAndTDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle);
uint32 SaveWaterDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle);
uint32 SaveSmokeDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle);
uint32 Save645TDB(uint8 pipe, SENSOR_DEV_INFO_T *dev, DB_CLIENT DBClientHandle);

void *AmrThread_LoRa(void *arg);
uint32 SensorAmr(AmrThreadArg_T *arg, RS485_RS232_PARA *commPara);
void taskmng_init_file_load_area();
void event_temperature_seq(uint8 pipe,uint8 *buf,uint16 len);
void temperature_event_save(uint8 pipe,uint8 *buf,uint16 len,uint16 pn);
void event_protect_seq(uint8 pipe,uint8 *buf,uint16 len);
void event_protect_save(uint8 pipe,uint8 *buf,uint16 len,uint16 pn);
void event_switchchange_seq(uint8 pipe,uint8 *buf,uint16 len);
void event_switchchange_save(uint8 pipe,uint8 *buf,uint16 len,uint16 pn);
void taskmng_sensor_param_load();
int taskmng_sensor_param_get(OOP_OAD_U oad,uint8 *port,uint8 *addr,uint8 addrlen);
void taskmng_HPLC_module_curve_flag_load();
uint8 taskmng_HPLC_module_curve_flag_get();
void taskmng_module_curve_store_data_load();
void taskmng_module_curve_store_data_get(OOP_MODULE_CURVE_STORE_DATA_T *data);
void taskmng_curve_sync_check(uint8 pipe);
void taskmng_curve_sync_flag_set(uint8 pipe,uint8 flag);
uint8 taskmng_curve_sync_flag_get(uint8 pipe);
int taskmng_sta_curve_state_get(uint8 pipe);
uint16 taskmng_meter_frame_make_sta_curve(uint8 Pipe,uint8 seqindex,time_t StartTime,FRAME_SEQ_INFO_T FrameSeqInfo,uint8 *DataBuff,uint16 taskid,uint8 logicid);
BOOL taskmng_amr_report_data_unpack_0071(uint8 *PayloadBuf,uint32 Len,AMR_PLC_INFO_REPORT_T  *AmrReportInfo);
void taskmng_645_meter_data_save_stacurve(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask);
uint16 taskmng_sta_curve_get_msg_pack(uint8 pipe,uint8 *payload,uint8 *addr,uint8 *buf,uint16 buflen);
int taskmng_event_clock_offset_deal(uint8 pipe,uint8 *buf,uint16 buflen);

#endif


#if (defined AREA_JIANGXI) || (defined AREA_ANHUI) || (defined AREA_JIANGSU)
//extern OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;
extern void Save698TerminalEventData(uint8 pipe,uint8* DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U MainOad,time_t StartTime);
extern uint16 OopPatchTeminalAmrDataSave_Record(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink);
extern uint16 taskmng_oop_meter_data_tasktable_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask,DateTimeBcd_t *StoreTime);
extern int taskmng_oop_perception_term_get_request(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff);
extern int OopTermianlPatchGetRequst(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff,SCHEME_TASK_RUN_INFO *pTmp_NormalLink);
extern int MakeTerminalPatchRsdFrame(uint8 Pipe,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime);
void taskmng_perception_tmn_info_clear();

#endif

#ifdef AREA_JIANGXI
#define TMN_POWER_NORMAL_JX             0
#define TMN_POWER_ON_JX                 1
#define TMN_POWER_OFF_JX                2
#define TMN_POWER_CHANGE_JX             3

typedef struct tagMtrPowerEventMsg
{
    uint16  len;
    uint8   buf[256];
}MTR_POWER_EVENT_MSG_JX;

typedef struct tagMtrPowerUnit
{
    uint8                   flag;
    time_t                  happenTime;
    uint8                   happenCnt;
    MTR_POWER_EVENT_MSG_JX  eventMsg[6];
}MTR_POWER_UNIT_JX;

typedef struct tagMtrPowerOnOff
{
    uint8               tmnPowerOnOffFlag;
    MTR_POWER_UNIT_JX   tmnPowerOn;
    MTR_POWER_UNIT_JX   tmnPowerOff;
    OOP_DATETIME_S_T    tmStart;            //终端停上电事件开始时间
    OOP_DATETIME_S_T    tmEnd;              //终端停上电事件结束时间
}MTR_POWER_ON_OFF_JX;

extern void taskmng_event_power_out_limit(uint8 pipe);
extern void taskmng_event_zero_wire_current(uint8 pipe, uint16 mtrIndex);
extern void taskmng_power_on_off_deal_tmn(AmrThreadArg_T* pAmrThreadArg, uint8* buf, uint16 len, MTR_POWER_ON_OFF_JX* pMtrPowerOnOff);
extern void taskmng_power_on_off_deal_met_jx(uint8 pipe,uint8* buf,uint16 len, MTR_POWER_ON_OFF_JX* pMtrPowerOnOff);
extern void PowerOnOffEventReport_Jx(uint8 pipe, MTR_POWER_ON_OFF_JX* pMtrPowerOnOff);

#endif

#ifdef AREA_SHANDONG
#define USERTYPE_BREAKER_SD    210     //智能量测开关的用户类型
#ifdef ROUND_FAIL_CNT_LIMIT
#undef ROUND_FAIL_CNT_LIMIT
#define ROUND_FAIL_CNT_LIMIT    9   /* 连续9个任务周期不通的表，重试次数改为1次 */
#endif


extern int GetRunStateEvent(uint8 pipe, uint8* buf, uint16 len);
void Change5min1minToReal(SCHEME_TASK_RUN_INFO *info, OOP_TASK_CFG *cfg);
uint32 TaskMgrGetMeterTime(uint8 pipe, uint8 *addr, uint8 prtl, time_t *linuxTime);
uint32 JudgeMeterTimeIsTooSlow(uint8 pipe, time_t meterTime, time_t terminalTime, uint32 taskPeriod, time_t *timeOffset);
uint32 TaskMgrGetTimeOffset(uint8 pipe, uint32 taskPeriod, MtrAmrInfo_T *info);
BOOL taskmng_energy_oad_check(OOP_OAD_U oad) ;
BOOL taskmng_taskinfo_4800_check(TASK_INFO *TaskInfo);
uint8 taskmng_meter_NILM_module_check(uint8 pipe,uint8 *buf,uint16 buflen,uint8* NILMflag,uint8* logic5flag);
BOOL taskmng_pv_control_unpack(uint8 pipe,uint8* buf,uint16 buflen);



#endif
extern int get_power_state(int *state);

#endif
extern void Save698SepicialEventData(uint8 pipe,uint8* DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U MainOad,time_t StartTime);
#ifdef AREA_FUJIAN
#include "fujian_task.h"
#include "fujian_common.h"

OOP_GATHER_E fujian_gather_mode_get(DB_CLIENT clientid);
void* thread_task_fujian ( void* arg );
void* thread_amr_fujian ( void* arg );


#endif