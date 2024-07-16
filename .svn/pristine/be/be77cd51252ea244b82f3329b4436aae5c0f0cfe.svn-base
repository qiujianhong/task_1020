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
#ifndef	__LC_MQTT_H
#define __LC_MQTT_H
    
#ifdef __cplusplus
        extern "C" {
#endif


int lc_mqtt_init(LC_APPDATA_T *pData);
int lc_login_to_smios(uint8 *pAppId, uint8 appidlen);
int lc_send_mqttmsg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
int lc_send_msg(MESSAGE_MQTT_T *pMsgMqtt, int qos);
int lc_send_uartmsg(uint16 iop, uint8* playload, uint16 length);
int lc_send_uart_syncmsg(uint16 iop, uint8* playload, uint16 length, uint8 *pRecvbuf, uint16 buflen, uint8 outtime, uint16 *recvLen);
int lc_send_mapmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int lc_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
#ifdef PRODUCT_SCU
int lc_send_dbmsg(uint16 label, uint16 index, uint16 iop, uint8* playload, uint16 length);
#else
int lc_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);
#endif
int lc_send_appmsg(MESSAGE_INFO_T *pMsgInfo);
int lc_heart_bear(MESSAGE_INFO_T *pInMsg);
int lc_get_version(MESSAGE_INFO_T *pInMsg);
int lc_get_appidmsg(MESSAGE_INFO_T *pInMsg, uint8 *pAppId, uint8 appidlen);
int lc_seek_app();
int lc_recv_regapp(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
uint8 lc_dbCenter_flag();
uint8 lc_appmap_flag();
uint8 lc_appgui_flag();
int lc_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode);
int lc_get_selfversion(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);

int lc_recv_mqttmsg(char *topic, uint16 topicLen, uint8 *pBuf, uint16 buflen);
void lc_mqtt_debug();
int lc_login_to_sys();
int lc_heart_to_sys(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic);
int lc_get_appreglist();
uint8 lc_appreglist_check(MESSAGE_JSON_T *pInMsg, void *pArg, void *pPublic);

#ifdef __cplusplus
}
#endif

#endif

