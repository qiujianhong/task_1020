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
#ifndef	__UART_COMM_H
#define __UART_COMM_H
    
#ifdef __cplusplus
        extern "C" {
#endif

#define  APP_RECV_BUF_LEN      8192	/*recvfrom���ջ�������С*/

#define UART_APP_NAME     "uartManager"
#define UART_INITFILE     UART_APP_NAME".ini"
#define UART_DATAFILE     UART_APP_NAME".json"

#define APP_SUBTOPIC1      "+/M-uartManager/#"
#define APP_SUBTOPIC2      "+/uartManager/#"
#define APP_SUBTOPIC3      "M-mapManager/Broadcast/#"

#define APP_DEFAULT_TOPIC "M-uartManager/M-smiOS"

#define UART_DB_CLIENT_NUM   4

#define UART_MAP_ID_MIN    1
#define UART_MAP_ID_MAX    5

#define UART_APPID_MAX     32

/* ���ڷ���APP�������� */
typedef struct tag_UART_APPDATA_T
{
    uint32      mqttip;       //mqtt��ַ
    uint16      mqttport;     //mqtt�˿�
    uint8       appidlen;
    uint8       appid[UART_APPID_MAX];
    char       *appName;      //APP����
    int         dbAgent;
    time_t      msgtime;  //��Ϣ�յ���ʱ��
}UART_APPDATA_T;

#ifdef __cplusplus
}
#endif

#endif


