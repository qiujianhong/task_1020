
#ifndef _OOP_MAP_H_
#define _OOP_MAP_H_

#include "CtsAll.h"

#define RT_DATA_INDEX  1
#define EVENT_SRC_DATA_INDEX  2
#define VIR_SRC_DATA_INDEX  3
#define DI_SINGL                            0                       //������ʶ
#define DI_BLOCK                            1                       //�鳭��ʶ
#define OOP_DAY_ADDREAD_TASKNUM 20
#define OOP_CRV_ADDREAD_TASKNUM 20
#ifdef AREA_JIANGSU
#define OOP_CRV_ADDREAD_MOST_DAYS_485 7
#else
#define OOP_CRV_ADDREAD_MOST_DAYS_485 1
#endif
#define OOP_CRV_ADDREAD_MOST_DAYS_PLC 1       //Ĭ��PLC����������ಹ����������������ʼʱ����1��������
#define OOP_DAY_ADDREAD_MOST_DAYS_485 3
#define OOP_DAY_ADDREAD_MOST_DAYS_PLC 3


#if 0
/** @brief ���ϵ����豸�������ݼ�  */
typedef struct tag_OOP_DEVFREZDATA
{
    uint16 devClsID;                /**< �豸��� */
    uint32 nEnergy;                 /**< ����       */
    int    nPower;                  /**< ����       */   
    uint8  startTime;               /**< ��ʱ��       */
    uint8  stopTime;                /**< ֹͣʱ��       */
    uint8  nCount;                  /**< �ϲ���ͣ��       */
}OOP_DEVFREZDATA_T;

typedef struct tag_OOP_DEVFREZDATAS
{
    uint32 nNum;
    uint32 rsv;
    OOP_DEVFREZDATA_T data[OOP_MAX_DEV];
}OOP_DEVFREZDATAS_T;


/** @brief �����豸�������ݼ�    */
typedef struct tag_OOP_DEVFREZDATA1
{
    uint16 devClsID;                /**< �豸��� */
    uint32 nEnergy;                 /**< ����       */
    int32  power;                 /**< ƽ������       */
    uint32 starttime;                 /**< ��ʱ��       */
    uint32 stoptime;                 /**< ֹͣʱ��       */
    uint32 startstop;                 /**< �ϲ���ͣ��       */
    
}OOP_DEVFREZDATA1_T;

typedef struct tag_OOP_DEVFREZDATAS1
{
    uint32 nNum;
    uint32 rsv;
    OOP_DEVFREZDATA1_T data[OOP_MAX_DEV];
}OOP_DEVFREZDATAS1_T;
#endif



/** @brief �����豸�������ݼ�    */
typedef struct tag_OOP_DEVSAMPDATA
{
    uint8  phaseinfo;                /**< ��λ��Ϣ */
    int32  vlotageSamp;             /**< ��ѹ����       */
    int32  currentSamp;             /**< ��������       */
}OOP_DEVSAMPDATA_T;

typedef struct tag_OOP_DEVSAMPDATAS
{
    uint32 nNum;
    uint32 rsv;
    OOP_DEVSAMPDATA_T data[OOP_MAX_DEV];
}OOP_DEVSAMPDATAS_T;

typedef struct 
{
  //uint8 type;     //ID���
  uint16  bDataId;       //Դ�ֵ�ID���ֵ��ʽ��
  uint16  OadId;      //OADID
  uint8   OadTag;      //OAD������
  //uint8 BlockOrSingle;  //CB_BLOCK  CB_SINGL
  uint8 OadNumOrDataLen;      //�鳭��ʱ��������ĸ��� ����鳭�ĵ�һ�γ�ʧ�ܾ�����+1 
  int8 descaler;   //�����ֵ��ʽ��С����λ��
  uint16  desDataType;    //Ŀ����������
}MetOopPrtl_t;

typedef struct{
    uint16 OadId;
    MetOopPrtl_t *pMetOopMap;
}ProfileOadMap_t;

typedef struct{
    OOP_OAD_U  OadId;      //OADID
    uint8   Group;
    int8   DesScaler;
    uint8  DesDataType;
    uint8   OadTag;      //OAD������
    uint32  DI07;        //07Э�����ݱ�ʶ
    uint16  DI97;        //97Э�����ݱ�ʶ
    uint8   DataLen;    //645 ���ݳ���
    int8   Scaler;     //645С����λ��
    uint16  DataType;   //645��������
}MapOopTo645_t;


// �ṹ�������    
typedef enum
{   
    E_OOP_ENERGY_T=0,  
    E_OOP_ENERGYS_T,
    E_OOP_HENERGY_T,
    E_OOP_HENERGYS_T,    
    E_OOP_DEMAND_T,
    E_OOP_DEMANDS_T,
    E_OOP_VOLTQR_T,
    E_OOP_WORD3V_T,
    E_OOP_DWORD3V_T,
    E_OOP_WORD4V_T,
    E_OOP_LONG4V_T,
    E_OOP_INT3V_T,
    E_OOP_INT4V_T,
    E_OOP_METWORD_T,
    E_DATETIME_S,
    E_FRZ_DATETIME_S,
    E_REAL_DATETIME_S,
    E_OOP_WALLET_T,
    E_DOUBLE_LONG_UNSIGNED,
    E_DOUBLE_LONG,
    E_LONG_UNSIGNED,
    E_LONG,
    E_OOP_FROZENSETS_T,
    E_OOP_HARMONIC_T,
    E_ENUM,
    E_OOP_SYNCCLOCK_T,
    E_OOP_OCTETVAR16_T,
    E_OOP_GEOGADD_T,
    E_OOP_GRPADDR_T,
    E_OOP_PERIOD_T,
    E_UNSIGNED,
    E_OOP_PUBHOLIDAY_T,
    E_OOP_VISIBLESTR8_T,
    E_OOP_TIMEZONE_T,
    E_OOP_DAYPERIOD_T,
    E_OOP_DAYPERIOD_E_T,
    E_OOP_RATETARIFF_T,
    E_OOP_STEPPRICE_T,
    E_OOP_LIMITCHARGE_T,
    E_OOP_OTHERLIMIT_T,
    E_OOP_POWERLIMIT_T,
    E_OOP_OTHERPOWERLIMIT_T,
    E_OOP_VISIBLESTR16_T,
    E_OOP_VISIBLEVAR8S_T,
    E_OOP_VISIBLEVAR32_T,
    E_OOP_BITSTR8_T,
    E_DAYPERIOD,
    E_TIMEZONE,
    E_OOP_PAYDAY_T,
    E_BIT_STRING,
    E_OOP_EVTSTAT_T,    //�¼���ǰֵ��¼��
    E_OOP_EVTSTAT1_T,   //�����¼���ǰֵ��¼��
    E_OOP_DWORD2_T,
    E_OOP_LONG3V_T,
    E_OOP_VOLTSTAT_T,
    E_OOP_PILE_WORK_STATUS_T,
    E_OOP_CHARGE_SERVICE_T,
    E_OOP_BITSTR32_T,
    E_OOP_HARMONIC_U_VALUE_T,               //��ѹг������
    E_OOP_HARMONIC_I_VALUE_T,               //����г������
    E_OOP_OCTETVAR64_T,

    #if (defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
    E_OOP_LINE_LOSS_T,                      //����
    E_OOP_SWITCH_IMPEDANCE_T,               //��·�迹
    #endif
    E_OOP_PROTOCOL_T,
    E_OOP_ASSOCIATION_CONTEXT_T,
    E_OOP_VOLTPARAM_T,
    E_OOP_PROTECTOR_31BB_T,
    E_OOP_LCD_T,

    E_OOP_METERMOD_LIST,
    E_OOP_MODVERSION_LIST,
    E_OOP_SOFTREC_LIST,
    E_OOP_DEVFREZDATAS_T,
    E_OOP_DEVINSTANTDATAS_T,
    E_OOP_DEVSAMPDATAS_T,
#ifdef AREA_HUNAN
    E_BIN,
    E_SWITCH_STATUS,
    E_DATA,
#endif
#ifdef AREA_SHANDONG
    E_OOP_HUMITURE_2E20_T,
    E_OOP_LOCK_STATE_2E21_T,
#endif

    E_OOP_HARMONIC_VOLTAGE_T,
    E_OOP_HARMONIC_CUR_POWER_T,
    E_OOP_HARMONIC_ACTIVE_POWER_T,

    E_TSA,
    E_OOP_Power_Characteristic_Record,
    E_OOP_Energy_Characteristic_Record,
    E_OOP_APPURTENANT_METER_LIST_T,
    E_OOP_REPORTS_T,
    E_OOP_EVTSOURCE_T,

    E_OOP_BREAK_RUN_STATUS_T,

    E_ABNORMAL_METER_INFO_T,
	E_ESAM_COUNT,   //esam������
    E_SECURITY_FLAG,
	E_OOP_EVENTTIME_T,       //�¼�ʱ��״̬ ����10
	E_OOP_EVENTTIME1_T,     //�����¼�ʱ��״̬��¼��
	E_Water_Temperature_T,
	E_OOP_ST_T,
	E_OOP_CHANNEL_T,
	E_OOP_IR_T,
	E_OOP_RS485_T,
	E_OOP_SWITCH_OUT_T,
	E_OOP_ROUTEMODULE_T,
	E_OOP_TELECON_PARAM_T,
	E_OOP_BROADCASTCLOCK_T,
	E_OOP_LOAD_I0_EVECFG_T,
	E_OOP_OBJ_SHOW_T,
	E_OOP_OBJ_SHOW_PARAM_T,
	E_OOP_TERMINAL_LIMIT_T,
	E_OOP_OADS_T,
	E_OOP_EVTCFG_3000_T,
	E_OOP_EVTCFG_WD_T,
	E_OOP_EVTCFG_ID_T,
	E_OOP_EVTCFG_3006_T,
	E_OOP_EVTCFG_DD_T,
	E_OOP_EVENT_T,
	E_OOP_CFG_LU_T,
	E_OOP_PAYDAYS_T,
	E_OOP_EVTCFG_3004_T,
	E_OOP_EVTCFG_DELAY_T,
	E_OOP_EVTCFG_3003_T,
	E_OOP_EVTCFG_ID,
    E_NULL=0xFFFF,

}DATASTRUCT;  


// �ṹ�������    
typedef enum
{   
    BCD4TOINT32=0,        

}CovertFormat;  

typedef struct 
{
    OOP_OAD_U  mainOad;              //�¼�OAD
    OOP_OAD_U  EventNoOad;           //698�¼���¼���OAD
}EventRcordNo;

typedef struct 
{   
    time_t AddReadTime;         //����ʱ��
    uint16 TaskId;              //����ID
    uint8  SchemeId;            //�������
    uint8  addr[6];             //����ַ����ΪУ��ʹ��
    uint8  patchresult[84];     //������־
    uint8  tmtv;                //����������ӣ�����һ��ֻ��15���Ӻ�60����
}OopCrvAddReadTime_t;

typedef struct{
    OOP_OAD_U  Oad;      //Ҫת����OAD
    int8   DesScaler;        //0AD��Ӧ�Ļ��㵥λ
    uint32  DI07;        //07Э�����ݱ�ʶ
    uint8   DataLen;    //645 ÿ����Ԫ�صĳ���
    uint8   TotalDataLen;    //645 ÿ����Ԫ�صĳ���
    uint16  DI97;
    DATASTRUCT  CovertSruct;   //Ҫת�ɵĽṹ������
    int8   Scaler;     //645С����λ��
}MapOopTo645_NEW;


typedef struct{
    OOP_OAD_U  Oad;      //Ҫת����OAD
    int8   DesScaler;        //0AD��Ӧ�Ļ��㵥λ
    uint8   DataLen;    //645 ÿ����Ԫ�صĳ���
    uint8   TotalDataLen;    //645 ÿ����Ԫ�صĳ���
    DATASTRUCT  CovertSruct;   //Ҫת�ɵĽṹ������
}MapOopToCap;



typedef struct{
    OOP_OAD_U  Oad;      //OADID
    uint32  DI07;        //07Э�����ݱ�ʶ
    uint8   DIAtr;
    const MapOopTo645_NEW *EventMap;
}EventMapOopTo645_t;


typedef struct{
    uint8 sourcebuffer[255];
    uint16 reclen;
    uint8  pwrType;//���ܱ���߷�ʽ��
}DataSource;

typedef struct{
    OOP_OAD_U  Oad;      //Ҫת����OAD    
    uint8 class;
    OOP_DATATYPE_E dataType;  //��������
    uint8 dataLen;       //oad��Ӧ�ĳ��� ����������
    uint8 subLen;           //�����Ӧ�ĳ���
    DATASTRUCT  CovertSruct;   //Ҫת�ɵĽṹ������
}OadInfoMap;

typedef struct
{
    uint8 WorkStatBit;      //״̬�ֶ�Ӧ��λ��Ϣ
    OOP_OAD_U EventOad;     //�¼�oad
    uint32 DI_EventCount;   //�¼��������� 645DI
    uint8 DINum;            //�¼���Ҫ���������������
    uint32 DI_Frid[10];     //�¼���Ҫ������������ ������������Ӧ ��һ�μ�¼ ����¼��ж������������ϼ���
}EventID_To_DI ;

typedef struct
{
    uint8 bitIndex;     //���ܱ�����ϱ�״̬�ֶ�Ӧbitλ
    OOP_OAD_U eventOad;     //��Ӧ���¼�oad
}EVENT_REPORT_STATE_TO_OAD_T;

/* ��������OAD�б� OIA1 = 0x00 */
extern const OadInfoMap EnergyList[];
/* ���������OAD�б� OIA1 = 0x01 */
extern const OadInfoMap DemandList[];

/* ������OAD�б� OIA1 = 0x02 */
extern const OadInfoMap VariateList[];

/* �¼���OAD�б� OIA1 = 0x03 */
extern const OadInfoMap EvnetList[];

/* �α�����OAD�б� OIA1 = 0x04 */
extern const OadInfoMap ParaList[];

/* ������OAD�б� OIA1 = 0x05 */
extern const OadInfoMap FreezeList[];

/* �ɼ���OAD�б� OIA1 = 0x06 */
extern const OadInfoMap CollectList[];

/* ������OAD�б� OIA1 = 0x07 */
extern const OadInfoMap GatherList[];

/* ���ƽӿ���OAD�б� OIA1 = 0x08 */
extern const OadInfoMap ControlList[];

/* ������ӿ���OAD�б� OIA1 = 0x09 */
extern const OadInfoMap OrderedchargeList[];

/* �ļ�����ӿ���OAD�б� OIA1 = 0x0F OIA2 = 0x00 */
extern const OadInfoMap FileList[];

/* ESAM�ӿ���OAD�б� OIA1 = 0x0F OIA2 = 0x01 */
extern const OadInfoMap EsamList[];

/* ���������OAD�б� OIA1 = 0x0F OIA2 = 0x02 */
extern const OadInfoMap IoList[];
//#if((1 == OOP_DAY_ADDREAD_485)||(1 == OOP_DAY_ADDREAD_PLC))
extern time_t GetDayNewStartTime(time_t OriStartTime, uint16 TaskId, uint8 SchemeId, uint16 MtrNo, uint8 PLC485Flag);
extern void UpdateDayNewStartTime(uint16 OI, DateTimeBcd_t *pStoreTm, uint16 TaskId, uint8 SchemeId, uint16 MtrNo, AcqInfo_t *pAcqInfo);

//#endif
extern const MapOopToCap MapOopToCap_RT_01[];

extern const MapOopTo645_NEW* Find645InfoPtrByOad(OOP_OAD_U mainOad, OOP_OAD_U subOad);
extern uint16 CaclBiasByOad(OOP_OAD_U mainOad,OOP_OAD_U subOad);
extern void Convert_6457_97_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopTo645_NEW *pOadInfo);

extern BOOL FindEvtRecNoIDByOad(OOP_OAD_U mainOad);
extern BOOL GetEventRecordNo(OOP_OAD_U Mainoad,OOP_OAD_U *eventnooad);
extern BOOL IsGroupByDI97(uint16 sDI97,uint16 dDI97);
extern BOOL IsGroupByDI07(uint32 sDI07,uint32 dDI07);
extern uint8 GetDI07_485(AcqInfo_t AcqInfo,CSD_TASK_INFO  *csd, uint8 subOadIndex, uint8 endNum, uint32 *DI07Buff,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe);
extern const EventMapOopTo645_t *taskmng_event_map_get(OOP_OAD_U mainOad);
extern const MapOopTo645_NEW* taskmng_map_get_from_mainoad(uint8 Pipe,uint16 MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink, uint16 *MapNum);
extern void Convert_Oop_Data_To_Struct(uint8 *soucerbuffer,uint16 *offset1,uint16 soucerlen,uint8 *desbuffer, OadInfoMap *pOadInfo,uint16 *deslength,uint8 pwrType);
extern void taskmng_meter_data_save(uint8   *inDataBuf,uint32 inDatalen ,OadStoreInfo StoreInfo,uint8 pipe );
extern void Convert_6457_07_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopTo645_NEW *pOadInfo);
extern BOOL taskmng_oad_info_get(OOP_OAD_U oad, OadInfoMap *OadMap);
extern BOOL  taskmng_oop_data_extract(const uint8 *inData,uint16 *offset,uint16 inDataLen,uint8 *tembuffer,enum tag_OOP_DATATYPE DataType,BOOL bWithType);
extern int CheckMapFromMainOAD(uint8 Pipe,uint16 MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,const MapOopTo645_NEW** pMap,uint16 *MapNum);
extern BOOL RecordMerge(uint8* inData,uint8* recordData,uint8* mergeData,uint16 *outLen,OadInfoMap *pOadInfo );
extern const EventID_To_DI *GetEventIDToDIByIndex(uint8 IDIndex);
extern BOOL Dlt645_ComBcdToBin(uint8 *sBcd,uint8 sLen,uint8 *dBin,uint8 dLen,BOOL withSign);
extern uint8 GetCapDI(CSD_TASK_INFO  *csd,  uint8 mapindex);
extern void Convert_Protector_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopTo645_NEW *pOadInfo);
extern void Convert_Dlms_Data_To_Struct(uint8 *soucerbuffer,uint16 *offset1,uint16 soucerlen,uint8 *desbuffer, OadInfoMap *pOadInfo,uint16 *deslength,uint8 pwrType,uint8 Csdtype,uint8 dlsnum);
uint8 taskmng_2015_event_oad_get(uint8 *reportBuf,OOP_OAD_U *oadlist);

#if((1 == OOP_CRV_ADDREAD_485)||(1 == OOP_CRV_ADDREAD_PLC)) 
extern time_t GetCrvNewStartTime(time_t OriStartTime, uint16 TaskId, uint8 SchemeId, uint16 MtrNo, uint8 PLC485Flag);
extern void UpdateCrvNewStartTime(uint16 OI, DateTimeBcd_t *pStoreTm, uint16 TaskId, uint8 SchemeId, uint16 MtrNo, AcqInfo_t *pAcqInfo);
#endif
extern BOOL taskmng_need_merge_check(OadInfoMap *pOadInfo );
extern BOOL Check_Data_Struct(DATASTRUCT     DataStruct);
extern void Convert_Minute_Data_To_Struct(uint8 *soucerbuffer,uint16 soucerlen,uint8 *desbuffer, OadInfoMap *pOadInfo,uint16 *deslength);
extern void taskmng_leak_data_save(uint8   *inDataBuf,uint32 inDatalen ,OadStoreInfo StoreInfo,uint8 pipe ,uint32 intv);

#endif
uint8 GetDI188(AcqInfo_t AcqInfo,CSD_TASK_INFO  *csd, uint8 subOadIndex, uint8 endNum, uint32 *DI07Buff,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe);
void STD188_MakeFrameEx(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo);
const MapOopTo645_NEW* Find188InfoPtrByOad(OOP_OAD_U mainOad, OOP_OAD_U subOad);
void Convert_std188_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopTo645_NEW *pOadInfo);

void taskmng_meter188_data_save(uint8   *inDataBuf,uint32 inDatalen ,OadStoreInfo StoreInfo,uint8 pipe );
extern time_t GetCrvStartTime(time_t OriStartTime, uint16 TaskId, uint16 MtrNo, uint8 PLC485Flag);
extern BOOL JudgeNeedPatch(uint16 TaskId, uint16 MtrNo,uint32 *starttime);
uint8 GetDI188New(AcqInfo_t AcqInfo,CSD_TASK_INFO  *csd, uint8 subOadIndex, uint8 endNum, uint32 *DI07Buff,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe);
const MapOopTo645_NEW* Find188InfoPtrByOadNew(OOP_OAD_U mainOad, OOP_OAD_U subOad);


