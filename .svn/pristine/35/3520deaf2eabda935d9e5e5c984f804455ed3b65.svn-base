/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： appManagerDefinition.h
           app管理器头文件
* @date： 2019-11-12
*********************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef __APP_MANAGER_DEFINITION_H__
#define __APP_MANAGER_DEFINITION_H__

#include<string.h>
#include<sys/time.h>
#include "xdrCustom.h"
#include "constant.h"

#define APP_NAME_LENGTH               (127)
#define APP_VERSION_LENGTH            (255)
#define APP_DOCKER_NAME_LENGTH        (128)
#define APP_START_PARAM_LENGTH        (255)
#define APP_DATA_PATH_LENGTH          (128)
#define APP_AUTHOR_NAME_LENGTH        (255)
#define APP_DIRECTORY_MAX_LENGTH      (512)
#define APP_CONFIG_FILE_MAX_SIZE      (2048)
#define APP_INSTALL_FLAG              (1)
#define APP_UNINSTALL_FLAG            (0)
#define APP_CMD_RESULT_MAX_LENGTH     (1024)
#define APP_CMD_MAX_LENGTH            (1024)
#define APP_RECORD_MAX_LENGTH         (1536)
#define APP_DOCKER_MAX_COUNT          (10)
#define APP_DOCKER_ID_LENGTH          (12)
#define APP_START_CMD_LENGTH          (1536)
#define APP_IP_MAX_LENGTH             (15)
#define UNIX_SOCKET_PATH_LENGTH       (128)
#define APP_HEART_BEAT_CHECK_TIME     (30)
#define APP_START_TABLE_RANGE         (20)
#define APP_REGISTER_TABLE_RANGE      (20)
#define APP_SHA1_READ_MAX_LENGTH      (1024)
#define APP_MD5_READ_MAX_LENGTH       (1024)
#define APP_FILE_PATH_MAX_LENGTH      (1024)
#define APP_CERT_SF_SIZE              (1024)
#define APP_SHARE_DIR_MAX_COUNT       (10)
#define APP_SHARE_FILE_NODE_MAX_COUNT (10)
#define APP_DIR_ARRAY_MAX_LENGTH      (32)
#define APP_PERMISSION_ARRAY_MAX_LEN  (4)
#define APP_FILE_NODE_NAME_MAX_LEN    (32)
#define APP_ENV_MAX_LENGTH            (32)
#define APP_MEM_LIMT_LENGTH           (16)
#define APP_CPU_RATE_LIMIT_LEN        (16)
#define APP_FLASH_SIZE_LIMIT_LEN      (16)
#define APP_EXPOSE_PORT_LENGTH        (128)
#define APP_MAX_COUNT                 (24)
#define APP_FLASH_SIZE_STR_MAX_LEN    (16)
#define APP_START_LIST_MAX_SIZE       (4096)
#define APP_REGISTER_LIST_MAX_SIZE    (4096)
#define APP_RESTART_MAX_COUNT         (20)

#define APP_HEART_BEAT_CHECK_MAX_TIME (APP_HEART_BEAT_CHECK_TIME * 3)
#define APP_START_LIST_FILE_MAX_SIZE  (APP_CONFIG_FILE_MAX_SIZE * 10)

#define APP_CERT_SF                   "CERT.SF"
#define APP_CERT_RSA                  "CERT.RSA"
#define CERT_PATH                     "/usr/lib/ssl/demoCA"
#define ROOT_CERT                     "cacert.cer"
#define GET_DEFAULT_CA_CERT(str) sprintf(str, "%s/%s", CERT_PATH, ROOT_CERT)
#define GET_CUSTOM_CERT(str, path, name) sprintf(str, "%s/%s", path, name)

#define MAX_LEGTH                      (4096)

//#define CERT_SWITCH                   (1)
#define RESTART_APP                   (1)

//#define APP_INSTALL_DEBUG             (1)

//enum
typedef enum protocol_type {
    PROTOCOL_UDP = 0,
    PROTOCOL_TCP,
    PROTOCOL_IPC,
    PROTOCOL_MQTT,
    PROTOCOL_INVALID
} PROTOCOL_TYPE;

typedef enum delete_type {
    DELETE_TYPE_ALL = 0,
    DELETE_TYPE_PART,
    DELETE_TYPE_INVALID
} DELETE_TYPE;

typedef enum uninstall_flag {
    UNINSTALL_FLAG_FALSE = 0,
    UNINSTALL_FLAG_TRUE,
    UNINSTALL_FLAG_INVALID
} UNINSTALL_FLAG;

typedef enum permission_type {
    PERMISSION_GET_CPU_UTILIZATION_RATE = 0x0001,
    PERMISSION_GET_RAM_UTILIZATION_RATE = 0x0002,
    PERMISSION_GET_REGISTER_LIST        = 0x0004,
    PERMISSION_SAVE_LOG                 = 0x0008
} PERMISSION_TYPE;

typedef enum app_net_bridge_type
{
    APP_NET_BRIDEG = 0,                         //使用docker daemon指定的网桥
    APP_NET_CONTAINER_NAME,                     //使用其他容器的网络，共享IP和PORT等网络资源
    APP_NET_CONTAINER_ID,                       //使用其他容器的网络，共享IP和PORT等网络资源
    APP_NET_CONTAINER_HOST,                     //容器使用主机的网络
    APP_NET_NONE                                //容器使用自己的网络（类似bridge）, 但是不进行配置
} APP_NET_BRIDGE_TYPE;

//structure
typedef struct app_address {
    sint8 ip[APP_IP_MAX_LENGTH + 1];
    uint16 port;
} APP_ADDRESS;

typedef struct ipc_info {
    char unixSockertPath[UNIX_SOCKET_PATH_LENGTH + 1];
} IPC_INFO;

typedef struct _protocol_info {
    PROTOCOL_TYPE protocolType;
    union {
        APP_ADDRESS udp;
        APP_ADDRESS tcp;
        IPC_INFO ipc;
    };
} PROTOCOL_INFO_T;

typedef struct app_share_dir_info {
    char directory[APP_DIR_ARRAY_MAX_LENGTH + 1];
    char permission[APP_PERMISSION_ARRAY_MAX_LEN + 1];
} APP_SHARE_DIR_T;

typedef struct app_share_file_node_info {
    char fileNode[APP_FILE_NODE_NAME_MAX_LEN + 1];
    char permission[APP_PERMISSION_ARRAY_MAX_LEN + 1];
} APP_SHARE_FILE_NODE_T;

typedef struct restart_app_info {
    char appName[64 + 1];
    sint32 pid;
} APP_RESTART_INFO_T;

//app启动信息
typedef struct _app_start_info_ {
    char appName[APP_NAME_LENGTH + 1];                                      //app名称
    char version[APP_VERSION_LENGTH + 1];                                   //版本号
    char dockerName[APP_DOCKER_NAME_LENGTH + 1];                            //镜像名
    char containerName[APP_DOCKER_NAME_LENGTH + 1];                         //容器名
    char startParam[APP_START_PARAM_LENGTH + 1];                            //启动参数
    char dataPath[APP_DATA_PATH_LENGTH + 1];
    char memThreshold[APP_MEM_LIMT_LENGTH + 1];                             //内存使用率最大值
    char cpuRateThreshold[APP_CPU_RATE_LIMIT_LEN + 1];                      //cpu使用率最大值
    char flashSizeThreshold[APP_FLASH_SIZE_LIMIT_LEN + 1];                  //flash 使用率最大值
    APP_NET_BRIDGE_TYPE net;                                                //网络模式
    char exposePort[APP_EXPOSE_PORT_LENGTH + 1];                            //映射端口
    sint32 shareDirCount;                                                   //映射目录数量
    APP_SHARE_DIR_T shareDir[APP_SHARE_DIR_MAX_COUNT];                      //映射目录
    sint32 shareFileNodeCount;                                              //映射文件节点数量
    APP_SHARE_FILE_NODE_T shareFileNode[APP_SHARE_FILE_NODE_MAX_COUNT];     //映射文件节点
    UNINSTALL_FLAG uninstallFlag;                                           //卸载标记
    DELETE_TYPE deleteType;                                                 //卸载类型
} APP_START_INFO_T;

//app注册信息
typedef struct _app_register_info_ {
    sint8 appName[APP_NAME_LENGTH + 1];                 //APP名
    sint8 author[APP_AUTHOR_NAME_LENGTH + 1];           //作者
    sint8 containerName[APP_DOCKER_NAME_LENGTH + 1];    //运行容器名称
    sint8 startParam[APP_START_PARAM_LENGTH + 1];       //启动参数
    PROTOCOL_INFO_T protocolInfo;                       //协议类型
    sint32 pid;                                         //进程号
    sint8 version[APP_VERSION_LENGTH];                                     //app版本
    APP_SUB_INFO_RAW_T subscribeInfo;                   //订阅信息
    uint16 permission;                                  //权限
    uint32 alivePeriod;                                 //生命周期
    sint32 aliveFlag;                                   //生命标记
    uint64 threadStatus;                                //线程状态
    UNINSTALL_FLAG uninstallFlag;                       //卸载标记
    DATA_TIME_T dataTime;                               //日期
    struct timespec tv;                                 //心跳时间
    uint32 appidlen;
    uint8 appid[APPID_LEN];
} APP_REGISTER_INFO_T;

typedef struct _app_docker_info_ {
    sint8 mirrorName[APP_DOCKER_NAME_LENGTH + 1];               //镜像名
    sint8 containerName[APP_DOCKER_NAME_LENGTH + 1];            //容器名
    sint8 dockerId[APP_DOCKER_ID_LENGTH + 1];                   //容器id
    sint8 ip[APP_IP_MAX_LENGTH + 1];                            //IP
} APP_DOCKER_INFO_T;

#endif

#ifdef __cplusplus
}
#endif 
