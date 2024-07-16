/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   配网对外消息处理
* @date：    2020-3-4
* @history： 
*********************************************************************
*/
#ifndef __MQTT_PROC_H__
#define __MQTT_PROC_H__

#ifdef __cplusplus
extern "C" {
#endif

int advPd_mqtt_init(uint32 ip, uint16 port);
void advPd_outmsg_uninit();
int advPd_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);
int advPd_send_ctsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advPd_send_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advPd_send_modmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advPd_send_rspmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advPd_send_smios(uint16 label, uint16 iop, uint8* playload, uint16 length);
int advPd_login_to_smios();
int advPd_heart_bear(MESSAGE_INFO_T *pInMsg);
int advPd_get_version(MESSAGE_INFO_T *pInMsg);
int advPd_get_appidmsg(MESSAGE_INFO_T *pInMsg);
int advPd_seek_app();
BOOLEAN advPd_recv_regapp(MESSAGE_INFO_T *pInMsg);
int advPd_recv_msg(uint8 *pBuf, uint16 buflen);
BOOLEAN advPd_start_goon();
DB_AGENT advPd_get_dbagent();
int advPd_mqtt_reproc(void *pMsg, int msglen);

#ifdef __cplusplus
}
#endif

#endif //__MQTT_PROC_H__

