/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： dockerManagerCommon.h
           docker manager 头文件
* @date： 2019-12-2
*********************************************************************
*/

#ifndef __DOCKER_MANAGER_COMMON_H__
#define __DOCKER_MANAGER_COMMON_H__

#include "baseType.h"
#include "dockerManagerDefinition.h"
#include "appManagerDefinition.h"
#include "qlibc.h"
#include "msgHandler.h"

#ifdef __cplusplus
extern "C" {
#endif

sint32 docker_manager_init(void);
sint32 docker_manager_start(void);
sint32 docker_manager_exit(void);
void docker_manager_container_clear(void);
sint32 docker_cmd_execute(const char* cmd,  char* result);

sint32 docker_mangager_container_start(char* mirrorName, char* containerName);
sint32 docker_manager_docker_start(DOCKER_INFO_T* dockerInfo, char* containerName);
sint32 docker_manager_docker_stop(char* dockerId);
//sint32 docker_manager_docker_remove(char* dockerName, char* version);
//sint32 docker_manager_load(char* mirrorName);
sint32 docker_manager_mirror_download(char* url, char* mirrorName, char* md5);
sint32 docker_manager_mirror_update(char* url, char* mirrorName, char* md5Sum, UPGRADE_FILE_TYPE type, char* md5sum);
sint32 docker_manager_mirror_delete(char* mirrorName);
sint32 docker_manager_mirror_info_get(char* data, sint32* strLength);

qhashtbl_t* docker_manager_get_container_table_head(void);
qhashtbl_t* docker_manager_get_mirror_table_head(void);
//容器安装包解压缩
sint32 docker_package_decompress(char* fileName);
//容器下载包校验
sint32 docker_download_package_check(char* fileName, char* md5Sum);
//容器安装包校验
sint32 docker_install_package_check(char* fileName, char* sha1Sum);
//容器数字签名校验
sint32 docker_digital_sign_check(char* fileName, char* containerName);
//容器安装包删除
sint32 docker_package_delete(char* packageName);
//容器配置文件处理
sint32 docker_manager_parse_config(char* filename, DOCKER_INFO_T* dockerInfo, APP_START_INFO_T* appStartInfo);
//容器配置文件保存
sint32 docker_manager_config_save(void);
//容器配置文件加载
sint32 docker_manager_config_load(void);

//容器镜像加载
sint32 docker_manager_mirror_load(char* mirrorName);
//获取容器名
sint32 docker_manager_container_name_get(char* containerNameArray, sint32* containerCount);
//获取容器中cpu占用率
sint32 docker_manager_container_cpu_rate_get(const char* containerName, float* cpu);
//获取容器中app名称
sint32 docker_manager_container_app_name_get(const char* containerName, char* appNameArray, sint32* appCount);
//获取容器中app名称
sint32 docker_manager_app_name_get(const char* containerName, char* appNameArray, sint32* appCount);
//获取容器内存占用率
sint32 docker_manager_container_mem_rate_get(const char* containerName, float* memory);
//获取容器falsh 占用量
sint32 docker_manager_container_flash_size_get(const char* containerName, char* flashSize);
//获取app cpu使用率
sint32 docker_manager_app_cpu_rate_get(const char* containerName, const char* appName, float* cpuRate);
//获取app 内存占用率
sint32 docker_manager_app_mem_rate_get(const char* containerName, const char* appName, float* memRate);
//获取app flash占用量
sint32 docker_manager_app_flash_size_get(const char* appName, char* flashSize);
//容器信息上报
sint32 docker_manager_info_report(char* data, sint32* length);

//创建容器监控线程
void createDockerMonitorThread(void);

#ifdef DOCKER_FUNCTION_TEST
void createDockerTestThread(void);
#endif

typedef struct
{
    uint8 num;
    SERVICE_START_T info[2];
}SERVICE_START_INFO_T;
void service_stop_check();
int service_stop(uint8 type);
int service_start(SERVICE_START_T *serviceInfo);

extern char* shellcmd(const char* cmd, char* buff, int size);
#ifdef __cplusplus
}
#endif

#endif


