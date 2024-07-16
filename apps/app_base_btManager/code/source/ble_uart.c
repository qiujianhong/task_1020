/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����APP ���ڲ���
* @date��    2021-8-9
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"

#include "ble_uart.h"

/* �������Ƶ��豸�ڵ� */
#define BLE_CTRL_NAME   "/dev/btctrl"

/* ��д������־ֵ */
#define	BT_IOC_MAGIC		'B'
#define BT_IO_SETHIGH		_IOW(BT_IOC_MAGIC, 1, __u8)
#define BT_IO_SETLOW		_IOW(BT_IOC_MAGIC, 2, __u8)
/* Ҫ������IO��������ֵ�ĵ�ַ */
#define BTC_IO_RST  0
#define BTC_IO_CTS  1
/*�������Ƶ�IO������������ģ��CTS���ƣ�����ģ�鸴λ���ơ�CTSΪ�͵�ƽʱ��ʹ�ܷ��ͣ���λ����Ϊ�͵�ƽ��λ*/


/**********************************************************************
* @name      : ble_dev_reset
* @brief     �������豸��λ
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-9
* @Update    :
**********************************************************************/
int ble_dev_reset()
{
    int fd = -1;
    int ret = 0;
    
    fd = open(BLE_CTRL_NAME, O_WRONLY);
    if(fd < 0)
    {
        return -1;
    }

    ret = ioctl(fd, BT_IO_SETLOW, BTC_IO_RST);
    if(0 != ret)
    {
        close(fd);
        return -2;
    }
    usleep(10000);
    ret = ioctl(fd, BT_IO_SETHIGH, BTC_IO_RST);
    if(0 != ret)
    {
        close(fd);
        return -3;
    }

    close(fd);
    return 0;
}

/**********************************************************************
* @name      : ble_uart_open
* @brief     �������豸��
* @param[in] ��char *name  ��������
* @param[out]��
* @return    ��>=0-���/������
* @Create    : ����
* @Date      ��2021-8-9
* @Update    :
**********************************************************************/
int ble_uart_open(char *name)
{
    if(NULL == name || 0 == strlen(name))
    {
        return -1;
    }
    if(0 != access(BLE_UART_NAME, F_OK))
    {
        return -1;
    }
    return open(name, O_RDWR);
}



/**********************************************************************
* @name      : ble_set_speed
* @brief     �����ò�����
* @param[in] ��int fd           ���ھ��
               int speed        ������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-9
* @Update    :
**********************************************************************/
int ble_set_speed(int fd, int speed)
{
    int i; 
    int ret; 
    int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B600, B300};
    int name_arr[] = {115200, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300};

    struct termios Opt; 	/*Opt�д洢���������ݽṹ*/
    if(0 != tcgetattr(fd, &Opt)) 	/*��ȡtermios�ṹ*/
    {
        return -1;
    }

    for(i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
    {
        if(speed == name_arr[i])   /*����֧�ֵĲ�����*/
        {
            cfsetispeed(&Opt, speed_arr[i]); /*�������벨����*/
            cfsetospeed(&Opt, speed_arr[i]); /*�������������*/
            ret = tcsetattr(fd, TCSANOW, &Opt);  /*���ô����µ�termios�ṹ*/
            if(0 != ret)
            {
                return -2;
            }
            tcflush(fd, TCIOFLUSH); /*������ڵ�������������е���*/
            return 0;                     
        }  
    }
    return -3;
}

/**********************************************************************
* @name      : ble_set_parity
* @brief     ����������
* @param[in] ��int fd              ���ھ��
               uint8  databits     ����λ(5~8)
               uint8  stopbits     ֹͣλ(1~2)
               char   parity       У��λ(N ��У�飬E żУ�飬O��У��)
               uint8  set_crtscts  Ӳ������ 1-���� 0-����
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-9
* @Update    :
**********************************************************************/
int ble_set_parity(const int fd, uint8 databits, uint8 stopbits, char parity, uint8 set_crtscts)
{
    struct termios options; 
    if(0 != tcgetattr(fd, &options)) /*��ȡ����termios�ṹ*/
    { 
        return -1;  
    }

    /* ����Ϊ���ӹ�ģʽ, ���� */
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag = IGNBRK;
    options.c_cflag |= CLOCAL | CREAD;
    options.c_cflag &= ~CSIZE;  /*���ԭ�ַ���*/

    switch(databits) /*��������λ��*/
    {   
        case 7:             
            options.c_cflag |= CS7;  /*����7λ����λ*/
            break;
        case 8:     
            options.c_cflag |= CS8;  /*����8λ����λ*/
            break;   
        default:    
            return -2;  
    }

    switch (parity) 
    {   
        case 'n':	/*��У��*/
        case 'N': 
            options.c_cflag &= ~PARENB;   /* ������żУ����Ч */
            break;

        case 'o': /*��У��*/
        case 'O':     															
            options.c_cflag |= (PARODD | PARENB);	/*����Ϊ��Ч��*/  
            //options.c_iflag |= INPCK;		/*�����������ż��*/
            break;

        case 'e': /*żУ��*/
        case 'E':  
            options.c_cflag &= ~PARODD;
            options.c_cflag |= PARENB;  /* ʹ��żУ����*/ 
            //options.c_iflag |= INPCK;		/*�����������ż��*/
            break;

        case 'S': /*spaceУ��*/
        case 's': 
            options.c_cflag &= ~CSTOPB;
            break;

        default:   
        return -3;  
    }

    /* ����ֹͣλ*/ 
    switch (stopbits)
    {   
        case 1: /*1λֹͣλ*/
            options.c_cflag &= ~CSTOPB;  
            break;  
        case 2: /*2λֹͣλ*/
            options.c_cflag |= CSTOPB;  
            break;
        default:  
            return -4; 
    }

    tcflush(fd, TCIFLUSH);        /* ���������*/
    options.c_cc[VTIME] = 100;   /* ���ó�ʱ10 seconds*/   
    options.c_cc[VMIN] = 0;      /* �������ã�ʹ������Ч */
    
// 	options.c_iflag |= (IXON | IXOFF);	/*���������*/
    options.c_iflag &= ~(IXON|IXOFF|IXANY);	/*�������*/

    if (set_crtscts == 1)
        options.c_cflag |= CRTSCTS; /*��Ӳ������*/
    else
        options.c_cflag &= ~CRTSCTS;/*��Ӳ������*/

    options.c_iflag &= ~IGNCR;  /*�������յ���CR�ַ�*/
    options.c_iflag &= ~ICRNL;

    if(0 != tcsetattr(fd, TCSANOW, &options))  /*�Դ��ڽ��в�������*/
    {  
        return -5;  
    }
    return 0;
}

/**********************************************************************
* @name      : ble_uart_set
* @brief     ��������������
* @param[in] ��int fd                ���
               int    baud           ������
               uint8  databits       ����λ(5~8)
               uint8  stopbits       ֹͣλ(1~2)
               char   parity         У��λ(N ��У�飬E żУ�飬O��У��)
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-9
* @Update    :
**********************************************************************/
int ble_uart_set(int fd, int baud, uint8 databits, uint8 stopbits, char parity)
{
    int ret = 0;
    if(fd < 0)
    {
        return -1;
    }

    ret = ble_set_speed(fd, baud);
    if(0 != ret)
    {
        printf("set_speed false, ret %d!\n", ret);
        return ret;
    }

    ret = ble_set_parity(fd, databits, stopbits, parity, 0);
    if(0 != ret)
    {
        printf("set_Parity false, ret %d!\n", ret);
        return ret;
    }
    return 0;
}

/**********************************************************************
* @name      : ble_uart_send
* @brief     �����ڷ�������
* @param[in] ��int fd                ���
               uint8* data           ����
               int len               ����
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-9
* @Update    :
**********************************************************************/
int ble_uart_send(int fd, uint8 *data, int len)
{
    int send_len = 0;  

    if(fd < 0 || NULL == data || 0 == len)
    {
        return -1;
    }

    send_len = write(fd, data, len);
    return send_len == len ? 0 : -2;
}

/**********************************************************************
* @name      : ble_uart_recv
* @brief     ���������ݽ��սӿ�(�ȴ�)
* @param[in] ��int fd                ���
               int nBufLen                  �����ճ���
               int rsptime                  ���ݷ���ʱ��
               int rcvtime                  �����ֽ�֮����ʱ��
* @param[out]��uint8* pBuffer               �������ݻ�����
* @return    �����յ����ݵĳ���
* @Create    : ����
* @Date      ��2021-8-9
* @Update    :
**********************************************************************/
int ble_uart_recv(int fd, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime)
{
	int recv_len = 0;        /*���յ����ݳ���*/
    int data_len = 0;        /*���ڻ����������ݵĳ���*/
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
            if(0 == data_len && count * 5 > rsptime) 
            {
                return data_len; /*-�������ݳ�ʱ-*/
            }

            if(0 != data_len && (count * 5 > rcvtime || data_len >= nBufLen))
            {
                return data_len;
            }
        }
        count++;
    }
}

/**********************************************************************
* @name      : ble_uart_close
* @brief     �������豸�ر�
* @param[in] ��int fd                ���
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-8-9
* @Update    :
**********************************************************************/
void ble_uart_close(int fd)
{
    if(fd >= 0)
    {
        close(fd);
    }
}



