/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 主要数据格式
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#ifndef	__COMM_H
#define __COMM_H

#include "appmsg_split.h"
#include "commuprot.h"
#include "framecomm.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_DCM_APP_FALG           1

#define COMM_TRHEAD_FLAG_MUX      30
#define COMM_TRHEAD_FLAG_RESTART  60  /* 内部重启标记 */

#define SMS_BUF_SIZE              1024  /*短信通道缓冲区*/
#define SERIAL_BUF_SIZE	          2048  /*串口缓冲区大小*/
#define BUF_LENGTH                5120	/*缓冲区大小，共用缓冲区及各线程缓冲区大小*/
#define QUEUE_SIZE                20

#define CHANNEL_STATE_INIT    0     /*当前通道运行状态 - 初始化*/
#define CHANNEL_STATE_RECV    1     /*当前通道运行状态 - 数据接收*/
#define CHANNEL_STATE_SEND    2     /*当前通道运行状态 - 数据发送*/
#define CHANNEL_STATE_CLOSE   3     /*当前通道运行状态 - 关闭*/

#define DEF_UDP_PORT          5000    /* UDP指定端口号 */
#define DEF_UDP_PORT_MAX      5500    /* UDP指定最大端口号 */


#define PATH_PROCNET_DEV     "/proc/net/dev"   /*网络设备路径*/


typedef struct tagCommQueue	/*通讯数据队列*/
{
	uint32 len;							/*数据长度*/
	uint8 buf_queue[BUF_LENGTH + 20];	/*队列存储缓冲区*/
}COMM_QUEUE_T;

typedef struct tagCommQueueList	/*锁共享缓冲区结构*/
{
	pthread_mutex_t f_lock;	    /*接收缓冲区互斥锁*/
	int16  head;                /*队列头*/
	int16  tail;                /*队列尾*/
	COMM_QUEUE_T queue[QUEUE_SIZE];	/*通讯数据队列*/
}COMM_QUEUE_LIST_T;

typedef struct tagCommTime
{
    uint8 year;  /*年份0代表1900年*/
    uint8 month; /*月份0代表1月*/
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;

    uint16 milliseconds;
}COMM_STRUCT_T;    /*时间结构*/

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
    BOOL              *pStartup;    /* 线程是否启动 */
}COMM_TRHEAD_T;

/*网卡数据流量结构*/
typedef struct tagIfStats{
  uint64 rx_bytes;        /*接收数据字节数*/
  uint32 rx_packets;      /*接收数据包数*/
  uint32 rx_errors;       
  uint32 rx_dropped;
  uint32 rx_fifo_errors;
  uint32 rx_frame_errors;
  uint32 rx_compressed;
  uint32 rx_multicast;
  
  uint64 tx_bytes;        /*发送数据字节数*/
  uint32 tx_packets;      /*发送数据包数*/
  uint32 tx_errors;
  uint32 tx_dropped;
  uint32 tx_fifo_errors;
  uint32 collisions;
  uint32 tx_carrier_errors;
  uint32 tx_compressed;
}IF_STATE_T;

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

#define COMM_PRINT_IPSTR(ips) ips[0], ips[1], ips[2], ips[3]
#define COMM_PRINT_IPADDR(ipa) (ipa >> 24) & 0xff, (ipa >> 16) & 0xff, (ipa >> 8) & 0xff, ipa & 0xff

/* 通道打开关闭确认通知 通道id, 内部协议, 入参 */
typedef int (*ChannelEvent)(uint8, uint8, void*);

void comm_clean_queue(COMM_QUEUE_LIST_T *pQueue);
uint32 comm_read_queue(COMM_QUEUE_LIST_T *pQueue, uint8 *pBuf);
int comm_write_queue(COMM_QUEUE_LIST_T *pQueue, uint8 *pBuf, uint32 bufLen);
BOOLEAN comm_ifnet_exist(const char *pNetName);
void comm_add_route_net(const char *pNetName, uint8 *pRouteAddr);
void comm_del_route_net(const char *pNetName, uint8 *pRouteAddr);
void comm_del_iproute_net(char *pNetName, uint32 ipaddr);
int comm_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen);
void comm_send_get_pppmode(uint8 id);
void comm_get_curtime_s(COMM_STRUCT_T *pTime);
void comm_add_channel(uint8 channel, uint8 *buf, uint16 len);
uint8 comm_get_bigchannel(uint8 *buf);
uint8 comm_get_protocol(uint8 *buf);
int comm_get_ifflux(char *ifName, uint64 *rxbytes, uint64 *txbytes);
int comm_listen_socket(uint16 port);
int comm_accept_socket(int listenfd, uint32 *pIP, uint16 *pPort);
int comm_init_udp(char *ifName);
int comm_connect_socket(char *ifName, uint32 IP, uint16 Port);
int comm_connect_socket_noblock(char *ifName, uint32 IP, uint16 Port, int *tempfd, uint32 *startTime);
void comm_set_ifnet(char *ifName, uint8 *pIp, uint8 *pMask, uint8 *pGetWay, uint8 *pMac);
int comm_check_link(char *ifName);
void comm_flow_init();
void comm_flow_add(uint8 bPPP, uint8 index, uint32 Sendflow, uint32 Recvflow);
void comm_flow_get(uint8 bPPP, uint8 index, uint32 *pSendflow, uint32 *pRecvflow); 
int comm_send_msg(uint32 msgid, uint8 *msgdata, uint16 datalen);
int comm_send_label_msg(uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen);
int comm_channel_event_add(uint8 channeltype, ChannelEvent func, void *pArg, void *ptHandle);
int comm_send_respmsg(uint32 msgid, uint16 label, uint8 *msgdata, uint16 datalen);
void set_startup_flag_ble(BOOL bRun);
void set_startup_flag_RS485(BOOL bRun);
void set_pin_flag_ble(BOOL bEn);
BOOL get_pin_flag_ble(void);
#ifdef __cplusplus
}
#endif

#endif
