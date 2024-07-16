/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP 串口操作
* @date：    2021-8-9
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"

#include "ble_uart.h"

/* 蓝牙控制的设备节点 */
#define BLE_CTRL_NAME   "/dev/btctrl"

/* 读写操作标志值 */
#define	BT_IOC_MAGIC		'B'
#define BT_IO_SETHIGH		_IOW(BT_IOC_MAGIC, 1, __u8)
#define BT_IO_SETLOW		_IOW(BT_IOC_MAGIC, 2, __u8)
/* 要操作的IO的索引的值的地址 */
#define BTC_IO_RST  0
#define BTC_IO_CTS  1
/*蓝牙控制的IO索引包含蓝牙模组CTS控制，蓝牙模组复位控制。CTS为低电平时候使能发送，复位引脚为低电平复位*/


/**********************************************************************
* @name      : ble_dev_reset
* @brief     ：蓝牙设备复位
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    :
**********************************************************************/
int ble_dev_reset()
{
    int fd = -1;
    int ret = 0;
    
    fd = open(BLE_CTRL_NAME, O_WRONLY);
    if(fd < 0)
    {
        return -1;
    }

    ret = ioctl(fd, BT_IO_SETLOW, BTC_IO_RST);
    if(0 != ret)
    {
        close(fd);
        return -2;
    }
    usleep(10000);
    ret = ioctl(fd, BT_IO_SETHIGH, BTC_IO_RST);
    if(0 != ret)
    {
        close(fd);
        return -3;
    }

    close(fd);
    return 0;
}

/**********************************************************************
* @name      : ble_uart_open
* @brief     ：串口设备打开
* @param[in] ：char *name  串口名称
* @param[out]：
* @return    ：>=0-句柄/错误码
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    :
**********************************************************************/
int ble_uart_open(char *name)
{
    if(NULL == name || 0 == strlen(name))
    {
        return -1;
    }
    if(0 != access(BLE_UART_NAME, F_OK))
    {
        return -1;
    }
    return open(name, O_RDWR);
}



/**********************************************************************
* @name      : ble_set_speed
* @brief     ：设置波特率
* @param[in] ：int fd           串口句柄
               int speed        波特率
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    :
**********************************************************************/
int ble_set_speed(int fd, int speed)
{
    int i; 
    int ret; 
    int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300};
    int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300};

    struct termios Opt; 	/*Opt中存储波特率数据结构*/
    if(0 != tcgetattr(fd, &Opt)) 	/*获取termios结构*/
    {
        return -1;
    }

    for(i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
    {
        if(speed == name_arr[i])   /*查找支持的波特率*/
        {
            cfsetispeed(&Opt, speed_arr[i]); /*设置输入波特率*/
            cfsetospeed(&Opt, speed_arr[i]); /*设置输出波特率*/
            ret = tcsetattr(fd, TCSANOW, &Opt);  /*设置串口新的termios结构*/
            if(0 != ret)
            {
                return -2;
            }
            tcflush(fd, TCIOFLUSH); /*清除串口的输入输出队列中的数*/
            return 0;                     
        }  
    }
    return -3;
}

/**********************************************************************
* @name      : ble_set_parity
* @brief     ：设置属性
* @param[in] ：int fd              串口句柄
               uint8  databits     数据位(5~8)
               uint8  stopbits     停止位(1~2)
               char   parity       校验位(N 无校验，E 偶校验，O奇校验)
               uint8  set_crtscts  硬件流控 1-设置 0-不设
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    :
**********************************************************************/
int ble_set_parity(const int fd, uint8 databits, uint8 stopbits, char parity, uint8 set_crtscts)
{
    struct termios options; 
    if(0 != tcgetattr(fd, &options)) /*读取串口termios结构*/
    { 
        return -1;  
    }

    /* 设置为不加工模式, 不回 */
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag = IGNBRK;
    options.c_cflag |= CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;  /*清除原字符大*/

    switch(databits) /*设置数据位数*/
    {   
        case 7:             
            options.c_cflag |= CS7;  /*设置7位数据位*/
            break;
        case 8:     
            options.c_cflag |= CS8;  /*设置8位数据位*/
            break;   
        default:    
            return -2;  
    }

    switch (parity) 
    {   
        case 'n':	/*无校验*/
        case 'N': 
            options.c_cflag &= ~PARENB;   /* 输入奇偶校验无效 */
            break;

        case 'o': /*奇校验*/
        case 'O':     															
            options.c_cflag |= (PARODD | PARENB);	/*设置为奇效验*/  
            //options.c_iflag |= INPCK;		/*对输入进行奇偶检*/
            break;

        case 'e': /*偶校验*/
        case 'E':  
            options.c_cflag &= ~PARODD;
            options.c_cflag |= PARENB;  /* 使奇偶校验有*/ 
            //options.c_iflag |= INPCK;		/*对输入进行奇偶检*/
            break;

        case 'S': /*space校验*/
        case 's': 
            options.c_cflag &= ~CSTOPB;
            break;

        default:   
        return -3;  
    }

    /* 设置停止位*/ 
    switch (stopbits)
    {   
        case 1: /*1位停止位*/
            options.c_cflag &= ~CSTOPB;  
            break;  
        case 2: /*2位停止位*/
            options.c_cflag |= CSTOPB;  
            break;
        default:  
            return -4; 
    }

    tcflush(fd, TCIFLUSH);        /* 清输入队列*/
    options.c_cc[VTIME] = 100;   /* 设置超时10 seconds*/   
    options.c_cc[VMIN] = 0;      /* 更新设置，使马上生效 */
    
// 	options.c_iflag |= (IXON | IXOFF);	/*开软件流控*/
    options.c_iflag &= ~(IXON|IXOFF|IXANY);	/*关软件流*/

    if (set_crtscts == 1)
        options.c_cflag |= CRTSCTS; /*开硬件流控*/
    else
        options.c_cflag &= ~CRTSCTS;/*关硬件流控*/

    options.c_iflag &= ~IGNCR;  /*不忽略收到的CR字符*/
    options.c_iflag &= ~ICRNL;

    if(0 != tcsetattr(fd, TCSANOW, &options))  /*对串口进行参数设置*/
    {  
        return -5;  
    }
    return 0;
}

/**********************************************************************
* @name      : ble_uart_set
* @brief     ：串口设置属性
* @param[in] ：int fd                句柄
               int    baud           波特率
               uint8  databits       数据位(5~8)
               uint8  stopbits       停止位(1~2)
               char   parity         校验位(N 无校验，E 偶校验，O奇校验)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    :
**********************************************************************/
int ble_uart_set(int fd, int baud, uint8 databits, uint8 stopbits, char parity)
{
    int ret = 0;
    if(fd < 0)
    {
        return -1;
    }

    ret = ble_set_speed(fd, baud);
    if(0 != ret)
    {
        printf("set_speed false, ret %d!\n", ret);
        return ret;
    }

    ret = ble_set_parity(fd, databits, stopbits, parity, 0);
    if(0 != ret)
    {
        printf("set_Parity false, ret %d!\n", ret);
        return ret;
    }
    return 0;
}

/**********************************************************************
* @name      : ble_uart_send
* @brief     ：串口发送数据
* @param[in] ：int fd                句柄
               uint8* data           数据
               int len               长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    :
**********************************************************************/
int ble_uart_send(int fd, uint8 *data, int len)
{
    int send_len = 0;  

    if(fd < 0 || NULL == data || 0 == len)
    {
        return -1;
    }

    send_len = write(fd, data, len);
    return send_len == len ? 0 : -2;
}

/**********************************************************************
* @name      : ble_uart_recv
* @brief     ：串口数据接收接口(等待)
* @param[in] ：int fd                句柄
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间
               int rcvtime                  数据字节之间间隔时间
* @param[out]：uint8* pBuffer               接收数据缓冲区
* @return    ：接收到数据的长度
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    :
**********************************************************************/
int ble_uart_recv(int fd, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime)
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
        timeout.tv_usec = 5000;
        do 
        {
            n = select(fd + 1, &set, NULL, NULL, &timeout);
        }while(n < 0);

        if (n > 0 && FD_ISSET(fd, &set))
        {
            recv_len = read(fd, pBuffer + data_len, nBufLen-data_len);
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
            if(0 == data_len && count * 5 > rsptime) 
            {
                return data_len; /*-接收数据超时-*/
            }

            if(0 != data_len && (count * 5 > rcvtime || data_len >= nBufLen))
            {
                return data_len;
            }
        }
        count++;
    }
}

/**********************************************************************
* @name      : ble_uart_close
* @brief     ：串口设备关闭
* @param[in] ：int fd                句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-8-9
* @Update    :
**********************************************************************/
void ble_uart_close(int fd)
{
    if(fd >= 0)
    {
        close(fd);
    }
}



