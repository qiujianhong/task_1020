/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   主应用调度进程698规约解析
* @date：    2019-12-4
* @history： 
*********************************************************************
*/
#include "baseCommon.h"

#include "threadpool.h"
#include "appcomm.h"
#include "protocol698.h"
#include "apducomm.h"
#include "framecomm.h"
#include "commClass.h"
#include "ssal.h"
#include "msgSecApp.h"


#define PROTOCOL_SEPARA_PACKAGE_MAXLEN   64
#define SEPARATE_PROC_RESULT_END         2   //处理结束
#define SEPARATE_PROC_RESULT_NEXTTIME    1   //下次调度处理
#define SEPARATE_PROC_RESULT_GOON        0   //继续处理

threadpool_t *g_Pool = NULL;
#define CHECK_CACHE_APDU(_type)  \
    (SERVERID_GETREQUEST == (_type) || SERVERID_SETREQUEST == (_type) || SERVERID_ACTIONREQUEST == (_type) || \
     SERVERID_REPORTRESPONSE == (_type) || SERVERID_PROXYREQUEST == (_type))

#define CHECK_CACHE_APDU_1(_apdu) \
    ((SERVERID_GETREQUEST == *(_apdu) && 5 != *(_apdu + 1)) || SERVERID_SETREQUEST == *(_apdu) || \
      SERVERID_ACTIONREQUEST == *(_apdu) || SERVERID_REPORTRESPONSE == *(_apdu) || SERVERID_PROXYREQUEST == *(_apdu))
extern uint8 gdelay;

/**********************************************************************
* @name      : protocol_sepa_ack_package
* @brief     ：封装链路分帧确认帧
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        帧头
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
void protocol_sepa_ack_package(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame)
{
    FRAME_HEAD_T tFrame;
    uint8 *pAddr = NULL;
    uint8  len = 0;
    uint16 msglen = 0;
    uint8  uMsgBuf[PROTOCOL_SEPARA_PACKAGE_MAXLEN] = {0};
    int    ret = 0;

    memcpy(&tFrame, ptFrame, sizeof(FRAME_HEAD_T));
    
    tFrame.dir = 1 - tFrame.dir;
    tFrame.separate = 1;
    tFrame.tSepara.type = LINK_SEPARATE_ACK;
    tFrame.apduLen = 0;
    tFrame.pApdu = NULL;
    if(0 != tFrame.sAddr.type)
    {
        len = app_get_localaddr(&pAddr);
        if(0 != len)
        {
            tFrame.sAddr.len = len;
            memcpy(tFrame.sAddr.addr, pAddr, len);
            tFrame.sAddr.type = 0;
        }
    }
    tFrame.headlen = APUD_COUNT_FRAME_LEN(tFrame.sAddr.len, tFrame.sAddr.logicAddr);
    tFrame.msglen = tFrame.headlen + 4;
    msglen = app_frame_to_package(&tFrame, uMsgBuf, PROTOCOL_SEPARA_PACKAGE_MAXLEN, TRUE);
    if(msglen > 0)
    {
        /* 直接发送 */
        if(1 == ssal_get_status() && ptChannel->tChannel.ip >= CHL_GPRS_1)
        {
            msglen = ssal_commdata_package(ptChannel->tChannel.ip, uMsgBuf, msglen, uMsgBuf, 128);
        }
        ret = send_emergency_ipcmsg(ptChannel->id, PRO_DATA_TRANSMIT, uMsgBuf, msglen);
        if(0 == ret)
        {
            APP698_BUF_DEBUG(uMsgBuf, msglen, "Channel %d send filed %d\n", ptChannel->id, ret);
        }
    }
}

/**********************************************************************
* @name      : protocol_link_separate
* @brief     ：698规约链路分帧处理
* @param[in] ：CHANNEL_CACHE_T *ptChannel   通道句柄
               FRAME_HEAD_T *ptFrame        解析帧头
               uint8 *pMsg                  消息
               uint16 msglen                消息长度
* @param[out]：FRAME_HEAD_T *ptFrame        解析帧头
* @return    ：SEPARATE_PROC_RESULT_END     处理结束
               SEPARATE_PROC_RESULT_NEXTTIME下次调度处理
               SEPARATE_PROC_RESULT_GOON    继续处理
* @Create    : 王津剑
* @Date      ：2019-12-5
* @Update    :
**********************************************************************/
int protocol_link_separate(CHANNEL_CACHE_T *ptChannel, FRAME_HEAD_T *ptFrame, uint8 *pMsg, uint16 msglen)
{
    int   errcode = 0;
    
    if(0 == ptFrame->separate)
    {
        return SEPARATE_PROC_RESULT_GOON;
    }
    
    if(LINK_SEPARATE_ACK == ptFrame->tSepara.type)
    {
        /* 上行分帧的确认 */
        CAHNNEL_LOCK_SEND(ptChannel, APP698_PROC_LOCK);
        errcode = app_set_sendblock(ptChannel, ptFrame->tSepara.block);
        CAHNNEL_UNLOCK_SEND(ptChannel);
        if(0 == errcode)
        {
            APP698_FMT_DEBUG("app_set_sendblock err %d\n", errcode);
        }
        return SEPARATE_PROC_RESULT_END;
    }
    
    /* 最后一帧需要判断发送缓存中是否还存在报文 存在就等待一会儿 */
    //if(0 < ptChannel->sendMessLen)
    //{
    //    APP698_FMT_TRACE("Channel %d has mess to send, wait to next time\n", ptChannel->id);
    //    return SEPARATE_PROC_RESULT_NEXTTIME;
    //}
    
    /* 下行分帧 先不考虑乱序的问题 */
    if(LINK_SEPARATE_START == ptFrame->tSepara.type)
    {
        CHANNEL_EMPTY_PROC_MESS(ptChannel);
        app_insert_procmesstail(ptChannel, pMsg, ptFrame->headlen + 1, 0);
    }
    
    app_insert_procmesstail(ptChannel, ptFrame->pApdu, ptFrame->apduLen, ptFrame->tSepara.block);

    /* 封装确认帧 */
    if(LINK_SEPARATE_END != ptFrame->tSepara.type)
    {
        protocol_sepa_ack_package(ptChannel, ptFrame);
        return SEPARATE_PROC_RESULT_END;
    }
    else
    {
        /* 尾部添加FCS和结束标记 */
        if(0 != app_insert_procmesstail(ptChannel, pMsg + msglen - 3, 3, ptFrame->tSepara.block))
        {
            APP698_FMT_DEBUG("Channel %d app_insert_procmesstail err\n", ptChannel->id);
            return SEPARATE_PROC_RESULT_END;
        }
        app_update_procmess_frame(ptChannel, ptFrame); //链路分帧，使用组装的消息
        /* 上行分帧结束, 不能把分帧标记带下来 */
        ptFrame->separate = 0;
    }
    return SEPARATE_PROC_RESULT_GOON;
}

/**********************************************************************
* @name      : protocol_check_frame
* @brief     ：698规约帧头校验
* @param[in] ：FRAME_HEAD_T *pTFrame   帧头
* @param[out]：
* @return    ：0-正确/错误码
* @Create    : 王津剑
* @Date      ：2019-12-7
* @Update    :
**********************************************************************/
int protocol_check_frame(FRAME_HEAD_T *pTFrame)
{
    uint32        i;
    uint8         len = 0;
    uint8         temp = 0;
    uint8        *pAddr = NULL;
    OOP_GRPADDR_T tGrpAddr;
    
    /* 去除自发自收帧 */
    if(MESSAGE_DIR_FROM_SERVER == pTFrame->dir)
    {
        return APP698_ERR_NO_PROC_MESS;
    }

    len = app_get_localaddr(&pAddr);
    /* 地址类型判断 */
    if(TRUE == pTFrame->NoEsamMac)
    {
        /* 特殊类型地址不需要进行判断 */
        return APP698_SECCESS;
    }
    else if(RTU_ADDRESS_SINGLE == pTFrame->sAddr.type)
    {
        if(len != pTFrame->sAddr.len || 0 != memcmp(pTFrame->sAddr.addr, pAddr, len))
        {
            return APP698_ERR_ADDR_NOTMATCH;
        }
    }
    else if(RTU_ADDRESS_WILD == pTFrame->sAddr.type)
    {
        if(pTFrame->sAddr.len < len)
        {
            return APP698_ERR_ADDR_NOTMATCH;
        }
        for(i = 0; i < len; i++)
        {
            temp = pTFrame->sAddr.addr[i];
            if(0xa0 == (pTFrame->sAddr.addr[i] & 0xf0))
            {
                temp &= 0x0f;
                temp |= pAddr[i]&0xf0;
            }
            if(0x0a == (pTFrame->sAddr.addr[i] & 0x0f))
            {
                temp &= 0xf0;
                temp |= pAddr[i]&0x0f;
            }
            if(temp != pAddr[i])
            {
                return APP698_ERR_ADDR_NOTMATCH;
            }
        }
        for(; i < pTFrame->sAddr.len; i++)
        {
            if(0xaa != pTFrame->sAddr.addr[i] && 0xaf != pTFrame->sAddr.addr[i])
            {
                return APP698_ERR_ADDR_NOTMATCH;
            }
        }
    }
    else if(RTU_ADDRESS_TEAM == pTFrame->sAddr.type)
    {
        app_get_groupaddr(&tGrpAddr);
        for(i = 0; i < tGrpAddr.nNum; i++)
        {
            if(tGrpAddr.sa[i].len == pTFrame->sAddr.len && 0 == memcmp(pTFrame->sAddr.addr, tGrpAddr.sa[i].add, len))
            {
                return APP698_SECCESS;
            }
        }
        return APP698_ERR_ADDR_NOTMATCH;
    }
    else if(RTU_ADDRESS_BROADCAST == pTFrame->sAddr.type)
    {
        if(1 != pTFrame->sAddr.len || 0xaa != pTFrame->sAddr.addr[0])
        {
            return APP698_ERR_ADDR_BROADCAST;
        }
    }
    else
    {
        return APP698_ERR_ADDR_NOTMATCH;
    }

    return APP698_SECCESS;
}

/**********************************************************************
* @name      : is_reject_from_group
* @brief     ：剔除投入命令后，对时可以下发
* @param[in] ：FRAME_HEAD_T *pTFrame   帧头
* @param[out]：
* @return    ：TRUE-剔除投入/FLASE-剔除解除
**********************************************************************/
BOOLEAN is_reject_from_group(DB_CLIENT clientid, FRAME_HEAD_T *pTFrame)
{
    uint8 data = 0;
    NOMAL_OAD_T normal;
    uint32 len = 0; 
    int32 ret = ERR_NORMAL; 
    BOOLEAN bReject = FALSE;

    if ((pTFrame == NULL) || (pTFrame->pApdu == NULL))
    {
        PRTL_FMT_DEBUG("is_reject_from_group err[null]\n");
    }

    MEMZERO(&normal, sizeof(NOMAL_OAD_T)); 
    normal.oad.value = 0x40240200;
    ret = db_read_nomal(clientid, &normal, sizeof(data), (uint8*)&data, &len);
    if ((ret != ERR_OK) || (len == 0))
    {
        PRTL_FMT_DEBUG("db_read_nomal failed. ret(%d), logicId(%d), infoNum(%d), oad(0x%08x)\n",
            ret, normal.logicId, normal.infoNum, normal.oad.value);
        return FALSE;
    }

    if (data == 0x01)
    {
        //剔除投入命令后，对时可以下发
        if ((pTFrame->pApdu[3] == 0x40) && (pTFrame->pApdu[4] == 0x00))
        {
            bReject = FALSE;
        }
        else
        {
            bReject = TRUE;
        }
    }
    else
    {
        bReject = FALSE;
    }
    
    return bReject;
}

/**********************************************************************
* @name      : protocol_698_msgproc
* @brief     ：698规约处理
* @param[in] ：CHANNEL_CACHE_T *ptChannel  通道句柄
               uint8 *msg698               698报文
               uint32 len698               698报文长度
               uint16 buflen               发送报文最大缓存
* @param[out]：uint8 *sendbuf              返回的698报文
               uint8 *proto                私有协议
* @return    ：发送报文报文长度
* @Create    : 王津剑
* @Date      ：2021-9-4
* @Update    :
**********************************************************************/
uint16 protocol_698_msgproc(CHANNEL_CACHE_T *ptChannel, uint8 *msg698, uint32 len698, uint8 *sendbuf, uint16 buflen, 
                            uint8 *proto)
{
    uint8        id = ptChannel->id;
    int          errcode = 0;
    uint8       *msg  = msg698;
    uint32       msglen = len698;
    uint16       newlen = 0;
    FRAME_HEAD_T tFrame;
    uint8        breakflag = 0;
    
    /* 合法性检查 */
    errcode = app_check_message(msg, msglen);
    if(0 != errcode)
    {
        APP698_BUF_DEBUG(msg, TRUE == ptChannel->bMsgExtProc ? 250 :msglen, "Channel %d check msg %d error %d!\n", 
                         id, msglen, errcode);
        return 0;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    errcode = app_get_frame(msg, msglen, &tFrame, TRUE);
    if(0 != errcode)
    {
        APP698_BUF_DEBUG(msg, TRUE == ptChannel->bMsgExtProc ? 250 :msglen, "Channel %d get frame error %d code = %d\n", 
                         id, msglen, errcode);
        return 0;
    }

    /* 后续帧头处理 485维护口标记(访问交采数据) */
    if(COMM_MEDIA_485 == ptChannel->tChannel.medium && RTU_ADDRESS_SINGLE == tFrame.sAddr.type)
    {
        uint8 checkaddr[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
        if(6 == tFrame.sAddr.len && 0 == memcmp(checkaddr, tFrame.sAddr.addr, 6))
        {
            tFrame.NoEsamMac = TRUE;

            //记入通道，用于规约解析区分是不是交采模块特殊地址
            ptChannel->private[4] = TRUE;
        }
    }

    /* 帧头检查 */
    errcode = protocol_check_frame(&tFrame);
    if(0 != errcode)
    {
        APP698_BUF_DEBUG(msg, TRUE == tFrame.bExtmsg ? 250 :msglen, "Channel %d check frame error %d, len %d\n", 
                         id, errcode, msglen);
        return 0;
    }

    /* 是否剔除投入 */
    if ((RTU_ADDRESS_TEAM == tFrame.sAddr.type) && is_reject_from_group(ptChannel->clientid, &tFrame))
    {
        APP698_FMT_TRACE("is_reject_from_group reject ok\n");
        return 0;
    }

    /* 扩展长度的消息不支持链路分帧处理 */
    if(TRUE != tFrame.bExtmsg && TRUE == tFrame.separate)
    {
        errcode = protocol_link_separate(ptChannel, &tFrame, msg, msglen);
        if(SEPARATE_PROC_RESULT_END == errcode)
        {
            return 0;
        }
    }

    ptChannel->bSafe = FALSE;
    /* 安全传输提前解析出APDU内容 先清除安全标识*/
    tFrame.security = 0;
    ptChannel->tContext.curSecurityMode = SecMod_NULL;
    ptChannel->tContext.securityServer = FALSE;

    #ifdef AREA_HEBEI
    //河北主动上报的确认帧如果是明文，没有MAC可解，不需要解密
    if ((SERVERID_SECURITYREQUEST == tFrame.pApdu[0] || ((SERVERID_SECURITYRESPONSE == tFrame.pApdu[0]) && (1 == tFrame.pApdu[1]))) && TRUE != tFrame.bExtmsg)
    #else
    if ((SERVERID_SECURITYREQUEST == tFrame.pApdu[0] || SERVERID_SECURITYRESPONSE == tFrame.pApdu[0]) && TRUE != tFrame.bExtmsg)
    #endif
    {
        uint8 *messtemp = NULL;
        uint8  security = 0;
        uint8  NoEsamMac = tFrame.NoEsamMac;
        
        messtemp = (uint8 *)malloc(APP_RECV_MSG_MAXLEN);
        if(NULL == messtemp)
        {
            APP698_FMT_DEBUG("Channel %d securityrequest_getapdu malloc failed\n", id);
            return 0;
        }

        msglen = protocol_securityrequest(&tFrame, messtemp, APP_RECV_MSG_MAXLEN-2, &ptChannel->tContext, &security);
        memcpy(msg, messtemp, msglen);
        free(messtemp);
        
        APP698_BUF_DEBUG(msg, msglen, "Chan %d new msg", id);

        /* 重新获取帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        errcode = app_get_frame(msg, msglen, &tFrame, TRUE);
        if(0 != errcode)
        {
            APP698_FMT_DEBUG("Channel %d get Frame error code = %d\n", id, errcode);
            return 0;
        }
        tFrame.security = security;
        tFrame.NoEsamMac = NoEsamMac;

        /* 在通道里记下安全模式标记 */
        ptChannel->tContext.curSecurityMode = security;
        ptChannel->tContext.securityServer = TRUE;

        if (tFrame.apduLen > 0)
        {
            PRTL_BUF_DEBUG(tFrame.pApdu, tFrame.apduLen, "解密数据ADPU内容:");
        }
        
        ptChannel->bSafe = TRUE;
    }
    else if(SERVERID_PROXYREQUEST == tFrame.pApdu[0] && TRUE != tFrame.bExtmsg)
    {
        /* 代理 */
        tFrame.security = 0x40;
    }

    /* APDU解析 */
    newlen = protocol_analyze_apdu(ptChannel, &tFrame, sendbuf, buflen - 2, proto, &breakflag);
    if(RTU_ADDRESS_TEAM == tFrame.sAddr.type || RTU_ADDRESS_BROADCAST == tFrame.sAddr.type)
    {
        newlen = 0; //组播 广播 消息 不应答
    }

    if(TRUE == breakflag)
    {
        if(CHECK_CACHE_APDU_1(tFrame.pApdu))
        {
            if(ptChannel->pBreakBakMsg)
            {
                free(ptChannel->pBreakBakMsg);
                ptChannel->pBreakBakMsg = NULL;
            }
            ptChannel->pBreakBakMsg = (uint8 *)malloc(msglen);
            if(NULL == ptChannel->pBreakBakMsg)
            {
                APP698_BUF_DEBUG(msg, msglen, "Channel %d save break msg error\n", id);
                return 0;
            }
            memcpy(ptChannel->pBreakBakMsg, msg, msglen);
            ptChannel->BreakMsgLen = msglen;
        }
    }
    ptChannel->bSendApduBreak = breakflag;

    return newlen;
}

/**********************************************************************
* @name      : protocol_analyze_ssal
* @brief     ：698规约处理
* @param[in] ：uint8 id      通道id
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-9-3
* @Update    :
**********************************************************************/
void protocol_analyze_ssal(uint8 id)
{
    int         ret = 0;
    SSAL_HEAD_T tSsalHead;
    uint16      start;
    uint16      msglen = 0;
    CHANNEL_CACHE_T *ptChannel = NULL;
    uint8       databuf[APP_RECV_MSG_MAXLEN] = {0};
    uint16      datalen = 0;
    uint16      retcode = 0;
    uint16      backlen = 0;
    int         backflag = 0;
    uint8       proto = 0;
    SEC_DATA_T  tSeamSend;
    SEC_DATA_T  tSeamRecv;
    uint8       addrlen = 0;
    uint8      *pAddr = NULL;
    
    ptChannel = app_get_channel(id);
    if(NULL == ptChannel || 0 == ptChannel->bOpen)
    {
        return;
    }
    
    memset(&tSsalHead, 0, sizeof(SSAL_HEAD_T));
    /* 获取SSAL合法报文 */
    ret = ssal_find_first_msg(id, &tSsalHead, &start, &msglen, APP698_PROC_LOCK);
    if(0 != ret)
    {
        goto end;
    }

    /* 刷新周期 */
    ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
    if(0 == ptChannel->tChannel.workmode)
    {
        /* 江苏地区4h登录一次 */
        ptChannel->leftHB = 14400;
    }

    /* 判断SSAL报文是否支持 */
    backflag = ssal_check_support(&tSsalHead, &retcode);
    APP698_FMT_DEBUG("start proc 698msg backflag % d\n", backflag);
    if(SSAL_CHECK_DROP == backflag)
    {
        APP698_FMT_DEBUG("Channel %d drop ssal msg index %d\n", id, tSsalHead.seq);
        goto end;
    }
    else if(SSAL_CHECK_ERROR == backflag)
    {
        goto sendback;
    }

    /* SSAL报文处理-先解密 */
    memset(&tSeamSend, 0, sizeof(SEC_DATA_T));
    memset(&tSeamRecv, 0, sizeof(SEC_DATA_T));

    tSeamSend.dataType = 2;
    tSeamSend.cryptType = 0;
    tSeamSend.appType = 0;
    memcpy(tSeamSend.time, &tSsalHead.tp, 7);
    addrlen = app_get_localaddr(&pAddr);
    tSeamSend.devAddrLen = addrlen;
    memcpy_r(tSeamSend.devAddr, pAddr, addrlen);
    memcpy(tSeamSend.dstIp, tSsalHead.ta.TA, 4);
    tSeamSend.policy = 0;
    tSeamSend.lenData = msglen;
    tSeamSend.data = ptChannel->recv + start;
    APP698_BUF_DEBUG(tSeamSend.data, tSeamSend.lenData, "下行报文\n");
    tSeamRecv.data = databuf;
    ret = sec_ssal_en_decryption(&tSeamSend, &tSeamRecv);
    APP698_FMT_DEBUG("Channel %d start proc 698msg ret % d\n", ret);
    if(0 != ret)
    {
        retcode = SAL_RECVINFO_ERROR_TERMINAL_DECIPHERING;
        backflag = SSAL_CHECK_ERROR;
        goto sendback;
    }

    backflag = SSAL_CHECK_OK;
    APP698_FMT_DEBUG("start proc 698msg tSeamRecv.policy % d,tSeamRecv.dataType %d\n", tSeamRecv.policy,tSeamRecv.dataType);
    APP698_BUF_DEBUG(tSeamRecv.data, tSeamRecv.lenData, "上行报文\n");
    if(2 == tSeamRecv.dataType)
    {
        if(SSAL_LINK_HB == tSsalHead.fc.ssaltype || SSAL_LINK_MANAGER == tSsalHead.fc.ssaltype)
        {
            ptChannel->lostHBCount = 0;
        }
        datalen = tSeamRecv.lenData;
    }
    else if(1 == tSeamRecv.dataType)
    {
        /* 698 报文 */
        uint8  send698[APP_RECV_MSG_MAXLEN] = {0};
        uint16 slen698 = 0;
        struct tm ptm;
        time_t now_t;

        APP698_BUF_DEBUG(tSeamRecv.data, tSeamRecv.lenData, "Channel %d start proc 698msg\n", id);
        /* 处理 */
        slen698 = protocol_698_msgproc(ptChannel, tSeamRecv.data, tSeamRecv.lenData, send698, APP_RECV_MSG_MAXLEN, &proto);
        if(0 == slen698)
        {
            goto end;
        }
        
        /* 加密 */
        now_t = time(NULL);
        LOCALTIME_R(&now_t, &ptm);
        
        tSeamSend.dataType = 1;
        tSeamSend.cryptType = tSeamRecv.cryptType;
        tSeamSend.appType = 0;
        tSeamSend.time[0] = (((ptm.tm_year + 1900) & 0xFF00) >> 8);
        tSeamSend.time[1] = (uint8)(ptm.tm_year + 1900);
        tSeamSend.time[2] = ptm.tm_mon + 1;
        tSeamSend.time[3] = ptm.tm_mday;
        tSeamSend.time[4] = ptm.tm_hour;
        tSeamSend.time[5] = ptm.tm_min;
        tSeamSend.time[6] = ptm.tm_sec;
        if(4 == tSsalHead.sa.sourcelen)
        {
            memcpy(tSeamSend.dstIp, tSsalHead.sa.SA, 4);
        }
        else
        {
            memcpy(tSeamSend.dstIp, &ptChannel->tChannel.ip, 4);
        }
        addrlen = app_get_localaddr(&pAddr);
        tSeamSend.devAddrLen = addrlen;
        memcpy(tSeamSend.devAddr, pAddr, addrlen);
        tSeamSend.policy = 0;
        tSeamSend.lenData = slen698;
        tSeamSend.data = send698;
        APP698_BUF_DEBUG(tSeamSend.data, tSeamSend.lenData, "返回主站加密前的698报文\n");
        memset(databuf, 0, tSeamRecv.lenData);
        tSeamRecv.data = databuf;
        ret = sec_ssal_en_decryption(&tSeamSend, &tSeamRecv);
        if(0 != ret)
        {
            retcode = SAL_RECVINFO_ERROR_TERMINAL_DECIPHERING;
            backflag = SSAL_CHECK_ERROR;
            goto sendback;
        }
        datalen = tSeamRecv.lenData;
        APP698_BUF_DEBUG(tSeamRecv.data, tSeamRecv.lenData, "返回主站加密后的698报文\n");
    }
    else
    {
        /* 直接返回 */
        goto end;
    }
    
sendback:
    if(SSAL_CHECK_ERROR == backflag)
    {
        /* 发送报错 */
        uint8 *pMsgSend = NULL;
        SSAL_HEAD_T tSsalSend;
            
        /* 返回消息赋值 */
        memset(&tSsalSend, 0, sizeof(SSAL_HEAD_T));
        ssal_recv_to_send_head(&tSsalHead, &tSsalSend);
        tSsalSend.data = databuf;
        tSsalSend.datalen = datalen;
        tSsalSend.retcode = retcode;
        pMsgSend = ssal_msg_package(&tSsalSend, &backlen);
        if(pMsgSend)
        {
            ret = send_emergency_ipcmsg(id, proto, pMsgSend, backlen);
            if(0 != ret)
            {
                APP698_BUF_DEBUG(pMsgSend, backlen, "Channel %d send filed %d\n", id, ret);
            }
            free(pMsgSend);
        }
    }
    else if(SSAL_CHECK_OK == backflag)
    {
        /* 直接返回 */
        APP698_BUF_DEBUG(databuf, datalen, "返回主站的SSAL报文\n");
        ret = send_emergency_ipcmsg(id, proto, databuf, datalen);
        if(0 != ret)
        {
            APP698_BUF_DEBUG(databuf, datalen, "Channel %d send filed %d\n", id, ret);
        }
    }
end:
    ssal_remove_recvmsg(id, start, msglen, APP698_PROC_LOCK);
}

/**********************************************************************
* @name      : protocol_analyze_channel
* @brief     ：698规约处理
* @param[in] ：uint8 id      通道id
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-4
* @Update    :
**********************************************************************/
void protocol_analyze_698(uint8 id)
{
    int               errcode = 0;
    CHANNEL_CACHE_T  *ptChannel = NULL;
    uint8            *msg  = NULL;
    uint8             msgrecv[APP_RECV_MSG_MAXLEN] = {0};     //接收IPC缓存
    uint8             sendbuf[APP_RECV_MSG_MAXLEN] = {0}; //发送IPC缓存
    uint32            msglen = 0;
    uint32            newlen = 0;
    FRAME_HEAD_T      tFrame;
    uint8             proto = 0;   //私有协议
    uint8             breakflag = 0;
    uint16            maxSendLen = 0;
    int               ret = 0;

    ptChannel = app_get_channel(id);
    if(NULL == ptChannel || 0 == ptChannel->bOpen)
    {
        return;
    }

    /* 获取第一个消息 */
    if(TRUE == ptChannel->bmsgExtOver)
    {   /* 扩展长度的消息 */
        msglen = ptChannel->extMsgLen;
        msg = ptChannel->extMsg;
        ptChannel->bMsgExtProc = TRUE;
        if(NULL == msg || 0 == msglen)
        {
            goto end;
        }
    }
    else
    {
        msg = msgrecv;

        //交采透传通道 用于校表
        if(id == CHL_ACMETER)
        {
             //取出645报文
            msglen = (uint32)app_get_645recv(id, msg, APP_RECV_MSG_MAXLEN, APP698_PROC_LOCK);
            if(msglen > 0)
            {
                APP698_BUF_DEBUG(msg, msglen, "Channel %d send 645 to acMeter\n", id);
                ret = puAmr_acMeter_revsnd(msg, (uint16)msglen, sendbuf, (uint16*)&newlen);
                if((ret == ERR_OK)&&(newlen > 0))
                {
                    /* 直接发送 */
                    ret = send_emergency_ipcmsg(id, proto, sendbuf, newlen);
                    if(0 != ret)
                    {
                        APP698_BUF_DEBUG(sendbuf, newlen, "Channel %d send filed %d\n", id, ret);
                    }
                }
            }
            goto end;
        }
        
        msglen = (uint32)app_get_recvmsg(id, msg, APP_RECV_MSG_MAXLEN, APP698_PROC_LOCK);
        if(0 == msglen)
        {
            return;
        }
        //消息收发表明在通信 刷新心跳计时
        ptChannel->leftHB = CHANNEL_GET_HEART(ptChannel);
    }

    if(TRUE == ptChannel->bNeedClose)
    {
        APP698_FMT_DEBUG("Channel %d need to bo closed!\n", id);
        goto end;
    }

    /* 合法性检查 */
    errcode = app_check_message(msg, msglen);
    if(0 != errcode)
    {
        APP698_BUF_DEBUG(msg, TRUE == ptChannel->bMsgExtProc ? 250 :msglen, "Channel %d check msg %d error %d!\n", 
                         id, msglen, errcode);
        goto end;
    }

    /* 解析帧头 */
    memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
    errcode = app_get_frame(msg, msglen, &tFrame, TRUE);
    if(0 != errcode)
    {
        APP698_BUF_DEBUG(msg, TRUE == ptChannel->bMsgExtProc ? 250 :msglen, "Channel %d get frame error %d code = %d\n", 
                         id, msglen, errcode);
        goto end;
    }

    /* 后续帧头处理 485维护口标记(访问交采数据) */
    if(COMM_MEDIA_485 == ptChannel->tChannel.medium && RTU_ADDRESS_SINGLE == tFrame.sAddr.type)
    {
        uint8 checkaddr[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
        if(6 == tFrame.sAddr.len && 0 == memcmp(checkaddr, tFrame.sAddr.addr, 6))
        {
            tFrame.NoEsamMac = TRUE;

            //记入通道，用于规约解析区分是不是交采模块特殊地址
            ptChannel->private[4] = TRUE;
        }
    }

    /* 帧头检查 */
    errcode = protocol_check_frame(&tFrame);
    if(0 != errcode)
    {
        APP698_BUF_DEBUG(msg, TRUE == tFrame.bExtmsg ? 250 :msglen, "Channel %d check frame error %d, len %d\n", 
                         id, errcode, msglen);
        goto end;
    }

    /* 是否剔除投入 */
    if ((RTU_ADDRESS_TEAM == tFrame.sAddr.type) && is_reject_from_group(ptChannel->clientid, &tFrame))
    {
        APP698_FMT_TRACE("is_reject_from_group reject ok\n");
        goto end;
    }

    APP698_BUF_DEBUG(msg, TRUE == tFrame.bExtmsg ? 250 :msglen, "Channel %d start to proc len %d!\n", id, msglen);

    /* 扩展长度的消息不支持链路分帧处理 */
    if(TRUE != tFrame.bExtmsg && TRUE == tFrame.separate)
    {
        errcode = protocol_link_separate(ptChannel, &tFrame, msg, msglen);
        if(SEPARATE_PROC_RESULT_END == errcode)
        {
            goto end;
        }
    }

    ptChannel->bSafe = FALSE;
    /* 安全传输提前解析出APDU内容 先清除安全标识*/
    tFrame.security = 0;
    ptChannel->tContext.curSecurityMode = SecMod_NULL;
    //测试用
    //ptChannel->tContext.curSecurityMode = 0;
    ptChannel->tContext.securityServer = FALSE;
    #if LONG_ENCRYPTION_EN == 1
    if(SERVERID_SECURITYREQUEST == tFrame.pApdu[0] || SERVERID_SECURITYRESPONSE == tFrame.pApdu[0])
    #else
    if((SERVERID_SECURITYREQUEST == tFrame.pApdu[0] || SERVERID_SECURITYRESPONSE == tFrame.pApdu[0]) && TRUE != tFrame.bExtmsg)
    #endif
    {
        uint8 *messtemp = NULL;
        uint8  security = 0;
        
        messtemp = (uint8 *)malloc(APP_RECV_BIGMSG_MAXLEN);
        if(NULL == messtemp)
        {
            APP698_FMT_DEBUG("Channel %d securityrequest_getapdu malloc failed\n", id);
            goto end;
        }

        msglen = protocol_securityrequest(&tFrame, messtemp, APP_RECV_BIGMSG_MAXLEN-2, &ptChannel->tContext, &security);
        memcpy(msg, messtemp, msglen);
        free(messtemp);
        
        APP698_BUF_DEBUG(msg, MIN(msglen, 2048), "Channel %d new msg len=%d", id, msglen);

        /* 重新获取帧头 */
        memset(&tFrame, 0, sizeof(FRAME_HEAD_T));
        errcode = app_get_frame(msg, msglen, &tFrame, TRUE);
        if(0 != errcode)
        {
            APP698_FMT_DEBUG("Channel %d get Frame error code = %d\n", id, errcode);
            goto end;
        }
        tFrame.security = security;

        /* 在通道里记下安全模式标记 */
        ptChannel->tContext.curSecurityMode = security;
        ptChannel->tContext.securityServer = TRUE;

        if (tFrame.apduLen > 0)
        {
            PRTL_BUF_DEBUG(tFrame.pApdu, MIN(tFrame.apduLen, 2048), "解密数据ADPU内容:");
        }
        
        ptChannel->bSafe = TRUE;
    }
    else if(SERVERID_PROXYREQUEST == tFrame.pApdu[0] && TRUE != tFrame.bExtmsg)
    {
        /* 代理 */
        tFrame.security = 0x40;
    }

    /* APDU解析 */
    newlen = (uint32)protocol_analyze_apdu(ptChannel, &tFrame, sendbuf, APP_SEND_MSG_MAXLEN-2, &proto, &breakflag);
    if(RTU_ADDRESS_TEAM == tFrame.sAddr.type || RTU_ADDRESS_BROADCAST == tFrame.sAddr.type)
    {
        newlen = 0; //组播 广播 消息 不应答
    }

    if(newlen > 1 && 0 != app_check_message(sendbuf, newlen))
    {
        APP698_BUF_DEBUG(sendbuf, newlen, "Channel %d check send msg error!\n", id);
        goto end;
    }

    maxSendLen = channel_get_break_frame_size(ptChannel);
    if(newlen > maxSendLen)
    {
        /* 大于链路分帧的放到缓存中发送，如果之前存在链路分帧, 就将其覆盖 */
        CAHNNEL_LOCK_SEND(ptChannel, APP698_PROC_LOCK);
        memcpy(ptChannel->sendMess, sendbuf, APP_SEND_MSG_MAXLEN);
        CHANNEL_SET_SEND_MESS_NOLOCK(ptChannel, newlen, 1, proto);
        CAHNNEL_UNLOCK_SEND(ptChannel);
    }
    else if(newlen > 0)
    {
        /* 直接发送 */
        ret = send_emergency_ipcmsg(id, proto, sendbuf, newlen);
        if(0 != ret)
        {
            APP698_BUF_DEBUG(sendbuf, newlen, "Channel %d send filed %d\n", id, ret);
        }
    }

    if(TRUE == breakflag)
    {
        if(CHECK_CACHE_APDU_1(tFrame.pApdu))
        {
            if(ptChannel->pBreakBakMsg)
            {
                free(ptChannel->pBreakBakMsg);
                ptChannel->pBreakBakMsg = NULL;
            }
            ptChannel->pBreakBakMsg = (uint8 *)malloc(msglen);
            if(NULL == ptChannel->pBreakBakMsg)
            {
                APP698_BUF_DEBUG(msg, msglen, "Channel %d save break msg error\n", id);
                goto end;
            }
            memcpy(ptChannel->pBreakBakMsg, msg, msglen);
            ptChannel->BreakMsgLen = msglen;
        }
    }
    ptChannel->bSendApduBreak = breakflag;

    /* 强制调度一次 减少时间 */
    protocol_698_addpool(id);
end:
    app_remove_firstrecvmsg(id, APP698_PROC_LOCK);
}

/**********************************************************************
* @name      : protocol_698_entry
* @brief     ：698规约处理入口线程
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-4
* @Update    :
**********************************************************************/
void *protocol_698_entry(void *pArg)
{
    uint8 i;
    PROTOCOL_THREAD_T *ptHandle = (PROTOCOL_THREAD_T *)pArg;

    /* 线程重启后 智能解锁 防止死锁 */
    for(i = 0; i < ptHandle->num; i++)
    {
        if(CHANNEL_DEFAULT == ptHandle->channel[i])
        {
            continue;
        }
        app_unlock_channel(ptHandle->channel[i], APP698_PROC_LOCK);
    }
    while(1)
    {
        *ptHandle->pRunFlag = 0;
        for(i = 0; i < ptHandle->num; i++)
        {
            if(CHANNEL_DEFAULT == ptHandle->channel[i])
            {
                continue;
            }
            
            *ptHandle->pRunFlag = 0;
            /* 规约解析 */
            app_channel_proctime(ptHandle->channel[i], TRUE);
            protocol_analyze_channel(ptHandle->channel[i]);
            app_channel_proctime(ptHandle->channel[i], FALSE);
        }
        usleep(10000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
        
    }
    return 0;
}
/**********************************************************************
* @name      : protocol_analyze_channel
* @brief     ：698规约处理
* @param[in] ：uint8 id      通道id
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-4
* @Update    :
**********************************************************************/
void protocol_analyze_channel(uint8 id)
{
    CHANNEL_CACHE_T  *ptChannel = NULL;

    ptChannel = app_get_channel(id);
    if(NULL == ptChannel || 0 == ptChannel->bOpen)
    {
        return;
    }

    if(1 == ssal_get_status() && 
       (COMM_MEDIA_ETH == ptChannel->tChannel.medium || COMM_MEDIA_GPRS == ptChannel->tChannel.medium))
    {
        protocol_analyze_ssal(id);
    }
    else
    {
        protocol_analyze_698(id);
    }
}

/**********************************************************************
* @name      : protocol_698_task
* @brief     ：698规约处理线程池任务函数
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-12
* @Update    :
**********************************************************************/
void protocol_698_task(long pArg) 
{
    uint8 chan = (uint8)pArg;

    /* 设置通道工作状态，保证一个通道只能同时被一个线程处理 */
    if(0 == app_set_channelworklock(chan, TRUE))
    {
        app_channel_proctime(chan, TRUE); //设置通道的开始工作时间，用于通道超时维护
        protocol_analyze_channel(chan);   //规约解析
        app_channel_proctime(chan, FALSE);
        app_set_channelworklock(chan, FALSE); //解除通道工作状态
    }
}

/**********************************************************************
* @name      : protocol_698_pool
* @brief     ：698规约处理线程池初始化
* @param[in] ：
* @param[out]：
* @return    ：0-成功/-1-失败
* @Create    : 王津剑
* @Date      ：2019-12-12
* @Update    :
**********************************************************************/
int protocol_698_poolinit()
{
    g_Pool = threadpool_create(PROTOCOL_POOL_SIZE, PROTOCOL_QUEUE_SIZE, 0);
    if(NULL == g_Pool)
    {
        return -1;
    }
    return 0;
}

/**********************************************************************
* @name      : protocol_698_addpool
* @brief     ：698规约处理线程池调度
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2019-12-12
* @Update    :
**********************************************************************/
int protocol_698_addpool(uint8 channel)
{
    return threadpool_add(g_Pool, protocol_698_task, channel, 0);
}

/**********************************************************************
* @name      : protocol_698_pool
* @brief     ：698规约处理线程池管理
* @param[in] ：void *pArg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-12-11
* @Update    :
**********************************************************************/
void *protocol_698_pool(void *pArg)
{
    uint8 i;
    PROTOCOL_THREAD_T *ptHandle = (PROTOCOL_THREAD_T *)pArg;
    CHANNEL_CACHE_T  *ptChannel = NULL;
    struct timespec curtime = {0};
    uint32 curSecond = 0;

    /* 线程重启后 智能解锁 防止死锁 */
    for(i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        app_unlock_channel(i, APP698_PROC_LOCK);
    }

    while(1)
    {
        *ptHandle->pRunFlag = 0;
        clock_gettime(CLOCK_MONOTONIC, &curtime);
        curSecond = (uint32)curtime.tv_sec;

        /* 强制调度 */
        for(i = 0; i < MAX_CHANNEL_NUM; i++)
        {
            ptChannel = app_get_channel(i);
            if(NULL == ptChannel || 0 == ptChannel->bOpen || 0 == ptChannel->totalLen || TRUE == ptChannel->bNeedClose)
            {
                continue;
            }
            if(1 == ptChannel->workFlag && 0 == ptChannel->longFlag)
            {
                if(curSecond > ptChannel->startTime + 120)
                {
                    /* 长时间处理没有结束 */
                    ptChannel->longFlag = 1;
                    APP698_FMT_DEBUG("Channel %d proc not finish more than 1min, restart proecss!\n", i);
                    app_log_channel_recvbuf(i, APP698_PROC_LOCK);
                    sleep(1);
                    printf("Channel %d proc not finish more than 1min, restart proecss!\n", i);
                    exit(0);  //处理时间超过1min的报文 重启进程
                }
            }
            protocol_698_addpool(i);
        }
        
        usleep(10000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    
    return 0;
}

