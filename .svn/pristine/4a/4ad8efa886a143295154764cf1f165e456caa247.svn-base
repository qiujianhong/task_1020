/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：rs232.h
* 版本号：V1.0
* 功能：232动态库头文件
* 开发人:尤斌
*    开发时间：2006年12月22日
* 修改人员、时间及修改内容:
* 其他： 
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

#define MAX_FRAME_OVERTIME  6       //帧超时时间 定义一个默认的超时时间 测试转加密的时候回的慢改成6秒
#define MAX_CHAR_OVERTIME  500       //字符超时时间 10ms

typedef struct
{
    uint8 sDeviceName[30];                    /*设备名，如"/dev/ttyS0" */
    int dwBaudRate;                            /*波特率*/
    int8 byByteSize;                            /*数据位, 4-8 */
    int8 byParity;                                /*校验 'n'  'e'  'o'  's' = None,Odd,Even,Space*/
    int8 byStopBits;                            /*停止位 0,1,2 = 1, 1.5, 2*/
    int fd_232;


} COMM_PARA;

typedef struct
{
    char  deviceName[30];        /*设备名 */
    uint8 isvalid;  //0代表无效，1代表有效，2代表485模块发生了插拔;
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
