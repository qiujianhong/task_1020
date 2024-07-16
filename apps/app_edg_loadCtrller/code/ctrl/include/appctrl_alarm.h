/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   告警、显示
* @date：    2020-4-14
* @history： 
*********************************************************************
*/
#ifndef __APPCTRL_ALARM_H_
#define __APPCTRL_ALARM_H_

#ifdef __cplusplus
        extern "C" {
#endif


/*-每条告警的状态-*/
#define NEW_ALARM   1        /*-新告警-*/
#define ALARM_ING   2        /*-告警进行中-*/
#define ALARM_OVER  0        /*-告警结束-*/

/*-告警进程步-*/
#define ALARM_INIT_STEP   0    /*-告警初始化进程步-*/
#define ALARM_OUTPUT_STEP 1    /*-告警输出进程步-*/
#define ALARM_DELAY_STEP  2    /*-时间刷新-*/

#define ALARM_INTERVAL  5    /*-告警间隔时间 5s-*/
#define ALARM_TIMES     3    /*-告警次数-*/

typedef struct
{
    uint8  status;          /*-显示状态,0:合闸,1:即将跳闸,2:跳闸,3:即将合闸-*/
    uint8  bShowTime;       /*是否显示时间*/
    uint8  timeType;        /*时间格式0-分:秒, 1-时:分:秒*/
    uint8  res[1];
    uint32 delaytime;       /*-剩余时间-*/
}ALARM_INFO_T;  /*-告警信息数据结构-*/

typedef struct tagCtrlAlarm
{
    uint8        ctrlType;      /*控制类型*/
    uint8        alarmStep;     /*-告警状态,0:告警结束,1:新告警,2:时间刷新-*/
    uint8        alarmType;     /*-告警类型,D0:语音告警,D1:显示画面告警-*/

    uint8        alarmInterval; /*-两次告警间隔时间-*/
    uint8        alarmTimes;    /*-告警次数 用于语音告警-*/

    uint8        timeCount;     /*-第一个语音告警持续时间计数-*/
    uint8        voiceIndex;    /*-语音索引 使用了蜂鸣器当前没有用到-*/

    uint8        group;         /*总加组序号0-7*/
    void        *pFunc;         /*构造显示提示框的函数*/
}ALARM_CTRL_T;


void appctrl_alarm_init(uint8 maxturn);
void appctrl_system_alarm(uint8 turnmax);
void appctrl_set_key(uint8 status);

#ifdef __cplusplus
}
#endif

#endif


