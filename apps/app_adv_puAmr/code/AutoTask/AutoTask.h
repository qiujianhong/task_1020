/*
*********************************************************************
* @file    AutoTask.h
* @brief�� �����ϱ�ģ��ͷ�ļ�
* @author������
* @date��   2019.11.11
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/

#ifndef __AUTO_TASK_H__
#define __AUTO_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_ID_NUM         256       //-ÿ��������֧�ֵ����ID����-
#define AUTOTASK_MAX_NUM    64     //�ն�֧�ֵ�ÿ�������������������

#define TMN_TIME_TICK_BEGIN     0
#define TMN_TIME_TICK_END       24

/** @brief �ϱ����� */
typedef enum
{
    RPT_TYPE_TASK   = 0,   /* �����ϱ� */
    RPT_TYPE_EVENT  = 1,   /* �¼��ϱ� */
    RPT_TYPE_MTREVT = 2,   /* ���ܱ��¼��ϱ� */    
    RPT_TYPE_RETRY =  3,   /* ���� */     
    RPT_TYPE_UNDEF =  4,   /* δ���� */      
}RPT_TYPE_E;

/** @brief ��վ��Ӧ״̬ */
typedef enum
{
    RPT_RES_INITIAL = 0, /* ��ʼ�� */
    RPT_RES_ALREADY = 1, /* �Ѿ���Ӧ */
    RPT_RES_WAITING = 2, /* �ȴ���Ӧ */
    RPT_RES_NONEED =  3  /* ������Ӧ */
}RPT_RES_STATUS_E;

/** @brief �ϱ����� */
typedef struct 
{
    RPT_TYPE_E       type;         /* �ϱ����� */
    RPT_RES_STATUS_E *resStatus;   /* �ϱ���Ӧ״̬ */
    const char       *filename;    /* ��¼�ļ���*/
    uint16           maxRecord;    /* ����¼��*/
    BOOLEAN             retryTag;     /* �ط����*/  
    uint8  *piid;                  /* piid*/
    time_t  lstTime;               /* �ϴ�ʱ��*/
    time_t  curTime;               /* ����ʱ��*/
}RPT_CFG_T;

/** @brief �������� */
typedef struct 
{
    uint16       timeOut;      /* ��ʱʱ�� */
    uint8        retryCnt;     /* ���Դ���*/
}RPT_RETRY_T;


/** @brief ��Ӧ����*/
typedef enum
{
    RPT_RES_PORC_NEXT = 0,     /* ����һ�� */
    RPT_RES_PORC_RETRY = 1,    /* �ط����� */
    RPT_RES_PORC_WAITING =  2  /* �ȴ���ʱ */
}RPT_RES_PROC_E;

typedef struct 
{
    uint8  taskIndex;  //�ϱ��õ������ڲ�ID
    uint8 *pOutBuf;   //�ϱ����ݻ�����
    uint16 bufLen;    //�ϱ����ݻ���������
    uint16 msgLen;    //�ϱ�����ʵ�ʳ���
}RPT_RESULT_T;

/*�ж��Ƿ���Ա����ϱ�����Ӧ*/
#define RPT_TASK_IS_MY_RES(piid)  ((piid&0x3f) == (TaskGLBpiid&0x3f))

#define RPT_EVENT_IS_MY_RES(piid)  ((piid&0x3f) == (EventGLBpiid&0x3f))


/** @brief �������ݽṹ*/
typedef struct                      
{
    uint8 bConflag;                     //����ȷ�ϱ�־    0 ����Ҫȷ��   1 ȷ��
    long  timeOut;                      //��ʱʱ��
    uint8 retryTime;                    //�ط�����
    uint16 bLen;                        //-��¼����-
    uint8  piid;                        //-piid���,�����ж�ȷ��֡-
    uint8  taskIndex;                      //�����
    uint8 bData[8192];  //-��¼��������-

}AUTO_DATA;
/*
        NEW_TASKDATAFILE �ļ��洢��ʽ��
        ��������������������������������������������������
        ��  ��¼������2�ֽڣ�            ��
        ��������������������������������������������������
        �� ���ϱ�������2�ֽڣ�            ��
        ��������������������������������������������������
        ��       ��������1           ��
        ��������������������������������������������������
        ��       ��������2           ��
        ��������������������������������������������������
        ��         ......        ��
        ��������������������������������������������������
        ��       ��������N           ��
        ��������������������������������������������������
*/
#define RPT_RECORD_FILE     "rpt_record_file"            //�������ݵĴ洢�ļ�
#define RPT_RECORD_MAX       1200                        //�������ݴ�ŵ��������
#define RPT_RECORD_CNT_OFFSET    0                       //��¼������ƫ��λ��
#define RPT_REPORT_CNT_OFFSET    2                       //���ϱ�������ƫ��λ��
#define RPT_TASK_MIN_TIMEOUT     5                       //�����ϱ�����Сʱ��

//�����ֵԼ��������ֵ
#define RPT_ADD_VALUE_CLEAR(count, mcount, value) ((count) + (value) >= (mcount) ? 0 : (count) + (value))
//��������
#define RPT_RECORD_DATA_OFFSET(index)   (4 + (index) * sizeof(AUTO_DATA))

//���ܱ��¼��ϱ�
#define RPT_MTREVT_IS_MY_RES(piid)  ((piid&0x3f) == (EventGLBpiid_met&0x3f))

#define RPT_MTREVT_REC_FILE     "rpt_mtrevt_rec_file"   

#define RPT_MTREVT_REC_MAX       200         //�¼��ϱ�����¼����

#define RPT_MTREVT_RECBUF_MAX    2048        //�¼��ϱ�������󳤶�
        
typedef struct
{
    uint32              dataLen;                          //���ݳ���
    uint8               rptType;                          //�ϱ�����
    uint8               pdataBuf[RPT_MTREVT_RECBUF_MAX];  //�ϱ�����
}RPT_MTREVT_INFO;

#define RPT_MTREVT_REC_OFFSET(index)   (4 + (index) * sizeof(RPT_MTREVT_INFO))

//�ն��¼��ϱ�
#define EVENT_RECORD_MAX_LEN    1024        //�¼��ϱ�������󳤶�

#define EVENT_RECORD_FILE     "event_record_file"  //�¼������ϱ��Ĵ洢�ļ�

#define EVENT_RECORD_MAX_NUM    200         //�¼��ϱ�����¼����

#define RPT_EVENT_RECORD_OFFSET(index)   (4 + (index) * sizeof(EVENT_REPORT_INFO))

/** @brief �ϱ�������Ϣ */
typedef struct tag_RPT_PLAN_CFG
{
    OOP_OAD_U  serviceOAD;
    uint16  apduLength;//�����apdu����
    uint8  apduValue[500];//�����apdu
    OOP_PLAN_REPORT_T plan;
    BOOLEAN   isNeedUpdateTIme;//����ʱ���Ƿ���Ҫ��Ϊ�޸�ʱ��
    uint16 TimeOffset;//��Ҫ�޸�ʱ���ƫ��
    CHANNEL_CACHE_T chann; //�ٲ�ͨ����Ϣ
    BOOLEAN   isModify;    //�Ƿ��޸�
}RPT_PLAN_CFG_T;

typedef enum
{
    TASK_CFG_UNINIT,              //δ��ʼ��
    TASK_CFG_MODIFY,              //�޸�
    TASK_CFG_MODIFY_DELAY,        //�޸���ʱ
    TASK_CFG_INITED,              //�ѳ�ʼ��
    TASK_CFG_REBUILD,             //�ؽ������ļ�
    TASK_CFG_PATCH,     //���������ļ�
} TASK_CFG_STATE;//��������״̬

typedef struct _PRO698_TASK_CFG
{
    uint8   TaskId;                 //����id
    uint16  RunCycle;               //ִ��Ƶ����ֵ
    uint8   RunCycleUnit;           //ִ��Ƶ�ʵ�λ
    ePlanType SchemeType;           //�ɼ���������
    uint8   SchemeId;               //�������
    char    RunBeginTime[7];        //����ִ�п�ʼʱ��
    char    RunEndTime[7];          //����ִ�н���ʱ��
    uint16  DelayTime;              //��ʱʱ����ֵ
    uint8   DelayTimeUnit;          //��ʱʱ�䵥λ
    uint8   Priority;               //�������ȼ�
    uint8   TaskState;              //״̬ ����(1),ͣ��(2)
    uint16  PreRunScript;           //����ʼǰ�ű�id
    uint16  OverRunScript;          //���������ű�id
    uint8   TimePeriordType;        //ʱ������
    uint8   TimePeriordNum;         //ʱ�θ���
    uint8   Periordhhmm[192];       //48*4�ֽ�,��������ʱ��hh:mm~hh:mm
    char    ThisTurnBeginTime[7];    //����ִ�п�ʼʱ��
    char    ThisTurnEndTime[7];     //����ִ�н���ʱ��
    char    NextTurnBeginTime[7];   //����ִ�п�ʼʱ��
    char    LastTurnBeginTime[7];   //��һ��ִ�п�ʼʱ��
    BOOL    thisTurnReportFlag;     //�����ϱ���־
    BOOL    NeedToRun;              //�����ϱ������Ƿ�ִ��
    BOOL    isNowCb;                //�Ƿ���������--����������
    time_t   lastEndTime;           //�óɹ�ʱ�������ݵĽ���ʱ��
#if defined(AREA_SHANDONG) || defined(AREA_ANHUI) 
    uint16 normalTaskId;            //��Ӧ����ͨ�ɼ�����id
    uint8  checkTaskFinish;          //�ڱ��ֿ�ʼ����ʱʱ��֮�� ��Ҫ��������Ƿ��� 0������Ҫ 1������Ҫ 2�����Ѿ������
    time_t lastCheckTime;           //�ϴη��Ͳ�ѯ60340200��ʱ��
#endif
}OOP_TASK_CFG;

typedef struct
{
    pthread_rwlock_t    rwLock;                  //��д�� ���̷߳���
    TASK_CFG_STATE      TaskCfgState;         //���񵵰�״̬
    int                 ProCfgModifyDelaySec;  //���������޸���ʱ
    time_t              FunLastRunTime;        //�����ϴ��޸�ʱ��
    uint8               TaskNum;               //�����ܸ���
    OOP_TASK_CFG        OopTaskCfg[256];       //����������
}OOP_TASK_CFG_LIST;

#define RPT_BUF_MAX  (2048)

/** @brief �ϱ����� */
typedef struct 
{
    uint16   timeOut;      /* ��ʱʱ�� */
    uint32   taskID;       /* �ϱ������*/
    uint16   dataLen;      /* �ϱ����ĳ��� */ 
    uint8    planID;       /* �ϱ�������*/
    uint8    priority;     /* �ϱ����ȼ�*/
    uint8    retryCnt;     /* ���Դ���*/    
    uint16   mergeNum;     /* �ϲ�����*/
    OOP_DATETIME_S_T    storeTime;      /**< �ɼ��洢ʱ��       */
    uint8    databuf[0];   /* �ϱ��������� */
}RPT_FIFO_T;

typedef struct 
{
    uint8  idxNum;             /* �����ϱ�������*/
    uint32 index[256];         /* �����ϱ�����*/ 
    uint16 timeOut;            /* �趨�ĳ�ʱʱ�� */
    uint32 taskID;             /* �����ϱ������(��Ϊ͸���¼���¼��Ԫ)*/
    uint16 rtrlen;             /* �����ϱ�����*/
    uint16 rptlen;             /* �����ϱ�����*/
    uint8  planID;             /* �����ϱ�������*/
    uint8  priority;           /* �ϱ����ȼ�*/
    uint8  retryCnt;           /* �趨�����Դ���*/    
    uint16 mergeNum;           /* �����ϱ��ϲ���͸�������*/
    OOP_DATETIME_S_T   storeTime; /**< �ɼ��洢ʱ��       */
    uint8  rtrbuf[RPT_BUF_MAX];   /* �����ϱ����ݣ����ڴ油����*/
    uint8  rptbuf[RPT_BUF_MAX];   /* �����ϱ����ݣ�����698���ģ�*/
}RPT_DATA_T;

/** @brief �ϱ�״̬*/
typedef enum
{
    RPT_NEXT = 0,     /* ����һ�� */
    RPT_RETRY = 1,    /* �ط����� */
    RPT_WAIT =  2     /* �ȴ���ʱ */
}RPT_STATUS_E;

/** @brief �ϱ����� */
typedef struct 
{
    RPT_TYPE_E        type;         /* �ϱ����� */    
    RPT_STATUS_E      status;       /* ����״̬ */
    RPT_RES_STATUS_E *resStatus;    /* �ϱ���Ӧ״̬ */
    BOOLEAN           sendTag;      /* ���ͱ��*/      
    BOOLEAN           retryTag;      /*���Ա��*/  
    uint8             *piid;        /* ��ǰpiid*/
    time_t            lstTime;      /* �ϴ�ʱ��*/
    time_t            curTime;      /* ����ʱ��*/
    DB_CLIENT         dbClient;     /* �������ľ��*/
    RPT_DATA_T        rptData;      /* �����ϱ�����*/
    uint16            timeOut;      /* ��ǰ��ʱʱ�� */
    uint8             retryCnt;     /* ��ǰ���Դ���*/    
    uint32            listID;       /* ����ID*/ 
    uint32            sublistID;    /* �Ӷ���ID*/ 
    uint8             chID;         /* �ϱ�ͨ��ID*/ 
    RPT_TYPE_E        realType;     /* ʵ���ϱ����� */
}RPT_ENTRY_T;

#ifdef AREA_FUJIAN

/** @brief �¼��ϱ��������� */
typedef struct 
{
    OOP_OADS_T          channel;        /**< �ϱ�ͨ��                       */
    uint8               priority;       /**< �ϱ����ȼ�                      */
    OOP_TI_T            timeout;        /**< �ϱ���Ӧ��ʱʱ��                   */
    uint8               retry;          /**< ����ϱ�����                     */
}RPT_EVENT_PLAN_T;

/** @brief ͸�����񷽰��ɼ�״̬���� */
typedef struct 
{
    uint16              taskid;         /**< ͸�����񷽰���                    */
    OOP_DATETIME_S_T    storeTime;      /**< �洢ʱ��                       */
    uint16              reportNum;      /**< �ϱ�����                       */
    uint16              confirmNum;     /**< ȷ������                       */
}RPT_TRANSTASK_STATE_T;

#define RPT_REPORT_LISTID (0xFF200A00)  //�ϱ�����
#define RPT_RETRYP_LISTID (0xFF200B00)  //��������
#define RPT_EVT_SUBLISTID (0x6E310200)  //�¼��Ӷ���

/** @brief ��־������Ϣ*/
typedef struct
{
    uint16                 transTaskid;
    OOP_DATETIME_S_T       storeTime;
    OOP_DATETIME_S_T       readTime;
    char                   filename[128];    //��־�ļ���
}OOP_RPT_LOG_T;

/** @brief ��־������Ϣ*/
typedef struct
{
    uint16                 num;            //��Ҫ�����ĸ���
    OOP_RPT_LOG_T          fileinfo[256];      //�貹������־��Ϣ
}OOP_RPT_LOGS_T;

/** @brief ��־����*/
typedef struct
{
    uint16                 rdptr;                 //������
    uint16                 wtptr;                 //д����
    OOP_RPT_LOG_T          fileinfo[256];      //�貹������־��Ϣ
}RPT_LOGFIFO_T;

/** @brief ��־����*/
typedef struct
{
    pthread_mutex_t        mutex;                 //�� ���̷߳���
    RPT_LOGFIFO_T          fifo;
}OOP_REPORT_LOG_T;

#define RPT_LOGREC_FILE     "rpt_logrec_file"           //ͨ����־��Ϣ����
#define RPT_LOGREC_MAX       256                        //ͨ����־�������

/** @brief ��־����*/
typedef struct
{
    uint16                 valid;                 //�Ƿ���Ч
    uint16                 evtconfirm;            //�¼�ȷ����
}RPT_STAT_T;

#define RPT_STAT_FILE     "rpt_stat_file"           //�ϱ�ͳ����Ϣ˽���ļ�


/** @brief ��־����*/
typedef enum
{
    RPT_CHECK_NO = 0,
    RPT_CHECK_OK = 1,
    RPT_CHECK_IG = 2,
}RPT_CHECK_E;

void rpt_task_init_fj(void);
void rpt_task_fj_entry(time_t curTime, time_t lstTime);
void rpt_retry_fj_entry(time_t curTime, time_t lstTime);
int rpt_evtstat_read(uint16 *confirmNum);
void rpt_evtstat_clear(void);
void rpt_commlog_qclr(void);

#endif

//�������·��ĵ�������ס ����һ���߳�д�������� ���Ч��
typedef struct
{
    pthread_mutex_t  lock;
    uint8 delay;    //�·�����������һ����ʱ ���ʱ�����д��������
    uint16 meterNum;
    OOP_METER_T meterList[NO_VIR_MTR_MAX_NUM];
}CACHE_METER_LIST_T;

//�����ϱ�
#define RPT_RETRY_IS_MY_RES(piid)  ((piid&0x3f) == (suppGLBpiid&0x3f))

void rpt_task_detect(void);                      //--�Զ������������ӿ�--
void rpt_response_check(RPT_TYPE_E type, uint8 seq);                         //���ȷ�ϱ�־
int rpt_record_write(AUTO_DATA *save_data);
uint32 rpt_task_period_get(OOP_TASK_CFG OopTaskCfg);
void rpt_task_init(void);
void rpt_event_entry(time_t curTime, time_t lstTime);
void rpt_mtr_evt_entry(time_t curTime, time_t lstTime);
void rpt_task_entry(time_t curTime, time_t lstTime);
uint8 rpt_taskstate(int8 RunStat,float  *ClockLimt);
void rpt_timetask_entry(time_t lasttime);
void rpt_ansertime_deal(uint8 *buf,uint16 buflen);
int rpt_channel_check(OOP_OAD_U chOAD, CHANNEL_CACHE_T *ptChannel);
int rpt_event_write(int clientid, void* data, uint16 datalen);
BOOLEAN rpt_channel_search(uint8 *chID);

extern BOOL systime_set(DateTimeHex_t *pDateTime);
void cache_meter_entry(DB_CLIENT client,time_t curtime,time_t lasttime);
    int cache_meter_add(OOP_METER_T *meterFile,uint16 num);
    void cache_meter_init();

#ifdef __cplusplus
}
#endif

#endif /* __AUTO_TASK_H__ */
