/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP MQTT
* @date：    2021-7-8
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "cJSON.h"
#include "oopType.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "framecomm.h"

#include "radio_data.h"
#include "radio_mqtt.h"


// 对外消息 客户端句柄
MQTT_CLIENT_T *g_RADIOMsgClient;

#define RADIO_SUB_TOPIC      "+/M-radioManager/#"
#define RADIO_SUB1_TOPIC     "+/radioManager/#"

pthread_mutex_t   g_RadioSendlock; 
pthread_mutex_t   g_RadioIndexlock; 
uint16            g_RadioMsgindex = 0;

char      g_AppName[APPMSG_NAME_LEN] = {0};
char      g_AppVer[APPMSG_NAME_LEN] = {0};

/**********************************************************************
* @name      : radio_mqtt_init
* @brief     ：对外消息初始化MQTT
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int radio_mqtt_init(uint32 ip, uint16 port)
{
    int   ret = 0;
    uint8 status = 0;
    char  mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[3];
    char  mqttstatus[2][16] = {"connectlost", "success"};

    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);

    /* 新建客户端 */
    sprintf((char *)g_AppName, "%s", RADIO_APP_NAME);
    sprintf((char *)g_AppVer, "%s", APP_VERSION);
    g_RADIOMsgClient = mqtt_client_create(RADIO_APP_NAME);
    if (NULL == g_RADIOMsgClient)
    {
        RADIO_FMT_DEBUG("mqtt_client_create fail\n");
        return -1;
    }
    
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, RADIO_SUB_TOPIC, strlen(RADIO_SUB_TOPIC));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, RADIO_SUB1_TOPIC, strlen(RADIO_SUB1_TOPIC));
	subtopic[2].qos = 0;
    memcpy(subtopic[2].topic, "M-smiOS/Broadcast/0000/#", strlen("M-smiOS/Broadcast/0000/#"));

    ret = mqtt_client_set_sub_topic(g_RADIOMsgClient, subtopic, 3);
    if(0 != ret)
    {
        RADIO_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        mqtt_client_destroy(g_RADIOMsgClient);
        return ret;
    }

    /* 链接服务端 */
    RADIO_FMT_DEBUG("链接mqtt服务端 %s:%d\n", mqttaddr, port);
    ret = mqtt_client_connect(g_RADIOMsgClient, mqttaddr, port, NULL);
    if(0 != ret)
    {
        RADIO_FMT_DEBUG("链接mqtt服务端链接失败 ret %d\n", ret);
        mqtt_client_destroy(g_RADIOMsgClient);
        return ret;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(g_RADIOMsgClient);
        RADIO_FMT_DEBUG("mqtt服务端链接状态: %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    RADIO_FMT_DEBUG("链接mqtt服务端成功\n");

    pthread_mutex_init(&g_RadioSendlock, NULL);
    pthread_mutex_init(&g_RadioIndexlock, NULL);
    return 0;
}

/**********************************************************************
* @name      : radio_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
uint16 radio_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE((void *)&g_RadioIndexlock);
    curindex = g_RadioMsgindex;
    g_RadioMsgindex++;
    PTHREAD_UNLOCK_SAFE((void *)&g_RadioIndexlock);

    return curindex;
}

/**********************************************************************
* @name      : radio_send_msg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_INFO_T *pMsgAXDR  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_send_axdr_msg(MESSAGE_INFO_T *pMsgAXDR, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    char   topic[512] = {0};
    
    if(NULL == pMsgAXDR)
    {
        return -1;
    }

    msglen = appmsg_axdr_package(pMsgAXDR, &pMsg);
    if(NULL == pMsg)
    {
        MQTTMSG_FMT_DEBUG("App send appmsg_axdr_package fail\n");
        return -2;
    }
    #if PRODUCT_SCU
    sprintf(topic, "%s/%s/%s", pMsgAXDR->souraddr, pMsgAXDR->destaddr, MQTT_TOPIC_A_XDR);
    #else
    sprintf(topic, "%s/%s", pMsgAXDR->souraddr, pMsgAXDR->destaddr);
    #endif

    PTHREAD_LOCK_SAFE(&g_RadioSendlock);
    ret = mqtt_client_send_msg(g_RADIOMsgClient, topic, qos, pMsg, (int)msglen);
    PTHREAD_UNLOCK_SAFE(&g_RadioSendlock);

    if(0 == ret)
    {
        MQTTMSG_BUF_DEBUG(pMsg, msglen, "Send topic %s\n", topic);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: %04x, iop: %04x, ret %d\n", 
                          pMsgAXDR->index, pMsgAXDR->label, pMsgAXDR->IID, pMsgAXDR->IOP, ret);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : radio_send_msg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_INFO_T *pMsgAXDR  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_send_axdr_msg_broadcast(MESSAGE_INFO_T *pMsgAXDR, int qos, uint16 iid, uint16 iop)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    char   topic[512] = {0};
    
    if(NULL == pMsgAXDR)
    {
        return -1;
    }

    msglen = appmsg_axdr_package(pMsgAXDR, &pMsg);
    if(NULL == pMsg)
    {
        MQTTMSG_FMT_DEBUG("App send appmsg_axdr_package fail\n");
        return -2;
    }
    sprintf(topic, "%s/%s/%04X/%04X", pMsgAXDR->souraddr, pMsgAXDR->destaddr, iid, iop);

    PTHREAD_LOCK_SAFE(&g_RadioSendlock);
    ret = mqtt_client_send_msg(g_RADIOMsgClient, topic, qos, pMsg, (int)msglen);
    PTHREAD_UNLOCK_SAFE(&g_RadioSendlock);

    if(0 == ret)
    {
        MQTTMSG_BUF_DEBUG(pMsg, msglen, "Send topic %s\n", topic);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: %04x, iop: %04x, ret %d\n", 
                          pMsgAXDR->index, pMsgAXDR->label, pMsgAXDR->IID, pMsgAXDR->IOP, ret);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : uart_send_msg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_MQTT_T *pMsgMqtt  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int radio_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos)
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
    /*else if(pMsgMqtt->tJson.pObject)
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
    }*/
    else
    {
        RADIO_FMT_DEBUG("Err input msg\n");
        return -3;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_RadioSendlock);
    ret = mqtt_client_send_msg(g_RADIOMsgClient, topic, qos, pMsg, (int)msglen);
    PTHREAD_UNLOCK_SAFE(&g_RadioSendlock);

    if(0 == ret)
    {
        if(MESSAGE_TYPE_A_XDR == pMsgMqtt->msgtype)
        {
            //心跳不打印
            if(pMsgMqtt->tAXDR.IID!=0x0000 && pMsgMqtt->tAXDR.IOP !=0x0010)
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
* @name      : radio_send_msg
* @brief     ：对外消息发送
* @param[in] ：char *topic               主题
               int qos                   消息级别
               char *pMsg                消息内容
               uint32 msglen             消息长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_send_json_msg(char *topic, int qos, char *pMsg, int msglen)
{
    int    ret = 0;
    
    if(NULL == topic || NULL == pMsg || 0 == msglen)
    {
        return -1;
    }
    
    PTHREAD_LOCK_SAFE(&g_RadioSendlock);
    ret = mqtt_client_send_msg(g_RADIOMsgClient, topic, qos, pMsg, msglen);
    PTHREAD_UNLOCK_SAFE(&g_RadioSendlock);

    if(0 == ret)
    {
        MQTTMSG_FMT_TRACE("Send topic %s\n%s\n", topic, pMsg);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Mqtt send failed, topic %s\n%s\n", topic, pMsg);
    }

    return ret;
}

/**********************************************************************
* @name      : radio_recv_mqtt
* @brief     ：对外消息接收 1s阻塞时间
* @param[in] ：uint16 topiclen  主题缓存容量
               uint16 buflen    缓存最大容量
* @param[out]：char *pTopic     主题
               uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int radio_recv_mqtt(char *pTopic, uint16 topiclen, uint8 *pBuf, uint16 buflen)
{
    int qos;
    return mqtt_client_recv_msg(g_RADIOMsgClient, pTopic, topiclen, &qos, pBuf, (int)buflen, 1000);
}

/**********************************************************************
* @name      : radio_login_to_sys
* @brief     ：APP注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    : 
**********************************************************************/
int radio_login_to_sys()
{
    int ret = 0;
    char topic[256] = {0};
    char msg[256] = {0};
    uint32 msglen = 0;
    uint32 token = 0;
    SYS_APPREG_T tAppReg;
    Date_Time_S tTime;

    memset(&tAppReg, 0, sizeof(SYS_APPREG_T));
    
    tAppReg.name.num = strlen(RADIO_APP_NAME);
    memcpy(tAppReg.name.value, RADIO_APP_NAME, tAppReg.name.num);
    tAppReg.version = radio_stoi(APP_VERSION);

    app_get_maketime(APP_PUBLISH_TIME, &tTime);
    tAppReg.releaseDate.year = tTime.year_h;
    tAppReg.releaseDate.year = (tAppReg.releaseDate.year << 8) + tTime.year_l;
    tAppReg.releaseDate.month = tTime.month;
    tAppReg.releaseDate.day = tTime.day;
    tAppReg.releaseDate.hour = tTime.hour;
    tAppReg.releaseDate.min = tTime.min;
    tAppReg.releaseDate.sec = tTime.sec;

    token = radio_get_next_index();
    msglen = sys_appreg_package(token, &tAppReg, msg, 256);
    if(0 == msglen)
    {
        return 1;
    }
    sprintf(topic, "%s/%s/%s/%s/%s", RADIO_APP_NAME, SYS_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_INFO_REQ, SYS_TOPIC_REGISTER);
    ret = radio_send_json_msg(topic, 0, msg, msglen);
    return ret;
}

/**********************************************************************
* @name      : radio_heart_bear
* @brief     ：心跳应答
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_heart_bear(MESSAGE_JSON_T *pInMsg)
{
    int ret = 0;
    uint32 token = 0;
    char   topic[256] = {0};
    char   msg[256] = {0};
    uint32 msglen = 0;

    ret = sys_keepAlive_split(pInMsg->infopath, pInMsg->pObject, &token);
    if(0 != ret)
    {
        YCMSG_FMT_DEBUG("keepAlive split error %d\n", ret);
        return 1;
    }

    msglen = sys_keepAlive_package(token, msg, 256);
    if(0 == msglen)
    {
        return 2;
    }
    sprintf(topic, "%s/%s/%s/%s/%s", pInMsg->destapp, SYS_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_INFO_RES, SYS_TOPIC_KEEPALIVE);
    ret = radio_send_json_msg(topic, 0, msg, msglen);
    return ret;
}

/**********************************************************************
* @name      : radio_send_errmsg
* @brief     ：对外消息发送错误的消息
* @param[in] ：MESSAGE_INFO_T *pRecvMsg  收到的消息
               int errCode               错误码
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int radio_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode)
{
    uint8         buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;
    uint8            *temp = ptAXDR->playload;

    if(NULL == pRecvMsg)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptAXDR->rpm = 1;
    ptAXDR->priority = pRecvMsg->priority;
    ptAXDR->index = pRecvMsg->index;
    ptAXDR->label = pRecvMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME, ptAXDR->sourlen);

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
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_err_json_res
* @brief     ：错误JSON应答
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               char *err                错误信息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_err_json_res(MESSAGE_JSON_T *pInMsg, uint8 errcode, char *err)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char    topic[256] = {0};
    int     ret = -1;
    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return -1;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(pInMsg->token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "status", cJSON_CreateNumber(errcode));
    if(err)
    {
        cJSON_AddItemToObject(root, "statusDes", cJSON_CreateString(err));
    }
    
    pMsg = cJSON_Print(root);
    cJSON_Delete(root);
    
    /* 发送错误消息 */
    if(pMsg)
    {
        sprintf(topic, "%s/%s/%s/%s/%s/%s", pInMsg->destapp, pInMsg->sourapp, MQTT_TOPIC_JSON, pInMsg->operat,
                                            MQTT_TOPIC_INFO_RES, pInMsg->infopath);
        ret = radio_send_json_msg(topic, 0, pMsg, strlen(pMsg));
        free(pMsg);
    }
    return ret;
}


