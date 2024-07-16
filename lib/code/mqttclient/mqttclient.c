/*
*********************************************************************
* @filename: mqttclient.c
* @brief   : 消息通信 DB操作
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2019-12-18    create
*********************************************************************
*/
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "mosquitto.h"
#include "mqtt_client.h"

typedef struct tagmqtt_msg
{
    void *pNext;
    int   mid;
    char  topic[TOPIC_MAX_LEN];
    int   qos;
    int   payloadlen;
    char  payload[0];
}MQTT_MESSAGE_T;

typedef struct tagMqttQueue
{
    pthread_mutex_t    lock;      //操作锁
    pthread_cond_t     notify;    //条件变量
    unsigned long long recvnum;   //接收数量
    unsigned long long procnum;   //处理数量
    unsigned long long dropnum;   //丢弃数量

    unsigned long long sendnum;   //发送数量
    unsigned long long sendfailnum;//发送数量失败

    unsigned int       msgnum;    //队列数量
    MQTT_MESSAGE_T    *head;      //队列头
    MQTT_MESSAGE_T    *tail;      //队列尾
}MQTT_CLIENT_QUEUE_T;

/**********************************************************************
* @name      : mqtt_msg_callback
* @brief     ：mqtt接收消息处理
* @param[in] ：mosquitto *mosq             句柄
               void *obj                   私有数据(mosquitto_new的第三个入参)
               mosquitto_message *message  消息
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
void mqtt_msg_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    MQTT_CLIENT_T *pClient = (MQTT_CLIENT_T *)obj;
    MQTT_CLIENT_QUEUE_T *pQueue = (MQTT_CLIENT_QUEUE_T *)pClient->pQueue;
    MQTT_MESSAGE_T *pNewMsg = NULL;
    MQTT_MESSAGE_T *pTempMsg = NULL;
    static int mqttmsglen = sizeof(MQTT_MESSAGE_T);
    int topiclen = strlen(message->topic);

    pQueue->recvnum++;

    /* 先执行预处理 处理成功直接返回 */
    if(message->payloadlen > 0 && NULL != pClient->prefunc)
    {
        if(0 == pClient->prefunc(message->payload, message->payloadlen))
        {
            return;
        }
    }
    
    /* 申请内存存储MQTT消息 */
    pNewMsg = (MQTT_MESSAGE_T *)malloc(mqttmsglen + message->payloadlen);
    if(NULL == pNewMsg)
    {
        pQueue->dropnum++;
        printf("Malloc failed\n");
        return;
    }
    memset(pNewMsg, 0, mqttmsglen);

    pNewMsg->mid = message->mid;
    pNewMsg->qos = message->qos;
    if(topiclen >= TOPIC_MAX_LEN)
    {
        topiclen = TOPIC_MAX_LEN - 1;
    }
    memcpy(pNewMsg->topic, message->topic, topiclen);
    memset(message->topic, 0, topiclen);
    pNewMsg->payloadlen = message->payloadlen;
    if(message->payloadlen > 0)
    {
        memcpy(pNewMsg->payload, message->payload, message->payloadlen);
    }

    /* 将MQTT消息放入接收队列pQueue */
    pthread_mutex_lock(&(pQueue->lock));

    if(NULL == pQueue->head)
    {
        pQueue->head = pNewMsg;
    }

    if(NULL != pQueue->tail)
    {
        pTempMsg = (MQTT_MESSAGE_T *)pQueue->tail;
        pTempMsg->pNext = pNewMsg;
    }
    pQueue->tail = pNewMsg;
    pQueue->msgnum += 1;

    pthread_cond_signal(&(pQueue->notify)); //设置信号量, 通知接收函数数据准备好
    pthread_mutex_unlock(&(pQueue->lock));
}

/**********************************************************************
* @name      : mqtt_collect_thread
* @brief     ：mqtt客户端接收消息, 服务端连接维护
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
void *mqtt_collect_thread(void *pArg)
{
    int ret = 0;
    MQTT_CLIENT_T *pClient = (MQTT_CLIENT_T *)pArg;
    int i = 0;

    while(1)
    {
        ret = mosquitto_loop(pClient->pMosq, -1, 1);
        if(MOSQ_ERR_SUCCESS == ret)
        {
            pClient->status = MQTT_CONNECT_SUCC;
        }
        else
        {
            pClient->status = MQTT_CONNECT_LOST;
            printf("Mqtt service lost, retconnect\n");
            sleep(2);
            if(MOSQ_ERR_SUCCESS == mosquitto_reconnect(pClient->pMosq))
            {
                printf("Mqtt service lost, retconnect success\n");
                /* 连接成功需要重新订阅 */
                for(i = 0; i < pClient->topicnum; i++)
                {
                    mosquitto_subscribe(pClient->pMosq, NULL, pClient->sub_topics[i].topic, pClient->sub_topics[i].qos);
                }
            }
        }
    }
    return 0;
}

/**********************************************************************
* @name      : mqtt_client_create
* @brief     ：创建mqtt客户端
* @param[in] ：char *id      名称
               char *host    主机地址
               int port      端口
* @param[out]：
* @return    ：MQTT_CLIENT_T 句柄
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
MQTT_CLIENT_T *mqtt_client_create(char *id)
{
    int ret = 0;
    MQTT_CLIENT_T *pNewClient = NULL;
    struct mosquitto *mosq = NULL;
    MQTT_CLIENT_QUEUE_T *pQueue = NULL;
    pthread_condattr_t  tAttr;

    /* mosquitto动态库初始化 */
    ret = mosquitto_lib_init();
    if(MOSQ_ERR_SUCCESS != ret)
    {
        return NULL;
    }

    pNewClient = (MQTT_CLIENT_T *)malloc(sizeof(MQTT_CLIENT_T));
    if(NULL == pNewClient)
    {
        return NULL;
    }
    memset(pNewClient, 0, sizeof(MQTT_CLIENT_T));

    /* MQTT接收缓存队列 */
    pQueue = (MQTT_CLIENT_QUEUE_T *)malloc(sizeof(MQTT_CLIENT_QUEUE_T));
    if(NULL == pQueue)
    {
        free(pNewClient);
        return NULL;
    }
    memset(pQueue, 0, sizeof(MQTT_CLIENT_QUEUE_T));
    pthread_mutex_init(&(pQueue->lock), NULL);
    /* pthread_cond_timedwait相对时间模式 */
    pthread_condattr_setclock(&tAttr, CLOCK_MONOTONIC);
    pthread_cond_init(&(pQueue->notify), &tAttr);
    pQueue->head = NULL;
    pQueue->tail = NULL;

    /* 创建MQTT客户端 */
    mosq = mosquitto_new(id, true, pNewClient);
    if(NULL == mosq)
    {
        mosquitto_lib_cleanup();
        free(pQueue);
        free(pNewClient);
        return NULL;
    }
    pNewClient->pQueue = pQueue;
    pNewClient->pMosq = mosq;
    pNewClient->status = MQTT_CONNECT_LOST;
    pNewClient->prefunc = NULL;

    /* 注册MQTT接收消息处理函数mqtt_msg_callback */
    mosquitto_message_callback_set(mosq, mqtt_msg_callback);
    return pNewClient;
}

/**********************************************************************
* @name      : mqtt_client_set_sub_topic
* @brief     ：设置订阅主题
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
               char *topic            主题
               int qos
* @param[out]：
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
int mqtt_client_set_default_send_topic(MQTT_CLIENT_T *pClient, char *topic, int qos)
{
    int len = strlen(topic);
    if(len + 1 > TOPIC_MAX_LEN)
    {
        return MOSQ_ERR_NOT_SUPPORTED;
    }

    memcpy(pClient->default_send_topic, topic, len);
    pClient->default_send_qos = qos;

    return MOSQ_ERR_SUCCESS;
}

/**********************************************************************
* @name      : mqtt_client_set_sub_topic
* @brief     ：设置订阅主题
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
               SUB_TOPIC_T *topic     主题结构体数组
               int topicNum           主题数量
* @param[out]：
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
int mqtt_client_set_sub_topic(MQTT_CLIENT_T *pClient, SUB_TOPIC_T *topic, int topicNum)
{
    int i = 0;

    if(topicNum > SUB_MAX_NUM)
    {
        return MOSQ_ERR_INVAL;
    }

    pClient->topicnum = topicNum;

    for(i = 0;i < topicNum; i++)
    {
        memcpy(pClient->sub_topics[i].topic, topic[i].topic, TOPIC_MAX_LEN);
        pClient->sub_topics[i].qos = topic[i].qos;
    }

    return MOSQ_ERR_SUCCESS;
}

/**********************************************************************
* @name      : mqtt_client_connect
* @brief     ：mqtt客户端连接服务端
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
               char *host             主机地址
               int port               端口
               char *local            本地网络地址(可以为NULL)
* @param[out]：
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    : 2020-8-1
**********************************************************************/
int mqtt_client_connect(MQTT_CLIENT_T *pClient, char *host, int port, char *local)
{
    int ret = 0;
    int i = 0;

    if(NULL == pClient || NULL == pClient->pMosq)
    {
        return MOSQ_ERR_CONN_PENDING;
    }

    if(NULL == local)
    {
        ret = mosquitto_connect(pClient->pMosq, host, port, 60);
    }
    else
    {
        ret = mosquitto_connect_bind(pClient->pMosq, host, port, 60, local);
    }
    if(MOSQ_ERR_SUCCESS != ret)
    {
        printf("mosquitto_connect failed: %s\n", strerror(errno));
    }
    else
    {
        for(i = 0; i < pClient->topicnum; i++)
        {
            mosquitto_subscribe(pClient->pMosq, NULL, pClient->sub_topics[i].topic, pClient->sub_topics[i].qos);
        }
    }

    ret = pthread_create(&pClient->threadId, NULL, mqtt_collect_thread, pClient);
    if (ret != 0)
    {
        pClient->threadId = 0;
        return MOSQ_ERR_UNKNOWN;
    }
    return MOSQ_ERR_SUCCESS;
}

/**********************************************************************
* @name      : mqtt_client_get_status
* @brief     ：获取连接状态
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
* @param[out]：
* @return    ：连接状态
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
MQTT_STATUS_E mqtt_client_get_status(MQTT_CLIENT_T *pClient)
{
    if(NULL == pClient)
    {
        return MQTT_CONNECT_LOST;
    }

    return pClient->status;
}

/**********************************************************************
* @name      : mqtt_client_disconnect
* @brief     ：mqtt客户端断开连接
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
void mqtt_client_disconnect(MQTT_CLIENT_T *pClient)
{
    if(NULL == pClient || NULL == pClient->pMosq)
    {
        return;
    }

    if(0 != pClient->threadId)
    {
        pthread_cancel(pClient->threadId);
        pthread_join(pClient->threadId, NULL);
        pClient->threadId = 0;
    }
    if(NULL != pClient->pQueue)
    {
        MQTT_CLIENT_QUEUE_T *pQueue = (MQTT_CLIENT_QUEUE_T *)pClient->pQueue;
        MQTT_MESSAGE_T *pTempMsg = NULL;
        MQTT_MESSAGE_T *pMsg = pQueue->head;
        while(pMsg)
        {
            pTempMsg = pMsg;
            pMsg = (MQTT_MESSAGE_T *)pTempMsg->pNext;
            free(pTempMsg);
        }
        pQueue->head = NULL;
        pQueue->tail = NULL;
        pQueue->msgnum = 0;
    }

    mosquitto_disconnect(pClient->pMosq);
}

/**********************************************************************
* @name      : mqtt_client_destroy
* @brief     ：mqtt客户端销毁
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
void mqtt_client_destroy(MQTT_CLIENT_T *pClient)
{
    if(NULL == pClient)
    {
        return;
    }

    if(0 != pClient->threadId)
    {
        pthread_cancel(pClient->threadId);
        pthread_join(pClient->threadId, NULL);
        pClient->threadId = 0;
    }
    if(NULL != pClient->pMosq)
    {
        mosquitto_destroy(pClient->pMosq);
    }
    mosquitto_lib_cleanup();

    if(NULL != pClient->pQueue)
    {
        MQTT_CLIENT_QUEUE_T *pQueue = (MQTT_CLIENT_QUEUE_T *)pClient->pQueue;
        MQTT_MESSAGE_T *pTempMsg = NULL;
        MQTT_MESSAGE_T *pMsg = pQueue->head;
        while(pMsg)
        {
            pTempMsg = pMsg;
            pMsg = (MQTT_MESSAGE_T *)pTempMsg->pNext;
            free(pTempMsg);
        }
        free(pQueue);
    }

    free(pClient);
}

/**********************************************************************
* @name      : mqtt_client_send_msg
* @brief     ：mqtt客户端带主题发送
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
               char *topic            发送主题
               int qos                发送qos
               void *data             发送数据
               int dataLen            长度
* @param[out]：
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
int mqtt_client_send_msg(MQTT_CLIENT_T *pClient, char *topic, int qos, void *data, int dataLen)
{
    if(NULL == pClient || NULL == pClient->pMosq || NULL == topic || NULL == data || dataLen <= 0)
    {
        return MOSQ_ERR_INVAL;
    }

    if(MQTT_CONNECT_SUCC != pClient->status)
    {
        return MOSQ_ERR_CONN_LOST;
    }
    int mid = 0;
    int ret = 0;
    MQTT_CLIENT_QUEUE_T *pQueue = (MQTT_CLIENT_QUEUE_T *)pClient->pQueue;

    ret = mosquitto_publish(pClient->pMosq, &mid, topic, dataLen, data, qos, 0);
    if(MOSQ_ERR_SUCCESS == ret)
    {
        pQueue->sendnum++;
    }
    else
    {
        pQueue->sendfailnum++;
    }
    return ret;
}

/**********************************************************************
* @name      : mqtt_client_send
* @brief     ：mqtt客户端使用默认主题发送
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
               void *data             发送数据
               int dataLen            长度
* @param[out]：
* @return    ：错误码
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
int mqtt_client_send(MQTT_CLIENT_T *pClient, void *data, int dataLen)
{
    if(NULL == pClient || NULL == pClient->pMosq || NULL == data || dataLen <= 0)
    {
        return MOSQ_ERR_INVAL;
    }

    if(0 == pClient->default_send_topic[0])
    {
        return MOSQ_ERR_NOT_SUPPORTED;
    }

    return mqtt_client_send_msg(pClient, pClient->default_send_topic, pClient->default_send_qos, data, dataLen);
}

/**********************************************************************
* @name      : mqtt_client_recv
* @brief     ：mqtt客户端接收消息
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
               int bufLen             缓冲区长度
* @param[out]：void *buf              缓冲区
* @return    ：消息长度/错误码
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
int mqtt_client_recv(MQTT_CLIENT_T *pClient, void *buf, int bufLen)
{
    MQTT_CLIENT_QUEUE_T *pQueue = (MQTT_CLIENT_QUEUE_T *)pClient->pQueue;
    MQTT_MESSAGE_T *pMsg = NULL;
    int len = 0;

    if(NULL == pClient || NULL == pQueue || NULL == buf || bufLen <= 0)
    {
        return -1;
    }

    if(NULL == pQueue->head)
    {
        return 0;
    }

    pthread_mutex_lock(&(pQueue->lock));
    pMsg = pQueue->head;
    if(NULL == pMsg)
    {
        pthread_mutex_unlock(&(pQueue->lock));
        return 0;
    }

    pQueue->head = (MQTT_MESSAGE_T *)pMsg->pNext;
    if(NULL == pQueue->head)
    {
        pQueue->tail = NULL;
        pQueue->msgnum = 0;
    }
    if(pQueue->msgnum > 0)
    {
        pQueue->msgnum -= 1;
    }
    pQueue->procnum++;
    pthread_mutex_unlock(&(pQueue->lock));

    len = bufLen < pMsg->payloadlen ? bufLen : pMsg->payloadlen;
    memcpy(buf,  pMsg->payload, len);
    free(pMsg);
    return len;
}

/**********************************************************************
* @name      : mqtt_client_recv_msg
* @brief     ：mqtt客户端接收消息, 支持主题和超时时间
* @param[in] ：MQTT_CLIENT_T *pClient 客户端句柄
               void *data             发送数据
               int bufLen             缓冲区长度
               long msec              超时时间ms
* @param[out]：char *topic            主题缓冲器
               void *buf              消息缓冲区
* @return    ：消息长度/错误码
* @Create    : 王津剑
* @Date      ：2020-6-10
* @Update    :
**********************************************************************/
int mqtt_client_recv_msg(MQTT_CLIENT_T *pClient, char *topic, int topiclen, int *qos, void *buf, int bufLen, long msec)
{
    MQTT_CLIENT_QUEUE_T *pQueue = (MQTT_CLIENT_QUEUE_T *)pClient->pQueue;
    MQTT_MESSAGE_T *pMsg = NULL;
    int len = 0;
    struct timespec outtime;
    long nsec = 0;

    if(NULL == pClient || NULL == pQueue || NULL == topic || NULL == buf || bufLen <= 0)
    {
        return -1;
    }

    pthread_mutex_lock(&(pQueue->lock));
    pMsg = pQueue->head;
    if(NULL != pMsg)
    {
        /* 已经存在消息 不需要等待 */
        goto msgcopy;
    }
    else if(msec <= 0)
    {
        pthread_mutex_unlock(&(pQueue->lock));
        return 0;
    }

    /* 不存在消息设置超时等待 */
    clock_gettime(CLOCK_MONOTONIC, &outtime);
    outtime.tv_sec += msec/1000;
    nsec = outtime.tv_nsec + (msec % 1000L) * 1000000L;
    if(nsec > 1000000000L)
    {
        outtime.tv_sec += 1;
        outtime.tv_nsec = nsec - 1000000000L;
    }
    else
    {
        outtime.tv_nsec = nsec;
    }
    pthread_cond_timedwait(&pQueue->notify, &pQueue->lock, &outtime);

    pMsg = pQueue->head;
    if(NULL == pMsg)
    {
        pthread_mutex_unlock(&(pQueue->lock));
        return 0;
    }

msgcopy:
    pQueue->head = (MQTT_MESSAGE_T *)pMsg->pNext;
    if(NULL == pQueue->head)
    {
        pQueue->tail = NULL;
        pQueue->msgnum = 0;
    }
    if(pQueue->msgnum > 0)
    {
        pQueue->msgnum -= 1;
    }
    pQueue->procnum++;
    pthread_mutex_unlock(&(pQueue->lock));
    
    len = bufLen < pMsg->payloadlen ? bufLen : pMsg->payloadlen;
    memcpy(buf, pMsg->payload, len);

    *qos = pMsg->qos;
    memcpy(topic, pMsg->topic, topiclen < strlen(pMsg->topic) ? topiclen : strlen(pMsg->topic));

    free(pMsg);
    return len;
}

/**********************************************************************
* @name      : mqtt_msg_handle_debug
* @brief     ：mqtt客户端调试内容获取
* @param[in] ：MQTT_CLIENT_T *pClient  客户端句柄
               int   buflen            缓存内容, 建议大于等于1024
* @param[out]：char *buf               客户端调试内容
* @return    ：内容长度
* @Create    : 王津剑
* @Date      ：2020-7-3
* @Update    :
**********************************************************************/
int mqtt_msg_handle_debug(MQTT_CLIENT_T *pClient, char *buf, int buflen)
{
    int offset = 0;
    int i = 0;
    MQTT_CLIENT_QUEUE_T *ptQueue = NULL;

    if(buflen < 1024 || NULL == pClient || NULL == buf)
    {
        return 0;
    }

    offset += sprintf(buf + offset, "Status      : %s\n", MQTT_CONNECT_SUCC == pClient->status ? "connect" : "lost");
    offset += sprintf(buf + offset, "SubTopic    :");
    for(i = 0; i < pClient->topicnum; i++)
    {
        offset += sprintf(buf + offset, " %s", pClient->sub_topics[i].topic);
    }
    offset += sprintf(buf + offset, "\n");

    offset += sprintf(buf + offset, "DefaultTopic: %s, %d\n", pClient->default_send_topic, pClient->default_send_qos);
    offset += sprintf(buf + offset, "Mqtt Hancle : %p\n", pClient->pMosq);
    offset += sprintf(buf + offset, "Msg Queue   : %p\n", pClient->pQueue);
    ptQueue = pClient->pQueue;
    if(NULL == ptQueue)
    {
        return offset;
    }
    pthread_mutex_lock(&(ptQueue->lock));
    offset += sprintf(buf + offset, "  Msg head  : %p\n", ptQueue->head);
    offset += sprintf(buf + offset, "  Msg tail  : %p\n", ptQueue->tail);
    offset += sprintf(buf + offset, "  Msg recv  : queue %u, recv %llu, proc %llu, drop %llu\n", 
                      ptQueue->msgnum, ptQueue->recvnum, ptQueue->procnum, ptQueue->dropnum);
    offset += sprintf(buf + offset, "  Msg send  : %llu, faile %llu\n", ptQueue->sendnum, ptQueue->sendfailnum);
    pthread_mutex_unlock(&(ptQueue->lock));

    return offset;
}

/**********************************************************************
* @name      : mqtt_msg_add_pre_porc
* @brief     ：mqtt客户端预处理函数
* @param[in] ：MQTT_CLIENT_T *pClient  客户端句柄
               MQTT_PRE_PROC_FUNC func 预处理函数
* @param[out]：
* @return    ：0-处理完成/其他-后续处理(队列)
* @Create    : 王津剑
* @Date      ：2020-10-26
* @Update    :
**********************************************************************/
int mqtt_msg_add_pre_porc(MQTT_CLIENT_T *pClient, MQTT_PRE_PROC_FUNC func)
{
    if(NULL == pClient)
    {
        return MOSQ_ERR_CONN_PENDING;
    }
    pClient->prefunc = func;
    return MOSQ_ERR_SUCCESS;
}

