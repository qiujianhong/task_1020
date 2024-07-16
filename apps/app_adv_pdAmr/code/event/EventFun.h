/*-***************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
* �������ƣ�EventFun.h
* �汾�ţ�1.0
* ���ܣ����¼����������õ������ݽṹ�����Ⱥ�����������
* ԭʼ�����˼�ʱ�䣺���� 2006��12��25��
* �޸���Ա��ʱ�估�޸�����:��
* ������
******************************************************************-*/
#ifndef _EVENTFUN_H
#define _EVENTFUN_H

typedef struct
{
    uint8   power_type;                         //��������߷�ʽ
    int32   cur_a;                              //A�����
    int32   cur_b;                              //B�����
    int32   cur_c;                              //C�����
    uint32  cur_gate;                           //�����-�������ֵ������*10,�����3λС���ĵ����Ƚ�
    uint16  vlt_gate;                           //���ѹ
    uint32  cur_lmt;                            //��ѹ���ࡢʧѹʱ������ֵ-
    uint16  vlt_a;                              //A���ѹ
    uint16  vlt_b;                              //B���ѹ
    uint16  vlt_c;                              //C���ѹ
    uint8   batt_status;                        //���״̬
    uint8   cb_interval;                        //������
}EVENT_GLOBAL_T;

//�궨��
#define BIT0                         0x01
#define BIT1                         0x02
#define BIT2                         0x04
#define BIT3                         0x08
#define BIT4                         0x10
#define BIT5                         0x20
#define BIT6                         0x40
#define BIT7                         0x80

//�¼���Ƕ���
#define BREAK_CIRCUIT                 0x00001     //������·
#define CUR_RVS                       0x00002     //����������
#define BREAK_PHASE                   0x00004     //��ѹ����
#define LOSE_VOLT                     0x00008     //��ѹȱ�ࣨʧѹ��
#define PHASE_REVER                   0x00010     //������
#define SUPASS_VLT                    0x00020     //��ѹ������
#define LOW_VLT                       0x00040     //��ѹ������
#define HIGH_CUR                      0x00080     //����������
#define HIGHER_CUR                    0x00100     //������������
#define CUR_NBLC                      0x00800     //������ƽ��
#define VLT_NBLC                      0x01000     //��ѹ��ƽ��
#define SHORT_CIRCUIT                 0x100000    //������·

#define A_PHASE                       0x01         //a��
#define B_PHASE                       0x02         //b��
#define C_PHASE                       0x04         //c��

#define METER_STATUS_LABEL             0x13

#define     Tmn_Ram_Info_ID      0                      // �ն��ڴ��Լ���Ϣ,1�ֽ�
#define     Tmn_Flash_Info_ID    (Tmn_Ram_Info_ID+1)    // �ն�Flash�Լ���Ϣ,��ʽͬ��
#define     Tmn_Eeprom_Info_ID   (Tmn_Flash_Info_ID+1)  // �ն�EEPROM�Լ���Ϣ,��ʽͬ��
#define     Tmn_Rtc_Info_ID      (Tmn_Eeprom_Info_ID+1) // �ն�RTC�Լ���Ϣ,��ʽͬ��
#define     RTUSelfCheckAddr     (Tmn_Rtc_Info_ID+1)    // 2�ֽڣ��ն��¼���¼��D0��ʾ�ն�ͣ�磬D1��ʾ�Լ��쳣����������
#define     GBLostvltAddr        (RTUSelfCheckAddr+2)   // 9�ֽڣ���ѹ�����¼��ַ
#define     GBLowvltAddr         (GBLostvltAddr+9)      // 9�ֽڣ���ѹʧѹ��¼��ַ
#define     GBVltUpAddr          (GBLowvltAddr+9)       // 9�ֽڣ���ѹԽ���޼�¼��ַ
#define     GBVltDownAddr        (GBLowvltAddr+9)       // 9�ֽڣ���ѹԽ���޼�¼��ַ
#define     GBCurrUpAddr         (GBVltDownAddr+9)      // 9�ֽڣ�����Խ���޼�¼��ַ��Ҳ��Ϊ��������
#define     GBCurrDownAddr       (GBCurrUpAddr+9)       // 9�ֽڣ�����Խ���޸澯��ַ��Ҳ��Ϊ��������
#define     GBVltUnbAddr         (GBCurrDownAddr+9)     // 9�ֽڣ���ѹ��ƽ���¼��ַ
#define     GBCurrUnbAddr        (GBVltUnbAddr+9)       // 9�ֽڣ�������ƽ���¼��ַ
#define     GBPhaseReveAddr      (GBCurrUnbAddr+9)      // 9�ֽڣ����������Լ�¼��ַ
#define     GBVltAbnormalAddr    (GBPhaseReveAddr+9)    // 9�ֽڣ������쳣(��ѹ)��¼��ַ
#define     GBCurrAbnormalAddr   (GBVltAbnormalAddr+9)  // 9�ֽڣ������쳣(����)��¼��ַ
#define     EVENT_RAMX_FILE_SZIE (GBCurrAbnormalAddr+9)

//��������
int   WriteRamXData(int off, char *buf, int len);
int   ReadRamXData(int off, char *buf, int len);

int   WriteEventToDB(mtrnum_t mtr_id, EVENT_COMM_INFO *eventCommInfo, OOP_EVTSOURCE_T eventSource, OOP_EVTBUF_T eventData, uint8 bType);
void  OopDateTimeSToLinuxTime(OOP_DATETIME_S_T *dateTime, time_t *linuxTim);

void  FillEventGlobal(mtrnum_t mtr_id, OOP_METER_T *oopMeter);                          //����������ȫ�ֽṹ��ʼ��
void  CurReverseEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType);             //���������Լ��
void  VltBreakEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType);               //��ѹʧѹ�������ж�
void  PhaseReverseEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter);                        //�������ж�
void  PhaseReverseEvent07(mtrnum_t mtr_id, uint8 type);                                 //�������ж�
void  VltCurImbalanceEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType);        //��ѹ��ƽ���¼�����/�ָ�
void  VltCurImbalanceEvent07(mtrnum_t mtr_id, uint8 type);                              //��ѹ��ƽ���¼�����/�ָ�
void  VltSupassEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter,uint8 bType);               //��ѹԽ�޼��ָ�
void  VltSupassEvent07(mtrnum_t mtr_id, uint8 type);                                    //��ѹԽ�޼��ָ�
void  CurHighEvent(mtrnum_t mtr_id, OOP_METER_T *oopMeter, uint8 bType);                //����Խ�޼��ָ�
void  SaveFileRecBfg(uint8 bEventType,uint8 bPhase,uint8 msn);

//��ȡ��ǰʱ��
void  GetCurrentOopDateTimeS(OOP_DATETIME_S_T *dateTime);

#endif
