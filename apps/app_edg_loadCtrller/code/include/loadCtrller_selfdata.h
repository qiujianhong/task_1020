/*
*********************************************************************
* Copyright(C) 2024 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���ɿ�������
* @date��    2024-07-4
* @history�� 
*********************************************************************
*/
#ifndef __LOADCTRLLER_SLEFDATA_H__
#define __LOADCTRLLER_SLEFDATA_H__

#ifdef __cplusplus
    extern "C" {
#endif

/** @brief �����еı���״̬*/
typedef struct
{
    pthread_mutex_t        mutex;              //�� ���̷߳���
    uint8                  guardState;         //����״̬
}PROTECT_CTRL_T;

/** @brief �����е�ң��״̬*/
typedef struct
{
    pthread_mutex_t        mutex;               //�� ���̷߳���
    uint8                  state[OOP_MAX_TRIP]; //ң��״̬
    uint8                  type[OOP_MAX_TRIP];  //ң������
    TELECON_TRIP           trip[OOP_MAX_TRIP];
    TELECON_CLOSE          close[OOP_MAX_TRIP];
}REMOTE_CTRL_T;

/** @brief �����еĴ߷Ѹ澯״̬*/
typedef struct
{
    pthread_mutex_t        mutex;              //�� ���̷߳���
    uint8                  status;             //Ͷ��״̬
    OOP_CALLFEE_T          param;              //�߷Ѹ澯����
}FEE_CTRL_T;



#ifdef __cplusplus
}
#endif

#endif //__LOADCTRLLER_SLEFDATA_H__

