/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程对外通信线程
* @date：    2019-12-6
* @history： 
*********************************************************************
*/
#ifndef	__APPOUTMSG_H
#define __APPOUTMSG_H


#ifdef __cplusplus
        extern "C" {
#endif

typedef struct tagAppOutMsg
{
    uint32          timeStep;

    int            *pClientId;
    uint8          *pRunFlag;
}OUTMSG_THREAD_T;

/* 参数: clientid, label, data, datalen, step */
typedef int (*AppMsgFun)(int, uint16, uint8*, uint16, uint32*);
typedef struct tagAppMsgFun
{
    uint32    msgid;
    AppMsgFun func;
}APP_MSG_FUN_T;

void *app_msg_entry(void *pArg);
void app_msg_sync_init();

    uint16 appmain_set_radio_speak(uint8 type, uint8* recvbuf, uint16 buflen);

#ifdef __cplusplus
}
#endif

#endif


