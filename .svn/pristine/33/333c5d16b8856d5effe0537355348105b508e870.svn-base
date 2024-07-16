/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief：   消息定义及收发接口
* @date：    2018/09/10
* @history： 
*********************************************************************
*/

#ifndef __MSG_H__
#define __MSG_H__

#include "common.h"
#include "utils.h"
#include "system.h"

#ifdef WIN32
#pragma warning(disable:4200)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define HMQT                    int32        /**< 消息队列TASK句柄        */

/** @brief 消息TLV三元组TAG结构    */
typedef union 
{
    uint32             nTag;             /**< 标识                            */
    struct 
    {
        uint32         iop  : 8;         /**< 接口操作                        */
        uint32         iid  : 24;         /**< 接口ID                            */
    };
} TAG_U;

/** @brief 消息结构    */
typedef struct 
{
    TAG_U             tag;             /**< 消息标识                        */
    uint32             nSize;             /**< 消息内容长度                    */
    uint8             pValue[0];         /**< 消息内容                        */
} MSG_T;

/** @brief 返回结果结构    */
typedef struct 
{
    int             nErrCode;         /**< 错误码(0为OK, 其他值为错误)    */
    uint32             nSize;          /**< 返回内容长度                    */
    uint8             pValue[0];         /**< 返回内容                        */
} ACK_T;

/** @brief 消息处理回调函数    */
typedef int (*MSG_HANDLER)(void *arg, const MSG_T *pMsg);

/**
 ******************************************************************************
 * @brief        分配一个消息
 * @param[in]    iid        ：    消息接口ID
 * @param[in]    iop     ：    消息接口操作编号
 * @param[in]    nSize    ：    消息值的长度
 * @return         成功返回消息指针, 失败返回NULL
 * @note         失败会直接产生异常, 所以可以不做异常判断
 *              分配完成后建议立刻发送出去，同时最多保有8个未发送的消息
 ******************************************************************************
 */
MSG_T* msg_alloc(uint32 iid, uint16 iop, uint32 nSize);

/**
 ******************************************************************************
 * @brief         分配一个返回信息
 * @param[in]    pMsg        ：    请求的消息
 * @param[in]    nErrCode    ：    返回值的错误码
 * @param[in]    nSize        ：    返回值的长度
 * @return         成功返回消息指针, 失败返回NULL
 * @note         失败会直接产生异常, 所以可以不做异常判断
 *              分配完成后建议立刻发送出去，同时最多保有8个未发送的返回数据
 ******************************************************************************
 */
ACK_T *msg_ack_alloc(const MSG_T *pMsg, int32 nErrCode, uint32 nSize);

/**
 ******************************************************************************
 * @brief         发送异步消息
 * @param[in]    hDst        :    发送消息目标队列句柄
 * @param[in]    ppMsg         :    需要发送的消息的指针(必须由MsgNewMsg分配, 不能是接收到的消息)
 * @param[in]    bHighPrio    :    消息优先级
 * @return         成功返回TRUE, 失败返回FALSE
 * @note         ppMsg调用后会置为NULL, 不能再使用;
                  也不要试图用其他变量备份ppMsg的指针值, 会产生系统异常
 ******************************************************************************
 */
BOOLEAN msg_post(HMQT hDst, MSG_T **ppMsg, BOOLEAN bHighPrio);

/**
 ******************************************************************************
 * @brief         发送事件消息
 * @param[in]    hDst        ：    发送消息目标队列句柄
 * @param[in]    nTag         ：    事件消息标识(1~MSGEVNET_END)
 * @return         成功返回TRUE, 失败返回FALSE
 * @note                 
 ******************************************************************************
 */
BOOLEAN msg_event_post(HMQT hDst, uint32 nTag);

/**
 ******************************************************************************
* @brief         发送同步消息并等待响应
* @param[in]    hDst         :    发送消息目标队列句柄
* @param[in]    ppMsg         :    需要发送的消息的指针(必须由MsgNewMsg分配, 不能是接收到的消息)
* @param[in]    bHighPrio    :    消息优先级
* @return         成功返回返回指针, 失败返回NULL
* @note         ppMsg调用后会置为NULL, 不能再使用;
                  也不要试图用其他变量备份ppMsg的指针值, 会产生系统异常;
                  返回的指针系统会自动释放,因此不要使用在其他地方
 ******************************************************************************
 */
ACK_T* msg_send_wait(HMQT hDst, MSG_T **ppMsg, BOOLEAN bHighPrio, uint32 nWaittime);

/**
 ******************************************************************************
* @brief         转发消息 
* @param[in]    hDst         :    转发消息目标队列句柄
* @param[in]    pMsg         :    需要转发的消息的指针(必须是接收到的消息)
* @return         成功返回TRUE, 失败返回FALSE 
* @note         转发的消息必须是收到的消息,而不是自己构建的
                   ppMsg调用后会置为NULL, 不能再使用;
 ******************************************************************************
 */
BOOLEAN msg_dispatch(HMQT hDst, const MSG_T **ppMsg);

/**
 ******************************************************************************
 * @brief         响应消息
 * @param[in]    pMsg         :    需要响应的消息
 * @param[in]    pRet         :    响应缓存指针(必须由MsgNewRetTlv分配, 不能是MsgSendWait返回的指针)
 * @return         成功返回TRUE, 失败返回FALSE
 * @note         ppRet调用后会置为NULL, 不能再使用;
                  也不要试图用其他变量备份ppRet的指针值, 会产生系统异常
 ******************************************************************************
 */
BOOLEAN msg_echo(const MSG_T *pMsg, ACK_T **ppRet);

/**
 ******************************************************************************
 * @brief         响应消息(错误信息)
 * @param[in]    pMsg         :    需要响应的消息
 * @param[in]    nErrCode     :    错误码
 * @return         成功返回TRUE, 失败返回FALSE
 * @note                 
 ******************************************************************************
 */
BOOLEAN msg_error_echo(const MSG_T *pMsg, int32 nErrCode);

/**
 ******************************************************************************
 * @brief         执行消息接收并处理
 * @param[in]    pHandler     :    消息处理回调函数指针
 * @param[in]    arg         :    回调函数参数
 * @param[in]    isBlock     :    是否阻塞, TRUE-没有收到会阻塞住,直到收到一个消息并调用回调函数后返回
                FALSE-不管有没有收到消息都会返回
 * @return         TRUE-收到一个消息并处理,FALSE-未收到消息
 * @note                 
 ******************************************************************************
 */
BOOLEAN msg_process(MSG_HANDLER pHandler, void *arg, BOOLEAN isBlock);

/**
 ******************************************************************************
 * @brief        判断消息是否应答
 * @param[in]    pMsg    ：    消息指针
 * @return         返回函数执行结果
 * @note                 
 ******************************************************************************
 */
BOOLEAN msg_is_ack(const MSG_T *pMsg);

/**
 ******************************************************************************
 * @brief         清除本线程缓存
 * @param[in]    none
 * @note         这个一般不需要手工调用,系统会自动执行;
                 当线程不调用MsgHandle时, 需要调用此函数来手工清除
 ******************************************************************************
 */
void msg_cache_clear(void);

#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H__ */
