/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控mqtt
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "mapmsg.h"
#include "mapmsg_split.h"
#include "usb_upg.h"

uint8  g_fileData[USB_UPPACK_SIZE];

#if DESC("串口操作",1)

/**********************************************************************
* @name      : uart_lock_set
* @brief     ：设置串口锁
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_lock_set(int fd, int type)
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

/**********************************************************************
* @name      : uart_speed_set
* @brief     ：设置串口波特率
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_speed_set(int fd, int speed)
{
    int i; 
    int status; 
    int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300};
    int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300};

    struct termios Opt;     /*Opt中存储波特率数据结构*/
    tcgetattr(fd, &Opt);    /*获取termios结构*/
    
    for ( i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++) 
    { 
        if (speed == name_arr[i])   /*查找支持的波特率*/
        { 
            cfsetispeed(&Opt, speed_arr[i]); /*设置输入波特?*/
            cfsetospeed(&Opt, speed_arr[i]); /*设置输出波特?*/
            status = tcsetattr(fd, TCSANOW, &Opt);  /*设置串口新的termios结构*/
            if  (status != 0) 
            {                                 
                return -1;     
            }    
            
            tcflush(fd,TCIOFLUSH); /*清除串口的输入输出队列中的数?*/
            return 1;                     
        }  
    }
    return -1;
}

/**********************************************************************
* @name      : uart_param_set
* @brief     ：设置串口参数
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_param_set(const int fd, USB_UART_PARAM_T *param)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) /*读取串口termios结构*/
    { 
        return(FALSE);  
    }
    
    /*设置为不加工模式, 不回?*/
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag = IGNBRK;
    options.c_cflag |= CLOCAL | CREAD;
    
    options.c_cflag &= ~CSIZE;  /*清除原字符大*/
    switch (param->dataBits) /*设置数据位数*/
    {   
        case 7:             
            options.c_cflag |= CS7;  /*设置7位数据位?*/
            break;
        case 8:     
            options.c_cflag |= CS8;  /*设置8位数据位?*/
            break;   
        default:    
            return (FALSE);  
    }
    
    switch (param->parity) 
    {   
        case UART_PARITY_NONE: 
            options.c_cflag &= ~PARENB;   /* 输入奇偶校验无效 */
            break;
              
        case UART_PARITY_ODD:                                                               
            options.c_cflag |= (PARODD | PARENB);   /*设置为奇效验*/  
            break;
              
        case UART_PARITY_EVEN:  
            options.c_cflag &= ~PARODD;
            options.c_cflag |= PARENB;  /* 使奇偶校验有?*/ 
            break;
            
        case UART_PARITY_SPACE: /*space校验*/
            options.c_cflag &= ~CSTOPB;
            break;
            
        default:   
            return (FALSE);  
    } 
     
    /* 设置停止?*/ 
    switch (param->stopBits)
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
        
    tcflush(fd,TCIFLUSH);                   /* 清输入队?*/
    options.c_cc[VTIME] = 100;      /* 设置超时10 seconds*/   
    options.c_cc[VMIN] = 0;             /* 更新设置，使马上生效 */
    
    options.c_iflag &= ~(IXON|IXOFF|IXANY); /*关软件流?*/
    
    if (param->flowCtrl == 1)
        options.c_cflag |= CRTSCTS; /*开硬件流控*/
    else
        options.c_cflag &= ~CRTSCTS;/*关硬件流?*/

    options.c_iflag &= ~IGNCR;  /*不忽略收到的CR字符*/
    options.c_iflag &= ~ICRNL;  

    if (tcsetattr(fd,TCSANOW,&options) != 0)  /* 对串口进行参数设??*/
    {  
        return (FALSE);  
    }
    return (TRUE);
}

/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_open(USB_UART_VDEV_T *pUart)
{
    if(pUart == NULL)
    {
        return ERR_PNULL;
    }

    if(pUart->fd < 0)
    {  
        pUart->fd = open(pUart->desc, O_RDWR|O_NOCTTY|O_NONBLOCK ); /*打开串口*/
        if(pUart->fd >= 0)
        {
            if (uart_lock_set(pUart->fd, F_WRLCK) == -1)    /*锁串?*/
            {
                UPDATE_FMT_DEBUG("uart_lock_set %s failed.\n", pUart->desc);  
                close(pUart->fd);
                pUart->fd = -1;
                return ERR_NORMAL;
            }
        }
    }
        
    if (pUart->fd < 0)     /*若打开串口失败，返??1，并通知应用?*/
    {           
        UPDATE_FMT_DEBUG("open %s failed, %s \n", pUart->desc, strerror(errno));  
        return -1;  
    }

    if (uart_speed_set(pUart->fd, pUart->param.baudRate) == -1)  /*设置串口波特?*/
    {
        UPDATE_FMT_DEBUG("uart_speed_set %s failed.\n", pUart->desc);  
        close(pUart->fd);
        pUart->fd = -1;
        return ERR_INVAL;
    }           

    if (uart_param_set(pUart->fd, &pUart->param) == FALSE) /*设置串口参数 数据??校验 停止?*/ 
    {
        UPDATE_FMT_DEBUG("uart_param_set %s failed.\n", pUart->desc);  
        close(pUart->fd);
        pUart->fd = -1;
        return ERR_INVAL;
    }

    UPDATE_FMT_DEBUG("open %s success.\n", pUart->desc); 
    
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_close(USB_UART_VDEV_T *pUart)
{
    int ret = 0;
    
    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(pUart->fd >= 0)
    {
        uart_lock_set(pUart->fd, F_UNLCK);
        ret = close(pUart->fd);
        if(ret == ERR_OK)
        {
            pUart->fd = -1;
            
            UPDATE_FMT_DEBUG("close %s success.\n", pUart->desc);  
        }
        else
        {
            UPDATE_FMT_DEBUG("close %s failed, %s \n", pUart->desc, strerror(errno));  
        }
    }

    return ret;
}

/**********************************************************************
* @name      : uart_write
* @brief     ：向串口写数据
* @param[in] ：uart    串口句柄
* @param[in] ：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_write(USB_UART_VDEV_T *pUart, uint8* buf, int len)
{
//    int write_len= 0;
    int send_len = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if (pUart->fd < 0)  
    {  
        UPDATE_FMT_DEBUG("fd %d is invalid\n", pUart->fd);
        return ERR_INVALID;  
    }

//    do 
//    {
//        if((write_len = write(pUart->fd, buf, len)) <= 0)
//        {
//            if(write_len < 0 && errno == EINTR)
//            {
//                write_len = 0;
//            }
//            else
//            {
//                return ERR_NORMAL;
//            }
//        }
//
//        send_len += write_len;
//
//    }while(send_len < len);

    send_len = write(pUart->fd, buf, len);    
    if(send_len == len)
    {
        clock_gettime(CLOCK_MONOTONIC, &pUart->sendTm);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

/**********************************************************************
* @name      : uart_read
* @brief     ：从串口读数据
* @param[in] ：uart    串口句柄
* @param[out]：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：读到的数据长度
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_read(USB_UART_VDEV_T *pUart, uint8* buf, int len)
{
    int fd = pUart->fd;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = pUart->param.baudRate;
    int firstCnt = pUart->frametimeout*1000;//微秒计算
    int packetCnt = pUart->bytetimeout*1000;//微秒计算
    
    if(NULL == pUart)
    {
        return ERR_PNULL;
    }

    if(fd < 0)
    {
        return ERR_INVAL;
    }
        
    /*串口接收数据超时时间按20个字节间隔时间来计算，300bps 发送一个字节需要36667us
      一个字节包括起始位、停止位、校验位、8位数据 共11位，则每串耗时11/baud秒*/
    if((baud%300 != 0 ) || baud == 0)
    {   
        delay = 11000000/9600;
    }
    else
    {   
        delay = 11000000/baud;
    }

    firstCnt /= delay;
    packetCnt /= delay;

    while(1)
    {
        //等待串口可读
        FD_ZERO(&set);
        FD_SET(fd, &set);
        timeout.tv_sec = 0;
        timeout.tv_usec = (baud < 19200)?30000:1000;
        
        do 
        {
            n = select(fd+1, &set, NULL, NULL, &timeout); //超时返回0
        }while(n < 0);
  
        if(n > 0)  
        {
            recv_len = read(fd, buf+data_len, len-data_len);
        }
        else
        {
            recv_len = 0;
        }

        // 收到字节后延时一个字节的时间再去接收
        if(recv_len > 0)   
        {
            data_len += recv_len;
            count = 0;
            usleep(delay);
        }
        else
        {
            // 读取超时
            if((data_len == 0) && (count > firstCnt)) 
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                usleep(delay);
                return data_len;
            }
            
            //后续字节读取超时或长度已经超过缓冲区
            if((data_len != 0) && ((count > packetCnt) || (data_len > len))) 
            {
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                usleep(delay);
                return (data_len < 0) ? 0 : data_len;                    
            }
        }
  
        count++;
    }

    return data_len;
}

/**********************************************************************
* @name      : uart_transmit
* @brief     ：串口数据传输
* @param[in] ：void *pDev                   串口句柄
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
* @param[out]：
* @return    ：接收到数据的长度
* @Create    : 贺宁
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
int uart_transmit(USB_UART_VDEV_T *pUart, uint8* pSend, int sendLen, uint8* pRecv, int maxRecv)
{
    int    ret = 0;

    ret = uart_write(pUart, pSend, sendLen);
    if(ret != 0)
    {
        return 0;
    }
    
    return uart_read(pUart, pRecv, maxRecv);
}

/**********************************************************************
* @name      : uart_reopen
* @brief     ：重新打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_reopen(USB_UART_VDEV_T *pUart)
{
    int ret = 0;
    
    ret = uart_close(pUart);
    if(ret == 0)
    {
       sleep_no_interrupt(2);
       ret = uart_open(pUart); 
    }

    return ret;
}

#endif


#if DESC("协处理器升级处理",1)
/**********************************************************************
* @name      : usb_update_parse
* @brief     ：模组接口协议解析
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
int usb_update_parse(uint8 *recvBuf, uint16 buflen)
{
    int            ret = 0;
    uint16         offset = 0;
    MAPMSG_INFO_T *pMsgInfo = NULL;
    uint8         *temp = NULL;
    uint16         dt = 0;
    int            tempret = 0;
    uint32 sOffset = 0;
    
    //找到帧起始位
    while(offset < buflen)
    {
        if(FRAMESTART == recvBuf[offset])
        {
            tempret = mapmsg_split(recvBuf + offset, buflen - offset, &pMsgInfo);
            if(tempret != 0)
            {
                UPDATE_BUF_DEBUG(recvBuf + offset, buflen - offset, "mapmsg split failed, tempret(%d)\n", tempret);
            }
            break;
        }
        offset++;
    }

    if(pMsgInfo == NULL)
    {
        UPDATE_BUF_DEBUG(temp, sOffset, "check null pointer.\n");
        return ERR_PNULL;
    }

    //功能单一，不考虑多帧
    if(MAPMSG_CTR_MSG == pMsgInfo->cmt.cmd && pMsgInfo->len > 4)
    {
        temp = pMsgInfo->apdu;

        if((MAPMSG_APDU_SetResponse == temp[0])&&(1 == temp[3]))  //data [1]
        {
            sOffset += 2;

            memcpy_r(&dt, &temp[1], sizeof(uint16));
            sOffset += sizeof(uint16);

            if(MAPMSG_DT_COMM_FILETRAN == dt)
            {
                if(temp[sOffset] != 0)
                {
                    ret = ERR_NORMAL;
                }
            }
        }
    }
    
    FREE_S(pMsgInfo);
    
    return ret;
}

/**********************************************************************
* @name      : usb_make_frame
* @brief     ：模组接口协议组帧
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-10
* @Update    :
**********************************************************************/
uint16 usb_update_make(USB_FILE_TRANS_T *fileInfo, uint8 *sendBuf, uint16 buflen)
{
    uint8          msgbuf[512] = {0};
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    static uint8 index = 0;
    uint16 dt = 0x0203;
    //uint8 byteNum = 0;

    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index++;

    Temp[offset++] = MAPMSG_APDU_SetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);

    //文件标识
    Temp[offset++] = 0;

    //段序号
    mapmsg_memcpy_r(Temp + offset, &fileInfo->nBlockId, sizeof(uint16));
    offset += sizeof(uint32);

    //总段数
    mapmsg_memcpy_r(Temp + offset, &fileInfo->nBlockNum, sizeof(uint16));
    offset += sizeof(uint32);

    //文件数据
    offset += mapmsg_set_len_offset(fileInfo->nLengh, Temp + offset);
    
    memcpy(Temp + offset, fileInfo->fileData, fileInfo->nLengh);
    offset += fileInfo->nLengh;

    pMsgInfo->len = offset;

    return mapmsg_package(pMsgInfo, sendBuf, buflen);
}

/**********************************************************************
* @name      : usb_update_deal
* @brief     ：升级包发送
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-09-07
* @Update    :
**********************************************************************/
int usb_update_deal(void *pPublic, void *pPrivate)
{
    USB_UPDATE_DATA_T *ptData = (USB_UPDATE_DATA_T *)pPrivate;
    USB_FILE_TRANS_T *pFileInfo = ptData->fileInfo;
    USB_UART_VDEV_T *pUart = ptData->pUart;
    uint8 sendbuf[256] = {0};
    uint16 sendLen = 0;
    int recvLen = 0;
    uint8 recvBuf[256] = {0};
    int ret = 0;
     
    lseek(ptData->fd, pFileInfo->nBlockId*USB_UPPACK_SIZE, SEEK_SET);

    if(pFileInfo->nBlockId == (pFileInfo->nBlockNum-1))
    {
        pFileInfo->nLengh = pFileInfo->nEndLengh;
    }
    else
    {
        pFileInfo->nLengh = USB_UPPACK_SIZE;
    }

    read(ptData->fd, pFileInfo->fileData, pFileInfo->nLengh);

    sendLen = usb_update_make(pFileInfo, sendbuf, 256);
    UPDATE_BUF_DEBUG(sendbuf, sendLen, "发送升级帧，总帧数 %d, 序号 %d :\n", pFileInfo->nBlockNum , pFileInfo->nBlockId);

    recvLen = uart_transmit(pUart, sendbuf, sendLen, recvBuf, sizeof(recvBuf));
    if(recvLen != 0)
    {
        ret = usb_update_parse(recvBuf, recvLen);
        if(ret != 0)
        {
            UPDATE_BUF_DEBUG(recvBuf, recvLen, "usb_update_parse failed,ret(%d).\n", ret);
            return ret;
        }
    }

    pFileInfo->nBlockId++;

    return 0;
}

/**********************************************************************
* @name      : usb_update_init
* @brief     ：升级初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-09-07
* @Update    :
**********************************************************************/
int usb_upg_init(void *pPublic, void **ppPrivate)
{
    USB_UPDATE_DATA_T *ptData = NULL;
    
    ptData = (USB_UPDATE_DATA_T *)malloc(sizeof(USB_UPDATE_DATA_T));
    if(NULL == ptData)
    {
        UPDATE_FMT_DEBUG("usb_update_init malloc failed\n");
        return -1;
    }
    
    memset(ptData, 0, sizeof(USB_UPDATE_DATA_T));

    ptData->pUart = (USB_UART_VDEV_T *)malloc(sizeof(USB_UART_VDEV_T));
    if(NULL == ptData->pUart)
    {
        UPDATE_FMT_DEBUG("usb_update_init uart malloc failed\n");
        return -1;
    }    

    ptData->fileInfo = (USB_FILE_TRANS_T *)malloc(sizeof(USB_FILE_TRANS_T));
    if(NULL == ptData->fileInfo)
    {
        UPDATE_FMT_DEBUG("usb_update_init fileInfo malloc failed\n");
        return -1;
    }

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : usb_update_proc
* @brief     ：回路巡检升级线程
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2021-09-07
* @Update    :
**********************************************************************/
int usb_upg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    USB_UPDATE_DATA_T *ptData = (USB_UPDATE_DATA_T *)pPrivate;
    USB_FILE_TRANS_T *pFileInfo = ptData->fileInfo;
    USB_UART_VDEV_T *pUart = ptData->pUart;
    int ret = 0;
    int size = 0;
    
    switch(ptData->status)
    {
        case USB_UPDATE_IDLE:
        {
            //文件存在则开始升级
            if(0 == access(USB_UPDATE_FILE, F_OK))
            {
                sleep(1);
                size = file_size_get(USB_UPDATE_FILE);
                UPDATE_FMT_DEBUG("检测到升级文件 %s，大小 %d B, 开始对协处理器升级.\n", USB_UPDATE_FILE, size);
                
                memset(pFileInfo, 0, sizeof(USB_FILE_TRANS_T));
                pFileInfo->fileData = g_fileData;

                pFileInfo->nEndLengh = size % USB_UPPACK_SIZE;
                pFileInfo->nBlockNum = size/USB_UPPACK_SIZE;
                
                if(pFileInfo->nEndLengh)
                {
                    pFileInfo->nBlockNum += 1;
                }

                ptData->fd = -1;
                ptData->fd = open(USB_UPDATE_FILE, O_RDONLY);
                if(ptData->fd < 0)
                {
                    UPDATE_FMT_DEBUG("copr file open failed, %s.\n", strerror(errno));
                    sleep(1);
                    return 10000000;
                }

                pUart->desc = "/dev/ttyCOPR";
                pUart->bytetimeout = 12;
                pUart->frametimeout = 100;
                pUart->param.baudRate = 9600;
                pUart->param.dataBits = 8;    
                pUart->param.stopBits = 1;    
                pUart->param.parity = UART_PARITY_EVEN;
                pUart->fd = -1;

                ret = uart_open(pUart);
                if(ret != 0)
                {
                    UPDATE_FMT_DEBUG("copr uart open failed. ret %d\n", ret);
                    sleep(1);
                    return ret;
                }
                
                ptData->status = USB_UPDATE_RUNING;
            }
        }break;

        case USB_UPDATE_RUNING:
        {            
            if(pFileInfo->nBlockId >= pFileInfo->nBlockNum)
            {            
                remove(USB_UPDATE_FILE);
                UPDATE_FMT_DEBUG("协处理器升级完成.\n");
                ptData->status = USB_UPDATE_IDLE;
            }
            else
            {
                usb_update_deal(pPublic, pPrivate);
                ptData->status = USB_UPDATE_RUNING;
            }
        }break;

        default:break;
    }
     
    return 10000;
}

#endif

