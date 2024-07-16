#include <rtc.h>

/*外部RTC设备路径 */
#define   RTC_DEV_NAME_EX      "/dev/rtc8025t"   
/*内部RTC */
#define   RTC_DEV_NAME            "/dev/misc/rtc"          

#define BCD2BIN(val)                 (((val) & 0x0f) + ((val)>>4)*10)
#define BIN2BCD(val)                 ((((val)/10)<<4) + (val)%10)

#define BCD_TO_BIN(val)           ((val)=BCD2BIN(val))
#define BIN_TO_BCD(val)           ((val)=BIN2BCD(val))

#define RTC_RD_WB                   0x13
#define RTC_SET_WB                  0x14

#define SET_CH                      0

#define RTC_DEV_NUM                 1
static char *aRtcDevice[RTC_DEV_NUM] = {HW_DEVICE_ID_RTC};

/*************************************************************************
 * 函数功能: 同步系统时钟为硬件时钟函数
 * 输入参数: dt --> 外部硬件时钟
 * 输出参数: 0: 同步成功  -1:failure
 * 操作流程: 
 ************************************************************************/
static int chk_sys_clock(struct rtc_time *dt)
{
    struct timeval tv = {0, 0};
    struct timezone tz = {0, 0};
    struct tm time;
    time_t t = 0;

    /* 取得当前系统时间 */
    if(gettimeofday (&tv, &tz)) 
    {
        perror("gettimeofday() failed");
        return -1;
    }
    
    /* 将硬件时间转化成秒 */    
    memset(&time, 0, sizeof(struct tm));
    time.tm_isdst = -1; // not known
    time.tm_sec   = dt->tm_sec;
    time.tm_min   = dt->tm_min;
    time.tm_hour  = dt->tm_hour;
    time.tm_mday  = dt->tm_mday;
    time.tm_mon   = dt->tm_mon;
    time.tm_year  = dt->tm_year;
    time.tm_wday  = dt->tm_wday;
    time.tm_yday  = dt->tm_yday;    
    t = mktime(&time);          //将时间结构体表示时间转换成经过的秒数

    /* 设置时间到系统时间 */
    tv.tv_sec = t;  
    if (settimeofday(&tv, &tz))
    {
        perror("settimeofday() failed");
        return -1;
    }

    return 0;   
}



/**********************************************
  所属功能单元: rtc
  功能描述: 读取外部实时时钟rtc
  输入参数: 无
  输出参数: prtc 实时时间（以rtc结构表示）
  函数返回值: 无
  内部处理流程：
***********************************************/
int32 read_rtc_ext(struct tag_RTC_DEVICE *dev, struct tm *prtc)
{
    struct rtc_time time_read;
    int32 fd = 0;
    int32 err = ERR_NORMAL;

    fd = open(RTC_DEV_NAME_EX, O_RDWR);
    if(fd < 0)
    {
        DEBUG("open rtc dev failed \n");
        return err;
    }
	//设置共享锁
    if( flock(fd, LOCK_EX) )
    {
        DEBUG("Lock Error");
	 err = ERR_LOCK;
        goto error;
    }

	//读出时钟
    ioctl(fd, RTC_RD_TIME, &time_read);

	//转换成BCD码
    prtc->tm_sec =  time_read.tm_sec;
    prtc->tm_min =  time_read.tm_min;
    prtc->tm_hour = time_read.tm_hour;
    prtc->tm_wday = time_read.tm_wday;
    prtc->tm_mday = time_read.tm_mday;
    prtc->tm_mon =  time_read.tm_mon;
    prtc->tm_year = time_read.tm_year;

   	//释放锁
    flock(fd, LOCK_UN);

    close(fd);
    return ERR_OK;
	
error:
    close(fd);
    return err;

}

/**********************************************
  所属功能单元: rtc
  功能描述: 写外部实时时钟rtc
  输入参数: prtc 实时时间（以rtc结构表示）
  输出参数: 无
  函数返回值: 无
  内部处理流程：
***********************************************/
int32 write_rtc_ext(RTC_DEVICE_T *dev, struct tm *prtc)
{
    int32 err = ERR_NORMAL;;
    struct rtc_time time_write;
    int32 fd = 0;

    /* 将写入时间格式转换成BIN格式*/
    time_write.tm_sec  = prtc->tm_sec;
    time_write.tm_min  = prtc->tm_min;
    time_write.tm_hour = prtc->tm_hour;
    time_write.tm_wday = prtc->tm_wday;
    time_write.tm_mday = prtc->tm_mday;
    time_write.tm_mon =  prtc->tm_mon;
    time_write.tm_year = prtc->tm_year;

    /*检验所设时间的有效性*/
    if (time_write.tm_year < 70)
        time_write.tm_year += 100;

    if (time_write.tm_sec < 0 || time_write.tm_sec >= 60) 
    {
        err = -EINVAL;
        goto errout_unlock;
    }
	
    if (time_write.tm_min < 0 || time_write.tm_min >= 60)
    {
        DEBUG("minutes out of range.\n");
        err = -EINVAL;
        goto errout_unlock;
    }
	
    if (time_write.tm_hour < 0 || time_write.tm_hour >= 24) 
   {
        DEBUG("hours out of range.\n");
        err = -EINVAL;
        goto errout_unlock;
    }
	
    if (time_write.tm_wday < 0 || time_write.tm_wday >= 7) 
   {
        DEBUG("wday out of range.\n");
        err = -EINVAL;
        goto errout_unlock;
    }
	
    if (time_write.tm_mon < 0 || time_write.tm_mon >= 12) 
   {
        DEBUG("month out of range.\n");
        err = -EINVAL;
        goto errout_unlock;
    }
	
    if (time_write.tm_year < 70 || time_write.tm_year >= 170) 
   {
        DEBUG("year out of range.\n");
        err = -EINVAL;
        goto errout_unlock;
    }
	
    if (time_write.tm_mday > 31 ||
        ((time_write.tm_mon == 3 || time_write.tm_mon == 5 ||
          time_write.tm_mon == 8 || time_write.tm_mon == 10)
         && time_write.tm_mday > 30) ||
        (time_write.tm_mon == 1 && time_write.tm_mday > 28 +
         (time_write.tm_year & 3 ? 0 : 1))) 
    {
        DEBUG("day out of range (with month=%d, "
                "year=%d): %d.\n", time_write.tm_mon, time_write.tm_year, time_write.tm_mon);
        err = -EINVAL;
        goto errout_unlock;
    }


    fd = open(RTC_DEV_NAME_EX, O_RDWR, 0666);
    if(fd < 0)
    {
        printf("open rtc dev failed \n");
        return ERR_NORMAL;
    }
	//设置互斥锁
    if( flock(fd, LOCK_EX) )
    {
        DEBUG("Lock Error");
	    err = ERR_LOCK;
        goto error;
    }

	//写入时间
    err = ioctl(fd, RTC_SET_TIME, &time_write);

	//释放锁
    flock(fd, LOCK_UN);

    close(fd);

	/* 同步系统时钟为外部硬件时钟 */
    if(chk_sys_clock(&time_write))
    {
        return ERR_NORMAL;  //failure
    }
	
	err = (err >= 0)?ERR_OK : ERR_NORMAL;
	
    return err;

errout_unlock:
    return err;
	
error:
    close(fd);
    return ERR_NORMAL;

}


/**
* @brief 设置rtc秒脉冲状态 1：开启 0：关闭
* @param[in] dev: 设备描述
* @param[in] val: 状态值
* @return 成功返回TRUE；失败返回FALSE
*/
int32 rtc_mode_set(struct tag_RTC_DEVICE *dev, int32 val)
{
    int32 iRet = ERR_OK;
  
    return iRet;
}
/**
* @brief 获取rtc秒脉冲状态 1：开启 0：关闭
* @param[in] dev: 设备描述
* @param[out] val: 状态值
* @return 成功返回TRUE；失败返回FALSE
*/
int32 rtc_mode_get(struct tag_RTC_DEVICE *dev, int32 *val)
{
    int32 iRet = ERR_OK;
  
    return iRet;
}

/**
* @brief 设置rtc温度补偿
* @param[in] dev: 设备描述
* @param[in] val: 温度值
* @return 成功返回TRUE；失败返回FALSE
*/
int32 rtc_temp_set(struct tag_RTC_DEVICE *dev, float32 val)
{
    int32 iRet = ERR_OK;
  
    return iRet;
}


static int rtc_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    RTC_DEVICE_T* dev = (RTC_DEVICE_T*)malloc(sizeof(RTC_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(*dev));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_RTC;
    dev->base.pModule = (HW_MODULE *)module;
    
   // dev->chk_rtc = chk_rtc;
   // dev->read_rtc_inter = read_rtc_inter;
   // dev->write_rtc_inter = write_rtc_inter;
   // dev->read_rtc_ext = read_rtc_ext;
   // dev->write_rtc_ext = write_rtc_ext;
   // dev->write_rtc_all = write_rtc_all;
   dev->rtc_time_set = write_rtc_ext;
   dev->rtc_time_get = read_rtc_ext;
   dev->rtc_mode_set = rtc_mode_set;  
   dev->rtc_mode_get = rtc_mode_get;
   dev->rtc_temp_set = rtc_temp_set;
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int rtc_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    RTC_DEVICE_T *dev = (RTC_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
RTC_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "rtc",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2018-10-18",
        .nDevNum = RTC_DEV_NUM,
        .szDevices = aRtcDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = rtc_open,
        .pfClose = rtc_close,
    },
};

