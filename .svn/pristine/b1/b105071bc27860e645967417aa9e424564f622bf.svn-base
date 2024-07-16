/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： msgHandler.h
           消息处理接口头文件
* @date： 2019-11-9
*********************************************************************
*/

#ifndef __MSGSERVICE_H__
#define __MSGSERVICE_H__

#include "baseType.h"
#include "msgHandler.h"
#include "msgServiceBase.h"
#include "qlibc.h"
#include "threadpool.h"
#include "xlChannel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUB_MSG_TAG (0x00050040)

typedef enum _smiOS_connect_status
{
    CONNECT_INIT = 0,
    CONNECT_INIT_START,
    CONNECT_WAIT,
    LOGIN_START,
    LOGIN_WAIT_DONE,
    LOGIN_DONE,
    LOGOUT,
} SMIOS_CONNECT_STATUS_E;

/* @brief:消息服务结构体  */
typedef struct _msg_service
{
    threadpool_t *pool;                 //线程池
    pthread_t smiOSTid;
    uint32 threadNum;                   //线程数量
    uint32 queueSize;                   //任务队列最大值
    qhashtbl_t *tbl;                    //消息订阅哈希表//暂定
    MSG_TYPE_E connectType;             //以什么形式连接至smiOS
    sint8 smiOSName[MAX_APP_NAME_LEN];  //系统管理权名称
    sint8 smiOSIP[IP_LEN];              // sniOS IP地址
    sint16 smiOSUdpPort;                // smios UDP 端口
    SMIOS_CONNECT_STATUS_E status;      //连接至smiOS状态
    uint16 msgIndex;                    //消息索引
    XL_CHANNEL_CTX_T *ctx;              // TCP or IPC channle 句柄
    sint8 url[MAX_URL_LEN];

} MSG_SERVICE_T;

/* @brief:消息初始化系统接口  */
sint32 msg_init_system(void);

/* @brief:消息清理系统接口  */
void msg_clean_up_system(void);

/* @brief:消息启动系统接口  */
sint32 msg_start_system(void);

/* @brief:消息停止系统接口  */
void msg_stop_system(void);

/* @brief:请求消息接口  */
sint32 request_msg(MSG_HANDER_OBJ_T *handerObj);
/* @brief:  数据库初始化事件 */
void send_db_init_event(uint8 eventType, uint8 logicalDeviceNum, uint8 dataClassIDOption, uint16 dataClassID);
/* @brief: 数据更型事件  */
void send_data_update_event(uint8 logicalDeviceNum, uint16 infoNum, uint16 dataTypeID, uint8 majorDIOptinal, uint32 majorDI, uint32* minorDI, uint8 minorDI_num);

/* @brief:消息订阅接口 */  // 暂定
sint32 msg_subscribe(void *subscriber_name, uint8 did, uint16 *info, uint8 infoNum, uint32 *dataTag, uint8 dataTagNum);

#ifdef __cplusplus
}
#endif

#endif