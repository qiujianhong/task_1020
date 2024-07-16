/*********************************************
 * Copyright(c) GW All rights reserved.
 * 程序名称: ac.c
 * 版本号: 0.1
 * 功能: 
 * 原始开发人及时间：
        2021年8月12日 
 * 修改人员、时间及修改内容：
        
 * 其他: 
 *********************************************/

#include "baseCommon.h"
#include <sys/ioctl.h>
#include <string.h>
#include <linux/spi/spidev.h>
#include "ac.h"
#include "macro.h"

#define DEBUG(fmt, args...) printf("[%s:%s:%d] "fmt, __FILE__,__FUNCTION__,__LINE__, ##args)

#define	SPI_CPHA	0x01			/* clock phase */
#define	SPI_CPOL	0x02			/* clock polarity */
#define	SPI_MODE_0	(0|0)			/* (original MicroWire) */
#define	SPI_MODE_1	(0|SPI_CPHA)
#define	SPI_MODE_2	(SPI_CPOL|0)
#define	SPI_MODE_3	(SPI_CPOL|SPI_CPHA)
#define	SPI_CS_HIGH	0x04			/* chipselect active high? */
#define	SPI_LSB_FIRST	0x08			/* per-word bits-on-wire */

#define NDELAY 100
#define MAX_SPEED   200000
#define SPEED_STEP  10000

#define  IOC_MAGIC		'I'
#define JC_IO_ENABLE		_IOW(IOC_MAGIC, 0, __u8)
#define JC_IO_DISABLE		_IOW(IOC_MAGIC, 1, __u8) 
/* int index的宏定义如下:*/
#define JC_GPIO_RST  1
#define JC_GPIO_CT   2



#define DEVICE_AC_CTRL      "/dev/acctrl"
#define DEVICE_AC           "/dev/ac0"


/*
SPI_IOC_RD_MODE:			读取spi模式
SPI_IOC_WR_MODE:			写spi模式
SPI_IOC_RD_LSB_FIRST:		读状态查看是否低位FIRST
SPI_IOC_WR_LSB_FIRST:		写低位FIRST
SPI_IOC_RD_BITS_PER_WORD:	传输字节位宽模式读取
SPI_IOC_WR_BITS_PER_WORD:	写传输字节位宽模式
SPI_IOC_RD_MAX_SPEED_HZ:	读取SPI最高频率
SPI_IOC_WR_MAX_SPEED_HZ:	写SPI最高频率
*/

#define SPI_IOC_MAGIC			'k'
#define SPI_IOC_RD_MODE		    _IOR(SPI_IOC_MAGIC, 1, __u8)
#define SPI_IOC_WR_MODE		    _IOW(SPI_IOC_MAGIC, 1, __u8)
#define SPI_IOC_RD_LSB_FIRST	_IOR(SPI_IOC_MAGIC, 2, __u8)
#define SPI_IOC_WR_LSB_FIRST	_IOW(SPI_IOC_MAGIC, 2, __u8)
#define SPI_IOC_RD_BITS_PER_WORD	_IOR(SPI_IOC_MAGIC, 3, __u8)
#define SPI_IOC_WR_BITS_PER_WORD	_IOW(SPI_IOC_MAGIC, 3, __u8)
#define SPI_IOC_RD_MAX_SPEED_HZ	    _IOR(SPI_IOC_MAGIC, 4, __u32)
#define SPI_IOC_WR_MAX_SPEED_HZ	    _IOW(SPI_IOC_MAGIC, 4, __u32)

static ACSPI_COMM_PARA_T s_Para = 
{
    SPI_CPHA,
    8,
    2000 * 1000,
    0,
};

/**
*********************************************************************
* @brief：      不中断的睡眠函数
* @param[in]： seconds 秒数
* @param[out]：NA
* @author:     贺宁
* @return：     NA
*********************************************************************
*/
void sleep_no_interrupt(unsigned int seconds)
{
    int left = seconds;

    //sleep can be wake up by alarm or Ctrl + C, but we can restart it! 
    //because the sleep function will return the left second after be wake up!    
    while (left > 0)
    {
        left = sleep(left);
    }

    return;
}

/**********************************************************************
* @name：      jc_reset
* @brief：     复位交采模块
* @param[in] ：delay_us    -  延时时间
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
int jc_reset(int delay_us)
{
    int err = -1;
    int fd_acctrl;
    int rst_delay_us = 5000;
    int index = JC_GPIO_RST;
    
    if(delay_us > 0)
         rst_delay_us = delay_us;

    fd_acctrl = open(DEVICE_AC_CTRL, O_RDWR);
    if( fd_acctrl <= 0) 
    {
        printf("cannot open /dev/acctrl\n");
        return -2;
    }

    err= ioctl(fd_acctrl,  JC_IO_ENABLE, &index);
    usleep(rst_delay_us);
    err+= ioctl(fd_acctrl,  JC_IO_DISABLE, &index);
    usleep(rst_delay_us);
    
    close(fd_acctrl);
    
    
    return err;
}



/**********************************************************************
* @name：      ct_reset
* @brief：     复位交采模块
* @param[in] ：delay_us    -  延时时间
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
int ct_reset(int delay_us)
{
    int err = -1;
    int fd_acctrl;
    int rst_delay_us = 5000;
    int index = JC_GPIO_CT;
    
    if(delay_us > 0)
         rst_delay_us = delay_us;

    fd_acctrl = open(DEVICE_AC_CTRL, O_RDWR);
    if( fd_acctrl <= 0) 
    {
        printf("cannot open /dev/acctrl\n");
        return -2;
    }

    err= ioctl(fd_acctrl,  JC_IO_ENABLE, &index);
    usleep(rst_delay_us);
    err+= ioctl(fd_acctrl,  JC_IO_DISABLE, &index);
    usleep(rst_delay_us);
    
    close(fd_acctrl);
    
    
    return err;
}


/**********************************************************************
* @name：      spi_mode_operation
* @brief：     模式设置
* @param[in] ：delay_us    -  延时时间
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
int spi_mode_operation(int32 fd, unsigned int mode, void *param)
{
    int err = -1;

    if (fd < 0)
    {  
        DEBUG("ac_spi not open fd[%d]\n", fd);  
        return ERR_NORMAL;  
    }

    switch (mode)
    {
    case SPI_IOC_WR_MODE:
        err = ioctl(fd, SPI_IOC_WR_MODE, param);
        if (err != ERR_OK)
        {
            break;
        }
        err = ioctl(fd, SPI_IOC_RD_MODE, param);
        if (err != ERR_OK)
        {
            break;
        }

        break;

    case SPI_IOC_WR_BITS_PER_WORD:
        err = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, param);
        if (err != ERR_OK)
        {
            break;
        }
        err = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, param);
        if (err != ERR_OK)
        {
            break;
        }

        break;

    case SPI_IOC_WR_MAX_SPEED_HZ:
        err = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, param);
        if (err != ERR_OK)
        {
            break;
        }
        err = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, param);
        if (err != ERR_OK)
        {
            break;
        }

        break;

    default:
        break;
    }

    return err;
}


/**********************************************************************
* @name：      ac_spi_open
* @brief：     打开交采设备枷锁
* @param[in] ：fp    -  设备句柄
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_open(int* fp)
{
    int fd;
////	int ret;
    
    fd = open(DEVICE_AC, O_RDWR);
    if (fd < 0 )
    {
        printf("can't open spi dev[%s]\n", DEVICE_AC);
        return -1;
    }
    if (flock(fd, LOCK_EX))
    {
        printf("Lock ac Error\n");
        close(fd);
        return -EAGAIN;
    }

    *fp = fd;
/****
    if(ret = spi_mode_operation(DEFAULT_MODE))
        return ret;
    if(ret = set_speed(DEFAULT_SPEED))
        return ret;
    ***/
    return ERR_OK;
}

/**********************************************************************
* @name：      ac_set_cs
* @brief：     片选操作
* @param[in] ：fp    -  设备句柄
* @param[out]：
* @return    ：
**********************************************************************/
// int ac_set_cs(int fd, uint8_t val)
// {
// 	int ret = 0;
// 	ret = ioctl(fd, SPI_IOC_SET_CS, &val);
// 	if (0 != ret) {
// 		printf("Set val to sc1187y CS failed.\n");
// 		return -1;
// 	}

//     return ERR_OK;
// }

#if 0
/**********************************************************************
* @name：      ac_spi_read_data
* @brief：     读取交采集数据
* @param[in] ：fp    -  设备句柄
               len   -  接收缓存大小
* @param[out]：buf   -  接收缓存
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_read_data(int32 fd, uint8 *buf, uint32 len)
{
    int32 recv_len = 0;                                       

    if (buf == NULL)
    {
        return ERR_PTR;
    }

    if (fd < 0)
    {  
        DEBUG("can't Open ac_spi fd %d\n", fd);  
        return ERR_NORMAL;  
    }

    recv_len = read(fd, buf, len);   

    return recv_len;
}
#endif
/**********************************************************************
* @name：      ac_spi_read_data
* @brief：     读取交采集数据
* @param[in] ：fp    -  设备句柄
               len   -  接收缓存大小
* @param[out]：buf   -  接收缓存
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_read(int32 fd, uint8 *txbuf, uint8 *rxbuf, uint16 rxlen)
{
    uint8 * tx = txbuf;
    uint8 * rx = rxbuf;

    struct spi_ioc_transfer tr = {
        .tx_buf = ((unsigned long) tx),
        .rx_buf = ((unsigned long) rx),
        .len = rxlen,
        .delay_usecs = s_Para.delay,
        .speed_hz = s_Para.speed,
        .bits_per_word = s_Para.bits,
        //.cs_change   = 0,
    };

    int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    if (ret < 0) 
        printf("jc recv can't send spi message");

    return ret;
}


/**********************************************************************
* @name：      ac_spi_close
* @brief：     关闭交采设备
* @param[in] ：fp    -  设备句柄
* @param[out]：
* @return    ：成功返回ERR_OK；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_close(int *fd)
{
    int spi_fd =  *fd;

    if (spi_fd >=0)
    {
        flock(spi_fd, LOCK_UN);
        close(spi_fd);
        *fd = -1;
    }

    return ERR_OK;
}

/**********************************************************************
* @name：      ac_spi_init
* @brief：     设备初始化
* @param[in] ：para    -  参数
* @param[out]：
* @return    ：成功返回句柄；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_init(ACSPI_COMM_PARA_T *para)
{
    int fd_spi = -1;
    int ret = -1;

    if (para == NULL)
    {
        return ERR_PTR;
    }

    ret = ac_spi_open(&fd_spi);
    if (ret != ERR_OK)
    {
        return ERR_NODEV;
    }

    ret = spi_mode_operation(fd_spi, SPI_IOC_WR_MODE, &para->mode);
    if (ret != ERR_OK)
    {
        return ERR_EXECFAIL;
    }

    ret = spi_mode_operation(fd_spi, SPI_IOC_WR_BITS_PER_WORD, &para->bits);
    if (ret != ERR_OK)
    {
        return ERR_EXECFAIL;
    }

    ret = spi_mode_operation(fd_spi, SPI_IOC_WR_MAX_SPEED_HZ, &para->speed);
    if (ret != ERR_OK)
    {
        return ERR_EXECFAIL;
    }

    s_Para = *para;

    return fd_spi;
}

/**********************************************************************
* @name      : ac_clean_comm_buf
* @brief     ：清除通讯接收数据
* @param[in] ：fd  -  串口操作句柄
* @param[out]：
* @return    ：
* @Date      ：2021-7-26
**********************************************************************/
void ac_clean_spi_buf(int32 fd, uint32 cnt)
{
    int     RecvLen = 0xff;                     //接收数据长度
    uint8   RecvBuf[1600] = {0};                //接收数据缓冲区
    uint8   SendBuf[1600] = {0};                //接收数据缓冲区
    int     i = 0;

    while (RecvLen > 0)
    {
        i ++;

        if (i > cnt)
        {
            break;
        }

        RecvLen = ac_spi_recv_data_timeout(fd, SendBuf, RecvBuf, sizeof(RecvBuf), 50000, 65);
    }

    return;
}

#if 0
/**********************************************************************
* @name：      ac_spi_recv_data_timeout
* @brief：     接收
* @param[in] ：fd        -  句柄
               buflen    -  接收缓存最大范围   
               frametime -  帧超时时间 ms
               bytetime  -  字节超时时间 ms
* @param[out]：buf       -  接收缓存
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_recv_data_timeout(int32 fd, uint8 *buf, uint32 buflen, uint16 frametime, uint16 bytetime)
{
	int recv_len = 0;        /*接收到数据长度*/
    int data_len = 0;        /*串口缓冲区中数据的长度*/
    int count = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;

    recv_len = 0;
    data_len = 0;
    
    if(fd < 0)
    {
        return 0;
    }

    while (1)
    {
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = 1000;
        do 
        {
            n = select(fd + 1, &set, NULL, NULL, &timeout);
        }while(n < 0);

        if (n > 0 && FD_ISSET(fd, &set))
        {
            recv_len = ac_spi_read_data(fd, buf + data_len, buflen - data_len);
        }
        else
        {
            recv_len = 0;
        }
        if (recv_len > 0)    
        {
            data_len += recv_len;
            count = 0;
        }
        else
        {
            if(0 == data_len && count > frametime) 
            {
                return data_len; /*-接收数据超时-*/
            }

            if(0 != data_len && (count > bytetime || data_len >= buflen))
            {
                return data_len;
            }
        }
        count++;
    }
}
#endif
/**********************************************************************
* @name：      ac_spi_recv_data_timeout
* @brief：     接收
* @param[in] ：fd        -  句柄
               buflen    -  接收缓存最大范围   
               frametime -  帧超时时间 ms
               bytetime  -  字节超时时间 10ms
* @param[out]：buf       -  接收缓存
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_recv_data_timeout(int32 fd, uint8 *txbuf, uint8 *rxbuf, uint16 buflen, uint32 frametime, uint32 bytetime)
{
    int   recv_len = 0;        /*接收到数据长度*/
    int   datalen = 0;
    int   n = 0;
    uint8 sum = 0;
    uint32 rxframetime = 50000;
    uint32 rxbytetime = 60;
    uint16 buflenNew = 0;

    if (fd < 0)
    {
        return -1;
    }

    memset(rxbuf, 0, buflen);
    memset(txbuf, 0, buflen);

    if (frametime > 0)
    {
        rxframetime = frametime;
    }

    if (bytetime > 0)
    {
        rxbytetime = bytetime;
    }


    while (n++ < rxbytetime)
    {
        recv_len = ac_spi_read(fd, txbuf, rxbuf, 1);
        if ((recv_len == 1) && (rxbuf[0] == 0x55))
        {
            sum = 0;
            recv_len = ac_spi_read(fd, txbuf, &rxbuf[1], 3);
            sum += rxbuf[0];
            sum += rxbuf[1];
            sum += rxbuf[2];
            sum += 0x33;

            if (rxbuf[3] == sum)
            {
                memcpy(&datalen, &rxbuf[1], 2);

                if (datalen > 4096)
                {
                    recv_len = ac_spi_read(fd, txbuf, &rxbuf[0], 4096);
                    buflenNew = ac_spi_read(fd, txbuf, &rxbuf[4096], datalen - 4096);
                    recv_len += buflenNew;

                }
                else
                {
                    recv_len = ac_spi_read(fd, txbuf, &rxbuf[0], datalen);
                }
                break;
            }
            else
            {
                usleep(10000);
                continue;
            }
        }
        else if ((recv_len == 1) && (rxbuf[0] == 0x68))
        {
            //交采抄读可以简单做个适应，读到55的本来就判的是长度，新版本也是支持55了，缓冲区开大就行；读到68的就按1600来读。
            buflenNew = MIN(buflen, 1600);
            recv_len = ac_spi_read(fd, txbuf, &rxbuf[1], buflenNew - 1);
            // recv_len = ac_spi_read(fd, txbuf, &rxbuf[1], 2);
            // memcpy((uint8 *)&datalen, (uint8 *)&rxbuf[1], 2);
            // if (datalen > 1600)
            // {
            //     return -1;
            // }
            // recv_len = ac_spi_read(fd, txbuf, &rxbuf[3], datalen);
            // recv_len += 3;
            recv_len += 1;
            break;
        }
        else if ((recv_len == 1) && (rxbuf[0] == 0xF1))
        {
            continue;
        }
        else
        {
            usleep(10000);
            continue;
        }
    }

    usleep(rxframetime);
    return recv_len;
}

#if 0
/**********************************************************************
* @name：      ac_spi_send_data
* @brief：     读取交采集数据
* @param[in] ：fp    -  设备句柄
               buf   -  接收缓存
               len   -  接收缓存大小
* @param[out]：
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_send_data(int32 fd, uint8 *buf, uint32 len)
{
    int32 send_len = 0;  

    if (buf == NULL)
    {
        return ERR_PTR;
    }
    
    if (fd < 0)  
    {  
        DEBUG(" ac_spi port fd error fd %d\n", fd);  
        return ERR_INVALID;  
    }
    
    send_len = write(fd, buf, len);  
    if(send_len == len)
        return ERR_OK;
      else
        return ERR_NORMAL;
}
#endif

/**********************************************************************
* @name：      ac_spi_send_data
* @brief：     读取交采集数据
* @param[in] ：fp    -  设备句柄
               buf   -  接收缓存
               len   -  接收缓存大小
* @param[out]：
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_send(int32 fd, uint8 *txbuf, uint32 txlen, uint8 *rxbuf)
{
    uint8 * tx = txbuf;
    uint8 * rx = rxbuf;

    struct spi_ioc_transfer tr = {
    .tx_buf = ((unsigned long) tx), //
    .rx_buf = ((unsigned long) rx),
    .len = txlen,
    .delay_usecs = s_Para.delay,
    .speed_hz = s_Para.speed,
    .bits_per_word = s_Para.bits,
    //.cs_change   = 0,
    };

    int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr); //阻塞方式

    if (ret < 0) 
        printf("jc send can't send spi message");

    return ret;
}

/**********************************************************************
* @name：      ac_spi_send_data,
* @brief：     读取交采集数据，待q
* @param[in] ：fp    -  设备句柄
               buf   -  接收缓存
               len   -  接收缓存大小
* @param[out]：
* @return    ：成功返回长度；失败返回错误码。详见：附录1。
**********************************************************************/
int ac_spi_send_data(int32 fd, uint8 *txbuf, uint32 txlen, uint8 *rxbuf, uint32 frametime)
{
    int fxDelayUs = 500000;
    int ret = ac_spi_send(fd, txbuf, txlen, rxbuf);

    if (frametime > 0)
    {
        fxDelayUs = frametime;
    }
    usleep(fxDelayUs);
    return ret;
}
