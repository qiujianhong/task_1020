/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块 串口(蓝牙)通信头文件
* @date：    2019-10-31
* @history： 
*********************************************************************
*/
#ifndef	__RS232_COMM_H
#define __RS232_COMM_H

#ifdef __cplusplus
        extern "C" {
#endif

#define COMM_BT_STATUS_GETAPP   0
#define COMM_BT_STATUS_SETCFG   1
#define COMM_BT_STATUS_GETPORT  2
#define COMM_BT_STATUS_SETPORT  3
#define COMM_BT_STATUS_PREPARE  4
#define COMM_BT_STATUS_WORK     5

/* 蓝牙线程句柄 */
typedef struct tagBlueHandle
{
    uint32             port1;     /* 端口号 */
    uint32             port2;     /* 端口号 */

    uint8              YCMAC[6];  /* 终端地址转的蓝牙MAC */
    char               name[4];   /* 蓝牙名称 */
    OOP_BTWORKPARAMS_T tBlueConf; /* 蓝牙配置 */
    
    uint8              bGetCfg;   /* 是否获取到配置 */
    uint8              btStatus;  /* 模组管理器查询状态 */
    uint8              bMac1Con;  /* 第一个MAC连接 */
    uint8              bMac2Con;  /* 第二个MAC连接 */
    uint8              res[2];
    
    char               MasterMAC1[6];   /* 被连接的第一个MAC */
    char               MasterMAC2[6];   /* 被连接的第二个MAC */

    BLE_CONFIG_T       btCfg;     /* 蓝牙芯片设置 */
    
    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;
    
    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_BLUE_HANDLE_T;

/* 232线程句柄 */
typedef struct tag232Handle
{
    uint8              bGetCfg;    /* 是否获取到配置 */
    uint8              res[3];
    COMM_QUEUE_LIST_T *ptRecvList;
    COMM_QUEUE_LIST_T *ptSendList;
    void              *pUart;
    
    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_232_HANDLE_T;

#ifdef PRODUCT_SCU
#define COMM_UART_ID_MAX  4
#define COMM_UART_ID  3
#else
#define COMM_UART_ID_MAX  2
#define COMM_UART_ID  1
#endif

//#ifdef PRODUCT_ECU
#if 0
#define COMM_485_GET_PORT    0
#define COMM_485_GET_CHEECK  1
#define COMM_485_GET_WAIT    2
#define COMM_485_GET_WORK    3

#define COMM_MOD_ID_MIN          1
#define COMM_MOD_ID_MAX          5

/* 第四路485通信口 */
typedef struct tag485Handle
{
    uint8              bNoMeter;   /* 档案没有配置第4路485 */
    uint8              bModeUpdate;/* 模式变更标记*/
    uint8              port485_1;  /* 485端口位置 */
    uint8              chanDefend; /* 维护口位置 */
    uint8              port_1;     /* 获取485左模组位置 */
    uint8              port_2;     /* 获取485右模组位置 目前用不上 */

    uint8              mapStatus;  /* 模组管理器查询状态 */
    uint8              count;      /* 状态执行次数 */
    uint8              res[2];
    MOD_INFO_ACK_T     module[COMM_MOD_ID_MAX];   //下标加1 表示模组序号

    COMM_QUEUE_LIST_T *ptRecvList;  //接收队列
    COMM_QUEUE_LIST_T *ptSendList;  //发送队列

    void              *pUart;      /* 串口ID */

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_485_HANDLE_T;
#else

#define COMM_UART_STATUS_GETAPP   0
#define COMM_UART_STATUS_CHECK    1
#define COMM_UART_STATUS_SETUART  2
#define COMM_UART_STATUS_WORK     3

/* 模式变更通知 */
typedef struct tagCommUartMode
{
    uint8              bGet;        //获取数据
    uint8              bOnly;       //是否独占模式
    uint8              bSlave;      //是否从模式
    uint8              res[1];
    UART_MODEINFO_T    modeinfo;    //模式信息
}COMM_UART_MODE_T;

/* 第四路485通信口 */
typedef struct tag485Handle
{
    uint8              uartStatus;  //模组管理器查询状态
    uint8              workNO;      //参与工作的串口下标
    uint8              bNoMeter;    //是否配置4路485档案
    uint8              res[1];
    UART_MSG_PARAM_T   param;       //参数
    COMM_UART_MODE_T   uartMode[COMM_UART_ID_MAX]; //串口

    COMM_QUEUE_LIST_T *ptRecvList;  //接收队列
    COMM_QUEUE_LIST_T *ptSendList;  //发送队列

    uint8             *pAppReStart;
    uint8             *pIPCok;
    uint8             *pRunFlag;
}COMM_485_HANDLE_T;
#endif


int comm_getcheck_bluetooth(int sockdbfd, void *pCfg, uint8 *pRestart);
void comm_timer_bt_status(int socketfd, void *pArg);
int comm_bt_getcfgmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_bt_setcfgmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_bt_getportmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_bt_setportmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_bt_recvmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_bt_portevent(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
BOOLEAN bluetooth_start(void *pArg);
void *bluetooth_entry(void *pArg);

#if 0
BOOLEAN comm_rs485_start(void *pArg);
void *comm_rs485_entry(void *pArg);
void comm_timer_485_status(int socketfd, void *pArg);
#else
BOOLEAN comm_rs485_start(void *pArg);
void *comm_rs485_entry(void *pArg);
void comm_timer_485_status(int socketfd, void *pArg);
int comm_485_statusmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_485_setmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_485_regappmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_485_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
int comm_485_recvmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg);
#endif
BOOLEAN comm_rs232_start(void *pArg);
void *comm_rs232_entry(void *pArg);

BOOLEAN comm_infrared_start(void *pArg);
void *comm_infrared_entry(void *pArg);

#ifdef __cplusplus
}
#endif

#endif
