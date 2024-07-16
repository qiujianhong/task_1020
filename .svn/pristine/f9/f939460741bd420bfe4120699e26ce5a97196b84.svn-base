
#include "CtsAll.h"
#include "mqtt_client.h"
#include "appmsg_split.h"
// mqtt 客户端句柄

MQTT_CLIENT_T *g_MqttClient;
// mqtt 连接状态
MQTT_STATUS_E s_mqtt_status;

extern char mqttIP[100];
extern int mqttport;
//连接mqtt broker 资源地址 协议类型://ip地址:端口
//const char *mqttUrl = "mqtt://127.0.0.1:1883";
// APP 名称 同时用作MQTT 用户ID 及默认订阅的主题
//const char *appName = "ccoRouter/cjMng";
//const char *appName = "cjManager";
 char *smiosName = "M-smiOS";
extern int get_app_id(uint8* appid,int maxlen);

// 登录标志
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
int mqtt_reproc(void *pMsg, int msglen)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;

    appmsg_split((uint8*)pMsg, (uint16)msglen , &pMsgInfo);
    if(NULL == pMsgInfo)
    {
        return 1;
    }

    ret = db_agent_msg_entry(gDBAgent, pMsgInfo);
    if(DB_AGENT_UNKONW_MSG == ret || DB_AGENT_DROP_MSG == ret)
    {
        /* 加锁失败放到队列中继续执行一次 */
        free(pMsgInfo);
        return -1;
    }
    //TASK_FMT_TRACE(-1,MQTT_LOG,"Recv msg from %s \n", pMsgInfo->souraddr);
    //TASK_BUF_TRACE(-1,MQTT_LOG,pMsg, msglen);
    if(0 != ret)
    {
        TASK_FMT_TRACE(-1,MQTT_LOG,"Drop msg from %s, ret %d \n", pMsgInfo->souraddr, ret);
        TASK_BUF_TRACE(-1,MQTT_LOG,pMsg, msglen);
    }
    free(pMsgInfo);
    return 0;
}

int taskmng_mqtt_init()
{
    int ret = -1;

    
    g_MqttClient = mqtt_client_create(APPNAME);
    if(NULL ==g_MqttClient )
    {
        TASK_FMT_TRACE(-1,MQTT_LOG,"mqtt_client_create fail \n");
        return -1;
    }
    SUB_TOPIC_T topic[9];
    memset(topic,0x00,sizeof(topic));
    sprintf(topic[0].topic,"+/%s",APPNAME);
    topic[0].qos = 0;
    //数据中心事件
    sprintf(topic[1].topic,"M-dbCenter/Broadcast/0005/0001/3");
    topic[1].qos = 0;
    //ccoRouter事件

    sprintf(topic[2].topic,"M-ccoRouter/Broadcast/0003/#");
    topic[2].qos = 0;

    //蓝牙事件
    sprintf(topic[3].topic,"M-btManager/Broadcast/000A/#");
    topic[3].qos = 0;

    //遥信变位
    sprintf(topic[4].topic,"M-rspSample/Broadcast/0008/#");
    topic[4].qos = 0;

    sprintf(topic[5].topic,"M-dbCenter/Broadcast/0005/0002/#");
    topic[5].qos = 0;
    
    sprintf(topic[6].topic,"M-dbCenter/Broadcast/0005/0001/2");
    topic[6].qos = 0;
    
    sprintf(topic[7].topic,"M-dbCenter/Broadcast/0005/0001/1");
    topic[7].qos = 0;
    
    sprintf(topic[8].topic,"M-dbCenter/Broadcast/0005/0001/4");
    topic[8].qos = 0;

    
    ret = mqtt_client_set_sub_topic(g_MqttClient, topic,9);
    if(ret!=0)
    {
        TASK_FMT_TRACE(-1,MQTT_LOG,"mqtt_client_set_sub_topic fail \n");
        mqtt_client_destroy(g_MqttClient);
        return -1;
    }
    /* 增加预处理处理函数 */
    ret = mqtt_msg_add_pre_porc(g_MqttClient, mqtt_reproc);
    if(0 != ret)
    {
        TASK_FMT_TRACE(-1,MQTT_LOG,"mqtt_msg_add_pre_porc fail, ret %d\n", ret);
        mqtt_client_destroy(g_MqttClient);
        return -1;
    }
    ret = mqtt_client_connect(g_MqttClient,mqttIP,mqttport, NULL);
    if(ret!=0)
    {
        TASK_FMT_TRACE(-1,MQTT_LOG,"mqtt_client_connect fail \n");
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

void login_to_smiOS(void)
{
    uint8 payload[500] = {0};
    uint16 index = 0;
    uint8 len;
    int appidlen;
    uint8 appid[MAX_APPID_LEN]={0};
    Date_Time_S publishtime;
    memset(&publishtime,0x00,sizeof(Date_Time_S));
    uint8 SendBuf[MAX_MSG_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;
    
    //uint16 eventiid = 0x0005;
    int pid = getpid();
    printf("当前进程号%d \n",pid);
    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",APPNAME,smiosName);
    memcpy_r(&payload[index],(uint8*)&pid,4);
    index+=4;
    len = strlen(APPNAME);
    payload[index++] = len;
    memcpy(&payload[index],APPNAME,len);
    index+=len;
    //不需要订阅事件了
    payload[index++] = 0; //订阅事件个数
    
//    memcpy_r(&payload[index],(uint8*)&eventiid,2);
//    index+=2;
//    payload[index++]=0;
//    payload[index++]=0x06;//订阅事件1和2 00000110
//    
//    eventiid = 0x0004;
//    memcpy_r(&payload[index],(uint8*)&eventiid,2);
//    index+=2;
//    payload[index++]=0;
//    payload[index++]=0x02;//订阅事件1 00000010
//
//    eventiid = 0x0003;
//    memcpy_r(&payload[index],(uint8*)&eventiid,2);
//    index+=2;
//    payload[index++]=0;
//    payload[index++]=0x02;//订阅事件1 00000010
//    
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

    appidlen = get_app_id(appid,MAX_APPID_LEN);
    payload[index++] = (uint8)appidlen;
    memcpy(&payload[index],appid,appidlen);
    index+=appidlen;
    
    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MSG_INFO_T)+index);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgIndex = GetMsgSeq();;
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0001;
    MsgSend->IOP = 0x0010;
    
    MsgSend->SourLen = strlen(APPNAME);
    memcpy(MsgSend->SourAddr,APPNAME,MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr,smiosName,MsgSend->DestLen );
    TASK_FMT_TRACE(-1,MQTT_LOG,"SourLen %d sour %s \n",MsgSend->SourLen,MsgSend->SourAddr);
    TASK_FMT_TRACE(-1,MQTT_LOG,"DestLen %d DestAddr %s \n",MsgSend->DestLen,MsgSend->DestAddr);
    MsgSend->MsgLen = index;
    memcpy(MsgSend->MsgData,payload,index);
    SendLen = msg_mqtt_package(MsgSend,MAX_MSG_LEN,SendBuf);
    TASK_FMT_TRACE(-1,MQTT_LOG,"登录到smios destServiceTopic %s SendLen %d msgdata \n",smiosName,SendLen);
    commfun_printstring(SendBuf,SendLen);

    TASK_BUF_TRACE(-1,MQTT_LOG,SendBuf, SendLen);
    mqtt_client_send_msg(g_MqttClient,topic,0,SendBuf, SendLen);
    free(MsgSend);

}

void heart_beat(uint16 index)
{
    uint8 payload[200] = {0};
    uint8 offset = 0;
    uint32 value = 12;
    uint8 SendBuf[MAX_MSG_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;

    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",APPNAME,smiosName);
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
    
    MsgSend->SourLen = strlen(APPNAME);
    memcpy(MsgSend->SourAddr,APPNAME,MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr,smiosName,MsgSend->DestLen );
    MsgSend->MsgLen = offset;
    memcpy(MsgSend->MsgData,payload,offset);
    SendLen = msg_mqtt_package(MsgSend,MAX_MSG_LEN,SendBuf);
    TASK_FMT_DEBUG(-1,MQTT_LOG,"heart_beat  \n");
    //commfun_printstring(SendBuf, SendLen);

    mqtt_client_send_msg(g_MqttClient,topic,0,SendBuf, SendLen);
    free(MsgSend);
   
}

int mqtt_read(uint8* RecvBuf)
{
    return mqtt_client_recv(g_MqttClient,RecvBuf, MAX_MSG_LEN);
}

int mqtt_topic_read(uint8* RecvBuf)
{
    char          topic[256];
    int           qos;
    return mqtt_client_recv_msg(g_MqttClient, topic, strlen(topic), &qos,RecvBuf, MAX_MSG_LEN, 1000);
}



int mqtt_topic_write(uint8* SendBuf,uint32 SendLen,char* topic)
{

    return mqtt_client_send_msg(g_MqttClient,topic,0,SendBuf, SendLen);
}


