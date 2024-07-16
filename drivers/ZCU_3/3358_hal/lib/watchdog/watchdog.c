#include "types.h"
#include <watchdog.h>
#include "sysDefine.h"

/*watchdog �豸·�� */
#define WATCHDOG_SCMDEV_NAME      "/dev/watchdog" 
#define WATCHDOG_MAXDEV_NAME      "/dev/max706wdt" 
#define WATCHDOG_DEV_NUM          1
static char *aWatchdogDevice[WATCHDOG_DEV_NUM] = {
                                         HW_DEVICE_ID_WATCHDOG};
                                         
                                         
static uint32  g_uTermType = 0;

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
    int timeout =0;
    int err = ERR_OK;
	
    if(g_uTermType == 1)
	    fd = open(WATCHDOG_SCMDEV_NAME, O_RDWR);
	  else
		  fd = open(WATCHDOG_MAXDEV_NAME, O_RDWR);
    if (fd <=0 )
    {
        DEBUG("open watchdog error !\n");
        return ERR_NORMAL;
    }
	
    if(flock(fd, LOCK_EX))
    {
        DEBUG("led lock Error");
        err = ERR_LOCK;
        goto end;
    }

    /*ʹ��10s���Ź� */
   // ioctl(fd, 3, 0);
    timeout = period;
    ////printf("Set timeout to %d\n", timeout);
    ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
    flock(fd, LOCK_UN);
end:
    
    close(fd);
    return err;
}

/***************************************************
* @brief ι������
* @param[in] dev: �豸����
* @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
* @author: 
****************************************************/
int32 watchdog_clear(struct tag_WATCHDOG_DEVICE *dev)
{
    int32 fd = 0;
    int32 err = ERR_OK;
    
    if(g_uTermType == 1)
	    fd = open(WATCHDOG_SCMDEV_NAME, O_RDWR);	 
	  else
		  fd = open(WATCHDOG_MAXDEV_NAME, O_RDWR);	       
    if (fd <=0 )
    {
        DEBUG("open watchdog error !\n");
        return ERR_NORMAL;
    }
	
    if(flock(fd, LOCK_EX))
    {
    	DEBUG("led lock Error");
    	err = ERR_LOCK;
    	goto end;
    }

    /*ι�� */
   ioctl(fd, WDIOC_KEEPALIVE, 0);
   flock(fd, LOCK_UN);  
end:
    
    close(fd);
    return err;
}

/***************************************************
* @brief ���Ź���λ
* @param[in] dev: �豸����
* @return �ɹ�����ERR_OK��ʧ�ܷ��ش�����
* @author: 
****************************************************/
int32 watchdog_rst(struct tag_WATCHDOG_DEVICE *dev)
{
    int32 fd = 0;
    int32 timeout = 0;
    int32 err = ERR_OK;
    
    if(g_uTermType == 1)
	      fd = open(WATCHDOG_SCMDEV_NAME, O_RDWR);	 
	  else
		    fd = open(WATCHDOG_MAXDEV_NAME, O_RDWR);	   
    if (fd <= 0 )
    {
        DEBUG("open watchdog error !\n");
        return ERR_NORMAL;
    }
	
    if(flock(fd, LOCK_EX))
    {
        DEBUG("led lock Error");
        err = ERR_LOCK;
        goto end;
    }

    /*��λ */
    timeout = 1;
    //printf("Set timeout to %d\n", timeout);
    ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
    flock(fd, LOCK_UN);
	
end:

    close(fd);
    return err;
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
	int32 timeleft = 0;
	
	if(g_uTermType == 1)
	    fd = open(WATCHDOG_SCMDEV_NAME, O_RDWR);	 
	else
		  fd = open(WATCHDOG_MAXDEV_NAME, O_RDWR);	
	if (fd <= 0 )
	{
		DEBUG("open watchdog error !\n");
		return ERR_NORMAL;
	}
	
	if(flock(fd, LOCK_EX))
	{
		DEBUG("led lock Error");
		timeleft = ERR_LOCK;
		goto end;
	}

	/* ��ʣ��ʱ�� */
	ioctl(fd, WDIOC_GETTIMELEFT, &timeleft);
	//printf("The timeout was is %d seconds\n", timeleft);
	
end:
	flock(fd, LOCK_UN);
	close(fd);
	
	return timeleft;
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
