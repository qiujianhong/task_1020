/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ�� ����ͨ��ͷ�ļ�
* @date��    2020-7-23
* @history�� 
*********************************************************************
*/
#ifndef	__SMS_COMM_H
#define __SMS_COMM_H

#include "comm.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define SMS_CENTER_MANA        0       /* �������ĺ������ */
#define SMS_MASTER_MANA        1       /* ��վ���� */
#define SMS_SEND_DATA          2       /* ���ݷ��� */
#define SMS_RECV_DATA          3       /* ���ݽ��� */
#define SMS_CLOSE_DEVICE       4       /* �ر� */


/* �����߳̾�� */
typedef struct tagCommSMSHandle
{
    int          dbClient;
    uint8              no;          /* ��� */
    uint8              bGetCfg;     /* ��ȡ���óɹ� */
    uint8              bSimOK; 
    uint8              bDiagOK;     /* ���ųɹ� */

    uint8              smsType;     /* ���Ÿ�ʽ PDU��0����TEXT��1�� */
    uint8              bCenter;     /* �Ƿ����óɹ��������ĺ��� */
    uint8              bCfgChange;  /* �����仯 */
    uint8              res[1];

    OOP_SMS_T          tSMS;        /* ����ͨ�Ų��� */

    uint8              masterid[OOP_MAX_HOST]; /* ��������վͨ��id */

    COMM_QUEUE_LIST_T *ptRecvList;  /* ���ն��� */
    COMM_QUEUE_LIST_T *ptSendList;  /* ���Ͷ��� */

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
