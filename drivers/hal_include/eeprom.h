#ifndef _EEPROM_H
#define _EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif 

/*
铁电定义：
前128（0-127）: 0-7存储MAC地址及其校验；16-47存储地区；50存储产品（0-无,1-集中器,2-ECU,3-专3,4-专1） 81储存地区（P-江西， 0-其他地区） 85-95储存停电时间和停电时长


后4096（4096-尾）：存储许可
*/

#include <hal.h>

#define   HW_DEVICE_ID_EEPROM    "eeprom"
#define   EEPROM_LIB_PATH        "/lib/hal_lib/libhal_eeprom.so"   /*eeprom动态库路径 */

/* 定义EEPROM模块结构 */
typedef struct tag_EEPROM_MODULE 
{
    HW_MODULE common;
}EEPROM_MODULE;

/*定义EEPROM设备结构 */
typedef struct tag_EEPROM_DEVICE
{
    HW_DEVICE    base;
    /**
    * @brief 写数据
    * @param[in] dev: 设备描述
    * @param[in] buffer: 数据缓存
    * @param[in] offest: 起始偏移地址
    * @param[in] length: 写入数据长度
    * @return 成功返回TRUE；失败返回FALSE
    */
    int (*eeprom_write)(struct tag_EEPROM_DEVICE *dev, uint8* buffer, uint32 offest, uint32 length);
    
    /**
    * @brief 读数据
    * @param[in] dev: 设备描述
    * @param[out] buffer: 缓冲区指针
    * @param[in] offest: 起始偏移地址
    * @param[in] length: 缓冲区长度
    * @return 成功返回读出的大于0的数据长度；失败返回FALSE
    */
    int (*eeprom_read)(struct tag_EEPROM_DEVICE *dev, uint8* buffer,  uint32 offest, uint32 length);

    /**
    * @brief 设置芯片写保护
    * @param[in] dev: 设备描述
    * @param[in] wp_enable: 1: 打开写保护， 0: 关闭写保护
    * @return 成功返回TRUE；失败返回FALSE
    */
    int (*eeprom_write_protect_enable)(struct tag_EEPROM_DEVICE *dev, uint8 wp_enable); 

    /**
    * @brief 获取设备大小
    * @param[in] dev: 设备描述
    * @return 成功返回设备大小；失败返回FALSE
    */
    int (*eeprom_devsize_get)(struct tag_EEPROM_DEVICE *dev);

}EEPROM_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

