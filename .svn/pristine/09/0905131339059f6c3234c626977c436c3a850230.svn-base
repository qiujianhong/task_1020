#ifndef _AUDIO_H
#define _AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_AUDIO  "audio"
#define   AUDIO_LIB_PATH      "/lib/hal_lib/libhal_audio.so"  /*audio 动态库路径 */

typedef enum
{
    EMAUDIO_INDEX_0 = 0,  //关闭音源输出
    EMAUDIO_INDEX_1,      //运行参数已更改，请注意查看
    EMAUDIO_INDEX_2,      //最新信息，请查看 
    EMAUDIO_INDEX_3,      //时段控已投入 
    EMAUDIO_INDEX_4,      //时段控已解除 
    EMAUDIO_INDEX_5,      //厂休控已投入 
    EMAUDIO_INDEX_6,      //厂休控已解除 
    EMAUDIO_INDEX_7,      //营业报停控投入 
    EMAUDIO_INDEX_8,      //营业报停控解除 
    EMAUDIO_INDEX_9,      //月电控已投入 
    EMAUDIO_INDEX_10,     //月电控已解除
    EMAUDIO_INDEX_11,     //购电控已投入 
    EMAUDIO_INDEX_12,     //购电控已解除 
    EMAUDIO_INDEX_13,     //当前功率下浮控已投入 
    EMAUDIO_INDEX_14,     //当前功率下浮控已解除 
    EMAUDIO_INDEX_15,     //遥控跳闸 
    EMAUDIO_INDEX_16,     //允许合闸 
    EMAUDIO_INDEX_17,     //已超负荷，请限电 
    EMAUDIO_INDEX_18,     //电量快用完，请速购电 
    EMAUDIO_INDEX_19,     //电量用完，已跳闸 
    EMAUDIO_INDEX_20,     //嘀(报警音,1秒) 
    EMAUDIO_INDEX_21,     //请用户及时结算电费
    EMAUDIO_INDEX_22,     //工控跳闸
    EMAUDIO_INDEX_23,     //灯控跳闸
    EMAUDIO_INDEX_24,     //滴 滴 滴
}AUDIO_INDEX_E;

/* 定义AUDIO模块结构 */
typedef struct tag_AUDIO_MODULE 
{
    HW_MODULE common;
}AUDIO_MODULE;

/*定义AUDIO设备结构 */
typedef struct tag_AUDIO_DEVICE
{
	HW_DEVICE	base;

	/**
	* @brief 控制音频播放设备
	* @param[in] dev: 设备描述
	* @param[in] state: 0 禁止播放,1 使能播放
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*audio_state_set)(struct tag_AUDIO_DEVICE *dev, uint32 state);
	/**
	* @brief 播放第N段音频
	* @param[in] dev: 设备描述
	* @param[in] index: 音频编号
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*audio_play)(struct tag_AUDIO_DEVICE *dev, AUDIO_INDEX_E index);
	/**
	* @brief 蜂鸣器输出
	* @param[in] dev: 设备描述
	* @param[in] val: 0： 关闭， 1： 开启
	* @param[in] frq: 输出频率（单位: hz）
	* @param[in] count: 次数
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*audio_set_buzzer_out)(struct tag_AUDIO_DEVICE *dev, uint8 val, uint32 frq, uint32 count);	
	// 用户扩展部分	
}AUDIO_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

