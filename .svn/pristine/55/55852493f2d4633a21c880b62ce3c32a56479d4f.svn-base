/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 电台通信头文件
* @date：    2022-07-29
* @history： 
*********************************************************************
*/
#ifndef __RADIO_COMM_H
#define __RADIO_COMM_H

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_RADIO_STATUS_GETAPP   0
#define COMM_RADIO_STATUS_SETCFG   1
#define COMM_RADIO_STATUS_PREPARE  2
#define COMM_RADIO_STATUS_WORK     3

/* 电台线程句柄 */
typedef struct tagRadioHandle
{
    uint8              bGetCfg;   /* 是否获取到配置 */
    uint8              btStatus;  
    RADIO_CONFIG_T     radioCfg;           /* 参数 */
    
    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;
    
    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_RADIO_HANDLE_T;

int comm_getcheck_radio(int sockdbfd, void *pCfg, uint8 *pRestart);
void comm_timer_radio_status(int socketfd, void *pArg);
int comm_radio_getcfgmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_radio_setcfgmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_radio_recvmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
BOOLEAN comm_radio_start(void *pArg);
void *comm_radio_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif


