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
#include "msgServiceBase.h"

#ifdef __cplusplus
extern "C" {
#endif

//注册回调函数
typedef sint32 (*LOGIN_CB_FP)(sint8 ack);

/* @brief:类型定义： 设置登录回调接口*/
void set_login_cb_fp(LOGIN_CB_FP fp);

/* @brief:类型定义：  通用的Handle处理接口*/
typedef sint32 (*XDR_HANDLE)(uint8 *req_xdr,
                             sint32 req_xdr_len,
                             uint8 *rsp_xdr,
                             sint32 rsp_xdr_max_len);

/* @brief:声明选择消息接口处理函数  */
XDR_HANDLE select_handle_func(uint32 tag);

/* @brief:类型定义：  设置订阅信息*/
sint32 set_subscription(uint8 *req_xdr,
                        sint32 req_xdr_len,
                        uint8 *rsp_xdr,
                        sint32 rsp_xdr_max_len,
                        void * subscriber);
//消息处理
void msg_handler(RAW_OBJ_T *   rawObjReq,
                 RAW_OBJ_T *   rawObjRsp,
                 MSG_OBJ_T *   msgReq,
                 MSG_OBJ_T *   msgRsp,
                 MSG_CLIENT_T *client);

// sint32 register_handler_cb(uint32 tag, )


#ifdef __cplusplus
}
#endif

#endif