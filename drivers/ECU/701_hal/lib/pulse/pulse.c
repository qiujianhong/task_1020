#include <pulse.h>

/*pulse 设备路径 */
#define   PULSE_DEV_NAME      "/dev/s3c2410_ym" 


/**
* @brief 读取某一路脉冲数
* @param[in] dev: 设备描述
* @param[in] channel: 选择第几路脉冲（0~max）
* @param[out] pulse_count: 该路脉冲数
* @return 成功返回读出的大于0的数据长度；失败返回FALSE
*/
int32 pulse_count_read(struct tag_PULSE_DEVICE *dev, uint32 channel,
      uint32 *pulse_count)
{
  return 0;

}
	  
/**
* @brief 读取某一路脉冲1分钟内的实际耗时和脉冲数
* @param[in] dev: 设备描述
* @param[in] channel: 选择第几路脉冲（0~max）
* @param[out] pulse_count_per_minute: 该路脉冲数
* @param[out] real_pulse_time_per_minute: 该路脉冲耗时（单位：mv）
* @return 成功返回TRUE；失败返回FALSE
*/
int32 pulse_real_time_and_count_per_minute_read(struct tag_PULSE_DEVICE *dev,
uint32 channel, uint32 *pulse_count_per_minute, uint32 *real_pulse_time_per_minute)
{
  return 0;
}

/**
* @brief 配置某一路脉宽
* @param[in] dev: 设备描述
* @param[in] channel: 选择第几路脉冲（0~max）
* @param[in] time: 脉宽，单位ms
* @return 成功返回TRUE；失败返回FALSE
*/
int32 pulse_config_time(struct tag_PULSE_DEVICE *dev, uint32 channel, uint32 time)
{
  return 0;
}

static int pulse_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    PULSE_DEVICE_T* dev = (PULSE_DEVICE_T*)malloc(sizeof(PULSE_DEVICE_T));
    if(dev == NULL)
    {
        return -1;
    }
	
    memset(dev, 0, sizeof(PULSE_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_PULSE;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->pulse_config_time = pulse_config_time;
	dev->pulse_count_read = pulse_count_read;
    dev->pulse_real_time_and_count_per_minute_read = pulse_real_time_and_count_per_minute_read;
    
    *device = (HW_DEVICE*)dev;
    
    return 0;
}

static int pulse_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    PULSE_DEVICE_T *dev = (PULSE_DEVICE_T*)pDevice;
    
    free(dev);
    return 0;
}

/* 初始化模块变量 */
PULSE_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "pulse",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2018-10-18",
        .szDevices = {HW_DEVICE_ID_PULSE},
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = pulse_open,
        .pfClose = pulse_close,
    },
};
