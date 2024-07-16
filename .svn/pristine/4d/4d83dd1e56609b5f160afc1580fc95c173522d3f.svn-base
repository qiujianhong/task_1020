/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程APDU解析
* @date：    2019-12-5
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appcrc16.h"
#include "db_api.h"
#include "framecomm.h"
#include "appcomm.h"
#include "msgSecApp.h"

#include "ssal.h"

uint8 g_SSAL_Flsg = 0;

/**********************************************************************
* @name      : ssal_set_status
* @brief     ：设置SSAL状态
* @param[in] ：uint8 status            状态
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2021-9-2
* @Update    :
**********************************************************************/
void ssal_set_status(uint8 status)
{
    if(status <= 1)
    {
        g_SSAL_Flsg = status;
    }
}

/**********************************************************************
* @name      : ssal_get_status
* @brief     ：获取SSAL状态
* @param[in] ：
* @param[out]：
* @return    ：状态
* @Create    : 
* @Date      ：2021-9-2
* @Update    :
**********************************************************************/
uint8 ssal_get_status()
{
    return g_SSAL_Flsg;
}

/**********************************************************************
* @name      : ssal_get_dbstatus
* @brief     ：获取数据中心ssal状态
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-9-2
* @Update    :
**********************************************************************/
int ssal_get_dbstatus(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    int         ret = 0;
    uint32      readlen = 0;
    NOMAL_OAD_T tOAD;
    uint8       ssal = 0;

    if(0xF1010400 != value)
    {
        return -1;
    }
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));

    tOAD.oad.value = value;
    ret = db_read_nomal(clientid, &tOAD, sizeof(uint8), &ssal, &readlen);
    if(0 == ret && readlen == sizeof(uint8))
    {
        ssal_set_status(ssal);
        APP698_FMT_DEBUG("SSAL new status %d\n", ssal);
    }
    
    return 0;
}

/**********************************************************************
* @name      : ssal_msg_split
* @brief     ：SSAL报文解析
* @param[in] ：uint8 *pMsg             报文内容
               uint16 msgLen           报文长度
* @param[out]：SSAL_HEAD_T *pSsalHead  报文结构
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2021-9-1
* @Update    :
**********************************************************************/
int ssal_msg_split(uint8 *pMsg, uint16 msglen, SSAL_HEAD_T *pSsalHead)
{
    uint16 len  = 0;
    uint16 offset = 0;
    uint8  temp = 0;
    //uint16 CRC = 0;
    //uint16 CS = 0;
    
    if(NULL == pMsg || msglen <= 12 || NULL == pSsalHead)
    {
        return SSAL_ERR_INPUT;
    }

    /* 简单判断 */
    if(0x98 != pMsg[0] || 0x16 != pMsg[msglen - 1])
    {
        return SSAL_ERR_UNKNOW;
    }

    memset(pSsalHead, 0, sizeof(SSAL_HEAD_T));
    /* 长度 */
    offset++;
    memcpy(&len, pMsg + offset, 2);
    offset += 2;

    /* 长度判断 */
    if(len + 4 != msglen)
    {
        return SSAL_ERR_MSG;
    }

    /* 帧序号 */
    memcpy(&(pSsalHead->seq), pMsg + offset, 2);
    offset += 2;

    /* 控制码 */
    memcpy(&(pSsalHead->c), pMsg + offset, 2);
    offset += 2;

    /* 功能码 */
    if(pSsalHead->c.FCflag)
    {
        temp = pMsg[offset++];
        pSsalHead->fc.dir = ((temp >> 7) & 0x01);
        pSsalHead->fc.prm = ((temp >> 6) & 0x01);
        pSsalHead->fc.consult = ((temp >> 4) & 0x01);
        pSsalHead->fc.ssaltype = (temp & 0x0F);
    }
    
    /* SSAL协议版本 */
    if(pSsalHead->c.SVflag)
    {
        temp = pMsg[offset++];
        pSsalHead->sv.cipherType = (temp & 0x0F);

        temp = pMsg[offset++];
        pSsalHead->sv.version = temp;
    }

    /* 设备地址类型DAT */
    if(pSsalHead->c.DATflag)
    {
        temp = pMsg[offset++];
        pSsalHead->dat.proType = (temp & 0x1F);

        temp = pMsg[offset++];
        pSsalHead->dat.addrType = (temp & 0x07);
        pSsalHead->dat.devType = ((temp >> 3) & 0x1F);
    }

    /* 设备地址 */
    if(pSsalHead->c.DAflag)
    {
        temp = pMsg[offset++];
        pSsalHead->da.mslen = ((temp >> 4) & 0x07);
        pSsalHead->da.termlen = (temp & 0x0F) + 1;

        memcpy_r(pSsalHead->da.DP, pMsg + offset, pSsalHead->da.termlen);
        offset += pSsalHead->da.termlen;

        if( pSsalHead->da.mslen > 0)
        {
            memcpy(pSsalHead->da.MP, pMsg + offset, pSsalHead->da.mslen);
            offset += pSsalHead->da.mslen;
        }
    }

    /* 原地址 */
    if(pSsalHead->c.SAflag)
    {
        temp = pMsg[offset++];
        pSsalHead->sa.sourcelen = (temp & 0x1F);
        pSsalHead->sa.portlen = ((temp >> 5) & 0x07);
        
        memcpy(pSsalHead->sa.SA, pMsg + offset, MIN(16, pSsalHead->sa.sourcelen));
        offset += pSsalHead->sa.sourcelen;
        
        memcpy(pSsalHead->sa.PORT, pMsg + offset, MIN(2, pSsalHead->sa.portlen));
        offset += pSsalHead->sa.portlen;
    }

    /* 目的地址 */
    if(pSsalHead->c.TAflag)
    {
        temp = pMsg[offset++];
        pSsalHead->ta.destlen = (temp & 0x1F);
        pSsalHead->ta.portlen = ((temp >> 5) & 0x07);
        
        memcpy(pSsalHead->ta.TA, pMsg + offset, MIN(16, pSsalHead->ta.destlen));
        offset += pSsalHead->ta.destlen;
        
        memcpy(pSsalHead->ta.PORT, pMsg + offset, MIN(2, pSsalHead->ta.portlen));
        offset += pSsalHead->ta.portlen;
    }

    /* 通信信息 */
    if(pSsalHead->c.CIflag)
    {
        temp = pMsg[offset++];
        pSsalHead->ci.type = ((temp >> 4) & 0x0F);
        pSsalHead->ci.len = (temp & 0x0F);

        memcpy(pSsalHead->ci.CIC, pMsg + offset, pSsalHead->ci.len);
        offset += pSsalHead->ci.len;
    }

    /* 时间标签 */
    if(pSsalHead->c.TPflag)
    {
        memcpy(&(pSsalHead->tp), pMsg + offset, 7);
        offset += 7;
    }

    /* 网关地址 */
    if(pSsalHead->c.GAflag)
    {
        temp = pMsg[offset++];
        pSsalHead->ga.GAL = temp;

        memcpy(pSsalHead->ga.GAC, pMsg + offset, pSsalHead->ga.GAL);
        offset += pSsalHead->ga.GAL;
    }

    /* 校验判断 */
    /*CRC = app_crc16_get(pMsg + 1, offset);
    CS = pMsg[offset] + pMsg[offset + 1] * 0x0100;
    if(CRC != CS)
    {
        return SSAL_ERR_CS;
    }*/
    offset += 2;
    
    /* 数据长度 */
    memcpy(&(pSsalHead->datalen), pMsg + offset, 2);
    offset += 2;

    /* 返回信息 */
    if(0 == pSsalHead->fc.prm)
    {
        memcpy(&(pSsalHead->retcode), pMsg + offset, 2);
        offset += 2;
    }

    /* 数据 */
    if(pSsalHead->datalen > 0)
    {
        pSsalHead->data= pMsg + offset;
        offset += pSsalHead->datalen;
    }
    
    /* 校验判断 */
    /*CRC = app_crc16_get(pMsg + 1, offset);
    CS = pMsg[offset] + pMsg[offset + 1] * 0x0100;
    if(CRC != CS)
    {
        return SSAL_ERR_CS;
    }*/
    offset += 2;
    
    return 0;
}

/**********************************************************************
* @name      : ssal_msg_package
* @brief     ：SSAL报文封装
* @param[in] ：SSAL_HEAD_T *pSsalHead  报文结构
* @param[out]：uint16 *pMsglen         消息长度
* @return    ：报文内容
* @Create    : 
* @Date      ：2021-9-1
* @Update    :
**********************************************************************/
uint8 *ssal_msg_package(SSAL_HEAD_T *pSsalHead, uint16 *pMsglen)
{
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    uint16 offset = 0;
    uint16 CRC = 0;
    
    if(NULL == pSsalHead || NULL == pMsglen)
    {
        return NULL;
    }

    /* 头 + L + SEQ + C */
    msglen = 1 + 2 + 2 + 2;

    /* 控制数据长度 */
    msglen += (1 == pSsalHead->c.FCflag) ? 1 : 0;
    msglen += (1 == pSsalHead->c.SVflag) ? 2 : 0;
    msglen += (1 == pSsalHead->c.DATflag) ? 2 : 0;
    msglen += (1 == pSsalHead->c.DAflag) ? (1 + pSsalHead->da.mslen + pSsalHead->da.termlen) : 0;
    msglen += (1 == pSsalHead->c.SAflag) ? (1 + pSsalHead->sa.sourcelen + pSsalHead->sa.portlen) : 0;
    msglen += (1 == pSsalHead->c.TAflag) ? (1 + pSsalHead->ta.destlen + pSsalHead->ta.portlen) : 0;
    msglen += (1 == pSsalHead->c.CIflag) ? (1 + pSsalHead->ci.len) : 0;
    msglen += (1 == pSsalHead->c.TPflag) ? 7 : 0;
    msglen += (1 == pSsalHead->c.GAflag) ? (1 + pSsalHead->ga.GAL) : 0;

    /* 帧头校验 */
    msglen += 2;
    /* 数据 */
    msglen += 2;
    msglen += 0 == pSsalHead->fc.prm ? 2 : 0;
    msglen += pSsalHead->datalen;
    
    /* 尾 */
    msglen += 2 + 1;

    pMsg = (uint8 *)malloc(msglen);
    if(NULL == pMsg)
    {
        return NULL;
    }
    memset(pMsg, 0, msglen);

    /* 填入内容 */
    pMsg[offset++] = 0x98;
    pMsg[offset++] = (msglen - 4)%256;
    pMsg[offset++] = (msglen - 4)/256;
    
    memcpy(pMsg + offset, &(pSsalHead->seq), 2);
    offset += 2;

    memcpy(pMsg + offset, &(pSsalHead->c), 2);
    offset += 2;

    /* 功能码 */
    if(1 == pSsalHead->c.FCflag)
    {
        pMsg[offset++] = (pSsalHead->fc.dir << 7) + (pSsalHead->fc.prm << 6) + (pSsalHead->fc.consult << 4) + 
                         pSsalHead->fc.ssaltype;
    }
    /* SSAL协议版本 */
    if(1 == pSsalHead->c.SVflag)
    {
        pMsg[offset++] = pSsalHead->sv.cipherType;
        pMsg[offset++] = pSsalHead->sv.version;
    }
    /* 设备地址类型 */
    if(1 == pSsalHead->c.DATflag)
    {
        pMsg[offset++] = pSsalHead->dat.proType;
        pMsg[offset++] = pSsalHead->dat.addrType + (pSsalHead->dat.devType << 3);
    }
    /* 设备地址 */
    if(1 == pSsalHead->c.DAflag)
    {
        pMsg[offset++] = pSsalHead->da.termlen -1  + (pSsalHead->da.mslen << 4);
        memcpy_r(pMsg + offset, pSsalHead->da.DP, pSsalHead->da.termlen);
        offset += pSsalHead->da.termlen;
        
        if(pSsalHead->da.mslen > 0)
        {
            memcpy(pMsg + offset, pSsalHead->da.MP, pSsalHead->da.mslen);
            offset += pSsalHead->da.mslen;
        }
    }
    /* 源地址 */
    if(1 == pSsalHead->c.SAflag)
    {
        pMsg[offset++] = pSsalHead->sa.sourcelen + (pSsalHead->sa.portlen << 5);
        memcpy(pMsg + offset, pSsalHead->sa.SA, pSsalHead->sa.sourcelen);
        offset += pSsalHead->sa.sourcelen;
        memcpy(pMsg + offset, pSsalHead->sa.PORT, pSsalHead->sa.portlen);
        offset += pSsalHead->sa.portlen;
    }
    /* 目的地址 */
    if(1 == pSsalHead->c.TAflag)
    {
        pMsg[offset++] = pSsalHead->ta.destlen + (pSsalHead->ta.portlen << 5);
        memcpy(pMsg + offset, pSsalHead->ta.TA, pSsalHead->ta.destlen);
        offset += pSsalHead->ta.destlen;
        memcpy(pMsg + offset, pSsalHead->ta.PORT, pSsalHead->ta.portlen);
        offset += pSsalHead->ta.portlen;
    }
    /* 通信信息 */
    if(1 == pSsalHead->c.CIflag)
    {
        pMsg[offset++] = pSsalHead->ci.len + (pSsalHead->ci.type << 4);
        memcpy(pMsg + offset, pSsalHead->ci.CIC, pSsalHead->ci.type);
        offset += pSsalHead->ci.type;
    }
    /* 时间标签 */
    if(1 == pSsalHead->c.TPflag)
    {
        pMsg[offset++] = pSsalHead->tp.yyH;
        pMsg[offset++] = pSsalHead->tp.yyL;
        pMsg[offset++] = pSsalHead->tp.mon;
        pMsg[offset++] = pSsalHead->tp.day;
        pMsg[offset++] = pSsalHead->tp.hour;
        pMsg[offset++] = pSsalHead->tp.min;
        pMsg[offset++] = pSsalHead->tp.sec;
    }
    /* 网关地址 */
    if(1 == pSsalHead->c.GAflag)
    {
        pMsg[offset++] = pSsalHead->ga.GAL;
        memcpy(pMsg + offset, pSsalHead->ga.GAC, pSsalHead->ga.GAL);
        offset += pSsalHead->ga.GAL;
    }
    
    /* 帧头校验 */
    CRC = app_crc16_get(pMsg + 1, offset - 1);
    pMsg[offset++] = (CRC & 0xFF);
    pMsg[offset++] = ((CRC >> 8) & 0xFF);

    /* 长度 */
    pMsg[offset++] = (pSsalHead->datalen & 0xFF);
    pMsg[offset++] = ((pSsalHead->datalen >> 8) & 0xFF);

    /* 返回码 */
    if(0 == pSsalHead->fc.prm)
    {
        pMsg[offset++] = (pSsalHead->retcode & 0xFF);
        pMsg[offset++] = ((pSsalHead->retcode >> 8) & 0xFF);
    }

    /* 数据 */
    if(pSsalHead->datalen > 0)
    {
        memcpy(pMsg + offset, pSsalHead->data, pSsalHead->datalen);
        offset += pSsalHead->datalen;
    }

    /* 校验 */
    CRC = app_crc16_get(pMsg + 1, offset - 1);
    pMsg[offset++] = (CRC & 0xFF);
    pMsg[offset++] = ((CRC >> 8) & 0xFF);

    pMsg[offset++] = 0x16;
    
    *pMsglen = msglen;
    return pMsg;
}

/**********************************************************************
* @name      : app_get_recvmsg
* @brief     ：获取缓存队列中第一个报文
* @param[in] ：uint8  id              通道id
               uint8  lockuser        加锁用户
* @param[out]：SSAL_HEAD_T *pSsalHead 解析的消息头
               uint16 *pStart         消息起始位置
               uint16 *pLen           消息有效长度
* @return    ：0-存在消息/错误码
* @Create    : 
* @Date      ：2021-9-3
* @Update    : 
**********************************************************************/
int ssal_find_first_msg(uint8 id, SSAL_HEAD_T *pSsalHead, uint16 *pStart, uint16 *pLen, uint8 lockuser)
{
    uint16 i = 0;
    uint8 *pTemp = NULL;
    uint16 len = 0;
    struct timespec   curtime = {0};
    CHANNEL_CACHE_T  *ptChannel = NULL;

    if(id >= MAX_CHANNEL_NUM || NULL == pSsalHead || NULL == pStart || NULL == pLen)
    {
        return SSAL_ERR_INPUT;
    }

    *pStart = 0;
    *pLen = 0;

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    ptChannel = app_get_channel(id);
    if(NULL == ptChannel)
    {
        return SSAL_ERR_INPUT;
    }
    if(FALSE == ptChannel->bOpen || 0 == ptChannel->totalLen)
    {
        ptChannel->checkTimeOK = curtime.tv_sec;
        return SSAL_ERR_INPUT;
    }
    
    pthread_mutex_lock(&(ptChannel->fRecv));
    ptChannel->sendLockUser = lockuser;

    if(ptChannel->totalLen > APP_RECV_IPC_MAXLEN)
    {
        ptChannel->totalLen = APP_RECV_IPC_MAXLEN;
    }

    pTemp = ptChannel->recv;
    /* 去掉开头不是0x98或者长度不正确的消息内容 */
    while(i < ptChannel->totalLen)
    {
        *pStart = i;
        if(0x98 == pTemp[i])
        {
            if(i + 12 >= ptChannel->totalLen)
            {
                break;
            }
            memcpy(&len, pTemp + i + 1, 2);
            len += 4;
            /* 长度异常 */
            if(i + len > ptChannel->totalLen)
            {
                break;
            }
            /* 结尾存在16 */
            if(0x16 == pTemp[i + len -1])
            {
                if(0 == ssal_msg_split(pTemp + i, len, pSsalHead))
                {
                    pthread_mutex_unlock(&(ptChannel->fRecv));
                    ptChannel->sendLockUser = APP698_NO_LOCK;
                    *pLen = len;
                    ptChannel->checkTimeOK = curtime.tv_sec;
                    return SSAL_SUCCESS;
                }
            }
        }
        i++;
    }
    
    pthread_mutex_unlock(&(ptChannel->fRecv));
    ptChannel->sendLockUser = APP698_NO_LOCK;

    return SSAL_ERR_OTHER;
}

/**********************************************************************
* @name      : ssal_remove_recvmsg
* @brief     ：清除缓存队列中报文数据
* @param[in] ：uint8  id             通道id
               uint16 start          起始位置
               uint16 msglen         消息长度
               uint8 lockuser        加锁用户
* @param[out]：
* @return    ：报文长度
* @Create    : 
* @Date      ：2021-9-4
* @Update    : 
**********************************************************************/
void ssal_remove_recvmsg(uint8 id, uint16 start, uint16 msglen, uint8 lockuser)
{
    struct timespec curtime = {0};
    CHANNEL_CACHE_T *ptChannel = NULL;

    ptChannel = app_get_channel(id);
    if(NULL == ptChannel || 0 == ptChannel->totalLen)
    {
        return;
    }
    
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    pthread_mutex_lock(&(ptChannel->fRecv));
    ptChannel->sendLockUser = lockuser;

    if(start + msglen > 0)
    {
        if(start + msglen >= ptChannel->totalLen)
        {
            memset(ptChannel->recv, 0, ptChannel->totalLen);
            ptChannel->totalLen = 0;
        }
        else
        {
            memmove(ptChannel->recv, ptChannel->recv + start + msglen, ptChannel->totalLen - start - msglen);
            ptChannel->totalLen -= start;
            ptChannel->totalLen -= msglen;
            memset(ptChannel->recv + ptChannel->totalLen, 0, APP_RECV_IPC_MAXLEN - ptChannel->totalLen);
        }
    }
    else if(0x98 == ptChannel->recv[0])
    {
        if(ptChannel->checkTimeOK + 10 < curtime.tv_sec)
        {
            /* 超过10s检查失败 */
            ptChannel->checkTimeOK = curtime.tv_sec;
            ptChannel->recv[0] = 0;
        }
    }

    pthread_mutex_unlock(&(ptChannel->fRecv));
    ptChannel->sendLockUser = APP698_NO_LOCK;
}

/**********************************************************************
* @name      : ssal_check_support
* @brief     ：判断SSAL是否支持
* @param[in] ：SSAL_HEAD_T *pSsalHead 判断是否支持改SSAL报文
* @param[out]：uint16 *pRetcode       错误码
* @return    ：0-继续处理/其他错误码SSAL_CHECK_E
* @Create    : 
* @Date      ：2021-9-4
* @Update    : 
**********************************************************************/
int ssal_check_support(SSAL_HEAD_T *pSsalHead, uint16 *pRetcode)
{
    uint8         i = 0;
    uint8         len = 0;
    uint8        *pAddr = NULL;
    OOP_GRPADDR_T tGrpAddr;
    
    if(0 == pSsalHead->c.FCflag || 0 == pSsalHead->c.SVflag || 0 == pSsalHead->c.DATflag || 0 == pSsalHead->c.DAflag)
    {
        APP698_FMT_DEBUG("Err control func, %d%d%d%d\n", pSsalHead->c.FCflag, pSsalHead->c.SVflag, pSsalHead->c.DATflag,
                                                         pSsalHead->c.DAflag);
        *pRetcode = SAL_RECVINFO_ERROR_DATA;
        return SSAL_CHECK_ERROR;
    }
    
    /* 过滤自发自收 */
	if (0 != pSsalHead->fc.dir) //滤除自发自收帧
	{
		return SSAL_CHECK_DROP;
	}
	/* 判断设备类型 协议版本号 */
	if (pSsalHead->dat.devType > 31 || pSsalHead->dat.addrType > 2)
	{
		*pRetcode = SAL_RECVINFO_ERROR_DEVICE_TYPE;
		return SSAL_CHECK_ERROR;
	}
	/* 不是698协议 */
	if (1 != pSsalHead->dat.proType) //不是698
	{
		*pRetcode = SAL_RECVINFO_ERROR_SAL_SV;
		return SSAL_CHECK_ERROR;
	}

    /* 主站发来的从动报文，如响应终端的主动上报 */
    if(0 == pSsalHead->fc.dir && 0 == pSsalHead->fc.prm)
    {
        if(0 != pSsalHead->retcode)
        {
            return SSAL_CHECK_DROP;
        }
    }

	/* 判断地址 */
	len = app_get_localaddr(&pAddr);

	if(0 == pSsalHead->dat.addrType)
    {
        if(len == pSsalHead->da.termlen && 0 == memcmp(pSsalHead->da.DP, pAddr, len))
        {
            return SSAL_CHECK_OK;
        }
    }
    else if(1 == pSsalHead->dat.addrType)
    {
        app_get_groupaddr(&tGrpAddr);
        for(i = 0; i < tGrpAddr.nNum; i++)
        {
            if(tGrpAddr.sa[i].len == pSsalHead->da.termlen && 0 == memcmp(pSsalHead->da.DP, tGrpAddr.sa[i].add, len))
            {
                return SSAL_CHECK_OK;
            }
        }
    }
    else if(2 == pSsalHead->dat.addrType)
    {
        if(1 == pSsalHead->da.termlen || 0xaa != pSsalHead->da.DP[0])
        {
            return SSAL_CHECK_OK;
        }
    }

    return SSAL_CHECK_DROP;
}

/**********************************************************************
* @name      : ssal_recv_to_send_head
* @brief     ：接收消息疯转发送消息
* @param[in] ：SSAL_HEAD_T *pSsalHead 判断是否支持改SSAL报文
* @param[out]：uint16 *pRetcode       错误码
* @return    ：0-继续处理/其他错误码SSAL_CHECK_E
* @Create    : 
* @Date      ：2021-9-4
* @Update    : 
**********************************************************************/
void ssal_recv_to_send_head(SSAL_HEAD_T *pSsalHead, SSAL_HEAD_T *pSsalSend)
{
    uint8         len = 0;
    uint8        *pAddr = NULL;

    if(NULL == pSsalHead || NULL == pSsalSend)
    {
        return;
    }

    pSsalSend->seq = pSsalHead->seq;
    pSsalSend->c = pSsalHead->c;
    pSsalSend->c.SAflag = pSsalHead->c.TAflag;
    pSsalSend->c.TAflag = pSsalHead->c.SAflag;
    
    pSsalSend->fc.dir = 1;
    pSsalSend->fc.prm = 0;
    pSsalSend->fc.consult = pSsalHead->fc.consult;
    pSsalSend->fc.ssaltype = pSsalHead->fc.ssaltype;

    pSsalSend->sv.version = pSsalHead->sv.version;
    pSsalSend->sv.cipherType = pSsalHead->sv.cipherType;

    pSsalSend->dat.devType = SSAL_DEV_GATHER;
    pSsalSend->dat.addrType = 0;
    pSsalSend->dat.proType = 1;

    pSsalSend->da.mslen = pSsalHead->da.mslen;
    if(pSsalSend->da.mslen > 0)
    {
        memcpy(pSsalSend->da.MP, pSsalHead->da.MP, pSsalHead->da.mslen);
    }
    
    len = app_get_localaddr(&pAddr);
    pSsalSend->da.termlen = len;
    memcpy(pSsalSend->da.DP, pAddr, len);

    //源地址-目的地址交换
    memcpy(&pSsalSend->ta, &pSsalHead->sa, sizeof(SSAL_SA_T));
    memcpy(&pSsalSend->sa, &pSsalHead->ta, sizeof(SSAL_TA_T));
    memcpy(&pSsalSend->ci, &pSsalHead->ci, sizeof(SSAL_CI_T));
    memcpy(&pSsalSend->ga, &pSsalHead->ga, sizeof(SSAL_GA_T));

    if(1 == pSsalSend->c.TPflag)
    {
        struct tm ptm;
        time_t now_t;

        now_t = time(NULL);
        LOCALTIME_R(&now_t, &ptm);

        pSsalSend->tp.yyH = ((ptm.tm_year+1900)&0xFF00)>>8;
        pSsalSend->tp.yyL = (uint8)(ptm.tm_year+1900);
        pSsalSend->tp.mon = (ptm.tm_mon+1);
        pSsalSend->tp.day = (ptm.tm_mday);
        pSsalSend->tp.hour = (ptm.tm_hour);
        pSsalSend->tp.min = (ptm.tm_min);
        pSsalSend->tp.sec = (ptm.tm_sec);
    }
}

/**********************************************************************
* @name      : ssal_linkrequest_package
* @brief     ：链路协商报文组织
* @param[in] ：uint32 ip              主站地址
               uint8 *link698         698链路协商报文
               uint16 len698          链路协商报文
               uint16 linkmax         最大缓存
* @param[out]：uint8 *ssallink        ssal报文
* @return    ：ssal报文长度
* @Create    : 
* @Date      ：2021-9-6
* @Update    : 
**********************************************************************/
uint16 ssal_linkrequest_package(uint32 ip, uint8 *link698, uint16 len698, uint8 *ssallink, uint16 linkmax)
{
    SEC_DATA_T  tSeamSend;
    SEC_DATA_T  tSeamRecv;
    struct      tm ptm;
    time_t      now_t;
    uint8       addrlen = 0;
    uint8      *pAddr = NULL;
    uint8       recvbuf[1000] = {0};
    int         ret = 0;
    
    now_t = time(NULL);
    LOCALTIME_R(&now_t, &ptm);

    /* SSAL报文处理-先解密 */
    memset(&tSeamSend, 0, sizeof(SEC_DATA_T));
    memset(&tSeamRecv, 0, sizeof(SEC_DATA_T));

    tSeamSend.dataType = 1;
    tSeamSend.cryptType = 0;
    tSeamSend.appType = 1;

    tSeamSend.time[0] = (((ptm.tm_year + 1900) & 0xFF00) >> 8);
    tSeamSend.time[1] = (uint8)(ptm.tm_year + 1900);
    tSeamSend.time[2] = ptm.tm_mon + 1;
    tSeamSend.time[3] = ptm.tm_mday;
    tSeamSend.time[4] = ptm.tm_hour;
    tSeamSend.time[5] = ptm.tm_min;
    tSeamSend.time[6] = ptm.tm_sec;
    
    addrlen = app_get_localaddr(&pAddr);
    tSeamSend.devAddrLen = addrlen;
    memcpy(tSeamSend.devAddr, pAddr, addrlen);
    memcpy(tSeamSend.dstIp, &ip, 4);
    tSeamSend.policy = 0;
    tSeamSend.lenData = len698;
    tSeamSend.data = link698;

    tSeamRecv.data = recvbuf;
    ret = sec_ssal_en_decryption(&tSeamSend, &tSeamRecv);
    if(0 != ret)
    {
        APP698_BUF_DEBUG(link698, len698, "Link msg to ssal failed %d\n", ret);
        return 0;
    }

    memcpy(ssallink, recvbuf, MIN(linkmax, tSeamRecv.lenData));

    return MIN(linkmax, tSeamRecv.lenData);
}

/**********************************************************************
* @name      : ssal_commdata_package
* @brief     ：业务报文组织
* @param[in] ：uint32 ip              主站地址
               uint8 *frame698        698报文
               uint16 len698          报文长度
               uint16 framemax        最大缓存
* @param[out]：uint8 *ssalframe       ssal报文
* @return    ：ssal报文长度
* @Create    : 
* @Date      ：2021-9-23
* @Update    : 
**********************************************************************/
uint16 ssal_commdata_package(uint32 ip, uint8 *frame698, uint16 len698, uint8 *ssalframe, uint16 framemax)
{
    SEC_DATA_T  tSeamSend;
    SEC_DATA_T  tSeamRecv;
    struct      tm ptm;
    time_t      now_t;
    uint8       addrlen = 0;
    uint8      *pAddr = NULL;
    uint8       recvbuf[4096] = {0};
    int         ret = 0;
    
    now_t = time(NULL);
    LOCALTIME_R(&now_t, &ptm);

    /* SSAL报文处理-先解密 */
    memset(&tSeamSend, 0, sizeof(SEC_DATA_T));
    memset(&tSeamRecv, 0, sizeof(SEC_DATA_T));

    tSeamSend.dataType = 1;
    tSeamSend.cryptType = 0;
    tSeamSend.appType = 0;

    tSeamSend.time[0] = (((ptm.tm_year + 1900) & 0xFF00) >> 8);
    tSeamSend.time[1] = (uint8)(ptm.tm_year + 1900);
    tSeamSend.time[2] = ptm.tm_mon + 1;
    tSeamSend.time[3] = ptm.tm_mday;
    tSeamSend.time[4] = ptm.tm_hour;
    tSeamSend.time[5] = ptm.tm_min;
    tSeamSend.time[6] = ptm.tm_sec;
    
    addrlen = app_get_localaddr(&pAddr);
    tSeamSend.devAddrLen = addrlen;
    memcpy(tSeamSend.devAddr, pAddr, addrlen);
    memcpy(tSeamSend.dstIp, &ip, 4);
    tSeamSend.policy = 0;
    tSeamSend.lenData = len698;
    tSeamSend.data = frame698;

    tSeamRecv.data = recvbuf;
    ret = sec_ssal_en_decryption(&tSeamSend, &tSeamRecv);
    if(0 != ret)
    {
        APP698_BUF_DEBUG(frame698, len698, "Link msg to ssal failed %d\n", ret);
        return 0;
    }

    memcpy(ssalframe, recvbuf, MIN(framemax, tSeamRecv.lenData));

    return MIN(framemax, tSeamRecv.lenData);
}




