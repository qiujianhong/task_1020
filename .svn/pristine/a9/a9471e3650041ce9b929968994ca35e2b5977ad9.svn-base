/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线拨号mqtt消息处理
* @date：    2020-1-9
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "appmsg_split.h"
#include "mqtt_client.h"

#include "wireless.h"
#include "wire_mqtt.h"
#include "msg_proc.h"


// 对外消息 客户端句柄
MQTT_CLIENT_T *g_WireMsgClient;

#define WIRELESS_DCM_SUBTOPIC      "+/M-wirelessDCM"
#define WIRELESS_DCM_SUBMOD_TOPIC  "M-mapManager/Broadcast/0004/0001"
#define WIRELESS_DCM_SUBGUI_TOPIC  "M-desktopGui/Broadcast/0007/#"


pthread_mutex_t   g_WireSendlock; 
pthread_mutex_t   g_WireIndexlock; 
uint16            g_WireMsgindex = 0;

uint8      g_PPPStatus[2] = {1, 1};

uint8 g_ATSwitch = FALSE;

/**********************************************************************
* @name      : wireless_mqtt_init
* @brief     ：对外消息初始化MQTT
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_mqtt_init(uint32 ip, uint16 port)
{
    int   ret = 0;
    uint8 status = 0;
    char  mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[3];
    char  mqttstatus[2][16] = {"connectlost", "success"};

    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);

    /* 新建客户端 */
    g_WireMsgClient = mqtt_client_create(WIRELESS_DCM_NAME);
    if (NULL == g_WireMsgClient)
    {
        WIRELESS_FMT_DEBUG("mqtt_client_create fail\n");
        return -1;
    }
    
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, WIRELESS_DCM_SUBTOPIC, strlen(WIRELESS_DCM_SUBTOPIC));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, WIRELESS_DCM_SUBMOD_TOPIC, strlen(WIRELESS_DCM_SUBMOD_TOPIC));
    subtopic[2].qos = 0;
    memcpy(subtopic[2].topic, WIRELESS_DCM_SUBGUI_TOPIC, strlen(WIRELESS_DCM_SUBGUI_TOPIC));

    ret = mqtt_client_set_sub_topic(g_WireMsgClient, subtopic, 3);
    if(0 != ret)
    {
        WIRELESS_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        mqtt_client_destroy(g_WireMsgClient);
        return ret;
    }

    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(g_WireMsgClient, "M-wirelessDCM/M-smiOS", 0);
    if(0 != ret)
    {
        WIRELESS_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        mqtt_client_destroy(g_WireMsgClient);
        return ret;
    }

    /* 链接服务端 */
    WIRELESS_FMT_DEBUG("链接mqtt服务端 %s:%d\n", mqttaddr, port);
    ret = mqtt_client_connect(g_WireMsgClient, mqttaddr, port, "127.0.0.1");
    if(0 != ret)
    {
        WIRELESS_FMT_DEBUG("链接mqtt服务端链接失败 ret %d\n", ret);
        mqtt_client_destroy(g_WireMsgClient);
        return ret;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(g_WireMsgClient);
        WIRELESS_FMT_TRACE("mqtt服务端链接状态: %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    WIRELESS_FMT_DEBUG("链接mqtt服务端成功\n");

    pthread_mutex_init(&g_WireSendlock, NULL);
    pthread_mutex_init(&g_WireIndexlock, NULL);
    return 0;
}

/**********************************************************************
* @name      : wireless_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
uint16 wireless_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE((void *)&g_WireIndexlock);
    curindex = g_WireMsgindex;
    g_WireMsgindex++;
    PTHREAD_UNLOCK_SAFE((void *)&g_WireIndexlock);

    return curindex;
}

/**********************************************************************
* @name      : wireless_send_msg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
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
    PTHREAD_LOCK_SAFE(&g_WireSendlock);
    if(NULL != pTopic)
    {
        ret = mqtt_client_send_msg(g_WireMsgClient, pTopic, qos, pMsg, (int)msglen);
    }
    else
    {
        ret = mqtt_client_send(g_WireMsgClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_WireSendlock);

    if(0 == ret)
    {
        MQTTMSG_BUF_TRACE(pMsg, msglen, "Send msg to %s:", pMsgInfo->destaddr);
    }
    else
    {
        WIRELESS_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                            pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : wireless_send_errmsg
* @brief     ：对外消息发送错误的消息
* @param[in] ：MESSAGE_INFO_T *pRecvMsg  收到的消息
               int errCode               错误码
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-2
* @Update    :
**********************************************************************/
int wireless_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode)
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

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

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
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pRecvMsg->souraddr);
    return wireless_send_msg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : wireless_recv_msg
* @brief     ：对外消息接收 1s阻塞时间
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：char *pTopic     主题
               uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_recv_msg(char *pTopic, uint8 *pBuf, uint16 buflen)
{
#if 1
    int qos;
    return mqtt_client_recv_msg(g_WireMsgClient, pTopic, 256, &qos, pBuf, (int)buflen, 1000);
#else 
    return mqtt_client_recv(g_WireMsgClient, pBuf, (int)buflen);
#endif
}

/**********************************************************************
* @name      : advance_login_to_smios
* @brief     ：APP注册
* @param[in] ：uint8 *pAppId   APPID
               uint8 appidlen  长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    : 2020-7-19
**********************************************************************/
int wireless_login_to_smios(uint8 *pAppId, uint8 appidlen)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 request[160] = {0};
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
    len = strlen(WIRELESS_DCM_NAME);
    request[offset++] = len;
    memcpy(&request[offset], WIRELESS_DCM_NAME, len);
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
        WIRELESS_FMT_DEBUG("Login to os malloc failed %d\n", sizeof(MESSAGE_INFO_T) + offset);
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = wireless_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = SYSTEM_IOP_APP_REGISTER;
    
    pMsgInfo->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(pMsgInfo->souraddr , WIRELESS_DCM_NAME, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = offset;
    memcpy(pMsgInfo->playload, request, offset);
    
    ret = wireless_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : wireless_heart_bear
* @brief     ：心跳
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
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
    
    pMsgInfo->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(pMsgInfo->souraddr , WIRELESS_DCM_NAME, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = sizeof(uint32);
    appmsg_memcpy_r(pMsgInfo->playload, &status, sizeof(uint32));
    
    ret = wireless_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : wireless_get_versionmsg
* @brief     ：获取版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-2
* @Update    :
**********************************************************************/
int wireless_get_versionmsg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
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

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

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
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pInMsg->souraddr);
    return wireless_send_msg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : wireless_get_selfversion
* @brief     ：获取版本
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-5-30
* @Update    :
**********************************************************************/
int wireless_get_selfversion(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    char         uTopic[256] = {0};
    uint8           buf[480] = {0};
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

    ptMsgNew->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(ptMsgNew->souraddr, WIRELESS_DCM_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    temp = ptMsgNew->playload;

    /* APP名称 */
    temp[msglen++] = strlen("wirelessDCM");
    memcpy(temp + msglen, "wirelessDCM", strlen("wirelessDCM"));
    msglen += strlen("wirelessDCM");

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
    
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, pInMsg->souraddr);
    return wireless_send_msg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : wireless_get_version
* @brief     ：获取版本
* @param[in] ：Date_Time_S *ptTime  发布时间
* @param[out]：
* @return    ：版本号
* @Create    : 王津剑
* @Date      ：2020-5-3
* @Update    :
**********************************************************************/
char* wireless_get_version(Date_Time_S *ptTime)
{
    if(NULL == ptTime)
    {
        return 0;
    }
    app_get_maketime(APP_PUBLISH_TIME, ptTime);
    return APP_VERSION;
}

/**********************************************************************
* @name      : wireless_send_dbmsg
* @brief     ：高级app对外发送数据中心接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_send_guimsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
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

    pMsgInfo->index = wireless_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(pMsgInfo->souraddr, WIRELESS_DCM_NAME, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen("M-desktopGui");
    memcpy(pMsgInfo->destaddr, "M-desktopGui", pMsgInfo->destlen);

    pMsgInfo->IID = iid;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, "M-desktopGui");
    ret = wireless_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : wireless_send_modmsg
* @brief     ：高级app对外模组管理器消息接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
int wireless_send_modmsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
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

    pMsgInfo->index = wireless_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(pMsgInfo->souraddr, WIRELESS_DCM_NAME, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen("M-mapManager");
    memcpy(pMsgInfo->destaddr, "M-mapManager", pMsgInfo->destlen);

    pMsgInfo->IID = iid;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    sprintf(uTopic, "%s/%s", WIRELESS_DCM_NAME, "M-mapManager");
    ret = wireless_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : wireless_send_diagchange
* @brief     ：高级app对外通知拨号事件变化
* @param[in] ：uint8 id        设备id
               uint8 status    状态 0-成功/1-失败
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int wireless_send_diagchange(uint8 id, uint8 status)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    
    infolen = sizeof(MESSAGE_INFO_T) + 2;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        WIRELESS_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;
    pMsgInfo->index = wireless_get_next_index();
    pMsgInfo->label = 0;
    
    pMsgInfo->sourlen = strlen(WIRELESS_DCM_NAME);
    memcpy(pMsgInfo->souraddr, WIRELESS_DCM_NAME, pMsgInfo->sourlen);
    
    pMsgInfo->destlen = strlen("Broadcast");
    memcpy(pMsgInfo->destaddr, "Broadcast", pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_WIRELESS;
    pMsgInfo->IOP = WIRELESS_IOP_DIAG_EVENT;

    pMsgInfo->msglen = 2;
    *pMsgInfo->playload = id;
    *(pMsgInfo->playload + 1) = status;

    ret = wireless_send_msg("M-wirelessDCM/Broadcast/0002/0001", pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : wireless_set_pppstatus
* @brief     ：ppp拨号状态记录
* @param[in] ：uint8 ifno      拨号状态
               uint8 status    状态 0-成功/1-失败
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
void wireless_set_pppstatus(uint8 ifno, uint8 status)
{
    if(ifno >= 2)
    {
        return;
    }
    if(status != g_PPPStatus[ifno])
    {
        g_PPPStatus[ifno] = status;
        wireless_send_diagchange(ifno + 1, status);
    }
}

/**********************************************************************
* @name      : wireless_send_change
* @brief     ：刷新工具栏 远程通信状态
* @param[in] ：uint8 id        设备id (0-1)
               uint8 netmode   网络制式
               uint16 csqvalue 信号强度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-14
* @Update    : 2020-4-22
**********************************************************************/
int wireless_send_change(uint8 id, uint8 netmode, uint16 csqvalue)
{
    uint8  playout[32] = {0};
    uint16 msglen = 0;

    playout[msglen++] = 2;

    /* 信号强度 */
    playout[msglen++] = id;
    playout[msglen++] = 0;
    playout[msglen++] = ((csqvalue >> 8) & 0xff);
    playout[msglen++] = (csqvalue & 0xff);

    /* 网络制式 */
    playout[msglen++] = id;
    playout[msglen++] = 1;
    playout[msglen++] = netmode;

    return wireless_send_guimsg(id, MESSAGE_IID_DESKGUI, DESKGUI_IOP_FRESH_TOOL, playout, msglen);
}

/**********************************************************************
* @name      : wireless_send_diagStatus
* @brief     ：高级app对外通知拨号状态事件
* @param[in] ：uint8  id    设备id (1-2)
               char  *str  内容
               uint16 len  长度
               uint32 times 刷新时间
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-4-1
* @Update    :
**********************************************************************/
int wireless_send_diagStatus(uint8 id, char *str, uint16 len, uint32 times)
{
    uint8  playout[512] = {0};
    uint16 msglen = 0;
    uint8  lenBuf[3] = {0};
    uint8  offset = 0;
    uint32 timelen = MAX(5, times);

    appmsg_memcpy_r(playout + msglen, &timelen, sizeof(uint32));
    msglen += sizeof(uint32);

    offset = appmsg_set_len_offset(len, lenBuf);
    memcpy(playout + msglen, lenBuf, offset);
    msglen += offset;

    memcpy(playout + msglen, str, len);
    msglen += len;

    return wireless_send_guimsg(id, MESSAGE_IID_DESKGUI, DESKGUI_IOP_FRESH_STATE, playout, msglen);
}

/**********************************************************************
* @name      : wireless_restart_mode
* @brief     ：发送重启模组复位的消息
* @param[in] ：uint32 port    设备编号
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2021-4-7
* @Update    :
**********************************************************************/
void wireless_restore_mode(uint8 ifnum, uint32 port)
{
    int ret = 0;
    uint8 buf[24] = {0};
    uint16 offset = 0;
    uint16 label = ifnum;

    if(0 == (port & 0x0f) || (port & 0x0f) > 5)
    {
        return;
    }

    appmsg_memcpy_r(buf, &port, sizeof(uint32));
    offset += sizeof(uint32);

    label = (ifnum << 8) + port;

    ret = wireless_send_modmsg(label, MESSAGE_IID_MODULE, MODULE_IOP_RESTORE, buf, offset);
    WIRELESS_FMT_DEBUG("发送模组%02X复位消息%s\n", port, 0 == ret ? "成功" : "失败");
}

/**********************************************************************
* @name      : wireless_reboot_mode
* @brief     ：发送重启设备的消息(整个槽位重启)
* @param[in] ：uint32 port    设备编号
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-4-24
* @Update    :
**********************************************************************/
void wireless_reboot_mode(uint8 ifnum, uint32 port)
{
    int ret = 0;
    uint8 buf[24] = {0};
    uint16 offset = 0;
    uint32 time = 5;
    uint16 label = ifnum;

    if(0 == port || port > 5)
    {
        return;
    }

    appmsg_memcpy_r(buf, &port, sizeof(uint32));
    offset += sizeof(uint32);
    appmsg_memcpy_r(buf + offset, &time, sizeof(uint32));
    offset += sizeof(uint32);

    label = (ifnum << 8) + port;

    ret = wireless_send_modmsg(label, MESSAGE_IID_MODULE, MODULE_IOP_RESTART, buf, offset);
    WIRELESS_FMT_DEBUG("发送重启槽位%d消息%s\n", port, 0 == ret ? "成功" : "失败");
}


