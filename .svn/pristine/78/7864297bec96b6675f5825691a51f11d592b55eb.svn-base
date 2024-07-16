/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief： xlDebug.h
* @date： 2020-6-10
*********************************************************************
*/
#ifndef __XL_DEBUG_H__
#define __XL_DEBUG_H__

// #define XL_DEBUG_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

#if defined XL_DEBUG_ENABLE


#include "olog.h"
#define XL_DEBUG    dolog_debug
#define XL_INFO     dolog_info
#define XL_WARN     dolog_warn
#define XL_ERROR    dolog_error
#define XL_FATAL    dolog_fatal


#else
//DEBUG
#define XL_DEBUG(m,...)
//INFO
#define XL_INFO(m,...)
//WARN
#define XL_WARN(m, ...)
//ERROR
#define XL_ERROR(m, ...)
//FATAL
#define XL_FATAL(m, ...)

#endif

#ifdef __cplusplus
}
#endif

#endif