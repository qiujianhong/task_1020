/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2019-10-25
*********************************************************************
*/


#ifndef __AC_METER_REDEF_H__
#define __AC_METER_REDEF_H__

#include "ac_all.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

// �߾��ȵ����ɼ���洢����
#define  HD_ENERGY_EN   1

// ������Ϣ���غ�
#define  DEBUG_ACAPP_EN   1
#define  DEBUG_ACCOMM_EN  1

#ifdef  DEBUG_ACAPP_EN
#include <errno.h>
extern int errno;
#define ACPRINT(x...) {if (getenv("DEBUG_AC") != NULL)  printf(x);}
#else
#define ACPRINT(x...)
#endif


#if DEBUG_ACCOMM_EN == 1
#define ACCOMM(x...) {if (getenv("DEBUG_ACCOMM") != NULL)  printf(x);}
#else
#define ACCOMM(x...)
#endif


#define TEST_VER_FLAG    (1)     //�Ƿ���ʱ���԰汾


/*******************************************************************************
* �궨��
*******************************************************************************/

#define MAX_ALLOWED_DOWN_NUM    8               //485ģ��������������߳�����
#define RUN_DOWN_NUM            1               //485ģ������ʵ�������߳�����

#define MAX_QUEUE_CNT		                 40                      //��������
#define MAX_QUEUE_BUFF_LEN                 4224                      //���С����л���������

#define MAX_MODULE_NUM     5       //ģ����������
#define MAX_CHANNEL_NUM     5      //ÿ��ģ�������5��ͨ�� 0Ĭ�Ϲ���ͨ��
#define MAX_MODULE_DELAY_TIME  6   //��ѯģ����Ϣ���ʱʱ��
#define AC485_MODULE_DEVNAME  "uart_acm"

#define AC_NORMAL_DATA    1               //������ͨ��������
#define AC_PARA_DATA      2               //������ͨ��������
#define AC_ACTION_DATA    3               //���ɲ�����������
#define AC_RECORD_DATA    4               //���ɼ�¼����������


/*******************************************************************************
* ö�ٺͽṹ�嶨��
*******************************************************************************/

//����
typedef struct
{
    int32  QueueLen;					        //���г���
    uint8   QueueBuf[MAX_QUEUE_BUFF_LEN];	    //���д洢������
}Queue_T;

//�����������ṹ
typedef struct
{
    pthread_mutex_t QueueLock;					//���ջ�����������
    int16  QueueHead;						    //����ͷ
    int16  QueueTail;							//����β
    Queue_T Queue[MAX_QUEUE_CNT];	            //ͨѶ���ݶ���
}LockQueue_T;

//ģ����Ϣ�е�ͨ����Ϣ
typedef struct
{
    uint8 ChannelType;//�ӿ�����	enum{CDC-ACM(1), CDC-ECM(2)},
    uint8 ChannelFunc;//��������	enum{����ͨ��(0), Զ��ͨ��(1)��PLC/RFͨ��(2)��RS-485ͨ��(3)��ң������(4)��CANͨ��(5)��M-Busͨ��(6)������(7)��ģ�����ɼ�(8)��RS232ͨ��(9)}
}CHANNEL_INFO_T;  //ģ����Ϣ�е�ͨ����Ϣ

//ģ����Ϣ
typedef struct
{
    pthread_rwlock_t   ModuleLock;  //��дģ����Ϣ���ö�д��
    int32 ModuleID;//ģ�����(��1��ʼ)
    uint8 IsValid;//�Ƿ���Ч 0��Ч 1��Ч
    uint8 DeviceType[32];//�豸�ͺ�
    uint8 DeviceID[32];//�豸id
    uint32 SoftVersion;//����汾
    uint8 SoftDate[10];//�����������
    uint32 HardVersion;//Ӳ���汾
    uint8 HardDate[10];//Ӳ����������
    uint8 ManufacturerCode[8];//���̱���
    uint8 ChannelNum;//ͨ������
    CHANNEL_INFO_T ChannelInfo[MAX_CHANNEL_NUM];//ͨ������
}MODULE_INFO_T;//ģ����Ϣ

//ң��ģ��HAL�ӿڲ���
typedef struct
{
    char  deviceName[30];  /*�豸�� */
    uint8 isvalid;
    uint8 moduleIndex;  /*ģ���� ��1��ʼ */
    UART_DEVICE_T* dev;
}DEV_HAL_INFO;



/*******************************************************************************
* ��������
*******************************************************************************/
#define MAX_TARIFF_NUM            4          // ��������:���63�� �⡢�塢ƽ����
#define PhaMaxTariffNum           0          // ��������������� 0:�޷ַ��ʣ�����

#define MAX_QUEUE_BUFF_LEN                 4224                      //���С����л���������
#define MAX_MSG_PAYLOAD_LEN                2040                      //��ϢMSG��payload��󳤶�

#define MAX_HARMONIC_NUM          21         // ��ѹг�������ʣ��ܼ�2��n�Σ� n��С

#define MAX_DB_R_W_LEN            1000       // �������ĵ��ζ�д��󳤶�

/*******************************************************************************
* ��ʾ������ز����ṹ����
*******************************************************************************/
#define AC_PROOF_LOG  "/mnt/log/acproof.log"
#define AC_APP_LOG    "/mnt/log/acapp.log"


/*******************************************************************************
* ���ݽṹ����
*******************************************************************************/

typedef enum
{
    ACT_COMM_IDLE = 0,                          //����
    ACT_COMM_SEND,                              //����
    ACT_COMM_RECV,                              //����
    ACT_COMM_RETRY,                             //����
    ACT_COMM_DELAY,                             //��ʱ
}ACTIVE_COMM_STATE_T;                           //����ͨѶ״̬��

typedef struct{
    OOP_OAD_U  Oad;          //OAD
    int8   DesScaler;        //0AD��Ӧ�Ļ��㵥λ
    uint32  DI07;	         //07Э�����ݱ�ʶ
    uint8   DataLen;	     //645 ÿ����Ԫ�صĳ���
    uint8   TotalDataLen;	 //645 �ܳ���
    uint16  DI97;
    DATA_E  CovertSruct; //Ҫת�ɵĽṹ������
    int8   Scaler;           //645С����λ��
}Map645ToOop_NEW;


/* �����ֽڶ���, û���ϸ��ձ��ĸ�ʽ, �յ�����Ϣ���������¸�ʽ */
typedef struct tagInfoData
{
    uint32 IP;        //Ŀ�Ķ�ip
    uint16 port;
    uint8  res[2];
    
    uint8  MsgRPM;         /* ������־λ, 1-����, 0-������ */
    uint8  MsgPRIORITY;    /* ���ȼ� */
    uint16 MsgIndex;       /* ��Ϣ��� */
    uint16 MsgLabel;       /* ��Ϣ��ǩ */

    uint8  SourLen;
    uint8  DestLen;
    char   SourAddr[128];
    char   DestAddr[128];

    uint16 IID_ID;
    uint16 IOP_ID;

    uint32 nPayloadLength; 
    uint8  aPayloadData[MAX_MSG_PAYLOAD_LEN];
}INFO_DATA_T;


/* �������ݱ仯���� */
#define DEAD_ZONE_ENG            (1)          // ���� -2
#define DEAD_ZONE_ENG_HD         (1)          // ����(�߾���) -4
#define DEAD_ZONE_DMD            (2)          // ���� 0
#define DEAD_ZONE_VOLT           (2)          // ��ѹ -1
#define DEAD_ZONE_CURR           (2)          // ���� -3
#define DEAD_ZONE_ANGLE          (2)          // ��� -1
#define DEAD_ZONE_POWER          (2)          // ���� -1
#define DEAD_ZONE_PF             (2)          // �������� -3
#define DEAD_ZONE_FR             (2)          // Ƶ�� -2
#define DEAD_ZONE_RATE           (2)          // ��ƽ����
#define DEAD_ZONE_HARM           (1)          // г��


/* ���������Ƿ�Խ���仯������־ */
typedef struct tagUpdateFlag
{
    /* ���� */
    BOOL bUpdateCmbActEng;             //(��ǰ)����й����ܿ�
    
    BOOL bUpdatePosActEng;             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaAPosActEng;             //(A��)�����й��ܵ���ʾֵ
    BOOL bUpdatePhaBPosActEng;             //(B��)�����й��ܵ���ʾֵ
    BOOL bUpdatePhaCPosActEng;             //(C��)�����й��ܵ���ʾֵ
    
    BOOL bUpdateNegActEng;             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaANegActEng;             //(A��)�����й��ܵ���ʾֵ
    BOOL bUpdatePhaBNegActEng;             //(B��)�����й��ܵ���ʾֵ
    BOOL bUpdatePhaCNegActEng;             //(C��)�����й��ܵ���ʾֵ
    
    BOOL bUpdatePosReaEng;             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaAPosReaEng;             //(A��)�����޹��ܵ���ʾֵ
    BOOL bUpdatePhaBPosReaEng;             //(B��)�����޹��ܵ���ʾֵ
    BOOL bUpdatePhaCPosReaEng;             //(C��)�����޹��ܵ���ʾֵ
    
    BOOL bUpdateNegReaEng;             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaANegReaEng;             //(A��)�����޹��ܵ���ʾֵ
    BOOL bUpdatePhaBNegReaEng;             //(B��)�����޹��ܵ���ʾֵ
    BOOL bUpdatePhaCNegReaEng;             //(C��)�����޹��ܵ���ʾֵ
    
    BOOL bUpdateQuad1ReaEng;             //(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
    BOOL bUpdatePhaA1ReaEng;             //(A��)��һ�����޹�����
    BOOL bUpdatePhaB1ReaEng;             //(B��)��һ�����޹�����
    BOOL bUpdatePhaC1ReaEng;             //(C��)��һ�����޹�����
    
    BOOL bUpdateQuad2ReaEng;             //(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
    BOOL bUpdatePhaA2ReaEng;             //(A��)�ڶ������޹�����
    BOOL bUpdatePhaB2ReaEng;             //(B��)�ڶ������޹�����
    BOOL bUpdatePhaC2ReaEng;             //(C��)�ڶ������޹�����
    
    BOOL bUpdateQuad3ReaEng;             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    BOOL bUpdatePhaA3ReaEng;             //(A��)���������޹�����
    BOOL bUpdatePhaB3ReaEng;             //(B��)���������޹�����
    BOOL bUpdatePhaC3ReaEng;             //(C��)���������޹�����
    
    BOOL bUpdateQuad4ReaEng;             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    BOOL bUpdatePhaA4ReaEng;             //(A��)���������޹�����
    BOOL bUpdatePhaB4ReaEng;             //(B��)���������޹�����
    BOOL bUpdatePhaC4ReaEng;             //(C��)���������޹�����
    
    BOOL bUpdatePosAppEng;             //(��ǰ)�������ڵ������ݿ�
    BOOL bUpdatePhaAActAppEng;             //(A��)�������ڵ���
    BOOL bUpdatePhaBActAppEng;             //(B��)�������ڵ���
    BOOL bUpdatePhaCActAppEng;             //(C��)�������ڵ���
    
    BOOL bUpdateNegAppEng;             //(��ǰ)�������ڵ������ݿ�
    BOOL bUpdatePhaANegAppEng;             //(A��)�������ڵ���
    BOOL bUpdatePhaBNegAppEng;             //(B��)�������ڵ���
    BOOL bUpdatePhaCNegAppEng;             //(C��)�������ڵ���

    /* �߾��� ���� */
    BOOL bUpdateCmbActEngHD;             //(��ǰ)����й����ܿ�
    
    BOOL bUpdatePosActEngHD;             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaAPosActEngHD;             //(A��)�����й��ܵ���ʾֵ
    BOOL bUpdatePhaBPosActEngHD;             //(B��)�����й��ܵ���ʾֵ
    BOOL bUpdatePhaCPosActEngHD;             //(C��)�����й��ܵ���ʾֵ
    
    BOOL bUpdateNegActEngHD;             //(��ǰ)�����й������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaANegActEngHD;             //(A��)�����й��ܵ���ʾֵ
    BOOL bUpdatePhaBNegActEngHD;             //(B��)�����й��ܵ���ʾֵ
    BOOL bUpdatePhaCNegActEngHD;             //(C��)�����й��ܵ���ʾֵ
    
    BOOL bUpdatePosReaEngHD;             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaAPosReaEngHD;             //(A��)�����޹��ܵ���ʾֵ
    BOOL bUpdatePhaBPosReaEngHD;             //(B��)�����޹��ܵ���ʾֵ
    BOOL bUpdatePhaCPosReaEngHD;             //(C��)�����޹��ܵ���ʾֵ
    
    BOOL bUpdateNegReaEngHD;             //(��ǰ)�����޹������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaANegReaEngHD;             //(A��)�����޹��ܵ���ʾֵ
    BOOL bUpdatePhaBNegReaEngHD;             //(B��)�����޹��ܵ���ʾֵ
    BOOL bUpdatePhaCNegReaEngHD;             //(C��)�����޹��ܵ���ʾֵ
    
    BOOL bUpdateQuad1ReaEngHD;             //(��ǰ)��һ�����޹��ܼ��ƽ�ȵ���ʾֵ
    BOOL bUpdatePhaA1ReaEngHD;             //(A��)��һ�����޹�����
    BOOL bUpdatePhaB1ReaEngHD;             //(B��)��һ�����޹�����
    BOOL bUpdatePhaC1ReaEngHD;             //(C��)��һ�����޹�����
    
    BOOL bUpdateQuad2ReaEngHD;             //(��ǰ)�ڶ������޹��ܼ��ƽ�ȵ���ʾֵ
    BOOL bUpdatePhaA2ReaEngHD;             //(A��)�ڶ������޹�����
    BOOL bUpdatePhaB2ReaEngHD;             //(B��)�ڶ������޹�����
    BOOL bUpdatePhaC2ReaEngHD;             //(C��)�ڶ������޹�����
    
    BOOL bUpdateQuad3ReaEngHD;             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    BOOL bUpdatePhaA3ReaEngHD;             //(A��)���������޹�����
    BOOL bUpdatePhaB3ReaEngHD;             //(B��)���������޹�����
    BOOL bUpdatePhaC3ReaEngHD;             //(C��)���������޹�����
    
    BOOL bUpdateQuad4ReaEngHD;             //(��ǰ)���������޹��ܼ��ƽ�ȵ���ʾֵ
    BOOL bUpdatePhaA4ReaEngHD;             //(A��)���������޹�����
    BOOL bUpdatePhaB4ReaEngHD;             //(B��)���������޹�����
    BOOL bUpdatePhaC4ReaEngHD;             //(C��)���������޹�����
    
    BOOL bUpdatePosAppEngHD;             //(��ǰ)�������ڵ������ݿ�
    BOOL bUpdatePhaAActAppEngHD;             //(A��)�������ڵ���
    BOOL bUpdatePhaBActAppEngHD;             //(B��)�������ڵ���
    BOOL bUpdatePhaCActAppEngHD;             //(C��)�������ڵ���
    
    BOOL bUpdateNegAppEngHD;             //(��ǰ)�������ڵ������ݿ�
    BOOL bUpdatePhaANegAppEngHD;             //(A��)�������ڵ���
    BOOL bUpdatePhaBNegAppEngHD;             //(B��)�������ڵ���
    BOOL bUpdatePhaCNegAppEngHD;             //(C��)�������ڵ���

    /* ���� */
    BOOL bUpdatePosActDmd;             //(��ǰ)�����й���������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaAPosActDmd;             //(A��)�����й����������ʾֵ
    BOOL bUpdatePhaBPosActDmd;             //(B��)�����й����������ʾֵ
    BOOL bUpdatePhaCPosActDmd;             //(C��)�����й����������ʾֵ
    
    BOOL bUpdateNegActDmd;             //(��ǰ)�����й���������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaANegActDmd;             //(A��)�����й����������ʾֵ
    BOOL bUpdatePhaBNegActDmd;             //(B��)�����й����������ʾֵ
    BOOL bUpdatePhaCNegActDmd;             //(C��)�����й����������ʾֵ
    
    BOOL bUpdatePosReaDmd;             //(��ǰ)�����޹���������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaAPosReaDmd;             //(A��)�����޹����������ʾֵ
    BOOL bUpdatePhaBPosReaDmd;             //(B��)�����޹����������ʾֵ
    BOOL bUpdatePhaCPosReaDmd;             //(C��)�����޹����������ʾֵ
    
    BOOL bUpdateNegReaDmd;             //(��ǰ)�����޹���������ܼ��ƽ��ʾֵ
    BOOL bUpdatePhaANegReaDmd;             //(A��)�����޹����������ʾֵ
    BOOL bUpdatePhaBNegReaDmd;             //(B��)�����޹����������ʾֵ
    BOOL bUpdatePhaCNegReaDmd;             //(C��)�����޹����������ʾֵ
    
    BOOL bUpdateQuad1ReaDmd;             //(��ǰ)��һ�����޹��ܼ��ƽ���������ʾֵ
    BOOL bUpdatePhaA1ReaDmd;             //(A��)��һ�����޹��������
    BOOL bUpdatePhaB1ReaDmd;             //(B��)��һ�����޹��������
    BOOL bUpdatePhaC1ReaDmd;             //(C��)��һ�����޹��������
    
    BOOL bUpdateQuad2ReaDmd;             //(��ǰ)�ڶ������޹��ܼ��ƽ���������ʾֵ
    BOOL bUpdatePhaA2ReaDmd;             //(A��)�ڶ������޹��������
    BOOL bUpdatePhaB2ReaDmd;             //(B��)�ڶ������޹��������
    BOOL bUpdatePhaC2ReaDmd;             //(C��)�ڶ������޹��������
    
    BOOL bUpdateQuad3ReaDmd;             //(��ǰ)���������޹��ܼ��ƽ���������ʾֵ
    BOOL bUpdatePhaA3ReaDmd;             //(A��)���������޹��������
    BOOL bUpdatePhaB3ReaDmd;             //(B��)���������޹��������
    BOOL bUpdatePhaC3ReaDmd;             //(C��)���������޹��������
    
    BOOL bUpdateQuad4ReaDmd;             //(��ǰ)���������޹��ܼ��ƽ���������ʾֵ
    BOOL bUpdatePhaA4ReaDmd;             //(A��)���������޹��������
    BOOL bUpdatePhaB4ReaDmd;             //(B��)���������޹��������
    BOOL bUpdatePhaC4ReaDmd;             //(C��)���������޹��������
    
    BOOL bUpdatePosAppDmd;             //(��ǰ)������������������ݿ�
    BOOL bUpdatePhaAActAppDmd;             //(A��)���������������
    BOOL bUpdatePhaBActAppDmd;             //(B��)���������������
    BOOL bUpdatePhaCActAppDmd;             //(C��)���������������
    
    BOOL bUpdateNegAppDmd;             //(��ǰ)������������������ݿ�
    BOOL bUpdatePhaANegAppDmd;             //(A��)���������������
    BOOL bUpdatePhaBNegAppDmd;             //(B��)���������������
    BOOL bUpdatePhaCNegAppDmd;             //(C��)���������������

    /* ʵʱ���� */
    BOOL bUpdateVolt; 			 //(��ǰ)�����ѹ
    BOOL bUpdateVoltHD; 			 //(��ǰ)�߾��������ѹ
    BOOL bUpdateZeroVolt; 			 //(��ǰ)�����ѹ
    BOOL bUpdateCurr; 			 //(��ǰ)�������
    BOOL bUpdateZeroCurr;              //�������
    BOOL bUpdateVoltAngle;              //��ѹ���
    BOOL bUpdateCurrAngle;              //��ѹ���
    BOOL bUpdateVoltCurrAngle;              //��ѹ������� 
    BOOL bUpdateActPower;              //(��ǰ)˲ʱ�����й�����
    BOOL bUpdateReaPower; 		     //(��ǰ)˲ʱ�����޹�����
    BOOL bUpdateAppPower;              //(��ǰ)˲ʱ�������ڹ���
    BOOL bUpdatePowerFactor;              //(��ǰ)˲ʱ�ܼ����๦������
    BOOL bUpdateLineFrz;              //����Ƶ��
    BOOL bUpdateMetStusPara;             //�������״̬��1-7
    BOOL bUpdateVoltUnRate;              //��ѹ��ƽ����
    BOOL bUpdateCurrUnRate;              //������ƽ����

    /* г�� */
    BOOL bUpdateAVoltHarmonic;           	//A���ѹг�������ʣ��ܼ�2��n�Σ�
    BOOL bUpdateBVoltHarmonic;           	//B���ѹг�������ʣ��ܼ�2��n�Σ�
    BOOL bUpdateCVoltHarmonic;           	//C���ѹг�������ʣ��ܼ�2��n�Σ�
    BOOL bUpdateACurrHarmonic;           	//A�����г�������ʣ��ܼ�2��n�Σ�
    BOOL bUpdateBCurrHarmonic;           	//B�����г�������ʣ��ܼ�2��n�Σ�
    BOOL bUpdateCCurrHarmonic;           	//C�����г�������ʣ��ܼ�2��n�Σ�

    BOOL bUpdateAVoltHarmonicValue;           	//A���ѹг���������ܼ�2��n�Σ�
    BOOL bUpdateBVoltHarmonicValue;           	//B���ѹг���������ܼ�2��n�Σ�
    BOOL bUpdateCVoltHarmonicValue;           	//C���ѹг���������ܼ�2��n�Σ�
    BOOL bUpdateACurrHarmonicValue;           	//A�����г���������ܼ�2��n�Σ�
    BOOL bUpdateBCurrHarmonicValue;           	//B�����г���������ܼ�2��n�Σ�
    BOOL bUpdateCCurrHarmonicValue;           	//C�����г���������ܼ�2��n�Σ�
}UPDATE_FLAG_T;






typedef union 
{   // С��ģʽ�µ�˫�ֽ�������
    uint8   Byte2[2];
    int16  Sinteger;
    uint16  Uinteger;
}__attribute__ ((packed)) BYTE2_INT16_TYPE;

typedef union 
{   // С��ģʽ�µ�4�ֽ�������
    uint8  Byte4[4];
    int    SLinteger;
    uint32 ULinteger; 
}__attribute__ ((packed)) BYTE4_INT32_TYPE;

typedef union 
{   // С��ģʽ�µ�8�ֽ�������
    uint8  Byte4[8];
    int64  SLinteger;
    uint64 ULinteger;
}__attribute__ ((packed)) BYTE8_INT64_TYPE;

typedef struct         
{
    uint8 ss;   // ��
    uint8 nn;   // ��
    uint8 hh;   // ʱ
    uint8 ww;   // ����
    uint8 dd;   // ��
    uint8 mm;   // ��
    uint8 yy;   // ��
} __attribute__ ((packed)) REAL_TIME_TYPE;

/*******************************************************************************
* ʱ����ؽṹ��
*******************************************************************************/
typedef struct
{    
    uint8    ss;
    uint8    nn;
    uint8    hh;
    uint8    WW;
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDDWWhhnnss_TYPE;     // YYMMDDWWhhnnss�ṹ������

typedef struct
{    
    uint8    ss;
    uint8    nn;
    uint8    hh;
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDDhhnnss_TYPE;       // YYMMDDhhnnss�ṹ������

typedef struct
{    
    uint8    nn;
    uint8    hh;
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDDhhnn_TYPE;         // YYMMDDhhnn�ṹ������

typedef struct
{    
    uint8    DD;
    uint8    MM;
    uint8    YY;
}__attribute__ ((packed))YYMMDD_TYPE;             // YYMMDD�ṹ������

typedef struct
{    
    uint8    nn;
    uint8    hh;
    uint8    DD;
    uint8    MM;
}__attribute__ ((packed))MMDDhhnn_TYPE;           // MMDDhhnn�ṹ������

typedef struct
{    
    uint8    DD;
    uint8    MM;
}__attribute__ ((packed))MMDD_TYPE;               // MMDD�ṹ������

typedef struct
{    
    uint8    nn;
    uint8    hh;
    uint8    DD;
}__attribute__ ((packed))DDhhnn_TYPE;             // DDhhnn�ṹ������

typedef struct
{
    uint8    hh;
    uint8    DD;
}__attribute__ ((packed))DDhh_TYPE;               // DDhh����

typedef struct
{    
    uint8    ss;
    uint8    nn;
    uint8    hh;
}__attribute__ ((packed))hhnnss_TYPE;             // hhnnss�ṹ������

typedef struct
{    
    uint8    nn;
    uint8    hh;
}__attribute__ ((packed))hhnn_TYPE;               // hhnn�ṹ������

/*******************************************************************************
* ˽�����ݽṹ��
*******************************************************************************/
typedef struct
{
    YYMMDDWWhhnnss_TYPE          LastCompTime;    // �ϴαȽ�ʱ��

}PRIVATE_DATA_TYPE;


/*******************************************************************************
* ʵʱ���ݽṹ��
*******************************************************************************/

typedef struct
{
    YYMMDDWWhhnnss_TYPE          CurTime;         // ��ǰʱ��
    YYMMDDhhnnss_TYPE            BuildTime;       // ����ʱ��(����WW)

}COMMON_DATA_TYPE;



/*******************************************************************************
* ״̬�֡������֡�ģʽ�֡�������Ϣ��
*******************************************************************************/
typedef struct
{    
    uint16 ReservedBit1          :1;              // ����
    uint16 DemandModeFlag        :1;              // �������㷽ʽ(0���1����)
    uint16 IntBatVoltFlag        :1;              // ʱ�ӵ��(0������1Ƿѹ)
    uint16 ExtBatVoltFlag        :1;              // ͣ�糭����(0������1Ƿѹ)
    uint16 ActPowNegFlag         :1;              // �й����ʷ���(0����1����)
    uint16 ReaPowNegFlag         :1;              // �޹����ʷ���(0����1����)
    uint16 ReservedBit7          :1;              // �ų������쳣(��չ)
    uint16 ReservedBit8          :1;              // ����
}__attribute__ ((packed)) METER_STATE1_BIT_TYPE;  // �������״̬��1

typedef union
{
    METER_STATE1_BIT_TYPE    BitSect;
    uint16                    Word;
}__attribute__ ((packed)) METER_STATE1_TYPE;

typedef struct
{    
    uint16 PhaAActPowNegFlag    :1;               // A���й����ʷ���    
    uint16 PhaBActPowNegFlag    :1;               // B���й����ʷ���    
    uint16 PhaCActPowNegFlag    :1;               // C���й����ʷ���
    uint16 ReservedBit4         :1;               // ����    
    uint16 PhaAReaPowNegFlag    :1;               // A���޹����ʷ���    
    uint16 PhaBReaPowNegFlag    :1;               // B���޹����ʷ���    
    uint16 PhaCReaPowNegFlag    :1;               // C���޹����ʷ���    
    uint16 ReservedBit8         :1;               // ����    
}__attribute__ ((packed)) METER_STATE2_BIT_TYPE;  // �������״̬��2 ע��0��������1������

typedef union
{
    METER_STATE2_BIT_TYPE    BitSect;
    uint16                   Word;
}__attribute__ ((packed)) METER_STATE2_TYPE;      // �������״̬��2 

typedef struct
{    
    uint16 CurPeriodFlag          :1;             // ��ǰ����ʱ��(0��һ�ף�1�ڶ���)
    uint16 SupEletModeFlag        :2;             // ���緽ʽ(00����Դ��01������Դ��10��ع���)
    uint16 ProgAllowFlag          :1;             // �������(0��ֹ��1���)
    uint16 RelayStatusFlag        :1;             // �̵���״̬(0ͨ��1��)
    uint16 CurTimZoneFlag         :1;             // ��ǰ����ʱ��(0��һ�ף�1�ڶ���)
    uint16 RelayOrderFlag         :1;             // �̵�������״̬(0ͨ��1��)
    uint16 PreJumpAlarm           :1;             // Ԥ��բ����״̬(0�ޣ�1��)
    uint16 MetModeFlag            :2;             // ���ܱ�����(00��Ԥ���ѱ�01������Ԥ���ѱ�10�����Ԥ���ѱ�
    uint16 CurTarifMark           :1;             // ��ǰ���з�ʱ���� 0:��һ�� 1:�ڶ���
    uint16 CurStepMark            :1;             // ��ǰ���� 0��һ�ף�1�ڶ���
}__attribute__ ((packed)) METER_STATE3_BIT_TYPE;  // �������״̬��3�������ࣩע���������һ��ָ��̰���״̬

typedef union
{
    METER_STATE3_BIT_TYPE    BitSect;
    uint16                   Word;
}__attribute__ ((packed)) METER_STATE3_TYPE;      // �������״̬��3�������ࣩ

typedef struct
{    
    uint16 LossVoltFlag         :1;               // ʧѹ
    uint16 LackVoltFlag         :1;               // Ƿѹ
    uint16 OverVoltFlag         :1;               // ��ѹ
    uint16 LossCurrFlag         :1;               // ʧ��
    uint16 OverCurrFlag         :1;               // ����
    uint16 OverLoadFlag         :1;               // ����    
    uint16 CurrRevsFlag         :1;               // ����    
    uint16 BreakVoltFlag        :1;               // ����
}__attribute__ ((packed)) METER_STATE456_BIT_TYPE;// �������״̬��456��ABC�����״̬��ע��0�����޴�����ϣ�1����ǰ�����������

typedef union
{
    METER_STATE456_BIT_TYPE   BitSect;
    uint16                    Word;    
}__attribute__ ((packed)) METER_STATE456_TYPE;    // �������״̬��456��ABC�����״̬��
    
typedef struct
{    
    uint16 VoltConvFlag         :1;               // ��ѹ������
    uint16 CurrConvFlag         :1;               // ����������
    uint16 VoltUnblFlag         :1;               // ��ѹ��ƽ��
    uint16 CurrUnblFlag         :1;               // ������ƽ��
    uint16 AsPowerLossFlag      :1;               // ������Դ��ʧ
    uint16 PowerLossFlag        :1;               // ��Դ��ʧ
    uint16 OverDemandFlag       :1;               // ��������
    uint16 LackPfFlag           :1;               // �ܹ�������������
    uint16 CurrUnblHighFlag     :1;               // �������ز�ƽ��
}__attribute__ ((packed)) METER_STATE7_BIT_TYPE;  // �������״̬��7���������״̬��ע��0�����޴�����ϣ�1����ǰ����������ϡ�

typedef union
{
    METER_STATE7_BIT_TYPE    BitSect;
    uint16                   Word;
}__attribute__ ((packed)) METER_STATE7_TYPE;      // �������״̬��7���������״̬��

typedef struct
{
    METER_STATE1_TYPE    Rsw1;                    // �������״̬��1
    METER_STATE2_TYPE    Rsw2;                    // �������״̬��2
    METER_STATE3_TYPE    Rsw3;                    // �������״̬��3
    METER_STATE456_TYPE  Rsw4;                    // �������״̬��4
    METER_STATE456_TYPE  Rsw5;                    // �������״̬��5
    METER_STATE456_TYPE  Rsw6;                    // �������״̬��6
    METER_STATE7_TYPE    Rsw7;                    // �������״̬��7
}__attribute__ ((packed)) METER_STATE_WORD_TYPE;  // �������״̬�ּ�������

/*******************************************************************************
* ����������ṹ��
*******************************************************************************/
typedef struct
{    
    BYTE4_INT32_TYPE     Energy;
}__attribute__ ((packed)) SNGL_ENERGY_TYPE;       // �����й��������ݽṹ������;

typedef struct
{
    SNGL_ENERGY_TYPE     Postive;
    SNGL_ENERGY_TYPE     Negative;
}__attribute__ ((packed)) PHASE_ACT_ENERGY_TYPE;  // �����й��������ݽṹ������

typedef struct
{    
    SNGL_ENERGY_TYPE     Postive;                
    SNGL_ENERGY_TYPE     Negative;
    SNGL_ENERGY_TYPE     Quadrant1;
    SNGL_ENERGY_TYPE     Quadrant2;
    SNGL_ENERGY_TYPE     Quadrant3;
    SNGL_ENERGY_TYPE     Quadrant4;
}__attribute__ ((packed)) PHASE_REA_ENERGY_TYPE;  // �����޹��������ݽṹ������

typedef struct
{    
    SNGL_ENERGY_TYPE    Postive;
    SNGL_ENERGY_TYPE     Negative;
}__attribute__ ((packed)) PHASE_APP_ENERGY_TYPE;  // ���������������ݽṹ������

typedef struct
{    
    PHASE_ACT_ENERGY_TYPE Active;                // �й�
    PHASE_REA_ENERGY_TYPE Reactive;              // �޹�
    PHASE_APP_ENERGY_TYPE Apparent;              // ����
}__attribute__ ((packed)) PHASE_ACT_REA_APP_ENERGY_TYPE;    // �������޹��������ݽṹ��







typedef struct
{    
    // ����ֵ xx.xxxx
    uint8 DemNum[3];
}__attribute__ ((packed)) DEM_NUM_TYPE;

typedef struct 
{    // ����ʱ������
    uint8 DemTime[6];
}__attribute__ ((packed))TERM_DEM_TIME_TYPE;

typedef struct
{    
    // �¼��ۼƴ����ṹ��
    uint8 Num[2];    
} __attribute__ ((packed)) NUMBER_TYPE;

typedef struct
{   // �¼��ۼ�ʱ��ṹ��
    uint8 Time[3];    
} __attribute__ ((packed)) TIME_TYPE;

typedef struct
{    
    // �¼���ֹʱ��ṹ��
    uint8 Time[4];    // �¡��ա�ʱ����    
} __attribute__ ((packed)) COM_TIME_TYPE;

#define  SIGNAL_INFO_LEN  (16)  //������ͨ��������

typedef struct{
    uint8 nFlag;                 //1����, 0δ����
    OOP_DATETIME_S_T stIdenTime; //ʶ��ɹ�ʱ��
    uint8 nPhase;                //�ź�������λ, 0:δ֪, 1:A��, 2:B��, 3:C��, 4:����
    int32 nSignalS1;             //�ź�ǿ��S1
    int32 nSignalS2;             //�ź�ǿ��S2
    int32 nNoise;                //��������
    uint8 nInfoLen;              //ʶ���źų���
    uint8 aInfo[SIGNAL_INFO_LEN];//ʶ���ź�����
}TOPO_IDEN_RESULT;//(�Զ�����չ)����ʶ����(�㽭)

/*******************************************************************************
* ����ģ�鹲���ڴ�
********************************************************************************/
typedef struct 
{   // ������ͳ��ģ�鹲���ڴ�ṹ������
    BYTE4_INT32_TYPE PosActEng[MAX_TARIFF_NUM+1];         // �����й�-��,��,��,ƽ,��
    BYTE4_INT32_TYPE NegActEng[MAX_TARIFF_NUM+1];         // �����й�-��,��,��,ƽ,��
    BYTE4_INT32_TYPE PosReaEng[MAX_TARIFF_NUM+1];         // �����޹�-��,��,��,ƽ,��
    BYTE4_INT32_TYPE NegReaEng[MAX_TARIFF_NUM+1];         // �����޹�-��,��,��,ƽ,��
    BYTE4_INT32_TYPE Quad1ReaEng[MAX_TARIFF_NUM+1];       // ��,��,��,ƽ,�� ����1
    BYTE4_INT32_TYPE Quad4ReaEng[MAX_TARIFF_NUM+1];       // ��,��,��,ƽ,�� ����4
    BYTE4_INT32_TYPE Quad2ReaEng[MAX_TARIFF_NUM+1];       // ��,��,��,ƽ,�� ����2
    BYTE4_INT32_TYPE Quad3ReaEng[MAX_TARIFF_NUM+1];       // ��,��,��,ƽ,�� ����3
    BYTE4_INT32_TYPE CmbActEng[MAX_TARIFF_NUM+1];         // ����й�-��,��,��,ƽ,��
    BYTE4_INT32_TYPE PosAppEng[MAX_TARIFF_NUM+1];         // �������ڵ���-��,��,��,ƽ,��
    BYTE4_INT32_TYPE NegAppEng[MAX_TARIFF_NUM+1];         // �������ڵ���-��,��,��,ƽ,��

    BYTE4_INT32_TYPE PhaAPosActEng[PhaMaxTariffNum+1];    // A�� ���� 
    BYTE4_INT32_TYPE PhaANegActEng[PhaMaxTariffNum+1];    // A�� ����
    BYTE4_INT32_TYPE PhaAPosReaEng[PhaMaxTariffNum+1];    // A�� ����
    BYTE4_INT32_TYPE PhaANegReaEng[PhaMaxTariffNum+1];    // A�� ����
    
    BYTE4_INT32_TYPE PhaA1ReaEng[PhaMaxTariffNum+1];    // A�� ����1
    BYTE4_INT32_TYPE PhaA2ReaEng[PhaMaxTariffNum+1];    // A�� ����2
    BYTE4_INT32_TYPE PhaA3ReaEng[PhaMaxTariffNum+1];    // A�� ����3
    BYTE4_INT32_TYPE PhaA4ReaEng[PhaMaxTariffNum+1];    // A�� ����4
    BYTE4_INT32_TYPE PhaACmbActEng[PhaMaxTariffNum+1];    // A�� ����й�
    BYTE4_INT32_TYPE PhaAActAppEng[PhaMaxTariffNum+1];    // A�� �������ڵ���
    BYTE4_INT32_TYPE PhaANegAppEng[PhaMaxTariffNum+1];    // A�� �������ڵ���
    
    BYTE4_INT32_TYPE PhaBPosActEng[PhaMaxTariffNum+1];    // B�� ���� 
    BYTE4_INT32_TYPE PhaBNegActEng[PhaMaxTariffNum+1];    // B�� ����
    BYTE4_INT32_TYPE PhaBPosReaEng[PhaMaxTariffNum+1];    // B�� ����
    BYTE4_INT32_TYPE PhaBNegReaEng[PhaMaxTariffNum+1];    // B�� ����

    BYTE4_INT32_TYPE PhaB1ReaEng[PhaMaxTariffNum+1];    // B�� ����1
    BYTE4_INT32_TYPE PhaB2ReaEng[PhaMaxTariffNum+1];    // B�� ����2
    BYTE4_INT32_TYPE PhaB3ReaEng[PhaMaxTariffNum+1];    // B�� ����3
    BYTE4_INT32_TYPE PhaB4ReaEng[PhaMaxTariffNum+1];    // B�� ����4
    BYTE4_INT32_TYPE PhaBCmbActEng[PhaMaxTariffNum+1];    // B�� ����й�
    BYTE4_INT32_TYPE PhaBActAppEng[PhaMaxTariffNum+1];    // B�� �������ڵ���
    BYTE4_INT32_TYPE PhaBNegAppEng[PhaMaxTariffNum+1];    // B�� �������ڵ���
    
    BYTE4_INT32_TYPE PhaCPosActEng[PhaMaxTariffNum+1];    // C�� ���� 
    BYTE4_INT32_TYPE PhaCNegActEng[PhaMaxTariffNum+1];    // C�� ����
    BYTE4_INT32_TYPE PhaCPosReaEng[PhaMaxTariffNum+1];    // C�� ����
    BYTE4_INT32_TYPE PhaCNegReaEng[PhaMaxTariffNum+1];    // C�� ����

    BYTE4_INT32_TYPE PhaC1ReaEng[PhaMaxTariffNum+1];    // C�� ����1
    BYTE4_INT32_TYPE PhaC2ReaEng[PhaMaxTariffNum+1];    // C�� ����2
    BYTE4_INT32_TYPE PhaC3ReaEng[PhaMaxTariffNum+1];    // C�� ����3
    BYTE4_INT32_TYPE PhaC4ReaEng[PhaMaxTariffNum+1];    // C�� ����4
    BYTE4_INT32_TYPE PhaCCmbActEng[PhaMaxTariffNum+1];    // C�� ����й�
    BYTE4_INT32_TYPE PhaCActAppEng[PhaMaxTariffNum+1];    // C�� �������ڵ���
    BYTE4_INT32_TYPE PhaCNegAppEng[PhaMaxTariffNum+1];    // C�� �������ڵ���

    /***�߾��ȵ��������� begin***/
    
    BYTE8_INT64_TYPE PosActEngHD[MAX_TARIFF_NUM+1];         // �����й�-��,��,��,ƽ,��
    BYTE8_INT64_TYPE NegActEngHD[MAX_TARIFF_NUM+1];         // �����й�-��,��,��,ƽ,��
    BYTE8_INT64_TYPE PosReaEngHD[MAX_TARIFF_NUM+1];         // �����޹�-��,��,��,ƽ,��
    BYTE8_INT64_TYPE NegReaEngHD[MAX_TARIFF_NUM+1];         // �����޹�-��,��,��,ƽ,��
    BYTE8_INT64_TYPE Quad1ReaEngHD[MAX_TARIFF_NUM+1];       // ��,��,��,ƽ,�� ����1
    BYTE8_INT64_TYPE Quad4ReaEngHD[MAX_TARIFF_NUM+1];       // ��,��,��,ƽ,�� ����4
    BYTE8_INT64_TYPE Quad2ReaEngHD[MAX_TARIFF_NUM+1];       // ��,��,��,ƽ,�� ����2
    BYTE8_INT64_TYPE Quad3ReaEngHD[MAX_TARIFF_NUM+1];       // ��,��,��,ƽ,�� ����3
    BYTE8_INT64_TYPE CmbActEngHD[MAX_TARIFF_NUM+1];         // ����й�-��,��,��,ƽ,��
    BYTE8_INT64_TYPE PosAppEngHD[MAX_TARIFF_NUM+1];         // �������ڵ���-��,��,��,ƽ,��
    BYTE8_INT64_TYPE NegAppEngHD[MAX_TARIFF_NUM+1];         // �������ڵ���-��,��,��,ƽ,��

    BYTE8_INT64_TYPE PhaAPosActEngHD[PhaMaxTariffNum+1];    // A�� ���� 
    BYTE8_INT64_TYPE PhaANegActEngHD[PhaMaxTariffNum+1];    // A�� ����
    BYTE8_INT64_TYPE PhaAPosReaEngHD[PhaMaxTariffNum+1];    // A�� ����
    BYTE8_INT64_TYPE PhaANegReaEngHD[PhaMaxTariffNum+1];    // A�� ����
    
    BYTE8_INT64_TYPE PhaA1ReaEngHD[PhaMaxTariffNum+1];    // A�� ����1
    BYTE8_INT64_TYPE PhaA2ReaEngHD[PhaMaxTariffNum+1];    // A�� ����2
    BYTE8_INT64_TYPE PhaA3ReaEngHD[PhaMaxTariffNum+1];    // A�� ����3
    BYTE8_INT64_TYPE PhaA4ReaEngHD[PhaMaxTariffNum+1];    // A�� ����4
    BYTE8_INT64_TYPE PhaACmbActEngHD[PhaMaxTariffNum+1];    // A�� ����й�
    BYTE8_INT64_TYPE PhaAActAppEngHD[PhaMaxTariffNum+1];    // A�� �������ڵ���
    BYTE8_INT64_TYPE PhaANegAppEngHD[PhaMaxTariffNum+1];    // A�� �������ڵ���
    
    BYTE8_INT64_TYPE PhaBPosActEngHD[PhaMaxTariffNum+1];    // B�� ���� 
    BYTE8_INT64_TYPE PhaBNegActEngHD[PhaMaxTariffNum+1];    // B�� ����
    BYTE8_INT64_TYPE PhaBPosReaEngHD[PhaMaxTariffNum+1];    // B�� ����
    BYTE8_INT64_TYPE PhaBNegReaEngHD[PhaMaxTariffNum+1];    // B�� ����

    BYTE8_INT64_TYPE PhaB1ReaEngHD[PhaMaxTariffNum+1];    // B�� ����1
    BYTE8_INT64_TYPE PhaB2ReaEngHD[PhaMaxTariffNum+1];    // B�� ����2
    BYTE8_INT64_TYPE PhaB3ReaEngHD[PhaMaxTariffNum+1];    // B�� ����3
    BYTE8_INT64_TYPE PhaB4ReaEngHD[PhaMaxTariffNum+1];    // B�� ����4
    BYTE8_INT64_TYPE PhaBCmbActEngHD[PhaMaxTariffNum+1];    // B�� ����й�
    BYTE8_INT64_TYPE PhaBActAppEngHD[PhaMaxTariffNum+1];    // B�� �������ڵ���
    BYTE8_INT64_TYPE PhaBNegAppEngHD[PhaMaxTariffNum+1];    // B�� �������ڵ���
    
    BYTE8_INT64_TYPE PhaCPosActEngHD[PhaMaxTariffNum+1];    // C�� ���� 
    BYTE8_INT64_TYPE PhaCNegActEngHD[PhaMaxTariffNum+1];    // C�� ����
    BYTE8_INT64_TYPE PhaCPosReaEngHD[PhaMaxTariffNum+1];    // C�� ����
    BYTE8_INT64_TYPE PhaCNegReaEngHD[PhaMaxTariffNum+1];    // C�� ����

    BYTE8_INT64_TYPE PhaC1ReaEngHD[PhaMaxTariffNum+1];    // C�� ����1
    BYTE8_INT64_TYPE PhaC2ReaEngHD[PhaMaxTariffNum+1];    // C�� ����2
    BYTE8_INT64_TYPE PhaC3ReaEngHD[PhaMaxTariffNum+1];    // C�� ����3
    BYTE8_INT64_TYPE PhaC4ReaEngHD[PhaMaxTariffNum+1];    // C�� ����4
    BYTE8_INT64_TYPE PhaCCmbActEngHD[PhaMaxTariffNum+1];    // C�� ����й�
    BYTE8_INT64_TYPE PhaCActAppEngHD[PhaMaxTariffNum+1];    // C�� �������ڵ���
    BYTE8_INT64_TYPE PhaCNegAppEngHD[PhaMaxTariffNum+1];    // C�� �������ڵ���
    
    /***�߾��ȵ��������� end***/

    DEM_NUM_TYPE CurrDemand[4][MAX_TARIFF_NUM+1];         // ���С����С����ޡ���������
    TERM_DEM_TIME_TYPE CurrDemandTime[4][MAX_TARIFF_NUM+1];// ���С����С����ޡ���������ʱ��

    DEM_NUM_TYPE QuaDemand[4][MAX_TARIFF_NUM+1];         // һ��������������������
    TERM_DEM_TIME_TYPE QuaDemandTime[4][MAX_TARIFF_NUM+1];// һ��������������������ʱ��

    DEM_NUM_TYPE AppaDemand[2][MAX_TARIFF_NUM+1];         // �������ڡ�������������
    TERM_DEM_TIME_TYPE AppaDemandTime[2][MAX_TARIFF_NUM+1];// �������ڡ�������������ʱ��

    
    DEM_NUM_TYPE PosActDemand[3];          // A��B��C�������й�����
    TERM_DEM_TIME_TYPE PosActDemandTime[3];// A��B��C�������й���������ʱ��

    DEM_NUM_TYPE NegActDemand[3];          // A��B��C�෴������
    TERM_DEM_TIME_TYPE NegActDemandTime[3];// A��B��C�෴����������ʱ��

    DEM_NUM_TYPE PosReaDemand[3];          // A��B��C����������
    TERM_DEM_TIME_TYPE PosReaDemandTime[3];// A��B��C��������������ʱ��

    DEM_NUM_TYPE NegReaDemand[3];          // A��B��C�෴������
    TERM_DEM_TIME_TYPE NegReaDemandTime[3];// A��B��C�෴����������ʱ��

    DEM_NUM_TYPE Qua1Demand[3];          // A��B��C��1��������
    TERM_DEM_TIME_TYPE Qua1DemandTime[3];// A��B��C��1������������ʱ��

    DEM_NUM_TYPE Qua2Demand[3];          // A��B��C��2��������
    TERM_DEM_TIME_TYPE Qua2DemandTime[3];// A��B��C��2������������ʱ��

    DEM_NUM_TYPE Qua3Demand[3];          // A��B��C��3��������
    TERM_DEM_TIME_TYPE Qua3DemandTime[3];// A��B��C��3������������ʱ��

    DEM_NUM_TYPE Qua4Demand[3];          // A��B��C��4��������
    TERM_DEM_TIME_TYPE Qua4DemandTime[3];// A��B��C��4������������ʱ��

    DEM_NUM_TYPE ActAppDemand[3];          // A��B��C��������������
    TERM_DEM_TIME_TYPE ActAppDemandTime[3];// A��B��C������������������ʱ��

    DEM_NUM_TYPE NegAppDemand[3];          // A��B��C�෴����������
    TERM_DEM_TIME_TYPE NegAppDemandTime[3];// A��B��C�෴��������������ʱ��

    int PosActPower;           // �����й�����
    int NegActPower;           // �����й�����
    int PhaAActPower;          // A���й�����
    int PhaBActPower;          // B���й�����
    int PhaCActPower;          // C���й�����

    int PosReaPower;           // �����޹�����
    int NegReaPower;           // �����޹�����
    int PhaAReaPower;          // A���޹�����
    int PhaBReaPower;          // B���޹�����
    int PhaCReaPower;          // C���޹�����

    int PosApparentPower;         // ˲ʱ���ڹ���
    int NegApparentPower;         // ˲ʱ���ڹ���
    int PhaAAppPower;          // A�����ڹ���
    int PhaBAppPower;          // B�����ڹ���
    int PhaCAppPower;          // C�����ڹ���

    int16 PowerFactor;        // �ܹ�������
    int16 PhaAPowerFactor;    // A�๦������
    int16 PhaBPowerFactor;    // B�๦������
    int16 PhaCPowerFactor;    // C�๦������

    uint16 PhaAVolt;           // A���ѹ 
    uint16 PhaBVolt;           // B���ѹ
    uint16 PhaCVolt;           // C���ѹ
    uint16 PhaZeroVolt;        // �����ѹ
    uint16 PhaPosVolt;         // �����ѹ
    uint16 PhaNegVolt;         // �����ѹ

    uint32 LineABVolt;         //AB�ߵ�ѹ
    uint32 LineBCVolt;         //BC�ߵ�ѹ
    uint32 LineCAVolt;         //CA�ߵ�ѹ

    uint32 PhaACurr;           // A����� XX.XXXX
    uint32 PhaBCurr;           // B����� 
    uint32 PhaCCurr;           // C�����
    uint32 PhaMCurr;           // �������
    uint16 PhaPosCurr;         // �������
    uint16 PhaNegCurr;         // �������
    uint32 PhaGCurr;           // �����ߵ���
    uint32 ResidCurr;          // ʣ�����
    
    uint32 LineFrz;			   //����Ƶ��
    uint32 LineAFrz;		   //A��Ƶ��
    uint32 LineBFrz;		   //B��Ƶ��
    uint32 LineCFrz;		   //C��Ƶ��
    int16 MtrTemp;
    uint16 ClkBatVol;
    uint16 CBBatVol;
    uint32 ActDemand;
    uint32 ReDemand;
    uint32 AppDemand;
    uint16 VoltUnbalanceRate;			   //��ѹ��ƽ����
    uint16 CurrUnbalanceRate;			   //������ƽ����
    
    uint16 PhaAAngle;        // A���ѹ�������
    uint16 PhaBAngle;        // B���ѹ�������
    uint16 PhaCAngle;        // C���ѹ�������
    uint16 PhaAVoltAngle;    // A���ѹ���
    uint16 PhaBVoltAngle;    // B���ѹ���
    uint16 PhaCVoltAngle;    // C���ѹ���
    uint16 PhaACurrAngle;    // A��������
    uint16 PhaBCurrAngle;    // B��������
    uint16 PhaCCurrAngle;    // C��������

    uint16 PhaAVoltHarmonic[MAX_HARMONIC_NUM];    // A���ѹг�������ʣ��ܼ�2��n�Σ�
    uint16 PhaBVoltHarmonic[MAX_HARMONIC_NUM];    // B���ѹг�������ʣ��ܼ�2��n�Σ�
    uint16 PhaCVoltHarmonic[MAX_HARMONIC_NUM];    // C���ѹг�������ʣ��ܼ�2��n�Σ�
    uint16 PhaACurrHarmonic[MAX_HARMONIC_NUM];    // A�����г�������ʣ��ܼ�2��n�Σ�
    uint16 PhaBCurrHarmonic[MAX_HARMONIC_NUM];    // B�����г�������ʣ��ܼ�2��n�Σ�
    uint16 PhaCCurrHarmonic[MAX_HARMONIC_NUM];    // C�����г�������ʣ��ܼ�2��n�Σ�

    uint16 PhaAVoltHarmonicValue[MAX_HARMONIC_NUM];    // A���ѹг���������ܼ�2��n�Σ�
    uint16 PhaBVoltHarmonicValue[MAX_HARMONIC_NUM];    // B���ѹг���������ܼ�2��n�Σ�
    uint16 PhaCVoltHarmonicValue[MAX_HARMONIC_NUM];    // C���ѹг���������ܼ�2��n�Σ�
    uint32 PhaACurrHarmonicValue[MAX_HARMONIC_NUM];    // A�����г���������ܼ�2��n�Σ�
    uint32 PhaBCurrHarmonicValue[MAX_HARMONIC_NUM];    // B�����г���������ܼ�2��n�Σ�
    uint32 PhaCCurrHarmonicValue[MAX_HARMONIC_NUM];    // C�����г���������ܼ�2��n�Σ�

    OOP_TIMEZONE_T CurrZoneTableRD;//��ǰ��ʱ����(�ӽ���ģ���ȡ)
    OOP_TIMEZONE_T AddiZoneTableRD;//������ʱ����(�ӽ���ģ���ȡ)
    OOP_DAYPERIOD_T CurrDayPeriTableRD;//��ǰ����ʱ�α�(�ӽ���ģ���ȡ)
    OOP_DAYPERIOD_T AddiDayPeriTableRD;//��ǰ����ʱ�α�(�ӽ���ģ���ȡ)
    uint8 MaxDemandPeriodRD;//�����������(�ӽ���ģ���ȡ)
    uint8 SlipDiffTimeRD;//����ʱ��(�ӽ���ģ���ȡ)

    OOP_TIMEZONE_T CurrZoneTableSet;//��ǰ��ʱ����(���ý���ģ��)
    OOP_TIMEZONE_T AddiZoneTableSet;//������ʱ����(���ý���ģ��)
    OOP_DAYPERIOD_T CurrDayPeriTableSet;//��ǰ����ʱ�α�(���ý���ģ��)
    OOP_DAYPERIOD_T AddiDayPeriTableSet;//��������ʱ�α�(���ý���ģ��)
    uint8 MaxDemandPeriodSet;//�����������(���ý���ģ��)
    uint8 SlipDiffTimeSet;//����ʱ��(���ý���ģ��)

    BOOL CurrZoneNumSetFlag;//��ǰ��ʱ����(���ý���ģ��)��־ ʱ������
    BOOL AddiZoneNumSetFlag;//������ʱ����(���ý���ģ��)��־ ʱ������
    BOOL CurrDayTableNumSetFlag;//��ǰ����ʱ�α�(���ý���ģ��)��־ ������
    BOOL AddiDayTalbeNumSetFlag;//��������ʱ�α�(���ý���ģ��)��־ ������
    BOOL CurrDayPeriNumSetFlag;//��ǰ����ʱ�α�(���ý���ģ��)��־ ʱ������
    BOOL AddiDayPeriNumSetFlag;//��������ʱ�α�(���ý���ģ��)��־ ʱ������
    uint8 CurrDayPeriNumSetFlagArr[OOP_MAX_DAYPERIODS];//��ǰ����ʱ�α�(���ý���ģ��)��־ ʱ������
    uint8 AddiDayPeriNumSetFlagArr[OOP_MAX_DAYPERIODS];//��������ʱ�α�(���ý���ģ��)��־ ʱ������

    BOOL CurrZoneTableSetFlag;//��ǰ��ʱ����(���ý���ģ��)��־
    BOOL AddiZoneTableSetFlag;//������ʱ����(���ý���ģ��)��־
    BOOL CurrDayPeriTableSetFlag;//��ǰ����ʱ�α�(���ý���ģ��)��־
    BOOL AddiDayPeriTableSetFlag;//��������ʱ�α�(���ý���ģ��)��־
    uint8 CurrDayPeriSetFlagArr[OOP_MAX_DAYPERIODS];//��ǰ����ʱ�α�(���ý���ģ��)��־
    uint8 AddiDayPeriSetFlagArr[OOP_MAX_DAYPERIODS];//��������ʱ�α�(���ý���ģ��)��־
    BOOL MaxDemandPeriodSetFlag;//�����������(���ý���ģ��)��־
    BOOL SlipDiffTimeSetFlag;//����ʱ��(���ý���ģ��)��־

    OOP_DATETIME_S_T DateTimeSet;//�ն˵�����ʱ��
    BOOL DateSetFlag;//�ն˵��������ڱ�־
    BOOL TimeSetFlag;//�ն˵�����ʱ���־

    OOP_DATETIME_S_T AddiZoneChangeTimeRD;//������ʱ�����л�ʱ�� ��ȡ
    OOP_DATETIME_S_T AddiPeriChangeTimeRD;//��������ʱ�α��л�ʱ�� ��ȡ
    OOP_PERIOD_T ZonePeriNumRD;//ʱ��ʱ���� ��ȡ

    OOP_BITSTR8_T ActPowCombWordRD;//�й���Ϸ�ʽ������ ��ȡ
    OOP_BITSTR8_T ReaPowComb1WordRD;//�޹���Ϸ�ʽ1������ ��ȡ
    OOP_BITSTR8_T ReaPowComb2WordRD;//�޹���Ϸ�ʽ2������ ��ȡ

    OOP_DATETIME_S_T AddiZoneChangeTimeSet;//������ʱ�����л�ʱ�� ����
    OOP_DATETIME_S_T AddiPeriChangeTimeSet;//��������ʱ�α��л�ʱ�� ����
    OOP_PERIOD_T ZonePeriNumSet;//ʱ��ʱ���� ����

    OOP_BITSTR8_T ActPowCombWordSet;//�й���Ϸ�ʽ������ ����
    OOP_BITSTR8_T ReaPowComb1WordSet;//�޹���Ϸ�ʽ1������ ����
    OOP_BITSTR8_T ReaPowComb2WordSet;//�޹���Ϸ�ʽ2������ ����

    BOOL ZoneTableNumSetFlag;//ʱ����(���ý���ģ��)��־
    BOOL DayPeriTableNumSetFlag;//��ʱ�α�(���ý���ģ��)��־
    BOOL DayPeriNumSetFlag;//��ʱ��(���ý���ģ��)��־
    BOOL CostRateNumSetFlag;//������(���ý���ģ��)��־
    BOOL HolidayNumSetFlag;//������(���ý���ģ��)��־

    BOOL AddiZoneChangeTimeSetFlag;//������ʱ�����л�ʱ�� ��־
    BOOL AddiPeriChangeTimeSetFlag;//��������ʱ�α��л�ʱ�� ��־
    BOOL ActPowCombWordSetFlag;//�й���Ϸ�ʽ������ ��־
    BOOL ReaPowComb1WordSetFlag;//�޹���Ϸ�ʽ1������ ��־
    BOOL ReaPowComb2WordSetFlag;//�޹���Ϸ�ʽ2������ ��־

    uint8 g_nCurrDayPeriLabel;//����ģ��Ŀǰ���е���ʱ�α��ǵڼ���, 0����ǰ���е�һ��, 1����ǰ���еڶ���

    NUMBER_TYPE BreakPhaNum[4];    // �ܡ�A��B��C�������
    TIME_TYPE BreakPhaTime[4];     // �ܡ�A��B��C����ʱ��
    COM_TIME_TYPE BreakPhaStart[4];// �ܡ�A��B��C������ʼʱ��
    COM_TIME_TYPE BreakPhaEnd[4];  // �ܡ�A��B��C�������ʱ��    
    uint8     ActConst[3];         // ��������й���
    uint8     ReactConst[3];       // ��������޹���    
    uint8     RealTime[6];         // ��ǰʵʱʱ�� 6B ������ʱ����
    BYTE4_INT32_TYPE LastEnergy[8][MAX_TARIFF_NUM+1];  // �������С����С����ޡ����ޡ�1��4���޵���
    BYTE4_INT32_TYPE LastPhaEng[12][PhaMaxTariffNum+1];// ���·��� ���С����С����ޡ�����(A��B��C)
    BYTE4_INT32_TYPE LastCombActEng[MAX_TARIFF_NUM+1]; // ��һ����������й�������
    DEM_NUM_TYPE LastDemand[4][MAX_TARIFF_NUM+1];      // �������С����С����ޡ���������
    TERM_DEM_TIME_TYPE LastDemandTime[4][MAX_TARIFF_NUM+1]; // �������С����С����ޡ���������ʱ��   

    uint16 MinPhaAVolt;                // A����ӵ�ѹ 
    uint16 MinPhaBVolt;                // B����ӵ�ѹ
    uint16 MinPhaCVolt;                // C����ӵ�ѹ    

    uint8    BatWorkTime[4];            // ��ع���ʱ��
    uint8    ProgramNum[3];             // ����ܴ���
    uint8    ProgramTime[6];            // ���һ�α�̷���ʱ��
    uint8    ClrMetNum[3];              // ��������ܴ���
    uint8    ClrMetTime[6];             // ���һ�����㷢��ʱ��
    uint8    ClrMaxDemNum[3];           // ���������ܴ���
    uint8    ClrMaxDemTime[6];          // ���һ�����㷢��ʱ��
    uint8    ClrEventNum[3];            // �¼������ܴ���
    uint8    ClrEventTime[6];           // ���һ�����㷢��ʱ��
    uint8    AdjClockNum[3];            // Уʱ�ܴ���
    uint8    AdjClockBeforeTime[6];     // Уʱǰʱ��
    uint8    AdjClockAfterTime[6];      // Уʱ��ʱ��
    uint8    AdjClockTime[6];           // ���һ��Уʱ����ʱ��
    uint8    AdjClockTableNum[3];       // ʱ�α����ܴ���
    uint8    AdjClockTableLastTime[6];  // ���һ��ʱ�α���ʱ��
        
    METER_STATE_WORD_TYPE MetStusPara;  // �������״̬��

    TOPO_IDEN_RESULT TopoIdenResult;    // (�Զ�����չ)����ʶ����(�㽭)

    BYTE4_INT32_TYPE CuPosActEng;       // ��ǰͭ���й��ܵ���ʾֵ
    BYTE4_INT32_TYPE FePosActEng;       // ��ǰ�����й��ܵ���ʾֵ
    BYTE4_INT32_TYPE LastCuPosActEng;   // ����ͭ���й��ܵ���ʾֵ
    BYTE4_INT32_TYPE LastFePosActEng;   // ���������й��ܵ���ʾֵ

    BYTE4_INT32_TYPE TIME_ZERO_Eng[2][MAX_TARIFF_NUM+1];  // (��һ����ʱ������)�����й�-��,��,��,ƽ,��    

    uint8 powerType;                    // ���߷�ʽ
    char  HaltCnt[3];
    char  HaltInfo[120];
    
    DEM_NUM_TYPE CurrPhaDemand[3];
    TERM_DEM_TIME_TYPE CurrPhaDemandTime[3];
    
    int16  TemperatureValue;

    int PhaAReaPowerBasic;          // A������޹�����
    int PhaBReaPowerBasic;          // B������޹�����
    int PhaCReaPowerBasic;          // C������޹�����

    uint32 PhaAVoltHD;              // �߾���A���ѹ 
    uint32 PhaBVoltHD;              // �߾���B���ѹ
    uint32 PhaCVoltHD;              // �߾���C���ѹ
}__attribute__ ((packed)) TERM_SHARE_DATA_TYPE;

typedef struct     
{    
    uint8  VoltCurBit    :1; // ��ѹ������
    uint8  PowBit        :1; // ����
    uint8  AngFreqBit    :1; // ��ǡ�Ƶ��
    uint8  EngBit        :1; // ����
    uint8  StatBit       :1; // ״̬
    uint8  CheckBit      :1; // ���üĴ���
    uint8  DemBit       :1; // ����     
}__attribute__ ((packed)) MEA_INST_MARK_BIT_TYPE;

typedef union     
{
    uint8 Word;
    MEA_INST_MARK_BIT_TYPE BitSect; 
}__attribute__ ((packed)) MEA_INST_MARK_TYPE;

typedef struct 
{
    uint16 PhaAActPowFlag    : 1;    
    uint16 PhaBActPowFlag    : 1;
    uint16 PhaCActPowFlag    : 1;
    uint16 ActPowFlag        : 1;
    
    uint16 PhaAReaPowFlag    : 1;    
    uint16 PhaBReaPowFlag    : 1;    
    uint16 PhaCReaPowFlag    : 1;
    uint16 ReaPowFlag        : 1;
    
    uint16 PhaAAppPowFlag    : 1;    
    uint16 PhaBAppPowFlag    : 1;    
    uint16 PhaCAppPowFlag    : 1;
    uint16 AppPowFlag        : 1;
}__attribute__ ((packed)) POW_BIT_TYPE;           // ���޹����ʷ�����λ�� 
typedef union   
{ 
    POW_BIT_TYPE  BitSect;
    uint16        Word;
}__attribute__ ((packed)) POW_DIR_TYPE;           // ���޹����ʷ�����

typedef struct 
{
    uint16 PhaAVolCurrAngFlag    : 1;    
    uint16 PhaBVolCurrAngFlag    : 1;
    uint16 PhaCVolCurrAngFlag    : 1;
    
    uint16 PhaAVolAngFlag    : 1;    
    uint16 PhaBVolAngFlag    : 1;    
    uint16 PhaCVolAngFlag    : 1;
    
    uint16 PhaACurrAngFlag    : 1;    
    uint16 PhaBCurrAngFlag    : 1;    
    uint16 PhaCCurrAngFlag    : 1;
}__attribute__ ((packed)) ANGLE_BIT_TYPE;           // ��ѹ/������Ƿ�����λ�� 
typedef union   
{ 
    ANGLE_BIT_TYPE  BitSect;
    uint16        Word;
}__attribute__ ((packed)) ANGLE_DIR_TYPE;           // ��ѹ/������Ƿ�����


typedef struct 
{
    uint8     TotActEngFlag    : 1;
    uint8     PosActEngFlag    : 1;
    uint8     NegActEngFlag    : 1;
    uint8     PosReaEngFlag    : 1;
    uint8     NegReaEngFlag    : 1;
    uint8     PosAppEngFlag    : 1;
    uint8     NegAppEngFlag    : 1;
    uint8     ReserveBit        : 1;
}__attribute__ ((packed)) TOT_ENG_BIT_TYPE;       // ���޹�����������λ�� 

typedef union   
{
    TOT_ENG_BIT_TYPE  BitSet;
    uint8             Word;
}__attribute__ ((packed)) TOT_ENG_DIR_TYPE;       // ���޹����ʷ�����

typedef  struct 
{
    uint8   Data[4];                              // ���ݱ�ʶ
} DI_ITEM_TYPE;

typedef struct 
{
    int8  EngFlag;                               // �������(���ű�ʶ)-1:������ 
    int8  AddFlag;                               // �ۼӱ�־
    uint16 CurPlu;                                // �²ɼ�����
    uint16 PluConst;                              // �������ÿ0.01��
    DI_ITEM_TYPE      DIMark;                     // ���ݱ�־
    uint16            *pResPlu;                   // ʣ�������׵�ַ  
    BYTE4_INT32_TYPE  *pEnergy;                   // ����������RAM�׵�ַ
} ENG_CAL_SAVE_INFOR_TYPE;// �����ļ���ͱ�����Ϣ�ṹ��

typedef struct
{
    uint8     VoltConvFlag    : 1;                // ��ѹ�������־
    uint8     CurrConvFlag    : 1;                // �����������־
    uint8     SIGFlag         : 1;                // SIG��־
    uint8     ReserveBit1     : 1;               
    uint8     PhaALatentFlag  : 1;                // A��Ǳ����־
    uint8     PhaBLatentFlag  : 1;                // B��Ǳ����־
    uint8     PhaCLatentFlag  : 1;                // C��Ǳ����־
    uint8     ReserveBit2     : 1;
}__attribute__ ((packed))MEA_STATUS_BIT_TYPE;     // ������Ǳ����ǵ�

typedef union
{
    MEA_STATUS_BIT_TYPE BitSect;
    uint8 Word;
}__attribute__ ((packed))MEA_STATUS_TYPE;         // ������Ǳ����ǵ� 

typedef struct
{
    uint8     PhaAVolt        : 1;
    uint8     PhaBVolt        : 1;
    uint8     PhaCVolt        : 1;
    uint8     PhaNVolt        : 1;
    uint8     PhaACurr        : 1;
    uint8     PhaBCurr        : 1;
    uint8     PhaCCurr        : 1;
    uint8     PhaNCurr        : 1;
}__attribute__ ((packed)) VOLT_CURR_BIT_TYPE;     // ��ѹ���������λ��

typedef struct
{
    VOLT_CURR_BIT_TYPE  Bitsect;
    uint8               Word;
}__attribute__ ((packed)) VOLT_CURR_MARK_TYPE;

typedef struct
{   // E214: 12��REG  COM:36Byte        
    BYTE4_INT32_TYPE    AWATTHR;    // 4    24    32ZP    (0x0031) A���й�ȫ��������
    BYTE4_INT32_TYPE    BWATTHR;    // 4    24    32ZP    (0x0032) B���й�ȫ��������
    BYTE4_INT32_TYPE    CWATTHR;    // 4    24    32ZP    (0x0033) C���й�ȫ��������
    BYTE4_INT32_TYPE    ZWATTHR;    // 4    24    32ZP    (0x0034) �����й�ȫ��������

    BYTE4_INT32_TYPE    AVARHR;     // 4    24    32ZP    (0x0035) A���޹�ȫ��������
    BYTE4_INT32_TYPE    BVARHR;     // 4    24    32ZP    (0x0036) B���޹�ȫ��������
    BYTE4_INT32_TYPE    CVARHR;     // 4    24    32ZP    (0x0037) C���޹�ȫ��������
    BYTE4_INT32_TYPE    ZVARHR;     // 4    24    32ZP    (0x0038) �����޹�ȫ��������

    BYTE4_INT32_TYPE    AVAHR;      // 4    24    32ZP    (0x70) A������ȫ��������
    BYTE4_INT32_TYPE    BVAHR;      // 4    24    32ZP    (0x71) B������ȫ��������
    BYTE4_INT32_TYPE    CVAHR;      // 4    24    32ZP    (0x72) C������ȫ��������
    BYTE4_INT32_TYPE    ZVAHR;      // 4    24    32ZP    (0x77) ��������ȫ��������
}ENERG_PLUSE_TYPE;

// ����оƬ��̬����Ӧ�ò㽻���ṹ�嶨��
typedef struct 
{   
    MEA_INST_MARK_TYPE    MeaInstMark;            // ������־��0x1F--���ݲɼ��������
    
    // ʵʱ���ݶ�Ӧ��־
    POW_DIR_TYPE          CurPowDir;         // ���޹������ڹ��ʷ����־ [MSB->LSB:�޹���CBA�й���CBA]
    TOT_ENG_DIR_TYPE      TotEngDir;         // ���޹����������������־
    MEA_STATUS_TYPE       MeaStatusMark;     // ������Ǳ����ǵ�
    VOLT_CURR_MARK_TYPE   VoltCurrMark;      // ��ѹ���������־

    // ����ģʽ��־������У��С����ʱ�����峣���Ŵ�8��
    uint8 TestModeFlag;
    
    // ��ѹ��ʽ:  XXXX.XXXX V   4λС��    �޷���
    uint32 PhaAVolt;                              // A���ѹ
    uint32 PhaBVolt;                              // B���ѹ
    uint32 PhaCVolt;                              // C���ѹ
    uint32 PhaMVolt;                              // ��ѹʸ����
    
    // ������ʽ:  XXXX.XXXX A   4λС�� 
    int PhaACurr;                            // A�����  �з���(�й����ʷ���)
    int PhaBCurr;                            // B�����  �з���(�й����ʷ���)
    int PhaCCurr;                            // C�����  �з���(�й����ʷ���)
    uint32 PhaMCurr;                         // �������      �޷���
    uint32 PhaGCurr;                         // �����ߵ���    �޷���
    int ResidCurr;                           // ʣ�����      �з���(�й����ʷ���)
    int AverageCurr;                         // ����ƽ������  �з���(�й����ʷ���)

    // ���ʸ�ʽ:  XX.XXXXXX kW  6λС��  �з���(�й����ʷ���)�����ܺ�֣���07����Լ��Ӧ
    int ActPower;                            // ���й�����
    int PhaAActPower;                        // A���й�����
    int PhaBActPower;                        // B���й�����
    int PhaCActPower;                        // C���й�����
    int ReaPower;                            // ���޹�����
    int PhaAReaPower;                        // A���޹�����
    int PhaBReaPower;                        // B���޹�����
    int PhaCReaPower;                        // C���޹�����
    int AppPower;                            // ˲ʱ���ڹ���
    int PhaAAppPower;                        // A�����ڹ���
    int PhaBAppPower;                        // B�����ڹ���
    int PhaCAppPower;                        // C�����ڹ���    

    // ����������ʽ:  XXX.XXXXX   5λС��    �з���(�й����ʷ���)
    int PowFactor;                           // �ܹ�������
    int PhaAPowFactor;                       // A�๦������
    int PhaBPowFactor;                       // B�๦������
    int PhaCPowFactor;                       // C�๦������
    

    // ����Ƶ�ʸ�ʽ:  XXXX.XXXX Hz  4λС��  �޷���
    uint32 ElectNetFrequ;                    // ����Ƶ��XX.XXXX

    // �¶ȸ�ʽ:  XXX.X Hz ���϶�  1λС��  �з���(���λ����)
    int16 MeaICTempNum;                     // ����оƬ�Բ��¶�

    // �����ѹ�����������ǣ����ݸ�ʽ(XXX.X)
    uint16 PhaAVoltAngle;                    // A���ѹ���(XXX.X)
    uint16 PhaBVoltAngle;                    // B���ѹ���
    uint16 PhaCVoltAngle;                    // C���ѹ���
    uint16 PhaACurrAngle;                    // A��������
    uint16 PhaBCurrAngle;                    // B��������
    uint16 PhaCCurrAngle;                    // C��������

    // �����ѹ������нǣ����ݸ�ʽ(XXX.X) -- 07����Լ 02 07 01/02/03 00
    uint16 PhaACurrVoltAngle;                // A��ѹ���н�
    uint16 PhaBCurrVoltAngle;                // B��ѹ���н�
    uint16 PhaCCurrVoltAngle;                // C��ѹ���н�

    // A,B,C,�����޹����޺�
    uint8 PhaACurrQuadNum;                   // A�൱ǰ���޺�
    uint8 PhaBCurrQuadNum;                   // B�൱ǰ���޺�
    uint8 PhaCCurrQuadNum;                   // C�൱ǰ���޺�
    uint8 CurrQuadNum;                       // ��ǰ���޺� 1��1���� 2��2���� 3��3���� 4��4����

    ENERG_PLUSE_TYPE      EnergPluse;        // ������ԭʼ���ݣ�������Ӧ�ò����
}MEA_REAL_DATA_COM_TYPE;

typedef struct     
{
    uint8  FreshMark            :1; // ˢ������
    uint8  ActReaEng            :1; // ����
    uint8  PhaActReaEng         :1;
    uint8  ActReaDem            :1; // ����
    uint8  BreakPha             :1;
    uint8  ProgRecd             :1; 
    uint8  MonFreze             :1; 
    uint8  TimeZoneSwitch       :1;  
}__attribute__ ((packed)) SHARED_MEM_MARK_BIT_TYPE;

typedef union     
{ 
    uint8   Word;
    SHARED_MEM_MARK_BIT_TYPE Bitsect; 
}__attribute__ ((packed)) SHARED_MEM_MARK_BYTE;


typedef struct 
{    
    uint8 PhaAAppCurrentFlag;
    uint8 PhaBAppCurrentFlag;
    uint8 PhaCAppCurrentFlag;
    uint8 ZeroLineCurrentFlag;  //���ߵ���
    uint8 ZeroCurrentFlag;      //�������
}CURRENT_SIGN_TYPE;//������������1��0��

typedef struct 
{
    uint8 AppPfFlag;
    uint8 PhaAAppPfFlag;
    uint8 PhaBAppPfFlag;
    uint8 PhaCAppPfFlag;
}POWER_FACTOR_TYPE;//����������������1��0��

typedef struct 
{
    uint8 PhaAVoltHarmonicFlag[MAX_HARMONIC_NUM];    // A���ѹг�������ʣ��ܼ�2��n�Σ�
    uint8 PhaBVoltHarmonicFlag[MAX_HARMONIC_NUM];    // B���ѹг�������ʣ��ܼ�2��n�Σ�
    uint8 PhaCVoltHarmonicFlag[MAX_HARMONIC_NUM];    // C���ѹг�������ʣ��ܼ�2��n�Σ�
    uint8 PhaACurrHarmonicFlag[MAX_HARMONIC_NUM];    // A�����г�������ʣ��ܼ�2��n�Σ�
    uint8 PhaBCurrHarmonicFlag[MAX_HARMONIC_NUM];    // B�����г�������ʣ��ܼ�2��n�Σ�
    uint8 PhaCCurrHarmonicFlag[MAX_HARMONIC_NUM];    // C�����г�������ʣ��ܼ�2��n�Σ�
    
    uint8 PhaAVoltHarmonicValueFlag[MAX_HARMONIC_NUM];    // A���ѹг���������ܼ�2��n�Σ�
    uint8 PhaBVoltHarmonicValueFlag[MAX_HARMONIC_NUM];    // B���ѹг���������ܼ�2��n�Σ�
    uint8 PhaCVoltHarmonicValueFlag[MAX_HARMONIC_NUM];    // C���ѹг���������ܼ�2��n�Σ�
    uint8 PhaACurrHarmonicValueFlag[MAX_HARMONIC_NUM];    // A�����г���������ܼ�2��n�Σ�
    uint8 PhaBCurrHarmonicValueFlag[MAX_HARMONIC_NUM];    // B�����г���������ܼ�2��n�Σ�
    uint8 PhaCCurrHarmonicValueFlag[MAX_HARMONIC_NUM];    // C�����г���������ܼ�2��n�Σ�
}HARMONIC_RATE_VALUE_TYPE;//г��������������1��0��

typedef struct 
{    
    uint8 SignalS1Flag;//�ź�ǿ��S1 ��������1��0��
    uint8 SignalS2Flag;//�ź�ǿ��S2 ��������1��0��
    uint8 NoiseFlag;//�������� ��������1��0��
}SIGNAL_IDEN_TYPE;//(�Զ�����չ)�㽭����ʶ����

typedef struct 
{    
    uint16 PluNumPer01Kwh;                        // ÿ0.01��������
    uint8  TestModeFlag;                          // ����ģʽ���
    POW_DIR_TYPE CurPowDir;                       // ���޹������ڹ��ʷ����־ [MSB->LSB:�޹���CBA�й���CBA]
    TOT_ENG_DIR_TYPE TotEngDir;                   // ���޹����������������־
    MEA_STATUS_TYPE    MeaStatusMark;             // ������Ǳ����ǵ�
    VOLT_CURR_MARK_TYPE    VoltCurrMark;          // ��ѹ���������־

    uint8 PhaACurrQuadNum;                        // A�൱ǰ���޺�
    uint8 PhaBCurrQuadNum;                        // B�൱ǰ���޺�
    uint8 PhaCCurrQuadNum;                        // C�൱ǰ���޺�
    uint8 CurrQuadNum;                            // ��ǰ���޺� 1��1���� 2��2���� 3��3���� 4��4���� 

    uint8    TotActMark;
    uint8    IncTotActMark;                       // �������й���ʶ 0x55:����
    uint16   IncTotActEng;                        // �ôε�������

    uint8 FirstLowVoltMark;                       // �͵�ѹ�µ���������ʶ
    PHASE_ACT_REA_APP_ENERGY_TYPE PhaEng[3];      // ��������BCD��

    ANGLE_DIR_TYPE AngleDir;                       // ��Ƿ���
    CURRENT_SIGN_TYPE CurrentSignType;  //��������1��0��
    POWER_FACTOR_TYPE PowerFactorType;  //������������1��0��
    uint8 CmbEnergyType[MAX_TARIFF_NUM+1];  //��ϵ��ܷ���1��0��
    uint8 CmbEnergyTypeHD[MAX_TARIFF_NUM+1];  //��ϵ���(�߾���)����1��0��
    
    uint8 PosReaEnergyType[MAX_TARIFF_NUM+1];  //�����޹����ܷ���1��0��
    uint8 NegReaEnergyType[MAX_TARIFF_NUM+1];  //�����޹����ܷ���1��0��
    uint8 PhaAPosReaEngType[PhaMaxTariffNum+1];    // A�� ���� ����1��0��
    uint8 PhaANegReaEngType[PhaMaxTariffNum+1];    // A�� ���� ����1��0��
    uint8 PhaBPosReaEngType[PhaMaxTariffNum+1];    // B�� ���� ����1��0��
    uint8 PhaBNegReaEngType[PhaMaxTariffNum+1];    // B�� ���� ����1��0��
    uint8 PhaCPosReaEngType[PhaMaxTariffNum+1];    // C�� ���� ����1��0��
    uint8 PhaCNegReaEngType[PhaMaxTariffNum+1];    // C�� ���� ����1��0��

    uint8 PosReaEnergyTypeHD[MAX_TARIFF_NUM+1];  //�����޹�����(�߾���)����1��0��
    uint8 NegReaEnergyTypeHD[MAX_TARIFF_NUM+1];  //�����޹�����(�߾���)����1��0��
    uint8 PhaAPosReaEngTypeHD[PhaMaxTariffNum+1];    // A��(�߾���) ���� ����1��0��
    uint8 PhaANegReaEngTypeHD[PhaMaxTariffNum+1];    // A��(�߾���) ���� ����1��0��
    uint8 PhaBPosReaEngTypeHD[PhaMaxTariffNum+1];    // B��(�߾���) ���� ����1��0��
    uint8 PhaBNegReaEngTypeHD[PhaMaxTariffNum+1];    // B��(�߾���) ���� ����1��0��
    uint8 PhaCPosReaEngTypeHD[PhaMaxTariffNum+1];    // C��(�߾���) ���� ����1��0��
    uint8 PhaCNegReaEngTypeHD[PhaMaxTariffNum+1];    // C��(�߾���) ���� ����1��0��
    
    HARMONIC_RATE_VALUE_TYPE HarmonicRateValueType;  //г��������/��������1��0��

    SIGNAL_IDEN_TYPE SignalIdenType;  //(�Զ�����չ)�㽭����ʶ���� 1��0��
}PRIVTE_MEASURE_TYPE; 






/*******************************************************************************
* �ṹ��
*******************************************************************************/
typedef struct
{
    uint8 ms100          :1;            // ��100ms��־ 
    uint8 ms500          :1;            // ��500ms��־ 
    uint8 Second         :1;            // �����־
    uint8 Minute         :1;            // ���ֱ�־
    uint8 Hour           :1;            // ��ʱ��־
    uint8 Day            :1;            // ���ձ�־
    uint8 Month          :1;            // ���±�־
    uint8 ReserveBit8    :1;            // ����
}PAST_TIME_FLAG_BIT_TYPE;

typedef union   
{   
    PAST_TIME_FLAG_BIT_TYPE   BitSect;
    uint8                     Word;
}PAST_TIME_FLAG_TYPE;                   // ��ʱ���־

typedef struct
{
    YYMMDDWWhhnnss_TYPE       InfoCurTime;        // ��ǰʱ��
    YYMMDDWWhhnnss_TYPE       InfoLastTime;       // �ϴαȽ�ʱ��
    PAST_TIME_FLAG_TYPE       InfoTimeFlag;       // ���֡������־
}TIME_INFO;  // ʱ����Ϣ��������ǰʱ�䡢���ֹ����־�ȵ�

//�¼�������Ϣ
typedef struct
{
    OOP_OAD_U           eventOAD;                       //�¼���ʶ
    uint8               checkflag;                      //��Ч��ʶ
    uint8               reportflag;                     //�ϱ���ʶ
    uint32              recordnum;                      //��¼���(ĳ���¼��洢������)
    uint16              eventNum;                       //�¼���¼��
    uint32              EventStoragNumRecAddr;          //�洢��� ��ʼ��ַ
    uint32              EventStoragTmRecAddr;           //����ʱ�� ��ʼ��ַ
    uint32              EventFlagAddr;                  //�¼�������־ ��ʼ��ַ
    uint16              EventFlagLen;                   //�¼�������־ ����
    OOP_RCSD_T          eventRCSD;                      //�¼��ϱ�RCSD
    OOP_DATETIME_S_T    starttime;      //����ʱ��
    OOP_DATETIME_S_T    endtime;        //����ʱ��
    uint32              storagenum;     //�洢���(д��¼�����ݵĳ���, �������ݿ��д洢���к�)
    uint32              ERFlg;          //״̬��ǣ�����¼��Ƿ���
    EVTSTATE            eventState;                     //�¼���ǰֵ
    EVTTIME             eventTime;                      //�¼�ʱ��״̬��¼��
    uint8               bType;                          //�¼����ͣ��ն� 0 �漰�����ܱ�1
    time_t              tStartTime;                     //����ʱ��
    uint8               EffectState;                    //�ų�����״̬
}EVENT_COMM_INFO;

typedef struct
{
    uint8 nState;//0�ѻָ� 1�ѷ���
    time_t tOccurTime;//����ʱ��
    time_t tRecovTime;//�ָ�ʱ��
}SINGLE_EVENT_INFO;  //�����¼�״̬


typedef struct
{
    SINGLE_EVENT_INFO stEvent300F;//���ɵ�ѹ�������¼�
}TOTAL_EVENT_INFO;  //�����¼�״̬

typedef struct
{
    EVENT_COMM_INFO   eventCommInfo;
    uint8             ishappen;         //״̬��ǣ�����¼��Ƿ���
    uint8             nDelay;           //�ж���ʱ
    uint8             initflag;         //������ʼ����ǣ���ʶ�����Ƿ��ʼ��
    OOP_WORD3V_T      valTemp;          //��ѹ
    OOP_METWORD_T     meterRsw;         //�������״̬��7
}EVENT_300F_INFO;
















#ifdef __cplusplus
}
#endif

#endif // __AC_METER_REDEF_H__
