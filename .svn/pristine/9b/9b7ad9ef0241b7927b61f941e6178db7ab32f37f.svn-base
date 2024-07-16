/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 数据库原始数据处理头文件
* @date： 2019-11-9
*********************************************************************
*/

#ifndef __DB_RAW_H__
#define __DB_RAW_H__
#include <pthread.h>
#include "sqlite3.h"
#include "baseType.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DB_TYPE_BACKUP  (0)
#define DB_TYPE_NORMAL  (1)
#define DB_TYPE_CACHE   (2)
#define DB_TYPE_RECORD  (3)
#define DB_TYPE_NORMAL_MEMORY  (4)

/* @brief:类型定义  */
typedef struct __db_raw DB_RAW_T;

/* @brief:功能描述  */
struct __db_raw
{

    sint32 DBType;
    sqlite3 *wHander;
    sqlite3 *rHander;
    sqlite3 *wHanderBackup;
    sqlite3 *rHanderBackup;
    //  sqlite3 *rHander[MAX_RHANDER_NUM];
    //  sint32 rHanderNum;
    sqlite3 *(*db_writehander_get)(DB_RAW_T *db);
    sqlite3 *(*db_readhander_get)(DB_RAW_T *db);
    sqlite3 *(*db_writebackuphander_get)(DB_RAW_T *db);
    sqlite3 *(*db_readbackuphander_get)(DB_RAW_T *db);
};

/* @brief:打开数据库  */
sint32 db_open(const sint8 *filename, const sint8 *backup, sint32 sqlType, DB_RAW_T **db);

/* @brief:打开SQlite数据库文件  */
// sint32 db_raw_open(const sint8 *file, const sint8 *bcakup, sint32 isWal, sint32 syncMod, DB_RAW_T *rdb);

/* @brief:关闭数据库  */
sint32 db_close(DB_RAW_T *rdb);

#ifdef __cplusplus
}
#endif

#endif





