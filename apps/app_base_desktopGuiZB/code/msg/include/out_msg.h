/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级app用采对外消息处理
* @date：    2020-1-6
* @history： 
*********************************************************************
*/

#ifndef __OUT_MSG_H__
#define __OUT_MSG_H__

#include "appmsg_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    GUI_STATUS_INIT = 0,  //初始化 APP注册
    GUI_STATUS_SEEK,      //查询APP信息
    GUI_STATUS_WORK,      //正常工作
}APP_GUI_STATUS_E;

typedef struct _APP_VERSION_INFO
{
    int                   loginAppCnt;      //注册app实际数量
    MQT_PLUGIN_T          pluginInfo[36];	//注册信息
}APP_VERSION_INFO_T;     //App版本信息

extern WIRELESS_INFO_T      ModuleInfoDisp;


int gui_outmsg_init(uint8 type, uint32 ip, uint16 port);
DB_AGENT gui_get_dbagent();
void gui_outmsg_uninit();
uint16 gui_get_next_index();
int gui_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
int gui_recv_msg(uint8 *pBuf, uint16 buflen);
int gui_login_to_smios();
int gui_send_smios(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_send_acmeter(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length);
int gui_send_radio(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_heart_bear(MESSAGE_INFO_T *pInMsg);
int gui_get_version(MESSAGE_INFO_T *pInMsg);
int gui_get_appid(MESSAGE_INFO_T *pInMsg);
int gui_get_Connecttime(MESSAGE_INFO_T *pInMsg);
int gui_get_moduleinfo(MESSAGE_INFO_T *pInMsg);
int gui_get_diaginfo(MESSAGE_INFO_T *pInMsg);
uint16 gui_get_ppp_gps(uint8 no, uint8* recvbuf, uint16 buflen);
#if defined (PRODUCT_ZCU_1)
BOOL get_radio_cfg_msg(MESSAGE_INFO_T *pInMsg);
int deal_at_ver(MESSAGE_INFO_T *pInMsg);
#endif

int gui_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);
int gui_send_ctsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length,uint16 *msgIndex);
int gui_send_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_send_extend_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_send_advpdamrmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_seek_app();
BOOL gui_recv_regapp(MESSAGE_INFO_T *pInMsg);
BOOL gui_start_goon();
int look_up_at_simsel();

#if defined(AREA_FUJIAN) && defined (PRODUCT_ZCU_3)
#define MAX_RESULT_LEN 4096
typedef struct tagScanData        /*运营商网络信号扫描数据*/
{
    OOP_DATETIME_S_T scanTime;     /*数据扫描时间*/
    char scanData[4096];                    /*数据扫描内容*/
}WIRELESS_SCAN_DATA_SEND_T;

int  gui_485_workmode_get();
int gui_485_workmode_get_msg(MESSAGE_INFO_T *pInMsg);

int  gui_485_workmode_set(uint8 workmode);
int gui_485_workmode_set_msg(MESSAGE_INFO_T *pInMsg);
BOOL gui_recv_scanData(uint8 *rcvData);
#endif

#ifdef __cplusplus
}
#endif

#endif //__MQTT_MSG_H__

