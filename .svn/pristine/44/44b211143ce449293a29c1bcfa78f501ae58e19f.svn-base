#include "types.h"
#include <uart.h>
#include <gpio.h>
#include "sysDefine.h"
#include <sys/file.h>    //�ļ�ϵͳ����
#include <sys/types.h>
#include <dirent.h>

#define PIN_485_1_DIR    18
#define PIN_485_2_DIR    19


/*uart �豸·�� */
#define   UART_DEV_NAME_TTYS1      "/dev/ttyS1"
#define   UART_DEV_NAME_TTYS2      "/dev/ttyS2"
#define   UART_DEV_NAME_TTYS3      "/dev/ttyS3"
#define   UART_DEV_NAME_TTYS4      "/dev/ttyS4"
#define   UART_DEV_NAME_TTYS5      "/dev/ttyS5"
#define   UART_DEV_NAME_TTYS6      "/dev/ttyS6"
#define   UART_DEV_NAME_TTYS7      "/dev/ttyS7"
#define   UART_DEV_NAME_TTYS8      "/dev/ttyS8"
#define   UART_DEV_NAME_TTYS9      "/dev/ttyS9"


#define   UART_DEV_NAME_TTYACM1_0      "/tmp/dev/usb_exm1_0"
#define   UART_DEV_NAME_TTYACM1_1      "/tmp/dev/usb_exm1_1"
#define   UART_DEV_NAME_TTYACM1_2      "/tmp/dev/usb_exm1_2"
#define   UART_DEV_NAME_TTYACM1_3      "/tmp/dev/usb_exm1_3"
#define   UART_DEV_NAME_TTYACM1_4      "/tmp/dev/usb_exm1_4"

#define   UART_DEV_NAME_TTYACM2_0      "/tmp/dev/usb_exm2_0"
#define   UART_DEV_NAME_TTYACM2_1      "/tmp/dev/usb_exm2_1"
#define   UART_DEV_NAME_TTYACM2_2      "/tmp/dev/usb_exm2_2"
#define   UART_DEV_NAME_TTYACM2_3      "/tmp/dev/usb_exm2_3"
#define   UART_DEV_NAME_TTYACM2_4      "/tmp/dev/usb_exm2_4"

#define   UART_DEV_NAME_TTYACM3_0      "/tmp/dev/usb_exm3_0"
#define   UART_DEV_NAME_TTYACM3_1      "/tmp/dev/usb_exm3_1"
#define   UART_DEV_NAME_TTYACM3_2      "/tmp/dev/usb_exm3_2"
#define   UART_DEV_NAME_TTYACM3_3      "/tmp/dev/usb_exm3_3"
#define   UART_DEV_NAME_TTYACM3_4      "/tmp/dev/usb_exm3_4"

#define   UART_DEV_NAME_TTYACM4_0      "/tmp/dev/usb_exm4_0"
#define   UART_DEV_NAME_TTYACM4_1      "/tmp/dev/usb_exm4_1"
#define   UART_DEV_NAME_TTYACM4_2      "/tmp/dev/usb_exm4_2"
#define   UART_DEV_NAME_TTYACM4_3      "/tmp/dev/usb_exm4_3"
#define   UART_DEV_NAME_TTYACM4_4      "/tmp/dev/usb_exm4_4"

#define   UART_DEV_NAME_TTYACM5_0      "/tmp/dev/usb_exm5_0"
#define   UART_DEV_NAME_TTYACM5_1      "/tmp/dev/usb_exm5_1"
#define   UART_DEV_NAME_TTYACM5_2      "/tmp/dev/usb_exm5_2"
#define   UART_DEV_NAME_TTYACM5_3      "/tmp/dev/usb_exm5_3"
#define   UART_DEV_NAME_TTYACM5_4      "/tmp/dev/usb_exm5_4"


enum
{
    TTYS4 = 0,
    TTYS3,
    TTYS9,
    TTYS8,

        TTYS_ACM1_0,
    TTYS_ACM1_1,
    TTYS_ACM1_2,
    TTYS_ACM1_3,
    TTYS_ACM1_4,
       TTYS_ACM2_0,
    TTYS_ACM2_1,
    TTYS_ACM2_2,
    TTYS_ACM2_3,
    TTYS_ACM2_4,
       TTYS_ACM3_0,
    TTYS_ACM3_1,
    TTYS_ACM3_2,
    TTYS_ACM3_3,
    TTYS_ACM3_4,
       TTYS_ACM4_0,
    TTYS_ACM4_1,
    TTYS_ACM4_2,
    TTYS_ACM4_3,
    TTYS_ACM4_4,
       TTYS_ACM5_0,
    TTYS_ACM5_1,
    TTYS_ACM5_2,
    TTYS_ACM5_3,
    TTYS_ACM5_4,

    TTYNULL=255,
};

 #define UART_DEV_NUM      29
static char *aUartDevice[UART_DEV_NUM] = {
                     UART_RS4852_DEVICE_ID_STR,
                     UART_RS4851_DEVICE_ID_STR,
                     UART_DEVICE_BLE_STR,
                     UART_DEVICE_ACPORT_STR,

                     UART_1_0_DEVICE_ID_STR,
                     UART_1_1_DEVICE_ID_STR,
                     UART_1_2_DEVICE_ID_STR,
                     UART_1_3_DEVICE_ID_STR,
                     UART_1_4_DEVICE_ID_STR,

                     UART_2_0_DEVICE_ID_STR,
                     UART_2_1_DEVICE_ID_STR,
                     UART_2_2_DEVICE_ID_STR,
                     UART_2_3_DEVICE_ID_STR,
                     UART_2_4_DEVICE_ID_STR,

                     UART_3_0_DEVICE_ID_STR,
                     UART_3_1_DEVICE_ID_STR,
                     UART_3_2_DEVICE_ID_STR,
                     UART_3_3_DEVICE_ID_STR,
                     UART_3_4_DEVICE_ID_STR,

                     UART_4_0_DEVICE_ID_STR,
                     UART_4_1_DEVICE_ID_STR,
                     UART_4_2_DEVICE_ID_STR,
                     UART_4_3_DEVICE_ID_STR,
                     UART_4_4_DEVICE_ID_STR,

                     UART_5_0_DEVICE_ID_STR,
                     UART_5_1_DEVICE_ID_STR,
                     UART_5_2_DEVICE_ID_STR,
                     UART_5_3_DEVICE_ID_STR,
                     UART_5_4_DEVICE_ID_STR,
};

static char* uart_device[UART_DEV_NUM] =
{
  UART_DEV_NAME_TTYS4,
  UART_DEV_NAME_TTYS3,
  UART_DEV_NAME_TTYS9,
  UART_DEV_NAME_TTYS8,
  UART_DEV_NAME_TTYACM1_0,
  UART_DEV_NAME_TTYACM1_1,
  UART_DEV_NAME_TTYACM1_2,
  UART_DEV_NAME_TTYACM1_3,
  UART_DEV_NAME_TTYACM1_4,
  UART_DEV_NAME_TTYACM2_0,
  UART_DEV_NAME_TTYACM2_1,
  UART_DEV_NAME_TTYACM2_2,
  UART_DEV_NAME_TTYACM2_3,
  UART_DEV_NAME_TTYACM2_4,
  UART_DEV_NAME_TTYACM3_0,
  UART_DEV_NAME_TTYACM3_1,
  UART_DEV_NAME_TTYACM3_2,
  UART_DEV_NAME_TTYACM3_3,
  UART_DEV_NAME_TTYACM3_4,
  UART_DEV_NAME_TTYACM4_0,
  UART_DEV_NAME_TTYACM4_1,
  UART_DEV_NAME_TTYACM4_2,
  UART_DEV_NAME_TTYACM4_3,
  UART_DEV_NAME_TTYACM4_4,
  UART_DEV_NAME_TTYACM5_0,
  UART_DEV_NAME_TTYACM5_1,
  UART_DEV_NAME_TTYACM5_2,
  UART_DEV_NAME_TTYACM5_3,
  UART_DEV_NAME_TTYACM5_4,
};


/*********************************************
 �������ܵ�Ԫ: gpio
 ��������:  set gpio status
 �������: ��
 �������: ��
 ��������ֵ: ��
 �ڲ��������̣�all
**********************************************/
static int32 set_gpio_value(uint32 pin, uint32 value)
{
    int32 fd;
    int32 ret_value;
    int32 count = 100;

    //��
    while(((fd = open(GPIO_DEV_NAME, O_RDONLY))<0)&&((count--)>0))
            usleep(2000);

    if (fd < 0)
    {
      printf("can not open gpio device.\n");
      return ERR_NORMAL;
    }

    //(0 low | 1 high | <0 error)
    ret_value = ioctl(fd, value, pin);

    //�ر�
    close(fd);

    return ret_value;
}

/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：锁串口
    输入参数�
        fd 串口文件句柄
        type 锁类�?F_RDLCK 共享锁；F_WRLCK 互斥�
    输出参数�
        �?
    函数返回值：

    内部处理流程�
********************************************************************/
int setlock(int fd, int type)
{
    struct flock lock;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_type = type;

    if (fcntl(fd, F_SETLK, &lock) == 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}


/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：设置串口参数，包括数据位、停止位、奇偶校�
    输入参数�
        fd				打开串口的文件句�
        databits	串口数据�
        stopbits	停止�
        parity		奇偶校验
        set_crtscts	硬件流控 1-设置 0-不设�
    输出参数�
        �
    函数返回值：
        TRUE			成功
        FALSE			失败
    内部处理流程：按输入参数进行串口参数设置
********************************************************************/
int set_Parity(const int fd, const signed char databits, const signed char stopbits, const signed char parity, const signed char set_crtscts)
{
    struct termios options;
    if  ( tcgetattr( fd,&options)  !=  0) /*读取串口termios结构*/
    {
        return(FALSE);
    }

    /*设置为不加工模式, 不回�*/
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag = IGNBRK;
    options.c_cflag |= CLOCAL | CREAD;

    options.c_cflag &= ~CSIZE; 	/*清除原字符大*/
    switch (databits) /*设置数据位数*/
    {
        case 7:
            options.c_cflag |= CS7;  /*设置7位数据位�*/
            break;
        case 8:
            options.c_cflag |= CS8;  /*设置8位数据位�*/
            break;
        default:
            return (FALSE);
    }

    switch (parity)
    {
        case 'n':	/*无校�*/
        case 'N':
            options.c_cflag &= ~PARENB;   /* 输入奇偶校验无效 */
            break;

        case 'o': /*奇校�*/
        case 'O':
            options.c_cflag |= (PARODD | PARENB);	/*设置为奇效验*/
            //options.c_iflag |= INPCK;		/*对输入进行奇偶检�*/
            break;

        case 'e': /*偶校�*/
        case 'E':
            options.c_cflag &= ~PARODD;
            options.c_cflag |= PARENB;  /* 使奇偶校验有�*/
            //options.c_iflag |= INPCK;		/*对输入进行奇偶检�*/
            break;

        case 'S': /*space校验*/
        case 's':
            options.c_cflag &= ~CSTOPB;
            break;

        default:
            return (FALSE);
    }

    /* 设置停止�*/
    switch (stopbits)
    {
        case 1: /*1位停止位*/
            options.c_cflag &= ~CSTOPB;
            break;
        case 2: /*2位停止位*/
            options.c_cflag |= CSTOPB;
           break;
        default:
             return (FALSE);
    }

    tcflush(fd,TCIFLUSH);					/* 清输入队�*/
    options.c_cc[VTIME] = 100; 		/* 设置超时10 seconds*/
    options.c_cc[VMIN] = 0; 			/* 更新设置，使马上生效 */

// 	options.c_iflag |= (IXON | IXOFF);	/*开软件流控*/
    options.c_iflag &= ~(IXON|IXOFF|IXANY);	/*关软件流�*/

    if (set_crtscts == 1)
        options.c_cflag |= CRTSCTS;	/*开硬件流控*/
    else
        options.c_cflag &= ~CRTSCTS;/*关硬件流�*/

    options.c_iflag &= ~IGNCR;	/*不忽略收到的CR字符*/
    options.c_iflag &= ~ICRNL;

    if (tcsetattr(fd,TCSANOW,&options) != 0)  /* 对串口进行参数设�?*/
    {
        return (FALSE);
    }
    return (TRUE);
}


/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：设置串口波特率,用于串口发送AT命令时，对串口的设置
    输入参数�
        fd		打开串口的文件句�
        speed	串口波特�
    输出参数�
        �
    函数返回值：
        �?
    内部处理流程：先读取termios结构，然后调用cfsetispeed cfsetospeed�
                                设置串口的输入、输出波特率�
********************************************************************/
int set_speed(int fd, int speed)
{
    int i;
    int status;
  int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300};
  int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300};

    struct termios Opt; 	/*Opt中存储波特率数据结构*/
    tcgetattr(fd, &Opt); 	/*获取termios结构*/

    for ( i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if (speed == name_arr[i])	/*查找支持的波特率*/
        {
            cfsetispeed(&Opt, speed_arr[i]); /*设置输入波特�*/
            cfsetospeed(&Opt, speed_arr[i]); /*设置输出波特�*/
            status = tcsetattr(fd, TCSANOW, &Opt);  /*设置串口新的termios结构*/
            if  (status != 0)
            {
                return -1;
            }

            tcflush(fd,TCIOFLUSH); /*清除串口的输入输出队列中的数�*/
            return 1;
        }
    }
    return -1;
}


/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：打开串口,进行串口参数设置
    输入参数�
        fd					打开串口的文件句�
        dwBaudRate	串口波特�
        byByteSize	数据�
        byStopBits	停止�
        byParity		奇偶校验
        set_crtscts	硬件流控 1-设置 0-不设�

    输出参数�
        �
    函数返回值：
        fd	打开设置串口成功
        -1	打开串口失败

    内部处理流程�

********************************************************************/
int32 OpenSerial(struct tag_UART_DEVICE *dev, const char *DeviceName,
                                            const int32 dwBaudRate,
                                            const unsigned char byByteSize,
                                            const unsigned char byStopBits,
                                            const unsigned char byParity,
                                            const signed char set_crtscts)
{
    if(dev->fd <= 0)
    {
        dev->fd = open(DeviceName, O_RDWR|O_NOCTTY|O_NONBLOCK );	/*打开串口*/
        if(dev->fd > 0)
        {
            if (setlock(dev->fd, F_WRLCK) == -1)    /*锁串�*/
            {
                DEBUG("setlock %s false!\n", DeviceName);
                close(dev->fd);
                dev->fd = 0;
                return -1;
            }
        }
    }

    if (dev->fd <= 0)		/*若打开串口失败，返�?1，并通知应用�*/
    {
        DEBUG("open %s false!\n", DeviceName);
        return -1;
    }

    if (set_speed(dev->fd, dwBaudRate) == -1)	/*设置串口波特�*/
    {
        DEBUG("set_speed %s false!\n", DeviceName);
        close(dev->fd);
        dev->fd = 0;
        return -1;
    }

    if (set_Parity(dev->fd, byByteSize, byStopBits, byParity, set_crtscts) == FALSE) /*设置串口参数 数据�?校验 停止�*/
    {
        DEBUG("set_Parity %s false!\n", DeviceName);
        close(dev->fd);
        dev->fd = 0;
        return -1;
    }

    return ERR_OK;
}


/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：打开串口,进行串口参数设置
    输入参数�
        fd					打开串口的文件句�
        speed   	        串口波特�
        read_len            最大长�
    输出参数�
        buf
    函数返回值：
        数据长度

    内部处理流程�
*********************************************************************/
int ReadSerial(int fd_232, int speed, uint8 *buf, int read_len)
{
    int recv_len;										/*接收到数据长�*/
    int data_len;										/*串口缓冲区中数据的长�*/
    int count=0;
    fd_set set;
    struct timeval timeout;
    int n = 0;

    recv_len = 0;
    data_len = 0;

    while (1)
    {
        FD_ZERO(&set);
        FD_SET(fd_232, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = 20000;
        do {
              n = select(fd_232+1, &set, NULL, NULL, &timeout);
        }while(n<0);
        if (n > 0)
              recv_len = read(fd_232, buf+data_len, read_len-data_len);	/*调用read()，进行串口数据接�*/
        else
              recv_len = 0;

        if (recv_len > 0)
        {
              data_len += recv_len;
              count = 0;
        }
        else
        {
            if((data_len == 0)&&(count>5))
            {
                  usleep(20000);
                  return data_len; /*-接收数据超时-*/
            }

            if((data_len != 0)&&((count>5)||(data_len>read_len)))
            {
                  usleep(20000);
                  return data_len;
            }
        }

        count++;
    }

    return data_len;
}

/****************************************************
* @brief ���豸ID��ת������Ӧ������
* @param[in] id: �豸ID��
* @return �ɹ������豸�ţ�ʧ�ܷ���TTYNULL
****************************************************/
static int uart_id_to_num(const char *id)
{
    if(id == NULL)
    {
        return ERR_PTR;
    }

    if(strcmp(id, UART_RS4852_DEVICE_ID_STR) == 0)
    {
        return TTYS3;
    }
    else if(strcmp(id, UART_RS4851_DEVICE_ID_STR) == 0)
    {
        return TTYS4;
    }
    else if(strcmp(id, UART_DEVICE_BLE_STR) == 0)
    {
        return TTYS9;
    }
    else if(strcmp(id, UART_DEVICE_ACPORT_STR) == 0)
    {
        return TTYS8;
    }
    else if(NULL != strstr(id, UART_ECU_DEVICE_ID_STR) && strlen(id) == strlen(UART_ECU_DEVICE_ID_STR) + 3)
    {
        uint8 port = 0;
        uint8 ifnum = 0;
        port = id[strlen(UART_ECU_DEVICE_ID_STR)] - '0';
        ifnum = id[strlen(UART_ECU_DEVICE_ID_STR) + 2] - '0';

        if(1 <= port && port <= 5 && 0 <= ifnum && ifnum <= 4)
        {
            return TTYS_ACM1_0 + (port - 1) * 5 + ifnum;
        }
    }

    return TTYNULL;
}


/****************************************************
* @brief ���ô���ͨ�Ų���
* @param[in] dev: �豸����
* @param[in] baud ������
* @param[in] databits ����λ(5~8)
* @param[in] stopbits ֹͣλ(1~2)
* @param[in] parity У��λ(N ��У�飬E żУ�飬O��У��)
* @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1
****************************************************/
static int32 uart_set_param(struct tag_UART_DEVICE *dev,
                          uint32 speed,
                          uint32 databits,
                          uint32 stopbits,
                          PARITY_TYPE_E parity)
{
    int32 iRet = -1;
    signed char paritys;
    int32 uport = 0;

    if(dev == NULL)
    {
        ////DEBUG("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }
    ////DEBUG("********deviceID %s********\n", dev->base.szDeviceID);

    if (0 != strlen(dev->devName))
    {
        ////DEBUG(" %s \n", uart_device[uport]);
        switch (parity)
        {
            case PARITY_NO:
                paritys = 'n';
                break;

            case PARIGY_ODD:
                paritys = 'o';
                break;

            case PARITY_EVEN:
                paritys = 'e';
                break;

            default:
                paritys = 'n';
                break;
        }

        iRet = OpenSerial(dev, dev->devName, speed, databits, stopbits, paritys, 0);
    }
    else
    {
          iRet = ERR_NORMAL;
    }

    /* ��������, Ĭ�Ͻ������� */
    uport = uart_id_to_num(dev->base.szDeviceID);
    if(ERR_OK == iRet)
    {
        if(uport == TTYS3)  //485-2
        {
            tcdrain(dev->fd);
            //set_gpio_value(PIN_485_2_DIR , GPIO_OUT_LOW);
        }
        if(uport == TTYS4)  //485-1
        {
            tcdrain(dev->fd);
            //set_gpio_value(PIN_485_1_DIR , GPIO_OUT_LOW);
        }
    }
    DEBUG("uart_set_param %s ret %d\n", dev->base.szDeviceID,iRet);

    return iRet;
}


/****************************************************
* @brief �Ӵ��ڽ�������
* @param[in] dev: �豸����
* @param[out] buf ���ջ�����
* @param[in] len ����������
* @return �ɹ����ش���0�����ݳ��ȣ�ʧ�ܷ���FALSE
****************************************************/
static int uart_recv_data(struct tag_UART_DEVICE *dev,
                          uint8 *buf,
                          uint32 len)
{
    //int   ret = ERR_NORMAL;
    UART_DEVICE_T *pDev = (UART_DEVICE_T *)dev;
    int32 fd;
    int   recvlen = 0;

    fd = pDev->fd;
    if(fd <= 0)
    {
        return ERR_ACCESS;
    }

    /* 801��485���ջ���ֻ��8�ֽڣ��ӿ���Ҫ�������ղ�û������ */
/*     while(recvlen < len)
    {
        ret = read(fd, buf + recvlen, len - recvlen);
        if(ret > 0)
        {
            recvlen += ret;
            usleep(5000);
        }
        else
        {
            break;
        }
    } */
    /* hal����ղ�����ʱ Ҫ��Ȼ��Ӧ�ò㲻���� */
    recvlen = read(fd, buf , len);
/*     if(recvlen > 0)
    {
        DEBUG("recv %d time %ld buf[", recvlen,time(NULL));
        uint32 i;
        for(i=0;i<recvlen;i++)
        {
            printf("%02x ",buf[i]);
        }
        printf("]\n");
    } */
    return recvlen;
}


/****************************************************
* @brief ��ȡ���ھ��
* @param[in] dev: �豸����
* @return���ش��ڴ�ʱ�ľ��ֵ������>0���쳣<=0����
****************************************************/
static int uart_get_handle(struct tag_UART_DEVICE *dev)
{
    int32 fd = -1;

    if (dev == NULL)
    {
        return ERR_NORMAL;
    }

    fd = dev->fd;

    return fd;
}


/****************************************************
* @brief ͨ�����ڷ�������
* @param[in] dev: �豸����
* @param[in] buf ���ͻ�����
* @param[in] len ����������
* @return�ɹ�����ERR_OK��ʧ�ܷ��ش����롣�������¼1��
****************************************************/
static int uart_send_data(struct tag_UART_DEVICE *dev,
                          const uint8 *buf,
                          int32 len)
{
    int32 send_len = 0;
    int32 uport = 0;

    if(dev == NULL)
    {
        ////DEBUG("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }

    if (dev->fd <= 0)
    {
          DEBUG(" Serial Port fd error fd %d!\n", dev->fd);
          return ERR_INVALID;
    }

    uport = uart_id_to_num(dev->base.szDeviceID);
    //if(uport == TTYS3)  //485-2
    //{
    //    set_gpio_value(PIN_485_2_DIR , GPIO_OUT_HIGH);
    //}
    //if(uport == TTYS4)  //485-1
    //{
    //    set_gpio_value(PIN_485_1_DIR , GPIO_OUT_HIGH);
    //}

    send_len = write(dev->fd, buf, len);
/*     uint32 i;
    DEBUG("dev %s fd %d time %ld sendlen %d buf [ ",dev->devName, dev->fd,time(NULL),len);
    for(i=0;i<len;i++)
    {
        printf("%02x ",buf[i]);
    }
    printf("]\n"); */
    if(uport == TTYS3)  //485-2
    {
        tcdrain(dev->fd);
        //set_gpio_value(PIN_485_2_DIR , GPIO_OUT_LOW);
    }
    if(uport == TTYS4)  //485-1
    {
        tcdrain(dev->fd);
        //set_gpio_value(PIN_485_1_DIR , GPIO_OUT_LOW);
    }

    if(send_len == len)
        return ERR_OK;
      else
          return ERR_NORMAL;
}

/**
* @brief ��ȡ������ʵ���豸�ڵ���
* @param[in] dev: �豸����
* @param[out] dev_name: �豸�ڵ���
* @return���ػ�ȡ�����豸�ڵ����ĳ��ȡ�
*/
int32 uart_get_real_dev_name(struct tag_UART_DEVICE *dev, uint8 *dev_name)
{
    if(NULL == dev || NULL == dev_name || NULL == dev->devName)
    {
        return 0;
    }
    memcpy(dev_name, dev->devName, strlen(dev->devName));

    return strlen(dev->devName);
}

/**
* @brief ��ȡ����״̬
* @param[in] dev: �豸����
* @return���ش��ڵ�ǰ״̬������>=0���쳣<0����
*/
int32 uart_get_status(struct tag_UART_DEVICE *dev)
{
    struct termios options;

    if(NULL == dev)
    {
        return ERR_PTR;
    }

    if( tcgetattr(dev->fd,&options)  !=  0)
    {
        return -1;
    }

    return 0;
}

static int uart_open(HW_MODULE *module, const char* device_id,
         HW_DEVICE **device)
{
    /* �˴�ͨ���жϴ����device id��ƥ�䵽ָ����Ӳ���豸������ͬһ��
    module�¶��Ӳ���豸�����������uart��������������£��豸˽��
    ��tag_HW_RTC����Ҳ��Ҫ������ */

    UART_DEVICE_T* dev = (UART_DEVICE_T*)malloc(sizeof(UART_DEVICE_T));
    if(dev == NULL)
    {
        //DEBUG("malloc failed \n");
        return ERR_MEM;
    }

    memset(dev, 0, sizeof(UART_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;

    dev->uart_param_set = uart_set_param;
    dev->uart_data_recv = uart_recv_data;
    dev->uart_data_send = uart_send_data;
    dev->uart_get_handle = uart_get_handle;
    dev->uart_get_real_dev_name = uart_get_real_dev_name;
    dev->uart_get_status = uart_get_status;
    dev->fd              = 0;

    int32 uport = uart_id_to_num(dev->base.szDeviceID);
    if ((uport >=0) && (uport != TTYNULL))
    {
        memcpy(dev->devName, uart_device[uport], strlen(uart_device[uport]));
    }
    else
    {
        return ERR_NOTEXIST;
    }

    *device = (HW_DEVICE*)dev;

    return ERR_OK;
}


static int uart_close(HW_MODULE *pModule, HW_DEVICE *pDevice )
{
    UART_DEVICE_T *dev = (UART_DEVICE_T*)pDevice;
    if(dev == NULL)
    {
        //DEBUG("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }

    if (dev->fd > 0)
    {
        setlock(dev->fd, F_UNLCK);
        close(dev->fd);
        dev->fd = 0;
    }

    free(dev);
    return ERR_OK;
}

/* ��ʼ��ģ����� */
UART_MODULE HAL_MODULE_INFO_SYM =
{
    .common =
    {
        .szName = "uart",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-6-15",
        .nDevNum  = UART_DEV_NUM,
        .szDevices = aUartDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = uart_open,
        .pfClose = uart_close,
    },
};

