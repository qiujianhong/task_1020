/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 IPC通信头文件
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#ifndef	__IPC_COMM_H
#define __IPC_COMM_H

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_SOCKET           4000  /*通讯应用环回socket端口号*/
#define COMM_SERV             2     /*通讯应用服务器标志*/


#define INIT_SOCK             0     /*环回socket运行状态 - 初始化socket连接*/
#define RECV_SOCK             1     /*环回socket运行状态 - socket数据接收*/
#define SEND_SOCK             2     /*环回socket运行状态 - socket数据发送*/
#define CLOSE_SOCK            3     /*环回socket运行状态 - 关闭socket连接*/

typedef struct tagCommIPCEvent
{
    uint8         type;
    uint8         res[3];
    ChannelEvent  func;
    void         *pArg;
}COMM_IPC_EVENT;
#define COMM_IPC_EVENT_NAX   5

/* IPC线程句柄 */
typedef struct tagCommIPCHandle
{
    /* 对于IPC 发送队列就是 其他线程的接收队列 */
    COMM_QUEUE_LIST_T *ptGPRS0Recv;
    COMM_QUEUE_LIST_T *ptGPRS0RecvBak;
    COMM_QUEUE_LIST_T *ptGPRS0Send;
    
    COMM_QUEUE_LIST_T *ptGPRS1Recv;
    COMM_QUEUE_LIST_T *ptGPRS1RecvBak;
    COMM_QUEUE_LIST_T *ptGPRS1Send;

    COMM_QUEUE_LIST_T *ptETH0Recv;
    COMM_QUEUE_LIST_T *ptETH0RecvBak;
    COMM_QUEUE_LIST_T *ptETH0Send;
    
    COMM_QUEUE_LIST_T *ptETH1Recv;
    COMM_QUEUE_LIST_T *ptETH1RecvBak;
    COMM_QUEUE_LIST_T *ptETH1Send;

    COMM_QUEUE_LIST_T *ptListenRecv;
    COMM_QUEUE_LIST_T *ptListenSend;

    COMM_QUEUE_LIST_T *ptBlueRecv;
    COMM_QUEUE_LIST_T *ptBlueSend;
    
    COMM_QUEUE_LIST_T *ptRadioRecv;
    COMM_QUEUE_LIST_T *ptRadioSend;

    COMM_QUEUE_LIST_T *ptProxyRecv;
    COMM_QUEUE_LIST_T *ptProxySend;

    COMM_QUEUE_LIST_T *pt485Recv;
    COMM_QUEUE_LIST_T *pt485Send;

    COMM_QUEUE_LIST_T *ptSMS0Recv;
    COMM_QUEUE_LIST_T *ptSMS0Send;
    
    COMM_QUEUE_LIST_T *ptSMS1Recv;
    COMM_QUEUE_LIST_T *ptSMS1Send;

    COMM_QUEUE_LIST_T *pt232Recv;
    COMM_QUEUE_LIST_T *pt232Send;

    COMM_QUEUE_LIST_T *ptIRRecv;
    COMM_QUEUE_LIST_T *ptIRSend;

    COMM_QUEUE_LIST_T *ptAppTransRecv;
    COMM_QUEUE_LIST_T *ptAppTransSend;

    int                sockfd;
    uint8             *pGprs0ReStart;
    uint8             *pGprs1ReStart;
    uint8             *pEth0ReStart;
    uint8             *pEth1ReStart;
    uint8             *pEthListenReStart;
    uint8             *pBlueReStart;
    uint8             *pRadioReStart;
    uint8             *pProxyReStart;
    uint8             *p485ReStart;
    uint8             *pSMS0ReStart;
    uint8             *pSMS1ReStart;
    uint8             *p232ReStart;
    uint8             *pIRReStart;

    uint8             *pIPCok;

    uint32             eventnum;
    COMM_IPC_EVENT     event[COMM_IPC_EVENT_NAX];

    uint8             *pRunFlag;
}COMM_IPC_HANDLE_T;


void *ipc_sock_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif
