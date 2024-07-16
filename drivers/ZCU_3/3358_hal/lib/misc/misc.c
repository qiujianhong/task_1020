#include <misc.h>
#include "sysDefine.h"

/*misc 设备路径 */
#define   MISC_DEV_NAME      "/dev/misc" 

#define MISC_DEV_NUM      1
static char *aMiscDevice[MISC_DEV_NUM] = {HW_DEVICE_ID_MISC};

/**
* @brief 获取文件系统分区状态
* @param[in] dev: 设备描述
* @param[in] fs_type: 文件系统类型（如ubifs/jiffs2等）
* @param[in] partition_idx:分区索引编号
* @return 成功返回TRUE；失败返回FALSE
*/
int32 misc_fs_partition_state_get(struct tag_MISC_DEVICE *dev, int32 fs_type, int32 partition_idx)
{
  return ERR_OK;
}
/**
* @brief 设置pwm开启/关闭
* @param[in] dev: 设备描述
* @param[in] val: 开启/关闭设置值
* @return 成功返回TRUE；失败返回FALSE
*/
int32 misc_pwm_state_set(struct tag_MISC_DEVICE *dev, int32 val)
{
  return ERR_OK;
}

/**
* @brief 设置pwm参数
* @param[in] dev: 设备描述
* @param[in] frq: pwm频率
* @param[in] duty: pwm占空比
* @return 成功返回TRUE；失败返回FALSE
*/
int32 misc_pwm_param_set(struct tag_MISC_DEVICE *dev, uint32 frq, uint32 duty)
{
  return ERR_OK;
}
/**
* @brief 获取pwm参数
* @param[in] dev: 设备描述
* @param[out] frq: pwm频率
* @param[out] duty: pwm占空比
* @return 成功返回TRUE；失败返回FALSE
*/
int32 misc_pwm_param_get(struct tag_MISC_DEVICE *dev, uint32 *frq, uint32 *duty)
{
  return ERR_OK;
}

/**        
* @brief 获取USB插入信息
* @param[in] dev: 设备描述
* @param[out] info: 获取信息
* @return 成功返回TRUE；失败返回FALSE
*/
int32 misc_usb_info_get(struct tag_MISC_DEVICE *dev, uint32 *info)
{
  return ERR_OK;
}

/**
* @brief sim卡加热功能
* @param[in] dev: 设备描述
* @param[in] val: 0： 停止加热， 1： 启动加热
* @return 成功返回TRUE；失败返回FALSE
*/
int32 misc_sim_heater_set(struct tag_MISC_DEVICE *dev, uint8 val)
{
  return ERR_OK;
}
// 用户扩展部分  


static int misc_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    MISC_DEVICE_T* dev = (MISC_DEVICE_T*)malloc(sizeof(MISC_DEVICE_T));
    if(dev == NULL)
    {
        return -1;
    }
    memset(dev, 0, sizeof(MISC_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_MISC;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->misc_fs_partition_state_get = misc_fs_partition_state_get;
    dev->misc_usb_info_get = misc_usb_info_get;
    dev->misc_pwm_param_get = misc_pwm_param_get;
    dev->misc_pwm_state_set = misc_pwm_state_set;
    dev->misc_usb_info_get = misc_usb_info_get;
    dev->misc_sim_heater_set = misc_sim_heater_set;
    
    
    *device = (HW_DEVICE*)dev;
    
    return 0;
}

static int misc_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    MISC_DEVICE_T *dev = (MISC_DEVICE_T*)pDevice;
    
    free(dev);
    return 0;
}

/* 初始化模块变量 */
MISC_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "misc",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2018-10-18",
        .nDevNum = MISC_DEV_NUM,
        .szDevices = aMiscDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = misc_open,
        .pfClose = misc_close,
    },
};
