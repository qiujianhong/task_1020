/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP对外通信模块
* @date：    2019-12-24
* @history： 
*********************************************************************
*/

#ifndef __MQTT_BASE_H__
#define __MQTT_BASE_H__

#include "advpd_base.h"
#include "framecomm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  RECV_BUF_LEN           8192	/*recvfrom接收缓冲区大小*/

typedef enum
{
    ADVPD_STATUS_INIT = 0,  //初始化 APP注册
    ADVPD_STATUS_SEEK,      //查询APP信息
    ADVPD_STATUS_WORK,      //正常工作
}APP_ADVPD_STATUS_E;

#ifdef __cplusplus
}
#endif

#endif //__MQTT_BASE_H__


