#ifndef _HAL_H
#define _HAL_H

#ifdef __cplusplus
extern "C" {
#endif 


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/file.h> 
#include <fcntl.h>
#include <errno.h>
#include <termios.h> 

#include "types.h"
#include "sysDefine.h"

#define   HAL_MODULE_INFO_SYM   HMI            /* HAL module名称 */
#define   HAL_MODULE_INFO_SYM_AS_STR  "HMI"    /* HAL module名称字符串 */

#define HW_MODULE_MAXDEVICES (128)    /* 一个模块内包含的最大设备数量 */
#define HW_DEVICE_ID_MAXLEN  (32)    /*设备ID名最大长度 */

struct tag_HW_DEVICE;
struct tag_HW_MODULE;

typedef struct tag_HW_MODULE 
{ 
    const char *szName;       /* 模块名 */
    unsigned int nVer;        /* 模块版本号 */
    const char *szAuthor;     /* 作者 */
    unsigned int nFactory;    /* 厂商代码 */
    const char *szBuildDate;  /* 发布日期YYYY-MM-DD */
    uint32 nDevNum; /* 模块包含的硬件设备数 */
    char ** szDevices;  /* 模块包含的硬件设备集合 */

    /**
    * @brief 打开设备访问接口
    * @param[in] pModule: 接口模块对象指针
    * @param[in] szDeviceID: 设备ID 
    * @param[in] ppDevice: 设备访问接口对象指针
    * @return 成功返回module结构指针，失败返回NULL
    */
    int (*pfOpen)( struct tag_HW_MODULE *pModule, const char *szDeviceID, 
        struct tag_HW_DEVICE **pDevice ); 
    
    /**
    * @brief 关闭设备访问接口
    * @param[in] pModule: 接口模块对象指针
    * @param[in] ppDevice: 设备访问接口对象指针
    * @return 成功返回0
    */
    int (*pfClose)(struct tag_HW_MODULE *pModule, struct tag_HW_DEVICE *pDevice ); 
}HW_MODULE;

typedef struct tag_HW_DEVICE 
{ 
    HW_MODULE *pModule;         /* 设备所属模块对象指针 */
    int nVer;                   /* 设备接口版本号 */
    const char * szDeviceID;  /* 设备ID名 */
}HW_DEVICE;


/****************************************************
* @brief 为应用层提供的硬件访问入口，获取设备
* @param[in] device_id: 指定device id
*****************************************************/
 struct tag_HW_DEVICE* hal_device_get(const char *device_id);


/**
*********************************************************************
* @brief       关闭设备并释放设备驱动
* @param[in]  dev: 设备指针
* @param[out]  无
* @return      成功返回SUCCEED; 失败返回错误编号
* @note        函数的应用编程手册（详细描述函数的使用说明）
* @warning     注意事项
*********************************************************************
*/
int hal_device_release(struct tag_HW_DEVICE* dev);


/**
*********************************************************************
* @brief       hal库初始化
* @param[in]   无
* @param[out]  无
* @return      成功返回0; 失败返回小于0错误编号
* @note        函数的应用编程手册（详细描述函数的使用说明）
* @warning     注意事项
*********************************************************************
*/
extern int hal_init(void);

/**
*********************************************************************
* @brief       驱动库退出 
* @param[in]   无
* @param[out]  无
* @return      成功返回0; 失败返回小于0错误编号
* @note        函数的应用编程手册（详细描述函数的使用说明）
* @warning     注意事项
*********************************************************************
*/
extern int hal_exit(void);

#ifdef __cplusplus
}
#endif 

#endif
