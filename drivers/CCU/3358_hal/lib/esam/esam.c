#include "types.h"
#include <esam.h>
#include <gpio.h>
#include "sysDefine.h"
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include <stdarg.h>

//#define     ESAM_1187           1

/*esam 设备路径 */
//  ESAM_1187
#define PW_ESAM_DEV_NAME           "/dev/encryption0" 
#define PW_ESAM_FILE               PW_ESAM_DEV_NAME
#define PW_TESAM_IOC_MAGIC         'x'

//1168
#define ESAM_DEV_NAME              "/sys/bus/spi/devices/spi32766.0/tesam"
#define ESAM_FILE                  ESAM_DEV_NAME 
#define TESAM_IOC_MAGIC            'y'

//文件锁和安全加固APP设备
#define ESAM_LOCK_FILE          "/tmp/dev/esam_lck"

#define ESAM_GET_DATA		    _IOW('E', 0x01, struct encryption_reg_req)
#define ESAM_SET_DATA		    _IOW('E', 0x02, struct encryption_reg_req)

#define TESAM_IOC_SET_CS          _IOW(TESAM_IOC_MAGIC, 5, unsigned char)
#define TESAM_IOC_SET_PWR         _IOW(TESAM_IOC_MAGIC, 6, unsigned char)
#define TESAM_IOC_SET_LOCK        _IOW(TESAM_IOC_MAGIC, 7, unsigned char)

#define TESAM_PWR_PIN           AT91_PIN_PA19
#define TESAM_CS_PIN            AT91_PIN_PA20

#define CHECK_STATUS_MAX_COUNT 	    100000	
#define START_FLAG_VALUE	    0x55

#define UNLOCK                      0
#define ENLOCK                      1


struct encryption_reg_req {
    int buflen;
    char* bufdata;
};


#define DATA_55H        				0x55
#define SW1_VAL         				0x90
#define SW2_VAL         				0x00
#define SW1_POS         				1
#define SW2_POS         				2
#define LEN1_POS        				3
#define LEN2_POS        				4
#define DATA_START_POS  			    5
#define LCR_OFFSET      				5       /* data len + LCR_OFFSET = LCR pos */
#define LCR_LEN         			    1       /* 1 byte */


#define ESAM_DEV_NUM                    2
static char *aEsamDevice[ESAM_DEV_NUM] = {HW_DEVICE_ID_YX_ESAM,
                                          HW_DEVICE_ID_PW_ESAM};

//文件
static int lockfd = -1;

extern void CsActivaten(int fd);
extern void CsDeactivate(int fd);


/***************************************************
* @brief 读取esam数据
* @param[out] pBuf: 读取数据缓存
* @param[in] iLen: 读取数据长度
* @return成功返回ERR_OK或接收的长度；失败返回错误码。详见：附录1�?
****************************************************/
static int32 esam_read_data(struct tag_ESAM_DEVICE *dev, uint8 *pBuf, int32 iLen)
{
    int32 fd = 0;
    int32 i = 0;
    int32 nRet = 1;
    int32 iRet = ERR_NORMAL;
    uint16 dataLen = 0;
    uint8 tmpBuf[10] = {0};
    int32 count = 100;

    /* Set CS activate status */
       while(((fd = open(ESAM_DEV_NAME, O_RDWR|0666))<0)&&((count--)>0))
            usleep(10000);
    
    if(fd < 0)
    {
        DEBUG("esam_read_data:cannot open device esam\n");
        return iRet;
    }
    
    if(flock(fd, LOCK_EX))
    {
        DEBUG("esam_read_data: lock Error");
        iRet = ERR_LOCK;
        goto end;
    }
    
    CsActivaten(fd);
    while (i++ < CHECK_STATUS_MAX_COUNT)           //check 0x55 in 3s
    {
        read(fd, pBuf, 1);
        if (*pBuf == START_FLAG_VALUE)
            break;
        else
            usleep(15);
    }
        
    if ((i >= CHECK_STATUS_MAX_COUNT) && (*pBuf!= START_FLAG_VALUE))
    {
        DEBUG(" ��ʱû���յ�0x55\n");
        goto time_out;
    }
    
    for (i = 1; i < 5; i++)
        nRet += read(fd, pBuf+i, 1);

    tmpBuf[0] = pBuf[4];
    tmpBuf[1] = pBuf[3];         // 数据长度
    dataLen = 0;
    memcpy((char *)&(dataLen), tmpBuf, sizeof(uint16));

    //DEBUG("read dataLen = %d\n", dataLen);
    nRet += read(fd, pBuf+i, dataLen+1);


time_out:
    CsDeactivate(fd);  /* Set CS deactivate status */
    flock(fd, LOCK_UN);
end:
    close(fd);
  
    iRet = (nRet >= 0)?nRet : ERR_NORMAL;
    return iRet;
}


/***************************************************
* @brief写ESAM数据
* @param[in] dev: 设备描述
* @param[in] pBuf: 写数据缓�?
* @param[in] iLen: 写数据长�?
* @return成功返回ERR_OK；失败返回错误码。详见：附录1�?
****************************************************/
int32 esam_write_data(struct tag_ESAM_DEVICE *dev, uint8 *pBuf, int32 iLen)
{
    int32 fd = 0;
    int32 iRet = ERR_NORMAL;
    uint8 start_flag = START_FLAG_VALUE;
    int32 count = 100;

    /* Set CS activate status */
    while(((fd = open(ESAM_DEV_NAME, O_RDWR|0666))<0)&&((count--)>0))
            usleep(10000);
    
    if(fd < 0)
    {
        DEBUG("cannot open device esam[%s]\n", ESAM_DEV_NAME);
        return  ERR_NORMAL;
    }
  
    if(flock(fd, LOCK_EX))
    {
        DEBUG("esam lock Error");
        iRet = ERR_LOCK;
        goto end;
    }

    CsActivaten(fd); 
    
    if(pBuf[0] != START_FLAG_VALUE)
    {
        iRet = write(fd, &start_flag, 1);
        if(iRet <= 0)
            DEBUG("esam read eeror\n");
    
    }  	 
  
    iRet = write(fd, pBuf, iLen);
    if(iRet <= 0)
    {
        DEBUG("esam read eeror\n");
    }

    iRet = (iRet == iLen)?ERR_OK : ERR_NORMAL;

    CsDeactivate(fd);  /* Set CS activate status */
    flock(fd, LOCK_UN);
    
end:
    close(fd);

    return iRet;
}


/****************************************************
* @brief 控制esam GPIO
* @param[in] cmd: 命令 
* @param[in] value: �?
* @return 成功返回ERR_OK；失败返回错误码。详见：附录1�?
****************************************************/
int32 esam_set_gpio_value(unsigned pin, uint32 value)
{
    int fd = 0;
    int iRet = 0;
    int count = 100;

    while(((fd = open(GPIO_DEV_NAME, O_RDONLY|0666))<0)&&((count--)>0))
            usleep(10000);

    if (fd <= 0)
    {
        DEBUG("can not open device.\n");
        return ERR_NORMAL;
    }

    ///printf("start flock\n");
    if(flock(fd, LOCK_EX))
    {
        DEBUG("esam flock Error\n");
        iRet = ERR_LOCK;
        goto end;
    }

    ///printf("start ioctl\n");
    iRet = ioctl(fd,  value, pin);

    //printf("end flock ioctl %d\n",iRet);
    flock(fd, LOCK_UN);	
    iRet = (iRet >= 0)?ERR_OK : ERR_NORMAL;

end:
    close(fd);

    return iRet;
}


void SetTesamPowerOn()
{
   esam_set_gpio_value(TESAM_PWR_PIN, GPIO_OUT_LOW);
}


void SetTesamPowerOff()
{
    esam_set_gpio_value(TESAM_PWR_PIN, GPIO_OUT_HIGH);

}


/* tesam 片�?�引脚控制函�? */
void CsActivaten(int fd)
{
    esam_set_gpio_value(TESAM_CS_PIN, GPIO_OUT_LOW);

}


/* tesam 片�?�引脚控制函�? */
void CsDeactivate(int fd)
{
    esam_set_gpio_value(TESAM_CS_PIN, GPIO_OUT_HIGH);

}

/* tesam 计算校验函数 */
uint8 CalLRC(uint8* pBuffer, int nLen)
{
    uint8 ch = 0x0;
    int  i  = 0;

    if(pBuffer != NULL)
    {
        for(i=0; i<nLen; i++)
           ch ^= pBuffer[i];

        ch = ~ch;
    }
    else
        DEBUG("pBuffer is NULL\n");

    return ch;
}

/****************************************************
* @brief 控制esam复位
* @param[in] dev: 设备描述
* @return 成功返回ERR_OK；失败返回错误码。详见：附录1�?
****************************************************/
static int32 esam_set_reset (struct tag_ESAM_DEVICE *dev)
{
    if(strcmp(dev->base.szDeviceID, HW_DEVICE_ID_YX_ESAM) == 0)
    {
        SetTesamPowerOff();
        sleep(1); 
        SetTesamPowerOn();
        sleep(1);
    }
   
    return ERR_OK;
}

/***************************************************
* @brief 设置esam电源状�??
* @param[in] dev: 设备描述
* @param[in] state: 在ESAM_PWR_STATE定义，PWR_OFF 关闭 PWR_ON 打开
* @return 成功返回ERR_OK；失败返回错误编�?
* @modfy: 2020-02-24
****************************************************/
static int32 esam_set_power(struct tag_ESAM_DEVICE *dev, int32 state)
{
   //DEBUG("esam_set_power %s %d\n", dev->base.szDeviceID,state);

   if(strcmp(dev->base.szDeviceID, HW_DEVICE_ID_YX_ESAM) == 0)
   {
       switch(state)
       {
           case PWR_ON:
               esam_set_reset(dev);
               SetTesamPowerOn();
               break;
        
           case PWR_OFF:
               SetTesamPowerOff();
               break;
        
           default:
               break;
        }
    }	

    return ERR_OK;
}

/**********************************************************************
* @name      : esam_set_wrlock
* @brief     ：设置esam设备加锁
* @param[in] ：ESAM_DEVICE_T *dev  设备
* @param[out]�?
* @return    �?0-成功/错误�?
**********************************************************************/
int esam_set_wrlock(ESAM_DEVICE_T *dev)
{
    if (dev == NULL)
    {
        return -1;
    }

    dev->fd_lock = open(ESAM_LOCK_FILE, O_CREAT |O_WRONLY | O_APPEND, 0664); 
    if (dev->fd_lock < 0)
    {
        return -2;
    }

    flock(dev->fd_lock, LOCK_EX);
    
    return 0;
}

/**********************************************************************
* @name      : esam_set_unlock
* @brief     ：设置esam设备加锁
* @param[in] ：ESAM_DEVICE_T *dev  设备
* @param[out]�?
* @return    �?0-成功/错误�?
**********************************************************************/
int esam_set_unlock(ESAM_DEVICE_T *dev)
{
    if (dev == NULL)
    {
        return -1;
    }

    if (dev->fd_lock < 0)
    {
        return -2;
    }

    flock(dev->fd_lock, LOCK_UN); //Unlock
    close(dev->fd_lock);
    dev->fd_lock = -1;
    
    return 0;
}


/***********************************************************
* @brief 设置esam设备加锁
* @param[in] dev: 设备描述
* @param[in] val: 1为上�?  0 为解�?
* @return 成功返回ERR_OK；失败返回错误编�?
************************************************************/
int32  esam_set_lock(struct tag_ESAM_DEVICE *dev, int32 val)
{
	//DEBUG("esam_set_lock %d\n",val);
	
    if (val == 0)
    {
        return esam_set_unlock(dev);
    }
    else if (val == 1)
    {
        return esam_set_wrlock(dev);
    }
    else
    {
        return -3;
    }
}

static int32 sc1187_EsamWrite(int32 fd,uint8 *buf,int32 len)
{
    struct encryption_reg_req encryption_reg_req_st;
    int iRet = ERR_OK;

    encryption_reg_req_st.buflen = len;
    encryption_reg_req_st.bufdata = (char *)buf;
    iRet = ioctl(fd, ESAM_SET_DATA, &encryption_reg_req_st);

    iRet = (iRet >= 0)?ERR_OK : ERR_NORMAL;
    
    return iRet;
}

static int32 sc1187_EsamRead(int fd, uint8 *buf, int32 len)
{
    int iRet = 0;
    struct encryption_reg_req encryption_reg_req_st;

    encryption_reg_req_st.buflen = len;
    encryption_reg_req_st.bufdata = (char *)buf;
    iRet = ioctl(fd, ESAM_GET_DATA, &encryption_reg_req_st);

    iRet = (iRet >= 0)?ERR_OK : ERR_NORMAL;

    return iRet;
}

/***************************************************
* @brief 读取esam数据
* @param[out] pBuf: 读取数据缓存
* @param[in] iLen: 读取数据长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录1�?
****************************************************/
int32 sc1187_send(struct tag_ESAM_DEVICE *dev, uint8* pBuffer, int32 nLen)
{
     int32 fd = 0;
     int32 iRet = ERR_OK;
     
     fd  = open(PW_ESAM_FILE, O_RDWR|0666);
     if(fd < 0)
     {
         DEBUG("/dev/encryption0 fd=%d\n",fd);
         return ERR_NORMAL;
     }
     
    iRet = sc1187_EsamWrite(fd, pBuffer, nLen);
    
    close(fd);

  return iRet;
}

/***************************************************
* @brief 读取esam数据
* @param[out] pBuf: 读取数据缓存
* @param[in] iLen: 读取数据长度
* @return成功返回ERR_OK或接收的长度；失败返回错误码。详见：附录1�?
****************************************************/
int32 sc1187_recv(struct tag_ESAM_DEVICE *dev, uint8*buf, int32 len)
{
    //int32 i = 0;
    //char lrc = 0;
    int32 iDataLen = 0;
    int32 fd = 0;
    int32 iRet = ERR_OK;

    fd  = open(PW_ESAM_FILE, O_RDWR|0666);
    if(fd < 0)
    {
         DEBUG("/dev/encryption0 fd=%d\n",fd);
         return ERR_NORMAL;
    }
     
    iRet =  sc1187_EsamRead(fd, buf, 4);
    if((buf[0] == SW1_VAL) &&
        (buf[1] == SW2_VAL))
    {
        iDataLen = (buf[2]<<8) | buf[3];
        if(iDataLen <= (len-5))
        {
            iRet = sc1187_EsamRead(fd, buf, iDataLen+4+1);  //4:90 00 le(high) le(low) 1：需要读出检验字�?         
        }
    }

     close(fd);

     return iRet ;
}


static int esam_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一�?
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私�?
    的tag_HW_RTC子类也需要定义多�? */
    
    ESAM_DEVICE_T* dev = (ESAM_DEVICE_T*)malloc(sizeof(ESAM_DEVICE_T));
    if(dev == NULL)
    {
        return -1;
    }
    memset(dev, 0, sizeof(ESAM_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;

    dev->esam_set_reset = esam_set_reset;
    dev->esam_set_power = esam_set_power;
    dev->esam_set_lock = esam_set_lock;
    if(strcmp(device_id, HW_DEVICE_ID_YX_ESAM) == 0)
    {
        dev->esam_data_read = esam_read_data;
        dev->esam_data_write = esam_write_data;
    }
    else
    {
        dev->esam_data_read = sc1187_recv;
        dev->esam_data_write = sc1187_send;
    }
    //dev->esam_check_mxdx = esam_check_mxdx;

    
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int esam_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    ESAM_DEVICE_T *dev = (ESAM_DEVICE_T*)pDevice;
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变�? */
ESAM_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "esam",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-03-20",
        .nDevNum = ESAM_DEV_NUM,
        .szDevices = aEsamDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入�? */
        .pfOpen = esam_open,
        .pfClose = esam_close,
    },
};

