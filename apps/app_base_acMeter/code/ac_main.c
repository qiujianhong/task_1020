/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#include "ac_main.h"
#include "ac_commdrv.h"
#include "ac.h"
#include "ac_save.h"
#include "ac_sample.h"
#include "ac_sample698.h"
#include "ac_report.h"
#include "ac_mqtt.h"
#include "ac_lib.h"
#include "ac_module.h"
#include "trace_back.h"
#include <sys/prctl.h>
#include <net/if.h> /*����ifi_xxx�ṹ����*/
#include "hal.h"
#include "rtc.h"
#include "uart.h"
#include "power.h"
#include "watchdog.h"
#include "adc.h"
#include "ac_485comm.h"
#include "db_update.h"
#include "ac_698msg.h"
#include "ac_otherfun.h"
#include "ac_update698.h"

uint8 gdelay =0 ;

/*******************************************************************************
* ȫ�ֱ���
*******************************************************************************/
//���ڲɼ����õĹ�Լ��0-DLT645 1-698.45
uint8 gSampPrtl = 0;

typedef enum tag_MSG_TYPE
{
    MSGAPDU = 0,           // APDU
    MSG698 = 1,            // 698����
    MSG645 = 2,            // 645����
}MSG_TYPE_E;

//������Ϣ�����ļ�
#define    AC_METER_INI_FILE  "acMeter.ini"

//485ͨ�������ļ�(ȫ·��)
#define    AC_485_INI_FILE    "/data/app/desktopGui/safemode.ini"

#define    DB_CLIENT_MAX       20     //�ͻ���������
DB_AGENT   gDBAgent = 0;              //���������Ľ�����agent
DB_CLIENT  gDataCenterClient = 0;     //���������Ľ����ľ��(���潻����������������)
DB_CLIENT  gDbUdpClient = 0;          //���������Ľ����ľ��(UDPר��)

//�����ϱ���ϢINDEX
uint16  ActiveInfoIndex = 0;                 //�����ϱ���ϢINDEX
pthread_mutex_t    ActiveInfoIndexLock;      //�����ϱ���ϢINDEX��

//unix udp �󶨵��ļ�����(ȫ·��)
#define    UDP_MSG_PATH    "/tmp/dev/"
char *gUdpFileName = "/tmp/dev/acmeter";
char *gUdpFileNameTest = "/tmp/dev/testfile";

//mqtt ip��ַ��˿ں�
char mqttIP[100] = {0};
int mqttport = 0;

//�Ƿ��ע��app����ע��
uint8 gMapManagerAppLoginFlag = 1;

uint8 gDBCenterAppLoginFlag = 0;

//��app�Ƿ��Ѿ�ע��ɹ�
uint8 bAppSelfLoginState = 0;

//�Ƿ�֧��ת��485���ڱ���
int gSupportRelay485Flag = 1;

//appname: appӢ������(���ڲ�ѯappid)
char *AppIdName = "acMeter";

//appid: ��Ӧapp��ID��Ϣ
char  AppId[APP_ID_LEN] = {0};

OOP_OCTETVAR16_T    tmnAddr;        			 //�ն�ͨ�ŵ�ַ
OOP_OCTETVAR64_T    MAC;                  	     //У����
DB_CLIENT  gRptClient = 0;            //�¼��ϱ��洢�þ��

#if TID_TIME == 1
THREAD_ARG_T g_ThreadTimeArg =
{
    "pthread_time_chk",  //�߳�����
    NULL,                //��������
    0,                   //ά������
};
#endif

#if TID_SLEF_CHECK == 1
THREAD_ARG_T g_ThreadSelfArg =
{
    "pthread_self_chk",  //�߳�����
    NULL,                //��������
    0,                   //ά������
};
#endif

THREAD_ARG_T g_ThreadMqttArg =
{
    "pthread_mqtt",       //�߳�����
    NULL,                 //��������
    0,                    //ά������
};

THREAD_ARG_T g_ThreadSampArg =
{
    "pthread_sample",      //�߳�����
    NULL,                  //��������
    0,                     //ά������
};

THREAD_ARG_T g_ThreadSaveArg =
{
    "pthread_save",        //�߳�����
    NULL,                  //��������
    0,                     //ά������
};

THREAD_ARG_T g_ThreadReportArg =
{
    "pthread_report",      //�߳�����
    NULL,                  //��������
    0,                     //ά������
};

THREAD_ARG_T g_ThreadUpdateArg =
{
    "db_update_entry",      //�߳�����
    NULL,                   //��������
    0,                      //ά������
};

THREAD_ARG_T g_ThreadTestArg =
{
    "pthread_test",         //�߳�����
    NULL,                   //��������
    0,                      //ά������
};

THREAD_ARG_T g_Thread485ModuleArg =
{
    "pthread_485module",     //�߳�����
    NULL,                    //��������
    0,                       //ά������
};

THREAD_ARG_T g_ThreadPowerArg =
{
    "pthread_power",         //�߳�����
    NULL,                   //��������
    0,                      //ά������
};


uint8 g_Arg_485comm[RUN_DOWN_NUM] = {PORT_485};
THREAD_ARG_T g_Thread485CommArg =
{
    "pthread_485comm",       //�߳�����
    &g_Arg_485comm[0],       //��������
    0,                       //ά������
};

MAIN_TRHEAD_T g_tMainThread[] =
{
    /*�Ƿ����� �߳�ID  ���                    �߳���ں���           �Ƿ�ά��*/
    #if TID_TIME == 1
    {TRUE,    0,     &g_ThreadTimeArg,         pthread_time_chk,     TRUE},
    #endif
    #if TID_SLEF_CHECK == 1
    {TRUE,    0,     &g_ThreadSelfArg,         pthread_self_chk,     TRUE},
    #endif
    {TRUE,    0,     &g_ThreadMqttArg,         pthread_mqtt,         TRUE},
    {TRUE,    0,     &g_ThreadSampArg,         pthread_sample,       TRUE},
    {TRUE,    0,     &g_ThreadSaveArg,         pthread_save,         TRUE},
    {TRUE,    0,     &g_ThreadReportArg,       pthread_report,       TRUE},
    {TRUE,    0,     &g_ThreadUpdateArg,       db_update_entry,      FALSE},
    {FALSE,   0,     &g_ThreadTestArg,         pthread_test,         TRUE},
    //#ifdef PRODUCT_ECU
    #if 0
    {TRUE,    0,     &g_Thread485ModuleArg,    pthread_485module,    TRUE},
    {TRUE,    0,     &g_Thread485CommArg,      pthread_485comm,      TRUE},
    #else
    {TRUE,    0,     &g_Thread485CommArg,      pthread_uartManager,  TRUE},
    #endif

    {TRUE,    0,     &g_ThreadPowerArg,        pthread_check_poweroff,  FALSE},
};
uint8 g_MainTaskNum = sizeof(g_tMainThread)/sizeof(MAIN_TRHEAD_T);

extern void sta_time_update_fset(BOOLEAN isUpdate);

/*********************************************************************
 ����������������ʼ��
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void ac_other_init(void)
{
    /* �ж��Ƿ���Ҫ�ļ����� */
    if (0 != access(UDP_MSG_PATH, F_OK))
    {
        INIT_FMT_DEBUG("%s non-existent!", UDP_MSG_PATH);
        if (0 != mkdir(UDP_MSG_PATH, 0777))
        {
            INIT_FMT_DEBUG("%s create failed!", UDP_MSG_PATH);
        }
        else
        {
            INIT_FMT_DEBUG("%s create ok", UDP_MSG_PATH);
        }
    }

    return;
}

/*********************************************************************
 �����������������Ľ������ݳ�ʼ���¼�����
 ���������buf[0]:0-���ݳ�ʼ����1-������ʼ����buf[1]:�߼���ַ
 �����������
 ��������ֵ����
********************************************************************/
void db_ac_init_deal(uint8* buf, uint16 len)
{
    ACMQTT_FMT_DEBUG("db_ac_init_deal. �յ��������Ľ������ݳ�ʼ���¼�choice[%d] logicAddr[%d] len[%d]\n",
        buf[0], buf[1], len);

    if (len >= 2)
    {
        //choice 0 ���ݳ�ʼ��, �߼��豸�� 1
        if ((0 == buf[0]) && (1 == buf[1]))
        {
            //����Ҫ����ʷ���ݶԱ�, ֱ�ӽ���ǰ�������������Ĵ�һ��
            gSaveFlag.bFirstSaveEnergy = TRUE;    //��������, ��һ�δ洢��־, ����
            gSaveFlag.bFirstSaveDemand = TRUE;    //��������, ��һ�δ洢��־, ����
            gSaveFlag.bFirstSaveRealtime = TRUE;  //��������, ��һ�δ洢��־, ʵʱ����
            gSaveFlag.bFirstSaveHarmonic = TRUE;  //��������, ��һ�δ洢��־, г��
            gSaveFlag.bFirstSaveSpecial = TRUE;   //��������, ��һ�δ洢��־, ��������
        }
    }
}

/*********************************************************************
 �����������������ݲ�����ͳ���߳�
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void *pthread_sample(void *arg)
{
    INIT_FMT_DEBUG("pthread_sample start...PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;
    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    //nice(-5);        // ������ȼ�
    if (gSampPrtl == 1)
    {
        ac_calc_deal_698(&tArg->cnt);
    }
    else
    {
        ac_calc_deal(&tArg->cnt);
    }

    pthread_exit(NULL);
}

/*******************************************************************************
* ��������: term_addres_update
* ��������: �����ն˵�ַ
* �������: ��
* �������: ��
* �� �� ֵ: 0-�ɹ��� ����-ʧ��
*******************************************************************************/
int term_addres_update(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int         ret = -1;
    uint32      len = 0;
    NOMAL_OAD_T nomalOAD;

    memset(&nomalOAD, 0, sizeof(NOMAL_OAD_T));
    memset(&tmnAddr, 0, sizeof(OOP_OCTETVAR16_T));
    memset(&MAC, 0, sizeof(OOP_OCTETVAR64_T));

    //��ȡ�ն˵�ַ
    nomalOAD.oad.value = 0x40010200;
    ret = db_read_nomal(clientid, &nomalOAD, sizeof(OOP_OCTETVAR16_T), (uint8 *)&tmnAddr, &len);
    if (0 == ret && len == sizeof(OOP_OCTETVAR16_T))
    {
        MAC.nNum = 6;
        memcpy(MAC.value, tmnAddr.value, MAC.nNum);
        ACDB_FMT_DEBUG("�����ն˵�ַ У��MAC=%02X %02X %02X %02X %02X %02X \n", MAC.value[0],
            MAC.value[1], MAC.value[2], MAC.value[3], MAC.value[4], MAC.value[5]);
    }
    else
    {
        ACDB_FMT_DEBUG("���ն˵�ַʧ��! OAD=%08X, ret=%d, len=%d\n", nomalOAD.oad.value, ret, len);
    }

    return ret;
}

/*********************************************************************
 �����������������Ķ�д�߳�
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void *pthread_save(void *arg)
{
    INIT_FMT_DEBUG("pthread_save start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    uint16 nEnergySecCounter = 0;  //�����ɼ����ʱ��
    uint16 nDemandSecCounter = 0;  //�����ɼ����ʱ��
    uint16 nHarmonicSecCounter = 0;  //г���ɼ����ʱ��
    uint16 nRealtimeSecCounter = 0;  //ʵʱ���ݲɼ����ʱ��
    uint16 nSpecialSecCounter = 0;  //�������ݲɼ����ʱ��
    uint8  nTaskType = SAVE_TYPE_NULL;  //��������

    uint16 nEnergySecCycle = 3;  //�����洢����
    uint16 nDemandSecCycle = 5;  //�����洢����
    uint16 nHarmonicSecCycle = 5;  //г���洢����
    uint16 nRealtimeSecCycle = 1;  //ʵʱ���ݴ洢����
    uint16 nSpecialSecCycle = 3;  //�������ݴ洢����

    uint8  bTimeCheckflag = FALSE;

    #if TID_MAGNETIC == 1
    //��¼�ϴδų�״̬
    int magneticStatusLast = 1;
    //���ڷ���
    int magneticStatusTmp = 1;
    #endif

    tArg->cnt = 0;

    //ʱ������ʼ��
    SaveTimeManageInit();

    //�ȴ�������������
    while (gDBCenterAppLoginFlag != TRUE)
    {
        tArg->cnt = 0;
        sleep(2);
    }

    //��ʼ���������ľ��
    gDataCenterClient  = db_client_create(DB_AC_AGENT, DB_AC_CLIENT_SAVE);

    if (gDataCenterClient <= 0)
    {
        ACDB_FMT_TRACE("pthread_save �������ľ��gDatyaCenterHandle����ʧ��, ����!\n");
        return NULL;
    }

    //���½��ɽ��߷�ʽ
    ac_power_type_update();

    #if TID_MAGNETIC == 1
    //��ȡ�ն˵�ַ
    term_addres_update(gDataCenterClient, 0, 0, 0x40010200);

    //��ʼ�����ݴų�״̬
    magneticStatusTmp = get_mapnetic_status();
    if (magneticStatusTmp < 0)
    {
        magneticStatusTmp = 1;
    }

    #if defined PRODUCT_ZCU_1
    magnetic_dev_init();
    #endif
    #endif

    while(1)
    {
        tArg->cnt = 0;                   // ���ڼ���߳������Ƿ�����
        SaveTimeManageStart();           // һ��ʱ�����ʼ
        nTaskType = SAVE_TYPE_NULL;      // ��������

        if ((Bcd2Hex(gSaveCommonData.CurTime.hh) % 8 == 0) &&
            (Bcd2Hex(gSaveCommonData.CurTime.nn) == 0) &&
            (Bcd2Hex(gSaveCommonData.CurTime.ss) >= 19) &&
            (bTimeCheckflag == TRUE))

        {
            bTimeCheckflag = FALSE;
            //�ͼ��֧����ر�8Сʱ��ʱһ�ε��߼�
            #if 0
            g_nAcSetDateFlag = 1;
            g_nAcSetTimeFlag = 1;
            #endif
        }

        if (Bcd2Hex(gSaveCommonData.CurTime.ss) < 19)
        {
            bTimeCheckflag = TRUE;
        }

        // ���봦��
        if (gSaveTimeFlag.BitSect.Second)
        {
            nEnergySecCounter ++;
            nDemandSecCounter ++;
            nHarmonicSecCounter ++;
            nRealtimeSecCounter ++;
            nSpecialSecCounter ++;

            //nOccurSecCouter ++;
            #if TID_MAGNETIC == 1
            proc_magnetic_chk(&magneticStatusLast, &magneticStatusTmp);
            #endif
        }

        //���ִ���
        if (gSaveTimeFlag.BitSect.Minute)
        {
            sta_volt_qualifyrate_calc();
            if((access(DELAY_FILE_PATH,F_OK)==0))
            {
                if(gdelay == 0)
                {
                    gdelay = 1;
                    gdelaytime = DELAYTIME;
                    UPDATE_FMT_DEBUG("���ɼ�⵽��ʼ��ѹ\n");
                }
            }else if((access(DELAY_FILE_PATH2,F_OK)==0))
            {
                if(gdelay != 2)
                {
                    gdelay = 2;
                    gdelaytime = DELAYTIME_MORE;
                    UPDATE_FMT_DEBUG("��һ����ѹ\n");

                }
            }
            else
            {
                if(gdelay != 0)
                {
                    gdelay = 0;
                    gdelaytime = 0;
                }
            }
        }

        //�����������
        if (nDemandSecCounter >= nDemandSecCycle)
        {
            nDemandSecCounter = 0;
            nTaskType = SAVE_TYPE_DEMAND;  //�������� ����
            ACDB_FMT_TRACE("pthread_save �������� �洢�������� �洢���� nDemandSecCycle[%d]\n", nDemandSecCycle);
        }
        else if (nHarmonicSecCounter >= nHarmonicSecCycle)
        {
            nHarmonicSecCounter = 0;
            nTaskType = SAVE_TYPE_HARMONIC;  //�������� г��
            ACDB_FMT_TRACE("pthread_save �������� �洢г������ �洢���� nHarmonicSecCycle[%d]\n", nHarmonicSecCycle);
        }
        else if (nEnergySecCounter >= nEnergySecCycle)
        {
            nEnergySecCounter = 0;
            nTaskType = SAVE_TYPE_ENERGY;  //�������� ����
            ACDB_FMT_TRACE("pthread_save �������� �洢�������� �洢���� nEnergySecCycle[%d]\n", nEnergySecCycle);
        }
        else if (nSpecialSecCounter >= nSpecialSecCycle)
        {
            nSpecialSecCounter = 0;
            nTaskType = SAVE_TYPE_SPECIAL;  //�������� ��ʱ��������
            ACDB_FMT_TRACE("pthread_save �������� �洢�������� �洢���� nSpecialSecCycle[%d]\n", nSpecialSecCycle);
        }
        else if (nRealtimeSecCounter >= nRealtimeSecCycle)
        {
            nRealtimeSecCounter = 0;
            nTaskType = SAVE_TYPE_REALTIME;  //�������� ʵʱ����
            ACDB_FMT_TRACE("pthread_save �������� �洢ʵʱ���� �洢���� nRealtimeSecCycle[%d]\n", nRealtimeSecCycle);
        }

        //����ʵʱ�������м����
        memcpy(&MidTermSharedData, &TermSharedData, sizeof(TERM_SHARE_DATA_TYPE));
        memcpy(&gMidPriMeasurePara, &gPriMeasurePara, sizeof(PRIVTE_MEASURE_TYPE));

        //���Դų������¼� begin
        /*
        if(nOccurSecCouter > 35)
        {
            MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7 = 1;
            ACDB_FMT_TRACE("pthread_save()ģ�����ɴų������¼� > 35 MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7=%d, nOccurSecCouter=%d \n",
                MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7, nOccurSecCouter);
            nOccurSecCouter = 0;
        }
        else if(nOccurSecCouter > 30)
        {
            MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7 = 1;
            ACDB_FMT_TRACE("pthread_save()ģ�����ɴų������¼� > 30 MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7=%d, nOccurSecCouter=%d \n",
                MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7, nOccurSecCouter);
        }
        else
        {
            MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7 = 0;
            ACDB_FMT_TRACE("pthread_save()ģ�����ɴų������¼� else MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7=%d, nOccurSecCouter=%d \n",
                MidTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7, nOccurSecCouter);
        }
        */
        //���Դų������¼� end

        //�����Խ���������д����������
        Save07AcIntoDataCenter(nTaskType);

        //���ᴦ��������д��֮��
        sta_frozen_proc();

        usleep(500000);
        SaveTimeManageEnd();                 // һ��ʱ��������
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);

}

/**********************************************************************
* @name��      get_head_from_buf
* @brief��     �ҺϷ��ı���68��ͷ����Ҫָ���뱨��
* @param[in] ��
* @param[out]��
* @return    ��
* @Date      ��2019-12-6
**********************************************************************/
uint8* get_645ptr_from_buf(uint8* buf, uint32 maxLen, uint32* offset, uint32* outLen)
{
    int i;
    uint8* head;
    uint8* pBuf = NULL;
    uint32 off = 0;
    uint8  frameCs;
    uint16 frameLen;
    uint16 DataLen = 0;

    if ((buf == NULL) || (maxLen <= 9))
        return NULL;

    head = buf;
    for (i = 0; i < maxLen - 9; i++)
    {
        if ((*head == 0x68) && (*(head + 7) == 0x68))
        {
            pBuf = head;

            //����2���ֽڳ���У��
            DataLen = pBuf[9] + (pBuf[10] << 8);
            frameLen = 0x0d + DataLen;
            if (frameLen > maxLen)
            {
                //��һ���ֽڳ���У��
                DataLen = pBuf[9];
                frameLen = 0x0c + DataLen;
                if (frameLen > maxLen)
                {
                    ACUDP_FMT_DEBUG("frameLen=%d, bufLen=%d \n", frameLen, maxLen);
                    continue;
                }

                frameCs = CalCs(pBuf, frameLen - 2);

                if ((pBuf[0] == 0x68)
                    && (pBuf[7] == pBuf[0]) /*-����ϵͳ����ַ��0��AA�ɱ�-*/
                    && (pBuf[frameLen - 2] == frameCs)
                    && (pBuf[frameLen - 1] == 0x16))
                {
                    *offset = off;
                    *outLen = frameLen;
                    ACUDP_FMT_TRACE("get_645ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
                else
                {
                    continue;
                }
            }

            frameCs = CalCs(pBuf, frameLen - 2);

            if ((pBuf[0] == 0x68)
                && (pBuf[7] == pBuf[0]) /*-����ϵͳ����ַ��0��AA�ɱ�-*/
                && (pBuf[frameLen - 2] == frameCs)
                && (pBuf[frameLen - 1] == 0x16))
            {
                *offset = off;
                *outLen = frameLen;
                ACUDP_FMT_TRACE("get_645ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                return head;
            }
            else
            {
                //��һ���ֽڳ���У��
                DataLen = pBuf[9];
                frameLen = 0x0c + DataLen;
                if (frameLen > maxLen)
                {
                    ACUDP_FMT_DEBUG("frameLen=%d, bufLen=%d \n", frameLen, maxLen);
                    continue;
                }

                frameCs = CalCs(pBuf, frameLen - 2);

                if ((pBuf[0] == 0x68)
                    && (pBuf[7] == pBuf[0]) /*-����ϵͳ����ַ��0��AA�ɱ�-*/
                    && (pBuf[frameLen - 2] == frameCs)
                    && (pBuf[frameLen - 1] == 0x16))
                {
                    *offset = off;
                    *outLen = frameLen;
                    ACUDP_FMT_TRACE("get_645ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
                else
                {
                    continue;
                }
            }
        }
        off++;
        head++;
    }
    return NULL;
}

/**********************************************************************
* @name��      get_head_from_buf
* @brief��     �ҺϷ��ı���68��ͷ����Ҫָ���뱨��
* @param[in] ��
* @param[out]��
* @return    ��
* @Date      ��2019-12-6
**********************************************************************/
uint8* get_698ptr_from_buf(uint8* buf, uint32 maxLen, uint32* offset, uint32* outLen)
{
    int i;
    uint8* head;
    uint32 len = 0;
    uint32 off = 0;

    if ((buf == NULL) || (maxLen <= 9))
        return NULL;

    head = buf;
    for (i = 0; i < maxLen - 9; i++)
    {
        if (*head == 0x68)
        {
            memcpy(&len, head + 1, 2);
            //��ֹ�������
            if ((i + len + 2) <= maxLen)
            {
                if (app_check_message((uint8*)head, len + 2) == 0)
                {
                    *offset = off;
                    *outLen = len + 2;

                    ACUDP_FMT_TRACE("get_698ptr_from_buf. offset[%d] outlen[%d]\n", *offset, *outLen);
                    return head;
                }
            }
        }
        off++;
        head++;
    }
    return NULL;
}

/*
*********************************************************************
* @name      : ac_get_msg_type
* @brief     ����ȡ�������ͣ�Ŀǰ��Ϊ�����ڻ�ϱ���
* @param[in] ��buf - ����
*              len - ���ĳ���
* @param[out]��
* @return    ������
*********************************************************************
*/
MSG_TYPE_E ac_get_msg_type(uint8 *buf, uint32 len)
{
    uint8* ptr = NULL;
    uint32 outLen = 0;
    uint32 offset = 0;

    ptr = get_698ptr_from_buf(buf, len, &offset, &outLen);
    if (ptr != NULL)
    {
        return MSG698;
    }

    ptr = get_645ptr_from_buf(buf, len, &offset, &outLen);
    if (ptr != NULL)
    {
        return MSG645;
    }

    return MSGAPDU;
}

/*
*********************************************************************
* @name      : pthread_report_698jc
* @brief     ����������UDP�ϱ������߳�(͸����698����)
* @param[in] ��
* @param[out]��
* @return    ����
*********************************************************************
*/
void *pthread_report_698jc(void *arg)
{
    INIT_FMT_DEBUG("pthread_report_698jc start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8    SocketState = ACIPC_INIT;        //����socket����״̬
    uint8   RecvBuf[MAX_UDP_BUFF_LEN] = {0}; //socket���ջ�����
    uint8   SendBuf[MAX_UDP_BUFF_LEN] = {0}; //socekt���ͻ�����
    uint8   outMsg[APP_RECV_MSG_MAXLEN] = {0}; //����IPC����
    int32   RecvLen = 0;                        //�������ݳ���
    int32   SendLen = 0;                        //�������ݳ���
    int     ret = -1;

    MSG_TYPE_E msgType = MSGAPDU;
    uint8   *ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;

    int     UdpServiceFd = -1;       //unix udp��չ��Ϣ���
    char    SenderName[100] = {0};  //������
    int     delay = 0;

    //698֡ͷ�ṹ��
    FRAME_HEAD_T tFrame = {0};
    uint8        Addr[6] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA};

    //��ʼ���������ľ��
    gDbUdpClient  = db_client_create(DB_AC_AGENT, DB_AC_CLIENT_UDP);

    if (gDbUdpClient <= 0)
    {
        ACDB_FMT_TRACE("pthread_report_698jc �������ľ��gDbUdpClient����ʧ��, ����! \n");
    }

    while (1)
    {
        tArg->cnt = 0;                    // ���ڼ���߳������Ƿ�����

        switch (SocketState)
        {
            case ACIPC_INIT:
            {
                //udp�����ʼ��
                UdpServiceFd = task_pfmsg_init(gUdpFileName);

                ACUDP_FMT_TRACE("pthread_report_698jc ACIPC_INIT... UdpServiceFd=%d \n", UdpServiceFd);
                if(UdpServiceFd < 0)
                {
                    ACUDP_FMT_DEBUG("pthread_report_698jc ACIPC_INIT err UdpServiceFd[%d]\n", UdpServiceFd);
                    sleep(1);
                }
                else
                {
                    //���ó�ʱʱ��
                    task_msg_settime(UdpServiceFd, 0, 10000);

                    //ת������
                    SocketState = ACIPC_RECV;
                }

                break;
            }
            case ACIPC_RECV:
            {
                //���ؽ������ݽ���
                RecvLen = 0;
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                SendLen = 0;
                memset(SendBuf,0x00,sizeof(SendBuf));

                //��ն���
                while ((SendLen = PlcReadQueue(pUdpSendDown, SendBuf) > 0));

                RecvLen = task_pfmsg_recv(&UdpServiceFd, RecvBuf, MAX_UDP_BUFF_LEN, SenderName, sizeof(SenderName));
                if (RecvLen > 0)
                {
                    ACUDP_BUF_DEBUG(RecvBuf, RecvLen, "recv from %s:", SenderName);

                    msgType = ac_get_msg_type(RecvBuf, RecvLen);
                    if (msgType == MSG698)
                    {
                        //Ŀǰ��Ϊ�����ڻ�ϱ��ģ���������һ��Ҳ������ͬ�ֱ��ģ���Ȼ���ô���
                        while ((ptr = get_698ptr_from_buf(RecvBuf, RecvLen, &offset, &outLen)) != NULL)
                        {
                            SendLen = outLen;
                            memcpy(outMsg, ptr, outLen);

                            RecvLen -= (offset + outLen);
                            memmove(RecvBuf, (ptr + outLen), RecvLen);

                            ret = PlcWriteQueue(pUdpRecvDown, outMsg, SendLen);
                            if (ret == 0)
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send queue full. outMsg:\n", msgType);
                            }
                            else
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send jc module:", msgType);
                            }
                        }
                    }
                    else if (msgType == MSG645)
                    {
                        //Ŀǰ��Ϊ�����ڻ�ϱ��ģ���������һ��Ҳ������ͬ�ֱ��ģ���Ȼ���ô���
                        while ((ptr = get_645ptr_from_buf(RecvBuf, RecvLen, &offset, &outLen)) != NULL)
                        {
                            SendLen = outLen;
                            memcpy(outMsg, ptr, outLen);

                            RecvLen -= (offset + outLen);
                            memmove(RecvBuf, (ptr + outLen), RecvLen);

                            ret = PlcWriteQueue(pUdpRecvDown, outMsg, SendLen);
                            if (ret == 0)
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send queue full. outMsg:\n", msgType);
                            }
                            else
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send jc module:", msgType);
                            }
                        }
                    }
                    else
                    {
                        //����RecvBuf���ص���������APDU������ƴ��װ��·��ͷ
                        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
                        tFrame.dir = 0;
                        tFrame.prm = 1;
                        tFrame.funCode = 3;
                        tFrame.apduLen = RecvLen;
                        tFrame.pApdu = RecvBuf;

                        //��дͨ���ַ
                        tFrame.sAddr.len = 6;
                        memcpy(tFrame.sAddr.addr, Addr, 6);
                        tFrame.sAddr.type = 1;
                        tFrame.sAddr.logicAddr = 0;

                        tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
                        tFrame.msglen = tFrame.headlen + tFrame.apduLen + APP_698_CS_REGIN_SIZE;

                        SendLen = app_frame_to_package(&tFrame, outMsg, APP_RECV_MSG_MAXLEN - 2, TRUE);

                        ret = PlcWriteQueue(pUdpRecvDown, outMsg, SendLen);
                        if (ret == 0)
                        {
                            ACUDP_BUF_DEBUG(outMsg, SendLen, "send queue full. outMsg:\n");
                        }
                        else
                        {
                            ACUDP_BUF_DEBUG(outMsg, SendLen, "send jc module:");
                        }
                    }

                    //ת������
                    SocketState = ACIPC_SEND;
                    delay = 0;
                    break;
                }
                else
                {
                    //��ʱ����Ϊ����07��Լ��2����ǹ���
                    if ((g_nAcSetDateFlag == 1) || (g_nAcSetTimeFlag == 1))
                    {
                        g_nAcSetDateFlag = 0;
                        g_nAcSetTimeFlag = 0;
                        if (app698msg_time_set() == DATA_SUCCESS)
                        {
                            ACUDP_FMT_DEBUG("set time ok\n");
                        }
                        else
                        {
                            ACUDP_FMT_DEBUG("set time err\n");
                        }
                    }
                }

                //ת������
                SocketState = ACIPC_RECV;

                break;
            }

            case ACIPC_SEND:
            {
                SendLen = PlcReadQueue(pUdpSendDown, SendBuf);
                if(SendLen > 0)
                {
                    ACUDP_BUF_DEBUG(SendBuf, SendLen, "recv jc module:");

                    if (msgType != MSGAPDU)
                    {
                        ACUDP_BUF_DEBUG(SendBuf, SendLen, "msg[%d] send %s:", msgType, SenderName);
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, SenderName);
                    }
                    else
                    {
                        /* �Ϸ��Լ�� */
                        ret = app_check_message(SendBuf, SendLen);
                        if (0 != ret)
                        {
                            ACUDP_BUF_DEBUG(SendBuf, SendLen, "app_check_message error[%d] udp recv:\n", ret);
                            break;
                        }

                        /* ����֡ͷ */
                        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
                        ret = app_get_frame(SendBuf, SendLen, &tFrame, TRUE);
                        if (0 != ret)
                        {
                            ACUDP_BUF_DEBUG(SendBuf, SendLen, "app_get_frame error[%d] udp recv:\n", ret);
                            break;
                        }

                        ACUDP_BUF_DEBUG(tFrame.pApdu, tFrame.apduLen, "udp send %s:", SenderName);
                        task_pfmsg_send(UdpServiceFd, tFrame.pApdu, tFrame.apduLen, SenderName);
                    }
                }
                else
                {
                    delay ++;
                    if (delay < 180)
                    {
                        break;
                    }
                }

                //ת����մ���
                SocketState = ACIPC_RECV;
                delay = 0;

                break;
            }

            default:
            {
                SocketState = ACIPC_INIT;
                break;
            }

        }

        usleep(50000);                         //����CPU��Դ
    }

    pthread_exit(NULL);
}

/*
*********************************************************************
* @name      : pthread_report_07jc
* @brief     ����������UDP�ϱ������߳�(07����)
* @param[in] ��
* @param[out]��
* @return    ����
*********************************************************************
*/
void *pthread_report_07jc(void *arg)
{
    INIT_FMT_DEBUG("pthread_report_07jc start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8    SocketState = ACIPC_INIT;           //����socket����״̬
    uint8   RecvBuf[MAX_UDP_BUFF_LEN] = {0};    //socket���ջ�����
    uint8   SendBuf[MAX_UDP_BUFF_LEN] = {0};    //socekt���ͻ�����
    uint8   outMsg[APP_RECV_MSG_MAXLEN] = {0};  //����IPC����
    int32   RecvLen = 0;                        //�������ݳ���
    int32   SendLen = 0;                        //�������ݳ���
    int     ret = -1;

    MSG_TYPE_E msgType = MSGAPDU;
    uint8*  ptr = NULL;
    uint32  outLen = 0;
    uint32  offset = 0;

    int     UdpServiceFd = 0;  //unix udp��չ��Ϣ���
    char    SenderName[100] = {0};  //������
    int     delay = 0;

    memset(RecvBuf, 0x00, sizeof(RecvBuf));
    memset(SendBuf, 0x00, sizeof(SendBuf));

    //��ʼ���������ľ��
    gDbUdpClient  = db_client_create(DB_AC_AGENT, DB_AC_CLIENT_UDP);

    if(gDbUdpClient <= 0)
    {
        ACDB_FMT_TRACE("pthread_report_07jc �������ľ��gDbUdpClient����ʧ��, ����! \n");
    }

    while(1)
    {
        tArg->cnt = 0;                    // ���ڼ���߳������Ƿ�����

        switch (SocketState)
        {
            case ACIPC_INIT:
            {
                //udp�����ʼ��
                UdpServiceFd = task_pfmsg_init(gUdpFileName);

                ACUDP_FMT_TRACE("pthread_report_07jc ACIPC_INIT... UdpServiceFd=%d \n", UdpServiceFd);
                if(UdpServiceFd < 0)
                {
                    ACUDP_FMT_TRACE("pthread_report_07jc ACIPC_INIT err UdpServiceFd[%d]\n", UdpServiceFd);
                    sleep(1);
                }
                else
                {
                    //���ó�ʱʱ��
                    task_msg_settime(UdpServiceFd, 0, 10000);

                    //ת������
                    SocketState = ACIPC_RECV;
                }

                break;
            }
            case ACIPC_RECV:
            {
                //���ؽ������ݽ���
                msgType = MSGAPDU;
                RecvLen = 0;
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                SendLen = 0;
                memset(SendBuf,0x00,sizeof(SendBuf));
                memset(SenderName,0x00,sizeof(SenderName));

                RecvLen = task_pfmsg_recv(&UdpServiceFd, RecvBuf, MAX_UDP_BUFF_LEN, SenderName, sizeof(SenderName));
                if (RecvLen > 0)
                {
                    ACUDP_BUF_DEBUG(RecvBuf, RecvLen, "udp recv %s:", SenderName);

                    msgType = ac_get_msg_type(RecvBuf, RecvLen);
                    if (msgType == MSG645)
                    {
                        //Ŀǰ��Ϊ�����ڻ�ϱ��ģ���������һ��Ҳ������ͬ�ֱ��ģ���Ȼ���ô���
                        while ((ptr = get_645ptr_from_buf(RecvBuf, RecvLen, &offset, &outLen)) != NULL)
                        {
                            SendLen = outLen;
                            memcpy(outMsg, ptr, outLen);

                            RecvLen -= (offset + outLen);
                            memmove(RecvBuf, (ptr + outLen), RecvLen);

                            ret = PlcWriteQueue(pUdpRecvDown, outMsg, SendLen);
                            if (ret == 0)
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send queue full. outMsg:\n", msgType);
                            }
                            else
                            {
                                ACUDP_BUF_DEBUG(outMsg, SendLen, "msg[%d] send jc module:", msgType);
                            }
                        }
                    }
                    else
                    {
                        //����apdu����
                        SendLen = udp_apdu_analyse(RecvBuf, RecvLen, SendBuf, MAX_UDP_BUFF_LEN);
                    }
                }

                if(SendLen > 0)
                {
                    //ת������
                    SocketState = ACIPC_SEND;
                    delay = 0;
                }

                break;
            }

            case ACIPC_SEND:
            {
                if (msgType == MSG645)
                {
                    SendLen = 0;
                    SendLen = PlcReadQueue(pUdpSendDown, SendBuf);
                    if (SendLen > 0)
                    {
                        ACUDP_BUF_DEBUG(SendBuf, SendLen, "msg[%d] recv jc module:", msgType);
                    }
                    else
                    {
                        delay++;
                        if (delay < 180)
                        {
                            break;
                        }
                    }
                }

                if (SendLen > 0)
                {
                    ACUDP_BUF_DEBUG(SendBuf, SendLen, "udp send %s:", SenderName);
                    task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, SenderName);
                }

                //ת����մ���
                SocketState = ACIPC_RECV;
                delay = 0;
                break;
            }

            default:
            {
                SocketState = ACIPC_INIT;
                break;
            }

        }

        usleep(50000);                         //����CPU��Դ
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);
}

/*
*********************************************************************
* @name      : pthread_report
* @brief     ������Ӧ698��07����
* @param[in] ��
* @param[out]��
* @return    ����
*********************************************************************
*/
void *pthread_report(void *arg)
{
    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    //���û���ж������ɰ汾�ţ���ȴ�
    while (isJCNeedGetVer())
    {
        tArg->cnt = 0;
        usleep(100000);
    }

    if (gSampPrtl == 1)
    {
        pthread_report_698jc(arg);
    }
    else
    {
        pthread_report_07jc(arg);
    }

    return NULL;
}

/*********************************************************************
 ������������������MQTTͨ���߳�
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void *pthread_mqtt(void *arg)
{
    INIT_FMT_DEBUG("pthread_mqtt() start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    struct timespec LastLoginTime = {0};
    struct timespec CurrentTime = {0};
    static uint32 LastLoginTimeSec = 0;
    uint32 CurrentTimeSec = 0;
    int8    SocketState = ACIPC_INIT;        //����socket����״̬
    uint8   RecvBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socket���ջ�����
    uint8   SendBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt���ͻ�����
    uint8   SendInfoBuf[MAX_QUEUE_BUFF_LEN] = {0}; //socekt���ͻ�����
    int32   RecvLen = 0;                        //�������ݳ���
    uint32  sleepus = 10000;
    int     ret;

    MSG_INFO_T *ptMsgInfo = NULL;

    memset(RecvBuf, 0x00, sizeof(RecvBuf));
    memset(SendBuf, 0x00, sizeof(SendBuf));
    memset(SendInfoBuf, 0x00, sizeof(SendInfoBuf));

    INFO_DATA_T stInfoData;  //��Ϣ
    memset(&stInfoData, 0x00, sizeof(stInfoData));

    uint16 nMapManagerSendIndex = 0;//������Ϣindex
    time_t MapManagerLastCheckTime;//��ѯʱ��
    MapManagerLastCheckTime = abs(time(NULL) - 5);//��ѯʱ��

    uint16 nDBCenterSendIndex = 0;//������Ϣindex
    time_t DBCenterLastCheckTime;//��ѯʱ��
    DBCenterLastCheckTime = abs(time(NULL) - 5);//��ѯʱ��

    while(1)
    {
        tArg->cnt = 0;                    // ���ڼ���߳������Ƿ�����
        sleepus = 10000;

        switch (SocketState)
        {
            case ACIPC_INIT:
            {
                //printx(" void *pthread_mqtt(void *arg)...case IPC_SOCK_INIT: \n");

                if(0 == gMapManagerAppLoginFlag)//�鿴ģ��������Ƿ�ע��ɹ�
                {
                    if( abs(time(NULL) - MapManagerLastCheckTime) > 2 )
                    {
                        ACMQTT_FMT_TRACE("��ѯģ�������ע��״̬ \n");
                        MapManagerLastCheckTime = time(NULL);
                        check_app_login_state(mapManagerName, &nMapManagerSendIndex);
                    }
                    SocketState = ACIPC_RECV;

                }
                else if(0 == gDBCenterAppLoginFlag)//�鿴���������Ƿ�ע��ɹ�
                {
                    if( abs(time(NULL) - DBCenterLastCheckTime) > 2 )
                    {
                        ACMQTT_FMT_TRACE("��ѯ��������ע��״̬ \n");
                        DBCenterLastCheckTime = time(NULL);
                        check_app_login_state(dbName, &nDBCenterSendIndex);
                    }
                    SocketState = ACIPC_RECV;

                }
                else
                {
                    //ת��ע��
                    SocketState = ACIPC_LOGIN;
                }

                break;
            }
            case ACIPC_LOGIN:
            {
                //printx(" void *pthread_mqtt(void *arg)...case IPC_SOCK_LOGIN: \n");

                login_to_smiOS();
                clock_gettime(CLOCK_MONOTONIC, &LastLoginTime);
                LastLoginTimeSec = (uint32)LastLoginTime.tv_sec;
                SocketState = ACIPC_LOGIN_WAIT;

                break;
            }
            case ACIPC_LOGIN_WAIT:
            {
                clock_gettime(CLOCK_MONOTONIC, &CurrentTime);
                CurrentTimeSec = (uint32)CurrentTime.tv_sec;
                if(CurrentTimeSec - LastLoginTimeSec > 5)
                    SocketState = ACIPC_LOGIN;
                else
                    SocketState = ACIPC_RECV;

                break;
            }
            case ACIPC_RECV:
            {
                //���ؽ������ݽ���
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                RecvLen = mqtt_read(RecvBuf);

                if(RecvLen > 0)
                {
                    sleepus = 0;
                    //����Ϣ������MSG_INFO_T
                    ret = msg_mqtt_split(RecvBuf,RecvLen, 0, 0, &ptMsgInfo);
                    if(ret < 0)
                    {
                        ACMQTT_BUF_DEBUG(RecvBuf, RecvLen, "��Ϣ����ʧ��\n");
                        break;
                    }

                    //��ӡ������Ϣ����
                    print_info_msg(ptMsgInfo);

                    if(ptMsgInfo->IID == 0x0000)
                    {
                        switch(ptMsgInfo->IOP)
                        {
                            case 0x0005:  //ϵͳУʱ�¼�
                            {
                                ACMQTT_FMT_DEBUG("ͨ��Mqtt�յ�ϵͳУʱ�¼�\n");
                                sta_time_update_fset(TRUE);
                                g_nAcSetDateFlag = 1;
                                g_nAcSetTimeFlag = 1;
                            }
                                break;
                            case 0x0010:  //����
                            {
                                 heart_beat(ptMsgInfo->MsgIndex);
                            }
                                break;
                            case 0x0013://�汾��Ϣ
                            {
                                Appversion_Response(ptMsgInfo);
                            }
                                break;
                            case 0x0016://appid��Ϣ
                            {
                                appid_response(ptMsgInfo);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0001)
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0010://ע����Ӧ
                            {
                                bAppSelfLoginState = 1;
                            }
                                break;
                            case 0x0013://��ѯָ��App��Ϣ
                            {
                                if(0 == gMapManagerAppLoginFlag)//�鿴ģ��������Ƿ�ע��ɹ�
                                {
                                    gMapManagerAppLoginFlag = unpack_login_info(ptMsgInfo, mapManagerName, nMapManagerSendIndex);
                                }
                                else if(0 == gDBCenterAppLoginFlag)//�鿴���������Ƿ�ע��ɹ�
                                {
                                    gDBCenterAppLoginFlag = unpack_login_info(ptMsgInfo, dbName, nDBCenterSendIndex);
                                }
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0005)//��������
                    {
                        MESSAGE_INFO_T *ptMsg = NULL;
                        appmsg_split(RecvBuf,RecvLen, &ptMsg);
                        if(NULL != ptMsg)
                        {
                            switch (ptMsgInfo->IOP)
                            {
                                case 0x0002://���ݳ�ʼ���¼�
                                {
                                    db_ac_init_deal(ptMsg->playload,ptMsg->msglen);
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
                                    db_agent_msg_entry(gDBAgent, ptMsg);
                                }
                                    break;

                                case 0x0001:
                                {
                                    db_update_msg_proc(ptMsg);
                                }break;

                                default:
                                    break;
                            }

                            free(ptMsg);
                            ptMsg = NULL;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x1155)
                    {
                        switch(ptMsgInfo->IOP)
                        {
                        case 0x9090:  //��ѯ���̰汾��Ϣ
                        {
                             Factoryversion_Response(ptMsgInfo);
                        }
                            break;
                        default:
                            break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0x0004)//ģ�������
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0001://ģ�����¼�
                            {
                                ModuleEventFun(ptMsgInfo->MsgData, ptMsgInfo->MsgLen);
                            }
                                break;
                            case 0x0010://��ѯģ����Ϣ��Ӧ
                            {
                                PlcWriteQueue(pModuleRecvUp, (uint8*)ptMsgInfo, sizeof(MSG_INFO_T)+ptMsgInfo->MsgLen);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if(ptMsgInfo->IID == 0xFFF0)//�����Զ�����Ϣ�ӿ�
                    {
                        switch (ptMsgInfo->IOP)
                        {
                            case 0x0001://���ɲ�����ʼ��
                            {
                                ACMQTT_FMT_DEBUG("pthread_mqtt() ���ɲ�����ʼ�� \n");
                                ac_param_initial(ptMsgInfo);
                            }
                                break;
                            case 0x0002://�������ݳ�ʼ��
                            {
                                ACMQTT_FMT_DEBUG("pthread_mqtt() �������ݳ�ʼ�� \n");
                                ac_data_initial(ptMsgInfo);
                            }
                                break;
                            case 0x0003://����������ʼ��
                            {
                                ACMQTT_FMT_DEBUG("pthread_mqtt() ����������ʼ�� \n");
                                ac_demand_initial(ptMsgInfo);
                            }
                                break;
                            case 0x0004://����ִ��ϵͳָ��
                            {
                                ACMQTT_FMT_DEBUG("pthread_mqtt() ����ִ��ϵͳָ�� \n");
                                ac_system_deal(ptMsgInfo);
                            }
                                break;
                            default:
                                break;
                        }
                    }
                    else if ((ptMsgInfo->IID == MESSAGE_IID_UART) && (gSupportRelay485Flag > 0))//���ڷ���������
                    {
                        //���ڷ�������SCU��ֲ����������β�һ�µ��������Ҫת���½ӿ�
                        MESSAGE_INFO_T *ptMessageInfo = NULL;
                        uint32          messageLen = sizeof(MESSAGE_INFO_T) + ptMsgInfo->MsgLen;

                        ptMessageInfo = (MESSAGE_INFO_T *)malloc(messageLen);
                        memset(ptMessageInfo, 0, messageLen);

                        ac_485comm_data_convert(ptMsgInfo, ptMessageInfo);
                        ac_485comm_msg_proc(ptMessageInfo);

                        free(ptMessageInfo);
                        ptMessageInfo = NULL;
                    }

                    free(ptMsgInfo);
                    ptMsgInfo = NULL;
                }

                if( (0 == gMapManagerAppLoginFlag) || (0 == gDBCenterAppLoginFlag) )
                {
                    SocketState = ACIPC_INIT;
                }
                else
                {
                    //�ж��Ƿ��¼�ɹ�
                    if(bAppSelfLoginState == 1)
                    {
                        SocketState = ACIPC_SEND;
                    }
                    else
                    {
                        SocketState = ACIPC_LOGIN_WAIT;
                    }
                }

                break;
            }

            case ACIPC_SEND:
            {
                //ת����̼����ݽ��մ���
                SocketState = ACIPC_RECV;

                break;
            }

            default:
            {
                SocketState = ACIPC_INIT;
                break;
            }

        }

        if (sleepus > 0)
        {
            usleep(sleepus);                         //����CPU��Դ
        }
//        if(gdelay != 0)
//        {
//            usleep(gdelaytime);
//        }
    }

    pthread_exit(NULL);
}

/*********************************************************************
 ����������485ģ����Ϣά���߳�
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void *pthread_485module(void *arg)
{
    INIT_FMT_DEBUG("pthread_485module() start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    while(1)
    {
        tArg->cnt = 0;

        if( (1 == gMapManagerAppLoginFlag) && (1 == gDBCenterAppLoginFlag) && (1 == bAppSelfLoginState) )
        {
            //��ѯģ����Ϣ
            if(gQueryAllModuleFlag == TRUE)
            {
                ACMQTT_FMT_DEBUG("pthread_485module() if(gQueryAllModuleFlag == TRUE)...\n");
                //��ѯ����ģ����Ϣ
                if(GetAllModuleInfo() > 0)//�ж��Ƿ���ģ�����
                {
                    UpdateModuleInfo();
                    gQueryAllModuleFlag = FALSE;
                }
            }
            else if(gReloadModuleFlag[0] == 1)
            {
                ACMQTT_FMT_DEBUG("pthread_485module() else if(gReloadModuleFlag[0] == 1)...\n");
                //��ѯ����ģ����Ϣ
                if(ModuleEventDeal() > 0)
                {
                    UpdateModuleInfo();
                }
            }
        }

        sleep(1);
    }

    pthread_exit(NULL);
}

/*********************************************************************
 ����������485ģ�鴮��ͨ���߳�
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void *pthread_485comm(void *arg)
{
    THREAD_ARG_T* tArg = (THREAD_ARG_T*)arg;

    uint8 pipe = *((uint8*)tArg->arg);
    INIT_FMT_DEBUG("pthread_485comm start. PID = %d. pipe = %d\n", (int)getpid(), pipe);

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8   SocketState = ACIPC_INIT;        //�߳�����״̬
    uint8  RecvBuf[MAX_QUEUE_BUFF_LEN];        //���ջ�����
    uint8  SendBuf[MAX_QUEUE_BUFF_LEN];        //���ͻ�����
    int32  RecvLen = 0, SendLen = 0;           //���ա��������ݳ���
    int    ret = -1;

    while(1)
    {
        tArg->cnt = 0;                      //���ڼ���߳������Ƿ�����

        switch (SocketState)
        {
        case ACIPC_INIT:                    //��ʼ������
            if(gSupportRelay485Flag <= 0)
            {
                SocketState = ACIPC_INIT;
                sleep(5);  //��֧��ת��485���ڱ���, �߳̿�ת
                break;
            }

            if(gDevHalInfo[pipe].isvalid == 0)
            {
                SocketState = ACIPC_INIT;
                sleep(1);  //ģ����Ϣ��Ч, �߳̿�ת
                break;
            }

            if((NULL == gDevHalInfo[pipe].dev) || (1 == gReloadDevFlag[pipe]))
            {
                //��ȡ�豸
                gDevHalInfo[pipe].dev = (UART_DEVICE_T*)hal_device_get(gDevHalInfo[pipe].deviceName);
                AC485COM_FMT_TRACE(pipe, "����[%d] deviceName:%s, dev:%p, gReloadDevFlag=%d \n",
                    pipe, gDevHalInfo[pipe].deviceName, gDevHalInfo[pipe].dev, gReloadDevFlag[pipe]);

                // ���豸
                ret = gDevHalInfo[pipe].dev->uart_param_set(gDevHalInfo[pipe].dev, 9600, 8, 1, PARITY_EVEN);  //TRUE:1, FALSE:0
                AC485COM_FMT_TRACE(pipe, "����[%d] dev->uart_param_set() ret=%d \n", pipe, ret);
                if(ret == 0)
                {
                    SocketState = ACIPC_RECV;
                    AC485COM_FMT_TRACE(pipe, "����[%d]�򿪳ɹ�DownTtlThread Success \n", pipe);
                    gReloadDevFlag[pipe] = 0;
                }
                else
                {
                    AC485COM_FMT_TRACE(pipe, "����[%d]��ʧ��DownTtlThread Error \n", pipe);
                    sleep(2);
                    break;
                }
                sleep(1);
            }
            else
            {
                SocketState = ACIPC_RECV;
            }
            break;

        case ACIPC_RECV:
            if((gDevHalInfo[pipe].isvalid == 0) || (1 == gReloadDevFlag[pipe]))
            {
                SocketState = ACIPC_INIT;
                break;
            }

            memset(RecvBuf, 0x00, sizeof(RecvBuf));

            if(gDevHalInfo[pipe].dev == NULL)
            {
                SocketState = ACIPC_INIT;
                break;
            }
            RecvLen = gDevHalInfo[pipe].dev->uart_data_recv(gDevHalInfo[pipe].dev, RecvBuf, sizeof(RecvBuf));
            if (RecvLen > 0)
            {
                AC485COM_FMT_TRACE(pipe, "����[%d]����, deviceName=%s, RecvLen = %d \n", pipe, gDevHalInfo[pipe].deviceName, RecvLen);
                AC485COM_BUF_TRACE(pipe, RecvBuf, RecvLen, "����");

                ret = PlcWriteQueue(pDealRecvDown[pipe], RecvBuf, RecvLen);
                AC485COM_FMT_TRACE(pipe, "PlcWriteQueue() ret=%d \n", ret);
            }

            SocketState = ACIPC_SEND;
            break;

        case ACIPC_SEND:
            if((gDevHalInfo[pipe].isvalid == 0) || (1 == gReloadDevFlag[pipe]))
            {
                SocketState = ACIPC_INIT;
                break;
            }

            SendLen = PlcReadQueue(pDealSendDown[pipe], SendBuf);
            if (SendLen > 0)
            {
                AC485COM_FMT_TRACE(pipe, "����[%d]����, deviceName=%s, SendLen = %d \n", pipe, gDevHalInfo[pipe].deviceName, SendLen);
                AC485COM_BUF_TRACE(pipe, SendBuf, SendLen, "����");

                if(gDevHalInfo[pipe].dev == NULL)
                {
                    SocketState = ACIPC_INIT;
                    break;
                }
                ret = gDevHalInfo[pipe].dev->uart_data_send(gDevHalInfo[pipe].dev, SendBuf, SendLen);
                AC485COM_FMT_TRACE(pipe, "���ͽ��: pipe=%d, deviceName=%s, ret=%d \n", pipe, gDevHalInfo[pipe].deviceName, ret);

                //дͨ����־
                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = ACIPC_INIT;
            break;

        case ACIPC_CLOSE:
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[pipe].dev);
            AC485COM_FMT_TRACE(pipe, "hal_device_release() ret=%d \n", ret);
            SocketState = ACIPC_INIT;
            break;

        default:
            ret = hal_device_release((HW_DEVICE *)gDevHalInfo[pipe].dev);
            AC485COM_FMT_TRACE(pipe, "hal_device_release() ret=%d \n", ret);
            SocketState = ACIPC_INIT;
            break;
        }

        usleep(100000);                         //����CPU��Դ

    }

    pthread_exit(NULL);
}

/*********************************************************************
 ����������485ģ�鴮��ͨ���߳�
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void *pthread_uartManager(void *arg)
{
    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    uint8 pipe = *((uint8 *)tArg->arg);
    INIT_FMT_DEBUG("pthread_485comm start. PID = %d. pipe = %d\n", (int)getpid(), pipe);

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8   SocketState = ACIPC_INIT;              //�߳�����״̬
    // uint8  RecvBuf[MAX_QUEUE_BUFF_LEN];        //���ջ�����
    uint8  SendBuf[MAX_QUEUE_BUFF_LEN] = {0};     //���ͻ�����
    uint8  SendBufTmp[MAX_QUEUE_BUFF_LEN] = {0};
    // int32  RecvLen = 0;
    int32  SendLen = 0;
    int    ret = -1;
    uint32 outLen = 0;
    uint32 offset = 0;
    uint8* ptr = NULL;

    while(1)
    {
        tArg->cnt = 0;                 //���ڼ���߳������Ƿ�����

        switch (SocketState)
        {
        case ACIPC_INIT:                       //��ʼ������
            if (gSupportRelay485Flag <= 0)
            {
                //����Ҫת��485���ڱ���, �߳̿�ת
                SocketState = ACIPC_INIT;
                sleep(5);
                break;
            }

            if (g_Comm485Info.isGetInfo == 0)
            {
                ret = ac_485comm_getinfo(pipe);
                if (ret == 0)
                {
                    AC485COM_FMT_DEBUG(pipe, "��ѯ������Ϣ�ɹ� ret[%d]\n", ret);
                    g_Comm485Info.isGetInfo = 1;
                }
                else
                {
                    AC485COM_FMT_DEBUG(pipe, "��ѯ������Ϣʧ�� ret[%d]\n", ret);
                    sleep(5);
                    break;
                }
                SocketState = ACIPC_INIT;
                break;
            }
            else if (g_Comm485Info.isSubcribe == 0)
            {
                ret = ac_485comm_subscribe(pipe);
                if (ret == 0)
                {
                    AC485COM_FMT_DEBUG(pipe, "���Ĵ��ڷ���ɹ� ret[%d]\n", ret);
                    g_Comm485Info.isSubcribe = 1;
                }
                else
                {
                    AC485COM_FMT_DEBUG(pipe, "���Ĵ��ڷ���ʧ�� ret[%d]\n", ret);
                    SocketState = ACIPC_INIT;
                    sleep(5);
                    break;
                }
            }

            if ((UART_MODE_MASTER == g_Comm485Info.tModeInfo.mode) ||
                (UART_MODE_ONLY == g_Comm485Info.tModeInfo.mode && 0 != strcmp(g_Comm485Info.tModeInfo.onlyApp, AC_APP_NAME)))
            {
                //����Ҫת��485���ڱ���, �߳̿�ת
                AC485COM_FMT_TRACE(pipe, "ģʽ�����߳̿�ת\n");
                SocketState = ACIPC_INIT;
                sleep(5);
                break;
            }

            SocketState = ACIPC_RECV;
            break;

        case ACIPC_RECV:

            //��������Ϣ�ĵ�mqtt�̣߳��ȴ����ڷ���������֪ͨ
            SocketState = ACIPC_SEND;
            break;

        case ACIPC_SEND:
            SendLen = PlcReadQueue(pDealSendDown[0], SendBuf);
            if (SendLen > 0)
            {
                while ((ptr = get_07ptr_from_buf_n(SendBuf, SendLen, &offset, &outLen)) != NULL)
                {
                    memcpy(SendBufTmp, ptr, outLen);

                    SendLen -= (offset + outLen);
                    memmove(SendBuf, (ptr + outLen), SendLen);

                    ret = ac_485comm_send(pipe, SendBufTmp, outLen);
                    if (ret != 0)
                    {
                        AC485COM_BUF_DEBUG(pipe, SendBufTmp, outLen, "����[%d]���ͱ���ʧ��. ret[%d]", pipe, ret);
                    }
                    else
                    {
                        AC485COM_BUF_DEBUG(pipe, SendBufTmp, outLen, "  ����[%d]���ͱ���", pipe);
                    }
                }

                memset(SendBuf, 0, sizeof(SendBuf));
            }
            SocketState = ACIPC_INIT;
            break;

        case ACIPC_CLOSE:
            SocketState = ACIPC_INIT;
            break;

        default:
            SocketState = ACIPC_INIT;
            break;
        }

        usleep(20000);                         //����CPU��Դ
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }

    }

    pthread_exit(NULL);
}

/*********************************************************************
 ���������������߳�
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void *pthread_test(void *arg)
{
    INIT_FMT_DEBUG("�����ԡ�pthread_test() AC main start... PID[%d]\n", (int)getpid());

    THREAD_ARG_T *tArg = (THREAD_ARG_T *)arg;

    //�߳���
    prctl(PR_SET_NAME, tArg->threadName);

    tArg->cnt = 0;

    int8    SocketState = ACIPC_INIT;        //����socket����״̬
    uint8   RecvBuf[MAX_UDP_BUFF_LEN] = {0}; //socket���ջ�����
    uint8   SendBuf[MAX_UDP_BUFF_LEN] = {0}; //socekt���ͻ�����
    int32   RecvLen = 0;                        //�������ݳ���
    int32   SendLen = 0;                        //�������ݳ���

    int     UdpServiceFd = 0;  //unix udp��չ��Ϣ���
    char    SenderName[100] = {0};  //������

    BOOL    bOnlyTestGetNormal = FALSE;
    BOOL    bTestGetNormal = FALSE;
    BOOL    bTestGetNormalList = FALSE;
    BOOL    bTestGetNormalOther = FALSE;
    BOOL    bTestApduBuff = TRUE;

    uint16 i = 0;
    uint16 j = 0;
    uint16 k = 0;
    uint16 index = 0;

    memset(RecvBuf, 0x00, sizeof(RecvBuf));
    memset(SendBuf, 0x00, sizeof(SendBuf));

    while(1)
    {
        tArg->cnt = 0;                    // ���ڼ���߳������Ƿ�����


        switch (SocketState)
        {
            case ACIPC_INIT:
            {
                //udp�����ʼ��
                UdpServiceFd = task_pfmsg_init(gUdpFileNameTest);
                ACUDP_FMT_TRACE("�����ԡ�pthread_test() ACIPC_INIT... UdpServiceFd=%d \n", UdpServiceFd);
                if(UdpServiceFd < 0)
                {
                    ACUDP_FMT_TRACE("�����ԡ�pthread_test() ACIPC_INIT... if(UdpServiceFd < 0) \n");
                    sleep(1);
                }
                else
                {
                    //ת������
                    SocketState = ACIPC_SEND;
                }

                break;
            }

            case ACIPC_SEND:
            {
                sleep(1);  //���ķ��ͼ��

                //���Է���1: GetRequestNormal����
                if(bTestGetNormal)
                {
                    if(i >= gSaveTaskAllNum)
                    {
                        sleep(20);
                        i = 0;
                        if(!bOnlyTestGetNormal)
                        {
                            bTestGetNormal = FALSE;
                            bTestGetNormalList = TRUE;
                            break;
                        }
                    }

                    index = 0;
                    SendLen = 0;
                    memset(SendBuf,0x00,sizeof(SendBuf));

                    //��װAPDU���Ա���
                    SendBuf[index] = 0x05;  //get-request
                    index += 1;
                    SendBuf[index] = 0x01;  //GetRequestNormal
                    index += 1;
                    SendBuf[index] = 0x01;  //PIID
                    index += 1;
                    memcpy_r(&SendBuf[index], &gSaveTaskAll[i], 4);  //OAD
                    index += 4;
                    SendBuf[index] = 0x01;  //û��ʱ���ǩ
                    index += 1;

                    if(index > 0)
                    {
                        SendLen = index;
                        ACUDP_FMT_TRACE("�����ԡ���GetNormal��pthread_test() ACIPC_SEND... SendLen=%d, DestName=%s \n", SendLen, gUdpFileName);
                        ACUDP_BUF_TRACE(SendBuf, SendLen, "�����ԡ���GetNormal�����ͱ���:");
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, gUdpFileName);
                    }

                    //������һ��OAD
                    i ++;
                }

                //���Է���2: GetRequestNormalList����
                if(bTestGetNormalList)
                {
                    if(j >= 5)
                    {
                        sleep(20);
                        j = 0;
                        bTestGetNormal = TRUE;
                        bTestGetNormalList = FALSE;
                        break;
                    }

                    index = 0;
                    SendLen = 0;
                    memset(SendBuf,0x00,sizeof(SendBuf));

                    //��װAPDU���Ա���
                    SendBuf[index] = 0x05;  //get-request
                    index += 1;
                    SendBuf[index] = 0x02;  //GetRequestNormalList
                    index += 1;
                    SendBuf[index] = 0x01;  //PIID
                    index += 1;
                    if(j == 0)
                    {
                        ChangeLenToBuff(gSaveTaskEnergyNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskEnergyNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskEnergy[k], 4);  //OAD
                            index += 4;
                        }
                    }
                    else if(j == 1)
                    {
                        ChangeLenToBuff(gSaveTaskDemandNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskDemandNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskDemand[k], 4);  //OAD
                            index += 4;
                        }
                    }
                    else if(j == 2)
                    {
                        ChangeLenToBuff(gSaveTaskRealtimeNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskRealtimeNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskRealtime[k], 4);  //OAD
                            index += 4;
                        }
                    }
                    else if(j == 3)
                    {
                        ChangeLenToBuff(gSaveTaskHarmonicNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskHarmonicNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskHarmonic[k], 4);  //OAD
                            index += 4;
                        }
                    }
                    else if(j == 4)
                    {
                        ChangeLenToBuff(gSaveTaskContainerNum, &index, &SendBuf[index]);  //seq of
                        for(k=0; k<gSaveTaskContainerNum; k++)
                        {
                            memcpy_r(&SendBuf[index], &gSaveTaskContainer[k], 4);  //OAD
                            index += 4;
                        }
                    }

                    SendBuf[index] = 0x01;  //û��ʱ���ǩ
                    index += 1;

                    if(index > 0)
                    {
                        SendLen = index;
                        ACUDP_FMT_TRACE("�����ԡ���GetNormalList��pthread_test() ACIPC_SEND... SendLen=%d, DestName=%s \n", SendLen, gUdpFileName);
                        ACUDP_BUF_TRACE(SendBuf, SendLen, "�����ԡ���GetNormalList�����ͱ���:");
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, gUdpFileName);
                    }

                    //������һ��OAD
                    j ++;
                }

                //���Է���3: GetRequestNormal����
                if(bTestGetNormalOther)
                {
                    if(i >= gSubOadDataNum)
                    {
                        sleep(20);
                        i = 0;
                    }

                    index = 0;
                    SendLen = 0;
                    memset(SendBuf,0x00,sizeof(SendBuf));

                    //��װAPDU���Ա���
                    SendBuf[index] = 0x05;  //get-request
                    index += 1;
                    SendBuf[index] = 0x01;  //GetRequestNormal
                    index += 1;
                    SendBuf[index] = 0x01;  //PIID
                    index += 1;
                    memcpy_r(&SendBuf[index], &gSubOadData[i], 4);  //OAD
                    index += 4;
                    SendBuf[index] = 0x01;  //û��ʱ���ǩ
                    index += 1;

                    if(index > 0)
                    {
                        SendLen = index;
                        ACUDP_FMT_TRACE("�����ԡ���bTestGetNormalOther��pthread_test() ACIPC_SEND... SendLen=%d, DestName=%s \n", SendLen, gUdpFileName);
                        ACUDP_BUF_TRACE(SendBuf, SendLen, "�����ԡ���bTestGetNormalOther�����ͱ���:");
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, gUdpFileName);
                    }

                    //������һ��OAD
                    i ++;
                }

                //���Է���4: �̶�apdu���Ĳ���
                if(bTestApduBuff)
                {
                    index = 0;
                    SendLen = 0;
                    memset(SendBuf,0x00,sizeof(SendBuf));

                    if(i > 4)
                    {
                        sleep(20);
                        i = 0;
                    }

                    //��װAPDU���Ա���
                    if(0 == i)
                    {
                        //���ý��ɵ�ѹ�ϸ��ʲ���
                        uint8 tmpbuf[] = {0x06,0x01,0x00,0x40,0x30,0x02,0x00,0x02,0x04,0x12,0x03,0xde,0x12,0x03,0xde,0x12,0x03,0xde,0x12,0x03,0xde,0x00};
                        uint8 tmpbufLen = sizeof(tmpbuf)/sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }
                    else if(1 == i)
                    {
                        //���ý��ɵ�ѹ���������
                        uint8 tmpbuf[] = {0x06,0x01,0x01,0x30,0x0f,0x06,0x00,0x02,0x01,0x11,0x0a,0x00};
                        uint8 tmpbufLen = sizeof(tmpbuf)/sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }
                    else if(2 == i)
                    {
                        //���ý��ɶ��������
                        uint8 tmpbuf[] = {0x07,0x01,0x02,0x50,0x04,0x07,0x00,0x01,0x01,0x02,0x03,0x12,0x00,0x0f,0x51,0x20,0x00,0x02,0x00,0x12,0x00,0x0f,0x00};
                        uint8 tmpbufLen = sizeof(tmpbuf)/sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }
                    else if(3 == i)
                    {
                        //ɾ�����ɶ�������У�698���Ա���
                        uint8 tmpbuf[] = {0x68, 0x1c, 0x00, 0x43, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbe, 0xa4, 0x07, 0x01, 0x02, 0x50, 0x04,
                            0x05, 0x00, 0x51, 0x20, 0x00, 0x02, 0x00, 0x00, 0x77, 0xb0, 0x16};
                        uint8 tmpbufLen = sizeof(tmpbuf)/sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }
                    else if (4 == i)
                    {
                        //645���Ա���
                        uint8 tmpbuf[] = {0x68, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x68, 0x14, 0x12, 0x34, 0x37, 0x33, 0x37, 0x33, 0x33, 0x33, 0x33, 0x33,
                            0x33, 0x33, 0x33, 0x34, 0x33, 0x33, 0x33, 0x33, 0x33, 0x92, 0x16};
                        uint8 tmpbufLen = sizeof(tmpbuf) / sizeof(uint8);
                        index = tmpbufLen;
                        memcpy(SendBuf, tmpbuf, tmpbufLen);
                    }

                    if(index > 0)
                    {
                        SendLen = index;
                        ACUDP_FMT_TRACE("�����ԡ���bTestGetNormalOther��pthread_test() ACIPC_SEND... SendLen=%d, DestName=%s \n", SendLen, gUdpFileName);
                        ACUDP_BUF_TRACE(SendBuf, SendLen, "�����ԡ���bTestGetNormalOther�����ͱ���:");
                        task_pfmsg_send(UdpServiceFd, SendBuf, SendLen, gUdpFileName);
                    }

                    //������һ��OAD
                    i ++;
                }

                //ת����մ���
                SocketState = ACIPC_RECV;

                break;
            }

            case ACIPC_RECV:
            {
                //���ؽ������ݽ���
                RecvLen = 0;
                memset(RecvBuf,0x00,sizeof(RecvBuf));
                memset(SenderName,0x00,sizeof(SenderName));

                RecvLen = task_pfmsg_recv(&UdpServiceFd, RecvBuf, MAX_UDP_BUFF_LEN, SenderName, sizeof(SenderName));

                if(RecvLen > 0)
                {
                    ACUDP_FMT_TRACE("�����ԡ�pthread_test() ACIPC_RECV... RecvLen=%d, SenderName=%s \n", RecvLen, SenderName);
                    ACUDP_BUF_TRACE(RecvBuf, RecvLen, "�����ԡ����ձ���:");
                }

                //ת������
                SocketState = ACIPC_SEND;

                break;
            }

            default:
            {
                SocketState = ACIPC_INIT;
                break;
            }

        }

        usleep(200000);                         //����CPU��Դ
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    pthread_exit(NULL);

}



/*********************************************************************
 ����������ȫ�ֱ�����ʼ��
 ���������
 �����������
 ��������ֵ����
********************************************************************/
void init_global()
{
    int ret = -1;

    //����������ϢINDEX����ʼ��
    if (pthread_mutex_init(&ActiveInfoIndexLock, NULL) != 0)
    {
        ACMQTT_FMT_DEBUG("init_global() init pthread ActiveInfoIndexLock lock error!\n");
        exit(1);
    }

    //ȫ�ֱ�����ʼ��
    memset(&TermSharedData, 0, sizeof(TERM_SHARE_DATA_TYPE));
    memset(&gPriMeasurePara, 0, sizeof(PRIVTE_MEASURE_TYPE));
    memset(&MidTermSharedData, 0, sizeof(TERM_SHARE_DATA_TYPE));
    memset(&gMidPriMeasurePara, 0, sizeof(PRIVTE_MEASURE_TYPE));
    memset(&HisTermSharedData, 0, sizeof(TERM_SHARE_DATA_TYPE));
    memset(&gHisPriMeasurePara, 0, sizeof(PRIVTE_MEASURE_TYPE));

    memset(g_nAcReadPeriNumFlagArr, 0x01, OOP_MAX_DAYPERIODS);
    memset(g_nAcReadCurrDayFlagArr, 0x01, OOP_MAX_DAYPERIODS);
    memset(g_nAcReadAddiDayFlagArr, 0x01, OOP_MAX_DAYPERIODS);

    //���ܱ�㶨�ų������¼�
    memset(&gEventCommInfo, 0, sizeof(EVENT_COMM_INFO));

    //���ص��ܱ�㶨�ų������¼�����
    ret = read_pdata_xram(gEventParaPath, (char *)&gEventCommInfo, 0, sizeof(EVENT_COMM_INFO));
    ACDB_FMT_DEBUG("init_global() ���شų������¼����� ret=%d \n", ret);
    if (ret == 0)
    {
        HisTermSharedData.MetStusPara.Rsw1.BitSect.ReservedBit7 = gEventCommInfo.EffectState;
        ACDB_FMT_DEBUG("init_global() ���شų������¼������ɹ�, ��ʼ״̬ gEventCommInfo.EffectState=%d \n", gEventCommInfo.EffectState);
    }

    //�����¼�״̬����
    memset(&gTotalEventInfo, 0, sizeof(gTotalEventInfo));

    //���ɵ�ѹ�������¼���ʼ��
    memset(&event_300F, 0, sizeof(event_300F));
    event_300F.eventCommInfo.eventOAD.value = 0x300F0200;

    //�����洢��ǳ�ʼ��
    memset(&gSaveFlag, 0, sizeof(SAVE_FLAG_T));
    gSaveFlag.bFirstSaveEnergy = TRUE;
    gSaveFlag.bFirstSaveDemand= TRUE;
    gSaveFlag.bFirstSaveRealtime = TRUE;
    gSaveFlag.bFirstSaveHarmonic = TRUE;
    gSaveFlag.bFirstSaveSpecial = TRUE;

    return;
}

/*********************************************************************
 ���������������ļ�����
 ���������
 �����������
 ��������ֵ����
********************************************************************/
int init_config()
{
    dictionary *ini = NULL;
    char* tmp;
    char* stringvalue = NULL;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //��ȡ��ǰĿ¼����·������ȥ��������
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("acMeter get err path!\n");
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }
    sprintf(filename, "%s%s", curpath, AC_METER_INI_FILE);
    printf("�����ļ�filename: %s!\n", filename);
    ini = iniparser_load(filename);

    /* ��־ģ���ʼ�� */
    diag_init(filename, TRUE);
    sleep(1);

    if(NULL != ini)
    {
        /* �����ļ����ݻ�ȡ */
        tmp = iniparser_getstring(ini, "mqtt:MQTTIP", "172.17.0.1");
        printf("��ȡmqttip = %s \n", tmp);
        strcpy(mqttIP, tmp);
        mqttport = iniparser_getint(ini, "mqtt:MQTTPORT", 1883);
        printf("��ȡmqttport = %d \n", mqttport);

        tmp = iniparser_getstring(ini, "FILEPATH_CONF:LOGPATH", "/data/app/acMeter/log");
        char   exceptpath[256] = {0};
        sprintf(exceptpath, "%s/Exception.log", tmp);
        change_sig_file(exceptpath);

        /*
        gSupportRelay485Flag = iniparser_getint(ini, "RELAY_CONF:AC485FLAG", 0);
        printf("��ȡgSupportRelay485Flag = %d \n", gSupportRelay485Flag);
        */

        /* ���ڲɼ����õĹ�Լ��0-DLT645 1-698.45 */
        gSampPrtl = iniparser_getint(ini, "SAMP_CONF:PRTL", 2);
        INIT_FMT_DEBUG("���ڲɼ����õĹ�Լ[%d], 0-DLT645 1-698.45\n", gSampPrtl);

        /* ��ȡ�������� */
        gUartConf.enable = iniparser_getint(ini, "UART_CONF:ENABLE", 0);
        INIT_FMT_DEBUG("��ȡ����ʹ�ܱ�־[%d]\n", gUartConf.enable);

#ifdef BOARD_801
        //801���İ崮��ͨ��HAL��ȡ
        UART_DEVICE_T *dev = (UART_DEVICE_T*)hal_device_get(UART_DEVICE_ACPORT_STR);
        if(dev)
        {
            dev->uart_get_real_dev_name(dev, (uint8*)gUartConf.dev);
            hal_device_release((HW_DEVICE *)dev);
        }
        else
        {
            sprintf(gUartConf.dev, "%s", "/tty/S8");
        }

#else
        stringvalue = iniparser_getstring(ini, "UART_CONF:DEV", "/dev/ttyS4");
        strcpy(gUartConf.dev, stringvalue);
        INIT_FMT_DEBUG("��ȡ�����豸����[%s]\n", gUartConf.dev);
#endif

        gUartConf.BaudRate = iniparser_getint(ini, "UART_CONF:BAUDRATE", 115200);
        INIT_FMT_DEBUG("��ȡ���ڵĲ�����[%d]\n", gUartConf.BaudRate);

        gUartConf.ByteSize = iniparser_getint(ini, "UART_CONF:BYTESIZE", 8);
        INIT_FMT_DEBUG("��ȡ�����ֽڳ���[%d]\n", gUartConf.ByteSize);

        stringvalue = iniparser_getstring(ini, "UART_CONF:PARITY", "e");
        gUartConf.Parity = stringvalue[0];
        INIT_FMT_DEBUG("��ȡ����У��ģʽ[%c]\n", gUartConf.Parity);

        gUartConf.StopBits = iniparser_getint(ini, "UART_CONF:STOPBITS", 1);
        INIT_FMT_DEBUG("��ȡ���ڵ�ֹͣλ[%d]\n", gUartConf.StopBits);

        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file: %s!\n", AC_METER_INI_FILE);
    }

    return 0;
}

/*********************************************************************
 ����������485-2���Կ������ļ�����
 ���������
 �����������
 ��������ֵ����
********************************************************************/
int init_485_config()
{
    dictionary *ini = NULL;
    char   filename[256] = {0};
    char*  stringvalue = NULL;

    sprintf(filename, "%s", AC_485_INI_FILE);
    INIT_FMT_TRACE("485-2���Կ������ļ�[%s]\n", filename);
    ini = iniparser_load(filename);

    if (NULL != ini)
    {
        /* �����ļ����ݻ�ȡ */
        gSupportRelay485Flag = iniparser_getint(ini, "SAFE_MODE:AC485FLAG", 1);
        INIT_FMT_DEBUG("��ȡgSupportRelay485Flag = %d \n", gSupportRelay485Flag);

        iniparser_freedict(ini);
    }
    else
    {
        INIT_FMT_DEBUG("485-2���Կ������ļ�[%s]��ȡʧ��\n", filename);
    }

    sprintf(filename, "%s", AC_METER_INI_FILE);
    ini = iniparser_load(filename);

    if (NULL != ini)
    {
        /* ��ȡipc·�� */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", (char*)"/mnt/ipc_socket");
        task_msg_prepare(stringvalue);

        iniparser_freedict(ini);
    }
    else
    {
        sprintf(filename, "/usr/local/extapps/acMeter/bin/%s", AC_METER_INI_FILE);
        ini = iniparser_load(filename);
        if (NULL != ini)
        {
            /* ��ȡipc·�� */
            stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", (char*)"/mnt/ipc_socket");
            task_msg_prepare(stringvalue);

            iniparser_freedict(ini);
        }
        else
        {
            INIT_FMT_DEBUG("�����ļ�[%s]��ȡʧ��!\n", filename);
        }
    }

    return 0;
}

uint32 shandong_get_ppp1_ip(char *ucPPPName)
{
    int fd;
    struct ifreq ifr;
    uint32 ipAddr = 0;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
        return 0;
    }

    strcpy(ifr.ifr_name, ucPPPName);
    if(0 == ioctl(fd, SIOCGIFFLAGS, &ifr))
    {
        if(0 == ioctl(fd, SIOCGIFADDR, &ifr))
        {
            ipAddr = ((struct sockaddr_in*)(&(ifr.ifr_addr)))->sin_addr.s_addr;
            ipAddr = ntohl(ipAddr);
        }
    }

    close(fd);
    return ipAddr;
}

/**********************************************************************
* @name��      shandong_ppp1_route
* @brief��    ɽ����������·������, ���ǹ�����Ҫȥ��
* @param[in] ��
* @param[out]��
* @return    ��
* @Date      ��2022-4-22
**********************************************************************/
void shandong_ppp1_route()
{
    uint32 pppIP = 0;
    /* ���·�� */
    FILE *fp;
    char ucRoute[256] = {0};
    char ucSystem[256] = {0};

    pppIP = shandong_get_ppp1_ip("ppp1");
    if(20 == ((pppIP >> 24) & 0xff) && 0 == ((pppIP >> 16) & 0xff) && (0 == ((pppIP >> 8) & 0xff)))
    {
        sprintf(ucSystem, "route -n|grep ppp1|grep '20.0.0.0'|grep -v UH|awk '{print $1}'");
        fp = popen(ucSystem, "r");
        if(fp)
        {
            fscanf(fp, "%s", ucRoute);
            pclose(fp);
            ucRoute[24] = 0;
            if(0 != strcmp(ucRoute, "20.0.0.0"))
            {
                system("route add -net 20.0.0.0/24 dev ppp1");
            }
        }
    }
    memset(ucSystem, 0, 256);
    memset(ucRoute, 0, 256);
    sprintf(ucSystem, "ifconfig usb1|grep 'inet addr:'|awk '{print $2}'");
    fp = popen(ucSystem, "r");
    if(fp)
    {
        fscanf(fp, "%s", ucRoute);
        pclose(fp);
        if(0 != strlen(ucRoute))
        {
            system("ifconfig usb1 down");
        }
    }
}

/**********************************************************************
* @name��      ac_Version_parser
* @brief��     �汾��Ϣ����
* @param[in] ��oad    -  oad
               pInMsg - ��������(������OAD)
               inLen  - �������
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
void get_samp_prtl(uint8 *pSampPrtl)
{
    uint8 sampPrtl = *pSampPrtl;

    uint32 cnt = 0;

    //�����ļ��������˲ɼ���ʽ��0 - 07��Լ; 1 - 698��Լ
    if ((sampPrtl == 0) || (sampPrtl == 1))
    {
        return;
    }

    //�ȳ�ʼ�����豸
    while ((fd_ac = ac_sample_dev_open()) < 0)
    {
        cnt ++;
        if (cnt > 10)
        {
            INIT_FMT_DEBUG("ac_sample_dev_open err fd[%d]\n", fd_ac);
            exit(1);
        }
        fd_ac = -1;
        sleep(1);
    }

    //��֤�Ƿ�֧��698��Լ
    for (cnt = 0; cnt < 3; cnt++)
    {
        if (ERR_OK == ac_get_698ver_bak(&fd_ac))
        {
            break;
        }
    }

    if (cnt >= 3)
    {
        sampPrtl = 0;
        INIT_FMT_DEBUG("��֤ʧ�ܣ��װ���07���� sampPrtl[%d]\n", sampPrtl);
    }
    else
    {
        sampPrtl = 1;
        INIT_FMT_DEBUG("��֤�ɹ����װ���698���� sampPrtl[%d]\n", sampPrtl);
    }

    *pSampPrtl = sampPrtl;

    ac_sample_dev_close(&fd_ac);

    return;
}

#if DESC("��ȡ���ɽ���״̬", 1)
/*******************************************************************************
* ��������: ac_get_power_type
* ��������: ��ȡ���߷�ʽ��Լ����
* �������: fd_spi �������
* �������: ��
* �� �� ֵ: >0    �ɹ�
*           <0    ʧ��
*******************************************************************************/
int ac_get_power_type(int32 *pfd)
{
    int   ret = -1;
    int32 recvLen = 0;
    int32 sendLen = 0;
    uint8 recvBuf[1024] = { 0 };
    uint8 sendBuf[1024] = { 0 };
    OOP_OAD_U oad = {0x40100200};

    int32 fd_spi = *pfd;

    if (fd_spi < 0)
    {
        fd_spi = ac_sample_dev_open();
        if (fd_spi < 0)
        {
            return ERR_NO_HANDLE;
        }
        else
        {
            *pfd = fd_spi;
            UPDATE_FMT_DEBUG("msg: the spi open ok.\n");
        }
    }

    sendLen = ac_get_msg_package(oad, sendBuf, 1024, 0);
    if (sendLen < 0)    //�������
    {
        return ERR_NORMAL;
    }

    if (gUartConf.enable)
    {
        ret = commdrv_Send_232(fd_spi, sendBuf, sendLen);
    }
    else
    {
        ret = ac_spi_send_data(fd_spi, sendBuf, sendLen, recvBuf, 500000);
    }

    if (ret < 0)
    {
        INIT_FMT_DEBUG("fd_spi = %d send ret = %d\n", fd_spi, ret);
        ac_sample_dev_close(pfd);
        return ERR_IFACE;
    }
    else
    {
        INIT_BUF_DEBUG(sendBuf, sendLen, "��ȡ���߷�ʽ������");
    }

    if (gUartConf.enable)
    {
        recvLen = commdrv_Recv_232(fd_spi, recvBuf, sizeof(recvBuf), 300, 8, 0xFF);
    }
    else
    {
        recvLen = ac_spi_recv_data_timeout(fd_spi, sendBuf, recvBuf, sizeof(recvBuf), 50000, 65);
    }

    if (recvLen > 0)
    {
        INIT_BUF_DEBUG(recvBuf, recvLen, "��ȡ���߷�ʽ������");
        ret = ac_read_sample_parser(SAMPLE_OTHER, recvBuf, recvLen);
        if (ret != ERR_OK)
        {
            INIT_FMT_DEBUG("��֡����쳣[%d]\n", ret);
            return ret;
        }
    }
    else
    {
        return ERR_TIMEOUT;
    }

    return ERR_OK;
}

/**********************************************************************
* @name��       get_phase_wire_type
* @brief��      ��ȡ����״̬
* @param[in] ��
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
void get_samp_power_type(void)
{
    char *type[] = {"δ֪", "�����", "��������", "��������"};

    uint32 cnt = 0;

    //�ȳ�ʼ�����豸
    while ((fd_ac = ac_sample_dev_open()) < 0)
    {
        cnt ++;
        if (cnt > 10)
        {
            INIT_FMT_DEBUG("ac_sample_dev_open err fd[%d]\n", fd_ac);
            exit(1);
        }
        fd_ac = -1;
        sleep(1);
    }

    //��ȡ����״̬
    for (cnt = 0; cnt < 3; cnt++)
    {
        if (ERR_OK == ac_get_power_type(&fd_ac))
        {
            break;
        }
    }

    if (cnt >= 3)
    {
        INIT_FMT_TRACE("��ȡ���߷�ʽʧ�� ����[%s]\n", type[TermSharedData.powerType]);
    }
    else
    {
        INIT_FMT_TRACE("��ȡ���߷�ʽ�ɹ� ����[%s]\n", type[TermSharedData.powerType]);
    }

    ac_sample_dev_close(&fd_ac);

    return;
}

/**********************************************************************
* @name��       ac_power_type_update
* @brief��      ��ȡ����״̬���µ���������
* @param[in] ��
* @param[out]��
* @return    ��������
* @Date      ��2019-12-6
**********************************************************************/
void ac_power_type_update(void)
{
    int  ret = -1;
    char *type[] = {"δ֪", "�����", "��������", "��������"};

    NOMAL_OAD_T nomalOad = {0};

    nomalOad.logicId = 1;  //�߼���ַ
    nomalOad.infoNum = 0;  //��Ϣ��

    if (gDataCenterClient <= 0)
    {
        return;
    }

    if (TermSharedData.powerType > 0)
    {
        nomalOad.oad.value = 0x40100200;
        nomalOad.classtag = CLASS_DATA_INIT;
        ret = db_write_nomal(gDataCenterClient, &nomalOad, (uint8*)&TermSharedData.powerType, sizeof(TermSharedData.powerType));
        if (ret != 0)
        {
            INIT_FMT_DEBUG("���߷�ʽд����������ʧ�� ������[%d] ����״̬[%s]\n", ret, type[TermSharedData.powerType]);
        }
        else
        {
            INIT_FMT_DEBUG("���߷�ʽд���������ĳɹ� ����״̬[%s]\n", type[TermSharedData.powerType]);
        }
    }
}

#endif

#if DESC("�������ϵ�5min��λ�ն�", 1)

#define POWER_CHARGE_CONTROL  0    /* �Ƿ��ϵ�30���Ӻ�ſ�����س�� */

int system_get_main_cell_power(int *value)
{
    int  ret = 0;
	ADC_DEVICE_T* dev = (ADC_DEVICE_T*)hal_device_get(HW_DEVICE_ID_ADC);
	if(NULL == dev)
	{
        return -1;
	}
	ret = dev->adc_volt_read(dev, EMADC_BATVOL_CHANNEL, value);
	hal_device_release((HW_DEVICE *)dev);
	return ret;
}

void *pthread_check_poweroff(void *arg)
{
    int32 lastpowerstate = -1;
    int32 lastpowerstate1 = 1;
    int32 lastpowerstate2 = 1;
    int32 lastpowerstate3 = 1;
    int32 powerstate;
    int ret = 0;
    int value = 0;
    int celllefttime = -1;  //�����������ʣ��
    POWER_DEVICE_T* dev = NULL;
    prctl(PR_SET_NAME, "pthread_check_poweroff");

    while(1)
    {
        if(NULL == dev)
        {
            dev = (POWER_DEVICE_T*) hal_device_get(HW_DEVICE_ID_POWER);
            if(dev)
            {
                dev->power_battery_charge_set(dev, 0, 1);
                dev->power_battery_charge_set(dev, 1, 1);
            }
        }
        sleep(1);

        if(NULL == dev)
        {
            continue;
        }

        if(dev->power_power_state_get(dev, POWER_MAIN, &powerstate) < 0)
        {
            continue;
        }

        if(powerstate == lastpowerstate1 && powerstate == lastpowerstate2 && powerstate == lastpowerstate3)
        {
            if(0 == lastpowerstate && powerstate == 1)
            {
                INIT_FMT_DEBUG("��⵽��Դ����\n");
                celllefttime = -1;
            }
            else if(1 == lastpowerstate && powerstate == 0)
            {
                INIT_FMT_DEBUG("��⵽��Դ�ϵ�, ֹͣ��ء����ݳ��\n");
                celllefttime = 300;

                g_nPowerOffFlag = 1;
            }
            else if(-1 == lastpowerstate && powerstate == 0)
            {
                //�ն�ͨ����������û�е�Դ���磬ֱ���ǵ�أ�5���Ӻ�ϵ�
                INIT_FMT_DEBUG("�޵�Դ����(��ع���)\n");
                celllefttime = 300;
            }
            lastpowerstate = powerstate;
        }
        lastpowerstate1 = lastpowerstate2;
        lastpowerstate2 = lastpowerstate3;
        lastpowerstate3 = powerstate;

        if(0 == celllefttime && 0 == lastpowerstate)
        {
            /* ����ʱ���� ��λ */
            WATCHDOG_DEVICE_T* dogdev = (WATCHDOG_DEVICE_T*)hal_device_get(HW_DEVICE_ID_WATCHDOG);
            INIT_FMT_DEBUG("�޵�Դ��������300s, �ն˸�λ!!!\n\n");
            celllefttime = -1;
            reboot_to_smiOS();
            sleep(10);
            if (dogdev)
            {
                ret = dogdev->watchdog_period_set(dogdev, 1);
                if (ret != ERR_OK)
                {
                    INIT_FMT_DEBUG("set watchdog period 1s err\n");
                }
                hal_device_release((HW_DEVICE *)dogdev);
            }
            continue;
        }
        else if(1 == lastpowerstate)
        {
            celllefttime = -1;
            continue;
        }

        ret = system_get_main_cell_power(&value);
        if(celllefttime >= 0 && 0 == ret && value > 0)
        {
            celllefttime--;
        }
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }

    return 0;
}

#endif

int ac_check_cmd_exist(char *cmd)
{
    BOOL ret = FALSE;
    char pstatus[10] = {0};
    FILE* pipe = NULL;

    if (cmd == NULL)
    {
        return ret;
    }

    pipe = popen(cmd, "r");

    if (!pipe)
    {
        pipe = popen(cmd, "r");
    }

    if (!pipe)
    {
        return ret;
    }

    fgets(pstatus, 10, pipe);
    pclose(pipe);

    if (pstatus[0] >= '1')
    {
        ret = TRUE;
    }

    return ret;
}

void ppp0_test()
{
    const char* cmd = "route add -net 20.0.0.0 netmask 255.0.0.0  dev ppp0";
    if(FALSE == ac_check_cmd_exist((char*)"ifconfig |grep ppp0") ||
        TRUE == ac_check_cmd_exist((char*)"route -n |grep 20.0.0.0"))
    {
        return;
    }
    INIT_FMT_DEBUG("cmd %s\n",cmd);

    system(cmd);
}

/*********************************************************************
 ��������������app������
 ���������argc
           argv
 �����������
 ��������ֵ����
********************************************************************/
int main(int argc, char *argv[])
{
    int           err  = 0;               // ���߳�ʱ�����صĴ����
    int           ret  = 0;
    int           i    = 0;
    MAIN_TRHEAD_T *pthread = NULL;
    int           count = 0;
    //�շ�������г�ʼ��
    LockQueue_T LockDealRecvDown[RUN_DOWN_NUM]; 					 //�����߳����н��չ����ڴ�ָ��
    LockQueue_T LockDealSendDown[RUN_DOWN_NUM]; 					 //�����߳����з��͹����ڴ�ָ��
    LockQueue_T LockUdpRecvDown; 					                 //UDP͸�����н��չ����ڴ�ָ��
    LockQueue_T LockUdpSendDown; 					                 //UDP͸�����з��͹����ڴ�ָ��
    LockQueue_T LockTimeRecvDown; 					                 //��ʱ���н��չ����ڴ�ָ��
    LockQueue_T LockTimeSendDown; 					                 //��ʱ���з��͹����ڴ�ָ��
    LockQueue_T LockModuleRecvUp;	                                 //ģ����Ϣ���н��չ����ڴ�ָ��
    LockQueue_T LockConfirmDown;	                                 //�ظ�����ģ��ȷ�Ϲ����ڴ�ָ��

    uint16  CheckSeconds = 120;  //�߳��쳣�������

    /* ������ֻ������һ������ */
    if (!singleton_check("acMeter"))
    {
        return -1;
    }

    if (argc >= 2)
    {
        if (0 == memcmp(argv[1], "debug", strlen("debug")))
        {
            printf("30s waiting...\n");
            sleep(30);
        }
    }

    //���CPU�ֽ�����һ���ն˱�Ȼ���ڵĽ��������
    check_endian();

    /* hal��ʼ�� */
    ret = hal_init();
    while(0 != ret)
    {
        INIT_FMT_DEBUG("hal��ʼ��ʧ��!\n");
        ret = hal_init();
        sleep(2);
    }

    /* �źŴ��� */
    init_sig_action(NULL);

    /* ��ȡ�����ļ� */
    init_config();
    INIT_FMT_DEBUG("Version: %s Time: %s, area: %s, Build: %s %s\n", APP_VERSION, APP_PUBLISH_TIME, AREANAME_PLACE,
        __DATE__, __TIME__);
    INIT_FMT_DEBUG("�������ļ�. mqttIP = %s, mqttport = %d\n", mqttIP, mqttport);

    init_485_config();
    INIT_FMT_DEBUG("�Ƿ�֧��ת��485���ڱ���[%d]\n", gSupportRelay485Flag);

    /* ���ڷ�����ͬ����Ϣ��ʼ�� */
    ac_485comm_msg_init();

    /* ȫ�ֱ�����ʼ�� */
    init_global();

    /* ������ʼ�� */
    ac_other_init();

    /* mqtt��ʼ�� */
    INIT_FMT_DEBUG("׼������mqtt��ʼ��... \n");
    ret = mqtt_init();
    if (ret < 0)
    {
        INIT_FMT_DEBUG("MQTT��ʼ��ʧ��! ret=%d \n", ret);
        exit(1);
    }

    /* ���ݸ��³�ʼ�� */
    db_updata_init();

    /* ��������ļ��У����ڲɼ���ԼΪ����Ӧ�����Զ���ȡ�װ�ɼ���ʽ */
    get_samp_prtl(&gSampPrtl);

    /* ��ȡ���߷�ʽ */
    get_samp_power_type();

    /* hal��ȡappid */
    //appid_query(AppIdName, AppId, APP_ID_LEN);

    /* ������������agent */
    gDBAgent = db_agent_create(AGENT_TYPE_THREAD, DB_AC_AGENT, info_to_dbCenter, DB_CLIENT_MAX);
    while (gDBAgent < 0)
    {
        INIT_FMT_DEBUG("����dbagentʧ��! gDBAgent = %d\n", gDBAgent);
        gDBAgent = db_agent_create(AGENT_TYPE_THREAD, DB_AC_AGENT, info_to_dbCenter, DB_CLIENT_MAX);
        sleep(1);
    }

    //�����߳����н��չ����ڴ�ָ��
    for (i = 0; i < RUN_DOWN_NUM; i++)
    {
        memset(&LockDealRecvDown[i], 0, sizeof(LockDealRecvDown[i]));
        if (pthread_mutex_init(&LockDealRecvDown[i].QueueLock, NULL) != 0)
        {
            INIT_FMT_DEBUG("init pthread LockDealRecvDown lock error! i=%d \n", i);
            exit(1);
        }
        pDealRecvDown[i] = &LockDealRecvDown[i];
    }

    //�����߳����з��͹����ڴ�ָ��
    for (i = 0; i < RUN_DOWN_NUM; i++)
    {
        memset(&LockDealSendDown[i], 0, sizeof(LockDealSendDown[i]));
        if (pthread_mutex_init(&LockDealSendDown[i].QueueLock, NULL) != 0)
        {
            INIT_FMT_DEBUG("init pthread LockDealSendDown lock error! i=%d \n", i);
            exit(1);
        }
        pDealSendDown[i] = &LockDealSendDown[i];
    }

    //UDP͸�����н��չ����ڴ�ָ��
    memset(&LockUdpRecvDown, 0, sizeof(LockUdpRecvDown));
    if (pthread_mutex_init(&LockUdpRecvDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockDealRecvDown lock error!\n");
        exit(1);
    }
    pUdpRecvDown = &LockUdpRecvDown;

    //UDP͸�����з��͹����ڴ�ָ��
    memset(&LockUdpSendDown, 0, sizeof(LockUdpSendDown));
    if (pthread_mutex_init(&LockUdpSendDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockUdpSendDown lock error!\n");
        exit(1);
    }
    pUdpSendDown = &LockUdpSendDown;

    //��ʱ���н��չ����ڴ�ָ��
    memset(&LockTimeRecvDown, 0, sizeof(LockTimeRecvDown));
    if (pthread_mutex_init(&LockTimeRecvDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockTimeRecvDown lock error!\n");
        exit(1);
    }
    pTimeRecvDown = &LockTimeRecvDown;

    //��ʱ���з��͹����ڴ�ָ��
    memset(&LockTimeSendDown, 0, sizeof(LockTimeSendDown));
    if (pthread_mutex_init(&LockTimeSendDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockTimeSendDown lock error!\n");
        exit(1);
    }
    pTimeSendDown = &LockTimeSendDown;

    //ģ����Ϣ���н��չ����ڴ�ָ��
    memset(&LockModuleRecvUp, 0, sizeof(LockModuleRecvUp));
    if (pthread_mutex_init(&LockModuleRecvUp.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockModuleRecvUp lock error! \n");
        exit(1);
    }
    pModuleRecvUp = &LockModuleRecvUp;

    //�ظ�����ģ��ȷ�Ϲ����ڴ�ָ��
    memset(&LockConfirmDown, 0, sizeof(LockConfirmDown));
    if (pthread_mutex_init(&LockConfirmDown.QueueLock, NULL) != 0)
    {
        INIT_FMT_DEBUG("init pthread LockConfirmDown lock error! \n");
        exit(1);
    }
    pConfirmDown = &LockConfirmDown;


    for (i = 0; i < g_MainTaskNum; i++)
    {
        pthread = &g_tMainThread[i];
        THREAD_ARG_T *tArg = (THREAD_ARG_T *)pthread->arg;

        if (pthread->isStart)
        {
            err = pthread_create(&pthread->nThreadId, NULL, pthread->pEntryFunc, pthread->arg);
            if (err != 0)
            {
                INIT_FMT_DEBUG("%s create error! err:%d \n", tArg->threadName, err);
                exit(1);
            }
        }
    }

    while (1)
    {
        for (i = 0; i < g_MainTaskNum; i++)
        {
            pthread = &g_tMainThread[i];
            THREAD_ARG_T *tArg = (THREAD_ARG_T *)pthread->arg;

            if ((pthread->isDefend) && (pthread->isStart))
            {
                if (tArg->cnt >= CheckSeconds)           // ��2���Ӽ�ⶼ����������رմ��̣߳�Ȼ�����´򿪴��߳�
                {
                    err = pthread_cancel(pthread->nThreadId);
                    if (err != 0)
                    {
                        INIT_FMT_DEBUG("%s cancel error! err[%d]\n", tArg->threadName, err);
                    }
                    else
                    {
                        pthread_join(pthread->nThreadId, NULL);
                        err = pthread_create(&pthread->nThreadId, NULL, pthread->pEntryFunc, tArg); // ��һ���������ݲ�����ͳ���߳�
                        if (err != 0)
                        {
                            INIT_FMT_DEBUG("%s create error! err:%d \n", tArg->threadName, err);
                        }
                        else
                        {
                            tArg->cnt = 0;
                            INIT_FMT_DEBUG("%s create OK!\n", tArg->threadName);
                        }
                    }
                }
                else
                {
                    tArg->cnt ++;
                }
            }
        }

        sleep(1);
        if(count++ > 10)
        {
#ifdef AREA_SHANDONG
//ɽ��������2���ӵ��Թ���
            shandong_ppp1_route();
#endif
            ppp0_test();

            count = 0;
        }
    }

    return 0;
}

