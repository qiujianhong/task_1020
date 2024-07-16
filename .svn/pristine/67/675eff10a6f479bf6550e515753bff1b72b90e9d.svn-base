/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块主线程功能
* @date：    2022-3-20
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "mapmsg_split.h"

#include "rsp_data.h"
#include "rsp_module.h"

#if DESC("终端本地串口操作", 1)

/**********************************************************************
* @name      : uart_create
* @brief     ：串口打开
* @param[in] ：char *name  串口名称
* @param[out]：
* @return    ：>=0-句柄/错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_uart_open(char *name)
{
    if(NULL == name || 0 == strlen(name))
    {
        return -1;
    }
    return open(name, O_RDWR|O_NOCTTY|O_NDELAY);
}

/**********************************************************************
* @name      : rsp_set_speed
* @brief     ：设置波特率
* @param[in] ：int fd           串口句柄
               int speed        波特率
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_set_speed(int fd, int speed)
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
* @name      : rsp_set_parity
* @brief     ：设置属性
* @param[in] ：int fd              串口句柄
               uint8  databits     数据位(5~8)
               uint8  stopbits     停止位(1~2)
               char   parity       校验位(N 无校验，E 偶校验，O奇校验)
               uint8  set_crtscts  硬件流控 1-设置 0-不设
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_set_parity(const int fd, uint8 databits, uint8 stopbits, char parity, uint8 set_crtscts)
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
* @name      : uart_set_param
* @brief     ：串口设置属性
* @param[in] ：int fd                句柄
               int    baud           波特率
               uint8  databits       数据位(5~8)
               uint8  stopbits       停止位(1~2)
               char   parity         校验位(N 无校验，E 偶校验，O奇校验)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_uart_set(int fd, int baud, uint8 databits, uint8 stopbits, char parity)
{
    int ret = 0;
    if(fd < 0)
    {
        return -1;
    }

    ret = rsp_set_speed(fd, baud);
    if(0 != ret)
    {
        printf("set_speed false, ret %d!\n", ret);
        return ret;
    }

    ret = rsp_set_parity(fd, databits, stopbits, parity, 0);
    if(0 != ret)
    {
        printf("set_Parity false, ret %d!\n", ret);
        return ret;
    }
    return 0;
}

/**********************************************************************
* @name      : rsp_uart_send
* @brief     ：串口发送数据
* @param[in] ：int fd                句柄
               uint8* data           数据
               int len               长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_uart_send(int fd, uint8 *data, int len)
{
    int send_len = 0;  

    if(fd < 0 || NULL == data || 0 == len)
    {
        return -1;
    }

    send_len = write(fd, data, len);  
    return send_len == len ? 0 : -3;
}

/**********************************************************************
* @name      : rsp_uart_recv
* @brief     ：串口数据接收接口(等待)
* @param[in] ：int fd                句柄
               int nBufLen                  最大接收长度
               int rsptime                  数据返回时间(20ms)
               int rcvtime                  数据字节之间间隔时间(10ms)
* @param[out]：uint8* pBuffer               接收数据缓冲区
* @return    ：接收到数据的长度
* @Create    : 王津剑
* @Date      ：2020-3-10
* @Update    :
**********************************************************************/
int rsp_uart_recv(int fd, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime)
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
* @name      : rsp_uart_close
* @brief     ：串口关闭
* @param[in] ：int fd                句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-12-28
* @Update    :
**********************************************************************/
void rsp_uart_close(int fd)
{
    if(fd >= 0)
    {
        close(fd);
    }
}
#endif

#if DESC("控制接入状态", 1)
/**********************************************************************
* @name      : rsp_proc_msg_ctrl
* @brief     ：控制状态解析
* @param[in] ：uint8 *apdu            APDU
               uint16 len            长度
* @param[out]：uint16 *status         状态
               uint8 *num            数量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-7-29
* @Update    :
**********************************************************************/
int rsp_proc_msg_ctrl(uint8 *apdu, uint16 len, uint16 *status, uint8 *num)
{
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    uint8  i = 0;
    uint8  bitstatus = 0;
    
    type = apdu[0];
    temp = apdu + 1;
    mapmsg_memcpy_r(&dt, temp, sizeof(uint16));
    temp += sizeof(uint16);

    if(MAPMSG_DT_CTR_LOOP_STATUS != dt || MAPMSG_APDU_GetResponse != type)
    {
        return -1;
    }
    
    if(0 == *temp)
    {
        return -2;
    }
    else if(1 != *temp)
    {
        return -3;
    }
    temp++;

    *num = 0;
    *status = 0xFF;
    
    if(1 != *temp)//array
    {
        return -4;
    }
    else
    {
        temp++;
        *status = 0;
        *num = MIN(*temp, 16);
        temp++;
        for(i = 0; i < *num; i++)
        {
            temp++;
            bitstatus = *temp;
            temp++;
            if(0 != bitstatus)
            {
                *status |= (0x01 << i);
            }
        }
    }

    return 0;
}

#endif

/**********************************************************************
* @name      : rsp_send_link_comm
* @brief     ：发送链路协商
* @param[in] ：int fd                句柄
               uint8 index          序号
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2021-11-1
* @Update    :
**********************************************************************/
int rsp_send_link_comm(int fd, uint8 index)
{
    uint8          msgbuf[128] = {0};
    uint8          sendbuf[128] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    int            ret = 0;
    uint16         version = 1;
    uint16         sizeM = 256;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_LinkRequest;

    Temp[offset++] = 2;  //structure
    Temp[offset++] = 6;  //structure

    MEMCPY_R(Temp + offset, &version, 2);
    offset += 2;
    
    Temp[offset++] = strlen(PRODUCT_NAME);
    memcpy(Temp + offset, PRODUCT_NAME, strlen(PRODUCT_NAME));

    Temp[offset++] = strlen("00001111");
    memcpy(Temp + offset, "00001111", strlen("00001111"));

    MEMCPY_R(Temp + offset, &sizeM, 2);
    offset += 2;
    MEMCPY_R(Temp + offset, &sizeM, 2);
    offset += 2;
    Temp[offset++] = 1;

    pMsgInfo->len = offset;

    msglen = mapmsg_package(pMsgInfo, sendbuf, 128);

    ret = rsp_uart_send(fd, sendbuf, msglen);
    if(0 != ret)
    {
        RSP_BUF_DEBUG(sendbuf, msglen, "协商发送报文失败");
    }
    else
    {
        RSP_BUF_TRACE(sendbuf, msglen, "协商发送报文成功");
    }
    return ret;
}

/**********************************************************************
* @name      : rsp_send_report_resp
* @brief     ：上报应答帧
* @param[in] ：int fd                 句柄
               uint8 index           序号
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2023-6-16
* @Update    :
**********************************************************************/
int rsp_send_report_resp(int fd, uint8 index, uint16 dt)
{
    uint8          msgbuf[128] = {0};
    uint8          sendbuf[128] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    int            ret = 0;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_MODULE;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_ReportResponse;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    pMsgInfo->len = offset;

    msglen = mapmsg_package(pMsgInfo, sendbuf, 128);

    ret = rsp_uart_send(fd, sendbuf, msglen);
    if(0 != ret)
    {
        RSP_BUF_DEBUG(sendbuf, msglen, "应答%04X发送报文失败", dt);
    }
    else
    {
        RSP_BUF_TRACE(sendbuf, msglen, "应答%04X发送报文成功", dt);
    }
    return ret;
}

/**********************************************************************
* @name      : rsp_send_get_comm
* @brief     ：发送获取信息
* @param[in] ：int fd                 句柄
               uint8 index           序号
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2021-11-1
* @Update    :
**********************************************************************/
int rsp_send_get_comm(int fd, uint8 index, uint16 dt)
{
    uint8          msgbuf[128] = {0};
    uint8          sendbuf[128] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    int            ret = 0;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_GetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    pMsgInfo->len = offset;

    msglen = mapmsg_package(pMsgInfo, sendbuf, 128);

    ret = rsp_uart_send(fd, sendbuf, msglen);
    if(0 != ret)
    {
        RSP_BUF_DEBUG(sendbuf, msglen, "获取%04X发送报文失败", dt);
    }
    else
    {
        RSP_BUF_TRACE(sendbuf, msglen, "获取%04X发送报文成功", dt);
    }
    return ret;
}

/**********************************************************************
* @name      : rsp_send_set_config
* @brief     ：发送设置配置
* @param[in] ：int fd                 句柄
               uint8 index           序号
               uint8 *cfg            配置
               uint8 num             数量
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2021-7-26
* @Update    :
**********************************************************************/
int rsp_send_set_config(int fd, uint8 index, uint8 *cfg, uint8 num)
{
    uint8          msgbuf[128] = {0};
    uint8          sendbuf[128] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_PULSE_SET;
    uint8          i = 0;
    int            ret = 0;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_SetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 4;    //bit-string
    Temp[offset++] = num;  //长度
    for(i = 0; i < num; i++)
    {
        Temp[offset] |= ((cfg[i] & 0x01) << (7 - i));
    }
    offset++;
    pMsgInfo->len = offset;

    msglen = mapmsg_package(pMsgInfo, sendbuf, 128);
    ret = rsp_uart_send(fd, sendbuf, msglen);
    RSP_BUF_DEBUG(sendbuf, msglen, "设置遥信脉冲(index = %d)发送%s\n", index, 0 == ret ? "成功" : "失败");
    return ret;
}

/**********************************************************************
* @name      : rsp_send_set_config
* @brief     ：发送设置配置
* @param[in] ：int fd                句柄
               uint8 index           序号
               uint16 shaketime      时间设置
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2021-7-26
* @Update    :
**********************************************************************/
int rsp_send_set_shake(int fd, uint8 index, uint16 shaketime)
{
    uint8          msgbuf[128] = {0};
    uint8          sendbuf[128] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_PULSE_SETTIME;
    int            ret = 0;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_SetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 18;    //long-unsigned
    mapmsg_memcpy_r(Temp + offset, &shaketime, sizeof(uint16));
    offset += sizeof(uint16);
    pMsgInfo->len = offset;
    
    msglen = mapmsg_package(pMsgInfo, sendbuf, 128);
    ret = rsp_uart_send(fd, sendbuf, msglen);
    RSP_BUF_DEBUG(sendbuf, msglen, "设置遥信防抖时间(index = %d)发送%s\n", index, 0 == ret ? "成功" : "失败");
    return ret;
}

/**********************************************************************
* @name      : rsp_send_set_time
* @brief     ：发送设置时间
* @param[in] ：int fd                 句柄
               uint8 index            序号
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2021-10-28
* @Update    :
**********************************************************************/
int rsp_send_set_time(int fd, uint8 index)
{
    uint8          msgbuf[128] = {0};
    uint8          sendbuf[128] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_COMM_TIMER;
    int            ret = 0;
    struct  timeval  tv;
    struct  timezone tz;
    struct  tm       systime;
    OOP_DATETIME_T   curtime;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_SetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 25;    //data_time

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &systime);

    curtime.year   = systime.tm_year + 1900; 
    curtime.month  = systime.tm_mon + 1;
    curtime.mday = systime.tm_mday;
    curtime.wday = systime.tm_wday;
    curtime.hour = systime.tm_hour;
    curtime.minute = systime.tm_min;
    curtime.second = systime.tm_sec;
    curtime.msec = tv.tv_usec/1000;

    mapmsg_memcpy_r(Temp + offset, &(curtime.year), 2);
    offset += 2;
    Temp[offset++] = curtime.month;
    Temp[offset++] = curtime.mday;
    Temp[offset++] = curtime.wday;
    Temp[offset++] = curtime.hour;
    Temp[offset++] = curtime.minute;
    Temp[offset++] = curtime.second;
    mapmsg_memcpy_r(Temp + offset, &(curtime.msec), 2);
    offset += 2;
    pMsgInfo->len = offset;

    msglen = mapmsg_package(pMsgInfo, sendbuf, 128);
    ret = rsp_uart_send(fd, sendbuf, msglen);
    RSP_BUF_DEBUG(sendbuf, msglen, "设置时间(%4d-%02d-%02d %02d:%02d:%02d.%03d)发送%s\n", curtime.year, curtime.month, 
                                    curtime.mday, curtime.hour, curtime.minute, curtime.second, curtime.msec,
                                    0 == ret ? "成功" : "失败");
    return ret;
}

/**********************************************************************
* @name      : rsp_send_res_time
* @brief     ：发送应答时间
* @param[in] ：int fd                 句柄
               uint8 index            序号
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2021-10-29
* @Update    :
**********************************************************************/
int rsp_send_res_time(int fd, uint8 index)
{
    uint8          msgbuf[128] = {0};
    uint8          sendbuf[128] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_COMM_TIMER;
    int            ret = 0;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       systime;
    OOP_DATETIME_T   curtime;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_MODULE;
    pMsgInfo->cmt.prm = MAPMSG_PRM_MODULE;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_GetResponse;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 1;     //Data
    Temp[offset++] = 25;    //data_time

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &systime);

    curtime.year   = systime.tm_year + 1900; 
    curtime.month  = systime.tm_mon + 1;
    curtime.mday = systime.tm_mday;
    curtime.wday = systime.tm_wday;
    curtime.hour = systime.tm_hour;
    curtime.minute = systime.tm_min;
    curtime.second = systime.tm_sec;
    curtime.msec = tv.tv_usec/1000;

    mapmsg_memcpy_r(Temp + offset, &(curtime.year), 2);
    offset += 2;
    Temp[offset++] = curtime.month;
    Temp[offset++] = curtime.mday;
    Temp[offset++] = curtime.wday;
    Temp[offset++] = curtime.hour;
    Temp[offset++] = curtime.minute;
    Temp[offset++] = curtime.second;
    mapmsg_memcpy_r(Temp + offset, &(curtime.msec), 2);
    offset += 2;
    pMsgInfo->len = offset;

    msglen = mapmsg_package(pMsgInfo, sendbuf, 128);
    ret = rsp_uart_send(fd, sendbuf, msglen);
    RSP_BUF_DEBUG(sendbuf, msglen, "应答时间(%4d-%02d-%02d %02d:%02d:%02d.%03d)发送%s\n", curtime.year, curtime.month, 
                                    curtime.mday, curtime.hour, curtime.minute, curtime.second, curtime.msec,
                                    0 == ret ? "成功" : "失败");
    return ret;
}

/**********************************************************************
* @name      : rsp_send_set_file
* @brief     ：发送设置文件
* @param[in] ：int fd                 句柄
               uint8 index            序号
               uint16 curNO           当前序号
               uint16 maxNo           最大序号
               uint8 *data            数据
               uint16 len             长度 (按照最大256设计)
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2021-10-28
* @Update    :
**********************************************************************/
int rsp_send_set_file(int fd, uint8 index, uint32 curNO, uint32 maxNo, uint8 *data, uint16 len)
{
    uint8          msgbuf[312] = {0};
    uint8          sendbuf[312] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_COMM_FILETRAN;
    int            ret = 0;
    uint8          byte[3] = {0};
    uint8          bytenum = 0;

    if(NULL == data || 0 == len)
    {
        return -2;
    }
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_SetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 2;    //structure
    Temp[offset++] = 4;    //数量

    Temp[offset++] = 17;  //unsigned
    Temp[offset++] = 0;                 //文件标识
    
    Temp[offset++] = 6;  //double long unsigned
    MEMCPY_R(Temp + offset, &curNO, 4); //段序号
    offset += 4;
 
    Temp[offset++] = 6;  //double long unsigned
    MEMCPY_R(Temp + offset, &maxNo, 4); //总段数
    offset += 4;

    Temp[offset++] = 9;  //octet-string
    SET_LEN_OFFSET(len, byte, bytenum);
    memcpy(Temp + offset, byte, bytenum);
    offset += bytenum;

    memcpy(Temp + offset, data, len);
    offset += len;

    pMsgInfo->len = offset;

    msglen = mapmsg_package(pMsgInfo, sendbuf, 312);
    ret = rsp_uart_send(fd, sendbuf, msglen);
    UPDATE_BUF_DEBUG(sendbuf, msglen, "文件传输(序号 %d, 总数 %d)发送%s\n", curNO, maxNo, 0 == ret ? "成功" : "失败");
    return ret;
}

/**********************************************************************
* @name      : rsp_proc_msg_rmt
* @brief     ：遥信状态解析
* @param[in] ：uint8 *apdu            APDU
               uint16 len             长度
* @param[out]：RSP_RMT_D_T *ptRmt     遥信状态
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-26
* @Update    :
**********************************************************************/
int rsp_proc_msg_rmt(uint8 *apdu, uint16 len, RSP_RMT_D_T *ptRmt)
{
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    uint8  i = 0;
    uint8  dre = 7;
    
    type = apdu[0];
    temp = apdu + 1;
    mapmsg_memcpy_r(&dt, temp, sizeof(uint16));
    temp += sizeof(uint16);

    if(MAPMSG_DT_PULSE_STATUS != dt || (MAPMSG_APDU_GetResponse != type && MAPMSG_APDU_ReportNotification != type))
    {
        return -1;
    }
    
    if(0 == *temp)
    {
        return -2;
    }
    else if(1 != *temp)
    {
        return -3;
    }
    temp++;

    memset(ptRmt, 0, sizeof(RSP_RMT_D_T));
    /* 解析 */
    if(2 != *temp || 3 != *(temp + 1))  //structurt [2]
    {
        return -4;
    }
    temp += 3;  //structure + 元素个数 + bit-string [4]
    ptRmt->stateNum = MIN(RSP_PORT_MAX, *temp);
    
    temp += 1;  //长度(内容为4)
    for(i = 0; i < ptRmt->stateNum; i++)
    {
        if(8 == i)
        {
            dre = 15;
            temp++;
        }
        ptRmt->ST[i] = ((*temp >> (dre - i)) & 0x01);
    }
    
    temp += 2;  //值 + bit-string [4]
    ptRmt->changeNum = MIN(RSP_PORT_MAX, *temp);

    temp += 1;  //长度(内容为4)
    dre = 7;
    for(i = 0; i < ptRmt->stateNum; i++)
    {
        if(8 == i)
        {
            dre = 15;
            temp++;
        }
        ptRmt->CT[i] = ((*temp >> (dre - i)) & 0x01);
    }

    temp += 2;  //值 + array [1]
    ptRmt->timeNum = MIN(RSP_PORT_MAX, *temp);

    temp += 1;  //元素个数
    for(i=0; i< ptRmt->timeNum; i++)
    {
        if(0 == *temp) //NULL [0]
        {
            temp += 1;
            ptRmt->validFlag[i] = 0;//时间无效
            continue;
        }
        if(25 == *temp) //date_time [25]
        {
            temp++;
            
            mapmsg_memcpy_r(&(ptRmt->changeTime[i].year), temp, 2);
            temp += 2;
            ptRmt->changeTime[i].month = *temp;
            temp++;
            ptRmt->changeTime[i].mday = *temp;
            temp++;
            ptRmt->changeTime[i].wday = *temp;
            temp++;
            ptRmt->changeTime[i].hour = *temp;
            temp++;
            ptRmt->changeTime[i].minute = *temp;
            temp++;
            ptRmt->changeTime[i].second = *temp;
            temp++;
            mapmsg_memcpy_r(&(ptRmt->changeTime[i].msec), temp, 2);
            temp += 2;
            ptRmt->validFlag[i] = 1;//时间有效
        }
        else
        {
            return -5;
        }
    }

    return 0;
}

/**********************************************************************
* @name      : rsp_proc_msg_pulse_d
* @brief     ：脉冲间隔解析
* @param[in] ：uint8 *apdu              APDU
               uint16 len               长度
* @param[out]：RSP_PULSE_A_T *ptPulseA  脉冲统计数量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-16
* @Update    :
**********************************************************************/
int rsp_proc_msg_pulse_d(uint8 *apdu, uint16 len, RSP_PULSE_D_T *ptPulseD)
{
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    uint8  i = 0;
    
    type = apdu[0];
    temp = apdu + 1;
    mapmsg_memcpy_r(&dt, temp, sizeof(uint16));
    temp += sizeof(uint16);

    if(MAPMSG_DT_PULSE_LASTDATA != dt || MAPMSG_APDU_GetResponse != type)
    {
        return -1;
    }
    
    if(0 == *temp)
    {
        return -2;
    }
    else if(1 != *temp)
    {
        return -3;
    }
    temp++;

    memset(ptPulseD, 0, sizeof(RSP_PULSE_D_T));
    /* 解析 */
    if(1 != *temp)  //array [1]
    {
        return -4;
    }
    temp++;
    
    ptPulseD->num= MIN(RSP_PORT_MAX, *temp); //数组个数
    temp++;
    
    for(i = 0; i < ptPulseD->num; i++)
    {
        ptPulseD->validFlag[i] = 0;
        if(0 == *temp)
        {
            temp++;
            continue;
        }
        if(2 != *temp || 2 != *(temp + 1))  //structurt [2]
        {
            return -5;
        }

        temp += 3;  //structure + 元素个数 + double-long-unsigned
        mapmsg_memcpy_r(&ptPulseD->nCount[i], temp, sizeof(uint32));
        temp += sizeof(uint32);
        temp += 1; //double-long-unsigned
        mapmsg_memcpy_r(&ptPulseD->nTimeMS[i], temp, sizeof(uint32));
        temp += sizeof(uint32);
        ptPulseD->validFlag[i] = 1;
    }

    return 0;
}


/**********************************************************************
* @name      : rsp_proc_msg_pulasea
* @brief     ：脉冲统计解析
* @param[in] ：uint8 *apdu              APDU
               uint16 len               长度
* @param[out]：RSP_PULSE_A_T *ptPulseA  脉冲统计数量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-26
* @Update    :
**********************************************************************/
int rsp_proc_msg_pulasea(uint8 *apdu, uint16 len, RSP_PULSE_A_T *ptPulseA)
{
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    uint8  i = 0;
    
    type = apdu[0];
    temp = apdu + 1;
    mapmsg_memcpy_r(&dt, temp, sizeof(uint16));
    temp += sizeof(uint16);

    if(MAPMSG_DT_PULSE_TOTALDATA != dt || MAPMSG_APDU_GetResponse != type)
    {
        return -1;
    }
    
    if(0 == *temp)
    {
        return -2;
    }
    else if(1 != *temp)
    {
        return -3;
    }
    temp++;

    memset(ptPulseA, 0, sizeof(RSP_PULSE_A_T));
    /* 解析 */
    if(1 != *temp)  //array [1]
    {
        return -4;
    }
    temp++;
    
    ptPulseA->num= MIN(RSP_PORT_MAX, *temp); //数组个数
    temp++;
    
    for(i = 0; i < ptPulseA->num; i++)
    {
        if(0 == *temp)
        {
            temp++;
            continue;
        }
        if(2 != *temp || 4 != *(temp + 1))  //structurt [2]
        {
            return -5;
        }
        temp += 3;  //structure + 元素个数 + double-long-unsigned
        mapmsg_memcpy_r(&ptPulseA->nCount[i], temp, sizeof(uint32));
        temp += sizeof(uint32);
        temp += 1; //double-long-unsigned
        mapmsg_memcpy_r(&ptPulseA->nTimeH[i], temp, sizeof(uint32));
        temp += sizeof(uint32);
        temp += 1; //double-long-unsigned
        mapmsg_memcpy_r(&ptPulseA->nTimeL[i], temp, sizeof(uint16));
        temp += sizeof(uint16);
        temp += 1; //date_time
        mapmsg_memcpy_r(&(ptPulseA->startTime[i].year), temp, 2);
        temp += 2;
        ptPulseA->startTime[i].month = *temp;
        temp++;
        ptPulseA->startTime[i].mday = *temp;
        temp++;
        ptPulseA->startTime[i].wday = *temp;
        temp++;
        ptPulseA->startTime[i].hour = *temp;
        temp++;
        ptPulseA->startTime[i].minute = *temp;
        temp++;
        ptPulseA->startTime[i].second = *temp;
        temp++;
        mapmsg_memcpy_r(&(ptPulseA->startTime[i].msec), temp, 2);
        temp += 2;
        ptPulseA->validFlag[i] = 1;
    }

    return 0;
}

/**********************************************************************
* @name      : rsp_proc_msg_mapinfo
* @brief     ：模组信息解析
* @param[in] ：uint8 *apdu              APDU
               uint16 apdulen           长度
* @param[out]：RSP_MAP_INFO_T *ptMap    模组信息
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-11-1
* @Update    :
**********************************************************************/
int rsp_proc_msg_mapinfo(uint8 *apdu, uint16 apdulen, RSP_MAP_INFO_T *ptMap)
{
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    uint16 offset = 0;
    uint8  len = 0;
    
    type = apdu[0];
    temp = apdu + 1;
    mapmsg_memcpy_r(&dt, temp, sizeof(uint16));
    temp += sizeof(uint16);

    if(MAPMSG_DT_COMM_MODINFO != dt || MAPMSG_APDU_GetResponse != type)
    {
        return -1;
    }
    
    if(0 == *temp)
    {
        return -2;
    }
    else if(1 != *temp)
    {
        return -3;
    }
    temp++;

    memset(ptMap, 0, sizeof(RSP_MAP_INFO_T));
    /* 解析 */
    if(2 != temp[offset++])  //structure [2]
    {
        return -4;
    }
    if(8 != temp[offset++])  //数量
    {
        return -4;
    }

    if(10 != temp[offset++])  //visible-string [2]
    {
        return -5;
    }
    len = temp[offset++];
    memcpy(ptMap->modeltype, temp + offset, MIN(127, len));
    offset += len;

    if(10 != temp[offset++])  //visible-string [2]
    {
        return -5;
    }
    len = temp[offset++];
    memcpy(ptMap->ID, temp + offset, MIN(127, len));
    offset += len;
        
    if(10 != temp[offset++])  //visible-string [2]
    {
        return -5;
    }
    len = temp[offset++];
    if(4 != len)
    {
        return -6;
    }
    memcpy(ptMap->manuNum, temp + offset, len);
    offset += len;

    if(10 != temp[offset++])  //visible-string [2]
    {
        return -5;
    }
    len = temp[offset++];
    if(4 != len)
    {
        return -6;
    }
    memcpy(ptMap->softversion, temp + offset, len);
    offset += len;

    if(10 != temp[offset++])  //visible-string [2]
    {
        return -5;
    }
    len = temp[offset++];
    if(6 != len)
    {
        return -6;
    }
    memcpy(ptMap->softdata, temp + offset, len);
    offset += len;

    if(10 != temp[offset++])  //visible-string [2]
    {
        return -5;
    }
    len = temp[offset++];
    if(4 != len)
    {
        return -6;
    }
    memcpy(ptMap->hwversion, temp + offset, len);
    offset += len;

    if(10 != temp[offset++])  //visible-string [2]
    {
        return -5;
    }
    len = temp[offset++];
    if(6 != len)
    {
        return -6;
    }
    memcpy(ptMap->hwdata, temp + offset, len);
    offset += len;

    if(10 != temp[offset++])  //visible-string [2]
    {
        return -5;
    }
    len = temp[offset++];
    if(8 != len)
    {
        return -6;
    }
    memcpy(ptMap->manuExten, temp + offset, len);
    offset += len;
    
    return 0;
}

/**********************************************************************
* @name      : rsp_proc_msg_linkinfo
* @brief     ：链路协商信息解析
* @param[in] ：uint8 *apdu              APDU
               uint16 apdulen           长度
* @param[out]：RSP_LINK_INFO_T *ptLink  链路信息
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-11-1
* @Update    :
**********************************************************************/
int rsp_proc_msg_linkinfo(uint8 *apdu, uint16 apdulen, RSP_LINK_INFO_T *ptLink)
{
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 offset = 0;
    uint8  len = 0;
    
    type = apdu[0];
    temp = apdu + 1;

    if(MAPMSG_APDU_LinkResponse != type)
    {
        return -1;
    }

    memset(ptLink, 0, sizeof(RSP_LINK_INFO_T));
    /* 解析 */
    MEMCPY_R(&(ptLink->version), temp + offset, 2);
    offset += 2;

    len = temp[offset++];
    memcpy(ptLink->modeltype, temp + offset, MIN(127, len));
    offset += len;

    len = temp[offset++];
    memcpy(ptLink->ID, temp + offset, MIN(127, len));
    offset += len;
        
    MEMCPY_R(&(ptLink->maxSend), temp + offset, 2);
    offset += 2;

    MEMCPY_R(&(ptLink->maxrecv), temp + offset, 2);
    offset += 2;

    ptLink->size = temp[offset++];
    ptLink->num = temp[offset++];
    ptLink->num = MIN(ptLink->num, RSP_CHANNEL_MAX);

    /* 虚拟通道目前没用到 */
    
    return 0;
}

/**********************************************************************
* @name      : rsp_proc_msg_485can
* @brief     ：485-2工作模式
* @param[in] ：uint8 *apdu              APDU
               uint16 len              长度
* @param[out]：uint8 *ptype             模式
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-12-14
* @Update    :
**********************************************************************/
int rsp_proc_msg_485can(uint8 *apdu, uint16 len, uint8 *ptype)
{
    uint8 *temp = NULL;
    uint8  type = 0;
    uint16 dt = 0;
    
    type = apdu[0];
    temp = apdu + 1;
    mapmsg_memcpy_r(&dt, temp, sizeof(uint16));
    temp += sizeof(uint16);

    if(MAPMSG_DT_485_CAN_STATUS != dt || MAPMSG_APDU_GetResponse != type)
    {
        return -1;
    }
    
    if(0 == *temp)
    {
        return -2;
    }
    else if(1 != *temp)
    {
        return -3;
    }
    temp++;

    /* 解析 */
    if(22 != *temp)  //enum [22]
    {
        return -4;
    }
    temp++;
    
    *ptype = *temp;
    temp++;

    return 0;
}

/**********************************************************************
* @name      : rsp_send_set_config
* @brief     ：发送设置配置
* @param[in] ：int fd                 句柄
               uint8 index           序号
               uint8 type            类型
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 王津剑
* @Date      ：2022-12-14
* @Update    :
**********************************************************************/
int rsp_send_set_485can(int fd, uint8 index, uint8 type)
{
    uint8          msgbuf[128] = {0};
    uint8          sendbuf[128] = {0};
    uint16         msglen = 0;
    MAPMSG_INFO_T *pMsgInfo = (MAPMSG_INFO_T *)msgbuf;
    uint8         *Temp = pMsgInfo->apdu;
    uint16         offset = 0;
    uint16         dt = MAPMSG_DT_485_CAN_STATUS;
    int            ret = 0;
    
    pMsgInfo->cmt.dir = MAPMSG_DIR_TERMINAL;
    pMsgInfo->cmt.prm = MAPMSG_PRM_TERMINAL;
    pMsgInfo->cmt.cmd = MAPMSG_CTR_MSG;
    pMsgInfo->cmt.fid = index;
    
    Temp[offset++] = MAPMSG_APDU_SetRequest;
    mapmsg_memcpy_r(Temp + offset, &dt, sizeof(uint16));
    offset += sizeof(uint16);
    Temp[offset++] = 22;    //enum
    Temp[offset++] = type;
    pMsgInfo->len = offset;

    msglen = mapmsg_package(pMsgInfo, sendbuf, 128);
    ret = rsp_uart_send(fd, sendbuf, msglen);
    RSP_BUF_DEBUG(sendbuf, msglen, "设置485-2口工作模式(index = %d)发送%s\n", index, 0 == ret ? "成功" : "失败");
    return ret;
}


