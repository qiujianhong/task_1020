#include <hal.h>
#include "types.h"
#include <adc.h>
#include "sysDefine.h"

/*adc 设备路径 */
#define   ADC_DEV_NAME      "/dev/adc" 

#define ADC_DEV_NUM      1
static char *aAdcDevice[ADC_DEV_NUM] = {HW_DEVICE_ID_ADC};

typedef struct
{
    int channel;
    int shift;
    int delay;
    int clock;
}adc_config;

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
    int32 fd = 0;
    int32 iRet = ERR_NORMAL;
    int32 iLen = 0;
    ////adc_config cfg;
    int32 iData = 0;  //模拟数据

    if((dev == NULL)||(pData == NULL))
    {
        DEBUG(" input pointer is null.\n");
        return ERR_PTR;        
    }
    
    fd = open(ADC_DEV_NAME, O_RDWR);
    if(fd < 0)
    {
        DEBUG(" open adc dev error\n");
        return ERR_NORMAL;
    }

    if(flock(fd, LOCK_EX))
    {
        printf("adc lock error\n");
	    iRet = ERR_LOCK;
        goto end;
    }

    if(index == EMADC_RTCVOL_CHANNEL)
    {
        iLen = ioctl(fd, EMADC_BATVOL_CHANNEL, &iData);
        iData = iData * 2;
    }
    else  if(index == EMADC_BATVOL_CHANNEL)
    {
        iLen = ioctl(fd, EMADC_RTCVOL_CHANNEL, &iData);
        iData = iData * 3;
    }
    else
        iLen = -1;
    
    if(iLen <0 )
        goto end;

    iRet = ERR_OK;
	
end:

    memcpy(pData, &iData, sizeof(iData));
    flock(fd, LOCK_UN);
    close(fd);
	
    return iRet;
}

static int adc_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
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

static int adc_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
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
