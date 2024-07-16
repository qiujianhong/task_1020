/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块通道定义
* @date：    2020-3-5
* @history： 
*********************************************************************
*/

#ifndef _COMM_ROT_H_
#define _COMM_ROT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ADVPD_MSG_BUF_LENGTH      4096	/*缓冲区大小*/

/* 通信介质 */
#define COMM_MEDIA_BLUETOOTH      0    //蓝牙通信介质
#define COMM_MEDIA_GSM            1    //短信通信介质
#define COMM_MEDIA_GPRS           2    //无线通信介质
#define COMM_MEDIA_ETH            3    //以太网通信介质
#define COMM_MEDIA_LISTEN         4    //监听通信介质
#define COMM_MEDIA_485            5    //485通信介质


/* 通道大类定义 需要和内部通信配合 */
#define CHL_LISTEN                  0x00        //监听通道,   内部排号 0x00-0x0f
#define CHL_GPRS_1                  0x10        //无线通道1,  内部排号 0x10-0x13
#define CHL_GPRS_2                  0x14        //无线通道2   内部排号 0x14-0x17
#define CHL_ETH_1                   0x18        //以太网通道1 内部排号 0x18-0x1b
#define CHL_ETH_2                   0x1c        //以太网通道2 内部排号 0x1c-0x1f
#define CHL_RESERVE                 0x20        //保留
#define MAX_CHANNEL_NUM             CHL_RESERVE


#define PRO_DATA_TRANSMIT         0     /*内部协议 - 数据中转传送*/
#define PRO_LOGIN_FRAME           1     /*内部协议 - 登陆帧*/
#define PRO_EXIT_LOGIN_FRAME      2     /*内部协议 - 退出登陆*/
#define PRO_HEART_FRAME           3     /*内部协议 - 心跳帧*/
#define PROT_OPEN_CHANNLE         4     /*内部协议 - 打开通道*/
#define PRO_PARA_MOD              5     /*内部协议 - 通讯参数修改*/
#define PROT_CLOSE_CHANNLE        6     /*内部协议 - 关闭通道*/
#define PROT_APP104_START         10    /*内部协议 - app104启动 获取 通道开启数据*/
#define PROT_COMM_START           11    /*内部协议 - 通信开启 获取 通道开启数据*/

typedef struct tagCommIpcMsg
{
    uint8  id;       //通道
    uint8  protocol; //内部协议
    uint16 len;     //长度1
    uint8  msg[0];   //消息内容
}COMM_IPCMSG_T;


#define CHANNEL_CLIENT_TYPE         0
#define CHANNEL_SERVICE_TYPE        1

/* 通道属性 */
typedef struct tagChannelInfo
{
    uint32 ip;        //主站ip
    uint16 port;      //主站port
    uint8  workmode;  //工作模式 客户端、服务器
    uint8  medium;    //通信介质
    uint16 heart;     //心跳时间s
    uint8  res[2];
}CHANNEL_INFO_T;


#define COMM_LISTEN_MAX     10
#define COMM_IFNAME_LEN     32
#define COMM_ARG_LEN        64

typedef struct tagCommuArg
{
    char   ifName[COMM_IFNAME_LEN];
    uint8  bOpen;       //是否开启功能
    uint8  media;       //通信媒介
    uint8  no;          //内部编号 0或1
    uint8  connMode;    //COMM_CONNECTMODE_E

    char   apn[COMM_ARG_LEN];
    char   usr[COMM_ARG_LEN];
    char   psd[COMM_ARG_LEN];
    uint32 mIP;
    uint32 sIP;
    uint16 mPort;
    uint16 sPort;    
}COMM_ARG_T;

/* 网络配置参数 */
typedef struct tagCommuArgs
{
    uint16     port;
    uint8      num;
    uint8      res[1];

    COMM_ARG_T eth0;
    COMM_ARG_T eth1;
    COMM_ARG_T ppp0;
    COMM_ARG_T ppp1;
}COMM_ARGS_T;


#ifdef __cplusplus
}
#endif

#endif
