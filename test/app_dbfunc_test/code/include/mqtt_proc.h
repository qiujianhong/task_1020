/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   配网对外消息处理
* @date：    2020-3-4
* @history： 
*********************************************************************
*/
#ifndef __MQTT_PROC_H__
#define __MQTT_PROC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define    APP_NAME             "M-dbfunctest"
#define    APP_SUBTOPIC         "+/M-dbfunctest"
#define    DBCENTER_NAME       "M-dbCenter"

// 接收消息 客户端句柄
extern XL_CHANNEL_CTX_T *g_MqttMsgCtx;
extern XL_CHANNEL_CTX_T *g_RecvMqttMsgCtx;
extern XL_CHANNEL_CTX_T *g_dbMqttMsgCtx[MAX_THREAD];


int app_mqtt_init(uint32 ip, uint16 port);
void app_outmsg_uninit();
int app_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);
int app_recv_msg(uint8 *pBuf, uint16 buflen);
BOOLEAN app_start_goon();
int app_send_mqttmsg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);

int db_mqtt_init(XL_CHANNEL_CTX_T ** cxt, char * app_name);
void db_outmsg_uninit(XL_CHANNEL_CTX_T * cxt);
int db_recv_msg(XL_CHANNEL_CTX_T *pctx, uint8 *pBuf, uint32 buflen);
int db_send_mqttmsg(XL_CHANNEL_CTX_T * cxt, char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
int app_send_msg1(XL_CHANNEL_CTX_T * cxt, MESSAGE_INFO_T *pInMsg, void *sendMsg);

int db_send_dbmsg(uint8 thread_id, uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);


#ifdef __cplusplus
}
#endif

#endif //__MQTT_PROC_H__

