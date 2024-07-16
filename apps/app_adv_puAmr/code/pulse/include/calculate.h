/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ң������ͷ�ļ�
* @date��    2019-11-21
* @history�� 
*********************************************************************
*/

#ifndef __CALCULATE_H__
#define __CALCULATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EN_SHARP		1		/*�������*/
#define EN_TOP			2		/*�������*/
#define EN_LEVEL		3		/*����ƽ��*/
#define EN_BOTTOM		4		/*�����ȶ�*/

#define POS_AE_Tag      0		/*00�����й�*/
#define POS_RE_Tag	    1		/*10�����޹�*/
#define NEG_AE_Tag	    2		/*01�����й�*/
#define NEG_RE_Tag	    3		/*11�����޹�*/

#define DM_POWER_MAX    15      /*��������������*/


uint8 pulse_get_rateNO(OOP_DAYPERIOD_T *ptDayPeriod, uint8 hours, uint8 mins);
uint32 pulse_Acal_pow_en(uint32 count, uint32 timems, uint32 K, uint32* pleftNum, uint32 *pEn);
uint32 cal_pls_dm(uint32 curPower, uint8 *pNum, uint32 powerList[]);

#ifdef __cplusplus
}
#endif

#endif
