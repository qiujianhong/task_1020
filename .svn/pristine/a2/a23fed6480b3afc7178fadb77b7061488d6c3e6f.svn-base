/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/


#ifndef __PW_DEBUG_H__
#define __PW_DEBUG_H__

#include "pw_redef.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

#define DEBUG_104FRAME
#ifdef DEBUG_104FRAME
#include <errno.h>
extern int errno;
#define printx(x...) ({if (getenv("DEBUG_104FRAME") != NULL)  printf(x);})
#else
#define printx(x...)
#endif

#define DEBUG_104DB
#ifdef DEBUG_104DB
#include <errno.h>
extern int errno;
#define printdb(x...) ({if (getenv("DEBUG_104DB") != NULL)  printf(x);})
#else
#define printdb(x...)
#endif

/*******************************************************************************
* 函数声明
*******************************************************************************/
extern void buffer_print(uint8 *buf, int len);
extern void buffer_print_db(uint8 *buf, int len);
extern void buff_no_space_to_str(char *buf, int len, int maxlen, char *comm_log);
extern void buff_space_to_str(char *buf, int len, int maxlen, char *comm_log);
extern void log_buff_detail_save(uint8 *buf, int len, char *path);
extern void bcd_time_print(DATE_TIME_T stCurBcdTime);
extern void bcd_time_print_db(DATE_TIME_T stCurBcdTime);



#ifdef __cplusplus
}
#endif

#endif // __PW_DEBUG_H__