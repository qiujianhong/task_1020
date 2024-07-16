/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： dockerManagerDefinition.h
           docker manager头文件
* @date： 2019-12-3
*********************************************************************
*/

#ifndef __DOCKER_MANAGER_DEFINITION_H__
#define __DOCKER_MANAGER_DEFINITION_H__

#include "baseType.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DOCKER_CMD_RESULT_MAX_LENGHT     (1024)
#define DOCKER_CMD_MAX_LENGTH            (1024)
#define DOCKER_NAME_MAX_LENGTH           (32)
#define DOCKER_VERSION_MAX_LENGTH        (32)
#define DOCKER_ID_LENGTH                 (12)
#define DOCKER_EXPOSE_PORT_LENGTH        (128)
#define DOCKER_CPU_SET_MAX_LENGTH        (16)
#define DOCKER_CPU_SHARES_MAX_LENGTH     (16)
#define DOCKER_MAX_COUNT                 (20)
#define DOCKER_DIR_ARRAY_MAX_LENGTH      (32)
#define DOCKER_PERMISSION_ARRAY_MAX_LEN  (4)
#define DOCKER_SHARE_DIR_MAX_COUNT       (10)
#define DOCKER_SHARE_FILE_NODE_MAX_COUNT (10)
#define DOCKER_FILE_NODE_NAME_MAX_LEN    (32)
#define DOCKER_ENV_MAX_LENGTH            (32)
#define DOCKER_MEM_LIMT_LENGTH           (16)
#define CONTAINER_NAME_MAX_LENGTH        (128)
#define CONTAINER_CPU_REATE_MAX_LENGTH   (7)
#define CONTAINER_MEM_INFO_MAX_LENGTH    (32)
#define CONTAINER_NET_MAX_LENGTH         (32)
#define CONTAINER_BLOCK_MAX_LENGTH       (32)
#define CONTAINER_MEM_PERC_MAX_LENGTH    (32)
#define CONTAINER_BLOCK_MAX_LENGTH       (32)
#define CONTAINER_PID_MAX_LENGTH         (32)
#define DOKCER_RECORD_MAX_LENGTH         (1536)

//#define DOCKER_CERT_SWITCH               (1)
//#define DOCKER_FUNCTION_TEST             (1)

typedef enum net_bridge_type
{
    NET_BRIDEG = 0,                         //使用docker daemon指定的网桥
    NET_CONTAINER_NAME,                     //使用其他容器的网络，共享IP和PORT等网络资源
    NET_CONTAINER_ID,                       //使用其他容器的网络，共享IP和PORT等网络资源
    NET_CONTAINER_HOST,                     //容器使用主机的网络
    NET_NONE                                //容器使用自己的网络（类似bridge）, 但是不进行配置
} NET_BRIDGE_TYPE;

typedef enum upgrade_file_type
{
    FULL_UPGRADE = 0,                       //全版本升级
    DIFFERENCE_UPGRADE                      //差分升级
} UPGRADE_FILE_TYPE;

typedef struct share_dir_info {
    char directory[DOCKER_DIR_ARRAY_MAX_LENGTH + 1];        //映射目录
    char permission[DOCKER_PERMISSION_ARRAY_MAX_LEN + 1];   //权限
} SHARE_DIR_T;

typedef struct share_file_node_info {
    char fileNode[DOCKER_FILE_NODE_NAME_MAX_LEN + 1];       //文件节点
    char permission[DOCKER_PERMISSION_ARRAY_MAX_LEN + 1];   //权限
} SHARE_FILE_NODE_T;

typedef struct docker_info {
    char mirrorName[DOCKER_NAME_MAX_LENGTH + 1];     //docker镜像名
    //char version[DOCKER_VERSION_MAX_LENGTH + 1];   //docker版本
    char mirrorPackage[DOCKER_NAME_MAX_LENGTH + 1];  //镜像名称
    char containerId[DOCKER_ID_LENGTH + 1];          //容器id
    char env[DOCKER_ENV_MAX_LENGTH + 1];
    char memThreshold[DOCKER_MEM_LIMT_LENGTH + 1];   //docker占用内存阈值
    char exposePort[DOCKER_EXPOSE_PORT_LENGTH + 1];  //容器对外开放的端口范围,例如8200-8203
    char cpuSet[DOCKER_CPU_SET_MAX_LENGTH + 1];      //容器运行时使用的cpu集合
    char cpuShares[DOCKER_CPU_SHARES_MAX_LENGTH + 1];
    //RESTART_TYPE restart;
    NET_BRIDGE_TYPE net;                             //容器网络模式
    sint32 shareDirCount;                            //映射文件数量
    SHARE_DIR_T shareDir[DOCKER_SHARE_DIR_MAX_COUNT];//映射目录信息
    sint32 shareFileNodeCount;                       //映射文件节点数量
    SHARE_FILE_NODE_T shareFileNode[DOCKER_SHARE_FILE_NODE_MAX_COUNT]; //映射文件节点信息
} DOCKER_INFO_T;

typedef struct container_info {
    char dockerName[DOCKER_NAME_MAX_LENGTH + 1];            //镜像名
    //char version[DOCKER_VERSION_MAX_LENGTH + 1];          //版本号
    char containerName[DOCKER_NAME_MAX_LENGTH + 1];         //容器名
    char containerId[DOCKER_ID_LENGTH + 1];                 //容器ID
    //char name[CONTAINER_NAME_MAX_LENGTH + 1];             //
    char cpuPerc[CONTAINER_CPU_REATE_MAX_LENGTH + 1];       //CPU占用率门限
    char memUse[CONTAINER_MEM_INFO_MAX_LENGTH + 1];         //使用内存数量
    char memLimit[CONTAINER_MEM_INFO_MAX_LENGTH + 1];       //内存最大数量
    char netIO[CONTAINER_NET_MAX_LENGTH + 1];               
    char blockIO[CONTAINER_BLOCK_MAX_LENGTH + 1];
    char memPerc[CONTAINER_MEM_PERC_MAX_LENGTH + 1];        //内存占用率门限
    //char pids[CONTAINER_PID_MAX_LENGTH];
} CONTAINER_INFO_T;

#ifdef __cplusplus
}
#endif 

#endif