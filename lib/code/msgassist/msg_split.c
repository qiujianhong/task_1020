/*
*********************************************************************
* Copyright(C) 2019 China Electric Power Research Institute
* All rights reserved. 
* @brief：   APP交互消息解析
* @date：    2020/1/3
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_split.h"

#include "debug_comm.h"
#include "cJSON.h"

#define IS_LITTLE_ENDIAN    1       //组消息的时候index label msgtag是否用小端模式 0为大端 1为小端
/**********************************************************************
* @name      : appmsg_memcpy_r
* @brief     ：倒序拷贝内存
* @param[in] ：void *dest        目的地址
               void *src         源地址
               int count         拷贝长度
* @param[out]：
* @return    ：目的地址
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
void *appmsg_memcpy_r(void *dest, void *src, int count)
{
    void *ptr = dest;
    
    if(NULL == dest || NULL == src || count <= 0)  
    {   
        return NULL;    
    }
    
    while(count--)
    {
        *(char *)dest++ = *(char *)(src+count); 
    }

    return ptr;
}

/**********************************************************************
* @name      : appmsg_get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：uint8 *bytes             便宜量
* @return    ：长度
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
uint16 appmsg_get_len_offset(uint8 *indata,uint8 *bytes)
{
    uint16 EleNum;
    uint8 nbytes = 1;
    
    EleNum = indata[0];
    if(EleNum==0x81)
    {
        EleNum = indata[1];
        nbytes = 2;
    }
    else if(EleNum==0x82)
    {
        EleNum = indata[1];
        EleNum <<= 8;
        EleNum += indata[2];
        nbytes = 3;
    }
    else if(EleNum==0x80 || EleNum>0x82)
    {
        return 0;
    }
    
    if(bytes != NULL)
        *bytes = nbytes;
        
    return EleNum;
}

/**********************************************************************
* @name      : appmsg_set_len_offset
* @brief     ：设置元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：const uint8 *indata      数据Buffer
* @return    ：长度
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
uint8 appmsg_set_len_offset(uint16 eleLen, uint8 *indata)
{
    if(eleLen < 0x80)
    {
        indata[0] = eleLen;
        return 1;
    }
    else if(eleLen <= 0xffff)
    {
        indata[0] = 0x82;
        indata[1] = (uint8)((eleLen >> 8) & 0xff);
        indata[2] = (uint8)(eleLen & 0xff);
        return 3;
    }
        
    return 0;
}

/**********************************************************************
* @name      : appmsg_get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：uint8 *bytes             便宜量
* @return    ：长度
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
uint16 get_len_offset(uint8 *indata,uint8 *bytes)
{
    return appmsg_get_len_offset(indata, bytes);
}

/**********************************************************************
* @name      : set_len_offset
* @brief     ：设置元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：const uint8 *indata      数据Buffer
* @return    ：长度
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
uint8 set_len_offset(uint16 eleLen, uint8 *indata)
{
    return appmsg_set_len_offset(eleLen, indata);
}

/**********************************************************************
* @name      : appmsg_axdr_split
* @brief     ：接收消息解析成 MESSAGE_MQTT_T
* @param[in] ：uint8 *pBuf               接受到的报文
               uint16 bufLen             报文长度
* @param[out]：MESSAGE_MQTT_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
int appmsg_axdr_split(uint8 *pBuf, uint16 bufLen, MESSAGE_MQTT_T **ppMsgOut)
{
    uint8  *pTemp = pBuf;
    uint8   offset = 0;
    uint16  tempLen = 0;
    MESSAGE_MQTT_T tMsgTemp;
    MESSAGE_MQTT_T *ptNewMsg = NULL;
    
    if(NULL == pBuf || NULL == ppMsgOut || 0 == bufLen)
    {
        return -1;
    }
    memset(&tMsgTemp, 0, sizeof(MESSAGE_MQTT_T));
    tMsgTemp.msgtype = MESSAGE_TYPE_A_XDR;

    /* PRM */
    tMsgTemp.tAXDR.rpm = (*pTemp) & 0x01;

    /* PRIORITY */
    tMsgTemp.tAXDR.priority = ((*pTemp)>>1) & 0x7f;

    /* INDEX */
    pTemp++;
#if IS_LITTLE_ENDIAN
    tMsgTemp.tAXDR.index = *((uint16 *)pTemp);
#else
    tMsgTemp.tAXDR.index = ntohs(*((uint16 *)pTemp));
#endif

    /* LABEL */
    pTemp += 2;
#if IS_LITTLE_ENDIAN
    tMsgTemp.tAXDR.label = *((uint16 *)pTemp);
#else
    tMsgTemp.tAXDR.label = ntohs(*((uint16 *)pTemp));
#endif


    /* SOURCE */
    pTemp += 2;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        DPRINT("SOURCE too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.tAXDR.souraddr, pTemp, tempLen);
    tMsgTemp.tAXDR.sourlen = tempLen;

    /* DESTINATION */
    pTemp += tempLen + 1;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        DPRINT("DESTINATION too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.tAXDR.destaddr, pTemp, tempLen);
    tMsgTemp.tAXDR.destlen = tempLen;

    /* TAG */
    pTemp += tempLen + 1;
#if IS_LITTLE_ENDIAN
    tMsgTemp.tAXDR.IID = *((uint16*)(pTemp+2));
    tMsgTemp.tAXDR.IOP = *((uint16*)pTemp);
#else
    tMsgTemp.tAXDR.IID = ((uint16)*pTemp)*256 + *(pTemp+1);
    tMsgTemp.tAXDR.IOP = ((uint16)*(pTemp+2))*256 + *(pTemp+3);
#endif

    /* MsgLen */
    pTemp += 4;
    tMsgTemp.tAXDR.msglen = appmsg_get_len_offset(pTemp, &offset);
    pTemp += offset;
    
    /* 拷贝 */
    ptNewMsg = (MESSAGE_MQTT_T *)malloc(sizeof(MESSAGE_MQTT_T) + tMsgTemp.tAXDR.msglen);
    if(NULL == ptNewMsg)
    {
        DPRINT("appmsg_udp_split malloc failed,size %d\n", (uint32)sizeof(MESSAGE_MQTT_T) + tMsgTemp.tAXDR.msglen);
        return -1;
    }
    
    memcpy(ptNewMsg, &tMsgTemp, sizeof(MESSAGE_MQTT_T));
    if(tMsgTemp.tAXDR.msglen > 0)
    {
        memcpy(ptNewMsg->tAXDR.playload, pTemp, tMsgTemp.tAXDR.msglen);
    }
    *ppMsgOut = ptNewMsg;

    return 0;
}

/**********************************************************************
* @name      : appmsg_json_split
* @brief     ：接收消息解析成 MESSAGE_JSON_T
* @param[in] ：uint8 *pBuf               接受到的报文
               uint16 bufLen             报文长度
               char *pTopic              消息主题
* @param[out]：MESSAGE_JSON_T *pMsgOut   解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
int appmsg_json_split(uint8 *pBuf, uint16 bufLen, char *pTopic, MESSAGE_JSON_T *pMsgOut)
{
    cJSON  *pObject = NULL;
    cJSON  *item = NULL;
    char   *temp = NULL;
    char   *temp1 = NULL;

    if(NULL == pBuf || NULL == pMsgOut || 0 == bufLen || NULL == pTopic)
    {
        return -1;
    }

    memset(pMsgOut, 0, sizeof(MESSAGE_JSON_T));
    
    /* 发送方 */
    temp = strstr(pTopic, "/");
    if(NULL == temp)
    {
        return -2;
    }
    memcpy(pMsgOut->sourapp, pTopic, MIN((uint32)(temp - pTopic), MESSAGE_APPNAME_LEN - 1));

    /* 接收方 */
    temp1 = temp + 1;
    temp = strstr(temp1, "/");
    if(NULL == temp)
    {
        return -2;
    }    
    memcpy(pMsgOut->destapp, temp1, MIN((uint32)(temp - temp1), MESSAGE_APPNAME_LEN - 1));

    /* JSON */
    temp1 = temp + 1;
    temp = strstr(temp1, "/");
    if(NULL == temp)
    {
        return -2;
    }
    
    /* operator */
    temp1 = temp + 1;
    temp = strstr(temp1, "/");
    if(NULL == temp)
    {
        return -2;
    }    
    memcpy(pMsgOut->operat, temp1, MIN((uint32)(temp - temp1), MESSAGE_OPERINFO_LEN));
    
    /* infoType */
    temp1 = temp + 1;
    temp = strstr(temp1, "/");
    if(NULL == temp)
    {
        /* 没有operator */
        memcpy(pMsgOut->infotype, pMsgOut->operat, MESSAGE_OPERINFO_LEN);
        memset(pMsgOut->operat, 0, MESSAGE_OPERINFO_LEN);
        
        /* infoPath */
        memcpy(pMsgOut->infopath, temp1, MIN(strlen(temp1), MESSAGE_INFOPATH_LEN));
    }
    else
    {
        memcpy(pMsgOut->infotype, temp1, MIN((uint32)(temp - temp1), MESSAGE_OPERINFO_LEN));

        /* infoPath */
        temp1 = temp + 1;
        memcpy(pMsgOut->infopath, temp1, MIN(strlen(temp1), MESSAGE_INFOPATH_LEN));
    }
    
    pObject = cJSON_Parse((char *)pBuf);
    if(NULL == pObject)
    {
        return -3;
    };
    item = cJSON_GetObjectItem(pObject, "token");
    if(NULL == item)
    {
        cJSON_Delete(pObject);
        return -4;
    }
    if(cJSON_String == item->type)
    {
        //memcpy(pMsgOut->token, item->valuestring, MIN(MESSAGE_TOKEN_LEN, strlen(item->valuestring)));
        pMsgOut->token = atoi(item->valuestring);
    }
    else if(cJSON_Number == item->type)
    {
        //sprintf(pMsgOut->token, "%u", item->valueint);
        pMsgOut->token = item->valueint;
    }
    
    item = cJSON_GetObjectItem(pObject, "timestamp");
    if(NULL == item)
    {
        cJSON_Delete(pObject);
        return -5;
    }
    if(NULL != item)
    {
        memcpy(pMsgOut->timestamp, item->valuestring, MIN(MESSAGE_TIME_LEN, strlen(item->valuestring)));
    }
    pMsgOut->pObject = pObject;

    return 0;
}

/**********************************************************************
* @name      : appmsg_mqtt_split
* @brief     ：接收消息解析成 MESSAGE_MQTT_T
* @param[in] ：uint8 *pBuf               接受到的报文内容
               uint32 bufLen             报文长度
               char *pTopic              主题(可以是空)
* @param[out]：MESSAGE_MQTT_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
int appmsg_mqtt_split(uint8 *pBuf, uint32 bufLen, char *pTopic, MESSAGE_MQTT_T **ppMsgOut)
{
    char    strtemp[16] = {0};
    char   *temp = NULL;
    char   *temp1 = NULL;

    if(NULL == pTopic)
    {
        /* 没有主题按照解析axdr格式 */
        return appmsg_axdr_split(pBuf, bufLen, ppMsgOut);
    }

    if (strstr(pTopic, "/Broadcast/IID/IOP/") != NULL)
    {
        /* 新广播消息 按照解析axdr格式 */
        return appmsg_axdr_split(pBuf, bufLen, ppMsgOut);
    }
    
    /* 发送方 */
    temp = strstr(pTopic, "/");
    if(NULL == temp)
    {
        return -1;
    }
    /* 接收方 */
    temp1 = temp + 1;
    temp = strstr(temp1, "/");
    if(NULL == temp)
    {
        if ((strstr(pTopic, MQTT_TOPIC_A_XDR) == NULL) && (strstr(pTopic, MQTT_TOPIC_JSON) == NULL))
        {
            /* 解析axdr格式 */
            return appmsg_axdr_split(pBuf, bufLen, ppMsgOut);
        }
        else
        {
            return -1;
        }
    }
    /* 编码规则 */
    temp1 = temp + 1;
    temp = strstr(temp1, "/");
    if(NULL == temp)
    {
        memcpy(strtemp, temp1, MIN(strlen(temp1), 16));
    }
    memcpy(strtemp, temp1, MIN((uint32)(temp - temp1), 16));

    if(0 == strcmp(strtemp, MQTT_TOPIC_A_XDR))
    {
        /* 解析axdr格式 */
        return appmsg_axdr_split(pBuf, bufLen, ppMsgOut);
    }
    else if(0 == strcmp(strtemp, MQTT_TOPIC_JSON))
    {
        MESSAGE_MQTT_T *pMsgOut = NULL;
        int             ret = 0;
        
        pMsgOut = (MESSAGE_MQTT_T *)malloc(sizeof(MESSAGE_MQTT_T));
        if(NULL == pMsgOut)
        {
            return -2;
        }
        memset(pMsgOut, 0, sizeof(MESSAGE_MQTT_T));
        pMsgOut->msgtype = MESSAGE_TYPE_JSON;
        ret = appmsg_json_split(pBuf, bufLen, pTopic, &(pMsgOut->tJson));
        if(0 != ret)
        {
            free(pMsgOut);
            return ret;
        }

        *ppMsgOut = pMsgOut;
        return 0;
    }

    return -3;
}

/**********************************************************************
* @name      : appmsg_mqtt_free
* @brief     ：清空消息
* @param[in] ：MESSAGE_MQTT_T *pMsgMQTT 消息
* @param[out]：
* @return    ：消息长度
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
void appmsg_mqtt_free(MESSAGE_MQTT_T *pMsgMQTT)
{
    if(NULL == pMsgMQTT)
    {
        return;
    }
    if(MESSAGE_TYPE_JSON == pMsgMQTT->msgtype && NULL != pMsgMQTT->tJson.pObject)
    {
        cJSON_Delete(pMsgMQTT->tJson.pObject);
        pMsgMQTT->tJson.pObject = NULL;
    }
    free(pMsgMQTT);
}

/**********************************************************************
* @name      : appmsg_free
* @brief     ：清空消息
* @param[in] ：MESSAGE_MQTT_T *pMsgMQTT 消息
* @param[out]：
* @return    ：消息长度
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
void appmsg_free(MESSAGE_MQTT_T *pMsgMQTT)
{
    if(NULL == pMsgMQTT)
    {
        return;
    }

    free(pMsgMQTT);
}

/**********************************************************************
* @name      : appmsg_json_free
* @brief     ：清空消息
* @param[in] ：MESSAGE_MQTT_T *pMsgMQTT 消息
* @param[out]：
* @return    ：消息长度
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
void appmsg_json_free(MESSAGE_MQTT_T *pMsgMQTT)
{
    if(NULL == pMsgMQTT)
    {
        return;
    }
    if(MESSAGE_TYPE_JSON == pMsgMQTT->msgtype && NULL != pMsgMQTT->tJson.pObject)
    {
        cJSON_Delete(pMsgMQTT->tJson.pObject);
        pMsgMQTT->tJson.pObject = NULL;
    }
}

/**********************************************************************
* @name      : appmsg_axdr_package
* @brief     ：指定格式消息组装成
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  结构消息
* @param[out]：uint8 **ppBuf             组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2021-7-7
* @Update    :
**********************************************************************/
uint16 appmsg_axdr_package(MESSAGE_INFO_T *pMsgInfo, uint8 **ppBuf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = NULL;
    uint8    *pTemp = NULL;
    
    bufLen = 1 + 2 + 2 + pMsgInfo->sourlen + 1 + pMsgInfo->destlen + 1 + 4;   //计算head和Tag长度

    /* 计算MSG's-Length 长度  */
    offset = appmsg_set_len_offset(pMsgInfo->msglen, lenBuf);
    if(0 == offset)
    {
        DPRINT("appmsg_udp_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->msglen;

    pMsgBuf = (uint8 *)malloc(bufLen);
    if(NULL == pMsgBuf)
    {
        DPRINT("appmsg_udp_package malloc failed,size %p\n", pMsgBuf);
        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->priority << 1) & 0xfe) + (pMsgInfo->rpm & 0x01);
    pTemp++;

    /* INDEX */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->index;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->index);
#endif
    pTemp += sizeof(uint16);

    /* LABEL */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->label;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->label);
#endif
    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->souraddr, pMsgInfo->sourlen);
    pTemp += pMsgInfo->sourlen + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->destaddr, pMsgInfo->destlen);
    pTemp += pMsgInfo->destlen + 1;

    /* MSG's-TAG */
#if IS_LITTLE_ENDIAN
    *((uint32 *)pTemp) =((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff);
#else
    *((uint32 *)pTemp) = htonl(((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff));
#endif

    pTemp += sizeof(uint32);

    /* MSG's-Length */
    memcpy(pTemp, lenBuf, offset);
    pTemp += offset;

    if(pMsgInfo->msglen)
    {
        memcpy(pTemp, pMsgInfo->playload, pMsgInfo->msglen);  //MSG's-Payload
    }

    *ppBuf = pMsgBuf;
    return bufLen;
}

/**********************************************************************
* @name      : appmsg_split
* @brief     ：接收消息解析成 MESSAGE_INFO_T
* @param[in] ：uint8 *pBuf           接受到的报文
               uint16 bufLen         报文长度
* @param[out]：MSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
int appmsg_split(uint8 *pBuf, uint16 bufLen, MESSAGE_INFO_T **ppMsgOut)
{
    uint8  *pTemp = pBuf;
    uint8   offset = 0;
    uint16  tempLen = 0;
    MESSAGE_INFO_T tMsgTemp;
    MESSAGE_INFO_T *ptNewMsg = NULL;
    
    if(NULL == pBuf || NULL == ppMsgOut || 0 == bufLen)
    {
        return -1;
    }
    memset(&tMsgTemp, 0, sizeof(MESSAGE_INFO_T));

    /* PRM */
    tMsgTemp.rpm = (*pTemp) & 0x01;

    /* PRIORITY */
    tMsgTemp.priority = ((*pTemp)>>1) & 0x7f;

    /* INDEX */
    pTemp++;
#if IS_LITTLE_ENDIAN
    tMsgTemp.index = *((uint16 *)pTemp);
#else
    tMsgTemp.index = ntohs(*((uint16 *)pTemp));
#endif

    /* LABEL */
    pTemp += 2;
#if IS_LITTLE_ENDIAN
    tMsgTemp.label = *((uint16 *)pTemp);
#else
    tMsgTemp.label = ntohs(*((uint16 *)pTemp));
#endif


    /* SOURCE */
    pTemp += 2;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        DPRINT("SOURCE too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.souraddr, pTemp, tempLen);
    tMsgTemp.sourlen = tempLen;

    /* DESTINATION */
    pTemp += tempLen + 1;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        DPRINT("DESTINATION too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.destaddr, pTemp, tempLen);
    tMsgTemp.destlen = tempLen;

    /* TAG */
    pTemp += tempLen + 1;
#if IS_LITTLE_ENDIAN
    tMsgTemp.IID = *((uint16*)(pTemp+2));
    tMsgTemp.IOP = *((uint16*)pTemp);
#else
    tMsgTemp.IID = ((uint16)*pTemp)*256 + *(pTemp+1);
    tMsgTemp.IOP = ((uint16)*(pTemp+2))*256 + *(pTemp+3);
#endif

    /* MsgLen */
    pTemp += 4;
    tMsgTemp.msglen = appmsg_get_len_offset(pTemp, &offset);
    pTemp += offset;
    
    /* 拷贝 */
    ptNewMsg = (MESSAGE_INFO_T *)malloc(sizeof(MESSAGE_INFO_T) + tMsgTemp.msglen);
    if(NULL == ptNewMsg)
    {
        DPRINT("appmsg_udp_split malloc failed,size %d\n", (uint32)sizeof(MESSAGE_INFO_T) + tMsgTemp.msglen);
        return -1;
    }
    
    memcpy(ptNewMsg, &tMsgTemp, sizeof(MESSAGE_INFO_T));
    if(tMsgTemp.msglen > 0)
    {
        memcpy(ptNewMsg->playload, pTemp, tMsgTemp.msglen);
    }
    *ppMsgOut = ptNewMsg;

    return 0;
}

/**********************************************************************
* @name      : appmsg_package
* @brief     ：指定格式消息组装成
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  结构消息
* @param[out]：uint8 **ppBuf             组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2020-1-3
* @Update    :
**********************************************************************/
uint16 appmsg_package(MESSAGE_INFO_T *pMsgInfo, uint8 **ppBuf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = NULL;
    uint8    *pTemp = NULL;
    
    bufLen = 1 + 2 + 2 + pMsgInfo->sourlen + 1 + pMsgInfo->destlen + 1 + 4;   //计算head和Tag长度

    /* 计算MSG's-Length 长度  */
    offset = appmsg_set_len_offset(pMsgInfo->msglen, lenBuf);
    if(0 == offset)
    {
        DPRINT("appmsg_udp_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->msglen;

    pMsgBuf = (uint8 *)malloc(bufLen);
    if(NULL == pMsgBuf)
    {
        DPRINT("appmsg_udp_package malloc failed,size %p\n", pMsgBuf);
        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->priority << 1) & 0xfe) + (pMsgInfo->rpm & 0x01);
    pTemp++;

    /* INDEX */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->index;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->index);
#endif
    pTemp += sizeof(uint16);

    /* LABEL */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->label;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->label);
#endif
    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->souraddr, pMsgInfo->sourlen);
    pTemp += pMsgInfo->sourlen + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->destaddr, pMsgInfo->destlen);
    pTemp += pMsgInfo->destlen + 1;

    /* MSG's-TAG */
#if IS_LITTLE_ENDIAN
    *((uint32 *)pTemp) =((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff);
#else
    *((uint32 *)pTemp) = htonl(((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff));
#endif

    pTemp += sizeof(uint32);

    /* MSG's-Length */
    memcpy(pTemp, lenBuf, offset);
    pTemp += offset;

    if(pMsgInfo->msglen)
    {
        memcpy(pTemp, pMsgInfo->playload, pMsgInfo->msglen);  //MSG's-Payload
    }

    *ppBuf = pMsgBuf;
    return bufLen;
}

/**********************************************************************
* @name      : appname_split
* @brief     ：app名称解析
* @param[in] ：uint8 *pName      APP名称
* @param[out]：APP_INFO_T *ptApp 名称解析结果
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int appname_split(uint8 *pName, APP_INFO_T *ptApp)
{
    uint32         len = 0;
    uint32        Port = 0;
    uint8  uIPAddr[16] = {0};
    char tempName[128] = {0};
    
    if(NULL == pName || NULL == ptApp)
    {
        return -1;
    }

    len = strlen((char *)pName);
    if(len > APPMSG_NAME_LEN || len <= 2)
    {
        return -1;
    }
    
    if('M' == pName[0] || 'I' == pName[0] || 'T' == pName[0])
    {
        ptApp->commType = pName[0];
        memcpy(ptApp->name, pName + 2, len - 2);
    }
    else if('U' == pName[0])
    {
        sscanf((char *)pName, "%c-%s:%s[%u]", &(ptApp->commType), tempName, uIPAddr, &Port);
        if(0 == strcmp("localhost", (char *)uIPAddr))
        {
            memset(uIPAddr, 0, 16);
            sprintf((char *)uIPAddr, "127.0.0.1");
        }
        inet_aton((char *)uIPAddr, (struct in_addr*)&ptApp->ipAddr);
        ptApp->ipAddr = ntohl(ptApp->ipAddr); /* 需要将网络序转换成主机序 */
        ptApp->port = (uint16)Port;
    }
    else
    {
        ptApp->commType = 'M';
        memcpy(ptApp->name, pName, len);
    }
    return 0;
}

/**********************************************************************
* @name      : appname_package
* @brief     ：app名称封装
* @param[in] ：APP_INFO_T *ptApp  名称解析结果
               uint16 datalen     缓存长度
* @param[out]：uint8 *pName       APP名称
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int appname_package(APP_INFO_T *ptApp, uint8 *pName, uint16 datalen)
{
    char tempName[256] = {0};
    uint8          len = 0;
    if('M' == ptApp->commType || 'I' == ptApp->commType || 'T' == ptApp->commType)
    {
        sprintf(tempName, "%c-%s", ptApp->commType, ptApp->name);
    }
    else if('U' == ptApp->commType)
    {
        if(0x7f000001 == ptApp->ipAddr)
        {
            sprintf(tempName, "%c-%s:%s[%u]", ptApp->commType, ptApp->name, "localhost", ptApp->port);
        }
        else
        {
            sprintf(tempName, "%c-%s:%u.%u.%u.%u[%u]", ptApp->commType, ptApp->name, 
                              (ptApp->ipAddr >> 24) & 0xff, (ptApp->ipAddr >> 16) & 0xff, 
                              (ptApp->ipAddr >> 8) & 0xff, ptApp->ipAddr & 0xff, ptApp->port);
        }
    }
    else
    {
        return -1;
    }
    len = strlen(tempName);
    if(len > datalen)
    {
        return -2;
    }
    memcpy(pName, tempName, len);
    return 0;
}

/**********************************************************************
* @name      : app_get_maketime
* @brief     ：解析定制的编译时间
* @param[in] ：char *ptime        例如 2020/5/22 13:19:20
* @param[out]：Date_Time_S *pTime 编译时间
* @return    ：
* @Create    : 
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
void app_get_maketime(char *ptime, Date_Time_S *pTime)
{
    uint32 a, b, c, d, e, f;
    sscanf(ptime, "%u/%u/%u %u:%u:%u", &a, &b, &c, &d, &e, &f);

    pTime->year_h = (uint8)((a & 0xff00) >> 8);
    pTime->year_l = (uint8)(a & 0xff);
    pTime->month = (uint8)b;
    pTime->day = (uint8)c;

    pTime->hour = (uint8)d;
    pTime->min = (uint8)e;
    pTime->sec = (uint8)f;
}

/**********************************************************************
* @name      : app_get_publish_time
* @brief     ：解析APP编译时间
* @param[in] ：char *pdate        __DATE__
               char *ptime        __TIME__
* @param[out]：Date_Time_S *pTime 编译时间
* @return    ：
* @Create    : 
* @Date      ：2020-1-9
* @Update    :
**********************************************************************/
void app_get_publish_time(char *pdate, char *ptime, Date_Time_S *pTime)
{
    char Mmm[4] = "Jan";
    uint32 a,b,c;
    sscanf(pdate, "%3s %u %u", Mmm, &a, &b);
    pTime->day = (uint8)a;
    pTime->year_h = (uint8)((b & 0xff00) >> 8);
    pTime->year_l = (uint8)b;

    if(0 == memcmp("Jan", Mmm, 3))
    {
        pTime->month = 1;
    }
    else if(0 == memcmp("Feb", Mmm, 3))
    {
        pTime->month = 2;
    }
    else if(0 == memcmp("Mar", Mmm, 3))
    {
        pTime->month = 3;
    }
    else if(0 == memcmp("Apr", Mmm, 3))
    {
        pTime->month = 4;
    }
    else if(0 == memcmp("May", Mmm, 3))
    {
        pTime->month = 5;
    }
    else if(0 == memcmp("Jun", Mmm, 3))
    {
        pTime->month = 6;
    }
    else if(0 == memcmp("Jul", Mmm, 3))
    {
        pTime->month = 7;
    }
    else if(0 == memcmp("Aug", Mmm, 3))
    {
        pTime->month = 8;
    }
    else if(0 == memcmp("Sep", Mmm, 3))
    {
        pTime->month = 9;
    }
    else if(0 == memcmp("Oct", Mmm, 3))
    {
        pTime->month = 10;
    }
    else if(0 == memcmp("Nov", Mmm, 3))
    {
        pTime->month = 11;
    }
    else if(0 == memcmp("Dec", Mmm, 3))
    {
        pTime->month = 12;
    }
    else
    {
        pTime->month = 0;
    }

    sscanf(ptime, "%u:%u:%u", &a, &b, &c);
    pTime->hour = (uint8)a;
    pTime->min = (uint8)b;
    pTime->sec = (uint8)c;
}

/**********************************************************************
* @name      : appmsg_modinfo_split
* @brief     ：解析模组管理器 模组信息
* @param[in] ：uint8 *playload          消息
* @param[out]：MOD_INFO_ACK_T *pOutInfo 模组信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int appmsg_modinfo_split(uint8 *playload, MOD_INFO_ACK_T *pOutInfo)
{
    uint8   bytes = 0;
    uint8  *ptemp = playload;
    uint8   i;
    if(NULL == playload || NULL == pOutInfo)
    {
        return -1;
    }

    memset(pOutInfo, 0, sizeof(MOD_INFO_ACK_T));
    pOutInfo->status = *ptemp;
    if(0 != pOutInfo->status)
    {
        return 0;
    }
    /* 模块设备型号 */
    ptemp++;
    pOutInfo->type.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->type.value, ptemp, MIN(64, pOutInfo->type.num));
    ptemp += pOutInfo->type.num;

    /* 模块设备ID */
    pOutInfo->Id.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->Id.value, ptemp, MIN(64, pOutInfo->Id.num));
    ptemp += pOutInfo->Id.num;

    /* 软件版本 */
    appmsg_memcpy_r(&(pOutInfo->softVersion), ptemp, sizeof(int));
    ptemp += sizeof(int);

    /* 软件发布日期 */
    pOutInfo->softDate.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->softDate.value, ptemp, MIN(64, pOutInfo->softDate.num));
    ptemp += pOutInfo->softDate.num;
    
    /* 硬件版本 */
    appmsg_memcpy_r(&(pOutInfo->hwVersion), ptemp, sizeof(int));
    ptemp += sizeof(int);

    /* 硬件发布日期 */
    pOutInfo->hwDate.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->hwDate.value, ptemp, MIN(64, pOutInfo->hwDate.num));
    ptemp += pOutInfo->hwDate.num;

    /* 厂商编码 */
    pOutInfo->manuCode.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->manuCode.value, ptemp, MIN(64, pOutInfo->manuCode.num));
    ptemp += pOutInfo->manuCode.num;

    /* 通道配置 */
    pOutInfo->num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    for(i = 0; i < MIN(pOutInfo->num, MOD_CHANNEL_MAX); i++)
    {
        pOutInfo->channel[i].type = *ptemp;
        ptemp++;
        pOutInfo->channel[i].function = *ptemp;
        ptemp++;
    }

    return 0;
}

/**********************************************************************
* @name      : appmsg_submodinfo_split
* @brief     ：解析模组管理器 模组信息
* @param[in] ：uint8 *playload          消息
* @param[out]：MOD_INFO_ACK_T *pOutInfo 模组信息
* @return    ：下一个偏移指针
* @Create    : 
* @Date      ：2021-4-7
* @Update    :
**********************************************************************/
uint8 *appmsg_submodinfo_split(uint8 *playload, MOD_INFO_ACK_T *pOutInfo)
{
    uint8   bytes = 0;
    uint8  *ptemp = playload;
    uint8   i;
    
    if(NULL == playload || NULL == pOutInfo)
    {
        return ptemp;
    }

    memset(pOutInfo, 0, sizeof(MOD_INFO_ACK_T));
    pOutInfo->subport = *ptemp;
    ptemp++;
    pOutInfo->status = *ptemp;
    ptemp++;
    if(0 != pOutInfo->status)
    {
        return ptemp;
    }
    
    /* 模块设备型号 */
    pOutInfo->type.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->type.value, ptemp, MIN(64, pOutInfo->type.num));
    ptemp += pOutInfo->type.num;

    /* 模块设备ID */
    pOutInfo->Id.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->Id.value, ptemp, MIN(64, pOutInfo->Id.num));
    ptemp += pOutInfo->Id.num;

    /* 软件版本 */
    appmsg_memcpy_r(&(pOutInfo->softVersion), ptemp, sizeof(int));
    ptemp += sizeof(int);

    /* 软件发布日期 */
    pOutInfo->softDate.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->softDate.value, ptemp, MIN(64, pOutInfo->softDate.num));
    ptemp += pOutInfo->softDate.num;
    
    /* 硬件版本 */
    appmsg_memcpy_r(&(pOutInfo->hwVersion), ptemp, sizeof(int));
    ptemp += sizeof(int);

    /* 硬件发布日期 */
    pOutInfo->hwDate.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->hwDate.value, ptemp, MIN(64, pOutInfo->hwDate.num));
    ptemp += pOutInfo->hwDate.num;

    /* 厂商编码 */
    pOutInfo->manuCode.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->manuCode.value, ptemp, MIN(64, pOutInfo->manuCode.num));
    ptemp += pOutInfo->manuCode.num;

    /* 通道配置 */
    pOutInfo->num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    for(i = 0; i < MIN(pOutInfo->num, MOD_CHANNEL_MAX); i++)
    {
        pOutInfo->channel[i].type = *ptemp;
        ptemp++;
        pOutInfo->channel[i].function = *ptemp;
        ptemp++;
    }

    return ptemp;
}

/**********************************************************************
* @name      : appmsg_modsinfo_split
* @brief     ：解析模组管理器 模组信息 支持子模组扩展
* @param[in] ：uint8 *playload           消息
* @param[out]：MODS_INFO_ACK_T *pOutInfo 模组信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-4-7
* @Update    :
**********************************************************************/
int appmsg_modsinfo_split(uint8 *playload, MODS_INFO_ACK_T *pOutInfo)
{
    uint8  *ptemp = playload;
    uint8   i;
    
    if(NULL == playload || NULL == pOutInfo)
    {
        return -1;
    }

    memset(pOutInfo, 0, sizeof(MODS_INFO_ACK_T));
    if(*ptemp < 3)
    {
        pOutInfo->num = 1;
        return appmsg_modinfo_split(playload, &(pOutInfo->info[0]));
    }
    else if(3 == *ptemp)
    {
        ptemp++;
        pOutInfo->bSubMod = TRUE;
        pOutInfo->num = MIN(*ptemp, SUB_MOD_MAX);
        ptemp++;
        for(i = 0; i < pOutInfo->num; i++)
        {
            ptemp = appmsg_submodinfo_split(ptemp, &(pOutInfo->info[i]));
        }
        return 0;
    }

    return 0;
}

/**********************************************************************
* @name      : appmsg_submodinfo_get
* @brief     ：解析模组管理器 子模组信息
* @param[in] ：uint8 *playload          消息
* @param[out]：MOD_INFO_NEW_T *pOutInfo 模组信息
* @return    ：下一个偏移指针
* @Create    : 
* @Date      ：2021-6-1
* @Update    :
**********************************************************************/
uint8 *appmsg_submodinfo_get(uint8 *playload, MOD_INFO_NEW_T *pOutInfo)
{
    uint8   bytes = 0;
    uint8  *ptemp = playload;
    uint8   i;
    
    if(NULL == playload || NULL == pOutInfo)
    {
        return ptemp;
    }
    
    /* 模块设备型号 */
    pOutInfo->type.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->type.value, ptemp, MIN(64, pOutInfo->type.num));
    ptemp += pOutInfo->type.num;

    /* 模块设备ID */
    pOutInfo->Id.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->Id.value, ptemp, MIN(64, pOutInfo->Id.num));
    ptemp += pOutInfo->Id.num;

    /* 软件版本 */
    pOutInfo->softVersion.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->softVersion.value, ptemp, MIN(64, pOutInfo->softVersion.num));
    ptemp += pOutInfo->softVersion.num;

    /* 软件发布日期 */
    pOutInfo->softDate.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->softDate.value, ptemp, MIN(64, pOutInfo->softDate.num));
    ptemp += pOutInfo->softDate.num;
    
    /* 硬件版本 */
    pOutInfo->hwVersion.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->hwVersion.value, ptemp, MIN(64, pOutInfo->hwVersion.num));
    ptemp += pOutInfo->hwVersion.num;

    /* 硬件发布日期 */
    pOutInfo->hwDate.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->hwDate.value, ptemp, MIN(64, pOutInfo->hwDate.num));
    ptemp += pOutInfo->hwDate.num;

    /* 厂商编码 */
    pOutInfo->manuCode.num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    memcpy(pOutInfo->manuCode.value, ptemp, MIN(64, pOutInfo->manuCode.num));
    ptemp += pOutInfo->manuCode.num;

    /* 通道配置 */
    pOutInfo->num = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    for(i = 0; i < MIN(pOutInfo->num, MOD_CHANNEL_MAX); i++)
    {
        pOutInfo->channel[i].type = *ptemp;
        ptemp++;
        pOutInfo->channel[i].function = *ptemp;
        ptemp++;

        pOutInfo->channel[i].dev.num = appmsg_get_len_offset(ptemp, &bytes);
        ptemp += bytes;
        memcpy(pOutInfo->channel[i].dev.value, ptemp, MIN(64, pOutInfo->channel[i].dev.num));
        ptemp += pOutInfo->channel[i].dev.num;
    }

    return ptemp;
}

/**********************************************************************
* @name      : appmsg_modsinfo_split_new
* @brief     ：解析模组管理器 模组信息(新版本) 支持子模组扩展
* @param[in] ：uint8 *playload           消息
* @param[out]：MODS_INFO_NEW_T *pOutInfo 模组信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-6-1
* @Update    :
**********************************************************************/
int appmsg_modsinfo_split_new(uint8 *playload, MODS_INFO_NEW_T *pOutInfo)
{
    uint8  *ptemp = playload;
    uint8   i;
    
    if(NULL == playload || NULL == pOutInfo)
    {
        return -1;
    }

    memset(pOutInfo, 0, sizeof(MODS_INFO_NEW_T));
    if(0 == *ptemp)
    {
        pOutInfo->num = 1;
        ptemp++;
        ptemp = appmsg_submodinfo_get(ptemp, &(pOutInfo->info[0]));
        return 0;
    }
    else if(3 == *ptemp)
    {
        ptemp++;
        pOutInfo->bSubMod = TRUE;
        pOutInfo->num = MIN(*ptemp, SUB_MOD_MAX);
        ptemp++;
        for(i = 0; i < pOutInfo->num; i++)
        {
            pOutInfo->info[i].subport = *ptemp;
            ptemp++;
            pOutInfo->info[i].status = *ptemp;
            ptemp++;
            if(0 == pOutInfo->info[i].status)
            {
                ptemp = appmsg_submodinfo_get(ptemp, &(pOutInfo->info[i]));
            }
        }
        return 0;
    }
    else
    {
        pOutInfo->num = 1;
        pOutInfo->info[0].status = *ptemp;
    }

    return 0;
}

/**********************************************************************
* @name      : appmsg_wireapn_split
* @brief     ：解析无线拨号 设置APN
* @param[in] ：uint8 *playload            消息
               uint16 len                长度
               uint8 bNew                新版本
* @param[out]：WIRE_DIAG_APN_T *pOutInfo APN配置信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-16
* @Update    : 2021-5-31
**********************************************************************/
int appmsg_wireapn_split(uint8 *playload, uint16 len, WIRE_DIAG_APN_T *pOutInfo, uint8 bNew)
{
    uint8   i;
    uint16  offset = 0;
    uint8  *ptemp = NULL;
    uint8   bytes = 0;
    
    if(NULL == playload || NULL == pOutInfo)
    {
        return 1;
    }
    ptemp = playload;

    pOutInfo->id = ptemp[offset++];
    //pOutInfo->stack = 0;
    
    pOutInfo->diagnum = ptemp[offset++];
    if(pOutInfo->diagnum > WIRELESS_DIAGAPN_MAX)
    {
        return 3;
    }
    for(i = 0; i < pOutInfo->diagnum; i++)
    {
        pOutInfo->tDiagApn[i].operat = ptemp[offset++];
        
        pOutInfo->tDiagApn[i].apn.nNum = appmsg_get_len_offset(ptemp + offset, &bytes);
        if(pOutInfo->tDiagApn[i].apn.nNum > 64)
        {
            return 1;
        }
        offset += bytes;
        memcpy(pOutInfo->tDiagApn[i].apn.value, ptemp + offset, pOutInfo->tDiagApn[i].apn.nNum);
        offset += pOutInfo->tDiagApn[i].apn.nNum;

        pOutInfo->tDiagApn[i].username.nNum = appmsg_get_len_offset(ptemp + offset, &bytes);
        if(pOutInfo->tDiagApn[i].username.nNum > 64)
        {
            return 1;
        }
        offset += bytes;
        memcpy(pOutInfo->tDiagApn[i].username.value, ptemp + offset, pOutInfo->tDiagApn[i].username.nNum);
        offset += pOutInfo->tDiagApn[i].username.nNum;
        
        pOutInfo->tDiagApn[i].pwd.nNum = appmsg_get_len_offset(ptemp + offset, &bytes);
        if(pOutInfo->tDiagApn[i].pwd.nNum > 64)
        {
            return 1;
        }
        offset += bytes;
        memcpy(pOutInfo->tDiagApn[i].pwd.value, ptemp + offset, pOutInfo->tDiagApn[i].pwd.nNum);
        offset += pOutInfo->tDiagApn[i].pwd.nNum;
    }

    if(bNew)
    {
        pOutInfo->network = ptemp[offset++];
        if(pOutInfo->network > 4)
        {
            return 4;
        }
        pOutInfo->authtype = ptemp[offset++];
        if(pOutInfo->authtype > 3 && pOutInfo->authtype < 255)
        {
            return 5;
        }
    }

    if(offset != len)
    {
        return 1;
    }
    return 0;
}

/**********************************************************************
* @name      : appmsg_wireapn_package
* @brief     ：组装无线拨号消息
* @param[in] ：WIRE_DIAG_APN_T *pDiagAPN  APN配置信息
               uint16 buflen              缓存长度
* @param[out]：uint8 *pBuf                消息体缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2020-1-16
* @Update    :
**********************************************************************/
uint16 appmsg_wireapn_package(WIRE_DIAG_APN_T *pDiagAPN, uint8 *pBuf, uint16 buflen)
{
    uint8  i;
    uint16 offset = 0;
    uint8  temp[2048] = {0};   //长度设置2048足够
    uint8  bytes = 0;
    uint8  lenbuf[3] = {0};

    if(NULL == pDiagAPN || NULL == pBuf || buflen < 3)
    {
        return 0;
    }

    temp[offset++] = pDiagAPN->id;
    //temp[offset++] = pDiagAPN->stack;
    temp[offset++] = pDiagAPN->diagnum;
    
    for(i = 0; i < pDiagAPN->diagnum; i++)
    {
        temp[offset++] = pDiagAPN->tDiagApn[i].operat;
        
        bytes = appmsg_set_len_offset(pDiagAPN->tDiagApn[i].apn.nNum, lenbuf);
        memcpy(temp + offset, lenbuf, bytes);
        offset += bytes;
        memcpy(temp + offset, pDiagAPN->tDiagApn[i].apn.value, pDiagAPN->tDiagApn[i].apn.nNum);
        offset += pDiagAPN->tDiagApn[i].apn.nNum;

        bytes = appmsg_set_len_offset(pDiagAPN->tDiagApn[i].username.nNum, lenbuf);
        memcpy(temp + offset, lenbuf, bytes);
        offset += bytes;
        memcpy(temp + offset, pDiagAPN->tDiagApn[i].username.value, pDiagAPN->tDiagApn[i].username.nNum);
        offset += pDiagAPN->tDiagApn[i].username.nNum;

        bytes = appmsg_set_len_offset(pDiagAPN->tDiagApn[i].pwd.nNum, lenbuf);
        memcpy(temp + offset, lenbuf, bytes);
        offset += bytes;
        memcpy(temp + offset, pDiagAPN->tDiagApn[i].pwd.value, pDiagAPN->tDiagApn[i].pwd.nNum);
        offset += pDiagAPN->tDiagApn[i].pwd.nNum;
    }

    temp[offset++] = pDiagAPN->network;
    temp[offset++] = pDiagAPN->authtype;
    if(buflen < offset)
    {
        return 0;
    }
    memcpy(pBuf, temp, offset);
    return offset;
}

/**********************************************************************
* @name      : appmsg_wireinfo_split
* @brief     ：解析无线拨号 查询模块信息
* @param[in] ：uint8 *playload           消息
* @param[out]：WIRELESS_INFO_T *pOutInfo 模块信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-19
* @Update    :
**********************************************************************/
int appmsg_wireinfo_split(uint8 *playload, WIRELESS_INFO_T *pOutInfo)
{
    uint8 *pTemp = playload;
    uint16 offset = 0;
    uint8  bytes = 0;
    uint16 len = 0;
    
    if(NULL == playload || NULL == pOutInfo)
    {
        return -1;
    }

    pOutInfo->instatus = pTemp[offset++];
    
    memcpy(&(pOutInfo->version), pTemp + offset, sizeof(COMM_MYGMR_T));
    offset += sizeof(COMM_MYGMR_T);

    len = appmsg_get_len_offset(pTemp + offset, &bytes);
    pOutInfo->typeinfo.num = len;
    offset += bytes;
    memcpy(pOutInfo->typeinfo.value, pTemp + offset, len);
    offset += len;

    len = appmsg_get_len_offset(pTemp + offset, &bytes);
    pOutInfo->IMEI.num = len;
    offset += bytes;
    memcpy(pOutInfo->IMEI.value, pTemp + offset, len);
    offset += len;

    return 0;
}

/**********************************************************************
* @name      : appmsg_wiretime_split
* @brief     ：解析无线拨号 拔号成功信息
* @param[in] ：uint8 *playload           消息
* @param[out]：WIRELESS_INFO_T *pOutInfo 模块信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-19
* @Update    :
**********************************************************************/
int appmsg_wiretime_split(uint8 *playload, WIRELESS_TIME_T *pOutInfo)
{
    uint8 *pTemp = playload;
    
    if(NULL == playload || NULL == pOutInfo)
    {
        return -1;
    }
    appmsg_memcpy_r(&pOutInfo->ConnectNum, pTemp,  2);
    appmsg_memcpy_r(&pOutInfo->FailedNum, pTemp + 2, 2);
    appmsg_memcpy_r(&pOutInfo->SuccessTimes, pTemp + 4,  4);
    return 0;
}

/**********************************************************************
* @name      : appmsg_wireinfo_package
* @brief     ：组装无线拨号消息 查询模块信息
* @param[in] ：WIRELESS_INFO_T *pInfo  模块信息
               uint16 buflen           缓存长度
* @param[out]：uint8 *pBuf             消息体缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2020-1-19
* @Update    :
**********************************************************************/
uint16 appmsg_wireinfo_package(WIRELESS_INFO_T *pInfo, uint8 *pBuf, uint16 buflen)
{
    uint8  playLoad[1024] = {0};
    uint8 *pTemp = playLoad;
    uint16 offset = 0;
    uint8  bytes = 0;
    uint8  lenbuf[3] = {0};

    if(NULL == pInfo || NULL == pBuf || 0 == buflen)
    {
        return 0;
    }
    
    pTemp[offset++] = pInfo->instatus;
    memcpy(pTemp + offset, &(pInfo->version), sizeof(COMM_MYGMR_T));
    offset += sizeof(COMM_MYGMR_T);

    bytes = appmsg_set_len_offset(pInfo->typeinfo.num, lenbuf);
    memcpy(pTemp + offset, lenbuf, bytes);
    offset += bytes;
    memcpy(pTemp + offset, pInfo->typeinfo.value, pInfo->typeinfo.num);
    offset += pInfo->typeinfo.num;

    bytes = appmsg_set_len_offset(pInfo->IMEI.num, lenbuf);
    memcpy(pTemp + offset, lenbuf, bytes);
    offset += bytes;
    memcpy(pTemp + offset, pInfo->IMEI.value, pInfo->IMEI.num);
    offset += pInfo->IMEI.num;

    if(buflen < offset)
    {
        return 0;
    }
    memcpy(pBuf, playLoad, offset);
    return offset;
}

/**********************************************************************
* @name      : appmsg_wirediag_split
* @brief     ：解析无线拨号 拨号状态
* @param[in] ：uint8 *playload             消息
* @param[out]：WIRE_DIAG_INFO_T *pDiagInfo 拨号状态
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-19
* @Update    :
**********************************************************************/
int appmsg_wirediag_split(uint8 *playload, WIRE_DIAG_INFO_T *pDiagInfo)
{
    uint8 *pTemp = playload;
    uint16 offset = 0;
    uint8  bytes = 0;
    uint16 len = 0;
    
    if(NULL == playload || NULL == pDiagInfo)
    {
        return -1;
    }

    pDiagInfo->simstatus = pTemp[offset++];
    pDiagInfo->diagstatus = pTemp[offset++];

    len = appmsg_get_len_offset(pTemp + offset, &bytes);
    pDiagInfo->CCID.num = len;
    offset += bytes;
    if(len > 0)
    {
        memcpy(pDiagInfo->CCID.value, pTemp + offset, len);
        offset += len;
    }

    len = appmsg_get_len_offset(pTemp + offset, &bytes);
    pDiagInfo->IMSI.num = len;
    offset += bytes;
    if(len > 0)
    {
        memcpy(pDiagInfo->IMSI.value, pTemp + offset, len);
        offset += len;
    }

    len = appmsg_get_len_offset(pTemp + offset, &bytes);
    pDiagInfo->phonenum.num = len;
    offset += bytes;
    if(len > 0)
    {
        memcpy(pDiagInfo->phonenum.value, pTemp + offset, len);
        offset += len;
    }
    pDiagInfo->csqvalue = pTemp[offset++];
    pDiagInfo->operat = pTemp[offset++];
    pDiagInfo->networkType = pTemp[offset++];

    memcpy(pDiagInfo->IP, pTemp + offset, 4);
    offset += 4;
    memcpy(pDiagInfo->MASK, pTemp + offset, 4);
    offset += 4;
    memcpy(pDiagInfo->GETWAY, pTemp + offset, 4);
    offset += 4;

    len = appmsg_get_len_offset(pTemp + offset, &bytes);
    pDiagInfo->APN.nNum = len;
    offset += bytes;
    memcpy(pDiagInfo->APN.value, pTemp + offset, len);
    offset += len;

    return 0;
}

/**********************************************************************
* @name      : appmsg_wirediag_package
* @brief     ：组装无线拨号消息 拨号状态
* @param[in] ：WIRE_DIAG_INFO_T *pDiagInfo  拨号状态
               uint16 buflen                缓存长度
* @param[out]：uint8 *pBuf                  消息体缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2020-1-19
* @Update    :
**********************************************************************/
uint16 appmsg_wirediag_package(WIRE_DIAG_INFO_T *pDiagInfo, uint8 *pBuf, uint16 buflen)
{
    uint8  playLoad[1024] = {0};
    uint8 *pTemp = playLoad;
    uint16 offset = 0;
    uint8  bytes = 0;
    uint8  lenbuf[3] = {0};

    if(NULL == pDiagInfo || NULL == pBuf || 0 == buflen)
    {
        return 0;
    }
    
    pTemp[offset++] = pDiagInfo->simstatus;
    pTemp[offset++] = pDiagInfo->diagstatus;

    bytes = appmsg_set_len_offset(pDiagInfo->CCID.num, lenbuf);
    memcpy(pTemp + offset, lenbuf, bytes);
    offset += bytes;
    memcpy(pTemp + offset, pDiagInfo->CCID.value, pDiagInfo->CCID.num);
    offset += pDiagInfo->CCID.num;

    bytes = appmsg_set_len_offset(pDiagInfo->IMSI.num, lenbuf);
    memcpy(pTemp + offset, lenbuf, bytes);
    offset += bytes;
    memcpy(pTemp + offset, pDiagInfo->IMSI.value, pDiagInfo->IMSI.num);
    offset += pDiagInfo->IMSI.num;

    bytes = appmsg_set_len_offset(pDiagInfo->phonenum.num, lenbuf);
    memcpy(pTemp + offset, lenbuf, bytes);
    offset += bytes;
    if(pDiagInfo->phonenum.num > 0)
    {
        memcpy(pTemp + offset, pDiagInfo->phonenum.value, pDiagInfo->phonenum.num);
        offset += pDiagInfo->phonenum.num;
    }
    pTemp[offset++] = pDiagInfo->csqvalue;
    pTemp[offset++] = pDiagInfo->operat;
    pTemp[offset++] = pDiagInfo->networkType;

    memcpy(pTemp + offset, pDiagInfo->IP, 4);
    offset += 4;
    memcpy(pTemp + offset, pDiagInfo->MASK, 4);
    offset += 4;
    memcpy(pTemp + offset, pDiagInfo->GETWAY, 4);
    offset += 4;

    bytes = appmsg_set_len_offset(pDiagInfo->APN.nNum, lenbuf);
    memcpy(pTemp + offset, lenbuf, bytes);
    offset += bytes;
    memcpy(pTemp + offset, pDiagInfo->APN.value, pDiagInfo->APN.nNum);
    offset += pDiagInfo->APN.nNum;

    //pTemp[offset++] = pDiagInfo->diagtype;

    if(buflen < offset)
    {
        return 0;
    }
    memcpy(pBuf, playLoad, offset);
    return offset;
}

/**********************************************************************
* @name      : appmsg_gps_split
* @brief     ：解析GPS
* @param[in] ：uint8 *playload  消息
* @param[out]：GPS_T *pGps      GPS信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-5-3
* @Update    :
**********************************************************************/
int appmsg_gps_split(uint8 *playload, GPS_T *pGps)
{
    uint8 *pTemp = playload;
    uint16 offset = 0;
    
    if(NULL == playload || NULL == pGps)
    {
        return -1;
    }

    appmsg_memcpy_r(&(pGps->utctime.year), pTemp, sizeof(uint16));
    offset += sizeof(uint16);
    pGps->utctime.month = pTemp[offset++];
    pGps->utctime.mday = pTemp[offset++];
    pGps->utctime.wday = pTemp[offset++];
    pGps->utctime.hour = pTemp[offset++];
    pGps->utctime.minute = pTemp[offset++];
    pGps->utctime.second = pTemp[offset++];
    appmsg_memcpy_r(&(pGps->utctime.msec), pTemp, sizeof(uint16));
    offset += sizeof(uint16);

    pGps->position.longitude.position = pTemp[offset++];
    appmsg_memcpy_r(&(pGps->position.longitude.degre), pTemp + offset, sizeof(uint16));
    offset += sizeof(uint16);
    appmsg_memcpy_r(&(pGps->position.longitude.branch), pTemp + offset, sizeof(float32));
    offset += sizeof(float32);

    pGps->position.latitude.position = pTemp[offset++];
    appmsg_memcpy_r(&(pGps->position.latitude.degre), pTemp + offset, sizeof(uint16));
    offset += sizeof(uint16);
    appmsg_memcpy_r(&(pGps->position.latitude.branch), pTemp + offset, sizeof(float32));
    offset += sizeof(float32);

    pGps->ststus = pTemp[offset++];
    
    appmsg_memcpy_r(&(pGps->position.height), pTemp + offset, sizeof(int));
    offset += sizeof(int);
    
    pGps->mode = pTemp[offset++];
    return 0;
}

/**********************************************************************
* @name      : appmsg_gpsmsg_split
* @brief     ：解析GPS消息
* @param[in] ：uint8 *playload  消息
* @param[out]：GPS_T *pGps      GPS信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-12-24
* @Update    :
**********************************************************************/
int appmsg_gpsmsg_split(uint8 *playload, GPS_MSG_T *pGps)
{
    uint8 *pTemp = playload;
    uint16 offset = 0;
    
    if(NULL == playload || NULL == pGps)
    {
        return -1;
    }

    pGps->utctime.hour = pTemp[offset++];
    pGps->utctime.minute = pTemp[offset++];
    pGps->utctime.second = pTemp[offset++];

    pGps->position.longitude.position = pTemp[offset++];
    pGps->position.longitude.degre = pTemp[offset++];
    pGps->position.longitude.branch = pTemp[offset++];
    pGps->position.longitude.second = pTemp[offset++];

    pGps->position.latitude.position = pTemp[offset++];
    pGps->position.latitude.degre = pTemp[offset++];
    pGps->position.latitude.branch = pTemp[offset++];
    pGps->position.latitude.second = pTemp[offset++];

    pGps->ststus = pTemp[offset++];
    
    appmsg_memcpy_r(&(pGps->position.height), pTemp + offset, sizeof(uint32));
    offset += sizeof(uint32);
    
    pGps->mode = pTemp[offset++];
    return 0;
}

/**********************************************************************
* @name      : appmsg_wirediag_package
* @brief     ：组装无线拨号消息 GPS位置
* @param[in] ：GPS_T *pGps   定位信息
               uint16 buflen 缓存长度
               uint8 bNew    是否新版本
* @param[out]：uint8 *pBuf   消息体缓存
* @return    ：消息长度
* @Create    : 
* @Date      ：2020-4-23
* @Update    :
**********************************************************************/
uint16 appmsg_gps_package(GPS_T *pGps, uint8 *pBuf, uint16 buflen, uint8 bNew)
{
    uint8  playLoad[512] = {0};
    uint8 *pTemp = playLoad;
    uint16 offset = 0;
    int    minute = 0;
    float  second = 0;

    if(NULL == pGps || NULL == pBuf || 0 == buflen)
    {
        return 0;
    }

    if(TRUE == bNew)
    {
        /* 新版本 */
        appmsg_memcpy_r(pTemp, &(pGps->utctime.year), sizeof(uint16));
        offset += sizeof(uint16);
        pTemp[offset++] = pGps->utctime.month;
        pTemp[offset++] = pGps->utctime.mday;
        pTemp[offset++] = pGps->utctime.wday;
        pTemp[offset++] = pGps->utctime.hour;
        pTemp[offset++] = pGps->utctime.minute;
        pTemp[offset++] = pGps->utctime.second;
        appmsg_memcpy_r(pTemp + offset, &(pGps->utctime.msec), sizeof(uint16));
        offset += sizeof(uint16);
        
        pTemp[offset++] = pGps->position.longitude.position;
        appmsg_memcpy_r(pTemp + offset, &(pGps->position.longitude.degre), sizeof(uint16));
        offset += sizeof(uint16);
        appmsg_memcpy_r(pTemp + offset, &(pGps->position.longitude.branch), sizeof(float32));
        offset += sizeof(float32);

        pTemp[offset++] = pGps->position.latitude.position;
        appmsg_memcpy_r(pTemp + offset, &(pGps->position.latitude.degre), sizeof(uint16));
        offset += sizeof(uint16);
        appmsg_memcpy_r(pTemp + offset, &(pGps->position.latitude.branch), sizeof(float32));
        offset += sizeof(float32);
        
        pTemp[offset++] = pGps->ststus;

        appmsg_memcpy_r(pTemp + offset, &(pGps->position.height), sizeof(uint32));
        offset += sizeof(uint32);

        pTemp[offset++] = pGps->mode;
    }
    else
    {
        /* 旧版本 */
        pTemp[offset++] = pGps->utctime.hour;
        pTemp[offset++] = pGps->utctime.minute;
        pTemp[offset++] = pGps->utctime.second;
        
        /* 经度 */
        pTemp[offset++] = pGps->position.longitude.position;
        pTemp[offset++] = (uint8)pGps->position.longitude.degre;
        minute = (int)pGps->position.longitude.branch;
        second = pGps->position.longitude.branch - minute;
        pTemp[offset++] = (uint8)minute;
        pTemp[offset++] = (uint8)(second * 60.0);

        /* 纬度 */
        pTemp[offset++] = pGps->position.latitude.position;
        pTemp[offset++] = (uint8)pGps->position.latitude.degre;
        minute = (int)pGps->position.latitude.branch;
        second = pGps->position.latitude.branch - minute;
        pTemp[offset++] = (uint8)minute;
        pTemp[offset++] = (uint8)(second * 60.0);

        pTemp[offset++] = 1 == pGps->ststus ? 0 : 1;

        appmsg_memcpy_r(pTemp + offset, &(pGps->position.height), sizeof(uint32));
        offset += sizeof(uint32);

        pTemp[offset++] = pGps->mode;
    }
    if(buflen < offset)
    {
        return 0;
    }
    memcpy(pBuf, playLoad, offset);
    return offset;
}

/**********************************************************************
* @name      : appmsg_rsprmt_split
* @brief     ：解析遥信状态
* @param[in] ：uint8 *playload       消息
* @param[out]：RST_RMT_T *pRmt  遥信信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
int appmsg_rsprmt_split(uint8 *playload, RST_RMT_T *pRmt)
{
    uint8 *pTemp = playload;
    uint16 offset = 0;
    uint8  num = 0;
    uint8  i = 0;

    if(NULL == playload || NULL == pRmt)
    {
        return -1;
    }

    if(0 == pTemp[offset])
    {
        pRmt->num = 0;
        pRmt->ST = 0xff;
        pRmt->CT = 0;
        return -1;
    }

    pRmt->num = pTemp[offset++];
    if(pRmt->num > 8)
    {
        memcpy(&pRmt->ST, pTemp + offset, 2);
        offset += 2;
    }
    else
    {
        pRmt->ST = pTemp[offset++];
    }

    num = pTemp[offset++];
    if(pRmt->num > 8)
    {
        memcpy(&pRmt->CT, pTemp + offset, 2);
        offset += 2;
    }
    else
    {
        pRmt->CT = pTemp[offset++];
    }

    num = pTemp[offset++];
    for(i = 0; i < num; i++)
    {
        appmsg_memcpy_r(&(pRmt->time[i].year), pTemp + offset, sizeof(uint16));
        offset += sizeof(uint16);
        pRmt->time[i].month = pTemp[offset++];
        pRmt->time[i].day = pTemp[offset++];
        pRmt->time[i].week = pTemp[offset++];
        pRmt->time[i].hour = pTemp[offset++];
        pRmt->time[i].min = pTemp[offset++];
        pRmt->time[i].sec = pTemp[offset++];
        appmsg_memcpy_r(&(pRmt->time[i].milsec), pTemp + offset, sizeof(uint16));
        offset += sizeof(uint16);

        if(0 == pRmt->time[i].year || 0xFFFF == pRmt->time[i].year)
        {
            pRmt->flag[i] = 0;
        }
        else
        {
            pRmt->flag[i] = 1;
        }
    }
    return 0;
}

/**********************************************************************
* @name      : appmsg_rsp_currmt_split
* @brief     ：解析遥信当前状态
* @param[in] ：uint8 *playload       消息
               uint8  stbuf         缓存容量
* @param[out]：uint8 *ST             遥信信息
* @return    ：数量
* @Create    : 
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
uint8 appmsg_rsp_currmt_split(uint8 bReport, uint8 *playload, uint8 *ST, RST_RMT_T *pRmt, uint8 stbuf)
{
    uint8  num = 0;
    uint8  numtime = 0;
    uint8  status = 0;
    uint8  i = 0;
    uint16 offset = 0;
    uint8  dre = 7;
    
    if(NULL == playload || NULL == ST || 0 == stbuf)
    {
        return 0;
    }

    num = playload[offset++];
    if(num > 16)
    {
        num = 16;
    }
    status = playload[offset++];

    for(i = 0; i < num && i < stbuf; i++)
    {
        if(8 == i)
        {
            dre = 15;
            status = playload[offset++];
        }
        ST[i] = ((status >> (dre - i)) & 0x01);
        pRmt->flag[i] = 1;
    }

    if(bReport)
    {
        appmsg_rsprmt_split(playload, pRmt);
    }
    else
    {
        /* 取第一个变位数据 */
        numtime = playload[offset++];
        if(numtime > 0)
        {
            appmsg_rsprmt_split(playload + offset, pRmt);
        }
    }
    return MIN(num, stbuf);
}


/**********************************************************************
* @name      : appmsg_rsppulse_split
* @brief     ：解析脉冲计数及统计时间
* @param[in] ：uint8 *playload            消息
* @param[out]：RST_PULSE_STATIC_T *pPulse 脉冲信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
int appmsg_rsppulse_split(uint8 *playload, RST_PULSE_STATIC_T *pPulse)
{
    uint8 *pTemp = playload;
    uint16 offset = 0;
    uint8  i = 0;

    if(NULL == playload || NULL == pPulse)
    {
        return -1;
    }

    pPulse->num = pTemp[offset++];
    for(i = 0; i < pPulse->num; i++)
    {
        pPulse->item[i].flag = 1;
        appmsg_memcpy_r(&(pPulse->item[i].count), pTemp + offset, sizeof(uint32));
        offset += sizeof(uint32);
        
        appmsg_memcpy_r(&(pPulse->item[i].timesec), pTemp + offset, sizeof(uint32));
        offset += sizeof(uint32);
        appmsg_memcpy_r(&(pPulse->item[i].timems), pTemp + offset, sizeof(uint16));
        offset += sizeof(uint16);
        
        appmsg_memcpy_r(&(pPulse->item[i].start.year), pTemp + offset, sizeof(uint16));
        offset += sizeof(uint16);

        pPulse->item[i].start.month = pTemp[offset++];
        pPulse->item[i].start.day= pTemp[offset++];
        pPulse->item[i].start.week = pTemp[offset++];
        pPulse->item[i].start.hour = pTemp[offset++];
        pPulse->item[i].start.min = pTemp[offset++];
        pPulse->item[i].start.sec = pTemp[offset++];
        appmsg_memcpy_r(&(pPulse->item[i].start.milsec), pTemp + offset, sizeof(uint16));
        offset += sizeof(uint16);

        if(0 == pPulse->item[i].start.year || 0xFFFF == pPulse->item[i].start.year)
        {
            pPulse->item[i].flag = 0;
        }
    }
    return 0;
}

/**********************************************************************
* @name      : appmsg_deviceinfo_split
* @brief     ：解析设备信息消息
* @param[in] ：uint8 *playload          消息
* @param[out]：OOP_DEVICERUNSTAT_T *deviceinfo 设备信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int appmsg_mac_split(uint8 *playload, OOP_ETHMAC_T *mac)
{
    uint8   bytes = 0;
    uint8  *ptemp = playload;
    uint16  len;
    if(NULL == playload || NULL == mac)
    {
        return -1;
    }

    memset(mac, 0, sizeof(OOP_ETHMAC_T));

    /* 硬件序列号 1*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 硬件序列号 2*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 硬件序列号3*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 硬件序列号 4*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 设备类型*/
    ptemp+=4;
    
    /* 设备供应商*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 硬件版本号*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 软件版本号*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 设备生产日期*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 设备描述符*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 设备MAC*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    len = MIN(6, len);
    memcpy(mac->data, ptemp, len);
    ptemp+=len;
    
    /* 设备内存容量*/
    ptemp+=sizeof(uint32);
    
    /* 设备存储容量*/
    ptemp+=sizeof(uint32);

    /* 处理器主频*/
    ptemp+=sizeof(uint32);

	/* cpu核数*/
    ptemp+=sizeof(uint8);

	/* CPU缓存*/
    ptemp+=sizeof(uint16);

	/* CPU架构*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;
    
    return 0;
}

/**********************************************************************
* @name      : appmsg_deviceinfo_split
* @brief     ：解析设备信息消息
* @param[in] ：uint8 *playload          消息
* @param[out]：OOP_DEVICERUNSTAT_T *deviceinfo 设备信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int appmsg_deviceinfo_split(uint8 *playload, OOP_DEVICERUNSTAT_T *deviceinfo)
{
    uint8   bytes = 0;
    uint8  *ptemp = playload;
    uint16  len;
    if(NULL == playload || NULL == deviceinfo)
    {
        return -1;
    }

    memset(deviceinfo, 0, sizeof(OOP_DEVICERUNSTAT_T));

    /* 硬件序列号 1*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 硬件序列号 2*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 硬件序列号3*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 硬件序列号 4*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 设备类型*/
    ptemp+=4;
    
    /* 设备供应商*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 硬件版本号*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 软件版本号*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 设备生产日期*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 设备描述符*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;

    /* 设备MAC*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
    ptemp+=len;
    
    /* 设备内存容量*/
    appmsg_memcpy_r(&(deviceinfo->phyMem),ptemp,sizeof(uint32));
    ptemp+=sizeof(uint32);
    
    /* 设备存储容量*/
    appmsg_memcpy_r(&(deviceinfo->diskSpace),ptemp,sizeof(uint32));
    ptemp+=sizeof(uint32);

    /* 处理器主频*/
    appmsg_memcpy_r(&(deviceinfo->cpuMainFrq),ptemp,sizeof(uint32));
    ptemp+=sizeof(uint32);

	/* cpu核数*/
    appmsg_memcpy_r(&(deviceinfo->cpuCoreNum),ptemp,sizeof(uint8));
    ptemp+=sizeof(uint8);

	/* CPU缓存*/
    appmsg_memcpy_r(&(deviceinfo->cpuCache),ptemp,sizeof(uint16));
    ptemp+=sizeof(uint16);

	/* CPU架构*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
	deviceinfo->cpuArchi.nNum = len;
	memcpy((deviceinfo->cpuArchi.value),ptemp,len);
    ptemp+=len;
    
    return 0;
}

/**********************************************************************
* @name      : appmsg_deviceinfo_run_split
* @brief     ：解析设备当前运行信息消息
* @param[in] ：uint8 *playload          消息
* @param[out]：OOP_DEVICERUNSTAT_T *deviceinfo 设备信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int appmsg_deviceinfo_run_split(uint8 *playload, OOP_DEVICERUNSTAT_T *deviceinfo)
{
    uint8   bytes = 0;
    uint8  *ptemp = playload;
    uint16  len;
    if(NULL == playload || NULL == deviceinfo)
    {
        return -1;
    }

	/* 虚拟内存*/
    appmsg_memcpy_r(&(deviceinfo->virMem),ptemp,sizeof(uint32));
    ptemp+=sizeof(uint32);

	/* 内存使用率*/
    appmsg_memcpy_r(&(deviceinfo->memUsage),ptemp,sizeof(uint16));
    ptemp+=sizeof(uint16);
    
    /* 硬盘使用率*/
    appmsg_memcpy_r(&(deviceinfo->diskUsage),ptemp,sizeof(uint16));
    ptemp+=sizeof(uint16);

    /* CPU 使用率*/
    appmsg_memcpy_r(&(deviceinfo->cpuUsage),ptemp,sizeof(uint16));
    ptemp+=sizeof(uint16);

	/* 操作系统名称*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
	deviceinfo->osName.nNum = len;
	memcpy((deviceinfo->osName.value),ptemp,len);
    ptemp+=len;

	/* 操作系统版本*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
	deviceinfo->osVer.nNum = len;
	memcpy((deviceinfo->osVer.value),ptemp,len);
    ptemp+=len;

	/* 操作系统内核*/
    len = appmsg_get_len_offset(ptemp, &bytes);
    ptemp += bytes;
	deviceinfo->osKernel.nNum = len;
	memcpy((deviceinfo->osKernel.value),ptemp,len);
    ptemp+=len;
	
	/* 系统启动时间*/
	//memcpy(&(deviceinfo->sysStrtm),ptemp,sizeof(deviceinfo->sysStrtm));
    appmsg_memcpy_r(&deviceinfo->sysStrtm.year,ptemp,2);
    ptemp+=2;
    deviceinfo->sysStrtm.month = *ptemp;
    ptemp++;
    deviceinfo->sysStrtm.day = *ptemp;
    ptemp++;
    deviceinfo->sysStrtm.hour = *ptemp;
    ptemp++;
    deviceinfo->sysStrtm.minute = *ptemp;
    ptemp++;
    deviceinfo->sysStrtm.second = *ptemp;
    ptemp++;

    
    return 0;
}


/**********************************************************************
* @name      : appmsg_deviceinfo_split
* @brief     ：解析设备信息消息
* @param[in] ：uint8 *playload          消息
* @param[out]：OOP_DEVICERUNSTAT_T *deviceinfo 设备信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int appmsg_radioparam_split(uint8 *playload, RADIO_PARAM_T *radioparam)
{
    uint8  *ptemp = playload;

    if(NULL == playload || NULL == radioparam)
    {
        return -1;
    }

    memset(radioparam, 0, sizeof(RADIO_PARAM_T));


    
    /* 频道*/
    appmsg_memcpy_r(&(radioparam->channel),ptemp,sizeof(uint8));
    ptemp+=sizeof(uint8);
    
    /* 功率*/
    appmsg_memcpy_r(&(radioparam->power),ptemp,sizeof(uint8));
    ptemp+=sizeof(uint8);

    /* 静噪级别*/
    appmsg_memcpy_r(&(radioparam->SquelchLecel),ptemp,sizeof(uint8));
    ptemp+=sizeof(uint8);
    
    return 0;
}


/**********************************************************************
* @name      : appmsg_radiostatus_split
* @brief     ：解析电台状态消息
* @param[in] ：uint8 *playload          消息
* @param[out]：OOP_DEVICERUNSTAT_T *deviceinfo 设备信息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-14
* @Update    :
**********************************************************************/
int appmsg_radiostatus_split(uint8 *playload, RADIO_STATUS_T *radiostatus)
{
    uint8  *ptemp = playload;

    if(NULL == playload || NULL == radiostatus)
    {
        return -1;
    }

    memset(radiostatus, 0, sizeof(RADIO_STATUS_T));
    
    /*网络状态*/
    appmsg_memcpy_r(&(radiostatus->network),ptemp,sizeof(uint8));
    ptemp+=sizeof(uint8);
    
    /*场强*/
    appmsg_memcpy_r(&(radiostatus->field),ptemp,sizeof(uint16));
    ptemp+=sizeof(uint16);

    /*扬声器状态*/
    appmsg_memcpy_r(&(radiostatus->speaker),ptemp,sizeof(uint8));
    ptemp+=sizeof(uint8);
    
    return 0;
}

