/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief：   性能测试线程
* @date：    2020-4-21
* @history：
*********************************************************************
*/

#ifndef __TEST_THREAD_H__
#define __TEST_THREAD_H__

#include "mes_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

    int GetIniKeyString(char *title, char *key, char *filename, char *buf);
    extern DB_CLIENT g_dbClients[MAX_THREAD];

    void *thread_function(void *pArg);
    void *thread_functions(void *pArg);
    void *thread_functionsSY(void *pArg);
	void *thread_StabilityTest(void *pArg);
    void *thread_OadTest(void *pArg);
	void *thread_OadTest1(void *pArg);
    int app_test_msg(MESSAGE_INFO_T *pInMsg);
    int app_send_msg(MESSAGE_INFO_T *pInMsg, void *sendMsg);
    int app_deal_auto_del_msg(MESSAGE_INFO_T *pInMsg);
    int app_deal_selfhelp_del_msg(MESSAGE_INFO_T *pInMsg);
    int app_deal_auto_del_msg1(MESSAGE_INFO_T *pInMsg);
    int app_deal_auto_del_msg2(MESSAGE_INFO_T *pInMsg);
    int app_deal_auto_del_msg3(MESSAGE_INFO_T *pInMsg);
	int app_deal_StabilityTest_msg(MESSAGE_INFO_T *pInMsg);
	void StabuilityTest(int select, struct tm *time);

	void Writefile(char *data, int len);

    //自动删除
    void test_functions(Para para, int j);
    //自救删除
    void test_functionsSY(Para para, int j);
	int Readrecordfile(char *data, char *infoNum);
	void FastSecondToDate(struct timeval* tv, struct timezone* tz, struct tm* tm);
	BOOL DT_DateTimeShort2Time(OOP_DATETIME_S_T *dateTime,time_t *linuxTim);
	BOOL DT_DateTimeShortIsValid(OOP_DATETIME_S_T *DateTime);
	time_t FastDateToSecond(struct tm* tm);
	BOOL DT_IsLeap(uint16 year);
	int CompareData(char *str1, char *str2, int bytes);
#ifdef __cplusplus
}
#endif

#endif //__TEST_THREAD_H__

