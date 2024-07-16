/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 数据库底层驱动程序头文件
* @date： 2019-11-9
*********************************************************************
*/

#ifndef _DB_MODULE_H_
#define _DB_MODULE_H_
#include "baseType.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* @brief:数据库初始化  */
sint32 db_init(void);
/* @brief: 数据库模块启动 */
sint32 db_start(void);
/* @brief: 数据库停止 */
void db_stop(void);
sint32 db_data_init(uint8 logi_dev );
sint32 db_spec_data_(uint8 logi_dev , sint16 inf_addr_no, sint32 prim_di, sint32 sub_di);
/* @brief:  恢复出厂设置 */
sint32 db_recovery_factory(uint8 logi_dev);

#ifdef __cplusplus
}
#endif

#endif /* _DB_CDRIVER_H_ */
