/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： appManager.h
           app管理器头文件
* @date： 2019-11-12
*********************************************************************
*/

#ifndef __APP_MANAGER_H__
#define __APP_MANAGER_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

#include "baseType.h"
#include "msgService.h"
#include "xdrCustom.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief:app管理器模块初始化 */
sint32 app_manager_init_system(void);

/* @brief:app管理器模块清理 */
void app_manager_clean_up_system(void);

/* @brief:app管理器启动 */
sint32 app_manager_start_system(void);

/* @brief:app管理器停止 */
void app_manager_stop_system(void);

/* @brief:app 注册 */
sint32 app_manager_login_process(MSG_DATE_T* msgData, DATA_TIME_T *dataTime);

/* @brief: app取消注册 */
sint32 app_manager_logout_process(char* appName);

/* @brief: 查询所有app注册信息 */
sint32 app_manager_all_app_query(MQT_PLUGIN_T **mqtPlugin, uint32* count);

/* @brief: 查询所有app注册信息 */
sint32 app_manager_one_app_query(MQT_PLUGIN_T *mqtPlugin, char* appName);

/* @brief: 查询单个app注册信息 */
sint32 app_manager_app_query(char* appName, MSG_CLIENT_T *client);

/* @brief: APP事件订阅 */
sint32 app_manager_sub_event(char *appName, APP_SUB_INFO_RAW_T *subInfo, sint32 isSub);

/* @brief: 查询APP事件订阅信息 */
sint32 app_manager_get_sub_info(char *appName, APP_SUB_INFO_RAW_T *subInfo);

/* @brief: app事件处理 */
sint32 app_manager_event_handle(uint32 eventID, uint8 *eventBuf, uint32 eventBufLen);

/* @brief: app心跳处理 */
sint32 app_manager_heart_beat_process(char* appName, uint64 threadStatus);

#ifdef __cplusplus
}
#endif 

#endif