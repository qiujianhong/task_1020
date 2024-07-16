/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief����Լ�����м�¼������ר�ô洢�ӿڣ�����libStorage.so,���þ��
* @date�� 2019.12.03
*********************************************************************
*/
#ifndef __COMM_OOP_STORAGE_H__
#define __COMM_OOP_STORAGE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "commClass.h"

#define BRK_SEQ_START      1                     //Ӧ�÷�֡��ʼ���

/*���صļ�¼��������*/
typedef struct _RECORD_BUF_T
{
    uint32          appSlicingFlag;              //Ӧ�÷�֡��ʶ
    uint32          tmEndSeg;                    //�����������ݵ�ʱ������㣬�����ж��Ƿ�ȡ�����м�¼
    uint32          recNumSeg;                   //��¼�����ϵ㣨����9 10��
    uint16          metsn;                       //60120300��¼�Ĳ�������Ϣ
    OOP_OCTETSTR6_T MAC;                         //����У�飬��ʵ�ͱ��ַ
    BP_INFO_T       breakInfo;                   //�ϵ���Ϣ
    uint32		    bufLen;				         //��¼���ݳ���
    uint8           *recordBuf;                  //��¼���ݣ��������ݰ�698��ʽ���
}RECORD_BUF_T;


OOP_DAR_E read_record_response(OAD_INFO_T *pOadInfo, APDU_INFO_T *apduInfo, const uint8 *inData, uint16 InDataLen, uint16 offset, uint16 csdStart, RECORD_BUF_T *outRecordBuf, RESULT_DATA_T *outData, uint16 outDataMax);

/**
*********************************************************************
* @brief��     �������¼��
* @param[in]�� inReadRecord  - ɸѡ��Ҫ������
               oopRcsd       - ��ѡ��
               readRecNum    - ����¼��������0����ȫ����¼ 1���������һ����¼
               maxBufLen     - ����ȡbuf����
* @param[out]��outRecordBuf  - ���ݼ�¼
* @return��    =0            - �ɹ�
               <0            - ������
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
