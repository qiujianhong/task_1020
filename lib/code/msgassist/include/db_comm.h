/*
*********************************************************************
* @filename: db_comm.h
* @brief   : 消息通信  DB操作
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2019-12-27    create
*********************************************************************
*/
#ifndef __DB_COMM_H
#define __DB_COMM_H

#include "db_api.h"
#include "debug_comm.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct tagdbReadNomalReq
{
    NOMAL_OAD_T  tOad;
    uint32       maxLen;
}READ_NOMAL_REQ_T;

typedef struct tagdbReadNomalAck
{
    int          code;     //错误码
    uint32       dataLen;
    uint8        data[0];
}READ_NOMAL_ACK_T;

typedef struct tagdbWriteNomalReq
{
    NOMAL_OAD_T  tOad;
    uint32       dataLen;
    uint8        data[0];
}WRITE_NOMAL_REQ_T;

typedef struct tagdbWriteNomalAck
{
    int          code;     //错误码
}WRITE_NOMAL_ACK_T;

typedef struct tagdbReadNomalsReq
{
    NOMAL_OADS_T  tOads;
}READ_NOMALS_REQ_T;

typedef struct tagdbReadRecordStartReq
{
    READ_RECORD_T  tRecord;
}READ_RECORD_START_REQ_T;

typedef struct tagdbReadRecordStartAck
{
    uint32        handle;
    uint32        recNum;
    int           code;     //错误码
}READ_RECORD_START_ACK_T;

typedef struct tagdbReadRecordStepReq
{
    uint32              handle;
    STEP_READ_RECORD_T  tStepRecord;
}READ_RECORD_STEP_REQ_T;

typedef struct tagdbReadRecordStepAck
{
    RECORD_UNIT_T     tRecord;
    uint32            len;
    int               code;     //错误码
}READ_RECORD_STEP_ACK_T;

typedef struct tagdbReadRecordEndReq
{
    uint32            handle;
}READ_RECORD_END_REQ_T;

typedef struct tagdbReadRecordEndAck
{
    int               code;     //错误码
}READ_RECORD_END_ACK_T;

typedef struct tagdbReadRecordReq
{
    READ_RECORD_T        tRecord;
    uint32               maxlen;
}READ_RECORD_REQ_T;

typedef struct tagdbReadRecordAck
{
    RECORD_UNIT_T        tResult;
    int                  code;     //错误码
}READ_RECORD_ACK_T;

typedef struct tagdbWriteRecordReq
{
    WRITE_RECORD_T       tRecord;
}WTITE_RECORD_REQ_T;

typedef struct tagdbWriteRecordAck
{
    uint16               storeNo;
    uint8                res[2];
    int                  code;     //错误码
}WTITE_RECORD_ACK_T;

typedef struct tagdbUpdateRecordReq
{
    UPDATE_RECORD_T      tRecord;
}UPDATE_RECORD_REQ_T;

typedef struct tagdbUpdateRecordAck
{
    int                  code;     //错误码
}UPDATE_RECORD_ACK_T;

typedef struct tagdbGetRecordNumReq
{
    GET_RECORD_NUM_T     tRecord;
}GET_RECORDNUM_REQ_T;

typedef struct tagdbGetRecordReq
{
    RECORD_OAD_T           tRoad;
}GET_RECORD_REQ_T;

typedef struct tagdbGetRecordAck
{
    uint32               recordnum;
    int                  code;     //错误码
}GET_RECORD_ACK_T;

typedef struct tagdbClearDateReq
{
    CLEAR_DATA_T         tData;
}CLEAR_DATA_REQ_T;

typedef struct tagdbCommonDBAck
{
    int                  code;     //错误码
}COMMON_DB_ACK_T;

typedef enum
{
    DBMSG_SUCCESS     = 0,
    DBMSG_ERR_INPUT   = 0x2000,
    DBMSG_ERR_MALLOC,
    DBMSG_ERR_TOO_LONG,
    DBMSG_ERR_SEND_FAILED,
    DBMSG_ERR_RECV_FAILED,
    DBMSG_ERR_OUT_TIME,
    DBMSG_ERR_REACH_MAX_CLIENT,
    DBMSG_ERR_LOCK_FAILED,
    DBMSG_ERR_UNKNOW,
}DBMSG_ERRCODE_E;


typedef int (*DBTaskMsgFun)(uint8*, uint32, void *);
typedef struct tagDBTaksMsgProc
{
    uint32             msgid;     /* 消息ID */
    DBTaskMsgFun       pMsgFunc;  /* 线程入口函数 */
}DB_TASKMSG_PROC_T;

typedef struct tagdbClientCache
{
    uint32          taskid;    //客户端id
    pthread_mutex_t lock;      //客户端操作锁
    pthread_cond_t  notify;    //线程模式条件变量
    uint16          msgindex;  //消息index
    uint8           taskindex; //内部消息index
    uint8           bwait;     //是否处于等待接收返回消息状态
    uint32          msgid;     //内部Task消息id
    uint32          ackbuflen; //返回消息缓存
    int             dbret;     //当前没用到
    uint32          datalen;   //数据长度
    uint8          *data;      //数据中心内的消息体内容
}DBCLIENT_CACHE_T;

typedef int (*DBMsgFun)(DB_AGENT, MESSAGE_INFO_T *, DBCLIENT_CACHE_T *);
typedef struct tagDBMsgProc
{
    uint32             msgid;     /* 消息ID */
    uint32             iop;
    DBMsgFun           pMsgFunc;  /* 线程入口函数 */
}DB_MSG_PROC_T;


uint16 db_get_len_offset(uint8 *indata,uint8 *bytes);
uint8 db_set_len_offset(uint16 eleLen, uint8 *indata);
void *db_memcpy_r(void *dest, const void *src, int count);
uint8 db_get_agent_type();
DBMSG_SEND_FUN db_get_send_func();


int db_clientinfo_init(uint8 clientnum);
void db_clientinfo_free();
DBCLIENT_CACHE_T *db_get_clientinfo(uint32 taskid, uint8 *clientid);
DBCLIENT_CACHE_T *db_get_clientinfo_byid(DB_CLIENT clientid);
void db_clear_clientinfo(DBCLIENT_CACHE_T *ptInfo);

int send_read_nomal_data(DBMSG_SEND_FUN func, uint16 label, NOMAL_OAD_T *nomalOAD, uint16 *index);
int send_write_nomal_data(DBMSG_SEND_FUN func, uint16 label, NOMAL_OAD_T *nomalOAD, uint8 *pData, 
                          uint32 dataLen, uint16 *index);
int send_read_nomal_datas(DBMSG_SEND_FUN func, uint16 label, NOMAL_OADS_T *nomalOADs, uint16 *index);
int send_write_nomal_datas(DBMSG_SEND_FUN func, uint16 label, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, 
                           uint8 inDataNum, uint16 *index);
int send_read_record_start(DBMSG_SEND_FUN func, uint16 label, READ_RECORD_T *startRecord, uint16 *index);
int send_read_record_step(DBMSG_SEND_FUN func, uint16 label, uint32 handle, STEP_READ_RECORD_T *stepRecord, uint16 *index);
int send_read_record_end(DBMSG_SEND_FUN func, uint16 label, uint32 handle, uint16 *index);
int send_write_record(DBMSG_SEND_FUN func, uint16 label, WRITE_RECORD_T *record, uint16 *index);
int send_write_records(DBMSG_SEND_FUN func, uint16 label, WRITE_RECORDS_T *record, uint16 *index);
int send_update_record(DBMSG_SEND_FUN func, uint16 label, UPDATE_RECORD_T *pData, uint16 *index);
int send_get_record_num(DBMSG_SEND_FUN func, uint16 label, GET_RECORD_NUM_T *pRecord, uint16 *index);
int send_get_record_depth(DBMSG_SEND_FUN func, uint16 label, RECORD_OAD_T *pRoad, uint16 *index);
int send_clear_data(DBMSG_SEND_FUN func, uint16 label, CLEAR_DATA_T *ptData, uint16 *index);
int send_clear_oad_data(DBMSG_SEND_FUN func, uint16 label, CLEAR_DATA_T *ptData, uint16 *index);
int send_recover_default(DBMSG_SEND_FUN func, uint16 label, uint8 logicId, uint16 *index);


#ifdef __cplusplus
}
#endif

#endif

