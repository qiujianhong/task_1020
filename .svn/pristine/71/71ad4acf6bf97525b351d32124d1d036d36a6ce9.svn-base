/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   通信模块无线通信
* @date：    2019-10-31
* @history： 
*********************************************************************
*/

#include "baseCommon.h"
#include <net/if.h>
#include <error.h>
#include "msgUdp.h"
#include "storage.h"
#include <linux/tcp.h>
#include "task_msg.h"
#include "db_api.h"
#include "msg_id.h"
#include "task_id.h"

#include "comm.h" 
#include "sms_comm.h"
#include "appcomm.h"
#include "area.h"

uint8   g_SMSdataBuf[2][SMS_BUF_SIZE + 8] = {{0}, {0}};   /* 缓冲区 */

uint8   *g_SimFlag[2] = {NULL};
uint8   *g_SMSDIAGFlag[2] = {NULL};

extern pthread_mutex_t  g_fCommMsg;
extern int              g_CommTaskSync;
extern uint8 gdelay;

/**********************************************************************
* @name      : comm_sms_init
* @brief     ：短信模块初始化
* @param[in] ：COMM_SMS_HANDLE_T *ptSMS0Handle
               COMM_SMS_HANDLE_T *ptSMS1Handle
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-7-23
* @Update    : 
**********************************************************************/
void comm_sms_init(COMM_SMS_HANDLE_T *ptSMS0Handle, COMM_SMS_HANDLE_T *ptSMS1Handle)
{
    g_SimFlag[0] = &(ptSMS0Handle->bSimOK);
    g_SimFlag[1] = &(ptSMS1Handle->bSimOK);

    g_SMSDIAGFlag[0] = &(ptSMS0Handle->bDiagOK);
    g_SMSDIAGFlag[1] = &(ptSMS1Handle->bDiagOK);

    ptSMS0Handle->dbClient = db_client_create(DB_YC_AGENT, DB_CLIENT_SMS0);
    if(ptSMS0Handle->dbClient <= 0)
    {
        COMM_FMT_DEBUG("SMS0 db_client_create create failed!");
    }

    ptSMS1Handle->dbClient = db_client_create(DB_YC_AGENT, DB_CLIENT_SMS1);
    if(ptSMS1Handle->dbClient <= 0)
    {
        COMM_FMT_DEBUG("SMS1 db_client_create create failed!");
    }
}

/**********************************************************************
* @name      : comm_set_smsarg
* @brief     ：设置短信参数
* @param[in] ：uint8 id          模块id 1,2
               char *center      短信中心号码
               uint8 type        短信格式类型
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-7-24
* @Update    : 
**********************************************************************/
int comm_set_smsarg(uint8 id, char *center, uint8 type)
{
    uint8   dataBuf[64] = {0};
    uint16  dataLen = 0;
    uint8   i= 0;
    uint16  centerlen = 0;

    /* 设置短信中心号码 */
    dataBuf[dataLen++] = id;
    dataBuf[dataLen++] = type;

    /* 去掉空格 */
    centerlen = strlen(center);
    for(i = 0; i < centerlen; i++)
    {
        if(' ' != center[i])
        {
            break;
        }
    }
    if(i == centerlen)
    {
        return -1;
    }

    dataBuf[dataLen++] = centerlen - i;
    memcpy(dataBuf + dataLen, center + i, centerlen - i);
    dataLen += centerlen - i;
    return comm_send_label_msg(MSG_COMM_SMSNO, id, dataBuf, dataLen);
}

/**********************************************************************
* @name      : comm_sms_eventmsg
* @brief     ：无线模块通知消息解析
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-7-23
* @Update    : 
**********************************************************************/
int comm_sms_eventmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    uint8 port;
    uint8 status;

    if(NULL == playload)
    {
        return -1;
    }

    port = *playload;
    status = *(playload + 1);
    if(0 == port || port > 2)
    {
        COMM_FMT_DEBUG("comm_sms_eventmsg port %d err\n", port);
        return -1;
    }

    if(0 == status)
    {
        if(g_SMSDIAGFlag[port-1])
        {
            *(g_SMSDIAGFlag[port-1]) = TRUE;
        }
    }
    else
    {
        if(g_SMSDIAGFlag[port-1])
        {
            *(g_SMSDIAGFlag[port-1]) = FALSE;
        }
    }

    return 0;    
}

/**********************************************************************
* @name      : comm_sms_smsnomsg
* @brief     ：设置短信参数标记
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-7-24
* @Update    : 
**********************************************************************/
int comm_sms_smsnomsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    COMM_SMS_HANDLE_T *ptHandle = (COMM_SMS_HANDLE_T *)pArg;

    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_sms_smsnomsg input null\n");
        return -1;
    }

    if(0 == *playload)
    {
        COMM_FMT_DEBUG("Set SMS %d center No %s error!\n", ptHandle->no, ptHandle->tSMS.centernum);
        *ptHandle->pAppReStart = TRUE;
        ptHandle->bCenter = FALSE;
    }
    else
    {
        COMM_FMT_DEBUG("Set SMS %d center No %s seccess!\n", ptHandle->no, ptHandle->tSMS.centernum);
        ptHandle->bCenter = TRUE;;
    }
    return 0;    
}

/**********************************************************************
* @name      : comm_getcheck_pppcfg
* @brief     ：获取并检查ppp配置是否发生变化
* @param[in] ：int sockdbfd      通信句柄
               void *pCfg        配置存储地址
* @param[out]：uint8 *pRestart   线程重启标记
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-7-24
* @Update    :
**********************************************************************/
int comm_getcheck_smscfg(int sockdbfd, void *pCfg, uint8 *pRestart)
{
    int retSMS = 0;
    uint32 readLen = 0;
    NOMAL_OAD_T        tOAD;
    OOP_SMS_T          tSMS;      /* 短信通信参数 */
    COMM_SMS_HANDLE_T *ptHandle = (COMM_SMS_HANDLE_T *)pCfg;
    static uint8 s_smscfg[2] = {0};  //首次获取失败打印，防止日志过多

    if(sockdbfd <= 0 || NULL == pCfg)
    {
        return -1;
    }
    
    bzero(&tOAD, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = (0 == ptHandle->no) ? 0x45000400 : 0x45010400;
    memset(&tSMS, 0, sizeof(OOP_SMS_T));
    retSMS = db_read_nomal(sockdbfd, &tOAD, sizeof(OOP_SMS_T), (uint8 *)&tSMS, &readLen);
    if(0 == retSMS)
    {
        if(readLen != sizeof(OOP_SMS_T))
        {
            //DPRINT("Read 0x%x error\n", tOAD.oad.value);
            retSMS = -1;
            memset(&tSMS, 0, sizeof(OOP_SMS_T));
        }
        /* 关键配置变化 */
        if(0 != memcmp(ptHandle->tSMS.centernum, tSMS.centernum, 16))
        {
            *ptHandle->pAppReStart = TRUE;
        }
        else if(0 != memcmp(&(ptHandle->tSMS.hostnum), &(tSMS.hostnum), sizeof(OOP_SMS_HOST_T)))
        {
            /* 主站变化 */
            ptHandle->bCfgChange = TRUE;
        }
        
        memcpy(&(ptHandle->tSMS), &tSMS, readLen);
    }
    else if(FALSE == s_smscfg[ptHandle->no])
    {
        COMM_FMT_DEBUG("Read 0x%x failed, ret %d!\n", tOAD.oad.value, retSMS);
        s_smscfg[ptHandle->no] = TRUE;
    }

    if(0 == retSMS && strlen(ptHandle->tSMS.centernum) > 0)
    {
        ptHandle->bGetCfg = TRUE;
    }
    else
    {
        ptHandle->bGetCfg = FALSE;
    }
    
    return retSMS;
}

/**********************************************************************
* @name      : comm_sms_dcmmsg
* @brief     ：DCM查询消息解析
* @param[in] ：void *playload    消息内容
               uint16 msglen     消息长度
               void *pArg        私有数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-7-24
* @Update    : 
**********************************************************************/
int comm_sms_dcmmsg(uint8 *playload, uint16 msglen, uint16 label, void *pArg)
{
    WIRE_DIAG_INFO_T tDiagInfo;
    COMM_SMS_HANDLE_T *ptHandle = (COMM_SMS_HANDLE_T *)pArg;

    if(NULL == playload || NULL == pArg)
    {
        COMM_FMT_DEBUG("comm_sms_dcmmsg input null\n");
        return -1;
    }

    appmsg_wirediag_split(playload, &tDiagInfo);
    ptHandle->bDiagOK = (0 == tDiagInfo.diagstatus ? TRUE : FALSE);
    ptHandle->bSimOK = (1 == tDiagInfo.simstatus ? TRUE : FALSE);
    return 0;    
}


/**********************************************************************
* @name      : comm_smd_start
* @brief     ：检查是否需要启动ppp0线程
* @param[in] ：
* @param[out]：
* @return    ：TRUE-启动/FALSE-不启动
* @Create    : 王津剑
* @Date      ：2020-7-24
* @Update    :
**********************************************************************/
BOOLEAN comm_smd_start(void *pArg)
{
    COMM_SMS_HANDLE_T *ptHandle = (COMM_SMS_HANDLE_T *)pArg;

    if(FALSE == ptHandle->bGetCfg || FALSE == ptHandle->bSimOK || 0 == *ptHandle->pIPCok || FALSE == ptHandle->bDiagOK)
    {
        return FALSE;
    }

    return TRUE;
}

#define SMS_SEND_RETRY_CNT 1 /* 短信发送重试次数 */
OOP_SMS_HOST_T  g_SmsHost[2] = {{0}};
uint32 CommSmsSendDataProc(COMM_SMS_HANDLE_T *ptHandle)
{
    uint8   dataBuf[SMS_BUF_SIZE + 8] = {0};
    uint16  dataLen = 0;
    uint8   sysCom[200] = {0};
    uint8   recvbuf[200] = {0};
    uint16  offset = 0;
    uint32  tmpLen = 0;
    uint32  i = 0;
    int    ret = 0;
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvbuf;

    dataLen = comm_read_queue(ptHandle->ptSendList, dataBuf);

    if (dataLen <= 4 || dataLen > SMS_FRAM_MAXLEN + 4)
    {
        COMM_FMT_TRACE("SMS%u send dataLen:%u return\n", ptHandle->no, dataLen);
        return 1;
    }

    /* 主站号码校验 */
    /*if (g_SmsHost[ptHandle->no].nNum == 0)
    {
        return 1;
    }*/

    sysCom[offset++] = ptHandle->no + 1;
    tmpLen = strlen(g_SmsHost[ptHandle->no].item[0].value);
    sysCom[offset++] = tmpLen;
    memcpy_s(sysCom + offset, sizeof(sysCom) - offset, g_SmsHost[ptHandle->no].item[0].value, tmpLen);
    offset += tmpLen;
    sysCom[offset++] = dataLen - 4; /* 4:sizeof(COMM_IPCMSG_T) */
    memcpy_s(sysCom + offset, sizeof(sysCom) - offset, dataBuf + 4, dataLen - 4); /* 4:sizeof(COMM_IPCMSG_T) */
    offset += dataLen - 4;
    COMM_BUF_DEBUG(sysCom, offset, "SMS%u CommSmsSendDataProc send:", ptHandle->no);

    for (i = 0; i <= SMS_SEND_RETRY_CNT; i++)
    {
        PTHREAD_LOCK_SAFE(&g_fCommMsg);
        ret = task_syncmsg_safe(g_CommTaskSync, MSG_COMM_SMSSEND, sysCom, offset, MSG_MONITOR, recvbuf, sizeof(recvbuf), 10);
        PTHREAD_UNLOCK_SAFE(&g_fCommMsg);

        if (0 == ret)
        {
            if (pMsg->msglen == 0 || *pMsg->msg == FALSE)
            {
                COMM_FMT_DEBUG("SMS%u send fail,msglen=%u,msg=%u\n", ptHandle->no, pMsg->msglen, *pMsg->msg);
                continue;
            }
            else
            {
                COMM_FMT_DEBUG("SMS%u send success\n", ptHandle->no);
                break;
            }
        }

        COMM_FMT_DEBUG("SMS%u send fail,ret=%d\n", ptHandle->no, ret);
    }

    return 0;
}

uint32 CommSmsHostNumSave(COMM_SMS_HANDLE_T *ptHandle, TASK_MSG_t *pMsg, uint16 *offset)
{
    uint16  tmpOffset = *offset;
    uint32  valueSize = sizeof(g_SmsHost[ptHandle->no].item[0].value);
    uint8   hostNumLen = pMsg->msg[tmpOffset]; /* 主站号码长度 */

    if (hostNumLen == 0 || hostNumLen > valueSize || tmpOffset + 1 + hostNumLen >= pMsg->msglen) 
    {
        COMM_FMT_DEBUG("SMS%u hostnum length[%u] illegal, msglen[%u].\n", ptHandle->no, hostNumLen, pMsg->msglen);
        return 1;
    }

    tmpOffset++; /* 偏移到主站号码 */
    /* 主站号码校验 */
    if (g_SmsHost[ptHandle->no].nNum == 0)
    {
        COMM_BUF_DEBUG(&pMsg->msg[tmpOffset], hostNumLen, "SMS%u,CommSmsHostNumSave recv:", ptHandle->no);
        memcpy(g_SmsHost[ptHandle->no].item[0].value, &pMsg->msg[tmpOffset], hostNumLen);
        g_SmsHost[ptHandle->no].nNum++;
    }
    else
    {
        COMM_BUF_DEBUG(&pMsg->msg[tmpOffset], hostNumLen, "SMS%u,CommSmsHostNumSave recv:", ptHandle->no);
        /* 和已存储号码比较逻辑待补充 */
        memcpy(g_SmsHost[ptHandle->no].item[0].value, &pMsg->msg[tmpOffset], hostNumLen);
        //g_SmsHost[ptHandle->no].nNum++;
    }

    tmpOffset += hostNumLen;
    *offset = tmpOffset;
    return 0;
}

uint32 CommSmsFrameSave(COMM_SMS_HANDLE_T *ptHandle, TASK_MSG_t *pMsg, uint16 offset)
{
    //uint16   smsFrameLen = pMsg->msg[offset]; /* 短信帧长度 */
    uint16  hostOffset = 0; /* 通道偏移，待实现 */
    uint16  dataLen = 0;
    uint8 lenoffset;
    uint16 smsFrameLen = get_len_offset(&pMsg->msg[offset],&lenoffset);
    /* 短信帧校验 */
    if (smsFrameLen == 0 || smsFrameLen > SMS_FRAM_MAXLEN || offset + lenoffset + smsFrameLen > pMsg->msglen)
    {
        COMM_FMT_DEBUG("SMS%u smsFrameLen length[%u] illegal, offset[%u] msglen[%u].\n", ptHandle->no, smsFrameLen, offset, pMsg->msglen);
        return 1;
    }
    else
    {
        offset+=lenoffset;
        COMM_BUF_DEBUG(&pMsg->msg[offset], smsFrameLen, "SMS%u,CommSmsFrameSave recv:", ptHandle->no);

        dataLen = smsFrameLen;
#ifdef AREA_HUNAN
        if(sms_msg_unpack(ptHandle,&pMsg->msg[offset], dataLen)==0)
            return 0;
#endif
        /*给数据加上会话协议头部*/
        comm_add_channel(((0 == ptHandle->no) ? CHL_GSM_1 : CHL_GSM_2) + hostOffset, &pMsg->msg[offset], dataLen);
        dataLen += 4;
        COMM_BUF_DEBUG(&pMsg->msg[offset], dataLen, "SMS%u,CommSmsFrameSave comm_write_queue:", ptHandle->no);

        if (comm_write_queue(ptHandle->ptRecvList, &pMsg->msg[offset], dataLen) != 0)
        {
            COMM_FMT_DEBUG("SMS%u Msg Queue is full\n", ptHandle->no);
            return 1;
        }
    }

    return 0;
}

uint32 CommSmsRecvDataProc(COMM_SMS_HANDLE_T *ptHandle)
{
    uint8   sysCom[200] = {0};
    uint8   recvbuf[200] = {0};
    uint16  offset = 0;
    uint16  recvOffset = 1;
    int    ret = 0;
    TASK_MSG_t *pMsg = (TASK_MSG_t *)recvbuf;

    sysCom[offset++] = ptHandle->no + 1;

    PTHREAD_LOCK_SAFE(&g_fCommMsg);
    ret = task_syncmsg_safe(g_CommTaskSync, MSG_COMM_SMSRECV, sysCom, offset, MSG_MONITOR, recvbuf, sizeof(recvbuf), 10);
    PTHREAD_UNLOCK_SAFE(&g_fCommMsg);
#if 0
    static uint8 testflag = 0;
    uint8 payload[200] ={0}; 
    uint16 testoffset = 0;
    char *testsms = "cssdl 2 001.000.000.001:4000:cs@dl.vpdn.hn:hn123456";
    if(testflag == 0)
    {
        ret = 0;
        testflag = 1;
        payload[testoffset++] = 1;
        payload[testoffset++] = 1;
        payload[testoffset++] = 1;
        payload[testoffset++] = strlen(testsms);
        memcpy(&payload[testoffset],testsms,strlen(testsms));
        testoffset+=strlen(testsms);
        pMsg->msglen = testoffset;
        memcpy(pMsg->msg,payload,testoffset);
    }
#endif
    if (0 == ret)
    {
        if (pMsg->msglen <= 4)
        {
            COMM_FMT_TRACE("SMS%u no recv msg,msglen=%u,msg=%u\n", ptHandle->no, pMsg->msglen, *pMsg->msg);
            return 1;
        }
        else
        {
            COMM_BUF_DEBUG(&pMsg->msg[0], pMsg->msglen, "SMS%u CommSmsRecvDataProc recv:", ptHandle->no);
            if (CommSmsHostNumSave(ptHandle, pMsg, &recvOffset) != 0)
            {
                return 1;
            }

            if (CommSmsFrameSave(ptHandle, pMsg, recvOffset) != 0)
            {
                return 1;
            }

            COMM_FMT_DEBUG("SMS%u recv success\n", ptHandle->no);
        }
    }

    return 0;
}

#define SMS_SEND_TIMER_CNT 5 /* 短信发送定时器 */
#define SMS_RECV_TIMER_CNT 50 /* 短信接收定时器 */
/**********************************************************************
* @name      : comm_sms_entry
* @brief     ：短信端口消息收发线程
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2019-10-31
* @Update    :
**********************************************************************/
void *comm_sms_entry(void *pArg)
{
    uint8   i;
    uint8   status = SMS_CENTER_MANA;
    uint8   dataBuf[SMS_BUF_SIZE + 8] = {0};
    uint16  dataLen = 0;
    uint32  sendTimer = 0;
    uint32  recvTimer = 0;
    
    COMM_IPCMSG_T   *ptIPCMsg = NULL;
    CHANNEL_INFO_T  *ptChanInfo = 0;
    COMM_SMS_HANDLE_T *ptHandle = (COMM_SMS_HANDLE_T *)pArg;
    OOP_SMS_T         *ptSMS = &(ptHandle->tSMS);

    if(NULL == pArg)
    {
        return 0;
    }

    if(NULL != ptHandle->pAppReStart)
    {
        *ptHandle->pAppReStart = 0;
    }
    
    /* 清空队列 */
    comm_clean_queue(ptHandle->ptRecvList);
    comm_clean_queue(ptHandle->ptSendList);

    for(i = 0; i < OOP_MAX_HOST; i++)
    {
        ptHandle->masterid[i] = 0;
    }

    /* 设置短信中心号码 */
    ptHandle->bCenter = FALSE;
    if(0 != comm_set_smsarg(ptHandle->no + 1, ptSMS->centernum, ptHandle->smsType))
    {
        COMM_FMT_DEBUG("SMS%d send center msg failed\n", ptHandle->no);
        *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
        return 0;
    }
    memset(dataBuf, 0, dataLen);

    while(1)
    {
        *ptHandle->pRunFlag = 0;

        if(FALSE == ptHandle->bDiagOK || FALSE == ptHandle->bGetCfg || TRUE == *ptHandle->pAppReStart 
           || TRUE == ptHandle->bCfgChange)
        {
            for(i = 0; i < OOP_MAX_HOST; i++)
            {
                if(ptHandle->masterid[i] > 0)
                {
                    /* 关闭所有通道 */
                    ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                    ptIPCMsg->id = ptHandle->masterid[i];
                    ptIPCMsg->protocol = PROT_CLOSE_CHANNLE;
                    dataLen = sizeof(COMM_IPCMSG_T);
                    if(0 != comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen))
                    {
                        COMM_FMT_TRACE("Msg Queue is full\n");
                    }
                    ptHandle->masterid[i] = 0;
                    COMM_FMT_DEBUG("SMS%d channel id %d close\n", ptHandle->no , ptIPCMsg->id);
                    memset(dataBuf, 0, dataLen);
                    usleep(10000);
                }
            }
            if(FALSE == ptHandle->bDiagOK || FALSE == ptHandle->bGetCfg || TRUE == *ptHandle->pAppReStart)
            {
                COMM_FMT_DEBUG("SMS%d need stop\n", ptHandle->no);
                *ptHandle->pRunFlag = COMM_TRHEAD_FLAG_MUX;
                return 0;
            }
            else if(TRUE == ptHandle->bCfgChange)
            {
                COMM_FMT_DEBUG("SMS%d change master\n", ptHandle->no);
                ptHandle->bCfgChange = FALSE;
                status = SMS_MASTER_MANA;
                break;
            }
        }
        switch(status)
        {
            case SMS_CENTER_MANA:
            {
                if(TRUE == ptHandle->bCenter)
                {
                    status = SMS_MASTER_MANA;
                }
                break;
            }
            case SMS_MASTER_MANA:
            {
                if(0 == ptSMS->hostnum.nNum)
                {
                    break;
                }
                dataLen = sizeof(COMM_IPCMSG_T) + sizeof(CHANNEL_INFO_T);
                ptIPCMsg = (COMM_IPCMSG_T *)dataBuf;
                ptIPCMsg->protocol = PROT_OPEN_CHANNLE;
                ptChanInfo = (CHANNEL_INFO_T *)ptIPCMsg->msg;
                ptChanInfo->workmode = 2;
                ptChanInfo->medium = COMM_MEDIA_GSM;
                ptChanInfo->connType = 0;
                ptChanInfo->bFirst = TRUE;
                ptChanInfo->heart = 180;
                ptChanInfo->dailcnt = 3;
                ptChanInfo->dailtime = 60;
                ptChanInfo->ip = 0;
                ptChanInfo->port = 0;

                for(i = 0; i < ptSMS->hostnum.nNum; i++)
                {
                    ptHandle->masterid[i] = (0 == ptHandle->no ? CHL_GSM_1 : CHL_GSM_2) + i;

                    ptIPCMsg->id = ptHandle->masterid[i];
                    memcpy(ptChanInfo->SMSMaster, ptSMS->hostnum.item[i].value, 32);
                    
                    comm_write_queue(ptHandle->ptRecvList, dataBuf, dataLen);
                    COMM_FMT_DEBUG("SMS%d id %d open, center NO %s\n", ptHandle->no, ptIPCMsg->id, ptChanInfo->SMSMaster);
                }
                status = SMS_SEND_DATA;
                break;
            }
            case SMS_SEND_DATA:
            {
                sendTimer++;
 
                if (sendTimer % SMS_SEND_TIMER_CNT == 0)
                {
                    CommSmsSendDataProc(ptHandle);
                }

                status = SMS_RECV_DATA;
                break;
            }
            case SMS_RECV_DATA:
            {
                recvTimer++;

                if (recvTimer % SMS_RECV_TIMER_CNT == 0)
                {
                    CommSmsRecvDataProc(ptHandle);
                }

                status = SMS_SEND_DATA;
                break;
            }
        }

        usleep(100000);
        if(gdelay != 0)
        {
            usleep(gdelaytime);
        }
    }
    return 0;
}

