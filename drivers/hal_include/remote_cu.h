#ifndef _REMOTE_CU_H
#define _REMOTE_CU_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>


#define   HW_DEVICE_ID_REMOTE_CU  "remote_cu"
#define   REMOTE_CU_LIB_PATH      "/lib/hal_lib/libhal_remote_cu.so"   /* remote_cu 动态库路径 */

/* 定义WATCHDOG模块结构 */
typedef struct tag_REMOTE_CU_MODULE 
{
    HW_MODULE common;
}REMOTE_CU_MODULE;

typedef struct tag_REMOTE_CU_DEVICE
{
    struct tag_HW_DEVICE    base;
    /**
    * @brief 设置远程通信单元电源
    * @param[in] dev: 设备描述
    * @param[in] val: 0：关闭， 1：开启
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*remote_cu_power_set)(struct tag_REMOTE_CU_DEVICE *dev, uint8 val);
    
    /**
    * @brief 复位远程通信单元
    * @param[in] dev: 设备描述
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*remote_cu_reset)(struct tag_REMOTE_CU_DEVICE *dev);
    
    /**
    * @brief 设置远程通信单元开机电平
    * @param[in] dev: 设备描述
    * @param[in] val: 0：开机电平置低， 1：开机电平置高
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*remote_cu_on_level_set)(struct tag_REMOTE_CU_DEVICE *dev, uint8 val);
    
    /**
    * @brief 获取远程通信模块单元插拔状态
    * @param[in] dev: 设备描述
    * @param[out] state: 插拔状态，0 未插入，1 有模块且响应AT指令，2 有模块不响应AT指令
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*remote_cu_insert_state_get)(struct tag_REMOTE_CU_DEVICE *dev, int32 *state);
    
    /**
    * @brief远程通信模块SIM卡加热
    * @param[in] dev: 设备描述
    * @param[in] val: 0：关闭， 1：开启
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*remote_cu_sim_card_heat)(struct tag_REMOTE_CU_DEVICE *dev, uint8 val);
    
    // 用户扩展部分

}REMOTE_CU_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

