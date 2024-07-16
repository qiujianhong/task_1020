/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 数据库底层驱动程序实现
* @date： 2019-11-9
*********************************************************************
*/

#include "dbModule.h"
#include "baseType.h"
#include "dbCenterConfig.h"
#include "globalConfiguration.h"
#include "dbComm.h"
#include "dbDataInterface.h"
#include "dbModule.h"
#include "dbRaw.h"
#include "error.h"
#include "sqlite3.h"
#include "olog.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

//日志输出
extern void *     db_ocat ;
//日志输出
extern void *     timeout_ocat;
//数据库操作用锁
extern pthread_mutex_t g_db_del_lock;
extern sint32 max_handle_count;
/**
*********************************************************************
* @brief： 数据表初始化
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 db_init(void)
{
    sint32 rc = S_OK;

    const char *sqlite_version =  sqlite3_libversion();
    olog_info(db_ocat, "sqlite_version [%s]", sqlite_version);
    if (strcmp(sqlite_version,SQLITE_VERSION) != 0)
    {
        olog_error(db_ocat, "local sqlite3_libversion [%s] miss match:[%s]", sqlite_version, SQLITE_VERSION);
        return -1;
    }

    if (access(g_configObj.dbCfg.toolSqlite, 0) == -1)
    {
        olog_error(db_ocat, "The sqlite is not exist");
        return -1;
    } else {
        char command[1024] = {0};
        char buf[1024] = {0};
        sprintf(command, "%s -version", g_configObj.dbCfg.toolSqlite);
        FILE* fp = NULL;
        if ((fp = popen(command, "r")) != NULL)
        {
            fgets(buf, 1024, fp);
            //olog_debug(db_err, "buf[%s]", buf);
            if (strlen(buf) == 0)
            {
                olog_error(db_ocat, "The sqlite file is error");
                pclose(fp);
                return -1;
            }
        }
        olog_debug(db_ocat, "The sqlite is exist");
        pclose(fp);
    }

    DIR *dir;
    dir = opendir(BAD_DB_PATH);
    if(NULL==dir)
        mkdir(BAD_DB_PATH,0775);
    else
        closedir(dir);

    dir = opendir(BAD_BACKUP_DB_PATH);
    if(NULL==dir)
        mkdir(BAD_BACKUP_DB_PATH,0775);
    else
        closedir(dir);
    
    //check database
//    check_and_recovery_database(g_configObj.dbCfg.basePath, 0);
//    check_and_recovery_database(g_configObj.dbCfg.backupPath, 1);

    if (g_configObj.serviceCfg.handerThreadNum > DB_OPEN_HANDLE_MAX_COUNT) {
        max_handle_count = g_configObj.serviceCfg.handerThreadNum;
    } else {
        max_handle_count = DB_OPEN_HANDLE_MAX_COUNT;
    }

    rc = db_normaldatadb_init();
    if(rc != S_OK )
    {
        //olog_debug(db_ocat,"db_init - db_normaldatadb_init Fail\r\n");
        olog_error(db_ocat,"db_normaldatadb_init Fail");
        return -1;
    }

    rc = db_paramdatadb_init();
    if(rc != S_OK)
    {
        olog_error(db_ocat,"db_init - db_paramdatadb_init Fail\r\n");
    }
    m_db_api_module_init();
    
    if (pthread_mutex_init(&(g_db_del_lock), NULL) != 0)
    {
        rc = -1;
        olog_error(db_ocat,"db_init - g_db_del_lock Fail\r\n");
    }

    return rc;
}

/**
*********************************************************************
* @brief： 数据库关闭
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
static void db_interrubt_stop(void)
{
    pthread_mutex_lock(&g_db_del_lock);
    db_normaldatadb_de_init();
    db_record_deinit();
}
/**
*********************************************************************
* @brief： 数据库停止
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void db_stop(void)
{
#if 0
    stop_del_service();
    free_hash_tbl();
    free_step_opt_hash_tbl();
    return S_OK;
#else
    db_interrubt_stop();
#endif
}

/**
*********************************************************************
* @brief： 数据库模块启动
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 db_start(void)
{
    //check database
    check_and_recovery_database(g_configObj.dbCfg.basePath, 0);
    check_and_recovery_database(g_configObj.dbCfg.backupPath, 1);
    
    int ret = start_db_del_servie();
    if(ret != S_OK)
    {
        olog_debug(db_ocat,"start_db_del_servie 失败 \n");
    }
    
    return S_OK;
}

/**
*********************************************************************
* @brief： 恢复出厂设置
* @param[in]： logi_dev   逻辑设备号
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 db_recovery_factory(uint8 logi_dev)
{
    return S_OK;
}
