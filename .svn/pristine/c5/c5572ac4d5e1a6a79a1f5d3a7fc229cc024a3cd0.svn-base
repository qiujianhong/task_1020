#ifndef _MQTT_SERVER_H__
#define _MQTT_SERVER_H__

#include <pthread.h>

#include "baseType.h"
// #include "constant.h"
#include "msgServiceBase.h"

#ifdef __cplusplus
extern "C" {
#endif

// MQTT重连间隔
#define RECONNECT_INTERVAL (5)
// MQTT重连最大间隔
#define RECONNECT_INTERVAL_MAX (15)
// MQTT SELECT 超时时间 单位毫秒
#define MOSQUITTO_SELECT_TIMEOUT (100)
// MQTT KEEP ALIVE 时间 单位秒
#define MOSQUITTO_KEEPALIVE_TIME (60)

typedef struct _mqtt_ctx
{
    MSG_OBJ_T reqMsg;     //请求消息头
    RAW_OBJ_T reqRawObj;  //请求信息对象
    MSG_OBJ_T rspMsg;     //回复消息头
    RAW_OBJ_T rspRawObj;  //回复信息对象
} MQTT_CTX_T;

/* @brief:mqtt通讯服务结构体封装  */
typedef struct _mqtt_server_obj_
{
    uint32            stop;          // MQTT服务的停止请求标志位
    uint16            serverPort;    // MQTT服务邦迪的服务端口
    sint8             serverIp[16];  // MQTT服务绑定的IP地址
    sint8             qos;  // MQTT服务绑定的IP地址
    sint8             userID[MAX_USER_ID_LEN];      // MQTT用户ID
    sint8             subTopicp[MAX_TOPIC_LEN];     //订阅的标题
    sint8             userName[MAX_USER_NAME_LEN];  // mqtt 登录用户名
    sint8             passwd[MAX_PASSWD_LEN];       // mqtt 登录密码
    pthread_t         tid;   // MQTT服务线程的ThreadIP
    struct mosquitto *mosq;  // mqtt mosquitto对象
} MQTT_SERVICE_T;

typedef struct _mqtt_server_sub_topic
{
    char topic[MAX_TOPIC_LEN];
    sint32 qos;
} MQTT_SERVER_SUB_TOPIC_T;

typedef void (*SUBSCRIBE_CB_FP)(MQTT_SERVER_SUB_TOPIC_T *subTopic[], sint32 *subNum);

/* @brief:MQTT服务初始化  */
sint32 mqtt_init_system(void);
/* @brief:MQTT服务启动  */
sint32 mqtt_start_system(void);
/* @brief:MQTT服务停止  */
void mqtt_stop_system(void);
/* @brief:MQTT消息发送  */
sint32 mqtt_sendto(sint8 *dest, void *data, uint32 dataLen);

void set_subscribe_cb_fp(SUBSCRIBE_CB_FP fp);

void mqtt_set_broker_ip(sint8 ip[IP_LEN]);
void mqtt_set_broker_port(sint16 port);
void mqtt_set_sub_qos(sint8 qos);
void mqtt_set_userID(sint8 username[MAX_USER_NAME_LEN]);
void mqtt_set_username(sint8 username[MAX_USER_NAME_LEN]);
void mqtt_set_password(sint8 passwd[MAX_PASSWD_LEN]);

#ifdef __cplusplus
}
#endif

#endif