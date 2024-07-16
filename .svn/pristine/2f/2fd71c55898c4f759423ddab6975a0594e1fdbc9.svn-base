/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   模组消息交互定义
* @date：    2020/2/28
* @history： 
*********************************************************************
*/

#ifndef __MAPMSG_H__
#define __MAPMSG_H__

#include "dataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

//传输方向位：bit7=0表示此帧是由终端发出的; bit7=1表示此帧是由模块发出的
#define MAPMSG_DIR_TERMINAL     0
#define MAPMSG_DIR_MODULE       1

//启动标志位：bit6=0表示此帧是由模块发起; bit6=1表示此帧是由终端发起
#define MAPMSG_PRM_TERMINAL     1
#define MAPMSG_PRM_MODULE       0

//控制码
typedef enum _MAPMSG_CTR_E
{
    MAPMSG_CTR_OK  = 0,      //确认
    MAPMSG_CTR_NO  = 1,      //否认
    MAPMSG_CTR_MSG = 2,      //信息帧
}MAPMSG_CTR_E;

typedef enum _MAPMSG_APDU_E
{
    MAPMSG_APDU_LinkRequest        = 1,
    MAPMSG_APDU_GetRequest         = 2,
    MAPMSG_APDU_SetRequest         = 3,
    MAPMSG_APDU_ReportNotification = 4,
    
    MAPMSG_APDU_LinkResponse       = 129,
    MAPMSG_APDU_GetResponse        = 130,
    MAPMSG_APDU_SetResponse        = 131,
    MAPMSG_APDU_ReportResponse     = 132,
}MAPMSG_APDU_E;


#define MAPMSG_DT_COMM_MODINFO       0x0000    //模块信息
#define MAPMSG_DT_COMM_LINKCON       0x0001    //链路协商信息
#define MAPMSG_DT_COMM_PROTOCOL      0x0002    //协议版本
#define MAPMSG_DT_COMM_POWERSTATUS   0x0003    //电源状态
#define MAPMSG_DT_COMM_FILETRAN_STAT 0x0004    //文件传输状态
#define MAPMSG_DT_COMM_TIMER         0x0100    //时钟
#define MAPMSG_DT_COMM_HARDINIT      0x0200    //硬件初始化
#define MAPMSG_DT_COMM_ARGSINIT      0x0201    //参数初始化
#define MAPMSG_DT_COMM_DATAINIT      0x0202    //数据初始化
#define MAPMSG_DT_COMM_FILETRAN      0x0203    //文件传输

#define MAPMSG_DT_PULSE_STATUS       0x4000    //遥信状态
#define MAPMSG_DT_PULSE_LASTDATA     0x4001    //上一次脉冲计数和时间
#define MAPMSG_DT_PULSE_TOTALDATA    0x4002    //脉冲计数及统计时间
#define MAPMSG_DT_PULSE_SET          0x4100    //遥信脉冲功能配置
#define MAPMSG_DT_PULSE_SETTIME      0x4101    //遥信防抖时间
#define MAPMSG_DT_485_CAN_STATUS     0x4F00    //485_CAN状态

#define MAPMSG_DT_CTR_LOOP_STATUS    0x7000    //控制回路接线状态
#define MAPMSG_DT_CTR_RELAY_TYPE     0x7100    //继电器输出模式
#define MAPMSG_DT_CTR_RELAY_WIDTH    0x7101    //继电器输出脉冲宽度
#define MAPMSG_DT_CTR_STATUS         0x7201    //继电器输出控制


#define MAPMSG_DT_ANALOG_GET         0x8000    //模拟量采样

#define MAPMSG_DT_LCM_WORKSTATUS       0x9000  //回路巡检模块工况信息
#define MAPMSG_DT_LCM_EVENT_WORKSTATUS 0x9001  //回路巡检模块事件发生时工况信息
#define MAPMSG_DT_LCM_VERSIONINFO      0x9003  //回路巡检互感器版本信息

/* 控制域 */
typedef union _CMT_FIELD_U
{
    struct
    {
        uint8 cmd :6; //控制码
        uint8 prm :1; //启动标识
        uint8 dir :1; //传输方向
        uint8 fid;    //帧序号
    };
    uint16 wcmd;
}__attribute__ ((packed))CMT_FIELD_U;

#define MESSAGE_RPM_RECV        0
#define MESSAGE_RPM_START       1
#define MESSAGE_ADDR_LEN        64
/* 考虑字节对齐, 没有严格按照报文格式, 收到的消息解析成如下格式 */
typedef struct tagMAPMsgInfo
{
    uint16      len;  //长度
    CMT_FIELD_U cmt;  //控制域

    uint8       apdu[0]; //apdu内容是大端
}MAPMSG_INFO_T;


#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H__ */
