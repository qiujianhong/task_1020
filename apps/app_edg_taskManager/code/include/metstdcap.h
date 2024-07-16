/*-
****************************************************************************************
 * Copyright (c) 2006,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：metlgr.h
 * 版本号：1.0
 * 功能：国标表动态库函数声明
 * 开发人：尤斌
 * 开发时间：2006年12月22日
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他：
*****************************************************************************************
-*/

extern CB_STATE Cb_State_Init_Cap(S_COMM_INFO_MET *Comm_Info_Oop,uint8 pipe,cbMtrPara   *MtrPara);
extern void Meter_Cap_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff);
extern void Meter_Cap_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff);

void STDCAP_MakeFrameEx(uint8 *SourAddr, uint8 *DestAddr, uint8 S_Type, uint8 Command, uint8 CmdType, uint8 *pBuf, uint8 *pSndLen,uint8 *Data, uint8 DataLen);

char *commfun_memchr_cap(char *buf, uint8 sync, uint32 buf_len);

uint16 STDCAP_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,S_COMM_INFO_MET *Comm_Info_Oop,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);

void STDCAP_MakeFrameEx(uint8 *SourAddr, uint8 *DestAddr, uint8 S_Type, uint8 Command, uint8 CmdType, uint8 *pBuf, uint8 *pSndLen,uint8 *Data, uint8 DataLen);

extern uint16 CaclCapBiasByOad(OOP_OAD_U subOad,MapOopToCap *tmppMapOopToCap,uint8 mapindex);


void Convert_Cap_To_Struct(DataSource sourcedata ,uint8 *desbuffer,uint16 *desLen,const MapOopToCap *pOadInfo);
extern void Prepare_Cap_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);
extern int STDCAP_CheckRspFrame(uint8 pipe, uint8 *pBuf, uint16 rcvLen);


