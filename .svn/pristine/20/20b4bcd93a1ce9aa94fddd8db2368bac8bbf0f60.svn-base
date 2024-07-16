#ifndef _KEY_H
#define _KEY_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_KEY  "key"
#define   KEY_LIB_PATH      "/lib/hal_lib/libhal_key.so"   /*key 动态库路径 */

/* key press */
#define KEY_ESC             1
#define KEY_ENTER 		    28
#define KEY_UP 				103
#define KEY_LEFT            105
#define KEY_RIGHT           106
#define KEY_DOWN            108

typedef struct tag_KEY_VALUE
{
    uint8 key;    //标准按键
    uint8 status;  //按键状态   0按下1  弹起
}KEY_VALUE_T;

/* 定义KEY模块结构 */
typedef struct tag_KEY_MODULE 
{
    HW_MODULE common;
}KEY_MODULE;

/*定义KEY设备结构 */
typedef struct tag_KEY_DEVICE
{
	HW_DEVICE	base;
    
	/**
	* @brief 读取最近一次键值
	* @param[in] dev: 设备描述
	* @param[in] size: 获取键值的最大缓冲区长度， 单位为字节
	* @param[out] key_val_list: 键值列表（采用linux内核定义的标准按键值）
	* @note 按键设计遵循读后清原则， 驱动接口只获取单一按键状态值， 
			 长按键或组合按键状态由平台基础库提供。
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*key_value_get)(struct tag_KEY_DEVICE *dev, struct tag_KEY_VALUE *key_val_list,
	uint32 size);


}KEY_DEVICE_T;

#ifdef __cplusplus
}
#endif 


#endif

