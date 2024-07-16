/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： xlChannel.c
           通用channel读写接口
* @date： 2019-12-2
*********************************************************************
*/
#include "xlChannel.h"
#include "baseConstant.h"
#include "mqttClient.h"
#include "tcpClient.h"
#include "udpClient.h"

/**
*********************************************************************
* @brief：                  通用通道创建
* @param[in]：
               url          连接目标服务的url tcp://127.0.0.1:8001
                                              udp://127.0.0.1:8002
                                              mqtt://127.0.0.1:1883
                                              ipc://tmp/smiOS.sock

* @param[out]：
               ctx          通道读写句柄
* @return： 返回值说明      0 成功      -1 失败
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
* @brief：                  通用通道销毁
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
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
* @brief：                  通用通道初始化
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_init(XL_CHANNEL_CTX_T *ctx)
{
    return ctx->c->initFp(ctx->c);
}

/**
*********************************************************************
* @brief：                  绑定IP及端口    （可选调用）
* @param[in]：
               ctx          通道读写句柄
               ip           绑定固定IP地址 （TCP UDP MQTT 模式有效）
               port         绑定固定IP端口  （TCP UDP MQTT 模式有效）
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_set_band_info(XL_CHANNEL_CTX_T *ctx, char *ip, uint16 port);

/**
*********************************************************************
* @brief：                  绑定IPC通讯文件路径    （可选调用）
* @param[in]：
               ctx          通道读写句柄
               ipcPath      绑定ipc 客户端路径（IPC 模式有效）
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_set_ipc_band_info(XL_CHANNEL_CTX_T *ctx, char *ipcPath);

/**
*********************************************************************
* @brief：                  设置MQTT 用户ID    （可选调用）
* @param[in]：
               ctx          通道读写句柄
               ipcPath      MQTT用户ID（MQTT 模式有效）MQTT3.1 21字节长度
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_set_mqtt_user_ID(XL_CHANNEL_CTX_T *ctx, const char *id)
{
    strncpy(ctx->c->uc.mclient.user_id, id, sizeof(ctx->c->uc.mclient.user_id));
    return 0;
}

/**
*********************************************************************
* @brief：                  设置MQTT 用户名    （可选调用）
* @param[in]：
               ctx          通道读写句柄
               username      MQTT用户名称（MQTT 模式有效）MQTT3.1 21字节长度
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_set_mqtt_user_name(XL_CHANNEL_CTX_T *ctx,
                                     const char *      username);

/**
*********************************************************************
* @brief：                  设置MQTT 用户密码    （可选调用）
* @param[in]：
               ctx          通道读写句柄
               passwd      MQTT用户密码（MQTT 模式有效）
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_set_mqtt_user_passwd(XL_CHANNEL_CTX_T *ctx,
                                       const char *      passwd);

/**
*********************************************************************
* @brief：                  设置MQTT 用户默认发送Topic    （可选调用）
* @param[in]：
               ctx          通道读写句柄
               topic      MQTT topic（MQTT 模式有效）
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
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
* @brief：                  设置MQTT 用户默认发送TopicQOS    （可选调用）
* @param[in]：
               ctx          通道读写句柄
               qos      MQTT QOS（MQTT 模式有效）
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
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
* @brief：                  设置MQTT 用户订阅Topic    （可选调用）
* @param[in]：
               ctx          通道读写句柄
               topicNum     订阅标题的个数
               topic        topic 对象 包含QOS
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
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
* @brief：                  连接至目标服务（非阻塞接口，需轮询连接状态）
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明          返回0 成功 返回-1 失败
*********************************************************************
*/
sint32 xl_channel_connect(XL_CHANNEL_CTX_T *ctx)
{
    return ctx->c->connectFp(ctx->c);
}

/**
*********************************************************************
* @brief：                  获取连接状态
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明          INIT          初始化完成
                                CH_CONNECTING   连接中
                                CH_CONNECT_FAIL 连接失败
                                CH_CONNECT_SUCC 连接成功
*********************************************************************
*/
XL_CHANNEL_CONNECT_STATUS_E
xl_channel_get_connect_status(XL_CHANNEL_CTX_T *ctx)
{
    return ctx->c->getConnectStatusFp(ctx->c);
}

/**
*********************************************************************
* @brief：                  发送数据
* @param[in]：
                ctx          通道读写句柄
                data         发送缓冲区数据
                dataLen      发送缓冲区数据长度
* @param[out]：

* @return： 返回值说明       返回写入的数量  返回-1 通道异常
*********************************************************************
*/
sint32 xl_channel_write(XL_CHANNEL_CTX_T *ctx, const char *data, sint32 dataLen)
{
    return ctx->c->writeFp(ctx->c, data, dataLen);
}

/**
*********************************************************************
* @brief：                  发送数据
* @param[in]：
                ctx          通道读写句柄
                topic        发送至目标topic
                data         发送缓冲区数据
                dataLen      发送缓冲区数据长度
* @param[out]：
                mid          发送消息的ID

* @return： 返回值说明       返回写入的数量  返回-1 通道异常
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
* @brief：                  读取数据
* @param[in]：
                ctx          通道读写句柄
                data         读取缓冲区
                dataLen      读取缓冲区数据最大长度
* @param[out]：

* @return： 返回值说明       返回0 通道内数据未就绪
                             返回-1 通道异常
                             返回 >0 已读取数据的大小
*********************************************************************
*/
sint32
xl_channel_read(XL_CHANNEL_CTX_T *ctx, char *data, sint32 dataMaxLen)
{
    return ctx->c->readFp(ctx->c, data, dataMaxLen);
}

/**
*********************************************************************
* @brief：                  读取MQTT数据
* @param[in]：
                ctx          通道读写句柄
                msecs        读取阻塞时间毫秒 0 非阻塞
* @param[out]：
                topic        接收数据标题 接收缓冲区 MAX_TOPIC_LEN
                qos          接收数据的QOS登记
                mid          接收数据的消息ID
                data         读取缓冲区
                dataLen      读取缓冲区数据最大长度

* @return： 返回值说明       返回0 通道内数据未就绪
                             返回-1 通道异常
                             返回 >0 已读取数据的大小
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
* @brief：                  关闭与服务器的连接
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明       NULL
*********************************************************************
*/
void xl_channel_disconnect(XL_CHANNEL_CTX_T *ctx)
{
    ctx->c->disconnectFp(ctx->c);
}

/**
*********************************************************************
* @brief：                  设置MQTT Reconnect Timeout
* @param[in]：
               ctx          通道读写句柄
               timeout      seconds
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_set_mqtt_reconnect_timeout(XL_CHANNEL_CTX_T *ctx, sint32 timeout)
{
    ctx->c->uc.mclient.connect_timeout_s = timeout;
    return 0;
}