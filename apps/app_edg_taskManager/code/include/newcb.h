#ifndef _NEWCB_H
#define _NEWCB_H

#include "CtsAll.h"
#include "metstd.h"
#include "metstd2007.h"

#define DEBUG_NEWCB "DEBUG_NEWCB"
#define CBPRINT(x...) ({ if (getenv("DEBUG_NEWCB") != NULL)  printf(x);})

extern const S_CbIDInfo QUEUE_LOSSVOLEVENT_TASK_ID[];

#define QUEUESTAT_UNALBE    0x55        //任务类型无效
#define QUEUESTAT_ENABLE    0xAA        //任务类型有效
#define RS485_MODULE_DEVNAME  "uart_acm"


typedef struct
{
    uint8 second;
    uint8 minute;
    uint8 hour;
    uint8 day;
    uint8 month;
    uint8 year;
}__attribute__ ((packed)) time_struct;        /*时间结构*/


//通道各种状态
typedef enum
{
    //PIPESTATE_STOP = -100,        //停止抄表
    PIPESTATE_XIAOBIAO=-100,        //校表
    PIPESTATE_POWEROFF,             //停电

    PIPESTATE_NORMAL = 0,           //正常抄表

    //PIPESTATE_PAUSE = 100,        //暂停抄表
    //PIPESTATE_RELAY,              //正在中继
    PIPESTATE_RUN_FLAG,             //空转状态
    PIPESTATE_CBRESTART,            //抄表重启，退出当前表库，重新进入
}PIPESTATE;

//typedef struct
//{
//    char  deviceName[30];        /*设备名 */
//    uint8 isvalid;
//    void *pDev;
//}RS485_RS232_PARA;

/*
抄表过程中，对通道各种状态的检查
该函数除了检查系统状态以外，还要检查其他状态
< 0 退出抄表流程
> 0 暂停抄表，等待重新启动
= 0 可以正常抄表
*/

PIPESTATE cbtaskOop_statecheck(uint8 pipe);//这个5s判一次
//PIPESTATE cbtasknew_statecheck(uint8 pipe, S_cbTaskQueue *pPipeCbTaskQueue);
//CB_RET cbtasknew_MetAutkExec(S_cbTaskQueue *pAllCbTaskQueue, enum_cbtask_id taskid);
extern CB_RET cbTaskOop_MetAutkExec(uint8 pipe,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern CB_RET Met485_AutkExec(uint8 pipe,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo);
extern CB_RET Check_Now_Task_Run_Info_Param(SCHEME_TASK_RUN_INFO *pCbTask,uint8 pipe,CTS_RUN_TASK_T *RunTaskInfo);
extern CB_STATE Cheeck_Cb_Pipe(SCHEME_TASK_RUN_INFO *pCbTask,S_COMM_INFO_MET* Comm_Info_Oop,cbMtrPara * MtrPara,uint8 pipe);
extern void Meter_645_97_Read_Item(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI97Index,uint8 *DI97Num,uint16 *DI97Buff);
extern void Meter_645_97_Recvive_Data(SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo, uint8 pipe, S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DI97Index,uint8 DI97Num,uint8 *CombinBuff,uint16 *CombinLen,uint16 *DI97Buff);
extern int Meter485RelayTransDeal (uint8 pipe,uint8 priority);
extern void Init_Relay_Cb_Pipe(uint8 pipe,uint8* speed);
extern void ExecMeter485Relay(uint8 pipe,CB_STATE *Cbstatus,uint8 priority);



void cbtasknew_ReadScalePara(mtrnum_t scaleNum, cbMtrPara *pCbMtrPara);        //读取计量点参数


typedef struct
{
    uint8    Flag;            /*- 处理标志 AUTKTAG_SUCCESS 成功   AUTKTAG_FAIL 失败-*/
    uint8    type;            /*- 数据字典ID类别 -*/
    uint16    DataId;            /*- 数据字典ID-*/
    uint16    Offset;            /*- 偏移地址 -*/
    uint16    len;            /*- 对应数据项的长度 -*/
}cbtasknew_autkTag;            /*-自动任务数据填充标志结构-*/

typedef struct
{
    uint8        type;            //数据类型
    uint16        bBeginID;        //起始ID
    uint16        bEndID;            //结束ID
    uint16        bReadTag;        //终端抄表标记
    uint16        bReadTime;        //终端抄表时间
}PROT_READ_FLAG_T;




#define AUTKTAG_INIT        0x00        //尚未处理
#define AUTKTAG_SUCCESS        0x55        //抄表成功
#define AUTKTAG_FAIL        0xAA        //抄表失败
#define AUTKTAG_UNSPU        0x11        //当前抄表驱动库不支持，不需要处理 表库里没有这个数据项
#define AUTKTAG_CLASS        0x12        //大小类号不支持

int cbtasknew_getItemMtrLen(uint8 type, uint32 id);            //取得某种数据类型的ID的长度
typedef enum
{
    RELAYDC_PIPENULL,        //通道无效 无需做穿透和点抄
    RELAYDC_IDLE,            //通道有效 无穿透抄任务
    RELAYDC_RCV_NEW,        //接收到主台新穿透抄任务
    RELAYDC_COMM_WAITFREE,    //等待抄表线程释放串口
    RELAYDC_COMM_FREEED,    //串口已经释放，可以发送并处理中继报文
    RELAYDC_SENDSOCK,        //穿透抄已执行，处于等待发送到IPC状态
}RELAYDC_STATE;         /*-穿透抄任务状态-*/



//判断任务ID是否是当前任务集中的任务
//uint8 cbtasknew_IsInTaskId(uint8 pipe, uint8 type, uint16 id, const S_CbIDInfo *pCbTaskID, uint16 *index, enum_cbtask_id taskid);
RELAYDC_STATE GetRelayState(uint8 pipe);
void SetRelayState(uint8 pipe, RELAYDC_STATE newstate);
//void cbtasknew_SDataSave(uint8 pipe, cbMtrPara mCbMtrPara, S_cbTaskQueue *pPipeCbTaskQueue, CB_RET flag, enum_cbtask_id taskid);
//CB_STATE cbtasknew_drvcheck(enum_cbtask_id taskid, cbMtrPara lstCbMtrPara, S_cbTaskQueue *pPipeCbTaskQueue, time_t *pLstTime, int *p_fd232, uint8 pipe);


CB_STATE cbtaskOop_drvcheck(SCHEME_TASK_RUN_INFO *pCbTask,cbMtrPara *lstCbMtrPara, time_t *pLstTime,uint8 pipe);

#if CBTASK_NEW == 1 && OS_RELAYTASK_EN == 1

//中继命令的相关信息
//typedef struct RelayInfo_
//{
//    RELAY_IPC_PULS relay_beg_msg;         /*-与应用层的IPC接口，不需修改-*/
//    struct RelayInfo_ *pNext;                //指向下一帧报文的中继信息
//}RelayInfo;

//中继命令的链表头
//typedef struct RelayInfoLink_
//{
//    pthread_mutex_t mutexlock;                //中继命令的互斥锁
//    RELAYDC_STATE state;                    //穿透抄任务状态
//    cbMetPara MetCommParam;                    //根据中继命令解析出来的通讯参数
//    time_t SendTime;                        //发送的起始时间
//    RelayInfo *pRelayInfoHead;                //第一个中继命令头
//}RelayInfoLink;

//新启动抄表任务，用于搜表判断优先级
typedef struct _NewTaskInfo
{
    pthread_mutex_t mutexlock;
    BOOL state;
}NewTaskInfo;

BOOL cbtasknew_IsNewTask(uint8 pipe);
BOOL cbtasknew_IsRelaying(uint8 pipe);
#endif
#define PrintfBuffer(pBuf,BufLen)      {printf("%s:",__FUNCTION__);int i;for( i=0;i<(int)(BufLen);i++) {printf("%02X ",*((pBuf)+i));} printf("\n");}
extern uint8 BCDtoBIN(uint8 BCDnum);
extern void PrintHexData(char* pHex, int nHexLen);
extern void GetRealTime(time_struct *pTime);
extern void GetRealTimeBin(time_struct *pTime);

#if D_485_698_RTTOPOINT == 1
extern BOOL IsRealDataCopyToPointData(void);
#endif
int checkIsExist2022(SCHEME_TASK_RUN_INFO *pCbTask,OOP_OAD_U  EventMainOad,uint8 pipe);





/* 模组类型码 */
#define MDL_YK                      2
#define MDL_YX                      4
#define MDL_RS485                   5

/* 函数返回 */
#define RET_OK                      0
#define RET_ERR                    -1

typedef struct 
{
    uint8            mdl_type;         // 模组类型
    uint8            device_no;         // 设备号从1开始,用于同类型设备从左到右的排序,如USB1-USB5
} MDL_DEV_INFO;                    // 模组设备相关信息

//typedef enum
//{
//    TASK_RELAY,                //0        中继直抄命令    由规约模块触发
//    TASK_DATA1_RELAY,       //1     点抄任务 中继线程处理 不在485抄表线程处理
//    TASK_CURVE,             //2     曲线任务 每15min启动，抄上个15min数据
//    TASK_CBDAY,             //3     抄表日任务    设定的抄表日时间
//    TASK_MONTH,             //4     月任务    当前时间为23点50分
//    TASK_MIN1440,           //5        1440分钟抄表数据项预抄    23时55分钟
//    TASK_MIN60,                //6        60分钟抄表数据项预抄    抄读07电能表的整点数据
//    TASK_MIN30,                //7        30分钟抄表数据项预抄    当前分钟为30或5分钟
//    TASK_POINTDAY,          //8     日任务 00:05启动
//    TASK_MIN1,              //9        1分钟抄表数据项预抄    当前分钟为5的倍数
//    TASK_FORM_CB,           //10    定制实时任务    时间为设定时间的倍数
//    TASK_PRE_MIN15,            //11    15分钟前任务，暂定为前一分钟任务，增加曲线数据的精度
//    TASK_ESP_CB,            //12    特殊任务    针对一些特殊要求的电表   交采任务 5秒抄一次
//    TASK_MIN15,             //13     15分钟抄表数据项预抄    当前分钟为15的倍数
//    TASK_POINTDAYBC,        //14    日冻结任务对应07电表的日数据补抄功能 0:15启动
//    TASK_EVENT_LOSS_VOL,    //14   失压事件
//    TASK_NO_MET,            //16    无抄表任务状态    当前没有抄表任务
//}enum_cbtask_id;
//抄表队列中，需要抄表的测量点号
typedef struct cbMtrLink_
{
    mtrnum_t mtrNum;                //需要抄表的测量点号
    uint8 MtrType;                    //测量点类别: MTR_485, MTR_SIM...
    uint8 PrtlType;                    //测量点规约
    struct cbMtrLink_ *pNextMtr;    //下一个测量点信息
}cbMtrLink;
 //抄表队列
 typedef struct cbTaskQueue_
 {
     uint8                QueueStat;            //当前任务有效性，0 代表无效 1 代表有效
     uint8                QueuePipe;            //当前任务对应的通道类型
     enum_cbtask_id        QueueTaskType;        //任务类型    日任务、月任务、小时任务 so so
     cbMtrLink            *pFirstMtrLink;        //任务队列中第一个测量点信息
     const S_CbIDInfo    *pQueueTaskID;        //任务数据项ID集合
     uint16                QueueTaskIDCount;    //数据项的个数
 }S_cbTaskQueue;

int UpdateMeterCommName(char *ttyName, uint8 ComNo);

extern uint8 GetHighPriorityTask(uint8 pipe,CTS_RUN_TASK_T **TaskInfo,SCHEME_TASK_RUN_INFO *CurTaskRunInfo);

extern void pTaskRunInfoinit(uint8 pipe);
extern void cbtasknew_TaskCb(uint8 pipe);
extern void Cb_State_Init(uint8 prtltype,uint8 pipe,S_COMM_INFO_MET* Comm_Info_Oop,cbMtrPara* MtrPara);
extern void Cb_State_Send(uint8 prtltype,SCHEME_TASK_RUN_INFO * pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe,S_COMM_INFO_MET* Comm_Info_Oop,cbMtrPara* MtrPara,uint8 *DIIndex,uint8 *DINum,uint16 *DI97Buff,uint32 *DI07Buff);
extern void Cb_State_Recv(uint8 prtltype,SCHEME_TASK_RUN_INFO *pCbTask,CTS_RUN_TASK_T *RunTaskInfo,uint8 pipe,S_COMM_INFO_MET *Comm_Info_Oop,cbMtrPara *MtrPara,uint8 *DIIndex,uint8 DINum,uint8 *CombinBuff,uint16 *CombinLen,uint16 *DI97Buff,uint32 *DI07Buff);
extern BOOL get_rs485_dev(uint8 Pipe);
extern int RelayCmdMeter485PrtlData ( uint8 pipe,uint8 speed,uint8 priority);
void taskmng_rs485_patch_file_delete(uint8 pipe,uint16 TaskId);
int taskmng_data_reading_encrypt_485(uint8 pipe,SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask);
int taskmng_file_transfer_deal_485(uint8 pipe);
void taskmng_task_run_info_reset_485(CTS_RUN_TASK_T *TaskInfo,SCHEME_TASK_RUN_INFO *TaskRunInfo);

#endif

