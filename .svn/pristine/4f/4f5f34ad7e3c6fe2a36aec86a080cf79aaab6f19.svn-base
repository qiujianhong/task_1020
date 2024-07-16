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
#ifndef	__LC_COMM_H
#define __LC_COMM_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define  APP_RECV_BUF_LEN      8192	/*recvfrom接收缓冲区大小*/

#define LC_INITFILE  "lcMonitor.ini"

#define LC_APP_NAME       "lcMonitor"
#define APP_SUBTOPIC1      "+/M-lcMonitor/#"
#define APP_SUBTOPIC2      "+/lcMonitor/#"
#define APP_SUBTOPIC3      "+/Broadcast/#"

#define APP_DEFAULT_TOPIC "M-lcMonitor/M-smiOS"

#define LC_DB_CLIENT_NUM   4

#define LC_MAP_ID_MIN    1
#define LC_MAP_ID_MAX    5

#define LC_APPID_MAX     32
#define LC_DEVNAME_MAX   64

typedef enum tag_LC_APPTYPE_E
{
    LC_APP_OLD = 0,
    LC_APP_NEW = 1,
}LC_APPTYPE_E;

typedef enum tag_LC_CHANNELID_E
{
    LC_CCH_MANAGER = 0,  //管理通道
    LC_CCH_DATACOM = 1,  //数据通道
    LC_CCH_MAX,
}LC_CHANNELID_E;

/* 回路监控公共数据 */
typedef struct taglcAPPData
{
    uint32       mqttip;       //mqtt地址
    uint16       mqttport;     //mqtt端口
    uint8        appidlen;
    LC_APPTYPE_E appType;      //app的类型（为兼容scu的各形态）
    uint8        appid[LC_APPID_MAX];
    char        *appName;      //APP名称
    int          dbAgent;
    char         devName[LC_CCH_MAX][LC_DEVNAME_MAX];
}LC_APPDATA_T;

#ifdef __cplusplus
}
#endif

#endif


