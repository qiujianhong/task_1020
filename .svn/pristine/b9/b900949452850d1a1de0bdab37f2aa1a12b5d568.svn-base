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
#define OS_METDLT645_EN 1

#ifndef _METSTD_H
#define _METSTD_H
 #include "CtsAll.h"
#if OS_METDLT645_EN ==1

//#ifdef FIRST_DEFINE_METPARAM
//cbMetPara STDPara =
//{
//    JFPG,/*-表征费率顺序-*/
//    1200,/*波特率*/
//    8,   /*数据位, 4-8 */
//    'e', /*校验 'n'  'e'  'o'  's' = None,Odd,Even,Space*/
//    1,   /*停止位 0,1,2 = 1, 1.5, 2*/
//    0x68,/*-同步头--*/
//    0xff,/*结束字节1*/
//    0xFF,/*结束字节2*/
//    0x09,/*长度1    */
//    0xFF,/*长度2    */
//    12,  /*附加长度,*/
//    100, /*--电表反应时间:命令码发出后，收到电表返回的第一个字节的时间--*/
//    100, /*--命令发送时间:发送命令报文的时间--*/
//    20,  /*--字节之间的间隔时间--*/
//    0,   /*-给有握手过程的电表使用,握手帧总数-*/
//    0    /*-给有握手过程的电表使用,读取数据帧总数-*/
//};
//#else
//extern cbMetPara STDPara;
//#endif


void STD_MetPowerDirection(uint8 metSn, uint8 WriteFlag);
void STD_MetVolCurDirection(uint8 metSn, uint8 WriteFlag);
void STD_MetUIPower_Direction(uint16 metSn, uint8 WriteFlag);
void STD_State_To_2007(uint16 metSn, uint8 WriteFlag);
extern void  STD97_ToNextReadItem(BOOL NextCSDFlg,S_COMM_INFO_MET *pComm_Info,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 *DI97Index);

extern void  STD_MakeFrame(uint8 * pMetAddr, uint16 fraDI, uint8 cmdDI, uint8 * pBuf, uint8 * pSndLen, uint8 bStdType);

extern void Prepare_645_97_meter_read(SCHEME_TASK_RUN_INFO *pCbTask, uint8 pipe,CTS_RUN_TASK_T   *RunTaskInfo, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara);

extern BOOL STD1997_FrameCombin(OOP_OAD_U mainOad,OOP_OAD_U suboad,uint16 DI97,uint8 DI97Index,uint8 *inData,uint16 inLen,uint8 *outData,uint16 *outLen);
extern uint16 STD64597_AUTK_Recv(uint8 pipe,OOP_OAD_U mainOAD,OOP_OAD_U subOAD,uint8 *pBuf, uint16 rcvLen, cbMtrPara mCbMtrPara, SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern BOOL IsGroupByDI97(uint16 sDI97,uint16 dDI97);
extern CB_STATE Cb_State_Init_645_97(S_COMM_INFO_MET *Comm_Info_Oop,uint8 pipe,cbMtrPara *MtrPara);
#endif

#endif

