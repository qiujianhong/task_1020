#ifndef _RTC_H
#define _RTC_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <linux/rtc.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_RTC     "rtc"
#define   RTC_LIB_PATH         "/lib/hal_lib/libhal_rtc.so"  /*rtc动态库路径 */

/* 实时时钟数据结构 */
typedef struct
{	
	char	ss;	/* 0~59 BCD */
	char	nn;	/* 0~59 BCD */
	char	hh;	/* 0~23 BCD */
	char	dd;	/* 1~31 BCD */
	char	mm;	/* 1~12 BCD */
	char	yy;	/* 00~99 BCD */
	char	ww;	/* 0~6 opposite Sun~Sat */	
}rtc_t;

/* 定义RTC模块结构 */
typedef struct tag_RTC_MODULE 
{
    HW_MODULE common;
}RTC_MODULE;

/*定义RTC设备结构 */
typedef struct tag_RTC_DEVICE
{
  HW_DEVICE    base;
  
/**
* @brief 设置rtc时间
* @param[in] dev: 设备描述
* @param[in] val: 时间参数
* @return 成功返回读出的大于0的数据长度；失败返回FALSE
*/
int32 (*rtc_time_set)(struct tag_RTC_DEVICE *dev, struct tm *val);
/**
* @brief 获取rtc时间
* @param[in] dev: 设备描述
* @param[out] val: 时间参数
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*rtc_time_get)(struct tag_RTC_DEVICE *dev, struct tm *val);
/**
* @brief 设置rtc秒脉冲状态 1：开启 0：关闭
* @param[in] dev: 设备描述
* @param[in] val: 状态值
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*rtc_mode_set)(struct tag_RTC_DEVICE *dev, int32 val);
/**
* @brief 获取rtc秒脉冲状态 1：开启 0：关闭
* @param[in] dev: 设备描述
* @param[out] val: 状态值
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*rtc_mode_get)(struct tag_RTC_DEVICE *dev, int32 *val);  
/**
* @brief 设置rtc温度补偿
* @param[in] dev: 设备描述
* @param[in] val: 温度值
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*rtc_temp_set)(struct tag_RTC_DEVICE *dev, float32 val);
// 用户扩展部分   
}RTC_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

