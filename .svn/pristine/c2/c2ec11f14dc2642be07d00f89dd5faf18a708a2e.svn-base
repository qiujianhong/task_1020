#ifndef _LOCAL_CU_H
#define _LOCAL_CU_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>


#define   HW_DEVICE_ID_LOCAL_CU  "local_cu"
#define   LOCAL_CU_LIB_PATH      "/lib/hal_lib/libhal_local_cu.so"   /* local_cu 动态库路径 */

/* 定义WATCHDOG模块结构 */
typedef struct tag_LOCAL_CU_MODULE 
{
    HW_MODULE common;
}LOCAL_CU_MODULE;

typedef struct tag_LOCAL_CU_DEVICE
{
    struct tag_HW_DEVICE    base;
    /**
    * @brief 设置本地通信单元电源
    * @param[in] dev: 设备描述
    * @param[in] val: 0：关闭， 1：开启
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int (*local_cu_power_set)(struct tag_LOCAL_CU_DEVICE *dev, uint8 val);
    
    /**
    * @brief 复位本地通信单元
    * @param[in] dev: 设备描述
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int (*local_cu_reset)(struct tag_LOCAL_CU_DEVICE *dev);

    /**
    * @brief 获取本地通信模块单元插拔状态
    * @param[in] dev: 设备描述
    * @param[out] state: 插拔状态，1插入，0 拔出
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int (*local_cu_insert_state_get)(struct tag_LOCAL_CU_DEVICE *dev, int *state);
    // 用户扩展部分 

} LOCAL_CU_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

