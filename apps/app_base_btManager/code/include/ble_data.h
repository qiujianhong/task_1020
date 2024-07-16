/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����APP ��������
* @date��    2021-7-8
* @history�� 
*********************************************************************
*/

#ifndef __BLE_DATA_H__
#define __BLE_DATA_H__

#include "ble.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIN_ENABLE            1

#define BT_APP_NAME              "btManager"
#define BT_APP_NAME_M            "M-btManager"
#define BT_MASTER_MAX            2
#define BT_SLAVE_MAX             3

/* ��������Ĭ�ϳ�ʱʱ�� */
#ifdef PRODUCT_ECU
#define BLE_RECV_TIME_OUT        50
#else
#define BLE_RECV_TIME_OUT        200
#endif

#define BLE_APPID_MAX            32

typedef struct BLEDevMsg
{
    struct BLEDevMsg *pNext;
    
    uint8             priority;   //���ȼ�
    uint8             type;       //MESSAGE_TYPE_A_XDR, MESSAGE_TYPE_JSON
    uint8             res[2];
    uint32            token;      //���(AXDR �� label �� index)
    char              source[128];//���ͷ�

    uint16            proc;       //��������(ͬIOP)
    uint16            datalen;
    uint8             data[0];
}BLE_DEV_MSG_T;

typedef struct tagBLEQueue	    /*�����������ṹ*/
{
	pthread_mutex_t f_lock;
	BLE_DEV_MSG_T  *pHead;
}BLE_QUEUE_T;


/* �����豸�������״̬ */
typedef struct tagBLEPortStatus
{
    uint8             bConfig;  //�Ƿ����ù�
    uint8             bConnect;
    uint8             channelid; //ͨ��(1-3)
    BLE_PORT_CONF_T   porttCfg; //�˿�����
    time_t            lasttime; //���һ�����ӡ�ͨ��ʱ��s(���)
    uint32            lastus;   //���һ�����ӡ�ͨ��ʱ��us(���)
}BLE_PORT_STATUS_T;

/* ��ģʽ�˿ڹ��� */
typedef struct tagBLEDevPort
{
    uint8             bChannel; //ͨ��(1-3)
    uint8             bConnect;
    uint8             MAC[6];   //MAC��ַ
    uint32            port;     /* �˿ں� */
    BLE_MODE_E        mode;     /* ģʽ BLE_MODE_E */
    BLE_MATCH_T       match;    /* ��Բ��� */
    time_t            lasttime; //���һ�����ӡ�ͨ��ʱ��s(���)
    uint32            lastus;   //���һ�����ӡ�ͨ��ʱ��us(���)

    uint16            timeout;                //��ʱʱ��ms(͸��)
    uint16            cachelen;               //�����С
    uint8             cache[BLE_MSG_MAX_LEN]; //���� 4K

    BLE_DEV_MSG_T    *pTranMsg;               //ת����Ϣ
}BLE_DEV_PORT_T;

/* �����豸������� */
typedef struct tagBLEDev
{
    uint8             bSleep;     //ģ��״̬(�Ƿ���˯��)
    uint8             bConfig;    //�Ƿ����ù�����
    uint8             bMatch;     //�Ƿ����ù���ģʽ���ò���
    uint8             bCheckMeter;//�������(��Чʱ�����ӶϿ�����Ӧ)
    BLE_CONFIG_T      config;     //��������(��������)
    BLE_MATCH_T       match;      //��ģʽ��Բ���(��������)
    BLE_SN_T          sn;         //����sn

    #if PIN_ENABLE == 1
    BLE_BROADCAST_T   broadcast;  //�㲥����
    #endif
    
    uint16            portnum;    //�˿�����(��ģʽ)
    BLE_PORT_STATUS_T PortCfg[BLE_PORT_MAX];  //�˿�����(��������)
    
    BLE_DEV_PORT_T    mPortStatus[BT_MASTER_MAX];//�˿�״̬(��ģʽ)
    BLE_DEV_PORT_T    sPortStatus[BT_SLAVE_MAX]; //�˿�״̬(��ģʽ)
}BLE_DEV_T;


/* �����ܻ���62K */
#define BLE_RECV_BUF_LEN          63488
#define BLE_DEV_STATUS_INIT       0
#define BLE_DEV_STATUS_PREPARE    1
#define BLE_DEV_STATUS_GETCONN    2
#define BLE_DEV_STATUS_WORK       3

/* �����豸�߳����� */
typedef struct tagBLEDevHandle
{
    uint8             status;                //״̬��
    uint8             res[1];
    uint16            recvlen;               //���վ��
    uint8             recv[BLE_RECV_BUF_LEN];//���ջ���

    int               devfd;                 //�豸���
    int               chkfdcnt;              //�������ʱ
    int               cfgDelay;              //�����ӳ�
    
    BLE_QUEUE_T       cfgQueue;              //���ö���
    BLE_QUEUE_T       transQueue;            //ת������(������Ϣ����)
    BLE_QUEUE_T       msgQueue;              //��Ϣ����
}BLE_DEV_H_T;


/* ����ȫ�ֱ��� */
typedef struct tagBLEDATA
{
    uint32          mqttip;       //mqtt��ַ
    uint16          mqttport;     //mqtt�˿�
    uint8           appidlen;
    uint8           appid[BLE_APPID_MAX];
    uint8           bReg;         //ע��ɹ�
    uint8           bWork;        //��ʼ����(�����豸)

    BLE_DEV_T       tDevStatus;   //����״̬
    BLE_DEV_H_T     tDevPorc;     //�豸����
}BLE_DATA_T;



void ble_clean_queue(BLE_QUEUE_T *pQueue);
BLE_DEV_MSG_T *ble_read_queue(BLE_QUEUE_T *pQueue);
void ble_write_queue(BLE_QUEUE_T *pQueue, BLE_DEV_MSG_T *pMsgProc, uint8 bOrder);

int ble_write_config(BLE_CONFIG_T *pConfig);
int ble_write_match(BLE_MATCH_T *pMatch);
int ble_write_portcfg(uint32 port, BLE_PORT_CONF_T *pPortcfg);
int ble_stoi(const char *str);

#ifdef __cplusplus
}
#endif

#endif //__BLE_DATA_H__

