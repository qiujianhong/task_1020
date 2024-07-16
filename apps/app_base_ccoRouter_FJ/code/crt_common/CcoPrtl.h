/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __CCO_PRTL_H__
#define __CCO_PRTL_H__

#include "CcoRedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


/*******************************************************************************
* 全局变量
*******************************************************************************/


/*******************************************************************************
* 宏定义
*******************************************************************************/
#define CHECK_RESULT_SUCCEED			0
#define CHECK_RESULT_FAILUER			-1
#define CHECK_RESULT_CONFIRM			1
#define CHECK_RESULT_DENY			    2

extern uint8   gPrtlLinkLen;                       //规约链路长度，默认2字节
extern uint8   gChannelID;                         //信道标识
extern uint8   g_InerSeq[MAX_DOWN_THREAD_NUM];     //内部帧序列号
extern uint8   g_MetRetLen;                        //预计返回的数据长度

/*******************************************************************************
* 函数声明
*******************************************************************************/

extern int8 CheckFrameFormat(uint16 FrameLen, uint8 *FrameBuff, uint16 *FrameEnd, uint8 LenMode);
extern int8 UnPackFrameData(uint16 FrameLen, uint8 *FrameBuff, AppData_T *AppData);
extern void GetCtrlCode(uint8 *DesData, AppData_T AppData, uint8 nArrIndex);
extern void GetInfoZoneData(uint8 *DesData, AppData_T AppData, uint8 nArrIndex);
extern uint8 ReadTmnAddr(uint8 *RtuAddr, uint8 nArrIndex);
extern void GetAddrZoneA1(uint8 *SrcAddr, uint8 nArrIndex);
extern int GetAddrZoneData(uint8 *DesData, AppData_T AppData, uint8 nArrIndex);
extern void GetDtInfo(uint8 *DT, uint16 Fn);
extern uint16 PackFrameData(AppData_T AppData, uint8 nArrIndex);
extern int PackMeterPrtl(uint8 *PrtlData, MeterPrtlFmt_T MeterData);
extern int GetLocalCommModuleRunningModeInfo(const uint8 *Buff, LOCAL_COMM_MODULE_RUNNING_PATTERN_INFO *lcmrpi,uint16 Len);
extern void Comm_DataRelay(MeterPrtlFmt_T MeterData, uint8 RouterFlag, uint8 *buf, uint32 buf_len, uint8 nArrIndex);
extern int8 CheckMeterProtocol(uint16 FrameLen, uint8 *FrameBuff, MeterPrtlFmt_T *MeterPrtlData, uint16 *offset);
extern int8 UnPackMeterData(uint16 FrameLen, uint8 *FrameBuff, MeterPrtlFmt_T *MeterPrtlData, uint16 *offset);
extern uint16 GetFrame(const uint8 *Buffer, uint16 DataLength, uint16 *OffSet);
extern uint16 CRC16_Get(const uint8* buf, uint32 len);


#ifdef __cplusplus
}
#endif

#endif // __CCO_PRTL_H__
