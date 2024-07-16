/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   �澯����ʾ
* @date��    2020-4-14
* @history�� 
*********************************************************************
*/
#ifndef __APPCTRL_ALARM_H_
#define __APPCTRL_ALARM_H_

#ifdef __cplusplus
        extern "C" {
#endif


/*-ÿ���澯��״̬-*/
#define NEW_ALARM   1        /*-�¸澯-*/
#define ALARM_ING   2        /*-�澯������-*/
#define ALARM_OVER  0        /*-�澯����-*/

/*-�澯���̲�-*/
#define ALARM_INIT_STEP   0    /*-�澯��ʼ�����̲�-*/
#define ALARM_OUTPUT_STEP 1    /*-�澯������̲�-*/
#define ALARM_DELAY_STEP  2    /*-ʱ��ˢ��-*/

#define ALARM_INTERVAL  5    /*-�澯���ʱ�� 5s-*/
#define ALARM_TIMES     3    /*-�澯����-*/

typedef struct
{
    uint8  status;          /*-��ʾ״̬,0:��բ,1:������բ,2:��բ,3:������բ-*/
    uint8  bShowTime;       /*�Ƿ���ʾʱ��*/
    uint8  timeType;        /*ʱ���ʽ0-��:��, 1-ʱ:��:��*/
    uint8  res[1];
    uint32 delaytime;       /*-ʣ��ʱ��-*/
}ALARM_INFO_T;  /*-�澯��Ϣ���ݽṹ-*/

typedef struct tagCtrlAlarm
{
    uint8        ctrlType;      /*��������*/
    uint8        alarmStep;     /*-�澯״̬,0:�澯����,1:�¸澯,2:ʱ��ˢ��-*/
    uint8        alarmType;     /*-�澯����,D0:�����澯,D1:��ʾ����澯-*/

    uint8        alarmInterval; /*-���θ澯���ʱ��-*/
    uint8        alarmTimes;    /*-�澯���� ���������澯-*/

    uint8        timeCount;     /*-��һ�������澯����ʱ�����-*/
    uint8        voiceIndex;    /*-�������� ʹ���˷�������ǰû���õ�-*/

    uint8        group;         /*�ܼ������0-7*/
    void        *pFunc;         /*������ʾ��ʾ��ĺ���*/
}ALARM_CTRL_T;


void appctrl_alarm_init(uint8 maxturn);
void appctrl_system_alarm(uint8 turnmax);
void appctrl_set_key(uint8 status);

#ifdef __cplusplus
}
#endif

#endif


