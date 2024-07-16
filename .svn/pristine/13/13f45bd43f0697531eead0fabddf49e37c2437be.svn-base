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


OOP_DAR_E read_record_response(OAD_INFO_T *pOadInfo, APDU_INFO_T *apduInfo, const uint8 *inData, uint16 InDataLen, uint16 offset, uint16 csdStart, RECORD_BUF_T *outRecordBuf, RESULT_DATA_T *outData, uint16 outDataMax);

/**
*********************************************************************
* @brief：     读任务记录表
* @param[in]： inReadRecord  - 筛选需要的数据
               oopRcsd       - 列选择
               readRecNum    - 读记录数量，：0：读全部记录 1：读最近的一条记录
               maxBufLen     - 最大读取buf长度
* @param[out]：outRecordBuf  - 数据记录
* @return：    =0            - 成功
               <0            - 错误码
*********************************************************************
*/
int record_task_table_read(DB_CLIENT hUdp, const READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_meter_read(DB_CLIENT hUdp, const READ_METER_T inReadRecord, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_searchmeter_res_all_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_normal_scheme_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_event_scheme_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_task_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_acq_monitor_read(DB_CLIENT hUdp, const READ_NOR_BP_T readNorBp, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_acq_rule_read(DB_CLIENT hUdp, const READ_NOR_BP_T readNorBp, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_tmn_event_read(DB_CLIENT hUdp, READ_RECORD_T *inReadRecord, RELY_INFO_T *relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_tmn_frozen_read(DB_CLIENT hUdp, READ_RECORD_T *inReadRecord, RELY_INFO_T *relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

int record_frozen_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord);

int record_stat_read(DB_CLIENT clientfd, RECORD_INFO_T *recordInfo, BUFFER_INFO_T *outRecord);

int record_data_is_valid(READ_RECORD_T inReadRecord, RECORD_UNIT_T unit);

void diag_ycmsg_open(void);

void diag_ycmsg_close(void);

OOP_DAR_E transparent_data_to_fmt698(const IC_OAD_INFO_T *pIcInfo, DATA_CONVERT_T *convert);

int record_transparent_task_table_read(DB_CLIENT hUdp, const READ_RECORD_T inReadRecord, RELY_INFO_T relyInfo, uint32 readRecNum, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);
OOP_DAR_E record_meter_acq_state(DB_CLIENT hUdp, const READ_NOR_BP_T inReadMeter, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);
int record_trans_scheme_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);
int transparent_read_param_get(RECORD_INFO_T *recordInfo, READ_RECORD_T *readRecord, uint32 *stepNum,OOP_Transparent_InfoNum_LIST_T *Transparent_InfoNum);
OOP_DAR_E transparent_unit2fmt698(RECORD_INFO_T *recordInfo, RECORD_UNIT_T *unit, BUFFER_INFO_T *outRecord);
int record_encryption_res_all_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);
int record_chip_node_rec_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);
int record_cfg_8107_rec_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);
uint16 app_get_meter_pn_list_by_tsa(OOP_TSA_T  tsa);
uint32 app_get_meter_port_list_by_tsa(OOP_TSA_T		tsa);

int record_report_scheme_read(DB_CLIENT hUdp, const READ_NOR_BP_T inReadNor, RELY_INFO_T relyInfo, uint32 maxBufLen, RECORD_BUF_T *outRecordBuf);

#ifdef __cplusplus
}
#endif

#endif
