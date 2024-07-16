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
#include "task_msg.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "task_base.h"
#include "task_id.h"
#include "msg_id.h"

#ifndef PRODUCT_SCU
#include "hal.h"
#include "uart.h"
#include "can.h"
#endif

#include "cJSON.h"
#include "uart_comm.h"
#include "uart_mqtt.h"

#include "uart_api.h"
#include "uart_service.h"

#ifdef PRODUCT_SCU
UART_INFO_T g_uart_info[]=
{
    /*端口名称     端口号 端口名        启用别名 端口号别名            端口别名        串口名     设备名     设备参数      描述符      */
    {"COMM-1", 1, "RS485-1", FALSE, 1, "RS485-1", {"com1", "", "/dev/ttyRS0",  COM_UART, {0}, -1}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-2", 2, "RS485-2", FALSE, 2, "RS485-2", {"com2", "", "/dev/ttyRS1",  COM_UART, {0}, -1}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-3", 3, "RS485-3", FALSE, 5, "RS232-1", {"com3", "", "/dev/ttyRS2",  COM_UART, {0}, -1}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-4", 4, "RS485-4", FALSE, 6, "RS232-2", {"com4", "", "/dev/ttyRS3",  COM_UART, {0}, -1}, UART_MODE_UNLL, "", 0, {""}},
};
#else
UART_INFO_T g_uart_info[]=
{
    /*端口名称     端口号 端口名        启用别名 端口号别名            端口别名        串口名     设备名     设备参数      描述符      */
    {"COMM-1", 0xf2010201, "RS485-1", FALSE, 1, "RS485-1", {"uart1", UART_RS4851_DEVICE_ID_STR, "", COM_UART, {0}, 0}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-2", 0xf2010202, "RS485-2", FALSE, 2, "RS485-2", {"uart2", UART_RS4852_DEVICE_ID_STR, "", COM_UART, {0}, 0}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-3", 0xf2210201, "CAN-1", FALSE, 2, "CAN-1", {"can1", CAN0_DEVICE_ID_STR, "/dev/ttyCAN0", COM_CAN, {0}, 0}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-4", 0xf2210202, "CAN-2", FALSE, 2, "CAN-2", {"can2", CAN1_DEVICE_ID_STR, "/dev/ttyCAN1", COM_CAN, {0}, 0}, UART_MODE_UNLL, "", 0, {""}},
    
    {"COMM-5", 0xf2010203, "RS485-3", FALSE, 1, "RS485-3", {"uart3", "", "", COM_UART, {0}, 0}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-6", 0xf2010204, "RS485-4", FALSE, 2, "RS485-4", {"uart4", "", "", COM_UART, {0}, 0}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-7", 0xf2010205, "RS485-5", FALSE, 3, "RS485-5", {"uart5", "", "", COM_UART, {0}, 0}, UART_MODE_UNLL, "", 0, {""}},
    {"COMM-8", 0xf2010206, "RS485-6", FALSE, 4, "RS485-6", {"uart6", "", "", COM_UART, {0}, 0}, UART_MODE_UNLL, "", 0, {""}},
};
#endif

int g_uart_info_num = sizeof(g_uart_info)/sizeof(UART_INFO_T);

uint32 g_uart_baud[] = {300, 600, 1200, 2400, 4800, 7200, 9600, 
                        19200, 38400, 57600, 115200, 230400, 460800,
                        10000, 25000, 50000, 125000, 250000, 500000,
                        1000000};
uint8 g_uart_databit[] = {5, 6, 7, 8};
uint8 g_uart_stopbit[] = {1, 2};
UART_PARITY_E g_uart_parity[] = {UART_PARITY_NONE, UART_PARITY_ODD, UART_PARITY_EVEN};
UART_FLOWCTRL_E g_uart_flowctrl[] = {UART_FLOWCTRL_NONE, UART_FLOWCTRL_HARD, UART_FLOWCTRL_SOFT};
char *g_uart_parity_desc[] = {"none", "odd", "even"};
extern uint8      g_AppName[APPMSG_NAME_LEN];
extern char  g_uart_datafile[256];
extern int uart_parity_tostring(UART_PARITY_E          parity ,char *paritystr);
extern int uart_mode_tostring(UART_MODE_E         mode ,char *modestr);
UART_MODINFO_T g_uart_modinfo = {0};

#if DESC("队列操作",1)

/**********************************************************************
* @name      : uart_pque_init
* @brief     ：优先队列初始化
* @param[in] ：pq  优先队列
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-07-08
* @Update    :
**********************************************************************/
int uart_pque_init(UART_PQUE_T *pq)
{
    memset(pq, 0, sizeof(UART_PQUE_T));
    
    pq->depth = UART_QUEUE_DEPTH;
    pq->queue[0].length = UART_QUEUE_BUFER;

    pthread_mutex_init(&pq->lock, NULL);

    return 0;
}

/**********************************************************************
* @name      : uart_pque_cmp
* @brief     ：优先队列比较
* @param[in] ：void
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-07-08
* @Update    :
**********************************************************************/
int uart_pque_cmp(UART_QUEUE_T *baseQ, UART_QUEUE_T *newsQ)
{
    int basePrio = baseQ->prio;
    int newsPrio = newsQ->prio;
   
    return (basePrio-newsPrio);
}

/**********************************************************************
* @name      : uart_pque_push
* @brief     ：优先队列插入
* @param[in] ：void
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-07-08
* @Update    :
**********************************************************************/
int uart_pque_push(UART_PQUE_T *pq, UART_QUEUE_T *inData)
{
    int ret = ERR_OK;
    int i = 0;

    pthread_mutex_lock(&pq->lock);

    if(pq->size >= pq->depth)
    {
        ret = ERR_O_RANGE;
    }
    
    for(i = ++pq->size; uart_pque_cmp(&pq->queue[i/2], inData) > 0; i /= 2)
    {
        memcpy(&pq->queue[i], &pq->queue[i/2], sizeof(UART_QUEUE_T));
    }

    memcpy(&pq->queue[i], inData, sizeof(UART_QUEUE_T));
    
    pthread_mutex_unlock(&pq->lock);
    
    return ret;
}

/**********************************************************************
* @name      : uart_pque_pop
* @brief     ：优先队列弹出
* @param[in] ：void
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-07-08
* @Update    :
**********************************************************************/
int uart_pque_pop(UART_PQUE_T *pq, UART_QUEUE_T *outData)
{
    int ret = ERR_OK;
    int i = 0;
    int child = 0;
    UART_QUEUE_T lastQ = {0};

    pthread_mutex_lock(&pq->lock);
        
    if(pq->size == 0)
    {
        pthread_mutex_unlock(&pq->lock);
        return ERR_NORESOURCE;
    }

    memcpy(outData, &pq->queue[1], sizeof(UART_QUEUE_T));

    memcpy(&lastQ, &pq->queue[pq->size--], sizeof(UART_QUEUE_T));
    
    for(i = 1; i*2 <= pq->size; i = child)
    {
        child = 2 * i;
        
        if((child != pq->size) && (uart_pque_cmp(&pq->queue[child+1], &pq->queue[child]) < 0))
        {
            child++;
        }

        if(uart_pque_cmp(&lastQ, &pq->queue[child]) > 0)
        {
            memcpy(&pq->queue[i], &pq->queue[child], sizeof(UART_QUEUE_T));
        }
        else
        {
            break;
        }
    }

    memcpy(&pq->queue[i], &lastQ, sizeof(UART_QUEUE_T));

    pthread_mutex_unlock(&pq->lock);
    
    return ret;
}

/**********************************************************************
* @name      : uart_fifo_init
* @brief     ：优先队列初始化
* @param[in] ：fifo  优先队列
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-07-08
* @Update    :
**********************************************************************/
int uart_fifo_init(UART_FIFO_T *fifo)
{
    memset(fifo, 0, sizeof(UART_FIFO_T));
    
    fifo->depth = UART_QUEUE_DEPTH;

    pthread_mutex_init(&fifo->lock, NULL);

    return 0;
}

/**********************************************************************
* @name      : uart_fifo_push
* @brief     ：FIFO队列插入
* @param[in] ：void
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-07-08
* @Update    :
**********************************************************************/
int uart_fifo_push(UART_FIFO_T *fifo, UART_QUEUE_T *inData)
{
    int ret = ERR_O_RANGE;

     pthread_mutex_lock(&fifo->lock);
    
    /*判队列中是否还有空的位置*/
     if (fifo->queue[fifo->tail].length == 0)
     {
         memcpy(&fifo->queue[fifo->tail], inData, sizeof(UART_QUEUE_T));
         fifo->tail = (fifo->tail+1) % fifo->depth;
         ret = ERR_OK;
     }
     
     pthread_mutex_unlock(&fifo->lock);
     
    return ret;
}

/**********************************************************************
* @name      : uart_fifo_pop
* @brief     ：FIFO队列弹出
* @param[in] ：void
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-07-08
* @Update    :
**********************************************************************/
int uart_fifo_pop(UART_FIFO_T *fifo, UART_QUEUE_T *outData)
{
    int ret = ERR_OK;

    pthread_mutex_lock(&fifo->lock);

    /*若队列中有数据，则取到buf中，解开锁*/
    if ((fifo->head != fifo->tail)||((fifo->head == fifo->tail)&&(fifo->queue[fifo->head].length != 0)))
    {
        memcpy(outData, &fifo->queue[fifo->head], sizeof(UART_QUEUE_T));
        memset(&fifo->queue[fifo->head], 0, sizeof(UART_QUEUE_T));     /*清数据*/
        fifo->head = (fifo->head+1) % fifo->depth; /*队列头向后移动一位*/
    }
    else
    {
        ret = ERR_NORESOURCE;
    }
    
    pthread_mutex_unlock(&fifo->lock);
    
    return ret;
}

#endif

#if DESC("串口基础",1)
#ifdef PRODUCT_SCU
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
    int name_arr[] = {UART_BAUD_115200, UART_BAUD_38400,  UART_BAUD_19200,  UART_BAUD_9600,  UART_BAUD_4800,  UART_BAUD_2400,  UART_BAUD_1200, UART_BAUD_600, UART_BAUD_300};

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
int uart_param_set(const int fd, UART_PARAM_T *param)
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

    if(uart_speed_set(fd, param->baudRate) < 0)
    {
        return (FALSE);  
    }
    
    return (TRUE);
}

/**********************************************************************
* @name      : uart_param_get
* @brief     ：读取串口参数
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_param_get(const int fd, UART_PARAM_T *param)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) /*读取串口termios结构*/
    { 
        return(FALSE);  
    }

    return TRUE;
}

/**********************************************************************
* @name      : uart_only_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_only_open(UART_DEVDES_T *pUart)
{
    int tmpfd = -1;
    
    if((pUart == NULL) || (pUart->devname == NULL))
    {
        return ERR_PNULL;
    }

    tmpfd = open(pUart->devname, O_RDWR|O_NOCTTY|O_NONBLOCK ); /*打开串口*/
    if(tmpfd>= 0)
    {
        if (uart_lock_set(tmpfd, F_WRLCK) == -1)    /*锁串?*/
        {
            UART_FMT_DEBUG(pUart->comdesc, "uart_lock_set %s false!\n", pUart->devname);  
            close(tmpfd);
            tmpfd = -1;
            pUart->fd = tmpfd;
            return ERR_NORMAL;
        }
    }
        
    if (tmpfd < 0)     /*若打开串口失败，返??1，并通知应用?*/
    {           
        UART_FMT_DEBUG(pUart->comdesc, "open %s false!\n", pUart->devname);  
        pUart->fd = tmpfd;
        return ERR_NORMAL;  
    }        

    pUart->fd = tmpfd;
    
    return ERR_OK;
}

/**********************************************************************
* @name      : com_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_open(UART_DEVDES_T *pUart)
{
    int tmpfd = -1;
    
    if((pUart == NULL) || (pUart->devname == NULL))
    {
        return ERR_PNULL;
    }

    tmpfd = open(pUart->devname, O_RDWR|O_NOCTTY|O_NONBLOCK ); /*打开串口*/
    if(tmpfd>= 0)
    {
        if (uart_lock_set(tmpfd, F_WRLCK) == -1)    /*锁串?*/
        {
            UART_FMT_DEBUG(pUart->comdesc, "uart_lock_set %s false!\n", pUart->devname);  
            close(tmpfd);
            tmpfd = -1;
            pUart->fd = tmpfd;
            return ERR_NORMAL;
        }
    }
        
    if (tmpfd < 0)     /*若打开串口失败，返??1，并通知应用?*/
    {           
        UART_FMT_DEBUG(pUart->comdesc, "open %s false!\n", pUart->devname);  
        pUart->fd = tmpfd;
        return ERR_NORMAL;  
    }        

    if (uart_param_set(tmpfd, &pUart->param) == FALSE) /*设置串口参数 数据??校验 停止?*/ 
    {
        UART_FMT_DEBUG(pUart->comdesc, "uart_param_set %s false!\n", pUart->devname);  
        close(tmpfd);
        tmpfd = -1;
        pUart->fd = tmpfd;
        pUart->status = UART_STATUS_PARAM_ERR;
        return ERR_INVAL;
    }
   
    pUart->fd = tmpfd;

    UART_FMT_DEBUG(pUart->comdesc, "open %s success: baudrate[%d]-databit[%d]-parity[%s]-stopbit[%d].\n", 
        pUart->devname, g_uart_baud[pUart->param.baudRate], pUart->param.dataBits, g_uart_parity_desc[pUart->param.parity], pUart->param.stopBits);  
    
    return ERR_OK;
}

/**********************************************************************
* @name      : com_open
* @brief     ：打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_close(UART_DEVDES_T *pUart)
{    
    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(pUart->fd >= 0)
    {
        uart_lock_set(pUart->fd, F_UNLCK);
        close(pUart->fd);
        pUart->fd = -1;
    }
    
    return ERR_OK;
}

/**********************************************************************
* @name      : com_write
* @brief     ：向串口写数据
* @param[in] ：uart    串口句柄
* @param[in] ：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_write(UART_DEVDES_T *pUart, uint8* buf, int len)
{
//    int write_len= 0;
    int send_len = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if (pUart->fd < 0)  
    {  
        UART_FMT_DEBUG(pUart->comdesc, "fd %d is invalid\n", pUart->fd);
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
        UART_BUF_DEBUG(pUart->comdesc, buf, len, "%s send:", pUart->devname);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {        
        UART_FMT_DEBUG(pUart->comdesc, "%s send failed.\n", pUart->devname);
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

/**********************************************************************
* @name      : com_read
* @brief     ：从串口读数据
* @param[in] ：uart    串口句柄
* @param[out]：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：读到的数据长度
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_read(UART_DEVDES_T *pUart,     uint16 frameTimeout, uint16 octetTimeout, uint8* buf, int len)
{
    int fd = pUart->fd;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = 0;
    int frameus = frameTimeout*1000;//微秒计算
    int octetus = octetTimeout*1000;//微秒计算
    
    if(fd < 0)
    {
        return ERR_INVAL;
    }

    UART_BAUD_TO_STRING(baud, pUart->param.baudRate);

    //等待串口可读
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = frameus;
    
    do 
    {
        n = select(fd+1, &set, NULL, NULL, &timeout); //超时返回0
    }while(n < 0);

    if(n == 0)
    {
        pUart->status = UART_STATUS_TIME_OUT; 
        //UART_FMT_DEBUG("%s frame timeout: %d ms\n", pUart->devname, frameTimeout);
        return 0;
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
    
    while(1)
    {
        recv_len = read(fd, buf+data_len, len-data_len);

        // 收到字节后延时一个字节的时间再去接收
        if(recv_len > 0)   
        {
            data_len += recv_len;
            count = 0;
        }
        else
        { 
            //字节读取超时
            if(count > octetus)
            {
//              pUart->status = UART_STATUS_TIME_OUT;
//              UART_FMT_DEBUG("%s octet timeout: limit-%d, curr-%d\n", pUart->devname, octetus, count);
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                break;                    
            }
        }

        //长度已经超过缓冲区
        if(data_len > len)
        {
            clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
            pUart->status = UART_STATUS_TASK_FULL;
            UART_FMT_DEBUG(pUart->comdesc, "%s buffer full: limit-%d, curr-%d\n", pUart->devname, len, data_len);
            pUart->recvCnt++;
            break;                    
        }

        usleep(delay);

        count += delay;
    }

    data_len = (data_len < 0) ? 0 : data_len;

    UART_BUF_DEBUG(pUart->comdesc,  buf, data_len, "%s recv:", pUart->devname);
    
    return data_len;
}

#else

/**********************************************************************
* @name      : uart_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int uart_open(UART_DEVDES_T *pUart)
{
    int ret = ERR_OK;
    UART_DEVICE_T* dev = NULL;
    
    if((pUart == NULL) || (pUart->halname == NULL))
    {
        return ERR_PNULL;
    }

    dev = (UART_DEVICE_T*)hal_device_get(pUart->halname);
    if(NULL == dev)
    {
        UART_FMT_DEBUG(pUart->comdesc, "hal_device_get failed. dev(%s)\n", pUart->halname);
        return ERR_PNULL;    
    }

    ret = dev->uart_get_real_dev_name(dev, (uint8*)pUart->devname);
    if(ret <= 0)
    {
        UART_FMT_DEBUG(pUart->comdesc, "uart_get_real_dev_name failed. dev(%s), ret(%d)\n", pUart->halname, ret);
        hal_device_release((HW_DEVICE *)dev);
        return ERR_OPTION;    
    }
    
    // 打开设备
    ret = dev->uart_param_set(dev, g_uart_baud[pUart->param.baudRate], pUart->param.dataBits, pUart->param.stopBits, pUart->param.parity);
    if(ret != 0)
    {
        UART_FMT_DEBUG(pUart->comdesc, "uart_param_set failed. dev(%s), ret(%d)\n", pUart->devname, ret);
        hal_device_release((HW_DEVICE *)dev);
        return ERR_INVAL;
    }

    pUart->fd = (int)dev;

    UART_FMT_DEBUG(pUart->comdesc, "open %s success: baudrate[%d]-databit[%d]-parity[%s]-stopbit[%d].\n", 
        pUart->devname, g_uart_baud[pUart->param.baudRate], pUart->param.dataBits, g_uart_parity_desc[pUart->param.parity], pUart->param.stopBits);  
    
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
int uart_close(UART_DEVDES_T *pUart)
{
    int ret = 0;
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;

    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(dev != NULL)
    {
        ret = hal_device_release((HW_DEVICE *)dev);
        if(ret == 0)
        {
            UART_FMT_DEBUG(pUart->comdesc, "%s close success.\n", pUart->devname);
        }
        
        dev = NULL;
    }

    pUart->fd = (int)dev;
    
    return ERR_OK;
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
int uart_write(UART_DEVDES_T *pUart, uint8* buf, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;
    int ret = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if(dev == NULL)  
    {  
        UART_FMT_DEBUG(pUart->comdesc, "dev is invalid\n");
        return ERR_INVALID;  
    }

    ret = dev->uart_data_send(dev, buf, len);
    if(ret == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &pUart->sendTm);
        UART_BUF_DEBUG(pUart->comdesc, buf, len, "%s send:", pUart->devname);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {        
        UART_BUF_DEBUG(pUart->comdesc, buf, len, "%s send failed, ret %d.\n", pUart->devname, ret);
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

//算下两个时间的差值 微秒
int time_check(struct timespec *time1,struct timespec *time2)
{
    int tmp =0;
    if(time2->tv_sec == time1->tv_sec)
    {
        tmp = (time2->tv_nsec -time1->tv_nsec)/1000;
        return tmp;
    }
    if(time2->tv_sec > time1->tv_sec)
    {
        tmp = (time2->tv_sec-time1->tv_sec)*1000+time2->tv_nsec/1000-time1->tv_nsec/1000;
        return tmp;
    }
    return tmp;
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
int uart_read(UART_DEVDES_T *pUart,     uint16 frameTimeout, uint16 octetTimeout, uint8* buf, int len)
{
    UART_DEVICE_T* dev = (UART_DEVICE_T* )pUart->fd;
    int fd = -1;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = 0;
    int frameus = frameTimeout*1000;//微秒计算
    int octetus = octetTimeout*1000;//微秒计算
    struct timespec timenow;
    struct timespec lastrecv;
    if(dev == NULL)
    {
        return ERR_INVAL;
    }

    fd = dev->uart_get_handle(dev);
    if(fd < 0)
    {
        return ERR_INVAL;
    }

    UART_BAUD_TO_STRING(baud, pUart->param.baudRate);

    //等待串口可读
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = frameus;
    //UART_FMT_DEBUG(pUart->comdesc,"uart_read begin \n");
    do 
    {
        n = select(fd+1, &set, NULL, NULL, &timeout); //超时返回0
    }while(n < 0);

    if(n == 0)
    {
        pUart->status = UART_STATUS_TIME_OUT; 
        //UART_FMT_DEBUG("%s frame timeout: %d ms\n", pUart->devname, frameTimeout);
        return 0;
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
    //UART_FMT_DEBUG(pUart->comdesc,"uart_data_recv \n");
    while(1)
    {
        recv_len = dev->uart_data_recv(dev, buf+data_len, len-data_len);

        // 收到字节后延时一个字节的时间再去接收
        if(recv_len > 0)   
        {
            UART_BUF_DEBUG(pUart->comdesc,  buf+data_len, recv_len, "recv:");
            data_len += recv_len;
            count = 0;
            clock_gettime(CLOCK_MONOTONIC, &lastrecv);
        }
        else
        { 
            //用delay的count来计算时间不准
            clock_gettime(CLOCK_MONOTONIC, &timenow);
            //字节读取超时
            if((time_check(&lastrecv,&timenow) > octetus) || count > octetus)
            {
//              pUart->status = UART_STATUS_TIME_OUT;
//              UART_FMT_DEBUG("%s octet timeout: limit-%d, curr-%d\n", pUart->devname, octetus, count);
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                break;                    
            }
        }

        //长度已经超过缓冲区
        if(data_len > len)
        {
            clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
            pUart->status = UART_STATUS_TASK_FULL;
            UART_FMT_DEBUG(pUart->comdesc, "%s buffer full: limit-%d, curr-%d\n", pUart->devname, len, data_len);
            pUart->recvCnt++;
            break;                    
        }

        usleep(delay);

        count += delay;
    }

    data_len = (data_len < 0) ? 0 : data_len;
    if(data_len > len)
    {
        UART_FMT_DEBUG(pUart->comdesc, "%s buffer over: limit-%d, curr-%d\n", pUart->devname, len, data_len);
        data_len = len;
    }

    UART_BUF_DEBUG(pUart->comdesc,  buf, data_len, "%s recv:", pUart->devname);
    
    return data_len;
}

/**********************************************************************
* @name      : can_param_get
* @brief     ：读取串口参数
* @param[in] ：fd      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int can_param_get(const int pdev, UART_PARAM_T *param)
{
    CAN_DEVICE_T* dev = (CAN_DEVICE_T* )pdev;
    int fd = -1;

    if(dev == NULL)
    {
        return FALSE;
    }
    
    fd = dev->can_get_handle(dev);
    if(fd < 0)
    {
        return FALSE;
    }

    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) /*读取串口termios结构*/
    { 
        return(FALSE);  
    }

    return TRUE;
}

/**********************************************************************
* @name      : can_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int can_open(UART_DEVDES_T *pUart)
{
    int ret = ERR_OK;
    CAN_DEVICE_T* dev = NULL;
    
    if((pUart == NULL) || (pUart->halname == NULL))
    {
        return ERR_PNULL;
    }

    dev = (CAN_DEVICE_T*)hal_device_get(pUart->halname);
    if(NULL == dev)
    {
        UART_FMT_DEBUG(pUart->comdesc, "hal_device_get failed. dev(%s)\n", pUart->halname);
        return ERR_PNULL;    
    }

//    ret = dev->uart_get_real_dev_name(dev, (uint8*)pUart->devname);
//    if(ret <= 0)
//    {
//        UART_FMT_DEBUG(pUart->comdesc, "uart_get_real_dev_name failed. dev(%s), ret(%d)\n", pUart->halname, ret);
//        return ERR_OPTION;    
//    }
    
    // 打开设备
    ret = dev->can_param_set(dev, g_uart_baud[pUart->param.baudRate]);
    if(ret != 0)
    {
        UART_FMT_DEBUG(pUart->comdesc, "uart_param_set failed. dev(%s), ret(%d)\n", pUart->devname, ret);
        hal_device_release((HW_DEVICE *)dev);
        return ERR_INVAL;
    }

    pUart->fd = (int)dev;

    UART_FMT_DEBUG(pUart->comdesc, "open %s success: baudrate[%d].\n", pUart->devname, g_uart_baud[pUart->param.baudRate]);  
    
    return ERR_OK;
}

/**********************************************************************
* @name      : can_open
* @brief     ：打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int can_close(UART_DEVDES_T *pUart)
{
    CAN_DEVICE_T* dev = (CAN_DEVICE_T* )pUart->fd;

    if(NULL == pUart)
    {
        return ERR_PNULL;
    }
    
    if(dev != NULL)
    {
        hal_device_release((HW_DEVICE *)dev);

        dev = NULL;
    }

    pUart->fd = (int)dev;
    
    return ERR_OK;
}

/**********************************************************************
* @name      : can_write
* @brief     ：向串口写数据
* @param[in] ：uart    串口句柄
* @param[in] ：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int can_write(UART_DEVDES_T *pUart, uint8* buf, int len)
{
    CAN_DEVICE_T* dev = (CAN_DEVICE_T* )pUart->fd;
    int ret = 0;  

    if(pUart== NULL)
    {
        return ERR_PNULL;
    }
    
    if(dev == NULL)  
    {  
        UART_FMT_DEBUG(pUart->comdesc, "dev is invalid\n");
        return ERR_INVALID;  
    }

    ret = dev->can_data_send(dev, buf, len);    
    if(ret == 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &pUart->sendTm);
        UART_BUF_DEBUG(pUart->comdesc, buf, len, "%s send:", pUart->devname);
        pUart->sendCnt++;
        return ERR_OK;
    }
    else
    {        
        UART_FMT_DEBUG(pUart->comdesc, "%s send failed.\n", pUart->devname);
        return ERR_NORMAL;
    }
      
    return ERR_OK;
}

/**********************************************************************
* @name      : can_read
* @brief     ：从串口读数据
* @param[in] ：uart    串口句柄
* @param[out]：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：读到的数据长度
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int can_read(UART_DEVDES_T *pUart,     uint16 frameTimeout, uint16 octetTimeout, uint8* buf, int len)
{
    CAN_DEVICE_T* dev = (CAN_DEVICE_T* )pUart->fd;
    int fd = -1;
    int recv_len = 0;                                       
    int data_len = 0;                                       
    int count = 0;
    int delay = 0;
    fd_set set;
    struct timeval timeout;
    int n = 0;
    uint32 baud = 0;
    int frameus = frameTimeout*1000;//微秒计算
    int octetus = octetTimeout*1000;//微秒计算
    
    if(dev == NULL)
    {
        return ERR_INVAL;
    }

    fd = dev->can_get_handle(dev);
    if(fd < 0)
    {
        return ERR_INVAL;
    }

    UART_BAUD_TO_STRING(baud, pUart->param.baudRate);

    //等待串口可读
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = frameus;
    
    do 
    {
        n = select(fd+1, &set, NULL, NULL, &timeout); //超时返回0
    }while(n < 0);

    if(n == 0)
    {
        pUart->status = UART_STATUS_TIME_OUT; 
        //UART_FMT_DEBUG("%s frame timeout: %d ms\n", pUart->devname, frameTimeout);
        return 0;
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
    
    while(1)
    {
        recv_len = dev->can_data_recv(dev, buf+data_len, len-data_len);

        // 收到字节后延时一个字节的时间再去接收
        if(recv_len > 0)   
        {
            data_len += recv_len;
            count = 0;
        }
        else
        { 
            //字节读取超时
            if(count > octetus)
            {
//              pUart->status = UART_STATUS_TIME_OUT;
//              UART_FMT_DEBUG("%s octet timeout: limit-%d, curr-%d\n", pUart->devname, octetus, count);
                clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
                pUart->recvCnt++;
                break;                    
            }
        }

        //长度已经超过缓冲区
        if(data_len > len)
        {
            clock_gettime(CLOCK_MONOTONIC, &pUart->recvTm);
            pUart->status = UART_STATUS_TASK_FULL;
            UART_FMT_DEBUG(pUart->comdesc, "%s buffer full: limit-%d, curr-%d\n", pUart->devname, len, data_len);
            pUart->recvCnt++;
            break;                    
        }

        usleep(delay);

        count += delay;
    }

    data_len = (data_len < 0) ? 0 : data_len;
    if(data_len > len)
    {
        UART_FMT_DEBUG(pUart->comdesc, "%s buffer over: limit-%d, curr-%d\n", pUart->devname, len, data_len);
        data_len = len;
    }

    UART_BUF_DEBUG(pUart->comdesc,  buf, data_len, "%s recv:", pUart->devname);
    
    return data_len;
}

/**********************************************************************
* @name      : com_open
* @brief     ：打开串口,防重复打开
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_open(UART_DEVDES_T *pUart)
{
    int ret = ERR_OK;

    if(pUart->comType == COM_UART)
    {
        return uart_open(pUart);
    }
    else if(pUart->comType == COM_CAN)
    {
        return can_open(pUart);
    }

    return ret;
}

/**********************************************************************
* @name      : com_open
* @brief     ：打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_close(UART_DEVDES_T *pUart)
{
    int ret = ERR_OK;

    if(pUart->comType == COM_UART)
    {
        return uart_close(pUart);
    }
    else if(pUart->comType == COM_CAN)
    {
        return can_close(pUart);
    }

    return ret;
}

/**********************************************************************
* @name      : com_write
* @brief     ：向串口写数据
* @param[in] ：uart    串口句柄
* @param[in] ：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_write(UART_DEVDES_T *pUart, uint8* buf, int len)
{
    int ret = ERR_OK;

    if(pUart->comType == COM_UART)
    {
        return uart_write(pUart, buf, len);
    }
    else if(pUart->comType == COM_CAN)
    {
        return can_write(pUart, buf, len);
    }

    return ret;
}

/**********************************************************************
* @name      : com_read
* @brief     ：从串口读数据
* @param[in] ：uart    串口句柄
* @param[out]：buf     数据缓冲区
* @param[in] ：len     缓冲区长度
* @return    ：读到的数据长度
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_read(UART_DEVDES_T *pUart,     uint16 frameTimeout, uint16 octetTimeout, uint8* buf, int len)
{
    int ret = ERR_OK;

    pUart->status = UART_STATUS_OK;

    if(pUart->comType == COM_UART)
    {
        return uart_read(pUart, frameTimeout, octetTimeout, buf, len);
    }
    else if(pUart->comType == COM_CAN)
    {
        return can_read(pUart, frameTimeout, octetTimeout, buf, len);
    }

    return ret;
}

#endif
#endif

#if DESC("串口操作",1)

/**********************************************************************
* @name      : com_transmit
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
int com_transmit(UART_DEVDES_T *pUart, UART_TRANSMIT_T *pTransmit, int *lengh)
{
    int  ret = 0;
    int  len = 0;

    ret = com_write(pUart, pTransmit->sendBuf, pTransmit->sendLen);
    if(ret != 0)
    {
        *lengh = 0;
        return ret;
    }
    
    len = com_read(pUart, pTransmit->frameTimeout, pTransmit->octetTimeout, pTransmit->recvBuf, pTransmit->recvMax);
    if(len < 0)
    {
        *lengh = 0;
        return ERR_INVALID;
    }

    *lengh  = len;

    return ret;
}

/**********************************************************************
* @name      : com_forward
* @brief     ：串口转发数据
* @param[in] ：void *pDev                   串口句柄
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
* @param[out]：
* @return    ：错误码
* @Create    : 贺宁
* @Date      ：2020-3-13
* @Update    :
**********************************************************************/
int com_forward(UART_DEVDES_T *pUart, UART_TRANSMIT_T *pTransmit, int *lengh)
{
    int ret = 0;
    
    ret = com_open(pUart);
    if(ret != 0)
    {
        *lengh = 0;
        return ret;
    }

    ret = com_write(pUart, pTransmit->sendBuf, pTransmit->sendLen);
    if(ret != 0)
    {
        *lengh = 0;
        return ret;
    }
    
    ret = com_read(pUart, pTransmit->frameTimeout, pTransmit->octetTimeout, pTransmit->recvBuf, pTransmit->recvMax);
    if(ret < 0)
    {
        *lengh = 0;
        return ret;
    }

    *lengh  = ret;

    return com_close(pUart);
}

/**********************************************************************
* @name      : com_reopen
* @brief     ：重新打开串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_reopen(UART_DEVDES_T *pUart)
{
    com_close(pUart);

    if(0 == access(pUart->devname, F_OK))
    {
        UART_FMT_DEBUG(pUart->comdesc, "%s is exist\n", pUart->devname);
    }
    else
    {
        return ERR_NOTEXIST;
    }
    
    return com_open(pUart); ;
}

/**********************************************************************
* @name      : com_listen
* @brief     ：监听串口
* @param[in] ：uart      串口句柄
* @return    ：=0,正确，!=0,错误
* @Create    : 贺宁
* @Date      ：2021-02-01
* @Update    :
**********************************************************************/
int com_listen(UART_DEVDES_T *pUart, uint8 *buf, uint16 buflen)
{
    int recvLen = 0;
    
//    com_open(ptUart);
    recvLen = com_read(pUart, 1, 50, buf, buflen);
//    com_close(ptUart);

    return recvLen;
}

#endif

#if DESC("配置更新",1)

/**********************************************************************
* @name      : uart_param_cfg_update
* @brief     ：串口参数配置更新
* @param[in] ：UART_INFO_T *pUartInfo     串口参数
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_param_cfg_update(UART_INFO_T *pUartInfo)
{
    int ret = 0;
    cJSON  *json = NULL;
    cJSON  *jList = NULL;
    cJSON  *jArray = NULL;
    cJSON  *item = NULL; 
    int     i = 0;
    int    num = 0;
    UART_DEVDES_T *pUart = &pUartInfo->uart;;
    char desc[8] = {0};    
    uint32 baudRate = 0;

    json = cJSON_read(g_uart_datafile);
    if(!cJSON_IsObject(json))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no User config.\n");
        return ERR_PNULL;
    }

    jList = cJSON_GetObjectItem(json, "UART_CFG");
    if(!cJSON_IsArray(jList))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no list item.\n");
        goto end;
    }

    num = cJSON_GetArraySize(jList);

    //遍历配置数组
    for(i = 0; i < num; i++)
    {
        jArray = cJSON_GetArrayItem(jList, i);
        if(!cJSON_IsObject(jArray))
        {
            continue;
        }
        
        item = cJSON_GetObjectItem(jArray, "name");
        if(!cJSON_IsString(item))
        {
            continue;
        }

        if(strcmp(item->valuestring, pUartInfo->comname) == 0)
        {
            item = cJSON_GetObjectItem(jArray, "param");
            if(!cJSON_IsObject(item))
            {
                return ERR_NOTEXIST;
            }

            UART_BAUD_TO_STRING(baudRate, pUart->param.baudRate);

            cJSON_ReplaceItemInObject(item, "baudRate", cJSON_CreateNumber(baudRate));
            cJSON_ReplaceItemInObject(item, "byteSize", cJSON_CreateNumber(pUart->param.dataBits));
            
            memset(desc, '\0', sizeof(desc));
            uart_parity_tostring(pUart->param.parity, desc);
            cJSON_ReplaceItemInObject(item, "parity", cJSON_CreateString(desc));
            cJSON_ReplaceItemInObject(item, "stopBits", cJSON_CreateNumber(pUart->param.stopBits));            

            break;
        }
    }

    cJSON_write(json, g_uart_datafile);
end:    
    cJSON_Delete(json);  

    return ret;
}

/**********************************************************************
* @name      : com_listen_cfg_update
* @brief     ：监听列表配置更新
* @param[in] ：UART_INFO_T *pUartInfo     串口参数
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_listen_cfg_update(UART_INFO_T *pUartInfo)
{
    int ret = 0;
    cJSON  *json = NULL;
    cJSON  *jList = NULL;
    cJSON  *jArray = NULL;
    cJSON  *jAPPList = NULL;
    cJSON  *jAPPArray = NULL;        
    cJSON  *jnewArray = NULL;
    cJSON  *item = NULL; 
    int     i = 0, j = 0, k = 0;
    int    num = 0;
    uint32 temListNum = 0;
    
    json = cJSON_read(g_uart_datafile);
    if(!cJSON_IsObject(json))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no User config.\n");
        return ERR_PNULL;
    }

    jList = cJSON_GetObjectItem(json, "UART_CFG");
    if(!cJSON_IsArray(jList))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no list item.\n");
        goto end;
    }

    num = cJSON_GetArraySize(jList);

    //遍历配置数组
    for(i = 0; i < num; i++)
    {
        jArray = cJSON_GetArrayItem(jList, i);
        if(!cJSON_IsObject(jArray))
        {
            continue;
        }
        
        item = cJSON_GetObjectItem(jArray, "name");
        if(!cJSON_IsString(item))
        {
            continue;
        }

        if(strcmp(item->valuestring, pUartInfo->comname) == 0)
        {
            jAPPList = cJSON_GetObjectItem(jArray, "appList");
            if(cJSON_IsArray(jAPPList))
            {
                for(j = 0; j < pUartInfo->listNum; j++)
                {
                    temListNum = cJSON_GetArraySize(jAPPList);
                    for(k = 0; k < temListNum; k++)
                    {
                        jAPPArray = cJSON_GetArrayItem(jAPPList, k);
                        if(!cJSON_IsObject(jAPPArray))
                        {
                            break;
                        }
                
                        item = cJSON_GetObjectItem(jAPPArray, "name");
                        if(cJSON_IsString(item))
                        {
                            if(strcmp(pUartInfo->appList[j], item->valuestring) == 0)
                            {
                                break;
                            }
                        }
                    }

                    if(k == temListNum)
                    {
                        jnewArray = cJSON_CreateObject();
                        if(NULL == jnewArray)
                        {
                            ret = ERR_PNULL;
                            goto end;
                        }

                        cJSON_AddItemToArray(jAPPList, jnewArray);
                        cJSON_AddItemToObject(jnewArray, "name", cJSON_CreateString(pUartInfo->appList[j]));
                    }
                }
            }

            break;
        }
    }

    cJSON_write(json, g_uart_datafile);
end:    
    cJSON_Delete(json);  
    return ret;
}

/**********************************************************************
* @name      : uart_mode_cfg_update
* @brief     ：串口模式配置更新
* @param[in] ：UART_INFO_T *pUartInfo     串口参数
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_mode_cfg_update(UART_INFO_T *pUartInfo)
{
    int ret = 0;
    cJSON  *json = NULL;
    cJSON  *jList = NULL;
    cJSON  *jArray = NULL;
    cJSON  *item = NULL; 
    int     i = 0, j = 0;
    int    num = 0;
    UART_DEVDES_T *pUart = &pUartInfo->uart;;
    char desc[8] = {0};    
    uint32 baudRate = 0;
    int appNum = 0;

    json = cJSON_read(g_uart_datafile);
    if(!cJSON_IsObject(json))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no User config.\n");
        return ERR_PNULL;
    }

    jList = cJSON_GetObjectItem(json, "UART_CFG");
    if(!cJSON_IsArray(jList))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no list item.\n");
        goto end;
    }

    num = cJSON_GetArraySize(jList);

    //遍历配置数组
    for(i = 0; i < num; i++)
    {
        jArray = cJSON_GetArrayItem(jList, i);
        if(!cJSON_IsObject(jArray))
        {
            continue;
        }
        
        item = cJSON_GetObjectItem(jArray, "name");
        if(!cJSON_IsString(item))
        {
            continue;
        }

        if(strcmp(item->valuestring, pUartInfo->comname) == 0)
        {
            //存储模式
            memset(desc, '\0', sizeof(desc));
            uart_mode_tostring(pUartInfo->mode, desc);
            cJSON_ReplaceItemInObject(jArray, "mode", cJSON_CreateString(desc));

            if(pUartInfo->mode == UART_MODE_ONLY)
            {
                cJSON_ReplaceItemInObject(jArray, "onlyApp", cJSON_CreateString(pUartInfo->onlyApp));
            }

            if(pUartInfo->mode != UART_MODE_SLAVE)
            {
                item = cJSON_GetObjectItem(jArray, "appList");
                if(!cJSON_IsArray(item))
                {
                    return ERR_NOTEXIST;
                }

                appNum = cJSON_GetArraySize(item);
                for(j = 0; j < appNum; j++)
                {
                    cJSON_DeleteItemFromArray(item, j);
                }
            }
    
            //存储参数
            item = cJSON_GetObjectItem(jArray, "param");
            if(!cJSON_IsObject(item))
            {
                return ERR_NOTEXIST;
            }

            UART_BAUD_TO_STRING(baudRate, pUart->param.baudRate);

            cJSON_ReplaceItemInObject(item, "baudRate", cJSON_CreateNumber(baudRate));
            cJSON_ReplaceItemInObject(item, "byteSize", cJSON_CreateNumber(pUart->param.dataBits));
            
            memset(desc, '\0', sizeof(desc));
            uart_parity_tostring(pUart->param.parity, desc);
            cJSON_ReplaceItemInObject(item, "parity", cJSON_CreateString(desc));
            cJSON_ReplaceItemInObject(item, "stopBits", cJSON_CreateNumber(pUart->param.stopBits));            

            break;
        }
    }

    cJSON_write(json, g_uart_datafile);
end:    
    cJSON_Delete(json);  

    return ret;
}


#endif

#if DESC("通知发送",1)

/**********************************************************************
* @name      : uart_data_recv_notify
* @brief     ：数据接收通知
* @param[in] ：UART_APPDATA_T *ptAppData     公有数据
* @param[out]：UART_SERVICE_T *ptSevData      私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_data_recv_notify(uint8 *buffer, uint16 buflen, UART_APPDATA_T* ptAppData, UART_SERVICE_T *ptSevData)
{
    int ret = 0;
    UART_INFO_T *pUartInfo = ptSevData->pUartInfo;
    uint8 msgbuf[UART_DATA_MAXLEN] = {0};
    uint16 msglen = 0;
    UART_RECV_DATA_T recvData = {0};
    uint16 index = 0;
    int i = 0;
    char topic[256] = {0};
    char *msgstr = NULL;
    char destapp[APP_NAME_MAX] = {0};

    sprintf(destapp, "%s/%04X/%04X", NAME_BROADCAST, MESSAGE_IID_UART, UART_IOP_NOTIFY_RECV);

    if(pUartInfo->aliasEn)
    {
        recvData.port.portType = UART_TYPE_232;
        recvData.port.portNo = (pUartInfo->portAlias&0x000000ff)-4;
    }
    else
    {
        recvData.port.portType = UART_TYPE_485;
        recvData.port.portNo = pUartInfo->port&0x000000ff;
    }

    recvData.prm = 1; //启动帧
    recvData.status = UART_RESULT_OK;
    recvData.datalen = buflen;
    memcpy(recvData.data, buffer, buflen);
    index = uart_get_next_index();

    msglen = uart_axdr_data_recv_package(&recvData, msgbuf, UART_DATA_MAXLEN);

    for(i = 0; i < pUartInfo->listNum; i++)
    {
        ret |= uart_notify_send(index, UART_IOP_NOTIFY_RECV, pUartInfo->appList[i], msgbuf, msglen);
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("uart data recv notify send failed. %d\n", ret);
        }
    }

    for(i = 0; i < pUartInfo->listNum; i++)
    {
        msgstr = uart_json_data_recv_package(index, pUartInfo->appList[i], &recvData, topic, 256);
        if(msgstr != NULL)
        {
            ret |= uart_send_mqttmsg(topic, 0, msgstr, strlen(msgstr));
            if(ret != 0)
            {
                MQTTMSG_FMT_DEBUG("uart data recv notify send failed. %d\n", ret);
            }
            else
            {
                MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msgstr);
            }
            
            free(msgstr);
        }
    }

    ret |= uart_notify_send(index, UART_IOP_NOTIFY_RECV, destapp, msgbuf, msglen);
    if(ret != 0)
    {
        MQTTMSG_FMT_DEBUG("uart data recv notify send failed. %d\n", ret);
    }
        
    return ret;
}

/**********************************************************************
* @name      : uart_mode_change_notify
* @brief     ：模式变更通知
* @param[in] ：UART_APPDATA_T *ptAppData     公有数据
* @param[out]：UART_SERVICE_T *ptSevData      私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_mode_change_notify(UART_MODEINFO_T *modeInfo, UART_APPDATA_T* ptAppData, UART_SERVICE_T *ptSevData)
{
    int ret = 0;
    uint8 msgbuf[UART_QUEUE_BUFER] = {0};
    uint16 msglen = 0;
    char topic[256] = {0};
    char *msgstr = NULL;
    uint16 index = uart_get_next_index();
    char destapp[APP_NAME_MAX] = {0};

    sprintf(destapp, "%s/%04X/%04X", NAME_BROADCAST, MESSAGE_IID_UART, UART_IOP_NOTIFY_MODE);
    msglen = uart_axdr_notify_mode_package(modeInfo, msgbuf, UART_QUEUE_BUFER);

    ret = uart_notify_send(index, UART_IOP_NOTIFY_MODE, destapp, msgbuf, msglen);
    if(ret != 0)
    {
        MQTTMSG_FMT_DEBUG("uart mode change notify send failed. %d\n", ret);
    }

    msgstr = uart_json_notify_mode_package(index, modeInfo, topic, 256);
    if(msgstr != NULL)
    {
        ret |= uart_send_mqttmsg(topic, 0, msgstr, strlen(msgstr));
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("uart mode change notify send failed. %d\n", ret);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msgstr);
        }
        
        free(msgstr);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("组装模式变更消息失败\n");        
    }

    return ret;
}

#endif

#if DESC("AXDR消息处理",1)

/**********************************************************************
* @name      : uart_axdrmsg_respond
* @brief     ：串口响应的json消息
* @param[in] ：uint16 taskid       客户端id
               uint16 index        序号
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-12
* @Update    : 
**********************************************************************/
int uart_axdrmsg_respond(MESSAGE_INFO_T *pInMsg, uint8 *payload, uint32 msglen)
{
    int ret = 0;
    uint32 size = sizeof(MESSAGE_INFO_T) + msglen;
    MESSAGE_INFO_T *pOutMsg = (MESSAGE_INFO_T*)malloc(size);

    if(pOutMsg == NULL)
    {
        MQTTMSG_FMT_DEBUG("uart_axdrmsg_respond malloc failed, size %d.\n", size);
        return ERR_PNULL;
    }

    memset(pOutMsg, 0, size);
    pOutMsg->rpm = 0;
    pOutMsg->index = pInMsg->index;
    pOutMsg->priority = pInMsg->priority;
    pOutMsg->label = pInMsg->label;
    pOutMsg->IID = pInMsg->IID;
    pOutMsg->IOP = pInMsg->IOP;
    
    pOutMsg->sourlen = pInMsg->destlen;
    memcpy(pOutMsg->souraddr , pInMsg->destaddr, pOutMsg->sourlen);
    pOutMsg->destlen = pInMsg->sourlen;
    memcpy(pOutMsg->destaddr, pInMsg->souraddr, pOutMsg->destlen);

    pOutMsg->msglen = msglen;
    memcpy(&pOutMsg->playload, payload, msglen);
    
    ret = uart_send_axdrmsg(pOutMsg, 0);
    free(pOutMsg);

    return ret;
}

/**********************************************************************
* @name      : uart_axdr_info_get
* @brief     ：串口信息查询列表
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_axdr_info_get(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_PORT_T portInfo = {0};
    UART_MODESTAUS_T uartInfo = {0};
    uint8 payload[UART_QUEUE_BUFER];
    uint16 msglen = 0;
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    int i = 0;

    ret = uart_axdr_get_info_req_split(pInMsg, &portInfo);
    if(ret != 0)
    {
        MQTTMSG_FMT_DEBUG("uart_axdr_get_info failed, ret = %d.\n", ret);
        return ret;
    }

    uartInfo.port = portInfo;
    uartInfo.mode = pUartInfo->mode;
    strcpy(uartInfo.onlyApp, pUartInfo->onlyApp);
    memcpy(&uartInfo.param, &pUartInfo->uart.param, sizeof(UART_PARAM_T));
    uartInfo.num = pUartInfo->listNum;

    for(i = 0; i < uartInfo.num; i++)
    {
        strcpy(uartInfo.monitorApp[i], pUartInfo->appList[i]);
    }
    
    msglen = uart_axdr_get_info_res_package(&uartInfo, payload, UART_QUEUE_BUFER);

    return uart_axdrmsg_respond(pInMsg, payload, msglen);
}

/**********************************************************************
* @name      : uart_axdr_param_set
* @brief     ：串口参数设置
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_axdr_param_set(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0; 
    UART_PORT_T portInfo = {0};
    UART_APPREG_T portParam = {0};
    uint8 payload[UART_QUEUE_BUFER];
    uint16 msglen = 0;
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    char appName[64] = {0};

    if(pInMsg->souraddr[0] != 'M')
    {
        strcpy(appName, pInMsg->souraddr);
    }
    else
    {
        strcpy(appName, pInMsg->souraddr+strlen("M-"));
    }

    //当前串口工作于独占模式，且请求的APP为独占APP
    if((pUartInfo->mode == UART_MODE_ONLY) && (strcmp(appName, pUartInfo->onlyApp) == 0))
    {
        ret = uart_axdr_set_param_req_split(pInMsg, &portParam);
        if(ret != 0)
        {
            msglen = uart_axdr_set_param_res_package(&portInfo, UART_ACK_OTHER, payload, UART_QUEUE_BUFER);
        }
        else
        {
            msglen = uart_axdr_set_param_res_package(&portInfo, UART_ACK_OK, payload, UART_QUEUE_BUFER);
            memcpy(&pUartInfo->uart.param, &portParam.param, sizeof(UART_PARAM_T));
            pUartInfo->uart.isUpdate = TRUE;

            //独占模式的参数保存到配置文件
            ret = uart_param_cfg_update(pUartInfo);
            if(ret != 0)
            {
                UART_FMT_DEBUG(pUartInfo->uart.comdesc, "uart_param_cfg_update failed, ret %d.\n", ret);
            }
        }
    }
    else
    {
        msglen = uart_axdr_set_param_res_package(&portInfo, UART_ACK_MODEERR, payload, UART_QUEUE_BUFER);
    }
      
    return uart_axdrmsg_respond(pInMsg, payload, msglen);
}

/**********************************************************************
* @name      : uart_axdr_app_reg
* @brief     ：从模式APP注册
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_axdr_app_reg(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_PORT_T portInfo = {0};
    uint8 payload[UART_QUEUE_BUFER];
    uint16 msglen = 0;
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    char appName[64] = {0};
    int i = 0;

    if(pUartInfo->mode == UART_MODE_SLAVE)
    {
        if(pInMsg->souraddr[0] != 'M')
        {
            strcpy(appName, pInMsg->souraddr);
        }
        else
        {
            strcpy(appName, pInMsg->souraddr+strlen("M-"));
        }

        for(i = 0; i < pUartInfo->listNum; i++)
        {
            if(strcmp(pUartInfo->appList[i], appName)==0)
            {
                break;
            }
        }

        if(i == pUartInfo->listNum)
        {
            strcpy(pUartInfo->appList[pUartInfo->listNum++], appName);
        }


        //从模式监听APP保存到配置文件
        ret = uart_listen_cfg_update(pUartInfo);
        if(ret != 0)
        {
            UART_FMT_DEBUG(pUartInfo->uart.comdesc, "com_listen_cfg_update failed, ret %d.\n", ret);
        }      
        
        msglen = uart_axdr_set_monitor_res_package(&portInfo, UART_ACK_OK, payload, UART_QUEUE_BUFER);
    }
    else
    {
        msglen = uart_axdr_set_monitor_res_package(&portInfo, UART_ACK_MODEERR, payload, UART_QUEUE_BUFER);
    }
    
    return uart_axdrmsg_respond(pInMsg, payload, msglen);
}

/**********************************************************************
* @name      : uart_axdr_data_trans
* @brief     ：转发数据消息
* @param[in] ：MESSAGE_INFO_T *pMsg      MQTT消息
* @param[out]：UART_SERVICE_T *pHandle  私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_axdr_data_trans(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_DEVDES_T *pUart = &pHandle->pUartInfo->uart;
    uint8 *estBuf = NULL; //串口预估接收缓存
    uint16 estLen = 0;    //预估接收长度
    uint8 recvBuf[UART_DATA_MAXLEN] = {0};
    int recvLen = 0; //实际接收长度
    UART_DEVDES_T tmpUart = {0};
    UART_SEND_DATA_T sendData = {0};
    UART_TRANSMIT_T transmit = {0};
    UART_RECV_DATA_T recvData = {0};
    uint32 useTime = 0;

    memcpy(&tmpUart, pUart, sizeof(UART_DEVDES_T));
    
    ret = uart_axdr_data_send_req_split(pInMsg, &sendData);
    if(ret != 0)
    {
        MQTTMSG_BUF_DEBUG(pInMsg->playload, pInMsg->msglen, "axdr msg split failed. \n");
        return ret;
    }

    if(sendData.bParam == TRUE)
    {
        memcpy(&tmpUart.param, &sendData.param, sizeof(UART_PARAM_T));
    }

    estBuf = recvBuf;
    estLen = UART_DATA_MAXLEN;
    if(sendData.estsize > UART_DATA_MAXLEN)
    {
        estBuf = malloc(sendData.estsize);
        estLen = sendData.estsize;
    }

    //先响应本消息
    recvLen  = uart_axdr_data_send_res_package(&sendData.port, UART_ACK_OK, estBuf, estLen);
    ret = uart_axdrmsg_respond(pInMsg, estBuf, recvLen);

    //从模式直接发送
    if(pHandle->pUartInfo->mode == UART_MODE_SLAVE)
    {
        pUart->retval = com_write(&tmpUart, sendData.data, sendData.datalen);
        if(pUart->retval != 0)
        {
            UART_FMT_DEBUG(tmpUart.comdesc, "uart send failed.\n");
        }

        useTime = (tmpUart.sendTm.tv_sec - tmpUart.recvTm.tv_sec)*1000 + (tmpUart.sendTm.tv_nsec - tmpUart.recvTm.tv_nsec)/1000000;

        UART_FMT_DEBUG(tmpUart.comdesc, "comm use: %d ms, success rate: send-%d/recv-%d = %.2f%%.\n", 
            useTime, tmpUart.sendCnt, tmpUart.recvCnt, tmpUart.recvCnt==0 ? 0: tmpUart.sendCnt*100.0/tmpUart.recvCnt);
    }
    else
    {
        transmit.sendBuf = sendData.data;
        transmit.sendLen = sendData.datalen;
        transmit.recvBuf = estBuf;
        transmit.recvMax = estLen;
        transmit.frameTimeout = sendData.frametimeout;
        transmit.octetTimeout = sendData.bytetimeout;

        //主模式消息中参数与上次无变化且独占模式参数未更新
        if((memcmp(&tmpUart.param, &pUart->param, sizeof(UART_PARAM_T)) == 0) && (pUart->isUpdate == FALSE))
        {
            pUart->retval = com_transmit(&tmpUart, &transmit, &recvLen);
            if(pUart->retval != 0)
            {
                UART_FMT_DEBUG(tmpUart.comdesc, "uart transmit failed.\n");
            }
        }
        else
        {
            memcpy(&pUart->param, &tmpUart.param, sizeof(UART_PARAM_T));
            pUart->retval= com_forward(&tmpUart, &transmit, &recvLen);
            if(pUart->retval != 0)
            {
                UART_FMT_DEBUG(tmpUart.comdesc, "uart forward failed.\n");
            }
            else
            {
                pUart->isUpdate = FALSE;
            }
        }
        
        useTime = (tmpUart.recvTm.tv_sec - tmpUart.sendTm.tv_sec)*1000 + (tmpUart.recvTm.tv_nsec - tmpUart.sendTm.tv_nsec)/1000000;

        UART_FMT_DEBUG(tmpUart.comdesc, "comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
            useTime, tmpUart.recvCnt, tmpUart.sendCnt, tmpUart.sendCnt==0 ? 0: tmpUart.recvCnt*100.0/tmpUart.sendCnt);

        recvData.port = sendData.port;
        recvData.prm = 0; //应答帧
        recvData.status = tmpUart.status;
        recvData.datalen = recvLen;
        memcpy(recvData.data, transmit.recvBuf, recvLen);

        recvLen = uart_axdr_data_recv_package(&recvData, estBuf, estLen);

        //再发送接收数据通知
        ret = uart_notify_send(pInMsg->index, UART_IOP_NOTIFY_RECV, pInMsg->souraddr, estBuf, recvLen);
        if(ret != 0)
        {
            UART_FMT_DEBUG(tmpUart.comdesc, "uart data recv notify send failed. %d\n", ret);
        }
    }

    if(sendData.estsize > UART_DATA_MAXLEN)
    {
        free(estBuf);
    }

    pUart->sendCnt = tmpUart.sendCnt;
    pUart->recvCnt = tmpUart.recvCnt;
    pUart->sendTm = tmpUart.sendTm;
    pUart->recvTm = tmpUart.recvTm;
    
    return ret;
}

/**********************************************************************
* @name      : uart_axdr_abn_comm
* @brief     ：通信异常通知
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_axdr_abn_comm(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_PORT_T portInfo = {0}; 
    uint8 payload[UART_QUEUE_BUFER];
    uint16 msglen = 0;
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    UART_ABNORMAL_T cause = {0};
    
    ret = uart_axdr_set_abnormal_req_split(pInMsg, &cause);
    if(ret != 0)
    {
        MQTTMSG_FMT_DEBUG("uart_axdr_abn_comm failed, ret = %d.\n", ret);
        return ret;
    }

    //清空串口的收发队列
    tcflush(pUartInfo->uart.fd, TCIOFLUSH);

    msglen = uart_axdr_set_abnormal_res_package(&portInfo, TRUE, payload, UART_QUEUE_BUFER);
    
    return uart_axdrmsg_respond(pInMsg, payload, msglen);
}

/**********************************************************************
* @name      : uart_axdr_app_unreg
* @brief     ：从模式APP注册
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_axdr_app_unreg(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_PORT_T portInfo = {0};
    uint8 payload[UART_QUEUE_BUFER];
    uint16 msglen = 0;
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    char appName[64] = {0};
    int i = 0;

    if(pUartInfo->mode == UART_MODE_SLAVE)
    {
        if(pInMsg->souraddr[0] != 'M')
        {
            strcpy(appName, pInMsg->souraddr);
        }
        else
        {
            strcpy(appName, pInMsg->souraddr+strlen("M-"));
        }

        for(i = 0; i < pUartInfo->listNum; i++)
        {
            if(strcmp(pUartInfo->appList[i], appName)==0)
            {
                if(i < pUartInfo->listNum-1)
                {
                    memmove(pUartInfo->appList[i], pUartInfo->appList[i+1], APP_NAME_MAX);
                }
                
                pUartInfo->listNum--;

                //从模式监听APP保存到配置文件
                ret = uart_listen_cfg_update(pUartInfo);
                if(ret != 0)
                {
                    UART_FMT_DEBUG(pUartInfo->uart.comdesc, "com_listen_cfg_update failed, ret %d.\n", ret);
                }      
                
                msglen = uart_axdr_set_monitor_res_package(&portInfo, UART_ACK_OK, payload, UART_QUEUE_BUFER);
                
                break;
            }
        }

        if(i == pUartInfo->listNum)
        {
            msglen = uart_axdr_set_monitor_res_package(&portInfo, UART_ACK_UNREG, payload, UART_QUEUE_BUFER);
        }
    }
    else
    {
        msglen = uart_axdr_set_monitor_res_package(&portInfo, UART_ACK_MODEERR, payload, UART_QUEUE_BUFER);
    }
    
    return uart_axdrmsg_respond(pInMsg, payload, msglen);
}

/**********************************************************************
* @name      : uart_axdr_mode_set
* @brief     ：串口模式设置
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_axdr_mode_set(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_PORT_T portInfo = {0};
    UART_MODESET_T modeSet = {0};
    uint8 payload[UART_QUEUE_BUFER];
    uint16 msglen = 0;
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    char appName[64] = {0};
    UART_MODEINFO_T modeInfo = {0};

    if(pInMsg->souraddr[0] != 'M')
    {
        strcpy(appName, pInMsg->souraddr);
    }
    else
    {
        strcpy(appName, pInMsg->souraddr+strlen("M-"));
    }

    if ((strcmp(appName, "puAmr") == 0) || (strcmp(appName, "stAmr") == 0) || (strcmp(appName, "GDW376") == 0))
    {
        ret = uart_axdr_set_mode_req_split(pInMsg, &modeSet);
        if(ret != 0)
        {
            msglen = uart_axdr_set_mode_res_package(&portInfo, UART_ACK_OTHER, payload, UART_QUEUE_BUFER);
        }
        else
        {
            msglen = uart_axdr_set_mode_res_package(&portInfo, UART_ACK_OK, payload, UART_QUEUE_BUFER);
            if(modeSet.bParam == TRUE)
            {
                memcpy(&pUartInfo->uart.param, &modeSet.param, sizeof(UART_PARAM_T));
                pUartInfo->uart.isUpdate = TRUE;
            }

            strcpy(modeInfo.lastApp,pUartInfo->onlyApp);
            strcpy(modeInfo.curApp,modeSet.onlyApp);
            modeInfo.mode = modeSet.mode;
            modeInfo.port = modeSet.port;
            
            strcpy(pUartInfo->onlyApp, modeSet.onlyApp);
 
            pUartInfo->mode = modeSet.mode;

            uart_mode_cfg_update(pUartInfo);
            uart_mode_change_notify(&modeInfo, ptAppData, pHandle);
            
        }
    }
    else
    {
        msglen = uart_axdr_set_mode_res_package(&portInfo, UART_ACK_OTHER, payload, UART_QUEUE_BUFER);
    }
    
    return uart_axdrmsg_respond(pInMsg, payload, msglen);
}


UART_AXDR_FUN_T g_axdrmsg_fun[] = 
{
    {UART_IOP_GET_INFO,    uart_axdr_info_get},
    {UART_IOP_SET_PARAM,   uart_axdr_param_set},
    {UART_IOP_SET_LISTEN,  uart_axdr_app_reg},
    {UART_IOP_SEND_DATA,   uart_axdr_data_trans},
    {UART_IOP_SET_ABNORMAL,uart_axdr_abn_comm},
    {UART_IOP_UNSET_LISTEN,uart_axdr_app_unreg},
    {UART_IOP_ONLYMODE_SET,uart_axdr_mode_set},
};

uint8 g_axdrmsg_num = sizeof(g_axdrmsg_fun)/sizeof(UART_AXDR_FUN_T);

#endif

#if DESC("JSON消息处理",1)

/**********************************************************************
* @name      : uart_jsonmsg_respond
* @brief     ：串口响应的json消息
* @param[in] ：uint16 taskid       客户端id
               uint16 index        序号
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-12
* @Update    : 
**********************************************************************/
int uart_jsonmsg_respond(MESSAGE_JSON_T *pInMsg, MESSAGE_JSON_T *ptJson, char *port, char *status, char *data)
{
    char    TIME[36] = {0};
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;

    //构造主题字段
    strcpy(ptJson->sourapp, pInMsg->destapp);
    strcpy(ptJson->destapp, pInMsg->sourapp);
    strcpy(ptJson->operat, pInMsg->operat);
    strcpy(ptJson->infotype, MQTT_TOPIC_INFO_RES);
    strcpy(ptJson->infopath, pInMsg->infopath);

    ptJson->pObject = cJSON_CreateObject();
    if(NULL == ptJson->pObject)
    {
        MQTTMSG_FMT_DEBUG("json creat object failed.\n");
        return ERR_PNULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld+0800", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
                  tm.tm_hour, tm.tm_min, tm.tm_sec, tv.tv_usec/1000);

    cJSON_AddItemToObject(ptJson->pObject, "token", cJSON_CreateNumber(pInMsg->token));
    cJSON_AddItemToObject(ptJson->pObject, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(ptJson->pObject, "port", cJSON_CreateString(port));
    cJSON_AddItemToObject(ptJson->pObject, "status", cJSON_CreateString(status));
    cJSON_AddItemToObject(ptJson->pObject, "data", cJSON_CreateString(data));

    return 0;
}

/**********************************************************************
* @name      : uart_json_info_get
* @brief     ：串口信息查询列表
* @param[in] ：MESSAGE_JSON_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_json_info_get(MESSAGE_JSON_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_PORT_T portInfo = {0};
    UART_MODESTAUS_T uartInfo = {0};
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    int i = 0;
    char topic[256] = {0};
    char *msgstr = NULL;

    ret = uart_json_get_info_req_split(pInMsg, &portInfo);
    if(ret != 0)
    {
        MQTTMSG_FMT_DEBUG("uart_axdr_get_info failed, ret = %d.\n", ret);
        return ret;
    }

    uartInfo.port = portInfo;
    uartInfo.mode = pUartInfo->mode;
    strcpy(uartInfo.onlyApp, pUartInfo->onlyApp);
    memcpy(&uartInfo.param, &pUartInfo->uart.param, sizeof(UART_PARAM_T));
    uartInfo.num = pUartInfo->listNum;

    for(i = 0; i < uartInfo.num; i++)
    {
        strcpy(uartInfo.monitorApp[i], pUartInfo->appList[i]);
    }
    
    msgstr = uart_json_get_info_res_package(pInMsg->token, pInMsg->sourapp, &uartInfo, topic, 256);
    if(msgstr != NULL)
    {
        ret = uart_send_mqttmsg(topic, 0, msgstr, strlen(msgstr));
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("uart info get failed. %d\n", ret);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msgstr);
        }        
        free(msgstr);
    }

    return ret;
}

/**********************************************************************
* @name      : uart_json_param_set
* @brief     ：串口参数设置
* @param[in] ：MESSAGE_JSON_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_json_param_set(MESSAGE_JSON_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0; 
    UART_APPREG_T portParam = {0};
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    char appName[64] = {0};
    char topic[256] = {0};
    char *msgstr = NULL;

    if(pInMsg->sourapp[0] != 'M')
    {
        strcpy(appName, pInMsg->sourapp);
    }
    else
    {
        strcpy(appName, pInMsg->sourapp+strlen("M-"));
    }

    //当前串口工作于独占模式，且请求的APP为独占APP
    if((pUartInfo->mode == UART_MODE_ONLY) && (strcmp(appName, pUartInfo->onlyApp) == 0))
    {
        ret = uart_json_set_param_req_split(pInMsg, &portParam);
        if(ret != 0)
        {
            msgstr = uart_json_set_param_res_package(pInMsg->token, pInMsg->sourapp, &portParam.port, "No Permission", topic, 256);
        }
        else
        {
            msgstr = uart_json_set_param_res_package(pInMsg->token, pInMsg->sourapp, &portParam.port, "OK", topic, 256);
            memcpy(&pUartInfo->uart.param, &portParam.param, sizeof(UART_PARAM_T));
            pUartInfo->uart.isUpdate = TRUE;

            //独占模式的参数保存到配置文件
            ret = uart_param_cfg_update(pUartInfo);
            if(ret != 0)
            {
                UART_FMT_DEBUG(pUartInfo->uart.comdesc, "uart_param_cfg_update failed, ret %d.\n", ret);
            }          
        }
    }
    else
    {
        msgstr = uart_json_set_param_res_package(pInMsg->token, pInMsg->sourapp, &portParam.port, "No Permission", topic, 256);
    }
      
    if(msgstr != NULL)
    {
        ret = uart_send_mqttmsg(topic, 0, msgstr, strlen(msgstr));
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("uart param set failed. %d\n", ret);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msgstr);
        }         
        free(msgstr);
    }

    return ret;
}

/**********************************************************************
* @name      : uart_axdr_app_reg
* @brief     ：从模式APP注册
* @param[in] ：MESSAGE_JSON_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_json_app_reg(MESSAGE_JSON_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0; 
    UART_APPREG_T appReg = {0};
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    char appName[64] = {0};
    char topic[256] = {0};
    char *msgstr = NULL;
    int i = 0;

    ret = uart_json_set_monitor_req_split(pInMsg, &appReg);

    if(pUartInfo->mode == UART_MODE_SLAVE)
    {
        if(pInMsg->sourapp[0] != 'M')
        {
            strcpy(appName, pInMsg->sourapp);
        }
        else
        {
            strcpy(appName, pInMsg->sourapp+strlen("M-"));
        }

        for(i = 0; i < pUartInfo->listNum; i++)
        {
            if(strcmp(pUartInfo->appList[i], appName)==0)
            {
                break;
            }
        }

        if(i == pUartInfo->listNum)
        {
            strcpy(pUartInfo->appList[pUartInfo->listNum++], appName);
        }

        //从模式监听APP保存到配置文件
        ret = uart_listen_cfg_update(pUartInfo);
        if(ret != 0)
        {
            UART_FMT_DEBUG(pUartInfo->uart.comdesc, "com_listen_cfg_update failed, ret %d.\n", ret);
        }         
        
        msgstr = uart_json_set_monitor_res_package(pInMsg->token, pInMsg->sourapp, &appReg.port, "OK", topic, 256);
    }
    else
    {
        msgstr = uart_json_set_monitor_res_package(pInMsg->token, pInMsg->sourapp, &appReg.port, "No Permission", topic, 256);
    }
    
    if(msgstr != NULL)
    {
        ret = uart_send_mqttmsg(topic, 0, msgstr, strlen(msgstr));
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("uart app register failed. %d\n", ret);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msgstr);
        } 
        
        free(msgstr);
    }

    return ret;
}

/**********************************************************************
* @name      : uart_json_data_trans
* @brief     ：转发数据消息
* @param[in] ：MESSAGE_JSON_T *pMsg      MQTT消息
* @param[out]：UART_SERVICE_T *pHandle  私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_json_data_trans(MESSAGE_JSON_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_DEVDES_T *pUart = &pHandle->pUartInfo->uart;
    uint8 *estBuf = NULL; //串口预估接收缓存
    uint16 estLen = 0;    //预估接收长度
    uint8 recvBuf[UART_QUEUE_BUFER] = {0};
    int recvLen = 0; //实际接收长度
    UART_DEVDES_T tmpUart = {0};
    UART_SEND_DATA_T sendData = {0};
    UART_TRANSMIT_T transmit = {0};
    UART_RECV_DATA_T recvData = {0};
//    struct timespec curtime = {0};
    char topic[256] = {0};
    char *msgstr = NULL;
    uint32 useTime = 0;

    memcpy(&tmpUart, pUart, sizeof(UART_DEVDES_T));

    ret = uart_json_data_send_split(pInMsg, &sendData);
    if(ret != 0)
    {
        MQTTMSG_FMT_DEBUG("json msg split failed, infopath: %s.\n", pInMsg->infopath);
        return ret;
    }

    if(sendData.bParam == TRUE)
    {
        memcpy(&tmpUart.param, &sendData.param, sizeof(UART_PARAM_T));
    }

    estBuf = recvBuf;
    estLen = UART_QUEUE_BUFER;
    if(sendData.estsize > UART_QUEUE_BUFER)
    {
        estBuf = malloc(sendData.estsize);
        estLen = sendData.estsize;
    }

    //从模式直接发送
    if(pHandle->pUartInfo->mode == UART_MODE_SLAVE)
    {
        pUart->retval = com_write(&tmpUart, sendData.data, sendData.datalen);
        if(pUart->retval != 0)
        {
            UART_FMT_DEBUG(tmpUart.comdesc, "uart send failed.\n");
        }   

        useTime = (tmpUart.sendTm.tv_sec - tmpUart.recvTm.tv_sec)*1000 + (tmpUart.sendTm.tv_nsec - tmpUart.recvTm.tv_nsec)/1000000;
        
        UART_FMT_DEBUG(tmpUart.comdesc, "comm use: %d ms, success rate: send-%d/recv-%d = %.2f%%.\n", 
            useTime, tmpUart.sendCnt, tmpUart.recvCnt, tmpUart.recvCnt==0 ? 0: tmpUart.sendCnt*100.0/tmpUart.recvCnt);
        
    }
    else
    {
        transmit.sendBuf = sendData.data;
        transmit.sendLen = sendData.datalen;
        transmit.recvBuf = estBuf;
        transmit.recvMax = estLen;
        transmit.frameTimeout = sendData.frametimeout;
        transmit.octetTimeout = sendData.bytetimeout;
        
        //主模式消息中参数与上次无变化且独占模式参数未更新
        if((memcmp(&tmpUart.param, &pUart->param, sizeof(UART_PARAM_T)) == 0) && (pUart->isUpdate == FALSE))
        {
            pUart->retval = com_transmit(&tmpUart, &transmit, &recvLen);
            if(pUart->retval != 0)
            {
                UART_FMT_DEBUG(tmpUart.comdesc, "uart transmit failed.\n");
            }
        }
        else
        {
            memcpy(&pUart->param, &tmpUart.param, sizeof(UART_PARAM_T));
            pUart->retval = com_forward(&tmpUart, &transmit, &recvLen);
            if(pUart->retval != 0)
            {
                UART_FMT_DEBUG(tmpUart.comdesc, "uart forward failed.\n");
            }
            else
            {
                pUart->isUpdate = FALSE;
            }
        }

//        clock_gettime(CLOCK_MONOTONIC, &curtime);
//        if((curtime.tv_nsec/1000000 - ptAppData->msgtime) > pHandle->tasktimeout)
//        {
//            recvData.status = UART_RESULT_TIME_OUT;
//        }
//        else
//        {
//            recvData.status = tmpUart.status;
//        }

        useTime = (tmpUart.recvTm.tv_sec - tmpUart.sendTm.tv_sec)*1000 + (tmpUart.recvTm.tv_nsec - tmpUart.sendTm.tv_nsec)/1000000;

        UART_FMT_DEBUG(tmpUart.comdesc, "comm use: %d ms, success rate: recv-%d/send-%d = %.2f%%.\n", 
            useTime, tmpUart.recvCnt, tmpUart.sendCnt, tmpUart.sendCnt==0 ? 0: tmpUart.recvCnt*100.0/tmpUart.sendCnt);

        recvData.port = sendData.port;
        recvData.prm = 0; //应答帧
        recvData.datalen = recvLen;
        memcpy(recvData.data, transmit.recvBuf, recvLen);

        msgstr = uart_json_data_recv_package(pInMsg->token, pInMsg->sourapp, &recvData, topic, 256);
        if(msgstr != NULL)
        {
            ret = uart_send_mqttmsg(topic, 0, msgstr, strlen(msgstr));
            if(ret != 0)
            {
                MQTTMSG_FMT_DEBUG("uart data trans failed. %d\n", ret);
            }
            else
            {
                MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msgstr);
            }                
            free(msgstr);        
        }
    }   

    if(sendData.estsize > UART_QUEUE_BUFER)
    {
        free(estBuf);
    }
    
    return ret;
}

/**********************************************************************
* @name      : uart_json_abn_comm
* @brief     ：通信异常通知
* @param[in] ：MESSAGE_JSON_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int uart_json_abn_comm(MESSAGE_JSON_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle)
{
    int ret = 0;
    UART_INFO_T *pUartInfo = pHandle->pUartInfo;
    UART_ABNORMAL_T cause = {0};
    char topic[256] = {0};
    char *msgstr = NULL;
    
    ret = uart_json_set_abnormal_req_split(pInMsg, &cause);
    if(ret != 0)
    {
        MQTTMSG_FMT_DEBUG("uart_axdr_abn_comm failed, ret = %d.\n", ret);
        return ret;
    }

    //清空串口的收发队列
    tcflush(pUartInfo->uart.fd, TCIOFLUSH);

    msgstr = uart_json_set_abnormal_res_package(pInMsg->token, pInMsg->sourapp, &cause.port, "OK", topic, 256);
    if(msgstr != NULL)
    {
        ret = uart_send_mqttmsg(topic, 0, msgstr, strlen(msgstr));
        if(ret != 0)
        {
            MQTTMSG_FMT_DEBUG("uart abnormal res failed. %d\n", ret);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", topic, msgstr);
        }        
        free(msgstr);        
    }

    return ret;
}

UART_JSON_FUN_T g_jsonmsg_fun[] = 
{
    {UART_TOPIC_UARTINFO,    uart_json_info_get},
    {UART_TOPIC_ONLYPARAM,   uart_json_param_set},
    {UART_TOPIC_MONITORREG,  uart_json_app_reg},
    {UART_TOPIC_DATA,        uart_json_data_trans},
    {UART_TOPIC_ABNORMAL,    uart_json_abn_comm},    
};

uint8 g_jsonmsg_num = sizeof(g_jsonmsg_fun)/sizeof(UART_JSON_FUN_T);

#endif

#if DESC("串口初始化",1)

/**********************************************************************
* @name      : uart_rs4851_init
* @brief     ：RS485-1端口初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_rs485_1_init(void *pPublic, void **ppPrivate)
{
    UART_SERVICE_T *ptData = NULL;

    ptData = (UART_SERVICE_T *)malloc(sizeof(UART_SERVICE_T));
    if(NULL == ptData)
    {
        UART_FMT_DEBUG(g_uart_info[0].uart.comdesc, "uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(UART_SERVICE_T));
    
    ptData->pUartInfo = &g_uart_info[0];
    ptData->pUartInfo->uart.isUpdate = TRUE;
    ptData->pUartInfo->uart.status = UART_STATUS_OK;

//    uart_pque_init(&ptData->pUartInfo->pque);
//    uart_fifo_init(&ptData->pUartInfo->fifo);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : uart_rs4852_init
* @brief     ：RS485-2端口初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_rs485_2_init(void *pPublic, void **ppPrivate)
{
    UART_SERVICE_T *ptData = NULL;

    ptData = (UART_SERVICE_T *)malloc(sizeof(UART_SERVICE_T));
    if(NULL == ptData)
    {
        UART_FMT_DEBUG(g_uart_info[1].uart.comdesc, "uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(UART_SERVICE_T));

    ptData->pUartInfo = &g_uart_info[1];
    ptData->pUartInfo->uart.isUpdate = TRUE;
    ptData->pUartInfo->uart.status = UART_STATUS_OK;

//    uart_pque_init(&ptData->pUartInfo->pque);
//    uart_fifo_init(&ptData->pUartInfo->fifo);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : uart_rs4853_init
* @brief     ：RS485-3端口初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_rs485_3_init(void *pPublic, void **ppPrivate)
{
    UART_SERVICE_T *ptData = NULL;

    ptData = (UART_SERVICE_T *)malloc(sizeof(UART_SERVICE_T));
    if(NULL == ptData)
    {
        UART_FMT_DEBUG(g_uart_info[2].uart.comdesc, "uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(UART_SERVICE_T));

    ptData->pUartInfo = &g_uart_info[2];
    ptData->pUartInfo->uart.isUpdate = TRUE;
    ptData->pUartInfo->uart.status = UART_STATUS_OK;

//    uart_pque_init(&ptData->pUartInfo->pque);
//    uart_fifo_init(&ptData->pUartInfo->fifo);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : uart_rs4853_init
* @brief     ：RS485-3端口初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_rs485_4_init(void *pPublic, void **ppPrivate)
{
    UART_SERVICE_T *ptData = NULL;

    ptData = (UART_SERVICE_T *)malloc(sizeof(UART_SERVICE_T));
    if(NULL == ptData)
    {
        UART_FMT_DEBUG(g_uart_info[3].uart.comdesc, "uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(UART_SERVICE_T));

    ptData->pUartInfo = &g_uart_info[3];
    ptData->pUartInfo->uart.isUpdate = TRUE;
    ptData->pUartInfo->uart.status = UART_STATUS_OK;

//    uart_pque_init(&ptData->pUartInfo->pque);
//    uart_fifo_init(&ptData->pUartInfo->fifo);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : uart_rs4851_init
* @brief     ：RS485-1端口初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_rs485_5_init(void *pPublic, void **ppPrivate)
{
    UART_SERVICE_T *ptData = NULL;

    ptData = (UART_SERVICE_T *)malloc(sizeof(UART_SERVICE_T));
    if(NULL == ptData)
    {
        UART_FMT_DEBUG(g_uart_info[4].uart.comdesc, "uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(UART_SERVICE_T));
    
    ptData->pUartInfo = &g_uart_info[4];
    ptData->pUartInfo->uart.isUpdate = TRUE;
    ptData->pUartInfo->uart.status = UART_STATUS_OK;

//    uart_pque_init(&ptData->pUartInfo->pque);
//    uart_fifo_init(&ptData->pUartInfo->fifo);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : uart_rs4852_init
* @brief     ：RS485-2端口初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_rs485_6_init(void *pPublic, void **ppPrivate)
{
    UART_SERVICE_T *ptData = NULL;

    ptData = (UART_SERVICE_T *)malloc(sizeof(UART_SERVICE_T));
    if(NULL == ptData)
    {
        UART_FMT_DEBUG(g_uart_info[5].uart.comdesc, "uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(UART_SERVICE_T));

    ptData->pUartInfo = &g_uart_info[5];
    ptData->pUartInfo->uart.isUpdate = TRUE;
    ptData->pUartInfo->uart.status = UART_STATUS_OK;

//    uart_pque_init(&ptData->pUartInfo->pque);
//    uart_fifo_init(&ptData->pUartInfo->fifo);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : uart_rs4853_init
* @brief     ：RS485-3端口初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_rs485_7_init(void *pPublic, void **ppPrivate)
{
    UART_SERVICE_T *ptData = NULL;

    ptData = (UART_SERVICE_T *)malloc(sizeof(UART_SERVICE_T));
    if(NULL == ptData)
    {
        UART_FMT_DEBUG(g_uart_info[6].uart.comdesc, "uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(UART_SERVICE_T));

    ptData->pUartInfo = &g_uart_info[6];
    ptData->pUartInfo->uart.isUpdate = TRUE;
    ptData->pUartInfo->uart.status = UART_STATUS_OK;

//    uart_pque_init(&ptData->pUartInfo->pque);
//    uart_fifo_init(&ptData->pUartInfo->fifo);

    *ppPrivate = ptData;
    return 0;
}

/**********************************************************************
* @name      : uart_rs4853_init
* @brief     ：RS485-3端口初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-07-09
* @Update    :
**********************************************************************/
int uart_rs485_8_init(void *pPublic, void **ppPrivate)
{
    UART_SERVICE_T *ptData = NULL;

    ptData = (UART_SERVICE_T *)malloc(sizeof(UART_SERVICE_T));
    if(NULL == ptData)
    {
        UART_FMT_DEBUG(g_uart_info[7].uart.comdesc, "uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptData, 0, sizeof(UART_SERVICE_T));

    ptData->pUartInfo = &g_uart_info[7];
    ptData->pUartInfo->uart.isUpdate = TRUE;
    ptData->pUartInfo->uart.status = UART_STATUS_OK;

//    uart_pque_init(&ptData->pUartInfo->pque);
//    uart_fifo_init(&ptData->pUartInfo->fifo);

    *ppPrivate = ptData;
    return 0;
}

#endif


/**********************************************************************
* @name      : uart_service_proc
* @brief     ：串口服务业务线程
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_service_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int ret = 0;
    int i = 0;
    UART_APPDATA_T *ptAppData = (UART_APPDATA_T*)pPublic;
    UART_SERVICE_T *ptMsgData = (UART_SERVICE_T*)pPrivate;
    UART_INFO_T *ptUartInfo = ptMsgData->pUartInfo;
    UART_QUEUE_T   queueData = {0};
    MESSAGE_MQTT_T *pMqttMsg = (MESSAGE_MQTT_T*)queueData.data;
    uint8 sRecvbuf[2048] = {0};//串口接收
    int sRecvLen = 0; //串口接收长度

    //从模式监听串口数据
    if(ptUartInfo->mode == UART_MODE_SLAVE)
    {
        TASK_SET_USERLOG(pArg, "uart slave listen.\n");
        sRecvLen = com_listen(&ptMsgData->pUartInfo->uart, sRecvbuf, sizeof(sRecvbuf));
        if(sRecvLen > 0)
        {
            uart_data_recv_notify(sRecvbuf, sRecvLen, ptAppData, ptMsgData);
        }
    }
    else //独占模式和主模式
    {
        TASK_SET_USERLOG(pArg, "uart master pque.\n");
        //从优先队列里取出优先级最高的消息
        ret = uart_pque_pop(&ptUartInfo->pque, &queueData);
        if(ret == 0)
        {
            if(pMqttMsg->msgtype == MESSAGE_TYPE_A_XDR)
            {
            
                TASK_SET_USERLOG(pArg, "pque pop axdr msg.\n");
                ret = uart_axdr_data_trans(&pMqttMsg->tAXDR, ptAppData, ptMsgData);
                if(ret != 0)
                {
                    UART_FMT_DEBUG(ptUartInfo->uart.comdesc, "uart iop 0x%x, ret %d\n", pMqttMsg->tAXDR.IOP, ret);
                    return ret;
                }            
            }
            else
            {
                TASK_SET_USERLOG(pArg, "pque pop json msg.\n");
                ret = uart_json_data_trans(&pMqttMsg->tJson, ptAppData, ptMsgData);
                if(ret != 0)
                {
                    UART_FMT_DEBUG(ptUartInfo->uart.comdesc, "uart infopath %s, ret %d\n", pMqttMsg->tJson.infopath, ret);
                    return ret;
                }

                appmsg_json_free(pMqttMsg);
            }
            
            return ret;
        }
    }

    TASK_SET_USERLOG(pArg, "uart fifo recv.\n");

    //从FIFO队列里取出消息
    ret = uart_fifo_pop(&ptUartInfo->fifo, &queueData);
    if(ret == 0)
    {
        if(pMqttMsg->msgtype == MESSAGE_TYPE_A_XDR)
        {
        
            TASK_SET_USERLOG(pArg, "fifo pop axdr msg 0x%04x.\n", pMqttMsg->tAXDR.IOP);
            
            for(i = 0; i < g_axdrmsg_num; i++)
            {
                if(g_axdrmsg_fun[i].IOP == pMqttMsg->tAXDR.IOP)
                {
                    ret = g_axdrmsg_fun[i].pProcMsg(&pMqttMsg->tAXDR, ptAppData, ptMsgData);
                    if(ret != 0)
                    {
                        UART_FMT_DEBUG(ptUartInfo->uart.comdesc, "uart iop 0x%x, ret %d\n", pMqttMsg->tAXDR.IOP, ret);
                        return ret;
                    }
                }
            }
        }
        else
        {
            TASK_SET_USERLOG(pArg, "fifo pop json msg %s.\n", pMqttMsg->tJson.infopath);
            
            for(i = 0; i < g_jsonmsg_num; i++)
            {
                if(strstr(pMqttMsg->tJson.infopath, g_jsonmsg_fun[i].infopath) != NULL)
                {
                    ret = g_jsonmsg_fun[i].pProcMsg(&pMqttMsg->tJson, ptAppData, ptMsgData);
                    if(ret != 0)
                    {
                        UART_FMT_DEBUG(ptUartInfo->uart.comdesc, "uart infopath %s, ret %d\n", pMqttMsg->tJson.infopath, ret);
                        return ret;
                    }
                }
            }
            
            appmsg_json_free(pMqttMsg);
        }
        
        return ret;
    }

    return ret;
}

/**********************************************************************
* @name      : uart_work_schedule
* @brief     ：串口服务业务线程
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_work_schedule(void *pArg, void *pPublic, void *pPrivate)
{
    int ret = 0;
    UART_SERVICE_T *ptMsgData = (UART_SERVICE_T*)pPrivate;
    UART_DEVDES_T *pUart = &ptMsgData->pUartInfo->uart;
    UART_DEVICE_T* dev = NULL;

    pUart->retval = 0;

    switch(ptMsgData->pUartInfo->workstate)
    {
        case UART_INIT:
        {
            ret = com_open(pUart);
            if(ret == ERR_OK)
            {
                ptMsgData->pUartInfo->workstate = UART_WORK;
                ptMsgData->pUartInfo->uart.isUpdate = TRUE;
            }
            sleep(1);
        }break;
        
        case UART_CONF:
        {
            ret = com_reopen(pUart);
            if(ret == ERR_OK)
            {
//                if(ptMsgData->pUartInfo->mode == UART_MODE_SLAVE)
//                {
//                    ptMsgData->pUartInfo->workstate = UART_PARA;
//                }
//                else
                {
                    ptMsgData->pUartInfo->workstate = UART_WORK;
                    ptMsgData->pUartInfo->uart.isUpdate = TRUE;
                }
            }
            sleep(1);
        }break;

//        case UART_PARA:
//        {
//            ret = uart_param_set(pUart->fd, &pUart->param);
//            if(ret == TRUE)
//            {
//                ptMsgData->pUartInfo->workstate = UART_WORK;
//                
//                UART_FMT_DEBUG(pUart->comdesc, "open %s success: baudrate[%d]-databit[%d]-parity[%s]-stopbit[%d].\n", 
//                    pUart->devname, g_uart_baud[pUart->param.baudRate], pUart->param.dataBits, g_uart_parity_desc[pUart->param.parity], pUart->param.stopBits);
//            }
//            sleep(1);
//        }break;

        case UART_WORK:
        {
            uart_service_proc(pArg, pPublic, pPrivate);
            usleep(10000);
        }break;

        default: break;
    }

    if((ptMsgData->pUartInfo->workstate == UART_WORK)&&(pUart->retval != 0))
    {
        ptMsgData->pUartInfo->workstate = UART_CONF;         
        UART_FMT_DEBUG(pUart->comdesc, "%s操作出错,返回%d\n", pUart->halname, pUart->retval);
    }

#ifdef PRODUCT_SCU
    if(uart_param_get(pUart->fd, &pUart->param) == FALSE)
    {
        ptMsgData->pUartInfo->workstate = UART_CONF;
    }
#else
    if(pUart->comType == COM_UART)
    {
        dev = (UART_DEVICE_T* )pUart->fd;
        if(dev == NULL)
        {
            ptMsgData->pUartInfo->workstate = UART_INIT;
            UART_FMT_DEBUG(pUart->comdesc, "%s未初始化\n", pUart->halname);
        }
        else if(dev->uart_get_status(dev)< 0)
        {
            ptMsgData->pUartInfo->workstate = UART_CONF;        
            UART_FMT_DEBUG(pUart->comdesc, "%s状态异常\n", pUart->devname);
        }
    }
    else
    {
#ifdef BOARD_3568
        if(can_param_get(pUart->fd, &pUart->param) == FALSE)
        {
            ptMsgData->pUartInfo->workstate = UART_CONF;
            UART_FMT_DEBUG(pUart->comdesc, "%s状态异常\n", pUart->halname);
        }
#endif
    }
#endif
    return ret;
}

/**********************************************************************
* @name      : uart_work_schedule
* @brief     ：串口服务业务线程
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int uart_virtual_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int ret = 0;
    UART_SERVICE_T *ptMsgData = (UART_SERVICE_T*)pPrivate;
    UART_DEVDES_T *pUart = &ptMsgData->pUartInfo->uart;
    UART_DEVICE_T* dev = NULL;

    //收到模块插拔事件
    if(g_uart_modinfo.change[ptMsgData->pUartInfo->portAlias] == TRUE)
    {    
        g_uart_modinfo.change[ptMsgData->pUartInfo->portAlias] = FALSE;
        
        if(g_uart_modinfo.status == MOD_STATUS_EXTRACT)
        {
            com_close(pUart);
        }
        else if(g_uart_modinfo.status == MOD_STATUS_INSERT)
        {
            ptMsgData->pUartInfo->workstate = UART_INIT;
        }
    }

    pUart->retval = 0;

    switch(ptMsgData->pUartInfo->workstate)
    {
        case UART_INIT:
        {
            if(g_uart_modinfo.slot != 0)
            {
                if(pUart->comType == COM_UART)
                {
                    sprintf(pUart->halname, "%s%d_%d",MOD_DEV_DESC_PRFIX, g_uart_modinfo.slot, ptMsgData->pUartInfo->portAlias); 
                    sprintf(pUart->devname, "%s",g_uart_modinfo.drvName[ptMsgData->pUartInfo->portAlias]); 
                }
                else if(pUart->comType == COM_CAN)
                {
                    ret= remove(pUart->devname);                
                    UART_FMT_DEBUG(pUart->comdesc, "%s删除%s,返回%d\n", pUart->halname, pUart->devname, ret);
                    symlink(g_uart_modinfo.drvName[ptMsgData->pUartInfo->portAlias], pUart->devname);                    
                    UART_FMT_DEBUG(pUart->comdesc, "%s新建%s->%s\n", pUart->halname, g_uart_modinfo.drvName[ptMsgData->pUartInfo->portAlias], pUart->devname);
                }
               
                ret = com_reopen(pUart);
                if(ret == ERR_OK)
                {
                    ptMsgData->pUartInfo->workstate = UART_WORK;
                    ptMsgData->pUartInfo->uart.isUpdate = TRUE;
                }
                else
                {
                    ptMsgData->pUartInfo->workstate = UART_GETM;
                }
            }
            else
            {
                ptMsgData->pUartInfo->workstate = UART_GETM;
            }
            
            sleep(1);
        }break;
        
        case UART_CONF:
        {
            ret = com_reopen(pUart);
            if(ret == ERR_OK)
            {
                ptMsgData->pUartInfo->workstate = UART_WORK;
                ptMsgData->pUartInfo->uart.isUpdate = TRUE;
            }
            sleep(1);
        }break;

        case UART_WORK:
        {
            uart_service_proc(pArg, pPublic, pPrivate);
            usleep(10000);
        }break;

        case UART_GETM:
        {
            if(g_uart_modinfo.slot != 0)
            {
                ptMsgData->pUartInfo->workstate = UART_INIT;
            }
            
            //过秒执行，1s查一次，查10次,查不到等待模组插入
            if(pUart->retry++ < 10 )
            {
                //uart_slotinfo_send(MOD_UART_TYPE_PRFIX); 
                uart_modinfo_send();
                sleep(1);
            }
            else
            {   
                ptMsgData->pUartInfo->workstate = UART_IDLE;
                pUart->retry = 0;
            }
        }break;

        default: break;
    }

    if((ptMsgData->pUartInfo->workstate == UART_WORK)&&(pUart->retval != 0))
    {
        ptMsgData->pUartInfo->workstate = UART_CONF;         
        UART_FMT_DEBUG(pUart->comdesc, "%s操作出错,返回%d\n", pUart->halname, pUart->retval);
    }
 
    if(pUart->comType == COM_UART)
    {
        dev = (UART_DEVICE_T* )pUart->fd;
        if((dev != NULL)&&(dev->uart_get_status(dev)< 0))
        {
            ptMsgData->pUartInfo->workstate = UART_CONF;        
            UART_FMT_DEBUG(pUart->comdesc, "%s状态异常\n", pUart->devname);
        }
    }
    else
    {
        if((pUart->fd != 0) && (can_param_get(pUart->fd, &pUart->param) == FALSE))
        {
            ptMsgData->pUartInfo->workstate = UART_CONF;
            UART_FMT_DEBUG(pUart->comdesc, "%s状态异常\n", pUart->halname);
        }
    }
    
    return 10000;
}

