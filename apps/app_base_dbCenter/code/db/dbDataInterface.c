/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 数据库底层操作API函数
* @date： 2019-11-9
*********************************************************************
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "baseType.h"
#include "crc16.h"
#include "dbCenterConfig.h"
#include "dbComm.h"
#include "dbDataInterface.h"
#include "dbModule.h"
#include "olog.h"
//#include "qlibc.h"
#include "xdrCustom.h"
#include "msgHandler.h"

#define SQL_DUMP_DEBUG 1

//单步读写记录型数据存储用hashtable
static qhashtbl_t *s_psetupReadInfotbl;
static qhashtbl_t *s_pstepWriteCltInfo;

//static sint32 m_is_recoder_exist(DB_TAB_NORMAL_INFO_T *handle, sint8 *condition, sint32 len);
//读普通型数据
static sint32
m_read_normal_data_process(sqlite3 *db, sint8 *sqlbuff, NORMAL_DATA_READ_RSP_T *rspdata, sint32 isforbackup);
//普通型数据CRC值处理
static sint32 m_read_normal_data_crc_process(sqlite3 *db, sint8 *sqlbuff, NORMAL_DATA_READ_RSP_T *rspdata);
//单步写记录型数据到缓存
static int step_write_collect_db(sqlite3 *db, WRITE_RECORD_STEP_INFO_T *pInfo, sqlite3_stmt *pStmt, sqlite3_stmt *pStmt_u);
//交缓存数据到ＤＢ，完成单步写记录型数据
static sint32 comp_write_collect_db(sqlite3_stmt *pStmt, sqlite3_stmt *pStmt_u);
static pthread_t db_interface_service_tid = 0;
//日志输出
extern void *db_ocat;
extern void *db_err;

/**
*********************************************************************
* @brief： 释放单步读取超时句柄中成员链表 (单步读取操作句柄超时清除)
* @param[in]： pList     已分组的记录链表
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void free_read_handler_obj(qlist_t *pList)
{
    qlist_obj_t listobj                    = {0};
    READ_RECORD_STEP_RSP_T *pRecordstepRsp = NULL;

    if (pList != 0)
    {
        memset((void *)&listobj, 0, sizeof(listobj));  //需要先设为0
        /* 释放list中存放的单步记录型数据 */
        while (pList->getnext(pList, &listobj, false) == true)
        {
            memcpy(&pRecordstepRsp, listobj.data, sizeof(pRecordstepRsp));
            olog_debug(db_ocat, "pRecordstepRsp [%u][%0x][%0x]", pRecordstepRsp->infoNum, pRecordstepRsp->majorDI,
                       pRecordstepRsp->minorDI);
			if(pRecordstepRsp != NULL)
        	{
        		free(pRecordstepRsp);
        	}
        }
		pList->clear(pList);
        pList->free(pList);
    }
}

/**
*********************************************************************
* @brief： 释放单步读取超时句柄 (单步读取操作句柄超时清除)
* @param[in]： handlerTable     已分组的哈希表
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void free_read_handler(qlist_t *handlerTable)
{

	if(handlerTable == NULL)
	{
		return;
	}
	
    qlist_obj_t listobj = {0};
    qlist_t *pList      = NULL;

    memset((void *)&listobj, 0, sizeof(listobj));  //需要先设为0
    /* 释放list中存放的单步记录型数据 */
    handlerTable->lock(handlerTable);
    while (handlerTable->getnext(handlerTable, &listobj, false) == true)
    {
        memcpy(&pList, listobj.data, sizeof(pList));
        free_read_handler_obj(pList);
    }
    handlerTable->unlock(handlerTable);
	handlerTable->clear(handlerTable);
    handlerTable->free(handlerTable);
}

/**
*********************************************************************
* @brief： 数据接口服务进程 (单步读取操作句柄超时清除)
* @param[in]： arg         线程参数当前未使用
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void *db_interface_service(void *arg)
{
    qhashtbl_obj_t hashobj = {0};
    struct timespec currenttime;
    STEP_READ_RECORD_INFO_T *recordInfo = NULL;
    qhashtbl_t *cancelTable             = qhashtbl(0, 0);

    while (1)
    {
        sleep(30);

        memset((void *)&hashobj, 0, sizeof(hashobj));
        //循环遍历所有单步读操作句柄,此处不需要锁定hashtable
        s_psetupReadInfotbl->lock(s_psetupReadInfotbl);
        while (s_psetupReadInfotbl->getnext(s_psetupReadInfotbl, &hashobj, false) == true)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
            //get the handle of step read  record
            recordInfo = (STEP_READ_RECORD_INFO_T *)strtoul(hashobj.data, NULL, 10);
            if (NULL != recordInfo)
            {
                olog_debug(db_ocat, "handle[%s] ct[%ld] wt[%ld]", hashobj.name, currenttime.tv_sec,
                           recordInfo->time.tv_sec);
                //判断最后操作时间是否超时
                if ((currenttime.tv_sec - recordInfo->time.tv_sec) > 60)
                {
                    //put the handle which is alread timeout into the cancel table
                    cancelTable->put(cancelTable, hashobj.name, hashobj.data, hashobj.size);
                    //remvoe the step read handle from the s_psetupReadInfotbl hashtable
                    s_psetupReadInfotbl->remove(s_psetupReadInfotbl, hashobj.name);
                }
            }
        }
        s_psetupReadInfotbl->unlock(s_psetupReadInfotbl);

        memset((void *)&hashobj, 0, sizeof(hashobj));
        //遍历待删除table中的内容
        while (cancelTable->getnext(cancelTable, &hashobj, false) == true)
        {
            recordInfo = (STEP_READ_RECORD_INFO_T *)strtoul(hashobj.data, NULL, 10);
            //单步读句柄
            free_read_handler(recordInfo->handerNum);
            olog_debug(db_ocat, "Clear Step Read Record Handler:Handler [%s]", (char*)hashobj.data);
            //释放资源
            if(recordInfo != NULL)
        	{
        		free(recordInfo);
        	}
            
        }
        cancelTable->clear(cancelTable);
    }

    return NULL;
}

/**
*********************************************************************
* @brief： 表地址数据
* @param[in]：
               pAddr 	带转换的表地址
               addrLen	地址长度
* @param[out]：
               pAddrStr  转换后的字符串格式地址，带长度
               strMaxLen 地址最大长度
* @return：    返回值说明 转换后的地址长度
*********************************************************************
*/
sint32 addr_trans_format(uint8 *pAddr, uint8 addrLen, char *pAddrStr, uint8 strMaxLen)
{
	int i = 0;
	int offset = 0;
    if(addrLen == 0 || addrLen >16)
	{
		olog_debug(db_ocat, "addr_trans_format address len is error[%u]", addrLen);
		return 0;
	}

	if(strMaxLen < addrLen*2)
	{
		olog_debug(db_ocat, "addr_trans_format strlen len is error[%u]<[%u]", strMaxLen, addrLen*2);
		return 0;
	}

	for (i=0; i<addrLen; i++)
    {
    	sprintf(pAddrStr+offset, "%02X", pAddr[i]);
		offset += 2;
    }

	return offset;
}

/**
*********************************************************************
* @brief： datainterface初始化
* @param[in]：
               void
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 m_db_api_module_init(void)
{
    sint32 ret = -1;
    // 创建hash表
    //单步读句柄hash表
    s_psetupReadInfotbl = qhashtbl(0, QHASHTBL_THREADSAFE);
    s_pstepWriteCltInfo = qhashtbl(0, QHASHTBL_THREADSAFE);
    //创建线程， 单步读取操作句柄超时清除
    ret                 = pthread_create(&db_interface_service_tid, NULL, db_interface_service, NULL);

    return ret;
}

/**
*********************************************************************
* @brief： 记录型数据用释放内存函数
* @param[in]：
               *p        记录型数据指针
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
void m_free_record_buf(void *p)
{
    if (NULL != p)
    {
        p = p - sizeof(READ_RECORD_STEP_RSP_T);
        free(p);
    }
}

/**
*********************************************************************
* @brief： 普通型数据crc处理函数
* @param[in]：
               db          数据库句柄
               sqlbuf      sql语句
* @param[out]：
               rspdata     回复内容
* @return： 返回值说明
*********************************************************************
*/
sint32 m_read_normal_data_crc_process(sqlite3 *db, sint8 *sqlbuff, NORMAL_DATA_READ_RSP_T *rspdata)
{
    sint32 nResult      = -1;  //执行结果
    sint32 nRc          = -1;  //函数返回值
    const sint8 *szTail = NULL;
    sqlite3_stmt *pVm   = NULL;  //预编译对象
    sint32 nLen = 0;
    uint16 crcValue = 0;
//    struct timespec currenttime;

    //预编译SQL语句
    nResult = sqlite3_prepare_v2(db, sqlbuff, -1, &pVm, &szTail);
    if (nResult == SQLITE_OK)
    {
//        clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//        olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], START", __FUNCTION__, sqlbuff, currenttime.tv_sec, currenttime.tv_nsec);
        //执行预编译的SQL语句
        nResult = sqlite3_step(pVm);
//        clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//       olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], END", __FUNCTION__, sqlbuff, currenttime.tv_sec, currenttime.tv_nsec);

        //存在行记录
        if (nResult == SQLITE_ROW)
        {
            //判断查询到的列
            sint32 nCols = sqlite3_column_count(pVm);
            if (nCols >= 1)
            {
                //读取Blob数据
                const void *pFileContent = sqlite3_column_blob(pVm, 0);
                //取得blob数据的长度，0是列索引值（从0开始）
                nLen = sqlite3_column_bytes(pVm, 0);
                if (nLen > 0 && pFileContent != NULL)
                {
                    //给outBut开辟空间
                    rspdata->outBuf = (uint8 *)malloc(nLen);
                    //查询到的数据拷贝到outBuf中
                    memcpy(rspdata->outBuf, pFileContent, nLen);
                    rspdata->bufLen       = nLen;
                    rspdata->outBufFreeFp = free;
                    nRc                   = 0;
                }

                //读取DataTypeID数据
                rspdata->dataTypeID = sqlite3_column_int(pVm, 1);

                //读取crc数据
                crcValue = sqlite3_column_int(pVm, 2);

                //计算crc值
                uint16 tmpCRCValue = CRC16_modebus(rspdata->outBuf, rspdata->bufLen);
                olog_debug(db_ocat, "%s, crc[%d], tmpCRC[%d]", __FUNCTION__, crcValue, tmpCRCValue);

                //比较crc值
                if (crcValue != tmpCRCValue)
                {
                    olog_debug(db_ocat, "The record is invalid");
                    nRc = -1;
                }

            }
        }
    }
    //释放掉预编译的sql语句
    sqlite3_finalize(pVm);
    return nRc;
}

/**
*********************************************************************
* @brief： 读普通数据
* @param[in]：
               req        读普通数据输入参数
* @param[out]：
               rsp        普通数据
* @return： 返回值说明
*********************************************************************
*/
sint32 m_read_normal_data(NORMAL_DATA_READ_REQ_T *req, uint16 datatype, NORMAL_DATA_READ_RSP_T *rsp)
{
    sint8 buff[1024]    = {0};
    DB_TAB_NORMAL_INFO_T hNormalinfo;
    sint32 nResult = -1;  //函数返回值，-1异常
//    struct timespec currenttime;

//    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//    olog_info(db_ocat, "====%s, sec[%ld], nsec[%ld], START", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);
    //根据dataTag，取得数据库情报
    if (get_normal_tab_info(req->dataTag, datatype, &hNormalinfo))
    {
        olog_error(db_ocat, "can't match table tag:[%d]", req->dataTag);
        return -1;
    }

//    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//    olog_info(db_ocat, "====%s, sec[%ld], nsec[%ld], START", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);

    rsp->dataTag = req->dataTag;

    //判断数据库句柄是否为空
    if (hNormalinfo.handle != NULL)
    {
        if (strcmp(hNormalinfo.tab_name, DB_CENT_TABLE_NAME_PARAMETER) != 0)
        {
            snprintf(buff, sizeof(buff),
                    "SELECT DataContent, DataTypeID FROM %s where LDeviceNo ='%u' and InfoAddrNo  ='%u' and DataID = '%u'",
                    hNormalinfo.tab_name, req->logicalDeviceNum, req->infoNum, req->dataTag);
#ifdef SQL_DUMP_DEBUG
            olog_debug(db_ocat, "SQL:[%s]", buff);
#endif
            //取得读取函数句柄
            sqlite3 *db = hNormalinfo.handle->db_readhander_get(hNormalinfo.handle);
            nResult     = m_read_normal_data_process(db, buff, rsp, 0);
            if (nResult < 0)
            {  //主数据库内数据不存在或其他异常
                //读取备份数据
                olog_debug(db_ocat, "not find the data in the major database, so read data from backup database");
                if (hNormalinfo.realtime == 2)  //内存型
                {
                    olog_debug(db_ocat, "table[%s] is realtime", hNormalinfo.tab_name);
                    return nResult;
                }
                sqlite3 *db_backup = hNormalinfo.handle->db_readbackuphander_get(hNormalinfo.handle);
                nResult            = m_read_normal_data_process(db_backup, buff, rsp, 1);
                if (nResult == S_OK)
                {
                    //回写
                    nResult = write_normal_data_mast(hNormalinfo.handle->db_writehander_get(hNormalinfo.handle),
                                                    hNormalinfo.tab_name, req, rsp);
                }
            }
        } else {
                // read the datacontent and crc
                snprintf(buff, sizeof(buff),
                        "SELECT DataContent, DataTypeID, CRC FROM %s where LDeviceNo ='%u' and InfoAddrNo  ='%u' and DataID = '%u'",
                        hNormalinfo.tab_name, req->logicalDeviceNum, req->infoNum, req->dataTag);
    #ifdef SQL_DUMP_DEBUG
                olog_debug(db_ocat, "SQL:[%s]", buff);
    #endif
                //取得读取函数句柄
                sqlite3 *db     = hNormalinfo.handle->db_readhander_get(hNormalinfo.handle);
                //uint16 crcValue = 0;
                nResult         = m_read_normal_data_crc_process(db, buff, rsp);

                if (nResult < 0)
                {  //主数据库内数据不存在或其他异常
                    //读取备份数据
                    olog_debug(db_ocat, "not find the data in the major database, so read data from backup database");
                    if (hNormalinfo.realtime == 2)  //内存型
                    {
                        olog_warn(db_ocat, "table[%s] is realtime", hNormalinfo.tab_name);
                        return nResult;
                    }
                    sqlite3 *db_backup = hNormalinfo.handle->db_readbackuphander_get(hNormalinfo.handle);
                    nResult            = m_read_normal_data_crc_process(db_backup, buff, rsp);
                    if (nResult == S_OK)
                    {
                        //回写
                        nResult = write_normal_data_mast(hNormalinfo.handle->db_writehander_get(hNormalinfo.handle),
                                                        hNormalinfo.tab_name, req, rsp);
                        if (nResult == -1)
                        {
                            olog_warn(db_ocat, "write_normal_data_mast failed");
                        }
                    }
                }
        }
    }

//    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);

//    olog_info(db_ocat, "==== %s,  sec[%ld], nsec[%ld], END", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);
    return nResult;
}

/**
*********************************************************************
* @brief： 写普通数据
* @param[in]：
               req        写普通数据结构体
* @param[out]：
               0-成功   其他-错误
* @return： 返回值说明
*********************************************************************
*/
sint32 m_write_normal_data(NORMAL_DATA_WRITE_RSP_T *req)
{
    sint32 nRc                       = -1;
    sint32 nRcbackup                 = -1;
    DB_TAB_NORMAL_INFO_T hNormalinfo = {0};
    sint8 sqlstr[512]                = {0};
    sqlite3_stmt *pStmt              = NULL;  //预编译SQL
//    struct timespec currenttime;
    //取得数据情报和数据操作句柄
    if (get_normal_tab_info(req->dataTag, req->dataTypeID, &hNormalinfo))
    {
        olog_debug(db_ocat, "can't match table tag:[%d]", req->dataTag);
        return -1;
    }

    //备份型数据和内存数据类型不能同时填写
    if ((hNormalinfo.is_backup == 1) && (hNormalinfo.realtime == 2))
    {
        olog_error(db_ocat, "The table config is_backup[1], realtime[2] is invalid");
        return -1;
    }
    // CRC16
    uint16 crc16 = 0;
    if (strcmp(hNormalinfo.tab_name, DB_CENT_TABLE_NAME_PARAMETER) == 0)
    {
        //计算crc值
        crc16 = CRC16_modebus(req->outBuf, req->outBufLen);
    }

    if (hNormalinfo.handle != NULL)
    {
        //拼接SQL的条件where之后的部分
        // snprintf(sqlcondition, sizeof(sqlcondition), "LDeviceNo = '%u' and InfoAddrNo = '%u' and DataID = '%u'",
        //         req->logicalDeviceNum, req->infoNum, req->dataTag);
        //判断是否已存在改记录
        // if ((m_is_recoder_exist(&hNormalinfo, sqlcondition, sizeof(sqlcondition))) > 0)
        // {
        //     //拼接更新用的SQL语句
        //     snprintf(sqlstr, sizeof(sqlstr), "UPDATE '%s' SET  DataContent =?,UpdateTime =
        //     datetime('now','localtime') where %s",
        //              hNormalinfo.tab_name, sqlcondition);
        // }
        // else
        //{
        //拼接插入用的SQL语句
        // snprintf(sqlstr, sizeof(sqlstr), "INSERT INTO '%s' VALUES ('%u', '%u', '%u' ,? ,
        // datetime('now','localtime'))",
        //          hNormalinfo.tab_name, req->logicalDeviceNum, req->infoNum, req->dataTag);
        //}
        snprintf(sqlstr, sizeof(sqlstr),
                 "REPLACE INTO '%s' VALUES ('%u', '%u', '%u', '%d' ,? , datetime('now','localtime'), '%d')",
                 hNormalinfo.tab_name, req->logicalDeviceNum, req->infoNum, req->dataTag, req->dataTypeID, crc16);
#ifdef SQL_DUMP_DEBUG
        olog_debug(db_ocat, "SQL:[%s],blobdata:[%s]", sqlstr, req->outBuf);
#endif
        //预编译SQL,生成pStmt
        nRc = sqlite3_prepare_v2(hNormalinfo.handle->db_writehander_get(hNormalinfo.handle), sqlstr, -1, &pStmt, 0);
        olog_debug(db_ocat,"nRc = 0x%x", nRc);
        //绑定第1个(函数的第二个参数)blob数据
        nRc = sqlite3_bind_blob(pStmt, 1, req->outBuf, (sint32)(req->outBufLen), NULL);  // 1 is a Index,  from 1
        // holog_debug(db_ocat,req->outBuf, (sint32)(req->outBufLen));
        olog_debug(db_ocat,"nRc = %d", nRc);
        if (nRc == SQLITE_OK)
        {
            // prepare/bind执行成功的话,执行step，INSERT/UPDATE如果执行成功则为SQLITE_DONE
//            clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//            olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], START", __FUNCTION__, sqlstr, currenttime.tv_sec, currenttime.tv_nsec);
            //执行预编译的SQL语句
            nRc = sqlite3_step(pStmt);
//            clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//            olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], END", __FUNCTION__, sqlstr, currenttime.tv_sec, currenttime.tv_nsec);
            olog_debug(db_ocat,"*sql:%s nRc = %d", sqlstr, nRc);
        }
        sqlite3_finalize(pStmt);  //释放预编译SQL语句

        //备份型数据
        if (1 == hNormalinfo.is_backup)
        {
            if (hNormalinfo.realtime == 2)  //内存型
            {
                return (nRc == SQLITE_DONE) ? S_OK : S_NG;
            }
            //插入/更新到备份表中
            nRcbackup = sqlite3_prepare_v2(hNormalinfo.handle->db_writebackuphander_get(hNormalinfo.handle), sqlstr, -1,
                                        &pStmt, 0);
            olog_debug(db_ocat,"nRcbackup1 = 0x%x", nRcbackup);
            nRcbackup |=
                sqlite3_bind_blob(pStmt, 1, req->outBuf, (sint32)(req->outBufLen), NULL);  // 1 is a Index,  from 1
                olog_debug(db_ocat,"nRcbackup2 = 0x%x", nRcbackup);
            if (nRcbackup == SQLITE_OK)
            {
//                clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//                olog_info(db_ocat, "====%s, sql[%s], BACKUP sec[%ld], nsec[%ld], START", __FUNCTION__, sqlstr, currenttime.tv_sec, currenttime.tv_nsec);
                //执行预编译的SQL语句
                nRcbackup = sqlite3_step(pStmt);
//                clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//                olog_info(db_ocat, "====%s, sql[%s], BACKUP sec[%ld], nsec[%ld], END", __FUNCTION__, sqlstr, currenttime.tv_sec, currenttime.tv_nsec);
            }
            sqlite3_finalize(pStmt);  //释放预编译SQL语句
        }

        if (nRc != SQLITE_DONE)
        {
            olog_error(db_ocat, "update data failuretable:%s ret:%d\n",  hNormalinfo.tab_name, nRc);
            return -1;
        }

        if ((1 == hNormalinfo.is_backup) && (nRcbackup != SQLITE_DONE))
        {
            olog_error(db_ocat, "update data failure backup table:%s ret:%d\n", hNormalinfo.tab_name,
                       nRcbackup);
            return -1;
        }
    }
    return S_OK;
}

/**
*********************************************************************
* @brief： Insert普通数据
* @param[in]：
               db       sql句柄
               tab_name  表名
               req      请求消息
* @param[out]：
               rsp      回复消息
* @return： 返回值说明
            0-成功   其他-错误
*********************************************************************
*/
sint32 write_normal_data_mast(sqlite3 *db, sint8 *tab_name, NORMAL_DATA_READ_REQ_T *req, NORMAL_DATA_READ_RSP_T *rsp)
{
    sint32 nRc = -1;
    // DB_TAB_NORMAL_INFO_T hNormalinfo = {0};
    sint8 sqlstr[256]   = {0};
    sqlite3_stmt *pStmt = NULL;  //预编译SQL
//    struct timespec currenttime;

    // CRC16
    uint16 crc = CRC16_modebus(rsp->outBuf, rsp->bufLen);
    olog_debug(db_ocat, "crc[%d]", crc);
    if (db != NULL)
    {
        snprintf(sqlstr, sizeof(sqlstr),
                 "REPLACE INTO '%s' VALUES ('%u', '%u', '%u', '%d' ,? , datetime('now','localtime'), '%d')", tab_name,
                 req->logicalDeviceNum, req->infoNum, req->dataTag, rsp->dataTypeID, crc);
        
#ifdef SQL_DUMP_DEBUG
        olog_debug(db_ocat, "SQL:[%s],blobdata:", sqlstr);
#endif
        //预编译SQL,生成pStmt
        nRc = sqlite3_prepare_v2(db, sqlstr, -1, &pStmt, 0);
        if (nRc != SQLITE_OK)
        {
            olog_error(db_ocat, "write_normal_data_mast sqlite3_prepare err \n");
            return nRc;
        }
        //绑定第1个(函数的第二个参数)blob数据
        nRc = sqlite3_bind_blob(pStmt, 1, rsp->outBuf, (sint32)(rsp->bufLen), NULL);  // 1 is a Index,  from 1
        if (nRc == SQLITE_OK)
        {
            // prepare/bind执行成功的话,执行step，INSERT/UPDATE如果执行成功则为SQLITE_DONE
//            clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//            olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], START", __FUNCTION__, sqlstr, currenttime.tv_sec, currenttime.tv_nsec);
            //执行预编译的SQL语句
            nRc = sqlite3_step(pStmt);
//            clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//            olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], END", __FUNCTION__, sqlstr, currenttime.tv_sec, currenttime.tv_nsec);
            if (nRc != SQLITE_DONE)
            {
                olog_warn(db_ocat, "write_normal_data_mast:sql:%s nRc = %d", sqlstr, nRc);
                nRc = -1;
            }
            else
            {
                nRc = 0;
            }
        }
        else
        {
            olog_debug(db_ocat, "write_normal_data_mast sqlite3_bind_blob err \n");
        }

        sqlite3_finalize(pStmt);  //释放预编译SQL语句
    }
    return nRc;
}

/**
*********************************************************************
* @brief： 开始读记录型数据
* @param[in]：
               req      写记录型数据结构体
* @param[out]：
               rsp      回复消息内容
* @return： 返回值说明
*********************************************************************
*/
sint32 m_start_read_record_data(RECORD_DATA_START_READ_REQ_T *req, RECORD_DATA_START_READ_RSP_T *rsp)
{
    uint32 nRows                            = 0;
    uint32 i                                = 0;
    qlist_t *pSelectList                    = NULL;
    uint8 keyname[64]                       = {0};
    sint8 buff[4096]                        = {0};
    sint8 filterparam[32]                   = {0};
    sint8 selectparam[128]                  = {0};
    sint8 orderparam[128]                   = {0};
    sint8 minorparam[2048]                  = {0};
    sint8 sqlbuf[64]                        = {0};
    sint8 *sortType                         = NULL;
    sint8 *normal_table_orderparam          = NULL;
    STEP_READ_RECORD_INFO_T *pReadrcordinfo = NULL;

    pReadrcordinfo = (STEP_READ_RECORD_INFO_T *)malloc(sizeof(STEP_READ_RECORD_INFO_T));
    if (!pReadrcordinfo)
    {
        olog_error(db_ocat, "malloc pReadrcordinfo fail \n");
        return -1;
    }
    // is need QHASHTBL_THREADSAFE?
    pSelectList = qlist(QHASHTBL_THREADSAFE);
    if (!pSelectList)
    {
        olog_error(db_ocat, "malloc pSelectList fail \n");
        free(pReadrcordinfo);
        return -1;
    }

    if (req->majorDI == 0 && (req->queryTimeType == 4 || req->queryTimeType == 255))
    {
        memset(minorparam, 0, sizeof(minorparam));
        for (i = 0; i < req->minorDINum; i++)
        {
            if (i == 0)
            {
                strcat(minorparam, " DataID in(");
            }
            else if (i > 0)
            {
                strcat(minorparam, ",");
            }

            snprintf(sqlbuf, sizeof(sqlbuf), "%u", *(req->minorDIs + i));
            if (strlen(minorparam)+strlen(sqlbuf) > 2048)
            {
                olog_error(db_ocat, "The start read record data message length is invalid minorDINum[%d] is too big!\n", req->minorDINum);
                free(pReadrcordinfo);
				pSelectList->free(pSelectList);
                return -2;
            }
                
            strcat(minorparam, sqlbuf);
            if (i == (req->minorDINum - 1))
            {
                strcat(minorparam, ")");
            }
        }
        switch (req->sortType)  //排序种类
        {
            case 0:
                normal_table_orderparam = "order by InfoAddrNo ASC";
                break;
            case 1:;
                normal_table_orderparam = "order by InfoAddrNo DESC";
                break;
            case 255:
                normal_table_orderparam = "";
                break;
            default:
                normal_table_orderparam = "";
                break;
        }
        
        if(req->queryTimeType == 4)
        {
            memset(selectparam, 0, sizeof(selectparam));
            if (req->startTime == req->endTime)
            {
                snprintf(selectparam, sizeof(selectparam), "and InfoAddrNo = %u", req->startTime);
            }
            else
            {
                snprintf(selectparam, sizeof(selectparam), "and InfoAddrNo >= %u and InfoAddrNo < %u", req->startTime, req->endTime);
            }
        }
        else
        {
            memset(selectparam, 0, sizeof(selectparam));
        }

        if (req->infoNum == 0xffff)
        {
            snprintf(buff, sizeof(buff),
                     "SELECT LDeviceNo,InfoAddrNo,DataID,DataContent FROM %%\163 WHERE LDeviceNo=%d and %s %s %s",
                     req->logicalDeviceNum, minorparam, selectparam, normal_table_orderparam);
        }
        else
        {
            snprintf(buff, sizeof(buff),
                     "SELECT LDeviceNo,InfoAddrNo,DataID,DataContent FROM %%\163 WHERE LDeviceNo=%d and "
                     "InfoAddrNo=%d and %s %s",
                     req->logicalDeviceNum, req->infoNum, minorparam, normal_table_orderparam);
        }
        m_read_all_normal_data(req->logicalDeviceNum, req->infoNum, req->majorDI, req->minorDINum, req->minorDIs, buff,
                               &nRows, pSelectList);
    }

    if (pSelectList->num <= 0)
    {
        switch (req->sortType)  //排序种类
        {
            case 0:
                sortType = "ASC";
                break;
            case 1:
                sortType = "DESC";
                break;
            case 255:
                sortType = "";
                break;
            default:
                sortType = "";
                break;
        }

        switch (req->queryTimeType)  //排序字段采集启动/采集成功/采集保存时间
        {
            case 0:
                strcpy(filterparam, "AcqStartTime");
                snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, AcqSaveTime %s, %s %s", sortType, filterparam,
                         sortType);
                break;
            case 1:
                strcpy(filterparam, "AcqSuccessTime");
                snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, AcqSaveTime %s, %s %s", sortType, filterparam,
                         sortType);
                break;
            case 2:
                strcpy(filterparam, "AcqSaveTime");
                snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, %s %s", filterparam, sortType);
                break;
            case 3:
                strcpy(filterparam, "No");
                snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, AcqSaveTime %s, %s %s", sortType, filterparam,
                         sortType);
                break;
            case 4:
                strcpy(filterparam, "InfoAddrNo");
                snprintf(orderparam, sizeof(orderparam), "order by %s %s, AcqSaveTime %s", filterparam, sortType, sortType);
                break;
            default:
                strcpy(filterparam, "AcqSaveTime");
                snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, %s %s", filterparam, sortType);
                break;
        }

        if (req->startTime == req->endTime)
        {
            snprintf(selectparam, sizeof(selectparam), "and %s = %u", filterparam, req->startTime);
        }
        else
        {
            snprintf(selectparam, sizeof(selectparam), "and %s >= %u and %s < %u", filterparam, req->startTime, filterparam, req->endTime);
        }

        //将各子DI按SQL条件拼接
        memset(minorparam, 0, sizeof(minorparam));
        for (i = 0; i < req->minorDINum; i++)
        {
            if (i == 0)
            {
                strcat(minorparam, "SubDataDI in(");
            }
            else if (i > 0)
            {
                strcat(minorparam, ",");
            }

            snprintf(sqlbuf, sizeof(sqlbuf), "%u", *(req->minorDIs + i));
            
            if (strlen(minorparam)+strlen(sqlbuf) > 2048)
            {
                olog_error(db_ocat, "The start read record data message length is invalid minorDINum[%d] is too big!\n", req->minorDINum);
                free(pReadrcordinfo);
                pSelectList->free(pSelectList);
                return -2;
            }
            
            strcat(minorparam, sqlbuf);
            if (i == (req->minorDINum - 1))
            {
                strcat(minorparam, ")");
            }
        }

        if (req->queryTimeType == 0 || req->queryTimeType == 1 || req->queryTimeType == 2 || req->queryTimeType == 3)
        {
            if (req->infoNum == 0xffff)
            {
                snprintf(buff, sizeof(buff),
                         "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, "
                         "AcqSaveTime, No, "
                         "AddCheck, DataContent "
                         "FROM %%\163 where %s %s %s;",
                         minorparam, selectparam, orderparam);
            }
            else
            {
                snprintf(buff, sizeof(buff),
                         "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, "
                         "AcqSaveTime, No, "
                         "AddCheck, DataContent "
                         "FROM %%\163 where InfoAddrNo = %u and %s %s %s;",
                         req->infoNum, minorparam, selectparam, orderparam);
            }

            olog_debug(db_ocat, "%s", buff);
        }
        else if (req->queryTimeType == 4)
        {
            snprintf(
                buff, sizeof(buff),
                "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, AcqSaveTime, No, "
                "AddCheck, DataContent "
                "FROM %%\163 where %s %s %s;",
                minorparam, selectparam, orderparam);
        }
        else if (req->queryTimeType == 255)
        {
            if (req->infoNum == 0xffff)
            {
                snprintf(
                    buff, sizeof(buff),
                    "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, AcqSaveTime, "
                    "No, AddCheck, DataContent "
                    "FROM %%\163 where  %s order by InfoAddrNo ASC, AcqSaveTime ASC;",
                    minorparam);
            }
            else
            {
                snprintf(
                    buff, sizeof(buff),
                    "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, AcqSaveTime, "
                    "No, AddCheck, DataContent "
                    "FROM %%\163 where InfoAddrNo = %u  and %s order by InfoAddrNo ASC, AcqSaveTime ASC;",
                    req->infoNum, minorparam);
            }
        }

        if (req->infoNum == 0xffff || req->queryTimeType == 4)
        {
            if (req->sortType == 1)
            {
                m_read_record_data(req->logicalDeviceNum, 1, req->majorDI, buff, &nRows, pSelectList);
                m_read_record_data(req->logicalDeviceNum, 0, req->majorDI, buff, &nRows, pSelectList);
            }
            else
            {
                m_read_record_data(req->logicalDeviceNum, 0, req->majorDI, buff, &nRows, pSelectList);
                m_read_record_data(req->logicalDeviceNum, 1, req->majorDI, buff, &nRows, pSelectList);
            }
        }
        else
        {
            m_read_record_data(req->logicalDeviceNum, req->infoNum, req->majorDI, buff, &nRows, pSelectList);
        }
    }

    if (pSelectList->num > 0)
    {
        pReadrcordinfo->handerNum = pSelectList;
        pReadrcordinfo->indexNum  = 0;
        pReadrcordinfo->recordNum = pSelectList->num;
        clock_gettime(CLOCK_MONOTONIC_RAW, &(pReadrcordinfo->time));
        //将保存记录的列表放入hashtab中
        snprintf((char*)keyname, sizeof(keyname), "%X", (uint32)pReadrcordinfo);
        s_psetupReadInfotbl->putint(s_psetupReadInfotbl, (const char*)keyname, (uint32)pReadrcordinfo);

        olog_debug(db_ocat, "create handler 0x%x \n", (uint32)pReadrcordinfo);
        //设定返回消息的handle和记录总数
        rsp->handleNum = (uint32)pReadrcordinfo;
        rsp->recordNum = pReadrcordinfo->recordNum;
    }
    else
    {
        free(pReadrcordinfo);
        pSelectList->free(pSelectList);
        rsp->handleNum = 0;
        rsp->recordNum = 0;
    }

    return 0;
}

/**
*********************************************************************
* @brief： 操作单步读记录型数据
* @param[in]：
               req       读记录型数据输入参数
               subp      回复内容Buffer
* @param[out]：
               rsp       记录型数据响应
               outLen    回复内容长度
               itemSun   记录条数
* @return： 返回值说明
*********************************************************************
*/
sint32 m_step_read_record(READ_RECORD_STEP_REQ_T *req,
                          READ_RECORD_STEP_RSP_T *rsp,
                          uint8 *subp,
                          sint32 *outLen,
                          sint16 *itemSun)
{
    int ret             = -1;
    sint8 keyHandle[32] = {0};
    uint32 totalSize    = 0;
    uint32 recordLen = MAX_PAYLOAD_LEN-MAX_STEP_HEADER;
    qlist_t *pqList     = NULL;
    // qhashtbl_t *ptable                     = NULL;
    qlist_t *pSelectList = NULL;
    STEP_READ_RECORD_INFO_T *phRecordinfo  = NULL;
    READ_RECORD_STEP_RSP_T *pRecordstepRsp = NULL;
    uint8 *temsubp                        = subp;

    snprintf((char*)keyHandle, sizeof(keyHandle), "%X", (uint32)(req->handerNum));
	int64_t temp = s_psetupReadInfotbl->getint(s_psetupReadInfotbl, (const char*)keyHandle);
	memcpy(&phRecordinfo, &temp, sizeof(STEP_READ_RECORD_INFO_T *));
//    phRecordinfo = (STEP_READ_RECORD_INFO_T *)((uint64)(s_psetupReadInfotbl->getint(s_psetupReadInfotbl, (char*)keyHandle)));
    if (NULL == phRecordinfo)
    {
        olog_warn(db_ocat, "phRecordinfo is NULL;req->handerNum=%X\n", req->handerNum);
        return -1;
    }
    //获得单步读的handle
    pSelectList = phRecordinfo->handerNum;

    if (NULL != pSelectList)
    {
        // qlist_t *pList = NULL;
        size_t s = 0;
        char *p  = pSelectList->popfirst(pSelectList, &s);
        if (p != NULL)
        {
            memcpy(&pqList, p, s);
            free(p);
        }
        else
        {
            olog_info(db_ocat, "s_psetupReadInfotbl record handle:[%x] is NULL", req->handerNum);
        }

        olog_debug(db_ocat, "keyHandle:%s", keyHandle);

        clock_gettime(CLOCK_MONOTONIC_RAW, &(phRecordinfo->time));
        if (NULL != pqList)
        {
            qlist_obj_t obj;
            sint32 first = 0;
            *itemSun     = pqList->num;
            memset((void *)&obj, 0, sizeof(obj));  // must be cleared before call
            while (pqList->getnext(pqList, &obj, false) == true)
            {
                // printf("DATA=%s, SIZE=%zu\n", (char *)obj.data, obj.size);
                memcpy(&pRecordstepRsp, obj.data, sizeof(pRecordstepRsp));
                olog_debug(db_ocat, "pRecordstepRsp [%u][%0x][%0x]", pRecordstepRsp->infoNum, pRecordstepRsp->majorDI,
                           pRecordstepRsp->minorDI);

                olog_debug(db_ocat, "pRecordstepRsp->checkInfoLen =%d \n", pRecordstepRsp->checkInfoLen);
                if (pRecordstepRsp->checkInfoLen == 0 || req->checkInfoLen == 0
                    || ((pRecordstepRsp->checkInfoLen == req->checkInfoLen)
                        && (0 == memcmp(pRecordstepRsp->checkInfo, req->checkInfo, req->checkInfoLen))))
                {
                	if (first == 0)
                    {
                        rsp->startTime   = pRecordstepRsp->startTime;
                        rsp->storageTime = pRecordstepRsp->storageTime;
                        rsp->endTime     = pRecordstepRsp->endTime;
                        rsp->infoNum     = pRecordstepRsp->infoNum;
                        rsp->timeFlag    = pRecordstepRsp->timeFlag;
                        first            = 1;
                    }
					
                     //数据长度计算, OPTIONAL+次数据DI+长度+数据
                    totalSize = (1+4+pRecordstepRsp->outBufLen+1);

                    //+主数据DI
                    if (pRecordstepRsp->majorDI == 1)
                    {
                        totalSize += 4;
						

                    }
                    //长度+2 
                    if (pRecordstepRsp->outBufLen >= 0x80)
                    {
                        totalSize += 2;

                    }

                    if (temsubp - subp + totalSize >= recordLen)
                    {
                        ret = -1;
                        olog_error(db_ocat, "The size exceed the max payload len");

                    }
                    else
                    {
                    	if (pRecordstepRsp->majorDI == 0)
                        {
                            push_u8(&temsubp, _OPTIONSL_FALSE);
                        }
                        else
                        {
                            push_u8(&temsubp, _OPTIONSL_TRUE);
                            push_u32(&temsubp, pRecordstepRsp->majorDI);
                        }
						
                        push_u32(&temsubp, pRecordstepRsp->minorDI);
                        push_nbyte(&temsubp, pRecordstepRsp->outBuf, pRecordstepRsp->outBufLen);
                        ret = 0;
                    }
                } 
				else 
				{
                    
                    ret = -1;
                }

                free(pRecordstepRsp);
            }
			pqList->clear(pqList);
            pqList->free(pqList);
        }
        *outLen = temsubp - subp;
        olog_debug(db_ocat, "*outLen =%d \n", *outLen);
    }
    else
    {
        olog_error(db_ocat, "handlerNum is null");
        ret = -1;
    }

    return ret;
}

/**
*********************************************************************
* @brief： 操作停止读记录型数据
* @param[in]：
               req       读记录型数据输入参数
* @param[out]：
               none
* @return： 0     success
            -1    fail
*********************************************************************
*/
sint32 m_stop_read_record_data(RECORD_DATA_STOP_READ_REQ_T *req)
{
    //qlist_t *pList                         = NULL;
    STEP_READ_RECORD_INFO_T *phRecordinfo  = NULL;
    //READ_RECORD_STEP_RSP_T *pRecordstepRsp = NULL;
    uint8 keyname[20]                      = {0};
    //qlist_obj_t obj                        = {0};

    snprintf((char*)keyname, sizeof(keyname), "%X", req->handleNum);
    //printf("recevie handler %u \n", req->handleNum);
    // s_psetupReadInfotbl->debug(s_psetupReadInfotbl, stdout);

    s_psetupReadInfotbl->lock(s_psetupReadInfotbl);
    //获取单步读信息
    int64_t temp = s_psetupReadInfotbl->getint(s_psetupReadInfotbl, (const char*)keyname);
	memcpy(&phRecordinfo, &temp, sizeof(STEP_READ_RECORD_INFO_T *));
//    phRecordinfo = (STEP_READ_RECORD_INFO_T*)((uint64)(s_psetupReadInfotbl->getint(s_psetupReadInfotbl, (const char*)keyname)));
    if (phRecordinfo != NULL)
    {
        //释放单步读句柄
        free_read_handler(phRecordinfo->handerNum);
        free(phRecordinfo);
        //从单步读hashtable中移除
        s_psetupReadInfotbl->remove(s_psetupReadInfotbl, (const char*)keyname);
        s_psetupReadInfotbl->unlock(s_psetupReadInfotbl);
        return 0;
    }
    olog_debug(db_ocat, "handlerNum [%u] is unreachable!", req->handleNum);
    s_psetupReadInfotbl->unlock(s_psetupReadInfotbl);
    return -1;
}

/**
*********************************************************************
* @brief： 单条写记录型数据
* @param[in]：
               req       读记录型数据输入参数
* @param[out]：
               rsp       记录型数据响应
* @return： 返回值说明
*********************************************************************
*/
sint32 m_write_record_data(RECORD_DATA_WRITE_REQ_T *req, RECORD_DATA_WRITE_RSP_T *rsp)
{
    sint32 nRet                      = -1;
    DB_TAB_RECORD_INFO_T hRecordinfo = {0};   //记录型数据情报句柄
    sqlite3_stmt *pStmt              = NULL;  //预编译对象
    char *err_msg                    = NULL;

    //获取表的handle
    get_collect_tab_info(req->logicalDeviceNum, req->infoNum, req->majorDI, 0, &hRecordinfo, true);

    if (hRecordinfo.handle != NULL)
    {
        sint8 table_name[64]   = {0};
        sint8 sqlstr[512]      = {0};
        sint8 sqlstr_cond[256] = {0};
        sint32 nNo             = 0;
	
        snprintf(table_name, sizeof(table_name),"%s%d", hRecordinfo.tab_name, req->dataTypeID);
                 
        do
        {
            //执行SQL语句
            nRet = sqlite3_exec(hRecordinfo.handle->db_writehander_get(hRecordinfo.handle), "BEGIN IMMEDIATE;", NULL,
                                NULL, &err_msg);
            if (nRet == SQLITE_BUSY)
            {
                olog_warn(db_ocat, "Getting BEGIN IMMEDIATE... \n");
                usleep(100);
            }

            if (err_msg != NULL)
            {
                olog_warn(db_ocat, "m_write_record_data error return[%d] err_msg[%s] \n", nRet, err_msg);
                sqlite3_free(err_msg);
                err_msg = NULL;
            }

        } while (nRet == SQLITE_BUSY);

#ifdef AREA_FUJIAN

		sint8 addCheck[36]               = {0};
		memset(addCheck, 0, sizeof(addCheck));
		addr_trans_format(req->checkInfo, req->checkInfoLen, addCheck, sizeof(addCheck));
		
        snprintf(sqlstr_cond, sizeof(sqlstr_cond),
                 "where InfoAddrNo='%u' and SubDataDI='%u' and DataTypeID='%d' and AcqStartTime='%u' and "
                 "AcqSuccessTime='%u' and AcqSaveTime='%u' and AddCheck='%s';",
                 req->infoNum, req->minorDI, req->dataTypeID, req->startTime, req->endTime, req->storageTime, addCheck);
		//查询表中是否有与sqlstr_cond相同的内容
        nNo = m_get_recoder_no(hRecordinfo.handle->db_readhander_get(hRecordinfo.handle), table_name, sqlstr_cond);
		//表中有类似的内容，则进行update操作
        if (nNo > 0)
        {
            snprintf(sqlstr, sizeof(sqlstr), "UPDATE %s SET AddCheck='%s' , DataContent=? %s",
                     table_name, addCheck, sqlstr_cond);
        }
        else
        {
            //表中没有类似的内容，则进行insert操作
            snprintf(sqlstr, sizeof(sqlstr), "INSERT INTO %s VALUES (NULL, %u, %u, %u, %u, %d, %u, %u, %u, '%s', ?)",
                     table_name, req->logicalDeviceNum, req->infoNum, req->majorDI, req->minorDI,
                     req->dataTypeID, req->startTime, req->endTime, req->storageTime, addCheck);
        }

        do
        {
            sqlite3 *db = hRecordinfo.handle->db_writehander_get(hRecordinfo.handle);
            nRet        = sqlite3_prepare_v2(db, sqlstr, -1, &pStmt, NULL);  //准备语句
            if (nRet != SQLITE_OK)
            {
                olog_error(db_ocat, "prepare fail nRet[%d], sql[%s]\n", nRet, sqlstr);
                break;
            }
            
            //绑定预编译SQL语句中的BLOB类型，以？为占位符，第二个参数是占位符的索引值
            nRet = sqlite3_bind_blob(pStmt, 1, req->outBuf, req->outBufLen, NULL);
            if (nRet != SQLITE_OK)
            {
                olog_error(db_ocat, "Data Bind Faild \n");
                break;
            }

			//执行预编译的SQL语句
            nRet = step_db(pStmt);
            if (nRet != SQLITE_DONE)
            {
                olog_debug(db_ocat, "m_write_record_data SQL Step Exce Fail ret:%d ;%s\n", nRet, err_msg);
                //销毁预编译sql语句
                sqlite3_finalize(pStmt);
                rsp->storageIndex = 0;
                break;
            }
            //返回最近一次成功插入数据的ROWID
            if (nNo > 0)
            {
                rsp->storageIndex = nNo;
            }
            else
            {
                rsp->storageIndex = sqlite3_last_insert_rowid(db);
            }
            // olog_debug(db_ocat,"rsp->storageIndex = %d", rsp->storageIndex);
            //销毁预编译sql语句
            nRet = sqlite3_finalize(pStmt);
        } while (0);
#else
		snprintf(sqlstr_cond, sizeof(sqlstr_cond),
                 "where InfoAddrNo = '%u' and SubDataDI = '%u' and DataTypeID='%d' and AcqStartTime = '%u' and "
                 "AcqSuccessTime= '%u' and AcqSaveTime='%u';",
                 req->infoNum, req->minorDI, req->dataTypeID, req->startTime, req->endTime, req->storageTime);
		//查询表中是否有与sqlstr_cond相同的内容
        nNo = m_get_recoder_no(hRecordinfo.handle->db_readhander_get(hRecordinfo.handle), table_name, sqlstr_cond);
        //表中有类似的内容，则进行update操作
        if (nNo > 0)
        {
            snprintf(sqlstr, sizeof(sqlstr), "UPDATE %s SET AddCheck=? , DataContent=? %s",
                     table_name, sqlstr_cond);
        }
        else
        {
            //表中没有类似的内容，则进行insert操作
            snprintf(sqlstr, sizeof(sqlstr), "INSERT INTO %s VALUES (NULL, %u, %u, %u, %u, %d, %u, %u, %u, ?, ?)",
                     table_name, req->logicalDeviceNum, req->infoNum, req->majorDI, req->minorDI,
                     req->dataTypeID, req->startTime, req->endTime, req->storageTime);
        }

        do
        {
            sqlite3 *db = hRecordinfo.handle->db_writehander_get(hRecordinfo.handle);
            nRet        = sqlite3_prepare_v2(db, sqlstr, -1, &pStmt, NULL);  //准备语句
            if (nRet != SQLITE_OK)
            {
                olog_error(db_ocat, "prepare fail nRet[%d], sql[%s]\n", nRet, sqlstr);
                break;
            }
            //绑定预编译SQL语句中的BLOB类型，以？为占位符，第二个参数是占位符的索引值
            nRet = sqlite3_bind_blob(pStmt, 1, req->checkInfo, req->checkInfoLen, NULL);
            if (nRet != SQLITE_OK)
            {
                olog_error(db_ocat, "Check Information Bind Fail \n");
                break;
            }
            //绑定预编译SQL语句中的BLOB类型，以？为占位符，第二个参数是占位符的索引值
            nRet = sqlite3_bind_blob(pStmt, 2, req->outBuf, req->outBufLen, NULL);
            if (nRet != SQLITE_OK)
            {
                olog_error(db_ocat, "Data Bind Faild \n");
                break;
            }

			//执行预编译的SQL语句
            nRet = step_db(pStmt);
            if (nRet != SQLITE_DONE)
            {
                olog_debug(db_ocat, "m_write_record_data SQL Step Exce Fail ret:%d ;%s\n", nRet, err_msg);
                //销毁预编译sql语句
                sqlite3_finalize(pStmt);
                rsp->storageIndex = 0;
                break;
            }
            //返回最近一次成功插入数据的ROWID
            if (nNo > 0)
            {
                rsp->storageIndex = nNo;
            }
            else
            {
                rsp->storageIndex = sqlite3_last_insert_rowid(db);
            }
            // olog_debug(db_ocat,"rsp->storageIndex = %d", rsp->storageIndex);
            //销毁预编译sql语句
            nRet = sqlite3_finalize(pStmt);
        } while (0);
#endif                               

        //执行SQL语句
        sqlite3_exec(hRecordinfo.handle->db_writehander_get(hRecordinfo.handle), "COMMIT;", NULL, NULL, NULL);

        put_collect_tab_info(req->logicalDeviceNum, req->infoNum, req->majorDI, 0);
        if (hRecordinfo.tab_status == ENUM_TABLE_TEMP) {
            db_close(hRecordinfo.handle);
        }

    } else {
        olog_warn(db_ocat, "The hRecordinfo.handle is NULL");
    }
    return nRet;
}

/**
*********************************************************************
* @brief： 操作更新记录型数据
* @param[in]：
               req       读记录型数据输入参数
* @param[out]：
               none
* @return： 返回值说明
            -1           失败
            SQLITE_DONE  成功
*********************************************************************
*/
sint32 m_update_record_data(RECORD_DATA_UPDATE_REQ_T *req)
{
    sint32 nRet                      = -1;  //执行结果
    sint32 rc                        = -1;  //返回结果
    DB_TAB_RECORD_INFO_T hRecordinfo = {0};
    sqlite3_stmt *pStmt              = NULL;
	sint8 talbe_name[64]             = {0};
#if AREA_FUJIAN
		sint8 data_type 				 = ENUM_INIT_PARAM;
#else
		sint8 data_type 				 = ENUM_NORMAL_DATA;
#endif

    get_collect_tab_info(req->logicalDeviceNum, req->infoNum, req->majorDI, 0, &hRecordinfo, true);
    if (hRecordinfo.handle != NULL)
    {
        sint8 sqlstr[256] = {0};
        sqlite3 *pDb = hRecordinfo.handle->db_writehander_get(hRecordinfo.handle);
        while(data_type <= ENUM_NOT_INIT_DATA)
        {
        	snprintf(talbe_name, sizeof(talbe_name), "%s%d", hRecordinfo.tab_name, data_type);
            data_type++;
			
 #if AREA_FUJIAN
 			sint8 addCheck[36]				 = {0};
 			memset(addCheck, 0, sizeof(addCheck));
			addr_trans_format(req->checkInfo, req->checkInfoLen, addCheck, sizeof(addCheck));
			
			switch (req->screenType)
            {
                case 1:
                    snprintf(sqlstr, sizeof(sqlstr),
                             "UPDATE %s SET DataContent=? WHERE LDeviceNo='%u' and SubDataDI='%u' \
                        and AcqSaveTime='%u' and InfoAddrNo='%u' and AddCheck='%s'",
                             talbe_name, req->logicalDeviceNum, req->minorDI, req->storageTime, req->infoNum, addCheck);
                    break;
                case 2:
                    snprintf(
                        sqlstr, sizeof(sqlstr), "UPDATE %s SET DataContent=? WHERE LDeviceNo='%u' and SubDataDI='%u' \
                        and No='%u' and InfoAddrNo='%u' and AddCheck='%s'",
                        talbe_name, req->logicalDeviceNum, req->minorDI, req->storageIndex, req->infoNum, addCheck);
                    break;
                default:
                    olog_debug(db_ocat, "m_update_record_data Error Screen Type0 \n");
                    return nRet;
                    break;
            }
            olog_debug(db_ocat, "sql:[%s]", sqlstr);
            
            //预编译SQL语句
            nRet = sqlite3_prepare_v2(pDb, sqlstr, -1, &pStmt, 0);
            if (nRet != SQLITE_OK)
            {
                olog_debug(db_ocat, "m_update_record_data prepare fail nRet%d sql:[%s]\n", nRet, sqlstr);
                //销毁预编译sql语句
                sqlite3_finalize(pStmt);
                nRet = -1;
                continue;
            }
            //绑定预编译SQL语句中的BLOB类型，以？为占位符，第二个参数是占位符的索引值
            nRet = sqlite3_bind_blob(pStmt, 1, req->outBuf, req->outBufLen, NULL);  // 1 is a Index, counted from 1
            if (nRet != SQLITE_OK)
            {
                olog_error(db_ocat, "m_update_record_data Data Bind Faild \n");
                //销毁预编译sql语句
                sqlite3_finalize(pStmt);
                nRet = -1;
                break;
            }
#else
			
			switch (req->screenType)
            {
                case 1:
                    snprintf(sqlstr, sizeof(sqlstr),
                             "UPDATE %s SET DataContent=? WHERE LDeviceNo='%u' and SubDataDI='%u' \
                        and AcqSaveTime='%u' and InfoAddrNo='%u' and AddCheck=?",
                             talbe_name, req->logicalDeviceNum, req->minorDI, req->storageTime, req->infoNum);
                    break;
                case 2:
                    snprintf(
                        sqlstr, sizeof(sqlstr), "UPDATE %s SET DataContent=? WHERE LDeviceNo='%u' and SubDataDI='%u' \
                        and No='%u' and InfoAddrNo='%u' and AddCheck=?",
                        talbe_name, req->logicalDeviceNum, req->minorDI, req->storageIndex, req->infoNum);
                    break;
                default:
                    olog_debug(db_ocat, "m_update_record_data Error Screen Type0 \n");
                    return nRet;
                    break;
            }
            olog_debug(db_ocat, "sql:[%s]", sqlstr);
            
            //预编译SQL语句
            nRet = sqlite3_prepare_v2(pDb, sqlstr, -1, &pStmt, 0);
            if (nRet != SQLITE_OK)
            {
                olog_debug(db_ocat, "m_update_record_data prepare fail nRet%d sql:[%s]\n", nRet, sqlstr);
                //销毁预编译sql语句
                sqlite3_finalize(pStmt);
                nRet = -1;
                continue;
            }
            //绑定预编译SQL语句中的BLOB类型，以？为占位符，第二个参数是占位符的索引值
            nRet = sqlite3_bind_blob(pStmt, 1, req->outBuf, req->outBufLen, NULL);  // 1 is a Index, counted from 1
            if (nRet != SQLITE_OK)
            {
                olog_error(db_ocat, "m_update_record_data Data Bind Faild \n");
                //销毁预编译sql语句
                sqlite3_finalize(pStmt);
                nRet = -1;
                break;
            }
            //绑定预编译SQL语句中的BLOB类型，以？为占位符，第二个参数是占位符的索引值
            nRet = sqlite3_bind_blob(pStmt, 2, req->checkInfo, req->checkInfoLen, NULL);
            if (nRet != SQLITE_OK)
            {
                olog_debug(db_ocat, "m_update_record_data Data Bind Faild \n");
                //销毁预编译sql语句
                sqlite3_finalize(pStmt);
                nRet = -1;
                break;
            }
#endif
            //执行预编译的SQL语句
            nRet = sqlite3_step(pStmt);

            if (nRet != SQLITE_DONE)
            {
                olog_debug(db_ocat, "m_update_record_data SQL Step Exce Fail \n");
                nRet = -1;
            }
            else
            {
                rc = SQLITE_OK;
                //查询有多少记录受到影响
//                nRet = sqlite3_changes(pDb);
            }
            //销毁预编译sql语句
            sqlite3_finalize(pStmt);
        } 

        put_collect_tab_info(req->logicalDeviceNum, req->infoNum, req->majorDI, 0);
        if (hRecordinfo.tab_status == ENUM_TABLE_TEMP) {
            db_close(hRecordinfo.handle);
        }
    }
    return rc;
}

/**
*********************************************************************
* @brief： 操作获取存储记录条数
* @param[in]：
               req       请求
* @param[out]：
               rsp       响应
* @return： 返回值说明
*********************************************************************
*/
sint32 m_get_storage_num(STORAGE_NUMBER_GET_REQ_T *req, STORAGE_NUMBER_GET_RSP_T *rsp)
{
    DB_TAB_RECORD_INFO_T tab_info = {0};
    DB_TAB_RECORD_INFO_T *pinfo   = &tab_info;
    sint8 sqlcondition[512]       = {0};
    sint8 strfilter[512]          = {0};
    sint32 nCount                 = 0;

    snprintf(sqlcondition, sizeof(sqlcondition), "where SubDataDI='%u' ",req->minorDI);

    if(req->infoNum != 0xFFFF)
    {
        snprintf(strfilter, sizeof(strfilter), " and InfoAddrNo='%u' ", req->infoNum);
        strncat(sqlcondition, strfilter, sizeof(sqlcondition));
    }

    if (req->screenType == SCREEN_TYPE_STORAGE_TIME)
    {
        snprintf(strfilter, sizeof(strfilter), " and AcqSaveTime >= '%u' and AcqSaveTime <= '%u'",
                 req->storageStartTime, req->storageEndTime);
        strncat(sqlcondition, strfilter, sizeof(sqlcondition));
    }

    if(req->infoNum != 0xFFFF)
    {
        get_collect_tab_info(req->logicalDeviceNum, req->infoNum, req->majorDI, req->minorDI, pinfo, true);
        if (pinfo->handle == NULL)
        {
            olog_warn(db_ocat, "pinfo->handle == NULL ;req->majorDI = %u", req->majorDI);
            return -1;
        }
        nCount += m_get_recoder_count(pinfo->handle->db_readhander_get(pinfo->handle), pinfo->tab_name, sqlcondition);
        put_collect_tab_info(req->logicalDeviceNum, req->infoNum, req->majorDI, req->minorDI);
        if (pinfo->tab_status == ENUM_TABLE_TEMP) {
            db_close(pinfo->handle);
        }
    }
    else
    {
        get_collect_tab_info(req->logicalDeviceNum, 0, req->majorDI, req->minorDI, pinfo, true);
        if (pinfo->handle == NULL)
        {
            olog_warn(db_ocat, "pinfo->handle == NULL ;req->majorDI = %u", req->majorDI);
        }
        else
        {
            nCount +=
                m_get_recoder_count(pinfo->handle->db_readhander_get(pinfo->handle), pinfo->tab_name, sqlcondition);
            put_collect_tab_info(req->logicalDeviceNum, 0, req->majorDI, req->minorDI);
            if (pinfo->tab_status == ENUM_TABLE_TEMP) {
                db_close(pinfo->handle);
            }
            
        }
        
        get_collect_tab_info(req->logicalDeviceNum, 1, req->majorDI, req->minorDI, pinfo, true);
        if (pinfo->handle == NULL)
        {
            olog_warn(db_ocat, "pinfo->handle == NULL ;req->majorDI = %u", req->majorDI);
        }
        else
        {
            nCount +=
                m_get_recoder_count(pinfo->handle->db_readhander_get(pinfo->handle), pinfo->tab_name, sqlcondition);
            put_collect_tab_info(req->logicalDeviceNum, 1, req->majorDI, req->minorDI);
            if (pinfo->tab_status == ENUM_TABLE_TEMP) {
                db_close(pinfo->handle);
            }
        }
    }
    
    olog_debug(db_ocat, "nCount = %u", nCount);

    rsp->recordNum = nCount;

    return 0;
}

/**
*********************************************************************
* @brief： 操作获取存储记录深度
* @param[in]：
               req       请求
* @param[out]：
               rsp       响应
* @return： 返回值说明
*********************************************************************
*/
sint32 m_get_depth_num(STORAGE_DEPTH_GET_REQ_T *req, STORAGE_DEPTH_GET_RSP_T *rsp)
{
    sint32 nRet     = -1;  //执行结果
    uint32 depthNum = 0;

    if (req != NULL && rsp != NULL)
    {
        //判断是否条件是否包涵次DI
//        if (0 == req->minorDIOptinal)
//        {
            //使用主DI查找记录深度
//            olog_debug(db_ocat, "just use major DI to get the depth");
//            depthNum = get_depth_value_pridi(req->majorDI);
//        }
//        else
        {
            //使用主DI和次DI查找记录深度
            olog_debug(db_ocat, "use major DI and minorDI to get depth");
            depthNum = get_depth_value(req->majorDI, req->minorDI);
        }
        nRet = 0;
    }
    rsp->depthNum = depthNum;
    return nRet;
}

/**
*********************************************************************
* @brief： 操作数据初始化2021-5-30 新规约接口
* @param[in]：
               req       请求消息
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 m_data_init_new(DB_INIT_REQ_T *req)
{
    int rc;

    rc = data_init_logi_dev(req->logicalDeviceNum, req->optinal, req->dataTypeID, 0);

    //数据/参数删除
    rc |= gen_param_data_init_logi_dev(req->logicalDeviceNum, 0, req->optinal, req->dataTypeID, false);

    return rc;
}


/**
*********************************************************************
* @brief： 操作指定数据初始化
* @param[in]：
               req       参数请求
               needSpaceReclaim  是否空间回收
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 m_data_init_by_parameter(DATA_INIT_REQ_T *req, uint8 needSpaceReclaim)
{
    //
    if (req->choice == 1)
    {
        //指定数据项标识删除
        return spec_data_del(req->logicalDeviceNum, req->infoNum, req->dataTypeID, req->majorDI, req->minorDI, needSpaceReclaim);
    }
    else if (req->choice == 0)
    {
        //指定数据类型删除
        return data_init_logi_dev_info_number(req->logicalDeviceNum, req->infoNum, req->dataTypeID, false);
    }
    else
    {
        olog_error(db_ocat, "init db error choice[%d]", req->choice);
    }

    return -1;
}

/**
*********************************************************************
* @brief： 操作恢复出厂设置
* @param[in]：
               logicalDeviceNum       逻辑设备号
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 m_factory_Reset_new(uint8 logicalDeviceNum)
{
    int rc;
    //删除记录型数据
    rc = data_init_logi_dev(logicalDeviceNum, 0, 0, 1);
    //数据/参数删除
    rc |= gen_param_data_init_logi_dev(logicalDeviceNum, 1, 0, 0, false);

    return rc;
}

#ifdef AREA_FUJIAN
/**
*********************************************************************
* @brief： 开始读记录型数据,福建集中器扩展接口
* @param[in]：
               req      写记录型数据结构体
* @param[out]：
               rsp      回复消息内容
* @return： 返回值说明
*********************************************************************
*/
sint32 m_start_read_record_data_fj(RECORD_DATA_START_READ_REQ_T *req, RECORD_DATA_START_READ_RSP_T *rsp)
{
    uint32 nRows                            = 0;
    uint32 i                                = 0;
    qlist_t *pSelectList                    = NULL;
    uint8 keyname[64]                       = {0};
	char addCheck[36]                       = {0};
    sint8 buff[4096]                        = {0};
    sint8 filterparam[32]                   = {0};
    sint8 selectparam[128]                  = {0};
    sint8 orderparam[128]                   = {0};
    sint8 minorparam[2048]                  = {0};
	sint8 addrparam[64]                  	= {0};
    sint8 sqlbuf[64]                        = {0};
    sint8 *sortType                         = NULL;
    STEP_READ_RECORD_INFO_T *pReadrcordinfo = NULL;

    pReadrcordinfo = (STEP_READ_RECORD_INFO_T *)malloc(sizeof(STEP_READ_RECORD_INFO_T));
    if (!pReadrcordinfo)
    {
        olog_error(db_ocat, "malloc pReadrcordinfo fail \n");
        return -1;
    }
    // is need QHASHTBL_THREADSAFE?
    pSelectList = qlist(QHASHTBL_THREADSAFE);
    if (!pSelectList)
    {
        olog_error(db_ocat, "malloc pSelectList fail \n");
        free(pReadrcordinfo);
        return -1;
    }

	switch (req->sortType)  //排序种类
    {
        case 0:
            sortType = "ASC";
            break;
        case 1:
            sortType = "DESC";
            break;
        case 255:
            sortType = "";
            break;
        default:
            sortType = "";
            break;
    }

    switch (req->queryTimeType)  //排序字段采集启动/采集成功/采集保存时间
    {
        case 0:
            strcpy(filterparam, "AcqStartTime");
            snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, AcqSaveTime %s, %s %s", sortType, filterparam,
                     sortType);
            break;
        case 1:
            strcpy(filterparam, "AcqSuccessTime");
            snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, AcqSaveTime %s, %s %s", sortType, filterparam,
                     sortType);
            break;
        case 2:
            strcpy(filterparam, "AcqSaveTime");
            snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, %s %s", filterparam, sortType);
            break;
        case 3:
            strcpy(filterparam, "No");
            snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, AcqSaveTime %s, %s %s", sortType, filterparam,
                     sortType);
            break;
        case 4:
            strcpy(filterparam, "InfoAddrNo");
            snprintf(orderparam, sizeof(orderparam), "order by %s %s, AcqSaveTime %s", filterparam, sortType, sortType);
            break;
        default:
            strcpy(filterparam, "AcqSaveTime");
            snprintf(orderparam, sizeof(orderparam), "order by InfoAddrNo ASC, %s DESC", filterparam);
            break;
    }

	if(req->queryTimeType != 0xff)
	{
        if (req->startTime == req->endTime)
	    {
	        snprintf(selectparam, sizeof(selectparam), "%s = %u", filterparam, req->startTime);
	    }
	    else
	    {
	        snprintf(selectparam, sizeof(selectparam), "%s >= %u and %s < %u", filterparam, req->startTime, filterparam, req->endTime);
	    }
    }

	if (req->addrLen != 0)
    {
    	memset(addCheck, 0, sizeof(addCheck));
		addr_trans_format(req->pAddr, req->addrLen, addCheck, sizeof(addCheck));
        snprintf(addrparam, sizeof(addrparam), "and AddCheck='%s'", addCheck);
    }
	    

    //将各子DI按SQL条件拼接
    memset(minorparam, 0, sizeof(minorparam));
    for (i = 0; i < req->minorDINum; i++)
    {
        if (i == 0)
        {
            strcat(minorparam, "and SubDataDI in(");
        }
        else if (i > 0)
        {
            strcat(minorparam, ",");
        }

        snprintf(sqlbuf, sizeof(sqlbuf), "%u", *(req->minorDIs + i));
        
        if (strlen(minorparam)+strlen(sqlbuf) > 2048)
        {
            olog_error(db_ocat, "The start read record data message length is invalid minorDINum[%d] is too big!\n", req->minorDINum);
            free(pReadrcordinfo);
            pSelectList->free(pSelectList);
            return -2;
        }
        
        strcat(minorparam, sqlbuf);
        if (i == (req->minorDINum - 1))
        {
            strcat(minorparam, ")");
        }
    }

	if (req->queryTimeType == 0 || req->queryTimeType == 1 || req->queryTimeType == 2 || req->queryTimeType == 3)
	{
		if (req->infoNum == 0xffff)
		{
			snprintf(buff, sizeof(buff),
	             "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, "
	             "AcqSaveTime, No, AddCheck, DataContent FROM %%\163 where %s %s %s %s;",
	             selectparam, minorparam, addrparam, orderparam);
		}
		else
		{
			snprintf(buff, sizeof(buff),
	             "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, "
	             "AcqSaveTime, No, AddCheck, DataContent FROM %%\163 where InfoAddrNo=%u and %s %s %s %s;",
	             req->infoNum, selectparam, addrparam, minorparam, orderparam);
		}
	}
	else if (req->queryTimeType == 4)
	{
		snprintf(buff, sizeof(buff),
	             "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, "
	             "AcqSaveTime, No, AddCheck, DataContent FROM %%\163 where %s %s %s %s;",
	             selectparam, addrparam, minorparam, orderparam);
	}
	else//req->queryTimeType == 255
	{
		if (req->infoNum == 0xffff)
		{
			if (req->minorDINum == 0)
			{
				snprintf( buff, sizeof(buff),
                    "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, AcqSaveTime, "
                    "No, AddCheck, DataContent FROM %%\163 where PriDataDI=%d %s order by InfoAddrNo ASC, AcqSaveTime ASC;", req->majorDI, addrparam);
			}
			else
			{
				snprintf( buff, sizeof(buff),
                    "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, AcqSaveTime, "
                    "No, AddCheck, DataContent FROM %%\163 where PriDataDI=%d %s %s order by InfoAddrNo ASC, AcqSaveTime ASC;",
                    req->majorDI, addrparam, minorparam);
			}
		}
		else
		{
			snprintf(buff, sizeof(buff),
                "SELECT LDeviceNo, InfoAddrNo, PriDataDI, SubDataDI, AcqStartTime, AcqSuccessTime, AcqSaveTime, "
                "No, AddCheck, DataContent FROM %%\163 where InfoAddrNo = %u %s %s order by InfoAddrNo ASC, AcqSaveTime ASC;",
                req->infoNum, addrparam, minorparam);
		}
	}

	olog_debug(db_ocat, "%s", buff);

	if (req->infoNum == 0xffff || req->queryTimeType == 4)
    {
        if (req->sortType == 1)
        {
            m_read_record_data_fj(req->logicalDeviceNum, 1, req->majorDI, buff, &nRows, pSelectList);
            m_read_record_data_fj(req->logicalDeviceNum, 0, req->majorDI, buff, &nRows, pSelectList);
        }
        else
        {
            m_read_record_data_fj(req->logicalDeviceNum, 0, req->majorDI, buff, &nRows, pSelectList);
            m_read_record_data_fj(req->logicalDeviceNum, 1, req->majorDI, buff, &nRows, pSelectList);
        }
    }
    else
    {
        m_read_record_data_fj(req->logicalDeviceNum, req->infoNum, req->majorDI, buff, &nRows, pSelectList);
    }

    if (pSelectList->num > 0)
    {
        pReadrcordinfo->handerNum = pSelectList;
        pReadrcordinfo->indexNum  = 0;
        pReadrcordinfo->recordNum = pSelectList->num;
        clock_gettime(CLOCK_MONOTONIC_RAW, &(pReadrcordinfo->time));
        //将保存记录的列表放入hashtab中
        snprintf((char*)keyname, sizeof(keyname), "%X", (uint32)pReadrcordinfo);
        s_psetupReadInfotbl->putint(s_psetupReadInfotbl, (const char*)keyname, (uint32)pReadrcordinfo);

        olog_debug(db_ocat, "create handler 0x%x \n", (uint32)pReadrcordinfo);
        //设定返回消息的handle和记录总数
        rsp->handleNum = (uint32)pReadrcordinfo;
        rsp->recordNum = pReadrcordinfo->recordNum;
    }
    else
    {
        free(pReadrcordinfo);
        pSelectList->free(pSelectList);
        rsp->handleNum = 0;
        rsp->recordNum = 0;
    }

    return 0;
}

/**
*********************************************************************
* @brief： 操作指定数据初始化
* @param[in]：
               req       参数请求
               needSpaceReclaim  是否空间回收
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 m_data_init_by_parameter_fj(DATA_INIT_REQ_T_FJ *req)
{
    sint32 rc                     = -1;
    DB_TAB_RECORD_INFO_T tab_info = {0};
    sint8 buf[512]                = {0};
    sint8 *zErrMsg                = NULL;
    DB_RAW_T *tcollect            = NULL;
    uint16 tmp_info_num[2]        = {0};
    uint8 tmp_info_num_count      = 0;
    uint8 i                       = 0;
    sint8 data_type               = ENUM_INIT_PARAM;
    sint8 talbe_name[64]          = {0};
	sint8 strInfo[64]          	  = {0};
	sint8 strSub[64]          	  = {0};
	sint8 strStor[128]            = {0};

    olog_debug(db_ocat, "logi_dev=%u inf_addr_no=%u prim_di=%u sub_di=%u", req->logicalDeviceNum, req->infoNum, req->majorDI, req->minorDI);
    if (req->infoNum != 0xffff)
    {
    	snprintf(strInfo, sizeof(strInfo), "and InfoAddrNo=%u", req->infoNum);
        tmp_info_num[0]    = req->infoNum;
        tmp_info_num_count = 1;
    }
    else
    {
        tmp_info_num[0]    = 0;
        tmp_info_num[1]    = 1;
        tmp_info_num_count = 2;
    }

	if (req->minorDI != 0xffffffff)
    {
    	snprintf(strSub, sizeof(strSub), "and SubDataDI=%u", req->minorDI);
    }

	if (req->queryTimeType != 0xff)
    {
        if (req->storageStartTime == req->storageEndTime)
	    {
	    	snprintf(strStor, sizeof(strStor), "and AcqSaveTime=%u", req->storageStartTime);
	    }
	    else
	    {
	        snprintf(strStor, sizeof(strStor), "and AcqSaveTime>=%u and AcqSaveTime<%u", req->storageStartTime, req->storageEndTime);
	    }
    }

    for (i = 0; i < tmp_info_num_count; i++)
    {
        rc = get_collect_tab_info(req->logicalDeviceNum, tmp_info_num[i], req->majorDI, req->minorDI, &tab_info, true);
        if (tab_info.handle == NULL || tab_info.tab_name == NULL)
        {
            olog_warn(db_ocat, "db_spec_data_del tab_info.handle or tab_name is NULL\n");
        }
        else
        {
        	data_type = ENUM_INIT_PARAM;
            while(data_type <= ENUM_NOT_INIT_DATA)
            {
                tcollect = tab_info.handle;
                snprintf(talbe_name, sizeof(talbe_name), "%s%d", tab_info.tab_name, data_type);
                data_type++;

				//数据库句柄之前是关闭的，检查数据库表是否存在
				if (tab_info.tab_status == ENUM_TABLE_CLOSE)
		        {
		        	uint8 tabEllaxist		= 0;
		            //判断需要删除的库表是否存在
					rc = check_table_isexist(&tab_info, talbe_name, &tabEllaxist);
					if (rc == S_NG)
					{
				        olog_error(db_ocat, "spec_data_del 's check table error");
				        return S_NG;
				    }

					//库表不存在，直接返回，不需要删除
					if (tabEllaxist == 0)
					{
				        olog_debug(db_ocat, "table is not exist, data is null");
				        continue;
				    }
		        }

                snprintf(buf, sizeof(buf), "delete from %s where LDeviceNo=%u %s and PriDataDI=%u %s %s;", talbe_name, req->logicalDeviceNum, strInfo, req->majorDI, strSub, strStor);

                dzlog_debug("sql:[%s]", buf);
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
                if (rc != SQLITE_OK)
                {
                    olog_warn(db_ocat, "spec_data_del Fail sql:%s ** err:%s", buf, zErrMsg);
    		        sqlite3_free(zErrMsg);
                    continue;
                }

                sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);

            }
            put_collect_tab_info(req->logicalDeviceNum, tmp_info_num[i], req->majorDI, req->minorDI);
        }
    }

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    return rc;
}

#endif

/**
*********************************************************************
* @brief： 设置数据更新通知
* @param[in]：
               req       参数请求
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
// sint32 m_set_subscription(SUBSCRIPTION_SET_REQ_T *req);

/**
*********************************************************************
* @brief： 是否存在记录
* @param[in]：
               handle数据情报句柄
               condition：执行SQl的条件语句（where之后的部分）
               len   codition的长度
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
#if 0
sint32 m_is_recoder_exist(DB_TAB_NORMAL_INFO_T *handle, sint8 *condition, sint32 len)
{
    sint8 sqlstr[256]   = {0};
    sqlite3_stmt *pStmt = NULL;
    sint32 count        = 0;
    sint32 nRet         = -1;
    const sint8 *pZtail = NULL;
    sqlite3 *pDb        = NULL;
    struct timespec currenttime;

    snprintf(sqlstr, sizeof(sqlstr), "select COUNT(*) from '%s' where %s", handle->tab_name, condition);
    olog_debug(db_ocat, "sql:[%s]", sqlstr);
    pDb = handle->handle->db_readhander_get(handle->handle);

    nRet = sqlite3_prepare_v2(pDb, sqlstr, -1, &pStmt, &pZtail);
    // olog_debug(db_ocat,"pZtail:[%s]", pZtail);
    olog_debug(db_ocat, "nRet:[%d]", nRet);
    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
    olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], START", __FUNCTION__, sqlstr, currenttime.tv_sec, currenttime.tv_nsec);
    nRet = sqlite3_step(pStmt);
    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
    olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], END", __FUNCTION__, sqlstr, currenttime.tv_sec, currenttime.tv_nsec);
    if (nRet == SQLITE_ROW)
    {
        //取得int型的值，0为对应列的索引
        olog_error(db_ocat, "nRet:[%d]", nRet);
        count = sqlite3_column_int(pStmt, 0);
    }
    sqlite3_finalize(pStmt);
    olog_debug(db_ocat, "count:[%d]", count);

    return count;
}
#endif

/**
*********************************************************************
* @brief： 获取记录总数
* @param[in]：
               db           handle数据情报句柄
               table_name   表名
               condition    执行SQl的条件语句（含where及其之后的部分）
* @param[out]：
               none
* @return： 返回值说明       记录条数
*********************************************************************
*/
sint32 m_get_recoder_count(sqlite3 *db, sint8 *table_name, sint8 *condition)
{
    sint8 sqlstr[256] = {0};
    sqlite3_stmt *pStmt;  //预编译对象
    sint32 nCount = 0;
    sint32 nRet   = -1;
    const sint8 *pZtail;
#ifdef AREA_FUJIAN
	sint8 data_type  = ENUM_INIT_PARAM;
#else
	sint8 data_type  = ENUM_NORMAL_DATA;
#endif

    while(data_type <= ENUM_NOT_INIT_DATA)
    {
        snprintf(sqlstr, sizeof(sqlstr), "select COUNT(*) from '%s%d' %s", table_name, data_type, condition);
        data_type++;
        
        //预编译SQL语句
        nRet = sqlite3_prepare_v2(db, sqlstr, -1, &pStmt, &pZtail);
        //执行预编译的SQL
        nRet = sqlite3_step(pStmt);
        if (nRet == SQLITE_ROW)
        {
            olog_debug(db_ocat, "nRet:[%d]", nRet);
            //取得int型的值，0为对应列的索引
            nCount += sqlite3_column_int(pStmt, 0);
        }
        else
        {
            olog_debug(db_ocat, "m_get_recoder_count sqlstr[%s] nRet:[%d]", sqlstr, nRet);
        }
        //销毁预编译sql语句
        sqlite3_finalize(pStmt);
    }
    olog_debug(db_ocat, "m_get_recoder_count nCount:[%d]", nCount);

    return nCount;
}
/**
*********************************************************************
* @brief： 获取记录No
* @param[in]：
               db           handle数据情报句柄
               table_name   表名
               condition    执行SQl的条件语句（含where及其之后的部分）
               pnNo         记录No
* @param[out]：
               none
* @return： 返回值说明        <=0  失败
                            其他 记录数量
*********************************************************************
*/
sint32 m_get_recoder_no(sqlite3 *db, sint8 *table_name, sint8 *condition)
{
    sint8 sqlstr[2048] = {0};
    sqlite3_stmt *pStmt;  //预编译对象
    sint32 nNo  = -1;
    sint32 nRet = -1;
    const sint8 *pZtail;

    if(db == NULL)
    {
        return -1;
    }

    snprintf(sqlstr, sizeof(sqlstr), "select No from '%s' %s", table_name, condition);
    olog_debug(db_ocat, "sql:[%s]", sqlstr);

    //预编译SQL语句
    nRet = sqlite3_prepare_v2(db, sqlstr, -1, &pStmt, &pZtail);
    //执行预编译的SQL
    nRet = sqlite3_step(pStmt);
    if (nRet == SQLITE_ROW)
    {
        olog_debug(db_ocat, "nRet:[%d]", nRet);
        //取得int型的值，0为对应列的索引
        nNo = sqlite3_column_int(pStmt, 0);
    }
    else
    {
        olog_debug(db_ocat, "m_get_recoder_no sqlstr[%s] nRet:[%d]", sqlstr, nRet);
    }
    //销毁预编译sql语句
    sqlite3_finalize(pStmt);
    
    olog_debug(db_ocat,"nNo:[%d]", nNo);

    return nNo;
}

/**
*********************************************************************
* @brief： 内部用读写普通数据执行函数
* @param[in]：
               db 操作数据库的句柄
               sqlbuff 要执行的sql语句。含有一个？的bind
               isforbackup 是否是操作备份数据库
* @param[out]：
               rspdata 返回普通数据的内容
* @return： 返回值说明
            0 查询到数据
           -1 没有数据
*********************************************************************
*/
sint32 m_read_normal_data_process(sqlite3 *db, sint8 *sqlbuff, NORMAL_DATA_READ_RSP_T *rspdata, sint32 isforbackup)
{
    sint32 nResult      = -1;  //执行结果
    sint32 nRc          = -1;  //函数返回值
    const sint8 *szTail = NULL;
    sqlite3_stmt *pVm   = NULL;  //预编译对象
    sint32 nLen = 0;
//    struct timespec currenttime;

    //预编译SQL语句
    nResult = sqlite3_prepare_v2(db, sqlbuff, -1, &pVm, &szTail);
    if (nResult == SQLITE_OK)
    {
//        clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//        olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], START", __FUNCTION__, sqlbuff, currenttime.tv_sec, currenttime.tv_nsec);
        //执行预编译的SQL语句
        nResult = sqlite3_step(pVm);
//        clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//        olog_info(db_ocat, "====%s, sql[%s], sec[%ld], nsec[%ld], END", __FUNCTION__, sqlbuff, currenttime.tv_sec, currenttime.tv_nsec);
        //存在行记录
        if (nResult == SQLITE_ROW)
        {
            //判断查询到的列
            sint32 nCols = sqlite3_column_count(pVm);
            if (nCols >= 1)
            {
                //读取Blob数据
                const void *pFileContent = sqlite3_column_blob(pVm, 0);
                //取得blob数据的长度，0是列索引值（从0开始）
                nLen = sqlite3_column_bytes(pVm, 0);
                if (nLen > 0 && pFileContent != NULL)
                {
                    //给outBut开辟空间
                    rspdata->outBuf = (uint8 *)malloc(nLen);
                    //查询到的数据拷贝到outBuf中
                    memcpy(rspdata->outBuf, pFileContent, nLen);
                    rspdata->bufLen       = nLen;
                    rspdata->outBufFreeFp = free;
                    nRc                   = 0;
                }

                //读取DataTypeID数据
                rspdata->dataTypeID = sqlite3_column_int(pVm, 1);

            }
        }
    }
    //释放掉预编译的sql语句
    sqlite3_finalize(pVm);
    return nRc;
}

/**
*********************************************************************
* @brief： 开始写单步记录型数据
* @param[in]：
               无       　无
* @param[out]：
               rsp       记录型数据响应
* @return： 返回值说明
*********************************************************************
*/
sint32 m_start_write_record_data(RECORD_DATA_START_WRITE_RSP_T *rsp)
{
    uint32 nRet       = -1;
    qlist_t *pList    = NULL;
    uint8 keyname[64] = {0};

    STEP_WRITE_RECORD_INFO_T *pReadrcordinfo = NULL;

    pReadrcordinfo = (STEP_WRITE_RECORD_INFO_T *)malloc(sizeof(STEP_WRITE_RECORD_INFO_T));
    if (!pReadrcordinfo)
    {
        olog_error(db_ocat, "m_start_write_record_data　pReadrcordinfo is NULL \n");
        return -1;
    }

    //初始化Ｌｉｓｔ
    pList = qlist(QLIST_THREADSAFE);
    if (pList == NULL)
    {
        rsp->handleNum = 0;
        olog_error(db_ocat, "m_start_write_record_data　pList is NULL \n");
        return -1;
    }

    pReadrcordinfo->handerNum = pList;
    // printf("m_start_write_record_data:%x \n", pList);
    pReadrcordinfo->indexNum = 0;
    time(&(pReadrcordinfo->time));

    //将保存记录的列表放入hashtab中
    snprintf((char*)keyname, sizeof(keyname), "%X", (uint32)pReadrcordinfo);
    s_pstepWriteCltInfo->putint(s_pstepWriteCltInfo, (const char*)keyname, (uint32)pReadrcordinfo);
    // s_pstepWriteCltInfo->debug(s_pstepWriteCltInfo, stdout);

    //设定返回消息的handle
    rsp->handleNum = (uint32)pReadrcordinfo;

    nRet = 0;

    return nRet;
}

/**
*********************************************************************
* @brief： 单步写记录型数据
* @param[in]：
               req       写记录型数据输入参数
* @param[out]：
               rsp       记录型数据响应
* @return： 返回值说明
*********************************************************************
*/
sint32 m_step_write_record_data(WRITE_RECORD_STEP_REQ_T *req, sint32 *rsp)
{
    int nRet          = -1;
    uint8 keyname[20] = {0};
    qlist_t *pList    = NULL;

    STEP_WRITE_RECORD_INFO_T *phRecordinfo = NULL;

    snprintf((char*)keyname, sizeof(keyname), "%X", (uint32)req->handerNum);
	int64_t temp = s_pstepWriteCltInfo->getint(s_pstepWriteCltInfo, (const char*)keyname);
	memcpy(&phRecordinfo, &temp, sizeof(STEP_WRITE_RECORD_INFO_T *));
//    phRecordinfo = (STEP_WRITE_RECORD_INFO_T *)((uint64)(s_pstepWriteCltInfo->getint(s_pstepWriteCltInfo, (const char*)keyname)));
    if (NULL == phRecordinfo)
    {
        olog_debug(db_ocat, "m_step_write_record_data phRecordinfo is NULL \n");
        return -1;
    }
    pList = phRecordinfo->handerNum;
    // printf("m_step_write_record_dat pList:%x \n", pList);

    WRITE_RECORD_STEP_INFO_T *precord =
        (WRITE_RECORD_STEP_INFO_T *)malloc(sizeof(WRITE_RECORD_STEP_INFO_T) + req->outBufLen);
    if (!precord)
    {
        olog_error(db_ocat, "m_step_write_record_dat precord is NULL \n");
        *rsp = 0;
        return -1;
    }
    else
    {
        // olog_debug(db_ocat,"step_write_malloc:%x \n", precord);
    }

    precord->flag           = 0;
    precord->majorDI        = req->majorDI;
    precord->minorDIOptinal = req->minorDIOptinal;
    precord->minorDI        = req->minorDI;
    precord->startTime      = req->startTime;
    precord->endTime        = req->endTime;
    precord->storageTime    = req->storageTime;
    precord->infoNum        = req->infoNum;
    precord->checkInfoLen   = req->checkInfoLen;
    memcpy(precord->checkInfo, req->checkInfo, sizeof(precord->checkInfo));
    // precord->outBuf = req->outBuf;
    precord->cBufLen = req->outBufLen;
    memcpy(&precord->cBuf[0], req->outBuf, req->outBufLen);

    precord->logi_devNo = req->logi_devNo;
    nRet                = pList->addlast(pList, &precord, sizeof(precord));
    if (nRet == true)
    {
        *rsp = pList->num;
        return 0;
    }
    else
    {
        //*rsp = pList->qlist_size(pList);
        // pList->debug(pList, stdout);
        olog_error(db_ocat, "m_step_write_record_dat addlast failure \n");
        return -1;
    }
}

/**
*********************************************************************
* @brief： 结束单步写记录型数据
* @param[in]：
               req       写记录型数据输入参数
* @param[out]：
               rsp       记录型数据响应
* @return： 返回值说明
*********************************************************************
*/
sint32 m_fin_step_write_record_data(WRITE_RECORD_STEP_REQ_T *req, sint8 *rsp)
{
    sint8 nRet        = -1;
    uint8 keyname[20] = {0};
    qlist_t *pList    = NULL;

    STEP_WRITE_RECORD_INFO_T *phRecordinfo = NULL;

    printf("req.handerNum2:%u \n", req->handerNum);

    *rsp = nRet;

    snprintf((char*)keyname, sizeof(keyname), "%X", req->handerNum);
    int64_t temp = s_pstepWriteCltInfo->getint(s_pstepWriteCltInfo, (const char*)keyname);
	memcpy(&phRecordinfo, &temp, sizeof(STEP_WRITE_RECORD_INFO_T *));
//    phRecordinfo = (STEP_WRITE_RECORD_INFO_T *)((int64_t)(s_pstepWriteCltInfo->getint(s_pstepWriteCltInfo, (const char*)keyname)));
    if (NULL == phRecordinfo)
    {
        olog_debug(db_ocat, "m_fin_steps_write_record_data phRecordinfo is NULL \n");
        return -1;
    }

    pList = phRecordinfo->handerNum;
    if (!pList)
    {
        olog_debug(db_ocat, "m_fin_steps_write_record_data pList is NULL \n");
        return -1;
    }
    // pList->debug(pList, stdout);

    pList->lock(pList);
    /* 获取pList中的所有记录 */
    WRITE_RECORD_STEP_INFO_T *pInfo = NULL;
    qlist_obj_t obj;
    memset(&obj, 0x00, sizeof(obj));
    while (pList->getnext(pList, &obj, false) == true)
    {
        memcpy(&pInfo, obj.data, sizeof(pInfo));
        if (pInfo->flag == 1)
        {
            // printf("continue majorDI %d\n", pInfo->majorDI);
            // printf("free:%x \n", pInfo);
            free(pInfo);
            qlist_removefirst(pList);
            continue;
        }
#ifdef DB_CENTER_DEBUG
        printf("1 pInfo_n->logi_devNo:%d\n", pInfo->logi_devNo);
        printf("1 pInfo_n->majorDI:%d\n", pInfo->majorDI);
        printf("1 pInfo_n->infoNum:%d\n", pInfo->infoNum);
        printf("1 pInfo_n->minorDI:%d\n", pInfo->minorDI);
#endif
        find_write_db_by_pridi(pInfo, pList);
        // printf("free:%x \n", pInfo);
        free(pInfo);
        qlist_removefirst(pList);
        // qlist_removeat(pList, 0);
    }

    pList->unlock(pList);
	pList->clear(pList);
    pList->free(pList);

    free(phRecordinfo);
    //从hash表中移除之前保存的list头
    s_pstepWriteCltInfo->remove(s_pstepWriteCltInfo, (const char*)keyname);

    *rsp = 0;
    return S_OK;
}
/**
*********************************************************************
* @brief： 对搜寻到的数据进行数据库的写入
* @param[in]：
               pInfo       写记录型数据输入参数
               pList       列表
* @param[out]：
               ret        成功／失败
* @return： 返回值说明
*********************************************************************
*/
int find_write_db_by_pridi(WRITE_RECORD_STEP_INFO_T *pInfo, qlist_t *pList)
{
    sint32 nRet                       = -1;
    WRITE_RECORD_STEP_INFO_T *pInfo_n = NULL;
    qlist_obj_t obj                   = {0};
    sint8 sqlstr[512]                 = {0};
    DB_TAB_RECORD_INFO_T hRecordinfo  = {0};   //记录型数据情报句柄
    sqlite3_stmt *pStmt_i             = NULL;  //预编译对象
    sqlite3_stmt *pStmt_u             = NULL;  //预编译对象

    get_collect_tab_info(pInfo->logi_devNo, pInfo->infoNum, pInfo->majorDI, 0, &hRecordinfo, true);

    sqlite3 *db = hRecordinfo.handle->db_writehander_get(hRecordinfo.handle);
    snprintf(sqlstr, sizeof(sqlstr), "INSERT OR IGNORE INTO %s VALUES (NULL, %u, ?, %u,?, ?, ?, ?, ?, ?)",
             hRecordinfo.tab_name, pInfo->logi_devNo, pInfo->majorDI);

    nRet = sqlite3_prepare_v2(db, sqlstr, -1, &pStmt_i, NULL);  //准备语句
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "find_write_db_by_pridi insert sqlite3_prepare Fail:　ret:%d; sql:%s  \n", nRet, sqlstr);
        put_collect_tab_info(pInfo->logi_devNo, pInfo->infoNum, pInfo->majorDI, 0);
        return S_NG;
    }
    // Update prepare
    snprintf(sqlstr, sizeof(sqlstr),
             "UPDATE %s SET AddCheck=?,DataContent=? where InfoAddrNo = ? and SubDataDI =? and AcqStartTime = ? and "
             "AcqSuccessTime= ? and AcqSaveTime=?;",
             hRecordinfo.tab_name);

    nRet = sqlite3_prepare_v2(db, sqlstr, -1, &pStmt_u, NULL);  //准备语句
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "find_write_db_by_pridi update sqlite3_prepare Fail:　ret:%d; sql:%s  \n", nRet, sqlstr);
        put_collect_tab_info(pInfo->logi_devNo, pInfo->infoNum, pInfo->majorDI, 0);
        return S_NG;
    }

    memset(&obj, 0x00, sizeof(obj));

    while (pList->getnext(pList, &obj, false) == true)
    {
        memcpy(&pInfo_n, obj.data, sizeof(pInfo_n));
        if (!pInfo_n || pInfo_n->flag == 1)
        {
            continue;
        }

        if (pInfo_n->majorDI != pInfo->majorDI || pInfo_n->logi_devNo != pInfo->logi_devNo
            || pInfo_n->infoNum != pInfo->infoNum)
        {
            continue;
        }
        //标识完成
        pInfo_n->flag = 1;

        step_write_collect_db(db, pInfo_n, pStmt_i, pStmt_u);
        // if(nRet != S_OK)  break;
    }

    comp_write_collect_db(pStmt_i, pStmt_u);
    put_collect_tab_info(pInfo->logi_devNo, pInfo->infoNum, pInfo->majorDI, 0);
    if (hRecordinfo.tab_status == ENUM_TABLE_TEMP) {
        db_close(hRecordinfo.handle);
    }
    return nRet;
}
/**
*********************************************************************
* @brief： 单步写记录型数据到缓存
* @param[in]：
               db          数据库句柄
               pInfo       写记录型数据输入参数
               pStmt　　　　ＳＱＬ解析语句
               pStmt_u     sql解析语句
* @param[out]：
               ret       成功／失败
* @return： 返回值说明
*********************************************************************
*/
int step_write_collect_db(sqlite3 *db, WRITE_RECORD_STEP_INFO_T *pInfo, sqlite3_stmt *pStmt, sqlite3_stmt *pStmt_u)
{
    sint32 nRet = -1;
//    struct timespec currenttime;

    sqlite3_reset(pStmt_u);
    // Update
    //绑定预编译SQL语句中的BLOB类型
    nRet = sqlite3_bind_blob(pStmt_u, 1, pInfo->checkInfo, pInfo->checkInfoLen, NULL);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "pStmt_u Check Information Bind Fail \n");
        return S_NG;
    }
    //绑定预编译SQL语句中的BLOB类型
    nRet = sqlite3_bind_blob(pStmt_u, 2, pInfo->cBuf, pInfo->cBufLen, NULL);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "pStmt_u Data Bind Faild \n");
        return S_NG;
    }

    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt_u, 3, pInfo->infoNum);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "Check pStmt_u infoNum Fail \n");
        return S_NG;
    }
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt_u, 4, pInfo->minorDI);
    if (nRet != SQLITE_OK)
    {
        olog_debug(db_ocat, "Check minorDI Fail \n");
        return S_NG;
    }
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt_u, 5, pInfo->startTime);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "Check pStmt_u Bind startTime Fail \n");
        return S_NG;
    }
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt_u, 6, pInfo->endTime);
    if (nRet != SQLITE_OK)
    {
        olog_debug(db_ocat, "Check pStmt_u Bind endTime Fail \n");
        return S_NG;
    }
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt_u, 7, pInfo->storageTime);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "Check pStmt_u Bind storageTime Fail \n");
        return S_NG;
    }
    //执行预编译的SQL语句
//    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//    olog_info(db_ocat, "====%s, sec[%ld], nsec[%ld], START", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);
    nRet = sqlite3_step(pStmt_u);
//    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//    olog_info(db_ocat, "====%s, sec[%ld], nsec[%ld], END", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);
    if (nRet != SQLITE_DONE)
    {
        olog_error(db_ocat, "step_write_collect_db SQL Step Exce Fail ret:%d\n", nRet);
        // sqlite3_finalize(pStmt);
        return S_NG;
    }
    //查询有多少记录受到影响
    nRet = sqlite3_changes(db);
    if (nRet > 0)
    {
        return S_OK;
    }

    // Insert
    sqlite3_reset(pStmt);
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt, 1, pInfo->infoNum);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "Check infoNum Fail \n");
        return S_NG;
    }
    //    if(pInfo->minorDIOptinal == 0)
    //     {
    //         nRet = sqlite3_bind_int(pStmt, 1, -1);
    //     }
    //     else
    //     {
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt, 2, pInfo->minorDI);
    //    }

    //    if (nRet != SQLITE_OK)
    //    {
    //        olog_error(db_ocat,"Check Information Bind minorDI Fail \n");
    //        return S_NG;
    //    }
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt, 3, pInfo->startTime);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "Check Information Bind startTime Fail \n");
        return S_NG;
    }
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt, 4, pInfo->endTime);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "Check Information Bind endTime Fail \n");
        return S_NG;
    }
    //绑定预编译SQL语句中的int类型
    nRet = sqlite3_bind_int(pStmt, 5, pInfo->storageTime);
    if (nRet != SQLITE_OK)
    {
        olog_debug(db_ocat, "Check Information Bind storageTime Fail \n");
        return S_NG;
    }

    //绑定预编译SQL语句中的BLOB类型，以？为占位符，第二个参数是占位符的索引值
    nRet = sqlite3_bind_blob(pStmt, 6, pInfo->checkInfo, pInfo->checkInfoLen, NULL);
    if (nRet != SQLITE_OK)
    {
        olog_debug(db_ocat, "Check Information Bind Fail \n");
        return S_NG;
    }
    //绑定预编译SQL语句中的BLOB类型，以？为占位符，第二个参数是占位符的索引值
    nRet = sqlite3_bind_blob(pStmt, 7, pInfo->cBuf, pInfo->cBufLen, NULL);
    if (nRet != SQLITE_OK)
    {
        olog_error(db_ocat, "Data Bind Faild \n");
        return S_NG;
    }
    //执行预编译的SQL语句
//    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//    olog_info(db_ocat, "====%s, sec[%ld], nsec[%ld], START", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);
    nRet = sqlite3_step(pStmt);
//    clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//    olog_info(db_ocat, "====%s, sec[%ld], nsec[%ld], END", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);
    if (nRet != SQLITE_DONE)
    {
        olog_error(db_ocat, "step_write_collect_db SQL Step Exce Fail2 ret:%d\n", nRet);
        // sqlite3_finalize(pStmt);
        return S_NG;
    }

    return S_OK;
}

/**
*********************************************************************
* @brief： 提交缓存数据到ＤＢ，完成单步写记录型数据
* @param[in]：
               pStmt     sql解析语句
               pStmt_u   sql解析语句
* @param[out]：
               rsp       记录型数据响应
* @return： 返回值说明
*********************************************************************
*/
sint32 comp_write_collect_db(sqlite3_stmt *pStmt, sqlite3_stmt *pStmt_u)
{
    sint32 nRet = 0;
    // olog_debug(db_ocat,"comp_write_collect_db \n");
    //销毁预编译sql语句
    sqlite3_finalize(pStmt_u);
    sqlite3_finalize(pStmt);

    return nRet;
}
/**
*********************************************************************
* @brief： 释放单步操作用的List
* @param[in]：
               无       无
* @param[out]：
               无       无
* @return： 返回值说明
*********************************************************************
*/
void free_step_opt_hash_tbl(void)
{
    if (s_pstepWriteCltInfo)
    {
        s_pstepWriteCltInfo->clear(s_pstepWriteCltInfo);
        s_pstepWriteCltInfo->free(s_pstepWriteCltInfo);
        s_pstepWriteCltInfo = NULL;
    }

    // if (s_psetupReadInfotbl)
    // {
    //     s_psetupReadInfotbl->clear(s_psetupReadInfotbl);
    //     s_psetupReadInfotbl->free(s_psetupReadInfotbl);
    //     s_psetupReadInfotbl = NULL;
    // }
}
/**
*********************************************************************
* @brief： 对搜寻到的数据进行整合处理
* @param[in]：
               pInfo       单个记录型数据
               pList       搜寻到的列表
* @param[out]：
               subp_org　　　　　结果要放入的缓存
               outLen　　　　 　 结果长度
               itemSum　　　　　 单元个数
               ret        成功／失败
* @return： 返回值说明
*********************************************************************
*/
int find_read_by_order(READ_RECORD_STEP_RSP_T *pInfo, qlist_t *pList, uint8 *subp_org, sint32 *outLen, sint32 *itemSum)
{
    sint32 nRet                     = 0;
    sint32 nSum                     = 0;
    READ_RECORD_STEP_RSP_T *pInfo_n = pInfo;
    qlist_obj_t obj                 = {0};
    uint8 *subp                     = subp_org;
    uint8 *old_subp                 = subp_org;
    uint32 sunLen                   = 0;

    //首个数据处理
    // 1月17变更暂时增加 ==================
    if (pInfo_n->majorDI == 0)
    {
        push_u8(&subp, _OPTIONSL_FALSE);
    }
    else
    {
        push_u8(&subp, _OPTIONSL_TRUE);
        push_u32(&subp, pInfo_n->majorDI);
    }

    // if (pInfo_n->minorDIOptinal)
    // {
    // push_u8(&subp, _OPTIONSL_TRUE);
    push_u32(&subp, pInfo_n->minorDI);
    // }
    // else
    // {
    // push_u8(&subp, _OPTIONSL_FALSE);
    // }

    //==================================

    // push_u32(&subp, pInfo_n->outBufLen);
    nSum++;

    push_nbyte(&subp, pInfo_n->outBuf, pInfo_n->outBufLen);
    sunLen += subp - (uint8 *)old_subp;

    old_subp = subp;

    memset(&obj, 0x00, sizeof(obj));
    //开始其余数据处理
    while (pList->getnext(pList, &obj, false) == true)
    {
        memcpy(&pInfo_n, obj.data, sizeof(pInfo_n));
#ifdef DB_CENTER_DEBUG
        printf("pInfo_n->majorDI:%d\n", pInfo_n->majorDI);
        printf("pInfo_n->infoNum:%d\n", pInfo_n->infoNum);
        printf("pInfo_n->minorDI:%d\n", pInfo_n->minorDI);
        printf("pInfo_n->startTime:%d\n", pInfo_n->startTime);
#endif
        if (!pInfo_n || pInfo_n->flag == 1)
        {
            continue;
        }

        if (0 != memcmp(pInfo->checkInfo, pInfo_n->checkInfo, pInfo->checkInfoLen)
            || pInfo->checkInfoLen != pInfo_n->checkInfoLen)
        {
            dzlog_warn("CHECK_INFO_2 miss matching req[%02x%02x%02x%02x%02x%02x] store[%02x%02x%02x%02x%02x%02x]",
                       pInfo->checkInfo[0], pInfo->checkInfo[1], pInfo->checkInfo[2], pInfo->checkInfo[3],
                       pInfo->checkInfo[4], pInfo->checkInfo[5], pInfo_n->checkInfo[0], pInfo_n->checkInfo[1],
                       pInfo_n->checkInfo[2], pInfo_n->checkInfo[3], pInfo_n->checkInfo[4], pInfo_n->checkInfo[5]);
            continue;
        }

        if (pInfo_n->startTime == pInfo->startTime && pInfo_n->storageTime == pInfo->storageTime
            && pInfo_n->endTime == pInfo->endTime && pInfo_n->infoNum == pInfo->infoNum)
        {
            if (pInfo_n->minorDI == pInfo->minorDI)
            {
                // holog_debug(db_ocat,pInfo_n, sizeof(READ_RECORD_STEP_RSP_T));
                // printf("******pInfo->minorDI %d\n", pInfo->minorDI);
                pInfo_n->flag = 1;  //标识完成
                continue;
            }
            // push_u32(&subp, pInfo_n->majorDI);

            if (pInfo_n->majorDI == 0)
            {
                push_u8(&subp, _OPTIONSL_FALSE);
            }
            else
            {
                push_u8(&subp, _OPTIONSL_TRUE);
                push_u32(&subp, pInfo_n->majorDI);
            }

            // if (pInfo_n->minorDIOptinal)
            //{
            //    push_u8(&subp, _OPTIONSL_TRUE);
            push_u32(&subp, pInfo_n->minorDI);
            //}
            // else
            //{
            //   push_u8(&subp, _OPTIONSL_FALSE);
            //}

            // printf("@@@pInfo->minorDI %d\n", pInfo->minorDI);
            // push_u32(&subp, pInfo_n->outBufLen);
            nSum++;

            push_nbyte(&subp, pInfo_n->outBuf, pInfo_n->outBufLen);
            sunLen += subp - (uint8 *)old_subp;
            old_subp      = subp;
            pInfo_n->flag = 1;  //标识完成
            // olog_debug(db_ocat,"sunLen[%d] = %d \n", pInfo_n->minorDI, sunLen);
            continue;
        }
        else
        {
        }
    }

    *itemSum = nSum;
    *outLen  = sunLen;

    return nRet;
}
