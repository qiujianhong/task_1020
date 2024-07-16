/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief��   ģ����Ϣ��������
* @date��    2020/2/28
* @history�� 
*********************************************************************
*/

#ifndef __MAPMSG_H__
#define __MAPMSG_H__

#include "dataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

//���䷽��λ��bit7=0��ʾ��֡�����ն˷�����; bit7=1��ʾ��֡����ģ�鷢����
#define MAPMSG_DIR_TERMINAL     0
#define MAPMSG_DIR_MODULE       1

//������־λ��bit6=0��ʾ��֡����ģ�鷢��; bit6=1��ʾ��֡�����ն˷���
#define MAPMSG_PRM_TERMINAL     1
#define MAPMSG_PRM_MODULE       0

//������
typedef enum _MAPMSG_CTR_E
{
    MAPMSG_CTR_OK  = 0,      //ȷ��
    MAPMSG_CTR_NO  = 1,      //����
    MAPMSG_CTR_MSG = 2,      //��Ϣ֡
}MAPMSG_CTR_E;

typedef enum _MAPMSG_APDU_E
{
    MAPMSG_APDU_LinkRequest        = 1,
    MAPMSG_APDU_GetRequest         = 2,
    MAPMSG_APDU_SetRequest         = 3,
    MAPMSG_APDU_ReportNotification = 4,
    
    MAPMSG_APDU_LinkResponse       = 129,
    MAPMSG_APDU_GetResponse        = 130,
    MAPMSG_APDU_SetResponse        = 131,
    MAPMSG_APDU_ReportResponse     = 132,
}MAPMSG_APDU_E;


#define MAPMSG_DT_COMM_MODINFO       0x0000    //ģ����Ϣ
#define MAPMSG_DT_COMM_LINKCON       0x0001    //��·Э����Ϣ
#define MAPMSG_DT_COMM_PROTOCOL      0x0002    //Э��汾
#define MAPMSG_DT_COMM_POWERSTATUS   0x0003    //��Դ״̬
#define MAPMSG_DT_COMM_FILETRAN_STAT 0x0004    //�ļ�����״̬
#define MAPMSG_DT_COMM_TIMER         0x0100    //ʱ��
#define MAPMSG_DT_COMM_HARDINIT      0x0200    //Ӳ����ʼ��
#define MAPMSG_DT_COMM_ARGSINIT      0x0201    //������ʼ��
#define MAPMSG_DT_COMM_DATAINIT      0x0202    //���ݳ�ʼ��
#define MAPMSG_DT_COMM_FILETRAN      0x0203    //�ļ�����

#define MAPMSG_DT_PULSE_STATUS       0x4000    //ң��״̬
#define MAPMSG_DT_PULSE_LASTDATA     0x4001    //��һ�����������ʱ��
#define MAPMSG_DT_PULSE_TOTALDATA    0x4002    //���������ͳ��ʱ��
#define MAPMSG_DT_PULSE_SET          0x4100    //ң�����幦������
#define MAPMSG_DT_PULSE_SETTIME      0x4101    //ң�ŷ���ʱ��
#define MAPMSG_DT_485_CAN_STATUS     0x4F00    //485_CAN״̬

#define MAPMSG_DT_CTR_LOOP_STATUS    0x7000    //���ƻ�·����״̬
#define MAPMSG_DT_CTR_RELAY_TYPE     0x7100    //�̵������ģʽ
#define MAPMSG_DT_CTR_RELAY_WIDTH    0x7101    //�̵������������
#define MAPMSG_DT_CTR_STATUS         0x7201    //�̵����������


#define MAPMSG_DT_ANALOG_GET         0x8000    //ģ��������

#define MAPMSG_DT_LCM_WORKSTATUS       0x9000  //��·Ѳ��ģ�鹤����Ϣ
#define MAPMSG_DT_LCM_EVENT_WORKSTATUS 0x9001  //��·Ѳ��ģ���¼�����ʱ������Ϣ
#define MAPMSG_DT_LCM_VERSIONINFO      0x9003  //��·Ѳ�컥�����汾��Ϣ

/* ������ */
typedef union _CMT_FIELD_U
{
    struct
    {
        uint8 cmd :6; //������
        uint8 prm :1; //������ʶ
        uint8 dir :1; //���䷽��
        uint8 fid;    //֡���
    };
    uint16 wcmd;
}__attribute__ ((packed))CMT_FIELD_U;

#define MESSAGE_RPM_RECV        0
#define MESSAGE_RPM_START       1
#define MESSAGE_ADDR_LEN        64
/* �����ֽڶ���, û���ϸ��ձ��ĸ�ʽ, �յ�����Ϣ���������¸�ʽ */
typedef struct tagMAPMsgInfo
{
    uint16      len;  //����
    CMT_FIELD_U cmt;  //������

    uint8       apdu[0]; //apdu�����Ǵ��
}MAPMSG_INFO_T;


#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H__ */
