/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ͨ��ģ��ͨ�ú���
* @date��    2019-10-31
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include <netdb.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>

#include "common_base.h"

#define DEF_UDP_PORT          15000    /* UDPָ���˿ں� */
#define DEF_UDP_PORT_MAX      15500    /* UDPָ�����˿ں� */


/**********************************************************************
* @name      : comm_write_queue
* @brief     ��������
* @param[in] ��pQueue  �������ṹָ��
* @param[out]��pBuf    �����ݻ�����
* @return    �����ݳ���
* @Create    : ����
* @Date      ��2019-11-1
* @Update    :
**********************************************************************/
uint32 comm_read_queue(COMM_QUEUE_LIST_T *pQueue, uint8 *pBuf)
{
    uint32 len;
    uint8 *pdataBuf;
    
    pthread_mutex_lock(&pQueue->f_lock);
    len = 0;

    /*�������������ݣ���ȡ��buf�У��⿪��*/
    if ((pQueue->head != pQueue->tail)||((pQueue->head == pQueue->tail)&&(pQueue->queue[pQueue->head].len != 0)))
    {
        pdataBuf = pQueue->queue[pQueue->head].buf_queue;
        len = pQueue->queue[pQueue->head].len;
        memcpy(pBuf, pdataBuf, len);  /*ȡ�����е�����*/
        memset(pdataBuf, 0, len);     /*������*/
        pQueue->queue[pQueue->head].len = 0;
        pQueue->head = (pQueue->head+1) % QUEUE_SIZE; /*����ͷ����ƶ�һλ*/
    }

    pthread_mutex_unlock(&pQueue->f_lock);
    return len;
}

/**********************************************************************
* @name      : comm_write_queue
* @brief     ��д����, ��������������
* @param[in] ��pQueue  �������ṹָ��
               pBuf    ��Ҫд����е�����
               bufLen  ���ݳ���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-11-1
* @Update    :
**********************************************************************/
int comm_write_queue(COMM_QUEUE_LIST_T *pQueue, uint8 *pBuf, uint32 bufLen)
{
    int flags;
    flags = -1;
    pthread_mutex_lock(&pQueue->f_lock);

   /*�ж������Ƿ��пյ�λ��*/
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
* @name      : comm_add_channel
* @brief     ����buf�е����ݣ�������ͨ�������ϻỰЭ��ͷ��
* @param[in] ��channel   ͨ�����
               buf       ������ݻ�����ָ��
               len       ���ݳ���
* @param[out]��buf       ���ϻỰЭ�������
* @return    ��
* @Create    : ����
* @Date      ��2019-11-1
* @Update    :
**********************************************************************/
void comm_add_channel(uint8 channel, uint8 *buf, uint16 len)
{
    /* ��0���ֽ���ͨ����, ��2���ֽ����ڲ�ͨ��Э��, ��2/3���ֽ��ǳ��� */
    memmove(buf+4, buf, len);
    buf[0] = channel;
    buf[1] = 0;
    buf[2] = len%256;
    buf[3] = len/256;
}

/**********************************************************************
* @name      : comm_find_bigchannel
* @brief     ���л������е����������ĸ�ͨ������
* @param[in] ��buf   ָ�������ݵĻ�������ָ��
* @param[out]��
* @return    ��ͨ������
* @Create    : ����
* @Date      ��2019-11-1
* @Update    :
**********************************************************************/
uint8 comm_get_bigchannel(uint8 *buf)
{
    if(*buf < CHL_GPRS_1)
    {
        return CHL_LISTEN;
    }
    else if(*buf < CHL_GPRS_2)
    {
        return CHL_GPRS_1;
    }
    else if(*buf < CHL_ETH_1)
    {
        return CHL_GPRS_2;
    }
    else if(*buf < CHL_ETH_2)
    {
        return CHL_ETH_1;
    }
    else if(*buf < CHL_RESERVE)
    {
        return CHL_ETH_2;
    }
    else
        return CHL_RESERVE;
}

/**********************************************************************
* @name      : comm_get_protocol
* @brief     ����ȡ�ڲ�ͨ��Э��
* @param[in] ��uint8 *buf  ��Ϣͷ
* @param[out]��
* @return    ���ڲ�ͨ��Э��
* @Create    : ����
* @Date      ��2019-11-2
* @Update    :
**********************************************************************/
uint8 comm_get_protocol(uint8 *buf)
{
  	return buf[1];
}

/**********************************************************************
* @name      : comm_connect_delay
* @brief     ����ǰ�û���������
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2019-11-1
* @Update    :
**********************************************************************/
int comm_connect_delay(const int fd, struct sockaddr *servaddr, socklen_t socklen, int delay)
{
    int ret = -1;
    int error = -1;
    int len;
    int flags_old;
    struct timeval tm;
    fd_set set;

    len = sizeof(socklen_t);

    /* ����SOCKET�ļ����Ϊ������ģʽ*/
    if((flags_old = fcntl(fd, F_GETFL, 0))<0) 
    {	
        COMM_FMT_TRACE("ERROR set mode");
        return -1; 
    }

    if(fcntl(fd, F_SETFL, flags_old | O_NONBLOCK))
    {	
        COMM_FMT_TRACE("ERROR set mode"); 
        return -1;
    }

    if(-1 == connect(fd, servaddr, socklen))
    {
        tm.tv_sec = delay;
        tm.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(fd, &set);
        if( select(fd+1, NULL, &set, NULL, &tm) > 0)
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
                ret = -1;
            }
        }
        else
        {
            ret = -2;
            COMM_FMT_TRACE("ERROR select\n"); 
        }
    }
    else 
        ret = 0;

    /*����fdģʽ*/
    if(fcntl(fd, F_SETFL, flags_old))
    {	
        COMM_FMT_TRACE("ERROR set mode"); 
        return -1;
    }
    
    return ret;
}

/**********************************************************************
* @name      : comm_connect_socket
* @brief     ����ǰ�û�������������
* @param[in] ��uint32 IP         IP
               uint16 Port       PORT
* @param[out]��
* @return    ��socket
* @Create    : ����
* @Date      ��2020-3-19
* @Update    :
**********************************************************************/
int comm_connect_socket(uint32 IP, uint16 Port)
{
    struct hostent *serverName;
    struct sockaddr_in pin;
    int   fd_socket;
    int   flags_block;
    char  hostName[20] = {0};

    if(0 == IP || 0 == Port)
    {
        return -1;
    }

    sprintf(hostName, "%u.%u.%u.%u", (IP>>24)&0xff, (IP>>16)&0xff, (IP>>8)&0xff, IP&0xff);
    /*���������Ϣ*/
    if((serverName = gethostbyname(hostName)) == 0)	
    {
        COMM_FMT_TRACE("Error resolving local host %s.\n", hostName);		
        return -1;
    }

    /*��ȡ�����������׽ӿ����ӵ�socket*/	
    if((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {	
        COMM_FMT_TRACE("%s error connecting to socket\n", hostName);
        return -1;
    }

    /*����sockaddr_in����*/		
    bzero(&pin, sizeof(pin));
    pin.sin_family      = AF_INET;
    pin.sin_addr.s_addr = ((struct in_addr *)(serverName->h_addr))->s_addr;
    pin.sin_port        = htons(Port);

    /*����ǰ�û�*/			
    if(0 != comm_connect_delay(fd_socket, (struct sockaddr *)&pin, (socklen_t)sizeof(pin), 12))
    {	
        close(fd_socket);
        COMM_FMT_DEBUG("Error connecting to master %s, %u\n", hostName, Port);
        return -1; 
    }

    /* ����SOCKET�ļ����Ϊ������ģʽ */
    if((flags_block = fcntl(fd_socket, F_GETFL, 0))<0) 
    {	
        COMM_FMT_TRACE("%s ERROR F_GETFL\n", hostName);
        close(fd_socket);
        return -1; 
    }	

    if(fcntl(fd_socket, F_SETFL, flags_block | O_NONBLOCK))
    {	
        COMM_FMT_TRACE("%s ERROR set mode\n", hostName); 
        close(fd_socket);
        return -1;
    }

    return fd_socket;	
}

/**********************************************************************
* @name      : comm_open_listen
* @brief     ����������
* @param[in] ��uint16 port          �����˿�
               uint8 num            ��������
* @param[out]��
* @return    ��socket���
* @Create    : ����
* @Date      ��2020-3-6
* @Update    : 2020-3-19
**********************************************************************/
int comm_open_listen(uint16 port, uint8 num)
{
    int listenfd;  
    struct sockaddr_in servaddr;
    int flags_block;
    int opt = 1;
    int val;

    if(0 == port || 0 == num)
    {
        COMM_FMT_TRACE("NULL input\n");
        return -1;
    }
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
        COMM_FMT_TRACE("%d get socket error\n", port);
        return -1;
    }

    /* ����SOCKET�ļ����Ϊ������ģʽ */
    if((flags_block = fcntl(listenfd, F_GETFL, 0))<0) 
    {   
        COMM_FMT_TRACE("%d ERROR F_GETFL\n", port);
        goto closefd;
    }   
    
    if(fcntl(listenfd, F_SETFL, flags_block | O_NONBLOCK))
    {   
        COMM_FMT_TRACE("%d ERROR set mode\n", port);
        close(listenfd);
        goto closefd;
    }

    /* �ر��ӽ��������ļ������� */
    val = fcntl(listenfd, F_GETFD);
    if (val == -1)
    {
        COMM_FMT_TRACE("%d ERROR F_GETFD\n", port);
        goto closefd;
    }

    val |= FD_CLOEXEC;
    if (fcntl(listenfd, F_SETFD, val) == -1)
    { 
        COMM_FMT_TRACE("%d ERROR set F_SETFD\n", port);
        goto closefd;
    }

    //SO_REUSEADDR ������bind���������б��ص�ַ���ظ�ʹ��
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
    {
         COMM_FMT_TRACE("%d setsockopt error SO_REUSEADDR\n", port);
         goto closefd;
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);
    
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0)
    {
        COMM_FMT_TRACE("%d bind error\n", port);
        goto closefd;
    }

    if (listen(listenfd, num) < 0)
    {
        COMM_FMT_TRACE("%d listen error\n", port);
        goto closefd;
    }
    return listenfd;
closefd:
    close(listenfd);
    return -1;
}

/**********************************************************************
* @name      : comm_init_udp
* @brief     ����ʼ��UDP�˿�, ָ��UDP�˿�ΪUDP_PORT
* @param[in] ��char *ifName      �ӿ�����
* @param[out]��
* @return    ��socket
* @Create    : ����
* @Date      ��2019-11-1
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

    /*����socket��ʶ��, UDPЭ��*/
    if ((fd_sock = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
        COMM_FMT_TRACE("%s init_udp_socket socket error\n", ifName);
        return -1;
    }

    /* socket������ */
    /*bzero(&interf, sizeof(interf));
    strncpy(interf.ifr_ifrn.ifrn_name, ifName, strlen(ifName));
    if (setsockopt(fd_sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interf, sizeof(interf)) < 0) 
    {
        COMM_FMT_TRACE("%s SO_BINDTODEVICE failed\n", ifName);
        return -1;
    }*/
    
    /*��fd_sock ��sin�ṹ�еĵ�ַ���а�*/
    bzero(&sin, sizeof(sin));
    udp_port = DEF_UDP_PORT;
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port        = htons(udp_port);
    bind_err = bind(fd_sock, (struct sockaddr *)&sin, sizeof(sin));

    /*��UDP_PORT�˿��Ѿ���ʹ����, �����°�UDP_PORT+1�˿�, ֱ���˿ں�Ϊ5500*/
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

    /*�󶨶˿ںųɹ���, ��fd_sock����Ϊ������ģʽ*/
    if (fd_sock > 0)
    {
        /* ����SOCKET�ļ����Ϊ������ģʽ */
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


