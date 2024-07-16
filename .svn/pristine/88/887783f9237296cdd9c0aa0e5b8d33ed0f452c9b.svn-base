/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 监听
* @date：    2020-2-3
* @history： 
*********************************************************************
*/
#ifndef	__LISTEN_COMM_H
#define __LISTEN_COMM_H

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_CONNECT_NUM        5      /* 监听,支持最大连接数量 */

/* 本地监听线程句柄 */
typedef struct tagCommListenHandle
{
    uint16             *ethHeart;   //以太网心跳 非0表示混合模式
    uint16             *pppHeart;   //无线心跳   非0表示混合模式(只支持无线1模块)

    uint8              *ethReStart; //混合模式下以太网心跳维护
    uint8              *pppReStart; //混合模式下无线心跳维护
    
    COMM_QUEUE_LIST_T *ptRecvList;  //接收队列
    COMM_QUEUE_LIST_T *ptSendList;  //发送队列

    int                fdListen;    //监听句柄
    int                fdClient;    //客户端句柄
    uint32             ClientTime;
    
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
