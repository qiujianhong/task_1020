/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程IPC入口出口操作
* @date：    2020-3-9
* @history： 
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

typedef struct tagCommMSGBUF	/*通讯数据队列*/
{
	uint8  id;							    /*通道号*/
    uint8  protocol;                        /*内部协议*/
    uint16 msglen;                          /*消息长度*/
	uint8  msgbuf[ADVPD_104MSG_BUF_LENGTH];	/*队列存储缓冲区*/
}COMM_MSGBUF_T;

typedef struct tagCommMSGList
{
	pthread_mutex_t f_lock;	    /*接收缓冲区互斥锁*/
	int16  head;                /*队列头*/
	int16  tail;                /*队列尾*/
	COMM_MSGBUF_T queue[COMM_MSG_LIST_SIZE];	/*通讯数据队列*/
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

