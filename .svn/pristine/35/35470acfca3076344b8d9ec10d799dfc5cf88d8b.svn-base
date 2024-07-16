/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 主要数据格式
* @date：    2020-3-6
* @history： 
*********************************************************************
*/
#ifndef	__COMMON_BASE_H
#define __COMMON_BASE_H

#include "oop_pw_area.h"

#include "appmsg_split.h"
#include "comm_port.h"
#include "framecomm.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_TRHEAD_FLAG_MUX      30
#define COMM_TRHEAD_FLAG_RESTART  60  /* 内部重启标记 延时10启动 */

#define BUF_LENGTH                ADVPD_MSG_BUF_LENGTH	/*缓冲区大小*/
#define QUEUE_SIZE                10

typedef struct tagCommQueue	/*通讯数据队列*/
{
	uint32 len;							/*数据长度*/
	uint8 buf_queue[BUF_LENGTH];	/*队列存储缓冲区*/
}COMM_QUEUE_T;

typedef struct tagCommQueueList	/*锁共享缓冲区结构*/
{
	pthread_mutex_t f_lock;	    /*接收缓冲区互斥锁*/
	int16  head;                /*队列头*/
	int16  tail;                /*队列尾*/
	COMM_QUEUE_T queue[QUEUE_SIZE];	/*通讯数据队列*/
}COMM_QUEUE_LIST_T;


/* 线程判断 */
typedef BOOLEAN (*CheckStart)(void *);
typedef void *(*EntryFun)(void *);
typedef struct tagCommsThread
{
    pthread_t          nThreadId;   /* 线程ID */
    CheckStart         pCheckFunc;  /* 线程启动判断依据, 为空表示默认启动 */
    EntryFun           pEntryFunc;  /* 线程入口函数 */
    void              *pArg;        /* 函数入参 */
    uint8             *pRunFlag;    /* 线程运行标记, 线程执行设置为0, 不可为空 */
    uint8             *pStartFlag;  /* 外部设置重启标签, 可以为空 */
}COMM_TRHEAD_T;

typedef enum
{
  TCP_ESTABLISHED = 1,
  TCP_SYN_SENT,
  TCP_SYN_RECV,
  TCP_FIN_WAIT1,
  TCP_FIN_WAIT2,
  TCP_TIME_WAIT,
  TCP_CLOSE,
  TCP_CLOSE_WAIT,
  TCP_LAST_ACK,
  TCP_LISTEN,
  TCP_CLOSING   /* now a valid state */
}TCP_STATUS_E;

uint32 comm_read_queue(COMM_QUEUE_LIST_T *pQueue, uint8 *pBuf);
int comm_write_queue(COMM_QUEUE_LIST_T *pQueue, uint8 *pBuf, uint32 bufLen);
void comm_add_route_net(const char *pNetName, uint32 IP);
void comm_del_route_net(const char *pNetName, uint32 IP);
void comm_add_channel(uint8 channel, uint8 *buf, uint16 len);
uint8 comm_get_bigchannel(uint8 *buf);
uint8 comm_get_protocol(uint8 *buf);
int comm_connect_socket(uint32 IP, uint16 Port);
int comm_open_listen(uint16 port, uint8 num);
int comm_init_udp(char *ifName);
int comm_send_label_msg(uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen);


#ifdef __cplusplus
}
#endif

#endif


