/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP MQTT消息处理
* @date：    2021-7-8
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "task_base.h"
#include "appmsg_split.h"
#include "framecomm.h"

#include "radio_data.h"
#include "radio_msg.h"
#include "radio_mqtt.h"
#include "storage.h"


pthread_mutex_t  g_MsgIndexlock; 
uint16           g_AppMsgindex = 0;
uint8           g_longSendFlag = 0;     //电台长发标记
RADIO_LONG_SEND_T g_longSendParam;

extern int AtGet(const int fd, char *send_buf, uint8 *get_buf, const int32 get_buf_len);
extern uint32 SetRadioChannel_WDASN(int fd_232, uint8 ch, uint8 ptx, uint8 sq);
extern int radio_write_para(RADIO_PARA_T *pConfig);
extern int radio_write_addr(RADIO_ADDR_T *pConfig);
extern uint8 answer_cch[5];
extern int radio_channel_get_by_command(int fd);

#if DESC("AXDR处理函数", 1)
/**********************************************************************
* @name      : radio_login_to_smios
* @brief     ：APP注册
* @param[in] ：uint8 *pAppId   APPID
               uint8 appidlen  长度
* @param[out]：
* @return    ：0-成功/错误码 
**********************************************************************/
int radio_login_to_smios(uint8 *pAppId, uint8 appidlen)
{
    uint8           buf[420] = {0};
    uint16          msglen = 0;
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T   *ptAXDR = &(ptMsgNew->tAXDR);
    uint8              *temp = ptAXDR->playload;
    int                  len = 0;
    pid_t                pid = getpid();
    Date_Time_S        tTime;
    char              *smios = "M-smiOS";
    char              *srcApp = "M-radioManager";

    app_get_maketime(APP_PUBLISH_TIME, &tTime);

    /* 进程号 */
    appmsg_memcpy_r(temp, &pid, sizeof(pid_t));
    msglen += 4;

    /* APP名称 */
    len = strlen(srcApp);
    temp[msglen++] = len;
    memcpy(temp + msglen, srcApp, len);  
    msglen += len;

    /* 订阅事件 */
    temp[msglen++] = 0;

    /* 版本信息 */
    len = strlen(g_AppVer);
    temp[msglen++] = len;
    memcpy(temp + msglen, g_AppVer, len);  
    msglen += len;

    /* 发布日期 */
    temp[msglen++] = tTime.year_h;
    temp[msglen++] = tTime.year_l;
    temp[msglen++] = tTime.month;
    temp[msglen++] = tTime.day;
    temp[msglen++] = tTime.hour;
    temp[msglen++] = tTime.min;
    temp[msglen++] = tTime.sec;

    /* APPID */
    temp[msglen++] = appidlen;
    if(appidlen > 0)
    {
        memcpy(temp + msglen, pAppId, appidlen);
    }
    msglen += appidlen;

    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 1;
    ptAXDR->index = radio_get_next_index();
    ptAXDR->priority = 0;
    ptAXDR->label = 0;
    ptAXDR->IID = MESSAGE_IID_SYSTEM;
    ptAXDR->IOP = SYSTEM_IOP_APP_REGISTER;
    
    ptAXDR->sourlen = strlen(srcApp);
    memcpy(ptAXDR->souraddr , srcApp, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(smios);
    memcpy(ptAXDR->destaddr, smios, ptAXDR->destlen);

    ptAXDR->msglen = msglen;
    
    return radio_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : radio_heart_bear
* @brief     ：心跳应答
* @param[in] ：MESSAGE_INFO_T *pInMsg 请求消息
               void *pArg             私有句柄
               void *pPublic          公共数据
* @param[out]：
* @return    ：0-成功/错误码
**********************************************************************/
int radio_heart_bear_to_smios(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pArg, RADIO_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T *ptAXDR = &(ptMsgNew->tAXDR);
    uint32 status = 1;
    char   *smios = "M-smiOS";
    char   *srcAPP = "M-radioManager";

    if(NULL == pInMsg)
    {
        return -1;
    }
    
    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 0;
    ptAXDR->index = pInMsg->index;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->label = pInMsg->label;
    ptAXDR->IID = MESSAGE_IID_COMMON;
    ptAXDR->IOP = pInMsg->IOP;
    
    ptAXDR->sourlen = strlen(srcAPP);
    memcpy(ptAXDR->souraddr, srcAPP, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(smios);
    memcpy(ptAXDR->destaddr, smios, ptAXDR->destlen);

    ptAXDR->msglen = sizeof(uint32);
    appmsg_memcpy_r(ptAXDR->playload, &status, sizeof(uint32));
    MQTTMSG_FMT_DEBUG("heartbeat\n");
    return radio_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : radio_get_versionmsg
* @brief     ：获取版本
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int radio_get_versionmsg(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    MESSAGE_INFO_T   *ptAXDR = (MESSAGE_INFO_T*)buf;
    uint8              *temp = ptAXDR->playload;
    uint32           version = radio_stoi(APP_VERSION);
    Date_Time_S       tTime;

    if(NULL == pInMsg || NULL == pHandle)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    /* 版本信息 */
    appmsg_memcpy_r(temp + msglen, &version, sizeof(uint32));
    msglen += sizeof(uint32);

    /* 发布日期 */
    app_get_maketime(APP_PUBLISH_TIME, &tTime);
    temp[msglen++] = tTime.year_h;
    temp[msglen++] = tTime.year_l;
    temp[msglen++] = tTime.month;
    temp[msglen++] = tTime.day;
    temp[msglen++] = tTime.hour;
    temp[msglen++] = tTime.min;
    temp[msglen++] = tTime.sec;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_send_msg_req
* @brief     ：发送消息请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_send_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    RADIO_MSG_T       tMsgSend;
    uint8           bytesnum = 0;
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        MQTTMSG_FMT_DEBUG("radio_send_msg_req null pointer\n");
        return -1;
    }

    MQTTMSG_FMT_DEBUG("radio_send_msg_req\n");

//    if(FALSE == pPublic->bWork)
//    {
//        MQTTMSG_FMT_DEBUG("电台设备不存在, 发送消息失败\n");
//        goto end;
//    }

    memset(&tMsgSend, 0, sizeof(RADIO_MSG_T));
    /* 消息解析 */
    GET_LEN_OFFSET(msglen, temp, bytesnum);
    if (msglen + bytesnum != pInMsg->msglen)
    {
        MQTTMSG_FMT_DEBUG("消息错误\n");
        goto end;
    }
    temp += bytesnum;

    pQueue = &(pPublic->tDevPorc.msgQueue);
    while(1)
    {
        tMsgSend.len = MIN(msglen, RADIO_MSG_MAX_LEN);
        memcpy(tMsgSend.msg, temp, tMsgSend.len);
        
        pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_MSG_T));
        if(NULL == pMsgDev)
        {
            MQTTMSG_FMT_DEBUG("申请内存失败\n");
            goto end;
        }
        memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_MSG_T));
        pMsgDev->pNext = NULL;
        pMsgDev->priority = pInMsg->priority;
        pMsgDev->type = MESSAGE_TYPE_A_XDR;
        pMsgDev->token = pInMsg->index;
        pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
        memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
        pMsgDev->proc = RADIO_IOP_SEND_DATA;
        pMsgDev->datalen = sizeof(RADIO_MSG_T);
        memcpy(pMsgDev->data, &tMsgSend, pMsgDev->datalen);
        
        /* 写入消息队列 */
        radio_write_queue(pQueue, pMsgDev, TRUE);
        MQTTMSG_FMT_DEBUG("radio_send_msg_req radio_write_queue\n");

        /* 判断是否结束 */
        msglen -= tMsgSend.len;
        temp += tMsgSend.len;
        if(0 == msglen)
        {
            break;
        }
    }

    return 0;

end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = 0;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}
#if 0
/**********************************************************************
* @name      : radio_trans_msg_req
* @brief     ：透传消息请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_trans_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    RADIO_TRANS_REQ_T tMsgTrans;
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    int             ret = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 发送消息失败\n");
        goto end;
    }

    memset(&tMsgTrans, 0, sizeof(RADIO_TRANS_REQ_T));
    /* 消息解析 */
    ret = radio_trans_req_split(pInMsg->playload, pInMsg->msglen, &tMsgTrans);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("电台转发消息解析失败\n");
        goto end;
    }

    /* 合法性判断 不支持上行通道使用透传 */
    if(tMsgTrans.port > RADIO_PORT_MAX || tMsgTrans.port <= RADIO_PORT_MASTER1)
    {
        MQTTMSG_FMT_DEBUG("无效端口 %d\n", tMsgTrans.port);
        goto end;
    }
    if(tMsgTrans.match.mode > RADIO_MATCH_OB)
    {
        MQTTMSG_FMT_DEBUG("配对模式错误%d\n", tMsgTrans.match.mode);
        goto end;
    }
    if(0 == tMsgTrans.waittime.msgtime)
    {
        /* 默认等待500ms */
        tMsgTrans.waittime.msgtime = 500;
    }
    if(0 == tMsgTrans.waittime.bytetime)
    {
        tMsgTrans.waittime.bytetime = 20;
    }
    
    pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_TRANS_REQ_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_TRANS_REQ_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = pInMsg->priority;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
    pMsgDev->proc = RADIO_IOP_TRAN_DATA;
    pMsgDev->datalen = sizeof(RADIO_TRANS_REQ_T);
    memcpy(pMsgDev->data, &tMsgTrans, pMsgDev->datalen);
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.transQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    ret = 1;
    MEMCPY_R(temp, &ret, 4);
    msglen += 4;
    temp[msglen++] = 0;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}
#endif

RADIO_CONF_T g_RadioConfig = 
{
    1,
    5,
    5,
    2,
    {0x32, 0x99},
    3,
    {0xF1, 0x23, 0x45},
    6,
    {0x32, 0x99, 0x00, 0x01, 0x23, 0x45}
};
uint32 g_RadioConfigChangeFlag  = 0;
uint32 g_RadioSetCfgFlag  = 0;
/**********************************************************************
* @name      : radio_setcfg_msg_req
* @brief     ：设置参数请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_setcfg_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    RADIO_PARA_T    config = {0};
    //RADIO_QUEUE_T    *pQueue = NULL;
    //RADIO_DEV_MSG_T  *pMsgDev = NULL;
    //int             ret = 0;
    uint8           value = 0;
    
    if (NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        MQTTMSG_FMT_DEBUG("radio_setcfg_msg_req null pointer\n");
        return -1;
    }

    MQTTMSG_FMT_DEBUG("radio_setcfg_msg_req 设置电台参数\n");

    if (pInMsg->msglen < sizeof(RADIO_PARA_T))
    {
        MQTTMSG_FMT_DEBUG("radio_setcfg_msg_req msglen[%u] is too short!\n", pInMsg->msglen);
        value = 0;
    }
    else
    {
        memcpy(&config, pInMsg->playload, sizeof(config));
        MQTTMSG_FMT_DEBUG("radio_setcfg_msg_req channel=%u,power=%u,sensitivity=%u\n", 
                          config.channel, config.power, config.sensitivity);

        if (config.power >= 0 && config.power <= 4 && config.sensitivity >= 0 && config.sensitivity <= 8)
        {
            g_RadioConfig.channel = config.channel;
            g_RadioConfig.power = config.power;
            g_RadioConfig.sensitivity = config.sensitivity;
            radio_write_para(&config);
            pPublic->tDevStatus.bParaConfig = TRUE;
            //g_RadioConfigChangeFlag = 1;

            /*if (FALSE == pPublic->bWork)
            {
                MQTTMSG_FMT_DEBUG("电台设备不存在, 参数设置失败\n");
                value =  0;
                goto end;
            }

            if (pPublic->tDevPorc.devfd < 0)
            {
                MQTTMSG_FMT_DEBUG("电台句柄无效, 参数设置失败\n");
                value =  0;
                goto end;
            }*/

            value = 1;
            g_RadioSetCfgFlag = 1;
            /*if (SetRadioChannel_WDASN(pPublic->tDevPorc.devfd, g_RadioConfig.channel, g_RadioConfig.power, g_RadioConfig.sensitivity) != 0)
            {
                MQTTMSG_FMT_DEBUG("radio_setcfg_msg_req SetRadioChannel_WDASN ret fail.\n");
                value = 0;
            }
            else
            {
                MQTTMSG_FMT_DEBUG("radio_setcfg_msg_req SetRadioChannel_WDASN ret succ.\n");
                value = 1;
            }*/
        }
        else
        {
            value =  0;
        }
    }
//end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = value;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_setaddr_msg_req
* @brief     ：设置地址请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_setaddr_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    RADIO_ADDR_T    config = {0};
    //RADIO_QUEUE_T    *pQueue = NULL;
    //RADIO_DEV_MSG_T  *pMsgDev = NULL;
    //int             ret = 0;
    uint8           value = 0;
    
    if (NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        MQTTMSG_FMT_DEBUG("radio_setaddr_msg_req null pointer\n");
        return -1;
    }

    MQTTMSG_FMT_DEBUG("radio_setaddr_msg_req 设置电台地址\n");

    /*if (FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 地址设置失败\n");
        value =  0;
        goto end;
    }*/

    if (pInMsg->msglen < sizeof(RADIO_ADDR_T))
    {
        MQTTMSG_FMT_DEBUG("radio_setaddr_msg_req msglen[%u] is too short!\n", pInMsg->msglen);
        value = 0;
    }
    else
    {
        memcpy(&config, pInMsg->playload, sizeof(config));
        MQTTMSG_FMT_DEBUG("radio_setaddr_msg_req areaCode=%02X %02X,devAddr=%02X %02X %02X,tmnAddr=%02X %02X %02X %02X %02X %02X\n", 
                          config.areaCode[0], config.areaCode[1],
                          config.devAddr[0], config.devAddr[1], config.devAddr[2], config.tmnAddr[0], config.tmnAddr[1],
                          config.tmnAddr[2], config.tmnAddr[3], config.tmnAddr[4], config.tmnAddr[5]);

        if (config.areaCodeLen == 2 && config.devAddrLen == 3 && config.tmnAddrLen == 6)
        {
            g_RadioConfig.areaCodeLen = 2;
            memcpy(g_RadioConfig.areaCode, config.areaCode, sizeof(g_RadioConfig.areaCode));
            g_RadioConfig.devAddrLen = 3;
            memcpy(g_RadioConfig.devAddr, config.devAddr, sizeof(g_RadioConfig.devAddr));
            g_RadioConfig.tmnAddrLen = 6;
            memcpy(g_RadioConfig.tmnAddr, config.tmnAddr, sizeof(g_RadioConfig.tmnAddr));
            radio_write_addr(&config);
            write_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_RadioConfig.areaCode, 0, 2);
            write_pdata_xram("/data/app/desktopGui/data/radio", (char*)&g_RadioConfig.devAddr, 2, 3);
            pPublic->tDevStatus.bAddrConfig = TRUE;
            //上海电台不需要地址
#ifndef AREA_SHANGHAI
            g_RadioConfigChangeFlag = 1;
#endif
            value = 1;
        }
        else
        {
            value =  0;
        }
    }
//end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = value;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}


uint8  g_RadioSpeakerCtl = 0 ;  /* 扬声器控制状态 1:关闭，2:打开 */

/**********************************************************************
* @name      : radio_setspeaker_msg_req
* @brief     ：设置扬声器状态请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_setspeaker_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    uint8           speakerFlag = 0;
    uint8           value = 0;
    
    if (NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        MQTTMSG_FMT_DEBUG("radio_setspeaker_msg_req null pointer\n");
        value = 0;
        goto end;
    }

    MQTTMSG_FMT_DEBUG("radio_setspeaker_msg_req 设置扬声器状态请求\n");

    /*if (FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 参数设置失败\n");
        value = 0;
        goto end;
    }*/

    if (pInMsg->msglen < 1)
    {
        MQTTMSG_FMT_DEBUG("radio_setcfg_msg_req msglen[%u] is too short!\n", pInMsg->msglen);
        value = 0;
    }
    else
    {
        speakerFlag = pInMsg->playload[0];

        if (speakerFlag == 1 || speakerFlag == 2)
        {
            if (speakerFlag == 2)
            {
                MQTTMSG_FMT_DEBUG("设置扬声器打开\n");
                g_RadioSpeakerCtl = 2;
                
            }
            else if (speakerFlag == 1)
            {
                MQTTMSG_FMT_DEBUG("设置扬声器关闭\n");
                g_RadioSpeakerCtl = 1;

            }

            value = 1;
        }
        else
        {
            value =  0;
        }
    }
end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = value;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

extern float g_pczPower[5];
uint32 ReadRadioChannel_WDASN(int fd_232, RADIO_PARA_T *para)
{
    char  rcv_buf[100];
    int8 at_status;
    uint8 answer_cch[5] = {0};
    char *ptemp = NULL;

    memset(rcv_buf, 0x0, sizeof(rcv_buf));

    at_status = AtGet(fd_232, "AT+RADIO&P=?\r", (uint8 *)rcv_buf, sizeof(rcv_buf));
    if(at_status > 0 && ((ptemp = strstr(rcv_buf, "AT+RADIO&P=")) != NULL))
    {
        uint8 ch = 0;
        uint8 ptx = 0;
        float tmpPTX = 0.0f;
        uint8 sq = 0;
        int tmpSQ = 0;
        char tmpBuf[10];
        int i;
        /*
        设置和查询电台参数。
        AT+RADIO&P=01,05.00,01\r    
        参数分别为频道(1)、功率（5W）、静噪级别(1)
        */
        memset(tmpBuf, 0x0, sizeof(tmpBuf));
        memcpy(tmpBuf, ptemp + 11, 2);
        ch = (uint8)atoi(tmpBuf) - 1;       //频道

        memset(tmpBuf, 0x0, sizeof(tmpBuf));
        memcpy(tmpBuf, ptemp + 14, 5);
        tmpPTX = atof(tmpBuf);      //功率
        for(i = 0; i < sizeof(g_pczPower)/sizeof(g_pczPower[0]); i++)
        {
            if(tmpPTX <= g_pczPower[i])
            {
                ptx = i;
                break;
            }
        }

        memset(tmpBuf, 0x0, sizeof(tmpBuf));
        memcpy(tmpBuf, ptemp + 20, 2);
        tmpSQ = (uint8)atoi(tmpBuf);        //静噪级别
        if((tmpSQ <= 9)&&(tmpSQ >= 1))
        {
            sq = tmpSQ;
        }
        else
            {
            sq = 5;
        }
        MQTTMSG_FMT_DEBUG(" WDASN ch = %d      ptx = %d    sq = %d     tmpPTX = %f     tmpSQ = %d\n", ch, ptx, sq, tmpPTX, tmpSQ);

        para->channel = ch; /*频道*/
        para->power = ptx;    /*功率*/
        para->sensitivity = sq; /*灵敏度*/
        answer_cch[4] = answer_cch[0]+answer_cch[1]+answer_cch[2]+answer_cch[3];
        return 0;
    }
    else        /*查询失败*/
    {
        para->channel = 0xff;
        para->power = 0xff;
        para->sensitivity = 0xff;
        return 1;
    }
    
}

uint32 ReadRadioChannel_Version(int fd_232, char *version)
{
    char  rcv_buf[100];
    int8 at_status;
    char *ptemp = NULL;

    memset(rcv_buf, 0x0, sizeof(rcv_buf));

    at_status = AtGet(fd_232, "AT+VER=?\r", (uint8 *)rcv_buf, sizeof(rcv_buf));
    if(at_status > 0 && ((ptemp = strstr(rcv_buf, "AT+VER=")) != NULL))
    {
        /*
        设置和查询电台参数。
        AT+RADIO&P=01,05.00,01\r    
        参数分别为频道(1)、功率（5W）、静噪级别(1)
        */
        //memset(tmpBuf, 0x0, sizeof(tmpBuf));
        memcpy(version, ptemp + 7, 4);

        return 0;
    }
    else        /*查询失败*/
    {
        return 1;
    }
    
}

/**********************************************************************
* @name      : radio_getcfg_msg_req
* @brief     ：查询参数请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_getcfg_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    RADIO_PARA_T    para = {0};
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    para.channel = 0xff;
    
    if (NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        MQTTMSG_FMT_DEBUG("radio_getcfg_msg_req null pointer\n");
        goto end;
    }

    MQTTMSG_FMT_DEBUG("radio_getcfg_msg_req 查询电台参数\n");

    pMsgDev = (RADIO_DEV_MSG_T *)malloc(sizeof(RADIO_DEV_MSG_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, strlen(pInMsg->souraddr));
    pMsgDev->proc = RADIO_IOP_GET_CONFIG;
    pMsgDev->datalen = 0;
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);
    return 0;

end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    RADIO_CONF_T tmpConfig = g_RadioConfig;
    tmpConfig.channel = para.channel;
    tmpConfig.power = para.power;
    tmpConfig.sensitivity = para.sensitivity;

    memcpy(ptAXDR->playload, (uint8 *)&tmpConfig, sizeof(tmpConfig));
    ptAXDR->msglen = sizeof(tmpConfig);
    
    return radio_send_axdr_msg(ptAXDR, 0);
}


/**********************************************************************
* @name      : radio_getcfg_msg_req
* @brief     ：查询参数请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_getversion_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    char tmpversion[4]={0};
    
    
    if (NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        MQTTMSG_FMT_DEBUG("radio_getcfg_msg_req null pointer\n");
        goto end;
    }

    MQTTMSG_FMT_DEBUG("radio_getcfg_msg_req 查询电台参数\n");

    pMsgDev = (RADIO_DEV_MSG_T *)malloc(sizeof(RADIO_DEV_MSG_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, strlen(pInMsg->souraddr));
    pMsgDev->proc = RADIO_IOP_GET_VERSION;
    pMsgDev->datalen = 0;
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);
    return 0;

end:


    memcpy(ptAXDR->playload, (uint8 *)&tmpversion, sizeof(tmpversion));
    ptAXDR->msglen = sizeof(tmpversion);
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_getcfg_msg_req
* @brief     ：查询参数请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int RadioGetCfgProc(int fd, RADIO_DEV_MSG_T *pMsg)
{
    uint8           buf[360] = {0};
    RADIO_PARA_T    para = {0};
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;
    para.channel = 0xff;
    
    if (NULL == pMsg)
    {
        MQTTMSG_FMT_DEBUG("RadioGetCfgProc null pointer\n");
        goto end;
    }

    MQTTMSG_FMT_DEBUG("RadioGetCfgProc 查询电台参数处理\n");

    if (fd < 0)
    {
        MQTTMSG_FMT_DEBUG("电台句柄无效, 参数查询失败\n");
        goto end;
    }
    else
    {
#ifdef AREA_DEFAULT
        radio_channel_get_by_command(fd);
        para.channel = answer_cch[1];
        para.power = answer_cch[2];
        para.sensitivity = answer_cch[3];
        //para.channel = g_RadioConfig.channel;
        //para.power = g_RadioConfig.power;
        //para.sensitivity = g_RadioConfig.sensitivity;
#else
        ReadRadioChannel_WDASN(fd, &para);
#endif
    }
end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pMsg->priority;
    ptAXDR->index = ((pMsg->token >> 16) & 0xFFFF);
    ptAXDR->label = pMsg->token;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    ptAXDR->destlen = strlen(pMsg->source);
    memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

    ptAXDR->IID = MESSAGE_IID_RADIO;
    ptAXDR->IOP = pMsg->proc;

    RADIO_CONF_T tmpConfig = g_RadioConfig;
    tmpConfig.channel = para.channel;
    tmpConfig.power = para.power - 1;
    tmpConfig.sensitivity = para.sensitivity - 1;

    memcpy(ptAXDR->playload, (uint8 *)&tmpConfig, sizeof(tmpConfig));
    ptAXDR->msglen = sizeof(tmpConfig);
    
    return radio_send_axdr_msg(ptAXDR, 0);
}


/**********************************************************************
* @name      : radio_getcfg_msg_req
* @brief     ：查询参数请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int RadioGetVersionProc(int fd, RADIO_DEV_MSG_T *pMsg)
{
    uint8           buf[360] = {0};
    char vesion[8] = {0};
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;
    
    if (NULL == pMsg)
    {
        MQTTMSG_FMT_DEBUG("RadioGetCfgProc null pointer\n");
        goto end;
    }

    MQTTMSG_FMT_DEBUG("RadioGetCfgProc 查询电台参数处理\n");

    if (fd < 0)
    {
        MQTTMSG_FMT_DEBUG("电台句柄无效, 参数查询失败\n");
        goto end;
    }
    else
    {
        ReadRadioChannel_Version(fd, vesion);
    }
end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pMsg->priority;
    ptAXDR->index = ((pMsg->token >> 16) & 0xFFFF);
    ptAXDR->label = pMsg->token;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    ptAXDR->destlen = strlen(pMsg->source);
    memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

    ptAXDR->IID = MESSAGE_IID_RADIO;
    ptAXDR->IOP = pMsg->proc;


    memcpy(ptAXDR->playload, (uint8 *)vesion, 4);
    ptAXDR->msglen = 4;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_getstatus_msg_req
* @brief     ：查询电台状态请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_getstatus_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{

    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T * pMsgDev = NULL;
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        MQTTMSG_FMT_DEBUG("radio_getstatus_msg_req null pointer\n");
        return -1;
    }

    MQTTMSG_FMT_DEBUG("radio_getstatus_msg_req 查询电台状态请求\n");

    pMsgDev = (RADIO_DEV_MSG_T *)malloc(sizeof(RADIO_DEV_MSG_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        return -1;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, strlen(pInMsg->souraddr));
    pMsgDev->proc = RADIO_IOP_GET_STATUS;
    pMsgDev->datalen = 0;
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);
    return 0;
}

#if 0
/**********************************************************************
* @name      : radio_setport_msg_req
* @brief     ：端口配置请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_setport_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    RADIO_PORTS_CONF_T tPort;
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    int             ret = 0;
    int             i = 0;
    uint8           MAC1[6] = {0};
    uint8           MAC2[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 参数设置失败\n");
        goto end;
    }

    memset(&tPort, 0, sizeof(RADIO_PORTS_CONF_T));
    /* 消息解析 */
    ret = radio_port_config_split(pInMsg->playload, pInMsg->msglen, &tPort);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("电台端口设置解析失败\n");
        goto end;
    }

    /* 参数检查 */
    if(0 == tPort.num)
    {
        MQTTMSG_FMT_DEBUG("无电台端口配置\n");
        goto end;
    }
    for(i = 0; i < tPort.num; i++)
    {
        if(tPort.tConf[i].port >= RADIO_PORT_MAX)
        {
            MQTTMSG_FMT_DEBUG("电台端口超过31\n");
            goto end;
        }
        if((tPort.tConf[i].port <= RADIO_PORT_MASTER1 && RADIO_MODE_SLAVE == tPort.tConf[i].mode) ||
           (tPort.tConf[i].port > RADIO_PORT_MASTER1 && RADIO_MODE_MASTER == tPort.tConf[i].mode) || 
           tPort.tConf[i].mode > 1)
        {
            MQTTMSG_FMT_DEBUG("电台端口配置错误\n");
            goto end;
        }
        if((tPort.tConf[i].port > RADIO_PORT_MASTER1 && RADIO_MODE_SLAVE == tPort.tConf[i].mode))
        {
            if(0 == memcmp(tPort.tConf[i].MAC, MAC1, 6) || 0 == memcmp(tPort.tConf[i].MAC, MAC2, 6))
            {
                MQTTMSG_FMT_DEBUG("电台端口配置错误\n");
                goto end;
            }
        }
    }
    
    pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_PORTS_CONF_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_CONF_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = pInMsg->priority;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
    pMsgDev->proc = RADIO_IOP_SET_PORT;
    pMsgDev->datalen = sizeof(RADIO_PORTS_CONF_T);
    memcpy(pMsgDev->data, &tPort, pMsgDev->datalen);
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = 0;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_getlink_msg_req
* @brief     ：电台连接信息请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_getlink_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8              buf[1024] = {0};
    MESSAGE_INFO_T    *ptAXDR = (MESSAGE_INFO_T*)buf; 
    RADIO_CONNECT_INFO_T tLink;
    RADIO_DEV_T         *ptDevStatus = &(pPublic->tDevStatus);
    uint32             i = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 端口查询失败\n");
        return -1;
    }

    memset(&tLink, 0, sizeof(RADIO_CONNECT_INFO_T));
    for(i = 0; i < RADIO_PORT_MAX; i++)
    {
        if(TRUE == ptDevStatus->PortCfg[i].bConfig || (i < 2 && TRUE == ptDevStatus->PortCfg[i].bConnect))
        {
            tLink.list[tLink.num].port = ptDevStatus->PortCfg[i].porttCfg.port;
            tLink.list[tLink.num].mode = ptDevStatus->PortCfg[i].porttCfg.mode;
            tLink.list[tLink.num].bConnect = ptDevStatus->PortCfg[i].bConnect;
            memcpy(tLink.list[tLink.num].MAC, ptDevStatus->PortCfg[i].porttCfg.MAC, 6);
            tLink.num += 1;
        }
    }
    
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    ptAXDR->msglen = radio_connect_package(&tLink, ptAXDR->playload, 900);
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_getsn_msg_req
* @brief     ：电台连接信息请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_getsn_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8              buf[1024] = {0};
    MESSAGE_INFO_T    *ptAXDR = (MESSAGE_INFO_T*)buf; 
    RADIO_SN_T tSn;
    RADIO_DEV_T         *ptDevStatus = &(pPublic->tDevStatus);
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 端口查询失败\n");
        return -1;
    }

    memcpy(&tSn, &ptDevStatus->sn, sizeof(tSn));
    
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    ptAXDR->msglen = radio_sn_package(&tSn, ptAXDR->playload, 900);
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_setcfg_msg_req
* @brief     ：设置参数请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_setsn_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    uint8           Result = 1;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    RADIO_SN_T        tSn;
    int             ret = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 参数设置失败\n");
        Result = 0;
        goto end;
    }

    memset(&tSn, 0, sizeof(tSn));
    /* 消息解析 */
    ret = radio_sn_split(pInMsg->playload, pInMsg->msglen, &tSn);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("电台SN解析失败\n");
        Result = 0;
        goto end;
    }

    MQTTMSG_BUF_DEBUG(tSn.sn, tSn.num, "设置电台SN");
    pPublic->tDevStatus.sn = tSn;
    
end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = Result;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : radio_setcheck_msg_req
* @brief     ：电台检表配置请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-12
* @Update    :
**********************************************************************/
int radio_setcheck_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    RADIO_CHECK_T     tcheck;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    uint8           result = 0;
    char           *checkname[] = {"秒脉冲", "需量周期", "时段投切", "正向谐波", "反向谐波", "无功脉冲", "有功脉冲"};
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 参数设置失败\n");
        goto end;
    }

    memset(&tcheck, 0, sizeof(RADIO_CHECK_T));
    if(0 != radio_check_split(pInMsg->playload, pInMsg->msglen, &tcheck))
    {
        MQTTMSG_FMT_DEBUG("电台检表配置错误\n");
        goto end;
    }
    if(tcheck.type > RADIO_TYPE_ACTIVE && tcheck.type < RADIO_TYPE_CLOSE)
    {
        MQTTMSG_FMT_DEBUG("%s设置电台检表功能%s\n", pInMsg->souraddr, "未知");
        goto end;
    }

    MQTTMSG_FMT_DEBUG("%s设置电台检表%s\n", pInMsg->souraddr, 
                      tcheck.type <= RADIO_TYPE_ACTIVE ? checkname[tcheck.type] : "关闭");

    /* 写入消息队列 */
    pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_CHECK_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_CHECK_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = pInMsg->priority;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
    pMsgDev->proc = RADIO_IOP_CHECK_METER;
    pMsgDev->datalen = sizeof(RADIO_CHECK_T);
    memcpy(pMsgDev->data, &tcheck, pMsgDev->datalen);
    
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);

    //不返回，先回消息
    #if CHECK_METER_ASYNC == 0
    return 0;
    #else
    //参数延迟几秒发送
    pPublic->tDevPorc.cfgDelay = 8*(200/RADIO_RECV_TIME_OUT);
    result = 1;
    #endif
end:
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    ptAXDR->playload[msglen++] = result;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}
#endif

/**********************************************************************
* @name      : radio_get_appid
* @brief     ：获取APPID
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int radio_get_appidmsg(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    MESSAGE_INFO_T   *ptAXDR = (MESSAGE_INFO_T*)buf;
    uint8              *temp = ptAXDR->playload;

    if(NULL == pInMsg || NULL == pHandle)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    /* APPID */
    temp[msglen++] = pPublic->appidlen;
    memcpy(temp + msglen, pPublic->appid, pPublic->appidlen);
    msglen += pPublic->appidlen;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

int radio_msg_no_proc(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    return 0;
}

/**********************************************************************
* @name      : radio_longsend_msg_req
* @brief     ：电台长发
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_longsend_msg_req(MESSAGE_INFO_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    //RADIO_LONG_SEND_T       data;
    uint8 result;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        MQTTMSG_FMT_DEBUG("radio_send_msg_req null pointer\n");
        return -1;
    }

    MQTTMSG_FMT_DEBUG("radio_longsend_msg_req\n");

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 发送消息失败\n");
        result = 0;
    }else
    {
        memset(&g_longSendParam, 0, sizeof(RADIO_LONG_SEND_T));
        /* 消息解析 */
        g_longSendParam.sendbuf = temp[0];
        memcpy_r(&g_longSendParam.sendtime,temp+1,2);
        MQTTMSG_FMT_DEBUG("收到电台长发消息 内容 %02x 持续时间 %d 秒\n",g_longSendParam.sendbuf,g_longSendParam.sendtime);
        result = 1;
        //持续时间为0时代表关闭电台长发
        if(g_longSendParam.sendtime == 0 )
        {
            g_longSendFlag = 0;
            MQTTMSG_FMT_DEBUG("关闭电台长发\n");
        }else
            g_longSendFlag = 1;
   }

    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = result;
    ptAXDR->msglen = msglen;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}


//#define RADIO_IOP_SET_SPEAKER  0x0015
RADIO_MSGFUN_T g_tMsgFunc[] = 
{/*  IID,                  IOP,                     消息处理函数 */
    {MESSAGE_IID_COMMON,   COMMON_IOP_VERSION,      radio_get_versionmsg},
    {MESSAGE_IID_COMMON,   COMMON_IOP_HERTBEAT,     radio_heart_bear_to_smios},
    {MESSAGE_IID_COMMON,   COMMON_IOP_GETAPPID,     radio_get_appidmsg},

    {MESSAGE_IID_DESKGUI,   DESKGUI_IOP_FRESH_TOOL,     radio_msg_no_proc},

    {MESSAGE_IID_RADIO,      RADIO_IOP_SEND_DATA,       radio_send_msg_req},
    //{MESSAGE_IID_RADIO,      RADIO_IOP_TRAN_DATA,       radio_trans_msg_req},
    {MESSAGE_IID_RADIO,      RADIO_IOP_SET_CONFIG,      radio_setcfg_msg_req},
    {MESSAGE_IID_RADIO,      RADIO_IOP_SET_ADDR,        radio_setaddr_msg_req},
    {MESSAGE_IID_RADIO,      RADIO_IOP_GET_CONFIG,      radio_getcfg_msg_req},
    {MESSAGE_IID_RADIO,      RADIO_IOP_GET_STATUS,      radio_getstatus_msg_req},
    {MESSAGE_IID_RADIO,      RADIO_IOP_SET_YANG,        radio_setspeaker_msg_req},
    {MESSAGE_IID_RADIO,      RADIO_IOP_LONG_SENG,       radio_longsend_msg_req},
    {MESSAGE_IID_RADIO,      RADIO_IOP_GET_VERSION,       radio_getversion_msg_req},
    //{MESSAGE_IID_RADIO,      RADIO_IOP_LINK_INFO,       radio_getlink_msg_req},
    //{MESSAGE_IID_RADIO,      RADIO_IOP_RADIO_SN,           radio_getsn_msg_req},
    //{MESSAGE_IID_RADIO,      RADIO_IOP_SET_RADIO_SN,       radio_setsn_msg_req},
    //{MESSAGE_IID_RADIO,      RADIO_IOP_CHECK_METER,     radio_setcheck_msg_req},
};
uint8 g_nMsgNum = sizeof(g_tMsgFunc)/sizeof(RADIO_MSGFUN_T);

#define RADIO_ERR_OTHER_FAIL 255
/**********************************************************************
* @name      : radio_axdr_msg_proc
* @brief     ：axdr消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo       消息
               RADIO_MSGHANDLE_T *ptMsgHandle   私有数据
               RADIO_DATA_T *ptRADIOData          共有数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
void radio_axdr_msg_proc(MESSAGE_INFO_T *pMsgInfo, RADIO_MSGHANDLE_T *ptMsgHandle, RADIO_DATA_T *ptRADIOData)
{
    uint8 i = 0;
    
    if(0 == strcmp(pMsgInfo->souraddr, RADIO_APP_NAME))
    {
        /* 消除自发自收 */
        MQTTMSG_FMT_DEBUG("Revc self msg, iid %d, iop %d\n", pMsgInfo->IID, pMsgInfo->IOP);
        return;
    }

    /* 注册应答 */
    if (MESSAGE_IID_SYSTEM == pMsgInfo->IID )
    {
        if (SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
        {
            ptRADIOData->bReg = TRUE;
            RADIO_FMT_DEBUG("电台APP注册\n");
        }
        return;
    }


    for(i = 0; i < g_nMsgNum; i++)
    {
        if(g_tMsgFunc[i].IID == pMsgInfo->IID && g_tMsgFunc[i].IOP == pMsgInfo->IOP)
        {
            if(g_tMsgFunc[i].pProcMsg)
            {
                g_tMsgFunc[i].pProcMsg(pMsgInfo, (void *)ptMsgHandle, (void *)ptRADIOData);
                break;
            }
        }
    }
    
    if(i == g_nMsgNum && !(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_ERRMSG == pMsgInfo->IOP))
    {
        radio_send_errmsg(pMsgInfo, RADIO_ERR_OTHER_FAIL);
    }
}


#endif

#if DESC("JSON处理函数", 1)
#if 0
/**********************************************************************
* @name      : radio_json_setparam_req
* @brief     ：设置参数请求
* @param[in] ：MESSAGE_JSON_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_json_setparam_req(MESSAGE_JSON_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    RADIO_CONF_T    tConfig;
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    int             ret = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 参数设置失败\n");
        goto end;
    }

    memset(&tConfig, 0, sizeof(RADIO_CONF_T));
    /* 消息解析 */
    ret = radio_setparam_req_split(pInMsg->infopath, pInMsg->pObject, &tConfig);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("电台参数设置解析失败\n");
        goto end;
    }
    
    pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_CONF_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_CONF_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = RADIO_IOP_SET_CONFIG;
    pMsgDev->datalen = sizeof(RADIO_CONF_T);
    memcpy(pMsgDev->data, &tConfig, pMsgDev->datalen);
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    return radio_err_json_res(pInMsg, RADIO_ERR_OTHER_FAIL, NULL);
}

/**********************************************************************
* @name      : radio_json_getparam_req
* @brief     ：查询参数请求
* @param[in] ：MESSAGE_JSON_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_json_getparam_req(MESSAGE_JSON_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    char    topic[256] = {0};
    char   *strmsg = NULL;
    int     ret = 1;
    uint32  status = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 参数查询失败\n");
        status = RADIO_ERR_OTHER_FAIL;
    }

    /* 构造返回消息 */
    strmsg = radio_getparam_res_package(pInMsg->token, &(pPublic->tDevStatus.config), status);
    if(strmsg)
    {
        sprintf(topic, "%s/%s/%s/%s/%s/%s", RADIO_APP_NAME, pInMsg->sourapp, MQTT_TOPIC_JSON, pInMsg->operat,
                                            MQTT_TOPIC_INFO_RES, pInMsg->infopath);
        ret = radio_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }
    return ret;
}

/**********************************************************************
* @name      : radio_json_setport_req
* @brief     ：端口配置请求
* @param[in] ：MESSAGE_INFO_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_json_setport_req(MESSAGE_JSON_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{ 
    RADIO_PORTS_CONF_T tPort;
    RADIO_QUEUE_T     *pQueue = NULL;
    RADIO_DEV_MSG_T   *pMsgDev = NULL;
    int              ret = 0;
    uint8            MAC1[6] = {0};
    uint8            MAC2[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 参数设置失败\n");
        goto end;
    }

    memset(&tPort, 0, sizeof(RADIO_PORTS_CONF_T));;
    /* 消息解析 */
    ret = radio_setport_req_split(pInMsg->infopath, pInMsg->pObject, &(tPort.tConf[0]));
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("电台端口设置解析失败\n");
        goto end;
    }

    /* 端口检查 */
    if(tPort.tConf[0].port >= RADIO_PORT_MAX)
    {
        MQTTMSG_FMT_DEBUG("电台端口超过31\n");
        goto end;
    }
    if((tPort.tConf[0].port <= RADIO_PORT_MASTER1 && RADIO_MODE_SLAVE == tPort.tConf[0].mode) ||
       (tPort.tConf[0].port > RADIO_PORT_MASTER1 && RADIO_MODE_MASTER == tPort.tConf[0].mode) || tPort.tConf[0].mode > 1)
    {
        MQTTMSG_FMT_DEBUG("电台端口配置错误\n");
        goto end;
    }
    if((tPort.tConf[0].port > RADIO_PORT_MASTER1 && RADIO_MODE_SLAVE == tPort.tConf[0].mode))
    {
        if(0 == memcmp(tPort.tConf[0].MAC, MAC1, 6) || 0 == memcmp(tPort.tConf[0].MAC, MAC2, 6))
        {
            MQTTMSG_FMT_DEBUG("电台端口配置错误\n");
            goto end;
        }
    }
    tPort.num = 1;
    
    pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_PORTS_CONF_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_CONF_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = RADIO_IOP_SET_PORT;
    pMsgDev->datalen = sizeof(RADIO_PORTS_CONF_T);
    memcpy(pMsgDev->data, &tPort, pMsgDev->datalen);
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    return radio_err_json_res(pInMsg, RADIO_ERR_OTHER_FAIL, NULL);
}

/**********************************************************************
* @name      : radio_json_setcheck_req
* @brief     ：电台检表配置请求
* @param[in] ：MESSAGE_JSON_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_json_setcheck_req(MESSAGE_JSON_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    uint8           errorcode = RADIO_ERR_OTHER_FAIL;
    int             ret = 0;
    RADIO_CHECK_T     tcheck;
    char           *checkname[] = {"秒脉冲", "需量周期", "时段投切", "正向谐波", "反向谐波", "无功脉冲", "有功脉冲"};
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 参数设置失败\n");
        goto end;
    }

    ret = radio_setdetect_req_split(pInMsg->infopath, pInMsg->pObject, &tcheck);
    if(0 != ret || (tcheck.type > RADIO_TYPE_ACTIVE && tcheck.type < RADIO_TYPE_CLOSE))
    {
        MQTTMSG_FMT_DEBUG("电台检表配置错误\n");
        goto end;
    }

    MQTTMSG_FMT_DEBUG("%s设置电台检表%s\n", pInMsg->sourapp, 
                      tcheck.type <= RADIO_TYPE_ACTIVE ? checkname[tcheck.type] : "关闭");

    /* 写入消息队列 */
    pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_CHECK_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_CHECK_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = RADIO_IOP_CHECK_METER;
    pMsgDev->datalen = sizeof(RADIO_CHECK_T);
    memcpy(pMsgDev->data, &tcheck, pMsgDev->datalen);
    
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);
    return 0;
end:
    return radio_err_json_res(pInMsg, errorcode, NULL);
}

/**********************************************************************
* @name      : radio_json_getlink_req
* @brief     ：电台连接信息请求
* @param[in] ：MESSAGE_JSON_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_json_getlink_req(MESSAGE_JSON_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    RADIO_CONNECT_INFO_T tLink;
    RADIO_DEV_T         *ptDevStatus = &(pPublic->tDevStatus);
    uint32             i = 0;
    char               topic[256] = {0};
    char              *strmsg = NULL;
    int                ret = 1;
    uint32             status = RADIO_ERR_OTHER_FAIL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    memset(&tLink, 0, sizeof(RADIO_CONNECT_INFO_T));
    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 端口查询失败\n");
        goto end;
    }

    for(i = 0; i < RADIO_PORT_MAX; i++)
    {
        if(TRUE == ptDevStatus->PortCfg[i].bConfig || (i < 2 && TRUE == ptDevStatus->PortCfg[i].bConnect))
        {
            tLink.list[tLink.num].port = ptDevStatus->PortCfg[i].porttCfg.port;
            tLink.list[tLink.num].mode = ptDevStatus->PortCfg[i].porttCfg.mode;
            tLink.list[tLink.num].bConnect = ptDevStatus->PortCfg[i].bConnect;
            memcpy(tLink.list[tLink.num].MAC, ptDevStatus->PortCfg[i].porttCfg.MAC, 6);
            tLink.num += 1;
        }
    }
    
    status = RADIO_SUCCESS;
end:
    /* 构造返回消息 */
    strmsg = radio_getconnect_res_package(pInMsg->token, &tLink, status);
    if(strmsg)
    {
        sprintf(topic, "%s/%s/%s/%s/%s/%s", pInMsg->destapp, pInMsg->sourapp, MQTT_TOPIC_JSON, pInMsg->operat,
                                            MQTT_TOPIC_INFO_RES, pInMsg->infopath);
        ret = radio_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }
    return ret;
}

/**********************************************************************
* @name      : radio_json_sendmsg_req
* @brief     ：发送消息请求
* @param[in] ：MESSAGE_JSON_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_json_sendmsg_req(MESSAGE_JSON_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    RADIO_MSG_T       tMsgSend;
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    int             ret = 0;
    uint8           prio = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 发送消息失败\n");
        goto end;
    }

    memset(&tMsgSend, 0, sizeof(RADIO_MSG_T));
    /* 消息解析 */
    ret = radio_sendmsg_req_split(pInMsg->infopath, pInMsg->pObject, &tMsgSend, &prio);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("发送消息解析失败, %d\n", ret);
        goto end;
    }
    if(tMsgSend.port >= RADIO_PORT_MAX)
    {
        MQTTMSG_FMT_DEBUG("无效端口 %d\n", tMsgSend.port);
        goto end;
    }
    
    pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_MSG_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_MSG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = prio;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = RADIO_IOP_SEND_DATA;
    pMsgDev->datalen = sizeof(RADIO_MSG_T);
    memcpy(pMsgDev->data, &tMsgSend, pMsgDev->datalen);
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.msgQueue);
    radio_write_queue(pQueue, pMsgDev, TRUE);

    return 0;
end:
    return radio_err_json_res(pInMsg, RADIO_ERR_OTHER_FAIL, NULL);
}

/**********************************************************************
* @name      : radio_json_transmsg_req
* @brief     ：透传消息请求
* @param[in] ：MESSAGE_JSON_T *pInMsg   请求消息
               RADIO_MSGHANDLE_T *pHandle 句柄
               RADIO_DATA_T *pPublic      公共消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_json_transmsg_req(MESSAGE_JSON_T *pInMsg, RADIO_MSGHANDLE_T *pHandle, RADIO_DATA_T *pPublic)
{
    char    topic[256] = {0};
    char   *strmsg = NULL;
    int     ret = 1;
    RADIO_TRANS_ACK_T tAck;
    RADIO_TRANS_REQ_T tMsgTrans;
    RADIO_QUEUE_T    *pQueue = NULL;
    RADIO_DEV_MSG_T  *pMsgDev = NULL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("电台设备不存在, 发送消息失败\n");
        goto end;
    }

    memset(&tMsgTrans, 0, sizeof(RADIO_TRANS_REQ_T));
    /* 消息解析 */
    ret = radio_transmsg_req_split(pInMsg->infopath, pInMsg->pObject, &tMsgTrans);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("电台转发消息解析失败\n");
        goto end;
    }

    /* 合法性判断 不支持上行通道使用透传 */
    if(tMsgTrans.port > RADIO_PORT_MAX || tMsgTrans.port <= RADIO_PORT_MASTER1)
    {
        MQTTMSG_FMT_DEBUG("无效端口 %d\n", tMsgTrans.port);
        goto end;
    }
    if(tMsgTrans.match.mode > RADIO_MATCH_OB)
    {
        MQTTMSG_FMT_DEBUG("配对模式错误%d\n", tMsgTrans.match.mode);
        goto end;
    }

    if(0 == tMsgTrans.waittime.msgtime)
    {
        /* 默认等待500ms */
        tMsgTrans.waittime.msgtime = 500;
    }
    if(0 == tMsgTrans.waittime.bytetime)
    {
        tMsgTrans.waittime.bytetime = 20;
    }
    
    pMsgDev = (RADIO_DEV_MSG_T*)malloc(sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_TRANS_REQ_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("申请内存失败\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(RADIO_DEV_MSG_T) + sizeof(RADIO_TRANS_REQ_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = RADIO_IOP_TRAN_DATA;
    pMsgDev->datalen = sizeof(RADIO_TRANS_REQ_T);
    memcpy(pMsgDev->data, &tMsgTrans, pMsgDev->datalen);
    
    /* 写入消息队列 */
    pQueue = &(pPublic->tDevPorc.transQueue);
    radio_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    tAck.ret = RADIO_ERR_OTHER_FAIL;
    tAck.len = 0;
    strmsg = radio_transmsg_res_package(pInMsg->token, &tAck);
    if(strmsg)
    {
        sprintf(topic, "%s/%s/%s/%s/%s/%s", pInMsg->destapp, pInMsg->sourapp, MQTT_TOPIC_JSON, pInMsg->operat,
                                            MQTT_TOPIC_INFO_RES, pInMsg->infopath);
        ret = radio_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }
    return ret;
}

/**********************************************************************
* @name      : radio_json_msg_proc
* @brief     ：JSON消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo       消息
               RADIO_MSGHANDLE_T *ptMsgHandle   私有数据
               RADIO_DATA_T *ptRADIOData          共有数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-8-13
* @Update    :
**********************************************************************/
void radio_json_msg_proc(MESSAGE_JSON_T *pMsgInfo, RADIO_MSGHANDLE_T *ptMsgHandle, RADIO_DATA_T *ptRADIOData)
{
    /* 注册 心跳处理 */
    if(0 == strcmp(pMsgInfo->sourapp, SYS_APP_NAME))
    {
        if(0 == strcmp(pMsgInfo->infopath, SYS_TOPIC_REGISTER))
        {
            ptRADIOData->bReg = TRUE;
            RADIO_FMT_DEBUG("电台APP注册\n");
        }
        else if(0 == strcmp(pMsgInfo->infopath, SYS_TOPIC_KEEPALIVE))
        {
            radio_heart_bear(pMsgInfo);
        }
        return;
    }

    /* JSON消息处理 */
    if((0 == strcmp(pMsgInfo->destapp, RADIO_APP_NAME) || 0 == strcmp(pMsgInfo->destapp, RADIO_APP_NAME))
       && 0 == strcmp(pMsgInfo->infotype, MQTT_TOPIC_INFO_REQ))
    {
        if(0 == strcmp(pMsgInfo->infopath, RADIO_TOPOC_SETPARAM) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_SET))
        {
            /* 电台参数设置 */
            radio_json_setparam_req(pMsgInfo, ptMsgHandle, ptRADIOData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, RADIO_TOPOC_GETPARAM) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_GET))
        {
            /* 电台参数查询 */
            radio_json_getparam_req(pMsgInfo, ptMsgHandle, ptRADIOData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, RADIO_TOPOC_SETPORT) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_SET))
        {
            /* 端口配置 */
            radio_json_setport_req(pMsgInfo, ptMsgHandle, ptRADIOData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, RADIO_TOPOC_SETDETECT) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_SET))
        {
            /* 检表配置 */
            radio_json_setcheck_req(pMsgInfo, ptMsgHandle, ptRADIOData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, RADIO_TOPOC_CONNINFO) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_GET))
        {
            /* 电台连接信息查询 */
            radio_json_getlink_req(pMsgInfo, ptMsgHandle, ptRADIOData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, RADIO_TOPOC_SENDDATA) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_ACT))
        {
            /* 发送消息 */
            radio_json_sendmsg_req(pMsgInfo, ptMsgHandle, ptRADIOData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, RADIO_TOPOC_FORWARD) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_ACT))
        {
            /* 透传 */
            radio_json_transmsg_req(pMsgInfo, ptMsgHandle, ptRADIOData);
        }
        else
        {
            /* 错误消息应答 */
            radio_err_json_res(pMsgInfo, RADIO_ERR_OTHER_FAIL, "Unsupport operation");
        }
    }
    
    return;
}
#endif
#endif

#if DESC("电台状态显示", 1)
/**********************************************************************
* @name      : radio_state_guishow
* @brief     ：电台状态显示刷新
* @param[in] ：uint8 status  电台连接状态(不显[0],长显[1])
* @param[out]：
* @return    ：0-成功/错误码 
**********************************************************************/
int radio_state_guishow(uint8 status)
{
    uint8           buf[420] = {0};
    uint16          msglen = 0;
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T *ptAXDR = &(ptMsgNew->tAXDR);
    uint8          *temp = ptAXDR->playload;
    char           *dstApp = "M-desktopGui";
    char           *srcApp = "M-radioManager";

    //刷新工具栏
    temp[msglen++] = 0x01;
    temp[msglen++] = 0x07;
    temp[msglen++] = status;

    MQTTMSG_FMT_DEBUG("radio_state_guishow status=%u\n", status); 

    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 1;
    ptAXDR->index = radio_get_next_index();
    ptAXDR->priority = 0;
    ptAXDR->label = 0;
    ptAXDR->IID = MESSAGE_IID_DESKGUI;
    ptAXDR->IOP = DESKGUI_IOP_FRESH_TOOL;
    
    ptAXDR->sourlen = strlen(srcApp);
    memcpy(ptAXDR->souraddr , srcApp, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(dstApp);
    memcpy(ptAXDR->destaddr, dstApp, ptAXDR->destlen);

    ptAXDR->msglen = msglen;
    
    return radio_send_msg(ptMsgNew, 0);
}
#endif

#if DESC("电台连接状态显示", 1)
/**********************************************************************
* @name      : radio_connectionstate_guishow
* @brief     ：电台连接状态显示刷新
* @param[in] ：char  *str  内容
               uint16 len  长度
               uint32 times 刷新时间
* @param[out]：
* @return    ：0-成功/错误码 
**********************************************************************/
int radio_connectionstate_guishow(char *str, uint16 len, uint32 times)
{
    uint8           buf[420] = {0};
    uint16          msglen = 0;
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T *ptAXDR = &(ptMsgNew->tAXDR);
    uint8          *temp = ptAXDR->playload;
    char           *dstApp = "M-desktopGui";
    char           *srcApp = "M-radioManager";

    uint8  lenBuf[3] = {0};
    uint8  offset = 0;
    uint32 timelen = MAX(5, times);

    appmsg_memcpy_r(temp + msglen, &timelen, sizeof(uint32));
    msglen += sizeof(uint32);

    offset = appmsg_set_len_offset(len, lenBuf);
    memcpy(temp + msglen, lenBuf, offset);
    msglen += offset;

    memcpy(temp + msglen, str, len);
    msglen += len;

    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 1;
    ptAXDR->index = radio_get_next_index();
    ptAXDR->priority = 0;
    ptAXDR->label = 0;
    ptAXDR->IID = MESSAGE_IID_DESKGUI;
    ptAXDR->IOP = DESKGUI_IOP_FRESH_STATE;
    
    ptAXDR->sourlen = strlen(srcApp);
    memcpy(ptAXDR->souraddr , srcApp, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(dstApp);
    memcpy(ptAXDR->destaddr, dstApp, ptAXDR->destlen);

    ptAXDR->msglen = msglen;
    
    return radio_send_msg(ptMsgNew, 0);
}
#endif

/**********************************************************************
* @name      : radio_mqttproc_init
* @brief     ：mqtt消息处理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int radio_mqttproc_init(void *pPublic, void **ppPrivate)
{
    RADIO_MSGHANDLE_T *ptMsgData = NULL;
    RADIO_DATA_T *ptPData = (RADIO_DATA_T*)pPublic;

    ptMsgData = (RADIO_MSGHANDLE_T *)malloc(sizeof(RADIO_MSGHANDLE_T));
    if(NULL == ptMsgData)
    {
        RADIO_FMT_DEBUG("初始化失败\n");
        return -1;
    }
    memset(ptMsgData, 0, sizeof(RADIO_MSGHANDLE_T));

    /* 先发送一次APP注册消息 */
    #ifdef PRODUCT_SCU
    radio_login_to_sys();
    #else
    radio_login_to_smios(ptPData->appid, ptPData->appidlen);

    //先将电台连接状态设置为不显示
    radio_state_guishow(0);
    #endif
    *ppPrivate = ptMsgData;
    return 0;
}

/**********************************************************************
* @name      : radio_mqttmsg_proc
* @brief     ：mqtt消息处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2021-7-8
* @Update    :
**********************************************************************/
int radio_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int         ret = 0;
    int         recvlen = 0;
    RADIO_MSGHANDLE_T *ptMsgHandle = (RADIO_MSGHANDLE_T*)pPrivate;
    RADIO_DATA_T      *ptWireData = (RADIO_DATA_T*)pPublic;
    struct timespec  curtime = {0};
    MESSAGE_INFO_T  *pMsgInfo = NULL;
    static uint8     sbProcFirst = TRUE;

    if(TRUE == sbProcFirst)
    {
        BASE_TASK_SET_OK(pArg); //基础任务完成准备工作
        sbProcFirst = FALSE;
    }
    
    /* 1 收消息 */
    recvlen = radio_recv_mqtt(ptMsgHandle->topic, 256, ptMsgHandle->recvBuf, RADIO_MQTT_MSG_LEN);
    if(recvlen > 0)
    {
        ret = appmsg_split(ptMsgHandle->recvBuf, (uint16)recvlen , &pMsgInfo);
        if(0 == ret && NULL != pMsgInfo)
        {
            /* AXDR格式 */
            MQTTMSG_BUF_DEBUG(ptMsgHandle->recvBuf, recvlen, "Recv msg from %s, iid 0x%04x, iop 0x%04x", 
                                               pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
            if(NULL != strstr(pMsgInfo->souraddr, RADIO_APP_NAME))
            {
                MQTTMSG_FMT_DEBUG("Revc msg from %s, iid 0x%x, iop 0x%x\n", pMsgInfo->souraddr, 
                                  pMsgInfo->IID, pMsgInfo->IOP);
            }
            else
            {
                radio_axdr_msg_proc(pMsgInfo, ptMsgHandle, ptWireData);
            }

            /* 资源释放 */
            free(pMsgInfo);
            pMsgInfo = NULL;
            memset(ptMsgHandle->recvBuf, 0, recvlen);
            memset(ptMsgHandle->topic, 0, 256);
        }
    }

    /* 2. 定时处理 */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(curtime.tv_sec - ptMsgHandle->timeflag < 5)
    {
        return 0;
    }
    ptMsgHandle->timeflag = curtime.tv_sec;
    /* APP相关 */
    if(FALSE == ptWireData->bReg)
    {
        radio_login_to_smios(ptWireData->appid, ptWireData->appidlen);
    }
    
    return 0;
}

/**********************************************************************
* @name      : radio_status_get
* @brief     ：查询电台状态
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
int radio_status_get(int fd, RADIO_DEV_MSG_T *pMsg)
{
    uint8           buf[360] = {0};
    uint16  index = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;

    uint8           rcv_buf[100] = {0};
    uint8           status = 0;
    uint16          fieldStrength = 0;
    int             at_status = 0;

    do{
        if (NULL == pMsg)
        {
            RADIO_FMT_DEBUG("radio_status_get null pointer\n");
            break;
        }

        RADIO_FMT_DEBUG("radio_status_get 查询电台状态处理\n");

        if (fd < 0)
        {
            RADIO_FMT_DEBUG("电台句柄无效, 查询失败\n");
            break;
        }
        else
        {
             memset(rcv_buf, 0x0, sizeof(rcv_buf));
            at_status = AtGet(fd, "AT+STATUS=?\r", rcv_buf, sizeof(rcv_buf));    /*串口发送AT命令*/

            if (at_status > 0)
            {
                char *radio_status = strstr((char *)rcv_buf, "AT+STATUS=");

                if (radio_status != NULL)
                {
                    radio_status += 10; //readio 在rcv_buf中的偏移量
                    status = atoi(radio_status);
                    MQTTMSG_FMT_DEBUG("当前电台网络状态:%u\n", status);
                }
            }

                memset(rcv_buf, 0x0, sizeof(rcv_buf));
                at_status = AtGet(fd, "AT+FILED=999DB\r", rcv_buf, sizeof(rcv_buf));    /*串口发送AT命令*/

                if (at_status > 0)
                {
                    /*
                    AT+FIELD=999DB\r
                    返回：AT+FIELD=XXXXDB
                    XXXX为当前场强值。如上小于四位前面补0，最大为5000，表示50.00dBuV。
                    */
                    char *ptemp = NULL;
                    if ((ptemp = strstr((char *)rcv_buf, "AT+FIELD=")) != NULL )
                    {
                        ptemp += 9;         //"AT+FIELD= 在rcv_buf中的偏移量
                        ptemp[4] = 0;
                        fieldStrength = atoi(ptemp);
                        MQTTMSG_FMT_DEBUG("当前电台场强:%u\n", fieldStrength);
                    }
                }
        }

    }while(0);

    /* 构造返回消息 */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pMsg->priority;
    ptAXDR->index = ((pMsg->token >> 16) & 0xFFFF);
    ptAXDR->label = pMsg->token;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    ptAXDR->destlen = strlen(pMsg->source);
    memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

    ptAXDR->IID = MESSAGE_IID_RADIO;
    ptAXDR->IOP = pMsg->proc;
    ptAXDR->playload[index++]  = status;
    memcpy_r(ptAXDR->playload+index,&fieldStrength,2);
    index+=2;
    ptAXDR->playload[index++] = g_RadioSpeakerFlag;

    ptAXDR->msglen = index;
    
    return radio_send_axdr_msg(ptAXDR, 0);
}

