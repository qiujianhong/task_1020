#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>

#include "types.h"
#include <can.h>
#include "sysDefine.h"

/*watchdog 设备路径 */
#define CAN0_DEV_NAME              "/dev/ttyCan0"
#define CAN1_DEV_NAME              "/dev/ttyCan1"

#define CAN_DEV_NUM          1
static char *aCanDevice[CAN_DEV_NUM] = {CAN0_DEVICE_ID_STR};

/**
* @brief 设置CAN通信参数
* @param[in] dev: 设备描述
* @param[in] baud 波特率
* @return成功返回ERR_OK；失败返回错误码。详见：附录。
*/
int32 can_param_set(struct tag_CAN_DEVICE *dev, uint32 baud)
{
    return ERR_OK;
}

/**
* @brief 设置CANid参数
* @param[in] dev: 设备描述
* @param[in] can_id 发送帧id
* @return成功返回ERR_OK；失败返回错误码。详见：附录。
*/
int32 can_id_set(struct tag_CAN_DEVICE *dev, canid_t can_id)
{
    return ERR_OK;
}

/**
* @brief 从CAN接收数据
* @param[in] dev: 设备描述
* @param[out] buf 接收缓存区
* @param[in] len 缓存区长度
* @return 成功返回大于0的数据长度，失败返回错误码。详见：附录。
*/
int32 can_data_recv(struct tag_CAN_DEVICE *dev, uint8* buf, uint32 len)
{
    return ERR_OK;
}

/**
* @brief 通过CAN发送数据
* @param[in] dev: 设备描述
* @param[in] buf 发送缓存区
* @param[in] len 缓存区长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录。
*/
int32 can_data_send(struct tag_CAN_DEVICE *dev, const uint8* buf, int32 len)
{
    return ERR_OK;
}

/**
* @brief 获取CAN句柄
* @param[in] dev: 设备描述
* @return返回CAN打开时的句柄值（正常>0，异常<=0）。
*/
int32 can_get_handle(struct tag_CAN_DEVICE *dev)
{
    return ERR_OK;
}


static int can_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    CAN_DEVICE_T* dev = (CAN_DEVICE_T*)malloc(sizeof(CAN_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(CAN_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->can_param_set = can_param_set;
    dev->can_id_set = can_id_set;
    dev->can_data_recv = can_data_recv;
    dev->can_data_send = can_data_send;
    dev->can_get_handle = can_get_handle;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int can_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    CAN_DEVICE_T *dev = (CAN_DEVICE_T*)pDevice;
    if(dev == NULL)
    {
        return ERR_PTR;
    }

    if (dev->fd > 0)
    {
        close(dev->fd);
        dev->fd = 0;
    }
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
CAN_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "can",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-05-24",
        .nDevNum  = CAN_DEV_NUM,
        .szDevices = aCanDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = can_open,
        .pfClose = can_close,
    },
};
