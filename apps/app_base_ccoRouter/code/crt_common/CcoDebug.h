/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 运行日志和调试信息打印
* @date： 2019-10-25
*********************************************************************
*/

#ifndef __CCO_DEBUG_H__
#define __CCO_DEBUG_H__

#include "CcoAll.h"
#include "CcoRedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

#define DEBUG_CCO
#ifdef DEBUG_CCO
#include <errno.h>
extern int errno;
#define printx(x...) ({if (getenv("DEBUG_CCO") != NULL)  printf(x);})
#else
#define printx(x...) 
#endif

/*******************************************************************************
* 调节日志路径
*******************************************************************************/

//main函数和上行线程日志文件
extern char *gUpRecordPath;   //main函数和上行线程日志文件

//调节日志路径
extern char *gDownRecordPath[MAX_ALLOWED_DOWN_THREAD_NUM];  //调节日志路径

//显示GUI线程日志文件
extern char *gGuiRecordPath;   //显示GUI线程日志文件

//本地通信app与消息中心的交互
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define CCOUP_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 19, gUpRecordPath, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define CCOUP_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 19, gUpRecordPath, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define CCOUP_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 19, gUpRecordPath, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define CCOUP_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 19, gUpRecordPath, 0, buf, len, fmt, ##__VA_ARGS__)
#define CCOUP_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 19, gUpRecordPath, 1, buf, len, fmt, ##__VA_ARGS__)
#define CCOUP_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 19, gUpRecordPath, 2, buf, len, fmt, ##__VA_ARGS__)

//本地通信app与载波模块的交互
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define CCODOWN_FMT_TRACE(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 20, gDownRecordPath[pipe], 0, NULL, 0, fmt, ##__VA_ARGS__)
#define CCODOWN_FMT_DEBUG(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 20, gDownRecordPath[pipe], 1, NULL, 0, fmt, ##__VA_ARGS__)
#define CCODOWN_FMT_LOGCC(pipe,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 20, gDownRecordPath[pipe], 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define CCODOWN_BUF_TRACE(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 20, gDownRecordPath[pipe], 0, buf, len, fmt, ##__VA_ARGS__)
#define CCODOWN_BUF_DEBUG(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 20, gDownRecordPath[pipe], 1, buf, len, fmt, ##__VA_ARGS__)
#define CCODOWN_BUF_LOGCC(pipe,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 20, gDownRecordPath[pipe], 2, buf, len, fmt, ##__VA_ARGS__)

//本地通信app显示GUI线程日志
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define CCOGUI_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 24, gGuiRecordPath, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define CCOGUI_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 24, gGuiRecordPath, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define CCOGUI_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 24, gGuiRecordPath, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define CCOGUI_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 24, gGuiRecordPath, 0, buf, len, fmt, ##__VA_ARGS__)
#define CCOGUI_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 24, gGuiRecordPath, 1, buf, len, fmt, ##__VA_ARGS__)
#define CCOGUI_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 24, gGuiRecordPath, 2, buf, len, fmt, ##__VA_ARGS__)

/*******************************************************************************
 * 函数申明
*******************************************************************************/

extern void ComposeBuf(char *buf, int len, char *comm_log);
extern void PrintBuf(uint8 *buf, int len);

extern void PrintTaskDetail(PLCTASK_DETAIL PlcTaskDetail, uint8 nArrIndex);
extern void PrintFilesDetail(AMR_METER_FILES_INFO_T AmrMeterFilesInfo, uint8 nArrIndex);
extern void PrintAmrActiveDetail(AMR_MODE_ACTIVE_T AmrModeActive);
extern void PrintFilesList(AMR_METER_FILES_LIST_T AmrMeterFilesList, uint8 nArrIndex);
extern void PrintInfoDetail(MSG_INFO_T  *stMsgInfoSend);
extern void PrintSearchMeterDetail(AMR_METER_START_SEARCH_T       AmrMeterStartSearch);
extern void PrintTaskList(PLCTASK_QUEUE lstPlcTaskQueue, uint8 nArrIndex);

extern void CharArrToString(char *buf, int len, int maxlen, char *comm_log);
extern void PrintLogInfoDetail(MSG_INFO_T *stMsgInfoSend);
extern void PrintLogBuffDetail(uint8 *buf, int len, uint8 nArrIndex);
extern void PrintCurrentTime(DateTime_T *dtbcd);

#ifdef __cplusplus
}
#endif

#endif // __CCO_DEBUG_H__
