#include "types.h"
#include <watchdog.h>
#include "sysDefine.h"

/*watchdog 设备路径 */
#define WATCHDOG_SCMDEV_NAME      "/dev/watchdog" 
#define WATCHDOG_MAXDEV_NAME      "/dev/max706wdt" 
#define WATCHDOG_DEV_NUM          1
static char *aWatchdogDevice[WATCHDOG_DEV_NUM] = {
                                         HW_DEVICE_ID_WATCHDOG};
                                         
                                         
static uint32  g_uTermType = 0;

/***************************************************
* @brief 设置看门狗溢出时间
* @param[in] dev: 设备描述
* @param[in] period: 超时时间，取值范围[1,3600]，单位：秒
* @return 成功返回ERR_OK；失败返回错误码
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

    /*使能10s看门狗 */
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
* @brief 喂狗操作
* @param[in] dev: 设备描述
* @return 成功返回ERR_OK；失败返回错误码
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

    /*喂狗 */
   ioctl(fd, WDIOC_KEEPALIVE, 0);
   flock(fd, LOCK_UN);  
end:
    
    close(fd);
    return err;
}

/***************************************************
* @brief 看门狗复位
* @param[in] dev: 设备描述
* @return 成功返回ERR_OK；失败返回错误码
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

    /*复位 */
    timeout = 1;
    //printf("Set timeout to %d\n", timeout);
    ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
    flock(fd, LOCK_UN);
	
end:

    close(fd);
    return err;
}

/***************************************************
* @brief 读取看门狗运行时间
* @param[in] dev: 设备描述
* @return 成功返回ERR_OK；失败返回错误码
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

	/* 读剩余时间 */
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
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
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

/* 初始化模块变量 */
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
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = watchdog_open,
        .pfClose = watchdog_close,
    },
};
