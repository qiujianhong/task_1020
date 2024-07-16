/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __AC_METER_MAIN_H__
#define __AC_METER_MAIN_H__

#include "ac_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

#define TEST_PROG_ENABLE  0

#define ACIPC_INIT        0         // 运行状态 - 初始化IPC连接
#define ACIPC_LOGIN       1         // 运行状态 - 初始化IPC连接
#define ACIPC_LOGIN_WAIT  2         // 运行状态 - 初始化IPC连接
#define ACIPC_RECV        3         // 运行状态 - IPC数据接收
#define ACIPC_SEND        4         // 运行状态 - IPC数据发送
#define ACIPC_CLOSE       5         // 运行状态 - 关闭IPC连接

extern uint8      gSampPrtl;             //周期采集采用的规约，0-DLT645 1-698.45

extern DB_AGENT   gDBAgent;              //和数据中心交互的agent
extern DB_CLIENT  gDataCenterClient;     //和数据中心交互的句柄
extern DB_CLIENT  gDbUdpClient;          //和数据中心交互的句柄(UDP专用)
extern DB_CLIENT  gRptClient;            //事件上报存储用句柄
//主动上报消息INDEX
extern uint16  ActiveInfoIndex;                     //主动上报消息INDEX
extern pthread_mutex_t    ActiveInfoIndexLock;      //主动上报消息INDEX锁

//mqtt ip地址与端口号
extern char mqttIP[100];
extern int mqttport;

//是否关注的app均已注册
extern uint8 gMapManagerAppLoginFlag;
extern uint8 gDBCenterAppLoginFlag;

//本app是否已经注册成功
extern uint8 bAppSelfLoginState;

//是否支持转发485串口报文
extern int gSupportRelay485Flag;

//appname: app英文名称(用于查询appid)
extern char *AppIdName;

//appid: 对应app的ID信息
#define  APP_ID_LEN             24
extern char  AppId[APP_ID_LEN];

/* 线程结构 */
typedef int (*TASK_ENTRY)(uint32, uint16);
typedef void *(*EntryFun)(void *);

typedef struct tagTHREAD_ARG
{
    char               threadName[64];
    void* arg;
    uint32             cnt;
}THREAD_ARG_T;

typedef struct tagMainThread
{
    BOOL               isStart;      /* 是否启动 */
    pthread_t          nThreadId;    /* 线程ID */
    void* arg;          /* 入参 */
    EntryFun           pEntryFunc;   /* 线程入口函数 */
    BOOL               isDefend; 
}MAIN_TRHEAD_T;

void *pthread_time_chk(void *arg);
void *pthread_self_chk(void *arg);
void *pthread_mqtt(void *arg);
void *pthread_sample(void *arg);
void *pthread_save(void *arg);
void *pthread_report(void *arg);
void *db_update_entry(void *pArg);
void *pthread_test(void *arg);
void *pthread_485module(void *arg);
void *pthread_485comm(void *arg);
void *pthread_model(void *arg);
void *pthread_uartManager(void *arg);
void  ac_power_type_update(void);
void *pthread_check_poweroff(void *arg);
#ifdef __cplusplus
}
#endif

#endif // __AC_METER_MAIN_H__
