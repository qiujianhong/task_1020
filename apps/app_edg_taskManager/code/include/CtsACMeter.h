
#ifndef __CTSACMETER_H__
#define __CTSACMETER_H__

int taskManager_acMeter_fd_init(void);
int taskManager_acMeter_revsnd(uint8 *sndData, uint16 sndLen, uint8 *pOutBuf, uint16 *rcvLen);
int taskmng_acmeter_frozen_get(OOP_OAD_U MainOad,OOP_OAD_U SubOad,CTS_RUN_TASK_T *RunTask,SCHEME_TASK_RUN_INFO *TaskInfo);
 void taskmng_time_manage_tg(void);
void taskmng_record_file_delete_tg(uint16 TaskId);
 void taskmng_data_init_check_tg();
 void taskmng_task_run_ergodic_tg();
 BOOL taskmng_task_is_tg_check(uint8 taskindex);
 void taskmng_task_record_file_check_tg();
 void taskmng_task_record_file_create_tg(uint16 TaskId);
 BOOL taskmng_task_run_time_update_tg(uint8 index,CTS_RUN_TASK_T *TaskInfo);
 void taskmng_task_period_stat_update_tg(uint8 index,CTS_RUN_TASK_T *TaskInfo);
 int taskmng_task_data_reading_tg();
 int taskmng_task_runlist_check_tg(CTS_RUN_TASK_T **TaskInfo);
 void taskmng_task_record_file_load_tg(SCHEME_TASK_RUN_INFO *TaskInfo,CTS_RUN_TASK_T* RunTask);
 int taskmng_data_transfer_tg(OOP_OAD_U MainOad,OOP_OAD_U SubOad,CTS_RUN_TASK_T *RunTask,SCHEME_TASK_RUN_INFO *TaskInfo);
 void taskmng_data_save_tg(uint8* buf,uint16 len,OOP_OAD_U MainOad,OOP_OAD_U SubOad,CTS_RUN_TASK_T *RunTask,time_t StoreTime);
 void taskmng_task_run_sort_tg();
 int taskmng_relay_check_tg(void);


#endif
