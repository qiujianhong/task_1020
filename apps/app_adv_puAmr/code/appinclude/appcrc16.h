/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程CRC计算
* @date：    2019-12-9
* @history： 
*********************************************************************
*/
#ifndef	__APPIPC_H
#define __APPIPC_H


#ifdef __cplusplus
        extern "C" {
#endif


uint16 app_crc16_get(const uint8* buf, uint32 len);

#ifdef __cplusplus
}
#endif

#endif


