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

#include <semaphore.h>

#include "baseType.h"
#include "constant.h"
#include "msgHelper.h"
#include "msgPqueue.h"
#include "msgService.h"
#include "msgServiceBase.h"
#include "qlibc.h"
#include "threadpool.h"
#include "xdrCustom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IP_RECORD_MAX_LENGTH (1536)

/* @brief:消息服务结构体  */
typedef struct _msg_service
{
    threadpool_t *dbTaskPool;     //线程池
    threadpool_t *otherTaskPool;  //线程池
    sem_t dbQueueSem;     // DB消息队列同步信号量
    sem_t otherQueueSem;  //其他消息队列同步信号量
    uint32 threadNum;     //线程数量
    uint32 queueSize;     //任务队列最大值
    PQUEUE_T *dbPq;       // DB优先级消息对列
    PQUEUE_T *otherPq;    // 其他优先级消息对列
    uint16 msgIndex;      //消息索引
} MSG_SERVICE_T;

extern  bool  setFlag;                  //设置标志
typedef struct _set_ip_info
{
    pthread_mutex_t     ipLock;
    uint8 type;                 //类型0-静态；1-DHCP
    char  nodeName[48];         //设备节点名
    uint8 ipVal[4];             //ip地址
    uint8 netMask[4];           //子网掩码
    uint8 gateWay[4];           //网关
    uint8 mac[6];               //MAC地址
}SET_IP_INFO_T;

typedef struct _msg_data
{
    sint32 pid;
    APP_SUB_INFO_RAW_T subscribeInfo;
    char version[200];
    MSG_CLIENT_T msgClient;
    uint8 appid[APPID_LEN];
    uint32 appidlen;
} MSG_DATE_T;

typedef struct 
{
    char name[100];
    uint8 mac[6];
    uint8 power;
    uint16 broadperiod;
    uint16 scanperiod;
}BLUE_TOOTH_PARAM_T;



extern int          rebootflag ;            //系统重启标志
extern SET_IP_INFO_T    setIpInfo ;         //ip设置信息

/* @brief:消息初始化系统接口  */
sint32 msg_init_system(void);

/* @brief:消息清理系统接口  */
void msg_clean_up_system(void);

/* @brief:消息启动系统接口  */
sint32 msg_start_system(void);

/* @brief:消息停止系统接口  */
void msg_stop_system(void);

/* @brief:心跳请求发送统接口  */
sint32 send_heartbeat_req_to_client(MSG_CLIENT_T *client);



sint8 * get_service_name_by_client(MSG_CLIENT_T *client);

/* @brief:事件发送接口  */
sint32
send_general_event_to_client(MSG_CLIENT_T *client, GENEERAL_EVENT_E event, uint8 *eventPayload, uint32 payloadLen);

int SendParaInitToDB(void);
sint32 send_bluetooth_param_query(MSG_CLIENT_T *client);
sint32 send_iotManager_container_query(MSG_CLIENT_T *client);
sint32 send_iotManager_app_query(MSG_CLIENT_T *client);
void app_get_maketime(char *ptime, OOP_DATETIME_S_T *pTime);

int act_set_ip(uint8 bEvent);
int act_set_mac(void);
int load_ip_info(SET_IP_INFO_T *ipinfo);
int local_mac_info_save(void);

#ifdef __cplusplus
}
#endif

#endif