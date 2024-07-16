/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ����ⲿͨ���߳�
* @date��    2019-12-3
* @history�� 
*********************************************************************
*/
#ifndef __APPOUTMSG_H
#define __APPOUTMSG_H

#include "OopAll.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct FrameHeadInfo_t_
{
    uint8 pipe;//����ͨ����
	LinkLayerCtrl_t ctrl;//������
	OOP_TSA_T tsa;
	OOP_OAD_U oad;
}FrameHeadInfo_t;

typedef struct 
{
	uint8 PIID;
	uint8 ServerID;
	uint8 nChioce;
	SecurityMode_t SecMod;
	FrameHeadInfo_t FrameHeadInfo;		//֡ͷ��Ϣ
//	PROXY_STATE state;					//��͸������״̬
//	time_t RelayStartTime;					//�м̿�ʼʱ��
//	time_t SendTime;						//���͵���ʼʱ��
//	uint16 TimeOut;							//����ʱʱ��
	uint16 RequestApduLen;					//���������APDU��
	uint8  pRequestApdu[1400];					//������������


}Proxyresquest;

typedef struct 
{
	uint8 PIID;
	SecurityMode_t SecMod;
	FrameHeadInfo_t FrameHeadInfo;		//֡ͷ��Ϣ
//	PROXY_STATE state;					//��͸������״̬
//	time_t RelayStartTime;					//�м̿�ʼʱ��
//	time_t SendTime;						//���͵���ʼʱ��
//	uint16 TimeOut;							//����ʱʱ��
	uint16 ResponseApduLen;					//���������APDU��
	uint8  ResponseApdu[2100];					//������������
	uint8 ServerID;

}ProxyResponse;


typedef struct
{
    uint16  Queue_Len;                          //����β
    uint8  Queue_Data[2100];               //ͨѶ���ݶ���
}Proxy_Queue; 

typedef struct
{
	pthread_mutex_t QueueLock;                  //���ջ�����������
    int16  QueueHead;                          //����ͷ
    int16  QueueTail;                          //����β
    Proxy_Queue Queue[10];               //ͨѶ���ݶ���
}ProxyLockQueue; 


typedef struct tagAppOutMsg
{
    int            *pClientId;
    uint8          *pRunFlag;
}OUTMSG_THREAD_T;

/* ����: clientid, label, data, datalen */
typedef int (*AppMsgFun)(int, uint16, uint8*, uint16);
typedef struct tagAppMsgFun
{
    uint32    msgid;
    AppMsgFun func;
}APP_MSG_FUN_T;

void *app_msg_entry(void *pArg);
int guiapp_send_msg(uint32 msgid, uint16 label, uint8 *pMsg, uint16 msglen);
int gui_send_syncmsg(uint32 msgid, uint8 *sendbuf, uint16 sendlen, uint8 *recvbuf, uint16 buflen, uint16 *recvLen);
//int Proc_Proxy(uint16* label,ProxyHeadInfo_t *ProxyHead );

BOOL GenProxyMSG(Proxyresquest *pProxyTask,uint32 *msgid,uint8 *outdata,uint16 *outlen);
uint8 SetComDCB(OOP_COMDCB_T comm);
int32 ProxyWriteQueue(ProxyLockQueue *ProxyQueue, uint8 *buf, int32 buf_len);
int32 ProxyReadQueue(ProxyLockQueue *ProxyQueue, uint8 *buf);



#ifdef __cplusplus
}
#endif

#endif

