/*
*********************************************************************
* @filename: mqttclient.c
* @brief   : ��Ϣͨ�� DB����
* @describe: 
* @history: 
*    auther      date          describe
*    ����      2019-12-18    create
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
    pthread_mutex_t    lock;      //������
    pthread_cond_t     notify;    //��������
    unsigned long long recvnum;   //��������
    unsigned long long procnum;   //��������
    unsigned long long dropnum;   //��������

    unsigned long long sendnum;   //��������
    unsigned long long sendfailnum;//��������ʧ��

    unsigned int       msgnum;    //��������
    MQTT_MESSAGE_T    *head;      //����ͷ
    MQTT_MESSAGE_T    *tail;      //����β
}MQTT_CLIENT_QUEUE_T;

/**********************************************************************
* @name      : mqtt_msg_callback
* @brief     ��mqtt������Ϣ����
* @param[in] ��mosquitto *mosq             ���
               void *obj                   ˽������(mosquitto_new�ĵ��������)
               mosquitto_message *message  ��Ϣ
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-10
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

    /* ��ִ��Ԥ���� ����ɹ�ֱ�ӷ��� */
    if(message->payloadlen > 0 && NULL != pClient->prefunc)
    {
        if(0 == pClient->prefunc(message->payload, message->payloadlen))
        {
            return;
        }
    }
    
    /* �����ڴ�洢MQTT��Ϣ */
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

    /* ��MQTT��Ϣ������ն���pQueue */
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

    pthread_cond_signal(&(pQueue->notify)); //�����ź���, ֪ͨ���պ�������׼����
    pthread_mutex_unlock(&(pQueue->lock));
}

/**********************************************************************
* @name      : mqtt_collect_thread
* @brief     ��mqtt�ͻ��˽�����Ϣ, ���������ά��
* @param[in] ��void *pArg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-10
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
                /* ���ӳɹ���Ҫ���¶��� */
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
* @brief     ������mqtt�ͻ���
* @param[in] ��char *id      ����
               char *host    ������ַ
               int port      �˿�
* @param[out]��
* @return    ��MQTT_CLIENT_T ���
* @Create    : ����
* @Date      ��2020-6-10
* @Update    :
**********************************************************************/
MQTT_CLIENT_T *mqtt_client_create(char *id)
{
    int ret = 0;
    MQTT_CLIENT_T *pNewClient = NULL;
    struct mosquitto *mosq = NULL;
    MQTT_CLIENT_QUEUE_T *pQueue = NULL;
    pthread_condattr_t  tAttr;

    /* mosquitto��̬���ʼ�� */
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

    /* MQTT���ջ������ */
    pQueue = (MQTT_CLIENT_QUEUE_T *)malloc(sizeof(MQTT_CLIENT_QUEUE_T));
    if(NULL == pQueue)
    {
        free(pNewClient);
        return NULL;
    }
    memset(pQueue, 0, sizeof(MQTT_CLIENT_QUEUE_T));
    pthread_mutex_init(&(pQueue->lock), NULL);
    /* pthread_cond_timedwait���ʱ��ģʽ */
    pthread_condattr_setclock(&tAttr, CLOCK_MONOTONIC);
    pthread_cond_init(&(pQueue->notify), &tAttr);
    pQueue->head = NULL;
    pQueue->tail = NULL;

    /* ����MQTT�ͻ��� */
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

    /* ע��MQTT������Ϣ������mqtt_msg_callback */
    mosquitto_message_callback_set(mosq, mqtt_msg_callback);
    return pNewClient;
}

/**********************************************************************
* @name      : mqtt_client_set_sub_topic
* @brief     �����ö�������
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
               char *topic            ����
               int qos
* @param[out]��
* @return    ��������
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     �����ö�������
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
               SUB_TOPIC_T *topic     ����ṹ������
               int topicNum           ��������
* @param[out]��
* @return    ��������
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     ��mqtt�ͻ������ӷ����
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
               char *host             ������ַ
               int port               �˿�
               char *local            ���������ַ(����ΪNULL)
* @param[out]��
* @return    ��������
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     ����ȡ����״̬
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
* @param[out]��
* @return    ������״̬
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     ��mqtt�ͻ��˶Ͽ�����
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     ��mqtt�ͻ�������
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     ��mqtt�ͻ��˴����ⷢ��
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
               char *topic            ��������
               int qos                ����qos
               void *data             ��������
               int dataLen            ����
* @param[out]��
* @return    ��������
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     ��mqtt�ͻ���ʹ��Ĭ�����ⷢ��
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
               void *data             ��������
               int dataLen            ����
* @param[out]��
* @return    ��������
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     ��mqtt�ͻ��˽�����Ϣ
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
               int bufLen             ����������
* @param[out]��void *buf              ������
* @return    ����Ϣ����/������
* @Create    : ����
* @Date      ��2020-6-10
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
* @brief     ��mqtt�ͻ��˽�����Ϣ, ֧������ͳ�ʱʱ��
* @param[in] ��MQTT_CLIENT_T *pClient �ͻ��˾��
               void *data             ��������
               int bufLen             ����������
               long msec              ��ʱʱ��ms
* @param[out]��char *topic            ���⻺����
               void *buf              ��Ϣ������
* @return    ����Ϣ����/������
* @Create    : ����
* @Date      ��2020-6-10
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
        /* �Ѿ�������Ϣ ����Ҫ�ȴ� */
        goto msgcopy;
    }
    else if(msec <= 0)
    {
        pthread_mutex_unlock(&(pQueue->lock));
        return 0;
    }

    /* ��������Ϣ���ó�ʱ�ȴ� */
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
* @brief     ��mqtt�ͻ��˵������ݻ�ȡ
* @param[in] ��MQTT_CLIENT_T *pClient  �ͻ��˾��
               int   buflen            ��������, ������ڵ���1024
* @param[out]��char *buf               �ͻ��˵�������
* @return    �����ݳ���
* @Create    : ����
* @Date      ��2020-7-3
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
* @brief     ��mqtt�ͻ���Ԥ������
* @param[in] ��MQTT_CLIENT_T *pClient  �ͻ��˾��
               MQTT_PRE_PROC_FUNC func Ԥ������
* @param[out]��
* @return    ��0-�������/����-��������(����)
* @Create    : ����
* @Date      ��2020-10-26
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

