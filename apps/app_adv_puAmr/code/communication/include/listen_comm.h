/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ�� ����
* @date��    2020-2-3
* @history�� 
*********************************************************************
*/
#ifndef	__LISTEN_COMM_H
#define __LISTEN_COMM_H

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_CONNECT_NUM        5      /* ����,֧������������� */

/* ���ؼ����߳̾�� */
typedef struct tagCommListenHandle
{
    uint16             *ethHeart;   //��̫������ ��0��ʾ���ģʽ
    uint16             *pppHeart;   //��������   ��0��ʾ���ģʽ(ֻ֧������1ģ��)

    uint8              *ethReStart; //���ģʽ����̫������ά��
    uint8              *pppReStart; //���ģʽ����������ά��
    
    COMM_QUEUE_LIST_T *ptRecvList;  //���ն���
    COMM_QUEUE_LIST_T *ptSendList;  //���Ͷ���

    int                fdListen;    //�������
    int                fdClient;    //�ͻ��˾��
    uint32             ClientTime;
    
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
