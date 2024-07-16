#ifndef _CTS_TASK_H_
#define _CTS_TASK_H_

 uint16 taskmng_task_need_mtr_num_get(uint8 taskindex);
 void taskmng_task_info_get(uint8 TaskId,uint8 logicid,SCHEME_TASK_RUN_INFO *TaskInfo);
 void taskmng_task_finish(DB_CLIENT clientid,uint16 TaskId);
 void taskmng_task_monitor_update(uint8 pipe,uint16 taskID,uint8 logicid,UpdateTaskCmd_t cmd,void *indata);
 BOOL taskmng_task_in_period_check(uint8 index,time_t RealTime_t);
 void taskmng_task_run_in_period(CTS_RUN_TASK_T * TaskInfo,OOP_TASK_CFG *pTaskCfg);
 DateTimeBcd_t taskmng_task_store_time_get_tg(CTS_RUN_TASK_T *RunTask,SCHEME_TASK_RUN_INFO* pTmp_NormalLink);
 CTS_RUN_TASK_T *taskmng_scheme_task_link_add(CTS_RUN_TASK_T *pAddrSchemeTaskRunInfo,CTS_RUN_TASK_T *HeadSchemeTaskRunInfo);
 DateTimeBcd_t taskmng_scheme_task_store_time_get(uint8 pipe,SCHEME_TASK_RUN_INFO* pTmp_NormalLink);
 DateTimeBcd_t taskmng_patch_scheme_task_store_time_get(uint8 pipe,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink);
 void taskmng_task_db_update_check(DB_UPDATE_T *dbupdate);
 void taskmng_scheme_task_ergodic(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue);
 void taskmng_task_monitor_init(uint16 TaskId,uint8 logicid);
 void taskmng_task_monitor_load();
 void taskmng_event_plan_list_load();
 BOOL taskmng_task_modify_fun(DB_UPDATE_T dbupdate,OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue);
 int taskmng_normal_plan_info_read(uint16 id,OOP_PLAN_NORMAL_T *PlanInfo,uint8 logicid);
 int taskmng_eventl_plan_info_read(uint16 id,OOP_PLAN_EVENT_T *PlanInfo,uint8 logicid);
 int taskmng_trans_plan_info_read(uint16 id,OOP_PLAN_TRANS_T *PlanInfok,uint8 logicid);
 void taskmng_task_record_file_delete(uint16 TaskId,uint8 logicid);
uint16 taskmng_ms_list_get(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid);
 uint16 taskmng_road_list_add(OOP_EVENTSELECT_T *inEvents,TASK_INFO *pEventTaskInfo);
 uint8 taskmng_priority_fix_by_mainoi(SCHEME_TASK_RUN_INFO  *pSchemeTaskTaskRunInfo, OOP_TASK_CFG OopTaskCfg);
 int taskmng_trans_plan_info_get(OOP_TRANS_ARRAY_T *TransArray,MS_INFO *pMsInfo,OOP_TRANS_INFO_T *TransInfo,uint8 logicid);
 BOOL taskmng_normal_plan_change_fun(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 choice,OOP_PLAN_NORMAL_T *NormalPlanInfo,uint8 logicid);
 BOOL taskmng_event_plan_change_fun(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 choice,OOP_PLAN_EVENT_T *EventPlanInfo,uint8 logicid);
 BOOL taskmng_trans_plan_change_fun(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue,uint8 choice,OOP_PLAN_TRANS_T *TransPlanInfo);
 uint32 taskmng_task_period_get(uint16 TaskId,uint8 logicid);
 BOOL taskmng_task_period_check(SCHEME_TASK_RUN_INFO* Tmp_AmrLink);
 DateTimeBcd_t taskmng_acq_store_time_get(StoreTimeType_t storeType,time_t startTime);
 int taskmng_encrypt_plan_info_read(uint16 id,OOP_PLAN_ENCRYPT_T *PlanInfo);
 int taskmng_encrypt_plan_info_get(OOP_PLAN_ENCRYPT_T *planinfo,MS_INFO *pMsInfo,OOP_ENCRYPT_DATA_T *encryptData);

 OOP_OAD_U taskmng_task_mainoad_get(uint8 taskindex);
 void taskmng_file_update_encrypt_plan_list_load();


#endif


