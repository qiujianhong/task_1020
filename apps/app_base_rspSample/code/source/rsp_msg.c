/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块主线程功能
* @date：    2022-3-20
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "appmsg_split.h"
#include "framecomm.h"
#include "task_base.h"

#include "rsp_mqtt.h"
#include "rsp_data.h"
#include "rsp_msg.h"
#include "rsp_map.h"


/**********************************************************************
* @name      : rsp_recv_time_event
* @brief     ：收到对时消息
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-6-17
* @Update    :
**********************************************************************/
int rsp_recv_time_event(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;

    RSP_FMT_DEBUG("系统对时\n");
    pRspData->tRspDev.bSetTime = TRUE;

    return 0;
}

/**********************************************************************
* @name      : rsp_recv_rmt_req
* @brief     ：获取遥信变位状态请求
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_recv_rmt_req(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8            msgbuf[2048] = {0};
    MESSAGE_INFO_T  *ptMsgNew = (MESSAGE_INFO_T *)msgbuf;
    uint8           *temp = NULL;
    uint16           msglen = 0;
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    char             topic[256] = {0};
    uint8            i = 0;
    uint8            j = 0;
    uint8            sum = 0;
    uint8            storeST[RSP_PORT_MAX] = {0};
    uint8            config[RSP_PORT_MAX] = {0};
    RSP_REPORT_DATA_T tNode[3];
    uint16            noindex = 0;
    
    if(pInMsg->msglen > 1)
    {
        return -1;
    }
    
    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;

    ptMsgNew->label = pInMsg->label;
    ptMsgNew->index = pInMsg->index;

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    temp = ptMsgNew->playload;
    MQTTMSG_FMT_DEBUG("%s获取设备%d遥信状态\n", pInMsg->souraddr, pInMsg->playload[0]);

    memset(tNode, 0, 3 * sizeof(RSP_REPORT_DATA_T));
    if(0 == pInMsg->playload[0])
    {
        PTHREAD_LOCK_SAFE(&pRspData->tRspDev.flock);
        sum = pRspData->tRspDev.sum;
        memcpy(config, pRspData->tRspDev.config, RSP_PORT_MAX);
        memcpy(storeST, pRspData->tRspDev.storeState, RSP_PORT_MAX);
        tNode[0] = pRspData->tRspDev.curNode;
        tNode[1] = pRspData->tRspDev.lastNode;
        tNode[2] = pRspData->tRspDev.oldNode;
        PTHREAD_UNLOCK_SAFE(&pRspData->tRspDev.flock);
    }
    else if(1 == pInMsg->playload[0])
    {
        if(0 != pRspData->tRspModel[0].slot)
        {
            PTHREAD_LOCK_SAFE(&pRspData->tRspModel[0].flock);
            sum = pRspData->tRspModel[0].sum;
            memcpy(config, pRspData->tRspModel[0].config, RSP_PORT_MAX);
            memcpy(storeST, pRspData->tRspModel[0].storeState, RSP_PORT_MAX);
            tNode[0] = pRspData->tRspModel[0].curNode;
            tNode[1] = pRspData->tRspModel[0].lastNode;
            tNode[2] = pRspData->tRspModel[0].oldNode;
            PTHREAD_UNLOCK_SAFE(&pRspData->tRspModel[0].flock);
        }
    }
    else if(2 == pInMsg->playload[0])
    {
        if(0 != pRspData->tRspModel[1].slot)
        {
            PTHREAD_LOCK_SAFE(&pRspData->tRspModel[1].flock);
            sum = pRspData->tRspModel[1].sum;
            memcpy(config, pRspData->tRspModel[1].config,RSP_PORT_MAX);
            memcpy(storeST, pRspData->tRspModel[1].storeState, RSP_PORT_MAX);
            tNode[0] = pRspData->tRspModel[1].curNode;
            tNode[1] = pRspData->tRspModel[1].lastNode;
            tNode[2] = pRspData->tRspModel[1].oldNode;
            PTHREAD_UNLOCK_SAFE(&pRspData->tRspModel[1].flock);
        }
    }
    else if(0xFF == pInMsg->playload[0])
    {
        PTHREAD_LOCK_SAFE(&pRspData->flock);
        sum = pRspData->curALLRmt.sum;
        memcpy(config, pRspData->curALLRmt.config, RSP_PORT_MAX);
        memcpy(storeST, pRspData->curALLRmt.ST, RSP_PORT_MAX);
        tNode[0] = pRspData->curALLRmt;
        tNode[1] = pRspData->lastALLRmt;
        tNode[2] = pRspData->oldALLRmt;
        PTHREAD_UNLOCK_SAFE(&pRspData->flock);
    }
    
    if(sum == 0)
    {
        temp[msglen++] = 0;
        temp[msglen++] = 0;
        temp[msglen++] = 0;
    }
    else
    {
        uint8  dre = 7;
        temp[msglen++] = sum;
        for(i = 0; i < sum && i < RSP_PORT_MAX; i++)
        {
            if(8 == i)
            {
                msglen++;
                dre = 15;
            }
            if(0 != storeST[i] && 0 == config[i])
            {
                temp[msglen] |= (0x1 << (dre - i));
            }
        }
        msglen++;
        noindex = msglen;
        temp[noindex] = 0;
        msglen++;

        for(i = 0; i < 3; i++)
        {
            if(0 == tNode[i].id)
            {
                break;
            }
            temp[noindex]++;
            
            temp[msglen++] = tNode[i].sum;
            if(tNode[i].sum > 0)
            {
                dre = 7;
                for(j = 0; j < tNode[i].sum; j++)
                {
                    if(8 == j)
                    {
                        msglen++;
                        dre = 15;
                    }
                    if(0 != tNode[i].ST[j] && 0 == tNode[i].config[j])
                    {
                        temp[msglen] |= (0x1 << (dre - j));
                    }
                }
                msglen++;
            }
            
            temp[msglen++] = tNode[i].sum;
            if(tNode[i].sum > 0)
            {
                dre = 7;
                for(j = 0; j < tNode[i].sum; j++)
                {
                    if(8 == j)
                    {
                        msglen++;
                        dre = 15;
                    }
                    if(0 != tNode[i].CT[j] && 0 == tNode[i].config[j])
                    {
                        temp[msglen] |= (0x1 << (dre - j));
                    }
                }
                msglen++;
            }
            
            temp[msglen++] = tNode[i].sum;
            if(tNode[i].sum > 0)
            {
                for(j = 0; j < tNode[i].sum; j++)
                {
                    MEMCPY_R(temp + msglen, &(tNode[i].chgTime[j].year), 2);
                    msglen += 2;
                    temp[msglen++] = tNode[i].chgTime[j].month;
                    temp[msglen++] = tNode[i].chgTime[j].mday;
                    temp[msglen++] = tNode[i].chgTime[j].wday;
                    temp[msglen++] = tNode[i].chgTime[j].hour;
                    temp[msglen++] = tNode[i].chgTime[j].minute;
                    temp[msglen++] = tNode[i].chgTime[j].second;
                    MEMCPY_R(temp + msglen, &(tNode[i].chgTime[j].msec), 2);
                    msglen += 2;
                }
            }
        }
        
    }
    ptMsgNew->msglen = msglen;

    sprintf(topic, "%s/%s", RSP_APP_NAME, pInMsg->souraddr);
    return rsp_send_mqtt_msg(topic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : rsp_recv_pluse_req
* @brief     ：获取脉冲统计请求
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_recv_pluse_req(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8            msgbuf[512] = {0};
    MESSAGE_INFO_T  *ptMsgNew = (MESSAGE_INFO_T *)msgbuf;
    uint8           *temp = NULL;
    uint16           msglen = 0;
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    char             topic[256] = {0};
    RSP_PULSE_A_T    tPulseAll;
    uint32           i = 0;
    uint32           j = 0;

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;

    ptMsgNew->label = pInMsg->label;
    ptMsgNew->index = pInMsg->index;

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    temp = ptMsgNew->playload;
    MQTTMSG_FMT_DEBUG("%s获取设备%d脉冲计数\n", pInMsg->souraddr, pInMsg->playload[0]);
    memset(&tPulseAll, 0, sizeof(RSP_PULSE_A_T));
    if(0 == pInMsg->playload[0])
    {
        PTHREAD_LOCK_SAFE(&pRspData->tRspDev.flock);
        tPulseAll = pRspData->tRspDev.tPulseAll;
        PTHREAD_UNLOCK_SAFE(&pRspData->tRspDev.flock);
    }
    else if(1 == pInMsg->playload[0])
    {
        PTHREAD_LOCK_SAFE(&pRspData->tRspModel[0].flock);
        tPulseAll = pRspData->tRspModel[0].tPulseAll;
        PTHREAD_UNLOCK_SAFE(&pRspData->tRspModel[0].flock);
    }
    else if(2 == pInMsg->playload[0])
    {
        PTHREAD_LOCK_SAFE(&pRspData->tRspModel[1].flock);
        tPulseAll = pRspData->tRspModel[1].tPulseAll;
        PTHREAD_UNLOCK_SAFE(&pRspData->tRspModel[1].flock);
    }
    else if(0xFF == pInMsg->playload[0])
    {
        uint8 allnum = 0;
        memset(&tPulseAll, 0, sizeof(RSP_PULSE_A_T));
        
        PTHREAD_LOCK_SAFE(&pRspData->tRspDev.flock);
        for(i = 0; i < pRspData->tRspDev.tPulseAll.num; i++)
        {
            if(0 == pRspData->tRspDev.tPulseAll.validFlag[i])
            {
                continue;
            }
            tPulseAll.validFlag[allnum] = 1;
            tPulseAll.nCount[allnum] = pRspData->tRspDev.tPulseAll.nCount[i];
            tPulseAll.nTimeH[allnum] = pRspData->tRspDev.tPulseAll.nTimeH[i];
            tPulseAll.nTimeL[allnum] = pRspData->tRspDev.tPulseAll.nTimeL[i];
            tPulseAll.startTime[allnum] = pRspData->tRspDev.tPulseAll.startTime[i];
            allnum++;
        }
        PTHREAD_UNLOCK_SAFE(&pRspData->tRspDev.flock);

        for(i = 0; i < 2; i++)
        {
            PTHREAD_LOCK_SAFE(&pRspData->tRspModel[i].flock);
            for(j = 0; j < pRspData->tRspModel[i].tPulseAll.num; j++)
            {
                if(0 == pRspData->tRspModel[i].tPulseAll.validFlag[j])
                {
                    continue;
                }
                tPulseAll.validFlag[allnum] = 1;
                tPulseAll.nCount[allnum] = pRspData->tRspModel[i].tPulseAll.nCount[j];
                tPulseAll.nTimeH[allnum] = pRspData->tRspModel[i].tPulseAll.nTimeH[j];
                tPulseAll.nTimeL[allnum] = pRspData->tRspModel[i].tPulseAll.nTimeL[j];
                tPulseAll.startTime[allnum] = pRspData->tRspModel[i].tPulseAll.startTime[j];
                allnum++;
            }
            PTHREAD_UNLOCK_SAFE(&pRspData->tRspModel[i].flock);
        }
        tPulseAll.num = allnum;
    }
    temp[msglen++] = tPulseAll.num;
    for(i = 0; i < tPulseAll.num; i++)
    {
        MEMCPY_R(temp + msglen, &(tPulseAll.nCount[i]), 4);
        msglen += 4;
        MEMCPY_R(temp + msglen, &(tPulseAll.nTimeH[i]), 4);
        msglen += 4;
        MEMCPY_R(temp + msglen, &(tPulseAll.nTimeL[i]), 2);
        msglen += 2;

        MEMCPY_R(temp + msglen, &(tPulseAll.startTime[i].year), 2);
        msglen += 2;
        temp[msglen++] = tPulseAll.startTime[i].month;
        temp[msglen++] = tPulseAll.startTime[i].mday;
        temp[msglen++] = tPulseAll.startTime[i].wday;
        temp[msglen++] = tPulseAll.startTime[i].hour;
        temp[msglen++] = tPulseAll.startTime[i].minute;
        temp[msglen++] = tPulseAll.startTime[i].second;
        MEMCPY_R(temp + msglen, &(tPulseAll.startTime[i].msec), 2);
        msglen += 2;
    }
    ptMsgNew->msglen = msglen;

    sprintf(topic, "%s/%s", RSP_APP_NAME, pInMsg->souraddr);
    return rsp_send_mqtt_msg(topic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : rsp_recv_config_req
* @brief     ：遥信脉冲功能配置请求
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_recv_config_req(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8            msgbuf[312] = {0};
    MESSAGE_INFO_T  *ptMsgNew = (MESSAGE_INFO_T *)msgbuf;
    uint8           *temp = pInMsg->playload;
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    char             topic[256] = {0};
    char             strtemp[256] = {0};
    uint16           offset = 0;
    uint8            i = 0;
    uint8            no = 0;
    uint8            cfg = 0;
    uint8            config[RSP_PORT_MAX] = {0};
    uint8            num = 0;
    uint8            retsult = FALSE;

    if(3 == pInMsg->msglen)
    {
        no = *temp;
        num = *(temp + 1);
        cfg = *(temp + 2);
        for(i = 0; i < num && i < RSP_PORT_MAX; i++)
        {
            config[i] = ((cfg >> (7 - i)) & 0x01);
            offset += sprintf(strtemp + offset, " %s", 0 == config[i] ? "遥信" : "脉冲");
        }
        RSP_FMT_DEBUG("%s设置设备%d配置(%s)\n", pInMsg->souraddr, no, strtemp);
    }
    if(0 == no)
    {
        if(TRUE == pRspData->tRspDev.bShare)
        {
            PTHREAD_LOCK_SAFE(&pRspData->tRspDev.flock);
            memcpy(pRspData->tRspDev.config, config, RSP_PORT_MAX);
            PTHREAD_UNLOCK_SAFE(&pRspData->tRspDev.flock);

            rsp_write_config(1, config, RSP_PORT_MAX);
            if(FALSE == pRspData->bIIC)
            {
                /*  通过串口设置*/
                pRspData->tRspDev.configStatus = 1;
                i = 0;
                while(i <= 200)
                {
                    if(0 == pRspData->tRspDev.configStatus)
                    {
                        retsult = TRUE;
                        break;
                    }
                    else if(2 == pRspData->tRspDev.configStatus)
                    {
                        break;
                    }
                    usleep(20000);
                    i++;
                }
                RSP_FMT_DEBUG("%s设置设备%d配置%s\n", pInMsg->souraddr, no, 
                                                      TRUE == retsult ? "成功" : (i >= 200 ? "超时" : "失败"));
            }
            else
            {
                retsult = TRUE;
            }
            
        }
        else
        {
            RSP_FMT_DEBUG("%s设置配置无效(遥信脉冲不共享)\n", pInMsg->souraddr);
            retsult = 1;
        }
    }
    else if(1 == no || 2 == no)
    {
        if(0 != pRspData->tRspModel[no - 1].slot && TRUE == pRspData->tRspModel[no - 1].bSet)
        {
            PTHREAD_LOCK_SAFE(&pRspData->tRspModel[no - 1].flock);
            memcpy(pRspData->tRspModel[no - 1].config, config, RSP_PORT_MAX);
            PTHREAD_UNLOCK_SAFE(&pRspData->tRspModel[no - 1].flock);

            /*  通过串口设置*/
            pRspData->tRspModel[no - 1].configStatus = 1;
            i = 0;
            while(i <= 200)
            {
                if(0 == pRspData->tRspModel[no - 1].configStatus)
                {
                    retsult = TRUE;
                    break;
                }
                else if(2 == pRspData->tRspModel[no - 1].configStatus)
                {
                    break;
                }
                usleep(20000);
                i++;
            }
            rsp_write_config(no, config, RSP_PORT_MAX);
            RSP_FMT_DEBUG("设备%s设置设备%d配置%s\n", pInMsg->souraddr, no, 
                                                  TRUE == retsult ? "成功" : (i >= 200 ? "超时" : "失败"));
        }
        else
        {
            retsult = TRUE;
        }
    }
    
    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;

    ptMsgNew->label = pInMsg->label;
    ptMsgNew->index = pInMsg->index;

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    ptMsgNew->msglen = 1;
    ptMsgNew->playload[0] = retsult;

    sprintf(topic, "%s/%s", RSP_APP_NAME, pInMsg->souraddr);
    return rsp_send_mqtt_msg(topic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : rsp_recv_shake_req
* @brief     ：遥信脉冲时间配置请求
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_recv_shake_req(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8            msgbuf[312] = {0};
    MESSAGE_INFO_T  *ptMsgNew = (MESSAGE_INFO_T *)msgbuf;
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    char             topic[256] = {0};
    uint8            retsult = FALSE;
    uint16           shakeTime = 0;
    uint8            i = 0;
    uint8            no = 0;

    if(3 == pInMsg->msglen)
    {
        no = *pInMsg->playload;
        MEMCPY_R(&shakeTime, pInMsg->playload + 1, 2);
        if(shakeTime < 10 || shakeTime > 60000)
        {
            RSP_FMT_DEBUG("%s设置设备%d防抖时间%dms 无效\n", pInMsg->souraddr, no, shakeTime);
            goto end; 
        }
    }
    if(0 == no)
    {
        PTHREAD_LOCK_SAFE(&pRspData->tRspDev.flock);
        pRspData->tRspDev.shakeTime = shakeTime;
        PTHREAD_UNLOCK_SAFE(&pRspData->tRspDev.flock);
        rsp_write_shaketime(1, pRspData->tRspDev.shakeTime);
        if(FALSE == pRspData->bIIC)
        {
            /*  通过串口设置*/
            pRspData->tRspDev.shakeStatus = 1;
            i = 0;
            while(i <= 200)
            {
                if(0 == pRspData->tRspDev.shakeStatus)
                {
                    retsult = TRUE;
                    break;
                }
                else if(2 == pRspData->tRspDev.shakeStatus)
                {
                    break;
                }
                usleep(20000);
                i++;
            }
            RSP_FMT_DEBUG("%s设置设备%d防抖时间%s\n", pInMsg->souraddr, no, 
                                                      TRUE == retsult ? "成功" : (i >= 200 ? "超时" : "失败"));
        }
        else
        {
            retsult = TRUE;
        }
    }
    else if(1 == no || 2 == no)
    {
        if(0 != pRspData->tRspModel[no - 1].slot)
        {
            PTHREAD_LOCK_SAFE(&pRspData->tRspModel[no - 1].flock);
            pRspData->tRspModel[no - 1].shakeTime = shakeTime;
            PTHREAD_UNLOCK_SAFE(&pRspData->tRspModel[no - 1].flock);

            rsp_write_shaketime(no, shakeTime);
            /*  通过串口设置*/
            pRspData->tRspModel[no - 1].shakeStatus = 1;
            i = 0;
            while(i <= 200)
            {
                if(0 == pRspData->tRspModel[no - 1].shakeStatus)
                {
                    retsult = TRUE;
                    break;
                }
                else if(2 == pRspData->tRspModel[no - 1].shakeStatus)
                {
                    break;
                }
                usleep(20000);
                i++;
            }
            RSP_FMT_DEBUG("%s设置设备%d防抖时间(%dms)%s\n", pInMsg->souraddr, no, shakeTime,
                                                  TRUE == retsult ? "成功" : (i >= 200 ? "超时" : "失败"));
        }
        else
        {
            retsult = TRUE;
        }
    }

end:
    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;

    ptMsgNew->label = pInMsg->label;
    ptMsgNew->index = pInMsg->index;

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    ptMsgNew->msglen = 1;
    ptMsgNew->playload[0] = retsult;

    sprintf(topic, "%s/%s", RSP_APP_NAME, pInMsg->souraddr);
    return rsp_send_mqtt_msg(topic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : rsp_recv_get_485_can_req
* @brief     ：485 can配置查询请求
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_recv_get_485_can_req(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8            msgbuf[312] = {0};
    MESSAGE_INFO_T  *ptMsgNew = (MESSAGE_INFO_T *)msgbuf;
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    char             topic[256] = {0};
    uint8            retsult = FALSE;
    uint8            i = 0;

    /*  通过串口查询*/
    pRspData->tRspDev.get485Status = 1;
    pRspData->tRspDev.get485_can = 255;
    i = 0;
    while(i <= 200)
    {
        if(0 == pRspData->tRspDev.get485Status)
        {
            retsult = TRUE;
            break;
        }
        else if(2 == pRspData->tRspDev.get485Status)
        {
            break;
        }
        usleep(20000);
        i++;
    }
    RSP_FMT_DEBUG("%s查询485-2口工作模式%s\n", pInMsg->souraddr, 
                                               TRUE == retsult ? "成功" : (i >= 200 ? "超时" : "失败"));

    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;

    ptMsgNew->label = pInMsg->label;
    ptMsgNew->index = pInMsg->index;

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    ptMsgNew->msglen = 1;
    ptMsgNew->playload[0] = pRspData->tRspDev.get485_can;

    sprintf(topic, "%s/%s", RSP_APP_NAME, pInMsg->souraddr);
    return rsp_send_mqtt_msg(topic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : rsp_recv_set_485_can_req
* @brief     ：485 can配置查询请求
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-25
* @Update    :
**********************************************************************/
int rsp_recv_set_485_can_req(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8            msgbuf[312] = {0};
    MESSAGE_INFO_T  *ptMsgNew = (MESSAGE_INFO_T *)msgbuf;
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;
    char             topic[256] = {0};
    uint8            retsult = FALSE;
    uint8            i = 0;

    if(pInMsg->playload[0] > 1)
    {
        goto end;
    }
    /*  通过串口设置*/
    pRspData->tRspDev.set485Status = 1;
    pRspData->tRspDev.set485_can = pInMsg->playload[0];
    i = 0;
    while(i <= 200)
    {
        if(0 == pRspData->tRspDev.set485Status)
        {
            retsult = TRUE;
            break;
        }
        else if(2 == pRspData->tRspDev.set485Status)
        {
            break;
        }
        usleep(20000);
        i++;
    }
    RSP_FMT_DEBUG("%s设置485-2口工作模式%s\n", pInMsg->souraddr, 
                                               TRUE == retsult ? "成功" : (i >= 200 ? "超时" : "失败"));

end:
    ptMsgNew->rpm = 0;
    ptMsgNew->priority = pInMsg->priority;

    ptMsgNew->label = pInMsg->label;
    ptMsgNew->index = pInMsg->index;

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptMsgNew->destlen = pInMsg->sourlen;

    ptMsgNew->IID = pInMsg->IID;
    ptMsgNew->IOP = pInMsg->IOP;

    ptMsgNew->msglen = 1;
    ptMsgNew->playload[0] = pRspData->tRspDev.get485_can;

    sprintf(topic, "%s/%s", RSP_APP_NAME, pInMsg->souraddr);
    return rsp_send_mqtt_msg(topic, ptMsgNew, 0);
}



RSP_MSGFUN_T g_tMsgFunc[] = 
{/*  IID,                    IOP,                         rpm, 消息处理函数 */
    {MESSAGE_IID_COMMON,     COMMON_IOP_SYSTEM_TIMING,    1,   rsp_recv_time_event},
    {MESSAGE_IID_COMMON,     COMMON_IOP_HERTBEAT,         1,   rsp_heart_bear},
    {MESSAGE_IID_COMMON,     COMMON_IOP_VERSION,          1,   rsp_version_msg},
    {MESSAGE_IID_COMMON,     COMMON_IOP_VERSION,          0,   rsp_mapversion_msg},

    {MESSAGE_IID_RSPSAMPLE,  RSPSAMPLE_IOP_RMT_STATE,     1,   rsp_recv_rmt_req},
    {MESSAGE_IID_RSPSAMPLE,  RSPSAMPLE_IOP_PULSE_STATIC,  1,   rsp_recv_pluse_req},
    {MESSAGE_IID_RSPSAMPLE,  RSPSAMPLE_IOP_CONFIGURE,     1,   rsp_recv_config_req},
    {MESSAGE_IID_RSPSAMPLE,  RSPSAMPLE_IOP_SHAKE_TIME,    1,   rsp_recv_shake_req},
    {MESSAGE_IID_RSPSAMPLE,  RSPSAMPLE_IOP_WORKMODE_GET,  1,   rsp_recv_get_485_can_req},
    {MESSAGE_IID_RSPSAMPLE,  RSPSAMPLE_IOP_WORKMODE_SET,  1,   rsp_recv_set_485_can_req},

    {MESSAGE_IID_MODULE,     MODULE_IOP_STATUS_EVENT,     1,   rsp_map_event},
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_ALL,          0,   rsp_mapall_msg},

};
uint8 g_nMsgNum = sizeof(g_tMsgFunc)/sizeof(RSP_MSGFUN_T);

/**********************************************************************
* @name      : rsp_mqttproc_init
* @brief     ：mqtt消息处理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_mqttproc_init(void *pPublic, void **ppPrivate)
{
    RSP_MSGHANDLE_T *ptMsgData = NULL;

    ptMsgData = (RSP_MSGHANDLE_T *)malloc(sizeof(RSP_MSGHANDLE_T));
    if(NULL == ptMsgData)
    {
        MQTTMSG_FMT_DEBUG("rsp_mqttproc_init malloc failed\n");
        return -1;
    }
    
    memset(ptMsgData, 0, sizeof(RSP_MSGHANDLE_T));

    /* 先发送一次APP注册消息 */
    rsp_login_to_smiOS();
    *ppPrivate = ptMsgData;
    return 0;
}

/**********************************************************************
* @name      : rsp_mqttmsg_proc
* @brief     ：mqtt消息处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int         ret = 0;
    int         recvlen = 0;
    uint8       i;
    RSP_MSGHANDLE_T *ptHandle = (RSP_MSGHANDLE_T*)pPrivate;
    RSP_DATA_T      *ptRspData = (RSP_DATA_T*)pPublic;
    MESSAGE_INFO_T  *pMsgInfo = NULL;
    struct timespec  curtime = {0};

    /* 1 收消息 */
    recvlen = rsp_recv_mqtt_msg(ptHandle->topic, 256, ptHandle->recvBuf, RSP_MQTT_MSG_LEN);
    if(recvlen > 0)
    {
        ret = appmsg_split(ptHandle->recvBuf, (uint16)recvlen, &pMsgInfo);
        if(0 == ret && NULL != pMsgInfo)
        {
            /* 用于线程维护日志记录 */
            TASK_SET_USERLOG(pArg, "mqtt msg from %s iid %04x, iop %04x", pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
            MQTTMSG_BUF_TRACE(ptHandle->recvBuf, recvlen, "Recv msg from %s, iid 0x%x, iop 0x%x", 
                              pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
            if(0 == strcmp(pMsgInfo->souraddr, RSP_APP_NAME))
            {
                /* 消除自发自收 */
                RSP_FMT_DEBUG("Revc msg form %s is the same name, iid %d, iop %d\n",
                              pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
                goto freemsg;
            }

            /* 注册应答 */
            if(MESSAGE_IID_SYSTEM == pMsgInfo->IID && SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
            {
                if(FALSE == ptHandle->bReg)
                {
                    ptHandle->bReg = TRUE;
                    printf("rspSamlpe register success, find desktopGui and mapManager from smiOS....\n");
                    RSP_FMT_DEBUG("遥信脉冲注册成功\n");
                    BASE_TASK_SET_OK(pArg); //基础任务完成准备工作
                    goto freemsg;
                }
            }

            for(i = 0; i < g_nMsgNum; i++)
            {
                if(g_tMsgFunc[i].IID == pMsgInfo->IID && g_tMsgFunc[i].IOP == pMsgInfo->IOP && g_tMsgFunc[i].rpm == pMsgInfo->rpm)
                {
                    if(g_tMsgFunc[i].pProcMsg)
                    {
                        g_tMsgFunc[i].pProcMsg(pMsgInfo, (void *)ptHandle, (void *)ptRspData);
                        goto freemsg;
                    }
                }
            }
            
            if(i == g_nMsgNum && !(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_ERRMSG == pMsgInfo->IOP))
            {
                rsp_send_errmsg(pMsgInfo, -1);
            }
freemsg:
            free(pMsgInfo);
            pMsgInfo = NULL;
        }
    }

    /* 2. 定时处理 */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(curtime.tv_sec - ptHandle->timeflag < 5)
    {
        return 0;
    }
    ptHandle->timeflag = curtime.tv_sec;
    if(FALSE == ptHandle->bReg)
    {
        rsp_login_to_smiOS();
    }
    
    return 0;
}

/**********************************************************************
* @name      : rsp_report_init
* @brief     ：消息上报处理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_report_init(void *pPublic, void **ppPrivate)
{
    RSP_DATA_T      *pRspData = (RSP_DATA_T*)pPublic;

    *ppPrivate = &(pRspData->reportList);
    return 0;
}

/**********************************************************************
* @name      : rsp_report_proc
* @brief     ：消息上报处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2022-3-20
* @Update    :
**********************************************************************/
int rsp_report_proc(void *pArg, void *pPublic, void *pPrivate)
{
    RSP_REPORT_LIST_T   *pList = (RSP_REPORT_LIST_T*)pPrivate;
    RSP_DATA_T      *ptRspData = (RSP_DATA_T*)pPublic;
    RSP_REPORT_DATA_T   tData;
    RSP_REPORT_DATA_T AllData;
    int                  ret = 0;
    char         uTopic[256] = {0};
    uint8           buf[512] = {0};
    uint16            msglen = 0;
    uint8              *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;
    uint8                  i = 0;
    char       showbuf[1024] = {0};
    uint16            offset = 0;
    uint8                dre = 7;
    
    /* 构造返回消息 */
    ptMsgNew->rpm = 1;
    ptMsgNew->priority = 0;
    ptMsgNew->label = 0;

    ptMsgNew->sourlen = strlen(RSP_APP_NAME);
    memcpy(ptMsgNew->souraddr, RSP_APP_NAME, ptMsgNew->sourlen);

    ptMsgNew->destlen = strlen("Broadcast");
    memcpy(ptMsgNew->destaddr, "Broadcast", ptMsgNew->destlen);

    ptMsgNew->IID = MESSAGE_IID_RSPSAMPLE;
    ptMsgNew->IOP = RSPSAMPLE_IOP_RMT_EVENT;

    temp = ptMsgNew->playload;
    sprintf(uTopic, "%s/Broadcast/%04X/%04X", RSP_APP_NAME, MESSAGE_IID_RSPSAMPLE, RSPSAMPLE_IOP_RMT_EVENT);

    while(0 == rsp_read_queue(pList, &tData))
    {
        PTHREAD_LOCK_SAFE(&ptRspData->flock);
        
        memcpy(&AllData, &ptRspData->curALLRmt, sizeof(RSP_REPORT_DATA_T));
        AllData.id = 0xFF;
        if(0 == tData.id)
        {
            for(i = 0; i < tData.sum && i < RSP_PORT_MAX; i++)
            {
                AllData.config[i] = tData.config[i];
                AllData.ST[i] = tData.ST[i];
                AllData.CT[i] = tData.CT[i];
                AllData.chgTime[i] = tData.chgTime[i];
                if(AllData.sum < i + 1)
                {
                    AllData.sum = i + 1;
                }
            }
        }
        else if(1 == tData.id)
        {
            for(i = 0; i < tData.sum && i < RSP_PORT_MAX; i++)
            {
                AllData.config[ptRspData->tRspDev.sum + i] = tData.config[i];
                AllData.ST[ptRspData->tRspDev.sum +i] = tData.ST[i];
                AllData.CT[ptRspData->tRspDev.sum +i] = tData.CT[i];
                AllData.chgTime[ptRspData->tRspDev.sum +i] = tData.chgTime[i];
                if(AllData.sum < ptRspData->tRspDev.sum + i + 1)
                {
                    AllData.sum = ptRspData->tRspDev.sum + i + 1;
                }
            }
        }
        else if(2 == tData.id)
        {
            for(i = 0; i < tData.sum && i < RSP_PORT_MAX; i++)
            {
                AllData.config[ptRspData->tRspDev.sum + ptRspData->tRspModel[0].sum + i] = tData.config[i];
                AllData.ST[ptRspData->tRspDev.sum + ptRspData->tRspModel[0].sum +i] = tData.ST[i];
                AllData.CT[ptRspData->tRspDev.sum + ptRspData->tRspModel[0].sum +i] = tData.CT[i];
                AllData.chgTime[ptRspData->tRspDev.sum + ptRspData->tRspModel[0].sum +i] = tData.chgTime[i];
                if(AllData.sum < ptRspData->tRspDev.sum + ptRspData->tRspModel[0].sum + i + 1)
                {
                    AllData.sum = ptRspData->tRspDev.sum + ptRspData->tRspModel[0].sum + i + 1;
                }
            }
        }

        memcpy(&ptRspData->oldALLRmt, &ptRspData->lastALLRmt, sizeof(RSP_REPORT_DATA_T));
        memcpy(&ptRspData->lastALLRmt, &ptRspData->curALLRmt, sizeof(RSP_REPORT_DATA_T));
        memcpy(&ptRspData->curALLRmt, &AllData, sizeof(RSP_REPORT_DATA_T));
        PTHREAD_UNLOCK_SAFE(&ptRspData->flock);

        if(FALSE == tData.bReport)
        {
            continue;
        }
        
        ptMsgNew->index = rsp_get_next_index();
        
        memset(temp, 0, 200);
        msglen = 0;
        
        temp[msglen++] = AllData.id;
        
        temp[msglen++] = AllData.sum;
        dre = 7;
        for(i = 0; i < AllData.sum && i < RSP_PORT_MAX; i++)
        {
            if(8 == i)
            {
                msglen++;
                dre = 15;
            }
            if(0 != AllData.ST[i] && 0 == AllData.config[i])
            {
                temp[msglen] |= (0x1 << (dre - i));
            }
        }
        msglen++;
        
        temp[msglen++] = AllData.sum;
        dre = 7;
        for(i = 0; i < AllData.sum && i < RSP_PORT_MAX; i++)
        {
            if(8 == i)
            {
                msglen++;
                dre = 15;
            }
            if(0 != AllData.CT[i] && 0 == AllData.config[1])
            {
                temp[msglen] |= (0x1 << (dre - i));
            }
        }
        msglen++;
        
        temp[msglen++] = AllData.sum;
        for(i = 0; i < AllData.sum && i < RSP_PORT_MAX; i++)
        {
            if(0 == AllData.config[i])
            {
                MEMCPY_R(temp + msglen, &(AllData.chgTime[i].year), 2);
                msglen += 2;
                temp[msglen++] = AllData.chgTime[i].month;
                temp[msglen++] = AllData.chgTime[i].mday;
                temp[msglen++] = AllData.chgTime[i].wday;
                temp[msglen++] = AllData.chgTime[i].hour;
                temp[msglen++] = AllData.chgTime[i].minute;
                temp[msglen++] = AllData.chgTime[i].second;
                MEMCPY_R(temp + msglen, &(AllData.chgTime[i].msec), 2);
                msglen += 2;
            }
            else
            {
                memset(temp, 0xFF, 10);
                msglen += 10;
            }
        }
        ptMsgNew->msglen = msglen;

        ret = rsp_send_mqtt_msg(uTopic, ptMsgNew, 0);

        memset(showbuf, 0, 1024);
        offset = 0;
        for(i = 0; i < AllData.sum && i < RSP_PORT_MAX; i++)
        {
            offset += sprintf(showbuf + offset, "遥信%d: 状态 %u, 变位 %u, 时间%04d-%02d-%02d %02d:%02d:%02d.%03d\n", 
                              i + 1, AllData.ST[i], AllData.CT[i], AllData.chgTime[i].year, AllData.chgTime[i].month,
                              AllData.chgTime[i].mday, AllData.chgTime[i].hour, AllData.chgTime[i].minute,
                              AllData.chgTime[i].second, AllData.chgTime[i].msec);
        }
        if(showbuf[0])
        {
            MQTTMSG_FMT_DEBUG("上报遥信变位(ret = %d), 设备%d: \n%s", ret, AllData.id, showbuf);
        }
    }
    
    return 50000;
}

