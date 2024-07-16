/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：mqtt消息相关访问接口
* @date： 2019.10.23
*********************************************************************
*/
#include "msgMqtt.h"
#define IS_LITTLE_ENDIAN    1       //组消息的时候index label msgtag是否用小端模式 0为大端 1为小端
/**
*********************************************************************
* @brief：     打印信息
* @param[in]： charInfo- 提示信息
               buf     - 内容
               len     - 长度
* @param[out]：          
* @return：    NULL
*********************************************************************
*/
void msg_mqtt_print_buf(char *charInfo, uint8 *buf, int32 len)
{
    uint16 i = 0;
    #if MSG_MQTT_BUF_LOG == 1
    char filename[100] = {0};
    #endif


    #if MSG_MQTT_BUF_LOG == 1
    sprintf(&filename[0], "/mnt/log/msgUMqtt.log");
    uint8 logbuf[8192] = {0};
    #endif

    MPRINT("%s\n", charInfo);

    if(len <= 0)
    {   
        MPRINT("len <= 0 \n");
        #if MSG_MQTT_BUF_LOG == 1
        writelog(filename, "%s %s\n", charInfo, "len <= 0");
        #endif
        return;
    }

    MPRINT("len = %d ", len);

    for(i = 0; i < len; i++)
        MPRINT("%02x ", buf[i]);
    MPRINT("\n");

    #if MSG_MQTT_BUF_LOG == 1
    if ((3*len + 1) > 8192)
    {
        MPRINT("db_print_buf len is too long\n");
        writelog(filename, "%s %s\n", charInfo, "db_print_buf len is too long\n");
        return;
    }
    memset(&logbuf[0], '\0', 3*len);
    for (i = 0; i < len; i++)
    {
        sprintf(&logbuf[3*i], "%02X ", buf[i]);
    }
    logbuf[3*len] = 0x00;//补上'\0'
    writelog(filename, "%s len = %d %s\n", charInfo, len, logbuf);
    #endif
}

/**********************************************************************
* @name      : get_len_offset
* @brief     ：获取串类型数据元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：uint8 *bytes             便宜量
* @return    ：长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint16 get_len_offset(uint8 *indata,uint8 *bytes)
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
* @name      : set_len_offset
* @brief     ：设置元素长度
* @param[in] ：const uint8 *indata      数据Buffer
* @param[out]：const uint8 *indata      数据Buffer
* @return    ：长度
* @Create    : 
* @Date      ：2019-10-30
* @Update    :
**********************************************************************/
uint8 set_len_offset(uint16 eleLen, uint8 *indata)
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
* @name      : msg_mqtt_package
* @brief     ：指定格式消息组装成
* @param[in] ：MSG_INFO_T *pMsgInfo  结构消息
               bufLenMax             buf最大长度
* @param[out]：uint8 *buf            组装消息
* @return    ：消息长度
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
uint16 msg_mqtt_package(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = buf;
    uint8    *pTemp = NULL;

    if ((pMsgInfo == NULL) || (buf == NULL))
    {
        MPRINT("msg_mqtt_package param wrong\n");
        return 0;
    }
    
    bufLen = 1 + 2 + 2 + strlen(pMsgInfo->SourAddr) + 1 + strlen(pMsgInfo->DestAddr) + 1 + 4;   //计算head和Tag长度

    /* 计算MSG's-Length 长度  */
    offset = set_len_offset(pMsgInfo->MsgLen, lenBuf);
    if(0 == offset)
    {
        MPRINT("msg_mqtt_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->MsgLen;

    if (bufLen > bufLenMax)
    {
        MPRINT("msg_mqtt_package bufLenMax wrong\n");
        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->MsgPRIORITY << 1) & 0xfe) + (pMsgInfo->MsgRPM & 0x01);
    pTemp++;

    /* INDEX */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgIndex;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgIndex);
#endif
    pTemp += sizeof(uint16);

    /* LABEL */
#if IS_LITTLE_ENDIAN
    *((uint16 *)pTemp) = pMsgInfo->MsgLabel;
#else
    *((uint16 *)pTemp) = htons(pMsgInfo->MsgLabel);
#endif
    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->SourAddr, strlen(pMsgInfo->SourAddr));
    pTemp += strlen(pMsgInfo->SourAddr) + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->DestAddr, strlen(pMsgInfo->DestAddr));
    pTemp += strlen(pMsgInfo->DestAddr) + 1;

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

    memcpy(pTemp, pMsgInfo->MsgData, pMsgInfo->MsgLen);  //MSG's-Payload

    return bufLen;
}

/**********************************************************************
* @name      : msg_mqtt_split
* @brief     ：接受的消息解析成指定格式
* @param[in] ：uint8 *pBuf           接受到的报文
               uint16 bufLen         报文长度
* @param[out]：MSG_INFO_T **ppMsgOut 解析出来的消息
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2019-10-29
* @Update    :
**********************************************************************/
int msg_mqtt_split(uint8 *pBuf, uint16 bufLen,uint32 ipAddr, uint16 port, MSG_INFO_T **ppMsgOut)
{
    uint8  *pTemp = pBuf;
    uint8   offset = 0;
    uint16  tempLen = 0;
    MSG_INFO_T tMsgTemp;
    MSG_INFO_T *ptNewMsg = NULL;
    
    if(NULL == pBuf || NULL == ppMsgOut || 0 == bufLen)
    {
        return -1;
    }

    memset(&tMsgTemp, 0, sizeof(MSG_INFO_T));

    tMsgTemp.IP = ipAddr;
    tMsgTemp.port = port;
    
    /* PRM */
    tMsgTemp.MsgRPM = (*pTemp) & 0x01;

    /* PRIORITY */
    tMsgTemp.MsgPRIORITY = ((*pTemp)>>1) & 0x7f;

    /* INDEX */
    pTemp++;
#if IS_LITTLE_ENDIAN
    tMsgTemp.MsgIndex = *((uint16 *)pTemp);
#else
    tMsgTemp.MsgIndex = ntohs(*((uint16 *)pTemp));
#endif

    /* LABEL */
    pTemp += 2;
#if IS_LITTLE_ENDIAN
    tMsgTemp.MsgLabel = *((uint16 *)pTemp);
#else
    tMsgTemp.MsgLabel = ntohs(*((uint16 *)pTemp));
#endif
    /* SOURCE */
    pTemp += 2;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        MPRINT("SOURCE too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.SourAddr, pTemp, tempLen);
    tMsgTemp.SourLen = tempLen;

    /* DESTINATION */
    pTemp += tempLen + 1;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        MPRINT("DESTINATION too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.DestAddr, pTemp, tempLen);
    tMsgTemp.DestLen = tempLen;

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
    tMsgTemp.MsgLen = get_len_offset(pTemp, &offset);
    pTemp += offset;
    
    /* 拷贝 */
    ptNewMsg = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
    if(NULL == ptNewMsg)
    {
        MPRINT("malloc failed,size %d\n", (uint32)sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
        return -1;
    }
    
    memcpy(ptNewMsg, &tMsgTemp, sizeof(MSG_INFO_T));
    memcpy(ptNewMsg->MsgData, pTemp, tMsgTemp.MsgLen);

    *ppMsgOut = ptNewMsg;

    return 0;
}


uint16 msg_mqtt_package_little(MSG_INFO_T *pMsgInfo, uint32 bufLenMax, uint8 *buf)
{
    uint16   bufLen = 0;
    uint8 lenBuf[3] = {0};
    uint8    offset = 0;
    uint8  *pMsgBuf = buf;
    uint8    *pTemp = NULL;

    if ((pMsgInfo == NULL) || (buf == NULL))
    {
        MPRINT("msg_mqtt_package param wrong\n");
        return 0;
    }
    
    bufLen = 1 + 2 + 2 + strlen(pMsgInfo->SourAddr) + 1 + strlen(pMsgInfo->DestAddr) + 1 + 4;   //计算head和Tag长度

    /* 计算MSG's-Length 长度  */
    offset = set_len_offset(pMsgInfo->MsgLen, lenBuf);
    if(0 == offset)
    {
        MPRINT("msg_mqtt_package offset wrong\n");
        return 0;
    }

    bufLen += offset + pMsgInfo->MsgLen;

    if (bufLen > bufLenMax)
    {
        MPRINT("msg_mqtt_package bufLenMax wrong\n");
        return 0;
    }

    memset(pMsgBuf, 0, bufLen);

    pTemp = pMsgBuf;

    /* PRM PRIORITY */
    *pTemp = ((pMsgInfo->MsgPRIORITY << 1) & 0xfe) + (pMsgInfo->MsgRPM & 0x01);
    pTemp++;

    /* INDEX */

    *((uint16 *)pTemp) = pMsgInfo->MsgIndex;

    pTemp += sizeof(uint16);

    /* LABEL */

    *((uint16 *)pTemp) = pMsgInfo->MsgLabel;

    pTemp += sizeof(uint16);

    /* SOURCE */
    memcpy(pTemp, pMsgInfo->SourAddr, strlen(pMsgInfo->SourAddr));
    pTemp += strlen(pMsgInfo->SourAddr) + 1;

    /* DESTINATION */
    memcpy(pTemp, pMsgInfo->DestAddr, strlen(pMsgInfo->DestAddr));
    pTemp += strlen(pMsgInfo->DestAddr) + 1;

    /* MSG's-TAG */

    *((uint32 *)pTemp) =((((uint32)pMsgInfo->IID) << 16) & 0xffff0000) + (((uint32)pMsgInfo->IOP) & 0x0000ffff);

    pTemp += sizeof(uint32);

    /* MSG's-Length */
    memcpy(pTemp, lenBuf, offset);
    pTemp += offset;

    memcpy(pTemp, pMsgInfo->MsgData, pMsgInfo->MsgLen);  //MSG's-Payload

    return bufLen;
}
void commfun_printstring(uint8 *buf, int16 len)
{
    uint16 i = 0;
    if(len <= 0)
        return;
    for(i = 0; i < len; i++)
        printf("%02x ", buf[i]);
    putchar('\n');
}

int msg_mqtt_split_little(uint8 *pBuf, uint16 bufLen,uint32 ipAddr, uint16 port, MSG_INFO_T **ppMsgOut)
{
    uint8  *pTemp = pBuf;
    uint8   offset = 0;
    uint16  tempLen = 0;
    MSG_INFO_T tMsgTemp;
    MSG_INFO_T *ptNewMsg = NULL;
    //commfun_printstring(pBuf,bufLen);
    if(NULL == pBuf || NULL == ppMsgOut || 0 == bufLen)
    {
        return -1;
    }

    memset(&tMsgTemp, 0, sizeof(MSG_INFO_T));

    tMsgTemp.IP = ipAddr;
    tMsgTemp.port = port;
    
    /* PRM */
    tMsgTemp.MsgRPM = (*pTemp) & 0x01;

    /* PRIORITY */
    tMsgTemp.MsgPRIORITY = ((*pTemp)>>1) & 0x7f;

    /* INDEX */
    pTemp++;

    tMsgTemp.MsgIndex = *((uint16 *)pTemp);


    /* LABEL */
    pTemp += 2;

    tMsgTemp.MsgLabel = *((uint16 *)pTemp);

    /* SOURCE */
    pTemp += 2;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        MPRINT("SOURCE too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.SourAddr, pTemp, tempLen);
    tMsgTemp.SourLen = tempLen;

    /* DESTINATION */
    pTemp += tempLen + 1;
    tempLen = strlen((char *)pTemp);
    if(tempLen > 128)
    {
        MPRINT("DESTINATION too long %d\n", tempLen);
        return -1;
    }
    memcpy(tMsgTemp.DestAddr, pTemp, tempLen);
    tMsgTemp.DestLen = tempLen;

    /* TAG */
    pTemp += tempLen + 1;

    tMsgTemp.IID = *((uint16*)(pTemp+2));
    tMsgTemp.IOP = *((uint16*)pTemp);
    //printf("msg_mqtt_split_little iid %04x iop %04x \n",tMsgTemp.IID,tMsgTemp.IOP);

    /* MsgLen */
    pTemp += 4;
    tMsgTemp.MsgLen = get_len_offset(pTemp, &offset);
    pTemp += offset;
    
    /* 拷贝 */
    ptNewMsg = (MSG_INFO_T *)malloc(sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
    if(NULL == ptNewMsg)
    {
        MPRINT("malloc failed,size %d\n", (uint32)sizeof(MSG_INFO_T) + tMsgTemp.MsgLen);
        return -1;
    }
    
    memcpy(ptNewMsg, &tMsgTemp, sizeof(MSG_INFO_T));
    memcpy(ptNewMsg->MsgData, pTemp, tMsgTemp.MsgLen);

    *ppMsgOut = ptNewMsg;

    return 0;
}


