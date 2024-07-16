/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程IPC入口出口操作
* @date：    2020-3-9
* @history： 
*********************************************************************
*/
#ifndef	__APP104_IPC_H
#define __APP104_IPC_H


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

int   advPd_appipc_init();
void *advPd_appipc_entry(void *pArg);
int advPd_send_104msg(uint8 id, uint8 protocol, uint8 appType, uint8 *pMsg, uint8 msgLen);


#ifdef __cplusplus
}
#endif

#endif

