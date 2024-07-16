#include <hal.h>
#include "types.h"
#include <adc.h>
#include <linux/ioctl.h>
#include "sysDefine.h"

/*adc �豸·�� */
#define DEV_PATH_ADC "/dev/adc"

#define ADC_GETBTVOL_CMD    IOCTL_MACRO('a', 1, READ_DATA_CMD, 4)       ///< ��ȡADC��ѹ ��ص�ѹ
#define ADC_GETRTCVOL_CMD   IOCTL_MACRO('a', 2, READ_DATA_CMD, 4)       ///< ��ȡADC��ѹ ʱ�ӵ�ѹ
#define ADC_GETCAPVOL_CMD   IOCTL_MACRO('a', 3, READ_DATA_CMD, 4)       ///< ��ȡADC��ѹ ���ݵ�ѹ
//#define ADC_SETCHANNEL_CMD  IOCTL_MACRO('a', 4, WRITE_DATA_CMD, 4)      ///< ���òɼ�ͨ��
//#define ADC_GETCHANNEL_CMD  IOCTL_MACRO('a', 5, READ_DATA_CMD, 4)       ///< ��òɼ�ͨ��
//#define ADC_GETCHANNELVOL_CMD  IOCTL_MACRO('a', 6, READ_DATA_CMD, 4)       ///< ��òɼ�ͨ�� ��ѹֵ


#define ADC_DEV_NUM      1
static char *aAdcDevice[ADC_DEV_NUM] = {HW_DEVICE_ID_ADC};

int32 __AdcDeviceRead(int32 fd, int32 index, int32*data)
{
    int32 nRet = ERR_NORMAL;
    int32 nData = 0;
    switch(index){
        case EMADC_RTCVOL_CHANNEL: //ʱ�ӵ��ADCͨ����
            nRet = ioctl(fd, ADC_GETRTCVOL_CMD, &nData);
            if(nData > 0)
            {
                *data = (int32)nData*3;
            }else{
                *data = 0;
            }
            break;
        case EMADC_BATVOL_CHANNEL: //���õ��ADCͨ����
            nRet = ioctl(fd, ADC_GETBTVOL_CMD, &nData);
            if(nData > 0)
            {
                *data = (int32)nData*3.67;
                if(*data >= 5650)
                {
                    *data = 0;
                }
            }else{
                /**< Ӳ����֧�ֱ��õ��ʱ����Ϊ0*/
                *data = 0;
            }
            break;

        case EMADC_TEMP_CHANNEL://�¶ȴ�����ADCͨ���ŵ�λ 0.1��
            /**< Ӳ����֧�ֱ��õ��ʱ����Ϊ0*/
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
        case EMADC_CAPVOL_CHANNEL://�������ݵ�ѹ
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
* @brief ��ȡָ��ADCͨ������
* @param[in] dev: �豸����
* @param[in] index: ADCͨ����ţ�0~max��
* @param[out] data: ADCͨ�����ݣ���λ��mv��
* @note pData����ת��֮���ģ���ѹֵ�� ��λΪmv�� ����ֱ�ӷ���
  ADC�����ļĴ���������
* @return �ɹ�����ERR_OK��ʧ�ܷ��ش����룬�������¼1
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
    
    /* 1�����豸�ļ����������ַ��豸�������豸��Ϊdevice id */
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
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
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

/* ��ʼ��ģ����� */
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
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = adc_open,
        .pfClose = adc_close,
    },
};
