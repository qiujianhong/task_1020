/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ�� ��̫��ͨ��ͷ�ļ�
* @date��    2020-3-7
* @history�� 
*********************************************************************
*/
#ifndef	__COMM_ETH_H
#define __COMM_ETH_H

#ifdef __cplusplus
        extern "C" {
#endif

#define ETH_CONNECT_MASTER     0       /* ������վ */
#define ETH_SEND_DATA          1       /* ���ݷ��� */
#define ETH_RECV_DATA          2       /* ���ݽ��� */
#define ETH_CLOSE_DEVICE       3       /* �ر� */

/* OAD���� */
typedef struct tagCommEth
{
    OOP_ETHCONFIG_T    tEthCfg;
    OOP_MASTERPARAMS_T tMaster;
}COMM_ETH_T;

/* ������̫�������߳̾�� */
typedef struct tagCommEthHandle
{
    uint8              no;
    uint8              bGetCfg;      /* �Ƿ��ȡ������ */
    uint8              bEthLink;     /* ��̫�����Ƿ����� */
    uint8              bChangeParam; /* �Ƿ�����Ӱ�����ӵĲ����仯����Ҫ֪ͨ��Լ�޸Ĳ��� */

    COMM_ETH_T         tEthCfg;      //����
    
    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
    
    uint8              bClose;     //�Ƿ�رչ���
    uint8              masterNUM; //������վ������
    uint8              masterStart;  //�ͻ��� ����ģʽ ʹ��
    uint8              res[1];

    uint32             ip[OOP_MAX_HOST];
    uint16             port[OOP_MAX_HOST];
    int                connect_fd[OOP_MAX_HOST];
    uint32             connet_time[OOP_MAX_HOST]; /* �ͻ������ӵ�ʱ�� */
}COMM_ETH_HANDLE_T;

void eth_set_name(uint8 ifno, char *ifname);
void comm_timer_getETH(int socketfd, void *pArg);
int comm_eth_statusmsg(uint8 *playload, uint16 msglen, void *pArg);
int comm_eth_eventmsg(uint8 *playload, uint16 msglen, void *pArg);
int comm_getcheck_ethcfg(int sockdbfd, void *pCfg, uint8 *pRestart);
BOOLEAN comm_eth_start(void *pArg);
void *comm_eth_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif

