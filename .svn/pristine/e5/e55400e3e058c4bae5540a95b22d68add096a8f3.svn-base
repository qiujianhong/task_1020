/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： 功能描述
* @date： 2019-10-25
*********************************************************************
*/


#ifndef __PW_REDEF_H__
#define __PW_REDEF_H__

#include "pw_all.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
#pragma warning(disable:4201)
#endif


/*******************************************************************************
* 数组长度定义
*******************************************************************************/
#define MAX_QUEUE_CNT         20                //通讯数据队列数量
#define MAX_QUEUE_BUFF_LEN    500               //通讯数据buff长度
#define MAX_MSG_LEN           5120              //消息最大长度

/*******************************************************************************
* 数组长度定义
*******************************************************************************/
#define DA_ESAM_MAX_LEN 	    	8192        //配网加密芯片通讯最大长度

/*******************************************************************************
* 数组长度定义
*******************************************************************************/
#define MAX_OAD_COLS_NUM (2)    //关联次OAD最大个数

/*******************************************************************************
* 枚举/结构体定义
*******************************************************************************/


//队列
typedef struct
{
	uint16  QueuePipe;							//队列通道序号
    int32   QueueLen;					        //队列长度
    uint8   QueueBuf[MAX_QUEUE_BUFF_LEN];	    //队列存储缓冲区
}QUEUE_T;

//锁共享缓冲区结构
typedef struct
{
    pthread_mutex_t QueueLock;					//接收缓冲区互斥锁
    int16  QueueHead;						    //队列头
    int16  QueueTail;							//队列尾
    QUEUE_T Queue[MAX_QUEUE_CNT];	            //通讯数据队列
}LOCK_QUEUE_T;

//线程参数结构
typedef struct
{
	LOCK_QUEUE_T *RecvBuf;	                    //接收缓冲区结构指针
    LOCK_QUEUE_T *SendBuf;	                    //发送缓冲区结构指针
}THREAD_ARG_T;

typedef struct
{
    uint8   Second;                             //BIN or BCD
    uint8   Minute;                             //BIN or BCD
    uint8   Hour;                               //BIN or BCD
    uint8   Day;                                //BIN or BCD
    uint8   Month;                              //BIN or BCD
    uint8   Year;                               //BIN or BCD
}DATE_TIME_T;                                    //日期时间结构

typedef struct
{
    uint8   Second				:1;				//过秒标志
    uint8   Minute				:1;				//过分标志
    uint8   Hour			    :1;				//过时标志
    uint8   Day					:1;    			//过日标志
    uint8   Month			    :1;				//过月标志
    uint8   ReserveBit6			:1;				//保留
    uint8   ReserveBit7			:1;				//保留
    uint8   ReserveBit8			:1;				//保留
}PAST_TIME_FLAG_BIT_T;

typedef union
{
    PAST_TIME_FLAG_BIT_T   BitSect;
    uint8  			       Word;
}PAST_TIME_FLAG_T;

/* 实时时钟数据结构 */
typedef struct
{	
	char	ss;	/* 0~59 BCD */
	char	nn;	/* 0~59 BCD */
	char	hh;	/* 0~23 BCD */
	char	dd;	/* 1~31 BCD */
	char	mm;	/* 1~12 BCD */
	char	yy;	/* 00~99 BCD */
	char	ww;	/* 0~6 opposite Sun~Sat */	
}SYSTEM_BCD_TIME_T;


#ifdef __cplusplus
}
#endif

#endif // __PW_REDEF_H__