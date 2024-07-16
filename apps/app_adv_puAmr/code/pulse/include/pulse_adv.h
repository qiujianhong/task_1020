/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ң������ģ��ͷ�ļ�
* @date��    2019-11-19
* @history�� 
*********************************************************************
*/

#ifndef __PULSE_ADVH__
#define __PULSE_ADVH__

#include "appmsg_split.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PULSE_DB_QUEUE_SIZE       40
typedef struct tagDBNotify 
{
	pthread_mutex_t    f_lock;	
	uint32             num;
	uint32             OAD[PULSE_DB_QUEUE_SIZE];
}DB_NOTIFY_T;

typedef struct tagPulseTime
{
    uint8 year;  /*���0����1900��*/
    uint8 month; /*�·�0����1��*/
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;

    uint16 milliseconds;
}TIME_STRUCT_T;    /*ʱ��ṹ*/

typedef struct tagPulseCount
{
    uint32    val;             /*������*/
    uint32    interval;        /*���ʱ��*/
    uint32    caluleft;        /*��������*/

    uint8     powerNum;
    uint8     res[3];
    uint32    powerList[DM_POWER_MAX]; /*��������洢�Ĺ���*/
}PULSE_COUNT_T;


typedef struct tagPulseMeasure
{
    OOP_VISIBLEVAR_T     tAddr;         //ͨ�ŵ�ַ
    OOP_PTCT_T           tPTCT;
    OOP_PULSESETS_T      tPulseCfg;     //��������
    int32                nActivePower;  //�й�����
    int32                nReactivePower;//�޹�����
#if 0
    OOP_ENERGY_T         tDayForwActEn; //���������й�����
    OOP_ENERGY_T         tMonForwActEn; //���������й�����
    OOP_ENERGY_T         tDayBackActEn; //���շ����й�����
    OOP_ENERGY_T         tMonBackActEn; //���·����й�����
    OOP_ENERGY_T         tDayForwRactEn; //���������޹�����
    OOP_ENERGY_T         tMonForwRactEn; //���������޹�����
    OOP_ENERGY_T         tDayBackRactEn; //���շ����޹�����
    OOP_ENERGY_T         tMonBackRactEn; //���·����޹�����
#endif
    OOP_ENERGY_T         tShowForwActEn; //�����й�����ʾֵ
    OOP_ENERGY_T         tShowForwRactEn;//�����޹�����ʾֵ
    OOP_ENERGY_T         tShowBackActEn; //�����й�����ʾֵ
    OOP_ENERGY_T         tShowBackRactEn;//�����޹�����ʾֵ

    OOP_PULSESETS_B_T    tBigCfg;
}PULSE_Measure_T;

typedef struct tagPulseTemp
{
    uint32     count;
    uint32     timems;
    uint32     totalcou;
    uint32     totals;
    uint16     totalms;
    uint8      res[2];
    Date_Time  start;
}PULSE_TEMP_T;

#define PULSE_CHG_MAX                  20
typedef struct tagPulseChgNode
{
    uint32               happentmes;
    uint32               systemtime;
    OOP_SWITCHIN_STATE_T sc; //��λ״̬
}PULSE_CHG_NODE_T;


typedef struct tagPulseChgList
{
    uint8                head;
    uint8                tail;
    PULSE_CHG_NODE_T     list[PULSE_CHG_MAX]; //��λ״̬
}PULSE_CHG_LIST_T;

/* ң��������������������������� */
typedef struct tagPulseOAD
{
    DB_CLIENT            dbclient;    //�������Ŀͻ���
    void                *pHalDev;
    
    uint8                nSwitchNUM;  //ң������ 0xf2030300
    uint8                nPulseNum;   //�����豸���� 
    uint8                bChangeTime; //ң�ŷ���ʱ��仯
    uint8                bInitSwitch; //��ʼ���Ƿ����ң��

    OOP_SWITCHIN_STATE_T tSwitchState; //ң��״̬ 0xf2030200
    OOP_SWITCHIN_FLAG_T  tSwitchFlag;  //ң�Ž���״̬ 0xf2030400
    OOP_STATUS_SIGNAL_T  tStatusSignal;//�źŽڵ�
    uint32               rmtTime;      //ң�ŷ���ʱ�� 0x49080200

    OOP_PULSE_T          tPulseInfo;   //���������豸(Ŀǰû�õ�) 0xf20a0200
    PULSE_Measure_T      tPulse[OOP_MAX_PULSE]; //�������������� 2401~2408

    OOP_DAYPERIOD_T      tDayPeriod;   //��ǰ����ʱ�α� 0x40160200

    uint8                bPowerClose;  //ͣ��
    uint8                bFirstGetRmt1;
    uint8                bFirstGetRmt2;
    PULSE_CHG_LIST_T     tList;
}PULSE_OAD_T;

#define PULSE_MODEL_MAX                      3
typedef struct tagPulseSet
{
    uint8    no;                                    //���;0-��Ч
    uint8    bSetCfg;                               //�Ƿ�֧����������
    uint8    res[1];
    uint8    portNum;                               //�˿�����0-��Ч
    uint8    config[OOP_MAX_SWITCHIN + OOP_MAX_PULSE]; //����: ���8·ң�š�8·����
    uint8    realcfg[OOP_MAX_SWITCHIN + OOP_MAX_PULSE];//ʵ������
}PULAE_SET_T;

typedef struct tagPulseCFG
{
    uint8         bHasPulse;
    uint8         mapSTStart;           //ģ��ң�ſ�ʼλ��(0��ʼ, ��ʾ1·)ͬ�ն˱���ң������
    PULAE_SET_T   cfg[PULSE_MODEL_MAX]; //ң����������

    char          datafile[256];             /* �������ݱ���·�� */
}PULSE_CFG_T;

/* ң�š���������м��������(��ǰ����ң�š��������8·ʵ��) */
/* ң�Ű���ģ�� */
typedef struct tagPulseCalculate
{
    uint8         curST[OOP_MAX_SWITCHIN];    //ң������APP���ص�ң��״̬
    uint8         realST[OOP_MAX_SWITCHIN];   //��ϳ������պ�ң�Ž����ۺϻ��ܵ����
    PULSE_CFG_T  *pCfg;                    //ң����������
    
    uint8         pulseNum;                  /* ģ��ʵ�ʽ������������ */
    PULSE_COUNT_T Pulse[OOP_MAX_PULSE];      //�������ԭʼ����
    PULSE_TEMP_T  tPulseTemp[OOP_MAX_PULSE]; //ģ��ɼ�����ԭʼ����
    char          datafile[256];             /* �������ݱ���·�� */
}PULSE_CALCULATE_T;

uint32 read_oad_notify(DB_NOTIFY_T *pQueue);
void *pulse_outmsg_entry(void *pArg);
int pulse_write_OAD(uint32 uOAD, uint8 *pData, uint32 dataLen);
int pulse_write_INITOAD(uint32 uOAD, uint16 classtag, void *pData, uint32 dataLen);
int pulse_get_OAD(uint32 uOAD, uint8 *pBuf, uint32 bufLen);
int pulse_dbCenter_init(PULSE_OAD_T *ptCfg);
void pulse_get_curtime_s(TIME_STRUCT_T *pTime);
int pulse_calculate_init(PULSE_CALCULATE_T *ptCalu, TIME_STRUCT_T *pTime);
void pulse_proc_calculate(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu, TIME_STRUCT_T *pTime);
int app_set_configure(int taskid, PULSE_CFG_T *pPluseCfg);
void app_set_shaketime(int taskid, uint32 timelong);
void app_get_rmt(int taskid, PULSE_CFG_T *pPluseCfg);
void app_reproc_rmt(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu, uint8 bExit);
void app_proc_rmt_later(PULSE_OAD_T *ptCfg);
void app_proc_rmt(uint8 id, uint8 *pMsg, uint16 msglen, uint8 bReport, PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu);
void app_get_pulse(int taskid, PULSE_CALCULATE_T *ptCalu);
void pulse_change_K(int taskid, uint32 OAD, PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu);
void pulse_check_status(PULSE_OAD_T *ptCfg);

int pulse_update_db(uint32 uOAD, PULSE_OAD_T *ptCfg);
void pulse_write_normal_init(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu);
void pulse_arg_arg_init(PULSE_OAD_T *ptCfg, PULSE_CALCULATE_T *ptCalu);
void pulse_write_pulse(PULSE_CALCULATE_T *ptCalu);

#ifdef __cplusplus
}
#endif


#endif

