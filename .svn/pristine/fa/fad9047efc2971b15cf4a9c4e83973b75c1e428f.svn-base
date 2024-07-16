#ifndef __CTSAMR_H__
#define __CTSAMR_H__

extern uint16 taskmng_oop_meter_data_normal_list_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask );
extern uint16 taskmng_oop_meter_data_record_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask,DateTimeBcd_t *StoreTime,uint8 flag);
extern BOOL taskmng_645_meter_data_save_event(uint8 Pipe,uint8 *pBuf, uint16 rcvLen,int MsIndex,SCHEME_TASK_RUN_INFO* pTmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask );
extern void taskmng_645_meter_data_save(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask, DateTimeBcd_t *StoreTime);
extern uint16 OopMeterAmrData(uint8 Pipe,uint16 MtrIndex,OOP_OAD_U MainOad,OOP_OAD_U Oad, uint8* pRcvBuf, uint16 RcvLen, SCHEME_TASK_RUN_INFO* pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask , DateTimeBcd_t *StoreTime);

extern uint16 taskmng_645_meter_data_save_frz(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask);
extern uint16 CacloffsByOad(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink, int MsIndex,uint8 *DataLen);
extern int taskmng_curve_DI_adapt(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink, int MsIndex,  OOP_OAD_U OAD,  const MapOopTo645_NEW* pMap);
extern void taskmng_645_meter_data_save_curve(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask);
extern void taskmng_645_meter_data_save_profile(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask);
extern int CacloffSpecial(uint8 Pipe,uint16 MsIndex,const MapOopTo645_NEW *pOadInfo,uint16  MapIndex,uint8 *pBuf, uint16 *rcvLen);

extern int taskmng_oop_relay_data_normallist_save(uint8 pipe,uint8 *DataBuff,uint16 DataLen);
extern uint16 OopRelay_SavaData(uint8 pipe,uint8 MtrIndex,OOP_OAD_U MainOad,OOP_OAD_U Oad, uint8* pRcvBuf, uint16 RcvLen,DateTimeBcd_t *StoreTime);
extern void taskmng_645_relay_data_save(uint8 pipe,uint8 prtl,uint8* pRcvBuf, uint16 RcvLen);
extern int CacloffSpecialRelay(uint8 Pipe,uint8 prtl,const MapOopTo645_NEW *pOadInfo,uint16  MapIndex,uint8 *pBuf, uint16 *rcvLen);
extern int Oop485Relay_Save(uint8 pipe,uint8 *DataBuff,uint16 DataLen);
extern uint16 OopPatchMeterAmrDataSave_Record(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pTmp_NormalLink,uint8 flag);

extern uint16 OopPatchAmrData(uint8 Pipe,uint16 MtrIndex,OOP_OAD_U MainOad,OOP_OAD_U Oad, uint8* pRcvBuf, uint16 RcvLen, SCHEME_TASK_RUN_INFO* pTmp_NormalLink, DateTimeBcd_t *StoreTime);
extern void OopPatchDataSave9707(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink, DateTimeBcd_t *StoreTime);
extern int CacloffSpecialPatch(uint8 Pipe,uint16 MsIndex,const MapOopTo645_NEW *pOadInfo,uint16  MapIndex,uint8 *pBuf, uint16 *rcvLen);
extern void OopPatchDataSave_curve(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink);
extern void OopPatchDataSave_profile(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink);
extern uint16 CaclPatchoffsByOad(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,int MsIndex,uint8 *DataLen);
extern int CurvePatchDiAdapt(uint8 Pipe,SCHEME_TASK_RUN_INFO* Tmp_AmrLink, int MsIndex, OOP_OAD_U OAD, const MapOopTo645_NEW* pMap);
extern uint16 OopPatchDataSave9707_frz(uint8 Pipe,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink);
extern uint16 taskmng_oop_meter_data_normal_save(uint8 Pipe,uint8 *DataBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO *pTmp_NormalLink,CTS_RUN_TASK_T *RecvRunTask );
void taskmng_cache_buf_save(uint8 pipe,uint16 pn,OOP_OAD_U mainoad,OOP_OAD_U suboad,uint8 *databuf,uint16 datalen,uint32 starttime,uint32   storeTm);
void taskmng_cache_buf_write_to_db(uint8 pipe,uint16 Pn,uint8 logicid);
void taskmng_patch_cache_buf_save(uint8 pipe,uint16 pn,OOP_OAD_U mainoad,OOP_OAD_U suboad,uint8 *databuf,uint16 datalen,uint32 starttime,uint32   storeTm);
void taskmng_patch_cache_buf_write_to_db(uint8 pipe,uint16 Pn,uint8 logicid);
void taskmng_645_meter_data_save_curve_new(uint8 Pipe,uint16 MapIndex,uint8 *pBuf, uint16 rcvLen, int MsIndex,SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask);
int taskmng_curve_DI_adapt_new(SCHEME_TASK_RUN_INFO* Tmp_AmrLink,OOP_OAD_U suboad, OOP_OAD_U OAD, const MapOopTo645_NEW* pMap);
void taskmng_645_meter_data_save_new(uint8 Pipe,uint16 mapindex,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask, DateTimeBcd_t *StoreTime);
uint16 taskmng_645_meter_data_save_frz_new(uint8 Pipe,uint16 mapindex,uint8 *pBuf, uint16 rcvLen, int MsIndex, SCHEME_TASK_RUN_INFO* Tmp_AmrLink,CTS_RUN_TASK_T *RecvRunTask);
uint16 taskmng_188_data_record_save(uint8 Pipe,uint8 *pBuf,uint16 DataLen,uint16 MtrIndex,SCHEME_TASK_RUN_INFO* pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
void taskmng_rs485_cache_buf_save(uint8 pipe,uint16 pn,OOP_OAD_U mainoad,OOP_OAD_U suboad,uint8 *databuf,uint16 datalen,uint32 starttime,uint32   storeTm,DATA_CACHE_T  *meterData);
void taskmng_rs485_cache_buf_write_to_db(uint8 pipe,uint16 Pn,uint8 logicid,DATA_CACHE_T *meterData);

#endif
