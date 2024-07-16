/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP 国网私有协议解析
* @date：    2021-8-13
* @history： 
*********************************************************************
*/

#ifndef __BLE_PROTOCOL_H__
#define __BLE_PROTOCOL_H__


#ifdef __cplusplus
extern "C" {
#endif

/* 蓝牙私有协议最小长度 */
#define BLE_PROTOCOL_LEN_MIN      21
#define BLE_PROTOCOL_DATA_START   18

typedef enum _BLE_Pro_TYPE
{
    BLE_PRO_TO_OUT  = 1,              /* 透传 */
    BLE_PRO_SET_BT  = 2,              /* 配置蓝牙模块参数 */
    BLE_PRO_GET_BT  = 3,              /* 读取蓝牙模块参数 */
    BLE_PRO_REPORT  = 4,              /* 蓝牙主动上报参数 */
}BLE_PROTOCOL_E;

/* 蓝牙控制命令 */
/* 0. 透传 */
#define CMD_DEV_TRANS_OUT        0x00000000

/* 1. 设置或读取各设备配对参数 */
#define CMD_DEV_PAIR_MODE        0xF20B0000

/* 2. 设置或读取各设备地址 */
#define CMD_DEV_MAC_ADDR         0xF20B0001

/* 3. 设置或读取本设备蓝牙工作参数 */
#define CMD_BLE_PARAMS           0xF20B0002

/* 4. 读取或主动上报蓝牙连接信息 */
#define CMD_BLE_CONN_INFO        0xF20B0003

/* 5. 读取或主动上报蓝牙连接信息 */
#define CMD_BLE_TRANS_MODE       0x00000006

/* 6. 设置或主动上报设备状态信息 */
#define CMD_MODULE_INFO          0x00000007
typedef enum _BLE_MODEL_INFO_TYPE
{
    BLE_MODEL_RESTART  = 1,              /* 模块复位 */
    BLE_MODEL_RESTORE  = 2,              /* 模块恢复出厂设置 */
    BLE_MODEL_SLEEP    = 3,              /* 模块进入睡眠状态 */
    BLE_MODEL_WAKE     = 4,              /* 模块退出睡眠状态 */
}BLE_MODEL_INFO_E;

/* 7. 读取设备固件版本信息 */
#define CMD_FIRMWARE_VERSION     0x00000008
typedef struct tagCmdVersion
{
    uint8  ic_type;
    uint8  app_version[5];
    uint8  git_version[4];
}__attribute__ ((packed)) CMD_VERSION_T;

/* 8. 设置设备扫描功能 */
#define CMD_SCAN                 0x00000009

/* 9. 设置广播过滤的设备地址参数 */
#define CMD_FILTER_ADDR          0x0000000A

/* 10.设置广播过滤的设备类型参数 */
#define CMD_FILTER_TYPE          0x0000000B

/* 11.读取广播过滤信息 */
#define CMD_GET_FILTER_INFO      0x0000000C

/* 12.设置或读取 UART 参数 */
#define CMD_UART_PARAMS          0x0000000D
typedef struct tagCmdMsgUart
{
    uint32 baud_rate;
    uint8  data_width;
    uint8  parity;
    uint8  stop_bit;
}__attribute__ ((packed)) CMD_MSG_UART_T;

/* 13.设置广播数据 */
#define CMD_ADV_DATA             0x0000000E

/* 14.设置扫描响应数据 */
#define CMD_RESP_DATA            0x0000000F

/* 15.设置广播过滤上报功能 */
#define CMD_ADV_INFO_REPORT      0x00000010

/* 16.清除指定设备的配对信息 */
#define CMD_CLEAR_BOND_INFO      0x00000011

/* 17.清除所有设备的配对信息 */
#define CMD_CLEAR_ALL_BOND_INFO  0x00000012

/* 18.切换进入/退出检定模式 */
#define CMD_PD                   0xF20B0201

/* 19.设置或读取设备名 */
#define CMD_DEV_NAME             0x00000013

/* 20.设置或读取发射功率 */
#define CMD_TX_POWER             0x00000014

/* 21.设置或读取广播间隔 */
#define CMD_ADV_INTERVAL         0x00000015

/* 22.设置或读取指定设备地址 */
#define CMD_DEV_ADDR             0x00000016

/* 23.设置或读取指定设备配对模式 */
#define CMD_DEV_PAIR_MODE_A      0x00000017

/* 24.设置或读取指定设备配对参数 */
#define CMD_DEV_PAIR_VALUE       0x00000018

/* 25.连接或断开指定设备 */
#define CMD_LINK_CMD             0x00000019

/* 26.连接或断开指定设备 */
#define CMD_ADV                  0x0000001A

/* 27.连接或断开指定设备 */
#define CMD_FIRMWARE_BUILD_TIME  0x0000001B

/* 28.读取对端设备链路RSSI */
#define CMD_LINK_RSSI            0x0000001C

/* 29.读取模组SN号 */
#define CMD_MODULE_SN            0x0000001D



/* 蓝牙私有协议内容 */
typedef struct tagBLEProtocolData
{
    uint8       type;      //报文类型
    uint8       res[1];    //BIT5-BIT3保留
    uint8       bStatus;   //状态报文标识
    uint8       bAck;      //应答报文标识

    uint32      cmd;       //控制命令
    uint8       reserve[4];//预留
    uint8       mac[6];    //传输设备地址
    uint16      datalen;   //数据域长度
    uint8       data[0];   //数据域
}BLE_PRODATA_T;

int ble_protocol_msg_ckeck(uint8 *msg, uint16 msglen);
int ble_protocol_split(uint8 *msg, uint16 len, BLE_PRODATA_T **ppProData);
uint16 ble_protocol_package(uint8 frametype, uint32 cmd, uint8 *mac, uint8 *pData, uint16 datalen, uint8 **ppFrame);

int ble_set_operate(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, BLE_PRODATA_T **pProRet,
                       uint8 *recvbuf, uint16 buflen);
int ble_get_operate(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, BLE_PRODATA_T **pProRet, 
                       uint8 *recvbuf, uint16 buflen);
int ble_tran_send(int fd, uint8 *MAC, uint8 *pSend, uint16 sendlen);
int ble_report_response(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen);
int ble_dev_report_conn_proc(int fd, BLE_PRODATA_T *pProData, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata);

#ifdef __cplusplus
}
#endif

#endif //__BLE_PROTOCOL_H__

