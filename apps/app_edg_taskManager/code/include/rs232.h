/*-***************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
* �������ƣ�rs232.h
* �汾�ţ�V1.0
* ���ܣ�232��̬��ͷ�ļ�
* ������:�ȱ�
*    ����ʱ�䣺2006��12��22��
* �޸���Ա��ʱ�估�޸�����:
* ������ 
******************************************************************-*/

#ifndef    _RS232_H
#define _RS232_H

#include "../include/CtsAll.h"
#include "hal.h"
#include "uart.h"
#include "can.h"
#define CONTROL_SET_RTS   0X6501
#define CONTROL_CLEAR_RTS   0X6502
#define CONTROL_GET_RTSSTATE 0X6503

#define MAX_FRAME_OVERTIME  6       //֡��ʱʱ�� ����һ��Ĭ�ϵĳ�ʱʱ�� ����ת���ܵ�ʱ��ص����ĳ�6��
#define MAX_CHAR_OVERTIME  500       //�ַ���ʱʱ�� 10ms

typedef struct
{
    uint8 sDeviceName[30];                    /*�豸������"/dev/ttyS0" */
    int dwBaudRate;                            /*������*/
    int8 byByteSize;                            /*����λ, 4-8 */
    int8 byParity;                                /*У�� 'n'  'e'  'o'  's' = None,Odd,Even,Space*/
    int8 byStopBits;                            /*ֹͣλ 0,1,2 = 1, 1.5, 2*/
    int fd_232;


} COMM_PARA;

typedef struct
{
    char  deviceName[30];        /*�豸�� */
    uint8 isvalid;  //0������Ч��1������Ч��2����485ģ�鷢���˲��;
    void *pDev;
    uint16 Buad;
    UART_DEVICE_T* dev;
    uint8 count;
}RS485_RS232_PARA;
#ifdef PRODUCT_ECU
int taskmng_uart_devname_get(uint8 pipe,char* devname);
void taskmng_uart_release(uint8 pipe);


#endif

int rs485_RS232_send(uint8 pipe,uint8 overtime,uint16 byteovertime, uint8 speed,PARITY_TYPE_E parity, uint8* pBuffer, int nBufLen);


int rs485_RS232_recv(uint8 pipe, uint8* pBuffer, int nBufLen);
uint16 Getbyteovertime(uint8 speed);

#endif        /*_LIBSERIAL_H*/
