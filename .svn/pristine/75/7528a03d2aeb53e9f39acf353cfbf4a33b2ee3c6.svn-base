#ifndef _POWER_H
#define _POWER_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_POWER  "power"
#define   POWER_LIB_PATH      "/lib/hal_lib/libhal_power.so"   /*power 动态库路径 */

typedef enum        ///<校验
{
POWER_MAIN = 0,
POWER_BACKUP,
} POWER_ID_E;

typedef enum        ///<校验
{
USB_NO = 0,
MODULE_1,
MODULE_2,
MODULE_3,
MODULE_4,
MODULE_5,
MODULE_BT,
MODULE_USB_HUB,
} MODULE_ID_E;

/* 定义pwer模块结构 */
typedef struct tag_POWER_MODULE 
{
    HW_MODULE common;
}POWER_MODULE;

/*定义power设备结构 */
typedef struct tag_POWER_DEVICE
{
	HW_DEVICE	base;
    
	/**
	* @brief 获取电源状态
	* @param[in] dev: 设备描述
	* @param[out] state: 电源状态，1有电，0 掉电
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*power_power_state_get)(struct tag_POWER_DEVICE *dev, uint8 power_id, int32 *state);
	/**
	* @brief 备用电源充电控制
	* @param[in] dev: 设备描述
	* @param[in] type: 0： 备用电池， 1： 法拉电容
	* @param[in] val: 0： 不充电， 1： 充电
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*power_battery_charge_set)(struct tag_POWER_DEVICE *dev, uint8 type, uint8 val);
	/**
	* @brief 备用电源供电控制
	* @param[in] dev: 设备描述
	* @param[in] type: 0： 备用电池， 1： 法拉电容
	* @param[in] val: 0： 关闭供电， 1： 供电
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*power_battery_out_set)(struct tag_POWER_DEVICE *dev, uint8 type, uint8 val);
	/**
	* @brief 扩展模块电源控制
	* @param[in] dev: 设备描述
	* module_id 	  // 5个模块+1个蓝牙+usb hub	
	* @param[in] val: 0： 关闭， 1： 开启
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*power_ply_vcc_out_set)(struct tag_POWER_DEVICE *dev,uint8 module_id, uint8 val);
	/**
	* @brief 扩展模块电源控制
	* @param[in] dev: 设备描述
	* module_id       // 5个模块+1个蓝牙+usb hub  
	* @param[in] val: 0： 关闭， 1： 开启
	* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
	*/
	int32 (*power_ply_vcc_state_get)(struct tag_POWER_DEVICE *dev,uint8 module_id, int32 *state);

	// 用户扩展部分	
}POWER_DEVICE_T;



#ifdef __cplusplus
}
#endif 

#endif

