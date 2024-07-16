#include <bluetooth.h>
#include "sysDefine.h"

/*pulse 设备路径 */
#define   BLUETOOTH_DEV_NAME      "/dev/ttyS5" 

static char *aBTDevice[BT_DEVICE_NUM] = {HW_DEVICE_ID_BT};


/**
* @brief       蓝牙数据发送
* @param[in]     data: 数据内容
* @param[in]     len: 发送数据长度
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 bt_send(struct tag_BT_DEVICE *dev, uint8 *data, int32 len)
{
   return ERR_OK;
}

/**
* @brief       蓝牙数据接收
* @param[out]  data: 数据内容
* @param[in]   len:缓冲区长度
* @return      成功返回数据长度；失败返回错误码。详见：附录1。
*/
int32 bt_recv(struct tag_BT_DEVICE *dev, uint8 *data, int32 len)
{

   return ERR_OK;
}

/**
* @brief       蓝牙模组软复位
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 bt_reset(struct tag_BT_DEVICE *dev)
{
  return ERR_OK;
}

/**
* @brief        蓝牙参数设置
* @param[in]    id: 信息ID
* @param[in]    data: 设置参数内容
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 bt_set_param(struct tag_BT_DEVICE *dev, HAL_BTINFO_E id, void* data)
{
   return ERR_OK;
}

/**
* @brief        蓝牙读取信息
* @param[in]    id: 信息ID
* @param[in]    len: 缓冲区最大长度
* @param[out]   info: 信息内容
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
int32 bt_get_param(struct tag_BT_DEVICE *dev, HAL_BTINFO_E id, void* info, int32 len)
{
   return ERR_OK;
}



/**
* @brief        读取蓝牙连接状态
* @param[in] 	dev: 设备描述
* @return      成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
bool bt_is_connect(struct tag_BT_DEVICE *dev)
{
   return ERR_OK;
}


static int bluetooth_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    BT_DEVICE_T* dev = (BT_DEVICE_T*)malloc(sizeof(BT_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
	
    memset(dev, 0, sizeof(BT_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_BT;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->bt_send = bt_send;
	dev->bt_recv = bt_recv;
    dev->bt_reset = bt_reset;  
	dev->bt_set_param = bt_set_param;
    dev->bt_get_param = bt_get_param;
	dev->bt_is_connect = bt_is_connect;
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int bluetooth_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    BT_DEVICE_T *dev = (BT_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
BT_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "bluetooth",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-04-12",
        .nDevNum  = BT_DEVICE_NUM,
        .szDevices = aBTDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = bluetooth_open,
        .pfClose = bluetooth_close,
    },
};
