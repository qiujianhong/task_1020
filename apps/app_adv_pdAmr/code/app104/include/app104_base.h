/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ���IPC��ڳ��ڲ���
* @date��    2020-3-9
* @history�� 
*********************************************************************
*/
#ifndef	__APP104_BASE_H
#define __APP104_BASE_H

#include "appmsg_struct.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_MSG_LIST_SIZE           100
#define ADVPD_104MSG_BUF_LENGTH      4096

typedef struct tagCommMSGBUF	/*ͨѶ���ݶ���*/
{
	uint8  id;							    /*ͨ����*/
    uint8  protocol;                        /*�ڲ�Э��*/
    uint16 msglen;                          /*��Ϣ����*/
	uint8  msgbuf[ADVPD_104MSG_BUF_LENGTH];	/*���д洢������*/
}COMM_MSGBUF_T;

typedef struct tagCommMSGList
{
	pthread_mutex_t f_lock;	    /*���ջ�����������*/
	int16  head;                /*����ͷ*/
	int16  tail;                /*����β*/
	COMM_MSGBUF_T queue[COMM_MSG_LIST_SIZE];	/*ͨѶ���ݶ���*/
}COMM_MSG_LIST_T;

void advPd_comm_msginit();
void advPd_rmt_init();
uint8 advPd_get_rmt(RST_RMT_T *pRmt);
uint16 advPd_get_commmsg(uint8 *pId, uint8 *pProtocol, uint8 *pMsg);
int advPd_add_commmsg(uint8 id, uint8 protocol, uint8 *pMsg, uint32 msgLen);

int appPd_reboot_to_smiOS(void);
int appPd_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen);

#ifdef __cplusplus
}
#endif

#endif

