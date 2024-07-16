#ifndef _MQTT_H_
#define _MQTT_H_

#include "msgUdp.h"


//模组管理器APP名称------<测试修改>------
extern char *mapManagerName;

//数据中心APP名称------<测试修改>------
extern char *dbName;

extern int mqtt_init();
extern void heart_beat(uint16 index);
extern void login_to_smiOS(void);
extern void event_to_smiOS(uint8 *pPayloadData, uint32 nPayloadLen);
void CcoEventToSmiOS(uint8 *pPayloadData, uint32 nPayloadLen);
extern void module_from_mapManager(uint8 *pPayloadData, uint32 nPayloadLen, uint16 *IID, uint16 *IOP, uint16 *index);
extern void Appversion_Response(MSG_INFO_T *MsgRecv);
extern void Factoryversion_Response(MSG_INFO_T *MsgRecv);
extern void check_app_login_state(char* AppName, uint16 *SendIndex);
extern int unpack_login_info(MSG_INFO_T *MsgRecv, char *AppName, uint16 SendIndex);
extern int appid_query(const char* appname, char* appid, int maxlen);
extern void appid_response(MSG_INFO_T *MsgRecv);

extern int mqtt_read(uint8* RecvBuf);
extern int mqtt_topic_read(char* topic, int *qos, int *mid, uint8* RecvBuf);
extern int mqtt_topic_write(uint8* SendBuf, uint32 SendLen, char* topic);
extern int mqtt_topic_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);

#endif
