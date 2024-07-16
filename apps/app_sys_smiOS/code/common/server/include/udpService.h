/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� udpService.h
           udpͨѶͷ�ļ�
* @date�� 2019-11-9
*********************************************************************
*/

#ifndef __UDPSERVICE_H__
#define __UDPSERVICE_H__

#include "event2/event-config.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/util.h"
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>

#include "baseType.h"
#include "msgServiceBase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* @brief:udpͨѶ����ṹ���װ  */
typedef struct _udp_server_obj_
{
    uint32             serverSock;    //������UDP����SockerFd
    uint32             stop;          // UDP�����ֹͣ�����־λ
    struct event_base* base;          // event base���
    sint8              serverIp[16];  // UDP����󶨵�IP��ַ
    sint16             serverPort;    // UDP�����ϵķ���˿�
    pthread_t          tid;           // UDP�����̵߳�ThreadIP
} UDP_SERVICE_T;

/* @brief:udpͨѶ�ͻ��˽ṹ���װ  */
typedef struct _udp_client_ctx
{
    sint32             serverSock;  //������UDP����SockerFd
    struct sockaddr_in client_sin;  //��Ҫ�ظ��Ŀͻ��˵�ַ
    MSG_OBJ_T          reqMsg;      //������Ϣͷ
    RAW_OBJ_T          reqRawObj;   //������Ϣ����
    MSG_OBJ_T          rspMsg;      //�ظ���Ϣͷ
    RAW_OBJ_T          rspRawObj;   //�ظ���Ϣ����
} UDP_CLIENT_CTX_T;

/* @brief:udpͨѶ����ϵͳ��ʼ��  */
sint32 udp_init_system(void);

/* @brief:udpͨѶ��������ϵͳ  */
void udp_clean_up_system(void);

/* @brief:udpͨѶ��������ϵͳ  */
sint32 udp_start_system(void);

/* @brief:udpͨѶ����ֹͣϵͳ  */
void udp_stop_system(void);

void set_udp_bind_ip(sint8 ip[IP_LEN]);

void set_udp_bind_port(uint16 port);

/* @brief:udp �ͻ������ݷ��ͽӿ� */
sint32 udp_sendto(sint8 ip[IP_LEN], uint16 port, void* data, uint32 dataLen);

#ifdef __cplusplus
}
#endif
#endif
