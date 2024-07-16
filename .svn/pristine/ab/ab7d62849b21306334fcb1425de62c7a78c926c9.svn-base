/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   消息通信  DB操作API接口
* @date：    2019/12/27
* @history： 
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

#define AGENT_TYPE_PROCESS       0     //进程级别的数据中心代理
#define AGENT_TYPE_THREAD        1     //线程级别的数据中心代理

#define CLIENT_SUPPORT_MAX_NUM   256

#define DB_AGENT_UNKONW_MSG      0x3333
#define DB_AGENT_DROP_MSG        0x33cc

/**
*********************************************************************
* @brief：     数据中心对外发送接口 
* @param[in]： uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：uint16*         index
* @return：    0-成功/错误码
*********************************************************************
*/
typedef int (*DBMSG_SEND_FUN)(uint16, uint16, uint8*, uint16 , uint16*);

/**
*********************************************************************
* @brief：     创建db代理并初始化 一个APP只能存在一个 
* @param[in]： uint8 type          代理类型 AGENT_TYPE_PROCESS AGENT_TYPE_THREAD
               uint32 taskid       内部通信用的taskid 低16位有效
               DBMSG_SEND_FUN func 对外消息发送回调函数
               uint8 clientnum     client最大容量
* @param[out]：
* @return：    DB_AGENT            代理句柄 大于0有效
*********************************************************************
*/
DB_AGENT db_agent_create(uint8 type, uint32 taskid, DBMSG_SEND_FUN func, uint8 clientnum);


/**
*********************************************************************
* @brief：     db代理关闭
* @param[in]： DB_AGENT fd          代理句柄 
* @param[out]：
* @return：
*********************************************************************
*/
void db_agent_close(DB_AGENT fd);

/**
*********************************************************************
* @brief：     db消息解析
* @param[in]： DB_AGENT fd          代理句柄
               MESSAGE_INFO_T *pMsg 消息结构体
* @param[out]：
* @return：    0-成功/错误码(返回DB_AGENT_UNKONW_MSG, 由其他消息函数处理)
*********************************************************************
*/
int db_agent_msg_entry(DB_AGENT fd, MESSAGE_INFO_T *pMsg);

/**
*********************************************************************
* @brief：     创建db客户端并初始化 一个线程只能存在一个 
* @param[in]： uint32 ataskid      DB代理的taskid 低16位有效
               uint32 ctaskid      DB客户端通信用的taskid 低16位有效
* @param[out]：
* @return：    DB_CLIENT           客户端句柄 大于0有效
*********************************************************************
*/
DB_CLIENT db_client_create(uint32 ataskid, uint32 ctaskid);

/**
*********************************************************************
* @brief：     db客户端关闭
* @param[in]： DB_AGENT fd          代理句柄 
* @param[out]：
* @return：
*********************************************************************
*/
void db_client_close(DB_CLIENT fd);

/**
*********************************************************************
* @brief：     db操作读取普通数据(单条数据) 
* @param[in]： DB_CLIENT fd          DB客户端句柄
               NOMAL_OAD_T *ptOAD    OAD
               uint32 inMaxLen       最大缓存数量
* @param[out]：uint8 *outData        缓存
               uint32 *outLen        数据长度
* @return：    0-成功/错误码
*********************************************************************
*/
int db_read_nomal(DB_CLIENT fd, NOMAL_OAD_T *ptOAD, uint32 inMaxLen, uint8 *outData, uint32 *outLen);

/**
*********************************************************************
* @brief：     db操作写普通数据(单条数据)
* @param[in]： DB_CLIENT fd          DB客户端句柄
               NOMAL_OAD_T *ptOAD    OAD
               uint8 *pData          数据
               uint32 datalen        数据长度
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int db_write_nomal(DB_CLIENT fd, NOMAL_OAD_T *ptOAD, uint8 *pData, uint32 datalen);

/**
*********************************************************************
* @brief：     db操作读取普通数据(多条数据)
* @param[in]： DB_CLIENT fd               DB客户端句柄
               NOMAL_OADS_T *ptOADs       OAD列表
               uint8 oadMaxNum            最大缓存数量
* @param[out]：NOMAL_OAD_DATE_T *outDatas 读取的普通数据缓存
               uint8 *dataNum             普通数据数量
* @return：    0-成功/错误码
*********************************************************************
*/
int db_read_nomal_more(DB_CLIENT fd, NOMAL_OADS_T *ptOADs, uint8 oadMaxNum, NOMAL_OAD_DATE_T *outDatas, uint8 *dataNum);

/**
*********************************************************************
* @brief：     db操作写普通数据(多条数据)
* @param[in]： DB_CLIENT fd              DB客户端句柄
               uint8 logic               逻辑设备号
               uint16 info               信息点号
               NOMAL_OAD_DATE_T *inDatas 多条数据
               uint8 inDataNum           数据数量
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int db_write_nomal_more(DB_CLIENT fd, uint8 logic, uint16 info, NOMAL_OAD_DATE_T *inDatas, uint8 inDataNum);

/**
*********************************************************************
* @brief：     db操作开始读记录型数据
* @param[in]： DB_CLIENT fd             DB客户端句柄
               READ_RECORD_T *ptRecord  记录条件      主OAD为0表示非复合型数据
* @param[out]：uint32 *handle           数据中心句柄
               uint32 *recNum           记录条数
* @return：    0-成功/错误码
*********************************************************************
*/
int db_read_record_start(DB_CLIENT fd, READ_RECORD_T *ptRecord, uint32 *handle, uint32 *recNum);

/**
*********************************************************************
* @brief：     db操作单步读记录型数据
* @param[in]： DB_CLIENT fd                 DB客户端句柄
               uint32 handle                数据中心句柄
               STEP_READ_RECORD_T *ptRecord 数据        nNUM为0表示没有附加校验
* @param[out]：RECORD_UNIT_T *stepRecordOut 单步数据
* @return：    0-成功/错误码
*********************************************************************
*/
int db_read_record_step(DB_CLIENT fd, uint32 handle, STEP_READ_RECORD_T *ptRecord, RECORD_UNIT_T *stepRecordOut);

/**
*********************************************************************
* @brief：     db操作结束记录型数据读取
* @param[in]： DB_CLIENT fd            DB客户端句柄
               uint32 handle           数据中心句柄
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int db_read_record_end(DB_CLIENT fd, uint32 handle);

/**
*********************************************************************
* @brief：     db操作读最近的一条记录
* @param[in]： DB_CLIENT fd             DB客户端句柄
               READ_RECORD_T *ptRecord  记录条件
               uint32 bufLen            最大读取buf长度
* @param[out]：RECORD_UNIT_T *recordOut 数据记录
* @return：    0-成功/错误码
*********************************************************************
*/
int db_read_record(DB_CLIENT fd, READ_RECORD_T *ptRecord, uint32 bufLen, RECORD_UNIT_T *recordOut);

/**
*********************************************************************
* @brief：     db操作写记录型数据
* @param[in]： DB_CLIENT fd             DB客户端句柄
               WRITE_RECORD_T *ptRecord 记录    主OAD为0表示非复合型数据
* @param[out]：uint32 *storeNo          存储序号
* @return：    0-成功/错误码
*********************************************************************
*/
int db_write_record(DB_CLIENT fd, WRITE_RECORD_T *ptRecord, uint32 *storeNo);
int db_write_records(DB_CLIENT fd, WRITE_RECORDS_T *ptRecord, uint32 *storeNo);


/**
*********************************************************************
* @brief：     db操作写记录型数据(多条记录)
* @param[in]： DB_CLIENT fd              DB客户端句柄
               WRITE_RECORDS_T *ptRecord 记录    主OAD为0表示非复合型数据
* @param[out]：uint32 *storeNo           存储序号
* @return：    0-成功/错误码
*********************************************************************
*/
int db_write_records(DB_CLIENT fd, WRITE_RECORDS_T *ptRecord, uint32 *storeNo);

/**
*********************************************************************
* @brief：     db操作更新记录数据
* @param[in]： DB_CLIENT fd              DB客户端句柄
               UPDATE_RECORD_T *ptRecord 数据   主OAD为0表示非复合型数据
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int db_update_record(DB_CLIENT fd, UPDATE_RECORD_T *ptRecord);

/**
*********************************************************************
* @brief：     db操作获取存储记录条数
* @param[in]： DB_CLIENT fd              DB客户端句柄
               GET_RECORD_NUM_T *pRecord 条件
* @param[out]：uint32 *recNum            数量
* @return：    0-成功/错误码
*********************************************************************
*/
int db_get_record_num(DB_CLIENT fd, GET_RECORD_NUM_T *pRecord, uint32 *recNum);

/**
*********************************************************************
* @brief：     db操作获取存储记录深度
* @param[in]： DB_CLIENT fd              DB客户端句柄
               OOP_ROAD_T *ptRroad       条件
* @param[out]：uint32 *depthNum          深度
* @return：    0-成功/错误码
*********************************************************************
*/
int db_get_record_depth(DB_CLIENT fd, RECORD_OAD_T *ptRroad, uint32 *depthNum);

/**
*********************************************************************
* @brief：     db操作数据初始化
* @param[in]： DB_CLIENT fd         DB客户端句柄
               CLEAR_DATA_T *ptData 清除选项
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int db_clear_data(DB_CLIENT fd, CLEAR_DATA_T *ptData);

/**
*********************************************************************
* @brief：     db操作指定数据初始化
* @param[in]： DB_CLIENT fd           DB客户端句柄
               CLEAR_DATA_T *ptData   清除选项
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int db_clear_oad_data(DB_CLIENT fd, CLEAR_DATA_T *ptData);

/**
*********************************************************************
* @brief：     db操作恢复出厂
* @param[in]： DB_CLIENT fd       DB客户端句柄
               uint8 logicId      逻辑设备号
* @param[out]：
* @return：    0-成功/错误码
*********************************************************************
*/
int db_recover_default(DB_CLIENT fd, uint8 logicId);

/**
*********************************************************************
* @brief：     普通数据转Base64格式
* @param[in]： uint8 *pData         数据
               uint32 datalen       数据长度
* @param[out]：
* @return：    base64结果
*********************************************************************
*/
char *db_data_to_base64(uint8 *pData, uint32 datalen);

/**
*********************************************************************
* @brief：     Base64格式专普通数据
* @param[in]： uint8 *pBase         Base64数据
               uint32 buflen        输出结果缓存大小
* @param[out]：uint8 *pBuf          数据输出结果
               uint32 *datalen      输出结果实际长度
* @return：    0-成功/错误码
*********************************************************************
*/
int db_base64_to_data(char *pBase, uint8 *pBuf, uint32 buflen, uint32 *datalen);

#ifdef __cplusplus
}
#endif

#endif

