
/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/

#ifndef __AC_METER_MQTT_H__
#define __AC_METER_MQTT_H__

#include "ac_redef.h"
#include "mqtt_client.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


/*******************************************************************************
* 全局变量
*******************************************************************************/

// 初始化标志
extern uint8 gMqttInitFlag;

//模组管理器APP名称------<测试修改>------
extern char *mapManagerName;

//数据中心APP名称------<测试修改>------
extern char *dbName;

extern char *appName;

extern MQTT_CLIENT_T *g_MqttClient;

/*******************************************************************************
* 枚举和结构体
*******************************************************************************/


/*******************************************************************************
* 函数声明
*******************************************************************************/
extern int mqtt_init();
extern void login_to_smiOS(void);
void reboot_to_smiOS(void);
extern void heart_beat(uint16 index);
extern void module_from_mapManager(uint8 *pPayloadData, uint32 nPayloadLen, uint16 *IID, uint16 *IOP, uint16 *index);
extern int info_to_dbCenter(uint16 label, uint16 iop, uint8* payload, uint16 len, uint16* pindex);
extern void Appversion_Response(MSG_INFO_T *MsgRecv);
extern void Factoryversion_Response(MSG_INFO_T *MsgRecv);
extern int make_resp_info(MSG_INFO_T *ptMsgInfo, uint16 BuffLen, uint8 *Buff);
extern void check_app_login_state(char* AppName, uint16 *SendIndex);
extern int unpack_login_info(MSG_INFO_T *MsgRecv, char *AppName, uint16 SendIndex);
extern int appid_query(const char* appname, char* appid, int maxlen);
extern void appid_response(MSG_INFO_T *MsgRecv);

extern int mqtt_read(uint8* RecvBuf);
extern int mqtt_topic_read(char* topic, int *qos, int *mid, uint8* RecvBuf);
extern int mqtt_topic_write(uint8* SendBuf, uint32 SendLen, char* topic);
extern int mqtt_msg_send(MQTT_CLIENT_T *mqttClient, MESSAGE_MQTT_T *pMsgMqtt, int qos);
extern void PrintInfoDetail(MSG_INFO_T *stMsgInfoSend);
extern void PrintInfoDetail_DEBUG(MSG_INFO_T *stMsgInfoSend);
extern void print_info_msg(MSG_INFO_T *stMsgInfoSend);
extern void PrintInfoDetailDB(MESSAGE_INFO_T *pMsgInfo);
extern uint16 GetInfoIndex();


#ifdef __cplusplus
}
#endif

#endif // __AC_METER_MQTT_H__

