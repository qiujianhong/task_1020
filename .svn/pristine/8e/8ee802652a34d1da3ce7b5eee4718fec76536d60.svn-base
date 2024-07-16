/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   配网mqtt消息处理
* @date：    2019-12-24
* @history： 
*********************************************************************
*/
#include "hal.h"
#include "devinfo.h"

#include "baseCommon.h"
#include "oopType.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "task_id.h"
#include "mqtt_client.h"

#include "msg_base.h"
#include "mqtt_proc.h"

uint8           g_PdAppID[32] = {0};
uint8           g_PdAppIDLen = 24;
DB_AGENT        g_dbAgent = 0;

// 对外消息 客户端句柄
MQTT_CLIENT_T *g_AdvPdMqttClient;

pthread_mutex_t   g_MsgSendlock; 
pthread_mutex_t   g_MsgIndexlock; 
uint16            g_AppMsgindex = 0;

uint8      g_AppName[APPMSG_NAME_LEN] = {0};
APP_INFO_T g_APPdbCenter;  //数据中心
APP_INFO_T g_APPWireless;  //无线拨号
APP_INFO_T g_APPMod;       //模组管理器
APP_INFO_T g_APPCTS;       //采集任务调度
APP_INFO_T g_APPGUI;       //显示
APP_INFO_T g_APPRsp;       //显示

#define  PD_DB_CLIENT_NUM       120

/**********************************************************************
* @name      : advPd_get_appid
* @brief     ：获取APPID
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-19
* @Update    : 
**********************************************************************/
void advPd_get_appid()
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        PDMSG_FMT_DEBUG("devinfo hal_device_get error \n");
    }
    else
    {
        g_PdAppIDLen = 24;
        ret = dev->devinfo_read_appid(dev, ADVPD_APP_NAME, (char*)g_PdAppID, 32);
        if(ERR_OK != ret)
        {
            PDMSG_FMT_DEBUG("devinfo devinfo_read_appid error ret %d\n", ret);
        }
        else
        {
            PDMSG_BUF_DEBUG(g_PdAppID, g_PdAppIDLen, "APPID\n");
        }
        hal_device_release((HW_DEVICE *)dev);
    }
}

/**********************************************************************
* @name      : advPd_get_next_index
* @brief     ：对外消息index管理初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-4
* @Update    :
**********************************************************************/
void advPd_next_index_init()
{
    pthread_mutex_init(&g_MsgIndexlock, NULL);
    g_AppMsgindex = 0;
}

/**********************************************************************
* @name      : advPd_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 王津剑
* @Date      ：2020-3-4
* @Update    :
**********************************************************************/
uint16 advPd_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE(&g_MsgIndexlock);
    curindex = g_AppMsgindex;
    g_AppMsgindex++;
    PTHREAD_UNLOCK_SAFE(&g_MsgIndexlock);

    return curindex;
}

/**********************************************************************
* @name      : advPd_mqtt_init
* @brief     ：对外消息初始化
* @param[in] ：uint32 ip
               uint16 port
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-4
* @Update    :
**********************************************************************/
int advPd_mqtt_init(uint32 ip, uint16 port)
{
    int         ret = 0;
    uint8       status = 0;
    char        mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[2];
    char        mqttstatus[2][16] = {"connectlost", "success"};

    /* 新建客户端 */
    sprintf((char *)g_AppName, "M-%s", ADVPD_APP_NAME);
    g_AdvPdMqttClient = mqtt_client_create((char *)g_AppName);
    if (NULL == g_AdvPdMqttClient)
    {
        PDMSG_FMT_DEBUG("mqtt_client_create fail\n");
        return -1;
    }
    
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, g_AppName, strlen((char *)g_AppName));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, ADVPD_APP_SUBTOPIC, strlen(ADVPD_APP_SUBTOPIC));
    ret = mqtt_client_set_sub_topic(g_AdvPdMqttClient, subtopic, 2);
    if(0 != ret)
    {
        PDMSG_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(g_AdvPdMqttClient, "M-pdAmr/M-smiOS", 0);
    if(0 != ret)
    {
        PDMSG_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 增加预处理处理函数 */
    ret = mqtt_msg_add_pre_porc(g_AdvPdMqttClient, advPd_mqtt_reproc);
    if(0 != ret)
    {
        YCMSG_FMT_DEBUG("mqtt_msg_add_pre_porc fail, ret %d\n", ret);
        goto errorend;
    }

    /* 链接服务端 */
    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    PDMSG_FMT_DEBUG("mqtt connect %s:%d\n", mqttaddr, port);
    ret =  mqtt_client_connect(g_AdvPdMqttClient, mqttaddr, port, NULL);
    if(0 != ret)
    {
        PDMSG_FMT_DEBUG("mqtt_client_connect fail, ret %d\n", ret);
        goto errorend;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(g_AdvPdMqttClient);
        PDMSG_FMT_DEBUG("Get mqtt connect status %d\n",  mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    PDMSG_FMT_DEBUG("mqtt connect %s:%d success\n", mqttaddr, port);

    advPd_next_index_init();
    pthread_mutex_init(&g_MsgSendlock, NULL);
    memset(&g_APPdbCenter , 0, sizeof(APP_INFO_T));
    memset(&g_APPWireless , 0, sizeof(APP_INFO_T));
    memset(&g_APPMod , 0, sizeof(APP_INFO_T));
    memset(&g_APPCTS , 0, sizeof(APP_INFO_T));

    advPd_get_appid();

    /* 创建db_agent */
    g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_PD_AGENT, advPd_send_dbmsg, PD_DB_CLIENT_NUM);
    {
        if(g_dbAgent < 0)
        {
            ret = -100;
            printf("db_agent_create error!\n");
            goto errorend;
        }
    }

    return 0;

errorend:
    mqtt_client_destroy(g_AdvPdMqttClient);
    return ret;

}

/**********************************************************************
* @name      : advPd_outmsg_uninit
* @brief     ：对外通信客户端去初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
void advPd_outmsg_uninit()
{
    mqtt_client_disconnect(g_AdvPdMqttClient);
    mqtt_client_destroy(g_AdvPdMqttClient);

    pthread_mutex_lock(&g_MsgSendlock);
    pthread_mutex_destroy(&g_MsgSendlock);
    pthread_mutex_lock(&g_MsgIndexlock);
    pthread_mutex_destroy(&g_MsgIndexlock);

    if(g_dbAgent > 0)
    {
        db_agent_close(g_dbAgent);
        g_dbAgent = 0;
    }
}

/**********************************************************************
* @name      : advPd_send_mqttmsg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
int advPd_send_mqttmsg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
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
        PDMSG_FMT_DEBUG("App send appmsg_package fail, IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        return -1;
    }

    if(msglen >= RECV_BUF_LEN)
    {
        PDMSG_FMT_DEBUG("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    if(NULL != pTopic)
    {
        ret = mqtt_client_send_msg(g_AdvPdMqttClient, pTopic, qos, pMsg, (int)msglen);
        PDMSG_FMT_TRACE("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);
    }
    else
    {
        ret = mqtt_client_send(g_AdvPdMqttClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);

    if(0 == ret)
    {
        PDMSG_BUF_TRACE(pMsg, msglen, "Send msg:");
    }
    else
    {
        PDMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                        pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}


/**********************************************************************
* @name      : advPd_send_dbmsg
* @brief     ：高级app对外发送数据中心接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：uint16*         index
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-4
* @Update    :
**********************************************************************/
int advPd_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPdbCenter.Online)
    {
        PDMSG_FMT_TRACE("No dbCenter!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        PDMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    *pIndex = advPd_get_next_index();
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
    ret = advPd_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advPd_send_ctsmsg
* @brief     ：高级app对外发送采集任务调度消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
int advPd_send_ctsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPCTS.Online)
    {
        PDMSG_FMT_TRACE("No CTS!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        PDMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = advPd_get_next_index();
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
    ret = advPd_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advPd_send_wirelessmsg
* @brief     ：高级app对外发送无线拨号app消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
int advPd_send_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPWireless.Online)
    {
        PDMSG_FMT_TRACE("No Wireless!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        PDMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = advPd_get_next_index();
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
    ret = advPd_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advPd_send_modmsg
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
int advPd_send_modmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPMod.Online)
    {
        PDMSG_FMT_TRACE("No mapManager!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        PDMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = advPd_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPMod, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_MODULE;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, g_APPMod.commType, g_APPMod.name);
    ret = advPd_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advPd_send_rspmsg
* @brief     ：高级app对外遥信脉冲消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-2
* @Update    :
**********************************************************************/
int advPd_send_rspmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPRsp.Online)
    {
        PDMSG_FMT_TRACE("No rsp!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        PDMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = advPd_get_next_index();
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
    ret = advPd_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advPd_send_smios
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-20
* @Update    :
**********************************************************************/
int advPd_send_smios(uint16 label, uint16 iop, uint8* playload, uint16 length)
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

    pMsgInfo->index = advPd_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    ret = advPd_send_mqttmsg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
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
int advPd_login_to_smios()
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 request[240] = {0};
    uint8 offset = 0;
    uint8 len = 0;
    uint32 subevent[6] = {0x00080002, 0x00050006, 0x00040002, 0x00020002, 0x00010002, 0x00070006};
    uint32 version = APP_VERSION;

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
    request[offset++] = 6;
    appmsg_memcpy_r(request + offset, &subevent[0], sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(request + offset, &subevent[1], sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(request + offset, &subevent[2], sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(request + offset, &subevent[3], sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(request + offset, &subevent[4], sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(request + offset, &subevent[5], sizeof(uint32));
        offset += sizeof(uint32);

    /* 版本信息 */
    appmsg_memcpy_r(request + offset, &version, sizeof(uint32));
    offset += 4;

    /* 发布日期 */
    request[offset++] = tTime.year_h;
    request[offset++] = tTime.year_l;
    request[offset++] = tTime.month;
    request[offset++] = tTime.day;
    request[offset++] = tTime.hour;
    request[offset++] = tTime.min;
    request[offset++] = tTime.sec;

    /* APPID */
    request[offset++] = g_PdAppIDLen;
    if(g_PdAppIDLen > 0)
    {
        memcpy(request + offset, g_PdAppID, g_PdAppIDLen);
    }
    offset += g_PdAppIDLen;
    
    pMsgInfo = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + offset);
    if(NULL == pMsgInfo)
    {
        PDMSG_FMT_DEBUG("Login to os malloc failed %d\n", sizeof(MESSAGE_INFO_T) + offset);
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = advPd_get_next_index();
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

    ret = advPd_send_mqttmsg(NULL, pMsgInfo, 0);
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
int advPd_heart_bear(MESSAGE_INFO_T *pInMsg)
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
        PDMSG_FMT_DEBUG("Heart bear malloc failed %d\n", len);
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

    ret = advPd_send_mqttmsg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : advPd_get_version
* @brief     ：查询版本号
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-29
* @Update    :
**********************************************************************/
int advPd_get_version(MESSAGE_INFO_T *pInMsg)
{
    char         uTopic[256] = {0};
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    uint8              *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;
    uint32           version = APP_VERSION;
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
    appmsg_memcpy_r(temp + msglen, &version, sizeof(uint32));
    msglen += sizeof(uint32);

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
    return advPd_send_mqttmsg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : advPd_get_appidmsg
* @brief     ：查询版本号
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-7-19
* @Update    :
**********************************************************************/
int advPd_get_appidmsg(MESSAGE_INFO_T *pInMsg)
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
    *temp = g_PdAppIDLen;
    if(g_PdAppIDLen > 0)
    {
        memcpy(temp + 1, g_PdAppID, g_PdAppIDLen);
    }
    ptMsgNew->msglen = g_PdAppIDLen + 1;

    sprintf(uTopic, "%s/%s", g_AppName, pInMsg->souraddr);
    return advPd_send_mqttmsg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : advPd_seek_app
* @brief     ：发送APP查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
int advPd_seek_app()
{
    MESSAGE_INFO_T  tMsgInfo;
    int             ret = 0;
    char           *smios = "M-smiOS";
    
    memset(&tMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    tMsgInfo.rpm = 1;
    tMsgInfo.priority = 2;

    tMsgInfo.index = advPd_get_next_index();
    tMsgInfo.label = 0;
    
    tMsgInfo.sourlen = strlen((char *)g_AppName);
    memcpy(tMsgInfo.souraddr , g_AppName, tMsgInfo.sourlen);
    tMsgInfo.destlen = strlen(smios);
    memcpy(tMsgInfo.destaddr, smios, tMsgInfo.destlen);

    tMsgInfo.IID = MESSAGE_IID_SYSTEM;
    tMsgInfo.IOP = SYSTEM_IOP_GET_REGISTERAPP;
    tMsgInfo.msglen = 0;

    ret = advPd_send_mqttmsg(NULL, &tMsgInfo, 0);
    return ret;    
}

/**********************************************************************
* @name      : advPd_recv_regapp
* @brief     ：收到注册APP查询
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
BOOLEAN advPd_recv_regapp(MESSAGE_INFO_T *pInMsg)
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
        memcpy(ucname, pTemp + offset, namelen);
        memset(&tApp, 0, sizeof(APP_INFO_T));
        if(0 == appname_split(ucname, &tApp))
        {
            if(0 == strcmp("dbCenter", tApp.name) && FALSE == g_APPdbCenter.Online)
            {
                memcpy(&g_APPdbCenter, &tApp, sizeof(APP_INFO_T));
                g_APPdbCenter.Online = TRUE;
                printf("======get dbcenter======\n");
                PDMSG_FMT_DEBUG("======get dbcenter======\n");
            }
            else if(0 == strcmp("wirelessDCM", tApp.name) && FALSE == g_APPWireless.Online)
            {
                memcpy(&g_APPWireless, &tApp, sizeof(APP_INFO_T));
                g_APPWireless.Online = TRUE;
                printf("======get wirelessDCM======\n");
                PDMSG_FMT_DEBUG("======get wirelessDCM======\n");
            }
            else if(0 == strcmp("mapManager", tApp.name) && FALSE == g_APPMod.Online)
            {
                memcpy(&g_APPMod, &tApp, sizeof(APP_INFO_T));
                g_APPMod.Online = TRUE;
                printf("======get mapManager======\n");
                PDMSG_FMT_DEBUG("======get mapManager======\n");
            }
            else if(0 == strcmp("taskManager", tApp.name) && FALSE == g_APPCTS.Online)
            {
                memcpy(&g_APPCTS, &tApp, sizeof(APP_INFO_T));
                g_APPCTS.Online = TRUE;
                printf("======get taskManager======\n");
                PDMSG_FMT_DEBUG("======get taskManager======\n");
            }
            else if(0 == strcmp("desktopGui", tApp.name) && FALSE == g_APPGUI.Online)
            {
                memcpy(&g_APPGUI, &tApp, sizeof(APP_INFO_T));
                g_APPGUI.Online = TRUE;
                printf("======get desktopGui======\n");
                PDMSG_FMT_DEBUG("======get desktopGui======\n");
            }
            else if(0 == strcmp("rspSample", tApp.name) && FALSE == g_APPRsp.Online)
            {
                memcpy(&g_APPRsp, &tApp, sizeof(APP_INFO_T));
                g_APPRsp.Online = TRUE;
                printf("======get rspSample======\n");
                PDMSG_FMT_TRACE("======get rspSample======\n");
            }
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

    if(TRUE == g_APPdbCenter.Online && TRUE == g_APPWireless.Online && TRUE == g_APPMod.Online
        && TRUE == g_APPCTS.Online && TRUE == g_APPRsp.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advPd_recv_msg
* @brief     ：对外消息接收, 使用了xl_channel_read 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
int advPd_recv_msg(uint8 *pBuf, uint16 buflen)
{
#if 1    
    char          topic[256];
    int           qos;
    return mqtt_client_recv_msg(g_AdvPdMqttClient, topic, 256, &qos, pBuf, (int)buflen, 1000);
#else 
    return mqtt_client_recv(g_AdvPdMqttClient, pBuf, (int)buflen);
#endif
}

/**********************************************************************
* @name      : advPd_get_dbagent
* @brief     ：获取dbagent句柄
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-10-26
* @Update    :
**********************************************************************/
DB_AGENT advPd_get_dbagent()
{
    return g_dbAgent;
}

/**********************************************************************
* @name      : advPd_start_goon
* @brief     ：判断启动是否可以继续
* @param[in] ：
* @param[out]：
* @return    ：TRUE-可以继续启动/FALSE-等待
* @Create    : 王津剑
* @Date      ：2020-3-5
* @Update    :
**********************************************************************/
BOOLEAN advPd_start_goon()
{
    if(TRUE == g_APPdbCenter.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advPd_mqtt_guiok
* @brief     ：判断是否获取GUI
* @param[in] ：
* @param[out]：
* @return    ：TRUE-可以继续启动/FALSE-等待
* @Create    : 王津剑
* @Date      ：2020-5-15
* @Update    :
**********************************************************************/
BOOLEAN advPd_mqtt_guiok()
{
    if(TRUE == g_APPGUI.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advPd_mqtt_handle
* @brief     ：获取mqtt句柄
* @param[in] ：
* @param[out]：
* @return    ：句柄
* @Create    : 王津剑
* @Date      ：2020-5-15
* @Update    :
**********************************************************************/
void *advPd_mqtt_handle()
{
    return g_AdvPdMqttClient;
}

/**********************************************************************
* @name      : advPd_mqtt_rspok
* @brief     ：判断是否获取遥信脉冲
* @param[in] ：
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2020-6-2
* @Update    :
**********************************************************************/
BOOLEAN advPd_mqtt_rspok()
{
    if(TRUE == g_APPRsp.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : advPd_mqtt_reproc
* @brief     ：mqtt预先处理(提前处理数据中心消息, 防止在队列处理过程中卡死)
* @param[in] ：
* @param[out]：
* @return    ：0-预处理完成;其他-不需要预处理
* @Create    : 王津剑
* @Date      ：2020-10-26
* @Update    :
**********************************************************************/
int advPd_mqtt_reproc(void *pMsg, int msglen)
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
    PDMSG_BUF_TRACE(pMsg, msglen, "Recv msg from %s:", pMsgInfo->souraddr);
    if(0 != ret)
    {
        PDMSG_BUF_DEBUG(pMsg, msglen, "Drop msg from %s, ret %d:", pMsgInfo->souraddr, ret);
    }
    free(pMsgInfo);
    return 0;
}

