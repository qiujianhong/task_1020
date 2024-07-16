/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   蓝牙消息接口
* @date：    2021/7/9
* @history： 
*********************************************************************
*/
#ifndef __BLE_H_
#define __BLE_H_


#ifdef __cplusplus
extern "C"
{
#endif

#define BLE_MSG_MAX_LEN    4096
#define BLE_NAME_MAX_LEN   8
#define BLE_PWD_MAX        16
#define BLE_PORT_MAX       32
/* 端口号0-1默认是主动模式(应该是从动模式, 林洋开发成这样也就这样吧),被主机连接 */
#define BLE_PORT_MASTER0   0
#define BLE_PORT_MASTER1   1

#define BLE_CHANNEL_MAX    5

#define BLE_SN_MAX         64

/* 蓝牙MAC类型 */
typedef enum _BLE_MAC_TYPE
{
    BLE_MAC_UNKNOW  = 0,
    BLE_MAC_METER   = 1,              /* 电能表 */
    BLE_MAC_GATHER  = 2,              /* 采集设备：采集器、集中器等 */
    BLE_MAC_DETECT  = 3,              /* 检测设备：台体、自动检测线 */
    BLE_MAC_SWITCH  = 4,              /* 负荷开关 */
    BLE_MAC_LOCK    = 5,              /* 智能锁具 */
    BLE_MAC_HANDDEV = 6,              /* 运维手持设备 */
}BLE_MAC_E;

typedef enum _BLE_MODE_TYPE
{
    BLE_MODE_MASTER = 0,              /* 主 */
    BLE_MODE_SLAVE  = 1,              /* 从 */
}BLE_MODE_E;

typedef enum _BLE_MODE_MATCH
{
    BLE_MATCH_NC    = 0,              /* Numeric Comparison */
    BLE_MATCH_JW    = 1,              /* Just Works */
    BLE_MATCH_PE    = 2,              /* Passkey Entry */
    BLE_MATCH_OB    = 3,              /* Out of Band */
}BLE_MATCH_E;

/* 蓝牙错误码 */
typedef enum _BLE_ERROR_CODE
{
    BLE_SUCCESS            = 0,              /* 成功 */
    BLE_ERR_UNKNOW_CMD     = 1,              /* 控制命令未定义 */
    BLE_ERR_UNEXIST_MAC    = 2,              /* 设备地址不存在 */
    BLE_ERR_CONNECT_FAIL   = 3,              /* 目标设备连接失败 */
    BLE_ERR_TRANS_FAIL     = 4,              /* 蓝牙链路透传失败 */
    BLE_ERR_OVER_LINKMAX   = 5,              /* 蓝牙链路数超过设置值 */
    BLE_ERR_MSG_PARAM      = 6,              /* 数据报文参数错误 */
    BLE_ERR_NO_MATCH       = 16,             /* 无设备配对信息 */
    BLE_ERR_MALLOC_FAIL    = 17,             /* 申请内存失败 */
    BLE_ERR_OPER_CHECK     = 253,            /* 检表过程中非法操作蓝牙 */
    BLE_ERR_OPER_FAIL      = 254,            /* 执行失败 */
    BLE_ERR_OTHER_FAIL     = 255,            /* 其它错误 */
}BLE_ERROR_E;

/* 蓝牙错误码 */
typedef enum _BLE_CHECK_TYPE
{
    BLE_TYPE_SECOND        = 0,              /* 秒脉冲 */
    BLE_TYPE_DEMAND        = 1,              /* 需量周期 */
    BLE_TYPE_PERIOD        = 2,              /* 时段投切 */
    BLE_TYPE_FORWARD_HARM  = 3,              /* 正向谐波 */
    BLE_TYPE_REVERSE_HARM  = 4,              /* 反向谐波 */
    BLE_TYPE_REACTIVE      = 5,              /* 无功脉冲 */
    BLE_TYPE_ACTIVE        = 6,              /* 有功脉冲	 */
    BLE_TYPE_CLOSE         = 255,            /* 关闭 */
}BLE_CHECK_E;


/* 报文通知 */
typedef struct tagBLEMsgEvent
{
    uint32   port;                    /* 端口号 */
    uint8    MAC[6];                  /* MAC地址 */
    uint16   len;                     /* 报文长度 */
    uint8    msg[BLE_MSG_MAX_LEN];    /* 报文内容 */
}BLE_MSG_EVENT_T;

/* 蓝牙端口通知 */
typedef struct tagBLEPortEvent
{
    uint32   port;                    /* 端口号 */
    uint8    MAC[6];                  /* MAC地址 */
    uint8    mode;                    /* 模式 BLE_MODE_E */
    uint8    bConnect;                /* 是否连接(TRUE-连接, FALSE-未连接) */
}BLE_PORT_EVENT_T;

/* 蓝牙数据 */
typedef struct tagBLEMSG
{
    uint32   port;                       /* 端口号 */
    uint32   len;                        /* 报文长度 */
    uint8    msg[BLE_MSG_MAX_LEN];       /* 报文内容 */
}BLE_MSG_T;

/* 配对参数 */
typedef struct tagBLEMatchPara
{
    BLE_MATCH_E  mode;                   /* 配对模式 */
    uint32       len;                    /* 密码长度 */
    char         pwd[BLE_PWD_MAX + 4];   /* 配对密码 */
}BLE_MATCH_T;

/* 超时时间 */
typedef struct tagBLEWaitTime
{
    uint16    msgtime;                   /* 帧超时时间 ms */
    uint16    bytetime;                  /* 字符超时时间 ms */
}BLE_WAIT_TIME_T;

/* 蓝牙转发请求 */
typedef struct tagBLETransReq
{
    uint32          port;                /* 端口号 */
    BLE_MATCH_T     match;               /* 配对参数 */
    uint8           MAC[6];              /* MAC地址 */
    uint16          len;                 /* 内容长度 */
    uint8           msg[BLE_MSG_MAX_LEN];/* 发送内容 */
    BLE_WAIT_TIME_T waittime;            /* 等待接收, 内容全0表示不等待 */
}BLE_TRANS_REQ_T;

/* 蓝牙转发应答 */
typedef struct tagBLETransAck
{
    int             ret;                 /* 发送结果 */
    uint32          len;                 /* 接收帧长度 */
    uint8           msg[BLE_MSG_MAX_LEN];/* 接收帧内容 */
}BLE_TRANS_ACK_T;

/* 蓝牙参数 */
typedef struct tagBLEConfig
{
    uint8    MAC[6];                     /* MAC地址 */
    uint8    power;                      /* 发射功率档 0:4dBm(8米), 1:0dBm, 2: -4dBm, 3: -8dBm, 4: -20dBm(2米) */
    uint8    len;                        /* 名称长度 */
    char     name[BLE_NAME_MAX_LEN];     /* 名称 */
    uint16   broadcast;                  /* 广播间隔 ms 40ms~1000ms，默认40ms */
    uint16   scan;                       /* 扫描间隔 ms 55ms~110ms，默认100 */
}BLE_CONFIG_T;

/* 蓝牙端口配置 */
typedef struct tagBLETPortConf
{
    uint32          port;                /* 端口号 */
    BLE_MODE_E      mode;                /* 模式 BLE_MODE_E */
    uint8           MAC[6];              /* MAC地址 */
    uint8           res[2];
    BLE_MATCH_T     match;               /* 配对参数 */
}BLE_PORT_CONF_T;

/* 蓝牙端口配置表 */
typedef struct tagBLETPortsConf
{
    uint32          num;                 /* 端口数量 */
    BLE_PORT_CONF_T tConf[BLE_PORT_MAX]; /* 端口配置 */
}BLE_PORTS_CONF_T;

/* 蓝牙连接节点信息 */
typedef struct tagBLENodeConn
{
    uint32   port;                       /* 端口号 */
    uint8    mode;                       /* 模式 BLE_MODE_E */
    uint8    bConnect;                   /* 是否连接 */
    uint8    MAC[6];                     /* MAC地址 */
}BLE_NODE_CONN_T;

/* 蓝牙连接信息 */
typedef struct tagBLEConnectInfo
{
    uint32    num;                       /* 数量 */
    BLE_NODE_CONN_T list[BLE_PORT_MAX];  /* 节点连接信息 */
}BLE_CONNECT_INFO_T;

/* 蓝牙SN */
typedef struct tagBLESn
{
    uint32    num;                       /* 数量 */
    uint8     sn[BLE_SN_MAX];            /* sn */
}BLE_SN_T;

/* 蓝牙检定 */
typedef struct tagBLECheck
{
    BLE_CHECK_E     type;               /* 蓝牙检定类型 */
    uint8           interval;           /* 信道发射间隔 ms */
    uint8           power;              /* 发射功率 */
    uint8           MAC[6];             /* 通信地址 */
    uint8           ch_num;             /* 通道数量 */
    uint16          freq[BLE_CHANNEL_MAX];/* 通道频点 */
}BLE_CHECK_T;

/* 广播参数 */
typedef struct tagBLEBROADCAST
{
    uint8    flagLen;                       /* 广播数据标志 */
    uint8    flagType;
    uint8    flagValue;
    uint8    factLen;                       /* 厂商数据长度 */
    uint8    factType;                      /* 厂商数据类型 */
    uint8    devCode;                       /* 设备识别码 */
    uint8    factCode[2];                   /* 厂商代码 */
    uint8    pinType[2];                    /* pin类型 */
    uint8    pin[16];                       /* pin类型 */ 
    uint8    devLen;                        /* 设备信息 */
    uint8    devType;
    uint8    devName[3];
}BLE_BROADCAST_T;

/*
*********************************************************************
* @brief     ：地址转蓝牙MAC
* @param[in] ：uint8 *pAddr              地址
               uint16 addrlen            地址(字节)长度
               BLE_MAC_E type            地址类型
* @param[out]：uint8 *pMAC               MAC(6个字节长度 大段)
* @return    ：0-成功/其他-错误码
*********************************************************************
*/
int ble_addr_to_mac(uint8 *pAddr, uint16 addrlen, BLE_MAC_E type, uint8 *pMAC);

/*
*********************************************************************
* @brief     ：蓝牙MAC转地址
* @param[in] ：uint8 *pMAC               MAC(6个字节长度 大段)
               uint16 addrlen            地址(字节)长度
* @param[out]：uint8 *pAddr              地址
* @return    ：地址类型 BLE_MAC_E
*********************************************************************
*/
uint8 ble_mac_to_addr(uint8 *pMAC, uint8 *pAddr, uint16 addrlen);

/*
*********************************************************************
* @brief     : 报文通知消息内容解析
* @param[in] : uint8 *pMsg               消息内容
               uint16 msglen             消息长度
* @param[out]: BLE_MSG_EVENT_T *pMsgOut  解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_msg_event_split(uint8 *pMsg, uint16 msglen, BLE_MSG_EVENT_T *pMsgOut);

/*
*********************************************************************
* @brief     : 报文通知消息内容封装
* @param[in] : BLE_MSG_EVENT_T *pMsgIn   通知内容
               uint16 buflen             缓存长度
* @param[out]: uint8 *pBuf               消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_msg_event_package(BLE_MSG_EVENT_T *pMsgIn, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 蓝牙端口通知消息内容解析
* @param[in] : uint8 *pMsg               消息内容
               uint16 msglen             消息长度
* @param[out]: BLE_PORT_EVENT_T *pPort   解析结果
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_port_event_split(uint8 *pMsg, uint16 msglen, BLE_PORT_EVENT_T *pPort);

/*
*********************************************************************
* @brief     : 蓝牙端口通知内容封装
* @param[in] : BLE_PORT_EVENT_T *pPort   通知内容
               uint16 buflen             缓存长度
* @param[out]: uint8 *pBuf               消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_port_event_package(BLE_PORT_EVENT_T *pPort, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 发送数据内容解析
* @param[in] : uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]: BLE_MSG_T *ptMsg            数据内容
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_msg_split(uint8 *pMsg, uint16 msglen, BLE_MSG_T *ptMsg);

/*
*********************************************************************
* @brief     : 发送数据内容封装
* @param[in] : BLE_MSG_T *ptMsg           数据内容
               uint16 buflen              缓存长度
* @param[out]: uint8 *pBuf                消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_msg_package(BLE_MSG_T *ptMsg, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 蓝牙转发内容解析
* @param[in] : uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]: BLE_TRANS_REQ_T *pTrans     转发内容
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_trans_req_split(uint8 *pMsg, uint16 msglen, BLE_TRANS_REQ_T *pTrans);

/*
*********************************************************************
* @brief     : 蓝牙转发内容封装
* @param[in] : BLE_TRANS_REQ_T *pTrans     转发内容
               uint16 buflen               缓存长度
* @param[out]: uint8 *pBuf                 消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_trans_req_package(BLE_TRANS_REQ_T *pTrans, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 蓝牙转发应答内容解析
* @param[in] : uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]: BLE_TRANS_ACK_T *pTrans     转发内容
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_trans_ack_split(uint8 *pMsg, uint16 msglen, BLE_TRANS_ACK_T *pTrans);

/*
*********************************************************************
* @brief     : 蓝牙转发应答内容封装
* @param[in] : BLE_TRANS_ACK_T *pTrans     转发内容
               uint16 buflen               缓存长度
* @param[out]: uint8 *pBuf                 消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_trans_ack_package(BLE_TRANS_ACK_T *pTrans, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 蓝牙参数配置内容解析
* @param[in] : uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]: BLE_CONFIG_T *pConfig       蓝牙参数
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_config_split(uint8 *pMsg, uint16 msglen, BLE_CONFIG_T *pConfig);

/*
*********************************************************************
* @brief     : 蓝牙参数配置内容封装
* @param[in] : BLE_CONFIG_T *pConfig       蓝牙参数
               uint16 buflen               缓存长度
* @param[out]: uint8 *pBuf                 消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_config_package(BLE_CONFIG_T *pConfig, uint8 *pBuf, uint16 buflen);

/**********************************************************************
* @name      : ble_sn_split
* @brief     ：蓝牙转发应答内容解析
* @param[in] ：uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]：BLE_CONFIG_T *pConfig       蓝牙参数
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_sn_split(uint8 *pMsg, uint16 msglen, BLE_SN_T *pSn);

/*
*********************************************************************
* @brief     : 蓝牙端口配置内容解析
* @param[in] : uint8 *pMsg                  消息内容
               uint16 msglen                消息长度
* @param[out]: BLE_PORTS_CONF_T *pPortConfig 端口配置
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_port_config_split(uint8 *pMsg, uint16 msglen, BLE_PORTS_CONF_T *pPortConfig);

/*
*********************************************************************
* @brief     : 蓝牙端口配置内容封装
* @param[in] : BLE_PORTS_CONF_T *pPortConfig 端口配置
               uint16 buflen                缓存长度
* @param[out]: uint8 *pBuf                  消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_port_config_package(BLE_PORTS_CONF_T *pPortConfig, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 蓝牙连接信息内容解析
* @param[in] : uint8 *pMsg                  消息内容
               uint16 msglen                消息长度
* @param[out]: BLE_PORT_CONF_T *pPortConfig 端口配置
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_connect_split(uint8 *pMsg, uint16 msglen, BLE_CONNECT_INFO_T *pConnect);

/*
*********************************************************************
* @brief     : 蓝牙连接信息内容封装
* @param[in] : BLE_CONNECT_INFO_T *pConnect 连接信息
               uint16 buflen                缓存长度
* @param[out]: uint8 *pBuf                  消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_connect_package(BLE_CONNECT_INFO_T *pConnect, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 蓝牙SN内容封装
* @param[in] : BLE_SN_T *pSN                sn
               uint16 buflen                缓存长度
* @param[out]: uint8 *pBuf                  消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_sn_package(BLE_SN_T *pSn, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 蓝牙检表内容解析
* @param[in] : uint8 *pMsg                  消息内容
               uint16 msglen                消息长度
* @param[out]: BLE_CHECK_T *pTcheck         检表配置
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_check_split(uint8 *pMsg, uint16 msglen, BLE_CHECK_T *pTcheck);

/*
*********************************************************************
* @brief     : 蓝牙检表内容封装
* @param[in] : BLE_CHECK_T *pTcheck        检表配置
               uint16 buflen               缓存长度
* @param[out]: uint8 *pBuf                 消息内容
* @return    : 消息长度
*********************************************************************
*/
uint16 ble_check_package(BLE_CHECK_T *pTcheck, uint8 *pBuf, uint16 buflen);

/*
*********************************************************************
* @brief     : 蓝牙通用请求内容封装(JSON)
* @param[in] : uint32 token              消息序号
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_common_req_package(uint32 token);

/*
*********************************************************************
* @brief     : 蓝牙通用应答内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: uint32 *status             状态
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_common_res_split(char *infopath, void *pJson, uint32 *status);

/*
*********************************************************************
* @brief     : 蓝牙通用应答内容封装(JSON)
* @param[in] : uint32 token              消息序号
               uint32 status             状态
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_common_res_package(uint32 token, uint32 status);

/*
*********************************************************************
* @brief     : 蓝牙参数配置请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_CONFIG_T *pConfig      蓝牙配置
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_setparam_req_split(char *infopath, void *pJson, BLE_CONFIG_T *pConfig);

/*
*********************************************************************
* @brief     : 蓝牙参数配置请求内容封装(JSON)
* @param[in] : uint32 token              消息序号
               BLE_CONFIG_T *pConfig     蓝牙参数
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_setparam_req_package(uint32 token, BLE_CONFIG_T *pConfig);

/*
*********************************************************************
* @brief     : 蓝牙参数配置查询响应内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_CONFIG_T *pConfig      蓝牙配置
               uint32 *status             状态
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_getparam_res_split(char *infopath, void *pJson, BLE_CONFIG_T *pConfig, uint32 *status);

/*
*********************************************************************
* @brief     : 蓝牙参数配置响应内容封装(JSON)
* @param[in] : uint32 token              消息序号
               BLE_CONFIG_T *pConfig     蓝牙参数
               uint32 status             状态
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_getparam_res_package(uint32 token, BLE_CONFIG_T *pConfig, uint32 status);

/*
*********************************************************************
* @brief     : 蓝牙端口配置请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_PORT_CONF_T *pPort     端口配置
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_setport_req_split(char *infopath, void *pJson, BLE_PORT_CONF_T *pPort);

/*
*********************************************************************
* @brief     : 蓝牙端口配置请求内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_PORT_CONF_T *pPort     端口配置
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_setport_req_package(uint32 token, BLE_PORT_CONF_T *pPort);

/*
*********************************************************************
* @brief     : 蓝牙端口通知内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_PORT_EVENT_T *pPort    端口通知
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_port_notify_split(char *infopath, void *pJson, BLE_PORT_EVENT_T *pPort);

/*
*********************************************************************
* @brief     : 蓝牙端口通知内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_PORT_EVENT_T *pPort    端口通知
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_port_notify_package(uint32 token, BLE_PORT_EVENT_T *pPort);

/*
*********************************************************************
* @brief     : 蓝牙检表配置请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_CHECK_T *pTcheck       检表配置
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_setdetect_req_split(char *infopath, void *pJson, BLE_CHECK_T *pTcheck);

/*
*********************************************************************
* @brief     : 蓝牙检表配置请求内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_CHECK_T *pTcheck       检表配置
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_setdetect_req_package(uint32 token, BLE_CHECK_T *pTcheck);

/*
*********************************************************************
* @brief     : 蓝牙参数配置查询响应内容解析(JSON)
* @param[in] : char *infopath               信息对象
               void *pJson                  JSON
* @param[out]: BLE_CONNECT_INFO_T *pConnect 连接信息
               uint32 *status               状态
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_getconnect_res_split(char *infopath, void *pJson, BLE_CONNECT_INFO_T *pConnect, uint32 *status);

/*
*********************************************************************
* @brief     : 蓝牙参数配置响应内容封装(JSON)
* @param[in] : uint32 token                 消息序号
               BLE_CONNECT_INFO_T *pConnect 连接信息
               uint32 status                状态
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_getconnect_res_package(uint32 token, BLE_CONNECT_INFO_T *pConnect, uint32 status);

/*
*********************************************************************
* @brief     : 发送数据请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_MSG_T *ptMsg           发送消息
               uint8 *prio                优先级
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_sendmsg_req_split(char *infopath, void *pJson, BLE_MSG_T *ptMsg, uint8 *prio);

/*
*********************************************************************
* @brief     : 发送数据请求内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_MSG_T *ptMsg           发送消息
               uint8 prio                 优先级
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_sendmsg_req_package(uint32 token, BLE_MSG_T *ptMsg, uint8 prio);

/*
*********************************************************************
* @brief     : 透传请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_TRANS_REQ_T *pTrans    转发消息
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_transmsg_req_split(char *infopath, void *pJson, BLE_TRANS_REQ_T *pTrans);

/*
*********************************************************************
* @brief     : 透传请求内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_TRANS_REQ_T *pTrans    转发消息
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_transmsg_req_package(uint32 token, BLE_TRANS_REQ_T *pTrans);

/*
*********************************************************************
* @brief     : 透传响应内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_TRANS_ACK_T *pAck      应答消息
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_transmsg_res_split(char *infopath, void *pJson, BLE_TRANS_ACK_T *pAck);

/*
*********************************************************************
* @brief     : 透传响应内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_TRANS_ACK_T *pAck      应答消息
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_transmsg_res_package(uint32 token, BLE_TRANS_ACK_T *pAck);

/*
*********************************************************************
* @brief     : 报文通知内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_MSG_EVENT_T *pMsgEvent 报文通知
* @return    : 0-成功/其他-错误码
*********************************************************************
*/
int ble_msg_notify_split(char *infopath, void *pJson, BLE_MSG_EVENT_T *pMsgEvent);

/*
*********************************************************************
* @brief     : 报文通知内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_MSG_EVENT_T *pMsgEvent 报文通知
* @param[out]: 
* @return    : JSON消息字符串
*********************************************************************
*/
char *ble_msg_notify_package(uint32 token, BLE_MSG_EVENT_T *pMsgEvent);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_H__ */
