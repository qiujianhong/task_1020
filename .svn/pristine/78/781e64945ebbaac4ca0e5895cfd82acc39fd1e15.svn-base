/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： appManager.c
* @date： 2019-11-12
*********************************************************************
*/

#ifndef __APP_MANAGER_COMMON_H__
#define __APP_MANAGER_COMMON_H__

#include "appManagerDefinition.h"
#include "qlibc.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct
{
	pthread_mutex_t		aliveLock;
	char 	aliveAppName[MAX_APP_COUNT][MAX_APP_NAME_LEN];		//保活状态的app名称
	uint8	aliveAppCnt;										//保活的app数量
}APP_ALIVE_T;

//app模块初始化
sint32 app_manager_init(void);
//启动列表处理
sint32 app_start_list_parse(void);
//启动列表信息保存
sint32 app_start_list_save(void);
//从启动列表中删除app
sint32 app_start_list_delete(char* appName);
//启动列表中添加app
sint32 app_start_list_add(APP_START_INFO_T* appStartListInfo);
//app启动信息保存
sint32 app_start_info_save(APP_START_INFO_T* appStartInfo, char* recordContext);

//添加注册信息到注册列表
sint32 app_register_list_add(APP_REGISTER_INFO_T* appRegitsterInfo);
//注册列表更新
sint32 app_register_list_update(APP_REGISTER_INFO_T* appRegitsterInfo);
//注册列表删除
sint32 app_register_list_delete(char* appName);
//注册列表保存
void* app_register_list_save( void* arg);
//注册信息保存
sint32 app_register_info_save(APP_REGISTER_INFO_T* pAppRegisterInfo, char* recordContext);
//注册列表文件处理
sint32 app_register_list_parse(void);
//获取注册列表中app数量
uint32 app_register_app_count_get(void);
//查询所有app
sint32 app_all_app_query(MQT_PLUGIN_T **mqtPlugin, uint32* count);
//事件处理
uint32 app_event_handle(uint32 eventID, uint8 *eventBuf, uint32 eventBufLen);
//获取容器中app名称
sint32 app_get_app_name_in_container(const char* containerName, char* appNameArray, sint32* appCount);

//获取app启动信息
APP_START_INFO_T* app_get_app_start_info(const char* appName);
//获取app注册信息
APP_REGISTER_INFO_T* app_get_app_register_info(const char* appName);
sint32 app_get_app_name_use_same_container(const char* containerName, char* appNameArray, sint32* appCount);

//启动全部app
sint32 app_all_start(void);
//app启动
sint32 app_start(APP_START_INFO_T* appStartListInfo);
sint32 app_cmd_execute(const char* cmd,  char* result);
//app配置信息处理
sint32 app_config_parse(char* appName, char* appConfigFile, APP_START_INFO_T* appStartListInfo);
//停止app
sint32 app_stop(char* appName);
//重启app
sint32 app_restart(char* appName);

sint32 app_install(char* appName);
sint32 app_upgrade(char* url, char* fileName, char* md5Sum);
sint32 app_uninstall(char* appName, DELETE_TYPE deleteType);

//app包解压缩
sint32 app_package_decompress(char* fileName);
//app下载
sint32 app_download(char* url, char* fileName, char* md5Sum);
//app下载包校验
sint32 app_download_package_check(char* fileName, char* md5Sum);
//app安装内容校验
sint32 app_install_package_check(char* fileName, char* sha1Sum);
//签名校验
sint32 app_package_digital_sign_check(char* fileName, char* appName);
//安装包删除
sint32 app_package_delete(char* packageName);
sint32 app_package_decrypt(char* fileName);
//app是否在注册列表中
sint32 app_register_list_check(char* appName);
//容器是否在运行
sint32 app_contaienr_is_running(char* containerName);
//容器删除
sint32 app_container_remove(char* containerName);

//void app_docker_clear(void);
//启动列表清除
void app_start_list_clear(void);
//注册列表清除
void app_register_list_clear(void);

//创建心跳检测函数
void createAppHeartBeatDetect(void);
//创建app注册信息保存线程
void createAppRegisterFileSaveThread(void);

sint32 alive_app_name_save(char *appName);
sint32 alive_app_name_del(char *appName);
sint32 alive_app_name_read(char *appName);

#ifdef APP_INSTALL_DEBUG
//just for debug
//static void* createAppInstallObserver(void);
void createAppInstallThread(void);
#endif

#ifdef __cplusplus
}
#endif

#endif

