/*
*********************************************************************
* Copyright(C) 2020�Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� mqttService.c
* @date�� 2020-6-10
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
#include "olog.h"

//mqtt������
static MQTT_SERVICE_T *s_mqttService   = NULL;
//��־���
static void *ocat                      = NULL;
//����callback����
static SUBSCRIBE_CB_FP subscribe_cb_fp = NULL;
extern void *     db_ini;
/**
*********************************************************************
* @brief�����ö��Ļص�����
* @param[in]�����ĺ���
* @param[out]��NONE

* @return�� NONE
*********************************************************************
*/
void set_subscribe_cb_fp(SUBSCRIBE_CB_FP fp)
{
    subscribe_cb_fp = fp;
}

/**
*********************************************************************
* @brief����ȡ����raw����
* @param[in]��obj  ��Ϣָ��
* @param[out]��rawBuffer ��Ϣ��ַ

* @return�� 0
*********************************************************************
*/
static sint32 mqtt_get_req_raw_buf(void *obj, RAW_OBJ_T **rawBuffer)
{
    MQTT_CTX_T *ctx = (MQTT_CTX_T *)obj;
    //��mqtt �յ������ݴ�ַ��MSG��
    *rawBuffer = &ctx->reqRawObj;
    return 0;
}

/**
*********************************************************************
* @brief����ȡ��������
* @param[in]��obj  ��Ϣָ��
* @param[out] msg ��Ϣ��ַ

* @return�� 0
*********************************************************************
*/
static sint32 mqtt_get_req_msg(void *obj, MSG_OBJ_T **msg)
{
    MQTT_CTX_T *ctx = (MQTT_CTX_T *)obj;
    // //��mqtt �յ������ݴ�ַ��MSG��
    *msg = &ctx->reqMsg;
    return 0;
}

/**
*********************************************************************
* @brief����ȡ�ظ�����
* @param[in]��obj  ��Ϣָ��
* @param[out] msg ��Ϣ��ַ

* @return�� 0
*********************************************************************
*/
static sint32 mqtt_get_rsp_msg(void *obj, MSG_OBJ_T **msg)
{
    MQTT_CTX_T *ctx = (MQTT_CTX_T *)obj;
    // //��mqtt �յ������ݴ�ַ��MSG��
    *msg = &(ctx->rspMsg);
    return 0;
}

/**
*********************************************************************
* @brief����ȡ�ظ�raw����
* @param[in]��obj  ��Ϣָ��
* @param[out] msg ��Ϣ��ַ

* @return�� 0
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
* @brief����ȡ�ͻ�����Ϣ
* @param[in]��obj  ��Ϣָ��
* @param[out] client �ͻ�����Ϣ

* @return�� 0
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
* @brief����Ϣ����ص�����
* @param[in]��obj  ��Ϣָ��
* @param[out] none

* @return�� ret
*********************************************************************
*/
sint32 mqtt_msg_handel_cb(void *obj)
{
    sint32 ret                     = -1;
    sint32 sentMid                 = -1;
    sint8 topic[MAX_TOPIC_LEN]     = {};
	sint32	rxBufLen			   = 0;
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
            olog_warn(ocat, "mqtt publish fail topic[%s] Len[%d]", topic, ctx->rspRawObj.length);
        }
        else
        {
            olog_debug(ocat, "mqtt publish topic[%s] mid[%d]", topic, sentMid);
            rxBufLen = pro_parser_msg(ctx->rspRawObj.payload, ctx->rspRawObj.length, &ctx->rspMsg);
            holog_debug(ocat, ctx->rspRawObj.payload, rxBufLen);
        }
    }

	if (NULL != ctx)
    {
        free(ctx);
    }

    if (NULL != msgHanderObj)
    {
        free(msgHanderObj);
    }
    return ret;
}

/**
*********************************************************************
* @brief��mqtt���Ӻ���
* @param[in]��rc ����ֵ
              mosq  mosquittoʵ��
              obj   ��mosquitto_new���ṩ���û�����
              rc    ������Ӧֵ
* @param[out] none

* @return�� none
*********************************************************************
*/
void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    int flag = 1;
    (void)mosq;
    (void)obj;

    olog_debug(ocat, "[mqtt] on connect rc:[%d]", rc);
    if (rc == 0)
    {
        if (setsockopt(mosquitto_socket(s_mqttService->mosq), IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int)) != 0)
        {
            olog_error(ocat, "mqtts Warning: Unable to set TCP_NODELAY.");
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
                    olog_info(ocat, "subscribe:[%s] qos:[%d]", subTopic[i].topic, subTopic[i].qos);
                }
            }
        }
        // mosquitto_subscribe(s_mqttService->mosq, NULL, s_mqttService->userID,
        //                     s_mqttService->qos);
    }
    else
    {
        olog_warn(ocat, "on_connect error rc:[%d]", rc);
    }
}

/**
*********************************************************************
* @brief��mqtt���ĺ���
* @param[in]��  mosq   mosquittoʵ��
                obj    ��mosquitto_new���ṩ���û�����
                mid    ��Ϣid
                qos_count the number of granted subscriptions (size of granted_qos)
                granted_qos an array of integers indicating the granted QoS for each of the subscriptions.
* @param[out]��

* @return�� none
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
* @brief����Ϣ�ص�����
* @param[in]�� mosq     mosquittoʵ��
               obj      ��mosquitto_new���ṩ���û�����
               message  ��Ϣ����
* @param[out]��

* @return�� none
*********************************************************************
*/
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    MSG_HANDER_OBJ_T *msgHanderObj = NULL;
    MQTT_CTX_T *ctx                = NULL;
    sint32 rxBufLen                = 0;

    (void)obj;
    (void)mosq;
    olog_debug(ocat, "recv msg mid[%u], topic:[%s]", message->mid, message->topic);
    // holog_debug(ocat, message->payload, message->payloadlen);

    do
    {
        ctx = (MQTT_CTX_T *)calloc(1, sizeof(MQTT_CTX_T));
        if (ctx == NULL)
        {
            olog_warn(ocat, "MQTT_CTX_T calloc fail");
            break;
        }

        if (message->payloadlen < MIN_RAW_BUF_LEN)
        {
            olog_warn(ocat, "mqtt recv data[%d] < MIN_RAW_BUF_LEN[%d]", message->payloadlen, MIN_RAW_BUF_LEN);
            break;
        }

        if (message->payloadlen > MAX_MSG_LEN)
        {
            olog_warn(ocat, "mqtt recv data[%d] > MAX_MSG_LEN[%d]", message->payloadlen, MAX_MSG_LEN);
            break;
        }

        memcpy(ctx->reqRawObj.payload, message->payload, message->payloadlen);
        rxBufLen = pro_parser_msg(ctx->reqRawObj.payload, message->payloadlen, &ctx->reqMsg);
        if (rxBufLen < 0)
        {
            olog_error(ocat, "pro_parser_msg fail");
            break;
        }
        else
        {
            holog_debug(ocat, ctx->reqRawObj.payload, rxBufLen);
            msgHanderObj = (MSG_HANDER_OBJ_T *)calloc(1, sizeof(MSG_HANDER_OBJ_T));

            if (msgHanderObj == NULL)
            {
                olog_warn(ocat, "msgHanderObj calloc fail");
                break;
            }

            msgHanderObj->obj             = ctx;  //��CTX��msgHanderObj��
            ctx->reqRawObj.length         = rxBufLen;
            msgHanderObj->getRspDataFp    = mqtt_get_rsp_raw_buf;  //������Ӧ���Ļص�����
            msgHanderObj->getReqDataFp    = mqtt_get_req_raw_buf;  //���������Ļ�ȡ�Ļص�����
            msgHanderObj->getReqDataMsgFp = mqtt_get_req_msg;
            msgHanderObj->getRspDataMsgFp = mqtt_get_rsp_msg;
            msgHanderObj->handerFp        = mqtt_msg_handel_cb;  //��hander���������ͷ� msgHanderObj
                                                                 // msgHanderObj->obj
            msgHanderObj->priority      = ctx->reqMsg.priority;
            msgHanderObj->getClientInfo = mqtt_get_client_info;
            if(0 != base_queue_message(msgHanderObj))
            {
                olog_warn(ocat, "base_queue_message msgHanderObj fail");
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
* @brief����Ϣ��������
* @param[in]�� mosq     mosquittoʵ��
               obj      ��mosquitto_new���ṩ���û�����
               mid      ��Ϣid
* @param[out]��

* @return�� none
*********************************************************************
*/
void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
    (void)mosq;
    (void)obj;
    olog_debug(ocat, "mqtt on publish mid=[%d]", mid);
}

/**
*********************************************************************
* @brief�����ӶϿ�������
* @param[in]��  mosq     mosquittoʵ��
                obj      ��mosquitto_new���ṩ���û�����
                rc       �Ͽ����ӵ�ԭ��
* @param[out]�� none

* @return�� none
*********************************************************************
*/
void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
    (void)mosq;
    (void)obj;
    olog_info(ocat, "mqtt on disconnect rc=%d", rc);
}

/*
static void *mqtt_service(void *arg)
{
    int firstConnect = -1;
    MQTT_SERVICE_T *mqservice = (MQTT_SERVICE_T *)arg;
    struct mosquitto * mosq;

    do
    {
        olog_debug(ocat, "mqtt connect start");
        firstConnect = mosquitto_connect(mqservice->mosq, "localhost", 1883, 60);
        if (0 != firstConnect)
        {
            olog_error(ocat, "mqtt connect fail ret:[%d]", firstConnect);
            sleep(RECONNECT_INTERVAL);
        }

    } while (firstConnect != 0);

    // while (1)
    {
        olog_debug(ocat, "=====>mosquitto_loop_forever");
        mosquitto_loop_forever(mqservice->mosq, MOSQUITTO_SELECT_TIMEOUT,1);
        olog_debug(ocat, "<=====mosquitto_loop_forever");
        // mosquitto_loop(mosq, 50, 1);
    }

    mosquitto_lib_cleanup();
    return 1;
}
*/

/**
*********************************************************************
* @brief�� mqttͨѶ��������ϵͳ
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
void mqtt_clean_up_system(void)
{
    olog_info(ocat, "mqtt_clean_up_system");
}

/**
*********************************************************************
* @brief�� mqttͨѶ�����ʼ��
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
sint32 mqtt_init_system(void)
{
    sint32 ret = -1;
	uint8  failFlag;

    ocat = olog_get_category("mqttS");
    olog_info(ocat, "mqtt_init_system");
    do
    {
    	failFlag = 0;
        s_mqttService = calloc(1, sizeof(MQTT_SERVICE_T));
        if (NULL == s_mqttService)
        {
            olog_error(ocat, "calloc s_mqttService fail");
            break;
        }

        if (0 != mosquitto_lib_init())
        {
            olog_error(ocat, "mosquitto_lib_init fail");
			failFlag = 1;
			usleep(100000);
            free(s_mqttService);
            break;
        }

        ret = 0;
    } while (failFlag);

    return ret;
}

/**
*********************************************************************
* @brief�� mqttͨѶ��������ϵͳ
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
sint32 mqtt_start_system()
{
    sint32 ret      = -1;
    sint8 *username = NULL;
    sint8 *password = NULL;
    sint32 flag     = 1;
	uint8  failFlag;
    
    UNUSED(flag);
    // pthread_t tid;
    // ocat = olog_get_category("mqttS");
    int bindret;
    olog_info(ocat, "mqtt_start_system");
    do
    {
    	failFlag = 0;
        s_mqttService->mosq = mosquitto_new(s_mqttService->userID, true, NULL);
        if (NULL == s_mqttService->mosq)
        {
            olog_error(ocat, "mosquitto_new fail");
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
            olog_error(ocat, "mosquitto_int_option fail");
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
            olog_error(ocat, "mosquitto_username_pw_set fail username:[%s]  password:[%s]", username, password);
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
            olog_error(ocat, "mqtt loop start fail");
			failFlag =1;
			usleep(100000);
            continue;
        }
	}while(failFlag);


	do
	{
		failFlag = 0;

#if defined PRODUCT_ZCU_3 || defined PRODUCT_ZCU_1
            bindret = mosquitto_connect_bind_async(s_mqttService->mosq, s_mqttService->serverIp, s_mqttService->serverPort, 60, "127.0.0.1");
#else
            bindret = mosquitto_connect_bind_async(s_mqttService->mosq, s_mqttService->serverIp, s_mqttService->serverPort, 60, NULL);
#endif
        
        //if (0 != mosquitto_connect_async(s_mqttService->mosq, s_mqttService->serverIp, s_mqttService->serverPort, 60))
        if(bindret != 0)
        {
            olog_error(ocat, "mqtt connect async fail");
			failFlag =1;
			usleep(100000);
            continue;
        }
        else
        {
            olog_debug(ocat, "[mqtt] connect start");
        }

        ret = 0;
    } while (failFlag);

    return ret;
}

/**
*********************************************************************
* @brief�� mqttͨѶ����ֹͣϵͳ
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
void mqtt_stop_system()
{
    mosquitto_disconnect(s_mqttService->mosq);
    mosquitto_loop_stop(s_mqttService->mosq, false);
    olog_info(ocat, "mqtt_stop_system");
}

/**
*********************************************************************
* @brief�� ��Ϣ����
* @param[in]�� dest    Ŀ�ĵ�
               data    ����
               dataLen ���ݳ���
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
sint32 mqtt_sendto(sint8 *dest, void *data, uint32 dataLen)
{
    int ret                    = -1;
    sint32 sentMid             = -1;
    sint8 topic[MAX_TOPIC_LEN] = {0};

    snprintf(topic, sizeof(topic), "%s/%s", s_mqttService->userID, dest);

    ret = mosquitto_publish(s_mqttService->mosq, &sentMid, topic, dataLen, data, s_mqttService->qos, false);
    if (ret != 0)
    {
        olog_warn(ocat, "mqtt publish fail topic[%s] dataLen[%d]", topic, dataLen);
	ret = -1;
    }
    else
    {
        olog_debug(ocat, "mqtt publish topic[%s] mid[%d]", topic, sentMid);
    }

    return ret;
}

/**
*********************************************************************
* @brief�� ����broker ip
* @param[in]�� ip
* @param[out]��none
* @return�� none
*********************************************************************
*/
void mqtt_set_broker_ip(sint8 ip[IP_LEN])
{
    strncpy(s_mqttService->serverIp, ip, IP_LEN);
}

/**
*********************************************************************
* @brief�� ����broker port
* @param[in]�� port
* @param[out]��none
* @return�� none
*********************************************************************
*/
void mqtt_set_broker_port(sint16 port)
{
    s_mqttService->serverPort = port;
}

/**
*********************************************************************
* @brief�� ���ö���qos
* @param[in]�� qos
* @param[out]��none
* @return�� none
*********************************************************************
*/
void mqtt_set_sub_qos(sint8 qos)
{
    s_mqttService->qos = qos;
}

/**
*********************************************************************
* @brief�� �����û���
* @param[in]�� username
* @param[out]��none
* @return�� none
*********************************************************************
*/
void mqtt_set_username(sint8 username[MAX_USER_NAME_LEN])
{
    strncpy(s_mqttService->userName, username, MAX_USER_NAME_LEN);
}

/**
*********************************************************************
* @brief�� ��������
* @param[in]�� password
* @param[out]��none
* @return�� none
*********************************************************************
*/
void mqtt_set_password(sint8 passwd[MAX_PASSWD_LEN])
{
    strncpy(s_mqttService->passwd, passwd, MAX_PASSWD_LEN);
}

/**
*********************************************************************
* @brief�� �����û�id
* @param[in]�� usrID
* @param[out]��none
* @return�� none
*********************************************************************
*/
void mqtt_set_userID(sint8 userID[MAX_USER_ID_LEN])
{
    strncpy(s_mqttService->userID, userID, MAX_USER_ID_LEN);
}
