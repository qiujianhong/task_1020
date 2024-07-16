
/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief�?时间相关函数封装接口
* @date�?2020-6-10
*********************************************************************
*/
#include "timeHelper.h"
#include <time.h>


/**
*********************************************************************
* @brief：获取系统时�?* @param[in]�?NA
* @param[out]：tv
* @return：none
*********************************************************************
*/
void getSysTime(struct timespec *tv)
{
	clock_gettime(CLOCK_MONOTONIC_RAW, tv);
}

/**
*********************************************************************
* @brief：时间比�?* @param[in]�?tv1      timeval
               tv2      timeval
* @param[out]：out      时间茶�?* @return：none
*********************************************************************
*/
void compareSysTime(struct timeval *tv1,
                    struct timeval *tv2,
                    struct timeval *out)
{
    out->tv_sec  = tv1->tv_sec - tv2->tv_sec;
    out->tv_usec = tv1->tv_usec - tv2->tv_usec;
}

/**
*********************************************************************
* @brief：超时函�?* @param[in]�?tv1      timeval
               tv2      timeval
               s        超时�?* @param[out]：none
* @return�?  超时
           0  未超�?*********************************************************************
*/
int compare_timeout_s(struct timespec *tv1, struct timespec *tv2, int s)
{
    if (tv2->tv_sec - tv1->tv_sec > s)
    {
        return 1;
    }
    else if (tv2->tv_sec - tv1->tv_sec == s && tv2->tv_nsec - tv1->tv_nsec)
    {
        return 1;
    }

    return 0;
}

/**
*********************************************************************
* @brief：获得系统时间字符串
* @param[in]�?none
* @param[out]：timeStr 系统时间字符�?* @return�?1  获取失败
           0  获取成功
*********************************************************************
*/
int get_unix_time_str(char timeStr[MAX_TIME_STR_LEN])
{
    time_t     timep;
    struct tm  utc_tm;
    struct timeval tv;


    time(&timep);
    gettimeofday(&tv, NULL);
    if(NULL != gmtime_r(&timep,&utc_tm))
    {
        /*


struct tm {
int    tm_sec; //   seconds [0,61]
int    tm_min; //   minutes [0,59]
int    tm_hour; //  hour [0,23]
int    tm_mday; //  day of month [1,31]
int    tm_mon; //   month of year [0,11]
int    tm_year; //  years since 1900
int    tm_wday; //  day of week [0,6] (Sunday = 0)
int    tm_yday; //  day of year [0,365]
int    tm_isdst; // daylight savings flag
};
        */
        snprintf(timeStr, MAX_TIME_STR_LEN, "%04d%02d%02d%02d%02d%02d%03ld",
                 utc_tm.tm_year + 1900, utc_tm.tm_mon + 1, utc_tm.tm_mday,
                 utc_tm.tm_hour, utc_tm.tm_min, utc_tm.tm_sec,(tv.tv_usec/1000));
        return 0;
    }
    return -1;
}