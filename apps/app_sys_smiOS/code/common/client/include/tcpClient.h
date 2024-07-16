/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief： tcpClient.h
* @date： 2020-6-10
*********************************************************************
*/

#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "baseConstant.h"
#include "xlChannel.h"

#ifdef __cplusplus
extern "C" {
#endif

XL_CLIENT_T *new_tcp_client(void);

void del_tcp_client(XL_CLIENT_T *c);

#ifdef __cplusplus
}
#endif

#endif