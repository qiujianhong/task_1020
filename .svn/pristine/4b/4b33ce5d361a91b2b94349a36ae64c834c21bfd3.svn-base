/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 无线通信头文件
* @date：    2020-7-23
* @history： 
*********************************************************************
*/
#ifndef	__SMS_COMM_H
#define __SMS_COMM_H

#include "comm.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define SMS_CENTER_MANA        0       /* 短信中心号码管理 */
#define SMS_MASTER_MANA        1       /* 主站管理 */
#define SMS_SEND_DATA          2       /* 数据发送 */
#define SMS_RECV_DATA          3       /* 数据接收 */
#define SMS_CLOSE_DEVICE       4       /* 关闭 */


/* 短信线程句柄 */
typedef struct tagCommSMSHandle
{
    int          dbClient;
    uint8              no;          /* 编号 */
    uint8              bGetCfg;     /* 获取配置成功 */
    uint8              bSimOK; 
    uint8              bDiagOK;     /* 拨号成功 */

    uint8              smsType;     /* 短信格式 PDU（0），TEXT（1） */
    uint8              bCenter;     /* 是否设置成功短信中心号码 */
    uint8              bCfgChange;  /* 参数变化 */
    uint8              res[1];

    OOP_SMS_T          tSMS;        /* 短信通信参数 */

    uint8              masterid[OOP_MAX_HOST]; /* 开启的主站通道id */

    COMM_QUEUE_LIST_T *ptRecvList;  /* 接收队列 */
    COMM_QUEUE_LIST_T *ptSendList;  /* 发送队列 */

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_SMS_HANDLE_T;

void comm_sms_init(COMM_SMS_HANDLE_T *ptSMS0Handle, COMM_SMS_HANDLE_T *ptSMS1Handle);
int comm_getcheck_smscfg(int sockdbfd, void *pCfg, uint8 *pRestart);
int comm_sms_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_sms_smsnomsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_sms_dcmmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
BOOLEAN comm_smd_start(void *pArg);
void *comm_sms_entry(void *pArg);



#ifdef __cplusplus
}
#endif

#endif
