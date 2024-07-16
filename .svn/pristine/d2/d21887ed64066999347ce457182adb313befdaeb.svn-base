/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 头文件
* @date： 2019-11-20
*********************************************************************
*/
#ifndef __SYSTEM_LOG_H__
#define __SYSTEM_LOG_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "msgHelper.h"
/*==============日志记录关联================*/
#ifdef SYSTEM_DEBUG
#define INFOPATH "path info"
#define WARNPATH "path warn"
#define ERRORPATH "path error"
#define DEBUGPATH "path debug"
#define writelog(filepath, format, ...)                                        \
  printf("%s." format "\n", filepath, ##__VA_ARGS__)
#else
// TODO log记录接口
#define INFOPATH "path info"
#define WARNPATH "path warn"
#define ERRORPATH "path error"
#define DEBUGPATH "path debug"
#define writelog(filepath, format, ...)                                        \
  printf("%s." format "\n", filepath, ##__VA_ARGS__)
#endif

/* @brief: 物联网平台消息记录接口 */
#define sys_iotmsg_record(format, ...) writelog(INFOPATH, "[SYS][IOTRecord]"format, ##__VA_ARGS__)
/* @brief:  告警消息记录接口 */
#define sys_warningmsg_record(format, ...)                                     \
  writelog(WARNPATH, "[SYS][WARN]"format, ##__VA_ARGS__)
/* @brief:  错误消息记录接口 */
#define sys_errormsg_record(format, ...)                                       \
  writelog(ERRORPATH, "[SYS][ERROR]"format, ##__VA_ARGS__)
/* @brief:  调试消息记录接口*/
#define sys_debugmsg_record(format, ...)                                       \
  writelog(DEBUGPATH, "[SYS][DEBUG]"format, ##__VA_ARGS__)

/* @brief:  消息记录接口 */
sint32 sys_recordmsg_tolog(MSG_OBJ_T *);
#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_LOG_H__ */