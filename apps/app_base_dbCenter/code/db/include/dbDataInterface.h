/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 数据库底层操作API头文件
* @date： 2019-11-9
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

#define CHECK_INFO_LEN              (32)//校验信息最大长度
#define SCREEN_TYPE_ALL             (0)//以存储INDEX查询
#define SCREEN_TYPE_STORAGE_TIME    (1)//以时间查询
#define SCREEN_TYPE_INDEX           (2)//以存储INDEX查询


#define _TRUE                       (1)//XDR TRUE值
#define _FALSE                      (0)//XDR FALSE值

#define _OPTIONSL_TRUE              (1)//XDR OPTIONS TRUE
#define _OPTIONSL_FALSE             (0)//XDR OPTIONS FALSE

//#define MAX_SEQUENCE_NUM            (36)//SEQUENCE,选项最大数量
#define DEL_COUNT                   (16)//自救删除时，每次最大删除条数



/* @brief:单步读记录型数据信息结构体封装  */
typedef struct _step_read_record_info
{
    void* handerNum;  
    uint32 recordNum;  //记录数量
    uint32 indexNum;   //读记录标记
    struct timespec time;       //时间戳
    sint8 **databuf;          //数据区域
}STEP_READ_RECORD_INFO_T;

/* @brief:普通型数据读请求封装  */
typedef struct _read_normale_data_req
{
    uint8 logicalDeviceNum;
    uint16 infoNum;
    uint8  dataNum;
    uint32 dataTag;
}NORMAL_DATA_READ_REQ_T;

/* @brief:普通型数据读应答封装  */
typedef struct _read_normale_data_rsp
{
    uint32 dataTag;
    sint16 dataTypeID;
    uint8  dataNum;
    uint32 bufLen;
    uint8 *outBuf;
    void (*outBufFreeFp)(void * outBuf);
} NORMAL_DATA_READ_RSP_T;

/* @brief:操作读普通型数据  */
sint32 m_read_normal_data(NORMAL_DATA_READ_REQ_T *req, uint16 datatype, NORMAL_DATA_READ_RSP_T *rsp);

/* @brief:普通型数据写请求封装  */
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

/* @brief:操作写普通型数据  */
sint32 m_write_normal_data(NORMAL_DATA_WRITE_RSP_T *req);

/* @brief:记录型数据开始读请求封装  */
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
    uint32 indexNum;      //数据存储序号
}RECORD_DATA_START_READ_REQ_T;

/* @brief:记录型数据开始响应封装  */
typedef struct _read_record_data_rsp
{
    uint32 handleNum;
    uint32 recordNum;
} RECORD_DATA_START_READ_RSP_T;

/* @brief:开始操作读记录型数据  */
sint32 m_start_read_record_data(RECORD_DATA_START_READ_REQ_T *req,RECORD_DATA_START_READ_RSP_T *rsp);

/* @brief:记录型数据单步读请求封装  */
typedef struct _step_read_record_req
{
    uint32 handerNum;
    uint8 checkInfoOpt;
    uint16 checkInfoLen;    
    uint8 checkInfo[CHECK_INFO_LEN];
    //uint32 dataBufLen;
}READ_RECORD_STEP_REQ_T;

/* @brief:记录型数据单步读响应封装  */
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

/* @brief:单步写记录型数据信息结构体事务封装  */
typedef struct _step_write_record_info
{
    void* handerNum;  
    uint32 recordNum;     //记录数量
    uint32 indexNum;      //读记录标记
    time_t time;          //时间戳
    //sint8 **databuf;    //数据区域
}STEP_WRITE_RECORD_INFO_T;

/* @brief:记录型数据单步写请求  */
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

/* @brief:记录型数据单步写内部用信息封装  */
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

/* @brief:记录型数据开始写响应封装  */
typedef struct record_data_start_write_rsp
{
    uint32 handleNum;
} RECORD_DATA_START_WRITE_RSP_T;

/* @brief:操作读记录型数据  */
sint32 m_step_read_record(READ_RECORD_STEP_REQ_T *req, READ_RECORD_STEP_RSP_T *rsp,uint8 *subp, sint32 *outLen, sint16 *itemSum);

/* @brief:记录型数据停止读请求封装  */
typedef struct stop_read_record_data_req
{
    uint32 handleNum;
}RECORD_DATA_STOP_READ_REQ_T;

/* @brief:操作停止读记录型数据  */
sint32 m_stop_read_record_data(RECORD_DATA_STOP_READ_REQ_T *req);


/* @brief:记录型数据写请求封装  */
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

/* @brief:记录型数据写响应封装  */
typedef struct _write_record_data_rsp
{
    uint32 storageIndex;
} RECORD_DATA_WRITE_RSP_T;

/* @brief:操作写记录型数据  */
sint32 m_write_record_data(RECORD_DATA_WRITE_REQ_T *req, RECORD_DATA_WRITE_RSP_T *rsp);

/* @brief:记录型数据更新请求封装  */
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

/* @brief:操作更新记录型数据  */
sint32 m_update_record_data(RECORD_DATA_UPDATE_REQ_T *req);

/* @brief:获取存储记录条数请求封装  */
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

/* @brief:获取存储记录条数响应封装  */
typedef struct _get_storage_num_rsp
{
    uint32 recordNum;
} STORAGE_NUMBER_GET_RSP_T;

/* @brief:操作获取存储记录条数  */
sint32 m_get_storage_num(STORAGE_NUMBER_GET_REQ_T *req, STORAGE_NUMBER_GET_RSP_T * rsp);

/* @brief:获取存储记录深度请求封装  */
typedef struct _get_storage_depth_req
{
    uint8 majorDIOptinal;
    uint32 majorDI;
    uint8 minorDIOptinal;
    uint32 minorDI;
} STORAGE_DEPTH_GET_REQ_T;

/* @brief:初始化请求封装  */
typedef struct _data_init_req
{
    uint8 logicalDeviceNum;
    uint8 optinal;
    uint16 dataTypeID;
} DB_INIT_REQ_T;

/* @brief:获取存储记录深度响应封装  */
typedef struct _get_storage_depth_rsp
{
    uint32 depthNum;
} STORAGE_DEPTH_GET_RSP_T;

/* @brief:操作获取存储记录深度  */
sint32 m_get_depth_num(STORAGE_DEPTH_GET_REQ_T *req, STORAGE_DEPTH_GET_RSP_T *rsp);

/* @brief:操作数据初始化  */
sint32 m_data_init_new(DB_INIT_REQ_T* req);


/* @brief:数据初始化请求封装  */
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

/* @brief:操作指定参数数据初始化  */
sint32 m_data_init_by_parameter(DATA_INIT_REQ_T *req, uint8 needSpaceReclaim);

#ifdef AREA_FUJIAN
/* @brief:数据初始化请求封装  */
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

/* @brief:操作恢复出厂设置  */
sint32 m_factory_Reset_new(uint8 logicalDeviceNum);

/* @brief:数据更新通知请求封装  */
typedef struct _set_subscription_req
{
    uint8   logicalDeviceNum;
    uint32    infoLen;
    uint16  *infoNum;
    uint32    dataTagLen;
    uint32    *dataTag;
}SUBSCRIPTION_SET_REQ_T;

sint32 addr_trans_format(uint8 *pAddr, uint8 addrLen, char *pAddrStr, uint8 strMaxLen);

/* @brief:设置数据更新通知  */
sint32 m_set_subscription(SUBSCRIPTION_SET_REQ_T *req);

/* @brief:数据库模组API接口初始化  */
sint32 m_db_api_module_init(void);

/* @brief: 开始写单步记录型数据 */
sint32 m_start_write_record_data(RECORD_DATA_START_WRITE_RSP_T *rsp);
/* @brief: 单步写记录型数据 */
sint32 m_step_write_record_data(WRITE_RECORD_STEP_REQ_T *req, sint32 *rsp);
/* @brief: 结束单步写记录型数据 */
sint32 m_fin_step_write_record_data(WRITE_RECORD_STEP_REQ_T *req, sint8 *rsp);
/* @brief: Insert普通数据 */
sint32 write_normal_data_mast(sqlite3 *db, sint8 *tab_name, NORMAL_DATA_READ_REQ_T *req, NORMAL_DATA_READ_RSP_T *rsp);
/* @brief: 获取记录No */
sint32 m_get_recoder_no(sqlite3 *db, sint8 *table_name, sint8 *condition);
/* @brief: 获取记录总数 */
sint32 m_get_recoder_count(sqlite3 *db, sint8 *table_name, sint8 *condition);
/* @brief: 对搜寻到的数据进行数据库的写入*/
int find_write_db_by_pridi(WRITE_RECORD_STEP_INFO_T *pInfo, qlist_t *pList);

#ifdef __cplusplus
}
#endif

#endif
