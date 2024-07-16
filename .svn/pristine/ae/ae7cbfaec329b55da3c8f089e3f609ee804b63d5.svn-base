/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/


#ifndef __PW_LIB_H__
#define __PW_LIB_H__

#include "pw_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************************************************************************
* 函数声明
*******************************************************************************/
extern void pw_string_print(uint8 *buf, int16 len);
extern int32 pw_lock_queue_write(LOCK_QUEUE_T *pLockQueue, uint8 *buf, int32 buf_len, uint16 pipe);
extern int32 pw_lock_queue_read(LOCK_QUEUE_T *pLockQueue, uint8 *buf, uint16 *pipe);
extern uint8 bcd_to_hex(uint8 compress_bcd);
extern time_t bcd_time_to_sec(uint8* BcdMeterTime);
extern time_t hex_time_to_sec(DATE_TIME_T *PointTime);
extern uint8 hex_to_bcd(uint8 hex);
extern void cur_bin_time_get(DATE_TIME_T *pTime);
extern void cur_bcd_time_get(DATE_TIME_T *pTime);
extern void sys_bcd_time_get( SYSTEM_BCD_TIME_T *pRtc );
extern void systm_to_ooptm(time_t sys_tm, OOP_DATETIME_S_T *poop_time);
extern void ooptm_to_systm(OOP_DATETIME_S_T oop_time, time_t *psys_tm);
extern void cp56time2a_to_ooptm(uint8 *cp56time, OOP_DATETIME_S_T *oop_time);
extern void ooptm_to_cp56time2a(OOP_DATETIME_S_T oop_time, uint8 *cp56time);
extern void *memcpy_r(void *dest, const void *src, int count);



#ifdef __cplusplus
}
#endif

#endif // __PW_LIB_H__