/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： appManager.c
* @date： 2019-11-12
*********************************************************************
*/
#include "appManager.h"
#include "appManagerCommon.h"
#include "appManagerDefinition.h"
#include "msgService.h"
#include "msgHandler.h"
#include "framecomm.h"

extern char APPID[APPID_LEN];
extern uint32 APPIDLen;
/**
*********************************************************************
* @brief： app管理器模块初始化 
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 app_manager_init_system(void)
{
    msg_handler_init(NULL);
    SMIOS_FMT_DEBUG("app_manager_init_system\n");
    return app_manager_init();
}

/**
*********************************************************************
* @brief： app管理器模块清理 
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void app_manager_clean_up_system(void)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    return;
}

/**
*********************************************************************
* @brief： app管理器模块启动 
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 app_manager_start_system(void)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    sint32 ret = app_all_start();
    return ret;
}

/**
*********************************************************************
* @brief： app管理器模块停止
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void app_manager_stop_system(void)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    //清除启动列表
    app_start_list_clear();
    //清除注册列表
    app_register_list_clear();

    return;
}

/**
*********************************************************************
* @brief： app注册
* @param[in]： msgData
               dataTime
* @param[out]：none
* @return： 返回值说明 -1 注册失败
                      0 注册成功
*********************************************************************
*/
sint32 app_manager_login_process(MSG_DATE_T* msgData, DATA_TIME_T *dataTime)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    //有效性检查
    if (NULL == msgData || NULL == dataTime) {
        SMIOS_FMT_DEBUG("Invalid message data\n");
        return -1;
    }
    APP_REGISTER_INFO_T appRegitsterInfo;
    memset(&appRegitsterInfo, 0, sizeof(APP_REGISTER_INFO_T));
    appRegitsterInfo.pid           = msgData->pid;
    appRegitsterInfo.subscribeInfo = msgData->subscribeInfo;
    strcpy(appRegitsterInfo.version ,msgData->version);
    appRegitsterInfo.appidlen = msgData->appidlen;
    memcpy(appRegitsterInfo.appid,msgData->appid,appRegitsterInfo.appidlen);

    MSG_CLIENT_T* message = &(msgData->msgClient);
    SMIOS_FMT_DEBUG( "type : %d\n", message->type);
    SMIOS_FMT_DEBUG( "client name : %s\n", message->clientName);
    strncpy(appRegitsterInfo.appName, message->clientName, APP_NAME_LENGTH);
    //消息协议类型
    switch (message->type)
    {
    case UDP:
        SMIOS_FMT_DEBUG( "UDP ip: %s\n", message->client.uClient.ip);
        SMIOS_FMT_DEBUG( "UDP port: %d\n", message->client.uClient.port);
        strncpy(appRegitsterInfo.protocolInfo.udp.ip, message->client.uClient.ip, APP_IP_MAX_LENGTH);
        appRegitsterInfo.protocolInfo.udp.port     = message->client.uClient.port;
        appRegitsterInfo.protocolInfo.protocolType = PROTOCOL_UDP;
        break;
    case TCP:
        SMIOS_FMT_DEBUG( "TCP ip: %s\n", message->client.tClient.ip);
        SMIOS_FMT_DEBUG( "TCP Port: %d\n", message->client.tClient.port);
        strncpy(appRegitsterInfo.protocolInfo.tcp.ip, message->client.tClient.ip, APP_IP_MAX_LENGTH);
        appRegitsterInfo.protocolInfo.tcp.port     = message->client.tClient.port;
        appRegitsterInfo.protocolInfo.protocolType = PROTOCOL_TCP;
        break;
    case IPC:
        SMIOS_FMT_DEBUG( "IPC : %s\n", message->client.iClient.unixSockertPath);
        strncpy(appRegitsterInfo.protocolInfo.ipc.unixSockertPath, message->client.iClient.unixSockertPath,
                UNIX_SOCKET_PATH_LENGTH);
        appRegitsterInfo.protocolInfo.protocolType = PROTOCOL_IPC;
        break;
    case MQTT:
        SMIOS_FMT_DEBUG( "recevice MQTT message\n");
        appRegitsterInfo.protocolInfo.protocolType = PROTOCOL_MQTT;
        break;
    default:
        SMIOS_FMT_DEBUG("reveive invalid message\n");
        return -1;
    }

    memcpy(&(appRegitsterInfo.dataTime), dataTime, sizeof(DATA_TIME_T));
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tv);

    appRegitsterInfo.tv.tv_sec = tv.tv_sec;
    appRegitsterInfo.tv.tv_nsec = tv.tv_nsec;

    //获取app启动信息
    APP_START_INFO_T* pAppStartInfo = app_get_app_start_info(appRegitsterInfo.appName);
    if (NULL != pAppStartInfo)
    {
        //get the app's container info and save it in the app register table
        strncpy(appRegitsterInfo.containerName, pAppStartInfo->containerName,
                APP_DOCKER_NAME_LENGTH);
        SMIOS_FMT_DEBUG( "find the app[%s] in the start list\n", appRegitsterInfo.appName);
    }

    //将app添加到注册列表
    sint32 ret = app_register_list_add(&appRegitsterInfo);

    if (pAppStartInfo != NULL)
    {
        free(pAppStartInfo);
    }

    return ret;
}

/**
*********************************************************************
* @brief： app取消注册
* @param[in]： appName
* @param[out]：none
* @return： 返回值说明 -1 取消注册失败
                      0 取消注册成功
*********************************************************************
*/
sint32 app_manager_logout_process(char* appName)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    //从注册列表中删除app
    return app_register_list_delete(appName);
}

/**
*********************************************************************
* @brief： 查询所有app注册信息
* @param[in]： none
* @param[out]：mqtPlugin 存储app的注册信息
               count     APP的数量
* @return： 返回值说明 -1 查询失败
                      0 查询成功
*********************************************************************
*/
sint32 app_manager_all_app_query(MQT_PLUGIN_T **mqtPlugin, uint32* count)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    app_all_app_query(mqtPlugin, count);
/*
    *count = app_register_app_count_get();
    if (0 == *count)
    {
        SMIOS_FMT_DEBUG("There is no app in the register table");
        return 0;
    }

    qhashtbl_t* pAppRegisterInfoTbl = app_register_list_head_get();
    if (NULL == pAppRegisterInfoTbl)
    {
        SMIOS_FMT_DEBUG("The app register table is NULL");
        return -1;
    }

    *mqtPlugin = calloc(*count, sizeof(MQT_PLUGIN_T));
    if (NULL == *mqtPlugin)
    {
        SMIOS_FMT_DEBUG("alloc buffer failed");
        return -1;
    }

    MQT_PLUGIN_T* pMqtPlutin = *mqtPlugin;
    APP_REGISTER_INFO_T* pAppRegisterInfo = NULL;
    qhashtbl_obj_t obj;
    memset(&obj, 0, sizeof(obj));
    pAppRegisterInfoTbl->lock(pAppRegisterInfoTbl);
    *count = 0;
    while(pAppRegisterInfoTbl->getnext(pAppRegisterInfoTbl, &obj, true) == true)
    {
        pAppRegisterInfo = (APP_REGISTER_INFO_T*)(obj.data);
        if(alive_app_name_read(pAppRegisterInfo->appName) >= 0)
        {
            strncpy((char*)(pMqtPlutin->comName), pAppRegisterInfo->appName, XDR_MAX_COM_NAME_LEN);
            pMqtPlutin->version = pAppRegisterInfo->version;
            pMqtPlutin->subr = pAppRegisterInfo->subscribeInfo;
            memcpy(&(pMqtPlutin->dataTime),&(pAppRegisterInfo->dataTime), sizeof(DATA_TIME_T));
            SMIOS_FMT_DEBUG( "app name[%s]", pMqtPlutin->comName);
            pMqtPlutin = pMqtPlutin + 1;
            (*count)++;
        }
        free(obj.name);
        free(obj.data);
    }
    pAppRegisterInfoTbl->unlock(pAppRegisterInfoTbl);
*/
    return 0;
}

/**
*********************************************************************
* @brief： 查询指定app注册信息
* @param[in]： appName    查询的App名称
               mqtPlugin  返回app注册信息
* @param[out]：none
* @return： 返回值说明 -1 查询失败
                      0 查询成功
*********************************************************************
*/
sint32 app_manager_one_app_query(MQT_PLUGIN_T *mqtPlugin, char* appName)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    //有效性检查
    if (NULL  == mqtPlugin)
    {
        SMIOS_FMT_DEBUG("Input param is invalid\n");
        return 0;
    }

    MQT_PLUGIN_T* pMqtPlutin = mqtPlugin;
    if(strcmp(appName,"M-smiOS")==0)
    {
        strcpy((char*)(pMqtPlutin->comName), "M-smiOS");
        strcpy(pMqtPlutin->version ,APP_VERSION);
        pMqtPlutin->subr.subscribeInfoNum = 0;
        app_get_maketime(APP_PUBLISH_TIME,(OOP_DATETIME_S_T*)&pMqtPlutin->dataTime);
        pMqtPlutin->appidlen = APPIDLen;
        memcpy(pMqtPlutin->appid, APPID,APPID_LEN);
        return 0;
    }
    
    //获取注册信息
    APP_REGISTER_INFO_T* pAppRegisterInfo = app_get_app_register_info(appName);
    if (NULL == pAppRegisterInfo)
    {
        SMIOS_FMT_DEBUG("The app[%s] is not in the register list\n", appName);
        return -1;
    }
    if(alive_app_name_read(appName) < 0)
    {
        SMIOS_FMT_DEBUG("%s: the [%s] app is not alive !\n",__FUNCTION__,appName);
        return -1;
    }
    strncpy((char*)(pMqtPlutin->comName), pAppRegisterInfo->appName, XDR_MAX_COM_NAME_LEN);
    strcpy(pMqtPlutin->version,pAppRegisterInfo->version);
    pMqtPlutin->subr    = pAppRegisterInfo->subscribeInfo;
    memcpy(&(pMqtPlutin->dataTime),&(pAppRegisterInfo->dataTime), sizeof(DATA_TIME_T));
    pMqtPlutin->appidlen  = pAppRegisterInfo->appidlen;
    memcpy(pMqtPlutin->appid,pAppRegisterInfo->appid,pMqtPlutin->appidlen);
    if (NULL != pAppRegisterInfo)
    {
        free(pAppRegisterInfo);
        pAppRegisterInfo = NULL;
    }

    return 0;
}

/**
*********************************************************************
* @brief： 查询指定app注册信息
* @param[in]： appName    查询的App名称
               mqtPlugin  返回app注册信息
* @param[out]：none
* @return： 返回值说明 -1 查询失败
                      0 查询成功
*********************************************************************
*/
sint32 app_manager_app_query(char* appName, MSG_CLIENT_T *client)
{
    SMIOS_FMT_DEBUG(" %s\n", __FUNCTION__);
    //有效性检查
    if (NULL == appName || NULL == client)
    {
        SMIOS_FMT_DEBUG("Invalid input param\n");
        return -1;
    }

    //获取app注册信息
    APP_REGISTER_INFO_T* pAppRegisterInfo = app_get_app_register_info(appName);
    if (NULL == pAppRegisterInfo)
    {
        SMIOS_FMT_DEBUG("not find the app[%s]\n", appName);
        return -1;
    }

    strncpy(client->clientName, pAppRegisterInfo->appName, MAX_APP_NAME_LEN);
    SMIOS_FMT_DEBUG( "app name[%s]\n", client->clientName);
    //获取app协议类型
    switch (pAppRegisterInfo->protocolInfo.protocolType)
    {
    case PROTOCOL_UDP:
        client->type = UDP;
        strncpy(client->client.uClient.ip, pAppRegisterInfo->protocolInfo.udp.ip, IP_LEN);
        client->client.uClient.port = pAppRegisterInfo->protocolInfo.udp.port;
        SMIOS_FMT_DEBUG("type[udp], ip[%s], port[%d]\n", client->client.uClient.ip, client->client.uClient.port);
        break;
    case PROTOCOL_TCP:
        client->type = TCP;
        strncpy(client->client.tClient.ip, pAppRegisterInfo->protocolInfo.tcp.ip, IP_LEN);
        client->client.tClient.port = pAppRegisterInfo->protocolInfo.tcp.port;
        SMIOS_FMT_DEBUG( "type[tcp], ip[%s], port[%d]\n", client->client.tClient.ip, client->client.tClient.port);
        break;
    case PROTOCOL_IPC:
        client->type = IPC;
        strncpy(client->client.iClient.unixSockertPath,pAppRegisterInfo->protocolInfo.ipc.unixSockertPath, UNIX_SOCKET_PATH_LEN);
        SMIOS_FMT_DEBUG( "type[IPC], socket path[%s]\n", client->client.iClient.unixSockertPath);
        break;
    case PROTOCOL_MQTT:
        client->type = MQTT;
        SMIOS_FMT_DEBUG( "type[MQTT]\n");
        break;
    default:
        break;
    }

    free(pAppRegisterInfo);
    return 0;
}

/**
*********************************************************************
* @brief： app事件订阅
* @param[in]： appName    App名称
               subInfo   订阅信息
               isSub      是否订阅
* @param[out]：none
* @return： 返回值说明 -1 订阅失败
                      0 订阅成功
*********************************************************************
*/
sint32 app_manager_sub_event(char *appName, APP_SUB_INFO_RAW_T *subInfo, sint32 isSub)
{
    SMIOS_FMT_TRACE(" %s\n", __FUNCTION__);
    uint32 i = 0, j = 0;
    if (NULL == appName || NULL == subInfo)
    {
        SMIOS_FMT_DEBUG("Invalid input param\n");
        return -1;
    }

    APP_REGISTER_INFO_T *pAppRegisterInfo = app_get_app_register_info(appName);
    if (NULL == pAppRegisterInfo)
    {
        SMIOS_FMT_DEBUG("not find the app[%s]\n", appName);
        return -1;
    }

    if(isSub)
    {
        pAppRegisterInfo->subscribeInfo = *subInfo;
    }
    else
    {
        for(i = 0; i < subInfo->subscribeInfoNum; i++)
        {
            for(j = 0; j < pAppRegisterInfo->subscribeInfo.subscribeInfoNum; j++)
            {
                SMIOS_FMT_TRACE( "subInfo->subscribeInfo[%d]= [0x%08x]\n", i,
                            subInfo->subscribeInfo[i]);
                SMIOS_FMT_TRACE(
                    "pAppRegisterInfo->subscribeInfo.subscribeInfo[%d]= "
                    "[0x%08x]\n",
                    j, pAppRegisterInfo->subscribeInfo.subscribeInfo[j]);
                if ((subInfo->subscribeInfo[i] & 0xffff0000) ==
                    (pAppRegisterInfo->subscribeInfo.subscribeInfo[j] & 0xffff0000))
                    {
                        pAppRegisterInfo->subscribeInfo.subscribeInfo[j] &=
                            (~(subInfo->subscribeInfo[i] & 0xffff));
                        SMIOS_FMT_DEBUG("%s取消订阅[%d]:[%08x]\n",appName, j,pAppRegisterInfo->subscribeInfo.subscribeInfo[j]);
                        break;
                    }
            }
        }
    }

    //添加app信息到注册列表
    app_register_list_add(pAppRegisterInfo);
    free(pAppRegisterInfo);
    return 0;
}

/**
*********************************************************************
* @brief： 获取app的订阅信息
* @param[in]： appName    App名称
* @param[out]：subInfo   订阅信息
* @return： 返回值说明 -1 获取失败
                      0 获取成功
*********************************************************************
*/
sint32 app_manager_get_sub_info(char *appName, APP_SUB_INFO_RAW_T *subInfo)
{
    SMIOS_FMT_TRACE(" %s\n", __FUNCTION__);
    if(NULL == appName || NULL == subInfo)
    {
        SMIOS_FMT_DEBUG("invalid input param\n");
        return -1;
    }

    //获取注册信息
    APP_REGISTER_INFO_T *pAppRegisterInfo = app_get_app_register_info(appName);
    if (NULL == pAppRegisterInfo)
    {
        SMIOS_FMT_DEBUG("not find the app[%s]\n", appName);
        return -1;
    }

    //获取订阅信息
    *subInfo = pAppRegisterInfo->subscribeInfo;
    if (NULL != pAppRegisterInfo)
    {
        free(pAppRegisterInfo);
        pAppRegisterInfo = NULL;
    }
    return 0;
}

/**
*********************************************************************
* @brief： 事件处理
* @param[in]： eventID    事件ID
               eventBuf   事件内容
               eventBufLen 事件内容长度
* @param[out]：none
* @return： 返回值说明 -1 处理失败
                      0 处理成功
*********************************************************************
*/
sint32 app_manager_event_handle(uint32 eventID, uint8 *eventBuf, uint32 eventBufLen)
{
    uint32 sendCount = app_event_handle(eventID, eventBuf, eventBufLen);

    return sendCount;
}

/**
*********************************************************************
* @brief： 心跳信息处理
* @param[in]： appName
               threadStatus
* @param[out]：none
* @return： 返回值说明 -1 处理失败
                      0 处理成功
*********************************************************************
*/
sint32 app_manager_heart_beat_process(char* appName, uint64 threadStatus)
{
    SMIOS_FMT_TRACE(" %s\n", __FUNCTION__);

    if (NULL == appName) {
        SMIOS_FMT_DEBUG("invalid app name\n");
        return -1;
    }

    //获取时间戳
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tv);
    SMIOS_FMT_TRACE( "app[%s], threadStatus[%llu]\n", appName, threadStatus);
    SMIOS_FMT_TRACE( "sec[%ld], nsec[%ld]\n", tv.tv_sec, tv.tv_nsec);

    APP_REGISTER_INFO_T* pAppRegisterInfo = app_get_app_register_info(appName);
    if (NULL == pAppRegisterInfo)
    {
        SMIOS_FMT_DEBUG("not find the app[%s]\n", appName);
        return -1;
    }

    alive_app_name_save(appName);
    
    pAppRegisterInfo->tv.tv_sec = tv.tv_sec;
    pAppRegisterInfo->tv.tv_nsec = tv.tv_nsec;
    pAppRegisterInfo->threadStatus = threadStatus;
    //更新注册列表
    app_register_list_update(pAppRegisterInfo);

    free(pAppRegisterInfo);

    return 0;
}
