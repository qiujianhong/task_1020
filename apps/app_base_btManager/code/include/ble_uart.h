/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP 串口操作
* @date：    2021-8-9
* @history： 
*********************************************************************
*/

#ifndef __BLE_UART_H__
#define __BLE_UART_H__


#ifdef __cplusplus
extern "C" {
#endif

/* 蓝牙串口设备节点 */
#ifdef PRODUCT_ECU
    #ifdef BOARD_3358
    #define BLE_UART_NAME   "/dev/ttyS5"
    #endif
    #ifdef BOARD_801
    #define BLE_UART_NAME   "/dev/ttyBT"
    #endif
#else
#define BLE_UART_NAME   "/dev/ttyBT"
#endif

int ble_dev_reset();

int ble_uart_open(char *name);
int ble_uart_set(int fd, int baud, uint8 databits, uint8 stopbits, char parity);
int ble_uart_send(int fd, uint8 *data, int len);
int ble_uart_recv(int fd, uint8* pBuffer, int nBufLen, int rsptime, int rcvtime);
void ble_uart_close(int fd);


#ifdef __cplusplus
}
#endif

#endif //__BLE_UART_H__

