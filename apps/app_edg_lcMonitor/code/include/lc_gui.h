/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控显示
* @date：    2020-6-30
* @history： 
* @User deatil:

*********************************************************************
*/
#ifndef	__LC_GUI_H
#define __LC_GUI_H
    
#ifdef __cplusplus
        extern "C" {
#endif

/** @brief 缓存 */
typedef struct tag_LC_EVT_FIFO
{
    int nCount;                       //成员数量 
    OOP_ABNORMALCTLOG_T  fifo[10];       //容量
}LC_EVT_FIFO_T;

void lc_app_stop();
int lc_get_dbClient();

#ifdef __cplusplus
}
#endif

#endif


