/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��������ģ��ͷ�ļ�
* @date��    2020-4-9
* @history�� 
*********************************************************************
*/
#ifndef __APPCTRL_ELEC_H_
#define __APPCTRL_ELEC_H_

#ifdef __cplusplus
        extern "C" {
#endif

#define HAVE_PASS_SETVAL    1

#define SET_ALARM_TYPE        1    //-����ֵ�澯-
#define BREAK_ALARM_TYPE    2    //-��բ�澯-

typedef struct
{
    uint8 EcTurnAllSet[OOP_MAX_TG];           //����ִ���Ͷ��
    uint8 EcTurnSet[OOP_MAX_TG][OOP_MAX_TURN];//-����ִ�(��λ1 Ͷ��, 0���)
    uint8 HavePassMonSet[OOP_MAX_TG];    /*-�µ�س���ֵ��־-*/
    uint8 HavePassBuySet[OOP_MAX_TG];    /*-����س���ֵ��־-*/
    uint8 bEcMonPassWarn[OOP_MAX_TG];    /*-�µ��Խ�޸澯��־-*/
    uint8 bEcBuyPassWarn[OOP_MAX_TG];    /*-�����Խ�޸澯��־-*/
    uint8 TelBuyAlarm[OOP_MAX_TG];       /*Զ��ģʽ����澯*/
    int64 MonUsedValue[OOP_MAX_TG];
    int64 MondefValue[OOP_MAX_TG];
    uint8 MonlimitCoe[OOP_MAX_TG];
    int8  MonfloatCoe[OOP_MAX_TG];
    uint8 BuyType[OOP_MAX_TG];          //�������� ������0������ѣ�1��
    int64 BuyLeftValue[OOP_MAX_TG]; 
    int64 BuyAlarmValue[OOP_MAX_TG];    //��������ֵ
    int64 BuySetValue[OOP_MAX_TG];      //��բ����ֵ

    uint16 MonShowLeftTimes[OOP_MAX_TG];
    uint16 BuyShowLeftTimes[OOP_MAX_TG];
}EC_INFO_T;    /*-�����Ϣ-*/

void appctrl_elec_init();
void appctrl_elec_ctrl(int cid, uint8 bPassMon, uint8 turnmax);
void appctrl_elec_status(uint8 turnmax);
void appctrl_callfee_ctrl();
void appctrl_elec_write(uint8 bDay);

#ifdef __cplusplus
}
#endif

#endif


