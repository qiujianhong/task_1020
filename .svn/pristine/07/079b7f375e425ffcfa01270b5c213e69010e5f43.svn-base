/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 串口(蓝牙)通信头文件
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#ifndef	__APP_COMM_H
#define __APP_COMM_H

#include "message.h"

#ifdef __cplusplus
        extern "C" {
#endif


/* 232蓝牙线程句柄 */
typedef struct tagAppTransHandle
{
    uint8              bOpen;
    uint8              res[3];
    
    MESSAGE_INFO_T     tMsgInfo;
    
    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;
    
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_APP_TRANS_HANDLE_T;

int comm_app_trans_msg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
BOOLEAN comm_app_trans_start(void *pArg);
void *comm_app_trans_entry(void *pArg);


#ifdef __cplusplus
}
#endif

#endif
