/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级app用采对内消息处理
* @date：    2020-3-5
* @history： 
*********************************************************************
*/

#ifndef __TASK_PROC_H__
#define __TASK_PROC_H__


#ifdef __cplusplus
extern "C" {
#endif

#define TASK_UNKNOW_MSG   0x3333

int main_task_init();
int task_appmain_recv_msg(MESSAGE_INFO_T *pMsgInfo);
int task_recv_mqttmsg(MESSAGE_INFO_T *pMsgInfo);


#ifdef __cplusplus
}
#endif

#endif //__MQTT_MSG_H__


