/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级app用采对外消息处理
* @date：    2020-1-6
* @history： 
*********************************************************************
*/

#ifndef __MSG_PROC_H__
#define __MSG_PROC_H__

#ifdef __cplusplus
extern "C" {
#endif

void *out_msg_entry(void *pArg);
void *task_msg_entry(void *pArg);
int msg_task_start(uint32 dbip, uint16 dbport);

#ifdef __cplusplus
}
#endif

#endif //__MQTT_MSG_H__

