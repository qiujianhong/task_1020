/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/

#ifndef _AC_698MSG_H
#define _AC_698MSG_H

//״̬��
#define ACMSG_INIT        0         // ����״̬ - ��ʼ��
#define ACMSG_RECV        1         // ����״̬ - ���ݽ���
#define ACMSG_SEND        2         // ����״̬ - I���ݷ���
#define ACMSG_CLOSE       3         // ����״̬ - �ر�����

typedef struct tagCONNECT_REQUEST_T
{
    uint16 version;      //Э��汾��
    uint8  prtlConf[8];  //Э��һ����
    uint8  funcConf[16]; //����һ����
    uint16 clientSFML;   //�ͻ�������֡����
    uint16 clientRFML;   //�ͻ�������֡����
    uint8  clientWS;     //�ͻ������մ��ڴ�С
    uint16 clientPFML;   //�ͻ������ɴ���֡�ߴ�
    uint32 timeOut;      //Ӧ�����ӳ�ʱʱ��
    uint8  *pMechains;   //Ӧ������������֤�Ļ�����Ϣ
}CONNECT_REQUEST_T;


typedef enum AppConnectResult
{
    CONNECT_PERMISSION = 0, //������Ӧ������
    CONNECT_ERR_PASSWORD = 1, //�������
    CONNECT_ERR_SYMMETRY = 2, //�Գƽ��ܴ���
    CONNECT_ERR_NSYMMETRY = 3, //�ǶԳƽ��ܴ���
    CONNECT_ERR_SIGNATURE = 4, //����ǩ������
    CONNECT_ERR_VERSION = 5, //Э��汾��ƥ��
    CONNECT_ERR_OTHER = 255, //��������
}CONNECT_RESULT_E;

typedef struct {
    uint16 version;                //Э��汾��
    uint8  prtlConf[8];            //Э��һ����
    uint8  funcConf[16];           //����һ����
    uint16 serverSFML;             //����������֡���ߴ�
    uint16 serverRFML;             //����������֡����
    uint8  serverWS;               //���������մ��ڴ�С
    uint16 serverPFML;             //���������ɴ���֡�ߴ�
    uint32 timeOut;                //Ӧ�����ӳ�ʱʱ��
    CONNECT_RESULT_E  connectRst;  //Ӧ�������������֤��Ӧ��Ϣ
}CONNECT_RESPONSE_T;

extern OOP_DAR_E app698msg_time_set(void);
extern int ac_get_msg_package(OOP_OAD_U oad, uint8* sendbuf, uint32 bufLenMax, uint8 logic);
extern int ac_act_msg_package(OOP_OMD_U omd, uint8* inData, uint32 inLen, uint8* sendbuf, uint32 bufLenMax, uint8 logic);
extern int ac_connect_package(CONNECT_REQUEST_T* pResponse, uint8* sendbuf, uint32 bufLenMax);
extern int ac_act_frame_factory_reset(uint8* sendbuf, uint32 maxLen);
extern int ac_act_frame_power_off(uint8* sendbuf, uint32 maxLen);
extern int ac_act_parser_factory_reset(uint8* recvBuf, uint32 recvLen);
extern int ac_rpt_msg_package(uint8 *addr, uint8 isRec, uint8 piid, OOP_OAD_U oad, uint8 *pSend, uint32 lenMax);

#endif