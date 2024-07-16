/*
*********************************************************************
* Copyright(C) 2024 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   负荷控制数据
* @date：    2024-07-4
* @history： 
*********************************************************************
*/
#ifndef __LOADCTRLLER_SLEFDATA_H__
#define __LOADCTRLLER_SLEFDATA_H__

#ifdef __cplusplus
    extern "C" {
#endif

/** @brief 控制中的保电状态*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint8                  guardState;         //保电状态
}PROTECT_CTRL_T;

/** @brief 控制中的遥控状态*/
typedef struct
{
    pthread_mutex_t        mutex;               //锁 多线程访问
    uint8                  state[OOP_MAX_TRIP]; //遥控状态
    uint8                  type[OOP_MAX_TRIP];  //遥控类型
    TELECON_TRIP           trip[OOP_MAX_TRIP];
    TELECON_CLOSE          close[OOP_MAX_TRIP];
}REMOTE_CTRL_T;

/** @brief 控制中的催费告警状态*/
typedef struct
{
    pthread_mutex_t        mutex;              //锁 多线程访问
    uint8                  status;             //投入状态
    OOP_CALLFEE_T          param;              //催费告警参数
}FEE_CTRL_T;



#ifdef __cplusplus
}
#endif

#endif //__LOADCTRLLER_SLEFDATA_H__

