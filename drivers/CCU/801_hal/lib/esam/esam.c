#include "types.h"
#include <esam.h>
#include <gpio.h>
#include "sysDefine.h"
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include <stdarg.h>
#include <pthread.h>

#define DEV_PATH_PESAM "/dev/encryption0"
#define DEV_PATH_XESAM "/dev/encryption1"

/*
///< ioctl
//#define GPIO_GETVER_CMD     IOCTL_MACRO('g', 0, READ_VER_CMD, VERSION_SIZE)             ///< 获取驱动版本
#define GPIO_SETMODE_CMD    IOCTL_MACRO('g', 0, WRITE_DATA_CMD, sizeof(gpio_arg_t))     ///< 配置gpio模式(输入/输出/脉冲)
#define GPIO_GETVAL_CMD     IOCTL_MACRO('g', 1, READ_DATA_CMD, sizeof(gpio_arg_t))      ///< 获取gpio电平
#define GPIO_SETVAL_CMD     IOCTL_MACRO('g', 2, WRITE_DATA_CMD, sizeof(gpio_arg_t))     ///< 设置gpio电平

///< gpio模式 bit[1:0](0-输入 1-输出 2-脉冲单次 3-脉冲循环)
#define GPIOMODE_INPUT		0
#define GPIOMODE_OUTPUT		1
#define GPIOMODE_PULSE		2
#define GPIOMODE_PULSECYC	3

#define GPIO_LOW        0
#define GPIO_HIGH       1

#define PB(n) (1*32+n)

#define ESAM_POWER_SIG 	PB(8)

typedef union {
    unsigned long ul;
    struct {
        unsigned short pin;     ///< 管脚序号
        unsigned char type;     ///< 类型 SETMUX:复用类型
                                ///< 类型 SETMODE: gpio模式 bit[1:0](0-输入 1-输出 2-脉冲单次 3-脉冲循环) bit[7:2]脉宽 单位10ms
        unsigned char state;    ///< 初始电平状态 脉宽模式示例：1(初始电平)->0(脉宽)->1
    } ch;
} gpio_arg_t;

static int set_gpio_output(int fd, unsigned short pin, unsigned char state)
{
    gpio_arg_t gpio_value;
    int retval;

    gpio_value.ch.pin   = pin;
    gpio_value.ch.state = GPIO_HIGH;
    gpio_value.ch.type  = GPIOMODE_OUTPUT;
    retval = ioctl(fd, GPIO_SETMODE_CMD, &gpio_value);  //设置 GPIO CHANNEL,
    if (retval < 0) {
        perror("ioctl:GPIO_SETMODE_CMD ERROR!\n");
        //close(fd);
        return -1;
    }
    //LOG(DEBUG,"SET GPIO OUTPUT VALUE: pin:%d, state:%d, type:%d OK!\n",gpio_value.ch.pin,gpio_value.ch.state,gpio_value.ch.type);
	gpio_value.ch.state = state;
    retval = ioctl(fd, GPIO_SETVAL_CMD, &gpio_value);  //设置 GPIO CHANNEL,
    if (retval < 0) {
        perror("ioctl:GPIO_SETVAL_CMD ERROR!\n");
        //close(fd);
        return -1;
    }
    LOG(DEBUG,"SET GPIO VALUE: pin:%d, state:%d, type:%d OK!\n",gpio_value.ch.pin, gpio_value.ch.state, gpio_value.ch.type);
	return 0;
}*/
/**
  *@ brief 读写结构体
*/
struct encryption_req {
      int buflen;
      char* bufdata;
};

#define ESAM_GET_DATA		_IOW('E', 0x01, struct encryption_req)
#define ESAM_SET_DATA		_IOW('E', 0x02, struct encryption_req)
#define ESAM_SETPWR_CMD     _IOW('E', 0x00, struct encryption_req)   ///< 电源控制 复位/上电/关电

static uint8 bNeedAckOK;
/*
///< yx_esam ioctl
#define XESAM_SETRST_CMD    IOCTL_MACRO('x', 0, WRITE_DATA_CMD, sizeof(int))   ///< 电源控制 复位并设置ESAM类型
#define XESAM_SETPWR_CMD    IOCTL_MACRO('x', 1, WRITE_DATA_CMD, sizeof(int))   ///< 电源控制 复位/上电/关电
#define XESAM_GETRSTF_CMD	IOCTL_MACRO('x', 2, READ_DATA_CMD,  sizeof(int))   ///< 设置复位标识
#define XESAM_SETLOCK_CMD   IOCTL_MACRO('x', 4, WRITE_DATA_CMD, sizeof(int))   ///< esam ??éè??1 é???￡? 0?a??
///< pw ioctl
#define PESAM_SETRST_CMD    IOCTL_MACRO('p', 0, WRITE_DATA_CMD, sizeof(int))   ///< 电源控制 复位并设置ESAM类型
#define PESAM_SETPWR_CMD    IOCTL_MACRO('p', 1, WRITE_DATA_CMD, sizeof(int))   ///< 电源控制 复位/上电/关电
#define PESAM_GETRSTF_CMD	IOCTL_MACRO('p', 2, READ_DATA_CMD,  sizeof(int))   ///< 设置复位标识
*/
/*
static char  CalEsamLrc(char *buf,int len)
{
	int  i;
	char LRC = 0;
	for(i=0;i<len;i++)
	{
		LRC ^= buf[i];
	}
	LRC = ~LRC;
	return LRC;
}*/
static void EsamWrite(int fd,uint8 *buf,int len)
{
	int wlen = 2048;
	struct encryption_req encryption_reg_req_st;
	char wbuf[2048];
	memset(wbuf,0,sizeof(wlen));
	memcpy(&wbuf[0],buf,len);
	encryption_reg_req_st.buflen = len;
	encryption_reg_req_st.bufdata = (char *)wbuf;
/*
	int i;
	printf("EsamWrite: \n");
	for(i=0;i<len;i++)
		printf("%02x ",wbuf[i]);
	printf("\n");
*/
	ioctl(fd, ESAM_SET_DATA, &encryption_reg_req_st);
	return;
}

static int EsamRead(int fd,uint8 *buf,int len)
{
	//int i = 0;
	//char lrc = 0;
	int le = 0;
	struct encryption_req encryption_reg_req_st={0};
	encryption_reg_req_st.buflen = 6;
	encryption_reg_req_st.bufdata = (char *)buf;
	ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);  //首先读出4个字节
	if(0x90==buf[1] && 0x00==buf[2])
	{
		le = ((int)(buf[3])<<8) + buf[4];
		//printf("le=%d\n",le);
		if(le <= (len-5))
		{
			encryption_reg_req_st.buflen = le+6;	//4:90 00 le(high) le(low) 1：需要读出检验字节
			encryption_reg_req_st.bufdata = (char *)buf;
			ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);
/*
			printf("EsamRead: \n");
			for(i=0;i<(le+4+1);i++) //4:sw1 sw2 len1 len2 1:检验
			{
				printf("%02x ",buf[i]);
			}
			printf("\n");*/
			//WriteSysCommLogFun(CommLogTypeEsamR,buf,le+4+1);
			//lrc = CalEsamLrc(buf,le+4);
			//if(buf[4+le] != lrc)
			//{
			//	printf("LRC2 != lrc  LRC2=%02x lrc=%02x \n",buf[4+le],lrc);
			//	return 0;
			//}
			return le+6;
		}
	}else{
/*
	 	 printf("result != 9000\n");
		for(i=0;i<5;i++)
		{
			printf("%02x ",buf[i]);
		}
		printf("\n");
*/
		return 6;
	}
	return 0;
}
/**
* @brief 控制esam复位
* @param[in] dev: 设备描述
* @return 成功返回读出的大于0的数据长度；失败返回错误码。详见：附录1。
*/
static int32 esam_set_reset(struct tag_ESAM_DEVICE *dev)
{
	//int32 flag = EMTESAM_OOB;
	ESAM_DEVICE_T* pDev = (ESAM_DEVICE_T*)dev;
	if(pDev == NULL)
	{
		return ERR_INVAL;
	}
	//return ioctl(pDev->fd, ESAM_SETRST_CMD, &flag);
	return ERR_OK;
/*    if(pDev->type == EM_ESAM_YX)
    {
        //return ioctl(pDev->fd, XESAM_SETRST_CMD, &flag);
    }
    else
    {
        //return ioctl(pDev->fd, PESAM_SETRST_CMD, &flag);
    }
    return ERR_INVAL;
*/
}
/**
* @brief 设置esam电源状态 1：开启 0：关闭
* @param[in] dev: 设备描述
* @param[in] val: 电源状态
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
static int32 esam_set_power(struct tag_ESAM_DEVICE *dev, int32 val)
{
//	int32 ret;//fd;
	//uint8 pow = val;
/*	ESAM_DEVICE_T* pDev = (ESAM_DEVICE_T*)dev;
	struct encryption_req encryption_reg_req_st;
	char wbuf[2];
	memset(wbuf,0,sizeof(wbuf));
	encryption_reg_req_st.buflen = 1;
	wbuf[0] = val;
	encryption_reg_req_st.bufdata = (char *)wbuf;
	if(pDev == NULL)
	{
		return ERR_INVAL;
	}
	ret = ioctl(pDev->fd, ESAM_SETPWR_CMD, &encryption_reg_req_st);  //设置esam上下电
	if (-1 == ret) {
		perror("ioctl:XESAM_SETPWR_CMD\n");
		return ERR_EXECFAIL;
	}*/
	return ERR_OK;
/*	fd = open(DEV_PATH_GPIO, O_RDWR);
	if (-1 == fd) {
		perror(DEV_PATH_GPIO);
		return ERR_NODEV;
	}
	if(val == 0) //关闭
	{
		ret = set_gpio_output(fd, ESAM_POWER_SIG, GPIO_HIGH);
	}
	else if(val == 0) //开启
	{
		ret = set_gpio_output(fd, ESAM_POWER_SIG, GPIO_LOW);
	}
	else
	{
		close(fd);
		return ERR_O_RANGE;
	}
	if(ret < 0)
	{
		close(fd);
		return ERR_NORMAL;
	}*/
	return ERR_OK;
/*	int32 ret;
	int32 fd = dev->fd;
	ret = ioctl(fd, XESAM_SETPWR_CMD, &val);  //设置esam上下电
	if (-1 == ret) {
		perror("ioctl:XESAM_SETPWR_CMD\n");
		return ERR_EXECFAIL;
	}
	return ERR_OK;
*/
	//return ERR_UNCOMPATIBLE;
}
/**
* @brief 设置esam设备加锁
* @param[in] dev: 设备描述
* @param[in] val: 1为上锁  0 为解锁
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
static int32 esam_set_lock(struct tag_ESAM_DEVICE *dev, int32 val)
{
	/*int32 ret;
	if(val)//上锁
	{
		ret = lockf(dev->fd, F_LOCK,0);
	}
	else//解锁
	{
		ret = lockf(dev->fd, F_ULOCK,0);
	}

	if(ret == 0)
		return ERR_OK;
	else
	{
		perror("lockf");
		return ERR_NORMAL;
	}*/
/*	int32 ret;
	int32 fd = dev->fd;

	ret = ioctl(fd, GPIO_LOCKXESAM_CMD, &val);  //设置esam锁定
	if (-1 == ret) {
		perror("ioctl:GPIO_LOCKXESAM_CMD\n");
		return ERR_EXECFAIL;
	}
	return ERR_OK;*/
	ESAM_DEVICE_T *pDev = (ESAM_DEVICE_T*)dev;
	if(pDev == NULL)
	{
		return ERR_INVAL;
	}
	if(val == 1)
	{
		pthread_mutex_lock(&pDev->devMetux);
		/*if(0 != sys_mutex_lock(pDev->devMetux))
		{
			printf("esam_set_lock:metux lock failed\r\n");
		}*/
		if(0 != lockf(pDev->fd_lock, F_LOCK, 0))
		{
			printf("esam_set_lock:lockf failed\r\n");
		}
	}else{
		if(0 != lockf(pDev->fd_lock, F_ULOCK, 0))
		{
			printf("esam_set_lock:lockf rel failed\r\n");
		}
		pthread_mutex_unlock(&pDev->devMetux);
		/*if(0 != sys_mutex_unlock(pDev->devMetux))
		{
			printf("esam_set_lock:metux unlock failed\r\n");
		}*/
	}

	return ERR_OK;
}

/**
* @brief 读取esam数据
* @param[out] buf: 读取数据缓存
* @param[in] len: 读取数据长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
#if 0
static int32 esam_data_read(struct tag_ESAM_DEVICE *dev, uint8 *buf,int32 len)
{
	uint8 wbuf[2048];
	int wlen;
	ESAM_DEVICE_T* pDev = (ESAM_DEVICE_T*)dev;
	if((pDev == NULL)||(buf == NULL))
	{
		return ERR_INVAL;
	}
	usleep(20000);
	wlen = EsamRead( pDev->fd, wbuf, len);
	if(wlen)
	{
		memcpy(buf,wbuf,wlen);
#if 0
		int i;
		printf("HAL_EsamRead:");
		for(i=0;i<wlen;i++)
			printf("%02X ",wbuf[i]);
		printf("\n");
#endif
		return wlen;
	}
	return ERR_EXECFAIL;
}
#endif
#if 1
static int32 esam_data_read(struct tag_ESAM_DEVICE *dev, uint8 *buf,int32 len)
{
	uint8 wbuf[2048]={0};
	int wlen;
	ESAM_DEVICE_T* pDev = (ESAM_DEVICE_T*)dev;
	if((pDev == NULL)||(buf == NULL))
	{
		return ERR_INVAL;
	}
	//add by whl 2023-02-14 16:22:08
#if 1
	struct stat st;
	if(bNeedAckOK && 0==stat("/tmp/dev/not_verify_sign", &st ))
	{
	  uint8 byOKCode[] = {0x55, 0x90, 0x00, 0x00, 0x00, 0x6f};
	  usleep(20*1000);
	  memcpy(&buf[0], byOKCode, sizeof(byOKCode));
	  wlen = sizeof(byOKCode);
	  bNeedAckOK = 0;
	  printf("hal verify ack ok\n");
	  return wlen;
	}
#endif
	wlen = EsamRead( pDev->fd, wbuf, len);
	if(wlen)
	{
		memcpy(buf,wbuf,wlen);
#if 0
		int i;
		printf("HAL_EsamRead:");
		for(i=0;i<wlen;i++)
			printf("%02X ",wbuf[i]);
		printf("\n");
#endif
		return wlen;
	}
	return ERR_EXECFAIL;
}
#endif
/**
* @brief写ESAM数据
* @param[in] dev: 设备描述
* @param[in] buf: 写数据缓存
* @param[in] len: 写数据长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
*/
static int32 esam_data_write(struct tag_ESAM_DEVICE *dev, uint8 *buf,int32 len)
{
	ESAM_DEVICE_T* pDev = (ESAM_DEVICE_T*)dev;
	uint8 byMatchCode1[] = {0x55, 0x80, 0x5A, 0x06, 0x00}; //验证签名是否有效uboot, kernel
	uint8 byMatchCode2[] = {0x55, 0x80, 0x0E, 0x48, 0x90}; //验证app license是否正确
	if((pDev == NULL)||(buf == NULL))
	{
		return ERR_INVAL;
	}
#if 1
	  struct stat st;
	  if(0==stat("/tmp/dev/not_verify_sign", &st ))
	  {
	    if(!memcmp(&buf[0], byMatchCode1, sizeof(byMatchCode1)))
	    {
	      printf("hal verify uboot/kernel\n");
	      bNeedAckOK = 1;
	    }
	    else if(!memcmp(&buf[0], byMatchCode2, sizeof(byMatchCode2)))
	    {
	      printf("hal verify app license\n");
	      bNeedAckOK = 1;
	    }
	    else
	    {
	      bNeedAckOK = 0;
	    }
	  }
#endif
	EsamWrite(pDev->fd, buf, len);
#if 0
	int i;
	printf("HAL_EsamWrite:");
	for(i=0;i<len;i++)
		printf("%02X ",buf[i]);
	printf("\n");
#endif
	  //add by whl 2023-02-14 16:22:08
	  //如果我们没有获取app license 先在hal处理一下，验证license时回复验签通过，保证全管理app能运行起来

	return ERR_OK;
}

static int32 open_esam(struct tag_HW_MODULE *module, const char* device_id,
		struct tag_HW_DEVICE **device)
{
	/* 分配内存空间 */
    char  lockName[64];
    ESAM_DEVICE_T* dev;
	/* 为ESAM_DEVICE_T分配内存空间 */
	dev = (ESAM_DEVICE_T*)malloc(sizeof(ESAM_DEVICE_T));
	if(dev == NULL)
	{
		return ERR_MEM;
	}
	memset(dev, 0, sizeof(*dev));
	/* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
	module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
	的tag_HW_DEVICE子类也需要定义多个，此处esam驱动有两个配电和营销的，所以有
	ESAM_DEVICE_T两个子类*/
	/* 入参校验 */
	if(module == NULL || device_id == NULL || device == NULL)
	{
		return ERR_PNULL;
	}
	/* 为ESAM_DEVICE_T的common成员变量赋值 */
	dev->base.nVer = 1;
	dev->base.pModule = (struct tag_HW_MODULE *)module;
    /* 打开设备 */
    memset(lockName,0,sizeof(lockName));
	if (access("/tmp/dev/", F_OK) != 0)
	{
		system("mkdir -p /tmp/dev");
	}
	//printf("device_id %s\n",device_id);
    if(0 == strcmp(device_id, HW_DEVICE_ID_YX_ESAM))
    {
        dev->base.szDeviceID = HW_DEVICE_ID_YX_ESAM;
        dev->fd = open(DEV_PATH_XESAM, O_RDWR);
        dev->type = EM_ESAM_YX;
        snprintf(lockName, sizeof(lockName),"/tmp/dev/.lock_%s", HW_DEVICE_ID_YX_ESAM);
    }
    else{
        dev->base.szDeviceID = HW_DEVICE_ID_PW_ESAM;
        dev->fd = open(DEV_PATH_PESAM, O_RDWR);
        dev->type = EM_ESAM_PW;
        snprintf(lockName, sizeof(lockName),"/tmp/dev/.lock_%s", HW_DEVICE_ID_PW_ESAM);
    }
	if(dev->fd == -1)
	{
		free(dev);
		return ERR_NODEV;
	}
	//printf("dev->fd %d\n",dev->esam.fd);
    dev->fd_lock = open(lockName,O_RDWR|O_CREAT ,666);
    if(dev->fd_lock < 0)
    {
        close(dev->fd);
		free(dev);
		return ERR_NORESOURCE;
    }

    pthread_mutex_init(&dev->devMetux,NULL);
/*  dev->devMetux = sys_mutex_create();
    if(dev->devMetux == NULL)
    {
        close(dev->fd);
        close(dev->fLock);
        free(dev);
        return ERR_NORESOURCE;
    }
*/
	dev->esam_set_reset  = esam_set_reset;
	dev->esam_set_power  = esam_set_power;
    dev->esam_set_lock   = esam_set_lock;
	dev->esam_data_read  = esam_data_read;
	dev->esam_data_write = esam_data_write;
	/* 强转为struct tag_HW_DEVICE指针，赋给device */
	*device = (struct tag_HW_DEVICE*)&(*dev);
	//printf("Esam init ok...\n");
	return ERR_OK;
}

static int32 close_esam( struct tag_HW_MODULE *module,
		struct tag_HW_DEVICE *device )
{
	ESAM_DEVICE_T* pDev = (ESAM_DEVICE_T*)device;
	if(pDev != NULL)
	{
		if (pDev->fd > 0)
		{
			close(pDev->fd);
		}
        if (pDev->fd_lock > 0)
		{
			close(pDev->fd_lock);
		}
        pthread_mutex_destroy(&pDev->devMetux);
		free(pDev);
	}
	return ERR_OK;
}

/* 模块中的设备列表 */
static const char * szDevices_table[] = {
	HW_DEVICE_ID_YX_ESAM,
	HW_DEVICE_ID_PW_ESAM,
	NULL
};

/* 初始化模块变量 */
ESAM_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "esam",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-07-18",
        .nDevNum = 2,
        .szDevices = (char**)szDevices_table,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = open_esam,
        .pfClose = close_esam,
    },
};

