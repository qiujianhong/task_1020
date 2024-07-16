#include <hal.h>
#include <adc.h>

/*adc �豸·�� */
#define   ADC_DEV_NAME      "/dev/adc0" 


#define PIN_BASE                32
#define GPIO_MAGIC              0xAA00
#define GPIO_IN                 (0<<4)
#define GPIO_OUT                (1<<4)
#define PERIPH_A                (2<<4)
#define PERIPH_B                (3<<4)
#define GPIO_OUT_HIGH           GPIO_MAGIC | GPIO_OUT | 1
#define GPIO_OUT_LOW            GPIO_MAGIC | GPIO_OUT | 0
#define GPIO_IN_NUP             GPIO_MAGIC | GPIO_IN | 0
#define GPIO_IN_PULLUP          GPIO_MAGIC | GPIO_IN | 1

#define	AT91_PIN_PA14	        (PIN_BASE + 0x00 + 14)
#define	AT91_PIN_PB16		    (PIN_BASE + 0x20 + 16)
#define	AT91_PIN_PB17		    (PIN_BASE + 0x20 + 17)

#define REG_ADDR_MASK		    0xFF0000
#define RNW8302_INTSIG_IO       AT91_PIN_PB16
#define RNW8302_CS_PIN          AT91_PIN_PA14

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
    int32 fd = 0;
    int32 iRet = FALSE;
    adc_config cfg;
	int32 iData =712;  //ģ������

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

    memset(&cfg, 0, sizeof(cfg));
    cfg.channel = index;
    iRet = write(fd, &cfg, sizeof(cfg));
    if(iRet < 0)
    {
        printf("adc write failed\n");
	 iRet = ERR_IO;
        goto end;
    }

    *pData = 0;
    iRet = read(fd, pData, sizeof(pData));
    if(iRet < 0)
    {
        printf("adc read failed\n");
	 iRet = ERR_IO;
        goto end;
    }

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

static int adc_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
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
        .szBuildDate = "2018-10-18",
        .nDevNum = ADC_DEV_NUM,
        .szDevices = aAdcDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = adc_open,
        .pfClose = adc_close,
    },
};
