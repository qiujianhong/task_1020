#include "types.h"
#include <remote_cu.h>
#include "sysDefine.h"

/*watchdog 设备路径 */
#define REMOTE_CU_SCMDEV_NAME      "/dev/gprs_io"
#define REMOTE_CU_INSERT_NAME      "/dev/ym"
#define REMOTEL_CU_DEV_NUM          1
static char *aRemoteCUDevice[REMOTEL_CU_DEV_NUM] = {HW_DEVICE_ID_REMOTE_CU};

//模块复位
#define REMOTE_CU_RESTART_ARG    0
//模块电源
#define REMOTE_CU_POWER_ARG      1
//模块开/关机
#define REMOTE_CU_ONOFF_ARG      2

//模块复位/关闭
#define REMOTE_CU_CLOSE_CMD      0
//模块正常/打开
#define REMOTE_CU_OPEN_CMD       1

/***************************************************
* @brief 设置远程通信单元电源
* @param[in] dev: 设备描述
* @param[in] val: 0：关闭， 1：开启
* @return成功返回ERR_OK；失败返回错误码。
****************************************************/
int32 remote_cu_power_set(struct tag_REMOTE_CU_DEVICE *dev, uint8 val)
{
    int fd = 0;
    int ret = 0;
    
    if(NULL == dev || val > 1)
    {
        return ERR_PTR;
    }

    fd = open(REMOTE_CU_SCMDEV_NAME, O_RDWR | O_NONBLOCK);
    if(fd < 0)
    {
        return ERR_NOTEXIST;
    }
    ret = ioctl(fd, val, REMOTE_CU_POWER_ARG);
    close(fd);

    return -1 == ret ? ERR_NORMAL : ERR_OK;
}

/***************************************************
* @brief 复位远程通信单元
* @param[in] dev: 设备描述
* @return成功返回ERR_OK；失败返回错误码。
****************************************************/
int32 remote_cu_reset(struct tag_REMOTE_CU_DEVICE *dev)
{
    int fd = 0;

    if(NULL == dev)
    {
        return ERR_PTR;
    }

    fd = open(REMOTE_CU_SCMDEV_NAME, O_RDWR | O_NONBLOCK);
    if(fd < 0)
    {
        return ERR_NOTEXIST;
    }

    //复位信道置高
    if(ioctl(fd, REMOTE_CU_OPEN_CMD, REMOTE_CU_RESTART_ARG) == -1)
    {
        close(fd);
        return ERR_NORMAL;
    }
    usleep(500000);//复位时间持续500ms

    //复位信道置低
    if(ioctl(fd, REMOTE_CU_CLOSE_CMD, REMOTE_CU_RESTART_ARG) == -1)/*复位*/
    {
        close(fd);
        return ERR_NORMAL;
    }
    sleep(1);//延时1s

    //复位信道置高
    if(ioctl(fd, REMOTE_CU_OPEN_CMD, REMOTE_CU_RESTART_ARG) == -1)
    {
        close(fd);
        return ERR_NORMAL;
    }
    close(fd);
    
    sleep(5); //等待5s
    return 0;
}

/***************************************************
* @brief 设置远程通信单元开机电平
* @param[in] dev: 设备描述
* @param[in] val: 0：开机电平置低， 1：开机电平置高
* @return成功返回ERR_OK；失败返回错误码。
****************************************************/
int32 remote_cu_on_level_set(struct tag_REMOTE_CU_DEVICE *dev, uint8 val)
{
    int fd = 0;
    int ret = 0;
    
    if(NULL == dev || val > 1)
    {
        return ERR_PTR;
    }

    fd = open(REMOTE_CU_SCMDEV_NAME, O_RDWR | O_NONBLOCK);
    if(fd < 0)
    {
        return ERR_NOTEXIST;
    }
    ret = ioctl(fd, val, REMOTE_CU_ONOFF_ARG);
    close(fd);

    return -1 == ret ? ERR_NORMAL : ERR_OK;
}

/***************************************************
* @brief 获取远程通信模块单元插拔状态
* @param[in] dev: 设备描述
* @param[out] state: 插拔状态，0 未插入，1 有模块且响应AT指令，2 有模块不响应AT指令
* @return成功返回ERR_OK；失败返回错误码。
****************************************************/
int32 remote_cu_insert_state_get(struct tag_REMOTE_CU_DEVICE *dev, int32 *state)
{
    uint8  type = 0;
    uint32 data1 = 1;
    uint32 data2 = 2;
    uint32 data3 = 3;
    uint32 data4 = 4;
    uint32 data5 = 5;
    int bReadCount = 0;
    int fd = -1;

    if(NULL == dev || NULL == state)
    {
        return ERR_PTR;
    }

    *state = 0;
    fd = open(REMOTE_CU_INSERT_NAME, O_RDONLY|O_NONBLOCK);
    if (fd <= 0)
    {
        return ERR_NOTEXIST;
    }
    
    while(bReadCount++ < 10)
    {
        read(fd, &data1, sizeof(data1));
        usleep(1000);
        
        read(fd, &data2, sizeof(data2));
        usleep(1000);
        
        read(fd, &data3, sizeof(data3));
        usleep(1000);
        
        read(fd, &data4, sizeof(data4));
        usleep(1000);
        
        read(fd, &data5, sizeof(data5));
        //连续5次读到相同的值，防止干扰
        if(data1 == data2 && data1 == data3 && data1 == data4 && data1 == data5)
        {
            type = ((data1 >> 16) & 0x0001);
            break;
        }
    }
    close(fd);

    if((type & 0x07) == 0x07)
    {
        *state = 0;
    }
    else if((type & 0x07) == 0x06)
    {
        *state = 1;
    }
    else
    {
        *state = 2;
    }
    return ERR_OK;
}

/***************************************************
* @brief远程通信模块SIM卡加热
* @param[in] dev: 设备描述
* @param[in] val: 0：关闭， 1：开启
* @return成功返回ERR_OK；失败返回错误码。
****************************************************/
int32 remote_cu_sim_card_heat(struct tag_REMOTE_CU_DEVICE *dev, uint8 val)
{
    return 0;
}


static int remote_cu_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    REMOTE_CU_DEVICE_T* dev = (REMOTE_CU_DEVICE_T*)malloc(sizeof(REMOTE_CU_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(REMOTE_CU_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->remote_cu_power_set = remote_cu_power_set;
    dev->remote_cu_reset = remote_cu_reset;
    dev->remote_cu_on_level_set = remote_cu_on_level_set;
    dev->remote_cu_insert_state_get = remote_cu_insert_state_get;
    dev->remote_cu_sim_card_heat = remote_cu_sim_card_heat;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int remote_cu_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    REMOTE_CU_DEVICE_T *dev = (REMOTE_CU_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
REMOTE_CU_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "remote_cu",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-05-24",
        .nDevNum  = REMOTEL_CU_DEV_NUM,
        .szDevices = aRemoteCUDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = remote_cu_open,
        .pfClose = remote_cu_close,
    },
};
