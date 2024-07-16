/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块GPRS和GSM功能头文件
* @date：    2019-10-29
* @history： 
*********************************************************************
*/

#ifndef __GPRS_GSM_H__
#define __GPRS_GSM_H__

#include "wireless.h"

#ifdef __cplusplus
extern "C" {
#endif


#define AT_INIT                       0         /*初始化*/
#define AT_ST                         1         /*模块开启后，等待模块状态*/
#define AT_AT                         2         /*发送AT检查模块*/
#define AT_CPIN                       3         /*判断SIM卡插入*/
#define AT_GMM                        4         /*读模块型号*/
#define AT_CCID                       5         /*读取SIM卡号*/
#define AT_CSQ                        6         /*读网络信号*/
#define AT_SYSINFO                    7         /*注册网络*/
#define AT_CREG                       8         /*注册网络*/
#define AT_CGDCONT                    9         /*设置APN*/
#define AT_CGATT                      10        /*检测gprs网络*/
#define AT_ATDT                       11        /*拨号*/
#define AT_PPP                        12        /*PPP验证*/
#define AT_LCP                        13        /*PPP链路协商*/
#define AT_IPCP                       14        /*PPP网口检查*/
#define AT_RESTART                    15        /*AT失败模块断电*/
#define AT_WAIT                       16        /*等待短信线程状态*/
#define PPP_SUCCESS                   17        /*拨号成功*/
#define PPP_CHECK                     18        /*连接状态检查*/
#define CONNECT_FAILED                19        /*连接失败*/

/* 串口模式 */
#define AT_CIMI                       21
#define AT_GETINFO                    22


#define GSM_INIT            0           /*初始化*/
#define GSM_AT              1           /*发送AT,检测模块*/
#define GSM_ATE0            2           /*发送ATE0,检测模块*/
#define GSM_CPIN            3           /*发送AT+CPIN?,查询(U)SIM 卡状态*/
#define GSM_CIMI            4           /*发送AT+CSQ,读信号*/
#define GSM_CSQ             5           /*发送AT+CREG?,注册网络*/
#define GSM_CREG            6           /*发送AT+CIMI,检测SIM卡*/
#define GSM_CMGF            7           /*发送AT+CMGF,设置短信格式*/
#define GSM_CPMS            8           /*发送AT+CPMS,设置短信存储模式*/
#define GSM_CSCA            9           /*短信中心号码*/
#define GSM_GPS             10          /*获取GPS*/
#define GSM_RESTART         11          /*AT失败模块断电*/
#define GSM_SUCCESS         12          /*成功*/
#define GSM_WORK            13          /*工作状态*/
#define GSM_INIT_END        14          /*初始化结束*/
#define GSM_AT_REBOOT       15          /*AT指令模块复位*/


#define GSM_LENGTH          500     /*短信息长度*/
#define GSM_GET_CNT         60      /*AT读取命令次数*/
#define MAX_MESSAGE_INDEX   60      /*短信最大存储数目*/
#define GSM_FLASH_CSQ_CNT   60      /*GSM 每循环10次，读一下信号*/


#define GMS_INIT_DEVICE      1     /*主通道运行状态 - 初始化*/
#define GMS_SEND_DEVICE      2     /*主通道运行状态 - 数据发送*/
#define GMS_RECV_DEVICE      3     /*主通道运行状态 - 数据接收*/
#define GMS_CLOSE_DEVICE     4     /*主通道运行状态 - 关闭通道*/
#define GMS_CHANGE_DEVICE    5     /*主通道运行状态 - 通道切换*/
#define GMS_SLEEP_DEVICE     6     /*主通道运行状态 - 通道休眠*/

typedef enum eFailedType
{
    GSM_FAILED_NULL = 0,
    GSM_FAILED_AT,
    GSM_FAILED_SIM,
    GSM_FAILED_REG,
    GSM_FAILED_CSQ,
}GSM_FAILED_E;

struct PDU_Addr_str         /*PDU地址格式*/
{
    uint8 len;                          /*长度*/
    uint8 type;                         /*号码类型*/
    uint8 addr[SMS_PDU_PHONE_LEN];      /*电话号码*/
};

typedef struct tagSMSPDURecvStr        /*发送方PDU格式*/
{
    struct PDU_Addr_str service_center_address;     /*短信中心地址*/
    uint8 pdu_type;                                 /*协议数据单元类型*/
    struct PDU_Addr_str originator_address;         /*发送方SME地址*/
    uint8 protocol_identifier;                      /*参数显示SMSC以何种方式处理SM (比如FAX,、Voice等)*/
    uint8 data_coding_scheme;                       /*参数表示用户数据(UD)编码方案*/
    uint8 service_time_stamp[7];                    /*收到消息的时间*/
    uint16 user_data_len;                           /*用户数据长度*/
    uint8  user_data[500];                          /*用户数据*/
}SMS_PDU_RECV_T;

typedef struct tagSMSPDUSendStr        /*发送方PDU格式*/
{
    struct PDU_Addr_str service_center_address;    /*短信中心地址*/
    uint8 pdu_type;                                /*协议数据单元类型*/
    uint8 message_reference;                     /*所有成功的SMS-SUBMIT参考数目(0..255)*/
    struct PDU_Addr_str destination_address;     /*接收方SME的地址*/
    uint8 protocol_identifier;                    /*参数显示SMSC以何种方式处理SM (比如FAX,、Voice等)*/
    uint8 data_coding_scheme;                    /*参数表示用户数据(UD)编码方案*/
    uint8 validity_period[7];                     /*消息在SMSC中不再有效的时长*/
    uint8 user_data_len;                        /*用户数据长度*/
    uint8 user_data[200];                        /*用户数据*/
}SMS_PDU_SEND_T;

/* 入口函数申明 */
typedef uint32 (*StatsProcFun)(uint8, char *, uint32, uint32 *, WIRELESS_HANDLE_T *, uint32 *);
typedef struct tagWorkMsgFun
{
    uint32             printNum;    /* 日志状态连续打印数量,0-默认都打印 */
    uint32             status;      /* 状态机 */
    StatsProcFun       func;
    char               name[24];    /* 状态名称 */
    char               info[48];    /* 状态栏显示内容, 可以为空 */
    uint32             showNum;     /* 状态栏连续刷新次数, 0-默认都刷新 */
    uint32             times;       /* 状态刷新时间 */
}WIRELESS_WORK_FUN_T;

int wireless_grps_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle);
int wireless_grpsnew_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle);
int wireless_gsm_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle);

#ifdef __cplusplus
}
#endif

#endif //__WIRELESS_H__


