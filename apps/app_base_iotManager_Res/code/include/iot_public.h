/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块PPP操作相关定义申明
* @date：    2019-10-24
* @history： 
*********************************************************************
*/
#ifndef __PPP_OPER_H__
#define __PPP_OPER_H__
        
#ifdef __cplusplus
        extern "C" {
#endif
#include "iot_init.h"
#include <sys/stat.h>
#include <errno.h>
#include "baseCommon.h"
#include "framecomm.h"
#include "mqtt_client.h"
#include "task_msg.h"
#include "appmsg_split.h"
#include "task_base.h"
#include "devinfo.h"
#include "hal.h"
#include <stdlib.h>
#include <sys/stat.h>
#include "iot_mqtt.h"
#define MAX_QUEUE_SIZE  2200
#define IS_LITTLE_ENDIAN    1

void timeformat(char* time,OOP_DATETIME_S_T *ooptime);
uint16 percentdatachange(char* buf);
char * strim(char *str);
void IPchange(char* buf,uint8* out);
int strrpc(char *str,char *outstr,char *oldstr,char *newstr);
uint8  iot_get_appid(uint8 *appid, uint8 len);
uint16 get_len_offset(uint8 *indata,uint8 *bytes);
uint16 msg_mqtt_package(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf);
uint8 set_len_offset(uint16 eleLen, uint8 *indata);
int32 taskmng_plc_queue_write(LockQueue_T *pLockQueue, uint8 *buf, int32 buf_len);
int32 taskmng_plc_queue_read(LockQueue_T *pLockQueue, uint8 *buf);
uint16 wuguan_appmsg_package(MSG_INFO_T *pMsgInfo, uint8 **ppBuf);
int mqtt_topic_write(uint8* SendBuf,uint32 SendLen,char* topic);
int getshellresult(const char* cmd,char result[][500],int size);
uint8 sizeAndmd5_judge(char *filepath,uint32 size,char* md5);
uint8 sizeAndSM3_judge(char *filepath,uint32 size,char* sm3);
uint16 iot_appmsg_package(MSG_INFO_T *pMsgInfo, uint8 **ppBuf);
void ssplit(char *src,const char *delim,char dest[][500],int *num);
OOP_DATETIME_S_T timeformat_s(char* time);

#ifdef __cplusplus
}
#endif

#endif //__PPP_OPER_H__

