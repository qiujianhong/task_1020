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
#include "cJSON.h"
#include "message.h"
#include "ble.h"

extern void FastSecondToDate(struct timeval* tv, struct timezone* tz, struct tm* tm);
extern char *db_data_to_base64(uint8 *pData, uint32 datalen);
extern int db_base64_to_data(char *pBase, uint8 *pBuf, uint32 buflen, uint32 *datalen);


/**********************************************************************
* @name      : ble_addr_to_mac
* @brief     ：地址转蓝牙MAC
* @param[in] ：uint8 *pAddr              地址
               uint16 addrlen            地址(字节)长度
               BLE_MAC_E type            地址类型
* @param[out]：uint8 *pMAC               MAC(6个字节长度 大段)
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-23
* @Update    :
**********************************************************************/
int ble_addr_to_mac(uint8 *pAddr, uint16 addrlen, BLE_MAC_E type, uint8 *pMAC)
{
    uint8 MAC[6] = {0};
    uint64 numtemp = 0;
    uint16 i = 0;
    uint8  h= 0;
    uint8  l= 0;

    if(NULL == pAddr || 0 == addrlen || BLE_MAC_UNKNOW == type || NULL == pMAC)
    {
        return -1;
    }
    
    for(i = 0; i < addrlen; i++)
    {
        h = ((pAddr[i] >> 4) & 0x0f);
        l = (pAddr[i] & 0x0f);
        if(h > 9 || l > 9)
        {
            return -2;
        }

        numtemp = numtemp * 100 + h * 10 + l;
    }

    for(i = 0; i < 5; i++)
    {
        pMAC[5 - i] = ((numtemp >> (8 * i)) & 0xFF);
    }
    
    pMAC[0] = 0xC0 + type;

    memcpy(MAC, pMAC, 6);
    return 0;
}

/**********************************************************************
* @name      : ble_addr_to_mac
* @brief     ：蓝牙MAC转地址
* @param[in] ：uint8 *pMAC               MAC(6个字节长度 大段)
               uint16 addrlen            地址(字节)长度
* @param[out]：uint8 *pAddr              地址
* @return    ：地址类型 BLE_MAC_E
* @Create    : 王津剑
* @Date      ：2021-8-24
* @Update    :
**********************************************************************/
uint8 ble_mac_to_addr(uint8 *pMAC, uint8 *pAddr, uint16 addrlen)
{
    uint8 MAC[6] = {0};
    uint64 numtemp = 0;
    uint16 i = 0;
    uint8  h = 0;
    uint8  l = 0;

    if(NULL == pAddr || 0 == addrlen || NULL == pMAC)
    {
        return BLE_MAC_UNKNOW;
    }

    if(0 == (pMAC[0] & 0xC0))
    {
        return BLE_MAC_UNKNOW;
    }

    for(i = 1; i <= 5; i++)
    {
        numtemp = (numtemp << 8) + MAC[i];
    }
    
    for(i = addrlen - 1; i >= 0; i++)
    {
        l = numtemp % 10;
        numtemp = numtemp / 10;
        h = numtemp % 10;
        numtemp = numtemp / 10;
        
        pAddr[i] = h * 16 + l;
    }
    
    return pMAC[0] & 0x3F;
}

/**********************************************************************
* @name      : ble_msg_event_split
* @brief     ：报文通知消息内容解析
* @param[in] ：uint8 *pMsg               消息内容
               uint16 msglen             消息长度
* @param[out]：BLE_MSG_EVENT_T *pMsgOut  解析结果
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_msg_event_split(uint8 *pMsg, uint16 msglen, BLE_MSG_EVENT_T *pMsgOut)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 13 || NULL == pMsgOut)
    {
        return -1;
    }

    /* 端口 */
    MEMCPY_R(&(pMsgOut->port), pMsg, sizeof(uint32));
    offset += sizeof(uint32);

    /* MAC */
    len = pMsg[offset++];
    if(6 != len)
    {
        return -2;
    }
    memcpy(pMsgOut->MAC, pMsg + offset, len);
    offset += len;

    /* 内容 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    if(offset + bytesnum + len != msglen)
    {
        return -3;
    }
    offset += bytesnum;

    if(len > BLE_MSG_MAX_LEN)
    {
        return -4;
    }
    pMsgOut->len = len;
    memcpy(pMsgOut->msg, pMsg + offset, len);

    return 0;
}

/**********************************************************************
* @name      : ble_msg_event_package
* @brief     ：报文通知消息内容封装
* @param[in] ：BLE_MSG_EVENT_T *pMsgIn   通知内容
               uint16 buflen             缓存长度
* @param[out]：uint8 *pBuf               消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_msg_event_package(BLE_MSG_EVENT_T *pMsgIn, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    
    if(NULL == pMsgIn || NULL == pBuf || buflen < 13)
    {
        return 0;
    }

    /* 端口 */
    MEMCPY_R(pBuf, &(pMsgIn->port), sizeof(uint32));
    offset += sizeof(uint32);

    /* MAC */
    pBuf[offset++] = 6;
    memcpy(pBuf + offset, pMsgIn->MAC, 6);
    offset += 6;

    /* 内容 */
    SET_LEN_OFFSET(pMsgIn->len, byte, bytenum);
    if(offset + bytenum + pMsgIn->len > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;

    memcpy(pBuf + offset, pMsgIn->msg, pMsgIn->len);
    offset += pMsgIn->len;
    
    return offset;
}

/**********************************************************************
* @name      : ble_port_event_split
* @brief     ：蓝牙端口通知消息内容解析
* @param[in] ：uint8 *pMsg               消息内容
               uint16 msglen             消息长度
* @param[out]：BLE_PORT_EVENT_T *pPort   解析结果
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_port_event_split(uint8 *pMsg, uint16 msglen, BLE_PORT_EVENT_T *pPort)
{
    uint16 offset = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 13 || NULL == pPort)
    {
        return -1;
    }

    /* 端口 */
    MEMCPY_R(&(pPort->port), pMsg, sizeof(uint32));
    offset += sizeof(uint32);

    /* 模式 */
    pPort->mode = pMsg[offset++];
    
    /* MAC */
    len = pMsg[offset++];
    if(6 != len)
    {
        return -2;
    }
    memcpy(pPort->MAC, pMsg + offset, len);
    offset += len;

    /* 是否连接 */
    pPort->bConnect = pMsg[offset++];
    return 0;
}

/**********************************************************************
* @name      : ble_port_event_package
* @brief     ：蓝牙端口通知内容封装
* @param[in] ：BLE_PORT_EVENT_T *pPort   通知内容
               uint16 buflen             缓存长度
* @param[out]：uint8 *pBuf               消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_port_event_package(BLE_PORT_EVENT_T *pPort, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    
    if(NULL == pPort || NULL == pBuf || buflen < 13)
    {
        return 0;
    }

    /* 端口 */
    MEMCPY_R(pBuf, &(pPort->port), sizeof(uint32));
    offset += sizeof(uint32);

    /* 模式 */
    pBuf[offset++] = pPort->mode;
    
    /* MAC */
    pBuf[offset++] = 6;
    memcpy(pBuf + offset, pPort->MAC, 6);
    offset += 6;

    /* 是否连接 */
    pBuf[offset++] = pPort->bConnect;

    return offset;
}

/**********************************************************************
* @name      : ble_msg_split
* @brief     ：发送数据内容解析
* @param[in] ：uint8 *pMsg               消息内容
               uint16 msglen             消息长度
* @param[out]：BLE_MSG_T *ptMsg          数据内容
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_msg_split(uint8 *pMsg, uint16 msglen, BLE_MSG_T *ptMsg)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 5 || NULL == ptMsg)
    {
        return -1;
    }

    /* 端口 */
    MEMCPY_R(&(ptMsg->port), pMsg, sizeof(uint32));
    offset += sizeof(uint32);

    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    if(offset + bytesnum + len != msglen)
    {
        return -2;
    }
    offset += bytesnum;
    ptMsg->len = MIN(len, BLE_MSG_MAX_LEN);
    memcpy(ptMsg->msg, pMsg + offset, ptMsg->len);
    offset += len;
    
    return 0;
}

/**********************************************************************
* @name      : ble_msg_package
* @brief     ：发送数据内容封装
* @param[in] ：BLE_MSG_T *ptMsg           数据内容
               uint16 buflen              缓存长度
* @param[out]：uint8 *pBuf                消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_msg_package(BLE_MSG_T *ptMsg, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    
    if(NULL == ptMsg || NULL == pBuf || buflen < 5)
    {
        return 0;
    }

    /* 端口 */
    MEMCPY_R(pBuf, &(ptMsg->port), sizeof(uint32));
    offset += sizeof(uint32);

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

/**********************************************************************
* @name      : ble_trans_req_split
* @brief     ：蓝牙转发内容解析
* @param[in] ：uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]：BLE_TRANS_REQ_T *pTrans     转发内容
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_trans_req_split(uint8 *pMsg, uint16 msglen, BLE_TRANS_REQ_T *pTrans)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 13 || NULL == pTrans)
    {
        return -1;
    }

    /* 端口 */
    MEMCPY_R(&(pTrans->port), pMsg, sizeof(uint32));
    offset += sizeof(uint32);

    /* MAC */
    len = pMsg[offset++];
    if(6 != len)
    {
        return -2;
    }
    memcpy(pTrans->MAC, pMsg + offset, len);
    offset += len;

    /* 配对参数 */
    pTrans->match.mode = pMsg[offset++];
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pTrans->match.len = MIN(len, BLE_PWD_MAX);
    memcpy(pTrans->match.pwd, pMsg + offset, pTrans->match.len);
    offset += len;

    /* 发送帧 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pTrans->len = MIN(BLE_MSG_MAX_LEN, len);
    memcpy(pTrans->msg, pMsg + offset, pTrans->len);
    offset += len;

    /* 等待接收 */
    if(1 != pMsg[offset++])
    {
        pTrans->waittime.msgtime = 0;
        pTrans->waittime.bytetime = 0;
    }
    else
    {
        MEMCPY_R(&(pTrans->waittime.msgtime), pMsg + offset, sizeof(uint16));
        offset += sizeof(uint16);
        MEMCPY_R(&(pTrans->waittime.bytetime), pMsg + offset, sizeof(uint16));
        offset += sizeof(uint16);
    }

    return 0;
}

/**********************************************************************
* @name      : ble_trans_req_package
* @brief     ：蓝牙转发内容封装
* @param[in] ：BLE_TRANS_REQ_T *pTrans     转发内容
               uint16 buflen               缓存长度
* @param[out]：uint8 *pBuf                 消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_trans_req_package(BLE_TRANS_REQ_T *pTrans, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    
    if(NULL == pTrans || NULL == pBuf || buflen < 20)
    {
        return 0;
    }
    /* 端口 */
    MEMCPY_R(pBuf, &(pTrans->port), sizeof(uint32));
    offset += sizeof(uint32);

    /* MAC */
    pBuf[offset++] = 6;
    memcpy(pBuf + offset, pTrans->MAC, 6);

    /* 配对参数 */
    pBuf[offset++] = pTrans->match.mode;
    SET_LEN_OFFSET(pTrans->match.len, byte, bytenum);
    if(offset + bytenum + pTrans->match.len > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pTrans->match.pwd, pTrans->match.len);
    offset += pTrans->match.len;

    /* 发送帧 */
    SET_LEN_OFFSET(pTrans->len, byte, bytenum);
    if(offset + bytenum + pTrans->len + 1 > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pTrans->msg, pTrans->len);
    offset += pTrans->len;

    /* 等待 */
    if(0 == pTrans->waittime.msgtime && 0 == pTrans->waittime.bytetime)
    {
        pBuf[offset++] = 0;
    }
    else
    {
        if(offset + 5 > buflen)
        {
            return 0;
        }
        pBuf[offset++] = 1;
        MEMCPY_R(pBuf + offset, &(pTrans->waittime.msgtime), sizeof(uint16));
        offset += sizeof(uint16);
        MEMCPY_R(pBuf + offset, &(pTrans->waittime.bytetime), sizeof(uint16));
        offset += sizeof(uint16);
    }

    return offset;
}

/**********************************************************************
* @name      : ble_trans_ack_split
* @brief     ：蓝牙转发应答内容解析
* @param[in] ：uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]：BLE_TRANS_REQ_T *pTrans     转发内容
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_trans_ack_split(uint8 *pMsg, uint16 msglen, BLE_TRANS_ACK_T *pTrans)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 5 || NULL == pTrans)
    {
        return -1;
    }

    /* 结果 */
    MEMCPY_R(&(pTrans->ret), pMsg, sizeof(int));
    offset += sizeof(int);

    /* 接收帧 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pTrans->len = MIN(BLE_MSG_MAX_LEN, len);
    memcpy(pTrans->msg, pMsg + offset, pTrans->len);
    offset += len;

    return 0;
}

/**********************************************************************
* @name      : ble_trans_ack_package
* @brief     ：蓝牙转发应答内容封装
* @param[in] ：BLE_TRANS_ACK_T *pTrans     转发内容
               uint16 buflen               缓存长度
* @param[out]：uint8 *pBuf                 消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_trans_ack_package(BLE_TRANS_ACK_T *pTrans, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    
    if(NULL == pTrans || NULL == pBuf || buflen < 5)
    {
        return 0;
    }
    
    /* 结果 */
    MEMCPY_R(pBuf, &(pTrans->ret), sizeof(int));
    offset += sizeof(int);

    /* 接收帧 */
    SET_LEN_OFFSET(pTrans->len, byte, bytenum);
    if(offset + bytenum + pTrans->len > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;
    memcpy(pBuf + offset, pTrans->msg, pTrans->len);
    offset += pTrans->len;

    return offset;
}

/**********************************************************************
* @name      : ble_config_split
* @brief     ：蓝牙转发应答内容解析
* @param[in] ：uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]：BLE_CONFIG_T *pConfig       蓝牙参数
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_config_split(uint8 *pMsg, uint16 msglen, BLE_CONFIG_T *pConfig)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 13 || NULL == pConfig)
    {
        return -1;
    }

    /* 蓝牙名称 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pConfig->len = MIN(BLE_NAME_MAX_LEN, len);
    if(pConfig->len > 0)
    {
        memcpy(pConfig->name, pMsg + offset, pConfig->len);
    }
    offset += len;

    /* MAC */
    len = pMsg[offset++];
    if(6 != len)
    {
        return -2;
    }
    memcpy(pConfig->MAC, pMsg + offset, len);
    offset += len;

    /* 发射 */
    pConfig->power = pMsg[offset++];

    /* 广播间隔 */
    MEMCPY_R(&(pConfig->broadcast), pMsg + offset, sizeof(uint16));
    offset += sizeof(uint16);

    /* 扫描间隔 */
    MEMCPY_R(&(pConfig->scan), pMsg + offset, sizeof(uint16));
    offset += sizeof(uint16);

    return 0;
}

/**********************************************************************
* @name      : ble_sn_split
* @brief     ：蓝牙转发应答内容解析
* @param[in] ：uint8 *pMsg                 消息内容
               uint16 msglen               消息长度
* @param[out]：BLE_CONFIG_T *pConfig       蓝牙参数
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_sn_split(uint8 *pMsg, uint16 msglen, BLE_SN_T *pSn)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    
    if(NULL == pMsg || msglen < 13 || NULL == pSn)
    {
        return -1;
    }

    /* 蓝牙名称 */
    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    offset += bytesnum;
    pSn->num = MIN(BLE_SN_MAX, len);
    if (pSn->num > 0)
    {
        memcpy(pSn->sn, pMsg + offset, pSn->num);
    }
    offset += len;

    return 0;
}

/**********************************************************************
* @name      : ble_config_package
* @brief     ：蓝牙参数配置内容封装
* @param[in] ：BLE_CONFIG_T *pConfig       蓝牙参数
               uint16 buflen               缓存长度
* @param[out]：uint8 *pBuf                 消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_config_package(BLE_CONFIG_T *pConfig, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    
    if(NULL == pConfig || NULL == pBuf || buflen < 13)
    {
        return 0;
    }
    
    /* 蓝牙名称 */
    pBuf[offset++] = pConfig->len;
    if(pConfig->len > 0)
    {
        memcpy(pBuf + offset, pConfig->name, pConfig->len);
        offset += pConfig->len;
    }
    /* MAC */
    pBuf[offset++] = 6;
    memcpy(pBuf + offset, pConfig->MAC, 6);
    offset += 6;

    /* 发射功率档 */
    pBuf[offset++] = pConfig->power;
    
    /* 广播间隔 */
    MEMCPY_R(pBuf + offset, &(pConfig->broadcast), sizeof(uint16));
    offset += sizeof(uint16);

    /* 扫描间隔 */
    MEMCPY_R(pBuf + offset, &(pConfig->scan), sizeof(uint16));
    offset += sizeof(uint16);

    return offset;
}

/**********************************************************************
* @name      : ble_port_config_split
* @brief     ：蓝牙端口配置内容解析
* @param[in] ：uint8 *pMsg                  消息内容
               uint16 msglen                消息长度
* @param[out]：BLE_PORTS_CONF_T *pPortConfig 端口配置
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_port_config_split(uint8 *pMsg, uint16 msglen, BLE_PORTS_CONF_T *pPortConfig)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    uint32 i = 0;
    
    if(NULL == pMsg || msglen < 1 || NULL == pPortConfig)
    {
        return -1;
    }

    GET_LEN_OFFSET(len, pMsg, bytesnum);
    offset += bytesnum;
    pPortConfig->num = MIN(BLE_PORT_MAX * 2, len);

    for(i = 0; i < pPortConfig->num;  i++)
    {
        /* 端口 */
        MEMCPY_R(&(pPortConfig->tConf[i].port), pMsg + offset, sizeof(uint32));
        offset += sizeof(uint32);

        /* 模式 */
        pPortConfig->tConf[i].mode = pMsg[offset++];

        if(6 != pMsg[offset++])
        {
            return -2;
        }
        memcpy(pPortConfig->tConf[i].MAC, pMsg + offset, 6);
        offset += 6;

        /* 配对参数 */
        pPortConfig->tConf[i].match.mode = pMsg[offset++];
        GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
        offset += bytesnum;
        pPortConfig->tConf[i].match.len = MIN(len, BLE_PWD_MAX);
        memcpy(pPortConfig->tConf[i].match.pwd, pMsg + offset, pPortConfig->tConf[i].match.len);
        offset += len;
    }
    return 0;
}

/**********************************************************************
* @name      : ble_port_config_package
* @brief     ：蓝牙端口配置内容封装
* @param[in] ：BLE_PORTS_CONF_T *pPortConfig 端口配置
               uint16 buflen                缓存长度
* @param[out]：uint8 *pBuf                  消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_port_config_package(BLE_PORTS_CONF_T *pPortConfig, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    uint32 i = 0;
    
    if(NULL == pPortConfig || NULL == pBuf || buflen < 8)
    {
        return 0;
    }

    /* 数量 */
    SET_LEN_OFFSET(pPortConfig->num, byte, bytenum);
    if(offset + bytenum > buflen)
    {
        return 0;
    }
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;

    for(i = 0; i < pPortConfig->num; i++)
    {
        /* 端口号 */
        MEMCPY_R(pBuf + offset, &(pPortConfig->tConf[i].port), sizeof(uint32));
        offset += sizeof(uint32);

        /* 模式 */
        pBuf[offset++] = pPortConfig->tConf[i].mode;

        /* MAC 地址 */
        pBuf[offset++] = 6;
        memcpy(pBuf + offset, pPortConfig->tConf[i].MAC, 6);
        offset += 6;

        /* 配对参数 */
        pBuf[offset++] = pPortConfig->tConf[i].match.mode;
        SET_LEN_OFFSET(pPortConfig->tConf[i].match.len, byte, bytenum);
        if(offset + bytenum + pPortConfig->tConf[i].match.len > buflen)
        {
            return 0;
        }
        memcpy(pBuf + offset, byte, bytenum);
        offset += bytenum;
        memcpy(pBuf + offset, pPortConfig->tConf[i].match.pwd, pPortConfig->tConf[i].match.len);
        offset += pPortConfig->tConf[i].match.len;
    }
    return offset;
}

/**********************************************************************
* @name      : ble_connect_split
* @brief     ：蓝牙连接信息内容解析
* @param[in] ：uint8 *pMsg                  消息内容
               uint16 msglen                消息长度
* @param[out]：BLE_PORT_CONF_T *pPortConfig 端口配置
* @return    ：0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
int ble_connect_split(uint8 *pMsg, uint16 msglen, BLE_CONNECT_INFO_T *pConnect)
{
    uint16 offset = 0;
    uint8  bytesnum = 0;
    uint16 len = 0;
    uint16 i = 0;
    
    if(NULL == pMsg || msglen < 1 || NULL == pConnect)
    {
        return -1;
    }

    GET_LEN_OFFSET(len, pMsg + offset, bytesnum);
    pConnect->num = MIN(len, BLE_PORT_MAX);
    offset += bytesnum;

    for(i = 0; i < pConnect->num; i++)
    {
        /* 端口 */
        MEMCPY_R(&(pConnect->list[i].port), pMsg + offset, sizeof(uint32));
        offset += sizeof(uint32);

        /* 模式 */
        pConnect->list[i].mode = pMsg[offset++];

        /* 是否连接 */
        pConnect->list[i].bConnect = pMsg[offset++];
        
        /* MAC */
        len = pMsg[offset++];
        if(6 != len)
        {
            return -2;
        }
        memcpy(pConnect->list[i].MAC, pMsg + offset, len);
        offset += len;
    }
    return 0;
}

/**********************************************************************
* @name      : ble_connect_package
* @brief     ：蓝牙连接信息内容封装
* @param[in] ：BLE_CONNECT_INFO_T *pConnect 连接信息
               uint16 buflen                缓存长度
* @param[out]：uint8 *pBuf                  消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_connect_package(BLE_CONNECT_INFO_T *pConnect, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    uint16 i = 0;
    
    if(NULL == pConnect || NULL == pBuf || buflen < 1)
    {
        return 0;
    }

    SET_LEN_OFFSET(pConnect->num, byte, bytenum);
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;

    for(i = 0; i < pConnect->num; i++)
    {
        /* 端口号 */
        MEMCPY_R(pBuf + offset, &(pConnect->list[i].port), sizeof(uint32));
        offset += sizeof(uint32);

        /* 模式 */
        pBuf[offset++] = pConnect->list[i].mode;

        /* 是否连接 */
        pBuf[offset++] = pConnect->list[i].bConnect;

        /* MAC */
        pBuf[offset++] = 6;
        memcpy(pBuf + offset, pConnect->list[i].MAC, 6);
        offset += 6;
    }
    return offset;
}

/**********************************************************************
* @name      : ble_connect_package
* @brief     ：蓝牙连接信息内容封装
* @param[in] ：BLE_CONNECT_INFO_T *pConnect 连接信息
               uint16 buflen                缓存长度
* @param[out]：uint8 *pBuf                  消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-9
* @Update    :
**********************************************************************/
uint16 ble_sn_package(BLE_SN_T *pSn, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    
    if(NULL == pSn || NULL == pBuf || buflen < 1)
    {
        return 0;
    }

    SET_LEN_OFFSET(pSn->num, byte, bytenum);
    memcpy(pBuf + offset, byte, bytenum);
    offset += bytenum;

    memcpy(pBuf + offset, pSn->sn, pSn->num);
    offset += pSn->num;
    
    return offset;
}

/**********************************************************************
* @name      : ble_common_req_package
* @brief     : 蓝牙检表内容解析
* @param[in] : uint8 *pMsg                  消息内容
               uint16 msglen                消息长度
* @param[out]: BLE_CHECK_T *pTcheck         检表配置
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2022-3-1
* @Update    :
**********************************************************************/
int ble_check_split(uint8 *pMsg, uint16 msglen, BLE_CHECK_T *pTcheck)
{
    uint16 offset = 0;
    uint8 i = 0;
    
    if(NULL == pMsg || msglen < 11 || NULL == pTcheck)
    {
        return -1;
    }

    pTcheck->type = pMsg[offset++];
    pTcheck->interval = pMsg[offset++];
    pTcheck->power = pMsg[offset++];
    if(6 != pMsg[offset])
    {
        return -2;
    }
    offset++;
    memcpy(pTcheck->MAC, pMsg + offset, 6);
    offset += 6;

    pTcheck->ch_num = pMsg[offset++];
    if(0 == pTcheck->ch_num || pTcheck->ch_num > BLE_CHANNEL_MAX)
    {
        return -2;
    }
    for(i = 0; i < pTcheck->ch_num; i++)
    {
        MEMCPY_R(&(pTcheck->freq[i]), pMsg + offset, 2);
        offset += 2;
    }
    
    return 0;
}

/**********************************************************************
* @name      : ble_common_req_package
* @brief     : 蓝牙检表内容封装
* @param[in] : BLE_CHECK_T *pTcheck        检表配置
               uint16 buflen               缓存长度
* @param[out]: uint8 *pBuf                 消息内容
* @return    : 消息长度
* @Create    : 王津剑
* @Date      ：2022-3-1
* @Update    :
**********************************************************************/
uint16 ble_check_package(BLE_CHECK_T *pTcheck, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  i = 0;
    
    if(NULL == pTcheck || 0 == pTcheck->ch_num || NULL == pBuf || buflen < 1)
    {
        return 0;
    }
    
    pBuf[offset++] = pTcheck->type;
    pBuf[offset++] = pTcheck->interval;
    pBuf[offset++] = pTcheck->power;
    pBuf[offset++] = 6;
    memcpy(pBuf + offset, pTcheck->MAC, 6);
    offset += 6;
    pBuf[offset++] = pTcheck->ch_num;
    for(i = 0; i < pTcheck->ch_num; i++)
    {
        MEMCPY_R(pBuf + offset, &(pTcheck->freq[i]), 2);
        offset += 2;
    }
    return offset;
}

/**********************************************************************
* @name      : ble_common_req_package
* @brief     : 蓝牙通用请求内容封装(JSON)
* @param[in] : uint32 token              消息序号
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
char *ble_common_req_package(uint32 token)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pMsg = cJSON_Print(root);
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_common_res_split
* @brief     : 蓝牙通用应答内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: uint32 *status             状态
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int ble_common_res_split(char *infopath, void *pJson, uint32 *status)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == status)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_SETPARAM, infopath) && 0 != strcmp(BLE_TOPOC_SETPORT, infopath) 
       && 0 != strcmp(BLE_TOPOC_SETDETECT, infopath) && 0 != strcmp(BLE_TOPOC_SENDDATA, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "status");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    *status = jTemp->valueint;
    
    return 0;
}

/**********************************************************************
* @name      : ble_common_res_package
* @brief     : 蓝牙通用应答内容封装(JSON)
* @param[in] : uint32 token              消息序号
               uint32 status             状态
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
char *ble_common_res_package(uint32 token, uint32 status)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "status", cJSON_CreateNumber(status));

    pMsg = cJSON_Print(root);
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_setparam_req_split
* @brief     : 蓝牙参数配置请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_CONFIG_T *pConfig      蓝牙配置
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int ble_setparam_req_split(char *infopath, void *pJson, BLE_CONFIG_T *pConfig)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    uint32 datalen = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pConfig)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_SETPARAM, infopath))
    {
        return -2;
    }

    memset(pConfig, 0, sizeof(BLE_CONFIG_T));
    jTemp = cJSON_GetObjectItem(pCJson, "name");
    if(NULL == jTemp || cJSON_String != jTemp->type)
    {
        return -3;
    }
    pConfig->len = MIN(strlen(jTemp->valuestring), BLE_NAME_MAX_LEN-1);
    if(pConfig->len > 0)
    {
        memcpy(pConfig->name, jTemp->valuestring, pConfig->len);
    }

    jTemp = cJSON_GetObjectItem(pCJson, "mac");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    db_base64_to_data(jTemp->valuestring, pConfig->MAC, 6, &datalen);
    if(6 != datalen)
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "power");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -6;
    }
    pConfig->power = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "Broadcastinterval");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -7;
    }
    pConfig->broadcast = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "scaninterval");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -8;
    }
    pConfig->scan = jTemp->valueint;
    return 0;
}

/**********************************************************************
* @name      : ble_setparam_req_package
* @brief     : 蓝牙参数配置请求内容封装(JSON)
* @param[in] : uint32 token              消息序号
               BLE_CONFIG_T *pConfig     蓝牙参数
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
char *ble_setparam_req_package(uint32 token, BLE_CONFIG_T *pConfig)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMacStr = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "name", cJSON_CreateString(pConfig->name));

    pMacStr = db_data_to_base64(pConfig->MAC, 6);
    if(NULL == pMacStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "mac", cJSON_CreateString(pMacStr));
    cJSON_AddItemToObject(root, "power", cJSON_CreateNumber(pConfig->power));
    cJSON_AddItemToObject(root, "Broadcastinterval", cJSON_CreateNumber(pConfig->broadcast));
    cJSON_AddItemToObject(root, "scaninterval", cJSON_CreateNumber(pConfig->scan));

    pMsg = cJSON_Print(root);
    free(pMacStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_getparam_res_split
* @brief     : 蓝牙参数配置查询响应内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_CONFIG_T *pConfig      蓝牙配置
               uint32 *status             状态
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int ble_getparam_res_split(char *infopath, void *pJson, BLE_CONFIG_T *pConfig, uint32 *status)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    uint32 datalen = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pConfig)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_GETPARAM, infopath))
    {
        return -2;
    }

    memset(pConfig, 0, sizeof(BLE_CONFIG_T));

    jTemp = cJSON_GetObjectItem(pCJson, "status");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    *status = jTemp->valueint;
    
    jTemp = cJSON_GetObjectItem(pCJson, "name");
    if(NULL == jTemp || cJSON_String != jTemp->type)
    {
        return -4;
    }
    pConfig->len = MIN(strlen(jTemp->valuestring), BLE_NAME_MAX_LEN-1);
    if(pConfig->len > 0)
    {
        memcpy(pConfig->name, jTemp->valuestring, pConfig->len);
    }

    jTemp = cJSON_GetObjectItem(pCJson, "mac");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -5;
    }
    db_base64_to_data(jTemp->valuestring, pConfig->MAC, 6, &datalen);
    if(6 != datalen)
    {
        return -6;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "power");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -7;
    }
    pConfig->power = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "Broadcastinterval");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -8;
    }
    pConfig->broadcast = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "scaninterval");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -9;
    }
    pConfig->scan = jTemp->valueint;
    return 0;
}

/**********************************************************************
* @name      : ble_getparam_res_package
* @brief     : 蓝牙参数配置请求内容封装(JSON)
* @param[in] : uint32 token              消息序号
               BLE_CONFIG_T *pConfig     蓝牙参数
               uint32 status             状态
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
char *ble_getparam_res_package(uint32 token, BLE_CONFIG_T *pConfig, uint32 status)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMacStr = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "status", cJSON_CreateNumber(status));
    cJSON_AddItemToObject(root, "name", cJSON_CreateString(pConfig->name));

    pMacStr = db_data_to_base64(pConfig->MAC, 6);
    if(NULL == pMacStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "mac", cJSON_CreateString(pMacStr));
    cJSON_AddItemToObject(root, "power", cJSON_CreateNumber(pConfig->power));
    cJSON_AddItemToObject(root, "Broadcastinterval", cJSON_CreateNumber(pConfig->broadcast));
    cJSON_AddItemToObject(root, "scaninterval", cJSON_CreateNumber(pConfig->scan));

    pMsg = cJSON_Print(root);
    free(pMacStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_setport_req_split
* @brief     : 蓝牙端口配置请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_PORT_CONF_T *pPort     端口配置
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int ble_setport_req_split(char *infopath, void *pJson, BLE_PORT_CONF_T *pPort)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    uint32 datalen = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pPort)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_SETPORT, infopath))
    {
        return -2;
    }

    memset(pPort, 0, sizeof(BLE_PORT_CONF_T));
    jTemp = cJSON_GetObjectItem(pCJson, "port");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    pPort->port = jTemp->valueint;
    
    jTemp = cJSON_GetObjectItem(pCJson, "mode");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -4;
    }
    pPort->mode = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "mac");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -5;
    }
    db_base64_to_data(jTemp->valuestring, pPort->MAC, 6, &datalen);
    if(6 != datalen)
    {
        return -6;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "pairmode");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -7;
    }
    pPort->match.mode = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "password");
    if(NULL == jTemp || cJSON_String != jTemp->type)
    {
        return -8;
    }
    pPort->match.len = MIN(BLE_PWD_MAX, strlen(jTemp->valuestring));
    if(pPort->match.len > 0)
    {
        memcpy(pPort->match.pwd, jTemp->valuestring, pPort->match.len);
    }
    return 0;
}

/**********************************************************************
* @name      : ble_setport_req_package
* @brief     : 蓝牙端口配置请求内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_PORT_CONF_T *pPort     端口配置
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
char *ble_setport_req_package(uint32 token, BLE_PORT_CONF_T *pPort)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMacStr = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "port", cJSON_CreateNumber(pPort->port));
    cJSON_AddItemToObject(root, "mode", cJSON_CreateNumber(pPort->mode));

    pMacStr = db_data_to_base64(pPort->MAC, 6);
    if(NULL == pMacStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "mac", cJSON_CreateString(pMacStr));
    cJSON_AddItemToObject(root, "pairmode", cJSON_CreateNumber(pPort->match.mode));
    cJSON_AddItemToObject(root, "password", cJSON_CreateString(pPort->match.pwd));

    pMsg = cJSON_Print(root);
    free(pMacStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_port_notify_split
* @brief     : 蓝牙端口通知内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_PORT_EVENT_T *pPort    端口通知
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int ble_port_notify_split(char *infopath, void *pJson, BLE_PORT_EVENT_T *pPort)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    uint32 datalen = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pPort)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_PORTNOTIFY, infopath))
    {
        return -2;
    }

    memset(pPort, 0, sizeof(BLE_PORT_EVENT_T));
    jTemp = cJSON_GetObjectItem(pCJson, "port");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    pPort->port = jTemp->valueint;
    
    jTemp = cJSON_GetObjectItem(pCJson, "mode");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -4;
    }
    pPort->mode = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "mac");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -5;
    }
    db_base64_to_data(jTemp->valuestring, pPort->MAC, 6, &datalen);
    if(6 != datalen)
    {
        return -6;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "connect");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -7;
    }
    pPort->bConnect = jTemp->valueint;

    return 0;
}

/**********************************************************************
* @name      : ble_port_notify_package
* @brief     : 蓝牙端口通知内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_PORT_EVENT_T *pPort    端口通知
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
char *ble_port_notify_package(uint32 token, BLE_PORT_EVENT_T *pPort)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMacStr = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "port", cJSON_CreateNumber(pPort->port));
    cJSON_AddItemToObject(root, "mode", cJSON_CreateNumber(pPort->mode));

    pMacStr = db_data_to_base64(pPort->MAC, 6);
    if(NULL == pMacStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "mac", cJSON_CreateString(pMacStr));
    cJSON_AddItemToObject(root, "connect", cJSON_CreateNumber(pPort->bConnect));

    pMsg = cJSON_Print(root);
    free(pMacStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_setdetect_req_split
* @brief     : 蓝牙检表配置请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_CHECK_T *pTcheck       检表配置
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2022-3-1
* @Update    :
**********************************************************************/
int ble_setdetect_req_split(char *infopath, void *pJson, BLE_CHECK_T *pTcheck)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    cJSON *jBody = NULL;
    uint32 datalen = 0;
    uint32 i = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pTcheck)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_SETDETECT, infopath))
    {
        return -2;
    }

    memset(pTcheck, 0, sizeof(BLE_CHECK_T));
    jTemp = cJSON_GetObjectItem(pCJson, "mode");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    pTcheck->type = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "interval");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -4;
    }
    pTcheck->interval = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "power");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -5;
    }
    pTcheck->power = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "mac");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -6;
    }
    db_base64_to_data(jTemp->valuestring, pTcheck->MAC, 6, &datalen);
    if(6 != datalen)
    {
        return -7;
    }

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || cJSON_Array != jTemp->type)
    {
        return -8;
    }
    pTcheck->ch_num = cJSON_GetArraySize(jBody);
    if(0 == pTcheck->ch_num || pTcheck->ch_num > BLE_CHANNEL_MAX)
    {
        return -9;
    }
    for(i = 0; i < pTcheck->ch_num; i++)
    {
        jTemp = cJSON_GetArrayItem(jBody, i);
        if(NULL != jTemp)
        {
            pTcheck->freq[i] = jTemp->valueint;
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : ble_setdetect_req_package
* @brief     : 蓝牙检表配置请求内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_CHECK_T *pTcheck       检表配置
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2022-3-1
* @Update    :
**********************************************************************/
char *ble_setdetect_req_package(uint32 token, BLE_CHECK_T *pTcheck)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    cJSON  *body = NULL;
    struct  timeval  tv;
    struct  timezone tz;
    struct  tm       tm;
    uint8   i = 0;
    char   *pMsg = NULL;
    char   *pMacStr = NULL;
    
    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "mode", cJSON_CreateNumber(pTcheck->type));
    cJSON_AddItemToObject(root, "interval", cJSON_CreateNumber(pTcheck->interval));
    cJSON_AddItemToObject(root, "power", cJSON_CreateNumber(pTcheck->power));
    pMacStr = db_data_to_base64(pTcheck->MAC, 6);
    if(NULL == pMacStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "addr", cJSON_CreateString(pMacStr));
    body = cJSON_CreateArray();
    if(NULL == root)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "body", body);
    for(i = 0; i < pTcheck->ch_num; i++)
    {
        cJSON_AddItemToArray(body, cJSON_CreateNumber(pTcheck->freq[i]));
    }

    pMsg = cJSON_Print(root);
    free(pMacStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_getconnect_res_split
* @brief     : 蓝牙参数配置查询响应内容解析(JSON)
* @param[in] : char *infopath               信息对象
               void *pJson                  JSON
* @param[out]: BLE_CONNECT_INFO_T *pConnect 连接信息
               uint32 *status               状态
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
int ble_getconnect_res_split(char *infopath, void *pJson, BLE_CONNECT_INFO_T *pConnect, uint32 *status)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jArray = NULL;
    cJSON *jTemp = NULL;
    uint32 datalen = 0;
    uint32 i = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pConnect || NULL == status)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_CONNINFO, infopath))
    {
        return -2;
    }

    memset(pConnect, 0, sizeof(BLE_CONNECT_INFO_T));

    jTemp = cJSON_GetObjectItem(pCJson, "status");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    *status = jTemp->valueint;
    
    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || cJSON_Array != jBody->type)
    {
        return -4;
    }
    pConnect->num = MIN(cJSON_GetArraySize(jBody), BLE_PORT_MAX);
    for(i = 0; i < pConnect->num; i++)
    {
        jArray = cJSON_GetArrayItem(jBody, i);
        if(NULL == jArray || FALSE == cJSON_IsObject(jArray))
        {
            continue;
        }

        jTemp = cJSON_GetObjectItem(jArray, "port");
        if(NULL == jTemp || cJSON_Number != jTemp->type)
        {
            return -5;
        }
        pConnect->list[i].port = jTemp->valueint;

        jTemp = cJSON_GetObjectItem(jArray, "mode");
        if(NULL == jTemp || cJSON_Number != jTemp->type)
        {
            return -6;
        }
        pConnect->list[i].mode = jTemp->valueint;

        jTemp = cJSON_GetObjectItem(jArray, "connect");
        if(NULL == jTemp || cJSON_Number != jTemp->type)
        {
            return -7;
        }
        pConnect->list[i].bConnect = jTemp->valueint;

        jTemp = cJSON_GetObjectItem(jArray, "mac");
        if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
        {
            return -8;
        }
        db_base64_to_data(jTemp->valuestring, pConnect->list[i].MAC, 6, &datalen);
        if(6 != datalen)
        {
            return -9;
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : ble_getconnect_res_package
* @brief     : 蓝牙参数配置响应内容封装(JSON)
* @param[in] : uint32 token                 消息序号
               BLE_CONNECT_INFO_T *pConnect 连接信息
               uint32 status                状态
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
char *ble_getconnect_res_package(uint32 token, BLE_CONNECT_INFO_T *pConnect, uint32 status)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    cJSON  *body = NULL;
    cJSON  *conn = NULL;
    struct  timeval  tv;
    struct  timezone tz;
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMacStr = NULL;
    uint32  i = 0;
    
    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "status", cJSON_CreateNumber(status));

    body = cJSON_CreateArray();
    if(NULL == body)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "body", body);
    for(i = 0; i < pConnect->num; i++)
    {
        conn = cJSON_CreateObject();
        if(NULL == conn)
        {
            goto end;
        }
        cJSON_AddItemToArray(body, conn);
        cJSON_AddItemToObject(conn, "port", cJSON_CreateNumber(pConnect->list[i].port));
        cJSON_AddItemToObject(conn, "mode", cJSON_CreateNumber(pConnect->list[i].mode));
        cJSON_AddItemToObject(conn, "connect", cJSON_CreateNumber(pConnect->list[i].bConnect));
        pMacStr = db_data_to_base64(pConnect->list[i].MAC, 6);
        if(NULL == pMacStr)
        {
            goto end;
        }
        cJSON_AddItemToObject(conn, "mac", cJSON_CreateString(pMacStr));
        free(pMacStr);
        pMacStr = NULL;
    }

    pMsg = cJSON_Print(root);
end:
    if(pMacStr)
    {
        free(pMacStr);
    }
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_sendmsg_req_split
* @brief     : 发送数据请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_MSG_T *ptMsg           发送消息
               uint8 *prio                优先级
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
int ble_sendmsg_req_split(char *infopath, void *pJson, BLE_MSG_T *ptMsg, uint8 *prio)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == ptMsg || NULL == prio)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_SENDDATA, infopath))
    {
        return -2;
    }

    memset(ptMsg, 0, sizeof(BLE_MSG_T));
    jTemp = cJSON_GetObjectItem(pCJson, "prio");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    *prio = jTemp->valueint;
    
    jTemp = cJSON_GetObjectItem(pCJson, "port");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -4;
    }
    ptMsg->port = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "data");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return 0;
    }
    db_base64_to_data(jTemp->valuestring, ptMsg->msg, BLE_MSG_MAX_LEN, &(ptMsg->len));
    
    return 0;
}

/**********************************************************************
* @name      : ble_sendmsg_req_package
* @brief     : 发送数据请求内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_MSG_T *ptMsg           发送消息
               uint8 prio                 优先级
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
char *ble_sendmsg_req_package(uint32 token, BLE_MSG_T *ptMsg, uint8 prio)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMsgStr = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "prio", cJSON_CreateNumber(prio));
    cJSON_AddItemToObject(root, "port", cJSON_CreateNumber(ptMsg->port));

    pMsgStr = db_data_to_base64(ptMsg->msg, ptMsg->len);
    if(NULL == pMsgStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "data", cJSON_CreateString(pMsgStr));

    pMsg = cJSON_Print(root);
    free(pMsgStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_transmsg_req_split
* @brief     : 透传请求内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_TRANS_REQ_T *pTrans    转发消息
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
int ble_transmsg_req_split(char *infopath, void *pJson, BLE_TRANS_REQ_T *pTrans)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    uint32 datalen = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pTrans)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_FORWARD, infopath))
    {
        return -2;
    }

    memset(pTrans, 0, sizeof(BLE_TRANS_REQ_T));
    
    jTemp = cJSON_GetObjectItem(pCJson, "port");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    pTrans->port = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "mac");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    db_base64_to_data(jTemp->valuestring, pTrans->MAC, 6, &datalen);
    if(6 != datalen)
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "pairmode");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -6;
    }
    pTrans->match.mode = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "password");
    if(NULL == jTemp || cJSON_String != jTemp->type)
    {
        return -7;
    }
    pTrans->match.len = MIN(strlen(jTemp->valuestring), BLE_PWD_MAX);
    if(pTrans->match.len > 0)
    {
        memcpy(pTrans->match.pwd, jTemp->valuestring, pTrans->match.len);
    }

    jTemp = cJSON_GetObjectItem(pCJson, "data");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -8;
    }
    db_base64_to_data(jTemp->valuestring, pTrans->msg, BLE_MSG_MAX_LEN, &datalen);
    pTrans->len = datalen;

    jTemp = cJSON_GetObjectItem(pCJson, "frametimeout");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -9;
    }
    pTrans->waittime.msgtime = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "chartimeout");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -10;
    }
    pTrans->waittime.bytetime = jTemp->valueint;
    
    return 0;
}

/**********************************************************************
* @name      : ble_transmsg_req_package
* @brief     : 透传请求内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_TRANS_REQ_T *pTrans    转发消息
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
char *ble_transmsg_req_package(uint32 token, BLE_TRANS_REQ_T *pTrans)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMsgStr = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "port", cJSON_CreateNumber(pTrans->port));

    pMsgStr = db_data_to_base64(pTrans->MAC, 6);
    if(NULL == pMsgStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "mac", cJSON_CreateString(pMsgStr));
    free(pMsgStr);
    pMsgStr = NULL;

    cJSON_AddItemToObject(root, "pairmode", cJSON_CreateNumber(pTrans->match.mode));
    cJSON_AddItemToObject(root, "password", cJSON_CreateString(pTrans->match.pwd));

    pMsgStr = db_data_to_base64(pTrans->msg, pTrans->len);
    if(NULL == pMsgStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "data", cJSON_CreateString(pMsgStr));

    cJSON_AddItemToObject(root, "frametimeout", cJSON_CreateNumber(pTrans->waittime.msgtime));
    cJSON_AddItemToObject(root, "chartimeout", cJSON_CreateNumber(pTrans->waittime.bytetime));

    pMsg = cJSON_Print(root);
    free(pMsgStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_transmsg_res_split
* @brief     : 透传响应内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_TRANS_ACK_T *pAck      应答消息
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
int ble_transmsg_res_split(char *infopath, void *pJson, BLE_TRANS_ACK_T *pAck)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == pAck)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_FORWARD, infopath))
    {
        return -2;
    }

    memset(pAck, 0, sizeof(BLE_TRANS_ACK_T));
    
    jTemp = cJSON_GetObjectItem(pCJson, "status");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    pAck->ret = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "data");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    db_base64_to_data(jTemp->valuestring, pAck->msg, BLE_MSG_MAX_LEN, &(pAck->len));
    
    return 0;
}

/**********************************************************************
* @name      : ble_transmsg_res_package
* @brief     : 透传响应内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_TRANS_ACK_T *pAck      应答消息
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
char *ble_transmsg_res_package(uint32 token, BLE_TRANS_ACK_T *pAck)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMsgStr = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "status", cJSON_CreateNumber(pAck->ret));

    if(pAck->len > 0)
    {
        pMsgStr = db_data_to_base64(pAck->msg, pAck->len);
        if(NULL == pMsgStr)
        {
            goto end;
        }
        cJSON_AddItemToObject(root, "data", cJSON_CreateString(pMsgStr));
    }
    else
    {
        cJSON_AddItemToObject(root, "data", cJSON_CreateString(""));
    }

    pMsg = cJSON_Print(root);
    free(pMsgStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

/**********************************************************************
* @name      : ble_msg_notify_split
* @brief     : 报文通知内容解析(JSON)
* @param[in] : char *infopath             信息对象
               void *pJson                JSON
* @param[out]: BLE_MSG_EVENT_T *pMsgEvent 报文通知
* @return    : 0-成功/其他-错误码
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
int ble_msg_notify_split(char *infopath, void *pJson, BLE_MSG_EVENT_T *pMsgEvent)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    uint32 datalen = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pMsgEvent)
    {
        return -1;
    }
    
    if(0 != strcmp(BLE_TOPOC_RECVNOTIFY, infopath))
    {
        return -2;
    }

    memset(pMsgEvent, 0, sizeof(BLE_MSG_EVENT_T));
    
    jTemp = cJSON_GetObjectItem(pCJson, "port");
    if(NULL == jTemp || cJSON_Number != jTemp->type)
    {
        return -3;
    }
    pMsgEvent->port = jTemp->valueint;

    jTemp = cJSON_GetObjectItem(pCJson, "mac");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -4;
    }
    db_base64_to_data(jTemp->valuestring, pMsgEvent->MAC, 6, &datalen);
    if(6 != datalen)
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "data");
    if(NULL == jTemp || cJSON_String != jTemp->type || 0 == strlen(jTemp->valuestring))
    {
        return -6;
    }
    db_base64_to_data(jTemp->valuestring, pMsgEvent->msg, BLE_MSG_MAX_LEN, &datalen);
    pMsgEvent->len = datalen;
    
    return 0;
}

/**********************************************************************
* @name      : ble_msg_notify_package
* @brief     : 报文通知内容封装(JSON)
* @param[in] : uint32 token               消息序号
               BLE_MSG_EVENT_T *pMsgEvent 报文通知
* @param[out]: 
* @return    : JSON消息字符串
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
char *ble_msg_notify_package(uint32 token, BLE_MSG_EVENT_T *pMsgEvent)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    char   *pMsgStr = NULL;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return NULL;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "port", cJSON_CreateNumber(pMsgEvent->port));

    pMsgStr = db_data_to_base64(pMsgEvent->MAC, 6);
    if(NULL == pMsgStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "mac", cJSON_CreateString(pMsgStr));
    free(pMsgStr);
    pMsgStr = NULL;

    pMsgStr = db_data_to_base64(pMsgEvent->msg, pMsgEvent->len);
    if(NULL == pMsgStr)
    {
        goto end;
    }
    cJSON_AddItemToObject(root, "data", cJSON_CreateString(pMsgStr));

    pMsg = cJSON_Print(root);
    free(pMsgStr);
end:
    cJSON_Delete(root);
    return pMsg;
}

