#include "types.h"
#include <watchdog.h>
#include "sysDefine.h"

/*watchdog �豸·�� */
#define WATCHDOG_SCMDEV_NAME      "/dev/wdog"
#define WATCHDOG_DEV_NUM          1
static char *aWatchdogDevice[WATCHDOG_DEV_NUM] = {
                                         HW_DEVICE_ID_WATCHDOG};

#define WDT_BUFFER_SIZE     100

#define EXWDG_AUTO      0x20221055  ///< �Զ�ι��ģʽ

#define EXWDG_SETCNT_CMD      IOCTL_MACRO('e', 1,  WRITE_DATA_CMD, sizeof(int))    ///< ���Ź�ι��
#define EXWDG_SETMODE_CMD     IOCTL_MACRO('e', 3,  WRITE_DATA_CMD, sizeof(int))    ///< �����ֶ�/�Զ�ι��ģʽ
#define EXWDG_SETPERIOD_CMD   IOCTL_MACRO('e', 14,  WRITE_DATA_CMD, sizeof(unsigned int))  ///< ���ÿ��Ź���ʱʱ��
#define EXWDG_GETRCNT_CMD     IOCTL_MACRO('e', 6,  READ_DATA_CMD, sizeof(int))     ///< ��ȡ���Ź�����ʱ�䣬��λ����

/***************************************************
* @brief ���ÿ��Ź����ʱ��
* @param[in] dev: �豸����
* @param[in] period: ��ʱʱ�䣬ȡֵ��Χ[1,3600]����λ����
* @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
* @author: 
****************************************************/
int32 watchdog_period_set(struct tag_WATCHDOG_DEVICE *dev, uint32 period)
{
    int fd = 0;
    int retval =0;

    if(!dev)
    {
        return ERR_PNULL;
    }
    if( period > 3600 || period == 0)
    {
        return ERR_INVAL;
    }
    
    fd = open(WATCHDOG_SCMDEV_NAME, O_RDWR);
    if (fd <= 0 )
    {
        return ERR_NORMAL;
    }

    retval = ioctl(fd, EXWDG_SETPERIOD_CMD, &period);
    close(fd);
    if (-1 == retval)
    {
        perror("ioctl: EXWDG_SETPERIOD_CMD");
        return ERR_EXECFAIL;
    }
    

    return ERR_OK;
}

/***************************************************
* @brief ι������
* @param[in] dev: �豸����
* @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
* @author: 
****************************************************/
int32 watchdog_clear(struct tag_WATCHDOG_DEVICE *dev)
{
    int fd = 0;
    int retval = ERR_OK;
    uint32 state = 0;

    if(!dev)
    {
        return ERR_PNULL;
    }
    
    fd = open(WATCHDOG_SCMDEV_NAME, O_RDWR);
    if (fd <= 0 )
    {
        DEBUG("open watchdog error !\n");
        return ERR_NORMAL;
    }
    
    retval = ioctl(fd, EXWDG_SETCNT_CMD, &state);
    close(fd);
    if (-1 == retval)
    {
        perror("ioctl: EXWDG_KEEPALIVE_CMD");
        return ERR_EXECFAIL;
    }
    return ERR_OK;
}

/***************************************************
* @brief ���Ź���λ
* @param[in] dev: �豸����
* @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
* @author: 
****************************************************/
int32 watchdog_rst(struct tag_WATCHDOG_DEVICE *dev)
{
    return ERR_UNCOMPATIBLE;
}

/***************************************************
* @brief ��ȡ���Ź�����ʱ��
* @param[in] dev: �豸����
* @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
* @author: 
****************************************************/
int32 watchdog_runsec_get(struct tag_WATCHDOG_DEVICE *dev)
{
    int32 fd = 0;
    int32 retval = 0;
    int32 time = 0;
    
    fd = open(WATCHDOG_SCMDEV_NAME, O_RDWR);
    if (fd <= 0 )
    {
        DEBUG("open watchdog error !\n");
        return ERR_NORMAL;
    }

    retval = ioctl(fd, EXWDG_GETRCNT_CMD, &time);
    close(fd);
    if (-1 == retval)
    {
        perror("ioctl: EXWDG_GETRCNT_CMD");
        return ERR_EXECFAIL;
    }
    
    return time;
}


static int watchdog_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */
    
    WATCHDOG_DEVICE_T* dev = (WATCHDOG_DEVICE_T*)malloc(sizeof(WATCHDOG_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(WATCHDOG_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_WATCHDOG;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->watchdog_period_set = watchdog_period_set;
    dev->watchdog_clear = watchdog_clear;
    dev->watchdog_rst = watchdog_rst;
    dev->watchdog_runsec_get = watchdog_runsec_get;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int watchdog_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    WATCHDOG_DEVICE_T *dev = (WATCHDOG_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* ��ʼ��ģ����� */
WATCHDOG_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "watchdog",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-20",
        .nDevNum  = WATCHDOG_DEV_NUM,
        .szDevices = aWatchdogDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = watchdog_open,
        .pfClose = watchdog_close,
    },
};
