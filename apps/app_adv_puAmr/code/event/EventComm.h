/*-***************************************************************
* Copyright (c) 2006,南京新联电子仪器有限公司
*            All rights reserved.
* 程序名称：eventcomm.h
* 版本号：1.0
* 功能：时间处理及格式转换
* 原始开发人及时间：梅安宁 2020年04月13日
* 修改人员、时间及修改内容:无
* 其他：
******************************************************************-*/
#ifndef _EVENTCOMM_H
#define _EVENTCOMM_H

#ifdef __cplusplus
extern "C"{
#endif

/*用于端口和从节点描述符“type = value; mfrs = value; idformat = value; id = value; mmfrs = value; midformat = value; mid = value”*/
typedef struct
{
    char type[10];      //本地通讯模块类型 value包括“PLC”、“WIRELESS”等 节点设备类型 
    char mfrs[4];       //芯片厂商代码 2个字符的ASCII码
    uint8 idformat;     //芯片ID格式类型，其value为以下几种：“0”为组合格式；“1”为BCD；“2”为BIN ；“3”为ASCII
    char id[100];       //芯片ID信息，其value为对应的字符串,根据idformat的格式进行相应转换
    char mmfrs[4];      //模块厂商代码，其value为2个字符的ASCII码
    uint8 midformat;    //模块ID格式类型，其的value为以下几种：“0”为组合格式；“1”为BCD；“2”为BIN ；“3”为ASCII
    char mid[200];      //块ID信息，其value为对应的字符串,根据midformat的格式进行相应转换。
}DESCRIPTOR_T;

/*--计量点类型定义--*/
#define OI_485       0XF201   /*-485表-*/
//#define OI_SIM       2   /*-模拟表-*/
#define OI_PULSE     0XF20A   /*-脉冲-*/
//#define OI_VIR       4   /*-虚拟-*/
#define OI_AC        0XF208   /*-交采-*/
#define OI_PLC       0XF209   /*-载波-*/
//#define OI_PLCMUI    7   /*-载波多功能-*/


void systm_to_ooptm(time_t sys_tm, OOP_DATETIME_S_T *poop_time);
void rtctm_to_ooptm(rtc_t rtc_tm, OOP_DATETIME_S_T *poop_time);

void ooptm_to_rcttm(OOP_DATETIME_S_T oop_time,rtc_t *prtc_tm);
void ooptm_to_systm(OOP_DATETIME_S_T oop_time, time_t *psys_tm);

void systm_to_rtctm(time_t sys_tm, rtc_t *prtc_tm);

//获取当前时间
void current_tm_to_rtctm(rtc_t *prtc_tm);
void current_tm_to_ooptm(OOP_DATETIME_S_T *poop_time);

int CompareTSA(OOP_TSA_T tsa1, OOP_TSA_T tsa2);

BOOL event_data_is_null(void* data, uint32 len);

DESCRIPTOR_T UnpackDescriptor(uint8 type, char* buf);

#ifdef __cplusplus
}
#endif

#endif


