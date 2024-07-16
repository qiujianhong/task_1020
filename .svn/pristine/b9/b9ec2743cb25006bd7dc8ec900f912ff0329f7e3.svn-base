/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： udpService.c
           实现udp通讯接口
* @date： 2019-11-9
*********************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "msgHelper.h"
#include "framecomm.h"
#include "xdrCustom.h"

/******************************************************************
        消息体报文格式
|  1   2   3   4   5   6   7   8  |   size |
-----------------------------------
|       PROORITY             |PRM |    1   |
----------------------------------|
|             INDEX               |    2   |
----------------------------------|
|             LABEL               |    2   |
----------------------------------|
|             SOURCE              |    N   |  // '\0'结束
----------------------------------|
|           DESTINATION           |    N   |  // '\0'结束
----------------------------------|
|              TAG                |    4   |
----------------------------------|
|              LEN                |    N   |  //变长 规范参考A-XDR
----------------------------------|
|             PAYLOAD             |    N   |
*********************************************************************/
/**
*********************************************************************
* @brief： 消息解析
* @param[in]：
               rxbuf         接收消息入口地址
               rxbuf_len     消息长度
* @param[out]：
               msgOjb        消息解析后内容
* @return： 返回值说明
*********************************************************************
*/
sint32 parser_msg(uint8 *rxbuf, sint32 rxbuf_len, MSG_OBJ_T *msgObj)
{
    sint32 ret = 0;

    UNUSED(rxbuf_len);

    //获取的PRM范围为0 ~1
    msgObj->prm = GET_PRM(rxbuf);
    //获取的PRIORITY范围为0 ~127
    msgObj->priority = GET_PRIORITY(rxbuf);
    //获取的INDEX 范围为0 ~ 65535
    msgObj->index = GET_INDEX(rxbuf);
    //获取的LABEL 范围为0 ~ 65535
    msgObj->label = GET_LABEL(rxbuf);
    //获取源端名称
    msgObj->source = (sint8 *)GET_SOURCE(rxbuf);
    //获取目标端名称
    msgObj->destination = (sint8 *)(GET_DESTINATION(rxbuf));
    //获取TAG地址传入参数destination 地址
    uint8 *tag_addr = (uint8 *)GET_TAG_ADDR(msgObj->destination);
    //获取TAG值
    msgObj->tag     = GET_TAG(tag_addr);
    uint8 *len_addr = GET_LENGTH_ADDR(tag_addr);
    //获取长度信息
    msgObj->length = GET_LENGTH(len_addr);
    //当消息长度大于0x80 改为变长长度 此时偏移量为MSG_PYLOAD_OFFSET_3（3）
    //否则为（1）
    msgObj->payload = len_addr + (msgObj->length < 0x80 ? MSG_PYLOAD_OFFSET_1 : MSG_PYLOAD_OFFSET_3);

#ifdef MSG_HEADER_DEBUG
    MQTTMSG_FMT_DEBUG("msgObj->prm = %d\n", msgObj->prm);
    MQTTMSG_FMT_DEBUG("msgObj->priority = %d\n", msgObj->priority);
    MQTTMSG_FMT_DEBUG("msgObj->index = %d\n", msgObj->index);
    MQTTMSG_FMT_DEBUG("msgObj->label = %d\n", msgObj->label);
    MQTTMSG_FMT_DEBUG("msgObj->source = %s\n", msgObj->source);
    MQTTMSG_FMT_DEBUG("msgObj->destination = %s\n", msgObj->destination);
    MQTTMSG_FMT_DEBUG("msgObj->prm = 0x%x\n", msgObj->tag);
    MQTTMSG_FMT_DEBUG("msgObj->length = %d\n", msgObj->length);
#endif
    return ret;
}

/**
*********************************************************************
* @brief： 消息解析
* @param[in]：
               rxbuf         接收消息入口地址
               rxbuf_len     消息长度
* @param[out]：
               msgOjb        消息解析后内容
* @return： 返回值说明
*********************************************************************
*/
sint32 pro_parser_msg(uint8 *rxbuf, uint32 rxbuf_len, MSG_OBJ_T *msgObj)
{
    sint32 ret            = -1;
    uint32 sourceLen      = 0;
    uint32 destinationLen = 0;
    uint32 msgLen         = 0;
    uint32 payloadOffset  = 0;

    do
    {
        //获取的PRM范围为0 ~1
        msgObj->prm = GET_PRM(rxbuf);
        //获取的PRIORITY范围为0 ~127
        msgObj->priority = GET_PRIORITY(rxbuf);
        //获取的INDEX 范围为0 ~ 65535
        msgObj->index = GET_INDEX(rxbuf);
        //获取的LABEL 范围为0 ~ 65535
        msgObj->label = GET_LABEL(rxbuf);
        //获取源端名称
        msgObj->source = (sint8 *)GET_SOURCE(rxbuf);
        sourceLen      = strlen(msgObj->source);
        if (sourceLen > MAX_SOURCE_LENGTH)
        {
            MQTTMSG_FMT_DEBUG("msg source len[%d] > MAX_SOURCE_LENGTH\n", sourceLen);
            break;
        }

        if ((uint8 *)(msgObj->source) - rxbuf + sourceLen >= rxbuf_len)
        {
            MQTTMSG_FMT_DEBUG("msg source too long\n");
            break;
        }

        //获取目标端名称
        msgObj->destination = (sint8 *)(GET_DESTINATION(rxbuf));
        destinationLen      = strlen(msgObj->destination);
        if (destinationLen > MAX_DESTINATION_LENGTH)
        {
            MQTTMSG_FMT_DEBUG("msg destinationLen len[%d] > MAX_DESTINATION_LENGTH\n", destinationLen);
            break;
        }

        if ((uint8 *)(msgObj->destination) - rxbuf + destinationLen >= rxbuf_len)
        {
            MQTTMSG_FMT_DEBUG("msg destination too long\n");
            break;
        }

        //容错，消息的源和目的端 不能相同
        if (sourceLen == destinationLen && memcmp(msgObj->source, msgObj->destination, sourceLen) == 0)
        {
            MQTTMSG_FMT_DEBUG("msg source and destination is identical\n");
            break;
        }

        //获取TAG地址传入参数destination 地址
        uint8 *tag_addr = (uint8 *)GET_TAG_ADDR(msgObj->destination);
        if ((uint32)(tag_addr - rxbuf + 1) >= rxbuf_len)
        {
            MQTTMSG_FMT_DEBUG("msg err 1\n");
            break;
        }

        if ((uint32)(tag_addr - rxbuf) >= rxbuf_len)
        {
            break;
        }

        //获取TAG值
        msgObj->tag = GET_TAG(tag_addr);

        uint8 *len_addr = GET_LENGTH_ADDR(tag_addr);

        if ((uint32)(len_addr - rxbuf) >= rxbuf_len)
        {
            break;
        }

        if (len_addr[0] == 0x82)
        {
            payloadOffset = MSG_PYLOAD_OFFSET_3;
            msgObj->length = (len_addr[1]<<8) + len_addr[2];
        }
        else if(len_addr[0] == 0x81)
        {
            payloadOffset = MSG_PYLOAD_OFFSET_2;
            msgObj->length = len_addr[1];
        }
        else
        {
            payloadOffset = MSG_PYLOAD_OFFSET_1;
            msgObj->length = len_addr[0];
        }

        if ((uint32)(len_addr + payloadOffset - rxbuf) > rxbuf_len)
        {
            break;
        }

        //获取长度信息
//        msgObj->length = GET_LENGTH(len_addr);

        //当消息长度大于0x80 改为变长长度 此时偏移量为MSG_PYLOAD_OFFSET_3（3）
        //否则为（1）
        msgObj->payload = len_addr + payloadOffset;
        msgLen          = msgObj->payload - rxbuf + msgObj->length;

        // if (msgLen > 1000)
        // {
        //     MQTTMSG_FMT_DEBUG("msg err 2 msgLen:[%u] rxbuf_len:[%u]", msgLen,
        //                 rxbuf_len);
        //     hMQTTMSG_FMT_DEBUG(rxbuf, rxbuf_len);
        // }

        if (msgLen > rxbuf_len)
        {
            MQTTMSG_BUF_DEBUG(rxbuf, rxbuf_len, "msg err 2 msgLen:[%u] rxbuf_len:[%u]", msgLen, rxbuf_len);
            break;
        }

        ret = msgLen;

// #define MSG_HEADER_DEBUG
#ifdef MSG_HEADER_DEBUG
        MQTTMSG_FMT_DEBUG("msgObj->prm = %d\n", msgObj->prm);
        MQTTMSG_FMT_DEBUG("msgObj->priority = %d\n", msgObj->priority);
        MQTTMSG_FMT_DEBUG("msgObj->index = %d\n", msgObj->index);
        MQTTMSG_FMT_DEBUG("msgObj->label = %d\n", msgObj->label);
        MQTTMSG_FMT_DEBUG("msgObj->source = %s\n", msgObj->source);
        MQTTMSG_FMT_DEBUG("msgObj->destination = %s\n", msgObj->destination);
        MQTTMSG_FMT_DEBUG("msgObj->tag = 0x%x\n", msgObj->tag);
        MQTTMSG_FMT_DEBUG("msgObj->length = %d\n", msgObj->length);
#endif

    } while (0);

    return ret;
}

/**
*********************************************************************
* @brief： 构造消息
* @param[in]：
               msgObj         消息头
               payload        消息载荷
               payload_len    消息载荷长度
* @param[out]：
               rsp            响应
* @return： 返回值说明
*********************************************************************
*/
sint32 build_msg(MSG_OBJ_T *msgObj, uint8 *payload, sint32 payload_len, RAW_OBJ_T *rsp)
{
    uint8 *source_addr      = NULL;
    uint8 *des_addr         = NULL;
    uint8 *tag_addr         = NULL;
    uint8 *len_add          = NULL;
    uint8 source_len        = 0;
    uint8 des_len           = 0;
    uint8 *msgHeader        = rsp->payload;
    uint8 *xdr_payload_addr = NULL;

    // PRM 及 priority 设置
    msgHeader[MSG_PRM_OFFSET] = (msgObj->prm & 0x01) | (msgObj->priority << 1);

#if defined MSG_BIG_ENCODE
    // index 设置
    msgHeader[MSG_INDEX_OFFSET]     = msgObj->index >> 8;  // INDEX 高字节在前大端序
    msgHeader[MSG_INDEX_OFFSET + 1] = msgObj->index & 0xff;

    // label 设置
    msgHeader[MSG_LABEL_OFFSET]     = msgObj->label >> 8;  // LABEL 高字节在前大端序
    msgHeader[MSG_LABEL_OFFSET + 1] = msgObj->label & 0xff;
#else
    // index 设置
    msgHeader[MSG_INDEX_OFFSET]     = msgObj->index & 0xff;  // INDEX 低字节在前小端序
    msgHeader[MSG_INDEX_OFFSET + 1] = msgObj->index >> 8;

    // label 设置
    msgHeader[MSG_LABEL_OFFSET]     = msgObj->label & 0xff;  // LABEL 低字节在前小端序
    msgHeader[MSG_LABEL_OFFSET + 1] = msgObj->label >> 8;
#endif

    //源地址设置
    source_addr = msgHeader + MSG_SOURCE_OFFSET;  //先计算source地址
    source_len  = strlen(msgObj->source);
    strncpy((sint8 *)source_addr, (sint8 *)(msgObj->source),
            source_len);  //对source进行copy

    //目标地址设置
    des_addr = source_addr + source_len + 1;  //先计算destination地址
    des_len  = strlen(msgObj->destination);
    strncpy((sint8 *)des_addr, (sint8 *)(msgObj->destination),
            des_len);  //对destination进行copy
    tag_addr = des_addr + des_len + 1;

#if defined MSG_BIG_ENCODE
    // tag值设置
    tag_addr[0] = (msgObj->tag >> 24) & 0xff;  //大端序高字节在前
    tag_addr[1] = (msgObj->tag >> 16) & 0xff;
    tag_addr[2] = (msgObj->tag >> 8) & 0xff;
    tag_addr[3] = msgObj->tag & 0xff;

#else

    tag_addr[3] = (msgObj->tag >> 24) & 0xff;  //大端序高字节在前
    tag_addr[2] = (msgObj->tag >> 16) & 0xff;
    tag_addr[1] = (msgObj->tag >> 8) & 0xff;
    tag_addr[0] = msgObj->tag & 0xff;

#endif
    len_add = tag_addr + 4;
    //数据载荷设置
    if (payload_len < XDR_SIGNEL_LEN)  //长度小于0x80 单字节长度偏移为1
    {
        len_add[0]       = payload_len;
        xdr_payload_addr = len_add + MSG_PYLOAD_OFFSET_1;
    }
    else  //长度大于0x80采用变长长度
    {
        len_add[0]       = 0x82;
        len_add[1]       = (payload_len >> 8) & 0xff;  //变长长度稿子忽而组全部
        len_add[2]       = payload_len & 0xff;
        xdr_payload_addr = len_add + MSG_PYLOAD_OFFSET_3;
    }

    memcpy(xdr_payload_addr, payload, payload_len);
    rsp->length = xdr_payload_addr - msgHeader + payload_len;
    return rsp->length;
}

/**
*********************************************************************
* @brief： 构造注册消息
* @param[in]：
               msgObj         消息头
               mqt            mqtt消息
* @param[out]：
               rsp            响应
* @return： 返回值说明
*********************************************************************
*/
sint32 build_login_msg(MSG_OBJ_T *msgObj, MQT_PLUGIN_T *mqt, RAW_OBJ_T *rsp)
{
    uint8 payload[MAX_PAYLOAD_LEN] = {};
    uint8 *tmp                     = payload;
    push_u32(&tmp, (sint32)getpid());
    push_mqt_plugin(&tmp, mqt);
    return build_msg(msgObj, payload, tmp - payload, rsp);
}

/**
*********************************************************************
* @brief： 消息类型转换
* @param[in]：
               type         消息类型
* @param[out]：
              none
* @return： 返回值说明   对应字符串
*********************************************************************
*/
const sint8 *str_msg_type(MSG_TYPE_E type)
{
    const char *ret = "NULL";
    switch (type)
    {
        case UDP:
            ret = "UDP";
            break;
        case TCP:
            ret = "TCP";
            break;
        case IPC:
            ret = "IPC";
            break;
        case MQTT:
            ret = "MQTT";
            break;
        default:
            ret = "NULL";
            break;
    }

    return ret;
}