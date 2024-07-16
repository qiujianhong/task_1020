/*
*********************************************************************
* Copyright(C) 2024 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���ɵ�������
* @date��    2024-06-27
* @history�� 
*********************************************************************
*/
#ifndef __LOADCTRLLER_MSG_H__
#define __LOADCTRLLER_MSG_H__

#ifdef __cplusplus
    extern "C" {
#endif


#define LCTRL_MAP_FUNC_CTRL     7


typedef struct tagLCtrlDBCheck
{
    uint32           dataflag;
    uint32           maskInfo;  //����, ����һ���OAD����
}LCTRL_DB_CHECK_T;


#define LCTRL_APPMSG_MAXLEN   8192
typedef struct tagLCtrlAPPMSG
{
    char     topic[256];

    time_t   timeflag; //��ʱ��ѯ���
    uint8    bReg;
    uint8    res[1];
    uint16   msglen;
    uint8    recv[LCTRL_APPMSG_MAXLEN];
}LCTRL_APPMSG_T;

    /* ��ں������� */
typedef int (*MsgProcFun)(MESSAGE_INFO_T *, void *, void *);
typedef struct tagLcMsgFun
{
    uint16             IID;
    uint16             IOP;
    uint32             rpm;      /* rpm */
    MsgProcFun         pProcMsg; /* ������� */
}LCTRL_MSGFUN_T;

#define LCTRL_APP_SYNC_NUM      2
typedef struct tagLcAppSyncMsg
{
    pthread_mutex_t    lock;      //�ͻ��˲�����
    pthread_cond_t     notify;    //�߳�ģʽ��������
    uint16             IID;
    uint16             IOP;
    uint16             index;
    uint8              bWait;
    uint8              res[3];
    uint16             recvlen;
    uint8              recv[LCTRL_APPMSG_MAXLEN];
}LCTRL_SYNCMSG_T;

int lctrl_appmsg_recv_sync(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);

#ifdef __cplusplus
}
#endif

#endif //__LA_MSG_H__

