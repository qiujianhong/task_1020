#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>

#include "types.h"
#include <can.h>
#include "sysDefine.h"

/*watchdog 设备路径 */
#define CAN0_DEV_NAME              "/dev/ttyCAN0"
#define CAN1_DEV_NAME              "/dev/ttyCAN1"

#define CAN_DEV_NUM          1
static char *aCanDevice[CAN_DEV_NUM] = {CAN0_DEVICE_ID_STR};

struct speed_map {
    speed_t   speed;
    uint32    value;
};

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

speed_t tty_value_to_baud(uint32 value)
{
    int i = 0;
    struct speed_map speeds[] = 
    {
        {B0, 0},
        {B50, 10000},    //10K
        {B75, 25000},    //25K
        {B110,50000},    //50K
        {B134,125000},   //125K
        {B150,250000},   //250K
        {B200,500000},   //500K
        {B1800, 1000000}, //1M
    };

    int NUM_SPEEDS = sizeof(speeds)/sizeof(struct speed_map);

    do {
        if (value == speeds[i].value) 
        {
            return speeds[i].speed;
        }
    } while (++i < NUM_SPEEDS);

    return B0;
}

/**
* @brief 设置CAN通信参数
* @param[in] dev: 设备描述
* @param[in] baud 波特率
* @return成功返回ERR_OK；失败返回错误码。详见：附录。
*/
int32 can_param_set(struct tag_CAN_DEVICE *dev, uint32 baud)
{
    speed_t speed;
    struct termios Opt;    /*Opt中存储波特率数据结构*/
    int    ret = 0;
    
    if(0 != tcgetattr(dev->fd, &Opt))     /*获取termios结构*/
    {
        return -1;
    }

    /*设置为不加工模式*/
    Opt.c_oflag = 0;
    Opt.c_lflag = 0;
    Opt.c_iflag = IGNBRK;
    Opt.c_cflag |= CLOCAL | CREAD;
    Opt.c_cflag &= ~CSIZE;      /*清除原字符大*/
    Opt.c_cflag |= CS8;         /*设置8位数据位*/
    Opt.c_cflag &= ~PARENB;     /* 输入奇偶校验无效 */
    Opt.c_cflag &= ~CSTOPB;     /*1位停止位*/
    Opt.c_cc[VTIME] = 100;      /* 设置超时10 seconds*/   
    Opt.c_cc[VMIN] = 0;         /* 更新设置，使马上生效 */
    Opt.c_iflag &= ~(IXON|IXOFF|IXANY); /*关软件流控*/
    Opt.c_cflag &= ~CRTSCTS;            /*关硬件流控*/
    Opt.c_iflag &= ~IGNCR;              /*不忽略收到的CR字符*/
    Opt.c_iflag &= ~ICRNL;  
    
    speed = tty_value_to_baud(baud);
    if(B0 == speed)
    {
        printf("No know baud %d\n", baud);
        return -1;
    }

    cfsetispeed(&Opt, B115200); /*设置输入波特率*/
    cfsetospeed(&Opt, speed);   /*设置输出波特率*/
    ret = tcsetattr(dev->fd, TCSANOW, &Opt);  /*设置串口新的termios结构*/
    if(0 != ret)
    {
        printf("Set baud %d failed\n", baud);
        return -1;
    }
    tcflush(dev->fd, TCIOFLUSH); /*清除串口的输入输出队列中的数*/
    
    return 0;
}


/**
* @brief 设置CANid参数
* @param[in] dev: 设备描述
* @param[in] can_id 发送帧id
* @return成功返回ERR_OK；失败返回错误码。详见：附录。
*/
//int32 can_id_set(struct tag_CAN_DEVICE *dev, canid_t can_id)
//{
//    uint8  sendmsg[16] = {0};
//    int    msglen = 0;
//    int    i = 0;
//    uint32 datalen = 4;
//    int    sendlen = 0;
//    uint8  FS = 0;
//
//    if(NULL == dev)
//    {
//        return ERR_PTR;
//    }
//
//    /* 私有协议 */
//    sendmsg[msglen++] = 0xEF;
//    sendmsg[msglen++] = 0x68;
//    memcpy(sendmsg + msglen, &datalen, sizeof(uint32));
//    msglen += 4;
//    sendmsg[msglen++] = 2;
//    memcpy(sendmsg + msglen, &can_id, sizeof(uint32));
//    msglen += 4;
//
//    for(i = 2; i < msglen; i++)
//    {
//        FS += sendmsg[i];
//    }
//    sendmsg[msglen++] = FS;
//    sendmsg[msglen++] = 0x86;
//    sendmsg[msglen++] = 0xFE;
//
//    sendlen = write(dev->fd, sendmsg, msglen);
//    if(sendlen == msglen)
//    {
//        return ERR_OK;
//    }
//    else
//    {
//        return ERR_NORMAL;
//    }
//}

/**
* @brief 从CAN接收数据
* @param[in] dev: 设备描述
* @param[out] buf 接收缓存区
* @param[in] len 缓存区长度
* @return 成功返回大于0的数据长度，失败返回错误码。详见：附录。
*/
int32 can_data_recv(struct tag_CAN_DEVICE *dev, uint8* buf, uint32 len)
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

/**
* @brief 通过CAN发送数据
* @param[in] dev: 设备描述
* @param[in] buf 发送缓存区
* @param[in] len 缓存区长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录。
*/
int32 can_data_send(struct tag_CAN_DEVICE *dev, const uint8* buf, int32 len)
{
    int    sendlen = 0;

    if(NULL == dev || dev->fd <= 0 || NULL == buf || 0 == len)
    {
        return ERR_PTR;
    }

    sendlen = write(dev->fd, buf, len);
    if(sendlen == len)
    {
        return ERR_OK;
    }
    else
    {
        return ERR_NORMAL;
    }
}

/**
* @brief 获取CAN句柄
* @param[in] dev: 设备描述
* @return返回CAN打开时的句柄值（正常>0，异常<=0）。
*/
int32 can_get_handle(struct tag_CAN_DEVICE *dev)
{
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    return dev->fd;
}


static int can_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    int fd = -1;
    fd = open(CAN0_DEV_NAME, O_RDWR|O_NOCTTY|O_NONBLOCK);
    if(fd > 0)
    {
        if (setlock(fd, F_WRLCK) == -1)
        {
            DEBUG("setlock %s false!\n", CAN0_DEV_NAME);
            close(fd);
            return ERR_NORMAL;
        }
    }
    else
    {
        return ERR_NOTEXIST;
    }

    CAN_DEVICE_T* dev = (CAN_DEVICE_T*)malloc(sizeof(CAN_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(CAN_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->can_param_set = can_param_set;
    //dev->can_id_set = can_id_set;
    dev->can_data_recv = can_data_recv;
    dev->can_data_send = can_data_send;
    dev->can_get_handle = can_get_handle;
    dev->fd = fd;
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int can_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    CAN_DEVICE_T *dev = (CAN_DEVICE_T*)pDevice;
    if(dev == NULL)
    {
        return ERR_PTR;
    }

    if (dev->fd > 0)
    {
        close(dev->fd);
        dev->fd = 0;
    }
    
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
CAN_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "can",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-06-14",
        .nDevNum  = CAN_DEV_NUM,
        .szDevices = aCanDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = can_open,
        .pfClose = can_close,
    },
};


