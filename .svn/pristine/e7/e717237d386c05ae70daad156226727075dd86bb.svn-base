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

void getSysTime(struct timeval *tv);

void compareSysTime(struct timeval *tv1, struct timeval *tv2, struct timeval *out);

int compare_timeout_s(struct timeval *tv1, struct timeval *tv2, int s);

int get_unix_time_str(char timeStr[MAX_TIME_STR_LEN]);

#ifdef __cplusplus
}
#endif

#endif