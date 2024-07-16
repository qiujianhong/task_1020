/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程msg入口出口操作
* @date：    2020-3-9
* @history： 
*********************************************************************
*/
#ifndef	__APP104_MSG_H
#define __APP104_MSG_H


#ifdef __cplusplus
        extern "C" {
#endif

typedef struct tagAppOutMsg
{
    uint8          *pRunFlag;
}APPOUTMSG_THREAD_T;

void *advPd_app104msg_entry(void *pArg);


#ifdef __cplusplus
}
#endif

#endif

