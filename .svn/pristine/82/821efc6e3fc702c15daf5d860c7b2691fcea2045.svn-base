/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块头文件
* @date：    2020-4-3
* @history： 
*********************************************************************
*/
#ifndef	__APPCTRL_CFG_H_
#define __APPCTRL_CFG_H_

#include "oopType.h"

#ifdef __cplusplus
        extern "C" {
#endif

#define CTRL_DB_QUEUE_SIZE       400
typedef struct tagCtrlDBNotify 
{
	pthread_mutex_t    f_lock;	
	uint32             num;
	uint32             OAD[CTRL_DB_QUEUE_SIZE];
    uint16             infonum[CTRL_DB_QUEUE_SIZE];
}CTRLDB_NOTIFY_T;

void appctrl_get_arg(int cid);
void appctrl_clear_daystatic();
void appctrl_clear_monstatic();
void appctrl_assign_status(int cid);
void appctrl_init_arg();
void appctrl_init_data();
int appctrl_update_db(uint32 Oad, uint16 infoNum);

void appctrl_write_last_time(OOP_DATETIME_S_T *ptTime);
void appctrl_get_last_time(OOP_DATETIME_S_T *ptTime);

#ifdef __cplusplus
}
#endif

#endif


