/*
*********************************************************************
* Copyright(C) 2021 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   蓝牙APP 设备操作
* @date：    2021-7-10
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_split.h"
#include "framecomm.h"
#include "db_struct.h"
#include "ble_data.h"
#include "ble_mqtt.h"
#include "ble_uart.h"
#include "ble_protocol.h"
#if PIN_ENABLE == 1
#include "OopAll.h"
#endif

#if DESC("蓝牙状态刷新", 1)
/**********************************************************************
* @name      : 蓝牙连接状态刷新
* @brief     ：蓝牙模块主动上报连接信息处理
* @param[in] ：BLE_DATA_T *pBledata    蓝牙数据
               BOOL        bInCnt      是否在连接过程中
* @param[out]：
* @return    ：
**********************************************************************/
void bel_refresh_status(BLE_DATA_T *pBledata, BOOL bInCnt)
{
    uint8      i = 0;
    BOOL       bConnect = FALSE;
    BLE_DEV_T *pData = &pBledata->tDevStatus;

    /* 主判断 */
    for(i = 0; i < BT_MASTER_MAX; i++)
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
        for(i = 0; i < BT_SLAVE_MAX; i++)
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

#if DESC("综合操作", 1)

/**********************************************************************
* @name      : ble_send_msgevent
* @brief     ：对外消息发送接收消息
* @param[in] ：BLE_MSG_EVENT_T *ptMsgEvent  对外发送的消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_send_msgevent(BLE_MSG_EVENT_T *ptMsgEvent)
{
    uint8           buf[4600] = {0};
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf;
    uint8          *temp = ptAXDR->playload;
    char           *strmsg = NULL;
    int             ret = 0;
    char            topic[256] = {0};
    
    if(NULL == ptMsgEvent)
    {
        return -1;
    }
    
    /* 构造返回消息 */
    ptAXDR->rpm = 1;
    ptAXDR->priority = 0;
    ptAXDR->index = ble_get_next_index();
    ptAXDR->label = 0;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    ptAXDR->destlen = strlen("Broadcast");
    memcpy(ptAXDR->destaddr, "Broadcast", ptAXDR->destlen);


    ptAXDR->IID = MESSAGE_IID_BLE;
    ptAXDR->IOP = BLE_IOP_RECV_EVENT;

    ptAXDR->msglen = ble_msg_event_package(ptMsgEvent, temp, 4200);
    
    #ifdef PRODUCT_SCU
    ret = ble_send_axdr_msg(ptAXDR, 0);
    #else
    ret = ble_send_axdr_msg_broadcast(ptAXDR, 0, MESSAGE_IID_BLE, BLE_IOP_RECV_EVENT);
    #endif

    strmsg = ble_msg_notify_package(ptAXDR->index, ptMsgEvent);
    if(strmsg)
    {
        sprintf(topic, "%s/Broadcast/%s/%s/%s/%s", BT_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_REPORT,
                                                   MQTT_TOPIC_INFO_RES, BLE_TOPOC_RECVNOTIFY);
        ret |= ble_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }
    return ret;
}

/**********************************************************************
* @name      : ble_devport_report
* @brief     ：端口上送消息
* @param[in] ：BLE_DEV_PORT_T *pDevPort     端口
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-9-15
* @Update    :
**********************************************************************/
void ble_devport_report(BLE_DEV_PORT_T *pDevPort)
{
    int              ret = 0;
    
    if(NULL == pDevPort)
    {
        return;
    }

    if(NULL == pDevPort->pTranMsg)
    {
        /* 上报通知 */
        BLE_MSG_EVENT_T tEvent;

        memset(&tEvent, 0, sizeof(BLE_MSG_EVENT_T));
        if(0 == pDevPort->cachelen)
        {
            return;
        }
        tEvent.port = pDevPort->port;
        memcpy(tEvent.MAC, pDevPort->MAC, 6);
        tEvent.len = pDevPort->cachelen;
        memcpy(tEvent.msg, pDevPort->cache, pDevPort->cachelen);
        ret = ble_send_msgevent(&tEvent);
        if(0 != ret)
        {
            BLE_BUF_DEBUG(tEvent.msg, tEvent.len, "收到消息上送失败 %02X:%02X:%02X:%02X:%02X:%02X\n", 
                          pDevPort->MAC[0], pDevPort->MAC[1], pDevPort->MAC[2], pDevPort->MAC[3], 
                          pDevPort->MAC[4], pDevPort->MAC[5]);
        }
        else
        {
            MQTTMSG_BUF_TRACE(tEvent.msg, tEvent.len, "消息上送 %02X:%02X:%02X:%02X:%02X:%02X\n", 
                              pDevPort->MAC[0], pDevPort->MAC[1], pDevPort->MAC[2], pDevPort->MAC[3], 
                              pDevPort->MAC[4], pDevPort->MAC[5]);
        }
        
        memset(pDevPort->cache, 0, pDevPort->cachelen);
        pDevPort->cachelen = 0;
    }
    else
    {
        /* 转发消息 */
    }
}

/**********************************************************************
* @name      : ble_send_msgevent_json
* @brief     ：对外消息发送接收消息
* @param[in] ：BLE_MSG_EVENT_T *ptMsgEvent  对外发送的消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_send_portevent(BLE_PORT_EVENT_T *ptPortEvent)
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
    ptAXDR->index = ble_get_next_index();
    ptAXDR->label = 0;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    ptAXDR->destlen = strlen("Broadcast");
    memcpy(ptAXDR->destaddr, "Broadcast", ptAXDR->destlen);


    ptAXDR->IID = MESSAGE_IID_BLE;
    ptAXDR->IOP = BLE_IOP_PORT_EVENT;

    ptAXDR->msglen = ble_port_event_package(ptPortEvent, temp, 256);
    
    #ifdef PRODUCT_SCU
    ret = ble_send_axdr_msg(ptAXDR, 0);
    #else
    ret = ble_send_axdr_msg_broadcast(ptAXDR, 0, MESSAGE_IID_BLE, BLE_IOP_RECV_EVENT);
    #endif

    strmsg = ble_port_notify_package(ptAXDR->index, ptPortEvent);
    if(strmsg)
    {
        sprintf(topic, "%s/Broadcast/%s/%s/%s/%s", BT_APP_NAME, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_REPORT,
                                                   MQTT_TOPIC_INFO_RES, BLE_TOPOC_PORTNOTIFY);
        ret |= ble_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }

    BLE_FMT_DEBUG("端口%d, MAC %02X:%02X:%02X:%02X:%02X:%02X, %s\n", ptPortEvent->port, ptPortEvent->MAC[0], 
                   ptPortEvent->MAC[1], ptPortEvent->MAC[2], ptPortEvent->MAC[3], ptPortEvent->MAC[4], 
                   ptPortEvent->MAC[5], ptPortEvent->bConnect ? "连接" : "断开");
    return ret;

}

/**********************************************************************
* @name      : ble_dev_msgreport_proc
* @brief     ：蓝牙模块接收消息上送
* @param[in] ：BLE_PRODATA_T *pProData 消息解析
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_dev_msgreport_proc(int fd, BLE_PRODATA_T *pProData, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    uint16     i = 0;
    uint32     port = BLE_PORT_MAX;
    uint8      newdata = 0;
    uint8     *temp = pProData->data;
    int        ret = 0;
    BLE_DEV_T *pDevStatus = NULL;
    struct timespec curtime = {0};
    BLE_MSG_EVENT_T tMsgEvent;

    BLE_PRODATA_T  *pProRet = NULL;

    //蓝牙检定时的特殊MAC地址
    uint8 MAC[6] = {0xee, 0xee, 0xee, 0xee, 0xee, 0xee};
    
    if(NULL == pProData || NULL == pBleHandle || NULL == pBledata)
    {
        return -1;
    }

    memset(&tMsgEvent, 0, sizeof(tMsgEvent));

    clock_gettime(CLOCK_MONOTONIC, &curtime);
    pDevStatus = &pBledata->tDevStatus;

    if(pProData->datalen > 4 && 0xFE == temp[0] && 0xFE == temp[1] && 0xFE == temp[2] && 0xFE == temp[3])
    {
        newdata = TRUE;
    }

    //蓝牙检定时收到的MAC地址切换到全EE,需要建立虚拟通道
    if((0 == memcmp(MAC, pProData->mac, 6)) && 
       (0 != memcmp(MAC, pDevStatus->mPortStatus[0].MAC, 6)))
    {
        /* 申请内存 */
        pProRet = (BLE_PRODATA_T*)malloc(7 + sizeof(BLE_PRODATA_T));
        if(NULL == pProRet)
        {
            return -3;
        }
        memset(pProRet, 0, 7 + sizeof(BLE_PRODATA_T));
        pProRet->datalen = 7;

        pProRet->data[0] = 0x01;
        memcpy(&pProRet->data[1], MAC, 6);

        ble_dev_report_conn_proc(0, pProRet, pBleHandle, pBledata);

        free(pProRet);
    }

    /* 查询端口 */
    for(i = 0; i < BT_MASTER_MAX; i++)
    {
        if(0 == memcmp(pDevStatus->mPortStatus[i].MAC, pProData->mac, 6))
        {
            port = pDevStatus->mPortStatus[i].port;
            pDevStatus->mPortStatus[i].lasttime = curtime.tv_sec;
            pDevStatus->mPortStatus[i].lastus = curtime.tv_nsec / 1000;
            if(pDevStatus->mPortStatus[i].cachelen + pProData->datalen > BLE_MSG_MAX_LEN || TRUE == newdata)
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
    if(port >= BLE_PORT_MAX)
    {
        for(i = 0; i < BT_SLAVE_MAX; i++)
        {
            if(0 == memcmp(pDevStatus->sPortStatus[i].MAC, pProData->mac, 6))
            {
                port = pDevStatus->sPortStatus[i].port;
                pDevStatus->sPortStatus[i].lasttime = curtime.tv_sec;
                pDevStatus->sPortStatus[i].lastus = curtime.tv_nsec / 1000;
                if(pDevStatus->sPortStatus[i].cachelen + pProData->datalen > BLE_MSG_MAX_LEN || TRUE == newdata)
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
    
    if(port >= BLE_PORT_MAX)
    {
        BLE_FMT_DEBUG("查询不到端口 %02X:%02X:%02X:%02X:%02X:%02X\n", pProData->mac[0], pProData->mac[1],
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
    ret = ble_send_msgevent(&tMsgEvent);
    if(0 != ret)
    {
        BLE_BUF_DEBUG(tMsgEvent.msg, tMsgEvent.len, "收到消息上送失败 %02X:%02X:%02X:%02X:%02X:%02X\n", 
                      pProData->mac[0], pProData->mac[1], pProData->mac[2], pProData->mac[3], 
                      pProData->mac[4], pProData->mac[5]);
    }

    return ret;
}

/**********************************************************************
* @name      : ble_dev_msgreport_later
* @brief     ：蓝牙模块接收消息上送后续处理 串口一次接收的数据认为是一组报文
* @param[in] ：BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-2-16
* @Update    :
**********************************************************************/
void ble_dev_msgreport_later(BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    uint16     i = 0;
    int        ret = 0;
    BLE_DEV_T *pDevStatus = NULL;
    BLE_MSG_EVENT_T tMsgEvent;
    
    if(NULL == pBleHandle || NULL == pBledata)
    {
        return;
    }

    memset(&tMsgEvent, 0, sizeof(tMsgEvent));

    pDevStatus = &pBledata->tDevStatus;
    /* 查询端口 */
    for(i = 0; i < BT_MASTER_MAX; i++)
    {
        /* 发送数据 */
        if(pDevStatus->mPortStatus[i].cachelen > 0)
        {
            tMsgEvent.port = pDevStatus->mPortStatus[i].port;
            memcpy(tMsgEvent.MAC, pDevStatus->mPortStatus[i].MAC, 6);
            tMsgEvent.len = pDevStatus->mPortStatus[i].cachelen;
            memcpy(tMsgEvent.msg, pDevStatus->mPortStatus[i].cache, tMsgEvent.len);

            /* 发送通知消息 */
            ret = ble_send_msgevent(&tMsgEvent);
            if(0 != ret)
            {
                BLE_BUF_DEBUG(tMsgEvent.msg, tMsgEvent.len, "收到消息上送失败[%d] %02X:%02X:%02X:%02X:%02X:%02X\n", ret, 
                                               tMsgEvent.MAC[0], tMsgEvent.MAC[1], tMsgEvent.MAC[2], tMsgEvent.MAC[3], 
                                               tMsgEvent.MAC[4], tMsgEvent.MAC[5]);
            }
    
            memset(pDevStatus->mPortStatus[i].cache, 0, pDevStatus->mPortStatus[i].cachelen);
            pDevStatus->mPortStatus[i].cachelen = 0;
        }
    }

    for(i = 0; i < BT_SLAVE_MAX; i++)
    {
        /* 发送数据 */
        if(pDevStatus->sPortStatus[i].cachelen > 0)
        {
            tMsgEvent.port = pDevStatus->sPortStatus[i].port;
            memcpy(tMsgEvent.MAC, pDevStatus->sPortStatus[i].MAC, 6);
            tMsgEvent.len = pDevStatus->sPortStatus[i].cachelen;
            memcpy(tMsgEvent.msg, pDevStatus->sPortStatus[i].cache, tMsgEvent.len);

            /* 发送通知消息 */
            ret = ble_send_msgevent(&tMsgEvent);
            if(0 != ret)
            {
                BLE_BUF_DEBUG(tMsgEvent.msg, tMsgEvent.len, "收到消息上送失败 %02X:%02X:%02X:%02X:%02X:%02X\n", 
                                               tMsgEvent.MAC[0], tMsgEvent.MAC[1], tMsgEvent.MAC[2], tMsgEvent.MAC[3], 
                                               tMsgEvent.MAC[4], tMsgEvent.MAC[5]);
            }
            
            memset(pDevStatus->sPortStatus[i].cache, 0, pDevStatus->sPortStatus[i].cachelen);
            pDevStatus->sPortStatus[i].cachelen = 0;
        }
    }
}

/**********************************************************************
* @name      : ble_dev_report_info_proc
* @brief     ：蓝牙模块主动上报设备状态处理
* @param[in] ：int fd                  设备句柄
               BLE_PRODATA_T *pProData 消息解析
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-16
* @Update    :
**********************************************************************/
int ble_dev_report_info_proc(int fd, BLE_PRODATA_T *pProData, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    uint8     *temp = pProData->data;
    char      *modelstr[] = {"模块复位", "恢复出厂", "进入睡眠", "进入睡眠"};
    uint8      sendbuf[2] = {0};
    
    if(*temp < BLE_MODEL_RESTART || *temp > BLE_MODEL_WAKE)
    {
        BLE_FMT_DEBUG("未知模块信息%d\n", *temp);
        return -1;
    }

    BLE_FMT_DEBUG("模块上报新状态: %s\n", modelstr[*temp - 1]);

    pBledata->tDevStatus.bSleep = BLE_MODEL_SLEEP == *temp ? TRUE : FALSE;

    //模块复位后，需要重新设置参数
    if (*temp == 1)
    {
        pBleHandle->status = BLE_DEV_STATUS_PREPARE;
    }
    
    /* 应答 */
    sendbuf[0] = *temp;
    return ble_report_response(fd, CMD_MODULE_INFO, sendbuf, 2);
}

/**********************************************************************
* @name      : ble_dev_report_conn_proc
* @brief     ：蓝牙模块主动上报连接信息处理
* @param[in] ：int fd                  设备句柄
               BLE_PRODATA_T *pProData 消息解析
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_dev_report_conn_proc(int fd, BLE_PRODATA_T *pProData, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    BLE_DEV_T *pData = &pBledata->tDevStatus;
    uint8     *temp = pProData->data;
    uint8      sendbuf[1] = {0};
    uint8      status;
    uint8      MAC[6] = {0};
    uint8      MACNO[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    BLE_PORT_EVENT_T tEvent;
    struct timespec curtime = {0};
    uint8      i = 0;
    uint8      j = 0;
    
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    memset(&tEvent, 0, sizeof(BLE_PORT_EVENT_T));
    status = *temp;
    temp++;

    /* 主判断 */
    for(i = 0; i < BT_MASTER_MAX; i++)
    {
        memcpy(tEvent.MAC, MACNO, 6);
        tEvent.port = i;
        tEvent.mode = BLE_MODE_MASTER;

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
            pData->PortCfg[i].porttCfg.mode = BLE_MODE_MASTER;
        }
        else if(pData->mPortStatus[i].bConnect)
        {
            memcpy(tEvent.MAC, pData->mPortStatus[i].MAC, 6);
            tEvent.bConnect = FALSE;
            
            memset(pData->mPortStatus[i].MAC, 0xFF, 6);
            pData->mPortStatus[i].bConnect = FALSE;
            
            pData->PortCfg[i].bConnect = FALSE;
            memcpy(pData->PortCfg[i].porttCfg.MAC, pData->mPortStatus[i].MAC, 6);
            pData->PortCfg[i].porttCfg.mode = BLE_MODE_MASTER;
        }
        pData->PortCfg[i].channelid = i + 1;

        if(0 != memcmp(tEvent.MAC, MACNO, 6))
        {
            pData->mPortStatus[i].cachelen = 0;
            memset(pData->mPortStatus[i].cache, 0, BLE_MSG_MAX_LEN);
            ble_send_portevent(&tEvent);
        }
    }

    /* 从判断 */
    for(i = 0; i < BT_SLAVE_MAX; i++)
    {
        memcpy(tEvent.MAC, MACNO, 6);
        tEvent.mode = BLE_MODE_SLAVE;

        if((status >> (i + 2 )) & 0x01)
        {
            memcpy(MAC, temp, 6);
            temp += 6;
            if(0 != memcmp(MAC, pData->sPortStatus[i].MAC, 6) || pData->sPortStatus[i].port > BLE_PORT_MAX)
            {
                memcpy(pData->sPortStatus[i].MAC, MAC, 6);
                /* 端口查询 */
                if(pData->sPortStatus[i].port > BLE_PORT_MAX)
                {
                    for(j = 2; j < BLE_PORT_MAX; j++)
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
                
                if(pData->sPortStatus[i].port > BLE_PORT_MAX)
                {
                    BLE_FMT_DEBUG("查询不到端口: 从%d, MAC %02X:%02X:%02X:%02X:%02X:%02X\n", i + 1,
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
            
            if(pData->sPortStatus[i].port < BLE_PORT_MAX)
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
            if(pData->sPortStatus[i].port < BLE_PORT_MAX)
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

        if(0 != memcmp(tEvent.MAC, MACNO, 6) && tEvent.port < BLE_PORT_MAX)
        {
            pData->sPortStatus[i].cachelen = 0;
            memset(pData->sPortStatus[i].cache, 0, BLE_MSG_MAX_LEN);
            ble_send_portevent(&tEvent);
        }
    }

    //刷新桌面蓝牙状态显示
    bel_refresh_status(pBledata, FALSE);

    /* 应答 */
    if(fd <= 0)
    {
        return 0;
    }
    return ble_report_response(fd, CMD_BLE_CONN_INFO, sendbuf, 1);
}

/**********************************************************************
* @name      : ble_set_param_proc
* @brief     ：设置蓝牙基本参数
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_CONFIG_T *ptParam   参数
               uint8 bFirst            是否首次执行
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_set_param_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_CONFIG_T *ptParam, uint8 bFirst)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint16 temp = 0;
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T   *pProRet = NULL;
    
    /* 1. 设置地址 */
    if(TRUE == bFirst)
    {
        memcpy(sendbuf, ptParam->MAC, 6);
        ret = ble_set_operate(fd, CMD_DEV_MAC_ADDR, sendbuf, 24, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                                               BLE_RECV_BUF_LEN - pBleHandle->recvlen);
        if(ret < 0)
        {
            return ret;
        }
        pBleHandle->recvlen += ret;
    }
    else
    {
        sendbuf[0] = 1;
        MEMCPY_R(sendbuf + 1, ptParam->MAC, 6);
        ret = ble_set_operate(fd, CMD_DEV_ADDR, sendbuf, 7, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                              BLE_RECV_BUF_LEN - pBleHandle->recvlen);
        if(ret < 0)
        {
            return ret;
        }
        pBleHandle->recvlen += ret;
    }
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙地址超时 %02X:%02X:%02X:%02X:%02X:%02X\n", ptParam->MAC[0], ptParam->MAC[1],
                      ptParam->MAC[2], ptParam->MAC[3], ptParam->MAC[4], ptParam->MAC[5]);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙地址成功 %02X:%02X:%02X:%02X:%02X:%02X\n", ptParam->MAC[0], ptParam->MAC[1],
                          ptParam->MAC[2], ptParam->MAC[3], ptParam->MAC[4], ptParam->MAC[5]);
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙地址失败(%d) %02X:%02X:%02X:%02X:%02X:%02X\n", status, ptParam->MAC[0], 
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
    ret = ble_set_operate(fd, CMD_BLE_PARAMS, sendbuf, 5, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙工作参数超时, 功率%d, 广播间隔%dms, 扫描间隔%dms\n", ptParam->power, ptParam->broadcast,
                                                                                    ptParam->scan);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙工作参数成功, 功率%d, 广播间隔%dms, 扫描间隔%dms\n", ptParam->power, ptParam->broadcast,
                                                                                    ptParam->scan);
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙工作参数失败(%d) 功率%d, 广播间隔%dms, 扫描间隔%dms\n", status, ptParam->power, 
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
    ret = ble_set_operate(fd, CMD_DEV_NAME, sendbuf, 5, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙名称%s超时\n", sendbuf);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙名称%s成功\n", sendbuf);
        }
        else
        {
            /* 蓝牙名称不支持设置 */
            BLE_FMT_DEBUG("设置蓝牙名称%s失败(%d)\n", sendbuf, status);
            return BLE_SUCCESS;
        }
    }

    return 0;
}

/**********************************************************************
* @name      : ble_set_checkmeter_proc
* @brief     ：设置蓝牙检定参数
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_CONFIG_T *ptParam   参数
               uint8 bFirst            是否首次执行
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_set_checkmeter_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata, BLE_CHECK_T *ptParam)
{
    int    ret = 0;
    int    i = 0;
    uint8  sendbuf[24] = {0};
    uint16 offset = 0;
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T   *pProRet = NULL;

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
    for (i = 0; i < MIN(ptParam->ch_num, BLE_CHANNEL_MAX); i++)
    {
        memcpy(&sendbuf[offset], &ptParam->freq[i], sizeof(uint16));
        offset += 2;
    }
    
    ret = ble_set_operate(fd, CMD_PD, sendbuf, offset, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙检定参数超时, 功率%d, 广播间隔%dms, 类型%d\n", ptParam->power, ptParam->interval, ptParam->type);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙检定参数成功, 功率%d, 广播间隔%dms, 类型%d\n", ptParam->power, ptParam->interval, ptParam->type);
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙检定参数失败, 错误码[%d], 功率%d, 广播间隔%dms, 类型%d\n", status, ptParam->power, ptParam->interval, 
                ptParam->type);
            return status;
        }
    }

    //如果退出检定，则退出虚拟连接
    if (ptParam->type == 0xff)
    {
        /* 申请内存 */
        pProRet = (BLE_PRODATA_T*)malloc(7 + sizeof(BLE_PRODATA_T));
        if(NULL == pProRet)
        {
            return BLE_ERR_OPER_CHECK;
        }
        memset(pProRet, 0, 7 + sizeof(BLE_PRODATA_T));
        pProRet->datalen = 7;

        ble_dev_report_conn_proc(0, pProRet, pBleHandle, pBledata);

        free(pProRet);
    }

    return 0;
}

/**********************************************************************
* @name      : ble_get_param_proc
* @brief     ：获取蓝牙基本参数
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
* @param[out]：BLE_CONFIG_T *ptParam   参数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_get_param_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_CONFIG_T *ptParam)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint16 temp = 0;
    BLE_PRODATA_T *pProRet = NULL;

    memset(ptParam, 0, sizeof(BLE_CONFIG_T));
    /* 1. 读取蓝牙地址 */
    sendbuf[0] = 1;
    ret = ble_get_operate(fd, CMD_DEV_ADDR, sendbuf, 1, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(pProRet)
    {
        if(7 == pProRet->datalen && 1 == pProRet->data[0])
        {
            MEMCPY_R(ptParam->MAC, pProRet->data + 1, 6);
            BLE_FMT_DEBUG("蓝牙地址%02X:%02X:%02X:%02X:%02X:%02X\n", ptParam->MAC[0], ptParam->MAC[1],
                          ptParam->MAC[2], ptParam->MAC[3], ptParam->MAC[4], ptParam->MAC[5]);
        }
        free(pProRet);
    }

    /* 2. 蓝牙名称 */
    ret = ble_get_operate(fd, CMD_DEV_NAME, NULL, 0, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(pProRet)
    {
        if(0 != pProRet->datalen)
        {
            ptParam->len = MIN(5, pProRet->datalen);
            memcpy(ptParam->name, pProRet->data, ptParam->len);
            BLE_FMT_DEBUG("蓝牙名称%s\n", ptParam->name);
        }
        free(pProRet);
    }

    /* 3. 工作参数 */
    ret = ble_get_operate(fd, CMD_BLE_PARAMS, NULL, 0, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(pProRet)
    {
        if(5 == pProRet->datalen)
        {
            ptParam->power = pProRet->data[0];
            MEMCPY_R(&temp, pProRet->data + 1, 2);
            ptParam->broadcast = temp * 0.625;
            MEMCPY_R(&temp, pProRet->data + 3, 2);
            ptParam->scan = temp * 0.625;
            BLE_FMT_DEBUG("蓝牙功率%d, 广播间隔%dms, 扫描间隔%dms\n", ptParam->power, ptParam->broadcast, ptParam->scan);
        }
        free(pProRet);
    }

    return 0;
}

/**********************************************************************
* @name      : ble_set_base_match
* @brief     ：设置蓝牙本机配对参数
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_MATCH_T *ptMatch    配对参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-2-17
* @Update    :
**********************************************************************/
int ble_set_base_match(int fd, BLE_DEV_H_T *pBleHandle, BLE_MATCH_T *ptMatch)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};
    
    sendbuf[0] = 1;
    sendbuf[1] = ptMatch->mode;
    sendbuf[2] = MIN(16, ptMatch->len);
    memcpy(sendbuf + 3, ptMatch->pwd, MIN(16, ptMatch->len));
    ret = ble_set_operate(fd, CMD_DEV_PAIR_MODE, sendbuf, 3 + MIN(16, ptMatch->len), &pProRet, 
                          pBleHandle->recv + pBleHandle->recvlen, BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙(%s)配对参数超时, 模式%d, 密码%s\n", devName[0], ptMatch->mode, ptMatch->pwd);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙(%s)配对参数成功, 模式%d, 密码%s\n", devName[0], ptMatch->mode, ptMatch->pwd);
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙(%s)配对参数失败(%d) 模式%d, 密码%s\n", devName[0], status,
                               ptMatch->mode, ptMatch->pwd);
            return status;
        }
    }
    return BLE_SUCCESS;
}

/**********************************************************************
* @name      : ble_set_slave_match
* @brief     ：设置蓝牙从机配对参数
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_MATCH_T *ptMatch    配对参数
               uint8 NO                序号(1-3从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_set_slave_match(int fd, BLE_DEV_H_T *pBleHandle, BLE_MATCH_T *ptMatch, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};
    
    if(NO > 3 || 0 == NO)
    {
        return -1;
    }
    
    sendbuf[0] = (0x01 << NO);
    sendbuf[1] = ptMatch->mode;
    sendbuf[2] = MIN(16, ptMatch->len);
    memcpy(sendbuf + 3, ptMatch->pwd, MIN(16, ptMatch->len));
    ret = ble_set_operate(fd, CMD_DEV_PAIR_MODE, sendbuf, 3 + MIN(16, ptMatch->len), &pProRet, 
                          pBleHandle->recv + pBleHandle->recvlen, BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙(%s)配对参数超时, 模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙(%s)配对参数成功, 模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙(%s)配对参数失败(%d) 模式%d, 密码%s\n", devName[NO], status,
                               ptMatch->mode, ptMatch->pwd);
            return status;
        }
    }
    return BLE_SUCCESS;
}

/**********************************************************************
* @name      : ble_set_match_proc
* @brief     ：设置蓝牙配对参数
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_MATCH_T *ptMatch    配对参数
               uint8 NO                序号(0-本机, 1-3从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_set_match_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_MATCH_T *ptMatch, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[24] = {0};
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};

    if(NO > 3)
    {
        return -1;
    }
    if(NO > 0)
    {
        BLE_FMT_DEBUG("蓝牙(%s)配对参数不设置，使用默认方式连接电表\n", devName[NO]);
        return 0;
    }
    
    sendbuf[0] = (0x01 << NO);
    sendbuf[1] = ptMatch->mode;
    sendbuf[2] = MIN(16, ptMatch->len);
    memcpy(sendbuf + 3, ptMatch->pwd, MIN(16, ptMatch->len));
    ret = ble_set_operate(fd, CMD_DEV_PAIR_MODE, sendbuf, 3 + MIN(16, ptMatch->len), &pProRet, 
                          pBleHandle->recv + pBleHandle->recvlen, BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙(%s)配对参数超时, 模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙(%s)配对参数成功, 模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙(%s)配对参数失败(%d) 模式%d, 密码%s\n", devName[NO], status,
                               ptMatch->mode, ptMatch->pwd);
            return status;
        }
    }
    return BLE_SUCCESS;
}

/**********************************************************************
* @name      : ble_get_match_proc
* @brief     ：查询蓝牙配对参数
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               uint8 NO                序号(0-本机, 1-3从设备)
* @param[out]：BLE_MATCH_T *ptMatch    配对参数
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_get_match_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_MATCH_T *ptMatch, uint8 NO)
{
    uint8  sendbuf[24] = {0};
    BLE_PRODATA_T *pProRet = NULL;
    char          *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};
    int            ret = 0;
    
    if(NO > 3)
    {
        return -1;
    }
    
    sendbuf[0] = (0x01 << NO);
    ret = ble_get_operate(fd, CMD_DEV_PAIR_MODE, sendbuf, 1, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
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
            BLE_FMT_DEBUG("蓝牙(%s)配对参数模式%d, 密码%s\n", devName[NO], ptMatch->mode, ptMatch->pwd);
        }
        free(pProRet);
    }
    return 0;
}

/**********************************************************************
* @name      : ble_get_baseinfo_proc
* @brief     ：查询蓝牙基本信息
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
void ble_get_baseinfo_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    uint8           recv[28] = {0};
    BLE_PRODATA_T  *pProRet = NULL;
    CMD_VERSION_T  *pVersion = NULL;
    uint16          temp = 0;
    uint32          temp32 = 0;
    int             ret = 0;

    //默认sn
    uint8 i =0;
    uint8 defSn[20] =  {0x48, 0xa5, 0xee, 0x8a, 0x71, 0x83, 0xc3, 0xf0, 0x43, 0x4c, 0x75, 0xfd, 0x91, 
                        0x2e, 0x0b, 0x7b, 0xe2, 0xd0, 0x7d, 0xf5};
    
    ret = ble_get_operate(fd, CMD_FIRMWARE_VERSION, NULL, 0, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return;
    }
    pBleHandle->recvlen += ret;
    if(pProRet)
    {
        if(pProRet->datalen == sizeof(CMD_VERSION_T))
        {
            pVersion = (CMD_VERSION_T*)pProRet->data;
            MEMCPY_R(&temp, pVersion->app_version + 2, 2);
            MEMCPY_R(&temp32, pVersion->git_version, 4);
            BLE_FMT_DEBUG("蓝牙芯片型号: %02X, APP版本: %d.%d.%d.%d, GIT版本: %08X\n", pVersion->ic_type,
                           pVersion->app_version[0], pVersion->app_version[1], temp, pVersion->app_version[4], temp32);
        }
        free(pProRet);
    }

    ret = ble_get_operate(fd, CMD_FIRMWARE_BUILD_TIME, NULL, 0, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return;
    }
    pBleHandle->recvlen += ret;
    if(pProRet)
    {
        if(pProRet->datalen == 24)
        {
            memcpy(recv, pProRet->data, 24);
            BLE_FMT_DEBUG("固件编译时间: %s\n", recv);
        }
        free(pProRet);
    }

    ret = ble_get_operate(fd, CMD_MODULE_SN, NULL, 0, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return;
    }
    pBleHandle->recvlen += ret;

    if(pProRet)
    {
        BLE_BUF_DEBUG(pProRet->data, pProRet->datalen, "模组SN,");

        for (i = 0; i < pProRet->datalen; i++)
        {
            if (pProRet->data[i] != 0xff)
            {
                break;;
            }
        }

        if (i != pProRet->datalen)
        {
            pBledata->tDevStatus.sn.num = pProRet->datalen;
            memcpy(pBledata->tDevStatus.sn.sn, pProRet->data, pProRet->datalen);
        }
        else
        {
            BLE_BUF_DEBUG(defSn, 20, "无效sn, 使用默认sn");
            pBledata->tDevStatus.sn.num = 20;
            memcpy(pBledata->tDevStatus.sn.sn, defSn, 20);
        }

        free(pProRet);
    }
}

/**********************************************************************
* @name      : ble_set_link_mode
* @brief     ：设置蓝牙链路数据接收模式
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-9-14
* @Update    :
**********************************************************************/
void ble_set_link_mode(int fd, BLE_DEV_H_T *pBleHandle)
{
    uint8           sendbuf[1] = {2};
    BLE_PRODATA_T  *pProRet = NULL;
    int             ret = 0;
    
    ret = ble_set_operate(fd, CMD_BLE_TRANS_MODE, sendbuf, 1, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        BLE_FMT_DEBUG("设置蓝牙链路数据接收模式失败\n");
        return;
    }
    pBleHandle->recvlen += ret;
    if(pProRet)
    {
        BLE_FMT_DEBUG("蓝牙链路数据接收模式设置%s\n", (pProRet->datalen == 1 && 0 == pProRet->data[0]) ? "成功" : "失败");
        free(pProRet);
    }
}

/**********************************************************************
* @name      : ble_set_mac_proc
* @brief     ：设置设备地址
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               uint8 *pMAC             地址
               uint8 NO                序号(0-本机, 1-3从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_set_mac_proc(int fd, BLE_DEV_H_T *pBleHandle, uint8 *pMAC, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[8] = {0};
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机", "从设备1", "从设备2", "从设备3"};
    
    if(NO > 3)
    {
        return -1;
    }
    
    sendbuf[0] = (0x01 << NO);
    MEMCPY_R(sendbuf + 1, pMAC, 6);
    ret = ble_set_operate(fd, CMD_DEV_ADDR, sendbuf, 7, &pProRet, 
                          pBleHandle->recv + pBleHandle->recvlen, BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙(%s)地址超时 %02X:%02X:%02X:%02X:%02X:%02X\n", devName[NO], pMAC[0], pMAC[1],
                      pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙(%s)地址成功 %02X:%02X:%02X:%02X:%02X:%02X\n", devName[NO], pMAC[0], pMAC[1],
                          pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙(%s)地址失败(%d) %02X:%02X:%02X:%02X:%02X:%02X\n", devName[NO], status,
                           pMAC[0], pMAC[1], pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
            return status;
        }
    }
    return BLE_SUCCESS;
}

/**********************************************************************
* @name      : ble_set_mac_proc
* @brief     ：设置设备地址
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               uint8 operat            方式(0-断开, 1-连接)
               uint8 *pMAC             地址
               uint8 NO                序号(0-1本机, 2-4从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_conn_disconn_mac(int fd, BLE_DEV_H_T *pBleHandle, uint8 operat, uint8 *pMAC, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[8] = {0};
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T *pProRet = NULL;
    char   *devName[] = {"本机1", "本机2", "从设备1", "从设备2", "从设备3"};
    
    if(NO > 4 || operat > 1)
    {
        return -1;
    }
    
    sendbuf[0] = (0x01 << NO);
    sendbuf[1] = operat;
    MEMCPY_R(sendbuf + 2, pMAC, 6);
    ret = ble_set_operate(fd, CMD_LINK_CMD, sendbuf, 8, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("%s蓝牙%s超时 %02X:%02X:%02X:%02X:%02X:%02X\n", 0 == operat ? "断开" : "连接", devName[NO], 
                      pMAC[0], pMAC[1], pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("%s蓝牙%s成功 %02X:%02X:%02X:%02X:%02X:%02X\n", 0 == operat ? "断开" : "连接", devName[NO], 
                          pMAC[0], pMAC[1], pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
        }
        else
        {
            BLE_FMT_DEBUG("%s蓝牙%s失败(%d) %02X:%02X:%02X:%02X:%02X:%02X\n", 0 == operat ? "断开" : "连接", 
                           devName[NO], status, pMAC[0], pMAC[1], pMAC[2], pMAC[3], pMAC[4], pMAC[5]);
            return status;
        }
    }
    return BLE_SUCCESS;
}

/**********************************************************************
* @name      : ble_get_conninfo_proc
* @brief     ：查询蓝牙连接信息
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_get_conninfo_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    BLE_PRODATA_T  *pProRet = NULL;
    int             ret = 0;
    
    ret = ble_get_operate(fd, CMD_BLE_CONN_INFO, NULL, 0, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(pProRet)
    {
        ret = ble_dev_report_conn_proc(0, pProRet, pBleHandle, pBledata);
        free(pProRet);
    }

    return ret;
}

/**********************************************************************
* @name      : ble_assign_dev_channel
* @brief     ：分配蓝牙从设备id
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DEV_T *pBleStatus   蓝牙状态
               BLE_PORT_CONF_T *pConf  端口配置
* @param[out]：
* @return    ：从设备号0 - 无效, 1-3有效
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
uint8 ble_assign_dev_channel(int fd, BLE_DEV_H_T *pBleHandle, BLE_DEV_T *pBleStatus, BLE_PORT_CONF_T *pConf)
{
    uint8 i = 0;
    uint8 num = 0;
    int   ret = 0;
    struct timespec curtime = {0};
    BLE_PORT_EVENT_T tEvent;

    /* 重复设置判断 */
    for(i = 0; i < BT_SLAVE_MAX; i++)
    {
        if(pBleStatus->sPortStatus[i].port == pConf->port)
        {
            /* 参数变化 */
            if(0 != memcmp(&pBleStatus->sPortStatus[i].match, &pConf->match, sizeof(BLE_MATCH_T)) ||
               0 != memcmp(&pBleStatus->sPortStatus[i].MAC, &pConf->MAC, 6))
            {
                num = i;
                if(TRUE == pBleStatus->sPortStatus[i].bConnect)
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
    for(i = 0; i < BT_SLAVE_MAX; i++)
    {
        if(FALSE == pBleStatus->sPortStatus[i].bConnect)
        {
            pBleStatus->sPortStatus[i].port = pConf->port;
            pBleStatus->sPortStatus[i].mode = pConf->mode;
            pBleStatus->sPortStatus[i].match = pConf->match;

            /* 配对参数设置 */
            ret = ble_set_match_proc(fd, pBleHandle, &pConf->match, i + 1);
            if(0 != ret)
            {
                BLE_FMT_DEBUG("设置蓝牙从设备%d配对参数失败, 端口%d\n", i + 1, pConf->port);
            }

            memcpy(pBleStatus->sPortStatus[i].MAC, pConf->MAC, 6);
            /* 地址设置 */
            ret = ble_set_mac_proc(fd, pBleHandle, pConf->MAC, i + 1);
            if(0 != ret)
            {
                BLE_FMT_DEBUG("设置蓝牙从设备%d地址失败, 端口%d\n", i + 1, pConf->port);
            }

            /* 刷新时间 */
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            pBleStatus->sPortStatus[i].lasttime = curtime.tv_sec;
            pBleStatus->sPortStatus[i].lastus = curtime.tv_nsec / 1000;
            return i + 1;
        }
    }

    /* 全部连接则选择时间最远的处理 */
    for(i = 1; i < BT_SLAVE_MAX; i++)
    {
        if(pBleStatus->sPortStatus[i].lasttime < pBleStatus->sPortStatus[num].lasttime
           || (pBleStatus->sPortStatus[i].lasttime == pBleStatus->sPortStatus[num].lasttime && 
               pBleStatus->sPortStatus[i].lastus < pBleStatus->sPortStatus[num].lastus))
        {
            num = i;
        }
    }

disconnect:
    /* 断开连接 */
    memset(&tEvent, 0, sizeof(BLE_PORT_EVENT_T));
    tEvent.port = pBleStatus->sPortStatus[num].port;
    ret = ble_conn_disconn_mac(fd, pBleHandle, 0, pBleStatus->sPortStatus[num].MAC, num + 2);
    if(0 == ret)
    {
        pBleStatus->sPortStatus[num].bConnect = FALSE;
        memcpy(tEvent.MAC, pBleStatus->sPortStatus[num].MAC, 6);
        tEvent.mode = pBleStatus->sPortStatus[num].mode;
        tEvent.bConnect = FALSE;
    }
    return num + 1;

setConfig:
    /* 设置参数 */
    pBleStatus->sPortStatus[num].port = pConf->port;
    pBleStatus->sPortStatus[num].mode = pConf->mode;
    pBleStatus->sPortStatus[num].match = pConf->match;

    /* 配对参数设置 */
    ret = ble_set_match_proc(fd, pBleHandle, &pConf->match, i + 1);
    if(0 != ret)
    {
        BLE_FMT_DEBUG("设置蓝牙从设备%d配对参数失败, 端口%d\n", num + 1, pConf->port);
    }

    memcpy(pBleStatus->sPortStatus[num].MAC, pConf->MAC, 6);
    /* 地址设置 */
    ret = ble_set_mac_proc(fd, pBleHandle, pConf->MAC, i + 1);
    if(0 != ret)
    {
        BLE_FMT_DEBUG("设置蓝牙从设备%d地址失败, 端口%d\n", num + 1, pConf->port);
    }

    /* 刷新时间 */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    pBleStatus->sPortStatus[num].lasttime = curtime.tv_sec;
    pBleStatus->sPortStatus[num].lastus = curtime.tv_nsec / 1000;
    
    return num + 1;
}

/**********************************************************************
* @name      : ble_out_msg_porc
* @brief     ：蓝牙透传操作
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
               BLE_MSG_T *pMsg         传送的数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_out_msg_porc(int fd, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata, BLE_MSG_T *pMsg)
{
    int     ret = 1;
    uint8   MAC1[6] = {0};
    uint8   MAC2[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint32  port = pMsg->port;
    struct timespec oldtime = {0};
    struct timespec curtime = {0};
    uint8   channid = 0;
    BLE_DEV_T *pBle = &(pBledata->tDevStatus);
    
    /* 主设备 */
    if(port <= BLE_PORT_MASTER1)
    {
        if(FALSE == pBle->mPortStatus[port].bConnect || 0 == memcmp(pBle->mPortStatus[port].MAC, MAC1, 6) 
           || 0 == memcmp(pBle->mPortStatus[port].MAC, MAC2, 6) )
        {
            BLE_FMT_DEBUG("蓝牙主设备%d无连接\n", pMsg->port + 1);
            return BLE_ERR_UNEXIST_MAC;
        }
        else
        {
            ret = ble_tran_send(fd, pBle->mPortStatus[port].MAC, pMsg->msg, pMsg->len);
            if(0 != ret)
            {
                /* 发送失败表示串口出问题, 重新打开一下设备 */
                pBleHandle->status = BLE_DEV_STATUS_INIT;
                BLE_FMT_DEBUG("蓝牙主设备%d发送失败, 重新打开设备\n", port + 1);
                return BLE_ERR_OPER_FAIL;
            }
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            pBle->mPortStatus[port].lasttime = curtime.tv_sec;
            pBle->mPortStatus[port].lastus = curtime.tv_nsec / 1000;
            pBle->PortCfg[port].lasttime = curtime.tv_sec;
            pBle->PortCfg[port].lastus = curtime.tv_nsec / 1000;
        }
        return BLE_SUCCESS;
    }

    /* 从设备 */
    
    /* 1. 通道选择 */
    channid = ble_assign_dev_channel(fd, pBleHandle, pBle, &(pBle->PortCfg[port].porttCfg));
    pBle->PortCfg[port].channelid = channid;
    if(0 == channid || channid > 3)
    {
        pBle->PortCfg[port].bConnect = FALSE;
        BLE_FMT_DEBUG("蓝牙发送失败, 从设备分配失败, 端口%d\n", port + 1);
        return BLE_ERR_CONNECT_FAIL;
    }

    /* 2. 连接设备 */
    if(FALSE == pBle->sPortStatus[channid - 1].bConnect)
    {
        /* 连接设备 */
        ret = ble_conn_disconn_mac(fd, pBleHandle, 1, pBle->sPortStatus[channid - 1].MAC, channid + 1);
        if(0 != ret)
        {
            return BLE_ERR_CONNECT_FAIL;
        }

        /* 等待 */
        clock_gettime(CLOCK_MONOTONIC, &oldtime);
        while(1)
        {
            clock_gettime(CLOCK_MONOTONIC, &curtime);
            if(TRUE == pBle->sPortStatus[channid - 1].bConnect)
            {
                break;
            }
            if(curtime.tv_sec > oldtime.tv_sec + 5)
            {
                BLE_FMT_DEBUG("蓝牙从设备%d连接%02X:%02X:%02X:%02X:%02X:%02X超时\n", channid, 
                              pBle->sPortStatus[channid - 1].MAC[0], pBle->sPortStatus[channid - 1].MAC[1], 
                              pBle->sPortStatus[channid - 1].MAC[2], pBle->sPortStatus[channid - 1].MAC[3],
                              pBle->sPortStatus[channid - 1].MAC[4], pBle->sPortStatus[channid - 1].MAC[5]);
                return BLE_ERR_CONNECT_FAIL;
            }

            ble_get_conninfo_proc(fd, pBleHandle, pBledata);
            usleep(200000);
        }
    }

    /* 3. 发送 */
    ret = ble_tran_send(fd, pBle->sPortStatus[channid - 1].MAC, pMsg->msg, pMsg->len);
    if(0 != ret)
    {
        /* 发送失败表示串口出问题, 重新打开一下设备 */
        pBleHandle->status = BLE_DEV_STATUS_INIT;
        BLE_FMT_DEBUG("蓝牙从设备%d发送失败, 重新打开设备\n", channid);
        return BLE_ERR_OPER_FAIL;
    }
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    pBle->sPortStatus[channid - 1].lasttime = curtime.tv_sec;
    pBle->sPortStatus[channid - 1].lastus = curtime.tv_nsec / 1000;
    pBle->PortCfg[port].lasttime = curtime.tv_sec;
    pBle->PortCfg[port].lastus = curtime.tv_nsec / 1000;
    return BLE_SUCCESS;
}

/**********************************************************************
* @name      : ble_set_check_proc
* @brief     ：设置蓝牙检定
* @param[in] ：int fd                   蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_CHECK_T *ptCheck    检定参数
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2022-3-1
* @Update    :
**********************************************************************/
int ble_set_check_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_CHECK_T *ptCheck)
{
    int    ret = 0;
    uint8  sendbuf[32] = {0};
    uint16 offset = 0;
    uint8  i = 0;
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T   *pProRet = NULL;

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
    ret = ble_set_operate(fd, CMD_BLE_PARAMS, sendbuf, offset, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置蓝牙检定超时, 类型%d, 间隔%dms, 功率%d, 地址%02X:%02X:%02X:%02X:%02X:%02X\n", 
                       ptCheck->type, ptCheck->interval, ptCheck->power, ptCheck->MAC[0], ptCheck->MAC[1],
                       ptCheck->MAC[2], ptCheck->MAC[3], ptCheck->MAC[4], ptCheck->MAC[5]);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置蓝牙检定成功, 类型%d, 间隔%dms, 功率%d, 地址%02X:%02X:%02X:%02X:%02X:%02X\n", 
                       ptCheck->type, ptCheck->interval, ptCheck->power, ptCheck->MAC[0], ptCheck->MAC[1],
                       ptCheck->MAC[2], ptCheck->MAC[3], ptCheck->MAC[4], ptCheck->MAC[5]);
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙检定失败(%d) 类型%d, 间隔%dms, 功率%d, 地址%02X:%02X:%02X:%02X:%02X:%02X\n", status, 
                          ptCheck->type, ptCheck->interval, ptCheck->power, ptCheck->MAC[0], ptCheck->MAC[1],
                       ptCheck->MAC[2], ptCheck->MAC[3], ptCheck->MAC[4], ptCheck->MAC[5]);
            return status;
        }
    }

    return 0;
}

/**********************************************************************
* @name      : ble_update_broadcast_param
* @brief     ：更新广播参数
* @param[in] ：ptMatch       配对参数
* @param[out]：ptBoardcast   参数
* @return    ：
* @Create    : 
* @Date      ：
* @Update    :
**********************************************************************/
void ble_update_broadcast_param(BLE_DATA_T *pBledata, BLE_BROADCAST_T *ptBoardcast)
{
    uint8  outData[512] = {0};
    uint16 outLen = 0;

    if ((pBledata == NULL) || (ptBoardcast == NULL))
    {
        return;
    }
    
    pBledata->tDevStatus.broadcast.flagLen = 0x02;
    pBledata->tDevStatus.broadcast.flagType = 0x01;
    pBledata->tDevStatus.broadcast.flagValue = 0x06;
    pBledata->tDevStatus.broadcast.factLen = 0x16;
    pBledata->tDevStatus.broadcast.factType = 0xff;
    pBledata->tDevStatus.broadcast.devCode = 0xC2;
    pBledata->tDevStatus.broadcast.factCode[0] = 0xff;
    pBledata->tDevStatus.broadcast.factCode[1] = 0xff;


    if ((pBledata->tDevStatus.match.mode != BLE_MATCH_PE) || (pBledata->tDevStatus.match.len == 0))
    {
        pBledata->tDevStatus.broadcast.pinType[0] = 0xff;
        pBledata->tDevStatus.broadcast.pinType[1] = 0xff;
        memset(&pBledata->tDevStatus.broadcast.pin[0], 0xff, 16);
    }
    else
    {
        if (ESAM_OK != ResponseFrameDeal(SecMod_Cipher, (uint8 *)pBledata->tDevStatus.match.pwd, 
                pBledata->tDevStatus.match.len, outData, &outLen))
        {
            pBledata->tDevStatus.broadcast.pinType[0] = 0x00;
            pBledata->tDevStatus.broadcast.pinType[1] = 0x00;
            pBledata->tDevStatus.broadcast.pin[0] = 0x31;
            pBledata->tDevStatus.broadcast.pin[1] = 0x32;
            pBledata->tDevStatus.broadcast.pin[2] = 0x33;
            pBledata->tDevStatus.broadcast.pin[3] = 0x34;
            pBledata->tDevStatus.broadcast.pin[4] = 0x35;
            pBledata->tDevStatus.broadcast.pin[5] = 0x36;
            memset(&pBledata->tDevStatus.broadcast.pin[6], 0xff, 10);

            BLE_BUF_DEBUG(pBledata->tDevStatus.broadcast.pin, 16, "PIN加密失败，使用默认PIN");
        }
        else
        {
            BLE_BUF_DEBUG(outData, outLen, "PIN加密成功");
            pBledata->tDevStatus.broadcast.pinType[0] = 0x01;
            pBledata->tDevStatus.broadcast.pinType[1] = 0x00;
            memcpy(pBledata->tDevStatus.broadcast.pin, outData, MIN(16, outLen));
        }
    }
    
    pBledata->tDevStatus.broadcast.devLen = 0x04;
    pBledata->tDevStatus.broadcast.devType = 0x09;
    memcpy(&pBledata->tDevStatus.broadcast.devName, pBledata->tDevStatus.config.name, MIN(BLE_NAME_MAX_LEN, 3));

    return;
}

/**********************************************************************
* @name      : ble_set_broadcast_proc
* @brief     ：设置广播参数
* @param[in] ：int fd                   蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_BROADCAST_T *ptMatch配对参数
               uint8 NO                序号(0-本机, 1-3从设备)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_set_broadcast_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata, uint8 NO)
{
    int    ret = 0;
    uint8  sendbuf[32] = {0};
    uint16 offset = 0;
    uint8  status = BLE_SUCCESS;
    BLE_PRODATA_T   *pProRet = NULL;
    BLE_BROADCAST_T *ptBoardcast = &pBledata->tDevStatus.broadcast;

    ble_update_broadcast_param(pBledata, ptBoardcast);

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

    ret = ble_set_operate(fd, CMD_ADV_DATA, sendbuf, offset, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                          BLE_RECV_BUF_LEN - pBleHandle->recvlen);
    if(ret < 0)
    {
        return ret;
    }
    pBleHandle->recvlen += ret;
    if(NULL == pProRet)
    {
        BLE_FMT_DEBUG("设置广播参数超时, 广播类型%d, 设备类型%X, 厂商%02X%02X 特征码%02X%02X\n", ptBoardcast->flagType, 
            ptBoardcast->devCode, ptBoardcast->factCode[0], ptBoardcast->factCode[1], ptBoardcast->pinType[0], 
            ptBoardcast->pinType[1]);
        return BLE_ERR_OTHER_FAIL;
    }
    else
    {
        status = pProRet->data[0];
        free(pProRet);
        if(BLE_SUCCESS == status)
        {
            BLE_FMT_DEBUG("设置广播参数成功, 广播类型%d, 设备类型%X, 厂商%02X%02X 特征码%02X%02X\n", ptBoardcast->flagType, 
                ptBoardcast->devCode, ptBoardcast->factCode[0], ptBoardcast->factCode[1], ptBoardcast->pinType[0], 
                ptBoardcast->pinType[1]);
        }
        else
        {
            BLE_FMT_DEBUG("设置广播参数失败, 广播类型%d, 设备类型%X, 厂商%02X%02X 特征码%02X%02X\n", ptBoardcast->flagType, 
                ptBoardcast->devCode, ptBoardcast->factCode[0], ptBoardcast->factCode[1], ptBoardcast->pinType[0], 
                ptBoardcast->pinType[1]);
            return status;
        }
    }

    return 0;
}

#endif

#if DESC("MQTT后续操作", 1)
    
/**********************************************************************
* @name      : ble_setparam_msg_proc
* @brief     ：设置蓝牙基本参数消息操作
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_MSG_T *pMsg     消息
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_setparam_msg_proc(int fd, BLE_DEV_MSG_T *pMsg, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    int ret = 0;
    BLE_CONFIG_T *pConfig = NULL;

    pConfig = (BLE_CONFIG_T *)pMsg->data;
    
    if(FALSE == pBledata->tDevStatus.bConfig || 
       0 != memcmp(&pBledata->tDevStatus.config, pConfig, sizeof(BLE_CONFIG_T)))
    {
        #if PIN_ENABLE == 1
        if (pBledata->tDevStatus.bMatch)
        {
            /* 如果地址变化，则可能需要重新设置广播参数 */
            if (0 != memcmp(pBledata->tDevStatus.config.name, pConfig->name, BLE_NAME_MAX_LEN))
            {
                /* 设置广播参数 */
                memcpy(pBledata->tDevStatus.config.name, pConfig->name, BLE_NAME_MAX_LEN);
                ret = ble_set_broadcast_proc(fd, pBleHandle, pBledata, 0);
            }
        }
        #endif
        
        ret = ble_set_param_proc(fd, pBleHandle, pConfig, TRUE);
        if(0 == ret)
        {
            memcpy(&pBledata->tDevStatus.config, pConfig, sizeof(BLE_CONFIG_T));
            pBledata->tDevStatus.bConfig = TRUE;
            /* 保存配置 */
            ble_write_config(pConfig);
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

        ptAXDR->sourlen = strlen(BT_APP_NAME_M);
        memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

        ptAXDR->destlen = strlen(pMsg->source);
        memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

        ptAXDR->IID = MESSAGE_IID_BLE;
        ptAXDR->IOP = pMsg->proc;

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return ble_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = ble_common_res_package(pMsg->token, ret);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", BT_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_SET,
                                                MQTT_TOPIC_INFO_RES, BLE_TOPOC_SETPARAM);
            ret = ble_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}

/**********************************************************************
* @name      : ble_setport_msg_proc
* @brief     ：设置蓝牙端口消息操作
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_MSG_T *pMsg     消息
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_setport_msg_proc(int fd, BLE_DEV_MSG_T *pMsg, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    int    ret = 0;
    uint32 i = 0;
    uint32 port = 0;
    BLE_PORTS_CONF_T *pPort = NULL;
    BLE_DEV_T        *ptDev = &(pBledata->tDevStatus);
    uint8   MAC1[6] = {0};
    uint8   MAC2[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    pPort = (BLE_PORTS_CONF_T *)pMsg->data;
    for(i = 0; i < pPort->num; i++)
    {
        port = pPort->tConf[i].port;
        if(pPort->tConf[i].port <= BLE_PORT_MASTER1 && BLE_MODE_MASTER == pPort->tConf[i].mode)
        {
            /* 主动设置 */
            if(FALSE == ptDev->bMatch || 0 != memcmp(&ptDev->match, &pPort->tConf[i].match, sizeof(BLE_MATCH_T)))
            {
                ret = ble_set_match_proc(fd, pBleHandle, &pPort->tConf[i].match, 0);
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
                        memcpy(&ptDev->match, &pPort->tConf[i].match, sizeof(BLE_MATCH_T));
                        ptDev->bMatch = TRUE;
                        
                        /* 设置广播参数 */
                        ble_set_broadcast_proc(fd, pBleHandle, pBledata, 0);
                                                
                        BLE_FMT_DEBUG("设置配对参数修改后，需要重新设置其他参数\n");
                        ble_set_param_proc(fd, pBleHandle, &pBledata->tDevStatus.config, FALSE);
                    }
                }
                #endif
                
                memcpy(&ptDev->match, &pPort->tConf[i].match, sizeof(BLE_MATCH_T));
                ptDev->bMatch = TRUE;
                /* 保存配置 */
                ble_write_match(&ptDev->match);

                ptDev->PortCfg[port].bConfig = TRUE;
                ptDev->PortCfg[port].porttCfg = pPort->tConf[i];
                /* 保存配置 */
                ble_write_portcfg(port, &(pPort->tConf[i]));
            }
        }
        else if(pPort->tConf[i].port > BLE_PORT_MASTER1 && BLE_MODE_SLAVE == pPort->tConf[i].mode)
        {
            /* 从动配置 端口分配 */
            ptDev->PortCfg[port].channelid = ble_assign_dev_channel(fd, pBleHandle, ptDev, &pPort->tConf[i]);
            ptDev->PortCfg[port].bConfig = TRUE;
            ptDev->PortCfg[port].porttCfg = pPort->tConf[i];
            /* 保存配置 */
            ble_write_portcfg(port, &(pPort->tConf[i]));

            //刷新桌面蓝牙状态显示
            bel_refresh_status(pBledata, TRUE);
        }
    }

    /* 设置完成, 连接一下从设备 */
    for(i = 0; i < BT_SLAVE_MAX; i++)
    {
        if(FALSE == ptDev->sPortStatus[i].bConnect && 0 != memcmp(ptDev->sPortStatus[i].MAC, MAC1, 6) &&
           0 != memcmp(ptDev->sPortStatus[i].MAC, MAC2, 6))
        {
            ret = ble_conn_disconn_mac(fd, pBleHandle, 1, ptDev->sPortStatus[i].MAC, i + 2);
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

        ptAXDR->sourlen = strlen(BT_APP_NAME_M);
        memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

        ptAXDR->destlen = strlen(pMsg->source);
        memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

        ptAXDR->IID = MESSAGE_IID_BLE;
        ptAXDR->IOP = pMsg->proc;

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return ble_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = ble_common_res_package(pMsg->token, ret);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", BT_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_SET,
                                                MQTT_TOPIC_INFO_RES, BLE_TOPOC_SETPORT);
            ret = ble_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}

/**********************************************************************
* @name      : ble_checkmeter_msg_proc
* @brief     ：设置蓝牙检定参数
* @param[in] ：int fd                   蓝牙句柄
               BLE_DEV_MSG_T *pMsg     消息
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_checkmeter_msg_proc(int fd, BLE_DEV_MSG_T *pMsg, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    int ret = 0;
    BLE_CHECK_T *pCheck = NULL;

    pCheck = (BLE_CHECK_T *)pMsg->data;
    
    ret = ble_set_checkmeter_proc(fd, pBleHandle, pBledata, pCheck);

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

        ptAXDR->sourlen = strlen(BT_APP_NAME_M);
        memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

        ptAXDR->destlen = strlen(pMsg->source);
        memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

        ptAXDR->IID = MESSAGE_IID_BLE;
        ptAXDR->IOP = pMsg->proc;

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return ble_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        BLE_FMT_DEBUG("JSON消息没有蓝牙检定消息 消息类型%d\n", pMsg->type);
        return ret;
    }
    #else
    return ret;
    #endif
}

/**********************************************************************
* @name      : ble_send_msg_proc
* @brief     ：蓝牙发送消息操作
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_MSG_T *pMsg     消息
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_send_msg_proc(int fd, BLE_DEV_MSG_T *pMsg, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    int          ret = -1;
    BLE_MSG_T   *pMsgSend = NULL;
    BLE_DEV_T   *ptDev = &(pBledata->tDevStatus);

    /* 判断端口是否有效 */
    pMsgSend = (BLE_MSG_T *)pMsg->data;
    if(pMsgSend->port >= BLE_PORT_MAX || 
       (pMsgSend->port > BLE_PORT_MASTER1 && FALSE == ptDev->PortCfg[pMsgSend->port].bConfig))
    {
        BLE_FMT_DEBUG("无配置端口%d\n", pMsgSend->port);
    }
    else
    {
        /* 发送数据 */
        ret = ble_out_msg_porc(fd, pBleHandle, pBledata, pMsgSend);
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

        ptAXDR->sourlen = strlen(BT_APP_NAME_M);
        memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

        ptAXDR->destlen = strlen(pMsg->source);
        memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

        ptAXDR->IID = MESSAGE_IID_BLE;
        ptAXDR->IOP = pMsg->proc;

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return ble_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = ble_common_res_package(pMsg->token, ret);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", BT_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_ACT,
                                                MQTT_TOPIC_INFO_RES, BLE_TOPOC_SENDDATA);
            ret = ble_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}

/**********************************************************************
* @name      : ble_trans_msg_proc
* @brief     ：蓝牙转发消息操作
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_MSG_T *pMsg     消息
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-18
* @Update    :
**********************************************************************/
int ble_trans_msg_proc(int fd, BLE_DEV_MSG_T *pMsg, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    int               ret = -1;
    BLE_TRANS_REQ_T  *pMsgSend = (BLE_TRANS_REQ_T*)pMsg->data;
    BLE_DEV_T        *ptDev = &(pBledata->tDevStatus);
    BLE_TRANS_ACK_T   tAck;
    BLE_MSG_T         tMsgOut;
    BLE_PORT_CONF_T   tempConf;
    struct timespec oldtime = {0};
    struct timespec curtime = {0};
    uint32            spacems = 0;
    BLE_PRODATA_T *pProData = NULL;
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
    memset(&tMsgOut, 0, sizeof(BLE_MSG_T));
    tMsgOut.port = pMsgSend->port;
    tMsgOut.len = pMsgSend->len;
    memcpy(tMsgOut.msg, pMsgSend->msg, pMsgSend->len);
    ret = ble_out_msg_porc(fd, pBleHandle, pBledata, &tMsgOut);
    
    /* 等待应答 */
    memset(&tAck, 0, sizeof(BLE_TRANS_ACK_T));
    tAck.ret = ret;
    if(0 == ret)
    {
        tAck.ret = BLE_ERR_OPER_FAIL;
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
            retlen = ble_uart_recv(fd, buf, 20480, 100, 200);
            if(retlen <= 0)
            {
                continue;
            }
            BLE_BUF_DEBUG(buf, retlen, "串口接收");

            temp = pBleHandle->recv;
            start = 0;
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
                            if(BLE_PRO_TO_OUT == pProData->type && 0 == pProData->cmd && 
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
                if(pBleHandle->recvlen < BLE_RECV_BUF_LEN)
                {
                    temp[pBleHandle->recvlen] = buf[start];
                    pBleHandle->recvlen += 1;
                }
                start++;
            }

            /* 剩余内容拷贝构造返回消息 */
            if(pProData)
            {
                start += BLE_PROTOCOL_LEN_MIN + pProData->datalen;
                if(start < retlen && pBleHandle->recvlen < BLE_RECV_BUF_LEN)
                {
                    msglen = MIN(BLE_RECV_BUF_LEN - pBleHandle->recvlen, retlen - start);
                    memcpy(temp + pBleHandle->recvlen, buf + start, msglen);
                    pBleHandle->recvlen += msglen;
                }

                tAck.ret = BLE_SUCCESS;
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

        ptAXDR->sourlen = strlen(BT_APP_NAME_M);
        memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

        ptAXDR->destlen = strlen(pMsg->source);
        memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

        ptAXDR->IID = MESSAGE_IID_BLE;
        ptAXDR->IOP = pMsg->proc;

        ptAXDR->msglen = ble_trans_ack_package(&tAck, ptAXDR->playload, 3000);
        return ble_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = ble_transmsg_res_package(pMsg->token, &tAck);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", BT_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_ACT,
                                                MQTT_TOPIC_INFO_RES, BLE_TOPOC_FORWARD);
            ret = ble_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}

/**********************************************************************
* @name      : ble_setcheck_msg_proc
* @brief     ：设置蓝牙检定消息操作
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_MSG_T *pMsg     消息
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_setcheck_msg_proc(int fd, BLE_DEV_MSG_T *pMsg, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    int ret = 0;
    BLE_CHECK_T *pCheck = NULL;
    
    pCheck = (BLE_CHECK_T *)pMsg->data;

    ret = ble_set_check_proc(fd, pBleHandle, pCheck);
    if(0 == ret)
    {
        pBledata->tDevStatus.bCheckMeter = pCheck->type != BLE_TYPE_CLOSE ? TRUE : FALSE;
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

        ptAXDR->sourlen = strlen(BT_APP_NAME_M);
        memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

        ptAXDR->destlen = strlen(pMsg->source);
        memcpy(ptAXDR->destaddr, pMsg->source, ptAXDR->destlen);

        ptAXDR->IID = MESSAGE_IID_BLE;
        ptAXDR->IOP = pMsg->proc;

        ptAXDR->playload[0] = 0 == ret ? 1 : 0;
        ptAXDR->msglen = 1;
        
        return ble_send_axdr_msg(ptAXDR, 0);
    }
    else
    {
        char  *pMsgStr = NULL;
        char   topic[256] = {0};

        pMsgStr = ble_common_res_package(pMsg->token, ret);
        if(pMsgStr)
        {
            sprintf(topic, "%s/%s/%s/%s/%s/%s", BT_APP_NAME, pMsg->source, MQTT_TOPIC_JSON, MQTT_TOPIC_OPER_SET,
                                                MQTT_TOPIC_INFO_RES, BLE_TOPOC_SETDETECT);
            ret = ble_send_json_msg(topic, 0, pMsgStr, strlen(pMsgStr));
            free(pMsgStr);
        }

        return ret;
    }
}


#endif
/**********************************************************************
* @name      : ble_check_devfd
* @brief     ：检测串口状态是否异常
* @param[in] ：BLE_DEV_H_T *pBleHandle 线程句柄
* @return    ：
**********************************************************************/
void ble_check_devfd(BLE_DEV_H_T *pBleHandle)
{
    struct termios options; 
    
    if (pBleHandle->chkfdcnt > 5*(200/BLE_RECV_TIME_OUT))
    {
        pBleHandle->chkfdcnt = 0;

        if (pBleHandle->devfd >= 0)
        {
            if (tcgetattr(pBleHandle->devfd, &options)  !=  0) /*读取串口termios结构*/
            {
                BLE_FMT_DEBUG("蓝牙设备%s异常，需要重新初始化 fd[%d]\n", BLE_UART_NAME, pBleHandle->devfd);
                pBleHandle->status = BLE_DEV_STATUS_INIT;
            }
        }
    }
    else
    {
        pBleHandle->chkfdcnt ++;
    }
    
    return;
}

/**********************************************************************
* @name      : ble_dev_try_open
* @brief     ：蓝牙串口打开尝试
* @param[in] ：BLE_DEV_H_T *pBleHandle 线程句柄
* @param[out]：
* @return    ：蓝牙句柄
* @Create    : 王津剑
* @Date      ：2021-8-24
* @Update    :
**********************************************************************/
int ble_dev_try_open(BLE_DEV_H_T *pBleHandle)
{
    int fd = -1;
    int ret = 0;
    uint8 i = 0;
    char  type[3] = {'N' , 'E', 'O'};
    uint8 bSucc = FALSE;
    uint8  sendbuf[1] = {1};
    BLE_PRODATA_T *pProRet = NULL;
    
    for(i = 0; i < 3; i++)
    {
        fd = ble_uart_open(BLE_UART_NAME);
        if(fd < -1)
        {
            BLE_FMT_DEBUG("打开蓝牙设备%s失败\n", BLE_UART_NAME);
            return -1;
        }
        
        ret = ble_uart_set(fd, 115200, 8, 1, type[i]);
        if(0 != ret)
        {
            BLE_FMT_DEBUG("设置蓝牙参数(115200, 8, 1, %c)失败, 错误码%d\n", type[i], ret);
        }
        
        ret = ble_get_operate(fd, CMD_DEV_ADDR, sendbuf, 1, &pProRet, pBleHandle->recv + pBleHandle->recvlen, 
                              BLE_RECV_BUF_LEN - pBleHandle->recvlen);
        if(ret >= 0)
        {
            pBleHandle->recvlen += ret;
        }
        
        if(pProRet)
        {
            /* 通过数据收发判断是否参数设置成功 */
            bSucc = TRUE;
            BLE_FMT_DEBUG("设置蓝牙参数(115200, 8, 1, %c)成功\n", type[i]);
            free(pProRet);
            break;
        }
        else
        {
            BLE_FMT_DEBUG("设置蓝牙参数(115200, 8, 1, %c)失败\n", type[i]);
        }
        
        ble_uart_close(fd);
        fd = -1;
        sleep(3);
    }

    if(FALSE == bSucc)
    {
        ble_uart_close(fd);
        fd = -1;
        BLE_FMT_DEBUG("蓝牙设备%s设置串口参数失败\n", BLE_UART_NAME);
    }

    return fd;
}

/**********************************************************************
* @name      : ble_dev_prepare
* @brief     ：蓝牙模块开工准备(获取基本参数，设置基本参数)
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_dev_prepare(int fd, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    BLE_DEV_T        *ptDev = &pBledata->tDevStatus;
    int               ret = 0;

    if(ptDev->bMatch)
    {
        /* 设置配对参数 */
        ret = ble_set_match_proc(fd, pBleHandle, &ptDev->match, 0);
    }
    else
    {
        /* 查询配对参数 */
        ble_get_match_proc(fd, pBleHandle, &ptDev->match, 0);
    }

    #if PIN_ENABLE == 1
    if (ptDev->bMatch)
    {
        /* 设置广播参数 */
        ret = ble_set_broadcast_proc(fd, pBleHandle, pBledata, 0);
    }
    #endif
    
    if(ptDev->bConfig)
    {
        /* 设置参数 */
        ret = ble_set_param_proc(fd, pBleHandle, &ptDev->config, TRUE);
    }
    else
    {
        /* 查询参数 */
        ble_get_param_proc(fd, pBleHandle, &ptDev->config);
    }
    
    /* 获取固件信息 */
    if(0 == ret)
    {
        ble_get_baseinfo_proc(fd, pBleHandle, pBledata);
    }

    /* 设置蓝牙接收模式 */
    ble_set_link_mode(fd, pBleHandle);
    return ret;
}

/**********************************************************************
* @name      : ble_working_proc
* @brief     ：蓝牙工作状态处理
* @param[in] ：int fd                  蓝牙句柄
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-17
* @Update    :
**********************************************************************/
int ble_working_proc(int fd, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    BLE_DEV_MSG_T *pMsg = NULL;
    int            ret = -1;
    uint8          bOper = FALSE;
    
    /* 参数设置 透传 */
    if (pBledata->tDevPorc.cfgDelay > 0)
    {
        pBledata->tDevPorc.cfgDelay --;
    }
    else
    {
        pMsg = ble_read_queue(&(pBleHandle->cfgQueue));
        if(pMsg)
        {
            switch(pMsg->proc)
            {
                case BLE_IOP_TRAN_DATA:
                {
                    ret = ble_trans_msg_proc(fd, pMsg, pBleHandle, pBledata);
                    break;
                }
                case BLE_IOP_SET_CONFIG:
                {
                    ret = ble_setparam_msg_proc(fd, pMsg, pBleHandle, pBledata);
                    break;
                }
                case BLE_IOP_SET_PORT:
                {
                    ret = ble_setport_msg_proc(fd, pMsg, pBleHandle, pBledata);
                    break;
                }
                case BLE_IOP_CHECK_METER:
                {
                    ret = ble_checkmeter_msg_proc(fd, pMsg, pBleHandle, pBledata);
                    break;
                }
                default: break;
            }
            bOper = TRUE;
            free(pMsg);
        }
    }

    /* 转发操作 */
    pMsg = ble_read_queue(&(pBleHandle->transQueue));
    if(pMsg)
    {
        if(BLE_IOP_TRAN_DATA == pMsg->proc)
        {
            /* 发送数据 */
            ret = ble_trans_msg_proc(fd, pMsg, pBleHandle, pBledata);
        }
        bOper = TRUE;
        free(pMsg);
    }

    pMsg = ble_read_queue(&(pBleHandle->msgQueue));
    if(pMsg)
    {
        if(BLE_IOP_SEND_DATA == pMsg->proc)
        {
            /* 发送数据 */
            ret = ble_send_msg_proc(fd, pMsg, pBleHandle, pBledata);
        }
        bOper = TRUE;
        free(pMsg);
    }

    if(FALSE == bOper)
    {
        /* 接收数据 */
        ret = ble_uart_recv(fd, pBleHandle->recv + pBleHandle->recvlen, BLE_RECV_BUF_LEN - pBleHandle->recvlen, 
                            BLE_RECV_TIME_OUT, 200);
        if(ret > 0)
        {
            BLE_BUF_DEBUG(pBleHandle->recv + pBleHandle->recvlen, ret, "串口接收");
            pBleHandle->recvlen += ret;
            return 0;
        }
    }
    return ret;
}

/**********************************************************************
* @name      : ble_dev_recvmsg_proc
* @brief     ：蓝牙模块接收消息处理
* @param[in] ：int fd                  蓝牙句柄
               BLE_PRODATA_T *pProData 消息解析
               BLE_DEV_H_T *pBleHandle 线程句柄
               BLE_DATA_T *pBledata    蓝牙数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-14
* @Update    :
**********************************************************************/
int ble_dev_recvmsg_proc(int fd, BLE_PRODATA_T *pProData, BLE_DEV_H_T *pBleHandle, BLE_DATA_T *pBledata)
{
    if(fd < 0 || NULL == pProData || NULL == pBleHandle || NULL == pBledata)
    {
        return -1;
    }

    if(BLE_PRO_TO_OUT == pProData->type && 0 == pProData->cmd)
    {
        /* 收到蓝牙芯片透传 */
        return ble_dev_msgreport_proc(fd, pProData, pBleHandle, pBledata);
    }
    else if(BLE_PRO_REPORT == pProData->type && 0 == pProData->bAck && 0 == pProData->bStatus)
    {
        /* 主动上报 */
        if(CMD_MODULE_INFO == pProData->cmd)
        {
            return ble_dev_report_info_proc(fd, pProData, pBleHandle, pBledata);
        }
        else if(CMD_BLE_CONN_INFO == pProData->cmd)
        {
            return ble_dev_report_conn_proc(fd, pProData, pBleHandle, pBledata);
        }
    }
    BLE_FMT_DEBUG("不支持或超时的消息: type %d, Ack %d, Status %d, cmd 0x%08X, MAC %02X:%02X:%02X:%02X:%02X:%02X\n", 
                   pProData->type, pProData->bAck, pProData->bStatus, pProData->cmd, pProData->mac[0],
                   pProData->mac[1], pProData->mac[2], pProData->mac[3], pProData->mac[4], pProData->mac[5]);
    return 0;
}

/**********************************************************************
* @name      : ble_dev_init
* @brief     ：mqtt消息处理初始化
* @param[in] ：void *pPublic    公有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-10
* @Update    :
**********************************************************************/
int ble_dev_init(void *pPublic, void **ppPrivate)
{
    BLE_DATA_T *pBledata = (BLE_DATA_T *)pPublic;

    /* data文件加载 todo */


    /* 检查蓝牙设备是否存在 */
    while(1)
    {
        if(0 == access(BLE_UART_NAME, F_OK))
        {
            break;
        }
        sleep(1);
    }

    /* 连接数据初始化 */
    pBledata->tDevStatus.mPortStatus[0].bChannel = 1;
    pBledata->tDevStatus.mPortStatus[0].bConnect = FALSE;
    memset(pBledata->tDevStatus.mPortStatus[0].MAC, 0xFF, 6);
    pBledata->tDevStatus.mPortStatus[0].port = 0;
    
    pBledata->tDevStatus.mPortStatus[1].bChannel = 2;
    pBledata->tDevStatus.mPortStatus[1].bConnect = FALSE;
    memset(pBledata->tDevStatus.mPortStatus[1].MAC, 0xFF, 6);
    pBledata->tDevStatus.mPortStatus[1].port = 1;

    pBledata->tDevStatus.sPortStatus[0].bChannel = 1;
    pBledata->tDevStatus.sPortStatus[0].bConnect = FALSE;
    memset(pBledata->tDevStatus.sPortStatus[0].MAC, 0xFF, 6);
    pBledata->tDevStatus.sPortStatus[0].port = 0xFF;
    
    pBledata->tDevStatus.sPortStatus[1].bChannel = 2;
    pBledata->tDevStatus.sPortStatus[1].bConnect = FALSE;
    memset(pBledata->tDevStatus.sPortStatus[1].MAC, 0xFF, 6);
    pBledata->tDevStatus.sPortStatus[1].port = 0xFF;

    pBledata->tDevStatus.sPortStatus[2].bChannel = 3;
    pBledata->tDevStatus.sPortStatus[2].bConnect = FALSE;
    memset(pBledata->tDevStatus.sPortStatus[2].MAC, 0xFF, 6);
    pBledata->tDevStatus.sPortStatus[2].port = 0xFF;

    if(FALSE == pBledata->tDevStatus.PortCfg[0].bConfig)
    {
        pBledata->tDevStatus.PortCfg[0].channelid = 0;
        pBledata->tDevStatus.PortCfg[0].bConnect = FALSE;
        memset(pBledata->tDevStatus.PortCfg[0].porttCfg.MAC, 0xFF, 6);
        pBledata->tDevStatus.PortCfg[0].porttCfg.mode = BLE_MODE_MASTER;
        pBledata->tDevStatus.PortCfg[0].porttCfg.port = 0;
    }
    if(FALSE == pBledata->tDevStatus.PortCfg[1].bConfig)
    {
        pBledata->tDevStatus.PortCfg[1].channelid = 0;
        pBledata->tDevStatus.PortCfg[1].bConnect = FALSE;
        memset(pBledata->tDevStatus.PortCfg[1].porttCfg.MAC, 0xFF, 6);
        pBledata->tDevStatus.PortCfg[1].porttCfg.mode = BLE_MODE_MASTER;
        pBledata->tDevStatus.PortCfg[1].porttCfg.port = 1;
    }
    
    ble_clean_queue(&pBledata->tDevPorc.cfgQueue);
    ble_clean_queue(&pBledata->tDevPorc.transQueue);
    ble_clean_queue(&pBledata->tDevPorc.msgQueue);
    pBledata->tDevPorc.devfd = -1;
    *ppPrivate = &pBledata->tDevPorc;
    BLE_FMT_DEBUG("蓝牙设备%s存在, 开始工作\n", BLE_UART_NAME);
    return 0;
}

/**********************************************************************
* @name      : ble_dev_proc
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
int ble_dev_proc(void *pArg, void *pPublic, void *pPrivate)
{
    BLE_DATA_T  *pBledata = (BLE_DATA_T *)pPublic;
    BLE_DEV_H_T *pBleHandle = (BLE_DEV_H_T *)pPrivate;

    //定时检测句柄状态(1秒一次)，异常则重新初始化
    ble_check_devfd(pBleHandle);
    
    if(BLE_DEV_STATUS_INIT == pBleHandle->status && pBleHandle->devfd >= 0)
    {
        ble_uart_close(pBleHandle->devfd);
        pBleHandle->devfd = -1;
        pBledata->bWork = FALSE;
    }
    
    switch(pBleHandle->status)
    {
        case BLE_DEV_STATUS_INIT:
        {
            int fd = -1;
            fd = ble_dev_try_open(pBleHandle);
            if(fd < 0)
            {
                sleep(10);
            }
            else
            {
                pBleHandle->status = BLE_DEV_STATUS_PREPARE;
                pBleHandle->devfd = fd;
                pBledata->bWork = TRUE;
            }
            break;
        }
        case BLE_DEV_STATUS_PREPARE:
        {
            /* 查询是否需要重置配置 */
            if(0 == ble_dev_prepare(pBleHandle->devfd, pBleHandle, pBledata))
            {
                pBleHandle->status = BLE_DEV_STATUS_GETCONN;
            }
            break;
        }
        case BLE_DEV_STATUS_GETCONN:
        {
            /* 查询连接信息 */
            ble_get_conninfo_proc(pBleHandle->devfd, pBleHandle, pBledata);
            pBleHandle->status = BLE_DEV_STATUS_WORK;
            break;
        }
        case BLE_DEV_STATUS_WORK:
        {
            /* 工作状态(发送、接收) */
            ble_working_proc(pBleHandle->devfd, pBleHandle, pBledata);
            break;
        }
        default:
        {
            pBleHandle->status = BLE_DEV_STATUS_INIT;
            break;
        }
    }

    /* 处理缓存消息(透传、上报) */
    if(pBleHandle->recvlen > 0)
    {
        int    ret = 0;
        uint16 offset = 0;
        uint16 msglen = 0;
        uint8  *temp = pBleHandle->recv;
        BLE_PRODATA_T *pProData = NULL;
        while(offset < pBleHandle->recvlen)
        {
            if(0x68 == temp[offset])
            {
                memcpy(&msglen, temp + offset + 1, 2);
                if(offset + BLE_PROTOCOL_LEN_MIN + msglen <= pBleHandle->recvlen)
                {
                    ret = ble_protocol_split(temp + offset, BLE_PROTOCOL_LEN_MIN + msglen, &pProData);
                    if(NULL != pProData)
                    {
                        /* 消息处理 */
                        ble_dev_recvmsg_proc(pBleHandle->devfd, pProData, pBleHandle, pBledata);
                        free(pProData);
                        pProData = NULL;

                        offset += BLE_PROTOCOL_LEN_MIN + msglen - 1;
                    }
                    else
                    {
                        BLE_BUF_DEBUG(&temp[offset], pBleHandle->recvlen - offset, "报文解析出错 错误码[%d]", ret);
                    }
                }
                else
                {
                    BLE_BUF_DEBUG(&temp[offset], pBleHandle->recvlen - offset, "报文长度出错 长度[%d]", msglen);
                }
            }
            offset++;
        }

        memset(pBleHandle->recv, 0, pBleHandle->recvlen);
        pBleHandle->recvlen = 0;
    }

    /* 透传处理,比较大的消息串口一次接收多个消息进行组合 */
    ble_dev_msgreport_later(pBleHandle, pBledata);
    
    return 1000;
}

