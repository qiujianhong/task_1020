/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 数据库操作共用函数集合
* @date： 2019-11-9
*********************************************************************
*/

#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/vfs.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#include "baseType.h"
#include "dbCenterConfig.h"
#include "dbComm.h"
#include "dbDataInterface.h"
#include "dbModule.h"
#include "dbRaw.h"
#include "globalConfiguration.h"
#include "olog.h"
#include "qlibc.h"
#include "sqlite3.h"

#define DB_SUB_DI_MAX_COUNT 200       //次DI数量
#define DB_RECOVERY_DEBUG   1         //debug开关

static qhashtbl_t *g_db_c_tbl;        //搜集型数据信息指针
static qhashtbl_t *g_db_n_cfg_tbl;    //普通型数据配置信息指针
static qhashtbl_t *g_tab_depth_cfgl;  //数据深度的配置信息指针
static qhashtbl_t *g_tab_info_cfg;    // save the table info

static DB_RAW_T *g_parameter_tbl;       //parameter table 句柄
static DB_RAW_T *g_generalr_tbl;        //general table 句柄
static DB_RAW_T *g_generalr_delay_tbl;  // general_delay table 句柄
static DB_RAW_T *g_memory_tbl;          //内存表句柄
pthread_mutex_t g_db_del_lock;

sint32 max_handle_count = DB_OPEN_HANDLE_MAX_COUNT;

//日志输出
extern void *db_ocat;
//日志输出
extern void *db_err;
//日志输出
extern void *db_recovery;

extern void *db_record_status;
/*定义深度配置文件Targe名和对应的数据结构
#指定主DI和次DI，或只有主DI(缺省)数据深度设定
[db_depth_1200]   =>   [主DI为1200]
default=608       =>   没有相应次DI的缺省值为608
0030=100          =>   次DI为0030的深度为100
0031=200          =>   次DI为0031的深度为100
[db_depth_1201]
default=20009
0030=100
0031=200
#缺省数据深度设定(没有匹配DI的设定)
[db_default_depth]
default=507
*/
const sint8 *c_depth_til_name = "db_depth_";            //配置表中深度前缀
const sint8 *c_fault_til_name = "db_default_depth";     //默认深度
typedef struct
{
    sint8 key[64];          //key 名称
    sint8 depth[16];        //深度值
} DEPTH_CFG_T;              //深度信息

typedef struct
{
    sint32 subDI;           //次DI
    sint32 depth;           //深度值
} SUB_DI_INFO_T;            //次DI深度信息

typedef struct
{
    sint8 Key[64];          //key 名称
    sint32 majorDI;         //主DI
    sint32 defaultDepth;    //默认深度
    sint32 subDICount;      //次DI数量
    SUB_DI_INFO_T subDIInfo[DB_SUB_DI_MAX_COUNT]; //次DI信息
} DEPTH_INFO_T;

//删除服务状态， 0 停止；1 运行
static int s_del_service_stat = 0;
//重新尝试次数
const sint32 DB_RETRY_STEP    = 5;
/**
*********************************************************************
* @brief： 数据库获取参数句柄
* @param[in]： none
* @param[out]：none
* @return： 参数句柄
*********************************************************************
*/
DB_RAW_T *get_Tparameter(void)
{
    return g_parameter_tbl;
}

/**
*********************************************************************
* @brief： 数据库获取通用句柄
* @param[in]： none
* @param[out]：none
* @return： 通用句柄
*********************************************************************
*/
DB_RAW_T *get_Tgeneral(void)
{
    return g_generalr_tbl;
}

/**
*********************************************************************
* @brief： 数据库获取通用句柄
* @param[in]： none
* @param[out]：none
* @return： 通用句柄
*********************************************************************
*/
DB_RAW_T *get_Tgeneral_delay(void)
{
    return g_generalr_delay_tbl;
}

/**
*********************************************************************
* @brief： 数据库获取通用句柄
* @param[in]： none
* @param[out]：none
* @return： 通用句柄
*********************************************************************
*/
DB_RAW_T *get_Tmemory(void)
{
    return g_memory_tbl;
}
/**
*********************************************************************
* @brief： 获取int类型值
* @param[in]：
               ps_b           字符串指针
* @param[out]：
               result         获取的输出值
* @return： 返回值说明：结束后新的位置
*********************************************************************
*/
sint8 *get_int_value(sint8 *ps_b, sint32 *result)
{
    sint8 buf[128] = {0};
    sint32 rc      = 0;
    sint32 len     = 0;
    sint8 *ps_cur  = ps_b;
    while (true)
    {
        if (*ps_cur == ',' || *ps_cur == 0x00)
        {
            if (len > 127)
                return NULL;
            strncpy(buf, ps_b, len);
            buf[len] = 0x00;
            rc       = atoi(buf);
            *result  = rc;
            break;
        }
        len++;
        ps_cur++;
    }
    return ++ps_cur;
}

/**
*********************************************************************
* @brief： 获取string类型值
* @param[in]：
               ps_b           字符串指针
* @param[out]：
               out            获取的输出字符串
* @return： 返回值说明：结束后新的位置
*********************************************************************
*/
sint8 *get_str_value(sint8 *ps_b, sint8 *out)
{
    sint32 len    = 0;
    sint8 *ps_cur = ps_b;
    while (*ps_cur != 0x00)
    {
        if (*ps_cur == ',')
        {
            strncpy(out, ps_b, len);
            break;
        }
        len++;
        ps_cur++;
    }
    return ++ps_cur;
}

/**
*********************************************************************
* @brief： 解析数据库普通型数据配置信息
* @param[in]：
               pstr           数据地址
               data_id        数据项标识
               is_backup      是否要备份
               is_realtime    是否是实时数据
               data_type      数据类型
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 parse_db_normal_cfg(sint8 *pstr, sint8 *data_id, sint32 *is_backup, sint32 *is_realtime, sint32 *data_type)
{
    sint8 *p_cur;
    if (pstr == NULL)
    {
        return S_NG;
    }
    p_cur = get_str_value(pstr, data_id);
    if (p_cur == NULL)
    {
        return S_NG;
    }
    p_cur = get_int_value(p_cur, data_type);
    if (p_cur == NULL)
    {
        return S_NG;
    }
    p_cur = get_int_value(p_cur, is_backup);
    if (p_cur == NULL)
    {
        return S_NG;
    }
    p_cur = get_int_value(p_cur, is_realtime);
    if (p_cur == NULL)
    {
        return S_NG;
    }

    return S_OK;
}
/**
*********************************************************************
* @brief： 解析table name
* @param[in]：
               tab_name       表名
* @param[out]：
               logical device No
* @return： 返回值说明
*********************************************************************
*/
uint8 get_logi_dev_no_by_name(sint8 *tab_name, sint8 *endp)
{
    uint8 ch[4]  = {0};
    sint8 *endp1 = NULL;
    uint8 ret;
    memcpy((char*)(&ch[0]), (char*)(&tab_name[1]), 2);
    ret   = strtoul((char*)ch, &endp1, 16);
    *endp = *endp1;

    return ret;
}

/**
*********************************************************************
* @brief： 载入普通型数据配置信息
* @param[in]：
               tbl         数据表指针
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 load_normal_data_cfg(qlisttbl_t *tbl)
{
    sint8 buf[64] = "";
    sint32 i      = 0;
    sint8 *pstr;
    sint8 data_id[16] = {0};
    sint32 is_backup;
    sint32 is_realtime;
    sint32 data_type;
    //初始化普通型数据链表
    g_db_n_cfg_tbl = qhashtbl(0, QHASHTBL_THREADSAFE);

    DB_TAB_NORMAL_CFG_INFO_T nor_cfg;

    sint32 norDataIdNum = tbl->getint(tbl, "db_normal_tabs_cfg.data_ID_NUM");
    for (i = 0; i < norDataIdNum; i++)
    {
        sprintf(buf, "db_normal_tabs_cfg.data_ID%d", i);
        // olog_debug(db_ocat,"db_init",buf);
        pstr = tbl->getstr(tbl, buf, false);
        memset(data_id, 0x00, sizeof(data_id));
        parse_db_normal_cfg(pstr, data_id, &is_backup, &is_realtime, &data_type);

        // string conver to hex
        uint64 tmpDataId = 0;
        sscanf(data_id, "%x", (unsigned int*)&tmpDataId);

        // hex to string
        memset(data_id, 0, sizeof(data_id));
        sprintf(data_id, "%x", (unsigned int)tmpDataId);

        nor_cfg.is_backup = is_backup;
        nor_cfg.realtime  = is_realtime;
        nor_cfg.tab_type  = data_type;
        g_db_n_cfg_tbl->put(g_db_n_cfg_tbl, data_id, (const void *)&nor_cfg, sizeof(nor_cfg));
    }

    // g_db_n_cfg_tbl->debug(g_db_n_cfg_tbl, stdout);
    olog_debug(db_ocat, "g_db_n_cfg_tbl size: (%d) .", g_db_n_cfg_tbl->size(g_db_n_cfg_tbl));

    return 0;
}

/**********************************************************************
* @name      : get_line_by_key
* @brief     ：找到关键字所在行的位置和长度
* @param[in] ：FILE *pf            文件句柄
               const char *key    关键字

* @param[out]：long *poffset 关键字所在行起始位置
* @return    ：>0长度, <0错误
* @Create    :
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
int get_line_by_key(FILE *pf/*in*/, const char *key/*in*/, long *poffset/*out*/)
{
    char line[1024] = {0};
    long offset = 0;
    int line_len = 0;

    if(pf == NULL){
        return -1;
    }

    while(fgets(line,1024,pf) != NULL)
    {
        if(strstr(line, key) != NULL)
        {
            offset = ftell(pf); // 记录文件指针位置, 因为后面还要读, 文件指针会移走
            line_len = strlen(line);
            *poffset = offset;
            return line_len;
        }
        
    }
    return -1;
}

int write_cfg_by_key(const char *filename/*in*/, const char *key/*in*/, const char *value/*in*/, int add_or_alter)
{
    
    FILE *fp;
    long offset;
    
    if (!(fp = fopen(filename, "r+"))) 
    { // 尝试以读写方式打开文件.
        return -1;
    }

    char *str = qfile_load(filename, NULL);
    if (str == NULL)
    {
        fclose(fp);
        return -1;
    }

    
    int len = get_line_by_key(fp , key, &offset);
    if (len < 0) 
    { 
        fclose(fp);
        free(str);
        return -1;
    }

    if(add_or_alter == 0)   //alter
    {
        offset -= len;
        fseek(fp, offset, SEEK_SET);
        fwrite(value, strlen(value), 1, fp);
        fwrite(str+offset+len, strlen(str+offset+len), 1, fp);
    }
    else  //add
    {
        fwrite(value, strlen(value), 1, fp);
        fwrite(str+offset, strlen(str+offset), 1, fp);
    }

    free(str);
    fclose(fp);
    return 0;
}

extern qlisttbl_t *qconfig_parse_file(qlisttbl_t *tbl, const char *filepath,
                               char sepchar);
/**
*********************************************************************
* @brief： 参数数据配置信息写入配置文件
* @param[in]：
               path         配置文件路径
               data_tag     数据标识
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 put_param_data_cfg(const sint8 *path, uint32 data_tag)
{
    sint8 buf[64] = "";
    sint8 data_id[16] = {0};
    qlisttbl_t *tbl = qlisttbl(0);
    bool ret = 0;
	int rc = 0;
    DB_TAB_NORMAL_CFG_INFO_T nor_cfg;

    //获取全局配置树
    if (qconfig_parse_file(tbl, path, '=') == NULL)
    {
        olog_error(db_ocat,"Failed to open [%s].", path);
        return -1;
    }

    sint32 norDataIdNum = tbl->getint(tbl, "db_normal_tabs_cfg.data_ID_NUM");
    sprintf(data_id, "data_ID%d", norDataIdNum-1);
    sprintf(buf, "data_ID%d=%08x,1,1,1\n", norDataIdNum, data_tag);
    rc = write_cfg_by_key(path, data_id, buf, 1);
    if (rc < 0)
    {
        olog_error(db_ocat,"Failed to putstrf data_tag [%08x].", data_tag);
        return -1;
    }
    
    sprintf(buf, "data_ID_NUM=%d\n", norDataIdNum+1);
    rc = write_cfg_by_key(path, "data_ID_NUM", buf, 0);
    if (rc < 0)
    {
        olog_error(db_ocat,"Failed to putint norDataIdNum.");
        return -1;
    }

    snprintf(data_id, sizeof(data_id), "%x", data_tag);
    nor_cfg.is_backup = 1;
    nor_cfg.realtime  = 1;
    nor_cfg.tab_type  = 1;
    ret = g_db_n_cfg_tbl->put(g_db_n_cfg_tbl, data_id, (const void *)&nor_cfg, sizeof(nor_cfg));
    if (!ret)
    {
        olog_error(db_ocat,"Failed to put g_db_n_cfg_tbl data_tag [%08x].", data_tag);
        return -1;
    }

    olog_debug(db_ocat, "put_param_data_cfg add param tag  (%08x).", data_tag);

    return 0;
}
/**
*********************************************************************
* @brief： 获取普通型数据表信息
* @param[in]：
               data_tag           数据标识
               dataType           数据类型
* @param[out]：
               tab_info           数据表地址
* @return： 返回值说明
*********************************************************************
*/
sint32 get_normal_tab_info(uint32 data_tag, sint16 dataType, DB_TAB_NORMAL_INFO_T *tab_info)
{
    size_t isize                    = 0;
    DB_TAB_NORMAL_CFG_INFO_T *pinfo = NULL;
    DB_TAB_NORMAL_CFG_INFO_T default_nor_cfg = {0, 0, 0, 0};  //普通数据
    DB_TAB_NORMAL_CFG_INFO_T default_par_cfg = {0, 1, 1, 1};  //参数数据
    sint8 buf[16]                            = {0};
    snprintf(buf, sizeof(buf), "%x", data_tag);
    // g_db_n_cfg_tbl->debug(g_db_n_cfg_tbl, stdout);

    //获取普通数据配置信息
    pinfo = g_db_n_cfg_tbl->get(g_db_n_cfg_tbl, buf, &isize, 0);
    if (!pinfo)
    {
        if (dataType == 1 || dataType == 2)        //参数写入参数表
        {
			pinfo = &default_par_cfg;
        }
        else
        {   
            olog_debug(db_ocat, "Not find the key[%s] in g_db_n_cfg_tbl, so use the default value", buf);
            pinfo = &default_nor_cfg;
        }
    }

    tab_info->realtime  = pinfo->realtime;
    tab_info->is_backup = pinfo->is_backup;

    // olog_debug(db_ocat,"pinfo->realtime = %d", pinfo->realtime);

    if (pinfo->tab_type == 0)  //数据(0)
    {
        if (pinfo->realtime == 1)
        {
            strncpy(tab_info->tab_name, DB_CENT_TABLE_NAME_NORMAL, sizeof(tab_info->tab_name));
            tab_info->handle = get_Tgeneral();
        }
        else if (pinfo->realtime == 2)  //内存
        {
            strncpy(tab_info->tab_name, TMEMORY_NAME, sizeof(tab_info->tab_name));
            tab_info->handle = get_Tmemory();
        }
        else  //非及时
        {
            strncpy(tab_info->tab_name, TGENERAL_DELAY_NAME, sizeof(tab_info->tab_name));
            tab_info->handle = get_Tgeneral_delay();
        }
    }
    else  //参数(1)
    {
        strncpy(tab_info->tab_name, DB_CENT_TABLE_NAME_PARAMETER, sizeof(tab_info->tab_name));
        tab_info->handle = get_Tparameter();
    }

    return S_OK;
}

sint32 get_handle_open_count(void)
{
    sint32 count = 0;
    if (g_db_c_tbl != NULL) {
        qhashtbl_obj_t obj;
        memset((void*) &obj, 0, sizeof(obj));
//        g_db_c_tbl->lock(g_db_c_tbl);
        while(g_db_c_tbl->getnext(g_db_c_tbl, &obj, false) == true) {
            DB_TAB_RECORD_INFO_T *data = (DB_TAB_RECORD_INFO_T*)(obj.data);
            if (data->tab_status == ENUM_TABLE_OPEN) {
                count++;
            }
        }
//        g_db_c_tbl->unlock(g_db_c_tbl);
    }
    olog_debug(db_ocat, "%s, open_handle_count[%d]", __FUNCTION__, count);

    return count;
}

sint32 get_handle_close_count(void)
{
    sint32 count = 0;
    if (g_db_c_tbl != NULL) {
        qhashtbl_obj_t obj;
        memset((void*) &obj, 0, sizeof(obj));
        g_db_c_tbl->lock(g_db_c_tbl);
        while(g_db_c_tbl->getnext(g_db_c_tbl, &obj, false) == true) {
            DB_TAB_RECORD_INFO_T *data = (DB_TAB_RECORD_INFO_T*)(obj.data);
            if (data->tab_status == ENUM_TABLE_CLOSE) {
                count++;
            }
        }
        g_db_c_tbl->unlock(g_db_c_tbl);
    }

    return count;
}

char g_collectSQL[512] = { "(No INTEGER PRIMARY KEY AUTOINCREMENT,\
        LDeviceNo INT8,InfoAddrNo SMALLINT,PriDataDI INTEGER, SubDataDI INTEGER, DataTypeID SMALLINT, AcqStartTime INTEGER,\
        AcqSuccessTime INTEGER, AcqSaveTime INTEGER, AddCheck BLOB,DataContent BLOB)" };

/**
*********************************************************************
* @brief： 检查记录型数据库表结构是否正常
* @param[in]：
               tab_name           数据表名
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 check_collect_db(DB_TAB_RECORD_INFO_T *tab_info)
{
	sint32 rc 					= -1;
	sint32 nResult      		= -1;
	sint8 *zErrMsg			  	= NULL;
	sint8 buf[1024] 		  	= {0};
	sint8 data_type 		  	= ENUM_NOT_INIT_DATA;
	DB_RAW_T *tcollect		  	= NULL;
	sqlite3_stmt *pStmt		    = NULL;
	char *pTmp					= NULL;
	
	if (tab_info == NULL || tab_info->handle == NULL)
	{
        return rc;
    }

	tcollect = tab_info->handle;
	
	snprintf(buf, sizeof(buf), "SELECT sql FROM sqlite_master WHERE type='table' AND name='%s%d';", tab_info->tab_name, data_type);

	nResult = sqlite3_prepare_v2(tcollect->db_readhander_get(tcollect), buf, -1, &pStmt, 0);
	olog_debug(db_ocat,"sqlite3_prepare_v2 nResult:[%d] sql[%s]", nResult, buf);
    if (nResult == SQLITE_OK)
    {
    	sint8 loop_flag = 1;
        while (loop_flag)
        {
            sint8 retry = 3;
            while (retry > 1)
            {
                nResult = sqlite3_step(pStmt);
                if (nResult == SQLITE_DONE)
                {
                    olog_debug(db_ocat,"SQL sqlite3_step:[%d]", nResult);
                    retry     = 0;
                    loop_flag = 0;
                }
                else if (nResult != SQLITE_ROW)
                {
                    olog_debug(db_err, "SQL DB sqlite3_step! nResult:[%d]", nResult);
                    sqlite3_reset(pStmt);
                    nResult = sqlite3_prepare_v2(tcollect->db_readhander_get(tcollect), buf, -1, &pStmt, 0);
                    retry--;
                    continue;
                }
                else
                {
                    retry = 0;
                }
            }

            if (nResult != SQLITE_ROW)
            {
                break;
            }

            //取得列总数
            pTmp = strstr((const char *)sqlite3_column_text(pStmt, 0), g_collectSQL);
            if(pTmp != NULL)
            {
                rc = 0;		//库表结构正确
            }

			break;
			
        }

    }

	if (NULL != pStmt)
    {
        sqlite3_finalize(pStmt);
    }

	if(rc != 0)
	{
		olog_warn(db_ocat,"check_collect_db error recreate table:[%s]", tab_info->tab_name);
#ifdef AREA_FUJIAN
		for(data_type = ENUM_INIT_PARAM; data_type <= ENUM_NOT_INIT_DATA; data_type++)
#else
		for(data_type = ENUM_NORMAL_DATA; data_type <= ENUM_NOT_INIT_DATA; data_type++)
#endif
        {
            snprintf(buf, sizeof(buf), "DROP TABLE IF EXISTS %s%d;", tab_info->tab_name, data_type);
            nResult = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
            if (nResult != SQLITE_OK)
            {
                olog_warn(db_ocat, "delete table Fail nResult%d sql:%s ** err:%s \n", nResult, buf, zErrMsg);
                sqlite3_free(zErrMsg);
            }
        }
		db_close(tcollect);
	}

	return rc;
}

/**
*********************************************************************
* @brief： 创建记录型数据库
* @param[in]：
               tab_name           数据表名
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 create_collect_db(sint8 *tab_name)
{
    olog_debug(db_ocat, "%s, table[%s], begin", __FUNCTION__, tab_name);
    
    sint32 rc = S_OK;
    sint8 *zErrMsg            = NULL;
    sint8 db_name[1024]        = {0};
    sint8 buf[1024]           = {0};
    sint8 data_type           = 0;
    sint8 tab_name_type[64]   = {0}; //数据库按照数据类型分表存储
    DB_RAW_T *tcollect        = NULL;
    DB_TAB_RECORD_INFO_T info = {0};
    const sint8 *strsql       = "CREATE TABLE if not exists %s %s;";
    const sint8 *index_sql = "CREATE  INDEX if not exists i%s ON %s(InfoAddrNo, SubDataDI , AcqStartTime,  AcqSuccessTime, AcqSaveTime)";
    

    sprintf(db_name, "%s%s.db", g_configObj.dbCfg.basePath, tab_name);

    sint32 ret = check_need_to_open_db();
    if (ret == S_NG) {
        olog_debug(db_ocat, "%s, can not open %s", __FUNCTION__, tab_name);
        return ret;
    }

    //打开数据库
    if (db_open(db_name, NULL, DB_TYPE_RECORD, &tcollect) != S_OK)
    {
        olog_error(db_ocat, "db_open fail base:[%s]", db_name);
        return -1;
    }
#ifdef AREA_FUJIAN
	for(data_type = ENUM_INIT_PARAM; data_type <= ENUM_NOT_INIT_DATA; data_type++)
#else
    for(data_type = ENUM_NORMAL_DATA; data_type <= ENUM_NOT_INIT_DATA; data_type++)
#endif

    {
        snprintf(tab_name_type, sizeof(tab_name_type), "%s%d", tab_name, data_type);
        snprintf(buf, sizeof(buf), strsql, tab_name_type, g_collectSQL);
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "Create table Fail sql: %s", buf);
		    sqlite3_free(zErrMsg);
        }

        snprintf(buf, sizeof(buf), index_sql, tab_name_type, tab_name_type);
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "Create Index collec db Fail sql: %s", buf);
		    sqlite3_free(zErrMsg);
        }

    }

	g_db_c_tbl->lock(g_db_c_tbl);

    strncpy(info.tab_name, tab_name, sizeof(info.tab_name));
    info.handle = tcollect;
    info.tab_status = ENUM_TABLE_OPEN;
    info.refer_count = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &(info.tv));
    g_db_c_tbl->put(g_db_c_tbl, tab_name, &info, sizeof(info));

	g_db_c_tbl->unlock(g_db_c_tbl);

    olog_debug(db_record_status, "create table[%s], refer_cnt[%d], status[%d], tv_sec[%lu], tv_nsec[%lu]",
             info.tab_name, info.refer_count, info.tab_status, info.tv.tv_sec, info.tv.tv_nsec);

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }

    olog_debug(db_ocat, "%s, table[%s], end", __FUNCTION__, tab_name);
    return rc;
}

/**
*********************************************************************
* @brief： 获取记录型数据表信息
* @param[in]：
               device_no           设备标识
               infoAddrNo          信息点号
               prim_di             数据主DI
               sub_di              数据次DI
               create_flag         是否需要创建
* @param[out]：
               tab_info           数据表地址
* @return： 返回值说明
*********************************************************************
*/
sint32 get_collect_tab_info(uint8 device_no,
                            uint16 infoAddrNo,
                            uint32 prim_di,
                            uint32 sub_di,
                            DB_TAB_RECORD_INFO_T *tab_info,
                            sint8 create_flag)
{
    sint8 tab_name[64];
    size_t isize;
    sint32 ret = S_OK;
    sint32 i                    = (infoAddrNo == 0) ? 0 : 1;
    DB_TAB_RECORD_INFO_T *pinfo = 0;

    sprintf(tab_name, "T%02X%d%X", device_no, i, prim_di);
    olog_debug(db_ocat, "%s, table[%s], begin", __FUNCTION__, tab_name);
    
    //获取收集型数据
#ifdef DB_CENTER_DEBUG
//    g_db_c_tbl->debug(g_db_c_tbl, stdout);
#endif
    g_db_c_tbl->lock(g_db_c_tbl);

    pinfo = g_db_c_tbl->get(g_db_c_tbl, tab_name, &isize, false);
    if (pinfo == NULL || pinfo->tab_status != ENUM_TABLE_OPEN)
    {
        if (pinfo == NULL && create_flag == false)
        {
            ret = S_NG;
            goto end;
        }

        ret = create_collect_db(tab_name);
        if (ret == S_NG) 
        {
            goto end;
        }
        
        pinfo = g_db_c_tbl->get(g_db_c_tbl, tab_name, &isize, false);
        if (pinfo == NULL)
        {
        	ret = S_NG;
			olog_warn(db_ocat, "open table[%s] fail", tab_name);
            goto end;
        }

    }

	if(check_collect_db(pinfo) != 0)		//容错检查，防止数据库表结构不对
	{
		ret = create_collect_db(tab_name);
        if (ret == S_NG) 
        {
            goto end;
        }
        
        pinfo = g_db_c_tbl->get(g_db_c_tbl, tab_name, &isize, false);
        if (pinfo == NULL)
        {
        	ret = S_NG;
			olog_warn(db_ocat, "open table[%s] fail", tab_name);
            goto end;
        }
	}

	memcpy(tab_info, pinfo, sizeof(DB_TAB_RECORD_INFO_T));
 
end:
    if (pinfo != NULL) 
    {
        pinfo->refer_count = pinfo->refer_count + 1;
        clock_gettime(CLOCK_MONOTONIC_RAW, &(pinfo->tv));
		
        olog_debug(db_record_status, "++table[%s], refer_cnt[%d], status[%d], tv_sec[%lu], tv_nsec[%lu]",
                 pinfo->tab_name, pinfo->refer_count, pinfo->tab_status, pinfo->tv.tv_sec, pinfo->tv.tv_nsec);
		
    }  
    else
    {
        olog_warn(db_ocat, "get table info[%s] fail", tab_name);
    }
    g_db_c_tbl->unlock(g_db_c_tbl);
    olog_debug(db_ocat, "%s, table[%s], end", __FUNCTION__, tab_name);
    return ret;
}

sint32 put_collect_tab_info(uint8 device_no,
                            uint16 infoAddrNo,
                            uint32 prim_di,
                            uint32 sub_di)
{
    sint8 tab_name[64];
    size_t size;
    sint32 i = (infoAddrNo == 0) ? 0 : 1;
    DB_TAB_RECORD_INFO_T *pinfo = 0;

    sprintf(tab_name, "T%02X%d%X", device_no, i, prim_di);
    olog_debug(db_ocat, "%s, table name[%s]", __FUNCTION__, tab_name);

    g_db_c_tbl->lock(g_db_c_tbl);
    pinfo = g_db_c_tbl->get(g_db_c_tbl, tab_name, &size, false);
    if (pinfo != NULL)
    {
        pinfo->refer_count = pinfo->refer_count - 1;
        clock_gettime(CLOCK_MONOTONIC_RAW, &(pinfo->tv));
        olog_debug(db_record_status, "--table[%s], refer_cnt[%d], status[%d], tv_sec[%lu], tv_nsec[%lu]",
                 pinfo->tab_name, pinfo->refer_count, pinfo->tab_status, pinfo->tv.tv_sec, pinfo->tv.tv_nsec);
    }
    g_db_c_tbl->unlock(g_db_c_tbl);
    return S_OK;
}

sint32 close_collect_tab_info(sint8 *tab_name)
{
    size_t size;
    sint32 ret = S_OK;
    DB_TAB_RECORD_INFO_T *pinfo = 0;

    g_db_c_tbl->lock(g_db_c_tbl);
    pinfo = g_db_c_tbl->get(g_db_c_tbl, tab_name, &size, false);
    if (pinfo != NULL)
    {
        if (pinfo->tab_status != ENUM_TABLE_CLOSE) 
        {
            db_close(pinfo->handle);
            pinfo->tab_status = ENUM_TABLE_CLOSE;
            pinfo->refer_count = 0;
            
        }
    }

    g_db_c_tbl->unlock(g_db_c_tbl);
    olog_debug(db_ocat, "%s, table[%s], end", __FUNCTION__, tab_name);
    return ret;
}

/**
*********************************************************************
* @brief： 释放哈希表资源
* @param[in]：
               none
* @param[out]：
               none
* @return：    none
*********************************************************************
*/
void free_hash_tbl(void)
{
    if (g_db_c_tbl)
    {
        g_db_c_tbl->clear(g_db_c_tbl);
        g_db_c_tbl->free(g_db_c_tbl);
        g_db_c_tbl = NULL;
    }

    if (g_db_n_cfg_tbl)
    {
        g_db_n_cfg_tbl->clear(g_db_n_cfg_tbl);
        g_db_n_cfg_tbl->free(g_db_n_cfg_tbl);
        g_db_n_cfg_tbl = NULL;
    }

    if (g_tab_depth_cfgl)
    {
        g_tab_depth_cfgl->clear(g_tab_depth_cfgl);
        g_tab_depth_cfgl->free(g_tab_depth_cfgl);
        g_tab_depth_cfgl = NULL;
    }

    if (g_tab_info_cfg)
    {
        g_tab_info_cfg->clear(g_tab_info_cfg);
        g_tab_info_cfg->free(g_tab_info_cfg);
        g_tab_info_cfg = NULL;
    }
}

sint32 check_need_to_open_db(void)
{
    bool found = false;
    sint32 ret = S_OK;
	sint32 index = 0;
	DB_TAB_RECORD_INFO_T tmpInfo;
	qhashtbl_obj_t obj;
	DB_TAB_RECORD_INFO_T *data = NULL;
	
    if (g_db_c_tbl != NULL) 
	{
        //获取哈希表中元素的个数
        size_t handle_open_cnt = get_handle_open_count();
        olog_debug(db_ocat, "check_need_to_open_db, The opened db handle count[%zu]", handle_open_cnt);

		//如果打开的句柄过多，关闭无操作的句柄
        while (handle_open_cnt >= max_handle_count) 
		{
            found = false;
            memset(&tmpInfo, 0, sizeof(DB_TAB_RECORD_INFO_T));
            //遍历哈希表
            memset((void*) &obj, 0, sizeof(obj));
            while(g_db_c_tbl->getnext(g_db_c_tbl, &obj, false) == true) 
			{
                data = (DB_TAB_RECORD_INFO_T*)(obj.data);
                if (index == 0) 
                {
                    //获取第一个未被引用的数据库句柄信息
                    if ((data->refer_count == 0) && (data->tab_status == ENUM_TABLE_OPEN)) 
                    {
                        memcpy(&tmpInfo, data, sizeof(DB_TAB_RECORD_INFO_T));
                        found = true;
                        index++;
                    }
                    continue;
                }
                
                //通过比较，获取未被引用并且时间戳最小的数据库句柄
                if ((data->refer_count == 0) && (data->tab_status == ENUM_TABLE_OPEN)) 
                {
                    if (tmpInfo.tv.tv_sec > data->tv.tv_sec)
                    {
                        memcpy(&tmpInfo, data, sizeof(DB_TAB_RECORD_INFO_T));
                    } 
                    else if (tmpInfo.tv.tv_sec == data->tv.tv_sec) 
                    {
                        if (tmpInfo.tv.tv_nsec > data->tv.tv_nsec) 
                        {
                            memcpy(&tmpInfo, data, sizeof(DB_TAB_RECORD_INFO_T));
                        }
                    }
                }
            }

            //如果找到，则关闭未被应用的数据库句柄。否则返回错误
            if (found == true) 
            {
            	g_db_c_tbl->lock(g_db_c_tbl);
                olog_debug(db_record_status, "close table[%s]", tmpInfo.tab_name);
                db_close(tmpInfo.handle);
				
                tmpInfo.tab_status = ENUM_TABLE_CLOSE;
                //更新哈希表内容
                g_db_c_tbl->put(g_db_c_tbl, tmpInfo.tab_name, &tmpInfo, sizeof(DB_TAB_RECORD_INFO_T));

				g_db_c_tbl->unlock(g_db_c_tbl);

				handle_open_cnt --;
            } 
            else 
            {
                break;
            }
        }
    }

    return ret;
}

/**
*********************************************************************
* @brief： 读数据库文件列表
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明 S_NG 失败
                     S_OK 成功
*********************************************************************
*/
sint32 create_map_by_filelist(void)
{
    sint8 *basePath = g_configObj.dbCfg.basePath;
    DIR *dir;
    struct dirent *ptr;
    DB_TAB_RECORD_INFO_T tab_info = {0};
    sint32 handle_count = 0;
    //初始化收集型数据链表
    g_db_c_tbl = qhashtbl(0, QHASHTBL_THREADSAFE);

    if ((dir = opendir(basePath)) == NULL)
    {
        // perror("Open dir error...");
        olog_error(db_ocat, "Open dir error...");
        return S_NG;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        //过滤current dir OR parrent dir
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }
        //过滤普通和参数数据
        if (strncmp(ptr->d_name, DB_CENT_TABLE_NAME_NORMAL, 8) == 0
            || strncmp(ptr->d_name, DB_CENT_TABLE_NAME_PARAMETER, 10) == 0)
        {
            continue;
        }
        else if (/* ptr->d_type == 8 && */ ptr->d_name[0] == 'T')  /// file
        {
            sint32 name_len = strlen(ptr->d_name);
            memset(&tab_info, 0x00, sizeof(tab_info));
            if (!strncmp((const sint8 *)&ptr->d_name[name_len - 3], ".db", 3))
            {
                sint8 key_buf[64]  = {0};
                sint8 dbfile[1024]  = {0};
                sint8 *endp1       = 0;
                strncpy(key_buf, &ptr->d_name[1], 2);
                strtoul(key_buf, &endp1, 16);
                printf("key_buf:%s ;%d \n", key_buf, *endp1);
                if (endp1[0] != 0x00)
                {
                    olog_debug(db_ocat, "DB File Name's DeviceNo is incorrect! (%s%s) Fail. \n", basePath, ptr->d_name);
                    continue;
                }
                if (ptr->d_name[3] < '0' || ptr->d_name[3] > '9')
                {
                    olog_debug(db_ocat, "DB File Name's InfoAddress is incorrect! (%s%s) Fail. \n", basePath,
                               ptr->d_name);
                    continue;
                }
                memset(key_buf, 0x00, sizeof(key_buf));
                strncpy(key_buf, &ptr->d_name[4], name_len - 7);
                strtoul(key_buf, &endp1, 16);
                printf("key_buf:%s ;%d \n", key_buf, *endp1);
                if (endp1[0] != 0x00)
                {
                    olog_debug(db_ocat, "DB File Name's PriDI is incorrect! (%s%s) Fail. \n", basePath, ptr->d_name);
                    continue;
                }
                snprintf(dbfile, sizeof(dbfile), "%s%s", basePath, ptr->d_name);

                sint32 handle_open_cnt = get_handle_open_count();
                if (handle_open_cnt >= max_handle_count) {
                    //olog_debug(db_ocat, "open the specified number of files first");
                    tab_info.handle = NULL;
                    strncpy(key_buf, ptr->d_name, name_len - 3);
                    strncpy(tab_info.tab_name, ptr->d_name, name_len - 3);
                    tab_info.tab_status = ENUM_TABLE_CLOSE;
                    g_db_c_tbl->put(g_db_c_tbl, key_buf, (const void *)&tab_info, sizeof(DB_TAB_RECORD_INFO_T));
                    continue;
                }
                //打开数据库
                strncpy(key_buf, ptr->d_name, name_len - 3);
                create_collect_db(key_buf);
                handle_count++;
            }
        }
    }

    closedir(dir);
    //debug, just print info
    //g_db_c_tbl->debug(g_db_c_tbl, stdout);
    return S_OK;
}

/**
*********************************************************************
* @brief： 创建普通型数据库
* @param[in]：
               tab_name           数据表名
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 create_normal_db(const sint8 *dbName, const sint8 *dbPath, sint32 sqlType, DB_RAW_T **db)
{
	sint32 iResult 		  = -1;
    sint8 path[128]       = {0};
	sint8 slqstr[1024]    = {0};
    sint8 pathBackup[128] = {0};
    sint8 *sqlTable  = "CREATE TABLE if not exists %s(      \
                    LDeviceNo CHAR(1),     \
                    InfoAddrNo SMALLINT,      \
                    DataID INTEGER,     \
                    DataTypeID SMALLINT,      \
                    DataContent BLOB,     \
                    UpdateTime TIMESTAMP,    \
                    CRC INTEGER \
                    );";

    sint8 *sqlIndex = "CREATE UNIQUE INDEX if not exists i%s ON %s(LDeviceNo,InfoAddrNo,DataID);";
    
	snprintf(path,sizeof(path), "%s%s.db", dbPath, dbName);
	if(sqlType != DB_TYPE_NORMAL_MEMORY)
	{
		snprintf(pathBackup,sizeof(pathBackup), "%sbackup/%s.db", dbPath, dbName);
	}

	if (db_open(path, pathBackup, sqlType, db) != 0)
	{
		olog_error(db_ocat, "db_open fail base:[%s] backup:[%s]", path, pathBackup);
		return -1;
	}

	//执行SQL语句
	snprintf(slqstr,sizeof(slqstr), sqlTable, dbName);
	iResult = sqlite3_exec((*db)->db_writehander_get(*db), slqstr, NULL, NULL, NULL);
	if (SQLITE_OK != iResult)
	{
		olog_error(db_ocat, "Create Table Fail [%s] \r\n", slqstr);
		return -1;
	}
	//建立parameter Index
	snprintf(slqstr,sizeof(slqstr),  sqlIndex, dbName, dbName);
	//执行SQL语句
	iResult = sqlite3_exec((*db)->db_writehander_get(*db), slqstr, NULL, NULL, NULL);
	if (SQLITE_OK != iResult)
	{
		olog_error(db_ocat, "Create Index Fail [%s] \r\n", slqstr);
		return -1;
	}
	
	if(sqlType != DB_TYPE_NORMAL_MEMORY)
	{
		//执行SQL语句
		snprintf(slqstr,sizeof(slqstr), sqlTable, dbName);
		iResult = sqlite3_exec((*db)->db_writebackuphander_get(*db), slqstr, NULL, NULL, NULL);
		if (SQLITE_OK != iResult)
		{
			olog_error(db_ocat, "Create Table Fail [%s] \r\n", slqstr);
			return -1;
		}
		//建立parameter Index
		snprintf(slqstr,sizeof(slqstr), sqlIndex, dbName, dbName);
		//执行SQL语句
		iResult = sqlite3_exec((*db)->db_writebackuphander_get(*db), slqstr, NULL, NULL, NULL);
		if (SQLITE_OK != iResult)
		{
			olog_error(db_ocat, "Create Index Fail [%s] \r\n", slqstr);
			return -1;
		}
	}

	return iResult;
}

/**
*********************************************************************
* @brief： 普通型数据库初始化
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 db_normaldatadb_init(void)
{
    sint32 iResult;

    //建立parameter
    iResult = create_normal_db(DB_CENT_TABLE_NAME_PARAMETER, g_configObj.dbCfg.basePath, DB_TYPE_BACKUP, &g_parameter_tbl);
    if (SQLITE_OK != iResult)
    {
        olog_error(db_ocat, "Create dbParameterCenter Fail\r\n");
        return -1;
    }

	//建立Tgeneral
    iResult = create_normal_db(DB_CENT_TABLE_NAME_NORMAL, g_configObj.dbCfg.basePath, DB_TYPE_BACKUP, &g_generalr_tbl);
    if (SQLITE_OK != iResult)
    {
        olog_error(db_ocat, "Create dbGeneralCenter Fail\r\n");
        return -1;
    }

	//建立Tgeneral_delay
    iResult = create_normal_db(TGENERAL_DELAY_NAME, g_configObj.dbCfg.basePath, DB_TYPE_CACHE, &g_generalr_delay_tbl);
    if (SQLITE_OK != iResult)
    {
        olog_error(db_ocat, "Create dbGeneralDelayCenter Fail\r\n");
        return -1;
    }

	//建立TMemory
    iResult = create_normal_db(TMEMORY_NAME, g_configObj.dbCfg.tmpfsPath, DB_TYPE_NORMAL_MEMORY, &g_memory_tbl);
    if (SQLITE_OK != iResult)
    {
        olog_error(db_ocat, "Create TMemory Fail\r\n");
        return -1;
    }
    
    return 0;
}


/**
*********************************************************************
* @brief： 普通型数据库DeInit
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 db_normaldatadb_de_init(void)
{

    /*
    static DB_RAW_T *g_parameter_tbl;
    static DB_RAW_T *g_generalr_tbl;
    static DB_RAW_T *g_generalr_delay_tbl;
    */
    int ret;
    olog_info(db_ocat, "sqlite3_db_cacheflush g_parameter_tbl start");
    ret = sqlite3_db_cacheflush(g_parameter_tbl->db_writehander_get(g_parameter_tbl));
    olog_info(db_ocat, "sqlite3_db_cacheflush g_parameter_tbl end ret:%d",ret);
    if(ret != SQLITE_OK)
    {
        olog_error(db_ocat, "sqlite3_db_cacheflush g_parameter_tbl Fail");
    }

    olog_info(db_ocat, "sqlite3_db_cacheflush g_generalr_tbl start");
    ret = sqlite3_db_cacheflush(g_generalr_tbl->db_writehander_get(g_generalr_tbl));
    olog_info(db_ocat, "sqlite3_db_cacheflush g_generalr_tbl end ret:%d",ret);
    if(ret != SQLITE_OK)
    {
        olog_error(db_ocat, "sqlite3_db_cacheflush g_generalr_tbl Fail");
    }

    olog_info(db_ocat, "sqlite3_db_cacheflush g_generalr_delay_tbl start");
    ret = sqlite3_db_cacheflush(g_generalr_delay_tbl->db_writehander_get(g_generalr_delay_tbl));
    olog_info(db_ocat, "sqlite3_db_cacheflush g_generalr_delay_tbl end ret:%d",ret);
    if(ret != SQLITE_OK)
    {
        olog_error(db_ocat, "sqlite3_db_cacheflush g_generalr_delay_tbl Fail");
    }

    olog_info(db_ocat, "sqlite3_db_cacheflush g_memory_tbl start");
    ret = sqlite3_db_cacheflush(g_memory_tbl->db_writehander_get(g_memory_tbl));
    olog_info(db_ocat, "sqlite3_db_cacheflush g_memory_tbl end ret:%d",ret);
    if(ret != SQLITE_OK)
    {
        olog_error(db_ocat, "sqlite3_db_cacheflush g_memory_tbl Fail");
    }


    olog_info(db_ocat, "close g_parameter_tbl");
    db_close(g_parameter_tbl);
    olog_info(db_ocat, "close g_generalr_tbl");
    db_close(g_generalr_tbl);
    olog_info(db_ocat, "close g_generalr_delay_tbl");
    db_close(g_generalr_delay_tbl);
    olog_info(db_ocat, "close g_memory_tbl");
    db_close(g_memory_tbl);
    return 0;
}

/**
*********************************************************************
* @brief： 通过读数据库文件名，建立映射表，初始化参数信息
* @param[in]：
               none
* @param[out]：
               none
* @return：    0  返回值说明
*********************************************************************
*/
sint32 db_paramdatadb_init(void)
{
    return create_map_by_filelist();
}

/**
*********************************************************************
* @brief： db_record_deinit
* @param[in]：
               none
* @param[out]：
               none
* @return：    0  返回值说明
*********************************************************************
*/
sint32 db_record_deinit(void)
{
    DB_TAB_RECORD_INFO_T table;
    qhashtbl_obj_t obj;
    memset((void *) &obj, 0, sizeof(obj));  // must be cleared before call
    int ret = 0;

    g_db_c_tbl->lock(g_db_c_tbl);
    while (g_db_c_tbl->getnext(g_db_c_tbl, &obj, false) == true) 
    {
        DB_TAB_RECORD_INFO_T* data = (DB_TAB_RECORD_INFO_T*)(obj.data);
        if (data->tab_status == ENUM_TABLE_CLOSE) {
            continue;
        }
    
        memcpy(&table, data, sizeof(DB_TAB_RECORD_INFO_T));
        olog_info(db_ocat, "sqlite3_db_cacheflush table:%s start",table.tab_name);
        data->refer_count = data->refer_count + 1;
        clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));
        ret = sqlite3_db_cacheflush(table.handle->db_writehander_get(table.handle));
        olog_info(db_ocat, "sqlite3_db_cacheflush table:%s end ret:%d",table.tab_name,ret);
        if(ret != SQLITE_OK)
        {
            olog_error(db_ocat, "sqlite3_db_cacheflush g_generalr_delay_tbl Fail");
        }
        olog_info(db_ocat, "close table:%s",table.tab_name);
        db_close(table.handle);
        data->tab_status = ENUM_TABLE_CLOSE;
        data->refer_count = data->refer_count - 1;
        clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));
        if (data->refer_count != 0) {
            olog_warn(db_ocat, "The table[%s] is still refered", data->tab_name);
        }
        olog_debug(db_ocat, "--table[%s], refer_cnt[%d], status[%d], tv_sec[%lu], tv_nsec[%lu]",
                data->tab_name, data->refer_count, data->tab_status, data->tv.tv_sec, data->tv.tv_nsec);

    }
    g_db_c_tbl->unlock(g_db_c_tbl);

    return ret;
}

/**
*********************************************************************
* @brief： 定时事件回调函数
* @param[in]：
               signo    信号
* @param[out]：
               none
* @return：    0  返回值说明
*********************************************************************
*/
void sigroutine_disk(sint32 signo)
{
    switch (signo)
    {
        // case SIGVTALRM:
        case SIGUSR1:
            // printf("Check Disk Space Catch a signal -- SIGVTALRM \n");
            olog_debug(db_ocat, "disk_space id is %d\n", getpid());
            // signal(SIGVTALRM, sigroutine_disk);
            signal(SIGUSR1, sigroutine_disk);
            break;
        default:
            break;
    }

    return;
}

/**
*********************************************************************
* @brief： 信号触发删除函数
* @param[in]：
               signo    信号
* @param[out]：
               none
* @return：    无
*********************************************************************
*/
void sigrout_time_del_db(sint32 signo)
{
    switch (signo)
    {
        case SIGALRM:
            // printf("Catch a signal -- SIGVTALRM \n");
            // printf("fixed_time id is %d\n", getpid());
            olog_debug(db_ocat, "fixed_time id is %d \n", getpid());
            dele_db_disk_space();

            // signal(SIGALRM, sigrout_time_del_db);
            break;
        case SIGVTALRM:
            // printf("Catch a signal -- SIGVTALRM \n");
            // printf("fixed_time id is %d\n", getpid());
            // signal(SIGVTALRM, sigrout_time_del_db);
            break;
        default:
            break;
    }
    return;
}

/**
*********************************************************************
* @brief： 回收磁盘空间
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明 S_NG 失败
                     S_OK 成功
*********************************************************************
*/
sint32 reclaim_space(DB_TAB_RECORD_INFO_T *data)
{
    sint32 rc;
    sint8 *zErrMsg     = NULL;
    DB_RAW_T *tcollect = NULL;

//    g_db_c_tbl->lock(g_db_c_tbl);
    sint8 *tab_name    = data->tab_name;
    tcollect           = data->handle;

    //回收空间
    olog_debug(db_err, "%s, space reclamation table[%s] begin", __FUNCTION__, tab_name);
    //执行SQL语句
    rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), "VACUUM;", NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        olog_error(db_ocat, "reclaim_space vacuum Fail return[%d] err:%s", rc, zErrMsg);
	    sqlite3_free(zErrMsg);
    }

    olog_debug(db_err, "%s, space reclamation table[%s] end", __FUNCTION__, tab_name);

    return rc;
}
/**
*********************************************************************
* @brief： 获取库表数据的数量
* @param[in]：
               data           数据对象
               tab_name       库表名称
* @param[out]：
               pcount           数据数量
* @return： S_OK:成功；S_NG:失败
*********************************************************************
*/
sint32 get_data_count(DB_TAB_RECORD_INFO_T* data, sint8 *tab_name, uint32* pcount)
{
    sint8 buf[1024]       = {0};
    DB_RAW_T *tcollect    = NULL;
    sint32 nResult      = -1;  //执行结果
    sint32 nRc          = S_NG;  //函数返回值
    const sint8 *szTail = NULL;
    sqlite3_stmt *pVm   = NULL;  //预编译对象

    tcollect = data->handle;
    if (!tcollect)
    {
        olog_debug(db_ocat, "get_data_count tcollect is NULL");
        return 0;
    }
  
    snprintf(buf, sizeof(buf), "select count(*) from %s;", tab_name);

    //预编译SQL语句
    nResult = sqlite3_prepare_v2(tcollect->db_readhander_get(tcollect), buf, -1, &pVm, &szTail);
    if (nResult == SQLITE_OK)
    {
        //执行预编译的SQL语句
        nResult = sqlite3_step(pVm);
        //存在行记录
        if (nResult == SQLITE_ROW)
        {
            //判断查询到的列
            sint32 nCols = sqlite3_column_count(pVm);
            if (nCols >= 1)
            {
                //读取数据
                *pcount = sqlite3_column_int(pVm, 0);
                nRc = S_OK;
            }
        }
    }
    //释放掉预编译的sql语句
    sqlite3_finalize(pVm);

     return nRc;
}

/**
*********************************************************************
* @brief： 获取数据库所在磁盘剩余空间
* @param[in]：
* @param[out]：
               无
* @return： 
*********************************************************************
*/
uint64 get_free_disk()
{
    struct statfs diskInfo;
    uint64 freeDisk = 0;
    
    statfs(g_configObj.dbCfg.checkDataPath, &diskInfo);
    freeDisk    = diskInfo.f_bavail * diskInfo.f_bsize;
    
    return freeDisk;
}

/**
*********************************************************************
* @brief： 获取数据库文件大小
* @param[in]：
* @param[out]：
               无
* @return： 
*********************************************************************
*/
uint64 get_del_db_size(sint8 *tab_name)
{
    struct stat st;
    sint8 dbfile[1024]  = {0};
    
    snprintf(dbfile, sizeof(dbfile), "%s%s.db", g_configObj.dbCfg.basePath, tab_name);
    stat( dbfile, &st );
    return st.st_size;
}

/**
*********************************************************************
* @brief： 判断数据库中指定表是否存在
* @param[in]：
               data           数据库对象
               pTableName      表名
* @param[out]：
               pExist         存在标记，1存在，0不存在
* @return： S_OK:成功；S_NG:失败
*********************************************************************
*/
sint32 check_table_isexist(DB_TAB_RECORD_INFO_T* data,sint8 *pTableName, uint8* pExist)
{
    sint8 buf[1024]       = {0};
    DB_RAW_T *tcollect    = NULL;
    sint32 nResult      = -1;  //执行结果
    sint32 nRc          = S_NG;  //函数返回值
    const sint8 *szTail = NULL;
    sqlite3_stmt *pVm   = NULL;  //预编译对象
    sint8 *pSql       = "SELECT count(*) FROM sqlite_master WHERE type = 'table' AND name = '%s';";

    if ( data == NULL)
    {
        olog_error(db_ocat, "check_table_isexist 's data is NULL");
        return nRc;
    }

    tcollect = data->handle;
    if (!tcollect )
    {
        olog_error(db_ocat, "check_table_isexist 's tcollect  is NULL");
        return nRc;
    }
    
    do
	{
        snprintf(buf, sizeof(buf), pSql, pTableName);
		olog_debug(db_ocat, "check_table_isexist 's sql  is: %s", buf);

        //预编译SQL语句
        nResult = sqlite3_prepare_v2(tcollect->db_readhander_get(tcollect), buf, -1, &pVm, &szTail);
        if (nResult == SQLITE_OK)
        {
            //执行预编译的SQL语句
            nResult = sqlite3_step(pVm);
            //存在行记录
            if (nResult == SQLITE_ROW)
            {
                //判断查询到的列
                sint32 nCols = sqlite3_column_count(pVm);
                if (nCols >= 1)
                {
                    //读取数据
                    *pExist = sqlite3_column_int(pVm, 0);
                    nRc = S_OK;
                }
            }
        }
        //释放掉预编译的sql语句
        sqlite3_finalize(pVm);

    } while(0);

     return nRc;
}

/**
*********************************************************************
* @brief： 批量删除数据
* @param[in]：
               obj        指定要删除的数据对象
               needSpaceReclaim 是否需要内存回收
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 batch_dele_record(DB_TAB_RECORD_INFO_T *data)
{
    sint32 rc          = 0;
    sint8 buf[512]     = {0};
    sint8 tbl_type[64] = {0};
    sint8 *zErrMsg     = NULL;
#if 1
    sint8 data_type                  = 0;
#else
    sint8 data_type                  = ENUM_NORMAL_DATA;
#endif

    DB_RAW_T *tcollect = NULL;
    sint8 *tab_name    = data->tab_name;
    tcollect           = data->handle;
    if (!tab_name || !tcollect)
    {
        olog_debug(db_ocat, "!tab_name || !tcollect is NULL");
        return 0;
    }

    data->refer_count = data->refer_count + 1;
    clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));
    sint8 *sql_str = "delete from %s where No < (select No from %s order by No asc limit (select count(*) from %s)/2, 1);";
    while(data_type <= ENUM_NOT_INIT_DATA)
    {
        if (data_type == 0)
        {
            snprintf(tbl_type, sizeof(tbl_type), "%s", tab_name);
            data_type = ENUM_NORMAL_DATA;
        }
        else
        {
            snprintf(tbl_type, sizeof(tbl_type), "%s%d", tab_name, data_type);
            data_type++;
        }
        
        snprintf(buf, sizeof(buf), sql_str, tbl_type, tbl_type, tbl_type);
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_warn(db_ocat, "batch_dele_record Fail sql:%s ** err:%s", buf, zErrMsg);
            sqlite3_free(zErrMsg);
            continue;
        }

        sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
    }
        
//    sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    data->refer_count = data->refer_count - 1;
    clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));

    return reclaim_space(data);
    
}

/**
*********************************************************************
* @brief： 批量删除数据
* @param[in]：
               obj        指定要删除的数据对象
               needSpaceReclaim 是否需要内存回收
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 batch_dele_record_safe(DB_TAB_RECORD_INFO_T *data)
{
#if 1
    sint8 data_type                  = 0;
#else
    sint8 data_type                  = ENUM_NORMAL_DATA;
#endif
    sint32 rc;
    sint8 buf[1024]      = {0};
    sint8 tbl_type[64]   = {0};
    sint8 *zErrMsg       = NULL;
    DB_RAW_T *tcollect   = NULL;
    sint8 *tab_name      = NULL;
    uint64 freeDisk      = 0;
    uint64 db_size       = 0;
    uint32 del_count     = 0;
    uint32 del_total     = 0;
        
    tab_name = data->tab_name;
    tcollect = data->handle;
    if (!tcollect || !tab_name)
    {
        olog_debug(db_ocat, "batch_dele_record 's tcollect or tab_name is NULL");
        return S_NG;
    }

    while(data_type <= ENUM_NOT_INIT_DATA)
    {
        if (data_type == 0)
        {
            snprintf(tbl_type, sizeof(tbl_type), "%s", tab_name);
            data_type = ENUM_NORMAL_DATA;
        }
        else
        {
            snprintf(tbl_type, sizeof(tbl_type), "%s%d", tab_name, data_type);
            data_type++;
        }

		//数据库句柄之前是关闭的，检查数据库表是否存在
		if (data->tab_status == ENUM_TABLE_CLOSE || data_type == ENUM_NORMAL_DATA)	//data_type = ENUM_NORMAL_DATA, 检查旧数据库是否有数据
        {
        	uint8 tabEllaxist		= 0;
            //判断需要删除的库表是否存在
			rc = check_table_isexist(data, tbl_type, &tabEllaxist);
			if (rc == S_NG)
			{
		        olog_error(db_ocat, "batch_dele_record_safe 's check table error");
		        return S_NG;
		    }

			//库表不存在，直接返回，不需要删除
			if (tabEllaxist == 0)
			{
		        olog_debug(db_ocat, "table is not exist, data is null");
		        continue;
		    }
        }

        freeDisk = get_free_disk();
        db_size = get_del_db_size(tab_name);

        //磁盘空间足够
        if (freeDisk > db_size*3)
        {
            //执行SQL语句
            snprintf(buf, sizeof(buf), "delete from %s where No < (select No from %s order by No asc limit (select count(*) from %s)/2, 1);", tbl_type, tbl_type, tbl_type);
            olog_debug(db_ocat, "batch_dele_record sql  %s", buf);
            rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_warn(db_ocat, "batch_dele_record Fail sql:%s ** err:%s", buf, zErrMsg);
                sqlite3_free(zErrMsg);
                continue;
            }

//            sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
        }
        else
        {
            rc = get_data_count(data, tbl_type, &del_total);
            if (rc != S_OK)
            {
                olog_warn(db_ocat, "get_data_count table[%s] err\n", tbl_type);
                continue;
            }

            if (del_total == 0)
            {
                olog_debug(db_ocat, "get_data_count table[%s] del_total0\n", tbl_type);
                continue;
            }

            del_total = del_total/2;          //对半删除
            
            if ((freeDisk/STEP_DATA_SIZE)/10 > del_total) //磁盘空间足够,数据占用空间估算，1w条数据大概占用1M空间，这里扩充10倍
            {
                //执行SQL语句
                snprintf(buf, sizeof(buf), "delete from %s where No < (select No from %s order by No asc limit %u, 1);", tbl_type, tbl_type, del_total);
                olog_debug(db_ocat, "batch_dele_record sql  %s", buf);
                rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
                if (rc != SQLITE_OK)
                {
                    olog_warn(db_ocat, "batch_dele_record Fail sql:%s ** err:%s", buf, zErrMsg);
                    sqlite3_free(zErrMsg);
                    continue;
                }

//                sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
            }
            else
            {
                while(del_total > 0)
                {
                    del_count = (freeDisk/STEP_DATA_SIZE)/10;        //一次删除空间的10%的数据

                    snprintf(buf, sizeof(buf), "delete from %s where No < (SELECT No FROM %s ORDER BY No ASC LIMIT  %u , 1 );", tbl_type, tbl_type, del_count);
                    
                    olog_debug(db_ocat, "batch_dele_record sql  %s", buf);
                    //执行SQL语句
                    rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
                    if (rc != SQLITE_OK)
                    {
                        olog_error(db_ocat, "delete table Fail sql:%s ** err:%s \n", buf, zErrMsg);
                        sqlite3_free(zErrMsg);
                        rc = S_NG;
                        break;
                    }

//                    sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);

                    if (del_count < del_total )     //数据未删除完
                    {
                        del_total -= del_count;
                        freeDisk = get_free_disk();
                    }
                    else
                    {
                        break;
                    }
                    
                }

            }
            
        }
        
    }
    
    sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    data->refer_count = data->refer_count - 1;
    clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));

    return reclaim_space(data);
    
}


/**
*********************************************************************
* @brief： 触发时循环删除数据
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明 S_NG 失败
                     S_OK 成功
*********************************************************************
*/
sint32 trig_dele_records(void)
{
    qhashtbl_obj_t obj;
    DB_RAW_T *tcollect = NULL;
    qhashtbl_t *tbl = g_db_c_tbl;
	DB_TAB_RECORD_INFO_T info;
    DB_TAB_RECORD_INFO_T* data;
    olog_warn(db_ocat, "%s", __FUNCTION__);
    if (!tbl)
    {
        olog_error(db_ocat, "trig_dele_records's tbl 为空 \n");
        return S_NG;
    }

    memset((void *)&obj, 0, sizeof(obj));
    
    while (tbl->getnext(tbl, &obj, false) == true)
    {
        data = (DB_TAB_RECORD_INFO_T*)(obj.data);
        olog_debug(db_ocat, "====trig_dele_records, table[%s]", data->tab_name);
        if (data->tab_status == ENUM_TABLE_CLOSE) 
        {
            olog_debug(db_ocat, "The table[%s] has already closed, so need to reopen", data->tab_name);
            sint8 db_name[1024] = {0};
            
            sprintf(db_name, "%s%s.db", g_configObj.dbCfg.basePath, data->tab_name);
            sint32 rc = db_open(db_name, NULL, DB_TYPE_RECORD, &tcollect);
            if (rc == S_OK) 
            {
                memset(&info, 0, sizeof(DB_TAB_RECORD_INFO_T));
                strcpy(info.tab_name, data->tab_name);
                info.handle = tcollect;
				info.tab_status = ENUM_TABLE_CLOSE;
                batch_dele_record_safe(&info);
                db_close(tcollect);
            }
        }
        else 
        {
            olog_debug(db_ocat, "the table[%s] is opend", data->tab_name);
            batch_dele_record_safe(data);
			tbl->lock(tbl);
            db_close(data->handle);
            data->tab_status = ENUM_TABLE_CLOSE;
            tbl->unlock(tbl);
        }
        
        usleep(1000);
    }
    
    return S_OK;
}
/**
*********************************************************************
* @brief： 根据磁盘空间触发删除数据
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 dele_db_disk_space(void)
{
    struct statfs diskInfo;
     uint64 totalSize;
    size_t mbTotalsize;
    uint64 freeDisk;
    size_t mbFreedisk;
    uint64 totalBlocks;
    sint8 *checkPath = g_configObj.dbCfg.checkDataPath;
    statfs(checkPath, &diskInfo);

    totalBlocks = diskInfo.f_bsize;
     totalSize = totalBlocks * diskInfo.f_blocks;
    mbTotalsize = totalSize >> 20;
    freeDisk    = diskInfo.f_bavail * totalBlocks;
    mbFreedisk  = freeDisk >> 20;
    olog_debug(db_ocat, "%s  total=%dMB, free=%dMB\n", checkPath, mbTotalsize, mbFreedisk);
    float rate = (float)mbFreedisk / (float)mbTotalsize;
    float rateLimit =  (float)g_configObj.dbCfg.autoDeleteNum /100;
    olog_debug(db_ocat, "now disk free rate[%f], free rate limt[%f]", rate, rateLimit);
    if (rate < rateLimit)
    {
        olog_info(db_ocat, "%s  total=%dMB, free=%dMB", checkPath, mbTotalsize, mbFreedisk);
        trig_dele_records();
    }

    return S_OK;
}
/**
*********************************************************************
* @brief： 定期删除读数据库
* @param[in]：
               none
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 dele_db_fixed_time(void)
{
    struct itimerval value, ovalue;  //(1)
    // printf("dele_db_fixed_time id is %d\n", getpid());
    olog_debug(db_ocat, "dele_db_fixed_time id is %d", getpid());
    signal(SIGALRM, sigrout_time_del_db);

    value.it_value.tv_sec     = DB_DELE_TIMER_INTERVAL;
    value.it_value.tv_usec    = 0;
    value.it_interval.tv_sec  = DB_DELE_TIMER_INTERVAL;
    value.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &value, &ovalue);  //(2)
    return S_OK;
}

/**
*********************************************************************
* @brief： 获取删除数据的数量
* @param[in]：
               data           数据对象
               logi_dev      逻辑设备号
               dataTypeIDOptioal  数据类型ID是否有效
               dataTypeID         数据类型ID
* @param[out]：
               pdel_count           需删除的数量
* @return： S_OK:成功；S_NG:失败
*********************************************************************
*/
sint32 get_del_data_count(DB_TAB_RECORD_INFO_T* data,uint8 logi_dev, sint8 dataTypeIDOptioal, sint16 dataTypeID, uint32* pdel_count)
{
    sint8 buf[1024]       = {0};
    sint8 condition[128]  = {0};
    DB_RAW_T *tcollect    = NULL;
    sint8 *tab_name       = NULL;
    sint32 nResult      = -1;  //执行结果
    sint32 nRc          = S_NG;  //函数返回值
    const sint8 *szTail = NULL;
    sqlite3_stmt *pVm   = NULL;  //预编译对象
    sint8 *sql_sel       = "select count(*) from %s where LDeviceNo=%u %s;";

    if (dataTypeIDOptioal)      //数据初始化，指定数据类 
    {
        snprintf(condition, sizeof(condition), "and DataTypeID=%d", dataTypeID);
    }
    else                        //数据初始化，或恢复出厂 6—不可初始化事件数据，保留
    {
        snprintf(condition, sizeof(condition), "and DataTypeID!=%d", ENUM_NOT_INIT_DATA);
    }
        
    tab_name = data->tab_name;
    if ( !tab_name)
    {
        olog_debug(db_ocat, "get_del_data_count 's tab_name is NULL");
        return S_NG;
    }

    tcollect = data->handle;
    if (!tcollect )
    {
        olog_debug(db_ocat, "get_del_data_count 's tcollect  is NULL");
        return S_NG;
    }
    
    do {
        snprintf(buf, sizeof(buf), sql_sel, tab_name, logi_dev, condition);

        //预编译SQL语句
        nResult = sqlite3_prepare_v2(tcollect->db_readhander_get(tcollect), buf, -1, &pVm, &szTail);
        if (nResult == SQLITE_OK)
        {
            //执行预编译的SQL语句
            nResult = sqlite3_step(pVm);
            //存在行记录
            if (nResult == SQLITE_ROW)
            {
                //判断查询到的列
                sint32 nCols = sqlite3_column_count(pVm);
                if (nCols >= 1)
                {
                    //读取数据
                    *pdel_count = sqlite3_column_int(pVm, 0);
                    nRc = S_OK;
                }
            }
        }
        //释放掉预编译的sql语句
        sqlite3_finalize(pVm);

    } while(0);

     return nRc;
}

/**
*********************************************************************
* @brief： 获取删除数据的数量
* @param[in]：
               data           数据对象
               logi_dev      逻辑设备号
               dataTypeIDOptioal  数据类型ID是否有效
               dataTypeID         数据类型ID
* @param[out]：
               pdel_count           需删除的数量
* @return： S_OK:成功；S_NG:失败
*********************************************************************
*/
sint32 get_evt_uninit(DB_TAB_RECORD_INFO_T* data, uint32* pevt_count)
{
    sint8 buf[1024]       = {0};
    DB_RAW_T *tcollect    = NULL;
    sint8 *tab_name       = NULL;
    sint32 nResult      = -1;  //执行结果
    sint32 nRc          = S_NG;  //函数返回值
    const sint8 *szTail = NULL;
    sqlite3_stmt *pVm   = NULL;  //预编译对象
        
    tab_name = data->tab_name;
    tcollect = data->handle;
    if (!tcollect || !tab_name)
    {
        olog_debug(db_ocat, "get_del_data_count 's tcollect or tab_name is NULL");
        return S_NG;
    }
    
    do {
        snprintf(buf, sizeof(buf), "select count(*) from %s where DataTypeID=%d;", tab_name, ENUM_NOT_INIT_DATA);

        //预编译SQL语句
        nResult = sqlite3_prepare_v2(tcollect->db_readhander_get(tcollect), buf, -1, &pVm, &szTail);
        if (nResult == SQLITE_OK)
        {
            //执行预编译的SQL语句
            nResult = sqlite3_step(pVm);
            //存在行记录
            if (nResult == SQLITE_ROW)
            {
                //判断查询到的列
                sint32 nCols = sqlite3_column_count(pVm);
                if (nCols >= 1)
                {
                    //读取数据
                    *pevt_count = sqlite3_column_int(pVm, 0);
                    nRc = S_OK;
                }
            }
        }
        //释放掉预编译的sql语句
        sqlite3_finalize(pVm);

    } while(0);

     return nRc;
}

/**
*********************************************************************
* @brief： 安全的删除4.0版本的数据，防止数据库过大，磁盘空间不够，一次删除，wal文件增大死机,
* @param[in]：
               obj           数据对象
               logi_dev      逻辑设备号
               dataTypeIDOptioal  数据类型ID是否有效
               dataTypeID         数据类型ID
               needSpaceReclaim   是否需要空间回收
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 dele_old_data_safe(DB_TAB_RECORD_INFO_T* data, uint8 logi_dev, sint8 dataTypeIDOptioal, sint16 dataTypeID)
{
    sint32 rc;
    sint8 buf[1024]      = {0};
    sint8 sql_buf[1024]  = {0};
    sint8 condition[128] = {0};
    sint8 *zErrMsg       = NULL;
    DB_RAW_T *tcollect   = NULL;
    sint8 *tab_name      = NULL;
    uint64 freeDisk      = 0;
    uint64 db_size       = 0;
    uint32 del_count     = 0;
    uint32 del_total     = 0;
        
    tab_name = data->tab_name;
    tcollect = data->handle;
    if (!tcollect || !tab_name)
    {
        olog_debug(db_ocat, "dele_data_logi_dev 's tcollect or tab_name is NULL");
        return S_NG;
    }

	uint8 tabEllaxist		= 0;
    //判断需要删除的库表是否存在
	rc = check_table_isexist(data, tab_name, &tabEllaxist);
	if (rc == S_NG)
	{
        olog_error(db_ocat, "dele_old_data_safe 's check table error");
        return S_NG;
    }

	//库表不存在，直接返回，不需要删除
	if (tabEllaxist == 0)
	{
        olog_debug(db_ocat, "table is not exist, data is null");
        return S_OK;
    }	

    if (dataTypeIDOptioal == 0)      //数据初始化
    {
        //执行SQL语句
        snprintf(buf, sizeof(buf), "DROP TABLE IF EXISTS %s;", tab_name);
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_warn(db_ocat, "delete table Fail sql:%s ** err:%s \n", buf, zErrMsg);
            sqlite3_free(zErrMsg);
            return S_NG;
        }
        sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
        return rc;
    }

    snprintf(sql_buf, sizeof(sql_buf), "delete from %s where DataTypeID=%d", tab_name, dataTypeID);

    //evt_uninit!=0,有不可初始化的事件数据
    
    freeDisk = get_free_disk();
    db_size = get_del_db_size(tab_name);

    //磁盘空间足够
    if (freeDisk > db_size*3)
    {
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), sql_buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_warn(db_ocat, "delete table Fail sql:%s ** err:%s \n", sql_buf, zErrMsg);
            sqlite3_free(zErrMsg);
            rc = S_NG;
        }

        sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
    }
    else
    {
        rc = get_del_data_count(data, logi_dev, dataTypeIDOptioal, dataTypeID, &del_total);
        if (rc != S_OK)
        {
            olog_warn(db_ocat, "get_del_data_count err\n");
            return rc;
        }
        if ((freeDisk/STEP_DATA_SIZE)/10 > del_total) //磁盘空间足够
        {
            //执行SQL语句
            rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), sql_buf, NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_warn(db_ocat, "delete table Fail sql:%s ** err:%s \n", sql_buf, zErrMsg);
                sqlite3_free(zErrMsg);
                rc = S_NG;
            }

        }
        else
        {
            while(del_total > 0)
            {
                del_count = (freeDisk/STEP_DATA_SIZE)/10;        //一次删除空间的10%的数据

                snprintf(condition, sizeof(condition), "and No < (SELECT No FROM %s ORDER BY No ASC LIMIT  %u , 1 );", tab_name, del_count >del_total? del_total: del_count);
                snprintf(buf, sizeof(buf), "%s %s", sql_buf, condition);
                
                olog_debug(db_ocat, "dele_data_safe sql  %s", buf);
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
                if (rc != SQLITE_OK)
                {
                    olog_warn(db_ocat, "delete table Fail sql:%s ** err:%s \n", buf, zErrMsg);
                    sqlite3_free(zErrMsg);
                    rc = S_NG;
                    break;
                }

                sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);

                if (del_count < del_total )     //数据未删除完
                {
                    del_total -= del_count;
                    freeDisk = get_free_disk();
                }
                else
                {
                    break;
                }
                
            }

        }
        
    }

    return rc;
}

/**
*********************************************************************
* @brief： 通过逻辑设备删除数据
* @param[in]：
               obj           数据对象
               logi_dev      逻辑设备号
               isFactory 	 恢复出厂消息
               dataTypeIDOptioal  数据类型ID是否有效
               dataTypeID         数据类型ID
               needSpaceReclaim   是否需要空间回收
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 dele_data_logi_dev(DB_TAB_RECORD_INFO_T* data, uint8 logi_dev, uint8 isFactory, sint8 dataTypeIDOptioal, sint16 dataTypeID, bool needSpaceReclaim)
{
    sint32 rc;
    // sint32 max_dele_sum = DB_MAX_DELE_SUM;
    sint8 buf[1024]      = {0};
    sint8 *zErrMsg       = NULL;
    DB_RAW_T *tcollect   = NULL;
    sint8 *tab_name      = NULL;
    sint8 data_type      = ENUM_NORMAL_DATA;

    if (!data)
    {
        olog_error(db_ocat, "dele_data_logi_dev's data 为空 \n");
        return S_NG;
    }
        
    tcollect = data->handle;
    tab_name = data->tab_name;
    if (!tcollect || !tab_name)
    {
        olog_debug(db_ocat, "dele_data_logi_dev 's tcollect || tab_name is NULL");
		return S_NG;
    }       
        
    data->refer_count = data->refer_count + 1;
    clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));
    
    if (dataTypeIDOptioal == 0)      //数据初始化，没有指定数据类 6—不可初始化事件数据，保留
    {

    	if (isFactory == 1)
	    {
#ifdef AREA_FUJIAN	    
	        data_type = ENUM_INIT_PARAM;
#endif
	    }

        for(; data_type < ENUM_NOT_INIT_DATA; data_type++)
        {
            snprintf(buf, sizeof(buf), "DROP TABLE IF EXISTS %s%d;", tab_name, data_type);
            olog_debug(db_ocat, "dele_data_logi_dev sql  %s", buf);
            rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_warn(db_ocat, "delete table Fail sql:%s ** err:%s \n", buf, zErrMsg);
                sqlite3_free(zErrMsg);
                continue;
            }

            sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
        }
    }
    else
    {
        snprintf(buf, sizeof(buf), "DROP TABLE IF EXISTS %s%d;", tab_name, dataTypeID);
        olog_debug(db_ocat, "dele_data_logi_dev sql  %s", buf);
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete table Fail sql:%s ** err:%s \n", buf, zErrMsg);
            sqlite3_free(zErrMsg);
//            return S_NG;
        }

        sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
    }


#ifdef SPACE_RECLAMATION_SWITCH
    if (needSpaceReclaim) {
        //回收空间
        reclaim_space(data);
    }
#else
    olog_debug(db_ocat, "space reclamation is closed");
#endif

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }

    return S_OK;
}

/**
*********************************************************************
* @brief： 通过逻辑设备和信息点号删除指定数据类型数据
* @param[in]：
               obj           数据对象
               logi_dev      逻辑设备号
               infoNum              信息点号
               dataTypeIDOptioal    数据类型ID是否有效
               dataTypeID           数据类型ID
               needSpaceReclaim     是否需要空间回收
* @param[out]：
               none
* @return： 返回值说明  S_NG    失败
                      S_OK    成功
*********************************************************************
*/
sint32 dele_data_logi_dev_info_number(DB_TAB_RECORD_INFO_T *data,
                                      uint8 logi_dev,
                                      uint16 infoNum,
                                      sint16 dataTypeID,
                                      bool needSpaceReclaim)
{
    sint32 rc						  = S_NG;
    sint8 buf[512]                    = {0};
    sint8 condition_info_number[128]  = {0};
    sint8 *zErrMsg                    = NULL;
    DB_RAW_T *tcollect                = NULL;
    sint8 tabName[64]                 = {0};


    if (!data)
    {
        olog_error(db_ocat, "dele_data_logi_dev_info_number's obj 为空 \n");
        return S_NG;
    }

    if (data->handle == NULL || data->tab_name == NULL)
    {
        olog_error(db_ocat, "dele_data_logi_dev_info_number 's handle || tab_name is NULL");
        return S_NG;
    }

	tcollect = data->handle;

	snprintf(tabName, sizeof(tabName), "%s%d", data->tab_name, dataTypeID);

	//数据库句柄之前是关闭的，检查数据库表是否存在
	if (data->tab_status == ENUM_TABLE_CLOSE)
    {
    	uint8 tabEllaxist		= 0;
        //判断需要删除的库表是否存在
		rc = check_table_isexist(data, tabName, &tabEllaxist);
		if (rc == S_NG)
		{
	        olog_error(db_ocat, "dele_data_logi_dev_info_number 's check table error");
	        return S_NG;
	    }

		//库表不存在，直接返回，不需要删除
		if (tabEllaxist == 0)
		{
	        olog_debug(db_ocat, "table is not exist, data is null");
	        return S_OK;
	    }
    }	
    
    sint8 *sql_del     = "delete from %s %s;";
    sint8 *sql_upd     = "update sqlite_sequence set seq=0 where name='%s'";  //更新主键从1开始
    uint8 tableInfoNum = data->tab_name[3] - 0x30;

    if (infoNum != 0xffff)
    {
        snprintf(condition_info_number, sizeof(condition_info_number), "where InfoAddrNo=%u", infoNum);
        if ((infoNum == 0 && tableInfoNum != 0) || (infoNum > 0 && tableInfoNum == 0))
        {
            olog_warn(db_ocat, "infoNum[%d] miss match  tableInfoNum[%d]\n", infoNum, tableInfoNum);
            return S_OK;
        }
    }

    do
	{
        snprintf(buf, sizeof(buf), sql_del, tabName, condition_info_number);
        dzlog_debug("sql:[%s]", buf);
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete table Fail sql:%s ** err:%s \n", buf, zErrMsg);
        	sqlite3_free(zErrMsg);
            return S_NG;
        }

        memset(buf, 0x00, sizeof(buf));
        snprintf(buf, sizeof(buf), sql_upd, tabName);
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "Update sqlite_sequence Fail sql:%s ** err:%s", buf, zErrMsg);
        	sqlite3_free(zErrMsg);
            return S_NG;
        }

        sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);
        
    }while(0);

#ifdef SPACE_RECLAMATION_SWITCH
    if (needSpaceReclaim) 
    {
        //回收空间
//        reclaim_space(data);
    }
#else
    olog_debug(db_ocat, "space reclamation is closed");
#endif
    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    return S_OK;
}

/**
*********************************************************************
* @brief： 通过逻辑设备和信息点号删除老版本数据
* @param[in]：
               obj           数据对象
               logi_dev      逻辑设备号
               infoNum              信息点号
               dataTypeIDOptioal    数据类型ID是否有效
               dataTypeID           数据类型ID
               needSpaceReclaim     是否需要空间回收
* @param[out]：
               none
* @return： 返回值说明  S_NG    失败
                      S_OK    成功
*********************************************************************
*/
sint32 dele_old_data_logi_dev_info_number(DB_TAB_RECORD_INFO_T *data,
                                      uint8 logi_dev,
                                      uint16 infoNum,
                                      sint8 dataTypeIDOptioal,
                                      sint16 dataTypeID,
                                      bool needSpaceReclaim)
{
    sint32 rc;
    // sint32 max_dele_sum = DB_MAX_DELE_SUM;
    sint8 buf[512]                    = {0};
    sint8 condition_data_type_ID[128] = {0};
    sint8 condition_info_number[128]  = {0};
    sint8 *zErrMsg                    = NULL;
    DB_RAW_T *tcollect                = NULL;
	sint8 *tab_name					  = NULL;

	if (!data)
    {
        olog_error(db_ocat, "dele_old_data_logi_dev_info_number's obj 为空 \n");
        return S_NG;
    }

    if (data->handle == NULL || data->tab_name == NULL)
    {
        olog_error(db_ocat, "dele_old_data_logi_dev_info_number 's handle || tab_name is NULL");
        return S_NG;
    }

	tcollect = data->handle;
	tab_name    = data->tab_name;

	uint8 tabEllaxist		= 0;
    //判断需要删除的库表是否存在
	rc = check_table_isexist(data, tab_name, &tabEllaxist);
	if (rc == S_NG)
	{
        olog_error(db_ocat, "dele_old_data_logi_dev_info_number 's check table error");
        return S_NG;
    }

	//库表不存在，直接返回，不需要删除
	if (tabEllaxist == 0)
	{
        olog_debug(db_ocat, "table is not exist, data is null");
        return S_OK;
    }

    
    sint8 *sql_del     = "delete from %s %s %s;";
    sint8 *sql_upd     = "update sqlite_sequence set seq=0 where name='%s'";  //更新主键从1开始
    uint8 tableInfoNum = tab_name[3] - 0x30;

    if (dataTypeIDOptioal)
    {
        snprintf(condition_data_type_ID, sizeof(condition_data_type_ID), "where DataTypeID=%d", dataTypeID);
    }

    if (infoNum != 0xffff)
    {
        if (strlen(condition_data_type_ID) == 0)
        {
            snprintf(condition_info_number, sizeof(condition_info_number), "where InfoAddrNo=%u", infoNum);
        }
        else
        {
            snprintf(condition_info_number, sizeof(condition_info_number), "and InfoAddrNo=%u", infoNum);
        }
            
        if ((infoNum == 0 && tableInfoNum != 0) || (infoNum > 0 && tableInfoNum == 0))
        {
            olog_warn(db_ocat, "infoNum[%d] miss match  tableInfoNum[%d]\n", infoNum, tableInfoNum);
            return 0;
        }
    }

    tcollect = data->handle;
    if (!tcollect || !tab_name)
    {
        olog_debug(db_ocat, "dele_data_logi_dev 's tcollect || tab_name is NULL");
    }

    snprintf(buf, sizeof(buf), sql_del, tab_name, condition_data_type_ID, condition_info_number);
    dzlog_debug("sql:[%s]", buf);
    //执行SQL语句
    rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        olog_warn(db_ocat, "delete table Fail sql:%s ** err:%s \n", buf, zErrMsg);
		sqlite3_free(zErrMsg);
        return S_NG;
    }

    memset(buf, 0x00, sizeof(buf));
    snprintf(buf, sizeof(buf), sql_upd, tab_name);
    //执行SQL语句
    rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        olog_warn(db_ocat, "Update sqlite_sequence Fail sql:%s ** err:%s", buf, zErrMsg);
		sqlite3_free(zErrMsg);
        return S_NG;
    }

    sqlite3_exec(tcollect->db_writehander_get(tcollect), "COMMIT;", NULL, NULL, NULL);

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    return S_OK;
}                                   

/**
*********************************************************************
* @brief： 数据/参数初始化
* @param[in]：
               logi_dev       逻辑设备号
               flag           (0:初始化；１：恢复出厂)
               dataTypeIDOptioal    数据类型ID是否有效
               dataTypeID           数据类型ID
               needSpaceReclaim     是否需要空间回收
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 gen_param_data_init_logi_dev(uint8 logi_dev, sint8 flag, uint8 dataTypeIDOptioal, uint16 dataTypeID, bool needSpaceReclaim)
{
    //数据和参数
    sint8 buf[512]       = {0};
    sint8 condition[128] = {0};
    DB_RAW_T *tcollect   = NULL;
    sint8 *zErrMsg       = NULL;
    int rc;

    if (dataTypeIDOptioal)
    {
        snprintf(condition, sizeof(condition), "and DataTypeID=%d", dataTypeID);
    }
    else                        //数据初始化，或恢复出厂 6—不可初始化事件数据，保留
    {
        snprintf(condition, sizeof(condition), "and DataTypeID!=%d", ENUM_NOT_INIT_DATA);
    }

    //数据/参数删除
    tcollect = get_Tgeneral();
    if (tcollect)
    {
        sint8 *sql_del = "delete from dbGeneralCenter where LDeviceNo=%u %s;";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, condition);

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete dbGeneralCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

#ifdef SPACE_RECLAMATION_SWITCH
        if (needSpaceReclaim) {
            //回收空间
            olog_debug(db_err, "%s, space reclamation table[dbGeneralCenter] begin", __FUNCTION__);
            //执行SQL语句
            rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "gen_param_data_init_logi_dev general vacuum Fail ** err:%s", zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            else
            {
                //同步
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_readhander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            olog_debug(db_err, "%s, space reclamation table[dbGeneralCenter] end", __FUNCTION__);
        }
#else
        olog_debug(db_ocat, "space reclamation is closed");
#endif
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

#ifdef SPACE_RECLAMATION_SWITCH
        if (needSpaceReclaim) {
            //回收空间
            olog_debug(db_err, "%s, space reclamation begin", __FUNCTION__);
            //执行SQL语句
            rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "gen_param_data_init_logi_dev general backup vacuum Fail ** err:%s", zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            else
            {
                //同步
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_readbackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            olog_debug(db_err, "%s, space reclamation end", __FUNCTION__);
        }
#else
        olog_debug(db_ocat, "space reclamation is closed");
#endif
    }
    else
    {
        olog_error(db_ocat, "delete table Fail dbGeneralCenter err: \n");
    }

    tcollect = get_Tgeneral_delay();
    if (tcollect)
    {
        sint8 *sql_del = "delete from dbGeneralDelayCenter where LDeviceNo=%u %s;";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, condition);

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

#ifdef SPACE_RECLAMATION_SWITCH
        if (needSpaceReclaim) {
            //回收空间
            olog_debug(db_err, "%s, space reclamation table[dbGeneralDelayCenter] begin", __FUNCTION__);
            //执行SQL语句
            rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "gen_param_data_init_logi_dev delay vacuum Fail ** err:%s", zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            else
            {
                //同步
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_readhander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            olog_debug(db_err, "%s, space reclamation table[dbGeneralDelayCenter] end", __FUNCTION__);
        }
#else
        olog_debug(db_ocat, "space reclamation is closed");
#endif
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

#ifdef SPACE_RECLAMATION_SWITCH
        if (needSpaceReclaim) {
            //回收空间
            olog_debug(db_err, "%s, space reclamation begin", __FUNCTION__);
            //执行SQL语句
            rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "gen_param_data_init_logi_dev delay backup vacuum Fail ** err:%s", zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            else
            {
                //同步
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_readbackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            olog_debug(db_err, "%s, space reclamation end", __FUNCTION__);
        }
#else
        olog_debug(db_ocat, "space reclamation is closed");
#endif
    }
    else
    {
        olog_error(db_ocat, "delete table Fail dbGeneralDelayCenter err: \n");
    }

    //恢复出厂
    if (flag == 1)
    {
        do {
            if (dataTypeIDOptioal && dataTypeID == ENUM_NOT_INIT_PARAM)
            {
                break;
            }

            tcollect = get_Tparameter();
            if (tcollect && flag == 1)
            {
                sint8 *sql_del = "delete from dbParameterCenter where LDeviceNo=%u %s;";
                if (dataTypeIDOptioal)
                {
                    snprintf(buf, sizeof(buf), sql_del, logi_dev, condition);
                }
                else
                {
                    char tmpCondtion[128] = {0};
                    snprintf(tmpCondtion, sizeof(tmpCondtion), "and DataTypeID !=%d", ENUM_NOT_INIT_PARAM);
                    snprintf(buf, sizeof(buf), sql_del, logi_dev, tmpCondtion);
                }

                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
                if (rc != SQLITE_OK)
                {
                    olog_error(db_ocat, "delete dbParameterCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		            sqlite3_free(zErrMsg);
                    return S_NG;
                }

#ifdef SPACE_RECLAMATION_SWITCH
                if (needSpaceReclaim) {
                    //回收空间
                    olog_debug(db_err, "%s, space reclamation table[dbParameterCenter] begin", __FUNCTION__);
                    //执行SQL语句
                    rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
                    if (rc != SQLITE_OK)
                    {
                        olog_error(db_ocat, "vacuum Fail ** err:%s\n", zErrMsg);
		                sqlite3_free(zErrMsg);
                    }
                    else
                    {
                        //同步
                        //执行SQL语句
                        rc = sqlite3_exec(tcollect->db_readhander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		                sqlite3_free(zErrMsg);
                    }
                    olog_debug(db_err, "%s, space reclamation table[dbParameterCenter] end", __FUNCTION__);
                }
#else
                olog_debug(db_ocat, "space reclamation is closed");
#endif
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
                if (rc != SQLITE_OK)
                {
                    olog_error(db_ocat, "delete dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		            sqlite3_free(zErrMsg);
                    return S_NG;
                }

#ifdef SPACE_RECLAMATION_SWITCH
                if (needSpaceReclaim) {
                    //回收空间
                    olog_debug(db_err, "%s, space reclamation begin", __FUNCTION__);
                    //执行SQL语句
                    rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
                    if (rc != SQLITE_OK)
                    {
                        olog_debug(db_ocat, "vacuum Fail ** err:%s\n", zErrMsg);
		                sqlite3_free(zErrMsg);
                    }
                    else
                    {
                        //同步
                        //执行SQL语句
                        rc = sqlite3_exec(tcollect->db_readbackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		                sqlite3_free(zErrMsg);
                    }
                    olog_debug(db_err, "%s, space reclamation end", __FUNCTION__);
                }
#else
                olog_debug(db_ocat, "space reclamation is closed");
#endif
            }
            else
            {
                olog_error(db_ocat, "delete table Fail dbParameterCenter err: \n");
            }
        } while(0);
    }

    //内存数据
    //数据/参数删除
    tcollect = get_Tmemory();
    if (tcollect)
    {
        sint8 *sql_del = "delete from Tmemory where LDeviceNo=%u %s;";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, condition);

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete Tmemory Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }
    }
    else
    {
        olog_error(db_ocat, "delete table Fail Tmemory err: \n");
    }

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    return S_OK;
}

/**
*********************************************************************
* @brief： 数据/参数初始化
* @param[in]：
               logi_dev       逻辑设备号
               info_number    信息点号
               flag           (0:初始化；１：恢复出厂)
               dataTypeIDOptioal 数据类型ID是否有效
               dataTypeID     数据类型ID
               needSpaceReclaim 是否需要空间回收
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 gen_param_data_init_logi_dev_info_number(uint8 logi_dev,
                                                uint16 info_number,
                                                sint8 flag,
                                                uint8 dataTypeIDOptioal,
                                                uint16 dataTypeID,
                                                bool needSpaceReclaim)
{
    //数据和参数
    sint8 buf[512]                    = {0};
    sint8 condition_data_type_ID[128] = {0};
    sint8 condition_info_number[128]  = {0};
    DB_RAW_T *tcollect                = NULL;
    sint8 *zErrMsg                    = NULL;
    int rc;

    if (dataTypeIDOptioal)
    {
        snprintf(condition_data_type_ID, sizeof(condition_data_type_ID), "and DataTypeID=%d", dataTypeID);
    }

    if (info_number != 0xffff)
    {
        snprintf(condition_info_number, sizeof(condition_info_number), "and InfoAddrNo=%u", info_number);
    }

    //数据/参数删除
    tcollect = get_Tgeneral();
    if (tcollect)
    {
        sint8 *sql_del = "delete from dbGeneralCenter where LDeviceNo=%u %s %s;";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, condition_data_type_ID, condition_info_number);
        olog_debug(db_ocat, "sql[%s]", buf);
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete dbGeneralCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

#ifdef SPACE_RECLAMATION_SWITCH
        if ((flag == 1) && needSpaceReclaim) {
            //回收空间
            dzlog_warn(db_err, "%s, space reclamation table[dbGeneralCenter] begin", __FUNCTION__);
            //执行SQL语句
            rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "gen_param_data_init_logi_dev general vacuum Fail ** err:%s", zErrMsg);
            }
            else
            {
                //同步
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_readhander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            dzlog_warn(db_err, "%s, space reclamation table[dbGeneralCenter] end", __FUNCTION__);
        }
#else
        olog_debug(db_ocat, "space reclamation is closed");
#endif

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

#ifdef SPACE_RECLAMATION_SWITCH
        if ((flag == 1) && needSpaceReclaim) {
            //回收空间
            dzlog_warn(db_err, "%s, space reclamation table[dbGeneralCenter] begin", __FUNCTION__);
            //执行SQL语句
            rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "gen_param_data_init_logi_dev general backup vacuum Fail ** err:%s", zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            else
            {
                //同步
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_readbackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            dzlog_warn(db_err, "%s, space reclamation table[dbGeneralCenter] end", __FUNCTION__);
        }
#else
        olog_debug(db_ocat, "space reclamation is closed");
#endif
    }
    else
    {
        olog_error(db_ocat, "delete table Fail dbGeneralCenter err: \n");
    }

    tcollect = get_Tgeneral_delay();
    if (tcollect)
    {
        sint8 *sql_del = "delete from dbGeneralDelayCenter where LDeviceNo=%u %s %s;";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, condition_data_type_ID, condition_info_number);
        olog_debug(db_ocat, "sql[%s]", buf);
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

#ifdef SPACE_RECLAMATION_SWITCH
        if ((flag == 1) && needSpaceReclaim) {
            //回收空间
            //执行SQL语句
            dzlog_warn(db_err, "%s, space reclamation table[dbGeneralDelayCenter] begin", __FUNCTION__);
            rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "gen_param_data_init_logi_dev delay vacuum Fail ** err:%s", zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            else
            {
                //同步
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_readhander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            dzlog_warn(db_err, "%s, space reclamation table[dbGeneralDelayCenter] wrend", __FUNCTION__);
        }
#else
        olog_debug(db_ocat, "space reclamation is closed");
#endif
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

#ifdef SPACE_RECLAMATION_SWITCH
        if ((flag == 1) && needSpaceReclaim) {
            //回收空间
            dzlog_warn(db_err, "%s, space reclamation table[dbGeneralDelayCenter] begin", __FUNCTION__);
            //执行SQL语句
            rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "gen_param_data_init_logi_dev delay backup vacuum Fail ** err:%s", zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            else
            {
                //同步
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_readbackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		        sqlite3_free(zErrMsg);
            }
            dzlog_warn(db_err, "%s, space reclamation table[dbGeneralDelayCenter] end", __FUNCTION__);
        }
#else
        olog_debug(db_ocat, "space reclamation is closed");
#endif
    }
    else
    {
        olog_error(db_ocat, "delete table Fail dbGeneralDelayCenter err: \n");
    }

    //恢复出厂
    if (flag == 1)
    {
        olog_debug(db_ocat, "operate the parameter DB");
        do {
            if (dataTypeIDOptioal && dataTypeID == ENUM_NOT_INIT_PARAM)
            {
                olog_warn(db_ocat, "can not delete the parameter which can not be initialized");
                break;
            }

            tcollect = get_Tparameter();
            if (tcollect && flag == 1)
            {
                sint8 *sql_del = "delete from dbParameterCenter where LDeviceNo=%u %s %s;";
                if (dataTypeIDOptioal)
                {
                    snprintf(buf, sizeof(buf), sql_del, logi_dev, condition_data_type_ID, condition_info_number);
                }
                else
                {
                    char tmpCondtion[128] = {0};
                    snprintf(tmpCondtion, sizeof(tmpCondtion), " and DataTypeID !=%d ", ENUM_NOT_INIT_PARAM);
                    snprintf(buf, sizeof(buf), sql_del, logi_dev, tmpCondtion, condition_info_number);
                }
                
                olog_debug(db_ocat, "sql[%s]", buf);
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
                if (rc != SQLITE_OK)
                {
                    olog_error(db_ocat, "delete dbParameterCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		            sqlite3_free(zErrMsg);
                    return S_NG;
                }

#ifdef SPACE_RECLAMATION_SWITCH
                if (needSpaceReclaim) {
                    //回收空间
                    dzlog_warn(db_err, "%s, space reclamation table[dbParameterCenter] begin", __FUNCTION__);
                    //执行SQL语句
                    rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
                    if (rc != SQLITE_OK)
                    {
                        olog_error(db_ocat, "vacuum Fail ** err:%s\n", zErrMsg);
		                sqlite3_free(zErrMsg);
                    }
                    else
                    {
                        //同步
                        rc = sqlite3_exec(tcollect->db_readhander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		                sqlite3_free(zErrMsg);
                    }
                    dzlog_warn(db_err, "%s, space reclamation table[dbParameterCenter] end", __FUNCTION__);
                }
#else
                olog_debug(db_ocat, "space reclamation is closed");
#endif
                //执行SQL语句
                rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
                if (rc != SQLITE_OK)
                {
                    olog_error(db_ocat, "delete dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		            sqlite3_free(zErrMsg);
                    return S_NG;
                }

#ifdef SPACE_RECLAMATION_SWITCH
                if (needSpaceReclaim) {
                    //回收空间
                    dzlog_warn(db_err, "%s, space reclamation table[dbParameterCenter] begin", __FUNCTION__);
                    //执行SQL语句
                    rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
                    if (rc != SQLITE_OK)
                    {
                        olog_debug(db_ocat, "vacuum Fail ** err:%s\n", zErrMsg);
		                sqlite3_free(zErrMsg);
                    }
                    else
                    {
                        //同步
                        //执行SQL语句
                        rc = sqlite3_exec(tcollect->db_readbackuphander_get(tcollect), "vacuum;", NULL, NULL, &zErrMsg);
		                sqlite3_free(zErrMsg);
                    }
                    dzlog_warn(db_err, "%s, space reclamation table[dbParameterCenter] end", __FUNCTION__);
                }
#else
                olog_debug(db_ocat, "space reclamation is closed");
#endif
            }
            else
            {
                olog_error(db_ocat, "delete table Fail dbParameterCenter err: \n");
            }
        } while(0);
    }

    //内存数据
    //数据/参数删除
    tcollect = get_Tmemory();
    if (tcollect)
    {
        sint8 *sql_del = "delete from Tmemory where LDeviceNo=%u %s %s;";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, condition_data_type_ID, condition_info_number);
        olog_debug(db_ocat, "sql[%s]", buf);
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete Tmemory Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }
    }
    else
    {
        olog_error(db_ocat, "delete table Fail Tmemory err: \n");
    }

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    return S_OK;
}

/**
*********************************************************************
* @brief： 指定数据/参数初始化
* @param[in]：
               logi_dev       逻辑设备号
               inf_addr_no    信息点号
               priDI          主DI
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 spec_gen_param_data_init(uint8 logi_dev, uint16 inf_addr_no, sint32 priDI)
{
    //数据和参数
    sint8 buf[512]                   = {0};
    DB_RAW_T *tcollect               = NULL;
    sint8 *zErrMsg                   = NULL;
    sint8 condition_info_number[128] = {0};
    sint8 condition_dataType_info[128] = {0};

    int rc = -1;

    if (inf_addr_no != 0xffff)
    {
        snprintf(condition_info_number, sizeof(condition_info_number), "and InfoAddrNo=%u", inf_addr_no);
    }

    //数据/参数删除
    tcollect = get_Tgeneral();
    if (tcollect)
    {
        sint8 *sql_del = "delete from dbGeneralCenter where LDeviceNo=%u and DataID=%u %s";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, priDI, condition_info_number);

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "spec_gen_param_data_init dbGeneralCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "spec_gen_param_data_init dbGeneralCenter backup Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }
    }
    else
    {
        olog_error(db_ocat, "spec_gen_param_data_init dbGeneralCenter err: \n");
    }

    tcollect = get_Tgeneral_delay();
    if (tcollect)
    {
        sint8 *sql_del = "delete from dbGeneralDelayCenter where LDeviceNo=%u and DataID=%u %s";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, priDI, condition_info_number);

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "spec_gen_param_data_init dbGeneralDelayCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "spec_gen_param_data_init dbGeneralDelayCenter backup Fail sql:%s ** err:%s \n", buf,
                       zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }
    }
    else
    {
        olog_error(db_ocat, "spec_gen_param_data_init dbGeneralDelayCenter err: \n");
    }

    tcollect = get_Tparameter();
    if (tcollect)
    {
//        snprintf(condition_dataType_info, sizeof(condition_dataType_info), "and  DataTypeID !=%d", ENUM_NOT_INIT_PARAM);
        sint8 *sql_del = "delete from dbParameterCenter where LDeviceNo=%u and DataID=%u %s %s";

        snprintf(buf, sizeof(buf), sql_del, logi_dev, priDI, condition_info_number, condition_dataType_info);

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "spec_gen_param_data_init dbParameterCenter Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }
        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writebackuphander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "spec_gen_param_data_init dbParameterCenter backup Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }
    }
    else
    {
        olog_error(db_ocat, "spec_gen_param_data_init dbParameterCenter err: \n");
    }
    //内存表
    tcollect = get_Tmemory();
    if (tcollect)
    {
        sint8 *sql_del = "delete from Tmemory where LDeviceNo=%u and DataID=%u %s";
        snprintf(buf, sizeof(buf), sql_del, logi_dev, priDI, condition_info_number);

        //执行SQL语句
        rc = sqlite3_exec(tcollect->db_writehander_get(tcollect), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "spec_gen_param_data_init Tmemory Fail sql:%s ** err:%s \n", buf, zErrMsg);
		    sqlite3_free(zErrMsg);
            return S_NG;
        }
    }
    else
    {
        olog_error(db_ocat, "spec_gen_param_data_init Tmemory err: \n");
    }

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    return rc;
}
/**
*********************************************************************
* @brief： 数据初始化
* @param[in]：
               req       请求消息
               isFactory 恢复出厂消息
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 data_init_logi_dev(uint8 logi_dev, uint8 optinal, uint16 dataTypeID, uint8 isFactory)
{
    sint32 rc;
    qhashtbl_obj_t obj;
    qhashtbl_t *tbl = g_db_c_tbl;
    // sint32 idx = 0;
    sint32 ret = S_OK;
    bool needSpaceReclaim = false;
    sint8 endc[2]        = {0};
    DB_TAB_RECORD_INFO_T* data;
    uint8 dev_no;

    if(optinal == 0)
    {
        needSpaceReclaim = true;
    }

    memset((void *)&obj, 0, sizeof(obj));
//    tbl->lock(tbl);
    while (tbl->getnext(tbl, &obj, false) == true)
    {
        data = (DB_TAB_RECORD_INFO_T *)(obj.data);
        
        if (!data->tab_name)
        {
            olog_warn(db_ocat, "data_init_logi_dev 's tab_name is NULL");
    		continue;
        }       
        
        dev_no = get_logi_dev_no_by_name(data->tab_name, endc);
        if (dev_no != logi_dev || endc[0] != 0x00)
        {
            olog_debug(db_ocat, "endc:%d; logicalDeviceNum=%d tab_name:%s\n", endc[0], logi_dev, data->tab_name);
            continue;
        }

        if (data->tab_status == ENUM_TABLE_CLOSE) 
        {
            sint8 db_name[1024] = {0};
            sprintf(db_name, "%s%s.db", g_configObj.dbCfg.basePath, data->tab_name);
            rc = db_open(db_name, NULL, DB_TYPE_RECORD, &data->handle);
            if (rc != SQLITE_OK)
            {
                olog_error(db_ocat, "Open DB File (%s) Fail ret%d.", db_name, rc);
                return S_NG;
            }
        }

		if (!data->handle)
        {
            olog_warn(db_ocat, "data_init_logi_dev 's tcollect is NULL");
    		continue;
        }  
    
        rc = dele_data_logi_dev(data, logi_dev, isFactory, optinal, dataTypeID, needSpaceReclaim); 
        if (rc != S_OK)
        {
            ret = rc;
        }
        
        tbl->lock(tbl);
        db_close(data->handle);
        data->tab_status = ENUM_TABLE_CLOSE;
        data->refer_count = 0;
        tbl->unlock(tbl);
    }
//    ret |=  close_db_handle(logi_dev);
//    tbl->unlock(tbl);

    return ret;
}

/**
*********************************************************************
* @brief： 数据初始化带参数
* @param[in]：
               req       请求消息
               needSpaceReclaim 是否需要空间回收
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 data_init_logi_dev_info_number(uint8 logi_dev, uint16 inf_no, uint16 dataTypeID, bool needSpaceReclaim)
{
    sint32 rc;
    qhashtbl_obj_t obj;
    qhashtbl_t *tbl = g_db_c_tbl;
    // sint32 idx = 0;
    sint32 ret = S_OK;
    sint32 needClose    = 0;
    uint8 dev_no        = 0;
    sint8 endc[2]       = {0};

    memset((void *)&obj, 0, sizeof(obj));
#ifdef AREA_FUJIAN
	if(dataTypeID >= ENUM_INIT_PARAM && dataTypeID <= ENUM_NOT_INIT_DATA)
#else
	if(dataTypeID >= ENUM_NORMAL_DATA && dataTypeID <= ENUM_NOT_INIT_DATA)
#endif
	{
		while (tbl->getnext(tbl, &obj, 0) == true)
		{
			DB_TAB_RECORD_INFO_T* data = (DB_TAB_RECORD_INFO_T*)(obj.data);
			dev_no = get_logi_dev_no_by_name(data->tab_name, endc);
			if (dev_no != logi_dev || endc[0] != 0x00)
			{
				olog_debug(db_ocat, "endc:%d; dev_no=%d tab_name:%s\n", endc[0], dev_no, data->tab_name);
				continue;
			}
			
			tbl->lock(tbl);
			if (data->tab_status == ENUM_TABLE_CLOSE) 
			{
				sint8 dbfile[1024] = {0};
				snprintf(dbfile, sizeof(dbfile), "%s%s.db", g_configObj.dbCfg.basePath, data->tab_name);
				rc = db_open(dbfile, NULL, DB_TYPE_RECORD, &(data->handle));
				if (rc != SQLITE_OK)
				{
					olog_error(db_ocat, "Open DB File (%s) Fail.", dbfile);
					continue;
				}
				needClose = 1;
			}

			data->refer_count = data->refer_count + 1;

			rc = dele_data_logi_dev_info_number(data, logi_dev, inf_no, dataTypeID, needSpaceReclaim);
			data->refer_count = data->refer_count - 1;
			clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));

			if (rc != S_OK)
			{
				ret = rc;
			}
			if (needClose == 1) 
			{
				db_close(data->handle);
				needClose = 0;
			}

			tbl->unlock(tbl);
		}
	}
    

    //数据删除/参数保留
    rc = gen_param_data_init_logi_dev_info_number(logi_dev, inf_no, 1, 1, dataTypeID, needSpaceReclaim);
    if (rc != S_OK)
    {
        olog_error(db_ocat, "gen_param_data_init_logi_dev err: \n");
        ret = S_NG;
    }
    //
    return ret;
}

/**
*********************************************************************
* @brief： 通过主DI获取所有对应的数据对象
* @param[in]： pridatadi   主DI
            
* @param[out]：qlist_t     数据对象
        
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 get_all_table_by_pridatadi(uint32 pridatadi, qlist_t *list)
{
    qhashtbl_obj_t obj;
    qhashtbl_t *tbl = g_db_c_tbl;
    sint8 buf[64]   = {0};
    sint32 ret      = S_OK;

    snprintf(buf, sizeof(buf), "%X", pridatadi);
    tbl->lock(tbl);
    memset((void *)&obj, 0, sizeof(obj));
    while (tbl->getnext(tbl, &obj, 0) == true)
    {
        olog_debug(db_ocat, "obj.name = %s buf=%s", obj.name, buf);
        if (0 == strcmp(obj.name + 4, buf))
        {
            list->addlast(list, obj.data, obj.size);
        }
    }
    tbl->unlock(tbl);
    return ret;
}

/**
*********************************************************************
* @brief： 指定数据初始化
* @param[in]：
               logi_dev       逻辑设备号
               inf_no         信息地址号
               prim_di			主DI
               sub_di			次DI
               needSpaceReclaim  是否需要空间回收
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 spec_data_del(uint8 logi_dev, uint16 inf_no, uint16 data_type, uint32 prim_di, uint32 sub_di, bool needSpaceReclaim)
{
    sint32 rc                     = -1;
    DB_TAB_RECORD_INFO_T tab_info = {0};
    sint8 buf[512]                = {0};
    sint8 *zErrMsg                = NULL;
    DB_RAW_T *tcollect            = NULL;
    uint16 tmp_info_num[2]        = {0};
    uint8 tmp_info_num_count      = 0;
    uint8 i                       = 0;

    sint8 talbe_name[64]          = {0};
	sint8 strInfo[64]          	  = {0};
	sint8 strsub[64]          	  = {0};

    olog_debug(db_ocat, "logi_dev=%u inf_addr_no=%u prim_di=%u sub_di=%u", logi_dev, inf_no, prim_di, sub_di);
    if (inf_no != 0xffff)
    {
    	snprintf(strInfo, sizeof(strInfo), "and InfoAddrNo=%u", inf_no);
        tmp_info_num[0]    = inf_no;
        tmp_info_num_count = 1;
    }
    else
    {
        tmp_info_num[0]    = 0;
        tmp_info_num[1]    = 1;
        tmp_info_num_count = 2;
    }

	if (sub_di != 0xffffffff)
    {
    	snprintf(strsub, sizeof(strsub), "and SubDataDI=%u", sub_di);
    }

    for (i = 0; i < tmp_info_num_count; i++)
    {
        rc = get_collect_tab_info(logi_dev, tmp_info_num[i], prim_di, sub_di, &tab_info, true);
        if (tab_info.handle == NULL || tab_info.tab_name == NULL)
        {
            olog_warn(db_ocat, "db_spec_data_del tab_info.handle or tab_name is NULL\n");
        }
        else
        {

#ifdef AREA_FUJIAN
			if(data_type >= ENUM_INIT_PARAM && data_type <= ENUM_NOT_INIT_DATA)
#else
			if(data_type >= ENUM_NORMAL_DATA && data_type <= ENUM_NOT_INIT_DATA)
#endif
            {
                tcollect = tab_info.handle;
                snprintf(talbe_name, sizeof(talbe_name), "%s%d", tab_info.tab_name, data_type);

				//数据库句柄之前是关闭的，检查数据库表是否存在
				if (tab_info.tab_status == ENUM_TABLE_CLOSE )
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

                snprintf(buf, sizeof(buf), "delete from %s where LDeviceNo=%u %s and PriDataDI=%u %s;", talbe_name, logi_dev, strInfo, prim_di, strsub);

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
            put_collect_tab_info(logi_dev, tmp_info_num[i], prim_di, sub_di);
        }
    }

    if (prim_di)
        rc = spec_gen_param_data_init(logi_dev, inf_no, prim_di);
    else
        rc = spec_gen_param_data_init(logi_dev, inf_no, sub_di);

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    return rc;
}

/**
*********************************************************************
* @brief： 指定数据初始化 CHOICE = 0
* @param[in]：
               logi_dev       逻辑设备号
               inf_addr_no    信息地址号
               data_type_id   数据类型ID
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 spec_data_del_C0(uint8 logi_dev, uint16 inf_addr_no, sint16 data_type_id)
{
    return 0;
}

/**
*********************************************************************
* @brief： 记录型数据初始化
* @param[in]：
               logi_dev       逻辑设备号
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 record_data_init(uint8 logi_dev)
{
    DIR *dp;
    sint8 rmPath[PATH_LEN*2] = {0};
    struct dirent *entry;
    struct stat statbuf;
    sint8 *destDir = g_configObj.dbCfg.basePath;
    sint32 ret = 0;

    if ((dp = opendir(destDir)) == NULL)
    {
        olog_error(db_ocat, "cannot open directory: %s\n", destDir);
        return S_NG;
    }

    g_db_c_tbl->lock(g_db_c_tbl);
    while ((entry = readdir(dp)) != NULL)
    {
        snprintf(rmPath, sizeof(rmPath), "%s%s", g_configObj.dbCfg.basePath, entry->d_name);
        lstat(rmPath, &statbuf);
        if (S_ISREG(statbuf.st_mode))
        {
            uint8 dev_no  = 0;
            sint8 endc[2] = {0};
            if (entry->d_name[1] == 'p' || entry->d_name[1] == 'g')
            {
                continue;
            }
            dev_no = get_logi_dev_no_by_name(entry->d_name, endc);
//            olog_warn(db_ocat, "endc:%d  dev_no=%d tab_name:%s\n", endc[0], dev_no, entry->d_name);
            if (dev_no != logi_dev || endc[0] != 0x00)
            {
                olog_debug(db_ocat, "record_data_init:[%s] dev_no:[%u] entry->d_name:%s", rmPath, dev_no, entry->d_name);
                continue;
            }

            remove(rmPath);
            olog_debug(db_ocat, "remove:[%s] dev_no:[%u]", rmPath, dev_no);
            perror("remove");
        }
    }
    closedir(dp);
    ret =  close_db_handle(logi_dev);
    
    g_db_c_tbl->unlock(g_db_c_tbl);
    return ret;
}


/**
*********************************************************************
* @brief： 工厂数据初始化
* @param[in]：
               logi_dev       逻辑设备号
* @param[out]：
               sint32
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 recovery_factory(uint8 logi_dev)
{
    DIR *dp;
    sint8 rmPath[PATH_LEN*2] = {0};
    struct dirent *entry;
    struct stat statbuf;
    sint8 *destDir = g_configObj.dbCfg.basePath;
    int rc;

    if ((dp = opendir(destDir)) == NULL)
    {
        olog_error(db_ocat, "cannot open directory: %s\n", destDir);
        return S_NG;
    }

    g_db_c_tbl->lock(g_db_c_tbl);
    while ((entry = readdir(dp)) != NULL)
    {
        snprintf(rmPath, sizeof(rmPath), "%s%s", g_configObj.dbCfg.basePath, entry->d_name);
        lstat(rmPath, &statbuf);
        if (S_ISREG(statbuf.st_mode))
        {
            uint8 dev_no  = 0;
            sint8 endc[2] = {0};
            if (entry->d_name[1] == 'p' || entry->d_name[1] == 'g')
            {
                continue;
            }
            dev_no = get_logi_dev_no_by_name(entry->d_name, endc);
//            olog_warn(db_ocat, "endc:%d  dev_no=%d tab_name:%s\n", endc[0], dev_no, entry->d_name);
            if (dev_no != logi_dev || endc[0] != 0x00)
            {
                olog_debug(db_ocat, "factory:[%s] dev_no:[%u] entry->d_name:%s", rmPath, dev_no, entry->d_name);
                continue;
            }

            remove(rmPath);
            olog_debug(db_ocat, "remove:[%s] dev_no:[%u]", rmPath, dev_no);
            perror("remove");
        }
    }
    closedir(dp);
    close_db_handle(logi_dev);
    g_db_c_tbl->unlock(g_db_c_tbl);

    //数据/参数删除
    rc = gen_param_data_init_logi_dev(logi_dev, 1, 0, 0, false);
    if (rc != S_OK)
    {
        olog_error(db_ocat, "recovery_factory gen_param_data_init_logi_dev err: \n");
    }

    return S_OK;
}

/**
*********************************************************************
* @brief： 通过配置文件，载入数据深度配置信息
* @param[in]：
               tbl         数据表
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 load_tab_depth_cfg(qlisttbl_t *tbl)
{
    qlisttbl_obj_t obj = {0};
    // size_t isize = 0;
    DEPTH_CFG_T depth_t;
    memset(&depth_t, 0, sizeof(DEPTH_CFG_T));

    if (!tbl)
    {
        olog_error(db_ocat, "load_tab_depth_cfg's tbl 为空 \n");
        return S_NG;
    }

    //初始化数据深度链表
    g_tab_depth_cfgl = qhashtbl(0, QHASHTBL_THREADSAFE);
    memset((void *)&obj, 0, sizeof(obj));

    while (tbl->getnext(tbl, &obj, NULL, 0) == true)
    {
        if (strncmp(obj.name, c_depth_til_name, strlen(c_depth_til_name))
            && strncmp(obj.name, c_fault_til_name, strlen(c_fault_til_name)))
        {
            continue;
        }

        memset(depth_t.key, 0x00, sizeof(depth_t));
        strncpy(depth_t.key, obj.name, sizeof(depth_t.key));
        strncpy(depth_t.depth, obj.data, sizeof(depth_t.depth));
        g_tab_depth_cfgl->put(g_tab_depth_cfgl, depth_t.key, (const void *)&depth_t, sizeof(depth_t));
        // free(obj.name);
        // free(obj.data);
    }
    // g_tab_depth_cfgl->debug(g_tab_depth_cfgl,stdout);

    return 0;
}

/**
*********************************************************************
* @brief： 通过配置文件，载入数据深度配置信息
* @param[in]：
               tbl         数据表
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 load_tab_depth_Info(qlisttbl_t *tbl)
{
    // olog_debug(db_ocat, "%s", __FUNCTION__);
    qlisttbl_obj_t obj   = {0};
    DEPTH_INFO_T depth_t;
    sint32 index         = 0;
    memset(&depth_t, 0, sizeof(DEPTH_INFO_T));
    if (!tbl)
    {
        olog_error(db_ocat, "load_tab_depth_cfg's tbl is NULL");
        return S_NG;
    }

    //初始化数据深度链表
    g_tab_info_cfg = qhashtbl(0, QHASHTBL_THREADSAFE);
    memset((void *)&obj, 0, sizeof(obj));

    while (tbl->getnext(tbl, &obj, NULL, 0) == true)
    {
        if (strncmp(obj.name, c_depth_til_name, strlen(c_depth_til_name))
            && strncmp(obj.name, c_fault_til_name, strlen(c_fault_til_name)))
        {
            continue;
        }

        if (atoi((char *)obj.data) != 0)
        {
            // olog_debug(db_ocat, "NAME=%s, DATA=%s, SIZE=%zu", obj.name, (char*)obj.data, obj.size);
            if (strncmp(obj.name, c_fault_til_name, strlen(c_fault_til_name)) == 0)
            {
                strcpy(depth_t.Key, c_fault_til_name);
                depth_t.defaultDepth = atoi((char *)obj.data);
                g_tab_info_cfg->put(g_tab_info_cfg, depth_t.Key, &depth_t, sizeof(DEPTH_INFO_T));
            }
            else
            {
                char str1[3]  = {0};
                char str2[6]  = {0};
                char str3[64] = {0};
                char str4[64] = {0};

                sscanf(obj.name, "%[^_]_%[^_]_%[^.].%s", str1, str2, str3, str4);
                // olog_debug(db_ocat, "str3[%s], str4[%s]", str3, str4);
                if (strcmp(str4, "subCount") == 0)
                {
                    memset(&depth_t, 0, sizeof(depth_t));
                    sscanf(str3, "%x", &(depth_t.majorDI));
                    sprintf(depth_t.Key, "%s%x", c_depth_til_name, depth_t.majorDI);
                    olog_debug(db_ocat, "major DI[%d]", depth_t.majorDI);
                    depth_t.subDICount = atoi((char *)obj.data);
                    if (depth_t.subDICount == -1)
                    {
                        depth_t.subDICount = 0;
                    }

                    if (depth_t.subDICount > DB_SUB_DI_MAX_COUNT)
                    {
                        olog_warn(db_ocat, "The sub DI Count[%d] exceed the max count", depth_t.subDICount);
                        depth_t.subDICount = DB_SUB_DI_MAX_COUNT;
                    }
                    index = 0;
                    continue;
                }

                if (strcmp(str4, "default") == 0)
                {
                    depth_t.defaultDepth = atoi((char *)obj.data);
                    g_tab_info_cfg->put(g_tab_info_cfg, depth_t.Key, &depth_t, sizeof(DEPTH_INFO_T));
                    // debug
                    olog_debug(db_ocat, "================table depth[%s]===========", depth_t.Key);
                    olog_debug(db_ocat, "majorDI[%x], defaultDepth[%d], subDICount[%d]", depth_t.majorDI,
                               depth_t.defaultDepth, depth_t.subDICount);
                    for (index = 0; index < depth_t.subDICount; index++)
                    {
                        olog_debug(db_ocat, "index[%d], subDI[%x], depth[%d]", index, depth_t.subDIInfo[index].subDI,
                                   depth_t.subDIInfo[index].depth);
                    }
                    olog_debug(db_ocat, "==========================================");
                    // end
                    index = 0;
                    continue;
                }

                // save the subDI
                if (index < DB_SUB_DI_MAX_COUNT) {
                    sscanf(str4, "%x", &(depth_t.subDIInfo[index].subDI));
                    depth_t.subDIInfo[index].depth = atoi((char *)obj.data);
                    index                          = index + 1;
                }
            }
        }
#if 0
        memset(depth_t.key, 0x00, sizeof(depth_t));
        strncpy(depth_t.key, obj.name, sizeof(depth_t.key));
        strncpy(depth_t.depth, obj.data, sizeof(depth_t.depth));
        g_tab_info_cfg->put(g_tab_info_cfg, depth_t.key, (const void *)&depth_t, sizeof(depth_t));
#endif
        // free(obj.name);
        // free(obj.data);
    }
    // g_tab_info_cfg->debug(g_tab_info_cfg,stdout);

    return 0;
}

/**
*********************************************************************
* @brief： 根据主DI、次DI，获取数据深度信息
* @param[in]：
               pri_di         主DI
               sub_di         次DI
* @param[out]：
               深度信息
* @return： 返回值说明
            深度信息
*********************************************************************
*/
uint32 get_depth_value(sint32 pri_di, sint32 sub_di)
{
    uint32 depth        = 0;
    sint8 buf[128]      = {0};
    DEPTH_INFO_T *pinfo = NULL;
    size_t size         = 0;

    // todo the format of the key
    snprintf(buf, sizeof(buf), "%s%x", c_depth_til_name, pri_di);
    olog_debug(db_ocat, "%s, buf[%s]", __FUNCTION__, buf);
    pinfo = g_tab_info_cfg->get(g_tab_info_cfg, buf, &size, false);

    if (NULL != pinfo)
    {
        olog_debug(db_ocat, "has find the record");
        sint32 index = 0;
        for (index = 0; index < pinfo->subDICount; index++)
        {
            if (sub_di == pinfo->subDIInfo[index].subDI)
            {
                depth = pinfo->subDIInfo[index].depth;
                break;
            }
        }

        if (index == pinfo->subDICount)
        {
            olog_debug(db_ocat, "not find the subDI, so use the default depth");
            depth = pinfo->defaultDepth;
        }
    }
    else
    {
        olog_debug(db_ocat, "not find the record so use the default depth");
        depth = g_configObj.dbCfg.deleteDepthValue;
    }

#if 0
    snprintf(buf, sizeof(buf), "%s%X.%X", c_depth_til_name, pri_di, sub_di);
    olog_debug(db_ocat, "%s, buf[%s]", __FUNCTION__, buf);
    pinfo = g_tab_depth_cfgl->get(g_tab_depth_cfgl, buf, &isize, 0);
    //g_tab_depth_cfgl->debug(g_tab_depth_cfgl,stdout);
    if (pinfo)
    {
        ret = strtoul(pinfo->depth, NULL,0);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%s%X.default", c_depth_til_name, pri_di);
        pinfo = g_tab_depth_cfgl->get(g_tab_depth_cfgl, buf, &isize, 0);
        if (pinfo)
        {
            ret = strtoul(pinfo->depth, NULL,0);
        }
        else
        {
            sprintf(buf, "%s.default", c_fault_til_name);
            //printf("c_fault_til_name:  %s\n", buf);
            pinfo = g_tab_depth_cfgl->get(g_tab_depth_cfgl, buf, &isize, 0);
            if (pinfo)
            {
                ret = strtoul(pinfo->depth, NULL,0);
            }
        }
    }
#endif

    return depth;
}

/**
*********************************************************************
* @brief： 根据主DI、获取数据深度信息
* @param[in]：
               pri_di         主DI
* @param[out]：
               深度信息
* @return： 返回值说明
            深度信息
*********************************************************************
*/
uint32 get_depth_value_pridi(uint32 pri_di)
{
    sint32 depth        = 0;
    sint8 buf[128]      = {0};
    DEPTH_INFO_T *pinfo = NULL;
    size_t isize        = 0;

    snprintf(buf, sizeof(buf), "%s%x.default", c_depth_til_name, pri_di);

    olog_debug(db_ocat, "search key[%s] from g_tab_info_cfg", buf);

    pinfo = g_tab_info_cfg->get(g_tab_info_cfg, buf, &isize, false);
    if (pinfo)
    {
        depth = pinfo->defaultDepth;
        olog_debug(db_ocat, "get %s default depth[%d] success", buf, depth);
    }
    else
    {
        olog_warn(db_ocat, "not find the key[%s], so just use the default depth", buf);
        depth = g_configObj.dbCfg.deleteDepthValue;
    }

    return depth;
}

/**
*********************************************************************
* @brief： 根据主DI、获取数据深度信息
* @param[in]：
               pri_di         主DI
* @param[out]：
               depthInfo      深度信息
* @return： 返回值说明
            S_NG    失败
            S_OK    成功
*********************************************************************
*/
uint32 get_depth_Info(uint32 *pri_di, DEPTH_INFO_T *depthInfo)
{
    olog_debug(db_ocat, "%s", __FUNCTION__);
    if (NULL == depthInfo)
    {
        olog_error(db_ocat, "Invalid param");
        return S_NG;
    }
    DEPTH_INFO_T *pDepthInfo = NULL;
    if (NULL == pri_di)
    {
        olog_debug(db_ocat, "get default table depth");
        size_t size;
        pDepthInfo = g_tab_info_cfg->get(g_tab_info_cfg, c_fault_til_name, &size, true);
        if (NULL == pDepthInfo)
        {
            olog_error(db_ocat, "get default table depth failed");
            return S_NG;
        }
        memcpy(depthInfo, pDepthInfo, sizeof(DEPTH_INFO_T));
        free(pDepthInfo);
        return S_OK;
    }

    size_t size;
    char key[64] = {0};
    sprintf(key, "%s%x", c_depth_til_name, *pri_di);
    olog_debug(db_ocat, "key[%s]", key);
    pDepthInfo = g_tab_info_cfg->get(g_tab_info_cfg, key, &size, true);
    if (NULL == pDepthInfo)
    {
        olog_debug(db_ocat, "get the priDI[%d] depth failed", *pri_di);
        return S_NG;
    }
    olog_debug(db_ocat, "majorDI[%x], default depth[%d]", pDepthInfo->majorDI, pDepthInfo->defaultDepth);
    memcpy(depthInfo, pDepthInfo, sizeof(DEPTH_INFO_T));
    free(pDepthInfo);
    return S_OK;
}

/**
*********************************************************************
* @brief： 删除超过深度的记录
* @param[in]：
               obj                    table信息
               cur_count              现有数据条数
               def_count              配置数据条数
               condition              查询条件
* @param[out]：
* @return： 返回值说明
            0 成功
            -1 失败
*********************************************************************
*/
sint32 del_over_records_by_no(DB_TAB_RECORD_INFO_T *pinfo, sint8 *table_name, sint32 defaultDepth, sint32 cur_count, sint8 *condition)
{
    sint8 buf[2048]             = {0};
    sint32 delete_no            = 0;
    sint32 rc                   = 0;
    sint8 *zErrMsg              = NULL;
    sint32 del_count            = 0;
    sint32 data_count           = 0;

    if(pinfo == NULL)
    {
        return -1;
    }

    data_count = cur_count;
    while(data_count - defaultDepth > 0)
    {
        if(data_count - defaultDepth > DEL_COUNT)
        {
            del_count = DEL_COUNT;
        }
        else
        {
            del_count = data_count - defaultDepth;
        }

        if(condition == NULL)
        {
            snprintf(buf, sizeof(buf), "ORDER BY No ASC LIMIT %d, 1;", del_count);
        }
        else
        {
            snprintf(buf, sizeof(buf), "%s ORDER BY No ASC LIMIT %d, 1;", condition, del_count);
        }
		
		delete_no = m_get_recoder_no(pinfo->handle->db_readhander_get(pinfo->handle), table_name, buf);

	    if(delete_no < 0)
	    {
	        olog_warn(db_ocat, "m_get_recoder_no failed tab_name[%s], condition[%s]!", table_name, buf);
	        return delete_no;
	    }

        if(condition == NULL)
        {
            snprintf(buf, sizeof(buf), "delete from %s where No < %d;", table_name, delete_no);
        }
        else
        {
            snprintf(buf, sizeof(buf), "delete from %s %s and No < %d ", table_name, condition, delete_no);
        }
        olog_debug(db_ocat, "sql cmd[%s]", buf);

        //执行SQL语句
        rc = sqlite3_exec(pinfo->handle->db_writehander_get(pinfo->handle), buf, NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "delete over table Fail sql:%s ** err:%s", buf, zErrMsg);
            sqlite3_free(zErrMsg);
            return rc;
        }

//        sqlite3_exec(pinfo->handle->db_writehander_get(pinfo->handle), "PRAGMA wal_checkpoint;", NULL, NULL, NULL);

//        sqlite3_exec(pinfo->handle->db_writehander_get(pinfo->handle), "COMMIT;", NULL, NULL, NULL);

        data_count -= del_count;
    }

        
    return 0;
}
/**
*********************************************************************
* @brief：获取当前表中数据数量
* @param[in]：
               pinfo 表信息
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 get_cur_tab_count(DB_TAB_RECORD_INFO_T *pinfo, sint8 *table_name)
{
    sint8 sqlstr[256] = {0};
    sqlite3_stmt *pstmt;
    sint32 count = 0;
    sint32 ret;
    const sint8 *pztail;
    sqlite3 *db;
//    struct timespec currenttime;

    db = pinfo->handle->db_readhander_get(pinfo->handle);
    if (!db)
    {
        olog_error(db_ocat, "db 为空 \n");
        return -1;
    }

    snprintf(sqlstr, sizeof(sqlstr), "select COUNT(*) from '%s';", table_name);
    // olog_debug(db_ocat,"sql:[%s]", sqlstr);

    ret = sqlite3_prepare_v2(db, sqlstr, -1, &pstmt, &pztail);
    if (ret != SQLITE_OK)
    {
        olog_warn(db_ocat, "get_cur_tab_count [%s] err: %s\n", sqlstr, sqlite3_errmsg(db));
        return -1;
    }

    ret = sqlite3_step(pstmt);
    if (ret == SQLITE_ROW)
    {
        //取得
        olog_debug(db_ocat, "ret:[%d]", ret);
        count = sqlite3_column_int(pstmt, 0);
    }
    sqlite3_finalize(pstmt);

    olog_debug(db_ocat, "tab[%s] get_cur_tab_count: %d", table_name, count);

    return count;
}

/**
*********************************************************************
* @brief： 获取表中指定次DI记录数量
* @param[in]：
               pinfo         表信息
               subDI         次DI
* @param[out]：
               none
* @return： 返回值说明
*********************************************************************
*/
sint32 get_cur_tab_subDI_record_count(DB_TAB_RECORD_INFO_T *pinfo, sint32 subDI, sint8 *table_name)
{
    sint8 sqlstr[256] = {0};
    sqlite3_stmt *pstmt;
    sint32 count = 0;
    sint32 ret;
    const sint8 *pztail;
    sqlite3 *db;
//    struct timespec currenttime;

    db = pinfo->handle->db_readhander_get(pinfo->handle);
    if (!db)
    {
        olog_error(db_ocat, "db 为空 \n");
        return S_NG;
    }

    snprintf(sqlstr, sizeof(sqlstr), "select COUNT(*) from '%s' where SubDataDI == %d;", table_name, subDI);
    olog_debug(db_ocat, "====sql:[%s]", sqlstr);

    ret = sqlite3_prepare_v2(db, sqlstr, -1, &pstmt, &pztail);
    if (ret != SQLITE_OK)
    {
        olog_error(db_ocat, "sqlite3_prepare_v2: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    ret = sqlite3_step(pstmt);
    if (ret == SQLITE_ROW)
    {
        //取得
        olog_debug(db_ocat, "ret:[%d]", ret);
        count = sqlite3_column_int(pstmt, 0);
    }
    sqlite3_finalize(pstmt);

    olog_debug(db_ocat, "%s: %d\n", __FUNCTION__, count);

    return count;
}

/**
*********************************************************************
* @brief： 删除超过深度的记录
* @param[in]：
               obj                    table信息
               depthInfoValid         深度信息是否有效
               depthInfo              深度信息
* @param[out]：
               深度信息
* @return： 返回值说明
            0 成功
            1 失败
*********************************************************************
*/
sint32 del_over_records(DB_TAB_RECORD_INFO_T *pinfo, DEPTH_INFO_T *depthInfo, bool depthInfoValid)
{
    // DB_TAB_RECORD_INFO_T info;
    sint32 rc;
    // sint32 max_dele_sum = DB_MAX_DELE_SUM;
    sint8 buf[2048] = {0};
    sint8 *zErrMsg = NULL;
    // DB_RAW_T *tcollect = NULL;
    sint32 cur_count        = 0;
    sint32 sub_record_count = 0;
    sint32 deleteCount      = 0;
    sint8  table_name[64]   = {0};
#if 1
    sint8 data_type                  = 0;
#else
    sint8 data_type                  = ENUM_NORMAL_DATA;
#endif

    bool hasDelete          = false;

    if (!pinfo)
    {
        olog_error(db_ocat, "input param is NULL");
        return S_NG;
    }

     while(data_type <= ENUM_NOT_INIT_DATA)
    {
        if (data_type == 0)
        {
            snprintf(table_name, sizeof(table_name), "%s", pinfo->tab_name);
            data_type = ENUM_NORMAL_DATA;
        }
        else
        {
            snprintf(table_name, sizeof(table_name), "%s%d", pinfo->tab_name, data_type);
            data_type++;
        }

		//数据库句柄之前是关闭的，检查数据库表是否存在
		if (pinfo->tab_status == ENUM_TABLE_CLOSE || data_type == ENUM_NORMAL_DATA)	//data_type = ENUM_NORMAL_DATA, 检查旧数据库是否有数据
        {
        	uint8 tabEllaxist		= 0;
            //判断需要删除的库表是否存在
			rc = check_table_isexist(pinfo, table_name, &tabEllaxist);
			if (rc == S_NG)
			{
		        olog_error(db_ocat, "del_over_records 's check table error");
		        return S_NG;
		    }

			//库表不存在，直接返回，不需要删除
			if (tabEllaxist == 0)
			{
		        olog_debug(db_ocat, "table is not exist, data is null");
		        continue;
		    }
        }	
		
        //获取总条数
        cur_count = get_cur_tab_count(pinfo, table_name);

        if (cur_count <= 0)
        {
            continue;
        }
        
        //配置了存储深度，按照配置，没有配置的按照默认深度
        if (depthInfoValid == true)
        {
            olog_debug(db_ocat, "table[%s] default depth[%d] cur_count[%d] should delete some record!!!", table_name, depthInfo->defaultDepth, cur_count);

            // get subDI record count
            sint32 subDICount = depthInfo->subDICount;
            sint32 index      = 0;

            // 检查有无配置次DI存储深度
            if (subDICount == 0)
            {
                if (cur_count > depthInfo->defaultDepth)
                {
                    olog_warn(db_ocat, "====delete some record in table[%s] cur_count[%d],defaultDepth[%u]!!!", table_name, cur_count, depthInfo->defaultDepth);
//                    sprintf(buf, "ORDER BY No ASC LIMIT %d, 1;", cur_count - depthInfo->defaultDepth);
                    del_over_records_by_no(pinfo, table_name, depthInfo->defaultDepth, cur_count, NULL);
                    hasDelete = true;
                }
            }
            else
            {
                olog_debug(db_ocat, "table[%s] subDICount[%d]!!!", table_name, subDICount);
                sint32 total_subDI_depth = 0;
                //删除每个次DI多余数据
                for (index = 0; index < subDICount; index++)
                {
                    sub_record_count = get_cur_tab_subDI_record_count(pinfo, depthInfo->subDIInfo[index].subDI, table_name);
                    olog_debug(db_ocat, "table[%s], subDI[%x], record count[%d], save_depth[%d]", table_name,
                               depthInfo->subDIInfo[index].subDI, sub_record_count, depthInfo->subDIInfo[index].depth);
                    total_subDI_depth = total_subDI_depth + depthInfo->subDIInfo[index].depth;
                    if (sub_record_count > depthInfo->subDIInfo[index].depth)
                    {
                        olog_warn(db_ocat, "table[%s], subDI[%x], record count[%d], save_depth[%d]", table_name,
                               depthInfo->subDIInfo[index].subDI, sub_record_count, depthInfo->subDIInfo[index].depth);

                        snprintf(buf, sizeof(buf), "where SubDataDI = %u", depthInfo->subDIInfo[index].subDI);
                        del_over_records_by_no(pinfo, table_name, depthInfo->subDIInfo[index].depth, sub_record_count, buf);
                        deleteCount = deleteCount + (sub_record_count - depthInfo->subDIInfo[index].depth);
                        hasDelete = true;
                    }
                    else
                    {
                        olog_debug(db_ocat, "no need to delete some subDI[%x] record", depthInfo->subDIInfo[index].subDI);
                    }
                }

                olog_debug(db_ocat, "save defaultDepth[%d], total_sub_depth[%d], delete count[%d]", depthInfo->defaultDepth,
                           total_subDI_depth, deleteCount);
                if(total_subDI_depth > depthInfo->defaultDepth)
                {
                    depthInfo->defaultDepth = total_subDI_depth;
                }
                //删除后，检查剩余数据是否还大于配置的存储深度
                if ((cur_count - deleteCount) > depthInfo->defaultDepth)
                {
                    olog_warn(db_ocat, "table[%s] need to delete more[%d]", table_name, cur_count - deleteCount - depthInfo->defaultDepth);
                    char condition[1024] = {0};
                    char tmpStr[64]     = {0};

                    //sprintf(tmpStr, "SubDataDI not in (", depthInfo->subDIInfo[index].subDI);
                    strcat(condition, "SubDataDI not in (");
                    memset(tmpStr, 0, sizeof(tmpStr));
                    
                    for (index = 0; index < subDICount - 1; index++)
                    {
                        sprintf(tmpStr, "%u, ", depthInfo->subDIInfo[index].subDI);
                        if(strlen(condition) + strlen(tmpStr) >1010)
                        {
                            olog_error(db_ocat, "condition len out [%u]", strlen(condition) + strlen(tmpStr));
                            break;
                        }
                        strcat(condition, tmpStr);
                        memset(tmpStr, 0, sizeof(tmpStr));
                    }

                    sprintf(tmpStr, "%u )", depthInfo->subDIInfo[index].subDI);
                    strcat(condition, tmpStr);

                    //按条件删除
                    snprintf(buf, sizeof(buf), "where %s", condition);
                    del_over_records_by_no(pinfo, table_name, depthInfo->defaultDepth, cur_count - deleteCount, buf);
                    hasDelete = true;
                }
            }
        }
        else    //按照默认配置删除多余数据
        {
            olog_debug(db_ocat, "===cur_count[%d], deleteDepthValue[%d]", cur_count, g_configObj.dbCfg.deleteDepthValue);
            if (cur_count > g_configObj.dbCfg.deleteDepthValue)
            {
                olog_warn(db_ocat, "====delete some record in table[%s] cur_count[%d],deleteDepthValue[%u]!!!", table_name, cur_count, g_configObj.dbCfg.deleteDepthValue);
                //按条件删除
//                sprintf(buf, "ORDER BY No ASC LIMIT %d, 1;", cur_count - g_configObj.dbCfg.deleteDepthValue);
//                del_over_records_by_no(pinfo, data_type, buf);
                del_over_records_by_no(pinfo, table_name, g_configObj.dbCfg.deleteDepthValue, cur_count, NULL);
                hasDelete = true;
            }
        }
    }

    if (hasDelete == true)
    {
#if 0
        //回收空间
        //执行SQL语句
        rc = sqlite3_exec(pinfo->handle->db_writehander_get(pinfo->handle), "vacuum;", NULL, NULL, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            olog_error(db_ocat, "vacuum Fail ** err:%s\n", zErrMsg);
			if (zErrMsg != NULL)
		    {
		        sqlite3_free(zErrMsg);
		        zErrMsg = NULL;
		    }
        }
        else
        {
            //同步
            //执行SQL语句
            rc = sqlite3_exec(pinfo->handle->db_readhander_get(pinfo->handle), "vacuum;", NULL, NULL, &zErrMsg);
			if (zErrMsg != NULL)
		    {
		        sqlite3_free(zErrMsg);
		        zErrMsg = NULL;
		    }
        }
#endif
    }

    if (zErrMsg != NULL)
    {
        sqlite3_free(zErrMsg);
        zErrMsg = NULL;
    }
    return 0;
}

/**
*********************************************************************
* @brief： wal资源回收
* @param[in]：
            
* @param[out]：
        
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 loop_dele_wal(void)
{
    sint32 ret = S_OK;
    struct stat st;
    sint8 filepath[1024]  = {0};
//    size_t isize;
    
    qhashtbl_obj_t obj;
    qhashtbl_t *tbl     = g_db_c_tbl;
    DB_TAB_RECORD_INFO_T* data;

    if (!tbl)
    {
        olog_error(db_ocat, "tbl指针 为空 \n");
        return S_NG;
    }

    memset((void *)&obj, 0, sizeof(obj));
    while (tbl->getnext(tbl, &obj, false) == true)
    {
        //检查删除超过深度的记录
        data = (DB_TAB_RECORD_INFO_T*)(obj.data);
        if (data == NULL || data->tab_name == NULL || data->handle ==NULL)
        {
            continue;
        }

        snprintf(filepath, sizeof(filepath), "%s%s.db-wal", g_configObj.dbCfg.basePath, data->tab_name);
        if (access(filepath, 0) == -1)
        {
            continue;
        }
        
        stat( filepath, &st );
        if (st.st_size > 1024*1024*4) 
        {
            tbl->lock(tbl);
			
            data->tab_status = ENUM_TABLE_CLOSE;
            db_close(data->handle);
            
            tbl->unlock(tbl);
            olog_debug(db_record_status, "--table[%s], closed", data->tab_name);
        }

    }

    return ret;
}

/**
*********************************************************************
* @brief： 遍历删除
* @param[in]：
            
* @param[out]：
        
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 loop_dele_records(void)
{
    qhashtbl_obj_t obj;
    qhashtbl_t *tbl     = g_db_c_tbl;
    sint8 buf[64]       = {0};
    sint32 ret          = S_OK;
    //uint32 depth;
    bool depthInfoValid = false;

    if (!tbl)
    {
        olog_error(db_ocat, "tbl指针 为空 \n");
        return S_NG;
    }

    memset((void *)&obj, 0, sizeof(obj));
//    tbl->lock(tbl);
    while (tbl->getnext(tbl, &obj, false) == true)
    {
        //depth = 0;
        olog_debug(db_ocat, "====obj.name = %s buf=%s", obj.name, buf);
        memset(buf, 0x00, sizeof(buf));
        memcpy(buf, obj.name + 4, strlen(obj.name) - 4);
        olog_debug(db_ocat, "====table major DI[%s]", buf);
        DEPTH_INFO_T depthInfo;
        uint32 tmpMajorDI = 0;
        sscanf(buf, "%x", &tmpMajorDI);

        ret = get_depth_Info(&tmpMajorDI, &depthInfo);
        if (S_OK == ret)
        {
            olog_debug(db_ocat, "get priDI[%s] success", buf);
            // debug
            olog_debug(db_ocat, "majorDI[%x], default depth[%d]", depthInfo.majorDI, depthInfo.defaultDepth);
            // end

            depthInfoValid = true;
        }
        else
        {
            olog_debug(db_ocat, "get priDI[%s] failed, so use the default depth", buf);
            // get_depth_Info(NULL, &depthInfo);
            depthInfoValid = false;
        }

#if 0
        depth = get_depth_value_pridi(buf,'s');
        if (depth == 0)
        {
            continue;
        }
#endif

        //检查删除超过深度的记录
        DB_TAB_RECORD_INFO_T* data = (DB_TAB_RECORD_INFO_T*)(obj.data);
        if (data->tab_status == ENUM_TABLE_CLOSE) {
            olog_debug(db_ocat, "The table[%s] has already closed, so need to reopen", data->tab_name);
            sint8 dbfile[1024]  = {0};
            snprintf(dbfile, sizeof(dbfile), "%s%s.db", g_configObj.dbCfg.basePath, data->tab_name);
            DB_TAB_RECORD_INFO_T info;
            memset(&info, 0, sizeof(DB_TAB_RECORD_INFO_T));
			info.tab_status = ENUM_TABLE_CLOSE;
            ret = db_open(dbfile, NULL, DB_TYPE_RECORD, &(info.handle));
            if (ret == SQLITE_OK)
            {
                strcpy(info.tab_name, data->tab_name);
                del_over_records(&info, &depthInfo, depthInfoValid);
                db_close(info.handle);
            }
        } 
        else 
        {
            tbl->lock(tbl);
            data->refer_count = data->refer_count + 1;
            clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));
            tbl->unlock(tbl);
            olog_debug(db_record_status, "++table[%s], refer_cnt[%d], status[%d], tv_sec[%lu], tv_nsec[%lu]",
                    data->tab_name, data->refer_count, data->tab_status, data->tv.tv_sec, data->tv.tv_nsec);
            del_over_records(data, &depthInfo, depthInfoValid);
            tbl->lock(tbl);
            data->refer_count = data->refer_count - 1;
            clock_gettime(CLOCK_MONOTONIC_RAW, &(data->tv));
            tbl->unlock(tbl);
            olog_debug(db_record_status, "--table[%s], refer_cnt[%d], status[%d], tv_sec[%lu], tv_nsec[%lu]",
                    data->tab_name, data->refer_count, data->tab_status, data->tv.tv_sec, data->tv.tv_nsec);
        }
        usleep(1000);
    }
//    tbl->unlock(tbl);

    return ret;
}

/********************************************************************
* @brief：  删除处理回调函数
* @param[in]：
            
* @param[out]：NONE
        
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
void *db_del_proc(void *_val)
{
    char buf[256];
    pthread_detach(pthread_self());  //注释这句代码join success
    // olog_debug(db_ocat,"del_service_started pid:%d \n", getpid());
    s_del_service_stat = 1;
    int count          = 0;

    int del_interval = g_configObj.dbCfg.deleteInterval;
    del_interval     = del_interval >> 1;
    time_t timep;
    struct tm *p;

    while (s_del_service_stat)
    {
        count = 0;
        //磁盘空间检查和处理
        olog_info(db_ocat,"====check disk space start\n");

        pthread_mutex_lock(&g_db_del_lock);

        //wal文件大小检查
        loop_dele_wal();
        
        //磁盘空间删除
        dele_db_disk_space();

        //根据表深度，遍历检查和删除
        loop_dele_records();
        
        pthread_mutex_unlock(&g_db_del_lock);

        olog_info(db_ocat,"====check disk space end\n");

        while (s_del_service_stat && count++ < del_interval)
        {
            sleep(2);
        }
        time(&timep);
        p = gmtime(&timep);
        snprintf(buf, sizeof(buf), "%04d%02d%02d %02d;%02d;%02d\n", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday,
                 p->tm_hour, p->tm_min, p->tm_sec);
        olog_debug(db_ocat, buf);
    }

    olog_debug(db_ocat, "del_service end!! \n");
    return NULL;
}
/**
*********************************************************************
* @brief：  开始数据库删除服务线程
* @param[in]：NONE
            
* @param[out]：NONE
        
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
int start_db_del_servie(void)
{
    pthread_t tid = 0;
    int ret       = pthread_create(&tid, NULL, db_del_proc, NULL);

    olog_debug(db_ocat, "start_db_del_servie pid:%lu \n", tid);
    return ret;
}
/**
*********************************************************************
* @brief：  停止数据库删除服务线程
* @param[in]：NONE
            
* @param[out]：NONE
        
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
int stop_del_service(void)
{
    s_del_service_stat = 0;
    sleep(3);

    return S_OK;
}

/**
*********************************************************************
* @brief：  关闭数据库
* @param[in]：NONE
            
* @param[out]：NONE
        
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 close_db_handle(uint8 logi_dev)
{
    qhashtbl_obj_t obj;
    qhashtbl_t *tbl    = g_db_c_tbl;
    if (!tbl)
    {
        olog_error(db_ocat, "trig_dele_records's tbl 为空 \n");
        return S_NG;
    }

    memset((void *)&obj, 0, sizeof(obj));

    
    while (tbl->getnext(tbl, &obj, 1) == true)
    {
        uint8 dev_no  = 0;
        sint8 endc[2] = {0};
        dev_no = get_logi_dev_no_by_name(obj.name, endc);
        if (dev_no != logi_dev || endc[0] != 0x00)
        {
            continue;
        }
        
        tbl->lock(tbl);
        DB_TAB_RECORD_INFO_T* data = (DB_TAB_RECORD_INFO_T *)(obj.data);
        if (data->tab_status != ENUM_TABLE_CLOSE) 
        {
            db_close(data->handle);
        }
        g_db_c_tbl->remove(g_db_c_tbl, obj.name);

        free(obj.name);
        free(obj.data);
        tbl->unlock(tbl);
    }
    
    
    return S_OK;
}
/**
*********************************************************************
* @brief：  在并发写入时的预解析执行
* @param[in]：pStmt SQL句柄
            
* @param[out]：
        
* @return： S_OK:成功；S_NG:失败
*********************************************************************/
sint32 step_db(sqlite3_stmt *pStmt)
{
    sint32 nRet   = 0;
    sint32 nRetry = 0;
//    struct timespec currenttime;
    while (nRetry++ < DB_RETRY_STEP)
    {
//        clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//        olog_info(db_ocat, "====%s, sec[%ld], nsec[%ld], START", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);
        nRet = sqlite3_step(pStmt);
//        clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//        olog_info(db_ocat, "====%s, sec[%ld], nsec[%ld], END", __FUNCTION__, currenttime.tv_sec, currenttime.tv_nsec);
        if (nRet == SQLITE_DONE)
        {
            break;
        }
        else if (nRet == SQLITE_BUSY)
        {
            olog_debug(db_ocat, "Busy sleep");
            sleep(1);
            continue;
        }
    }
    return nRet;
}

#ifdef AREA_FUJIAN
/***********************************************************************
* @brief： 操作读记录型数据
* @param[in]：
               logicalDeviceNum 逻辑设备号
               infoNum          信息点号
               majorDI          主DI
               sqlbase          sql
               nRows            记录条数
* @param[out]：
               pSelectList      记录list
* @return： 返回值说明
**********************************************************************/
sint32 m_read_record_data_fj(uint8 logicalDeviceNum,
                          uint16 infoNum,
                          uint32 majorDI,
                          char *sqlbase,
                          uint32 *nRows,
                          qlist_t *pSelectList)
{
    sint32 datalen      = 0;
    sint32 checklen     = 0;
    uint32 nCols        = 0;
    //uint32 i            = 0;
    uint32 nRet         = -1;
    sint32 nResult      = -1;
    //sint8 skey[64]      = {0};
    sint8 sql[4096]      = {0};
    qlist_t *pList      = NULL;
    sqlite3_stmt *pStmt = NULL;

//    uint32 lastStorageTime           = 0;
//    uint16 lastInfoNum               = 0;
    DB_TAB_RECORD_INFO_T hRecordinfo = {0};
    sint8 data_type                  = ENUM_INIT_PARAM;


    sint8 talbe_name[64]      = {0};
//    struct timespec currenttime;

    //取得记录型表情报
    get_collect_tab_info(logicalDeviceNum, infoNum, majorDI, 0, &hRecordinfo, true);
    if (hRecordinfo.handle != NULL)
    {
        sqlite3 *db = hRecordinfo.handle->db_readhander_get(hRecordinfo.handle);
        while(data_type <= ENUM_NOT_INIT_DATA)
        {
            if (data_type == 0)
            {
                snprintf(talbe_name, sizeof(talbe_name), "%s", hRecordinfo.tab_name);
                data_type = ENUM_INIT_PARAM;
            }
            else
            {
                snprintf(talbe_name, sizeof(talbe_name), "%s%d", hRecordinfo.tab_name, data_type);
                data_type++;
            }
            
            snprintf(sql, sizeof(sql), sqlbase, talbe_name);

            //#ifdef SQL_DUMP_DEBUG
            olog_debug(db_ocat, "SQL:[%s]", sql);
            //#endif
            nResult = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
            if (nResult == SQLITE_OK)
            {
                //执行预编译的SQL,如果存在行记录
                sint8 loop_flag = 1;
                while (loop_flag)
                {
                    sint8 retry = 3;
                    while (retry > 1)
                    {
                        nResult = sqlite3_step(pStmt);
                        if (nResult == SQLITE_DONE)
                        {
                            olog_debug(db_ocat,"SQL nResult4:[%d]", nResult);
                            retry     = 0;
                            loop_flag = 0;
                        }
                        else if (nResult != SQLITE_ROW)
                        {
                            olog_debug(db_err, "SQL DB conflict! nResult:[%d]", nResult);
                            sqlite3_reset(pStmt);
                            nResult = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
                            retry--;
                            continue;
                        }
                        else
                        {
                            // Loop2 end
                             olog_debug(db_err,"SQL nResult3:[%d]", nResult);
                            retry = 0;
                        }
                    }

                    if (nResult != SQLITE_ROW)
                    {
                        // Loop1 end.
                         olog_debug(db_ocat,"SQL nResult2:[%d]", nResult);
                        break;
                    }

                    //取得列总数
                    nCols = sqlite3_column_count(pStmt);
                    if (nCols > 1)
                    {
                        datalen = 0;
                        //取得DataContent字段内数据长度，8是DataContent查询后字段索引值
                        datalen = sqlite3_column_bytes(pStmt, 9);
                        //给保存的记录分配内存。DataContent字段的存储空间一起分配
                        READ_RECORD_STEP_RSP_T *precord_rsp =
                            (READ_RECORD_STEP_RSP_T *)malloc(sizeof(READ_RECORD_STEP_RSP_T) + datalen);
                        if (!precord_rsp)
                        {
                            olog_error(db_ocat, "precord_rsp is NULL \n");
                            return nRet;
                        }
                        precord_rsp->infoNum     = sqlite3_column_int(pStmt, 1);
                        precord_rsp->majorDI     = sqlite3_column_int(pStmt, 2);
                        precord_rsp->minorDI     = sqlite3_column_int(pStmt, 3);
                        precord_rsp->startTime   = sqlite3_column_int(pStmt, 4);
                        precord_rsp->endTime     = sqlite3_column_int(pStmt, 5);
                        precord_rsp->storageTime = sqlite3_column_int(pStmt, 6);
                        precord_rsp->indexNum    = sqlite3_column_int(pStmt, 7);
                        precord_rsp->timeFlag    = 1;
                        if (0 != precord_rsp->minorDI)
                        {
                            precord_rsp->minorDIOptinal = 1;
                        }
                        else
                        {
                            precord_rsp->minorDIOptinal = 0;
                        }

						//福建集中器扩展接口不判校验
						
//                        checklen = sqlite3_column_bytes(pStmt, 8);
//                        if (checklen > CHECK_INFO_LEN)
//                        {
//                            olog_warn(db_ocat, "checkInfo len != CHECK_INFO_LEN");
//                            checklen = CHECK_INFO_LEN;
//                        }
//                        const char *check               = sqlite3_column_text(pStmt, 8);
                        precord_rsp->checkInfoLen = checklen;						
//                        memcpy(precord_rsp->checkInfo, check, checklen);



                        //读取Blob数据
                        const void *pData = sqlite3_column_blob(pStmt, 9);
                        //将blob数据保存到之前分配的内存空间里
                        precord_rsp->outBuf    = ((uint8 *)precord_rsp) + sizeof(READ_RECORD_STEP_RSP_T);
                        precord_rsp->outBufLen = datalen;
                        memcpy(((sint8 *)precord_rsp) + sizeof(READ_RECORD_STEP_RSP_T), pData, datalen);
                        // olog_debug(db_ocat,"pList add precord_rsp = %lx sizeof(precord_rsp):%d;\n", precord_rsp,
                        // sizeof(precord_rsp)); 将保存记录的内存地址添加到list中

//                        if (pList == 0 || lastInfoNum != precord_rsp->infoNum || lastStorageTime != precord_rsp->storageTime)
//                        {
//                            lastInfoNum     = precord_rsp->infoNum;
//                            lastStorageTime = precord_rsp->storageTime;
                            pList           = qlist(0);  // QLIST_THREADSAFE is need?
                            if (pList)
                            {
                                pSelectList->addlast(pSelectList, &pList, sizeof(pList));
                            }
//                        }

                        //??
                        if (pList == NULL)
                        {
                        	free(precord_rsp);
                            olog_error(db_ocat, "qlist malloc fail");
                        }
                        else
                        {
                            olog_debug(db_ocat, "create qlist putin ptable");
                            pList->addlast(pList, &precord_rsp, sizeof(precord_rsp));
                        }

                        (*nRows)++;
                    }
                    else
                    {
                        olog_debug(db_ocat, "nCols =%d \n", nCols);
                    }
                }
            }
            else
            {
                olog_debug(db_ocat, "start step read prepare nResult is %d SQL:[%s]", nResult, sql);
            }

            if (NULL != pStmt)
            {
                sqlite3_finalize(pStmt);
            }

        }
        put_collect_tab_info(logicalDeviceNum, infoNum, majorDI, 0);
    } 
    else
    {
        olog_warn(db_ocat, "Can not find the table [%u]:[%u]:[%u]", logicalDeviceNum, infoNum, majorDI);
    }
    

    return nRet;
}

#endif

/**
*********************************************************************
* @brief： 操作读记录型数据
* @param[in]：
               logicalDeviceNum 逻辑设备号
               infoNum          信息点号
               majorDI          主DI
               sqlbase          sql
               nRows            记录条数
* @param[out]：
               pSelectList      记录list
* @return： 返回值说明
*********************************************************************
*/
sint32 m_read_record_data(uint8 logicalDeviceNum,
                          uint16 infoNum,
                          uint32 majorDI,
                          char *sqlbase,
                          uint32 *nRows,
                          qlist_t *pSelectList)
{
    sint32 datalen      = 0;
    sint32 checklen     = 0;
    uint32 nCols        = 0;
    //uint32 i            = 0;
    uint32 nRet         = -1;
    sint32 nResult      = -1;
    //sint8 skey[64]      = {0};
    sint8 sql[4096]      = {0};
    qlist_t *pList      = NULL;
    sqlite3_stmt *pStmt = NULL;

    uint32 lastStorageTime           = 0;
    uint16 lastInfoNum               = 0;
    DB_TAB_RECORD_INFO_T hRecordinfo = {0};
#if AREA_FUJIAN
    sint8 data_type                  = ENUM_INIT_PARAM;
#else
    sint8 data_type                  = ENUM_NORMAL_DATA;
#endif

    sint8 talbe_name[64]      = {0};
//    struct timespec currenttime;

    //取得记录型表情报
    get_collect_tab_info(logicalDeviceNum, infoNum, majorDI, 0, &hRecordinfo, true);
    if (hRecordinfo.handle != NULL)
    {
        sqlite3 *db = hRecordinfo.handle->db_readhander_get(hRecordinfo.handle);
        while(data_type <= ENUM_NOT_INIT_DATA)
        {
            snprintf(talbe_name, sizeof(talbe_name), "%s%d", hRecordinfo.tab_name, data_type);
            data_type++;
			
            snprintf(sql, sizeof(sql), sqlbase, talbe_name);

            //#ifdef SQL_DUMP_DEBUG
            olog_debug(db_ocat, "SQL:[%s]", sql);
            //#endif
            nResult = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
            if (nResult == SQLITE_OK)
            {
                //执行预编译的SQL,如果存在行记录
                sint8 loop_flag = 1;
                while (loop_flag)
                {
                    sint8 retry = 3;
                    while (retry > 1)
                    {
                        nResult = sqlite3_step(pStmt);
                        if (nResult == SQLITE_DONE)
                        {
                            olog_debug(db_ocat,"SQL nResult4:[%d]", nResult);
                            retry     = 0;
                            loop_flag = 0;
                        }
                        else if (nResult != SQLITE_ROW)
                        {
                            olog_debug(db_err, "SQL DB conflict! nResult:[%d]", nResult);
                            sqlite3_reset(pStmt);
                            nResult = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
                            retry--;
                            continue;
                        }
                        else
                        {
                            // Loop2 end
                             olog_debug(db_err,"SQL nResult3:[%d]", nResult);
                            retry = 0;
                        }
                    }

                    if (nResult != SQLITE_ROW)
                    {
                        // Loop1 end.
                         olog_debug(db_ocat,"SQL nResult2:[%d]", nResult);
                        break;
                    }

                    //取得列总数
                    nCols = sqlite3_column_count(pStmt);
                    if (nCols > 1)
                    {
                        datalen = 0;
                        //取得DataContent字段内数据长度，8是DataContent查询后字段索引值
                        datalen = sqlite3_column_bytes(pStmt, 9);
                        //给保存的记录分配内存。DataContent字段的存储空间一起分配
                        READ_RECORD_STEP_RSP_T *precord_rsp =
                            (READ_RECORD_STEP_RSP_T *)malloc(sizeof(READ_RECORD_STEP_RSP_T) + datalen);
                        if (!precord_rsp)
                        {
                            olog_error(db_ocat, "precord_rsp is NULL \n");
                            return nRet;
                        }
                        precord_rsp->infoNum     = sqlite3_column_int(pStmt, 1);
                        precord_rsp->majorDI     = sqlite3_column_int(pStmt, 2);
                        precord_rsp->minorDI     = sqlite3_column_int(pStmt, 3);
                        precord_rsp->startTime   = sqlite3_column_int(pStmt, 4);
                        precord_rsp->endTime     = sqlite3_column_int(pStmt, 5);
                        precord_rsp->storageTime = sqlite3_column_int(pStmt, 6);
                        precord_rsp->indexNum    = sqlite3_column_int(pStmt, 7);
                        precord_rsp->timeFlag    = 1;
                        if (0 != precord_rsp->minorDI)
                        {
                            precord_rsp->minorDIOptinal = 1;
                        }
                        else
                        {
                            precord_rsp->minorDIOptinal = 0;
                        }

                        checklen = sqlite3_column_bytes(pStmt, 8);
                        if (checklen > CHECK_INFO_LEN)
                        {
                            olog_warn(db_ocat, "checkInfo len != CHECK_INFO_LEN");
                            checklen = CHECK_INFO_LEN;
                        }
						#if AREA_FUJIAN
						    const unsigned char *check      = sqlite3_column_text(pStmt, 8);
						#else
						    const char *check               = sqlite3_column_blob(pStmt, 8);
						#endif

                        precord_rsp->checkInfoLen = checklen;
                        memcpy(precord_rsp->checkInfo, check, checklen);

                        //读取Blob数据
                        const void *pData = sqlite3_column_blob(pStmt, 9);
                        //将blob数据保存到之前分配的内存空间里
                        precord_rsp->outBuf    = ((uint8 *)precord_rsp) + sizeof(READ_RECORD_STEP_RSP_T);
                        precord_rsp->outBufLen = datalen;
                        memcpy(((sint8 *)precord_rsp) + sizeof(READ_RECORD_STEP_RSP_T), pData, datalen);
                        // olog_debug(db_ocat,"pList add precord_rsp = %lx sizeof(precord_rsp):%d;\n", precord_rsp,
                        // sizeof(precord_rsp)); 将保存记录的内存地址添加到list中

                        if (pList == 0 || lastInfoNum != precord_rsp->infoNum || lastStorageTime != precord_rsp->storageTime)
                        {
                            lastInfoNum     = precord_rsp->infoNum;
                            lastStorageTime = precord_rsp->storageTime;
                            pList           = qlist(0);  // QLIST_THREADSAFE is need?
                            if (pList)
                            {
                                pSelectList->addlast(pSelectList, &pList, sizeof(pList));
                            }
                        }

                        //??
                        if (pList == NULL)
                        {
                        	free(precord_rsp);
                            olog_error(db_ocat, "qlist malloc fail");
                        }
                        else
                        {
                            olog_debug(db_ocat, "create qlist putin ptable");
                            pList->addlast(pList, &precord_rsp, sizeof(precord_rsp));
                        }

                        (*nRows)++;
                    }
                    else
                    {
                        olog_debug(db_ocat, "nCols =%d \n", nCols);
                    }
                }
            }
            else
            {
                olog_debug(db_ocat, "start step read prepare nResult is %d SQL:[%s]", nResult, sql);
            }

            if (NULL != pStmt)
            {
                sqlite3_finalize(pStmt);
            }

        }
        put_collect_tab_info(logicalDeviceNum, infoNum, majorDI, 0);
    } 
    else
    {
        olog_warn(db_ocat, "Can not find the table [%u]:[%u]:[%u]", logicalDeviceNum, infoNum, majorDI);
    }
    

    return nRet;
}

/**
*********************************************************************
* @brief： 根据主DI读普通型数据
* @param[in]：
               db               数据库句柄
               sqlbase          sql
               tableName        表名
               majorDI          主DI
               nRows            记录条数
* @param[out]：
               pSelectList      记录list
* @return： 返回值说明
*********************************************************************
*/
sint32 m_read_normal_data_all_info(sqlite3 *db,
                                   sint8 *sqlbase,
                                   sint8 *tableName,
                                   uint32 majorDI,
                                   qlist_t *pSelectList,
                                   uint32 *nRows)
{
    sint32 datalen  = 0;
    uint32 nCols    = 0;
    sint32 nResult = -1;

    sqlite3_stmt *pStmt = NULL;

    //===============================================
    // sint8 skey[64] = {0};
    sint8 sql[1600] = {0};
    uint32 lastStorageTime = 0;
    uint16 lastInfoNum = 0;
//    struct timespec currenttime;
    qlist_t *pList         = NULL;

    snprintf(sql, sizeof(sql), sqlbase, tableName);
    olog_debug(db_ocat, "SQL:[%s]", sql);

    nResult = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
    if (nResult == SQLITE_OK)
    {
        //执行预编译的SQL,如果存在行记录
        sint8 loop_flag = 1;
        while (loop_flag)
        {
            sint8 retry = 3;
            while (retry > 1)
            {
//                clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//                olog_info(db_ocat, "====%s, SQL[%s], sec[%ld], nsec[%ld], START", __FUNCTION__, sql, currenttime.tv_sec, currenttime.tv_nsec);
                nResult = sqlite3_step(pStmt);
//                clock_gettime(CLOCK_MONOTONIC_RAW, &currenttime);
//                olog_info(db_ocat, "====%s, SQL[%s], sec[%ld], nsec[%ld], END", __FUNCTION__, sql, currenttime.tv_sec, currenttime.tv_nsec);
                if (nResult == SQLITE_DONE)
                {
                    // olog_debug(db_ocat,"SQL nResult4:[%d]", nResult);
                    retry     = 0;
                    loop_flag = 0;
                }
                else if (nResult != SQLITE_ROW)
                {
                    olog_debug(db_ocat, "SQL DB conflict! nResult:[%d]", nResult);
                    sqlite3_reset(pStmt);
                    nResult = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);
                    retry--;
                    continue;
                }
                else
                {
                    // Loop2 end
                    // olog_debug(db_ocat,"SQL nResult3:[%d]", nResult);
                    retry = 0;
                }
            }

            if (nResult != SQLITE_ROW)
            {
                // Loop1 end.
                // olog_debug(db_ocat,"SQL nResult2:[%d]", nResult);
                break;
            }

            //取得列总数
            nCols = sqlite3_column_count(pStmt);
            if (nCols > 1)
            {
                datalen = 0;
                //取得DataContent字段内数据长度，8是DataContent查询后字段索引值
                datalen = sqlite3_column_bytes(pStmt, 3);
                //给保存的记录分配内存。DataContent字段的存储空间一起分配
                READ_RECORD_STEP_RSP_T *precord_rsp =
                    (READ_RECORD_STEP_RSP_T *)calloc(1,sizeof(READ_RECORD_STEP_RSP_T) + datalen);
                if (!precord_rsp)
                {
                    olog_error(db_ocat, "precord_rsp malloc fail \n");
					if (NULL != pStmt)
				    {
				        sqlite3_finalize(pStmt);
				    }
                    return -1;
                }
                precord_rsp->infoNum = sqlite3_column_int(pStmt, 1);
                if (majorDI)
                {
                    precord_rsp->majorDI = sqlite3_column_int(pStmt, 2);
                    precord_rsp->minorDI = 0;
                    olog_debug(db_ocat, "majorDI=%d", precord_rsp->majorDI);
                }
                else
                {
                    precord_rsp->minorDI = sqlite3_column_int(pStmt, 2);
                    precord_rsp->majorDI = 0;
                    olog_debug(db_ocat, "minorDI=%d", precord_rsp->minorDI);
                }

                precord_rsp->startTime   = 0;
                precord_rsp->endTime     = 0;
                precord_rsp->storageTime = 0;
                precord_rsp->indexNum    = 0;
                precord_rsp->timeFlag    = 0;


                //读取Blob数据
                const void *pData = sqlite3_column_blob(pStmt, 3);
                //将blob数据保存到之前分配的内存空间里
                precord_rsp->outBuf    = ((uint8 *)precord_rsp) + sizeof(READ_RECORD_STEP_RSP_T);
                precord_rsp->outBufLen = datalen;

                memcpy(((sint8 *)precord_rsp) + sizeof(READ_RECORD_STEP_RSP_T), pData, datalen);

                if (pList == 0 || lastInfoNum != precord_rsp->infoNum || lastStorageTime != precord_rsp->storageTime)
                {
                    lastInfoNum     = precord_rsp->infoNum;
                    lastStorageTime = precord_rsp->storageTime;
                    pList           = qlist(0);  // QLIST_THREADSAFE is need?
                    if(pList != NULL)
                    {
                        pSelectList->addlast(pSelectList, &pList, sizeof(pList));
                    }

                }

                //??
                if (pList == NULL)
                {
                    olog_error(db_ocat, "qlist malloc fail");
					free(precord_rsp);
                }
                else
                {
                    olog_debug(db_ocat, "create qlist putin ptable");
                    pList->addlast(pList, &precord_rsp, sizeof(precord_rsp));
                }

                (*nRows)++;
            }
            else
            {
                olog_debug(db_ocat, "nCols =%d \n", nCols);
            }
        }
    }
    else
    {
        olog_debug(db_ocat, "start step read prepare nResult is %d", nResult);
    }

    if (NULL != pStmt)
    {
        sqlite3_finalize(pStmt);
    }

    return S_OK;
}

/**
*********************************************************************
* @brief： 读普通型数据
* @param[in]：
               logicalDeviceNum       逻辑设别号
               infoNum                信息点号
               majorDI                主DI
               minorDINum             次DI数量
               minorDIs               次DI数组
               sql                    sql语句
* @param[out]：
               nRows                  记录条数
               pSelectList            记录list
* @return： 返回值说明
*********************************************************************
*/
sint32 m_read_all_normal_data(uint8 logicalDeviceNum,
                              uint16 infoNum,
                              uint32 majorDI,
                              uint32 minorDINum,
                              uint32 *minorDIs,
                              char *sql,
                              uint32 *nRows,
                              qlist_t *pSelectList)
{
    sint32 i                         = 0;
    DB_TAB_NORMAL_INFO_T hNormalinfo = {0};
    qhashtbl_t *selectedTable        = NULL;
    uint32 tmpnum                    = 0;

    selectedTable = qhashtbl(0, 0);

    if (majorDI != 0)
    {
        //根据dataTag，取得数据库情报
        if (0 == get_normal_tab_info(majorDI, 0, &hNormalinfo))
        {
            m_read_normal_data_all_info(hNormalinfo.handle->db_readhander_get(hNormalinfo.handle), sql,
                                        hNormalinfo.tab_name, majorDI, pSelectList, nRows);
        }

        if (pSelectList->num <= 0)
        {
            get_normal_tab_info(majorDI, 1, &hNormalinfo);
            m_read_normal_data_all_info(hNormalinfo.handle->db_readhander_get(hNormalinfo.handle), sql,
                                        hNormalinfo.tab_name, majorDI, pSelectList, nRows);
        }
    }
    else
    {
        for (i = 0; i < minorDINum; i++)
        {
            tmpnum = pSelectList->num;
            if (0 == get_normal_tab_info(minorDIs[i], 0, &hNormalinfo))
            {
                if (1 == selectedTable->getint(selectedTable, hNormalinfo.tab_name))
                {
                    olog_debug(db_ocat, "The database[%s] was searched", hNormalinfo.tab_name);
                }
                else
                {
                    m_read_normal_data_all_info(hNormalinfo.handle->db_readhander_get(hNormalinfo.handle), sql,
                                                hNormalinfo.tab_name, majorDI, pSelectList, nRows);
                    
                    if (pSelectList->num == tmpnum)
                    {
                        get_normal_tab_info(minorDIs[i], 1, &hNormalinfo);
                        m_read_normal_data_all_info(hNormalinfo.handle->db_readhander_get(hNormalinfo.handle), sql,
                                                    hNormalinfo.tab_name, majorDI, pSelectList, nRows);
                    }
                    selectedTable->putint(selectedTable, hNormalinfo.tab_name, 1);
                }
            }
        }
    }
	selectedTable->clear(selectedTable);
    selectedTable->free(selectedTable);
    return 0;
}

int getFileSize(const char* path)
{
    int fileSize = 0;
    struct stat statbuffer;
    if (stat(path, &statbuffer) < 0) {
        return fileSize;
    } else {
        fileSize = statbuffer.st_size;
    }

    return fileSize;
}
bool get_system_tf_free(char* path, double *free) {
    if ((path == NULL) || (free == NULL)) {
        return false;
    }

    struct statfs diskInfo;
    memset(&diskInfo, 0, sizeof(struct statfs));
    if (statfs(path, &diskInfo) == -1) {
        return -1;
    }

    unsigned long long totalBlocks = diskInfo.f_bsize;
    unsigned long long freeDisk = diskInfo.f_bavail * totalBlocks;

    *free = freeDisk;

    return 0;
}

/**
*********************************************************************
* @brief： 数据库是否有损坏
* @param[in]：
               path       数据库文件
               filename   文件名
* @param[out]：
               无
* @return： 返回值说明   S_OK    文件未损坏
                       S_NG    文件损坏
*********************************************************************
*/
sint32 check_database_error(char* path, char* filename)
{
    sint32 ret = S_OK;
    sint8 command[1024] = {0};
    sint8 buf[128] = {0};
    FILE* fp = NULL;
    FILE* fp1 = NULL;

    struct statfs diskInfo;
    size_t mbTotalsize;
    uint64 freeDisk;
    size_t mbFreedisk;
    uint64 totalBlocks;
    sint8 *checkPath = g_configObj.dbCfg.checkDataPath;
    statfs(checkPath, &diskInfo);

    totalBlocks = diskInfo.f_bsize;
    // totalSize = totalBlocks * diskInfo.f_blocks;
    mbTotalsize = diskInfo.f_blocks * 4;
    freeDisk    = diskInfo.f_bavail * totalBlocks;
    mbFreedisk  = freeDisk >> 10;
    olog_debug(db_ocat, "%s  total=%dKB, free=%dKB\n", checkPath, mbTotalsize, mbFreedisk);
    float rate = (float)mbFreedisk / (float)mbTotalsize;
    float rateLimit =  (float)g_configObj.dbCfg.autoDeleteNum /100;
    olog_debug(db_ocat, "now disk free rate[%f], free rate limt[%f]", rate, rateLimit);
    //磁盘空间满的时候，不检查。
    if (rate < 0.01)
    {
        return S_OK;
    }
    
    olog_debug(db_err, "%s, fileName[%s]", __FUNCTION__, filename);
    do {
        //数据库是否有损坏
        sprintf(command, "%s %s \"PRAGMA integrity_check;\" 2>&1", g_configObj.dbCfg.toolSqlite,path);
        olog_debug(db_recovery,"command:[%s]",command);
        if ((fp = popen(command, "r")) != NULL)
        {
            fgets(buf, 127, fp);
            //olog_debug(db_err, "buf[%s]", buf);
            if ( strstr(buf, "disk I/O error") != NULL) {
                olog_error(db_recovery, "No space left on device!!!!!!!");
            } else if ((strstr(buf, "ok") == NULL)) {
                olog_error(db_recovery,"check_database db not pass:[%s]!!!",path);
                ret = S_NG;
                break;
            }
            else
            {
                olog_debug(db_recovery,"check_database db pass:[%s]",path);
            }
        }

        //数据表格是否有损坏
        memset(command, 0, sizeof(command));
        memset(buf, 0, sizeof(buf));
        sprintf(command, "%s %s .table 2>&1", g_configObj.dbCfg.toolSqlite, path);
        olog_debug(db_recovery, "command[%s]", command);
        if ((fp1 = popen(command, "r")) != NULL)
        {
            fgets(buf, 127, fp1);
            olog_debug(db_err, "buf[%s]", buf);
            if (strstr(buf, "disk I/O error") != NULL) {
                olog_error(db_recovery, "No space left on device!!!!!!!");
            } else if (strstr(buf, filename) == NULL)
            {
                ret = S_NG;
                olog_info(db_recovery,"check_database table not pass:[%s]!!!",path);
                break;
            }
            else
            {
                olog_debug(db_recovery,"check_database table pass:[%s]!!!",path);
            }
        }

        olog_info(db_recovery,"check_database db ok:[%s]",path);

    } while(0);

    //关闭文件
    if (fp != NULL) {
        pclose(fp);
        fp = NULL;
    }

    //关闭文件
    if (fp1 != NULL) {
        pclose(fp1);
        fp1 = NULL;
    }

    if (ret == S_NG)
    {
        //获取文件大小
        int fileSize = getFileSize(path);
        olog_debug(db_recovery, "file[%s], size[%d]", path, fileSize);
        //获取磁盘剩余空间大小
        double diskFreeSize = 0.0;
        get_system_tf_free(g_configObj.dbCfg.checkDataPath, &diskFreeSize);
        olog_debug(db_recovery, "disk[%s], free[%f]", g_configObj.dbCfg.checkDataPath, diskFreeSize);
        if (diskFreeSize <= fileSize * 2) 
        {
            olog_warn(db_recovery, "The disk space is not enough, so can not do db[%s] recovery!!!", path);
            ret = S_OK;
        }
    }

    return ret;
}

/**
*********************************************************************
* @brief： 命令执行函数
* @param[in]：
               cmd          命令
               db           sql句柄
               table_name   表名
* @param[out]：
               无
* @return： 返回值说明   0    成功
                       -1   失败
*********************************************************************
*/
sint32 db_cmd_execute(const char* cmd, sqlite3 *db, char * table_name)
{
    char resultBuf[40960 + 1];
    char tmpCmd[512 + 1] = {0};
    char insert_sql[1024] = {0};
    char delete_sql[1024] = {0};
    char update_sql[1024] = {0};
    int insert_sql_len = 0;
    int delete_sql_len = 0;
    int update_sql_len = 0;
    FILE* fp = NULL;
    int nRc = 0;
    char *errMsg;

    sprintf(insert_sql,"INSERT INTO %s",table_name);
    sprintf(delete_sql,"DELETE FROM %s",table_name);
    sprintf(update_sql,"UPDATE %s",table_name);
    insert_sql_len = strlen(insert_sql);
    delete_sql_len = strlen(delete_sql);
    update_sql_len = strlen(update_sql);


    strncpy(tmpCmd, cmd, 512);
    if ((fp = popen(tmpCmd, "r")) != NULL)
    {
        while( fgets(resultBuf, sizeof(resultBuf)-1, fp) != NULL) {
            //olog_debug(db_ocat, "read[%s]", resultBuf);
#if 0
            if (strstr(resultBuf, "INSERT") 
            || strstr(resultBuf, "DELETE"))
#else
            if( strncmp(resultBuf,insert_sql,insert_sql_len) == 0
            || strncmp(resultBuf,delete_sql,delete_sql_len) == 0
            || strncmp(resultBuf,update_sql,update_sql_len) == 0)
#endif
            {
#if 0
                nRc = sqlite3_prepare_v2(db, resultBuf, -1, &pStmt, 0);
                memset(outbuf, 1, sizeof(outbuf));
                nRc = sqlite3_bind_blob(pStmt, 1, outbuf, 4096, NULL);
                nRc = sqlite3_step(pStmt);
                sqlite3_finalize(pStmt);
#else
                nRc = sqlite3_exec(db,resultBuf,NULL,NULL,&errMsg);
                if(nRc != SQLITE_OK)
                {
                    olog_error(db_err,"SQL:[%s] exec fail:%s",resultBuf,errMsg);
                    sqlite3_free(errMsg);
                }

#endif
            }
        }
        pclose(fp);
        fp = NULL;
        
    } else {
        olog_error(db_err, "popen %s error", tmpCmd);
        return -1;
    }
    return 0;
}



/**
*********************************************************************
* @brief： 数据库恢复
* @param[in]：
               databaseName         数据库名
               table_name           表名
               isBackup             是否有备份
* @param[out]：
               无
* @return： 返回值说明   0    成功
                       -1   失败
*********************************************************************
*/
sint32 recovery_database(char *databaseName, char *table_name, int isBackup)
{
    olog_debug(db_err, "%s, db[%s]", __FUNCTION__, databaseName);
    // char databaseSql[512] = {0};
    char command[1024] = {0};
    char create_index[512] = {0};
    char* tableSql = NULL;
    char sqlbuf[512] = {0};
    char buf[1024] = {0};
    char timeNow[128];
#if 1
    sint8 data_type                  = 0;
#else
    sint8 data_type                  = ENUM_NORMAL_DATA;
#endif
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time); 

    sprintf(timeNow,"%ld_%ld",time.tv_sec, time.tv_nsec);
    
    close_collect_tab_info(table_name);
    
    //将损坏的数据库重名命
    memset(command, 0, sizeof(command));
    sprintf(command, "mv %s-shm %s-shm_bad", databaseName, databaseName);
    system(command);

    memset(command, 0, sizeof(command));
    sprintf(command, "mv %s-wal %s-wal_bad", databaseName, databaseName);
    system(command);

    //save the old db file  db_bad
    memset(command, 0, sizeof(command));
    sprintf(command, "mv %s %s_bad", databaseName, databaseName);
    system(command);  

    sqlite3 *db;
    int iResult = 0;
    iResult = sqlite3_open_v2(databaseName, &db,
                                  SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_FULLMUTEX /*| SQLITE_OPEN_SHAREDCACHE*/, NULL);

    if (SQLITE_OK != iResult)
    {
        olog_error(db_err, "creat db failed");
        return S_NG;
    }

    sqlite3_busy_timeout(db, 50);

    sprintf(sqlbuf, "PRAGMA synchronous = %s; ", "FULL");
    //执行SQL语句
    iResult = sqlite3_exec(db, sqlbuf, 0, 0, 0);
    if (SQLITE_OK != iResult)
    {
        
        olog_error(db_err, "set FULL fail");
        return S_NG;
    }

#if 0
    iResult = sqlite3_exec(db, "PRAGMA journal_mode=WAL;", NULL, 0, 0);
    if (SQLITE_OK != iResult)
    {
        olog_error(db_err, "set WAL fail");
        return S_NG;
    }
#endif

    //olog_debug(db_err, "tableSql[%s]", tableSql);
    //执行SQL语句
    iResult = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    if (SQLITE_OK != iResult)
    {
        olog_error(db_err, "create table fail");
        return S_NG;
    }

    //record data
    char* dbName = strstr(databaseName, "T");
    if (NULL != dbName)
    {
        char table[128] = {0};
#ifdef AREA_FUJIAN
		for(data_type = ENUM_INIT_PARAM; data_type <= ENUM_NOT_INIT_DATA; data_type++)
#else
        for(data_type = ENUM_NORMAL_DATA; data_type <= ENUM_NOT_INIT_DATA; data_type++)
#endif
        {
            sprintf(table, "%s%d", table_name, data_type);
            sprintf(create_index, "CREATE  INDEX if not exists i%s ON %s(InfoAddrNo, SubDataDI , AcqStartTime,  AcqSuccessTime , AcqSaveTime)", table, table);
             sprintf(buf, "CREATE TABLE if not exists %s %s;", table, g_collectSQL);
            tableSql = buf;
            //olog_debug(db_err, "tableSql[%s]", tableSql);
            //执行SQL语句
            iResult = sqlite3_exec(db, tableSql, NULL, NULL, NULL);
            if (SQLITE_OK != iResult)
            {
                olog_error(db_err, "create table fail");
                return S_NG;
            }

            //执行SQL语句
            iResult = sqlite3_exec(db, create_index, NULL, NULL, NULL);
            if (SQLITE_OK != iResult)
            {
                olog_error(db_err, "create condition fail");
                return S_NG;
            }

            //create the sql file
            sprintf(command, "%s %s_bad \".dump\"", g_configObj.dbCfg.toolSqlite,databaseName);
            db_cmd_execute(command, db,table);

            olog_debug(db_err,"db %s recover",table);
            
        }

        
    } 
    else//普通型数据
    {
        //dump sql
        if (strstr(databaseName, TPARAMER_DB_NAME))
        {
            // sprintf(databaseSql, "/data/app/dbCenter/%s.db", DB_CENT_TABLE_NAME_PARAMETER);
            strncpy(create_index, "CREATE UNIQUE INDEX if not exists iTparameter ON dbParameterCenter (LDeviceNo,InfoAddrNo,DataID);", sizeof(create_index) - 1);
            tableSql  = "CREATE TABLE if not exists dbParameterCenter(      \
                    LDeviceNo CHAR(1),     \
                    InfoAddrNo SMALLINT,      \
                    DataID INTEGER,     \
                    DataTypeID SMALLINT,      \
                    DataContent BLOB,     \
                    UpdateTime TIMESTAMP,    \
                    CRC INTEGER \
                    );";
        } else if (strstr(databaseName, TGENERAL_DELAY_DB_NAME)) {
            // sprintf(databaseSql, "/data/app/dbCenter/%s.db", TGENERAL_DELAY_NAME);
            strncpy(create_index, "CREATE UNIQUE INDEX if not exists iTgeneral_delay ON dbGeneralDelayCenter (LDeviceNo,InfoAddrNo,DataID);", sizeof(create_index) -1);
            tableSql = "CREATE TABLE if not exists dbGeneralDelayCenter(      \
                    LDeviceNo CHAR(1),     \
                    InfoAddrNo SMALLINT,      \
                    DataID INTEGER,     \
                    DataTypeID SMALLINT,      \
                    DataContent BLOB,     \
                    UpdateTime TIMESTAMP,    \
                    CRC INTEGER \
                    );";
        } else if (strstr(databaseName, TGENERAL_DB_NAME)) {
            // sprintf(databaseSql, "/data/app/dbCenter/%s.db", DB_CENT_TABLE_NAME_NORMAL);
            strncpy(create_index, "CREATE UNIQUE INDEX if not exists iTgeneral ON dbGeneralCenter (LDeviceNo,InfoAddrNo,DataID);", sizeof(create_index) -1);
            tableSql = "CREATE TABLE if not exists dbGeneralCenter(      \
                    LDeviceNo CHAR(1),     \
                    InfoAddrNo SMALLINT,      \
                    DataID INTEGER,     \
                    DataTypeID SMALLINT,      \
                    DataContent BLOB,     \
                    UpdateTime TIMESTAMP,     \
                    CRC INTEGER \
                    );";
        } else if (strstr(databaseName, TMEMORY_DB_NAME)) {
            strncpy(create_index, "CREATE UNIQUE INDEX if not exists iTmemory ON Tmemory(LDeviceNo,InfoAddrNo,DataID);", sizeof(create_index) - 1);
            tableSql = "CREATE TABLE if not exists Tmemory( \
                    LDeviceNo CHAR(1), \
                    InfoAddrNo SMALLINT, \
                    DataID INTEGER,  \
                    DataTypeID SMALLINT,      \
                    DataContent BLOB,\
                    UpdateTime TIMESTAMP, \
                    CRC INTEGER \
                    );";
        } 
        else
        {
            olog_debug(db_err,"db %s error",table_name);
            return S_NG;
        }
        //olog_debug(db_err, "tableSql[%s]", tableSql);
        //执行SQL语句
        iResult = sqlite3_exec(db, tableSql, NULL, NULL, NULL);
        if (SQLITE_OK != iResult)
        {
            olog_error(db_err, "create table fail");
            return S_NG;
        }

        //执行SQL语句
        iResult = sqlite3_exec(db, create_index, NULL, NULL, NULL);
        if (SQLITE_OK != iResult)
        {
            olog_error(db_err, "create condition fail");
            return S_NG;
        }

        //create the sql file
        sprintf(command, "%s %s_bad \".dump\"", g_configObj.dbCfg.toolSqlite,databaseName);
        db_cmd_execute(command, db,table_name);

        

        olog_debug(db_err,"db %s recover",table_name);
    }
        
    //执行SQL语句
    iResult = sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    if (SQLITE_OK != iResult)
    {
        olog_error(db_err, "COMMIT fail");
        return S_NG;
    }

#ifdef DB_RECOVERY_DEBUG
    if(isBackup)
    {
        memset(command, 0, sizeof(command));
        sprintf(command, "mv  %s_bad %s/%s.db_bad_%s", databaseName, BAD_BACKUP_DB_PATH,table_name,timeNow);
        olog_debug(db_err, "command[%s]", command);
        system(command);

        memset(command, 0, sizeof(command));
        sprintf(command, "mv  %s-shm_bad %s/%s.db-shm_bad_%s", databaseName, BAD_BACKUP_DB_PATH,table_name,timeNow);
        olog_debug(db_err, "command[%s]", command);
        system(command);

        memset(command, 0, sizeof(command));
        sprintf(command, "mv  %s-wal_bad %s/%s.db-wal_bad_%s", databaseName, BAD_BACKUP_DB_PATH,table_name,timeNow);
        olog_debug(db_err, "command[%s]", command);
        system(command);
    }
    else
    {
        memset(command, 0, sizeof(command));
        sprintf(command, "mv  %s_bad %s/%s.db_bad_%s", databaseName, BAD_DB_PATH,table_name,timeNow);
        olog_debug(db_err, "command[%s]", command);
        system(command);

        memset(command, 0, sizeof(command));
        sprintf(command, "mv  %s-shm_bad %s/%s.db-shm_bad_%s", databaseName, BAD_DB_PATH,table_name,timeNow);
        olog_debug(db_err, "command[%s]", command);
        system(command);

        memset(command, 0, sizeof(command));
        sprintf(command, "mv  %s-wal_bad %s/%s.db-wal_bad_%s", databaseName, BAD_DB_PATH,table_name,timeNow);
        olog_debug(db_err, "command[%s]", command);
        system(command);

    }
#else
    memset(command, 0, sizeof(command));
    sprintf(command, "rm  %s_bad", databaseName);
    olog_debug(db_err, "command[%s]", command);
    system(command);

    memset(command, 0, sizeof(command));
    sprintf(command, "rm  %s-shm_bad", databaseName);
    olog_debug(db_err, "command[%s]", command);
    system(command);

    memset(command, 0, sizeof(command));
    sprintf(command, "rm  %s-wal_bad", databaseName);
    olog_debug(db_err, "command[%s]", command);
    system(command);
#endif
    //move the tmp db
    //memset(command, 0, sizeof(command));
    //sprintf(command, "mv  %s* /data/app/dbCenter", databaseSql);
    //olog_debug(db_err, "command[%s]", command);
    //system(command);

    sqlite3_close_v2(db);

    return 0;
}

/**
*********************************************************************
* @brief： 检查数据库是否损坏
* @param[in]：
               path                 数据库路径
               dbname               数据库名
               isBackup             是否有备份
* @param[out]：
               无
* @return： 返回值说明   S_OK    成功
                       S_NG   失败
*********************************************************************
*/
sint32 check_database(char* path, char* dbname,int isBackup)
{

    olog_debug(db_err, "check_database path:[%s] dbname[%s]", path, dbname);
    if (dbname[0] == 0)
    {
        char command[2048] = {0};
        sprintf(command, "rm %s", path);
        system(command);
        olog_warn(db_err, "check_database path:[%s] dbname[%s] is null return", path, dbname);
        return S_OK;
    }

    if (check_database_error(path, dbname) == S_NG)
    {
        olog_debug(db_recovery, "recovery db[%s]", dbname);
        sint32 ret = recovery_database(path,dbname,isBackup);
        if (ret == S_NG) {
            olog_debug(db_recovery, "recovery db[%s] failed", dbname);
            return S_NG;
        }
    }

    //when the db bad file is OK, delete the bad db file
    char file[1024];
    sprintf(file, "%s_bad", path);
    if (access(file, 0) != -1)
    {
        char command[2048] = {0};
        sprintf(command, "rm %s", file);
        system(command);
        olog_debug(db_err, "command[%s]", command);
    }

    return S_OK;
}

/**
*********************************************************************
* @brief： 检查以及恢复数据库
* @param[in]：
               path                 数据库路径
               isBackup             是否备份
* @param[out]：
               无
* @return： 返回值说明   S_OK    成功
                       S_NG   失败
*********************************************************************
*/
sint32 check_and_recovery_database(char* path, int isBackup)
{
    DIR *dir;
    struct dirent *ptr;
    char dbName[512] = {0};
    char tableName[64] = {0};
    char badTableName[512] = {0};
    olog_debug(db_ocat, "%s, dir[%s]", __FUNCTION__, path);
    if ((dir = opendir(path)) == NULL)
    {
        olog_error(db_ocat, "Open dir[%s] error...", path);
        return S_NG;
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        //过滤current dir OR parrent dir
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0 ||
            strcmp(ptr->d_name, "backup") == 0 || strcmp(ptr->d_name, "bad") == 0 ||
            strcmp(ptr->d_name, "log") == 0)
        {
            continue;
        }

        //过滤不含关键字.db的文件
        if (strstr(ptr->d_name, "shm") || strstr(ptr->d_name, "wal") ||
            strstr(ptr->d_name, "journal") || (strstr(ptr->d_name, ".db") == NULL))
        {
            olog_debug(db_ocat, "do not check file[%s]", ptr->d_name);
            continue;
        }

        //至此待处理的有.db_bad文件以及.db文件
        olog_debug(db_ocat, "file[%s]", ptr->d_name);
        //只存在bad文件(恢复数据库过程中有断电)
        if (strstr(ptr->d_name, "bad"))
        {
            char tbl[128] = {0};
            char command[1024] = {0};
            //获取数据库名
            sscanf(ptr->d_name, "%[^_]_bad", tbl);
            char filepath[512] = {0};
            sprintf(filepath, "%s%s", path, tbl);
            //数据库是否存在
            if (access(filepath, 0) == -1)
            {
                //损坏的数据库原文件不存在
                olog_debug(db_err, "the file[%s] not exisit", tbl);
                memset(command, 0, sizeof(command));
                //备份数据库重命名
                sprintf(command, "cp %s%s-shm_bad %s%s-shm", path, ptr->d_name, path, tbl);
                system(command);
                olog_debug(db_err, "command[%s]", command);

                memset(command, 0, sizeof(command));
                sprintf(command, "cp %s%s-wal_bad %s%s-wal", path, ptr->d_name, path, tbl);
                system(command);

                memset(command, 0, sizeof(command));
                sprintf(command, "cp %s%s %s%s", path, ptr->d_name, path, tbl);
                system(command);
                olog_debug(db_err, "command[%s]", command);

                memset(dbName, 0, sizeof(dbName));
                memset(tableName, 0, sizeof(tableName));
                sprintf(dbName, "%s%s", path, tbl);
                sscanf(tbl, "%[^.].db", tableName);
                olog_debug(db_err, "path[%s], tbl[%s]", dbName, tableName);
                //数据库恢复
                check_database(dbName, tableName, isBackup); 
            }

            continue;
        }

        //bad file
        sprintf(badTableName, "%s%s_bad", path, ptr->d_name);
        ////bad文件以及对应的数据库原文件都存在(恢复数据库过程中有断电)
        if (access(badTableName, 0) != -1)
        {
            //删除对应损坏的数据库原文件
            char command[1024] = {0};
            sprintf(command, "rm -f %s%s", path, ptr->d_name);
            system(command);
            olog_debug(db_err, "command[%s]", command);

            sprintf(command, "rm -f %s%s-shm", path, ptr->d_name);
            system(command);
            olog_debug(db_err, "command[%s]", command);

            sprintf(command, "rm -f %s%s-wal", path, ptr->d_name);
            system(command);
            olog_debug(db_err, "command[%s]", command);

            //将bad文件重名命
            memset(command, 0, sizeof(command));
            sprintf(command, "cp %s-shm_bad %s%s-shm", badTableName, path, ptr->d_name);
            system(command);
            olog_debug(db_err, "command[%s]", command);

            memset(command, 0, sizeof(command));
            sprintf(command, "cp %s-wal_bad %s%s-wal", badTableName, path, ptr->d_name);
            system(command);

            memset(command, 0, sizeof(command));
            sprintf(command, "cp %s %s%s", badTableName, path, ptr->d_name);
            system(command);
            olog_debug(db_err, "command[%s]", command);

            olog_debug(db_err, "command[%s]", command);
            olog_warn(db_err, "need to recovery db[%s]", ptr->d_name);
        }

        memset(dbName, 0, sizeof(dbName));
        memset(tableName, 0, sizeof(tableName));
        sprintf(dbName, "%s%s", path, ptr->d_name);
        sscanf(ptr->d_name, "%[^.].db", tableName);
//        olog_info(db_err, "path[%s], tbl[%s]", dbName, tableName);
        //检查并恢复损坏的数据库文件
        check_database(dbName, tableName, isBackup);
    }
    closedir(dir);

    return S_OK;
}
