/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� ʱ����غ�����װ�ӿ�
* @date�� 2019-11-25
*********************************************************************
*/
#ifndef __TIME_HELPER_H__
#define __TIME_HELPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "baseConstant.h"

#ifdef __cplusplus
extern "C" {
#endif

//��ȡϵͳʱ��
void getSysTime(struct timespec *tv);
//�Ƚ�ʱ��
void compareSysTime(struct timeval *tv1, struct timeval *tv2, struct timeval *out);
//����ʮ�ֳ�ʱ
int compare_timeout_s(struct timespec *tv1, struct timespec *tv2, int s);
//��ȡʱ���ַ���
int get_unix_time_str(char timeStr[MAX_TIME_STR_LEN]);

#ifdef __cplusplus
}
#endif

#endif