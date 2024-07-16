#ifndef _NEWCB_H
#define _NEWCB_H

#include "CtsAll.h"
#include "metstd.h"
#include "metstd2007.h"

#define DEBUG_NEWCB "DEBUG_NEWCB"
#define CBPRINT(x...) ({ if (getenv("DEBUG_NEWCB") != NULL)  printf(x);})

extern const S_CbIDInfo QUEUE_LOSSVOLEVENT_TASK_ID[];

#define QUEUESTAT_UNALBE    0x55        //����������Ч
#define QUEUESTAT_ENABLE    0xAA        //����������Ч
#define RS485_MODULE_DEVNAME  "uart_acm"


typedef struct
{
    uint8 second;
    uint8 minute;
    uint8 hour;
    uint8 day;
    uint8 month;
    uint8 year;
}__attribute__ ((packed)) time_struct;        /*ʱ��ṹ*/


//ͨ������״̬
typedef enum
{
    //PIPESTATE_STOP = -100,        //ֹͣ����
    PIPESTATE_XIAOBIAO=-100,        //У��
    PIPESTATE_POWEROFF,             //ͣ��

    PIPESTATE_NORMAL = 0,           //��������

    //PIPESTATE_PAUSE = 100,        //��ͣ����
    //PIPESTATE_RELAY,              //�����м�
    PIPESTATE_RUN_FLAG,             //��ת״̬
    PIPESTATE_CBRESTART,            //�����������˳���ǰ��⣬���½���
}PIPESTATE;

//typedef struct
//{
//    char  deviceName[30];        /*�豸�� */
//    uint8 isvalid;
//    void *pDev;
//}RS485_RS232_PARA;

/*
��������У���ͨ������״̬�ļ��
�ú������˼��ϵͳ״̬���⣬��Ҫ�������״̬
< 0 �˳���������
> 0 ��ͣ�����ȴ���������
= 0 ������������
*/

PIPESTATE cbtaskOop_statecheck(uint8 pipe);//���5s��һ��
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



void cbtasknew_ReadScalePara(mtrnum_t scaleNum, cbMtrPara *pCbMtrPara);        //��ȡ���������


typedef struct
{
    uint8    Flag;            /*- �����־ AUTKTAG_SUCCESS �ɹ�   AUTKTAG_FAIL ʧ��-*/
    uint8    type;            /*- �����ֵ�ID��� -*/
    uint16    DataId;            /*- �����ֵ�ID-*/
    uint16    Offset;            /*- ƫ�Ƶ�ַ -*/
    uint16    len;            /*- ��Ӧ������ĳ��� -*/
}cbtasknew_autkTag;            /*-�Զ�������������־�ṹ-*/

typedef struct
{
    uint8        type;            //��������
    uint16        bBeginID;        //��ʼID
    uint16        bEndID;            //����ID
    uint16        bReadTag;        //�ն˳�����
    uint16        bReadTime;        //�ն˳���ʱ��
}PROT_READ_FLAG_T;




#define AUTKTAG_INIT        0x00        //��δ����
#define AUTKTAG_SUCCESS        0x55        //����ɹ�
#define AUTKTAG_FAIL        0xAA        //����ʧ��
#define AUTKTAG_UNSPU        0x11        //��ǰ���������ⲻ֧�֣�����Ҫ���� �����û�����������
#define AUTKTAG_CLASS        0x12        //��С��Ų�֧��

int cbtasknew_getItemMtrLen(uint8 type, uint32 id);            //ȡ��ĳ���������͵�ID�ĳ���
typedef enum
{
    RELAYDC_PIPENULL,        //ͨ����Ч ��������͸�͵㳭
    RELAYDC_IDLE,            //ͨ����Ч �޴�͸������
    RELAYDC_RCV_NEW,        //���յ���̨�´�͸������
    RELAYDC_COMM_WAITFREE,    //�ȴ������߳��ͷŴ���
    RELAYDC_COMM_FREEED,    //�����Ѿ��ͷţ����Է��Ͳ������м̱���
    RELAYDC_SENDSOCK,        //��͸����ִ�У����ڵȴ����͵�IPC״̬
}RELAYDC_STATE;         /*-��͸������״̬-*/



//�ж�����ID�Ƿ��ǵ�ǰ�����е�����
//uint8 cbtasknew_IsInTaskId(uint8 pipe, uint8 type, uint16 id, const S_CbIDInfo *pCbTaskID, uint16 *index, enum_cbtask_id taskid);
RELAYDC_STATE GetRelayState(uint8 pipe);
void SetRelayState(uint8 pipe, RELAYDC_STATE newstate);
//void cbtasknew_SDataSave(uint8 pipe, cbMtrPara mCbMtrPara, S_cbTaskQueue *pPipeCbTaskQueue, CB_RET flag, enum_cbtask_id taskid);
//CB_STATE cbtasknew_drvcheck(enum_cbtask_id taskid, cbMtrPara lstCbMtrPara, S_cbTaskQueue *pPipeCbTaskQueue, time_t *pLstTime, int *p_fd232, uint8 pipe);


CB_STATE cbtaskOop_drvcheck(SCHEME_TASK_RUN_INFO *pCbTask,cbMtrPara *lstCbMtrPara, time_t *pLstTime,uint8 pipe);

#if CBTASK_NEW == 1 && OS_RELAYTASK_EN == 1

//�м�����������Ϣ
//typedef struct RelayInfo_
//{
//    RELAY_IPC_PULS relay_beg_msg;         /*-��Ӧ�ò��IPC�ӿڣ������޸�-*/
//    struct RelayInfo_ *pNext;                //ָ����һ֡���ĵ��м���Ϣ
//}RelayInfo;

//�м����������ͷ
//typedef struct RelayInfoLink_
//{
//    pthread_mutex_t mutexlock;                //�м�����Ļ�����
//    RELAYDC_STATE state;                    //��͸������״̬
//    cbMetPara MetCommParam;                    //�����м��������������ͨѶ����
//    time_t SendTime;                        //���͵���ʼʱ��
//    RelayInfo *pRelayInfoHead;                //��һ���м�����ͷ
//}RelayInfoLink;

//�������������������ѱ��ж����ȼ�
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





/* ģ�������� */
#define MDL_YK                      2
#define MDL_YX                      4
#define MDL_RS485                   5

/* �������� */
#define RET_OK                      0
#define RET_ERR                    -1

typedef struct 
{
    uint8            mdl_type;         // ģ������
    uint8            device_no;         // �豸�Ŵ�1��ʼ,����ͬ�����豸�����ҵ�����,��USB1-USB5
} MDL_DEV_INFO;                    // ģ���豸�����Ϣ

//typedef enum
//{
//    TASK_RELAY,                //0        �м�ֱ������    �ɹ�Լģ�鴥��
//    TASK_DATA1_RELAY,       //1     �㳭���� �м��̴߳��� ����485�����̴߳���
//    TASK_CURVE,             //2     �������� ÿ15min���������ϸ�15min����
//    TASK_CBDAY,             //3     ����������    �趨�ĳ�����ʱ��
//    TASK_MONTH,             //4     ������    ��ǰʱ��Ϊ23��50��
//    TASK_MIN1440,           //5        1440���ӳ���������Ԥ��    23ʱ55����
//    TASK_MIN60,                //6        60���ӳ���������Ԥ��    ����07���ܱ����������
//    TASK_MIN30,                //7        30���ӳ���������Ԥ��    ��ǰ����Ϊ30��5����
//    TASK_POINTDAY,          //8     ������ 00:05����
//    TASK_MIN1,              //9        1���ӳ���������Ԥ��    ��ǰ����Ϊ5�ı���
//    TASK_FORM_CB,           //10    ����ʵʱ����    ʱ��Ϊ�趨ʱ��ı���
//    TASK_PRE_MIN15,            //11    15����ǰ�����ݶ�Ϊǰһ�������������������ݵľ���
//    TASK_ESP_CB,            //12    ��������    ���һЩ����Ҫ��ĵ��   �������� 5�볭һ��
//    TASK_MIN15,             //13     15���ӳ���������Ԥ��    ��ǰ����Ϊ15�ı���
//    TASK_POINTDAYBC,        //14    �ն��������Ӧ07���������ݲ������� 0:15����
//    TASK_EVENT_LOSS_VOL,    //14   ʧѹ�¼�
//    TASK_NO_MET,            //16    �޳�������״̬    ��ǰû�г�������
//}enum_cbtask_id;
//��������У���Ҫ����Ĳ������
typedef struct cbMtrLink_
{
    mtrnum_t mtrNum;                //��Ҫ����Ĳ������
    uint8 MtrType;                    //���������: MTR_485, MTR_SIM...
    uint8 PrtlType;                    //�������Լ
    struct cbMtrLink_ *pNextMtr;    //��һ����������Ϣ
}cbMtrLink;
 //�������
 typedef struct cbTaskQueue_
 {
     uint8                QueueStat;            //��ǰ������Ч�ԣ�0 ������Ч 1 ������Ч
     uint8                QueuePipe;            //��ǰ�����Ӧ��ͨ������
     enum_cbtask_id        QueueTaskType;        //��������    ������������Сʱ���� so so
     cbMtrLink            *pFirstMtrLink;        //��������е�һ����������Ϣ
     const S_CbIDInfo    *pQueueTaskID;        //����������ID����
     uint16                QueueTaskIDCount;    //������ĸ���
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

