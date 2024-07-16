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

#include "uart_comm.h"
#include "uart_mqtt.h"
#include "uart_msg.h"
#include "uart_api.h"
#include "uart_service.h"
#include "cJSON.h"


// 对外消息 客户端句柄
MQTT_CLIENT_T   *g_MqttClient;

pthread_mutex_t  g_MsgSendlock; 
pthread_mutex_t  g_MsgIndexlock; 
uint16           g_AppMsgindex = 0;

DB_AGENT         g_dbAgent = 0;

char      g_AppName[APPMSG_NAME_LEN] = {0};
char      g_AppVer[APPMSG_NAME_LEN] = {0};
APP_INFO_T g_APPdbCenter;  //数据中心
APP_INFO_T g_APPMod;       //模组管理器
APP_INFO_T g_APPGui;       //显示
extern UART_MODINFO_T g_uart_modinfo;

/**********************************************************************
* @name      : uart_next_index_init
* @brief     ：对外消息index管理初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
void uart_next_index_init()
{
    pthread_mutex_init(&g_MsgIndexlock, NULL);
    g_AppMsgindex = 0;
}

/**********************************************************************
* @name      : uart_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
uint16 uart_get_next_index()
{
    uint16 curindex = 0;

    PTHREAD_LOCK_SAFE(&g_MsgIndexlock);
    curindex = g_AppMsgindex;
    g_AppMsgindex++;
    PTHREAD_UNLOCK_SAFE(&g_MsgIndexlock);

    return curindex;
}

/**********************************************************************
* @name      : uart_mqtt_init
* @brief     ：mqtt初始化
* @param[in] ：uint32 ip
               uint16 port
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int uart_mqtt_init(UART_APPDATA_T *pData)
{
    int ret = 0;
    uint8 status = 0;
    char   mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[3];
    char  mqttstatus[2][16] = {"connectlost", "success"};
    uint32 ip = pData->mqttip;

    /* 新建客户端 */
    sprintf((char *)g_AppName, "M-%s", UART_APP_NAME);
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
    subtopic[2].qos = 0;
    memcpy(subtopic[2].topic, APP_SUBTOPIC3, strlen(APP_SUBTOPIC3));
    ret = mqtt_client_set_sub_topic(g_MqttClient, subtopic, 3);
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
    ret = mqtt_client_connect(g_MqttClient, mqttaddr, pData->mqttport, "127.0.0.1");
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
//    g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_UART_AGENT, uart_send_dbmsg, NULL, UART_DB_CLIENT_NUM, UART_APP_NAME);
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

    uart_mqtt_debug();

    uart_next_index_init();
    pthread_mutex_init(&g_MsgSendlock, NULL);
    memset(&g_APPdbCenter , 0, sizeof(APP_INFO_T));
    memset(&g_APPMod , 0, sizeof(APP_INFO_T));
    return 0;

errorend:
    mqtt_client_destroy(g_MqttClient);
    return ret;
}

#if DESC("公共发送接口", 1)

/**********************************************************************
* @name      : uart_send_axdrmsg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  对外消息
* @param[in] ：int qos                   消息级别               
* @param[out]：NA
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-08-15
* @Update    :
**********************************************************************/
int uart_send_axdrmsg(MESSAGE_INFO_T *pMsgInfo, int qos)
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
    sprintf(topic, "%s/%s", pMsgInfo->souraddr, pMsgInfo->destaddr);
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
* @name      : uart_send_jsonmsg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  对外消息
* @param[in] ：int qos                   消息级别               
* @param[out]：NA
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-08-15
* @Update    :
**********************************************************************/
int uart_send_jsonmsg(MESSAGE_JSON_T *pMsgInfo, int qos)
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
* @name      : uart_send_mqttmsg
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
int uart_send_mqttmsg(char *pTopic, int qos, void *msgdata, int msglen)
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
* @name      : uart_send_msg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_MQTT_T *pMsgMqtt  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos)
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

#endif

#if DESC("消息发送接口", 1)
/**********************************************************************
* @name      : uart_send_dbmsg
* @brief     ：高级app对外发送数据中心接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：uint16*         index
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_send_dbmsg(uint16 label, uint16 index, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;

    if(FALSE == g_APPdbCenter.Online)
    {
        MQTTMSG_FMT_TRACE("No dbCenter!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = index;
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPdbCenter, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_DB;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);
    
    ret = uart_send_axdrmsg(pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : uart_send_mapmsg
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int uart_send_mapmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;

    if(FALSE == g_APPMod.Online)
    {
        MQTTMSG_FMT_TRACE("No mapManager!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        YCMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = uart_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPMod, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_MODULE;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    
    ret = uart_send_axdrmsg(pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : uart_send_guimsg
* @brief     ：高级app对外发送显示
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-7-8
* @Update    :
**********************************************************************/
int uart_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;

    if(FALSE == g_APPGui.Online)
    {
        WIRELESS_FMT_TRACE("No GUI!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        WIRELESS_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = uart_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char*)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPGui, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_DESKGUI;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    
    ret = uart_send_axdrmsg(pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : uart_send_appmsg
* @brief     ：发送消息给其他APP
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  消息内容
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_send_appmsg(MESSAGE_INFO_T *pMsgInfo)
{
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    return uart_send_axdrmsg(pMsgInfo, 0);
}

#endif

#if DESC("AXDR消息处理", 1)
/**********************************************************************
* @name      : uart_login_to_smios
* @brief     ：APP注册
* @param[in] ：uint8 *pAppId   APPID
               uint8 appidlen  长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-10
* @Update    : 
**********************************************************************/
int uart_login_to_smios(uint8 *pAppId, uint8 appidlen)
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
    ptAXDR->index = uart_get_next_index();
    ptAXDR->priority = 0;
    ptAXDR->label = 0;
    ptAXDR->IID = MESSAGE_IID_SYSTEM;
    ptAXDR->IOP = SYSTEM_IOP_APP_REGISTER;
    
    ptAXDR->sourlen = strlen(g_AppName);
    memcpy(ptAXDR->souraddr , g_AppName, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(smios);
    memcpy(ptAXDR->destaddr, smios, ptAXDR->destlen);

    ptAXDR->msglen = msglen;
    
    return uart_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : uart_send_errmsg
* @brief     ：对外消息发送错误的消息
* @param[in] ：MESSAGE_INFO_T *pRecvMsg  收到的消息
               int errCode               错误码
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode)
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
    
    return uart_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : uart_heart_bear
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
int uart_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
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
    
    return uart_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : uart_get_appidmsg
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
int uart_get_appidmsg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    UART_APPDATA_T   *ptAppData = (UART_APPDATA_T*)pPublic;
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
    
    return uart_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : uart_get_versionmsg
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
int uart_get_version(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
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
    
    return uart_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : uart_seek_app
* @brief     ：发送APP查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-10
* @Update    :
**********************************************************************/
int uart_seek_app()
{
    MESSAGE_INFO_T  tMsgInfo;
    int             ret = 0;
    char           *smios = "M-smiOS";
    
    memset(&tMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    tMsgInfo.rpm = 1;
    tMsgInfo.priority = 2;

    tMsgInfo.index = uart_get_next_index();
    tMsgInfo.label = 0;
    
    tMsgInfo.sourlen = strlen((char *)g_AppName);
    memcpy(tMsgInfo.souraddr , g_AppName, tMsgInfo.sourlen);
    tMsgInfo.destlen = strlen(smios);
    memcpy(tMsgInfo.destaddr, smios, tMsgInfo.destlen);

    tMsgInfo.IID = MESSAGE_IID_SYSTEM;
    tMsgInfo.IOP = SYSTEM_IOP_GET_REGISTERAPP;
    tMsgInfo.msglen = 0;

    ret = uart_send_axdrmsg(&tMsgInfo, 0);
    return ret;    
}

/**********************************************************************
* @name      : uart_recv_regapp
* @brief     ：收到注册APP查询
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_recv_regapp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
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
    UART_MSGDATA_T *ptMsgData = (UART_MSGDATA_T*)pArg;
    
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

    ptMsgData->appStep = UART_STATUS_WORK;
    
    return 0;
}

/**********************************************************************
* @name      : uart_notify_send
* @brief     ：将事件发送给destApp
* @param[in] ：buffer       数据内容
               length      数据长度
* @param[out]：
* @Create    : 贺宁
* @Date      ：2021-7-12
* @Update    : 
**********************************************************************/
int uart_notify_send(uint16 index, uint16 IOP, char *destApp, uint8 *buffer, uint16 length)
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
    
    ptAXDR->IID = MESSAGE_IID_UART;
    ptAXDR->IOP = IOP;
    
    ptAXDR->sourlen = strlen(g_AppName);
    memcpy(ptAXDR->souraddr, g_AppName, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(destApp);
    memcpy(ptAXDR->destaddr, destApp, ptAXDR->destlen);

    ptAXDR->msglen = length;
    memcpy(ptAXDR->playload, buffer, length);

    ret = uart_send_axdrmsg(ptAXDR, 0);

    free(ptAXDR);

    return ret;
}

/**********************************************************************
* @name      : uart_slotinfo_resp
* @brief     ：收到模组查询返回
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_modeinfo_resp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8  slotnum = 0;
    uint8  bytes = 0;
    uint8 *pTemp = NULL;
    uint8  i = 0, j = 0;
    uint16  offset = 0;
    uint8  status = 0;
    uint8  infolen = 0;
    uint8  chnum = 0;
    uint8  iftype = 0;
    uint8  funcode = 0;
    uint8  isfind = FALSE;
    
    if(NULL == pInMsg)
    {
        return -1;
    }

    pTemp = pInMsg->playload;
    slotnum = appmsg_get_len_offset(&pTemp[offset], &bytes);
    offset += bytes;

    //查找模组功能配置
    for(i = 0; i < slotnum; i++)
    {
        status = pTemp[offset++]; //模组识别状态

        if(status == 0)
        {
            infolen = appmsg_get_len_offset(&pTemp[offset], &bytes); //模组型号
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&pTemp[offset], &bytes); //模组ID
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&pTemp[offset], &bytes); //软件版本
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&pTemp[offset], &bytes); //软件发布日期
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&pTemp[offset], &bytes); //硬件件版本
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&pTemp[offset], &bytes); //硬件发布日期
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&pTemp[offset], &bytes); //厂商编码
            offset += bytes;
            offset += infolen;

            chnum = appmsg_get_len_offset(&pTemp[offset], &bytes); //通道信息
            offset += bytes;

            for(j = 0; j < chnum; j++)
            {
                iftype = pTemp[offset++]; //接口类型
                funcode = pTemp[offset++]; //功能配置
                
                memset(g_uart_modinfo.drvName[j], 0, sizeof(g_uart_modinfo.drvName[j]));
                infolen = appmsg_get_len_offset(&pTemp[offset], &bytes); //驱动名称
                offset += bytes;
                memcpy(g_uart_modinfo.drvName[j], &pTemp[offset], infolen);
                offset += infolen;
                
                if(iftype == 1) //ACM口
                {
                    if(funcode == 3 || funcode == 5) //3-485 5-CAN
                    {
                        g_uart_modinfo.slot = i+1;
                        g_uart_modinfo.cfgfunc[j] = funcode;
                        isfind = TRUE;
                    }
                }
            }

            if(isfind == TRUE)
            {
                MQTTMSG_FMT_DEBUG("detected slot%d for \"%s\".\n", g_uart_modinfo.slot, MOD_UART_TYPE_PRFIX);
                break;
            }
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : uart_slotinfo_resp
* @brief     ：收到槽位模组查询返回
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_slotinfo_resp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8  slotnum = 0;
    uint8  bytes = 0;
    uint8 *pTemp = NULL;
    uint8  i = 0;
    uint8  offset = 0;
    
    if(NULL == pInMsg)
    {
        return -1;
    }

    pTemp = pInMsg->playload;
    slotnum = appmsg_get_len_offset(&pTemp[offset], &bytes);
    offset += bytes;

    //当前只支持1个模组
    for(i = 0; (i< 1) && (i < slotnum); i++)
    {
        memcpy_r(&g_uart_modinfo.slot, &pTemp[offset], sizeof(int));
        pTemp += sizeof(int);

        if(offset > pInMsg->msglen)
        {
            MQTTMSG_FMT_DEBUG("module slot ack msg fatal error: offset(%d) > msglen(%d).\n", offset, pInMsg->msglen);
            return ERR_OVERFLOW;
        }

        MQTTMSG_FMT_DEBUG("detected slot%d for \"%s\".\n", g_uart_modinfo.slot, MOD_UART_TYPE_PRFIX);
    }
    
    return 0;
}

/**********************************************************************
* @name      : uart_modchange_evt
* @brief     ：收到槽位模组查询返回
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_modchange_evt(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;
    uint16 msglen = pInMsg->msglen;
    uint8 *revcbuf = pInMsg->playload;
    uint8  offset = 0;
    int slot = 0;
    uint8 status = 0;
    uint8  bytes = 0;
    uint8  infolen = 0;
    uint8  chnum = 0;
    uint8  iftype = 0;
    uint8  funcode = 0;
    uint8  isfind = FALSE;
    uint8  i = 0;

    if(msglen != 0)
    {
        memcpy_r(&slot, &revcbuf[offset], sizeof(int));
        offset += sizeof(int);  

        memcpy(&status, &revcbuf[offset], sizeof(uint8));
        offset += sizeof(uint8);

        //模组插入则读模组信息
        if(status == 0)
        {
            infolen = appmsg_get_len_offset(&revcbuf[offset], &bytes); //模组型号
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&revcbuf[offset], &bytes); //模组ID
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&revcbuf[offset], &bytes); //软件版本
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&revcbuf[offset], &bytes); //软件发布日期
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&revcbuf[offset], &bytes); //硬件件版本
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&revcbuf[offset], &bytes); //硬件发布日期
            offset += bytes;
            offset += infolen;

            infolen = appmsg_get_len_offset(&revcbuf[offset], &bytes); //厂商编码
            offset += bytes;
            offset += infolen;

            chnum = appmsg_get_len_offset(&revcbuf[offset], &bytes); //通道信息
            offset += bytes;

            for(i = 0; i < chnum; i++)
            {
                iftype = revcbuf[offset++]; //接口类型
                funcode = revcbuf[offset++]; //功能配置
                
                memset(g_uart_modinfo.drvName[i], 0, sizeof(g_uart_modinfo.drvName[i]));
                infolen = appmsg_get_len_offset(&revcbuf[offset], &bytes); //驱动名称
                offset += bytes;
                memcpy(g_uart_modinfo.drvName[i], &revcbuf[offset], infolen);
                offset += infolen;
                
                if(iftype == 1) //ACM口
                {
                    if(funcode == 3 || funcode == 5) //3-485 5-CAN
                    {
                        g_uart_modinfo.cfgfunc[i] = funcode;
                        isfind = TRUE;
                    }
                }
            }
        }

        if(offset > msglen)
        {
            MQTTMSG_FMT_DEBUG("module plug/uplug msg: offset(%d) > msglen(%d).\n", offset, msglen);
            return ERR_OVERFLOW;
        }

        //模块已经拔出，再检测到插入模块
        if((g_uart_modinfo.slot == 0) &&(isfind == TRUE))
        {        
            g_uart_modinfo.slot = slot;
            g_uart_modinfo.status = MOD_STATUS_INSERT;

            g_uart_modinfo.change[1] = TRUE;
            g_uart_modinfo.change[2] = TRUE;
            g_uart_modinfo.change[3] = TRUE;
            g_uart_modinfo.change[4] = TRUE;
            
            MQTTMSG_FMT_DEBUG("module plug notify: slot(%d)\n", slot);
        }
        else if((status == 1) && (g_uart_modinfo.slot == slot)) //拔出目标模块，则清零槽位和状态，待查询到新信息
        {
            g_uart_modinfo.slot = 0;
            g_uart_modinfo.status = MOD_STATUS_EXTRACT; 
            
            g_uart_modinfo.change[1] = TRUE;
            g_uart_modinfo.change[2] = TRUE;
            g_uart_modinfo.change[3] = TRUE;
            g_uart_modinfo.change[4] = TRUE;
            
            MQTTMSG_FMT_DEBUG("module unplug: slot(%d)\n", slot);
        }
    }
    
    return ret;
}

/**********************************************************************
* @name      : uart_seek_app
* @brief     ：发送APP查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-10
* @Update    :
**********************************************************************/
int uart_slotinfo_send(const char *type)
{
    MESSAGE_INFO_T  tMsgInfo;
    int             ret = 0;
    char           *destapp = "M-mapManager";
    uint8 sendbuf[128] = {0};
    uint8 size = 0;
    
    size = strlen(type);
    sendbuf[0] = size;
    memcpy(sendbuf + 1, type, size);
    
    memset(&tMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    tMsgInfo.rpm = 1;
    tMsgInfo.priority = 2;

    tMsgInfo.index = uart_get_next_index();
    tMsgInfo.label = 0;
    
    tMsgInfo.sourlen = strlen((char *)g_AppName);
    memcpy(tMsgInfo.souraddr , g_AppName, tMsgInfo.sourlen);
    tMsgInfo.destlen = strlen(destapp);
    memcpy(tMsgInfo.destaddr, destapp, tMsgInfo.destlen);

    tMsgInfo.IID = MESSAGE_IID_MODULE;
    tMsgInfo.IOP = MODULE_IOP_GET_PORTINFO;
    tMsgInfo.msglen = size+1;
    memcpy(tMsgInfo.playload, sendbuf, size+1);

    ret = uart_send_axdrmsg(&tMsgInfo, 0);
    return ret;    
}


/**********************************************************************
* @name      : uart_seek_app
* @brief     ：发送APP查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-10
* @Update    :
**********************************************************************/
int uart_modinfo_send(void)
{
    MESSAGE_INFO_T  tMsgInfo;
    int             ret = 0;
    char           *destapp = "M-mapManager";
    
    memset(&tMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    tMsgInfo.rpm = 1;
    tMsgInfo.priority = 2;

    tMsgInfo.index = uart_get_next_index();
    tMsgInfo.label = 0;
    
    tMsgInfo.sourlen = strlen((char *)g_AppName);
    memcpy(tMsgInfo.souraddr , g_AppName, tMsgInfo.sourlen);
    tMsgInfo.destlen = strlen(destapp);
    memcpy(tMsgInfo.destaddr, destapp, tMsgInfo.destlen);

    tMsgInfo.IID = MESSAGE_IID_MODULE;
    tMsgInfo.IOP = MODULE_IOP_GET_ALL;
    tMsgInfo.msglen = 0;

    ret = uart_send_axdrmsg(&tMsgInfo, 0);
    return ret;    
}

#endif

/**********************************************************************
* @name      : uart_dbCenter_flag
* @brief     ：回路巡检准备正常
* @param[in] ：
* @param[out]：
* @return    ：TRUE-FALSE
* @Create    : 贺宁
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
uint8 uart_dbCenter_flag()
{
    if(TRUE == g_APPdbCenter.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : uart_appmap_flag
* @brief     ：回路巡检查询到模组管理器
* @param[in] ：
* @param[out]：
* @return    ：TRUE-FALSE
* @Create    : 贺宁
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
uint8 uart_appmap_flag()
{
    if(TRUE == g_APPMod.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : uart_appgui_flag
* @brief     ：回路巡检查询到桌面显示
* @param[in] ：
* @param[out]：
* @return    ：TRUE-FALSE
* @Create    : 贺宁
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
uint8 uart_appgui_flag()
{
    if(TRUE == g_APPGui.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : uart_get_selfversion
* @brief     ：获取内部版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
int uart_get_selfversion(int sockid, MESSAGE_INFO_T *pInMsg)
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
    temp[msglen++] = strlen(UART_APP_NAME);
    memcpy(temp + msglen, UART_APP_NAME, strlen(UART_APP_NAME));
    msglen += strlen(UART_APP_NAME);

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
    
    return uart_send_axdrmsg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : uart_recv_mqttmsg
* @brief     ：对外消息接收, 使用了mqtt_client_recv_msg 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_recv_mqttmsg(char *topic, uint16 topicLen, uint8 *pBuf, uint16 buflen)
{
    int qos;
    return mqtt_client_recv_msg(g_MqttClient, topic, topicLen, &qos, pBuf, (int)buflen, 1000);
}

/**********************************************************************
* @name      : uart_mqtt_debug
* @brief     ：mqtt调试信息打印
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-3
* @Update    :
**********************************************************************/
void uart_mqtt_debug()
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
* @name      : uart_login_to_sys
* @brief     ：APP注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int uart_login_to_sys()
{
    int ret = 0;
    char topic[256] = {0};
    char msg[256] = {0};
    uint32 msglen = 0;
    uint32 token = 0;
    SYS_APPREG_T tAppReg;
    Date_Time_S tTime;

    memset(&tAppReg, 0, sizeof(SYS_APPREG_T));
    
    tAppReg.name.num = strlen(UART_APP_NAME);
    memcpy(tAppReg.name.value, UART_APP_NAME, tAppReg.name.num);
    tAppReg.version = (uint32)APP_VERSION;

    app_get_maketime(APP_PUBLISH_TIME, &tTime);
    tAppReg.releaseDate.year = tTime.year_h;
    tAppReg.releaseDate.year = (tAppReg.releaseDate.year << 8) + tTime.year_l;
    tAppReg.releaseDate.month = tTime.month;
    tAppReg.releaseDate.day = tTime.day;
    tAppReg.releaseDate.hour = tTime.hour;
    tAppReg.releaseDate.min = tTime.min;
    tAppReg.releaseDate.sec = tTime.sec;

    token = uart_get_next_index();
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
* @name      : uart_heart_to_sys
* @brief     ：心跳
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int uart_heart_to_sys(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic)
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
* @name      : uart_get_appreglist
* @brief     ：APP注册列表查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int uart_get_appreglist()
{
    int ret = 0;
    char   topic[256] = {0};
    char   msg[256] = {0};
    uint32 msglen = 0;
    uint32 token = 0;

    /* 进程号 */
    token = uart_get_next_index();
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
* @name      : uart_appreglist_check
* @brief     ：app注册列表解析
* @param[in] ：MESSAGE_JSON_T *pInMsg    应答消息
* @param[out]：
* @return    ：FALSE-继续/TRUE-完成
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
uint8 uart_appreglist_check(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic)
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
        if(FALSE == g_APPdbCenter.Online && NULL != strstr(tAppList.name[i].value, "dbCenter"))
        {
            MQTTMSG_FMT_DEBUG("dbCenter register\n");
            g_APPdbCenter.Online = TRUE;
            memcpy(g_APPdbCenter.name, "dbCenter", strlen("dbCenter"));
            g_APPdbCenter.commType = 'M';
        }
        if(FALSE == g_APPMod.Online && NULL != strstr(tAppList.name[i].value, "mapManager"))
        {
            MQTTMSG_FMT_DEBUG("mapManager register\n");
            g_APPMod.Online = TRUE;
            memcpy(g_APPMod.name, "mapManager", strlen("mapManager"));
            g_APPMod.commType = 'M';
        }
    }
    
    return ret;
}

#endif

