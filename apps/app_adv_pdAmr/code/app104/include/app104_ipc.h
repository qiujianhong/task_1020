/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ���IPC��ڳ��ڲ���
* @date��    2020-3-9
* @history�� 
*********************************************************************
*/
#ifndef	__APP104_IPC_H
#define __APP104_IPC_H


#ifdef __cplusplus
        extern "C" {
#endif


/* IPCͨ������ */
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

