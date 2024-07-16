/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   MAP交互消息解析
* @date：    2020/2/28
* @history： 
*********************************************************************
*/
#ifndef __MAPMSG_SPLIT_H__
#define __MAPMSG_SPLIT_H__

#include "mapmsg.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define FRAMESTART   0x68
#define FRAMEEND     0x16

uint16 CRC16(uint8 *pMsg, uint16 len);
void *mapmsg_memcpy_r(void *dest, void *src, int count);
uint16 mapmsg_get_len_offset(uint8 *indata,uint8 *bytes);
uint8 mapmsg_set_len_offset(uint16 eleLen, uint8 *indata);
int mapmsg_split(uint8 *pBuf, uint16 bufLen, MAPMSG_INFO_T **ppMsgOut);
uint16 mapmsg_package(MAPMSG_INFO_T *pMsgInfo, uint8 *pMsgBuf, uint16 maxlen);





#ifdef __cplusplus
}
#endif

#endif /* __APPMSG_SPLIT_H__ */
