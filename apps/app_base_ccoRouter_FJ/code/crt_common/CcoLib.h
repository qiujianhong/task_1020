/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __CCO_LIB_H__
#define __CCO_LIB_H__

#include "CcoRedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************************************************************************
* 宏定义小函数
*******************************************************************************/
#define MAKETOLONG(ll, l, h, hh)      (MAKELONG(MAKEWORD(ll, l), MAKEWORD(h, hh)))

/*******************************************************************************
 * 宏定义
*******************************************************************************/
#define MINS_IN_DAY             86400           //每日的秒数

#define PLC_CHANNEL_CTRL        "/dev/tty_io"   //载波模块控制通道
#define UART_DEVICE_HPLC_STR     "uart_hplc"
#define PLC_CHANNEL_DATA        UART_DEVICE_HPLC_STR    //载波模块数据通道

//扩展模块
#define CONTROL_GET_MODSTATE    0x6506          //模块插入识别
#define CONTROL_CLEAR_RTS       0X6502          //复位脚拉低复位模块
#define CONTROL_SET_RTS         0X6501          //复位脚拉高

/*******************************************************************************
 * 变量申明
*******************************************************************************/
extern LockQueue_T *pAmrRecvDown[MAX_DOWN_THREAD_NUM];
#ifdef AREA_FUJIAN
extern LockQueue_T  g_CcoRecvDown[MAX_DOWN_THREAD_NUM];                      //福建规约抄表线程下行接收共享内存指针
#endif
extern LockQueue_T *pAmrSendDown[MAX_DOWN_THREAD_NUM]; 
extern LockQueue_T *pAmrRecvUp;
extern LockQueue_T *pAmrSendUp; 
extern LockQueue_T  gAmrRecvDownSearchMeter[MAX_DOWN_THREAD_NUM];           //抄表线程下行接收共享内存指针(用于处理搜表接收数据)
extern LockQueue_T  gAmrRecvDownReportModule[MAX_DOWN_THREAD_NUM];          //抄表线程下行接收共享内存指针(用于处理主动上报模块信息)

/*******************************************************************************
 * 函数申明
*******************************************************************************/

extern int32 PlcWriteQueue(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);
extern int32 PlcReadQueue(LockQueue_T *pLockQueue, uint8 *buf);
extern void commfun_printcurtime(BOOL blnNewline, BOOL blnPrint);
extern void commfun_printstring(uint8 *buf, int16 len);
extern DateTime_T SecToDateTime_T(time_t sec_t);
extern DateTime_T DateTimeToBCDDateTime_T(DateTime_T dt_t);
extern uint8 BcdToHex(uint8 compress_bcd);
extern time_t BcdTimeToSec(uint8* BcdMeterTime);
extern uint8 HexToBcd(uint8 hex);
extern void GetCurBinTime(DateTime_T *pTime);
extern void GetCurBcdTime(DateTime_T *pTime);
extern void Uint32ToCharArr(const uint32 num, uint8* arr);
extern void Uint32ToCharArr_r(const uint32 num, uint8* arr);
extern void Uint16ToCharArr(const uint16 num, uint8* arr);
extern void Uint16ToCharArr_r(const uint16 num, uint8* arr);
extern void CharArrToUint32(const uint8* arr, uint32 *num);
extern void CharArrToUint32_r(const uint8* arr, uint32 *num);
extern void CharArrToUint16(const uint8* arr, uint16 *num);
extern void CharArrToUint16_r(const uint8* arr, uint16 *num);
extern uint32 ChangeLenToBuff(const uint16 len, uint16 *IndexID, uint8 *buff);
extern uint32 BuffToChangeLen(const uint8 *buff, uint16 *IndexID, uint16 *len);
extern int8 PortIDToArrIndex(uint32 nPortID, uint8 *nTtlIndex);
extern int8 CtsPortIDToArrIndex(uint16 nPortID, uint8 *nTtlIndex);
extern uint16 crc16(uint8 * ptr, uint8 len);
extern int IsCloseInnerDelayState(int Condition, int *Cnt, int CntMax);
extern uint8 visit_Files(AMR_METER_FILES_INFO_T c);
extern uint8 InitList_Files(AMR_METER_FILES_LIST_T *L) ;
extern uint8 ListEmpty_Files(AMR_METER_FILES_LIST_T L);
extern uint8 ClearList_Files(AMR_METER_FILES_LIST_T *L);
extern uint32 ListLength_Files(AMR_METER_FILES_LIST_T L);
extern uint8 GetElem_Files(AMR_METER_FILES_LIST_T L,int i,AMR_METER_FILES_INFO_T *e);
extern int LocateElem_Files(AMR_METER_FILES_LIST_T L,AMR_METER_FILES_INFO_T e);
extern int LocateElemByPara_Files(AMR_METER_FILES_LIST_T L, AMR_METER_FILES_INFO_T e, AMR_FILES_CMP_PARA_E cmp);
//extern uint16 GetMeterIDByAddr_Files(AMR_METER_FILES_LIST_T L, AMR_METER_FILES_INFO_T e);
extern uint16 UpdateByAddr_Files(AMR_METER_FILES_LIST_T L, AMR_METER_FILES_INFO_T e);
extern uint8 ListInsert_Files(AMR_METER_FILES_LIST_T *L,int i,AMR_METER_FILES_INFO_T e);
extern uint8 ListDelete_Files(AMR_METER_FILES_LIST_T *L,int i,AMR_METER_FILES_INFO_T *e) ;
extern uint8 ListDeleteByIndex_Files(AMR_METER_FILES_LIST_T *L,int i) ;
extern uint8 ListTraverse_Files(AMR_METER_FILES_LIST_T L);
extern void unionL_Files(AMR_METER_FILES_LIST_T *La,AMR_METER_FILES_LIST_T Lb);
//extern uint16 GetIndexByFilesID_Files(AMR_METER_FILES_LIST_T L, uint16 id);
uint16 GetIndexByAddr_Files(AMR_METER_FILES_LIST_T L, uint8 addr[6]);

extern uint8 visit_Task(PLCTASK_DETAIL c);
extern uint8 InitList_Task(PLCTASK_QUEUE *L) ;
extern uint8 ListEmpty_Task(PLCTASK_QUEUE L);
extern uint8 ClearList_Task(PLCTASK_QUEUE *L);
extern int ListLength_Task(PLCTASK_QUEUE L);
extern uint8 GetElem_Task(PLCTASK_QUEUE L,int i,PLCTASK_DETAIL *e);
extern int LocateElem_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e);
extern int LocateElemSameNamePort_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e);
extern int LocateElemPriorityLater_Task(PLCTASK_QUEUE L,PLCTASK_DETAIL e);
extern uint8 ListInsert_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL e);
extern uint8 ListDelete_Task(PLCTASK_QUEUE *L,int i,PLCTASK_DETAIL *e) ;
extern uint8 ListDeleteByIndex_Task(PLCTASK_QUEUE *L,int i) ;
extern uint8 ListTraverse_Task(PLCTASK_QUEUE L);

extern uint32 InfoMsgFactory_1(INFO_TASK_TPYE_T InfoTaskType, INFO_DATA_T *InfoMsgData, uint16	 nMeterFilesID);
extern uint32 InfoMsgFactory_2(INFO_TASK_TPYE_T InfoTaskType, INFO_DATA_T *InfoMsgData, uint16	 nMeterFilesID);

void currentTimeGet(struct timeval *tm);

void writeMsgDealTime(uint8* addr,struct timeval *msgrecvtv,struct timeval *downsendtv,struct timeval *downrecvtv,uint8 result);
#ifdef __cplusplus
}
#endif

#endif // __CCO_LIB_H__
