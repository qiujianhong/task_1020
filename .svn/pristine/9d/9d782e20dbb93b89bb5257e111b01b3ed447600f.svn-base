/*
*********************************************************************
* Copyright(C) 2020南京新联电子股份有限公司
* All rights reserved.
* @brief： mqttService.c
* @date： 2020-6-10
*********************************************************************
*/
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "baseType.h"
#include "mosquitto.h"
#include "mqttService.h"
#include "framecomm.h"

//mqtt服务句柄
static MQTT_SERVICE_T *s_mqttService   = NULL;

//订阅callback函数
static SUBSCRIBE_CB_FP subscribe_cb_fp = NULL;

/**
*********************************************************************
* @brief：设置订阅回调函数
* @param[in]：订阅函数
* @param[out]：NONE

* @return： NONE
*********************************************************************
*/
void set_subscribe_cb_fp(SUBSCRIBE_CB_FP fp)
{
    subscribe_cb_fp = fp;
}

/**
*********************************************************************
* @brief：获取请求raw数据
* @param[in]：obj  消息指针
* @param[out]：rawBuffer 消息地址

* @return： 0
*********************************************************************
*/
static sint32 mqtt_get_req_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    MQTT_CTX_T *ctx = (MQTT_CTX_T *)obj;
    //将mqtt 收到的数据传址给MSG层
    *rawBuffer = &ctx->reqRawObj;
    return 0;
}

/**
*********************************************************************
* @brief：获取请求数据
* @param[in]：obj  消息指针
* @param[out] msg 消息地址

* @return： 0
*********************************************************************
*/
static sint32 mqtt_get_req_msg(void *obj, MSG_OBJ_T **msg)
{
    MQTT_CTX_T *ctx = (MQTT_CTX_T *)obj;
    // //将mqtt 收到的数据传址给MSG层
    *msg = &ctx->reqMsg;
    return 0;
}

/**
*********************************************************************
* @brief：获取回复数据
* @param[in]：obj  消息指针
* @param[out] msg 消息地址

* @return： 0
*********************************************************************
*/
static sint32 mqtt_get_rsp_msg(void *obj, MSG_OBJ_T **msg)
{
    MQTT_CTX_T *ctx = (MQTT_CTX_T *)obj;
    // //将mqtt 收到的数据传址给MSG层
    *msg = &(ctx->rspMsg);
    return 0;
}

/**
*********************************************************************
* @brief：获取回复raw数据
* @param[in]：obj  消息指针
* @param[out] msg 消息地址

* @return： 0
*********************************************************************
*/
static sint32 mqtt_get_rsp_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    MQTT_CTX_T *ctx = (MQTT_CTX_T *)obj;
    *rawBuffer      = &(ctx->rspRawObj);
    return 0;
}

/**
*********************************************************************
* @brief：获取客户端信息
* @param[in]：obj  消息指针
* @param[out] client 客户端信息

* @return： 0
*********************************************************************
*/
static sint32 mqtt_get_client_info(void *obj, MSG_CLIENT_T *client)
{
    MQTT_CTX_T *ctx = (MQTT_CTX_T *)obj;
    client->type    = MQTT;
    strncpy(client->clientName, ctx->reqMsg.source, sizeof(client->clientName));
    return 0;
}

/**
*********************************************************************
* @brief：信息处理回调函数
* @param[in]：obj  消息指针
* @param[out] none

* @return： ret
*********************************************************************
*/
sint32 mqtt_msg_handel_cb(void *obj)
{
    sint32 ret                     = -1;
    sint32 sentMid                 = -1;
    sint8 topic[MAX_TOPIC_LEN]     = {};
    sint32  rxBufLen               = 0;
    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)obj;
    MQTT_CTX_T *ctx                = (MQTT_CTX_T *)msgHanderObj->obj;

    if (ctx->rspRawObj.length > 0)
    {
        // topic = ctx->rspMsg.destination;
        snprintf(topic, sizeof(topic), "%s/%s", ctx->rspMsg.source, ctx->rspMsg.destination);

        ret = mosquitto_publish(s_mqttService->mosq, &sentMid, topic, ctx->rspRawObj.length, ctx->rspRawObj.payload,
                                s_mqttService->qos, false);

        if (ret != 0)
        {
            MQTTMSG_FMT_DEBUG("mqtt publish fail topic[%s] Len[%d]\n", topic, ctx->rspRawObj.length);
        }
        else
        {
            rxBufLen = pro_parser_msg(ctx->rspRawObj.payload, ctx->rspRawObj.length, &ctx->rspMsg);
            MQTTMSG_BUF_DEBUG(ctx->rspRawObj.payload, rxBufLen, "mqtt publish topic[%s] mid[%d]:", topic, sentMid);
        }
    }

    free(ctx);
    free(msgHanderObj);
    return ret;
}

/**
*********************************************************************
* @brief：mqtt连接函数
* @param[in]：rc 返回值
              mosq  mosquitto实例
              obj   在mosquitto_new中提供的用户数据
              rc    连接响应值
* @param[out] none

* @return： none
*********************************************************************
*/
void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    int flag = 1;
    (void)mosq;
    (void)obj;

    MQTTMSG_FMT_DEBUG("[mqtt] on connect rc:[%d]\n", rc);
    if (rc == 0)
    {
        if (setsockopt(mosquitto_socket(s_mqttService->mosq), IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int)) != 0)
        {
            MQTTMSG_FMT_DEBUG("mqtts Warning: Unable to set TCP_NODELAY.\n");
            // exit(-1);
        }

        if (subscribe_cb_fp)
        {
            MQTT_SERVER_SUB_TOPIC_T *subTopic;
            sint32 subNum = 0;
            sint32 i      = 0;
            subscribe_cb_fp(&subTopic, &subNum);
            if (subNum)
            {
                for (i = 0; i < subNum; i++)
                {
                    mosquitto_subscribe(s_mqttService->mosq, NULL, subTopic[i].topic, subTopic[i].qos);
                    MQTTMSG_FMT_DEBUG("subscribe:[%s] qos:[%d]\n", subTopic[i].topic, subTopic[i].qos);
                }
            }
        }
        // mosquitto_subscribe(s_mqttService->mosq, NULL, s_mqttService->userID,
        //                     s_mqttService->qos);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("on_connect error rc:[%d]\n", rc);
    }
}

/**
*********************************************************************
* @brief：mqtt订阅函数
* @param[in]：  mosq   mosquitto实例
                obj    在mosquitto_new中提供的用户数据
                mid    消息id
                qos_count the number of granted subscriptions (size of granted_qos)
                granted_qos an array of integers indicating the granted QoS for each of the subscriptions.
* @param[out]：

* @return： none
*********************************************************************
*/
void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
    (void)mosq;
    (void)obj;
    (void)mid;
    (void)qos_count;
    (void)granted_qos;
}

/**
*********************************************************************
* @brief：消息回调函数
* @param[in]： mosq     mosquitto实例
               obj      在mosquitto_new中提供的用户数据
               message  消息数据
* @param[out]：

* @return： none
*********************************************************************
*/
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    MSG_HANDER_OBJ_T *msgHanderObj = NULL;
    MQTT_CTX_T *ctx                = NULL;
    sint32 rxBufLen                = 0;

    (void)obj;
    (void)mosq;

    do
    {
        ctx = (MQTT_CTX_T *)calloc(1, sizeof(MQTT_CTX_T));
        if (ctx == NULL)
        {
            MQTTMSG_FMT_DEBUG("MQTT_CTX_T calloc fail\n");
            break;
        }

        if (message->payloadlen < MIN_RAW_BUF_LEN)
        {
            MQTTMSG_FMT_DEBUG("mqtt recv data[%d] < MIN_RAW_BUF_LEN[%d]\n", message->payloadlen, MIN_RAW_BUF_LEN);
            break;
        }

        if (message->payloadlen > MAX_MSG_LEN)
        {
            MQTTMSG_FMT_DEBUG("mqtt recv data[%d] > MAX_MSG_LEN[%d]\n", message->payloadlen, MAX_MSG_LEN);
            break;
        }

        memcpy(ctx->reqRawObj.payload, message->payload, message->payloadlen);
        rxBufLen = pro_parser_msg(ctx->reqRawObj.payload, message->payloadlen, &ctx->reqMsg);
        if (rxBufLen < 0)
        {
            MQTTMSG_FMT_DEBUG("pro_parser_msg fail\n");
            break;
        }
        else
        {
            MQTTMSG_BUF_DEBUG(ctx->reqRawObj.payload, rxBufLen, "收到消息 topic:[%s]:", message->topic);
            msgHanderObj = (MSG_HANDER_OBJ_T *)calloc(1, sizeof(MSG_HANDER_OBJ_T));

            if (msgHanderObj == NULL)
            {
                MQTTMSG_FMT_DEBUG("msgHanderObj calloc fail\n");
                break;
            }

            msgHanderObj->obj             = ctx;  //将CTX与msgHanderObj绑定
            ctx->reqRawObj.length         = rxBufLen;
            msgHanderObj->getRspDataFp    = mqtt_get_rsp_raw_buf;  //设置响应报文回调函数
            msgHanderObj->getReqDataFp    = mqtt_get_req_raw_buf;  //设置请求报文获取的回调函数
            msgHanderObj->getReqDataMsgFp = mqtt_get_req_msg;
            msgHanderObj->getRspDataMsgFp = mqtt_get_rsp_msg;
            msgHanderObj->handerFp        = mqtt_msg_handel_cb;  //在hander处理后必须释放 msgHanderObj
                                                                 // msgHanderObj->obj
            msgHanderObj->priority      = ctx->reqMsg.priority;
            msgHanderObj->getClientInfo = mqtt_get_client_info;
            if(0 != base_queue_message(msgHanderObj))
            {
                MQTTMSG_FMT_DEBUG("base_queue_message msgHanderObj fail\n");
                break;
            }
        }

        return;
    } while (0);

    if (NULL != ctx)
    {
        free(ctx);
    }

    if (NULL != msgHanderObj)
    {
        free(msgHanderObj);
    }
}

/**
*********************************************************************
* @brief：消息发布函数
* @param[in]： mosq     mosquitto实例
               obj      在mosquitto_new中提供的用户数据
               mid      消息id
* @param[out]：

* @return： none
*********************************************************************
*/
void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
    (void)mosq;
    (void)obj;
    MQTTMSG_FMT_TRACE("mqtt on publish mid=[%d]\n", mid);
}

/**
*********************************************************************
* @brief：连接断开处理函数
* @param[in]：  mosq     mosquitto实例
                obj      在mosquitto_new中提供的用户数据
                rc       断开连接的原因
* @param[out]： none

* @return： none
*********************************************************************
*/
void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
    (void)mosq;
    (void)obj;
    MQTTMSG_FMT_DEBUG("mqtt on disconnect rc=%d\n", rc);
}

/*
static void *mqtt_service(void *arg)
{
    int firstConnect = -1;
    MQTT_SERVICE_T *mqservice = (MQTT_SERVICE_T *)arg;
    struct mosquitto * mosq;

    do
    {
        MQTTMSG_FMT_DEBUG("mqtt connect start\n");
        firstConnect = mosquitto_connect(mqservice->mosq, "localhost", 1883, 60);
        if (0 != firstConnect)
        {
            MQTTMSG_FMT_DEBUG("mqtt connect fail ret:[%d]\n", firstConnect);
            sleep(RECONNECT_INTERVAL);
        }

    } while (firstConnect != 0);

    // while (1)
    {
        MQTTMSG_FMT_DEBUG("=====>mosquitto_loop_forever\n");
        mosquitto_loop_forever(mqservice->mosq, MOSQUITTO_SELECT_TIMEOUT,1);
        MQTTMSG_FMT_DEBUG("<=====mosquitto_loop_forever\n");
        // mosquitto_loop(mosq, 50, 1);
    }

    mosquitto_lib_cleanup();
    return 1;
}
*/

/**
*********************************************************************
* @brief： mqtt通讯服务清理系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void mqtt_clean_up_system(void)
{
    MQTTMSG_FMT_DEBUG("mqtt_clean_up_system\n");
}

/**
*********************************************************************
* @brief： mqtt通讯服务初始化
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 mqtt_init_system(void)
{
    sint32 ret = -1;
    uint8  failFlag;

    INIT_FMT_DEBUG("mqtt_init_system\n");
    do
    {
        failFlag = 0;
        s_mqttService = calloc(1, sizeof(MQTT_SERVICE_T));
        if (NULL == s_mqttService)
        {
            INIT_FMT_DEBUG("calloc s_mqttService fail\n");
            break;
        }

        if (0 != mosquitto_lib_init())
        {
            INIT_FMT_DEBUG("mosquitto_lib_init fail\n");
            failFlag = 1;
            usleep(100000);
            break;
        }

        ret = 0;
    } while (failFlag);

    return ret;
}

/**
*********************************************************************
* @brief： mqtt通讯服务启动系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 mqtt_start_system()
{
    sint32 ret      = -1;
    sint8 *username = NULL;
    sint8 *password = NULL;
    sint32 flag     = 1;
    uint8  failFlag;
    uint16 count = 0;
    
    UNUSED(flag);
    // pthread_t tid;

    // ocat = olog_get_category("mqttS");

    MQTTMSG_FMT_DEBUG("mqtt_start_system\n");
    do
    {
        failFlag = 0;
        s_mqttService->mosq = mosquitto_new(s_mqttService->userID, true, NULL);
        if (NULL == s_mqttService->mosq)
        {
            MQTTMSG_FMT_DEBUG("mosquitto_new fail\n");
            failFlag = 1;
            usleep(100000);
            break;
        }
    }while(failFlag);
        
#ifdef MQTT_V5
    do
    {
        failFlag = 0;
        if (0 != mosquitto_int_option(s_mqttService->mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5))
        {
            MQTTMSG_FMT_DEBUG("mosquitto_int_option fail\n");
            failFlag = 1;
            usleep(100000);
            break;
        }
    }while(failFlag);
#endif
    do
    {
        failFlag = 0;
        if (strlen(s_mqttService->userName) > 0)
        {
            username = s_mqttService->userName;
        }

        if (strlen(s_mqttService->passwd) > 0)
        {
            password = s_mqttService->passwd;
        }

        if (0 != mosquitto_username_pw_set(s_mqttService->mosq, username, password))
        {
            MQTTMSG_FMT_DEBUG("mosquitto_username_pw_set fail username:[%s]  password:[%s]\n", username, password);
            failFlag = 1;
            usleep(100000);
            break;
        }
    }while(failFlag);

    mosquitto_reconnect_delay_set(s_mqttService->mosq, RECONNECT_INTERVAL, RECONNECT_INTERVAL_MAX, 0);
    mosquitto_connect_callback_set(s_mqttService->mosq, on_connect);
    mosquitto_disconnect_callback_set(s_mqttService->mosq, on_disconnect);
    mosquitto_publish_callback_set(s_mqttService->mosq, on_publish);
    mosquitto_subscribe_callback_set(s_mqttService->mosq, on_subscribe);
    mosquitto_message_callback_set(s_mqttService->mosq, message_callback);

        // mosquitto_socket(s_mqttService->mosq);
    do
    {
        failFlag = 0;
        if (0 != mosquitto_loop_start(s_mqttService->mosq))
        {
            MQTTMSG_FMT_DEBUG("mqtt loop start fail\n");
            failFlag =1;
            usleep(100000);
            break;
        }
    }while(failFlag);

    while(count++ <= 120)
    {
        if (0 !=  mosquitto_connect_bind_async(s_mqttService->mosq, s_mqttService->serverIp, 
                                              s_mqttService->serverPort, 60, "127.0.0.1"))
        {
            if(count % 20 <= 1)
            {
                MQTTMSG_FMT_DEBUG("mqtt connect failed!\n");
            }
            sleep(1);
        }
        else
        {
            MQTTMSG_FMT_DEBUG("[mqtt] connect start\n");
            ret = 0;
            break;
        }
    }

    return ret;
}

/**
*********************************************************************
* @brief： mqtt通讯服务停止系统
* @param[in]： none
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
void mqtt_stop_system()
{
    mosquitto_disconnect(s_mqttService->mosq);
    mosquitto_loop_stop(s_mqttService->mosq, false);
    MQTTMSG_FMT_DEBUG("mqtt_stop_system\n");
}

/**
*********************************************************************
* @brief： 消息发送
* @param[in]： dest    目的地
               data    数据
               dataLen 数据长度
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 mqtt_sendto(sint8 *dest,char* sendtopic, void *data, uint32 dataLen)
{
    int ret                    = -1;
    sint32 sentMid             = -1;
    sint8 topic[MAX_TOPIC_LEN] = {0};
    if(NULL == sendtopic)
    {
        snprintf(topic, sizeof(topic), "%s/%s", s_mqttService->userID, dest);
    }else
    {
        strcpy(topic,sendtopic);
    }
    

    ret = mosquitto_publish(s_mqttService->mosq, &sentMid, topic, dataLen, data, s_mqttService->qos, false);
    if (ret != 0)
    {
        MQTTMSG_FMT_DEBUG("mqtt publish fail topic[%s] dataLen[%d]\n", topic, dataLen);
    ret = -1;
    }
    else
    {
        MQTTMSG_BUF_DEBUG(data, dataLen, "发送消息 topic[%s]:", topic);
    }

    return ret;
}

/**
*********************************************************************
* @brief： 设置broker ip
* @param[in]： ip
* @param[out]：none
* @return： none
*********************************************************************
*/
void mqtt_set_broker_ip(sint8 ip[IP_LEN])
{
    strncpy(s_mqttService->serverIp, ip, IP_LEN);
}

/**
*********************************************************************
* @brief： 设置broker port
* @param[in]： port
* @param[out]：none
* @return： none
*********************************************************************
*/
void mqtt_set_broker_port(sint16 port)
{
    s_mqttService->serverPort = port;
}

/**
*********************************************************************
* @brief： 设置订阅qos
* @param[in]： qos
* @param[out]：none
* @return： none
*********************************************************************
*/
void mqtt_set_sub_qos(sint8 qos)
{
    s_mqttService->qos = qos;
}

/**
*********************************************************************
* @brief： 设置用户名
* @param[in]： username
* @param[out]：none
* @return： none
*********************************************************************
*/
void mqtt_set_username(sint8 username[MAX_USER_NAME_LEN])
{
    strncpy(s_mqttService->userName, username, MAX_USER_NAME_LEN);
}

/**
*********************************************************************
* @brief： 设置密码
* @param[in]： password
* @param[out]：none
* @return： none
*********************************************************************
*/
void mqtt_set_password(sint8 passwd[MAX_PASSWD_LEN])
{
    strncpy(s_mqttService->passwd, passwd, MAX_PASSWD_LEN);
}

/**
*********************************************************************
* @brief： 设置用户id
* @param[in]： usrID
* @param[out]：none
* @return： none
*********************************************************************
*/
void mqtt_set_userID(sint8 userID[MAX_USER_ID_LEN])
{
    strncpy(s_mqttService->userID, userID, MAX_USER_ID_LEN);
}
