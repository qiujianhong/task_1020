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



int gui_outmsg_init(uint8 type, uint32 ip, uint16 port);
DB_AGENT gui_get_dbagent();
void gui_outmsg_uninit();
uint16 gui_get_next_index();
int gui_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
int gui_recv_msg(uint8 *pBuf, uint16 buflen);
int gui_login_to_smios();
int gui_send_smios(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_send_acmeter(uint16 label, uint16 iip, uint16 iop, uint8* playload, uint16 length);

int gui_heart_bear(MESSAGE_INFO_T *pInMsg);
int gui_get_version(MESSAGE_INFO_T *pInMsg);
int gui_get_appid(MESSAGE_INFO_T *pInMsg);

int gui_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex);
int gui_send_ctsmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_send_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_send_extend_wirelessmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_send_advpdamrmsg(uint16 label, uint16 iop, uint8* playload, uint16 length);
int gui_seek_app();
BOOL gui_recv_regapp(MESSAGE_INFO_T *pInMsg);
BOOL gui_start_goon();

#ifdef __cplusplus
}
#endif

#endif //__MQTT_MSG_H__

