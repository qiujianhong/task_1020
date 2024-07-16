/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#ifndef _AC_SAMPLE698_H_
#define _AC_SAMPLE698_H_

#include "ac_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

#define BIG_DATA_MOD     1			//���������ģʽ������4096������

#if BIG_DATA_MOD == 1
#define SEND_BUF_LEN    4224
#define SEND_APDU_LEN   4096
#define RECV_BUF_LEN    4224
#define RECV_APDU_LEN   4096
#else
#define SEND_BUF_LEN    1600
#define SEND_APDU_LEN   1600
#define RECV_BUF_LEN    1600
#define RECV_APDU_LEN   1600
#endif

#define COMWAITTIME     3			//���崮�ڵȴ����ݴ���

//�����ֵԼ��������ֵ
#define ACSAMP_ADD_VALUE_CLEAR(count, mcount, value) ((count) + (value) >= (mcount) ? 0 : (count) + (value))

//���ݽ�������
typedef int (*OOP_PARSER_FUN)(OOP_OAD_U oad, uint8 *pInMsg, uint32 inLen, void *destData, void *destDataBak);

//oad�ٷ���
typedef enum tagSAMPLE_TYPE
{
    SAMPLE_NULL     = 0,
    SAMPLE_ENERGY   = 1,
    SAMPLE_HENERGY  = 2,
    SAMPLE_DEMAND   = 3,
    SAMPLE_DEMAND1  = 4,
    SAMPLE_REALTIME = 5,
    SAMPLE_HARMONIC = 6,
    SAMPLE_OTHER    = 7,
    SAMPLE_PARAM_INIT = 8,
    SAMPLE_POWER_OFF  = 9,
    SAMPLE_ERR,
}SAMPLE_TYPE_E;

typedef struct tagOAD_INFO_MAP
{
    SAMPLE_TYPE_E    type;
    OOP_OAD_U        oad;
    OOP_CLASS_E      class;        //���ڵ���
    OOP_DATATYPE_E   dataType;     //��������
    uint8            dataLen;      //oad��Ӧ�ĳ��� ����������
    uint8            subLen;       //�����Ӧ�ĳ���
    DATA_E           structType;   //Ҫת�ɵĽṹ������
    int8             scalerOOP;    //0AD��Ӧ�Ļ��㵥λ
    void*            destData;     //Ŀ������
    void*            destDataBak;  //����Ŀ������
    OOP_PARSER_FUN   pOopParserFun;//��������
}OAD_INFO_MAP_T;

typedef struct tagOAD_INFO_LIST
{
    SAMPLE_TYPE_E  type;
    OAD_INFO_MAP_T *pOadInfoMap;
    uint32         *pOadInfoNum;
}OAD_INFO_LIST_T;

typedef struct tagUART_CONF_T
{
    BOOL   enable;
    char   dev[256];
    uint32 BaudRate;
    uint8  ByteSize;
    char   Parity;
    uint8  StopBits;
}UART_CONF_T;

//����Ԥ����
typedef struct tagAC_PRE_PROC
{
    uint8           apduType;           //��������
    OOP_OAD_U       oad;
    uint32          sendLen;
    uint8           sendBuf[SEND_BUF_LEN];
    uint32          delay;              //�ӳ�ִ��
    void*           destData;           //Ŀ������
    void*           destDataBak;        //����Ŀ������
    OOP_PARSER_FUN  pOopParserFun;      //��������
}AC_PRE_PROC_T;

extern BOOL gIsSupport4K;
extern UART_CONF_T gUartConf;

extern void ac_calc_deal_698(uint32 *cntchk);
extern uint8 *get_68ptr_from_buf(uint8 *buf, uint32 maxLen, uint32 *offset, uint32 *outLen);
extern int ac_sample_dev_open(void);
extern int ac_sample_dev_close(int* fd_spi);
extern uint32 make_apdu_to_package(uint8 *apdu, uint32 apduLen, uint8 *outMsg, uint32 maxLen, uint8 logic);
extern int ac_read_sample_parser(SAMPLE_TYPE_E type, uint8* recvBuf, uint32 recvLen);
extern int ac_get_connect(int32 fd_spi, BOOL* pIsSupport4K);

#ifdef __cplusplus
}
#endif

#endif // _AC_SAMPLE698_H_