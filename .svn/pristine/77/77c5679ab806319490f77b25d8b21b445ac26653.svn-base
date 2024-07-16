/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块 继电器和其他设备控制
* @date：    2020-4-3
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "mapmsg_split.h"
#include "hal.h"
#include "uart.h"
#include "audio.h"
#include "drv_closing.h"

#include "mapmsg_split.h"

#include "appctrl_comm.h"
#include "appctrl_dev.h"
#include "framecomm.h"

uint8           g_TurnPort = 0;
uint8           g_TurnMax = OOP_MAX_TURN;
uint8           g_SwitchState[OOP_MAX_TURN];//初始化默认合闸状态
UN_BRAKE_DATA_T g_BrakeBuf[OOP_MAX_TURN];   //
uint8           g_CtrlModelindex;

CTRL_RS232_T g_CtrlDev1;
CTRL_RS232_T g_CtrlDev2;
CTRL_RS232_T g_CtrlAlarm;

DRV_CLOSING_DEVICE_T *g_ecudev = NULL;

#define PREPARE_ON                      0x11
#define PREPARE_OFF                     0x7F

#define SET_MID_BOARD_YK_DAT            0x11
#define SET_MID_BOARD_YD_DAT            0x22
#define SET_LOW_BOARD_YK_DAT            0xb1
#define SET_LOW_BOARD_YD_DAT            0xb2

/**********************************************************************
* @name      : appctrl_IIC_Prepare
* @brief     ：中板遥控预动（合闸继电器）
* @param[in] ：uint8         yd_out (bit6是告警)
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-7-28
* @Update    :
**********************************************************************/
int appctrl_IIC_Prepare(uint8 yd_out)
{
    int fd;
    int count = 10;

    while(((fd = open("/dev/ykyd", O_RDONLY))<0)&&((count--)>0))
    {
        usleep(2000);
    }
    
    if (fd <= 0) {
          CTRL_FMT_DEBUG("无ykyd文件\n");
          return -1;
      }

    //写入预动状态
    if ( ioctl(fd, SET_MID_BOARD_YD_DAT, yd_out) < 0)
    {
        CTRL_FMT_DEBUG("合闸预动失败\n");
        return -1;
    }
    
    //关闭设备
    if (fd > 0) close(fd);
    return 0;
}

/**********************************************************************
* @name      : appctrl_IIC_Contactor
* @brief     ：中板遥控（合闸继电器）
* @param[in] ：uint8         ctrl_out (bit0-bit7分别表示1-8路)
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-7-28
* @Update    :
**********************************************************************/
int appctrl_IIC_Contactor(uint8 ctrl_out)
{
    int fd;
    int count = 10;
    ctrl_out &= 0xff;

    while(((fd = open("/dev/ykyd", O_RDONLY))<0)&&((count--)>0))
    {
        usleep(2000);
    }
    
    if (fd <= 0) 
    {
          CTRL_FMT_DEBUG("无ykyd文件\n");
          return -1;
    }

    //写入控制状态
    if ( ioctl(fd, SET_MID_BOARD_YK_DAT, ctrl_out) < 0)
    {
        CTRL_FMT_DEBUG("合闸设置失败\n");
        return -1;
    }

    //关闭设备
    close(fd);
    return 0;
}

/**********************************************************************
* @name      : appctrl_IIC_PrepareExt
* @brief     ：低板遥控预动（跳闸闸继电器）
* @param[in] ：uint8         yd_out
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-7-28
* @Update    :
**********************************************************************/
int appctrl_IIC_PrepareExt(unsigned char yd_out)
{
    int fd;
    int count = 10;

    while(((fd = open("/dev/ykyd", O_RDONLY))<0)&&((count--)>0))
    {
        usleep(2000);
    }
    if (fd <= 0)
    {
        CTRL_FMT_DEBUG("无ykyd文件\n");
        return -1;
    }

    //写入预动状态
    if ( ioctl(fd, SET_LOW_BOARD_YD_DAT, yd_out) < 0)
    {
        CTRL_FMT_DEBUG("跳闸预动失败\n");
        return -1;
    }
    
    //关闭设备
    close(fd);
    return 0;
}

/**********************************************************************
* @name      : appctrl_IIC_ContactorExt
* @brief     ：低板遥控（跳闸继电器）
* @param[in] ：uint8         ctrl_out (bit0-bit7分别表示1-8路)
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-7-28
* @Update    :
**********************************************************************/
int appctrl_IIC_ContactorExt(uint8 ctrl_out)
{
    int fd;
    int count = 10;
    ctrl_out &= 0xff;

    while(((fd = open("/dev/ykyd", O_RDONLY))<0)&&((count--)>0))
    {
        usleep(2000);
    }
    if (fd <= 0) 
    {
        CTRL_FMT_DEBUG("无ykyd文件\n");
        return -1;
     }

    //写入控制状
    if ( ioctl(fd, SET_LOW_BOARD_YK_DAT, ctrl_out) < 0)
    {
        CTRL_FMT_DEBUG("跳闸设置失败\n");
        return -1;
    }

    //关闭设备
    close(fd);
    return 0;
}

/**********************************************************************
* @name      : appctrl_IIC_getinput
* @brief     ：获取控制回路接入状态
* @param[in] ：uint8 maxport       继电器数量
* @param[out]：uint8 *inputOpen   跳闸继电器接入情况
               uint8 *inputClose  合闸继电器接入情况
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-7-30
* @Update    :
**********************************************************************/
int appctrl_IIC_getinput(uint8 *inputOpen, uint8 *inputClose, uint8 portnum)
{
    int32 fd = 0;
    int32 iRet= 0;
    struct flock lock;
    uint8 date[3] = {16, 0xFF, 0xFF};
    uint8 i = 0;

    fd = open("/tmp/dev/ctrlstatus", O_RDONLY, 0666);
    if(fd < 0)
    {
        goto endget;
    }

    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    iRet = fcntl(fd, F_SETLKW, &lock);
    if(iRet != 0)
    {
        printf("Set lock ret %d\n",iRet);
        goto endget;
    }

    iRet = read(fd, date, 3); 
    if(iRet < 0)
    {
        printf("active_id read error\n");
        goto endget;
    }
    if(0 == date[0])
    {
        date[1] = 0xFF;
        date[2] = 0xFF;
    }

    lock.l_type = F_UNLCK;
    iRet = fcntl(fd, F_SETLK, &lock);
    if (iRet < 0)
    {
        printf("doorstatus unlock failed msg:%s\n", strerror(errno));
    }

    close(fd);
endget:
    for(i = 0; i < portnum && i < OOP_MAX_SWITCHOUT / 2; i++)
    {
        inputOpen[i] = ((date[1] >> i) & 0x01);
        inputClose[i] = ((date[2] >> i) & 0x01);
    }
    return 0;
}


/**********************************************************************
* @name      : ctrl_get_index
* @brief     ：初始化串口接口
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-6-20
* @Update    :
**********************************************************************/
uint8 ctrl_get_index()
{
    return g_CtrlModelindex++;
}

/**********************************************************************
* @name      : ctrl_uart_Init
* @brief     ：初始化串口接口
* @param[in] ：CTRL_RS232_T *pDevPara 串口描述
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
int ctrl_uart_Init(CTRL_RS232_T *pDevPara)
{
    UART_DEVICE_T* dev;
    uint32         speed = 115200;
    dev = (UART_DEVICE_T*)hal_device_get(pDevPara->deviceName);
    if(NULL == dev)
    {
        return -1;
    }
    
    // 打开设备
#ifdef PRODUCT_ZCU_3
    speed = 19200;
#endif
    if(0 != dev->uart_param_set(dev, speed, 8, 1, PARITY_EVEN))
    {
        CTRL_FMT_DEBUG("设置串口%s属性失败\n", pDevPara->deviceName);
        hal_device_release((HW_DEVICE *)dev);
        pDevPara->pDev = NULL;
        return -1;
    }

    pDevPara->pDev = dev;
#ifdef PRODUCT_ZCU_3
    pDevPara->fd = open("/dev/tty_io", O_RDONLY);;
#else
    pDevPara->fd = -1;
#endif

    return 0;
}

/**********************************************************************
* @name      : ctrl_uart_UnInit
* @brief     ：去初始化串口接口
* @param[in] ：CTRL_RS232_T *pDevPara 串口描述
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
void ctrl_uart_UnInit(CTRL_RS232_T *pDevPara)
{
    if(NULL == pDevPara || NULL == pDevPara->pDev)
    {
        return;
    }
    
    hal_device_release((HW_DEVICE *)pDevPara->pDev);
    pDevPara->pDev = NULL;

    if(pDevPara->fd > 0)
    {
        close(pDevPara->fd);
    }
}


/**********************************************************************
* @name      : ctrl_uart_send
* @brief     ：串口数据接收接口
* @param[in] ：CTRL_RS232_T *pDevPara 串口描述
               uint8* pBuffer         发送数据缓冲区
               int nBufLen            发送长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
int ctrl_uart_send(CTRL_RS232_T *pDevPara, uint8* pBuffer, int nBufLen)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T*)pDevPara->pDev;
    if(0 == dev->uart_data_send(dev, pBuffer, nBufLen))
    {
        return 0;
    }
    return -1;
}

/**********************************************************************
* @name      : ctrl_RS232_recv
* @brief     ：串口数据接收接口
* @param[in] ：CTRL_RS232_T *pDevPara 串口描述
               int nBufLen            最大接收长度
               int rsptime            数据返回时间(20ms)
               int rcvtime            数据字节之间间隔时间(20ms)
* @param[out]：uint8* pBuffer         接收数据缓冲区
* @return    ：接收到数据的长度
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
int ctrl_uart_recv(CTRL_RS232_T *pDevPara, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime)
{
    int recv_len = 0;        /*接收到数据长度*/
    int data_len = 0;        /*串口缓冲区中数据的长度*/
    int count = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    int fd = 0;

    UART_DEVICE_T* dev = (UART_DEVICE_T*)pDevPara->pDev;

    recv_len = 0;
    data_len = 0;
    
    fd = dev->uart_get_handle(dev);
    if(fd <= 0)
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

/**********************************************************************
* @name      : ctrl_update_devname
* @brief     ：更新控制模块设备符名称
* @param[in] ：uint8 port             设备端口
* @param[out]：CTRL_RS232_T *pDevPara 串口描述
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
void ctrl_update_devname(uint8 port, uint8 subport, CTRL_RS232_T *pDevPara)
{
    char ttyName[30] = {0};

    if(0 == port)
    {
        if(NULL != pDevPara->pDev)
        {
            hal_device_release((HW_DEVICE *)pDevPara->pDev);
        }
        pDevPara->port = port;
        pDevPara->pDev = NULL;
        memset(pDevPara->deviceName, 0, 30);
    }
    else if(port != pDevPara->port || subport != pDevPara->subport)
    {
        sprintf(ttyName, "%s%d_%d", UART_ECU_DEVICE_ID_STR, port, subport);
        if(0 != strcmp(ttyName, pDevPara->deviceName))
        {
            if(NULL != pDevPara->pDev)
            {
                hal_device_release((HW_DEVICE *)pDevPara->pDev);
            }
            pDevPara->port = port;
            pDevPara->subport = subport;
            pDevPara->pDev = NULL;
            strcpy(pDevPara->deviceName, ttyName);
        }
    }
}

/**********************************************************************
* @name      : apptrl_set_switch
* @brief     ：继电器工作
* @param[in] ：uint8 *DataBuf     数据内容
*              uint8 CtrlCode    控制码
* @param[out]：uint8 *SendBuf     输出缓冲区
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
uint8 appctrl_switch_package(uint8 *DataBuf, uint8 CtrlCode, uint8 *SendBuf)
{
    uint8     index = 0;
    uint16    Crc;

    SendBuf[index++] = 0x68;
    SendBuf[index++] = CtrlCode;
    
    SendBuf[index ++] = DataBuf[0];
    SendBuf[index ++] = DataBuf[1];

    Crc = CRC16((uint8 *)SendBuf, 4);
    SendBuf[index++] = Crc%256;
    SendBuf[index++] = Crc/256;

    SendBuf[index] = 0x16;
    index += 1;
    
    return index;
}

/**********************************************************************
* @name      : appctrl_switch_split
* @brief     ：解帧函数
* @param[in] ：uint8 *pBuf         接收数据
*              uint8 len        数据长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
int appctrl_switch_split(uint8 *pBuf,int len)
{
    char         *ptr = NULL;
    char          CtrlCode;//控制码
    uint16        Crc;

    //有效帧截取    
    ptr = (char*)memchr(pBuf, 0x68, len);
    if(ptr == NULL)
    {
        return -1;
    }
    else 
    {        
        if(ptr[6] != 0x16)
        {
            //格式不对
            return -2;
        }
    }
    
    //CRC判断
    Crc = CRC16((uint8 *)ptr,4);
    if(ptr[5] != Crc/256 || ptr[4] != Crc%256 )
    {
        //校验错误
        return -3;
    }

    //控制码验证
    CtrlCode = ptr[1];
    
    switch(CtrlCode)
    {
        case 0x3A://输出控制命令上行帧
            if(ptr[2] == 0x55)
            {
                return 0;
            }
            break;
        case 0x4A://读取控制状态命令帧
            break;

        case 0x5A://读取控制回路状态命令帧
                return ptr[2];
    }
    
    return -4;
}

/**********************************************************************
* @name      : appctrl_set_zcu_3_status_Flag
* @brief     ：继电器消息收发
* @param[in] ：CTRL_RS232_T *pDevPara  设备句柄
               char *pMsg              发送消息
               uint8 msglen            消息长度
               uint8 logFlag           日志是否打印 0 打印,1 不打印
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
int appctrl_set_zcu_3_status_flag(CTRL_RS232_T *pDevPara, uint8 *pMsg, uint8 msglen, uint8 logFlag)
{
    uint8   RetryCnt = 0;
    uint8   Rev_buf[200];        //接受缓冲区
    int     Rec_Len;   
    int     ret = 0;
    if(NULL == pDevPara || NULL == pMsg || 0 == msglen)
    {
        return -1;//失败
    }

    while (RetryCnt < 3)
    {
        RetryCnt ++;
        
        ret = ctrl_uart_send(pDevPara, pMsg, msglen);
        if(0 == logFlag)
        {
            CTRL_BUF_DEBUG(pMsg, msglen, "Ctrl %s send, ret %d", pDevPara->deviceName, ret);
        }
        if(0 != ret)
        {
            usleep(20000);
            continue;
        }
        
        memset(Rev_buf, 0x0, sizeof(Rev_buf));
        Rec_Len = ctrl_uart_recv(pDevPara, Rev_buf, sizeof(Rev_buf), 50, 20);
        if(Rec_Len <= 0)
        {
            usleep(20000);
            continue;
        }
        
        ret = appctrl_switch_split(Rev_buf, Rec_Len);
        if(0 == logFlag)
        {
            CTRL_BUF_DEBUG(Rev_buf, Rec_Len, "Ctrl %s recv, ret %d", pDevPara->deviceName, ret);
        }
        if (0 == ret) 
        {
            return 0;
        }
        usleep(20000);
    }

    return -1;//失败
}


/**********************************************************************
* @name      : appctrl_set_zcu_3_status
* @brief     ：继电器消息收发
* @param[in] ：CTRL_RS232_T *pDevPara  设备句柄
               char *pMsg              发送消息
               uint8 msglen            消息长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
int appctrl_set_zcu_3_status(CTRL_RS232_T *pDevPara, uint8 *pMsg, uint8 msglen)
{
    uint8   RetryCnt = 0;
    uint8   Rev_buf[200];        //接受缓冲区
    int     Rec_Len;   
    int     ret = 0;
    if(NULL == pDevPara || NULL == pMsg || 0 == msglen)
    {
        return -1;//失败
    }

    while (RetryCnt < 3)
    {
        RetryCnt ++;
        
        ret = ctrl_uart_send(pDevPara, pMsg, msglen);
        CTRL_BUF_DEBUG(pMsg, msglen, "Ctrl %s send, ret %d", pDevPara->deviceName, ret);
        if(0 != ret)
        {
            usleep(20000);
            continue;
        }
        
        memset(Rev_buf, 0x0, sizeof(Rev_buf));
        Rec_Len = ctrl_uart_recv(pDevPara, Rev_buf, sizeof(Rev_buf), 50, 20);
        if(Rec_Len <= 0)
        {
            usleep(20000);
            continue;
        }
        
        ret = appctrl_switch_split(Rev_buf, Rec_Len);
        CTRL_BUF_DEBUG(Rev_buf, Rec_Len, "Ctrl %s recv, ret %d", pDevPara->deviceName, ret);
        if (0 == ret) 
        {
            return 0;
        }
        usleep(20000);
    }

    return -1;//失败
}

/**********************************************************************
* @name      : appctrl_get_zcu_3_input
* @brief     ：继电器消息收发
* @param[in] ：CTRL_RS232_T *pDevPara  设备句柄
               uint8 portNum           端口数量
* @param[out]：uint8 *input             接入配置
* @return    ：实际数量
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
int appctrl_get_zcu_3_input(CTRL_RS232_T *pDevPara, uint8 *input, uint8 portNum)
{
    uint8   datas[2] = {0};
    uint8   msglen = 0;
    uint8   Send_buf[200] = {0};
    uint8   Rev_buf[200] = {0};
    int     Rec_Len;
    int     ret = 0;
    uint8   RetryCnt = 0;
    uint8   i = 0;
    
    if(NULL == pDevPara || NULL == input || 0 == portNum)
    {
        return 4;//失败
    }

    if(0 == ioctl(pDevPara->fd, CONTROL_GET_MODSTATE, 0))
    {
        //检测到模块插入识别为未插入,返回-1，0表示插上,1未插模块
        msglen = appctrl_switch_package(datas, 0XA5, Send_buf);
        ret = ctrl_uart_send(pDevPara, Send_buf, msglen);
        CTRL_BUF_TRACE(Send_buf, msglen, "Ctrl %s send, ret %d", pDevPara->deviceName, ret);

        while (RetryCnt < 2)
        {
            RetryCnt ++;

            memset(Rev_buf, 0x0, sizeof(Rev_buf));
            Rec_Len = ctrl_uart_recv(pDevPara, Rev_buf, sizeof(Rev_buf), 50, 20);
            if(Rec_Len <= 0)
            {
                usleep(20000);
                continue;
            }
            
            ret = appctrl_switch_split(Rev_buf, Rec_Len);
            CTRL_BUF_TRACE(Rev_buf, Rec_Len, "Ctrl %s recv, ret %d", pDevPara->deviceName, ret);
            if(ret >= 0)
            {
                for(i = 0; i < portNum; i++)
                {
                    input[i] = (ret >> i) & 0x01;
                }
                return 4;
            }
            usleep(20000);
        }       
    }

    for(i = 0; i < portNum; i++)
    {
        input[i] = 1;
    }

    return 4;//失败
}

/**********************************************************************
* @name      : appctrl_switch_init_zcu_1
* @brief     ：控制模块继电器首次状态设置（专1）
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2023-1-18
* @Update    :
**********************************************************************/
void appctrl_switch_init_zcu_1()
{
    appctrl_IIC_ContactorExt(0xFF);
    appctrl_IIC_PrepareExt(PREPARE_OFF);
    appctrl_IIC_Contactor(0xFF);
    appctrl_IIC_Prepare(PREPARE_OFF);
}

/**********************************************************************
* @name      : appctrl_switch_init
* @brief     ：控制模块继电器初始化
* @param[in] ：uint8 turnMax 最大控制轮次
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-5
* @Update    :
**********************************************************************/
void appctrl_switch_init(uint8 port, uint8 turnMax)
{
    uint8 grp;
    uint8 turn;
    char  buf[4096] = {0};
    uint32 len = 0;
    int   ret = 0;
    
    g_TurnPort = port;   //当前一个模块有4个轮次，先写死
    g_TurnMax = turnMax;
    memset(&g_CtrlDev1, 0, sizeof(CTRL_RS232_T));
    memset(&g_CtrlDev2, 0, sizeof(CTRL_RS232_T));

    /* 获取文件的数据 */
    ret = appctrl_read_ctrl(CTRL_STATUS_TYPE, (uint8*)buf, 4096, &len);
    if(0 == ret && sizeof(g_BrakeBuf) == len)
    {
        memcpy(g_BrakeBuf, buf, len);
        
        memset(buf, 0, 4096);
        len = 0;
        for(turn = 0; turn < OOP_MAX_TURN; turn++)
        {
            len += sprintf(buf + len, "轮次%d: ", turn + 1);
            len += sprintf(buf + len, "%s ", TURN_ON_ALARM == g_BrakeBuf[turn].bRemoteAState ? "警" : "无");
            len += sprintf(buf + len, "%s\n", TURN_ON_STATE == g_BrakeBuf[turn].bRemoteSState ? "跳" : "合");
        }
        CTRL_FMT_DEBUG("重启前遥控状态:\n%s", buf);

        memset(buf, 0, 4096);
        len = 0;
        for(turn = 0; turn < OOP_MAX_TURN; turn++)
        {
            len += sprintf(buf + len, "轮次%d:", turn + 1);
            for(grp = 0; grp < OOP_MAX_TG; grp++)
            {
                len += sprintf(buf + len, " (%s ", TURN_ON_ALARM == g_BrakeBuf[turn].bPowerAState[grp] ? "警" : "无");
                len += sprintf(buf + len, "%s)", TURN_ON_STATE == g_BrakeBuf[turn].bPowerSState[grp] ? "跳" : "合");
            }
            len += sprintf(buf + len, "\n");
        }
        CTRL_FMT_DEBUG("重启前功控状态:\n%s", buf);

        memset(buf, 0, 4096);
        len = 0;
        for(turn = 0; turn < OOP_MAX_TURN; turn++)
        {
            len += sprintf(buf + len, "轮次%d:", turn + 1);
            for(grp = 0; grp < OOP_MAX_TG; grp++)
            {
                len += sprintf(buf + len, " (%s ", TURN_ON_ALARM == g_BrakeBuf[turn].bMonthENAState[grp] ? "警" : "无");
                len += sprintf(buf + len, "%s)", TURN_ON_STATE == g_BrakeBuf[turn].bMonthENSState[grp] ? "跳" : "合");
            }
            len += sprintf(buf + len, "\n");
        }
        CTRL_FMT_DEBUG("重启前月电控状态:\n%s", buf);

        memset(buf, 0, 4096);
        len = 0;
        for(turn = 0; turn < OOP_MAX_TURN; turn++)
        {
            len += sprintf(buf + len, "轮次%d:", turn + 1);
            for(grp = 0; grp < OOP_MAX_TG; grp++)
            {
                len += sprintf(buf + len, " (%s ", TURN_ON_ALARM == g_BrakeBuf[turn].bBuyENAState[grp] ? "警" : "无");
                len += sprintf(buf + len, "%s)", TURN_ON_STATE == g_BrakeBuf[turn].bBuyENSState[grp] ? "跳" : "合");
            }
            len += sprintf(buf + len, "\n");
        }
        CTRL_FMT_DEBUG("重启前购电控状态:\n%s", buf);
        return;
    }
    
    CTRL_FMT_DEBUG("文件中没有合法状态数据 %d, 数据长度 %d\n", ret, len);
    for(turn = 0; turn < OOP_MAX_TURN; turn++)
    {
        for(grp = 0; grp < OOP_MAX_TG; grp++)
        {
            if(g_BrakeBuf[turn].bDownAState[grp] != TURN_ON_ALARM)
                g_BrakeBuf[turn].bDownAState[grp] = TURN_OFF_ALARM;
            if(g_BrakeBuf[turn].bStopAState[grp] != TURN_ON_ALARM)
                g_BrakeBuf[turn].bStopAState[grp] = TURN_OFF_ALARM;
            if(g_BrakeBuf[turn].bRestAState[grp] != TURN_ON_ALARM)
                g_BrakeBuf[turn].bRestAState[grp] = TURN_OFF_ALARM;
            if(g_BrakeBuf[turn].bSegAState[grp] != TURN_ON_ALARM)
                g_BrakeBuf[turn].bSegAState[grp] = TURN_OFF_ALARM;
            if(g_BrakeBuf[turn].bBuyENAState[grp] != TURN_ON_ALARM)
                g_BrakeBuf[turn].bBuyENAState[grp] = TURN_OFF_ALARM;
            if(g_BrakeBuf[turn].bMonthENAState[grp] != TURN_ON_ALARM)
                g_BrakeBuf[turn].bMonthENAState[grp] = TURN_OFF_ALARM;
            if(g_BrakeBuf[turn].bPowerAState[grp] != TURN_ON_ALARM)
                g_BrakeBuf[turn].bPowerAState[grp] = TURN_OFF_ALARM;
            if(g_BrakeBuf[turn].bRemoteAState != TURN_ON_ALARM)
                g_BrakeBuf[turn].bRemoteAState = TURN_OFF_ALARM;
                
            if(g_BrakeBuf[turn].bDownSState[grp] != TURN_ON_STATE)
                g_BrakeBuf[turn].bDownSState[grp] = TURN_OFF_STATE;
            if(g_BrakeBuf[turn].bStopSState[grp] != TURN_ON_STATE)
                g_BrakeBuf[turn].bStopSState[grp] = TURN_OFF_STATE;
            if(g_BrakeBuf[turn].bRestSState[grp] != TURN_ON_STATE)
                g_BrakeBuf[turn].bRestSState[grp] = TURN_OFF_STATE;
            if(g_BrakeBuf[turn].bSegSState[grp] != TURN_ON_STATE)
                g_BrakeBuf[turn].bSegSState[grp] = TURN_OFF_STATE;
            if(g_BrakeBuf[turn].bBuyENSState[grp] != TURN_ON_STATE)
                g_BrakeBuf[turn].bBuyENSState[grp] = TURN_OFF_STATE;
            if(g_BrakeBuf[turn].bMonthENSState[grp] != TURN_ON_STATE)
                g_BrakeBuf[turn].bMonthENSState[grp] = TURN_OFF_STATE;
            if(g_BrakeBuf[turn].bPowerSState[grp] != TURN_ON_STATE)
                g_BrakeBuf[turn].bPowerSState[grp] = TURN_OFF_STATE;
            if(g_BrakeBuf[turn].bRemoteSState != TURN_ON_STATE)
                g_BrakeBuf[turn].bRemoteSState = TURN_OFF_STATE;
        }
         g_BrakeBuf[turn].bRemoteCState = TURN_OFF_STATE;
    }
}

/**********************************************************************
* @name      : apptrl_set_switch
* @brief     ：继电器工作
* @param[in] ：uint8 *DataBuf     数据内容
*              uint8 CtrlCode    控制码
* @param[out]：char *SendBuf     输出缓冲区
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
uint16 appctrl_switch_set_package(CTRL_SWITCH_STATUS_T *output, uint8 index, uint8 *sendBuf, uint16 buflen)
{
    uint8          msgbuf[256] = {0};
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_CTR_STATUS;
    uint8          i = 0;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_SetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 1;            //array
    Temp[offset++] = output->num;  //数量
    for(i = 0; i < output->num; i++)
    {
        Temp[offset++] = 2;            //structure
        Temp[offset++] = 2;            //数量

        Temp[offset++] = 17;            //unsigned
        Temp[offset++] = i + 1;

        Temp[offset++] = 22;            //enum
        Temp[offset++] = output->status[i];
    }
    pMsgInfo->len = offset;

    return mapmsg_package(pMsgInfo, sendBuf, buflen);
}

/**********************************************************************
* @name      : appctrl_switch_set_check
* @brief     ：解帧函数
* @param[in] ：uint8 *pBuf         接收数据
*              uint8 len        数据长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
int appctrl_switch_set_check(uint8 *pMsg, uint16 len, uint8 index, uint16 DT)
{
    int            ret = -1;
    MAPMSG_INFO_T *pMapMsg = NULL;
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    uint8  dir = 0;
    char  *dirName[3] = {"成功", "拒绝读写", "信息类未定义"};

    ret = mapmsg_split(pMsg, len, &pMapMsg);
    if(ret == 0 && NULL != pMapMsg)
    {
        if(MAPMSG_CTR_MSG != pMapMsg->cmt.cmd || pMapMsg->len < 4)
        {
            free(pMapMsg);
            return -1;
        }

        type = pMapMsg->apdu[0];
        temp = pMapMsg->apdu + 1;
        mapmsg_memcpy_r(&dt, temp, sizeof(uint16));
        temp += sizeof(uint16);
    
        if(MAPMSG_APDU_SetResponse == type && index == pMapMsg->cmt.fid && dt == DT)
        {
            dir = *temp;
            CTRL_FMT_DEBUG("配置%04X%s\n", DT, dir < 3 ? dirName[dir] : "失败, 其他原因");
            ret = dir;
        }
        else
        {
            CTRL_FMT_DEBUG("不匹配的消息 DT(%04X-%04X), FID(%d-%d)\n", dt, DT, pMapMsg->cmt.fid, index);
            ret = -2;
        }
        free(pMapMsg);
    }

    return ret;
}

/**********************************************************************
* @name      : appctrl_switch_status_out
* @brief     ：继电器消息收发
* @param[in] ：CTRL_RS232_T *pDevPara  设备句柄
               char *pMsg              发送消息
               uint8 msglen            消息长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
int appctrl_switch_status_out(CTRL_RS232_T *pDevPara, CTRL_SWITCH_STATUS_T *output)
{
    uint8   sendbuf[256];
    uint16  sendlen = 0;
    uint8   retryCnt = 0;
    uint8   recvbuf[256];        //接受缓冲区
    int     recvLen;
    int     ret = 0;
    uint8   index = ctrl_get_index();
    
    if(NULL == pDevPara || NULL == output)
    {
        return -1;//失败
    }

    /* 构造发送消息 */
    sendlen = appctrl_switch_set_package(output, index, sendbuf, 256);
    if(0 == sendlen)
    {
        CTRL_FMT_DEBUG("构造继电器输出控制消息失败\n");
        return -2;
    }

    /* 先接收缓存 */
    ctrl_uart_recv(pDevPara, recvbuf, 256, 50, 20);

    ret = ctrl_uart_send(pDevPara, sendbuf, sendlen);
    CTRL_BUF_DEBUG(sendbuf, sendlen, "设备%s发送消息%s", pDevPara->deviceName, 0 == ret ? "成功" : "失败");
    if(0 != ret)
    {
        return 1;
    }
    while (retryCnt < 3)
    {
        retryCnt ++;
        
        memset(recvbuf, 0x0, sizeof(recvbuf));
        recvLen = ctrl_uart_recv(pDevPara, recvbuf, 256, 200, 20);
        if(recvLen <= 0)
        {
            continue;
        }
        
        CTRL_BUF_DEBUG(recvbuf, recvLen, "接收设备%s应答", pDevPara->deviceName);
        ret = appctrl_switch_set_check(recvbuf, recvLen, index, MAPMSG_DT_CTR_STATUS);
        if (0 != ret) 
        {
            continue;
        }
        else
        {
            return 0;
        }
    }

    return -1;//失败
}

/**********************************************************************
* @name      : appctrl_switch_status_out
* @brief     ：继电器消息收发
* @param[in] ：CTRL_RS232_T *pDevPara  设备句柄
               uint8 status
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
int appctrl_switch_set_attri(CTRL_RS232_T *pDevPara, uint8 status)
{
    uint8   sendbuf[256];
    uint16  sendlen = 0;
    uint8   retryCnt = 0;
    uint8   recvbuf[256];        //接受缓冲区
    int     recvLen;
    int     ret = 0;
    uint8   index = ctrl_get_index();
    uint8          msgbuf[256] = {0};
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_CTR_RELAY_TYPE;
    
    if(NULL == pDevPara || status > 1)
    {
        return -1;//失败
    }

    /* 构造发送消息 */
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_SetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 22;            //enum
    Temp[offset++] = status + 1;    //1-脉冲模式;2-电平模式
    pMsgInfo->len = offset;

    sendlen = mapmsg_package(pMsgInfo, sendbuf, 256);

    while (retryCnt < 3)
    {
        retryCnt ++;
        
        ret = ctrl_uart_send(pDevPara, sendbuf, sendlen);
        CTRL_BUF_DEBUG(sendbuf, sendlen, "设备%s发送消息%s", pDevPara->deviceName, 0 == ret ? "成功" : "失败");
        if(0 != ret)
        {
            usleep(20000);
            continue;
        }
        
        memset(recvbuf, 0x0, sizeof(recvbuf));
        recvLen = ctrl_uart_recv(pDevPara, recvbuf, 256, 100, 20);
        if(recvLen <= 0)
        {
            usleep(20000);
            continue;
        }
        
        CTRL_BUF_DEBUG(recvbuf, recvLen, "接收设备%s应答", pDevPara->deviceName);
        ret = appctrl_switch_set_check(recvbuf, recvLen, index, MAPMSG_DT_CTR_RELAY_TYPE);
        if (0 != ret) 
        {
            usleep(20000);
            continue;
        }
        else
        {
            return 0;
        }
    }

    return -1;//失败
}


/**********************************************************************
* @name      : appctrl_switch_close_out
* @brief     ：合闸继电器操作
* @param[in] ：CTRL_SWITCH_STATUS_T *output 控制状态
               CTRL_SWITCH_STATUS_T *flag   有效标记
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-28
* @Update    :
**********************************************************************/
int appctrl_switch_close_out(CTRL_SWITCH_STATUS_T *output, CTRL_SWITCH_STATUS_T *flag)
{
    uint32 i = 0;
    int  ret = 0;

    if(NULL == g_ecudev)
    {
        g_ecudev = (DRV_CLOSING_DEVICE_T *)hal_device_get(HW_DEVICE_ID_DRV_CLOSING);
        CTRL_FMT_DEBUG("打开合闸设备%s\n", HW_DEVICE_ID_DRV_CLOSING);
    }

    if(NULL != g_ecudev)
    {
        for(i = 0; i < output->num; i++)
        {
            if(0 == flag->status[i])
            {
                continue;
            }
            ret |= g_ecudev->closing_turn(g_ecudev, i + 1, output->status[i]);
            CTRL_FMT_DEBUG("设置合闸继电器%d%s动作%s\n", i + 1, 0 == output->status[i] ? "" : "取消",
                                                         0 == ret ? " 成功" : "失败");
        }
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("设置继电器动作失败\n");
            hal_device_release((HW_DEVICE*)g_ecudev);
            g_ecudev = NULL;
        }
    }
    return ret;
}

/**********************************************************************
* @name      : appctrl_switch_alarm_out
* @brief     ：告警继电器操作
* @param[in] ：CTRL_SWITCH_STATUS_T *output 控制状态
               CTRL_SWITCH_STATUS_T *flag   有效标记
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-28
* @Update    :
**********************************************************************/
int appctrl_switch_alarm_out(uint8 status)
{
    int  ret = 0;

    if(NULL == g_ecudev)
    {
        g_ecudev = (DRV_CLOSING_DEVICE_T *)hal_device_get(HW_DEVICE_ID_DRV_CLOSING);
        CTRL_FMT_DEBUG("打开合闸设备%s\n", HW_DEVICE_ID_DRV_CLOSING);
    }

    if(NULL != g_ecudev)
    {
        ret = g_ecudev->closing_turn_alarm(g_ecudev, status);
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("设置告警继电器%s动作失败\n", 0 == status ? "" : "取消");
            hal_device_release((HW_DEVICE*)g_ecudev);
            g_ecudev = NULL;
        }
    }
    else
    {
        return -1;
    }
    return ret;
}


/**********************************************************************
* @name      : appctrl_module_input_package
* @brief     ：获取模组接入检测消息组装
* @param[in] ：uint8 index       序号
*              uint16 buflen    缓冲区大小
* @param[out]：char *SendBuf     输出缓冲区
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2022-7-1
* @Update    :
**********************************************************************/
uint16 appctrl_module_input_package(uint8 index, uint8 *sendBuf, uint16 buflen)
{
    uint8          msgbuf[256] = {0};
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_CTR_LOOP_STATUS;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_GetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    pMsgInfo->len = offset;

    return mapmsg_package(pMsgInfo, sendBuf, buflen);
}

/**********************************************************************
* @name      : appctrl_module_input_split
* @brief     ：解帧函数
* @param[in] ：uint8 *pBuf       接收数据
*              uint8 len        数据长度
               uint8 index      序号
               uint16 DT        消息
               uint8 buflen     缓存
* @param[out]：uint8 *pInput     接入
* @return    ：接口数量
* @Create    : 王津剑
* @Date      ：2022-7-1
* @Update    :
**********************************************************************/
uint8 appctrl_module_input_split(uint8 *pMsg, uint16 len, uint8 index, uint16 DT, uint8 *pInput, uint8 buflen)
{
    int            ret = -1;
    uint8          portnum = 0;
    MAPMSG_INFO_T *pMapMsg = NULL;
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    uint8  dir = 0;
    uint8  choice = 1;
    char  *dirName[3] = {"成功", "拒绝读写", "信息类未定义"};

    ret = mapmsg_split(pMsg, len, &pMapMsg);
    if(ret == 0 && NULL != pMapMsg)
    {
        if(MAPMSG_CTR_MSG != pMapMsg->cmt.cmd || pMapMsg->len < 4)
        {
            free(pMapMsg);
            return portnum;
        }

        type = pMapMsg->apdu[0];
        temp = pMapMsg->apdu + 1;
        mapmsg_memcpy_r(&dt, temp, sizeof(uint16));
        temp += sizeof(uint16);
        choice = *temp;
        temp++;
    
        if(MAPMSG_APDU_GetResponse == type && index == pMapMsg->cmt.fid && dt == DT && 1 == choice)
        {
            uint8 i = 0;
            if(1 == *temp) //array
            {
                temp++;
                portnum = *temp;
                temp++;
                for(i = 0; i < portnum && i < buflen; i++)
                {
                    temp++;
                    pInput[i] = *temp;
                    temp++;
                }
            }
            
            portnum = *temp;
            temp++;
            
        }
        else
        {
            if(1 == choice)
            {
                RSP_BUF_DEBUG(pMsg, len, "不匹配的消息 DT(%04X-%04X), FID(%d-%d)\n", dt, DT, pMapMsg->cmt.fid, index);
            }
            else
            {
                dir = *temp;
                RSP_BUF_DEBUG(pMsg, len, "获取%04X%s\n", DT, dir < 3 ? dirName[dir] : "失败, 其他原因");
            }
        }
        free(pMapMsg);
    }
    else
    {
        RSP_BUF_DEBUG(pMsg, len, "未知消息\n");
    }

    return portnum;
}


/**********************************************************************
* @name      : appctrl_module_input_get
* @brief     ：获取模组接入检测
* @param[in] ：CTRL_RS232_T *pDevPara  设备
               uint8 portNum           端口数量
* @param[out]：uint8 *input             接入配置
* @return    ：实际数量
* @Create    : 王津剑
* @Date      ：2022-7-1
* @Update    :
**********************************************************************/
uint8 appctrl_module_input_get(CTRL_RS232_T *pDevPara, uint8 *input, uint8 portNum)
{
    uint8   sendbuf[256];
    uint16  sendlen = 0;
    uint8   retryCnt = 0;
    uint8   recvbuf[256];        //接受缓冲区
    int     recvLen;
    int     ret = 0;
    uint8   portnum = 0;
    uint8   index = ctrl_get_index();
    
    if(NULL == pDevPara || NULL == input || 0 == portNum)
    {
        return 0;//失败
    }

    /* 构造发送消息 */
    sendlen = appctrl_module_input_package(index, sendbuf, 256);
    if(0 == sendlen)
    {
        CTRL_FMT_DEBUG("构造继电器接线状态消息失败\n");
        return 0;
    }

    ret = ctrl_uart_send(pDevPara, sendbuf, sendlen);
    if(0 != ret)
    {
        hal_device_release((HW_DEVICE *)pDevPara->pDev);
        pDevPara->pDev = NULL;
        CTRL_BUF_DEBUG(sendbuf, sendlen, "设备%s发送消息失败", pDevPara->deviceName);
        return 0;
    }
    CTRL_BUF_TRACE(sendbuf, sendlen, "设备%s发送消息", pDevPara->deviceName);

    while (retryCnt < 3)
    {
        retryCnt ++;
        memset(recvbuf, 0x0, sizeof(recvbuf));
        recvLen = ctrl_uart_recv(pDevPara, recvbuf, 256, 50, 20);
        if(recvLen <= 0)
        {
            continue;
        }
        CTRL_BUF_TRACE(recvbuf, recvLen, "设备%s接收消息", pDevPara->deviceName);
        portnum = appctrl_module_input_split(recvbuf, recvLen, index, MAPMSG_DT_CTR_LOOP_STATUS, input, portNum);
        if (ret <= 0) 
        {
            continue;
        }
        else
        {
            return portnum;
        }
    }

    return 0;//失败
}



/**********************************************************************
* @name      : appctrl_close_set_attr
* @brief     ：合闸继电器设置
* @param[in] ：uint8 status           属性
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-7-1
* @Update    :
**********************************************************************/
uint8 appctrl_close_set_attr(uint8 status)
{
    int  ret = 1;

    if(NULL == g_ecudev)
    {
        g_ecudev = (DRV_CLOSING_DEVICE_T *)hal_device_get(HW_DEVICE_ID_DRV_CLOSING);
        CTRL_FMT_DEBUG("打开合闸设备%s\n", HW_DEVICE_ID_DRV_CLOSING);
    }

    if(NULL != g_ecudev)
    {
        ret = g_ecudev->closing_mode_set(g_ecudev, status);
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("设置继电器状态失败\n");
            hal_device_release((HW_DEVICE*)g_ecudev);
            g_ecudev = NULL;
        }
    }
    else
    {
        CTRL_FMT_DEBUG("打开合闸设备%s失败\n", HW_DEVICE_ID_DRV_CLOSING);
    }

    return ret;
}

/**********************************************************************
* @name      : appctrl_switch_close_out
* @brief     ：合闸继电器操作
* @param[in] ：uint8 portNum           端口数量
* @param[out]：uint8 *input            接入配置
* @return    ：实际数量
* @Create    : 王津剑
* @Date      ：2022-7-1
* @Update    :
**********************************************************************/
uint8 appctrl_close_input_get(uint8 *input, uint8 portNum)
{
    uint8  i = 0;
    uint32 status = 0xFFFFFFFF;
    int  ret = 0;

    if(NULL == g_ecudev)
    {
        g_ecudev = (DRV_CLOSING_DEVICE_T *)hal_device_get(HW_DEVICE_ID_DRV_CLOSING);
        CTRL_FMT_DEBUG("打开合闸设备%s\n", HW_DEVICE_ID_DRV_CLOSING);
    }

    if(NULL != g_ecudev)
    {
        ret = g_ecudev->closing_line_state_get(g_ecudev, &status);
        if(0 != ret)
        {
            status = 0xFFFFFFFF;
            CTRL_FMT_DEBUG("获取合闸接入状态失败\n");
            hal_device_release((HW_DEVICE*)g_ecudev);
            g_ecudev = NULL;
        }
    }
    else
    {
        CTRL_FMT_DEBUG("打开合闸设备%s失败\n", HW_DEVICE_ID_DRV_CLOSING);
    }

    for(i = 0; i < portNum; i++)
    {
        input[i] = 1 - ((status >> i) & 0x01);
    }
    return portNum;
}

/**********************************************************************
* @name      : appctrl_set_switch_zcu1
* @brief     ：继电器工作(专变3型)
* @param[in] ：SWITCH_INTERFACE_T *pInterface 继电器动作
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-7-28
* @Update    :
**********************************************************************/
int appctrl_set_switch_zcu1(SWITCH_INTERFACE_T *pInterface)
{
    static uint8         s_ZOpen = 0xff;  //跳闸
    static uint8         s_Zclose[OOP_MAX_TURN] = {1, 1, 1, 1, 1, 1, 1, 1};//合闸
    uint8                curOpen = 0;
    uint8                curClose = 0;
    uint8                zAlarm = 0x40;
    static uint8         sswitchFirst = TRUE;
    struct timespec      curtime = {0};
    static uint32        sOpen_setTime = 0;
    uint8                i = 0;
    uint8                prepare = 0;
    uint8                prepareExt = 0;
    int                  ret = 0;
    
    /* 默认使用脉冲模式 */
    for(i = 0; i < g_TurnPort; i++)
    {
        if(1 == pInterface->TURN_CTRL[i])
        {
            curOpen |= (0x01 << i);
        }
        if(1 == pInterface->CLOSE_CTRL[i])
        {
            //curClose |= (0x01 << i);
            if(1 == pInterface->TURN_CTRL[i])
            {
                appctrl_ctrl_led(EMLED_ROUND1G + 2 * i, FALSE);
                appctrl_ctrl_led(EMLED_ROUND1R + 2 * i, TRUE);
            }
            else
            {
                appctrl_ctrl_led(EMLED_ROUND1G + 2 * i, TRUE);
                appctrl_ctrl_led(EMLED_ROUND1R + 2 * i, FALSE);
            }
        }
        else
        {
            appctrl_ctrl_led(EMLED_ROUND1G + 2 * i, TRUE);
            appctrl_ctrl_led(EMLED_ROUND1R + 2 * i, TRUE);
        }
        //已经动作过的下次不动作（针对合闸继电器）
        if(pInterface->CLOSE_CTRL[i] == s_Zclose[i] || 1 == pInterface->CLOSE_CTRL[i])
        {
            curClose |= (0x01 << i);
        }
        s_Zclose[i] = pInterface->CLOSE_CTRL[i];
    }

    appctrl_ctrl_led(EMLED_PC, pInterface->POWER_CTRL_LIGHT);
    appctrl_ctrl_led(EMLED_EC, pInterface->ELE_CTRL_LIGHT);

    if(FALSE != appctrl_alarm_zcu1_get())
    {
        zAlarm = 0;
        appctrl_alarm_zcu1(FALSE);
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    
    if(TRUE == sswitchFirst || s_ZOpen != curOpen || 0 == zAlarm || 0xFF != curClose ||
       (curtime.tv_sec >= sOpen_setTime + 60 && 0xFF != curOpen)) 
    {
        if(s_ZOpen != curOpen || (curtime.tv_sec >= sOpen_setTime + 60 && 0xFF != curOpen))
        {
            appctrl_IIC_PrepareExt(PREPARE_ON);
            prepareExt = 1;
        }
        if(0xFF != curClose || 0 == zAlarm)
        {
            appctrl_IIC_Prepare(PREPARE_ON | zAlarm);
            if(0 == zAlarm)
            {
                CTRL_FMT_DEBUG("告警继电器 动作\n");
                zAlarm = 0x40;
            }
            prepare = 1;
        }
        usleep(600000);
    
        if(1 == prepareExt)
        {
            if(s_ZOpen != curOpen || (curtime.tv_sec >= sOpen_setTime + 60 && 0xFF != curOpen))
            {
                ret = appctrl_IIC_ContactorExt(curOpen);
                CTRL_FMT_DEBUG("跳闸操作 %02X %s\n", curOpen, 0 == ret ? "成功" : "失败");
                s_ZOpen = curOpen;
                sOpen_setTime = curtime.tv_sec;
            }
        }

        if(1 == prepare)
        {
            appctrl_IIC_Prepare(PREPARE_ON | zAlarm); //关闭告警继电器
            if(0xFF  != curClose)
            {
                ret = appctrl_IIC_Contactor(curClose);
                CTRL_FMT_DEBUG("合闸操作 %02X %s\n", curClose, 0 == ret ? "成功" : "失败");
            }
        }

        /* 脉冲默认范围300ms±100ms */
        usleep(280000);
        appctrl_IIC_ContactorExt(0xFF);
        appctrl_IIC_PrepareExt(PREPARE_OFF);
        appctrl_IIC_Contactor(0xFF);
        appctrl_IIC_Prepare(PREPARE_OFF);
        
    }
    sswitchFirst = FALSE;
    
    return 0;
}


/**********************************************************************
* @name      : appctrl_set_switch_zcu3
* @brief     ：继电器工作(专变3型)
* @param[in] ：uint8 acm1   设备1
               uint8 acm2   设备2
               SWITCH_INTERFACE_T *pInterface 继电器动作
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-7-13
* @Update    :
**********************************************************************/
int appctrl_set_switch_zcu3(uint8 acm1, uint8 subport1, SWITCH_INTERFACE_T *pInterface)
{
    static SWITCH_BITS_T if_output;
    SWITCH_BITS_T        cur_output;
    SWITCH_BITS_T        qx_output;
    SWITCH_BITS_T        led_output;
    static uint8         sswitchFirst = TRUE;
    uint8                SendBuf[255];
    uint8                SendLen  = 0;
    uint8                sendFlag = 0;  //是否发送过指令，防止同一轮中多次发送控制命令
    struct timespec      curtime  = {0};
    static uint32        sctrl_setTime = 0;
    static uint32        ledFlag = 0;    //用于转换灯的颜色
    static uint8         Attri_flag = 0; //用于记录脉冲式跳闸的状态
    uint8                bHasOn = FALSE;
    uint8                bRed   = FALSE;         //判断跳闸红灯点亮
    uint8                bGreen = FALSE;         //判断是否绿灯
    uint8                bLedOn = FALSE;         //判断是否灯闪烁
    uint8                bRemote_status = FALSE; //判断远程状态下是否黄灯
    uint8                bTurn_status = FALSE;   //判断轮次未投入下是否灯灭
    uint8                i = 0;
    int                  ret = 0;
    
    ctrl_update_devname(acm1, subport1, &g_CtrlDev1);
    if(0 != acm1 && NULL == g_CtrlDev1.pDev)
    {
        if(0 != ctrl_uart_Init(&g_CtrlDev1))
        {
            CTRL_FMT_DEBUG("ctrl_uart_Init %s failed\n", g_CtrlDev1.deviceName);
        }
    }

    if(NULL == g_CtrlDev1.pDev)
    {
        sswitchFirst = TRUE;
        return 0;
    }
    ret = ioctl(g_CtrlDev1.fd, CONTROL_GET_MODSTATE, 0);
    if(0 != ret)
    {
        //检测到模块插入识别为未插入,返回-1，0表示插上,1未插模块
        sswitchFirst = TRUE;
        return 0;
    }

    /* 先不管控灯 */
    memset(&cur_output, 0x0, sizeof(SWITCH_BITS_T));
    memset(&led_output, 0x0, sizeof(SWITCH_BITS_T));
    cur_output.POWER_CTRL_LIGHT   = pInterface->POWER_CTRL_LIGHT;
    cur_output.ELE_CTRL_LIGHT     = pInterface->ELE_CTRL_LIGHT;
    cur_output.ALARM_OUTPUT       = pInterface->ALARM_OUTPUT; //告警继电器默认不输出
    cur_output.PROTECT_LIGHT      = pInterface->PROTECT_LIGHT;
    cur_output.TURN_1_CTRL        = pInterface->TURN_CTRL[0];
    cur_output.TURN_1_REDLIGHT    = pInterface->TURN_REDLIGHT[0];
    cur_output.TURN_1_BLUELIGHT   = pInterface->TURN_BLUELIGHT[0];
    cur_output.TURN_2_CTRL        = pInterface->TURN_CTRL[1];
    cur_output.TURN_2_REDLIGHT    = pInterface->TURN_REDLIGHT[1];
    cur_output.TURN_2_BLUELIGHT   = pInterface->TURN_BLUELIGHT[1];
    cur_output.TURN_3_CTRL        = pInterface->TURN_CTRL[2];
    cur_output.TURN_3_REDLIGHT    = pInterface->TURN_REDLIGHT[2];
    cur_output.TURN_3_BLUELIGHT   = pInterface->TURN_BLUELIGHT[2];
    cur_output.TURN_4_CTRL        = pInterface->TURN_CTRL[3];
    cur_output.TURN_4_REDLIGHT    = pInterface->TURN_REDLIGHT[3];
    cur_output.TURN_4_BLUELIGHT   = pInterface->TURN_BLUELIGHT[3];
    memcpy(&led_output, &cur_output, sizeof(SWITCH_BITS_T));
    
    for(i = 0; i < 4; i++)
    {
        /* 有继电器动作才补跳 */
        if(0 == pInterface->TURN_CTRL[i])
        {
            bHasOn = TRUE;
        }
    }
    
    for(i = 0; i < 4; i++)
    {
        pInterface->Attri_OPEN[i] = appctrl_get_switch_type(i);
    }

    /*  灯控说明，按照以下优先级进行控灯，判断逻辑相反
        1.跳闸：红灯；
        2.远程合闸：黄灯；
        3.合闸未投入任何控制：灯灭；
        4.合闸投入轮次但未接入：灯闪烁；
        5.合闸投入且接入：绿灯。
    */
    
    for(i = 0; i < 4; i++)
    {
        /*合闸状态投入轮次并且接线接入(0) 绿灯点亮*/
        if(1 == pInterface->TURN_CTRL[i] && 1 == pInterface->Ctrl_STATUS[i] && 0 == pInterface->INPUT_STATUS[i])
        {
            bGreen = TRUE;
        }
        /* 合闸状态投入轮次但接线未接入(1)，控制灯闪烁*/
        if(1 == pInterface->TURN_CTRL[i] && 1 == pInterface->Ctrl_STATUS[i] && 1 == pInterface->INPUT_STATUS[i])
        {
            bLedOn = TRUE;
        }
        /* 合闸状态未投入任何轮次， 红灯绿灯都灭*/
        if(0 == pInterface->Ctrl_STATUS[i])
        {
            bTurn_status = TRUE;
        }
        /* 远程合闸状态，控制绿灯红灯一起亮，即黄灯*/
        if(0 == pInterface->CLOSE_CTRL[i])
        {
            bRemote_status = TRUE;
        }
        /* 跳闸    红灯点亮*/
        if(0 == pInterface->TURN_CTRL[i])
        {
            bRed = TRUE;
        }
        
    }

    /*合闸状态投入轮次并且接线接入, 绿灯点亮*/
    if(TRUE == bGreen)
    {
        if(1 == pInterface->TURN_CTRL[0] && 1 == pInterface->Ctrl_STATUS[0] && 0 == pInterface->INPUT_STATUS[0])
        {
            led_output.TURN_1_BLUELIGHT = 1;
            led_output.TURN_1_REDLIGHT  = 0;
        }
        if(1 == pInterface->TURN_CTRL[1] && 1 == pInterface->Ctrl_STATUS[1] && 0 == pInterface->INPUT_STATUS[1])
        {
            led_output.TURN_2_BLUELIGHT = 1;
            led_output.TURN_2_REDLIGHT  = 0;
        }
        if(1 == pInterface->TURN_CTRL[2] && 1 == pInterface->Ctrl_STATUS[2] && 0 == pInterface->INPUT_STATUS[2])
        {
            led_output.TURN_3_BLUELIGHT = 1;
            led_output.TURN_3_REDLIGHT  = 0;
        }
        if(1 == pInterface->TURN_CTRL[3] && 1 == pInterface->Ctrl_STATUS[3] && 0 == pInterface->INPUT_STATUS[3])
        {
            led_output.TURN_4_BLUELIGHT = 1;
            led_output.TURN_4_REDLIGHT  = 0;
        }
        ledFlag = 1 - ledFlag;
    }

    /* 合闸状态投入轮次但接线未接入(1)，控制灯闪烁*/
    if(TRUE == bLedOn)
    {
        if(1 == pInterface->TURN_CTRL[0] && 1 == pInterface->INPUT_STATUS[0] && 1 == pInterface->Ctrl_STATUS[0])
        {
            led_output.TURN_1_BLUELIGHT = 1 - ledFlag;
            led_output.TURN_1_REDLIGHT = ledFlag;
        }
        if(1 == pInterface->TURN_CTRL[1] && 1 == pInterface->INPUT_STATUS[1] && 1 == pInterface->Ctrl_STATUS[1])
        {
            led_output.TURN_2_BLUELIGHT = 1 - ledFlag;
            led_output.TURN_2_REDLIGHT = ledFlag;
        }
        if(1 == pInterface->TURN_CTRL[2] && 1 == pInterface->INPUT_STATUS[2] && 1 == pInterface->Ctrl_STATUS[2])
        {
            led_output.TURN_3_BLUELIGHT = 1 - ledFlag;
            led_output.TURN_3_REDLIGHT = ledFlag;
        }
        if(1 == pInterface->TURN_CTRL[3] && 1 == pInterface->INPUT_STATUS[3] && 1 == pInterface->Ctrl_STATUS[3])
        {
            led_output.TURN_4_BLUELIGHT = 1 - ledFlag;
            led_output.TURN_4_REDLIGHT = ledFlag;
        }
        ledFlag = 1 - ledFlag;
    }
    
    /* 合闸状态未投入任何轮次， 红灯绿灯都灭*/
    if(TRUE == bTurn_status)
    {
        if(0 == pInterface->Ctrl_STATUS[0] && 1 == pInterface->TURN_CTRL[0])
        {
            led_output.TURN_1_BLUELIGHT = 0;
            led_output.TURN_1_REDLIGHT  = 0;
        }
        if(0 == pInterface->Ctrl_STATUS[1] && 1 == pInterface->TURN_CTRL[1])
        {
            led_output.TURN_2_BLUELIGHT = 0;
            led_output.TURN_2_REDLIGHT  = 0;
        }
        if(0 == pInterface->Ctrl_STATUS[2] && 1 == pInterface->TURN_CTRL[2])
        {
            led_output.TURN_3_BLUELIGHT = 0;
            led_output.TURN_3_REDLIGHT  = 0;
        }
        if(0 == pInterface->Ctrl_STATUS[3] && 1 == pInterface->TURN_CTRL[3])
        {
            led_output.TURN_4_BLUELIGHT = 0;
            led_output.TURN_4_REDLIGHT  = 0;
        }
    }

    /* 远程合闸状态，控制绿灯红灯一起亮，即黄灯*/
    if(TRUE == bRemote_status)
    {
        if(0 == pInterface->CLOSE_CTRL[0])
        {
            led_output.TURN_1_BLUELIGHT = 1;
            led_output.TURN_1_REDLIGHT  = 1;
        }
        if(0 == pInterface->CLOSE_CTRL[1])
        {
            led_output.TURN_2_BLUELIGHT = 1;
            led_output.TURN_2_REDLIGHT  = 1;
        }
        if(0 == pInterface->CLOSE_CTRL[2])
        {
            led_output.TURN_3_BLUELIGHT = 1;
            led_output.TURN_3_REDLIGHT  = 1;
        }
        if(0 == pInterface->CLOSE_CTRL[3])
        {
            led_output.TURN_4_BLUELIGHT = 1;
            led_output.TURN_4_REDLIGHT  = 1;
        }
    }
    
     /* 跳闸状态      红灯点亮*/
    if(TRUE == bRed)
    {
        if(0 == pInterface->TURN_CTRL[0])
        {
            led_output.TURN_1_BLUELIGHT = 0;
            led_output.TURN_1_REDLIGHT  = 1;
        }
        if(0 == pInterface->TURN_CTRL[1])
        {
            led_output.TURN_2_BLUELIGHT = 0;
            led_output.TURN_2_REDLIGHT  = 1;
        }
        if(0 == pInterface->TURN_CTRL[2])
        {
            led_output.TURN_3_BLUELIGHT = 0;
            led_output.TURN_3_REDLIGHT  = 1;
        }
        if(0 == pInterface->TURN_CTRL[3])
        {
            led_output.TURN_4_BLUELIGHT = 0;
            led_output.TURN_4_REDLIGHT  = 1;
        }
    }

     clock_gettime(CLOCK_MONOTONIC, &curtime);
    
    /* 专3控制模块不会自己取消动作 */
    if(TRUE == sswitchFirst || 0 != memcmp(&cur_output, &if_output, sizeof(SWITCH_BITS_T)) ||
       (curtime.tv_sec >= sctrl_setTime + 60 && TRUE == bHasOn))
    {
        sendFlag = 1;
        memcpy(&if_output, &cur_output, sizeof(CTRL_SWITCH_STATUS_T));
        
        if(bLedOn||bRemote_status||bTurn_status||bRed||bGreen)
        {
            cur_output.TURN_1_REDLIGHT = led_output.TURN_1_REDLIGHT;
            cur_output.TURN_2_REDLIGHT = led_output.TURN_2_REDLIGHT;
            cur_output.TURN_3_REDLIGHT = led_output.TURN_3_REDLIGHT;
            cur_output.TURN_4_REDLIGHT = led_output.TURN_4_REDLIGHT;
            cur_output.TURN_1_BLUELIGHT = led_output.TURN_1_BLUELIGHT;
            cur_output.TURN_2_BLUELIGHT = led_output.TURN_2_BLUELIGHT;
            cur_output.TURN_3_BLUELIGHT = led_output.TURN_3_BLUELIGHT;
            cur_output.TURN_4_BLUELIGHT = led_output.TURN_4_BLUELIGHT;
        }

        if(TRUE == bHasOn)
        {
            Attri_flag = 0; //重新记录脉冲属性的继电器跳闸状态(放在跳闸动作前，防止运行卡)
            memcpy(&qx_output, &cur_output, sizeof(CTRL_SWITCH_STATUS_T));
            if(0 == pInterface->Attri_OPEN[0])
            {
                qx_output.TURN_1_CTRL = 1;
                Attri_flag = 1;
            }
            if(0 == pInterface->Attri_OPEN[1])
            {
                qx_output.TURN_2_CTRL = 1;
                Attri_flag |= 1 << 1;
            }
            if(0 == pInterface->Attri_OPEN[2])
            {
                qx_output.TURN_3_CTRL = 1;
                Attri_flag |= 1 << 2;
            }
            if(0 == pInterface->Attri_OPEN[3])
            {
                qx_output.TURN_4_CTRL = 1;
                Attri_flag |= 1 << 3;
            }
        }
        
        SendLen = appctrl_switch_package((uint8 *)&cur_output, 0xA3, SendBuf);
        appctrl_set_zcu_3_status_flag(&g_CtrlDev1, SendBuf, SendLen, 0);
        sctrl_setTime = curtime.tv_sec;

        /* 脉冲模式取消合闸, 300±100ms */
        if(TRUE == bHasOn && Attri_flag != 0)
        {
            //如果都是电平模式，下面就不需要执行
            usleep(230000);
            SendLen = appctrl_switch_package((uint8 *)&qx_output, 0xA3, SendBuf);
            appctrl_set_zcu_3_status_flag(&g_CtrlDev1, SendBuf, SendLen, 0);
            CTRL_FMT_DEBUG("脉冲模式恢复状态, flag[%d]\n", Attri_flag);
        }
    }
       
    if(0 == sendFlag && (bLedOn||bRemote_status||bTurn_status||bRed||bGreen))
    {
        //如果脉冲模式进行了合闸 控灯时 脉冲继电器不动作
        if(Attri_flag & 0x1)
        {
            led_output.TURN_1_CTRL = 1;
        }
        if((Attri_flag>>1) & 0x1)
        {
            led_output.TURN_2_CTRL = 1;
        }
        if((Attri_flag>>2) & 0x1)
        {
            led_output.TURN_3_CTRL = 1;
        }
        if((Attri_flag>>3) & 0x1)
        {
            led_output.TURN_4_CTRL = 1;
        }
        SendLen = appctrl_switch_package((uint8 *)&led_output, 0xA3, SendBuf);
        appctrl_set_zcu_3_status_flag(&g_CtrlDev1, SendBuf, SendLen, 1);
    }

    sswitchFirst = FALSE;
    return 0;

}

/**********************************************************************
* @name      : appctrl_set_switch
* @brief     ：继电器工作
* @param[in] ：uint8 acm1   设备1
               uint8 acm2   设备2
               SWITCH_INTERFACE_T *pInterface 继电器动作
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
int appctrl_set_switch(uint8 acm1, uint8 subport1, uint8 acm2, uint8 subport2, SWITCH_INTERFACE_T *pInterface)
{
    static CTRL_SWITCH_STATUS_T if_output1;
    static CTRL_SWITCH_STATUS_T if_output2;
    CTRL_SWITCH_STATUS_T        cur_output;
    uint32                      i;
    static uint8                switchAttr = 0;
    static uint8                bswitchFirst[2] = {TRUE, TRUE};
    static uint32               ctrl_setTime[2] = {0, 0};
    struct timespec             curtime = {0};
    uint8                       bHasOn = FALSE;
    int                         ret = 0;
    
    ctrl_update_devname(acm1, subport1, &g_CtrlDev1);
    ctrl_update_devname(acm2, subport2, &g_CtrlDev2);

    if(0 != acm1 && NULL == g_CtrlDev1.pDev)
    {
        if(0 != ctrl_uart_Init(&g_CtrlDev1))
        {
            CTRL_FMT_DEBUG("ctrl_uart_Init %s failed\n", g_CtrlDev1.deviceName);
        }
        usleep(1000);
    }
    if(0 != acm2 && NULL == g_CtrlDev2.pDev)
    {
        if(0 != ctrl_uart_Init(&g_CtrlDev2))
        {
            CTRL_FMT_DEBUG("ctrl_uart_Init %s failed\n", g_CtrlDev2.deviceName);
        }
        usleep(1000);
    }

    for(i = 0; i < pInterface->portMax; i++)
    {
        pInterface->Attri_OPEN[i] = appctrl_get_switch_type(i);
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    memset(&cur_output, 0x0, sizeof(CTRL_SWITCH_STATUS_T));
    if(NULL != g_CtrlDev1.pDev)
    {
        bHasOn = FALSE;
        cur_output.num      = pInterface->portMax;
        for(i = 0; i < cur_output.num && i < OOP_MAX_TURN; i++)
        {
            cur_output.status[i] = pInterface->TURN_CTRL[i];
            if(0 == cur_output.status[i])
            {
                bHasOn = TRUE;
            }
        }
        if(TRUE == bswitchFirst[0] || switchAttr != pInterface->Attri_OPEN[0] )
        {
            /* 属性设置 */
            switchAttr = pInterface->Attri_OPEN[0];
            ret = appctrl_switch_set_attri(&g_CtrlDev1, switchAttr);
            CTRL_FMT_DEBUG("设置继电器1属性%s%s\n", 0 == switchAttr ? "脉冲式" : "保持式", 0 == ret ? "成功" : "失败");
        }
        if(TRUE == bswitchFirst[0] || 0 != memcmp(&cur_output, &if_output1, sizeof(CTRL_SWITCH_STATUS_T)) ||
           (curtime.tv_sec >= ctrl_setTime[0] + 60 && TRUE == bHasOn))
        {
            memcpy(&if_output1, &cur_output, sizeof(CTRL_SWITCH_STATUS_T));
            ret = appctrl_switch_status_out(&g_CtrlDev1, &if_output1);
            if(1 == ret)
            {
                hal_device_release((HW_DEVICE *)g_CtrlDev1.pDev);
                g_CtrlDev1.pDev = NULL;
            }
            ctrl_setTime[0] = curtime.tv_sec;
            bswitchFirst[0] = FALSE;
        }
    }
    else
    {
        bswitchFirst[0] = TRUE;
    }
    
    if(NULL != g_CtrlDev2.pDev)
    {
        bHasOn = FALSE;
        cur_output.num      = pInterface->portMax;
        for(i = 0; i < cur_output.num && i + pInterface->portMax < OOP_MAX_TURN; i++)
        {
            cur_output.status[i] = pInterface->TURN_CTRL[i + pInterface->portMax];
            if(0 == cur_output.status[i])
            {
                bHasOn = TRUE;
            }
        }
        if(TRUE == bswitchFirst[1] || switchAttr != pInterface->Attri_OPEN[pInterface->portMax] )
        {
            /* 属性设置 */
            switchAttr = pInterface->Attri_OPEN[0];
            ret = appctrl_switch_set_attri(&g_CtrlDev2, switchAttr);
            CTRL_FMT_DEBUG("设置继电器2属性%s%s\n", 0 == switchAttr ? "脉冲式" : "保持式", 0 == ret ? "成功" : "失败");
        }
        if(TRUE == bswitchFirst[1] || 0 != memcmp(&cur_output, &if_output2, sizeof(CTRL_SWITCH_STATUS_T)) ||
           (curtime.tv_sec >= ctrl_setTime[1] + 60 && TRUE == bHasOn))
        {
            memcpy(&if_output2, &cur_output, sizeof(CTRL_SWITCH_STATUS_T));
            ret = appctrl_switch_status_out(&g_CtrlDev2, &if_output2);
            if(1 == ret)
            {
                hal_device_release((HW_DEVICE *)g_CtrlDev2.pDev);
                g_CtrlDev2.pDev = NULL;
            }
            ctrl_setTime[1] = curtime.tv_sec;
            bswitchFirst[1] = FALSE;
        }
    }
    else
    {
        bswitchFirst[1] = TRUE;
    }

    return 0;
}

/**********************************************************************
* @name      : appctrl_set_switch
* @brief     ：继电器工作
* @param[in] ：SWITCH_INTERFACE_T *pInterface 继电器动作
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
int appctrl_set_switch_close(SWITCH_INTERFACE_T *pInterface)
{
    static CTRL_SWITCH_STATUS_T if_outputSelf;
    CTRL_SWITCH_STATUS_T        cur_output;
    uint32                      i;
    static uint8                bswitchSelf = TRUE;
    static uint8                switchAttrSelf = TRUE;
    
    int                         ret = 0;

    for(i = 0; i < pInterface->portMax; i++)
    {
        pInterface->Attri_CLOSE[i] = appctrl_get_switch_type(i + pInterface->portMax);
    }

    /* 终端本地合闸操作 */
    cur_output.num      = pInterface->portMax;
    for(i = 0; i < cur_output.num && i < OOP_MAX_TURN; i++)
    {
        cur_output.status[i] = pInterface->CLOSE_CTRL[i];
    }
    if(TRUE == bswitchSelf || switchAttrSelf != pInterface->Attri_CLOSE[0] )
    {
        /* 属性设置 */
        switchAttrSelf = pInterface->Attri_CLOSE[0];
        ret = appctrl_close_set_attr(switchAttrSelf);
        CTRL_FMT_DEBUG("设置合闸继电器1属性%s%s\n", 0 == switchAttrSelf ? "脉冲式" : "保持式", 0 == ret ? "成功" : "失败");
    }
    if(TRUE == bswitchSelf || 0 != memcmp(&cur_output, &if_outputSelf, sizeof(CTRL_SWITCH_STATUS_T)))
    {
        CTRL_SWITCH_STATUS_T        flag;
        flag.num = cur_output.num;
        for(i = 0; i < cur_output.num && i < OOP_MAX_TURN; i++)
        {
            if(TRUE == bswitchSelf || cur_output.status[i] != if_outputSelf.status[i])
            {
                flag.status[i] = TRUE;
            }
            else
            {
                flag.status[i] = FALSE;
            }
        }
        memcpy(&if_outputSelf, &cur_output, sizeof(CTRL_SWITCH_STATUS_T));
        appctrl_switch_close_out(&if_outputSelf, &flag);
        bswitchSelf = FALSE;
    }

#if defined PRODUCT_ECU || defined PRODUCT_ECU_G
    uint8                zAlarm = 1;   //默认不动作

    if(FALSE != appctrl_alarm_zcu1_get())
    {
        zAlarm = 0;
        appctrl_alarm_zcu1(FALSE);

        CTRL_FMT_DEBUG("告警继电器动作\n");
    }

    appctrl_switch_alarm_out(zAlarm);
#endif

    return 0;
}

/**********************************************************************
* @name      : appctrl_get_switch_input_zcu1
* @brief     ：控制回路接入检测(转变1)
* @param[in] ：SWITCH_INTERFACE_T *pInterface 继电器动作
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-7-28
* @Update    :
**********************************************************************/
int appctrl_get_switch_input_zcu1(uint8 acm1, uint8 acm2, SWITCH_INTERFACE_T *pInterface)
{
    uint8    inputOpen[OOP_MAX_TURN] = {0};  //跳闸接入
    uint8    inputClose[OOP_MAX_TURN] = {0}; //合闸接入
    
    appctrl_IIC_getinput(inputOpen, inputClose, OOP_MAX_TURN);

    memcpy(pInterface->INPUT_STATUS, inputOpen, OOP_MAX_TURN);
    memcpy(pInterface->CLOSE_INPUT, inputClose, OOP_MAX_TURN);
    /* 更新F2050200接线状态 */
    appctrl_set_oopswitch_input(acm1, acm2, inputOpen, inputClose);

    return 0;
}

/**********************************************************************
* @name      : appctrl_get_switch_input
* @brief     ：控制回路接入检测
* @param[in] ：CTRL_RS232_T *pDevPara  设备句柄
               uint8 acm2   设备2
               SWITCH_INTERFACE_T *pInterface 继电器动作
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-29
* @Update    :
**********************************************************************/
int appctrl_get_switch_input(uint8 acm1, uint8 subport1, uint8 acm2, uint8 subport2, SWITCH_INTERFACE_T *pInterface)
{
    static uint8    inputM[OOP_MAX_TURN];  //模组接入
    static uint8    inputB[OOP_MAX_TURN];  //本体接入
    uint8           num = 0;
    uint8           i = 0;

    ctrl_update_devname(acm1, subport1, &g_CtrlDev1);
    ctrl_update_devname(acm2, subport2, &g_CtrlDev2);

    if(NULL != g_CtrlDev1.pDev)
    {
#if defined PRODUCT_ECU || defined PRODUCT_ECU_G || defined PRODUCT_SEU
        num = appctrl_module_input_get(&g_CtrlDev1, inputM, g_TurnPort);
#endif
#ifdef PRODUCT_ZCU_3
        num = appctrl_get_zcu_3_input(&g_CtrlDev1, inputM, g_TurnPort);
#endif
    }
    else
    {
        num = g_TurnPort;
        for(i = 0; i < g_TurnPort; i++)
        {
            inputM[i] = 1;
        }
    }
    
    if(NULL != g_CtrlDev2.pDev)
    {
#if defined PRODUCT_ECU || defined PRODUCT_ECU_G || defined PRODUCT_SEU
        num = appctrl_module_input_get(&g_CtrlDev2, inputM + g_TurnPort, MAX(OOP_MAX_TURN - g_TurnPort, 0));
#endif
    }
    else
    {
        num = MAX(OOP_MAX_TURN, g_TurnPort + g_TurnPort);
        for(i = g_TurnPort; i < num; i++)
        {
            inputM[i] = 1;
        }
    }

    num = appctrl_close_input_get(inputB, g_TurnPort);
    if(0 == num)
    {
        for(i = g_TurnPort; i < num; i++)
        {
            inputB[i] = 1;
        }
    }

    memcpy(pInterface->INPUT_STATUS, inputM, OOP_MAX_TURN);
    memcpy(pInterface->CLOSE_INPUT, inputB, OOP_MAX_TURN);
    /* 更新F2050200接线状态 */
    appctrl_set_oopswitch_input(acm1, acm2, inputM, inputB);

    return 0;
}

/**********************************************************************
* @name      : appctrl_init_alarm_switch
* @brief     ：继电器工作
* @param[in] ：uint8 acm1   设备1
               uint8 acm2   设备2
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
int appctrl_init_alarm_switch(uint8 acm, uint8 subport)
{
    static uint8          sAlarmFirst = TRUE;
    int                   ret = 0;
    
    ctrl_update_devname(acm, subport, &g_CtrlAlarm);

    if(0 != acm && NULL == g_CtrlAlarm.pDev)
    {
        if(0 != ctrl_uart_Init(&g_CtrlAlarm))
        {
            CTRL_FMT_DEBUG("ctrl_uart_Init %s failed\n", g_CtrlAlarm.deviceName);
        }
        usleep(1000);
    }
    
    if(NULL != g_CtrlAlarm.pDev)
    {
        if(TRUE == sAlarmFirst)
        {
            /* 属性设置 */
            ret = appctrl_switch_set_attri(&g_CtrlAlarm, 0);
            CTRL_FMT_DEBUG("设置告警继电器属性%s%s\n", "脉冲式", 0 == ret ? "成功" : "失败");
        }
        sAlarmFirst = FALSE;
    }
    else
    {
        sAlarmFirst = TRUE;
    }
    
    return 0;
}

/**********************************************************************
* @name      : appctrl_set_switch
* @brief     ：继电器工作
* @param[in] ：uint8 acm1   设备1
               uint8 acm2   设备2
               SWITCH_INTERFACE_T *pInterface 继电器动作
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-11-23
* @Update    :
**********************************************************************/
void appctrl_alarm_out()
{
    CTRL_SWITCH_STATUS_T alarm_oput;
    int ret = 0;
    
    if(g_CtrlAlarm.pDev)
    {
        memset(&alarm_oput, 0, sizeof(CTRL_SWITCH_STATUS_T));
        alarm_oput.num = 1;
        alarm_oput.status[0] = 0;
        //告警继电器和语音保持同步
        ret = appctrl_switch_status_out(&g_CtrlAlarm, &alarm_oput);
        if(1 == ret)
        {
            hal_device_release((HW_DEVICE *)g_CtrlAlarm.pDev);
            g_CtrlAlarm.pDev = NULL;
        }
    }
}

/**********************************************************************
* @name      : appctrl_pcstop_alarm
* @brief     ：根据功控类型清除告警状态
* @param[in] ：uint8 pctype    功控类型
               uint8 grp       总加组序号
               uint8 turn      轮次
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
void appctrl_pcstop_alarm(uint8 pckind,uint8 grp,uint8 turn)
{
    switch(pckind)
    {
        case CTRL_DOWN_TYPE:    //-下浮控-
            if(g_BrakeBuf[turn].bDownAState[grp] == TURN_ON_ALARM)
            {
                g_BrakeBuf[turn].bDownAState[grp] = TURN_OFF_ALARM;    //-下浮控取消告警状态-
                g_BrakeBuf[turn].bPowerAState[grp] = TURN_OFF_ALARM;
                //g_BrakeBuf[turn].bCtrlCs = CalCtrlCS(turn);    //-计算校验和-
            }
            break;
        case CTRL_STOP_TYPE:    //-营业报停控-
            if(g_BrakeBuf[turn].bStopAState[grp] == TURN_ON_ALARM)
            {
                g_BrakeBuf[turn].bStopAState[grp] = TURN_OFF_ALARM;    //-营业控取消告警状态-
                g_BrakeBuf[turn].bPowerAState[grp] = TURN_OFF_ALARM;
                //g_BrakeBuf[turn].bCtrlCs = CalCtrlCS(turn);    //-计算校验和-
            }
            break;
        case CTRL_REST_TYPE:    //-厂休控-
            if(g_BrakeBuf[turn].bRestAState[grp] == TURN_ON_ALARM)
            {
                g_BrakeBuf[turn].bRestAState[grp] = TURN_OFF_ALARM;    //-厂休控取消告警状态-
                g_BrakeBuf[turn].bPowerAState[grp] = TURN_OFF_ALARM;
                //g_BrakeBuf[turn].bCtrlCs = CalCtrlCS(turn);    //-计算校验和-
            }
            break;
        case CTRL_PERIOD_TYPE:    //-时段控-
            if(g_BrakeBuf[turn].bSegAState[grp] == TURN_ON_ALARM)
            {
                g_BrakeBuf[turn].bSegAState[grp] = TURN_OFF_ALARM;    //-时段控取消告警状态-
                g_BrakeBuf[turn].bPowerAState[grp] = TURN_OFF_ALARM;
                //g_BrakeBuf[turn].bCtrlCs = CalCtrlCS(turn);    //-计算校验和-
            }
            break;
        default:
            break;
    }
}

/**********************************************************************
* @name      : appctrl_pcclose_break
* @brief     ：根据功控类型清除跳闸状态
* @param[in] ：uint8 pctype    功控类型
               uint8 grp       总加组序号
               uint8 turn      轮次
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
void appctrl_pcclose_break(uint8 pctype, uint8 grp, uint8 turn)
{
    switch(pctype)
    {
        case CTRL_DOWN_TYPE:
            if(g_BrakeBuf[turn].bDownSState[grp] == TURN_ON_STATE)
            {
                g_BrakeBuf[turn].bDownSState[grp] = TURN_OFF_STATE;  //-下浮控取消跳闸状态-
                g_BrakeBuf[turn].bPowerSState[grp] = TURN_OFF_STATE;    //-功控取消跳闸状态-
                //bBrakeBuf[turn].bCtrlCs = CalCtrlCS(turn); //-计算校验和-
            }
            break;
        case CTRL_STOP_TYPE:
            if(g_BrakeBuf[turn].bStopSState[grp] == TURN_ON_STATE)
            {
                g_BrakeBuf[turn].bStopSState[grp] = TURN_OFF_STATE;    //-营业控取消跳闸状态-
                g_BrakeBuf[turn].bPowerSState[grp] = TURN_OFF_STATE;    //-功控取消跳闸状态-
                //bBrakeBuf[turn].bCtrlCs = CalCtrlCS(turn); //-计算校验和-
            }
            break;
        case CTRL_REST_TYPE:
            if(g_BrakeBuf[turn].bRestSState[grp] == TURN_ON_STATE)
            {
                g_BrakeBuf[turn].bRestSState[grp] = TURN_OFF_STATE;  //-厂休控取消跳闸状态-
                g_BrakeBuf[turn].bPowerSState[grp] = TURN_OFF_STATE;    //-功控取消跳闸状态-
                //bBrakeBuf[turn].bCtrlCs = CalCtrlCS(turn); //-计算校验和-
            }
            break;
        case CTRL_PERIOD_TYPE:
            if (g_BrakeBuf[turn].bSegSState[grp] == TURN_ON_STATE)
            {
                g_BrakeBuf[turn].bSegSState[grp] = TURN_OFF_STATE;   //-时段控取消告警状态-
                g_BrakeBuf[turn].bPowerSState[grp] = TURN_OFF_STATE;    //-功控取消跳闸状态-
                //bBrakeBuf[turn].bCtrlCs = CalCtrlCS(turn); //-计算校验和-
            }
            break;
        default:
            break;
    }
}

/**********************************************************************
* @name      : appctrl_get_switch_status
* @brief     ：获取继电器控制状态
* @param[in] ：uint8 bigType   大类 CTRL_TYPE_E
               uint8 type      类型 CTRL_STORE_TYPE_E
               uint8 grp       总加组序号
               uint8 turn      轮次
* @param[out]：
* @return    ：TURN_OFF/TURN_ON
* @Create    : 王津剑
* @Date      ：2020-4-7
* @Update    :
**********************************************************************/
uint8 appctrl_get_switch_status(uint8 bigType, uint8 type, uint8 turn, uint8 grp)
{
    uint8 status = TURN_OFF_STATE;

    if(turn > OOP_MAX_TURN || grp > OOP_MAX_TG || CTRL_NULL_TYPE == bigType || CTRL_NULL_STORE == type)
    {
        return TURN_OFF;
    }
    switch(bigType)
    {
        case CTRL_DOWN_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bDownAState[grp] : g_BrakeBuf[turn].bDownSState[grp];
            break;
        }
        case CTRL_STOP_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bStopAState[grp] : g_BrakeBuf[turn].bStopSState[grp];
            break;
        }
        case CTRL_REST_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bRestAState[grp] : g_BrakeBuf[turn].bRestSState[grp];
            break;
        }
        case CTRL_PERIOD_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bSegAState[grp] : g_BrakeBuf[turn].bSegSState[grp];
            break;
        }
        case CTRL_BUY_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bBuyENAState[grp] : g_BrakeBuf[turn].bBuyENSState[grp];
            break;
        }
        case CTRL_MONTH_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bMonthENAState[grp] : g_BrakeBuf[turn].bMonthENSState[grp];
            break;
        }
        case CTRL_REMOTE_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bRemoteAState : g_BrakeBuf[turn].bRemoteSState;
            break;
        }
        case CTRL_CLOSE_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bRemoteCAState : g_BrakeBuf[turn].bRemoteCState;
            break;
        }
        case CTRL_POWER_TYPE:
        {
            status = CTRL_ALARM_STORE == type ? g_BrakeBuf[turn].bPowerAState[grp] : g_BrakeBuf[turn].bPowerSState[grp];
            break;
        }
        default: return TURN_OFF;
    }
    if(TURN_ON_STATE == status)
    {
        return TURN_ON;
    }
    return TURN_OFF;
}

/**********************************************************************
* @name      : appctrl_set_switch_status
* @brief     ：设置继电器控制状态
* @param[in] ：uint8 bigType   大类 CTRL_TYPE_E
               uint8 type      类型 CTRL_STORE_TYPE_E
               uint8 grp       总加组序号
               uint8 turn      轮次
               uint8 status    状态 TURN_OFF/TURN_ON
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-9
* @Update    :
**********************************************************************/
void appctrl_set_switch_status(uint8 bigType, uint8 type, uint8 turn, uint8 grp, uint8 status)
{
    uint8 ss = 0;

    if(turn > OOP_MAX_TURN || grp > OOP_MAX_TG || CTRL_NULL_TYPE == bigType || CTRL_NULL_STORE == type)
    {
        return;
    }

    if(CTRL_ALARM_STORE == type)
    {
        if(TURN_ON == status)
        {
            ss = TURN_ON_ALARM;
        }
        else
        {
            ss = TURN_OFF_ALARM;
        }
    }
    else
    {
        if(TURN_ON == status)
        {
            ss = TURN_ON_STATE;
        }
        else
        {
            ss = TURN_OFF_STATE;
        }
    }

    /* 保电状态下 新的拉闸不执行新的拉闸 */
    //if(TRUE == appctrl_protect_status() && CTRL_STATUS_STORE == type && TURN_ON_STATE == ss)
    //{
    //    return;
    //}

    switch(bigType)
    {
        case CTRL_DOWN_TYPE:
        {
            if(CTRL_ALARM_STORE == type)
            {
                g_BrakeBuf[turn].bDownAState[grp] = ss;
            }
            else
            {
                g_BrakeBuf[turn].bDownSState[grp] = ss;
            }
            break;
        }
        case CTRL_STOP_TYPE:
        {
            if(CTRL_ALARM_STORE == type)
            {
                g_BrakeBuf[turn].bStopAState[grp] = ss;
            }
            else
            {
                g_BrakeBuf[turn].bStopSState[grp] = ss;
            }
            break;
        }
        case CTRL_REST_TYPE:
        {
            if(CTRL_ALARM_STORE == type)
            {
                g_BrakeBuf[turn].bRestAState[grp] = ss;
            }
            else
            {
                g_BrakeBuf[turn].bRestSState[grp] = ss;
            }
            break;
        }
        case CTRL_PERIOD_TYPE:
        {
            if(CTRL_ALARM_STORE == type)
            {
                g_BrakeBuf[turn].bSegAState[grp] = ss;
            }
            else
            {
                g_BrakeBuf[turn].bSegSState[grp] = ss;
            }
            break;
        }
        case CTRL_BUY_TYPE:
        {
            if(CTRL_ALARM_STORE == type)
            {
                g_BrakeBuf[turn].bBuyENAState[grp] = ss;
            }
            else
            {
                g_BrakeBuf[turn].bBuyENSState[grp] = ss;
            }
            break;
        }
        case CTRL_MONTH_TYPE:
        {
            if(CTRL_ALARM_STORE == type)
            {
                g_BrakeBuf[turn].bMonthENAState[grp] = ss;
            }
            else
            {
                g_BrakeBuf[turn].bMonthENSState[grp] = ss;
            }
            break;
        }
        case CTRL_REMOTE_TYPE:
        {
            if(CTRL_ALARM_STORE == type)
            {
                g_BrakeBuf[turn].bRemoteAState = ss;
            }
            else
            {
                g_BrakeBuf[turn].bRemoteSState = ss;
            }
            break;
        }
        case CTRL_CLOSE_TYPE:
        {
            if(CTRL_STATUS_STORE == type)
            {
                g_BrakeBuf[turn].bRemoteCState = ss;
            }
            else
            {
                g_BrakeBuf[turn].bRemoteCAState = ss;
            }
            break;
        }
        case CTRL_POWER_TYPE:
        {
            if(CTRL_ALARM_STORE == type)
            {
                g_BrakeBuf[turn].bPowerAState[grp] = ss;
            }
            else
            {
                g_BrakeBuf[turn].bPowerSState[grp] = ss;
            }
        }
    }

}

/**********************************************************************
* @name      : appctrl_control_state
* @brief     ：获取每个轮次及总的闸控制状态
* @param[in] ：uint8 acm1   设备1
               uint8 acm2   设备2
* @param[out]：SWITCH_INTERFACE_T *pInterface  继电器模块控制状态
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
void appctrl_control_state(uint8 acm1, uint8 acm2, SWITCH_INTERFACE_T *pInterface)
{
    static UN_BRAKE_DATA_T bLastBrakeBuf[OOP_MAX_TURN];//-上次开关状态-
    uint8  turn, grp;    //-轮次，总加组-
    uint8  bType = CTRL_NULL_TYPE;    //-合闸告警类型-
    uint8  group = 0;
    uint8  bfgTurnOn[OOP_MAX_TURN] = {0};
    uint8  turnStatus = 0;
    uint8  closeStatus = 0;
    uint8  alarmstatus = 1;

    /* 轮次投入状态刷新 */
    CTRL_ARG_ALL_T  *ptCtrlArg = NULL; //控制模块基本参数
    CTRL_SUBGROUP_T *ptSubGroups = NULL; //总加组配置及其数据
    CTRL_STATIC_T   *ptCtrlStatic = NULL;//控制统计
    uint8            pwStatus;
    uint8            ecStatus;
    /* 获取OAD配置 */
    appctrl_get_argcopy(&ptCtrlArg, &ptSubGroups, &ptCtrlStatic);

    for(turn = 0; turn < OOP_MAX_TURN; turn++)
    {
        pInterface->Ctrl_STATUS[turn] = 0;
        for(grp = 0; grp < OOP_MAX_TG; grp++)
        {
            pwStatus = ptSubGroups[grp].ctrlPwTurn.nValue;
            ecStatus = ptSubGroups[grp].ctrlEcTurn.nValue;
            if((pwStatus & (0x01 << turn)) || (ecStatus & (0x01 << turn)))
            {
                pInterface->Ctrl_STATUS[turn] = 1;
            }
        }
    }

    memset(bfgTurnOn, 0x0, OOP_MAX_TURN);
    for(turn = 0; turn < g_TurnMax; turn++)
    {
        for(grp = 0;grp < OOP_MAX_TG; grp++)
        {
            if(TURN_ON_STATE == g_BrakeBuf[turn].bBuyENSState[grp]    ||
               TURN_ON_STATE == g_BrakeBuf[turn].bMonthENSState[grp]  ||
               TURN_ON_STATE == g_BrakeBuf[turn].bPowerSState[grp]    ||
               TURN_ON_STATE == g_BrakeBuf[turn].bRemoteSState)
            {    //-某轮次任意一种控制处于跳闸状态则将该轮次总加组状态置跳闸状态-
                g_BrakeBuf[turn].bSwitchState[grp]  = SWITCH_OPEN;
                bfgTurnOn[turn] = 1;
            }
            else
            {
                if(TURN_OFF_STATE == bLastBrakeBuf[turn].bRemoteSState &&
                   TURN_OFF_STATE == bLastBrakeBuf[turn].bPowerSState[grp])
                {    //-上次轮次遥控功控闸状态为合闸状态-
                    if(TURN_OFF_STATE == bLastBrakeBuf[turn].bBuyENSState[grp]    &&
                       TURN_ON_STATE  == bLastBrakeBuf[turn].bMonthENSState[grp])
                    {    //-只有当拉闸单独由月电控引起时，才进行实际的语音合闸告警-
                        bType = CTRL_MONTH_TYPE;
                        group = grp;
                    }
                    else if(TURN_OFF_STATE == bLastBrakeBuf[turn].bMonthENSState[grp]  &&
                            TURN_ON_STATE  == bLastBrakeBuf[turn].bBuyENSState[grp])
                    {    //-只有当拉闸单独由购电控引起时，才进行实际的语音合闸告警-
                        bType = CTRL_BUY_TYPE;
                        group = grp;
                    }
                    else if(TURN_OFF_STATE == bLastBrakeBuf[turn].bMonthENSState[grp] &&
                            TURN_OFF_STATE == bLastBrakeBuf[turn].bBuyENSState[grp])
                    {
                    }
                }
                else if(TURN_OFF_STATE == bLastBrakeBuf[turn].bRemoteSState       &&
                        TURN_OFF_STATE == bLastBrakeBuf[turn].bMonthENSState[grp] &&
                        TURN_OFF_STATE == bLastBrakeBuf[turn].bBuyENSState[grp])
                {  //-遥控和电控状态都不拉闸-
                    if(TURN_OFF_STATE == bLastBrakeBuf[turn].bSegSState[grp]  &&
                       TURN_OFF_STATE == bLastBrakeBuf[turn].bRestSState[grp] &&
                       TURN_OFF_STATE == bLastBrakeBuf[turn].bStopSState[grp] &&
                       TURN_ON_STATE  == bLastBrakeBuf[turn].bDownSState[grp])
                    {    //-只有当拉闸单独由功率下浮控引起时，才填写相应的合闸告警结构-
                        bType = CTRL_DOWN_TYPE;
                        group = grp;
                    }
                    else if(TURN_OFF_STATE == bLastBrakeBuf[turn].bSegSState[grp]  &&
                            TURN_OFF_STATE == bLastBrakeBuf[turn].bRestSState[grp] &&
                            TURN_ON_STATE  == bLastBrakeBuf[turn].bStopSState[grp] &&
                            TURN_OFF_STATE == bLastBrakeBuf[turn].bDownSState[grp])
                    {    //-只有当拉闸单独由报停控引起时，才填写相应的合闸告警结构-
                        bType = CTRL_STOP_TYPE;
                        group = grp;
                    }
                    else if(TURN_OFF_STATE == bLastBrakeBuf[turn].bSegSState[grp]  &&
                            TURN_ON_STATE  == bLastBrakeBuf[turn].bRestSState[grp] &&
                            TURN_OFF_STATE == bLastBrakeBuf[turn].bStopSState[grp] &&
                            TURN_OFF_STATE == bLastBrakeBuf[turn].bDownSState[grp])
                    {    //-只有当拉闸单独由厂休控引起时，才填写相应的合闸告警结构-
                        bType = CTRL_REST_TYPE;
                        group = grp;
                    }
                    else if((bLastBrakeBuf[turn].bSegSState[grp] == TURN_ON_STATE)
                        &&(bLastBrakeBuf[turn].bRestSState[grp] == TURN_OFF_STATE)
                        &&(bLastBrakeBuf[turn].bStopSState[grp] == TURN_OFF_STATE)
                        &&(bLastBrakeBuf[turn].bDownSState[grp] == TURN_OFF_STATE) )
                    {    //-只有当拉闸单独由时段控引起时，才填写相应的合闸告警结构-
                        bType = CTRL_PERIOD_TYPE;
                        group = grp;
                    }
                }
                else if(TURN_OFF_STATE == bLastBrakeBuf[turn].bBuyENSState[grp]    &&
                        TURN_OFF_STATE == bLastBrakeBuf[turn].bMonthENSState[grp]  &&
                        TURN_OFF_STATE == bLastBrakeBuf[turn].bPowerSState[grp]    &&
                        TURN_ON_STATE  == bLastBrakeBuf[turn].bRemoteSState)
                {    //-只有当拉闸单独由遥控引起时，才填写相应的合闸告警结构-
                    bType = CTRL_REMOTE_TYPE;
                    group = grp;
                }
                g_BrakeBuf[turn].bSwitchState[grp] =  SWITCH_CLOSE;
            }
            //-保存状态-
            bLastBrakeBuf[turn].bBuyENSState[grp]    = g_BrakeBuf[turn].bBuyENSState[grp];
            bLastBrakeBuf[turn].bMonthENSState[grp]  = g_BrakeBuf[turn].bMonthENSState[grp];
            bLastBrakeBuf[turn].bSegSState[grp]      = g_BrakeBuf[turn].bSegSState[grp];
            bLastBrakeBuf[turn].bRestSState[grp]     = g_BrakeBuf[turn].bRestSState[grp];
            bLastBrakeBuf[turn].bStopSState[grp]     = g_BrakeBuf[turn].bStopSState[grp];
            bLastBrakeBuf[turn].bDownSState[grp]     = g_BrakeBuf[turn].bDownSState[grp];
            bLastBrakeBuf[turn].bPowerSState[grp]    = g_BrakeBuf[turn].bPowerSState[grp];
            bLastBrakeBuf[turn].bRemoteSState        = g_BrakeBuf[turn].bRemoteSState;
            bLastBrakeBuf[turn].bRemoteCState        = g_BrakeBuf[turn].bRemoteCState;

            if(TURN_ON_ALARM == g_BrakeBuf[turn].bBuyENAState[grp]    ||
               TURN_ON_ALARM == g_BrakeBuf[turn].bMonthENAState[grp]  ||
               TURN_ON_ALARM == g_BrakeBuf[turn].bPowerAState[grp]    ||
               TURN_ON_ALARM == g_BrakeBuf[turn].bRemoteAState)
            {
                alarmstatus = 0;
            }
        }
        switch(bType)
        {
            case CTRL_DOWN_TYPE:
            case CTRL_STOP_TYPE:
            case CTRL_REST_TYPE:
            case CTRL_PERIOD_TYPE:
            {
                /* 发送合闸告警 */
                appctrl_alarm_data(bType, group, 3, EMAUDIO_INDEX_16, appctrl_power_guishow);
                break;
            }
            case CTRL_BUY_TYPE:
            case CTRL_MONTH_TYPE:
            {
                /* 发送合闸告警 */
                appctrl_alarm_data(bType, group, 3, EMAUDIO_INDEX_16, appctrl_elec_guishow);
                break;
            }
            case CTRL_REMOTE_TYPE:
            {
                /* 发送合闸告警 */
                appctrl_alarm_data(bType, group, 3, EMAUDIO_INDEX_16, appctrl_remote_guishow);
                break;
            }
            default:
                break;
        }
        if(bfgTurnOn[turn])
        {
            g_SwitchState[turn] = SWITCH_OPEN;    //-该轮次总闸状态跳闸状态-
            pInterface->TURN_CTRL[turn] = 0;
        }
        else
        {
            g_SwitchState[turn] = SWITCH_CLOSE;    //-该轮次总闸状态合闸状态-
            pInterface->TURN_CTRL[turn] = 1;
        }
    }

    pInterface->POWER_CTRL_LIGHT = appctrl_check_power_status();
    pInterface->ELE_CTRL_LIGHT = appctrl_check_elec_status();
    if(TRUE == appctrl_protect_status())
    {
        /* 保电 状态下 全部合闸 */
        pInterface->PROTECT_LIGHT = 1;
        for(turn = 0; turn < g_TurnMax; turn++)
        {
            pInterface->TURN_CTRL[turn] = 1;
            pInterface->TURN_REDLIGHT[turn] = 0;
            pInterface->TURN_BLUELIGHT[turn] = 1;
        }
    }
    else
    {
        pInterface->PROTECT_LIGHT = 0;
        for(turn = 0; turn < g_TurnMax; turn++)
        {
            if(SWITCH_OPEN == g_SwitchState[turn])//-该轮次总闸状态跳闸状态-
            {
                pInterface->TURN_CTRL[turn] = 0;
                pInterface->TURN_REDLIGHT[turn] = 1;
                pInterface->TURN_BLUELIGHT[turn] = 0;
                turnStatus |= (0x01 << turn);
            }
            else//-该轮次总闸状态合闸状态-
            {  
                pInterface->TURN_CTRL[turn] = 1;
                pInterface->TURN_REDLIGHT[turn] = 0;
                pInterface->TURN_BLUELIGHT[turn] = 1;
            }
        }

        pInterface->ALARM_OUTPUT = alarmstatus;
    }

    /* 合闸继电器(保电情况下可以直接合闸) */
    for(turn = 0; turn < g_TurnMax; turn++)
    {
        if(TURN_ON_STATE == g_BrakeBuf[turn].bRemoteCState)//-该轮次总闸状态跳闸状态-
        {
            pInterface->CLOSE_CTRL[turn] = 0;
        }
        else//-该轮次总闸状态合闸状态-
        {  
            pInterface->CLOSE_CTRL[turn] = 1;
            closeStatus |= (0x01 << turn);
        }
    }
    
    /* 设置继电器状态 */
    appctrl_set_oopswitch(acm1, acm2, turnStatus, closeStatus);
}

extern void *app_leddev_get();

/**********************************************************************
* @name      : appctrl_ctrl_led
* @brief     ：控制LED
* @param[in] ：uint8 bOpen   打开/关闭
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-13
* @Update    : 2020-4-30
**********************************************************************/
void appctrl_ctrl_led(LED_INDEX_E index, uint8 bOpen)
{
#ifndef PRODUCT_SEU
    LED_DEVICE_T* dev = (LED_DEVICE_T*)app_leddev_get();

    if(NULL == dev)
    {
        CTRL_FMT_DEBUG("LED dev is null\n");
        return;
    }
    if(FALSE == bOpen)
    {
        dev->led_state_set(dev, index, LED_OFF, COM_MODE, NULL, 0);
    }
    else
    {
        dev->led_state_set(dev, index, LED_ON, COM_MODE, NULL, 0);
    }
#endif
}

/**********************************************************************
* @name      : appctrl_ctrl_ZCUS_turn_led
* @brief     ：控制LED
* @param[in] ：uint8 bOpen   打开/关闭
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-13
* @Update    : 2020-4-30
**********************************************************************/
void appctrl_ctrl_ZCUS_turn_led(uint8 index, uint8 bOpen)
{
    LED_DEVICE_T* dev = (LED_DEVICE_T*)app_leddev_get();

    if(NULL == dev)
    {
        CTRL_FMT_DEBUG("LED dev is null\n");
        return;
    }
    if(FALSE == bOpen)
    {
        dev->led_state_set(dev, index, LED_OFF, COM_MODE, NULL, 0);
    }
    else
    {
        dev->led_state_set(dev, index, LED_ON, COM_MODE, NULL, 0);
    }
}

/**********************************************************************
* @name      : appctrl_ctrl_audio
* @brief     ：控制LED
* @param[in] ：uint8 ctrlType   控制类型
               uint8 audiotype  类型
* @param[out]：
* @return    ：AUDIO_INDEX_E
* @Create    : 王津剑
* @Date      ：2020-4-13
* @Update    :
**********************************************************************/
uint8 app_audio_index(uint8 bigType, uint8 audiotype)
{
    return 0;  //关闭音源输出
}

extern void appmain_buzzer_status(int dbClient, uint8 status);
/**********************************************************************
* @name      : appctrl_ctrl_buzzer
* @brief     ：控制蜂鸣器
* @param[in] ：int dbClient  数据中心id
               uint8 bOpen   是否打开
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
void appctrl_ctrl_buzzer(int dbClient, uint8 bOpen)
{
    static int s_ctrlclient = 0;

    if(dbClient > 0)
    {
        s_ctrlclient = dbClient;
    }
    appmain_buzzer_status(s_ctrlclient, bOpen);
}

/**********************************************************************
* @name      : appctrl_status_write
* @brief     ：控制中间状态写入私有数据
* @param[in] ：uint8 bDay  过日
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-5-7
* @Update    :
**********************************************************************/
void appctrl_status_write(uint8 bDay)
{
    int ret = 0;
    static uint8 s_BrakeBuf[OOP_MAX_TURN * sizeof(UN_BRAKE_DATA_T)] = {0};

    if(0 == memcmp(s_BrakeBuf, g_BrakeBuf, OOP_MAX_TURN * sizeof(UN_BRAKE_DATA_T)) && FALSE == bDay)
    {
        return;
    }
    memcpy(s_BrakeBuf, g_BrakeBuf, OOP_MAX_TURN * sizeof(UN_BRAKE_DATA_T));
    ret = appctrl_write_ctrl(CTRL_STATUS_TYPE, (uint8 *)g_BrakeBuf, OOP_MAX_TURN * sizeof(UN_BRAKE_DATA_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("保存遥控中间数据错误, %d\n", ret);
    }
}

