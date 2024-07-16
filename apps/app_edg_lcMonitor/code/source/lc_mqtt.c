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
#include <sys/un.h>

#include "task_id.h"
#include "task_msg.h"

#include "db_api.h"
#include "appmsg_split.h"
#include "mqtt_client.h"

#include "lc_comm.h"
#include "lc_mqtt.h"
#include "lc_msg.h"
#include "cJSON.h"


// 对外消息 客户端句柄
MQTT_CLIENT_T   *g_LcMqttClient;
pthread_mutex_t  g_MsgSendlock; 
pthread_mutex_t  g_MsgIndexlock; 
uint16           g_AppMsgindex = 0;

DB_AGENT         g_dbAgent = 0;

char      g_AppName[APPMSG_NAME_LEN] = {0};
char      g_AppVer[APPMSG_NAME_LEN] = {0};

APP_INFO_T g_APPdbCenter;  //数据中心
APP_INFO_T g_APPMod;       //模组管理器
APP_INFO_T g_APPGui;       //显示

/**********************************************************************
* @name      : lc_next_index_init
* @brief     ：对外消息index管理初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
void lc_next_index_init()
{
    pthread_mutex_init(&g_MsgIndexlock, NULL);
    g_AppMsgindex = 0;
}

/**********************************************************************
* @name      : lc_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
uint16 lc_get_next_index()
{
    uint16 curindex = 0;

    PTHREAD_LOCK_SAFE(&g_MsgIndexlock);
    curindex = g_AppMsgindex;
    g_AppMsgindex++;
    PTHREAD_UNLOCK_SAFE(&g_MsgIndexlock);

    return curindex;
}

/**********************************************************************
* @name      : lc_mqtt_init
* @brief     ：mqtt初始化
* @param[in] ：uint32 ip
               uint16 port
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int lc_mqtt_init(LC_APPDATA_T *pData)
{
    int ret = 0;
    uint8 status = 0;
    char   mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[3];
    char  mqttstatus[2][16] = {"connectlost", "success"};
    uint32 ip = pData->mqttip;

    /* 新建客户端 */
    sprintf((char *)g_AppName, "M-%s", LC_APP_NAME);
    sprintf((char *)g_AppVer, "%s", APP_VERSION);
    g_LcMqttClient = mqtt_client_create((char *)g_AppName);
    if (NULL == g_LcMqttClient)
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
    ret = mqtt_client_set_sub_topic(g_LcMqttClient, subtopic, 3);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(g_LcMqttClient, APP_DEFAULT_TOPIC, 0);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 链接服务端 */
    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    MQTTMSG_FMT_DEBUG("mqtt connect %s:%d\n", mqttaddr, pData->mqttport);
    ret = mqtt_client_connect(g_LcMqttClient, mqttaddr, pData->mqttport, NULL);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_connect fail, ret %d", ret);
        goto errorend;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(g_LcMqttClient);
        MQTTMSG_FMT_DEBUG("Get mqtt connect status %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    MQTTMSG_FMT_DEBUG("mqtt connect %s:%d success\n", mqttaddr, pData->mqttport);

#ifdef PRODUCT_SCU
    /* 创建db_agent */
    g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_LC_AGENT, lc_send_dbmsg, NULL, LC_DB_CLIENT_NUM, LC_APP_NAME);
#else
    /* 创建db_agent */
    g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_LC_AGENT, lc_send_dbmsg, LC_DB_CLIENT_NUM);
#endif
    if(g_dbAgent < 0)
    {
        LCMONIT_FMT_DEBUG("db_agent_create failed!\n");
        mqtt_client_disconnect(g_LcMqttClient);
        ret = -1;
        goto errorend;
    }
    pData->dbAgent = g_dbAgent;
    pData->appName = (char*)g_AppName;

    lc_mqtt_debug();

    lc_next_index_init();
    pthread_mutex_init(&g_MsgSendlock, NULL);
    memset(&g_APPdbCenter , 0, sizeof(APP_INFO_T));
    memset(&g_APPMod , 0, sizeof(APP_INFO_T));
    return 0;

errorend:
    mqtt_client_destroy(g_LcMqttClient);
    return ret;
}

/**********************************************************************
* @name      : lc_send_axdrmsg
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  对外消息
* @param[in] ：int qos                   消息级别               
* @param[out]：NA
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-08-15
* @Update    :
**********************************************************************/
int lc_send_axdrmsg(MESSAGE_INFO_T *pMsgInfo, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    char   topic[APPMSG_NAME_LEN*3] = {0};
    
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
    ret = mqtt_client_send_msg(g_LcMqttClient, topic, qos, pMsg, (int)msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);

    if(0 == ret)
    {
        MQTTMSG_BUF_TRACE(pMsg, msglen, "topic: %s Send msg:\n", topic);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                          pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : lc_send_mqttmsg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_send_mqttmsg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    msglen = appmsg_axdr_package(pMsgInfo, &pMsg);
    if(0 == msglen)
    {
        MQTTMSG_FMT_DEBUG("App send appmsg_axdr_package fail, IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        return -1;
    }

    if(msglen >= APP_RECV_BUF_LEN)
    {
        MQTTMSG_FMT_DEBUG("App send appmsg_axdr_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    if(NULL != pTopic && 'M' == pMsgInfo->destaddr[0])
    {
        ret = mqtt_client_send_msg(g_LcMqttClient, pTopic, qos, pMsg, (int)msglen);
        MQTTMSG_FMT_TRACE("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);
    }
    else
    {
        ret = mqtt_client_send(g_LcMqttClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);

    if(0 == ret)
    {
        MQTTMSG_BUF_TRACE(pMsg, msglen, "%s, Send msg:\n", pTopic);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                          pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
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
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int lc_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos)
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
    ret = mqtt_client_send_msg(g_LcMqttClient, topic, qos, pMsg, (int)msglen);
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

#ifdef PRODUCT_SCU
/**********************************************************************
* @name      : lc_send_dbmsg
* @brief     ：数据中心消息发送接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：uint16*         index
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2022-5-25
* @Update    :
**********************************************************************/
int lc_send_dbmsg(uint16 label, uint16 index, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;

//    if(FALSE == g_APPdbCenter.Online)
//    {
//        MQTTMSG_FMT_TRACE("No dbCenter!\n");
//        return -1;
//    }

    strcpy(g_APPdbCenter.name, "dataCenter");
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    pMsgInfo->index = index;
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen((char *)g_APPdbCenter.name);
    memcpy(pMsgInfo->destaddr, g_APPdbCenter.name, pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_DB;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    ret = lc_send_axdrmsg(pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}
#else
/**********************************************************************
* @name      : lc_send_dbmsg
* @brief     ：数据中心消息发送接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：uint16*         index
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2022-5-25
* @Update    :
**********************************************************************/
int lc_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[APPMSG_NAME_LEN*3] = {0};

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

    *pIndex = lc_get_next_index();
    pMsgInfo->index = *pIndex;
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPdbCenter, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_DB;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/M-%s", g_AppName, g_APPdbCenter.name);
    ret = lc_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

#endif
/**********************************************************************
* @name      : lc_send_mapmsg
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int lc_send_mapmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[APPMSG_NAME_LEN*3] = {0};

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

    pMsgInfo->index = lc_get_next_index();
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
    sprintf(uTopic, "%s/%c-%s", g_AppName, g_APPMod.commType, g_APPMod.name);
    ret = lc_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : lc_send_guimsg
* @brief     ：高级app对外发送显示
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-7-8
* @Update    :
**********************************************************************/
int lc_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[APPMSG_NAME_LEN*3] = {0};

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

    pMsgInfo->index = lc_get_next_index();
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
    sprintf(uTopic, "%s/M-%s", g_AppName, g_APPGui.name);
    ret = lc_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : lc_send_appmsg
* @brief     ：发送消息给其他APP
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  消息内容
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_send_appmsg(MESSAGE_INFO_T *pMsgInfo)
{
    char    uTopic[256] = {0};

    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    sprintf(uTopic, "%s/%s", g_AppName, pMsgInfo->destaddr);
    return lc_send_mqttmsg(uTopic, pMsgInfo, 0);
}

/**********************************************************************
* @name      : lc_dbCenter_flag
* @brief     ：回路巡检准备正常
* @param[in] ：
* @param[out]：
* @return    ：TRUE-FALSE
* @Create    : 王津剑
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
uint8 lc_dbCenter_flag()
{
    if(TRUE == g_APPdbCenter.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : lc_appmap_flag
* @brief     ：回路巡检查询到模组管理器
* @param[in] ：
* @param[out]：
* @return    ：TRUE-FALSE
* @Create    : 王津剑
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
uint8 lc_appmap_flag()
{
    if(TRUE == g_APPMod.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : lc_appgui_flag
* @brief     ：回路巡检查询到桌面显示
* @param[in] ：
* @param[out]：
* @return    ：TRUE-FALSE
* @Create    : 王津剑
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
uint8 lc_appgui_flag()
{
    if(TRUE == g_APPGui.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : lc_send_errmsg
* @brief     ：对外消息发送错误的消息
* @param[in] ：MESSAGE_INFO_T *pRecvMsg  收到的消息
               int errCode               错误码
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
int lc_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode)
{
    char         uTopic[APPMSG_NAME_LEN*3] = {0};
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    uint8             *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;

    if(NULL == pRecvMsg)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptMsgNew->rpm = 1;
    ptMsgNew->priority = pRecvMsg->priority;
    ptMsgNew->index = pRecvMsg->index;
    ptMsgNew->label = pRecvMsg->label;

    ptMsgNew->sourlen = strlen((char*)g_AppName);
    memcpy(ptMsgNew->souraddr, g_AppName, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pRecvMsg->souraddr, pRecvMsg->sourlen);
    ptMsgNew->destlen = pRecvMsg->sourlen;

    ptMsgNew->IID = MESSAGE_IID_COMMON;
    ptMsgNew->IOP = COMMON_IOP_ERRMSG;

    temp = ptMsgNew->playload;
    appmsg_memcpy_r(temp, &(pRecvMsg->IID), 2);
    msglen += 2;
    appmsg_memcpy_r(temp + msglen, &(pRecvMsg->IOP), 2);
    msglen += 2;
    appmsg_memcpy_r(temp + msglen, &errCode, 2);
    msglen += 2;

    ptMsgNew->msglen = msglen;
    
    sprintf(uTopic, "%s/%s", g_AppName, pRecvMsg->souraddr);
    return lc_send_mqttmsg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : lc_get_selfversion
* @brief     ：获取内部版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
int lc_get_selfversion(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    char         uTopic[APPMSG_NAME_LEN*3] = {0};
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
    temp[msglen++] = strlen(LC_APP_NAME);
    memcpy(temp + msglen, LC_APP_NAME, strlen(LC_APP_NAME));
    msglen += strlen(LC_APP_NAME);

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
    
    sprintf(uTopic, "%s/%s", g_AppName, pInMsg->souraddr);
    return lc_send_mqttmsg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : lc_recv_mqttmsg
* @brief     ：对外消息接收, 使用了mqtt_client_recv_msg 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_recv_mqttmsg(char *topic, uint16 topicLen, uint8 *pBuf, uint16 buflen)
{
    int qos;
    return mqtt_client_recv_msg(g_LcMqttClient, topic, topicLen, &qos, pBuf, (int)buflen, 1000);
}

/**********************************************************************
* @name      : lc_mqtt_debug
* @brief     ：mqtt调试信息打印
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-3
* @Update    :
**********************************************************************/
void lc_mqtt_debug()
{
    char   buflog[1024] = {0};

    if(0 != mqtt_msg_handle_debug(g_LcMqttClient, buflog, 1024))
    {
        MQTTMSG_FMT_DEBUG("Mqtt client debug info\n%s\n", buflog);
        printf("Mqtt client debug info\n%s\n", buflog);
    }
}

#if DESC("AXDR消息",1)

/**********************************************************************
* @name      : lc_login_to_smios
* @brief     ：APP注册
* @param[in] ：uint8 *pAppId   APPID
               uint8 appidlen  长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    : 2020-7-19
**********************************************************************/
int lc_login_to_smios(uint8 *pAppId, uint8 appidlen)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 request[240] = {0};
    uint8 offset = 0;
    uint8 len = 0;

    pid_t pid = getpid();
    Date_Time_S tTime;
    char *smios = "M-smiOS";

    app_get_maketime(APP_PUBLISH_TIME, &tTime);

    /* 进程号 */
    appmsg_memcpy_r(request, &pid, sizeof(pid_t));
    offset += 4;

    /* APP名称 */
    len = strlen((char *)g_AppName);
    request[offset++] = len;
    memcpy(&request[offset], g_AppName, len);  
    offset += len;

    /* 订阅事件 */
    request[offset++] = 0;

    /* 版本信息 */
    len = strlen((char *)g_AppVer);
    request[offset++] = len;
    memcpy(&request[offset], g_AppVer, len);
    offset += len;

    /* 发布日期 */
    request[offset++] = tTime.year_h;
    request[offset++] = tTime.year_l;
    request[offset++] = tTime.month;
    request[offset++] = tTime.day;
    request[offset++] = tTime.hour;
    request[offset++] = tTime.min;
    request[offset++] = tTime.sec;

    /* APPID */
    request[offset++] = appidlen;
    if(appidlen > 0)
    {
        memcpy(request + offset, pAppId, appidlen);
    }
    offset += appidlen;
    
    pMsgInfo = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + offset);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Login to os malloc failed %d\n", sizeof(MESSAGE_INFO_T) + offset);
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = lc_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = SYSTEM_IOP_APP_REGISTER;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr , g_AppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = offset;
    memcpy(pMsgInfo->playload, request, offset);

    ret = lc_send_mqttmsg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : lc_heart_bear
* @brief     ：心跳
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int lc_heart_bear(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16 len = 0;
    uint32 status = 1;
    char *smios = "M-smiOS";

    len = sizeof(MESSAGE_INFO_T) + sizeof(uint32);
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Heart bear malloc failed %d\n", len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 0;
    pMsgInfo->index = pInMsg->index;
    pMsgInfo->priority = pInMsg->priority;
    pMsgInfo->label = pInMsg->label;
    pMsgInfo->IID = MESSAGE_IID_COMMON;
    pMsgInfo->IOP = COMMON_IOP_HERTBEAT;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr , g_AppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = sizeof(uint32);
    appmsg_memcpy_r(pMsgInfo->playload, &status, sizeof(uint32));

    ret = lc_send_mqttmsg(NULL, pMsgInfo, 0);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("=======advance send heart err=======\n");
    }
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : lc_seek_app
* @brief     ：发送APP查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_seek_app()
{
    MESSAGE_INFO_T  tMsgInfo;
    int             ret = 0;
    char           *smios = "M-smiOS";
    
    memset(&tMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    tMsgInfo.rpm = 1;
    tMsgInfo.priority = 2;

    tMsgInfo.index = lc_get_next_index();
    tMsgInfo.label = 0;
    
    tMsgInfo.sourlen = strlen((char *)g_AppName);
    memcpy(tMsgInfo.souraddr , g_AppName, tMsgInfo.sourlen);
    tMsgInfo.destlen = strlen(smios);
    memcpy(tMsgInfo.destaddr, smios, tMsgInfo.destlen);

    tMsgInfo.IID = MESSAGE_IID_SYSTEM;
    tMsgInfo.IOP = SYSTEM_IOP_GET_REGISTERAPP;
    tMsgInfo.msglen = 0;

    ret = lc_send_mqttmsg(NULL, &tMsgInfo, 0);
    return ret;    
}

/**********************************************************************
* @name      : lc_recv_regapp
* @brief     ：收到注册APP查询
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_recv_regapp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8  appnun = 0;
    uint8  bytes = 0;
    uint16 offset = 0;
    uint8 *pTemp = NULL;
    uint8  i = 0;
    uint8  namelen = 0;
    uint8  ucname[APPMSG_NAME_LEN] = {0};
    APP_INFO_T tApp;
    uint8  subnum = 0;
    LC_MSGDATA_T *ptMsgData = (LC_MSGDATA_T*)pArg;
    uint8 versionlen = 0;
    uint8 version[64]={0};
    OOP_DATETIME_S_T dateTimeS;
    uint8  appIdLen = 0;
    uint8  appId[128] = {0};
    
    if(NULL == pInMsg)
    {
        return FALSE;
    }

    pTemp = pInMsg->playload;
    appnun = appmsg_get_len_offset(pTemp, &bytes);
    pTemp += bytes;

    for(i = 0; i < appnun; i++)
    {
        namelen = pTemp[offset++];
        
        memset(ucname, 0, APPMSG_NAME_LEN);
        memset(&dateTimeS, 0, sizeof(OOP_DATETIME_S_T));
        memset(appId, 0, 128);
        memcpy(ucname, pTemp + offset, namelen);
        memset(&tApp, 0, sizeof(APP_INFO_T));
        if(0 == appname_split(ucname, &tApp))
        {
            if(0 == strcmp("dbCenter", tApp.name) && FALSE == g_APPdbCenter.Online)
            {
                memcpy(&g_APPdbCenter, &tApp, sizeof(APP_INFO_T));
                g_APPdbCenter.Online = TRUE;
                printf("======get dbcenter======\n");
                MQTTMSG_FMT_DEBUG("======get dbcenter======\n");
            }
            else if(0 == strcmp("mapManager", tApp.name) && FALSE == g_APPMod.Online)
            {
                memcpy(&g_APPMod, &tApp, sizeof(APP_INFO_T));
                g_APPMod.Online = TRUE;
                printf("======get mapManager======\n");
                MQTTMSG_FMT_DEBUG("======get mapManager======\n");
            }
            else if(0 == strcmp("desktopGui", tApp.name) && FALSE == g_APPGui.Online)
            {
                memcpy(&g_APPGui, &tApp, sizeof(APP_INFO_T));
                g_APPGui.Online = TRUE;
                printf("======get desktopGui======\n");
                MQTTMSG_FMT_DEBUG("======get desktopGui======\n");
            }
        }

        offset += namelen;
        subnum = pTemp[offset++];
        offset += subnum * sizeof(uint32);  //订阅事件

        versionlen = pTemp[offset++];
        memset(version,0x00,sizeof(version));
        memcpy(version,&pTemp[offset],versionlen);
        offset+=versionlen;

        memcpy(&dateTimeS, &pTemp[offset], sizeof(Date_Time_S)); //发布时间
        offset += sizeof(Date_Time_S);

        appIdLen = pTemp[offset++];
        memcpy(appId, pTemp + offset, appIdLen);  //APP_ID
        offset += appIdLen;

        pTemp += offset;
        offset = 0;
    }

#ifdef PRODUCT_ECU
    if(TRUE == g_APPdbCenter.Online && TRUE == g_APPMod.Online && TRUE == g_APPGui.Online)
    {
        ptMsgData->appStep = LC_STATUS_WORK;
    }
#else
    if(TRUE == g_APPdbCenter.Online && TRUE == g_APPGui.Online)
    {
        ptMsgData->appStep = LC_STATUS_WORK;
    }
#endif
    return 0;
}

/**********************************************************************
* @name      : lc_get_version
* @brief     ：查询版本号
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_get_version(MESSAGE_INFO_T *pInMsg)
{
    char         uTopic[APPMSG_NAME_LEN*3] = {0};
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    uint8              *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;
    Date_Time_S tTime;
    uint8 len = 0;

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
    ptMsgNew->msglen = msglen;

    sprintf(uTopic, "%s/%s", g_AppName, pInMsg->souraddr);
    return lc_send_mqttmsg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : lc_get_appidmsg
* @brief     ：查询appid
* @param[in] ：MESSAGE_INFO_T *pInMsg  查询消息
               uint8 *pAppId   APPID
               uint8 appidlen  长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-7-19
* @Update    :
**********************************************************************/
int lc_get_appidmsg(MESSAGE_INFO_T *pInMsg, uint8 *pAppId, uint8 appidlen)
{
    char         uTopic[APPMSG_NAME_LEN*3] = {0};
    uint8           buf[360] = {0};
    uint8              *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;

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

    /* APPID */
    *temp = appidlen;
    if(appidlen > 0)
    {
        memcpy(temp + 1, pAppId, appidlen);
    }

    ptMsgNew->msglen = appidlen + 1;

    sprintf(uTopic, "%s/%s", g_AppName, pInMsg->souraddr);
    return lc_send_mqttmsg(uTopic, ptMsgNew, 0);
}

#endif

#if DESC("JSON消息",1)

/**********************************************************************
* @name      : lc_login_to_sys
* @brief     ：APP注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int lc_login_to_sys()
{
    int ret = 0;
    char topic[256] = {0};
    char msg[256] = {0};
    uint32 msglen = 0;
    uint32 token = 0;
    SYS_APPREG_T tAppReg;
    Date_Time_S tTime;

    memset(&tAppReg, 0, sizeof(SYS_APPREG_T));
    
    tAppReg.name.num = strlen(LC_APP_NAME);
    memcpy(tAppReg.name.value, LC_APP_NAME, tAppReg.name.num);
    tAppReg.version = (uint32)APP_VERSION;

    app_get_maketime(APP_PUBLISH_TIME, &tTime);
    tAppReg.releaseDate.year = tTime.year_h;
    tAppReg.releaseDate.year = (tAppReg.releaseDate.year << 8) + tTime.year_l;
    tAppReg.releaseDate.month = tTime.month;
    tAppReg.releaseDate.day = tTime.day;
    tAppReg.releaseDate.hour = tTime.hour;
    tAppReg.releaseDate.min = tTime.min;
    tAppReg.releaseDate.sec = tTime.sec;

    token = lc_get_next_index();
    msglen = sys_appreg_package(token, &tAppReg, msg, 256);
    if(0 == msglen)
    {
        return 1;
    }
    sprintf(topic, "%s/%s/%s/%s/%s", g_AppName, SYS_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_INFO_REQ, SYS_TOPIC_REGISTER);

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_LcMqttClient, topic, 0, msg, msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);
    if(0 == ret)
    {
        MQTTMSG_FMT_DEBUG("Send msg, tpoic %s, msg:\n%s\n", topic, msg);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send msg failed ret %d, tpoic %s, msg:\n%s\n", ret, topic, msg);
    }

    return ret;
}

/**********************************************************************
* @name      : lc_heart_to_sys
* @brief     ：心跳
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int lc_heart_to_sys(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic)
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
    ret = mqtt_client_send_msg(g_LcMqttClient, topic, 0, msg, msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);
    if(0 == ret)
    {
        MQTTMSG_FMT_DEBUG("Send msg, tpoic %s, msg:\n%s\n", topic, msg);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send msg failed ret %d, tpoic %s, msg:\n%s\n", ret, topic, msg);
    }

    return ret;
}

/**********************************************************************
* @name      : lc_get_appreglist
* @brief     ：APP注册列表查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int lc_get_appreglist()
{
    int ret = 0;
    char   topic[256] = {0};
    char   msg[256] = {0};
    uint32 msglen = 0;
    uint32 token = 0;

    /* 进程号 */
    token = lc_get_next_index();
    msglen = sys_common_package(token, msg, 256);
    if(0 == msglen)
    {
        return 1;
    }
    sprintf(topic, "%s/%s/%s/%s/%s", g_AppName, SYS_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_INFO_REQ, SYS_TOPIC_GETREGLIST);

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    ret = mqtt_client_send_msg(g_LcMqttClient, topic, 0, msg, msglen);
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);
    if(0 == ret)
    {
        MQTTMSG_FMT_DEBUG("Send msg, tpoic %s, msg:\n%s\n", topic, msg);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send msg failed ret %d, tpoic %s, msg:\n%s\n", ret, topic, msg);
    }

    return ret;
}

/**********************************************************************
* @name      : lc_appreglist_check
* @brief     ：app注册列表解析
* @param[in] ：MESSAGE_JSON_T *pInMsg    应答消息
* @param[out]：
* @return    ：FALSE-继续/TRUE-完成
* @Create    : 贺宁
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
uint8 lc_appreglist_check(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic)
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

