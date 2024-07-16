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
#ifndef	__LC_MOD_H
#define __LC_MOD_H
    
#ifdef __cplusplus
        extern "C" {
#endif

/* G-功能模块; M-模拟量采集模块 X-功能类型无补充属性 
   见《能源控制器功能模组技术规范》1 模块分类和类型标识代码*/
#define MOD_LC_TYPE_PRFIX       "GRX"   

/*功能模块的设备描述符前缀 描述符=uart_acm[模组序号/槽位号]_[虚拟通道号]
  由HAL提供的LIB定义，参见framework/include/hal/uart.h*/
#define MOD_DEV_DESC_PRFIX        "uart_acm"

#define MOD_SAMPLE_INTERV  (5)  //模组信息读取间隔
#define MOD_SAMPLE_PERIOD  (60) //模组状态生成周期
#define MOD_SAMPLE_LIMITS ((MOD_SAMPLE_PERIOD/MOD_SAMPLE_INTERV)/2) //周期内模组状态阈值

#define LCM_DI_WORKSTATUS       0x07000001  //互感器工况信息
#define LCM_DI_CTVERSION        0x07000002  //互感器版本信息
#define LCM_DI_COREVERSION      0x07000003  //算法板版本信息
#define LCM_DI_FILETRANSMIT     0x0F000001  //升级用文件传输

/** @brief 模组回路状态枚举   */
typedef enum tag_LC_MOD_STATUS
{
    LC_MOD_STATUS_NR  = 0, //正常
    LC_MOD_STATUS_SC =  1, //短路
    LC_MOD_STATUS_OC  = 2, //开路
    LC_MOD_STATUS_SE =  6, //串接半导体    
    LC_MOD_STATUS_ED,
}LC_MOD_STATUS_E;

/** @brief 事件回路状态枚举   */
typedef enum tag_LC_EVT_STATUS
{
    LC_EVT_STATUS_NR  = 0, //正常
    LC_EVT_STATUS_SC =  4, //短路
    LC_EVT_STATUS_OC  = 6, //开路
    LC_EVT_STATUS_SE =  1, //串接半导体
}LC_EVT_STATUS_E;

/** @brief 规约回路状态枚举   */
typedef enum tag_LC_OOP_STATUS
{
    LC_OOP_STATUS_NR  = 0, //正常
    LC_OOP_STATUS_SC =  1, //短路
    LC_OOP_STATUS_OC  = 2, //开路
    LC_OOP_STATUS_SE =  3, //串接半导体
}LC_OOP_STATUS_E;

/*模组工况信息结构体*/
typedef struct taglcWorkStatus
{
    uint8  loopStatus[3];        //电流回路状态
    uint32 envTemp[3];           //环境温度
    uint32 freqMax[3];           //频率最大值
    uint32 freqMin[3];           //频率最小值
    uint32 curValue[3];          //工频电流有效值
    uint32 impeFreq1[3];         //第一组阻抗频率
    uint32 impeValue1[3];        //第一组阻抗模值
    uint32 impeAnge1[3];         //第一组阻抗角度
    uint32 impeFreq2[3];         //第二组阻抗频率
    uint32 impeValue2[3];        //第二组阻抗模值
    uint32 impeAnge2[3];         //第二组阻抗角度
    uint32 impeFreq3[3];         //第三组阻抗频率
    uint32 impeValue3[3];        //第三组阻抗模值
    uint32 impeAnge3[3];         //第三组阻抗角度                   
}LC_WORK_STATUS_T;

/*模组工况信息统计结构体*/
typedef struct taglcStatusStat
{
    uint32 count[3]; //3相计数
    LC_WORK_STATUS_T status;
}LC_STATUS_STAT_T;

/** @brief 回路巡检读取状态   */
typedef enum tag_LC_MONITOR_COL
{
    LCM_INIT  = 0, //初始态
    LCM_SEND =  1, //发送态
    LCM_OPEN  = 2, //打开态
    LCM_RECV =  3, //接收态
    LCM_FUPG  = 4, //升级态
    LCM_TRAN  = 5, //透传态
    LCM_GETM  = 6, //获取态    
    LCM_IDLE  = 7, //空闲态
}LC_MONITOR_COL_E;

#define LCM_SEND_RETRY (0)
#define LCM_RECV_RETRY ((MOD_SAMPLE_PERIOD/MOD_SAMPLE_INTERV)*(60.0/MOD_SAMPLE_PERIOD))

#define VF_BEGIN       0x68  //帧起始字符-
#define VF_END         0x16  //帧结束字符-

#define DL645_ADDR                 1                //地址域
#define DL645_CTRL                 8                //控制码
#define DL645_LEGH                 9                //长度域
#define DL645_DATA                 10               //数据域
    
#define DL645_AFN_CTEXT            0x03     //扩展功能码：互感器数据
#define DL645_FRAME_MIN      12   //报文最小长度（68(1)+addr(6)+68(1)+C(1)+L(1)+cs(1)+16(1)）
#define DL645_DATA_MAX       256  //数据域最大长度

typedef int (*MODGET)(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);

typedef struct tag_LC_MODPARSE
{
    uint32 di;
    MODGET apduGet;
}LC_MODPARSE_T;

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

#define DL645_ADDR_LEN       6    //地址域长度

typedef union DL645_CMD
{
    struct
    {
        unsigned char   afn:5;  //功能码   
        unsigned char   next:1; //后续帧标记 0-无后续帧 1-有后续帧
        unsigned char   res:1;  //从站应答类型 0-从站正确应答 1-从站对异常信息的应答
        unsigned char   dir:1;  //传输方向 0-由主站发出的命令帧 1-由从站发出的应答帧
    };
    unsigned char value;
} DL645_CMD_U; //-控制码字节-

//帧信息
typedef struct
{
    uint8 *pData;     //数据域    
    uint8 lengh;      //长度域
    DL645_CMD_U ctrl; //控制码
    uint8 addr[DL645_ADDR_LEN];    //地址域
}FRAME_INFO_T;

void lc_monitor_collect(void *pPublic, void *pPrivate);
void lc_monitor_deal(void *pPublic, void *pPrivate);
void lc_workstate_analyze(DB_CLIENT clientid, int sockfd);
int lc_version_get(void *pDev, DB_CLIENT clientid, int socketfd);
int lc_workstate_get(void *pDev, DB_CLIENT clientid, int socketfd);
int lcm_version_get(void *pDev, DB_CLIENT clientid, int socketfd);
int lcm_workstate_get(void *pDev, DB_CLIENT clientid, int socketfd);
void lc_monitor_read(void *pDev, uint16 dt, DB_CLIENT clientid, int socketfd);
int uart_open(LC_UART_VDEV_T *pUart);

int lc_get_map_slot(int sockfd, const char *type);

void lc_mod_restart_send(uint32 slot, uint32 time);
void lc_uart_mode_check(void *pPrivate);

int lc_ctworkstate_get(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lc_versioninfo_get(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lcm_ctworkstate_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lcm_ctversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lcm_coreversion_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);
int lcm_filetrans_parse(DB_CLIENT clientid, uint8 *apdu, uint32 apduLen, void *stdata);

#ifdef __cplusplus
}
#endif

#endif


