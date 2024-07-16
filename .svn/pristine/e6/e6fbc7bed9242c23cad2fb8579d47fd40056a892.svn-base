/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： appManager.c
* @date： 2019-11-12
*********************************************************************
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

#include "cJSON.h"
#include "framecomm.h"
#include "openssl/sha.h"
#include "openssl/md5.h"
#include "openssl/evp.h"
#include "openssl/x509.h"
#include "dockerManagerCommon.h"
#include "appManagerCommon.h"
#include "globalConfiguration.h"
#include "msgService.h"

//app启动列表
qhashtbl_t* appStartTbl = NULL;
//app注册列表
qhashtbl_t* appRegisterTbl = NULL;
//注册列表有变化标记
sint32      registerFileNeedUpdate = 0;

APP_ALIVE_T  aliveApp;

extern char APPID[APPID_LEN];
extern uint32 APPIDLen;
/**
*********************************************************************
* @brief：alive_app_name_save
* @param[in]： appName
               
* @param[out]：
               
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 alive_app_name_save(char *appName)
{
	char nameTemp[128] = {0};
	if(appName == NULL)
	{
		return -1;
	}

	strncpy(nameTemp,appName,strlen(appName));

	if(alive_app_name_read(nameTemp) >= 0)
	{
		//SMIOS_FMT_DEBUG("%s : %s is in the alive array,no need to add!",__FUNCTION__,nameTemp);
		return 0;
	}
	pthread_mutex_lock(&aliveApp.aliveLock);
	if(aliveApp.aliveAppCnt >= MAX_APP_COUNT)
	{
		SMIOS_FMT_DEBUG("%s: array is full, clear them!\n",__FUNCTION__);
		memset(aliveApp.aliveAppName,0x0,sizeof(aliveApp.aliveAppName));
		aliveApp.aliveAppCnt = 0;
	}
	strncpy(aliveApp.aliveAppName[aliveApp.aliveAppCnt],nameTemp,MAX_APP_NAME_LEN);
	SMIOS_FMT_TRACE("%s: add alive app name successfully: %s\n",__FUNCTION__,aliveApp.aliveAppName[aliveApp.aliveAppCnt]);
	aliveApp.aliveAppCnt++ ;
	pthread_mutex_unlock(&aliveApp.aliveLock);
	
	return 0;
}

/**
*********************************************************************
* @brief：alive_app_name_del
* @param[in]： appName
               
* @param[out]：
               
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 alive_app_name_del(char * appName)
{
	int i;
	sint32 ret = -1;
	if(appName == NULL)
	{
		return -1;
	}
	pthread_mutex_lock(&aliveApp.aliveLock);
	SMIOS_FMT_TRACE("%s: alive app cnt is : %d\n",__FUNCTION__,aliveApp.aliveAppCnt);
	for(i = 0; i < aliveApp.aliveAppCnt;i++)
	{
//		SMIOS_FMT_DEBUG("%s: %s len is %d, %s len is %d \n",__FUNCTION__,aliveApp.aliveAppName[i],strlen(aliveApp.aliveAppName[i]),
//																appName,strlen(appName));
		if(0 == memcmp(aliveApp.aliveAppName[i],appName,strlen(appName)))
		{
			memset(aliveApp.aliveAppName[i],0x0,sizeof(aliveApp.aliveAppName[i]));
			if(i != MAX_APP_COUNT -1)
			{
				memmove(aliveApp.aliveAppName[i],aliveApp.aliveAppName[i+1],(MAX_APP_COUNT-i-1)*sizeof(aliveApp.aliveAppName[i]));
			}
			
			if(aliveApp.aliveAppCnt > 0)
			{
				aliveApp.aliveAppCnt--;
			}
			SMIOS_FMT_TRACE("%s: delete the not alive app name successfully: %s\n",__FUNCTION__,appName);
			ret = 0;
		}
	}
	pthread_mutex_unlock(&aliveApp.aliveLock);
	return ret;
}

/**
*********************************************************************
* @brief：alive_app_name_read
* @param[in]： appName
               
* @param[out]：
               
* @return： 返回非0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 alive_app_name_read(char * appName)
{
	int i;
	sint32 ret = -1;
	if(appName == NULL)
	{
		return -1;
	}
	pthread_mutex_lock(&aliveApp.aliveLock);
	SMIOS_FMT_TRACE("%s: alive app cnt is : %d\n",__FUNCTION__,aliveApp.aliveAppCnt);
	for(i = 0; i < aliveApp.aliveAppCnt;i++)
	{
		//SMIOS_FMT_DEBUG("appName%d : %s; len : %d",i ,aliveApp.aliveAppName[i],strlen(aliveApp.aliveAppName[i]));
		if(strcmp(aliveApp.aliveAppName[i],appName) == 0)
		{
			//SMIOS_FMT_DEBUG("%s: find alive app name successfully: %s",__FUNCTION__,appName);
			ret = i;
			break;
		}
	}
	pthread_mutex_unlock(&aliveApp.aliveLock);
	return ret;
}

/**
*********************************************************************
* @brief：app管理模块初始化
* @param[in]： 
               none
* @param[out]：
               none
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_manager_init(void)
{
    SMIOS_FMT_DEBUG( __FUNCTION__);
    //app启动列表初始化 不需要启动列表了
    //appStartTbl = qhashtbl(APP_START_TABLE_RANGE, QHASHTBL_THREADSAFE);
    
    //app注册列表初始化
    appRegisterTbl = qhashtbl(APP_REGISTER_TABLE_RANGE, QHASHTBL_THREADSAFE);
    if (NULL == appRegisterTbl)
    {
        SMIOS_FMT_DEBUG("create hash table failed\n");


        if (NULL != appRegisterTbl)
        {
            qhashtbl_free(appRegisterTbl);
        }
        return -1;
    }
    //处理app启动列表配置文件
    //app_start_list_parse();
    //处理app注册列表配置文件
    app_register_list_parse();
    //创建app心跳发送线程
    createAppHeartBeatDetect();
    //创建app注册信息保存线程
    createAppRegisterFileSaveThread();
    memset(&aliveApp,0x0,sizeof(APP_ALIVE_T));

    return 0;
}

/**
*********************************************************************
* @brief：从app启动文件中读取数据，并将数据放到启动列表中
* @param[in]： 
               none
* @param[out]：
               none
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_start_list_parse(void)
{
    FILE* fp = NULL;
    char buf[APP_START_LIST_MAX_SIZE] = {0};
    SMIOS_FMT_DEBUG("start parse the app start list\n");
    if (NULL == appStartTbl)
    {
        SMIOS_FMT_DEBUG("app start hash table is invalid\n");
        return -1;
    }

    //open the start list file
    if ((fp = fopen(g_configObj.serviceCfg.startListFile, "r")) == NULL)
    { 
        SMIOS_FMT_DEBUG("Can not open file[%s]\n", g_configObj.serviceCfg.startListFile);
        return -1;
    }

    SMIOS_FMT_DEBUG( "Open file[%s] success\n", g_configObj.serviceCfg.startListFile);
    //read the file
    sint32 ret = fread(buf, 1, APP_START_LIST_MAX_SIZE, fp);

    if (ret > 0)
    {
        fclose(fp);
        fp = NULL;

        //处理json字符串
        cJSON* root = cJSON_Parse(buf);
        if (NULL == root)
        {
            SMIOS_FMT_DEBUG("Parse app start list error\n");
            return -1; 
        }
        //get app info
        cJSON* value = cJSON_GetObjectItem(root, "app_info");
        if (value == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        //get app array
        sint32 len = cJSON_GetArraySize(value);
        SMIOS_FMT_DEBUG("app count = %d\n", len);

        cJSON* appName          = NULL;             // app名称
        cJSON* version          = NULL;             // 版本号
        cJSON* dockerName       = NULL;             // 镜像名
        cJSON* containerName    = NULL;             // 容器名
        cJSON* startParam       = NULL;             // 启动参数
        cJSON* dataPath         = NULL;             
        cJSON* uninstallFlag    = NULL;             //卸载标记
        cJSON* shareDir         = NULL;             //映射目录
        cJSON* shareFileNode    = NULL;             //映射节点
        cJSON* net              = NULL;             //网络模式
        cJSON* exposePort       = NULL;             //映射端口
        cJSON* memThreshold     = NULL;             //内存占用率
        cJSON* cpuRateThreshold = NULL;             //cpu占用率
        cJSON* flashSizeThreshold = NULL;           //flash占用率
        cJSON* pointer          = NULL;
        APP_START_INFO_T tmpAppStartInfo;

        sint32 i = 0;
        for( i = 0; i < len; i++ )
        {
            memset(&tmpAppStartInfo, 0, sizeof(APP_START_INFO_T));
            APP_START_INFO_T* pAppStartInfo = &tmpAppStartInfo;

            pointer          = cJSON_GetArrayItem(value, i);

            appName          = cJSON_GetObjectItem(pointer,       "app_name");
            version          = cJSON_GetObjectItem(pointer,       "version");
            dockerName       = cJSON_GetObjectItem(pointer,    "docker_name");
            containerName    = cJSON_GetObjectItem(pointer, "container_name");
            startParam       = cJSON_GetObjectItem(pointer,    "start_param");
            dataPath         = cJSON_GetObjectItem(pointer,       "dataPath");
            uninstallFlag    = cJSON_GetObjectItem(pointer, "uninstall_flag");
            exposePort       = cJSON_GetObjectItem(pointer,     "exposePort");
            net              = cJSON_GetObjectItem(pointer,            "net");
            memThreshold     = cJSON_GetObjectItem(pointer,   "memThreshold");
            shareDir         = cJSON_GetObjectItem(pointer,       "ShareDir");
            shareFileNode    = cJSON_GetObjectItem(pointer,  "ShareFileNode");
            cpuRateThreshold = cJSON_GetObjectItem(pointer, "cpuRateThreshold");
            flashSizeThreshold = cJSON_GetObjectItem(pointer, "flashSizeThreshold");

            //检测信息有效性
            if (NULL == appName || NULL == version || NULL == dockerName || NULL == containerName ||
                NULL == startParam || NULL == dataPath || NULL == uninstallFlag || NULL == exposePort || NULL == net ||
                NULL == memThreshold || NULL == shareDir || NULL == shareFileNode || NULL == cpuRateThreshold ||
                NULL == flashSizeThreshold)
            {
                SMIOS_FMT_DEBUG("the config file is invalid\n");
                return -1;
            }
            SMIOS_FMT_DEBUG( "%s\t%s\t%s\t%s\t%s\n%s\t%s\t%s\n", appName->valuestring, version->valuestring,
                        dockerName->valuestring, startParam->valuestring,
                        uninstallFlag->valuestring, net->valuestring,
                        exposePort->valuestring, memThreshold->valuestring);

            strncpy(pAppStartInfo->appName,            appName->valuestring,    APP_NAME_LENGTH);
            strncpy(pAppStartInfo->version,            version->valuestring,    APP_VERSION_LENGTH);
            strncpy(pAppStartInfo->dockerName,         dockerName->valuestring, APP_DOCKER_NAME_LENGTH);
            strncpy(pAppStartInfo->containerName,      containerName->valuestring, APP_DOCKER_NAME_LENGTH);
            strncpy(pAppStartInfo->startParam,         startParam->valuestring, APP_START_PARAM_LENGTH);
            strncpy(pAppStartInfo->dataPath,           dataPath->valuestring, APP_DATA_PATH_LENGTH);
            strncpy(pAppStartInfo->exposePort,         exposePort->valuestring, APP_EXPOSE_PORT_LENGTH);
            strncpy(pAppStartInfo->memThreshold,       memThreshold->valuestring, APP_MEM_LIMT_LENGTH);
            strncpy(pAppStartInfo->cpuRateThreshold,   cpuRateThreshold->valuestring, APP_CPU_RATE_LIMIT_LEN);
            strncpy(pAppStartInfo->flashSizeThreshold, flashSizeThreshold->valuestring, APP_FLASH_SIZE_LIMIT_LEN);
            pAppStartInfo->net           = atoi(net->string);
            pAppStartInfo->uninstallFlag = atoi(uninstallFlag->string);

            if (shareDir == NULL)
            {
                SMIOS_FMT_DEBUG("GetObjectItem error\n");
                return -1;
            }

            SMIOS_FMT_DEBUG( "shareDir: %s\n", shareDir->valuestring);
            //get share dir count
            int shareDirLen = cJSON_GetArraySize(shareDir);
            SMIOS_FMT_DEBUG( "shareDirLen = %d\n", shareDirLen);
            pAppStartInfo->shareDirCount = shareDirLen;
            if (shareDirLen > APP_SHARE_DIR_MAX_COUNT)
            {
                SMIOS_FMT_DEBUG("the share directory count exceed the max count\n");
                return -1;
            }

            cJSON* directory  = NULL;
            cJSON* permission = NULL;
            int index = 0;
            //get the directory and permisson information
            for (index = 0; index < shareDirLen; index++)
            {
                directory  = cJSON_GetArrayItem(shareDir, index);
                permission = cJSON_GetArrayItem(shareDir, index);

                directory  = cJSON_GetObjectItem(directory, "directory");
                permission = cJSON_GetObjectItem(permission,"permission");

                char* tmpDirectory  = directory->valuestring;
                char* tmpPermission = permission->valuestring;

                SMIOS_FMT_DEBUG( "directory:%s, permisson:%s\n", tmpDirectory, tmpPermission);
                strncpy(pAppStartInfo->shareDir[index].directory,  tmpDirectory, APP_DIR_ARRAY_MAX_LENGTH);
                strncpy(pAppStartInfo->shareDir[index].permission, tmpPermission, APP_PERMISSION_ARRAY_MAX_LEN);
            }

            if (shareFileNode == NULL)
            {
                SMIOS_FMT_DEBUG( "GetObjectItem error\n");
                return -1;
            }

            SMIOS_FMT_DEBUG( "shareFileNode: %s\n", shareFileNode->valuestring);
            //get the file node count
            int shareFileNodeLen = cJSON_GetArraySize(shareFileNode);
            SMIOS_FMT_DEBUG( "shareFileNodeLen = %d\n", shareFileNodeLen);
            pAppStartInfo->shareFileNodeCount = shareFileNodeLen;
            if (shareFileNodeLen > APP_SHARE_FILE_NODE_MAX_COUNT)
            {
                SMIOS_FMT_DEBUG("the share file node exceed the max count\n");
                return -1;
            }

            cJSON* device =NULL;
            permission= NULL;
            index = 0;
            //get the file node information and permission
            for (index = 0; index < shareFileNodeLen; index++)
            {
                device     = cJSON_GetArrayItem(shareFileNode, index);
                permission = cJSON_GetArrayItem(shareFileNode, index);

                device     = cJSON_GetObjectItem(device, "device");
                permission = cJSON_GetObjectItem(permission, "permission");

                char* tmpDevice = device->valuestring;
                char* tmpPermission = permission->valuestring;

                SMIOS_FMT_DEBUG( "device:%s, permisson:%s\n", tmpDevice, tmpPermission);
                strncpy(pAppStartInfo->shareFileNode[index].fileNode, tmpDevice, APP_FILE_NODE_NAME_MAX_LEN);
                strncpy(pAppStartInfo->shareFileNode[index].permission, tmpPermission, APP_PERMISSION_ARRAY_MAX_LEN);
            }
            appStartTbl->put(appStartTbl, pAppStartInfo->appName, pAppStartInfo, sizeof(APP_START_INFO_T));
        }
    }else
    {
        fclose(fp);
        fp = NULL;
        return -1;
    }

    return 0;
}

/**
*********************************************************************
* @brief：从app register文件中读取数据，并将数据放到启动列表中
* @param[in]： 
               none
* @param[out]：
               none
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_register_list_parse(void)
{
    FILE* fp = NULL;
    char buf[APP_REGISTER_LIST_MAX_SIZE] = {0};
    SMIOS_FMT_DEBUG("start parse the app register list\n");
    if (NULL == appRegisterTbl)
    {
        SMIOS_FMT_DEBUG("app register hash table is invalid\n");
        return -1;
    }

    //打开注册列表
    if ((fp = fopen(g_configObj.serviceCfg.registerListFile, "r")) == NULL)
    { 
        SMIOS_FMT_DEBUG("Can not open file[%s]\n", g_configObj.serviceCfg.registerListFile);
        return -1;
    }

    SMIOS_FMT_DEBUG( "Open file[%s] success\n", g_configObj.serviceCfg.registerListFile);
    //读注册列表文件内容
    sint32 ret = fread(buf, 1, APP_REGISTER_LIST_MAX_SIZE, fp);

    if (ret > 0)
    {
        fclose(fp);
        fp = NULL;

        //处理json字符串
        cJSON* root = cJSON_Parse(buf);
        if (NULL == root)
        {
            SMIOS_FMT_DEBUG("Parse app register list error\n");
            return -1; 
        }
        //获取根节点
        cJSON* value = cJSON_GetObjectItem(root, "app_info");
        if (value == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        //获取数组中元素数量
        sint32 len = cJSON_GetArraySize(value);
        SMIOS_FMT_DEBUG("app count = %d\n", len);

        cJSON* appName          = NULL;
        cJSON* protocolType     = NULL;
        cJSON* ip               = NULL;
        cJSON* port             = NULL;
        cJSON* pointer          = NULL;
        APP_REGISTER_INFO_T tmpAppRegisterInfo;

        //解析数组中app信息
        sint32 i = 0;
        for( i = 0; i < len; i++ )
        {
            memset(&tmpAppRegisterInfo, 0, sizeof(APP_REGISTER_INFO_T));
            APP_REGISTER_INFO_T* pAppRegisterInfo = &tmpAppRegisterInfo;

            pointer          = cJSON_GetArrayItem(value, i);
            //app名称
            appName          = cJSON_GetObjectItem(pointer, "app_name");
            //协议类型
            protocolType     = cJSON_GetObjectItem(pointer, "protocol_type");
            //ip
            ip               = cJSON_GetObjectItem(pointer, "IP");
            //端口号
            port             = cJSON_GetObjectItem(pointer, "port");

            //信息有效性判断
            if (NULL == appName || NULL == protocolType || NULL == ip || NULL == port)
            {
                SMIOS_FMT_DEBUG("the config file is invalid\n");
                return -1;
            }
            SMIOS_FMT_DEBUG( "appName[%s], protocolType[%d], ip[%s], port[%d]\n",
                        appName->valuestring, atoi(protocolType->valuestring), ip->valuestring, atoi(port->valuestring));

            //更新时间戳
            struct timespec tv;
            clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
            pAppRegisterInfo->tv.tv_sec = tv.tv_sec;
            pAppRegisterInfo->tv.tv_nsec = tv.tv_nsec;
            pAppRegisterInfo->pid = -1; //设置为-1无效值，需要收到注册信息后修改为争取的pid
            strncpy(pAppRegisterInfo->appName, appName->valuestring, APP_NAME_LENGTH);
            pAppRegisterInfo->protocolInfo.protocolType = atoi(protocolType->valuestring);
            strncpy(pAppRegisterInfo->protocolInfo.udp.ip, ip->valuestring, APP_IP_MAX_LENGTH);
            pAppRegisterInfo->protocolInfo.udp.port = atoi(port->valuestring);

            //添加到注册列表
            appRegisterTbl->put(appRegisterTbl, pAppRegisterInfo->appName, pAppRegisterInfo, sizeof(APP_REGISTER_INFO_T));
        }
    }else
    {
        //关闭文件
        fclose(fp);
        fp = NULL;
        return -1;
    }

    return 0;
}

/**
*********************************************************************
* @brief：启动app启动列表中的app
* @param[in]： 
               appStartListInfo  app启动信息
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_all_start(void)
{
    SMIOS_FMT_DEBUG("start all the app in the start list\n");
    if (NULL == appStartTbl)
    {
        SMIOS_FMT_DEBUG("The app start table is inavlid\n");
        return 0;
    }

    //获得启动列表中app数量
    uint32 tableCount = appStartTbl->size(appStartTbl);
    if (0 == tableCount)
    {
        SMIOS_FMT_DEBUG("there is no app need to start\n");
        return 0;
    }

    //遍历启动列表，启动app
    APP_START_INFO_T* pAppStartInfo = NULL;
    qhashtbl_obj_t obj;
    memset(&obj, 0, sizeof(obj));
    appStartTbl->lock(appStartTbl);
    while (appStartTbl->getnext(appStartTbl, &obj, true) == true)
    {
        pAppStartInfo = (APP_START_INFO_T*)(obj.data);
        //启动app
        app_start(pAppStartInfo);
        free(obj.data);
        free(obj.name);
    }
    appStartTbl->unlock(appStartTbl);

    return 0;
}

/**
*********************************************************************
* @brief：app启动
* @param[in]： 
               appStartListInfo  app启动信息
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_start(APP_START_INFO_T* appStartInfo)
{
    SMIOS_FMT_TRACE("%s\n", __FUNCTION__);
    //有效性检查
    if (NULL == appStartInfo->appName || strlen(appStartInfo->appName) == 0)
    {
        SMIOS_FMT_DEBUG("Invalid app!!\n");
        return 0;
    }

    //SMIOS_FMT_DEBUG("app name: %s",          appStartInfo->appName);
    //SMIOS_FMT_DEBUG("docker name: %s",       appStartInfo->dockerName);
    //SMIOS_FMT_DEBUG("uninstall flag: %d",    appStartInfo->uninstallFlag);
    //SMIOS_FMT_DEBUG("global local dir[%s]",  g_configObj.serviceCfg.localShareDir);
    //SMIOS_FMT_DEBUG("global docker dir[%s]", g_configObj.serviceCfg.dockerShareDir);

    //检查卸载标记。如果该标记为卸载，则表示卸载过程未完成，进入卸载流程
    if (appStartInfo->uninstallFlag == UNINSTALL_FLAG_TRUE)
    {
        //This app need to uninstall
        SMIOS_FMT_DEBUG("uninstall app[%s], deleteType[%d]\n", appStartInfo->appName, appStartInfo->deleteType);
        app_uninstall(appStartInfo->appName, appStartInfo->deleteType);
        return 0;
    }

    //检查app是否在容器中运行
    if (strlen(appStartInfo->dockerName) == 0 || strlen(appStartInfo->containerName) == 0)
    {
        SMIOS_FMT_DEBUG( "The app[%s] need not to run in container\n", appStartInfo->appName);
        char command[256] = {0};
        sprintf(command, "%s -d", appStartInfo->appName);
        system(command);
        return 0;
    }

    //get the container table
    qhashtbl_t* appContainerTbl = docker_manager_get_container_table_head();
    if (NULL == appContainerTbl)
    {
        SMIOS_FMT_DEBUG("the container table is invalid\n");
        return 0;
    }

    bool dockerFound = false;
    CONTAINER_INFO_T* pContainerInfo = NULL;
    //check whether the docker is running. If the doker is running, no need to start the docker
    //just use the dokcer's id
    size_t size = 0;
    pContainerInfo = appContainerTbl->get(appContainerTbl, appStartInfo->containerName, &size, true);
    if (NULL == pContainerInfo)
    {
        SMIOS_FMT_DEBUG("the container[%s] is not running\n", appStartInfo->containerName);
        dockerFound = false;
    } else {
        dockerFound = true;
        SMIOS_FMT_DEBUG("the container[%s] is running\n", appStartInfo->containerName);
        //the pContaienrInfo will be freed later
    }

    if (dockerFound == true)
    {
        //check the container whether is running
        sint32 ret = app_contaienr_is_running(appStartInfo->containerName);
        if (ret < 0)
        {
            dockerFound = false;
            free(pContainerInfo);
            pContainerInfo = NULL;
            appContainerTbl->remove(appContainerTbl, appStartInfo->containerName);
        }
    }

    //need to new container
    if (dockerFound == false)
    {
        //get the docker mirror image info
        qhashtbl_t* mirrorTbl = docker_manager_get_mirror_table_head();
        if (NULL == mirrorTbl)
        {
            SMIOS_FMT_DEBUG("the docker mirror table is null, so can not start app[%s]\n", appStartInfo->appName);
            return -1;
        }
        size = 0;
        DOCKER_INFO_T* pDockerInfo = mirrorTbl->get(mirrorTbl, appStartInfo->dockerName, &size, true);
        if (NULL == pDockerInfo)
        {
            SMIOS_FMT_DEBUG("there is no docker mirror[%s], so can not start app[%s]\n", appStartInfo->dockerName,
            appStartInfo->appName);
            return -1;
        }

        //启动容器
        sint32 ret = docker_manager_docker_start(pDockerInfo, appStartInfo->containerName);
        free(pDockerInfo);

        if (ret < 0)
        {
            SMIOS_FMT_DEBUG( "the container[%s] start fail\n", appStartInfo->containerName);
            return -1;
        }
    }

    //The array save the result of command
    char result[APP_CMD_RESULT_MAX_LENGTH + 1];
    memset(result, 0, APP_CMD_RESULT_MAX_LENGTH + 1);

    //The array save the command
    char command[APP_CMD_MAX_LENGTH + 1];
    memset(command, 0, APP_CMD_MAX_LENGTH + 1);

    //The array save the app start command
    char appStartCmd[APP_START_CMD_LENGTH + 1];
    memset(appStartCmd, 0, APP_START_CMD_LENGTH + 1);

    //The docker is running, so just use the id of docker
    //SMIOS_FMT_DEBUG("the docker[%s] is running", appStartInfo->dockerName);
    sprintf(command, "docker exec -d %s  /bin/bash %s\n", appStartInfo->containerName, appStartInfo->dataPath);
    //SMIOS_FMT_DEBUG( "cmd : %s", command);

    app_cmd_execute(command, result);
    //SMIOS_FMT_DEBUG( "result : %s", result);

    if (NULL != pContainerInfo)
    {
        free(pContainerInfo);
    }

    return 0;
}

/**
*********************************************************************
* @brief：执行app模块指令
* @param[in]： 
               cmd  执行的命令
* @param[out]：
               result 命令执行结果
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_cmd_execute(const char* cmd,  char* result)
{
    char resultBuf[APP_CMD_RESULT_MAX_LENGTH + 1];
    char tmpCmd[APP_CMD_MAX_LENGTH + 1] = {0};
    FILE* fp = NULL;
    strncpy(tmpCmd, cmd, APP_CMD_MAX_LENGTH);
    if ((fp = popen(tmpCmd, "r")) != NULL)
    {
        while( fgets(resultBuf, APP_CMD_RESULT_MAX_LENGTH, fp) != NULL) {
            strcat(result, resultBuf);
            if( strlen(result) > APP_CMD_RESULT_MAX_LENGTH)
            {
                break;
            }
        }
        pclose(fp);
        fp = NULL;
    } else {
        SMIOS_FMT_DEBUG("popen %s error\n", tmpCmd);
        return -1;
    }
    return 0;
}

/**
*********************************************************************
* @brief：app安装包config文件解析
* @param[in]： 
               appName  app的名称
* @param[out]：
               appStartListInfo 保存app config信息
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_config_parse(char* appName, char* appConfigFile, APP_START_INFO_T* appStartInfo)
{
    SMIOS_FMT_DEBUG("parse app config\n");
    //检查参数有效性
    if (NULL == appName || NULL == appStartInfo)
    {
        SMIOS_FMT_DEBUG("Invalid input parameter!!!\n");
        return -1;
    }

    //判断app名称有效性
    if(strlen(appName) > APP_NAME_LENGTH)
    {
        SMIOS_FMT_DEBUG("the app name exceed the max length[%d]\n", APP_NAME_LENGTH);
        return -1;
    }

    //SMIOS_FMT_DEBUG("app config[%s]", appConfigFile);

    //open the app config file
    FILE* fp = fopen(appConfigFile, "r");
    if (NULL == fp)
    {
        SMIOS_FMT_DEBUG("can not open file[%s]\n", appConfigFile);
        return -1;
    }

    char fileContext[APP_CONFIG_FILE_MAX_SIZE + 1];
    memset(fileContext, 0, APP_CONFIG_FILE_MAX_SIZE + 1);

    //读取配置文件内容
    sint32 ret = fread(fileContext, 1, APP_CONFIG_FILE_MAX_SIZE, fp);
    if (ret > 0)
    {
        fclose(fp);
        fp = NULL;

        //处理json格式字符串
        cJSON* root = cJSON_Parse(fileContext);
        if (NULL == root)
        {
            SMIOS_FMT_DEBUG("parse app config file error\n");
            return -1; 
        }
        //获取根节点
        cJSON* value = cJSON_GetObjectItem(root, "config");
        if (value == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        //char* app_info = cJSON_Print(value);
        //SMIOS_FMT_DEBUG( "app_info = %s", app_info);

        cJSON* tmpAppName       = NULL;
        cJSON* version          = NULL;
        cJSON* author           = NULL;
        cJSON* dockerName       = NULL;
        cJSON* containerName    = NULL;
        cJSON* startParam       = NULL;
        cJSON* dataPath         = NULL;
        cJSON* alivePeriod      = NULL;
        cJSON* protocolType     = NULL;
        cJSON* port             = NULL;
        cJSON* topic            = NULL;
        cJSON* shareDir         = NULL;
        cJSON* shareFileNode    = NULL;
        cJSON* net              = NULL;
        cJSON* exposePort       = NULL;
        cJSON* memThreshold     = NULL;
        cJSON* cpuRateThreshold = NULL;
        cJSON* flashSizeThreshold = NULL;
        APP_START_INFO_T* pAppStartInfo = appStartInfo;

        tmpAppName       = cJSON_GetObjectItem(value, "app_name");
        version          = cJSON_GetObjectItem(value, "version");
        author           = cJSON_GetObjectItem(value, "author");
        dockerName       = cJSON_GetObjectItem(value, "docker_name");
        containerName    = cJSON_GetObjectItem(value, "container_name");
        startParam       = cJSON_GetObjectItem(value, "start_param");
        dataPath         = cJSON_GetObjectItem(value, "dataPath");
        alivePeriod      = cJSON_GetObjectItem(value, "alivePeriod");
        protocolType     = cJSON_GetObjectItem(value, "protocolType");
        port             = cJSON_GetObjectItem(value, "port");
        topic            = cJSON_GetObjectItem(value, "topic");
        net              = cJSON_GetObjectItem(value, "net");

        //检查信息有效性
        if( NULL == tmpAppName || NULL == version ||
            NULL == author || NULL == dockerName ||
            NULL == startParam || NULL == dataPath ||
            NULL == alivePeriod || NULL == protocolType ||
            NULL == port || NULL == topic || NULL == net)
        { 
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
 
        SMIOS_FMT_DEBUG( "%s\t%s\t%s\t%s\t%s\n%s\t%s\t%s\t%s\t%s\t%s\n", tmpAppName->valuestring, version->valuestring,
                    author->valuestring, dockerName->valuestring, startParam->valuestring, alivePeriod->valuestring,
                    protocolType->valuestring, port->valuestring, topic->valuestring, net->valuestring, dataPath->valuestring);

        strncpy(pAppStartInfo->appName,       tmpAppName->valuestring, APP_NAME_LENGTH);
        strncpy(pAppStartInfo->version,       version->valuestring,    APP_VERSION_LENGTH);
        strncpy(pAppStartInfo->dockerName,    dockerName->valuestring, APP_DOCKER_NAME_LENGTH);
        strncpy(pAppStartInfo->containerName, containerName->valuestring, APP_DOCKER_NAME_LENGTH);
        strncpy(pAppStartInfo->startParam,    startParam->valuestring, APP_START_PARAM_LENGTH);
        strncpy(pAppStartInfo->dataPath,      dataPath->valuestring, APP_DATA_PATH_LENGTH);
        pAppStartInfo->uninstallFlag = UNINSTALL_FLAG_FALSE;

        //获取网路模式
        char* tmpNet = net->valuestring;
        //SMIOS_FMT_DEBUG( "Net: %s", tmpNet);
        if (strcmp(tmpNet, "bridge") == 0)
        {
            SMIOS_FMT_DEBUG( "net bridge\n");
            pAppStartInfo->net = APP_NET_BRIDEG;
        } else if(strcmp(tmpNet, "host") == 0)
        {
            SMIOS_FMT_DEBUG( "net host\n");
            pAppStartInfo->net = APP_NET_CONTAINER_HOST;
        }

        //暴露的端口号
        exposePort = cJSON_GetObjectItem(value, "exposePort");
        if (exposePort == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        char* tmpExposePort = exposePort->valuestring;
        //SMIOS_FMT_DEBUG( "expose port: %s", tmpExposePort);
        strncpy(pAppStartInfo->exposePort, tmpExposePort, APP_EXPOSE_PORT_LENGTH);

        //内存阈值
        memThreshold= cJSON_GetObjectItem(value, "memThreshold");
        if (memThreshold == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        char* tmpMemThreshold = memThreshold->valuestring;
        //SMIOS_FMT_DEBUG( "memThreshold: %s", tmpMemThreshold);
        strncpy(pAppStartInfo->memThreshold, tmpMemThreshold, APP_MEM_LIMT_LENGTH);

        //cpu占用率阈值
        cpuRateThreshold= cJSON_GetObjectItem(value, "cpuRateThreshold\n");
        if (cpuRateThreshold == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        char* tmpCpuRateThreshold = cpuRateThreshold->valuestring;
        //SMIOS_FMT_DEBUG( "cpuRateThreshold: %s", tmpCpuRateThreshold);
        strncpy(pAppStartInfo->cpuRateThreshold, tmpCpuRateThreshold, APP_CPU_RATE_LIMIT_LEN);

        //flash占用率阈值
        flashSizeThreshold= cJSON_GetObjectItem(value, "flashSizeThreshold");
        if (flashSizeThreshold == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        char* tmpFlashSizeThreshold = flashSizeThreshold->valuestring;
        //SMIOS_FMT_DEBUG( "flashSizeThreshold: %s", tmpFlashSizeThreshold);
        strncpy(pAppStartInfo->flashSizeThreshold, tmpFlashSizeThreshold, APP_FLASH_SIZE_LIMIT_LEN);

        //映射目录
        shareDir = cJSON_GetObjectItem(value, "ShareDir");
        if (shareDir == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        //char* shareDirStr = shareDir->valuestring;
        //SMIOS_FMT_DEBUG( "shareDir: %s", shareDirStr);

        //获取映射目录数量
        int shareDirLen = cJSON_GetArraySize(shareDir);
        //SMIOS_FMT_DEBUG( "shareDirLen = %d", shareDirLen);
        pAppStartInfo->shareDirCount = shareDirLen;
        if (shareDirLen > APP_SHARE_DIR_MAX_COUNT)
        {
            SMIOS_FMT_DEBUG("the share directory count exceed the max count\n");
            return -1;
        }

        cJSON* directory =NULL;
        cJSON* permission= NULL;
        int index = 0;
        //获取映射目录及其权限
        for (index = 0; index < shareDirLen; index++)
        {
            directory = cJSON_GetArrayItem(shareDir, index);
            permission = cJSON_GetArrayItem(shareDir, index);

            directory = cJSON_GetObjectItem(directory, "directory");
            permission = cJSON_GetObjectItem(permission, "permission");

            char* tmpDirectory = directory->valuestring;
            char* tmpPermission = permission->valuestring;

            //SMIOS_FMT_DEBUG( "directory:%s, permisson:%s", tmpDirectory, tmpPermission);
            strncpy(pAppStartInfo->shareDir[index].directory, tmpDirectory, APP_DIR_ARRAY_MAX_LENGTH);
            strncpy(pAppStartInfo->shareDir[index].permission, tmpPermission, APP_PERMISSION_ARRAY_MAX_LEN);
        }

        //获取映射文件节点
        shareFileNode = cJSON_GetObjectItem(value, "ShareFileNode");
        if (shareFileNode == NULL)
        {
            SMIOS_FMT_DEBUG( "GetObjectItem error\n");
            return -1;
        }

        //char* shareFileNodeStr = shareFileNode->valuestring;
        //SMIOS_FMT_DEBUG( "shareFileNode: %s", shareFileNodeStr);

        //获取映射文件节点数量
        int shareFileNodeLen = cJSON_GetArraySize(shareFileNode);
        //SMIOS_FMT_DEBUG( "shareFileNodeLen = %d", shareFileNodeLen);
        pAppStartInfo->shareFileNodeCount = shareFileNodeLen;
        if (shareFileNodeLen > APP_SHARE_FILE_NODE_MAX_COUNT)
        {
            SMIOS_FMT_DEBUG("the share file node exceed the max count\n");
            return -1;
        }

        //获取映射文件节点及其权限
        cJSON* device =NULL;
        permission= NULL;
        index = 0;
        for (index = 0; index < shareFileNodeLen; index++)
        {
            device = cJSON_GetArrayItem(shareFileNode, index);
            permission = cJSON_GetArrayItem(shareFileNode, index);

            device = cJSON_GetObjectItem(device, "device");
            permission = cJSON_GetObjectItem(permission, "permission");

            char* tmpDevice = device->valuestring;
            char* tmpPermission = permission->valuestring;

            SMIOS_FMT_DEBUG( "device:%s, permisson:%s\n", tmpDevice, tmpPermission);
            strncpy(pAppStartInfo->shareFileNode[index].fileNode, tmpDevice, APP_FILE_NODE_NAME_MAX_LEN);
            strncpy(pAppStartInfo->shareFileNode[index].permission, tmpPermission, APP_PERMISSION_ARRAY_MAX_LEN);
        }

    } else {
        //关闭文件
        fclose(fp);
        fp = NULL;
        return -1;
    }

    return 0;
}

/**
*********************************************************************
* @brief：保存app启动列表
* @param[in]： 
               appStartListInfo  app启动列表头指针
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_start_list_save(void)
{
    SMIOS_FMT_DEBUG("save the app start list...\n");
    FILE* fp = NULL;

    //启动列表是否有效
    if (NULL == appStartTbl)
    {
        SMIOS_FMT_DEBUG("the app start table is NULL\n");
        return -1;
    }

    //打开启动列表配置文件
    fp = fopen(g_configObj.serviceCfg.startListFile , "w");
    if (NULL == fp)
    {
        SMIOS_FMT_DEBUG("open app start list file failed!!!\n");
        return -1;
    }
    char recordContext[APP_RECORD_MAX_LENGTH + 1];
    memset(recordContext, 0, APP_RECORD_MAX_LENGTH);

    strncpy(recordContext, "{\"app_info\" :[\n", APP_RECORD_MAX_LENGTH);
    fwrite(recordContext, 1, strlen(recordContext), fp);
    APP_START_INFO_T* pAppStartInfo = NULL;
    qhashtbl_obj_t obj;
    uint32 appStartInfoCount = 0;
    memset(&obj, 0, sizeof(qhashtbl_obj_t));
    appStartTbl->lock(appStartTbl);
    while (appStartTbl->getnext(appStartTbl, &obj, true) == true) {
        pAppStartInfo = (APP_START_INFO_T*)obj.data;
        appStartInfoCount = appStartInfoCount + 1;
        //保存app启动信息
        app_start_info_save(pAppStartInfo, recordContext);
        if (appStartInfoCount < appStartTbl->size(appStartTbl))
        {
            strcat(recordContext, "]},\n");
        } else {
            strcat(recordContext, "]}\n");
        }
        SMIOS_FMT_DEBUG( "record context : %s\n", recordContext);
        fwrite(recordContext, 1, strlen(recordContext), fp);
        free(obj.name);
        free(obj.data);
    }
    appStartTbl->unlock(appStartTbl);

    memset(recordContext, 0, APP_RECORD_MAX_LENGTH);
    strncpy(recordContext, "]}", APP_RECORD_MAX_LENGTH);
    fwrite(recordContext, 1, strlen(recordContext), fp);

    fclose(fp);
    fp = NULL;
    return 0;
}

/**
*********************************************************************
* @brief：获取app启动信息
* @param[in]： 
               appName  
* @param[out]：
               appStartInfo
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_info_get(char* appName, APP_START_INFO_T* appStartInfo)
{
    SMIOS_FMT_DEBUG( __FUNCTION__);
    if (NULL == appName || NULL == appStartInfo)
    {
        SMIOS_FMT_DEBUG("invalid param\n");
        return -1;
    }

    size_t size = 0;
    APP_START_INFO_T* pAppStartInfo = appStartTbl->get(appStartTbl, appName, &size, true);
    if (NULL == pAppStartInfo)
    {
        SMIOS_FMT_DEBUG("not find the app[%s]\n", appName);
        return -1;
    }
    memcpy(appStartInfo, pAppStartInfo, sizeof(APP_START_INFO_T));

    free(pAppStartInfo);
    return 0;
}

/**
*********************************************************************
* @brief：app启动信息保存
* @param[in]： 
               pAppStartInfo
               recordContext
* @param[out]：
               none
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_start_info_save(APP_START_INFO_T* pAppStartInfo, char* recordContext)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    if (NULL == pAppStartInfo || NULL == recordContext)
    {
        SMIOS_FMT_DEBUG("%s, invalid parameter\n", __FUNCTION__);
        return -1;
    }

    sprintf(recordContext, "{\"app_name\":\"%s\", \"version\":\"%s\", \"docker_name\":\"%s\", \"container_name\":\"%s\", \"start_param\":\"%s\", \"dataPath\":\"%s\", \"uninstall_flag\":\"%d\", \
                    \"net\":\"%d\", \"exposePort\":\"%s\", \"memThreshold\":\"%s\", \"cpuRateThreshold\":\"%s\", \"flashSizeThreshold\":\"%s\", \"ShareDir\":[\n",
            pAppStartInfo->appName, pAppStartInfo->version, pAppStartInfo->dockerName, pAppStartInfo->containerName, pAppStartInfo->startParam, pAppStartInfo->dataPath,
            pAppStartInfo->uninstallFlag, pAppStartInfo->net, pAppStartInfo->exposePort, pAppStartInfo->memThreshold, pAppStartInfo->cpuRateThreshold,
            pAppStartInfo->flashSizeThreshold);

    if (pAppStartInfo->shareDirCount > 0)
    {
        int index = 0;
        char shareDir[512] = {0};
        for (index = 0; index < pAppStartInfo->shareDirCount - 1; index++)
        {
            memset(shareDir, 0, 512);
            sprintf(shareDir, "{\"directory\":\"%s\", \"permission\":\"%s\"},", pAppStartInfo->shareDir[index].directory,
                    pAppStartInfo->shareDir[index].permission);
            strcat(recordContext, shareDir);
        }
        memset(shareDir, 0, 512);
        sprintf(shareDir, "{\"directory\":\"%s\", \"permission\":\"%s\"}", pAppStartInfo->shareDir[index].directory,
                pAppStartInfo->shareDir[index].permission);
        strcat(recordContext, shareDir);
    } 
    strcat(recordContext, "],");
    strcat(recordContext, "\"ShareFileNode\":[");
    if (pAppStartInfo->shareFileNodeCount > 0)
    {
        int index = 0;
        char shareDir[512] = {0};
        for (index = 0; index < pAppStartInfo->shareFileNodeCount - 1; index++)
        {
            sprintf(shareDir, "{\"device\":\"%s\", \"permission\":\"%s\"},", pAppStartInfo->shareFileNode[index].fileNode,
                    pAppStartInfo->shareFileNode[index].permission);
            strcat(recordContext, shareDir);
            memset(shareDir, 0, 512);
        }
        sprintf(shareDir, "{\"device\":\"%s\", \"permission\":\"%s\"}", pAppStartInfo->shareFileNode[index].fileNode,
                pAppStartInfo->shareFileNode[index].permission);
        strcat(recordContext, shareDir);
    }

    return 0;
}

/**
*********************************************************************
* @brief：app注册信息保存
* @param[in]： 
               pAppRegisterInfo
               recordContext
* @param[out]：
               none
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_register_info_save(APP_REGISTER_INFO_T* pAppRegisterInfo, char* recordContext)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    if (NULL == pAppRegisterInfo || NULL == recordContext)
    {
        SMIOS_FMT_DEBUG("%s, invalid parameter\n", __FUNCTION__);
        return -1;
    }

    //组装app注册信息json字符串
    sprintf(recordContext, "{\"app_name\":\"%s\", \"protocol_type\":\"%d\", \"IP\":\"%s\", \"port\":\"%d\"", 
            pAppRegisterInfo->appName,
            pAppRegisterInfo->protocolInfo.protocolType,
            pAppRegisterInfo->protocolInfo.udp.ip,
            pAppRegisterInfo->protocolInfo.udp.port);

    return 0;
}

/**
*********************************************************************
* @brief：添加注册信息到注册列表
* @param[in]： 
               appRegisterInfo  app注册信息
* @param[out]：
               appRegisterInfoList  app注册列表头指针
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_register_list_add(APP_REGISTER_INFO_T* appRegitsterInfo)
{
    LOGIN_FMT_DEBUG("add app info to register list\n");
    if (NULL == appRegisterTbl)
    {
        LOGIN_FMT_DEBUG("the app register table is invalid\n");
        return 0;
    }

    // add the app register information
    bool ret = appRegisterTbl->put(appRegisterTbl, appRegitsterInfo->appName, appRegitsterInfo, sizeof(APP_REGISTER_INFO_T));
    if (false == ret)
    {
        LOGIN_FMT_DEBUG("add app[%s] to register list failed\n", appRegitsterInfo->appName);
        return -1;
    }

    LOGIN_FMT_DEBUG("add app[%s] to register list success\n", appRegitsterInfo->appName);
    //保存注册列表信息
    registerFileNeedUpdate = 1;
    LOGIN_FMT_DEBUG( "%s, ====need to save app register file\n", __FUNCTION__);
    return 0;
}

/**
*********************************************************************
* @brief：更新注册信息到注册列表
* @param[in]： 
               appRegisterInfo  app注册信息
* @param[out]：
               appRegisterInfoList  app注册列表头指针
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_register_list_update(APP_REGISTER_INFO_T* appRegitsterInfo)
{
    //SMIOS_FMT_DEBUG("update app info to register list");
    if (NULL == appRegisterTbl)
    {
        SMIOS_FMT_DEBUG("the app register table is invalid\n");
        return 0;
    }

    // update the app register information
    bool ret = appRegisterTbl->put(appRegisterTbl, appRegitsterInfo->appName, appRegitsterInfo, sizeof(APP_REGISTER_INFO_T));
    if (false == ret)
    {
        SMIOS_FMT_DEBUG("update app[%s] to register list failed\n", appRegitsterInfo->appName);
        return -1;
    }

    SMIOS_FMT_TRACE( "update app[%s] to register list success\n", appRegitsterInfo->appName);
    return 0;
}
/**
*********************************************************************
* @brief：从注册列表中删除指定的app
* @param[in]： 
               appName  app名称
* @param[out]：
               appRegisterInfoList  app注册列表头指针
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_register_list_delete(char* appName)
{
    if (NULL == appName)
    {
        SMIOS_FMT_DEBUG("Invalid app name\n");
        return -1;
    }
    if (NULL == appRegisterTbl)
    {
        SMIOS_FMT_DEBUG("the app register table is invalid\n");
        return -1;
    }

    //获取注册表中app数量
    size_t appCount = app_register_app_count_get();
    if (appCount == 0)
    {
        SMIOS_FMT_DEBUG("there is no app in register table\n");
        return 0;
    }

    //删除指定app注册信息
    bool result = appRegisterTbl->remove(appRegisterTbl, appName);
    if (false == result)
    {
        SMIOS_FMT_DEBUG("Not find the app name[%s] in the register list\n", appName);
        return -1;
    }

    LOGIN_FMT_TRACE("delete app[%s] from the app register table success\n", appName);
    //保存注册列表信息
	alive_app_name_del(appName);
    //设置注册列表变化标记
    registerFileNeedUpdate = 1;
    SMIOS_FMT_TRACE( "%s, ====need to save app register file\n", __FUNCTION__);
    return 0;
}

/**
*********************************************************************
* @brief：保存注册列表
* @param[in]： 
               none
* @param[out]：
               none
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
 void* app_register_list_save( void* arg)
{
    while(1) {
        sleep(35);
        //注册列表有变化
        if (registerFileNeedUpdate) {
            registerFileNeedUpdate = 0;
            SMIOS_FMT_DEBUG( "save the app register list...\n");
            FILE* fp = NULL;
            //判断注册列表有效性
            if (NULL == appRegisterTbl)
            {
                SMIOS_FMT_DEBUG("the app start table is NULL\n");
                continue;
            }
            //打开注册列表配置文件
            fp = fopen(g_configObj.serviceCfg.registerListFile, "w");
            if (NULL == fp)
            {
                SMIOS_FMT_DEBUG("open app register list file failed!!!\n");
                continue;
            }
            char recordContext[APP_RECORD_MAX_LENGTH + 1];
            memset(recordContext, 0, APP_RECORD_MAX_LENGTH);

            strncpy(recordContext, "{\"app_info\" :[\n", APP_RECORD_MAX_LENGTH);
            fwrite(recordContext, 1, strlen(recordContext), fp);
            APP_REGISTER_INFO_T* pAppRegisterInfo = NULL;
            qhashtbl_obj_t obj;
            uint32 appStartInfoCount = 0;
            memset(&obj, 0, sizeof(qhashtbl_obj_t));

            //遍历注册列表，保存配置列表信息
            //appRegisterTbl->lock(appRegisterTbl);
            while (appRegisterTbl->getnext(appRegisterTbl, &obj, true) == true) {
                pAppRegisterInfo = (APP_REGISTER_INFO_T*)obj.data;
                appStartInfoCount = appStartInfoCount + 1;
                app_register_info_save(pAppRegisterInfo, recordContext);
                if (appStartInfoCount < appRegisterTbl->size(appRegisterTbl))
                {
                    strcat(recordContext, "},\n");
                } else {
                    strcat(recordContext, "}\n");
                }
                SMIOS_FMT_DEBUG( "record context : %s\n", recordContext);
                fwrite(recordContext, 1, strlen(recordContext), fp);
                //释放空间
                free(obj.name);
                free(obj.data);
            }
            //appRegisterTbl->unlock(appRegisterTbl);

            memset(recordContext, 0, APP_RECORD_MAX_LENGTH);
            strncpy(recordContext, "]}", APP_RECORD_MAX_LENGTH);

            //保存信息
            fwrite(recordContext, 1, strlen(recordContext), fp);

            fclose(fp);
            fp = NULL;
            SMIOS_FMT_DEBUG( "save the app register list end\n");
        }
    }

}

/**
*********************************************************************
* @brief：创建注册列表保存线程
* @param[in]： 
               none
* @param[out]：
               none
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
void createAppRegisterFileSaveThread(void)
{
    pthread_t tidp;
    if ((pthread_create(&tidp, NULL, app_register_list_save, NULL)) == -1)
    {
        SMIOS_FMT_DEBUG("create save app register file thread failed\n");
    } else {
        SMIOS_FMT_DEBUG("create save app register file thread success\n");
    }

    return;
}
/**
*********************************************************************
* @brief： 注册表中app数量
* @param[in]： 
               none
* @param[out]：
               none
* @return： 返回app count 正常退出
            返回0         处理异常
*********************************************************************
*/
uint32 app_register_app_count_get(void)
{
    SMIOS_FMT_TRACE(" %s\n", __FUNCTION__);
    if (NULL == appRegisterTbl)
    {
        SMIOS_FMT_DEBUG("the app register table is null\n");
        return 0;
    }
    //获取注册列表中app数量
    uint32 appCount = appRegisterTbl->size(appRegisterTbl);
    //SMIOS_FMT_DEBUG( "app count[%d]", appCount);
    return appCount;
}

/**
*********************************************************************
* @brief： 获取app启动信息
* @param[in]： 
               appName
* @param[out]：
               none
* @return： 返回 APP启动信息指针 正常退出
            返回 NULL         处理异常
*********************************************************************
*/
APP_START_INFO_T* app_get_app_start_info(const char* appName)
{
    //检查参数有效性
    if (NULL == appName)
    {
        SMIOS_FMT_DEBUG("the app name is NULL\n");
        return NULL;
    }
    //检测启动列表有效性
    if (NULL == appStartTbl)
    {
        SMIOS_FMT_DEBUG("The app start table is NULL\n");
        return NULL;
    }

    //获取app启动信息
    appStartTbl->lock(appStartTbl);
    size_t memSize = 0;
    char tmpAppName[64] = {0};
    char tmpProtocol[4] = {0};
    sscanf(appName, "%[^-]-%s", tmpProtocol, tmpAppName);
    APP_START_INFO_T* appStartInfo = appStartTbl->get(appStartTbl, tmpAppName, &memSize, true);
    appStartTbl->unlock(appStartTbl);

    if (NULL == appStartInfo)
    {
        SMIOS_FMT_DEBUG("can not find the app[%s] in the app start list\n", appName);
        return NULL;
    }

    return appStartInfo;
}

/**
*********************************************************************
* @brief： 获取app注册信息
* @param[in]： 
               appName
* @param[out]：
               none
* @return： 返回 APP注册信息指针 正常退出
            返回 NULL         处理异常
*********************************************************************
*/
APP_REGISTER_INFO_T* app_get_app_register_info(const char* appName)
{
    //参数有效性
    if (NULL == appName)
    {
        SMIOS_FMT_DEBUG("the app name is invalid\n");
        return NULL;
    }

    //检测注册列表有效性
    if (NULL == appRegisterTbl)
    {
        SMIOS_FMT_DEBUG("The app register table is NULL\n");
        return NULL;
    }

    //获取app注册信息
    size_t size = 0;
    APP_REGISTER_INFO_T* pAppRegisterInfo = appRegisterTbl->get(appRegisterTbl, appName, &size, true);
    if (NULL == pAppRegisterInfo)
    {
        SMIOS_FMT_DEBUG("The app[%s] is not in the register list\n", appName);
        return NULL;
    }

    return pAppRegisterInfo;
}

/**
*********************************************************************
* @brief： 容器中app名称获取
* @param[in]： containerName 容器名称
* @param[out]：appNameArray  app名称数组
               appCount      app数量
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 app_get_app_name_use_same_container(const char* containerName, char* appNameArray, sint32* appCount)
{
    SMIOS_FMT_DEBUG("%s", __FUNCTION__);
    //参数有效性
    if (NULL == containerName || NULL == appNameArray || NULL == appCount)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG( "%s, containerName[%s]\n", __FUNCTION__, containerName);
    //启动列表有效性
    if (NULL == appStartTbl)
    {
        SMIOS_FMT_DEBUG("%s, the app start table is null\n", __FUNCTION__);
        return -1;
    }

    //遍历启动列表查找使用同一个容器的app
    sint32 index = 0;
    qhashtbl_obj_t obj;
    char* pAppNameArray = appNameArray;
    memset(&obj, 0, sizeof(obj));
    appStartTbl->lock(appStartTbl);
    while (appStartTbl->getnext(appStartTbl, &obj, true) == true)
    {
        APP_START_INFO_T* pAppStartInfo = (APP_START_INFO_T*)obj.data;
        if (pAppStartInfo != NULL)
        {
            //使用同一容器的app
            if (strcmp(pAppStartInfo->containerName, containerName) == 0)
            {
                strcpy(pAppNameArray, pAppStartInfo->appName);
                index = index + 1;
                pAppNameArray = pAppNameArray + index * (APP_NAME_LENGTH + 1); 
            }

            free(obj.name);
            free(obj.data);
        }
    }
    appStartTbl->unlock(appStartTbl);
    *appCount = index;
    SMIOS_FMT_DEBUG( "%s, appCount[%d]\n", __FUNCTION__, *appCount);
    for(index = 0; index < *appCount; index++)
    {
        SMIOS_FMT_DEBUG( "%s, app name[%s]\n", __FUNCTION__, appNameArray + index * APP_NAME_LENGTH);
    }

    return 0;
}

/**
*********************************************************************
* @brief：停止app
* @param[in]： 
               appName  需要停止的app名称
               appRegisterInfoList  app注册列表头指针
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_stop(char* appName)
{
    SMIOS_FMT_TRACE(" %s\n", __FUNCTION__);
    //参数有效性
    if (NULL == appName || strlen(appName) == 0)
    {
        SMIOS_FMT_DEBUG("app name is invalid\n");
        return -1;
    }
    //启动列表有效性
    if (NULL == appStartTbl)
    {
        SMIOS_FMT_DEBUG("app start list is NULL\n");
        return -1;
    }

    size_t memSize = 0;
    char tmpAppName[64] = {0};
    char protocol;
    //根据注册app名称获取app协议类型以及app名称
    sscanf(appName, "%[^-]-%s", &protocol, tmpAppName);
    //查询启动列表
    APP_START_INFO_T* pAppStartInfo = (APP_START_INFO_T*)appStartTbl->get(appStartTbl, tmpAppName, &memSize, true);
    if (NULL == pAppStartInfo)
    {
        SMIOS_FMT_DEBUG("未在启动列表中找到[%s]\n", appName);
        //get the app pid from register table
        APP_REGISTER_INFO_T* pAppRegisterInfo = app_get_app_register_info(appName);
        if (pAppRegisterInfo != NULL) {

            SMIOS_FMT_DEBUG( "use another way to stop app[%s]\n", appName);
            //从注册列表中删除app
            app_register_list_delete(appName);
            //获取pid
            sint32 appPID = pAppRegisterInfo->pid;
            char command[128] = {0};
            //关闭app
            sprintf(command, "kill %d", appPID);
            system(command);
            free(pAppRegisterInfo);
            pAppRegisterInfo = NULL;
        } else {
            return -1;
        }
        return 0;
    }

    //app不在容器中运行
    if (strlen(pAppStartInfo->containerName) == 0 || strlen(pAppStartInfo->dockerName) == 0)
    {
        SMIOS_FMT_DEBUG("The app[%s] does not run in container\n", appName);
        //获取注册信息
        APP_REGISTER_INFO_T* pAppRegisterInfo = app_get_app_register_info(appName);
        if (pAppRegisterInfo != NULL) {

            SMIOS_FMT_DEBUG( "use another way to stop app[%s]\n", appName);
            //从注册列表中删除app
            app_register_list_delete(appName);
            sint32 appPID = pAppRegisterInfo->pid;
            char command[128] = {0};
            //关闭app
            sprintf(command, "kill %d", appPID);
            system(command);
            free(pAppRegisterInfo);
            pAppRegisterInfo = NULL;
        } else {
            return -1;
        }

        if (pAppStartInfo != NULL)
        {
            free(pAppStartInfo);
        }
        return 0;
    }

    //get the app progress id
    sint32 progressId = 0;
    APP_REGISTER_INFO_T* pAppRegisterInfo = app_get_app_register_info(appName);
    if (pAppRegisterInfo != NULL)
    {
        progressId = pAppRegisterInfo->pid;
        SMIOS_FMT_DEBUG( "the app[%s] pid[%d]\n", pAppRegisterInfo->appName, progressId);

        //kill the app
        char command[APP_CMD_MAX_LENGTH + 1] = {0};
        char result[APP_CMD_RESULT_MAX_LENGTH + 1] = {0};
        memset(command, 0, APP_CMD_MAX_LENGTH + 1);
        memset(result, 0, APP_CMD_RESULT_MAX_LENGTH + 1);

        //关闭容器中运行的app
        sprintf(command, "docker exec -i %s kill %d", pAppRegisterInfo->containerName, progressId);
        app_cmd_execute(command, result);
        //从注册列表中删除app
        app_register_list_delete(appName);

        free(pAppRegisterInfo);
    }

    if (NULL != pAppStartInfo)
    {
        free(pAppStartInfo);
    }
    return 0;
}

/**
*********************************************************************
* @brief：重启app
* @param[in]： 
               appName  需要重启的app名称
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_restart(char* appName)
{
    SMIOS_FMT_TRACE("%s\n", __FUNCTION__);
    //参数有效性检查
    if (NULL == appName)
    {
        SMIOS_FMT_DEBUG("the app name is NULL\n");
        return -1;
    }

    //停止app
    sint32 ret = app_stop(appName);
    if (-1 == ret)
    {
        SMIOS_FMT_DEBUG("app[%s] stop success\n", appName);
        return -1;
    }

    size_t size = 0;
    char tmpAppName[64] = {0};
    char tmpProtocol[4] = {0};
    //从app注册名称中获取协议类型和app名称
    sscanf(appName, "%[^-]-%s", tmpProtocol, tmpAppName);
    //获取app启动信息
    APP_START_INFO_T* pAppStartInfo = appStartTbl->get(appStartTbl, tmpAppName, &size, true);
    if (NULL == pAppStartInfo)
    {
        SMIOS_FMT_DEBUG("the app[%s] is not in start table\n", appName);
        return -1;
    }

    //启动app
    ret = app_start(pAppStartInfo);
    if (-1 == ret)
    {
        SMIOS_FMT_DEBUG("restart app[%s] failed\n", appName);
        free(pAppStartInfo);
        return -1;
    }

    LOGIN_FMT_DEBUG("重启[%s]成功\n", appName);
    free(pAppStartInfo);
    return 0;
}

/**
*********************************************************************
* @brief：安装app
* @param[in]： 
               appName  需要停止的app名称
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_install(char* appName)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    sint32 ret = 0;
 
    char command[APP_CMD_MAX_LENGTH + 1] = {0};
    char result[APP_CMD_RESULT_MAX_LENGTH + 1] = {0};
    //安装app
    sprintf(command, "%s %s/tmp/%s/%s.deb", "dpkg -i", g_configObj.serviceCfg.localAppInstallDir, appName, appName);
    SMIOS_FMT_DEBUG( "decompress cmd[%s]\n", command);
    app_cmd_execute(command, result);

    //app config parse
    APP_START_INFO_T appStartInfo;
    memset(&appStartInfo, 0, sizeof(APP_START_INFO_T));
    char appConfigFile[APP_DIRECTORY_MAX_LENGTH + APP_NAME_LENGTH + 1];
    memset(appConfigFile, 0, APP_DIRECTORY_MAX_LENGTH + APP_NAME_LENGTH + 1);
    snprintf(appConfigFile, APP_DIRECTORY_MAX_LENGTH + APP_NAME_LENGTH, "%s/tmp/%s/packageInfo", g_configObj.serviceCfg.localAppInstallDir, appName);
    //app配置信息处理
    ret = app_config_parse(appName, appConfigFile, &appStartInfo);
    if (-1 == ret)
    {
        SMIOS_FMT_DEBUG("parse app config failed\n");
        return ret;
    }

    //app start
    ret = app_start(&appStartInfo);
    if (-1 == ret)
    {
        SMIOS_FMT_DEBUG("app start failed\n");
        return ret;
    }

    //write start list
    ret = app_start_list_add(&appStartInfo);
    if (-1 == ret)
    {
        SMIOS_FMT_DEBUG("add app start info failed\n");
        return ret;
    }

    //save start list file
    ret = app_start_list_save();
    if (-1 == ret)
    {
        SMIOS_FMT_DEBUG("save app start info failed\n");
        return ret;
    }

    return ret;
}

/**
*********************************************************************
* @brief：升级app
* @param[in]： 
               appName  需要升级的app名称
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_upgrade(char* url, char* fileName, char* md5Sum)
{
    sint32 ret = -1;
    //参数有效性检查
    if (NULL == fileName || NULL == url || NULL == md5Sum)
    {
        SMIOS_FMT_DEBUG("the input param is invalid\n");
        return ret;
    }

    //下载app
    ret = app_download(url, fileName, md5Sum);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("download app[%s] fail\n", fileName);
        return ret;
    }

    //check whether the app is in the register list
    ret = app_uninstall(fileName, DELETE_TYPE_PART);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("app[%s] uninstall fail\n", fileName);
        return ret;
    }

    //app install
    app_install(fileName);
    //app安装包删除
    app_package_delete(fileName);
    return ret;
}

/**
*********************************************************************
* @brief：卸载app
* @param[in]： 
               appName  需要卸载的app名称
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_uninstall(char* appName, DELETE_TYPE deleteType)
{
    sint32 ret = -1;
    if (NULL == appName)
    {
        SMIOS_FMT_DEBUG("the app name is null\n");
        return ret;
    }
    SMIOS_FMT_DEBUG( "%s, app name[%s]\n", __FUNCTION__, appName);
    //检查app是否在注册列表中
    ret = app_register_list_check(appName);
    if (0 == ret)
    {
        SMIOS_FMT_DEBUG("%s, the app[%s] is running\n", __FUNCTION__, appName);
        //停止app
        ret = app_stop(appName);
        if (-1 == ret)
        {
            SMIOS_FMT_DEBUG("%s, the app[%s] stopped failed\n", __FUNCTION__, appName);
        }
    }

    //set the uninstall flag in the start list
    size_t size = 0;
    char tmpAppName[64] = {0};
    char tmpProtocol[4] = {0};
    //获取协议类型和app名称
    sscanf(appName, "%[^-]-%s", tmpProtocol, tmpAppName);
    APP_START_INFO_T* pAppStartInfo = appStartTbl->get(appStartTbl, tmpAppName, &size, true);
    //app不在启动列表中嗯
    if (NULL == pAppStartInfo)
    {
        SMIOS_FMT_DEBUG("%s, not find the app[%s] in the start list\n", __FUNCTION__, appName);
        SMIOS_FMT_DEBUG( "use another way to uninstall app[%s]\n", appName);
        char command[128] = {0};
        //全部删除
        if (DELETE_TYPE_ALL == deleteType)
        {
            //全部删除命令
            sprintf(command, "dpkg -P %s", tmpAppName);
            SMIOS_FMT_DEBUG( "delete all command[%s]\n", command);
            system(command);
        } else if (DELETE_TYPE_PART == deleteType) //部分删除
        {
            //部分删除命令
            sprintf(command, "dpkg -r %s", tmpAppName);
            SMIOS_FMT_DEBUG( "delete part command[%s]\n", command);
            system(command);
        } else {
            return -1;
        }
        return 0;
    }

    //设置卸载标记
    pAppStartInfo->uninstallFlag = UNINSTALL_FLAG_TRUE;
    //设置卸载类型
    pAppStartInfo->deleteType = deleteType;
    //app启动信息防止到启动列表
    bool result = appStartTbl->put(appStartTbl, tmpAppName, pAppStartInfo, sizeof(APP_START_INFO_T));
    if (true == result)
    {
        //保存启动列表信息
        app_start_list_save();
    }

    free(pAppStartInfo);
    pAppStartInfo = NULL;

    char command[APP_CMD_MAX_LENGTH + 1];
    char cmdResult[APP_CMD_RESULT_MAX_LENGTH + 1];
    memset(command, 0, APP_CMD_MAX_LENGTH+1);
    memset(cmdResult, 0, APP_CMD_RESULT_MAX_LENGTH + 1);
    if (DELETE_TYPE_ALL == deleteType) //全部删除
    {
        sprintf(command, "dpkg -P %s", tmpAppName);
        SMIOS_FMT_DEBUG( "%s, delete all command[%s]\n", __FUNCTION__, command);
        app_cmd_execute(command, cmdResult);
    } else if (DELETE_TYPE_PART == deleteType)  //部分删除
    {
        sprintf(command, "dpkg -r %s", tmpAppName);
        SMIOS_FMT_DEBUG( "%s, delete part command[%s]\n", __FUNCTION__, command);
        app_cmd_execute(command, cmdResult);
    }

    //从启动列表中删除app
    ret = app_start_list_delete(tmpAppName);
    if (-1 == ret)
    {
        SMIOS_FMT_DEBUG("Delete app[%s] from app start list failed\n", appName);
    }

    //保存启动列表信息
    app_start_list_save();

    return ret;
}

/**
*********************************************************************
* @brief：将app启动信息添加到启动列表中
* @param[in]： 
               appStartListInfo  需要添加的app信息
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_start_list_add(APP_START_INFO_T* appStartInfo)
{
    SMIOS_FMT_DEBUG("add app info to start list\n");
    //有效性检查
    if (NULL == appStartInfo)
    {
        SMIOS_FMT_DEBUG("The input param is invalid\n");
        return -1;
    }
    //检查启动列表
    if (NULL == appStartTbl)
    {
        SMIOS_FMT_DEBUG("The app start table is null\n");
        return 0;
    }

    //从启动列表中查找app信息
    size_t size = 0;
    APP_START_INFO_T* pAppStartInfo = (APP_START_INFO_T*)appStartTbl->get(appStartTbl, appStartInfo->appName, &size, false);
    if (NULL != pAppStartInfo)
    {
        SMIOS_FMT_DEBUG("%s, the app[%s] has already been added in the start list\n", __FUNCTION__, appStartInfo->appName);
        return -1;
    }

    //添加app启动信息到启动列表
    bool ret = appStartTbl->put(appStartTbl, appStartInfo->appName, appStartInfo, sizeof(APP_START_INFO_T));
    if (false == ret) {
        SMIOS_FMT_DEBUG("%s, add app[%s] to start table fail\n", __FUNCTION__, appStartInfo->appName);
        return -1;
    }

    //SMIOS_FMT_DEBUG( "app_name : %s", appStartInfo->appName);
    //SMIOS_FMT_DEBUG( "version : %s", appStartInfo->version);

    return 0;
}

/**
*********************************************************************
* @brief：将app启动信息从启动列表中删除
* @param[in]： 
               appName  删除的app名称
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_start_list_delete(char* appName)
{
    //有效性检查
    if (NULL == appName)
    {
        SMIOS_FMT_DEBUG("%s, invalid app name\n", __FUNCTION__);
        return -1;
    }

    //启动列表有效性检查
    if (NULL == appStartTbl)
    {
        SMIOS_FMT_DEBUG("%s, the app start list is NULL\n", __FUNCTION__);
        return -1;
    }

    //从启动列表中删除app
    bool ret = appStartTbl->remove(appStartTbl, appName);
    if (false == ret)
    {
        SMIOS_FMT_DEBUG("%s, delete app[%s] failed\n", __FUNCTION__, appName);
        return -1;
    }

    return 0;
}

/**
*********************************************************************
* @brief：将下载的app安装包解压缩
* @param[in]： 
               fileName  app安装包名
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_package_decompress(char* fileName)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    if (NULL == fileName)
    {
        SMIOS_FMT_DEBUG("%s, the file name is invalid\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG(  "%s, decompress app package\n", __FUNCTION__);
    char tarPath[512 + 1] = {0};
    char tmpCmd[APP_CMD_MAX_LENGTH + 1] = {0};
    char result[APP_CMD_RESULT_MAX_LENGTH + 1] = {0};
    sprintf(tarPath, "%s/%s.tar.gz", g_configObj.serviceCfg.localAppInstallDir, fileName);
    sprintf(tmpCmd, "tar -xf %s -C %s/tmp", tarPath, g_configObj.serviceCfg.localAppInstallDir);
    SMIOS_FMT_DEBUG("depress command : %s\n", tmpCmd);
    app_cmd_execute(tmpCmd, result);

    //delete the tar
    SMIOS_FMT_DEBUG( "%s, delete app package\n", __FUNCTION__);
    memset(tmpCmd, 0, sizeof(tmpCmd));
    memset(result, 0, sizeof(result));
    sprintf(tmpCmd, "rm -rf %s", tarPath);
    app_cmd_execute(tmpCmd, result);

    return 0;
}

/**
*********************************************************************
* @brief：下载app安装包
* @param[in]： 
               url  下载链接
               md5Sum md5校验值
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_download(char* url, char* fileName, char* md5Sum)
{
    if (NULL == url || NULL == fileName || NULL == md5Sum)
    {
        SMIOS_FMT_DEBUG("%s, the input param is invalid\n", __FUNCTION__);
        return -1;
    }

    //download app
    char command[APP_CMD_MAX_LENGTH + 1] = {0};
    char result[APP_CMD_RESULT_MAX_LENGTH + 1] = {0};
    sprintf(command, "wget -O %s/%s.tar.gz %s", g_configObj.serviceCfg.localAppInstallDir,fileName, url);
    sint32 ret = app_cmd_execute(command, result);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, execute command[%s] fail\n", __FUNCTION__, command);
        return -1;
    }

    //下载文件有效性检测
    ret = app_download_package_check(fileName, md5Sum);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, the download package[%s] is invalid\n", __FUNCTION__, fileName);
        return -1;
    }

    //decompress
    app_package_decompress(fileName);

    //handle the CERT dir
    //get the sha1sum
    char certSFPath[1024] = {0};
    sprintf(certSFPath, "%s/tmp/%s/CERT/CERT.SF", g_configObj.serviceCfg.localAppInstallDir, fileName);
    FILE* fp = fopen(certSFPath, "r");
    if (NULL == fp)
    {
        SMIOS_FMT_DEBUG("can not open file[%s]\n", certSFPath);
        app_package_delete(fileName);
        return -1;
    }

    char buffer[1024];
    memset(buffer, 0, 1024);

    size_t size = fread(buffer, 1,1024, fp);
    (void)size;
    SMIOS_FMT_DEBUG("read context:\n");
    SMIOS_FMT_DEBUG("%s\n", buffer);

    char* ptr = strstr(buffer, "Name:");
    char name[256] = {0};
    sscanf(ptr, "Name:%s\n", name);
    SMIOS_FMT_DEBUG("name1 : %s\n", name);

    ptr = strstr(ptr, "SHA1-Digest:");
    char sum[256] = {0};
    sscanf(ptr, "SHA1-Digest:%s\n", sum);
    SMIOS_FMT_DEBUG("sum1 : %s\n", sum);
    char debInfoStr[1024] = {0};
    sprintf(debInfoStr, "%s/tmp/%s/%s.deb", g_configObj.serviceCfg.localAppInstallDir,fileName, fileName);
    ret = app_install_package_check(debInfoStr, sum);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG( "check the deb failed\n");
        app_package_delete(fileName);
        return -1;
    }

    ptr = strstr(ptr, "Name:");
    memset(name, 0, 256);
    sscanf(ptr, "Name:%s\n", name);
    SMIOS_FMT_DEBUG("name2 : %s\n", name);

    ptr = strstr(ptr, "SHA1-Digest:");
    memset(sum, 0, 256);
    sscanf(ptr, "SHA1-Digest:%s\n", sum);
    SMIOS_FMT_DEBUG("sum2 : %s\n", sum);
    char packageInfoStr[1024] = {0};
    sprintf(packageInfoStr, "%s/tmp/%s/packageInfo", g_configObj.serviceCfg.localAppInstallDir,fileName);
    //安装包有效性检查
    ret = app_install_package_check(packageInfoStr, sum);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG( "check the packageinfo failed\n");
        app_package_delete(fileName);
        return -1;
    }

    //security file
    char signFile[256] = {0};
    sprintf(signFile, "%s/tmp/%s/CERT/CERT.RSA", g_configObj.serviceCfg.localAppInstallDir, fileName);
    ret = app_package_digital_sign_check(signFile, fileName);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, digital sign file check fail\n", __FUNCTION__);
        app_package_delete(fileName);
        return -1;
    }
    //app install
    //app_install(tmpFileName);
    //app_package_delete(tmpFileName);
    return 0;
}

/**
*********************************************************************
* @brief：对下载的安装包进行MD5校验
* @param[in]： 
               fileName  安装包名
               md5Sum    md5校验值
* @param[out]：
               none        出参说明
* @return： 返回0  校验成功
            返回-1 校验失败
*********************************************************************
*/
sint32 app_download_package_check(char* fileName, char* md5Sum)
{
#ifdef CERT_SWITCH
    //参数有效性检查
    if (NULL == fileName || NULL == md5Sum)
    {
        SMIOS_FMT_DEBUG("%s, the input param is invalid\n", __FUNCTION__);
        return -1;
    }

    char tmpFileName[256] = {0};
    sprintf(tmpFileName, "%s/%s.tar.gz", g_configObj.serviceCfg.localAppInstallDir, fileName);
    SMIOS_FMT_DEBUG( "fileName[%s]\n", tmpFileName);
    FILE* fp = fopen(tmpFileName, "rb");
    if (NULL == fp)
    {
        SMIOS_FMT_DEBUG("%s, open file[%s] failed\n", __FUNCTION__, fileName);
        return -1;
    }

    MD5_CTX md5Ctx;
    uint8 md5Buffer[APP_MD5_READ_MAX_LENGTH + 1];
    uint8 sum[MD5_DIGEST_LENGTH + 1];
    memset(md5Buffer, 0, APP_MD5_READ_MAX_LENGTH + 1);
    memset(sum, 0, MD5_DIGEST_LENGTH);
    MD5_Init(&md5Ctx);
    while(!feof(fp))
    {
        //计算md5值
        size_t size = fread(md5Buffer, 1, APP_MD5_READ_MAX_LENGTH, fp);
        MD5_Update(&md5Ctx, md5Buffer, size);
    }

    MD5_Final((unsigned char*)sum, &md5Ctx);
    fclose(fp);
    int i = 0;
    //just debug
#if 0
	for(i = 0; i < 16; i++)
	{
		printf(" %x", (int)sum[i]);
	}
    printf("\n");
#endif
    ////

	char restult[10] = {0};
	//SMIOS_FMT_DEBUG("result: ");
	char md5Buf[33] = {0};
	for (i = 0; i < 16; i++)
    {
		memset(restult, 0, 10);
        snprintf(restult, sizeof(restult), "%08x", (int)sum[i]);
		//SMIOS_FMT_DEBUG("%s", &(restult[6]));
		strcat(md5Buf, &(restult[6]));
    }

    SMIOS_FMT_DEBUG("md5 result: %s, len[%zu]\n", md5Buf, strlen(md5Buf));
    SMIOS_FMT_DEBUG("md5 sum:%s, len[%zu]\n", md5Sum, strlen(md5Sum));
    
    if (strcmp(md5Buf, md5Sum) != 0)
    {
        SMIOS_FMT_DEBUG("%s, the file[%s] is invalid\n", __FUNCTION__, fileName);
        return -1;
    }
#else
    (void)fileName;
    (void)md5Sum;
#endif
    SMIOS_FMT_DEBUG("%s, check the file[%s] success\n", __FUNCTION__, fileName);
    return 0;
}

/**
*********************************************************************
* @brief：验证app签名
* @param[in]： 
               fileName  app安装包名
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_install_package_check(char* fileName, char* sha1Sum)
{
#ifdef CERT_SWITCH
    //参数有效性检查
    if (NULL == fileName || NULL == sha1Sum)
    {
        SMIOS_FMT_DEBUG("%s, the input param is invalid\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG("%s, check file[%s]\n", __FUNCTION__, fileName);
    FILE* fp = fopen(fileName, "rb");
    if (NULL == fp)
    {
        SMIOS_FMT_DEBUG("%s, open file[%s] failed\n", __FUNCTION__, fileName);
        return -1;
    }

    SHA_CTX shaCtx;
    memset(&shaCtx, 0, sizeof(SHA_CTX));
    SHA1_Init(&shaCtx);
    uint8 shaSum[SHA_DIGEST_LENGTH + 1];
    char mdShaSum[SHA_DIGEST_LENGTH*2 + 1] = {0};
    uint8 buffer[APP_SHA1_READ_MAX_LENGTH + 1];
    memset(buffer, 0, APP_SHA1_READ_MAX_LENGTH + 1);
    while(!feof(fp))
    {
        size_t size = fread(buffer, 1, APP_SHA1_READ_MAX_LENGTH, fp);
        //SMIOS_FMT_DEBUG( "read context[%s]", buffer);
        SHA1_Update(&shaCtx, buffer, size);
        memset(buffer, 0, APP_SHA1_READ_MAX_LENGTH+1);
    }

    SHA1_Final(shaSum, &shaCtx);
    fclose(fp);
    sint32 i = 0;
    char result[10];
    for (i =0; i < SHA_DIGEST_LENGTH; i++)
    {
        memset(result, 0, 10);
        sprintf(result, "%08x", shaSum[i]);
        strcat(mdShaSum, &(result[6]));
    }

    SMIOS_FMT_DEBUG( "sha1 sum[%s]\n", mdShaSum);
    if (strcmp(mdShaSum, sha1Sum) != 0)
    {
        SMIOS_FMT_DEBUG("%s, the file[%s] is invalid\n", __FUNCTION__, fileName);
        return -1;
    }

    SMIOS_FMT_DEBUG("%s, check file[%s] success\n", __FUNCTION__, fileName);
#else
    (void)fileName;
    (void)sha1Sum;
#endif
    return 0;
}

/**
*********************************************************************
* @brief：加载证书
* @param[in]： 
               
* @param[out]：
               
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_load_cert(uint8 *str, uint32 *str_len,
              const char *verify_cert, const uint32 cert_len)
{
    FILE *fp;
    fp = fopen(verify_cert, "rb");
    if ( NULL == fp)
    {
        SMIOS_FMT_DEBUG("%s, open file[%s] fail\n", __FUNCTION__, verify_cert);
        return -1;
    }

    *str_len = fread(str, 1, cert_len, fp);
    fclose(fp);
    return 0;
}

X509 *app_der_to_x509(const uint8 *der_str, uint32 der_str_len)
{
    X509 *x509;
    x509 = d2i_X509(NULL, &der_str, der_str_len);
    if ( NULL == x509 )
    {
        SMIOS_FMT_DEBUG("%s, d2i_X509 fail\n", __FUNCTION__);

        return NULL;
    }
    return x509;
}
sint32 app_x509_verify(char* usrCertName, char* appName)
{
    sint32 ret;
    UNUSED(usrCertName);

    char cert[MAX_LEGTH];   

    uint8 user_der[MAX_LEGTH];
    uint32 user_der_len;
    X509 *user = NULL;

    uint8 ca_der[MAX_LEGTH];
    uint32 ca_der_len;
    X509 *ca = NULL;

    X509_STORE *ca_store = NULL;
    X509_STORE_CTX *ctx = NULL;
    STACK_OF(X509) *ca_stack = NULL;

    /* x509初始化 */
    ca_store = X509_STORE_new();
    ctx = X509_STORE_CTX_new();

    /* root ca*/
    GET_DEFAULT_CA_CERT(cert);
    SMIOS_FMT_DEBUG( "%s, root ca[%s]\n", __FUNCTION__, cert);
    /* 从文件中读取 */
    ret = app_load_cert(ca_der, (uint32 *)(&ca_der_len), cert, MAX_LEGTH);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, load cert file failed\n", __FUNCTION__);
        goto EXIT;
    }
    /* DER编码转X509结构 */
    ca = app_der_to_x509(ca_der, ca_der_len);
    /* 加入证书存储区 */
    if (NULL == ca)
    {
        SMIOS_FMT_DEBUG("%s, der to x509 failed\n", __FUNCTION__);
        goto EXIT;
    }
    ret = X509_STORE_add_cert(ca_store, ca);
    if ( ret != 1 )
    {
        SMIOS_FMT_DEBUG("%s, X509_STORE_add_cert fail, ret = %d\n", __FUNCTION__, ret);
        goto EXIT;
    }

    /* 需要校验的证书 */
    memset(cert, 0, 4096);
    sprintf(cert, "%s/tmp/%s/CERT/%s", g_configObj.serviceCfg.localAppInstallDir, appName, APP_CERT_RSA);
    SMIOS_FMT_DEBUG( "%s, user ca[%s]\n", __FUNCTION__, cert);
    app_load_cert(user_der, (uint32 *)&user_der_len, cert, 4096);
    user = app_der_to_x509(user_der, user_der_len);
    if (user == NULL)
    {
        SMIOS_FMT_DEBUG("%s, der to x509 failed\n", __FUNCTION__);
        goto EXIT;
    }
    X509_NAME* tmpName = X509_get_subject_name(user);
    char sha1buf[64] = {0};
	X509_NAME_get_text_by_NID(tmpName, NID_commonName, sha1buf, 64);
	SMIOS_FMT_DEBUG("NID_commonName[%s]", sha1buf);
    char certRsaFile[256] = {0};
    sprintf(certRsaFile, "%s/tmp/%s/CERT/%s", g_configObj.serviceCfg.localAppInstallDir, appName, APP_CERT_SF);
    SMIOS_FMT_DEBUG( "%s, app cert rsa [%s]\n", __FUNCTION__, certRsaFile);
    ret = app_install_package_check(certRsaFile, sha1buf);
    if ( ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, the CERT.RSA file check failed\n", __FUNCTION__);
        goto EXIT;
    }
    ret = X509_STORE_CTX_init(ctx, ca_store, user, ca_stack);
    if ( ret != 1 )
    {
        SMIOS_FMT_DEBUG("%s, X509_STORE_CTX_init fail, ret = %d\n", __FUNCTION__, ret);
        goto EXIT;
    }

    ret = X509_verify_cert(ctx);
    if ( ret != 1 )
    {
        SMIOS_FMT_DEBUG("%s, verify cert fail\n", __FUNCTION__);
        goto EXIT;
    }

    SMIOS_FMT_DEBUG("%s, verify success\n", __FUNCTION__);

EXIT:
    X509_free(user);
    X509_free(ca);
    
    if (ctx != NULL)
    {
        X509_STORE_CTX_cleanup(ctx);
        X509_STORE_CTX_free(ctx);
    }

    X509_STORE_free(ca_store);

    return ret == 1 ? 0 : -1;
}

/**
*********************************************************************
* @brief：数字签名校验
* @param[in]： 
               
* @param[out]：
               
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_package_digital_sign_check(char* fileName, char* appName)
{
#ifdef CERT_SWITCH
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    OpenSSL_add_all_algorithms();
    return app_x509_verify(fileName, appName);
#else
    (void)fileName;
    (void)appName;
    return 0;
#endif
}


/**
*********************************************************************
* @brief：安装包删除
* @param[in]： packageName
               
* @param[out]：
               
* @return： 返回0 正常退出
*********************************************************************
*/
sint32 app_package_delete(char* packageName)
{
    char command[APP_CMD_MAX_LENGTH + 1] = {0};
    char result[APP_CMD_RESULT_MAX_LENGTH + 1] = {0};
    sprintf(command, "rm -rf %s/tmp/%s", g_configObj.serviceCfg.localAppInstallDir, packageName);
    app_cmd_execute(command, result);
    return 0;
}

/**
*********************************************************************
* @brief：app包解密
* @param[in]： 
               fileName  app安装包名
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_package_decrypt(char* fileName)
{
    (void)fileName;
    return 0;
}

/**
*********************************************************************
* @brief：搜索指定的app是否在注册列表中
* @param[in]： 
               fileName  app名
               appRegisterList 注册列表头指针
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
sint32 app_register_list_check(char* appName)
{
    if (NULL == appName || NULL == appRegisterTbl) 
    {
        SMIOS_FMT_DEBUG("The app register info list is null\n");
        return -1;
    }

    size_t size = 0;
    APP_REGISTER_INFO_T* pAppRegisterInfo = NULL;
    //获取注册信息
    pAppRegisterInfo = appRegisterTbl->get(appRegisterTbl, appName, &size, false);
    if (NULL == pAppRegisterInfo)
    {
        SMIOS_FMT_DEBUG("%s, the app is not in the register table\n", __FUNCTION__);
        return -1;
    }
    SMIOS_FMT_DEBUG( "%s, the app is in the register table\n", __FUNCTION__);
    return 0;
}

/**
*********************************************************************
* @brief：搜索指定的容器是否在运行
* @param[in]： 
               containerName  容器ID
* @param[out]：
               none        出参说明
* @return： 返回0 容器在运行
            返回-1 容器已退出
*********************************************************************
*/
sint32 app_contaienr_is_running(char* containerName)
{
    if (NULL == containerName)
    {
        SMIOS_FMT_DEBUG("%s, the input param is invalid\n", __FUNCTION__);
        return -1;
    }

    //获取正在运行的容器名
    char command[APP_CMD_MAX_LENGTH + 1] = {0};
    char result[APP_CMD_RESULT_MAX_LENGTH + 1] = {0};
    sprintf(command, "docker container ls --format {{.Names}}");
    sint32 ret = app_cmd_execute(command, result);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, execute command[%s] fail\n", __FUNCTION__, command);
        return -1;
    }

    //查找容器是否运行
    if (strstr(result, containerName) == NULL)
    {
        SMIOS_FMT_DEBUG("%s, the container[%s] is not running\n", __FUNCTION__, containerName);
        return -1;
    }
    return 0;
}

/**
*********************************************************************
* @brief：删除已经停止运行的容器
* @param[in]： 
               containerName  容器ID
* @param[out]：
               none        出参说明
* @return： 返回0 删除成功
            返回-1 删除失败
*********************************************************************
*/
sint32 app_container_remove(char* containerName)
{
    if (NULL == containerName)
    {
        SMIOS_FMT_DEBUG("%s, the input param is invalid\n", __FUNCTION__);
        return -1;
    }
    SMIOS_FMT_DEBUG("%s, remove the container[%s] which has already stopped\n", __FUNCTION__, containerName);
    char command[APP_CMD_MAX_LENGTH + 1] = {0};
    char result[APP_CMD_RESULT_MAX_LENGTH + 1] = {0};
    sprintf(command, "docker rm %s", containerName);
    sint32 ret = app_cmd_execute(command, result);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, execute command[%s] fail\n", __FUNCTION__, command);
        return -1;
    }

    return 0;
}

/**
*********************************************************************
* @brief：释放app启动列表
* @param[in]： 
               none        入参说明
* @param[out]：
               none        出参说明
* @return： 返回0 正常退出
            返回-1 处理异常
*********************************************************************
*/
void app_start_list_clear(void)
{
    SMIOS_FMT_DEBUG("%s, clear the app start list\n", __FUNCTION__);
    if (NULL == appStartTbl)
    {
        return;
    }
    appStartTbl->free(appStartTbl);
    return;
}

/**
*********************************************************************
* @brief：释放app注册列表
* @param[in]： 
               none        入参说明
* @param[out]：
               none        出参说明
* @return： void
*********************************************************************
*/
void app_register_list_clear(void)
{
    SMIOS_FMT_DEBUG("%s, clear the app register list\n", __FUNCTION__);
    if (NULL == appRegisterTbl)
    {
        return;
    }
    appRegisterTbl->free(appRegisterTbl);

    return;
}

static void* appStart(void* command)
{
    SMIOS_FMT_DEBUG( "%s, command[%s], begin\n", __FUNCTION__, (char*)command);
    FILE* fp = NULL;
    if ((fp = popen(command, "r")) != NULL)
    {
        pclose(fp);
        fp = NULL;
    } else {
        SMIOS_FMT_DEBUG("popen %s error\n", (char*)command);
    }
    SMIOS_FMT_TRACE( "%s, end\n", __FUNCTION__);

    return NULL;
}

uint32 app_event_handle(uint32 eventID, uint8 *eventBuf, uint32 eventBufLen)
{
    //uint32 i = 0;
   // uint32 sendCount = 0;
    //uint32 bitEventID = 0;
    MSG_CLIENT_T client = {0};
    SMIOS_FMT_TRACE(" %s\n", __FUNCTION__);

    // if (NULL == eventBuf)
    // {
    //     return 0;
    // }
    sprintf(client.clientName,"Broadcast");
    client.type = MQTT;
    send_general_event_to_client(&client, eventID, eventBuf,eventBufLen);
    return 0;

//    bitEventID = (1 << (eventID & 0xffff)) | (eventID & 0xffff0000); 
//    
//    APP_REGISTER_INFO_T *pAppRegisterInfo = NULL;
//    qhashtbl_obj_t       obj;
//    memset(&obj, 0, sizeof(obj));
//
//    size_t appCount = app_register_app_count_get();
//    if (appCount <= 0)
//    {
//        SMIOS_FMT_DEBUG("the app count is 0\n");
//        return 0;
//    }
//
//    SMIOS_FMT_TRACE( "app count[%zu]\n", appCount);
//    appRegisterTbl->lock(appRegisterTbl);
//    while (1)
//    {
//        if (appRegisterTbl->getnext(appRegisterTbl, &obj, true) != true)
//        {
//            break;
//        }
//        pAppRegisterInfo = (APP_REGISTER_INFO_T *)(obj.data);
//
//        for (i = 0; i < pAppRegisterInfo->subscribeInfo.subscribeInfoNum; i++)
//        {
//            if (((bitEventID & 0xffff0000) == (pAppRegisterInfo->subscribeInfo.subscribeInfo[i] & 0xffff0000))
//                && ((bitEventID & 0xffff) & (pAppRegisterInfo->subscribeInfo.subscribeInfo[i] & 0xffff)))
//            {
//                strncpy(client.clientName, pAppRegisterInfo->appName,
//                        MAX_APP_NAME_LEN);
//                SMIOS_FMT_TRACE( "app name[%s]\n", client.clientName);
//                switch (pAppRegisterInfo->protocolInfo.protocolType)
//                {
//                    case PROTOCOL_UDP:
//                        client.type = UDP;
//                        strncpy(client.client.uClient.ip,
//                                pAppRegisterInfo->protocolInfo.udp.ip, IP_LEN);
//                        client.client.uClient.port =
//                            pAppRegisterInfo->protocolInfo.udp.port;
//                        SMIOS_FMT_DEBUG( "type[udp], ip[%s], port[%d]\n",
//                                    client.client.uClient.ip, client.client.uClient.port);
//                        break;
//                    case PROTOCOL_TCP:
//                        client.type = TCP;
//                        strncpy(client.client.tClient.ip,
//                                pAppRegisterInfo->protocolInfo.tcp.ip, IP_LEN);
//                        client.client.tClient.port =
//                            pAppRegisterInfo->protocolInfo.tcp.port;
//                        SMIOS_FMT_DEBUG( "type[tcp], ip[%s], port[%d]\n",
//                                    client.client.tClient.ip, client.client.tClient.port);
//                        break;
//                    case PROTOCOL_IPC:
//                        client.type = IPC;
//                        strncpy(
//                            client.client.iClient.unixSockertPath,
//                            pAppRegisterInfo->protocolInfo.ipc.unixSockertPath,
//                            UNIX_SOCKET_PATH_LEN);
//                        SMIOS_FMT_DEBUG( "type[IPC], socket path[%s]\n",
//                                    client.client.iClient.unixSockertPath);
//                        break;
//                    case PROTOCOL_MQTT:
//                        client.type = MQTT;
//                        SMIOS_FMT_TRACE( "type[MQTT]\n");
//                        break;
//                    default:
//                        break;
//                }
//
//                send_general_event_to_client(&client, eventID, eventBuf,
//                                             eventBufLen);
//                sendCount++;
//            }
//        }
//        
//        free(obj.name);
//        free(obj.data);
//    }
//    appRegisterTbl->unlock(appRegisterTbl);
//
//    return sendCount;
}

sint32 app_get_app_name_in_container(const char* containerName, char* appNameArray, sint32* appCount)
{
    sint32 index = 0;
    qhashtbl_obj_t obj;
    char* pAppNameArray = appNameArray;

    memset(&obj, 0, sizeof(obj));
    if (appRegisterTbl == NULL)
    {
        SMIOS_FMT_DEBUG("The app register table is NULL\n");
        return -1;
    }

    //appRegisterTbl->lock(appRegisterTbl);
    while (appRegisterTbl->getnext(appRegisterTbl, &obj, true) == true)
    {
        APP_REGISTER_INFO_T* pAppRegisterInfo = (APP_REGISTER_INFO_T*)obj.data;
        if (pAppRegisterInfo != NULL)
        {
            if (strcmp(pAppRegisterInfo->containerName, containerName) == 0)
            {
                strcpy(pAppNameArray, pAppRegisterInfo->appName);
                index = index + 1;
                pAppNameArray = pAppNameArray + index * (APP_NAME_LENGTH + 1); 
            }

            free(obj.name);
            free(obj.data);
        }
    }
    //appRegisterTbl->unlock(appRegisterTbl);
    *appCount = index;
    return 0;
}

sint32 app_all_app_query(MQT_PLUGIN_T **mqtPlugin, uint32* count)
{
    //SMIOS_FMT_DEBUG( __FUNCTION__);
    uint8 otherappcount;
    otherappcount = app_register_app_count_get();
    if (0 == otherappcount)
    {
        SMIOS_FMT_DEBUG("There is no other app in the register table\n");
    }

    *mqtPlugin = calloc(otherappcount+1, sizeof(MQT_PLUGIN_T));
    if (NULL == *mqtPlugin)
    {
        SMIOS_FMT_DEBUG("alloc buffer failed\n");
        return -1;
    }

    MQT_PLUGIN_T* pMqtPlutin = *mqtPlugin;
    *count = 0;
    if(otherappcount != 0)
    {
        APP_REGISTER_INFO_T* pAppRegisterInfo = NULL;
        qhashtbl_obj_t obj;
        memset(&obj, 0, sizeof(obj));

        //appRegisterTbl->lock(appRegisterTbl);
        
        while(appRegisterTbl->getnext(appRegisterTbl, &obj, true) == true)
        {
            pAppRegisterInfo = (APP_REGISTER_INFO_T*)(obj.data);
            if(alive_app_name_read(pAppRegisterInfo->appName) >= 0)
            {
                strncpy((char*)(pMqtPlutin->comName), pAppRegisterInfo->appName, XDR_MAX_COM_NAME_LEN);
                strcpy(pMqtPlutin->version,pAppRegisterInfo->version);
                pMqtPlutin->subr = pAppRegisterInfo->subscribeInfo;
                memcpy(&(pMqtPlutin->dataTime),&(pAppRegisterInfo->dataTime), sizeof(DATA_TIME_T));
                SMIOS_FMT_DEBUG( "app name[%s]\n", pMqtPlutin->comName);
                pMqtPlutin->appidlen = pAppRegisterInfo->appidlen;
                memcpy(pMqtPlutin->appid,pAppRegisterInfo->appid,pAppRegisterInfo->appidlen);
                pMqtPlutin = pMqtPlutin + 1;
                (*count)++;
            }
            free(obj.name);
            free(obj.data);
        }
        //appRegisterTbl->unlock(appRegisterTbl);
    }
    //将smiOS自己添加到返回的注册列表中
    strcpy((char*)(pMqtPlutin->comName), "M-smiOS");
    strcpy(pMqtPlutin->version ,APP_VERSION);
    pMqtPlutin->subr.subscribeInfoNum = 0;
    app_get_maketime(APP_PUBLISH_TIME,(OOP_DATETIME_S_T*)&pMqtPlutin->dataTime);
    pMqtPlutin->appidlen =APPIDLen;
    memcpy(pMqtPlutin->appid,APPID,APPIDLen);

    (*count)++;
    return 0;
}

void updateRegisterTime(char *appName)
{
    if (appName ==NULL) {
        LOGIN_FMT_DEBUG("the app name is null\n");
        return;
    }
    size_t size = 0;
    APP_REGISTER_INFO_T* pAppRegisterinfo = appRegisterTbl->get(appRegisterTbl, appName, &size, true);
    if (pAppRegisterinfo != NULL)
    {
        struct timespec tv;
        LOGIN_FMT_DEBUG("%s, update app[%s]\n", __FUNCTION__, pAppRegisterinfo->appName);
        clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
        pAppRegisterinfo->tv.tv_sec = tv.tv_sec;
        pAppRegisterinfo->tv.tv_nsec = tv.tv_nsec;
        app_register_list_update(pAppRegisterinfo);
        free(pAppRegisterinfo);
    }
}

/**
*********************************************************************
* @brief：mosquito运行检测（只针对能3358核心板）
* @param[in]： 
               none        入参说明
* @param[out]：
               none        出参说明
* @return： void
*********************************************************************
*/
void mosquitoCheck()
{
    char resultBuf[128] = {0};
    
    FILE* fp = NULL;
    fp = popen("ps |grep mosquitto|grep \"/etc/mosquitto/mosquitto.conf\"|grep -v grep|awk '{print $1}'", "r");
    if (fp)
    {
        usleep(10000);
        fgets(resultBuf, sizeof(resultBuf), fp);
        pclose(fp);
        fp = NULL;
    }
    if(0 == resultBuf[0])
    {
        SMIOS_FMT_DEBUG("mosquitto is not exist, restart!\n");
        system("mosquitto -c /etc/mosquitto/mosquitto.conf >/dev/null 2>&1 &");
    }
}

/**
*********************************************************************
* @brief：心跳检测函数
* @param[in]： 
               none        入参说明
* @param[out]：
               none        出参说明
* @return： void
*********************************************************************
*/
static void* appAliveCheck(void* arg)
{
    (void)arg;
    char restartAppName[64] = {0};
    APP_RESTART_INFO_T appInfo[APP_RESTART_MAX_COUNT];
    sint32 count = 0;
    sint32 index = 0;
    sint32 needUpdateTime;
    sint32 appIndex;
    char command[APP_MAX_COUNT][256] = {0};
    char sendErrorApp[APP_MAX_COUNT][APP_NAME_LENGTH];
    
    memset(appInfo, 0, sizeof(APP_RESTART_INFO_T) * APP_RESTART_MAX_COUNT);
    while(1)
    {
        sleep(APP_HEART_BEAT_CHECK_TIME);
#if defined BOARD_3358 || defined BOARD_3568
        mosquitoCheck();
#endif
        if (NULL == appRegisterTbl) {
            SMIOS_FMT_DEBUG("%s, the app register table is null\n", __FUNCTION__);
            continue;
        }
        HEART_FMT_DEBUG("%s, send message to check app alive\n", __FUNCTION__);
        needUpdateTime = 0;
        appIndex = 0;
        qhashtbl_obj_t obj;
        memset((void*)&obj, 0, sizeof(obj));
        memset(sendErrorApp, 0, APP_MAX_COUNT * APP_NAME_LENGTH);
        //appRegisterTbl->lock(appRegisterTbl);
        while(appRegisterTbl->getnext(appRegisterTbl, &obj, true) == true)
        {
            APP_REGISTER_INFO_T* pAppRegisterInfo = (APP_REGISTER_INFO_T*)(obj.data);
            //SMIOS_FMT_DEBUG( "app[%s] sec[%ld], nsec[%ld]", pAppRegisterInfo->appName, pAppRegisterInfo->tv.tv_sec,
            //            pAppRegisterInfo->tv.tv_nsec);
            struct timespec tp;
            clock_gettime(CLOCK_MONOTONIC_RAW, &tp);

            uint64 timediff = tp.tv_sec - (pAppRegisterInfo->tv.tv_sec);
            if (timediff > APP_HEART_BEAT_CHECK_MAX_TIME)
            {
                //call function to restart app
                HEART_FMT_DEBUG("[%s]长时间未响应\n", pAppRegisterInfo->appName);

                if (index < APP_RESTART_MAX_COUNT)
                {
                    strncpy(appInfo[index].appName, pAppRegisterInfo->appName, 64);
                    appInfo[index].pid = pAppRegisterInfo->pid;
                    index++;
                } else
                {
                    HEART_FMT_DEBUG("The restart app count exceed the max count\n");
                }
            } else {
                HEART_FMT_DEBUG("send heartbeat: [%s]\n",  pAppRegisterInfo->appName);

                MSG_CLIENT_T msgClient;
                strncpy(msgClient.clientName, pAppRegisterInfo->appName, MAX_APP_NAME_LEN);
                switch (pAppRegisterInfo->protocolInfo.protocolType)
                {
                case PROTOCOL_UDP:
                    msgClient.type = UDP;
                    strncpy(msgClient.client.uClient.ip, pAppRegisterInfo->protocolInfo.udp.ip, IP_LEN);
                    msgClient.client.uClient.port = pAppRegisterInfo->protocolInfo.udp.port;
                    SMIOS_FMT_DEBUG( "%s, app[%s], type[UDP], ip[%s], port[%d]\n", __FUNCTION__, msgClient.clientName,
                            msgClient.client.uClient.ip, msgClient.client.uClient.port);
                    break;
                case PROTOCOL_TCP:
                    msgClient.type = TCP;
                    strncpy(msgClient.client.tClient.ip, pAppRegisterInfo->protocolInfo.tcp.ip, IP_LEN);
                    msgClient.client.tClient.port = pAppRegisterInfo->protocolInfo.tcp.port;
                    SMIOS_FMT_DEBUG( "%s, app[%s], type[TCP], ip[%s], port[%d]\n", __FUNCTION__, msgClient.clientName,
                            msgClient.client.tClient.ip, msgClient.client.tClient.port);
                    break;
                case PROTOCOL_IPC:
                    msgClient.type = IPC;
                    strncpy(msgClient.client.iClient.unixSockertPath, pAppRegisterInfo->protocolInfo.ipc.unixSockertPath,
                            UNIX_SOCKET_PATH_LEN);
                    break;
                case PROTOCOL_MQTT:
                    msgClient.type = MQTT;
                    break;
                default:
                    SMIOS_FMT_DEBUG("invalid protocol type\n");
                    break;
                }
                //send heart beat message
                sint32 ret = send_heartbeat_req_to_client(&msgClient);
                if (ret < 0)
                {
                    needUpdateTime = 1;
                    strncpy(sendErrorApp[appIndex], pAppRegisterInfo->appName, APP_NAME_LENGTH);
                    HEART_FMT_DEBUG("====The network may be disconnect!!!!\n");
                    appIndex++;
                }

            }
            free(obj.name);
            free(obj.data);
        }
        //appRegisterTbl->unlock(appRegisterTbl);

        //如果出现网络不通的情况需要更新每个app的心跳时间，防止app被从注册列表中删除
        if (needUpdateTime) {
            HEART_FMT_DEBUG("just update the register time, count[%d]\n", appIndex);
            sint32 i = 0;
            for (i = 0; i < appIndex; i++)
            {
                HEART_FMT_DEBUG("send error app[%s]\n", sendErrorApp[i]);
                updateRegisterTime(sendErrorApp[i]);
            }
        }

        //resart the app
        count = index;
        HEART_FMT_DEBUG( "The restart app count is %d\n", count);

        if (count > APP_RESTART_MAX_COUNT)
        {
            count = APP_RESTART_MAX_COUNT;
        }
        for ( index = 0; index < count; index++)
        {
            //sint32 ret = app_restart(appInfo[index].appName);
            //if (ret != 0)
            {
                //HEART_FMT_DEBUG( "restart app[%s] failed, try another way\n", appInfo[index].appName);
                char protocol[64] = {0};
                sscanf(appInfo[index].appName, "%[^-]-%s", protocol, restartAppName);
                
                HEART_FMT_DEBUG( "restartAppName[%s], pid[%d]\n", restartAppName, appInfo[index].pid);
                if ((appInfo[index].pid != -1) && (appInfo[index].pid != 0))
                {
                    memset(command[index], 0, 256);
                    sprintf(command[index], "kill -9 %d", appInfo[index].pid);
                    //目前改为不主动杀进程 方便调试
                    //system(command);
                }
                //restart app
                if (strlen(restartAppName) != 0) {
                    HEART_FMT_DEBUG("restart app[%s]\n", restartAppName);

                    //use thread to restart the app
                    memset(command[index], 0, 256);
                    sprintf(command[index], "/usr/local/extapps/%s/bin/startup_app.sh", restartAppName);
                    pthread_t tid;
                    pthread_attr_t attr;
                    pthread_attr_init(&attr);
                    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                    if ((pthread_create(&tid, &attr, appStart, command[index])) == -1)
                    {
                        HEART_FMT_DEBUG( "create app start thread failed\n");
                    } else {
                        HEART_FMT_DEBUG( "create app start thread success\n");
                    }
                    
                    HEART_FMT_DEBUG( "重启[%s]成功\n", appInfo[index].appName);
                    memset(restartAppName, 0, sizeof(restartAppName));
                    //从注册列表中删除
                    app_register_list_delete(appInfo[index].appName);
                }
            }
        }
        count = 0;
        index = 0;
        memset(appInfo, 0, sizeof(appInfo));
    }

    return NULL;
}

/**
*********************************************************************
* @brief：创建心跳检测线程
* @param[in]： 
               none        入参说明
* @param[out]：
               none        出参说明
* @return： void
*********************************************************************
*/
void createAppHeartBeatDetect(void)
{
    pthread_t tid;
    if ((pthread_create(&tid, NULL, appAliveCheck, NULL)) == -1)
    {
        SMIOS_FMT_DEBUG("create app heart beat thread failed\n");
    } else {
        SMIOS_FMT_DEBUG("create app heart beat thread success\n");
    }
}

#ifdef APP_INSTALL_DEBUG
//just for debug, need to delete in release version
static void* createAppInstallObserver(void* arg)
{
    //check whether new file added
    (void)arg;
    SMIOS_FMT_DEBUG("start the app install observer thread....\n");
    char command[1024];
    char result[1024];
    char appName[128];
    char tmpAppName[128];
    char delCommand[128] = {0};
    char tmpCmd[1024] = {0};
    memset(command, 0, 1024);
    memset(result, 0, 1024);
    memset(appName, 0, 128);
    memset(tmpAppName, 0, 128);
    memset(delCommand, 0, 128);
    memset(tmpCmd, 0, 1024);
    //sprintf(command, "ls %s/*.deb", g_configObj.serviceCfg.localAppInstallDir);
    sprintf(command, "ls %s/*.tar.gz", g_configObj.serviceCfg.localAppInstallDir);
    while(1) {
        app_cmd_execute(command, result);
        SMIOS_FMT_DEBUG("The file list : %s\n", result);
        //sprintf(format, "%s/%s", g_configObj.serviceCfg.localShareDir, "\%s");
        //sscanf(result, "/usr/local/appInstall/%s.deb", appName);
        //decompress
        if (strlen(result) > 3)
        {
            result[strlen(result) - 1] = '\0';
        } else {
            SMIOS_FMT_DEBUG("no new tar add\n");
            sleep(10);
            continue;
        }

        char tarPath[1024];
        strncpy(tarPath, result, 1024);
        sprintf(tmpCmd, "tar -xf %s -C %s/tmp", result, g_configObj.serviceCfg.localAppInstallDir);
        SMIOS_FMT_DEBUG("depress command : %s\n", tmpCmd);
        app_cmd_execute(tmpCmd, result);

        memset(tmpCmd, 0, 1024);
        memset(result, 0, 1024);
        sprintf(tmpCmd, "rm -rf %s", tarPath);
        SMIOS_FMT_DEBUG("delete tar: %s\n", tmpCmd);
        app_cmd_execute(tmpCmd, result);

        //check
        memset(result, 0, 1024);
        memset(tmpCmd, 0, 1024);
        char tarName[1024] = {0};
        SMIOS_FMT_DEBUG("tar path : %s\n", tarPath);
        sscanf(tarPath, "/usr/local/appInstall/%s.tar.gz", tarName);
        char tmpTarName[512] = {0};
        sscanf(tarName, "%[^.]", tmpTarName);
        SMIOS_FMT_DEBUG("tar name[%s]\n", tmpTarName);
        sprintf(tmpCmd, "ls %s/tmp/%s/*.deb", g_configObj.serviceCfg.localAppInstallDir, tmpTarName);
        app_cmd_execute(tmpCmd, result);
        SMIOS_FMT_DEBUG("tmp command 2 : %s\n", tmpCmd);
        SMIOS_FMT_DEBUG("result[%s]\n", result);
        sscanf(result, "/usr/local/appInstall/tmp/msgClient/%s.deb", appName);
        sscanf(appName, "%[^.]", tmpAppName);
        SMIOS_FMT_DEBUG("app name[%s]\n", tmpAppName);

#ifdef CERT_SWITCH
        //get the sha1sum
        char certSFPath[1024] = {0};
        sprintf(certSFPath, "/usr/local/appInstall/tmp/%s/CERT/CERT.SF", tmpTarName);
        //FILE* fp = fopen("/usr/local/appInstall/tmp/app/CERT/CERT.SF", "r");
        FILE* fp = fopen(certSFPath, "r");
        if (NULL == fp)
        {
            SMIOS_FMT_DEBUG("can not open file[%s]\n", certSFPath);
            return NULL;
        }

        char buffer[1024];
        memset(buffer, 0, 1024);

        size_t size = fread(buffer, 1,1024, fp);
        (void)size;
        SMIOS_FMT_DEBUG("read context:\n");
        SMIOS_FMT_DEBUG("%s\n", buffer);

        char* ptr = strstr(buffer, "Name:");
        char name[256] = {0};
        sscanf(ptr, "Name:%s\n", name);
        SMIOS_FMT_DEBUG("name1 : %s\n", name);

        ptr = strstr(ptr, "SHA1-Digest:");
        char sum[256] = {0};
        sscanf(ptr, "SHA1-Digest:%s\n", sum);
        SMIOS_FMT_DEBUG("sum1 : %s\n", sum);
        char debInfoStr[1024] = {0};
        sprintf(debInfoStr, "/usr/local/appInstall/tmp/%s/%s.deb", tmpTarName, tmpAppName);
        sint32 ret = app_install_package_check(debInfoStr, sum);
        if (ret < 0)
        {
            SMIOS_FMT_DEBUG( "check the deb failed\n");
            return NULL;
        }

        ptr = strstr(ptr, "Name:");
        memset(name, 0, 256);
        sscanf(ptr, "Name:%s\n", name);
        SMIOS_FMT_DEBUG("name2 : %s\n", name);

        ptr = strstr(ptr, "SHA1-Digest:");
        memset(sum, 0, 256);
        sscanf(ptr, "SHA1-Digest:%s\n", sum);
        SMIOS_FMT_DEBUG("sum2 : %s\n", sum);
        char packageInfoStr[1024] = {0};
        sprintf(packageInfoStr, "/usr/local/appInstall/tmp/%s/packageInfo", tmpTarName);
        ret = app_install_package_check(packageInfoStr, sum);
        if (ret < 0)
        {
            SMIOS_FMT_DEBUG( "check the packageinfo failed\n");
            return NULL;
        }
#endif
#if 1
        if (strlen(tmpAppName) != 0) {
            SMIOS_FMT_DEBUG("===================INSTALL APP\n");
            app_install(tmpAppName);
            sleep(3);
            SMIOS_FMT_DEBUG("delete the app[%s]", result);
            sprintf(delCommand, "rm -rf %s", result);
            memset(result, 0, 1024);
            app_cmd_execute(delCommand, result);
            memset(delCommand, 0, 128);

            sprintf(delCommand, "rm -rf /usr/local/appInstall/tmp/*");
            app_cmd_execute(delCommand, result);
            memset(delCommand, 0, 128);
            sleep(1000);

            SMIOS_FMT_DEBUG("===================UNINSTALL APP\n");
            //app_uninstall(tmpAppName, DELETE_TYPE_ALL);
        } else {
            SMIOS_FMT_DEBUG("no new tar add\n");
            sleep(3);
        }
#endif
        memset(result, 0, 1024);
        memset(appName, 0, 128);
        memset(tmpAppName, 0, 128);
    }


    return 0;
}

void createAppInstallThread(void)
{
    pthread_t tidp;
    if ((pthread_create(&tidp, NULL, createAppInstallObserver, NULL)) == -1)
    {
        SMIOS_FMT_DEBUG("create app install thread failed\n");
    } else {
        SMIOS_FMT_DEBUG("create app install thread success\n");
    }

    return;
}


#endif
