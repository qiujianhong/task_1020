#include "types.h"
#include <key.h>
#include "sysDefine.h"

/*key 设备路径 */
#define   KEY_DEV_NAME      "/dev/key"
#define   KEYRELEASE          0x80        //KEY release flag
//记录下最近一次按键时的系统运行时间
#define  KEY_PRESS_TIME_FILE  "/tmp/dev/lst_press_time"

#define KEY_DEV_NUM      1
static char *aKeyDevice[KEY_DEV_NUM] = {HW_DEVICE_ID_KEY};

/**
* @brief 将最近一次按键时的系统运行时间写入文件
* @param[in] 
* @param[out] 
* @note
* @return成功返回ERR_OK；失败返回错误码
*/
int32 key_press_time_write()
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;
    struct timespec tp;
    uint32 lst_press_time;
    fd = open(KEY_PRESS_TIME_FILE, O_CREAT|O_WRONLY,0666);
    if(fd < 0)
    {
        printf("open %s error!\n", KEY_PRESS_TIME_FILE);
        return ERR_NORMAL;   
    }

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    
    iRet = fcntl(fd, F_SETLKW, &lock);
    
    if(iRet != 0)
        printf("fcntl error ret:%d\n", iRet);

    //获取系统运行时间
    clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
    lst_press_time = tp.tv_sec;
    printf("lst_press_time:%d\n", lst_press_time);
    
    iRet = write(fd, &lst_press_time, sizeof(uint32));
    
    if(iRet != sizeof(uint32))
    {
        printf("lst_press_time write error\n");
        iRet = ERR_NORMAL;
    }
    else
    {
        iRet = ERR_OK;
    }

    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    
    if(iRet < 0)
    {
        printf("fcntl failed msg:%s\n", strerror(errno));
        iRet = ERR_LOCK;
    }
    
    close(fd);
    
    return iRet;
    
}


/****************************************************
* @brief 读取最近一次键值
* @param[in] dev: 设备描述
* @param[in] size: 获取键值的最大缓冲区长度， 单位为字节
* @param[out] key_val_list: 键值列表（采用linux内核定义的标准按键值）
* @note 按键设计遵循读后清原则， 驱动接口只获取单一按键状态值， 
         长按键或组合按键状态由平台基础库提供。
* @return 成功返回TRUE；失败返回FALSE
* @author: liuyue 2018-10-24
****************************************************/
static int32 key_get_value(struct tag_KEY_DEVICE *dev, struct tag_KEY_VALUE *key_val_list,
	uint32 size)
{

    int fd = 0;
    int retval = 0;
    fd_set fdset;
    int iRet = ERR_NORMAL;

    FD_ZERO (&fdset);                          /*清空集合中所有的元素*/    
    FD_SET(STDOUT_FILENO,&fdset);              /*设置stdout，使集合中包含stdout*/ 

    if((NULL == dev)||(NULL == key_val_list))
    {
        printf("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }
    
    fd = open(KEY_DEV_NAME, O_RDONLY|O_NONBLOCK);
    if(fd < 0) 
    {
        printf("Error opening %s device\n", KEY_DEV_NAME);
        return ERR_NORMAL;
	}

    if(flock(fd, LOCK_EX))
    {
		printf("key lock Error");
		goto end;
    }

    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);

    retval = select(fd + 1, &fdset, NULL, NULL, NULL);
    if (retval < 0) 
    {
	 DEBUG("KEY select error\n");
	 goto end;
    }    
	
    if(retval == 0) 
    {
    	 DEBUG("timeout.\n");
        goto end;
    } 

	/*如果当前有数据可读的设备是按键，则读取键值并显示*/
    if(FD_ISSET(fd, &fdset)) 
    {
        retval = read(fd, &(key_val_list->key), sizeof(key_val_list->key));
        if(0 >= retval) 
        {
            //printf("%s:%d read key error, ret=%d\n", __FUNCTION__, __LINE__, retval);
            goto end;
        }

    	if(key_val_list->key & KEYRELEASE)
    		key_val_list->status = 0;
    	else
    		key_val_list->status = 1;

        key_val_list->key &= 0x7F; 

        printf("key:%02x status=%02x\n", key_val_list->key, key_val_list->status);
        key_press_time_write();

	    iRet = ERR_OK;
    }
    
end:
    flock(fd, LOCK_UN);
    close(fd);
	
    return iRet;
}
/**
* @brief 读取最近一次按键时的系统运行时间
* @param[in] dev: 设备描述
* @param[out] lst_press_time: 最近一次按键时的系统运行时间（系统运行时间非系统时钟以防对时影响，单位为“秒”）
* @note 桌面APP获取最近一次按键时的系统运行时间与当前系统运行时间进行比对，
        超过一定限值后桌面APP强制收回显示控制权。
        当key_value_get函数获取到键值时刷新lst_press_time，
        并将其存储在/tmp/dev/lst_press_time文件下供所有接口共享。
* @return成功返回ERR_OK；失败返回错误码，键值未变化返回一般错误
（ERR_NORMAL）。
*/
static int32 key_press_time_get(struct tag_KEY_DEVICE *dev, uint32 *lst_press_time)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;
    uint32 tmptime = 0;

    fd = open(KEY_PRESS_TIME_FILE, O_CREAT|O_RDONLY,0666);
    if(fd < 0)
    {
        printf("open %s error!\n", KEY_PRESS_TIME_FILE);
        return ERR_NORMAL;   
    }
    
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    
    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
        printf("fcntl error ret:%d\n", iRet);
    
    
    iRet = read(fd, &tmptime, sizeof(uint32) ); 
    if(iRet < 0)
    {
        printf("lst_press_time read error\n");
        iRet = ERR_NORMAL;
    }
    else
    {
        iRet = ERR_OK;
    }
    *lst_press_time = tmptime;
    
   
   lock.l_type = F_UNLCK;
   iRet = fcntl(fd, F_SETLK, &lock);
   
   if(iRet < 0)
   {
       printf("fcntl failed msg:%s\n", strerror(errno));
       iRet = ERR_LOCK;
   }
   
   close(fd);
   return iRet;
}   

static int key_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    KEY_DEVICE_T* dev = (KEY_DEVICE_T*)malloc(sizeof(KEY_DEVICE_T));
    if(dev == NULL)
    {
        printf("malloc failed \n");
        return ERR_MEM;
    }
	
    memset(dev, 0, sizeof(KEY_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_KEY;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->key_value_get = key_get_value;
    dev->key_press_time_get = key_press_time_get;
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int key_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    KEY_DEVICE_T *dev = (KEY_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
KEY_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "key",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-20",
        .nDevNum = KEY_DEV_NUM,
        .szDevices = aKeyDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = key_open,
        .pfClose = key_close,
    },
};
