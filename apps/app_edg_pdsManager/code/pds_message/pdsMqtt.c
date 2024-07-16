
#include "pdsAll.h"
#include "mqtt_client.h"

#include "message.h"
#include "pdsDB_update.h"
#include "pdsCommon.h"
#define MSG_INIT           0               //初始化连接
#define MSG_NOMAL           1     //正常状态

#define MAX_APPID_LEN 24
MQTT_CLIENT_T *g_MqttClient;
// mqtt 连接状态
MQTT_STATUS_E s_mqtt_status;

DB_AGENT gDBAgent = 0;
pthread_mutex_t  gMqttSendLock;
pthread_mutex_t  gMsgSeqLock;
uint16 gMsgSeq = 0;

char gPdsAppName[20]={0};
int gMqttSock = 0;

extern LockQueue_T gTransQueue; 
extern uint8 isDBCenterUp;
/*******************************************************************************
* 函数名称: GetMsgSeq
* 函数功能: 获取消息序号
* 输入参数: 
* 输出参数: 
* 返 回 值: 消息序号
*******************************************************************************/
uint16 GetMsgSeq()
{
    uint16 seq =0 ;
    pthread_mutex_lock(&gMsgSeqLock);
    seq = gMsgSeq++;
    pthread_mutex_unlock(&gMsgSeqLock);
    return seq;
}

int mqtt_message_send(MESSAGE_INFO_T *MsgSend,char* topic)
{
    int ret;
   
    uint16 SendLen;
    uint8* SendBuf = NULL;
    SendLen = appmsg_package(MsgSend,&SendBuf);
       
    MQTTMSG_BUF_TRACE(SendBuf, SendLen,"发送mqtt消息 topic %s  \n",topic);
    pthread_mutex_lock(&gMqttSendLock);
    ret = mqtt_client_send_msg(g_MqttClient,topic,0,SendBuf,SendLen);
    pthread_mutex_unlock(&gMqttSendLock);
    free(SendBuf);
    return ret;
}
/*******************************************************************************
* 函数名称: mqtt_sendtodb 
* 函数功能: //通过mqtt向数据中心发送数据
* 输入参数: label 消息标签
        IOP 消息IOP
        payload 消息内容
        len 发送的消息长度

* 输出参数: pindex 消息序号
* 返 回 值:  >0成功 -1 失败
*******************************************************************************/
int mqtt_sendtodb(uint16 label,uint16 iop,uint8* payload,uint16 len,uint16* pindex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;

    uint16 msglen = 0;
    int ret; 
    char topic[100];
    sprintf(topic,"%s/%s",gPdsAppName,DBCENTER_NAME);
    msglen = sizeof(MESSAGE_INFO_T) + len;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(msglen);
    if(pMsgInfo == NULL)
    {
        return -1;
    }
    memset(pMsgInfo,0x00,msglen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority =1;
    *pindex = GetMsgSeq();
    pMsgInfo->index = *pindex;
    pMsgInfo->label = label;
    pMsgInfo->sourlen = strlen(gPdsAppName);
    memcpy(pMsgInfo->souraddr,gPdsAppName,pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(DBCENTER_NAME);
    memcpy(pMsgInfo->destaddr,DBCENTER_NAME,pMsgInfo->destlen);
    pMsgInfo->IID = MESSAGE_IID_DB;
    pMsgInfo->IOP = iop;
    pMsgInfo->msglen = len;
    memcpy(pMsgInfo->playload,payload,len);

    ret = mqtt_message_send( pMsgInfo,topic);

    free(pMsgInfo);
    if(ret>0)
        return 0;
    else
        return ret;
}
void mqtt_login_to_smiOS(void)
{
    uint8 payload[500] = {0};
    uint16 index = 0;
    uint8 len;
    int appidlen;
    uint8 appid[MAX_APPID_LEN]={0};
    Date_Time_S publishtime;
    memset(&publishtime,0x00,sizeof(Date_Time_S));
    MESSAGE_INFO_T *MsgSend = NULL;
    int pid = getpid();
    
    char topic[256] = {0};
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",gPdsAppName,SMIOS_NAME);
    memcpy_r(&payload[index],(uint8*)&pid,4);
    index+=4;
    len = strlen(gPdsAppName);
    payload[index++] = len;
    memcpy(&payload[index],gPdsAppName,len);
    index+=len;
    
    payload[index++] = 0; //订阅事件个数
    
    payload[index++] = strlen(APP_VERSION);
    memcpy(payload + index, APP_VERSION, strlen(APP_VERSION));
    index += strlen(APP_VERSION);
    
    app_get_maketime(APP_PUBLISH_TIME,&publishtime);
    payload[index++]=publishtime.year_h;
    payload[index++]=publishtime.year_l;
    payload[index++]=publishtime.month;
    payload[index++]=publishtime.day;
    payload[index++]=publishtime.hour;
    payload[index++]=publishtime.min;
    payload[index++]=publishtime.sec;

    appidlen = MAX_APPID_LEN;
    payload[index++] = (uint8)appidlen;
    memcpy(&payload[index],appid,appidlen);
    index+=appidlen;
    
    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
    MsgSend->rpm = 1;
    MsgSend->index = GetMsgSeq();
    MsgSend->priority = 1;
    MsgSend->label = 1;
    MsgSend->IID = 0x0001;
    MsgSend->IOP = 0x0010;
    
    MsgSend->sourlen = strlen(gPdsAppName);
    memcpy(MsgSend->souraddr,gPdsAppName,MsgSend->sourlen);
    MsgSend->destlen = strlen(SMIOS_NAME);
    memcpy(MsgSend->destaddr,SMIOS_NAME,MsgSend->destlen );

    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);

    mqtt_message_send(MsgSend,topic);
  
    free(MsgSend);

}
int mqtt_recv_msg(uint8 *pBuf, uint16 buflen)
{
    char          topic[256];
    int           qos;
    return mqtt_client_recv_msg(g_MqttClient, topic, 256, &qos, pBuf, (int)buflen, 1000);

}
/**********************************************************************
* @name      : mqtt_heartbeat
* @brief     ：心跳
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int mqtt_heartbeat(MESSAGE_INFO_T *pInMsg)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16 len = 0;
    uint32 status = 1;
    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",gPdsAppName,SMIOS_NAME);
    len = sizeof(MESSAGE_INFO_T) + sizeof(uint32);
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Heart beat malloc failed %d\n", len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 0;
    pMsgInfo->index = pInMsg->index;
    pMsgInfo->priority = pInMsg->priority;
    pMsgInfo->label = pInMsg->label;
    pMsgInfo->IID = MESSAGE_IID_COMMON;
    pMsgInfo->IOP = COMMON_IOP_HERTBEAT;
    
    pMsgInfo->sourlen = strlen(gPdsAppName);
    memcpy(pMsgInfo->souraddr , gPdsAppName, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(SMIOS_NAME);
    memcpy(pMsgInfo->destaddr, SMIOS_NAME, pMsgInfo->destlen);

    pMsgInfo->msglen = sizeof(uint32);
    appmsg_memcpy_r(pMsgInfo->playload, &status, sizeof(uint32));

    ret = mqtt_message_send(pMsgInfo, topic);
    if(ret == 0)
    {
        MQTTMSG_FMT_DEBUG("======= send heart secc=======\n");
    }
    else
    {
        MQTTMSG_FMT_DEBUG("=======send heart err=======\n");
    }
    free(pMsgInfo);
    return ret;
}
/**********************************************************************
* @name      : mqtt_get_version
* @brief     ：查询版本号
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int mqtt_get_version(MESSAGE_INFO_T *pInMsg)
{
    char         uTopic[128] = {0};
    uint8           buf[360] = {0};
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

    ptMsgNew->sourlen = strlen(gPdsAppName);
    memcpy(ptMsgNew->souraddr, gPdsAppName, ptMsgNew->sourlen);

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

    sprintf(uTopic, "%s/%s", gPdsAppName,SMIOS_NAME);
    return mqtt_message_send( ptMsgNew, uTopic);
}

/**********************************************************************
* @name      : mqtt_recv_app_info
* @brief     ：解析查询到的APP信息
* @param[in] ：
* @param[out]：
* @return    ：
**********************************************************************/
void mqtt_recv_app_info(MESSAGE_INFO_T *pInMsg)
{
    uint8 payload[500]={0};
    uint16 len = pInMsg->msglen;
    memcpy(payload,pInMsg->playload,len);
    char appname[100]={0};
    uint8 applen = 0;
    uint8 offset;
    if(len>0)
    {
        applen = get_len_offset(payload,&offset);
        memcpy(appname,&payload[offset],applen);
        MQTTMSG_FMT_DEBUG("查询到的APPname %s \n",appname);
        if(strcmp(appname,DBCENTER_NAME)==0)
        {
            isDBCenterUp = 1;
        }
    }

}



void* mqtt_thread(void* arg)
{
    int ret;
    uint8 mqttstatus = MSG_INIT;
    int Recvlen = 0;
    uint8 Recvbuf[MAX_MSG_LEN]={0};
    struct timespec curtime = {0};
    struct timespec oldtime = {0};
    MESSAGE_INFO_T *MsgRecv = NULL;
    
    mqtt_login_to_smiOS();
    
    clock_gettime(CLOCK_MONOTONIC, &oldtime);
    while(1)
    {
        Recvlen = mqtt_recv_msg(Recvbuf,MAX_MSG_LEN);
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        if(Recvlen>0)
        {
            ret = appmsg_split(Recvbuf,Recvlen,&MsgRecv);
            if(NULL!=MsgRecv)
            {
                if(strcmp(MsgRecv->souraddr,gPdsAppName)==0)
                {
                    MQTTMSG_FMT_TRACE("收到自发的消息 过滤 iid %04x iop %04x \n",MsgRecv->IID,MsgRecv->IOP);
                    free(MsgRecv);
                    MsgRecv = NULL;
                    continue;
                }
            }
            if(ret==0 && MsgRecv!=NULL)
            {
             if((MsgRecv->IID!=0x0005|| MsgRecv->IOP!=0x0001)&&MsgRecv->IID != 0x0000)
             {
                MQTTMSG_BUF_TRACE(Recvbuf, Recvlen, "收到mqtt消息 sourc %s IID 0X%04X  IOP 0X%04X \n",MsgRecv->souraddr,
                                    MsgRecv->IID,MsgRecv->IOP);
             }
                if(mqttstatus ==MSG_INIT )
                {
                    if(MsgRecv->IID ==MESSAGE_IID_SYSTEM && MsgRecv->IOP ==SYSTEM_IOP_APP_REGISTER )
                    {
                        //收到请求注册回码
                        mqttstatus = MSG_NOMAL;
                    }
                }else
                {
                    switch(MsgRecv->IID)
                    {
                        case MESSAGE_IID_COMMON:
                        {
                            switch(MsgRecv->IOP)
                            {
                                //心跳
                                case COMMON_IOP_HERTBEAT:
                                {
                                    mqtt_heartbeat(MsgRecv);
                                }
                                    break;
                                //查询版本
                                case COMMON_IOP_VERSION:
                                {
                                    mqtt_get_version(MsgRecv);
                                }
                                    break;
                                default:
                                    break;
                            }
                        }
                            break;
                        case MESSAGE_IID_SYSTEM:
                        {
                            switch(MsgRecv->IOP)
                            {
                                case SYSTEM_IOP_GET_APP:
                                {
                                    mqtt_recv_app_info(MsgRecv);
                                }
                                    break;
                                default:
                                    break;
                            }
                        }
                            break;
                        case MESSAGE_IID_DB:
                        {
                            switch(MsgRecv->IOP)
                            {
                                case DB_IOP_UPDATE_EVENT:
                                case DB_IOP_INIT_EVENT:
                                {
                                    db_update_func(MsgRecv);
                                }
                                break;
                                default:
                                    db_agent_msg_entry(gDBAgent,MsgRecv);
                                break;
                            }
                        }
                            break;
                        case MESSAGE_IID_PDS:
                        {
                            switch(MsgRecv->IOP)
                            {
                                case PDS_IOP_TOPOLOGY_START:
                                {
                                    topology_start(MsgRecv);
                                }
                                    break;
                                default:
                                    break;

                            }
                        }
                            break;
                        case MESSAGE_IID_CTS:
                        {
                            switch(MsgRecv->IOP)
                            {
                                case CTS_IOP_READ_LIST_REQ:
                                {
                                    TransWriteQueue(&gTransQueue, (uint8*)MsgRecv, sizeof(MESSAGE_INFO_T)+MsgRecv->msglen);
                                }
                                    break;
                                default:
                                    break;

                            }
                        }
                            break;
                        default:
                            break;


                    }
                }
                free(MsgRecv);
                MsgRecv = NULL;

            }
        }
        if(mqttstatus == MSG_INIT)
        {
            if(curtime.tv_sec - oldtime.tv_sec > 5)
            {
                MQTTMSG_FMT_TRACE("5秒内未收到注册回码，重新注册 \n");
                mqtt_login_to_smiOS();
            }
        }

    }
}

int mqtt_init(char* ip,int port)
{
    int ret;
    //char appname[20]={0};
    pthread_t mqttid=0;
    sprintf(gPdsAppName,"M-%s",PDS_APP_NAME);
    g_MqttClient = mqtt_client_create(gPdsAppName);
    if(NULL ==g_MqttClient )
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_create %s fail \n",gPdsAppName);
        return -1;
    }
    SUB_TOPIC_T topic[2];
    memset(topic,0x00,sizeof(topic));
    sprintf(topic[0].topic,"+/%s",gPdsAppName);
    topic[0].qos = 0;
    sprintf(topic[1].topic,"M-dbCenter/Broadcast/0005/#");
    topic[1].qos = 0;
    ret = mqtt_client_set_sub_topic(g_MqttClient, topic,2);
    if(ret!=0)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_sub_topic fail \n");
        mqtt_client_destroy(g_MqttClient);
        return -1;
    }

	ret = mqtt_client_connect(g_MqttClient,ip,port, NULL);
	if(ret!=0)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_connect fail \n");
        mqtt_client_destroy(g_MqttClient);
        return -1;
    }
    while(s_mqtt_status!=MQTT_CONNECT_SUCC)
	{
        s_mqtt_status = mqtt_client_get_status(g_MqttClient);
        sleep(1);
	}
	MQTTMSG_FMT_DEBUG(" APP: %s mqtt_client_connect  success \n",gPdsAppName);
	
    gDBAgent = db_agent_create(AGENT_TYPE_THREAD, DB_PDS_AGENT, mqtt_sendtodb, 20);
    if(gDBAgent <0 )
    {
        MQTTMSG_FMT_DEBUG("创建dbagent失败！！ \n");
        return -1;
    }
    pthread_mutex_init(&gMsgSeqLock, NULL);
    pthread_mutex_init(&gMqttSendLock, NULL);
    db_update_init();
    gMqttSock = task_msg_init(PDS_MQTT_IPC);
    if(gMqttSock<0)
    {
        MQTTMSG_FMT_DEBUG("创建PDS_MQTT_IPC 失败 ！！ \n");
        return -1;
    }
    task_msg_settime(gMqttSock,0,100000);
    ret = pthread_create(&mqttid,NULL,mqtt_thread,NULL);
    if(ret!=0)
    {
        MQTTMSG_FMT_DEBUG("创建mqtt线程失败 ！！ \n");
        return -1;
    }
	return 0;
}

/*******************************************************************************
* 函数名称: mqtt_sendtotaskManager 
* 函数功能: //通过mqtt向taskManager发送消息
* 输入参数: label 消息标签
        IOP 消息IOP
        payload 消息内容
        len 发送的消息长度

* 输出参数: pindex 消息序号
* 返 回 值:  >0成功 -1 失败
*******************************************************************************/
int mqtt_sendtotaskManager(uint16 iop,uint8* payload,uint16 len,uint16* pindex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;

    uint16 msglen = 0;
    int ret; 
    char topic[100];
    sprintf(topic,"%s/%s",gPdsAppName,TASKMANAGER_NAME);
    msglen = sizeof(MESSAGE_INFO_T) + len;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(msglen);
    if(pMsgInfo == NULL)
    {
        return -1;
    }
    memset(pMsgInfo,0x00,msglen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority =1;
    *pindex = GetMsgSeq();
    pMsgInfo->index = *pindex;
    pMsgInfo->label = 0;
    pMsgInfo->sourlen = strlen(gPdsAppName);
    memcpy(pMsgInfo->souraddr,gPdsAppName,pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(TASKMANAGER_NAME);
    memcpy(pMsgInfo->destaddr,TASKMANAGER_NAME,pMsgInfo->destlen);
    pMsgInfo->IID = MESSAGE_IID_CTS;
    pMsgInfo->IOP = iop;
    pMsgInfo->msglen = len;
    memcpy(pMsgInfo->playload,payload,len);

    ret = mqtt_message_send( pMsgInfo,topic);

    free(pMsgInfo);
    if(ret>0)
        return 0;
    else
        return ret;
}


/*******************************************************************************
* 函数名称: mqtt_sendtotaskManager 
* 函数功能: //通过mqtt向taskManager发送透传
* 输入参数: buf 发送数据内容
         len 发送的数据内容长度
        port 端口号
        comm 通讯字
        overtime 超时时间
* 输出参数: msgindex 消息序号
* 返 回 值:  >0成功 -1 失败
*******************************************************************************/
int mqtt_sendtrans(uint8* buf,uint16 len,uint32 port,uint8 baud,uint32 overtime,uint16 *msgindex)
{
    int ret;
    uint8 payload[MAX_MSG_LEN]={0};
    uint16 index = 0;
    
    uint8 comm = 0; //通讯字
    //端口号
    memcpy_r(&payload[index],&port,4);
    index+=4;
    //通讯字 默认1停止位 偶校验 8数据位
    comm|=0x03;
    comm|=(1<<3);
    comm|=(baud<<5);
    payload[index++] = comm;
    //帧超时时间
    memcpy_r(&payload[index],&overtime,2);
    index+=2;
    //字符超时时间
    memcpy_r(&payload[index],&overtime,2);
    index+=2;
    if(len<0x80)
    {
        payload[index++] = len;
    }
    else
    {
        payload[index++] = 0x82;
        payload[index++] = (uint8)(len>>8);
        payload[index++] = (uint8)len;
    }
    memcpy(&payload[index],buf,len);
    index+=len;
    ret = mqtt_sendtotaskManager(CTS_IOP_READ_LIST_REQ,payload,index,msgindex);
    if(ret!=0)
    {
        MQTTMSG_BUF_DEBUG(payload,index,"发送透传消息失败\n");
    }
    return ret;
}

/*******************************************************************************
* 函数名称: mqtt_query_app_info 
* 函数功能: //通过mqtt向smios查询指定app信息
* 输入参数: appname app名称

* 输出参数: 
* 返 回 值:  
*******************************************************************************/
void mqtt_query_app_info(char* appname)
{
    MESSAGE_INFO_T *MsgSend = NULL;
    uint8 payload[100]={0};
    uint16 index = 0;
    uint8 applen = strlen(appname);
    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",gPdsAppName,SMIOS_NAME);
    if(applen>100)
        return;
    payload[index++]= applen;
    memcpy(&payload[index],appname,applen);
    index +=applen;
    
    MsgSend = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T)+index);
    memset(MsgSend,0x00,sizeof(MESSAGE_INFO_T)+index);
    MsgSend->rpm = 1;
    MsgSend->index = GetMsgSeq();
    MsgSend->priority = 1;
    MsgSend->label = 1;
    MsgSend->IID = 0x0001;
    MsgSend->IOP = 0x0013;
    
    MsgSend->sourlen = strlen(gPdsAppName);
    memcpy(MsgSend->souraddr,gPdsAppName,MsgSend->sourlen);
    MsgSend->destlen = strlen(SMIOS_NAME);
    memcpy(MsgSend->destaddr,SMIOS_NAME,MsgSend->destlen );

    MsgSend->msglen = index;
    memcpy(MsgSend->playload,payload,index);

    mqtt_message_send(MsgSend,topic);
  
    free(MsgSend);
}


