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

#ifdef __cplusplus
extern "C" {
#endif


void gui_next_index_init();
uint16 gui_get_next_index();
int gui_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);

#ifdef __cplusplus
}
#endif

#endif //__MQTT_MSG_H__

