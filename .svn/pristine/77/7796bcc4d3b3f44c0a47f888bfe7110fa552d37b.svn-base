/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   APP交互消息解析
* @date：    2020/1/3
* @history： 
*********************************************************************
*/
#ifndef __APPMSG_SPLIT_H__
#define __APPMSG_SPLIT_H__

#include "message.h"
#include "appmsg_struct.h"

#ifdef __cplusplus
extern "C"
{
#endif

void *appmsg_memcpy_r(void *dest, void *src, int count);
uint16 appmsg_get_len_offset(uint8 *indata,uint8 *bytes);
uint8 appmsg_set_len_offset(uint16 eleLen, uint8 *indata);
int appmsg_mqtt_split(uint8 *pBuf, uint32 bufLen, char *pTopic, MESSAGE_MQTT_T **ppMsgOut);
void appmsg_mqtt_free(MESSAGE_MQTT_T *pMsgMQTT);
void appmsg_free(MESSAGE_MQTT_T *pMsgMQTT);
void appmsg_json_free(MESSAGE_MQTT_T *pMsgMQTT);
uint16 appmsg_axdr_package(MESSAGE_INFO_T *pMsgInfo, uint8 **ppBuf);
int appmsg_split(uint8 *pBuf, uint16 bufLen, MESSAGE_INFO_T **ppMsgOut);
uint16 appmsg_package(MESSAGE_INFO_T *pMsgInfo, uint8 **ppBuf);
int appname_split(uint8 *pName, APP_INFO_T *ptApp);
int appname_package(APP_INFO_T *ptApp, uint8 *pName, uint16 datalen);
void app_get_maketime(char *ptime, Date_Time_S *pTime);
void app_get_publish_time(char *pdate, char *ptime, Date_Time_S *pTime);

int sys_string_time_struct(char *timestr, Date_Time *pTime);
int sys_commstatus_split(char *infopath, void *pJson, int *status, uint32 *token);
int sys_keepAlive_split(char *infopath, void *pJson, uint32 *token);
int sys_getreglist_split(char *infopath, void *pJson, SYS_APPLIST_T *pAppList, uint32 *token);
int sys_getregapp_split(char *infopath, void *pJson, SYS_APPREG_T *pApp, uint32 *token);
int sys_devinfo_split(char *infopath, void *pJson, SYS_DEVINFO_T *pDevInfo, uint32 *token);
int sys_devstatus_split(char *infopath, void *pJson, SYS_DEVSTATUS_T *pDevStatus, uint32 *token);
int sys_ethcfg_split(char *infopath, void *pJson, SYS_ETHCFG_T *pETHCfg, uint32 *token);
int sys_appinfo_split(char *infopath, void *pJson, SYS_APPINFO_T *pAppInfo, uint32 *token);
int sys_container_split(char *infopath, void *pJson, SYS_CONTAINER_LIST_T *pContainer, uint32 *token);
int sys_containerApp_split(char *infopath, void *pJson, SYS_CONTAPP_LIST_T *pAppList, uint32 *token);
int sys_appid_split(char *infopath, void *pJson, SYS_APPSID_T *pAppID, uint32 *token);
int sys_applic_split(char *infopath, void *pJson, SYS_APPS_LIC_T *pAppLic, uint32 *token);
int sys_sntp_split(char *infopath, void *pJson, SYS_SNTP_T *pSntp, uint32 *token);
int sys_toreboot_split(char *infopath, void *pJson, uint32 *delaytime);
int sys_ethSchange_split(char *infopath, void *pJson, uint32 *status);
int sys_ethCchange_split(char *infopath, void *pJson, SYS_ETHCFG_T *pETHCfg);
int sys_timeevent_split(char *infopath, void *pJson, SYS_TIME_E_T *pTimeEvent);
uint32 sys_common_package(uint32 token, char *pBuf, uint32 bufLen);
uint32 sys_appreg_package(uint32 token, SYS_APPREG_T *pApp, char *pBuf, uint32 bufLen);
uint32 sys_appunreg_package(uint32 token, char *appname, char *pBuf, uint32 bufLen);
uint32 sys_keepAlive_package(uint32 token, char *pBuf, uint32 bufLen);
uint32 sys_getregapp_package(uint32 token, char *appname, char *pBuf, uint32 bufLen);
uint32 sys_ethcfg_package(uint32 token, SYS_ETHCFG_T *pEthCfg, char *pBuf, uint32 bufLen);
uint32 sys_appinfo_package(uint32 token, char *appname, char *pBuf, uint32 bufLen);
uint32 sys_containerapp_package(uint32 token, char *containername, char *pBuf, uint32 bufLen);
uint32 sys_appid_package(uint32 token, char *appname, char *pBuf, uint32 bufLen);
uint32 sys_applic_package(uint32 token, char *appNo, uint8 *appLic, uint16 liclen, char *pBuf, uint32 bufLen);
uint32 sys_settime_package(uint32 token, Date_Time *pTime, char *pBuf, uint32 bufLen);
uint32 sys_route_package(uint32 token, SYS_ROUTE_T *pRoute, char *pBuf, uint32 bufLen);
uint32 sys_sntp_package(uint32 token, SYS_SNTP_T *pSntp, char *pBuf, uint32 bufLen);
uint32 sys_reboot_package(uint32 token, uint32 delytime, char *pBuf, uint32 bufLen);
uint32 sys_install_con_package(uint32 token, SYS_INSTALL_CON_T *pConInfo, char *pBuf, uint32 bufLen);
uint32 sys_install_set_app_package(uint32 token, SYS_INSTALL_APP_T *pAppInfo, char *pBuf, uint32 bufLen);
uint32 sys_update_app_package(uint32 token, char *conname, char *appname, char *pBuf, uint32 bufLen);
uint32 sys_operate_app_package(uint32 token, char *conname, char *appname, uint8 type, char *pBuf, uint32 bufLen);

int appmsg_modinfo_split(uint8 *playload, MOD_INFO_ACK_T *pOutInfo);
int appmsg_modsinfo_split(uint8 *playload, MODS_INFO_ACK_T *pOutInfo);
int appmsg_modsinfo_split_new(uint8 *playload, MODS_INFO_NEW_T *pOutInfo);
int appmsg_wireapn_split(uint8 *playload, uint16 len, WIRE_DIAG_APN_T *pOutInfo, uint8 bNew);
uint16 appmsg_wireapn_package(WIRE_DIAG_APN_T *pDiagAPN, uint8 *pBuf, uint16 buflen);
int appmsg_wireinfo_split(uint8 *playload, WIRELESS_INFO_T *pOutInfo);
uint16 appmsg_wireinfo_package(WIRELESS_INFO_T *pInfo, uint8 *pBuf, uint16 buflen);
int appmsg_wirediag_split(uint8 *playload, WIRE_DIAG_INFO_T *pDiagInfo);
int appmsg_wiretime_split(uint8 *playload, WIRELESS_TIME_T *pDiagInfo);
uint16 appmsg_wirediag_package(WIRE_DIAG_INFO_T *pDiagInfo, uint8 *pBuf, uint16 buflen);
int appmsg_gps_split(uint8 *playload, GPS_T *pGps);
int appmsg_gpsmsg_split(uint8 *playload, GPS_MSG_T *pGps);
uint16 appmsg_gps_package(GPS_T *pGps, uint8 *pBuf, uint16 buflen, uint8 bNew);
int appmsg_rsprmt_split(uint8 *playload, RST_RMT_T *pRmt);
uint8 appmsg_rsp_currmt_split(uint8 bReport, uint8 *playload, uint8 *ST, RST_RMT_T *pRmt, uint8 stbuf);
int appmsg_rsppulse_split(uint8 *playload, RST_PULSE_STATIC_T *pPulse);
int appmsg_mac_split(uint8 *playload, OOP_ETHMAC_T *mac);
int appmsg_deviceinfo_split(uint8 *playload, OOP_DEVICERUNSTAT_T *deviceinfo);
int appmsg_deviceinfo_run_split(uint8 *playload, OOP_DEVICERUNSTAT_T *deviceinfo);
int appmsg_radioparam_split(uint8 *playload, RADIO_PARAM_T *radioparam);
int appmsg_radiostatus_split(uint8 *playload, RADIO_STATUS_T *radiostatus);


#ifdef __cplusplus
}
#endif

#endif /* __APPMSG_SPLIT_H__ */
