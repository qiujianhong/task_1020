/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程IPC入口出口操作
* @date：    2019-12-3
* @history： 
*********************************************************************
*/
#ifndef	__APPIPC_H
#define __APPIPC_H


#ifdef __cplusplus
        extern "C" {
#endif


/* IPC通信配置 */
typedef struct tagIpcConfig
{
    pthread_mutex_t fLock;
    int             fdSock;
}IPC_CONFIG_T;

typedef struct tagAppIpcThread
{
    uint8        *pRunFlag;
}IPC_THREAD_T;

int   app_ipc_init();
void *app_ipc_recv(void *pArg);
void *app_ipc_send(void *pArg);

#ifdef __cplusplus
}
#endif

#endif


