/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控mqtt
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "task_id.h"
#include "task_msg.h"

#include "db_api.h"
#include "appmsg_split.h"
#include "mqtt_client.h"

#include "sec_comm.h"
#include "sec_mqtt.h"
#include "sec_msg.h"
#include "cJSON.h"


// 对外消息 客户端句柄
MQTT_CLIENT_T   *g_MqttClient;

pthread_mutex_t  g_MsgSendlock; 
pthread_mutex_t  g_MsgIndexlock; 
uint16           g_AppMsgindex = 0;

DB_AGENT         g_dbAgent = 0;

char      g_AppName[APPMSG_NAME_LEN] = {0};
char      g_AppVer[APPMSG_NAME_LEN] = {0};

/**********************************************************************
* @name      : sec_next_index_init
* @brief     ：对外消息index管理初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
void sec_next_index_init()
{
    pthread_mutex_init(&g_MsgIndexlock, NULL);
    g_AppMsgindex = 0;
}

/**********************************************************************
* @name      : sec_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
uint16 sec_get_next_index()
{
    uint16 curindex = 0;

    PTHREAD_LOCK_SAFE(&g_MsgIndexlock);
    curindex = g_AppMsgindex;
    g_AppMsgindex++;
    PTHREAD_UNLOCK_SAFE(&g_MsgIndexlock);

    return curindex;
}

/**********************************************************************
* @name      : sec_mqtt_init
* @brief     ：mqtt初始化
* @param[in] ：uint32 ip
               uint16 port
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int sec_mqtt_init(SEC_APPDATA_T *pData)
{
    int ret = 0;
    uint8 status = 0;
    char   mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[2];
    char  mqttstatus[2][16] = {"connectlost", "success"};
    uint32 ip = pData->mqttip;

    /* 新建客户端 */
    sprintf((char *)g_AppName, "M-%s", SEC_APP_NAME);
    sprintf((char *)g_AppVer, "%s", APP_VERSION);
    g_MqttClient = mqtt_client_create((char *)g_AppName);
    if (NULL == g_MqttClient)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_create fail\n");
        return -1;
    }
    
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, APP_SUBTOPIC1, strlen(APP_SUBTOPIC1));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, APP_SUBTOPIC2, strlen(APP_SUBTOPIC2));
    ret = mqtt_client_set_sub_topic(g_MqttClient, subtopic, 2);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(g_MqttClient, APP_DEFAULT_TOPIC, 0);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 链接服务端 */
    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    MQTTMSG_FMT_DEBUG("mqtt connect %s:%d\n", mqttaddr, pData->mqttport);
    ret = mqtt_client_connect(g_MqttClient, mqttaddr, pData->mqttport, NULL);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_connect fail, ret %d", ret);
        goto errorend;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(g_MqttClient);
        MQTTMSG_FMT_DEBUG("Get mqtt connect status %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    MQTTMSG_FMT_DEBUG("mqtt connect %s:%d success\n", mqttaddr, pData->mqttport);

//    /* 创建db_agent */
//    g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_SEC_AGENT, sec_send_dbmsg, NULL, SEC_DB_CLIENT_NUM, SEC_APP_NAME);
//    {
//        if(g_dbAgent < 0)
//        {
//            LCMONIT_FMT_DEBUG("db_agent_create failed!\n");
//            mqtt_client_disconnect(g_MqttClient);
//            ret = -1;
//            goto errorend;
//        }
//    }
//    pData->dbAgent = g_dbAgent;
//    pData->appName = (char*)g_AppName;

    sec_mqtt_debug();

    sec_next_index_init();
    pthread_mutex_init(&g_MsgSendlock, NULL);
    return 0;

errorend:
    mqtt_client_destroy(g_MqttClient);
    return ret;
}

#if DESC("公共发送接口", 1)

/**********************************************************************
* @name      : sec_send_axdrmsg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  对外消息
* @param[in] ：int qos                   消息级别               
* @param[out]：NA
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-08-15
* @Update    :
**********************************************************************/
int sec_send_axdrmsg(MESSAGE_INFO_T *pMsgInfo, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    char   topic[384] = {0};
    
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    msglen = appmsg_axdr_package(pMsgInfo, &pMsg);
    if(0 == msglen)
    {
        MQTTMSG_FMT_DEBUG("App send appmsg_package fail, IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        return -1;
    }

    if(msglen >= APP_RECV_BUF_LEN)
    {
        MQTTMSG_FMT_DEBUG("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

#ifdef PRODUCT_SCU
    sprintf(topic, "%s/%s/%s", pMsgInfo->souraddr, pMsgInfo->destaddr, MQTT_TOPIC_A_XDR);
#else
    if(strncmp(pMsgInfo->destaddr, NAME_BROADCAST, strlen(NAME_BROADCAST)) == 0)
    {
        sprintf(topic, "%s/%s/%04X/%04X", pMsgInfo->souraddr, pMsgInfo->destaddr, pMsgInfo->IID, pMsgInfo->IOP);
    }
    else
    {
        sprintf(topic, "%s/%s", pMsgInfo->souraddr, pMsgInfo->destaddr);
    }
#endif

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_MqttClient, topic, qos, pMsg, (int)msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);

    if(0 == ret)
    {
        MQTTMSG_BUF_DEBUG(pMsg, msglen, "TOPIC: %s, IID-%04X, IOP-%04X:\n", topic, pMsgInfo->IID, pMsgInfo->IOP);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("send axdr msg failed: topic %s, index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                          topic, pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : sec_send_jsonmsg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  对外消息
* @param[in] ：int qos                   消息级别               
* @param[out]：NA
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-08-15
* @Update    :
**********************************************************************/
int sec_send_jsonmsg(MESSAGE_JSON_T *pMsgInfo, int qos)
{
    int    ret = 0;
    char *pMsg = NULL;
    uint16 msglen = 0;
    char   topic[384] = {0};
    
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    pMsg = cJSON_Print(pMsgInfo->pObject);
    if(NULL == pMsg)
    {
        MQTTMSG_FMT_DEBUG("App send cJSON_Print fail\n");
        return -2;
    }
    
    msglen = strlen(pMsg);
    sprintf(topic, "%s/%s/%s/%s/%s/%s", pMsgInfo->sourapp, pMsgInfo->destapp, MQTT_TOPIC_JSON,
                                        pMsgInfo->operat, pMsgInfo->infotype, pMsgInfo->infopath);
    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_MqttClient, topic, qos, pMsg, (int)msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);

    if(0 == ret)
    {
        MQTTMSG_FMT_DEBUG("TOPIC-%s:\n%s\n", topic, pMsg);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("send json msg failed, topic %s\n%s\n", topic, pMsg);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : sec_send_mqttmsg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题
               int qos                  消息级别
               void *msgdata            消息内容
               uint32 msglen            消息长度
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_send_mqttmsg(char *pTopic, int qos, void *msgdata, int msglen)
{
    int    ret = 0;
    
    if(NULL == pTopic)
    {
        return -1;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_MqttClient, pTopic, qos, msgdata, msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);
    return ret;
}

/**********************************************************************
* @name      : sec_send_msg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_MQTT_T *pMsgMqtt  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int sec_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos)
{
    int    ret = 0;
    char *pMsg = NULL;
    uint16 msglen = 0;
    char   topic[384] = {0};
    
    if(NULL == pMsgMqtt)
    {
        return -1;
    }

    if(MESSAGE_TYPE_A_XDR == pMsgMqtt->msgtype)
    {
        msglen = appmsg_axdr_package(&(pMsgMqtt->tAXDR), (uint8**)&pMsg);
        if(NULL == pMsg)
        {
            MQTTMSG_FMT_DEBUG("App send appmsg_axdr_package fail\n");
            return -2;
        }
#ifdef PRODUCT_SCU
        sprintf(topic, "%s/%s/%s", pMsgMqtt->tAXDR.souraddr, pMsgMqtt->tAXDR.destaddr, MQTT_TOPIC_A_XDR);
#else
        sprintf(topic, "%s/%s", pMsgMqtt->tAXDR.souraddr, pMsgMqtt->tAXDR.destaddr);
#endif
    }
    else if(pMsgMqtt->tJson.pObject)
    {
        pMsg = cJSON_Print(pMsgMqtt->tJson.pObject);
        if(NULL == pMsg)
        {
            MQTTMSG_FMT_DEBUG("App send cJSON_Print fail\n");
            return -2;
        }
        msglen = strlen(pMsg);
        sprintf(topic, "%s/%s/%s/%s/%s/%s", pMsgMqtt->tJson.sourapp, pMsgMqtt->tJson.destapp, MQTT_TOPIC_JSON,
                                            pMsgMqtt->tJson.operat, pMsgMqtt->tJson.infotype, pMsgMqtt->tJson.infopath);
    }
    else
    {
        BLE_FMT_DEBUG("Err input msg\n");
        return -3;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_MqttClient, topic, qos, pMsg, (int)msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);

    if(0 == ret)
    {
        if(MESSAGE_TYPE_A_XDR == pMsgMqtt->msgtype)
        {
            MQTTMSG_BUF_DEBUG(pMsg, msglen, "Send topic %s\n", topic);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("Send topic %s\n%s\n", topic, pMsg);
        }
    }
    else
    {
        if(MESSAGE_TYPE_A_XDR == pMsgMqtt->msgtype)
        {
            MQTTMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: %04x, iop: %04x, ret %d\n", 
                          pMsgMqtt->tAXDR.index, pMsgMqtt->tAXDR.label, pMsgMqtt->tAXDR.IID, pMsgMqtt->tAXDR.IOP, ret);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("Mqtt send failed, topic %s\n%s\n", topic, pMsg);
        }
    }

    free(pMsg);
    return ret;
}


/**********************************************************************
* @name      : sec_axdrmsg_respond
* @brief     ：响应的axdr消息
* @param[in] ：uint16 taskid       客户端id
               uint16 index        序号
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-12
* @Update    : 
**********************************************************************/
int sec_axdrmsg_respond(MESSAGE_INFO_T *pInMsg, uint8 *payload, uint32 msglen)
{
    int ret = 0;
    uint32 size = sizeof(MESSAGE_INFO_T) + msglen;
    MESSAGE_INFO_T *pOutMsg = (MESSAGE_INFO_T*)malloc(size);

    if(pOutMsg == NULL)
    {
        MQTTMSG_FMT_DEBUG("scsMonitor_axdrmsg_respond malloc failed, size %d.\n", size);
        return ERR_PNULL;
    }

    memset(pOutMsg, 0, size);
    pOutMsg->rpm = 0;
    pOutMsg->index = pInMsg->index;
    pOutMsg->priority = pInMsg->priority;
    pOutMsg->label = pInMsg->label;
    pOutMsg->IID = pInMsg->IID;
    pOutMsg->IOP = pInMsg->IOP;
    
    pOutMsg->sourlen = pInMsg->destlen;
    memcpy(pOutMsg->souraddr , pInMsg->destaddr, pOutMsg->sourlen);
    pOutMsg->destlen = pInMsg->sourlen;
    memcpy(pOutMsg->destaddr, pInMsg->souraddr, pOutMsg->destlen);

    pOutMsg->msglen = msglen;
    memcpy(&pOutMsg->playload, payload, msglen);
    
    ret = sec_send_axdrmsg(pOutMsg, 0);
    free(pOutMsg);

    return ret;
}

#endif

#if DESC("AXDR消息处理", 1)
/**********************************************************************
* @name      : sec_login_to_smios
* @brief     ：APP注册
* @param[in] ：uint8 *pAppId   APPID
               uint8 appidlen  长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-10
* @Update    : 
**********************************************************************/
int sec_login_to_smios(uint8 *pAppId, uint8 appidlen)
{
    uint8           buf[420] = {0};
    uint16          msglen = 0;
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T   *ptAXDR = &(ptMsgNew->tAXDR);
    uint8              *temp = ptAXDR->playload;
    int                  len = 0;
    pid_t                pid = getpid();
    Date_Time_S        tTime;
    char              *smios = "M-smiOS";

    app_get_maketime(APP_PUBLISH_TIME, &tTime);

    /* 进程号 */
    appmsg_memcpy_r(temp, &pid, sizeof(pid_t));
    msglen += 4;

    /* APP名称 */
    len = strlen(g_AppName);
    temp[msglen++] = len;
    memcpy(temp + msglen, g_AppName, len);  
    msglen += len;

    /* 订阅事件 */
    temp[msglen++] = 0;

    /* 版本信息 */
    len = strlen(g_AppVer);
    temp[msglen++] = len;
    memcpy(temp + msglen, g_AppVer, len);  
    msglen += len;

    /* 发布日期 */
    temp[msglen++] = tTime.year_h;
    temp[msglen++] = tTime.year_l;
    temp[msglen++] = tTime.month;
    temp[msglen++] = tTime.day;
    temp[msglen++] = tTime.hour;
    temp[msglen++] = tTime.min;
    temp[msglen++] = tTime.sec;

    /* APPID */
    temp[msglen++] = appidlen;
    if(appidlen > 0)
    {
        memcpy(temp + msglen, pAppId, appidlen);
    }
    msglen += appidlen;

    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 1;
    ptAXDR->index = sec_get_next_index();
    ptAXDR->priority = 0;
    ptAXDR->label = 0;
    ptAXDR->IID = MESSAGE_IID_SYSTEM;
    ptAXDR->IOP = SYSTEM_IOP_APP_REGISTER;
    
    ptAXDR->sourlen = strlen(g_AppName);
    memcpy(ptAXDR->souraddr , g_AppName, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(smios);
    memcpy(ptAXDR->destaddr, smios, ptAXDR->destlen);

    ptAXDR->msglen = msglen;
    
    return sec_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : sec_send_errmsg
* @brief     ：对外消息发送错误的消息
* @param[in] ：MESSAGE_INFO_T *pRecvMsg  收到的消息
               int errCode               错误码
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int sec_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode)
{
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T *ptAXDR = &(ptMsgNew->tAXDR);
    uint8           *temp = ptAXDR->playload;

    if(NULL == pRecvMsg)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 1;
    ptAXDR->priority = pRecvMsg->priority;
    ptAXDR->index = pRecvMsg->index;
    ptAXDR->label = pRecvMsg->label;

    ptAXDR->sourlen = strlen(g_AppName);
    memcpy(ptAXDR->souraddr, g_AppName, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pRecvMsg->souraddr, pRecvMsg->sourlen);
    ptAXDR->destlen = pRecvMsg->sourlen;

    ptAXDR->IID = MESSAGE_IID_COMMON;
    ptAXDR->IOP = COMMON_IOP_ERRMSG;

    appmsg_memcpy_r(temp, &(pRecvMsg->IID), 2);
    msglen += 2;
    appmsg_memcpy_r(temp + msglen, &(pRecvMsg->IOP), 2);
    msglen += 2;
    appmsg_memcpy_r(temp + msglen, &errCode, 2);
    msglen += 2;

    ptAXDR->msglen = msglen;
    
    return sec_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : sec_heart_bear
* @brief     ：心跳应答
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int sec_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8           buf[360] = {0};
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T *ptAXDR = &(ptMsgNew->tAXDR);
    uint32 status = 1;
    char   *smios = "M-smiOS";

    if(NULL == pInMsg)
    {
        return -1;
    }
    
    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 0;
    ptAXDR->index = pInMsg->index;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->label = pInMsg->label;
    ptAXDR->IID = MESSAGE_IID_COMMON;
    ptAXDR->IOP = pInMsg->IOP;
    
    ptAXDR->sourlen = strlen(g_AppName);
    memcpy(ptAXDR->souraddr, g_AppName, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(smios);
    memcpy(ptAXDR->destaddr, smios, ptAXDR->destlen);

    ptAXDR->msglen = sizeof(uint32);
    appmsg_memcpy_r(ptAXDR->playload, &status, sizeof(uint32));
    
    return sec_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : sec_get_appidmsg
* @brief     ：获取appid
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          全局数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-5-2
* @Update    :
**********************************************************************/
int sec_get_appidmsg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    SEC_APPDATA_T   *ptAppData = (SEC_APPDATA_T*)pPublic;
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T *ptAXDR = &(ptMsgNew->tAXDR);
    uint8             *temp = ptAXDR->playload;

    if(NULL == pInMsg || NULL == pArg)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(g_AppName);
    memcpy(ptAXDR->souraddr, g_AppName, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    /* APPID */
    temp[msglen++] = ptAppData->appidlen;
    if(ptAppData->appidlen > 0)
    {
        memcpy(temp + msglen, ptAppData->appid, ptAppData->appidlen);
        msglen += ptAppData->appidlen;
    }

    ptAXDR->msglen = msglen;
    
    return sec_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : sec_get_versionmsg
* @brief     ：获取版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int sec_get_version(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T   *ptAXDR = &(ptMsgNew->tAXDR);
    uint8              *temp = ptAXDR->playload;
    Date_Time_S       tTime;
    uint8 len = 0;

    if(NULL == pInMsg || NULL == pArg)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(g_AppName);
    memcpy(ptAXDR->souraddr, g_AppName, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    /* 版本信息 */
    len = strlen(g_AppVer);
    temp[msglen++] = len;
    memcpy(temp + msglen, &g_AppVer, len);
    msglen += len;

    /* 发布日期 */
    app_get_maketime(APP_PUBLISH_TIME, &tTime);
    temp[msglen++] = tTime.year_h;
    temp[msglen++] = tTime.year_l;
    temp[msglen++] = tTime.month;
    temp[msglen++] = tTime.day;
    temp[msglen++] = tTime.hour;
    temp[msglen++] = tTime.min;
    temp[msglen++] = tTime.sec;
    ptAXDR->msglen = msglen;
    
    return sec_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : sec_seek_app
* @brief     ：发送APP查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-10
* @Update    :
**********************************************************************/
int sec_seek_app()
{
    MESSAGE_INFO_T  tMsgInfo;
    int             ret = 0;
    char           *smios = "M-smiOS";
    
    memset(&tMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    tMsgInfo.rpm = 1;
    tMsgInfo.priority = 2;

    tMsgInfo.index = sec_get_next_index();
    tMsgInfo.label = 0;
    
    tMsgInfo.sourlen = strlen((char *)g_AppName);
    memcpy(tMsgInfo.souraddr , g_AppName, tMsgInfo.sourlen);
    tMsgInfo.destlen = strlen(smios);
    memcpy(tMsgInfo.destaddr, smios, tMsgInfo.destlen);

    tMsgInfo.IID = MESSAGE_IID_SYSTEM;
    tMsgInfo.IOP = SYSTEM_IOP_GET_REGISTERAPP;
    tMsgInfo.msglen = 0;

    ret = sec_send_axdrmsg(&tMsgInfo, 0);
    return ret;    
}

/**********************************************************************
* @name      : sec_recv_regapp
* @brief     ：收到注册APP查询
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_recv_regapp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8  appnun = 0;
    uint8  bytes = 0;
    uint16 offset = 0;
    uint8 *pTemp = NULL;
    uint8  i = 0;
    uint8  namelen = 0;
    uint8  appidlen = 0;
    uint8  ucname[APPMSG_NAME_LEN] = {0};
    APP_INFO_T tApp;
    uint8  subnum = 0;
    SEC_MSGDATA_T *ptMsgData = (SEC_MSGDATA_T*)pArg;
    
    if(NULL == pInMsg)
    {
        return -1;
    }

    pTemp = pInMsg->playload;
    appnun = appmsg_get_len_offset(pTemp, &bytes);
    pTemp += bytes;

    for(i = 0; i < appnun; i++)
    {
        namelen = pTemp[offset++];
        
        memset(ucname, 0, APPMSG_NAME_LEN);
        memcpy(ucname, pTemp + offset, namelen);
        memset(&tApp, 0, sizeof(APP_INFO_T));
        if(0 == appname_split(ucname, &tApp))
        {

        }

        offset += namelen;
        subnum = pTemp[offset++];
        offset += (subnum + 1) * sizeof(uint32);  //订阅事件+版本信息
        offset += sizeof(Date_Time_S);

        /* APPID偏移 */
        appidlen = pTemp[offset++];
        offset += appidlen;

        pTemp += offset;
        offset = 0;
    }

    ptMsgData->appStep = SEC_STATUS_WORK;
    
    return 0;
}

/**********************************************************************
* @name      : sec_notify_send
* @brief     ：将事件发送给destApp
* @param[in] ：buffer       数据内容
               length      数据长度
* @param[out]：
* @Create    : 贺宁
* @Date      ：2021-7-12
* @Update    : 
**********************************************************************/
int sec_notify_send(uint16 index, uint16 IOP, char *destApp, uint8 *buffer, uint16 length)
{
    int ret = 0;
    MESSAGE_INFO_T *ptAXDR = NULL;
    uint16 msglen = sizeof(MESSAGE_INFO_T) + length;

    ptAXDR = (MESSAGE_INFO_T *)malloc(msglen);
    if(NULL == ptAXDR)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", msglen);
        return -1;
    }
    
    memset(ptAXDR, 0, msglen);
    
    ptAXDR->rpm = 1;
    ptAXDR->priority = 2;
    ptAXDR->index = index;
    ptAXDR->label = 0;
    
    ptAXDR->IID = MESSAGE_IID_SCS;
    ptAXDR->IOP = IOP;
    
    ptAXDR->sourlen = strlen(g_AppName);
    memcpy(ptAXDR->souraddr, g_AppName, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(destApp);
    memcpy(ptAXDR->destaddr, destApp, ptAXDR->destlen);

    ptAXDR->msglen = length;
    memcpy(ptAXDR->playload, buffer, length);

    ret = sec_send_axdrmsg(ptAXDR, 0);

    free(ptAXDR);

    return ret;
}

#endif

#if DESC("安全在线监测消息", 1)

/*
*********************************************************************
* @brief     : sec_axdr_danger_cmd_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_dangercmd_send(SEC_DANGERCMD_T *dangercmd)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    
    payload[offset++] = dangercmd->cmd;
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_DANGER_OPREATE, NAME_BROADCAST, payload, offset);
    
    return ret;
}

/*
*********************************************************************
* @brief     : sec_pwdchanged_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_pathchanged_send(SEC_PATHCHG_T *pathchg)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    int i = 0;
    uint8 len = 0;
   
    payload[offset++] = pathchg->nNum;

    for(i = 0; i < pathchg->nNum; i++)
    {
        len = strlen(pathchg->path[i]);

        payload[offset++] = len;
        memcpy(&payload[offset], pathchg->path[i], len);
        offset += len;
    }
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_KEY_PATH_CHGE, NAME_BROADCAST, payload, offset);
    
    return ret;
}

/*
*********************************************************************
* @brief     : sec_pwdchanged_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_pwdchanged_send(SEC_PWDCHG_T *pwdchg)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    uint8 len = strlen(pwdchg->usrname);
    
    payload[offset++] = len;
    memcpy(&payload[offset], pwdchg->usrname, len);
    offset += len;
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_PASSWARD_CHANGED, NAME_BROADCAST, payload, offset);
    
    return ret;
}

/*
*********************************************************************
* @brief     : sec_ethportopen_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_ethportopen_send(SEC_PORTOPEN_T *portopen)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    int i = 0;
    
    payload[offset++] = portopen->nNum;

    for(i = 0; i < portopen->nNum; i++)
    {
        memcpy_r(&payload[offset], &portopen->port[i], sizeof(uint16));
        offset += sizeof(uint16);
    }
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_ETH_PORT_OPEN, NAME_BROADCAST, payload, offset);
    
    return ret;
}

/*
*********************************************************************
* @brief     : sec_ethconnect_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_ethconnect_send(SEC_CONNECT_T *ethconn)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    int i = 0;
    uint32 ipaddr = 0;
    
    payload[offset++] = ethconn->nNum;

    for(i = 0; i < ethconn->nNum; i++)
    {
        payload[offset++] = 4;
        
        ipaddr = inet_addr(ethconn->connect[i].ip);
        memcpy(&payload[offset], &ipaddr, sizeof(uint32));
        offset += sizeof(uint32);

        memcpy_r(&payload[offset], &ethconn->connect[i].port, sizeof(uint16));
        offset += sizeof(uint16);        
    }
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_ETH_PORT_CONN, NAME_BROADCAST, payload, offset);
    
    return ret;
}

/*
*********************************************************************
* @brief     : sec_ethlogin_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_ethlogin_send(SEC_ETHLOGIN_T *login)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    
    payload[offset++] = login->status;
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_ETH_LOGIN_CONSOLE, NAME_BROADCAST, payload, offset);
    
    return ret;
}

/*
*********************************************************************
* @brief     : sec_ethlogin_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_seriallogin_send(SEC_SERIALSLOGIN_T *login)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    
    payload[offset++] = login->status;
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_UART_PORT_LOGIN, NAME_BROADCAST, payload, offset);
    
    return ret;
}

/*
*********************************************************************
* @brief     : sec_sshabnormal_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_sshabnormal_send(SEC_SSHABNORMAL_T *abnormal)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    
    payload[offset++] = abnormal->status;
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_LOCAL_MAINTAIN, NAME_BROADCAST, payload, offset);
    
    return ret;
}

/*
*********************************************************************
* @brief     : sec_loginabnormal_send
* @param[in] : cmd     接收消息
* @return    : 返回值
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int sec_loginabnormal_send(SEC_LOGINABNORMAL_T *abnormal)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    uint8 len = 0;
    
    payload[offset++] = abnormal->type;
    
    len = strlen(abnormal->usrname);
    payload[offset++] = len;
    memcpy(&payload[offset], abnormal->usrname, len);
    offset += len;

    len = strlen(abnormal->loginip);
    payload[offset++] = len;
    memcpy(&payload[offset], abnormal->loginip, len);
    offset += len;
    
    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_LOGIN_ABNORMAL, NAME_BROADCAST, payload, offset);
    
    return ret;
}

int sec_flowabnormal_send(SEC_SESSIONFLOWS_T *abnormal)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint32 ipaddr = 0;
    uint16 index = 0;
    int i = 0;
    uint8 len = 0;

    payload[offset++] = abnormal->nNum;

    for(i = 0; i < abnormal->nNum; i++)
    {
        payload[offset++] = 4;

        ipaddr = inet_addr(abnormal->flow[i].remoteIP);
        memcpy(&payload[offset], &ipaddr, sizeof(uint32));
        offset += sizeof(uint32);

        memcpy_r(&payload[offset], &abnormal->flow[i].remotePort, sizeof(uint16));
        offset += sizeof(uint16);        

        payload[offset++] = 4;

        ipaddr = inet_addr(abnormal->flow[i].localIP);
        memcpy(&payload[offset], &ipaddr, sizeof(uint32));
        offset += sizeof(uint32);

        memcpy_r(&payload[offset], &abnormal->flow[i].localPort, sizeof(uint16));
        offset += sizeof(uint16);

        len = strlen(abnormal->flow[i].protocol);
        payload[offset++] = len;
        memcpy(&payload[offset], abnormal->flow[i].protocol, len);
        offset += len;

        len = strlen(abnormal->flow[i].process);
        payload[offset++] = len;
        memcpy(&payload[offset], abnormal->flow[i].process, len);
        offset += len;

        payload[offset++] = abnormal->flow[i].procType;
    }

    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_SESSION_ABNORMAL, NAME_BROADCAST, payload, offset);
    
    return ret;
}

int sec_bandabnormal_send(SEC_SESSIONBANDWIDTHS_T *abnormal)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint32 ipaddr = 0;
    uint16 index = 0;
    int i = 0;
    uint8 len = 0;

    payload[offset++] = abnormal->nNum;

    for(i = 0; i < abnormal->nNum; i++)
    {
        payload[offset++] = 4;

        ipaddr = inet_addr(abnormal->flow[i].remoteIP);
        memcpy(&payload[offset], &ipaddr, sizeof(uint32));
        offset += sizeof(uint32);

        memcpy_r(&payload[offset], &abnormal->flow[i].remotePort, sizeof(uint16));
        offset += sizeof(uint16);        

        payload[offset++] = 4;

        ipaddr = inet_addr(abnormal->flow[i].localIP);
        memcpy(&payload[offset], &ipaddr, sizeof(uint32));
        offset += sizeof(uint32);

        memcpy_r(&payload[offset], &abnormal->flow[i].localPort, sizeof(uint16));
        offset += sizeof(uint16);

        len = strlen(abnormal->flow[i].protocol);
        payload[offset++] = len;
        memcpy(&payload[offset], abnormal->flow[i].protocol, len);
        offset += len;

        len = strlen(abnormal->flow[i].process);
        payload[offset++] = len;
        memcpy(&payload[offset], abnormal->flow[i].process, len);
        offset += len;

        memcpy_r(&payload[offset], &abnormal->flow[i].flowLimit, sizeof(uint32));
        offset += sizeof(uint32);

        memcpy_r(&payload[offset], &abnormal->flow[i].curFlow, sizeof(uint32));
        offset += sizeof(uint32);
    }

    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_BANDWIDTH_ABNORMAL, NAME_BROADCAST, payload, offset);
    
    return ret;
}

int sec_procabnormal_send(SEC_PROCABNORMALS_T *abnormal)
{
    int ret = 0;
    uint8 payload[256] = {0};
    uint16 offset = 0;
    uint16 index = 0;
    int i = 0;
    uint8 len = 0;

    payload[offset++] = abnormal->nNum;

    for(i = 0; i < abnormal->nNum; i++)
    {
        //异常类型
        payload[offset++] = abnormal->info[i].type;

        //启动时间
        memcpy_r(&payload[offset], &abnormal->info[i].stime.year, sizeof(uint16));
        offset += sizeof(uint16);

        payload[offset++] = abnormal->info[i].stime.month;
        payload[offset++] = abnormal->info[i].stime.day;
        payload[offset++] = abnormal->info[i].stime.hour;
        payload[offset++] = abnormal->info[i].stime.minute;
        payload[offset++] = abnormal->info[i].stime.second;

        //进程名
        len = strlen(abnormal->info[i].name);
        payload[offset++] = len;
        memcpy(&payload[offset], abnormal->info[i].name, len);
        offset += len;

        //进程user
        len = strlen(abnormal->info[i].user);
        payload[offset++] = len;
        memcpy(&payload[offset], abnormal->info[i].user, len);
        offset += len;

        //进程group
        len = strlen(abnormal->info[i].group);
        payload[offset++] = len;
        memcpy(&payload[offset], abnormal->info[i].group, len);
        offset += len;

        //进程cmd
        len = strlen(abnormal->info[i].cmdline);
        payload[offset++] = len;
        memcpy(&payload[offset], abnormal->info[i].cmdline, len);
        offset += len;
    }

    index = sec_get_next_index();

    ret = sec_notify_send(index, SCS_IOP_PROCESS_ABNORMAL, NAME_BROADCAST, payload, offset);
    
    return ret;
}

#endif

/**********************************************************************
* @name      : sec_get_selfversion
* @brief     ：获取内部版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
int sec_get_selfversion(int sockid, MESSAGE_INFO_T *pInMsg)
{
    uint8           buf[480] = {0};
    uint16            msglen = 0;
    uint8              *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;
    Date_Time_S tTime;

    if(NULL == pInMsg)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;
    ptMsgNew->index = pInMsg->index;
    ptMsgNew->label = pInMsg->label;

    ptMsgNew->sourlen = strlen((char*)g_AppName);
    memcpy(ptMsgNew->souraddr, g_AppName, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    temp = ptMsgNew->playload;

    /* APP名称 */
    temp[msglen++] = strlen(SEC_APP_NAME);
    memcpy(temp + msglen, SEC_APP_NAME, strlen(SEC_APP_NAME));
    msglen += strlen(SEC_APP_NAME);

    /* 版本号 */
    temp[msglen++] = strlen("XL01");
    memcpy(temp + msglen, "XL01", strlen("XL01"));
    msglen += strlen("XL01");

    /* 厂商代号 */
    temp[msglen++] = strlen("XLDZ");
    memcpy(temp + msglen, "XLDZ", strlen("XLDZ"));
    msglen += strlen("XLDZ");

    /* 发布日期 */
    app_get_maketime(APP_PUBLISH_TIME, &tTime);
    temp[msglen++] = strlen("XL-2020-05-30");
    msglen += sprintf((char *)temp + msglen, "XL-%04d-%02d-%02d", 
                      tTime.year_h * 256 + tTime.year_l, tTime.month, tTime.day); 

    ptMsgNew->msglen = msglen;
    
    return sec_send_axdrmsg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : sec_recv_mqttmsg
* @brief     ：对外消息接收, 使用了mqtt_client_recv_msg 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_recv_mqttmsg(char *topic, uint16 topicLen, uint8 *pBuf, uint16 buflen)
{
    int qos;
    return mqtt_client_recv_msg(g_MqttClient, topic, topicLen, &qos, pBuf, (int)buflen, 1000);
}

/**********************************************************************
* @name      : sec_mqtt_debug
* @brief     ：mqtt调试信息打印
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-3
* @Update    :
**********************************************************************/
void sec_mqtt_debug()
{
    char   buflog[1024] = {0};

    if(0 != mqtt_msg_handle_debug(g_MqttClient, buflog, 1024))
    {
        MQTTMSG_FMT_DEBUG("Mqtt client debug info\n%s\n", buflog);
        printf("Mqtt client debug info\n%s\n", buflog);
    }
}

#if DESC("JSON消息",1)

/**********************************************************************
* @name      : sec_login_to_sys
* @brief     ：APP注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int sec_login_to_sys()
{
    int ret = 0;
    char topic[256] = {0};
    char msg[256] = {0};
    uint32 msglen = 0;
    uint32 token = 0;
    SYS_APPREG_T tAppReg;
    Date_Time_S tTime;

    memset(&tAppReg, 0, sizeof(SYS_APPREG_T));
    
    tAppReg.name.num = strlen(SEC_APP_NAME);
    memcpy(tAppReg.name.value, SEC_APP_NAME, tAppReg.name.num);
    tAppReg.version = (uint32)APP_VERSION;

    app_get_maketime(APP_PUBLISH_TIME, &tTime);
    tAppReg.releaseDate.year = tTime.year_h;
    tAppReg.releaseDate.year = (tAppReg.releaseDate.year << 8) + tTime.year_l;
    tAppReg.releaseDate.month = tTime.month;
    tAppReg.releaseDate.day = tTime.day;
    tAppReg.releaseDate.hour = tTime.hour;
    tAppReg.releaseDate.min = tTime.min;
    tAppReg.releaseDate.sec = tTime.sec;

    token = sec_get_next_index();
    msglen = sys_appreg_package(token, &tAppReg, msg, 256);
    if(0 == msglen)
    {
        return 1;
    }
    sprintf(topic, "%s/%s/%s/%s/%s", g_AppName, SYS_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_INFO_REQ, SYS_TOPIC_REGISTER);

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_MqttClient, topic, 0, msg, msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);
    if(0 == ret)
    {
        MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msg);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send msg failed ret %d, tpoic %s, msg:\n%s\n", ret, topic, msg);
    }

    return ret;
}

/**********************************************************************
* @name      : sec_heart_to_sys
* @brief     ：心跳
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int sec_heart_to_sys(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;
    uint32 token = 0;
    char   topic[256] = {0};
    char   msg[256] = {0};
    uint32 msglen = 0;

    ret = sys_keepAlive_split(pInMsg->infopath, pInMsg->pObject, &token);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("keepAlive split error %d\n", ret);
        return 1;
    }

    msglen = sys_keepAlive_package(token, msg, 256);
    if(0 == msglen)
    {
        return 2;
    }
    sprintf(topic, "%s/%s/%s/%s/%s", g_AppName, SYS_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_INFO_RES, SYS_TOPIC_KEEPALIVE);

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_MqttClient, topic, 0, msg, msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);
    if(0 == ret)
    {
        MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msg);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send msg failed ret %d, tpoic %s, msg:\n%s\n", ret, topic, msg);
    }

    return ret;
}

/**********************************************************************
* @name      : sec_get_appreglist
* @brief     ：APP注册列表查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int sec_get_appreglist()
{
    int ret = 0;
    char   topic[256] = {0};
    char   msg[256] = {0};
    uint32 msglen = 0;
    uint32 token = 0;

    /* 进程号 */
    token = sec_get_next_index();
    msglen = sys_common_package(token, msg, 256);
    if(0 == msglen)
    {
        return 1;
    }
    sprintf(topic, "%s/%s/%s/%s/%s", g_AppName, SYS_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_INFO_REQ, SYS_TOPIC_GETREGLIST);

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_MqttClient, topic, 0, msg, msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);
    if(0 == ret)
    {
        MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msg);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send msg failed ret %d, tpoic %s, msg:\n%s\n", ret, topic, msg);
    }

    return ret;
}

/**********************************************************************
* @name      : sec_appreglist_check
* @brief     ：app注册列表解析
* @param[in] ：MESSAGE_JSON_T *pInMsg    应答消息
* @param[out]：
* @return    ：FALSE-继续/TRUE-完成
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
uint8 sec_appreglist_check(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;
    uint32 token = 0;
    SYS_APPLIST_T tAppList;
    uint32 i = 0;
    
    ret = sys_getreglist_split(pInMsg->infopath, pInMsg->pObject, &tAppList, &token);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("getreglist split error %d\n", ret);
        return FALSE;
    }

    for(i = 0; i < tAppList.num; i++)
    {
  
    }
    
    return ret;
}

#endif

