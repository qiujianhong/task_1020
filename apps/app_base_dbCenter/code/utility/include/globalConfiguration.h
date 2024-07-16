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

#include "constant.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "baseType.h"

/* @brief:UDP通讯服务配置参数 结构体封装  */
typedef struct _server_config_obj
{
    sint8  registerServerType[CONNECT_TYPE_LEN];//注册服务类型
    sint8  serverName[MAX_APP_NAME_LEN];        // 服务名称
    sint8  mqttUserName[MAX_USER_NAME_LEN];     // MQTT用户名
    sint8  mqttPassword[MAX_PASSWD_LEN];        // MQTT密码
    sint8  mqttBrokerIP[IP_LEN];                // mqtt Broker IP
    uint16 mqttBrokerPort;                      // mqtt Broker 端口
    sint8  tcpServerIP[IP_LEN];                 // tcp 服务的IP
    uint16 tcpServerPort;                       // tcp 服务的端口
    sint8  udpServerIP[IP_LEN];                 // udp 服务的IP
    uint16 udpServerPort;                       // udp 服务的端口
    sint8  debugSqlIP[IP_LEN];                  // debug 服务的IP
    uint16 debugSqlPort;                        // debug 服务的端口
    sint8  smiOSConnectType[CONNECT_TYPE_LEN];  //连接类型的字符串长度
    sint8  smiOSIP[IP_LEN];                     // smiOS IP地址
    uint16 smiOSPort;                        // smios UDP端口
    uint32 maxClientAccept;                     //最大客户端接入数
    uint32 handerThreadNum;                     //最大线程数
    uint32 handerTaskQueueSize;                 //最大任务队列数

} SERVICE_CONFIG_OBJ_T;

/* @brief:数据库配置参数 结构体封装  */
typedef struct _db_config_obj
{
    uint32 walCount;                //wal cache count
    sint8  basePath[PATH_LEN];      //数据库的基础路径
    sint8  backupPath[PATH_LEN];    //数据库的备份路径
    sint8  tmpfsPath[64];           //虚拟内存映射路径
    sint8  cfgFile[PATH_LEN];       //数据配置文件
    sint8  logFile[PATH_LEN];       //日志配置文件
    sint8  toolSqlite[PATH_LEN];    //sqlite工具
    sint8  powerDb[PATH_LEN];       //权限数据库表
    sint8  checkDataPath[PATH_LEN]; //数据库磁盘空间路径
    uint32 autoDeleteNum;           //自动删除设置
    uint32 deleteInterval;          //删除时间间隔
    uint32 deleteDepthValue;
} DB_CONFIG_OBJ_T;

/* @brief:全局配置结构体封装 包括UDP 数据库配置  */
typedef struct _global_config_obj
{
    SERVICE_CONFIG_OBJ_T serviceCfg;  //服务配置
    DB_CONFIG_OBJ_T      dbCfg;       //数据库配置

} GLOBAL_CONFIG_OBJ_T;

/* @brief:定义一个全局配置变量  */
extern GLOBAL_CONFIG_OBJ_T g_configObj;

int dir_init(void);

/* @brief:全局配置参数载入  */
sint32 global_config_init(const sint8 *path);

/* @brief:全局配置debug打印出来  */
void global_config_dump(void);

#ifdef __cplusplus
}
#endif

#endif
