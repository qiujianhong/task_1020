/*
*********************************************************************
* @file    framecomm.h
* @brief： 公共内容
* @author：all
* @date：   2019.12.24
* @copyright 2019 南京新联电子股份有限公司 All rights reserved.
*********************************************************************
*/

#ifndef __FRAME_COMM_H__
#define __FRAME_COMM_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "baseCommon.h"
//#include "zlog.h"

#define ZLOG_CONFIG_PATH     "/mnt/bin/applog.conf"

#define APP_FMT_FATAL(fmt, ...)  diag_info("APP", __FILE__, __FUNCTION__, __LINE__, ZLOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#define     LOG_PACK_MAX    (10)   //允许保存的压缩包数量
#define     LOG_PATH_MAX    (255)  //维护的最大日志目录数
#define     DELAYTIME   200000  //打高分延时改成200ms 240103
#define     DELAYTIME_MORE  500000      //进一步提高延时
#define     DELAY_FILE_PATH  "/tmp/perfectscore"   //加延时的时候判断的文件
#define     DELAY_FILE_PATH2  "/tmp/perfectscoremore"   //加延时的时候判断的文件 第二阶段

extern uint32 gdelaytime;


typedef enum 
{
    DIAG_TRACE = 0, /** < 流程追踪 */
    DIAG_DEBUG = 1, /** < 调试信息 */        
    DIAG_LOGCC = 2, /** < 受控日志 */
    DIAG_EMERG = 3, /** < 紧急日志 */     
}DIAG_TYPE_E;

typedef struct 
{
    const char *module; /** < 模块名 */
    int bitCtrl;        /** < 位控码*/
    int sizeLimit;      /** < 单个日志文件大小限制*/
    int packedNum;      /** < 打包数量*/
}DIAG_INFO_T;

typedef struct 
{
    uint8 warPower;  /** < 断电告警 */
    uint8 warHalt;   /** < 关机告警 */
    uint8 bData;     /** < 强制写/data, 不需要写/tmp */
    uint8 res;
}DIAG_WARNING_T;

#define DIAG_IS_MERGE(bits) (bits&(1UL<<4))
#define DIAG_IS_TRLOG(bits) (bits&(1UL<<3))
#define DIAG_IS_LOGCC(bits) (bits&(1UL<<2))
#define DIAG_IS_DEBUG(bits) (bits&(1UL<<1))
#define DIAG_IS_TRACE(bits) (bits&(1UL<<0))

//统计模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define STAT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 0, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define STAT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 0, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define STAT_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 0, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define STAT_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 0, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define STAT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 0, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define STAT_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 0, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//上报模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define RPT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__,  1, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define RPT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__,  1, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define RPT_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__,  1, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define RPT_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define RPT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define RPT_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//规约解析模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define PRTL_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 2, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define PRTL_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 2, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define PRTL_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 2, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define PRTL_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 2, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define PRTL_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 2, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define PRTL_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 2, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//终端事件模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define EVENT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__,  3, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define EVENT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__,  3, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define EVENT_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__,  3, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)

#define EVENT_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 3, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define EVENT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 3, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define EVENT_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 3, NULL, 2, buf, len, fmt, ##__VA_ARGS__)


//通信模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define COMM_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 4, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define COMM_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 4, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define COMM_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 4, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define COMM_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 4, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define COMM_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 4, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define COMM_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 4, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//遥信脉冲模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define PULSE_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 5, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define PULSE_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 5, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define PULSE_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 5, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define PULSE_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 5, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define PULSE_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 5, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define PULSE_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 5, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//698调度入口模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define APP698_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 6, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define APP698_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 6, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define APP698_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 6, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define APP698_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 6, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define APP698_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 6, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define APP698_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 6, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//用采MSG模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define YCDB_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 7, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define YCMSG_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 8, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define YCMSG_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 8, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define YCMSG_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 8, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define YCMSG_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 8, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define YCMSG_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 8, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define YCMSG_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 8, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//显示APP
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define GUI_FMT_TRACE(fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 9, NULL, DIAG_TRACE, NULL, 0, fmt, ##__VA_ARGS__)
#define GUI_FMT_DEBUG(fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 9, NULL, DIAG_DEBUG, NULL, 0, fmt, ##__VA_ARGS__)
#define GUI_FMT_LOGCC(fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 9, NULL, DIAG_LOGCC, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define GUI_BUF_TRACE(buf,len,fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 9, NULL, DIAG_TRACE, buf, len, fmt, ##__VA_ARGS__)
#define GUI_BUF_DEBUG(buf,len,fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 9, NULL, DIAG_DEBUG, buf, len, fmt, ##__VA_ARGS__)
#define GUI_BUF_LOGCC(buf,len,fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 9, NULL, DIAG_LOGCC, buf, len, fmt, ##__VA_ARGS__)

//配网MSG模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define PDDB_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 10, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define PDMSG_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 11, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define PDMSG_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 11, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define PDMSG_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 11, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define PDMSG_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 11, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define PDMSG_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 11, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define PDMSG_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 11, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//104调度入口模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define APP104_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 12, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define APP104_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 12, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define APP104_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 12, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define APP104_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 12, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define APP104_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 12, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define APP104_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 12, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//交采升级
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define UPDATE_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 13, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define UPDATE_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 13, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define UPDATE_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 13, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define UPDATE_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 13, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define UPDATE_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 13, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define UPDATE_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 13, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//交采模块串口通信
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define ACCOMM_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 14, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define ACCOMM_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 14, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define ACCOMM_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 14, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define ACCOMM_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 14, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define ACCOMM_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 14, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define ACCOMM_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 14, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//交采模块数据中心通信
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define ACDB_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 15, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define ACDB_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 15, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define ACDB_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 15, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define ACDB_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 15, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define ACDB_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 15, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define ACDB_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 15, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//ESAM打印
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define TESAM_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 16, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define TESAM_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 16, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define TESAM_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 16, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define TESAM_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 16, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define TESAM_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 16, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define TESAM_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 16, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//配网104与主站之间的交互
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define FRAME104_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 17, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define FRAME104_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 17, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define FRAME104_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 17, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define FRAME104_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 17, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define FRAME104_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 17, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define FRAME104_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 17, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//配网104与数据中心之间的交互
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define DB104_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 18, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define DB104_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 18, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define DB104_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 18, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define DB104_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 18, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define DB104_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 18, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define DB104_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 18, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//控制模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define CTRL_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 21, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define CTRL_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 21, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define CTRL_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 21, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define CTRL_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 21, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define CTRL_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 21, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define CTRL_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 21, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//交采UDP服务
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define ACUDP_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 22, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define ACUDP_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 22, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define ACUDP_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 22, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define ACUDP_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 22, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define ACUDP_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 22, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define ACUDP_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 22, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//交采MQTT通信
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define ACMQTT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 23, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define ACMQTT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 23, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define ACMQTT_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 23, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define ACMQTT_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 23, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define ACMQTT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 23, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define ACMQTT_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 23, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//遥信脉冲模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define RSP_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 25, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define RSP_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 25, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define RSP_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 25, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define RSP_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 25, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define RSP_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 25, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define RSP_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 25, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//模组管理器
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define MDLMSG_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 29, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define MDLMSG_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 29, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define MDLMSG_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 29, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define MDLMSG_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 29, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define MDLMSG_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 29, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define MDLMSG_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 29, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define MDLUP_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 30, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define MDLUP_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 30, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define MDLUP_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 30, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define MDLUP_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 30, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define MDLUP_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 30, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define MDLUP_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 30, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define MQTTMSG_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 31, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define MQTTMSG_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 31, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define MQTTMSG_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 31, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define MQTTMSG_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 31, NULL, 1, buf, len, fmt, ##__VA_ARGS__)

//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define WIRELESS_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 32, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define WIRELESS_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 32, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define WIRELESS_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 32, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define WIRELESS_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 32, NULL, 1, buf, len, fmt, ##__VA_ARGS__)

//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define LCMONIT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 34, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define LCMONIT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 34, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define LCMONIT_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 34, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define LCMONIT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 34, NULL, 1, buf, len, fmt, ##__VA_ARGS__)

//交采对时服务
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define TMCHK_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 35, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define TMCHK_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 35, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define TMCHK_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 35, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define TMCHK_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 35, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define TMCHK_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 35, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define TMCHK_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 35, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//高级APP交采UDP服务
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define MSGAC_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 36, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define MSGAC_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 36, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define MSGAC_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 36, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define MSGAC_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 36, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define MSGAC_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 36, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define MSGAC_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 36, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//SELF检测
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define SELFCHK_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 37, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define SELFCHK_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 37, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define SELFCHK_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 37, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define SELFCHK_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 37, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define SELFCHK_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 37, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define SELFCHK_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 37, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

// 台区状态管理
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define PDS_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 38, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define PDS_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 38, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define PDS_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 38, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define PDS_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 38, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define PDS_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 38, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define PDS_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 38, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

// 台区状态管理
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define PDSDB_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 39, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define PDSDB_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 39, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define PDSDB_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 39, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define PDSDB_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 39, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define PDSDB_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 39, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define PDSDB_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 39, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

// 安全在线监测
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define AUTH_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 40, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define AUTH_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 40, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define AUTH_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 40, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define AUTH_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 40, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define AUTH_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 40, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define AUTH_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 40, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//698交采模块相关服务
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define ACMSG_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 41, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define ACMSG_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 41, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define ACMSG_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 41, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define ACMSG_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 41, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define ACMSG_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 41, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define ACMSG_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 41, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//存储模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define STORE_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 42, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define STORE_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 42, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define STORE_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 42, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define STORE_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 42, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define STORE_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 42, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define STORE_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 42, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//初始化模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define INIT_FMT_TRACE(fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 43, NULL, DIAG_TRACE, NULL, 0, fmt, ##__VA_ARGS__)
#define INIT_FMT_DEBUG(fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 43, NULL, DIAG_DEBUG, NULL, 0, fmt, ##__VA_ARGS__)
#define INIT_FMT_LOGCC(fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 43, NULL, DIAG_LOGCC, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define INIT_BUF_TRACE(buf,len,fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 43, NULL, DIAG_TRACE, buf, len, fmt, ##__VA_ARGS__)
#define INIT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 43, NULL, DIAG_DEBUG, buf, len, fmt, ##__VA_ARGS__)
#define INIT_BUF_LOGCC(buf,len,fmt,...)  diag_info_print((char *)&__FILE__,__LINE__,__FUNCTION__, 43, NULL, DIAG_LOGCC, buf, len, fmt, ##__VA_ARGS__)

//交采采集数据日志接口
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define ACSAMP_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 44, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define ACSAMP_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 44, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define ACSAMP_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 44, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define ACSAMP_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 44, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define ACSAMP_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 44, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define ACSAMP_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 44, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//系统管理器日志接口
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define SMIOS_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 45, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define SMIOS_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 45, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define SMIOS_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 45, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define SMIOS_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 45, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define SMIOS_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 45, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define SMIOS_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 45, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//系统管理器日志接口
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define LOGIN_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 46, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define LOGIN_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 46, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define LOGIN_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 46, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define LOGIN_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 46, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define LOGIN_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 46, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define LOGIN_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 46, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//系统管理器日志接口
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define HEART_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 47, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define HEART_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 47, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define HEART_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 47, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define HEART_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 47, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define HEART_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 47, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define HEART_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 47, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//安全软算法模块日志接口
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define SM_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 48, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define SM_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 48, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define SM_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 48, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define SM_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 48, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define SM_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 48, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define SM_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 48, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//蓝牙
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define BLE_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 49, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define BLE_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 49, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define BLE_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 49, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define BLE_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 49, NULL, 1, buf, len, fmt, ##__VA_ARGS__)

//USB
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define USB_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 51, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define USB_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 51, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define USB_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 51, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define USB_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 51, NULL, 1, buf, len, fmt, ##__VA_ARGS__)

//安全
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define SEC_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 52, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define SEC_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 52, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define SEC_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 52, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define SEC_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 52, NULL, 1, buf, len, fmt, ##__VA_ARGS__)

//IOT
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
//#define MQTT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 51, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
//#define MQTT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 51, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define IOT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 53, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define IOT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 53, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)

#define IOT_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 53, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define IOT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 53, NULL, 1, buf, len, fmt, ##__VA_ARGS__)

//统计冻结的日志打印
#define FREEZE_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 55, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define FREEZE_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 55, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define FREEZE_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 55, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define FREEZE_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 55, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define FREEZE_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 55, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define FREEZE_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 55, NULL, 2, buf, len, fmt, ##__VA_ARGS__)

//电台
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define RADIO_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 56, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define RADIO_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 56, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define RADIO_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 56, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define RADIO_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 56, NULL, 1, buf, len, fmt, ##__VA_ARGS__)

#define    CHAR_TYPE_NONE             "\033[0m"
#define    COLOR_FONT_RED             "\033[31m"
#define    COLOR_FONT_GREEN           "\033[32m"
#define    COLOR_FONT_YELLOW          "\033[33m"
#define    UNDER_LINE                 "\033[4m"
#define    HIGH_LIGHT                 "\033[1m"

/* 实时时钟数据结构 */
typedef struct
{   
    char    ss; /* 0~59 BCD */
    char    nn; /* 0~59 BCD */
    char    hh; /* 0~23 BCD */
    char    dd; /* 1~31 BCD */
    char    mm; /* 1~12 BCD */
    char    yy; /* 00~99 BCD */
    char    ww; /* 0~6 opposite Sun~Sat */  
}rtc_t;

typedef union tag_PASS_TIME
{
    struct 
    {
        uint8   Second              :1;             //过秒标志
        uint8   Minute              :1;             //过分标志
        uint8   Hour                :1;             //过时标志
        uint8   Day                 :1;             //过日标志
        uint8   Month               :1;             //过月标志
        uint8   ReserveBit6         :1;             //保留
        uint8   ReserveBit7         :1;             //保留
        uint8   ReserveBit8         :1;             //保留
    };

    uint8               Word;
}FRM_PASS_TIME_U; 

#define COMMFUN_ABS(a,b)                    ((a)>(b)?(a)-(b):(b)-(a))

#define PATH_MAX_LEN  256  //定义路径最大长度

#define LOG_MAX_SIZE  (2097152)  /*日志大小 字节数2M*/
#define LOG_SAVE_NUM  (10)        /*日志备份文件保存个数*/

typedef enum       
{
  TIME_REALTIME = 0,
  TIME_MONOTONIC,
}TIME_TYPE_E;

int rcd_lock(int fd, int cmd, int type, off_t offset, int whence, off_t len);

#define WRITEW_LOCK(fd, offset, len) \
            rcd_lock(fd, F_SETLKW, F_WRLCK, offset, SEEK_SET, len)

#define UNLOCK(fd, offset, len) \
            rcd_lock(fd, F_SETLK, F_UNLCK, offset, SEEK_SET, len)

typedef int (*cmpfun)(void *vl, void *vr);

void arrcpy(void *dstArray, int dstStep, void *srcArray, int srcStep, int count);
void *memcpy_rev(void *dest, const void *src, int count);
void *memcpy_r(void *dest, const void *src, int count);
void *addrcpy(void *dest, const void *src, int count, BOOLEAN isRev);
void memrev(uint8* buff,uint16 len);
void membitrev(uint8* buff, uint16 len);
void memcpy_bitrev(uint8 *dest, const uint8 *src, int count);

void arrsingle(void *array, int *num, int size, cmpfun arrcmp);
void arrdsingle(void *array1, int *num1, void *array2, int *num2, int size, cmpfun arrcmp);

void diag_info_print(char *file, int line, const char *func, int modid, char *subfix, DIAG_TYPE_E diagtype, const void *buf, uint32 len, const char *fmt, ...);

int diag_init(char* conf, uint8 bNoTmp);
void diag_info(char *cname, const char *file, const char *func,long line, int level, const char *format, ...);

BOOLEAN oop_tsa_equal(OOP_TSA_T *pstTSA1, OOP_TSA_T *pstTSA2);

BOOLEAN singleton_check(char *appName);
void sleep_no_interrupt(unsigned int seconds);
int sleep_s(unsigned int s);
int msleep_s(unsigned int ms);
int usleep_s(unsigned int us);

void set_bit_value(uint8 *BS,uint16 bitStringlen,uint16 bitpos);
void  clr_bit_value(uint8 *BS,uint16 bitStringlen,uint16 bitpos);
uint8 get_bit_value(uint8 *BS,uint16 bitStringlen,uint16 bitpos);

char commfun_HexToBcd(int hex);
int commfun_BcdToHex(char compress_bcd);
time_t commfun_rtc2time2(rtc_t src_rtc);

BOOL DT_IsLeap(uint16 year);
BOOL DT_DateTimeShortIsValid(OOP_DATETIME_S_T *DateTime);
BOOL DT_DateTimeShort2Time(OOP_DATETIME_S_T *dateTime,time_t *linuxTim);
BOOL DT_Time2DateTimeShort(time_t linuxTime, OOP_DATETIME_S_T *dateTime);
BOOL DT_DateTimeHexIsValid(DateTimeHex_t *DateTime);
BOOL DT_DateTimeHex2Time(DateTimeHex_t *dateTimeHex,time_t *linuxTim);
BOOL DT_Time2DateTimeHex(time_t linuxTime,DateTimeHex_t *dateTimeHex);
BOOL DT_DateTimeBCDIsValid(DateTimeBcd_t *DateTime);
BOOL DT_DateTimeBCD2Time(DateTimeBcd_t *dateTimeBCD,time_t *linuxTime);
BOOL DT_Time2DateTimeBCD(time_t linuxTime,DateTimeBcd_t *dateTimeBCD);
BOOL DT_DateTimeBCD2Hex(DateTimeBcd_t *dateTimeBCD,DateTimeHex_t *dateTimeHex);
BOOL DT_DateTimeBCD2OOPS(DateTimeBcd_t *dateTimeBCD,OOP_DATETIME_S_T *oopDateTime);
int DT_String2Struct(char *timestr, OOP_DATETIME_S_T *pTime);

long DT_TI2Seconds(OOP_TI_T ti);
void DT_TimeAddInv(time_t *time,OOP_TI_T ti);
void DT_TimeSubInv(time_t *time,OOP_TI_T ti);
void DT_Time2RTC(time_t time,rtc_t *prtc);
uint32 DT_RTC_Interval(rtc_t *ptrF, rtc_t *ptrL);

uint16 Int16_ABS(uint16 value);

uint32  BCD_change_to_long(uint8 *src,uint8 n);

int commfun_ASC2BCD(char *bcd, const char *asc, int len, int fmt);
int commfun_BCD2ASC(char *asc, char *bcd, int len, int fmt);
void commfun_IPToStr(int nIp, char* pVal);
BOOL commfun_IpToInt(char* pczIp, int* pnIp);
void commfun_IPToHexStr(int nIP, char* pVal);
void commfun_HexIPToInt(uint8 *pczIp, int *pnIp);
void commfun_toupper(char* pStr);
void commfun_tolower(char* pStr);

//库函数的安全函数
int memcpy_s(void * __restrict dest, size_t destsz, const void * __restrict src, size_t count);
int memmove_s(void * dest, size_t destsz, const void * src, size_t count);
int memset_s(void * dest, size_t destsz, int ch, size_t count);
size_t strnlen_s(const char * str, size_t strsz);
int strcpy_s(char * __restrict dest, size_t destsz, const char * __restrict src);
int strncpy_s(char * __restrict dest, size_t destsz, const char * __restrict src, size_t count);
int strcat_s(char * __restrict dest, size_t destsz, const char * __restrict src);
//自定义函数的安全函数
int memcpy_rs(void * __restrict dest, size_t destsz, const void * __restrict src, size_t count);

BOOLEAN file_exist(const char *filepath);
int file_path_extract(const char *src, char *path);
BOOLEAN path_exist(const char *pathname);
BOOLEAN path_dir_make(const char *path);
int file_name_extract(const char *src, BOOLEAN withSubfix, char *filename);
int file_extension_extract(const char *src, char *ext);
ssize_t file_size_get(const char *filename);

void str2lower(char * s);
int strt2octet(char *str, int strLen, unsigned char *octet, int octetLen);
void split(char *src, const char *delim, char **dest, int *num);
void str2upper(char * s);
int octet2str(unsigned char *octet, int octetLen, char *str, int strLen);
int strformat(char *fmtstr, int fmtlen, char *str, int strLen);

void FastSecondToDate(struct timeval* tv, struct timezone* tz, struct tm* tm);
void LOCALTIME_R(time_t* times, struct tm* tm);
time_t FastDateToSecond(struct tm* tm);
uint32 Get_Interface_IP(char *ifname);
void diag_sig_handler(int signo);
time_t get_time(TIME_TYPE_E type);
uint8 checksum_calc(uint8 *SrcData, int DataLen);
void check_endian(void);
void long_change_to_BCD(uint8 *dec,uint8 n,uint32 src);
void longlong_change_to_BCD(uint8 *dec,uint8 n,uint64 src);
uint32 BCD_change_to_long(uint8 *src,uint8 n);
    BOOL app_indocker_check();
void hexchangetodec(uint16 hex,uint32 *dec);
int wildcard_remove(char *filename);

#ifdef __cplusplus
}
#endif

#endif /*__FRAME_COMM_H__*/

