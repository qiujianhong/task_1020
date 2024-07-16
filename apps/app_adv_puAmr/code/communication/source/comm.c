/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块通用函数
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include <netdb.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <linux/tcp.h>

#include "comm.h"

/**********************************************************************
* @name      : comm_clean_queue
* @brief     ：清队列
* @param[in] ：pQueue  缓冲区结构指针
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-22
* @Update    :
**********************************************************************/
void comm_clean_queue(COMM_QUEUE_LIST_T *pQueue)
{
    pthread_mutex_lock(&pQueue->f_lock);
    pQueue->head = 0;
    pQueue->tail = 0;
    memset(&(pQueue->queue[0]), 0, QUEUE_SIZE * sizeof(COMM_QUEUE_T));
    pthread_mutex_unlock(&pQueue->f_lock);
}

/**********************************************************************
* @name      : comm_write_queue
* @brief     ：读队列
* @param[in] ：pQueue  缓冲区结构指针
* @param[out]：pBuf    读数据缓冲区
* @return    ：数据长度
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
uint32 comm_read_queue(COMM_QUEUE_LIST_T *pQueue, uint8 *pBuf)
{
    uint32 len;
    uint8 *pdataBuf;
    
    pthread_mutex_lock(&pQueue->f_lock);
    len = 0;

    /*若队列中有数据，则取到buf中，解开锁*/
    if ((pQueue->head != pQueue->tail)||((pQueue->head == pQueue->tail)&&(pQueue->queue[pQueue->head].len != 0)))
    {
        pdataBuf = pQueue->queue[pQueue->head].buf_queue;
        len = pQueue->queue[pQueue->head].len;
        memcpy(pBuf, pdataBuf, len);  /*取队列中的数据*/
        memset(pdataBuf, 0, len);     /*清数据*/
        pQueue->queue[pQueue->head].len = 0;
        pQueue->head = (pQueue->head+1) % QUEUE_SIZE; /*队列头向后移动一位*/
    }

    pthread_mutex_unlock(&pQueue->f_lock);
    return len;
}

/**********************************************************************
* @name      : comm_write_queue
* @brief     ：写队列, 若队列满，丢弃
* @param[in] ：pQueue  缓冲区结构指针
               pBuf    需要写入队列的数据
               bufLen  数据长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
int comm_write_queue(COMM_QUEUE_LIST_T *pQueue, uint8 *pBuf, uint32 bufLen)
{
    int flags;
    flags = -1;
    pthread_mutex_lock(&pQueue->f_lock);

   /*判队列中是否还有空的位置*/
    if (pQueue->queue[pQueue->tail].len == 0)
    {
        pQueue->queue[pQueue->tail].len = bufLen;
        memcpy(pQueue->queue[pQueue->tail].buf_queue, pBuf, bufLen);
        flags = 0;
        pQueue->tail = (pQueue->tail+1) % QUEUE_SIZE;
    }
    pthread_mutex_unlock(&pQueue->f_lock);
    return flags;
}

/**********************************************************************
* @name      : comm_ifnet_exist
* @brief     ：查看指定的网口是否存在
* @param[in] ：char *pNetName    网口名称
* @return    ：TRUE-存在/FALSE-不存在
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
BOOLEAN comm_ifnet_exist(const char *pNetName)
{
    if (0 == if_nametoindex(pNetName))
    {
        return FALSE;
    }  
    return TRUE;
}

/**********************************************************************
* @name      : comm_get_curtime_s
* @brief     ：获取系统时间, 精确到s
* @param[in] ：
* @return    ：COMM_STRUCT_T *pTime
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
void comm_get_curtime_s(COMM_STRUCT_T *pTime)
{
    time_t now;
    struct tm *systime;
    time(&now);
    systime = localtime(&now);
    pTime->milliseconds = 0;
    pTime->second = (uint8)systime->tm_sec;
    pTime->minute = (uint8)systime->tm_min;
    pTime->hour = (uint8)systime->tm_hour;
    pTime->day = (uint8)systime->tm_mday;
    pTime->month = (uint8)systime->tm_mon;         /*月份0代表1月*/
    pTime->year = (uint8)systime->tm_year;         /*年份0代表1900年*/
}

/**********************************************************************
* @name      : comm_add_channel
* @brief     ：将buf中的数据，按所在通道，加上会话协议头部
* @param[in] ：channel   通道编号
               buf       存放数据缓冲区指针
               len       数据长度
* @param[out]：buf       加上会话协议的数据
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
void comm_add_channel(uint8 channel, uint8 *buf, uint16 len)
{
    /* 第0个字节是通道好, 第2个字节是内部通信协议, 第2/3个字节是长度 */
    memmove(buf+4, buf, len);
    buf[0] = channel;
    buf[1] = 0;
    buf[2] = len%256;
    buf[3] = len/256;
}

/**********************************************************************
* @name      : comm_find_bigchannel
* @brief     ：判缓冲区中的数据属于哪个通道大类
* @param[in] ：buf   指向存放数据的缓冲区的指针
* @param[out]：
* @return    ：通道大类
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
uint8 comm_get_bigchannel(uint8 *buf)
{
    if(buf[0] <= CHL_MANUAL_AMR)
    {
        return buf[0];
    }

    if(LOCATE_BLUE <= buf[0] && buf[0] <= LOCATE_BLUE + 1)
    {
        return LOCATE_BLUE;
    }
    
    if(CHL_ACMETER == buf[0])
    {
        return CHL_ACMETER;
    }

    /* 多通道类型 */
    if(CHL_GPRS_1 <= buf[0] && buf[0] < CHL_GSM_1)
    {
        return CHL_GPRS_1;
    }
    else if(CHL_GSM_1 <= buf[0] && buf[0] < CHL_GSM_1 + 4)
    {
        return CHL_GSM_1;
    }
    if(CHL_GPRS_2 <= buf[0] && buf[0] < CHL_GSM_2)
    {
        return CHL_GPRS_2;
    }
    else if(CHL_GSM_2 <= buf[0] && buf[0] < CHL_GSM_2 + 4)
    {
        return CHL_GSM_2;
    }
    else if(CHL_ETH_1 <= buf[0] && buf[0] < CHL_ETH_2)
    {
        return CHL_ETH_1;
    }
    else if(CHL_ETH_2 <= buf[0] && buf[0] < CHL_RESERVE)
    {
        return CHL_ETH_2;
    }
    
    return CHL_RESERVE;
}

/**********************************************************************
* @name      : comm_get_protocol
* @brief     ：获取内部通信协议
* @param[in] ：uint8 *buf  消息头
* @param[out]：
* @return    ：内部通信协议
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
uint8 comm_get_protocol(uint8 *buf)
{
  	return buf[1];
}

/**********************************************************************
* @name      : comm_get_ifflux
* @brief     ：统计网口的流量
* @param[in] ：char *ifName    网口名称
* @param[out]：uint64 *rxbytes 接收流量
               uint64 *txbytes 发送流量
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-2
* @Update    :
**********************************************************************/
int comm_get_ifflux(char *ifName, uint64 *rxbytes, uint64 *txbytes)
{
    char *temp;
    FILE *fh;
    char buf[512];
    IF_STATE_T tIfState = {0};

    if (NULL == ifName || NULL == rxbytes || NULL == txbytes)
    {
    	return -1;
    }
    fh = fopen(PATH_PROCNET_DEV, "r");
    if (!fh)
    {
        COMM_FMT_TRACE("%s fopen PATH_PROCNET_DEV", ifName);
        return -1;
    }

    fgets(buf, sizeof buf, fh);
    fgets(buf, sizeof buf, fh);

    while (fgets(buf, sizeof buf, fh))
    {
        if (strstr(buf, ifName))
        {
          	temp = strstr(buf, ":");
          	if (temp)
          	{
                sscanf(temp+1,
                "%llu %u %u %u %u %u %u %u %llu %u %u %u %u %u %u %u",
                &tIfState.rx_bytes,
                &tIfState.rx_packets,
                &tIfState.rx_errors,
                &tIfState.rx_dropped,
                &tIfState.rx_fifo_errors,
                &tIfState.rx_frame_errors,
                &tIfState.rx_compressed,
                &tIfState.rx_multicast,

                &tIfState.tx_bytes,
                &tIfState.tx_packets,
                &tIfState.tx_errors,
                &tIfState.tx_dropped,
                &tIfState.tx_fifo_errors,
                &tIfState.collisions,
                &tIfState.tx_carrier_errors,
                &tIfState.tx_compressed);
            }
        }
    }
    fclose(fh);
    *rxbytes = tIfState.rx_bytes;
    *txbytes = tIfState.tx_bytes;
    return 0;
}

/**********************************************************************
* @name      : comm_listen_socket
* @brief     ：创建一个端口监听
* @param[in] ：uint16 port      端口号
* @param[out]：
* @return    ：socket句柄
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
int comm_listen_socket(uint16 port)
{
	int listenfd;  
	struct sockaddr_in servaddr;
	int flags_block;
	int val;
	int opt = 1; 

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
	{
		COMM_FMT_TRACE("get socket error\n");
		return -1;
	}

    /* 设置SOCKET文件句柄为非阻塞模式 */
    if((flags_block = fcntl(listenfd, F_GETFL, 0))<0) 
    {   
        COMM_FMT_TRACE("ERROR F_GETFL\n");
        close(listenfd);
        return -1;
    }   
    
    if(fcntl(listenfd, F_SETFL, flags_block | O_NONBLOCK))
    {   
        COMM_FMT_TRACE("ERROR set mode\n"); 
        close(listenfd);
        return -1;
    }
    

    /* 关闭子进程无用文件描述符 */
    val = fcntl(listenfd, F_GETFD);
    if (val == -1)
    {
        COMM_FMT_TRACE("ERROR F_GETFD\n"); 
        close(listenfd);
        return -1;
    }
    
    val |= FD_CLOEXEC;
    if (fcntl(listenfd, F_SETFD, val) == -1)
    {
        COMM_FMT_TRACE("ERROR set F_SETFD\n"); 
        close(listenfd);
        return -1;
    }


	//SO_REUSEADDR 允许在bind（）过程中本地地址可重复使用
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    {
         COMM_FMT_TRACE("setsockopt error SO_REUSEADDR\n");
         return -1;
    }

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(port);
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0)
	{
		close(listenfd);
		COMM_FMT_TRACE("bind error\n");
        return -1;
	}
	if (listen(listenfd, 10) < 0)
	{
		close(listenfd);
		COMM_FMT_TRACE("listen error\n");
		return -1;
	}
	
	return listenfd;
}

/**********************************************************************
* @name      : comm_accept_socket
* @brief     ：端口监听
* @param[in] ：int listenfd    监听句柄
* @param[out]：uint32 *pIP     客户端IP
               uint16 *pPort   客户端Port
* @return    ：客户端句柄
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
int comm_accept_socket(int listenfd, uint32 *pIP, uint16 *pPort)
{
    int connect_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(struct sockaddr);
    int   flags_block;
    int   val;

    if (listenfd <= 0)
    {
        return -1;
    }

    connect_fd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len);

    if (connect_fd < 0)
    {
        return -1;
    }

    /* 关闭子进程无用文件描述符 */
    val = fcntl(connect_fd, F_GETFD);
    if (val == -1)
    {
        COMM_FMT_TRACE("ERROR F_GETFD\n"); 
        close(connect_fd);
        return -1;
    }
    
    val |= FD_CLOEXEC;
    if (fcntl(connect_fd, F_SETFD, val) == -1)
    {
        COMM_FMT_TRACE("ERROR set F_SETFD\n"); 
        close(connect_fd);
        return -1;
    }


    /* 设置SOCKET文件句柄为非阻塞模式 */
    if((flags_block = fcntl(connect_fd, F_GETFL, 0))<0) 
    {	
        COMM_FMT_TRACE("ERROR set mode\n");
        close(connect_fd);
        return -1; 
    }				
    if(fcntl(connect_fd, F_SETFL, flags_block | O_NONBLOCK))
    {	
        COMM_FMT_TRACE("ERROR set mode\n"); 
        close(connect_fd);
        return -1;
    }

    *pIP   = ntohl(client_addr.sin_addr.s_addr);
    *pPort = ntohs(client_addr.sin_port);
    return connect_fd;
}

/**********************************************************************
* @name      : comm_init_udp
* @brief     ：初始化UDP端口, 指定UDP端口为UDP_PORT
* @param[in] ：char *ifName      接口名称
* @param[out]：
* @return    ：socket
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
int comm_init_udp(char *ifName)
{
    int    flags_block;
    int    fd_sock = -1;
    uint16 udp_port;
    int    bind_err;
    struct sockaddr_in sin;
    //struct ifreq interf;

    /*分配socket标识符, UDP协议*/
    if ((fd_sock = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        COMM_FMT_TRACE("%s init_udp_socket socket error\n", ifName);
        return -1;
    }

    /* socket绑定网口 */
    /*bzero(&interf, sizeof(interf));
    strncpy(interf.ifr_ifrn.ifrn_name, ifName, strlen(ifName));
    if (setsockopt(fd_sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interf, sizeof(interf)) < 0) 
    {
        COMM_FMT_TRACE("%s SO_BINDTODEVICE failed\n", ifName);
        return -1;
    }*/
    
    /*将fd_sock 与sin结构中的地址进行绑定*/
    bzero(&sin, sizeof(sin));
    udp_port = DEF_UDP_PORT;
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port        = htons(udp_port);
    bind_err = bind(fd_sock, (struct sockaddr *)&sin, sizeof(sin));

    /*若UDP_PORT端口已经被使用了, 则重新绑定UDP_PORT+1端口, 直到端口号为5500*/
    while(bind_err < 0)
    {
        COMM_FMT_TRACE("%s init_udp_socket bind error\n", ifName);
        sin.sin_port = htons(++udp_port);
        bind_err = bind(fd_sock, (struct sockaddr *)&sin, sizeof(sin));
        if (udp_port > DEF_UDP_PORT_MAX)
        {
            return -1;
        }
    }

    /*绑定端口号成功后, 将fd_sock设置为非阻塞模式*/
    if (fd_sock > 0)
    {
        /* 设置SOCKET文件句柄为非阻塞模式 */
        if((flags_block = fcntl(fd_sock, F_GETFL, 0))<0) 
        {	
            COMM_FMT_TRACE("%s fcntl ERROR get mode\n", ifName);
            close(fd_sock);
            return -1; 
        }				
        if(fcntl(fd_sock, F_SETFL, flags_block | O_NONBLOCK))
        {
            COMM_FMT_TRACE("%s fcntl ERROR set mode\n", ifName); 
            close(fd_sock);
            return -1;
        }
    }

    return fd_sock;
}

/**********************************************************************
* @name      : comm_connect_delay
* @brief     ：与前置机进行连接
* @param[in] ：int fd                      句柄
               struct sockaddr *servaddr   目的
               socklen_t socklen           长度
               int delay                   延时
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
int comm_connect_delay(int fd, struct sockaddr *servaddr, socklen_t socklen, int delay)
{
    int ret = -1;
    int error = -1;
    int len;
    int flags_old;
    struct timeval tm;
    fd_set set;

    len = sizeof(socklen_t);

    /* 设置SOCKET文件句柄为非阻塞模式*/
    if((flags_old = fcntl(fd, F_GETFL, 0))<0) 
    {	
        COMM_FMT_TRACE("ERROR set mode");
        return -1; 
    }

    if(fcntl(fd, F_SETFL, flags_old | O_NONBLOCK))
    {	
        COMM_FMT_TRACE("ERROR set mode"); 
        return -2;
    }

    if(-1 == connect(fd, servaddr, socklen))
    {
        tm.tv_sec = delay;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        if(select(fd+1, NULL, &set, NULL, &tm) > 0)
        {
            if(FD_ISSET(fd, &set))
            {
                getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
                if(error == 0) 
                {
                    ret = 0;
                }
                else
                {
                    COMM_FMT_TRACE("ERROR error %d\n", error); 
                    ret = error;
                }	    		
            }
            else
            {
                COMM_FMT_TRACE("ERROR FD_ISSET\n"); 
                ret = -3;
            }
        }
        else
        {
            ret = -4;
            COMM_FMT_TRACE("ERROR select\n"); 
        }
    }
    else 
        ret = 0;

    /*设置fd模式*/
    if(fcntl(fd, F_SETFL, flags_old))
    {	
        COMM_FMT_TRACE("ERROR set mode"); 
        return -5;
    }
    
    return ret;
}

/**********************************************************************
* @name      : comm_connect_socket
* @brief     ：与前置机进行网口连接
* @param[in] ：char *ifName      接口名称
               uint32 IP         IP
               uint16 Port       PORT
* @param[out]：
* @return    ：socket
* @Create    : 王津剑
* @Date      ：2019-11-1
* @Update    :
**********************************************************************/
int comm_connect_socket(char *ifName, uint32 IP, uint16 Port)
{
    struct hostent *serverName;
    struct sockaddr_in pin;
    int   fd_socket;
    int   flags_block;
    //struct ifreq interf;
    char  hostName[24] = {0};
    int ret = 0;

    sprintf(hostName, "%u.%u.%u.%u", (IP>>24)&0xff, (IP>>16)&0xff, (IP>>8)&0xff, IP&0xff);
    /*获得主机信息*/
    if((serverName = gethostbyname(hostName)) == 0)	
    {
        COMM_FMT_DEBUG("%s Error resolving local host %s.\n", ifName, hostName);		
        return -1;
    }

    if(NULL == serverName->h_addr)
    {
        COMM_FMT_DEBUG("%s find no host %s.\n", ifName, hostName);        
        return -2;
    }

    /*获取与主机进行套接口连接的socket*/	
    if((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {	
        COMM_FMT_TRACE("%s Error connecting to socket\n", ifName);
        return -3;
    }
    
    /* socket绑定网口 */
    /*bzero(&interf, sizeof(interf));
    strncpy(interf.ifr_ifrn.ifrn_name, ifName, strlen(ifName));
    if (setsockopt(fd_socket, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interf, sizeof(interf)) < 0) 
    {
        COMM_FMT_TRACE("%s SO_BINDTODEVICE failed\n", ifName);
        return -1;
    }*/

    /*设置sockaddr_in参数*/		
    bzero(&pin, sizeof(pin));
    pin.sin_family      = AF_INET;
    pin.sin_addr.s_addr = ((struct in_addr *)(serverName->h_addr))->s_addr;
    pin.sin_port        = htons(Port);

    /*连接前置机*/
    ret = comm_connect_delay(fd_socket, (struct sockaddr *)&pin, (socklen_t)sizeof(pin), 16);
    if(0 != ret)
    {	
        close(fd_socket);
        COMM_FMT_DEBUG("%s连接%s:%u失败, err = %d(%s)\n", ifName, hostName, Port, ret, 
                                                          ret > 0 ? strerror(ret) : "other error");
        return -4; 
    }

    /* 设置SOCKET文件句柄为非阻塞模式 */
    if((flags_block = fcntl(fd_socket, F_GETFL, 0))<0) 
    {	
        COMM_FMT_TRACE("%s ERROR F_GETFL\n", ifName);
        close(fd_socket);
        return -5; 
    }	

    if(fcntl(fd_socket, F_SETFL, flags_block | O_NONBLOCK))
    {	
        COMM_FMT_TRACE("%s ERROR set mode\n", ifName); 
        close(fd_socket);
        return -6;
    }

    return fd_socket;	
}

/**********************************************************************
* @name      : comm_connect_socket_noblock
* @brief     ：与前置机进行网口连接(非阻塞), 不能间隔太长时间调度
* @param[in] ：char *ifName      网口名称
               uint32 IP         IP
               uint16 Port       PORT
               int *tempfd       临时句柄
               uint32 *startTime 开始时间
* @param[out]：
* @return    ：socket
* @Create    : 王津剑
* @Date      ：2021-5-6
* @Update    :
**********************************************************************/
int comm_connect_socket_noblock(char *ifName, uint32 IP, uint16 Port, int *tempfd, uint32 *startTime)
{
    struct hostent *serverName;
    struct sockaddr_in pin;
    int   fd_socket;
    int   flags_block;
    int   len;
    char  hostName[24] = {0};
    int ret = 0;
    struct timespec curtime = {0};
    int error = -1;
    struct tcp_info info;

    sprintf(hostName, "%u.%u.%u.%u", (IP>>24)&0xff, (IP>>16)&0xff, (IP>>8)&0xff, IP&0xff);
    /*获得主机信息*/
    if((serverName = gethostbyname(hostName)) == 0)	
    {
        COMM_FMT_DEBUG("%s Error resolving local host %s.\n", ifName, hostName);		
        return -1;
    }

    if(NULL == serverName->h_addr)
    {
        COMM_FMT_DEBUG("%s find no host %s.\n", ifName, hostName);        
        return -2;
    }

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    /* 规定超过75s没有connect成功, socket需要关闭一次 这里缩小到30s */
    if(*tempfd <= 0 || *startTime + 30 < curtime.tv_sec)
    {
        if(*tempfd > 0)
        {
            close(*tempfd);
            *tempfd = -1;
            usleep(100000);
        }
        /*获取与主机进行套接口连接的socket*/
        fd_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(fd_socket <= 0)
        {	
            COMM_FMT_TRACE("%s Error connecting to socket\n", ifName);
            ret = -3;
            goto end;
        }

        /* 设置SOCKET文件句柄为非阻塞模式 */
        if((flags_block = fcntl(fd_socket, F_GETFL, 0))<0) 
        {	
            close(fd_socket);
            COMM_FMT_TRACE("%s ERROR F_GETFL\n", ifName);
            ret = -4; 
            goto end;
        }	

        if(fcntl(fd_socket, F_SETFL, flags_block | O_NONBLOCK) < 0)
        {	
            close(fd_socket);
            COMM_FMT_TRACE("%s ERROR set mode\n", ifName); 
            ret = -5; 
            goto end;
        }

        *tempfd = fd_socket;

        /*设置sockaddr_in参数*/     
        bzero(&pin, sizeof(pin));
        pin.sin_family      = AF_INET;
        pin.sin_addr.s_addr = ((struct in_addr *)(serverName->h_addr))->s_addr;
        pin.sin_port        = htons(Port);
        
        *startTime = curtime.tv_sec;
        /*连接前置机*/
        if(-1 != connect(fd_socket, (struct sockaddr *)&pin, (socklen_t)sizeof(pin)))
        {
            goto end;
        }
        usleep(100000);
    }
    else
    {
        fd_socket = *tempfd;
    }
    
    len = sizeof(info);
    getsockopt(fd_socket, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    if(TCP_ESTABLISHED == info.tcpi_state)
    {
        ret = 0;
    }
    else
    {
        getsockopt(fd_socket, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
        if(error != 0) 
        {
            ret = error; /* 有错误码 说明connect结束, 需要重新开始 */
            if(*tempfd > 0)
            {
                close(*tempfd);
            }
            *tempfd = -1;
            COMM_FMT_DEBUG("%s failed to connect master %s:%u, state %d, ret %d\n", ifName, hostName, Port, 
                                                                                    info.tcpi_state, ret);
            return -6;
        }
        else
        {
            ret = -7;
        }
    }

end:
    if(0 == ret)
    {
        COMM_FMT_DEBUG("%s connects to master %s:%u\n", ifName, hostName, Port);
        *tempfd = -1;
        return fd_socket;
    }
    else
    {
        COMM_FMT_TRACE("%s connects to master %s:%u error, state %d, ret %d\n", ifName, hostName, Port, 
                                                                                info.tcpi_state, ret);
        return -1;
    }
}

/**********************************************************************
* @name      : comm_set_ifnet
* @brief     ：设置网口属性
* @param[in] ：char *ifName      接口名称
               uint8 *pIp        ip地址
               uint8 *pMask      掩码
               uint8 *pGetWay    网关
               uint8 *pMac       MAC地址
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
void comm_set_ifnet(char *ifName, uint8 *pIp, uint8 *pMask, uint8 *pGetWay, uint8 *pMac)
{
    char sysCom[100] = {0};
    
    /* 清空网卡路由 */
    
    sprintf(sysCom, "ifconfig %s %u.%u.%u.%u netmask %u.%u.%u.%u", 
                    ifName, pIp[0], pIp[1], pIp[2], pIp[3], pMask[0], pMask[1], pMask[2], pMask[3]);
    system(sysCom);
    COMM_FMT_TRACE("%s\n", sysCom);
    usleep(10000);

    memset(sysCom, 0, 100);
    sprintf(sysCom, "ifconfig %s down && ifconfig %s hw ether %u:%u:%u:%u:%u:%u && ifconfig %s up", 
                    ifName, ifName, pMac[0], pMac[1], pMac[2], pMac[3], pMac[4], pMac[5], ifName);
    COMM_FMT_TRACE("%s\n", sysCom);
    usleep(10000);

    if(0 != pGetWay[0] + pGetWay[1] + pGetWay[2] + pGetWay[3])
    {
        memset(sysCom, 0, 100);
        sprintf(sysCom, "route add default gw %u.%u.%u.%u dev %s", 
                        pGetWay[0], pGetWay[1], pGetWay[2], pGetWay[3], ifName);
        system(sysCom);
        COMM_FMT_TRACE("%s\n", sysCom);
        usleep(10000);
    }
}

/**********************************************************************
* @name      : comm_check_link
* @brief     ：实现网线的插拔检测
* @param[in] ：char *ifName      接口名称
* @param[out]：
* @return    ：1-网线连接上
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
int comm_check_link(char *ifName)
{
	int err;
	struct ethtool_value edata;
	int allfail = FALSE;
	struct ifreq ifr;
	int fd;

	/* Setup our control structures. */
	memset(&ifr, 0, sizeof(ifr) );
	strcpy(ifr.ifr_name, ifName);

	/* Open control socket. */
	fd = socket( AF_INET, SOCK_DGRAM, 0);

	if (fd < 0)
	{
		return 0;
	}

	edata.cmd = ETHTOOL_GLINK;
	ifr.ifr_data = (caddr_t)&edata;
	err = ioctl(fd, SIOCETHTOOL, &ifr);//send_ioctl(fd, ifr);
	close(fd);

	if (err == 0)
	{
		allfail = edata.data;
	}

	return allfail;
}

/**********************************************************************
* @name      : comm_del_allroute
* @brief     ：清除所有路由
* @param[in] ：char *ifName      接口名称
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-11-4
* @Update    :
**********************************************************************/
void comm_del_allroute(char *ifName)
{
}


uint32 g_CommSFlow[4] = {0};
uint32 g_CommRFlow[4] = {0};
pthread_mutex_t  g_fCommFlow;

/**********************************************************************
* @name      : comm_flow_init
* @brief     ：流量统计初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-2-11
* @Update    :
**********************************************************************/
void comm_flow_init()
{
    pthread_mutex_init(&g_fCommFlow, NULL);
}

/**********************************************************************
* @name      : comm_flow_add
* @brief     ：流量统计值增加
* @param[in] ：uint8 bPPP         是否无线网
               uint8 index       网络编号0-1
               uint32 Sendflow   发送流量
               uint32 Recvflow   接收流量
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-2-11
* @Update    :
**********************************************************************/
void comm_flow_add(uint8 bPPP, uint8 index, uint32 Sendflow, uint32 Recvflow)
{
    pthread_mutex_lock(&g_fCommFlow);
    if(TRUE == bPPP)
    {
        if(index <= 1)
        {
            g_CommSFlow[index] += Sendflow;
            g_CommRFlow[index] += Recvflow;
        }
    }
    else
    {
        if(index <= 1)
        {
            g_CommSFlow[2 + index] += Sendflow;
            g_CommRFlow[2 + index] += Recvflow;
        }
    }
    pthread_mutex_unlock(&g_fCommFlow);
}

/**********************************************************************
* @name      : comm_flow_get
* @brief     ：流量统计值获取
* @param[in] ：uint8 bPPP         是否无线网
               uint8 index       网络编号0-1
* @param[out]：uint32 *pSendflow 发送流量
               uint32 *pRecvflow 接收流量
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-2-11
* @Update    :
**********************************************************************/
void comm_flow_get(uint8 bPPP, uint8 index, uint32 *pSendflow, uint32 *pRecvflow)
{
    pthread_mutex_lock(&g_fCommFlow);
    if(TRUE == bPPP)
    {
        if(index <= 1)
        {
            *pSendflow = g_CommSFlow[index];
            g_CommSFlow[index] = 0;
            *pRecvflow = g_CommRFlow[index];
            g_CommRFlow[index] = 0;
        }
    }
    else
    {
        if(index <= 1)
        {
            *pSendflow = g_CommSFlow[2 + index];
            g_CommSFlow[2 + index] = 0;
            *pRecvflow = g_CommRFlow[2 + index];
            g_CommRFlow[2 + index] = 0;
        }
    }
    pthread_mutex_unlock(&g_fCommFlow);
}

