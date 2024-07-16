/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   统一终端平台消息定义
* @date：    2019/12/27
* @history： 
*********************************************************************
*/

#ifndef __MES_STRUCT_H__
#define __MES_STRUCT_H__

#include "dataType.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* 消息内容 */
typedef struct tagMsgInfo
{
    uint32 secc_num;      //成功次数
    uint32 err_num;       //失败次数
    uint32 total_time;    //总用时   毫秒秒  
}MSG_INFO_t;

/* 消息结构 */
typedef struct tagMsgHead1
{
    MSG_INFO_t  msgs[9];//消息内容
}MSG_HEAD1_t;

typedef struct tagRWMsgHead
{
    MSG_HEAD1_t  msgs;      //性能测试
    MSG_INFO_t   rwmsg[4];  //读写测试
}RW_MSG_HEAD_t;


/* 消息结构 */
typedef struct tagMsgHead2
{
    uint32 data_len[5];  //消息长度 10 500 1000 4000 8000
    MSG_INFO_t  msg1[5];//消息内容 
    MSG_INFO_t  msg2[5];//消息内容
    MSG_INFO_t  msg3[5];//消息内容
    MSG_INFO_t  msg4[5];//消息内容
}MSG_HEAD2_t;

/* 消息结构 */
typedef struct tagMsgHead3
{
    MSG_HEAD1_t  msg1;//消息内容
    MSG_HEAD1_t  msg2;//消息内容
}MSG_HEAD3_t;

typedef struct tag_BIG_DATA
{
    MSG_INFO_t  msgrecordes[9][10];//消息内容
    MSG_INFO_t  msgnormal;//消息内容
}MSG_BIG_DATA_t;

typedef struct mypara
{
	MESSAGE_INFO_T *pInMsg;//消息
	uint32 MainOAD;//主OAD
	uint32 ColOAD;//次OAD
	uint8   logicId; //逻辑地址
	uint8  _CLASS_DATA_E;//存储数据类型
}Para;

typedef struct readrecord
{
	int Num;
	int len;
	uint32 savetime[288];
	char record[288][600];
}Readrecord;


#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H__ */
