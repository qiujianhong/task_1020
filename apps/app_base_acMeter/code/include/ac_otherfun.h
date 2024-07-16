/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#ifndef _AC_OTHERFUN_H
#define _AC_OTHERFUN_H

#include "ac_redef.h"

#ifdef ARM32R
#define TID_CONTAINER_APP     0      //����APP�Լ�
#define TID_CONTAINER         0      //�����Լ�
#else
#define TID_CONTAINER_APP     1      //����APP�Լ�
#define TID_CONTAINER         1      //�����Լ�
#endif
#define TID_TIME              1      //�Զ���ʱ����
#define TID_SLEF_CHECK        1      //�ն��Լ�
#define TID_HUB               1      //hub�Լ�

#if defined PRODUCT_ZCU_1
#define TID_MAGNETIC          1      //�ų����
#else
#define TID_MAGNETIC          0      //�ų����
#endif


#if TID_CONTAINER_APP == 1
#define CONTAINER_APP_NUM_MAX     16

typedef struct tagAPPCHECK
{
    char *appName;          //APP����
    char *containerName;    //��������
    char *appTarPath;       //APP��װ��·��
}APP_CHECK_T;

extern const APP_CHECK_T appCheck[];
extern const uint32 appCheckNum;

void proc_container_app_chk(char *appName, struct timespec curtime, struct timespec oldtime, uint32 *pNoCheckCnt);
#endif

#if TID_MAGNETIC == 1
//���ų���IO�ӿ�
#define    CC1_PIN            SCM_PIN_PD13
#define    CC2_PIN            SCM_PIN_PI20
#define    GPIO_DEV_NAME      "/dev/gpio"

extern void magnetic_dev_init(void);
extern int get_mapnetic_status(void);
extern void proc_magnetic_chk(int *pMagneticStatusLast, int *pMagneticStatusTmp);
#endif

#if TID_TIME == 1
extern void *pthread_time_chk(void *arg);
#endif

#if TID_SLEF_CHECK == 1
extern void *pthread_self_chk(void *arg);
#endif

#endif