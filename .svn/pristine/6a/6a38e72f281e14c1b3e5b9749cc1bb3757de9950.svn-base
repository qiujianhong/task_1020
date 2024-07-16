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
#include <net/if.h>
#include "oopType.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "task_id.h"
#include "mqtt_client.h"

#include "msg_base.h"
#include "out_msg.h"
#include "framecomm.h"

// 对外消息 客户端句柄
MQTT_CLIENT_T *g_GuiMqttClient;


pthread_mutex_t   g_MsgSendlock; 
pthread_mutex_t   g_MsgIndexlock; 
uint16            g_AppMsgindex = 0;
int	gRetry = 0;
BOOL g_getdbstate;

extern 	DB_AGENT   g_dbAgent;

uint8      g_AppName[APPMSG_NAME_LEN] = {0};
APP_INFO_T g_APPdbCenter;  //数据中心
APP_INFO_T g_APPWireless;  //无线拨号
APP_INFO_T g_APPCTS;       //采集任务调度

APP_VERSION_INFO_T g_AppVersion;

/**********************************************************************
* @name      : gui_next_index_init
* @brief     ：对外消息index管理初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
void gui_next_index_init()
{
    pthread_mutex_init(&g_MsgIndexlock, NULL);
    g_AppMsgindex = 0;
}

/**********************************************************************
* @name      : gui_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
uint16 gui_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE(&g_MsgIndexlock);
    curindex = g_AppMsgindex;
    g_AppMsgindex++;
    PTHREAD_UNLOCK_SAFE(&g_MsgIndexlock);

    return curindex;
}

/**********************************************************************
* @name      : gui_outmsg_init
* @brief     ：对外消息初始化
* @param[in] ：uint8 type   通信方式 0-UDP/1-TCP/2-MQTT
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int gui_outmsg_init(uint8 type, uint32 ip, uint16 port)
{
    int ret = 0;
    uint8 status = 0;
	char   mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[2];
    char  mqttstatus[2][16] = {"connectlost", "success"};

    /* 新建客户端 */
    sprintf((char *)g_AppName, "M-%s", GUI_APP_NAME);
    g_GuiMqttClient = mqtt_client_create((char *)g_AppName);
    if (NULL == g_GuiMqttClient)
    {
        GUI_FMT_TRACE("mqtt_client_create fail\n");
        return -1;
    }
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, g_AppName, strlen((char *)g_AppName));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, GUI_APP_SUBTOPIC, strlen(GUI_APP_SUBTOPIC));
    ret = mqtt_client_set_sub_topic(g_GuiMqttClient, subtopic, 2);
    if(0 != ret)
    {
        GUI_FMT_TRACE("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        goto errorend;
    }
    
    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(g_GuiMqttClient, "M-desktopGui/M-smiOS", 0);
    if(0 != ret)
    {
        GUI_FMT_TRACE("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        return ret;
    }

    /* 链接服务端 */
    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    GUI_FMT_TRACE("mqtt connect %s:%d\n", mqttaddr, port);
    ret = mqtt_client_connect(g_GuiMqttClient, mqttaddr, port, "127.0.0.1");
    if(0 != ret)
    {
        GUI_FMT_TRACE("mqtt_client_connect fail, ret %d", ret);
        goto errorend;
    }

    /* 等待链接成功 */
    status = mqtt_client_get_status(g_GuiMqttClient);
    GUI_FMT_DEBUG("Get mqtt connect status %s\n", mqttstatus[status]);
    while(1)
    {
        status = mqtt_client_get_status(g_GuiMqttClient);
        if(MQTT_CONNECT_SUCC == status)
        {
            GUI_FMT_DEBUG("Get mqtt connect status %s\n", mqttstatus[status]);
            break;
        }
        sleep(1);
    }
    GUI_FMT_DEBUG("mqtt connect %s:%d success\n", mqttaddr, port);

    /* 创建db_agent */
    g_dbAgent = db_agent_create(AGENT_TYPE_THREAD, DB_GUI_AGENT, gui_send_dbmsg, GUI_DB_CLIENT_NUM);
    {
        if(g_dbAgent < 0)
        {
            GUI_FMT_TRACE("db_agent_create failed!\n");
            ret = 1;
            goto errorend;
        }
    }

    gui_next_index_init();
    pthread_mutex_init(&g_MsgSendlock, NULL);
    memset(&g_APPdbCenter , 0, sizeof(APP_INFO_T));
    memset(&g_APPWireless , 0, sizeof(APP_INFO_T));
    memset(&g_APPCTS , 0, sizeof(APP_INFO_T));

    return 0;
errorend:
    mqtt_client_destroy(g_GuiMqttClient);
    return ret;

}

/**********************************************************************
* @name      : gui_get_dbagent
* @brief     ：获取dbagent句柄
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
DB_AGENT gui_get_dbagent()
{
    return g_dbAgent;
}

/**********************************************************************
* @name      : gui_outmsg_uninit
* @brief     ：对外通信客户端去初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
void gui_outmsg_uninit()
{
    mqtt_client_disconnect(g_GuiMqttClient);
    mqtt_client_destroy(g_GuiMqttClient);

    db_agent_close(g_dbAgent);
    g_dbAgent = 0;
    pthread_mutex_lock(&g_MsgSendlock);
    pthread_mutex_destroy(&g_MsgSendlock);
    pthread_mutex_lock(&g_MsgIndexlock);
    pthread_mutex_destroy(&g_MsgIndexlock);
}


/**********************************************************************
* @name      : gui_send_msg
* @brief     ：对外消息异步发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int gui_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
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
        GUI_FMT_TRACE("App send appmsg_package fail\n");
        return -1;
    }

    if(msglen >= APP_RECV_BUF_LEN)
    {
        GUI_FMT_TRACE("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);
    if(NULL != pTopic && 'M' == pMsgInfo->destaddr[0])
    {
        ret = mqtt_client_send_msg(g_GuiMqttClient, pTopic, qos, pMsg, (int)msglen);
        GUI_FMT_TRACE("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);
    }
    else
    {
        ret = mqtt_client_send(g_GuiMqttClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);

    if(0 == ret)
    {
		GUI_BUF_TRACE(pMsg, msglen, "Send msg:");

    }
    else
    {
        GUI_FMT_TRACE("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                        pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : gui_send_dbmsg
* @brief     ：gui对外发送数据中心接口
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
int gui_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

//    if(FALSE == g_APPdbCenter.Online)
    {
//        GUI_FMT_TRACE("No dbCenter!\n");
//        return -1;
    }
    appname_split((uint8*)"M-dbCenter", &g_APPdbCenter);
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_TRACE("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    *pIndex = gui_get_next_index();
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

    sprintf(uTopic, "%s/M-%s",g_AppName, g_APPdbCenter.name);
    ret = gui_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : gui_send_ctsmsg
* @brief     ：gui对外发送采集任务调度消息接口
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
int gui_send_ctsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
	char           *cts = "M-taskManager";

//    if(FALSE == g_APPCTS.Online)
//    {
//        GUI_FMT_TRACE("No CTS!\n");
//        return -1;
//    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_TRACE("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

	pMsgInfo->destlen = strlen((char *)cts);
	memcpy(pMsgInfo->destaddr, cts, pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_CTS;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%s", g_AppName, cts);
    ret = gui_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : gui_send_advpdamrmsg
* @brief     ：gui对外发送无线拨号app消息接口
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
/*int gui_send_advpdamrmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[36] = {0};

    if(FALSE == g_APPAdvPuAmr.Online)
    {
        GUI_FMT_TRACE("No advPdAmr!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_TRACE("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    appname_package(&g_APPAdvPuAmr, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_APPGUI;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/%c-%s", g_AppName, g_APPAdvPuAmr.commType, g_APPAdvPuAmr.name);
    ret = gui_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}
*/
/**********************************************************************
* @name      : gui_send_wirelessmsg
* @brief     ：gui对外发送无线拨号app消息接口(扩展)
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
int gui_send_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_APPWireless.Online)
    {
        GUI_FMT_TRACE("No Wireless!\n");
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_TRACE("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = gui_get_next_index();
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
    ret = gui_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : gui_send_smios
* @brief     ：gui对外模组管理器消息接口
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
int gui_send_smios(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char           *smios = "M-smiOS";

    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = gui_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)g_AppName);
    memcpy(pMsgInfo->souraddr, g_AppName, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    ret = gui_send_msg(NULL, pMsgInfo, 0);
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
int gui_send_acmeter(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length)
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
        GUI_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = gui_get_next_index();
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
    ret = gui_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}



/**********************************************************************
* @name      : gui_login_to_smios
* @brief     ：APP注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int gui_login_to_smios()
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 request[200] = {0};
    uint8 offset = 0;
    uint8 len = 0;
    uint32 subevent[4] = {0x00040002, 0x00020002, 0x10040002, 0x1005000e};
    
    pid_t pid = getpid();
    Date_Time_S tTime;
    char *smios = "M-smiOS";

    uint32 version = APP_VERSION;
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
    request[offset++] = 4;
    appmsg_memcpy_r(request + offset, &subevent[0], sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(request + offset, &subevent[1], sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(request + offset, &subevent[2], sizeof(uint32));
    offset += sizeof(uint32);
	appmsg_memcpy_r(request + offset, &subevent[3], sizeof(uint32));
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

    /* app_id */
    request[offset++] = 24;
    offset += 24;
    
    pMsgInfo = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + offset);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_TRACE("Login to os malloc failed %d\n", sizeof(MESSAGE_INFO_T) + offset);
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = gui_get_next_index();
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
    
    ret = gui_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : gui_heart_bear
* @brief     ：心跳
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int gui_heart_bear(MESSAGE_INFO_T *pInMsg)
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
        GUI_FMT_TRACE("Heart bear malloc failed %d\n", len);
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
    
    ret = gui_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : gui_seek_app
* @brief     ：发送APP查询
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int gui_seek_app()
{
    MESSAGE_INFO_T  tMsgInfo;
    int             ret = 0;
    char           *smios = "M-smiOS";
    
    memset(&tMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    tMsgInfo.rpm = 1;
    tMsgInfo.priority = 2;

    tMsgInfo.index = gui_get_next_index();
    tMsgInfo.label = 0;
    
    tMsgInfo.sourlen = strlen((char *)g_AppName);
    memcpy(tMsgInfo.souraddr , g_AppName, tMsgInfo.sourlen);
    tMsgInfo.destlen = strlen(smios);
    memcpy(tMsgInfo.destaddr, smios, tMsgInfo.destlen);

    tMsgInfo.IID = MESSAGE_IID_SYSTEM;
    tMsgInfo.IOP = SYSTEM_IOP_GET_REGISTERAPP;
    tMsgInfo.msglen = 0;

    ret = gui_send_msg(NULL, &tMsgInfo, 0);
    return ret;    
}

/**********************************************************************
* @name      : gui_recv_regapp
* @brief     ：收到注册APP查询
* @param[in] ：MESSAGE_INFO_T *pInMsg  消息
* @param[out]：
* @return    ：TRUE-全部结束/FALSE-没有结束
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
BOOL gui_recv_regapp(MESSAGE_INFO_T *pInMsg)
{
    uint8  bytes = 0;
    uint16 offset = 0;
    uint8 *pTemp = NULL;
    uint8  i = 0;
    uint8  namelen = 0;
    uint8  ucname[APPMSG_NAME_LEN] = {0};
    APP_INFO_T tApp;
    uint8  subnum = 0;
    uint8  IDlen = 0;
    
    if(NULL == pInMsg)
    {
        return FALSE;
    }

	memset(&g_AppVersion, 0, sizeof(APP_VERSION_INFO_T));

    pTemp = pInMsg->playload;
    g_AppVersion.loginAppCnt = appmsg_get_len_offset(pTemp, &bytes);
    pTemp += bytes;

	if(g_AppVersion.loginAppCnt > 36)
    {
        g_AppVersion.loginAppCnt = 36;		//最大个数
    }

    for(i = 0; i < g_AppVersion.loginAppCnt; i++)
    {
        namelen = pTemp[offset++];
        
        memset(ucname, 0, APPMSG_NAME_LEN);
        memcpy(ucname, pTemp + offset, namelen);
        memset(&tApp, 0, sizeof(APP_INFO_T));
        if(0 == appname_split(ucname, &tApp))
        {
        	memcpy(g_AppVersion.pluginInfo[i].comName, tApp.name, strlen(tApp.name));
        }

        offset += namelen;
        subnum = pTemp[offset++];
        offset += subnum  * sizeof(uint32);  //订阅事件
        appmsg_memcpy_r(&g_AppVersion.pluginInfo[i].version, pTemp + offset, sizeof(uint32));		//版本信息
        offset +=  sizeof(uint32);
		g_AppVersion.pluginInfo[i].dataTime.year_h = pTemp[offset++];
		g_AppVersion.pluginInfo[i].dataTime.year_l = pTemp[offset++];
		g_AppVersion.pluginInfo[i].dataTime.month = pTemp[offset++];
		g_AppVersion.pluginInfo[i].dataTime.day = pTemp[offset++];
		g_AppVersion.pluginInfo[i].dataTime.hour = pTemp[offset++];
		g_AppVersion.pluginInfo[i].dataTime.min = pTemp[offset++];
		g_AppVersion.pluginInfo[i].dataTime.sec = pTemp[offset++];

        //appid
        IDlen = pTemp[offset++];      
        offset += IDlen;
        
        pTemp += offset;
        offset = 0;
    }

	return TRUE;
}

/**********************************************************************
* @name      : gui_recv_msg
* @brief     ：对外消息接收, 使用了xl_channel_read 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int gui_recv_msg(uint8 *pBuf, uint16 buflen)
{
#if 1    
    char    topic[256];
    int     qos;
    return mqtt_client_recv_msg(g_GuiMqttClient, topic, 256, &qos, pBuf, (int)buflen, 1000);
#else
    usleep(10);
    return mqtt_client_recv(g_GuiMqttClient, pBuf, (int)buflen);
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
BOOL gui_start_goon()
{
    if(TRUE == g_APPdbCenter.Online || gRetry > 10)
    {
        return TRUE;
    }
    return FALSE;
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
int gui_get_version(MESSAGE_INFO_T *pInMsg)
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
    return gui_send_msg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : advance_get_appid
* @brief     ：查询APPID
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-29
* @Update    :
**********************************************************************/
int gui_get_appid(MESSAGE_INFO_T *pInMsg)
{
    char         uTopic[256] = {0};
    uint8           buf[360] = {0};
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

    ptMsgNew->msglen = 24;

    sprintf(uTopic, "%s/%s", g_AppName, pInMsg->souraddr);
    return gui_send_msg(uTopic, ptMsgNew, 0);
}



