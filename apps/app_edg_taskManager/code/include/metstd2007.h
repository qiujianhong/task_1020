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
#ifndef _METSTD2007_H
#define _METSTD2007_H
 #include "CtsAll.h"
#if OS_METSTD2007_EN == 1


#if CBTASK_NEW == 1
typedef struct
{
    uint8    type;            //ID类别
    uint16    bDataId;        //源字典ID（字典格式）
    uint8   BlockOrSingle;    //CB_BLOCK     CB_SINGL
    uint8    dataNum;        //数据项偏移（一个数据ID由多个数据组成）一般都是1
    uint8    srcDataLen;        //表规约格式的长度
    uint8    srcDecLen;        //规约格式的小数点位数
    uint8    rate;            //数据字典格式的小数点位数
    uint32    FraDI;            //单抄或块抄的目标识（表规约格式）
    uint8    DINum;            //块抄的时候，数据项的个数 如果块抄的第一次抄失败就跳到+DINum
    uint8   srcTodes;        //源数据类型到目的数据类型(UBCD_TO_UBCD等)

}cbtasknew_MET2007_PRTL_T;
//CB_RET cbtasknew_STD2007_MetAutkDealEx(S_cbTaskQueue *pPipeCbTaskQueue, enum_cbtask_id taskid, uint8 bStdType);
CB_RET STD64507_MetAutkDeal(SCHEME_TASK_RUN_INFO *pCbTask);

#endif



void STD2007_PowerCut(uint16 metSn,uint8 WriteFlag);
void STD2007_MetPowerDirection(uint16 metSn, uint8 WriteFlag);
void STD2007_MetPhaseAG(uint16 metSn, uint8 WriteFlag);
void Break_VLT_Data(uint16 metSn, uint8 WriteFlag);
void cbtasknew_DataSave_07Ext(mtrnum_t  metSn);
uint16 STD64507_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);

//void FormatConvertToOop(dataFormat *pDF);
//extern void FormatToStd2007(uint16 eDataId, dataFormat *pDF, uint8 bStdType, uint8 TaskType, mtrnum_t MetSn);
extern void STD2007_MakeFrame(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, uint8 bStdType);
#define  POINT_LASTTIME_FRADI 0x05060001 //（上1次）日冻结时间
#define  CURVEDATE_EN   1 //曲线数据
extern void Prepare_645_07_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe, CTS_RUN_TASK_T   *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);
extern void STD2007_LoadPrifle_Init(cbLPPara_t *pCbLpPara ,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T   *RunTaskInfo);
uint16 STD64507_AUTK_Recv_Profile(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
BOOL STD2007_FrameCombin(OOP_OAD_U mainOad,OOP_OAD_U suboad,uint32 DI07,uint8 DI07Index,uint8 *inData,uint8 inLen,uint8 *outData,uint16 *outLen);
uint16 STD64507_AUTK_Recv_Frz(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
uint16 STD64507_AUTK_Recv_Event(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,uint8 *DI07Index,uint8 DI07Num,CTS_RUN_TASK_T *RunTaskInfo);
extern CB_STATE Cb_State_Init_645_07(S_COMM_INFO_MET *Comm_Info_Oop,uint8 pipe,cbMtrPara    *MtrPara);
extern void Meter_645_07_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 *DI07Num,uint32 *DI07Buff);
extern void Meter_645_07_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff);
extern void  STD07_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern void STD2007_MakeFrameEx(uint8 *pMetAddr, uint32 fraDI, uint8 cmdDI, uint8 *pBuf, uint8 *pSndLen, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask, CTS_RUN_TASK_T *RunTaskInfo);
extern uint16 STD64507_AUTK_Recv_One(uint8 pipe,OOP_OAD_U oad,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern void ReduceGap(uint8 *pRecvBuf, uint8 offs, uint8  gapValue);
extern CB_STATE Deal_Receive_Result_Protector_645_07(S_COMM_INFO_MET *Comm_Info_Oop,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,BOOL actResult,uint8 pipe);
extern void Protector_645_07_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI07Index,uint8 DI07Num,uint8 *CombinBuff,uint16 *CombinLen,uint32 *DI07Buff);
extern uint16 Protector_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, DateTimeBcd_t *StoreTime,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);

#endif
#endif

