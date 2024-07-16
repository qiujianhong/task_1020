#include <hal.h>
#include <eeprom.h>
#include <string.h>

/*eeprom 设备路径 */
//#define EEPROM_DEV_FILE      "/sys/bus/i2c/devices/2-0050/eeprom"   /*eeprom映射成的文件 */
//#define EEPROM_DEV_NAME      "/dev/i2c-0"   /*eeprom设备 */
#define EEPROM_DEV_FILE        "/dev/eeprom"

#define EEPROM_DEV_NUM      1

static char *aEepromDevice[EEPROM_DEV_NUM] = {HW_DEVICE_ID_EEPROM};

/**********************************************************
* @brief 写数据
* @param[in] dev: 设备描述
* @param[in] pBuffer: 数据缓存
* @param[in] nOffest: 起始偏移地址
* @param[in] nLength: 写入数据长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
***********************************************************/
static int32 eeprom_write(EEPROM_DEVICE_T *dev, uint8* pBuffer, 
          uint32 nOffest, uint32 nLength)
{
    int32 iRet = 0;
    int32 fd = 0;
    int32 i,n;
    int32 seg_cnt;    //按512字节为一段，要写的数据分为seg_cnt段
    int32 remainder;  //分段后剩余的字节数
    
    //写缓存不能为空
    if (pBuffer == NULL) {
        DEBUG("Write buffer is NULL.\n");
        return ERR_PTR;
    }
    //写数据的范围是8K以内
    if (nOffest + nLength < 1 || nOffest + nLength > 8192) {
        DEBUG("Write eeprom out of range(should be in 8K).\n");
        return ERR_NORMAL;
    }

    fd = open(EEPROM_DEV_FILE, O_RDWR, 0666);
    if(fd == -1) 
    {
        DEBUG("eeprom dev open error\n");
        return ERR_NORMAL;
    }
    //设置写锁
    if( flock(fd, LOCK_EX) ){
       DEBUG("Lock Error");
	   iRet = ERR_LOCK;
        goto errexit;
    }

    usleep(10000);		

    seg_cnt = nLength / 512;
    remainder = nLength % 512;

    //每512字节一写
    for (i = 0; i < seg_cnt; i++) 
    {
        //定位
        if (lseek(fd, nOffest+(i*512), SEEK_SET) < 0) {
            DEBUG("eeprom locate error.\n");
	     iRet = ERR_IO;
            goto errexit;
        }
        //写入
        n = write(fd, pBuffer+(i*512), 512);
        if (n < 0) {
            DEBUG("eeprom write error.\n");
	     iRet = ERR_IO;
            goto errexit;
        } 
        else if (n<512 && n>=0){
            DEBUG("In segment %d write eeprom less then expectation.\n",i);
            break;
        }
        iRet += n;	//记录实际读到的字节数作为函数返回值
    }

    usleep(10000);	
    //写分段后的剩余部分 
    if ( (i == seg_cnt) ) 
    {
        //定位
        if (lseek(fd, nOffest+(seg_cnt*512), SEEK_SET) < 0) {
            DEBUG("eeprom locate error.\n");
	     iRet = ERR_IO;
            goto errexit;
        }
        //写入
        n = write(fd, pBuffer+(seg_cnt*512), remainder);
        if (n  < 0) {
            DEBUG("eeprom write error.\n");
	     iRet = ERR_IO;
            goto errexit;
        } 
        else if (n<remainder && n>=0){
            DEBUG("In remainder write eeprom less then expectation.\n");
        }
		
        iRet += n;
    }

    flock(fd, LOCK_UN);
    close(fd);
	iRet = (iRet >= 0)?ERR_OK : ERR_NORMAL;
    return iRet;

errexit:
    flock(fd, LOCK_UN);
    close(fd);
    return  iRet;
}

/**********************************************************
* @brief 读数据
* @param[in] dev: 设备描述
* @param[out] pBuffer: 缓冲区指针
* @param[in] nOffest: 起始偏移地址
* @param[in] nLength: 缓冲区长度
* @return 成功返回读出的大于0的数据长度；失败返回错误码
***********************************************************/
static int eeprom_read(EEPROM_DEVICE_T *dev, uint8* pBuffer, 
             uint32 nOffest, uint32 nLength)
{
    int32 iRet = ERR_OK;
    int32 fd = 0;

	//读缓存不能为空
	if (pBuffer == NULL) 
    {
        printf("Eeprom read buffer is NULL.\n");
        return ERR_PTR;
    }
	
	//读取的数据要在8K以内
	if (nOffest + nLength < 1 || nOffest + nLength > 8192) 
    {
		printf("Read eeprom out of range.\n");
		return ERR_NORMAL;
	}

    fd = open(EEPROM_DEV_FILE, O_RDWR, 0666);
    if(fd == -1) 
    {
        printf("eeprom dev open error\n");
        return ERR_NORMAL;
    }
	
    if(flock(fd, LOCK_EX))
    {
        printf("eeprom lock Error");
		iRet = ERR_LOCK;
        goto errexit;
    }
    
	usleep(10000);	

	int i,n;
	int seg_cnt;	//按512字节为一段，要读的数据分为seg_cnt段
	int remainder;	//分段后剩余的字节数
	seg_cnt = nLength / 512;
	remainder = nLength % 512;

	//每512字节一读取
	for (i=0; i<seg_cnt; i++) 
	{
		//定位
		if (lseek(fd, nOffest+(i*512), SEEK_SET) < 0) {
			printf("eeprom locate error.\n");
			goto errexit;
		}
		//读取
		n = read(fd, pBuffer+(i*512), 512);
		if (n < 0) 
        {
			printf("eeprom read error.\n");
			goto errexit;
		} 
        else if (n<512 && n>=0)
        {
			printf("In segment %d read eeprom less then expectation.\n",i);
                        break;
		}
		
		iRet += n;	//记录实际读到的字节数作为函数返回值 		
	}

	usleep(10000);
	//读分段后的剩余部分
	if ( (i == seg_cnt) ) 
	{
		//定位
		if (lseek(fd, nOffest+(seg_cnt*512), SEEK_SET) < 0) {
			printf("eeprom locate error.\n"); 
			goto errexit;
		}
		//读取
		n = read(fd, pBuffer+(seg_cnt*512), remainder);
		if (n  < 0) { 
			printf("eeprom read error.\n");
			goto errexit;
		} 
        else if (n<remainder && n>=0){
			printf("In remainder read eeprom less then expectation.\n");
		}
		
		iRet += n;
	}

	flock(fd, LOCK_UN);
    close(fd);
	return iRet;

errexit:
    flock(fd, LOCK_UN);
    close(fd);
    return ERR_NORMAL;
}


/**********************************************************
* @brief 设置芯片写保护
* @param[in] dev: 设备描述
* @param[in] wp_enable: 1: 打开写保护， 0: 关闭写保护
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
***********************************************************/
static int eeprom_write_protect_enable(EEPROM_DEVICE_T *dev, 
    unsigned char wp_enable)
{
    int32 fd = 0;
    int32 iRet = ERR_OK;
    
	//检验输入参数
	if (wp_enable != 1 && wp_enable != 0) {
    	DEBUG("WriteProtect param error(should be 1 or 0).\n");
    	return ERR_NORMAL;
	}

    fd = open(EEPROM_DEV_FILE, O_RDWR);
    if(fd == -1) 
    {
        DEBUG("eeprom dev open error\n");
        return ERR_NORMAL;
    }
    if(flock(fd, LOCK_EX))
    {
        DEBUG("Lock Error");
	   iRet = ERR_LOCK;
        goto end;
    }
    
	//写保护或去保护
    ioctl(fd, wp_enable);
    flock(fd, LOCK_UN);
end:

    close(fd);
    return iRet;
}


/**********************************************************
* @brief 获取设备大小
* @param[in] dev: 设备描述
* @return 成功返回设备大小；失败返回FALSE
***********************************************************/
static int eeprom_get_devsize(EEPROM_DEVICE_T *dev)
{
    return 8192;
}


static int eeprom_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_DEVICE子类也需要定义多个 */
    
    EEPROM_DEVICE_T* dev = (EEPROM_DEVICE_T*)malloc(sizeof(EEPROM_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
	
    memset(dev, 0, sizeof(EEPROM_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = HW_DEVICE_ID_EEPROM;
    dev->base.pModule = (HW_MODULE *)module;    

    dev->eeprom_write = eeprom_write;
    dev->eeprom_read = eeprom_read;
	dev->eeprom_devsize_get = eeprom_get_devsize;
    dev->eeprom_write_protect_enable = eeprom_write_protect_enable;
   
    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int eeprom_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    EEPROM_DEVICE_T *dev = (EEPROM_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
EEPROM_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "eeprom",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-18",
        .nDevNum = EEPROM_DEV_NUM,
        .szDevices = aEepromDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = eeprom_open,
        .pfClose = eeprom_close,
    },
};

