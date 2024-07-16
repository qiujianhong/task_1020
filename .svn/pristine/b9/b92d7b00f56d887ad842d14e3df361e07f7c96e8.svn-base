#include "types.h"
#include <drv_closing.h>
#include "sysDefine.h"

/*watchdog 设备路径 */
#define DRV_CLOSING_SCMDEV_NAME    "/dev/ttyYK"

#define DRV_CLOSINGDEV_NUM          1
static char *aDrvClosingDevice[DRV_CLOSINGDEV_NUM] = {HW_DEVICE_ID_DRV_CLOSING};


//传输方向位：bit7=0表示此帧是由终端发出的; bit7=1表示此帧是由模块发出的
#define DRV_MAPMSG_DIR_TERMINAL     0
#define DRV_MAPMSG_DIR_MODULE       1

//启动标志位：bit6=0表示此帧是由模块发起; bit6=1表示此帧是由终端发起
#define DRV_MAPMSG_PRM_TERMINAL     1
#define DRV_MAPMSG_PRM_MODULE       0

//确认
#define DRV_MAPMSG_CTR_OK    0
//否认
#define DRV_MAPMSG_CTR_NO    1
//信息帧
#define DRV_MAPMSG_CTR_MSG   2


#define DRV_MAPMSG_APDU_LinkRequest        1
#define DRV_MAPMSG_APDU_GetRequest         2
#define DRV_MAPMSG_APDU_SetRequest         3
#define DRV_MAPMSG_APDU_ReportNotification 4
#define DRV_MAPMSG_APDU_LinkResponse       129
#define DRV_MAPMSG_APDU_GetResponse        130
#define DRV_MAPMSG_APDU_SetResponse        131
#define DRV_MAPMSG_APDU_ReportResponse     132

#define DRV_MAPMSG_DT_CTR_LOOP_STATUS    0x7000    //控制回路接线状态
#define DRV_MAPMSG_DT_CTR_RELAY_TYPE     0x7100    //继电器输出模式
#define DRV_MAPMSG_DT_CTR_RELAY_WIDTH    0x7101    //继电器输出脉冲宽度
#define DRV_MAPMSG_DT_CTR_STATUS         0x7201    //继电器输出控制


/* 控制域 */
typedef union _DRV_CMT_FIELD_U
{
    struct
    {
        uint8 cmd :6; //控制码
        uint8 prm :1; //启动标识
        uint8 dir :1; //传输方向
        uint8 fid;    //帧序号
    };
    uint16 wcmd;
}__attribute__ ((packed))DRV_CMT_FIELD_U;

#define DRV_MESSAGE_RPM_RECV        0
#define DRV_MESSAGE_RPM_START       1
#define DRV_MESSAGE_ADDR_LEN        64
/* 考虑字节对齐, 没有严格按照报文格式, 收到的消息解析成如下格式 */
typedef struct tagMODELMsgInfo
{
    uint16          len;  //长度
    DRV_CMT_FIELD_U cmt;  //控制域

    uint8           apdu[0]; //apdu内容是大端
}DRVMSG_INFO_T;

static uint16 g_fcstab[256] =
{
    0x0000,0x1189,0x2312,0x329b,0x4624,0x57ad,0x6536,0x74bf,
    0x8c48,0x9dc1,0xaf5a,0xbed3,0xca6c,0xdbe5,0xe97e,0xf8f7,
    0x1081,0x0108,0x3393,0x221a,0x56a5,0x472c,0x75b7,0x643e,
    0x9cc9,0x8d40,0xbfdb,0xae52,0xdaed,0xcb64,0xf9ff,0xe876,
    0x2102,0x308b,0x0210,0x1399,0x6726,0x76af,0x4434,0x55bd,
    0xad4a,0xbcc3,0x8e58,0x9fd1,0xeb6e,0xfae7,0xc87c,0xd9f5,
    0x3183,0x200a,0x1291,0x0318,0x77a7,0x662e,0x54b5,0x453c,
    0xbdcb,0xac42,0x9ed9,0x8f50,0xfbef,0xea66,0xd8fd,0xc974,
    0x4204,0x538d,0x6116,0x709f,0x0420,0x15a9,0x2732,0x36bb,
    0xce4c,0xdfc5,0xed5e,0xfcd7,0x8868,0x99e1,0xab7a,0xbaf3,
    0x5285,0x430c,0x7197,0x601e,0x14a1,0x0528,0x37b3,0x263a,
    0xdecd,0xcf44,0xfddf,0xec56,0x98e9,0x8960,0xbbfb,0xaa72,
    0x6306,0x728f,0x4014,0x519d,0x2522,0x34ab,0x0630,0x17b9,
    0xef4e,0xfec7,0xcc5c,0xddd5,0xa96a,0xb8e3,0x8a78,0x9bf1,
    0x7387,0x620e,0x5095,0x411c,0x35a3,0x242a,0x16b1,0x0738,
    0xffcf,0xee46,0xdcdd,0xcd54,0xb9eb,0xa862,0x9af9,0x8b70,
    0x8408,0x9581,0xa71a,0xb693,0xc22c,0xd3a5,0xe13e,0xf0b7,
    0x0840,0x19c9,0x2b52,0x3adb,0x4e64,0x5fed,0x6d76,0x7cff,
    0x9489,0x8500,0xb79b,0xa612,0xd2ad,0xc324,0xf1bf,0xe036,
    0x18c1,0x0948,0x3bd3,0x2a5a,0x5ee5,0x4f6c,0x7df7,0x6c7e,
    0xa50a,0xb483,0x8618,0x9791,0xe32e,0xf2a7,0xc03c,0xd1b5,
    0x2942,0x38cb,0x0a50,0x1bd9,0x6f66,0x7eef,0x4c74,0x5dfd,
    0xb58b,0xa402,0x9699,0x8710,0xf3af,0xe226,0xd0bd,0xc134,
    0x39c3,0x284a,0x1ad1,0x0b58,0x7fe7,0x6e6e,0x5cf5,0x4d7c,
    0xc60c,0xd785,0xe51e,0xf497,0x8028,0x91a1,0xa33a,0xb2b3,
    0x4a44,0x5bcd,0x6956,0x78df,0x0c60,0x1de9,0x2f72,0x3efb,
    0xd68d,0xc704,0xf59f,0xe416,0x90a9,0x8120,0xb3bb,0xa232,
    0x5ac5,0x4b4c,0x79d7,0x685e,0x1ce1,0x0d68,0x3ff3,0x2e7a,
    0xe70e,0xf687,0xc41c,0xd595,0xa12a,0xb0a3,0x8238,0x93b1,
    0x6b46,0x7acf,0x4854,0x59dd,0x2d62,0x3ceb,0x0e70,0x1ff9,
    0xf78f,0xe606,0xd49d,0xc514,0xb1ab,0xa022,0x92b9,0x8330,
    0x7bc7,0x6a4e,0x58d5,0x495c,0x3de3,0x2c6a,0x1ef1,0x0f78
};

#define PPPINITFCS16 0xffff     /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8     /* Good final FCS value */

#define FRAMESTART   0x68
#define FRAMEEND     0x16

/*
* Calculate a new fcs given the current fcs and the new data.
*/
uint16 pppfcs16(uint16 fcs, uint8 *cp, uint16 len)
{
    while(len--)
    {
        fcs = (fcs >> 8) ^ g_fcstab[(fcs ^ *cp++) & 0xff];
    }
    return (fcs);
}

/**********************************************************************
* @name      : drv_memcpy_r
* @brief     ：倒序拷贝内存
* @param[in] ：void *dest        目的地址
               void *src         源地址
               int count         拷贝长度
* @param[out]：
* @return    ：目的地址
* @Create    : 
* @Date      ：2020-2-29
* @Update    :
**********************************************************************/
void *drv_memcpy_r(void *dest, void *src, int count)
{
    void *ptr = dest;
    
    if(NULL == dest || NULL == src || count <= 0)  
    {   
        return NULL;    
    }
    
    while(count--)
    {
        *(char *)dest++ = *(char *)(src+count); 
    }

    return ptr;
}

/**********************************************************************
* @name      : drv_get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：uint8 *bytes             便宜量
* @return    ：长度
* @Create    : 
* @Date      ：2020-2-28
* @Update    :
**********************************************************************/
uint16 drv_get_len_offset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }
    
    if(bytes != NULL)
        *bytes = nbytes;
        
    return EleNum;
}

/**********************************************************************
* @name      : drv_set_len_offset
* @brief     ：设置元素长度
* @param[in] ：const uint8 *indata     数据Buffer
* @param[out]：const uint8 *indata      数据Buffer
* @return    ：长度
* @Create    : 
* @Date      ：2020-2-28
* @Update    :
**********************************************************************/
uint8 drv_set_len_offset(uint16 eleLen, uint8 *indata)
{
    if(eleLen < 0x80)
    {
        indata[0] = eleLen;
        return 1;
    }
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
        
    return 0;
}

/**********************************************************************
* @name      : mapmsg_split
* @brief     ：接收消息解析成 MAPMSG_INFO_T
* @param[in] ：uint8 *pBuf              接受到的报文
               uint16 bufLen            报文长度
* @param[out]：MAPMSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int drv_mapmsg_split(uint8 *pBuf, uint16 bufLen, DRVMSG_INFO_T **ppMsgOut)
{
    uint8  *pTemp = pBuf + 1;
    DRVMSG_INFO_T tMsgTemp;
    DRVMSG_INFO_T *ptNewMsg = NULL;
    uint16 trialfcs = 0;
    uint16 fcs = 0;
    
    if(NULL == pBuf || NULL == ppMsgOut || 0 == bufLen)
    {
        return -1;
    }

    memset(&tMsgTemp, 0, sizeof(DRVMSG_INFO_T));

    memcpy(&tMsgTemp.len, pTemp, sizeof(uint16));
    pTemp += sizeof(uint16);

    memcpy(&tMsgTemp.cmt, pTemp, sizeof(uint16));
    pTemp += sizeof(uint16);

    memcpy(&fcs, pTemp + tMsgTemp.len, sizeof(uint16));
    
    // 计算效验
    trialfcs = pppfcs16(PPPINITFCS16, pBuf + 1, 4 + tMsgTemp.len);
    trialfcs ^= PPPINITFCS16;
    if (fcs != trialfcs)
    {   
        return -2;
    }

    if(FRAMESTART != *pBuf || FRAMEEND != *(pTemp + tMsgTemp.len + 2))
    {
        return -3;
    }
    
    /* 拷贝 */
    ptNewMsg = (DRVMSG_INFO_T *)malloc(sizeof(DRVMSG_INFO_T) + tMsgTemp.len);
    if(NULL == ptNewMsg)
    {
        return -4;
    }
    
    memcpy(ptNewMsg, &tMsgTemp, sizeof(DRVMSG_INFO_T));
    if(tMsgTemp.len > 0)
    {
        memcpy(ptNewMsg->apdu, pTemp, tMsgTemp.len);
    }
    *ppMsgOut = ptNewMsg;

    return 0;
}

/**********************************************************************
* @name      : mapmsg_package
* @brief     ：指定格式消息组装成
* @param[in] ：MAPMSG_INFO_T *pMsgInfo  结构消息
               uint16 maxlen            消息最大长度
* @param[out]：uint8 *pMsgBuf           组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2020-2-28
* @Update    :
**********************************************************************/
uint16 drv_mapmsg_package(DRVMSG_INFO_T *pMsgInfo, uint8 *pMsgBuf, uint16 maxlen)
{
    uint16 bufLen = 0;
    uint16 offset = 0;
    uint16 trialfcs = 0;

    if(NULL == pMsgInfo || NULL == pMsgBuf || 0 == maxlen)
    {
        return 0;
    }

    /*       标志 + 长度域 + 控制域 + 数据域        + FCS + 标志*/
    bufLen = 1    + 2      + 2      + pMsgInfo->len + 2   + 1;
    if(maxlen < bufLen)
    {
        return 0;
    }

    memset(pMsgBuf, 0, bufLen);
    pMsgBuf[offset++] = FRAMESTART;

    memcpy(pMsgBuf + offset, &(pMsgInfo->len), sizeof(uint16));
    offset += sizeof(uint16);
    memcpy(pMsgBuf + offset, &(pMsgInfo->cmt.wcmd), sizeof(uint16));
    offset += sizeof(uint16);

    memcpy(pMsgBuf + offset, pMsgInfo->apdu, pMsgInfo->len);
    offset += pMsgInfo->len;

    trialfcs = pppfcs16(PPPINITFCS16, pMsgBuf + 1, 4 + pMsgInfo->len);
    trialfcs ^= PPPINITFCS16;

    memcpy(pMsgBuf + offset, &trialfcs, sizeof(uint16));
    offset += sizeof(uint16);

    pMsgBuf[offset] = FRAMEEND;

    return bufLen;
}

//设置锁
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

//设置波特率
int set_speed(int fd, int speed)
{
    int i; 
    int status; 
    int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300};
    int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300};

    struct termios Opt;
    tcgetattr(fd, &Opt);
    
    for ( i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++) 
    { 
        if (speed == name_arr[i])
        { 
            cfsetispeed(&Opt, speed_arr[i]); 
            cfsetospeed(&Opt, speed_arr[i]); 
            status = tcsetattr(fd, TCSANOW, &Opt); 
            if  (status != 0) 
            {
                return -1;
            }
            
            tcflush(fd,TCIOFLUSH);
            return 1;
        }  
    }
    return -1;
}

//设置参数
int set_Parity(int fd, signed char databits, signed char stopbits, signed char parity, signed char set_crtscts)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0)
    { 
        return(FALSE);  
    }
    
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag = IGNBRK;
    options.c_cflag |= CLOCAL | CREAD;
    
    options.c_cflag &= ~CSIZE; 
    switch (databits)
    {   
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;   
        default:    
            return (FALSE);
    }
    
    switch (parity) 
    {   
        case 'n':
        case 'N': 
            options.c_cflag &= ~PARENB;
            break;
              
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            break;
              
        case 'e':
        case 'E':
            options.c_cflag &= ~PARODD;
            options.c_cflag |= PARENB;
            break;
            
        case 'S':
        case 's':
            options.c_cflag &= ~CSTOPB;
            break;
            
        default:
            return (FALSE);
    } 
     
    switch (stopbits)
    {   
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;  
        case 2:
            options.c_cflag |= CSTOPB;
           break;
        default:  
             return (FALSE); 
    }
        
    tcflush(fd,TCIFLUSH);
    options.c_cc[VTIME] = 100;
    options.c_cc[VMIN] = 0;
    
    options.c_iflag &= ~(IXON|IXOFF|IXANY);
    
    if (set_crtscts == 1)
        options.c_cflag |= CRTSCTS;
    else
        options.c_cflag &= ~CRTSCTS;

    options.c_iflag &= ~IGNCR;
    options.c_iflag &= ~ICRNL;

    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {  
        return (FALSE);  
    }
    return (TRUE);
}


int closing_drv_fd_get(struct tag_CLOSING_DEVICE *dev)
{
    if(NULL == dev)
    {
        return -1;
    }
    if(dev->fd <= 0)
    {  
        dev->fd = open(DRV_CLOSING_SCMDEV_NAME, O_RDWR|O_NOCTTY|O_NONBLOCK );
        if(dev->fd > 0)
        {
            if (setlock(dev->fd, F_WRLCK) == -1)    /*閿佷覆鍙*/
            {
                DEBUG("setlock %s false!\n", DRV_CLOSING_SCMDEV_NAME);
                close(dev->fd);
                dev->fd = 0;
                return -1;
            }
        }
        else
        {
            DEBUG("open %s false!\n", DRV_CLOSING_SCMDEV_NAME);
            return -1;
        }

        if (set_speed(dev->fd, 115200) == -1)
        {
            DEBUG("set_speed %s false!\n", DRV_CLOSING_SCMDEV_NAME);  
            close(dev->fd);
            dev->fd = 0;
            return -1;
        }

        if (set_Parity(dev->fd, 8, 1, 'E', 0) == FALSE)
        {
            DEBUG("set_Parity %s false!\n", DRV_CLOSING_SCMDEV_NAME);
            close(dev->fd);
            dev->fd = 0;
            return -1;
        }
    }

    return dev->fd;
}

/**********************************************************************
* @name      : drv_uart_recv
* @brief     ：串口数据接收接口
* @param[in] ：int fd                 句柄
               int nBufLen           最大接收长度
               int rsptime           数据返回时间(ms)
               int rcvtime           数据字节之间间隔时间(ms)
* @param[out]：uint8* pBuffer         接收数据缓冲区
* @return    ：接收到数据的长度
* @Create    : 王津剑
* @Date      ：2022-6-23
* @Update    :
**********************************************************************/
int drv_uart_recv(int fd, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime)
{
    int recv_len = 0;        /*接收到数据长度*/
    int data_len = 0;        /*串口缓冲区中数据的长度*/
    int count = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;

    recv_len = 0;
    data_len = 0;

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
            if(0 == data_len && count * 5 >= rsptime) 
            {
                return data_len; /*-接收数据超时-*/
            }

            if(0 != data_len && (count * 5 >= rcvtime || data_len >= nBufLen))
            {
                return data_len;
            }
        }
        count++;
    } 
}

/**
* @brief 设置合闸继电器输出模式
* @param[in] dev: 设备描述
* @param[in] mode: 合闸模式
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_mode_set(struct tag_CLOSING_DEVICE *dev, CLOSING_MODE_E mode)
{
    int            fd = closing_drv_fd_get(dev);
    uint8          msgbuf[256] = {0};
    DRVMSG_INFO_T *pMsgInfo = (DRVMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = DRV_MAPMSG_DT_CTR_RELAY_TYPE;
    uint8          index = 0;
    DRVMSG_INFO_T *pMapMsg = NULL;

    uint8   sendbuf[256];
    uint16  sendlen = 0;
    uint8   recvbuf[1024];        //接受缓冲区
    uint16  recvlen = 0;
    int     ret = 0;
    uint8 *temp = NULL;
    uint8  type = 0;
    uint8  dir = 0;
    char  *dirName[3] = {"success", "reject", "notexist"};
    
    if(fd <= 0)
    {
        DEBUG("Get fd error\n");
        return -1;
    }

    //构造报文
    index = dev->index++;
    pMsgInfo->cmt.dir = DRV_MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = DRV_MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = DRV_MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = DRV_MAPMSG_APDU_SetRequest;
    drv_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 22;  //enum
    Temp[offset++] = mode + 1;
    pMsgInfo->len = offset;

    sendlen = drv_mapmsg_package(pMsgInfo, sendbuf, 256);
    if(sendlen <= 0)
    {
        DEBUG("Get fd error\n");
        return -1;
    }

    //先清空接收队列
    drv_uart_recv(fd, recvbuf, 1024, 20, 10);

    //发送报文
    ret = write(fd, sendbuf, sendlen);
    if(ret != sendlen)
    {
        DEBUG("Send failed\n");
        return -1;
    }
    DEBUG_MSG(sendbuf, sendlen, "send msg(%d): ", sendlen);

    //接收
    memset(recvbuf, 0, 1024);
    ret = drv_uart_recv(fd, recvbuf, 1024, 200, 20);
    if(ret <= 0)
    {
        DEBUG("Recv zero\n");
        return -1;
    }
    recvlen = ret;
    DEBUG_MSG(recvbuf, recvlen, "recv msg(%d): ", recvlen);

    //解析
    ret = drv_mapmsg_split(recvbuf, recvlen, &pMapMsg);
    if(ret == 0 && NULL != pMapMsg)
    {
        if(DRV_MAPMSG_CTR_MSG != pMapMsg->cmt.cmd || pMapMsg->len < 4)
        {
            free(pMapMsg);
            return -2;
        }

        type = pMapMsg->apdu[0];
        temp = pMapMsg->apdu + 1;
        drv_memcpy_r(&dt, temp, sizeof(uint16));
        temp += sizeof(uint16);
    
        if(DRV_MAPMSG_APDU_SetResponse == type && index == pMapMsg->cmt.fid && dt == DRV_MAPMSG_DT_CTR_RELAY_TYPE)
        {
            dir = *temp;
            DEBUG("set %04X %s\n", dt, dir < 3 ? dirName[dir] : "other err");
            ret = dir;
        }
        else
        {
            DEBUG("not match dt %04X, FID(%d-%d)\n", dt, pMapMsg->cmt.fid, index);
            ret = -2;
        }
        free(pMapMsg);
    }
    else
    {
        ret = -3;
    }
    return ret;
}

/**
* @brief 设置合闸继电器输出脉冲宽度
* @param[in] dev: 设备描述
* @param[in] period: 合闸继电器输出脉冲宽度，单位：ms
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_pulse_width_set(struct tag_CLOSING_DEVICE *dev, uint32 pulse_width)
{
    int            fd = closing_drv_fd_get(dev);
    uint8          msgbuf[256] = {0};
    DRVMSG_INFO_T *pMsgInfo = (DRVMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = DRV_MAPMSG_DT_CTR_RELAY_WIDTH;
    uint8          index = 0;
    DRVMSG_INFO_T *pMapMsg = NULL;

    uint8   sendbuf[256];
    uint16  sendlen = 0;
    uint8   recvbuf[1024];        //接受缓冲区
    uint16  recvlen = 0;
    int     ret = 0;
    uint8 *temp = NULL;
    uint8  type = 0;
    uint8  dir = 0;
    char  *dirName[3] = {"success", "reject", "notexist"};

    uint16 width = pulse_width;
    if(fd <= 0)
    {
        DEBUG("Get fd error\n");
        return -1;
    }

    if(pulse_width > 0xFFFF || pulse_width == 0)
    {
        width = 300;
    }

    //构造报文
    index = dev->index++;
    pMsgInfo->cmt.dir = DRV_MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = DRV_MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = DRV_MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = DRV_MAPMSG_APDU_SetRequest;
    drv_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 18;  //long-unsigned
    drv_memcpy_r(Temp + offset, &width, 2);
    offset += 2;
    pMsgInfo->len = offset;

    sendlen = drv_mapmsg_package(pMsgInfo, sendbuf, 256);
    if(sendlen <= 0)
    {
        DEBUG("Get fd error\n");
        return -1;
    }

    //先清空接收队列
    drv_uart_recv(fd, recvbuf, 1024, 20, 10);

    //发送报文
    ret = write(fd, sendbuf, sendlen);
    if(ret != sendlen)
    {
        DEBUG("Send failed\n");
        return -1;
    }
    DEBUG_MSG(sendbuf, sendlen, "send msg(%d): ", sendlen);

    //接收
    memset(recvbuf, 0, 1024);
    ret = drv_uart_recv(fd, recvbuf, 1024, 200, 20);
    if(ret <= 0)
    {
        DEBUG("Recv zero\n");
        return -1;
    }
    recvlen = ret;
    DEBUG_MSG(recvbuf, recvlen, "recv msg(%d): ", recvlen);

    //解析
    ret = drv_mapmsg_split(recvbuf, recvlen, &pMapMsg);
    if(ret == 0 && NULL != pMapMsg)
    {
        if(DRV_MAPMSG_CTR_MSG != pMapMsg->cmt.cmd || pMapMsg->len < 4)
        {
            free(pMapMsg);
            return -2;
        }

        type = pMapMsg->apdu[0];
        temp = pMapMsg->apdu + 1;
        drv_memcpy_r(&dt, temp, sizeof(uint16));
        temp += sizeof(uint16);
    
        if(DRV_MAPMSG_APDU_SetResponse == type && index == pMapMsg->cmt.fid && dt == DRV_MAPMSG_DT_CTR_RELAY_WIDTH)
        {
            dir = *temp;
            DEBUG("set %04X %s\n", dt, dir < 3 ? dirName[dir] : "other err");
            ret = dir;
        }
        else
        {
            DEBUG("not match dt %04X, FID(%d-%d)\n", dt, pMapMsg->cmt.fid, index);
            ret = -2;
        }
        free(pMapMsg);
    }
    else
    {
        ret = -3;
    }
    return ret;
}

/**
* @brief 获取跳闸接线状态
* @param[in] dev: 设备描述
* @param[out] state: 接线状态，bit0…bit(n-1)按位表示1~n 轮接线状态，1有接线，0 没接线
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_line_state_get(struct tag_CLOSING_DEVICE *dev, uint32 *state)
{
    int            fd = closing_drv_fd_get(dev);
    uint8          msgbuf[256] = {0};
    DRVMSG_INFO_T *pMsgInfo = (DRVMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = DRV_MAPMSG_DT_CTR_LOOP_STATUS;
    uint8          index = 0;
    DRVMSG_INFO_T *pMapMsg = NULL;

    uint8   sendbuf[256];
    uint16  sendlen = 0;
    uint8   recvbuf[1024];        //接受缓冲区
    uint16  recvlen = 0;
    int     ret = 0;
    uint8 *temp = NULL;
    uint8  type = 0;
    uint8  dir = 0;
    uint8  choice = 0;
    char  *dirName[3] = {"success", "reject", "notexist"};

    if(fd <= 0)
    {
        DEBUG("Get fd error\n");
        return -1;
    }

    //构造报文
    index = dev->index++;
    pMsgInfo->cmt.dir = DRV_MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = DRV_MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = DRV_MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = DRV_MAPMSG_APDU_GetRequest;
    drv_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    pMsgInfo->len = offset;
    
    sendlen = drv_mapmsg_package(pMsgInfo, sendbuf, 256);
    if(sendlen <= 0)
    {
        DEBUG("Get fd error\n");
        return -1;
    }

    //先清空接收队列
    drv_uart_recv(fd, recvbuf, 1024, 20, 10);

    //发送报文
    ret = write(fd, sendbuf, sendlen);
    if(ret != sendlen)
    {
        DEBUG("Send failed\n");
        return -1;
    }
    DEBUG_MSG(sendbuf, sendlen, "send msg(%d): ", sendlen);

    //接收
    memset(recvbuf, 0, 1024);
    ret = drv_uart_recv(fd, recvbuf, 1024, 200, 20);
    if(ret <= 0)
    {
        DEBUG("Recv zero\n");
        return -1;
    }
    recvlen = ret;
    DEBUG_MSG(recvbuf, recvlen, "recv msg(%d): ", recvlen);

    //解析
    ret = drv_mapmsg_split(recvbuf, recvlen, &pMapMsg);
    if(ret == 0 && NULL != pMapMsg)
    {
        if(DRV_MAPMSG_CTR_MSG != pMapMsg->cmt.cmd || pMapMsg->len < 4)
        {
            free(pMapMsg);
            return -2;
        }

        type = pMapMsg->apdu[0];
        temp = pMapMsg->apdu + 1;
        drv_memcpy_r(&dt, temp, sizeof(uint16));
        temp += sizeof(uint16);
    
        if(DRV_MAPMSG_APDU_GetResponse == type && index == pMapMsg->cmt.fid && dt == DRV_MAPMSG_DT_CTR_LOOP_STATUS)
        {
            choice = *temp;
            temp++;
            if(1 == choice)
            {
                uint8 num;
                uint8 i = 0;
                uint32 curstate = 0;
                uint8  bitstatus = 0;
                if(1 != *temp)
                {
                    ret = 4;
                }
                else
                {
                    temp++;
                    num = *temp;
                    temp++;
                    for(i = 0; i < num; i++)
                    {
                        temp++;
                        bitstatus = *temp;
                        temp++;
                        if(0 == bitstatus)
                        {
                            /* 模组协议和hal接口是反的 */
                            curstate |= (0x01 << i);
                        }
                    }
                    *state = curstate;
                }
                ret = 0;
            }
            else
            {
                dir = *temp;
                DEBUG("Get %04X %s\n", dt, dir < 3 ? dirName[dir] : "other err");
                ret = dir;
            }
        }
        else
        {
            DEBUG("not match dt %04X, FID(%d-%d)\n", dt, pMapMsg->cmt.fid, index);
            ret = -2;
        }
        free(pMapMsg);
    }
    else
    {
        ret = -3;
    }
    return ret;

}

/**
* @brief合闸继电器输出控制
* @param[in] dev: 设备描述
* @param[in] id: 1~4为4轮合闸
* @param[in] turn: 动作控制
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_turn(struct tag_CLOSING_DEVICE *dev, uint8 id, CLOSING_TURN_E turn)
{
    int            fd = closing_drv_fd_get(dev);
    uint8          msgbuf[256] = {0};
    DRVMSG_INFO_T *pMsgInfo = (DRVMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = DRV_MAPMSG_DT_CTR_STATUS;
    uint8          index = 0;
    DRVMSG_INFO_T *pMapMsg = NULL;

    uint8   sendbuf[256];
    uint16  sendlen = 0;
    uint8   recvbuf[1024];        //接受缓冲区
    uint16  recvlen = 0;
    int     ret = 0;
    uint8 *temp = NULL;
    uint8  type = 0;
    uint8  dir = 0;
    char  *dirName[3] = {"success", "reject", "notexist"};

    if(fd <= 0)
    {
        DEBUG("Get fd error\n");
        return -1;
    }
    if(0 == id || id > 5)
    {
        DEBUG("input error\n");
        return -1;
    }

    //构造报文
    index = dev->index++;
    pMsgInfo->cmt.dir = DRV_MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = DRV_MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = DRV_MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = DRV_MAPMSG_APDU_SetRequest;
    drv_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 1;  //array
    Temp[offset++] = 1;
    Temp[offset++] = 2;  //structure
    Temp[offset++] = 2;
    Temp[offset++] = 17;  //unsigned
    Temp[offset++] = id;
    Temp[offset++] = 22;  //enum
    Temp[offset++] = turn;
    pMsgInfo->len = offset;

    sendlen = drv_mapmsg_package(pMsgInfo, sendbuf, 256);
    if(sendlen <= 0)
    {
        DEBUG("Get fd error\n");
        return -1;
    }

    //先清空接收队列
    drv_uart_recv(fd, recvbuf, 1024, 20, 10);

    //发送报文
    ret = write(fd, sendbuf, sendlen);
    if(ret != sendlen)
    {
        DEBUG("Send failed\n");
        return -1;
    }
    DEBUG_MSG(sendbuf, sendlen, "send msg(%d): ", sendlen);

    //接收
    memset(recvbuf, 0, 1024);
    ret = drv_uart_recv(fd, recvbuf, 1024, 200, 20);
    if(ret <= 0)
    {
        DEBUG("Recv zero\n");
        return -1;
    }
    recvlen = ret;
    DEBUG_MSG(recvbuf, recvlen, "recv msg(%d): ", recvlen);

    //解析
    ret = drv_mapmsg_split(recvbuf, recvlen, &pMapMsg);
    if(ret == 0 && NULL != pMapMsg)
    {
        if(DRV_MAPMSG_CTR_MSG != pMapMsg->cmt.cmd || pMapMsg->len < 4)
        {
            free(pMapMsg);
            return -2;
        }

        type = pMapMsg->apdu[0];
        temp = pMapMsg->apdu + 1;
        drv_memcpy_r(&dt, temp, sizeof(uint16));
        temp += sizeof(uint16);
    
        if(DRV_MAPMSG_APDU_SetResponse == type && index == pMapMsg->cmt.fid && dt == DRV_MAPMSG_DT_CTR_STATUS)
        {
            dir = *temp;
            DEBUG("set %04X %s\n", dt, dir < 3 ? dirName[dir] : "other err");
            ret = dir;
        }
        else
        {
            DEBUG("not match dt %04X, FID(%d-%d)\n", dt, pMapMsg->cmt.fid, index);
            ret = -2;
        }
        free(pMapMsg);
    }
    else
    {
        ret = -3;
    }
    return ret;
}

/**
* @brief告警继电器输出控制
* @param[in] dev: 设备描述
* @param[in] turn: 动作控制
* @return成功返回ERR_OK；失败返回错误码。
*/
int32 closing_turn_alarm(struct tag_CLOSING_DEVICE *dev, CLOSING_TURN_E turn)
{
    return closing_turn(dev, 5, turn);
}

static int drv_closing_open(HW_MODULE *module, const char* device_id, HW_DEVICE **device)
{
    /* 此处通过判断传入的device id，匹配到指定的硬件设备，用于同一个
    module下多个硬件设备的情况（如多个uart），在这种情况下，设备私有
    的tag_HW_RTC子类也需要定义多个 */
    
    DRV_CLOSING_DEVICE_T* dev = (DRV_CLOSING_DEVICE_T*)malloc(sizeof(DRV_CLOSING_DEVICE_T));
    if(dev == NULL)
    {
        return ERR_NORMAL;
    }
    memset(dev, 0, sizeof(DRV_CLOSING_DEVICE_T));
    dev->base.nVer = 0;
    dev->base.szDeviceID = device_id;
    dev->base.pModule = (HW_MODULE *)module;
    
    dev->closing_mode_set = closing_mode_set;
    dev->closing_pulse_width_set = closing_pulse_width_set;
    dev->closing_line_state_get = closing_line_state_get;
    dev->closing_turn = closing_turn;
    dev->closing_turn_alarm = closing_turn_alarm;
    
    dev->fd = -1;
    *device = (HW_DEVICE*)dev;
    
    return ERR_OK;
}

static int drv_closing_close(HW_MODULE *pModule, HW_DEVICE *pDevice)
{
    DRV_CLOSING_DEVICE_T *dev = (DRV_CLOSING_DEVICE_T*)pDevice;

    if(dev->fd > 0)
    {
        close(dev->fd);
        dev->fd = -1;
    }
    free(dev);
    return ERR_OK;
}

/* 初始化模块变量 */
DRV_CLOSING_MODULE HAL_MODULE_INFO_SYM = 
{
    .common = 
    {
        .szName = "drv_closing",
        .nVer = 0,
        .szAuthor = "xinlian Electric",
        .nFactory = 0,
        .szBuildDate = "2022-06-15",
        .nDevNum  = DRV_CLOSINGDEV_NUM,
        .szDevices = aDrvClosingDevice,
        /* 自定义的函数指针，这里即是获取struct tag_HW_DEVICE的入口 */
        .pfOpen = drv_closing_open,
        .pfClose = drv_closing_close,
    },
};
