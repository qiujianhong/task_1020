#ifndef _DATAFLASH_H
#define _DATAFLASH_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_DATAFLASH  "dataflash"

#define   DATAFLASH_LIB_PATH      "/lib/hal_lib/libhal_dataflash.so"  /*dataflash 动态库路径 */

/* 定义ADC模块结构 */
typedef struct tag_DATAFLASH_MODULE 
{
    HW_MODULE common;
}DATAFLASH_MODULE;

/*定义ADC设备结构 */
typedef struct tag_DATAFLASH_DEVICE
{
	HW_DEVICE	base;

	/**
	* @brief 写数据
	* @param[in] dev: 设备描述
	* @param[in] buffer: 数据缓存
	* @param[in] offest: 起始偏移地址
	* @param[in] length: 写入数据长度
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*dataflash_write)(struct tag_DATAFLASH_DEVICE *dev, uint8 *buffer, 
	uint32 offest, uint32 length);
	/**
	* @brief 读数据
	* @param[in] dev: 设备描述
	* @param[out] buffer: 缓冲区指针
	* @param[in] offest: 起始偏移地址
	* @param[in] length: 缓冲区长度
	* @return 成功返回读出的大于0的数据长度；失败返回FALSE
	*/
	int32 (*dataflash_read)(struct tag_DATAFLASH_DEVICE *dev, uint8* buffer, 
	uint32 offest, uint32 length);
	/**
	* @brief 擦除数据
	* @param[in] dev: 设备描述
	* @param[in] offest: 起始偏移地址
	* @param[in] length: 缓冲区长度
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*dataflash_erase)(struct tag_DATAFLASH_DEVICE *dev, uint32 offest, uint32 length);
	/**
	* @brief 获取可擦除块大小
	* @param[in] dev: 设备描述
	* @return 成功返回设备可擦除块大小；失败返回FALSE
	*/
	int32 (*dataflash_ebsize_get)(struct tag_DATAFLASH_DEVICE *dev);
	/**
	* @brief 获取设备大小
	* @param[in] dev: 设备描述
	* @return 成功返回设备大小；失败返回FALSE
	*/
	uint32 (*dataflash_devsize_get)(struct tag_DATAFLASH_DEVICE *dev);
	/**
	* @brief 设置芯片写保护
	* @param[in] dev: 设备描述
	* @param[in] wp_enable: 1: 打开写保护， 0: 关闭写保护
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*dataflash_write_protect_enable)(struct tag_DATAFLASH_DEVICE *dev, uint8 wp_enable);
	// 用户扩展部分	
}DATAFLASH_DEVICE_T;

#ifdef __cplusplus
}
#endif 


#endif

