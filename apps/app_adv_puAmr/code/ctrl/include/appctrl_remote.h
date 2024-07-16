/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   Զ������ģ��ͷ�ļ�
* @date��    2020-4-10
* @history�� 
*********************************************************************
*/
#ifndef __APPCTRL_REMOTE_H_
#define __APPCTRL_REMOTE_H_

#ifdef __cplusplus
        extern "C" {
#endif

#define SWITCH_IDLE_STEP        0   /*-��ʼ�����̲�-*/
#define SWITCH_START_STEP       1   /*-ң������Ԥ������̲�-*/
#define SWITCH_ALARM_STEP       2   /*-�澯��ʱ������̲�-*/
#define SWITCH_OPEN_STEP        3   /*-ң����բ�޵���ʱ���̲�-*/

#define SWITCH_TYPE_OPEN        0   /* ��բ�̵��� */
#define SWITCH_TYPE_CLOSE       1   /* ��բ�̵��� */

typedef struct
{
    uint8  switchtype[OOP_MAX_TURN * 2];      /*-�̵�������*/
    uint8  turnNO[OOP_MAX_TURN * 2];          /*-�ִ���ţ�0-7��*/
    uint8  bSwitchStep[OOP_MAX_TURN * 2];     /*-״̬����־-*/
    uint8  btripNo[OOP_MAX_TURN * 2];         /*-�������-*/
    uint16 wOpenBrakeDelay[OOP_MAX_TURN * 2]; /*-�澯�뵹��ʱ-*/    
    uint32 wLimitEcTime[OOP_MAX_TURN * 2];    /*-�ƻ��޵�ʱ��-*/
    uint8  bLimitForever[OOP_MAX_TURN * 2];   /* �Ƿ������޵� */
    uint8 bfgFirstRtrl[OOP_MAX_TURN * 2];     /*-��һ�β��������Ӻ�ң�ؼ�¼��־-*/
    uint8 bTwoMin[OOP_MAX_TURN * 2];          /*-�����Ӽ�ʱ-*/
    uint32 oad[OOP_MAX_TURN * 2];             /* �̵���OAD */
    int64 TwoMinPower[OOP_MAX_TURN * 2][OOP_MAX_TG]; /* ң����բ��2min�ܼ����й����� */
    int64 actPower[OOP_MAX_TURN * 2][OOP_MAX_TG];    /* ң����բ�����ܼ����й����� */

}REMOTE_INFO_T;  /*-ң����Ϣ���ݽṹ-*/

void appctrl_remote_init();
void appctrl_remote_ctrl(int cid, uint8 bDayPass, uint8 nmax);
void appctrl_remote_status(uint8 turnmax);
void appctrl_remote_write(uint8 bDay);

#ifdef __cplusplus
}
#endif

#endif


