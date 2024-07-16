/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程外部通信线程
* @date：    2019-12-3
* @history： 
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
    uint8 pipe;//报文通道号
	LinkLayerCtrl_t ctrl;//控制码
	OOP_TSA_T tsa;
	OOP_OAD_U oad;
}FrameHeadInfo_t;

typedef struct 
{
	uint8 PIID;
	uint8 ServerID;
	uint8 nChioce;
	SecurityMode_t SecMod;
	FrameHeadInfo_t FrameHeadInfo;		//帧头信息
//	PROXY_STATE state;					//穿透抄任务状态
//	time_t RelayStartTime;					//中继开始时间
//	time_t SendTime;						//发送的起始时间
//	uint16 TimeOut;							//任务超时时间
	uint16 RequestApduLen;					//代理请求的APDU长
	uint8  pRequestApdu[1400];					//任务请求数据


}Proxyresquest;

typedef struct 
{
	uint8 PIID;
	SecurityMode_t SecMod;
	FrameHeadInfo_t FrameHeadInfo;		//帧头信息
//	PROXY_STATE state;					//穿透抄任务状态
//	time_t RelayStartTime;					//中继开始时间
//	time_t SendTime;						//发送的起始时间
//	uint16 TimeOut;							//任务超时时间
	uint16 ResponseApduLen;					//代理请求的APDU长
	uint8  ResponseApdu[2100];					//任务请求数据
	uint8 ServerID;

}ProxyResponse;


typedef struct
{
    uint16  Queue_Len;                          //队列尾
    uint8  Queue_Data[2100];               //通讯数据队列
}Proxy_Queue; 

typedef struct
{
	pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                          //队列头
    int16  QueueTail;                          //队列尾
    Proxy_Queue Queue[10];               //通讯数据队列
}ProxyLockQueue; 


typedef struct tagAppOutMsg
{
    int            *pClientId;
    uint8          *pRunFlag;
}OUTMSG_THREAD_T;

/* 参数: clientid, label, data, datalen */
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

