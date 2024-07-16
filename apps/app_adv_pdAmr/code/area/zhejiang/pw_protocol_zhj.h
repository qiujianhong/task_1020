/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2020-01-03
*********************************************************************
*/


#ifndef __PW_PROTOCOL_ZHJ_H__
#define __PW_PROTOCOL_ZHJ_H__

#include "pw_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************�㽭����ר�ò��������**********************/

/****************************************************************
***ȫ�ֺ�������
****************************************************************/

#define MAX_EQUIP_TYPE_NUM             9           //�豸�����������
#define MAX_EQUIP_GROUP_NUM            2           //һ���豸�������
#define MAX_EQUIP_COUNT_NUM            10           //һ���豸������

#define MAX_POINT_CALL_NUM             80           //���ٻ����������Ϣ�����
#define MAX_PARA_CALL_NUM              80           //���ֲ����ٻ����������Ϣ�����

//�ϱ��������
#define MAX_REPORT_TASK_NUM       43           //�ϱ����������Ŀ
#define MAX_INFO_ITEM_NUM         80           //�ϱ�������ϢԪ�������Ŀ
#define REPORT_TASK_YX             1           //�ϱ�����Ϊң������
#define REPORT_TASK_YC             2           //�ϱ�����Ϊң������

//�ϱ���������
#define TASK_DATA_NULL             0           //δ֪��������
#define TASK_DATA_REALTIME         1           //ʵʱ��������
#define TASK_DATA_FROZEN           2           //������������

//��Ϣ���ַ����
#define INFO_ADDR_TYPE_NULL           0           //δ֪��Ϣ������
#define INFO_ADDR_TYPE_YX             1           //ң�ŵ�ַ
#define INFO_ADDR_TYPE_YC             2           //ң���ַ
#define INFO_ADDR_TYPE_PARA           3           //������ֵ��ַ
#define INFO_ADDR_TYPE_FAULT          4           //�����¼�

//�������ݴ洢���
#define FROZEN_SAVE_DEPTH_MAX        50           //�������ݴ洢���

//��ֵ������
#define MAX_AREA_NUM_ZHJ             2            //��ֵ������

//�������ź��������(һά����)
#define EQGROUP (MAX_EQUIP_GROUP_NUM) //һ���豸�������(��Ϊ���������Ƴ���)
#define EQCOUNT (MAX_EQUIP_COUNT_NUM) //һ���豸������(��Ϊ���������Ƴ���)

//�������ź��������(һά����) ��(�ն�+���+������+©��)(���������*����������*�����͵���ź�������)
#define MAX_YX_NUM_ZHJ     (1*1* 20 + 1*1* 60 + EQGROUP*EQCOUNT*20 + EQGROUP*EQCOUNT*20)  //�ն�ң������
#define MAX_YC_NUM_ZHJ     (1*1* 60 + 1*1*800 + EQGROUP*EQCOUNT*60 + EQGROUP*EQCOUNT*80)  //ң����������
#define FAULT_MAX_NUM_ZHJ  (1*1* 10 + 1*1* 80 + EQGROUP*EQCOUNT*10 + EQGROUP*EQCOUNT*10)  //������Ϣ������
#define IDEN_MAX_NUM_ZHJ   (1*1*  1 + 1*1*  1 + EQGROUP*EQCOUNT* 1 + EQGROUP*EQCOUNT* 1)  //��ֵ��������ʶ������
#define PARA_MAX_NUM_ZHJ   (1*1*200 + 1*1*100 + EQGROUP*EQCOUNT*50 + EQGROUP*EQCOUNT*50)  //��������������

//����ϱ������������(һά����) ��(ֻ���ն�)(���������*����������*���豸����ϱ���������)
#define TASK_MAX_NUM_ZHJ   (1*1*MAX_REPORT_TASK_NUM)  //��������������

//�ɼ��豸�����������(һά����) ��(������+©��)(���������*����������*���豸����ϱ���������)
#define FILE_MAX_NUM_ZHJ   (EQGROUP*EQCOUNT*1 + EQGROUP*EQCOUNT*1)  //ÿ���豸Ĭ��ֻ��һ������

//�����ź����⴦����
//ң��
#define YX_SP_START              (0x10000000)           //ң�����⴦��ʼ
#define YX_SP_END                (0x1FFFFFFF)           //ң�����⴦��ʼ

//����
#define EN_SP_START              (0x20000000)           //�������⴦��ʼ
#define EN_SP_END                (0x2FFFFFFF)           //�������⴦��ʼ

//ң��
#define YC_SP_START              (0x30000000)           //ң�����⴦��ʼ

#define YC_SP_PRE_DAY_START          (0x30000001)            //------��ͳ�ƿ�ʼ------
#define YC_SP_PRE_DAY_MAX            (0x30000002)            //���յ����ֵ����ֵ
#define YC_SP_PRE_DAY_MIN            (0x30000003)            //���յ���Сֵ����ֵ
#define YC_SP_PRE_DAY_MT_TIME        (0x30000004)            //���ʱ��
#define YC_SP_PRE_DAY_QR_TIME        (0x30000005)            //�ϸ�ʱ��
#define YC_SP_PRE_DAY_UP_TIME        (0x30000006)            //������ʱ��
#define YC_SP_PRE_DAY_DW_TIME        (0x30000007)            //������ʱ��
#define YC_SP_PRE_DAY_QT_RATE        (0x30000008)            //�ϸ���
#define YC_SP_PRE_DAY_UP_RATE        (0x30000009)            //��������
#define YC_SP_PRE_DAY_DW_RATE        (0x3000000A)            //��������
#define YC_SP_PRE_DAY_MT_TOTAL       (0x3000000B)            //�ۼӺ�
#define YC_SP_PRE_DAY_MT_AVG         (0x3000000C)            //ƽ��ֵ
#define YC_SP_PRE_DAY_PF_CAL         (0x3000000D)            //���ι�������ͳ��ʱ��
#define YC_SP_PRE_DAY_DEMAND         (0x3000000E)            //����ͳ��
#define YC_SP_PRE_DAY_END            (0x3000FFFF)            //------��ͳ�ƽ���------

#define YC_SP_PRE_MON_START          (0x30010001)            //------��ͳ�ƿ�ʼ------
#define YC_SP_PRE_MON_MAX            (0x30010002)            //���µ����ֵ����ֵ
#define YC_SP_PRE_MON_MIN            (0x30010003)            //���µ���Сֵ����ֵ
#define YC_SP_PRE_MON_MT_TIME        (0x30010004)            //���ʱ��
#define YC_SP_PRE_MON_QR_TIME        (0x30010005)            //�ϸ�ʱ��
#define YC_SP_PRE_MON_UP_TIME        (0x30010006)            //������ʱ��
#define YC_SP_PRE_MON_DW_TIME        (0x30010007)            //������ʱ��
#define YC_SP_PRE_MON_QT_RATE        (0x30010008)            //�ϸ���
#define YC_SP_PRE_MON_UP_RATE        (0x30010009)            //��������
#define YC_SP_PRE_MON_DW_RATE        (0x3001000A)            //��������
#define YC_SP_PRE_MON_MT_TOTAL       (0x3001000B)            //�ۼӺ�
#define YC_SP_PRE_MON_MT_AVG         (0x3001000C)            //ƽ��ֵ
#define YC_SP_PRE_MON_PF_CAL         (0x3001000D)            //���ι�������ͳ��ʱ��
#define YC_SP_PRE_MON_DEMAND         (0x3001000E)            //����ͳ��
#define YC_SP_PRE_MON_END            (0x3001FFFF)            //------��ͳ�ƽ���------

#define YC_SP_RT_START              (0x30020001)            //------ʵʱͳ�ƿ�ʼ------
#define YC_SP_RT_TMN_WORK_TIME      (0x30020002)            //�ն��ۼ�����ʱ��
#define YC_SP_RT_VOLT_SPLIT         (0x30020003)            //��ѹ���򡢸����������
#define YC_SP_RT_CURR_SPLIT         (0x30020004)            //�������򡢸����������
#define YC_SP_RT_VOLT_DIFF          (0x30020005)            //ABC�����ѹƫ��
#define YC_SP_RT_FR_DIFF            (0x30020006)            //�ܼ�ABC����Ƶ��ƫ��
#define YC_SP_RT_VOLT_IMBALA        (0x30020007)            //��ѹ��ƽ���
#define YC_SP_RT_CURR_IMBALA        (0x30020008)            //������ƽ���
#define YC_SP_RT_LOAD_RATE          (0x30020009)            //������
#define YC_SP_RT_U_HMONIC_ODD       (0x3002000A)            //ABC����г����ѹ���г��������
#define YC_SP_RT_U_HMONIC_EVEN      (0x3002000B)            //ABC����г����ѹż��г��������
#define YC_SP_RT_I_ANGLE            (0x3002000C)            //ABC����������
#define YC_SP_RT_END                (0x3002FFFF)            //------ʵʱͳ�ƽ���------

#define YC_SP_HARMONIC_START        (0x30030001)            //------г����ؿ�ʼ------
#define YC_SP_HARMONIC_RT_GET       (0x30030002)            //г��ʵʱ��ȡ
#define YC_SP_HARMONIC_END          (0x3003FFFF)            //------г����ؽ���------

#define YC_SP_DEMAND_START          (0x30040001)            //------������ؿ�ʼ------
#define YC_SP_DEMAND_RT_GET         (0x30040002)            //����ʵʱ��ȡ
#define YC_SP_DEMAND_END            (0x3004FFFF)            //------������ؽ���------

#define YC_SP_ON_DAY_START          (0x30050001)            //------����ͳ�ƿ�ʼ------
#define YC_SP_ON_DAY_MAX            (0x30050002)            //���յ����ֵ����ֵ
#define YC_SP_ON_DAY_MIN            (0x30050003)            //���յ���Сֵ����ֵ
#define YC_SP_ON_DAY_MT_TIME        (0x30050004)            //���ʱ��
#define YC_SP_ON_DAY_QR_TIME        (0x30050005)            //�ϸ�ʱ��
#define YC_SP_ON_DAY_UP_TIME        (0x30050006)            //������ʱ��
#define YC_SP_ON_DAY_DW_TIME        (0x30050007)            //������ʱ��
#define YC_SP_ON_DAY_QT_RATE        (0x30050008)            //�ϸ���
#define YC_SP_ON_DAY_UP_RATE        (0x30050009)            //��������
#define YC_SP_ON_DAY_DW_RATE        (0x3005000A)            //��������
#define YC_SP_ON_DAY_MT_TOTAL       (0x3005000B)            //�ۼӺ�
#define YC_SP_ON_DAY_MT_AVG         (0x3005000C)            //ƽ��ֵ
#define YC_SP_ON_DAY_PF_CAL         (0x3005000D)            //���ι�������ͳ��ʱ��
#define YC_SP_ON_DAY_DEMAND         (0x3005000E)            //����ͳ��
#define YC_SP_ON_DAY_END            (0x3005FFFF)            //------����ͳ�ƽ���------

#define YC_SP_ON_MON_START          (0x30060001)            //------����ͳ�ƿ�ʼ------
#define YC_SP_ON_MON_MAX            (0x30060002)            //���µ����ֵ����ֵ
#define YC_SP_ON_MON_MIN            (0x30060003)            //���µ���Сֵ����ֵ
#define YC_SP_ON_MON_MT_TIME        (0x30060004)            //���ʱ��
#define YC_SP_ON_MON_QR_TIME        (0x30060005)            //�ϸ�ʱ��
#define YC_SP_ON_MON_UP_TIME        (0x30060006)            //������ʱ��
#define YC_SP_ON_MON_DW_TIME        (0x30060007)            //������ʱ��
#define YC_SP_ON_MON_QT_RATE        (0x30060008)            //�ϸ���
#define YC_SP_ON_MON_UP_RATE        (0x30060009)            //��������
#define YC_SP_ON_MON_DW_RATE        (0x3006000A)            //��������
#define YC_SP_ON_MON_MT_TOTAL       (0x3006000B)            //�ۼӺ�
#define YC_SP_ON_MON_MT_AVG         (0x3006000C)            //ƽ��ֵ
#define YC_SP_ON_MON_PF_CAL         (0x3006000D)            //���ι�������ͳ��ʱ��
#define YC_SP_ON_MON_DEMAND         (0x3006000E)            //����ͳ��
#define YC_SP_ON_MON_END            (0x3006FFFF)            //------����ͳ�ƽ���------


#define YC_SP_END                   (0x3FFFFFFF)           //ң�����⴦�����
#define YC_SP_INVALID               (0x40000000)           //��������


typedef enum
{   
    E_EQUIP_FIRST = 1,
    E_EQUIP_POWERTERMINAL = 1,       //�ն� 1
    E_EQUIP_TRANSFORMER,         //����ѹ�� 2
    E_EQUIP_SVG,                 //SVG 3
    E_EQUIP_CAPACITOR,           //���ܵ����� 4
    E_EQUIP_COMMUTATIONSWITCH,   //���࿪�� 5
    E_EQUIP_RESIDENTMETER,       //�û��� 6
    E_EQUIP_CHARGINGPILE,        //���׮ 7
    E_EQUIP_PHOTOVOLTAIC,        //��� 8
    E_EQUIP_RESIDUALCURRENT,     //ʣ��������������� 9
    E_EQUIP_INTELLIGENTSWITCH,   //���ܿ��� 10
    E_EQUIP_LOWVOLTSHUNT,        //���ܵ�ѹ��·����豸 11
    E_EQUIP_LOWVOLTFAULT,        //���ܵ�ѹ���ϴ����� 12
    E_EQUIP_MEACTRLMETER,        //�๦�ܲ�ر� 13
    E_EQUIP_STATIONMONITOR,      //վ����������ն� 14
    E_EQUIP_LAST = E_EQUIP_STATIONMONITOR,
}PW_EQUIP_TYPE_E;

//ң��������Ϣ�ṹ��
typedef struct
{
    uint16 nEqType;  //�豸���� ��1��ʼ
    uint16 nEqGroup;  //�豸��� ��0��ʼ
    uint16 nEquipNo;  //�豸������� ��1��ʼ
    uint8  nObjAddrHB;    //��Ϣ�����ַ������ֽڣ�
	uint16 nObjAddr;      //��Ϣ�����ַ��������ʼ��ַ��ʵ��ֵ��
	uint32 nObjAddrU32;   //��Ϣ�����ַ������3���ֽڣ�
	char   *szObjName;    //��Ϣ��������
	uint32 nSignalType;   //���ͱ�ʶ
	uint32 nDbCenterOAD;  //�������Ĳ�ѯ��ǩ��OAD��
	uint32 nOadColsNum;  //������OAD����
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //������OAD
	uint8  nItemIndex;    //������ţ���0��ʼ��
	uint32 nDealType;  //���⴦������(��ֱ�Ӵ��������Ļ�ȡ, �˹�����)
}YX_DATA_CONFIG_ZHJ;

//ң��������Ϣ�ṹ��
typedef struct
{
    uint16 nEqType;  //�豸���� ��1��ʼ
    uint16 nEqGroup;  //�豸��� ��0��ʼ
    uint16 nEquipNo;  //�豸������� ��1��ʼ
    uint8  nObjAddrHB;    //��Ϣ�����ַ������ֽڣ�
	uint16 nObjAddr;      //��Ϣ�����ַ��������ʼ��ַ��ʵ��ֵ��
	uint32 nObjAddrU32;   //��Ϣ�����ַ������3���ֽڣ�
	char   *szObjName;    //��Ϣ��������
	uint32 nSignalType;   //���ͱ�ʶ
	int8 nReportRatio;    //��ǰ����������վ�ı���
	uint16 nDeadZoneAddr; //�仯����ֵ�Ķ����ַ
	float32 fDeadZoneValue; //�仯����ֵ
	uint8 nLogicId; //�߼���ַ�����ɣ�1�����0
    uint16 nInfoNum; //��Ϣ���  �����ɣ�0�������1��ʼ������Ϣ���Ϊ0xFFFFʱ����ʾȫ����Ϣ��
	uint32 nDbCenterOAD;  //�������Ĳ�ѯ��ǩ����OAD��
	uint32 nOadColsNum;  //������OAD����
	uint32 aOadCols[MAX_OAD_COLS_NUM];  //������OAD
	uint8  nItemIndex;    //������ţ���0��ʼ��
	float32 fFullValue;  //����ֵ�����ڹ�һ��ң��ֵ��
	uint32 nDealType;  //���⴦������(��ֱ�Ӵ��������Ļ�ȡ, �˹�����)
	uint8 nArrLen;       //���账��������������(���ش�)
}YC_DATA_CONFIG_ZHJ;

//������Ϣ�ϱ��������ݽṹ���ϱ�����ʱ��ĵ���ң���ϱ�
typedef struct 
{
    uint16 nEqType;  //�豸���� ��1��ʼ
    uint16 nEqGroup;  //�豸��� ��0��ʼ
    uint16 nEquipNo;  //�豸������� ��1��ʼ
    uint8  nObjAddrHB;    //��Ϣ�����ַ������ֽڣ�
	uint16 nObjAddr;      //��Ϣ�����ַ��������ʼ��ַ��ʵ��ֵ��
	uint32 nObjAddrU32;   //��Ϣ�����ַ������3���ֽڣ�
	char   *szObjName;    //��Ϣ��������
	uint8  nEuipType;     //�豸���ͣ�����ȷ��ң�ŵ�������
	uint32 nSignalType;   //���ͱ�ʶ��������Ϣ��
	uint32 nFaultYxAddr;  //����ң�ŵ�ַ(�����¼����ж��߼�)
	uint8  nRptDataSrc;    //�ϱ�������Դ�������ĸ��豸��
	uint8  nReportNum;      //�ϱ����ݸ���(�ܸ���)
	uint8  nReportYxNum;      //�ϱ����ݸ���(ң�Ÿ���)
	uint8  nReportYcNum;      //�ϱ����ݸ���(����ʱ�� ң�����)
	uint8  nReportYcTiNum;      //�ϱ����ݸ���(��ʱ�� ң�����)
	uint8  nReportEnNum;      //�ϱ����ݸ���(����������)
	uint8  nReportStrNum;      //�ϱ����ݸ���(���ش�����)
	uint8  nReportParaNum;      //�ϱ����ݸ���(�������� �����ڲ�������¼�)
	uint16 aReportAddr[MAX_FAULT_SIGNAL_NUM]; //�ϱ���Ϣ���ַ(ȡ�����Ϣ���ַ�Ĵ��λ��)
	uint32 aReportAddrU32[MAX_FAULT_SIGNAL_NUM]; //�ϱ���Ϣ���ַ(ȡ�����Ϣ���ַ�Ĵ��λ��)
	uint32 nDbCenterOAD;  //�������Ĳ�ѯ��ǩ��OAD��
	uint8  nItemIndex;    //������ţ���0��ʼ��
}FAULT_REPORT_CONFIG_ZHJ;

//��ֵ��������ʶ�ṹ��
typedef struct
{
    uint16 nEqType;  //�豸���� ��1��ʼ
    uint16 nEqGroup;  //�豸��� ��0��ʼ
    uint16 nEquipNo;  //�豸������� ��1��ʼ
    uint16 nFixedAreaID;  //��ֵ����SN
	PARA_IDENTIFY_UN unParaIdentify;  //����������ʶ
	uint8  nSetState;  //0x55��Ԥ��, 0xaa�ѹ̻�/����
	uint8  aSetPara[255];  //�������ñ���(���ֽ�Ϊ��־0x55/0xaa, ���ֽ�Ϊ����, �����ַ+tag+����+ֵ)
}__attribute__ ((packed)) FIXED_AREA_IDEN_ZHJ;

//��ֵ�����趨�ṹ�壨��Ҫ���ڱ��ģ�
typedef struct
{
    uint16 nEqType;  //�豸���� ��1��ʼ
    uint16 nEqGroup;  //�豸��� ��0��ʼ
    uint16 nEquipNo;  //�豸������� ��1��ʼ
    BOOL bModifiable;     //���޸ĵ�
    uint8  nObjAddrHB;    //��Ϣ�����ַ������ֽڣ�
	uint16 nObjAddr;      //��Ϣ�����ַ
	uint32 nObjAddrU32;   //��Ϣ�����ַ������3���ֽڣ�
	uint8  res;           //��Ϣ�����ַ�����ֽڱ�����
	uint8  nTagType;      //Tag����
	uint8  nDataLen;      //���ݳ���
	VALUE_TPYE_UN unValue;//����value
	BOOL   bValid;        //����value�Ƿ���Ч
	float  fDefaultValue; //Ĭ��ֵ
	int64  nDefaultValueI64; //Ĭ��ֵ(����)
	float  fMinValue;     //��Сֵ
	float  fMaxValue;     //���ֵ
	uint32 nDealType;     //���⴦������(��ֱ�Ӵ��������Ļ�ȡ, �˹�����)
	uint8  nUpdateFlag;   //�Ƿ�������е�˽���ļ�����
	uint16 nUpdateArea;   //���µĶ�ֵ���Ŷ�Ӧ����
	uint16 nArrLen;       //���账��������������
	uint8  aArrValue[MAX_ARR_VALUE_LEN];     //���账����������
}PARA_SET_VALUE_ZHJ;

//�ϱ��������ýṹ��
typedef struct
{
    uint16 nEqType;  //�豸���� ��1��ʼ
    uint16 nEqGroup;  //�豸��� ��0��ʼ
    uint16 nEquipNo;  //�豸������� ��1��ʼ
    uint8 nTaskNo;  //����� ��1��ʼ
    uint8 bIsValid;  //������Ч��־ 0��Ч 1��Ч
    uint8 bIsCorrect;  //�������в���������ȷ 0�����쳣���� 1���в�����ȷ
    uint8 nReportDataType;  //�ϱ��������� 1ʵʱ 2����
    uint8 nReportTaskType;  //�ϱ��������� 1ң�� 2ң��
	uint8 aFrozenBaseTime[7];  //�����׼ʱ�䣬CP56Time2a
	time_t tFrozenBaseTime;  //�����׼ʱ�䣬��
	OOP_DATETIME_S_T stFrozenBaseTime;
	//uint32 nFrozenPeriod;  //�������ڣ���λ����
	uint8 nFrozenPeriodValue;  //�������ڣ���ֵ
	uint8 nFrozenPeriodUnit;  //�������ڵ�λ��0x02 ��; 0x03 Сʱ; 0x04 ��; 0x05 ��; 
	uint8 aReportBaseTime[7];  //���ͻ�׼ʱ�䣬CP56Time2a
	time_t tReportBaseTime;  //���ͻ�׼ʱ�䣬��
	OOP_DATETIME_S_T stReportBaseTime;
	//uint32 nReportPeriod;  //�������ڣ���λ����
	uint8 nReportPeriodValue;  //�������ڣ���ֵ
	uint8 nReportPeriodUnit;  //�������ڵ�λ��0x02 ��; 0x03 Сʱ; 0x04 ��; 0x05 ��; 
	uint8 nSelectRatio;  //���ݳ�ȡ����
	uint8 nInfoItemNum;  // ��ϢԪ��������NN ������Ϊ7���豸��ſ���Ϊ255��
	uint8 nRealInfoItemNum;  // ʵ����ϢԪ��������������ַΪ�����ĳ���豸��
	uint16 aPublicAddr[MAX_INFO_ITEM_NUM];  //������ַ
	uint16 aEquipType[MAX_INFO_ITEM_NUM];  //�豸����
	uint16 aEquipGroup[MAX_INFO_ITEM_NUM];  //�豸���
	uint16 aEquipNo[MAX_INFO_ITEM_NUM];  //�豸���
	uint32 aInfoItemAddr[MAX_INFO_ITEM_NUM];  //��Ϣ���ַ
	uint16 aSignalType[MAX_INFO_ITEM_NUM];  //��Ϣ���ݱ�ʶ
}REPORT_TASK_CONFIG_ZHJ;

//�ϱ��������ݽṹ��
typedef struct
{
    BOOL  bValidFlag[MAX_INFO_ITEM_NUM];  //������Ч�Ա�־
	uint8 aYxFrozenValue[MAX_INFO_ITEM_NUM];  //ң�Ŷ�������
	float32 aYcFrozenValue[MAX_INFO_ITEM_NUM];  //ң�ⶳ������
	uint8 nFrozenArrLen[MAX_INFO_ITEM_NUM];  //������ش�����
    uint8 aFrozenArrValue[MAX_INFO_ITEM_NUM][MAX_YC_STRING_LEN];  //������ش�����
	uint8 aFrozenTime[7];  //����ʱ�䣬CP56Time2a
	time_t tFrozenTime;  //����ʱ�䣬��
	OOP_DATETIME_S_T stFrozenTime;
    uint8 aReportTime[7];  //�ϱ�ʱ�䣬CP56Time2a
	time_t tReportTime;  //�ϱ�ʱ�䣬��
	OOP_DATETIME_S_T stReportTime;
}REPORT_TASK_DATA_ZHJ;

//�����ٲ�ṹ��
typedef struct
{
    uint8 nTaskNo;  //������
    uint8 aStartTime[7];  //������ʼʱ�䣬CP56Time2a
	time_t tStartTime;  //������ʼʱ�䣬��
	OOP_DATETIME_S_T stStartTime;
    uint8 aEndTime[7];  //���ݽ���ʱ�䣬CP56Time2a
	time_t tEndTime;  //���ݽ���ʱ�䣬��
	OOP_DATETIME_S_T stEndTime;
}TASK_CALL_CONFIG_ZHJ;

//�ɼ��豸����
typedef struct
{
    uint16 nEqType;  //�豸���� ��1��ʼ
    uint16 nEqGroup;  //�豸��� ��0��ʼ
    uint16 nEquipNo;  //�豸������� ��1��ʼ
    OOP_METER_T stMeterFile;  //�ɼ��豸������Ϣ
}OOP_METER_FILE_ZHJ;


//��������˽���ļ���
extern char *g_pFixedAreaPathZhj;  //��ֵ������˽���ļ�
extern char *g_pParamValuePathZhj;  //��ֵ����˽���ļ�
extern char *g_pFrozenValuePathZhj;  //��ʷ��������˽���ļ�

extern uint16 g_aGroupCountZhj[MAX_EQUIP_TYPE_NUM];  //��ʾ��ͬ�����豸ʵ������
extern uint16 g_aEquipCountZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM];  //��ʾ��ͬ�����豸ʵ������

//ң�ŵ��ʵ�����ø���
extern uint16 g_nConfigYxNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern  uint32 g_nTotalYxNumZhj;//����ң�ŵ������֮��

//ң����ʵ�����ø���
extern uint16 g_nConfigYcNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint32 g_nTotalYcNumZhj;//����ң��������֮��

//�������ʵ�����ø���
extern uint16 g_nConfigParaNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint32 g_nTotalParaNumZhj;//���в����������֮��

//�¼����ʵ�����ø���
extern uint16 g_nConfigFaultNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint32 g_nTotalFaultNumZhj;//���й��ϵ������֮��

//ң�ŵ������
extern YX_DATA_CONFIG_ZHJ g_aYxDataConfigZhj[MAX_YX_NUM_ZHJ];    //ң���ź�������
extern YX_DATA_SEND   g_aYxDataSendZhj[MAX_YX_NUM_ZHJ];  //ң���źŴ洢��
extern uint32         g_aYxDataIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];  //�ź�����

//ң��������
extern YC_DATA_CONFIG_ZHJ g_aYcDataConfigZhj[MAX_YC_NUM_ZHJ];  //ң������������
extern YC_DATA_SEND   g_aYcDataSendZhj[MAX_YC_NUM_ZHJ];  //ң�����ݴ洢��
extern uint32         g_aYcDataIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];  //�ź�����
extern uint32         g_nYcStatVoltDay;  //�յ�ѹ���ͳ����ʼ��
extern uint32         g_nYcStatHarmonyDay;  //�յ�ѹг��ͳ����ʼ��
extern uint32         g_nYcStatVoltUnbalanceDay;  //�յ�ѹ��ƽ���ͳ����ʼ��
extern uint32         g_nYcVoltUnbalance;  //��ѹ��ƽ���ʵʱֵ��ʼ��
extern uint32         g_nYcStatCurrUnbalanceDay;  //�յ�����ƽ���ͳ����ʼ��
extern uint32         g_nYcStatFrequencyDay;  //��Ƶ��ͳ����ʼ��
extern uint32         g_nYcStatExtremeDay; //��ֵͳ����ʼ��
extern uint32         g_nYcStatPwrFactDay; //��������ͳ����ʼ��


//�¼��������
extern FAULT_REPORT_CONFIG_ZHJ g_aFaultDataConfigZhj[FAULT_MAX_NUM_ZHJ];  //������Ϣ������
extern uint32         g_aFaultDataIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];  //�ź�����

//��ֵ������仯��־
extern uint16 g_nFixedAreaIdZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];    //��ֵ����
extern uint8 g_bFixedChangeFlagZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_FIXED_AREA_NUM];    //��ֵ�����ı��־����Ҫͬ������������

//��ֵ��������ʶ�ṹ���������
extern uint16 g_aFixedAreaNumZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//��ֵ������
extern FIXED_AREA_IDEN_ZHJ g_aFixedValueAreaZhj[MAX_AREA_NUM_ZHJ][IDEN_MAX_NUM_ZHJ];//��ֵ��������ʶ
extern uint32 g_aFixedAreaIndexZhj[MAX_AREA_NUM_ZHJ][MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//�ź�����

//��ֵ��������
extern PARA_SET_VALUE_ZHJ g_aParaValueInitZhj[PARA_MAX_NUM_ZHJ];//������
extern uint32             g_aParaValueIndexZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//�ź�����
extern PARA_SET_VALUE_ZHJ g_aParaSetValueZhj[MAX_AREA_NUM_ZHJ][PARA_MAX_NUM_ZHJ];//˽���ļ��������

//д��ֵ����ʱ�����Ĺ�����ַ
extern uint16 g_nFixedValuePubAddrZhj;   //д��ֵ�����Ĺ�����ַ	
extern uint8 g_aFixedValueStateZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];

//�����̻���־
extern uint8  g_nParaFixedFlagZhj[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_FIXED_AREA_NUM];//�����̻���־

//�ն��ϱ���������
extern REPORT_TASK_CONFIG_ZHJ g_aReportTaskConfig[TASK_MAX_NUM_ZHJ];//����
extern REPORT_TASK_DATA_ZHJ   g_aReportTaskData[TASK_MAX_NUM_ZHJ];//�����ڻ�ʵʱ��������
extern uint32                 g_aReportTaskIndex[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//�ź�����

//�ն��ϱ�����Ĺ�����ַ(ͳ�ƹ��м��ֲ�ͬ�Ĺ�����ַ)
extern uint16 g_aReportTaskPubAddrNum[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];
extern uint16 g_aReportTaskPubAddr[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM][MAX_INFO_ITEM_NUM];
extern uint8 g_aReportTaskPubAddrState[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM][MAX_INFO_ITEM_NUM];
    
//�������ݴ洢(��ʷ����)
extern uint16 g_aHisFrozenDataNum[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//����

//�豸��������
extern OOP_METER_FILE_ZHJ g_aAllEquipFiles[FILE_MAX_NUM_ZHJ];
extern uint32             g_aEquipFilesIndex[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//�ź�����

//��ͨ�ɼ�������¼��ѡ�����
extern uint16 g_aSchemeColOADNum[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM];//����OAD�ľ������
extern uint32 g_aSchemeColOAD[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_YC_NUM];//������Ϊÿ��ң�����Ӧһ��oad

//���ٻ������Ĺ�����ַ(�豸����+�豸���)	
extern uint16 g_nAllCallPubAddrZhj;   //���ٻ������Ĺ�����ַ(�豸����+�豸���)	

//���ٻ��豸�������豸��š��豸���
extern uint8 g_aZeroNull[MAX_EQUIP_COUNT_NUM];
extern uint8 g_aAllCallEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint8 g_aAllCallEquipStateYX[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��
extern uint8 g_aAllCallEquipStateYC_R[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(������)
extern uint8 g_aAllCallEquipStateYC_R_T[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(������)
extern uint8 g_aAllCallEquipStateYC_STR[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(�ַ���)

//�����ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)	
extern uint16 g_nEnergyCallPubAddrZhj;   //���ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)	
extern uint8 g_aEnergyCallEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];


//���ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)	
extern uint16 g_nPointCallPubAddrZhj;   //���ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)
extern uint8 g_nPointCallInfoAddrNum;//���ٻ���ַ����
extern uint32 g_aPointCallInfoAddr[MAX_POINT_CALL_NUM];//���ٻ���ַ����
extern uint8 g_aPointCallEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];
extern uint8 g_aPointCallEquipStateYX[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��
extern uint8 g_aPointCallEquipStateYC_R[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(������)
extern uint8 g_aPointCallEquipStateYC_R_T[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(������)
extern uint8 g_aPointCallEquipStateYC_R_EN[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(������)
extern uint8 g_aPointCallEquipStateYC_STR[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(�ַ���)

//�����ٻ��豸�������豸���
extern uint16 g_nTaskCallPubAddrZhj;   //�����ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)
extern TASK_CALL_CONFIG_ZHJ g_stTaskCallConfig;//�����ٻ�����ֹʱ��
extern uint8 g_aTaskCallEquipState[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];
extern uint8 g_aTaskCallEquipStateYX[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//δʹ��
extern uint8 g_aTaskCallEquipStateYC_R[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//δʹ��
extern uint8 g_aTaskCallEquipStateYC_STR[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//δʹ��
extern uint8 g_aTaskCallPubAddrState[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM][MAX_INFO_ITEM_NUM];

//���ݱ仯�����ϱ��豸�������豸���
extern uint8 g_aChangedEquipStateYX[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��
extern uint8 g_aChangedEquipStateYC_R[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(������)
extern uint8 g_aChangedEquipStateYC_R_T[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(������)
extern uint8 g_aChangedEquipStateYC_R_EN[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(������)
extern uint8 g_aChangedEquipStateYC_STR[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];//ң��(�ַ���)

//�����ϱ��豸�������豸���
extern uint8 g_aReportTaskState[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];
extern uint8 g_aReportTaskStateYX[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//δʹ��
extern uint8 g_aReportTaskStateYC_R[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//δʹ��
extern uint8 g_aReportTaskStateYC_STR[MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM][MAX_REPORT_TASK_NUM];//δʹ��

//���в����ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)	
extern uint16 g_nAllParaCallPubAddrZhj;   //���в����ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)	
extern uint16 g_nAllParaCallAreaIdZhj;   //���в����ٻ���ֵ����
extern uint8 g_aAllParaEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];

//���ֲ����ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)	
extern uint16 g_nSomeParaCallPubAddrZhj;   //���ֲ����ٻ������Ĺ�����ַ(�豸����+�豸���+�豸���)
extern uint16 g_nSomeParaCallAreaIdZhj;   //���ֲ����ٻ���ֵ����
extern uint8 g_nSomeParaCallInfoAddrNum;//���ֲ����ٻ���ַ����
extern uint32 g_aSomeParaCallInfoAddr[MAX_PARA_CALL_NUM];//���ֲ����ٻ���ַ����
extern uint8 g_aSomeParaCallEquipState[MAX_EQUIP_TYPE_NUM][MAX_EQUIP_GROUP_NUM][MAX_EQUIP_COUNT_NUM];

//ʱ��ͬ��������ַ(�豸����+�豸���+�豸���)	
extern uint16 g_nTimeSynPubAddrZhj;   //ʱ��ͬ�������Ĺ�����ַ(�豸����+�豸���+�豸���)

//104����ִ������ȫ�ֱ���(���ڶ������ϱ�ר��)
extern TASK_RUN_STEP_T g_sTimingTaskRunStep;

//����
extern int get_next_equip_attr_zhj(uint16 curTyIndex, uint16 curGrIndex, uint16 curSeqIndex, uint16 *nexTyIndex, uint16 *nexGrIndex, uint16 *nexSeqIndex);
extern int scheme_coload_init_zhj();
extern int query_yx_type_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr);
extern int query_yx_index_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr);
extern int query_yc_type_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr);
extern int query_yc_index_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr);
extern int query_para_index_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint16 areaIndex, uint32 InfoAddr);
extern int timing_task_create_zhj();
extern int scheme_coload_statistics_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint32 oad);
extern int scheme_coload_save_zhj();
extern int equip_files_create_zhj(uint16 equipType, uint16 equipGroup, uint16 equipNo, OOP_METER_T *newOopMeter);
extern int meter_data_clear_zhj(DB_CLIENT fd, uint16 infoNum);
extern int equip_files_save_zhj();
extern int clear_all_scheme_zhj(DB_CLIENT fd);;
extern int equip_scheme_save_zhj();
extern int clear_all_task_zhj(DB_CLIENT fd);
extern int equip_task_save_zhj();
extern int i_frame_data_update_zhj(TOTAL_104_FRAME_T stTotal104Frame, uint8 *pOutBuf);
extern int i_frame_data_init_end_zhj(uint8 *pOutBuf);
extern int i_frame_data_pack_zhj(uint8 nTI, uint8 nVSQ, uint16 nCOT, uint16 nPubAddr, uint8 *pInObjBuff, uint8 nInObjLen, uint8 *pOutBuf);
extern int check_call_all_addr_zhj(uint16 PubAddr);
extern int check_call_point_addr_zhj(uint16 PubAddr);
extern int check_call_point_info_zhj(uint8* pInfo, uint8 nInfoLen);
extern int check_call_task_addr_zhj(uint16 PubAddr, uint8* pInfo, uint8 nInfoLen);
extern int move_to_next_call_task_zhj(uint8 nType);
extern int check_para_oper_addr_zhj(uint16 PubAddr);
extern int para_data_update_zhj(uint16 nEqTypeIndex, uint16 nEqGroupIndex, uint16 nEqIdIndex, uint16 nFixArea, PARA_SET_VALUE_T sParaSetValue);
extern uint8 para_set_operate_zhj(uint8 infonum, uint8 *pdata, uint8 infolen, uint8 btype);
extern int i_frame_data_unpack_zhj(uint16 nFrameLen, uint8 *pFrameBuff, uint8 *pOutBuf, uint8 *pAppType);
extern int report_task_create_zhj();
extern int get_next_equip_yx_zhj();
extern int get_next_equip_en_zhj();
extern int get_next_equip_all_para_zhj();
extern int get_next_equip_some_para_zhj();
extern int get_next_equip_yc_zhj(uint8 yctype);
extern int get_next_point_equip_yx_zhj();
extern int get_next_point_equip_yc_zhj(uint8 yctype);
extern uint8 yx_total_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 bStartNum,uint8 *pGetNum);
extern uint8 yc_total_nva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_sva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_total_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int yc_total_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern uint8 en_total_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int en_total_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern uint8 yx_point_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 bStartNum,uint8 *pGetNum);
extern uint8 yc_point_nva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_sva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_r_en_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_point_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int yc_point_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern int changed_yx_left_zhj();
extern int changed_yc_left_zhj(uint8 yctype);
extern int next_changed_yx_eq_zhj();
extern int next_changed_yc_eq_zhj(uint8 yctype);
extern uint8 yx_change_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_nva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_sva_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_r_en_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_change_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int move_next_task_call_pub_zhj();
extern int yc_change_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern int yx_frozen_call_get_zhj(uint8 bAdulen, uint8 *pData, uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_r_call_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_r_t_call_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_en_call_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_str_call_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_call_get_zhj(uint8 bAdulen, uint8 *pData, uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_refresh_frozen_call_zhj(uint8 *pData, uint8 *pCurrLen);
extern int next_call_all_equip_zhj(uint8 SigType);
extern int next_call_point_equip_zhj(uint8 SigType);
extern int report_task_run_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern int query_yx_by_detail_zhj(uint16 nPubAddr, uint32 nInfoAddr, uint8 *nInfoValue, BOOL *bValidFlag);
extern int query_yc_r_by_detail_zhj(uint16 nPubAddr, uint32 nInfoAddr, float32 *fInfoValue, BOOL *bValidFlag);
extern int query_yc_str_by_detail_zhj(uint16 nPubAddr, uint32 nInfoAddr, uint8 *nArrLen, uint8 *aArr, BOOL *bValidFlag);
extern int check_info_addr_type_zhj(uint32 nInfoAddr);
extern int query_yc_ratio_config_zhj(uint16 eqtypeIndex, uint16 eqgroupIndex, uint16 eqnoIndex, uint32 InfoAddr, int *ratio);
extern int check_frozen_time_zhj(uint16 nGroupIndex, uint16 nNoIndex, uint16 nTaskIndex, OOP_DATETIME_S_T stCurrentTime, time_t tCurrentTime);
extern int check_report_time_zhj(uint16 nGroupIndex, uint16 nNoIndex, uint16 nTaskIndex, OOP_DATETIME_S_T stCurrentTime, time_t tCurrentTime);
extern int save_frozen_data_zhj(uint16 nGroupIndex, uint16 nNoIndex, uint16 nTaskIndex, REPORT_TASK_DATA_ZHJ stFrozenData);
extern void timing_active_send_zhj();
extern int move_to_next_task_zhj(uint8 nType);
extern uint8 yx_timing_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_timing_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_timing_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_timing_en_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern uint8 yc_timing_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pGetNum);
extern int yc_timing_get_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);
extern int yx_frozen_get_zhj(uint8 bAdulen, uint8 *pData, uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_r_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_r_t_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_en_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_str_get_zhj(uint8 bAdulen, uint8 *pData ,uint16 *pStartNum,uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_frozen_get_zhj(uint8 bAdulen, uint8 *pData, uint8 *pCurrLen, uint8 nMaxLen);
extern int yc_refresh_frozen_zhj(uint8 *pData, uint8 *pCurrLen);
extern int timing_active_run_zhj(uint8 *pPipeIndex, uint8 *pReportBuf, uint8 *pAppType);



#ifdef __cplusplus
}
#endif

#endif // __PW_PROTOCOL_ZHJ_H__
