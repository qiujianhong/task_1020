/*
*********************************************************************
* Copyright(C) 2024 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   负荷控制数据
* @date：    2024-06-27
* @history： 
*********************************************************************
*/
#ifndef __LOADCTRLLER_MQTT_H__
#define __LOADCTRLLER_MQTT_H__

#ifdef __cplusplus
    extern "C" {
#endif

#define LCTRL_APP_NAME             "M-loadCtrller"

int lctrl_mqtt_init(uint32 ip, uint16 port);

uint16 lctrl_get_next_index();
int lctrl_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
int lctrl_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode);
int lctrl_recv_msg(char *pTopic, uint8 *pBuf, uint16 buflen);
int lctrl_login_to_smios(uint8 *pAppId, uint8 appidlen);
int lctrl_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int lctrl_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);
void lctrl_get_dbCenter();
void lctrl_get_mapManager();
int lctrl_version_msg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int lctrl_send_backmsg(MESSAGE_INFO_T *pInMsg, uint8* playload, uint16 length);



#ifdef __cplusplus
}
#endif

#endif //__LA_MQTT_H__

