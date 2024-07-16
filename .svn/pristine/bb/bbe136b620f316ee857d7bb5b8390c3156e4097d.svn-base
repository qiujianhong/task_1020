/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP 串口操作
* @date：    2021-8-9
* @history： 
*********************************************************************
*/

#ifndef __RADIO_UART_H__
#define __RADIO_UART_H__


#ifdef __cplusplus
extern "C" {
#endif

/* 电台串口设备节点 */
#ifdef PRODUCT_SEU
#define RADIO_UART_NAME   "/dev/ttyS3"
#else
    #if defined (BOARD_801) && defined (PRODUCT_ZCU_1)
        #define RADIO_UART_NAME   "/dev/ttyS7"
    #else
        #define RADIO_UART_NAME   "/dev/ttyS0"
    #endif
#endif

int radio_dev_reset();

int radio_uart_open(char *name);
int radio_uart_set(int fd, int baud, uint8 databits, uint8 stopbits, char parity);
int radio_uart_send(int fd, uint8 *data, int len);
int radio_uart_recv(int fd, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime);
void radio_uart_close(int fd);

void SetPttLow(uint32 delay, uint8 *pRadioState);
void SetPttHigh(uint32 delay, uint8 *pRadioState);
uint32 ReadPttDelay();

#ifdef __cplusplus
}
#endif

#endif //__RADIO_UART_H__

