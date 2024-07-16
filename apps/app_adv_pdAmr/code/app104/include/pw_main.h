/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2020-01-03
*********************************************************************
*/


#ifndef __CCO_MAIN_H__
#define __CCO_MAIN_H__

#include "pw_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************************************************************************
* ȫ�ֱ�������
*******************************************************************************/
extern THREAD_ARG_T g_sUpRecvDealThreadArg; 		//������վ���������̲߳���
extern THREAD_ARG_T g_sUpSendDealThreadArg; 		//������վ�ϱ������̲߳���
extern THREAD_ARG_T g_sDownDeviceThreadArg; 		//�����豸ͨ���̲߳���
extern THREAD_ARG_T g_sEventCheckThreadArg;		    //����ģ���¼�����̲߳���
extern THREAD_ARG_T g_sMonitStatThreadArg;          //���ͳ���̲߳���

extern uint8 g_nUpRecvDealThreadFlag;               //��վ�����������̱߳�־
extern uint8 g_nUpSendDealThreadFlag;               //�ϱ���վ�������̱߳�־
extern uint8 g_nDownDeviceThreadFlag;               //�����豸�̱߳�־
extern uint8 g_nEventCheckThreadFlag;               //����ģ���¼�����̱߳�־
extern uint8 g_nMonitStatThreadFlag;                //���ͳ���̱߳�־

/*******************************************************************************
* ȫ�ֺ�������
*******************************************************************************/
extern void *event_check_thread(void *arg);
extern void *up_recv_deal_thread(void *arg);
extern void *down_device_thread(void *arg);
extern void *monit_stat_thread(void *arg);
extern int pw_main_init();




#ifdef __cplusplus
}
#endif

#endif // __CCO_MAIN_H__
