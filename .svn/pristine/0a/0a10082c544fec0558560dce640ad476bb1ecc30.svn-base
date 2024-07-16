/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程主动上报
* @date：    2019-12-5
* @history： 
*********************************************************************
*/
#ifndef	__LCMTRANS_H
#define __LCMTRANS_H


#ifdef __cplusplus
        extern "C" {
#endif

#define VF_BEGIN       0x68  //帧起始字符-
#define VF_END         0x16  //帧结束字符-

typedef enum
{
    UART_PARITY_NONE     = 0,
    UART_PARITY_ODD,       //奇校验
    UART_PARITY_EVEN,      //偶校验
    UART_PARITY_SPACE,    
}UART_PARITY_E;

/*串口参数*/
typedef struct tag_LC_UART_PARAM_T
{
    uint32        baudRate;      /** < 波特率 */ 
    uint8         dataBits;      /** < 数据位 */
    uint8         stopBits;      /** < 停止位 */
    uint8         parity;        /** < 校验位 */    
    uint8         flowCtrl;      /** < 流控位 */    
}LC_UART_PARAM_T;

/*串口操作句柄*/
typedef struct tag_LC_UART_VDEV_T
{
    char             halname[32];   /** < HAL设备描述符   */    
    char            *desc;          /** < 真实设备描述符   */    
    LC_UART_PARAM_T  param;         /** < 串口参数   */
    uint16           frametimeout;  /** < 帧超时   */
    uint16           bytetimeout;   /** < 字节超时   */    
    int              fd;            /** < 串口句柄(要初始化为-1，串口打开后生成) */
    uint32           sendCnt;       /** < 串口发送计数 */
    uint32           recvCnt;       /** < 串口接收计数 */ 
    struct timespec  sendTm;        /** < 串口发送时刻 */
    struct timespec  recvTm;        /** < 串口接收时刻 */  
}LC_UART_VDEV_T;

typedef struct tagLcmTransThread
{
    uint8        *pRunFlag;
    uint8         status;    //状态
    DB_CLIENT     clientid;
    time_t        curtime;       //当前时间    
    time_t        lsttime;       //上次时间
}LCMTRANS_THREAD_T;

/** @brief 回路巡检读取状态   */
typedef enum tag_LC_MONITOR_COL
{
    LCM_INIT  =  0, //初始态    
    LCM_ERRO  =  1, //错误态
    LCM_CONF  =  2, //配置态    
    LCM_TRAN  =  3, //透传态
}LC_MONITOR_COL_E;

typedef int (*MODGET)(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);

typedef struct tag_LC_MODPARSE
{
    uint32 di;
    MODGET apduGet;
}LC_MODPARSE_T;

#define LCM_SEND_RETRY (0)

#define LCM_OAD_TAVERSION        0x40400200  //TA模块版本信息
#define LCM_OAD_WORKSTATUS       0x40400300  //TA模块工况信息

#define LCM_QUEUE_DEPTH (10)
#define LCM_QUEUE_BUFER (2048)

/* 队列内容*/
typedef struct tag_LCMTRANS_QUEUE_T
{
    uint8   type;                         /** < 队列类型       */
    uint16  length;                       /** < 数据长度       */
    uint8   data[LCM_QUEUE_BUFER];       /** < 数据缓冲区 */
}LCMTRANS_QUEUE_T; 

/* 先进先出队列 */
typedef struct tag_LCMTRANS_FIFO_T
{
    pthread_mutex_t  lock;                    /*接收缓冲区互斥锁*/
    uint32           depth;                   /** < 队列深度*/
    uint16           head;
    uint16           tail;
    LCMTRANS_QUEUE_T queue[LCM_QUEUE_DEPTH]; /** < 队列数据     */
}LCMTRANS_FIFO_T;

typedef struct tagLcmEsamFrameHead
{
    uint32          msglen;  //长度
    uint32          apduLen; //apud长度
    uint8          *pApdu;
}LCM_ESAMFRAME_HEAD_T;

int uart_open(LC_UART_VDEV_T *pUart);
int uart_close(LC_UART_VDEV_T *pUart);
int uart_write(LC_UART_VDEV_T *pUart, uint8* buf, int len);
int uart_read(LC_UART_VDEV_T *pUart, uint8* buf, int len);

void *app_lcm_trans_work(void *pArg);
void *app_ljm_trans_work(void *pArg);

#ifdef __cplusplus
}
#endif

#endif


