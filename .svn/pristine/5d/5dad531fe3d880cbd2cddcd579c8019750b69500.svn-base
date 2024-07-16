/*
*********************************************************************
* Copyright(C) 2020南京新联电子股份有限公司
* All rights reserved.
* @brief： mqttClient.c
* @date： 2020-6-10
*********************************************************************
*/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "baseConstant.h"
#include "baseType.h"
#include "channelStatus.h"
#include "mosquitto.h"
#include "mqttClient.h"
#include "qlibc.h"
#include "xlClient.h"
#include "xlDebug.h"
#include "socketHelper.h"

//接收到的包结构
typedef struct _recv_package
{
    uint32 payloadLen;
    uint32 mid;
    char   topic[MAX_TOPIC_LEN];
    uint32 qos;
} RECV_PACKAGE_T;

/**
*********************************************************************
* @brief：mqtt连接函数callback函数
* @param[in]：
               mosq
               obj
               rc
* @param[out]：NONE

* @return： NONE
*********************************************************************
*/
static void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    (void)mosq;
    int          flag        = 1;
    XL_CLIENT_T *mqtt_client = obj;
    int          i           = 0;
    // printf("mqtt on connect\n");
    if (rc == 0)
    {
        if (setsockopt(mosquitto_socket(mqtt_client->uc.mclient.mqttHandler),
                       IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int)) != 0)
        {
            XL_WARN("mqttc Warning: Unable to set TCP_NODELAY.");
            // break;
        }

        // printf("subscribe:[%s]", s_mqtt_client->uc.mclient.user_id);
        mosquitto_subscribe(mqtt_client->uc.mclient.mqttHandler, NULL,
                            mqtt_client->uc.mclient.user_id, 0);
        for (i = 0; i < MAX_SUB_NUM; i++)
        {
            if (mqtt_client->uc.mclient.sub_topics[i].topic[0] != 0)
            {
                mosquitto_subscribe(mqtt_client->uc.mclient.mqttHandler, NULL,
                                    mqtt_client->uc.mclient.sub_topics[i].topic,
                                    mqtt_client->uc.mclient.sub_topics[i].qos);
            }
            else
            {
                break;
            }
        }

        mqtt_client->status = CONNECT_SUCC;
    }
    else
    {
        XL_WARN("on_connect error rc:[%d]", rc);
        mqtt_client->status = CONNECT_FAIL;
    }
}

/**
*********************************************************************
* @brief：mqtt订阅callback函数
* @param[in]：
               mosq
               obj
               mid
               qos_count
               granted_qos
* @param[out]：NONE

* @return： NONE
*********************************************************************
*/
static void on_subscribe(struct mosquitto *mosq,
                         void *            obj,
                         int               mid,
                         int               qos_count,
                         const int *       granted_qos)
{
    (void)mosq;
    (void)obj;
    (void)mid;
    (void)qos_count;
    (void)granted_qos;
}

/**
*********************************************************************
* @brief：mqtt订阅callback函数
* @param[in]：
               mosq
               obj
               mid
               qos_count
               granted_qos
* @param[out]：NONE

* @return： NONE
*********************************************************************
*/
static void message_callback(struct mosquitto *              mosq,
                             void *                          obj,
                             const struct mosquitto_message *message)
{
    (void)mosq;
    (void)message;
    XL_CLIENT_T *mqtt_client = obj;
    size_t       len         = message->payloadlen;
    char *       tmp         = NULL;
    // printf("recv_i:%s\n", (char *)message->payload);
    RECV_PACKAGE_T *package = malloc(sizeof(RECV_PACKAGE_T) + len);
    package->mid            = message->mid;
    package->payloadLen     = message->payloadlen;
    package->qos            = message->qos;
    tmp = (void *)(((char *)package) + sizeof(RECV_PACKAGE_T));
    memcpy(tmp, message->payload, len);
    strncpy(package->topic, message->topic, MAX_TOPIC_LEN);

    ((qqueue_t *)mqtt_client->uc.mclient.queue)
        ->push(mqtt_client->uc.mclient.queue, &package, sizeof(package));

#if 0
    if (0 != sem_post(&mqtt_client->uc.mclient.recvSem))
    {
        XL_WARN("sem_post fail");
    }
#endif
    sint8 write_ok = 1;
    sint32 result = write(mqtt_client->uc.mclient.pipe_fd[1], &write_ok, sizeof(write_ok));
    if (result != sizeof(write_ok))
    {
        XL_ERROR("Write pip error\n");
    }

    // else
    // {
    // printf("sem_post [0x%p]\n", &mqtt_client->uc.mclient.recvSem);
    // printf("sem_post errno :%d [%s]\n", errno, strerror(errno));
    // }
}

/**
*********************************************************************
* @brief：mqtt发布函数
* @param[in]：
               mosq
               obj
               mid
* @param[out]：NONE

* @return： NONE
*********************************************************************
*/
static void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
    (void)mosq;
    (void)obj;
    (void)mid;
    XL_DEBUG("mqtt on publish mid=[%d]\n", mid);
}

/**
*********************************************************************
* @brief：mqtt断开连接函数
* @param[in]：
               mosq
               obj
               rc
* @param[out]：NONE

* @return： NONE
*********************************************************************
*/
static void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
    (void)mosq;
    (void)rc;

    XL_CLIENT_T *mqtt_client = obj;

    XL_INFO("mqtt on disconnect rc=%d\n", rc);
    mqtt_client->status = CONNECT_FAIL;
}

/**
*********************************************************************
* @brief：mqtt客户端初始化
* @param[in]：
* @param[out]：NONE

* @return： 0  success
            -1  fail
*********************************************************************
*/
static sint32 mqtt_client_init(XL_CLIENT_T *c)
{
    sint32 flag     = 1;
    sint8 *username = NULL;
    sint8 *password = NULL;

    UNUSED(c);
    UNUSED(flag);
    do
    {
        if (0 != mosquitto_lib_init())
        {
            XL_ERROR("mosquitto_lib_init fail");
            break;
        }

        sint32 result = pipe(c->uc.mclient.pipe_fd);
        if (result < 0)
        {
            XL_ERROR("pipe init fail");
            break;
        }

        setnonblock(c->uc.mclient.pipe_fd[0]);
        setnonblock(c->uc.mclient.pipe_fd[1]);
#if 0
        if (0 != sem_init(&c->uc.mclient.recvSem, 0, 0))
        {
            XL_ERROR("sem_init fail");
        }
#endif

        c->uc.mclient.queue = qqueue(QQUEUE_THREADSAFE);
        if (NULL == c->uc.mclient.queue)
        {
            XL_ERROR("qqueue fail");
            break;
        }

        XL_DEBUG("c->uc.mclient.user_id = %s", c->uc.mclient.user_id);
        c->uc.mclient.mqttHandler =
            mosquitto_new(c->uc.mclient.user_id, true, c);
        if (NULL == c->uc.mclient.mqttHandler)
        {
            XL_ERROR("mosquitto_new fail errno[%d][%s]", errno,
                     strerror(errno));
            break;
        }

#ifdef MQTT_V5
        if (0 != mosquitto_int_option(c->uc.mclient.mqttHandler,
                                      MOSQ_OPT_PROTOCOL_VERSION,
                                      MQTT_PROTOCOL_V5))
        {
            fprintf(stderr, "mosquitto_int_option fail");
            break;
        }
#endif

        if (strlen(c->uc.mclient.user_name) > 0)
        {
            username = c->uc.mclient.user_name;
        }

        if (strlen(c->uc.mclient.user_passwd) > 0)
        {
            password = c->uc.mclient.user_passwd;
        }

        if (0 != mosquitto_username_pw_set(c->uc.mclient.mqttHandler, username,
                                           password))
        {
            XL_ERROR(
                "mosquitto_username_pw_set fail username:[%s]  password:[%s]",
                username, password);
            break;
        }

        mosquitto_reconnect_delay_set(c->uc.mclient.mqttHandler,
                                      c->uc.mclient.connect_timeout_s,
                                      c->uc.mclient.connect_timeout_s, 0);
        mosquitto_connect_callback_set(c->uc.mclient.mqttHandler, on_connect);
        mosquitto_disconnect_callback_set(c->uc.mclient.mqttHandler,
                                          on_disconnect);
        mosquitto_publish_callback_set(c->uc.mclient.mqttHandler, on_publish);
        mosquitto_subscribe_callback_set(c->uc.mclient.mqttHandler,
                                         on_subscribe);
        mosquitto_message_callback_set(c->uc.mclient.mqttHandler,
                                       message_callback);

        if (0 != mosquitto_loop_start(c->uc.mclient.mqttHandler))
        {
            XL_ERROR("mqtt loop start fail");
            break;
        }

        c->status = INIT;
        return 0;
    } while (0);

    return -1;
}

/**
*********************************************************************
* @brief：mqtt客户端连接函数
* @param[in]：
               rc
* @param[out]：NONE

* @return： 0  success
           -1  fail
*********************************************************************
*/
static sint32 mqtt_client_connect(XL_CLIENT_T *c)
{
    sint32 ret = -1;
    //首次调用时执行异步连接请求
    if (INIT == c->status)
    {
        if (0 != mosquitto_connect_async(c->uc.mclient.mqttHandler,
                                         c->uc.mclient.server_ip,
                                         c->uc.mclient.server_port, 60))
        {
            XL_ERROR("mqtt connect async fail");
        }
        else
        {
            c->status = CONNECTING;
            ret       = 0;
        }
    }
    else
    {
        ret = 0;
    }

    return ret;
}

/**
*********************************************************************
* @brief：获取mqtt客户端状态
* @param[in]： c
* @param[out]：NONE

* @return： CLIENT_STATUS_E
*********************************************************************
*/
static CLIENT_STATUS_E mqtt_client_get_status(XL_CLIENT_T *c)
{
    return c->status;
}

/**
*********************************************************************
* @brief：mqtt客户端写接口
* @param[in]： c
               data     数据
               dataLen  数据长度
* @param[out]：-1       fail
               数据长度  success

* @return： CLIENT_STATUS_E
*********************************************************************
*/
static sint32 mqtt_clien_write(XL_CLIENT_T *c, const char *data, sint32 dataLen)
{
    sint32 mid;
    sint32 ret = -1;
    if (c->status == CONNECT_SUCC)
    {
        ret = mosquitto_publish(c->uc.mclient.mqttHandler, &mid,
                                c->uc.mclient.default_send_topic, dataLen, data,
                                c->uc.mclient.default_send_topic_qos, false);
        if (ret == MOSQ_ERR_SUCCESS)
        {
            ret = dataLen;
        }
        else
        {
            XL_WARN("mosquitto_publish fail ret:[%d]\n", ret);
            ret = -1;
        }
    }
    else
    {
        XL_DEBUG("broker is not connected\n");
        ret = -1;
    }

    return ret;
}

/**
*********************************************************************
* @brief：mqtt客户端读接口
* @param[in]： c
               data     数据
               dataLen  数据长度
* @param[out]：-1       fail
               数据长度  success

* @return： CLIENT_STATUS_E
*********************************************************************
*/
static sint32 mqtt_clien_read(XL_CLIENT_T *c, char *data, sint32 dataLen)
{
    UNUSED(c);
    UNUSED(data);
    UNUSED(dataLen);

    uint32          cpLen   = 0;
    void *          qp      = NULL;
    size_t          qpLen   = 0;
    RECV_PACKAGE_T *package = NULL;
#if 0
    if (0 != sem_trywait(&c->uc.mclient.recvSem))
    {
        return -1;
    }
#endif

    sint8 value = 0;
    sint32 result = read(c->uc.mclient.pipe_fd[0], &value, sizeof(value));
    if (result != 1)
    {
        return cpLen;
    }

    qp = ((qqueue_t *)c->uc.mclient.queue)->pop(c->uc.mclient.queue, &qpLen);
    memcpy(&package, qp, qpLen);

    cpLen = (uint32)dataLen < package->payloadLen ? (uint32)dataLen :
                                                    package->payloadLen;

    memcpy(data, (char *)package + sizeof(RECV_PACKAGE_T), cpLen);
    free(qp);
    free(package);
    return cpLen;
}

/**
*********************************************************************
* @brief：mqtt客户端断开连接
* @param[in]： c        XL_CLIENT_T
* @param[out]：none

* @return： 0
*********************************************************************
*/
static sint32 mqtt_client_disconnect(XL_CLIENT_T *c)
{
    mosquitto_disconnect(c->uc.mclient.mqttHandler);
    return 0;
}

/**
*********************************************************************
* @brief：删除mqtt客户端
* @param[in]： c        XL_CLIENT_T
* @param[out]：none

* @return： 0
*********************************************************************
*/
void del_mqtt_client(XL_CLIENT_T *c)
{
    // do any
    mosquitto_loop_stop(c->uc.mclient.mqttHandler, false);
    //sem_destroy(&c->uc.mclient.recvSem);
    close(c->uc.mclient.pipe_fd[1]);
    close(c->uc.mclient.pipe_fd[0]);
    free(c);
}

/**
*********************************************************************
* @brief：创建mqtt客户端
* @param[in]： none
* @param[out]：none

* @return： XL_CLIENT_T*
*********************************************************************
*/
XL_CLIENT_T *new_mqtt_client(void)
{
    XL_CLIENT_T *mqtt_client = calloc(1, sizeof(XL_CLIENT_T));
    do
    {
        // printf("mqtt_client %p\n", mqtt_client);
        if (NULL == mqtt_client)
        {
            break;
        }

        mqtt_client->initFp             = mqtt_client_init;
        mqtt_client->connectFp          = mqtt_client_connect;
        mqtt_client->getConnectStatusFp = mqtt_client_get_status;
        mqtt_client->readFp             = mqtt_clien_read;
        mqtt_client->writeFp            = mqtt_clien_write;
        mqtt_client->disconnectFp       = mqtt_client_disconnect;

    } while (0);

    return mqtt_client;
}

/**
*********************************************************************
* @brief：mqtt客户端写接口
* @param[in]： 
* @param[out]：none

* @return： -1   fail
            datalen  success
*********************************************************************
*/
sint32 mqtt_clien_write_plus(XL_CLIENT_T *c,
                             char *       topic,
                             sint32       qos,
                             sint32 *     mid,
                             const char * data,
                             sint32       dataLen)
{
    sint32 ret = -1;
    if (c->status == CONNECT_SUCC)
    {
        ret = mosquitto_publish(c->uc.mclient.mqttHandler, mid, topic, dataLen,
                                data, qos, false);
        if (ret == MOSQ_ERR_SUCCESS)
        {
            ret = dataLen;
        }
        else
        {
            XL_WARN("mosquitto_publish fail ret:[%d]\n", ret);
            ret = -1;
        }
    }
    else
    {
        XL_WARN("broker is not connected\n");
        ret = -1;
    }

    return ret;
}

/*
static int sem_timedwait_millsecs(sem_t *sem, long msecs)
{
    struct timespec ts;
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long secs = msecs / 1000;
    msecs     = msecs % 1000;

    long add = 0;
    msecs    = msecs * 1000 * 1000 + ts.tv_nsec;
    add      = msecs / (1000 * 1000 * 1000);
    ts.tv_sec += (add + secs);
    ts.tv_nsec = msecs % (1000 * 1000 * 1000);

    // return sem_timedwait(sem, &ts);

    for (;;)
    {
        if(0 == sem_trywait(sem))
        {
            return 0;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);
        if (now.tv_sec >= ts.tv_sec && now.tv_nsec >= ts.tv_nsec)
        {
            return -1;
        }
        usleep(1);
    }
    
}
*/

/**
*********************************************************************
* @brief：mqtt客户端读接口
* @param[in]： 
* @param[out]：none

* @return： 0   fail
            datalen  success
*********************************************************************
*/
sint32 mqtt_clien_read_plus(XL_CLIENT_T *c,
                            char *       topic,
                            sint32 *     qos,
                            sint32 *     mid,
                            char *       data,
                            sint32       dataLen,
                            long         msecs)
{
    UNUSED(c);
    UNUSED(data);
    UNUSED(dataLen);
    uint32          cpLen   = 0;
    void *          qp      = NULL;
    size_t          qpLen   = 0;
    RECV_PACKAGE_T *package = NULL;

    if (msecs <= 0)
    {
#if 0
        // printf("sem_trywait [0x%p]\n", &c->uc.mclient.recvSem);
        if (0 != sem_trywait(&c->uc.mclient.recvSem))
        {
            // printf("errno :%d [%s]",errno,strerror(errno));
            return -1;
        }
#endif
        sint8 value = 0;
        sint32 result = read(c->uc.mclient.pipe_fd[0], &value, sizeof(value));
        if (result != 1)
        {
            return cpLen;
        }
    }
    else
    {
#if 0
        // printf("sem_timedwait_millsecs [0x%p]\n", &c->uc.mclient.recvSem);
        if (0 != sem_timedwait_millsecs(&c->uc.mclient.recvSem, msecs))
        {
            return -1;
        }
#endif
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(c->uc.mclient.pipe_fd[0], &rset);
        struct timeval tv;
        tv.tv_sec     = msecs / 1000;
        tv.tv_usec    = (msecs % 1000)*1000;
        int ret = select(c->uc.mclient.pipe_fd[0] + 1, &rset, NULL, NULL, &tv);
        if (ret > 0)
        {
            if (FD_ISSET(c->uc.mclient.pipe_fd[0], &rset))
            {
                sint8 value = 0;
                sint32 result = read(c->uc.mclient.pipe_fd[0], &value, sizeof(value));
                if (result != 1)
                {
                    return cpLen;
                }
            } 
            else {
                return cpLen;
            }
        } else {
            return cpLen;
        }
    }

    qp = ((qqueue_t *)c->uc.mclient.queue)->pop(c->uc.mclient.queue, &qpLen);
    memcpy(&package, qp, qpLen);

    cpLen = (uint32)dataLen < package->payloadLen ? (uint32)dataLen :
                                                    package->payloadLen;
    strncpy(topic, package->topic, MAX_TOPIC_LEN);
    *qos = package->qos;
    *mid = package->mid;
    memcpy(data, (char *)package + sizeof(RECV_PACKAGE_T), cpLen);
    free(qp);
    free(package);
    return cpLen;
}
