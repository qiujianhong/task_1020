/*-
****************************************************************************************
 * Copyright (c) 2006,�Ͼ��������������������ι�˾
 *            All rights reserved.
 * �������ƣ�metlgr.h
 * �汾�ţ�1.0
 * ���ܣ������̬�⺯������
 * �����ˣ��ȱ�
 * ����ʱ�䣺2006��12��22��
 * �޸���:
 * �޸�ʱ��:
 * �޸ļ�Ҫ˵��:
 * ������
*****************************************************************************************
-*/
#ifndef _METSTD2007_H
#define _METSTD2007_H
 #include "CtsAll.h"
#if OS_METSTD2007_EN == 1


#if CBTASK_NEW == 1
typedef struct
{
    uint8    type;            //ID���
    uint16    bDataId;        //Դ�ֵ�ID���ֵ��ʽ��
    uint8   BlockOrSingle;    //CB_BLOCK     CB_SINGL
    uint8    dataNum;        //������ƫ�ƣ�һ������ID�ɶ��������ɣ�һ�㶼��1
    uint8    srcDataLen;        //���Լ��ʽ�ĳ���
    uint8    srcDecLen;        //��Լ��ʽ��С����λ��
    uint8    rate;            //�����ֵ��ʽ��С����λ��
    uint32    FraDI;            //������鳭��Ŀ��ʶ�����Լ��ʽ��
    uint8    DINum;            //�鳭��ʱ��������ĸ��� ����鳭�ĵ�һ�γ�ʧ�ܾ�����+DINum
    uint8   srcTodes;        //Դ�������͵�Ŀ����������(UBCD_TO_UBCD��)

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
#define  POINT_LASTTIME_FRADI 0x05060001 //����1�Σ��ն���ʱ��
#define  CURVEDATE_EN   1 //��������
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

