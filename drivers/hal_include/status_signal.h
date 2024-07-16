#ifndef _status_singal_H
#define _status_singal_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>


#define   STATUS_SIGNAL_DEVICE_ID_STR  "status_singal"
#define   STATUS_SIGNAL_LIB_PATH       "/lib/hal_lib/libhal_status_singal.so"   /* remote_cu 动态库路径 */

/* 定义WATCHDOG模块结构 */
typedef struct tag_STATUS_SIGNAL_MODULE 
{
    HW_MODULE common;
}STATUS_SIGNAL_MODULE;

typedef struct tag_STATUS_SIGNAL_DEVICE
{
    struct tag_HW_DEVICE    base;
    
    /**
    * @brief 获取门节点、上盖触点等状态信号数据
    * @param[in] dev: 设备描述
    * @param[out] status: 状态数据，按位表示；bit0-门节点，bit1-上盖触点，bit2-端尾触点，其他保留；0-分，1-合
    * @return成功返回ERR_OK；失败返回错误码。
    */
    int32 (*status_singal_get)(struct tag_STATUS_SIGNAL_DEVICE *dev, uint32 *status);
    
    // 用户扩展部分

}STATUS_SIGNAL_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

