#ifndef _ESAM_H
#define _ESAM_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define HW_DEVICE_ID_YX_ESAM "yx_esam" /* 营销esam*/
#define HW_DEVICE_ID_PW_ESAM "pw_esam" /* 配网esam*/


#define ESAM_LIB_PATH      "/lib/hal_lib/libhal_esam.so"  /*esam 动态库路径 */ 

/*电源子模块 */
typedef enum
{
    DEV_SWITCH=2,
    DEV_NET=4,
    DEV_USBNET=5,
    ESAM_PWR=6,
}ESAM_SUBDEV;

/*控制状态，0 关闭，1 打开 */
typedef enum
{
    PWR_OFF=0,
    PWR_ON=1,
}ESAM_PWR_STATE;

typedef enum
{
    EM_ESAM_YX = 0,
    EM_ESAM_PW,
} EM_ESAM_USR;

/* 定义ESAM模块结构 */
typedef struct tag_ESAM_MODULE 
{
    HW_MODULE common;
}ESAM_MODULE;

/*定义ESAM设备结构 */
typedef struct tag_ESAM_DEVICE
{
    HW_DEVICE    base;
    
    /**
    * @brief 控制esam复位
    * @param[in] dev: 设备描述
    * @return 成功返回ERR_OK；失败返回错误码
    */
    int32 (*esam_set_reset)(struct tag_ESAM_DEVICE *dev);
    
    /**
    * @brief 设置esam电源状态 1：开启 0：关闭
    * @param[in] dev: 设备描述
    * @param[in] val: 电源状态
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*esam_set_power)(struct tag_ESAM_DEVICE *dev, int32 val);
    
    /**
    * @brief 设置esam设备加锁
    * @param[in] dev: 设备描述
    * @param[in] val: 1为上锁  0 为解锁
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*esam_set_lock)(struct tag_ESAM_DEVICE *dev, int32 val);
    
    /**
    * @brief 读取esam数据
    * @param[out] buf: 读取数据缓存
    * @param[in] len: 读取数据长度
    * @return成功返回读出的数据长度；失败返回错误码。
    */
    int32 (*esam_data_read)(struct tag_ESAM_DEVICE *dev, uint8 *buf, int32 len);
    
    /**
    * @brief写ESAM数据
    * @param[in] dev: 设备描述
    * @param[in] buf: 写数据缓存
    * @param[in] len: 写数据长度
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*esam_data_write)(struct tag_ESAM_DEVICE *dev, uint8 *buf, int32 len);
    // 用户扩展部分    
    int32 fd_lock;
	int32 fd;
    EM_ESAM_USR             type;                   ///< 0:xesam 1:pesam
    pthread_mutex_t         devMetux;
}ESAM_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

