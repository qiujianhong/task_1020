

/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 采集任务管理调度APP主函数
* @date： 2019-10-24
*********************************************************************
*/
#include "CtsAll.h"
#include "newcb.h"
#include "Mqtt.h"
#include "db_api.h"
#include "task_msg.h"
#include "iniparser.h"
#include "hal.h"
#include "trace_back.h"
#include "area.h"
#include "CtsACMeter.h"
#include "CtsPlc.h"
#include "min_gather.h"
#include "CtsBt.h"

//#include <adc.h>
extern int appmsg_split(uint8 * pBuf, uint16 bufLen, MESSAGE_INFO_T * *ppMsgOut);
uint8           gmqttThreadFlag; //mqtt线程标志
uint8           gTaskThreadFlag; //任务管理线程标志
uint8           gTGThreadFlag; //总加组线程标志

//HANDLE  UpHandle[10];
uint8           gDownThreadFlag[AMR_THREAD_NUM] =
{
    0
};

//下行通信线程标志
uint8           gRebootAmrThread[AMR_THREAD_NUM] =
{
    0
};

//重启抄表线程标志
uint8           gAmrThreadFlag[AMR_THREAD_NUM] =
{
    0
};

//自动抄表线程标志
#ifdef AREA_JIANGSU
AmrThreadArg_T  gCajThreadArg[2];

#endif

AmrThreadArg_T  gAmrThreadArg[AMR_THREAD_NUM];
LockQueue_T *   pAmrRecvDown[AMR_THREAD_NUM]; //抄表线程下行接收共享内存指针
LockQueue_T *   pAmrSendDown[AMR_THREAD_NUM]; //抄表线程下行发送共享内存指针
LockQueue_T *   pAmrRecvDown_Cb[AMR_THREAD_NUM]; //抄表线程下行接收共享内存指针
LockQueue_T *   pAmrRecvUp[AMR_THREAD_NUM]; //抄表线程上行接收共享内存指针
LockQueue_T *   pAmrSendUp[AMR_THREAD_NUM]; //抄表线程上行发送共享内存指针

LockQueue_T LockRecvDown[AMR_THREAD_NUM];       //下行接收缓冲区结构
LockQueue_T LockSendDown[AMR_THREAD_NUM];       //下行发送缓冲区结构
LockQueue_T LockSendDown_Cb[AMR_THREAD_NUM];    //下行发送缓冲区结构
Down_ThreadArg_T DownThreadArg[AMR_THREAD_NUM];

LockQueue_T LockRecvUp[AMR_THREAD_NUM];         //上行接收缓冲区结构
LockQueue_T LockSendUp[AMR_THREAD_NUM];         //上行发送缓冲区结构
ThreadArg_T UpThreadArg[AMR_THREAD_NUM];        //上行线程参数

LockQueue_T     gMeterSearchQueue; //搜表用的队列

#ifdef AREA_HUNAN
LORA_AMR_T      g_LoRaAmr;
extern OOP_OAD_U gTemperatureOad;
extern OOP_OAD_U gSmokeOad;
extern OOP_OAD_U gWaterOad;
extern OOP_OAD_U gTransTempOad;
extern uint8    gTemperatureAddr; //温湿度传感器地址
extern uint8    gTemperaturePort; //温湿度传感器端口

extern uint8    gSmokeAddr; //烟雾传感器地址
extern uint8    gSmokePort; //端口

extern uint8    gWaterAddr; //水浸传感器地址
extern uint8    gWaterPort; //端口

extern uint8    gTransTempAddr[6]; //变压器桩头温度传感器地址
extern uint8    gTransTempPort; //端口

#endif

#ifdef AREA_FUJIAN
FUJIAN_grop_meter oop_grop_meter; //群组地址结构体
DBUPDATE_QUEUE_T pGropMeterFileUpdate; //档案变更事件
OOP_GATHER_E    gGatherMode;
uint8           g485ShareFlag; //485-2口数据共享功能

#endif

DBUPDATE_QUEUE_T pTaskDBUpdate; //存储任务或者方案的数据变更事件
DBUPDATE_QUEUE_T pMeterFileUpdate; //档案变更事件
DBUPDATE_QUEUE_T pTaskRecordFileDel[AMR_THREAD_NUM + 1]; //存放需要删除的任务记录文件的id

//方案任务执行队列
SCHEME_TASK_RUN_INFO SchemeTaskRunQueue[SCHEME_MAX_NUM]; //载波方案任务队列
SCHEME_TASK_RUN_INFO * SchemeTaskRun_NormalLink = NULL; //当前载波方案日常的任务

//方案任务参数队列
OOP_TASK_CFG_LIST SchemeTaskCfgList; //载波方案任务参数列表

//档案队列
Meter_File_T    MeterFileAll; //终端档案信息
uint8           MeterFileFlag; //档案修改标志
pthread_rwlock_t TaskRWLock; //任务列表读写锁 多线程访问
pthread_mutex_t SeqLock;

extern DateTime_T gCurBinTimeTask; //当前BIN时间

extern DateTime_T gPreBinTimeTask; //上次BIN时间
char *          DownName; //定义下行的消息发送方名称
char *          CCOName;
char *          WIRELESSName;
ThreadArg_T     APPArg; //和其余APP交互的结构体
uint8           LocalAddr[6] =
{
    0
};

//终端地址
uint8           IsSetAddr[PLC_NUM] =
{
    0
};

//是否给本地设置过终端地址
//uint16 SearchMeterSeq = 0;  //搜表多线程上报数据后统一设置搜表信息点号
//pthread_mutex_t SearchMeterSeqLock;
uint16          NetTopologySeq = 0; //存储网络拓扑信息使用的信息点号
pthread_mutex_t NetTopologySeqLock;

#if ( defined AREA_JIANGXI) || (defined AREA_ANHUI) || (defined AREA_JIANGSU)
const OOP_OAD_U PerceptionTerminalOad =
{
    0xFF110E00
};

//感知终端信息
#endif

const OOP_OAD_U EventReportOad =
{
    0xFF100A00
};

//电表事件存储用的oad
const OOP_OAD_U EventReportOad645 =
{
    0xFF100B00
};
const OOP_OAD_U MinuteReportOad =
{
    0xFF100D00
};

//电表事件存储用的oad
const OOP_OAD_U AcrossAreaOad =
{
    0X60020500
};

//非本台区电表结果存储OAD
const OOP_OAD_U NodeRejectOad =
{
    0XF2091500
};

//节点拒绝信息存储oad
const OOP_OAD_U PowerOnOffOad =
{
    0xFF100A00
};

//停上电事件存储oad 不再扩0C00
const OOP_OAD_U MtrTimingOad =
{
    0xFFA00100
};

//电表校时产生的信息
const OOP_OAD_U ClockOutOad =
{
    0xFFA00200
};

//电表时钟偏差
//const OOP_OAD_U  IdenyifyResultOad = {0x3E020200};  //电表校时产生的信息
uint16 EventReportInfoNum;
uint16 MinuteReportnfoNum;
uint16 EventReportInfoNum645;

#if ( defined AREA_JIANGXI) || (defined AREA_ANHUI)|| (defined AREA_JIANGSU)
uint16 PerceptionTerminalInfoNum;
#endif

uint16 NodeRejectNum;
uint16 PowerOnOffNum;

//uint16 IdenyifyResultNum;
pthread_mutex_t EventReportNumLock;

DB_UPDATE_T dbupdateMeter; //存储收到的档案变更信息
DB_UPDATE_T dbupdateTask;

EVENT_PLAN_T EventPlanList; //存储采集类型为2的所有事件采集方案

OOP_PLAN_Transparent_LIST_T oopPlanTransparentList; //透明方案方案集合


TASK_MONITOR_T TaskMonitor; //任务监控6034

SEARCH_METER_CLEAR_T SearchMeterClean; //60020800 搜表结果清空选项

SEARCH_METER_CYCLICITY_T SearchMeterParam; //周期搜表参数

SYNC_TIMING_T SyncTimingParam; //广播校时相关参数

NODE_UPDATE_CYCLE_T UpdateCycle; //0XF2090600更新周期

ACROSS_AREA_T AcrossAreaResult; //非本台区电能表上报信息结果

METER_SEARCH_RES_T MeterSearchResult; //搜表结果

FILE_UPDATE_ENCRYPT_T gFileUpdateEncrypt;       //转加密的升级方案

int gEncryptPipe; //定义一个转加密任务的变量 默认是-1 如果有端口在执行转加密任务就置位 别的端口就不能执行任务

CCO_STA_INFO_T ccostainfo;
uint8 Rs485PipeConut = 0;

uint8 MeterSearchResultClearFlag = 0; //搜表结果清空标志 某线程清空后其余线程无需清空
uint8 Modulepullflag[5] =
{
    0
};

//模组拔出标志;数组第一个元素代表模组1；依次类推，第五个元素代表模组5;
char mqttIP[100] =
{
    0
};
int mqttport = 0;
char CommLog[100] =
{
    0
};
char TaskLog[100] =
{
    0
};
char RecLog[100] =
{
    0
};
char RelayLog[100] =
{
    0
};
char UpLog[100] =
{
    0
};
char MqttLog[100] =
{
    0
};
char TgLog[100] =
{
    0
};
char SocketDir[100] =
{
    0
};
char PrivateDir[100] =
{
    0
};
char pathname[AMR_THREAD_NUM][512];

DB_AGENT gDBAgent = 0; //和数据中心交互的agent
DB_CLIENT gDBClientMain = 0;
DB_CLIENT gDBClientTask = 0;
pthread_mutex_t gMqttSendLock;
int gMqttSock = 0; //mqtt线程创建的线程间通信fd
int gMainSock = 0; //主线程创建的
int gTaskSock = 0; //task线程创建的

TotalGroupThreadArg TGThreadArg; //总加组处理线程使用的结构体
uint8 gSafeButton = 0;
uint8 gSafeButtonHPLC = 0; //载波默认的MAC校验开关 默认关闭
uint8 ReloadTaskFlag = 0; //档案变更后置为1 任务线程检查后重新初始化任务
uint8 DataInitFlag[AMR_THREAD_NUM + 1] =
{
    0
};

//数据区初始化标记 每个抄表线程清空记录的事件序号
uint8 DataInitFlagMain = 0;
uint8 gRouterStudy = 0;
uint8 gMinuteCollect = 0;

#if ( defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;
#endif

OOP_OCTETVAR16_T tmnAddr; //终端通信地址
OOP_OCTETVAR64_T MAC;
#define FROEN_PATH "/data/app/forezon"
uint8 gReponsePIIDCAD = 1;
uint8 minutecollect;
uint8 routecollectstate;
uint8 nodecollectstate;
uint16 nodecollectcount;
uint16 cajpn = 300;
uint16 nodecount = 0;
OOP_COLLECTOR_T Collector;

#ifdef PRODUCT_ECU
pthread_mutex_t gModuleInfoLock;
OOP_MODULE_INFO_NEW_T gModuleInfo; //模组信息

#endif
uint8 gdelay=0;
/*******************************************************************************
* 函数名称: AlreadyRunning
* 函数功能: 检测进程是否运行
* 输入参数：TaskName            进程名指针
* 输出参数：无
* 返 回 值: 1                   进程正在运行
*           0                   进程未运行
*******************************************************************************/
static int AlreadyRunning(char * TaskName)
{
    char sBuf[128];
    pid_t pid;
    struct flock locker;
    int fd;

    sprintf(sBuf, "%s.pid", TaskName);
    fd  = open(sBuf, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if(fd < 0)
    {
        return 0; //文件打不开，表示程序未在执行
    }

    locker.l_type = F_WRLCK;
    locker.l_start = 0;
    locker.l_whence = SEEK_SET;
    locker.l_len = 0;

    if(fcntl(fd, F_SETLK, &locker) < 0)
    {
        close(fd);
        return 1; //不能上锁，表示程序已经执行
    }

    ftruncate(fd, 0);
    pid = getpid();
    write(fd, &pid, sizeof(pid_t));

    return 0;
}

/**********************************************************************
* @name      : MntVersionCheck
* @brief     ：终端通信地址更改
* @param[in] ：OOP_OCTETVAR16_T tLocalAddr 当前通信地址
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2020-02-16
* @Update    :
**********************************************************************/
void GetMntAddr(OOP_OCTETVAR16_T tLocalAddr)
{
    //终端通信地址
    tmnAddr = tLocalAddr;
    MAC.nNum = 6;
    memcpy(MAC.value, tmnAddr.value, MAC.nNum);

}

/**********************************************************************
* @name      : evt_read_nomal_data
* @brief     ：读取普通型数据
* @param[in] ：int    clientid     数据中心句柄
               uint8 logicId   逻辑设备号
               uint16 mtr_id   信息点号
               uint32 value_oad OAD
               uint16 datalen   数据长度
* @param[out]：uint8* pout_buf   数据
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2021-1-26
* @Update    :
**********************************************************************/
int evt_read_nomal_data(int clientid, uint8 logicid, uint16 infonum, uint32 value_oad, uint32 data_len, uint8 * pout_buf)
{
    int ret = -1;
    uint32 len = 0;
    NOMAL_OAD_T nomalOAD;

    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));

    nomalOAD.logicId = logicid;
    nomalOAD.infoNum = infonum;
    nomalOAD.oad.value = value_oad;
    ret = db_read_nomal(clientid, &nomalOAD, data_len, pout_buf, &len);

    if(ret != ERR_OK || len != data_len)
    {

        return ERR_NORMAL;
    }

    return ret;
}



/**********************************************************************
* @name      : readMntAddr
* @brief     ：读取终端地址
* @param[in] ：int             clientid     数据中心句柄
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2021-1-26
* @Update    :
**********************************************************************/
int readMntAddr(int clientid)
{
    int ret = -1;
    OOP_OCTETVAR16_T tLocalAddr;

    ret = evt_read_nomal_data(clientid, 0, 0, 0x40010200, sizeof(OOP_OCTETVAR16_T), (uint8 *) &tLocalAddr);

    if(ret == ERR_OK)
    {
        GetMntAddr(tLocalAddr);
    }

    return ret;
}

/*******************************************************************************
* 函数名称: taskmng_global_init
* 函数功能: 全局初始化
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: -1 初始化失败
*******************************************************************************/
int taskmng_global_init(void)
{
    int ret;
    uint8 i;
    gEncryptPipe = -1;
    CCOName = CCO_APP_NAME;
    DownName = APPNAME;
    WIRELESSName = WIRELESSDCM_APP_NAME;
    memset(&TGThreadArg, 0x00, sizeof(TotalGroupThreadArg));
    MeterFileAll.FileState = TASK_CFG_UNINIT;
    SchemeTaskCfgList.TaskCfgState = TASK_CFG_UNINIT;
    hal_init();

#ifdef AREA_HUNAN
    taskmng_init_file_load_area();

#else

    taskmng_init_file_load();
#endif

    taskmng_privatedir_prepare(PrivateDir);

    Init_Rs485_Pipe();

    ret = task_msg_prepare(SocketDir);

    if(ret != 0)
    {
        printf("task_msg_prepare error!\n");
        return - 1;
    }

    if(pthread_rwlock_init(&SchemeTaskCfgList.rwLock, NULL) != 0)
    {
        printf("SchemeTaskCfgList.rwLock lock error!\n");
        return - 1;
    }

    if(pthread_rwlock_init(&TaskRWLock, NULL) != 0)
    {
        printf("init pthread TaskRWLock lock error!\n");
        return - 1;
    }

    if(pthread_mutex_init(&SeqLock, NULL) != 0)
    {
        printf("init pthread SeqLock lock error!\n");
        return - 1;
    }

    if(pthread_mutex_init(&EventReportNumLock, NULL) != 0)
    {
        printf("init pthread EventReportNumLock lock error!\n");
        return - 1;
    }

    if(pthread_mutex_init(&NetTopologySeqLock, NULL) != 0)
    {
        printf("init pthread SeqLock lock error!\n");
        return - 1;
    }

    memset(&TaskMonitor, 0x00, sizeof(TASK_MONITOR_T));

    if(pthread_mutex_init(&TaskMonitor.MonitorLock, NULL) != 0)
    {
        printf("init TaskMonitor lock error!\n");
        return - 1;
    }

    memset(&SearchMeterClean, 0x00, sizeof(SEARCH_METER_CLEAR_T));

    if(pthread_mutex_init(&SearchMeterClean.lock, NULL) != 0)
    {
        printf("init SearchMeterClean lock error!\n");
        return - 1;
    }

    memset(&SearchMeterParam, 0x00, sizeof(SEARCH_METER_CYCLICITY_T));

    if(pthread_mutex_init(&SearchMeterParam.Lock, NULL) != 0)
    {
        printf("init SearchMeterParam lock error!\n");
        return - 1;
    }

    memset(&SyncTimingParam, 0x00, sizeof(SYNC_TIMING_T));

    if(pthread_mutex_init(&SyncTimingParam.Lock, NULL) != 0)
    {
        printf("init SyncTimingParam lock error!\n");
        return - 1;
    }

    memset(&UpdateCycle, 0x00, sizeof(NODE_UPDATE_CYCLE_T));

    if(pthread_mutex_init(&UpdateCycle.Lock, NULL) != 0)
    {
        printf("init UpdateCycle lock error!\n");
        return - 1;
    }

    memset(&AcrossAreaResult, 0x00, sizeof(ACROSS_AREA_T));

    if(pthread_mutex_init(&AcrossAreaResult.Lock, NULL) != 0)
    {
        printf("init pthread AcrossAreaResult lock error!\n");
        return - 1;
    }

    memset(&MeterSearchResult, 0x00, sizeof(METER_SEARCH_RES_T));

    if(pthread_mutex_init(&MeterSearchResult.lock, NULL) != 0)
    {
        printf("init pthread AcrossAreaResult lock error!\n");
        return - 1;
    }

    memset(&gFileUpdateEncrypt,0x00,sizeof(gFileUpdateEncrypt));
    if(pthread_mutex_init(&gFileUpdateEncrypt.lock, NULL) != 0)
    {
        printf("init pthread gFileUpdateEncrypt lock error!\n");
        return - 1;
    }

    memset((uint8 *) &EventPlanList, 0x00, sizeof(EVENT_PLAN_T));
    pthread_rwlock_init(&EventPlanList.rwLock, NULL);
    memset(&ccostainfo, 0x00, sizeof(CCO_STA_INFO_T));

    if(pthread_mutex_init(&ccostainfo.lock, NULL) != 0)
    {
        printf("init pthread AcrossAreaResult lock error!\n");
        return - 1;
    }

    if(pthread_mutex_init(&gMqttSendLock, NULL) != 0)
    {
        printf("init pthread gMqttSendLock lock error!\n");
        return - 1;
    }
    taskmng_meter_power_cache_init();
    taskmng_power_state_init();
    taskmng_meter_power_buflist_init();


    gDBAgent = db_agent_create(AGENT_TYPE_THREAD, DB_AGENT_CTS, DBSendMQTT, DB_CLIENT_MAX);

    if(gDBAgent < 0)
    {
        TASK_FMT_TRACE(-1, MQTT_LOG, "创建dbagent失败！！ \n");
        return - 1;
    }

    //mqtt初始化
    TASK_FMT_DEBUG(-1, MQTT_LOG, "mqtt 开始初始化！！！\n ");
    taskmng_mqtt_init();
    gDBClientMain = db_client_create(DB_AGENT_CTS, DB_CLIENT_MAIN);

    if(gDBClientMain < 0)
    {
        TASK_FMT_TRACE(-1, MQTT_LOG, "创建main dbclient失败！！ \n");
        return - 1;
    }

    //初始化档案信息
    if(pthread_rwlock_init(&MeterFileAll.rwLock, NULL) != 0)
    {
        printf("init pthread pMeterFileAll lock error!\n");
        return - 1;
    }

#ifdef AREA_FUJIAN
    memset(&oop_grop_meter, 0, sizeof(oop_grop_meter));

    if(pthread_rwlock_init(&oop_grop_meter.rwLock, NULL) != 0)
    {
        printf("oop_grop_meter.rwLock lock error!\n");
        exit(1);
    }

    Init_Get_Grop_Meter(&oop_grop_meter);
    memset(&pGropMeterFileUpdate, 0x00, sizeof(DBUPDATE_QUEUE_T));

    if(pthread_mutex_init(&pGropMeterFileUpdate.QueueLock, NULL) != 0)
    {
        printf("pGropMeterFileUpdate error!\n");
        return - 1;
    }

#endif

    memset(&pTaskDBUpdate, 0x00, sizeof(DBUPDATE_QUEUE_T));

    if(pthread_mutex_init(&pTaskDBUpdate.QueueLock, NULL) != 0)
    {
        printf("pTaskDBUpdate error!\n");
        return - 1;
    }

    memset(&pMeterFileUpdate, 0x00, sizeof(DBUPDATE_QUEUE_T));

    if(pthread_mutex_init(&pMeterFileUpdate.QueueLock, NULL) != 0)
    {
        printf("pMeterFileUpdate error!\n");
        return - 1;
    }

    for(i = 0; i < AMR_THREAD_NUM + 1; i++)
    {
        memset(&pTaskRecordFileDel[i], 0x00, sizeof(DBUPDATE_QUEUE_T));

        if(pthread_mutex_init(&pTaskRecordFileDel[i].QueueLock, NULL) != 0)
        {
            printf("pTaskDBUpdateerror!\n");
            exit(1);
        }
    }

    return 0;
}


/*******************************************************************************
* 函数名称: main
* 函数功能: 线程创建和维护
* 输入参数:
* 输出参数:
* 内部处理:
* 返 回 值: 无
*******************************************************************************/
int main(int argc, char * argv[])
{
    uint8 i = 0;
    int32 err;


    pthread_t MqttTheadID;                          //mqtt线程标识

#ifndef AREA_FUJIAN
    memset(&TGThreadArg, 0x00, sizeof(TotalGroupThreadArg));
    pthread_t TGThreadID;
#endif

    pthread_t AmrThreadID[AMR_THREAD_NUM];          //自动抄表线程标识

#ifdef AREA_HUNAN
    pthread_t LoRaThreadID;                         //LoRa抄表线程标识
#endif

    pthread_t TaskThreadID;                         //任务管理线程标识


    uint16 MeterCount = 0;

    LockQueue_T LockSendApp;                        //和其余APP交互发送的消息
    LockQueue_T LockRecvApp;                        //和其余APP交互收到的消息

    LockQueue_T LockSendTg;                         //交采线程用
    LockQueue_T LockRecvTg;                         //交采线程用
    ThreadArg_T LockBufTg;

    uint8 RecvBuf[MAX_MSG_LEN] =
    {
        0
    };
    int RecvLen = 0;
    uint32 sender;
    uint32 maincnt = 0;
    OOP_OAD_U DeleteOad =
    {
        0xF2090C00
    };

    printf("-COMPILE TIME : %s %s\n", __TIME__, __DATE__);
    printf("-GCC VERSION  : %s\n", __VERSION__);

    CCOName = CCO_APP_NAME;
    DownName = APPNAME;

    //检测进程是否在运行
    if(AlreadyRunning("/var/run/taskManager") > 0)
    {
        exit(1);
    }

    if(argc >= 2)
    {
        if(0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("Debug, waiting 30s\n");
            sleep(30);
        }
    }

    /* 信号处理 */
    init_sig_action("/data/app/taskManager/log/Exception.log");



    if(taskmng_global_init() < 0)
    {
        TASK_FMT_DEBUG(-1, UP_LOG, "初始化失败\n");
        exit(1);
    }
    TASK_FMT_DEBUG(-1, UP_LOG, "@@@@@@@@@@@taskManager开始运行@@@@@@@@@@@@@@@ \n");
    TASK_FMT_DEBUG(-1, UP_LOG, "taskManager 版本号 %s 发布日期 %s 地区 %s 产品 %s\n", APP_VERSION, APP_PUBLISH_TIME, AREANAME_PLACE,PRODUCT_NAME);

    memset(&LockSendApp, 0x00, sizeof(LockQueue_T));
    memset(&LockRecvApp, 0x00, sizeof(LockQueue_T));

    if(pthread_mutex_init(&LockSendApp.QueueLock, NULL) != 0)
    {
        printf("init pthread LockSendApp error!\n");
        exit(1);
    }

    if(pthread_mutex_init(&LockRecvApp.QueueLock, NULL) != 0)
    {
        printf("init pthread LockSendApp error!\n");
        exit(1);
    }

    APPArg.RecvBuf = &LockRecvApp;
    APPArg.SendBuf = &LockSendApp;

    memset(&LockSendTg, 0x00, sizeof(LockQueue_T));
    memset(&LockRecvTg, 0x00, sizeof(LockQueue_T));

    if(pthread_mutex_init(&LockSendTg.QueueLock, NULL) != 0)
    {
        printf("init pthread LockSendTg error!\n");
        exit(1);
    }

    if(pthread_mutex_init(&LockRecvTg.QueueLock, NULL) != 0)
    {
        printf("init pthread LockRecvTg error!\n");
        exit(1);
    }

    LockBufTg.RecvBuf = &LockRecvTg;
    LockBufTg.SendBuf = &LockSendTg;
    TGThreadArg.pBufArg = &LockBufTg;

    //每个端口创建载波抄表线程
    for(i = 0; i < AMR_THREAD_NUM; i++)
    {
        memset(&gAmrThreadArg[i], 0, sizeof(AmrThreadArg_T));
        gAmrThreadArg[i].CommPipe = i;

        //下行通信线程参数初始化
        memset(&LockRecvDown[i], 0, sizeof(LockQueue_T));
        memset(&LockSendDown[i], 0, sizeof(LockQueue_T));
        memset(&LockSendDown_Cb[i], 0, sizeof(LockQueue_T));

        if(pthread_mutex_init(&LockRecvDown[i].QueueLock, NULL) != 0)
        {
            PIPEPRINT(i, "init pthread LockRecvDown lock error!\n");
            exit(1);
        }

        if(pthread_mutex_init(&LockSendDown[i].QueueLock, NULL) != 0)
        {
            PIPEPRINT(i, "init pthread LockSendDown lock error!\n");
            exit(1);
        }

        if(pthread_mutex_init(&LockSendDown_Cb[i].QueueLock, NULL) != 0)
        {
            PIPEPRINT(i, "init pthread LockSendDown lock error!\n");
            exit(1);
        }

        DownThreadArg[i].RecvBuf = &LockRecvDown[i];
        DownThreadArg[i].SendBuf = &LockSendDown[i];
        DownThreadArg[i].CbSendBuf = &LockSendDown_Cb[i];
        gAmrThreadArg[i].DownArg = &DownThreadArg[i];

        //上行IPC通信初始化
        memset(&LockRecvUp[i], 0, sizeof(LockQueue_T));
        memset(&LockSendUp[i], 0, sizeof(LockQueue_T));

        if(pthread_mutex_init(&LockRecvUp[i].QueueLock, NULL) != 0)
        {

            PIPEPRINT(i, "init pthread LockRecvUp lock error!\n");
            exit(1);
        }

        if(pthread_mutex_init(&LockSendUp[i].QueueLock, NULL) != 0)
        {

            PIPEPRINT(i, "init pthread LockSendUp lock error!\n");
            exit(1);
        }

        UpThreadArg[i].RecvBuf = &LockRecvUp[i];
        UpThreadArg[i].SendBuf = &LockSendUp[i];
        gAmrThreadArg[i].UpArg = &UpThreadArg[i];


    }

    err = pthread_create(&MqttTheadID, NULL, MqttThread, NULL);

    if(err != 0)
    {
        printf("0pthread_create UpThread error");
        exit(1);
    }

    //检查数据中心和模组管理器是否注册成功
    while(1)
    {
        if(taskmng_register_app_check(DBCENTER_NAME) == TRUE)
            break;

        TASK_FMT_TRACE(-1, UP_LOG, "数据中心未注册成功 3秒后重试 \n");

#ifdef PRODUCT_ECU

        if(taskmng_register_app_check(MAPMANAGER_APP_NAME) == TRUE)
            break;

        TASK_FMT_TRACE(-1, UP_LOG, "模组管理器未注册成功 3秒后重试 \n");

#endif

        sleep(3);
    }
    //稍晚点初始化esam 以防安全app还没起
    Init_Fd_Esam();
    security_rand_get();

#ifdef AREA_HUNAN
    taskmng_sensor_param_load();
#endif

#ifdef AREA_JIANGSU
    frozen_init_jiangsu();
    readMntAddr(gDBClientMain);
    char fileName[100];
    sprintf(fileName, "%s", FROEN_PATH);
    min_gather_set_file(fileName, MAC.value);
    min_gather_frozen_start();
    min_gather_real_start(FALSE);
#endif


    taskmng_normal_data_delete(0xFFFF, DeleteOad);


    MeterCount = taskmng_meter_file_init(&MeterFileAll);

    if(MeterCount == 0)
    {
        TASK_FMT_TRACE(-1, TASK_LOG, "档案读取失败\n");
    }

    TASK_FMT_TRACE(-1, TASK_LOG, "档案初始化完成!\n");
    taskmng_amr_info_read();
    taskmng_speed_list_get();
    taskmng_meter_search_result_read();

    if(MeterFileAll.MeterCount != 0)
        MeterFileAll.FileState = TASK_CFG_INITED;

#if (   defined AREA_JIANGXI) || (defined AREA_ANHUI)
    taskmng_event_report_num_load(PerceptionTerminalOad);
#endif

    taskmng_event_report_num_load(EventReportOad);

#if (   defined AREA_JIANGSU)
    taskmng_event_report_num_load(MinuteReportOad);
#endif

    taskmng_event_report_num_load(EventReportOad645);

    //taskmng_event_report_num_load(AcrossAreaOad);
    taskmng_event_report_num_load(NodeRejectOad);
    taskmng_event_report_num_load(PowerOnOffOad);
    taskmng_search_meter_param_load();
    taskmng_search_meter_clean_load();
    taskmng_sync_time_param_load();
    taskmng_update_cycle_load();
    taskmng_across_area_result_load();
    taskmng_local_addr_read(LocalAddr);

#if (   defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
    taskmng_perception_tmn_info_load();
#endif

#ifdef AREA_FUJIAN

    //获取外部设备采集模式
    gGatherMode = fujian_gather_mode_get(gDBClientMain);
    g485ShareFlag = fujian_485_share_flag_get(gDBClientMain);

#endif



    //每个端口创建载波抄表线程
    for(i = 0; i < AMR_THREAD_NUM; i++)
    {

#ifdef AREA_FUJIAN

        if(gGatherMode == OOP_GATHER_FJ)
        {
            //尝试载波和485用一个线程
            err = pthread_create(&AmrThreadID[i], NULL, thread_amr_fujian, &gAmrThreadArg[i]);
        }
        else
        {
            if(i < PLC_NUM)
            {
                if(i == 0)
                    err = pthread_create(&AmrThreadID[i], NULL, AmrThread, &gAmrThreadArg[i]);
                else
                    err = pthread_create(&AmrThreadID[i], NULL, BtThread, &gAmrThreadArg[i]);
            }
            else if(i < PLC_NUM + MAX_485_NUM)
            {
                //只能按照485路数建线程 要不然会多建两个线程 导致485相关参数的数组溢出 最终导致莫名的问题
                err = pthread_create(&AmrThreadID[i], NULL, AmrThread_485, &gAmrThreadArg[i]);
            }
        }

#else

        //创建抄表线程
        if(i < PLC_NUM)
        {
            if(i == 0)
                err = pthread_create(&AmrThreadID[i], NULL, AmrThread, &gAmrThreadArg[i]);
            else
                err = pthread_create(&AmrThreadID[i], NULL, BtThread, &gAmrThreadArg[i]);
        }
        else
        {
            err = pthread_create(&AmrThreadID[i], NULL, AmrThread_485, &gAmrThreadArg[i]);
        }

#endif

        if(err != 0)
        {
            PIPEPRINT(i, "0pthread_create AmrThread error");
            exit(1);
        }

    }

#ifdef AREA_FUJIAN

    if(gGatherMode == OOP_GATHER_FJ)
    {
        //创建任务管理线程 福建增补创建新的任务线程
        err = pthread_create(&TaskThreadID, NULL, thread_task_fujian, NULL);

        if(err != 0)
        {
            printf("pthread_create TaskThread error");
            exit(1);
        }

    }
    else
    {
        //创建任务管理线程
        err = pthread_create(&TaskThreadID, NULL, TaskThread, NULL);

        if(err != 0)
        {
            printf("pthread_create TaskThread error");
            exit(1);
        }
    }

#else

    //创建任务管理线程
    err = pthread_create(&TaskThreadID, NULL, TaskThread, NULL);

    if(err != 0)
    {
        printf("pthread_create TaskThread error");
        exit(1);
    }

#endif

#ifndef AREA_FUJIAN

    //创建总加组处理线程
    err = pthread_create(&TGThreadID, NULL, TGThread, NULL);

    if(err != 0)
    {
        printf("pthread_create TGThread error");
        exit(1);
    }

#endif

    gMainSock = task_msg_init(IPC_MAIN);

    if(gMainSock <= 0)
    {
        printf("main task_msg_init error! ");
        exit(1);
    }

    task_msg_settime(gMainSock, 0, 100000);

    //LoadHPLCChannel();
    taskmng_HPLC_across_area_flag_load();
    taskmng_HPLC_node_reject_flag_load();

#ifdef PRODUCT_ECU
    TASK_FMT_TRACE(-1, UP_LOG, "查询模组信息！！！！！！ \n");

    if(pthread_mutex_init(&gModuleInfoLock, NULL) != 0)
    {
        printf("init pthread gModuleInfoLock lock error!\n");
        exit(1);
    }

    taskmng_all_module_info_get();
    TASK_FMT_TRACE(-1, UP_LOG, "查询模组信息结束！！！！！！ \n");
#endif

    while(1)
    {
        if(DataInitFlagMain == 1)
        {
#if (           defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
            taskmng_perception_tmn_info_clear();
#endif

            taskmng_meter_search_result_clear();
            taskmng_search_meter_param_load();
            taskmng_search_meter_clean_load();
            taskmng_sync_time_param_load();
            taskmng_update_cycle_load();
            taskmng_across_area_result_clear();
            DataInitFlagMain = 0;
        }

        //接收消息
        memset(&dbupdateMeter, 0x00, sizeof(DB_UPDATE_T));

        RecvLen = task_recv_msg(gMainSock, RecvBuf, MAX_MSG_LEN, &sender);

        if(RecvLen > 0)
        {
            TASK_MSG_t * msg = (TASK_MSG_t *)
            RecvBuf;

            switch(msg->msg_id)
            {
                case MSG_DBUPDATE:
                    {
                        memcpy(&dbupdateMeter, msg->msg, msg->msglen);
                        TASK_FMT_TRACE(-1, MQTT_LOG, "收到mqtt发送的数据更新消息 infonum %d oad %08x !\n", dbupdateMeter.infonum, dbupdateMeter.oad);

                        //                if(dbupdateMeter.oad.value == 0x60000200)
                        //                    MeterFileAll.FileState = TASK_CFG_MODIFY;
                        //                else
                        if(dbupdateMeter.oad.value == 0x60020200)
                        {
                            taskmng_meter_search_result_change_fun(dbupdateMeter.infonum);
                        }
                        else if(dbupdateMeter.oad.value == 0x60020500)
                        {
                            taskmng_across_area_result_change_fun(dbupdateMeter.infonum);
                        }
                        else if(dbupdateMeter.oad.value == 0x60020900)
                        {
                            taskmng_search_meter_param_load();
                        }
                        else if(dbupdateMeter.oad.value == 0x60020800)
                        {
                            taskmng_search_meter_clean_load();
                        }
                        else if(dbupdateMeter.oad.value == 0x42040200 || dbupdateMeter.oad.value == 0x42040300)
                        {
                            taskmng_sync_time_param_load();
                        }
                        else if(dbupdateMeter.oad.value == 0xF2090600)
                        {
                            taskmng_update_cycle_load();
                        }
                        else if(dbupdateMeter.oad.value == 0x40010200)
                        {
                            taskmng_local_addr_read(LocalAddr);
                        }
                        else if(dbupdateMeter.oad.value == 0xF2090900)
                        {
                            //LoadHPLCChannel();
                        }
                        else if(dbupdateMeter.oad.value == 0xF2090B00)
                        {
                            taskmng_HPLC_across_area_flag_load();
                        }
                        else if(dbupdateMeter.oad.value == 0xF2091600)
                        {
                            taskmng_HPLC_node_reject_flag_load();
                        }
                         else if(dbupdateMeter.oad.value == 0x31060200)
                        {
                            taskmng_power_event_deal(gDBClientMain,dbupdateMeter.infonum);
                        }

#ifdef AREA_FUJIAN
                        else if(dbupdateMeter.oad.value == 0x6E030300)
                        {
                            g485ShareFlag = fujian_485_share_flag_get(gDBClientMain);
                        }

#endif

#ifdef AREA_HUNAN
                        else if(dbupdateMeter.oad.value == gTemperatureOad.value)
                        {
                            taskmng_sensor_param_get(gTemperatureOad, &gTemperaturePort, &gTemperatureAddr, 1);
                        }
                        else if(dbupdateMeter.oad.value == gSmokeOad.value)
                        {
                            taskmng_sensor_param_get(gSmokeOad, &gSmokePort, &gSmokeAddr, 1);
                        }
                        else if(dbupdateMeter.oad.value == gWaterOad.value)
                        {
                            taskmng_sensor_param_get(gWaterOad, &gWaterPort, &gWaterAddr, 1);
                        }
                        else if(dbupdateMeter.oad.value == gTransTempOad.value)
                        {
                            taskmng_sensor_param_get(gTransTempOad, &gTransTempPort, gTransTempAddr, 6);
                        }
                        else if(dbupdateMeter.oad.value == 0XF2096300)
                        {
                            taskmng_HPLC_module_curve_flag_load();

                            if(taskmng_HPLC_module_curve_flag_get() == 1)
                            {
                                for(i = 0; i < PLC_NUM; i++)
                                {
                                    taskmng_curve_sync_flag_set(i, 1);
                                }
                            }
                        }
                        else if(dbupdateMeter.oad.value == 0XF2096500)
                        {
                            taskmng_module_curve_store_data_load();

                            for(i = 0; i < PLC_NUM; i++)
                            {
                                taskmng_curve_sync_flag_set(i, 1);
                            }
                        }

#endif

                        else
                            memset(&dbupdateMeter, 0x00, sizeof(DB_UPDATE_T));
                    }
                    break;

                default:
                    break;
            }

        }

        taskmng_meter_file_update_check(&dbupdateMeter);
        taskmng_meter_file_ergodic(&MeterFileAll);

#ifdef AREA_FUJIAN
        taskmng_grop_meter_file_update_check(&dbupdateMeter);
        taskmng_grop_meter_file_ergodic(&oop_grop_meter);
#endif

        //        #ifdef AREA_HUNAN
        //        PowerOffClosedMoudle();
        //        #endif
        //大概3秒检查一次线程状态
        if(maincnt > 300)
        {
            //TASK_FMT_TRACE(-1,UP_LOG, "定时检查线程状态！！！ \n");
            maincnt = 0;

            for(i = 0; i < AMR_THREAD_NUM; i++)
            {
                if(gAmrThreadFlag[i] == 0)
                {
                    TASK_FMT_DEBUG(-1, UP_LOG, "抄表线程端口 %d 出现问题!\n", i);
                }

                gAmrThreadFlag[i] = 0;
            }

            if(gTaskThreadFlag == 0)
            {
                TASK_FMT_DEBUG(-1, UP_LOG, "任务管理线程出现问题!\n", i);
            }

            gTaskThreadFlag = 0;

            if(gmqttThreadFlag == 0)
            {
                TASK_FMT_DEBUG(-1, UP_LOG, "mqtt线程出现问题!\n", i);
            }

            gmqttThreadFlag = 0;

            if(gTGThreadFlag == 0)
            {
                //TASK_FMT_DEBUG(-1,UP_LOG, "总加组线程出现问题!\n",i);
            }

            gTGThreadFlag = 0;
        }

        usleep(20);
        maincnt++;
    }

    pthread_join(MqttTheadID, NULL);

    pthread_join(TaskThreadID, NULL);
    pthread_rwlock_destroy(&TaskRWLock);
    pthread_mutex_destroy(&SeqLock);
    pthread_mutex_destroy(&EventReportNumLock);
    pthread_mutex_destroy(&NetTopologySeqLock);
    pthread_mutex_destroy(&gMqttSendLock);

#ifdef AREA_HUNAN
    pthread_join(LoRaThreadID, NULL);
#endif

    for(i = 0; i < AMR_THREAD_NUM; i++)
    {

        pthread_join(AmrThreadID[i], NULL);

        pthread_mutex_destroy(&LockRecvDown[i].QueueLock);
        pthread_mutex_destroy(&LockSendDown[i].QueueLock);
        pthread_mutex_destroy(&LockRecvUp[i].QueueLock);
        pthread_mutex_destroy(&LockSendUp[i].QueueLock);

    }

    exit(1);

    return 0;
}


void DefaultMeterSearch(uint8 pipe)
{
    static uint8 SearchMeter = 1;

    if(SearchMeter == 1)
    {
        SearchMeter = 0;

        if(gAmrThreadArg[pipe].IsNeedSearch == 0)
        {
            gAmrThreadArg[pipe].IsNeedSearch = 1;
            gAmrThreadArg[pipe].MeterSearch.SearchTime = 0x15;

            //ResetSearchMeterSeq();//重置搜表序号
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "上电搜表！！！\n");
        }
        else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前正在搜表中！！！\n");
        }
    }
}

/*******************************************************************************
* 函数名称: AmrThread
* 函数功能: 自动抄表线程
* 输入参数：arg                 指向参数区的指针
* 输出参数：无
* 内部处理：抄表任务调度
* 返 回 值: 无
*******************************************************************************/
void * AmrThread(void * arg)
{

    uint8 CommPipe;
    AmrThreadArg_T * pAmrThreadArg;
    uint8 ModuleStat;
    uint16 isfirst;                                 //是否首次插入模块
    uint16 recinfo = 0;                             // 表示第一次收到CC0模块的消息；
    pAmrThreadArg = (AmrThreadArg_T *)
    arg;
    CommPipe = pAmrThreadArg->CommPipe;
    uint32 sender;
    int RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN];
    char ThreadName[20];
    sprintf(ThreadName, "AmrThread-%d", CommPipe);
    prctl(PR_SET_NAME, ThreadName);
    taskmng_amr_thread_queue_init(arg);             //线程队列初始化
    taskmng_past_time_deal_init(CommPipe);          //时间过处理初始化
    taskmng_all_state_init(CommPipe);               //状态字初始化
    taskmng_all_flag_init(CommPipe);                //标志初始化

    memset(pAmrThreadArg->PlcTaskQueue, 0x00, PLCTASK_MAXNUM * sizeof(PLCTASK_QUEUE));
    pAmrThreadArg->PlcTask_RunLink = NULL;

#ifdef AREA_JIANGXI

    /* 江西需求，通过终端和电表停上电来判断全台区的停上电 */
    MTR_POWER_ON_OFF_JX mtrPowerOnOff;
    memset(&mtrPowerOnOff, 0x00, sizeof(MTR_POWER_ON_OFF_JX));
#endif

    while(1)
    {
        gAmrThreadFlag[CommPipe] = 1;

        RecvLen = task_recv_msg(pAmrThreadArg->MsgSock, RecvBuf, MAX_MSG_LEN, &sender);

        //检测当前模块是否已经插入
        ModuleStat = taskmng_plc_module_check(CommPipe);
#if defined(PRODUCT_ZCU_3) || defined(PRODUCT_ZCU_1)
        //专三和专一线程空转
        sleep(10);
        continue;
#endif
        if(ModuleStat != 0)
        {
            isfirst = 0;
            memset(RecvBuf, 0x00, sizeof(RecvBuf));
            RecvLen = 0;
            taskmng_task_record_file_del_que_clear(CommPipe);
            usleep(200000);
            continue;
        }

        if(isfirst == 0)
        {
            isfirst = 1;
            memset(&pAmrThreadArg->TotalState, 0x00, sizeof(TOTAL_STATE_T));
            memset(&pAmrThreadArg->MeterSearch, 0x00, sizeof(SEARCH_METER_T));
            memset(&pAmrThreadArg->RelayTrans, 0x00, sizeof(RELAY_TRANS_T));
            memset(&pAmrThreadArg->EventReport, 0x00, sizeof(EVENT_REPORT_T));
            memset(pAmrThreadArg->PlcTaskQueue, 0x00, PLCTASK_MAXNUM * sizeof(PLCTASK_QUEUE));
            pAmrThreadArg->PlcTask_RunLink = NULL;
            pAmrThreadArg->TaskModifyFlag = TASK_CFG_UNINIT;
            pAmrThreadArg->IsNeedSearch = 0;
            pAmrThreadArg->BroadTiming.TimingType = 0;
            gHPLCRouterState[CommPipe] = 0;

#ifdef AREA_HUNAN
            taskmng_curve_sync_flag_set(CommPipe, 1);
#endif

#ifdef AREA_JIANGSU
            nodecount = 0;
            memset((uint8 *) &Collector, 0x00, sizeof(Collector));
#endif



        }

        if(RecvLen > 0)
        {

            TASK_MSG_t * msg = (TASK_MSG_t *)
            RecvBuf;
            TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到IPC消息 msg_id %04x RecvLen %d msg->msglen %d \n", msg->msg_id, RecvLen, msg->msglen);

            TASK_BUF_DEBUG(CommPipe, RELAY_LOG, msg->msg, msg->msglen);

            switch(msg->msg_id)
            {
                //            case MSG_SEARCH_METER:
                //            {
                //                TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到搜表上报消息\n");
                //            #ifdef AREA_ZHEJIANG
                //                taskmng_meter_search_result_write_area(CommPipe,msg->msg,msg->msglen);
                //            #else
                //                taskmng_meter_search_result_write(CommPipe,msg->msg,msg->msglen);
                //            #endif
                //            }
                //                break;
                case MSG_EVENT_REPORT:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到电表事件上报\n");
                        taskmng_event_report_amr_fun(CommPipe, msg->msg, msg->msglen);
                        //适配跑分台体 ccoRouter在收到上报的情况下会直接结束从节点监控 收到电表事件上报时记个时间 3秒内没有新的事件上报才开始执行任务
                        taskmng_meter_event_report_time_set();
                    }
                    break;

                case MSG_HPLC_INFO:
                    {
                        if(recinfo == 0)
                        {
                            recinfo = 1;
                        }

                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到HPLC查询信息上报\n");
                        taskmng_HPLC_info_save(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_NODE_REJECT:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到节点拒绝信息上报\n");
                        taskmng_node_reject_info_save(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_POWER_ON_OFF_COL:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到采集器停上电事件上报\n");
                        taskmng_power_on_off_deal_col(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_POWER_ON_OFF_MET:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到电表停上电事件上报\n");

#ifdef AREA_JIANGXI

                        if(mtrPowerOnOff.tmnPowerOnOffFlag == TMN_POWER_CHANGE_JX)
                        {
                            taskmng_power_on_off_deal_met_jx(CommPipe, msg->msg, msg->msglen, &mtrPowerOnOff);
                        }
                        else
                        {
#endif

                            taskmng_power_on_off_deal_met(CommPipe, msg->msg, msg->msglen);

#ifdef AREA_JIANGXI
                        }

#endif
                    }
                    break;

#ifdef AREA_JIANGXI
                case MSG_TMN_POWER_ON_OFF:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到终端停上电事件\n");
                        memset(&mtrPowerOnOff, 0x00, sizeof(MTR_POWER_ON_OFF_JX));
                        taskmng_power_on_off_deal_tmn(pAmrThreadArg, msg->msg, msg->msglen, &mtrPowerOnOff);
                    }
                    break;

#endif

                //            case MSG_RECORD_FILE_DEL:
                //            {
                //                uint16 taskid;
                //                memcpy(&taskid,msg->msg,2);
                //                TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到任务 %d 的变更消息\n",taskid);
                //                taskmng_record_file_delete_pipe(CommPipe,taskid);
                //            }
                //                break;
                //            case MSG_HPLC_CHANNEL_CHANGE:
                //            {
                //                uint8 channel = msg->msg[0];
                //                TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到频点设置的变更消息 频点为 %d \n",channel);
                //                SetHPLCChannel(CommPipe,channel);
                //            }
                //                break;
                case MSG_HPLC_ACROSSAREA_CHANGE:
                    {
                        uint8 flag = msg->msg[0];
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到台区识别开关的变更消息 开关为 %d \n", flag);
                        taskmng_HPLC_across_area_flag_set(CommPipe, flag);
                        taskmng_HPLC_off_grid_set(CommPipe, flag);
                    }
                    break;

                case MSG_HPLC_NODEREJECT_CHANGE:
                    {
                        uint8 flag = msg->msg[0];
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到拒绝节点上报开关的变更消息 开关为 %d \n", flag);
                        taskmng_HPLC_node_reject_set(CommPipe, flag);
                    }
                    break;

                case MSG_METERFILE_CLEAR:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到档案清空消息 \n");
                        taskmng_cco_meter_file_clear(CommPipe);
                        taskmng_event_report_clear(CommPipe);
                    }
                    break;

                case MSG_CCO_STA_INFO:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到从节点版本消息 \n");
                        taskmng_cco_sta_info_write(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_MODULE_INFO:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到抄表模块信息 \n");
                        taskmng_msg_0061_unpack(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_FILE_TRANSFER:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到文件代发信息 \n");
                        taskmng_file_transfer_msg_amr(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_HPLC_UPDATE:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "收到载波模块升级文件 \n");
                        taskmng_hplc_update_msg_amr(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                default:
                    break;
            }
        }
        //如果是档案清空消息 放上面会挡住到档案加载完才会执行
        if(MeterFileAll.FileState != TASK_CFG_INITED)
        {
            sleep(1);
            continue;
        }

#ifdef AREA_JIANGSU

        if(recinfo == 1)
        {
            recinfo = 2;
            DefaultMeterSearch(CommPipe);
        }

#endif


        taskmng_meter_search_report_check(CommPipe);
        taskmng_data_init_check(CommPipe);
        taskmng_time_manage(CommPipe);              //时间管理函数

        taskmng_task_run_ergodic(CommPipe);         //一秒钟运行一次有可能会出现任务结束时间没判断到的情况

        taskmng_plc_task_creat(CommPipe);           //任务的产生判断

        taskmng_plc_task_run(CommPipe);             //任务的执行处理

        /* 判断下有没有没上报的停上电事件 */
                /* 判断下有没有没上报的停上电事件 */
        taskmng_meter_power_cache_check(CommPipe);
        taskmng_meter_power_report_check(CommPipe);
        /*没有任务的时候多sleep  */
        if(gAmrThreadArg[CommPipe].PlcTask_RunLink==NULL)
            usleep(100000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
#ifdef AREA_JIANGXI
        PowerOnOffEventReport_Jx(CommPipe, &mtrPowerOnOff);
#endif
        /* 加上sleep 降低cpu占用 */
        //usleep(3000);
    }

    pthread_exit(NULL);
}

/*******************************************************************************
* 函数名称: AmrThread_485
* 函数功能: 自动抄表线程
* 输入参数：arg                 指向参数区的指针
* 输出参数：无
* 内部处理：抄表任务调度
* 返 回 值: 无
*******************************************************************************/
void * AmrThread_485(void * arg)
{
    AmrThreadArg_T * Cb_485_param;
    uint8 CommPipe;
    Cb_485_param = (AmrThreadArg_T *)
    arg;
    CommPipe = Cb_485_param->CommPipe;

    //TASK_FMT_DEBUG(CommPipe,REC_LOG,"AmrThread_485 is BEGIN\n");
    char ThreadName[20];
    sprintf(ThreadName, "AmrThread_485-%d", CommPipe);
    prctl(PR_SET_NAME, ThreadName);
    taskmng_amr_thread_queue_init(arg);             //线程队列初始化
    taskmng_past_time_deal_init(CommPipe);          //时间过处理初始化

    taskmng_all_state_init(CommPipe);               //状态字初始化
    taskmng_all_flag_init(CommPipe);                //标志初始化
    pTaskRunInfoinit(CommPipe);                     //运行时的方案测量点序号等信息初始化

    cbtasknew_TaskCb(CommPipe);
    pthread_exit(NULL);
}

/*******************************************************************************
* 函数名称: UpThread
* 函数功能: 与上行应用进程进行通信
* 输入参数：arg                 指向参数区的指针
* 输出参数：无
* 内部处理：
* 返 回 值: 无
*******************************************************************************/
void * UpThread(void * arg)
{
    /*
        prctl(PR_SET_NAME,"UpThread");
        //uint8               pipe = 0;
        uint8               i;
        LockQueue_T         *pLockRecv;                     //载波锁接收缓冲区指针
        LockQueue_T         *pLockSend;                     //载波锁发送缓冲区指针
        uint8               UDPState = MSG_INIT;
        HMSG                UpHandle;
        //uint8               RecvBuf[MAX_MSG_LEN]={0};
        int                 RecvLen=0;
        uint8               SendBuf[MAX_MSG_LEN]={0};
        int                 SendLen =0;

        int ret;
        MSG_INFO_T* MsgRecv=NULL;
        PRAR_UDP_T paraUdp;
        paraUdp.appName = APPNAME;
        paraUdp.srcIP = inet_addr(LOCAL_IP);
        paraUdp.srcIP = ntohl(paraUdp.srcIP);
        paraUdp.srcPort = UP_PORT; //自己的端口
        paraUdp.bBlock = 0;

        while(1)
        {
            gUpThreadFlag = 1;
            switch(UDPState)
            {
            case MSG_INIT:
                UpHandle  = msg_handle_create( paraUdp);
                if(UpHandle==NULL)
                {
                    sleep(1);
                }else
                {
                    UDPState = MSG_RECV;
                }
                break;
            case MSG_RECV:

                //调用接收消息接口
                ret= msg_udp_read(UpHandle, &MsgRecv);
                if(ret==0)
                {
                    RecvLen = sizeof(MSG_INFO_T) +MsgRecv->MsgLen;
                    uint8 pipe ;
                    TASK_FMT_TRACE(-1,UP_LOG,"上行线程接收 IID %04x IOP %04x msglen %d msgdata \n",MsgRecv->IID,
        MsgRecv->IOP,MsgRecv->MsgLen);

                    TASK_BUF_TRACE(-1,UP_LOG,MsgRecv->MsgData, MsgRecv->MsgLen);

                    if(MsgRecv->IID == 0x1003)
                    {
                        switch(MsgRecv->IOP)
                        {
                            case 0x0010://请求代理读取若干个服务器的若干个对象属性
                            {
                                TASK_FMT_TRACE(-1,UP_LOG,"收到0x0010  \n");
                                uint8 i;
                                for(i=0;i<AMR_THREAD_NUM;i++)
                                {
                                    pLockSend = gAmrThreadArg[i].UpArg->SendBuf;

                                    taskmng_plc_queue_write ( pLockSend, (uint8*)MsgRecv, RecvLen );
                                }
                            }
                                break;
                            case 0x0011:
                            {
                                TASK_FMT_TRACE(-1,UP_LOG,"收到0x0011  \n");
                                //TSA为octet-string 第一个字节为长度 第二个字节为电表地址长度减1
                                OOP_TSA_T tsa; //电表地址可能为广播地址 或者单个电表地址
                                memcpy(&tsa.af,MsgRecv->MsgData+1,1);
                                memcpy(tsa.add,MsgRecv->MsgData+2,tsa.len+1);
                                pipe = taskmng_pipe_get_from_tsa(tsa);
                                if(pipe!=0xff)
                                {
                                    pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;
                                    //写共享的接收缓冲区
                                    taskmng_plc_queue_write ( pLockSend, (uint8*)MsgRecv, RecvLen );
                                }
                            }
                                break;
                            case 0x0016://请求代理操作透明转发命令
                            {
                                OOP_OAD_U oad ;//转发端口对象描述符
                                memcpy_r((uint8 *) &oad, MsgRecv->MsgData , sizeof(OOP_OAD_U));
                                TASK_FMT_TRACE(-1,UP_LOG,"收到0x1016 转发端口为 0x%08x \n",oad.value);
                                pipe = taskmng_mtr_pipe_get(&oad);
                                if(pipe!=0xff)
                                {
                                    pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;
                                    //写共享的接收缓冲区
                                    taskmng_plc_queue_write ( pLockSend, (uint8*)MsgRecv, RecvLen );

                                }
                            }
                                break;
                            case 0x0020://载波/微功率无线接口透明转发
                            {
                                OOP_TSA_T tsa; //电表地址可能为广播地址 或者单个电表地址
                                memcpy(&tsa.af,MsgRecv->MsgData+1,1);
                                memcpy_r(tsa.add,MsgRecv->MsgData+2,tsa.len+1);
                                TASK_FMT_TRACE(-1,UP_LOG,"收到0x1020 tsa.af %02x \n",tsa.af);
                                //广播地址 每个端口都需要
                                if(tsa.type==3)
                                {
                                    uint8 i;
                                    for(i=0;i<PLC_NUM;i++)
                                    {
                                        pLockSend = gAmrThreadArg[i].UpArg->SendBuf;

                                        taskmng_plc_queue_write ( pLockSend, (uint8*)MsgRecv, RecvLen );
                                    }
                                }
                                else
                                {
                                    pipe = taskmng_pipe_get_from_tsa(tsa);
                                    TASK_FMT_TRACE(-1,UP_LOG,"单地址 pipe %d \n",pipe);
                                    if(pipe!=0xff)
                                    {
                                        pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;
                                        //写共享的接收缓冲区
                                        taskmng_plc_queue_write ( pLockSend, (uint8*)MsgRecv, RecvLen );
                                    }
                                }
                            }
                                break;
                            case 0x0021: //搜表
                            {
                                uint16  SearchTime;
                                uint8   i;
                                memcpy_r((uint8*)&SearchTime,MsgRecv->MsgData,2);
                                for(i=0;i<PLC_NUM;i++)
                                {
                                    if(gAmrThreadArg[pipe].IsNeedSearch==0)
                                    {
                                        gAmrThreadArg[pipe].IsNeedSearch=1;
                                        gAmrThreadArg[pipe].MeterSearch.SearchTime = SearchTime;
                                    }
                                }
                                SearchMeterUpMsg(MsgRecv);
                            }
                                break;
                            default:
                                break;

                        }

                    }
                    free(MsgRecv);
                }

                UDPState = MSG_SEND;

                break;
            case MSG_SEND:
                for(i=0;i<AMR_THREAD_NUM;i++)
                {
                    pLockRecv = gAmrThreadArg[i].UpArg->RecvBuf;
                    SendLen = taskmng_plc_queue_read(pLockRecv, SendBuf);
                    if(SendLen>0)
                    {
                        //调用消息接口发送消息
                        MSG_INFO_T* msg  = (MSG_INFO_T*)SendBuf;
                        TASK_FMT_TRACE(-1,UP_LOG,"上行发送消息 IID %04x IOP %04x msglen %d msgdata \n",msg->IID,
        msg->IOP,msg->MsgLen);

                        TASK_BUF_TRACE(-1,UP_LOG,msg->MsgData, msg->MsgLen);

                        msg_udp_write(UpHandle,(MSG_INFO_T*)SendBuf);
                        memset(SendBuf,0x00,sizeof(SendBuf));
                    }
                }
                UDPState = MSG_RECV;
                break;
            case MSG_CLOSE:
                msg_handle_close(UpHandle);
                UDPState = MSG_INIT;
                break;
            default:
                UDPState = MSG_INIT;
                break;
            }
            usleep(20000);
        }*/
    pthread_exit(NULL);

}

/*******************************************************************************
* 函数名称: TaskThread
* 函数功能: 任务管理线程
* 输入参数：arg                 指向参数区的指针
* 输出参数：无
* 内部处理：
* 返 回 值: 无
*******************************************************************************/
void * TaskThread(void * arg)
{
    //uint32 sender;
    //int RecvLen;
    //uint8 RecvBuf[MAX_MSG_LEN] = {0};
    prctl(PR_SET_NAME, "TaskThread");
    taskmng_cur_bin_time_get(&gCurBinTimeTask);
    memcpy(&gPreBinTimeTask, &gCurBinTimeTask, sizeof(DateTime_T));


    gDBClientTask = db_client_create(DB_AGENT_CTS, DB_CLIENT_TASK);


    memset(&dbupdateTask, 0x00, sizeof(DB_UPDATE_T));

    while(1)
    {
        gTaskThreadFlag = 1;

        taskmng_task_db_update_check(&dbupdateTask);

        taskmng_scheme_task_ergodic(&SchemeTaskCfgList, SchemeTaskRunQueue);
        taskmng_time_manage_task();
        usleep(20000);
    }

    db_client_close(gDBClientTask);


}

/*******************************************************************************
* 函数名称:    taskmng_amr_thread_queue_init
* 函数功能:    线程队列初始化
* 输入参数:    无
* 输出参数:    无
* 返 回 值:    无
*******************************************************************************/
void taskmng_amr_thread_queue_init(void * arg)
{
    uint8 pipe;

    AmrThreadArg_T * AmrThreadArg;                  //抄表线程参数
    AmrThreadArg = (AmrThreadArg_T *)
    arg;
    pipe = AmrThreadArg->CommPipe;

    pAmrRecvDown[pipe] = AmrThreadArg->DownArg->SendBuf;
    pAmrSendDown[pipe] = AmrThreadArg->DownArg->RecvBuf;
    pAmrRecvDown_Cb[pipe] = AmrThreadArg->DownArg->CbSendBuf;
    pAmrRecvUp[pipe] = AmrThreadArg->UpArg->SendBuf;
    pAmrSendUp[pipe] = AmrThreadArg->UpArg->RecvBuf;

}

void * MqttThread(void * arg)
{
    prctl(PR_SET_NAME, "MqttThread");
    uint8 MqttStatus = MSG_INIT;
    static time_t LastLoginTime;
    int RecvLen;
    uint8 RecvBuf[8192] =
    {
        0
    };
    MESSAGE_INFO_T * MsgRecv = NULL;
    static uint8 LoginFlag = 0;
    LockQueue_T * pLockRecv;
    uint8 SendBuf[MAX_MSG_LEN] =
    {
        0
    };
    int SendLen = 0;
    uint8 i;

    uint32 MSGLen;
    uint8 * MSGBuf = NULL;
    LockQueue_T * pLockSend;
    LockQueue_T * pLockSend_Cb;
    int ret;
    uint8 pipe;

    // char topic[500] = {0};
    while(1)
    {
        gmqttThreadFlag = 1;

        switch(MqttStatus)
        {
            case MSG_INIT:
                {

                    if(gMqttSock <= 0)
                    {
                        gMqttSock = task_msg_init(IPC_MQTT);

                        if(gMqttSock > 0)
                        {
                            MqttStatus = MSG_LOGIN;
                            break;
                        }

                        TASK_FMT_DEBUG(-1, MQTT_LOG, "mqtt task_msg_init error! ");
                        sleep(1);
                        break;
                    }

                    MqttStatus = MSG_LOGIN;
                }
                break;

            case MSG_LOGIN:
                {
                    if(LoginFlag == 1)
                    {
                        MqttStatus = MSG_RECV;
                        break;
                    }

                    printf("登录到smios！！！\n");
                    login_to_smiOS();
                    LastLoginTime = time(NULL);
                    MqttStatus = MSG_LOGIN_WAIT;
                }
                break;

            case MSG_LOGIN_WAIT:
                {
                    if(abs(time(NULL) -LastLoginTime) > 5)
                        MqttStatus = MSG_LOGIN;
                    else
                        MqttStatus = MSG_RECV;
                }
                break;

            case MSG_RECV:
                {
                    memset(RecvBuf, 0x00, sizeof(RecvBuf));
                    RecvLen = 0;

                    //RecvLen = mqtt_topic_read(topic,&qos,&mid,RecvBuf);
                    RecvLen = mqtt_read(RecvBuf);

                    if(RecvLen > 0)
                    {
                        ret = appmsg_split(RecvBuf, RecvLen, &MsgRecv);

                        if(ret < 0)
                            break;

                        //增加防护 以防出现死循环
                        if(strcmp(MsgRecv->souraddr, APPNAME) == 0)
                            break;

                        if((MsgRecv->IID != 0x0005 || MsgRecv->IOP != 0x0001) && MsgRecv->IID != 0x0000)
                        {
                            TASK_FMT_TRACE(-1, MQTT_LOG, "mqttrecv source %s iid %04x iop %04x MsgLabel %d MsgIndex %d  \n",
                                MsgRecv->souraddr, MsgRecv->IID, MsgRecv->IOP, MsgRecv->label, MsgRecv->index);
                            TASK_BUF_TRACE(-1, MQTT_LOG, RecvBuf, RecvLen);
                        }

                        RecvLen = sizeof(MESSAGE_INFO_T) +MsgRecv->msglen;

                        if(MsgRecv->IID == 0x0000)
                        {
                            switch(MsgRecv->IOP)
                            {
                                case 0x0010: //心跳
                                    {
                                        heart_beat(MsgRecv->index);
                                    }
                                    break;

#ifdef AREA_FUJIAN
                                case 0x0012: //透明传输目前是与无线拨号的交互（通信模块任务）
                                    {
                                        pipe = MsgRecv->label; //发送时label填端口号
                                        pLockSend = gAmrThreadArg[pipe].DownArg->SendBuf;

                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //清空接收缓冲区
                                            //taskmng_comm_recv_buff_clear(pipe);
                                        }
                                    }
                                    break;

#endif

                                case 0x0014: //错误信息
                                    {
                                        MsgErrorFunc(MsgRecv, RecvLen);

                                    }
                                    break;

                                case 0x0013: //版本信息
                                    {
                                        Appversion_Response(MsgRecv);
                                    }
                                    break;

                                case 0x0016: //APPID
                                    {
                                        Appid_Response(MsgRecv);
                                    }
                                    break;

                                default:
                                    break;
                            }
                        }
                        else if(MsgRecv->IID == 0x0001)
                        {
                            switch(MsgRecv->IOP)
                            {
                                case 0x0010: //注册响应
                                    {
                                        printf("收到注册响应!!!! \n");
                                        LoginFlag = 1;
                                    }
                                    break;

                                case 0x0013:
                                    {
                                        taskmng_plc_queue_write(APPArg.RecvBuf, (uint8 *) MsgRecv, RecvLen);
                                    }
                                    break;

                                default:
                                    break;
                            }
                        }

#ifdef PRODUCT_ECU
                        else if(MsgRecv->IID == 0x0004)
                        {
                            switch(MsgRecv->IOP)
                            {
                                case 0x0001:
                                    {
                                        ModuleEventFun(MsgRecv->playload, MsgRecv->msglen);

                                    }
                                    break;

                                case 0x0016:
                                    {
                                        taskmng_plc_queue_write(APPArg.RecvBuf, (uint8 *) MsgRecv, RecvLen);
                                    }
                                    break;

                                default:
                                    break;
                            }

                        }

#endif

                        else if(MsgRecv->IID == 0x0005) //数据中心
                        {
                            switch(MsgRecv->IOP)
                            {
                                case 0x0001: //数据更新事件
                                    {
                                        DBUpdateFun(MsgRecv);
                                    }
                                    break;

                                case 0x0002: //数据初始化事件
                                    {
                                        DBInitFun(MsgRecv->playload, MsgRecv->msglen);
                                    }
                                    break;

                                case 0x0010:
                                case 0x0011:
                                case 0x0012:
                                case 0x0013:
                                case 0x0014:
                                case 0x0015:
                                case 0x0016:
                                case 0x0020:
                                case 0x0021:
                                case 0x0030:
                                case 0x0031:
                                    {
                                        ret = db_agent_msg_entry(gDBAgent, MsgRecv);

                                        if(ret != 0)
                                        {
                                            TASK_FMT_DEBUG(-1, MQTT_LOG, "db_agent_msg_entry error ret  0x%04x \n", ret);
                                        }
                                    }
                                    break;

                                default:
                                    break;
                            }
                        }
                        else if(MsgRecv->IID == 0x1003)
                        {
                            switch(MsgRecv->IOP)
                            {
                                case 0x0010: //透明转发命令
                                    {
                                        OOP_OAD_U oad; //转发端口对象描述符
                                        memcpy_r((uint8 *) &oad, MsgRecv->playload, sizeof(OOP_OAD_U));
                                        TASK_FMT_DEBUG(-1, MQTT_LOG, "收到0x0010 转发端口为 0x%08x \n", oad.value);
                                        pipe = taskmng_mtr_pipe_get(&oad);

                                        if(pipe >= 0 && pipe < 10)
                                        {
                                            pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;

                                            //写共享的接收缓冲区
                                            taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        }
                                    }
                                    break;

                                case 0x0011: //代理
                                    {
                                        TASK_FMT_DEBUG(-1, MQTT_LOG, "收到0x0011 代理 \n");
                                        uint8 i;
#ifdef AREA_FUJIAN
                                        for(i = 0; i < AMR_THREAD_NUM - 1; i++)
#else
                                        for(i = 0; i < AMR_THREAD_NUM; i++)
#endif
                                        {
                                            pLockSend = gAmrThreadArg[i].UpArg->SendBuf;

                                            taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);
                                        }

                                        //新增交采的代理透传
                                        taskmng_plc_queue_write(TGThreadArg.pBufArg->SendBuf, (uint8 *) MsgRecv, RecvLen);
                                    }
                                    break;

                                case 0x0012: //搜表
                                    {
                                        uint16 SearchTime;

                                        uint8 pipe;
                                        OOP_OAD_U pipeoad;
                                        memcpy_r(&pipeoad.value,MsgRecv->playload, 4);
                                        pipe = taskmng_mtr_pipe_get(&pipeoad);
                                        if(pipe != 0xff)
                                        {
                                            memcpy_r((uint8 *) &SearchTime, MsgRecv->playload+4, 2);
                                            if(gAmrThreadArg[pipe].IsNeedSearch == 0)
                                            {
                                                gAmrThreadArg[pipe].IsNeedSearch = 1;
                                                gAmrThreadArg[pipe].MeterSearch.SearchTime = SearchTime;

                                                //ResetSearchMeterSeq();//重置搜表序号
                                            }
                                            else
                                            {
                                                TASK_FMT_DEBUG(pipe, RELAY_LOG, "当前正在搜表中！！！\n");
                                            }
                                            up_msg_response(MsgRecv, 1);
                                        }else
                                        {
                                            up_msg_response(MsgRecv, 0);
                                        }

                                    }
                                    break;

                                case 0x0013: //当日抄读状态
                                    {
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "收到0x0013 查询当日抄读状态 \n");
                                        uint8 Addr[10] =
                                        {
                                            0
                                        };
                                        uint8 len = MsgRecv->playload[0];

                                        if(len == 6)
                                        {
                                            memcpy_r(Addr, MsgRecv->playload + 1, 6);
                                            pipe = taskmng_pipe_get_from_addr(Addr, LOGIC_ID_YC);

                                            if(pipe >= 0 && pipe < 10)
                                            {
                                                //pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;

                                                //写共享的接收缓冲区
                                               // taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);
                                                taskmng_amr_status_get_mqtt(pipe,Addr,MsgRecv);

                                                break;
                                            }
                                        }

                                        ErrorUpMsg(MsgRecv); //收到内容不正确 回复错误码
                                    }
                                    break;

                                case 0x0020:
                                    {
                                        ret = taskmng_file_transfer_msg_fun(MsgRecv->playload, MsgRecv->msglen);

                                        if(ret == 0)
                                        {
                                            up_msg_response(MsgRecv, 1);
                                        }
                                        else
                                            up_msg_response(MsgRecv, 0);
                                    }
                                    break;

                                case 0x0021: //查询设备升级结果
                                    {
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "收到0x0021 查询设备升级结果 \n");
                                        taskmng_file_transfer_result_check(MsgRecv);

                                    }
                                    break;

#ifdef AREA_FUJIAN
                                case 0x0030: //更新透明任务方案采集状态
                                    {
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "收到0x0030 更新透明任务方案采集状态 \n");
                                        fujian_collection_send_recv_update(MsgRecv);

                                    }
                                    break;

                                case 0x0031: //扩充的上电消息通知
                                    {
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "收到0x0031 上电通知 \n");
                                        fujian_power_on_notify(MsgRecv);

                                    }
                                    break;

#endif

                                default:
                                    break;
                            }

                        }
                        else if(MsgRecv->IID == 0x0003)
                        {

                            switch(MsgRecv->IOP)
                            {
                                case 0x0031: //抄表回复数据
#ifdef AREA_HUNAN
                                case 0x0071:
#endif

#ifdef AREA_FUJIAN
                                case 0x0104:
                                case 0x0105:
                                case 0x0102: //上报从节点信息
#endif

                                    {
                                        pipe = MsgRecv->label; //发送时label填端口号
                                        pipe = 0;
                                        pLockSend_Cb = gAmrThreadArg[pipe].DownArg->CbSendBuf;
                                        ret = taskmng_plc_queue_write(pLockSend_Cb, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //清空接收缓冲区
                                            ClearCommCbRecvBuff(pipe);
                                        }

                                        break;
                                    }

                                case 0x003E:
#ifdef AREA_HUNAN
                                case 0x0070:
#endif

                                    break;

                                case 0x0001: //主动上报事件
                                    {
                                        EventReportMqtt(MsgRecv->playload, MsgRecv->msglen);
                                        break;
                                    }

#ifdef AREA_FUJIAN

                                    //福建上报事件数据iop改为0003
                                    //case 0x0103: //上报事件数据

                                case 0x0003:
                                    {
                                        fujian_event_report_mqtt(MsgRecv->playload, MsgRecv->msglen);

                                        //up_msg_response(MsgRecv,1);
                                        break;
                                    }

#endif
                                case 0x0034:
                                case 0x0035:
                                    break; //启动搜表和停止搜表回复确认帧

                                case 0x0036: //上报搜表数据 直接存数据中心
                                    {

                                        SearchMeterUpFun(MsgRecv->playload, MsgRecv->msglen);

                                    }
                                    break;

                                case 0x0033:
                                case 0x0038:
                                case 0x003B:
                                case 0x0040:
                                case 0x0041:
                                case 0x0042:
                                case 0x0043:
                                case 0x0050:
#ifdef AREA_FUJIAN
                                case 0x0082: //查询本地模块参数
                                case 0x0084: //查询节点信息
                                case 0x0088: //模块地址设置
                                case 0x0092: //启动从节点注册
                                case 0x0094: //启动预告任务执行
                                case 0x0095: //预告抄读对象
                                case 0x0100: //数据转发国网
                                case 0x0101: //数据透传 福建
                                case 0x0108: //查询抄读方式
                                case 0x0090: //启动获取广播延时
                                case 0x0091: //广播
                                case 0x0106: //上报广播延时
                                case 0x0107: //上报广播完成
#endif

                                    {
                                        pipe = 0;
                                        pLockSend = gAmrThreadArg[pipe].DownArg->SendBuf;

                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //清空接收缓冲区
                                            taskmng_comm_recv_buff_clear(pipe);
                                        }
                                    }
                                    break;

                                case 0x0051:
                                    {
                                        HPLCInfoUpFun(MsgRecv->playload, MsgRecv->msglen);
                                    }
                                    break;

                                case 0x0061:
                                    {

                                        pipe = 0;
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "收到0061抄表模块信息 pipe %d \n", pipe);
                                        task_send_msg(gMqttSock, MSG_MODULE_INFO,MsgRecv->playload, MsgRecv->msglen, IPC_AMR + pipe);
                                    }
                                    break;

                                case 0xFF01:
                                    {
                                        taskmng_cco_sta_info_fun(MsgRecv->playload, MsgRecv->msglen);
                                    }
                                    break;

                                default:
                                    {
                                        pipe = 0; //发送时label填端口号


                                        pLockSend = gAmrThreadArg[pipe].DownArg->SendBuf;
                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //清空接收缓冲区
                                            taskmng_comm_recv_buff_clear(pipe);
                                        }

                                        break;
                                    }
                            }
                        }
                        else if(MsgRecv->IID == 0x0009)
                        {
                            switch(MsgRecv->IOP)
                            {
                                case 0x0002:
                                    {
                                        OOP_OAD_U portoad;
                                        memcpy_r(&portoad, MsgRecv->playload, 4);
                                        uint8 port = taskmng_mtr_pipe_get(&portoad);

                                        if(port ==0xFF)
                                            break;


                                        pLockSend = gAmrThreadArg[port].DownArg->SendBuf;
                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //清空接收缓冲区
                                            taskmng_comm_recv_buff_clear(port);
                                        }
                                    }
                                    break;

                                case 0x0013: //串口回复 bool
                                    {

                                    }
                                    break;

                                default:
                                    break;
                            }
                        }
                        else if(MsgRecv->IID == 0x000A)
                        {
                            switch(MsgRecv->IOP)
                            {
                                case 0x0001:
                                    {
                                        //直接放蓝牙线程缓冲区
                                        pLockSend = gAmrThreadArg[1].DownArg->SendBuf;
                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //清空接收缓冲区
                                            taskmng_comm_recv_buff_clear(1);
                                        }
                                    }
                                    break;

                                case 0x0014:
                                    {
                                        taskmng_plc_queue_write(APPArg.RecvBuf, (uint8 *) MsgRecv, RecvLen);
                                    }
                                    break;

                                default:
                                    break;
                            }
                        }
#ifdef AREA_FUJIAN
                        //接收遥信变位事件 判断触发任务
                        else if(MsgRecv->IID == 0x0008)
                        {
                            if(MsgRecv->IOP == 0X0001)
                            {
                                taskmng_trigger_task_yxchange_check(MsgRecv->playload, MsgRecv->msglen);
                            }
                        }

#endif

                        free(MsgRecv);
                        usleep(2000);
                    }

                    else
                    {
                        usleep(3000);
                    }

                    if(LoginFlag == 1)
                        MqttStatus = MSG_SEND;
                    else
                        MqttStatus = MSG_LOGIN_WAIT;
                }
                break;

            case MSG_SEND:
                {
                    SendLen = taskmng_plc_queue_read(APPArg.SendBuf, SendBuf);

                    if(SendLen > 0)
                    {
                        //调用消息接口发送消息
                        MESSAGE_INFO_T * msg = (MESSAGE_INFO_T *)
                        SendBuf;
                        char topic[500] =
                        {
                            0
                        };
                        sprintf(topic, "%s/%s", msg->souraddr, msg->destaddr);
                        TASK_FMT_TRACE(-1, MQTT_LOG, "Sendupmsg topic %s IID %04x IOP %04x msglen %d msgdata \n", topic, msg->IID, msg->IOP, msg->msglen);

                        //TASK_BUF_TRACE(-1,MQTT_LOG,msg->MsgData, msg->MsgLen);
                        //组消息
                        MSGLen = appmsg_package(msg, &MSGBuf);
                        TASK_BUF_TRACE(-1, MQTT_LOG, MSGBuf, MSGLen);

                        //MSGLen = msg_mqtt_package_little(msg,MAX_MSG_LEN,MSGBuf);
                        mqtt_topic_write(MSGBuf, MSGLen, topic);
                        memset(SendBuf, 0x00, sizeof(SendBuf));
                        free(MSGBuf);
                        MSGBuf = NULL;

                    }

                    SendLen = taskmng_plc_queue_read(TGThreadArg.pBufArg->RecvBuf, SendBuf);

                    if(SendLen > 0)
                    {
                        //调用消息接口发送消息
                        MESSAGE_INFO_T * msg = (MESSAGE_INFO_T *)
                        SendBuf;
                        char topic[500] =
                        {
                            0
                        };
                        sprintf(topic, "%s/%s", msg->souraddr, msg->destaddr);

                        TASK_FMT_TRACE(-1, MQTT_LOG, "透传交采上行 topic %s IID %04x IOP %04x msglen %d msgdata \n", topic, msg->IID, msg->IOP, msg->msglen);

                        //TASK_BUF_TRACE(-1,MQTT_LOG,msg->MsgData, msg->MsgLen);
                        //组消息
                        MSGLen = appmsg_package(msg, &MSGBuf);
                        TASK_BUF_TRACE(-1, MQTT_LOG, MSGBuf, MSGLen);

                        //MSGLen = msg_mqtt_package_little(msg,MAX_MSG_LEN,MSGBuf);
                        mqtt_topic_write(MSGBuf, MSGLen, topic);
                        memset(SendBuf, 0x00, sizeof(SendBuf));
                        free(MSGBuf);
                        MSGBuf = NULL;

                    }

                    for(i = 0; i < AMR_THREAD_NUM; i++)
                    {
                        pLockRecv = gAmrThreadArg[i].UpArg->RecvBuf;
                        SendLen = taskmng_plc_queue_read(pLockRecv, SendBuf);

                        if(SendLen > 0)
                        {
                            //调用消息接口发送消息
                            MESSAGE_INFO_T * msg = (MESSAGE_INFO_T *)
                            SendBuf;
                            char topic[500] =
                            {
                                0
                            };
                            if(msg->IID == 0X1003 && msg->IOP == 0X0001)
                            {
                                sprintf(topic, "%s/Broadcast/1003/0001", msg->souraddr);
                            }else
                            {
                                sprintf(topic, "%s/%s", msg->souraddr, msg->destaddr);
                            }
                            TASK_FMT_TRACE(-1, MQTT_LOG, "Sendupmsg topic %s  IID %04x IOP %04x msglen %d msgdata \n", topic, msg->IID, msg->IOP, msg->msglen);

                            //组消息
                            MSGLen = appmsg_package(msg, &MSGBuf);
                            TASK_BUF_TRACE(-1, MQTT_LOG, MSGBuf, MSGLen);

                            //MSGLen = msg_mqtt_package_little(msg,MAX_MSG_LEN,MSGBuf);
                            mqtt_topic_write(MSGBuf, MSGLen, topic);
                            memset(SendBuf, 0x00, sizeof(SendBuf));
                            free(MSGBuf);
                            MSGBuf = NULL;

                        }

                        pLockRecv = gAmrThreadArg[i].DownArg->RecvBuf;
                        SendLen = taskmng_plc_queue_read(pLockRecv, SendBuf);

                        if(SendLen > 0)
                        {
                            //调用消息接口发送消息
                            MESSAGE_INFO_T * msg = (MESSAGE_INFO_T *)
                            SendBuf;
                            char topic[500] =
                            {
                                0
                            };
                            sprintf(topic, "%s/%s", msg->souraddr, msg->destaddr);

                            TASK_FMT_TRACE(-1, MQTT_LOG, "Senddownmsg topic %s MsgIndex %d IOP %04x lable %d msglen %d msgdata \n", topic, msg->index, msg->IOP, msg->label, msg->msglen);

                            //组消息
                            MSGLen = appmsg_package(msg, &MSGBuf);
                            TASK_BUF_TRACE(-1, MQTT_LOG, MSGBuf, MSGLen);

                            //MSGLen = msg_mqtt_package_little(msg,MAX_MSG_LEN,MSGBuf);
                            mqtt_topic_write(MSGBuf, MSGLen, topic);
                            memset(SendBuf, 0x00, sizeof(SendBuf));
                            free(MSGBuf);
                            MSGBuf = NULL;

                        }
                    }

                    MqttStatus = MSG_RECV;
                }
                break;

            default:
                MqttStatus = MSG_INIT;
                break;
        }
        //usleep(2000);
    }
}

/*******************************************************************************
* 函数名称: taskmng_init_file_load
* 函数功能: 加载配置文件
* 输入参数:
* 输出参数: 无
* 返 回 值:
*******************************************************************************/
void taskmng_init_file_load()
{
    char filename[256] =
    {
        0
    };
    dictionary * ini = NULL;
    char * tmp;
    char curpath[256] =
    {
        0
    };

    int i;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);

    if(cnt < 0 || cnt >= 256)
    {
        printf(" get err path!\n");

    }
    else
    {
        for(i = cnt; i >= 0; --i)
        {
            if(curpath[i] == '/')
            {
                curpath[i + 1] = '\0';
                break;
            }
        }
    }

    sprintf(filename, "%s%s", curpath, "taskManager.ini");
    diag_init(filename, TRUE);
    ini = iniparser_load(filename);
    printf("读取配置文件%s \n", filename);

    if(ini != NULL)
    {
        tmp = iniparser_getstring(ini, "mqtt:MQTTIP", "172.17.0.1:1883");
        printf("读取mqttip%s \n", tmp);
        strcpy(mqttIP, tmp);
        mqttport = iniparser_getint(ini, "mqtt:MQTTPORT", 1883);
        printf("读取mqttport %d \n", mqttport);
        tmp = iniparser_getstring(ini, "Log:COMM_LOG", "/data/app/taskManager/log/ctscomm.log");
        printf("CommLog %s \n", tmp);
        strcpy(CommLog, tmp);
        tmp = iniparser_getstring(ini, "Log:TASK_LOG", "/data/app/taskManager/log/ctstask.log");
        printf("TaskLog %s \n", tmp);
        strcpy(TaskLog, tmp);
        tmp = iniparser_getstring(ini, "Log:REC_LOG", "/data/app/taskManager/log/ctsrec.log");
        printf("RecLog %s \n", tmp);
        strcpy(RecLog, tmp);
        tmp = iniparser_getstring(ini, "Log:UP_LOG", "/data/app/taskManager/log/ctsup.log");
        printf("UpLog %s \n", tmp);
        strcpy(UpLog, tmp);
        tmp = iniparser_getstring(ini, "Log:RELAY_LOG", "/data/app/taskManager/log/ctsrelay.log");
        printf("RelayLog %s \n", tmp);
        strcpy(RelayLog, tmp);
        tmp = iniparser_getstring(ini, "Log:MQTT_LOG", "/data/app/taskManager/log/ctsmqtt.log");
        printf("MqttLog %s \n", tmp);
        strcpy(MqttLog, tmp);
        tmp = iniparser_getstring(ini, "Log:TG_LOG", "/data/app/taskManager/log/ctstg.log");
        printf("TgLog %s \n", tmp);
        strcpy(TgLog, tmp);
        tmp = iniparser_getstring(ini, "Dir:SOCKET_DIR", "/usr/local/extapps/taskManager/socket");
        printf("SocketDir %s \n", tmp);
        strcpy(SocketDir, tmp);
        tmp = iniparser_getstring(ini, "Dir:PRIVATE_DIR", "/usr/local/extapps/taskManager/private");
        printf("PrivateDir %s \n", tmp);
        strcpy(PrivateDir, tmp);

        //gSafeButton= iniparser_getint(ini,"SAFE_MODE:SAFE_BUTTON",0);
        //TASK_FMT_DEBUG(-1,UP_LOG,"gSafeButton %d  \n",gSafeButton);
        iniparser_freedict(ini);
    }

    diag_init(filename, TRUE);

    //装载安全模式
    memset(filename, 0x00, sizeof(filename));
    sprintf(filename, "%s", "/data/app/desktopGui/safemode.ini");
    ini = iniparser_load(filename);

    if(ini != NULL)
    {
        gSafeButton = iniparser_getint(ini, "SAFE_MODE:MAC_MODE", 1);
        TASK_FMT_DEBUG(-1, UP_LOG, "gSafeButton %d  \n", gSafeButton);
        gSafeButtonHPLC = iniparser_getint(ini, "SAFE_MODE:HPLC_MAC_MODE", 0);
        TASK_FMT_DEBUG(-1, UP_LOG, "gSafeButtonHPLC %d  \n", gSafeButtonHPLC);
        //为了台体测试，默认关闭路由学习
        gRouterStudy = iniparser_getint(ini, "SAFE_MODE:ROUTER_STATE_STUDY", 0);
        TASK_FMT_DEBUG(-1, UP_LOG, "gRouterStudy %d  \n", gRouterStudy);
        gMinuteCollect = iniparser_getint(ini, "SAFE_MODE:MINUTE_COLLECT_STATE", 0);
        TASK_FMT_DEBUG(-1, UP_LOG, "gMinuteCollect %d  \n", gMinuteCollect);
        iniparser_freedict(ini);
    }
    else
    {
        gSafeButton = 1;
        TASK_FMT_DEBUG(-1, UP_LOG, "没有读到gSafeButton %d  \n", gSafeButton);
        gSafeButtonHPLC = 0;
        TASK_FMT_DEBUG(-1, UP_LOG, "gSafeButtonHPLC %d  \n", gSafeButtonHPLC);
        //如果没配 就默认关闭路由学习 以防对测试有影响
        gRouterStudy = 0;
        TASK_FMT_DEBUG(-1, UP_LOG, "gRouterStudy %d  \n", gRouterStudy);
        gMinuteCollect = 0;
        TASK_FMT_DEBUG(-1, UP_LOG, "gMinuteCollect %d  \n", gMinuteCollect);
    }
    //送检分支 直接把路由学习关闭
    gRouterStudy = 0;
    TASK_FMT_DEBUG(-1, UP_LOG, "gRouterStudy %d  送检分支关闭\n", gRouterStudy);
}

/*******************************************************************************
* 函数名称: taskmng_privatedir_prepare
* 函数功能: 如果私有文件目录不存在则创建
* 输入参数: taskmng_privatedir_prepare 文件目录
* 输出参数: 无
* 返 回 值:
*******************************************************************************/
void taskmng_privatedir_prepare(char * Dir)
{

    if(0 != access(Dir, F_OK))
    {
        printf("创建目录%s\n", Dir);
        mkdir(Dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
}

/*******************************************************************************
* 函数名称: TGThread
* 函数功能: 总加组抄表线程，负责交采数据以及总加组脉冲数据的抄读
* 输入参数:
* 输出参数:
* 返 回 值:
*******************************************************************************/
void * TGThread(void * arg)
{
    prctl(PR_SET_NAME, "TGThread");
    TGThreadArg.DBClientHandle = db_client_create(DB_AGENT_CTS, DB_CLIENT_TG);
    taskManager_acMeter_fd_init();
    int ret;

    while(1)
    {
        gTGThreadFlag = 1;

        //处理交采代理
        while(1)
        {
            ret = taskmng_relay_check_tg();

            if(ret == -2)
                break;
        }

        if(MeterFileAll.FileState != TASK_CFG_INITED)
        {
            sleep(1);
            continue;
        }

        if(TGThreadArg.Pn == 0)
        {
            sleep(3);
            continue;
        }

        if(TGThreadArg.DBClientHandle < 0)
        {
            TASK_FMT_TRACE(-1, TG_LOG, "TGThread 创建dbclient失败！ \n");
            TGThreadArg.DBClientHandle = db_client_create(DB_AGENT_CTS, DB_CLIENT_TG);
            sleep(3);
            continue;
        }

        taskmng_data_init_check_tg();
        taskmng_time_manage_tg();
        usleep(20000);
    }

    db_client_close(TGThreadArg.DBClientHandle);
}
