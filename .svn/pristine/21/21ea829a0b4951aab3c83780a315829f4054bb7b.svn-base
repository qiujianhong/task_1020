/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   电台APP 设备操作
* @date：    2021-7-10
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_split.h"
#include "framecomm.h"
#include "db_struct.h"
#include "radio_data.h"
#include "radio_mqtt.h"
#include "radio_uart.h"
#include "radio_protocol.h"
#include "rs.h"

#if PIN_ENABLE == 1
#include "OopAll.h"
#endif

#include "radio_dev.h"

#include "radio_msg.h"

uint8    g_bRadioRS = FALSE;
uint8 radio_state;
uint8 answer_cch[5];

#if DESC("电台状态刷新", 1)

#if 0
/**********************************************************************
* @name      : 电台连接状态刷新
* @brief     ：电台模块主动上报连接信息处理
* @param[in] ：RADIO_DATA_T *pRadiodata    电台数据
               BOOL        bInCnt      是否在连接过程中
* @param[out]：
* @return    ：
**********************************************************************/
void bel_refresh_status(RADIO_DATA_T *pRadiodata, BOOL bInCnt)
{
    uint8      i = 0;
    BOOL       bConnect = FALSE;
    RADIO_DEV_T *pData = &pRadiodata->tDevStatus;

    /* 主判断 */
    for(i = 0; i < RADIO_MASTER_MAX; i++)
    {
        if (pData->mPortStatus[i].bConnect == TRUE)
        {
            bConnect = TRUE;
            break;
        }
    }

    /* 从判断 */
    if (FALSE == bConnect)
    {
        for(i = 0; i < RADIO_SLAVE_MAX; i++)
        {
            if (pData->sPortStatus[i].bConnect == TRUE)
            {
                bConnect = TRUE;
                break;
            }
        }
    }

    if (bConnect)
    {
        bel_state_guishow(2);
    }
    else
    {
        if (bInCnt)
        {
            bel_state_guishow(1);
        }
        else
        {
            bel_state_guishow(0);
        }
    }

    return;
}
#endif
#endif

#if DESC("综合操作", 1)

#define RADIO_MSG_MAX_LEN    4096

///* 报文通知 */
//typedef struct tagRADIO_MSG_EVENT_T
//{
//    uint16   len;                     /* 报文长度 */
//    uint8    msg[RADIO_MSG_MAX_LEN];    /* 报文内容 */
//}RADIO_MSG_EVENT_T;

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
uint16 radio_msg_event_package(RADIO_MSG_EVENT_T *pMsgIn, uint8 *pBuf, uint16 buflen)
{
    uint16 offset = 0;
    uint8  byte[5] = {0};
    uint8  bytenum = 0;
    
    if(NULL == pMsgIn || NULL == pBuf || buflen < 3)
    {
        return 0;
    }

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
* @name      : radio_send_msgevent
* @brief     ：对外消息发送接收消息
* @param[in] ：RADIO_MSG_EVENT_T *ptMsgEvent  对外发送的消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_send_msgevent(RADIO_MSG_EVENT_T *ptMsgEvent)
{
    uint8           buf[4600] = {0};
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;
    uint8          *temp = ptAXDR->playload;
    //char           *strmsg = NULL;
    int             ret = 0;
    //char            topic[256] = {0};
    
    if(NULL == ptMsgEvent)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptAXDR->rpm = 1;
    ptAXDR->priority = 0;
    ptAXDR->index = radio_get_next_index();
    ptAXDR->label = 0;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    ptAXDR->destlen = strlen("Broadcast");
    memcpy(ptAXDR->destaddr, "Broadcast", ptAXDR->destlen);


    ptAXDR->IID = MESSAGE_IID_RADIO;
    ptAXDR->IOP = RADIO_IOP_RECV_EVENT;

    ptAXDR->msglen = radio_msg_event_package(ptMsgEvent, temp, 4200);
    
    #ifdef PRODUCT_SCU
    ret = radio_send_axdr_msg(ptAXDR, 0);
    #else
    ret = radio_send_axdr_msg_broadcast(ptAXDR, 0, MESSAGE_IID_RADIO, RADIO_IOP_RECV_EVENT);
    #endif

    /*strmsg = radio_msg_notify_package(ptAXDR->index, ptMsgEvent);
    if(strmsg)
    {
        sprintf(topic, "%s/Broadcast/%s/%s/%s/%s", RADIO_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_REPORT,
                                                   MQTT_TOPIC_INFO_RES, RADIO_TOPOC_RECVNOTIFY);
        ret |= radio_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }*/
    return ret;
}

/**********************************************************************
* @name      : radio_devport_report
* @brief     ：端口上送消息
* @param[in] ：RADIO_DEV_PORT_T *pDevPort     端口
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-9-15
* @Update    :
**********************************************************************/
void radio_devport_report(uint8 *buf, uint16 bufLen)
{
    int              ret = 0;

    /* 上报通知 */
    RADIO_MSG_EVENT_T tEvent;

    memset(&tEvent, 0, sizeof(RADIO_MSG_EVENT_T));

    tEvent.len = bufLen;
    memcpy(tEvent.msg, buf, bufLen);
    ret = radio_send_msgevent(&tEvent);
    if(0 != ret)
    {
        RADIO_BUF_DEBUG(tEvent.msg, tEvent.len, "收到消息上送失败\n");
    }
    else
    {
        MQTTMSG_BUF_TRACE(tEvent.msg, tEvent.len, "消息上送\n");
    }
}
#if 0
/**********************************************************************
* @name      : radio_send_msgevent_json
* @brief     ：对外消息发送接收消息
* @param[in] ：RADIO_MSG_EVENT_T *ptMsgEvent  对外发送的消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_send_portevent(RADIO_PORT_EVENT_T *ptPortEvent)
{
    uint8         buf[512] = {0};
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;
    uint8          *temp = ptAXDR->playload;
    char           *strmsg = NULL;
    int             ret = 0;
    char            topic[256] = {0};
    
    if(NULL == ptPortEvent)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptAXDR->rpm = 1;
    ptAXDR->priority = 0;
    ptAXDR->index = radio_get_next_index();
    ptAXDR->label = 0;

    ptAXDR->sourlen = strlen(RADIO_APP_NAME_M);
    memcpy(ptAXDR->souraddr, RADIO_APP_NAME_M, ptAXDR->sourlen);

    ptAXDR->destlen = strlen("Broadcast");
    memcpy(ptAXDR->destaddr, "Broadcast", ptAXDR->destlen);


    ptAXDR->IID = MESSAGE_IID_RADIO;
    ptAXDR->IOP = RADIO_IOP_PORT_EVENT;

    ptAXDR->msglen = radio_port_event_package(ptPortEvent, temp, 256);
    
    #ifdef PRODUCT_SCU
    ret = radio_send_axdr_msg(ptAXDR, 0);
    #else
    ret = radio_send_axdr_msg_broadcast(ptAXDR, 0, MESSAGE_IID_RADIO, RADIO_IOP_RECV_EVENT);
    #endif

    strmsg = radio_port_notify_package(ptAXDR->index, ptPortEvent);
    if(strmsg)
    {
        sprintf(topic, "%s/Broadcast/%s/%s/%s/%s", RADIO_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_REPORT,
                                                   MQTT_TOPIC_INFO_RES, RADIO_TOPOC_PORTNOTIFY);
        ret |= radio_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }

    RADIO_FMT_DEBUG("端口%d, MAC %02X:%02X:%02X:%02X:%02X:%02X, %s\n", ptPortEvent->port, ptPortEvent->MAC[0], 
                   ptPortEvent->MAC[1], ptPortEvent->MAC[2], ptPortEvent->MAC[3], ptPortEvent->MAC[4], 
                   ptPortEvent->MAC[5], ptPortEvent->bConnect ? "连接" : "断开");
    return ret;

}

/**********************************************************************
* @name      : radio_dev_msgreport_proc
* @brief     ：电台模块接收消息上送
* @param[in] ：RADIO_PRODATA_T *pProData 消息解析
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_dev_msgreport_proc(int fd, RADIO_PRODATA_T *pProData, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    uint16     i = 0;
    uint32     port = RADIO_PORT_MAX;
    uint8      newdata = 0;
    uint8     *temp = pProData->data;
    int        ret = 0;
    RADIO_DEV_T *pDevStatus = NULL;
    struct timespec curtime = {0};
    RADIO_MSG_EVENT_T tMsgEvent;

    RADIO_PRODATA_T  *pProRet = NULL;

    //电台检定时的特殊MAC地址
    uint8 MAC[6] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee};
    
    if(NULL == pProData || NULL == pRadioHandle || NULL == pRadiodata)
    {
        return -1;
    }

    memset(&tMsgEvent, 0, sizeof(tMsgEvent));

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    pDevStatus = &pRadiodata->tDevStatus;

    if(pProData->datalen > 4 && 0xFE == temp[0] && 0xFE == temp[1] && 0xFE == temp[2] && 0xFE == temp[3])
    {
        newdata = TRUE;
    }

    //电台检定时收到的MAC地址切换到全EE,需要建立虚拟通道
    if((0 == memcmp(MAC, pProData->mac, 6)) && 
       (0 != memcmp(MAC, pDevStatus->mPortStatus[0].MAC, 6)))
    {
        /* 申请内存 */
        pProRet = (RADIO_PRODATA_T*)malloc(7 + sizeof(RADIO_PRODATA_T));
        if(NULL == pProRet)
        {
            return -3;
        }
        memset(pProRet, 0, 7 + sizeof(RADIO_PRODATA_T));
        pProRet->datalen = 7;

        pProRet->data[0] = 0x01;
        memcpy(&pProRet->data[1], MAC, 6);

        radio_dev_report_conn_proc(0, pProRet, pRadioHandle, pRadiodata);

        free(pProRet);
    }

    /* 查询端口 */
    for(i = 0; i < RADIO_MASTER_MAX; i++)
    {
        if(0 == memcmp(pDevStatus->mPortStatus[i].MAC, pProData->mac, 6))
        {
            port = pDevStatus->mPortStatus[i].port;
            pDevStatus->mPortStatus[i].lasttime = curtime.tv_sec;
            pDevStatus->mPortStatus[i].lastus = curtime.tv_nsec / 1000;
            if(pDevStatus->mPortStatus[i].cachelen + pProData->datalen > RADIO_MSG_MAX_LEN || TRUE == newdata)
            {
                /* 发送数据 */
                if(pDevStatus->mPortStatus[i].cachelen > 0)
                {
                    tMsgEvent.port = pDevStatus->mPortStatus[i].port;
                    memcpy(tMsgEvent.MAC, pDevStatus->mPortStatus[i].MAC, 6);
                    tMsgEvent.len = pDevStatus->mPortStatus[i].cachelen;
                    memcpy(tMsgEvent.msg, pDevStatus->mPortStatus[i].cache, tMsgEvent.len);
                    
                    memset(pDevStatus->mPortStatus[i].cache, 0, pDevStatus->mPortStatus[i].cachelen);
                    pDevStatus->mPortStatus[i].cachelen = 0;
                }
            }

            memcpy(pDevStatus->mPortStatus[i].cache + pDevStatus->mPortStatus[i].cachelen, pProData->data, pProData->datalen);
            pDevStatus->mPortStatus[i].cachelen += pProData->datalen;
            break;
        }
    }
    if(port >= RADIO_PORT_MAX)
    {
        for(i = 0; i < RADIO_SLAVE_MAX; i++)
        {
            if(0 == memcmp(pDevStatus->sPortStatus[i].MAC, pProData->mac, 6))
            {
                port = pDevStatus->sPortStatus[i].port;
                pDevStatus->sPortStatus[i].lasttime = curtime.tv_sec;
                pDevStatus->sPortStatus[i].lastus = curtime.tv_nsec / 1000;
                if(pDevStatus->sPortStatus[i].cachelen + pProData->datalen > RADIO_MSG_MAX_LEN || TRUE == newdata)
                {
                    /* 发送数据 */
                    if(pDevStatus->sPortStatus[i].cachelen > 0)
                    {
                        tMsgEvent.port = pDevStatus->sPortStatus[i].port;
                        memcpy(tMsgEvent.MAC, pDevStatus->sPortStatus[i].MAC, 6);
                        tMsgEvent.len = pDevStatus->sPortStatus[i].cachelen;
                        memcpy(tMsgEvent.msg, pDevStatus->sPortStatus[i].cache, tMsgEvent.len);
                        
                        memset(pDevStatus->sPortStatus[i].cache, 0, pDevStatus->sPortStatus[i].cachelen);
                        pDevStatus->sPortStatus[i].cachelen = 0;
                    }
                }

                memcpy(pDevStatus->sPortStatus[i].cache + pDevStatus->sPortStatus[i].cachelen, pProData->data, pProData->datalen);
                pDevStatus->sPortStatus[i].cachelen += pProData->datalen;
                break;
            }
        }
    }
    
    if(port >= RADIO_PORT_MAX)
    {
        RADIO_FMT_DEBUG("查询不到端口 %02X:%02X:%02X:%02X:%02X:%02X\n", pProData->mac[0], pProData->mac[1],
                      pProData->mac[2], pProData->mac[3], pProData->mac[4], pProData->mac[5]);
        return -2;
    }
    

    /* 刷新时间 */
    pDevStatus->PortCfg[port].lasttime = curtime.tv_sec;
    pDevStatus->PortCfg[port].lastus = curtime.tv_nsec / 1000;

    if(0 == tMsgEvent.len)
    {
        return 0;
    }
    /* 超过4K缓存 发送通知消息 */
    ret = radio_send_msgevent(&tMsgEvent);
    if(0 != ret)
    {
        RADIO_BUF_DEBUG(tMsgEvent.msg, tMsgEvent.len, "收到消息上送失败 %02X:%02X:%02X:%02X:%02X:%02X\n", 
                      pProData->mac[0], pProData->mac[1], pProData->mac[2], pProData->mac[3], 
                      pProData->mac[4], pProData->mac[5]);
    }

    return ret;
}

/**********************************************************************
* @name      : radio_dev_msgreport_later
* @brief     ：电台模块接收消息上送后续处理 串口一次接收的数据认为是一组报文
* @param[in] ：RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-2-16
* @Update    :
**********************************************************************/
void radio_dev_msgreport_later(RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    uint16     i = 0;
    int        ret = 0;
    RADIO_DEV_T *pDevStatus = NULL;
    RADIO_MSG_EVENT_T tMsgEvent;
    
    if(NULL == pRadioHandle || NULL == pRadiodata)
    {
        return;
    }

    memset(&tMsgEvent, 0, sizeof(tMsgEvent));

    pDevStatus = &pRadiodata->tDevStatus;
    /* 查询端口 */
    for(i = 0; i < RADIO_MASTER_MAX; i++)
    {
        /* 发送数据 */
        if(pDevStatus->mPortStatus[i].cachelen > 0)
        {
            tMsgEvent.port = pDevStatus->mPortStatus[i].port;
            memcpy(tMsgEvent.MAC, pDevStatus->mPortStatus[i].MAC, 6);
            tMsgEvent.len = pDevStatus->mPortStatus[i].cachelen;
            memcpy(tMsgEvent.msg, pDevStatus->mPortStatus[i].cache, tMsgEvent.len);

            /* 发送通知消息 */
            ret = radio_send_msgevent(&tMsgEvent);
            if(0 != ret)
            {
                RADIO_BUF_DEBUG(tMsgEvent.msg, tMsgEvent.len, "收到消息上送失败[%d] %02X:%02X:%02X:%02X:%02X:%02X\n", ret, 
                                               tMsgEvent.MAC[0], tMsgEvent.MAC[1], tMsgEvent.MAC[2], tMsgEvent.MAC[3], 
                                               tMsgEvent.MAC[4], tMsgEvent.MAC[5]);
            }
    
            memset(pDevStatus->mPortStatus[i].cache, 0, pDevStatus->mPortStatus[i].cachelen);
            pDevStatus->mPortStatus[i].cachelen = 0;
        }
    }

    for(i = 0; i < RADIO_SLAVE_MAX; i++)
    {
        /* 发送数据 */
        if(pDevStatus->sPortStatus[i].cachelen > 0)
        {
            tMsgEvent.port = pDevStatus->sPortStatus[i].port;
            memcpy(tMsgEvent.MAC, pDevStatus->sPortStatus[i].MAC, 6);
            tMsgEvent.len = pDevStatus->sPortStatus[i].cachelen;
            memcpy(tMsgEvent.msg, pDevStatus->sPortStatus[i].cache, tMsgEvent.len);

            /* 发送通知消息 */
            ret = radio_send_msgevent(&tMsgEvent);
            if(0 != ret)
            {
                RADIO_BUF_DEBUG(tMsgEvent.msg, tMsgEvent.len, "收到消息上送失败 %02X:%02X:%02X:%02X:%02X:%02X\n", 
                                               tMsgEvent.MAC[0], tMsgEvent.MAC[1], tMsgEvent.MAC[2], tMsgEvent.MAC[3], 
                                               tMsgEvent.MAC[4], tMsgEvent.MAC[5]);
            }
            
            memset(pDevStatus->sPortStatus[i].cache, 0, pDevStatus->sPortStatus[i].cachelen);
            pDevStatus->sPortStatus[i].cachelen = 0;
        }
    }
}

/**********************************************************************
* @name      : radio_dev_report_info_proc
* @brief     ：电台模块主动上报设备状态处理
* @param[in] ：int fd                  设备句柄
               RADIO_PRODATA_T *pProData 消息解析
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-16
* @Update    :
**********************************************************************/
int radio_dev_report_info_proc(int fd, RADIO_PRODATA_T *pProData, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    uint8     *temp = pProData->data;
    char      *modelstr[] = {"模块复位", "恢复出厂", "进入睡眠", "进入睡眠"};
    uint8      sendbuf[2] = {0};
    
    if(*temp < RADIO_MODEL_RESTART || *temp > RADIO_MODEL_WAKE)
    {
        RADIO_FMT_DEBUG("未知模块信息%d\n", *temp);
        return -1;
    }

    RADIO_FMT_DEBUG("模块上报新状态: %s\n", modelstr[*temp - 1]);

    pRadiodata->tDevStatus.bSleep = RADIO_MODEL_SLEEP == *temp ? TRUE : FALSE;

    //模块复位后，需要重新设置参数
    if (*temp == 1)
    {
        pRadioHandle->status = RADIO_DEV_STATUS_PREPARE;
    }
    
    /* 应答 */
    sendbuf[0] = *temp;
    return radio_report_response(fd, CMD_MODULE_INFO, sendbuf, 2);
}

/**********************************************************************
* @name      : radio_dev_report_conn_proc
* @brief     ：电台模块主动上报连接信息处理
* @param[in] ：int fd                  设备句柄
               RADIO_PRODATA_T *pProData 消息解析
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_dev_report_conn_proc(int fd, RADIO_PRODATA_T *pProData, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    RADIO_DEV_T *pData = &pRadiodata->tDevStatus;
    uint8     *temp = pProData->data;
    uint8      sendbuf[1] = {0};
    uint8      status;
    uint8      MAC[6] = {0};
    uint8      MACNO[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    RADIO_PORT_EVENT_T tEvent;
    struct timespec curtime = {0};
    uint8      i = 0;
    uint8      j = 0;
    
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    memset(&tEvent, 0, sizeof(RADIO_PORT_EVENT_T));
    status = *temp;
    temp++;

    /* 主判断 */
    for(i = 0; i < RADIO_MASTER_MAX; i++)
    {
        memcpy(tEvent.MAC, MACNO, 6);
        tEvent.port = i;
        tEvent.mode = RADIO_MODE_MASTER;

        if((status >> i) & 0x01)
        {
            memcpy(MAC, temp, 6);
            temp += 6;
            if(0 != memcmp(MAC, pData->mPortStatus[i].MAC, 6))
            {
                /* MAC变化 */
                memcpy(pData->mPortStatus[i].MAC, MAC, 6);
                
                memcpy(tEvent.MAC, pData->mPortStatus[i].MAC, 6);
                tEvent.bConnect = TRUE;
            }

            pData->mPortStatus[i].bConnect = TRUE;
            pData->mPortStatus[i].lasttime = curtime.tv_sec;
            pData->mPortStatus[i].lastus = curtime.tv_nsec / 1000;
            
            pData->PortCfg[i].lasttime = curtime.tv_sec;
            pData->PortCfg[i].lastus = curtime.tv_nsec / 1000;
            pData->PortCfg[i].bConnect = TRUE;
            memcpy(pData->PortCfg[i].porttCfg.MAC, pData->mPortStatus[i].MAC, 6);
            pData->PortCfg[i].porttCfg.mode = RADIO_MODE_MASTER;
        }
        else if(pData->mPortStatus[i].bConnect)
        {
            memcpy(tEvent.MAC, pData->mPortStatus[i].MAC, 6);
            tEvent.bConnect = FALSE;
            
            memset(pData->mPortStatus[i].MAC, 0xFF, 6);
            pData->mPortStatus[i].bConnect = FALSE;
            
            pData->PortCfg[i].bConnect = FALSE;
            memcpy(pData->PortCfg[i].porttCfg.MAC, pData->mPortStatus[i].MAC, 6);
            pData->PortCfg[i].porttCfg.mode = RADIO_MODE_MASTER;
        }
        pData->PortCfg[i].channelid = i + 1;

        if(0 != memcmp(tEvent.MAC, MACNO, 6))
        {
            pData->mPortStatus[i].cachelen = 0;
            memset(pData->mPortStatus[i].cache, 0, RADIO_MSG_MAX_LEN);
            radio_send_portevent(&tEvent);
        }
    }

    /* 从判断 */
    for(i = 0; i < RADIO_SLAVE_MAX; i++)
    {
        memcpy(tEvent.MAC, MACNO, 6);
        tEvent.mode = RADIO_MODE_SLAVE;

        if((status >> (i + 2 )) & 0x01)
        {
            memcpy(MAC, temp, 6);
            temp += 6;
            if(0 != memcmp(MAC, pData->sPortStatus[i].MAC, 6) || pData->sPortStatus[i].port > RADIO_PORT_MAX)
            {
                memcpy(pData->sPortStatus[i].MAC, MAC, 6);
                /* 端口查询 */
                if(pData->sPortStatus[i].port > RADIO_PORT_MAX)
                {
                    for(j = 2; j < RADIO_PORT_MAX; j++)
                    {
                        if(pData->PortCfg[j].bConfig && 0 == memcmp(pData->PortCfg[j].porttCfg.MAC, MAC, 6))
                        {
                            pData->PortCfg[j].channelid = i + 1;
                            pData->PortCfg[j].bConnect = TRUE;
                            pData->sPortStatus[i].port = pData->PortCfg[j].porttCfg.port;
                            break;
                        }
                    }
                }
                
                if(pData->sPortStatus[i].port > RADIO_PORT_MAX)
                {
                    RADIO_FMT_DEBUG("查询不到端口: 从%d, MAC %02X:%02X:%02X:%02X:%02X:%02X\n", i + 1,
                                  MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);
                    continue;
                }
                
                memcpy(tEvent.MAC, pData->sPortStatus[i].MAC, 6);
                tEvent.bConnect = TRUE;
            }
            else if(0 == memcmp(MAC, pData->sPortStatus[i].MAC, 6) && FALSE == pData->sPortStatus[i].bConnect)
            {
                memcpy(tEvent.MAC, pData->sPortStatus[i].MAC, 6);
                tEvent.bConnect = TRUE;
            }
            
            pData->sPortStatus[i].bConnect = TRUE;
            pData->sPortStatus[i].lasttime = curtime.tv_sec;
            pData->sPortStatus[i].lastus = curtime.tv_nsec / 1000;
            
            if(pData->sPortStatus[i].port < RADIO_PORT_MAX)
            {
                pData->PortCfg[pData->sPortStatus[i].port].bConnect = FALSE;
                pData->PortCfg[pData->sPortStatus[i].port].lasttime = curtime.tv_sec;
                pData->PortCfg[pData->sPortStatus[i].port].lastus = curtime.tv_nsec / 1000;
                pData->PortCfg[pData->sPortStatus[i].port].bConnect = TRUE;
            }
        }
        else if(TRUE == pData->sPortStatus[i].bConnect)
        {
            /* 断开连接处理 */
            if(pData->sPortStatus[i].port < RADIO_PORT_MAX)
            {
                memcpy(tEvent.MAC, pData->sPortStatus[i].MAC, 6);

                pData->PortCfg[pData->sPortStatus[i].port].bConnect = FALSE;
                pData->PortCfg[pData->sPortStatus[i].port].channelid = i + 1;
            }
            
            memset(pData->sPortStatus[i].MAC, 0xFF, 6);
            pData->sPortStatus[i].bConnect = FALSE;
            pData->sPortStatus[i].port = 0xFF;
            
        }
        tEvent.port = pData->sPortStatus[i].port;

        if(0 != memcmp(tEvent.MAC, MACNO, 6) && tEvent.port < RADIO_PORT_MAX)
        {
            pData->sPortStatus[i].cachelen = 0;
            memset(pData->sPortStatus[i].cache, 0, RADIO_MSG_MAX_LEN);
            radio_send_portevent(&tEvent);
        }
    }

    //刷新桌面电台状态显示
    bel_refresh_status(pRadiodata, FALSE);

    /* 应答 */
    if(fd <= 0)
    {
        return 0;
    }
    return radio_report_response(fd, CMD_RADIO_CONN_INFO, sendbuf, 1);
}

/**********************************************************************
* @name      : radio_set_param_proc
* @brief     ：设置电台基本参数
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_CONF_T *ptParam   参数
               uint8 bFirst            是否首次执行
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_set_param_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_CONF_T *ptParam, uint8 bFirst)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint16 temp = 0;
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T   *pProRet = NULL;
    
    /* 1. 设置地址 */
    if(TRUE == bFirst)
    {
        memcpy(sendbuf, ptParam->MAC, 6);
        ret = radio_set_operate(fd, CMD_DEV_MAC_ADDR, sendbuf, 24, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                                               RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
        if(ret < 0)
        {
            return ret;
        }
        pRadioHandle->recvlen += ret;
    }
    else
    {
        sendbuf[0] = 1;
        MEMCPY_R(sendbuf + 1, ptParam->MAC, 6);
        ret = radio_set_operate(fd, CMD_DEV_ADDR, sendbuf, 7, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                              RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
        if(ret < 0)
        {
            return ret;
        }
        pRadioHandle->recvlen += ret;
    }
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台地址超时 %02X:%02X:%02X:%02X:%02X:%02X\n", ptParam->MAC[0], ptParam->MAC[1],
                      ptParam->MAC[2], ptParam->MAC[3], ptParam->MAC[4], ptParam->MAC[5]);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台地址成功 %02X:%02X:%02X:%02X:%02X:%02X\n", ptParam->MAC[0], ptParam->MAC[1],
                          ptParam->MAC[2], ptParam->MAC[3], ptParam->MAC[4], ptParam->MAC[5]);
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台地址失败(%d) %02X:%02X:%02X:%02X:%02X:%02X\n", status, ptParam->MAC[0], 
                          ptParam->MAC[1], ptParam->MAC[2], ptParam->MAC[3], ptParam->MAC[4], ptParam->MAC[5]);
            return status;
        }
    }

    /* 2. 工作参数设置 */
    memset(sendbuf, 0, 7);
    sendbuf[0] = ptParam->power;
    temp = ptParam->broadcast / 0.625;
    MEMCPY_R(sendbuf + 1, &temp, 2);
    temp = ptParam->scan/ 0.625;
    MEMCPY_R(sendbuf + 3, &temp, 2);
    ret = radio_set_operate(fd, CMD_RADIO_PARAMS, sendbuf, 5, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台工作参数超时, 功率%d, 广播间隔%dms, 扫描间隔%dms\n", ptParam->power, ptParam->broadcast,
                                                                                    ptParam->scan);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台工作参数成功, 功率%d, 广播间隔%dms, 扫描间隔%dms\n", ptParam->power, ptParam->broadcast,
                                                                                    ptParam->scan);
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台工作参数失败(%d) 功率%d, 广播间隔%dms, 扫描间隔%dms\n", status, ptParam->power, 
                           ptParam->broadcast, ptParam->scan);
            return status;
        }
    }

    /* 3. 名称设置 */
    if(0 == ptParam->len)
    {
        return 0;
    }
    memset(sendbuf, 0, 5);
    memcpy(sendbuf, ptParam->name, MIN(5, ptParam->len));
    ret = radio_set_operate(fd, CMD_DEV_NAME, sendbuf, 5, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台名称%s超时\n", sendbuf);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台名称%s成功\n", sendbuf);
        }
        else
        {
            /* 电台名称不支持设置 */
            RADIO_FMT_DEBUG("设置电台名称%s失败(%d)\n", sendbuf, status);
            return RADIO_SUCCESS;
        }
    }

    return 0;
}

/**********************************************************************
* @name      : radio_set_checkmeter_proc
* @brief     ：设置电台检定参数
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_CONF_T *ptParam   参数
               uint8 bFirst            是否首次执行
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_set_checkmeter_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata, RADIO_CHECK_T *ptParam)
{
    int    ret = 0;
    int    i = 0;
    uint8  sendbuf[24] = {0};
    uint16 offset = 0;
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T   *pProRet = NULL;

    memset(sendbuf, 0, 24);
    sendbuf[offset++] = ptParam->type;
    sendbuf[offset++] = ptParam->interval;
    sendbuf[offset++] = ptParam->power;
    sendbuf[offset++] = ptParam->MAC[0];
    sendbuf[offset++] = ptParam->MAC[1];
    sendbuf[offset++] = ptParam->MAC[2];
    sendbuf[offset++] = ptParam->MAC[3];
    sendbuf[offset++] = ptParam->MAC[4];
    sendbuf[offset++] = ptParam->MAC[5];
    sendbuf[offset++] = ptParam->ch_num;
    for (i = 0; i < MIN(ptParam->ch_num, RADIO_CHANNEL_MAX); i++)
    {
        memcpy(&sendbuf[offset], &ptParam->freq[i], sizeof(uint16));
        offset += 2;
    }
    
    ret = radio_set_operate(fd, CMD_PD, sendbuf, offset, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台检定参数超时, 功率%d, 广播间隔%dms, 类型%d\n", ptParam->power, ptParam->interval, ptParam->type);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台检定参数成功, 功率%d, 广播间隔%dms, 类型%d\n", ptParam->power, ptParam->interval, ptParam->type);
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台检定参数失败, 错误码[%d], 功率%d, 广播间隔%dms, 类型%d\n", status, ptParam->power, ptParam->interval, 
                ptParam->type);
            return status;
        }
    }

    //如果退出检定，则退出虚拟连接
    if (ptParam->type == 0xff)
    {
        /* 申请内存 */
        pProRet = (RADIO_PRODATA_T*)malloc(7 + sizeof(RADIO_PRODATA_T));
        if(NULL == pProRet)
        {
            return RADIO_ERR_OPER_CHECK;
        }
        memset(pProRet, 0, 7 + sizeof(RADIO_PRODATA_T));
        pProRet->datalen = 7;

        radio_dev_report_conn_proc(0, pProRet, pRadioHandle, pRadiodata);

        free(pProRet);
    }

    return 0;
}

/**********************************************************************
* @name      : radio_get_param_proc
* @brief     ：获取电台基本参数
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
* @param[out]：RADIO_CONF_T *ptParam   参数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_get_param_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_CONF_T *ptParam)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint16 temp = 0;
    RADIO_PRODATA_T *pProRet = NULL;

    memset(ptParam, 0, sizeof(RADIO_CONF_T));
    /* 1. 读取电台地址 */
    sendbuf[0] = 1;
    ret = radio_get_operate(fd, CMD_DEV_ADDR, sendbuf, 1, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(pProRet)
    {
        if(7 == pProRet->datalen && 1 == pProRet->data[0])
        {
            MEMCPY_R(ptParam->MAC, pProRet->data + 1, 6);
            RADIO_FMT_DEBUG("电台地址%02X:%02X:%02X:%02X:%02X:%02X\n", ptParam->MAC[0], ptParam->MAC[1],
                          ptParam->MAC[2], ptParam->MAC[3], ptParam->MAC[4], ptParam->MAC[5]);
        }
        free(pProRet);
    }

    /* 2. 电台名称 */
    ret = radio_get_operate(fd, CMD_DEV_NAME, NULL, 0, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(pProRet)
    {
        if(0 != pProRet->datalen)
        {
            ptParam->len = MIN(5, pProRet->datalen);
            memcpy(ptParam->name, pProRet->data, ptParam->len);
            RADIO_FMT_DEBUG("电台名称%s\n", ptParam->name);
        }
        free(pProRet);
    }

    /* 3. 工作参数 */
    ret = radio_get_operate(fd, CMD_RADIO_PARAMS, NULL, 0, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(pProRet)
    {
        if(5 == pProRet->datalen)
        {
            ptParam->power = pProRet->data[0];
            MEMCPY_R(&temp, pProRet->data + 1, 2);
            ptParam->broadcast = temp * 0.625;
            MEMCPY_R(&temp, pProRet->data + 3, 2);
            ptParam->scan = temp * 0.625;
            RADIO_FMT_DEBUG("电台功率%d, 广播间隔%dms, 扫描间隔%dms\n", ptParam->power, ptParam->broadcast, ptParam->scan);
        }
        free(pProRet);
    }

    return 0;
}

/**********************************************************************
* @name      : radio_set_base_match
* @brief     ：设置电台本机配对参数
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_MATCH_T *ptMatch    配对参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-2-17
* @Update    :
**********************************************************************/
int radio_set_base_match(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_MATCH_T *ptMatch)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};
    
    sendbuf[0] = 1;
    sendbuf[1] = ptMatch->mode;
    sendbuf[2] = MIN(16, ptMatch->len);
    memcpy(sendbuf + 3, ptMatch->pwd, MIN(16, ptMatch->len));
    ret = radio_set_operate(fd, CMD_DEV_PAIR_MODE, sendbuf, 3 + MIN(16, ptMatch->len), &pProRet, 
                          pRadioHandle->recv + pRadioHandle->recvlen, RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台(%s)配对参数超时, 模式%d, 密码%s\n", devName[0], ptMatch->mode, ptMatch->pwd);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台(%s)配对参数成功, 模式%d, 密码%s\n", devName[0], ptMatch->mode, ptMatch->pwd);
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台(%s)配对参数失败(%d) 模式%d, 密码%s\n", devName[0], status,
                               ptMatch->mode, ptMatch->pwd);
            return status;
        }
    }
    return RADIO_SUCCESS;
}

/**********************************************************************
* @name      : radio_set_slave_match
* @brief     ：设置电台从机配对参数
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_MATCH_T *ptMatch    配对参数
               uint8 NO                序号(1-3从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_set_slave_match(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_MATCH_T *ptMatch, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};
    
    if(NO > 3 || 0 == NO)
    {
        return -1;
    }
    
    sendbuf[0] = (0x01 << NO);
    sendbuf[1] = ptMatch->mode;
    sendbuf[2] = MIN(16, ptMatch->len);
    memcpy(sendbuf + 3, ptMatch->pwd, MIN(16, ptMatch->len));
    ret = radio_set_operate(fd, CMD_DEV_PAIR_MODE, sendbuf, 3 + MIN(16, ptMatch->len), &pProRet, 
                          pRadioHandle->recv + pRadioHandle->recvlen, RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台(%s)配对参数超时, 模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台(%s)配对参数成功, 模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台(%s)配对参数失败(%d) 模式%d, 密码%s\n", devName[NO], status,
                               ptMatch->mode, ptMatch->pwd);
            return status;
        }
    }
    return RADIO_SUCCESS;
}

/**********************************************************************
* @name      : radio_set_match_proc
* @brief     ：设置电台配对参数
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_MATCH_T *ptMatch    配对参数
               uint8 NO                序号(0-本机, 1-3从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_set_match_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_MATCH_T *ptMatch, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};

    if(NO > 3)
    {
        return -1;
    }
    if(NO > 0)
    {
        RADIO_FMT_DEBUG("电台(%s)配对参数不设置，使用默认方式连接电表\n", devName[NO]);
        return 0;
    }
    
    sendbuf[0] = (0x01 << NO);
    sendbuf[1] = ptMatch->mode;
    sendbuf[2] = MIN(16, ptMatch->len);
    memcpy(sendbuf + 3, ptMatch->pwd, MIN(16, ptMatch->len));
    ret = radio_set_operate(fd, CMD_DEV_PAIR_MODE, sendbuf, 3 + MIN(16, ptMatch->len), &pProRet, 
                          pRadioHandle->recv + pRadioHandle->recvlen, RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台(%s)配对参数超时, 模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台(%s)配对参数成功, 模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台(%s)配对参数失败(%d) 模式%d, 密码%s\n", devName[NO], status,
                               ptMatch->mode, ptMatch->pwd);
            return status;
        }
    }
    return RADIO_SUCCESS;
}

/**********************************************************************
* @name      : radio_get_match_proc
* @brief     ：查询电台配对参数
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               uint8 NO                序号(0-本机, 1-3从设备)
* @param[out]：RADIO_MATCH_T *ptMatch    配对参数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_get_match_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_MATCH_T *ptMatch, uint8 NO)
{
    uint8  sendbuf[24] = {0};
    RADIO_PRODATA_T *pProRet = NULL;
    char          *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};
    int            ret = 0;
    
    if(NO > 3)
    {
        return -1;
    }
    
    sendbuf[0] = (0x01 << NO);
    ret = radio_get_operate(fd, CMD_DEV_PAIR_MODE, sendbuf, 1, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(pProRet)
    {
        if(1 == pProRet->data[0] && pProRet->datalen >= 3)
        {
            ptMatch->mode = pProRet->data[1];
            ptMatch->len = pProRet->data[2];
            if(ptMatch->len > 0)
            {
                memcpy(ptMatch->pwd, pProRet->data +3, ptMatch->len);
            }
            RADIO_FMT_DEBUG("电台(%s)配对参数模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        }
        free(pProRet);
    }
    return 0;
}

/**********************************************************************
* @name      : radio_get_baseinfo_proc
* @brief     ：查询电台基本信息
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
void radio_get_baseinfo_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    uint8           recv[28] = {0};
    RADIO_PRODATA_T  *pProRet = NULL;
    CMD_VERSION_T  *pVersion = NULL;
    uint16          temp = 0;
    uint32          temp32 = 0;
    int             ret = 0;

    //默认sn
    uint8 i =0;
    uint8 defSn[20] =  {0x48, 0xa5, 0xee, 0x8a, 0x71, 0x83, 0xc3, 0xf0, 0x43, 0x4c, 0x75, 0xfd, 0x91, 
                        0x2e, 0x0b, 0x7b, 0xe2, 0xd0, 0x7d, 0xf5};
    
    ret = radio_get_operate(fd, CMD_FIRMWARE_VERSION, NULL, 0, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return;
    }
    pRadioHandle->recvlen += ret;
    if(pProRet)
    {
        if(pProRet->datalen == sizeof(CMD_VERSION_T))
        {
            pVersion = (CMD_VERSION_T*)pProRet->data;
            MEMCPY_R(&temp, pVersion->app_version + 2, 2);
            MEMCPY_R(&temp32, pVersion->git_version, 4);
            RADIO_FMT_DEBUG("电台芯片型号: %02X, APP版本: %d.%d.%d.%d, GIT版本: %08X\n", pVersion->ic_type,
                           pVersion->app_version[0], pVersion->app_version[1], temp, pVersion->app_version[4], temp32);
        }
        free(pProRet);
    }

    ret = radio_get_operate(fd, CMD_FIRMWARE_BUILD_TIME, NULL, 0, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return;
    }
    pRadioHandle->recvlen += ret;
    if(pProRet)
    {
        if(pProRet->datalen == 24)
        {
            memcpy(recv, pProRet->data, 24);
            RADIO_FMT_DEBUG("固件编译时间: %s\n", recv);
        }
        free(pProRet);
    }

    ret = radio_get_operate(fd, CMD_MODULE_SN, NULL, 0, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return;
    }
    pRadioHandle->recvlen += ret;

    if(pProRet)
    {
        RADIO_BUF_DEBUG(pProRet->data, pProRet->datalen, "模组SN,");

        for (i = 0; i < pProRet->datalen; i++)
        {
            if (pProRet->data[i] != 0xff)
            {
                break;;
            }
        }

        if (i != pProRet->datalen)
        {
            pRadiodata->tDevStatus.sn.num = pProRet->datalen;
            memcpy(pRadiodata->tDevStatus.sn.sn, pProRet->data, pProRet->datalen);
        }
        else
        {
            RADIO_BUF_DEBUG(defSn, 20, "无效sn, 使用默认sn");
            pRadiodata->tDevStatus.sn.num = 20;
            memcpy(pRadiodata->tDevStatus.sn.sn, defSn, 20);
        }

        free(pProRet);
    }
}

/**********************************************************************
* @name      : radio_set_link_mode
* @brief     ：设置电台链路数据接收模式
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-9-14
* @Update    :
**********************************************************************/
void radio_set_link_mode(int fd, RADIO_DEV_H_T *pRadioHandle)
{
    uint8           sendbuf[1] = {2};
    RADIO_PRODATA_T  *pProRet = NULL;
    int             ret = 0;
    
    ret = radio_set_operate(fd, CMD_RADIO_TRANS_MODE, sendbuf, 1, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        RADIO_FMT_DEBUG("设置电台链路数据接收模式失败\n");
        return;
    }
    pRadioHandle->recvlen += ret;
    if(pProRet)
    {
        RADIO_FMT_DEBUG("电台链路数据接收模式设置%s\n", (pProRet->datalen == 1 && 0 == pProRet->data[0]) ? "成功" : "失败");
        free(pProRet);
    }
}

/**********************************************************************
* @name      : radio_set_mac_proc
* @brief     ：设置设备地址
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               uint8 *pMAC             地址
               uint8 NO                序号(0-本机, 1-3从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_set_mac_proc(int fd, RADIO_DEV_H_T *pRadioHandle, uint8 *pMAC, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[8] = {0};
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};
    
    if(NO > 3)
    {
        return -1;
    }
    
    sendbuf[0] = (0x01 << NO);
    MEMCPY_R(sendbuf + 1, pMAC, 6);
    ret = radio_set_operate(fd, CMD_DEV_ADDR, sendbuf, 7, &pProRet, 
                          pRadioHandle->recv + pRadioHandle->recvlen, RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台(%s)地址超时 %02X:%02X:%02X:%02X:%02X:%02X\n", devName[NO], pMAC[0], pMAC[1],
                      pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台(%s)地址成功 %02X:%02X:%02X:%02X:%02X:%02X\n", devName[NO], pMAC[0], pMAC[1],
                          pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台(%s)地址失败(%d) %02X:%02X:%02X:%02X:%02X:%02X\n", devName[NO], status,
                           pMAC[0], pMAC[1], pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
            return status;
        }
    }
    return RADIO_SUCCESS;
}

/**********************************************************************
* @name      : radio_set_mac_proc
* @brief     ：设置设备地址
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               uint8 operat            方式(0-断开, 1-连接)
               uint8 *pMAC             地址
               uint8 NO                序号(0-1本机, 2-4从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_conn_disconn_mac(int fd, RADIO_DEV_H_T *pRadioHandle, uint8 operat, uint8 *pMAC, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[8] = {0};
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机1", "本机2", "从设备1", "从设备2", "从设备3"};
    
    if(NO > 4 || operat > 1)
    {
        return -1;
    }
    
    sendbuf[0] = (0x01 << NO);
    sendbuf[1] = operat;
    MEMCPY_R(sendbuf + 2, pMAC, 6);
    ret = radio_set_operate(fd, CMD_LINK_CMD, sendbuf, 8, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("%s电台%s超时 %02X:%02X:%02X:%02X:%02X:%02X\n", 0 == operat ? "断开" : "连接", devName[NO], 
                      pMAC[0], pMAC[1], pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("%s电台%s成功 %02X:%02X:%02X:%02X:%02X:%02X\n", 0 == operat ? "断开" : "连接", devName[NO], 
                          pMAC[0], pMAC[1], pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
        }
        else
        {
            RADIO_FMT_DEBUG("%s电台%s失败(%d) %02X:%02X:%02X:%02X:%02X:%02X\n", 0 == operat ? "断开" : "连接", 
                           devName[NO], status, pMAC[0], pMAC[1], pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
            return status;
        }
    }
    return RADIO_SUCCESS;
}

/**********************************************************************
* @name      : radio_get_conninfo_proc
* @brief     ：查询电台连接信息
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_get_conninfo_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    RADIO_PRODATA_T  *pProRet = NULL;
    int             ret = 0;
    
    ret = radio_get_operate(fd, CMD_RADIO_CONN_INFO, NULL, 0, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(pProRet)
    {
        ret = radio_dev_report_conn_proc(0, pProRet, pRadioHandle, pRadiodata);
        free(pProRet);
    }

    return ret;
}

/**********************************************************************
* @name      : radio_assign_dev_channel
* @brief     ：分配电台从设备id
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DEV_T *pRadioStatus   电台状态
               RADIO_PORT_CONF_T *pConf  端口配置
* @param[out]：
* @return    ：从设备号0 - 无效, 1-3有效
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
uint8 radio_assign_dev_channel(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_DEV_T *pRadioStatus, RADIO_PORT_CONF_T *pConf)
{
    uint8 i = 0;
    uint8 num = 0;
    int   ret = 0;
    struct timespec curtime = {0};
    RADIO_PORT_EVENT_T tEvent;

    /* 重复设置判断 */
    for(i = 0; i < RADIO_SLAVE_MAX; i++)
    {
        if(pRadioStatus->sPortStatus[i].port == pConf->port)
        {
            /* 参数变化 */
            if(0 != memcmp(&pRadioStatus->sPortStatus[i].match, &pConf->match, sizeof(RADIO_MATCH_T)) ||
               0 != memcmp(&pRadioStatus->sPortStatus[i].MAC, &pConf->MAC, 6))
            {
                num = i;
                if(TRUE == pRadioStatus->sPortStatus[i].bConnect)
                {
                    goto disconnect;
                }
                else
                {
                    goto setConfig;
                }
            }
            else
            {
                return i + 1;
            }
        }
    }
    
    /* 从没有连接的从设备中分配 */
    for(i = 0; i < RADIO_SLAVE_MAX; i++)
    {
        if(FALSE == pRadioStatus->sPortStatus[i].bConnect)
        {
            pRadioStatus->sPortStatus[i].port = pConf->port;
            pRadioStatus->sPortStatus[i].mode = pConf->mode;
            pRadioStatus->sPortStatus[i].match = pConf->match;

            /* 配对参数设置 */
            ret = radio_set_match_proc(fd, pRadioHandle, &pConf->match, i + 1);
            if(0 != ret)
            {
                RADIO_FMT_DEBUG("设置电台从设备%d配对参数失败, 端口%d\n", i + 1, pConf->port);
            }

            memcpy(pRadioStatus->sPortStatus[i].MAC, pConf->MAC, 6);
            /* 地址设置 */
            ret = radio_set_mac_proc(fd, pRadioHandle, pConf->MAC, i + 1);
            if(0 != ret)
            {
                RADIO_FMT_DEBUG("设置电台从设备%d地址失败, 端口%d\n", i + 1, pConf->port);
            }

            /* 刷新时间 */
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            pRadioStatus->sPortStatus[i].lasttime = curtime.tv_sec;
            pRadioStatus->sPortStatus[i].lastus = curtime.tv_nsec / 1000;
            return i + 1;
        }
    }

    /* 全部连接则选择时间最远的处理 */
    for(i = 1; i < RADIO_SLAVE_MAX; i++)
    {
        if(pRadioStatus->sPortStatus[i].lasttime < pRadioStatus->sPortStatus[num].lasttime
           || (pRadioStatus->sPortStatus[i].lasttime == pRadioStatus->sPortStatus[num].lasttime && 
               pRadioStatus->sPortStatus[i].lastus < pRadioStatus->sPortStatus[num].lastus))
        {
            num = i;
        }
    }

disconnect:
    /* 断开连接 */
    memset(&tEvent, 0, sizeof(RADIO_PORT_EVENT_T));
    tEvent.port = pRadioStatus->sPortStatus[num].port;
    ret = radio_conn_disconn_mac(fd, pRadioHandle, 0, pRadioStatus->sPortStatus[num].MAC, num + 2);
    if(0 == ret)
    {
        pRadioStatus->sPortStatus[num].bConnect = FALSE;
        memcpy(tEvent.MAC, pRadioStatus->sPortStatus[num].MAC, 6);
        tEvent.mode = pRadioStatus->sPortStatus[num].mode;
        tEvent.bConnect = FALSE;
    }
    return num + 1;

setConfig:
    /* 设置参数 */
    pRadioStatus->sPortStatus[num].port = pConf->port;
    pRadioStatus->sPortStatus[num].mode = pConf->mode;
    pRadioStatus->sPortStatus[num].match = pConf->match;

    /* 配对参数设置 */
    ret = radio_set_match_proc(fd, pRadioHandle, &pConf->match, i + 1);
    if(0 != ret)
    {
        RADIO_FMT_DEBUG("设置电台从设备%d配对参数失败, 端口%d\n", num + 1, pConf->port);
    }

    memcpy(pRadioStatus->sPortStatus[num].MAC, pConf->MAC, 6);
    /* 地址设置 */
    ret = radio_set_mac_proc(fd, pRadioHandle, pConf->MAC, i + 1);
    if(0 != ret)
    {
        RADIO_FMT_DEBUG("设置电台从设备%d地址失败, 端口%d\n", num + 1, pConf->port);
    }

    /* 刷新时间 */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    pRadioStatus->sPortStatus[num].lasttime = curtime.tv_sec;
    pRadioStatus->sPortStatus[num].lastus = curtime.tv_nsec / 1000;
    
    return num + 1;
}

/**********************************************************************
* @name      : radio_out_msg_porc
* @brief     ：电台透传操作
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
               RADIO_MSG_T *pMsg         传送的数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_out_msg_porc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata, RADIO_MSG_T *pMsg)
{
    int     ret = 1;
    uint8   MAC1[6] = {0};
    uint8   MAC2[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint32  port = pMsg->port;
    struct timespec oldtime = {0};
    struct timespec curtime = {0};
    uint8   channid = 0;
    RADIO_DEV_T *pRadio = &(pRadiodata->tDevStatus);
    
    /* 主设备 */
    if(port <= RADIO_PORT_MASTER1)
    {
        if(FALSE == pRadio->mPortStatus[port].bConnect || 0 == memcmp(pRadio->mPortStatus[port].MAC, MAC1, 6) 
           || 0 == memcmp(pRadio->mPortStatus[port].MAC, MAC2, 6) )
        {
            RADIO_FMT_DEBUG("电台主设备%d无连接\n", pMsg->port + 1);
            return RADIO_ERR_UNEXIST_MAC;
        }
        else
        {
            ret = radio_tran_send(fd, pRadio->mPortStatus[port].MAC, pMsg->msg, pMsg->len);
            if(0 != ret)
            {
                /* 发送失败表示串口出问题, 重新打开一下设备 */
                pRadioHandle->status = RADIO_DEV_STATUS_INIT;
                RADIO_FMT_DEBUG("电台主设备%d发送失败, 重新打开设备\n", port + 1);
                return RADIO_ERR_OPER_FAIL;
            }
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            pRadio->mPortStatus[port].lasttime = curtime.tv_sec;
            pRadio->mPortStatus[port].lastus = curtime.tv_nsec / 1000;
            pRadio->PortCfg[port].lasttime = curtime.tv_sec;
            pRadio->PortCfg[port].lastus = curtime.tv_nsec / 1000;
        }
        return RADIO_SUCCESS;
    }

    /* 从设备 */
    
    /* 1. 通道选择 */
    channid = radio_assign_dev_channel(fd, pRadioHandle, pRadio, &(pRadio->PortCfg[port].porttCfg));
    pRadio->PortCfg[port].channelid = channid;
    if(0 == channid || channid > 3)
    {
        pRadio->PortCfg[port].bConnect = FALSE;
        RADIO_FMT_DEBUG("电台发送失败, 从设备分配失败, 端口%d\n", port + 1);
        return RADIO_ERR_CONNECT_FAIL;
    }

    /* 2. 连接设备 */
    if(FALSE == pRadio->sPortStatus[channid - 1].bConnect)
    {
        /* 连接设备 */
        ret = radio_conn_disconn_mac(fd, pRadioHandle, 1, pRadio->sPortStatus[channid - 1].MAC, channid + 1);
        if(0 != ret)
        {
            return RADIO_ERR_CONNECT_FAIL;
        }

        /* 等待 */
        clock_gettime(CLOCK_MONOTONIC, &oldtime);
        while(1)
        {
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            if(TRUE == pRadio->sPortStatus[channid - 1].bConnect)
            {
                break;
            }
            if(curtime.tv_sec > oldtime.tv_sec + 5)
            {
                RADIO_FMT_DEBUG("电台从设备%d连接%02X:%02X:%02X:%02X:%02X:%02X超时\n", channid, 
                              pRadio->sPortStatus[channid - 1].MAC[0], pRadio->sPortStatus[channid - 1].MAC[1], 
                              pRadio->sPortStatus[channid - 1].MAC[2], pRadio->sPortStatus[channid - 1].MAC[3],
                              pRadio->sPortStatus[channid - 1].MAC[4], pRadio->sPortStatus[channid - 1].MAC[5]);
                return RADIO_ERR_CONNECT_FAIL;
            }

            radio_get_conninfo_proc(fd, pRadioHandle, pRadiodata);
            usleep(200000);
        }
    }

    /* 3. 发送 */
    ret = radio_tran_send(fd, pRadio->sPortStatus[channid - 1].MAC, pMsg->msg, pMsg->len);
    if(0 != ret)
    {
        /* 发送失败表示串口出问题, 重新打开一下设备 */
        pRadioHandle->status = RADIO_DEV_STATUS_INIT;
        RADIO_FMT_DEBUG("电台从设备%d发送失败, 重新打开设备\n", channid);
        return RADIO_ERR_OPER_FAIL;
    }
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    pRadio->sPortStatus[channid - 1].lasttime = curtime.tv_sec;
    pRadio->sPortStatus[channid - 1].lastus = curtime.tv_nsec / 1000;
    pRadio->PortCfg[port].lasttime = curtime.tv_sec;
    pRadio->PortCfg[port].lastus = curtime.tv_nsec / 1000;
    return RADIO_SUCCESS;
}

/**********************************************************************
* @name      : radio_set_check_proc
* @brief     ：设置电台检定
* @param[in] ：int fd                   电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_CHECK_T *ptCheck    检定参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-1
* @Update    :
**********************************************************************/
int radio_set_check_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_CHECK_T *ptCheck)
{
    int    ret = 0;
    uint8  sendbuf[32] = {0};
    uint16 offset = 0;
    uint8  i = 0;
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T   *pProRet = NULL;

    sendbuf[offset++] = ptCheck->type;
    sendbuf[offset++] = ptCheck->interval;
    sendbuf[offset++] = ptCheck->power;
    memcpy(sendbuf + offset, ptCheck->MAC, 6);
    offset += 6;
    sendbuf[offset++] = ptCheck->ch_num;
    for(i = 0; i < ptCheck->ch_num; i++)
    {
        memcpy(sendbuf + offset, &(ptCheck->freq[i]), 2);
        offset += 2;
    }
    ret = radio_set_operate(fd, CMD_RADIO_PARAMS, sendbuf, offset, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置电台检定超时, 类型%d, 间隔%dms, 功率%d, 地址%02X:%02X:%02X:%02X:%02X:%02X\n", 
                       ptCheck->type, ptCheck->interval, ptCheck->power, ptCheck->MAC[0], ptCheck->MAC[1],
                       ptCheck->MAC[2], ptCheck->MAC[3], ptCheck->MAC[4], ptCheck->MAC[5]);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置电台检定成功, 类型%d, 间隔%dms, 功率%d, 地址%02X:%02X:%02X:%02X:%02X:%02X\n", 
                       ptCheck->type, ptCheck->interval, ptCheck->power, ptCheck->MAC[0], ptCheck->MAC[1],
                       ptCheck->MAC[2], ptCheck->MAC[3], ptCheck->MAC[4], ptCheck->MAC[5]);
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台检定失败(%d) 类型%d, 间隔%dms, 功率%d, 地址%02X:%02X:%02X:%02X:%02X:%02X\n", status, 
                          ptCheck->type, ptCheck->interval, ptCheck->power, ptCheck->MAC[0], ptCheck->MAC[1],
                       ptCheck->MAC[2], ptCheck->MAC[3], ptCheck->MAC[4], ptCheck->MAC[5]);
            return status;
        }
    }

    return 0;
}

/**********************************************************************
* @name      : radio_update_broadcast_param
* @brief     ：更新广播参数
* @param[in] ：ptMatch       配对参数
* @param[out]：ptBoardcast   参数
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
void radio_update_broadcast_param(RADIO_DATA_T *pRadiodata, RADIO_BROADCAST_T *ptBoardcast)
{
    uint8  outData[512] = {0};
    uint16 outLen = 0;

    if ((pRadiodata == NULL) || (ptBoardcast == NULL))
    {
        return;
    }
    
    pRadiodata->tDevStatus.broadcast.flagLen = 0x02;
    pRadiodata->tDevStatus.broadcast.flagType = 0x01;
    pRadiodata->tDevStatus.broadcast.flagValue = 0x06;
    pRadiodata->tDevStatus.broadcast.factLen = 0x16;
    pRadiodata->tDevStatus.broadcast.factType = 0xff;
    pRadiodata->tDevStatus.broadcast.devCode = 0xC2;
    pRadiodata->tDevStatus.broadcast.factCode[0] = 0xff;
    pRadiodata->tDevStatus.broadcast.factCode[1] = 0xff;


    if ((pRadiodata->tDevStatus.match.mode != RADIO_MATCH_PE) || (pRadiodata->tDevStatus.match.len == 0))
    {
        pRadiodata->tDevStatus.broadcast.pinType[0] = 0xff;
        pRadiodata->tDevStatus.broadcast.pinType[1] = 0xff;
        memset(&pRadiodata->tDevStatus.broadcast.pin[0], 0xff, 16);
    }
    else
    {
        if (ESAM_OK != ResponseFrameDeal(SecMod_Cipher, (uint8 *)pRadiodata->tDevStatus.match.pwd, 
                pRadiodata->tDevStatus.match.len, outData, &outLen))
        {
            pRadiodata->tDevStatus.broadcast.pinType[0] = 0x00;
            pRadiodata->tDevStatus.broadcast.pinType[1] = 0x00;
            pRadiodata->tDevStatus.broadcast.pin[0] = 0x31;
            pRadiodata->tDevStatus.broadcast.pin[1] = 0x32;
            pRadiodata->tDevStatus.broadcast.pin[2] = 0x33;
            pRadiodata->tDevStatus.broadcast.pin[3] = 0x34;
            pRadiodata->tDevStatus.broadcast.pin[4] = 0x35;
            pRadiodata->tDevStatus.broadcast.pin[5] = 0x36;
            memset(&pRadiodata->tDevStatus.broadcast.pin[6], 0xff, 10);

            RADIO_BUF_DEBUG(pRadiodata->tDevStatus.broadcast.pin, 16, "PIN加密失败，使用默认PIN");
        }
        else
        {
            RADIO_BUF_DEBUG(outData, outLen, "PIN加密成功");
            pRadiodata->tDevStatus.broadcast.pinType[0] = 0x01;
            pRadiodata->tDevStatus.broadcast.pinType[1] = 0x00;
            memcpy(pRadiodata->tDevStatus.broadcast.pin, outData, MIN(16, outLen));
        }
    }
    
    pRadiodata->tDevStatus.broadcast.devLen = 0x04;
    pRadiodata->tDevStatus.broadcast.devType = 0x09;
    memcpy(&pRadiodata->tDevStatus.broadcast.devName, pRadiodata->tDevStatus.config.name, MIN(RADIO_NAME_MAX_LEN, 3));

    return;
}

/**********************************************************************
* @name      : radio_set_broadcast_proc
* @brief     ：设置广播参数
* @param[in] ：int fd                   电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_BROADCAST_T *ptMatch配对参数
               uint8 NO                序号(0-本机, 1-3从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_set_broadcast_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[32] = {0};
    uint16 offset = 0;
    uint8  status = RADIO_SUCCESS;
    RADIO_PRODATA_T   *pProRet = NULL;
    RADIO_BROADCAST_T *ptBoardcast = &pRadiodata->tDevStatus.broadcast;

    radio_update_broadcast_param(pRadiodata, ptBoardcast);

    sendbuf[offset++] = ptBoardcast->flagLen;
    sendbuf[offset++] = ptBoardcast->flagType;
    sendbuf[offset++] = ptBoardcast->flagValue;
    sendbuf[offset++] = ptBoardcast->factLen;
    sendbuf[offset++] = ptBoardcast->factType;
    sendbuf[offset++] = ptBoardcast->devCode;
    sendbuf[offset++] = ptBoardcast->factCode[0];
    sendbuf[offset++] = ptBoardcast->factCode[1];
    sendbuf[offset++] = ptBoardcast->pinType[0];
    sendbuf[offset++] = ptBoardcast->pinType[1];
    memcpy(&sendbuf[offset], ptBoardcast->pin, 16);
    offset += 16;
    sendbuf[offset++] = ptBoardcast->devLen;
    sendbuf[offset++] = ptBoardcast->devType;
    memcpy(&sendbuf[offset], ptBoardcast->devName, 3);
    offset += 3;

    ret = radio_set_operate(fd, CMD_ADV_DATA, sendbuf, offset, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                          RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pRadioHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        RADIO_FMT_DEBUG("设置广播参数超时, 广播类型%d, 设备类型%X, 厂商%02X%02X 特征码%02X%02X\n", ptBoardcast->flagType, 
            ptBoardcast->devCode, ptBoardcast->factCode[0], ptBoardcast->factCode[1], ptBoardcast->pinType[0], 
            ptBoardcast->pinType[1]);
        return RADIO_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(RADIO_SUCCESS == status)
        {
            RADIO_FMT_DEBUG("设置广播参数成功, 广播类型%d, 设备类型%X, 厂商%02X%02X 特征码%02X%02X\n", ptBoardcast->flagType, 
                ptBoardcast->devCode, ptBoardcast->factCode[0], ptBoardcast->factCode[1], ptBoardcast->pinType[0], 
                ptBoardcast->pinType[1]);
        }
        else
        {
            RADIO_FMT_DEBUG("设置广播参数失败, 广播类型%d, 设备类型%X, 厂商%02X%02X 特征码%02X%02X\n", ptBoardcast->flagType, 
                ptBoardcast->devCode, ptBoardcast->factCode[0], ptBoardcast->factCode[1], ptBoardcast->pinType[0], 
                ptBoardcast->pinType[1]);
            return status;
        }
    }

    return 0;
}
#endif

#endif

#if DESC("MQTT后续操作", 1)
#if 0
/**********************************************************************
* @name      : radio_setparam_msg_proc
* @brief     ：设置电台基本参数消息操作
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_MSG_T *pMsg     消息
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_setparam_msg_proc(int fd, RADIO_DEV_MSG_T *pMsg, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    int ret = 0;
    RADIO_CONF_T *pConfig = NULL;

    pConfig = (RADIO_CONF_T *)pMsg->data;
    
    if(FALSE == pRadiodata->tDevStatus.bConfig || 
       0 != memcmp(&pRadiodata->tDevStatus.config, pConfig, sizeof(RADIO_CONF_T)))
    {
        #if PIN_ENABLE == 1
        if (pRadiodata->tDevStatus.bMatch)
        {
            /* 如果地址变化，则可能需要重新设置广播参数 */
            if (0 != memcmp(pRadiodata->tDevStatus.config.name, pConfig->name, RADIO_NAME_MAX_LEN))
            {
                /* 设置广播参数 */
                memcpy(pRadiodata->tDevStatus.config.name, pConfig->name, RADIO_NAME_MAX_LEN);
                ret = radio_set_broadcast_proc(fd, pRadioHandle, pRadiodata, 0);
            }
        }
        #endif
        
        ret = radio_set_param_proc(fd, pRadioHandle, pConfig, TRUE);
        if(0 == ret)
        {
            memcpy(&pRadiodata->tDevStatus.config, pConfig, sizeof(RADIO_CONF_T));
            pRadiodata->tDevStatus.bConfig = TRUE;
            /* 保存配置 */
            radio_write_config(pConfig);
        }
    }

    /* 构造返回信息 */
    if(MESSAGE_TYPE_A_XDR == pMsg->type)
    {
        uint8           buf[360] = {0};
        MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;

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

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return radio_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = radio_common_res_package(pMsg->token, ret);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", RADIO_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_SET,
                                                MQTT_TOPIC_INFO_RES, RADIO_TOPOC_SETPARAM);
            ret = radio_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}

/**********************************************************************
* @name      : radio_setport_msg_proc
* @brief     ：设置电台端口消息操作
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_MSG_T *pMsg     消息
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_setport_msg_proc(int fd, RADIO_DEV_MSG_T *pMsg, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    int    ret = 0;
    uint32 i = 0;
    uint32 port = 0;
    RADIO_PORTS_CONF_T *pPort = NULL;
    RADIO_DEV_T        *ptDev = &(pRadiodata->tDevStatus);
    uint8   MAC1[6] = {0};
    uint8   MAC2[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    pPort = (RADIO_PORTS_CONF_T *)pMsg->data;
    for(i = 0; i < pPort->num; i++)
    {
        port = pPort->tConf[i].port;
        if(pPort->tConf[i].port <= RADIO_PORT_MASTER1 && RADIO_MODE_MASTER == pPort->tConf[i].mode)
        {
            /* 主动设置 */
            if(FALSE == ptDev->bMatch || 0 != memcmp(&ptDev->match, &pPort->tConf[i].match, sizeof(RADIO_MATCH_T)))
            {
                ret = radio_set_match_proc(fd, pRadioHandle, &pPort->tConf[i].match, 0);
                if(0 != ret)
                {
                    break;
                }

                #if PIN_ENABLE == 1
                if (ptDev->bMatch)
                {
                    //重新设置参数
                    if (0 != memcmp(&pPort->tConf[i].match, &ptDev->match, sizeof(ptDev->match)))
                    {
                        memcpy(&ptDev->match, &pPort->tConf[i].match, sizeof(RADIO_MATCH_T));
                        ptDev->bMatch = TRUE;
                        
                        /* 设置广播参数 */
                        radio_set_broadcast_proc(fd, pRadioHandle, pRadiodata, 0);
                                                
                        RADIO_FMT_DEBUG("设置配对参数修改后，需要重新设置其他参数\n");
                        radio_set_param_proc(fd, pRadioHandle, &pRadiodata->tDevStatus.config, FALSE);
                    }
                }
                #endif
                
                memcpy(&ptDev->match, &pPort->tConf[i].match, sizeof(RADIO_MATCH_T));
                ptDev->bMatch = TRUE;
                /* 保存配置 */
                radio_write_match(&ptDev->match);

                ptDev->PortCfg[port].bConfig = TRUE;
                ptDev->PortCfg[port].porttCfg = pPort->tConf[i];
                /* 保存配置 */
                radio_write_portcfg(port, &(pPort->tConf[i]));
            }
        }
        else if(pPort->tConf[i].port > RADIO_PORT_MASTER1 && RADIO_MODE_SLAVE == pPort->tConf[i].mode)
        {
            /* 从动配置 端口分配 */
            ptDev->PortCfg[port].channelid = radio_assign_dev_channel(fd, pRadioHandle, ptDev, &pPort->tConf[i]);
            ptDev->PortCfg[port].bConfig = TRUE;
            ptDev->PortCfg[port].porttCfg = pPort->tConf[i];
            /* 保存配置 */
            radio_write_portcfg(port, &(pPort->tConf[i]));

            //刷新桌面电台状态显示
            bel_refresh_status(pRadiodata, TRUE);
        }
    }

    /* 设置完成, 连接一下从设备 */
    for(i = 0; i < RADIO_SLAVE_MAX; i++)
    {
        if(FALSE == ptDev->sPortStatus[i].bConnect && 0 != memcmp(ptDev->sPortStatus[i].MAC, MAC1, 6) &&
           0 != memcmp(ptDev->sPortStatus[i].MAC, MAC2, 6))
        {
            ret = radio_conn_disconn_mac(fd, pRadioHandle, 1, ptDev->sPortStatus[i].MAC, i + 2);
            if(0 != ret)
            {
               break;
            }
        }
    }
    
    /* 构造返回信息 */
    if(MESSAGE_TYPE_A_XDR == pMsg->type)
    {
        uint8           buf[360] = {0};
        MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;

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

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return radio_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = radio_common_res_package(pMsg->token, ret);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", RADIO_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_SET,
                                                MQTT_TOPIC_INFO_RES, RADIO_TOPOC_SETPORT);
            ret = radio_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}

/**********************************************************************
* @name      : radio_checkmeter_msg_proc
* @brief     ：设置电台检定参数
* @param[in] ：int fd                   电台句柄
               RADIO_DEV_MSG_T *pMsg     消息
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_checkmeter_msg_proc(int fd, RADIO_DEV_MSG_T *pMsg, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    int ret = 0;
    RADIO_CHECK_T *pCheck = NULL;

    pCheck = (RADIO_CHECK_T *)pMsg->data;
    
    ret = radio_set_checkmeter_proc(fd, pRadioHandle, pRadiodata, pCheck);

    #if CHECK_METER_ASYNC == 0
    /* 构造返回信息 */
    if(MESSAGE_TYPE_A_XDR == pMsg->type)
    {
        uint8           buf[360] = {0};
        MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;

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

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return radio_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        RADIO_FMT_DEBUG("JSON消息没有电台检定消息 消息类型%d\n", pMsg->type);
        return ret;
    }
    #else
    return ret;
    #endif
}
#endif
/**********************************************************************
* @name      : radio_send_msg_proc
* @brief     ：电台发送消息操作
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_MSG_T *pMsg     消息
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_send_msg_proc(int fd, RADIO_DEV_MSG_T *pMsg, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    int          ret = -1;
    RADIO_MSG_T   *pMsgSend = NULL;
    //RADIO_DEV_T   *ptDev = &(pRadiodata->tDevStatus);

    pMsgSend = (RADIO_MSG_T *)pMsg->data;
#ifdef AREA_SHANGHAI
    //0x33为带RS编码处理
    uint8 czBuf[8] = {0};
    if(TRUE == g_bRadioRS)
    {
        czBuf[0] = 0x33;        //RS编码
        memset(czBuf + 1, 0xAA, 4);
        memset(czBuf + 5, 0x7E, 3);
        RADIO_BUF_DEBUG(pMsgSend->msg, pMsgSend->len, "原始报文");
        pMsgSend->len = rs_code_pack(pMsgSend->msg, pMsgSend->len, 2);
    }
    
#endif
    /* 发送数据 */
    //ret = radio_out_msg_porc(fd, pRadioHandle, pRadiodata, pMsgSend);
#ifdef AREA_SHANGHAI
    ret = radio_uart_send(fd, czBuf, 8);
    RADIO_BUF_DEBUG(czBuf, 8, "radio_send_msg_proc ret=%d\n", ret);
#endif
    ret = radio_uart_send(fd, pMsgSend->msg, pMsgSend->len);
    RADIO_BUF_DEBUG(pMsgSend->msg, pMsgSend->len, "radio_send_msg_proc ret=%d\n", ret);

    /* 构造返回信息 */
    if(MESSAGE_TYPE_A_XDR == pMsg->type)
    {
        uint8           buf[360] = {0};
        MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;

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

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return radio_send_axdr_msg(ptAXDR, 0);
    }
    /*else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = radio_common_res_package(pMsg->token, ret);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", RADIO_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_ACT,
                                                MQTT_TOPIC_INFO_RES, RADIO_TOPOC_SENDDATA);
            ret = radio_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }*/

    return -1;
}
#if 0
/**********************************************************************
* @name      : radio_trans_msg_proc
* @brief     ：电台转发消息操作
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_MSG_T *pMsg     消息
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int radio_trans_msg_proc(int fd, RADIO_DEV_MSG_T *pMsg, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    int               ret = -1;
    RADIO_TRANS_REQ_T  *pMsgSend = (RADIO_TRANS_REQ_T*)pMsg->data;
    RADIO_DEV_T        *ptDev = &(pRadiodata->tDevStatus);
    RADIO_TRANS_ACK_T   tAck;
    RADIO_MSG_T         tMsgOut;
    RADIO_PORT_CONF_T   tempConf;
    struct timespec oldtime = {0};
    struct timespec curtime = {0};
    uint32            spacems = 0;
    RADIO_PRODATA_T *pProData = NULL;
    uint8  buf[20480] = {0};  //20K长度内部接收缓存
    int    retlen = 0;
    uint8  *temp = 0;
    uint16 start = 0;
    uint16 msglen = 0;
    
    /* 暂时替换 */
    tempConf = ptDev->PortCfg[pMsgSend->port].porttCfg;
    ptDev->PortCfg[pMsgSend->port].porttCfg.match = pMsgSend->match;
    memcpy(ptDev->PortCfg[pMsgSend->port].porttCfg.MAC, pMsgSend->MAC, 6);
    
    /* 消息发送出去 */
    memset(&tMsgOut, 0, sizeof(RADIO_MSG_T));
    tMsgOut.port = pMsgSend->port;
    tMsgOut.len = pMsgSend->len;
    memcpy(tMsgOut.msg, pMsgSend->msg, pMsgSend->len);
    ret = radio_out_msg_porc(fd, pRadioHandle, pRadiodata, &tMsgOut);
    
    /* 等待应答 */
    memset(&tAck, 0, sizeof(RADIO_TRANS_ACK_T));
    tAck.ret = ret;
    if(0 == ret)
    {
        tAck.ret = RADIO_ERR_OPER_FAIL;
        clock_gettime(CLOCK_MONOTONIC, &oldtime);
        while(1)
        {
            /* 超时判断 */
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            spacems = (curtime.tv_sec - oldtime.tv_sec) * 1000 + (curtime.tv_nsec - oldtime.tv_nsec) / 10000000;
            if(spacems >= pMsgSend->waittime.msgtime)
            {
                break;
            }
            /* 串口接收 */
            retlen = radio_uart_recv(fd, buf, 20480, 100, 200);
            if(retlen <= 0)
            {
                continue;
            }
            RADIO_BUF_DEBUG(buf, retlen, "串口接收");

            temp = pRadioHandle->recv;
            start = 0;
            /* 判断接收内容 */
            while(start < retlen)
            {
                if(0x68 == buf[start])
                {
                    memcpy(&msglen, buf + start + 1, 2);
                    if(start + RADIO_PROTOCOL_LEN_MIN + msglen <= retlen)
                    {
                        ret = radio_protocol_split(buf + start, RADIO_PROTOCOL_LEN_MIN + msglen, &pProData);
                        if(NULL != pProData)
                        {
                            if(RADIO_PRO_TO_OUT == pProData->type && 0 == pProData->cmd && 
                                0 == memcmp(pMsgSend->MAC, pProData->mac, 6))
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
                if(pRadioHandle->recvlen < RADIO_RECV_BUF_LEN)
                {
                    temp[pRadioHandle->recvlen] = buf[start];
                    pRadioHandle->recvlen += 1;
                }
                start++;
            }

            /* 剩余内容拷贝构造返回消息 */
            if(pProData)
            {
                start += RADIO_PROTOCOL_LEN_MIN + pProData->datalen;
                if(start < retlen && pRadioHandle->recvlen < RADIO_RECV_BUF_LEN)
                {
                    msglen = MIN(RADIO_RECV_BUF_LEN - pRadioHandle->recvlen, retlen - start);
                    memcpy(temp + pRadioHandle->recvlen, buf + start, msglen);
                    pRadioHandle->recvlen += msglen;
                }

                tAck.ret = RADIO_SUCCESS;
                tAck.len = pProData->datalen;
                memcpy(tAck.msg, pProData->data, pProData->datalen);
                free(pProData);
                pProData = NULL;
                break;
            }
        }
    }

    /* 配置替换回去 */
    ptDev->PortCfg[pMsgSend->port].porttCfg = tempConf;
    /* 构造返回信息 */
    if(MESSAGE_TYPE_A_XDR == pMsg->type)
    {
        uint8           buf[3600] = {0};
        MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;

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

        ptAXDR->msglen = radio_trans_ack_package(&tAck, ptAXDR->playload, 3000);
        return radio_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = radio_transmsg_res_package(pMsg->token, &tAck);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", RADIO_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_ACT,
                                                MQTT_TOPIC_INFO_RES, RADIO_TOPOC_FORWARD);
            ret = radio_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}

/**********************************************************************
* @name      : radio_setcheck_msg_proc
* @brief     ：设置电台检定消息操作
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_MSG_T *pMsg     消息
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_setcheck_msg_proc(int fd, RADIO_DEV_MSG_T *pMsg, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    int ret = 0;
    RADIO_CHECK_T *pCheck = NULL;
    
    pCheck = (RADIO_CHECK_T *)pMsg->data;

    ret = radio_set_check_proc(fd, pRadioHandle, pCheck);
    if(0 == ret)
    {
        pRadiodata->tDevStatus.bCheckMeter = pCheck->type != RADIO_TYPE_CLOSE ? TRUE : FALSE;
    }

    /* 构造返回信息 */
    if(MESSAGE_TYPE_A_XDR == pMsg->type)
    {
        uint8           buf[360] = {0};
        MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;

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

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return radio_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = radio_common_res_package(pMsg->token, ret);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", RADIO_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_SET,
                                                MQTT_TOPIC_INFO_RES, RADIO_TOPOC_SETDETECT);
            ret = radio_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}
#endif

#endif
/**********************************************************************
* @name      : radio_check_devfd
* @brief     ：检测串口状态是否异常
* @param[in] ：RADIO_DEV_H_T *pRadioHandle 线程句柄
* @return    ：
**********************************************************************/
void radio_check_devfd(RADIO_DEV_H_T *pRadioHandle)
{
    struct termios options; 
    
    if (pRadioHandle->chkfdcnt > 5*(200/RADIO_RECV_TIME_OUT))
    {
        pRadioHandle->chkfdcnt = 0;

        if (pRadioHandle->devfd >= 0)
        {
            if (tcgetattr(pRadioHandle->devfd, &options)  !=  0) /*读取串口termios结构*/
            {
                RADIO_FMT_DEBUG("电台设备%s异常，需要重新初始化 fd[%d]\n", RADIO_UART_NAME, pRadioHandle->devfd);
                pRadioHandle->status = RADIO_DEV_STATUS_INIT;
            }
        }
    }
    else
    {
        pRadioHandle->chkfdcnt ++;
    }
    
    return;
}

/**********************************************************************
* @name      : radio_dev_try_open
* @brief     ：电台串口打开尝试
* @param[in] ：RADIO_DEV_H_T *pRadioHandle 线程句柄
* @param[out]：
* @return    ：电台句柄
* @Create    : 王津剑
* @Date      ：2021-8-24
* @Update    :
**********************************************************************/
int radio_dev_try_open(RADIO_DEV_H_T *pRadioHandle)
{
    int fd = -1;
    int ret = 0;
    uint8 i = 0;
    char  type[3] = {'N' , 'E', 'O'};
    uint8 bSucc = FALSE;
    uint8  sendbuf[1] = {1};
    RADIO_PRODATA_T *pProRet = NULL;
    
    for(i = 0; i < 3; i++)
    {
        fd = radio_uart_open(RADIO_UART_NAME);
        if(fd < -1)
        {
            RADIO_FMT_DEBUG("打开电台设备%s失败\n", RADIO_UART_NAME);
            return -1;
        }
        
        ret = radio_uart_set(fd, 115200, 8, 1, type[i]);
        if(0 != ret)
        {
            RADIO_FMT_DEBUG("设置电台参数(115200, 8, 1, %c)失败, 错误码%d\n", type[i], ret);
        }
        
        ret = radio_get_operate(fd, CMD_DEV_ADDR, sendbuf, 1, &pProRet, pRadioHandle->recv + pRadioHandle->recvlen, 
                              RADIO_RECV_BUF_LEN - pRadioHandle->recvlen);
        if(ret >= 0)
        {
            pRadioHandle->recvlen += ret;
        }
        
        if(pProRet)
        {
            /* 通过数据收发判断是否参数设置成功 */
            bSucc = TRUE;
            RADIO_FMT_DEBUG("设置电台参数(115200, 8, 1, %c)成功\n", type[i]);
            free(pProRet);
            break;
        }
        else
        {
            RADIO_FMT_DEBUG("设置电台参数(115200, 8, 1, %c)失败\n", type[i]);
        }
        
        radio_uart_close(fd);
        fd = -1;
        sleep(3);
    }

    if(FALSE == bSucc)
    {
        radio_uart_close(fd);
        fd = -1;
        RADIO_FMT_DEBUG("电台设备%s设置串口参数失败\n", RADIO_UART_NAME);
    }

    return fd;
}
#if 0
/**********************************************************************
* @name      : radio_dev_prepare
* @brief     ：电台模块开工准备(获取基本参数，设置基本参数)
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_dev_prepare(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    RADIO_DEV_T        *ptDev = &pRadiodata->tDevStatus;
    int               ret = 0;

    if(ptDev->bMatch)
    {
        /* 设置配对参数 */
        ret = radio_set_match_proc(fd, pRadioHandle, &ptDev->match, 0);
    }
    else
    {
        /* 查询配对参数 */
        radio_get_match_proc(fd, pRadioHandle, &ptDev->match, 0);
    }

    #if PIN_ENABLE == 1
    if (ptDev->bMatch)
    {
        /* 设置广播参数 */
        ret = radio_set_broadcast_proc(fd, pRadioHandle, pRadiodata, 0);
    }
    #endif
    
    if(ptDev->bConfig)
    {
        /* 设置参数 */
        ret = radio_set_param_proc(fd, pRadioHandle, &ptDev->config, TRUE);
    }
    else
    {
        /* 查询参数 */
        radio_get_param_proc(fd, pRadioHandle, &ptDev->config);
    }
    
    /* 获取固件信息 */
    if(0 == ret)
    {
        radio_get_baseinfo_proc(fd, pRadioHandle, pRadiodata);
    }

    /* 设置电台接收模式 */
    radio_set_link_mode(fd, pRadioHandle);
    return ret;
}
#endif
uint32 SetRadioChannel_WDASN(int fd_232, uint8 ch, uint8 ptx, uint8 sq);
extern int RadioGetCfgProc(int fd, RADIO_DEV_MSG_T *pMsg);
extern int RadioGetVersionProc(int fd, RADIO_DEV_MSG_T *pMsg);
extern int radio_status_get(int fd, RADIO_DEV_MSG_T *pMsg);
extern uint32 g_RadioSetCfgFlag;
extern RADIO_CONF_T g_RadioConfig;
extern uint8           g_longSendFlag ;     //电台长发标记
extern RADIO_LONG_SEND_T g_longSendParam;
#define MAX_LONG_SEND_TIME  20  //长发最长时间
void radio_long_send(int fd)
{
    uint8 duration;
    char cmd[100]={0};
    time_t sendtime;
    if(g_longSendParam.sendtime > MAX_LONG_SEND_TIME)
        duration = MAX_LONG_SEND_TIME;
    else
        duration = g_longSendParam.sendtime ;
    sprintf(cmd,"AT+SINGLEFREQUENCYSET=01,%02d\r",duration);
    RADIO_FMT_DEBUG("电台长发发送指令 %s\n",cmd);
    //radio_uart_send(fd,(uint8*)cmd,strlen(cmd));
    sendtime = time(NULL);
    AtCheck(fd, cmd, "AT+OK\r", 1000000);
    
    while(1)
    {
        sleep(1);
        if(time(NULL)- sendtime >=MAX_LONG_SEND_TIME)
            break;

    }
    RADIO_FMT_DEBUG("电台长发结束\n");    
}


void milliseconds_sleep(unsigned long mSec)
{
    
    struct timeval tv;
    tv.tv_sec = mSec / 1000;
    tv.tv_usec = (mSec % 1000) * 1000;
    int             err;
    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    }
    while(err < 0 && errno == EINTR);
    
}

void radio_long_send_msk(int fd)
{
    uint8 duration;
    char czBuf[7];
    memset(czBuf, 0xAA, 4);
    memset(czBuf+4, 0x7E, 3);
    if(g_longSendParam.sendtime > MAX_LONG_SEND_TIME)
        duration = MAX_LONG_SEND_TIME;
    else
        duration = g_longSendParam.sendtime ;
    uint8 sendbuf[10] = {0};
    uint32 ptt_delay;
    uint8 lasttime = 0;
    time_t starttime= time(NULL);
    RADIO_FMT_DEBUG("电台长发开始\n");
    ptt_delay = ReadPttDelay();
    SetPttLow(ptt_delay, &radio_state);

    write(fd, czBuf, sizeof(czBuf));

    sendbuf[0] = 0x68;
    write(fd, sendbuf, 1);
    sendbuf[0] = (g_longSendParam.sendbuf==0x11)?0xff:0x00;
    RADIO_FMT_DEBUG("发送报文 0x%02x\n",sendbuf[0]);
    while(1)
    {
        milliseconds_sleep(3);
        write(fd, sendbuf, 1);
        if(starttime != time(NULL))
        {
            lasttime++;
            starttime = time(NULL);
        }
        if(lasttime >= duration)
        {
            break;
        }
        //发送过程中检查下是否收到关闭长发的指令
        if(g_longSendFlag == 0)
        {
            RADIO_FMT_DEBUG("收到关闭长发指令\n"); 
            break;
        }
    }
    RADIO_FMT_DEBUG("电台长发结束\n");    

    tcdrain(fd);
    SetPttHigh(ptt_delay/3, &radio_state);


}

uint8 pass_sec()
{
    
    static time_t   pre_sec;
    time_t          cur_time;
    
    cur_time = time(NULL);
    
    if(cur_time != pre_sec)
    {
        pre_sec = cur_time;
        return 1;
    }
    
    return 0;
    
}

#define YC_DEV      "/dev/talk"
extern uint8  g_RadioSpeakerCtl;
BOOL bOpenYc = FALSE;
int yc_fd;
uint8 lasttime = 0;
time_t starttime;

/**********************************************************************
* @name      : radio_working_proc
* @brief     ：电台工作状态处理
* @param[in] ：int fd                  电台句柄
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int radio_working_proc(int fd, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    RADIO_DEV_MSG_T *pMsg = NULL;
    int            ret = -1;
    uint8          bOper = FALSE;
    
    unsigned int data = 127;
    uint8 duration = 10;
    
    if(bOpenYc != TRUE)
	{
		/*打开预抄设备文件*/
		if((yc_fd = open(YC_DEV, O_RDONLY)) < 0)
		{
			RADIO_FMT_DEBUG("Error opening %s device\n", YC_DEV);
		}
		else
		{
			bOpenYc = TRUE;
            //RADIO_FMT_DEBUG("打开预抄设备文件success\n");
		}
	}

    if(bOpenYc == TRUE)
	{
		if(read(yc_fd, &data, sizeof data) == sizeof(data))
		{
			//RADIO_FMT_DEBUG("yc_fd data is %d\n", data);
			if(data == 0)
			{
			    if(starttime != time(NULL))
                {
                    lasttime++;
                    starttime = time(NULL);
                }         
			        
                if(lasttime >= duration)
                {
                    RADIO_FMT_DEBUG("主板按键长按10s打开通话\n");
                    lasttime = 0;
                    g_RadioSpeakerCtl = 2;
                }
			}
			else
			{
				lasttime = 0;
			}
		}
	}
    if(g_longSendFlag ==1)
    {
        RADIO_FMT_DEBUG("开始电台长发功能\n");
        //radio_long_send(fd);
        radio_long_send_msk(fd);
        g_longSendFlag = 0;

    }

    if (g_RadioSetCfgFlag == 1)
    {
        g_RadioSetCfgFlag = 0;
#ifdef AREA_DEFAULT
        if (SetRadioChannel_command(fd, g_RadioConfig.channel, g_RadioConfig.power, g_RadioConfig.sensitivity) != 0)
#else
        if (SetRadioChannel_WDASN(fd, g_RadioConfig.channel, g_RadioConfig.power, g_RadioConfig.sensitivity) != 0)
#endif
        {
            RADIO_FMT_DEBUG("radio_working_proc SetRadioChannel_WDASN ret fail.\n");
        }
        else
        {
            RADIO_FMT_DEBUG("radio_working_proc SetRadioChannel_WDASN ret succ.\n");
        }
    }

    pMsg = radio_read_queue(&(pRadioHandle->cfgQueue));

    if(pMsg)
    {
        RADIO_FMT_DEBUG("radio_working_proc pMsg->proc=0x%04X.\n", pMsg->proc);
        switch(pMsg->proc)
        {
            case RADIO_IOP_GET_CONFIG:
            {
                ret = RadioGetCfgProc(fd, pMsg);
                break;
            }
            case RADIO_IOP_GET_STATUS:
            {
                ret = radio_status_get(fd, pMsg);
                break;
            }
            case RADIO_IOP_GET_VERSION:
            {
                RadioGetVersionProc(fd, pMsg);
                break;
            }
            default: break;
        }
        bOper = TRUE;
        free(pMsg);
    }
    
    pMsg = radio_read_queue(&(pRadioHandle->msgQueue));
    if(pMsg)
    {
        RADIO_FMT_DEBUG("radio_working_proc pMsg->proc=0x%04X\n", pMsg->proc);
        if(RADIO_IOP_SEND_DATA == pMsg->proc)
        {
            RADIO_FMT_DEBUG("radio_working_proc RADIO_IOP_SEND_DATA\n");
            /* 发送数据 */
            ret = radio_send_msg_proc(fd, pMsg, pRadioHandle, pRadiodata);
        }
        bOper = TRUE;
        free(pMsg);
    }

    if(FALSE == bOper)
    {
        /* 接收数据 */
        ret = radio_uart_recv(fd, pRadioHandle->recv + pRadioHandle->recvlen, RADIO_RECV_BUF_LEN - pRadioHandle->recvlen, 
                            10, 50);
        if(ret > 0)
        {
            RADIO_BUF_DEBUG(pRadioHandle->recv + pRadioHandle->recvlen, ret, "串口接收");
            pRadioHandle->recvlen += ret;
            RADIO_FMT_DEBUG("当前recvlen %d \n",pRadioHandle->recvlen);
        }
    }
    if(pass_sec())
    {
        radio_speak_manager(fd);
    }

    return ret;
}
#if 0
/**********************************************************************
* @name      : radio_dev_recvmsg_proc
* @brief     ：电台模块接收消息处理
* @param[in] ：int fd                  电台句柄
               RADIO_PRODATA_T *pProData 消息解析
               RADIO_DEV_H_T *pRadioHandle 线程句柄
               RADIO_DATA_T *pRadiodata    电台数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int radio_dev_recvmsg_proc(int fd, RADIO_PRODATA_T *pProData, RADIO_DEV_H_T *pRadioHandle, RADIO_DATA_T *pRadiodata)
{
    if(fd < 0 || NULL == pProData || NULL == pRadioHandle || NULL == pRadiodata)
    {
        return -1;
    }

    if(RADIO_PRO_TO_OUT == pProData->type && 0 == pProData->cmd)
    {
        /* 收到电台芯片透传 */
        return radio_dev_msgreport_proc(fd, pProData, pRadioHandle, pRadiodata);
    }
    else if(RADIO_PRO_REPORT == pProData->type && 0 == pProData->bAck && 0 == pProData->bStatus)
    {
        /* 主动上报 */
        if(CMD_MODULE_INFO == pProData->cmd)
        {
            return radio_dev_report_info_proc(fd, pProData, pRadioHandle, pRadiodata);
        }
        else if(CMD_RADIO_CONN_INFO == pProData->cmd)
        {
            return radio_dev_report_conn_proc(fd, pProData, pRadioHandle, pRadiodata);
        }
    }
    RADIO_FMT_DEBUG("不支持或超时的消息: type %d, Ack %d, Status %d, cmd 0x%08X, MAC %02X:%02X:%02X:%02X:%02X:%02X\n", 
                   pProData->type, pProData->bAck, pProData->bStatus, pProData->cmd, pProData->mac[0],
                   pProData->mac[1], pProData->mac[2], pProData->mac[3], pProData->mac[4], pProData->mac[5]);
    return 0;
}
#endif
/**********************************************************************
* @name      : radio_dev_init
* @brief     ：mqtt消息处理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_dev_init(void *pPublic, void **ppPrivate)
{
    RADIO_DATA_T *pRadiodata = (RADIO_DATA_T *)pPublic;

    /* data文件加载 todo */


    /* 检查电台设备是否存在 */
    while(1)
    {
        if(0 == access(RADIO_UART_NAME, F_OK))
        {
            break;
        }
        sleep(1);
    }
#if 0
    /* 连接数据初始化 */
    pRadiodata->tDevStatus.mPortStatus[0].bChannel = 1;
    pRadiodata->tDevStatus.mPortStatus[0].bConnect = FALSE;
    memset(pRadiodata->tDevStatus.mPortStatus[0].MAC, 0xFF, 6);
    pRadiodata->tDevStatus.mPortStatus[0].port = 0;
    
    pRadiodata->tDevStatus.mPortStatus[1].bChannel = 2;
    pRadiodata->tDevStatus.mPortStatus[1].bConnect = FALSE;
    memset(pRadiodata->tDevStatus.mPortStatus[1].MAC, 0xFF, 6);
    pRadiodata->tDevStatus.mPortStatus[1].port = 1;

    pRadiodata->tDevStatus.sPortStatus[0].bChannel = 1;
    pRadiodata->tDevStatus.sPortStatus[0].bConnect = FALSE;
    memset(pRadiodata->tDevStatus.sPortStatus[0].MAC, 0xFF, 6);
    pRadiodata->tDevStatus.sPortStatus[0].port = 0xFF;
    
    pRadiodata->tDevStatus.sPortStatus[1].bChannel = 2;
    pRadiodata->tDevStatus.sPortStatus[1].bConnect = FALSE;
    memset(pRadiodata->tDevStatus.sPortStatus[1].MAC, 0xFF, 6);
    pRadiodata->tDevStatus.sPortStatus[1].port = 0xFF;

    pRadiodata->tDevStatus.sPortStatus[2].bChannel = 3;
    pRadiodata->tDevStatus.sPortStatus[2].bConnect = FALSE;
    memset(pRadiodata->tDevStatus.sPortStatus[2].MAC, 0xFF, 6);
    pRadiodata->tDevStatus.sPortStatus[2].port = 0xFF;

    if(FALSE == pRadiodata->tDevStatus.PortCfg[0].bConfig)
    {
        pRadiodata->tDevStatus.PortCfg[0].channelid = 0;
        pRadiodata->tDevStatus.PortCfg[0].bConnect = FALSE;
        memset(pRadiodata->tDevStatus.PortCfg[0].porttCfg.MAC, 0xFF, 6);
        pRadiodata->tDevStatus.PortCfg[0].porttCfg.mode = RADIO_MODE_MASTER;
        pRadiodata->tDevStatus.PortCfg[0].porttCfg.port = 0;
    }
    if(FALSE == pRadiodata->tDevStatus.PortCfg[1].bConfig)
    {
        pRadiodata->tDevStatus.PortCfg[1].channelid = 0;
        pRadiodata->tDevStatus.PortCfg[1].bConnect = FALSE;
        memset(pRadiodata->tDevStatus.PortCfg[1].porttCfg.MAC, 0xFF, 6);
        pRadiodata->tDevStatus.PortCfg[1].porttCfg.mode = RADIO_MODE_MASTER;
        pRadiodata->tDevStatus.PortCfg[1].porttCfg.port = 1;
    }
#endif
    radio_clean_queue(&pRadiodata->tDevPorc.cfgQueue);
    radio_clean_queue(&pRadiodata->tDevPorc.transQueue);
    radio_clean_queue(&pRadiodata->tDevPorc.msgQueue);
    pRadiodata->tDevPorc.devfd = -1;
    *ppPrivate = &pRadiodata->tDevPorc;
    RADIO_FMT_DEBUG("电台设备%s存在, 开始工作\n", RADIO_UART_NAME);
    return 0;
}

#define AT_ANSWER_LENGTH        2048
#define AT_SEND_CNT             3   /*发送AT命令次数*/

extern RADIO_CONF_T g_RadioConfig;
extern uint32 g_RadioConfigChangeFlag;
extern uint32 g_RadioSpeakerFlag;

int8 AtCheck(const int fd, char *send_buf, char *check_buf, int u_time)
{
    int8 cnt;      /*发送AT命令的次数*/
    int32 recv_cnt;    /*读at命令返回值次数*/ 
    int32 recv_len;    /*AT命令返回字符串长度*/
    char recv_buf[AT_ANSWER_LENGTH];    /*存放AT命令返回字符串的缓冲区*/
    //#if COMM_DEBUG_PRO == 1 
    //char message_display_buf[AT_ANSWER_LENGTH];
    //memset(message_display_buf, 0, AT_ANSWER_LENGTH);   /*清接收缓冲区*/
    //WriteRamXData(DEBUG_PRO_ATSEND, message_display_buf);
    //WriteRamXData(DEBUG_PRO_ATRECV, message_display_buf);
    //#endif

    cnt = 0;

    while (cnt < AT_SEND_CNT)   /*若不成功，AT命令重发AT_CNT次*/
    {
        recv_cnt = 0;
        recv_len = 0;

        //commfun_printcurtime(FALSE, getenv("DEBUG_COMM") != NULL);
        RADIO_FMT_DEBUG("AtCheck Send: %s\n",send_buf);
        memset(recv_buf, 0, sizeof(recv_buf));  /*清接收缓冲区*/
        write(fd, send_buf, strlen(send_buf));  /*发送AT命令*/

        //#if COMM_DEBUG_PRO == 1
        //memset(message_display_buf, 0, AT_ANSWER_LENGTH);   /*清接收缓冲区*/
        //strip_x_char(message_display_buf, send_buf, strlen(send_buf));  
        //WriteRamXData(DEBUG_PRO_ATSEND, message_display_buf);
        //#endif

        while((recv_len <= 0)&&(recv_cnt<3))        /*at命令发送出去后，每隔100ms读一次返回值，共3次*/
        {
            usleep(u_time); /*延时u_time us*/         
            recv_len = read(fd, recv_buf, sizeof(recv_buf));    /*读AT命令返回字符串*/
            recv_cnt++;
        }

        if (recv_len > 0)
        {
            //#if COMM_DEBUG_PRO == 1
            //bzero(message_display_buf, AT_ANSWER_LENGTH);
            //strip_x_char_pro(message_display_buf, recv_buf, recv_len, send_buf, strlen(send_buf));
            //WriteRamXData(DEBUG_PRO_ATRECV, message_display_buf);
            //#endif
            /*验证AT命令应答是否正确*/
            RADIO_BUF_DEBUG(recv_buf,recv_len,"recv: \n");
            RADIO_FMT_DEBUG("AtCheck Recv: %s\n",recv_buf);
            if(strstr((char *)recv_buf, check_buf) != NULL)
            {
                //commfun_printcurtime(FALSE, getenv("DEBUG_COMM") != NULL);
                //RADIO_FMT_DEBUG("AtCheck Recv: %s\n",recv_buf);
                return 1;
            }   
        }
        cnt++;
        sleep(1);   /*若AT命令不回，延时1s重发*/
    }

    return -1;
}

/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述: 获取AT命令返回值
    输入参数：
        fd              串口操作文件句柄
        send_buf    要发送的AT命令
        get_buf     存放AT应答字符串
        get_buf_len 缓冲区长度
    输出参数：
        get_buf     AT命令应答字符串
    函数返回值：
        1       AT命令成功
        -1  AT命令不成功
    内部处理流程：串口先发送AT命令，等待100ms后，读取AT命令返回字符串
                                将字符串放入get_buf
********************************************************************/
int AtGet(const int fd, char *send_buf, uint8 *get_buf, const int32 get_buf_len)
{
    int8 cnt;  /*AT命令发送次数*/
    uint8 recv_buf[AT_ANSWER_LENGTH];   /*AT命令接收缓冲区*/
    int32 recv_len;    /*AT命令接收长度*/
    int32 data_len;
    int8 recv_cnt;     /*读取at命令返回值次数*/
    uint16 i;
    cnt = 0;

    while (cnt < AT_SEND_CNT)           /*不成功重发AT_CNT次*/
    {
        data_len = 0;
        recv_len = 0;
        recv_cnt = 0;

        //commfun_printcurtime(FALSE, getenv("DEBUG_COMM") != NULL);
        RADIO_FMT_DEBUG("AtGet Send: %s\n",send_buf);
        memset(recv_buf, 0, sizeof(recv_buf));  /*请接收缓冲区*/
        write(fd, send_buf, strlen(send_buf));  /*发送AT命令*/

        //#if COMM_DEBUG_PRO == 1
        //memset(message_display_buf, 0, AT_ANSWER_LENGTH);
        //strip_x_char(message_display_buf, send_buf, strlen(send_buf));  
        //WriteRamXData(DEBUG_PRO_ATSEND, message_display_buf);
        //#endif

        /*发送AT命令,等100ms后读串口返回内容;若无返回值,每隔100ms读一次,读三次*/
        while ((data_len <= 0)&&(recv_cnt < 5))
        {
            usleep(150000);
            recv_len = read(fd, recv_buf, sizeof(recv_buf));

            /*连续读完所有返回数据*/
            while((recv_len > 0)&&(data_len < AT_ANSWER_LENGTH))
            {
                data_len += recv_len;
                usleep(150000);     
                recv_len = read(fd, recv_buf+data_len, AT_ANSWER_LENGTH - data_len);    /*读AT命令返回字符串*/
            }
            recv_cnt++;
        }

        if (data_len > 0)
        {
            RADIO_BUF_DEBUG(recv_buf,data_len,"recv: \n");
            uint8 flag =0;
            //过滤下 可能前面有乱报文 判断AT
            for(i=0;i<data_len;i++)
            {
                if(recv_buf[i]=='A' && recv_buf[i+1]=='T')
                {
                    flag = 1;
                    break;
                }
            }
            if(flag == 0)
            {
                RADIO_FMT_DEBUG("未识别到AT\n");
                cnt++;
                break;
            }
            /*将应答字符串放入get_buf指向的缓冲区中*/
            if (data_len-i < get_buf_len)
                memcpy(get_buf, recv_buf+i, data_len-i);
            else
                memcpy(get_buf, recv_buf+i, get_buf_len);


            //#if COMM_DEBUG_PRO == 1
            //memset(message_display_buf, 0, AT_ANSWER_LENGTH);   /*清接收缓冲区*/
            //strip_x_char_pro(message_display_buf, recv_buf, data_len, send_buf, strlen(send_buf));
            //WriteRamXData(DEBUG_PRO_ATRECV, message_display_buf);
            //#endif
            //commfun_printcurtime(FALSE, getenv("DEBUG_COMM") != NULL);
            RADIO_FMT_DEBUG("AtGet Recv: %s\n", get_buf);
            return 1;
        }
        cnt++;
        sleep(1);
    }
    return -1;
}

#define POWER_ON    0xFF
#define POWER_OFF   0xFD
#define OUTPUT                  3
int WriteDTComm(uint8 sbus_data)
{
    int retv = 0;
    int sbus_handle = 0;
    uint8 sbus_flag = 0xff;

    //return 1;

    //if(ReadRamXData(SBUS_FLAG, &sbus_flag)<0)
    //    sbus_flag = 0xff;
    //读取总线标志，将最高位取出来，防止干扰外接语音的播放
    sbus_flag = (sbus_flag & 0x80) + (sbus_data & 0x7f);
    //WriteRamXData(SBUS_FLAG, &sbus_flag);

    if((sbus_handle = open("/dev/sbus", O_RDWR)) < 0)
    {
        RADIO_FMT_DEBUG("open radio error\n");
        return -1;
    }
    ioctl(sbus_handle, OUTPUT, 0x5);

    retv = write(sbus_handle, &sbus_flag, 1);
    if (retv != 1) 
    {
        if (sbus_handle > 0) 
            close(sbus_handle);
        RADIO_FMT_DEBUG("write radio error\n");
        return -1;
    }
    if (sbus_handle > 0) 
        close(sbus_handle);

    return 1;
}

/*复位通讯模块*/
int restart_model_radio()
{
    int bRet = 0;
    bRet = WriteDTComm(POWER_OFF);
    RADIO_FMT_DEBUG("POWER_OFF   %02X    bRet = %d\n",POWER_OFF,bRet);
    sleep(10);
    return bRet;
}
/*启动通讯模块*/
int start_model_radio()
{
    int bRet = 0;
    //uint8 dbchl_wdasn = 0;        // 电台类型的标识
    bRet = WriteDTComm(POWER_ON);
    RADIO_FMT_DEBUG("POWER_ON    %02X    bRet = %d\n",POWER_ON,bRet);
    //ReadXPara(TMN_RUN_PARA, DOUBLE_CHANNEL_WDASN_RADIO_ID, (char *)&dbchl_wdasn, TMN_ID);
    //if(dbchl_wdasn == 2)//软猫电台需要等待1分钟进行dsp加载等操作才能收发命令
    //  sleep(60);
    //else
    //  sleep(10);
    sleep(30);
    return bRet;
}

/*******************************************************
函数功能:串口发送函数
函数输入值:sBuf发送buf，sLen发送长度
fd_232串口句柄
函数返回值:1表示成功，0失败
********************************************************/
int Code_Send_Model(uint8 *sBuf,int sLen,int fd_232)
{
    int SendLen=0;

    /*调用read()，进行串口数据接收*/
    SendLen = write(fd_232, sBuf, sLen);    
    if(SendLen>0)
    {   
        //MPRINT("发送报文长度%d\n", sLen);
        //Mputout(sBuf, sLen);
        return 1;
    }

    return 0;
}

/*******************************************************
函数功能:串口发送函数
函数输入值:rBuf接受buf，fd_232串口句柄
函数返回值:返回发送长度，0失败
********************************************************/
int Code_Recv_Model(uint8 *rBuf,int fd_232)
{
    int RcvLen=0;

    /*调用read()，进行串口数据接收*/
    RcvLen = read(fd_232, rBuf, 100);   
    if(RcvLen>0)
    {
        //MPRINT("接受报文长度%d\n", RcvLen);
        //Mputout(rBuf, RcvLen);
        return RcvLen;
    }

    return 0;
}

/**********************************************************
函数功能:查询modem版本号
函数输入值:fd_232串口句柄
函数返回值:0 失败，1成功
*************************************************************/
int  Get_Version(int fd_232,int flag)
{
    //应用层发送AB 05 05 AB FD 5D 16
    //modem回码AB 09 09 AB FD X X X X x 16
    //X X X X是版本号，ascii码，36 2E 31 34就是6.14

    int Rcvnum=0;
    int i=0,RLen=0;
    int sendlen = 0;
    uint8 verbuf[20];
    uint8 Rbuf[128];
    //uint8 Progbuf=0;

    memset(verbuf,0,sizeof(verbuf));
    memset(Rbuf,0,sizeof(Rbuf));

    uint8 *findcode=NULL;

    //查询modem版本
    sendlen = 0;
    verbuf[sendlen++] = 0xFF;verbuf[sendlen++] = 0xFF;verbuf[sendlen++] = 0xFF;verbuf[sendlen++] = 0xFF;
    verbuf[sendlen++] = 0xAB;verbuf[sendlen++] = 0x05;verbuf[sendlen++] = 0x05;verbuf[sendlen++] = 0xAB;
    verbuf[sendlen++] = 0xFD;verbuf[sendlen++] = 0x5D;verbuf[sendlen++] = 0x16;

    while(Rcvnum++ < 10)
    {
        Code_Send_Model(verbuf,sendlen,fd_232);
        usleep(500000);

        RLen=Code_Recv_Model(Rbuf,fd_232);

        if(RLen >= 11)
        {
            i=0;
            findcode = &Rbuf[0];

            while(i <= (RLen-10))
            {
                if(*findcode == 0xAB && *(findcode+1) == 0x09 && *(findcode+2) == 0x09 &&*(findcode+3) == 0xAB
                    && *(findcode+4) == 0xFD && *(findcode+10) == 0x16)
                {
                    //开始升级的时候将升级进度清0
                    if(flag == 0)
                    {
                        //Progbuf = 0xFF; //代表升级重新开始
                    }
                    else
                    {
                        //Progbuf = 0xFE; //代表升级成功
                    }
                    //WriteRamXData(Modem_Prog,&Progbuf);
                    //WriteModemVer(findcode+5,flag);
                    //WriteTmnXData (RT_DATA,Modem_Run_Ver, findcode+5);
                    //writelog(MODEM_UPDATE_LOG, "modem版本%c%c%c%c",*(findcode+5),*(findcode+6),*(findcode+7),*(findcode+8));
                    RADIO_FMT_DEBUG("modem版本%c%c%c%c\n",*(findcode+5),*(findcode+6),*(findcode+7),*(findcode+8));
                    return 1;
                }

                findcode++;
                i++;
            }
        }
    }
    RADIO_FMT_DEBUG("获取modem版本失败\n");
    return 0;
}

#define SPEAK_EN                0xfe
#define SPEAK_CLOSE             0xff
int OpenSpeak(uint8 *pRadioState)
{
    int flag;
    uint8 state;
    state = (*pRadioState)&SPEAK_EN;
    *pRadioState = state;
    flag = WriteDTComm(*pRadioState);
    return flag;
}

int CloseSpeak(uint8 *pRadioState)
{
    int flag;
    uint8 state;
    state = (*pRadioState)|(~SPEAK_EN);
    *pRadioState = state;
    flag = WriteDTComm((*pRadioState));
    return flag;
}

float g_pczPower[5] = {1, 2.5, 5, 7.5, 10};
char *cz_pczPower[] = {"01.00", "02.50", "05.00", "07.50", "10.00"};
int g_pczYareGrade[] = {-12, -9, -6, -3, 0, 3, 6, 9, 12};

int CheckCrc_WDASN(uint8 *buf, uint32 len)
{
    uint8 crc = 0;
    int i;

    for(i=0; i<len-1; i++)
    {
        crc += buf[i];
    }

    if (crc == buf[len -1])
        return 0;
    else
        return -1;
}

#if 0
/*
设置指令    aa  02  CH  TX  SQ  00  00  00  00  00  SH  SL
设置应答指令  aa  ee  ST  00  00  00  00  00  00  00  SH  SL
查询指令    aa  12  00  00  00  00  00  00  00  00  SH  SL
查询应答指令  aa  ee  ST  CH  00  00  00  00  00  00  SH  SL
*/
void RadioChannelManage_WDASN(int fd_232)
{
    uint8 radio_cch_set[5];

    if (ReadRamXData(Radio_Channel_Set, (char *)radio_cch_set) != 5)
        return;

    if ((radio_cch_set[0] == 0x00)&&(CheckCrc_WDASN(radio_cch_set, 5) == 0))
    {
        RADIO_FMT_DEBUG("need to READ cch\n");
        putout(radio_cch_set, 5);
        ReadRadioChannel_WDASN(fd_232);
    }
    else if ((radio_cch_set[0] == 0x01)&&(CheckCrc_WDASN(radio_cch_set, 5) == 0))
    {
        RADIO_FMT_DEBUG("need to SET cch\n");
        putout(radio_cch_set, 5);       
        SetRadioChannel_WDASN(fd_232, radio_cch_set[1], radio_cch_set[2], radio_cch_set[3]);
    }
}
#endif
uint32 SetRadioChannel_WDASN(int fd_232, uint8 ch, uint8 ptx, uint8 sq)
{
    /*
    设置和查询电台参数。
    AT+RADIO&P=01,05.00,01\r    
    参数分别为频道(1)、功率（5W）、静噪级别(1)
    */
    char  snd_buf[100];
    //char  rcv_buf[100];
    int8 at_status;
    //uint8 answer_cch[5];
    //int i;

    memset(snd_buf, 0x0, sizeof(snd_buf));

    //#if HARDWARE_TYPE == 1
    //sprintf(snd_buf, "AT+RADIO&P=%02d,%s,%02d\r", ch + 1, cz_pczPower[ptx - 1], g_pczYareGrade[sq - 1]);
    sprintf(snd_buf, "AT+RADIO&P=%02d,%s,%02d\r", ch + 1, cz_pczPower[ptx], sq + 1);
    RADIO_FMT_DEBUG("SetRadioChannel_WDASN snd_buf=%s\n", snd_buf);
    //#else
    //sprintf(snd_buf, "AT+CHANNEL=%02d\r", ch);
    //#endif
    //因为猫给电台设置参数的时候，终端也能收到回码
    //所以不能直接用判断回码是否为"OK"来判断
    //需要直接判断字符中是否出现"OK"
    at_status = AtCheck(fd_232, snd_buf, "AT+OK\r", 1000000);

    if (at_status < 0)
    {
        return 1;
    }

    return 0;
}

uint8 pass_sec_wdasn()
{
    static time_t pre_sec;
    time_t cur_time;

    cur_time = time(NULL);

    if (cur_time != pre_sec)
    {
        pre_sec = cur_time;
        return 1;
    }
    return 0;
}

typedef struct tagCOMM_PARA_T
{
    uint8 sDeviceName[30];                    /*设备名，如"/dev/ttyS0" */
    int dwBaudRate;                            /*波特率*/
    int8 byByteSize;                            /*数据位, 4-8 */
    int8 byParity;                                /*校验 'n'  'e'  'o'  's' = None,Odd,Even,Space*/
    int8 byStopBits;                            /*停止位 0,1,2 = 1, 1.5, 2*/
    int fd_232;
}COMM_PARA_T;

//#define RADIO_TTYS 1
//#define RADIO_TTYS_STR "1"

COMM_PARA_T g_RadioCommPara =
{
    RADIO_UART_NAME,
    4800,
    8,
    'N',
    1,
    -1
};

/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：设置串口参数，包括数据位、停止位、奇偶校验
    输入参数：
        fd              打开串口的文件句柄
        databits    串口数据位
        stopbits    停止位
        parity      奇偶校验        
        set_crtscts 硬件流控 1-设置 0-不设置
    输出参数：
        无
    函数返回值：
        TRUE            成功
        FALSE           失败
    内部处理流程：按输入参数进行串口参数设置
********************************************************************/
int set_Parity(const int fd, const int8 databits, const int8 stopbits, const int8 parity, const int8 set_crtscts)
{ 
    struct termios options; 
    if  ( tcgetattr( fd,&options)  !=  0) /*读取串口termios结构*/
    { 
        //#if WRITE_COMMU_LOG == 1 
        //  #if NEW_WRITE_LOG == 1
        //      write_log2("Error set_Parity: can not read termios options!",sizeof("Error set_Parity: can not read termios options!"),NULL,0);
        //  #else
        //      write_log("Error set_Parity: can not read termios options!");
        //  #endif
        //#endif
        return(FALSE);  
    }
    
    /*设置为不加工模式, 不回显*/
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_iflag = IGNBRK;
    options.c_cflag |= CLOCAL | CREAD;
    
    options.c_cflag &= ~CSIZE;  /*清除原字符大小*/
    switch (databits) /*设置数据位数*/
    {   
        case 7:             
            options.c_cflag |= CS7;  /*设置7位数据位数*/
            break;
        case 8:     
            options.c_cflag |= CS8;  /*设置8位数据位数*/
            break;   
        default:    
            //#if WRITE_COMMU_LOG == 1 
            //  #if NEW_WRITE_LOG == 1
            //      write_log2("Error set_Parity: unsupported databits!",sizeof("Error set_Parity: unsupported databits!"),NULL,0);
            //  #else
            //      write_log("Error set_Parity: unsupported databits!");
            //  #endif
            //#endif
            return (FALSE);  
    }
    
    switch (parity) 
    {   
        case 'n':   /*无校验*/
        case 'N': 
            options.c_cflag &= ~PARENB;   /* 输入奇偶校验无效 */
            break;
              
        case 'o': /*奇校验*/
        case 'O':                                                               
            options.c_cflag |= (PARODD | PARENB);   /*设置为奇效验*/  
            //options.c_iflag |= INPCK;     /*对输入进行奇偶检测*/
            break;
              
        case 'e': /*偶校验*/
        case 'E':  
            options.c_cflag &= ~PARODD;
            options.c_cflag |= PARENB;  /* 使奇偶校验有效*/ 
            //options.c_iflag |= INPCK;     /*对输入进行奇偶检测*/
            break;
            
        case 'S': /*space校验*/
        case 's': 
            options.c_cflag &= ~CSTOPB;
            break;
            
        default:   
            //#if WRITE_COMMU_LOG == 1 
            //  #if NEW_WRITE_LOG == 1
            //      write_log2("Error set_Parity: unsupported parity!",sizeof("Error set_Parity: unsupported parity!"),NULL,0);
            //  #else
            //      write_log("Error set_Parity: unsupported parity!");
            //  #endif
            //#endif            
            return (FALSE);  
    } 
     
/* 设置停止位*/  
    switch (stopbits)
    {   
        case 1: /*1位停止位*/
            options.c_cflag &= ~CSTOPB;  
            break;  
        case 2: /*2位停止位*/
            options.c_cflag |= CSTOPB;  
        break;
        default:  
            //#if WRITE_COMMU_LOG == 1 
            //  #if NEW_WRITE_LOG == 1
            //      write_log2("Error set_Parity: unsupported stop bits!",sizeof("Error set_Parity: unsupported stop bits!"),NULL,0);
            //  #else
            //      write_log("Error set_Parity: unsupported stop bits!");
            //  #endif
            //#endif
            return (FALSE); 
    }
        
    tcflush(fd,TCIFLUSH);                   /* 清输入队列 */
    options.c_cc[VTIME] = 100;      /* 设置超时10 seconds*/   
    options.c_cc[VMIN] = 0;             /* 更新设置，使马上生效 */
    
//  options.c_iflag |= (IXON | IXOFF);  /*开软件流控*/
    options.c_iflag &= ~(IXON|IXOFF|IXANY); /*关软件流控*/
    
    if (set_crtscts == 1)
        options.c_cflag |= CRTSCTS; /*开硬件流控*/
    else
        options.c_cflag &= ~CRTSCTS;/*关硬件流控*/

    options.c_iflag &= ~IGNCR;  /*不忽略收到的CR字符*/
    options.c_iflag &= ~ICRNL;  

    if (tcsetattr(fd,TCSANOW,&options) != 0)  /* 对串口进行参数设置 */
        {  
            //#if WRITE_COMMU_LOG == 1
            //  #if NEW_WRITE_LOG == 1
            //      write_log2("Error set_Parity: can not setattr!",sizeof("Error set_Parity: can not setattr!"),NULL,0);
            //  #else
            //      write_log("Error set_Parity: can not setattr!");
            //  #endif
            //#endif
            return (FALSE);  
        }
    return (TRUE);
}

/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：设置串口波特率,用于串口发送AT命令时，对串口的设置
    输入参数：
        fd      打开串口的文件句柄
        speed   串口波特率
    输出参数：
        无
    函数返回值：
        无 
    内部处理流程：先读取termios结构，然后调用cfsetispeed cfsetospeed，
                                设置串口的输入、输出波特率。
********************************************************************/
int set_speed(int fd, int32 speed)
{
    int32 i; 
    int32 status = 0; 
    int32 speed_arr[] = {B115200, B57600,B38400, B19200, B9600, B4800, B2400, B1200, B600, B300};
    int32 name_arr[] = {115200, 57600, 38400,  19200,  9600,  4800,  2400,  1200, 600, 300};  
    struct termios Opt;     /*Opt中存储波特率数据结构*/
    tcgetattr(fd, &Opt);    /*获取termios结构*/
    
    for ( i = 0;  i < sizeof(speed_arr) / sizeof(int);  i++) 
    { 
        if  (speed == name_arr[i])  /*查找支持的波特率*/
            { 
                cfsetispeed(&Opt, speed_arr[i]); /*设置输入波特率*/
                cfsetospeed(&Opt, speed_arr[i]); /*设置输出波特率*/
                status = tcsetattr(fd, TCSANOW, &Opt);  /*设置串口新的termios结构*/
                if  (status != 0) 
                    {                                 
                        //#if WRITE_COMMU_LOG == 1
                        //  #if NEW_WRITE_LOG == 1
                        //      write_log2("Error set_speed: can not set speed!",sizeof("Error set_speed: can not set speed!"),NULL,0);
                        //  #else
                        //      write_log("Error set_speed: can not set speed!");
                        //  #endif
                        //#endif
                        RADIO_FMT_DEBUG("波特率设置失败 status = %d \n", status);
                        return -1;     
                    }    
                tcflush(fd,TCIOFLUSH); /*清除串口的输入输出队列中的数据*/
                RADIO_FMT_DEBUG("波特率设置成功 speed = %d \n", speed);
                return 1;                     
            }  
    }
    
    //#if WRITE_COMMU_LOG == 1
    //  #if NEW_WRITE_LOG == 1
    //      write_log2("Error set_speed: unsupported speed!",sizeof("Error set_speed: unsupported speed!"),NULL,0);
    //  #else
    //      write_log("Error set_speed: unsupported speed!");
    //  #endif
    //#endif
    RADIO_FMT_DEBUG("波特率设置失败 status = %d \n", status);
    return -1;
}

/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：锁串口
    输入参数：
        fd 串口文件句柄
        type 锁类型 F_RDLCK 共享锁；F_WRLCK 互斥锁
    输出参数：
        无 
    函数返回值：
            
    内部处理流程：
********************************************************************/
int setlock(int fd, int type)
{
    struct flock lock;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 1;
    lock.l_type = type;
    
    if (fcntl(fd, F_SETLK, &lock) == 0)
        return 1;
    else
        {
            //#if WRITE_COMMU_LOG == 1
            //  #if NEW_WRITE_LOG == 1
            //      write_log2("Error setlock: The serial is using!",sizeof("Error setlock: The serial is using!"),NULL,0);
            //  #else
            //      write_log("Error setlock: The serial is using!");
            //  #endif
            //#endif
            return -1;
        }
}

/*********************************************************************
    所属功能单元：动态库公用函数
    功能描述：打开串口,进行串口参数设置
    输入参数：
        fd                  打开串口的文件句柄
        dwBaudRate  串口波特率
        byByteSize  数据位
        byStopBits  停止位
        byParity        奇偶校验
        set_crtscts 硬件流控 1-设置 0-不设置
        
    输出参数：
        无
    函数返回值：
        fd  打开设置串口成功
        -1  打开串口失败
        
    内部处理流程：
                                
********************************************************************/
int OpenSerial(const char *dev, const int32 dwBaudRate, const uint8 byByteSize, const uint8 byStopBits, const uint8 byParity, const int8 set_crtscts)
{
    int fd;
    fd = open(dev, O_RDWR|O_NOCTTY|O_NONBLOCK );    /*打开串口*/
      
    if (fd == -1)       /*若打开串口失败，返回-1，并通知应用层*/
        {           
            //#if WRITE_COMMU_LOG == 1
            //  #if NEW_WRITE_LOG == 1
            //      write_log2("Error OpenSerial: Can't Open Serial Port!",sizeof("Error OpenSerial: Can't Open Serial Port!"),NULL,0);
            //  #else
            //      write_log("Error OpenSerial: Can't Open Serial Port!");
            //  #endif
            //#endif
                    
            close(fd);
            RADIO_FMT_DEBUG("OpenSerial \n");

            return -1;  
        }   
        
    if (setlock(fd, F_WRLCK) == -1) /*锁串口*/
        {
            close(fd);
            RADIO_FMT_DEBUG("OpenSerial \n");
            return -1;
        }
        
    if (set_speed(fd, dwBaudRate) == -1)    /*设置串口波特率*/
        {
            //#if WRITE_COMMU_LOG == 1
            //  #if NEW_WRITE_LOG == 1
            //      write_log2("Error OpenSerial: Set Speed Error!",sizeof("Error OpenSerial: Set Speed Error!"),NULL,0);
            //  #else
            //      write_log("Error OpenSerial: Set Speed Error!");
            //  #endif
            //#endif
            RADIO_FMT_DEBUG("OpenSerial \n");
            close(fd);
            return -1;
        }           

    if (set_Parity(fd, byByteSize, byStopBits, byParity, set_crtscts) == FALSE) /*设置串口参数 数据位 校验 停止位*/ 
        {
            //#if WRITE_COMMU_LOG == 1
            //  #if NEW_WRITE_LOG == 1
            //      write_log2("Error OpenSerial: Set Parity Error!",sizeof("Error OpenSerial: Set Parity Error!"),NULL,0);
            //  #else
            //      write_log("Error OpenSerial: Set Parity Error!");
            //  #endif
            //#endif
            RADIO_FMT_DEBUG("OpenSerial \n");
            close(fd);
            return -1;
        }
    return fd;
}

uint8 wdasn_radio_state = 0;
extern RADIO_DATA_T g_tRADIOPublic;
/*********************************************************************
    所属功能单元：电台动态库
    功能描述：初始化电台所用串口
    输入参数：
        pCommPara       本地通讯参数
        pPeerAddr       通讯对端参数
        pCommStatus 通讯状态参数
    输出参数：
        无
    函数返回值：
        1           成功
        -1      失败
    内部处理流程：打开串口并进行串口初始化
********************************************************************/
int DeviceInit_wdasn_radio()
{
#if 0
    return 0;
#else
    
    //char display_debug;
    int fd_232;                             /*232标识符*/
    uint8 Devicename_232[30];   /*设备名，如"/dev/ttyS0" */
    int32 dwBaudRate_232;      /*波特率*/
    uint8 byByteSize_232;           /*数据位, 7-8 */
    uint8 byParity_232;             /*校验 0-4=None,Odd,Even,Mark,Space*/
    uint8 byStopBits_232;           /*停止位 0,1,2 = 1, 1.5, 2*/ 
    //int8 cch_init_state;
    int8 at_status;
    //int8 loop_cnt = 0;
    char  rcv_buf[100],snd_buf[100];
    char *radio_status = NULL;
    int value;
    //char clear_buf[50] = {0};

    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio.\n");
    radio_state_guishow(0);

    sprintf((char *)Devicename_232, "%s", g_RadioCommPara.sDeviceName);
    dwBaudRate_232 = g_RadioCommPara.dwBaudRate;
    byByteSize_232 = g_RadioCommPara.byByteSize;
    byParity_232 = g_RadioCommPara.byParity;
    byStopBits_232 = g_RadioCommPara.byStopBits;
    radio_connectionstate_guishow("电台打开串口", strlen("电台打开串口"), 10);
    RADIO_FMT_DEBUG("Open radio rs232 %s  dwBaudRate_232 = %ld\n", (char *)Devicename_232, dwBaudRate_232);
    fd_232 = OpenSerial((char *)Devicename_232, dwBaudRate_232, byByteSize_232, byStopBits_232, byParity_232, 0);
    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio fd:%d\n", fd_232);
    
    if (fd_232 > 0)
    {
        radio_connectionstate_guishow("电台扫描信道", strlen("电台扫描信道"), 10);
        radio_channel_get(fd_232);
        sleep(20);
        if (restart_model_radio() < 0)  /*复位通讯模块*/
            goto ATFAIL;
        
        if (start_model_radio() < 0)        /*启动通讯模块*/
            goto ATFAIL;
        
        /*清数据字典中的信号值、终端IP、SIM卡号*/
        //WriteRamXData(ramdiskNetSignalId, clear_buf);

        wdasn_radio_state = 0xff;
        WriteDTComm(wdasn_radio_state);
        RADIO_FMT_DEBUG("wdasn_radio_state open..\n");
        //Get_Version(fd_232,0);

        //at_status = 0;  //打开串口成功
        //WriteRamXData(ramdiskInitStateId, (char *)&at_status);
        sleep(2);
        radio_connectionstate_guishow("设置行政区划码终端地址", strlen("设置行政区划码终端地址"), 10);
        strcpy(snd_buf,"AT+ADDR=");

        //ReadXPara(TMN_RUN_PARA, A1A2_Addr_ID, rcv_buf, 0);
        commfun_BCD2ASC(&snd_buf[8], (char *)g_RadioConfig.areaCode, 4, 0);
        memset(rcv_buf,0,sizeof(rcv_buf));
        commfun_BCD2ASC(rcv_buf, (char *)g_RadioConfig.devAddr, 5, 0);
        RADIO_FMT_DEBUG("DeviceInit_wdasn_radio rcv_buf=%s,devAddr=%02X %02X %02X\n", rcv_buf, g_RadioConfig.devAddr[0], g_RadioConfig.devAddr[1], g_RadioConfig.devAddr[2]);
        //ReadXPara(TMN_RUN_PARA, Tmr_Addr_ID, rcv_buf, 0);
        sprintf(&snd_buf[12],",%s\r", rcv_buf);
        
        //commfun_printstring(snd_buf, 30);
        RADIO_FMT_DEBUG("DeviceInit_wdasn_radio snd_buf:%s\n", snd_buf);
#if 0   
#if STD_698PRO_BETA == 1
        ReadXPara(TMN_RUN_PARA, Tmn_Addr_OOP_ID, rcv_buf, 0);   //行政区划码
        memmove(rcv_buf,&rcv_buf[1],2);                         
        commfun_BCD2ASC(&snd_buf[8],rcv_buf,4,0);
        memset(rcv_buf,0,16);
        ReadXPara(TMN_RUN_PARA, Tmn_Addr_OOP_ID, tmp_buf, 0);   //终端地址
        sprintf(&snd_buf[12],",");
        rcv_buf[1]= tmp_buf[5];
        rcv_buf[2]= tmp_buf[6];
        commfun_BCD2ASC(tmp,rcv_buf,6,0);
        memcpy(&snd_buf[13],&tmp[1],5);
        sprintf(&snd_buf[18],"\r");
        commfun_printstring(snd_buf, 30);   
#else   

        ReadXPara(TMN_RUN_PARA, A1A2_Addr_ID, rcv_buf, 0);
        commfun_BCD2ASC(&snd_buf[8],rcv_buf,4,0);
        
        memset(rcv_buf,0,16);
        ReadXPara(TMN_RUN_PARA, Tmr_Addr_ID, rcv_buf, 0);
        sprintf(&snd_buf[12],",%05u\r",chartoint32(rcv_buf));
        commfun_printstring(snd_buf, 30);
#endif      
#endif
        // 设置终端短地址
        //at_status = 21+offset_wdasn_status; 
        //WriteRamXData(ramdiskInitStateId, (char *)&at_status);

        at_status = AtCheck(fd_232,snd_buf,"AT+OK\r",1000000);
        if(at_status < 0)
        {   
            //如果识别不到AT+OK则可能电台不支持，如果识别到AT+NO则直接结束
            at_status = AtCheck(fd_232,snd_buf,"AT+NO\r",1000000);
            if(at_status>=0)
            {
                RADIO_FMT_DEBUG("电台不支持AT+ADDR 结束初始化流程\n");
                RADIO_FMT_DEBUG("获得modem版本\n");
                Get_Version(fd_232,0);
                return fd_232;
            }
            goto ATFAIL;
        }
        
        // 设置应用层面向对象地址
        memset(snd_buf, '\0', sizeof(snd_buf));
        //memset(rcv_buf, 0x00, sizeof(rcv_buf));
        //ReadXPara(TMN_RUN_PARA, Tmn_Addr_OOP_ID, rcv_buf, 0);   
        strcpy(snd_buf, "AT+OOPADDR=");
        /*if (rcv_buf[0] > 6)
        {
            rcv_buf[0] = 6;
        }*/
        commfun_BCD2ASC(&snd_buf[11], (char *)g_RadioConfig.tmnAddr, 12, 0);
        snd_buf[23] = '\r';
        
        //commfun_printstring(snd_buf, 30);
        RADIO_FMT_DEBUG("DeviceInit_wdasn_radio snd_buf:%s\n", snd_buf);
        radio_connectionstate_guishow("电台附着申请", strlen("电台附着申请"), 10);
        at_status = AtCheck(fd_232, snd_buf, "AT+OK\r", 1000000);
        if(at_status < 0)
        {
            goto ATFAIL;
        }

        sleep(2);
        
        //at_status = 22+offset_wdasn_status; //启动连接
        
        //WriteRamXData(ramdiskInitStateId, (char *)&at_status);
        radio_connectionstate_guishow("电台连接申请", strlen("电台连接申请"), 10);
        at_status = AtCheck(fd_232, "AT+CONNECT=1\r","AT+OK\r",100000);
        if(at_status < 0)
        {
            goto ATFAIL;
        }

        RADIO_FMT_DEBUG("获得modem版本\n");
        Get_Version(fd_232,0);

        while(1)
        {
#if 0
            if (pass_sec_wdasn())
            {
                WriteDTComm(wdasn_radio_state);
                RadioChannelManage_WDASN(fd_232, pCommPara);
                RadioSpeakManage(fd_232, &wdasn_radio_state);

                //查场强
                memset(rcv_buf, 0x0, sizeof(rcv_buf));
                value = -1;
                at_status = AtGet(fd_232, "AT+FILED=999DB\r", rcv_buf, sizeof(rcv_buf));    /*串口发送AT命令*/
                if (at_status > 0)
                {
                    /*
                    AT+FIELD=999DB\r
                    返回：AT+FIELD=XXXXDB
                    XXXX为当前场强值。如上小于四位前面补0，最大为5000，表示50.00dBuV。
                    */
                    char *ptemp = NULL;
                    if ( (ptemp = strstr(rcv_buf, "AT+FIELD=")) != NULL )
                    {
                        ptemp += 9;         //"AT+FIELD= 在rcv_buf中的偏移量
                        ptemp[4] = 0;
                        
                        value = atoi(ptemp);
                        value /= 100;       //缩小100倍
                    }
                }
                WriteRamXData(ramdiskNetSignalId, (char *)&value);
            }
#endif
            if (g_RadioConfigChangeFlag == 1)
            {
                g_RadioConfigChangeFlag = 0;
                RADIO_FMT_DEBUG("设置电台通信地址，重新初始化\n");
                goto ATFAIL;
            }

            if (g_tRADIOPublic.tDevStatus.bParaConfig == TRUE)
            {
                g_tRADIOPublic.tDevStatus.bParaConfig = FALSE;

                if (SetRadioChannel_WDASN(fd_232, g_RadioConfig.channel, g_RadioConfig.power, g_RadioConfig.sensitivity) != 0)
                {
                    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio SetRadioChannel_WDASN ret fail.\n");
                    sleep(2);
                    continue;
                }
                else
                {
                    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio SetRadioChannel_WDASN ret succ.\n");
                }
            }

            RADIO_DEV_MSG_T *pMsg = radio_read_queue(&(g_tRADIOPublic.tDevPorc.cfgQueue));

            if(pMsg)
            {
                RADIO_FMT_DEBUG("DeviceInit_wdasn_radio pMsg->proc=0x%04X.\n", pMsg->proc);
                switch(pMsg->proc)
                {
                    case RADIO_IOP_GET_CONFIG:
                    {
                        RadioGetCfgProc(fd_232, pMsg);
                        break;
                    }
                    case RADIO_IOP_GET_STATUS:
                    {                         
                        radio_status_get(fd_232, pMsg);
                    }
                    case RADIO_IOP_GET_VERSION:
                    {
                        RadioGetVersionProc(fd_232, pMsg);
                        break;
                    }
                    default: break;
                }

                free(pMsg);
            }
#if 0
            if (g_RadioSpeakerFlag == 2)
            {
                if (OpenSpeak(&wdasn_radio_state) == 1)
                {
                    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio 打开扬声器成功.\n");
                }
            }
            else if (g_RadioSpeakerFlag == 1)
            {
                if (CloseSpeak(&wdasn_radio_state) == 1)
                {
                    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio 关闭扬声器成功.\n");
                }
            }
#endif
            memset(rcv_buf, 0x0, sizeof(rcv_buf));
            at_status = AtGet(fd_232, "AT+STATUS=?\r", (uint8 *)rcv_buf, 20);    /*串口发送AT命令*/
            if (at_status < 0)
            {
                //value = 1;  //通讯模块异常
                //WriteRamXData(ramdiskErrCodeId, (char *)&value);
                sleep(2);
                continue;
            }
            
            radio_status = strstr(rcv_buf, "AT+STATUS=") ;
            if(radio_status == NULL)
            {
                RADIO_FMT_DEBUG("DeviceInit_wdasn_radio at_status AtGet: %s\n",rcv_buf);
                continue;
            }
            
            radio_status += 10; //readio 在rcv_buf中的偏移量
            value = atoi(radio_status);
            
            RADIO_FMT_DEBUG("DeviceInit_wdasn_radio at_status radio_status : %d\n", value);
            if((value >= 1)&&(value <= 4) )
            {   
                //value += (16+offset_wdasn_status);      //readio 在g_czCchState 中的偏移量
                
                //WriteRamXData(ramdiskInitStateId, (char *)&value);
                
                if(value == 4)
                {
                    radio_state_guishow(1);
                    sleep(3);//为了可以看到连接成功
                    radio_connectionstate_guishow("电台连接成功", strlen("电台连接成功"), 10);
                    //cch_init_state = CCH_INIT_SUCCESS;
                    //WriteRamXData(ramdiskRunStateId, (char *)&cch_init_state);
                    break;
                }
            }
            else if((value == 5))
            {
                //value = 16;     //readio 在g_czCchErr 中的偏移量
                //WriteRamXData(ramdiskErrCodeId, (char *)&value);
                //break;
            }
#if 0
            /* 仅供测试消息接口使用 */
            if(value == 7)
            {
                radio_state_guishow(1);
                sleep(3);
                radio_connectionstate_guishow("电台连接成功", strlen("电台连接成功"), 10);
                //cch_init_state = CCH_INIT_SUCCESS;
                //WriteRamXData(ramdiskRunStateId, (char *)&cch_init_state);
                break;
            }
#endif
            else
                radio_status = NULL;
                
            sleep(1);
        }
    }
    else
    {
        //cch_init_state = CCH_INIT_FAIL;
        //WriteRamXData(ramdiskRunStateId, (char *)&cch_init_state);
        
        //display_debug = 0x00;
        //WriteRamXData(ramdiskErrCodeId, (char *)&display_debug);
        radio_connectionstate_guishow("电台打开串口失败", strlen("电台打开串口失败"), 10);
        close(fd_232);
        return -1;
    }
    return fd_232;  /*返回串口文件句柄*/
    
ATFAIL:
        
    //cch_init_state = CCH_INIT_FAIL;
    //WriteRamXData(ramdiskRunStateId, (char *)&cch_init_state);
    
    //value = 1;  //通讯模块异常
    //WriteRamXData(ramdiskErrCodeId, (char *)&value);
    radio_connectionstate_guishow("电台通讯模块异常", strlen("电台通讯模块异常"), 10);
    sleep(2);
    close(fd_232);
    //close(gfd_rdo);
    
    return -1;
#endif
}

void commfun_selectsleep(unsigned sec, unsigned us)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = us;
    select(0,0,0,0,&tv);
}

/*读串口数据*/
int32 ReadWdasn_RadioData(int fd_232, int32 speed, uint8 *buf, int32 read_len)
{
    int32 delay;
    int32 recv_len;
    int32 data_len;
    
    delay = 240*1200/speed;
    
    data_len = 0;
    recv_len = 0;
    
    recv_len = read(fd_232, buf, read_len); /*调用read()，进行串口数据接收*/

    while (recv_len > 0)        /*read()在没有数据时，返回-1*/
    {
//      usleep(delay);
        commfun_selectsleep(0, delay);
        data_len += recv_len;
        recv_len = read(fd_232, buf+data_len, read_len-data_len);
        if (recv_len == read_len-data_len)
        {
            data_len = read_len;
            break;
        }
    }
    return data_len;    
}
#if 0
/*********************************************************************
    所属功能单元：电台动态库
    功能描述：电台数据接收接口
    输入参数：
        fd_232          串口文件句柄
        pBuffer         接收数据缓冲区
        pCommPara       本地通讯参数
        pPeerAddr       通讯对端参数
        pCommStatus 通讯状态参数
    输出参数：
        pBuffer         接收到的数据
    函数返回值：
        data_len        接收到数据的长度
        0                       没有接收到数据
    内部处理流程：进行串口数据接收，当接收到超过15个字节或者数据接收完后
                                才将接收到的数据放入pBuffer缓冲区中，函数返回接收到数据
                                的长度
********************************************************************/
int32 DeviceRecv_wdasn_radio (int fd_232, uint8* pBuffer, int32 nBufLen, COMM_PARA * pCommPara, PEER_ADDR* pAddr, COMM_STATUS *pCommStatus)
{
    #if 0
    return 0;
    #else
    int32 recv_len;
    uint8* pos = 0;
    uint8 tmp;
    int ret;

    uint32 ramdiskNetSignalId = Net_Signal;

    #if DOUBLE_CHANNEL_EN == 1
    if (CheckIsDoubleChannel() == TRUE)
    {
        ramdiskNetSignalId = Net_Signal_WDASN;
    }
    #endif
    
    recv_len = ReadWdasn_RadioData(fd_232, pCommPara->Radio.dwBaudRate, pBuffer, nBufLen);
    if (recv_len > 0)
    {
        pos = strstr((char *)pBuffer, "AT+STATUS=6\r");
        if ((pos != NULL) && (pos - pBuffer < 6))   //-防止报文中有AT指令一样的数据-
        {
            tmp = 0xAA;
            ret = WriteRamXData(RX_Restart_Radio, (char*)&tmp);
        }
        //另外，当对终端进行通信时，电台会把场强和数据质量跟随在规约帧的后面送出
        //格式“68 L  L 68 ………….CS 16 MM NN”。其中的MM表示数据质量，NN为场强，数据格式为Bin格式。
        if(pBuffer[recv_len - 3] == 0x16)
        {
            //printf("recv_len = %ld    pBuffer[recv_len - 1] = %d\n", recv_len, pBuffer[recv_len - 1]);
            WriteRamXData(ramdiskNetSignalId, (char *)&pBuffer[recv_len - 1]);
        }
    }

    /*if (pass_sec_wdasn())
    {
        WriteDTComm(wdasn_radio_state);
        RadioChannelManage_WDASN(fd_232, pCommPara);
        RadioSpeakManage(fd_232, &wdasn_radio_state);
    }*/

    return recv_len;
    #endif
}

int32 DeviceSend_wdasn_radio(int fd_232, uint8* pBuffer, int32 nBufLen, COMM_PARA * pCommPara, PEER_ADDR* pAddr, COMM_STATUS *pCommStatus)
{
    #if 0
    return 0;
    #else
    uint32 send_len;
    //uint32 ptt_delay;
    
    if ((fd_232 < 0)||(pBuffer == NULL)||(nBufLen == 0))
        return -1;
        
    //ptt_delay = ReadPttDelay();

    send_len = write(fd_232, pBuffer, nBufLen);
    tcdrain(fd_232);
    return send_len;
    #endif
}

/*********************************************************************
    所属功能单元：电台动态库
    功能描述：串口关闭接口
    输入参数：
        fd_232          串口文件句柄
        pCommStatus 通讯状态参数
    输出参数：
        无 
    函数返回值：
        无
    内部处理流程：关闭fd_232文件句柄
********************************************************************/
void DeviceClose_wdasn_radio (int fd_232, COMM_STATUS *pCommStatus)
{
    close(fd_232);
}
#endif
extern time_t g_RadioOpenTime;

/**********************************************************************
* @name      : radio_dev_proc
* @brief     ：mqtt消息处理
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int radio_dev_proc(void *pArg, void *pPublic, void *pPrivate)
{
    RADIO_DATA_T  *pRadiodata = (RADIO_DATA_T *)pPublic;
    RADIO_DEV_H_T *pRadioHandle = (RADIO_DEV_H_T *)pPrivate;
    char *pos = NULL;

    
    if (g_RadioConfigChangeFlag == 1)
    {
        RADIO_FMT_DEBUG("设置电台参数，重新初始化\n");
        pRadioHandle->status = RADIO_DEV_STATUS_INIT;
        g_RadioConfigChangeFlag = 0;
    }

    //定时检测句柄状态(1秒一次)，异常则重新初始化
    radio_check_devfd(pRadioHandle);
    
    if(RADIO_DEV_STATUS_INIT == pRadioHandle->status && pRadioHandle->devfd >= 0)
    {
        radio_uart_close(pRadioHandle->devfd);
        pRadioHandle->devfd = -1;
        pRadiodata->bWork = FALSE;
    }
    
    switch(pRadioHandle->status)
    {
        case RADIO_DEV_STATUS_INIT:
        {
            int fd = -1;
#if (defined AREA_SHANGHAI || defined AREA_DEFAULT)
            fd = DeviceInit_radio();
#else
            fd = DeviceInit_wdasn_radio();

#endif
            
            if(fd < 0)
            {
                sleep(10);
            }
            else
            {
                pRadioHandle->status = RADIO_DEV_STATUS_WORK;
                pRadioHandle->devfd = fd;
                pRadiodata->bWork = TRUE;
                starttime= time(NULL);
            }
            break;
        }
        case RADIO_DEV_STATUS_WORK:
        {
            /* 工作状态(发送、接收) */
            radio_working_proc(pRadioHandle->devfd, pRadioHandle, pRadiodata);
            break;
        }
        default:
        {
            pRadioHandle->status = RADIO_DEV_STATUS_INIT;
            break;
        }
    }
    update_modem_start(pRadioHandle->devfd);
    /* 处理缓存消息(透传、上报) */
    if (pRadioHandle->recvlen > 0)
    {
        pos = strstr((char *)pRadioHandle->recv, "AT+STATUS=6\r");
        if ((pos != NULL) && (pos - (char *)pRadioHandle->recv < 6))   //-防止报文中有AT指令一样的数据-
        {
            RADIO_FMT_DEBUG("请求终端对电台进行复位\n");
            pRadioHandle->status = RADIO_DEV_STATUS_INIT;
            return 1000;
        }

#ifdef AREA_SHANGHAI
        //0x33为带RS编码处理
        uint8 buftemp[256] = {0};
        uint16 templen = 0;
        uint16 offset = 0;
        uint16 reallen = 0;

        g_bRadioRS = FALSE;
        if(pRadioHandle->recv[0] == 0x33 && 
           pRadioHandle->recv[1] == 0x68 && 
           pRadioHandle->recv[2] == pRadioHandle->recv[4] && 
           pRadioHandle->recv[3] == pRadioHandle->recv[5] && 
           pRadioHandle->recv[6]  == 0x68)
        {
            //取得报文长度
            memcpy(&reallen, pRadioHandle->recv + 2, 2);
            reallen >>= 2;
            reallen += 8 + 1;

            buftemp[templen++] = 0x33;
            offset++;
            while(1)
            {
                if(templen + 9 < reallen)
                {
                    memcpy(buftemp + templen, pRadioHandle->recv + offset, 9);
                    templen += 9;
                    offset += 15;
                }
                else
                {
                    memcpy(buftemp + templen, pRadioHandle->recv + offset, reallen - templen);
                    templen = reallen;
                    break;
                }
            }
            /* 判断是否376报文, 只判断尾巴是16 */
            if(buftemp[reallen - 1] == 0x16)
            {
                memcpy(pRadioHandle->recv, buftemp, reallen);
                pRadioHandle->recvlen = reallen;
                RADIO_BUF_DEBUG(buftemp, reallen, "识别RS编码\n");
                g_bRadioRS = TRUE;
            }
        }
        else if(0x33 == pRadioHandle->recv[0] && 0x10 == pRadioHandle->recv[1] && 0x11 != pRadioHandle->recv[1 + 7])
        {
            //固定帧
            uint8 seq;
            /* 解析判断Tp */
            seq = pRadioHandle->recv[1 + 8];
            if(seq & 0x80)
            {
                reallen = 1 + 15 + 6;
            }
            else
            {
                reallen = 1 + 15;
            }

            /* 判断尾巴 */
            if(reallen <= pRadioHandle->recvlen && 0x16 == pRadioHandle->recv[reallen - 1])
            {
                buftemp[templen++] = 0x33;
                offset++;
                while(1)
                {
                    if(templen + 9 < reallen)
                    {
                        memcpy(buftemp + templen, pRadioHandle->recv + offset, 9);
                        templen += 9;
                        offset += 15;
                    }
                    else
                    {
                        memcpy(buftemp + templen, pRadioHandle->recv + offset, reallen - templen);
                        templen = reallen;
                        break;
                    }
                }
                memcpy(pRadioHandle->recv, buftemp, reallen);
                pRadioHandle->recvlen = reallen;
                RADIO_BUF_DEBUG(buftemp, reallen, "识别RS编码\n");
                g_bRadioRS = TRUE;
            }
        }
#endif
        radio_devport_report(pRadioHandle->recv, pRadioHandle->recvlen);
        memset(pRadioHandle->recv, 0, pRadioHandle->recvlen);
        pRadioHandle->recvlen = 0;
    }
    return 1000;
}

int CheckRadioCrc(uint8 *buf, uint32 len)
{
	int i;
	uint16 crc = 0;;
	
	for(i=0; i<len-2; i++)
	{
		crc += buf[i];
	}
	
	if((((crc>>8)&0xff) == buf[len-2])&&((crc&0xff) == buf[len-1]))
		return 0;
	else
		return -1;
}

#define PRO_CHANNEL             0xf7
int radio_channel_get_by_command(int fd)
{
    int ret = 0;
    //uint8 answer_cch[5];
	//sint8 send_flag;
	sint32 read_len;
	uint8 read_buf[12];
	uint8 read_channel_command[12];
	uint8 state;
	
	read_channel_command[0] = 0xaa;
	read_channel_command[1] = 0x12;
	read_channel_command[2] = 0x00;
	read_channel_command[3] = 0x00;
	read_channel_command[4] = 0x00;
	read_channel_command[5] = 0x00;
	read_channel_command[6] = 0x00;
	read_channel_command[7] = 0x00;
	read_channel_command[8] = 0x00;
	read_channel_command[9] = 0x00;
	read_channel_command[10] = 0x00;
	read_channel_command[11] = 0x12;
					
    state = radio_state;
	radio_state = PRO_CHANNEL;
	WriteDTComm(radio_state);

	set_speed(fd, 9600);
	set_Parity(fd, 8, 1, 'E', 0);
	
	//send_flag = write(fd, read_channel_command, sizeof(read_channel_command));
	write(fd, read_channel_command, sizeof(read_channel_command));
	tcdrain(fd);
	usleep(200000);
	
	read_len = read(fd, (char *)read_buf, sizeof(read_buf));
	if (read_len > 0)
	{
		RADIO_BUF_DEBUG(read_buf, read_len, "read the ack: \n");
		//putout(read_buf, sizeof(read_buf));
		if ((read_buf[1] == 0xee)&&(read_buf[2] == 0x01)&&(CheckRadioCrc(read_buf, 12) == 0))
		{
			answer_cch[0] = 0x80;
			answer_cch[1] = read_buf[3];	/*频道*/
			answer_cch[2] = read_buf[4];	/*功率*/
			answer_cch[3] = read_buf[5];	/*灵敏度*/
			answer_cch[4] = answer_cch[0]+answer_cch[1]+answer_cch[2]+answer_cch[3];
			//WriteRamXData(Radio_Channel_Set, (char *)&answer_cch);
		}
		else	/*查询失败*/
		{
			answer_cch[0] = 0x80;
			answer_cch[1] = 0xff;
			answer_cch[2] = 0xff;
			answer_cch[3] = 0xff;
			answer_cch[4] = answer_cch[0]+answer_cch[1]+answer_cch[2]+answer_cch[3];
			//WriteRamXData(Radio_Channel_Set, (char *)&answer_cch);
			ret = -1;
		}
	}
	else	/*查询失败*/
	{
		answer_cch[0] = 0x80;
		answer_cch[1] = 0xff;
		answer_cch[2] = 0xff;
		answer_cch[3] = 0xff;
		answer_cch[4] = answer_cch[0]+answer_cch[1]+answer_cch[2]+answer_cch[3];
		//WriteRamXData(Radio_Channel_Set, (char *)&answer_cch);	
		ret = -1;
	}
	
    set_speed(fd, g_RadioCommPara.dwBaudRate);
    set_Parity(fd, g_RadioCommPara.byByteSize, g_RadioCommPara.byStopBits, g_RadioCommPara.byParity, 0);
    radio_state = state;
	WriteDTComm(radio_state);

    if(ret<0)
        RADIO_FMT_DEBUG("查询电台频道失败\n");
    return ret;
}

uint32 SetRadioChannel_command(int fd_232, uint8 ch, uint8 ptx, uint8 sq)
{
    uint32 ret = 0;
    uint16 crc = 0;
	//uint8 answer_cch[5];
	//sint8 send_flag;
	sint32 read_len;
	uint8 read_buf[12];
	uint8 set_cch_cmd[12];
	uint8 state;
	
	set_cch_cmd[0] = 0xaa;
	set_cch_cmd[1] = 0x02;
	set_cch_cmd[2] = ch;
	set_cch_cmd[3] = ptx + 1;
	set_cch_cmd[4] = sq + 1;
	set_cch_cmd[5] = 0x00;
	set_cch_cmd[6] = 0x00;
	set_cch_cmd[7] = 0x00;
	set_cch_cmd[8] = 0x00;
	set_cch_cmd[9] = 0x00;
	crc = set_cch_cmd[1]+set_cch_cmd[2]+set_cch_cmd[3]+set_cch_cmd[4];
	set_cch_cmd[10] = (crc>>8)&0xff;
	set_cch_cmd[11] = crc&0xff;
					
    state = radio_state;
	radio_state = PRO_CHANNEL;
	WriteDTComm(radio_state);

	set_speed(fd_232, 9600);
	set_Parity(fd_232, 8, 1, 'E', 0);
	
	//send_flag = write(fd_232, set_cch_cmd, sizeof(set_cch_cmd));
    write(fd_232, set_cch_cmd, sizeof(set_cch_cmd));
    RADIO_BUF_DEBUG(set_cch_cmd, 12, "write: \n");
	tcdrain(fd_232);
	usleep(200000);
	
	read_len = read(fd_232, (char *)read_buf, sizeof(read_buf));
	if (read_len > 0)
	{
		RADIO_BUF_DEBUG(read_buf, read_len, "read the ack: \n");
		//putout(read_buf, sizeof(read_buf));
		if ((read_buf[1] == 0xee)&&(read_buf[2] == 0x01)&&(CheckRadioCrc(read_buf, 12) == 0))
		{
			answer_cch[0] = 0x81;
			answer_cch[1] = 0x00;
			answer_cch[2] = 0x00;
			answer_cch[3] = 0x00;
			answer_cch[4] = 0x81;
			//WriteRamXData(Radio_Channel_Set, (char *)&answer_cch);
		}
		else	/*查询失败*/
		{
			answer_cch[0] = 0x80;
			answer_cch[1] = 0xff;
			answer_cch[2] = 0xff;
			answer_cch[3] = 0xff;
			answer_cch[4] = 0x7d;
			//WriteRamXData(Radio_Channel_Set, (char *)&answer_cch);		
			ret = 1;
		}
	}
	else	/*查询失败*/
	{
		answer_cch[0] = 0x80;
		answer_cch[1] = 0xff;
		answer_cch[2] = 0xff;
		answer_cch[3] = 0xff;
		answer_cch[4] = 0x7d;
		//WriteRamXData(Radio_Channel_Set, (char *)&answer_cch);			
		ret = 1;
	}
	set_speed(fd_232, g_RadioCommPara.dwBaudRate);
	set_Parity(fd_232, g_RadioCommPara.byByteSize, g_RadioCommPara.byStopBits, g_RadioCommPara.byParity, 0);	

    radio_state = state;
    WriteDTComm(radio_state);
    if(ret != 0)
        RADIO_FMT_DEBUG("设置电台频道失败\n");
    return ret;
}

int radio_channel_get(int fd)
{
    int ret;
    uint16 i;
    char sendbuf[100]={0};
    uint8 recvbuf[100]={0};

    sprintf(sendbuf,"AT+CHANNEL=?\r");
    ret = AtGet(fd,sendbuf,recvbuf,sizeof(recvbuf));
    for(i=0;i<ret;i++)
        {

    }
    if(ret<0)
        RADIO_FMT_DEBUG("查询电台频道失败\n");
    return ret;
}

unsigned short check_getCrc16(char *ptr, long count)
{
    long i;
    unsigned short crc;
    crc = 0;
    while (count--)
    {
        crc = crc ^ (int) * ptr++ << 8;

        for (i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        }
    }
    return (crc & 0xFFFF);
}

BOOL check_CalcCRC(const char *pfilename, unsigned short *pCRC)
{
    struct stat sta;
    char filesourcepath[256];//文件源路径
    #if SHANXI1_FUN == 1 && HARDWARE_TYPE == 1
	if(strstr(pfilename, "SHRModem") == NULL && strstr(pfilename, "SCRModem") == NULL)
        return FALSE;
    #else
	if(strstr(pfilename, "SHRModem") == NULL && strstr(pfilename, "TmnModem") == NULL)
        return FALSE;
    #endif

    if(strstr(pfilename, ".cry") == NULL)
        return FALSE;

    sprintf(filesourcepath, "/data/app/radioManager/%s", pfilename);
    if( stat(filesourcepath, &sta) < 0 )
        return FALSE;

    if(sta.st_size <= 0)
        return FALSE;

    char tmpFileBuf[sta.st_size];
    int     fd;

    fd = open(filesourcepath, O_RDONLY, S_IRWXU);
    if(fd < 0)
        return FALSE;

    if( read(fd, tmpFileBuf, sta.st_size) != sta.st_size)
    {
        close(fd);
        return FALSE;
    }
    close(fd);

    *pCRC = check_getCrc16(tmpFileBuf, sta.st_size);
    return TRUE;
}


/**********************************************************
函数功能:获得文件传输起始标志0x43,也即大写C
函数输入值:fd_232串口句柄
函数返回值:成功,则返回接受长度；0失败
*************************************************************/
int State_CODE(int fd_232)
{
    int RcvNum=0;
    int Rcvlen=0;
    uint8 Rbuf[50];
    memset(Rbuf,0,sizeof(Rbuf));

    while(RcvNum++ < 10)
    {
        Rcvlen=0;
        memset(Rbuf,0,sizeof(Rbuf));

        Rcvlen=radio_uart_recv(fd_232, Rbuf, 10240, 10, 50);

        if(Rcvlen > 0 && Rbuf[0] == MODEM_C)
        {
            RADIO_FMT_DEBUG("可以开始传输文件\n");
            return Rcvlen;
        }
        sleep(1);
    }
    RADIO_FMT_DEBUG("没有获得传输文件许可\n");
    return 0;
}


/*********************************************************************
    所属功能单元：电台动态库 上海用的
    功能描述：初始化电台所用串口
    输入参数：
    输出参数：
        无
    函数返回值：
        1           成功
        -1      失败
    内部处理流程：打开串口并进行串口初始化
********************************************************************/

int DeviceInit_radio()
{
    int fd_232;                             /*232标识符*/
    uint8 Devicename_232[30];   /*设备名，如"/dev/ttyS0" */
    int32 dwBaudRate_232;      /*波特率*/
    uint8 byByteSize_232;           /*数据位, 7-8 */
    uint8 byParity_232;             /*校验 0-4=None,Odd,Even,Mark,Space*/
    uint8 byStopBits_232;           /*停止位 0,1,2 = 1, 1.5, 2*/ 
    int ret;

    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio.\n");
    radio_state_guishow(0);

    sprintf((char *)Devicename_232, "%s", g_RadioCommPara.sDeviceName);
    dwBaudRate_232 = g_RadioCommPara.dwBaudRate;
    byByteSize_232 = g_RadioCommPara.byByteSize;
    byParity_232 = g_RadioCommPara.byParity;
    byStopBits_232 = g_RadioCommPara.byStopBits;
    
    RADIO_FMT_DEBUG("Open radio rs232 %s  dwBaudRate_232 = %ld\n", (char *)Devicename_232, dwBaudRate_232);
    fd_232 = OpenSerial((char *)Devicename_232, dwBaudRate_232, byByteSize_232, byStopBits_232, byParity_232, 0);
    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio fd:%d\n", fd_232);
    
    if(fd_232 >0)
    {
        do
        {
            RADIO_FMT_DEBUG("开始复位电台\n");
            if (restart_model_radio() < 0)  /*复位通讯模块*/
                break;
            radio_state = POWER_OFF;
        
            if (start_model_radio() < 0)        /*启动通讯模块*/
                break;
            radio_state = POWER_ON;
            RADIO_FMT_DEBUG("查询电台频道\n");
            #ifdef AREA_DEFAULT
            ret = radio_channel_get_by_command(fd_232);
            #else
            ret = radio_channel_get(fd_232);
            #endif
            if(ret<0)
                break;
            radio_state_guishow(1);
            RADIO_FMT_DEBUG("电台初始化成功\n");
            sleep(3);
            return fd_232;
        }while(0);
        RADIO_FMT_DEBUG("电台初始化失败\n");
        close(fd_232);
        return -1;
    }else
    {
        return -1;
    }


}

/**********************************************************
函数功能:切换model状态
函数输入值:fd_232串口句柄
函数返回值:1成功0失败
*************************************************************/
int State_Start(int fd_232)
{
    /*
    发送AB 05 05 AB FB 5B 16，
    电台回复AB 05 05 AB FE 5E 16。
    等3秒，电台进入升级状态，
    然后串口速率调到57600，
    就可以用YModem协议发送数据了
    */

    int i=0;
    int RLen=0;
    int RcvNum=0;
    char *findcode=NULL;

    uint8 buf[20];
    uint8 Rbuf[128];
    memset(buf,0,sizeof(buf));
    memset(Rbuf,0,sizeof(Rbuf));

    //切换升级的报文
    buf[i++] = 0xFF;buf[i++] = 0xFF;buf[i++] = 0xFF;buf[i++] = 0xFF;
    buf[i++] = 0xAB;buf[i++] = 0x05;buf[i++] = 0x05;buf[i++] = 0xAB;
    buf[i++] = 0xFB; buf[i++] = 0x5B;buf[i++] = 0x16;

    if(Code_Send_Model(buf,i,fd_232) > 0)
    {

        while(RcvNum++ < 20)
        {
            if(RcvNum == 10)
            {
                //中间再次发送
                i = 11;
                Code_Send_Model(buf,i,fd_232);
                usleep(100000);
            }

            memset(Rbuf,0,sizeof(Rbuf));
            RLen=0;
            //延时接受
            usleep(10000);

            RLen=radio_uart_recv(fd_232, Rbuf, 10240, 10, 50);
            if(RLen >= 7)
            {
                i=0;
                findcode = (char *)&Rbuf[0];

                while(i <= (RLen-7))
                {
                    if(*findcode == 0xAB && *(findcode+1) == 0x05 && *(findcode+2) == 0x05 &&*(findcode+3) == 0xAB
                        && *(findcode+4) == 0xFE && *(findcode+5) == 0x5E && *(findcode+6) == 0x16)
                    {
                        RADIO_FMT_DEBUG("收到合法报文，切换升级状态成功\n");
                        if (set_speed(fd_232, 57600) == -1)    /*设置串口波特率*/
                        {
                            RADIO_FMT_DEBUG("设置串口波特率57600 失败 \n");
                        }  
                        RADIO_FMT_DEBUG("设置串口波特率57600 成功 \n");

                        return 1;
                    }

                    findcode++;
                    i++;
                }
            }
        }
    }
    //此处返回1，是由于，有些时候，发了状态转换帧后，
    //终端可能会停电，即使终端再上电，modem也是出于升级状态，
    //所以即使失败也返回1
    RADIO_FMT_DEBUG("无效报文，切换升级失败\n");
    return 1;
}

/**********************************************************
函数功能:文件传输第一帧，传输文件信息
包括文件名称，文件大小，
函数输入值:fd_232串口句柄
函数返回值:成功,则返回1；0失败
*************************************************************/
extern uint16 CRC16(uint8 *pMsg, uint16 len);
int State_File_One(int fd_232, char *namebuf)
{
    struct stat statbuf1;

    int fdcry=0;
    uint16 CRCNUM=0;
    int RLen=0;	

    uint8 sbuf[128];
    uint8 sizebuf[50];
    uint8 crybuf[2048];
    uint8 Rbuf[1024];

    int i = 0;

    memset(sbuf,0,sizeof(sbuf));
    memset(sizebuf,0,sizeof(sizebuf));
    memset(crybuf,0,sizeof(crybuf));	
    memset(Rbuf,0,sizeof(Rbuf));

    YMODEL_TYPE  YMODEL= {0};

    int RcvNum=0;


    sprintf((char *)sbuf,"/data/app/radioManager/%s",namebuf);

    fdcry = open((char *)sbuf, O_RDONLY);
    if(fdcry< 0)
    {
        RADIO_FMT_DEBUG("升级文件不存在  %s\n", (char *)sbuf);
        return 0;
    }

    if(stat((char *)sbuf,&statbuf1) < 0)
    {
        close(fdcry);
        RADIO_FMT_DEBUG("获得升级文件大小\n");
        return 0;
    }


    //填写结构体
    YMODEL.Kind=MODEM_SOH;
    YMODEL.SEQ[0]=0x00;
    YMODEL.SEQ[1]=0xFF;

    RADIO_FMT_DEBUG("文件长度%ld\n",statbuf1.st_size);

    //拷贝文件名和长度信息
    sprintf((char *)sizebuf,"%ld",statbuf1.st_size);
    memcpy(YMODEL.Sbuf,namebuf,strlen(namebuf));
    memcpy(YMODEL.Sbuf+strlen(namebuf)+1,sizebuf,strlen((char *)sizebuf));

    RADIO_FMT_DEBUG("发送第一帧数据\n");

    if(MODEM_SOH == YMODEL.Kind)
        CRCNUM=CRC16(YMODEL.Sbuf,PACK128);

    YMODEL.CRC[0]=CRCNUM%256;
    YMODEL.CRC[1]=CRCNUM/256;

    //拷贝发送报文
    memcpy(crybuf,&YMODEL,1+2+PACK128);
    memcpy(crybuf+1+2+PACK128,&YMODEL.CRC,2);

    Code_Send_Model(crybuf,1+2+PACK128+2,fd_232);

    sleep(2);

    while(RcvNum++ < 10)
    {
        memset(Rbuf,0,sizeof(Rbuf));
        RLen=0;

        usleep(200000);

        RLen=radio_uart_recv(fd_232, Rbuf, 10240, 10, 50);
        if(RLen >= 2)
        {
            for(i = 0; i < RLen -1; i++)
            {
                if((Rbuf[i] == MODEM_ACK) && (Rbuf[i + 1] == MODEM_C))
                {
                    close(fdcry);
                    return 1;
                }
            }
        }
    }

    sleep(1);

    close(fdcry);
    return 0;
}

void ModemComposeDebug(char *buf, int len, char *comm_log)
{
    int i;
    char *p = comm_log+strlen(comm_log);

    if (len < 2500)
    {
        for (i=0; i<len; i++)
        {	
            sprintf(p+3*i, "%02x ", buf[i]);   
        }
    }
    else
    {
        for (i=0; i<2500; i++)
        {	
            sprintf(p+3*i, "%02x ", buf[i]);
        }
    }
}

/**********************************************************
函数功能:文件传输后续帧，传输文件本身
函数输入值:fd_232串口句柄
函数返回值:成功,则返回1；0失败
*************************************************************/
int State_File_Two(int fd_232, char *namebuf)
{
    struct stat statbuf1;

    int fdcry=0;
    uint16 CRCNUM=0;
    int i=0,PackNum=0;
    int RLen=0;	
    uint8 RecvCnt=0;
    uint8 *findcode_I=NULL;
    uint8 sbuf[128];
    uint8 crybuf[2048];
    uint8 Rbuf[1024];
    int send_retry_count = 0;
    // char comm_log[3000] = {0}; 

    memset(Rbuf,0,sizeof(Rbuf));
    memset(sbuf,0,sizeof(sbuf));
    memset(crybuf,0,sizeof(crybuf));	

    YMODEL_TYPE  YMODEL;
    memset(&YMODEL,0,sizeof(YMODEL_TYPE));	

    sprintf((char *)sbuf,"/data/app/radioManager/%s",namebuf);

    fdcry = open((char *)sbuf, O_RDONLY);
    if(fdcry< 0)
    {
        RADIO_FMT_DEBUG("升级文件不存在 %s\n", (char *)sbuf);
        return 0;
    }

    if(stat((char *)sbuf,&statbuf1) < 0)
    {
        RADIO_FMT_DEBUG("获得文件信息失败\n");
        return 0;
    }

    //获得所有的包数，也即序列号
    if(statbuf1.st_size % PACK1024)
        PackNum= statbuf1.st_size / PACK1024 + 1;
    else
        PackNum= statbuf1.st_size / PACK1024;


    //填写结构体
    YMODEL.Kind=MODEM_STX;
    YMODEL.SEQ[0]=0x01;
    YMODEL.SEQ[1]=0xFe;

    while(YMODEL.SEQ[0] <= PackNum)
    {
        memset(crybuf,0,sizeof(crybuf));	
        memset(YMODEL.Sbuf,0,sizeof(YMODEL.Sbuf));	
        // memset(comm_log,0,sizeof(comm_log));

        lseek(fdcry, (YMODEL.SEQ[0]-1)*PACK1024, SEEK_SET);
        read(fdcry, YMODEL.Sbuf, PACK1024);

        if(MODEM_STX == YMODEL.Kind)
            CRCNUM=CRC16(YMODEL.Sbuf,PACK1024);

        YMODEL.CRC[0]=CRCNUM%256;
        YMODEL.CRC[1]=CRCNUM/256;

        //拷贝发送报文
        memcpy(crybuf,&YMODEL,1+2+PACK1024+2);

        RADIO_FMT_DEBUG("总包数%d,已发送第%d包CRC校验码%04x\n",PackNum,YMODEL.SEQ[0],CRCNUM);

        // strcpy(comm_log, "升级数据包内容 :\n");
        // ModemComposeDebug((char *)(crybuf), 1+2+PACK1024+2, comm_log);	

        Code_Send_Model(crybuf,1+2+PACK1024+2,fd_232);
        send_retry_count++;

        RecvCnt = 0;
        while(RecvCnt < 3)
        {
            usleep(300000);
            RLen=radio_uart_recv(fd_232, Rbuf, 10240, 10, 50);
            if(RLen > 0)
            {
                break;
            }
            RADIO_FMT_DEBUG("RecvCnt \n");
            RecvCnt++;
        }
		
        if(RLen > 0)
        {

            //后续帧的回码判断
            i=0;
            findcode_I = &Rbuf[0];

            while(i < RLen)
            {

                if(*findcode_I  == MODEM_NAK)
                {
                    //表示报文有错误，需要重发
                    memset(Rbuf,0,sizeof(Rbuf));
                    RLen=0;
                    RADIO_FMT_DEBUG("***** \n");
                    break;
                }
                else if(*findcode_I  == MODEM_CAN)
                {
                    //取消传输，则直接将对应文件删除。且该线程重启下
                    close(fdcry);
                    system("rm -rf /data/app/radioManager/*.cry");
                    return 0;
                }
                else if((*findcode_I) == MODEM_ACK)
                {
                    send_retry_count = 0;
                    memset(Rbuf,0,sizeof(Rbuf));
                    RLen=0;

                    YMODEL.SEQ[0]++;
                    YMODEL.SEQ[1]--;
                    RADIO_FMT_DEBUG("YMODEL.SEQ[1] = %d\n", YMODEL.SEQ[1]);
                    break;
                }

                findcode_I++;
                i++;
            }
        }
        if(send_retry_count > 6)
        {
            RADIO_FMT_DEBUG("***** \n");
            close(fdcry);
            return 0;
        }
        //初始化buf
        memset(YMODEL.Sbuf,0,sizeof(YMODEL.Sbuf));	

        usleep(800000);
    }
    close(fdcry);
    return 1;
}

/**********************************************************
函数功能:RebootByTaskd
        通过taskd重启终端
函数输入值:无
函数返回值:无
*************************************************************/
void RebootByTaskd(void)
{
       uint16 aTime = 0;
       //DMSG    message;

    /* 向守护进程发送信息，重启终端 */
    //message.mtype = DMSG_REBOOT;
    //message.data0 = 0xFE;
    //SendDMsg(&message);

    for (aTime = 0; aTime < 30; aTime++)
    {
        //KeepMeAlive();      //守护进程监视
        sleep(1);
    }   //等待10秒后还没有重启，自动重启
    reboot(RB_AUTOBOOT);
    return;
}

/**********************************************************
函数功能:传输文件结束应用层发04告知结束
函数输入值:fd_232串口句柄
函数返回值:成功,则返回1；0失败
*************************************************************/
int State_End(int fd_232)
{
    int RcvNum06=0,RcvNum43=0;
    int RLen=0;

    uint8 Rbuf[1024];
    uint8 crybuf[128];

    memset(Rbuf,0,sizeof(Rbuf));
    memset(crybuf,0,sizeof(crybuf));

    crybuf[0] = MODEM_EOT;/*文件发送完成*/
    Code_Send_Model(crybuf,1,fd_232);


    while(RcvNum06 ++ < 10)	
    {
        usleep(200000);

        RLen=Code_Recv_Model(Rbuf,fd_232);

        if((RLen >= 2) && (Rbuf[0] == MODEM_ACK) && (Rbuf[1] == MODEM_C))
        {
            RADIO_FMT_DEBUG("找到结束代码，升级完全结束\n");
            return 1;
        }
        else if((RLen >= 1) && (Rbuf[0] == MODEM_ACK))
        {

            while(RcvNum43 ++ < 10)	
            {
                usleep(400000);
                RLen=Code_Recv_Model(&Rbuf[1],fd_232);
                if((RLen >0) && (Rbuf[1] == MODEM_C))
                {
                    RADIO_FMT_DEBUG("找到结束代码，升级完全结束\n");
                    return 1;
                }
            }

        }	
    }

    RADIO_FMT_DEBUG("没有收到合法码字，程序返回\n");
    return 0;
}

/**********************************************************
函数功能:ymodem协议会继续传输下一文件，
此处发个不包含文件名称和文件大小的
第一帧告知modem没有下一文件
函数输入值:fd_232串口句柄
函数返回值:成功,则返回1；0失败
*************************************************************/
int State_NEXT(int fd_232)
{

    int RcvNum06=0;
    int RLen=0;	
    uint8 Rbuf[512];
    memset(Rbuf,0,sizeof(Rbuf));

    //组织发送报文
    Rbuf[0]=MODEM_SOH;
    Rbuf[1]=0x00;
    Rbuf[2]=0xFF;

    Code_Send_Model(Rbuf,1+2+PACK128+2,fd_232);
    memset(Rbuf,0,sizeof(Rbuf));


    while(RcvNum06 ++ < 10)	
    {
        usleep(200000);

        RLen=radio_uart_recv(fd_232, Rbuf, 10240, 10, 50);

        if((RLen > 0) && (Rbuf[0] == MODEM_ACK))
        {
            RADIO_FMT_DEBUG("不在继续传输下一文件\n");
            return 1;
        }

    }

    RADIO_FMT_DEBUG("没有收到最后判断结束符，程序升级失败\n");
    return 0;
}

/**********************************************************
函数功能:查询升级后的新版本
函数输入值:fd_232串口句柄
函数返回值:成功,则返回1；0失败
*************************************************************/
int Update_Version(int fd_232,int speed,int flag)
{
    int TurnNum=0;

    //首先休眠10秒，等待modem重启
    sleep(10);
    while(TurnNum++ < 10)
    {
        sleep(1);
        if(set_speed(fd_232, speed) > 0)
        {
            sleep(1);
            RADIO_FMT_DEBUG("查询升级后的新版本  读取MODEM版本\n");
            return (Get_Version(fd_232,flag));
        }
    }

    return 0;
}

/*********************************************************************
    所属功能单元：电台升级初始化函数
    功能描述：初始化电台升级所用串口
    输入参数：
        pCommPara       本地通讯参数
        pPeerAddr       通讯对端参数
        pCommStatus 通讯状态参数
    输出参数：
        无
    函数返回值：
        1           成功
        -1      失败
    内部处理流程：打开串口并进行串口初始化
********************************************************************/
int UpdateModemInit()
{
    RADIO_FMT_DEBUG("modem 升级初始化");
    int fd_232;                             /*232标识符*/
    uint8 Devicename_232[30];   /*设备名，如"/dev/ttyS0" */
    int32 dwBaudRate_232;      /*波特率*/
    uint8 byByteSize_232;           /*数据位, 7-8 */
    uint8 byParity_232;             /*校验 0-4=None,Odd,Even,Mark,Space*/
    uint8 byStopBits_232;           /*停止位 0,1,2 = 1, 1.5, 2*/ 
    char sbuf[128] = {0};

    sprintf(sbuf,"/data/app/radioManager/radio_update.cry");

    if (access(sbuf, R_OK) != 0)
    {
        return -2;
    }
    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio.\n");
    radio_state_guishow(0);

    sprintf((char *)Devicename_232, "%s", g_RadioCommPara.sDeviceName);
    dwBaudRate_232 = g_RadioCommPara.dwBaudRate;
    byByteSize_232 = g_RadioCommPara.byByteSize;
    byParity_232 = g_RadioCommPara.byParity;
    byStopBits_232 = g_RadioCommPara.byStopBits;
    radio_connectionstate_guishow("电台打开串口", strlen("电台打开串口"), 10);
    RADIO_FMT_DEBUG("Open radio rs232 %s  dwBaudRate_232 = %ld\n", (char *)Devicename_232, dwBaudRate_232);
    fd_232 = OpenSerial((char *)Devicename_232, dwBaudRate_232, byByteSize_232, byStopBits_232, byParity_232, 0);
    RADIO_FMT_DEBUG("DeviceInit_wdasn_radio fd:%d\n", fd_232);
    return fd_232 > 0 ? fd_232 : -1;

}

/**********************************************************
函数功能:检测modem升级函数
函数输入值:无
函数返回值:无
*************************************************************/
void update_modem(int fd)
{
    //表示升级状态
    int MState = M_OLDVER;
    int aTime =0;

    char namebuf[100] = {0};
    uint8 retry_count = 0;

    memset(namebuf,0,sizeof(namebuf));
    sprintf(namebuf,"radio_update.cry");
    RADIO_FMT_DEBUG("电台升级开始\n");
    while(1)
    {
        switch(MState)
        {
        case M_OLDVER:
            RADIO_FMT_DEBUG("获得modem版本\n");
            Get_Version(fd,0);
            MState = M_START;
            aTime = 0;
            break;

        case M_START:
            RADIO_FMT_DEBUG("切换升级状态\n");
            if(State_Start(fd) > 0)
            {
                MState = M_CODE;
                aTime = 0;
            }
            else
            {
                if(aTime++ > 15)
                {
                    MState = M_FAIL;
                    aTime = 0;
                }
            }
            break;

        case M_CODE:
            aTime ++;
            RADIO_FMT_DEBUG("获得开始传输文件标示\n");
            if((State_CODE(fd)) > 0)
            {
                MState = TRAN_File_FIR;
                aTime = 0;
            }
            else
            {
                if(aTime++ > 3)
                {
                    aTime = 0;
                    MState = M_FAIL;
                }
            }
            break;

        case TRAN_File_FIR:
            RADIO_FMT_DEBUG("文件传输第一帧\n");
            if(State_File_One(fd, namebuf) > 0)
            {
                MState=TRAN_File_FIN;
                aTime = 0;
            }
            else
            {
                if(aTime++ > 10)
                {
                    MState=M_FAIL;
                }
            }                       
            break;

        case TRAN_File_FIN:
            RADIO_FMT_DEBUG("文件传输后续帧\n");
            MState = M_END;
            aTime = 0;
            if(State_File_Two(fd, namebuf) > 0 )
            {
                MState = M_END;
                aTime = 0;
            }
            else
            {
                if(aTime++ > 10)
                {
                    MState=M_FAIL;
                }
            }
            break;

        case M_END:
            RADIO_FMT_DEBUG("升级结束\n");
            if(State_End(fd) > 0)
            {
                RADIO_FMT_DEBUG("接收到数据，进入下一状态\n");
                MState=M_NEXT;
            }
            else
            {
                if(aTime++ > 10)
                {
                    RADIO_FMT_DEBUG("等待数据次数过多，进入下一状态\n");
                    MState=M_NEXT;
                }
            }
            break;
        case M_NEXT:
            RADIO_FMT_DEBUG("传输下一文件\n");
            if(State_NEXT(fd) > 0)
            {
                MState=M_NEWVER;
                aTime = 0;
            }
            else
            {
                if(aTime++ > 10)
                {
                    MState=M_FAIL;
                }
            }
            break;

        case M_NEWVER:
            RADIO_FMT_DEBUG("设置波特率为19200，查询新版本\n");
            if(Update_Version(fd,19200,1) > 0)
            {
                RADIO_FMT_DEBUG("执行完，删除升级的文件\n");
                //执行完，删除升级的文件
                system("rm -rf /data/app/radioManager/*.cry");
                RADIO_FMT_DEBUG("升级成功，重启终端\n");
                restart_model_radio();
                start_model_radio();
                return;
            }
            else
            {
                MState=M_FAIL;
            }
            break;
        case M_FAIL:
            {
                
                RADIO_FMT_DEBUG("retry_count = %d\n", retry_count);
                //将升级文件移至临时文件夹以便于分析升级失败原因
                system("mkdir -p /data/app/radioManager/radio_update");
                system("mv -rf /data/app/radioManager/*.cry /data/app/radioManager/radio_update");
                system("rm -rf /data/app/radioManager/*.cry");
                RADIO_FMT_DEBUG("升级失败\n");
                return;
            }
            break;

        default:
            MState=M_OLDVER;
            break;
        }
        usleep(500000);
    }
}

void update_modem_start(int fd)
{
    char sbuf[128] = {0};
    sprintf(sbuf,"/data/app/radioManager/radio_update.cry");
    if (access(sbuf, R_OK) == 0)
    {
        update_modem(fd);
        Get_Version(fd,0);
    }
}
uint32 g_RadioSpeakerFlag = 0xFF;  /* 扬声器状态 1:关闭，2:打开 */
time_t g_RadioOpenTime;     //通话打开的时间 最长600秒 
extern uint8  g_RadioSpeakerCtl;

/**********************************************************************
* @name      : radio_speak_manager
* @brief     ：设置扬声器状态处理
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/

void radio_speak_manager(int fd)
{
    WriteDTComm(radio_state);

    if(g_RadioSpeakerCtl != 0)
    {
        if(g_RadioSpeakerCtl == 2)
        {
            RADIO_FMT_DEBUG("设置扬声器打开radio_state 0x%02x\n",radio_state);
            if (OpenSpeak(&radio_state) == 1)
            {
                RADIO_FMT_DEBUG("设置扬声器打开成功radio_state 0x%02x\n",radio_state);
                g_RadioSpeakerFlag = g_RadioSpeakerCtl;
                g_RadioOpenTime = time(NULL);
            }
        }else if(g_RadioSpeakerCtl == 1)
        {
            RADIO_FMT_DEBUG("设置扬声器关闭radio_state 0x%02x\n",radio_state);
            if (CloseSpeak(&radio_state) == 1)
            {
                RADIO_FMT_DEBUG("设置扬声器关闭成功radio_state 0x%02x\n",radio_state);
                g_RadioSpeakerFlag = g_RadioSpeakerCtl;
                g_RadioOpenTime = 0;
            }
        }
        g_RadioSpeakerCtl = 0;
    }
    //判断下是否要关闭通话
    if(g_RadioOpenTime!=0)
    {
        if(abs(time(NULL) - g_RadioOpenTime) > 600)
        {
            RADIO_FMT_DEBUG("关闭扬声器\n");
            if (CloseSpeak(&radio_state) == 1)
            {
                RADIO_FMT_DEBUG("设置扬声器关闭成功\n");
                g_RadioSpeakerFlag = 1;
                g_RadioOpenTime = 0;
            }
        }
    }
}
