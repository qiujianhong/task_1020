
#ifndef _MET_OOP_CB_H_
#define _MET_OOP_CB_H_


extern CB_RET Oop_MetAutkDeal(SCHEME_TASK_RUN_INFO *pCbTask);
//extern CB_RET Oop_MetAutkDealEx(S_cbTaskQueue *pAllCbTaskQueue, enum_cbtask_id taskid, uint8 bStdType);

extern void Prepare_OOP_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T   *RunTaskInfo,S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);
extern uint16 Oop_AUTK_Recv(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern uint16 Oop_AUTK_Recv_Profile(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
int InvokeGetRequstTest(uint8 pipe,OOP_TSA_T *sa,CSD_TASK_INFO  *csd, S_COMM_INFO_MET *pCommInfo,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo );
extern void Cb_State_Init_Oop(S_COMM_INFO_MET *Comm_Info_Oop,uint8 pipe,cbMtrPara    *MtrPara);
extern void Meter_Oop_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);
extern void Oop_LoadPrifle_Init(cbLPPara_t *pCbLpPara ,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern void  Oop_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe,OOP_OAD_U Suboad);
extern int addCharDataByte(uint8 *tmp_buff,uint8 *sendBuff,int sendLen,uint32 num);
extern void Meter_Oop_recvive_data( SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);
extern void OopEventCntReset(OOP_OAD_U mainOad,cbMtrPara mCbMtrPara,uint8 pipe);
extern BOOL Compare_Buffer(uint8 *buf1 ,uint8 *buf2,uint32 buflen1 ,uint32 buflen2);
extern void  IsNeedBcMeterOopFrzDayData(SCHEME_TASK_RUN_INFO *pCbTask,uint8 *pBuf,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe);
extern CB_STATE Deal_Receive_Result_Oop(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, BOOL actResult,uint8 pipe,OOP_OAD_U Suboad);
uint16 Oop_AUTK_Recv_NormalList(uint8 pipe,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop);
uint16 Oop_Autk_Save_Buf(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,DATA_CACHE_T *meterData);

#endif
