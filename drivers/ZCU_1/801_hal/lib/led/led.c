#include "types.h"
#include <led.h>
#include "sysDefine.h"

/*led �豸·�� */
#define   LED_DEV_NAME      "/dev/led" 
/*led ���� */
#define   LED_MASK    0xfffffff

#define LED_DEV_NUM      1
static char *aLedDevice[LED_DEV_NUM] = {HW_DEVICE_ID_LED};

typedef struct tag_LEDINDEX
{
    int emled;
    int index;
}LED_INDEX_T;

LED_INDEX_T g_LED_index[] = 
{
    {EMLED_RUN,      -1},
    {EMLED_ALARM,    -1},
    {EMLED_PC,        8},
    {EMLED_EC,        9},
    {EMLED_ES,       -1},
    {EMLED_OL,       -1},
    {EMLED_SIGG,     -1},
    {EMLED_SIGR,     -1},
    {EMLED_DR,       -1},
    {EMLED_DT,       -1},
    {EMLED_UR,       -1},
    {EMLED_UT,       -1},
    {EMLED_485IR,    -1},
    {EMLED_485IT,    -1},
    {EMLED_485IIR,   -1},
    {EMLED_485IIT,   -1},
    {EMLED_ROUND1G,  1},
    {EMLED_ROUND1R,  0},
    {EMLED_ROUND2G,  3},
    {EMLED_ROUND2R,  2},
    {EMLED_ROUND3G,  5},
    {EMLED_ROUND3R,  4},
    {EMLED_ROUND4G,  7},
    {EMLED_ROUND4R,  6},
    
    {EMLED_ROUND5G,  13},
    {EMLED_ROUND5R,  12},
    {EMLED_ROUND6G,  15},
    {EMLED_ROUND6R,  14},
    {EMLED_ROUND7G,  17},
    {EMLED_ROUND7R,  16},
    {EMLED_ROUND8G,  19},
    {EMLED_ROUND8R,  18},
    
    {EMLED_RADIOTX,  -1},
    {EMLED_RADIORX,  -1},
    {EMLED_RADIOTEL, -1},
};

/****************************************************
* @brief ����LED״̬
* @param[in] dev: �豸����
* @param[in] index: LED�����
* @param[in] state: LED��״̬

###* @param[in] mode: ����ģʽ����ƽ/��˸ģʽ��
###* @param[in] hz: ��˸Ƶ��(��λ��0.01Hz)
###* @param[in] sync_id: ͬ����˸����ű�ʶ��0�� ��ʾ����Ҫ����
                ��LED��ͬ����˸�� ����0�� ��ʾ��Ҫͬ����˸����ţ�
                ����ʵ��ʱ�� ����0����ͬ��ŵ�LED��ͬ����˸��
###* @return �ɹ�����TRUE��ʧ�ܷ���FALSE
###* @author: liuyue 2018-10-24
****************************************************/
int led_set_state(struct tag_LED_DEVICE *dev, LED_INDEX_E index, LED_TYPE_E state, 
MODE_TYPE_E mode, LED_ON_OFF_RATIO_E *ratio, uint32 flicker_count)
{
    int fd = 0;
    int iRet = ERR_OK;

    if(-1 == g_LED_index[index - 1].index)
    {
        return ERR_NORMAL;
    }
    
    fd = open(LED_DEV_NAME, O_RDWR);
    if (fd < 0) 
    { 
	printf("can not open device led.err %d \n",fd); 
	return ERR_NORMAL; 
    }
	
    if(flock(fd, LOCK_EX))
    {
	printf("led lock Error");
	iRet = ERR_LOCK;
	goto end;
    }
    
    /*����LED���� */
    iRet = ioctl(fd, state, g_LED_index[index - 1].index);
    if(iRet < 0)
    {
        printf("led ioctl failed\n");
        iRet = ERR_NORMAL;
    }
	
end:
    flock(fd, LOCK_UN);
    close(fd);
	
    return iRet;
}


static int led_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
    LED_DEVICE_T* dev = (LED_DEVICE_T*)malloc(sizeof(LED_DEVICE_T));
    if(dev == NULL)
    {
        printf("malloc failed \n");
        return -1;
    }

    memset(dev, 0, sizeof( LED_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_LED;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->led_state_set = led_set_state;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int led_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    LED_DEVICE_T *dev = (LED_DEVICE_T *)pDevice;
    if(dev == NULL)
    {
        printf("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    free(dev);
    return ERR_OK;
}

/* ��ʼ��ģ����� */
LED_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "led",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2018-10-18",
        .nDevNum = LED_DEV_NUM,
        .szDevices = aLedDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = led_open,
        .pfClose = led_close,
    },
};
