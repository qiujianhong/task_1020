/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/
#include <pthread.h>

#ifndef _AC_485comm_H
#define _AC_485comm_H

#include "uart_api.h"

//维护口编号
#define  PORT_485          2  

//消息体内容最大长度
#define  MQTT_MSG_LEN_MAX  2048

#define UART_CTRLBITS_SET(d,p,s,f) ((((f)<<5)&0x60)|(((s)<<4)&0x10)|(((p)<<2)&0x0c)|((d)&0x03))

typedef struct tagCOMM_485_INFO_T
{
    uint8              isSubcribe;
    uint8              isGetInfo;
    uint8              bNoMeter;    //是否配置4路485档案
    UART_MODESTAUS_T   tModeInfo;
    UART_MODEINFO_T    tUartMode;
}COMM_485_INFO_T;

extern COMM_485_INFO_T  g_Comm485Info;

extern int ac_485comm_msg_proc(MESSAGE_INFO_T *pMsg);
extern int ac_485comm_msg_init(void);
extern int ac_485comm_send(uint32 port, uint8* pSend, int sendLen);
extern int ac_485comm_subscribe(uint32 port);
extern int ac_485comm_getinfo(uint32 port);
extern uint8* get_07ptr_from_buf(uint8* buf, uint32 maxLen, uint32* offset, uint32* outLen);
extern void ac_485comm_data_convert(MSG_INFO_T *ptMsgInfo  ,MESSAGE_INFO_T *ptMessageInfo);
extern uint8 *get_07ptr_from_buf_n(uint8* buf, uint32 maxLen, uint32* offset, uint32* outLen);
#endif