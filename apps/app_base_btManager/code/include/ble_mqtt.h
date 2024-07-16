/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP MQTT操作
* @date：    2021-7-8
* @history： 
*********************************************************************
*/

#ifndef __BLE_MQTT_H__
#define __BLE_MQTT_H__

#ifdef __cplusplus
extern "C" {
#endif
extern char g_AppName[APPMSG_NAME_LEN];
extern char g_AppVer[APPMSG_NAME_LEN];

int ble_mqtt_init(uint32 ip, uint16 port);
uint16 ble_get_next_index();
int ble_send_axdr_msg(MESSAGE_INFO_T *pMsgAXDR, int qos);
int ble_send_axdr_msg_broadcast(MESSAGE_INFO_T *pMsgAXDR, int qos, uint16 iid, uint16 iop);
int ble_send_json_msg(char *topic, int qos, char *pMsg, int msglen);
int ble_recv_mqtt(char *pTopic, uint16 topiclen, uint8 *pBuf, uint16 buflen);
int ble_login_to_sys();
int ble_login_to_smios(uint8 *pAppId, uint8 appidlen);
int ble_heart_bear(MESSAGE_JSON_T *pInMsg);
int ble_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode);
int ble_err_json_res(MESSAGE_JSON_T *pInMsg, uint8 errcode, char *err);
int ble_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos);
int bel_state_guishow(uint8 status);

#ifdef __cplusplus
}
#endif

#endif //__BLE_MQTT_H__

