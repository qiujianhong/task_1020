#ifndef __CTSALL_H__
#define __CTSALL_H__


#define OS_METSTD2007_EN  1
#ifdef AREA_JIANGSU
#define OOP_CRV_ADDREAD_485 0    //485���߲�������
#else
#define OOP_CRV_ADDREAD_485 0

#endif
#define OOP_DAY_ADDREAD_485 0       //485���������ն��Ჹ������
#define D_485_698_RTTOPOINT 0
#define OS_METDLMS_EN  1
#define Rs485ReadNum  35

//#ifdef BOARD_3358
//#define IS_NEED_ADD_8HOUR 0     //�浽�������ĵ�ʱ����Ƿ���Ҫ����8Сʱ
////#include "../include/oopType.h"
//#else
#define IS_NEED_ADD_8HOUR 1     //�浽�������ĵ�ʱ����Ƿ���Ҫ����8Сʱ

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


//�����ر�����ʵʱת����
#ifdef AREA_SHAANXI
    #define CURRENT_TO_CURVE_ENABLE  0   //����ʵʱת����ʹ��
#else
    #define CURRENT_TO_CURVE_ENABLE  1
#endif
extern AmrThreadArg_T gAmrThreadArg[AMR_THREAD_NUM];
extern LockQueue_T *pAmrRecvDown[AMR_THREAD_NUM];       //�����߳����н��չ����ڴ�ָ��
extern LockQueue_T *pAmrSendDown[AMR_THREAD_NUM];       //�����߳����з��͹����ڴ�ָ��
extern LockQueue_T *pAmrRecvDown_Cb[AMR_THREAD_NUM];       //�����߳����н��չ����ڴ�ָ��
extern LockQueue_T *pAmrRecvUp[AMR_THREAD_NUM];         //�����߳����н��չ����ڴ�ָ��
extern LockQueue_T *pAmrSendUp[AMR_THREAD_NUM];         //�����߳����з��͹����ڴ�ָ��
extern SCHEME_TASK_RUN_INFO           SchemeTaskRunQueue[SCHEME_MAX_NUM];        //�ز������������
extern SCHEME_TASK_RUN_INFO           *SchemeTaskRun_NormalLink;          //��ǰ�ز������ճ�������
//���������������
extern OOP_TASK_CFG_LIST              SchemeTaskCfgList;                         //�ز�������������б�

//��������
extern Meter_File_T                   MeterFileAll;                         //�ն˵�����Ϣ
extern uint8                          MeterFileFlag;            //�����޸ı�־
extern pthread_rwlock_t               TaskRWLock;                         //�����б��д�� ���̷߳���

extern uint8 gSafeButton;
extern uint8 gSafeButtonHPLC;
extern const OopGet_t            OopGet;
extern char   *DownName;      //�������е���Ϣ���ͷ�����
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
extern PastTimeFlag_T             gPastTimeFlag[AMR_THREAD_NUM];       //��ʱ���־ ���һ������������߳���
extern DateTime_T                 gCurBinTime[AMR_THREAD_NUM];         //��ǰBINʱ��
extern DateTime_T                 gCurBcdTime[AMR_THREAD_NUM];         //��ǰBCDʱ��
extern DateTime_T                 gPreBinTime[AMR_THREAD_NUM];         //�ϴ�BINʱ��
extern MODEFY_TYPE_T              gMeterFileModifyType[AMR_THREAD_NUM];      //�����޸ķ�ʽ
extern uint8                      gMeterFileCmpFlag[AMR_THREAD_NUM];
extern uint8                      gMeterFileModifyFlag[AMR_THREAD_NUM];

extern uint8 gParallelNum;

extern DB_AGENT gDBAgent ;         //���������Ľ�����agent
extern DB_CLIENT gDBClientMain ;            
extern DB_CLIENT gDBClientTask ;
extern pthread_mutex_t  gMqttSendLock;
extern int gMqttSock ;           //mqtt�̴߳������̼߳�ͨ��fd
extern int gMainSock ;                      //���̴߳�����
extern int gTaskSock ;                      //task�̴߳�����

extern uint16 PerceptionTerminalInfoNum;
extern uint16 EventReportInfoNum;
extern uint16 EventReportInfoNum645;
extern uint16 MinuteReportnfoNum;

extern pthread_mutex_t EventReportNumLock;
extern const OOP_OAD_U  EventReportOad; //����¼��洢�õ�oad
extern const OOP_OAD_U  EventReportOad645; 
extern const OOP_OAD_U  AcrossAreaOad ;  //�Ǳ�̨��������洢OAD
extern const OOP_OAD_U  NodeRejectOad ;  //�ڵ�ܾ���Ϣ�洢oad
extern const OOP_OAD_U  PowerOnOffOad;
extern const OOP_OAD_U  MtrTimingOad;
extern const OOP_OAD_U  IdenyifyResultOad;  //
#if (defined AREA_JIANGXI) || (defined AREA_ANHUI) || (defined AREA_JIANGSU)
extern const OOP_OAD_U  PerceptionTerminalOad; //��֪�ն���Ϣ



#endif
extern const OOP_OAD_U  MinuteReportOad; //����¼��洢�õ�oad

extern uint16 NodeRejectNum;
extern uint16 PowerOnOffNum;
extern uint16 IdenyifyResultNum;

extern uint16 NetTopologySeq;    //�洢����������Ϣʹ�õ���Ϣ���
extern pthread_mutex_t NetTopologySeqLock;
extern char PrivateDir[100];
extern TASK_MONITOR_T TaskMonitor;
extern SEARCH_METER_CYCLICITY_T SearchMeterParam; 
extern SYNC_TIMING_T   SyncTimingParam;
extern uint8 IsSetAddr[PLC_NUM];
extern NODE_UPDATE_CYCLE_T UpdateCycle;
extern ACROSS_AREA_T AcrossAreaResult; 
extern METER_SEARCH_RES_T  MeterSearchResult;  //�ѱ���

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
extern OOP_MODULE_INFO_NEW_T gModuleInfo;  //ģ����Ϣ
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

#define MET07_RT_DUMP_CURVE_MIN_EN  1           //07���ʵʱת������ʹ��
#define CURVE_ADAPT_FUNC            1       //07�������߱�ʶ����Ӧ����
#define CBTASK_NEW  1
#define HARDWARE_2012_EN            1           //2012���¹淶�ն˹���
#define CBTASK_OOP  1
#define OS_METDLT645_EN             1

#define MAX_APPID_LEN  24

//��ӡ��ʽ����Ϣ��TRACE�ǲ���¼��־��,DEBUG�Ǳؼ���־��,LOGCC���ܿؼ���־��
#define TASK_FMT_TRACE(pipe,logpath,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 27, GetFileName(logpath,pipe), 0, NULL, 0, fmt, ##__VA_ARGS__)
#define TASK_FMT_DEBUG(pipe,logpath,fmt,...)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 27, GetFileName(logpath,pipe), 1, NULL, 0, fmt, ##__VA_ARGS__)
//��ӡ��������buffer����,��ʽΪÿ2λ16��������һ��
#define TASK_BUF_TRACE(pipe,logpath,buf,len)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 27, GetFileName(logpath,pipe), 0, buf, len, NULL)
#define TASK_BUF_DEBUG(pipe,logpath,buf,len)  diag_info_print(__FILE__,__LINE__,__FUNCTION__, 27, GetFileName(logpath,pipe), 1, buf, len, NULL)


extern int task_msg_init(uint32 id);
extern int task_msg_settime(int sockfd, uint32 sec, uint32 usec);
extern int read_pdata_xram(const char *filename, char *buf, unsigned offset, unsigned len);
extern int write_pdata_xram(const char *filename, const char *buf, unsigned offset, unsigned len);
extern void app_get_maketime(char *ptime, Date_Time_S *pTime);
extern void *memcpy_r(void *dest, const void *src, int count);

extern DateTime_T                 gCurBinTimeTask;                             //��ǰBINʱ��

extern DateTime_T                 gPreBinTimeTask;                             //�ϴ�BINʱ��

#endif
