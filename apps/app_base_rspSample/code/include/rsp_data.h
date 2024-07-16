/*
*********************************************************************
* Copyright(C) 2022 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2022-3-20
*********************************************************************
*/
#ifndef __RSP_DATA_H__
#define __RSP_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RSP_APP_NAME                    "M-rspSample"

/* 最大 */
#define RSP_PULSE_STAT_NUM              3
#define RSP_PULSE_TIME_MIN              60
#define RSP_PULSE_TIME_MAX              100

/* 最大 */
#define RSP_PORT_MAX                    16
/* 遥信配置 */
#define RSP_CONFIG_YX                   0
/* 脉冲配置 */
#define RSP_CONFIG_PU                   1

#define RSP_STATUS_GET_DEV              0
#define RSP_STATUS_INIT                 1
#define RSP_STATUS_WORK                 2

#define RSP_MAP_LINK                    0
#define RSP_MAP_INFO                    1
#define RSP_MAP_CHECK                   2
#define RSP_MAP_FILE                    3

#define RSP_PULSE_IO                    0
#define RSP_PULSE_SYS                   1
#define RSP_PULSE_UART                  0xFF


typedef struct tagRspReportDATA
{
    uint8           id;
    uint8           bReport;
    uint8           sum;                      //端口总数量
    uint8           config[RSP_PORT_MAX];     //配置
    uint8           ST[RSP_PORT_MAX];         //状态
    uint8           CT[RSP_PORT_MAX];         //变位
    OOP_DATETIME_T  chgTime[RSP_PORT_MAX];    //变位时间(北京时间)
}RSP_REPORT_DATA_T;

#define RSP_REPORT_CACHE                20
typedef struct tagRspReportList
{
    uint8             head;
    uint8             tail;
    RSP_REPORT_DATA_T list[RSP_REPORT_CACHE]; //变位时间(北京时间)
}RSP_REPORT_LIST_T;

#define RSP_MSG_MAXLEN                 512
typedef struct tagRspMsgDATA
{
    uint32          len;
    uint8           msg[RSP_MSG_MAXLEN];     //变位时间(北京时间)
}RSP_MSG_DATA_T;

#define RSP_MSG_CACHE                  20
typedef struct tagRspMSGList
{
    uint8             head;
    uint8             tail;
    RSP_MSG_DATA_T    list[RSP_MSG_CACHE]; //变位时间(北京时间)
}RSP_MSG_LIST_T;

typedef struct tagRspPulseTime
{
    uint8          bStart;
    uint8          point[3];  //统计3个点
    uint32         start_s;
    uint16         start_ms;
    uint8          bH;
    uint8          bL;
    
    uint32         H_time_s;
    uint16         H_time_ms;
    uint32         L_time_s;
    uint16         L_time_ms;
}RPS_PULSE_TIME_T;


#define RSP_BUF_LENGTH           4096

#define RSP_PRINT_TIME(pTi) (pTi)->year, (pTi)->month, (pTi)->mday, (pTi)->hour, (pTi)->minute, (pTi)->second, (pTi)->msec

typedef struct tagRspRmtData
{
    uint8          stateNum; //遥信状态bit-str个数
    uint8          ST[RSP_PORT_MAX];
    uint8          changeNum; //遥信状态bit-str个数
    uint8          CT[RSP_PORT_MAX];
    uint8          timeNum;
    uint8          validFlag[RSP_PORT_MAX]; //date_time有效标志
    OOP_DATETIME_T changeTime[RSP_PORT_MAX];

    uint8          bReport; //是否主动上报
}RSP_RMT_D_T;

typedef struct tagRspPulseData
{
    uint32         num;                      //数量
    uint8          validFlag[RSP_PORT_MAX]; //date_time有效标志
    uint32         nCount[RSP_PORT_MAX];    //脉冲个数
    uint32         nTimeMS[RSP_PORT_MAX];   //统计时间 ms 
}RSP_PULSE_D_T;

typedef struct tagRspPulseTotal
{
    uint32         num;                      //数量
    uint8          validFlag[RSP_PORT_MAX]; //date_time有效标志
    uint32         nCount[RSP_PORT_MAX];    //脉冲个数
    uint32         nTimeH[RSP_PORT_MAX];    //统计时间 s
    uint16         nTimeL[RSP_PORT_MAX];    //统计时间 ms
    OOP_DATETIME_T startTime[RSP_PORT_MAX]; //启动时间
}RSP_PULSE_A_T;

typedef struct tagRspMapInfo
{
    char           modeltype[128];    //设备型号
    char           ID[128];           //模组设备ID
    
    char           manuNum[4 + 1];    /* 厂商代码 */
    char           softversion[4 + 1];/* 软件版本号 */
    char           softdata[6 + 1];   /* 软件版本日期 */
    char           hwversion[4 + 1];  /* 硬件版本号 */
    char           hwdata[6 + 1];     /* 硬件版本日期 */
    char           manuExten[8 + 1];  /* 厂商扩展信息 */
}RSP_MAP_INFO_T;

#define RSP_CHANNEL_MAX         8
typedef struct tagRspCannel
{
	uint8          type;
	uint8          func;
}RSP_CHANNEL_T;

typedef struct tagRspLinkInfo
{
    char           modeltype[128];    //设备型号
    char           ID[128];           //模组设备ID
    uint16         version;           //协议版本号
    uint16         maxSend;           //最大发送
    uint16         maxrecv;           //最大接收
    uint8          size;              //并发窗口
    uint8          num;               //虚拟通道
    
    RSP_CHANNEL_T  channel[RSP_CHANNEL_MAX];
}RSP_LINK_INFO_T;

typedef struct tagRmtPulseDev
{
    //配置
    uint8           bShare;                   //遥信脉冲端口是否共享
    uint8           doorStatus;               //0-分；1-合
    uint8           sum;                      //遥信端口总数量
    uint8           yksum;                    //遥控数量
    uint8           pulsesum;                 //脉冲端口总数量
    uint8           pulseLocate;              //IO采集方式, 脉冲口位置（用bit位表示）
    uint16          shakeTime;                //防抖时间, 单位ms
    uint32          ykLocate;                 //遥控位置
    
    uint8           config[RSP_PORT_MAX];     //配置: 0-遥信, 1-脉冲

    pthread_mutex_t flock;                    //数据访问锁

    //遥信
    uint32          bFirstRun;                //获取到首次遥信状态
    uint8           storeState[RSP_PORT_MAX]; //有效存储状态
    OOP_DATETIME_T  chgTime[RSP_PORT_MAX];    //变位时间(北京时间)
    uint8           chgState[RSP_PORT_MAX];   //有效存储状态
    
    uint8           lastState[RSP_PORT_MAX];    //上个统计周期状态
    struct timespec firstChgTime[RSP_PORT_MAX]; //首次状态变化时间(系统运行时间)
    int             timeSpace[RSP_PORT_MAX];    //持续变化时间 us

    RSP_REPORT_DATA_T curNode;               //当前状态
    RSP_REPORT_DATA_T lastNode;              //上次状态
    RSP_REPORT_DATA_T oldNode;               //上上次状态

    //脉冲（脉宽 80±20 ms）
    OOP_DATETIME_T   gettime;                  //脉冲统计时间
    RSP_PULSE_A_T    tPulseAll;                //脉冲总数
    RPS_PULSE_TIME_T pulseStat[RSP_PORT_MAX]; //脉冲统计

    uint8           workstauts;
    uint8           index;
    uint8           configStatus;
    uint8           configindex;              //功能设置帧序号
    uint8           shakeStatus;
    uint8           shakeindex;               //遥信防抖帧序号

    uint8           get485Status;
    uint8           get485index;
    uint8           get485_can;
    uint8           set485Status;
    uint8           set485index;
    uint8           set485_can;

    uint8           bFirstGet;

    struct timeval  timeflag;                 //定时查询标记
    uint16          recvlen;                  //缓存长度
    uint8           recv[RSP_BUF_LENGTH];     //串口接收缓存

    /* 升级相关 */
    struct tm       lasttime;                 //过日检测
    uint16          maxData;                  //最大发送文件大小
    uint8           mapStatus;                //模组状态
    uint8           fileindex;                //文件传输当前序号
    uint8           bNeedSend;                //是否需要发送
    uint8           bSetTime;                 //对时标记
    
    uint16          maptimems;                //运行ms 判断应答消息1s超时
    time_t          mapTime;                  //定时查询标记
    uint16          fileNO;                   //段序号
    uint16          fileMAX;                  //段总序号
    int             filelen;                  //文件大小
    uint8          *fileData;                 //文件内容
    char            updatefile[128];          //文件名称
    
    RSP_MAP_INFO_T  mapInfo;                  //模组信息
    RSP_LINK_INFO_T linkInfo;                 //链路协商信息

    //控制
    uint8           switchnum;                //控制路数
    uint16          switchStatus;             //接入状态
}RSP_DEV_T;

typedef struct tagRmtPulseModel
{
    //配置
    uint8           no;                       //内部排序
    uint8           sum;                      //端口总数
    uint8           slot;                     //模组序号
    uint8           channel;                  //端口序号

    uint32          bFirstRun;                //获取到首次遥信状态

    uint8           bWorkAgain;               //重新开始执行
    uint8           bSet;                     //是否支持设置
    uint8           config[RSP_PORT_MAX];     //配置: 0-遥信, 1-脉冲
    uint16          shakeTime;                //防抖时间

    pthread_mutex_t flock;                    //数据访问锁
    //遥信
    uint8             storeState[RSP_PORT_MAX];//有效存储状态
    RSP_REPORT_DATA_T curNode;                //当前状态
    RSP_REPORT_DATA_T lastNode;               //上次状态
    RSP_REPORT_DATA_T oldNode;                //上上次状态
    
    //脉冲
    RSP_PULSE_A_T     tPulseAll;              //脉冲

    struct timeval  timeflag;                 //定时查询标记
    uint8           workstauts;
    uint8           index;

    uint8           configStatus;
    uint8           configindex;              //功能设置帧序号
    uint8           shakeStatus;
    uint8           shakeindex;               //遥信防抖帧序号

    uint16          recvlen;                  //缓存长度
    uint8           recv[RSP_BUF_LENGTH];     //串口接收缓存

    int             fd;                       //串口句柄
    void           *pUart;                    //串口HAL
}RSP_MODEL_T;

typedef struct tagRspDATA
{
    uint32            mqttip;       //mqtt地址
    uint16            mqttport;     //mqtt端口
    uint8             bIIC;         //是否IIC采集(终端本体)
    uint8             bMAP;         //是否支持模组管理器

    int               ymfd;         //遥信设备
    int               uartfd;       //串口设备
    RSP_DEV_T         tRspDev;      //状态检查(终端本体)

    uint8                   bMapReg;               //模组注册
    uint8                   bGetModel;             //模组获取
    uint8                   pulseType;             //脉冲采集方式
    uint8                   bGetCtrl;              //是否支持控制采集
    
    OOP_MODULE_INFO_NEW_T   modelInfo;             //模组信息
    RSP_MODEL_T             tRspModel[2];          //最多支持2路

    pthread_mutex_t         flock;                 //数据访问锁
    /* 遥信汇总 */
    RSP_REPORT_DATA_T       curALLRmt;             //当前状态
    RSP_REPORT_DATA_T       lastALLRmt;            //上次状态
    RSP_REPORT_DATA_T       oldALLRmt;             //上上次状态
    /* 脉冲汇总 */
    RSP_PULSE_A_T           pulseAll;              //脉冲
    
    RSP_REPORT_LIST_T reportList;   //上报缓存
}RSP_DATA_T;


void rsp_write_queue(RSP_REPORT_LIST_T *pList, RSP_REPORT_DATA_T *pNode);
int rsp_read_queue(RSP_REPORT_LIST_T *pList, RSP_REPORT_DATA_T *pNode);
int rsp_write_msg_queue(RSP_MSG_LIST_T *pList, uint8 *pMsg, uint16 msglen);
int rsp_read_msg_queue(RSP_MSG_LIST_T *pList, uint8 *pMsg, uint16 buflen);
void rsp_get_curtime(OOP_DATETIME_T *pCurTime);

int rsp_write_config(uint8 id, uint8 *config, uint8 num);
int rsp_write_shaketime(uint8 id, uint16 shaketime);
void rsp_write_ctrlstatus(uint8 num, uint16 status);

#ifdef __cplusplus
}
#endif

#endif // __RSP_DATA_H__


