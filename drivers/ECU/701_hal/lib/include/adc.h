#ifndef _ADC_H
#define _ADC_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <time.h>
#include <sys/time.h>
#include <string.h>


#define   HW_DEVICE_ID_ADC  "adc"
#define   ADC_LIB_PATH      "/lib/hal_lib/libhal_adc.so"    /*adc 动态库路径 */

typedef enum
{
    EMADC_RTCVOL_CHANNEL = 0,  /*时钟电池ADC通道号 */
    EMADC_BATVOL_CHANNEL,      /*备用电池ADC通道号 */
    EMADC_TEMP_CHANNEL,        /*温度传感器ADC通道号 单位 0.1°*/
    EMADC_CAPVOL_CHANNEL,     /* 超级电容电压 */
    EMADC_MAX_CHANNEL = 255,   /*ADC通道号最大限定值 */
}ADC_CHANNEL_E;

/* 定义ADC模块结构 */
typedef struct tag_ADC_MODULE 
{
    HW_MODULE common;
}ADC_MODULE;

/*定义ADC设备结构 */
typedef struct tag_ADC_DEVICE
{
	HW_DEVICE	base;
   /**
	* @brief 获取指定ADC通道数据
	* @param[in] dev: 设备描述
	* @param[in] index: ADC通道序号（0~max）
	* @param[out] data: ADC通道数据（单位：mv）
	* @note pData返回转换之后的模拟电压值， 单位为mv， 不能直接返回
	  ADC采样的寄存器数字量
	* @return 成功返回TRUE；失败返回FALSE
	*/
	int32 (*adc_volt_read)(struct tag_ADC_DEVICE *dev, ADC_CHANNEL_E index, int32 *data);
	// 用户扩展部分	

}ADC_DEVICE_T;


#ifdef __cplusplus
}
#endif 

#endif

