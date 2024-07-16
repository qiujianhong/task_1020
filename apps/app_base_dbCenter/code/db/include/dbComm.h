/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： dbComm.h
           本头文件包括数据库操作的一些共用参数结构体
* @date： 2019-11-9
*********************************************************************
*/

#ifndef _DB_COMMON_H_
#define _DB_COMMON_H_

#include "dbRaw.h"
//#include "dbModule.h"
#include "qlibc.h"
#include "baseType.h"
//#include "dbDataInterface.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DB_CENTER_DEBUG 1

//success
#define   S_OK  0
//fail
#define   S_NG  1

#define SECTION_MAX_LEN 256
#define STRVALUE_MAX_LEN 256
#define LINE_CONTENT_MAX_LEN 256
#define READ_STR_OK  0x0000
#define READ_STR_ERR 0x0001

#define STEP_DATA_SIZE 150          //记录型数据的单条数据在磁盘中占用空间（估算，10000条1M）


#define SPACE_RECLAMATION_SWITCH    (1)
#define DB_OPEN_HANDLE_MAX_COUNT    (20)  //打开的数据库句柄最大数量

#define DB_CENT_TABLE_NAME_PARAMETER      "dbParameterCenter"
#define TPARAMER_DB_NAME                  "dbParameterCenter.db"
#define DB_CENT_TABLE_NAME_NORMAL         "dbGeneralCenter"
#define TGENERAL_DB_NAME                  "dbGeneralCenter.db"

#define TGENERAL_DELAY_DB_NAME            "dbGeneralDelayCenter.db"
#define TGENERAL_DELAY_NAME               "dbGeneralDelayCenter"
#define TMEMORY_DB_NAME                   "TMemory.db"
#define TMEMORY_NAME                      "TMemory"

/* @brief:数据表类型枚举  */
enum ENUM_TABLE_TYPE_E
{
    ENUM_NORMAL_TABLE = 0,
    ENUM_PARAM_TABLE,
    ENUM_BACKUP_TABLE, 
    ENUM_MAX_TABLE_NUM
};

enum ENUM_DATA_TYPE
{
   ENUM_NOT_INIT_PARAM = 1,
   ENUM_INIT_PARAM     = 2,
   ENUM_NORMAL_DATA    = 3,
   ENUM_EVENT_DATA     = 4,
   ENUM_COLLECT_DATA   = 5,
   ENUM_NOT_INIT_DATA  = 6,
   ENUM_INVALID
};

enum ENUM_TABLE_STATUS
{
   ENUM_TABLE_CLOSE  = 0,
   ENUM_TABLE_OPEN   = 1,
   ENUM_TABLE_TEMP   = 2,
   ENUM_TABLE_INVLIAD
};

/* @brief:普通型数据表信息  */
typedef struct __db_tab_normal_info
{
   DB_RAW_T *handle;
   //  void* backup_handle;
   sint32 is_backup; /*0:no backup; 1:backup */
   sint32 realtime;  /*0:no need; 1: realtime; 2 write to memory*/
   sint8 tab_name[30];  
} DB_TAB_NORMAL_INFO_T;

/* @brief:普通配置数据配置  */
typedef struct __db_tab_normal_cfg_info
{
   void* handle;
   sint32 is_backup; /*0:no backup; 1:backup */
   sint32 realtime;
   sint16 tab_type;  
} DB_TAB_NORMAL_CFG_INFO_T;

/* @brief:记录型数据表信息  */
typedef struct __db_tab_record_info
{
   DB_RAW_T* handle;
   sint8 tab_name[30];
   sint8 tab_status;
   sint32 refer_count;
   struct timespec tv;
} DB_TAB_RECORD_INFO_T;
sint32 check_table_isexist(DB_TAB_RECORD_INFO_T* data,sint8 *pTableName, uint8* pExist);


/* @brief:获取普通、参数型数据表信息  */

sint32 get_normal_tab_info(uint32 data_tag, sint16 dataType, DB_TAB_NORMAL_INFO_T *tab_info);
	
/* @brief:获取记录型数据表信息  */
sint32 get_collect_tab_info(uint8 device_no, uint16 infoAddrNo, uint32 prim_di, uint32 sub_di, DB_TAB_RECORD_INFO_T*  tab_info, sint8 flag);
sint32 put_collect_tab_info(uint8 device_no, uint16 infoAddrNo, uint32 prim_di, uint32 sub_di);
sint32 load_normal_data_cfg(qlisttbl_t *tbl);
sint32 get_handle_open_count(void);
sint32 check_need_to_open_db(void);

/* @brief:数据库获取参数句柄  */
DB_RAW_T *get_Tparameter(void);

/* @brief:数据库获取通用句柄  */
DB_RAW_T *get_Tgeneral(void);

void free_hash_tbl(void);

/* @brief: 普通型数据库初始化 */
sint32 db_normaldatadb_init(void);
/* @brief: 参数型数据库初始化 */
sint32 db_paramdatadb_init(void);
/* @brief: 创建记录型数据库 */
sint32 create_collect_db(sint8* tab_name);
sint32 create_normal_db(const sint8 *dbName, const sint8 *dbPath, sint32 sqlType, DB_RAW_T **db);

/* @brief: 指定数据初始化 */
sint32 spec_data_del(uint8 logi_dev, uint16 inf_addr_no, uint16 data_type, uint32 prim_di, uint32 sub_di, bool needSpaceReclaim);

/* @brief: 通过主DI获取所有数据对象 */
sint32 get_all_table_by_pridatadi(uint32 pridatadi, qlist_t *list);
/* @brief: 数据初始化*/
sint32 data_init_logi_dev(uint8 logi_dev, uint8 optinal, uint16 dataTypeID, uint8 isFactory);
sint32 record_data_init(uint8 logi_dev);
/* @brief: 遍历删除 */
sint32 loop_dele_records(void);
/* @brief: 根据主DI，次DI 获取数据深度 */
uint32 get_depth_value(sint32 pri_di, sint32 sub_di);
/* @brief: 载入数据深度信息 */
sint32 load_tab_depth_cfg(qlisttbl_t *tbl);
sint32 load_tab_depth_Info(qlisttbl_t *tbl);

sint32 gen_param_data_init_logi_dev(uint8 logi_dev, sint8 flag, uint8 dataTypeIDOptioal, uint16 dataTypeID, bool needSpaceReclaim);

/* @brief: 数据/参数初始化*/
sint32 gen_param_data_init_logi_dev_info_number(uint8 logi_dev, uint16 info_number, sint8 flag, uint8 dataTypeIDOptioal, uint16 dataTypeID, bool needSpaceReclaim);

/* @brief: 操作读记录型数据 */
#ifdef AREA_FUJIAN
sint32 m_read_record_data_fj(uint8 logicalDeviceNum, uint16 infoNum, uint32 majorDI, char *sql, uint32 *nRows, qlist_t *ptable);
#endif
sint32 m_read_record_data(uint8 logicalDeviceNum, uint16 infoNum, uint32 majorDI, char *sql, uint32 *nRows, qlist_t *ptable);

/* @brief: 读普通型数据 */
sint32 m_read_all_normal_data(uint8 logicalDeviceNum,
                              uint16 infoNum,
                              uint32 majorDI,
                              uint32 minorDINum,
                              uint32 *minorDIs,
                              char *sql,
                              uint32 *nRows,
                              qlist_t *ptable);

/* @brief: 根据磁盘空间触发删除函数 */
sint32 dele_db_disk_space(void);
/* @brief: 关闭数据库 */
sint32 close_db_handle(uint8 logi_dev);

/* db recovery */
//sint32 check_database_error(sqlite3 **handle, char* filename);
/* @brief: 数据库是否有损坏*/
sint32 check_database_error(char* path, char* filename);
/* @brief: 恢复数据库*/
sint32 recovery_database(char* path, char* dbname, int isBackup);
/* @brief: 检查并恢复数据库 */
sint32 check_database(char* path, char* dbname, int isBackup);
/* @brief: 数据库检查 */
sint32 check_and_recovery_database(char* path, int isBackup);
/* @brief: 根据主DI获取记录深度 */
uint32 get_depth_value_pridi(uint32 pri_di);
/* @brief: 数据初始化带参数 */
sint32 data_init_logi_dev_info_number(uint8 logi_dev, uint16 inf_no, uint16 dataTypeID, bool needSpaceReclaim);
/* @brief: 普通型数据库deinit*/
sint32 db_normaldatadb_de_init(void);
/* @brief: 记录型数据库deinit*/
sint32 db_record_deinit(void);
/* @brief: 数据库删除服务线程*/
int start_db_del_servie(void);
/* @brief: 并发写入时的预解析执行*/
sint32 step_db(sqlite3_stmt *pStmt);

#ifdef __cplusplus
}
#endif

#endif /* _DB_COMMON_H_ */
