/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监测主线程功能
* @date：    2020-6-30
* @history： 
*********************************************************************
*/

#include "iot_init.h"
#include "iot_mqtt.h"
#include "iot_public.h"
uint16    g_IotMsgindex = 0;
char      w_AppVer[APPMSG_NAME_LEN] = {0};
uint8     w_AppName[APPMSG_NAME_LEN] = {0};

extern uint16 get_len_offset(uint8 *indata,uint8 *bytes);
extern uint16 msg_mqtt_package(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf);
extern int32 taskmng_plc_queue_write(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);

/**********************************************************************
* @name      : msg_mqtt_split
* @brief     ：接受的消息解析成指定格式
* @param[in] ：uint8 *pBuf           接受到的报文
               uint16 bufLen         报文长度
* @param[out]：MSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
int msg_mqtt_split(uint8 *pBuf, uint16 bufLen,uint32 ipAddr, uint16 port, MSG_INFO_T **ppMsgOut)
{
    uint8  *pTemp = pBuf;
    uint8   offset = 0;
    uint16  tempLen = 0;
    MSG_INFO_T tMsgTemp;
    MSG_INFO_T *ptNewMsg = NULL;
    
    if(NULL == pBuf || NULL == ppMsgOut || 0 == bufLen)
    {
        return -1;
    }

    memset(&tMsgTemp, 0, sizeof(MSG_INFO_T));

    tMsgTemp.IP = ipAddr;
    tMsgTemp.port = port;
    
    /* PRM */
    tMsgTemp.MsgRPM = (*pTemp) & 0x01;

    /* PRIORITY */
    tMsgTemp.MsgPRIORITY = ((*pTemp)>>1) & 0x7f;

    /* INDEX */
    pTemp++;

    tMsgTemp.MsgIndex = *((uint16 *)pTemp);


    /* LABEL */
    pTemp += 2;

    tMsgTemp.MsgLabel = *((uint16 *)pTemp);
    /* SOURCE */
    pTemp += 2;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        
        return -1;
    }
    memcpy(tMsgTemp.SourAddr, pTemp, tempLen);
    tMsgTemp.SourLen = tempLen;

    /* DESTINATION */
    pTemp += tempLen + 1;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        return -1;
    }
    memcpy(tMsgTemp.DestAddr, pTemp, tempLen);
    tMsgTemp.DestLen = tempLen;

    /* TAG */
    pTemp += tempLen + 1;

    tMsgTemp.IID = *((uint16*)(pTemp+2));
    tMsgTemp.IOP = *((uint16*)pTemp);

    /* MsgLen */
    pTemp += 4;
    tMsgTemp.MsgLen = get_len_offset(pTemp, &offset);
    pTemp += offset;
    
    /* 拷贝 */
    ptNewMsg = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
    if(NULL == ptNewMsg)
    {
       // MPRINT("malloc failed,size %d\n", (uint32)sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
        return -1;
    }
    
    memcpy(ptNewMsg, &tMsgTemp, sizeof(MSG_INFO_T));
    memcpy(ptNewMsg->MsgData, pTemp, tMsgTemp.MsgLen);

    *ppMsgOut = ptNewMsg;

    return 0;
}

/**********************************************************************
* @name      : heart_beat
* @brief     ：给系统管理器回复心跳
* @param[in] ：uint16 index 第几个
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/

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
    sprintf(topic,"%s/%s",WUGUAN_APP_NAME_M,"M-smiOS");
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
    
    MsgSend->SourLen = strlen(WUGUAN_APP_NAME_M);
    memcpy(MsgSend->SourAddr,WUGUAN_APP_NAME_M,MsgSend->SourLen);
    MsgSend->DestLen = strlen("M-smiOS");
    memcpy(MsgSend->DestAddr,"M-smiOS",MsgSend->DestLen );
    MsgSend->MsgLen = offset;
    memcpy(MsgSend->MsgData,payload,offset);
    SendLen = msg_mqtt_package(MsgSend,MAX_MSG_LEN,SendBuf);
//    COMM_FMT_DEBUG("heart_beat  \n");
    mqtt_client_send_msg(i_MqttClient,topic,0,SendBuf, SendLen);
    free(MsgSend);
   
}


int iot_mqttinit(WGPubattr *pData)
{
    int ret = 0;
    uint8 status = 0;
    char   mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[4];
    char  mqttstatus[2][16] = {"connectlost", "success"};
    uint32 ip = pData->mqttip;

    /* 新建客户端 */
    sprintf((char *)w_AppName, "%s", "wuguan");
    sprintf((char *)w_AppVer, "%s", APP_VERSION);
    i_MqttClient = mqtt_client_create("wuguan");
    if (NULL == i_MqttClient)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_create fail\n");
        
        return -1;
    }
    
    /* 订阅主题 */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, WG_SUBTOPIC1, strlen(WG_SUBTOPIC1));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, WG_SUBTOPIC2, strlen(WG_SUBTOPIC2));
    subtopic[2].qos = 0;
    memcpy(subtopic[2].topic, WG_SUBTOPIC3, strlen(WG_SUBTOPIC3));
    subtopic[3].qos = 0;
    memcpy(subtopic[3].topic, WG_SUBTOPIC4, strlen(WG_SUBTOPIC4));
    ret = mqtt_client_set_sub_topic(i_MqttClient, subtopic, 4);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 设置默认主题 */
    ret = mqtt_client_set_default_send_topic(i_MqttClient, WG_DEFAULT_TOPIC, 0);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        goto errorend;
    }

    /* 链接服务端 */
    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
        INIT_FMT_DEBUG("mqtt connect %s:%d\n", mqttaddr, pData->mqttport);
    ret = mqtt_client_connect(i_MqttClient, mqttaddr, pData->mqttport, NULL);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("mqtt_client_connect fail, ret %d", ret);
        goto errorend;
    }

    /* 等待链接成功 */
    while(1)
    {
        status = mqtt_client_get_status(i_MqttClient);
        INIT_FMT_DEBUG("Get mqtt connect status %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    INIT_FMT_DEBUG("mqtt connect %s:%d success\n", mqttaddr, pData->mqttport);

    return 0;

errorend:
    mqtt_client_destroy(i_MqttClient);
    return ret;

    return ret;
}

uint16 iot_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE((void *)&g_IotIndexlock);
    curindex = g_IotMsgindex;
    g_IotMsgindex++;
    PTHREAD_UNLOCK_SAFE((void *)&g_IotIndexlock);

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
int iot_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
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
        MQTTMSG_FMT_DEBUG("App send appmsg_package fail\n");
        return -1;
    }
    COMM_BUF_DEBUG(pMsg,msglen,"");
    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_IotSendLock);
    if(NULL != pTopic && 'M' == pMsgInfo->destaddr[0])
    {
        ret = mqtt_client_send_msg(i_MqttClient, pTopic, qos, pMsg, (int)msglen);
    }
    else
    {
        ret = mqtt_client_send(i_MqttClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_IotSendLock);

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
* @name      : iot_recv_mqttmsg
* @brief     ：对外消息接收, 使用了mqtt_client_recv_msg 非阻塞式
* @param[in] ：uint16 buflen    缓存最大容量
* @param[out]：uint8  *pBuf     消息内容
* @return    ：>= 0 消息长度/ < 0 错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int wuguan_recv_mqttmsg(char *topic, uint16 topicLen, uint8 *pBuf, uint32 buflen)
{
    int qos;
    return mqtt_client_recv_msg(i_MqttClient, topic, topicLen, &qos, pBuf, (int)buflen, 1000);
}


/*******************************************************************************
* 函数名称: taskmng_register
* 函数功能: 注册到系统管理服务
* 输入参数: 
* 输出参数: 
* 返 回 值: 
*******************************************************************************/
int  taskmng_register(WGPubattr * pdata)
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
    len = strlen(WUGUAN_APP_NAME_M);
    request[offset++] = len;
    memcpy(&request[offset], WUGUAN_APP_NAME_M, len);
    offset += len;

    /* 订阅事件 */
    request[offset++] = 0;

    /* 版本信息 */
    request[offset++] = strlen(w_AppVer);
    memcpy(request + offset, w_AppVer, strlen(w_AppVer));
    offset += strlen(w_AppVer);

    /* 发布日期 */
    request[offset++] = tTime.year_h;
    request[offset++] = tTime.year_l;
    request[offset++] = tTime.month;
    request[offset++] = tTime.day;
    request[offset++] = tTime.hour;
    request[offset++] = tTime.min;
    request[offset++] = tTime.sec;

    /* APPID */
    request[offset++] = pdata->appidlen;
    if(pdata->appidlen > 0)
    {
        memcpy(request + offset, pdata->appid, pdata->appidlen);
    }
    offset += pdata->appidlen;
    
    pMsgInfo = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + offset);
    if(NULL == pMsgInfo)
    {
        COMM_FMT_DEBUG("Login to os malloc failed %d\n", sizeof(MESSAGE_INFO_T) + offset);
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = iot_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = SYSTEM_IOP_APP_REGISTER;
    
    pMsgInfo->sourlen = strlen(WUGUAN_APP_NAME_M);
    memcpy(pMsgInfo->souraddr , WUGUAN_APP_NAME_M, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = offset;
    memcpy(pMsgInfo->playload, request, offset);
    COMM_FMT_DEBUG("向系统管理器注册 IID %04x IOP %04x\n",pMsgInfo->IID,pMsgInfo->IOP);
    ret = iot_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}


int iot_mqtt_proc(void *pArg, void *pPublic, void *pPrivate)
{
    WGPubattr *public    =  (WGPubattr *)pPublic;
    wg_mqttattr *private =  (wg_mqttattr *)pPrivate;
    int RecvLen = 0,ret=0;
    MSG_INFO_T *MsgRecv = NULL;;
    WG_MSGDATA_T pMsgData;
    WG_MSGDATA_T *pMsg = &pMsgData;
    switch(private->status)
    {
        case STATUS_LOGIN:
        {
            taskmng_register((WGPubattr *)pPublic);
            private->status = STATUS_RECV;
        }break;
        case STATUS_RECV:
        {
            memset(pMsg,0,sizeof(WG_MSGDATA_T));
            RecvLen = 0;
			RecvLen = wuguan_recv_mqttmsg(pMsg->topic, WG_TOPIC_LEN, pMsg->recvBuf, APP_RECV_BUF_LEN);
            if(RecvLen>0)
            {   
            	if(strstr(pMsg->topic,JSON_REPORT_FLAG)!=NULL)
				{                   
					COMM_FMT_DEBUG("收到json上报消息 \n");
					COMM_FMT_DEBUG("topic %s \n %s \n",pMsg->topic,pMsg->recvBuf);
					ret = taskmng_plc_queue_write(&public->jsonque, (uint8 *) pMsg,sizeof(*pMsg));
					if(ret == 0)
					{
                        COMM_FMT_DEBUG("json队列添加失败\n");
                    }
                    break;
				}
                ret = msg_mqtt_split(pMsg->recvBuf,RecvLen,0,0,&MsgRecv);
                if(ret<0)
                    break;
                 //增加防护 以防出现死循环
                if(strcmp(MsgRecv->SourAddr,WUGUAN_APP_NAME_M)==0)
                    break;
                if(MsgRecv->IID !=0x0000)
                {
                    COMM_BUF_DEBUG(pMsg->recvBuf, RecvLen,"收到MQTT消息 source %s IID %04x IOP %04x \n",MsgRecv->SourAddr,MsgRecv->IID,MsgRecv->IOP);
                }
                RecvLen = sizeof(MSG_INFO_T)+MsgRecv->MsgLen;
                 switch (MsgRecv->IID)
                    {
                        case 0x0000:
                        {
                            if(MsgRecv->IOP == 0x0010)
                            {
                                heart_beat(MsgRecv->MsgIndex);
                            }
							/* 系统停电或复位 内存日志拷贝到磁盘*/
			                else if(MESSAGE_IID_COMMON == MsgRecv->IOP )
			                {
			                    if(COMMON_IOP_URGENT_POWERDOWN == MsgRecv->IOP || COMMON_IOP_SYSTEM_POWERDOWN == MsgRecv->IOP || COMMON_IOP_SYSTEM_RESTART == MsgRecv->IOP || COMMON_IOP_SYSTEM_POWERON == MsgRecv->IOP)
			                    {
									COMM_FMT_DEBUG("recv sys msg iop %u\n", MsgRecv->IOP);
			                    	//evt_emerge_set(MsgRecv->IOP);
			                    }
			                }
                        }break;
                        case 0x0001:
                        {   
                            if(MsgRecv->IOP == 0x0010)
                            {
                                COMM_FMT_DEBUG("系统管理器注册成功 \n");
                                BASE_TASK_SET_OK(pArg);
                                private->logined= 1;
                            } 
                        }break;
                        case 0x000B:
                        {
                            //发送到上行命令处理线程
                            ret = taskmng_plc_queue_write(&public->msgque, (uint8 *) MsgRecv,RecvLen);
        					if(ret == 0)
        					{
                                COMM_FMT_DEBUG("msg队列添加失败\n");
                            }
                        break;
                        }break;
                        default:
                        {
                            COMM_FMT_DEBUG("不识别的消息 \n");
                        }break;
                            
                 }
                free(MsgRecv);
                MsgRecv = NULL;
                usleep(100000);
            }
            else
            {   
                usleep(100000);
            }
            if(private->logined == 1)
                private->status = STATUS_SEND;
            else
                private->status = STATUS_LOGIN;
        }break;
        case STATUS_SEND:
        {
            RecvLen = 0;
            uint8 msglen=0;
            uint8 *sendmsg = NULL;
            RecvLen = taskmng_plc_queue_read(&public->upque, pMsg->recvBuf);
            if(RecvLen > 0)
            {
                MSG_INFO_T *msgup = NULL;
                char topic[1000]={0};
                msgup = (MSG_INFO_T *)pMsg->recvBuf;
                sprintf(topic,"%s/%s/%04X/%04X/",msgup->SourAddr,msgup->DestAddr,msgup->IID,msgup->IOP);
                msglen = wuguan_appmsg_package((MSG_INFO_T *)pMsg->recvBuf,&sendmsg); 
                if(msglen > 0)
                {
                    COMM_BUF_DEBUG(sendmsg,msglen,"上行报文     TOPIC %s \n:",topic);
                    if(mqtt_topic_write(sendmsg, msglen,topic)!=0)
                    {
                        COMM_FMT_DEBUG("发送失败\n");
                    } 
                }
            }
            private->status = STATUS_RECV;
            free(sendmsg);
            sendmsg = NULL;
            usleep(10000);
        }break;
        default:
            break;
    }
    return 10000;
}

int iot_mqtt_init(void *pPublic, void **ppPrivate)
{
    WGPubattr *pPubattr = (WGPubattr*)pPublic;
    COMM_FMT_DEBUG("启动comm线程\n");
    iot_mqttinit(pPubattr);
    wg_mqttattr *ptData = NULL;
    ptData    = (wg_mqttattr *)malloc(sizeof(wg_mqttattr));
    if(ptData == NULL)
    {
        COMM_FMT_DEBUG("初始化失败\n");
            return -1;
    }
    ptData->status =  STATUS_LOGIN;
    ptData->logined = 0;
    if ( pthread_mutex_init ( &g_IotSendLock, NULL ) != 0 )
    {
        printf ("init pthread g_IotSendlock lock error!\n" );
        exit ( 1 );
    }
    if ( pthread_mutex_init ( &g_MqttIndexlock, NULL ) != 0 )
    {
        printf ("init pthread g_IotSendlock lock error!\n" );
        exit ( 1 );
    }
    if ( pthread_mutex_init ( &g_IotIndexlock, NULL ) != 0 )
    {
        printf ("init pthread g_IotSendlock lock error!\n" );
        exit ( 1 );
    }
    *ppPrivate = ptData;
    return 0;
}
