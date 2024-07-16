#ifndef _DRC_CLOSING_H
#define _DRC_CLOSING_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>


#define HW_DEVICE_ID_DRV_CLOSING  "drv_closing"
#define DRV_CLOSING_LIB_PATH      "/lib/hal_lib/libhal_drv_closing.so"   /* drv_closing 动态库路径 */

/* 定义WATCHDOG模块结构 */
typedef struct tag_DRV_CLOSING_MODULE 
{
    HW_MODULE common;
}DRV_CLOSING_MODULE;

typedef enum //<合闸模式
{
    CLOSING_PULSE = 0,
    CLOSING_LEVEL,
}CLOSING_MODE_E;

typedef enum //<动作控制
{
    CLOSING_TURN_ON = 0,
    CLOSING_TURN_OFF,
}CLOSING_TURN_E;


typedef struct tag_CLOSING_DEVICE
{
    struct tag_HW_DEVICE    base;
    
    /**
    * @brief 设置合闸继电器输出模式
    * @param[in] dev: 设备描述
    * @param[in] mode: 合闸模式
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*closing_mode_set)(struct tag_CLOSING_DEVICE *dev, CLOSING_MODE_E mode);
    
    /**
    * @brief 设置合闸继电器输出脉冲宽度
    * @param[in] dev: 设备描述
    * @param[in] period: 合闸继电器输出脉冲宽度，单位：ms
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*closing_pulse_width_set)(struct tag_CLOSING_DEVICE *dev, uint32 pulse_width);

    /**
    * @brief 获取跳闸接线状态
    * @param[in] dev: 设备描述
    * @param[out] state: 接线状态，bit0…bit(n-1)按位表示1~n 轮接线状态，1有接线，0 没接线
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*closing_line_state_get)(struct tag_CLOSING_DEVICE *dev, uint32 *state);
    
    /**
    * @brief合闸继电器输出控制
    * @param[in] dev: 设备描述
    * @param[in] id: 1~4为4轮合闸，5为告警输出（蜂鸣器同步输出）
    * @param[in] turn: 动作控制
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*closing_turn)(struct tag_CLOSING_DEVICE *dev, uint8 id, CLOSING_TURN_E turn);

    /**
    * @brief告警继电器输出控制
    * @param[in] dev: 设备描述
    * @param[in] turn: 动作控制
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*closing_turn_alarm)(struct tag_CLOSING_DEVICE *dev, CLOSING_TURN_E turn);
    
    // 用户扩展部分
    int   fd;
    uint8 index;

    CLOSING_MODE_E mode;
    uint32         pulse_width;
    void          *list;
    pthread_t      threadid;
    uint8          stop_flag;

    uint8          turn[4];
}DRV_CLOSING_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

