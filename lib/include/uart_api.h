/*
*********************************************************************
* Copyright(C) 2021 China Electric Power Research Institute
* All rights reserved. 
* @brief��   ����APP��Ϣ�ӿ�
* @date��    2021/7/30
* @history�� 
*********************************************************************
*/
#ifndef __UART_API_H_
#define __UART_API_H_

#include "message.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    UART_TYPE_485     = 0,
    UART_TYPE_232,
    UART_TYPE_CAN,
    UART_TYPE_UNLL    = 255,
}UART_TYPE_E;

typedef enum
{
    UART_MODE_ONLY     = 0,
    UART_MODE_MASTER,
    UART_MODE_SLAVE,
    UART_MODE_UNLL    = 255,
}UART_MODE_E;

typedef enum
{
    UART_BAUD_300     = 0,
    UART_BAUD_600,
    UART_BAUD_1200,
    UART_BAUD_2400,
    UART_BAUD_4800,
    UART_BAUD_7200,
    UART_BAUD_9600,
    UART_BAUD_19200,
    UART_BAUD_38400,
    UART_BAUD_57600,
    UART_BAUD_115200, //10
    UART_BAUD_230400, //CANʹ��
    UART_BAUD_460800,
    UART_BAUD_10K,
    UART_BAUD_25K,
    UART_BAUD_50K,
    UART_BAUD_125K,
    UART_BAUD_250K,
    UART_BAUD_500K,
    UART_BAUD_1M,
    UART_BAUD_NULL = 255,
}UART_BAUDRATE_E;

typedef enum
{
    UART_PARITY_NONE     = 0,
    UART_PARITY_ODD,       //��У��
    UART_PARITY_EVEN,      //żУ��
    UART_PARITY_SPACE,    
}UART_PARITY_E;

/*����λ*/
typedef enum tag_UART_FLOWCTRL_E  
{
    UART_FLOWCTRL_NONE = 0,
    UART_FLOWCTRL_HARD,
    UART_FLOWCTRL_SOFT,   
}UART_FLOWCTRL_E;

typedef enum
{
    UART_RESULT_OK       = 0,  //�ɹ�
    UART_RESULT_TIME_OUT,      //��ʱ
    UART_RESULT_TASK_FULL,     //������, APP��Ҫ�ط�
    UART_RESULT_PARAM_ERR,     //��������
    UART_RESULT_DATA_ERR,      //���ݴ��� ������Base64�����ʽ
    UART_RESULT_DATA_OTHER = 255,
}UART_RESULT_E;

typedef enum
{
    UART_ACK_OK      = 0,  //�ɹ�
    UART_ACK_OTHER,         //����ʧ������
    UART_ACK_TIMEOUT,       //֡��ʱ
    UART_ACK_PORTERR = 4,   //�˿ڴ���
    UART_ACK_FMTERR,        //��ʽ����    
    UART_ACK_UNREG,         //δע��
    UART_ACK_MODEERR,       //ģʽ����
}UART_ACK_E;

typedef enum
{
    UART_ABNORMAL_ERROR_ANSWER     = 0,
    UART_ABNORMAL_UNKNOW_CODE,
}UART_ABNORMAL_E;

typedef struct tagUartPort
{
    UART_TYPE_E        portType;    //�˿�����
    uint32             portNo;      //�˿����(�������)
}UART_PORT_T;

typedef struct tagMsgUartParam
{
    uint8             baudRate;   //UART_BAUDRATE_E
    uint8             dataBits;   //����λ: 5��6��7��8
    uint8             parity;     //У��λ���� UART_PARITY_E
    uint8             stopBits;   //ֹͣλ������1��2
    uint8             flowCtrl;   //������ UART_FLOWCTRL_E
}UART_MSG_PARAM_T;

/* ģʽ���֪ͨ */
typedef struct tagUartModeInfo
{
    UART_PORT_T        port;       //�˿�
    UART_MODE_E        mode;       //ģʽ
    char               lastApp[64];//��һ��ռ�ö˿ڵ�APP����
    char               curApp[64]; //Ŀǰռ�ö˿ڵ�APP����
}UART_MODEINFO_T;

#define UART_MONITOR_MAX   20
/* ����ģʽ״̬ */
typedef struct tagUartModeStatus
{
    UART_PORT_T        port;       //�˿�       
    UART_MODE_E        mode;       //ģʽ
    char               onlyApp[64];//��ռAPP����
    UART_MSG_PARAM_T   param;      //����
    uint32             num;        //APP�б�����
    char               monitorApp[UART_MONITOR_MAX][64]; //���ݼ���APP�б�
}UART_MODESTAUS_T;

/* ��ռģʽAPPע��-���ݼ���APPע�� */
typedef struct tagUartAPPReg
{
    UART_PORT_T        port;       //�˿�       
    char               onlyApp[64];//��ռAPP����
    UART_MSG_PARAM_T   param;      //���� ���ݼ��� ��
}UART_APPREG_T;

#define UART_DATA_MAXLEN   4600
/* ���ڷ�����Ϣ�ṹ�� */
typedef struct tagUartSendData
{
    UART_PORT_T        port;         //�˿�
    uint32             prio;         //���ȼ� 0-��, 1-��
    uint32             prm;          //����֡��־��0-Ӧ��֡��1-����֡
    uint16             frametimeout; //֡��ʱʱ�� ,��λ���루ms��
    uint16             bytetimeout;  //�ַ���ʱʱ�� ,��λ���루ms��
    uint16             tasktimeout;  //�����������ȴ�ʱ�䣨ms��
    uint32             bParam;       //�Ƿ�Я������ 0-��Я��(��ռģʽ��Я��), 1-Я��
    UART_MSG_PARAM_T   param;        //����
    uint16             estsize;      //Ԥ���������ֽ�
    uint16             datalen;      //���ݳ���
    uint8              data[UART_DATA_MAXLEN]; //����
}UART_SEND_DATA_T;

/* ���ڽ�����Ϣ�ṹ�� */
typedef struct tagUartRecvData
{
    UART_PORT_T        port;         //�˿�
    uint8              prm;          //������־ 1-��������
    uint8              status;       //��� UART_RESULT_E
    uint16             datalen;      //���ݳ���
    uint8              data[UART_DATA_MAXLEN]; //����
}UART_RECV_DATA_T;

/* ����ģʽ���� */
typedef struct tagUartMode
{
    UART_PORT_T        port;       //�˿�       
    char               AppName[64];//APP����
    UART_MODE_E        mode;       //ģʽ
}UART_MODE_T;

/* �����쳣 */
typedef struct tagUartAbnormal
{
    UART_PORT_T        port;       //�˿�       
    char               AppName[64];//APP����
    UART_ABNORMAL_E    cause;      //�쳣ԭ��
}UART_ABNORMAL_T;

/* ����ģʽ״̬ */
typedef struct tagUartModeSet
{
    UART_PORT_T        port;       //�˿�       
    UART_MODE_E        mode;       //ģʽ
    char               onlyApp[64];//��ռAPP����
    uint32             bParam;     //�Ƿ�Я������ 0-��Я��(��ռģʽ��Я��), 1-Я��
    UART_MSG_PARAM_T   param;      //����
}UART_MODESET_T;

#define UART_BAUD_TO_STRING(baudint, baudE)\
do\
{\
    if(UART_BAUD_300 == baudE)\
    {\
        baudint = 300;\
    }\
    else if(UART_BAUD_600 == baudE)\
    {\
        baudint = 600;\
    }\
    else if(UART_BAUD_1200 == baudE)\
    {\
        baudint = 1200;\
    }\
    else if(UART_BAUD_2400 == baudE)\
    {\
        baudint = 2400;\
    }\
    else if(UART_BAUD_4800 == baudE)\
    {\
        baudint = 4800;\
    }\
    else if(UART_BAUD_7200 == baudE)\
    {\
        baudint = 7200;\
    }\
    else if(UART_BAUD_9600 == baudE)\
    {\
        baudint = 9600;\
    }\
    else if(UART_BAUD_19200 == baudE)\
    {\
        baudint = 19200;\
    }\
    else if(UART_BAUD_38400 == baudE)\
    {\
        baudint = 38400;\
    }\
    else if(UART_BAUD_38400 == baudE)\
    {\
        baudint = 38400;\
    }\
    else if(UART_BAUD_57600 == baudE)\
    {\
        baudint = 57600;\
    }\
    else if(UART_BAUD_115200 == baudE)\
    {\
        baudint = 115200;\
    }\
    else if(UART_BAUD_230400 == baudE)\
    {\
        baudint = 230400;\
    }\
    else if(UART_BAUD_460800 == baudE)\
    {\
        baudint = 460800;\
    }\
    else if(UART_BAUD_10K == baudE)\
    {\
        baudint = 10000;\
    }\
    else if(UART_BAUD_25K == baudE)\
    {\
        baudint = 25000;\
    }\
    else if(UART_BAUD_50K == baudE)\
    {\
        baudint = 50000;\
    }\
    else if(UART_BAUD_125K == baudE)\
    {\
        baudint = 125000;\
    }\
    else if(UART_BAUD_250K == baudE)\
    {\
        baudint = 250000;\
    }\
        else if(UART_BAUD_500K == baudE)\
    {\
        baudint = 500000;\
    }\
    else if(UART_BAUD_1M == baudE)\
    {\
        baudint = 1000000;\
    }\
    else\
    {\
        baudint = 2400;\
    }\
}while(0)

#define UART_STRING_TO_BAUD(baudint, baudE)\
do\
{\
    if(300 == baudint)\
    {\
        baudE = UART_BAUD_300;\
    }\
    else if(600 == baudint)\
    {\
        baudE = UART_BAUD_600;\
    }\
    else if(1200 == baudint)\
    {\
        baudE = UART_BAUD_1200;\
    }\
    else if(2400 == baudint)\
    {\
        baudE = UART_BAUD_2400;\
    }\
    else if(4800 == baudint)\
    {\
        baudE = UART_BAUD_4800;\
    }\
    else if(7200 == baudint)\
    {\
        baudE = UART_BAUD_7200;\
    }\
    else if(9600 == baudint)\
    {\
        baudE = UART_BAUD_9600;\
    }\
    else if(19200 == baudint)\
    {\
        baudE = UART_BAUD_19200;\
    }\
    else if(38400 == baudint)\
    {\
        baudE = UART_BAUD_38400;\
    }\
    else if(38400 == baudint)\
    {\
        baudE = UART_BAUD_38400;\
    }\
    else if(57600 == baudint)\
    {\
        baudE = UART_BAUD_57600;\
    }\
    else if(115200 == baudint)\
    {\
        baudE = UART_BAUD_115200;\
    }\
    else if(230400 == baudint)\
    {\
        baudE = UART_BAUD_230400;\
    }\
    else if(460800 == baudint)\
    {\
        baudE = UART_BAUD_460800;\
    }\
    else if(10000 == baudint)\
    {\
        baudE = UART_BAUD_10K;\
    }\
    else if(25000 == baudint)\
    {\
        baudE = UART_BAUD_25K;\
    }\
    else if(50000 == baudint)\
    {\
        baudE = UART_BAUD_50K;\
    }\
    else if(125000 == baudint)\
    {\
        baudE = UART_BAUD_125K;\
    }\
    else if(250000 == baudint)\
    {\
        baudE = UART_BAUD_250K;\
    }\
    else if(500000 == baudint)\
    {\
        baudE = UART_BAUD_500K;\
    }\
    else if(1000000 == baudint)\
    {\
        baudE = UART_BAUD_1M;\
    }\
    else\
    {\
        baudE = UART_BAUD_NULL;\
    }\
}while(0)

#define UART_OAD_TO_GPORT(portoad, porttype, portnum) \
do\
{\
    if((portoad&0xFFFFFF00) == 0xF2010200)\
    {\
        porttype = UART_TYPE_485;\
        portnum = portoad&0x000000FF;\
    }\
    else if((portoad&0xFFFFFF00) == 0xF2000200)\
    {\
        porttype = UART_TYPE_232;\
        portnum = portoad&0x000000FF;\
    }\
    else if((portoad&0xFFFFFF00) == 0xF2210200)\
    {\
        porttype = UART_TYPE_CAN;\
        portnum = portoad&0x000000FF;\
    }\
}while(0)

#define UART_GPORT_TO_OAD(portoad, porttype, portnum) \
do\
{\
    if(porttype == UART_TYPE_485)\
    {\
        portoad = 0xF2010200 + (portnum);\
    }\
    else if(porttype == UART_TYPE_232)\
    {\
        portoad = 0xF2000200 + (portnum);\
    }\
    else if(porttype == UART_TYPE_CAN)\
    {\
        portoad = 0xF2210200 + (portnum);\
    }\
}while(0)

/* �ַ���ת�˿� */
#define UART_STRING_TO_PORT(portstring, porttype, portnum) \
do\
{\
    if(0 == memcmp(portstring, "RS485-", strlen("RS485-")))\
    {\
        porttype = UART_TYPE_485;\
        portnum = *((char*)portstring + strlen("RS485-")) - '0';\
    }\
    else if(0 == memcmp(portstring, "RS232-", strlen("RS232-")))\
    {\
        porttype = UART_TYPE_232;\
        portnum = *((char*)portstring + strlen("RS232-")) - '0';\
    }\
    else if(0 == memcmp(portstring, "CAN-", strlen("CAN-")))\
    {\
        porttype = UART_TYPE_CAN;\
        portnum = *((char*)portstring + strlen("CAN-")) - '0';\
    }\
    else\
    {\
        porttype = UART_TYPE_UNLL;\
    }\
}while(0)

/*
*********************************************************************
* @brief     : ģʽ���֪ͨ����
* @param[in] : MESSAGE_JSON_T *pMsg      ������ϢJSON��ʽ
* @param[out]: UART_ONLY_T *pMsgOnly     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_notify_mode_split(MESSAGE_JSON_T *pMsg, UART_MODEINFO_T *pMsgOnly);

/*
*********************************************************************
* @brief     : ģʽ���֪ͨ��װ
* @param[in] : uint32 token              ��Ϣ���
               UART_ONLY_T *pMsgOnly     ���ڶ�ռģʽ֪ͨ
               int topiclen              ���⻺������
* @param[out]: char *topic               ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_notify_mode_package(uint32 token, UART_MODEINFO_T *pMsgOnly, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ������Ϣ��ѯ������Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg      ������ϢJSON��ʽ
* @param[out]: UART_PORT_T *pRSPort      �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_get_info_req_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort);

/*
*********************************************************************
* @brief     : ������Ϣ��ѯ������Ϣ��װ
* @param[in] : uint32 token              ��Ϣ���
               char *sourapp             ����APP����
               UART_PORT_T *pRSPort      �˿�
               int topiclen              ���⻺������
* @param[out]: char *topic               ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_get_info_req_package(uint32 token, char *sourapp, UART_PORT_T *pRSPort, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ������Ϣ��ѯ��Ӧ��Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg          ������ϢJSON��ʽ
* @param[out]: UART_MODESTAUS_T *pModeStatus �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_get_info_res_split(MESSAGE_JSON_T *pMsg, UART_MODESTAUS_T *pModeStatus);

/*
*********************************************************************
* @brief     : ������Ϣ��ѯ��Ӧ��Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *destapp                  Ŀ��APP
               UART_MODESTAUS_T *pModeStatus  ģʽ��״̬
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_get_info_res_package(uint32 token, char *destapp, UART_MODESTAUS_T *pModeStatus, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ���ڶ�ռģʽAPPע��������Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg       ������ϢJSON��ʽ
* @param[out]: UART_APPREG_T *pAppReg     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_set_param_req_split(MESSAGE_JSON_T *pMsg, UART_APPREG_T *pAppReg);

/*
*********************************************************************
* @brief     : ���ڶ�ռģʽAPPע��������Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *sourapp                  ����APP
               UART_APPREG_T *pAppReg         ע��APP
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_set_param_req_package(uint32 token, char *sourapp, UART_APPREG_T *pAppReg, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ���ڶ�ռģʽAPPע����Ӧ��Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg       ������ϢJSON��ʽ
* @param[out]: UART_PORT_T *pRSPort       �������
               uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_set_param_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : ���ڶ�ռģʽAPPע����Ӧ��Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *destapp                  Ŀ��APP
               UART_PORT_T *pRSPort           �˿�
               char *status                   ���
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_set_param_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ���ݼ���ע��������Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg       ������ϢJSON��ʽ
* @param[out]: UART_APPREG_T *pAppReg     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_set_monitor_req_split(MESSAGE_JSON_T *pMsg, UART_APPREG_T *pAppReg);

/*
*********************************************************************
* @brief     : ���ݼ���ע������������Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *sourapp                  ����APP
               UART_APPREG_T *pAppReg         ע��APP
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_set_monitor_req_package(uint32 token, char *sourapp, UART_APPREG_T *pAppReg, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ���ݼ���ע��������Ӧ��Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg       ������ϢJSON��ʽ
* @param[out]: UART_PORT_T *pRSPort       �������
               uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_set_monitor_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : ���ݼ���ע��������Ӧ��Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *destapp                  Ŀ��APP
               UART_PORT_T *pRSPort           �˿�
               char *status                   ���
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_set_monitor_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ���ݷ�����Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg           ������ϢJSON��ʽ
* @param[out]: UART_SEND_DATA_T *pSendMsg     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_data_send_split(MESSAGE_JSON_T *pMsg, UART_SEND_DATA_T *pSendMsg);

/*
*********************************************************************
* @brief     : ���ݷ�����Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *sourapp                  ����APP
               UART_SEND_DATA_T *pSendMsg     ��������
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_data_send_package(uint32 token, char *sourapp, UART_SEND_DATA_T *pSendMsg, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ���ݽ�����Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg           ������ϢJSON��ʽ
* @param[out]: UART_RECV_DATA_T *pRecvMsg     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_data_recv_split(MESSAGE_JSON_T *pMsg, UART_RECV_DATA_T *pRecvMsg);

/*
*********************************************************************
* @brief     : ���ݽ�����Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *destapp                  Ŀ��APP
               UART_RECV_DATA_T *pRecvMsg     ������Ϣ
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_data_recv_package(uint32 token, char *destapp, UART_RECV_DATA_T *pRecvMsg, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ����ģʽ����������Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg           ������ϢJSON��ʽ
* @param[out]: UART_MODE_T *pMode             �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_set_mode_req_split(MESSAGE_JSON_T *pMsg, UART_MODE_T *pMode);

/*
*********************************************************************
* @brief     : ����ģʽ����������Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *sourapp                  ����APP
               UART_SEND_DATA_T *pSendMsg     ��������
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_set_mode_req_package(uint32 token, char *sourapp, UART_MODE_T *pMode, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ����ģʽ������Ӧ��Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg       ������ϢJSON��ʽ
* @param[out]: UART_PORT_T *pRSPort       �������
               uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_set_mode_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : ����ģʽ������Ӧ��Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *destapp                  Ŀ��APP
               UART_PORT_T *pRSPort           �˿�
               char *status                   ���
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_set_mode_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : �����쳣����������Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg           ������ϢJSON��ʽ
* @param[out]: UART_ABNORMAL_T *pAbnormal     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_set_abnormal_req_split(MESSAGE_JSON_T *pMsg, UART_ABNORMAL_T *pAbnormal);

/*
*********************************************************************
* @brief     : �����쳣����������Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *sourapp                  ����APP
               UART_ABNORMAL_T *pAbnormal     �쳣��Ϣ
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_set_abnormal_req_package(uint32 token, char *sourapp, UART_ABNORMAL_T *pAbnormal, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : �����쳣������Ӧ��Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsg       ������ϢJSON��ʽ
* @param[out]: UART_PORT_T *pRSPort       �������
               uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_json_set_abnormal_res_split(MESSAGE_JSON_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : �����쳣������Ӧ��Ϣ��װ
* @param[in] : uint32 token                   ��Ϣ���
               char *destapp                  Ŀ��APP
               UART_PORT_T *pRSPort           �˿�
               char *status                   ���
               int topiclen                   ���⻺������
* @param[out]: char *topic                    ����
* @return    : JSON��Ϣ�ַ���
*********************************************************************
*/
char *uart_json_set_abnormal_res_package(uint32 token, char *destapp, UART_PORT_T *pRSPort, char *status, char *topic, int topiclen);

/*
*********************************************************************
* @brief     : ģʽ���֪ͨ����
* @param[in] : MESSAGE_INFO_T *pMsg      ������ϢAXDR��ʽ
* @param[out]: UART_ONLY_T *pMsgOnly     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_notify_mode_split(MESSAGE_INFO_T *pMsg, UART_MODEINFO_T *pMsgOnly);

/*
*********************************************************************
* @brief     : ģʽ���֪ͨ��װ
* @param[in] : UART_ONLY_T *pMsgOnly     ���ڶ�ռģʽ֪ͨ
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_notify_mode_package(UART_MODEINFO_T *pMsgData, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ������Ϣ��ѯ������Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg      ������ϢAXDR��ʽ
* @param[out]: UART_PORT_T *pRSPort      �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_get_info_req_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pRSPort);

/*
*********************************************************************
* @brief     : ������Ϣ��ѯ������Ϣ��װ
* @param[in] : UART_PORT_T *pRSPort      �˿�
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_get_info_req_package(UART_PORT_T *pRSPort, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ������Ϣ��ѯ��Ӧ��Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg          ������ϢAXDR��ʽ
* @param[out]: UART_MODESTAUS_T *pModeStatus �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_get_info_res_split(MESSAGE_INFO_T *pMsg, UART_MODESTAUS_T *pModeStatus);

/*
*********************************************************************
* @brief     : ������Ϣ��ѯ��Ӧ��Ϣ��װ
* @param[in] : UART_MODESTAUS_T *pModeStatus  ģʽ��״̬
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_get_info_res_package(UART_MODESTAUS_T *pModeStatus, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ���ڶ�ռģʽAPPע��������Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg       ������ϢAXDR��ʽ
* @param[out]: UART_APPREG_T *pAppReg     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_set_param_req_split(MESSAGE_INFO_T *pMsg, UART_APPREG_T *pAppReg);

/*
*********************************************************************
* @brief     : ���ڶ�ռģʽAPPע��������Ϣ��װ
* @param[in] : UART_APPREG_T *pAppReg         ע��APP
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_set_param_req_package(UART_APPREG_T *pAppReg, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ���ڶ�ռģʽAPPע����Ӧ��Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg       ������ϢAXDR��ʽ
* @param[out]: uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_set_param_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK);

/*
*********************************************************************
* @brief     : ���ڶ�ռģʽAPPע����Ӧ��Ϣ��װ
* @param[in] : UART_PORT_T *pRSPort           �˿�
               BOOLEN status                  ���
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_set_param_res_package(UART_PORT_T *pRSPort, uint8 status, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ���ݼ���ע��������Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg       ������ϢAXDR��ʽ
* @param[out]: UART_APPREG_T *pAppReg     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_set_monitor_req_split(MESSAGE_INFO_T *pMsg, UART_APPREG_T *pAppReg);

/*
*********************************************************************
* @brief     : ���ݼ���ע������������Ϣ��װ
* @param[in] : UART_APPREG_T *pAppReg         ע��APP
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_set_monitor_req_package(UART_APPREG_T *pAppReg, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ���ݼ���ע��������Ӧ��Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg       ������ϢAXDR��ʽ
* @param[out]: uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_set_monitor_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK);

/*
*********************************************************************
* @brief     : ���ݼ���ע��������Ӧ��Ϣ��װ
* @param[in] : UART_PORT_T *pRSPort           �˿�
               BOOLEN status                  ���
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_set_monitor_res_package(UART_PORT_T *pRSPort, uint8 status, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ���ݷ�����Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg           ������ϢAXDR��ʽ
* @param[out]: UART_SEND_DATA_T *pSendMsg     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_data_send_req_split(MESSAGE_INFO_T *pMsg, UART_SEND_DATA_T *pSendMsg);

/*
*********************************************************************
* @brief     : ���ݷ�����Ϣ��װ
* @param[in] : UART_SEND_DATA_T *pSendMsg     ��������
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_data_send_req_package(UART_SEND_DATA_T *pSendMsg, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : �����쳣������Ӧ��Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg       ������ϢAXDR��ʽ
* @param[out]: UART_PORT_T *pRSPort       �������
               uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_data_send_res_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : �����쳣������Ӧ��Ϣ��װ
* @param[in] : UART_PORT_T *pRSPort           �˿�
               BOOLEN status                  ���
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_data_send_res_package(UART_PORT_T *pRSPort, uint8 status, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ���ݽ�����Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg           ������ϢAXDR��ʽ
* @param[out]: UART_RECV_DATA_T *pRecvMsg     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_data_recv_split(MESSAGE_INFO_T *pMsg, UART_RECV_DATA_T *pRecvMsg);

/*
*********************************************************************
* @brief     : ���ݽ�����Ϣ��װ
* @param[in] : UART_RECV_DATA_T *pRecvMsg     ������Ϣ
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_data_recv_package(UART_RECV_DATA_T *pRecvMsg, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : �����쳣����������Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg           ������ϢAXDR��ʽ
* @param[out]: UART_ABNORMAL_T *pAbnormal     �������
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_set_abnormal_req_split(MESSAGE_INFO_T *pMsg, UART_ABNORMAL_T *cause);

/*
*********************************************************************
* @brief     : �����쳣����������Ϣ��װ
               UART_ABNORMAL_T *pAbnormal     �쳣��Ϣ
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_set_abnormal_req_package(UART_ABNORMAL_T *pMsgData, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : �����쳣������Ӧ��Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg       ������ϢAXDR��ʽ
* @param[out]: UART_PORT_T *pRSPort       �������
               uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
*********************************************************************
*/
int uart_axdr_set_abnormal_res_split(MESSAGE_INFO_T *pMsg, UART_PORT_T *pRSPort, uint8 *bOK);

/*
*********************************************************************
* @brief     : �����쳣������Ӧ��Ϣ��װ
* @param[in] : UART_PORT_T *pRSPort           �˿�
               BOOLEN status                  ���
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
*********************************************************************
*/
uint16 uart_axdr_set_abnormal_res_package(UART_PORT_T *pRSPort, BOOLEAN status, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ����ģʽ����������Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg       ������ϢAXDR��ʽ
* @param[out]: uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
* @Create    : ����
* @Date      ��2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_mode_req_split(MESSAGE_INFO_T *pMsg, UART_MODESET_T *pModeSet);

/*
*********************************************************************
* @brief     : ����ģʽ����������Ϣ��װ
* @param[in] : UART_PORT_T *pRSPort           �˿�
               BOOLEN status                  ���
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
* @Create    : ����
* @Date      ��2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_mode_req_package(UART_MODESET_T *pModeSet, uint8 *payload, uint16 maxlen);

/*
*********************************************************************
* @brief     : ����ģʽ���û�����Ϣ����
* @param[in] : MESSAGE_INFO_T *pMsg       ������ϢAXDR��ʽ
* @param[out]: uint8 *bOK                 �Ƿ񷵻�OK TRUE-OK/FALSE-ʧ��
* @return    : 0-�ɹ�/����-������
* @Create    : ����
* @Date      ��2021-8-10
* @Update    :
*********************************************************************
*/
int uart_axdr_set_mode_res_split(MESSAGE_INFO_T *pMsg, uint8 *bOK);

/*
*********************************************************************
* @brief     : ����ģʽ���û�����Ϣ��װ
* @param[in] : UART_PORT_T *pRSPort           �˿�
               BOOLEN status                  ���
               uint16 maxlen         �����С
* @param[out]��uint8 *payload         ��������             
* @return    : payload����
* @Create    : ����
* @Date      ��2021-8-10
* @Update    :
*********************************************************************
*/
uint16 uart_axdr_set_mode_res_package(UART_PORT_T *pMsgData, uint8 status, uint8 *payload, uint16 maxlen);

int cco_json_data_recv_split(MESSAGE_JSON_T *pMsg, UART_RECV_DATA_T *pRecvMsg);


#ifdef __cplusplus
}
#endif

#endif /* __UART_API_H_ */

