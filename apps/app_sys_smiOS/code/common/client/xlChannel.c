/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� xlChannel.c
           ͨ��channel��д�ӿ�
* @date�� 2019-12-2
*********************************************************************
*/
#include "xlChannel.h"
#include "baseConstant.h"
#include "mqttClient.h"
#include "tcpClient.h"
#include "udpClient.h"

/**
*********************************************************************
* @brief��                  ͨ��ͨ������
* @param[in]��
               url          ����Ŀ������url tcp://127.0.0.1:8001
                                              udp://127.0.0.1:8002
                                              mqtt://127.0.0.1:1883
                                              ipc://tmp/smiOS.sock

* @param[out]��
               ctx          ͨ����д���
* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_create(const char *url, XL_CHANNEL_CTX_T **ctx)
{
    char   typeStr[CONNECT_TYPE_LEN] = {0};
    char   subStr[128]               = {0};
    sint8  destIP[IP_LEN]            = {0};
    uint16 destPort                  = {0};

    if (url == NULL || ctx == NULL)
    {
        return -1;
    }

    *ctx = calloc(1, sizeof(XL_CHANNEL_CTX_T));
    if (*ctx == NULL)
    {
        return -1;
    }

    sscanf(url, "%[^:]://%s", typeStr, subStr);

    // printf("typeStr:%s subStr:%s\n", typeStr, subStr);
    if (0 == strncmp("ipc", typeStr, CONNECT_TYPE_LEN))
    {
        (*ctx)->type = CLIENT_IPC;
        sscanf(subStr, "%s", (*ctx)->c->uc.iclient.server_path);
    }
    else
    {
        sscanf(subStr, "%[^:]:%hu", destIP, &destPort);
        if (0 == strncmp("tcp", typeStr, CONNECT_TYPE_LEN))
        {
            (*ctx)->type = CLIENT_TCP;
            (*ctx)->c    = new_tcp_client();
            strncpy((*ctx)->c->uc.tclient.server_ip, destIP, IP_LEN);
            (*ctx)->c->uc.tclient.server_port = destPort;
        }
        else if (0 == strncmp("udp", typeStr, CONNECT_TYPE_LEN))
        {
            (*ctx)->type = CLIENT_UDP;
            (*ctx)->c    = new_udp_client();
            strncpy((*ctx)->c->uc.uclient.server_ip, destIP, IP_LEN);
            (*ctx)->c->uc.uclient.server_port = destPort;
            (*ctx)->c->uc.uclient.connect_timeout_ms = 5000;
        }
        else if (0 == strncmp("mqtt", typeStr, CONNECT_TYPE_LEN))
        {
            (*ctx)->type = CLIENT_MQTT;
            (*ctx)->c    = new_mqtt_client();
            strncpy((*ctx)->c->uc.mclient.server_ip, destIP, IP_LEN);
            (*ctx)->c->uc.mclient.server_port = destPort;
            (*ctx)->c->uc.mclient.connect_timeout_s = 5;
        }
        else
        {
            return -1;
        }
    }

    return 0;
}

/**
*********************************************************************
* @brief��                  ͨ��ͨ������
* @param[in]��
               ctx          ͨ����д���
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_destroy(XL_CHANNEL_CTX_T *ctx)
{
    int ret = -1;
    if (ctx->type == CLIENT_MQTT)
    {
        del_mqtt_client(ctx->c);
        ret = 0;
    }
    else if (ctx->type == CLIENT_UDP)
    {
        del_udp_client(ctx->c);
        ret = 0;
    }
    else if (ctx->type == CLIENT_TCP)
    {
        del_tcp_client(ctx->c);
        ret = 0;
    }
    else if (ctx->type == CLIENT_IPC)
    {
    }

    return ret;
}

/**
*********************************************************************
* @brief��                  ͨ��ͨ����ʼ��
* @param[in]��
               ctx          ͨ����д���
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_init(XL_CHANNEL_CTX_T *ctx)
{
    return ctx->c->initFp(ctx->c);
}

/**
*********************************************************************
* @brief��                  ��IP���˿�    ����ѡ���ã�
* @param[in]��
               ctx          ͨ����д���
               ip           �󶨹̶�IP��ַ ��TCP UDP MQTT ģʽ��Ч��
               port         �󶨹̶�IP�˿�  ��TCP UDP MQTT ģʽ��Ч��
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_band_info(XL_CHANNEL_CTX_T *ctx, char *ip, uint16 port);

/**
*********************************************************************
* @brief��                  ��IPCͨѶ�ļ�·��    ����ѡ���ã�
* @param[in]��
               ctx          ͨ����д���
               ipcPath      ��ipc �ͻ���·����IPC ģʽ��Ч��
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_ipc_band_info(XL_CHANNEL_CTX_T *ctx, char *ipcPath);

/**
*********************************************************************
* @brief��                  ����MQTT �û�ID    ����ѡ���ã�
* @param[in]��
               ctx          ͨ����д���
               ipcPath      MQTT�û�ID��MQTT ģʽ��Ч��MQTT3.1 21�ֽڳ���
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_mqtt_user_ID(XL_CHANNEL_CTX_T *ctx, const char *id)
{
    strncpy(ctx->c->uc.mclient.user_id, id, sizeof(ctx->c->uc.mclient.user_id));
    return 0;
}

/**
*********************************************************************
* @brief��                  ����MQTT �û���    ����ѡ���ã�
* @param[in]��
               ctx          ͨ����д���
               username      MQTT�û����ƣ�MQTT ģʽ��Ч��MQTT3.1 21�ֽڳ���
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_mqtt_user_name(XL_CHANNEL_CTX_T *ctx,
                                     const char *      username);

/**
*********************************************************************
* @brief��                  ����MQTT �û�����    ����ѡ���ã�
* @param[in]��
               ctx          ͨ����д���
               passwd      MQTT�û����루MQTT ģʽ��Ч��
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_mqtt_user_passwd(XL_CHANNEL_CTX_T *ctx,
                                       const char *      passwd);

/**
*********************************************************************
* @brief��                  ����MQTT �û�Ĭ�Ϸ���Topic    ����ѡ���ã�
* @param[in]��
               ctx          ͨ����д���
               topic      MQTT topic��MQTT ģʽ��Ч��
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_mqtt_default_topic(XL_CHANNEL_CTX_T *ctx,
                                         const char *      topic)
{
    strncpy(ctx->c->uc.mclient.default_send_topic, topic,
            sizeof(ctx->c->uc.mclient.default_send_topic));
    return 0;
}

/**
*********************************************************************
* @brief��                  ����MQTT �û�Ĭ�Ϸ���TopicQOS    ����ѡ���ã�
* @param[in]��
               ctx          ͨ����д���
               qos      MQTT QOS��MQTT ģʽ��Ч��
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_mqtt_default_topic_qos(XL_CHANNEL_CTX_T *ctx,
                                         sint8     qos)
{
    // strncpy(ctx->c->uc.mclient.default_send_topic, topic,
    //         sizeof(ctx->c->uc.mclient.default_send_topic));
    ctx->c->uc.mclient.default_send_topic_qos = qos;

    return 0;
}

/**
*********************************************************************
* @brief��                  ����MQTT �û�����Topic    ����ѡ���ã�
* @param[in]��
               ctx          ͨ����д���
               topicNum     ���ı���ĸ���
               topic        topic ���� ����QOS
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_mqtt_sub_topic(XL_CHANNEL_CTX_T *ctx,
                                     sint32            topicNum,
                                     MQTT_TOPIC_T    (*topic)[])
{
    sint32 i = 0;
    if(topicNum > MAX_SUB_NUM)
    {
        return -1;
    }

    for(i = 0;i<topicNum;i++)
    {
        strncpy(ctx->c->uc.mclient.sub_topics[i].topic, (*topic)[i].topic, MAX_TOPIC_LEN);
        ctx->c->uc.mclient.sub_topics[i].qos = (*topic)[i].qos;
    }
    return 0;
}

/**
*********************************************************************
* @brief��                  ������Ŀ����񣨷������ӿڣ�����ѯ����״̬��
* @param[in]��
               ctx          ͨ����д���
* @param[out]��

* @return�� ����ֵ˵��          ����0 �ɹ� ����-1 ʧ��
*********************************************************************
*/
sint32 xl_channel_connect(XL_CHANNEL_CTX_T *ctx)
{
    return ctx->c->connectFp(ctx->c);
}

/**
*********************************************************************
* @brief��                  ��ȡ����״̬
* @param[in]��
               ctx          ͨ����д���
* @param[out]��

* @return�� ����ֵ˵��          INIT          ��ʼ�����
                                CH_CONNECTING   ������
                                CH_CONNECT_FAIL ����ʧ��
                                CH_CONNECT_SUCC ���ӳɹ�
*********************************************************************
*/
XL_CHANNEL_CONNECT_STATUS_E
xl_channel_get_connect_status(XL_CHANNEL_CTX_T *ctx)
{
    return ctx->c->getConnectStatusFp(ctx->c);
}

/**
*********************************************************************
* @brief��                  ��������
* @param[in]��
                ctx          ͨ����д���
                data         ���ͻ���������
                dataLen      ���ͻ��������ݳ���
* @param[out]��

* @return�� ����ֵ˵��       ����д�������  ����-1 ͨ���쳣
*********************************************************************
*/
sint32 xl_channel_write(XL_CHANNEL_CTX_T *ctx, const char *data, sint32 dataLen)
{
    return ctx->c->writeFp(ctx->c, data, dataLen);
}

/**
*********************************************************************
* @brief��                  ��������
* @param[in]��
                ctx          ͨ����д���
                topic        ������Ŀ��topic
                data         ���ͻ���������
                dataLen      ���ͻ��������ݳ���
* @param[out]��
                mid          ������Ϣ��ID

* @return�� ����ֵ˵��       ����д�������  ����-1 ͨ���쳣
*********************************************************************
*/
sint32 xl_channel_mqtt_write(XL_CHANNEL_CTX_T *ctx,
                             char *            topic,
                             sint32            qos,
                             sint32 *          mid,
                             const char *      data,
                             sint32            dataLen)
{
    return mqtt_clien_write_plus(ctx->c, topic, qos, mid, data, dataLen);
}

/**
*********************************************************************
* @brief��                  ��ȡ����
* @param[in]��
                ctx          ͨ����д���
                data         ��ȡ������
                dataLen      ��ȡ������������󳤶�
* @param[out]��

* @return�� ����ֵ˵��       ����0 ͨ��������δ����
                             ����-1 ͨ���쳣
                             ���� >0 �Ѷ�ȡ���ݵĴ�С
*********************************************************************
*/
sint32
xl_channel_read(XL_CHANNEL_CTX_T *ctx, char *data, sint32 dataMaxLen)
{
    return ctx->c->readFp(ctx->c, data, dataMaxLen);
}

/**
*********************************************************************
* @brief��                  ��ȡMQTT����
* @param[in]��
                ctx          ͨ����д���
                msecs        ��ȡ����ʱ����� 0 ������
* @param[out]��
                topic        �������ݱ��� ���ջ����� MAX_TOPIC_LEN
                qos          �������ݵ�QOS�Ǽ�
                mid          �������ݵ���ϢID
                data         ��ȡ������
                dataLen      ��ȡ������������󳤶�

* @return�� ����ֵ˵��       ����0 ͨ��������δ����
                             ����-1 ͨ���쳣
                             ���� >0 �Ѷ�ȡ���ݵĴ�С
*********************************************************************
*/
sint32 xl_channel_mqtt_read(XL_CHANNEL_CTX_T *ctx,
                            char *            topic,
                            sint32 *          qos,
                            sint32 *          mid,
                            char *            data,
                            sint32            dataLen,
                            long              msecs)
{
    /*
    sint32 mqtt_clien_read_plus(XL_CLIENT_T *c,
                                char *       topic,
                                sint32 *     qos,
                                sint32 *     mid,
                                char *       data,
                                sint32       dataLen,
                                long         msecs)

    */
    return mqtt_clien_read_plus(ctx->c, topic, qos, mid, data, dataLen, msecs);
}

/**
*********************************************************************
* @brief��                  �ر��������������
* @param[in]��
               ctx          ͨ����д���
* @param[out]��

* @return�� ����ֵ˵��       NULL
*********************************************************************
*/
void xl_channel_disconnect(XL_CHANNEL_CTX_T *ctx)
{
    ctx->c->disconnectFp(ctx->c);
}

/**
*********************************************************************
* @brief��                  ����MQTT Reconnect Timeout
* @param[in]��
               ctx          ͨ����д���
               timeout      seconds
* @param[out]��

* @return�� ����ֵ˵��      0 �ɹ�      -1 ʧ��
*********************************************************************
*/
sint32 xl_channel_set_mqtt_reconnect_timeout(XL_CHANNEL_CTX_T *ctx, sint32 timeout)
{
    ctx->c->uc.mclient.connect_timeout_s = timeout;
    return 0;
}