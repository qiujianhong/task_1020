/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ�� ��Ҫ���ݸ�ʽ
* @date��    2019-10-31
* @history�� 
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
#define COMM_TRHEAD_FLAG_RESTART  60  /* �ڲ�������� */

#define SMS_BUF_SIZE              1024  /*����ͨ��������*/
#define SERIAL_BUF_SIZE	          2048  /*���ڻ�������С*/
#define BUF_LENGTH                5120	/*��������С�����û����������̻߳�������С*/
#define QUEUE_SIZE                20

#define CHANNEL_STATE_INIT    0     /*��ǰͨ������״̬ - ��ʼ��*/
#define CHANNEL_STATE_RECV    1     /*��ǰͨ������״̬ - ���ݽ���*/
#define CHANNEL_STATE_SEND    2     /*��ǰͨ������״̬ - ���ݷ���*/
#define CHANNEL_STATE_CLOSE   3     /*��ǰͨ������״̬ - �ر�*/

#define DEF_UDP_PORT          5000    /* UDPָ���˿ں� */
#define DEF_UDP_PORT_MAX      5500    /* UDPָ�����˿ں� */


#define PATH_PROCNET_DEV     "/proc/net/dev"   /*�����豸·��*/


typedef struct tagCommQueue	/*ͨѶ���ݶ���*/
{
	uint32 len;							/*���ݳ���*/
	uint8 buf_queue[BUF_LENGTH + 20];	/*���д洢������*/
}COMM_QUEUE_T;

typedef struct tagCommQueueList	/*�����������ṹ*/
{
	pthread_mutex_t f_lock;	    /*���ջ�����������*/
	int16  head;                /*����ͷ*/
	int16  tail;                /*����β*/
	COMM_QUEUE_T queue[QUEUE_SIZE];	/*ͨѶ���ݶ���*/
}COMM_QUEUE_LIST_T;

typedef struct tagCommTime
{
    uint8 year;  /*���0����1900��*/
    uint8 month; /*�·�0����1��*/
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;

    uint16 milliseconds;
}COMM_STRUCT_T;    /*ʱ��ṹ*/

/* �߳��ж� */
typedef BOOLEAN (*CheckStart)(void *);
typedef void *(*EntryFun)(void *);
typedef struct tagCommsThread
{
    pthread_t          nThreadId;   /* �߳�ID */
    CheckStart         pCheckFunc;  /* �߳������ж�����, Ϊ�ձ�ʾĬ������ */
    EntryFun           pEntryFunc;  /* �߳���ں��� */
    void              *pArg;        /* ������� */
    uint8             *pRunFlag;    /* �߳����б��, �߳�ִ������Ϊ0, ����Ϊ�� */
    uint8             *pStartFlag;  /* �ⲿ����������ǩ, ����Ϊ�� */
    BOOL              *pStartup;    /* �߳��Ƿ����� */
}COMM_TRHEAD_T;

/*�������������ṹ*/
typedef struct tagIfStats{
  uint64 rx_bytes;        /*���������ֽ���*/
  uint32 rx_packets;      /*�������ݰ���*/
  uint32 rx_errors;       
  uint32 rx_dropped;
  uint32 rx_fifo_errors;
  uint32 rx_frame_errors;
  uint32 rx_compressed;
  uint32 rx_multicast;
  
  uint64 tx_bytes;        /*���������ֽ���*/
  uint32 tx_packets;      /*�������ݰ���*/
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

/* ͨ���򿪹ر�ȷ��֪ͨ ͨ��id, �ڲ�Э��, ��� */
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
