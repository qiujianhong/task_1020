/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级app用采mqtt消息处理
* @date：    2019-12-24
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "devinfo.h"
#include <net/if.h>
#include "oopType.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "task_id.h"
#include "mqtt_client.h"
#include "oopVarUpdate.h"
#include "msg_base.h"
#include "out_msg.h"
#include "EventFun.h"

#define APP_SUBBOARD "+/Broadcast/#"
extern EVENT_31XX_INFO event_3327;

uint8           g_AdvAppID[32] = {0};
uint8           g_AdvAppIDLen = 24;

// 对外消息 客户端句柄
MQTT_CLIENT_T *g_AdvMqttClient;

pthread_mutex_t   g_MsgSendlock; 
pthread_mutex_t   g_MsgIndexlock; 
uint16            g_AppMsgindex = 0;

DB_AGENT          g_dbAgent = 0;
extern DB_CLIENT  g_dbClien;

uint8      g_AppName[APPMSG_NAME_LEN] = {0};
APP_INFO_T g_APPdbCenter;  //数据中心
APP_INFO_T g_APPWireless;  //无线拨号
APP_INFO_T g_APPMap;       //模组管理器
APP_INFO_T g_APPCTS;       //采集任务调度
APP_INFO_T g_APPDefend;    //安全防护
APP_INFO_T g_APPProxy;     //通信代理
APP_INFO_T g_APPGUI;       //显示
APP_INFO_T g_APPRsp;       //遥信脉冲管理


/**********************************************************************
* @name      : advance_get_appid
* @brief     ：获取APPID
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-19
* @Update    : 
**********************************************************************/
void advance_get_appid()
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        YCMSG_FMT_DEBUG("devinfo hal_device_get error \n");
    }
    else
    {
        g_AdvAppIDLen = 24;
        ret = dev->devinfo_read_appid(dev, ADV_APP_NAME,(char*)g_AdvAppID, 32);
        if(ERR_OK != ret)
        {
            YCMSG_FMT_DEBUG("devinfo devinfo_read_appid error ret %d\n", ret);
        }
        else
        {
            YCMSG_BUF_DEBUG(g_AdvAppID, g_AdvAppIDLen, "APPID\n");
        }
        hal_device_release((HW_DEVICE *)dev);
    }
}

/**********************************************************************
* @name      : advance_next_index_init
* @brief     ：对外消息index管理初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
void advance_next_index_init()
{
    pthread_mutex_init(&g_MsgIndexlock, NULL);
    g_AppMsgindex = 0;
}

/**********************************************************************
* @name      : advance_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
uint16 advance_get_next_index()
{
    uint16 curindex = 0;

    PTHREAD_LOCK_SAFE(&g_MsgIndexlock);
    curindex = g_AppMsgindex;
    g_AppMsgindex++;
    PTHREAD_UNLOCK_SAFE(&g_MsgIndexlock);

    return curindex;
}

/**********************************************************************
* @name      : advance_outmsg_init
* @brief     ：对外消息初始化
* @param[in] ：uint8 type   通信方式 0-UDP/1-TCP/2-MQTT
               uint32 ip
               uint16 port
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int advance_outmsg_init(uint8 type, uint32 ip, uint16 port)
{
    int ret = 0;
    uint8 status = 0;
    char   mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[2];
    char  mqttstatus[2][16] = {"connectlost", "success"};

    /* 新建客户端 */
    sprintf((char *)g_AppName, "M-%s", ADV_APP_NAME);
    g_AdvMqttClient = mqtt_client_create((char *)g_AppName);
    if (NULL == g_AdvMqttClient)
    {
        YCMSG_FMT_DEBUG("mqtt_client_create fail\n");
        return -1;
    }
    
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, APP_SUBTOPIC, strlen(APP_SUBTOPIC));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, APP_SUBBOARD, strlen(APP_SUBBOARD));
    ret = mqtt_client_set_sub_topic(g_AdvMqttClient, subtopic, 2);
    if(0 != ret)
    {
        YCMSG_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(g_AdvMqttClient, APP_DEFAULT_TOPIC, 0);
    if(0 != ret)
    {
        YCMSG_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 增加预处理处理函数 */
    ret = mqtt_msg_add_pre_porc(g_AdvMqttClient, advance_mqtt_reproc);
    if(0 != ret)
    {
        YCMSG_FMT_DEBUG("mqtt_msg_add_pre_porc fail, ret %d\n", ret);
        goto errorend;
    }

    /* 链接服务端 */
    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    YCMSG_FMT_DEBUG("mqtt connect %s:%d\n", mqttaddr, port);
    ret = mqtt_client_connect(g_AdvMqttClient, mqttaddr, port, NULL);
    if(0 != ret)
    {
        YCMSG_FMT_DEBUG("mqtt_client_connect fail, ret %d", ret);
        goto errorend;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(g_AdvMqttClient);
        YCMSG_FMT_DEBUG("Get mqtt connect status %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    YCMSG_FMT_DEBUG("mqtt connect %s:%d success\n", mqttaddr, port);

    /* 创建db_agent */
    g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_YC_AGENT, advance_send_dbmsg, ADVANCE_DB_CLIENT_NUM);
    {
        if(g_dbAgent < 0)
        {
            YCMSG_FMT_DEBUG("db_agent_create failed!\n");
            mqtt_client_disconnect(g_AdvMqttClient);
            ret = -1;
            goto errorend;
        }
    }

    advance_next_index_init();
    pthread_mutex_init(&g_MsgSendlock, NULL);
    memset(&g_APPdbCenter , 0, sizeof(APP_INFO_T));
    memset(&g_APPWireless , 0, sizeof(APP_INFO_T));
    memset(&g_APPMap , 0, sizeof(APP_INFO_T));
    memset(&g_APPCTS , 0, sizeof(APP_INFO_T));
    memset(&g_APPDefend , 0, sizeof(APP_INFO_T));
    memset(&g_APPGUI , 0, sizeof(APP_INFO_T));

    advance_mqtt_debug();

    advance_get_appid();
    return 0;

errorend:
    mqtt_client_destroy(g_AdvMqttClient);
    return ret;
}

/**********************************************************************
* @name      : advance_get_dbagent
* @brief     ：获取dbagent句柄
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
DB_AGENT advance_get_dbagent()
{
    return g_dbAgent;
}

/**********************************************************************
* @name      : advance_outmsg_uninit
* @brief     ：对外通信客户端去初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
void advance_outmsg_uninit()
{
    mqtt_client_disconnect(g_AdvMqttClient);
    mqtt_client_destroy(g_AdvMqttClient);

    db_agent_close(g_dbAgent);
    g_dbAgent = 0;
    pthread_mutex_lock(&g_MsgSendlock);
    pthread_mutex_destroy(&g_MsgSendlock);
    pthread_mutex_lock(&g_MsgIndexlock);
    pthread_mutex_destroy(&g_MsgIndexlock);
}

/**********************************************************************
* @name      : advance_send_msg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int advance_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    msglen = appmsg_package(pMsgInfo, &pMsg);
    if(0 == msglen)
    {
        YCMSG_FMT_DEBUG("App send appmsg_package fail, IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        return -1;
    }

    if(msglen >= APP_RECV_BUF_LEN)
    {
        YCMSG_FMT_DEBUG("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    if(NULL != pTopic)
    {
        ret = mqtt_client_send_msg(g_AdvMqttClient, pTopic, qos, pMsg, (int)msglen);
        YCMSG_FMT_TRACE("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);

    }
    else
    {
        ret = mqtt_client_send(g_AdvMqttClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);


    if(0 == ret)
    {
        YCMSG_BUF_TRACE(pMsg, msglen, "Send msg:");
    }
    else
    {
        YCMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                        pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : advance_send_dbmsg
* @brief     ：高级app对外发送数据中心接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：uint16*         index
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int advance_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPdbCenter.Online)
    {
        YCMSG_FMT_TRACE("No dbCenter!\n");
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

    *pIndex = advance_get_next_index();
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
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_ctsmsg
* @brief     ：高级app对外发送采集任务调度消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int advance_send_ctsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPCTS.Online)
    {
        YCMSG_FMT_TRACE("No CTS!\n");
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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPCTS, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_CTS;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, g_APPCTS.commType, g_APPCTS.name);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_wirelessmsg
* @brief     ：高级app对外发送无线拨号app消息接口
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
int advance_send_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPWireless.Online)
    {
        YCMSG_FMT_TRACE("No Wireless!\n");
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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPWireless, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_WIRELESS;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, g_APPWireless.commType, g_APPWireless.name);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_commmsg
* @brief     ：高级app对外发送通用消息接口
* @param[in] ：char* 		   destaddr
			   uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int advance_send_commmsg(char* destaddr, uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);
	pMsgInfo->destlen = strlen(destaddr);
	memcpy(pMsgInfo->destaddr, destaddr, pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_COMMON;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%s", g_AppName, destaddr);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_modmsg
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
int advance_send_modmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPMap.Online)
    {
        YCMSG_FMT_TRACE("No mapManager!\n");
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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPMap, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_MODULE;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    sprintf(uTopic, "%s/%c-%s", g_AppName, g_APPMap.commType, g_APPMap.name);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_scsmsg
* @brief     ：高级app对外消息SCSApp接口，非必要APP
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
int advance_send_scsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    APP_INFO_T appProxy;

    MEMZERO(&appProxy, sizeof(APP_INFO_T));

    appProxy.commType = 'M';
    strcpy(appProxy.name, "scsMonitor");

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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&appProxy, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_SCS;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, appProxy.commType, appProxy.name);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_proxymsg
* @brief     ：高级app发往通信代理APP消息接口，非必要APP
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
int advance_send_proxymsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint8 response)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    APP_INFO_T appProxy;

    MEMZERO(&appProxy, sizeof(APP_INFO_T));

    appProxy.commType = 'M';
    strcpy(appProxy.name, "commAgent");
        
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        YCMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);

    pMsgInfo->rpm = (response == 1)? 0:1; 
    pMsgInfo->priority = 3;

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&appProxy, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_PROXY;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, appProxy.commType, appProxy.name);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_iotManager
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iip
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int advance_send_iotManager(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
	char            uTopic[256] = {0};
    char           *iotname = "M-iotManager";

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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen(iotname);
    memcpy(pMsgInfo->destaddr, iotname, pMsgInfo->destlen);

    pMsgInfo->IID = iip;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }

	sprintf(uTopic, "%s/%s", g_AppName, iotname);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}


/**********************************************************************
* @name      : advance_send_smios
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iip
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int advance_send_smios(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char           *smios = "M-smiOS";

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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->IID = iip;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    ret = advance_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_acmeter
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iip
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int advance_send_acmeter(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char           *acmeter = "M-acMeter";
    char            uTopic[256] = {0};

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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen(acmeter);
    memcpy(pMsgInfo->destaddr, acmeter, pMsgInfo->destlen);

    pMsgInfo->IID = iip;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    sprintf(uTopic, "%s/%s", g_AppName, acmeter);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_taskManager
* @brief     ：高级app对外采集任务APP同步消息接口
* @param[in] ：uint16          label
               uint16          iip
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int advance_send_taskManager(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char           *taskManager = "M-taskManager";
    char            uTopic[256] = {0};
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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen(taskManager);
    memcpy(pMsgInfo->destaddr, taskManager, pMsgInfo->destlen);

    pMsgInfo->IID = iip;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    sprintf(uTopic, "%s/%s", g_AppName, taskManager);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_ccomsg
* @brief     ：高级app对本地APP同步消息接口
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
int advance_send_ccomsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char           *ccoRouter = "M-ccoRouter";
    char            uTopic[256] = {0};
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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen(ccoRouter);
    memcpy(pMsgInfo->destaddr, ccoRouter, pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_CCOROUTER;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    sprintf(uTopic, "%s/%s", g_AppName, ccoRouter);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_pdsManager
* @brief     ：高级app对台区状体管APP同步消息接口
* @param[in] ：uint16          label
               uint16          iip
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int advance_send_pdsManager(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    APP_INFO_T appProxy;

    MEMZERO(&appProxy, sizeof(APP_INFO_T));

    appProxy.commType = 'M';
    strcpy(appProxy.name, "pdsManager");

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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&appProxy, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_PDS;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, appProxy.commType, appProxy.name);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}


/**********************************************************************
* @name      : advance_send_guimsg
* @brief     ：高级app对外发送显示接口消息
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-27
* @Update    :
**********************************************************************/
int advance_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPGUI.Online)
    {
        YCMSG_FMT_TRACE("No GUI!\n");
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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPGUI, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_DESKGUI;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, g_APPGUI.commType, g_APPGUI.name);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_uartmsg
* @brief     ：高级app对外发送串口APP接口消息
* @param[in] ：uint16          label
               uint16          iid
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-30
* @Update    :
**********************************************************************/
int advance_send_uartmsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        YCMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);
#ifdef PRODUCT_SCU
    pMsgInfo->destlen = strlen("uartManager");
    memcpy(pMsgInfo->destaddr, "uartManager", pMsgInfo->destlen);
#else
    pMsgInfo->destlen = strlen("M-uartManager");
    memcpy(pMsgInfo->destaddr, "M-uartManager", pMsgInfo->destlen);
#endif

    pMsgInfo->IID = iid;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
#ifdef PRODUCT_SCU
    sprintf(uTopic, "%s/uartManager/%s", g_AppName, MQTT_TOPIC_A_XDR);
#else
    sprintf(uTopic, "%s/M-uartManager", g_AppName);
#endif
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_btmsg
* @brief     ：高级app对外发送蓝牙APP接口消息
* @param[in] ：uint16          label
               uint16          iid
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-2
* @Update    :
**********************************************************************/
int advance_send_btmsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        YCMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);
#ifdef PRODUCT_SCU
    pMsgInfo->destlen = strlen("btManager");
    memcpy(pMsgInfo->destaddr, "btManager", pMsgInfo->destlen);
#else
    pMsgInfo->destlen = strlen("M-btManager");
    memcpy(pMsgInfo->destaddr, "M-btManager", pMsgInfo->destlen);
#endif

    pMsgInfo->IID = iid;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
#ifdef PRODUCT_SCU
    sprintf(uTopic, "%s/btManager/%s", g_AppName, MQTT_TOPIC_A_XDR);
#else
    sprintf(uTopic, "%s/M-btManager", g_AppName);
#endif
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_radiomsg
* @brief     ：高级app对外发送电台APP接口消息
* @param[in] ：uint16          label
               uint16          iid
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-2
* @Update    :
**********************************************************************/
int advance_send_radiomsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        YCMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);
#ifdef PRODUCT_SCU
    pMsgInfo->destlen = strlen("radioManager");
    memcpy(pMsgInfo->destaddr, "radioManager", pMsgInfo->destlen);
#else
    pMsgInfo->destlen = strlen("M-radioManager");
    memcpy(pMsgInfo->destaddr, "M-radioManager", pMsgInfo->destlen);
#endif

    pMsgInfo->IID = iid;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
#ifdef PRODUCT_SCU
    sprintf(uTopic, "%s/radioManager/%s", g_AppName, MQTT_TOPIC_A_XDR);
#else
    sprintf(uTopic, "%s/M-radioManager", g_AppName);
#endif
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_scs
* @brief     ：高级app对外发送接口消息
* @param[in] ：uint16          label
               uint16          iid
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-2
* @Update    :
**********************************************************************/
int advance_send_scs(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        YCMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);
#ifdef PRODUCT_SCU
    pMsgInfo->destlen = strlen("scsMonitor");
    memcpy(pMsgInfo->destaddr, "scsMonitor", pMsgInfo->destlen);
#else
    pMsgInfo->destlen = strlen("M-scsMonitor");
    memcpy(pMsgInfo->destaddr, "M-scsMonitor", pMsgInfo->destlen);
#endif

    pMsgInfo->IID = iid;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
#ifdef PRODUCT_SCU
    sprintf(uTopic, "%s/scsMonitor/%s", g_AppName, MQTT_TOPIC_A_XDR);
#else
    sprintf(uTopic, "%s/M-scsMonitor", g_AppName);
#endif
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_send_rspmsg
* @brief     ：高级app对外发送遥信脉冲接口消息
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
int advance_send_rspmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPRsp.Online)
    {
        YCMSG_FMT_TRACE("No RSP!\n");
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

    pMsgInfo->index = advance_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPRsp, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_RSPSAMPLE;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, g_APPRsp.commType, g_APPRsp.name);
    ret = advance_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}


/**********************************************************************
* @name      : advance_send_appmsg
* @brief     ：高级app对其他消息接口
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  消息内容
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int advance_send_appmsg(MESSAGE_INFO_T *pMsgInfo)
{
    char            uTopic[256] = {0};

    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    sprintf(uTopic, "%s/%s", g_AppName, pMsgInfo->destaddr);
    return advance_send_msg(uTopic, pMsgInfo, 0);
}

/**********************************************************************
* @name      : advance_login_to_smios
* @brief     ：APP注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int advance_login_to_smios()
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 request[240] = {0};
    uint8 offset = 0;
    uint8 len = 0;
//    uint32 subevent[8] = {0x0000000e, 0x00010006, 0x00020002, 0x00040002, 0x00050006, 
//                          0x0006007e, 0x00070006, 0x00080002};
    //uint32 version = APP_VERSION;

    pid_t pid = getpid();
    Date_Time_S tTime;
    char *smios = "M-smiOS";

    app_get_maketime(APP_PUBLISH_TIME, &tTime);

    //app_get_publish_time(__DATE__, __TIME__, &tTime);

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
//    appmsg_memcpy_r(request + offset, &subevent[0], sizeof(uint32));
//    offset += sizeof(uint32);
//    appmsg_memcpy_r(request + offset, &subevent[1], sizeof(uint32));
//    offset += sizeof(uint32);
//    appmsg_memcpy_r(request + offset, &subevent[2], sizeof(uint32));
//    offset += sizeof(uint32);
//    appmsg_memcpy_r(request + offset, &subevent[3], sizeof(uint32));
//    offset += sizeof(uint32);
//    appmsg_memcpy_r(request + offset, &subevent[4], sizeof(uint32));
//    offset += sizeof(uint32);
//    appmsg_memcpy_r(request + offset, &subevent[5], sizeof(uint32));
//    offset += sizeof(uint32);
//    appmsg_memcpy_r(request + offset, &subevent[6], sizeof(uint32));
//    offset += sizeof(uint32);
//    appmsg_memcpy_r(request + offset, &subevent[7], sizeof(uint32));
//    offset += sizeof(uint32);

    /* 版本信息 */
    //appmsg_memcpy_r(request + offset, &version, sizeof(uint32));
    //offset += 4;
    request[offset++] = strlen(APP_VERSION);
    memcpy(request + offset,APP_VERSION,strlen(APP_VERSION));
    offset+=strlen(APP_VERSION);
    
    /* 发布日期 */
    request[offset++] = tTime.year_h;
    request[offset++] = tTime.year_l;
    request[offset++] = tTime.month;
    request[offset++] = tTime.day;
    request[offset++] = tTime.hour;
    request[offset++] = tTime.min;
    request[offset++] = tTime.sec;
    
    /* APPID */
    request[offset++] = g_AdvAppIDLen;
    if(g_AdvAppIDLen > 0)
    {
        memcpy(request + offset, g_AdvAppID, g_AdvAppIDLen);
    }
    offset += g_AdvAppIDLen;

    pMsgInfo = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + offset);
    if(NULL == pMsgInfo)
    {
        YCMSG_FMT_DEBUG("Login to os malloc failed %d\n", sizeof(MESSAGE_INFO_T) + offset);
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = advance_get_next_index();
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

    ret = advance_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_heart_bear
* @brief     ：心跳
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int advance_heart_bear(MESSAGE_INFO_T *pInMsg)
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
        YCMSG_FMT_DEBUG("Heart bear malloc failed %d\n", len);
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

    ret = advance_send_msg(NULL, pMsgInfo, 0);
    if(ret == 0)
    {
        YCMSG_FMT_DEBUG("=======advance send heart secc=======\n");
    }
    else
    {
        YCMSG_FMT_DEBUG("=======advance send heart err=======\n");
    }
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advance_get_version
* @brief     ：查询版本号
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-29
* @Update    :
**********************************************************************/
int advance_get_version(MESSAGE_INFO_T *pInMsg)
{
    char         uTopic[256] = {0};
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    uint8              *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;
    //uint32           version = APP_VERSION;
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

    /* 版本信息 */
    //appmsg_memcpy_r(temp + msglen, &version, sizeof(uint32));
   // msglen += sizeof(uint32);

    temp[msglen++] = strlen(APP_VERSION);
    memcpy(temp + msglen,APP_VERSION,strlen(APP_VERSION));
    msglen+=strlen(APP_VERSION);

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
    return advance_send_msg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : advance_get_appidmsg
* @brief     ：查询appid
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-29
* @Update    :
**********************************************************************/
int advance_get_appidmsg(MESSAGE_INFO_T *pInMsg)
{
    char         uTopic[256] = {0};
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
    *temp = g_AdvAppIDLen;
    if(g_AdvAppIDLen > 0)
    {
        memcpy(temp + 1, g_AdvAppID, g_AdvAppIDLen);
    }
    ptMsgNew->msglen = g_AdvAppIDLen + 1;

    sprintf(uTopic, "%s/%s", g_AppName, pInMsg->souraddr);
    return advance_send_msg(uTopic, ptMsgNew, 0);
}


/**********************************************************************
* @name      : advance_seek_app
* @brief     ：发送APP查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int advance_seek_app()
{
    MESSAGE_INFO_T  tMsgInfo;
    int             ret = 0;
    char           *smios = "M-smiOS";
    
    memset(&tMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    tMsgInfo.rpm = 1;
    tMsgInfo.priority = 2;

    tMsgInfo.index = advance_get_next_index();
    tMsgInfo.label = 0;
    
    tMsgInfo.sourlen = strlen((char *)g_AppName);
    memcpy(tMsgInfo.souraddr , g_AppName, tMsgInfo.sourlen);
    tMsgInfo.destlen = strlen(smios);
    memcpy(tMsgInfo.destaddr, smios, tMsgInfo.destlen);

    tMsgInfo.IID = MESSAGE_IID_SYSTEM;
    tMsgInfo.IOP = SYSTEM_IOP_GET_REGISTERAPP;
    tMsgInfo.msglen = 0;

    ret = advance_send_msg(NULL, &tMsgInfo, 0);
    return ret;
}

/*
CPU		   				（0）， 
内存		  				（1），
存储		   				（2），
主板温度	   				（3），
CPU 利用率超过阀值        	（4）， 
内存使用率超过阀值        	（5），
磁盘空间使用率超过阀值    	（6），
容器异常重启              	（7），
容器故障                  	（8），
应用异常重启              	（9），
应用升级                  	 (10), 
无效签名证书               	(12),
无签名证书                	 (13)
安装                   		（14），
升级                   		（15），
卸载                   		（16），
创建                   		（17），
启动                   		（18），
停止                   		（19），
APP注册码错误          		（20）

*/
uint8 get_warrn_type(uint8 type,uint8 noticetype,uint8 devtype)
{
    uint8 result = 0xff;
    if(noticetype == 0)
    {   
        switch(type)
        {
            case 0:
            {
                if(noticetype==0)
                {
                    result = 0;
                }
                else if(noticetype== 1 || noticetype == 2)
                {
                    result = 4;
                }
            }break;
            case 1://内存
            {
                if(noticetype==0)
                {
                    result = 1;
                }
                else if(noticetype== 1 || noticetype == 2)
                {
                    result = 5;
                }
            }break;
            case 2://磁盘
            {
                if(noticetype==0)
                {
                    result = 2;
                }
                else if(noticetype== 1 || noticetype == 2)
                {
                    result = 6;
                }
            }break;
            case 3://温度
            {
                if(noticetype== 1 || noticetype == 2)
                {
                    result = 3;
                }
            }break;
            case 4://异常
            {

            }break;
        }
    }else if(devtype == 1)
    {
        switch(type)
        {
            case 0://cpu
            {
                if(noticetype==0)
                {
                    result = 0;
                }
                else if(noticetype== 1 || noticetype == 2)
                {
                    result = 4;
                }
            }break;
            case 1://内存
            {
                if(noticetype==0)
                {
                    result = 1;
                }
                else if(noticetype== 1 || noticetype == 2)
                {
                    result = 5;
                }
            }break;
            case 2://磁盘
            {
                if(noticetype==0)
                {
                    result = 2;
                }
                else if(noticetype== 1 || noticetype == 2)
                {
                    result = 6;
                }
            }break;
            case 3://重启
            {
                if(noticetype==0)
                {
                    result = 7;
                }
            }break;
            case 4://异常
            {
                if(noticetype==0)
                {
                    result = 8;
                }
            }break;
                
        }
    }else
    {
        switch(type)
        {
            case 0://cpu
            {
                if(noticetype==0)
                {
                    result = 0;
                }
                else if(noticetype== 1 || noticetype == 2)
                {
                    result = 4;
                }
            }break;
            case 1://内存
            {
                if(noticetype==0)
                {
                    result = 1;
                }
                else if(noticetype== 1 || noticetype == 2)
                {
                    result = 5;
                }
            }break;
            case 3://重启
            {
                if(noticetype==0)
                {
                    result = 9;
                }
            }break;
            case 4://异常
            {
                
            }break;
                
        }

    }

    return result;
}


uint8 get_warrn_exec_type(uint8 type,uint8 devtype)
{
    uint8 result = 0xff;
    if(devtype == 0)
    {
        result = 15;
    }else if(devtype == 1)
    {
        switch(type)
        {
            case 0://安装
            {
                result = 14;
            }break;
            case 1://升级
            {
                result = 15;
            }break;
            case 2://卸载
            {
                result = 17;
            }break;
            case 3://启动
            {
                result = 18;
            }break;
            case 4://停止
            {
                result = 19;
            }break;
            case 5://配置参数
            {
                result = 8;
            }break;
                
        }
    }else
    {
        switch(type)
        {
            case 0://安装
            {
                result = 14;
            }break;
            case 1://升级
            {
                result = 15;
            }break;
            case 2://卸载
            {
                result = 17;
            }break;
            case 3://启动
            {
                result = 18;
            }break;
            case 4://停止
            {
                result = 19;
            }break;
            case 5://配置参数
            {
                result = 8;
            }break;
            case 6:
            {
                
            }break;
            case 7:
            {

            }break;
            case 8:
            {
                result = 20;
            }break;
        }
    }

    return result;
}

int write_3327_toDB(OOP_MONITORLOG_T *ptmp)
{
    time_t              timep = 0;
    WRITE_RECORD_T      recInData;   
    
    memset(&recInData,0,sizeof(WRITE_RECORD_T));
    time(&timep); /*获取time_t类型的当前时间*/
    recInData.recordOAD.classtag = CLASS_DATA_EVENT;
    recInData.recordOAD.road.oadCols.nNum = 1;
    recInData.recordOAD.road.oadCols.oad[0].value = 0x33270200;
//    recInData.MAC = MAC;
    recInData.colStartTm = timep + COMPENSATION_TIME;
    recInData.colEndTm = timep + COMPENSATION_TIME;
    recInData.colStoreTm = ptmp->nIndex+1;

    recInData.inDatalen = sizeof(OOP_MONITORLOG_T);
    memcpy(recInData.inDataBuf, ptmp, sizeof(OOP_MONITORLOG_T));
    return db_write_record(g_dbClien, &recInData, &event_3327.eventCommInfo.storagenum);
}

void Generates_event_in_docker(MESSAGE_INFO_T *pInMsg,uint8 Dtype)
{
    OOP_VISIBLEVAR_T    dockername;
    OOP_VISIBLEVAR_T    appName;
    OOP_DATETIME_S_T    strarttime;
    uint8 *data = pInMsg->playload;
    uint32 msglen = pInMsg->msglen,index=0;
    uint8 type=0;
    uint8 result=0,noticetype = 0;
    memset(&dockername,0,sizeof(dockername));
    memset(&strarttime,0,sizeof(strarttime));
    memset(&appName,0,sizeof(appName));

    /*解帧*/
    /*
    容器名称	visible-string，
    发生时间  date_time_s,
    通知类别 	enum{安装（0），升级（1），卸载（2），启动（4），停止（4），配置参数（5）}，
    结果 enum{成功（0），失败（1）}，
    失败原因 visible-string OPTIONAL
    */
    if(Dtype == 0)
    {

    }else if(Dtype == 1 || Dtype == 2)
    {
        dockername.nNum = data[index++];
        memcpy(dockername.value,&data[index],dockername.nNum);
        index+=dockername.nNum;
    }else if(Dtype == 2)
    {
        appName.nNum = data[index++];
        memcpy(appName.value,&data[index],appName.nNum);
        index+=appName.nNum; 
    }
    memcpy(&strarttime.year,&data[index],sizeof(uint16));
    index+=sizeof(uint16);
    strarttime.month = data[index++];
    strarttime.day = data[index++];
    strarttime.hour = data[index++];
    strarttime.minute = data[index++];
    strarttime.second = data[index++];
    if(pInMsg->IOP == IOT_IOP_DOCKER_ACTION || pInMsg->IOP == IOT_IOP_APP_ACTION || pInMsg->IOP == IOT_IOP_SYS_WARRING)
    {
        type = data[index++];
        result = data[index++];
        if(result == 0)//只记录失败的
        return;
    }
    else if(pInMsg->IOP == IOT_IOP_DOCKER_WARRING || pInMsg->IOP == IOT_IOP_APP_WARRING || pInMsg->IOP == IOT_IOP_SYS_MOVE_UP)
    {
        type = data[index++];
        noticetype = data[index++];
    }
    uint8 option = data[index++];
    if(option == 1)
    {
        OOP_VISIBLEVAR256_T failreason;
        memset(&failreason,0,sizeof(failreason));
        failreason.nNum = data[index++];
        memcpy(&failreason.value,&data[index],failreason.nNum);
    }
    else if(option != 0)
    {
        /*消息错误*/
        YCMSG_FMT_DEBUG("advance recv iotManager failed,option [%d]\n",option);
        return;
    }
    if(index!=msglen)
    {
        YCMSG_FMT_DEBUG("advance recv iotManager failed,index [%d] msglen [%d]\n",index,msglen);
        return;
    }
    /*填充事件结构体*/
    event_3327.eventCommInfo.recordnum++;
    OOP_MONITORLOG_T oop_3327;
    memset(&oop_3327,0,sizeof(oop_3327));
    oop_3327.nIndex = event_3327.eventCommInfo.recordnum;
    oop_3327.tmStart = strarttime;
    oop_3327.tmEnd   = strarttime;
    oop_3327.source.choice = 22;
    oop_3327.source.nValue = Dtype;
    if(pInMsg->IOP == IOT_IOP_DOCKER_ACTION || pInMsg->IOP == IOT_IOP_APP_ACTION || pInMsg->IOP == IOT_IOP_SYS_WARRING)
    {
        oop_3327.warn = get_warrn_exec_type(type,Dtype);
    }else if(pInMsg->IOP == IOT_IOP_DOCKER_WARRING || pInMsg->IOP == IOT_IOP_APP_WARRING || pInMsg->IOP == IOT_IOP_SYS_MOVE_UP)
    {
        oop_3327.warn = get_warrn_type(type,noticetype,Dtype);
    }
    oop_3327.value = 0;
    oop_3327.rate = 0;
    if(Dtype == 1 || Dtype == 2)
    {
        oop_3327.containerName = dockername;
    }  
    if(Dtype == 2 )
    {
        oop_3327.appName = appName;
    }
    write_3327_toDB(&oop_3327);
        

}

/**********************************************************************
* @name      : advance_recv_regapp
* @brief     ：收到iotmanager消息
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 温占礼
* @Date      ：2022-7-6
* @Update    :
**********************************************************************/
BOOLEAN advance_recv_iotManager(MESSAGE_INFO_T *pInMsg)
{
    switch (pInMsg->IOP)
        {
            case IOT_IOP_DOCKER_ACTION:
            case IOT_IOP_DOCKER_WARRING:
            {
                Generates_event_in_docker(pInMsg,1);
            }break;
            case IOT_IOP_APP_ACTION:
            case IOT_IOP_APP_WARRING:
            {
                Generates_event_in_docker(pInMsg,2);
            }break;
            case IOT_IOP_SYS_MOVE_UP:
            case IOT_IOP_SYS_WARRING:
            {
                Generates_event_in_docker(pInMsg,0);
            }break;
            case IOT_IOP_DOCKER_INSTALL:
            {

            }break;
            case IOT_IOP_DOCKER_ACTION1:
            {

            }break;
            case IOT_IOP_DOCKER_SET:
            {

            }break;
            case IOT_IOP_DOCKER_FIND:
            {

            }break;
            case IOT_IOP_APP_INSTALL:
            {

            }break;
            case IOT_IOP_APP_ACTION1:
            {

            }break;
            case IOT_IOP_APP_SET:
            {

            }break;
            case IOT_IOP_APP_FIND:
            {

            }break;
            case IOT_IOP_APP_LICENSE_SET:
            {

            }break;
            case IOT_IOP_APP_LICENSE_FIND:
            {

            }break;
            case IOT_IOP_SYS_MOVE_UP1:
            {

            }break;
            case IOT_IOP_SYS_VERSION_FIND:
            {

            }break;
            case IOT_IOP_LOG_INFO_FIND:
            {

            }break;
            case IOT_IOP_LOG_PACK:
            {
                
            }break;
            case IOT_IOP_LOG_DELECT:
            {

            }break;
            case IOT_IOP_SYS_INSPECT:
            {

            }break;
        }
    return FALSE;
}


/**********************************************************************
* @name      : advance_recv_regapp
* @brief     ：收到注册APP查询
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
BOOLEAN advance_recv_regapp(MESSAGE_INFO_T *pInMsg)
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
    //uint32 appVerInfo = 0;
    uint8 versionlen = 0;
    uint8 version[64]={0};
    Date_Time_S dateTimeS;
    uint8  appIdLen = 0;
    uint8  appId[128] = {0};
    
    if(NULL == pInMsg)
    {
        return FALSE;
    }

    pTemp = pInMsg->playload;
    appnun = appmsg_get_len_offset(pTemp, &bytes);
    pTemp += bytes;

    if (appnun > APP_NUM_MAX)
    {
        YCMSG_FMT_DEBUG("advance recv regapp failed appnun[%d]\n", appnun);
        return FALSE;
    }

    oopRegAppInfoList.appNum = appnun;

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
                YCMSG_FMT_DEBUG("======get dbcenter======\n");
            }
            else if(0 == strcmp("wirelessDCM", tApp.name) && FALSE == g_APPWireless.Online)
            {
                memcpy(&g_APPWireless, &tApp, sizeof(APP_INFO_T));
                g_APPWireless.Online = TRUE;
                printf("======get wirelessDCM======\n");
                YCMSG_FMT_DEBUG("======get wirelessDCM======\n");
            }
#ifdef PRODUCT_ECU
            else if(0 == strcmp("mapManager", tApp.name) && FALSE == g_APPMap.Online)
            {
                memcpy(&g_APPMap, &tApp, sizeof(APP_INFO_T));
                g_APPMap.Online = TRUE;
                printf("======get mapManager======\n");
                YCMSG_FMT_DEBUG("======get mapManager======\n");
            }
#endif
            else if(0 == strcmp("taskManager", tApp.name) && FALSE == g_APPCTS.Online)
            {
                memcpy(&g_APPCTS, &tApp, sizeof(APP_INFO_T));
                g_APPCTS.Online = TRUE;
                printf("======get taskManager======\n");
                YCMSG_FMT_DEBUG("======get taskManager======\n");
            }
            else if(0 == strcmp("scsMonitor", tApp.name) && FALSE == g_APPDefend.Online)
            {
                memcpy(&g_APPDefend, &tApp, sizeof(APP_INFO_T));
                g_APPDefend.Online = TRUE;
                printf("======get scsMonitor======\n");
                YCMSG_FMT_DEBUG("======get scsMonitor======\n");
            }
            else if(0 == strcmp("desktopGui", tApp.name) && FALSE == g_APPGUI.Online)
            {
                memcpy(&g_APPGUI, &tApp, sizeof(APP_INFO_T));
                g_APPGUI.Online = TRUE;
                printf("======get desktopGui======\n");
                YCMSG_FMT_DEBUG("======get desktopGui======\n");
            }
            else if(0 == strcmp("rspSample", tApp.name) && FALSE == g_APPRsp.Online)
            {
                memcpy(&g_APPRsp, &tApp, sizeof(APP_INFO_T));
                g_APPRsp.Online = TRUE;
                printf("======get rspSample======\n");
                YCMSG_FMT_DEBUG("======get rspSample======\n");
            }
        }

        offset += namelen;
        subnum = pTemp[offset++];
        offset += subnum * sizeof(uint32);  //订阅事件

       // memcpy(&appVerInfo, &pTemp[offset], sizeof(uint32)); //版本信息
       // offset += sizeof(uint32);

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

        oopRegAppInfoList.appInfo[i].appName.nNum = strlen(tApp.name);
        memcpy(oopRegAppInfoList.appInfo[i].appName.value, tApp.name, strlen(tApp.name));
        oopRegAppInfoList.appInfo[i].version.nNum = versionlen;
        memcpy(oopRegAppInfoList.appInfo[i].version.value,version,versionlen);
        oopRegAppInfoList.appInfo[i].dataTime.year = dateTimeS.year_l * 256 + dateTimeS.year_h;
        oopRegAppInfoList.appInfo[i].dataTime.month = dateTimeS.month;
        oopRegAppInfoList.appInfo[i].dataTime.day = dateTimeS.day;
        oopRegAppInfoList.appInfo[i].dataTime.hour = dateTimeS.hour;
        oopRegAppInfoList.appInfo[i].dataTime.minute = dateTimeS.min;
        oopRegAppInfoList.appInfo[i].dataTime.second = dateTimeS.sec;
        oopRegAppInfoList.appInfo[i].appID.nNum = appIdLen;
        memcpy(oopRegAppInfoList.appInfo[i].appID.value, appId, appIdLen);
    }

    if (regapp_callin_stat_get() == 1)
    {
        regapp_callin_stat_set(0);
    }

    if(TRUE == g_APPdbCenter.Online && TRUE == g_APPWireless.Online 
#ifdef PRODUCT_ECU
        && TRUE == g_APPMap.Online
#endif
        && TRUE == g_APPCTS.Online && TRUE == g_APPGUI.Online && TRUE == g_APPRsp.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advance_recv_msg
* @brief     ：对外消息接收, 使用了xl_channel_read 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int advance_recv_msg(uint8 *pBuf, uint16 buflen)
{
#if 1    
    char          topic[256];
    int           qos;
    return mqtt_client_recv_msg(g_AdvMqttClient, topic, 256, &qos, pBuf, (int)buflen, 1000);
#else
    return mqtt_client_recv(g_AdvMqttClient, pBuf, (int)buflen);
#endif
}

/**********************************************************************
* @name      : advance_start_goon
* @brief     ：判断启动是否可以继续
* @param[in] ：
* @param[out]：
* @return    ：TRUE-可以继续启动/FALSE-等待
* @Create    : 王津剑
* @Date      ：2020-1-10
* @Update    :
**********************************************************************/
BOOLEAN advance_start_goon()
{
    if(TRUE == g_APPdbCenter.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advance_mqtt_guiok
* @brief     ：判断是否获取GUI
* @param[in] ：
* @param[out]：
* @return    ：TRUE-可以继续启动/FALSE-等待
* @Create    : 王津剑
* @Date      ：2020-5-15
* @Update    :
**********************************************************************/
BOOLEAN advance_mqtt_guiok()
{
    if(TRUE == g_APPGUI.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advance_mqtt_handle
* @brief     ：获取mqtt句柄
* @param[in] ：
* @param[out]：
* @return    ：句柄
* @Create    : 王津剑
* @Date      ：2020-5-15
* @Update    :
**********************************************************************/
void *advance_mqtt_handle()
{
    return g_AdvMqttClient;
}

/**********************************************************************
* @name      : advance_mqtt_rspok
* @brief     ：判断是否获取遥信脉冲
* @param[in] ：
* @param[out]：
* @return    ：TRUE-可以继续启动/FALSE-等待
* @Create    : 王津剑
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
BOOLEAN advance_mqtt_rspok()
{
    if(TRUE == g_APPRsp.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advance_mqtt_mapok
* @brief     ：判断是否获取模组管理器
* @param[in] ：
* @param[out]：
* @return    ：TRUE-可以继续启动/FALSE-等待
* @Create    : 王津剑
* @Date      ：2020-7-17
* @Update    :
**********************************************************************/
BOOLEAN advance_mqtt_mapok()
{
    if(TRUE == g_APPMap.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advance_mqtt_debug
* @brief     ：mqtt调试信息打印
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-3
* @Update    :
**********************************************************************/
void advance_mqtt_debug()
{
    char   buflog[1024] = {0};

    if(0 != mqtt_msg_handle_debug(g_AdvMqttClient, buflog, 1024))
    {
        YCMSG_FMT_DEBUG("Mqtt client debug info\n%s\n", buflog);
        printf("Mqtt client debug info\n%s\n", buflog);
    }
}

/**********************************************************************
* @name      : advance_mqtt_reproc
* @brief     ：mqtt预先处理(提前处理数据中心消息, 防止在队列处理过程中卡死)
* @param[in] ：
* @param[out]：
* @return    ：0-预处理完成;其他-不需要预处理
* @Create    : 王津剑
* @Date      ：2020-10-26
* @Update    :
**********************************************************************/
int advance_mqtt_reproc(void *pMsg, int msglen)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;

    appmsg_split((uint8*)pMsg, (uint16)msglen , &pMsgInfo);
    if(NULL == pMsgInfo)
    {
        return 1;
    }

    ret = db_agent_msg_entry(g_dbAgent, pMsgInfo);
    if(DB_AGENT_UNKONW_MSG == ret || DB_AGENT_DROP_MSG == ret)
    {
        /* 加锁失败放到队列中继续执行一次 */
        free(pMsgInfo);
        return -1;
    }
    YCMSG_BUF_TRACE(pMsg, msglen, "Recv msg from %s:", pMsgInfo->souraddr);
    if(0 != ret)
    {
        YCMSG_BUF_DEBUG(pMsg, msglen, "Drop msg from %s, ret %d:", pMsgInfo->souraddr, ret);
    }
    free(pMsgInfo);
    return 0;
}

