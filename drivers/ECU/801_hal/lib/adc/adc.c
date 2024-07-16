#include <hal.h>
#include "types.h"
#include <adc.h>
#include <linux/ioctl.h>
#include "sysDefine.h"

/*adc 设备路径 */
#define DEV_PATH_ADC "/dev/adc"

#define ADC_GETBTVOL_CMD    IOCTL_MACRO('a', 1, READ_DATA_CMD, 4)       ///< 获取ADC电压 电池电压
#define ADC_GETRTCVOL_CMD   IOCTL_MACRO('a', 2, READ_DATA_CMD, 4)       ///< 获取ADC电压 时钟电压
#define ADC_GETCAPVOL_CMD   IOCTL_MACRO('a', 3, READ_DATA_CMD, 4)       ///< 获取ADC电压 电容电压
//#define ADC_SETCHANNEL_CMD  IOCTL_MACRO('a', 4, WRITE_DATA_CMD, 4)      ///< 设置采集通道
//#define ADC_GETCHANNEL_CMD  IOCTL_MACRO('a', 5, READ_DATA_CMD, 4)       ///< 获得采集通道
//#define ADC_GETCHANNELVOL_CMD  IOCTL_MACRO('a', 6, READ_DATA_CMD, 4)       ///< 获得采集通道 电压值


#define ADC_DEV_NUM      1
static char *aAdcDevice[ADC_DEV_NUM] = {HW_DEVICE_ID_ADC};

int32 __AdcDeviceRead(int32 fd, int32 index, int32*data)
{
    int32 nRet = ERR_NORMAL;
    int32 nData = 0;
    switch(index){
        case EMADC_RTCVOL_CHANNEL: //时钟电池ADC通道号
            nRet = ioctl(fd, ADC_GETRTCVOL_CMD, &nData);
            if(nData > 0)
            {
                *data = (int32)nData*3;
            }else{
                *data = 0;
            }
            break;
        case EMADC_BATVOL_CHANNEL: //备用电池ADC通道号
            nRet = ioctl(fd, ADC_GETBTVOL_CMD, &nData);
            if(nData > 0)
            {
                *data = (int32)nData*3.67;
                if(*data >= 5650)
                {
                    *data = 0;
                }
            }else{
                /**< 硬件不支持备用电池时设置为0*/
                *data = 0;
            }
            break;

        case EMADC_TEMP_CHANNEL://温度传感器ADC通道号单位 0.1°
            /**< 硬件不支持备用电池时设置为0*/
            *data = 0;
            /*
            nRet = adc_get_temp(&nData);
            if(nData > 0)
            {
                *data = (int32)nData;
            }else{
                *data = 0;
            }*/
            break;
        case EMADC_CAPVOL_CHANNEL://超级电容电压
            nRet = ioctl(fd, ADC_GETCAPVOL_CMD, &nData);
            if(nData > 0)
            {
                *data = (int32)nData*7.06;
            }else
            {
                *data = 0;
            }
            break;
        default:
            break;
    }
    return nRet;
}


//static unsigned char V[4] = { 0 };
//static unsigned char I[4] = { 0 };
//static float fV[4] = {0.0};
//static float fI[4] = {0.0};
/************************************************************
* @brief 获取指定ADC通道数据
* @param[in] dev: 设备描述
* @param[in] index: ADC通道序号（0~max）
* @param[out] data: ADC通道数据（单位：mv）
* @note pData返回转换之后的模拟电压值， 单位为mv， 不能直接返回
  ADC采样的寄存器数字量
* @return 成功返回ERR_OK；失败返回错误码，详见：附录1
****************************************************/
int32 adc_read_volt(struct tag_ADC_DEVICE*dev, ADC_CHANNEL_E index, int32 *pData)
{
    int32 ret,fd;
    if(dev == NULL || pData == NULL )
    {
        return ERR_PNULL;
    }
    if(index > EMADC_MAX_CHANNEL )
    {
        return ERR_ACCESS;
    }
    
    /* 1、打开设备文件，驱动是字符设备驱动，设备名为device id */
    fd = open(DEV_PATH_ADC, O_RDONLY);
    if(-1 == fd)
    {
        DEBUG("open %s error\n",DEV_PATH_ADC);
        return ERR_NODEV;
    }
    ret = __AdcDeviceRead(fd, (int32)index, pData);
    close(fd);
    
    return ret;
}

static int adc_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    ADC_DEVICE_T* dev = (ADC_DEVICE_T*)malloc(sizeof(ADC_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_MEM;
    }
    
    memset(dev, 0, sizeof( ADC_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_ADC;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->adc_volt_read = adc_read_volt;
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int adc_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    ADC_DEVICE_T *dev = (ADC_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
ADC_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "adc",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-03-18",
        .nDevNum = ADC_DEV_NUM,
        .szDevices = aAdcDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = adc_open,
        .pfClose = adc_close,
    },
};
