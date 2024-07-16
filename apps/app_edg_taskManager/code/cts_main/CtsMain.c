

/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� �ɼ�����������APP������
* @date�� 2019-10-24
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
uint8           gmqttThreadFlag; //mqtt�̱߳�־
uint8           gTaskThreadFlag; //��������̱߳�־
uint8           gTGThreadFlag; //�ܼ����̱߳�־

//HANDLE  UpHandle[10];
uint8           gDownThreadFlag[AMR_THREAD_NUM] =
{
    0
};

//����ͨ���̱߳�־
uint8           gRebootAmrThread[AMR_THREAD_NUM] =
{
    0
};

//���������̱߳�־
uint8           gAmrThreadFlag[AMR_THREAD_NUM] =
{
    0
};

//�Զ������̱߳�־
#ifdef AREA_JIANGSU
AmrThreadArg_T  gCajThreadArg[2];

#endif

AmrThreadArg_T  gAmrThreadArg[AMR_THREAD_NUM];
LockQueue_T *   pAmrRecvDown[AMR_THREAD_NUM]; //�����߳����н��չ����ڴ�ָ��
LockQueue_T *   pAmrSendDown[AMR_THREAD_NUM]; //�����߳����з��͹����ڴ�ָ��
LockQueue_T *   pAmrRecvDown_Cb[AMR_THREAD_NUM]; //�����߳����н��չ����ڴ�ָ��
LockQueue_T *   pAmrRecvUp[AMR_THREAD_NUM]; //�����߳����н��չ����ڴ�ָ��
LockQueue_T *   pAmrSendUp[AMR_THREAD_NUM]; //�����߳����з��͹����ڴ�ָ��

LockQueue_T LockRecvDown[AMR_THREAD_NUM];       //���н��ջ������ṹ
LockQueue_T LockSendDown[AMR_THREAD_NUM];       //���з��ͻ������ṹ
LockQueue_T LockSendDown_Cb[AMR_THREAD_NUM];    //���з��ͻ������ṹ
Down_ThreadArg_T DownThreadArg[AMR_THREAD_NUM];

LockQueue_T LockRecvUp[AMR_THREAD_NUM];         //���н��ջ������ṹ
LockQueue_T LockSendUp[AMR_THREAD_NUM];         //���з��ͻ������ṹ
ThreadArg_T UpThreadArg[AMR_THREAD_NUM];        //�����̲߳���

LockQueue_T     gMeterSearchQueue; //�ѱ��õĶ���

#ifdef AREA_HUNAN
LORA_AMR_T      g_LoRaAmr;
extern OOP_OAD_U gTemperatureOad;
extern OOP_OAD_U gSmokeOad;
extern OOP_OAD_U gWaterOad;
extern OOP_OAD_U gTransTempOad;
extern uint8    gTemperatureAddr; //��ʪ�ȴ�������ַ
extern uint8    gTemperaturePort; //��ʪ�ȴ������˿�

extern uint8    gSmokeAddr; //����������ַ
extern uint8    gSmokePort; //�˿�

extern uint8    gWaterAddr; //ˮ����������ַ
extern uint8    gWaterPort; //�˿�

extern uint8    gTransTempAddr[6]; //��ѹ��׮ͷ�¶ȴ�������ַ
extern uint8    gTransTempPort; //�˿�

#endif

#ifdef AREA_FUJIAN
FUJIAN_grop_meter oop_grop_meter; //Ⱥ���ַ�ṹ��
DBUPDATE_QUEUE_T pGropMeterFileUpdate; //��������¼�
OOP_GATHER_E    gGatherMode;
uint8           g485ShareFlag; //485-2�����ݹ�����

#endif

DBUPDATE_QUEUE_T pTaskDBUpdate; //�洢������߷��������ݱ���¼�
DBUPDATE_QUEUE_T pMeterFileUpdate; //��������¼�
DBUPDATE_QUEUE_T pTaskRecordFileDel[AMR_THREAD_NUM + 1]; //�����Ҫɾ���������¼�ļ���id

//��������ִ�ж���
SCHEME_TASK_RUN_INFO SchemeTaskRunQueue[SCHEME_MAX_NUM]; //�ز������������
SCHEME_TASK_RUN_INFO * SchemeTaskRun_NormalLink = NULL; //��ǰ�ز������ճ�������

//���������������
OOP_TASK_CFG_LIST SchemeTaskCfgList; //�ز�������������б�

//��������
Meter_File_T    MeterFileAll; //�ն˵�����Ϣ
uint8           MeterFileFlag; //�����޸ı�־
pthread_rwlock_t TaskRWLock; //�����б��д�� ���̷߳���
pthread_mutex_t SeqLock;

extern DateTime_T gCurBinTimeTask; //��ǰBINʱ��

extern DateTime_T gPreBinTimeTask; //�ϴ�BINʱ��
char *          DownName; //�������е���Ϣ���ͷ�����
char *          CCOName;
char *          WIRELESSName;
ThreadArg_T     APPArg; //������APP�����Ľṹ��
uint8           LocalAddr[6] =
{
    0
};

//�ն˵�ַ
uint8           IsSetAddr[PLC_NUM] =
{
    0
};

//�Ƿ���������ù��ն˵�ַ
//uint16 SearchMeterSeq = 0;  //�ѱ���߳��ϱ����ݺ�ͳһ�����ѱ���Ϣ���
//pthread_mutex_t SearchMeterSeqLock;
uint16          NetTopologySeq = 0; //�洢����������Ϣʹ�õ���Ϣ���
pthread_mutex_t NetTopologySeqLock;

#if ( defined AREA_JIANGXI) || (defined AREA_ANHUI) || (defined AREA_JIANGSU)
const OOP_OAD_U PerceptionTerminalOad =
{
    0xFF110E00
};

//��֪�ն���Ϣ
#endif

const OOP_OAD_U EventReportOad =
{
    0xFF100A00
};

//����¼��洢�õ�oad
const OOP_OAD_U EventReportOad645 =
{
    0xFF100B00
};
const OOP_OAD_U MinuteReportOad =
{
    0xFF100D00
};

//����¼��洢�õ�oad
const OOP_OAD_U AcrossAreaOad =
{
    0X60020500
};

//�Ǳ�̨��������洢OAD
const OOP_OAD_U NodeRejectOad =
{
    0XF2091500
};

//�ڵ�ܾ���Ϣ�洢oad
const OOP_OAD_U PowerOnOffOad =
{
    0xFF100A00
};

//ͣ�ϵ��¼��洢oad ������0C00
const OOP_OAD_U MtrTimingOad =
{
    0xFFA00100
};

//���Уʱ��������Ϣ
const OOP_OAD_U ClockOutOad =
{
    0xFFA00200
};

//���ʱ��ƫ��
//const OOP_OAD_U  IdenyifyResultOad = {0x3E020200};  //���Уʱ��������Ϣ
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

DB_UPDATE_T dbupdateMeter; //�洢�յ��ĵ��������Ϣ
DB_UPDATE_T dbupdateTask;

EVENT_PLAN_T EventPlanList; //�洢�ɼ�����Ϊ2�������¼��ɼ�����

OOP_PLAN_Transparent_LIST_T oopPlanTransparentList; //͸��������������


TASK_MONITOR_T TaskMonitor; //������6034

SEARCH_METER_CLEAR_T SearchMeterClean; //60020800 �ѱ������ѡ��

SEARCH_METER_CYCLICITY_T SearchMeterParam; //�����ѱ����

SYNC_TIMING_T SyncTimingParam; //�㲥Уʱ��ز���

NODE_UPDATE_CYCLE_T UpdateCycle; //0XF2090600��������

ACROSS_AREA_T AcrossAreaResult; //�Ǳ�̨�����ܱ��ϱ���Ϣ���

METER_SEARCH_RES_T MeterSearchResult; //�ѱ���

FILE_UPDATE_ENCRYPT_T gFileUpdateEncrypt;       //ת���ܵ���������

int gEncryptPipe; //����һ��ת��������ı��� Ĭ����-1 ����ж˿���ִ��ת�����������λ ��Ķ˿ھͲ���ִ������

CCO_STA_INFO_T ccostainfo;
uint8 Rs485PipeConut = 0;

uint8 MeterSearchResultClearFlag = 0; //�ѱ�����ձ�־ ĳ�߳���պ������߳��������
uint8 Modulepullflag[5] =
{
    0
};

//ģ��γ���־;�����һ��Ԫ�ش���ģ��1���������ƣ������Ԫ�ش���ģ��5;
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

DB_AGENT gDBAgent = 0; //���������Ľ�����agent
DB_CLIENT gDBClientMain = 0;
DB_CLIENT gDBClientTask = 0;
pthread_mutex_t gMqttSendLock;
int gMqttSock = 0; //mqtt�̴߳������̼߳�ͨ��fd
int gMainSock = 0; //���̴߳�����
int gTaskSock = 0; //task�̴߳�����

TotalGroupThreadArg TGThreadArg; //�ܼ��鴦���߳�ʹ�õĽṹ��
uint8 gSafeButton = 0;
uint8 gSafeButtonHPLC = 0; //�ز�Ĭ�ϵ�MACУ�鿪�� Ĭ�Ϲر�
uint8 ReloadTaskFlag = 0; //�����������Ϊ1 �����̼߳������³�ʼ������
uint8 DataInitFlag[AMR_THREAD_NUM + 1] =
{
    0
};

//��������ʼ����� ÿ�������߳���ռ�¼���¼����
uint8 DataInitFlagMain = 0;
uint8 gRouterStudy = 0;
uint8 gMinuteCollect = 0;

#if ( defined AREA_JIANGXI) || (defined AREA_ANHUI)||(defined AREA_JIANGSU)
OOP_PERCEPTION_TERMINAL_LIST_T perceptionterminal;
#endif

OOP_OCTETVAR16_T tmnAddr; //�ն�ͨ�ŵ�ַ
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
OOP_MODULE_INFO_NEW_T gModuleInfo; //ģ����Ϣ

#endif
uint8 gdelay=0;
/*******************************************************************************
* ��������: AlreadyRunning
* ��������: �������Ƿ�����
* ���������TaskName            ������ָ��
* �����������
* �� �� ֵ: 1                   ������������
*           0                   ����δ����
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
        return 0; //�ļ��򲻿�����ʾ����δ��ִ��
    }

    locker.l_type = F_WRLCK;
    locker.l_start = 0;
    locker.l_whence = SEEK_SET;
    locker.l_len = 0;

    if(fcntl(fd, F_SETLK, &locker) < 0)
    {
        close(fd);
        return 1; //������������ʾ�����Ѿ�ִ��
    }

    ftruncate(fd, 0);
    pid = getpid();
    write(fd, &pid, sizeof(pid_t));

    return 0;
}

/**********************************************************************
* @name      : MntVersionCheck
* @brief     ���ն�ͨ�ŵ�ַ����
* @param[in] ��OOP_OCTETVAR16_T tLocalAddr ��ǰͨ�ŵ�ַ
* @param[out]��
* @return    ��
* @Create    : ÷����
* @Date      ��2020-02-16
* @Update    :
**********************************************************************/
void GetMntAddr(OOP_OCTETVAR16_T tLocalAddr)
{
    //�ն�ͨ�ŵ�ַ
    tmnAddr = tLocalAddr;
    MAC.nNum = 6;
    memcpy(MAC.value, tmnAddr.value, MAC.nNum);

}

/**********************************************************************
* @name      : evt_read_nomal_data
* @brief     ����ȡ��ͨ������
* @param[in] ��int    clientid     �������ľ��
               uint8 logicId   �߼��豸��
               uint16 mtr_id   ��Ϣ���
               uint32 value_oad OAD
               uint16 datalen   ���ݳ���
* @param[out]��uint8* pout_buf   ����
* @return    ��0-�ɹ�/������
* @Create    : ÷����
* @Date      ��2021-1-26
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
* @brief     ����ȡ�ն˵�ַ
* @param[in] ��int             clientid     �������ľ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ÷����
* @Date      ��2021-1-26
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
* ��������: taskmng_global_init
* ��������: ȫ�ֳ�ʼ��
* �������:
* �������:
* �ڲ�����:
* �� �� ֵ: -1 ��ʼ��ʧ��
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
        TASK_FMT_TRACE(-1, MQTT_LOG, "����dbagentʧ�ܣ��� \n");
        return - 1;
    }

    //mqtt��ʼ��
    TASK_FMT_DEBUG(-1, MQTT_LOG, "mqtt ��ʼ��ʼ��������\n ");
    taskmng_mqtt_init();
    gDBClientMain = db_client_create(DB_AGENT_CTS, DB_CLIENT_MAIN);

    if(gDBClientMain < 0)
    {
        TASK_FMT_TRACE(-1, MQTT_LOG, "����main dbclientʧ�ܣ��� \n");
        return - 1;
    }

    //��ʼ��������Ϣ
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
* ��������: main
* ��������: �̴߳�����ά��
* �������:
* �������:
* �ڲ�����:
* �� �� ֵ: ��
*******************************************************************************/
int main(int argc, char * argv[])
{
    uint8 i = 0;
    int32 err;


    pthread_t MqttTheadID;                          //mqtt�̱߳�ʶ

#ifndef AREA_FUJIAN
    memset(&TGThreadArg, 0x00, sizeof(TotalGroupThreadArg));
    pthread_t TGThreadID;
#endif

    pthread_t AmrThreadID[AMR_THREAD_NUM];          //�Զ������̱߳�ʶ

#ifdef AREA_HUNAN
    pthread_t LoRaThreadID;                         //LoRa�����̱߳�ʶ
#endif

    pthread_t TaskThreadID;                         //��������̱߳�ʶ


    uint16 MeterCount = 0;

    LockQueue_T LockSendApp;                        //������APP�������͵���Ϣ
    LockQueue_T LockRecvApp;                        //������APP�����յ�����Ϣ

    LockQueue_T LockSendTg;                         //�����߳���
    LockQueue_T LockRecvTg;                         //�����߳���
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

    //�������Ƿ�������
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

    /* �źŴ��� */
    init_sig_action("/data/app/taskManager/log/Exception.log");



    if(taskmng_global_init() < 0)
    {
        TASK_FMT_DEBUG(-1, UP_LOG, "��ʼ��ʧ��\n");
        exit(1);
    }
    TASK_FMT_DEBUG(-1, UP_LOG, "@@@@@@@@@@@taskManager��ʼ����@@@@@@@@@@@@@@@ \n");
    TASK_FMT_DEBUG(-1, UP_LOG, "taskManager �汾�� %s �������� %s ���� %s ��Ʒ %s\n", APP_VERSION, APP_PUBLISH_TIME, AREANAME_PLACE,PRODUCT_NAME);

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

    //ÿ���˿ڴ����ز������߳�
    for(i = 0; i < AMR_THREAD_NUM; i++)
    {
        memset(&gAmrThreadArg[i], 0, sizeof(AmrThreadArg_T));
        gAmrThreadArg[i].CommPipe = i;

        //����ͨ���̲߳�����ʼ��
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

        //����IPCͨ�ų�ʼ��
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

    //����������ĺ�ģ��������Ƿ�ע��ɹ�
    while(1)
    {
        if(taskmng_register_app_check(DBCENTER_NAME) == TRUE)
            break;

        TASK_FMT_TRACE(-1, UP_LOG, "��������δע��ɹ� 3������� \n");

#ifdef PRODUCT_ECU

        if(taskmng_register_app_check(MAPMANAGER_APP_NAME) == TRUE)
            break;

        TASK_FMT_TRACE(-1, UP_LOG, "ģ�������δע��ɹ� 3������� \n");

#endif

        sleep(3);
    }
    //������ʼ��esam �Է���ȫapp��û��
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
        TASK_FMT_TRACE(-1, TASK_LOG, "������ȡʧ��\n");
    }

    TASK_FMT_TRACE(-1, TASK_LOG, "������ʼ�����!\n");
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

    //��ȡ�ⲿ�豸�ɼ�ģʽ
    gGatherMode = fujian_gather_mode_get(gDBClientMain);
    g485ShareFlag = fujian_485_share_flag_get(gDBClientMain);

#endif



    //ÿ���˿ڴ����ز������߳�
    for(i = 0; i < AMR_THREAD_NUM; i++)
    {

#ifdef AREA_FUJIAN

        if(gGatherMode == OOP_GATHER_FJ)
        {
            //�����ز���485��һ���߳�
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
                //ֻ�ܰ���485·�����߳� Ҫ��Ȼ��ཨ�����߳� ����485��ز������������ ���յ���Ī��������
                err = pthread_create(&AmrThreadID[i], NULL, AmrThread_485, &gAmrThreadArg[i]);
            }
        }

#else

        //���������߳�
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
        //������������߳� �������������µ������߳�
        err = pthread_create(&TaskThreadID, NULL, thread_task_fujian, NULL);

        if(err != 0)
        {
            printf("pthread_create TaskThread error");
            exit(1);
        }

    }
    else
    {
        //������������߳�
        err = pthread_create(&TaskThreadID, NULL, TaskThread, NULL);

        if(err != 0)
        {
            printf("pthread_create TaskThread error");
            exit(1);
        }
    }

#else

    //������������߳�
    err = pthread_create(&TaskThreadID, NULL, TaskThread, NULL);

    if(err != 0)
    {
        printf("pthread_create TaskThread error");
        exit(1);
    }

#endif

#ifndef AREA_FUJIAN

    //�����ܼ��鴦���߳�
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
    TASK_FMT_TRACE(-1, UP_LOG, "��ѯģ����Ϣ������������ \n");

    if(pthread_mutex_init(&gModuleInfoLock, NULL) != 0)
    {
        printf("init pthread gModuleInfoLock lock error!\n");
        exit(1);
    }

    taskmng_all_module_info_get();
    TASK_FMT_TRACE(-1, UP_LOG, "��ѯģ����Ϣ���������������� \n");
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

        //������Ϣ
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
                        TASK_FMT_TRACE(-1, MQTT_LOG, "�յ�mqtt���͵����ݸ�����Ϣ infonum %d oad %08x !\n", dbupdateMeter.infonum, dbupdateMeter.oad);

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
        //���3����һ���߳�״̬
        if(maincnt > 300)
        {
            //TASK_FMT_TRACE(-1,UP_LOG, "��ʱ����߳�״̬������ \n");
            maincnt = 0;

            for(i = 0; i < AMR_THREAD_NUM; i++)
            {
                if(gAmrThreadFlag[i] == 0)
                {
                    TASK_FMT_DEBUG(-1, UP_LOG, "�����̶߳˿� %d ��������!\n", i);
                }

                gAmrThreadFlag[i] = 0;
            }

            if(gTaskThreadFlag == 0)
            {
                TASK_FMT_DEBUG(-1, UP_LOG, "��������̳߳�������!\n", i);
            }

            gTaskThreadFlag = 0;

            if(gmqttThreadFlag == 0)
            {
                TASK_FMT_DEBUG(-1, UP_LOG, "mqtt�̳߳�������!\n", i);
            }

            gmqttThreadFlag = 0;

            if(gTGThreadFlag == 0)
            {
                //TASK_FMT_DEBUG(-1,UP_LOG, "�ܼ����̳߳�������!\n",i);
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

            //ResetSearchMeterSeq();//�����ѱ����
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "�ϵ��ѱ�����\n");
        }
        else
        {
            TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ǰ�����ѱ��У�����\n");
        }
    }
}

/*******************************************************************************
* ��������: AmrThread
* ��������: �Զ������߳�
* ���������arg                 ָ���������ָ��
* �����������
* �ڲ����������������
* �� �� ֵ: ��
*******************************************************************************/
void * AmrThread(void * arg)
{

    uint8 CommPipe;
    AmrThreadArg_T * pAmrThreadArg;
    uint8 ModuleStat;
    uint16 isfirst;                                 //�Ƿ��״β���ģ��
    uint16 recinfo = 0;                             // ��ʾ��һ���յ�CC0ģ�����Ϣ��
    pAmrThreadArg = (AmrThreadArg_T *)
    arg;
    CommPipe = pAmrThreadArg->CommPipe;
    uint32 sender;
    int RecvLen;
    uint8 RecvBuf[MAX_MSG_LEN];
    char ThreadName[20];
    sprintf(ThreadName, "AmrThread-%d", CommPipe);
    prctl(PR_SET_NAME, ThreadName);
    taskmng_amr_thread_queue_init(arg);             //�̶߳��г�ʼ��
    taskmng_past_time_deal_init(CommPipe);          //ʱ��������ʼ��
    taskmng_all_state_init(CommPipe);               //״̬�ֳ�ʼ��
    taskmng_all_flag_init(CommPipe);                //��־��ʼ��

    memset(pAmrThreadArg->PlcTaskQueue, 0x00, PLCTASK_MAXNUM * sizeof(PLCTASK_QUEUE));
    pAmrThreadArg->PlcTask_RunLink = NULL;

#ifdef AREA_JIANGXI

    /* ��������ͨ���ն˺͵��ͣ�ϵ����ж�ǫ̈̄����ͣ�ϵ� */
    MTR_POWER_ON_OFF_JX mtrPowerOnOff;
    memset(&mtrPowerOnOff, 0x00, sizeof(MTR_POWER_ON_OFF_JX));
#endif

    while(1)
    {
        gAmrThreadFlag[CommPipe] = 1;

        RecvLen = task_recv_msg(pAmrThreadArg->MsgSock, RecvBuf, MAX_MSG_LEN, &sender);

        //��⵱ǰģ���Ƿ��Ѿ�����
        ModuleStat = taskmng_plc_module_check(CommPipe);
#if defined(PRODUCT_ZCU_3) || defined(PRODUCT_ZCU_1)
        //ר����רһ�߳̿�ת
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
            TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ�IPC��Ϣ msg_id %04x RecvLen %d msg->msglen %d \n", msg->msg_id, RecvLen, msg->msglen);

            TASK_BUF_DEBUG(CommPipe, RELAY_LOG, msg->msg, msg->msglen);

            switch(msg->msg_id)
            {
                //            case MSG_SEARCH_METER:
                //            {
                //                TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ��ѱ��ϱ���Ϣ\n");
                //            #ifdef AREA_ZHEJIANG
                //                taskmng_meter_search_result_write_area(CommPipe,msg->msg,msg->msglen);
                //            #else
                //                taskmng_meter_search_result_write(CommPipe,msg->msg,msg->msglen);
                //            #endif
                //            }
                //                break;
                case MSG_EVENT_REPORT:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ�����¼��ϱ�\n");
                        taskmng_event_report_amr_fun(CommPipe, msg->msg, msg->msglen);
                        //�����ܷ�̨�� ccoRouter���յ��ϱ�������»�ֱ�ӽ����ӽڵ��� �յ�����¼��ϱ�ʱ�Ǹ�ʱ�� 3����û���µ��¼��ϱ��ſ�ʼִ������
                        taskmng_meter_event_report_time_set();
                    }
                    break;

                case MSG_HPLC_INFO:
                    {
                        if(recinfo == 0)
                        {
                            recinfo = 1;
                        }

                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ�HPLC��ѯ��Ϣ�ϱ�\n");
                        taskmng_HPLC_info_save(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_NODE_REJECT:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ��ڵ�ܾ���Ϣ�ϱ�\n");
                        taskmng_node_reject_info_save(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_POWER_ON_OFF_COL:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ��ɼ���ͣ�ϵ��¼��ϱ�\n");
                        taskmng_power_on_off_deal_col(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_POWER_ON_OFF_MET:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ����ͣ�ϵ��¼��ϱ�\n");

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
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ��ն�ͣ�ϵ��¼�\n");
                        memset(&mtrPowerOnOff, 0x00, sizeof(MTR_POWER_ON_OFF_JX));
                        taskmng_power_on_off_deal_tmn(pAmrThreadArg, msg->msg, msg->msglen, &mtrPowerOnOff);
                    }
                    break;

#endif

                //            case MSG_RECORD_FILE_DEL:
                //            {
                //                uint16 taskid;
                //                memcpy(&taskid,msg->msg,2);
                //                TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ����� %d �ı����Ϣ\n",taskid);
                //                taskmng_record_file_delete_pipe(CommPipe,taskid);
                //            }
                //                break;
                //            case MSG_HPLC_CHANNEL_CHANGE:
                //            {
                //                uint8 channel = msg->msg[0];
                //                TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ�Ƶ�����õı����Ϣ Ƶ��Ϊ %d \n",channel);
                //                SetHPLCChannel(CommPipe,channel);
                //            }
                //                break;
                case MSG_HPLC_ACROSSAREA_CHANGE:
                    {
                        uint8 flag = msg->msg[0];
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ�̨��ʶ�𿪹صı����Ϣ ����Ϊ %d \n", flag);
                        taskmng_HPLC_across_area_flag_set(CommPipe, flag);
                        taskmng_HPLC_off_grid_set(CommPipe, flag);
                    }
                    break;

                case MSG_HPLC_NODEREJECT_CHANGE:
                    {
                        uint8 flag = msg->msg[0];
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ��ܾ��ڵ��ϱ����صı����Ϣ ����Ϊ %d \n", flag);
                        taskmng_HPLC_node_reject_set(CommPipe, flag);
                    }
                    break;

                case MSG_METERFILE_CLEAR:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ����������Ϣ \n");
                        taskmng_cco_meter_file_clear(CommPipe);
                        taskmng_event_report_clear(CommPipe);
                    }
                    break;

                case MSG_CCO_STA_INFO:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ��ӽڵ�汾��Ϣ \n");
                        taskmng_cco_sta_info_write(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_MODULE_INFO:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ�����ģ����Ϣ \n");
                        taskmng_msg_0061_unpack(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_FILE_TRANSFER:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ��ļ�������Ϣ \n");
                        taskmng_file_transfer_msg_amr(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                case MSG_HPLC_UPDATE:
                    {
                        TASK_FMT_DEBUG(CommPipe, RELAY_LOG, "�յ��ز�ģ�������ļ� \n");
                        taskmng_hplc_update_msg_amr(CommPipe, msg->msg, msg->msglen);
                    }
                    break;

                default:
                    break;
            }
        }
        //����ǵ��������Ϣ ������ᵲס������������Ż�ִ��
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
        taskmng_time_manage(CommPipe);              //ʱ�������

        taskmng_task_run_ergodic(CommPipe);         //һ��������һ���п��ܻ�����������ʱ��û�жϵ������

        taskmng_plc_task_creat(CommPipe);           //����Ĳ����ж�

        taskmng_plc_task_run(CommPipe);             //�����ִ�д���

        /* �ж�����û��û�ϱ���ͣ�ϵ��¼� */
                /* �ж�����û��û�ϱ���ͣ�ϵ��¼� */
        taskmng_meter_power_cache_check(CommPipe);
        taskmng_meter_power_report_check(CommPipe);
        /*û�������ʱ���sleep  */
        if(gAmrThreadArg[CommPipe].PlcTask_RunLink==NULL)
            usleep(100000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
#ifdef AREA_JIANGXI
        PowerOnOffEventReport_Jx(CommPipe, &mtrPowerOnOff);
#endif
        /* ����sleep ����cpuռ�� */
        //usleep(3000);
    }

    pthread_exit(NULL);
}

/*******************************************************************************
* ��������: AmrThread_485
* ��������: �Զ������߳�
* ���������arg                 ָ���������ָ��
* �����������
* �ڲ����������������
* �� �� ֵ: ��
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
    taskmng_amr_thread_queue_init(arg);             //�̶߳��г�ʼ��
    taskmng_past_time_deal_init(CommPipe);          //ʱ��������ʼ��

    taskmng_all_state_init(CommPipe);               //״̬�ֳ�ʼ��
    taskmng_all_flag_init(CommPipe);                //��־��ʼ��
    pTaskRunInfoinit(CommPipe);                     //����ʱ�ķ�����������ŵ���Ϣ��ʼ��

    cbtasknew_TaskCb(CommPipe);
    pthread_exit(NULL);
}

/*******************************************************************************
* ��������: UpThread
* ��������: ������Ӧ�ý��̽���ͨ��
* ���������arg                 ָ���������ָ��
* �����������
* �ڲ�����
* �� �� ֵ: ��
*******************************************************************************/
void * UpThread(void * arg)
{
    /*
        prctl(PR_SET_NAME,"UpThread");
        //uint8               pipe = 0;
        uint8               i;
        LockQueue_T         *pLockRecv;                     //�ز������ջ�����ָ��
        LockQueue_T         *pLockSend;                     //�ز������ͻ�����ָ��
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
        paraUdp.srcPort = UP_PORT; //�Լ��Ķ˿�
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

                //���ý�����Ϣ�ӿ�
                ret= msg_udp_read(UpHandle, &MsgRecv);
                if(ret==0)
                {
                    RecvLen = sizeof(MSG_INFO_T) +MsgRecv->MsgLen;
                    uint8 pipe ;
                    TASK_FMT_TRACE(-1,UP_LOG,"�����߳̽��� IID %04x IOP %04x msglen %d msgdata \n",MsgRecv->IID,
        MsgRecv->IOP,MsgRecv->MsgLen);

                    TASK_BUF_TRACE(-1,UP_LOG,MsgRecv->MsgData, MsgRecv->MsgLen);

                    if(MsgRecv->IID == 0x1003)
                    {
                        switch(MsgRecv->IOP)
                        {
                            case 0x0010://��������ȡ���ɸ������������ɸ���������
                            {
                                TASK_FMT_TRACE(-1,UP_LOG,"�յ�0x0010  \n");
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
                                TASK_FMT_TRACE(-1,UP_LOG,"�յ�0x0011  \n");
                                //TSAΪoctet-string ��һ���ֽ�Ϊ���� �ڶ����ֽ�Ϊ����ַ���ȼ�1
                                OOP_TSA_T tsa; //����ַ����Ϊ�㲥��ַ ���ߵ�������ַ
                                memcpy(&tsa.af,MsgRecv->MsgData+1,1);
                                memcpy(tsa.add,MsgRecv->MsgData+2,tsa.len+1);
                                pipe = taskmng_pipe_get_from_tsa(tsa);
                                if(pipe!=0xff)
                                {
                                    pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;
                                    //д����Ľ��ջ�����
                                    taskmng_plc_queue_write ( pLockSend, (uint8*)MsgRecv, RecvLen );
                                }
                            }
                                break;
                            case 0x0016://����������͸��ת������
                            {
                                OOP_OAD_U oad ;//ת���˿ڶ���������
                                memcpy_r((uint8 *) &oad, MsgRecv->MsgData , sizeof(OOP_OAD_U));
                                TASK_FMT_TRACE(-1,UP_LOG,"�յ�0x1016 ת���˿�Ϊ 0x%08x \n",oad.value);
                                pipe = taskmng_mtr_pipe_get(&oad);
                                if(pipe!=0xff)
                                {
                                    pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;
                                    //д����Ľ��ջ�����
                                    taskmng_plc_queue_write ( pLockSend, (uint8*)MsgRecv, RecvLen );

                                }
                            }
                                break;
                            case 0x0020://�ز�/΢�������߽ӿ�͸��ת��
                            {
                                OOP_TSA_T tsa; //����ַ����Ϊ�㲥��ַ ���ߵ�������ַ
                                memcpy(&tsa.af,MsgRecv->MsgData+1,1);
                                memcpy_r(tsa.add,MsgRecv->MsgData+2,tsa.len+1);
                                TASK_FMT_TRACE(-1,UP_LOG,"�յ�0x1020 tsa.af %02x \n",tsa.af);
                                //�㲥��ַ ÿ���˿ڶ���Ҫ
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
                                    TASK_FMT_TRACE(-1,UP_LOG,"����ַ pipe %d \n",pipe);
                                    if(pipe!=0xff)
                                    {
                                        pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;
                                        //д����Ľ��ջ�����
                                        taskmng_plc_queue_write ( pLockSend, (uint8*)MsgRecv, RecvLen );
                                    }
                                }
                            }
                                break;
                            case 0x0021: //�ѱ�
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
                        //������Ϣ�ӿڷ�����Ϣ
                        MSG_INFO_T* msg  = (MSG_INFO_T*)SendBuf;
                        TASK_FMT_TRACE(-1,UP_LOG,"���з�����Ϣ IID %04x IOP %04x msglen %d msgdata \n",msg->IID,
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
* ��������: TaskThread
* ��������: ��������߳�
* ���������arg                 ָ���������ָ��
* �����������
* �ڲ�����
* �� �� ֵ: ��
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
* ��������:    taskmng_amr_thread_queue_init
* ��������:    �̶߳��г�ʼ��
* �������:    ��
* �������:    ��
* �� �� ֵ:    ��
*******************************************************************************/
void taskmng_amr_thread_queue_init(void * arg)
{
    uint8 pipe;

    AmrThreadArg_T * AmrThreadArg;                  //�����̲߳���
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

                    printf("��¼��smios������\n");
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

                        //���ӷ��� �Է�������ѭ��
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
                                case 0x0010: //����
                                    {
                                        heart_beat(MsgRecv->index);
                                    }
                                    break;

#ifdef AREA_FUJIAN
                                case 0x0012: //͸������Ŀǰ�������߲��ŵĽ�����ͨ��ģ������
                                    {
                                        pipe = MsgRecv->label; //����ʱlabel��˿ں�
                                        pLockSend = gAmrThreadArg[pipe].DownArg->SendBuf;

                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //��ս��ջ�����
                                            //taskmng_comm_recv_buff_clear(pipe);
                                        }
                                    }
                                    break;

#endif

                                case 0x0014: //������Ϣ
                                    {
                                        MsgErrorFunc(MsgRecv, RecvLen);

                                    }
                                    break;

                                case 0x0013: //�汾��Ϣ
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
                                case 0x0010: //ע����Ӧ
                                    {
                                        printf("�յ�ע����Ӧ!!!! \n");
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

                        else if(MsgRecv->IID == 0x0005) //��������
                        {
                            switch(MsgRecv->IOP)
                            {
                                case 0x0001: //���ݸ����¼�
                                    {
                                        DBUpdateFun(MsgRecv);
                                    }
                                    break;

                                case 0x0002: //���ݳ�ʼ���¼�
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
                                case 0x0010: //͸��ת������
                                    {
                                        OOP_OAD_U oad; //ת���˿ڶ���������
                                        memcpy_r((uint8 *) &oad, MsgRecv->playload, sizeof(OOP_OAD_U));
                                        TASK_FMT_DEBUG(-1, MQTT_LOG, "�յ�0x0010 ת���˿�Ϊ 0x%08x \n", oad.value);
                                        pipe = taskmng_mtr_pipe_get(&oad);

                                        if(pipe >= 0 && pipe < 10)
                                        {
                                            pLockSend = gAmrThreadArg[pipe].UpArg->SendBuf;

                                            //д����Ľ��ջ�����
                                            taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        }
                                    }
                                    break;

                                case 0x0011: //����
                                    {
                                        TASK_FMT_DEBUG(-1, MQTT_LOG, "�յ�0x0011 ���� \n");
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

                                        //�������ɵĴ���͸��
                                        taskmng_plc_queue_write(TGThreadArg.pBufArg->SendBuf, (uint8 *) MsgRecv, RecvLen);
                                    }
                                    break;

                                case 0x0012: //�ѱ�
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

                                                //ResetSearchMeterSeq();//�����ѱ����
                                            }
                                            else
                                            {
                                                TASK_FMT_DEBUG(pipe, RELAY_LOG, "��ǰ�����ѱ��У�����\n");
                                            }
                                            up_msg_response(MsgRecv, 1);
                                        }else
                                        {
                                            up_msg_response(MsgRecv, 0);
                                        }

                                    }
                                    break;

                                case 0x0013: //���ճ���״̬
                                    {
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "�յ�0x0013 ��ѯ���ճ���״̬ \n");
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

                                                //д����Ľ��ջ�����
                                               // taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);
                                                taskmng_amr_status_get_mqtt(pipe,Addr,MsgRecv);

                                                break;
                                            }
                                        }

                                        ErrorUpMsg(MsgRecv); //�յ����ݲ���ȷ �ظ�������
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

                                case 0x0021: //��ѯ�豸�������
                                    {
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "�յ�0x0021 ��ѯ�豸������� \n");
                                        taskmng_file_transfer_result_check(MsgRecv);

                                    }
                                    break;

#ifdef AREA_FUJIAN
                                case 0x0030: //����͸�����񷽰��ɼ�״̬
                                    {
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "�յ�0x0030 ����͸�����񷽰��ɼ�״̬ \n");
                                        fujian_collection_send_recv_update(MsgRecv);

                                    }
                                    break;

                                case 0x0031: //������ϵ���Ϣ֪ͨ
                                    {
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "�յ�0x0031 �ϵ�֪ͨ \n");
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
                                case 0x0031: //����ظ�����
#ifdef AREA_HUNAN
                                case 0x0071:
#endif

#ifdef AREA_FUJIAN
                                case 0x0104:
                                case 0x0105:
                                case 0x0102: //�ϱ��ӽڵ���Ϣ
#endif

                                    {
                                        pipe = MsgRecv->label; //����ʱlabel��˿ں�
                                        pipe = 0;
                                        pLockSend_Cb = gAmrThreadArg[pipe].DownArg->CbSendBuf;
                                        ret = taskmng_plc_queue_write(pLockSend_Cb, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //��ս��ջ�����
                                            ClearCommCbRecvBuff(pipe);
                                        }

                                        break;
                                    }

                                case 0x003E:
#ifdef AREA_HUNAN
                                case 0x0070:
#endif

                                    break;

                                case 0x0001: //�����ϱ��¼�
                                    {
                                        EventReportMqtt(MsgRecv->playload, MsgRecv->msglen);
                                        break;
                                    }

#ifdef AREA_FUJIAN

                                    //�����ϱ��¼�����iop��Ϊ0003
                                    //case 0x0103: //�ϱ��¼�����

                                case 0x0003:
                                    {
                                        fujian_event_report_mqtt(MsgRecv->playload, MsgRecv->msglen);

                                        //up_msg_response(MsgRecv,1);
                                        break;
                                    }

#endif
                                case 0x0034:
                                case 0x0035:
                                    break; //�����ѱ��ֹͣ�ѱ�ظ�ȷ��֡

                                case 0x0036: //�ϱ��ѱ����� ֱ�Ӵ���������
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
                                case 0x0082: //��ѯ����ģ�����
                                case 0x0084: //��ѯ�ڵ���Ϣ
                                case 0x0088: //ģ���ַ����
                                case 0x0092: //�����ӽڵ�ע��
                                case 0x0094: //����Ԥ������ִ��
                                case 0x0095: //Ԥ�泭������
                                case 0x0100: //����ת������
                                case 0x0101: //����͸�� ����
                                case 0x0108: //��ѯ������ʽ
                                case 0x0090: //������ȡ�㲥��ʱ
                                case 0x0091: //�㲥
                                case 0x0106: //�ϱ��㲥��ʱ
                                case 0x0107: //�ϱ��㲥���
#endif

                                    {
                                        pipe = 0;
                                        pLockSend = gAmrThreadArg[pipe].DownArg->SendBuf;

                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //��ս��ջ�����
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
                                        TASK_FMT_TRACE(-1, MQTT_LOG, "�յ�0061����ģ����Ϣ pipe %d \n", pipe);
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
                                        pipe = 0; //����ʱlabel��˿ں�


                                        pLockSend = gAmrThreadArg[pipe].DownArg->SendBuf;
                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //��ս��ջ�����
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
                                            //��ս��ջ�����
                                            taskmng_comm_recv_buff_clear(port);
                                        }
                                    }
                                    break;

                                case 0x0013: //���ڻظ� bool
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
                                        //ֱ�ӷ������̻߳�����
                                        pLockSend = gAmrThreadArg[1].DownArg->SendBuf;
                                        ret = taskmng_plc_queue_write(pLockSend, (uint8 *) MsgRecv, RecvLen);

                                        if(ret == 0)
                                        {
                                            //��ս��ջ�����
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
                        //����ң�ű�λ�¼� �жϴ�������
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
                        //������Ϣ�ӿڷ�����Ϣ
                        MESSAGE_INFO_T * msg = (MESSAGE_INFO_T *)
                        SendBuf;
                        char topic[500] =
                        {
                            0
                        };
                        sprintf(topic, "%s/%s", msg->souraddr, msg->destaddr);
                        TASK_FMT_TRACE(-1, MQTT_LOG, "Sendupmsg topic %s IID %04x IOP %04x msglen %d msgdata \n", topic, msg->IID, msg->IOP, msg->msglen);

                        //TASK_BUF_TRACE(-1,MQTT_LOG,msg->MsgData, msg->MsgLen);
                        //����Ϣ
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
                        //������Ϣ�ӿڷ�����Ϣ
                        MESSAGE_INFO_T * msg = (MESSAGE_INFO_T *)
                        SendBuf;
                        char topic[500] =
                        {
                            0
                        };
                        sprintf(topic, "%s/%s", msg->souraddr, msg->destaddr);

                        TASK_FMT_TRACE(-1, MQTT_LOG, "͸���������� topic %s IID %04x IOP %04x msglen %d msgdata \n", topic, msg->IID, msg->IOP, msg->msglen);

                        //TASK_BUF_TRACE(-1,MQTT_LOG,msg->MsgData, msg->MsgLen);
                        //����Ϣ
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
                            //������Ϣ�ӿڷ�����Ϣ
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

                            //����Ϣ
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
                            //������Ϣ�ӿڷ�����Ϣ
                            MESSAGE_INFO_T * msg = (MESSAGE_INFO_T *)
                            SendBuf;
                            char topic[500] =
                            {
                                0
                            };
                            sprintf(topic, "%s/%s", msg->souraddr, msg->destaddr);

                            TASK_FMT_TRACE(-1, MQTT_LOG, "Senddownmsg topic %s MsgIndex %d IOP %04x lable %d msglen %d msgdata \n", topic, msg->index, msg->IOP, msg->label, msg->msglen);

                            //����Ϣ
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
* ��������: taskmng_init_file_load
* ��������: ���������ļ�
* �������:
* �������: ��
* �� �� ֵ:
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

    //��ȡ��ǰĿ¼����·������ȥ��������
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
    printf("��ȡ�����ļ�%s \n", filename);

    if(ini != NULL)
    {
        tmp = iniparser_getstring(ini, "mqtt:MQTTIP", "172.17.0.1:1883");
        printf("��ȡmqttip%s \n", tmp);
        strcpy(mqttIP, tmp);
        mqttport = iniparser_getint(ini, "mqtt:MQTTPORT", 1883);
        printf("��ȡmqttport %d \n", mqttport);
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

    //װ�ذ�ȫģʽ
    memset(filename, 0x00, sizeof(filename));
    sprintf(filename, "%s", "/data/app/desktopGui/safemode.ini");
    ini = iniparser_load(filename);

    if(ini != NULL)
    {
        gSafeButton = iniparser_getint(ini, "SAFE_MODE:MAC_MODE", 1);
        TASK_FMT_DEBUG(-1, UP_LOG, "gSafeButton %d  \n", gSafeButton);
        gSafeButtonHPLC = iniparser_getint(ini, "SAFE_MODE:HPLC_MAC_MODE", 0);
        TASK_FMT_DEBUG(-1, UP_LOG, "gSafeButtonHPLC %d  \n", gSafeButtonHPLC);
        //Ϊ��̨����ԣ�Ĭ�Ϲر�·��ѧϰ
        gRouterStudy = iniparser_getint(ini, "SAFE_MODE:ROUTER_STATE_STUDY", 0);
        TASK_FMT_DEBUG(-1, UP_LOG, "gRouterStudy %d  \n", gRouterStudy);
        gMinuteCollect = iniparser_getint(ini, "SAFE_MODE:MINUTE_COLLECT_STATE", 0);
        TASK_FMT_DEBUG(-1, UP_LOG, "gMinuteCollect %d  \n", gMinuteCollect);
        iniparser_freedict(ini);
    }
    else
    {
        gSafeButton = 1;
        TASK_FMT_DEBUG(-1, UP_LOG, "û�ж���gSafeButton %d  \n", gSafeButton);
        gSafeButtonHPLC = 0;
        TASK_FMT_DEBUG(-1, UP_LOG, "gSafeButtonHPLC %d  \n", gSafeButtonHPLC);
        //���û�� ��Ĭ�Ϲر�·��ѧϰ �Է��Բ�����Ӱ��
        gRouterStudy = 0;
        TASK_FMT_DEBUG(-1, UP_LOG, "gRouterStudy %d  \n", gRouterStudy);
        gMinuteCollect = 0;
        TASK_FMT_DEBUG(-1, UP_LOG, "gMinuteCollect %d  \n", gMinuteCollect);
    }
    //�ͼ��֧ ֱ�Ӱ�·��ѧϰ�ر�
    gRouterStudy = 0;
    TASK_FMT_DEBUG(-1, UP_LOG, "gRouterStudy %d  �ͼ��֧�ر�\n", gRouterStudy);
}

/*******************************************************************************
* ��������: taskmng_privatedir_prepare
* ��������: ���˽���ļ�Ŀ¼�������򴴽�
* �������: taskmng_privatedir_prepare �ļ�Ŀ¼
* �������: ��
* �� �� ֵ:
*******************************************************************************/
void taskmng_privatedir_prepare(char * Dir)
{

    if(0 != access(Dir, F_OK))
    {
        printf("����Ŀ¼%s\n", Dir);
        mkdir(Dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
}

/*******************************************************************************
* ��������: TGThread
* ��������: �ܼ��鳭���̣߳����𽻲������Լ��ܼ����������ݵĳ���
* �������:
* �������:
* �� �� ֵ:
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

        //�����ɴ���
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
            TASK_FMT_TRACE(-1, TG_LOG, "TGThread ����dbclientʧ�ܣ� \n");
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
