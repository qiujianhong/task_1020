/*-***************************************************************
* Copyright (c) 2006,�Ͼ����������������޹�˾
*            All rights reserved.
* �������ƣ�eventcomm.h
* �汾�ţ�1.0
* ���ܣ�ʱ�䴦����ʽת��
* ԭʼ�����˼�ʱ�䣺÷���� 2020��04��13��
* �޸���Ա��ʱ�估�޸�����:��
* ������
******************************************************************-*/
#ifndef _EVENTCOMM_H
#define _EVENTCOMM_H

#ifdef __cplusplus
extern "C"{
#endif

/*���ڶ˿ںʹӽڵ���������type = value; mfrs = value; idformat = value; id = value; mmfrs = value; midformat = value; mid = value��*/
typedef struct
{
    char type[10];      //����ͨѶģ������ value������PLC������WIRELESS���� �ڵ��豸���� 
    char mfrs[4];       //оƬ���̴��� 2���ַ���ASCII��
    uint8 idformat;     //оƬID��ʽ���ͣ���valueΪ���¼��֣���0��Ϊ��ϸ�ʽ����1��ΪBCD����2��ΪBIN ����3��ΪASCII
    char id[100];       //оƬID��Ϣ����valueΪ��Ӧ���ַ���,����idformat�ĸ�ʽ������Ӧת��
    char mmfrs[4];      //ģ�鳧�̴��룬��valueΪ2���ַ���ASCII��
    uint8 midformat;    //ģ��ID��ʽ���ͣ����valueΪ���¼��֣���0��Ϊ��ϸ�ʽ����1��ΪBCD����2��ΪBIN ����3��ΪASCII
    char mid[200];      //��ID��Ϣ����valueΪ��Ӧ���ַ���,����midformat�ĸ�ʽ������Ӧת����
}DESCRIPTOR_T;

/*--���������Ͷ���--*/
#define OI_485       0XF201   /*-485��-*/
//#define OI_SIM       2   /*-ģ���-*/
#define OI_PULSE     0XF20A   /*-����-*/
//#define OI_VIR       4   /*-����-*/
#define OI_AC        0XF208   /*-����-*/
#define OI_PLC       0XF209   /*-�ز�-*/
//#define OI_PLCMUI    7   /*-�ز��๦��-*/


void systm_to_ooptm(time_t sys_tm, OOP_DATETIME_S_T *poop_time);
void rtctm_to_ooptm(rtc_t rtc_tm, OOP_DATETIME_S_T *poop_time);

void ooptm_to_rcttm(OOP_DATETIME_S_T oop_time,rtc_t *prtc_tm);
void ooptm_to_systm(OOP_DATETIME_S_T oop_time, time_t *psys_tm);

void systm_to_rtctm(time_t sys_tm, rtc_t *prtc_tm);

//��ȡ��ǰʱ��
void current_tm_to_rtctm(rtc_t *prtc_tm);
void current_tm_to_ooptm(OOP_DATETIME_S_T *poop_time);

int CompareTSA(OOP_TSA_T tsa1, OOP_TSA_T tsa2);

BOOL event_data_is_null(void* data, uint32 len);

DESCRIPTOR_T UnpackDescriptor(uint8 type, char* buf);

#ifdef __cplusplus
}
#endif

#endif


