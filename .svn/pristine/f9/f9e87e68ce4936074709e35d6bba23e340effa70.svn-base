/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块主线程功能
* @date：    2021-7-24
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "db_api.h"
#include "cJSON.h"
#include "framecomm.h"

#include "rsp_mqtt.h"
#include "rsp_data.h"

// 对外消息 客户端句柄
MQTT_CLIENT_T    *g_RspMsgClient;

pthread_mutex_t   g_RspSendlock; 
pthread_mutex_t   g_RspIndexlock; 
uint16            g_RspMsgindex = 0;

uint8             g_bDBCenter = FALSE;

#define RSP_SUBTOPIC      "+/M-rspSample"
#define RSP_SUBMAPTOPIC   "M-mapManager/Broadcast/#"
#define RSP_SUBTIMETOPIC  "M-smiOS/Broadcast/#"


/**********************************************************************
* @name      : rsp_mqtt_init
* @brief     ：对外消息初始化MQTT
* @param[in] ：uint32 ip           服务器地址
               uint16 port         服务器端口
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-24
* @Update    :
**********************************************************************/
int rsp_mqtt_init(uint32 ip, uint16 port)
{
    int   ret = 0;
    uint8 status = 0;
    char  mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[3];
    char  mqttstatus[2][16] = {"connectlost", "success"};

    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);

    /* 新建客户端 */
    g_RspMsgClient = mqtt_client_create(RSP_APP_NAME);
    if (NULL == g_RspMsgClient)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_create fail\n");
        return -1;
    }
    
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, RSP_SUBTOPIC, strlen(RSP_SUBTOPIC));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, RSP_SUBMAPTOPIC, strlen(RSP_SUBMAPTOPIC));
    subtopic[2].qos = 0;
    memcpy(subtopic[2].topic, RSP_SUBTIMETOPIC, strlen(RSP_SUBTIMETOPIC));
    ret = mqtt_client_set_sub_topic(g_RspMsgClient, subtopic, 3);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        mqtt_client_destroy(g_RspMsgClient);
        return ret;
    }

    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(g_RspMsgClient, "M-rspSample/M-smiOS", 0);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        mqtt_client_destroy(g_RspMsgClient);
        return ret;
    }

    /* 链接服务端 */
    MQTTMSG_FMT_DEBUG("链接mqtt服务端 %s:%d\n", mqttaddr, port);
    ret = mqtt_client_connect(g_RspMsgClient, mqttaddr, port, "127.0.0.1");
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("链接mqtt服务端链接失败 ret %d\n", ret);
        mqtt_client_destroy(g_RspMsgClient);
        return ret;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(g_RspMsgClient);
        MQTTMSG_FMT_DEBUG("mqtt服务端链接状态: %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    MQTTMSG_FMT_DEBUG("链接mqtt服务端成功\n");
    RSP_FMT_DEBUG("链接mqtt服务端成功\n");

    pthread_mutex_init(&g_RspSendlock, NULL);
    pthread_mutex_init(&g_RspIndexlock, NULL);
    return 0;
}

/**********************************************************************
* @name      : rsp_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 王津剑
* @Date      ：2021-7-24
* @Update    :
**********************************************************************/
uint16 rsp_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE((void *)&g_RspIndexlock);
    curindex = g_RspMsgindex;
    g_RspMsgindex++;
    PTHREAD_UNLOCK_SAFE((void *)&g_RspIndexlock);
    return curindex;
}

/**********************************************************************
* @name      : rsp_recv_mqtt_msg
* @brief     ：对外消息接收 1s阻塞时间
* @param[in] ：uint16 topiclen  主题最大容量
               int    buflen    缓存最大容量
* @param[out]：char *pTopic     主题
               uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_recv_mqtt_msg(char *pTopic, uint16 topiclen, uint8 *pBuf, int buflen)
{
    int qos;
    return mqtt_client_recv_msg(g_RspMsgClient, pTopic, topiclen, &qos, pBuf, buflen, 1000);
}

/**********************************************************************
* @name      : rsp_send_mqtt_msg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic               指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_send_mqtt_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
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
        WIRELESS_FMT_DEBUG("App send appmsg_package fail\n");
        return -1;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_RspSendlock);
    if(NULL != pTopic)
    {
        ret = mqtt_client_send_msg(g_RspMsgClient, pTopic, qos, pMsg, (int)msglen);
    }
    else
    {
        ret = mqtt_client_send(g_RspMsgClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_RspSendlock);

    if(0 == ret)
    {
        MQTTMSG_BUF_TRACE(pMsg, msglen, "Send msg to %s:", pMsgInfo->destaddr);
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
* @name      : rsp_send_mapmsg
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_send_mapmsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
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

    pMsgInfo->index = rsp_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen(RSP_APP_NAME);
    memcpy(pMsgInfo->souraddr, RSP_APP_NAME, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen("M-mapManager");
    memcpy(pMsgInfo->destaddr, "M-mapManager", pMsgInfo->destlen);

    pMsgInfo->IID = iid;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    sprintf(uTopic, "%s/%s", RSP_APP_NAME, "M-mapManager");
    ret = rsp_send_mqtt_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : rsp_login_to_smiOS
* @brief     ：APP注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_login_to_smiOS()
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 request[160] = {0};
    uint8 offset = 0;
    uint8 len = 0;
    uint8  appId[24] = {0};
    
    pid_t pid = getpid();
    Date_Time_S tTime;
    char *smios = "M-smiOS";

    app_get_maketime(APP_PUBLISH_TIME, &tTime);

    /* 进程号 */
    appmsg_memcpy_r(request, &pid, sizeof(pid_t));
    offset += 4;

    /* APP名称 */
    len = strlen(RSP_APP_NAME);
    request[offset++] = len;
    memcpy(&request[offset], RSP_APP_NAME, len);
    offset += len;

    /* 订阅事件 */
    request[offset++] = 0;

    /* 版本信息 */
    request[offset++] = strlen(APP_VERSION);
    memcpy(request + offset, APP_VERSION, strlen(APP_VERSION));
    offset += strlen(APP_VERSION);

    /* 发布日期 */
    request[offset++] = tTime.year_h;
    request[offset++] = tTime.year_l;
    request[offset++] = tTime.month;
    request[offset++] = tTime.day;
    request[offset++] = tTime.hour;
    request[offset++] = tTime.min;
    request[offset++] = tTime.sec;

    /* APPID 目前没有写0 */
    request[offset++] = 24;
    memcpy(request + offset, appId, 24);
    offset += 24;
    
    pMsgInfo = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + offset);
    if(NULL == pMsgInfo)
    {
        WIRELESS_FMT_DEBUG("Login to os malloc failed %d\n", sizeof(MESSAGE_INFO_T) + offset);
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = rsp_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = SYSTEM_IOP_APP_REGISTER;
    
    pMsgInfo->sourlen = strlen(RSP_APP_NAME);
    memcpy(pMsgInfo->souraddr , RSP_APP_NAME, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = offset;
    memcpy(pMsgInfo->playload, request, offset);
    
    ret = rsp_send_mqtt_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : rsp_heart_bear
* @brief     ：心跳
* @param[in] ：MESSAGE_JSON_T *pInMsg  请求消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
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
        WIRELESS_FMT_DEBUG("Heart bear malloc failed %d\n", len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 0;
    pMsgInfo->index = pInMsg->index;
    pMsgInfo->priority = pInMsg->priority;
    pMsgInfo->label = pInMsg->label;
    pMsgInfo->IID = MESSAGE_IID_COMMON;
    pMsgInfo->IOP = pInMsg->IOP;
    
    pMsgInfo->sourlen = strlen(RSP_APP_NAME);
    memcpy(pMsgInfo->souraddr , RSP_APP_NAME, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = sizeof(uint32);
    appmsg_memcpy_r(pMsgInfo->playload, &status, sizeof(uint32));
    
    ret = rsp_send_mqtt_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : rsp_version_msg
* @brief     ：获取版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_version_msg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    char         uTopic[256] = {0};
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    uint8              *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;
    Date_Time_S tTime;

    if(NULL == pInMsg || NULL == pArg)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;
    ptMsgNew->index = pInMsg->index;
    ptMsgNew->label = pInMsg->label;

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    temp = ptMsgNew->playload;

    /* 版本信息 */
    temp[msglen++] = strlen(APP_VERSION);
    memcpy(temp + msglen, APP_VERSION, strlen(APP_VERSION));
    msglen += strlen(APP_VERSION);

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
    
    sprintf(uTopic, "%s/%s", RSP_APP_NAME, pInMsg->souraddr);
    return rsp_send_mqtt_msg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : rsp_send_errmsg
* @brief     ：对外消息发送错误的消息
* @param[in] ：MESSAGE_INFO_T *pRecvMsg  收到的消息
               int errCode               错误码
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode)
{
    char         uTopic[256] = {0};
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

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

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
    
    sprintf(uTopic, "%s/%s", RSP_APP_NAME, pRecvMsg->souraddr);
    return rsp_send_mqtt_msg(uTopic, ptMsgNew, 0);
}

