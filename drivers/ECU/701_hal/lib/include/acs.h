#ifndef _ACS_H
#define _ACS_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <hal.h>

#define   HW_DEVICE_ID_ACS  "acs"
#define   ACS_LIB_PATH      "/lib/hal_lib/libhal_acs.so"   /* */

/* 定义ACS模块结构 */
typedef struct tag_ACS_MODULE 
{
    HW_MODULE common;
}ACS_MODULE;

/*定义PULSE设备结构 */
typedef struct tag_ACS_DEVICE
{
	HW_DEVICE	base;

/**
* @brief        交采报文处理
* @param[in]     input_data: 报文数据
* @param[in]     input_len: 报文数据长度
* @param[out]    output_data: 应答报文缓冲区
* @param[in]     output_len: 应答报文缓冲区长度
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 (*acs_data_transfer)(struct tag_ACS_DEVICE *dev, uint8 *input_data, int32 input_len, uint8 *output_data, int32 output_len);

	// 用户扩展部分	
}ACS_DEVICE_T;

#ifdef __cplusplus
}
#endif 

#endif

