/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： sqlite数据库原始数据操作接口函数
* @date： 2019-11-9
*********************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "baseType.h"
#include "dbRaw.h"
#include "globalConfiguration.h"
#include "olog.h"
#include "sqlite3.h"

#define SYNC_OFF (0)    //写同步关闭
#define SYNC_NORMAL (1) //写同步正常
#define SYNC_FULL (2)   //写同步全开

//#define DB_RECOVERY 1

#define DB_NORMAL 1

//日志输出
extern void *     db_ocat   ;
//日志输出
extern void *     db_err;

/**
*********************************************************************
* @brief： 获取数据库写操作句柄
* @param[in]： 
               db      数据库操作信息结构体 
* @param[out]：
               none
* @return： 写操作句柄
*********************************************************************
*/
static sqlite3 *db_writehander_get(DB_RAW_T *db)
{
    return db->wHander;
}

/**
*********************************************************************
* @brief： 获取数据库读操作句柄
* @param[in]： 
               db       数据库操作信息结构体 
* @param[out]：
               none
* @return： 读操作句柄
*********************************************************************
*/
static sqlite3 *db_readhander_get(DB_RAW_T *db)
{
    return db->rHander;
}

/**
*********************************************************************
* @brief： 获取数据库备份写操作句柄
* @param[in]： 
               db       数据库操作信息结构体
* @param[out]：
               none
* @return： 备份写操作句柄
*********************************************************************
*/
static sqlite3 *db_writebackuphander_get(DB_RAW_T *db)
{
    return db->wHanderBackup;
}

/**
*********************************************************************
* @brief： 获取数据库备份读操作句柄
* @param[in]： 
               db       数据库操作信息结构体
* @param[out]：
               none
* @return： 备份读操作句柄
*********************************************************************
*/
static sqlite3 *db_readbackuphander_get(DB_RAW_T *db)
{
    return db->rHanderBackup;
}

/**
*********************************************************************
* @brief： 关闭数据库
* @param[in]： 
               rdb       数据库操作信息结构体
* @param[out]：
               none
* @return： 0
*********************************************************************
*/
sint32 db_close(DB_RAW_T *rdb)
{
    if (rdb == NULL)
    {
        return 0;
    }
    if (rdb->DBType == DB_TYPE_BACKUP)
    {
        sqlite3_close_v2(rdb->wHanderBackup);
        sqlite3_close_v2(rdb->rHanderBackup);
    }

    sqlite3_close_v2(rdb->wHander);
    //close all rHander
    sqlite3_close_v2(rdb->rHander);

    free(rdb);
    rdb = NULL;

    return 0;
}

/**
*********************************************************************
* @brief： 打开SQlite数据库文件
* @param[in]： 
               file       数据库文件
               backup     数据库备份文件
               isWal      WAL模式标志位
               syncMod    写同步标志位
* @param[out]：
               rdb        数据库信息返回结构体
* @return： 操作结果
*********************************************************************
*/
sint32 db_raw_open(const sint8 *file, const sint8 *bcakup, uint32 isWal_NULL, sint32 syncMod, DB_RAW_T *rdb)
{
    sint32 iResult;
    sint32 initNum = 1;
    sint32 i = 0;
    sint8 *sync;
    sint8 sqlbuf[1024];
    uint32 walCount = g_configObj.dbCfg.walCount;

    const sint8 *filename;
    const sint8 *fbuf[2] = {};
    sqlite3 **dbRead;
    sqlite3 **dbwrite;

    fbuf[0] = file;
    fbuf[1] = bcakup;

    switch (syncMod)
    {
    case SYNC_OFF:
        sync = "OFF";
        break;
    case SYNC_NORMAL:
        sync = "NORMAL";
        break;
    case SYNC_FULL:
        sync = "FULL";
        break;
    default:
        sync = "FULL";
        break;
    }

    if (bcakup != NULL)
    {
        initNum = 2;
    }

    for (i = 0; i < initNum; i++)
    {
        filename = fbuf[i];
        if (i == 0)
        {
            dbwrite = &rdb->wHander;
            dbRead = &rdb->rHander;
        }
        else
        {
            dbwrite = &rdb->wHanderBackup;
            dbRead = &rdb->rHanderBackup;
        }

        do
        {
            //write hander init
            {
                iResult = sqlite3_open_v2(filename, dbwrite,
                                          SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX /*| SQLITE_OPEN_SHAREDCACHE*/, NULL);
                if (SQLITE_OK != iResult)
                {
                    olog_error(db_ocat, "open db [%s] fail  RET:[%d]", filename, iResult);
                    break;
                }

                sqlite3_busy_timeout(*dbwrite, 200);

#ifdef DB_RECOVERY
                //check whether db is breakdown
                sint32 ret = check_database_error(dbwrite, filename);
                if (ret == 1)
                {
                    olog_warn(db_err, "need recovery the database[%s]", filename);
                    sqlite3_close_v2(dbwrite);
                    recovery_database(filename);

                    olog_warn(db_err, "reopen the db[%s]", filename);
                    iResult = sqlite3_open_v2(filename, dbwrite,
                                          SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX /*| SQLITE_OPEN_SHAREDCACHE*/, NULL);
                    if (SQLITE_OK != iResult)
                    {
                        olog_error(db_ocat, "open db [%s] fail  RET:[%d]", filename, iResult);
                        break;
                    }
                    sqlite3_busy_timeout(*dbwrite, 200);
                }
#endif
                sprintf(sqlbuf, "PRAGMA synchronous = %s; ", sync);
                //执行SQL语句
                iResult = sqlite3_exec(*dbwrite, sqlbuf, 0, 0, 0);
                if (SQLITE_OK != iResult)
                {
                    olog_error(db_ocat, "wHander synchronous set[%s] fail   RET:[%d]", sync, iResult);
                    break;
                }

                if (walCount > 0)
                {
                    //执行SQL语句
                    iResult = sqlite3_exec(*dbwrite, "PRAGMA journal_mode=WAL;", NULL, 0, 0);
                    if (SQLITE_OK != iResult)
                    {
                        olog_error(db_ocat, "wHander WAL Mode set Fail RET:[%d]", iResult);
                        break;
                    }
                    else
                    {
                        olog_debug(db_ocat, "wHander WAL Mode set suss RET:[%d]", iResult);
                    }

                    //PRAGMA wal_autocheckpoint=
                    sprintf(sqlbuf, "PRAGMA wal_autocheckpoint = %d; ", walCount);
                    //执行SQL语句
                    iResult = sqlite3_exec(*dbwrite, sqlbuf, 0, 0, 0);
                    if (SQLITE_OK != iResult)
                    {
                        olog_error(db_ocat, "set wal_autocheckpoint = %d fail   RET:[%d]", walCount, iResult);
                        break;
                    }
                    else
                    {
                        olog_debug(db_ocat, "set wal_autocheckpoint = %d succ   RET:[%d]", walCount, iResult);
                    }
                } else {
                    //执行SQL语句
                    iResult = sqlite3_exec(*dbwrite, "PRAGMA journal_mode=DELETE;", NULL, 0, 0);
                    if (SQLITE_OK != iResult)
                    {
                        olog_error(db_ocat, "wHander DELETE Mode set Fail RET:[%d]", iResult);
                        break;
                    }
                    else
                    {
                        olog_debug(db_ocat, "wHander DELETE Mode set suss RET:[%d]", iResult);
                    }
                }
                //空间自动回收设定
/*
#ifdef SPACE_RECLAMATION_SWITCH
                iResult = sqlite3_exec(*dbwrite, "PRAGMA auto_vacuum=FULL;", NULL, 0, 0);
                if (SQLITE_OK != iResult)
                {
                    olog_error(db_ocat, "auto_vacuum set Fail RET:[%d]", iResult);
                    break;
                }
#else
*/                //close the auto_vacuum
                //执行SQL语句
                iResult = sqlite3_exec(*dbwrite, "PRAGMA auto_vacuum=NONE;", NULL, 0, 0);
                if (SQLITE_OK != iResult)
                {
                    olog_error(db_ocat, "auto_vacuum set Fail RET:[%d]", iResult);
                    break;
                }
//#endif 
            }
#if 1
            //read hander init
            {
                iResult = sqlite3_open_v2(filename, dbRead,
                                          SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX  | SQLITE_OPEN_SHAREDCACHE, NULL);
                if (SQLITE_OK != iResult)
                {
                    olog_error(db_ocat, "open db [%s] fail RET:[%d]", filename, iResult);
                    break;
                }

                sqlite3_busy_timeout(*dbRead, 200);

                //check whether db is breakdown
                //check_database_error(dbRead, filename);

                sprintf(sqlbuf, "PRAGMA synchronous = %s; ", sync);
                //执行SQL语句
                iResult = sqlite3_exec(*dbRead, sqlbuf, 0, 0, 0);
                if (SQLITE_OK != iResult)
                {
                    olog_error(db_ocat, "rHander synchronous set[%s] fail RET:[%d]", sync, iResult);
                    break;
                }

                olog_debug(db_ocat, "create db filename:%s ,buf:%s sync:%s \n",filename, sqlbuf, sync);

                if (walCount > 0)
                {
                    //执行SQL语句
                    iResult = sqlite3_exec(*dbRead, "PRAGMA journal_mode=WAL;", NULL, 0, 0);
                    if (SQLITE_OK != iResult)
                    {
                        olog_error(db_ocat, "rHander WAL Mode set Fail RET:[%d]", iResult);
                        break;
                    }
                    else
                    {
                        olog_debug(db_ocat, "rHander WAL Mode set suss RET:[%d]", iResult);
                    }

                    // PRAGMA wal_autocheckpoint=
                    sprintf(sqlbuf, "PRAGMA wal_autocheckpoint = %d; ", walCount);
                    //执行SQL语句
                    iResult = sqlite3_exec(*dbRead, sqlbuf, 0, 0, 0);
                    if (SQLITE_OK != iResult)
                    {
                        olog_error(db_ocat, "set wal_autocheckpoint = %d fail RET:[%d]", walCount, iResult);
                        break;
                    }
                    else
                    {
                        olog_debug(db_ocat, "set wal_autocheckpoint = %d succ RET:[%d]", walCount, iResult);
                    }
                } else {
                    //执行SQL语句
                    iResult = sqlite3_exec(*dbRead, "PRAGMA journal_mode=DELETE;", NULL, 0, 0);
                    if (SQLITE_OK != iResult)
                    {
                        olog_error(db_ocat, "rHander DELETE Mode set Fail RET:[%d]", iResult);
                        break;
                    }
                    else
                    {
                        olog_debug(db_ocat, "rHander DELETE Mode set suss RET:[%d]", iResult);
                    }
                }
            }
#endif

        } while (0);
    }

    if (iResult)
    {
        if (rdb->rHander)
        {
            sqlite3_close_v2(rdb->rHander);
        }

        if (rdb->wHander)
        {
            sqlite3_close_v2(rdb->wHander);
        }

        if (rdb->wHanderBackup)
        {
            sqlite3_close_v2(rdb->wHanderBackup);
        }

        if (rdb->rHanderBackup)
        {
            sqlite3_close_v2(rdb->rHanderBackup);
        }
    }
    else
    {
        rdb->db_readhander_get = db_readhander_get;
        rdb->db_writehander_get = db_writehander_get;
        rdb->db_readbackuphander_get = db_readbackuphander_get;
        rdb->db_writebackuphander_get = db_writebackuphander_get;
    }

    return iResult;
}

/**
*********************************************************************
* @brief： 打开数据文件
* @param[in]： 
               filename     数据库文件
               backup       数据库备份文件
               sqlType      数据库存储类型
* @param[out]：
               db           数据库信息返回结构体
* @return： 操作结果
*********************************************************************
*/
sint32 db_open(const sint8 *filename, const sint8 *backup, sint32 sqlType, DB_RAW_T **db)
{

    sint32 ret = -1;

    if (NULL == filename)
        return ret;

    *db = (DB_RAW_T *)malloc(sizeof(DB_RAW_T));
    memset(*db, 0, sizeof(DB_RAW_T));
    (*db)->DBType = sqlType;
    (*db)->rHander = NULL;
    (*db)->wHander = NULL;
    (*db)->wHanderBackup = NULL;
    (*db)->rHanderBackup = NULL;

    switch (sqlType)
    {
    case DB_TYPE_BACKUP:

        if (NULL != filename && NULL != backup)
        {
            ret = db_raw_open(filename, backup, 1, SYNC_FULL, *db);
        }
        else
        {
            olog_error(db_ocat,"DB_TYPE_BACKUP need valid path.");
        }
        break;
    case DB_TYPE_NORMAL:
        ret = db_raw_open(filename, NULL, 1, SYNC_NORMAL, *db);
        break;
#if DB_NORMAL
    case DB_TYPE_CACHE:
        ret = db_raw_open(filename, backup, 1, SYNC_NORMAL, *db);
        break;
    case DB_TYPE_RECORD:
        ret = db_raw_open(filename, NULL, 1, SYNC_NORMAL, *db);
        break;
#else
    case DB_TYPE_CACHE:
        ret = db_raw_open(filename, backup, 1, SYNC_OFF, *db);
        break;
    case DB_TYPE_RECORD:
        ret = db_raw_open(filename, NULL, 1, SYNC_OFF, *db);
        break;
#endif
    case DB_TYPE_NORMAL_MEMORY:
        ret = db_raw_open(filename, NULL, 1, SYNC_OFF, *db);
        break;
    default:
        break;
    }

    return ret;
}
