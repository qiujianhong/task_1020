/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： dockerManager.h
           docker manager头文件
* @date： 2019-12-2
*********************************************************************
*/

#ifndef __DOCKER_MANAGER_H__
#define __DOCKER_MANAGER_H__

#include "baseType.h"
#include "dockerManagerDefinition.h"

#ifdef __cplusplus
extern "C" {
#endif 

/* @brief:docker管理模块初始化 */
sint32 docker_manager_init_system(void);

/* @brief:docker管理模块清理 */
void docker_manager_clean_up_system(void);

/* @brief:docker管理启动 */
sint32 docker_manager_start_system(void);

/* @brief:docker管理停止 */
void docker_manager_stop_system(void);

#ifdef __cplusplus
}
#endif 

#endif
