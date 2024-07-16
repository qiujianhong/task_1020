/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   �߼�app�òɶ�����Ϣ����
* @date��    2020-1-6
* @history�� 
*********************************************************************
*/

#ifndef __MSG_PROC_H__
#define __MSG_PROC_H__

#include "message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _STATUS_BAR_INFO
{
    uint32            disptime;    //��ʾʱ����ʱ��Ϊ0��ʾһֱ��ʾ
    time_t            dispEnd;    //��ʾ����ʱ��
    char              text[24];    //��ʾ����
}STATUS_BAR_INFO_T;

//������Ϣ��ʾ��
typedef struct _POP_MES_BOX
{    
    BOOL              is_pop;
    uint32            disptime;
    time_t            dispEnd;    //��ʾ����ʱ��
    uint32            delayTime;
    char              text[512];
}POP_MES_BOX_T;

 typedef enum
{
    SIG_STRENGTH,      //�ź�ǿ�ȣ�long_unsigned
    NET_FORMAT,        //������ʽ��unsigned
    CON_STATUS,        //����״̬��bool
}REMOTE_COM_STATE_E;

typedef enum
{
    CHANNEL1,        //ͨ��1��Զ��ͨ��״̬
    CHANNEL2,        //ͨ��2��Զ��ͨ��״̬            
    ETH_CON_STATUS,  //��̫������            bool
    WARN,            //�澯        bool
    INFO_ID,         //��Ϣ��        long-unsigned
    KEY_STATUS,      //��Կ״̬        unsigned
    BLUE_STATUS,	 //����״̬
    RADIO_STATUS,		//��̨״̬
}TOOL_BAR_FIELD_E;

typedef struct
{
    uint16     signalStrengh;    //�ź�ǿ��
    uint8      netFormat;        //������ʽ
    bool       conStatus;        //����״̬
}REMOTE_COM_STATE_T;

typedef struct 
{
    REMOTE_COM_STATE_T    channel1;        //ͨ��1
    REMOTE_COM_STATE_T    channel2;        //ͨ��2
    bool                  ethConStatus;    //��̫������
    bool                  warning;        //�澯
    uint16                infoID;            //��Ϣ���
    uint8                 keyStatus;        //��Կ״̬
    uint8                 blueStatus;       //����״̬
    uint8                 radioStatus;      //��̨״̬
}TOOL_BAR_INFO_T;

extern STATUS_BAR_INFO_T     statusInfoFresh;
extern POP_MES_BOX_T         g_pop_mes_Fresh;
extern TOOL_BAR_INFO_T        toolInfoFresh;

int desktop_msg_handle(MESSAGE_INFO_T *pInMsg);

#ifdef __cplusplus
}
#endif

#endif //__MQTT_MSG_H__

