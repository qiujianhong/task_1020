/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： xlCmd.c
* @date： 2019-11-9
*********************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "baseType.h"
#include "cJSON.h"
#include "globalConfiguration.h"
#include "timeHelper.h"
#include "xlChannel.h"
#include "xlCmd.h"
#include "xlmPlatform.h"
#include "xlmPlatformDef.h"
#include "xlmPlatformInternal.h"
#include "framecomm.h"
#include "appManagerCommon.h"
#include "dockerManagerCommon.h"

typedef enum xl_cmd {
    TIME_REQ = 0,      //时间请求
} XL_CMD_T;

/**
*********************************************************************
* @brief： 时间请求函数
* @param[in]：
               platform
* @param[out]：
               none
* @return： none
*********************************************************************
*/
void platform_TimeReq(XL_PLATFORM_T *platform)
{
    UNUSED(platform);

    cJSON *rsp = NULL;
    sint32 mid;
    sint8  timeStr[MAX_TIME_STR_LEN];
    do
    {
        //创建json对象
        rsp = cJSON_CreateObject();
        if (rsp == NULL)
        {
            break;
        }

        //获取时间
        if (0 == get_unix_time_str(timeStr))
        {
            SMIOS_FMT_DEBUG("get_unix_time_str:%s\n", timeStr);
        }
        else
        {
            SMIOS_FMT_DEBUG("get_unix_time_str fail\n");
            break;
        }

        //添加消息类型
        if (NULL == cJSON_AddStringToObject(rsp, "msgType", "TimeReq"))
        {
            break;
        }
        //添加mid
        if (NULL == cJSON_AddNumberToObject(rsp, "mid", 1))
        {
            break;
        }
        //添加时间
        if (NULL == cJSON_AddStringToObject(rsp, "msgTime", timeStr))
        {
            break;
        }

        char *out = cJSON_Print(rsp);
        //SMIOS_FMT_DEBUG("body len:%zu\n", strlen(out));

        //发送消息
        xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                              platform->cmdPubTopic.qos, &mid, out,
                              strlen(out));

        //销毁json对象
        cJSON_Delete(rsp);
    } while (0);
}

/**
*********************************************************************
* @brief： 时间响应函数
* @param[in]：
               mid
               jsonObjStr
* @param[out]：
               none
* @return： none
*********************************************************************
*/
void platform_time_rsp(sint32 mid, sint8 * jsonObjStr)
{
    //处理json字符串
    cJSON *jsonObj = cJSON_Parse(jsonObjStr);
    // sint8  rspTimeStr[MAX_TIME_STR_LEN];
    UNUSED(mid);

    if (jsonObj == NULL)
    {
        SMIOS_FMT_DEBUG("cJSON_Parse fail [%s]\n", jsonObjStr);
    }
    else
    {
        //获取响应时间
        //char *tmp_rspTimeStr = cJSON_GetObjectItem(jsonObj, "resTime")->valuestring;
        //SMIOS_FMT_DEBUG("Time rsp :%s\n", tmp_rspTimeStr);
    }
}

/**
*********************************************************************
* @brief： 获取smios状态
* @param[in]：
               platform         平台信息
               mid
               jsonObjStr
* @param[out]：
               none
* @return： none
*********************************************************************
*/
void platform_get_smiOS_status(XL_PLATFORM_T *platform,
                               sint32         mid,
                               sint8 *        jsonObjStr)
{
    UNUSED(jsonObjStr);
    char data[2048] = {0};
    sint32 strLength = 0;
    //获取容器信息
    docker_manager_info_report(data, &strLength);
    const char *format =
        "{\"msgType\": \"stateReq\", 	\"mid\": 1, 	\"result\": %s }";
    
    char result[2048] = {0};
    //将获取的数据信息格式化保存到result中
    sprintf(result, format, data);
    //发送消息
    xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                          platform->cmdPubTopic.qos, &mid, result,
                          strlen(result));
}

/**
*********************************************************************
* @brief： 容器镜像管理函数
* @param[in]：
               platform         平台信息
               mid
               jsonObjStr
* @param[out]：
               none
* @return： none
*********************************************************************
*/
void platform_image_manager(XL_PLATFORM_T *platform,
                            sint8*         cmd,
                            sint32         mid,
                            sint8 *        jsonObjStr)
{
    char result[1024] = {0};
    sint32 ret = 0;
    if (NULL == cmd || NULL == jsonObjStr)
    {
        SMIOS_FMT_DEBUG("Invalid param!!\n");
        snprintf(result, sizeof(result),
             "{\"msgType\": \"%s\", \"mid\": %d, 	"
             "\"result\": false }",
             cmd, mid);
        //发送消息
        xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                        platform->cmdPubTopic.qos, &mid, result,
                        strlen(result));
        return;
    }

    //处理json字符串
    cJSON* root = cJSON_Parse(jsonObjStr);
    if (root == NULL)
    {
        SMIOS_FMT_DEBUG("GetObjectItem error\n");
        //error message
        snprintf(result, sizeof(result),
            "{\"msgType\": \"%s\", \"mid\": %d, 	"
            "\"result\": false }",
            cmd, mid);
        //发送消息
        xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                        platform->cmdPubTopic.qos, &mid, result,
                        strlen(result));
        return ;
    }
    cJSON_Print(root);
/*
    cJSON* value = cJSON_GetObjectItem(root, "url");
    char* url = cJSON_Print(value);
    value = cJSON_GetObjectItem(root,"md5");
    char* md5 = cJSON_Print(value);
    value = cJSON_GetObjectItem(root, "imageName");
    char* imageName = cJSON_Print(value);
    SMIOS_FMT_DEBUG("url[%s], imageName[%s], md5[%s]", url, imageName, md5);
*/
    //下载镜像
    if (strcmp(cmd, "downloadImage") == 0)
    {
        SMIOS_FMT_DEBUG("Download Image\n");
        do {
            //获取url
            cJSON* value = cJSON_GetObjectItem(root, "url");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("Invalid message\n");
                ret = -1;
                break;
            }
            char* url = value->valuestring;

            //获取md5
            value = cJSON_GetObjectItem(root,"md5");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("Invalid message\n");
                ret = -1;
                break;
            }
            char* md5 = value->valuestring;

            //get image name
            value = cJSON_GetObjectItem(root, "imageName");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("Invalid message\n");
                ret = -1;
                break;
            }
            char* imageName = value->valuestring;

            SMIOS_FMT_DEBUG("url[%s], imageName[%s], md5[%s]\n", url, imageName, md5);
            //down load the docker image
            ret = docker_manager_mirror_download(url, imageName, md5);
            if (ret == 0)
            {
                SMIOS_FMT_DEBUG("[docker] now the docker check success!!!\n");
                //load the docker image
                ret = docker_manager_mirror_load(imageName);

                //delete the install package
                docker_package_delete(imageName);
            }
        } while(0);
    } else if(strcmp(cmd, "updateImage") == 0)      //update the docker image
    {
        //SMIOS_FMT_DEBUG("Update Image\n");
        do {
            //get url
            cJSON* value = cJSON_GetObjectItem(root, "url");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* url = value->valuestring;

            //get md5
            value = cJSON_GetObjectItem(root,"md5");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* md5 = value->valuestring;
            
            //get image name
            value = cJSON_GetObjectItem(root, "imageName");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* imageName = value->valuestring;

            //get upgrade type
            value = cJSON_GetObjectItem(root, "upgradeType");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            sint32 upgradeType = atoi(value->valuestring);

            //差分升级
            if (upgradeType == DIFFERENCE_UPGRADE)
            {
                //SMIOS_FMT_DEBUG("url[%s], imageName[%s], md5[%s], upgradeType[%d]\n", url, imageName, md5, upgradeType);
                //获取md5
                value = cJSON_GetObjectItem(root, "md5Sum");
                if (value == NULL)
                {
                    SMIOS_FMT_DEBUG("md5Sum is invalid\n");
                    ret = -1;
                    break;
                }
                char* md5sum = value->valuestring;
                //镜像升级
                ret = docker_manager_mirror_update(url, imageName, md5, DIFFERENCE_UPGRADE, md5sum);
            }  else if (upgradeType == FULL_UPGRADE) //全版本升级
            {
                //镜像升级
                ret = docker_manager_mirror_update(url, imageName, md5, FULL_UPGRADE, NULL);
            } else 
            {
                ret = -1;
            }
        } while(0);
    } else if (strcmp(cmd, "deleteImage") == 0)  //卸载镜像
    {
        //SMIOS_FMT_DEBUG("Delete Image\n");
        do {
            //get image name
            cJSON* value = cJSON_GetObjectItem(root, "imageName");
            if (value == NULL)
            {
                SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* imageName = value->valuestring;

            //delete the image
            ret = docker_manager_mirror_delete(imageName);
        } while(0);
    } else if (strcmp(cmd, "getImageInfo") == 0)    //获取镜像信息
    {
        //SMIOS_FMT_DEBUG("Get Image Info\n");
        char data[2048] = {0};
        sint32 strLength = 0;
        //获取镜像信息
        docker_manager_mirror_info_get(data, &strLength);
        snprintf(result, sizeof(result),
            "{\"msgType\": \"%s\", \"mid\": %d, 	"
            "%s }",
            cmd, mid, data);
        //发送消息
        xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                          platform->cmdPubTopic.qos, &mid, result,
                          strlen(result));
        return;
    }else {
        SMIOS_FMT_DEBUG("Invalid param\n");
        ret = -1;
    }

    if (ret == 0) {
        //success
        snprintf(result, sizeof(result),
             "{\"msgType\": \"%s\", \"mid\": %d, "
             "\"result\": true }",
             cmd, mid);
    } else {
        //fail
        snprintf(result, sizeof(result),
             "{\"msgType\": \"%s\", \"mid\": %d, "
             "\"result\": false }",
             cmd, mid);
    }

    //发送消息
    xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                          platform->cmdPubTopic.qos, &mid, result,
                          strlen(result));
}

/**
*********************************************************************
* @brief： 容器管理函数
* @param[in]：
               platform         平台信息
               cmd              消息类型
               mid
               jsonObjStr
* @param[out]：
               none
* @return： none
*********************************************************************
*/
void platform_container_manager(XL_PLATFORM_T *platform,
                            sint8 *        cmd,
                            sint32         mid,
                            sint8 *        jsonObjStr)
{
    if (NULL == cmd || NULL == jsonObjStr)
    {
        SMIOS_FMT_DEBUG("Invalid param\n");
        return;
    }
    //SMIOS_FMT_DEBUG("cmd[%s]\n", cmd);
    //SMIOS_FMT_DEBUG("jsonObjStr:%s\n", jsonObjStr);

    char result[1024] = {0};
    sint32 ret = 0;

    //json对象处理
    cJSON* root = cJSON_Parse(jsonObjStr);
    //cJSON_Print(root);
    if (root == NULL)
    {
        SMIOS_FMT_DEBUG("GetObjectItem error\n");
        snprintf(result, sizeof(result),
            "{\"msgType\": \"%s\", \"mid\": %d, "
            "\"result\": false }",
            cmd, mid);
        //发送消息
        xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                        platform->cmdPubTopic.qos, &mid, result,
                        strlen(result));
        return ;
    }

    if (strcmp(cmd, "startContainer") == 0)
    {
        //启动容器
        do {
            //get the container id
            cJSON* value = cJSON_GetObjectItem(root, "containerID");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* containerID = value->valuestring;

            //get the mirror name which the container needs
            value = cJSON_GetObjectItem(root, "mirrorName");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* mirrorName = value->valuestring;
            //SMIOS_FMT_DEBUG("containerID[%s], mirrorName[%s]\n", containerID, mirrorName);
            //启动容器
            ret = docker_mangager_container_start(mirrorName, containerID);
        } while(0);
    } else if (strcmp(cmd, "stopContainer") == 0)
    {
        //停止容器
        do {
            //获取容器id
            cJSON* value = cJSON_GetObjectItem(root, "containerID");
            if (value == NULL)
            {
                SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* containerID = value->valuestring;
            //SMIOS_FMT_DEBUG("containerID[%s]\n", containerID);
            //停止容器
            ret = docker_manager_docker_stop(containerID);
        } while(0);
    } else if (strcmp(cmd, "deleteContainer") == 0)
    {
        //卸载容器
        //docker_manager_docker_remove(containerID);
    }

    if (ret == 0) {
        //success
        snprintf(result, sizeof(result),
             "{\"msgType\": \"%s\", \"mid\": %d,"
             "\"result\": true }",
             cmd,mid);
    } else {
        //fail
        snprintf(result, sizeof(result),
             "{\"msgType\": \"%s\", \"mid\": %d,"
             "\"result\": false }",
             cmd,mid);
    }

    //发送消息
    xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                          platform->cmdPubTopic.qos, &mid, result,
                          strlen(result));
}

/**
*********************************************************************
* @brief： app管理函数
* @param[in]：
               platform         平台信息
               cmd              消息类型
               mid
               jsonObjStr
* @param[out]：
               none
* @return： none
*********************************************************************
*/
void platform_app_manager(XL_PLATFORM_T *platform,
                          sint8 *        cmd,
                          sint32         mid,
                          sint8 *        jsonObjStr)
{
    char result[1024] = {0};
    sint32 ret = 0;
    if (NULL == cmd || NULL == jsonObjStr)
    {
        SMIOS_FMT_DEBUG("Invalid param\n");
        return;
    }
    SMIOS_FMT_DEBUG("cmd[%s]\n", cmd);
    SMIOS_FMT_DEBUG("jsonObjStr:%s\n", jsonObjStr);
    //下载app
    if (strcmp(cmd, "downloadApp") == 0)
    {
        //json对象处理
        cJSON* root = cJSON_Parse(jsonObjStr);
        //cJSON_Print(root);
        do {
            //get url
            cJSON* value = cJSON_GetObjectItem(root, "url");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* url = value->valuestring;

            //get app name
            value = cJSON_GetObjectItem(root, "appName");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* appName = value->valuestring;

            //get md5
            value = cJSON_GetObjectItem(root, "md5");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* md5 = value->valuestring;
            //SMIOS_FMT_DEBUG("appName[%s], url[%s], md5[%s]\n", appName, url, md5);
            //app download
            ret = app_download(url, appName, md5);
            if (ret == 0)
            {
                //app安装
                app_install(appName);
                //app安装包删除
                app_package_delete(appName);
            }
        } while(0);
    } else if (strcmp(cmd, "updateApp") == 0)  //app升级
    {
        //json对象处理
        cJSON* root = cJSON_Parse(jsonObjStr);
        //cJSON_Print(root);
        do {
            //get url
            cJSON* value = cJSON_GetObjectItem(root, "url");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* url = value->valuestring;

            //get app name
            value = cJSON_GetObjectItem(root, "appName");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* appName = value->valuestring;

            //get md5
            value = cJSON_GetObjectItem(root, "md5");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* md5 = value->valuestring;
            SMIOS_FMT_DEBUG("url[%s], md5[%s]\n", url, md5);

            //app升级
            ret = app_upgrade(url, appName, md5);
        } while(0);
    } else if (strcmp(cmd, "stopApp") == 0)     //停止app
    {
        //json对象处理
        cJSON* root = cJSON_Parse(jsonObjStr);
        //cJSON_Print(root);
        do {
            //get the app name
            cJSON* value = cJSON_GetObjectItem(root, "appName");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* appName = value->valuestring;
            SMIOS_FMT_DEBUG("appName[%s]\n", appName);

            //stop the app
            ret = app_stop(appName);
        } while(0);
    } else if (strcmp(cmd, "deleteApp") == 0)       //卸载app
    {
        //json对象处理
        cJSON* root = cJSON_Parse(jsonObjStr);
        //cJSON_Print(root);
        do {
            //get app name
            cJSON* value = cJSON_GetObjectItem(root, "appName");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* appName = value->valuestring;

            //获取卸载类型
            value = cJSON_GetObjectItem(root, "deleteType");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* deleteType = value->valuestring;
            SMIOS_FMT_DEBUG("appName[%s], delete type[%s]\n", appName, deleteType);

            //部分删除
            if (strcmp(deleteType, "\"part\"") == 0)
            {
                ret = app_uninstall(appName, DELETE_TYPE_PART);
            } else if (strcmp(deleteType, "\"all\"") == 0)  //全部删除
            {
                ret = app_uninstall(appName, DELETE_TYPE_ALL);
            } else
            {
                //SMIOS_FMT_DEBUG("Invalid param!!\n");
                ret = -1;
            }
        } while(0);
    } else if (strcmp(cmd, "restartApp") == 0)  //重启app
    {
        //json对象处理
        cJSON* root = cJSON_Parse(jsonObjStr);
        //cJSON_Print(root);
        do {
            //get app name
            cJSON* value = cJSON_GetObjectItem(root, "appName");
            if (value == NULL)
            {
                //SMIOS_FMT_DEBUG("The message is invalid\n");
                ret = -1;
                break;
            }
            char* appName = value->valuestring;
            SMIOS_FMT_DEBUG("restart appName[%s]\n", appName);

            //重启app
            ret = app_restart(appName);
        } while(0);
    }

    if (ret == 0) {
        //success
        snprintf(result, sizeof(result),
             "{\"msgType\": \"%s\", 	\"mid\": %d, 	"
             "\"result\": true }",
             cmd, mid);
    } else {
        //fail
        snprintf(result, sizeof(result),
             "{\"msgType\": \"%s\", 	\"mid\": %d, 	"
             "\"result\": false }",
             cmd, mid);
    }

    //发送消息
    xl_channel_mqtt_write(platform->ctx, platform->cmdPubTopic.topic,
                          platform->cmdPubTopic.qos, &mid, result,
                          strlen(result));
}