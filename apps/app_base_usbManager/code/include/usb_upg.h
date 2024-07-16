/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控业务
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef __USB_UPG_H
#define __USB_UPG_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define USB_UPDATE_FILE "/data/app/usbManager/copr_update_file"

#define USB_UPPACK_SIZE  (196) //互感器最大只能传输196字节 

/** @brief 文件分块传输状态 */
typedef struct
{
    uint16 nBlockNum;       //总块数
    uint8  packStatus[64];   //包传输状态
} USB_FILE_STATUS_T;

/** @brief 文件分块传输信息 */
typedef struct
{
    uint8  fileMark;   //文件标识    
    uint32 nBlockNum;  //总块数
    uint16 nEndLengh;  //最后一次长度
    uint32 nBlockId;   //当前块序号
    uint16 nLengh;     //当前块长度
    uint8 *fileData;   //当前块内容指针
} USB_FILE_TRANS_T;

typedef enum
{
    USB_UPDATE_IDLE = 0,
    USB_UPDATE_RUNING = 1,
    USB_UPDATE_MAINTAIN = 2,
}USB_UPDATE_STATUS_E;

typedef enum
{
    UART_PARITY_NONE     = 0,
    UART_PARITY_ODD,       //奇校验
    UART_PARITY_EVEN,      //偶校验
    UART_PARITY_SPACE,    
}USB_UART_PARITY_E;

/*串口参数*/
typedef struct tag_USB_UART_PARAM_T
{
    uint32        baudRate;      /** < 波特率 */ 
    uint8         dataBits;      /** < 数据位 */
    uint8         stopBits;      /** < 停止位 */
    uint8         parity;        /** < 校验位 */    
    uint8         flowCtrl;      /** < 流控位 */    
}USB_UART_PARAM_T;

/*串口操作句柄*/
typedef struct tag_USB_UART_VDEV_T
{
    char            *desc;          /** < 设备描述符   */    
    USB_UART_PARAM_T param;         /** < 串口参数   */
    uint16           frametimeout;  /** < 帧超时   */
    uint16           bytetimeout;   /** < 字节超时   */    
    int              fd;            /** < 串口句柄(要初始化为-1，串口打开后生成) */
    uint32           sendCnt;       /** < 串口发送计数 */
    uint32           recvCnt;       /** < 串口接收计数 */ 
    struct timespec  sendTm;        /** < 串口发送时刻 */
    struct timespec  recvTm;        /** < 串口接收时刻 */
}USB_UART_VDEV_T;

/* 回路巡检升级公共数据           */
typedef struct tagUsbUpdateData
{
    USB_UPDATE_STATUS_E status;     //升级状态
    USB_FILE_TRANS_T   *fileInfo;   //文件传输信息
    int                fd;         //文件句柄
    USB_UART_VDEV_T    *pUart;      //串口句柄
}USB_UPDATE_DATA_T;

#ifdef __cplusplus
}
#endif

#endif


