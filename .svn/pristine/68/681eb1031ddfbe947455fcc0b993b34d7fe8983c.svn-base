#ifndef __CTSALL_H__
#define __CTSALL_H__


#define OS_METSTD2007_EN  1
#ifdef AREA_JIANGSU
#define OOP_CRV_ADDREAD_485 0    //485曲线补抄开关
#else
#define OOP_CRV_ADDREAD_485 0

#endif
#define OOP_DAY_ADDREAD_485 0       //485面向对象表日冻结补抄功能
#define D_485_698_RTTOPOINT 0
#define OS_METDLMS_EN  1
#define Rs485ReadNum  35

//#ifdef BOARD_3358
//#define IS_NEED_ADD_8HOUR 0     //存到数据中心的时间戳是否需要增加8小时
////#include "../include/oopType.h"
//#else
#define IS_NEED_ADD_8HOUR 1     //存到数据中心的时间戳是否需要增加8小时

#define MIN_TASK_CHOICE_ADD 100  
//#endif
#include "framecomm.h"
#include "oadlist.h"
#include "msgUdp.h"
#include "msgMqtt.h"
#include "appmsg_struct.h"
#include "CtsMain.h"
#include "CtsRedef.h"
#include "CtsDebug.h"
#include "CommFun.h"
#include "OOPMap.h"
#include "CtsMeterFile.h"
#include "OopAll.h"
#include "CtsAmr.h"
#include "CtsCommon.h"
#include "rs232.h"
#include "newcb.h"
#include "metstd.h"
#include "metstd2007.h"
#include "MetOop.h"
#include "metstdcap.h"
#include "metstdprotector.h"
#include "storage.h"
 #include <sys/prctl.h>
#include "CtsTask.h"
#include "CtsPlc.h"
#include "metdlms.h"
#include "metstd188.h"


//陕西关闭曲线实时转冻结
#ifdef AREA_SHAANXI
    #define CURRENT_TO_CURVE_ENABLE  0   //曲线实时转冻结使能
#else
    #define CURRENT_TO_CURVE_ENABLE  1
#endif
extern AmrThreadArg_T gAmrThreadArg[AMR_THREAD_NUM];
extern LockQueue_T *pAmrRecvDown[AMR_THREAD_NUM];       //抄表线程下行接收共享内存指针
extern LockQueue_T *pAmrSendDown[AMR_THREAD_NUM];       //抄表线程下行发送共享内存指针
extern LockQueue_T *pAmrRecvDown_Cb[AMR_THREAD_NUM];       //抄表线程下行接收共享内存指针
extern LockQueue_T *pAmrRecvUp[AMR_THREAD_NUM];         //抄表线程上行接收共享内存指针
extern LockQueue_T *pAmrSendUp[AMR_THREAD_NUM];         //抄表线程上行发送共享内存指针
extern SCHEME_TASK_RUN_INFO           SchemeTaskRunQueue[SCHEME_MAX_NUM];        //载波方案任务队列
extern SCHEME_TASK_RUN_INFO           *SchemeTaskRun_NormalLink;          //当前载波方案日常的任务
//方案任务参数队列
extern OOP_TASK_CFG_LIST              SchemeTaskCfgList;                         //载波方案任务参数列表

//档案队列
extern Meter_File_T                   MeterFileAll;                         //终端档案信息
extern uint8                          MeterFileFlag;            //档案修改标志
extern pthread_rwlock_t               TaskRWLock;                         //任务列表读写锁 多线程访问

extern uint8 gSafeButton;
extern uint8 gSafeButtonHPLC;
extern const OopGet_t            OopGet;
extern char   *DownName;      //定义下行的消息发送方名称
extern char   *CCOName;
extern char   *WIRELESSName;
extern ThreadArg_T        APPArg;
extern DB_UPDATE_T dbupdateMeter;
extern DB_UPDATE_T dbupdateTask;
extern DB_UPDATE_T dbupdatePlan;
extern EVENT_PLAN_T EventPlanList; 
extern char    pathname[AMR_THREAD_NUM][512];
extern TotalGroupThreadArg  TGThreadArg;
extern SEARCH_METER_CLEAR_T SearchMeterClean;

extern char mqttIP[100];
extern int mqttport;
extern char SocketDir[100];
extern uint8 gHPLCRouterState[PLC_NUM];

extern uint8 ReloadTaskFlag;
extern uint8 LocalAddr[6];
extern uint8 DataInitFlag[AMR_THREAD_NUM+1];
extern PastTimeFlag_T             gPastTimeFlag[AMR_THREAD_NUM];       //过时间标志 最后一个给任务管理线程用
extern DateTime_T                 gCurBinTime[AMR_THREAD_NUM];         //当前BIN时间
extern DateTime_T                 gCurBcdTime[AMR_THREAD_NUM];         //当前BCD时间
extern DateTime_T                 gPreBinTime[AMR_THREAD_NUM];         //上次BIN时间
extern MODEFY_TYPE_T              gMeterFileModifyType[AMR_THREAD_NUM];      //档案修改方式
extern uint8                      gMeterFileCmpFlag[AMR_THREAD_NUM];
extern uint8                      gMeterFileModifyFlag[AMR_THREAD_NUM];

extern uint8 gParallelNum;

extern DB_AGENT gDBAgent ;         //和数据中心交互的agent
extern DB_CLIENT gDBClientMain ;            
extern DB_CLIENT gDBClientTask ;
extern pthread_mutex_t  gMqttSendLock;
extern int gMqttSock ;           //mqtt线程创建的线程间通信fd
extern int gMainSock ;                      //主线程创建的
extern int gTaskSock ;                      //task线程创建的

extern uint16 PerceptionTerminalInfoNum;
extern uint16 EventReportInfoNum;
extern uint16 EventReportInfoNum645;
extern uint16 MinuteReportnfoNum;

extern pthread_mutex_t EventReportNumLock;
extern const OOP_OAD_U  EventReportOad; //电表事件存储用的oad
extern const OOP_OAD_U  EventReportOad645; 
extern const OOP_OAD_U  AcrossAreaOad ;  //非本台区电表结果存储OAD
extern const OOP_OAD_U  NodeRejectOad ;  //节点拒绝信息存储oad
extern const OOP_OAD_U  PowerOnOffOad;
extern const OOP_OAD_U  MtrTimingOad;
extern const OOP_OAD_U  IdenyifyResultOad;  //
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI) || (defined AREA_JIANGSU)
extern const OOP_OAD_U  PerceptionTerminalOad; //感知终端信息



#endif
extern const OOP_OAD_U  MinuteReportOad; //电表事件存储用的oad

extern uint16 NodeRejectNum;
extern uint16 PowerOnOffNum;
extern uint16 IdenyifyResultNum;

extern uint16 NetTopologySeq;    //存储网络拓扑信息使用的信息点号
extern pthread_mutex_t NetTopologySeqLock;
extern char PrivateDir[100];
extern TASK_MONITOR_T TaskMonitor;
extern SEARCH_METER_CYCLICITY_T SearchMeterParam; 
extern SYNC_TIMING_T   SyncTimingParam;
extern uint8 IsSetAddr[PLC_NUM];
extern NODE_UPDATE_CYCLE_T UpdateCycle;
extern ACROSS_AREA_T AcrossAreaResult; 
extern METER_SEARCH_RES_T  MeterSearchResult;  //搜表结果

extern CCO_STA_INFO_T ccostainfo;

extern uint8 MeterSearchResultClearFlag ; 

extern DBUPDATE_QUEUE_T pTaskDBUpdate; 
extern DBUPDATE_QUEUE_T pMeterFileUpdate;
extern DBUPDATE_QUEUE_T pTaskRecordFileDel[AMR_THREAD_NUM+1];   

extern uint8  gFreeFlag[AMR_THREAD_NUM];
extern uint8  gPowerOnFlag[AMR_THREAD_NUM];
extern uint8   gAmrThreadFlag[AMR_THREAD_NUM] ;

extern LockQueue_T gMeterSearchQueue;

extern FILE_UPDATE_ENCRYPT_T gFileUpdateEncrypt;

extern pthread_mutex_t     gModuleInfoLock;
extern OOP_MODULE_INFO_NEW_T gModuleInfo;  //模组信息
extern SPEED_LIST_485_T gSpeedList;

extern int gEncryptPipe;
#ifdef AREA_FUJIAN
extern DBUPDATE_QUEUE_T pGropMeterFileUpdate;
#endif

#define ACM3  3
#define ACM4  4
#define PIPE0 254
#define PIPE1 254
#define PIPE2 ACM3
#define PIPE3 ACM4
#define PIPE4 254
#define PIPE5 254
#define PIPE6 254
#define PIPE7 254
#define PIPE8 254
#define PIPE9 254

#define MET07_RT_DUMP_CURVE_MIN_EN  1           //07电表实时转存曲线使能
#define CURVE_ADAPT_FUNC            1       //07表负荷曲线标识自适应功能
#define CBTASK_NEW  1
#define HARDWARE_2012_EN            1           //2012年新规范终端功能
#define CBTASK_OOP  1
#define OS_METDLT645_EN             1

#define MAX_APPID_LEN  24

//打印格式化信息，TRACE是不记录日志的,DEBUG是必记日志的,LOGCC是受控记日志的
#define TASK_FMT_TRACE(pipe,logpath,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 27, GetFileName(logpath,pipe), 0, NULL, 0, fmt, ##__VA_ARGS__)
#define TASK_FMT_DEBUG(pipe,logpath,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 27, GetFileName(logpath,pipe), 1, NULL, 0, fmt, ##__VA_ARGS__)
//打印带描述的buffer内容,格式为每2位16进制数空一格
#define TASK_BUF_TRACE(pipe,logpath,buf,len)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 27, GetFileName(logpath,pipe), 0, buf, len, NULL)
#define TASK_BUF_DEBUG(pipe,logpath,buf,len)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 27, GetFileName(logpath,pipe), 1, buf, len, NULL)


extern int task_msg_init(uint32 id);
extern int task_msg_settime(int sockfd, uint32 sec, uint32 usec);
extern int read_pdata_xram(const char *filename, char *buf, unsigned offset, unsigned len);
extern int write_pdata_xram(const char *filename, const char *buf, unsigned offset, unsigned len);
extern void app_get_maketime(char *ptime, Date_Time_S *pTime);
extern void *memcpy_r(void *dest, const void *src, int count);

extern DateTime_T                 gCurBinTimeTask;                             //当前BIN时间

extern DateTime_T                 gPreBinTimeTask;                             //上次BIN时间

#endif
