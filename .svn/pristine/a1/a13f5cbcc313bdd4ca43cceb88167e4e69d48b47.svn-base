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
#include<pthread.h>
#include<unistd.h>
#include<sys/time.h>

#include "framecomm.h"
#include "cJSON.h"
#include "openssl/sha.h"
#include "openssl/md5.h"
#include "openssl/evp.h"
#include "openssl/x509.h"
#include "globalConfiguration.h"
#include "appManagerCommon.h"
#include "dockerManagerCommon.h"
#include "configterminal.h"
#include <sys/prctl.h>
#include "storage.h"

//docker mirror table head
qhashtbl_t* dockerTbl = NULL;

//docker container table head
qhashtbl_t* containerTbl = NULL;

//extern CONTAINSTAT_T containerState;
ALL_CONTAINER_AND_APP_T allcontainerinfo;

const char* gServiceInfoFile = "/data/app/smiOS/data/serviceInfo";

pthread_mutex_t gServiceInfoLock;
/**
*********************************************************************
* @brief： docker管理模块初始化
* @param[in]： none
* @param[out]：none
* @return： 返回值说明 -1 初始化失败
                      0 初始化成功
*********************************************************************
*/
sint32 docker_manager_init(void)
{
    //init the docker mirror table
    dockerTbl = qhashtbl(DOCKER_MAX_COUNT, QHASHTBL_THREADSAFE);
    if (NULL == dockerTbl)
    {
        SMIOS_FMT_DEBUG("%s, create mirror hash table failed\n", __FUNCTION__);
        return -1;
    }

    //init the docker container table
    containerTbl = qhashtbl(DOCKER_MAX_COUNT, QHASHTBL_THREADSAFE);
    if (NULL == containerTbl)
    {
        SMIOS_FMT_DEBUG("%s, create container hash table failed\n", __FUNCTION__);
        return -1;
    }

    //clear the container environment
    //docker_manager_container_clear();

    //load the docker config information to the start list
    docker_manager_config_load();

    pthread_mutex_init(&gServiceInfoLock,NULL);
    return 0;
}

/**
*********************************************************************
* @brief： 启动容器
* @param[in]： none
* @param[out]：none
* @return： 返回值说明 -1 容器启动失败
                      0 容器启动成功
*********************************************************************
*/
sint32 docker_manager_start(void)
{
/*
    if (NULL == dockerTbl)
    {
        SMIOS_FMT_DEBUG("%s, the docker table is null\n", __FUNCTION__);
        return 0;
    }

    uint32 tableCount = dockerTbl->size(dockerTbl);
    if (0 == tableCount)
    {
        SMIOS_FMT_DEBUG("%s, there is no app need to start\n", __FUNCTION__);
        return 0;
    }

    DOCKER_INFO_T* pDockerInfo = NULL;
    qhashtbl_obj_t obj;
    memset(&obj, 0, sizeof(obj));
    dockerTbl->lock(dockerTbl);
    while (dockerTbl->getnext(dockerTbl, &obj, true) == true)
    {
        pDockerInfo = (DOCKER_INFO_T*)(obj.data);
        docker_manager_docker_start(pDockerInfo);
        free(obj.data);
        free(obj.name);
    }
    dockerTbl->unlock(dockerTbl);
*/

    return 0;
}

/**
*********************************************************************
* @brief： docker管理模块退出
* @param[in]： none
* @param[out]：none
* @return： 返回值说明 -1 退出失败
                      0 退出成功
*********************************************************************
*/
sint32 docker_manager_exit(void)
{
    SMIOS_FMT_DEBUG("clear the docker list\n");
    //判断镜像hash表有效性
    if (NULL != dockerTbl)
    {
        //释放docker信息hash表
        dockerTbl->free(dockerTbl);
        dockerTbl = NULL;
    }

    //判断容器hash表有效性
    if (NULL != containerTbl)
    {
        //释放容器信息hash表
        containerTbl->free(containerTbl);
        containerTbl = NULL;
    }
    return 0;
}

/**
*********************************************************************
* @brief： docker管理模块初始化时清理环境
* @param[in]： none
* @param[out]：none
* @return： 返回值说明 -1 容器清理失败
                      0 容器清理成功
*********************************************************************
*/
void docker_manager_container_clear(void)
{
    SMIOS_FMT_DEBUG( __FUNCTION__);
    char command[128] = {0};
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1] = {0};
    int i = 0;
    for (i = 0; i < DOCKER_MAX_COUNT; i++)
    {
        //停止容器
        sprintf(command, "docker stop container%02d", i+1);
        docker_cmd_execute(command, result);

        //删除容器
        memset(command, 0, 128);
        sprintf(command, "docker rm -f container%02d", i+1);
        docker_cmd_execute(command, result);
    }
    return;
}

/**
*********************************************************************
* @brief： 执行docker命令
* @param[in]： cmd    执行的命令
* @param[out]：result 命令执行结果
* @return： 返回值说明 -1 执行失败
                      0 执行成功
*********************************************************************
*/
sint32 docker_cmd_execute(const char* cmd,  char* result)
{
    //有效性检查
    if (NULL == cmd || NULL == result)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }
    char resultBuf[DOCKER_CMD_RESULT_MAX_LENGHT + 1];
    char tmpCmd[DOCKER_CMD_MAX_LENGTH + 1] = {0};
    FILE* fp = NULL;
    strncpy(tmpCmd, cmd, DOCKER_CMD_MAX_LENGTH);
    if ((fp = popen(tmpCmd, "r")) != NULL)
    {
        while(fgets(resultBuf, DOCKER_CMD_RESULT_MAX_LENGHT, fp) != NULL) {
            strcat(result, resultBuf);
            if(strlen(result) > DOCKER_CMD_RESULT_MAX_LENGHT)
            {
                break;
            }
        }
        pclose(fp);
        fp = NULL;
    } else {
        SMIOS_FMT_DEBUG("%s, popen %s error\n", __FUNCTION__, tmpCmd);
        return -1;
    }

    return 0;
}

/**
*********************************************************************
* @brief： 容器启动
* @param[in]： mirrorName 镜像名
               containerName 容器名
* @param[out]：none
* @return： 返回值说明 -1 启动失败
                      0 启动成功
*********************************************************************
*/
sint32 docker_mangager_container_start(char* mirrorName, char* containerName)
{
    SMIOS_FMT_DEBUG( __FUNCTION__);
    //参数有效性检查
    if (NULL == mirrorName || NULL == containerName)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }
    size_t size = 0;
    //获取docker镜像信息
    DOCKER_INFO_T* pDockerInfo = dockerTbl->get(dockerTbl, mirrorName, &size, true);
    if (NULL == pDockerInfo)
    {
        SMIOS_FMT_DEBUG("%s, not find the mirror[%s]\n", __FUNCTION__, mirrorName);
        return -1;
    }
    //启动容器
    return docker_manager_docker_start(pDockerInfo, containerName);
}

/**
*********************************************************************
* @brief： 启动指定的docker
* @param[in]： dockerInfo  docker信息
               containerName 容器名
* @param[out]：none
* @return： 返回值说明 -1 启动失败
                      0 启动成功
*********************************************************************
*/
sint32 docker_manager_docker_start(DOCKER_INFO_T* dockerInfo, char* containerName)
{
    SMIOS_FMT_DEBUG( __FUNCTION__);
    //参数有效性检查
    if (NULL == dockerInfo || NULL == containerName)
    {
        SMIOS_FMT_DEBUG("%s, the docker info is null\n", __FUNCTION__);
        return -1;
    }

    //SMIOS_FMT_DEBUG("%s, start mirror[%s]\n", __FUNCTION__, dockerInfo->mirrorName);
    char command[DOCKER_CMD_MAX_LENGTH];
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1];
    memset(command, 0, DOCKER_CMD_MAX_LENGTH);
    memset(result, 0, DOCKER_CMD_RESULT_MAX_LENGHT + 1);

    //build start container commnad
    char tmpStr[1024];
    strcat(command,"docker run -dit ");
    if (strlen(dockerInfo->cpuSet) != 0)
    {
        char tmp[64] = {0};
        sprintf(tmp, " --cpuset-cpus=%s ", dockerInfo->cpuSet);
        strcat(command, tmp);
    }

    //网络模式设置
    switch (dockerInfo->net)
    {
    //桥接模式
    case NET_BRIDEG:
        strcat(command, " --net=bridge ");
        break;
    //容器间通信
    case NET_CONTAINER_NAME:
    case NET_CONTAINER_ID:
        memset(tmpStr, 0, 1024);
        sprintf(tmpStr, " --net=%s ", dockerInfo->containerId);
        strcat(command, tmpStr);
        break;
    //host模式
    case NET_CONTAINER_HOST:
        strcat(command, " --net=host ");
        break;
    case NET_NONE:
        strcat(command, " --net=none ");
        break;
    default:
        SMIOS_FMT_DEBUG("%s, invalid net config\n", __FUNCTION__);
        return -1;
    }
    
    //内存门限
    if (strlen(dockerInfo->memThreshold) > 0)
    {
        char tmp[256] = {0};
        sprintf(tmp, " -m %s ", dockerInfo->memThreshold);
        strcat(command, tmp);
    }

    //映射端口
    if (strlen(dockerInfo->exposePort) > 0)
    {
        char tmp[256] = {0};
        sprintf(tmp, " --expose=%s ", dockerInfo->exposePort);
        strcat(command, tmp);
    }

    //映射目录数量
    if (dockerInfo->shareDirCount > 0)
    {
        //映射目录
        char tmp[256] = {0};
        sint32 index = 0;
        for (index = 0; index < dockerInfo->shareDirCount; index++ )
        {
            sprintf(tmp, " -v %s:%s:%s ", dockerInfo->shareDir[index].directory,
                                        dockerInfo->shareDir[index].directory,
                                        dockerInfo->shareDir[index].permission);
            strcat(command, tmp);
            memset(tmp, 0, 256);

        }
    }

    //文件节点数量
    if (dockerInfo->shareFileNodeCount > 0)
    {
        char tmp[256] = {0};
        sint32 index = 0;
        //映射文件节点
        for (index = 0; index < dockerInfo->shareFileNodeCount; index++ )
        {
            sprintf(tmp, " --device=%s:%s:%s ", dockerInfo->shareFileNode[index].fileNode,
                                        dockerInfo->shareFileNode[index].fileNode,
                                        dockerInfo->shareFileNode[index].permission);
            strcat(command, tmp);
            memset(tmp, 0, 256);
        }
    }

    //运行容器名称
    if (strlen(containerName) != 0)
    {
        char tmp[256] = {0};
        sprintf(tmp, " --name=%s ", containerName);
        strcat(command, tmp);
    }

    //容器权限配置
    strcat(command, "--cap-add=ALL --privileged=true ");

    //镜像名
    if (strlen(dockerInfo->mirrorName) != 0)
    {
        strcat(command, dockerInfo->mirrorName);
    }

    //SMIOS_FMT_DEBUG("%s, cmd[%s]\n", __FUNCTION__, command);
    //启动容器
    docker_cmd_execute(command, result);

    SMIOS_FMT_DEBUG("the docker start result[%s]\n", result);
    if (strlen(result) != 0)
    {
        SMIOS_FMT_DEBUG("command execute success!!\n");
        SMIOS_FMT_DEBUG("command : %s\n", result);
    } else {
        SMIOS_FMT_DEBUG("command execute failed!!\n");
        return -1;
    }

    //save the id of docker
    char containerId[DOCKER_ID_LENGTH + 1];
    memset(containerId, 0, DOCKER_ID_LENGTH + 1);
    strncpy(containerId, result, DOCKER_ID_LENGTH);
    SMIOS_FMT_DEBUG("%s, container id : %s\n", __FUNCTION__, containerId);

    CONTAINER_INFO_T containerInfo;
    memset(&containerInfo, 0, sizeof(containerInfo));
    strncpy(containerInfo.containerId, containerId, DOCKER_ID_LENGTH);
    strncpy(containerInfo.containerName, containerName, DOCKER_NAME_MAX_LENGTH);
    strncpy(containerInfo.dockerName, dockerInfo->mirrorName, DOCKER_NAME_MAX_LENGTH);

    //将容器信息添加到hash表中
    containerTbl->put(containerTbl, containerInfo.containerName, &containerInfo, sizeof(containerInfo));
    return 0;
}

/**
*********************************************************************
* @brief： 停止指定的docker
* @param[in]： dockerId  docker名称
* @param[out]：none
* @return： 返回值说明 -1 停止失败
                      0 停止成功
*********************************************************************
*/
sint32 docker_manager_docker_stop(char* dockerId)
{
    //参数有效性检查
    if (NULL == dockerId)
    {
        SMIOS_FMT_DEBUG("%s, the docker id is null\n", __FUNCTION__);
        return -1;
    }

    //remove the app from register list
    SMIOS_FMT_DEBUG("%s, remove the app from register list\n", __FUNCTION__);
    char appNameArray[APP_MAX_COUNT][APP_NAME_LENGTH + 1];
    sint32 appCount = 0;
    sint32 i = 0;
    //获得容器中app的名称
    memset(appNameArray, 0, APP_MAX_COUNT * (APP_NAME_LENGTH + 1));
    docker_manager_container_app_name_get(dockerId, (char*)appNameArray, &appCount);
    for (i = 0; i < appCount; i++)
    {
        SMIOS_FMT_DEBUG("%s, appName[%s]\n", __FUNCTION__, appNameArray[i]);
        //从注册列表中删除app信息
        app_register_list_delete(appNameArray[i]);
    }

    SMIOS_FMT_DEBUG("%s, stop docker[%s]\n", __FUNCTION__, dockerId);
    char command[DOCKER_CMD_MAX_LENGTH + 1];
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1];
    memset(command, 0, DOCKER_CMD_MAX_LENGTH + 1);
    memset(result, 0, DOCKER_CMD_RESULT_MAX_LENGHT + 1);

    //停止容器
    sprintf(command, "docker stop %s", dockerId);
    docker_cmd_execute(command, result);

    //将容器删除
    memset(command, 0, DOCKER_CMD_MAX_LENGTH);
    sprintf(command, "docker rm -f %s", dockerId);
    docker_cmd_execute(command, result);

    //remove the container from the container table
    containerTbl->remove(containerTbl, dockerId);

    return 0;
}

/**
*********************************************************************
* @brief： 容器镜像下载
* @param[in]： url        下载链接
               mirrorName 镜像名称
               md5Sum     下载包的md5摘要值
* @param[out]：none
* @return： 返回值说明 -1 下载失败
                      0 下载成功
*********************************************************************
*/
sint32 docker_manager_mirror_download(char* url, char* mirrorName, char* md5Sum)
{
    SMIOS_FMT_DEBUG("%s, mirror download\n", __FUNCTION__);
    //参数有效性检查
    if (NULL == url || NULL == mirrorName || NULL == md5Sum)
    {
        SMIOS_FMT_DEBUG("%s, the input value is invalid\n", __FUNCTION__);
        return -1;
    }

    //download mirror
    char command[256] = {0};
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1] = {0};
    sprintf(command, "wget -O %s/%s.tar.gz %s", g_configObj.serviceCfg.localAppInstallDir,mirrorName, url);
    sint32 ret = docker_cmd_execute(command, result);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, execute command[%s] fail\n", __FUNCTION__, command);
        return -1;
    }

    //对下载文件进行校验
    ret = docker_download_package_check(mirrorName, md5Sum);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, the download package[%s] is invalid\n", __FUNCTION__, mirrorName);
        return -1;
    }

    //解压缩下载文件
    docker_package_decompress(mirrorName);

    //handle the CERT dir
    //get the sha1sum
    char certSFPath[1024] = {0};
    sprintf(certSFPath, "%s/tmp/%s/CERT/CERT.SF", g_configObj.serviceCfg.localAppInstallDir, mirrorName);
    FILE* fp = fopen(certSFPath, "r");
    if (NULL == fp)
    {
        SMIOS_FMT_DEBUG("can not open file[%s]\n", certSFPath);
        //如果打开失败，则安装失败，删除压缩包
        docker_package_delete(mirrorName);
        return -1;
    }

    char buffer[1024];
    memset(buffer, 0, 1024);

    //读取校验文件信息
    size_t size = fread(buffer, 1,1024, fp);
    (void)size;
    //SMIOS_FMT_DEBUG("read context:\n");
    //SMIOS_FMT_DEBUG("%s\n\n", buffer);

    //获取文件名称
    char* ptr = strstr(buffer, "Name:");
    char name[256] = {0};
    sscanf(ptr, "Name:%s\n", name);
    //SMIOS_FMT_DEBUG("name1 : %s\n", name);

    //获取sha1值
    ptr = strstr(ptr, "SHA1-Digest:");
    char sum[256] = {0};
    sscanf(ptr, "SHA1-Digest:%s\n", sum);
    //SMIOS_FMT_DEBUG("sum1 : %s\n", sum);
    char debInfoStr[1024] = {0};
    sprintf(debInfoStr, "%s/tmp/%s/%s", g_configObj.serviceCfg.localAppInstallDir,mirrorName, mirrorName);
    //镜像有效性检测
    ret = docker_install_package_check(debInfoStr, sum);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("check the mirror image failed\n");
        docker_package_delete(mirrorName);
        return -1;
    }

    ptr = strstr(ptr, "Name:");
    memset(name, 0, 256);
    sscanf(ptr, "Name:%s\n", name);
    //SMIOS_FMT_DEBUG("name2 : %s\n", name);

    ptr = strstr(ptr, "SHA1-Digest:");
    memset(sum, 0, 256);
    sscanf(ptr, "SHA1-Digest:%s\n", sum);
    //SMIOS_FMT_DEBUG("sum2 : %s\n", sum);
    char packageInfoStr[1024] = {0};
    sprintf(packageInfoStr, "%s/tmp/%s/packageInfo", g_configObj.serviceCfg.localAppInstallDir, mirrorName);
    //packageInfo 有效性检查
    ret = docker_install_package_check(packageInfoStr, sum);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("check the packageinfo failed\n");
        //packageInfo无效，需要删除镜像包，安装失败
        docker_package_delete(mirrorName);
        return -1;
    }

    char signInfoStr[1024] = {0};
    sprintf(signInfoStr, "%s/tmp/%s/CERT/CERT.RSA", g_configObj.serviceCfg.localAppInstallDir, mirrorName);
    //数字签名校验
    ret = docker_digital_sign_check(signInfoStr, mirrorName);
    if (ret < 0)
    {
        docker_package_delete(mirrorName);
        return -1;
    }

/*
    SMIOS_FMT_DEBUG("now the docker check success!!!\n");
    SMIOS_FMT_DEBUG("package info path[%s]\n", packageInfoStr);
    DOCKER_INFO_T dockerInfo;
    memset(&dockerInfo, 0, sizeof(dockerInfo));
    docker_manager_parse_config(packageInfoStr, &dockerInfo);
    char mirrorPath[1024] = {0};
    sprintf(mirrorPath, "%s/tmp/%s/%s", g_configObj.serviceCfg.localAppInstallDir, tmpFileName, dockerInfo.mirrorPackage);
    docker_manager_mirror_load(mirrorPath);

    //delete the install package
    docker_package_delete(tmpFileName);
*/
    return 0;
}

/**
*********************************************************************
* @brief： 容器镜像文件升级
* @param[in]： url         下载路径
               mirrorName  镜像名称
               md5sum      md5校验值
* @param[out]：none
* @return： 返回值说明 -1 更新失败
                      0 更新成功
*********************************************************************
*/
sint32 docker_manager_mirror_update(char* url, char* mirrorName, char* md5,  UPGRADE_FILE_TYPE type, char* md5sum)
{
    //有效性检查
    if (NULL == url || NULL == mirrorName || NULL == md5)
    {
        SMIOS_FMT_DEBUG("invalid param\n");
        return -1;
    }

    sint32 ret = 0;
    //download the mirror
    ret = docker_manager_mirror_download(url, mirrorName, md5);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("download docker image fail\n");
        //下载失败，删除镜像包
        docker_package_delete(mirrorName);
        return -1;
    }

    //差分升级
    if (DIFFERENCE_UPGRADE == type)
    {
        if (md5sum == NULL)
        {
            SMIOS_FMT_DEBUG("invalid param\n");
            return -1;
        }
        SMIOS_FMT_DEBUG("difference upgrade, md5sum[%s]\n", md5sum);

        //patch
        char oldfile[256] = {0}; 
        char newfile[256] = {0};
        char patch[256] = {0};
        sprintf(oldfile, "%s/package/%s.tar.gz", g_configObj.serviceCfg.localAppInstallDir, mirrorName);
        sprintf(newfile, "%s/%s.tar.gz", g_configObj.serviceCfg.localAppInstallDir, mirrorName);
        sprintf(patch, "%s/tmp/%s/patch.patch", g_configObj.serviceCfg.localAppInstallDir, mirrorName);
        
        //打差分包
        char command[512] = {0};
        sprintf(command, "bspatch %s %s %s", oldfile, newfile, patch);
        SMIOS_FMT_DEBUG("command[%s]\n", command);
        system(command);

        //check md5sum
        ret = docker_install_package_check(newfile, md5sum);
        if (ret == 0)
        {
            SMIOS_FMT_DEBUG("difference upgrade md5 check success\n");
        } else {
            SMIOS_FMT_DEBUG("difference upgrade md5 check failed\n");
            return -1;
        }

        //包重名命
        memset(command, 0, sizeof(command));
        sprintf(command, "cp %s %s", newfile, oldfile);
        system(command);
        //decompress
        docker_package_decompress(mirrorName);
    }

    //将使用该镜像的容器停止
    qhashtbl_obj_t obj;
    memset(&obj, 0, sizeof(obj));
    containerTbl->lock(containerTbl);
    while (containerTbl->getnext(containerTbl, &obj, true) == true)
    {
        CONTAINER_INFO_T* pContainerInfo = (CONTAINER_INFO_T*)(obj.data);
        SMIOS_FMT_DEBUG("tbl docker name[%s]\n", pContainerInfo->dockerName);
        if(strcmp(pContainerInfo->dockerName, mirrorName) == 0)
        {
            //停止容器
            docker_manager_docker_stop(pContainerInfo->containerName);
        }

        free(obj.data);
        free(obj.name);
    }
    containerTbl->unlock(containerTbl);

    //load the image
    docker_manager_mirror_load(mirrorName);

    //delete the install package
    //docker_package_delete(mirrorName);

    return 0;
}
/**
*********************************************************************
* @brief： 容器镜像删除
* @param[in]： mirrorName 镜像名称
* @param[out]：none
* @return： 返回值说明 -1 删除失败
                      0 删除成功
*********************************************************************
*/
sint32 docker_manager_mirror_delete(char* mirrorName)
{
    SMIOS_FMT_DEBUG("%s, delete mirror[%s]\n", __FUNCTION__, mirrorName);
    //有效性检查
    if (NULL == mirrorName)
    {
        SMIOS_FMT_DEBUG("%s, the mirror name is invalid\n", __FUNCTION__);
        return -1;
    }

    //将使用该镜像的容器停止
    qhashtbl_obj_t obj;
    memset(&obj, 0, sizeof(obj));
    containerTbl->lock(containerTbl);
    while (containerTbl->getnext(containerTbl, &obj, true) == true)
    {
        CONTAINER_INFO_T* pContainerInfo = (CONTAINER_INFO_T*)(obj.data);
        SMIOS_FMT_DEBUG("%s, tbl docker name[%s]\n", __FUNCTION__, pContainerInfo->dockerName);
        if(strcmp(pContainerInfo->dockerName, mirrorName) == 0)
        {
            //停止容器
            docker_manager_docker_stop(pContainerInfo->containerName);
        }

        //释放分配的空间
        free(obj.data);
        free(obj.name);
    }
    containerTbl->unlock(containerTbl);

    char commnad[DOCKER_CMD_MAX_LENGTH] = {0};
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1] = {0};
    memset(commnad, 0, DOCKER_CMD_MAX_LENGTH);
    memset(result, 0, DOCKER_CMD_RESULT_MAX_LENGHT);

    //删除镜像
    sprintf(commnad, "docker rmi -f %s", mirrorName);
    docker_cmd_execute(commnad, result);
    //将镜像从hash表中去除
    dockerTbl->remove(dockerTbl, mirrorName);
    //镜像信息保存
    docker_manager_config_save();

    return 0;
}

/**
*********************************************************************
* @brief： 容器信息获取
* @param[in]： none
* @param[out]：data 容器信息
               strLength 信息长度
* @return： 返回值说明 -1 获取容器信息失败
                      0 获取容器信息成功
*********************************************************************
*/
sint32 docker_manager_mirror_info_get(char* data, sint32* strLength)
{
    //有效性检查
    if (NULL == data || NULL == strLength)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }

    //镜像hash表是否有效
    if (NULL == dockerTbl)
    {
        SMIOS_FMT_DEBUG("%s, the mirror table is invalid\n", __FUNCTION__);
        return -1;
    }

    //构建镜像信息
    strcat(data, " \"imageInfo\": [");

    DOCKER_INFO_T* pDockerStartInfo = NULL;
    qhashtbl_obj_t obj;
    uint32 dockerStartInfoCount = 0;
    memset(&obj, 0, sizeof(qhashtbl_obj_t));
    char recordCtx[2048] = {0};
    dockerTbl->lock(dockerTbl);
    while (dockerTbl->getnext(dockerTbl, &obj, true) == true) {
        pDockerStartInfo = (DOCKER_INFO_T*)obj.data;
        dockerStartInfoCount = dockerStartInfoCount + 1;
        sprintf(recordCtx, "{\"Name\":\"%s\", \"ENV\":\"%s\", \"MemoryThreshold\":\"%s\", \"ExposePort\":\"%s\", \"CpuSet\":\"%s\", \
                \"CpuShares\":\"%s\", \"Net\":\"%d\", \"ShareDir\":[",
            pDockerStartInfo->mirrorName, pDockerStartInfo->env, pDockerStartInfo->memThreshold, pDockerStartInfo->exposePort,
            pDockerStartInfo->cpuSet, pDockerStartInfo->cpuShares, pDockerStartInfo->net);
            SMIOS_FMT_DEBUG("%s, shareDirCount[%d]\n", __FUNCTION__, pDockerStartInfo->shareDirCount);
        if (pDockerStartInfo->shareDirCount > 0)
        {
            int index = 0;
            char shareDir[128] = {0};
            for (index = 0; index < pDockerStartInfo->shareDirCount - 1; index++)
            {
                sprintf(shareDir, "{\"directory\":\"%s\", \"permission\":\"%s\"},", pDockerStartInfo->shareDir[index].directory,
                        pDockerStartInfo->shareDir[index].permission);
                strcat(recordCtx, shareDir);
                memset(shareDir, 0, 128);
            }
            sprintf(shareDir, "{\"directory\":\"%s\", \"permission\":\"%s\"}", pDockerStartInfo->shareDir[index].directory,
                    pDockerStartInfo->shareDir[index].permission);
            strcat(recordCtx, shareDir);
        } 
        strcat(recordCtx, "],");
        strcat(recordCtx, "\"ShareFileNode\":[");
        if (pDockerStartInfo->shareFileNodeCount > 0)
        {
            int index = 0;
            char shareDir[128] = {0};
            for (index = 0; index < pDockerStartInfo->shareFileNodeCount - 1; index++)
            {
                sprintf(shareDir, "{\"device\":\"%s\", \"permission\":\"%s\"},", pDockerStartInfo->shareFileNode[index].fileNode,
                        pDockerStartInfo->shareFileNode[index].permission);
                strcat(recordCtx, shareDir);
                memset(shareDir, 0, 128);
            }
            sprintf(shareDir, "{\"device\":\"%s\", \"permission\":\"%s\"}", pDockerStartInfo->shareFileNode[index].fileNode,
                    pDockerStartInfo->shareFileNode[index].permission);
            strcat(recordCtx, shareDir);
        }

        if (dockerStartInfoCount < dockerTbl->size(dockerTbl))
        {
            strcat(recordCtx, "]},\n");
        } else {
            strcat(recordCtx, "]}\n");
        }
        //SMIOS_FMT_DEBUG("record context : %s\n", data);
        //释放分配的空间
        free(obj.name);
        free(obj.data);
    }
    dockerTbl->unlock(dockerTbl);
    strcat(data, recordCtx);
    strcat(data, "]");

    *strLength = strlen(data);

    return 0;
}

/**
*********************************************************************
* @brief： 获取容器列表
* @param[in]： none
* @param[out]：none
* @return： 返回值说明 容器列表
*********************************************************************
*/
qhashtbl_t* docker_manager_get_container_table_head(void)
{
    return containerTbl;
}

/**
*********************************************************************
* @brief： 获取镜像列表
* @param[in]： none
* @param[out]：none
* @return： 返回值说明 镜像列表
*********************************************************************
*/
qhashtbl_t* docker_manager_get_mirror_table_head(void)
{
    return dockerTbl;
}

/**
*********************************************************************
* @brief： 镜像文件解压缩
* @param[in]： fileName  镜像文件
* @param[out]：none
* @return： 返回值说明 -1 解压缩失败
                      0 解压缩成功
*********************************************************************
*/
sint32 docker_package_decompress(char* fileName)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    //有效性检查
    if (NULL == fileName)
    {
        SMIOS_FMT_DEBUG("%s, the file name is invalid\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG("%s, decompress app package\n", __FUNCTION__);
    //安装包解压缩
    char tarPath[512 + 1] = {0};
    char tmpCmd[512] = {0};
    char result[1024] = {0};
    sprintf(tarPath, "%s/%s.tar.gz", g_configObj.serviceCfg.localAppInstallDir, fileName);
    sprintf(tmpCmd, "tar -xf %s -C %s/tmp", tarPath, g_configObj.serviceCfg.localAppInstallDir);
    SMIOS_FMT_DEBUG("depress command : %s\n", tmpCmd);
    app_cmd_execute(tmpCmd, result);

    //delete the tar
    SMIOS_FMT_DEBUG("%s, delete docker image package\n", __FUNCTION__);
    memset(tmpCmd, 0, sizeof(tmpCmd));
    memset(result, 0, sizeof(result));
    sprintf(tmpCmd, "rm -rf %s", tarPath);
    app_cmd_execute(tmpCmd, result);
    return 0;
}

/**
*********************************************************************
* @brief： 容器镜像包校验
* @param[in]： fileName  镜像名称
               md5Sum    md5校验值
* @param[out]：none
* @return： 返回值说明 -1 校验失败
                      0 校验成功
*********************************************************************
*/
sint32 docker_download_package_check(char* fileName, char* md5Sum)
{
#ifdef DOCKER_CERT_SWITCH
    //有效性检查
    if (NULL == fileName || NULL == md5Sum)
    {
        SMIOS_FMT_DEBUG("%s, the input param is invalid\n", __FUNCTION__);
        return -1;
    }

    //安装包路径
    char tmpFileName[128] = {0};
    sprintf(tmpFileName, "%s/%s.tar.gz", g_configObj.serviceCfg.localAppInstallDir, fileName);
    SMIOS_FMT_DEBUG("fileName[%s]\n", tmpFileName);
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
    //md5 初始化
    MD5_Init(&md5Ctx);
    //计算安装包md5值
    while(!feof(fp))
    {
        size_t size = fread(md5Buffer, 1, APP_MD5_READ_MAX_LENGTH, fp);
        MD5_Update(&md5Ctx, md5Buffer, size);
    }
    //md5计算结束
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
    //SMIOS_FMT_DEBUG("result:\n");
    char md5Buf[33] = {0};
    for (i = 0; i < 16; i++)
    {
        memset(restult, 0, 10);
        snprintf(restult, sizeof(restult), "%08x", (int)sum[i]);
        //SMIOS_FMT_DEBUG("%s\n", &(restult[6]));
        strcat(md5Buf, &(restult[6]));
    }

    SMIOS_FMT_DEBUG("md5 result: %s, len[%zu]\n", md5Buf, strlen(md5Buf));
    SMIOS_FMT_DEBUG("md5 sum:%s, len[%zu]\n", md5Sum, strlen(md5Sum));
    //比较md5值
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
* @brief： 容器镜像文件校验
* @param[in]： fileName  镜像名称
               sha1Sum   sha1校验值
* @param[out]：none
* @return： 返回值说明 -1 校验失败
                      0 校验成功
*********************************************************************
*/
sint32 docker_install_package_check(char* fileName, char* sha1Sum)
{
#ifdef DOCKER_CERT_SWITCH
    //有效性检查
    if (NULL == fileName || NULL == sha1Sum)
    {
        SMIOS_FMT_DEBUG("%s, the input param is invalid\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG("%s, check file[%s]\n", __FUNCTION__, fileName);
    //打开文件
    FILE* fp = fopen(fileName, "rb");
    if (NULL == fp)
    {
        SMIOS_FMT_DEBUG("%s, open file[%s] failed\n", __FUNCTION__, fileName);
        return -1;
    }

    SHA_CTX shaCtx;
    memset(&shaCtx, 0, sizeof(SHA_CTX));
    //sha1计算初始化
    SHA1_Init(&shaCtx);
    uint8 shaSum[SHA_DIGEST_LENGTH + 1];
    char mdShaSum[SHA_DIGEST_LENGTH*2 + 1] = {0};
    uint8 buffer[APP_SHA1_READ_MAX_LENGTH + 1];
    memset(buffer, 0, APP_SHA1_READ_MAX_LENGTH + 1);
    //计算sha1
    while(!feof(fp))
    {
        size_t size = fread(buffer, 1, APP_SHA1_READ_MAX_LENGTH, fp);
        //SMIOS_FMT_DEBUG("[app] read context[%s]\n", buffer);
        SHA1_Update(&shaCtx, buffer, size);
        memset(buffer, 0, APP_SHA1_READ_MAX_LENGTH+1);
    }

    //sha1计算结束
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
    SMIOS_FMT_DEBUG("sha1 sum[%s]\n", mdShaSum);
    //比较sha1值
    if (strcmp(mdShaSum, sha1Sum) != 0)
    {
        SMIOS_FMT_DEBUG("%s, the file[%s] is invalid\n", __FUNCTION__, fileName);
        return -1;
    }
#else
    (void)fileName;
    (void)sha1Sum;
#endif
    SMIOS_FMT_DEBUG("%s, check file[%s] success\n", __FUNCTION__, fileName);
    return 0;
}

/**
*********************************************************************
* @brief： 容器证书读取
* @param[in]： verify_cert  证书名称
               cert_len     证书名称长度
* @param[out]：str          读取的证书内容
               str_len      读取的证书内容长度
* @return： 返回值说明 -1 读取证书失败
                      0 读取证书成功
*********************************************************************
*/
sint32 docker_load_cert(uint8 *str, uint32 *str_len,
              const char *verify_cert, const uint32 cert_len)
{
    FILE *fp;
    //打开证书
    fp = fopen(verify_cert, "rb");
    if ( NULL == fp)
    {
        SMIOS_FMT_DEBUG("%s, open file fail\n", __FUNCTION__);
        return -1;
    }

    //读取证书内容
    *str_len = fread(str, 1, cert_len, fp);
    fclose(fp);
    return 0;
}

/**
*********************************************************************
* @brief： 证书内容由der格式转为x509格式
* @param[in]： der_str         证书内容
               der_str_len     证书内容长度
* @param[out]：none
* @return： 返回值说明 X509*
*********************************************************************
*/
X509 *docker_der_to_x509(const uint8 *der_str, uint32 der_str_len)
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

/**
*********************************************************************
* @brief： 证书验证
* @param[in]： usrCertName       用户证书名
               containerName     容器名
* @param[out]：none
* @return： 返回值说明 0  验证成功
                     -1 验证失败
*********************************************************************
*/
sint32 docker_x509_verify(char* usrCertName, char*containerName)
{
    (void)usrCertName;
    int ret;
    char cert[MAX_LEGTH] = {0};

    uint8 user_der[MAX_LEGTH] = {0};
    uint32 user_der_len;
    X509 *user = NULL;

    uint8 ca_der[MAX_LEGTH] = {0};
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
    /* 从文件中读取 */
    SMIOS_FMT_DEBUG("%s, root cert[%s]\n", __FUNCTION__, cert);
    ret = docker_load_cert(ca_der, &ca_der_len, cert, MAX_LEGTH);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, load cert file failed\n", __FUNCTION__);
        goto EXIT;
    }
    /* DER编码转X509结构 */
    ca = docker_der_to_x509(ca_der, ca_der_len);
    /* 加入证书存储区 */
    ret = X509_STORE_add_cert(ca_store, ca);
    if ( ret != 1 )
    {
        SMIOS_FMT_DEBUG("[%s], X509_STORE_add_cert fail, ret = %d\n", __FUNCTION__, ret);
        goto EXIT;
    }

    /* 需要校验的证书 */
    memset(cert, 0, MAX_LEGTH);
    sprintf(cert, "%s/tmp/%s/CERT/CERT.RSA", g_configObj.serviceCfg.localAppInstallDir, containerName);
    SMIOS_FMT_DEBUG("%s, user cert[%s]\n", __FUNCTION__, cert);
    docker_load_cert(user_der, &user_der_len, cert, MAX_LEGTH);
    user = docker_der_to_x509(user_der, user_der_len);
    if (user == NULL)
    {
        SMIOS_FMT_DEBUG("%s, der to x509 failed\n", __FUNCTION__);
        goto EXIT;
    }

    X509_NAME* tmpName = X509_get_subject_name(user);
    char sha1buf[64] = {0};
    X509_NAME_get_text_by_NID(tmpName, NID_commonName, sha1buf, 64);
    SMIOS_FMT_DEBUG("NID_commonName[%s]\n", sha1buf);
    char certRsaFile[256] = {0};
    sprintf(certRsaFile, "%s/tmp/%s/CERT/%s", g_configObj.serviceCfg.localAppInstallDir, containerName, APP_CERT_SF);
    SMIOS_FMT_DEBUG("%s, docker cert rsa [%s]\n", __FUNCTION__, certRsaFile);
    ret = docker_install_package_check(certRsaFile, sha1buf);
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
    } else {
        SMIOS_FMT_DEBUG("%s, verify success\n", __FUNCTION__);
    }
EXIT:
    X509_free(user);
    X509_free(ca);

    X509_STORE_CTX_cleanup(ctx);
    X509_STORE_CTX_free(ctx);

    X509_STORE_free(ca_store);

    return ret == 1 ? 0 : -1;
}

/**
*********************************************************************
* @brief： 证书验证
* @param[in]： fileName          文件名
               containerName     容器名
* @param[out]：none
* @return： 返回值说明 0  验证成功
                     -1 验证失败
*********************************************************************
*/
sint32 docker_digital_sign_check(char* fileName, char* containerName)
{
#ifdef DOCKER_CERT_SWITCH
    SMIOS_FMT_DEBUG("[docker] %s\n", __FUNCTION__);
    OpenSSL_add_all_algorithms();
    return docker_x509_verify(fileName, containerName);
#else
    (void)fileName;
    (void)containerName;
    return 0;
#endif
}

/**
*********************************************************************
* @brief： 证书验证
* @param[in]： fileName          文件名
               containerName     容器名
* @param[out]：none
* @return： 返回值说明 0  验证成功
                     -1 验证失败
*********************************************************************
*/
sint32 docker_package_delete(char* packageName)
{
    char command[256] = {0};
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1] = {0};
    sprintf(command, "rm -rf %s/tmp/%s", g_configObj.serviceCfg.localAppInstallDir, packageName);
    docker_cmd_execute(command, result);
    return 0;
}

/**
*********************************************************************
* @brief： 容器配置文件解析
* @param[in]： fileName          文件名
* @param[out]：dockerInfo        容器信息
* @return： 返回值说明 0  解析成功
                     -1 解析失败
*********************************************************************
*/
sint32 docker_manager_parse_config(char* filename, DOCKER_INFO_T* dockerInfo, APP_START_INFO_T* appStartInfo)
{
    //有效性检查
    if (NULL == filename || NULL == dockerInfo || NULL == appStartInfo)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }

    FILE* fp = NULL;
    char buf[2048] = {0};
    SMIOS_FMT_DEBUG("%s, start parse the container start list[%s]\n", __FUNCTION__, filename);
    //打开配置文件
    if ((fp = fopen(filename, "r")) == NULL)
    {
        SMIOS_FMT_DEBUG("%s, can not open file\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG("Open file success\n");
    int ret = fread(buf, 1, 2048, fp);

    if (ret > 0)
    {
        fclose(fp);
        fp = NULL;
        //json字符串处理
        cJSON* root = cJSON_Parse(buf);
        if (NULL == root)
        {
            SMIOS_FMT_DEBUG("Parse docker info error\n");
            return -1;
        }
        //cJSON_Print(root);

        //get type value
        cJSON* value = cJSON_GetObjectItem(root, "type");
        if (value == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        char* type = value->valuestring;
        SMIOS_FMT_DEBUG("type : %s\n", type);
/*
        if (strcmp(type, "docker") != 0)
        {
            SMIOS_FMT_DEBUG("%s, the config file is invalid\n", __FUNCTION__);
            return -1;
        }
*/
        //get config value
        cJSON* value2 = cJSON_GetObjectItem(root, "config");
        if (value == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        //get name value
        cJSON* value3 = cJSON_GetObjectItem(value2, "Name");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        char* name = value3->valuestring;
        strncpy(dockerInfo->mirrorName, name, DOCKER_NAME_MAX_LENGTH);
        SMIOS_FMT_DEBUG("name: %s\n", name);

        //get mirrorPackage value
        value3 = cJSON_GetObjectItem(value2, "mirrorPackage");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        char* mirrorPackage = value3->valuestring;
        SMIOS_FMT_DEBUG("mirrorPackage : %s\n", mirrorPackage);
        strncpy(dockerInfo->mirrorPackage, mirrorPackage, DOCKER_NAME_MAX_LENGTH);

        //get evn value
        value3 = cJSON_GetObjectItem(value2, "ENV");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        char* env = value3->valuestring;
        SMIOS_FMT_DEBUG("env : %s\n", env);
        strncpy(dockerInfo->env, env, DOCKER_ENV_MAX_LENGTH);

        //get MemoryThreshold value
        value3 = cJSON_GetObjectItem(value2, "MemoryThreshold");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        char* memThreshold = value3->valuestring;
        strncpy(dockerInfo->memThreshold, memThreshold, DOCKER_MEM_LIMT_LENGTH);
        SMIOS_FMT_DEBUG("MemoryThreshold: %s\n", memThreshold);

        //get ExposePort value
        value3 = cJSON_GetObjectItem(value2, "ExposePort");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        char* exposePort = value3->valuestring;
        strncpy(dockerInfo->exposePort, exposePort, DOCKER_EXPOSE_PORT_LENGTH);
        SMIOS_FMT_DEBUG("ExposePort: %s\n", exposePort);

        //get CpuSet value
        value3 = cJSON_GetObjectItem(value2, "CpuSet");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        char* cpuSet = value3->valuestring;
        strncpy(dockerInfo->cpuSet, cpuSet, DOCKER_CPU_SET_MAX_LENGTH);
        SMIOS_FMT_DEBUG("cpu set: %s\n", cpuSet);

        //get CpuShares value
        value3 = cJSON_GetObjectItem(value2, "CpuShares");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        char* cpuShares = value3->valuestring;
        SMIOS_FMT_DEBUG("Cpu Shares: %s\n", cpuShares);
        strncpy(dockerInfo->cpuShares, cpuShares, DOCKER_CPU_SHARES_MAX_LENGTH);

        //get Restart value
        value3 = cJSON_GetObjectItem(value2, "Restart");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        char* restart = value3->valuestring;
        SMIOS_FMT_DEBUG("Restart: %s\n", restart);

        //get Net value
        value3 = cJSON_GetObjectItem(value2, "Net");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }
        char* net = value3->valuestring;
        SMIOS_FMT_DEBUG("Net: %s\n", net);
        if (strcmp(net, "bridge") == 0)
        {
            SMIOS_FMT_DEBUG("net bridge\n");
            dockerInfo->net = NET_BRIDEG;
        } else if(strcmp(net, "host") == 0)
        {
            SMIOS_FMT_DEBUG("net host\n");
            dockerInfo->net = NET_CONTAINER_HOST;
        }

        //get ShareDir value
        value3 = cJSON_GetObjectItem(value2, "ShareDir");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        char* shareDir = value3->valuestring;
        SMIOS_FMT_DEBUG("shareDir: %s\n", shareDir);

        int shareDirLen = cJSON_GetArraySize(value3);
        SMIOS_FMT_DEBUG("shareDirLen = %d\n", shareDirLen);
        dockerInfo->shareDirCount = shareDirLen;
        if (shareDirLen > DOCKER_SHARE_DIR_MAX_COUNT)
        {
            SMIOS_FMT_DEBUG("%s, the share directory count exceed the max count\n", __FUNCTION__);
            return -1;
        }

        cJSON* directory  = NULL;
        cJSON* permission = NULL;
        int index = 0;
        for (index = 0; index < shareDirLen; index++)
        {
            //get directory value
            directory  = cJSON_GetArrayItem(value3, index);
            //get permission
            permission = cJSON_GetArrayItem(value3, index);

            directory = cJSON_GetObjectItem(directory, "directory");
            permission = cJSON_GetObjectItem(permission, "permission");
            if (directory == NULL || permission == NULL)
            {
                SMIOS_FMT_DEBUG("parse the docker config file failed\n");
                return -1;
            }

            char* tmpDirectory  = directory->valuestring;
            char* tmpPermission = permission->valuestring;

            //SMIOS_FMT_DEBUG("directory:%s, permisson:%s\n", tmpDirectory, tmpPermission);
            strncpy(dockerInfo->shareDir[index].directory,  tmpDirectory,  DOCKER_DIR_ARRAY_MAX_LENGTH);
            strncpy(dockerInfo->shareDir[index].permission, tmpPermission, DOCKER_PERMISSION_ARRAY_MAX_LEN);
        }

        //get ShareFileNode value
        value3 = cJSON_GetObjectItem(value2, "ShareFileNode");
        if (value3 == NULL)
        {
            SMIOS_FMT_DEBUG("GetObjectItem error\n");
            return -1;
        }

        //char* shareFileNode = value3->valuestring;
        //SMIOS_FMT_DEBUG("shareFileNode: %s\n", shareFileNode);

        int shareFileNodeLen = cJSON_GetArraySize(value3);
        //SMIOS_FMT_DEBUG("shareFileNodeLen = %d\n", shareFileNodeLen);
        dockerInfo->shareFileNodeCount = shareFileNodeLen;
        if (shareFileNodeLen > DOCKER_SHARE_FILE_NODE_MAX_COUNT)
        {
            SMIOS_FMT_DEBUG("%s, the share file node exceed the max count\n", __FUNCTION__);
            return -1;
        }
        
        cJSON* device = NULL;
        permission    = NULL;
        index = 0;
        for (index = 0; index < shareFileNodeLen; index++)
        {
            //get device value
            device = cJSON_GetArrayItem(value3, index);
            //get permission value
            permission = cJSON_GetArrayItem(value3, index);

            device = cJSON_GetObjectItem(device, "device");
            permission = cJSON_GetObjectItem(permission, "permission");
            //有效性检查
            if (device == NULL || permission == NULL)
            {
                SMIOS_FMT_DEBUG("parse the docker config file failed\n");
                return -1;
            }
            char* tmpDevice = device->valuestring;
            char* tmpPermission = permission->valuestring;

            //SMIOS_FMT_DEBUG("device:%s, permisson:%s\n", tmpDevice, tmpPermission);
            strncpy(dockerInfo->shareFileNode[index].fileNode, tmpDevice, DOCKER_FILE_NODE_NAME_MAX_LEN);
            strncpy(dockerInfo->shareFileNode[index].permission, tmpPermission, DOCKER_PERMISSION_ARRAY_MAX_LEN);
        }

        //get app Info
        value3  = cJSON_GetObjectItem(value2, "app");
        int len = cJSON_GetArraySize(value3);
        SMIOS_FMT_DEBUG("element count = %d\n", len);

        cJSON* jAppName             = NULL;                     //app name
        cJSON* jVersion             = NULL;                     //version
        cJSON* jDockerName          = NULL;                     //docker name
        cJSON* jStartParam          = NULL;                     //start param
        cJSON* jDataPath            = NULL;                     //data path
        cJSON* jPort                = NULL;                     //port
        cJSON* jNet                 = NULL;                     //network
        cJSON* jExposePort          = NULL;                     //expose port
        cJSON* jMemThreshold        = NULL;                     //memory threshold
        cJSON* jContainerName       = NULL;                     //container name
        cJSON* jCpuRateThreshold    = NULL;                     //cpu rate threshold
        cJSON* jFlashSizeThreshold  = NULL;                     //flash size threshold
        cJSON* value4               = NULL;
        int i = 0;
        for( i = 0; i < len; i++ )
        {
            value4 = cJSON_GetArrayItem(value3, i);
            do {
                //get app name
                jAppName = cJSON_GetObjectItem(value4, "app_name");
                if (jAppName == NULL)
                {
                    SMIOS_FMT_DEBUG("The app_name is invalid\n");
                    ret = -1;
                    break;
                }

                //get version value
                jVersion = cJSON_GetObjectItem(value4, "version");
                if (jVersion == NULL)
                {
                    SMIOS_FMT_DEBUG("The version is invalid\n");
                    ret = -1;
                    break;
                }

                //get docker name
                jDockerName = cJSON_GetObjectItem(value4, "docker_name");
                if (jDockerName == NULL)
                {
                    SMIOS_FMT_DEBUG("The docker_name is invalid\n");
                    ret = -1;
                    break;
                }

                //get start parameter
                jStartParam = cJSON_GetObjectItem(value4, "start_param");
                if (jStartParam == NULL)
                {
                    SMIOS_FMT_DEBUG("The start_param is invalid\n");
                    ret = -1;
                    break;
                }

                //get data path
                jDataPath = cJSON_GetObjectItem(value4, "dataPath");
                if (jDataPath == NULL)
                {
                    SMIOS_FMT_DEBUG("The dataPath is invalid\n");
                    ret = -1;
                    break;
                }

                //get port value
                jPort = cJSON_GetObjectItem(value4, "port");
                if (jPort == NULL)
                {
                    SMIOS_FMT_DEBUG("The port is invalid\n");
                    ret = -1;
                    break;
                }

                //get net value
                jNet = cJSON_GetObjectItem(value4, "net");
                if (jNet == NULL)
                {
                    SMIOS_FMT_DEBUG("The net is invalid\n");
                    ret = -1;
                    break;
                }

                //get expose port value
                jExposePort = cJSON_GetObjectItem(value4, "exposePort");
                if (jExposePort == NULL)
                {
                    SMIOS_FMT_DEBUG("The exposePort is invalid\n");
                    ret = -1;
                    break;
                }

                //get memory threshold
                jMemThreshold = cJSON_GetObjectItem(value4, "memThreshold");
                if (jMemThreshold == NULL)
                {
                    SMIOS_FMT_DEBUG("The memThreshold is invalid\n");
                    ret = -1;
                    break;
                }

                //get container value
                jContainerName = cJSON_GetObjectItem(value4, "container_name");
                if (jContainerName == NULL)
                {
                    SMIOS_FMT_DEBUG("The container_name is invalid\n");
                    ret = -1;
                    break;
                }

                //get cpu rate threshold
                jCpuRateThreshold = cJSON_GetObjectItem(value4, "cpuRateThreshold");
                if (jCpuRateThreshold == NULL)
                {
                    SMIOS_FMT_DEBUG("The cpuRateThreshold is invalid\n");
                    ret = -1;
                    break;
                }

                //get flash size threshold
                jFlashSizeThreshold = cJSON_GetObjectItem(value4, "flashSizeThreshold");
                if (jFlashSizeThreshold == NULL)
                {
                    SMIOS_FMT_DEBUG("The flashSizeThreshold is invalid\n");
                    ret = -1;
                    break;
                }
            } while(0);

            if (ret == -1)
            {
                return -1;
            }

            strncpy(appStartInfo->appName,            jAppName->valuestring,            APP_NAME_LENGTH);
            strncpy(appStartInfo->version,            jVersion->valuestring,            APP_VERSION_LENGTH);
            strncpy(appStartInfo->dockerName,         jDockerName->valuestring,         APP_DOCKER_NAME_LENGTH);
            strncpy(appStartInfo->startParam,         jStartParam->valuestring,         APP_START_PARAM_LENGTH);
            strncpy(appStartInfo->dataPath,           jDataPath->valuestring,           APP_DATA_PATH_LENGTH);
            strncpy(appStartInfo->exposePort,         jExposePort->valuestring,         APP_EXPOSE_PORT_LENGTH);
            strncpy(appStartInfo->memThreshold,       jMemThreshold->valuestring,       APP_MEM_LIMT_LENGTH);
            strncpy(appStartInfo->containerName,      jContainerName->valuestring,      APP_DOCKER_NAME_LENGTH);
            strncpy(appStartInfo->cpuRateThreshold,   jCpuRateThreshold->valuestring,   APP_CPU_RATE_LIMIT_LEN);
            strncpy(appStartInfo->flashSizeThreshold, jFlashSizeThreshold->valuestring, APP_FLASH_SIZE_LIMIT_LEN);
            
            //网络模式设置
            char* appNet = jNet->valuestring;
            SMIOS_FMT_DEBUG("Net: %s\n", appNet);
            if (strcmp(appNet, "bridge") == 0)
            {
                SMIOS_FMT_DEBUG("net bridge\n");
                appStartInfo->net = APP_NET_BRIDEG;
            } else if(strcmp(appNet, "host") == 0)
            {
                SMIOS_FMT_DEBUG("net host\n");
                appStartInfo->net = APP_NET_CONTAINER_HOST;
            }
            appStartInfo->uninstallFlag = UNINSTALL_FLAG_FALSE;
            appStartInfo->shareDirCount = dockerInfo->shareDirCount;
            int j = 0;
            for (j = 0; j < appStartInfo->shareDirCount; j++)
            {
                //app share directory
                strncpy(appStartInfo->shareDir[j].directory,  dockerInfo->shareDir[j].directory,  APP_DIR_ARRAY_MAX_LENGTH);
                //app share directory permisson
                strncpy(appStartInfo->shareDir[j].permission, dockerInfo->shareDir[j].permission, APP_PERMISSION_ARRAY_MAX_LEN);
            }
            appStartInfo->shareFileNodeCount = dockerInfo->shareFileNodeCount;
            for (j = 0; j < appStartInfo->shareFileNodeCount; j++)
            {
                strncpy(appStartInfo->shareFileNode[j].fileNode,   dockerInfo->shareFileNode[j].fileNode,   APP_FILE_NODE_NAME_MAX_LEN);
                strncpy(appStartInfo->shareFileNode[j].permission, dockerInfo->shareFileNode[j].permission, APP_PERMISSION_ARRAY_MAX_LEN);
            }
        }
        if (len > 0)
        {
            //save app start information
            app_start_list_save();
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
* @brief： 容器配置信息保存
* @param[in]： none
* @param[out]：none
* @return： 返回值说明 0  保存成功
                     -1 保存失败
*********************************************************************
*/
sint32 docker_manager_config_save(void)
{
    SMIOS_FMT_DEBUG("%s, save the docker start list...\n", __FUNCTION__);
    FILE* fp = NULL;

    //镜像列表有效性
    if (NULL == dockerTbl)
    {
        SMIOS_FMT_DEBUG("%s, the docker start table is NULL\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG("%s, file[%s]\n", __FUNCTION__, g_configObj.serviceCfg.dockerstartListFile);
    //open the docker start config file
    fp = fopen(g_configObj.serviceCfg.dockerstartListFile , "w");
    if (NULL == fp)
    {
        SMIOS_FMT_DEBUG("%s, open docker start list file failed!!!\n", __FUNCTION__);
        return -1;
    }

    //构造镜像配置信息
    char recordContext[DOKCER_RECORD_MAX_LENGTH + 1];
    memset(recordContext, 0, DOKCER_RECORD_MAX_LENGTH);

    strncpy(recordContext, "{\"docker_info\" :[\n", DOKCER_RECORD_MAX_LENGTH);
    fwrite(recordContext, 1, strlen(recordContext), fp);

    DOCKER_INFO_T* pDockerStartInfo = NULL;
    qhashtbl_obj_t obj;
    uint32 dockerStartInfoCount = 0;
    memset(&obj, 0, sizeof(qhashtbl_obj_t));

    dockerTbl->lock(dockerTbl);
    while (dockerTbl->getnext(dockerTbl, &obj, true) == true) {
        pDockerStartInfo = (DOCKER_INFO_T*)obj.data;
        dockerStartInfoCount = dockerStartInfoCount + 1;
        sprintf(recordContext, "{\"Name\":\"%s\", \"ENV\":\"%s\", \"MemoryThreshold\":\"%s\", \"ExposePort\":\"%s\", \"CpuSet\":\"%s\", \
                \"CpuShares\":\"%s\", \"Net\":\"%d\", \"ShareDir\":[",
            pDockerStartInfo->mirrorName, pDockerStartInfo->env, pDockerStartInfo->memThreshold, pDockerStartInfo->exposePort,
            pDockerStartInfo->cpuSet, pDockerStartInfo->cpuShares, pDockerStartInfo->net);
            SMIOS_FMT_DEBUG("%s, shareDirCount[%d]\n", __FUNCTION__, pDockerStartInfo->shareDirCount);
        if (pDockerStartInfo->shareDirCount > 0)
        {
            int index = 0;
            char shareDir[128] = {0};
            for (index = 0; index < pDockerStartInfo->shareDirCount - 1; index++)
            {
                sprintf(shareDir, "{\"directory\":\"%s\", \"permission\":\"%s\"},", pDockerStartInfo->shareDir[index].directory,
                        pDockerStartInfo->shareDir[index].permission);
                strcat(recordContext, shareDir);
                memset(shareDir, 0, 128);
            }
            sprintf(shareDir, "{\"directory\":\"%s\", \"permission\":\"%s\"}", pDockerStartInfo->shareDir[index].directory,
                    pDockerStartInfo->shareDir[index].permission);
            strcat(recordContext, shareDir);
        } 
        strcat(recordContext, "],");
        strcat(recordContext, "\"ShareFileNode\":[");
        if (pDockerStartInfo->shareFileNodeCount > 0)
        {
            int index = 0;
            char shareDir[128] = {0};
            for (index = 0; index < pDockerStartInfo->shareFileNodeCount - 1; index++)
            {
                sprintf(shareDir, "{\"device\":\"%s\", \"permission\":\"%s\"},", pDockerStartInfo->shareFileNode[index].fileNode,
                        pDockerStartInfo->shareFileNode[index].permission);
                strcat(recordContext, shareDir);
                memset(shareDir, 0, 128);
            }
            sprintf(shareDir, "{\"device\":\"%s\", \"permission\":\"%s\"}", pDockerStartInfo->shareFileNode[index].fileNode,
                    pDockerStartInfo->shareFileNode[index].permission);
            strcat(recordContext, shareDir);
        }

        if (dockerStartInfoCount < dockerTbl->size(dockerTbl))
        {
            strcat(recordContext, "]},\n");
        } else {
            strcat(recordContext, "]}\n");
        }
        //SMIOS_FMT_DEBUG("record context : %s\n", recordContext);
        fwrite(recordContext, 1, strlen(recordContext), fp);
        free(obj.name);
        free(obj.data);
    }
    dockerTbl->unlock(dockerTbl);

    memset(recordContext, 0, APP_RECORD_MAX_LENGTH);
    strncpy(recordContext, "]}", APP_RECORD_MAX_LENGTH);
    fwrite(recordContext, 1, strlen(recordContext), fp);

    fclose(fp);
    fp = NULL;
    return 0;
}

/**
*********************************************************************
* @brief： 容器镜像加载
* @param[in]： mirrorName        容器镜像名
* @param[out]：none
* @return： 返回值说明 0  加载成功
                     -1 加载失败
*********************************************************************
*/
sint32 docker_manager_mirror_load(char* mirrorName)
{
    SMIOS_FMT_DEBUG("%s, load mirror\n", __FUNCTION__);
    //有效性检查
    if (NULL == mirrorName)
    {
        SMIOS_FMT_DEBUG("%s, the mirror name is invalid\n", __FUNCTION__);
        return -1;
    }

    char packageInfoStr[1024] = {0};
    sprintf(packageInfoStr, "%s/tmp/%s/packageInfo", g_configObj.serviceCfg.localAppInstallDir,mirrorName);

    DOCKER_INFO_T dockerInfo;
    APP_START_INFO_T appStartInfo;
    memset(&dockerInfo, 0, sizeof(dockerInfo));
    memset(&appStartInfo, 0, sizeof(appStartInfo));
    //parse the docker config file
    sint32 ret = docker_manager_parse_config(packageInfoStr, &dockerInfo, &appStartInfo);
    if (ret == -1)
    {
        SMIOS_FMT_DEBUG("pharse config file failed\n");
        return -1;
    }
    //put the docker image info to docker hash table
    dockerTbl->put(dockerTbl, dockerInfo.mirrorName, &dockerInfo, sizeof(dockerInfo));
    //save the docker information
    docker_manager_config_save();

    char mirrorPath[1024] = {0};
    sprintf(mirrorPath, "%s/tmp/%s/%s", g_configObj.serviceCfg.localAppInstallDir, mirrorName, dockerInfo.mirrorPackage);

    SMIOS_FMT_DEBUG("%s, load mirror[%s]\n", __FUNCTION__, mirrorPath);
    char commnad[DOCKER_CMD_MAX_LENGTH];
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1];
    memset(commnad, 0, DOCKER_CMD_MAX_LENGTH);
    memset(result, 0, DOCKER_CMD_RESULT_MAX_LENGHT + 1);
    //load the docker image
    sprintf(commnad, "docker load --input %s", mirrorPath);
    docker_cmd_execute(commnad, result);

    //SMIOS_FMT_DEBUG("%s, ===========appName[%s]\n", __FUNCTION__, appStartInfo.appName);
    if (strlen(appStartInfo.appName) > 0)
    {
        //save app start information
        app_start_list_add(&appStartInfo);
        //start the app
        app_start(&appStartInfo);
        //save the app information
        app_start_list_save();
    }

    return 0;
}

/**
*********************************************************************
* @brief： 容器配置信息加载
* @param[in]： none
* @param[out]：none
* @return： 返回值说明 -1 加载失败
                      0 加载成功
*********************************************************************
*/
sint32 docker_manager_config_load(void)
{
    //镜像列表有效性检查
    if (NULL == dockerTbl)
    {
        SMIOS_FMT_DEBUG("%s, invalid docker table\n", __FUNCTION__);
        return -1;
    }

    //open the docker start list file
    FILE* fp = NULL;
    if ((fp = fopen(g_configObj.serviceCfg.dockerstartListFile, "r")) == NULL)
    {
        SMIOS_FMT_DEBUG("%s, Can not open file\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG("%s, Open file success\n", __FUNCTION__);
    //读取文件内容
    char buf[2048] = {0};
    int ret = fread(buf, 1, 2048, fp);

    if (ret > 0)
    {
        fclose(fp);
        fp = NULL;
        //处理json字符串
        cJSON* root = cJSON_Parse(buf);
        if (NULL == root)
        {
            SMIOS_FMT_DEBUG("%s, Parse docker info error\n", __FUNCTION__);
            return -1;
        }
        //cJSON_Print(root);

        //获取json root节点
        cJSON* value = cJSON_GetObjectItem(root, "docker_info");
        if (value == NULL)
        {
            SMIOS_FMT_DEBUG("%s, GetObjectItem error\n", __FUNCTION__);
            return -1;
        }
        //获取容器数组的数量
        int len = cJSON_GetArraySize(value);
        SMIOS_FMT_DEBUG("%s, element count = %d\n", __FUNCTION__, len);
        cJSON* jName            = NULL;                     //docker name
        cJSON* jEnv             = NULL;                     //docker environment
        cJSON* jMemoryThreshold = NULL;                     //docker memory threshold
        cJSON* jExposePort      = NULL;                     //expose prot
        cJSON* jCpuSet          = NULL;                     //use cpu set
        cJSON* jCpuShares       = NULL;                     //cpu share
        cJSON* jNet             = NULL;                     //the container nework pattern
        cJSON* jShareDir        = NULL;                     //the share directory which container needs
        cJSON* jShareFileNode   = NULL;                     //the file node which container needs
        DOCKER_INFO_T dockerInfo;
        int i = 0;
        for (i = 0; i < len; i++)
        {
            jName             = cJSON_GetArrayItem(value, i);
            jEnv              = cJSON_GetArrayItem(value, i);
            jMemoryThreshold  = cJSON_GetArrayItem(value, i);
            jExposePort       = cJSON_GetArrayItem(value, i);
            jCpuSet           = cJSON_GetArrayItem(value, i);
            jCpuShares        = cJSON_GetArrayItem(value, i);
            jNet              = cJSON_GetArrayItem(value, i);
            jShareDir         = cJSON_GetArrayItem(value, i);
            jShareFileNode    = cJSON_GetArrayItem(value, i);

            jName             = cJSON_GetObjectItem(jName, "Name");
            jEnv              = cJSON_GetObjectItem(jEnv, "Env");
            jMemoryThreshold  = cJSON_GetObjectItem(jMemoryThreshold, "MemoryThreshold");
            jExposePort       = cJSON_GetObjectItem(jExposePort, "ExposePort");
            jCpuSet           = cJSON_GetObjectItem(jCpuSet, "CpuSet");
            jCpuShares        = cJSON_GetObjectItem(jCpuShares, "CpuShares");
            jNet              = cJSON_GetObjectItem(jNet, "Net");
            jShareDir         = cJSON_GetObjectItem(jShareDir, "ShareDir");
            jShareFileNode    = cJSON_GetObjectItem(jShareFileNode, "ShareFileNode");

            //有效性检查
            if (jName == NULL || jEnv == NULL || jMemoryThreshold == NULL || jExposePort == NULL || jCpuSet == NULL ||
               jCpuShares == NULL || jNet == NULL || jShareDir == NULL || jShareFileNode == NULL)
            {
                SMIOS_FMT_DEBUG("load docker config file failed\n");
                return -1;
            }

            memset(&dockerInfo, 0, sizeof(dockerInfo));
            strncpy(dockerInfo.mirrorName,      jName->valuestring,             DOCKER_NAME_MAX_LENGTH);
            strncpy(dockerInfo.env,             jEnv->valuestring,              DOCKER_ENV_MAX_LENGTH);
            strncpy(dockerInfo.memThreshold,    jMemoryThreshold->valuestring,  DOCKER_MEM_LIMT_LENGTH);
            strncpy(dockerInfo.exposePort,      jExposePort->valuestring,       DOCKER_EXPOSE_PORT_LENGTH);
            strncpy(dockerInfo.cpuSet,          jCpuSet->valuestring,           DOCKER_CPU_SET_MAX_LENGTH);
            strncpy(dockerInfo.cpuShares,       jCpuShares->valuestring,        DOCKER_CPU_SHARES_MAX_LENGTH);
            char* tmpNet = jNet->valuestring;
            dockerInfo.net = atoi(tmpNet);
/*
            if (strcmp(tmpNet, "bridge") == 0)
            {
                SMIOS_FMT_DEBUG("%s, net bridge\n", __FUNCTION__);
                dockerInfo.net = NET_BRIDEG;
            } else if(strcmp(tmpNet, "host") == 0)
            {
                SMIOS_FMT_DEBUG("%s,  net host\n", __FUNCTION__);
                dockerInfo.net = NET_CONTAINER_HOST;
            }
*/
/*
            SMIOS_FMT_DEBUG("%s, Name: %s\n", __FUNCTION__, dockerInfo.mirrorName);
            SMIOS_FMT_DEBUG("%s, Env: %s\n", __FUNCTION__, dockerInfo.env);
            SMIOS_FMT_DEBUG("%s, MemoryThreshold: %s\n", __FUNCTION__, dockerInfo.memThreshold);
            SMIOS_FMT_DEBUG("%s, ExposePort: %s\n", __FUNCTION__, dockerInfo.exposePort);
            SMIOS_FMT_DEBUG("%s, CpuSet: %s\n", __FUNCTION__, dockerInfo.cpuSet);
            SMIOS_FMT_DEBUG("%s, CpuShares: %s\n", __FUNCTION__, dockerInfo.cpuShares);
            SMIOS_FMT_DEBUG("%s, Net: %d\n", __FUNCTION__, dockerInfo.net);
*/
            //获取映射目录数量
            int shareDirCount = cJSON_GetArraySize(jShareDir);
            SMIOS_FMT_DEBUG("%s, share directory count : %d\n\n", __FUNCTION__, shareDirCount);
            int index = 0;
            cJSON* jDirectory = NULL;
            cJSON* jPermission = NULL;
            dockerInfo.shareDirCount = shareDirCount;
            //获取映射目录及其访问权限
            for(index = 0; index < shareDirCount; index++)
            {
                jDirectory = cJSON_GetArrayItem(jShareDir, index);
                jPermission = cJSON_GetArrayItem(jShareDir, index);

                jDirectory = cJSON_GetObjectItem(jDirectory, "directory");
                jPermission = cJSON_GetObjectItem(jPermission, "permission");
                //有效性检查
                if (NULL == jDirectory || NULL == jPermission)
                {
                    SMIOS_FMT_DEBUG("load the docker config file failed\n");
                    return -1;
                }

                strncpy(dockerInfo.shareDir[index].directory, jDirectory->valuestring, DOCKER_DIR_ARRAY_MAX_LENGTH);
                strncpy(dockerInfo.shareDir[index].permission, jPermission->valuestring, DOCKER_PERMISSION_ARRAY_MAX_LEN);
                //SMIOS_FMT_DEBUG("%s, directory: %s\n", __FUNCTION__, dockerInfo.shareDir[index].directory);
                //SMIOS_FMT_DEBUG("%s, permission: %s\n", __FUNCTION__, dockerInfo.shareDir[index].permission);
            }

            //获取映射节点数量
            int shareFileNodeCount = cJSON_GetArraySize(jShareFileNode);
            //SMIOS_FMT_DEBUG("%s, share file node count : %d\n", __FUNCTION__, shareFileNodeCount);
            cJSON* jDevice = NULL;
            dockerInfo.shareFileNodeCount = shareFileNodeCount;
            //获取映射节点及其权限
            for(index = 0; index < shareFileNodeCount; index++)
            {
                jDevice = cJSON_GetArrayItem(jShareFileNode, index);
                jPermission = cJSON_GetArrayItem(jShareFileNode, index);

                jDevice = cJSON_GetObjectItem(jDevice, "device");
                jPermission = cJSON_GetObjectItem(jPermission, "permission");
                //有效性检查
                if (NULL == jDevice || NULL == jPermission)
                {
                    SMIOS_FMT_DEBUG("load the docker config file failed\n");
                    return -1;
                }

                strncpy(dockerInfo.shareFileNode[index].fileNode, jDevice->valuestring, DOCKER_FILE_NODE_NAME_MAX_LEN);
                strncpy(dockerInfo.shareFileNode[index].permission, jPermission->valuestring, DOCKER_PERMISSION_ARRAY_MAX_LEN);
                //SMIOS_FMT_DEBUG("%s, device: %s\n\n", __FUNCTION__, dockerInfo.shareFileNode[index].fileNode);
                //SMIOS_FMT_DEBUG("%s, permission: %s\n\n", __FUNCTION__, dockerInfo.shareFileNode[index].permission);
            }
            //add docker information to the docker hash table
            bool result = dockerTbl->put(dockerTbl, dockerInfo.mirrorName, &dockerInfo, sizeof(DOCKER_INFO_T));

            if (false == result) {
                SMIOS_FMT_DEBUG("%s, add docker[%s] to start table fail\n", __FUNCTION__, dockerInfo.mirrorName);
                return -1;
            }
        }
    } else {
        fclose(fp);
        fp = NULL;
    }

    return 0;
}

/**
*********************************************************************
* @brief： 容器名称获取
* @param[in]： none
* @param[out]：containerNameArray     容器名称数组
               containerCount         容器的数量
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 docker_manager_container_name_get(char* containerNameArray, sint32* containerCount)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    //有效性检查
    if (NULL == containerNameArray || NULL == containerCount)
    {
        SMIOS_FMT_DEBUG("%s, invalid parameter\n", __FUNCTION__);
        return -1;
    }
    char command[128] = {0};
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1] = {0};
    //get the container name
    sprintf(command, "docker stats --no-stream --format \"{{.Name}}\"");
    sint32 ret = docker_cmd_execute(command, result);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, get container name failed\n", __FUNCTION__);
        *containerCount = 0;
        return -1;
    }

    //check the result
    if (strlen(result) == 0)
    {
        *containerCount = 0;
        SMIOS_FMT_DEBUG("%s, there is no container\n", __FUNCTION__);
        return 0;
    }

    sint32 index = 0;
    char* pointer = result;
    char tmpName[128] = {0};
    char* tmpContainerName = containerNameArray;
    for( index = 0; index < DOCKER_MAX_COUNT; index++)
    {
        if (strlen(pointer) == 0)
        {
            break;
        }

        //put the container name into the containerNameArray
        memset(tmpName, 0, 128);
        sscanf(pointer, "%s\n", tmpName);
        SMIOS_FMT_DEBUG("%s, container name: %s\n", __FUNCTION__, tmpName);
        strncpy(tmpContainerName + (CONTAINER_NAME_MAX_LENGTH + 1) * index, tmpName, CONTAINER_NAME_MAX_LENGTH);
        pointer = pointer + strlen(tmpName) + 1;
    }
    *containerCount = index;
    return 0;
}

/**
*********************************************************************
* @brief： 容器CPU占用率
* @param[in]： containerName 容器名称
* @param[out]：cpuRate       cpu占用率
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 docker_manager_container_cpu_rate_get(const char* containerName, float* cpuRate)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    //参数有效性检查
    if (NULL == containerName || NULL == cpuRate)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }

    char command[128] = {0};
    char result[DOCKER_CMD_RESULT_MAX_LENGHT +1] = {0};
    //get the cpu rate of container
    sprintf(command, "docker stats --no-stream --format \"{{.CPUPerc}}\" %s", containerName);
    sint32 ret = docker_cmd_execute(command, result);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, get container CPU rate failed\n", __FUNCTION__);
        return -1;
    }

    //判断是否有查询结果输出
    if (strlen(result) == 0)
    {
        SMIOS_FMT_DEBUG("%s, there is no container\n", __FUNCTION__);
        return 0;
    }

    //get cpu rate
    SMIOS_FMT_DEBUG("%s, container[%s] cpu :[%s]\n", __FUNCTION__, containerName, result);
    sscanf(result, "%f", cpuRate);

    return 0;
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
sint32 docker_manager_container_app_name_get(const char* containerName, char* appNameArray, sint32* appCount)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    //get the app in the container
    return app_get_app_name_use_same_container(containerName, appNameArray, appCount);
}

/**
*********************************************************************
* @brief： 容器内app名称获取
* @param[in]： containerName 容器名称
* @param[out]：appNameArray  app名词数组
               appCount      app数量
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 docker_manager_app_name_get(const char* containerName, char* appNameArray, sint32* appCount)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    //参数有效性检查
    if (NULL == containerName || NULL == appNameArray || NULL == appCount)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }

    SMIOS_FMT_DEBUG("%s, containerName[%s]\n", __FUNCTION__, containerName);

    //get the app in the container
    app_get_app_name_in_container(containerName, appNameArray, appCount);
    SMIOS_FMT_DEBUG("%s, appCount[%d]\n", __FUNCTION__, *appCount);
/*
    sint32 index = 0;
    for(index = 0; index < *appCount; index++)
    {
        SMIOS_FMT_DEBUG("%s, app name[%s]\n", __FUNCTION__, appNameArray + index * APP_NAME_LENGTH);
    }
*/
    return 0;
}

/**
*********************************************************************
* @brief： 容器内存占用率
* @param[in]： containerName 容器名称
* @param[out]：memoryRate    内存占用率
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 docker_manager_container_mem_rate_get(const char* containerName, float* memoryRate)
{
    //参数有效性检查
    if (NULL == containerName || NULL == memoryRate)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }

    char command[128] = {0};
    char result[DOCKER_CMD_RESULT_MAX_LENGHT + 1] = {0};
    // get the memory rate of container
    sprintf(command, "docker stats --no-stream --format \"{{.MemPerc}}\" %s", containerName);
    sint32 ret = docker_cmd_execute(command, result);
    if (ret < 0)
    {
        SMIOS_FMT_DEBUG("%s, get container memory usage failed\n", __FUNCTION__);
        return -1;
    }

    //检查执行命令是否有结果输出
    if (strlen(result) == 0)
    {
        SMIOS_FMT_DEBUG("%s, there is no container\n", __FUNCTION__);
        *memoryRate = 0.0;
        return 0;
    }

    //获取内存使用率
    SMIOS_FMT_DEBUG("%s, container[%s] MemPerc[%s]\n", __FUNCTION__, containerName, result);
    sscanf(result, "%f", memoryRate);
    SMIOS_FMT_DEBUG("%s, memory perc[%f]\n", __FUNCTION__, *memoryRate);

    return 0;
}

/**
*********************************************************************
* @brief： 容器FLASH
* @param[in]： containerName 容器名称
* @param[out]：ramRate       ram占用率
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 docker_manager_container_flash_size_get(const char* containerName, char* flashSize)
{
    (void)containerName;
    (void)flashSize;
    return 0;
}

/**
*********************************************************************
* @brief： 容器APP cpu占用率
* @param[in]： containerName 容器名称
               appName       应用名称
* @param[out]：cpuRate       cpu占用率
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 docker_manager_app_cpu_rate_get(const char* containerName, const char* appName, float* cpuRate)
{
    //参数有效性检查
    if (NULL == containerName || NULL == appName || NULL == cpuRate)
    {
        SMIOS_FMT_DEBUG("%s, invaid param\n", __FUNCTION__);
        return -1;
    }

    char command[128] = {0};
    char result[256] = {0};
    //get the pid of app in the container
    sprintf(command, "docker exec -i %s ps -C %s | grep %s", containerName, appName, appName);
    app_cmd_execute(command, result);
    SMIOS_FMT_DEBUG("command result[%s]\n", result);
    sint32 progressId = 0;
    sscanf(result, "%d", &progressId);

    //get the cpu rate of app
    memset(command, 0, 128);
    memset(result, 0, 256);
    sprintf(command, "docker exec -i %s ps -u -p %d | grep %s", containerName, progressId, appName);
    app_cmd_execute(command, result);
    char usr[64] = {0};
    sint32 pid = 0;
    float memRate = 0.0;
    SMIOS_FMT_DEBUG("%s, result[%s]\n", __FUNCTION__, result);
    sscanf(result, "%s %d %f %f", usr, &pid, cpuRate, &memRate);
    SMIOS_FMT_DEBUG("%s, app[%s], cpuRate[%f]\n", __FUNCTION__, appName, *cpuRate);

    return 0;
}

/**
*********************************************************************
* @brief： 容器APP 内存占用率
* @param[in]： containerName 容器名称
               appName       应用名称
* @param[out]：memRate       内存占用率
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 docker_manager_app_mem_rate_get(const char* containerName, const char* appName, float* memRate)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    //参数有效性检查
    if (NULL == containerName || NULL == appName || NULL == memRate)
    {
        SMIOS_FMT_DEBUG("%s, invaid param\n", __FUNCTION__);
        return -1;
    }

    char command[128] = {0};
    char result[256] = {0};
    //get the pid of app in the container
    sprintf(command, "docker exec -i %s ps -C %s | grep %s", containerName, appName, appName);
    app_cmd_execute(command, result);
    SMIOS_FMT_DEBUG("command result[%s]\n", result);
    sint32 progressId = 0;
    //获取pid
    sscanf(result, "%d", &progressId);

    //get the memory rate of app 
    memset(command, 0, 128);
    memset(result, 0, 256);
    sprintf(command, "docker exec -i %s ps -u -p %d | grep %s", containerName, progressId, appName);
    app_cmd_execute(command, result);
    char usr[64] = {0};
    sint32 pid = 0;
    float cpuRate = 0.0;
    SMIOS_FMT_DEBUG("%s, result[%s]\n", __FUNCTION__, result);
    sscanf(result, "%s %d %f %f", usr, &pid, &cpuRate, memRate);
    SMIOS_FMT_DEBUG("%s, app[%s], memRate[%f]\n", __FUNCTION__, appName, *memRate);

    return 0;
}

/**
*********************************************************************
* @brief： 容器APP flash占用率
* @param[in]： containerName 容器名称
               appName       app名称
* @param[out]：flashSize     占用flash大小
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 docker_manager_app_flash_size_get(const char* appName, char* flashSize)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    //参数有效性检查
    if (NULL == appName || NULL == flashSize)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }

    //获取flash 占用率
    char command[256] = {0};
    char result[256] = {0};
    sprintf(command, "du -b %s/%s", g_configObj.serviceCfg.localAppConfigDir,appName);
    app_cmd_execute(command, result);
    SMIOS_FMT_DEBUG("%s, result:%s\n", __FUNCTION__, result);
    sscanf(result, "%s", flashSize);

    return 0;
}

/**
*********************************************************************
* @brief： 容器信息上报
* @param[in]： none
* @param[out]：data 上报信息
               length 上报信息长度
* @return： 返回值说明 -1 获取上报信息失败
                      0 获取上报信息成功
*********************************************************************
*/
sint32 docker_manager_info_report(char* data, sint32* length)
{
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    //参数有效性检查
    if (NULL == data)
    {
        SMIOS_FMT_DEBUG("%s, invalid param\n", __FUNCTION__);
        return -1;
    }
    int index = 0;
    float cpuRate = 0.0;
    float memRate = 0.0;
    char containerNameArray[DOCKER_MAX_COUNT][CONTAINER_NAME_MAX_LENGTH + 1];
    char appNameArray[APP_MAX_COUNT][APP_NAME_LENGTH + 1];
    sint32 containerCount = 0;
    sint32 appCount = 0;
    sint32 appIndex = 0;
    char* pData = data;

    strcat(pData, "{\"info\" : [");
    //get the container name
    memset(containerNameArray, 0, DOCKER_MAX_COUNT * (CONTAINER_NAME_MAX_LENGTH + 1));
    docker_manager_container_name_get((char*)containerNameArray, &containerCount);
    
    for(index = 0; index < containerCount; index++ )
    {
        //check the docker cpu percent
        char container[256];
        memset(container, 0, 256);
        docker_manager_container_cpu_rate_get(containerNameArray[index], &cpuRate);

        //check the docker flash percent
        docker_manager_container_mem_rate_get(containerNameArray[index], &memRate);
        sprintf(container, "{ \"container\": \"%s\", \"cpuRate\":\"%f\", \"memRate\":\"%f\",", containerNameArray[index], cpuRate, memRate);
        strcat(pData, container);
        //get the app array
        memset(appNameArray, 0, APP_MAX_COUNT * (APP_NAME_LENGTH + 1));
        docker_manager_container_app_name_get(containerNameArray[index], (char*)appNameArray, &appCount);
        strcat(pData, "\"app\":[");
        char appInfo[384] = {0};
        for (appIndex = 0; appIndex < appCount; appIndex++)
        {
            //get the cpu rate of app
            float appCpuRate = 0.0;
            docker_manager_app_cpu_rate_get(containerNameArray[index], appNameArray[appIndex], &appCpuRate);

            //get the memory rate of app
            float appMemRate = 0.0;
            docker_manager_app_mem_rate_get(containerNameArray[index], appNameArray[appIndex], &appMemRate);

            char appFlashSize[APP_FLASH_SIZE_STR_MAX_LEN];
            memset(appFlashSize, 0, APP_FLASH_SIZE_STR_MAX_LEN);
            //获取app的flash 占用率
            docker_manager_app_flash_size_get(appNameArray[appIndex], appFlashSize);

            memset(appInfo, 0, sizeof(appInfo));
            sprintf(appInfo, " {\"appName\":\"%s\",\"memRate\":\"%f\",\"cpuRate\":\"%f\",\"flashSize\":\"%s\"}", appNameArray[appIndex],
                    appMemRate, appCpuRate, appFlashSize);
            strcat(pData, appInfo);
            if (appIndex < (appCount - 1))
            {
                strcat(pData, ",");
            }
        }
        strcat(pData, "]}");
        if (index < (containerCount - 1))
        {
            strcat(pData, ",");
        }
    }
    strcat(pData, "]}");
    *length = strlen(pData);
    //SMIOS_FMT_DEBUG("[app] %s, report data:%s\n", __FUNCTION__, pData);
    return 0;
}

extern void get_app_info(char* containername,ALL_APP_INFO_T* appinfo);
extern void get_container_info(ALL_CONTAINER_INFO_T *allcontainer);
/**
*********************************************************************
* @brief： 容器监视函数
* @param[in]： arg
* @param[out]：none
* @return： 返回值说明 void*
*********************************************************************
*/
static void* createDockerMonitor(void* arg)
{
    (void)arg;
    prctl(PR_SET_NAME,"monitor");
    SMIOS_FMT_DEBUG("%s\n", __FUNCTION__);
    uint8 i;
//    int index = 0;
//    float cpuRate = 0.0;
//    float memRate = 0.0;
//    char containerNameArray[DOCKER_MAX_COUNT][CONTAINER_NAME_MAX_LENGTH + 1];
//    char appNameArray[APP_MAX_COUNT][APP_NAME_LENGTH + 1];
//    sint32 containerCount = 0;
//    sint32 appCount = 0;
//    sint32 appIndex = 0;
    
    //memset(&containerState,0x0,sizeof(containerState));

    memset(&allcontainerinfo,0x00,sizeof(ALL_CONTAINER_AND_APP_T));
    pthread_mutex_init(&allcontainerinfo.lock, NULL);
    ALL_CONTAINER_INFO_T AllContainer;
    struct timespec lasttp;
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC_RAW, &lasttp);
    while (1)
    {
        //一分钟检查一次是否需要关闭服务
        clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
        if(tp.tv_sec - lasttp.tv_sec >=60)
        {
            service_stop_check();
            lasttp = tp;
            memset(&AllContainer,0x00,sizeof(ALL_CONTAINER_INFO_T));
            get_container_info(&AllContainer);
            pthread_mutex_lock(&allcontainerinfo.lock);
            allcontainerinfo.count = AllContainer.containercount;
            for(i=0;i<AllContainer.containercount;i++)
            {
                memcpy(&allcontainerinfo.containers[i].container,&AllContainer.container[i],sizeof(EVERY_CONTAINER_T));
                get_app_info((char*)AllContainer.container[i].ContainerName,&allcontainerinfo.containers[i].apps);

            }
            pthread_mutex_unlock(&allcontainerinfo.lock);
            
        }
        sleep(1);
        //get the container name
//        memset(containerNameArray, 0, DOCKER_MAX_COUNT * (CONTAINER_NAME_MAX_LENGTH + 1));
//        docker_manager_container_name_get((char*)containerNameArray, &containerCount);
//        for( index = 0; index < containerCount; index++ )
//        {
//            //check the docker cpu percent
//            docker_manager_container_cpu_rate_get(containerNameArray[index], &cpuRate);
//
//            //check the docker flash percent
//            docker_manager_container_mem_rate_get(containerNameArray[index], &memRate);
//
//            //get the app array
//            memset(appNameArray, 0, APP_MAX_COUNT * (APP_NAME_LENGTH + 1));
//            docker_manager_app_name_get(containerNameArray[index], (char*)appNameArray, &appCount);
//            SMIOS_FMT_DEBUG("%s, the app count[%d]\n", __FUNCTION__, appCount);
//
//            for (appIndex = 0; appIndex < appCount; appIndex++)
//            {
//                //获取app的cpu占用率
//                float appCpuRate = 0.0;
//                docker_manager_app_cpu_rate_get(containerNameArray[index], appNameArray[appIndex], &appCpuRate);
//
//                //获取app的内存占用率
//                float appMemRate = 0.0;
//                docker_manager_app_mem_rate_get(containerNameArray[index], appNameArray[appIndex], &appMemRate);
//
//                //获取app的Flash占用率
//                char appFlashSize[APP_FLASH_SIZE_STR_MAX_LEN];
//                memset(appFlashSize, 0, APP_FLASH_SIZE_STR_MAX_LEN);
//                docker_manager_app_flash_size_get(appNameArray[appIndex], appFlashSize);
//                float flashSize;
//                sscanf(appFlashSize, "%f", &flashSize);
//
//                //获取app启动信息
//                APP_START_INFO_T* pAppStartInfo = app_get_app_start_info(appNameArray[appIndex]);
//                if (NULL == pAppStartInfo)
//                {
//                    SMIOS_FMT_DEBUG("%s, not found the app[%s]\n", __FUNCTION__, appNameArray[appIndex]);
//                    continue;
//                }
//
//                SMIOS_FMT_DEBUG("%s, app name[%s], cpuRateThreshold[%s], memThreshold[%s], flashSizeThreshold[%s]", __FUNCTION__, pAppStartInfo->appName,
//                            pAppStartInfo->cpuRateThreshold, pAppStartInfo->memThreshold, pAppStartInfo->flashSizeThreshold);
//                float cpuRateThreshold;
//                float memThreshold;
//                float flashSizeThreshold;
//                //char memUnit[5] = {0};
//                sscanf(pAppStartInfo->cpuRateThreshold, "%f", &cpuRateThreshold);
//                sscanf(pAppStartInfo->memThreshold, "%f", &memThreshold);
//                sscanf(pAppStartInfo->flashSizeThreshold, "%f", &flashSizeThreshold);
//                //SMIOS_FMT_DEBUG("%s, threshold cpuRate[%f], memRate[%f], flashSize[%f], memUnit[%s]", __FUNCTION__, cpuRateThreshold,
//                //            memThreshold, flashSizeThreshold, memUnit);
//                //cpu占用率超限
//                if (cpuRateThreshold < appCpuRate)
//                {
//                    //todo report
//                    SMIOS_FMT_DEBUG("%s, app[%s] cpuRate[%f], cpuRateThreshold[%f]\n", __FUNCTION__, pAppStartInfo->appName, appCpuRate, cpuRateThreshold);
//                }
//                //内存使用率超限
//                if (memThreshold < appMemRate)
//                {
//                    //todo report
//                    SMIOS_FMT_DEBUG("%s, app[%s] memRate[%f], memRateThreshold[%f]\n", __FUNCTION__, pAppStartInfo->appName, appMemRate, memThreshold);
//                }
//                //flash占用率超限
//                if (flashSizeThreshold < flashSize)
//                {
//                    //todo report
//                    SMIOS_FMT_DEBUG("%s, app[%s] flashSize[%f], flashSizeThreshold[%f]\n", __FUNCTION__, pAppStartInfo->appName, flashSize, flashSizeThreshold);
//                }
//
//                if (NULL != pAppStartInfo)
//                {
//                    free(pAppStartInfo);
//                    pAppStartInfo = NULL;
//                }
//              //保存容器信息提供给显示
//              strncpy(containerState.appName[containerState.appCnt],appNameArray[appIndex],sizeof(containerState.appName[containerState.appCnt]));
//              strncpy(containerState.appContainer[containerState.appCnt],containerNameArray[index],sizeof(containerState.appContainer[containerState.appCnt]));
//              containerState.appCpuRate[containerState.appCnt] = appCpuRate;
//              containerState.appMemRate[containerState.appCnt] = appMemRate;
//              containerState.appFlashSize[containerState.appCnt] = flashSize;
//              containerState.cpuRate[containerState.appCnt] = cpuRate;
//              containerState.memRate[containerState.appCnt] = memRate;
//              containerState.appCnt++;
//            }
//        }
    }

    return NULL;
}

/**
*********************************************************************
* @brief： 创建容器监视线程
* @param[in]： none
* @param[out]：none
* @return： void
*********************************************************************
*/
void createDockerMonitorThread(void)
{
    //创建容器监视线程
    pthread_t tidp;
    if ((pthread_create(&tidp, NULL, createDockerMonitor, NULL)) == -1)
    {
        SMIOS_FMT_DEBUG("create docker monitor thread failed\n");
    } else {
        SMIOS_FMT_DEBUG("create docker monitor thread success\n");
    }
    return;
}

//choice 0 开启ssh 1 关闭ssh
void ssh_control(uint8 choice)
{
    char cmd[100]={0};

    if(choice ==0)
    {
        sprintf(cmd,"systemctl start sshd");
    }else 
    {
        sprintf(cmd,"systemctl stop sshd");        
    }
    system(cmd);
    
    if(choice ==0)
    {
        sprintf(cmd,"/usr/sbin/sshd -D &");
    }else 
    {
        sprintf(cmd,"ps |grep /usr/sbin/sshd |grep -v grep|awk '{print $1}'|xargs kill all -9| pkill sshd");
    }
    system(cmd);
}

//检查ssh在不在
int ssh_check()
{
    char cmd[100]={0};
    sprintf(cmd,"ps |grep /usr/sbin/sshd |grep -v grep|awk '{print $1}'");
    char str[100]={0};
    shellcmd(cmd,str,100);
    if(strlen(str) == 0)
        return -1;
    else 
        return 0;
}

void sftp_control(uint8 choice)
{
    char cmd[100]={0};
    if(choice == 0)
    {
        sprintf(cmd,"sed -i '/sftp-server/s/#//g' /etc/ssh/sshd_config ");
    }else
    {
        sprintf(cmd,"sed -i '/sftp-server/s/^/#/g' /etc/ssh/sshd_config ");
    }
    system(cmd);
}

/**
*********************************************************************
* @brief：     启动服务 同时写入配置文件
* @param[in]：
               
* @param[out]：
               
* @return：    
*********************************************************************
*/
int service_start(SERVICE_START_T *serviceInfo)
{

    SERVICE_START_INFO_T allinfo;
    uint8 index;
    memset(&allinfo,0x00,sizeof(allinfo));
    allinfo.info[0].authorizeTime = 0xffffffff;
    allinfo.info[1].authorizeTime = 0xffffffff;
    if(serviceInfo->serviceType != 1 && serviceInfo->serviceType != 2)
    {
        SMIOS_FMT_DEBUG("不支持的服务类型 %d \n",serviceInfo->serviceType);
        return -1;
    }
    index = serviceInfo->serviceType - 1;
    pthread_mutex_lock(&gServiceInfoLock);
    if(access(gServiceInfoFile,R_OK)==0)
    {
        read_pdata_xram(gServiceInfoFile,(char*)&allinfo,0,sizeof(SERVICE_START_INFO_T));
    }
    memcpy(&allinfo.info[index],serviceInfo,sizeof(SERVICE_START_T));
    write_pdata_xram(gServiceInfoFile,(char*)&allinfo,0,sizeof(SERVICE_START_INFO_T));
    pthread_mutex_unlock(&gServiceInfoLock);
    if(serviceInfo->serviceType ==1)
    {
        //打开ssh服务
        ssh_control(0);
    }else
    {
        //打开sftp
        sftp_control(0);
        if(ssh_check() == 0)
        {
            ssh_control(1);
        }
        ssh_control(0);
    }
    return 0;

}

/**
*********************************************************************
* @brief：     启动服务 同时写入配置文件
* @param[in]：
               
* @param[out]：
               
* @return：    
*********************************************************************
*/
int service_stop(uint8 type)
{
    if(type != 1 && type != 2)
    {
        SMIOS_FMT_DEBUG("不支持的服务类型 %d \n",type);
        return -1;
    }
    SERVICE_START_INFO_T allinfo;
    uint8 index;
    memset(&allinfo,0x00,sizeof(allinfo));
    index = type -1;
    pthread_mutex_lock(&gServiceInfoLock);
    if(access(gServiceInfoFile,R_OK)==0)
    {
        read_pdata_xram(gServiceInfoFile,(char*)&allinfo,0,sizeof(SERVICE_START_INFO_T));
        allinfo.info[index].authorizeTime = 0xffffffff;
        write_pdata_xram(gServiceInfoFile,(char*)&allinfo,0,sizeof(SERVICE_START_INFO_T));
    }
    pthread_mutex_unlock(&gServiceInfoLock);

    if(type ==1)
    {
        //关闭ssh服务
        ssh_control(1);
    }else
    {
        //关闭sftp
        sftp_control(1);
        if(ssh_check() ==0)
        {
            SMIOS_FMT_DEBUG("重启ssh \n");
            ssh_control(1);
            ssh_control(0);
        }else
        {
            SMIOS_FMT_DEBUG("当前ssh已关闭 \n");
        }
    }
    return 0;
}

void service_stop_check()
{
    SERVICE_START_INFO_T allinfo;

    memset(&allinfo,0x00,sizeof(allinfo));

    pthread_mutex_lock(&gServiceInfoLock);
    if(access(gServiceInfoFile,R_OK)==0)
    {
        read_pdata_xram(gServiceInfoFile,(char*)&allinfo,0,sizeof(SERVICE_START_INFO_T));

        if(allinfo.info[0].authorizeTime != 0xffffffff )
        {
            SMIOS_FMT_DEBUG("ssh 授权时间还剩 %u \n",allinfo.info[0].authorizeTime);
            if(allinfo.info[0].authorizeTime > 0)
            {
                allinfo.info[0].authorizeTime--;
            }else
            {
                //授权时间到 关闭ssh
                ssh_control(1);
                allinfo.info[0].authorizeTime = 0xffffffff;
            }
        }
        if(allinfo.info[1].authorizeTime != 0xffffffff )
        {
            SMIOS_FMT_DEBUG("sftp 授权时间还剩 %u \n",allinfo.info[1].authorizeTime);
            if(allinfo.info[1].authorizeTime > 0)
            {
                allinfo.info[1].authorizeTime--;
            }else
            {
                //授权时间到 关闭ssh
                sftp_control(1);
                if(ssh_check() ==0)
                {
                    ssh_control(1);
                    ssh_control(0);
                }
                allinfo.info[1].authorizeTime = 0xffffffff;
            }
        }
        write_pdata_xram(gServiceInfoFile,(char*)&allinfo,0,sizeof(SERVICE_START_INFO_T));
    }
    pthread_mutex_unlock(&gServiceInfoLock);


}
