#include "types.h"
#include <local_cu.h>
#include "sysDefine.h"

/*watchdog 设备路径 */
#define LOCAL_CU_SCMDEV_NAME      "/dev/tty_io" 
#define LOCAL_CU_DEV_NUM          1
static char *aLocalCUDevice[LOCAL_CU_DEV_NUM] = {HW_DEVICE_ID_LOCAL_CU};

#define LOCAL_CU_GET_MODSTATE    0x6506          //模块插入识别
#define LOCAL_CU_CLEAR_RTS       0X6502          //复位脚拉低复位模块
#define LOCAL_CU_SET_RTS         0X6501          //复位脚拉高

/***************************************************
* @brief 设置本地通信单元电源
* @param[in] dev: 设备描述
* @param[in] val: 0：关闭， 1：开启
* @return成功返回ERR_OK；失败返回错误码。
****************************************************/
int local_cu_power_set(struct tag_LOCAL_CU_DEVICE *dev, uint8 val)
{
    int fd = 0;

    if(NULL == dev)
    {
        return ERR_PTR;
    }
    
    fd = open(LOCAL_CU_SCMDEV_NAME, O_RDONLY);
    if(fd <= 0)
    {
        return ERR_NOTEXIST;
    }
    if(val ==0 )
    {
        ioctl(fd, LOCAL_CU_CLEAR_RTS, 5);

    }else if(val ==1)
    {
        ioctl(fd, LOCAL_CU_SET_RTS, 5);
    }else
        return ERR_O_RANGE;
    close(fd);
    return ERR_OK;
}

/***************************************************
* @brief 复位本地通信单元
* @param[in] dev: 设备描述
* @return成功返回ERR_OK；失败返回错误码。
****************************************************/
int local_cu_reset(struct tag_LOCAL_CU_DEVICE *dev)
{
    int fd = 0;

    if(NULL == dev)
    {
        return ERR_PTR;
    }
    
    fd = open(LOCAL_CU_SCMDEV_NAME, O_RDONLY);
    if(fd <= 0)
    {
        return ERR_NOTEXIST;
    }

    ioctl(fd, LOCAL_CU_CLEAR_RTS, 5);
    sleep(2);
    ioctl(fd, LOCAL_CU_SET_RTS, 5);
    sleep(2);
    close(fd);

    return ERR_OK;
}

/***************************************************
* @brief 获取本地通信模块单元插拔状态
* @param[in] dev: 设备描述
* @param[out] state: 插拔状态，1插入，0 拔出
* @return成功返回ERR_OK；失败返回错误码。
****************************************************/
int local_cu_insert_state_get(struct tag_LOCAL_CU_DEVICE *dev, int *state)
{
    int fd = 0;
    int ret = 0;
    if(NULL == dev || NULL == state)
    {
        return ERR_PTR;
    }
    
    fd = open(LOCAL_CU_SCMDEV_NAME, O_RDONLY);
    if (fd <= 0)
    {
        return ERR_NOTEXIST;
    }

    ret = ioctl(fd, LOCAL_CU_GET_MODSTATE, 5);
    *state = 0 == ret ? 1 : 0;
    
    close(fd);
    return ERR_OK;
}


static int local_cu_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    LOCAL_CU_DEVICE_T* dev = (LOCAL_CU_DEVICE_T*)malloc(sizeof(LOCAL_CU_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(LOCAL_CU_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->local_cu_power_set = local_cu_power_set;
    dev->local_cu_reset = local_cu_reset;
    dev->local_cu_insert_state_get = local_cu_insert_state_get;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int local_cu_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    LOCAL_CU_DEVICE_T *dev = (LOCAL_CU_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
LOCAL_CU_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "local_cu",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-05-24",
        .nDevNum  = LOCAL_CU_DEV_NUM,
        .szDevices = aLocalCUDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = local_cu_open,
        .pfClose = local_cu_close,
    },
};
