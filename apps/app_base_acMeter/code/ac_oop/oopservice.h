/*
*********************************************************************
* @file    apduservice.c
* @brief�� apduӦ�÷�����
* @author������
* @date��   2019.12.06
* @copyright 2019 �Ͼ��������ӹɷ����޹�˾ All rights reserved.
*********************************************************************
*/
#ifndef __OOP_SERVICE_H__
#define __OOP_SERVICE_H__


#include "baseCommon.h"

/** @brief �ӿ�������   */
#define OOP_IC_MAX  26

//�����ĵ�OAD����ƫ��
#define OOP_DATA_OFFSET  3
#define OOP_DATA_OFFSET_NEXT  8

//LIST�����ĵ�OAD����ƫ��
#define OOP_DATA_LIST_OFFSET  4

//LIST�����ݵ����ڵ��� �� SEQUENCE OF�ĸ�������Ϊ1�ֽ�����
#define OOP_DATA_LIST_MAX  255

//�ն������֡����
#define LOCAL_RECV_FRAM_MAXLEN             4224

//UDP�շ���󳤶�
#define MAX_UDP_BUFF_LEN                   4224                      


/** @brief ��������ö��   */
typedef enum
{
    REQ_GET_NORMAL             = 1,
    REQ_GET_NORMAL_LIST        = 2,        
    REQ_GET_RECORD             = 3,   
    REQ_GET_RECORD_LIST        = 4,   
    REQ_GET_NEXT               = 5,   
    REQ_GET_MD5                = 6,   

    REQ_SET_NORMAL             = 1,
    REQ_SET_NORMAL_LIST        = 2,        
    REQ_STG_NORMAL_LIST        = 3,    

    REQ_ACT_NORMAL             = 1,
    REQ_ACT_NORMAL_LIST        = 2,        
    REQ_ATG_NORMAL_LIST        = 3,
    
}PRTL_REQ_TYPE_E;

/** @brief ��Ӧ����ö��   */
typedef enum
{
    RES_RPT_NORMAL_LIST        = 1,  //�ϱ����ɸ�����������Ӧ
    RES_RPT_RECORD_LIST        = 2,  //�ϱ����ɸ���¼�Ͷ������Ե���Ӧ        
    RES_RPT_TRANS_DATA         = 3,  //�ϱ�͸�����ݵ���Ӧ
}PRTL_RES_TYPE_E;

typedef struct{
    uint8           appSlicingFlag;        //Ӧ�÷�֡��ʶ
    uint8           isEnd;                 //�Ƿ�ĩβ֡
    OOP_OAD_U       serviceOAD;            //��¼�����ݵ���oad(��60120300)
    uint8           seviceapdu[3];         //��ȡ�ͼ�¼�ķ�������
    uint16          seq;                   //Ӧ�÷�֡���
    uint8           objPre;                //��֡��Ӧ��������
    uint8           reqType;               //��Ӧ����apdu[1]
    uint16          metsn;                 //60120300��¼�Ĳ�������Ϣ
    OOP_OCTETSTR6_T MAC;                   //����У�飬��ʼ�ͱ��ַ
    DateTimeBcd_t   dateTimeTag;           //60120300��¼�Ķ����ݵ�ʱ��
    uint8           theLastNTimes;         //60120300��¼��ʣ��δ������n��
    uint8           ms_cnt;                //60120300������¼��ʱ�����Ϣ
    uint32          tmStartSeg;            //�����������ݵ�ʱ������㣬�����ж��Ƿ�ȡ�����м�¼
    uint32          recNumSeg;             //��¼�����ϵ㣨����9 10��
    uint32          index;                 //�����ͨ����ʹ��,�óɹ�ʱ��ɸѡʱ�����µ��ĸ����ݵ�
}__attribute__ ((packed)) BP_INFO_T;

/** @brief ���ݽ����Ϣ */
typedef struct 
{
    uint32  resultDataLen;      // ���ݽ������
    uint8   pResultData[LOCAL_RECV_FRAM_MAXLEN];       // ���ݽ��������ָ��
}RESULT_DATA_T;



#endif /*__OOP_SERVICE_H__*/

