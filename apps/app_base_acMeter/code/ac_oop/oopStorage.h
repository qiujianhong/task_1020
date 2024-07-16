/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：规约解析中记录型数据专用存储接口，依赖libStorage.so,共用句柄
* @date： 2019.12.03
*********************************************************************
*/
#ifndef __COMM_OOP_STORAGE_H__
#define __COMM_OOP_STORAGE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "commClass.h"

#define BRK_SEQ_START      1                     //应用分帧起始序号

/*返回的记录数据内容*/
typedef struct _RECORD_BUF_T
{
    uint32          appSlicingFlag;              //应用分帧标识
    uint32          tmEndSeg;                    //这条数据内容的时间结束点，用于判断是否取完所有记录
    uint32          recNumSeg;                   //记录条数断点（方法9 10）
    uint16          metsn;                       //60120300记录的测量点信息
    OOP_OCTETSTR6_T MAC;                         //附加校验，其实就表地址
    BP_INFO_T       breakInfo;                   //断点信息
    uint32		    bufLen;				         //记录内容长度
    uint8           *recordBuf;                  //记录内容，数据内容按698格式组好
}RECORD_BUF_T;

OOP_DAR_E read_record_response(OAD_INFO_T *pOadInfo, DB_CLIENT fd, const uint8 *inData, uint16 InDataLen, uint16 offset, uint16 csdStart, RECORD_BUF_T *outRecordBuf, RESULT_DATA_T *outData, uint16 outDataMax);

int record_tmn_event_read(DB_CLIENT hUdp, READ_RECORD_T *inReadRecord, RELY_INFO_T *relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_tmn_frozen_read(DB_CLIENT hUdp, READ_RECORD_T *inReadRecord, RELY_INFO_T *relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_event_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord);

int record_frozen_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord);

int record_data_is_valid(READ_RECORD_T inReadRecord, RECORD_UNIT_T unit);

#ifdef __cplusplus
}
#endif

#endif
