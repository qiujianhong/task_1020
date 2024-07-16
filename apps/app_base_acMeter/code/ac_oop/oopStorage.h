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
