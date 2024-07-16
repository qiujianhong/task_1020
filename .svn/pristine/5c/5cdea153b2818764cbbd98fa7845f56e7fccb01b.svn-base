#include <uart.h>
//#include <sys/types.h>
//#include <unistd.h>

#define UART_DEVICE_ID_START       "uart_acm" /* ģ�鴮����ʼ���� */

/*uart �豸·�� */
#define   UART_DEV_NAME_TTYS0      "/dev/ttyS0" 
#define   UART_DEV_NAME_TTYS1      "/dev/ttyS1" 
#define   UART_DEV_NAME_TTYS2      "/dev/ttyS2" 
#define   UART_DEV_NAME_TTYS3      "/dev/ttyS3" 
#define   UART_DEV_NAME_TTYS4      "/dev/ttySz4" 
#define   UART_DEV_NAME_TTYS5      "/dev/ttyS5" 
#define   UART_DEV_NAME_TTYS6      "/dev/ttyS6"

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

#define   UART_DEV_NAME_TTYACM1_1_0    "/tmp/dev/usb_exm1_1_0" 
#define   UART_DEV_NAME_TTYACM1_1_1    "/tmp/dev/usb_exm1_1_1"
#define   UART_DEV_NAME_TTYACM1_1_2    "/tmp/dev/usb_exm1_1_2"
#define   UART_DEV_NAME_TTYACM1_1_3    "/tmp/dev/usb_exm1_1_3"
#define   UART_DEV_NAME_TTYACM1_1_4    "/tmp/dev/usb_exm1_1_4"
#define   UART_DEV_NAME_TTYACM1_2_0    "/tmp/dev/usb_exm1_2_0" 
#define   UART_DEV_NAME_TTYACM1_2_1    "/tmp/dev/usb_exm1_2_1"
#define   UART_DEV_NAME_TTYACM1_2_2    "/tmp/dev/usb_exm1_2_2"
#define   UART_DEV_NAME_TTYACM1_2_3    "/tmp/dev/usb_exm1_2_3"
#define   UART_DEV_NAME_TTYACM1_2_4    "/tmp/dev/usb_exm1_2_4"

#define   UART_DEV_NAME_TTYACM2_1_0    "/tmp/dev/usb_exm2_1_0" 
#define   UART_DEV_NAME_TTYACM2_1_1    "/tmp/dev/usb_exm2_1_1"
#define   UART_DEV_NAME_TTYACM2_1_2    "/tmp/dev/usb_exm2_1_2"
#define   UART_DEV_NAME_TTYACM2_1_3    "/tmp/dev/usb_exm2_1_3"
#define   UART_DEV_NAME_TTYACM2_1_4    "/tmp/dev/usb_exm2_1_4"
#define   UART_DEV_NAME_TTYACM2_2_0    "/tmp/dev/usb_exm2_2_0" 
#define   UART_DEV_NAME_TTYACM2_2_1    "/tmp/dev/usb_exm2_2_1"
#define   UART_DEV_NAME_TTYACM2_2_2    "/tmp/dev/usb_exm2_2_2"
#define   UART_DEV_NAME_TTYACM2_2_3    "/tmp/dev/usb_exm2_2_3"
#define   UART_DEV_NAME_TTYACM2_2_4    "/tmp/dev/usb_exm2_2_4"

#define   UART_DEV_NAME_TTYACM3_1_0    "/tmp/dev/usb_exm3_1_0" 
#define   UART_DEV_NAME_TTYACM3_1_1    "/tmp/dev/usb_exm3_1_1"
#define   UART_DEV_NAME_TTYACM3_1_2    "/tmp/dev/usb_exm3_1_2"
#define   UART_DEV_NAME_TTYACM3_1_3    "/tmp/dev/usb_exm3_1_3"
#define   UART_DEV_NAME_TTYACM3_1_4    "/tmp/dev/usb_exm3_1_4"
#define   UART_DEV_NAME_TTYACM3_2_0    "/tmp/dev/usb_exm3_2_0" 
#define   UART_DEV_NAME_TTYACM3_2_1    "/tmp/dev/usb_exm3_2_1"
#define   UART_DEV_NAME_TTYACM3_2_2    "/tmp/dev/usb_exm3_2_2"
#define   UART_DEV_NAME_TTYACM3_2_3    "/tmp/dev/usb_exm3_2_3"
#define   UART_DEV_NAME_TTYACM3_2_4    "/tmp/dev/usb_exm3_2_4"

#define   UART_DEV_NAME_TTYACM4_1_0    "/tmp/dev/usb_exm4_1_0" 
#define   UART_DEV_NAME_TTYACM4_1_1    "/tmp/dev/usb_exm4_1_1"
#define   UART_DEV_NAME_TTYACM4_1_2    "/tmp/dev/usb_exm4_1_2"
#define   UART_DEV_NAME_TTYACM4_1_3    "/tmp/dev/usb_exm4_1_3"
#define   UART_DEV_NAME_TTYACM4_1_4    "/tmp/dev/usb_exm4_1_4"
#define   UART_DEV_NAME_TTYACM4_2_0    "/tmp/dev/usb_exm4_2_0" 
#define   UART_DEV_NAME_TTYACM4_2_1    "/tmp/dev/usb_exm4_2_1"
#define   UART_DEV_NAME_TTYACM4_2_2    "/tmp/dev/usb_exm4_2_2"
#define   UART_DEV_NAME_TTYACM4_2_3    "/tmp/dev/usb_exm4_2_3"
#define   UART_DEV_NAME_TTYACM4_2_4    "/tmp/dev/usb_exm4_2_4"

#define   UART_DEV_NAME_TTYACM5_1_0    "/tmp/dev/usb_exm5_1_0" 
#define   UART_DEV_NAME_TTYACM5_1_1    "/tmp/dev/usb_exm5_1_1"
#define   UART_DEV_NAME_TTYACM5_1_2    "/tmp/dev/usb_exm5_1_2"
#define   UART_DEV_NAME_TTYACM5_1_3    "/tmp/dev/usb_exm5_1_3"
#define   UART_DEV_NAME_TTYACM5_1_4    "/tmp/dev/usb_exm5_1_4"
#define   UART_DEV_NAME_TTYACM5_2_0    "/tmp/dev/usb_exm5_2_0" 
#define   UART_DEV_NAME_TTYACM5_2_1    "/tmp/dev/usb_exm5_2_1"
#define   UART_DEV_NAME_TTYACM5_2_2    "/tmp/dev/usb_exm5_2_2"
#define   UART_DEV_NAME_TTYACM5_2_3    "/tmp/dev/usb_exm5_2_3"
#define   UART_DEV_NAME_TTYACM5_2_4    "/tmp/dev/usb_exm5_2_4"

enum
{
    TTYS0=0,
    TTYS1,
    TTYS2,
    TTYS3,
    TTYS4,
    TTYS5,
    TTYS6,
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

       TTYS_ACM1_1_0,
    TTYS_ACM1_1_1,
    TTYS_ACM1_1_2,
    TTYS_ACM1_1_3,
    TTYS_ACM1_1_4,
       TTYS_ACM1_2_0,
    TTYS_ACM1_2_1,
    TTYS_ACM1_2_2,
    TTYS_ACM1_2_3,
    TTYS_ACM1_2_4,

       TTYS_ACM2_1_0,
    TTYS_ACM2_1_1,
    TTYS_ACM2_1_2,
    TTYS_ACM2_1_3,
    TTYS_ACM2_1_4,
       TTYS_ACM2_2_0,
    TTYS_ACM2_2_1,
    TTYS_ACM2_2_2,
    TTYS_ACM2_2_3,
    TTYS_ACM2_2_4,

       TTYS_ACM3_1_0,
    TTYS_ACM3_1_1,
    TTYS_ACM3_1_2,
    TTYS_ACM3_1_3,
    TTYS_ACM3_1_4,
       TTYS_ACM3_2_0,
    TTYS_ACM3_2_1,
    TTYS_ACM3_2_2,
    TTYS_ACM3_2_3,
    TTYS_ACM3_2_4,

       TTYS_ACM4_1_0,
    TTYS_ACM4_1_1,
    TTYS_ACM4_1_2,
    TTYS_ACM4_1_3,
    TTYS_ACM4_1_4,
       TTYS_ACM4_2_0,
    TTYS_ACM4_2_1,
    TTYS_ACM4_2_2,
    TTYS_ACM4_2_3,
    TTYS_ACM4_2_4,

       TTYS_ACM5_1_0,
    TTYS_ACM5_1_1,
    TTYS_ACM5_1_2,
    TTYS_ACM5_1_3,
    TTYS_ACM5_1_4,
       TTYS_ACM5_2_0,
    TTYS_ACM5_2_1,
    TTYS_ACM5_2_2,
    TTYS_ACM5_2_3,
    TTYS_ACM5_2_4,

    TTYNULL=255,
};

static char* uart_device [] = 
{
  UART_DEV_NAME_TTYS0,
  UART_DEV_NAME_TTYS1,
  UART_DEV_NAME_TTYS2,
  UART_DEV_NAME_TTYS3,
  UART_DEV_NAME_TTYS4,
  UART_DEV_NAME_TTYS5,
  UART_DEV_NAME_TTYS6,
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

  UART_DEV_NAME_TTYACM1_1_0,
  UART_DEV_NAME_TTYACM1_1_1,
  UART_DEV_NAME_TTYACM1_1_2,
  UART_DEV_NAME_TTYACM1_1_3,
  UART_DEV_NAME_TTYACM1_1_4,
  UART_DEV_NAME_TTYACM1_2_0,
  UART_DEV_NAME_TTYACM1_2_1,
  UART_DEV_NAME_TTYACM1_2_2,
  UART_DEV_NAME_TTYACM1_2_3,
  UART_DEV_NAME_TTYACM1_2_4,

  UART_DEV_NAME_TTYACM2_1_0,
  UART_DEV_NAME_TTYACM2_1_1,
  UART_DEV_NAME_TTYACM2_1_2,
  UART_DEV_NAME_TTYACM2_1_3,
  UART_DEV_NAME_TTYACM2_1_4,
  UART_DEV_NAME_TTYACM2_2_0,
  UART_DEV_NAME_TTYACM2_2_1,
  UART_DEV_NAME_TTYACM2_2_2,
  UART_DEV_NAME_TTYACM2_2_3,
  UART_DEV_NAME_TTYACM2_2_4,

  UART_DEV_NAME_TTYACM3_1_0,
  UART_DEV_NAME_TTYACM3_1_1,
  UART_DEV_NAME_TTYACM3_1_2,
  UART_DEV_NAME_TTYACM3_1_3,
  UART_DEV_NAME_TTYACM3_1_4,
  UART_DEV_NAME_TTYACM3_2_0,
  UART_DEV_NAME_TTYACM3_2_1,
  UART_DEV_NAME_TTYACM3_2_2,
  UART_DEV_NAME_TTYACM3_2_3,
  UART_DEV_NAME_TTYACM3_2_4,

  UART_DEV_NAME_TTYACM4_1_0,
  UART_DEV_NAME_TTYACM4_1_1,
  UART_DEV_NAME_TTYACM4_1_2,
  UART_DEV_NAME_TTYACM4_1_3,
  UART_DEV_NAME_TTYACM4_1_4,
  UART_DEV_NAME_TTYACM4_2_0,
  UART_DEV_NAME_TTYACM4_2_1,
  UART_DEV_NAME_TTYACM4_2_2,
  UART_DEV_NAME_TTYACM4_2_3,
  UART_DEV_NAME_TTYACM4_2_4,

  UART_DEV_NAME_TTYACM5_1_0,
  UART_DEV_NAME_TTYACM5_1_1,
  UART_DEV_NAME_TTYACM5_1_2,
  UART_DEV_NAME_TTYACM5_1_3,
  UART_DEV_NAME_TTYACM5_1_4,
  UART_DEV_NAME_TTYACM5_2_0,
  UART_DEV_NAME_TTYACM5_2_1,
  UART_DEV_NAME_TTYACM5_2_2,
  UART_DEV_NAME_TTYACM5_2_3,
  UART_DEV_NAME_TTYACM5_2_4,

};

 #define UART_DEV_NUM      84
static char *aUartDevice[UART_DEV_NUM] = {
                     UART0_DEVICE_ID_STR,
                     UART1_DEVICE_ID_STR,
                     UART2_DEVICE_ID_STR,
                     UART3_DEVICE_ID_STR,
                     UART4_DEVICE_ID_STR,
                     UART5_DEVICE_ID_STR,
                     UART6_DEVICE_ID_STR,
                     UART17_DEVICE_ID_STR,
                     UART18_DEVICE_ID_STR,
                     UART19_DEVICE_ID_STR,
                     UART20_DEVICE_ID_STR,
                     UART21_DEVICE_ID_STR,
                     UART22_DEVICE_ID_STR,
                     UART23_DEVICE_ID_STR,
                     UART24_DEVICE_ID_STR,
                     UART25_DEVICE_ID_STR,
                     UART26_DEVICE_ID_STR,
                     UART27_DEVICE_ID_STR,
                     UART28_DEVICE_ID_STR,
                     UART29_DEVICE_ID_STR,
                     UART30_DEVICE_ID_STR,
                     UART31_DEVICE_ID_STR,
                     UART32_DEVICE_ID_STR,
                     UART33_DEVICE_ID_STR,
                     UART34_DEVICE_ID_STR,
                     UART35_DEVICE_ID_STR,
                     UART36_DEVICE_ID_STR,
                     UART37_DEVICE_ID_STR,
                     UART38_DEVICE_ID_STR,
                     UART39_DEVICE_ID_STR,
                     UART40_DEVICE_ID_STR,
                     UART41_DEVICE_ID_STR,
                     UART42_DEVICE_ID_STR,
                     UART43_DEVICE_ID_STR,
                     
                     UART44_DEVICE_ID_STR,
                     UART45_DEVICE_ID_STR,
                     UART46_DEVICE_ID_STR,
                     UART47_DEVICE_ID_STR,
                     UART48_DEVICE_ID_STR,
                     UART49_DEVICE_ID_STR,
                     
                     UART50_DEVICE_ID_STR,
                     UART51_DEVICE_ID_STR,
                     UART52_DEVICE_ID_STR,
                     UART53_DEVICE_ID_STR,
                     UART54_DEVICE_ID_STR,
                     UART55_DEVICE_ID_STR,
                     UART56_DEVICE_ID_STR,
                     UART57_DEVICE_ID_STR,
                     UART58_DEVICE_ID_STR,
                     UART59_DEVICE_ID_STR,
                     
                     UART60_DEVICE_ID_STR,
                     UART61_DEVICE_ID_STR,
                     UART62_DEVICE_ID_STR,
                     UART63_DEVICE_ID_STR,
                     UART64_DEVICE_ID_STR,
                     UART65_DEVICE_ID_STR,
                     UART66_DEVICE_ID_STR,
                     UART67_DEVICE_ID_STR,
                     UART68_DEVICE_ID_STR,
                     UART69_DEVICE_ID_STR,

                     UART70_DEVICE_ID_STR,
                     UART71_DEVICE_ID_STR,
                     UART72_DEVICE_ID_STR,
                     UART73_DEVICE_ID_STR,
                     UART74_DEVICE_ID_STR,
                     UART75_DEVICE_ID_STR,
                     UART76_DEVICE_ID_STR,
                     UART77_DEVICE_ID_STR,
                     UART78_DEVICE_ID_STR,
                     UART79_DEVICE_ID_STR,

                     UART80_DEVICE_ID_STR,
                     UART81_DEVICE_ID_STR,
                     UART82_DEVICE_ID_STR,
                     UART83_DEVICE_ID_STR,
                     UART84_DEVICE_ID_STR,
                     UART85_DEVICE_ID_STR,
                     UART86_DEVICE_ID_STR,
                     UART87_DEVICE_ID_STR,
                     UART88_DEVICE_ID_STR,
                     UART89_DEVICE_ID_STR,

                     UART90_DEVICE_ID_STR,
                     UART91_DEVICE_ID_STR,
                     UART92_DEVICE_ID_STR,
                     UART93_DEVICE_ID_STR,
};
                     
                     
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
        //DEBUG(" pointer is null!\n");
        return ERR_PTR;
    }

    if(strcmp(id, UART0_DEVICE_ID_STR) == 0)
    {
        return TTYS0;
    }
    else if(strcmp(id, UART1_DEVICE_ID_STR) == 0)
    {
        return TTYS1;
    }
    else if(strcmp(id, UART2_DEVICE_ID_STR) == 0)
    {
        return TTYS2;
    }
    else if(strcmp(id, UART3_DEVICE_ID_STR) == 0)
    {
        return TTYS3;
    }
    else if(strcmp(id, UART4_DEVICE_ID_STR) == 0)
    {
        return TTYS4;
    }
    else if(strcmp(id, UART5_DEVICE_ID_STR) == 0)
    {
        return TTYS5;
    }
    else if(strcmp(id, UART6_DEVICE_ID_STR) == 0)
    {
        return TTYS6;
    }
    else if(strcmp(id, UART17_DEVICE_ID_STR) == 0)
    {
        return TTYS3; //BT
    }
    else if(strcmp(id, UART43_DEVICE_ID_STR) == 0)
    {
        return TTYS4;
    }
    else if(NULL != strstr(id, UART_DEVICE_ID_START) && strlen(id) == strlen(UART_DEVICE_ID_START) + 3)
    {
        uint8 port = 0;
        uint8 ifnum = 0;
        port = id[strlen(UART_DEVICE_ID_START)] - '0';
        ifnum = id[strlen(UART_DEVICE_ID_START) + 2] - '0';

        if(1 <= port && port <= 5 && 0 <= ifnum && ifnum <= 4)
        {
            return TTYS_ACM1_0 + (port - 1) * 5 + ifnum;
        }
    }
    else if(NULL != strstr(id, UART_DEVICE_ID_START) && strlen(id) == strlen(UART_DEVICE_ID_START) + 5)
    {
        uint8 port = 0;
        uint8 subport = 0;
        uint8 ifnum = 0;
        port = id[strlen(UART_DEVICE_ID_START)] - '0';
        subport = id[strlen(UART_DEVICE_ID_START) + 2] - '0';
        ifnum = id[strlen(UART_DEVICE_ID_START) + 4] - '0';

        if(1 <= port && port <= 5 && 1 <= subport && subport <= 2 && 0 <= ifnum && ifnum <= 4)
        {
            return TTYS_ACM1_1_0 + (port - 1) * 10 + (subport - 1) * 5 + ifnum;
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
    int32 uport = 0;
    signed char paritys;

    if(dev == NULL)
    {
        ////DEBUG("%s:%d pointer is null!\n", __FUNCTION__, __LINE__);
        return ERR_PTR;
    }
    ////DEBUG("********deviceID %s********\n", dev->base.szDeviceID);

    uport = uart_id_to_num(dev->base.szDeviceID);

    if ((uport >0) && (uport != TTYNULL))
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
       
        iRet = OpenSerial(dev, uart_device[uport], speed, databits, stopbits, paritys, 0);  
    }
    else
    {
        //DEBUG("unsupport uart(%d)\n",  uport);
          iRet = ERR_NORMAL;
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
static int uart_recv_data(struct tag_UART_DEVICE *dev,
                          uint8 *buf, 
                          uint32 len)
{
    int32 recv_len = 0;                                       

    if (dev == NULL)
    {
        return ERR_PTR;
    }

    if ( dev->fd <= 0)  
    {  
          DEBUG("Can't Open Serial Port fd %d\n", dev->fd);  
          return ERR_NORMAL;  
    }

    recv_len = read(dev->fd, buf, len);   

    return recv_len;
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
    
    send_len = write(dev->fd, buf, len);  

    if(send_len == len)
        return ERR_OK;
      else
          return ERR_NORMAL;
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
    dev->fd              = 0;
    
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
        .szBuildDate = "2019-10-18",
        .nDevNum  = UART_DEV_NUM,
        .szDevices = aUartDevice,
        /* �Զ���ĺ���ָ�룬���Ｔ�ǻ�ȡstruct tag_HW_DEVICE����� */
        .pfOpen = uart_open,
        .pfClose = uart_close,
    },
};
