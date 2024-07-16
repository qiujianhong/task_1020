#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_BT  "bluetooth"
#define   BT_DEVICE_NUM    1


typedef enum    ///<蓝牙信息
{
    HAL_BTINFO_MAC =0,  ///< 蓝牙MAC,     固定格式: "xx.xx.xx.xx.xx.xx"
    HAL_BTINFO_PAIRM,   ///< 配对模式,    1:需要配对码  0:无需配对码
    HAL_BTINFO_PAIRC,   ///< 配对码,      数据格式: "xxxxxx", 最大长度16
    HAL_BTINFO_BCINVL,  ///< 广播间隔，   数据格式: uint32 , 625us的倍数，取值范围32-16384
    HAL_BTINFO_SDBM,    ///< 发送功率     数据格式: int32   取值范围1～16 表示-15dBm->8dBm
    HAL_BTINFO_NAME,    ///< 蓝牙名称     数据格式: "xxxxxxxx", 最大长度32 
    HAL_BTINFO_VER,     ///< 蓝牙版本信息 数据格式: "xxxxxxxx", 最大长度32 
} HAL_BTINFO_E;


typedef struct tag_BT_MODULE 
{
    HW_MODULE common;
}BT_MODULE;

typedef struct tag_BT_DEVICE
{
struct tag_HW_DEVICE    base;
/**
* @brief       蓝牙数据发送
* @param[in]     data: 数据内容
* @param[in]     len: 发送数据长度
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32(*bt_send)(struct tag_BT_DEVICE *dev, uint8 *data, int32 len);

/**
* @brief       蓝牙数据接收
* @param[out]  data: 数据内容
* @param[in]   len:缓冲区长度
* @return      成功返回数据长度；失败返回错误码。详见：附录1。
*/
int32(*bt_recv)(struct tag_BT_DEVICE *dev, uint8 *data, int32 len);

/**
* @brief       蓝牙模组软复位
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32(*bt_reset)(struct tag_BT_DEVICE *dev);

/**
* @brief        蓝牙参数设置
* @param[in]    id: 信息ID
* @param[in]    data: 设置参数内容
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32(*bt_set_param)(struct tag_BT_DEVICE *dev, HAL_BTINFO_E id, void* data);

/**
* @brief        蓝牙读取信息
* @param[in]    id: 信息ID
* @param[in]    len: 缓冲区最大长度
* @param[out]   info: 信息内容
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32(*bt_get_param)(struct tag_BT_DEVICE *dev, HAL_BTINFO_E id, void* info, int32 len);

/**
* @brief        读取蓝牙连接状态
* @param[in] 	dev: 设备描述
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
bool(*bt_is_connect)(struct tag_BT_DEVICE *dev);


// 用户扩展部分   
}BT_DEVICE_T;

#ifdef __cplusplus
}
#endif 


#endif

