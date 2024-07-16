/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线拨号mqtt消息处理
* @date：    2020-1-9
* @history： 
*********************************************************************
*/
#ifndef __WIRE_MQTT_H__
#define __WIRE_MQTT_H__

#ifdef __cplusplus
extern "C" {
#endif


int wireless_mqtt_init(uint32 ip, uint16 port);
void wireless_mqtt_uninit();
uint16 wireless_get_next_index();
int wireless_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
int wireless_recv_msg(char *pTopic, uint8 *pBuf, uint16 buflen);
int wireless_login_to_smios(uint8 *pAppId, uint8 appidlen);
int wireless_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int wireless_get_versionmsg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int wireless_get_selfversion(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int wireless_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);
int wireless_send_modmsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length);
int wireless_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode);
int wireless_send_guimsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length);


#ifdef __cplusplus
}
#endif

#endif //__WIRE_MQTT_H__
