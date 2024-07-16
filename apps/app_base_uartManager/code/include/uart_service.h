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
#ifndef	__UART_SERVICE_H
#define __UART_SERVICE_H
    
#ifdef __cplusplus
        extern "C" {
#endif

//串口
//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define UART_FMT_TRACE(log,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 50, log, 0, NULL, 0, fmt, ##__VA_ARGS__)
#define UART_FMT_DEBUG(log,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 50, log, 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define UART_BUF_TRACE(log,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 50, log, 0, buf, len, fmt, ##__VA_ARGS__)
#define UART_BUF_DEBUG(log,buf,len,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 50, log, 1, buf, len, fmt, ##__VA_ARGS__)


#define  UART_MONITOR_SEEK_PORT   0
#define  UART_MONITOR_GET_INFO    1
#define  UART_MONITOR_WORK        2

/* G-功能模块; M-模拟量采集模块 X-功能类型无补充属性 
   见《能源控制器功能模组技术规范》1 模块分类和类型标识代码*/
#define MOD_UART_TYPE_PRFIX       "GBR"  

/*功能模块的设备描述符前缀 描述符=uart_acm[模组序号/槽位号]_[虚拟通道号]
  由HAL提供的LIB定义，参见driver/product/xxxx_hal/uart.h*/
#define MOD_DEV_DESC_PRFIX        "uart_acm"

#define UART_QUEUE_DEPTH (10)
#define UART_QUEUE_BUFER (4096)
#define APP_LIST_MAX     (20)
#define APP_NAME_MAX     (64)

///*校验位*/
//typedef enum tag_UART_PARITY_E  
//{
//    UART_PARITY_NO = 0,
//    UART_PARIGY_ODD,
//    UART_PARITY_EVEN,
//    UART_PARITY_SPACE,    
//}UART_PARITY_E;

/*串口功能*/
typedef enum tag_UART_FUNC_E  
{
    UART_UPSTREAM = 0,
    UART_DWSTREAM = 1,
}UART_FUNC_E;

/*配置选择*/
typedef enum tag_UART_CONFCHOICE_E  
{
    UART_FUNCODE = 0,
    UART_PARAM   = 1,
}UART_CONFCHOICE_E;

/*串口参数*/
typedef struct tag_UART_PARAM_T
{
    uint8             baudRate;   //波特率 UART_BAUDRATE_E
    uint8             dataBits;   //数据位: 5、6、7、8
    uint8             parity;     //校验位参数 UART_PARITY_E
    uint8             stopBits;   //停止位参数：1、2
    uint8             flowCtrl;   //流控制 UART_FLOWCTRL_E
}UART_PARAM_T;

typedef enum 
{
    UART_STATUS_OK       = 0,  //成功
    UART_STATUS_TIME_OUT,      //超时
    UART_STATUS_TASK_FULL,     //队列满, APP需要重发
    UART_STATUS_PARAM_ERR,     //参数错误
    UART_STATUS_DATA_ERR,      //数据错误 不符合Base64编码格式
    UART_STATUS_DATA_OTHER = 255,
}UART_STATUS_E;

typedef enum 
{
    COM_UART  = 0,
    COM_CAN   = 1, 
    COM_MAX,
}UART_COMTYPE_E;

typedef enum
{
    MODTYPE_485 = 0,
    MODTYPE_CAN = 1,
}UART_MODTYPE_E;

typedef enum
{
    MOD_STATUS_INSERT =  0, //插入
    MOD_STATUS_EXTRACT = 1, //拔出
}UART_MODSTATUE_E;

typedef struct tag_UART_MODINFO_T
{
    uint8 type;           //模块类型 @ref UART_MODTYPE_E
    uint8 status;         //模块状态 @ref UART_MODSTATUE_E    
    uint8 change[16];     //模块变更 是 1 否 0
    uint8 slot;           //模组槽位号
    uint8 cfgnum;         //配置数量
    uint8 cfgfunc[16];    //配置功能
    char  drvName[16][64];//驱动名称
}UART_MODINFO_T;

/*串口操作句柄*/
typedef struct tag_UART_DEVDES_T
{
    char            *comdesc;       /** < 端口名称     */
    char             halname[64];   /** < 设备名称 */
    char             devname[64];   /** < 设备名称 */
    uint8            comType;       /** < 串口类型 */
    UART_PARAM_T     param;         /** < 串口参数 */
    int              fd;            /** < 串口描述符 */
    UART_STATUS_E    status;        /** < 串口状态 */
    uint32           sendCnt;       /** < 串口发送计数 */
    uint32           recvCnt;       /** < 串口接收计数 */
    struct timespec  sendTm;        /** < 串口发送时刻 */
    struct timespec  recvTm;        /** < 串口接收时刻 */
    BOOL             isUpdate;      /** < 串口参数更新 */
    uint8            retry;         /** < 重试次数 */
    int              retval;        /** < 串口操作返回码 */
}UART_DEVDES_T;

/*串口传输*/
typedef struct tag_UART_TRANSMIT_T
{
    uint8           *sendBuf;
    uint16           sendLen;
    uint8           *recvBuf;
    uint16           recvMax;
    uint16           frameTimeout;   /** < 帧超时(毫秒)   */
    uint16           octetTimeout;   /** < 字节超时   (毫秒)*/    
}UART_TRANSMIT_T;

/* 队列内容*/
typedef struct tag_UART_QUEUE_T
{
    uint32  portid;                       /** < 端口号       */
    char    portname[32];                 /** < 端口名       */
    uint8   prio;                         /** < 优先级       */
    uint16  length;                       /** < 数据长度       */
    uint8   data[UART_QUEUE_BUFER];       /** < 数据缓冲区 */
}UART_QUEUE_T; 

/* 优先队列 */
typedef struct tag_UART_PQUE_T
{
    pthread_mutex_t lock;                  /*接收缓冲区互斥锁*/
    uint32          depth;                   /** < 队列深度*/
    uint32          size;                    /** < 队列大小     */
    UART_QUEUE_T    queue[UART_QUEUE_DEPTH]; /** < 队列数据     */
}UART_PQUE_T;

/* 先进先出队列 */
typedef struct tag_UART_FIFO_T
{
    pthread_mutex_t lock;                    /*接收缓冲区互斥锁*/
    uint32          depth;                   /** < 队列深度*/
    uint16          head;
    uint16          tail;
    UART_QUEUE_T    queue[UART_QUEUE_DEPTH]; /** < 队列数据     */
}UART_FIFO_T;

/* 串口服务状态 */
typedef enum tag_UART_WORKSTATE_E
{
    UART_INIT = 0,  //初始状态
    UART_PARA = 1,  //参数状态 
    UART_CONF = 2,  //配置状态
    UART_WORK = 3,  //工作状态
    UART_GETM = 4,  //查询状态
    UART_IDLE = 5,  //空闲状态
}UART_WORKSTATE_E;

/* 串口服务表 */
typedef struct tag_UART_INFO_T
{
    char         *comname;                              /** < 端口名称     */
    uint32        port;                                 /** < 设备端口号*/
    char         *name;                                 /** < 设备端口名     */
    BOOLEAN       aliasEn;                              /** < 使能别名(软件无法感知，暂时不启用该字段)     */
    uint32        portAlias;                            /** < 设备端口号别名*/
    char         *nameAlias;                            /** < 设备端口名别名     */
    UART_DEVDES_T uart;                                 /** < 操作句柄     */
    UART_MODE_E   mode;                                 /** < 工作模式     */
    char          onlyApp[APP_NAME_MAX];                /** < 独占APP名,独占模式下有效   */ 
    uint32        listNum;                              /** < 监听列表数量     */
    char          appList[APP_LIST_MAX][APP_NAME_MAX];  /** < 监听APP列表,从模式下有效   */    
    UART_PQUE_T   pque;                                 /** < 优先队列     */
    UART_FIFO_T   fifo;                                 /** < FIFO队列   */
    UART_WORKSTATE_E  workstate;                        /** < 串口工作状态   */
}UART_INFO_T;

/* 串口服务线程数据 */
typedef struct tag_UART_SERVICE_T
{
    uint8          status;    //状态
    uint8          res[2];
    uint8          port;
    DB_CLIENT      clientid;
    int            tasksocket;
    time_t         tasktimeout; //任务超时时间
    //uint8          recvBuf[TASK_MSG_MAX_LEN];
    UART_INFO_T   *pUartInfo;
}UART_SERVICE_T;

/* 入口函数申明 */
typedef int (*UART_AXDRMSG_FUN)(MESSAGE_INFO_T*, UART_APPDATA_T*, UART_SERVICE_T*);

typedef struct tag_UART_AXDR_FUN_T
{
    uint32               IOP;      /** < 标识 */
    UART_AXDRMSG_FUN     pProcMsg; /** < 处理函数 */
}UART_AXDR_FUN_T;

/* 入口函数申明 */
typedef int (*UART_JSONMSG_FUN)(MESSAGE_JSON_T*, UART_APPDATA_T*, UART_SERVICE_T*);

typedef struct tag_UART_JSON_FUN_T
{
    char               *infopath; /** < 标识 */
    UART_JSONMSG_FUN    pProcMsg; /** < 处理函数 */
}UART_JSON_FUN_T;

#define UART_DATABITS_GET(c) (((c)&0x03)>>0) //数据位
#define UART_PARIBITS_GET(c) (((c)&0x0c)>>2) //校验位
#define UART_STOPBITS_GET(c) (((c)&0x10)>>4) //停止位
#define UART_FLOWCTRL_GET(c) (((c)&0x60)>>5) //流控制

#define UART_CTRLBITS_SET(d,p,s,f) ((((f)<<5)&0x60)|(((s)<<4)&0x10)|(((p)<<2)&0x0c)|((d)&0x03))

#define UART_BAUDRATE_SET(baudrate)\
int _j = 0; \
do{\
    for(_j = 0; _j < sizeof(g_uart_baud)/sizeof(uint32); _j++)\
    {\
        if((baudrate) == g_uart_baud[_j])\
        {\
            break;\
        }\
    }\
}while(0), _j


int uart_pque_init(UART_PQUE_T *pq);
int uart_pque_push(UART_PQUE_T *pq, UART_QUEUE_T *inData);
int uart_pque_pop(UART_PQUE_T *pq, UART_QUEUE_T *outData);
int uart_fifo_init(UART_FIFO_T *fifo);
int uart_fifo_push(UART_FIFO_T *fifo, UART_QUEUE_T *inData);
int uart_fifo_pop(UART_FIFO_T *fifo, UART_QUEUE_T *outData);

int uart_portlist_get(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic);
int uart_jsonmsg_respond(MESSAGE_JSON_T *pInMsg, MESSAGE_JSON_T *ptJson, char *port, char *status, char *data);
int uart_mode_change_notify(UART_MODEINFO_T *modeInfo, UART_APPDATA_T* ptAppData, UART_SERVICE_T *ptSevData);
int uart_axdr_info_get(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle);
int uart_axdr_param_set(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle);
int uart_axdr_app_reg(MESSAGE_INFO_T *pInMsg, UART_APPDATA_T* ptAppData, UART_SERVICE_T *pHandle);

#ifdef __cplusplus
}
#endif

#endif /* __UART_SERVICE_H */


