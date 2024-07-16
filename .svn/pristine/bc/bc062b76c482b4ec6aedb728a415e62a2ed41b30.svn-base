/*
*********************************************************************
* Copyright(C) 2018 China Electric Power Research Institute
* All rights reserved. 
* @brief：   消息驱动接口
* @date：    2018/09/10
* @history： 
*********************************************************************
*/

#ifndef __MSB_APP_H__
#define __MSB_APP_H__

#include "msg.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MQT_NAME_LEN             64           /**< 最大名称长度            */
#define MQT_DEFAULT_QUELEN         0x2000         /**< 缺省消息队列长度        */
#define MQT_DEFAULT_READLEN     4000         /**< 缺省读取缓存长度        */

/** @brief 组件版本信息    */
typedef union 
{
    uint32             number;
    struct 
    {
        uint32        build      : 8;            /**< Build号                */
        uint32         evolution : 8;             /**< 演进版本号                */
        uint32         expansion : 8;             /**< 扩展版本号                */
        uint32         major     : 8;             /**< 主版本号                */
    };
} VERSION_T;

/**
 ******************************************************************************
 * @brief    组件初始化配置    
 * @note     组件名称规则：
 *            if( name[0] )
 *            {
 *                '_'    :    -> Base inner thread    exp:"_monitor"
 *                '/'    :    -> Base TCP-Socket        exp:"/db"
 *                '$'    :    -> Base UDP-Socket        exp:"$ppdc:192.168.8.2[9000]"
 *            }
 *            else
 *            {
 *                        -> Base IPC                exp:"smiOS"
 *            }
 *            消息中心：smiOS:192.168.8.1[8000][8001] -- [TCP端口][UDP端口]
 ******************************************************************************
 */
typedef struct
{
    char             myName[MQT_NAME_LEN];     /**< 本组件名称                    */
    char            mcName[MQT_NAME_LEN];    /**< 消息中心名称                */
    uint32             eventMask;                 /**< 订阅事件                    */
    uint32             msgQueLen;                 /**< 消息队列长度(0为缺省)        */
    uint32             msgReadLen;             /**< 消息读取缓存长度(0为缺省)    */
    VERSION_T         version;                 /**< 版本号                        */
} MQT_PROFILE_T;

/**
 ******************************************************************************
 * @brief         初始化组件配置
 * @param[in]    pProfile    :    组件配置参数
 * @return         成功返回TRUE, 失败返回FALSE
 * @note                     
 ******************************************************************************
 */
BOOLEAN app_profile_init(const MQT_PROFILE_T *pProfile);

/**
 ******************************************************************************
 * @brief         生成带有消息队列的线程并自动接收消息
 * @param[in]    pName             :    消息队列/线程名称
 * @param[in]    nQueSize        :    队列缓存大小(为0表示只能接收事件)
 * @param[in]    pHandler         :    消息处理回调函数指针
 * @param[in]    arg             :    消息处理回调函数参数
 * @param[in]    subEventMask    :     需要订阅的事件掩码
 * @return         成功返回TRUE, 失败返回FALSE
 * @note         线程超时固定为1小时
 ******************************************************************************
 */
BOOLEAN app_mqt_create(const char *pName, uint32 nQueSize, MSG_HANDLER pHandler, void *arg, uint32 subEventMask);

/**
 ******************************************************************************
 * @brief         生成带有消息队列的线程(需手工接收消息)
 * @param[in]    pName             :    消息队列/线程名称
 * @param[in]    nQueSize        :    队列缓存大小(为0表示只能接收事件)
 * @param[in]    pHandler         :    消息处理回调函数指针
 * @param[in]    arg             :    消息处理回调函数参数
 * @param[in]    subEventMask    :     需要订阅的事件掩码
 * @return         成功返回TRUE, 失败返回FALSE
 * @note         线程超时固定为1小时
 ******************************************************************************
 */
BOOLEAN app_mqt_mix_create(const char *pName, uint32 nQueSize, void* (*pHandler)(void*), void *arg, uint32 subEventMask);

/**
 ******************************************************************************
 * @brief         查询消息队列句柄
 * @param[in]    pName        :    消息队列名称
 * @return         成功返回消息队列句柄, 失败返回NULL
 * @note                 
 ******************************************************************************
 */
HMQT app_mqt_find(const char *pName);

/**
 ******************************************************************************
 * @brief         获取本线程的句柄
 * @param[in]     none
 * @return         成功返回实际句柄, 失败返回NULL
 * @note                 
 ******************************************************************************
 */
HMQT app_mqt_self(void);

/**
 ******************************************************************************
 * @brief         获取本进程的句柄
 * @param[in]    none
 * @return         成功返回实际句柄, 失败返回NULL
 * @note                 
 ******************************************************************************
 */
HMQT app_self(void);

/**
 ******************************************************************************
 * @brief         开始运行客户端组件(除非组件退出或异常不会返回)
 * @param[in]    pHandler    :    消息处理回调函数
 * @param[in]    arg         :    回调函数参数
 * @return         none
 * @note         该函数会阻塞不会退出
 ******************************************************************************
 */
void app_run(MSG_HANDLER pHandler, void *arg);

/**
 ******************************************************************************
 * @brief         请求退出
 * @param[in]    none
 * @return         none
 * @note                 
 ******************************************************************************
 */
void app_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* __MSB_APP_H__ */
