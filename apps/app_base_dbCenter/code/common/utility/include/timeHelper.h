/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 时间相关函数封装接口
* @date： 2019-11-25
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

//获取系统时间
void getSysTime(struct timespec *tv);
//比较时间
void compareSysTime(struct timeval *tv1, struct timeval *tv2, struct timeval *out);
//计算十分超时
int compare_timeout_s(struct timespec *tv1, struct timespec *tv2, int s);
//获取时间字符串
int get_unix_time_str(char timeStr[MAX_TIME_STR_LEN]);

#ifdef __cplusplus
}
#endif

#endif