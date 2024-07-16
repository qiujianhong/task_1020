/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 数据中心服务全局配置头文件
* @date： 2019-11-9
*********************************************************************
*/

#ifndef __GLOBAL_CONFIGURATION_H__
#define __GLOBAL_CONFIGURATION_H__

#include "baseType.h"
#include "constant.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief:通讯服务配置参数 结构体封装  */
typedef struct _server_config_obj
{
    sint8  mqttUserName[MAX_USER_NAME_LEN];
    sint8  mqttPassword[MAX_PASSWD_LEN];
    sint8  mqttBrokerIP[IP_LEN];  // mqtt 服务的IP
    uint16 mqttBrokerPort;        // mqtt 服务的端口
    uint16 mqttSubQos;            // mqtt Broker 订阅qos
    sint8  tcpServerIP[IP_LEN];   // tcp 服务的IP
    uint16 tcpServerPort;         // tcp 服务的端口
    sint8  udpServerIP[IP_LEN];   // udp 服务的IP
    uint16 udpServerPort;         // udp 服务的端口
    sint8  debugSqlIP[IP_LEN];    // debug 服务的IP
    uint16 debugSqlPort;          // debug 服务的端口
    uint32 maxClientAccept;       //最大客户端接入数
    uint32 handerThreadNum;       //最大线程数
    uint32 handerTaskQueueSize;   //最大任务队列数
    sint8 mqttServiceName[MAX_APP_NAME_LEN];
    sint8 tcpServiceName[MAX_APP_NAME_LEN];
    sint8 udpServiceName[MAX_APP_NAME_LEN];
    
    sint8 localShareDir[APP_MAX_SHARE_DIR_LEN];
    sint8 localAppInstallDir[APP_MAX_SHARE_DIR_LEN];
    sint8 localAppConfigDir[APP_MAX_SHARE_DIR_LEN];
    sint8 dockerShareDir[APP_MAX_SHARE_DIR_LEN];
    sint8 dockerstartListFile[DOCKER_START_LIST_FILE_NAME_LEN];
    sint8 startListFile[APP_START_LIST_FILE_NAME_LEN];
    sint8 signFilePath[APP_SIGN_FILE_PATH_LEN];
    sint8 registerListFile[APP_REGISTER_LIST_FILE_NAME_LEN];
	sint8 localIpInfoFile[LOCAL_IP_FILE_NAME_LEN];

    uint32 memratewarnlevel;
    uint32 cpuratewarnlevel;
    uint32 storagerateWarnlevel;
    uint32 wdfeedtimerval;
} SERVICE_CONFIG_OBJ_T;

typedef struct _platform_conf
{
    sint8  mqttBrokerIP[IP_LEN];  // 管理平台 MQTT 服务的IP
    uint16 mqttBrokerPort;        // 管理平台 MQTT 服务的端口
    sint8  mqttUserID[MAX_USER_ID_LEN];
    sint8  mqttUserName[MAX_USER_NAME_LEN];
    sint8  mqttPassword[MAX_PASSWD_LEN];
    // TLS config

} PLATFORM_CONFIG_OBJ_T;

typedef struct _fcgi_conf
{
    sint8  ip[IP_LEN];  // FCGI 绑定IP
    uint16 port;        // FCGI 绑定端口
    // TLS config

} FCGI_CONFIG_OBJ_T;

/* @brief:全局配置结构体封装 包括UDP 数据库配置  */
typedef struct _global_config_obj
{
    SERVICE_CONFIG_OBJ_T  serviceCfg;   //服务配置
    PLATFORM_CONFIG_OBJ_T platformCfg;  //平台配置
    FCGI_CONFIG_OBJ_T fcgiCfg;          //fcgi服务配置

} GLOBAL_CONFIG_OBJ_T;

/* @brief:定义一个全局配置变量  */
extern GLOBAL_CONFIG_OBJ_T g_configObj;

/* @brief:全局配置参数载入  */
sint32 global_config_init(const sint8 *path);

/* @brief:全局配置debug打印出来  */
void global_config_dump(void);

#ifdef __cplusplus
}
#endif

#endif