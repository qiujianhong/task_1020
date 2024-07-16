#ifndef _MQTT_H_
#define _MQTT_H_

#include "mqtt_client.h"
#include "message.h"

#ifdef __cplusplus
extern "C" {
#endif

//appid: 对应app的ID信息
#define  APP_ID_LEN             24

extern char  AppId[APP_ID_LEN];
extern const char *appName;
extern const char *regAppName;
extern char *smiosName;
extern char *desktopName;
extern MQTT_CLIENT_T *g_MdlMqttClient;
extern pthread_mutex_t g_MsgSendlock;

extern int mqtt_init(uint32 ip, uint16 port);
extern void heart_beat(uint16 index);
extern void login_to_smiOS(void);
extern int mqtt_recv_msg(uint8* RecvBuf, uint16 datalen);
extern void mqtt_write(uint8*SendBuf,uint32 SendLen );
extern int mqtt_topic_write(uint8* SendBuf,uint32 SendLen,char* topic);
extern void appversion_response(MSG_INFO_T *MsgRecv);
extern int msg_mqtt_split(uint8 *pBuf, uint16 bufLen,uint32 ipAddr, uint16 port, MSG_INFO_T **ppMsgOut);
extern uint16 msg_mqtt_package(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf);
extern int mdlmgr_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
extern int appid_query(const char* appname, char* appid, int maxlen);
extern void appid_response(MSG_INFO_T *MsgRecv);

#ifdef __cplusplus
}
#endif

#endif
