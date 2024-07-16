
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#include "ac_mqtt.h"
#include "ac_lib.h"
#include "ac_main.h"
#include "mqtt_client.h"


/*******************************************************************************
* 交采APP MQTT通信参数 *********************------<测试修改>------**********
*******************************************************************************/

#define MQTT_COMM_TYPE  0   //0:容器，1:非容器

//连接mqtt broker 资源地址 协议类型://ip地址:端口------<测试修改>------
#if MQTT_COMM_TYPE == 0
const char *mqttUrl = "mqtt://172.17.0.1:1883";  //容器
#else
const char *mqttUrl = "mqtt://127.0.0.1:1883";  //非容器
#endif

// APP 名称 同时用作MQTT 用户ID------<测试修改>------
char *appName = "M-acMeter";
#define APP_BORADTOPIC     "+/Broadcast/#"

//接收数据的topic 名称------<测试修改>------
char *srcServiceTopic = "+/M-acMeter";

//系统管理器APP名称------<测试修改>------
char *smiosName = "M-smiOS";

//数据中心APP名称------<测试修改>------
char *dbName = "M-dbCenter";

//模组管理器APP名称------<测试修改>------
char *mapManagerName = "M-mapManager";

#define FAC_VERSION   "XL01"            //厂商版本号
#define FAC_CODE  "XLDZ"                //厂商代码
#define FAC_PUB_TIME  "XL-2020-05-30"    //发布日期


// mqtt 客户端句柄
MQTT_CLIENT_T *g_MqttClient;

// mqtt 连接状态
MQTT_STATUS_E s_mqtt_status;


#define LOGIN_STATE_INIT 0
#define LOGIN_STATE_START 1
#define LOGIN_STATE_WAIT 2
#define LOGIN_STATE_DONE 3

// 初始化标志
uint8 gMqttInitFlag = 0;  //1:初始化完成

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
int ac_advance_mqtt_reproc(void* pMsg, int msglen)
{
    int ret = 0;
    MESSAGE_INFO_T* pMsgInfo = NULL;

    appmsg_split((uint8*)pMsg, (uint16)msglen, &pMsgInfo);
    if (NULL == pMsgInfo)
    {
        return 1;
    }

    ret = db_agent_msg_entry(gDBAgent, pMsgInfo);
    if (DB_AGENT_UNKONW_MSG == ret || DB_AGENT_DROP_MSG == ret)
    {
        /* 加锁失败放到队列中继续执行一次 */
        free(pMsgInfo);
        return -1;
    }
    ACMQTT_BUF_TRACE(pMsg, msglen, "Recv msg from %s:", pMsgInfo->souraddr);
    if (0 != ret)
    {
        ACMQTT_BUF_DEBUG(pMsg, msglen, "Drop msg from %s, ret %d:", pMsgInfo->souraddr, ret);
    }
    free(pMsgInfo);
    return 0;
}

/*********************************************************************
 功能描述：初始化
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int mqtt_init()
{
    int ret = -1;
    
    g_MqttClient = mqtt_client_create(appName);
    if(NULL == g_MqttClient)
    {
        ACMQTT_FMT_TRACE("mqtt_init() mqtt_client_create fail \n");
        return -1;
    }

    /* 订阅主题 */
    SUB_TOPIC_T topic[2];
    memset(topic, 0x00, sizeof(topic));
    topic[0].qos = 0;
    sprintf(topic[0].topic, "+/%s", appName);
    topic[1].qos = 0;
    memcpy(topic[1].topic, APP_BORADTOPIC, strlen(APP_BORADTOPIC));
    
    ret = mqtt_client_set_sub_topic(g_MqttClient, topic, 2);
    if(ret!=0)
    {
        ACMQTT_FMT_TRACE("mqtt_init() mqtt_client_set_sub_topic fail \n");
        mqtt_client_destroy(g_MqttClient);
        return -1;
    }

    /* 增加预处理处理函数 */
    ret = mqtt_msg_add_pre_porc(g_MqttClient, ac_advance_mqtt_reproc);
    if (0 != ret)
    {
        ACMQTT_FMT_DEBUG("mqtt_msg_add_pre_porc fail, ret %d\n", ret);
        return -1;
    }

    ret = mqtt_client_connect(g_MqttClient, mqttIP, mqttport, "127.0.0.1");
    if(ret!=0)
    {
        ACMQTT_FMT_TRACE("mqtt_init() mqtt_client_connect fail \n");
        mqtt_client_destroy(g_MqttClient);
        return -1;
    }
    while(s_mqtt_status!=MQTT_CONNECT_SUCC)
    {
        s_mqtt_status = mqtt_client_get_status(g_MqttClient);
        sleep(1);
    }

    return ret;
}

/*********************************************************************
 功能描述：系统管理器复位终端
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void reboot_to_smiOS(void)
{
    uint8           buf[512] = {0};
    MESSAGE_INFO_T *pMsgInfo = (MESSAGE_INFO_T*)buf;
    char           *smios = "M-smiOS";
    uint8          *pMsg = NULL;
    uint16          msglen = 0;

    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T));
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    pMsgInfo->index = 0;
    pMsgInfo->label = 0;
    
    pMsgInfo->sourlen = strlen("M-acMeter");
    memcpy(pMsgInfo->souraddr, "M-acMeter", pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = SYSTEM_IOP_REQUIRE_RESTART;
    pMsgInfo->msglen = 1;
    pMsgInfo->playload[0] = 2;
    
    msglen = appmsg_package(pMsgInfo, &pMsg);

    if(pMsg > 0)
    {
        ACMQTT_FMT_DEBUG("请求终端重启\n");
        mqtt_client_send_msg(g_MqttClient, "M-acMeter/M-smiOS", 0, pMsg, msglen);
        free(pMsg);
    }
}

/*********************************************************************
 功能描述：系统管理器注册
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void login_to_smiOS(void)
{
    uint8 payload[200] = {0};
    uint8 index = 0;
    uint8 len;
    uint8 SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;
    uint16 eventiid = 0;
    int pid = getpid();
    ACMQTT_FMT_TRACE("当前进程号%d \n",pid);
    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",appName,smiosName);
    memcpy_r(&payload[index],(uint8*)&pid,4);
    index+=4;
    len = strlen(appName);
    payload[index++] = len;
    memcpy(&payload[index],appName,len);
    index+=len;
    payload[index++] = 3;  //订阅事件个数

    eventiid = 0x0000;
    memcpy_r(&payload[index],(uint8*)&eventiid,2);
    index+=2;
    payload[index++]=0;
    payload[index++]=0x20;//订阅事件5 00100000

    eventiid = 0x0004;
    memcpy_r(&payload[index],(uint8*)&eventiid,2);
    index+=2;
    payload[index++]=0;
    payload[index++]=0x02;//订阅事件1 00000010

    eventiid = 0x0005;
    memcpy_r(&payload[index],(uint8*)&eventiid,2);
    index+=2;
    payload[index++]=0;
    payload[index++]=0x06;//订阅事件1,2 00000110
    
    //版本信息
    payload[index++] = strlen(APP_VERSION);
    memcpy_r(&payload[index], APP_VERSION, strlen(APP_VERSION));
    index += strlen(APP_VERSION);

    //发布日期
    Date_Time_S publishtime;
    memset(&publishtime,0x00,sizeof(Date_Time_S));
    app_get_maketime(APP_PUBLISH_TIME,&publishtime);
    payload[index++]=publishtime.year_h;
    payload[index++]=publishtime.year_l;
    payload[index++]=publishtime.month;
    payload[index++]=publishtime.day;
    payload[index++]=publishtime.hour;
    payload[index++]=publishtime.min;
    payload[index++]=publishtime.sec;

    //appid信息
    payload[index] = APP_ID_LEN;
    index += 1;

    memcpy(&payload[index], AppId, APP_ID_LEN);
    index += APP_ID_LEN;
    
    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MSG_INFO_T)+index);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgIndex = GetInfoIndex();
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0001;
    MsgSend->IOP = 0x0010;
    
    MsgSend->SourLen = strlen(appName);
    memcpy(MsgSend->SourAddr,appName,MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr,smiosName,MsgSend->DestLen );
    MsgSend->MsgLen = index;
    memcpy(MsgSend->MsgData,payload,index);
    SendLen = msg_mqtt_package(MsgSend,MAX_QUEUE_BUFF_LEN,SendBuf);

    //打印完整消息内容
    ACMQTT_FMT_TRACE("login_to_smiOS() 登陆smiOS...通过MQTT发送的消息为: (topic:%s) \n", topic);
    PrintInfoDetail(MsgSend);
    ACMQTT_BUF_TRACE(SendBuf, SendLen, "完整发送:");
    mqtt_client_send_msg(g_MqttClient, topic, 0, SendBuf, SendLen);
    free(MsgSend);

}

/*********************************************************************
 功能描述：心跳回复
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void heart_beat(uint16 index)
{
    uint8 payload[200] = {0};
    uint8 offset = 0;
    uint32 value = 12;
    uint8 SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;
    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",appName,smiosName);
    memcpy_r(payload, (uint8*)&value, 4);
    offset+=4;

    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+4);
    memset(MsgSend,0x00,sizeof(MSG_INFO_T)+4);
    MsgSend->MsgRPM = 0;
    MsgSend->MsgIndex = index;
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0000;
    MsgSend->IOP = 0x0010;
    
    MsgSend->SourLen = strlen(appName);
    memcpy(MsgSend->SourAddr,appName,MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr,smiosName,MsgSend->DestLen );
    MsgSend->MsgLen = offset;
    memcpy(MsgSend->MsgData,payload,offset);
    SendLen = msg_mqtt_package(MsgSend,MAX_QUEUE_BUFF_LEN,SendBuf);

    //打印完整消息内容
    ACMQTT_FMT_TRACE("heart_beat() 心跳回复...通过MQTT发送的消息为 (topic:%s) \n", topic);
    PrintInfoDetail(MsgSend);
    ACMQTT_BUF_TRACE(SendBuf, SendLen, "完整发送:");
    mqtt_client_send_msg(g_MqttClient, topic, 0, SendBuf, SendLen);
    free(MsgSend);
   
}

//从模组管理器查询模组信息
void module_from_mapManager(uint8 *pPayloadData, uint32 nPayloadLen, uint16 *IID, uint16 *IOP, uint16 *index)
{
    uint8 offset = 0;
    uint8 SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;
    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",appName,mapManagerName);

    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T) + nPayloadLen);
    memset(MsgSend,0x00,sizeof(MSG_INFO_T) + nPayloadLen);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgIndex = GetInfoIndex();
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0004;
    MsgSend->IOP = 0x0010;

    *IID = MsgSend->IID;
    *IOP = MsgSend->IOP;
    *index = MsgSend->MsgIndex;
    
    MsgSend->SourLen = strlen(appName);
    memcpy(MsgSend->SourAddr,appName,MsgSend->SourLen);
    MsgSend->DestLen = strlen(mapManagerName);
    memcpy(MsgSend->DestAddr,mapManagerName,MsgSend->DestLen );
    
    MsgSend->MsgLen = nPayloadLen;
    memcpy(&MsgSend->MsgData[offset], pPayloadData, nPayloadLen);
    SendLen = msg_mqtt_package(MsgSend, MAX_QUEUE_BUFF_LEN, SendBuf);

    //打印完整消息内容
    ACMQTT_FMT_TRACE("module_from_mapManager() 查询模组信息...通过MQTT发送的消息为 (topic:%s) \n", topic);
    PrintInfoDetail(MsgSend);
    ACMQTT_BUF_TRACE(SendBuf, SendLen, "完整发送:");
    mqtt_client_send_msg(g_MqttClient, topic, 0, SendBuf, SendLen);
    free(MsgSend);
}


/*********************************************************************
 功能描述：通过mqtt向数据中心发送数据
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int info_to_dbCenter(uint16 label, uint16 iop, uint8* payload, uint16 len, uint16* pindex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 *buf = NULL;
    uint32 SendLen;
    uint16 msglen = 0;
    int ret; 
    char topic[100];
    sprintf(topic,"%s/%s", appName, dbName);
    msglen = sizeof(MESSAGE_INFO_T) + len;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(msglen);
    if(pMsgInfo == NULL)
    {
        return -1;
    }
    memset(pMsgInfo,0x00,msglen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority =1;
    *pindex = GetInfoIndex();
    pMsgInfo->index = *pindex;
    pMsgInfo->label = label;
    pMsgInfo->sourlen = strlen(appName);
    memcpy(pMsgInfo->souraddr,appName,pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(dbName);
    memcpy(pMsgInfo->destaddr,dbName,pMsgInfo->destlen);
    pMsgInfo->IID = 0X0005;
    pMsgInfo->IOP = iop;
    pMsgInfo->msglen = len;
    memcpy(pMsgInfo->playload,payload,len);

    SendLen = appmsg_package(pMsgInfo, &buf);
    if(SendLen == 0)
    {
        return -1;
    }
    
    //打印完整消息内容
    ACMQTT_FMT_LOGCC("info_to_dbCenter() 数据中心读写...通过MQTT发送的消息为 (topic:%s) \n", topic);
    PrintInfoDetailDB(pMsgInfo);
    ACMQTT_BUF_LOGCC(buf, SendLen, "报文:");
    
    ret = mqtt_topic_write(buf,SendLen,topic);
    free(buf);
    free(pMsgInfo);
    if(ret>0)
        return 0;
    else
        return ret;
    
}

/*******************************************************************************
* 函数名称: Appversion_Response
* 函数功能:收到iid 0000 IOP0013后回复版本信息
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void Appversion_Response(MSG_INFO_T *MsgRecv)
{
    if(MsgRecv==NULL)
    {
        return;
    }
    
    uint8 payload[100]={0};
    uint8 index = 0;
    Date_Time_S publishtime;
    memset(&publishtime,0x00,sizeof(Date_Time_S));

    //版本
    payload[index++] = strlen(APP_VERSION);
    memcpy(&payload[index], APP_VERSION, strlen(APP_VERSION));
    index += strlen(APP_VERSION);

    app_get_maketime(APP_PUBLISH_TIME,&publishtime);
    payload[index++]=publishtime.year_h;
    payload[index++]=publishtime.year_l;
    payload[index++]=publishtime.month;
    payload[index++]=publishtime.day;
    payload[index++]=publishtime.hour;
    payload[index++]=publishtime.min;
    payload[index++]=publishtime.sec;
    
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
    ACMQTT_FMT_TRACE("Appversion_Response()查询app版本信息...通过MQTT发送的消息为: (topic:%s) \n", topic);
    PrintInfoDetail(pMsgInfoSend);

    //组消息
    SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);

    //发送
    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
    
    free(pMsgInfoSend);
    pMsgInfoSend = NULL;

}

/*******************************************************************************
* 函数名称: Factoryversion_Response
* 函数功能:收到iid 1155 IOP9090后回复厂商版本信息
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void Factoryversion_Response(MSG_INFO_T *MsgRecv)
{
    if(MsgRecv==NULL)
    {
        return;
    }
    
    uint8 payload[200]={0};
    uint8 index = 0;
    uint8 len;
    len = strlen(appName);
    payload[index++]=len;
    memcpy(&payload[index],appName,len);
    index+=len;
    
    len = strlen(FAC_VERSION);
    payload[index++]=len;
    memcpy(&payload[index],FAC_VERSION,len);
    index+=len;

    len = strlen(FAC_CODE);
    payload[index++]=len;
    memcpy(&payload[index],FAC_CODE,len);
    index+=len;

    len = strlen(FAC_PUB_TIME);
    payload[index++]=len;
    memcpy(&payload[index],FAC_PUB_TIME,len);
    index+=len;

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
    ACMQTT_FMT_TRACE("Factoryversion_Response()查询厂商版本信息...通过MQTT发送的消息为: (topic:%s) \n", topic);
    PrintInfoDetail(pMsgInfoSend);

    //组消息
    SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);

    //发送
    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
    
    free(pMsgInfoSend);
    pMsgInfoSend = NULL;
    
}

/*********************************************************************
 功能描述： 组装应答报文
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
int make_resp_info(MSG_INFO_T *ptMsgInfo, uint16 BuffLen, uint8 *Buff)
{
    MSG_INFO_T *pMsgInfoSend = NULL;
    char topic[256] = {0};  //主题
    uint8   SendInfoBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt发送缓冲区 
    int32   SendInfoLen = 0; //发送数据长度
    
    pMsgInfoSend = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T)+BuffLen);
    memset(pMsgInfoSend, 0x00, sizeof(MSG_INFO_T)+BuffLen);
    
    if(ptMsgInfo==NULL)
    {
        ACMQTT_FMT_TRACE("make_resp_info()...if(ptMsgInfo==NULL) \n");
        return -1;
    }

    //拷贝头部head
    memcpy(pMsgInfoSend, ptMsgInfo, sizeof(MSG_INFO_T));

    //将头部head的源地址/目的地址清空
    memset(pMsgInfoSend->SourAddr, 0x00, sizeof(pMsgInfoSend->SourAddr));
    memset(pMsgInfoSend->DestAddr, 0x00, sizeof(pMsgInfoSend->DestAddr));

    //消息回复
    pMsgInfoSend->MsgRPM = 0; 
    pMsgInfoSend->SourLen = ptMsgInfo->DestLen;
    memcpy(pMsgInfoSend->SourAddr, ptMsgInfo->DestAddr, pMsgInfoSend->SourLen);
    pMsgInfoSend->DestLen = ptMsgInfo->SourLen;
    memcpy(pMsgInfoSend->DestAddr, ptMsgInfo->SourAddr, pMsgInfoSend->DestLen);

    //payload赋值
    pMsgInfoSend->MsgLen = BuffLen;
    memcpy(&pMsgInfoSend->MsgData[0], Buff, pMsgInfoSend->MsgLen);
    
    //消息主题
    memset(topic, 0x00, sizeof(topic));
    sprintf(topic, "%s/%s", pMsgInfoSend->SourAddr, pMsgInfoSend->DestAddr);

    //打印完整消息内容
    ACMQTT_FMT_TRACE("make_resp_info()...通过MQTT发送的消息为: (topic:%s) \n", topic);
    PrintInfoDetail(pMsgInfoSend);

    //组消息
    SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);

    //发送
    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
    
    free(pMsgInfoSend);
    pMsgInfoSend = NULL;

    return 1;
}

/*********************************************************************
 功能描述：查询关注的app是否注册成功
 输入参数：
 输出参数：无
 函数返回值：无
********************************************************************/
void check_app_login_state(char* AppName, uint16 *SendIndex)
{
    uint8 payload[200] = {0};
    uint8 offset = 0;
    uint8 SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;
    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",appName,smiosName);

    payload[offset++] = strlen(AppName);
    memcpy(&payload[offset], AppName, strlen(AppName));
    offset += strlen(AppName);

    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+offset);
    memset(MsgSend,0x00,sizeof(MSG_INFO_T)+4);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgIndex = GetInfoIndex();
    *SendIndex = MsgSend->MsgIndex;
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0001;
    MsgSend->IOP = 0x0013;
    
    MsgSend->SourLen = strlen(appName);
    memcpy(MsgSend->SourAddr, appName, MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr, smiosName, MsgSend->DestLen);
    MsgSend->MsgLen = offset;
    memcpy(MsgSend->MsgData, payload, offset);
    SendLen = msg_mqtt_package(MsgSend, MAX_QUEUE_BUFF_LEN, SendBuf);

    //打印完整消息内容
    ACMQTT_FMT_TRACE("check_app_login_state() 检查 %s 是否已注册...通过MQTT发送的消息为 (topic:%s) \n", AppName, topic);
    PrintInfoDetail(MsgSend);
    ACMQTT_BUF_TRACE(SendBuf, SendLen, "完整发送:");
    mqtt_client_send_msg(g_MqttClient, topic, 0, SendBuf, SendLen);
    free(MsgSend);
}

/*******************************************************************************
* 函数名称: unpack_login_info
* 函数功能: 查询app注册状态的接收信息
* 输入参数:
* 输出参数: 
* 返 回 值:  
*******************************************************************************/
int unpack_login_info(MSG_INFO_T *MsgRecv, char *AppName, uint16 SendIndex)
{
    int offset = 0;
    uint8 NameLen = 0;
    char ResultName[100] = {0};
    
    if(MsgRecv->MsgIndex == SendIndex)
    {
        if(MsgRecv->MsgLen > 0)
        {
            NameLen = MsgRecv->MsgData[offset];
            offset += 1;

            if(MsgRecv->MsgLen > NameLen)
            {
                memcpy(ResultName, &MsgRecv->MsgData[offset], NameLen);

                if(0 == strcmp(AppName, ResultName))
                {
                    ACMQTT_FMT_TRACE("unpack_login_info()查询app注册状态: AppName=%s已注册完成! \n", AppName);
                    return 1;
                }
            }
        }
    }

    ACMQTT_FMT_TRACE("unpack_login_info()查询app注册状态: AppName=%s失败! MsgRecv->MsgIndex=%d, SendIndex=%d, MsgRecv->MsgLen=%d\n", 
        AppName, MsgRecv->MsgIndex, SendIndex, MsgRecv->MsgLen);
    return 0;
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
        ACMQTT_FMT_TRACE("appid_query() hal_device_get() 获取设备失败! \n");
    }
    else
    {
        if(dev->devinfo_read_appid(dev, appname, appid, maxlen) < 0)
        {
            memset(appid, 0x00, maxlen);
            ACMQTT_FMT_TRACE("appid_query() devinfo_read_appid() 获取appid失败! \n");
        }
        else
        {
            ret = 1;
            ACMQTT_FMT_TRACE("appid_query() devinfo_read_appid() 获取appid成功! \n");
        }
        ACMQTT_BUF_TRACE(appid, maxlen, "%s的appid为: ", appname);
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
    ACMQTT_FMT_TRACE("appid_response()查询appid...通过MQTT发送的消息为: (topic:%s) \n", topic);
    PrintInfoDetail(pMsgInfoSend);

    //组消息
    SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);
    ACMQTT_FMT_TRACE("Send Up IPC data... SendInfoLen=%d \n", SendInfoLen);
    ACMQTT_BUF_TRACE(SendInfoBuf, SendInfoLen, "发送mqtt报文:");

    //发送
    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
    
    free(pMsgInfoSend);
    pMsgInfoSend = NULL;

}


int mqtt_read(uint8* RecvBuf)
{
    return mqtt_client_recv(g_MqttClient,RecvBuf, MAX_QUEUE_BUFF_LEN);
}

int mqtt_topic_read(char* topic, int *qos, int *mid, uint8* RecvBuf)
{
    
    return mqtt_client_recv_msg(g_MqttClient, topic, strlen(topic), qos, RecvBuf, MAX_QUEUE_BUFF_LEN, 0);
}

int mqtt_topic_write(uint8* SendBuf, uint32 SendLen, char* topic)
{

    return mqtt_client_send_msg(g_MqttClient, topic, 0, SendBuf, SendLen);
}


/*******************************************************************************
* 函数名称: PrintInfoDetail
* 函数功能: 打印完整消息（head+msg)
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void PrintInfoDetail(MSG_INFO_T *stMsgInfoSend)
{
    uint32 IP;        //目的端ip
    uint16 port;
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128] = {0};
    char   DestAddr[128] = {0};

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[65535] = {0};

    IP = stMsgInfoSend->IP;
    port = stMsgInfoSend->port;

    MsgRPM = stMsgInfoSend->MsgRPM;
    MsgPRIORITY = stMsgInfoSend->MsgPRIORITY;
    MsgIndex = stMsgInfoSend->MsgIndex;
    MsgLabel = stMsgInfoSend->MsgLabel;
    SourLen = stMsgInfoSend->SourLen;
    DestLen = stMsgInfoSend->DestLen;
    memcpy(SourAddr, stMsgInfoSend->SourAddr, SourLen);
    memcpy(DestAddr, stMsgInfoSend->DestAddr, DestLen);
    IID = stMsgInfoSend->IID;
    IOP = stMsgInfoSend->IOP;
    MsgLen = stMsgInfoSend->MsgLen;
    memcpy(MsgData, stMsgInfoSend->MsgData, MsgLen);

    ACMQTT_FMT_TRACE("消息的完整内容为: IP:%x, port:%d, MsgRPM:%d, MsgPRIORITY:%d, MsgIndex:%d, MsgLabel:%d, SourLen:%d, DestLen:%d, IID:%04x, IOP:%04x, MsgLen:%d. \n", 
        IP, port, MsgRPM, MsgPRIORITY,MsgIndex, MsgLabel, SourLen, DestLen, IID, IOP, MsgLen);
    ACMQTT_FMT_TRACE("SourAddr: %s\n", SourAddr);
    ACMQTT_BUF_TRACE((uint8*)SourAddr, SourLen, "SourAddr:");
    ACMQTT_FMT_TRACE("DestAddr: %s\n", DestAddr);
    ACMQTT_BUF_TRACE((uint8*)DestAddr, DestLen, "DestAddr:");
    ACMQTT_FMT_TRACE("MsgData: \n");
    ACMQTT_BUF_TRACE((uint8*)MsgData, MsgLen, "报文:");
    ACMQTT_FMT_TRACE("消息的完整内容打印完毕. \n");
}

/*******************************************************************************
* 函数名称: PrintInfoDetail
* 函数功能: 打印完整消息（head+msg)
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void PrintInfoDetail_DEBUG(MSG_INFO_T *stMsgInfoSend)
{
    uint32 IP;        //目的端ip
    uint16 port;
    
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128] = {0};
    char   DestAddr[128] = {0};

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[65535] = {0};

    IP = stMsgInfoSend->IP;
    port = stMsgInfoSend->port;

    MsgRPM = stMsgInfoSend->MsgRPM;
    MsgPRIORITY = stMsgInfoSend->MsgPRIORITY;
    MsgIndex = stMsgInfoSend->MsgIndex;
    MsgLabel = stMsgInfoSend->MsgLabel;
    SourLen = stMsgInfoSend->SourLen;
    DestLen = stMsgInfoSend->DestLen;
    memcpy(SourAddr, stMsgInfoSend->SourAddr, SourLen);
    memcpy(DestAddr, stMsgInfoSend->DestAddr, DestLen);
    IID = stMsgInfoSend->IID;
    IOP = stMsgInfoSend->IOP;
    MsgLen = stMsgInfoSend->MsgLen;
    memcpy(MsgData, stMsgInfoSend->MsgData, MsgLen);

    ACMQTT_FMT_DEBUG("消息的完整内容为: IP:%x, port:%d, MsgRPM:%d, MsgPRIORITY:%d, MsgIndex:%d, MsgLabel:%d, SourLen:%d, DestLen:%d, IID:%04x, IOP:%04x, MsgLen:%d. \n", 
        IP, port, MsgRPM, MsgPRIORITY,MsgIndex, MsgLabel, SourLen, DestLen, IID, IOP, MsgLen);
    ACMQTT_FMT_DEBUG("SourAddr: %s\n", SourAddr);
    ACMQTT_BUF_DEBUG((uint8*)SourAddr, SourLen, "SourAddr:");
    ACMQTT_FMT_DEBUG("DestAddr: %s\n", DestAddr);
    ACMQTT_BUF_DEBUG((uint8*)DestAddr, DestLen, "DestAddr:");
    ACMQTT_FMT_DEBUG("MsgData: \n");
    ACMQTT_BUF_DEBUG((uint8*)MsgData, MsgLen, "报文:");
    ACMQTT_FMT_DEBUG("消息的完整内容打印完毕. \n");
}

/*******************************************************************************
* 函数名称: PrintInfoDetail
* 函数功能: 打印完整消息（head+msg)
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void print_info_msg(MSG_INFO_T *stMsgInfoSend)
{
    switch (stMsgInfoSend->IID)
    {
    case 0x0001:
    case 0x0009:
    {
        ACMQTT_FMT_DEBUG("通过Mqtt收到的消息为:\n");
        PrintInfoDetail_DEBUG(stMsgInfoSend);
        break;
    }
    default:
    {
        ACMQTT_FMT_TRACE("通过Mqtt收到的消息为:\n");
        PrintInfoDetail(stMsgInfoSend);
        break;
    }
    }
}

/*******************************************************************************
* 函数名称: PrintInfoDetail
* 函数功能: 打印完整消息（head+msg)
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
void PrintInfoDetailDB(MESSAGE_INFO_T *pMsgInfo)
{
    uint8  MsgRPM;         /* 启动标志位, 1-启动, 0-非启动 */
    uint8  MsgPRIORITY;    /* 优先级 */
    uint16 MsgIndex;       /* 消息序号 */
    uint16 MsgLabel;       /* 消息标签 */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128] = {0};
    char   DestAddr[128] = {0};

    uint16 IID;
    uint16 IOP;

    uint32 MsgLen; 
    uint8  MsgData[65535] = {0};

    MsgRPM = pMsgInfo->rpm;
    MsgPRIORITY = pMsgInfo->priority;
    MsgIndex = pMsgInfo->index;
    MsgLabel = pMsgInfo->label;
    SourLen = pMsgInfo->sourlen;
    DestLen = pMsgInfo->destlen;
    memcpy(SourAddr, pMsgInfo->souraddr, SourLen);
    memcpy(DestAddr, pMsgInfo->destaddr, DestLen);
    IID = pMsgInfo->IID;
    IOP = pMsgInfo->IOP;
    MsgLen = pMsgInfo->msglen;
    memcpy(MsgData, pMsgInfo->playload, MsgLen);

    ACMQTT_FMT_LOGCC("消息的完整内容为: MsgRPM:%d, MsgPRIORITY:%d, MsgIndex:%d, MsgLabel:%d, SourLen:%d, DestLen:%d, IID:%04x, IOP:%04x, MsgLen:%d. \n", 
        MsgRPM, MsgPRIORITY,MsgIndex, MsgLabel, SourLen, DestLen, IID, IOP, MsgLen);
    ACMQTT_FMT_LOGCC("SourAddr: %s\n", SourAddr);
    ACMQTT_BUF_LOGCC((uint8*)SourAddr, SourLen, "SourAddr:");
    ACMQTT_FMT_LOGCC("DestAddr: %s\n", DestAddr);
    ACMQTT_BUF_LOGCC((uint8*)DestAddr, DestLen, "DestAddr:");
    ACMQTT_FMT_LOGCC("MsgData: \n");
    ACMQTT_BUF_LOGCC((uint8*)MsgData, MsgLen, "报文:");
    ACMQTT_FMT_LOGCC("消息的完整内容打印完毕. \n");
    
}



/*******************************************************************************
* 函数名称: GetInfoIndex
* 函数功能: 主动上送消息的index
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
uint16 GetInfoIndex()
{
    uint16 index =0 ;
    pthread_mutex_lock(&ActiveInfoIndexLock);
    index = ActiveInfoIndex++;
    pthread_mutex_unlock(&ActiveInfoIndexLock);
    return index;
}

/**********************************************************************
* @name      : mqtt_msg_send
* @brief     ：对外消息发送
* @param[in] ：MESSAGE_MQTT_T *pMsgMqtt  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int mqtt_msg_send(MQTT_CLIENT_T *mqttClient, MESSAGE_MQTT_T *pMsgMqtt, int qos)
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
            ACMQTT_FMT_DEBUG("App send appmsg_axdr_package fail\n");
            return -2;
        }
        #ifdef PRODUCT_SCU
        sprintf(topic, "%s/%s/%s", pMsgMqtt->tAXDR.souraddr, pMsgMqtt->tAXDR.destaddr, MQTT_TOPIC_A_XDR);
        #else
        sprintf(topic, "%s/%s", pMsgMqtt->tAXDR.souraddr, pMsgMqtt->tAXDR.destaddr);
        #endif
    }
    else
    {
        ACMQTT_FMT_DEBUG("Err input msg\n");
        return -3;
    }

    /* 两端都是MQTT */
    //PTHREAD_LOCK_SAFE(&mqttSendlock);
    ret = mqtt_client_send_msg(mqttClient, topic, qos, pMsg, (int)msglen);
    //PTHREAD_UNLOCK_SAFE(&mqttSendlock);

    if(0 == ret)
    {
        if(MESSAGE_TYPE_A_XDR == pMsgMqtt->msgtype)
        {
            ACMQTT_BUF_DEBUG(pMsg, msglen, "Send topic %s\n", topic);
        }
        else
        {
            ACMQTT_FMT_DEBUG("Send topic %s\n%s\n", topic, pMsg);
        }
    }
    else
    {
        if(MESSAGE_TYPE_A_XDR == pMsgMqtt->msgtype)
        {
            ACMQTT_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: %04x, iop: %04x, ret %d\n", 
                          pMsgMqtt->tAXDR.index, pMsgMqtt->tAXDR.label, pMsgMqtt->tAXDR.IID, pMsgMqtt->tAXDR.IOP, ret);
        }
        else
        {
            ACMQTT_FMT_DEBUG("Mqtt send failed, topic %s\n%s\n", topic, pMsg);
        }
    }

    free(pMsg);
    return ret;
}

