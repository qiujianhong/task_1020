/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   mqtt通信基础头文件
* @date：    2020-4-16
* @history： 
*********************************************************************
*/

#ifndef __MQTT_BASE_H__
#define __MQTT_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define  RECV_BUF_LEN           5120	/*recvfrom接收缓冲区大小*/

typedef enum
{
    APP_STATUS_INIT = 0,  //初始化 APP注册
    APP_STATUS_SEEK,      //查询APP信息
    APP_STATUS_WORK,      //正常工作
}APP_STATUS_E;

int msg_task_start(uint32 ip, uint16 port);

#ifdef __cplusplus
}
#endif

#endif //__MQTT_BASE_H__


