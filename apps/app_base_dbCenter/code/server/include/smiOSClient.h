#ifndef __SMIOS_CLIENT_H__
#define __SMIOS_CLIENT_H__

#include "baseConstant.h"
#include "event2/event-config.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/util.h"
#include "msgHelper.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _smios_connect
{
    MSG_TYPE_E type;                //��������
    sint8      smiosIP[IP_LEN];     //smios IP��ַ
    uint16     smiosPort;           //smios�˿ں�
    struct bufferevent *bev ;
    pthread_t tid;                  // smiOS�����̵߳�ThreadIP
    struct event notify_event;      // event �첽֪ͨ�¼�
    struct event recv_event;        // event �첽֪ͨ�¼�
    sint32     notifyReceiveFd;     //�첽��������ļ�������
    sint32     notifySendFd;        //�첽������������
} SMIOS_CONNECT_T;

//��ʼ��smiOS�ͻ����߳�
sint32 init_smiOS_connect(MSG_TYPE_E type, sint8 *ip, uint16 port);
//����������smiOS����
sint32 sendto_smiOS(sint8 *data, uint32 dataLen);

#ifdef __cplusplus
}
#endif

#endif