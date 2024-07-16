/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief： udpClient.h
* @date： 2020-6-10
*********************************************************************
*/
#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__

#include "baseConstant.h"
#include "xlChannel.h"

#ifdef __cplusplus
extern "C" {
#endif

XL_CLIENT_T *new_udp_client(void);

void del_udp_client(XL_CLIENT_T *c);

#ifdef __cplusplus
}
#endif

#endif