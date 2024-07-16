/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控公共数据
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __USB_INIT_H
#define __USB_INIT_H
    
#ifdef __cplusplus
        extern "C" {
#endif
#include "iot_mqtt.h"
#define IOT_APP_NAME_M     "M-iotManager"
#define IOT_APP_NAME     "iotManager"
#define IOT_INITFILE     "iotManager.ini"
#define IOT_DATAFILE     "iotManager.json"
#define IOT_DEFAULT_TOPIC "M-iotManager/M-smiOS"

#define IOT_SUBTOPIC1      "+/M-iotManager/#"
#define IOT_SUBTOPIC2      "+/iotManager/#"

//规约解析模块
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define IOTINIT_FMT_TRACE(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define IOTINIT_FMT_DEBUG(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 1, NULL, 0, fmt, ##__VA_ARGS__)
#define IOTINIT_FMT_LOGCC(fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 2, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define IOTINIT_BUF_TRACE(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 0, buf, len, fmt, ##__VA_ARGS__)
#define IOTINIT_BUF_DEBUG(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 1, buf, len, fmt, ##__VA_ARGS__)
#define IOTINIT_BUF_LOGCC(buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 1, NULL, 2, buf, len, fmt, ##__VA_ARGS__)
#define    APPMSG_NAME_LEN 128
#define IOT_APPID_MAX  32
typedef struct
{
    uint16 updateTime;
    uint16 monitorTime;
    uint16 devCpuLimit;
    uint16 devMemLimit;
    uint16 devDiskLimit;
    uint16 devCpuTempup;
    uint16 devCpuTempdown;
}app_conf;

typedef struct
{
    uint8 type_u;
    uint8 result;
    SEQ_STR  appname;
    SEQ_STR  conname; 
}app_update;

typedef struct 
{
    uint32      mqttip;       //mqtt地址
    uint16      mqttport;     //mqtt端口
    uint8       appidlen;
    uint8       appid[IOT_APPID_MAX];
    time_t      msgtime;  //消息收到的时间
    uint8       init_type;   //初始化标记
    app_conf    app_t;
    app_update  app_u;
}IOT_APPDATA_T;


typedef struct
{
    uint16 cpulimit;
    uint16 memLimit;
    uint16 diskLimit;
    uint16 cputemp_up;
    uint16 cputemp_down;
    uint16 sys_monitortime;
}SYS_CONF;

#ifdef __cplusplus
}
#endif

#endif


