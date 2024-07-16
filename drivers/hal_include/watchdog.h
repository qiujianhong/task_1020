#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>
#include <linux/watchdog.h>


#define   HW_DEVICE_ID_WATCHDOG  "drv_watchdog"
#define   WATCHDOG_LIB_PATH      "/lib/hal_lib/libhal_watchdog.so"   /*watchdog 动态库路径 */

/* 定义WATCHDOG模块结构 */
typedef struct tag_WATCHDOG_MODULE 
{
    HW_MODULE common;
}WATCHDOG_MODULE;

/*定义WATCHDOG设备结构 */
typedef struct tag_WATCHDOG_DEVICE
{
    HW_DEVICE    base;

    /**
    * @brief 设置看门狗溢出时间
    * @param[in] dev: 设备描述
    * @param[in] period: 超时时间，取值范围[1,3600]，单位：秒
    * @return 成功返回ERR_OK；失败返回错误码
    */
    int (*watchdog_period_set)(struct tag_WATCHDOG_DEVICE *dev, unsigned int period);

    /**
    * @brief 喂狗操作
    * @param[in] dev: 设备描述
    * @return 成功返回ERR_OK；失败返回错误码
    */
    int (*watchdog_clear)(struct tag_WATCHDOG_DEVICE *dev);

    /**
    * @brief 看门狗复位
    * @param[in] dev: 设备描述
    * @return 成功返回ERR_OK；失败返回错误码
    */
    int (*watchdog_rst)(struct tag_WATCHDOG_DEVICE *dev);

    /**
    * @brief 读取看门狗运行时间
    * @param[in] dev: 设备描述
    * @return 成功返回ERR_OK；失败返回错误码
    */
    int (*watchdog_runsec_get)(struct tag_WATCHDOG_DEVICE *dev);
}WATCHDOG_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

