/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   �߼�app�òɶ�����Ϣ����
* @date��    2020-1-6
* @history�� 
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
    GUI_STATUS_INIT = 0,  //��ʼ�� APPע��
    GUI_STATUS_SEEK,      //��ѯAPP��Ϣ
    GUI_STATUS_WORK,      //��������
}APP_GUI_STATUS_E;

typedef struct _APP_VERSION_INFO
{
    int                   loginAppCnt;      //ע��appʵ������
    MQT_PLUGIN_T          pluginInfo[36];	//ע����Ϣ
}APP_VERSION_INFO_T;     //App�汾��Ϣ

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
typedef struct tagScanData        /*��Ӫ�������ź�ɨ������*/
{
    OOP_DATETIME_S_T scanTime;     /*����ɨ��ʱ��*/
    char scanData[4096];                    /*����ɨ������*/
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

