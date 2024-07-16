/*
*********************************************************************
* Copyright(C) 2024 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���ɵ�������
* @date��    2024-06-27
* @history�� 
*********************************************************************
*/
#ifndef __LOADCTRLLER_STAT_H__
#define __LOADCTRLLER_STAT_H__

#ifdef __cplusplus
    extern "C" {
#endif

#define EN_SHARP		1		/*�������*/
#define EN_TOP			2		/*�������*/
#define EN_LEVEL		3		/*����ƽ��*/
#define EN_BOTTOM		4		/*�����ȶ�*/

#define POS_AE_Tag      0		/*00�����й�*/
#define POS_RE_Tag	    1		/*10�����޹�*/
#define NEG_AE_Tag	    2		/*01�����й�*/
#define NEG_RE_Tag	    3		/*11�����޹�*/

#define DM_POWER_MAX    15      /*��������������*/
typedef struct tagLctrlPulseCount
{
    uint32    val;             /*������*/
    uint32    interval;        /*���ʱ��*/
    uint32    caluleft;        /*��������*/

    uint8     powerNum;
    uint8     res[3];
    uint32    powerList[DM_POWER_MAX]; /*��������洢�Ĺ���*/
}LCTRL_PULSE_COUNT_T;

typedef struct tagLctrPulseTemp
{
    uint32     count;      //����ͳ�������������������0
    uint32     timems;     //����ͳ������ͳ��ʱ�䣬�������0
    uint32     totalcou;   //�ϴ�ͳ������
    uint32     totals;     //�ϴ�ͳ����ʱ����
    uint16     totalms;    //�ϴ�ͳ����ʱ�����
    uint8      res[2];
    Date_Time  start;      //�ϴ�ͳ������ʱ��
}LCTRL_PULSE_TEMP_T;


/* ң�š���������м��������(��ǰ����ң�š��������8·ʵ��) */
/* ң�Ű���ģ�� */
typedef struct tagLcPulse
{
    uint8               pulseNum;                  /* ģ��ʵ�ʽ������������ */
    LCTRL_PULSE_COUNT_T Pulse[OOP_MAX_PULSE];      //�������ԭʼ����
    LCTRL_PULSE_TEMP_T  tPulseTemp[OOP_MAX_PULSE]; //ģ��ɼ�����ԭʼ����
}LCTRL_PULSE_T;

typedef struct tagLcPulseCFG
{
    OOP_VISIBLEVAR_T     tAddr;         //ͨ�ŵ�ַ
    OOP_PTCT_T           tPTCT;
    OOP_PULSESETS_T      tPulseCfg;     //��������
    int32                nActivePower;  //�й�����
    int32                nReactivePower;//�޹�����

    OOP_ENERGY_T         tDayForwActEn; //���������й�����
    OOP_ENERGY_T         tMonForwActEn; //���������й�����
    OOP_ENERGY_T         tDayBackActEn; //���շ����й�����
    OOP_ENERGY_T         tMonBackActEn; //���·����й�����
    OOP_ENERGY_T         tDayForwRactEn; //���������޹�����
    OOP_ENERGY_T         tMonForwRactEn; //���������޹�����
    OOP_ENERGY_T         tDayBackRactEn; //���շ����޹�����
    OOP_ENERGY_T         tMonBackRactEn; //���·����޹�����

    OOP_ENERGY_T         tShowForwActEn; //�����й�����ʾֵ
    OOP_ENERGY_T         tShowForwRactEn;//�����޹�����ʾֵ
    OOP_ENERGY_T         tShowBackActEn; //�����й�����ʾֵ
    OOP_ENERGY_T         tShowBackRactEn;//�����޹�����ʾֵ

    OOP_PULSESETS_B_T    tBigCfg;
}
LCTRL_PULSE_CFG_T;

typedef struct tagLCtrlSATA
{
    OOP_DATETIME_S_T     lastTime;                  /* ����ʱ�� */
    
    /* ������������� */
    OOP_DAYPERIOD_T      tDayPeriod;                   //��ǰ����ʱ�α� 0x40160200
    LCTRL_PULSE_CFG_T    tPulseOOP[OOP_MAX_PULSE];     //����������������������2401~2408
    LCTRL_PULSE_T        tPulseData;                   //����ӿ�

    /* ͳ�Ʋ��������� */
    
}LCTRL_SATA_T;



#ifdef __cplusplus
}
#endif

#endif //__LA_MSG_H__

