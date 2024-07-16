/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��̨APP ��Ϣ����
* @date��    2021-7-8
* @history�� 
*********************************************************************
*/

#ifndef __RADIO_MSG_H__
#define __RADIO_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif
extern uint32 g_RadioSpeakerFlag;

#define RADIO_MQTT_MSG_LEN      8192	/*��������С�����û����������̻߳�������С*/

typedef struct tagRADIOMsgHandle
{
    time_t   timeflag; //��ʱ��ѯ���
    char     topic[256];
    uint8    recvBuf[RADIO_MQTT_MSG_LEN];
}RADIO_MSGHANDLE_T;

/* ��ں������� */
typedef int (*MsgProcFun)(MESSAGE_INFO_T *, RADIO_MSGHANDLE_T *, RADIO_DATA_T *);
typedef struct tagRADIOMsgFun
{
    uint16             IID;      /* �߳�ID */
    uint16             IOP;      /* �߳���ں��� */
    MsgProcFun         pProcMsg; /* ������� */
}RADIO_MSGFUN_T;

typedef struct tagLongSend
{
    uint8  sendbuf;     //���͵����� Ĭ��0x55
    uint16 sendtime;    //�������͵�ʱ��
}RADIO_LONG_SEND_T;

//�춨��Ϣ�Ƿ��Ȼ���
#define CHECK_METER_ASYNC 1

#ifdef __cplusplus
}
#endif

#endif //__RADIO_MSG_H__

