/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2020-01-03
*********************************************************************
*/


#ifndef __PW_COMMON_ZHJ_H__
#define __PW_COMMON_ZHJ_H__

#include "pw_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif

/*******************浙江地区专用参数与变量**********************/

/****************************************************************
***全局函数声明
****************************************************************/

extern LOCK_QUEUE_T g_stSoeBuff;                        //遥信变位共享内存

extern time_t nPreDayTime_t;  //前一日23:59:59转换的秒数
extern time_t nPreMonTime_t;  //上一月最后一天23:59:59转换的秒数
extern time_t nTmnStartTime_t;  //终端启动时间

extern int his_frozen_file_read_zhj(uint16 groupIndex, uint16 seqIndex, uint16 taskIndex, char *pData, uint32 nMaxLen);
extern int his_frozen_file_write_zhj(uint16 groupIndex, uint16 seqIndex, uint16 taskIndex, char *pData, uint32 nMaxLen);
extern int fixed_area_iden_read_zhj(uint16 nArea, char *pData, uint32 nMaxLen);
extern int fixed_area_iden_write_zhj(uint16 nArea, char *pData, uint32 nMaxLen);
extern int fixed_para_value_read_zhj(uint16 nArea, char *pData, uint32 nMaxLen);
extern int fixed_para_value_write_zhj(uint16 nArea, char *pData, uint32 nMaxLen);
extern int config_xml_load_zhj();
extern void task_run_step_init_zhj();
extern void param_data_init_zhj();
extern int global_init_zhj();
extern int para_query_int64_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, int64 *nOut);
extern int para_query_float32_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, float32 *fOut);
extern int para_query_array_zhj(uint16 typeIndex, uint16 groupIndex, uint16 seqIndex, uint32 nParaAddr, uint8 *pOut);
extern void yx_db_center_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index);
extern void yx_real_signal_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index);
extern void yx_calculate_statistics_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index);
extern void yc_db_nomal_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index);
extern void yc_db_record_read_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index);
extern void yc_calculate_statistics_zhj(uint8 typeIndex, uint16 groupIndex, uint16 seqIndex, uint16 index);
extern void data_db_center_read_zhj(READ_STATE_T ReadState);

#ifdef __cplusplus
}
#endif

#endif // __PW_COMMON_ZHJ_H__