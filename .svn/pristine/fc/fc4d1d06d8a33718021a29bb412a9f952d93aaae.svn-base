/*
*********************************************************************
* Copyright(C) 2022 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2022-3-20
*********************************************************************
*/
#ifndef __RSP_MQTT_H__
#define __RSP_MQTT_H__


#ifdef __cplusplus
extern "C" {
#endif


int rsp_mqtt_init(uint32 ip, uint16 port);
uint16 rsp_get_next_index();
int rsp_recv_mqtt_msg(char *pTopic, uint16 topiclen, uint8 *pBuf, int buflen);
int rsp_send_mqtt_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
int rsp_send_mapmsg(uint16 label, uint16 iid, uint16 iop, uint8* playload, uint16 length);
int rsp_login_to_smiOS();
int rsp_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int rsp_version_msg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int rsp_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode);


#ifdef __cplusplus
}
#endif

#endif // __RSP_MQTT_H__

