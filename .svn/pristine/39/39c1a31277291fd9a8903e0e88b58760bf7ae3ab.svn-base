/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙消息接口
* @date：    2021-7-9
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "message.h"
#include "radio.h"

/**********************************************************************
* @name      : radio_config_split
* @brief     ：参数内容解析
* @param[in] ：uint8 *pMsg                  消息内容
               uint16 msglen               消息长度
* @param[out]：RADIO_CONFIG_T *pConfig      参数
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int radio_config_split(uint8 *pMsg, uint16 msglen, RADIO_CONFIG_T *pConfig)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 7 || NULL == pConfig)
    {
        return -1;
    }

    /* 频道 */
    pConfig->channel = pMsg[offset++];

    /* 功率 */
    pConfig->power = pMsg[offset++];

    /* 静噪级别 */
    pConfig->noise = pMsg[offset++];

    #if 0
    /* 调整方式 */
    pConfig->mode = pMsg[offset++];

    /* 频率偏值 */
    pConfig->offVaule = pMsg[offset++];
    #endif
    
    /* 行政区码 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pConfig->areaNo.nNum = len;
    if (len > 0)
    {
        memcpy(pConfig->areaNo.value, pMsg + offset, len);
    }
    offset += len;

    /* 设备地址 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pConfig->devAddr.nNum = len;
    if (len > 0)
    {
        memcpy(pConfig->devAddr.value, pMsg + offset, len);
    }
    offset += len;

    /* 终端地址 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pConfig->addr.nNum = len;
    if (len > 0)
    {
        memcpy(pConfig->addr.value, pMsg + offset, len);
    }
    offset += len;
    
    return 0;
}

/**********************************************************************
* @name      : radio_config_package
* @brief     ：发送数据内容封装
* @param[in] ：BLE_MSG_T *ptMsg           数据内容
               uint16 buflen              缓存长度
* @param[out]：uint8 *pBuf                消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 radio_config_package(RADIO_CONFIG_T *pConfig, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;

    if(NULL == pConfig || NULL == pBuf || buflen < 13)
    {
        return 0;
    }

    #if 0
    /* 频道 */
    pBuf[offset++] = pConfig->channel;

    /* 发射功率档 */
    pBuf[offset++] = pConfig->power;
    
    /* 静噪级别 */
    pBuf[offset++] = pConfig->noise;
    
    /* 调整方式 */
    pBuf[offset++] = pConfig->mode;

    /* 频率偏值 */
    pBuf[offset++] = pConfig->offVaule;
    #endif
    
    /* 行政区 */
    SET_LEN_OFFSET(pConfig->areaNo.nNum, byte, bytenum);
    if(offset + bytenum + pConfig->areaNo.nNum > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pConfig->areaNo.value, pConfig->areaNo.nNum);
    offset += pConfig->areaNo.nNum;

    /* 设备地址 */
    SET_LEN_OFFSET(pConfig->devAddr.nNum, byte, bytenum);
    if(offset + bytenum + pConfig->devAddr.nNum > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pConfig->devAddr.value, pConfig->devAddr.nNum);
    offset += pConfig->devAddr.nNum;

    /* 终端地址 */
    SET_LEN_OFFSET(pConfig->addr.nNum, byte, bytenum);
    if(offset + bytenum + pConfig->addr.nNum > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pConfig->addr.value, pConfig->addr.nNum);
    offset += pConfig->addr.nNum;

    return offset;
}

/**********************************************************************
* @name      : radio_msg_event_split
* @brief     ：报文通知消息内容解析
* @param[in] ：uint8 *pMsg               消息内容
               uint16 msglen             消息长度
* @param[out]：RADIO_MSG_EVENT_T *pMsgOut  解析结果
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int radio_msg_event_split(uint8 *pMsg, uint16 msglen, RADIO_MSG_EVENT_T *pMsgOut)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 1 || NULL == pMsgOut)
    {
        return -1;
    }

    /* 内容 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    if(offset + bytesnum + len != msglen)
    {
        return -3;
    }
    offset += bytesnum;

    if(len > RADIO_MSG_MAX_LEN)
    {
        return -4;
    }
    pMsgOut->len = len;
    memcpy(pMsgOut->msg, pMsg + offset, len);

    return 0;
}

/**********************************************************************
* @name      : radio_msg_package
* @brief     ：发送数据内容封装
* @param[in] ：BLE_MSG_T *ptMsg           数据内容
               uint16 buflen              缓存长度
* @param[out]：uint8 *pBuf                消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 radio_msg_package(RADIO_MSG_T *ptMsg, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    
    if(NULL == ptMsg || NULL == pBuf || buflen < 1)
    {
        return 0;
    }

    /* 长度 */
    SET_LEN_OFFSET(ptMsg->len, byte, bytenum);
    if(offset + bytenum + ptMsg->len > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;

    /* 内容 */
    memcpy(pBuf + offset, ptMsg->msg, ptMsg->len);
    offset += ptMsg->len;

    return offset;
}

