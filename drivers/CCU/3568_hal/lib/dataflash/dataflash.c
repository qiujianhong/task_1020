#include "types.h"
#include <dataflash.h>
#include "sysDefine.h"

/*dataflash 设备路径 */
#define   DATAFLASH_DEV_NAME   "/dev/dataflash" 

#define DF_DEV_NUM      1

static char *aDFDevice[DF_DEV_NUM] = {HW_DEVICE_ID_DATAFLASH};

/**
* @brief 写数据
* @param[in] dev: 设备描述
* @param[in] buffer: 数据缓存
* @param[in] offest: 起始偏移地址
* @param[in] length: 写入数据长度
* @return 成功返回TRUE；失败返回FALSE
*/
int32 dataflash_write(struct tag_DATAFLASH_DEVICE *dev, uint8 *buffer, 
uint32 offest, uint32 length)
{
  return ERR_OK;
}
/**
* @brief 读数据
* @param[in] dev: 设备描述
* @param[out] buffer: 缓冲区指针
* @param[in] offest: 起始偏移地址
* @param[in] length: 缓冲区长度
* @return 成功返回读出的大于0的数据长度；失败返回FALSE
*/
int32 dataflash_read(struct tag_DATAFLASH_DEVICE *dev, uint8* buffer, 
uint32 offest, uint32 length)
{
  return ERR_OK;
}
/**
* @brief 擦除数据
* @param[in] dev: 设备描述
* @param[in] offest: 起始偏移地址
* @param[in] length: 缓冲区长度
* @return 成功返回TRUE；失败返回FALSE
*/
int32 dataflash_erase(struct tag_DATAFLASH_DEVICE *dev, uint32 offest, uint32 length)
{
  return ERR_OK;
}

/**
* @brief 获取可擦除块大小
* @param[in] dev: 设备描述
* @return 成功返回设备可擦除块大小；失败返回FALSE
*/
int32 dataflash_ebsize_get(struct tag_DATAFLASH_DEVICE *dev)
{
  return ERR_OK;

}

/**
* @brief 获取设备大小
* @param[in] dev: 设备描述
* @return 成功返回设备大小；失败返回FALSE
*/
uint32 dataflash_devsize_get(struct tag_DATAFLASH_DEVICE *dev)
{
   return ERR_OK;

}


/**
* @brief 设置芯片写保护
* @param[in] dev: 设备描述
* @param[in] wp_enable: 1: 打开写保护， 0: 关闭写保护
* @return 成功返回TRUE；失败返回FALSE
*/
int32 dataflash_write_protect_enable(struct tag_DATAFLASH_DEVICE *dev, uint8 wp_enable)
{
  return ERR_OK;
}

static int dataflash_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    DATAFLASH_DEVICE_T* dev = (DATAFLASH_DEVICE_T*)malloc(sizeof(DATAFLASH_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(DATAFLASH_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_DATAFLASH;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->dataflash_write = dataflash_write;
    dev->dataflash_read = dataflash_read;
    dev->dataflash_erase = dataflash_erase;
    dev->dataflash_ebsize_get = dataflash_ebsize_get;
    dev->dataflash_devsize_get = dataflash_devsize_get;
    dev->dataflash_write_protect_enable = dataflash_write_protect_enable;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int dataflash_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    DATAFLASH_DEVICE_T *dev = (DATAFLASH_DEVICE_T*)pDevice;
    
    free(dev);
    return 0;
}

/* 初始化模块变量 */
DATAFLASH_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "dataflash",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-20",
        .nDevNum = DF_DEV_NUM,
        .szDevices = aDFDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = dataflash_open,
        .pfClose = dataflash_close,
    },
};
