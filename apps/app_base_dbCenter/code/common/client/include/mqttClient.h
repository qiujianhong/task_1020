/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief： mqttClient.h
* @date： 2020-6-10
*********************************************************************
*/
#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include "baseConstant.h"
#include "xlChannel.h"

#ifdef __cplusplus
extern "C" {
#endif

//创建mqtt客户端
XL_CLIENT_T *new_mqtt_client(void);

//删除mqtt客户端
void del_mqtt_client(XL_CLIENT_T *c);

//mqtt客户端读接口
sint32 mqtt_clien_read_plus(XL_CLIENT_T *c,
                            char *       topic,
                            sint32 *     qos,
                            sint32 *     mid,
                            char *       data,
                            sint32       dataLen,
                            long         msecs);

//mqtt客户端写接口
sint32 mqtt_clien_write_plus(XL_CLIENT_T *c,
                             char *       topic,
                             sint32       qos,
                             sint32 *     mid,
                             const char * data,
                             sint32       dataLen);
#ifdef __cplusplus
}
#endif

#endif