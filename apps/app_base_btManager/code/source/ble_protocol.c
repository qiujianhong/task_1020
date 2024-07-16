/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP 国网蓝牙私有协议解析
* @date：    2021-8-13
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "ble_data.h"
#include "ble_protocol.h"
#include "ble_uart.h"

#ifdef PRODUCT_CCU
#define BLE_OPERATION_MAXTIME_MS  2000
#else
#define BLE_OPERATION_MAXTIME_MS  800
#endif

/**********************************************************************
* @name      : ble_protocol_msg_ckeck
* @brief     ：蓝牙协议报文合法性检查(从68H开始检查)
* @param[in] ：uint8 *msg                报文内容
               uint16 msglen             报文长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
int ble_protocol_msg_ckeck(uint8 *msg, uint16 msglen)
{
    uint16 i = 0;
    uint16 len = 0;
    uint8  temp = 0;
    
    if(NULL == msg || msglen < BLE_PROTOCOL_LEN_MIN)
    {
        return -1;
    }

    /* 检查首尾值 */
    if(0x68 != msg[0] || 0x16 != msg[msglen - 1])
    {
        return -2;
    }
    
    /* 检查数据域长度是否匹配 */
    memcpy(&len, msg + 1, 2);
    if(BLE_PROTOCOL_LEN_MIN + len != msglen)
    {
        return -3;
    }

    /* 检查帧起始符 */
    if(0x68 != msg[BLE_PROTOCOL_DATA_START])
    {
        return -4;
    }

    /* 检查校验 */
    len = msglen - 2;
    for(i = 0; i < len; i++)
    {
        temp += msg[i];
    }
    if(temp != msg[len])
    {
        return -5;
    }
    
    return 0;
}

/**********************************************************************
* @name      : ble_protocol_split
* @brief     ：蓝牙协议报文解析(起始符68开始)
* @param[in] ：uint8 *msg                报文内容
               uint16 msglen             报文长度
* @param[out]：BLE_PRODATA_T **ppProData 报文解析结构体
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
int ble_protocol_split(uint8 *msg, uint16 msglen, BLE_PRODATA_T **ppProData)
{
    int ret = 0;
    uint16 datalen = 0;
    uint8  temp = 0;
    BLE_PRODATA_T *ptProData = NULL;

    if(NULL == ppProData)
    {
        return -1;
    }

    /* 检查 */
    ret = ble_protocol_msg_ckeck(msg, msglen);
    if(0 != ret)
    {
        BLE_BUF_DEBUG(msg, msglen, "格式检测错误");
        return ret;
    }
    
    /* 长度计算 */
    memcpy(&datalen, msg + 1, 2);

    /* 申请内存 */
    ptProData = (BLE_PRODATA_T*)malloc(datalen + sizeof(BLE_PRODATA_T));
    if(NULL == ptProData)
    {
        return 1;
    }
    memset(ptProData, 0, datalen + sizeof(BLE_PRODATA_T));
    ptProData->datalen = datalen;

    /* 报文类型 */
    temp = msg[3];
    ptProData->type = (temp & 0x07);
    ptProData->bStatus = ((temp >> 6) & 0x01);
    ptProData->bAck = ((temp >> 7) & 0x01);

    /* 控制命令 */
    memcpy(&(ptProData->cmd), msg + 4, 4);

    /* 传输设备地址 */
    MEMCPY_R(ptProData->mac, msg + 8, 6);
    
    /* 拷贝内容 */
    memcpy(ptProData->data, msg + BLE_PROTOCOL_DATA_START + 1, datalen);

    *ppProData = ptProData;
    return 0;
}

/**********************************************************************
* @name      : ble_protocol_package
* @brief     ：蓝牙协议报文封装(前四个字节是FE前导符)
* @param[in] ：uint8 frametype           帧类型
               uint32 cmd                命令
               uint8 *mac                目的地址
               uint8 *pData              发送内容
               uint16 datalen            内容长度
* @param[out]：uint8 **ppFrame           报文内容
* @return    ：报文内容长度
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
uint16 ble_protocol_package(uint8 frametype, uint32 cmd, uint8 *mac, uint8 *pData, uint16 datalen, uint8 **ppFrame)
{
    uint16 len = 0;
    uint16 offset = 0;
    uint16 i = 0;
    uint8  temp = 0;
    uint8 *pMsg = 0;
    
    *ppFrame = NULL;
    /* 控制数据大小10K内 */
    if((NULL == pData && 0 != datalen) || NULL == ppFrame || datalen > 1024 * 10)
    {
        return 0;
    }

    len = BLE_PROTOCOL_LEN_MIN + datalen + 4;
    pMsg = (uint8*)malloc(len);
    if(NULL == pMsg)
    {
        return 0;
    }

    pMsg[offset++] = 0xFE;
    pMsg[offset++] = 0xFE;
    pMsg[offset++] = 0xFE;
    pMsg[offset++] = 0xFE;

    pMsg[offset++] = 0x68;
    
    memcpy(pMsg + offset, &datalen, 2);
    offset += 2;
    
    pMsg[offset++] = frametype;
    
    memcpy(pMsg + offset, &cmd, 4);
    offset += 4;
    
    MEMCPY_R(pMsg + offset, mac, 6);
    offset += 6;
    
    memset(pMsg + offset, 0, 4);
    offset += 4;
    
    pMsg[offset++] = 0x68;

    if(datalen > 0)
    {
        memcpy(pMsg + offset, pData, datalen);
        offset += datalen;
    }
    
    for(i = 4; i < offset; i++)
    {
        temp += pMsg[i];
    }
    pMsg[offset++] = temp;
    pMsg[offset++] = 0x16;

    *ppFrame = pMsg;
    return offset;
}

/**********************************************************************
* @name      : ble_set_oper
* @brief     ：蓝牙设置操作
* @param[in] ：int fd                    设备句柄
               uint32 cmd                命令
               uint8 *pSend              发送内容
               uint16 sendlen            发送长度
               uint16 buflen             接收缓存容量
* @param[out]：BLE_PRODATA_T **pProRet   设置结果
               uint8 *recvbuf            接收结果(主动上报、透传消息)
* @return    ：>=0 recvbuf新增长度; < 0 错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_set_operate_one(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, BLE_PRODATA_T **pProRet,
                       uint8 *recvbuf, uint16 buflen)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8 *pMsg = NULL;
    uint8  MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8  buf[10240] = {0};  //10K长度内部接收缓存
    int    retlen = 0;
    int    start = 0;
    uint16 offset = 0;
    BLE_PRODATA_T *pProData = NULL;
    
    *pProRet = NULL;
    
    if(fd < 0 || 0 == cmd || NULL == pSend || 0 == sendlen || NULL == pProRet || NULL == recvbuf || 0 == buflen)
    {
        BLE_FMT_DEBUG("蓝牙设置入参错误, cmd 0x%08X\n", cmd);
        return 0;
    }

    msglen = ble_protocol_package(BLE_PRO_SET_BT, cmd, MAC, pSend, sendlen, &pMsg);
    if(NULL == pMsg)
    {
        BLE_BUF_DEBUG(pSend, sendlen, "串口组帧失败, cmd 0x%08X", cmd);
        return 0;
    }
    
    /* 发送消息 */
    ret = ble_uart_send(fd, pMsg, msglen);
    BLE_BUF_DEBUG(pMsg, msglen, "串口发送(cmd = 0x%08X)%s", cmd, 0 == ret ? "成功" : "失败");
    free(pMsg);
    if(0 != ret)
    {
        return -1;
    }

    /* 串口接收 */
    retlen = ble_uart_recv(fd, buf, 10240, BLE_OPERATION_MAXTIME_MS, 200);
    if(retlen <= 0)
    {
        BLE_FMT_DEBUG("接收超时\n", cmd);
        return 0;
    }
    BLE_BUF_DEBUG(buf, retlen, "串口接收");

    pMsg = recvbuf;
    /* 判断接收内容 */
    while(start < retlen)
    {
        if(0x68 == buf[start])
        {
            memcpy(&msglen, buf + start + 1, 2);
            if(start + BLE_PROTOCOL_LEN_MIN + msglen <= retlen)
            {
                ret = ble_protocol_split(buf + start, BLE_PROTOCOL_LEN_MIN + msglen, &pProData);
                if (NULL != pProData)
                {
                    if(cmd == pProData->cmd && BLE_PRO_SET_BT == pProData->type &&
                       1 == pProData->bAck && 1 == pProData->bStatus)
                    {
                        break;
                    }
                    else
                    {
                        free(pProData);
                        pProData = NULL;
                    }
                }
            }
        }
        
        /* 拷贝 */
        if(offset < buflen)
        {
            recvbuf[offset++] = buf[start];
        }
        start++;
    }

    if (NULL == pProData)
    {
        BLE_FMT_DEBUG("报文解析错误，错误码[%d]\n", ret);
        return -2;
    }

    *pProRet = pProData;

    /* 剩余内容拷贝 */
    if(pProData)
    {
        start += BLE_PROTOCOL_LEN_MIN + pProData->datalen;
        if(start < retlen && offset < buflen)
        {
            msglen = MIN(buflen - offset, retlen - start);
            memcpy(recvbuf + offset, buf + start, msglen);
            offset += msglen;
        }
    }
    return offset;
}

/**********************************************************************
* @name      : ble_set_oper
* @brief     ：蓝牙设置操作
* @param[in] ：int fd                    设备句柄
               uint32 cmd                命令
               uint8 *pSend              发送内容
               uint16 sendlen            发送长度
               uint16 buflen             接收缓存容量
* @param[out]：BLE_PRODATA_T **pProRet   设置结果
               uint8 *recvbuf            接收结果(主动上报、透传消息)
* @return    ：>=0 recvbuf新增长度; < 0 错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_set_operate(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, BLE_PRODATA_T **pProRet,
                       uint8 *recvbuf, uint16 buflen)
{
    uint32 i = 0;
    int    len = -1;

    while (i < 3)
    {
        len = ble_set_operate_one(fd, cmd, pSend, sendlen, pProRet, recvbuf, buflen);
        if (len > 0)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return len;
}

/**********************************************************************
* @name      : ble_get_operate
* @brief     ：蓝牙设置操作
* @param[in] ：int fd                    设备句柄
               uint32 cmd                命令
               uint8 *pSend              发送内容
               uint16 sendlen            发送长度
               uint16 buflen             接收缓存容量
* @param[out]：BLE_PRODATA_T **pProRet   设置结果
               uint8 *recvbuf            接收结果(主动上报、透传消息)
* @return    ：>=0 recvbuf新增长度; < 0 错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_get_operate_one(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, BLE_PRODATA_T **pProRet, 
                       uint8 *recvbuf, uint16 buflen)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8 *pMsg = NULL;
    uint8  MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8  buf[10240] = {0};  //10K长度内部接收缓存
    int    retlen = 0;
    int    start = 0;
    uint16 offset = 0;
    BLE_PRODATA_T *pProData = NULL;
    
    *pProRet = NULL;
    
    if(fd < 0 || 0 == cmd || (NULL == pSend && 0 != sendlen)  || NULL == pProRet || NULL == recvbuf || 0 == buflen)
    {
        BLE_FMT_DEBUG("蓝牙获取入参错误, cmd 0x%08X\n", cmd);
        return 0;
    }

    msglen = ble_protocol_package(BLE_PRO_GET_BT, cmd, MAC, pSend, sendlen, &pMsg);
    if(NULL == pMsg)
    {
        BLE_BUF_DEBUG(pSend, sendlen, "串口组帧失败, cmd 0x%08X", cmd);
        return 0;
    }
    
    /* 发送消息 */
    ret = ble_uart_send(fd, pMsg, msglen);
    BLE_BUF_DEBUG(pMsg, msglen, "串口发送(cmd = 0x%08X)%s", cmd, 0 == ret ? "成功" : "失败");
    free(pMsg);
    if(0 != ret)
    {
        return -1;
    }

    /* 串口接收 */
    retlen = ble_uart_recv(fd, buf, 10240, BLE_OPERATION_MAXTIME_MS, 200);
    if(retlen <= 0)
    {
        BLE_FMT_DEBUG("接收超时\n", cmd);
        return 0;
    }
    BLE_BUF_DEBUG(buf, retlen, "串口接收");

    pMsg = recvbuf;
    /* 判断接收内容 */
    while(start < retlen)
    {
        if(0x68 == buf[start])
        {
            memcpy(&msglen, buf + start + 1, 2);
            if(start + BLE_PROTOCOL_LEN_MIN + msglen <= retlen)
            {
                ret = ble_protocol_split(buf + start, BLE_PROTOCOL_LEN_MIN + msglen, &pProData);
                if(NULL != pProData)
                {
                    if(cmd == pProData->cmd && BLE_PRO_GET_BT == pProData->type && 1 == pProData->bAck)
                    {
                        break;
                    }
                    else
                    {
                        free(pProData);
                        pProData = NULL;
                    }
                }
            }
        }
        
        /* 拷贝 */
        if(offset < buflen)
        {
            recvbuf[offset++] = buf[start];
        }
        start++;
    }
    
    if (NULL == pProData)
    {
        BLE_FMT_DEBUG("报文解析错误，错误码[%d]\n", ret);
        return -2;
    }

    *pProRet = pProData;

    /* 剩余内容拷贝 */
    if(pProData)
    {
        start += BLE_PROTOCOL_LEN_MIN + pProData->datalen;
        if(start < retlen && offset < buflen)
        {
            msglen = MIN(buflen - offset, retlen - start);
            memcpy(recvbuf + offset, buf + start, msglen);
            offset += msglen;
        }
    }
    return offset;
}

/**********************************************************************
* @name      : ble_get_operate
* @brief     ：蓝牙设置操作
* @param[in] ：int fd                    设备句柄
               uint32 cmd                命令
               uint8 *pSend              发送内容
               uint16 sendlen            发送长度
               uint16 buflen             接收缓存容量
* @param[out]：BLE_PRODATA_T **pProRet   设置结果
               uint8 *recvbuf            接收结果(主动上报、透传消息)
* @return    ：>=0 recvbuf新增长度; < 0 错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_get_operate(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen, BLE_PRODATA_T **pProRet, 
                       uint8 *recvbuf, uint16 buflen)
{
    uint32 i = 0;
    int    len = -1;

    while (i < 3)
    {
        len = ble_get_operate_one(fd, cmd, pSend, sendlen, pProRet, recvbuf, buflen);
        if (len > 0)
        {
            break;
        }
        else
        {
            i++;
            usleep(10000);
        }
    }

    return len;
}

/**********************************************************************
* @name      : ble_tran_send
* @brief     ：蓝牙透传消息
* @param[in] ：int fd                    设备句柄
               uint8 *MAC                对端地址
               uint8 *pSend              发送内容
               uint16 sendlen            发送长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_tran_send(int fd, uint8 *MAC, uint8 *pSend, uint16 sendlen)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8 *pMsg = NULL;
    
    if(fd < 0 || 0 == MAC || NULL == pSend || 0 == sendlen)
    {
        BLE_FMT_DEBUG("蓝牙透传入参错误\n");
        return 0;
    }

    msglen = ble_protocol_package(BLE_PRO_TO_OUT, 0, MAC, pSend, sendlen, &pMsg);
    if(NULL == pMsg)
    {
        BLE_BUF_DEBUG(pSend, sendlen, "串口组帧失败, 透传");
        return 0;
    }
    
    /* 发送消息 */
    ret = ble_uart_send(fd, pMsg, msglen);
    BLE_BUF_DEBUG(pMsg, msglen, "串口发送(透传%02X:%02X:%02X:%02X:%02X:%02X)%s", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4],
                                MAC[5], 0 == ret ? "成功" : "失败");
    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : ble_report_response
* @brief     ：蓝牙响应主动上报
* @param[in] ：int fd                    设备句柄
               uint32 cmd                命令
               uint8 *pSend              发送内容
               uint16 sendlen            发送长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_report_response(int fd, uint32 cmd, uint8 *pSend, uint16 sendlen)
{
    int    ret = 0;
    uint16 msglen = 0;
    uint8  type = BLE_PRO_REPORT;
    uint8  MAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8 *pMsg = NULL;
    
    if(fd < 0 || NULL == pSend || 0 == sendlen)
    {
        BLE_FMT_DEBUG("蓝牙上报应答入参错误\n");
        return 0;
    }

    msglen = ble_protocol_package(type + 0xC0, cmd, MAC, pSend, sendlen, &pMsg);
    if(NULL == pMsg)
    {
        BLE_BUF_DEBUG(pSend, sendlen, "串口组帧失败, 上报应答");
        return 0;
    }
    
    /* 发送消息 */
    ret = ble_uart_send(fd, pMsg, msglen);
    BLE_BUF_DEBUG(pMsg, msglen, "串口发送(上报应答)%s", 0 == ret ? "成功" : "失败");
    free(pMsg);
    return ret;

}


