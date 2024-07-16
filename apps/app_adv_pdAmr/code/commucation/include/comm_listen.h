/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 以太网通信头文件
* @date：    2020-3-19
* @history： 
*********************************************************************
*/
#ifndef	__COMM_LISTEN_H
#define __COMM_LISTEN_H

#ifdef __cplusplus
        extern "C" {
#endif

#define LISTEN_OPEN_DEVICE        0       /* 打开监听 */
#define LISTEN_SEND_DATA          1       /* 数据发送 */
#define LISTEN_RECV_DATA          2       /* 数据接收 */
#define LISTEN_CLOSE_DEVICE       3       /* 关闭 */


/* 本地以太网监听线程句柄 */
typedef struct tagCommListenHandle
{
    uint16             port;    //配置
    uint8              num;
    uint8              accept_num;  //侦听端口数量

    int                listen_fd;                  // 侦听句柄
    int                accept_fd[COMM_LISTEN_MAX]; // 连接句柄
    uint32             cIP[COMM_LISTEN_MAX];       // 客户端IP
    uint32             cPort[COMM_LISTEN_MAX];     // 客户端PORT

    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_LISTEN_HANDLE_T;


BOOLEAN comm_listen_start(void *pArg);
void *comm_listen_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif

