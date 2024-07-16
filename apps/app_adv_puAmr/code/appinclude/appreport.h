/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��Ӧ�õ��Ƚ��������ϱ�
* @date��    2019-12-5
* @history�� 
*********************************************************************
*/
#ifndef	__APPREPORT_H
#define __APPREPORT_H


#ifdef __cplusplus
        extern "C" {
#endif

typedef struct tagAppReportThread
{
    uint8        *pRunFlag;
}REPORT_THREAD_T;


void app_report_init();
void app_report_getdb(int sockdbfd);
void *app_auto_report(void *pArg);
void *app_preconnect_proc(void *pArg);
void *app_event_time_work(void *pArg);
void *app_other_time_work(void *pArg);
void rpt_transresult_data();
int appdb_get_version(int clientid, uint8 logicId, uint16 infoNum, uint32 value);

#ifdef __cplusplus
}
#endif

#endif


