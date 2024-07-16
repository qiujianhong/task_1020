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
#ifndef	__SEC_COMM_H
#define __SEC_COMM_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define  APP_RECV_BUF_LEN      8192	/*recvfrom接收缓冲区大小*/

#define SEC_APP_NAME     "scsMonitor"
#define SEC_INITFILE     SEC_APP_NAME".ini"
#define SEC_DATAFILE     SEC_APP_NAME".json"

#define APP_SUBTOPIC1      "+/M-scsMonitor/#"
#define APP_SUBTOPIC2      "+/scsMonitor/#"

#define APP_DEFAULT_TOPIC "M-scsMonitor/M-smiOS"

#define SEC_DB_CLIENT_NUM   4

#define SEC_MAP_ID_MIN    1
#define SEC_MAP_ID_MAX    5

#define SEC_APPID_MAX     32

#define SEC_RECORD_MAX    32

#define APP_NAME_MAX     (64)

/* 串口服务APP公共数据 */
typedef struct tag_SEC_APPDATA_T
{
    uint32      mqttip;       //mqtt地址
    uint16      mqttport;     //mqtt端口
    uint8       appidlen;
    uint8       appid[SEC_APPID_MAX];
    char       *appName;      //APP名称
    int         dbAgent;
    time_t      msgtime;  //消息收到的时间
}SEC_APPDATA_T;

#ifdef __cplusplus
}
#endif

#endif


