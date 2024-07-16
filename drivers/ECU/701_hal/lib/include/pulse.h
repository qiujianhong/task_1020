#ifndef _PULSE_H
#define _PULSE_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_PULSE  "pulse"
#define   PULSE_LIB_PATH      "/lib/hal_lib/libhal_pulse.so"   /*pulse 动态库路径 */

/* 定义PULSE模块结构 */
typedef struct tag_PULSE_MODULE 
{
    HW_MODULE common;
}PULSE_MODULE;

/*定义PULSE设备结构 */
typedef struct tag_PULSE_DEVICE
{
	HW_DEVICE	base;

	/**
	* @brief 读取某一路脉冲数
	* @param[in] dev: 设备描述
	* @param[in] channel: 选择第几路脉冲（0~max）
	* @param[out] pulse_count: 该路脉冲数
	* @return 成功返回读出的大于0的数据长度；失败返回FALSE
	*/
	int32 (*pulse_count_read)(struct tag_PULSE_DEVICE *dev, uint32 channel,uint32 *pulse_count);
	/**
	* @brief 读取某一路脉冲1分钟内的实际耗时和脉冲数
	* @param[in] dev: 设备描述
	* @param[in] channel: 选择第几路脉冲（0~max）
	* @param[out] pulse_count_per_minute: 该路脉冲数
	* @param[out] real_pulse_time_per_minute: 该路脉冲耗时（单位：mv）
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*pulse_real_time_and_count_per_minute_read)(struct tag_PULSE_DEVICE *dev, uint32 channel,
	      uint32 *pulse_count_per_minute, uint32 *real_pulse_time_per_minute);
	/**
	* @brief 配置某一路脉宽
	* @param[in] dev: 设备描述
	* @param[in] channel: 选择第几路脉冲（0~max）
	* @param[in] time: 脉宽，单位ms
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*pulse_config_time)(struct tag_PULSE_DEVICE *dev, uint32 channel, uint32 time);
	// 用户扩展部分	
}PULSE_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

