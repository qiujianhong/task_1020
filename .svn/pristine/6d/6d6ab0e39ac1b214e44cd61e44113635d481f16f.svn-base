
#ifndef __CTSCOMMON_H__
#define __CTSCOMMON_H__

#define CHECK_RESULT_SUCCEED            0
#define CHECK_RESULT_FAILUER            -1
#define CHECK_RESULT_CONFIRM            1
#define CHECK_RESULT_DENY                2
#define TASK_CFG_PARA     0x80      //任务配置单元
#define NOR_ACQ_PARA      0x81      //普通采集方案
#define MTR_EVT_PARA      0x82      //事件采集方案
#define REPORT_CFG_PARA   0x83      //上报方案
#define TRANS_PARENT_PARA   0x84    //透明任务方案

#define MAX_MSG_DELAY_TIME  6            //消息最大超时时间
#define MAX_OAD_RD_LEN             1000                      //单帧最大抄读的OAD长度 电表超过512字节就会分帧
#ifdef AREA_JIANGSU
#define PATCH_MAX_DAY               7   //往前最大补抄时间
#else
#define PATCH_MAX_DAY               3   //往前最大补抄时间
#endif
#define MAX_METER_FAILURE_COUNT     2   //一轮抄表单块表最大失败次数 暂时改为2次
#define MAX_METER_ALL_FAILURE_COUNT (3*3)    //一块表最大失败次数 超过后出发重新组网流程 每轮会有3次失败
#define MAX_METER_ALL_FAILURE_TIME    (3600*2)  //一块表最大失败时间 
#define ROUND_FAIL_CNT_LIMIT 2      //如果两轮都没抄通 重试次数改为1 以防表一直抄不通影响效率

#define BROAD_TIMING_OVERTIME       60      //广播校时超时时间

#define PATCH_FILE_VERSION      "XL-1"
#define TASK_RECORD_VERSION     "TR-2"

#define FILE_TRANSFER_OVERTIME 30     //文件代发超时时间
#define BintoBCD(num)	        ((num)/10*16+(num)%10)
#define OOP_ADDRLEN		6
#define OOP_0XFE_CNT	0		/* 面向对象0xFE个数*/
#define R_FRAME_ERROR				0xFFFF		//接收帧乱码
#define OOP_ADDRINDEX	 5
#define NUM_99 		         99
#define MAX_METER_CNT               32          //测量点个数
#define MIN_PRTL_LEN	12
#define ACQ_ADDR_INDEX              1           //地址域索引位置


/*******************************************************************************
* 湖南电表事件上报默认事件
*******************************************************************************/
static const OOP_OAD_U g_report_301f_obj[]=   //电能表拉闸事件  电能表合闸事件 蓝牙微断跳闸事件 蓝牙微断合闸事件
{
    {0x20220200},//事件记录序号
    {0x201E0200},//事件发生时间
    {0x20200200},//事件结束时间
    {0x20240200},//事件发生源
};

/*******************************************************************************
* 每次读取 删除 添加 路由的数目
*******************************************************************************/
#define    MODULEDEAL_CNT                            100




//锁共享缓冲区结构




//锁共享缓冲区结构


extern const MapOopTo645_NEW MapOopTo645_RT[];
extern const MapOopTo645_NEW MapOopTo188_RT[];
extern const MapOopTo645_NEW MapOopTo188_RT_New[];
extern const MapOopTo645_NEW MapOopTo188_DAY[];

extern const uint16 Map_RT_len;
extern void taskmng_time_manage_task();
extern uint8 taskmng_mtr_pipe_get(OOP_OAD_U *oad);

extern uint16 taskmng_oad_list_get(OOP_RECORD_COL_T *inRecdCol,CSD_TASK_INFO *pCsdInfo,AcqInfo_t *Acqtype);
extern void taskmng_csd_task_add(OOP_OAD_U MainOad,uint8 SubOadNum, OOP_OAD_U *SubOad,CSD_TASK_INFO *pCsdInfo,AcqInfo_t *Acqtype);
extern BOOL IsMtrAddrMatch(OOP_TSA_T *Addr1,OOP_TSA_T *Addr2);
extern void TsaCalcfun(OOP_TSA_T *TsaAddr,int Fun);
extern void CateSchemeTaskByTI(OOP_TASK_CFG_LIST *pSchemeTaskParaList,SCHEME_TASK_RUN_INFO *SchemeTaskRunQueue);


//extern void SchemeTaskMonitorUpdata(uint16 TaskID,UpdateTaskCmd_t cmd,void *indata);
extern void taskmng_past_time_deal_init ( uint8 CommPipe );
extern void taskmng_all_state_init ( uint8 CommPipe );
extern int8 Remove_RepeatPlcTask ( uint8 CommPipe,MAIN_CYC_STATE_T TaskName );
extern void InsertPlcTaskQueue ( uint8 CommPipe,PLCTASK_QUEUE* PlcAddTask );
extern PLCTASK_QUEUE* SortPlcTaskQueue ( PLCTASK_QUEUE* pHeadTaskLink,PLCTASK_QUEUE* PlcAddTask );

//extern int8 UnPackMsgData(uint16 Len, uint8 *Buff, MSG_T *Msg);
extern void ClearCommCbRecvBuff(uint8 CommPipe);
extern int taskmng_meter_file_cmp_deal(uint8 Pipe);
extern MODULE_METADDRHEAD *taskmng_module_head_node_delete(MODULE_METADDRHEAD *pModuleMetAddrHead,uint16 DeleteNum);
extern void *taskmng_malloc(char const* file, int line, size_t size);
extern void taskmng_free(char const* file, int line, void *p);
extern MODULE_METADDRHEAD *taskmng_module_head_node_add(uint16 RouterSn,uint8 *Addr,uint8 prtl,MODULE_METADDRHEAD *pModuleMetAddrHead);
extern int SerachNodeInLink(MODULE_METADDRHEAD *pModuleMetAddrHead,uint8 *Addr);
extern void TaskRunSort(uint8 Pipe);
extern BOOL TaskRun_UpdateTimes(uint8 Pipe,uint8 index,CTS_RUN_TASK_T *TaskInfo);
extern void TaskRun_UpdatePeriodStat(uint8 index,CTS_RUN_TASK_T *TaskInfo);
extern uint8 GetPriorityFromTaskID(uint8 Pipe,uint8 TaskId);
extern int taskmng_oop_meter_security_para_read(uint8 Pipe,uint16 Pn,uint8 *MtrEsamSerialNo,uint8 *MtrSecModEn);
extern uint16 taskmng_meter_frame_make(uint8 Pipe,uint8 seqindex,time_t StartTime,FRAME_SEQ_INFO_T FrameSeqInfo,uint8 *DataBuff,uint16 taskid,uint8 logicid);
extern int taskmng_oop_meter_get_request(uint8 Pipe,uint8 seqindex,time_t StartTime, int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff);
extern int MakeRsdFrame(uint8 Pipe,uint8 Selector,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink, OOP_OAD_U RsdOad,int MsIndex,time_t StartTime);
extern int taskmng_meter_prtl_645_pack(uint8 *PrtlData, MeterPrtlFmt_T MeterData);
extern int taskmng_paral_meter_read_data_unpack(uint8 Pipe,uint8 seqindex,uint8 logicid, AMR_PLC_INFO_REPORT_T AmrReportInfo, MeterPrtlFmt_T *MeterPrtlData, uint16 *MtrIndex,char *apdu, FRAME_SEQ_INFO_T FrameSeqInfo);
extern uint32 bin2_int32u(uint8 bin[4]);
extern BOOL process_result_data(OOP_OAD_U Mainoad,OOP_OAD_U Suboad ,uint8 *sourbuffer,uint8 *metaddr, uint32 colStartTm,uint32 colEndTm,uint32 colStoreTm,uint8 logicId,uint16 infoNum);
extern BOOL ConverOopDataToStruct(OOP_OAD_U Suboad ,uint8* sourbuffer,uint8 *desbuffer,uint16 *souroffset,uint16 *desoffset);
extern int taskmng_645_event_DI_search(uint8 Pipe,uint16 MsIndex,uint8 prtl,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T *CurTask,DI_INFO_T* DI);
extern BOOL taskmng_mtr_result_check(uint16 index,uint8 *MtrResult);
extern void taskmng_mtr_result_set(uint16 index,uint8 *MtrResult);
extern void ReOrderPatchFile(uint8 Pipe);
extern void CreatPatchFile(uint8 Pipe,uint8 DayCnt);
extern int IsMtrSnInList(uint16 MtrSn,uint16 MsNum,MS_INFO *pMsInfo);
extern void CheckPatchFile(uint8 Pipe);
extern void RestPlcTaskStat ( PLCTASK_QUEUE* PlcCurTask );
extern int8 taskmng_645_meter_data_unpack(uint16 FrameLen, uint8 *FrameBuff, uint8 *MtrAddr , uint8 *CtrlCode, uint16 *DataLen, uint8 *DataBuff);
extern void CreatPatchFile_TaskId(uint8 Pipe,SCHEME_TASK_RUN_INFO *Tmp_AmrLink,uint8 DayCnt);
extern int taskmng_plc_module_check(uint8 pipe);
extern uint16 GetNeedMtrNum(uint8 pipe,uint8 taskindex);
extern BOOL get_task_run_frequence(uint16 TaskId,OOP_TI_T *Ti);
extern void WriteOopMeterSecurityEsamSerialNo(uint8 Pipe,uint16 Pn,uint8 *MtrEsamSerialNo);
extern void WriteOopMeterSecuritySecModEn(uint8 Pipe,uint16 Pn,uint8 MtrSecModEn);
extern int ReadOopMeterSecurityEsamSerialNo(uint8 Pipe,uint16 Pn,uint8 *MtrEsamSerialNo);
extern int ReadOopMeterSecuritySecModEn(uint8 Pipe,uint16 Pn,uint8 *MtrSecModEn);

extern void taskmng_698_addr_get(uint8 *inframe,uint16 inLen,uint8 *Addr);
void up_msg_response(MESSAGE_INFO_T* MsgInfo ,uint8 result);
extern uint16 GetMsgSeq();
extern int UnpackRelay485Data_0010(uint8 pipe,uint8* DataBuff,uint16 DataLen,MeterPrtlFmt_T*  MeterPrtlData,char* apdu);
//extern int RelayCmdMeter485PrtlData ( uint8 pipe,int fd232);
extern int PatchDataReading(uint8 pipe);
extern int GetPatchTaskInfo(uint8 pipe,uint8 daycnt,CTS_RUN_TASK_T** patchtask);
extern int GetNextPatchDI(uint8 pipe,DI_INFO_T* DiInfo, int *MsIndex,SCHEME_TASK_RUN_INFO *TaskInfo,int8 daycnt);
extern uint8 GetPatchOopOad(uint8 Pipe,uint16 Pn,DI_INFO_T* DiInfo, int MsIndex,SCHEME_TASK_RUN_INFO *TaskInfo);
extern int SearchPatch645DI(uint8 Pipe,uint16 MsIndex,uint8 prtl,SCHEME_TASK_RUN_INFO *TaskInfo,uint8* DI);
extern void InitPatchTask(SCHEME_TASK_RUN_INFO *pSchemeTaskRunInfo,PATCH_MTR_INFO_T* PatchMtr ,PATCH_TASK_RUN_T* tmpTask, time_t PatchTime);
extern uint8 GetPatchCurNum(time_t patchtime,OI MainOI);
extern uint16 PatchMakeFrame(uint8 Pipe,uint16 seqindex,time_t StartTime,FRAME_SEQ_INFO_T FrameSeqInfo,uint8 *DataBuff,SCHEME_TASK_RUN_INFO *pTmp_NormalLink);
extern int OopPatchGetRequst(uint8 Pipe,uint8 seqindex, time_t StartTime,int MsIndex,OOP_OAD_U MainOad,uint8 SubOadNum,OOP_OAD_U *SubOad,uint16 *FrameLen, uint8 *FrameBuff,SCHEME_TASK_RUN_INFO *pTmp_NormalLink);
extern int MakePatchRsdFrame(uint8 Pipe,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime);
extern void RefreshPatchPnState(uint8 Pipe,uint8 SeqIndex);
extern int JudgePatchMoveNextAmrTaskIndex(uint8 Pipe,uint16 MsIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,uint8 *pRecvDI, char CtrlCode, uint16* DataLen);
extern int PatchAmrMakeFrame(uint8 pipe,int8 daycnt);
extern void JudgePatchFrz07MoveNext(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,int MsIndex, uint8 Result);
extern void WritePatchResult(uint8 pipe,uint16 taskid,uint8 DayCnt,uint8 timeindex,PATCH_TIME_T *meterinfo);
extern uint16 GetPatchNeedMtrNum(PATCH_TIME_T PatchTime);
extern uint16 PackUpMsgData(uint16 IID,uint16 IOP,uint16 len,uint8 *PayLoad,char* DestAddr,uint16 *MsgIndex);
extern int taskmng_up_msg_fun_deal(uint16 IID,uint16 IOP,uint16 len,uint8 *PayLoad,char* DestAddr,uint16 *RecvLoadLen,uint8 *RecvPayLoad);
extern int taskmng_module_info_get(uint8 ModuleIndex);
extern int taskmng_local_addr_read(uint8 *addr);
extern void DBUpdateFun(MESSAGE_INFO_T *ptMsg);
extern int task_send_msg(int sockfd, uint32 msgid, uint8 *msgdata, uint16 datalen, uint32 dest);
extern void taskmng_all_module_info_get();
extern void ModuleEventFun(uint8* buf,uint16 len);
extern void taskmng_search_meter_status_write(uint8 pipe,uint8 status);
extern void SearchMeterUpFun(uint8* buf,uint16 len);
extern void ResetSearchMeterSeq();
extern uint16 GetSearchMeterSeq();
extern void MsgErrorFunc(MESSAGE_INFO_T *MsgRecv,int MSGLen);
extern OOP_OAD_U  taskmng_pipe_oad_get(uint8 pipe);
extern void CheckTaskRecordFile(uint8 pipe);
extern void CreateTaskRecordFile(uint8 pipe,uint16 TaskId,uint8 logicid);
extern void CheckTaskRecordFileMeter(uint8 pipe,uint16 TaskId,uint8 logicid);
extern uint8 GetTaskState(uint16 TaskId,uint8 logicid);
extern void ErrorUpMsg(MESSAGE_INFO_T* MsgInfo );
extern uint16 MakeDateTimeS(uint8* buf,time_t time);
extern BOOL CheckMeterSearchResult(uint8 pipe,OOP_SEARCHMET_RES_T searchresult,uint16 *infonum);
extern int EventReportDeal(uint8 pipe);
extern void EventReportMqtt(uint8* buf,uint16 len);
extern int ReportEventStateFun(uint8 pipe,uint8* buf,uint16 len,uint16 index);
extern uint16 MakeRelay485Data_0010(uint8 pipe,DI_INFO_T DiInfo,uint8 prtl,uint8* DataBuff);
extern int GetNext485RelayDI(uint8 pipe,DI_INFO_T* DiInfo,uint8* prtl);

extern int EventReportRecvFun(uint8 pipe,uint8* buf,uint16 len);
extern int EventReportSendFrame(uint8 pipe,uint8 monitorParallFlag);
extern int GetEventSubOad(uint16 pn,OOP_OAD_U MainOad,OOP_OAD_U* SubOad,uint16 *newpn);
extern void SetEventReportNum(uint16 InfoNum,OOP_OAD_U Oad);
extern uint16 taskmng_event_report_num_get(OOP_OAD_U Oad);
extern void taskmng_event_report_num_load();
extern void Save698EventData(uint8 pipe,uint8* DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U MainOad,time_t StartTime);
extern void taskmng_event_report_data_write(uint8 pipe,uint8* DataBuf,uint16 DataLen,OOP_OAD_U Oad);
extern void AddEventReport(uint8* Addr);
extern void OOPFollowReport(uint8 pipe,uint16 Pn,uint8* buf,uint16 len);
extern void taskmng_normal_data_delete(uint16 infoNum,OOP_OAD_U Oad);
extern void HPLCInfoUpFun(uint8* buf,uint16 len);


extern void DeleteHPLCInfo(uint8 pipe);
extern uint32 GetTaskReportSeq(uint8 pipe,uint16 TaskId,uint8 logicid,OOP_OAD_U MainOad,uint16 Pn);
extern void SetTaskReportSeq(uint8 pipe,uint16 TaskId,uint8 logicid,OOP_OAD_U MainOad,uint16 Pn,uint32 seq);
extern uint8 GetTaskReportEventState(uint8 pipe, uint16 TaskId, uint8 logicid, OOP_OAD_U MainOad, uint16 Pn);
extern void SetTaskReportEventState(uint8 pipe, uint16 TaskId, uint8 logicid, OOP_OAD_U MainOad, uint16 Pn, uint8 state);

extern void GetTaskRealBegiantime(uint8 pipe,uint16 taskID,uint8 logicid,CTS_RUN_TASK_T *indata);

extern BOOL taskmng_register_app_check(char* AppName);
extern BOOL IsModuleInfoValid();
extern BOOLEAN GetNeedAgainTaskInfo(SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T **RunTask,uint8 Pipe);
extern int Rs485TransFrameSend(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask);

extern int GetNext485SetRelayData(uint8 pipe);
extern void SetTGRecordFile(uint16 TaskId);
extern int Rs485TransDataReading(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask);


extern void taskmng_net_topology_seq_reset();
extern uint16 taskmng_net_topology_seq_get();
extern void SaveEventPowerOnOff(uint8 pipe,uint8* Addr,uint8 EventType);
extern void taskmng_search_meter_param_load(void);
extern void taskmng_perception_tmn_info_load();

extern void SearchMeterCheck(uint8 pipe);
extern void DBInitFun(uint8* buf,uint16 len);
extern void taskmng_sync_time_param_load();
extern void SyncTimingCheck(uint8 pipe);
extern int BroadTiming(uint8 pipe);
extern int BroadTimingSend(uint8 pipe);
extern uint16 PackSetTimeFrame(uint8 pipe,uint8* outBuf,uint8 prtl,uint8 isBroad,uint16 delay);
extern int GetBroadDelay(uint8 pipe);
extern int SetMeterTime(uint8 pipe);
extern void SaveMeterTime(uint8 pipe,uint8* buf,uint16 len);
extern int GetMeterDelay(uint8 pipe);
extern int GetMeterTime(uint8 pipe);
extern void SendFrame_0038(uint8 pipe,uint8 prtl,uint8 *Addr,uint8 *buf,uint16 len,uint16 *MsgIndex);
extern int SendBroadTiming(uint8 pipe);
extern void BroadTimingRecv(uint8 pipe,uint8* buf,uint16 len);
extern void SaveMeterTimingEvent(uint8 pipe);
extern void taskmng_update_cycle_load();

extern void taskmng_plc_pipe_set(uint8 pipe,uint8 choice);
extern int Rs485EventReportSendFrame(uint8 pipe);
extern int Rs485EventReportRecvFun(uint8 pipe,uint8* buf,uint16 len);
extern void OOPRS485FollowReport(uint8 pipe,uint16 Pn,uint8* buf,uint16 len);
extern void DealEventReport(uint8 pipe,uint8 prtl ,uint16 pn);
extern void Appversion_Response(MESSAGE_INFO_T *MsgRecv );
extern BOOL taskmng_oop_frame_broadtiming_check(uint8* buf,uint16 len,time_t *settime);
extern int8 taskmng_frame_3762_format_check(uint16 FrameLen, uint8 *FrameBuff, uint16 *FrameEnd);
extern void LoadHPLCChannel();
extern void taskmng_HPLC_across_area_flag_load();
extern void taskmng_HPLC_node_reject_flag_load();
extern void SetHPLCChannel(uint8 pipe,uint8 channel);
extern BOOL taskmng_prtl_isvalid_check(uint8 prtl);
extern void taskmng_search_meter_clean_load(void);
extern int taskmng_search_meter_clear_param_get();
extern void taskmng_across_area_result_load();
extern BOOL CheckAcrossAreaResult(uint8 *Addr,uint16 *infonum);
extern void taskmng_across_area_result_change_fun(uint16 infonum);
extern int taskmng_across_area_result_read_from_pn(uint16 infonum,OOP_ACROSSAREA_RES_T *result);
extern void taskmng_meter_search_result_clear();
extern void taskmng_meter_search_result_change_fun(uint16 infonum);
extern BOOL taskmng_amr_state_get_from_cco(uint8 pipe,AMR_STATE_T *state);
extern int Rs485BroadTiming(uint8 pipe);
extern int Rs485BroadTimingSend(uint8 pipe);
extern void Rs485BroadTimingRecv(uint8 pipe,uint8* buf,uint16 len);
extern int Rs485SendBroadTiming(uint8 pipe);
extern int Rs485SetMeterTime(uint8 pipe);
extern int Rs485GetMeterTime(uint8 pipe);
extern int get_app_id(uint8* appid,int maxlen);
extern void Appid_Response(MESSAGE_INFO_T *MsgRecv );

extern int32 taskmng_db_update_queue_read(DBUPDATE_QUEUE_T *pLockQueue, DB_UPDATE_T *buf);
extern int32 taskmng_db_update_queue_write(DBUPDATE_QUEUE_T *pLockQueue, DB_UPDATE_T *buf);
extern void taskmng_across_area_result_clear();
extern void GetTGRunTask(uint8 TaskId,CTS_RUN_TASK_T **RunTask);
extern BOOL CheckBroadTimingisValid(uint8 type);
extern uint32 GetTaskDelay(uint16 TaskId,uint8 logicid);
extern int MakeRsd(uint8 Pipe,uint8 Selector,uint8 *RsdBuf,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,OOP_OAD_U RsdOad,int MsIndex,time_t StartTime,OOP_MSP_T    *ms);
extern void Change_circuit_Breaker_Data_to_Struct(uint8* tmpbuf ,uint16 len,uint8 pipe);
extern void taskmng_485_dev_find(BOOL plug,uint8 index);
int ReNetworking(uint8 pipe);
uint8 taskmng_curve_point_get(uint16 taskid,uint8 logicid);

uint16 event_report_data_save(uint8 pipe,uint8 *DataBuf,uint16 DataLen,uint16 Pn);
void WritePerceptionTerminalData(uint8 pipe,uint8* DataBuf,uint16 DataLen,OOP_OAD_U Oad);

int8 taskmng_frame_3762_unpack(uint16 FrameLen, uint8 *FrameBuff, AppData_T *AppData);
uint16 deal_reportNotificationlist_data_save(uint8 pipe,uint8 *DataBuf,uint16 DataLen,uint16 Pn,OOP_OAD_U RecvOad);
extern void WriteLoadRate(DB_CLIENT fd);
extern void taskmng_rs485_patch_file_delete_by_pn(uint16 metrno);
extern void taskmng_rs485_delete_all_patch_file(void);
uint16 data_report_data_save(uint8 pipe,uint8 *DataBuf,uint16 DataLen,uint16 Pn);
extern BOOL taskmng_energy_oad_check(OOP_OAD_U oad); 
#ifdef AREA_SHANDONG
void TaskmngAdjustReadOad(SCHEME_TASK_RUN_INFO *pCbTask);
#endif
int SendBroadTimingCipher(uint8 pipe);
BOOLEAN GetAcNeedAgainTaskInfo(SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T **RunTask);
int Rs485SendBroadTimingCipher(uint8 pipe);
extern uint16 taskmng_mtsd_188_get_request( uint8 pipe,uint8 *FrameBuff,int MsIndex);
extern int8 taskmng_188_meter_data_unpack(uint16 FrameLen, uint8 *FrameBuff, uint8 *MtrAddr , uint8 *CtrlCode, uint16 *DataLen, uint8 *DataBuff,uint16 DI);
void UpdatePatchReadInfo(uint8 pipe,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T  *RunTaskInfo);
void GetPatch(uint8 pipe);
uint16 taskmng_mtr_meter_patch(uint8 datalen,uint8 *MtrResult);
void right_shift(uint8 *buf,uint8 len);
void taskmng_meter_event_report_time_set();
BOOL taskmng_meter_event_report_time_check();
int GetMsindexFromAddr(uint8 pipe,uint8 *Addr);

void taskmng_amr_status_get_mqtt(uint8 pipe,uint8* Addr,MESSAGE_INFO_T* MsgInfo);
int GetMsindexFromAddr(uint8 pipe,uint8 *Addr);



#if DBCENTER_MQTT 
extern int DBSendMQTT(uint16 label,uint16 iop,uint8* payload,uint16 len,uint16* pindex);
extern uint16 appmsg_package(MESSAGE_INFO_T *pMsgInfo, uint8 **ppBuf);
extern int mqtt_topic_write(uint8* SendBuf,uint32 SendLen,char* topic);
#endif
#if DESC("终端带电状态",1)
int taskmng_power_state_init();
int taskmng_power_event_deal(DB_CLIENT clientid,uint16 infonum);
void taskmng_power_state_get(TMN_POWER_STATE_T *state);
#endif
#endif
