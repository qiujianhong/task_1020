/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP 公共数据
* @date：    2021-7-8
* @history： 
*********************************************************************
*/

#ifndef __BLE_DATA_H__
#define __BLE_DATA_H__

#include "ble.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PIN_ENABLE            1

#define BT_APP_NAME              "btManager"
#define BT_APP_NAME_M            "M-btManager"
#define BT_MASTER_MAX            2
#define BT_SLAVE_MAX             3

/* 蓝牙接收默认超时时间 */
#ifdef PRODUCT_ECU
#define BLE_RECV_TIME_OUT        50
#else
#define BLE_RECV_TIME_OUT        200
#endif

#define BLE_APPID_MAX            32

typedef struct BLEDevMsg
{
    struct BLEDevMsg *pNext;
    
    uint8             priority;   //优先级
    uint8             type;       //MESSAGE_TYPE_A_XDR, MESSAGE_TYPE_JSON
    uint8             res[2];
    uint32            token;      //序号(AXDR 是 label 与 index)
    char              source[128];//发送方

    uint16            proc;       //操作类型(同IOP)
    uint16            datalen;
    uint8             data[0];
}BLE_DEV_MSG_T;

typedef struct tagBLEQueue	    /*锁共享缓冲区结构*/
{
	pthread_mutex_t f_lock;
	BLE_DEV_MSG_T  *pHead;
}BLE_QUEUE_T;


/* 蓝牙设备相关配置状态 */
typedef struct tagBLEPortStatus
{
    uint8             bConfig;  //是否配置过
    uint8             bConnect;
    uint8             channelid; //通道(1-3)
    BLE_PORT_CONF_T   porttCfg; //端口配置
    time_t            lasttime; //最后一次连接、通信时间s(相对)
    uint32            lastus;   //最后一次连接、通信时间us(相对)
}BLE_PORT_STATUS_T;

/* 从模式端口管理 */
typedef struct tagBLEDevPort
{
    uint8             bChannel; //通道(1-3)
    uint8             bConnect;
    uint8             MAC[6];   //MAC地址
    uint32            port;     /* 端口号 */
    BLE_MODE_E        mode;     /* 模式 BLE_MODE_E */
    BLE_MATCH_T       match;    /* 配对参数 */
    time_t            lasttime; //最后一次连接、通信时间s(相对)
    uint32            lastus;   //最后一次连接、通信时间us(相对)

    uint16            timeout;                //超时时间ms(透传)
    uint16            cachelen;               //缓存大小
    uint8             cache[BLE_MSG_MAX_LEN]; //缓存 4K

    BLE_DEV_MSG_T    *pTranMsg;               //转发消息
}BLE_DEV_PORT_T;

/* 蓝牙设备相关配置 */
typedef struct tagBLEDev
{
    uint8             bSleep;     //模块状态(是否处于睡眠)
    uint8             bConfig;    //是否设置过配置
    uint8             bMatch;     //是否设置过主模式配置参数
    uint8             bCheckMeter;//检表配置(生效时从链接断开或不响应)
    BLE_CONFIG_T      config;     //蓝牙参数(保存数据)
    BLE_MATCH_T       match;      //主模式配对参数(保存数据)
    BLE_SN_T          sn;         //蓝牙sn

    #if PIN_ENABLE == 1
    BLE_BROADCAST_T   broadcast;  //广播参数
    #endif
    
    uint16            portnum;    //端口数量(从模式)
    BLE_PORT_STATUS_T PortCfg[BLE_PORT_MAX];  //端口配置(保存数据)
    
    BLE_DEV_PORT_T    mPortStatus[BT_MASTER_MAX];//端口状态(主模式)
    BLE_DEV_PORT_T    sPortStatus[BT_SLAVE_MAX]; //端口状态(从模式)
}BLE_DEV_T;


/* 最大接受缓存62K */
#define BLE_RECV_BUF_LEN          63488
#define BLE_DEV_STATUS_INIT       0
#define BLE_DEV_STATUS_PREPARE    1
#define BLE_DEV_STATUS_GETCONN    2
#define BLE_DEV_STATUS_WORK       3

/* 蓝牙设备线程数据 */
typedef struct tagBLEDevHandle
{
    uint8             status;                //状态机
    uint8             res[1];
    uint16            recvlen;               //接收句柄
    uint8             recv[BLE_RECV_BUF_LEN];//接收缓存

    int               devfd;                 //设备句柄
    int               chkfdcnt;              //句柄检测计时
    int               cfgDelay;              //参数延迟
    
    BLE_QUEUE_T       cfgQueue;              //配置队列
    BLE_QUEUE_T       transQueue;            //转发队列(高于消息队列)
    BLE_QUEUE_T       msgQueue;              //消息队列
}BLE_DEV_H_T;


/* 蓝牙全局变量 */
typedef struct tagBLEDATA
{
    uint32          mqttip;       //mqtt地址
    uint16          mqttport;     //mqtt端口
    uint8           appidlen;
    uint8           appid[BLE_APPID_MAX];
    uint8           bReg;         //注册成功
    uint8           bWork;        //开始工作(存在设备)

    BLE_DEV_T       tDevStatus;   //配置状态
    BLE_DEV_H_T     tDevPorc;     //设备操作
}BLE_DATA_T;



void ble_clean_queue(BLE_QUEUE_T *pQueue);
BLE_DEV_MSG_T *ble_read_queue(BLE_QUEUE_T *pQueue);
void ble_write_queue(BLE_QUEUE_T *pQueue, BLE_DEV_MSG_T *pMsgProc, uint8 bOrder);

int ble_write_config(BLE_CONFIG_T *pConfig);
int ble_write_match(BLE_MATCH_T *pMatch);
int ble_write_portcfg(uint32 port, BLE_PORT_CONF_T *pPortcfg);
int ble_stoi(const char *str);

#ifdef __cplusplus
}
#endif

#endif //__BLE_DATA_H__

