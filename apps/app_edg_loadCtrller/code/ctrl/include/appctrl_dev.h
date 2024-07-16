/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����ģ��̵����������豸���
* @date��    2020-4-5
* @history�� 
*********************************************************************
*/
#ifndef __APPCTRL_DEV_H_
#define __APPCTRL_DEV_H_

#ifdef __cplusplus
        extern "C" {
#endif

#define CONTROL_SET_RTS          0X6501  //��������ģʽ
#define CONTROL_CLEAR_RTS        0X6502  //��λ�����͸�λģ��
#define CONTROL_GET_MODSTATE     0x6506  //ģ�����ʶ�� ������0 Ϊ��ģ�飬 1Ϊû�У�


#define SWITCH_CLOSE            0x55  //-բ��ǰ״̬����բ-
#define SWITCH_OPEN             0xaa  //-բ��ǰ״̬����բ-

#define TURN_OFF_ALARM          0x55  //-δ���ڸ澯״̬-
#define TURN_ON_ALARM           0xaa  //-���ڸ澯״̬-

#define TURN_OFF_STATE          0x55  //-δ������բ״̬-
#define TURN_ON_STATE           0xaa  //-������բ״̬-

#define TURN_NON_FLAG           0x55  //-���ִ�δͶ��-
#define TURN_SET_FLAG           0xaa  //-���ִ���Ͷ��-

#define TURN_INTERVAL           60    //-����ʱ����-
#define OUTPUT_DELAY            2     //-բ�������ǰ��ʱ-

// 1�ֽ� �ӿڰ����״̬��־
// D3~D0: ÿλ��Ӧ��1�������բ�� ��Ӧ��0�������բ
// D4��    ��1�������Ԥ������0������Ԥ���ر�
// D5��    ��1������澯�̵����������0������Ԥ���ر�
// D6��    ��1������������������0����������������
#define PREPARE_BIT   0x10        //Ԥ��
#define RELAY_BIT     0x20        //�澯�̵���
#define BELL_BIT      0x40        //������

/*-******************************************************************-*/
/*-                            ���ݽṹ                                 -*/
/*-******************************************************************-*/

typedef struct
{
    uint8  TURN_1_CTRL          :1;    //�ִ�1�������(0��ʾ�̵������  1��ʾ�̵���������)
    uint8  TURN_1_REDLIGHT      :1;    //�ִ�1���(0-Ϩ��1-����)
    uint8  TURN_1_BLUELIGHT     :1;    //�ִ�1�̵�(0-Ϩ��1-����)
    uint8  TURN_2_CTRL          :1;    //�ִ�2�������(0��ʾ�̵������  1��ʾ�̵���������)
    uint8  TURN_2_REDLIGHT      :1;    //�ִ�2���(0-Ϩ��1-����)
    uint8  TURN_2_BLUELIGHT     :1;    //�ִ�2�̵�(0-Ϩ��1-����)
    uint8  TURN_3_CTRL          :1;    //�ִ�3�������(0��ʾ�̵������  1��ʾ�̵���������)
    uint8  TURN_3_REDLIGHT      :1;    //�ִ�3���(0-Ϩ��1-����)
    uint8  TURN_3_BLUELIGHT     :1;    //�ִ�3�̵�(0-Ϩ��1-����)
    uint8  TURN_4_CTRL          :1;    //�ִ�4�������(0��ʾ�̵������  1��ʾ�̵���������)
    uint8  TURN_4_REDLIGHT      :1;    //�ִ�4���(0-Ϩ��1-����)
    uint8  TURN_4_BLUELIGHT     :1;    //�ִ�4�̵�(0-Ϩ��1-����)
    uint8  POWER_CTRL_LIGHT     :1;    //���ص�(0-Ϩ��1-����) �п���Ͷ��ʱ
    uint8  ELE_CTRL_LIGHT       :1;    //��ص�(0-Ϩ��1-����) �п���Ͷ��ʱ
    uint8  ALARM_OUTPUT         :1;    //�澯�̵���(0��ʾ�̵������  1��ʾ�̵���������)
    uint8  PROTECT_LIGHT        :1;    //�����(0-Ϩ��1-����) �п���Ͷ��ʱ
}__attribute__((packed))SWITCH_BITS_T;

typedef struct tagCtrlSwitch
{
    uint32     num;
    uint8      status[OOP_MAX_TURN];
}CTRL_SWITCH_STATUS_T;

typedef struct tagCtrlRs232
{
    uint8 port;
    uint8 subport;
    uint8 res[2];
    char  deviceName[30];        /*�豸�� */
    void *pDev;
    int   fd;
}CTRL_RS232_T;


void appctrl_switch_init(uint8 port, uint8 turnMax);
void appctrl_switch_init_zcu_1();
void appctrl_control_state(uint8 acm1, uint8 acm2, SWITCH_INTERFACE_T *pInterface);
int appctrl_set_switch_close(SWITCH_INTERFACE_T *pInterface);
int appctrl_set_switch_zcu1(SWITCH_INTERFACE_T *pInterface);
int appctrl_set_switch_zcu3(uint8 acm1, uint8 subport1, SWITCH_INTERFACE_T *pInterface);
int appctrl_set_switch(uint8 acm1, uint8 subport1, uint8 acm2, uint8 subport2, SWITCH_INTERFACE_T *pInterface);
int appctrl_get_switch_input_zcu1(uint8 acm1, uint8 acm2, SWITCH_INTERFACE_T *pInterface);
int appctrl_get_switch_input(uint8 acm1, uint8 subport1, uint8 acm2, uint8 subport2, SWITCH_INTERFACE_T *pInterface);
void appctrl_status_write(uint8 bDay);
int appctrl_init_alarm_switch(uint8 acm, uint8 subport);
void appctrl_alarm_out();

#ifdef __cplusplus
}
#endif

#endif

