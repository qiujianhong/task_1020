#ifndef __FUJIAN_TASK_H__
#define __FUJIAN_TASK_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "CtsAll.h"



typedef struct
{
    pthread_mutex_t     Lock;                  //��д�� ���̷߳���
    TASK_CFG_STATE      TaskCfgState;         //���񵵰�״̬
    int                 ProCfgModifyDelaySec;  //���������޸���ʱ
    time_t              FunLastRunTime;        //�����ϴ��޸�ʱ��
    uint16               TaskNum;               //�����ܸ���
    OOP_TRANS_TASK_T    TaskCfg[MAX_TRANS_TASK_NUM];       //����������
}FUJIAN_TASK_CFG_LIST_T;




typedef struct
{
    uint16 taskNum;
    FUJIAN_SPECIFIC_TASK_T taskList[MAX_SPECIFIC_TASK_NUM]; //���о�������
}FUJIAN_SPECIFIC_TASK_LIST_T;


typedef struct
{
    pthread_rwlock_t    rwLock;
    TASK_CFG_STATE                FileState;                 //����״̬
    int                           ModifyDelaySec;     //���������޸���ʱ
    time_t                        FunLastRunTime;            //�����ϴ��޸�ʱ��
    uint16 nNum;
    OOP_GROUPMETER_NEW_T  grop_meter[255];  
}FUJIAN_grop_meter;



//͸�����񷽰��ɼ�״̬
typedef struct
{
    pthread_mutex_t lock;
    uint16 taskNum;
    OOP_TRANS_TASK_RLUESTATE_T taskState[MAX_TRANS_TASK_NUM];
}FUJIAN_TASK_COLLECTION_STATE_T;

//͸������������¼�ļ�
typedef struct
{   
    uint16 taskid;
    char  thisTurnBeginTime[7]; //���ֿ�ʼʱ��
    uint16 specificTaskIndex;   //��¼�µ�ǰ���е��ڼ�����������
    uint16 meterIndex;          //��¼�µ�ǰ���е���һ����������ĵڼ�����
    uint16 storageCnt;      //�洢����
    RUN_RESULT  result;              //��ǰ�ִ��Ƿ�ִ�гɹ�
   uint16 meterNum;        //һ�����񷽰������е����� Ԥ�泭��Ͳ���������
    FUJIAN_METER_INFO_T meterInfo[MAX_METER_NUM]; //��Ӧÿ��������Ϣ Ԥ�泭��Ͳ���������
}FUJIAN_TASK_RECORD_FILE_T;

typedef enum
{
    TRANS_TASK_INIT = 0,
    TRANS_TASK_START,
   // TRANS_TASK_SEND,
    //TRANS_TASK_RECV,
    TRANS_TASK_SUCC,
    TRANS_TASK_EXEC,
    TRANS_TASK_CLEAR,   //�յ����ݳ�ʼ����������
    TRANS_TASK_RELATION,        //��֡������������ʱ��
}FUJIAN_TASK_STATE_CMD_E;

typedef enum
{
    TASK_SCHEDULED = 0,
    TASK_BROADCAST,
    TASK_METERSEARCH,
    TASK_IDLE,
    TASK_TRIGGER = 5,
    TASK_MASTER_TRANS,
    TASK_NULL,

    PIPE_ERROR = 255,
    
}FUJIAN_TASK_TYPE_E;

typedef struct
{
    uint8 buflen;
    uint8 buf[255];
}FUJIAN_645BUF_T;

typedef struct
{
    uint8 num;
    FUJIAN_645BUF_T databuf[20];
}FUJIAN_MULTI_645BUF_T;

typedef struct
{
    uint16 len;
    uint8 recvbuf[5000];
}FUJIAN_MULTI_645_RECV_T;



extern uint8 LocalAddr[6];

void taskmng_trans_task_cfg_init();
void taskmng_trans_task_ergodic();
void taskmng_trans_task_db_update_check(DB_UPDATE_T *dbupdate);
void taskmng_trans_task_run_ergodic(uint8 Pipe);
BOOL taskmng_grop_addr_repect_check(FUJIAN_grop_meter *pMeterFileAll,OOP_GROUPMETER_NEW_T *meterFile);

int taskmng_grop_addr_isvalid(uint8 *MtrAddr);
void taskmng_trans_task_run_reset(FUJIAN_TASK_RUN_T *runTaskInfo);
void taskmng_fujian_task_creat(uint8 Pipe);

void taskmng_fujian_task_run ( uint8 CommPipe );
int taskmng_trans_task_deal(uint8 Pipe);
void taskmng_trans_task_run_retry(uint8 pipe,FUJIAN_TASK_RUN_T *runTaskInfo);
uint8 get_last_buf(uint8 pipe,uint8* lastbuf,uint16 *lastbuflen,FUJIAN_TASK_RUN_T * pAddTaskRunInfo,uint16 nodeid,OOP_TSA_T *add);
uint8 packet_verific_Ispass(uint8 pipe,OOP_RECV_MESS_PROC_T *verific,FRAME_CONVERT_T *convert,FUJIAN_TASK_RUN_T *pAddTaskRunInfo,FUJIAN_SPECIFIC_TASK_T *currruntask, OOP_TSA_T *add);
uint16 taskmng_data_intercept(uint8 *buf,uint16 buflen,OOP_RECV_MESS_PROC_T * verific);

void taskmng_trans_task_record_file_write(uint8 pipe,FUJIAN_TASK_RUN_T* RunTask);
int taskmng_trans_task_result_save(uint8 pipe,uint8 isreport,OOP_TSA_T *tsa,time_t exectime,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *currentSpecificTask,uint8 errorCode,uint8 *databuf,uint16 datalen);
int taskmng_task_trigger(uint8 pipe,uint16 taskId,OOP_TSA_T *tsa);
void taskmng_trigger_task_poweron_check(uint8 pipe);
void taskmng_trigger_task_yxchange_check(uint8* buf,uint16 len);
void fujian_event_report_mqtt(uint8* buf,uint16 len);
void taskmng_trans_task_collection_state_update(uint8 pipe,FUJIAN_TASK_RUN_T * runTask,FUJIAN_TASK_STATE_CMD_E cmd,void* inbuf);
void taskmng_pipe_collection_state_write(uint8 pipe,FUJIAN_TASK_TYPE_E type,uint16 taskid,uint8 priority);
uint16 Get_698framebuf_check(uint8 *Buffer,uint16 DataLength,uint8 *crtl,uint8 *apdu);
int taskmng_trans_task_run_check(uint8 Pipe, FUJIAN_TASK_RUN_T **TaskInfo);
void fujian_task_log_event_repoort(uint8 pipe,char *logfile);
uint16 respons_645_ctrl(uint8 pipe,uint8 resMessType,uint8 *buf,uint16 buflen,uint8 *outbuf);

void taskmng_grop_meter_file_update_check(DB_UPDATE_T *dbupdate);
void taskmng_grop_meter_file_ergodic(FUJIAN_grop_meter *oop_grop_meter);
void fujian_collection_update_deal(uint8* buf,uint16 len);
void taskmng_parallel_task_meter_fail(uint8 Pipe,uint8 SeqIndex,FUJIAN_TASK_RUN_T * runTask);
int taskmng_trans_task_error_report(uint8 pipe,OOP_TSA_T *tsa,time_t exectime,FUJIAN_TASK_RUN_T * runTask,FUJIAN_SPECIFIC_TASK_T *currentSpecificTask);
void taskmng_trans_task_isreport_check(uint8 pipe,FUJIAN_TASK_RUN_T * runTask);
void taskmng_trans_task_sort(uint8 Pipe);


extern uint8 taskmng_mtr_pipe_get(OOP_OAD_U *oad);
extern uint16 taskmng_ms_list_get_by_mettype(OOP_MSP_T *inMS, MS_INFO *pMsInfo,uint8 logicid);
extern int taskManager_acMeter_revsnd(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen);

#ifdef __cplusplus
}
#endif
#endif
