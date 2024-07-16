/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：UDP消息相关访问接口
* @date： 2019.10.23
*********************************************************************
*/
#ifndef __MSGMQTT_H__
#define __MSGMQTT_H__

#include "../../include/baseCommon.h"
#include "msgUdp.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DEBUG_MQTT
#ifdef DEBUG_MQTT
  #include <errno.h>
  extern int errno;
  #define MPRINT(x...) ({ if (getenv("DEBUG_MQTT") != NULL)  printf(x);})
#else
  #define MPRINT(x...)
#endif

/**********************************************************************
* @name      : msg_mqtt_package
* @brief     ：指定格式消息组装成
* @param[in] ：MSG_INFO_T *pMsgInfo  结构消息
               bufLenMax             buf最大长度
* @param[out]：uint8 *buf            组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
uint16 msg_mqtt_package(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf);

/**********************************************************************
* @name      : msg_mqtt_split
* @brief     ：接受的消息解析成指定格式
* @param[in] ：uint8 *pBuf           接受到的报文
               uint16 bufLen         报文长度
* @param[out]：MSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
int msg_mqtt_split(uint8 *pBuf, uint16 bufLen,uint32 ipAddr, uint16 port, MSG_INFO_T **ppMsgOut);

#ifdef __cplusplus
}
#endif

#endif