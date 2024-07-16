/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： dockerManager.h
           docker manager头文件
* @date： 2019-12-2
*********************************************************************
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "framecomm.h"
#include "dockerManager.h"
#include "dockerManagerCommon.h"
#include "msgHandler.h"


/* @brief:docker管理模块初始化 */
sint32 docker_manager_init_system(void)
{
    msg_handler_init(NULL);
    INIT_FMT_DEBUG( "docker manager init.\n");
    docker_manager_init();
    return 0;
}

/* @brief:docker管理模块清理 */
void docker_manager_clean_up_system(void)
{
    SMIOS_FMT_DEBUG( "%s\n", __FUNCTION__);
    return;
}

/* @brief:docker管理启动 */
sint32 docker_manager_start_system(void)
{
    SMIOS_FMT_DEBUG( "%s\n", __FUNCTION__);
#ifdef DOCKER_FUNCTION_TEST
    createDockerTestThread();
#endif
    docker_manager_start();
    createDockerMonitorThread();
    return 0;
}

/* @brief:docker管理停止 */
void docker_manager_stop_system(void)
{
    SMIOS_FMT_DEBUG( "%s\n", __FUNCTION__);
    return;
}
