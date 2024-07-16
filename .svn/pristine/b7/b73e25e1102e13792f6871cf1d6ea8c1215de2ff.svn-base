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
#ifndef	__LC_MONITOR_H
#define __LC_MONITOR_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define  LC_MONITOR_SEEK_PORT   0
#define  LC_MONITOR_GET_INFO    1
#define  LC_MONITOR_WORK        2

#define LC_UPDATE_FILE "/data/app/lcMonitor/lc_update_file"

#define LC_UPPACK_SIZE  (196) //互感器最大只能传输196字节 

/** @brief 文件分块传输状态 */
typedef struct
{
    uint16 nBlockNum;        //总块数
    uint8  packStatus[64];   //包传输状态
} LC_FILE_STATUS_T;

/** @brief 文件分块传输信息 */
typedef struct
{
    uint8  fileMark;   //文件标识    
    uint32 nBlockNum;  //总块数
    uint16 nEndLengh;  //最后一次长度
    uint32 nBlockId;   //当前块序号
    uint16 nLengh;     //当前块长度
    uint8 *fileData;   //当前块内容指针
} LC_FILE_TRANS_T;

/* 回路监控公共数据 只支持1个模块 */
typedef struct taglcMonitorData
{
    uint8            status;    //状态
    uint8            res[2];
    MOD_INFO_ACK_T   module[LC_MAP_ID_MAX];   //下标加1 表示模组序号
    DB_CLIENT        clientid;
    int              tasksocket;
    int              recvlen;
    uint8            recvBuf[TASK_MSG_MAX_LEN];
    void *           pUart;   //串口句柄
    LC_FILE_TRANS_T  *fileInfo;   //文件传输信息   
    int               fd;         //文件句柄
 #ifdef PRODUCT_SCU
    UART_PORT_T      transport;     //透传端口
    uint8            slavereg;      //从模式是否注册
    uint8            transLen;      //透传报文长度
    uint8            transBuf[288]; //透传报文缓存
#endif
}LC_MONITOR_DATA_T;

/* 入口函数申明 */
typedef int (*TaskMsgProcFun1)(TASK_MSG_t*, LC_MONITOR_DATA_T*);
typedef struct tagLcMonitorFun
{
    uint32             msgid;
    TaskMsgProcFun1    pProcMsg; /* 处理函数 */
}LC_MONITORFUN_T;

#ifdef __cplusplus
}
#endif

#endif


