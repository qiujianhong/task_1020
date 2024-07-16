/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 IPC通信头文件
* @date：    2020-3-7
* @history： 
*********************************************************************
*/
#ifndef	__COMM_IPC_H
#define __COMM_IPC_H

#ifdef __cplusplus
        extern "C" {
#endif

#define INIT_SOCK             0     /*环回socket运行状态 - 初始化socket连接*/
#define RECV_SOCK             1     /*环回socket运行状态 - socket数据接收*/
#define SEND_SOCK             2     /*环回socket运行状态 - socket数据发送*/
#define CLOSE_SOCK            3     /*环回socket运行状态 - 关闭socket连接*/

/* IPC线程句柄 */
typedef struct tagCommIPCHandle
{
    /* 对于IPC 发送队列就是 其他线程的接收队列 */
    COMM_QUEUE_LIST_T *ptGPRS0Recv;
    COMM_QUEUE_LIST_T *ptGPRS0Send;
    
    COMM_QUEUE_LIST_T *ptGPRS1Recv;
    COMM_QUEUE_LIST_T *ptGPRS1Send;

    COMM_QUEUE_LIST_T *ptETH0Recv;
    COMM_QUEUE_LIST_T *ptETH0Send;
    
    COMM_QUEUE_LIST_T *ptETH1Recv;
    COMM_QUEUE_LIST_T *ptETH1Send;

    COMM_QUEUE_LIST_T *ptListenRecv;
    COMM_QUEUE_LIST_T *ptListenSend;

    int                sockfd;
    uint8             *pGprs0ReStart;
    uint8             *pGprs1ReStart;
    uint8             *pEth0ReStart;
    uint8             *pEth1ReStart;
    uint8             *pListenReStart;
    
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_IPC_HANDLE_T;


void *ipc_sock_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif

