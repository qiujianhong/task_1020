/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����Զ�̲���ģ��GPRS��GSM����ͷ�ļ�
* @date��    2019-10-29
* @history�� 
*********************************************************************
*/

#ifndef __GPRS_GSM_H__
#define __GPRS_GSM_H__

#include "wireless.h"

#ifdef __cplusplus
extern "C" {
#endif


#define AT_INIT                       0         /*��ʼ��*/
#define AT_ST                         1         /*ģ�鿪���󣬵ȴ�ģ��״̬*/
#define AT_AT                         2         /*����AT���ģ��*/
#define AT_CPIN                       3         /*�ж�SIM������*/
#define AT_GMM                        4         /*��ģ���ͺ�*/
#define AT_CCID                       5         /*��ȡSIM����*/
#define AT_CSQ                        6         /*�������ź�*/
#define AT_SYSINFO                    7         /*ע������*/
#define AT_CREG                       8         /*ע������*/
#define AT_CGDCONT                    9         /*����APN*/
#define AT_CGATT                      10        /*���gprs����*/
#define AT_ATDT                       11        /*����*/
#define AT_PPP                        12        /*PPP��֤*/
#define AT_LCP                        13        /*PPP��·Э��*/
#define AT_IPCP                       14        /*PPP���ڼ��*/
#define AT_RESTART                    15        /*ATʧ��ģ��ϵ�*/
#define AT_WAIT                       16        /*�ȴ������߳�״̬*/
#define PPP_SUCCESS                   17        /*���ųɹ�*/
#define PPP_CHECK                     18        /*����״̬���*/
#define CONNECT_FAILED                19        /*����ʧ��*/

/* ����ģʽ */
#define AT_CIMI                       21
#define AT_GETINFO                    22


#define GSM_INIT            0           /*��ʼ��*/
#define GSM_AT              1           /*����AT,���ģ��*/
#define GSM_ATE0            2           /*����ATE0,���ģ��*/
#define GSM_CPIN            3           /*����AT+CPIN?,��ѯ(U)SIM ��״̬*/
#define GSM_CIMI            4           /*����AT+CSQ,���ź�*/
#define GSM_CSQ             5           /*����AT+CREG?,ע������*/
#define GSM_CREG            6           /*����AT+CIMI,���SIM��*/
#define GSM_CMGF            7           /*����AT+CMGF,���ö��Ÿ�ʽ*/
#define GSM_CPMS            8           /*����AT+CPMS,���ö��Ŵ洢ģʽ*/
#define GSM_CSCA            9           /*�������ĺ���*/
#define GSM_GPS             10          /*��ȡGPS*/
#define GSM_RESTART         11          /*ATʧ��ģ��ϵ�*/
#define GSM_SUCCESS         12          /*�ɹ�*/
#define GSM_WORK            13          /*����״̬*/
#define GSM_INIT_END        14          /*��ʼ������*/
#define GSM_AT_REBOOT       15          /*ATָ��ģ�鸴λ*/


#define GSM_LENGTH          500     /*����Ϣ����*/
#define GSM_GET_CNT         60      /*AT��ȡ�������*/
#define MAX_MESSAGE_INDEX   60      /*�������洢��Ŀ*/
#define GSM_FLASH_CSQ_CNT   60      /*GSM ÿѭ��10�Σ���һ���ź�*/


#define GMS_INIT_DEVICE      1     /*��ͨ������״̬ - ��ʼ��*/
#define GMS_SEND_DEVICE      2     /*��ͨ������״̬ - ���ݷ���*/
#define GMS_RECV_DEVICE      3     /*��ͨ������״̬ - ���ݽ���*/
#define GMS_CLOSE_DEVICE     4     /*��ͨ������״̬ - �ر�ͨ��*/
#define GMS_CHANGE_DEVICE    5     /*��ͨ������״̬ - ͨ���л�*/
#define GMS_SLEEP_DEVICE     6     /*��ͨ������״̬ - ͨ������*/

typedef enum eFailedType
{
    GSM_FAILED_NULL = 0,
    GSM_FAILED_AT,
    GSM_FAILED_SIM,
    GSM_FAILED_REG,
    GSM_FAILED_CSQ,
}GSM_FAILED_E;

struct PDU_Addr_str         /*PDU��ַ��ʽ*/
{
    uint8 len;                          /*����*/
    uint8 type;                         /*��������*/
    uint8 addr[SMS_PDU_PHONE_LEN];      /*�绰����*/
};

typedef struct tagSMSPDURecvStr        /*���ͷ�PDU��ʽ*/
{
    struct PDU_Addr_str service_center_address;     /*�������ĵ�ַ*/
    uint8 pdu_type;                                 /*Э�����ݵ�Ԫ����*/
    struct PDU_Addr_str originator_address;         /*���ͷ�SME��ַ*/
    uint8 protocol_identifier;                      /*������ʾSMSC�Ժ��ַ�ʽ����SM (����FAX,��Voice��)*/
    uint8 data_coding_scheme;                       /*������ʾ�û�����(UD)���뷽��*/
    uint8 service_time_stamp[7];                    /*�յ���Ϣ��ʱ��*/
    uint16 user_data_len;                           /*�û����ݳ���*/
    uint8  user_data[500];                          /*�û�����*/
}SMS_PDU_RECV_T;

typedef struct tagSMSPDUSendStr        /*���ͷ�PDU��ʽ*/
{
    struct PDU_Addr_str service_center_address;    /*�������ĵ�ַ*/
    uint8 pdu_type;                                /*Э�����ݵ�Ԫ����*/
    uint8 message_reference;                     /*���гɹ���SMS-SUBMIT�ο���Ŀ(0..255)*/
    struct PDU_Addr_str destination_address;     /*���շ�SME�ĵ�ַ*/
    uint8 protocol_identifier;                    /*������ʾSMSC�Ժ��ַ�ʽ����SM (����FAX,��Voice��)*/
    uint8 data_coding_scheme;                    /*������ʾ�û�����(UD)���뷽��*/
    uint8 validity_period[7];                     /*��Ϣ��SMSC�в�����Ч��ʱ��*/
    uint8 user_data_len;                        /*�û����ݳ���*/
    uint8 user_data[200];                        /*�û�����*/
}SMS_PDU_SEND_T;

/* ��ں������� */
typedef uint32 (*StatsProcFun)(uint8, char *, uint32, uint32 *, WIRELESS_HANDLE_T *, uint32 *);
typedef struct tagWorkMsgFun
{
    uint32             printNum;    /* ��־״̬������ӡ����,0-Ĭ�϶���ӡ */
    uint32             status;      /* ״̬�� */
    StatsProcFun       func;
    char               name[24];    /* ״̬���� */
    char               info[48];    /* ״̬����ʾ����, ����Ϊ�� */
    uint32             showNum;     /* ״̬������ˢ�´���, 0-Ĭ�϶�ˢ�� */
    uint32             times;       /* ״̬ˢ��ʱ�� */
}WIRELESS_WORK_FUN_T;

int wireless_grps_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle);
int wireless_grpsnew_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle);
int wireless_gsm_working(uint8 commport, uint8 ifnum, WIRELESS_HANDLE_T *ptHandle);

#ifdef __cplusplus
}
#endif

#endif //__WIRELESS_H__


