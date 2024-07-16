#include "types.h"
#include <acs.h>
#include "sysDefine.h"

/*acs 设备路径 */
#define   ACS_DEV_NAME      "/dev/acs0" 

#define ACS_DEV_NUM      1

static char *aAcsDevice[ACS_DEV_NUM] = {HW_DEVICE_ID_ACS};

/**
* @brief        交采报文处理
* @param[in]     input_data: 报文数据
* @param[in]     input_len: 报文数据长度
* @param[out]    output_data: 应答报文缓冲区
* @param[in]     output_len: 应答报文缓冲区长度
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 acs_data_transfer(struct tag_ACS_DEVICE *dev, uint8 *input_data, int32 input_len, uint8 *output_data, int32 output_len)
{
    int32 iRet = ERR_NORMAL;

    return iRet;
	
}
int32 (*acs_data_transfer)(struct tag_ACS_DEVICE *dev, uint8 *input_data, int32 input_len, uint8 *output_data, int32 output_len);




static int32 acs_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    ACS_DEVICE_T* dev = (ACS_DEVICE_T*)malloc(sizeof(ACS_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_PTR;
    }
	
    memset(dev, 0, sizeof(ACS_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_ACS;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->acs_data_transfer = acs_data_transfer;
   
    *device = (HW_DEVICE*)dev;
    
    return 0;
}

static int acs_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    ACS_DEVICE_T *dev = (ACS_DEVICE_T*)pDevice;
    
    free(dev);
    return 0;
}

/* 初始化模块变量 */
ACS_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "acs",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-18",
        .nDevNum = ACS_DEV_NUM,
        .szDevices = aAcsDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = acs_open,
        .pfClose = acs_close,
    },
};
