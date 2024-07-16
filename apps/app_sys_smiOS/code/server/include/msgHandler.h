/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： msgHandler.h
           消息句柄
* @date： 2019-11-9
*********************************************************************
*/

#ifndef __MSG_HANDLER_H__
#define __MSG_HANDLER_H__

#include "baseType.h"
#include "msgHelper.h"
#include "msgServiceBase.h"
#include "framecomm.h"

#ifdef __cplusplus
extern "C" {
#endif

// typedef enum __event_id {

//     EMERGENCY_BLACKOUT = 0x00000001,
//     SYSTEM_POWER_OFF   = 0x00000002,
//     SYSTEM_REBOOT   = 0x00000003,
//     SYSTEM_POWER_ON = 0x00000004,
//     SYSTEM_CAL_TIME = 0x00000005,

//     DB_DATA_UPDATE = 0x00050001,
//     DB_INIT = 0x00050002

// } EVENT_ID_E;

/* @brief:类型定义：  通用的Handle处理接口*/
typedef sint32 (*MSG_HANDLE)(uint8 *       req_xdr,
                             sint32        req_xdr_len,
                             uint8 *       rsp_xdr,
                             sint32        rsp_xdr_max_len,
                             MSG_CLIENT_T *client);

/* @brief:声明选择消息接口处理函数  */
/*
MSG_HANDLE select_handle_func(uint32 tag);

sint32 msg_heartbeat_check(uint8 *       req_xdr,
                           sint32        req_xdr_len,
                           uint8 *       rsp_xdr,
                           sint32        rsp_xdr_max_len,
                           MSG_CLIENT_T *client);

sint32 msg_execute_commands(uint8 *       req_xdr,
                            sint32        req_xdr_len,
                            uint8 *       rsp_xdr,
                            sint32        rsp_xdr_max_len,
                            MSG_CLIENT_T *client);

sint32 msg_transmission(uint8 *       req_xdr,
                        sint32        req_xdr_len,
                        uint8 *       rsp_xdr,
                        sint32        rsp_xdr_max_len,
                        MSG_CLIENT_T *client);

sint32 msg_event_fast_power_cut(uint8 *       req_xdr,
                                sint32        req_xdr_len,
                                uint8 *       rsp_xdr,
                                sint32        rsp_xdr_max_len,
                                MSG_CLIENT_T *client);

sint32 msg_event_system_power_off(uint8 *       req_xdr,
                                  sint32        req_xdr_len,
                                  uint8 *       rsp_xdr,
                                  sint32        rsp_xdr_max_len,
                                  MSG_CLIENT_T *client);

sint32 msg_event_system_restart(uint8 *       req_xdr,
                                sint32        req_xdr_len,
                                uint8 *       rsp_xdr,
                                sint32        rsp_xdr_max_len,
                                MSG_CLIENT_T *client);

sint32 msg_event_system_power_on(uint8 *       req_xdr,
                                 sint32        req_xdr_len,
                                 uint8 *       rsp_xdr,
                                 sint32        rsp_xdr_max_len,
                                 MSG_CLIENT_T *client);

sint32 msg_app_login_request(uint8 *       req_xdr,
                             sint32        req_xdr_len,
                             uint8 *       rsp_xdr,
                             sint32        rsp_xdr_max_len,
                             MSG_CLIENT_T *client);

sint32 msg_app_logout_request(uint8 *       req_xdr,
                              sint32        req_xdr_len,
                              uint8 *       rsp_xdr,
                              sint32        rsp_xdr_max_len,
                              MSG_CLIENT_T *client);

sint32 msg_query_login_apps(uint8 *       req_xdr,
                            sint32        req_xdr_len,
                            uint8 *       rsp_xdr,
                            sint32        rsp_xdr_max_len,
                            MSG_CLIENT_T *client);

sint32 msg_query_app_by_name(uint8 *       req_xdr,
                             sint32        req_xdr_len,
                             uint8 *       rsp_xdr,
                             sint32        rsp_xdr_max_len,
                             MSG_CLIENT_T *client);

sint32 msg_event_subscribe(uint8 *       req_xdr,
                           sint32        req_xdr_len,
                           uint8 *       rsp_xdr,
                           sint32        rsp_xdr_max_len,
                           MSG_CLIENT_T *client);

sint32 msg_event_unsubscribe(uint8 *       req_xdr,
                             sint32        req_xdr_len,
                             uint8 *       rsp_xdr,
                             sint32        rsp_xdr_max_len,
                             MSG_CLIENT_T *client);

sint32 msg_get_subscribe_info(uint8 *       req_xdr,
                              sint32        req_xdr_len,
                              uint8 *       rsp_xdr,
                              sint32        rsp_xdr_max_len,
                              MSG_CLIENT_T *client);

sint32 msg_request_system_restart(uint8 *       req_xdr,
                                  sint32        req_xdr_len,
                                  uint8 *       rsp_xdr,
                                  sint32        rsp_xdr_max_len,
                                  MSG_CLIENT_T *client);

sint32 msg_get_power_status(uint8 *       req_xdr,
                            sint32        req_xdr_len,
                            uint8 *       rsp_xdr,
                            sint32        rsp_xdr_max_len,
                            MSG_CLIENT_T *client);

sint32 msg_get_ethernet_status(uint8 *       req_xdr,
                               sint32        req_xdr_len,
                               uint8 *       rsp_xdr,
                               sint32        rsp_xdr_max_len,
                               MSG_CLIENT_T *client);
*/
void msg_handler_init(void *category);
void msg_handler(RAW_OBJ_T *   rawObjReq,
                 RAW_OBJ_T *   rawObjRsp,
                 MSG_OBJ_T *   msgReq,
                 MSG_OBJ_T *   msgRsp,
                 MSG_CLIENT_T *client);

sint32
general_event_handler(uint32 eventID ,uint8 *req_xdr, sint32 req_xdr_len);

int rtcTimeSet(struct tm rtc1);

typedef struct
{
    uint8 serviceType;      //1 ssh  2 sftp
    uint32 authorizeTime;       //授权时长
}SERVICE_START_T;

void smios_set_starttime();
#ifdef __cplusplus
}
#endif

#endif