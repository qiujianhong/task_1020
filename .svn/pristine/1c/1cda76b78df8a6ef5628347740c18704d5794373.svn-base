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


#define   UART_DEV_NAME_USB0       "/dev/ttyUSB0" 
#define   UART_DEV_NAME_USB1       "/dev/ttyUSB1" 
#define   UART_DEV_NAME_USB2       "/dev/ttyUSB2"
#define   UART_DEV_NAME_USB3       "/dev/ttyUSB3"

#define   UART_SYS_DEV0_PATH       "/sys/bus/usb/devices/3-1:1.0/tty"
#define   UART_SYS_DEV1_PATH       "/sys/bus/usb/devices/3-1:1.2/tty"
#define   UART_SYS_DEV2_PATH       "/sys/bus/usb/devices/3-1:1.4/tty"
#define   UART_SYS_DEV3_PATH       "/sys/bus/usb/devices/3-1:1.6/tty"

#define CONTROL_SET_RTS     0X6501
#define CONTROL_CLEAR_RTS   0X6502
#define CONTROL_GET_RTSSTATE 0X6503
#define CONTROL_START_38K   0X6504
#define CONTROL_STOP_38K    0X6505

enum
{
    TTYS1=0,
    TTYS2,
    TTYS3,
    TTYS4,
    TTYS6,
    TTYS7,
    TTYS8,
    TTYS9,
    TTYS_USB_0,
    TTYS_USB_1,
    TTYS_USB_2,
    TTYS_USB_3,

    TTYNULL=255,
};

 #define UART_DEV_NUM      11
static char *aUartDevice[UART_DEV_NUM] = {
                     UART_IR_DEVICE_ID_STR,
                     UART_1_1_DEVICE_ID_STR,
                     UART_RS4852_DEVICE_ID_STR,
                     UART_RS4851_DEVICE_ID_STR,
                     UART_232_DEVICE_ID_STR,
                     UART_DEVICE_BLE_STR,
                     UART_DEVICE_ACPORT_STR,
                     UART_REMOTE_CU_DEVICE_ID_STR,
                     
                     UART_USB_MANAGER_DEVICE_ID_STR,
                     UART_USB_AT_DEVICE_ID_STR ,
                     UART_USB_DIAG_DEVICE_ID_STR
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

/************************************************************
* @brief TTY���Ϳ���
* @param[in] ttys_no: �������
* @param[in] cmd:  ����
* @return �ɹ�����ERR_OK��ʧ�ܷ��ش����룬�������¼1
****************************************************/
static int tty_ioctl(uint32 ttys_no, uint32 cmd)
{
    int fd =0;
    int iret = -1;

    if((ttys_no <0) || (ttys_no >10) )
       return -EINVAL;

    fd = open("/dev/tty_io", O_RDONLY);
    if(fd <= 0)
       return -EPERM;

    iret = ioctl(fd, cmd,  ttys_no);

    close(fd);

    return iret;
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
#if 0
    else
    {
        if(strcmp(dev->base.szDeviceID, UART_RS4851_DEVICE_ID_STR) == 0)
        {
            DEBUG("exec io cmd io -4 0xff148008 0x00000041!\n"); 
            system("io -4 0xff148008 0x00000041");
        }
        else if(strcmp(dev->base.szDeviceID, UART_LOCAL_CU_DEVICE_ID_STR) == 0)
        {
            DEBUG("exec io cmd io -4 0xff170008 0x00000041!\n"); 
            system("io -4 0xff170008 0x00000041");
        }
    }
#endif
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
        //DEBUG(" pointer is null!\n");
        return ERR_PTR;
    }

    if(strcmp(id, UART_IR_DEVICE_ID_STR) == 0)
    {
        return TTYS1;
    }
    else if(strcmp(id, UART_1_1_DEVICE_ID_STR) == 0)
    {
        return TTYS2;
    }
    else if(strcmp(id, UART_RS4852_DEVICE_ID_STR) == 0)
    {
        return TTYS3;
    }
    else if(strcmp(id, UART_RS4851_DEVICE_ID_STR) == 0)
    {
        return TTYS4;
    }
    else if(strcmp(id, UART_232_DEVICE_ID_STR) == 0)
    {
        return TTYS6;
    }
    else if(strcmp(id, UART_DEVICE_BLE_STR) == 0)
    {
        return TTYS7;
    }
    else if(strcmp(id, UART_DEVICE_ACPORT_STR) == 0)
    {
        return TTYS8;
    }
    else if(strcmp(id, UART_REMOTE_CU_DEVICE_ID_STR) == 0)
    {
        return TTYS9;
    }
    else if(strcmp(id, UART_USB_MANAGER_DEVICE_ID_STR) == 0)
    {
        return TTYS_USB_0;
    }
    else if(strcmp(id, UART_USB_AT_DEVICE_ID_STR) == 0)
    {
        return TTYS_USB_1;
    }
    else if(strcmp(id, UART_USB_DIAG_DEVICE_ID_STR) == 0)
    {
        return TTYS_USB_2;
    }

    return TTYNULL;
}

int uart_id_to_usbdev(const char *id, char *dev, int devlen)
{
    DIR           *dirp;
    struct dirent *entp;
    char subpath[312] = {0};

    if(0 == strcmp(id, UART_USB_MANAGER_DEVICE_ID_STR))
    {
        /* ���ж�USB */
        if(0 == access(UART_DEV_NAME_USB0, F_OK))
        {
            memcpy(dev, UART_DEV_NAME_USB0, strlen(UART_DEV_NAME_USB0));
            return strlen(UART_DEV_NAME_USB0);
        }
        dirp = opendir(UART_SYS_DEV0_PATH);
    }
    else if(0 == strcmp(id, UART_USB_AT_DEVICE_ID_STR))
    {
        /* ���ж�USB */
        if(0 == access(UART_DEV_NAME_USB1, F_OK))
        {
            memcpy(dev, UART_DEV_NAME_USB1, strlen(UART_DEV_NAME_USB1));
            return strlen(UART_DEV_NAME_USB1);
        }
        dirp = opendir(UART_SYS_DEV1_PATH);
    }
    else if(0 == strcmp(id, UART_USB_DIAG_DEVICE_ID_STR))
    {
        /* ���ж�USB */
        if(0 == access(UART_DEV_NAME_USB2, F_OK))
        {
            memcpy(dev, UART_DEV_NAME_USB2, strlen(UART_DEV_NAME_USB2));
            return strlen(UART_DEV_NAME_USB2);
        }
        dirp = opendir(UART_SYS_DEV2_PATH);
    }
    else
    {
        return 0;
    }
    
    if (NULL == dirp)
    {
        return 0;
    }

    while((entp = readdir(dirp)) != NULL)
    {
        if((strcmp(entp->d_name, ".") == 0) || (strcmp(entp->d_name, "..") == 0))
        {
            continue;
        }
        sprintf(subpath, "/dev/%s", entp->d_name);
        break;
    }
    closedir(dirp);

    if(devlen >= strlen(subpath))
    {
        memcpy(dev, subpath, strlen(subpath));
    }
    else
    {
        memcpy(dev, subpath, devlen);
    }
    return strlen(subpath);
}

/****************************************************
* @brief ���豸ID��ת������Ӧ������
* @param[in] id: �豸ID��
* @return �豸�ַ�������
****************************************************/
static int uart_id_to_devpath(const char *id, char *dev, int devlen)
{
    if(id == NULL || NULL == dev || 0 == devlen)
    {
        return 0;
    }

    memset(dev, 0, devlen);

    if(strcmp(id, UART_IR_DEVICE_ID_STR) == 0)
    {
        memcpy(dev, UART_DEV_NAME_TTYS1, strlen(UART_DEV_NAME_TTYS1));
        return strlen(UART_DEV_NAME_TTYS1);
    }
    else if(strcmp(id, UART_1_1_DEVICE_ID_STR) == 0)
    {
        memcpy(dev, UART_DEV_NAME_TTYS2, strlen(UART_DEV_NAME_TTYS2));
        return strlen(UART_DEV_NAME_TTYS2);
    }
    else if(strcmp(id, UART_RS4851_DEVICE_ID_STR) == 0)
    {
        memcpy(dev, UART_DEV_NAME_TTYS4, strlen(UART_DEV_NAME_TTYS4));
        return strlen(UART_DEV_NAME_TTYS4);
    }
    else if(strcmp(id, UART_RS4852_DEVICE_ID_STR) == 0)
    {
        memcpy(dev, UART_DEV_NAME_TTYS3, strlen(UART_DEV_NAME_TTYS3));
        return strlen(UART_DEV_NAME_TTYS3);
    }
    else if(strcmp(id, UART_232_DEVICE_ID_STR) == 0)
    {
        memcpy(dev, UART_DEV_NAME_TTYS6, strlen(UART_DEV_NAME_TTYS6));
        return strlen(UART_DEV_NAME_TTYS6);
    }
    else if(strcmp(id, UART_DEVICE_BLE_STR) == 0)
    {
        memcpy(dev, UART_DEV_NAME_TTYS7, strlen(UART_DEV_NAME_TTYS7));
        return strlen(UART_DEV_NAME_TTYS7);
    }
    else if(strcmp(id, UART_DEVICE_ACPORT_STR) == 0)
    {
        memcpy(dev, UART_DEV_NAME_TTYS8, strlen(UART_DEV_NAME_TTYS8));
        return strlen(UART_DEV_NAME_TTYS8);
    }
    else if(strcmp(id, UART_REMOTE_CU_DEVICE_ID_STR) == 0)
    {
        memcpy(dev, UART_DEV_NAME_TTYS9, strlen(UART_DEV_NAME_TTYS9));
        return strlen(UART_DEV_NAME_TTYS9);
    }
    else if(0 == strcmp(id, UART_USB_MANAGER_DEVICE_ID_STR) || 0 == strcmp(id, UART_USB_AT_DEVICE_ID_STR)||
            0 == strcmp(id, UART_USB_DIAG_DEVICE_ID_STR))
    {
        return uart_id_to_usbdev(id, dev, devlen);
    }

    return 0;
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
    int32 uport = 0;
    signed char paritys;

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
        //DEBUG("unsupport uart(%d)\n",  uport);
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

        if(uport == TTYS1)
        {
            tcdrain(dev->fd);
            tty_ioctl(uport, CONTROL_STOP_38K);
        }
    }
    return iRet;
}


/****************************************************
* @brief �Ӵ��ڽ�������
* @param[in] dev: �豸����
* @param[out] buf ���ջ�����
* @param[in] len ����������
* @return �ɹ����ش���0�����ݳ��ȣ�ʧ�ܷ���FALSE
****************************************************/
static int uart_recv_data(struct tag_UART_DEVICE *dev,        uint8 *buf, uint32 len)
{
    int   ret = ERR_NORMAL;
    UART_DEVICE_T *pDev = (UART_DEVICE_T *)dev;
    int32 fd;
    int   recvlen = 0;
    
    fd = pDev->fd;
    if(fd <= 0)
    {
        return ERR_ACCESS;
    }

    /* 801��485���ջ���ֻ��8�ֽڣ��ӿ���Ҫ�������ղ�û������ */
    while(recvlen < len)
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
    }

#if 0
    if(recvlen > 0 && 0 == strcmp(pDev->base.szDeviceID, UART_RS4851_DEVICE_ID_STR))
    {
        struct timeval tv;
        struct tm     *stime = NULL;
        uint16 iii;
        gettimeofday(&tv, NULL);
        stime = localtime(&tv.tv_sec);
        printf("[%04d-%02d-%02d %02d:%02d:%02d.%03ld]recvlen %d:", stime->tm_year + 1900, stime->tm_mon + 1, stime->tm_mday, 
                stime->tm_hour, stime->tm_min, stime->tm_sec, tv.tv_usec/1000, recvlen);
        for(iii = 0; iii < recvlen; iii++)
        {
          printf("%02x ", buf[iii]);
        }
        printf("\n");
    }
#endif
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

    if(uport == TTYS1)
    {
        tty_ioctl(uport, CONTROL_START_38K);
    }
    //if(uport == TTYS3)  //485-2
    //{
    //    set_gpio_value(PIN_485_2_DIR , GPIO_OUT_HIGH);
    //}
    //if(uport == TTYS4)  //485-1
    //{
    //    set_gpio_value(PIN_485_1_DIR , GPIO_OUT_HIGH);
    //}
 
    send_len = write(dev->fd, buf, len);

    if(uport == TTYS1)
    {
        tcdrain(dev->fd);
        tty_ioctl(uport, CONTROL_STOP_38K);
    }
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
    UART_DEVICE_T* dev = NULL;
    char pathname[48] = {0};
    int  namelen = 0;
    
    namelen = uart_id_to_devpath(device_id, pathname, 48 - 1);
    if(0 == namelen)
    {
        return ERR_NOTEXIST;
    }
    
    dev = (UART_DEVICE_T*)malloc(sizeof(UART_DEVICE_T));
    if(dev == NULL)
    {
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
    memcpy(dev->devName, pathname, namelen);

    if(0 == strcmp(device_id, UART_RS4851_DEVICE_ID_STR))
    {
        nice(-20);  //485-1������ߵĵ������ȼ�
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
        .szBuildDate = "2022-5-25",
        .nDevNum  = UART_DEV_NUM,
        .szDevices = aUartDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = uart_open,
        .pfClose = uart_close,
    },
};

