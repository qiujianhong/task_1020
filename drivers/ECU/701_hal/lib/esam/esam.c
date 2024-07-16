#include <esam.h>

//#define     ESAM_1187           1

/*esam 设备路径 */
//  ESAM_1187
#define PW_ESAM_DEV_NAME           "/dev/encryption0" 
#define PW_ESAM_FILE                     PW_ESAM_DEV_NAME
#define PW_TESAM_IOC_MAGIC               'x'

//1168
#define ESAM_DEV_NAME                    "/dev/tesam"
#define ESAM_FILE                        ESAM_DEV_NAME 
#define TESAM_IOC_MAGIC               'y'

//文件锁和安全加固APP设备
#define ESAM_LOCK_FILE          "/tmp/dev/esam_lck"
#define SECURITY_APP_DEST       "/tmp/dev/smiSecurity"

#define ESAM_GET_DATA		    _IOW('E', 0x01, struct encryption_reg_req)
#define ESAM_SET_DATA		    _IOW('E', 0x02, struct encryption_reg_req)

#define TESAM_IOC_SET_CS          _IOW(TESAM_IOC_MAGIC, 5, unsigned char)
#define TESAM_IOC_SET_PWR      _IOW(TESAM_IOC_MAGIC, 6, unsigned char)
#define TESAM_IOC_SET_LOCK     _IOW(TESAM_IOC_MAGIC, 7, unsigned char)

#define PW_ESAM_IOC_SET_CS          _IOW(PW_TESAM_IOC_MAGIC, 5, unsigned char)
#define PW_ESAM_IOC_SET_PWR      _IOW(PW_TESAM_IOC_MAGIC, 6, unsigned char)
#define PW_ESAM_IOC_SET_LOCK     _IOW(PW_TESAM_IOC_MAGIC, 7, unsigned char)

#define TESAM_PWR_PIN              TESAM_IOC_SET_PWR
#define TESAM_CS_PIN                 TESAM_IOC_SET_CS

#define CHECK_STATUS_MAX_COUNT 	100000	
#define START_FLAG_VALUE		       0x55

#define UNLOCK                             0
#define ENLOCK                              1


struct encryption_reg_req {
    int buflen;
    char* bufdata;
};

#define GPIO_OUT_HIGH                        1
#define GPIO_OUT_LOW                         0

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

//文件锁
static int lockfd = -1;

extern void CsActivaten(int fd);
extern void CsDeactivate(int fd);


/***************************************************
* @brief 读取esam数据
* @param[out] pBuf: 读取数据缓存
* @param[in] iLen: 读取数据长度
* @return成功返回ERR_OK或接收的长度；失败返回错误码。详见：附录1。
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
        nRet += read(fd, pBuf, 1);
        if (*pBuf == START_FLAG_VALUE)
            break;
        else
            usleep(15);
    }
        
    if ((i >= CHECK_STATUS_MAX_COUNT) && (*pBuf!= START_FLAG_VALUE))
    {
        DEBUG(" 规定时间内未找到0x55\n");
        goto time_out;
    }
    
    for (i = 1; i < 5; i++)
        nRet += read(fd, pBuf+i, 1);

    tmpBuf[0] = pBuf[4];
    tmpBuf[1] = pBuf[3];         // 数据长度
    dataLen = 0;
    memcpy((char *)&(dataLen), tmpBuf, sizeof(uint16));

    DEBUG("read dataLen = %d\n", dataLen);
    nRet += read(fd, pBuf+i, dataLen+1);

    iRet = nRet;
time_out:
    CsDeactivate(fd);  /* Set CS deactivate status */
    flock(fd, LOCK_UN);
end:
    close(fd);
  
    iRet = (iRet >= 0)?ERR_OK : ERR_NORMAL;
    return iRet;
}


/***************************************************
* @brief写ESAM数据
* @param[in] dev: 设备描述
* @param[in] pBuf: 写数据缓存
* @param[in] iLen: 写数据长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
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
        DEBUG("cannot open device esam\n");
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
* @param[in] value: 值
* @return 成功返回ERR_OK；失败返回错误码。详见：附录1。
****************************************************/
int32 esam_set_gpio_value(unsigned cmd, uint32 value)
{
    int fd = 0;
    int iRet = 0;
    int count = 100;

    while(((fd = open(ESAM_DEV_NAME, O_RDONLY|0666))<0)&&((count--)>0))
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
    iRet = ioctl(fd,  cmd, value);

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


/* tesam 片选引脚控制函数 */
void CsActivaten(int fd)
{
    ////esam_set_gpio_value(TESAM_CS_PIN, GPIO_OUT_LOW);
    if(fd>0)
        ioctl(fd, TESAM_CS_PIN, GPIO_OUT_LOW);
}


/* tesam 片选引脚控制函数 */
void CsDeactivate(int fd)
{
    ////esam_set_gpio_value(TESAM_CS_PIN, GPIO_OUT_HIGH);
    if(fd>0)
        ioctl(fd, TESAM_CS_PIN, GPIO_OUT_HIGH);
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
* @return 成功返回ERR_OK；失败返回错误码。详见：附录1。
****************************************************/
static int32 esam_set_reset (struct tag_ESAM_DEVICE *dev)
{
    if(strcmp(dev->base.szDeviceID, HW_DEVICE_ID_YX_ESAM) == 0)
    {
        SetTesamPowerOff();
        usleep(500000); 
        SetTesamPowerOn();
        usleep(500000);
    }
   
    return ERR_OK;
}

/***************************************************
* @brief 设置esam电源状态
* @param[in] dev: 设备描述
* @param[in] state: 在ESAM_PWR_STATE定义，PWR_OFF 关闭 PWR_ON 打开
* @return 成功返回ERR_OK；失败返回错误编码
* @modfy: 2020-02-24
****************************************************/
static int32 esam_set_power(struct tag_ESAM_DEVICE *dev, int32 state)
{
   if(strcmp(dev->base.szDeviceID, HW_DEVICE_ID_YX_ESAM) == 0)
   {
       switch(state)
       {
           case PWR_ON:
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
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int esam_set_wrlock(ESAM_DEVICE_T *dev)
{
    int ret = 0;
    int fd = 0;
    struct flock flockstr;

    flockstr.l_type = F_WRLCK;
    flockstr.l_start = 0;
    flockstr.l_whence = SEEK_SET;
    flockstr.l_len = 0;
    flockstr.l_pid = getpid();
    
    if (lockfd < 0)
    {
        fd = open(ESAM_LOCK_FILE, O_WRONLY | O_CREAT, 0660); 
        if (fd < 0)
        {
            printf("open %d failed.\n", lockfd);
            return -1;
        }
        else
        {
            lockfd = fd;
        } 
    }

    ret = fcntl(lockfd, F_SETLK, &flockstr);
    if (ret < 0)
    {
        fcntl(lockfd, F_GETLK, &flockstr);
        printf("Esam is lock now! and it's pid is %d\n", flockstr.l_pid);
    }
    
    return ret < 0 ? -2 : 0;
}

/**********************************************************************
* @name      : esam_set_unlock
* @brief     ：设置esam设备加锁
* @param[in] ：ESAM_DEVICE_T *dev  设备
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int esam_set_unlock(ESAM_DEVICE_T *dev)
{
    int ret = 0;
    int fd = 0;
    struct flock flockstr;

    flockstr.l_type = F_UNLCK;
    flockstr.l_start = 0;
    flockstr.l_whence = SEEK_SET;
    flockstr.l_len = 0;
    flockstr.l_pid = getpid();
    
    if (lockfd < 0)
    {
        fd = open(ESAM_LOCK_FILE, O_WRONLY | O_CREAT, 0660); 
        if (fd < 0)
        {
            //printf("open %s failed.\n", lockf);
            return -1;
        }
        else
        {
            lockfd = fd;
        }
    }

    ret = fcntl(lockfd, F_SETLK, &flockstr);
    if (ret < 0)
    {
        fcntl(lockfd, F_GETLK, &flockstr);
        printf("Esam is lock now! and it's pid is %d\n", flockstr.l_pid);
    }
    
    close(lockfd);
    lockfd = -1;
    return ret < 0 ? -2 : 0;
}


/***********************************************************
* @brief 设置esam设备加锁
* @param[in] dev: 设备描述
* @param[in] val: 1为上锁  0 为解锁
* @return 成功返回ERR_OK；失败返回错误编码
************************************************************/
int32  esam_set_lock(struct tag_ESAM_DEVICE *dev, int32 val)
{
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

#if 0
static int32 esam_write(uint8* pBuffer, int32 nLen)
{
    int32 iRet = 0;
    int32 fd = 0;
    
    fd = open(ESAM_FILE, O_RDWR);
    if(fd < 0)
    {
        DEBUG("cannot open device esam\n");
        return  ERR_NORMAL;
    }

    iRet = write(fd, pBuffer, nLen);
    

    close(fd);
    return iRet;
}

static int32 esam_read(uint8* pBuffer, int32 nLen)
{
    int32 iRet = 0;
    int32 fd = 0;
    
    fd = open(ESAM_FILE, O_RDWR);
    if(fd < 0)
    {
        DEBUG("cannot open device esam\n");
        return -1;
    }

    iRet = read(fd, pBuffer, nLen);

    close(fd);
    return iRet;
}
#endif
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
* @return成功返回ERR_OK；失败返回错误码。详见：附录1。
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
* @return成功返回ERR_OK或接收的长度；失败返回错误码。详见：附录1。
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
            iRet = sc1187_EsamRead(fd, buf, iDataLen+4+1);  //4:90 00 le(high) le(low) 1：需要读出检验字节 
            
            //for(i=0;i<(iDataLen+4+1);i++) //4:sw1 sw2 len1 len2 1:检验
                //printf("%02x ",buf[i]);

            //printf("\n\n");
            
            //lrc = CalLRC(buf,iDataLen+4);
            //if(buf[4+iDataLen] != lrc)
            //    DEBUG("LRC2 != lrc  LRC2=%02x lrc=%02x \n",buf[4+iDataLen],lrc);
        
        }
    }
    else
    {
        //DEBUG("result != 9000\n");
        //for(i=0;i<4;i++)
        //{
        //	printf("%02x ",buf[i]);
        //}
        
        //printf("\n\n");
     }

     close(fd);

     return iRet ;
}

#if 0
/*************************************************************************
 * 函数功能: 向TESAM中发送数据
 * 输入参数: fd ---> 文件描述符 pBuffer 数据指针，nLen 数据长度
 * 输出参数: >0: 发送长度  -1:failure
 * 操作流程: 1、向ESAM发送数据
 *           3、返回读取长度或错误
 ************************************************************************/

static int32 SendData(int fd, uint8* pBuffer, int32 nLen)
{
    int32 iRet = 0;

    //CsActivaten();  /* Set CS activate status */
    iRet = esam_write(pBuffer, nLen);
    //CsDeactivate(); /* Set CS deactivate status */

    return iRet;
}

/*************************************************************************
 * 函数功能: 读TESAM中数据
 * 输入参数: fd ---> 文件描述符 pBuffer 数据指针，nLen 数据长度
 * 输出参数: >0: 接收长度  -1:failure
 * 操作流程: 1、读取状态字0x55
 *           2、读取ESAM中序列号
 *           3、返回读取长度或错误
 ************************************************************************/
static int32 RecvData(int32 fd, uint8* pBuffer, int32 nLen)
{
    int32 i = 0;
    int32 iRet = -1;
    int32 iDataLen = 0;

   // CsActivaten();

    while(i++ < 1000)
    {
        esam_read(pBuffer, 1);
        if(pBuffer[0] == 0x55)
        {
            break;
        }
    }
    if(i >= 1000)
    {
        DEBUG("read 0x55 error\n");
        goto err;
    }

    iRet = esam_read(pBuffer+1, 4);
    if(iRet <= 0)
    {
        DEBUG("read sw1 sw2 error\n");
        goto err;
    }
    
    if((pBuffer[0] == DATA_55H) && 
        (pBuffer[SW1_POS] == SW1_VAL) &&
        (pBuffer[SW2_POS] == SW2_VAL))
    {
        iDataLen = (pBuffer[LEN1_POS]<<8) | pBuffer[LEN2_POS];
        iRet = esam_read(pBuffer+5, iDataLen+1);
        if(iRet <= 0)
        {
            printf("read data error\n");
            goto err;
        }
    }

    printf("data[%x][%x][%x][%x][%x][%x][%x][%x][%x][%x][%x][%x][%x][%x][%x][%x]\n",
        pBuffer[0],pBuffer[1],pBuffer[2],pBuffer[3],
        pBuffer[4],pBuffer[5],pBuffer[6],pBuffer[7],
        pBuffer[8],pBuffer[9],pBuffer[10],pBuffer[11],
        pBuffer[12],pBuffer[13],pBuffer[14],pBuffer[15]);
err:
  //  CsDeactivate();
    return iRet;
}

/*************************************************************************
 * 函数功能: 检查数据帧是否正确
 * 输入参数: pBuffer 数据指针，nLen 数据长度
 * 输出参数: 0: 数据帧正确  -1:数据错误
 * 操作流程: 1、通过检查数据头和数据校验判读数据帧的正确性
 ************************************************************************/
static int32 CheckFrame(uint8* pBuffer, int nLen)
{
 //  int32 head_pos = 0;
   int32 i = 0;
   uint8 * p;
   int32 iDataLen = 0;

   if( (pBuffer == NULL) || (nLen <= 0))
        return 0;

   p = pBuffer;
   
   for(i=0; i<nLen; i++)
   {
        if(pBuffer[i] == DATA_55H)
        {
              //  head_pos = i;
                if( (nLen-i) > 5)
                {
                        //DEBUG("find i[%d]\n", i);
                        p = pBuffer+i;
                        break;
                }
                else
                {
                        //DEBUG("Receive data error!\n");
                        return -1;
                }
         }
    }

    if((p[SW1_POS] == SW1_VAL) && (p[SW2_POS] == SW2_VAL))
    {
        iDataLen = (p[LEN1_POS]<<8) | p[LEN2_POS];
        /* add SW1 SW2 LEN1 LEN2 4 bytes */
        if( p[iDataLen + LCR_OFFSET] == CalLRC( &p[SW1_POS], (iDataLen + 4)) )
        {
            //DEBUG("Receive data LCR  ok!\n");
            return 0;
        }
        else
        {
            //DEBUG("Receive data LCR error!\n");
        }
    }
    else
    {
        printf("sw1 & sw2 error\n");
    }
    
    return -1;
}
#endif
#if 0
static int32 esam_check_1168(struct tag_ESAM_DEVICE *dev)
{
    uint8 aSendBuff[] = { 0x55, 0x80, 0x36, 0x00, 0xFF, 0x00, 0x00, 0xb6 };//Get tesam
    uint8 aRecvBuff[256] = {0};
    int32  iRecvLen = 0x60;
    int32 iRet = 0;
    int32 fd = 0;
    int32 iSendLen = sizeof(aSendBuff);

    sleep(1);
    
    DEBUG(" Sending %d bytes data...\n", iSendLen);
    iRet = SendData(fd, aSendBuff, iSendLen);
    if(iRet <= 0)
        DEBUG("Send data failure %d \n", iRet);

    usleep(100);

    //printf("tesam ready for receiving data...\n");
    iRet = RecvData(fd, aRecvBuff, iRecvLen);
    if(iRet < 0)
    {
        printf("receive data error\n");
        return -1;
    }
    
    DEBUG(" receiving data finished ...\n Checking data...\n");
    iRet = CheckFrame(aRecvBuff, iRecvLen);

    //close(fd);

    return iRet;
}

static int32 esam_check_1187(struct tag_ESAM_DEVICE *dev)
{
    uint8 aSendBuff[] = {0x55, 0x00, 0xb0, 0x99, 0x05, 0x00, 0x02, 0x00, 0x08, 0xd9};
    uint8 aRecvBuff[256] = {0};
    int32  iRecvLen = 0x60;
    int32 iRet = 0;
    int32 fd = 0;
    int32 iSendLen = sizeof(aSendBuff);

    fd = open(ESAM_FILE, O_RDWR);
    if(fd < 0)
    {
        DEBUG("/dev/encryption0 fd=%d\n",fd);
        exit(1);
    }
    sleep(1);
    
    DEBUG(" Sending %d bytes data...\n", iSendLen);
    sc1187_send(fd, aSendBuff, iSendLen);

    usleep(100);

    sc1187_recv(fd, aRecvBuff, iRecvLen);

    close(fd);

    return iRet;
}

static int32 esam_check_mxdx(struct tag_ESAM_DEVICE *dev)
{
    int32 iRet = 0;
    

    iRet = esam_check_1187(dev);

    iRet = esam_check_1168(dev);


    return iRet;
}
#endif 


static int esam_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
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

/* 初始化模块变量 */
ESAM_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "esam",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2020-02-20",
        .nDevNum = ESAM_DEV_NUM,
        .szDevices = aEsamDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = esam_open,
        .pfClose = esam_close,
    },
};
