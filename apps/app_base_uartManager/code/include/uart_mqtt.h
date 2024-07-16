/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控MQTT接口
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef	__UART_MQTT_H
#define __UART_MQTT_H
    
#ifdef __cplusplus
        extern "C" {
#endif


int uart_mqtt_init(UART_APPDATA_T *pData);
int uart_login_to_smios(uint8 *pAppId, uint8 appidlen);
int uart_send_axdrmsg(MESSAGE_INFO_T *pMsgInfo, int qos);
int uart_send_jsonmsg(MESSAGE_JSON_T *pMsgInfo, int qos);
int uart_send_mqttmsg(char *pTopic, int qos, void *msgdata, int msglen);
int uart_send_mapmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int uart_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int uart_send_dbmsg(uint16 label, uint16 index, uint16 iop, uint8* playload, uint16 length);
int uart_send_appmsg(MESSAGE_INFO_T *pMsgInfo);

int uart_seek_app();
int uart_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos);
int uart_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode);
int uart_recv_mqttmsg(char *topic, uint16 topicLen, uint8 *pBuf, uint16 buflen);
int uart_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_get_version(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_get_appidmsg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_login_to_smios(uint8 *pAppId, uint8 appidlen);
int uart_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_recv_regapp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_notify_send(uint16 index, uint16 IOP, char *destApp, uint8 *buffer, uint16 length);
int uart_modeinfo_resp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_slotinfo_resp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_modchange_evt(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_slotinfo_send(const char *type);
int uart_modinfo_send(void);

uint8 uart_dbCenter_flag();
uint8 uart_appmap_flag();
uint8 uart_appgui_flag();
int uart_get_selfversion(int sockid, MESSAGE_INFO_T *pInMsg);

void uart_mqtt_debug();
int uart_login_to_sys();
int uart_heart_to_sys(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic);
int uart_get_appreglist();
uint8 uart_appreglist_check(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic);
uint16 uart_get_next_index();

#ifdef __cplusplus
}
#endif

#endif

