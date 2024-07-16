/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控公共数据
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __USB_INIT_H
#define __USB_INIT_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define USB_APP_NAME     "usbManager"
#define USB_INITFILE     USB_APP_NAME".ini"
#define USB_DATAFILE     USB_APP_NAME".json"

typedef enum
{
    USB_DEV_ADD    = 0,
    USB_DEV_REMOVE = 1,
    USB_DEV_ADD_OK = 2,    
    USB_DEV_RMV_OK = 3,
}USB_DEVSTATUS_T;

/* USB设备公共数据 */
typedef struct tag_USB_DEVICES_T
{
    char name[32];
    char type[32];
    uint8 enable;    
    char path[256];
    char add[256];
    char remove[256];
    uint8 linkerNum;
    char (*linker)[32];
    uint8  status;    //状态 @ref USB_DEVSTATUS_T
}USB_DEVICES_T;

/* USB管理APP公共数据 */
typedef struct tag_USB_APPDATA_T
{
    uint32 devNum;
    USB_DEVICES_T *devices;
}USB_APPDATA_T;

#ifdef __cplusplus
}
#endif

#endif


