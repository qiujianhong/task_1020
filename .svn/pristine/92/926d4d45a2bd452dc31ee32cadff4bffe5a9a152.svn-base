/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：参数变更处理
* @date： 2019.12.03
*********************************************************************
*/
#ifndef __COMM_OOP_VAR_UPDATE_H__
#define __COMM_OOP_VAR_UPDATE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "sys_cfg.h"
#include "framecomm.h"

#define GET_NEW_METER_EN   1                                //从搜表结果中自动更新档案，慎用功能  
  
/** @brief 复位标记 */
extern uint32 resetFlag;

/** @brief 修改IP*/
typedef struct
{
    pthread_mutex_t        mutex;                           //锁 多线程访问
    BOOL                   isVaild;                         //IP是否有效
    uint8                  feNum;                           //网口设备号
	uint8				   ip[4];
	uint8				   mask[4];
	uint8				   ipGateway[4];
}IPADDR_SET_T;
extern IPADDR_SET_T ipaddrSet;

/** @brief 档案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;                           //锁 多线程访问
    uint16                 oopMeterNum;                     //电表总个数
    uint16                 meterID[NO_VIR_MTR_MAX_NUM+1];   //档案内部索引
    BOOL                   isUpdate[NO_VIR_MTR_MAX_NUM + 1]; //档案是否正在更新
    OOP_METER_T            oopMeter[NO_VIR_MTR_MAX_NUM + 1];//每个电表的档案
}OOP_METER_P_LIST_T;

extern OOP_METER_P_LIST_T oopMeterList;

/** @brief 任务数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;          //锁 多线程访问
    uint16                 oopTaskNum;     //任务个数
    BOOLEAN                   isChanged[256]; //变更标记
    BOOLEAN                   isVaild[256];   //任务是否有效
    OOP_TASK_T             oopTask[256];   //任务参数
}OOP_TASK_LIST_T;

extern OOP_TASK_LIST_T oopTaskList;

/** @brief 上报方案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint16                 oopPlanReportNum;   //方案个数
    BOOLEAN                   isChanged[256];          //变更标记
    BOOLEAN                   isVaild[256];       //方案是否有效
    OOP_PLAN_REPORT_T      oopPlanReport[256]; //方案参数
}OOP_PLAN_REPORT_LIST_T;

extern OOP_PLAN_REPORT_LIST_T oopPlanReportList;

/** @brief 普通采集方案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint16                 oopPlanNormalNum;   //方案个数
    BOOLEAN                   isChanged;          //变更标记
    BOOLEAN                   isVaild[256];       //方案是否有效
    OOP_PLAN_NORMAL_T      oopPlanNormal[256]; //方案参数
}OOP_PLAN_NORMAL_LIST_T;

extern OOP_PLAN_NORMAL_LIST_T oopPlanNormalList;

/** @brief 事件采集方案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint16                 oopPlanEventNum;   //方案个数
    BOOLEAN                   isChanged;          //变更标记
    BOOLEAN                   isVaild[256];       //方案是否有效
    OOP_PLAN_EVENT_T       oopPlanEvent[256]; //方案参数
}OOP_PLAN_EVENT_LIST_T;

extern OOP_PLAN_EVENT_LIST_T oopPlanEventList;

/** @brief 控制中的保电状态*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint8                  guardState;         //保电状态
}OOP_GUARD_T;

extern OOP_GUARD_T oopGuard;

/** @brief 控制中的遥控状态*/
typedef struct
{
    pthread_mutex_t        mutex;               //锁 多线程访问
    uint8                  state[OOP_MAX_TRIP]; //遥控状态
    uint8                  type[OOP_MAX_TRIP];  //遥控类型
}OOP_REMOTE_CTRL_T;

extern OOP_REMOTE_CTRL_T oopRemoteCtrl;

/** @brief 搜表结果据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;                             //锁 多线程访问
    uint16                 seekMeterNum;                      //搜表结果个数
    BOOLEAN                   isChanged[NO_VIR_MTR_MAX_NUM+1];   //变更标记
    BOOLEAN                   isVaild[NO_VIR_MTR_MAX_NUM+1];     //搜表结果是否有效
    OOP_SEARCHMET_RES_T    seekMeter[NO_VIR_MTR_MAX_NUM+1];   //搜表结果
    BOOLEAN                 isReport[NO_VIR_MTR_MAX_NUM+1]; //标记搜表结果是否已上报
}OOP_SEEK_METER_LIST_T;

extern OOP_SEEK_METER_LIST_T seekMeterList;

/** @brief 转加密结果据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;            //锁 多线程访问
    uint16                 encryptionNum;    //搜表结果个数
    BOOLEAN                isChanged[256];   //变更标记
    BOOLEAN                isVaild[256];     //搜表结果是否有效
    OOP_ENCRYPTRES_T       encryption[256];  //搜表结果
    BOOLEAN                isReport[256];    //标记搜表结果是否已上报
}OOP_ENCRYPTION_RES_LIST_T;

extern OOP_ENCRYPTION_RES_LIST_T encryptionList;

/** @brief 跨台区搜表结果据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;                             //锁 多线程访问
    uint16                 seekAcrossNum;                     //搜表结果个数
    BOOLEAN                   isChanged[NO_VIR_MTR_MAX_NUM+1];   //变更标记
    BOOLEAN                   isVaild[NO_VIR_MTR_MAX_NUM+1];     //搜表结果是否有效
    OOP_ACROSSAREA_RES_T   seekAcross[NO_VIR_MTR_MAX_NUM+1];  //搜表结果
}OOP_SEEK_ACROSS_LIST_T;

extern OOP_SEEK_ACROSS_LIST_T seekAcrossList;

#ifdef AREA_ZHEJIANG
#define    CBIR_MAX_NUM      5000                                   //拓扑关系识别信息结果的最大数量
/** @brief 搜表结果据缓存结构*/
typedef struct
{
    pthread_mutex_t                    mutex;                       //锁 多线程访问
    uint16                             cbirResultNum;               //拓扑关系识别信息结果个数
    BOOLEAN                            isChanged[CBIR_MAX_NUM+1];   //变更标记
    BOOLEAN                            isVaild[CBIR_MAX_NUM+1];     //结果是否有效
    Circuit_Breaker_Identify_Result    cbirResult[CBIR_MAX_NUM+1];  //拓扑关系识别信息结果
    BOOLEAN                            isReport[CBIR_MAX_NUM+1];    //标记结果是否已上报
}CBIR_RESULT_LIST_T;
extern CBIR_RESULT_LIST_T cbirResultList;
#endif

/** @brief 透明采集方案数据缓存结构*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint16                 oopPlanTransparentNum;   //方案个数
    BOOLEAN                   isChanged;          //变更标记
    BOOLEAN                   isVaild[256];       //方案是否有效
    OOP_PLAN_TRANS_T       oopPlanTransparent[256]; //方案参数
}OOP_PLAN_Transparent_LIST_T;

typedef struct
{
    uint16 pn;
    uint8  add[6];
}OOP_infomun_T;


/** @brief 透明采集方案数据缓存结构*/
typedef struct
{
    uint8 meternum;
    OOP_infomun_T  infomun[OOP_MAX_TRANS_INFO];
    OOP_PLAN_TRANS_T PLAN_TRANS_T;
}OOP_Transparent_InfoNum_LIST_T;

extern OOP_PLAN_Transparent_LIST_T oopPlanTransparentList;

/** @brief 主站对终端精准校时结构*/
typedef struct
{
    pthread_mutex_t         mutex;                            //锁 多线程访问
    uint8                   timingModeValid;                    //校时模式是否有效
    uint8                   timingMode;                       //校时模式
    uint8                   syncClockValid;                 //心跳校时参数是否有效
    OOP_SYNCCLOCK_T         syncClock;                        //心跳校时参数
}OOP_SYNCCLOCK_TIMING_T;

extern OOP_SYNCCLOCK_TIMING_T syncClockParam;

/** @brief 主站对终端精准校时结构*/
typedef struct
{
    pthread_mutex_t         mutex;                          //锁 多线程访问
    uint8 isValid;
    OOP_SYNCCYCLE_T     synccycle;                          //时间同步周期
}OOP_SYNC_CYCLE_T; 


/** @brief 更新esam密钥下发状态*/
typedef struct
{
    pthread_mutex_t         mutex;                            //锁 多线程访问
    uint8                   isEsamUpdate;                     //是否更新密钥状态
    uint8                   isEsamSet;                        //密钥状态 0 - 未下发 1 - 已下发
}OOP_ESAM_STATE_T;

extern OOP_ESAM_STATE_T esamState;

typedef enum
{
    TMN_NOACT = 0,     //无动作
    TMN_REBOOT = 1,    //终端复位
    TMN_DATA_INIT = 2, //数据初始化
    TMN_PARA_INIT = 3, //参数初始化
    TMN_REBOOTING = 4, //正在复位中，如果复位过长或者失败后的处理
    TMN_RBT_DELAY_20S = 5, //延时20秒复位
}TMN_CTRL_E;

typedef enum
{
    SIMOS_NOACT = 0,     //无动作
    SIMOS_REBOOT = 1,    //终端复位
}SIMOS_CTRL_E;

typedef struct
{
    pthread_mutex_t     mutex;
    OOP_ETHIP_T         ethip;
}OOP_ETHIP_INFO_T;

typedef struct
{
    pthread_mutex_t     mutex;
    uint8               isCallIn;
    uint32              appNum;
    OOP_REG_APP_INFO_T  appInfo[APP_NUM_MAX];
}OOP_REG_APP_INFO_LIST_T;

extern OOP_REG_APP_INFO_LIST_T oopRegAppInfoList;

/** @brief 搜表参数结构*/
typedef struct
{
    pthread_mutex_t         mutex;                            //锁 多线程访问
    uint8                   searchParamValid;                 //搜表参数是否有效
    OOP_SEARCHPARAM_T       searchParam;                      //搜表参数
}OOP_SEARCHPARAM_G_T;

extern OOP_SEARCHPARAM_G_T oopSearchParam;

/** @brief 载波字节版本信息*/
typedef struct
{
    pthread_mutex_t        mutex;                              //锁 多线程访问
    uint16                 nodeVersionNum;                     //子节点信息数量
    BOOLEAN                isChanged[NO_VIR_MTR_MAX_NUM + 1];  //变更标记
    BOOLEAN                isVaild[NO_VIR_MTR_MAX_NUM + 1];    //搜表结果是否有效
    OOP_NODEVERSIONINFO_T  nodeVersion[NO_VIR_MTR_MAX_NUM + 1];//搜表结果
}OOP_NODEVERSIONINFO_LIST_T;

extern OOP_NODEVERSIONINFO_LIST_T nodeVersionList;

/** @brief 子节点信息*/
typedef struct
{
    pthread_mutex_t        mutex;                              //锁 多线程访问
    uint16                 childNodeNum;                     //子节点信息数量
    BOOLEAN                isChanged[NO_VIR_MTR_MAX_NUM + 1];  //变更标记
    BOOLEAN                isVaild[NO_VIR_MTR_MAX_NUM + 1];    //搜表结果是否有效
    OOP_CHILDNODE_T        childNode[NO_VIR_MTR_MAX_NUM + 1];//搜表结果
}OOP_CHILDNODE_LIST_T;

extern OOP_CHILDNODE_LIST_T childNodeList;

#ifdef AREA_FUJIAN
/** @brief 群组档案信息*/
#define         GROUP_NUM              256
#define         GROUP_METER_NUM        OOP_MAX_MSPARAM

typedef struct
{
    OOP_TSA_T groupAddr;                      //群组地址
    uint32    num;                            //关联档案数量
    OOP_TSA_T addr[GROUP_METER_NUM];          //关联档案
}OOP_GROUPMETER_T;

typedef struct
{
    pthread_mutex_t        mutex;             //锁 多线程访问
    uint32                 num;               //群组档案数量
    BOOLEAN                isVaild[GROUP_NUM];//有效标识
    uint16                 inform[GROUP_NUM]; //每个群组档案的信息点
    OOP_GROUPMETER_T       group[GROUP_NUM];  //群组档案
}OOP_GROUPMETER_LIST_T;

extern OOP_GROUPMETER_LIST_T groupMeterList;

typedef struct
{
    pthread_mutex_t        mutex;             //锁 多线程访问
    BOOLEAN                isVaild;           //有效标识
    OOP_FTPSERVER_T        serverInfo;        //升级服务器
    OOPM_FTP_T             ftp;               //升级信息
}OOPM_FTP_INFO_T;

extern OOPM_FTP_INFO_T ftpInfo;

#endif

/**********************************************************************
* @name      : app_get_meter_list
* @brief     ：获取档案数据
* @param[in] ：i        - 档案序号
* @param[out]：oopMeter - 档案
* @return    : 错误码
* @Create    : 
* @Date      ：2019-12-16
* @Update    :
**********************************************************************/
int app_get_meter_list(uint32 i, OOP_METER_T *oopMeter);
int app_get_meter_list_by_id(uint32 meterID, OOP_METER_T *oopMeter);
int app_get_meter_list_by_tsa(OOP_TSA_T		tsa, OOP_METER_T *oopMeter);
int app_get_seek_meter_list(uint16 infoNum, OOP_SEARCHMET_RES_T *seekMeter);
int app_get_encryption_res_list(uint16 infoNum, OOP_ENCRYPTRES_T *encryption);
BOOLEAN app_check_seek_meter_isreport(uint16 infoNum);
int app_get_seek_across_list(uint16 infoNum, OOP_ACROSSAREA_RES_T *seekAcross);
int app_get_nodeversion_list(uint16 infoNum, OOP_NODEVERSIONINFO_T *nodeVersion);
int app_get_childnode_list(uint16 infoNum, OOP_CHILDNODE_T *childNode);
int appdb_get_childnode_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int app_get_task_list(uint8 taskID, OOP_TASK_T *oopTask);
int app_get_plan_normal_list(uint8 planID, OOP_PLAN_NORMAL_T *oopPlanNormal);
int app_get_plan_report_list(uint8 planID, OOP_PLAN_REPORT_T *oopPlanReport);
int app_get_plan_event_list(uint8 planID, OOP_PLAN_EVENT_T *oopPlanEvent);
int app_get_plan_transparent_list(uint8 planID, OOP_PLAN_TRANS_T *oopPlanTransparent);
void app_update_version_set(char *str);
int app_update_version_get(char *ver);
int appdb_get_task_list(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
void remain_energy_lock(void);
void remain_energy_unlock(void);
int remain_energy_write(int clientid, uint16 vir_no, int64 energy);
int remain_energy_read(int clientid, uint16 vir_no, int64* energy);
void appdb_var_update_init(int clientid);
void set_reset_flag(int dbClient, uint32 flag);
uint32 get_reset_flag(void);
uint8 get_guard_state(void);
int set_guard_state(uint8 guardState);
uint8 get_remote_state(uint8 turn, uint8 *type);
void set_remote_state_invalid(uint8 turn);
void set_remote_state_valid(uint8 turn, uint8 type);
void appdb_plan_transparent_list_record_init(int clientid);
void app_set_timingmode(uint8 timingmode);
void app_set_syncparam(OOP_SYNCCLOCK_T *syncclock,uint16  len);
int app_get_timingmode(void);
int app_get_syncparam(OOP_SYNCCLOCK_T *syncclock);
int app_timingmode_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
int app_syncclock_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
void sync_clock_param_lock_init(void);
void set_esam_state(uint8 isEsamSet);
uint8 get_esam_state(void);
void set_esam_update_state(uint8 isEsamUpdate);
uint8 get_esam_update_state(void);
void ethip_info_init(void);
int ethip_info_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
void ethip_info_set(OOP_ETHIP_T *ethip);
OOP_ETHIP_T ethip_info_get();
bool ethip_compare(OOP_ETHIP_T *newethip);
uint8 regapp_callin_stat_get(void);
void regapp_callin_stat_set(uint8 isCallin);
uint32 app_get_meter_port_list_by_tsa(OOP_TSA_T         tsa);
bool  app_get_meter_basic_info_by_tsa(OOP_TSA_T     tsa,OOP_METER_BASIC_T* basic);
int app_get_search_param(OOP_SEARCHPARAM_T *searchParm);
bool app_get_meter_tsa_list_by_pn(uint16        pn,OOP_TSA_T *tsa);
int app_set_meter_update_start(uint32 infoNum);
int is_meter_update_ok(uint32 infoNum);
int  GetVirtualPipePn(uint16 pn,OOP_METER_T *oopMeter);
void load_safemod_init(void);
uint32 ac_relay_flag_get(void);
void sync_cycle_param_lock_init(void);
void app_set_synccycle_param(OOP_SYNCCYCLE_T *synccycle,uint16  len);
int app_get_synccycle_param(OOP_SYNCCYCLE_T *synccycle);
int app_time_sync_cycle_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
void app_set_seek_meter_isreport(uint16 infoNum);

#ifdef AREA_ZHEJIANG
extern int app_get_cbir_result_list(uint16 infoNum, Circuit_Breaker_Identify_Result *cbirResult);
#endif

#ifdef AREA_FUJIAN
extern int app_get_groupmeter_list(uint16 index, OOP_GROUPMETER_T *groupMeter);
extern int app_server_info_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int app_ftp_info_read(int clientid, uint8 logicId, uint16 infoNum, uint32 value);
extern int app_get_server_info(OOP_FTPSERVER_T *serverInfo);
extern int app_get_ftp_info(OOPM_FTP_T *ftp);
extern BOOLEAN app_get_ftp_status(void);
extern void app_clr_ftp_status(void);

#endif

#ifdef __cplusplus
}
#endif

#endif
