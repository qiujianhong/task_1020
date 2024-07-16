#ifndef _MISC_H
#define _MISC_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_MISC  "misc"
#define   MISC_LIB_PATH      "/lib/hal_lib/libhal_misc.so"   /*misc 动态库路径 */

typedef enum
{
    EMFS_TYPE_UBIFS = 0, //ubifs
    EMFS_TYPE_JIFFS2 = 1, //jiffs2
    EMFS_TYPE_MAX = 255,
}FS_TYPE_E;

/* 定义MISC模块结构 */
typedef struct tag_MISC_MODULE 
{
    HW_MODULE common;
}MISC_MODULE;

/*定义MISC设备结构 */
typedef struct tag_MISC_DEVICE
{
	HW_DEVICE	base;

/**
* @brief 获取文件系统分区状态
* @param[in] dev: 设备描述
* @param[in] fs_type: 文件系统类型（如ubifs/jiffs2等）
* @param[in] partition_idx:分区索引编号
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*misc_fs_partition_state_get)(struct tag_MISC_DEVICE *dev, int32 fs_type, int32 partition_idx);
/**
* @brief 设置pwm开启/关闭
* @param[in] dev: 设备描述
* @param[in] val: 开启/关闭设置值
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*misc_pwm_state_set)(struct tag_MISC_DEVICE *dev, int32 val);
/**
* @brief 设置pwm参数
* @param[in] dev: 设备描述
* @param[in] frq: pwm频率
* @param[in] duty: pwm占空比
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*misc_pwm_param_set)(struct tag_MISC_DEVICE *dev, uint32 frq, uint32 duty);
/**
* @brief 获取pwm参数
* @param[in] dev: 设备描述
* @param[out] frq: pwm频率
* @param[out] duty: pwm占空比
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*misc_pwm_param_get)(struct tag_MISC_DEVICE *dev, uint32 *frq, uint32 *duty);
/**
* @brief 获取USB插入信息
* @param[in] dev: 设备描述
* @param[out] info: 获取信息
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*misc_usb_info_get)(struct tag_MISC_DEVICE *dev, uint32 *info);
/**
* @brief sim卡加热功能
* @param[in] dev: 设备描述
* @param[in] val: 0： 停止加热， 1： 启动加热
* @return 成功返回TRUE；失败返回FALSE
*/
int32 (*misc_sim_heater_set)(struct tag_MISC_DEVICE *dev, uint8 val);
// 用户扩展部分  
}MISC_DEVICE_T;

#ifdef __cplusplus
}
#endif 


#endif

