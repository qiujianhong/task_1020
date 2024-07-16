#include "types.h"
#include <status_signal.h>
#include "sysDefine.h"

#define STATUS_SIGNAL_DEV_NUM          1
static char *aStatusSignalDevice[STATUS_SIGNAL_DEV_NUM] = {STATUS_SIGNAL_DEVICE_ID_STR};


/**
* @brief 获取门节点、上盖触点等状态信号数据
* @param[in] dev: 设备描述
* @param[out] status: 状态数据，按位表示；bit0-门节点，bit1-上盖触点，bit2-端尾触点，其他保留；0-分，1-合
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 status_singal_get(struct tag_STATUS_SIGNAL_DEVICE *dev, uint32 *status)
{
    uint8  newstatus = 0;
    uint32 data1 = 1;
    uint32 data2 = 2;
    uint32 data3 = 3;
    uint32 data4 = 4;
    uint32 data5 = 5;
    int bReadCount = 0;
    int fd = -1;

    if(NULL == dev || NULL == status)
    {
        return ERR_PTR;
    }

    fd = open("/dev/ym", O_RDONLY|O_NONBLOCK);
    if (fd <= 0)
    {
        return ERR_NOTEXIST;
    }
    
    while(bReadCount++ < 10)
    {
        read(fd, &data1, sizeof(data1));
        data1 &= 0x00F0;
        usleep(1000);
        
        read(fd, &data2, sizeof(data2));
        data2 &= 0x00F0;
        usleep(1000);
        
        read(fd, &data3, sizeof(data3));
        data3 &= 0x00F0;
        usleep(1000);
        
        read(fd, &data4, sizeof(data4));
        data4 &= 0x00F0;
        usleep(1000);
        
        read(fd, &data5, sizeof(data5));
        data5 &= 0x00F0;
        
        //连续5次读到相同的值，防止干扰
        if(data1 == data2 && data1 == data3 && data1 == data4 && data1 == data5)
        {
            newstatus = 1 - ((data1 >> 4) & 0x0001);
            break;
        }
    }
    close(fd);
    if(bReadCount >= 10)
    {
        return ERR_OK;
    }

    *status = newstatus;
    return ERR_OK;
}

static int status_signal_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    STATUS_SIGNAL_DEVICE_T* dev = (STATUS_SIGNAL_DEVICE_T*)malloc(sizeof(STATUS_SIGNAL_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(STATUS_SIGNAL_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->status_singal_get = status_singal_get;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int status_signal_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    STATUS_SIGNAL_DEVICE_T *dev = (STATUS_SIGNAL_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
STATUS_SIGNAL_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "status_signal",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-06-21",
        .nDevNum  = STATUS_SIGNAL_DEV_NUM,
        .szDevices = aStatusSignalDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = status_signal_open,
        .pfClose = status_signal_close,
    },
};
