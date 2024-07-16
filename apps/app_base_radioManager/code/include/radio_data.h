/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP 公共数据
* @date：    2021-7-8
* @history： 
*********************************************************************
*/

#ifndef __RADIO_DATA_H__
#define __RADIO_DATA_H__

#include "radio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIN_ENABLE            1

#define RADIO_APP_NAME              "radioManager"
#define RADIO_APP_NAME_M            "M-radioManager"
#define RADIO_MASTER_MAX            2
#define RADIO_SLAVE_MAX             3

/* 电台接收默认超时时间 */
#if defined PRODUCT_ECU || defined PRODUCT_ECU_G
#define RADIO_RECV_TIME_OUT        50
#else
#define RADIO_RECV_TIME_OUT        200
#endif

#define RADIO_APPID_MAX            32

#define RADIO_PORT_MAX              1

typedef struct RADIODevMsg
{
    struct RADIODevMsg *pNext;
    
    uint8             priority;   //优先级
    uint8             type;       //MESSAGE_TYPE_A_XDR, MESSAGE_TYPE_JSON
    uint8             res[2];
    uint32            token;      //序号(AXDR 是 label 与 index)
    char              source[128];//发送方

    uint16            proc;       //操作类型(同IOP)
    uint16            datalen;
    uint8             data[0];
}RADIO_DEV_MSG_T;

typedef struct tagRADIOQueue	    /*锁共享缓冲区结构*/
{
	pthread_mutex_t f_lock;
	RADIO_DEV_MSG_T  *pHead;
}RADIO_QUEUE_T;

#if 0
/* 电台设备相关配置状态 */
typedef struct tagRADIOPortStatus
{
    uint8             bConfig;  //是否配置过
    uint8             bConnect;
    uint8             channelid; //通道(1-3)
    //RADIO_PORT_CONF_T   porttCfg; //端口配置
    time_t            lasttime; //最后一次连接、通信时间s(相对)
    uint32            lastus;   //最后一次连接、通信时间us(相对)
}RADIO_PORT_STATUS_T;

/* 从模式端口管理 */
typedef struct tagRADIODevPort
{
    uint8             bChannel; //通道(1-3)
    uint8             bConnect;
    uint8             MAC[6];   //MAC地址
    uint32            port;     /* 端口号 */
    RADIO_MODE_E        mode;     /* 模式 RADIO_MODE_E */
    RADIO_MATCH_T       match;    /* 配对参数 */
    time_t            lasttime; //最后一次连接、通信时间s(相对)
    uint32            lastus;   //最后一次连接、通信时间us(相对)

    uint16            timeout;                //超时时间ms(透传)
    uint16            cachelen;               //缓存大小
    uint8             cache[RADIO_MSG_MAX_LEN]; //缓存 4K

    RADIO_DEV_MSG_T    *pTranMsg;               //转发消息
}RADIO_DEV_PORT_T;
#endif

#pragma pack(1)

/* 电台参数地址 */
typedef struct tagRADIO_CONF_T
{
    uint8   channel;        /* 频道 */
    uint8   power;          /* 功率 0:"01.00w", 1:"02.50w", 2:"05.00w", 3:"07.50w", 4:"10.00w" */
    uint8   sensitivity;    /* 静噪级别 0:-12dBu, 1:-9dBu, 2:-6dBu, 3:-3dBu, 4:0dBu, 5:3dBu, 6:6dBu, 7:9dBu, 8:12dBu */
    //uint8   modulation;     /* 调制方式 34:江苏4FSK */
    //uint8   freqOffset;     /* 频偏值 */
    uint8   areaCodeLen;
    uint8   areaCode[2];    /* 行政区号 */
    uint8   devAddrLen;
    uint8   devAddr[3];     /* 设备地址 */
    uint8   tmnAddrLen;
    uint8   tmnAddr[6];     /* 终端地址 */
}RADIO_CONF_T;

/* 电台参数 */
typedef struct tagRADIO_PARA_T
{
    uint8   channel;        /* 频道 */
    uint8   power;          /* 功率 0:"01.00w", 1:"02.50w", 2:"05.00w", 3:"07.50w", 4:"10.00w" */
    uint8   sensitivity;    /* 静噪级别 0:-12dBu, 1:-9dBu, 2:-6dBu, 3:-3dBu, 4:0dBu, 5:3dBu, 6:6dBu, 7:9dBu, 8:12dBu */
}RADIO_PARA_T;

/* 电台地址 */
typedef struct tagRADIO_ADDR_T
{
    uint8   areaCodeLen;
    uint8   areaCode[2];    /* 行政区号 */
    uint8   devAddrLen;
    uint8   devAddr[3];     /* 设备地址 */
    uint8   tmnAddrLen;
    uint8   tmnAddr[6];     /* 终端地址 */
}RADIO_ADDR_T;

#pragma pack()

/* 电台设备相关配置 */
typedef struct tagRADIODev
{
    //uint8             bSleep;     //模块状态(是否处于睡眠)
    uint8             bConfig;    //是否设置过配置
    uint8             bParaConfig;    //是否设置过配置
    uint8             bAddrConfig;    //是否设置过配置
    //uint8             bMatch;     //是否设置过主模式配置参数
    //uint8             bCheckMeter;//检表配置(生效时从链接断开或不响应)
    RADIO_CONF_T      config;     //电台参数(保存数据)
    //RADIO_MATCH_T       match;      //主模式配对参数(保存数据)
    //RADIO_SN_T          sn;         //电台sn

    //#if PIN_ENABLE == 1
    //RADIO_BROADCAST_T   broadcast;  //广播参数
    //#endif
    
    //uint16            portnum;    //端口数量(从模式)
    //RADIO_PORT_STATUS_T PortCfg[RADIO_PORT_MAX];  //端口配置(保存数据)
    
    //RADIO_DEV_PORT_T    mPortStatus[RADIO_MASTER_MAX];//端口状态(主模式)
    //RADIO_DEV_PORT_T    sPortStatus[RADIO_SLAVE_MAX]; //端口状态(从模式)
}RADIO_DEV_T;


/* 最大接受缓存62K */
#define RADIO_RECV_BUF_LEN          63488
#define RADIO_DEV_STATUS_INIT       0
#define RADIO_DEV_STATUS_PREPARE    1
#define RADIO_DEV_STATUS_GETCONN    2
#define RADIO_DEV_STATUS_WORK       3

/* 电台设备线程数据 */
typedef struct tagRADIODevHandle
{
    uint8             status;                //状态机
    uint8             res[1];
    uint16            recvlen;               //接收句柄
    uint8             recv[RADIO_RECV_BUF_LEN];//接收缓存

    int               devfd;                 //设备句柄
    int               chkfdcnt;              //句柄检测计时
    int               cfgDelay;              //参数延迟
    
    RADIO_QUEUE_T       cfgQueue;              //配置队列
    RADIO_QUEUE_T       transQueue;            //转发队列(高于消息队列)
    RADIO_QUEUE_T       msgQueue;              //消息队列
}RADIO_DEV_H_T;


/* 电台全局变量 */
typedef struct tagRADIODATA
{
    uint32          mqttip;       //mqtt地址
    uint16          mqttport;     //mqtt端口
    uint8           appidlen;
    uint8           appid[RADIO_APPID_MAX];
    uint8           bReg;         //注册成功
    uint8           bWork;        //开始工作(存在设备)

    RADIO_DEV_T       tDevStatus;   //配置状态
    RADIO_DEV_H_T     tDevPorc;     //设备操作
}RADIO_DATA_T;

void radio_clean_queue(RADIO_QUEUE_T *pQueue);
RADIO_DEV_MSG_T *radio_read_queue(RADIO_QUEUE_T *pQueue);
void radio_write_queue(RADIO_QUEUE_T *pQueue, RADIO_DEV_MSG_T *pMsgProc, uint8 bOrder);

int radio_write_config(RADIO_CONF_T *pConfig);
//int radio_write_match(RADIO_MATCH_T *pMatch);
//int radio_write_portcfg(uint32 port, RADIO_PORT_CONF_T *pPortcfg);
int radio_stoi(const char *str);

#ifdef __cplusplus
}
#endif

#endif //__RADIO_DATA_H__

