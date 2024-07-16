/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   MQTT客户端接口
* @date：    2020/6/10
* @history： 
*********************************************************************
*/
#ifndef __MQTT_CLIENT_H_
#define __MQTT_CLIENT_H_


#ifdef __cplusplus
extern "C"
{
#endif

#define TOPIC_MAX_LEN   256
#define SUB_MAX_NUM     12

typedef enum _mqtt_client_status
{
    MQTT_CONNECT_LOST = 0,
    MQTT_CONNECT_SUCC
}MQTT_STATUS_E;

typedef struct tagMqttTopic
{
    char   topic[TOPIC_MAX_LEN];
    int    qos;
}SUB_TOPIC_T;

/**
*********************************************************************
* 预处理函数
* 入参: 消息内容, 消息长度
* 出参: 
* 返回：0-处理完成/其他-后续处理
*********************************************************************
*/
typedef int (*MQTT_PRE_PROC_FUNC)(void *, int);

typedef struct tagMqttClient
{
    int                  status;   //连接状态
    int                  topicnum; //订阅主题数量
    SUB_TOPIC_T          sub_topics[SUB_MAX_NUM]; //订阅的主题
    char                 default_send_topic[TOPIC_MAX_LEN]; //默认方发送主题
    int                  default_send_qos; //默认发送的QOS
    pthread_t            threadId; //MQTT消息接收线程句柄
    void                *pQueue;   //接收消息队列
    void                *pMosq;    //mosquito句柄
    MQTT_PRE_PROC_FUNC   prefunc;  //预处理函数
}MQTT_CLIENT_T;

/*
*********************************************************************
* @brief     : 创建mqtt客户端 
* @param[in] : char *id         客户端名称
* @param[out]: 
* @return    : MQTT_CLIENT_T *  客户端句柄 非0有效
*********************************************************************
*/
MQTT_CLIENT_T *mqtt_client_create(char *id);

/*
*********************************************************************
* @brief     : 设置客户端发送默认主题
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               char *topic              发送的主题
               int qos                  消息QOS
* @param[out]: 
* @return    : 0-成功/错误码
*********************************************************************
*/
int mqtt_client_set_default_send_topic(MQTT_CLIENT_T *pClient, char *topic, int qos);

/*
*********************************************************************
* @brief     : 设置客户端订阅的主题
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               SUB_TOPIC_T *topic       主题结构体数组
               int topicNum             主题数量
* @param[out]: 
* @return    : 0-成功/错误码
*********************************************************************
*/
int mqtt_client_set_sub_topic(MQTT_CLIENT_T *pClient, SUB_TOPIC_T *topic, int topicNum);

/*
*********************************************************************
* @brief     : 客户端链接服务端(执行一次, 内部主动维护链接状态)
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               char *host               客户端MQTT地址(IP地址或者网址)
               int port                 客户端MQTT端口
               char *local              本地网络地址(容器内APP填写NULL)
* @param[out]: 
* @return    : 0-成功/错误码
*********************************************************************
*/
int mqtt_client_connect(MQTT_CLIENT_T *pClient, char *host, int port, char *local);

/*
*********************************************************************
* @brief     : 获取客户端链接状态
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
* @param[out]: 
* @return    : MQTT_STATUS_E 链接状态
*********************************************************************
*/
MQTT_STATUS_E mqtt_client_get_status(MQTT_CLIENT_T *pClient);

/*
*********************************************************************
* @brief     : 客户端从服务器端断开链接
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
* @param[out]: 
* @return    : 
*********************************************************************
*/
void mqtt_client_disconnect(MQTT_CLIENT_T *pClient);

/*
*********************************************************************
* @brief     : mqtt客户端销毁
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
* @param[out]: 
* @return    : 
*********************************************************************
*/
void mqtt_client_destroy(MQTT_CLIENT_T *pClient);

/*
*********************************************************************
* @brief     : 客户端指定主题发送消息
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               char *topic              发送主题
               int qos                  使用QOS
               void *data               消息内容
               int dataLen              内容长度
* @param[out]: 
* @return    : 0-成功/错误码
*********************************************************************
*/
int mqtt_client_send_msg(MQTT_CLIENT_T *pClient, char *topic, int qos, void *data, int dataLen);

/*
*********************************************************************
* @brief     : 客户端使用默认主题发送消息
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               void *data               消息内容
               int dataLen              内容长度
* @param[out]: 
* @return    : 0-成功/错误码
*********************************************************************
*/
int mqtt_client_send(MQTT_CLIENT_T *pClient, void *data, int dataLen);

/*
*********************************************************************
* @brief     : 客户端接收消息(非阻塞)
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               void *data               消息内容
               int dataLen              内容长度
* @param[out]: 
* @return    : 消息长度(>=0)/错误码(<0)
*********************************************************************
*/
int mqtt_client_recv(MQTT_CLIENT_T *pClient, void *buf, int bufLen);

/*
*********************************************************************
* @brief     : 客户端接收消息(阻塞) 支持主题和超时时间
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               int topiclen             主题内容缓存长度
               int bufLen               消息内容缓存长度
               long msec                超时时间(ms 毫秒)
* @param[out]: char *topic              消息主题
               int *qos                 消息QOS
               void *buf                消息内容
* @return    : 消息长度(>=0)/错误码(<0)
*********************************************************************
*/
int mqtt_client_recv_msg(MQTT_CLIENT_T *pClient, char *topic, int topiclen, int *qos, void *buf, int bufLen, long msec);

/*
*********************************************************************
* @brief     : mqtt客户端调试内容获取
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               int   buflen             缓存内容, 大于等于1024
* @param[out]: char *buf                客户端调试内容
* @return    : 内容长度
*********************************************************************
*/
int mqtt_msg_handle_debug(MQTT_CLIENT_T *pClient, char *buf, int buflen);

/*
*********************************************************************
* @brief     : mqtt客户端预处理函数注册
* @param[in] : MQTT_CLIENT_T *pClient   客户端句柄
               MQTT_PRE_PROC_FUNC func  预处理函数
* @param[out]: 
* @return    : 0-处理完成/其他-后续处理(队列)
*********************************************************************
*/
int mqtt_msg_add_pre_porc(MQTT_CLIENT_T *pClient, MQTT_PRE_PROC_FUNC func);

#ifdef __cplusplus
}
#endif

#endif /* __APPMSG_SPLIT_H__ */
