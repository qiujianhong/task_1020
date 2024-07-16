/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ�� ��̫��ͨ��ͷ�ļ�
* @date��    2020-3-19
* @history�� 
*********************************************************************
*/
#ifndef	__COMM_LISTEN_H
#define __COMM_LISTEN_H

#ifdef __cplusplus
        extern "C" {
#endif

#define LISTEN_OPEN_DEVICE        0       /* �򿪼��� */
#define LISTEN_SEND_DATA          1       /* ���ݷ��� */
#define LISTEN_RECV_DATA          2       /* ���ݽ��� */
#define LISTEN_CLOSE_DEVICE       3       /* �ر� */


/* ������̫�������߳̾�� */
typedef struct tagCommListenHandle
{
    uint16             port;    //����
    uint8              num;
    uint8              accept_num;  //�����˿�����

    int                listen_fd;                  // �������
    int                accept_fd[COMM_LISTEN_MAX]; // ���Ӿ��
    uint32             cIP[COMM_LISTEN_MAX];       // �ͻ���IP
    uint32             cPort[COMM_LISTEN_MAX];     // �ͻ���PORT

    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_LISTEN_HANDLE_T;


BOOLEAN comm_listen_start(void *pArg);
void *comm_listen_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif

