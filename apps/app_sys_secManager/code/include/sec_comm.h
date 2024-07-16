/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·��ع�������
* @date��    2020-6-30
* @history�� 
* @User deatil:

*********************************************************************
*/
#ifndef	__SEC_COMM_H
#define __SEC_COMM_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define  APP_RECV_BUF_LEN      8192	/*recvfrom���ջ�������С*/

#define SEC_APP_NAME     "scsMonitor"
#define SEC_INITFILE     SEC_APP_NAME".ini"
#define SEC_DATAFILE     SEC_APP_NAME".json"

#define APP_SUBTOPIC1      "+/M-scsMonitor/#"
#define APP_SUBTOPIC2      "+/scsMonitor/#"

#define APP_DEFAULT_TOPIC "M-scsMonitor/M-smiOS"

#define SEC_DB_CLIENT_NUM   4

#define SEC_MAP_ID_MIN    1
#define SEC_MAP_ID_MAX    5

#define SEC_APPID_MAX     32

#define SEC_RECORD_MAX    32

#define APP_NAME_MAX     (64)

/* ���ڷ���APP�������� */
typedef struct tag_SEC_APPDATA_T
{
    uint32      mqttip;       //mqtt��ַ
    uint16      mqttport;     //mqtt�˿�
    uint8       appidlen;
    uint8       appid[SEC_APPID_MAX];
    char       *appName;      //APP����
    int         dbAgent;
    time_t      msgtime;  //��Ϣ�յ���ʱ��
}SEC_APPDATA_T;

#ifdef __cplusplus
}
#endif

#endif


