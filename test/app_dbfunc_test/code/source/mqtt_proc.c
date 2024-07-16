/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   mqtt消息处理
* @date：    2020-4-6
* @history： 
*********************************************************************
*/
#include "xlChannel.h"
#include "baseCommon.h"
#include "oopType.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "comm_log.h"

#include "mqtt_base.h"
#include "mqtt_proc.h"
#include "macro.h"
#include "framecomm.h"


// 对外消息 客户端句柄
XL_CHANNEL_CTX_T *g_MqttMsgCtx;

//连接mqtt broker 资源地址 协议类型://ip地址:端口
const char *mqttUrl = "mqtt://172.17.0.1:1883";




pthread_mutex_t   g_MsgSendlock; 
pthread_mutex_t   g_MsgIndexlock; 
uint16            g_AppMsgindex = 0;


char       g_destServiceTopic[APPMSG_NAME_LEN] = {0};
// 接收消息 客户端句柄
XL_CHANNEL_CTX_T *g_RecvMqttMsgCtx;

XL_CHANNEL_CTX_T *g_dbMqttMsgCtx[MAX_THREAD];


APP_INFO_T g_APPdbCenter;  //数据中心


/**********************************************************************
* @name      : app_next_index_init
* @brief     ：对外消息index管理初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：
* @Update    :
**********************************************************************/
void app_next_index_init()
{
    pthread_mutex_init(&g_MsgIndexlock, NULL);
    g_AppMsgindex = 0;
}

/**********************************************************************
* @name      : app_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 
* @Date      ：2020-3-4
* @Update    :
**********************************************************************/
uint16 app_get_next_index()
{
    uint16 curindex = 0;

    PTHREAD_LOCK_SAFE(&g_MsgIndexlock);

    curindex = g_AppMsgindex;
    g_AppMsgindex++;

    PTHREAD_UNLOCK_SAFE(&g_MsgIndexlock);
    return curindex;
}

/**********************************************************************
* @name      : app_mqtt_init
* @brief     ：对外消息初始化
* @param[in] ：uint32 ip
               uint16 port
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
int app_mqtt_init(uint32 ip, uint16 port)
{
    int          ret = 0;
    uint8        status = 0;
    char         mqttaddr[48] = {0};
    MQTT_TOPIC_T subtopic[2];

    sprintf(mqttaddr, "mqtt://%d.%d.%d.%d:%d", (ip>>24) & 0xff, (ip>>16) & 0xff, (ip>>8) & 0xff, ip & 0xff, port);
    
    memset(&subtopic, 0, sizeof(subtopic));
    
    ret = xl_channel_create(mqttaddr, &g_MqttMsgCtx);
    if (ret != 0)
    {
        printf("xl_channel_create fail, ret %d\n", ret);
        return ret;
    }
    
    ret =  xl_channel_set_mqtt_user_ID(g_MqttMsgCtx, (char*)APP_NAME);
    if(0 != ret)
    {
        printf("xl_channel_set_mqtt_user_ID fail, ret %d\n", ret);
        return ret;
    }
    
    /* 订阅主题 */
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, APP_NAME, strlen((char *)APP_NAME));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, APP_SUBTOPIC, strlen(APP_SUBTOPIC));
    ret = xl_channel_set_mqtt_sub_topic(g_MqttMsgCtx, 2, &subtopic);
    if(0 != ret)
    {
        printf("xl_channel_set_mqtt_user_ID fail, ret %d\n", ret);
        return ret;
    }

    /* 设置默认主题 */
    xl_channel_set_mqtt_default_topic(g_MqttMsgCtx, "M-appe/M-smiOS");
    if(0 != ret)
    {
        printf("xl_channel_set_mqtt_default_topic fail, ret %d\n", ret);
        return ret;
    }
    
    ret =  xl_channel_init(g_MqttMsgCtx);
    if(0 != ret)
    {
        printf("xl_channel_init fail, ret %d\n", ret);
        return ret;
    }

    while(1)
    {
        status = xl_channel_get_connect_status(g_MqttMsgCtx);
        printf("Get mqtt connect status %d\n", status);
        if(INIT == status)
        {
            xl_channel_connect(g_MqttMsgCtx);
        }
        else if(CONNECT_ERR == status)
        {
            printf("Get mqtt connect status error(%s), retry!\n", mqttaddr);

            sleep(2);
            xl_channel_connect(g_MqttMsgCtx);//不退出，一直重复尝试
        }
        else if(CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }

    app_next_index_init();
    pthread_mutex_init(&g_MsgSendlock, NULL);
    memset(&g_APPdbCenter , 0, sizeof(APP_INFO_T));

    /* 测试使用 */
    memcpy(g_APPdbCenter.name, "dbCenter", strlen("dbCenter"));
    g_APPdbCenter.commType = 'M';
    g_APPdbCenter.Online = TRUE;

    return 0;
}

/**********************************************************************
* @name      : app_outmsg_uninit
* @brief     ：对外通信客户端去初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
void app_outmsg_uninit()
{
    xl_channel_disconnect(g_MqttMsgCtx);
    xl_channel_destroy(g_MqttMsgCtx);

    pthread_mutex_lock(&g_MsgSendlock);
    pthread_mutex_destroy(&g_MsgSendlock);
    pthread_mutex_lock(&g_MsgIndexlock);
    pthread_mutex_destroy(&g_MsgIndexlock);
}

/**********************************************************************
* @name      : app_send_mqttmsg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
int app_send_mqttmsg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    int    mid = 0;
    
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    msglen = appmsg_package(pMsgInfo, &pMsg);
    if(0 == msglen)
    {
        printf("App send appmsg_package fail, IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        return -1;
    }

    if(msglen >= RECV_BUF_LEN)
    {
        printf("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_MsgSendlock);

    if(NULL != pTopic && 'M' == pMsgInfo->destaddr[0])
    {
        ret = xl_channel_mqtt_write(g_MqttMsgCtx, pTopic, qos, &mid, (char*)pMsg, (int)msglen);
        //printf("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);
    }
    else
    {
        ret = xl_channel_write(g_MqttMsgCtx, (char*)pMsg, (int)msglen);
    }

    PTHREAD_UNLOCK_SAFE(&g_MsgSendlock);

    if(ret > 0)
    {
        DPRINT_MSG(pMsg, msglen, "Send mqtt msg:\n");
    }
    else
    {
        printf("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return (ret > 0) ? 0 : -1;
}


/**********************************************************************
* @name      : app_send_dbmsg
* @brief     ：高级app对外发送数据中心接口
* @param[in] ：uint16          label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]：uint16*         index
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
int app_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[36] = {0};

    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        printf("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    *pIndex = app_get_next_index();
    pMsgInfo->index = *pIndex;
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen((char *)APP_NAME);
    memcpy(pMsgInfo->souraddr, APP_NAME, pMsgInfo->sourlen);

    appname_package(&g_APPdbCenter, (uint8 *)pMsgInfo->destaddr, MESSAGE_ADDR_LEN);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_DB;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/M-%s", APP_NAME, g_APPdbCenter.name);
    ret = app_send_mqttmsg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}


/**********************************************************************
* @name      : app_recv_msg
* @brief     ：对外消息接收, 使用了xl_channel_read 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
int app_recv_msg(uint8 *pBuf, uint16 buflen)
{
    if(INIT == xl_channel_get_connect_status(g_MqttMsgCtx))
    {
        printf("Mqtt offline, reconnect!\n");
        xl_channel_connect(g_MqttMsgCtx);
        sleep(1);
    }
        
#if 1    
    char          topic[256];
    int           qos;
    int           mid;
    return xl_channel_mqtt_read(g_MqttMsgCtx, topic, &qos, &mid, (char*)pBuf, (int)buflen, 1000);
#else
    usleep(10);  
    return xl_channel_read(g_MqttMsgCtx, (char*)pBuf, (int)buflen);
#endif
}

/**********************************************************************
* @name      : app_start_goon
* @brief     ：判断启动是否可以继续
* @param[in] ：
* @param[out]：
* @return    ：TRUE-可以继续启动/FALSE-等待
* @Create    : 
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
BOOLEAN app_start_goon()
{
    if(TRUE == g_APPdbCenter.Online)
    {
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************
* @name      : app_mqtt_init
* @brief     ：对外消息初始化
* @param[in] ：XL_CHANNEL_CTX_T ** cxt
               g_AppName  使用前赋值 
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-5-6
* @Update    :
**********************************************************************/
int db_mqtt_init(XL_CHANNEL_CTX_T ** cxt, char * app_name)
{
    int          ret = 0;
    uint8        status = 0;
    char         mqttaddr[48] = {0};
    MQTT_TOPIC_T subtopic[1];
    
    memset(&subtopic, 0, sizeof(subtopic));
    
    ret = xl_channel_create(mqttUrl, cxt);
    if (ret != 0)
    {
        printf("xl_channel_create fail, ret %d\n", ret);
        return ret;
    }
    
    ret =  xl_channel_set_mqtt_user_ID(*cxt, (char*)app_name);
    if(0 != ret)
    {
        printf("xl_channel_set_mqtt_user_ID fail, ret %d\n", ret);
        return ret;
    }
    
    /* 订阅主题 */
    subtopic[0].qos = 0;
    sprintf(subtopic[0].topic,"+/%s", app_name);
    ret = xl_channel_set_mqtt_sub_topic(*cxt, 1, &subtopic);
    if(0 != ret)
    {
        printf("xl_channel_set_mqtt_user_ID fail, ret %d\n", ret);
        return ret;
    }

    
    /* 设置默认主题 */
    sprintf(g_destServiceTopic,"%s/%s", app_name, DBCENTER_NAME);
    xl_channel_set_mqtt_default_topic(*cxt, g_destServiceTopic);
    if(0 != ret)
    {
        printf("xl_channel_set_mqtt_default_topic fail, ret %d\n", ret);
        return ret;
    }
    
    ret =  xl_channel_init(*cxt);
    if(0 != ret)
    {
        printf("xl_channel_init fail, ret %d\n", ret);
        return ret;
    }

    while(1)
    {
        status = xl_channel_get_connect_status(*cxt);
        printf("Get mqtt connect status %d\n", status);
        if(INIT == status)
        {
            xl_channel_connect(*cxt);
        }
        else if(CONNECT_ERR == status)
        {
            printf("Get mqtt connect status error(%s), retry!\n", mqttaddr);

            sleep(2);
            xl_channel_connect(*cxt);//不退出，一直重复尝试
        }
        else if(CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }

    return 0;
}

/**********************************************************************
* @name      : app_outmsg_uninit
* @brief     ：对外通信客户端去初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
void db_outmsg_uninit(XL_CHANNEL_CTX_T * cxt)
{
    xl_channel_disconnect(cxt);
    xl_channel_destroy(cxt);
}

/**********************************************************************
* @name      : app_send_mqttmsg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
int db_send_mqttmsg(XL_CHANNEL_CTX_T * cxt, char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    int    mid = 0;
    
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    msglen = appmsg_package(pMsgInfo, &pMsg);
    if(0 == msglen)
    {
        printf("App send appmsg_package fail, IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        return -1;
    }

    if(msglen >= MAX_MSG_BUF_LEN)
    {
        printf("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

	MQTTMSG_BUF_TRACE(pMsg, msglen, "Send mqtt msg pTopic %s :\n", pTopic);

    /* 两端都是MQTT */

    if(NULL != pTopic && 'M' == pMsgInfo->destaddr[0])
    {
        ret = xl_channel_mqtt_write(cxt, pTopic, qos, &mid, (char*)pMsg, (int)msglen);
        //printf("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);
    }
    else
    {
        ret = xl_channel_write(cxt, (char*)pMsg, (int)msglen);
    }

    if(ret > 0)
    {
        DPRINT_MSG(pMsg, msglen, "Send mqtt msg:\n");
    }
    else
    {
        printf("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return (ret > 0) ? 0 : -1;
}

/**********************************************************************
* @name      : app_send_dbmsg
* @brief     ：发送数据中心消息接口
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
int db_send_dbmsg(uint8 thread_id, uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[36] = {0};
    //char            sourname[128] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        printf("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    *pIndex = app_get_next_index();
    pMsgInfo->index = *pIndex;
    pMsgInfo->label = label;
    sprintf(pMsgInfo->souraddr, "%s%u", APP_NAME, thread_id);
    pMsgInfo->sourlen = strlen(pMsgInfo->souraddr);
    sprintf(pMsgInfo->destaddr, "%s", DBCENTER_NAME);
    pMsgInfo->destlen = strlen(pMsgInfo->destaddr);

    pMsgInfo->IID = MESSAGE_IID_DB;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s%u/%s", APP_NAME, thread_id, DBCENTER_NAME);
    ret = db_send_mqttmsg(g_dbMqttMsgCtx[thread_id], uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : app_recv_msg
* @brief     ：对外消息接收, 使用了xl_channel_read 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 
* @Date      ：2020-4-16
* @Update    :
**********************************************************************/
int db_recv_msg(XL_CHANNEL_CTX_T *pctx, uint8 *pBuf, uint32 buflen)
{
    XL_CHANNEL_CONNECT_STATUS_E mqtt_status;
	//int         recvlen = 0;
    //int         ret = 0;

    mqtt_status = xl_channel_get_connect_status(pctx);

    switch (mqtt_status)
    {
        case INIT:
            printf("Mqtt offline, reconnect!\n");
            xl_channel_connect(pctx);
            break;
        case CONNECT_ERR:
            xl_channel_disconnect(pctx);
            xl_channel_connect(pctx);
            printf("Mqtt offline, reconnect!\n");
            break;
        default:
            break;
    }

    if (mqtt_status == CONNECT_SUCC)
    {
        return xl_channel_read(pctx, (sint8 *)pBuf, buflen);
    }
	return 0;
}


