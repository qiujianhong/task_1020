#include "framecomm.h"
#include "mdlcommon.h"
#include "mdlmgr.h"
#include "Mqtt.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "hal.h"
#include "devinfo.h"

// APP 名称主题
const char *appName = "M-mapManager";
const char *regAppName = "mapManager";
char *smiosName     = "M-smiOS";
char *desktopName   = "M-desktopGui";

//appid: 对应app的ID信息
char  AppId[APP_ID_LEN] = {0};

//发送消息锁
pthread_mutex_t g_MsgSendlock;

// mqtt 客户端句柄
MQTT_CLIENT_T *g_MdlMqttClient = NULL;

#define LOGIN_STATE_INIT 0
#define LOGIN_STATE_START 1
#define LOGIN_STATE_WAIT 2
#define LOGIN_STATE_DONE 3

/**
*********************************************************************
* @name      : mqtt_init
* @brief     ：MQTT初始化，直到连接上mqtt
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
*********************************************************************
*/
int mqtt_init(uint32 ip, uint16 port)
{
    int ret = 0;
    uint8 status = 0;
    char   mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[2];
    char  mqttstatus[2][16] = {"connectlost", "success"};
    
    /* 新建客户端 */
    g_MdlMqttClient = mqtt_client_create((char *)appName);
    if (NULL == g_MdlMqttClient)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "mqtt_client_create fail\n");
        return -1;
    }
    
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, "+/M-mapManager", strlen("+/M-mapManager"));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, "M-desktopGui/Broadcast/#", strlen("M-desktopGui/Broadcast/#"));
    ret = mqtt_client_set_sub_topic(g_MdlMqttClient, subtopic, 2);
    if(0 != ret)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "mqtt_client_set_sub_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(g_MdlMqttClient, APP_DEFAULT_TOPIC, 0);
    if(0 != ret)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 链接服务端 */
    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    MDLMGR_FMT_DEBUG(mdlnum, "mqtt connect %s:%d\n", mqttaddr, port);
    ret = mqtt_client_connect(g_MdlMqttClient, mqttaddr, port, "127.0.0.1");
    if(0 != ret)
    {
        MDLMGR_FMT_DEBUG(mdlnum, "mqtt_client_connect fail, ret %d\n", ret);
        goto errorend;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(g_MdlMqttClient);
        MDLMGR_FMT_DEBUG(mdlnum, "Get mqtt connect status %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    MDLMGR_FMT_DEBUG(mdlnum, "mqtt connect %s:%d success\n", mqttaddr, port);

    return 0;

errorend:
    mqtt_client_destroy(g_MdlMqttClient);
    return ret;
}

/*******************************************************************************
* 函数名称: appid_query
* 函数功能: 查询appid
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int appid_query(const char* appname, char* appid, int maxlen)
{
    int ret = -1;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        memset(appid, 0x00, maxlen);
        MDLMGR_FMT_DEBUG(mdlnum, "appid_query() hal_device_get() 获取设备失败! \n");
    }
    else
    {
        if(dev->devinfo_read_appid(dev, appname, appid, maxlen) < 0)
        {
            memset(appid, 0x00, maxlen);
            MDLMGR_FMT_DEBUG(mdlnum, "appid_query() devinfo_read_appid() 获取appid失败! \n");
        }
        else
        {
            ret = 1;
            MDLMGR_FMT_DEBUG(mdlnum, "appid_query() devinfo_read_appid() 获取appid成功! \n");
        }
        MDLMGR_BUF_DEBUG(mdlnum, appid, maxlen, "%s的appid为: ", appname);
        hal_device_release((HW_DEVICE *)dev);
    }

    return ret;
}

/*******************************************************************************
* 函数名称: appid_response
* 函数功能: 应答appid
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void appid_response(MSG_INFO_T *MsgRecv)
{
    if(MsgRecv==NULL)
    {
        return;
    }

    uint8 payload[100] = {0};
    uint8 index = 0;

    payload[index] = APP_ID_LEN;
    index += 1;

    memcpy(&payload[index], AppId, APP_ID_LEN);
    index += APP_ID_LEN;
    
    MSG_INFO_T *pMsgInfoSend = NULL;
    char topic[256] = {0};  //主题
    uint8   SendInfoBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt发送缓冲区 
    int32   SendInfoLen = 0; //发送数据长度
    
    pMsgInfoSend = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T)+index);
    memset(pMsgInfoSend, 0x00, sizeof(MSG_INFO_T)+index);

    //拷贝头部head
    memcpy(pMsgInfoSend, MsgRecv, sizeof(MSG_INFO_T));

    //将头部head的源地址/目的地址清空
    memset(pMsgInfoSend->SourAddr, 0x00, sizeof(pMsgInfoSend->SourAddr));
    memset(pMsgInfoSend->DestAddr, 0x00, sizeof(pMsgInfoSend->DestAddr));

    //消息回复
    pMsgInfoSend->MsgRPM = 0; 
    pMsgInfoSend->SourLen = MsgRecv->DestLen;
    memcpy(pMsgInfoSend->SourAddr, MsgRecv->DestAddr, pMsgInfoSend->SourLen);
    pMsgInfoSend->DestLen = MsgRecv->SourLen;
    memcpy(pMsgInfoSend->DestAddr, MsgRecv->SourAddr, pMsgInfoSend->DestLen);

    //payload赋值
    pMsgInfoSend->MsgLen = index;
    memcpy(&pMsgInfoSend->MsgData[0], payload, pMsgInfoSend->MsgLen);
    
    //消息主题
    memset(topic, 0x00, sizeof(topic));
    sprintf(topic, "%s/%s", pMsgInfoSend->SourAddr, pMsgInfoSend->DestAddr);

    //打印完整消息内容
    MDLMSG_FMT_TRACE("appid_response()查询appid...通过MQTT发送的消息为: (topic:%s) \n", topic);
    PrintInfoDetail(pMsgInfoSend);

    //组消息
    SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);
    MDLMSG_FMT_TRACE("Send Up IPC data... SendInfoLen=%d \n", SendInfoLen);
    MDLMSG_BUF_TRACE(SendInfoBuf, SendInfoLen, "发送mqtt报文:");

    //发送
    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
    
    free(pMsgInfoSend);
    pMsgInfoSend = NULL;

}

/*******************************************************************************
* 函数名称: Appversion_Response
* 函数功能:收到iid 0000 IOP0013后回复版本信息
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void appversion_response(MSG_INFO_T *MsgRecv)
{
    if(MsgRecv==NULL)
    {
        return;
    }
    
    uint8  payload[100]={0};
    uint8  index = 0;
    Date_Time_S publishtime;

    memset(&publishtime,0x00,sizeof(Date_Time_S));

    //版本
    payload[index++] = strlen(APP_VERSION) + 1;
    memcpy(&payload[index], APP_VERSION, strlen(APP_VERSION) + 1);
    index += strlen(APP_VERSION) + 1;

    app_get_maketime(APP_PUBLISH_TIME,&publishtime);
    payload[index++] = publishtime.year_h;
    payload[index++] = publishtime.year_l;
    payload[index++] = publishtime.month;
    payload[index++] = publishtime.day;
    payload[index++] = publishtime.hour;
    payload[index++] = publishtime.min;
    payload[index++] = publishtime.sec;
    
    MSG_INFO_T *pMsgInfoSend = NULL;
    char topic[256] = {0};  //主题
    uint8   SendInfoBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt发送缓冲区 
    int32   SendInfoLen = 0; //发送数据长度
    
    pMsgInfoSend = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T)+index);
    memset(pMsgInfoSend, 0x00, sizeof(MSG_INFO_T)+index);

    //拷贝头部head
    memcpy(pMsgInfoSend, MsgRecv, sizeof(MSG_INFO_T));

    //将头部head的源地址/目的地址清空
    memset(pMsgInfoSend->SourAddr, 0x00, sizeof(pMsgInfoSend->SourAddr));
    memset(pMsgInfoSend->DestAddr, 0x00, sizeof(pMsgInfoSend->DestAddr));

    //消息回复
    pMsgInfoSend->MsgRPM = 0; 
    pMsgInfoSend->SourLen = MsgRecv->DestLen;
    memcpy(pMsgInfoSend->SourAddr, MsgRecv->DestAddr, pMsgInfoSend->SourLen);
    pMsgInfoSend->DestLen = MsgRecv->SourLen;
    memcpy(pMsgInfoSend->DestAddr, MsgRecv->SourAddr, pMsgInfoSend->DestLen);

    //payload赋值
    pMsgInfoSend->MsgLen = index;
    memcpy(&pMsgInfoSend->MsgData[0], payload, pMsgInfoSend->MsgLen);
    
    //消息主题
    memset(topic, 0x00, sizeof(topic));
    sprintf(topic, "%s/%s", pMsgInfoSend->SourAddr, pMsgInfoSend->DestAddr);

    //组消息
    SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);

    //打印完整消息内容
    MDLMSG_FMT_DEBUG("发送app版本信息 源APP[%s] 目的APP[%s]\n", pMsgInfoSend->SourAddr, pMsgInfoSend->DestAddr);
    MDLMSG_BUF_DEBUG(SendInfoBuf, SendInfoLen, "发送版本消息");
    PrintInfoDetail(pMsgInfoSend);
    
    //发送
    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);

    free(pMsgInfoSend);
    pMsgInfoSend = NULL;
}

void login_to_smiOS(void)
{
    uint8  payload[200] = {0};
    uint8  index = 0;
    uint8  len;
    uint8  SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;

    int pid = getpid();
    appmsg_memcpy_r(&payload[index], (uint8*)&pid,4);
    index += 4;

    len = strlen(appName);
    payload[index++] = len;
    memcpy(&payload[index], appName, len);
    index += len;

    payload[index++] = 2;
    index += 3;
    payload[index++] = 0x2e;		//订阅系统停电、停电、复位、校时事件
    index ++;
    payload[index] = 0x07;
    index += 2;
    payload[index] = 0x06;			//桌面消息激活事件、轮显通知事件
    index ++;

    //版本信息
    payload[index++] = strlen(APP_VERSION);
    memcpy_r(&payload[index], APP_VERSION, strlen(APP_VERSION));
    index += strlen(APP_VERSION);

    //发布日期
    Date_Time_S publishtime;
    memset(&publishtime, 0x00, sizeof(Date_Time_S));
    app_get_maketime(APP_PUBLISH_TIME, &publishtime);
    payload[index++] = publishtime.year_h;
    payload[index++] = publishtime.year_l;
    payload[index++] = publishtime.month;
    payload[index++] = publishtime.day;
    payload[index++] = publishtime.hour;
    payload[index++] = publishtime.min;
    payload[index++] = publishtime.sec;

    //appid信息
    payload[index] = APP_ID_LEN;
    index ++;

    memcpy(&payload[index], AppId, APP_ID_LEN);
    index += APP_ID_LEN;
    
    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+index);
    memset(MsgSend, 0x00, sizeof(MSG_INFO_T) + index);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgIndex = GetInfoIndex();
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0001;
    MsgSend->IOP = 0x0010;
    MsgSend->SourLen = strlen(appName);
    memcpy(MsgSend->SourAddr, appName, MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr, smiosName, MsgSend->DestLen );

    MsgSend->MsgLen = index;
    memcpy(MsgSend->MsgData, payload, index);
    SendLen = msg_mqtt_package(MsgSend, MAX_QUEUE_BUFF_LEN, SendBuf);

    MDLMSG_FMT_DEBUG("注册系统管理器 源APP[%s] 目的APP[%s] PID[%d]\n", MsgSend->SourAddr, MsgSend->DestAddr, pid);
    MDLMSG_BUF_DEBUG(SendBuf, SendLen, "发送注册消息");

    mqtt_topic_write(SendBuf, SendLen, APP_DEFAULT_TOPIC);
    free(MsgSend);
}

void heart_beat(uint16 index)
{
    uint8  payload[200] = {0};
    uint8  offset = 0;
    uint32 value = 1;
    uint8  SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;

    appmsg_memcpy_r(payload, (uint8*)&value, 4);
    offset+=4;

    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+4);
    memset(MsgSend, 0x00, sizeof(MSG_INFO_T)+4);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgIndex = index;
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0000;
    MsgSend->IOP = 0x0010;
    
    MsgSend->SourLen = strlen(appName);
    memcpy(MsgSend->SourAddr, appName, MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr, smiosName, MsgSend->DestLen);
    MsgSend->MsgLen = offset;
    memcpy(MsgSend->MsgData, payload, offset);
    SendLen = msg_mqtt_package(MsgSend, MAX_QUEUE_BUFF_LEN, SendBuf);

    MDLMSG_BUF_TRACE(SendBuf, SendLen, "heat_beat_send:");
    mqtt_topic_write(SendBuf, SendLen, APP_DEFAULT_TOPIC);
    free(MsgSend);
}

int mqtt_recv_msg(uint8* RecvBuf, uint16 datalen)
{
#if 1    
    char          topic[256];
    int           qos;
    return mqtt_client_recv_msg(g_MdlMqttClient, topic, 256, &qos, RecvBuf, (int)datalen, 1000);
#else
    return mqtt_client_recv(g_AdvMqttClient, pBuf, (int)buflen);
#endif
}

void mqtt_write(uint8*SendBuf,uint32 SendLen )
{
    mqtt_client_send(g_MdlMqttClient, SendBuf, SendLen);
}

int mqtt_topic_write(uint8* SendBuf,uint32 SendLen,char* topic)
{
    return mqtt_client_send_msg(g_MdlMqttClient, topic, 0, SendBuf, SendLen);
}

/**********************************************************************
* @name      : mdlmgr_send_msg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int mdlmgr_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
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
        MDLMSG_FMT_DEBUG("App send appmsg_package fail, IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        return -1;
    }

    if(msglen >= 5120)
    {
        MDLMSG_FMT_DEBUG("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

    /* 两端都是MQTT */
    pthread_mutex_lock(&g_MsgSendlock);
    if(NULL != pTopic)
    {
        ret = mqtt_client_send_msg(g_MdlMqttClient, pTopic, qos, pMsg, (int)msglen);
        MDLMSG_FMT_TRACE("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);
    }
    else
    {
        ret = mqtt_client_send(g_MdlMqttClient, pMsg, (int)msglen);
    }
    pthread_mutex_unlock(&g_MsgSendlock);


    if(0 == ret)
    {
        MDLMSG_BUF_TRACE(pMsg, msglen, "mqtt_SEND:");
    }
    else
    {
        MDLMSG_FMT_DEBUG("mqtt_SEND failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                        pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}