/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� udpService.c
           ʵ��udpͨѶ�ӿ�
* @date�� 2019-11-9
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
        ��Ϣ�屨�ĸ�ʽ
|  1   2   3   4   5   6   7   8  |   size |
-----------------------------------
|       PROORITY             |PRM |    1   |
----------------------------------|
|             INDEX               |    2   |
----------------------------------|
|             LABEL               |    2   |
----------------------------------|
|             SOURCE              |    N   |  // '\0'����
----------------------------------|
|           DESTINATION           |    N   |  // '\0'����
----------------------------------|
|              TAG                |    4   |
----------------------------------|
|              LEN                |    N   |  //�䳤 �淶�ο�A-XDR
----------------------------------|
|             PAYLOAD             |    N   |
*********************************************************************/
/**
*********************************************************************
* @brief�� ��Ϣ����
* @param[in]��
               rxbuf         ������Ϣ��ڵ�ַ
               rxbuf_len     ��Ϣ����
* @param[out]��
               msgOjb        ��Ϣ����������
* @return�� ����ֵ˵��
*********************************************************************
*/
sint32 parser_msg(uint8 *rxbuf, sint32 rxbuf_len, MSG_OBJ_T *msgObj)
{
    sint32 ret = 0;

    UNUSED(rxbuf_len);

    //��ȡ��PRM��ΧΪ0 ~1
    msgObj->prm = GET_PRM(rxbuf);
    //��ȡ��PRIORITY��ΧΪ0 ~127
    msgObj->priority = GET_PRIORITY(rxbuf);
    //��ȡ��INDEX ��ΧΪ0 ~ 65535
    msgObj->index = GET_INDEX(rxbuf);
    //��ȡ��LABEL ��ΧΪ0 ~ 65535
    msgObj->label = GET_LABEL(rxbuf);
    //��ȡԴ������
    msgObj->source = (sint8 *)GET_SOURCE(rxbuf);
    //��ȡĿ�������
    msgObj->destination = (sint8 *)(GET_DESTINATION(rxbuf));
    //��ȡTAG��ַ�������destination ��ַ
    uint8 *tag_addr = (uint8 *)GET_TAG_ADDR(msgObj->destination);
    //��ȡTAGֵ
    msgObj->tag     = GET_TAG(tag_addr);
    uint8 *len_addr = GET_LENGTH_ADDR(tag_addr);
    //��ȡ������Ϣ
    msgObj->length = GET_LENGTH(len_addr);
    //����Ϣ���ȴ���0x80 ��Ϊ�䳤���� ��ʱƫ����ΪMSG_PYLOAD_OFFSET_3��3��
    //����Ϊ��1��
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
* @brief�� ��Ϣ����
* @param[in]��
               rxbuf         ������Ϣ��ڵ�ַ
               rxbuf_len     ��Ϣ����
* @param[out]��
               msgOjb        ��Ϣ����������
* @return�� ����ֵ˵��
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
        //��ȡ��PRM��ΧΪ0 ~1
        msgObj->prm = GET_PRM(rxbuf);
        //��ȡ��PRIORITY��ΧΪ0 ~127
        msgObj->priority = GET_PRIORITY(rxbuf);
        //��ȡ��INDEX ��ΧΪ0 ~ 65535
        msgObj->index = GET_INDEX(rxbuf);
        //��ȡ��LABEL ��ΧΪ0 ~ 65535
        msgObj->label = GET_LABEL(rxbuf);
        //��ȡԴ������
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

        //��ȡĿ�������
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

        //�ݴ���Ϣ��Դ��Ŀ�Ķ� ������ͬ
        if (sourceLen == destinationLen && memcmp(msgObj->source, msgObj->destination, sourceLen) == 0)
        {
            MQTTMSG_FMT_DEBUG("msg source and destination is identical\n");
            break;
        }

        //��ȡTAG��ַ�������destination ��ַ
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

        //��ȡTAGֵ
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

        //��ȡ������Ϣ
//        msgObj->length = GET_LENGTH(len_addr);

        //����Ϣ���ȴ���0x80 ��Ϊ�䳤���� ��ʱƫ����ΪMSG_PYLOAD_OFFSET_3��3��
        //����Ϊ��1��
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
* @brief�� ������Ϣ
* @param[in]��
               msgObj         ��Ϣͷ
               payload        ��Ϣ�غ�
               payload_len    ��Ϣ�غɳ���
* @param[out]��
               rsp            ��Ӧ
* @return�� ����ֵ˵��
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

    // PRM �� priority ����
    msgHeader[MSG_PRM_OFFSET] = (msgObj->prm & 0x01) | (msgObj->priority << 1);

#if defined MSG_BIG_ENCODE
    // index ����
    msgHeader[MSG_INDEX_OFFSET]     = msgObj->index >> 8;  // INDEX ���ֽ���ǰ�����
    msgHeader[MSG_INDEX_OFFSET + 1] = msgObj->index & 0xff;

    // label ����
    msgHeader[MSG_LABEL_OFFSET]     = msgObj->label >> 8;  // LABEL ���ֽ���ǰ�����
    msgHeader[MSG_LABEL_OFFSET + 1] = msgObj->label & 0xff;
#else
    // index ����
    msgHeader[MSG_INDEX_OFFSET]     = msgObj->index & 0xff;  // INDEX ���ֽ���ǰС����
    msgHeader[MSG_INDEX_OFFSET + 1] = msgObj->index >> 8;

    // label ����
    msgHeader[MSG_LABEL_OFFSET]     = msgObj->label & 0xff;  // LABEL ���ֽ���ǰС����
    msgHeader[MSG_LABEL_OFFSET + 1] = msgObj->label >> 8;
#endif

    //Դ��ַ����
    source_addr = msgHeader + MSG_SOURCE_OFFSET;  //�ȼ���source��ַ
    source_len  = strlen(msgObj->source);
    strncpy((sint8 *)source_addr, (sint8 *)(msgObj->source),
            source_len);  //��source����copy

    //Ŀ���ַ����
    des_addr = source_addr + source_len + 1;  //�ȼ���destination��ַ
    des_len  = strlen(msgObj->destination);
    strncpy((sint8 *)des_addr, (sint8 *)(msgObj->destination),
            des_len);  //��destination����copy
    tag_addr = des_addr + des_len + 1;

#if defined MSG_BIG_ENCODE
    // tagֵ����
    tag_addr[0] = (msgObj->tag >> 24) & 0xff;  //�������ֽ���ǰ
    tag_addr[1] = (msgObj->tag >> 16) & 0xff;
    tag_addr[2] = (msgObj->tag >> 8) & 0xff;
    tag_addr[3] = msgObj->tag & 0xff;

#else

    tag_addr[3] = (msgObj->tag >> 24) & 0xff;  //�������ֽ���ǰ
    tag_addr[2] = (msgObj->tag >> 16) & 0xff;
    tag_addr[1] = (msgObj->tag >> 8) & 0xff;
    tag_addr[0] = msgObj->tag & 0xff;

#endif
    len_add = tag_addr + 4;
    //�����غ�����
    if (payload_len < XDR_SIGNEL_LEN)  //����С��0x80 ���ֽڳ���ƫ��Ϊ1
    {
        len_add[0]       = payload_len;
        xdr_payload_addr = len_add + MSG_PYLOAD_OFFSET_1;
    }
    else  //���ȴ���0x80���ñ䳤����
    {
        len_add[0]       = 0x82;
        len_add[1]       = (payload_len >> 8) & 0xff;  //�䳤���ȸ��Ӻ�����ȫ��
        len_add[2]       = payload_len & 0xff;
        xdr_payload_addr = len_add + MSG_PYLOAD_OFFSET_3;
    }

    memcpy(xdr_payload_addr, payload, payload_len);
    rsp->length = xdr_payload_addr - msgHeader + payload_len;
    return rsp->length;
}

/**
*********************************************************************
* @brief�� ����ע����Ϣ
* @param[in]��
               msgObj         ��Ϣͷ
               mqt            mqtt��Ϣ
* @param[out]��
               rsp            ��Ӧ
* @return�� ����ֵ˵��
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
* @brief�� ��Ϣ����ת��
* @param[in]��
               type         ��Ϣ����
* @param[out]��
              none
* @return�� ����ֵ˵��   ��Ӧ�ַ���
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