/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   遥脉计算头文件
* @date：    2019-11-21
* @history： 
*********************************************************************
*/

#ifndef __CALCULATE_H__
#define __CALCULATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define EN_SHARP		1		/*电量尖段*/
#define EN_TOP			2		/*电量峰段*/
#define EN_LEVEL		3		/*电量平段*/
#define EN_BOTTOM		4		/*电量谷段*/

#define POS_AE_Tag      0		/*00正向有功*/
#define POS_RE_Tag	    1		/*10正向无功*/
#define NEG_AE_Tag	    2		/*01反向有功*/
#define NEG_RE_Tag	    3		/*11反向无功*/

#define DM_POWER_MAX    15      /*需量计算数据量*/


uint8 pulse_get_rateNO(OOP_DAYPERIOD_T *ptDayPeriod, uint8 hours, uint8 mins);
uint32 pulse_Acal_pow_en(uint32 count, uint32 timems, uint32 K, uint32* pleftNum, uint32 *pEn);
uint32 cal_pls_dm(uint32 curPower, uint8 *pNum, uint32 powerList[]);

#ifdef __cplusplus
}
#endif

#endif
