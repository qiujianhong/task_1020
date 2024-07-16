/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief�� ��������
* @date�� 2020-01-03
*********************************************************************
*/


#ifndef __PW_ALL_H__
#define __PW_ALL_H__


#include "baseCommon.h"
#include "dataCommon.h"
#include "dataType.h"
#include "errCode.h"
#include "macro.h"

#include "storage.h"
#include "db_api.h"
#include "task_id.h"

#include "app104_ipc.h"
#include "app104_base.h"
#include "app104_xml.h"
#include "comm_port.h"

#include "security.h"
#include "framecomm.h"
#include "task_msg.h"
#include "msg_id.h"


#include "../../msg/include/db_update.h"
#ifdef AREA_ZHEJIANG
#include "EventIO_zhejiang.h"
#else
#include "../../event/EventIO.h"
#endif
#include "monit_stat.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

//�ź�����
#define TEST_VER_FLAG (1) //�Ƿ���԰汾

/*******************************************************************************
* �����ⲿ����
*******************************************************************************/
extern uint8 advPd_get_rmt(RST_RMT_T *pRmt);
extern void yx_calculate_sta_set(uint32 subtype, uint8 subindex, uint8 value);
extern int para_query_float32_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, float32 *fOut);
extern int para_query_int64_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, int64 *nOut);
extern int para_query_array_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, uint8 *pOut);
extern int yx_event_signal_update_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nAddr, uint8 nNewState);

/*******************************************************************************
* ��Ҫͳ��/��������ݶ���
*******************************************************************************/

//�ź�����
#define BASE_CLASS_YC        	    0x40010000				//ң������
#define BASE_CLASS_YX        	    0x00010000				//ң������
#define BASE_CLASS_YK        	    0x60010000				//ң������
#define BASE_CLASS_EN        	    0x64010000				//��������
#define BASE_CLASS_FAULT        	0x01010000				//��������
#define BASE_CLASS_PARA        	    0x80010000				//��������

//ң��ͳ��/������
#define CALC_YC_A_VOLT_DVALUE        	    0x40010001				//A���ѹƫ��
#define CALC_YC_B_VOLT_DVALUE        	    0x40010002				//B���ѹƫ��
#define CALC_YC_C_VOLT_DVALUE        	    0x40010003				//C���ѹƫ��
#define CALC_YC_FR_DVALUE        	        0x40010004				//Ƶ��ƫ��
#define CALC_YC_PD_LOAD_RATE        	    0x40010005				//��为����

//ң��ͳ��/������
#define CALC_YX_REAL_SIGNAL        	    0x00010001				//����ң������ģ�鹲��8·Ӳң��
#define CALC_YX_VOLT_DOWN_L        	    0x00010002				//�ܽ��ߵ�ѹԽ����
#define CALC_YX_VOLT_OVER_H        	    0x00010003				//�ܽ��ߵ�ѹԽ����
#define CALC_YX_PHASE_LOSS        	    0x00010004				//����
#define CALC_YX_VOLT_LOSS        	    0x00010005				//ʧѹ
#define CALC_YX_POWER_FAILURE        	0x00010006				//ͣ��
#define CALC_YX_CURR_IMBALANCE        	0x00010007				//��ѹ���������಻ƽ��
#define CALC_YX_VOLT_IMBALANCE        	0x00010008				//��ѹ����ѹ���಻ƽ��
#define CALC_YX_TR_OVERLOAD        	    0x00010009				//��ѹ������
#define CALC_YX_TR_HEAVYLOAD        	0x0001000A				//��ѹ������
#define CALC_YX_S_OVER_LIMIT        	0x0001000B				//���ڹ���Խ��

//����ͳ��/������
#define CALC_EN_15MIN_FREEZE        	0x64010001				//15���Ӷ�������
#define CALC_EN_DAY_FREEZE        	    0x64010002				//�ն�������

//���в�������
#define CALC_PARA_TMN_TYPE        	0x80010001				//�ն�����
#define CALC_PARA_TMN_OPSYS        	0x80010002				//�ն˲���ϵͳ
#define CALC_PARA_TMN_MAKER        	0x80010003				//�ն�������
#define CALC_PARA_TMN_HARD        	0x80010004				//�ն�Ӳ���汾
#define CALC_PARA_TMN_SOFT        	0x80010005				//�ն�����汾
#define CALC_PARA_TMN_CHECK        	0x80010006				//�ն�����汾У����
#define CALC_PARA_TMN_PROTOCOL      0x80010007				//�ն�ͨ�Ź�Լ����
#define CALC_PARA_TMN_MODEL        	0x80010008				//�ն˳����ͺ�
#define CALC_PARA_TMN_ID        	0x80010009				//�ն�ID��
#define CALC_PARA_TMN_MAC        	0x8001000A				//�ն�����MAC��ַ

// �ṹ�������    
typedef enum
{   
    E_OOP_ENERGY_T=0,  
	E_OOP_ENERGYS_T,
	E_OOP_HENERGY_T,
    E_OOP_HENERGYS_T,    
    E_OOP_DEMAND_T,
    E_OOP_DEMANDS_T,
    E_OOP_VOLTQR_T,
    E_OOP_DWORD3V_T,
    E_OOP_WORD3V_T,
    E_OOP_INT3V_T,
    E_OOP_WORD4V_T,
    E_OOP_LONG4V_T,
    E_OOP_INT4V_T,
    E_OOP_METWORD_T,
    E_DATETIME_S,
    E_FRZ_DATETIME_S,
    E_REAL_DATETIME_S,
    E_OOP_WALLET_T,
    E_DOUBLE_LONG_UNSIGNED,
    E_DOUBLE_LONG,
    E_LONG_UNSIGNED,
    E_LONG,
    E_OOP_FROZENSETS_T,
    E_OOP_HARMONIC_T,
    E_ENUM,
    E_OOP_SYNCCLOCK_T,
    E_OOP_OCTETVAR16_T,
    E_OOP_GEOGADD_T,
    E_OOP_GRPADDR_T,
    E_OOP_PERIOD_T,
    E_UNSIGNED,
    E_OOP_PUBHOLIDAY_T,
    E_OOP_VISIBLESTR8_T,
    E_OOP_TIMEZONE_T,
    E_OOP_DAYPERIOD_T,
    E_OOP_RATETARIFF_T,
    E_OOP_STEPPRICE_T,
    E_OOP_LIMITCHARGE_T,
    E_OOP_OTHERLIMIT_T,
    E_OOP_POWERLIMIT_T,
    E_OOP_OTHERPOWERLIMIT_T,
    E_OOP_VISIBLESTR16_T,
    E_DAYPERIOD,
    E_TIMEZONE,
    E_OOP_PAYDAY_T,
    E_BIT_STRING,
    //��������0x49010200
	E_OOP_DEAD_ZONE_T,
	//PT����0x49020200
	E_OOP_PT_T,
	//CT����0x49030200
	E_OOP_CT_T,
	//�͵�ѹ����0x49040200
	E_OOP_LOW_U_T,
	//����ѹ����0x49050200
	E_OOP_OVER_U_T,
	//���ر���0x49060200
	E_OOP_HEAVY_LOAD_T,
	//���ر���0x49070200
	E_OOP_OVER_LOAD_T,
	//�������ɼ�������ʱ��0x49080200
	E_OOP_YX_GET_TIME_T,
	//��բ������屣��ʱ��0x49090200
	E_OOP_PULSE_TIME_T,
	//�����Զ������0x490A0200
	E_OOP_AUTO_ACT_CYCLE_T,
	//��ص�ѹ�ͱ�����ֵ0x490B0200
	E_OOP_BATT_LOW_U_T,
	//��·���ϱ���0x490C0200
	E_OOP_SHORT_FAULT_T,
	//¼���糡�½�����0x490E0200
	E_OOP_RECORD_DROP_T,
	//���ù���0x490F0200
	E_OOP_RESET_TPYE_T,
	//���ù���0x490F0300
	E_OOP_RESET_TIME_T,
	//���ɵ���������ֵ0x49100200
	E_OOP_LOAD_I_LIMIT_T,
	//������������ֵ0x49110200
	E_OOP_OVER_I_LIMIT_T,
	//����·�е���ֵ0x49120200
	E_OOP_PHASE_LINE_POWER_T,
	//����ָʾ���Զ�����0x49130200
	E_OOP_FAULT_TUBE_RESET_T,
	//����ң�ű���ʱ��0x49140200
	E_OOP_FAULT_YX_KEEP_T,
	//��������������I�Σ�0x49160200
	//��������������II�Σ�0x49160300
	//��������������III�Σ�0x49160400
	//�������������������0x49160500
	E_OOP_OVER_I_PARA_T,
    E_NULL=0xFF,

}DATASTRUCT; 

//645���� �� ������� ��Ӧ��ϵ
typedef struct{
	OOP_OAD_U  Oad;          //OAD
	int8   DesScaler;        //0AD��Ӧ�Ļ��㵥λ
	uint32  DI07;	         //07Э�����ݱ�ʶ
	uint8   DataLen;	     //645 ÿ����Ԫ�صĳ���
	uint8   TotalDataLen;	 //645 �ܳ���
	uint16  DI97;			 //97Э�����ݱ�ʶ
    DATASTRUCT  CovertSruct; //Ҫת�ɵĽṹ������
    int8   Scaler;           //645С����λ��
}Map645ToOop;

#ifdef __cplusplus
}
#endif

#endif // __PW_ALL_H__