

#include "Mqtt.h"
#include "CcoRedef.h"
#include "CcoDebug.h"
#include "CcoCommon.h"
#include "CcoMain.h"
#include "CcoLib.h"
#include "mqtt_client.h"


/*******************************************************************************
* 本地对上通信 MQTT通信参数 *********************------<测试修改>------**********
*******************************************************************************/

//连接mqtt broker 资源地址 协议类型://ip地址:端口------<测试修改>------
#if DOWN_DEV_TYPE == 2
const char *mqttUrl = "mqtt://172.17.0.1:1883";  //容器
#else
const char *mqttUrl = "mqtt://127.0.0.1:1883";  //非容器
#endif

// APP 名称 同时用作MQTT 用户ID------<测试修改>------
char *appName = "M-ccoRouter";

//接收数据的topic 名称------<测试修改>------
char *srcServiceTopic = "+/M-ccoRouter";

// xl_channel_write 发送数据的topic 名称------<测试修改>------
char *destServiceTopic = "M-ccoRouter/M-taskManager";

//系统管理器APP名称------<测试修改>------
char *smiosName = "M-smiOS";

//模组管理器APP名称------<测试修改>------
char *mapManagerName = "M-mapManager";

//数据中心APP名称------<测试修改>------
char *dbName = "M-dbCenter";

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

int mqtt_init()
{
    int ret = -1;
    uint32 topicNum = 0;
    
    g_MqttClient = mqtt_client_create(appName);
    if(NULL == g_MqttClient)
    {
        CCOUP_FMT_DEBUG("mqtt_init() mqtt_client_create fail \n");
        return -1;
    }
    SUB_TOPIC_T topic[3];
    memset(topic, 0x00, sizeof(topic));
    sprintf(topic[0].topic, "+/%s", appName);
    topic[0].qos = 0;
    topicNum++;
    sprintf(topic[1].topic, "M-desktopGui/Broadcast/0007/#");
    topic[1].qos = 0;
    topicNum++;

    ret = mqtt_client_set_sub_topic(g_MqttClient, topic, topicNum);
    if(ret!=0)
    {
        CCOUP_FMT_DEBUG("mqtt_init() mqtt_client_set_sub_topic fail \n");
        mqtt_client_destroy(g_MqttClient);
        return -1;
    }

	ret = mqtt_client_connect(g_MqttClient, mqttIP, mqttport, NULL);
	if(ret!=0)
    {
        CCOUP_FMT_DEBUG("mqtt_init() mqtt_client_connect fail \n");
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
    uint8 payload[200] = {0};
    uint8 index = 0;
    uint8 len;
    uint8 SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;
    //uint8 log[MAX_QUEUE_BUFF_LEN] = {0};
    //uint16 eventiid = 0;

    //进程号
    int pid = getpid();
    CCOUP_FMT_DEBUG("当前进程号%d \n",pid);
    char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/%s",appName,smiosName);
    memcpy_r(&payload[index],(uint8*)&pid,4);
    index+=4;

    //组件名称
    len = strlen(appName);
    payload[index++] = len;
    memcpy(&payload[index],appName,len);
    index+=len;

    //订阅显示事件
    payload[index++] = 0;  //订阅事件个数

    //版本信息
    //uint32 appversion = APP_VERSION;
    //memcpy_r(&payload[index], (uint8*)&appversion, 4);
    //index+=4;
    payload[index++] = strlen(APP_VERSION);
    memcpy(payload + index, APP_VERSION, strlen(APP_VERSION));
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
    CCOUP_FMT_DEBUG("login_to_smiOS() 登陆smiOS...通过MQTT发送的消息为: (topic:%s) \n", topic);
    PrintInfoDetail(MsgSend);
    CCOUP_FMT_DEBUG("Send Up IPC data... SendLen=%d \n", SendLen);
    CCOUP_BUF_DEBUG(SendBuf, SendLen, "发送mqtt报文:");
    mqtt_client_send_msg(g_MqttClient, topic, 0, SendBuf, SendLen);
    free(MsgSend);

}

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
	CCOUP_FMT_DEBUG("heart_beat() 心跳回复...通过MQTT发送的消息为 (topic:%s) \n", topic);
	PrintInfoDetail(MsgSend);
	CCOUP_FMT_DEBUG("Send Up IPC data... SendLen=%d \n", SendLen);
	CCOUP_BUF_DEBUG(SendBuf, SendLen, "发送mqtt报文:");
    mqtt_client_send_msg(g_MqttClient,topic,0,SendBuf, SendLen);
    free(MsgSend);
   
}

//事件内容上报smiOS
void event_to_smiOS(uint8 *pPayloadData, uint32 nPayloadLen)
{
	uint8 offset = 0;
    uint8 SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;
	char topic[100] ;
    memset(topic,0x00,sizeof(topic));
    sprintf(topic,"%s/Broadcast/0003/0001", appName);

    MsgSend = (MSG_INFO_T*)malloc(sizeof(MSG_INFO_T) + nPayloadLen);
	memset(MsgSend,0x00,sizeof(MSG_INFO_T) + nPayloadLen);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgIndex = GetInfoIndex();
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0003;
    MsgSend->IOP = 0x0001;
    
    MsgSend->SourLen = strlen(appName);
    memcpy(MsgSend->SourAddr,appName,MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr,smiosName,MsgSend->DestLen );
	
    MsgSend->MsgLen = nPayloadLen;
    memcpy(&MsgSend->MsgData[offset], pPayloadData, nPayloadLen);
    SendLen = msg_mqtt_package(MsgSend, MAX_QUEUE_BUFF_LEN, SendBuf);

	//打印完整消息内容
	CCOUP_FMT_DEBUG("event_to_smiOS() 事件上报smiOS...通过MQTT发送的消息为 (topic:%s) \n", topic);
	PrintInfoDetail(MsgSend);
	CCOUP_FMT_DEBUG("Send Up IPC data... SendLen=%d \n", SendLen);
	CCOUP_BUF_DEBUG(SendBuf, SendLen, "发送mqtt报文:");
    mqtt_client_send_msg(g_MqttClient,topic,0,SendBuf, SendLen);
    free(MsgSend);
}

/* 福建事件内容上报smiOS */
void CcoEventToSmiOS(uint8 *pPayloadData, uint32 nPayloadLen)
{
    uint8 offset = 0;
    uint8 SendBuf[MAX_QUEUE_BUFF_LEN] = {0};
    uint16 SendLen;
    MSG_INFO_T *MsgSend = NULL;
    char topic[100] ;
    memset(topic, 0x00, sizeof(topic));
    sprintf(topic, "%s/%s", appName, smiosName);

    MsgSend = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T) + nPayloadLen);
    memset(MsgSend, 0x00, sizeof(MSG_INFO_T) + nPayloadLen);
    MsgSend->MsgRPM = 1;
    MsgSend->MsgIndex = GetInfoIndex();
    MsgSend->MsgPRIORITY = 1;
    MsgSend->MsgLabel = 1;
    MsgSend->IID = 0x0003;
    MsgSend->IOP = 0x0003;
    
    MsgSend->SourLen = strlen(appName);
    memcpy(MsgSend->SourAddr, appName, MsgSend->SourLen);
    MsgSend->DestLen = strlen(smiosName);
    memcpy(MsgSend->DestAddr, smiosName, MsgSend->DestLen);
    
    MsgSend->MsgLen = nPayloadLen;
    memcpy(&MsgSend->MsgData[offset], pPayloadData, nPayloadLen);
    SendLen = msg_mqtt_package(MsgSend, MAX_QUEUE_BUFF_LEN, SendBuf);

    //打印完整消息内容
    CCOUP_FMT_DEBUG("CcoEventToSmiOS() 事件上报smiOS...通过MQTT发送的消息为 (topic:%s) \n", topic);
    PrintInfoDetail(MsgSend);
    CCOUP_FMT_DEBUG("Send Up IPC data... SendLen=%d \n", SendLen);
    CCOUP_BUF_DEBUG(SendBuf, SendLen, "发送mqtt报文:");
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
	CCOUP_FMT_DEBUG("module_from_mapManager() 查询模组信息...通过MQTT发送的消息为 (topic:%s) \n", topic);
	PrintInfoDetail(MsgSend);
	CCOUP_FMT_DEBUG("Send Up IPC data... SendLen=%d \n", SendLen);
	CCOUP_BUF_DEBUG(SendBuf, SendLen, "发送mqtt报文:");
    mqtt_client_send_msg(g_MqttClient,topic,0,SendBuf, SendLen);
    free(MsgSend);
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
    //uint32 appversion = APP_VERSION;
    Date_Time_S publishtime;
    memset(&publishtime,0x00,sizeof(Date_Time_S));
    //memcpy_r(&payload[index],(uint8*)&appversion,4);
    //index+=4;
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
    
    MSG_INFO_T *pMsgInfoSend = NULL;
    char topic[500] = {0};  //主题
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
	CCOUP_FMT_DEBUG("Appversion_Response()查询app版本信息...通过MQTT发送的消息为: (topic:%s) \n", topic);
	PrintInfoDetail(pMsgInfoSend);

	//组消息
	SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);
	CCOUP_FMT_DEBUG("Send Up IPC data... SendInfoLen=%d \n", SendInfoLen);
	CCOUP_BUF_DEBUG(SendInfoBuf, SendInfoLen, "发送mqtt报文:");

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
	char topic[500] = {0};  //主题
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
	CCOUP_FMT_DEBUG("Factoryversion_Response()查询厂商版本信息...通过MQTT发送的消息为: (topic:%s) \n", topic);
	PrintInfoDetail(pMsgInfoSend);
	CCOUP_FMT_DEBUG("Send Up IPC data... SendInfoLen=%d \n", SendInfoLen);
	CCOUP_BUF_DEBUG(SendInfoBuf, SendInfoLen, "发送mqtt报文:");

	//组消息
	SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);

	//发送
    mqtt_topic_write(SendInfoBuf, SendInfoLen, topic);
	
    free(pMsgInfoSend);
	pMsgInfoSend = NULL;
	
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
	CCOUP_FMT_DEBUG("check_app_login_state() 检查 %s 是否已注册...通过MQTT发送的消息为 (topic:%s) \n", AppName, topic);
	PrintInfoDetail(MsgSend);
	CCOUP_BUF_DEBUG(SendBuf, SendLen, "完整发送:");
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
					CCOUP_FMT_DEBUG("unpack_login_info()查询app注册状态: AppName=%s已注册完成! \n", AppName);
					return 1;
				}
			}
		}
	}

	CCOUP_FMT_DEBUG("unpack_login_info()查询app注册状态: AppName=%s失败! MsgRecv->MsgIndex=%d, SendIndex=%d, MsgRecv->MsgLen=%d\n", 
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
        CCOUP_FMT_DEBUG("appid_query() hal_device_get() 获取设备失败! \n");
	}
	else
	{
    	if(dev->devinfo_read_appid(dev, appname, appid, maxlen) < 0)
    	{
    	    memset(appid, 0x00, maxlen);
        	CCOUP_FMT_DEBUG("appid_query() devinfo_read_appid() 获取appid失败! \n");
    	}
		else
		{
			ret = 1;
			CCOUP_FMT_DEBUG("appid_query() devinfo_read_appid() 获取appid成功! \n");
		}
		CCOUP_BUF_DEBUG(appid, maxlen, "%s的appid为: ", appname);
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
	char topic[500] = {0};  //主题
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
	CCOUP_FMT_DEBUG("appid_response()查询appid...通过MQTT发送的消息为: (topic:%s) \n", topic);
	PrintInfoDetail(pMsgInfoSend);

	//组消息
	SendInfoLen = msg_mqtt_package(pMsgInfoSend, MAX_QUEUE_BUFF_LEN, SendInfoBuf);
	CCOUP_FMT_DEBUG("Send Up IPC data... SendInfoLen=%d \n", SendInfoLen);
	CCOUP_BUF_DEBUG(SendInfoBuf, SendInfoLen, "发送mqtt报文:");

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

/**********************************************************************
* @name      : mqtt_topic_send_msg
* @brief     ：对外消息发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int mqtt_topic_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
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
        CCOUP_FMT_DEBUG("App send appmsg_package fail, IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        return -1;
    }

    if(msglen >= 5120)
    {
        CCOUP_FMT_DEBUG("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

    /* 两端都是MQTT */
    if(NULL != pTopic)
    {
        ret = mqtt_client_send_msg(g_MqttClient, pTopic, qos, pMsg, (int)msglen);
        CCOUP_FMT_DEBUG("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);
    }
    else
    {
        ret = mqtt_client_send(g_MqttClient, pMsg, (int)msglen);
    }


    if(0 == ret)
    {
        CCOUP_BUF_DEBUG(pMsg, msglen, "Send msg:");
    }
    else
    {
        CCOUP_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                        pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}



