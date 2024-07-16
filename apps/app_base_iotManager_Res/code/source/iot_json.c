/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监测主线程功能
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include"iot_init.h"
#include"iot_public.h"
#include "cJSON.h"

char jsonalarmname[3][10]={"无","越限","恢复"};
char jsonoticsys_a[5][10]={"CPU","内存","磁盘","温度","异常"};
char jsonoticcon_a[5][10]={"CPU","内存","磁盘","重启","异常"};
char jsonoticapp_a[4][10]={"CPU","内存","重启","异常"};
char jsonoticcon_n[6][10]={"安装","升级","卸载","启动","停止","配置参数"};
char result[2][10] = {"成功","失败"};
char jsonoticapp_n[9][10]={"安装","升级","卸载","启动","停止","配置参数","使能","去使能","下发许可"};


void get_dataTime_s(char *sourcetime,OOP_DATETIME_S_T *StartTime)
{
	if(strlen(sourcetime)==0)
		return ;
	char tempbuff[10] = {0};
	char *tail = NULL;
	char *head = NULL;
	head = sourcetime;
	tail = strstr(head,"-");
	if(tail != NULL)
	{
		memcpy(tempbuff,head,tail-head);
		StartTime->year = atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
	}
	tail ++;
	head = tail;
	tail = strstr(head,"-");
	if(tail != NULL)
	{
		memcpy(tempbuff,head,tail-head);
		StartTime->month = atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
	}
	tail ++;
	head = tail;
	tail = strstr(head," ");
	if(tail != NULL)
	{
		memcpy(tempbuff,head,tail-head);
		StartTime->day = atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
	}
	tail ++;
	head = tail;
	tail = strstr(head,":");
	if(tail != NULL)
	{
		memcpy(tempbuff,head,tail-head);
		StartTime->hour = atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
	}
	tail ++;
	head = tail;
	tail = strstr(head,":");
	if(tail != NULL)
	{
		memcpy(tempbuff,head,tail-head);
		StartTime->minute = atoi(tempbuff);
		memset(tempbuff,0,sizeof(tempbuff));
	}
	tail ++;
	head = tail ;
	tail = &head[strlen(head)];
	memcpy(tempbuff,head,tail-head);
	StartTime->second = atoi(tempbuff);
}


int json_mess_split(char *pBuf,JSON_REPORT_DATA *data)
{
	if(strlen(pBuf)== 0)
		return -1;
	cJSON  *root  = NULL;
	cJSON  *tempjson  = NULL;
	root = cJSON_Parse(pBuf);

	tempjson = cJSON_GetObjectItem(root,"container");
	if(tempjson)
	{
		strcpy(data->container,tempjson->valuestring);
	}
	tempjson = cJSON_GetObjectItem(root,"app");
	if(tempjson)
	{
		strcpy(data->app,tempjson->valuestring);
	}
	tempjson = cJSON_GetObjectItem(root,"datetime");
	if(tempjson)
	{
		strcpy(data->datetime,tempjson->valuestring);
	}
	tempjson = cJSON_GetObjectItem(root,"msg");
	if(tempjson)
	{
		strcpy(data->msg,tempjson->valuestring);
	}
	tempjson = cJSON_GetObjectItem(root,"reason");
	if(tempjson)
	{
		strcpy(data->reason,tempjson->valuestring);
	}
	tempjson = cJSON_GetObjectItem(root,"alarmType");
	if(tempjson)
	{
		data->alarmType = tempjson->valueint;
	}
	tempjson = cJSON_GetObjectItem(root,"notifyType");
	if(tempjson)
	{
		data->notifyType = tempjson->valueint;
	}
	tempjson = cJSON_GetObjectItem(root,"result");
	if(tempjson)
	{
		data->result = tempjson->valueint;
	}
	cJSON_Delete(root);
	return 0;
}

void operate_report_pack(JSON_REPORT_DATA *data,char *device ,char *sendbuff,WGPubattr *public)
{
    uint16 offset=0;
    uint8 buff[APP_RECV_BUF_LEN]={0};
    uint8 apptype = 0;
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
    buff[offset++] = strlen(data->container);
    memcpy(&buff[offset],data->container,strlen(data->container));
	offset+=strlen(data->container);
	if(strcmp(device,"app")==0)
	{
	    buff[offset++] = strlen(data->app);
	    memcpy(&buff[offset],data->app,strlen(data->app));
		offset+=strlen(data->app);
        apptype = 1;
	}
	get_dataTime_s(data->datetime,&StartTime);
	memcpy_r(&buff[offset],&StartTime.year,sizeof(uint16));
    offset+=sizeof(uint16);
	buff[offset++] = StartTime.month;
	buff[offset++] = StartTime.day;
	buff[offset++] = StartTime.hour;
	buff[offset++] = StartTime.minute;
	buff[offset++] = StartTime.second;
	buff[offset++] = data->notifyType;
	buff[offset++] = data->result;
	if(strlen(data->reason)!=0)
	{
	    buff[offset++] = 1;
        buff[offset++] = strlen(data->reason);
	    memcpy((char *)&buff[offset],data->reason,strlen(data->reason));
		offset+=strlen(data->reason);
	}
    else
    {
        buff[offset++] = 0;
    }
    MSG_INFO_T* msg  = malloc(sizeof(MSG_INFO_T)+offset);    
    if(msg==NULL)
        return;
    memset(msg,0,sizeof(MSG_INFO_T)+offset);
    msg->MsgRPM = 1;
    msg->MsgPRIORITY = 1;
    msg->MsgIndex = iot_get_next_index();
    msg->MsgLabel = 0;
    
    msg->SourLen = strlen(WUGUAN_APP_NAME_M);
    memcpy(msg->SourAddr, WUGUAN_APP_NAME_M, msg->SourLen);

    msg->DestLen = strlen("Broadcast");
    strcpy(msg->DestAddr, "Broadcast");
    msg->IID = 0x000B;
	if(strcmp(device,"app")==0)
	{
		msg->IOP = 0x0003;
	}else if(strcmp(device,"container")==0)
	{
		msg->IOP = 0x0001;
	}else 
	{
		RPT_FMT_DEBUG("错误的设备类型 %s \n",device);
	}
    msg->MsgLen = offset;
    memcpy(msg->MsgData,buff,offset);
    if(apptype == 0 && data->notifyType < 6)
    {
        RPT_FMT_DEBUG("JSON上报解析 \n 容器[%s]执行[%s][%s]\n",data->container,jsonoticcon_n[data->notifyType],result[data->result]);
    }
    else
    {     
        RPT_FMT_DEBUG("JSON上报解析 \n 容器[%s]APP[%s]执行[%s][%s]msg[%s]\n",data->container,data->app,jsonoticapp_n[data->notifyType],result[data->result],data->reason);
    }
    if(apptype == 1 && data->notifyType == 1)
    {
        RPT_FMT_DEBUG("延时10s等待集抄上线\n");
        sleep(10);
    }
	if(taskmng_plc_queue_write(&public->upque,(uint8*)msg,sizeof(MSG_INFO_T)+msg->MsgLen) ==0 )
	{
        RPT_FMT_DEBUG("消息发送失败 \n");
    }
	free(msg);
}

void alarm_report_pack(JSON_REPORT_DATA *data,char *device ,char *sendbuff,WGPubattr *public)
{
    uint16 offset=0;
    uint8 buff[APP_RECV_BUF_LEN]={0};
    uint8 devvicetype = 0;
    OOP_DATETIME_S_T StartTime;
    memset(&StartTime,0x00,sizeof(OOP_DATETIME_S_T));
	if(strcmp(device,"sys") != 0)
	{
		buff[offset++] = strlen(data->container);
		memcpy(&buff[offset],data->container,strlen(data->container));
		offset+=strlen(data->container);
	}
    else
    {
        return ;//json上报的系统异常告警不在上报，自己判断
    }

	if(strcmp(device,"app")==0)
	{
	    buff[offset++] = strlen(data->app);
	    memcpy(&buff[offset],data->app,strlen(data->app));
		offset+=strlen(data->app);
	}
	get_dataTime_s(data->datetime,&StartTime);
    memcpy_r(&buff[offset],&StartTime.year,sizeof(uint16));
    offset += sizeof(uint16);
	buff[offset++] = StartTime.month;
	buff[offset++] = StartTime.day;
	buff[offset++] = StartTime.hour;
	buff[offset++] = StartTime.minute;
	buff[offset++] = StartTime.second;
	buff[offset++] = data->alarmType;
	buff[offset++] = data->notifyType;
	if(strlen(data->msg)!=0)
	{
	    buff[offset++] = 1;
        buff[offset++] = strlen(data->msg);
	    memcpy(&buff[offset],data->msg,strlen(data->msg));
		offset+=strlen(data->msg);
	}
    else
    {
        buff[offset++] = 0;
    }
    MSG_INFO_T* msg  = malloc(sizeof(MSG_INFO_T)+offset);    
    if(msg==NULL)
        return;
    memset(msg,0,sizeof(MSG_INFO_T)+offset);
    msg->MsgRPM = 1;
    msg->MsgPRIORITY = 1;
    msg->MsgIndex = iot_get_next_index();
    msg->MsgLabel = 0;
    
    msg->SourLen = strlen(WUGUAN_APP_NAME_M);
    memcpy(msg->SourAddr, WUGUAN_APP_NAME_M, msg->SourLen);

    msg->DestLen = strlen("Broadcast");
    strcpy(msg->DestAddr, "Broadcast");
    msg->IID = 0x000B;
	if(strcmp(device,"container")==0)
	{
		msg->IOP = 0x0002;
        devvicetype = 0;
	}else if(strcmp(device,"app")==0)
	{
		msg->IOP = 0x0004;
        devvicetype = 1;
	}else if(strcmp(device,"sys")==0)
	{
		msg->IOP = 0x0005;
        devvicetype = 2;
	}else 
	{
		RPT_FMT_DEBUG("错误的设备类型 [%s] \n",device);
	}
    msg->MsgLen = offset;
    memcpy(msg->MsgData,buff,offset);
    if(devvicetype == 0)
    {
        RPT_FMT_DEBUG("JSON上报解析 \n 容器[%s]告警[%s][%s]告警描述[%s]\n",data->container,jsonoticcon_a[data->notifyType],jsonalarmname[data->alarmType],data->msg);
    }
    else if(devvicetype == 1 && data->notifyType < 4)
    {
        RPT_FMT_DEBUG("JSON上报解析 \n 容器[%s]APP[%s]告警[%s][%s]告警描述[%s]\n",data->container,data->app,jsonoticapp_a[data->notifyType],jsonalarmname[data->alarmType],data->msg);
    }
    else
    {
        RPT_FMT_DEBUG("JSON上报解析 \n 系统告警[%s][%s]告警描述[%s]\n",jsonoticsys_a[data->notifyType],jsonalarmname[data->alarmType],data->msg);
    }
	taskmng_plc_queue_write(&public->upque,(uint8*)msg,sizeof(MSG_INFO_T)+msg->MsgLen);
	free(msg);
}


void json_report_process(char *pBuf,char *report,char *device,WGPubattr *public)
{
	JSON_REPORT_DATA reportdata;
	memset(&reportdata,0,sizeof(reportdata));
	char sendbuff[2048] = {0} ;
	if(json_mess_split(pBuf,&reportdata) < 0)
		return ;
	if(strcmp(report,"operate")==0)
	{		
		 operate_report_pack(&reportdata,device,sendbuff,public);
	}else if(strcmp(report,"alarm")==0)
	{
		 alarm_report_pack(&reportdata,device,sendbuff,public);
	}
	//写到队列里
//	taskmng_plc_queue_write(&LockRecvMsg,sendlen);
}


uint8 report_type_judgmen(char *report,char *device)
{
	if(strlen(report)==0 || strlen(device)==0)
		return 4;
	if(strcmp(report,"alarm")==0 || strcmp(report,"operate")==0 )
	{
		if(strcmp(device,"sys")==0 || strcmp(device,"container")==0 || strcmp(device,"app")==0)
		{
			return 0;
		}
		else 
		{
			return  2;
		}
	}
	else 
	{
		return 1;
	}
	return 0;
}

int topic_type_split(char *topic,char *report,char *device)
{
	if(topic==NULL)
		return -1;
	//operate/broadcast/notify/app
	char   *split_head = NULL;
    char   *split_tail = NULL;
    /*上报类型*/
	split_head = topic;
    split_tail = strstr(split_head,"/");
    if(split_tail == NULL)
    {
    //	MQTTMSG_FMT_DEBUG("ERROR point \n");
		return -1;
	}
   	memcpy(report,split_head,split_tail-split_head);
//ls	MQTTMSG_FMT_DEBUG("report %s\n", report);
    split_tail++;
	split_head = split_tail;
	/*broadcast*/
    split_tail = strstr(split_head,"/");
    if(split_tail == NULL)
    {
    //	MQTTMSG_FMT_DEBUG("ERROR point \n");
		return -1;
	}

	split_tail++;
	/*notify*/
    split_head  = split_tail;
    split_tail = strstr(split_head,"/");
    if(split_tail == NULL)
    {
    //	MQTTMSG_FMT_DEBUG("ERROR point \n");
		return -1;
	}
    
	split_tail++;
    split_head  = split_tail;
    /*设备类型*/
    split_tail = &topic[strlen(topic)];
    memcpy(device,split_head,split_tail-split_head);
	//MQTTMSG_FMT_DEBUG("report %s\n", device);
	return 0;
}


void json_report_deal(WG_MSGDATA_T *pMsgData,WGPubattr *public)
{
	if(strlen(pMsgData->topic)==0 || strlen((char *)pMsgData->recvBuf)==0)
		return;
	char device[20] = {0} ;   // 上报的设备类型  container app device
	char report[20] = {0};    //上报类型  aralm opera
	uint8 ret=0;
	if(topic_type_split(pMsgData->topic,report,device)==-1)
	{
		RPT_FMT_DEBUG("local json repoty topic error \n",pMsgData->topic);
	}
	else
	{
//		MQTTMSG_FMT_DEBUG("上报类型 %s 设备类型%s  \n",report,device);
		ret = report_type_judgmen(report,device);
		if(ret  == 1)
		{
			RPT_FMT_DEBUG("上报类型错误 ret [%d] [%s] \n",ret,report);
		}
		else if(ret  == 2)
		{
			RPT_FMT_DEBUG("上报设备类型错误 ret [%d] [%s] \n",ret,device);
		}
		if(ret > 0)
			return;
		json_report_process((char*)pMsgData->recvBuf,report,device,public);
	}
}


int iot_jsonproc(void *pArg, void *pPublic, void *pPrivate)
{
    int RecvLen;
    WGPubattr *public    =  (WGPubattr *)pPublic;
    WG_MSGDATA_T *pMsg = NULL;
    uint8 recvBuff[2173] = {0};
    RecvLen = taskmng_plc_queue_read(&public->jsonque, recvBuff);
    if(RecvLen > 0)
    {
        pMsg = (WG_MSGDATA_T *)recvBuff;
        RPT_FMT_DEBUG("接收到json上报的事件 topic %s\n  %s \n",pMsg->topic,pMsg->recvBuf);
        json_report_deal(pMsg,pPublic);
    }
    return 10000;
}


int iot_jsoninit(void *pPublic, void **ppPrivate)
{
    RPT_FMT_DEBUG("json 线程启用 \n");
    return 0;
}


