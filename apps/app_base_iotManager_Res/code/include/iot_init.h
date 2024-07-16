/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块PPP操作相关定义申明
* @date：    2019-10-24
* @history： 
*********************************************************************
*/
#ifndef __IOT_INIT_H__
#define __IOT_INIT_H__
        
#ifdef __cplusplus
        extern "C" {
#endif
#include <sys/stat.h>
#include <errno.h>
#include "baseCommon.h"
#include "framecomm.h"
#include "mqtt_client.h"
#include "task_msg.h"
#include "appmsg_split.h"
#include "task_base.h"
#include "devinfo.h"
#include "hal.h"
#include <stdlib.h>
#include <sys/stat.h>
#include "iniparser.h"

#define MAX_QUEUE_CNT           20
#define MAX_QUEUE_BUFF_LEN      500
#define NAMELEN 100
#define TIMELEN 20
#define MSGLEN  255
#define WUGUAN_INITFILE     "iotManager.ini"

int iot_version_print(char *buf, uint32 buflen);
int iot_app_init(void **ppPublicData);
extern uint8 iot_get_appid(uint8 *appid, uint8 len);
#define WUGUAN_APPID_MAX        32

typedef struct Queue
{
    int32  QueueLen;                            //队列长度
    uint8   QueueBuf[MAX_QUEUE_BUFF_LEN];       //队列存储缓冲区
}Queue_T;

//锁共享缓冲区结构
typedef struct LockQueue
{
    pthread_mutex_t QueueLock;                  //接收缓冲区互斥锁
    int16  QueueHead;                           //队列头
    int16  QueueTail;                           //队列尾
    Queue_T Queue[MAX_QUEUE_CNT];               //通讯数据队列
}LockQueue_T;

typedef struct
{
  uint16 updateTime;
  uint16 monitorTime;
  uint16 devCpuLimit;
  uint16 devMemLimit;
  uint16 devDiskLimit;
  uint16 devCpuTempup;
  uint16 devCpuTempdown;
}SYSCONFIG;

typedef struct
{
  uint16 cpu_usage;
  uint16 mem_usage;
  uint16 disk_usage;
  uint16 cputemp;
}SYSINFO;

typedef struct
{
    uint8 cpuflag;
    uint8 memflag;
    uint8 diskflag;
    uint8 tempflag;
}SYSEVENTFLAG;

typedef struct
{
    time_t      runtime;
    SYSCONFIG   sysconfig;
    SYSINFO     sysinfo;
    SYSEVENTFLAG sysflag;
}WGSYSINFO;
typedef struct
{
    uint32      mqttip;
    uint16      mqttport;     //mqtt端口
    uint8       appidlen;
    uint8       appid[WUGUAN_APPID_MAX];
    LockQueue_T msgque;
    LockQueue_T jsonque;
    LockQueue_T upque;
    WGSYSINFO   systheadinfo;
}WGPubattr;

typedef struct
{
    char  container[NAMELEN];
    char  app[NAMELEN];
    char  datetime[TIMELEN];
    char  msg[MSGLEN];
    char  reason[MSGLEN];
    uint8 alarmType;
    uint8 notifyType;
    uint8 result; 
}JSON_REPORT_DATA;


#ifdef __cplusplus
}
#endif

#endif //__PPP_OPER_H__

