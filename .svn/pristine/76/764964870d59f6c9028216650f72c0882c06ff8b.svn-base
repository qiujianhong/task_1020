/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： xlCmd.h
* @date： 2019-11-9
*********************************************************************
*/
#ifndef __XL_CMD_H__
#define __XL_CMD_H__

#include "baseType.h"
#include "xlmPlatformInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

void platform_TimeReq(XL_PLATFORM_T *platform);

void platform_time_rsp(sint32 mid, sint8 *jsonObjStr);

void platform_get_smiOS_status(XL_PLATFORM_T *platform,
                               sint32         mid,
                               sint8 *        jsonObjStr);

void platform_image_manager(XL_PLATFORM_T *platform,
                            sint8 *        cmd,
                            sint32         mid,
                            sint8 *        jsonObjStr);

void platform_container_manager(XL_PLATFORM_T *platform,
                                sint8 *        cmd,
                                sint32         mid,
                                sint8 *        jsonObjStr);

void platform_app_manager(XL_PLATFORM_T *platform,
                          sint8 *        cmd,
                          sint32         mid,
                          sint8 *        jsonObjStr);

// int platform_recv(XL_PLATFORM_T *platform,
//                   sint32 *       mid,
//                   sint8          topic[MAX_TOPIC_LEN],
//                   sint8          msgType[PL_MSG_TYPE_LEN],
//                   sint8 *        data,
//                   sint32         dataMaxLen,
//                   long           msecs);

#ifdef __cplusplus
}
#endif

#endif