/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ�� ����ͨ��ͷ�ļ�
* @date��    2019-10-31
* @history�� 
*********************************************************************
*/
#ifndef	__PPP_COMM_H
#define __PPP_COMM_H

#include "comm.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define PPP_CONNECT_MASTER     0       /* ������վ */
#define PPP_SEND_DATA          1       /* ���ݷ��� */
#define PPP_RECV_DATA          2       /* ���ݽ��� */
#define PPP_CLOSE_DEVICE       3       /* �ر� */

#define PPP_CONNECT_NUM        5      /* �˿ڼ���,֧������������� */

typedef struct tagCommPortFlow
{ 
    uint32 applyRxFlow;         /* Ӧ�ò������� */
    uint32 applyTxFlow;         /* Ӧ�ò������� */

    uint64 ifnetRxFlow;         /* ���������� */
    uint64 ifnetTxFlow;         /* ���������� */
}COMM_PORTFLOW_T;

/* OAD���� �� ���߲��Ų�ѯ������ */
typedef struct tagCommGprs
{
    OOP_GPRSCONFIG_T   tOopGprs;  /* ͨ������ */
    OOP_MASTERPARAMS_T tOopMaster;/* ��վͨ�Ų��� */
    OOP_APNGROUP_T     tAPN;      /* ���߹�������ͨ�� */

    /* �������� */
    uint8              chooseType; /* ��ר�л�ģʽ */
    OOP_PRIVATE_NET_T  tPrivate;   /* ר����� */
}COMM_GPRS_T;

/* PPP�߳̾�� */
typedef struct tagCommPPPHandle
{
    uint8              no;          /* ��� */
    uint8              bGetCfg;     /* ��ȡ���óɹ� */
    uint8              bGetDCM;     /* ��ȡ����ģ��ɹ� */
    uint8              bDiagOK;     /* ���ųɹ� */

    char               curApn[64];  /* ��ǰAPN */
    char               targetApn[64];/* Ŀ��APN */

    uint8              operat;      /* ��Ӫ�� */
    uint8              nettype;     /* �������� */
    uint8              apnIndex;    /* apn���, 0-һ��ͨ��������, ����-����ͨ�� */
    uint8              bChangeApn;  /* �Ƿ����л�APN������ */

    uint8              bFirstRun;   //�״�����
    uint8              bGetApn;     //��ȡAPN
    uint8              bSingleCfg;  //��������
    uint8              bMoreCfg;    //��������

    uint8              stack;       /* Э��ջ */
    uint8              bMasterGui;  /* ������GUIˢ��״̬ 0-����Ҫˢ�� 1-���� 2-�Ͽ� */
    uint8              masterStart; //�ͻ��ˡ����ģʽʹ��, ����ģʽ������ʼ���
    uint8              res[1];
    uint16             proxyport;
    uint32             proxyip;
    time_t             getApnTime;  //��ȡAPN��ʱ��
    time_t             getDiagTime; //��ȡAPN��ʱ��

    COMM_GPRS_T        tGprsCfg;

    OOP_VERSION_T      version;     //����ģ��汾��
    OOP_VISIBLEVAR_T   DevIMEI;     //����ģ��IMEI
    OOP_VISIBLEVAR_T   OldDescript; //������ģ��������Ӧ��������ͨ��ģ�����¼�
    COMM_PORTFLOW_T    tFlow;       //����ͳ��
    
    uint8             *pETHStatus;  /* ��̫������״̬ */
    OOP_MASTERPARAMS_T *pETHMaster; /* ��̫��״̬ */

    COMM_QUEUE_LIST_T *ptRecvList;  /* ���ն��� */
    COMM_QUEUE_LIST_T *ptRecvListBak; /* ���պ󱸶��� */
    COMM_QUEUE_LIST_T *ptSendList;  /* ���Ͷ��� */

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
    uint16            *MixHeart;    //���ģʽ��������

    uint8              res1[1];
    uint8              bProxySSAL;
    uint8              bChangeParam;/* �Ƿ�����Ӱ�����ӵĲ����仯����Ҫ֪ͨ��Լ�޸Ĳ��� */ 
    uint8              masterNUM;   /* ������վ������ */
    uint8              listenNum;   /* �����˿����� */

    uint32             masterIP[OOP_MAX_HOST];    /* �ͻ��������ģʽ,UDPѡ�����վIP */
    uint16             masterPort[OOP_MAX_HOST];  /* �ͻ��������ģʽ,UDPѡ�����վPORT */
    int                temp_fd[OOP_MAX_HOST];     /* ��ʱ���, �����м�״̬���� */
    uint32             temp_time[OOP_MAX_HOST];   /* ��ʱʱ�����ӵ�ʱ�� */
    int                client_fd[OOP_MAX_HOST];   /* �ͻ��������ģʽ,UDP�¾�� */
    uint32             connet_time[OOP_MAX_HOST]; /* �ͻ��������ģʽ���ӵ�ʱ�� */

    int                listen_fd[OOP_MAX_LISTENS]; /* ������� */
    int                accept_fd[OOP_MAX_LISTENS][PPP_CONNECT_NUM]; /* ���Ӿ�� */
    uint32             accept_ip[OOP_MAX_LISTENS][PPP_CONNECT_NUM];
    uint32             accept_port[OOP_MAX_LISTENS][PPP_CONNECT_NUM];
}COMM_PPP_HANDLE_T;


int comm_getcheck_ppp0cfg(int sockdbfd, void *pCfg, uint8 *pRestart);
int comm_getcheck_ppp1cfg(int sockdbfd, void *pCfg, uint8 *pRestart);
void comm_timer_rediag(int socketfd, void *pArg);
void comm_timer_getDCM(int socketfd, void *pArg);
void comm_timer_getMode(int socketfd, void *pArg);
void comm_timer_pppmastergui(int socketfd, void *pArg);
int comm_ppp_dcmmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_ppp_modemsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_ppp_guishow(uint8 id, uint8 protocol, void *pArg);
int comm_ppp_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
void comm_timer_nextapn(int socketfd, void *pArg);
int comm_ppp_apnmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_ppp_gui_toolmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
BOOLEAN comm_ppp_start(void *pArg);
void *comm_ppp0_entry(void *pArg);
void *comm_ppp1_entry(void *pArg);



#ifdef __cplusplus
}
#endif

#endif
