/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   日志记录头文件
* @date：    2020-4-21
* @history： 
*********************************************************************
*/
#ifndef __DB_RW_INTERFACE_H__
#define __DB_RW_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

int db_read_nomaldata(uint8 thread_id, NOMAL_OAD_T *ptOAD, uint32 inMaxLen, uint8 *outData, uint32 *outLen);
int db_write_nomaldata(uint8 thread_id, NOMAL_OAD_T *ptOAD, uint8 *pData, uint32 datalen);
int db_write_nomaldata_more(uint8 thread_id, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, uint8 inDataNum);
int db_read_recorddata_start(uint8 thread_id, READ_RECORD_T *ptRecord, uint32 *handle, uint32 *recNum);
int db_read_recorddata_step(uint8 thread_id, uint32 handle, STEP_READ_RECORD_T *ptRecord, RECORD_UNIT_T *stepRecordOut);
int db_read_recorddata_end(uint8 thread_id, uint32 handle);
int db_read_recorddata(uint8 thread_id, READ_RECORD_T *ptRecord, uint32 bufLen, RECORD_UNIT_T *recordOut);
int db_write_recorddata(uint8 thread_id, WRITE_RECORD_T *ptRecord, uint32 *storeNo);
int db_get_recorddata_num(uint8 thread_id, GET_RECORD_NUM_T *pRecord, uint32 *recNum);
int db_get_recorddata_depth(uint8 thread_id, RECORD_OAD_T *pRecord, uint32 *recNum);


#ifdef __cplusplus
}
#endif

#endif //__DB_RW_INTERFACE_H__

