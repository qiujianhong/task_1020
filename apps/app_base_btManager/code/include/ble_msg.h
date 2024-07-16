/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����APP ��Ϣ����
* @date��    2021-7-8
* @history�� 
*********************************************************************
*/

#ifndef __BLE_MSG_H__
#define __BLE_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MQTT_MSG_LEN      8192	/*��������С�����û����������̻߳�������С*/

typedef struct tagBLEMsgHandle
{
    time_t   timeflag; //��ʱ��ѯ���
    char     topic[256];
    uint8    recvBuf[BLE_MQTT_MSG_LEN];
}BLE_MSGHANDLE_T;

/* ��ں������� */
typedef int (*MsgProcFun)(MESSAGE_INFO_T *, BLE_MSGHANDLE_T *, BLE_DATA_T *);
typedef struct tagBLEMsgFun
{
    uint16             IID;      /* �߳�ID */
    uint16             IOP;      /* �߳���ں��� */
    MsgProcFun         pProcMsg; /* ������� */
}BLE_MSGFUN_T;

//�춨��Ϣ�Ƿ��Ȼ���
#define CHECK_METER_ASYNC 1


#ifdef __cplusplus
}
#endif

#endif //__BLE_MSG_H__

