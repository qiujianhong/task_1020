/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ���ݿ�ײ����APIͷ�ļ�
* @date�� 2019-11-9
*********************************************************************
*/

#ifndef __DB_DATA_INTERFACE_H__
#define __DB_DATA_INTERFACE_H__

#include <time.h>
//#include "dbApiModule.h"
#include "baseType.h"
#include "sqlite3.h"
#include "qlibc.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CHECK_INFO_LEN              (32)//У����Ϣ��󳤶�
#define SCREEN_TYPE_ALL             (0)//�Դ洢INDEX��ѯ
#define SCREEN_TYPE_STORAGE_TIME    (1)//��ʱ���ѯ
#define SCREEN_TYPE_INDEX           (2)//�Դ洢INDEX��ѯ


#define _TRUE                       (1)//XDR TRUEֵ
#define _FALSE                      (0)//XDR FALSEֵ

#define _OPTIONSL_TRUE              (1)//XDR OPTIONS TRUE
#define _OPTIONSL_FALSE             (0)//XDR OPTIONS FALSE

//#define MAX_SEQUENCE_NUM            (36)//SEQUENCE,ѡ���������
#define DEL_COUNT                   (16)//�Ծ�ɾ��ʱ��ÿ�����ɾ������



/* @brief:��������¼��������Ϣ�ṹ���װ  */
typedef struct _step_read_record_info
{
    void* handerNum;  
    uint32 recordNum;  //��¼����
    uint32 indexNum;   //����¼���
    struct timespec time;       //ʱ���
    sint8 **databuf;          //��������
}STEP_READ_RECORD_INFO_T;

/* @brief:��ͨ�����ݶ������װ  */
typedef struct _read_normale_data_req
{
    uint8 logicalDeviceNum;
    uint16 infoNum;
    uint8  dataNum;
    uint32 dataTag;
}NORMAL_DATA_READ_REQ_T;

/* @brief:��ͨ�����ݶ�Ӧ���װ  */
typedef struct _read_normale_data_rsp
{
    uint32 dataTag;
    sint16 dataTypeID;
    uint8  dataNum;
    uint32 bufLen;
    uint8 *outBuf;
    void (*outBufFreeFp)(void * outBuf);
} NORMAL_DATA_READ_RSP_T;

/* @brief:��������ͨ������  */
sint32 m_read_normal_data(NORMAL_DATA_READ_REQ_T *req, uint16 datatype, NORMAL_DATA_READ_RSP_T *rsp);

/* @brief:��ͨ������д�����װ  */
typedef struct _write_normale_data_req
{
    uint8 logicalDeviceNum;
    uint16 infoNum;
    uint8  dataNum;
    uint32 dataTag;
    sint16 dataTypeID;
    uint32 outBufLen;
    uint8 *outBuf;
} NORMAL_DATA_WRITE_RSP_T;

/* @brief:����д��ͨ������  */
sint32 m_write_normal_data(NORMAL_DATA_WRITE_RSP_T *req);

/* @brief:��¼�����ݿ�ʼ�������װ  */
typedef struct _read_record_data_req
{
    uint8 logicalDeviceNum;
    uint16 infoNum;
    uint8 queryTimeType;
    uint32 startTime;
    uint32 endTime;
    uint8 sortType;
    uint8 majorDIOptinal;    
    uint32 majorDI;
    uint32 minorDINum;
    uint32 *minorDIs;
	
#ifdef AREA_FUJIAN
	uint8 addrLen;
	uint8 *pAddr;
#endif
    uint32 indexNum;      //���ݴ洢���
}RECORD_DATA_START_READ_REQ_T;

/* @brief:��¼�����ݿ�ʼ��Ӧ��װ  */
typedef struct _read_record_data_rsp
{
    uint32 handleNum;
    uint32 recordNum;
} RECORD_DATA_START_READ_RSP_T;

/* @brief:��ʼ��������¼������  */
sint32 m_start_read_record_data(RECORD_DATA_START_READ_REQ_T *req,RECORD_DATA_START_READ_RSP_T *rsp);

/* @brief:��¼�����ݵ����������װ  */
typedef struct _step_read_record_req
{
    uint32 handerNum;
    uint8 checkInfoOpt;
    uint16 checkInfoLen;    
    uint8 checkInfo[CHECK_INFO_LEN];
    //uint32 dataBufLen;
}READ_RECORD_STEP_REQ_T;

/* @brief:��¼�����ݵ�������Ӧ��װ  */
typedef struct _step_read_record_Rsp
{
    uint32 indexNum;
    sint8 flag;
    sint8 timeFlag;
    uint32 startTime;
    uint32 endTime;
    uint32 storageTime;
    uint16 infoNum;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
    uint16 checkInfoLen;    
    uint8 checkInfo[CHECK_INFO_LEN];
    uint32 outBufLen;
    uint8 *outBuf;
    void (*outBufFreeFp)(uint8 *outBuf);
} READ_RECORD_STEP_RSP_T;

/* @brief:����д��¼��������Ϣ�ṹ�������װ  */
typedef struct _step_write_record_info
{
    void* handerNum;  
    uint32 recordNum;     //��¼����
    uint32 indexNum;      //����¼���
    time_t time;          //ʱ���
    //sint8 **databuf;    //��������
}STEP_WRITE_RECORD_INFO_T;

/* @brief:��¼�����ݵ���д����  */
typedef struct _step_write_record_Req
{
    uint32 handerNum;
    uint32 startTime;
    uint32 endTime;
    uint32 storageTime;
    uint8  logi_devNo;
    uint8  flag;   
    uint16 infoNum;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
    uint16 checkInfoLen;
    uint8 checkInfo[CHECK_INFO_LEN];
    uint32 outBufLen;
    uint8 *outBuf;
    void (*outBufFreeFp)(uint8 *outBuf);
} WRITE_RECORD_STEP_REQ_T;

/* @brief:��¼�����ݵ���д�ڲ�����Ϣ��װ  */
typedef struct _step_write_recordInfo
{
    //uint32 handerNum;
    uint32 startTime;
    uint32 endTime;
    uint32 storageTime;
    uint8  logi_devNo;
    uint8  flag;   
    uint16 infoNum;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
    uint16 checkInfoLen;
    uint8 checkInfo[CHECK_INFO_LEN];
    uint32 cBufLen;
    uint8  cBuf[0];
} WRITE_RECORD_STEP_INFO_T;

/* @brief:��¼�����ݿ�ʼд��Ӧ��װ  */
typedef struct record_data_start_write_rsp
{
    uint32 handleNum;
} RECORD_DATA_START_WRITE_RSP_T;

/* @brief:��������¼������  */
sint32 m_step_read_record(READ_RECORD_STEP_REQ_T *req, READ_RECORD_STEP_RSP_T *rsp,uint8 *subp, sint32 *outLen, sint16 *itemSum);

/* @brief:��¼������ֹͣ�������װ  */
typedef struct stop_read_record_data_req
{
    uint32 handleNum;
}RECORD_DATA_STOP_READ_REQ_T;

/* @brief:����ֹͣ����¼������  */
sint32 m_stop_read_record_data(RECORD_DATA_STOP_READ_REQ_T *req);


/* @brief:��¼������д�����װ  */
typedef struct _write_record_data_req
{
    uint8 logicalDeviceNum;
    uint8 majorDIOptinal;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
    sint16 dataTypeID;
    uint32 startTime;
    uint32 endTime;
    uint32 storageTime;

    uint16 infoNum;
    uint16 checkInfoLen;
    uint8 checkInfo[CHECK_INFO_LEN];
    uint32 outBufLen;
    uint8 *outBuf;
} RECORD_DATA_WRITE_REQ_T;

/* @brief:��¼������д��Ӧ��װ  */
typedef struct _write_record_data_rsp
{
    uint32 storageIndex;
} RECORD_DATA_WRITE_RSP_T;

/* @brief:����д��¼������  */
sint32 m_write_record_data(RECORD_DATA_WRITE_REQ_T *req, RECORD_DATA_WRITE_RSP_T *rsp);

/* @brief:��¼�����ݸ��������װ  */
typedef struct _update_record_data_req
{
    uint8 logicalDeviceNum;
    uint8 majorDIOptinal;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
    uint8 screenType;
    uint32 storageIndex;
    uint32 storageTime;

    uint16 infoNum;
    uint16 checkInfoLen;
    uint8 checkInfo[CHECK_INFO_LEN];
    uint32 outBufLen;
    uint8 *outBuf;
} RECORD_DATA_UPDATE_REQ_T;

/* @brief:�������¼�¼������  */
sint32 m_update_record_data(RECORD_DATA_UPDATE_REQ_T *req);

/* @brief:��ȡ�洢��¼���������װ  */
typedef struct _get_storage_num_req
{
    uint8 logicalDeviceNum;
    uint16 infoNum;
    uint8 majorDIOptinal;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
    uint8  screenType;
    uint32 storageStartTime;
    uint32 storageEndTime;
}STORAGE_NUMBER_GET_REQ_T;

/* @brief:��ȡ�洢��¼������Ӧ��װ  */
typedef struct _get_storage_num_rsp
{
    uint32 recordNum;
} STORAGE_NUMBER_GET_RSP_T;

/* @brief:������ȡ�洢��¼����  */
sint32 m_get_storage_num(STORAGE_NUMBER_GET_REQ_T *req, STORAGE_NUMBER_GET_RSP_T * rsp);

/* @brief:��ȡ�洢��¼��������װ  */
typedef struct _get_storage_depth_req
{
    uint8 majorDIOptinal;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
} STORAGE_DEPTH_GET_REQ_T;

/* @brief:��ʼ�������װ  */
typedef struct _data_init_req
{
    uint8 logicalDeviceNum;
    uint8 optinal;
    uint16 dataTypeID;
} DB_INIT_REQ_T;

/* @brief:��ȡ�洢��¼�����Ӧ��װ  */
typedef struct _get_storage_depth_rsp
{
    uint32 depthNum;
} STORAGE_DEPTH_GET_RSP_T;

/* @brief:������ȡ�洢��¼���  */
sint32 m_get_depth_num(STORAGE_DEPTH_GET_REQ_T *req, STORAGE_DEPTH_GET_RSP_T *rsp);

/* @brief:�������ݳ�ʼ��  */
sint32 m_data_init_new(DB_INIT_REQ_T* req);


/* @brief:���ݳ�ʼ�������װ  */
typedef struct _date_init_by_parameter_req
{
    uint8 logicalDeviceNum;
    uint16 infoNum;
    uint8  choice;
    uint16 dataTypeID;
    uint8 majorDIOptinal;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
}DATA_INIT_REQ_T;

/* @brief:����ָ���������ݳ�ʼ��  */
sint32 m_data_init_by_parameter(DATA_INIT_REQ_T *req, uint8 needSpaceReclaim);

#ifdef AREA_FUJIAN
/* @brief:���ݳ�ʼ�������װ  */
typedef struct _date_init_by_parameter_req_fj
{
    uint8 logicalDeviceNum;
    uint16 infoNum;
    uint8 majorDIOptinal;
    uint32 majorDI;
    uint32 minorDI;
	uint8 queryTimeType;
	uint32 storageStartTime;
    uint32 storageEndTime;
}DATA_INIT_REQ_T_FJ;

sint32 m_data_init_by_parameter_fj(DATA_INIT_REQ_T_FJ *req);

sint32 m_start_read_record_data_fj(RECORD_DATA_START_READ_REQ_T *req, RECORD_DATA_START_READ_RSP_T *rsp);

#endif

/* @brief:�����ָ���������  */
sint32 m_factory_Reset_new(uint8 logicalDeviceNum);

/* @brief:���ݸ���֪ͨ�����װ  */
typedef struct _set_subscription_req
{
    uint8   logicalDeviceNum;
    uint32    infoLen;
    uint16  *infoNum;
    uint32    dataTagLen;
    uint32    *dataTag;
}SUBSCRIPTION_SET_REQ_T;

sint32 addr_trans_format(uint8 *pAddr, uint8 addrLen, char *pAddrStr, uint8 strMaxLen);

/* @brief:�������ݸ���֪ͨ  */
sint32 m_set_subscription(SUBSCRIPTION_SET_REQ_T *req);

/* @brief:���ݿ�ģ��API�ӿڳ�ʼ��  */
sint32 m_db_api_module_init(void);

/* @brief: ��ʼд������¼������ */
sint32 m_start_write_record_data(RECORD_DATA_START_WRITE_RSP_T *rsp);
/* @brief: ����д��¼������ */
sint32 m_step_write_record_data(WRITE_RECORD_STEP_REQ_T *req, sint32 *rsp);
/* @brief: ��������д��¼������ */
sint32 m_fin_step_write_record_data(WRITE_RECORD_STEP_REQ_T *req, sint8 *rsp);
/* @brief: Insert��ͨ���� */
sint32 write_normal_data_mast(sqlite3 *db, sint8 *tab_name, NORMAL_DATA_READ_REQ_T *req, NORMAL_DATA_READ_RSP_T *rsp);
/* @brief: ��ȡ��¼No */
sint32 m_get_recoder_no(sqlite3 *db, sint8 *table_name, sint8 *condition);
/* @brief: ��ȡ��¼���� */
sint32 m_get_recoder_count(sqlite3 *db, sint8 *table_name, sint8 *condition);
/* @brief: ����Ѱ�������ݽ������ݿ��д��*/
int find_write_db_by_pridi(WRITE_RECORD_STEP_INFO_T *pInfo, qlist_t *pList);

#ifdef __cplusplus
}
#endif

#endif
