#ifndef __CAN_H
#define __CAN_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netdb.h>

#define   CAN0_DEVICE_ID_STR      "can0"
#define   CAN1_DEVICE_ID_STR      "can1"
#define   CAN_LIB_PATH            "/lib/hal_lib/libhal_can.so"   /* can 动态库路径 */

#define MAX_CAN_NAME_LEN               32

/* 定义WATCHDOG模块结构 */
typedef struct tag_CAN_MODULE 
{
    HW_MODULE common;
}CAN_MODULE;

typedef struct tag_CAN_DEVICE
{
    struct tag_HW_DEVICE    base;
    
    /**
    * @brief 设置CAN通信参数
    * @param[in] dev: 设备描述
    * @param[in] baud 波特率
    * @return成功返回ERR_OK；失败返回错误码。详见：附录。
    */
    int32 (*can_param_set)(struct tag_CAN_DEVICE *dev, uint32 baud);

    /**
    * @brief 从CAN接收数据
    * @param[in] dev: 设备描述
    * @param[out] buf 接收缓存区
    * @param[in] len 缓存区长度
    * @return 成功返回大于0的数据长度，失败返回错误码。详见：附录。
    */
    int32 (*can_data_recv)(struct tag_CAN_DEVICE *dev, uint8* buf, uint32 len);
    
    /**
    * @brief 通过CAN发送数据
    * @param[in] dev: 设备描述
    * @param[in] buf 发送缓存区
    * @param[in] len 缓存区长度
    * @return成功返回ERR_OK；失败返回错误码。详见：附录。
    */
    int32 (*can_data_send)(struct tag_CAN_DEVICE *dev, const uint8* buf, int32 len);
    
    /**
    * @brief 获取CAN句柄
    * @param[in] dev: 设备描述
    * @return返回CAN打开时的句柄值（正常>0，异常<=0）。
    */
    int32 (*can_get_handle)(struct tag_CAN_DEVICE *dev);
    
    // 用户扩展部分   
    int32 fd;
    canid_t can_id;
    
    int32  baud;
    char   devName[MAX_CAN_NAME_LEN];
    //int32  canfd;
    uint8* rbuf;
    int32  rlen;
}CAN_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

