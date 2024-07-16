/*
*********************************************************************
* Copyright(C) 2022 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2022-3-20
*********************************************************************
*/
#ifndef __RSP_MSG_H__
#define __RSP_MSG_H__


#ifdef __cplusplus
extern "C" {
#endif


#define RSP_MQTT_MSG_LEN      5120

typedef struct tagRspMsgHandle
{
    uint8    bReg;
    uint8    res[3];
    time_t   timeflag; //��ʱ��ѯ���
    char     topic[256];
    uint8    recvBuf[RSP_MQTT_MSG_LEN];
}RSP_MSGHANDLE_T;

/* ��ں������� */
typedef int (*MsgProcFun)(MESSAGE_INFO_T *, void *, void *);
typedef struct tagRspMsgFun
{
    uint16             IID;      /* �߳�ID */
    uint16             IOP;      /* �߳���ں��� */
    uint32             rpm;      /* rpm */
    MsgProcFun         pProcMsg; /* ������� */
}RSP_MSGFUN_T;


#ifdef __cplusplus
}
#endif

#endif // __RSP_MQTT_H__

