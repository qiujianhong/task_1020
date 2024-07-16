/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __CCO_FILES_H__
#define __CCO_FILES_H__

#include "CcoRedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/**
*********************************************************************
* @brief： 功能描述
* @param[in]： 入参说明
* @param[out]： 出参说明
* @return： 返回值说明
*********************************************************************
*/

extern void RegularWriteFilesToPData(uint8	  nArrIndex, uint16 nMinute);
extern void WriteFilesToPData(uint8      nArrIndex);
extern void ReadFilesFromPData(uint8 nArrIndex);
extern void ReadAmrInfo(uint8    nArrIndex);
extern int IsValidMtrAddr(uint8 *MtrAddr, uint8 AcqUnitFlag);
extern int CheckRepeatMtrAddr(char *MtrAddr, uint8 nArrIndex);
extern uint16 GetPnByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex);
extern MTR_TYPE_T ReadMtrType(uint16 Pn, uint8 nArrIndex);
extern int ReadAcqUnitAddr(uint8 *DesData, uint16 Pn, uint8 nArrIndex);
extern int ReadMtrAddr(uint8 *DesData, uint16 Pn, uint8 nArrIndex);
//extern uint16 GetFilesIdByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex);
extern uint16 GetProtocolByMeterAddr(uint8 *MtrAddr, uint8 nArrIndex);
extern int ReadMtrPrtlMode(uint8 *DesData, uint16 Pn, uint8 nArrIndex);
extern MTR_TYPE_T IsExistAcqUnitAddrInAmrInfo(uint8 *AcqUnitAddr, uint16 *MtrIndex, uint8 nArrIndex);
extern MTR_TYPE_T IsExistCommAddrInAmrInfo(uint8 *CommAddr, uint16 *MtrIndex, uint8 nArrIndex);
extern int IsExistMtrAddrInAmrInfo(uint8 *MtrAddr, uint16 *MtrIndex, uint8 nArrIndex);






#ifdef __cplusplus
}
#endif

#endif // __CCO_FILES_H__