#include "types.h"
#include <can.h>
#include "sysDefine.h"

#define can_cmd  "ip link set can%d type can bitrate "
#define can_up   "ifconfig can%d up"
#define can_down "ifconfig can%d down"

#define DEV_PATH_CAN_STATE   "/dev/cantxflag"
#define GET_CAN_STATE_CMD         IOCTL_MACRO('C', 1, READ_DATA_CMD, 4)             // 读取状态

#define CAN_DEV_NUM          1
static char *aCanDevice[CAN_DEV_NUM] = {CAN0_DEVICE_ID_STR};

/* 模块中的设备列表 */
static const char * szDevices_table[];

#define HAL_CAN_MTU                    8
#define HAL_CAN_RBUF_SIZE            1024

static int hal_can_itf_set(struct tag_CAN_DEVICE* dev, bool isUp)
{
    char cmd[128];
    char itfname[32];

    memset(itfname, 0, sizeof(itfname));
    sprintf(itfname, "%s", dev->devName);

    if(isUp)
    {
        snprintf(cmd, sizeof(cmd), "ifconfig %s down", itfname);
        DEBUG("%s\n", cmd);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "ip link set %s type can bitrate %d", itfname, dev->baud);
        DEBUG("%s\n", cmd);
        system(cmd);
        snprintf(cmd, sizeof(cmd), "ifconfig %s up", itfname);
        DEBUG("%s\n", cmd);
        system(cmd);
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "ifconfig %s down", itfname);
        system(cmd);
    }
    return 0;
}

static int32 can_socket_init(struct tag_CAN_DEVICE* pDev)
{
    struct sockaddr_can addr;
    struct ifreq ifr;
    int flag = 0;

    if(pDev->fd > 0)
    {
        close(pDev->fd);
        pDev->fd = -1;
    }

    pDev->fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    DEBUG("socket fd:%d\r\n", pDev->fd);

    if(pDev->fd > 0)
    {
        strcpy(ifr.ifr_name, pDev->devName);
        ioctl(pDev->fd, SIOCGIFINDEX, &ifr);

        memset(&addr, 0, sizeof(addr));
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;
        if (bind(pDev->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            DEBUG("Bind\r\n");
        }

        flag = fcntl(pDev->fd,F_GETFL);
        flag |= O_NONBLOCK;
        fcntl(pDev->fd,F_SETFL,flag);
    }

    return pDev->fd;
}

/**
* @brief 设置CAN通信参数
* @param[in] dev: 设备描述
* @param[in] baud 波特率
* @return成功返回ERR_OK；失败返回错误码。详见：附录。
*/
static int32 can_param_set(struct tag_CAN_DEVICE *dev, uint32 baud)
{
    if(NULL == dev)
    {
        return ERR_INVAL;
    }
    DEBUG("%s-%d\r\n", dev->devName, baud);
    if(dev->fd > 0)
    {
        close(dev->fd);
        dev->fd = -1;
    }
    dev->baud = baud;
    if((baud < 10000)||(baud > 2000000))
    {
        dev->baud = 100000;
    }
    hal_can_itf_set(dev, TRUE);
    if(can_socket_init(dev) > 0)
    {
        return ERR_OK;
    }
    return ERR_NORMAL;
}
/**
* @brief 从CAN接收数据
* @param[in] dev: 设备描述
* @param[out] buf 接收缓存区
* @param[in] len 缓存区长度
* @return 成功返回大于0的数据长度，失败返回错误码。详见：附录。
*/
static int32 can_data_recv(struct tag_CAN_DEVICE *dev, uint8 *buf, uint32 len)
{
    int               data_len = 0;		/*缓冲区中数据的长度*/
    int               recv_len = 0;		/*接收到数据长度*/
    struct can_frame  frame;

    while(1)
    {
        recv_len = read(dev->fd, &frame, sizeof(struct can_frame));
        if(recv_len > 0)
        {
            if(data_len + frame.can_dlc >= len)
            {
                return data_len; /*-接收满-*/
            }
            memcpy(buf + data_len,frame.data,frame.can_dlc);
            data_len += frame.can_dlc;
            if(data_len + 8 >= len)
            {
                return data_len; /*-接收块满-*/
            }
        }
        else
        {
            return data_len; /*收不到数据就超时*/
        }
        usleep(10000);
    }

    return  0;
}

/**
* @brief 通过CAN发送数据
* @param[in] dev: 设备描述
* @param[in] buf 发送缓存区
* @param[in] len 缓存区长度
* @return成功返回ERR_OK；失败返回错误码。详见：附录。
*/
static int32 can_data_send(struct tag_CAN_DEVICE *dev, const uint8*buf, int32 len)
{
    int              sendlen = 0;
    int              leftlen = len;
    int              i = 0;
    struct can_frame frame;
    int              delayms = 0;

    while(leftlen > 0)
    {
        sendlen = (leftlen >= 8) ? 8 : leftlen;
        leftlen -= sendlen;

        frame.can_dlc = sendlen;
        memcpy(frame.data, buf + i * 8, sendlen);
        if(write(dev->fd, &frame, sizeof(frame)) != sizeof(frame))
        {
            return -1;
        }
        i++;
        //扩展帧每帧有131bit，在此基础上，时间额外延长150%以确保发送成功
        delayms= (int)(1000000L*(131)*1.5/ dev->baud);
        usleep(delayms);
    }

    return  0;
}
/**
* @brief 获取CAN句柄
* @param[in] dev: 设备描述
* @return返回CAN打开时的句柄值（正常>0，异常<=0）。
*/
static int32 can_get_handle(struct tag_CAN_DEVICE *dev)
{
    if(NULL == dev)
    {
        return ERR_INVAL;
    }
    return dev->fd;
}

static int can_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    CAN_DEVICE_T* dev = NULL;

    dev = (CAN_DEVICE_T*)malloc(sizeof(CAN_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }

    memset(dev, 0, sizeof(CAN_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->can_param_set = can_param_set;
    dev->can_data_recv = can_data_recv;
    dev->can_data_send = can_data_send;
    dev->can_get_handle = can_get_handle;
    dev->fd = 0;
    dev->can_id = 0; //默认是1

    memcpy(dev->devName, device_id, MAX_CAN_NAME_LEN);
    dev->rbuf = malloc(HAL_CAN_RBUF_SIZE);
    if(dev->rbuf == NULL)
    {
        free(dev);
        return ERR_NORESOURCE;
    }
    dev->baud = 100000; // 默认100K
    hal_can_itf_set(dev, TRUE);
    if(can_socket_init(dev) < 0)
    {
        free(dev->rbuf);
        free(dev);
        return ERR_INVALID;//ERR_NOINIT;
    }
    
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
    if(dev->rbuf != NULL)
    {
        free(dev->rbuf);
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
        .szBuildDate = "2022-07-28",
        .nDevNum  = CAN_DEV_NUM,
        .szDevices = aCanDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = can_open,
        .pfClose = can_close,
    },
};
