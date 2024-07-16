/*
*********************************************************************
* Copyright(C) 2021 China Electric Power Research Institute
* All rights reserved. 
* @brief：   串口APP消息接口
* @date：    2021/7/30
* @history： 
*********************************************************************
*/
#ifndef __UART_API_H_
#define __UART_API_H_

#include "message.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    UART_TYPE_485     = 0,
    UART_TYPE_232,
    UART_TYPE_CAN,
    UART_TYPE_UNLL    = 255,
}UART_TYPE_E;

typedef enum
{
    UART_MODE_ONLY     = 0,
    UART_MODE_MASTER,
    UART_MODE_SLAVE,
    UART_MODE_UNLL    = 255,
}UART_MODE_E;

typedef enum
{
    UART_BAUD_300     = 0,
    UART_BAUD_600,
    UART_BAUD_1200,
    UART_BAUD_2400,
    UART_BAUD_4800,
    UART_BAUD_7200,
    UART_BAUD_9600,
    UART_BAUD_19200,
    UART_BAUD_38400,
    UART_BAUD_57600,
    UART_BAUD_115200, //10
    UART_BAUD_230400, //CAN使用
    UART_BAUD_460800,
    UART_BAUD_10K,
    UART_BAUD_25K,
    UART_BAUD_50K,
    UART_BAUD_125K,
    UART_BAUD_250K,
    UART_BAUD_500K,
    UART_BAUD_1M,
    UART_BAUD_NULL = 255,
}UART_BAUDRATE_E;

typedef enum
{
    UART_PARITY_NONE     = 0,
    UART_PARITY_ODD,       //奇校验
    UART_PARITY_EVEN,      //偶校验
    UART_PARITY_SPACE,    
}UART_PARITY_E;

/*流控位*/
typedef enum tag_UART_FLOWCTRL_E  
{
    UART_FLOWCTRL_NONE = 0,
    UART_FLOWCTRL_HARD,
    UART_FLOWCTRL_SOFT,   
}UART_FLOWCTRL_E;

typedef enum
{
    UART_RESULT_OK       = 0,  //成功
    UART_RESULT_TIME_OUT,      //超时
    UART_RESULT_TASK_FULL,     //队列满, APP需要重发
    UART_RESULT_PARAM_ERR,     //参数错误
    UART_RESULT_DATA_ERR,      //数据错误 不符合Base64编码格式
    UART_RESULT_DATA_OTHER = 255,
}UART_RESULT_E;

typedef enum
{
    UART_ACK_OK      = 0,  //成功
    UART_ACK_OTHER,         //其它失败类型
    UART_ACK_TIMEOUT,       //帧超时
    UART_ACK_PORTERR = 4,   //端口错误
    UART_ACK_FMTERR,        //格式错误    
    UART_ACK_UNREG,         //未注册
    UART_ACK_MODEERR,       //模式错误
}UART_ACK_E;

typedef enum
{
    UART_ABNORMAL_ERROR_ANSWER     = 0,
    UART_ABNORMAL_UNKNOW_CODE,
}UART_ABNORMAL_E;

typedef struct tagUartPort
{
    UART_TYPE_E        portType;    //端口类型
    uint32             portNo;      //端口序号(配和类型)
}UART_PORT_T;

typedef struct tagMsgUartParam
{
    uint8             baudRate;   //UART_BAUDRATE_E
    uint8             dataBits;   //数据位: 5、6、7、8
    uint8             parity;     //校验位参数 UART_PARITY_E
    uint8             stopBits;   //停止位参数：1、2
    uint8             flowCtrl;   //流控制 UART_FLOWCTRL_E
}UART_MSG_PARAM_T;

/* 模式变更通知 */
typedef struct tagUartModeInfo
{
    UART_PORT_T        port;       //端口
    UART_MODE_E        mode;       //模式
    char               lastApp[64];//上一个占用端口的APP名称
    char               curApp[64]; //目前占用端口的APP名称
}UART_MODEINFO_T;

#define UART_MONITOR_MAX   20
/* 串口模式状态 */
typedef struct tagUartModeStatus
{
    UART_PORT_T        port;       //端口       
    UART_MODE_E        mode;       //模式
    char               onlyApp[64];//独占APP名称
    UART_MSG_PARAM_T   param;      //参数
    uint32             num;        //APP列表数量
    char               monitorApp[UART_MONITOR_MAX][64]; //数据监听APP列表
}UART_MODESTAUS_T;

/* 独占模式APP注册-数据监听APP注册 */
typedef struct tagUartAPPReg
{
    UART_PORT_T        port;       //端口       
    char               onlyApp[64];//独占APP名称
    UART_MSG_PARAM_T   param;      //参数 数据监听 无
}UART_APPREG_T;

#define UART_DATA_MAXLEN   4600
/* 串口发送消息结构体 */
typedef struct tagUartSendData
{
    UART_PORT_T        port;         //端口
    uint32             prio;         //优先级 0-高, 1-低
    uint32             prm;          //启动帧标志，0-应答帧、1-启动帧
    uint16             frametimeout; //帧超时时间 ,单位毫秒（ms）
    uint16             bytetimeout;  //字符超时时间 ,单位毫秒（ms）
    uint16             tasktimeout;  //串口最大任务等待时间（ms）
    uint32             bParam;       //是否携带参数 0-不携带(独占模式不携带), 1-携带
    UART_MSG_PARAM_T   param;        //参数
    uint16             estsize;      //预估最大接收字节
    uint16             datalen;      //数据长度
    uint8              data[UART_DATA_MAXLEN]; //数据
}UART_SEND_DATA_T;

/* 串口接收消息结构体 */
typedef struct tagUartRecvData
{
    UART_PORT_T        port;         //端口
    uint8              prm;          //启动标志 1-主动接收
    uint8              status;       //结果 UART_RESULT_E
    uint16             datalen;      //数据长度
    uint8              data[UART_DATA_MAXLEN]; //数据
}UART_RECV_DATA_T;

/* 串口模式设置 */
typedef struct tagUartMode
{
    UART_PORT_T        port;       //端口       
    char               AppName[64];//APP名称
    UART_MODE_E        mode;       //模式
}UART_MODE_T;

/* 串口异常 */
typedef struct tagUartAbnormal
{
    UART_PORT_T        port;       //端口       
    char               AppName[64];//APP名称
    UART_ABNORMAL_E    cause;      //异常原因
}UART_ABNORMAL_T;

/* 串口模式状态 */
typedef struct tagUartModeSet
{
    UART_PORT_T        port;       //端口       
    UART_MODE_E        mode;       //模式
    char               onlyApp[64];//独占APP名称
    uint32             bParam;     //是否携带参数 0-不携带(独占模式不携带), 1-携带
    UART_MSG_PARAM_T   param;      //参数
}UART_MODESET_T;

#define UART_BAUD_TO_STRING(baudint, baudE)\
do\
{\
    if(UART_BAUD_300 == baudE)\
    {\
        baudint = 300;\
    }\
    else if(UART_BAUD_600 == baudE)\
    {\
        baudint = 600;\
    }\
    else if(UART_BAUD_1200 == baudE)\
    {\
        baudint = 1200;\
    }\
    else if(UART_BAUD_2400 == baudE)\
    {\
        baudint = 2400;\
    }\
    else if(UART_BAUD_4800 == baudE)\
    {\
        baudint = 4800;\
    }\
    else if(UART_BAUD_7200 == baudE)\
    {\
        baudint = 7200;\
    }\
    else if(UART_BAUD_9600 == baudE)\
    {\
        baudint = 9600;\
    }\
    else if(UART_BAUD_19200 == baudE)\
    {\
        baudint = 19200;\
    }\
    else if(UART_BAUD_38400 == baudE)\
    {\
        baudint = 38400;\
    }\
    else if(UART_BAUD_38400 == baudE)\
    {\
        baudint = 38400;\
    }\
    else if(UART_BAUD_57600 == baudE)\
    {\
        baudint = 57600;\
    }\
    else if(UART_BAUD_115200 == baudE)\
    {\
        baudint = 115200;\
    }\
    else if(UART_BAUD_230400 == baudE)\
    {\
        baudint = 230400;\
    }\
    else if(UART_BAUD_460800 == baudE)\
    {\
        baudint = 460800;\
    }\
    else if(UART_BAUD_10K == baudE)\
    {\
        baudint = 10000;\
    }\
    else if(UART_BAUD_25K == baudE)\
    {\
        baudint = 25000;\
    }\
    else if(UART_BAUD_50K == baudE)\
    {\
        baudint = 50000;\
    }\
    else if(UART_BAUD_125K == baudE)\
    {\
        baudint = 125000;\
    }\
    else if(UART_BAUD_250K == baudE)\
    {\
        baudint = 250000;\
    }\
        else if(UART_BAUD_500K == baudE)\
    {\
        baudint = 500000;\
    }\
    else if(UART_BAUD_1M == baudE)\
    {\
        baudint = 1000000;\
    }\
    else\
    {\
        baudint = 2400;\
    }\
}while(0)

#define UART_STRING_TO_BAUD(baudint, baudE)\
do\
{\
    if(300 == baudint)\
    {\
        baudE = UART_BAUD_300;\
    }\
    else if(600 == baudint)\
    {\
        baudE = UART_BAUD_600;\
    }\
    else if(1200 == baudint)\
    {\
        baudE = UART_BAUD_1200;\
    }\
    else if(2400 == baudint)\
    {\
        baudE = UART_BAUD_2400;\
    }\
    else if(4800 == baudint)\
    {\
        baudE = UART_BAUD_4800;\
    }\
    else if(7200 == baudint)\
    {\
        baudE = UART_BAUD_7200;\
    }\
    else if(9600 == baudint)\
    {\
        baudE = UART_BAUD_9600;\
    }\
    else if(19200 == baudint)\
    {\
        baudE = UART_BAUD_19200;\
    }\
    else if(38400 == baudint)\
    {\
        baudE = UART_BAUD_38400;\
    }\
    else if(38400 == baudint)\
    {\
        baudE = UART_BAUD_38400;\
    }\
    else if(57600 == baudint)\
    {\
        baudE = UART_BAUD_57600;\
    }\
    else if(115200 == baudint)\
    {\
        baudE = UART_BAUD_115200;\
    }\
    else if(230400 == baudint)\
    {\
        baudE = UART_BAUD_230400;\
    }\
    else if(460800 == baudint)\
    {\
        baudE = UART_BAUD_460800;\
    }\
    else if(10000 == baudint)\
    {\
        baudE = UART_BAUD_10K;\
    }\
    else if(25000 == baudint)\
    {\
        baudE = UART_BAUD_25K;\
    }\
    else if(50000 == baudint)\
    {\
        baudE = UART_BAUD_50K;\
    }\
    else if(125000 == baudint)\
    {\
        baudE = UART_BAUD_125K;\
    }\
    else if(250000 == baudint)\
    {\
        baudE = UART_BAUD_250K;\
    }\
    else if(500000 == baudint)\
    {\
        baudE = UART_BAUD_500K;\
    }\
    else if(1000000 == baudint)\
    {\
        baudE = UART_BAUD_1M;\
    }\
    else\
    {\
        baudE = UART_BAUD_NULL;\
    }\
}while(0)

#define UART_OAD_TO_GPORT(portoad, porttype, portnum) \
do\
{\
    if((portoad&0xFFFFFF00) == 0xF2010200)\
    {\
        porttype = UART_TYPE_485;\
        portnum = portoad&0x000000FF;\
    }\
    else if((portoad&0xFFFFFF00) == 0xF2000200)\
    {\
        porttype = UART_TYPE_232;\
        portnum = portoad&0x000000FF;\
    }\
    else if((portoad&0xFFFFFF00) == 0xF2210200)\
    {\
        porttype = UART_TYPE_CAN;\
        portnum = portoad&0x000000FF;\
    }\
}while(0)

#define UART_GPORT_TO_OAD(portoad, porttype, portnum) \
do\
{\
    if(porttype == UART_TYPE_485)\
    {\
        portoad = 0xF2010200 + (portnum);\
    }\
    else if(porttype == UART_TYPE_232)\
    {\
        portoad = 0xF2000200 + (portnum);\
    }\
    else if(porttype == UART_TYPE_CAN)\
    {\
        portoad = 0xF2210200 + (portnum);\
    }\
}while(0)

/* 字符串转端口 */
#define UART_STRING_TO_PORT(portstring, porttype, portnum) \
do\
{\
    if(0 == memcmp(portstring, "RS485-", strlen("RS485-")))\
    {\
        porttype = UART_TYPE_485;\
        portnum = *((char*)portstring + strlen("RS485-")) - '0';\
    }\
    else if(0 == memcmp(portstring, "RS232-", strlen("RS232-")))\
    {\
        porttype = UART_TYPE_232;\
        portnum = *((char*)portstring + strlen("RS232-")) - '0';\
    }\
    else if(0 == memcmp(portstring, "CAN-", strlen("CAN-")))\
    {\
        porttype = UART_TYPE_CAN;\
        portnum = *((char*)portstring + strlen("CAN-")) - '0';\
    }\
    else\
    {\
        porttype = UART_TYPE_UNLL;\
    }\
}while(0)

/*
*********************************************************************
* @brief     : 模式变更通知解析
* @param[in] : MESSAGE_JSON_T *pMsg      接收消息JSON格式
* @param[out]: UART_ONLY_T *pMsgOnly     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_notify_mode_split(MESSAGE_JSON_T *pMsg, UART_MODEINFO_T *pMsgOnly);

/*
*********************************************************************
* @brief     : 模式变更通知封装
* @param[in] : uint32 token              消息序号
               UART_ONLY_T *pMsgOnly     串口独占模式通知
               int topiclen              主题缓存容量
* @param[out]: char *topic               主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_notify_mode_package(uint32 token, UART_MODEINFO_T *pMsgOnly, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 串口信息查询请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg      接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort      解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_get_info_req_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort);

/*
*********************************************************************
* @brief     : 串口信息查询请求消息封装
* @param[in] : uint32 token              消息序号
               char *sourapp             请求APP名称
               UART_PORT_T *pRSPort      端口
               int topiclen              主题缓存容量
* @param[out]: char *topic               主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_get_info_req_package(uint32 token, char *sourapp, UART_PORT_T *pRSPort, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 串口信息查询响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg          接收消息JSON格式
* @param[out]: UART_MODESTAUS_T *pModeStatus 解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_get_info_res_split(MESSAGE_JSON_T *pMsg, UART_MODESTAUS_T *pModeStatus);

/*
*********************************************************************
* @brief     : 串口信息查询响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_MODESTAUS_T *pModeStatus  模式与状态
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_get_info_res_package(uint32 token, char *destapp, UART_MODESTAUS_T *pModeStatus, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 串口独占模式APP注册请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_APPREG_T *pAppReg     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_set_param_req_split(MESSAGE_JSON_T *pMsg, UART_APPREG_T *pAppReg);

/*
*********************************************************************
* @brief     : 串口独占模式APP注册请求消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_APPREG_T *pAppReg         注册APP
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_set_param_req_package(uint32 token, char *sourapp, UART_APPREG_T *pAppReg, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 串口独占模式APP注册响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_set_param_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : 串口独占模式APP注册响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_PORT_T *pRSPort           端口
               char *status                   结果
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_set_param_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 数据监听注册请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_APPREG_T *pAppReg     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_set_monitor_req_split(MESSAGE_JSON_T *pMsg, UART_APPREG_T *pAppReg);

/*
*********************************************************************
* @brief     : 数据监听注册请求请求消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_APPREG_T *pAppReg         注册APP
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_set_monitor_req_package(uint32 token, char *sourapp, UART_APPREG_T *pAppReg, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 数据监听注册请求响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_set_monitor_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : 数据监听注册请求响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_PORT_T *pRSPort           端口
               char *status                   结果
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_set_monitor_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 数据发送消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_SEND_DATA_T *pSendMsg     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_data_send_split(MESSAGE_JSON_T *pMsg, UART_SEND_DATA_T *pSendMsg);

/*
*********************************************************************
* @brief     : 数据发送消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_SEND_DATA_T *pSendMsg     发送内容
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_data_send_package(uint32 token, char *sourapp, UART_SEND_DATA_T *pSendMsg, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 数据接收消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_RECV_DATA_T *pRecvMsg     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_data_recv_split(MESSAGE_JSON_T *pMsg, UART_RECV_DATA_T *pRecvMsg);

/*
*********************************************************************
* @brief     : 数据接收消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_RECV_DATA_T *pRecvMsg     接收消息
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_data_recv_package(uint32 token, char *destapp, UART_RECV_DATA_T *pRecvMsg, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 串口模式设置请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_MODE_T *pMode             解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_set_mode_req_split(MESSAGE_JSON_T *pMsg, UART_MODE_T *pMode);

/*
*********************************************************************
* @brief     : 串口模式设置请求消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_SEND_DATA_T *pSendMsg     发送内容
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_set_mode_req_package(uint32 token, char *sourapp, UART_MODE_T *pMode, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 串口模式设置响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_set_mode_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : 串口模式设置响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_PORT_T *pRSPort           端口
               char *status                   结果
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_set_mode_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 串口异常报告请求消息解析
* @param[in] : MESSAGE_JSON_T *pMsg           接收消息JSON格式
* @param[out]: UART_ABNORMAL_T *pAbnormal     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_set_abnormal_req_split(MESSAGE_JSON_T *pMsg, UART_ABNORMAL_T *pAbnormal);

/*
*********************************************************************
* @brief     : 串口异常报告请求消息封装
* @param[in] : uint32 token                   消息序号
               char *sourapp                  请求APP
               UART_ABNORMAL_T *pAbnormal     异常信息
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_set_abnormal_req_package(uint32 token, char *sourapp, UART_ABNORMAL_T *pAbnormal, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 串口异常报告响应消息解析
* @param[in] : MESSAGE_JSON_T *pMsg       接收消息JSON格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_json_set_abnormal_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : 串口异常报告响应消息封装
* @param[in] : uint32 token                   消息序号
               char *destapp                  目的APP
               UART_PORT_T *pRSPort           端口
               char *status                   结果
               int topiclen                   主题缓存容量
* @param[out]: char *topic                    主题
* @return    : JSON消息字符串
*********************************************************************
*/
char *uart_json_set_abnormal_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : 模式变更通知解析
* @param[in] : MESSAGE_INFO_T *pMsg      接收消息AXDR格式
* @param[out]: UART_ONLY_T *pMsgOnly     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_notify_mode_split(MESSAGE_INFO_T *pMsg, UART_MODEINFO_T *pMsgOnly);

/*
*********************************************************************
* @brief     : 模式变更通知封装
* @param[in] : UART_ONLY_T *pMsgOnly     串口独占模式通知
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_notify_mode_package(UART_MODEINFO_T *pMsgData, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口信息查询请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg      接收消息AXDR格式
* @param[out]: UART_PORT_T *pRSPort      解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_get_info_req_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pRSPort);

/*
*********************************************************************
* @brief     : 串口信息查询请求消息封装
* @param[in] : UART_PORT_T *pRSPort      端口
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_get_info_req_package(UART_PORT_T *pRSPort, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口信息查询响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg          接收消息AXDR格式
* @param[out]: UART_MODESTAUS_T *pModeStatus 解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_get_info_res_split(MESSAGE_INFO_T *pMsg, UART_MODESTAUS_T *pModeStatus);

/*
*********************************************************************
* @brief     : 串口信息查询响应消息封装
* @param[in] : UART_MODESTAUS_T *pModeStatus  模式与状态
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_get_info_res_package(UART_MODESTAUS_T *pModeStatus, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口独占模式APP注册请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: UART_APPREG_T *pAppReg     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_set_param_req_split(MESSAGE_INFO_T *pMsg, UART_APPREG_T *pAppReg);

/*
*********************************************************************
* @brief     : 串口独占模式APP注册请求消息封装
* @param[in] : UART_APPREG_T *pAppReg         注册APP
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_set_param_req_package(UART_APPREG_T *pAppReg, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口独占模式APP注册响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_set_param_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK);

/*
*********************************************************************
* @brief     : 串口独占模式APP注册响应消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_set_param_res_package(UART_PORT_T *pRSPort, uint8 status, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 数据监听注册请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: UART_APPREG_T *pAppReg     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_set_monitor_req_split(MESSAGE_INFO_T *pMsg, UART_APPREG_T *pAppReg);

/*
*********************************************************************
* @brief     : 数据监听注册请求请求消息封装
* @param[in] : UART_APPREG_T *pAppReg         注册APP
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_set_monitor_req_package(UART_APPREG_T *pAppReg, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 数据监听注册请求响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_set_monitor_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK);

/*
*********************************************************************
* @brief     : 数据监听注册请求响应消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_set_monitor_res_package(UART_PORT_T *pRSPort, uint8 status, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 数据发送消息解析
* @param[in] : MESSAGE_INFO_T *pMsg           接收消息AXDR格式
* @param[out]: UART_SEND_DATA_T *pSendMsg     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_data_send_req_split(MESSAGE_INFO_T *pMsg, UART_SEND_DATA_T *pSendMsg);

/*
*********************************************************************
* @brief     : 数据发送消息封装
* @param[in] : UART_SEND_DATA_T *pSendMsg     发送内容
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_data_send_req_package(UART_SEND_DATA_T *pSendMsg, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口异常报告响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_data_send_res_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : 串口异常报告响应消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_data_send_res_package(UART_PORT_T *pRSPort, uint8 status, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 数据接收消息解析
* @param[in] : MESSAGE_INFO_T *pMsg           接收消息AXDR格式
* @param[out]: UART_RECV_DATA_T *pRecvMsg     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_data_recv_split(MESSAGE_INFO_T *pMsg, UART_RECV_DATA_T *pRecvMsg);

/*
*********************************************************************
* @brief     : 数据接收消息封装
* @param[in] : UART_RECV_DATA_T *pRecvMsg     接收消息
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_data_recv_package(UART_RECV_DATA_T *pRecvMsg, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口异常报告请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg           接收消息AXDR格式
* @param[out]: UART_ABNORMAL_T *pAbnormal     解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_set_abnormal_req_split(MESSAGE_INFO_T *pMsg, UART_ABNORMAL_T *cause);

/*
*********************************************************************
* @brief     : 串口异常报告请求消息封装
               UART_ABNORMAL_T *pAbnormal     异常信息
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_set_abnormal_req_package(UART_ABNORMAL_T *pMsgData, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口异常报告响应消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: UART_PORT_T *pRSPort       解析结果
               uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int uart_axdr_set_abnormal_res_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : 串口异常报告响应消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
*********************************************************************
*/
uint16 uart_axdr_set_abnormal_res_package(UART_PORT_T *pRSPort, BOOLEAN status, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口模式设置请求消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_mode_req_split(MESSAGE_INFO_T *pMsg, UART_MODESET_T *pModeSet);

/*
*********************************************************************
* @brief     : 串口模式设置请求消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_mode_req_package(UART_MODESET_T *pModeSet, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : 串口模式设置回码消息解析
* @param[in] : MESSAGE_INFO_T *pMsg       接收消息AXDR格式
* @param[out]: uint8 *bOK                 是否返回OK TRUE-OK/FALSE-失败
* @return    : 0-成功/其他-错误码
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_mode_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK);

/*
*********************************************************************
* @brief     : 串口模式设置回码消息封装
* @param[in] : UART_PORT_T *pRSPort           端口
               BOOLEN status                  结果
               uint16 maxlen         缓存大小
* @param[out]：uint8 *payload         负荷内容             
* @return    : payload长度
* @Create    : 贺宁
* @Date      ：2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_mode_res_package(UART_PORT_T *pMsgData, uint8 status, uint8 *payload, uint16 maxlen);

int cco_json_data_recv_split(MESSAGE_JSON_T *pMsg, UART_RECV_DATA_T *pRecvMsg);


#ifdef __cplusplus
}
#endif

#endif /* __UART_API_H_ */

