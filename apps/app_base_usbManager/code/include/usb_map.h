/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控业务
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __USB_MAP_H
#define __USB_MAP_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define UEVENT_BUFFER_SIZE 2048  

/* usb设备映射 */
typedef struct tag_USB_MAP_T
{
    int    socket;    //套接字
}USB_MAP_T;

#ifdef __cplusplus
}
#endif

#endif /* __USB_MAP_H */


