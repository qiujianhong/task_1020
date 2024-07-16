/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP MQTT操作
* @date：    2021-7-8
* @history： 
*********************************************************************
*/

#ifndef __RADIO_MQTT_H__
#define __RADIO_MQTT_H__

#ifdef __cplusplus
extern "C" {
#endif
extern char g_AppName[APPMSG_NAME_LEN];
extern char g_AppVer[APPMSG_NAME_LEN];

int radio_mqtt_init(uint32 ip, uint16 port);
uint16 radio_get_next_index();
int radio_send_axdr_msg(MESSAGE_INFO_T *pMsgAXDR, int qos);
int radio_send_axdr_msg_broadcast(MESSAGE_INFO_T *pMsgAXDR, int qos, uint16 iid, uint16 iop);
int radio_send_json_msg(char *topic, int qos, char *pMsg, int msglen);
int radio_recv_mqtt(char *pTopic, uint16 topiclen, uint8 *pBuf, uint16 buflen);
int radio_login_to_sys();
int radio_login_to_smios(uint8 *pAppId, uint8 appidlen);
int radio_heart_bear(MESSAGE_JSON_T *pInMsg);
int radio_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode);
int radio_err_json_res(MESSAGE_JSON_T *pInMsg, uint8 errcode, char *err);
int radio_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos);
int radio_state_guishow(uint8 status);
int radio_connectionstate_guishow(char *str, uint16 len, uint32 times);
#ifdef __cplusplus
}
#endif

#endif //__RADIO_MQTT_H__

