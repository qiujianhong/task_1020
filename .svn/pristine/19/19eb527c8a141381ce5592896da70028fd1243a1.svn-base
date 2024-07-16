/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/


#ifndef __PW_COMMON_H__
#define __PW_COMMON_H__

#include "pw_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************************************************************************
* 变量声明
*******************************************************************************/

//方案档案状态
typedef enum
{
    TASK_CFG_UNINIT,              //未初始化
    TASK_CFG_MODIFY,              //修改
    TASK_CFG_MODIFY_DELAY,        //修改延时
    TASK_CFG_INITED,              //已初始化
    TASK_CFG_REBUILD,             //重建采样文件
    TASK_CFG_PATCH,               //创建补抄文件
} TASK_CFG_STATE;

#define METER_MAX_NUM        	    2040				//最大测量点个数

//电表档案
typedef struct
{
    pthread_rwlock_t              rwLock;                     //读写锁 多线程访问
    TASK_CFG_STATE                FileState;                 //档案状态
    int                           ModifyDelaySec;            //方案配置修改延时
    time_t                        FunLastRunTime;            //方案上次修改时间
    uint16                        MeterCount;                //电表总个数
    OOP_METER_T                   MeterFile[METER_MAX_NUM];    //每个电表的档案
}METER_FILE_T;

//变量采集计时器
typedef struct
{
    uint16 nHardYxSecCounter;  //硬遥信 秒计时器
	uint16 nCommonYxSecCounter;  //普通遥信 秒计时器
	uint16 nEventYxSecCounter;  //事件遥信 秒计时器
	uint16 nCommonYcSecCounter;  //普通遥测 秒计时器
	uint16 nRecordYcSecCounter;  //记录遥测 秒计时器
	uint16 nRealYcSecCounter;  //实时遥测 秒计时器
	uint16 nEnergyYcSecCounter;  //电量 秒计时器
	uint16 nDemandYcSecCounter;  //需量 秒计时器
	uint16 nHarmonicYcSecCounter;  //谐波 秒计时器
	uint16 nCalculateYcSecCounter;  //统计遥测 秒计时器
}SECOND_COUNTER_T;

//变量采集状态
typedef struct
{
    uint8 nHardYxFlag;  //硬遥信 是否采集
	uint8 nCommonYxFlag;  //普通遥信 是否采集
	uint8 nEventYxFlag;  //事件遥信 是否采集
	uint8 nCommonYcFlag;  //普通遥测 是否采集
	uint8 nRecordYcFlag;  //记录遥测 是否采集
	uint8 nRealYcFlag;  //实时遥测 是否采集
	uint8 nEnergyYcFlag;  //电量 是否采集
	uint8 nDemandYcFlag;  //需量 是否采集
	uint8 nHarmonicYcFlag;  //谐波 是否采集
	uint8 nCalculateYcFlag;  //统计遥测 是否采集
	uint8 nPreDayYcFlag;  //上日统计数据 是否采集
	uint8 nPreMonYcFlag;  //上月统计数据 是否采集
}READ_STATE_T;

//调节日志路径
extern char *g_pRecordPathMain;//主线程调节日志
extern char *g_pRecordPathUpRecv;//上行接收调节日志
extern char *g_pRecordPathUpSend;//上行发送调节日志
extern char *g_pRecordPathDownDB;//下行数据中心交互日志

//变量保存私有文件名
extern char *g_pFixedAreaName;  //定值区特性私有文件
extern char *g_pParamValueName;  //定值参数私有文件

/*******************************************************************************
* 全局变量
*******************************************************************************/
extern DB_CLIENT g_sDataCenterClient;  //和数据中心交互的句柄(新)
extern DB_CLIENT g_sEventDbClient;  //和数据中心交互的句柄(事件使用)
extern METER_FILE_T g_sMeterFileAll;  //终端档案信息
extern BOOL g_bTestEnable;  //遥测遥信电量测试数据开关

/*******************************************************************************
* 计时相关变量声明
*******************************************************************************/
extern PAST_TIME_FLAG_T             g_unPastTimeFlag;                           //过时间标志
extern DATE_TIME_T                  g_stCurBinTime;                             //当前BIN时间
extern DATE_TIME_T                  g_stCurBcdTime;                             //当前BCD时间
extern DATE_TIME_T                  g_stPreBinTime;                             //上次BIN时间

/*******************************************************************************
* 计时相关变量声明
*******************************************************************************/
extern PAST_TIME_FLAG_T             g_unDownPastTimeFlag;                           //过时间标志
extern DATE_TIME_T                  g_stDownCurBinTime;                             //当前BIN时间
extern DATE_TIME_T                  g_stDownCurBcdTime;                             //当前BCD时间
extern DATE_TIME_T                  g_stDownPreBinTime;                             //上次BIN时间

/*******************************************************************************
* 计时相关变量声明
*******************************************************************************/
extern PAST_TIME_FLAG_T             g_eMtPastTimeFlag;                            //过时间标志
extern DATE_TIME_T                  g_stMtCurBinTime;                             //当前BIN时间
extern DATE_TIME_T                  g_stMtCurBcdTime;                             //当前BCD时间
extern DATE_TIME_T                  g_stMtPreBinTime;                             //上次BIN时间

/*******************************************************************************
* 函数声明
*******************************************************************************/

extern void param_data_init();
extern void past_time_init();
extern void past_time_init_down();
extern void task_run_step_init();
extern int meter_file_by_pn_read(uint16 infonum, OOP_METER_T *gMeterFile);
extern uint16 meter_file_init(METER_FILE_T *pMeterFileAll);
extern int config_xml_load();
extern int global_init();
extern void meter_file_change(METER_FILE_T *pMeterFileAll, uint8 choice, OOP_METER_T gMeterFile);
extern int files_update_event_deal(DB_UPDATE_MSG_T *pUpdateMsg, METER_FILE_T *pMeterFileAll);
extern void time_manage();
extern void time_manage_down();
extern void time_manage_mt();
extern int data_block_split(uint32 nOAD, uint8 nItemID, uint8 *buff, float32 *fOut);
extern int para_query_int64(uint16 nParaAddr, int64 *nOut);
extern int para_query_float32(uint16 nParaAddr, float32 *fOut);
extern void system_time_get(uint8 *aTime);
extern const Map645ToOop* oad_info_find(uint32 oad);
extern int meter_addr_query(uint16 Pn, uint8 *pAddr, uint8 nAddrLen);
extern void yx_db_center_read(uint16 index);
extern void yx_real_signal_update_zhj(RST_RMT_T stRmt, uint8 status);
extern int yx_event_signal_update_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nAddr, uint8 nNewState);
extern void yx_real_signal_read(uint16 index);
extern void yx_calculate_sta_set(uint32 subtype, uint8 subindex, uint8 value);
extern void yc_db_nomal_read(uint16 index);
extern void yc_db_record_read(uint16 index);
extern void yc_calculate_statistics(uint16 index);
extern void en_db_nomal_read(uint16 index);
extern void en_db_record_read(uint16 index);
extern void en_calculate_statistics(uint16 index);
extern void data_db_center_read();
extern void para_db_center_save();
extern void mt_calc_entry(DB_CLIENT clientid);

#ifdef __cplusplus
}
#endif

#endif // __PW_COMMON_H__