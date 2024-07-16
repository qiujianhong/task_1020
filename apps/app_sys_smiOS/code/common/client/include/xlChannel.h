/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved.
* @brief： xlChannel.h
* @date： 2020-6-10
*********************************************************************
*/
#ifndef __XL_CHANNEL_H__
#define __XL_CHANNEL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #include "baseConstant.h"
#include "baseType.h"
#include "channelStatus.h"
#include "xlClient.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _xl_channel_ctx
{

    XL_CLIENT_T * c;
    CLIENT_TYPE_E type;

} XL_CHANNEL_CTX_T;

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
sint32 xl_channel_create(const char *url, XL_CHANNEL_CTX_T **ctx);

/**
*********************************************************************
* @brief：                  通用通道销毁
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_destroy(XL_CHANNEL_CTX_T *ctx);

/**
*********************************************************************
* @brief：                  通用通道初始化
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明      0 成功      -1 失败
*********************************************************************
*/
sint32 xl_channel_init(XL_CHANNEL_CTX_T *ctx);

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
sint32 xl_channel_set_mqtt_user_ID(XL_CHANNEL_CTX_T *ctx, const char *id);

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
                                         const char *      topic);

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
                                         sint8     qos);

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
                                     MQTT_TOPIC_T    (*topic)[]);

/**
*********************************************************************
* @brief：                  连接至目标服务（非阻塞接口，需轮询连接状态）
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明          返回0 成功 返回-1 失败
*********************************************************************
*/
sint32 xl_channel_connect(XL_CHANNEL_CTX_T *ctx);

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
xl_channel_get_connect_status(XL_CHANNEL_CTX_T *ctx);

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
sint32
xl_channel_write(XL_CHANNEL_CTX_T *ctx, const char *data, sint32 dataLen);

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
                             sint32            dataLen);

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
sint32 xl_channel_read(XL_CHANNEL_CTX_T *ctx, char *data, sint32 dataMaxLen);

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
                            long              msecs);

/**
*********************************************************************
* @brief：                  关闭与服务器的连接
* @param[in]：
               ctx          通道读写句柄
* @param[out]：

* @return： 返回值说明       NULL
*********************************************************************
*/
void xl_channel_disconnect(XL_CHANNEL_CTX_T *ctx);


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
sint32 xl_channel_set_mqtt_reconnect_timeout(XL_CHANNEL_CTX_T *ctx, sint32 timeout);

#ifdef __cplusplus
}
#endif

#endif