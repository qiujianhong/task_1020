/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief��   ��Ϣͨ��  DB����API�ӿ�
* @date��    2019/12/27
* @history�� 
*********************************************************************
*/
#ifndef    __DB_API_H
#define __DB_API_H

#include "message.h"
#include "db_struct.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef int  DB_AGENT;
typedef int  DB_CLIENT;

#define AGENT_TYPE_PROCESS       0     //���̼�����������Ĵ���
#define AGENT_TYPE_THREAD        1     //�̼߳�����������Ĵ���

#define CLIENT_SUPPORT_MAX_NUM   256

#define DB_AGENT_UNKONW_MSG      0x3333
#define DB_AGENT_DROP_MSG        0x33cc

/**
*********************************************************************
* @brief��     �������Ķ��ⷢ�ͽӿ� 
* @param[in]�� uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]��uint16*         index
* @return��    0-�ɹ�/������
*********************************************************************
*/
typedef int (*DBMSG_SEND_FUN)(uint16, uint16, uint8*, uint16 , uint16*);

/**
*********************************************************************
* @brief��     ����db������ʼ�� һ��APPֻ�ܴ���һ�� 
* @param[in]�� uint8 type          �������� AGENT_TYPE_PROCESS AGENT_TYPE_THREAD
               uint32 taskid       �ڲ�ͨ���õ�taskid ��16λ��Ч
               DBMSG_SEND_FUN func ������Ϣ���ͻص�����
               uint8 clientnum     client�������
* @param[out]��
* @return��    DB_AGENT            ������ ����0��Ч
*********************************************************************
*/
DB_AGENT db_agent_create(uint8 type, uint32 taskid, DBMSG_SEND_FUN func, uint8 clientnum);


/**
*********************************************************************
* @brief��     db����ر�
* @param[in]�� DB_AGENT fd          ������ 
* @param[out]��
* @return��
*********************************************************************
*/
void db_agent_close(DB_AGENT fd);

/**
*********************************************************************
* @brief��     db��Ϣ����
* @param[in]�� DB_AGENT fd          ������
               MESSAGE_INFO_T *pMsg ��Ϣ�ṹ��
* @param[out]��
* @return��    0-�ɹ�/������(����DB_AGENT_UNKONW_MSG, ��������Ϣ��������)
*********************************************************************
*/
int db_agent_msg_entry(DB_AGENT fd, MESSAGE_INFO_T *pMsg);

/**
*********************************************************************
* @brief��     ����db�ͻ��˲���ʼ�� һ���߳�ֻ�ܴ���һ�� 
* @param[in]�� uint32 ataskid      DB�����taskid ��16λ��Ч
               uint32 ctaskid      DB�ͻ���ͨ���õ�taskid ��16λ��Ч
* @param[out]��
* @return��    DB_CLIENT           �ͻ��˾�� ����0��Ч
*********************************************************************
*/
DB_CLIENT db_client_create(uint32 ataskid, uint32 ctaskid);

/**
*********************************************************************
* @brief��     db�ͻ��˹ر�
* @param[in]�� DB_AGENT fd          ������ 
* @param[out]��
* @return��
*********************************************************************
*/
void db_client_close(DB_CLIENT fd);

/**
*********************************************************************
* @brief��     db������ȡ��ͨ����(��������) 
* @param[in]�� DB_CLIENT fd          DB�ͻ��˾��
               NOMAL_OAD_T *ptOAD    OAD
               uint32 inMaxLen       ��󻺴�����
* @param[out]��uint8 *outData        ����
               uint32 *outLen        ���ݳ���
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_read_nomal(DB_CLIENT fd, NOMAL_OAD_T *ptOAD, uint32 inMaxLen, uint8 *outData, uint32 *outLen);

/**
*********************************************************************
* @brief��     db����д��ͨ����(��������)
* @param[in]�� DB_CLIENT fd          DB�ͻ��˾��
               NOMAL_OAD_T *ptOAD    OAD
               uint8 *pData          ����
               uint32 datalen        ���ݳ���
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_write_nomal(DB_CLIENT fd, NOMAL_OAD_T *ptOAD, uint8 *pData, uint32 datalen);

/**
*********************************************************************
* @brief��     db������ȡ��ͨ����(��������)
* @param[in]�� DB_CLIENT fd               DB�ͻ��˾��
               NOMAL_OADS_T *ptOADs       OAD�б�
               uint8 oadMaxNum            ��󻺴�����
* @param[out]��NOMAL_OAD_DATE_T *outDatas ��ȡ����ͨ���ݻ���
               uint8 *dataNum             ��ͨ��������
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_read_nomal_more(DB_CLIENT fd, NOMAL_OADS_T *ptOADs, uint8 oadMaxNum, NOMAL_OAD_DATE_T *outDatas, uint8 *dataNum);

/**
*********************************************************************
* @brief��     db����д��ͨ����(��������)
* @param[in]�� DB_CLIENT fd              DB�ͻ��˾��
               uint8 logic               �߼��豸��
               uint16 info               ��Ϣ���
               NOMAL_OAD_DATE_T *inDatas ��������
               uint8 inDataNum           ��������
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_write_nomal_more(DB_CLIENT fd, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, uint8 inDataNum);

/**
*********************************************************************
* @brief��     db������ʼ����¼������
* @param[in]�� DB_CLIENT fd             DB�ͻ��˾��
               READ_RECORD_T *ptRecord  ��¼����      ��OADΪ0��ʾ�Ǹ���������
* @param[out]��uint32 *handle           �������ľ��
               uint32 *recNum           ��¼����
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_read_record_start(DB_CLIENT fd, READ_RECORD_T *ptRecord, uint32 *handle, uint32 *recNum);

/**
*********************************************************************
* @brief��     db������������¼������
* @param[in]�� DB_CLIENT fd                 DB�ͻ��˾��
               uint32 handle                �������ľ��
               STEP_READ_RECORD_T *ptRecord ����        nNUMΪ0��ʾû�и���У��
* @param[out]��RECORD_UNIT_T *stepRecordOut ��������
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_read_record_step(DB_CLIENT fd, uint32 handle, STEP_READ_RECORD_T *ptRecord, RECORD_UNIT_T *stepRecordOut);

/**
*********************************************************************
* @brief��     db����������¼�����ݶ�ȡ
* @param[in]�� DB_CLIENT fd            DB�ͻ��˾��
               uint32 handle           �������ľ��
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_read_record_end(DB_CLIENT fd, uint32 handle);

/**
*********************************************************************
* @brief��     db�����������һ����¼
* @param[in]�� DB_CLIENT fd             DB�ͻ��˾��
               READ_RECORD_T *ptRecord  ��¼����
               uint32 bufLen            ����ȡbuf����
* @param[out]��RECORD_UNIT_T *recordOut ���ݼ�¼
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_read_record(DB_CLIENT fd, READ_RECORD_T *ptRecord, uint32 bufLen, RECORD_UNIT_T *recordOut);

/**
*********************************************************************
* @brief��     db����д��¼������
* @param[in]�� DB_CLIENT fd             DB�ͻ��˾��
               WRITE_RECORD_T *ptRecord ��¼    ��OADΪ0��ʾ�Ǹ���������
* @param[out]��uint32 *storeNo          �洢���
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_write_record(DB_CLIENT fd, WRITE_RECORD_T *ptRecord, uint32 *storeNo);
int db_write_records(DB_CLIENT fd, WRITE_RECORDS_T *ptRecord, uint32 *storeNo);


/**
*********************************************************************
* @brief��     db����д��¼������(������¼)
* @param[in]�� DB_CLIENT fd              DB�ͻ��˾��
               WRITE_RECORDS_T *ptRecord ��¼    ��OADΪ0��ʾ�Ǹ���������
* @param[out]��uint32 *storeNo           �洢���
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_write_records(DB_CLIENT fd, WRITE_RECORDS_T *ptRecord, uint32 *storeNo);

/**
*********************************************************************
* @brief��     db�������¼�¼����
* @param[in]�� DB_CLIENT fd              DB�ͻ��˾��
               UPDATE_RECORD_T *ptRecord ����   ��OADΪ0��ʾ�Ǹ���������
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_update_record(DB_CLIENT fd, UPDATE_RECORD_T *ptRecord);

/**
*********************************************************************
* @brief��     db������ȡ�洢��¼����
* @param[in]�� DB_CLIENT fd              DB�ͻ��˾��
               GET_RECORD_NUM_T *pRecord ����
* @param[out]��uint32 *recNum            ����
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_get_record_num(DB_CLIENT fd, GET_RECORD_NUM_T *pRecord, uint32 *recNum);

/**
*********************************************************************
* @brief��     db������ȡ�洢��¼���
* @param[in]�� DB_CLIENT fd              DB�ͻ��˾��
               OOP_ROAD_T *ptRroad       ����
* @param[out]��uint32 *depthNum          ���
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_get_record_depth(DB_CLIENT fd, RECORD_OAD_T *ptRroad, uint32 *depthNum);

/**
*********************************************************************
* @brief��     db�������ݳ�ʼ��
* @param[in]�� DB_CLIENT fd         DB�ͻ��˾��
               CLEAR_DATA_T *ptData ���ѡ��
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_clear_data(DB_CLIENT fd, CLEAR_DATA_T *ptData);

/**
*********************************************************************
* @brief��     db����ָ�����ݳ�ʼ��
* @param[in]�� DB_CLIENT fd           DB�ͻ��˾��
               CLEAR_DATA_T *ptData   ���ѡ��
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_clear_oad_data(DB_CLIENT fd, CLEAR_DATA_T *ptData);

/**
*********************************************************************
* @brief��     db�����ָ�����
* @param[in]�� DB_CLIENT fd       DB�ͻ��˾��
               uint8 logicId      �߼��豸��
* @param[out]��
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_recover_default(DB_CLIENT fd, uint8 logicId);

/**
*********************************************************************
* @brief��     ��ͨ����תBase64��ʽ
* @param[in]�� uint8 *pData         ����
               uint32 datalen       ���ݳ���
* @param[out]��
* @return��    base64���
*********************************************************************
*/
char *db_data_to_base64(uint8 *pData, uint32 datalen);

/**
*********************************************************************
* @brief��     Base64��ʽר��ͨ����
* @param[in]�� uint8 *pBase         Base64����
               uint32 buflen        �����������С
* @param[out]��uint8 *pBuf          ����������
               uint32 *datalen      ������ʵ�ʳ���
* @return��    0-�ɹ�/������
*********************************************************************
*/
int db_base64_to_data(char *pBase, uint8 *pBuf, uint32 buflen, uint32 *datalen);

#ifdef __cplusplus
}
#endif

#endif

