/*
*********************************************************************
* Copyright(C) 2021 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ����APP MQTT��Ϣ����
* @date��    2021-7-8
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "task_base.h"
#include "appmsg_split.h"
#include "framecomm.h"

#include "ble_data.h"
#include "ble_msg.h"
#include "ble_mqtt.h"

pthread_mutex_t  g_MsgIndexlock; 
uint16           g_AppMsgindex = 0;

#if DESC("AXDR������", 1)
/**********************************************************************
* @name      : ble_login_to_smios
* @brief     ��APPע��
* @param[in] ��uint8 *pAppId   APPID
               uint8 appidlen  ����
* @param[out]��
* @return    ��0-�ɹ�/������ 
**********************************************************************/
int ble_login_to_smios(uint8 *pAppId, uint8 appidlen)
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
    char              *srcApp = "M-btManager";

    app_get_maketime(APP_PUBLISH_TIME, &tTime);

    /* ���̺� */
    appmsg_memcpy_r(temp, &pid, sizeof(pid_t));
    msglen += 4;

    /* APP���� */
    len = strlen(srcApp);
    temp[msglen++] = len;
    memcpy(temp + msglen, srcApp, len);  
    msglen += len;

    /* �����¼� */
    temp[msglen++] = 0;

    /* �汾��Ϣ */
    len = strlen(g_AppVer);
    temp[msglen++] = len;
    memcpy(temp + msglen, g_AppVer, len);  
    msglen += len;

    /* �������� */
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
    ptAXDR->index = ble_get_next_index();
    ptAXDR->priority = 0;
    ptAXDR->label = 0;
    ptAXDR->IID = MESSAGE_IID_SYSTEM;
    ptAXDR->IOP = SYSTEM_IOP_APP_REGISTER;
    
    ptAXDR->sourlen = strlen(srcApp);
    memcpy(ptAXDR->souraddr , srcApp, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(smios);
    memcpy(ptAXDR->destaddr, smios, ptAXDR->destlen);

    ptAXDR->msglen = msglen;
    
    return ble_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : ble_heart_bear
* @brief     ������Ӧ��
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ˽�о��
               void *pPublic          ��������
* @param[out]��
* @return    ��0-�ɹ�/������
**********************************************************************/
int ble_heart_bear_to_smios(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pArg, BLE_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T *ptAXDR = &(ptMsgNew->tAXDR);
    uint32 status = 1;
    char   *smios = "M-smiOS";
    char   *srcAPP = "M-btManager";

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
    
    return ble_send_msg(ptMsgNew, 0);
}

/**********************************************************************
* @name      : ble_get_versionmsg
* @brief     ����ȡ�汾
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int ble_get_versionmsg(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    MESSAGE_INFO_T   *ptAXDR = (MESSAGE_INFO_T*)buf;
    uint8              *temp = ptAXDR->playload;
    uint32           version = ble_stoi(APP_VERSION);
    Date_Time_S       tTime;

    if(NULL == pInMsg || NULL == pHandle)
    {
        return -1;
    }
    
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    /* �汾��Ϣ */
    appmsg_memcpy_r(temp + msglen, &version, sizeof(uint32));
    msglen += sizeof(uint32);

    /* �������� */
    app_get_maketime(APP_PUBLISH_TIME, &tTime);
    temp[msglen++] = tTime.year_h;
    temp[msglen++] = tTime.year_l;
    temp[msglen++] = tTime.month;
    temp[msglen++] = tTime.day;
    temp[msglen++] = tTime.hour;
    temp[msglen++] = tTime.min;
    temp[msglen++] = tTime.sec;
    ptAXDR->msglen = msglen;
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_send_msg_req
* @brief     ��������Ϣ����
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-10
* @Update    :
**********************************************************************/
int ble_send_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    BLE_MSG_T       tMsgSend;
    uint8           bytesnum = 0;
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ������Ϣʧ��\n");
        goto end;
    }

    memset(&tMsgSend, 0, sizeof(BLE_MSG_T));
    /* ��Ϣ���� */
    MEMCPY_R(&tMsgSend.port, temp, 4);
    if(tMsgSend.port >= BLE_PORT_MAX)
    {
        MQTTMSG_FMT_DEBUG("��Ч�˿� %d\n", tMsgSend.port);
        goto end;
    }
    
    temp += 4;
    GET_LEN_OFFSET(msglen, temp, bytesnum);
    if(4 + msglen + bytesnum != pInMsg->msglen)
    {
        MQTTMSG_FMT_DEBUG("��Ϣ����\n");
        goto end;
    }
    temp += bytesnum;

    pQueue = &(pPublic->tDevPorc.msgQueue);
    while(1)
    {
        tMsgSend.len = MIN(msglen, BLE_MSG_MAX_LEN);
        memcpy(tMsgSend.msg, temp, tMsgSend.len);
        
        pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_MSG_T));
        if(NULL == pMsgDev)
        {
            MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
            goto end;
        }
        memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_MSG_T));
        pMsgDev->pNext = NULL;
        pMsgDev->priority = pInMsg->priority;
        pMsgDev->type = MESSAGE_TYPE_A_XDR;
        pMsgDev->token = pInMsg->index;
        pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
        memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
        pMsgDev->proc = BLE_IOP_SEND_DATA;
        pMsgDev->datalen = sizeof(BLE_MSG_T);
        memcpy(pMsgDev->data, &tMsgSend, pMsgDev->datalen);
        
        /* д����Ϣ���� */
        ble_write_queue(pQueue, pMsgDev, TRUE);

        /* �ж��Ƿ���� */
        msglen -= tMsgSend.len;
        temp += tMsgSend.len;
        if(0 == msglen)
        {
            break;
        }
    }

    return 0;

end:
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = 0;
    ptAXDR->msglen = msglen;
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_trans_msg_req
* @brief     ��͸����Ϣ����
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-10
* @Update    :
**********************************************************************/
int ble_trans_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    BLE_TRANS_REQ_T tMsgTrans;
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
    int             ret = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ������Ϣʧ��\n");
        goto end;
    }

    memset(&tMsgTrans, 0, sizeof(BLE_TRANS_REQ_T));
    /* ��Ϣ���� */
    ret = ble_trans_req_split(pInMsg->playload, pInMsg->msglen, &tMsgTrans);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("����ת����Ϣ����ʧ��\n");
        goto end;
    }

    /* �Ϸ����ж� ��֧������ͨ��ʹ��͸�� */
    if(tMsgTrans.port > BLE_PORT_MAX || tMsgTrans.port <= BLE_PORT_MASTER1)
    {
        MQTTMSG_FMT_DEBUG("��Ч�˿� %d\n", tMsgTrans.port);
        goto end;
    }
    if(tMsgTrans.match.mode > BLE_MATCH_OB)
    {
        MQTTMSG_FMT_DEBUG("���ģʽ����%d\n", tMsgTrans.match.mode);
        goto end;
    }
    if(0 == tMsgTrans.waittime.msgtime)
    {
        /* Ĭ�ϵȴ�500ms */
        tMsgTrans.waittime.msgtime = 500;
    }
    if(0 == tMsgTrans.waittime.bytetime)
    {
        tMsgTrans.waittime.bytetime = 20;
    }
    
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_TRANS_REQ_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_TRANS_REQ_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = pInMsg->priority;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
    pMsgDev->proc = BLE_IOP_TRAN_DATA;
    pMsgDev->datalen = sizeof(BLE_TRANS_REQ_T);
    memcpy(pMsgDev->data, &tMsgTrans, pMsgDev->datalen);
    
    /* д����Ϣ���� */
    pQueue = &(pPublic->tDevPorc.transQueue);
    ble_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

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
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_setcfg_msg_req
* @brief     �����ò�������
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-12
* @Update    :
**********************************************************************/
int ble_setcfg_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    BLE_CONFIG_T    tConfig;
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
    int             ret = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ��������ʧ��\n");
        goto end;
    }

    memset(&tConfig, 0, sizeof(BLE_CONFIG_T));
    /* ��Ϣ���� */
    ret = ble_config_split(pInMsg->playload, pInMsg->msglen, &tConfig);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("�����������ý���ʧ��\n");
        goto end;
    }
    
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CONFIG_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CONFIG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = pInMsg->priority;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
    pMsgDev->proc = BLE_IOP_SET_CONFIG;
    pMsgDev->datalen = sizeof(BLE_CONFIG_T);
    memcpy(pMsgDev->data, &tConfig, pMsgDev->datalen);
    
    /* д����Ϣ���� */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    ble_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = 0;
    ptAXDR->msglen = msglen;
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_getcfg_msg_req
* @brief     ����ѯ��������
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-12
* @Update    :
**********************************************************************/
int ble_getcfg_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ������ѯʧ��\n");
        return -1;
    }

    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    ptAXDR->msglen = ble_config_package(&(pPublic->tDevStatus.config), ptAXDR->playload, 128);
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_setport_msg_req
* @brief     ���˿���������
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-12
* @Update    :
**********************************************************************/
int ble_setport_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    BLE_PORTS_CONF_T tPort;
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
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
        MQTTMSG_FMT_DEBUG("�����豸������, ��������ʧ��\n");
        goto end;
    }

    memset(&tPort, 0, sizeof(BLE_PORTS_CONF_T));
    /* ��Ϣ���� */
    ret = ble_port_config_split(pInMsg->playload, pInMsg->msglen, &tPort);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("�����˿����ý���ʧ��\n");
        goto end;
    }

    /* ������� */
    if(0 == tPort.num)
    {
        MQTTMSG_FMT_DEBUG("�������˿�����\n");
        goto end;
    }
    for(i = 0; i < tPort.num; i++)
    {
        if(tPort.tConf[i].port >= BLE_PORT_MAX)
        {
            MQTTMSG_FMT_DEBUG("�����˿ڳ���31\n");
            goto end;
        }
        if((tPort.tConf[i].port <= BLE_PORT_MASTER1 && BLE_MODE_SLAVE == tPort.tConf[i].mode) ||
           (tPort.tConf[i].port > BLE_PORT_MASTER1 && BLE_MODE_MASTER == tPort.tConf[i].mode) || 
           tPort.tConf[i].mode > 1)
        {
            MQTTMSG_FMT_DEBUG("�����˿����ô���\n");
            goto end;
        }
        if((tPort.tConf[i].port > BLE_PORT_MASTER1 && BLE_MODE_SLAVE == tPort.tConf[i].mode))
        {
            if(0 == memcmp(tPort.tConf[i].MAC, MAC1, 6) || 0 == memcmp(tPort.tConf[i].MAC, MAC2, 6))
            {
                MQTTMSG_FMT_DEBUG("�����˿����ô���\n");
                goto end;
            }
        }
    }
    
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_PORTS_CONF_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CONFIG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = pInMsg->priority;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
    pMsgDev->proc = BLE_IOP_SET_PORT;
    pMsgDev->datalen = sizeof(BLE_PORTS_CONF_T);
    memcpy(pMsgDev->data, &tPort, pMsgDev->datalen);
    
    /* д����Ϣ���� */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    ble_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = 0;
    ptAXDR->msglen = msglen;
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_getlink_msg_req
* @brief     ������������Ϣ����
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-12
* @Update    :
**********************************************************************/
int ble_getlink_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8              buf[1024] = {0};
    MESSAGE_INFO_T    *ptAXDR = (MESSAGE_INFO_T*)buf; 
    BLE_CONNECT_INFO_T tLink;
    BLE_DEV_T         *ptDevStatus = &(pPublic->tDevStatus);
    uint32             i = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, �˿ڲ�ѯʧ��\n");
        return -1;
    }

    memset(&tLink, 0, sizeof(BLE_CONNECT_INFO_T));
    for(i = 0; i < BLE_PORT_MAX; i++)
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
    
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    ptAXDR->msglen = ble_connect_package(&tLink, ptAXDR->playload, 900);
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_getsn_msg_req
* @brief     ������������Ϣ����
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-12
* @Update    :
**********************************************************************/
int ble_getsn_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8              buf[1024] = {0};
    MESSAGE_INFO_T    *ptAXDR = (MESSAGE_INFO_T*)buf; 
    BLE_SN_T tSn;
    BLE_DEV_T         *ptDevStatus = &(pPublic->tDevStatus);
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, �˿ڲ�ѯʧ��\n");
        return -1;
    }

    memcpy(&tSn, &ptDevStatus->sn, sizeof(tSn));
    
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    ptAXDR->msglen = ble_sn_package(&tSn, ptAXDR->playload, 900);
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_setcfg_msg_req
* @brief     �����ò�������
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-12
* @Update    :
**********************************************************************/
int ble_setsn_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8          *temp = pInMsg->playload;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    uint8           Result = 1;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    BLE_SN_T        tSn;
    int             ret = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ��������ʧ��\n");
        Result = 0;
        goto end;
    }

    memset(&tSn, 0, sizeof(tSn));
    /* ��Ϣ���� */
    ret = ble_sn_split(pInMsg->playload, pInMsg->msglen, &tSn);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("����SN����ʧ��\n");
        Result = 0;
        goto end;
    }

    MQTTMSG_BUF_DEBUG(tSn.sn, tSn.num, "��������SN");
    pPublic->tDevStatus.sn = tSn;
    
end:
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    temp = ptAXDR->playload;
    temp[msglen++] = Result;
    ptAXDR->msglen = msglen;
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_setcheck_msg_req
* @brief     �����������������
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-12
* @Update    :
**********************************************************************/
int ble_setcheck_msg_req(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    BLE_CHECK_T     tcheck;
    uint8           buf[360] = {0};
    uint16          msglen = 0;
    MESSAGE_INFO_T *ptAXDR = (MESSAGE_INFO_T*)buf; 
    uint8           result = 0;
    char           *checkname[] = {"������", "��������", "ʱ��Ͷ��", "����г��", "����г��", "�޹�����", "�й�����"};
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ��������ʧ��\n");
        goto end;
    }

    memset(&tcheck, 0, sizeof(BLE_CHECK_T));
    if(0 != ble_check_split(pInMsg->playload, pInMsg->msglen, &tcheck))
    {
        MQTTMSG_FMT_DEBUG("����������ô���\n");
        goto end;
    }
    if(tcheck.type > BLE_TYPE_ACTIVE && tcheck.type < BLE_TYPE_CLOSE)
    {
        MQTTMSG_FMT_DEBUG("%s�������������%s\n", pInMsg->souraddr, "δ֪");
        goto end;
    }

    MQTTMSG_FMT_DEBUG("%s�����������%s\n", pInMsg->souraddr, 
                      tcheck.type <= BLE_TYPE_ACTIVE ? checkname[tcheck.type] : "�ر�");

    /* д����Ϣ���� */
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CHECK_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CHECK_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = pInMsg->priority;
    pMsgDev->type = MESSAGE_TYPE_A_XDR;
    pMsgDev->token = pInMsg->index;
    pMsgDev->token = (pMsgDev->token << 16) + pInMsg->label;
    memcpy(pMsgDev->source, pInMsg->souraddr, pInMsg->sourlen);
    pMsgDev->proc = BLE_IOP_CHECK_METER;
    pMsgDev->datalen = sizeof(BLE_CHECK_T);
    memcpy(pMsgDev->data, &tcheck, pMsgDev->datalen);
    
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    ble_write_queue(pQueue, pMsgDev, FALSE);

    //�����أ��Ȼ���Ϣ
    #if CHECK_METER_ASYNC == 0
    return 0;
    #else
    //�����ӳټ��뷢��
    pPublic->tDevPorc.cfgDelay = 8*(200/BLE_RECV_TIME_OUT);
    result = 1;
    #endif
end:
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    ptAXDR->playload[msglen++] = result;
    ptAXDR->msglen = msglen;
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

/**********************************************************************
* @name      : ble_get_appid
* @brief     ����ȡAPPID
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int ble_get_appidmsg(MESSAGE_INFO_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    MESSAGE_INFO_T   *ptAXDR = (MESSAGE_INFO_T*)buf;
    uint8              *temp = ptAXDR->playload;

    if(NULL == pInMsg || NULL == pHandle)
    {
        return -1;
    }
    
    /* ���췵����Ϣ */
    ptAXDR->rpm = 0;
    ptAXDR->priority = pInMsg->priority;
    ptAXDR->index = pInMsg->index;
    ptAXDR->label = pInMsg->label;

    ptAXDR->sourlen = strlen(BT_APP_NAME_M);
    memcpy(ptAXDR->souraddr, BT_APP_NAME_M, ptAXDR->sourlen);

    memcpy(ptAXDR->destaddr, pInMsg->souraddr, pInMsg->sourlen);
    ptAXDR->destlen = pInMsg->sourlen;

    ptAXDR->IID = pInMsg->IID;
    ptAXDR->IOP = pInMsg->IOP;

    /* APPID */
    temp[msglen++] = pPublic->appidlen;
    memcpy(temp + msglen, pPublic->appid, pPublic->appidlen);
    msglen += pPublic->appidlen;
    ptAXDR->msglen = msglen;
    
    return ble_send_axdr_msg(ptAXDR, 0);
}

BLE_MSGFUN_T g_tMsgFunc[] = 
{/*  IID,                  IOP,                     ��Ϣ������ */
    {MESSAGE_IID_COMMON,   COMMON_IOP_VERSION,      ble_get_versionmsg},
    {MESSAGE_IID_COMMON,   COMMON_IOP_HERTBEAT,     ble_heart_bear_to_smios},
    {MESSAGE_IID_COMMON,   COMMON_IOP_GETAPPID,     ble_get_appidmsg},

    {MESSAGE_IID_BLE,      BLE_IOP_SEND_DATA,       ble_send_msg_req},
    {MESSAGE_IID_BLE,      BLE_IOP_TRAN_DATA,       ble_trans_msg_req},
    {MESSAGE_IID_BLE,      BLE_IOP_SET_CONFIG,      ble_setcfg_msg_req},
    {MESSAGE_IID_BLE,      BLE_IOP_GET_CONFIG,      ble_getcfg_msg_req},
    {MESSAGE_IID_BLE,      BLE_IOP_SET_PORT,        ble_setport_msg_req},
    {MESSAGE_IID_BLE,      BLE_IOP_LINK_INFO,       ble_getlink_msg_req},
    {MESSAGE_IID_BLE,      BLE_IOP_BT_SN,           ble_getsn_msg_req},
    {MESSAGE_IID_BLE,      BLE_IOP_SET_BT_SN,       ble_setsn_msg_req},
    {MESSAGE_IID_BLE,      BLE_IOP_CHECK_METER,     ble_setcheck_msg_req},
};
uint8 g_nMsgNum = sizeof(g_tMsgFunc)/sizeof(BLE_MSGFUN_T);

/**********************************************************************
* @name      : ble_axdr_msg_proc
* @brief     ��axdr��Ϣ����
* @param[in] ��MESSAGE_INFO_T *pMsgInfo       ��Ϣ
               BLE_MSGHANDLE_T *ptMsgHandle   ˽������
               BLE_DATA_T *ptBLEData          ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
void ble_axdr_msg_proc(MESSAGE_INFO_T *pMsgInfo, BLE_MSGHANDLE_T *ptMsgHandle, BLE_DATA_T *ptBLEData)
{
    uint8 i = 0;
    
    if(0 == strcmp(pMsgInfo->souraddr, BLE_APP_NAME))
    {
        /* �����Է����� */
        MQTTMSG_FMT_DEBUG("Revc self msg, iid %d, iop %d\n", pMsgInfo->IID, pMsgInfo->IOP);
        return;
    }

    /* ע��Ӧ�� */
    if (MESSAGE_IID_SYSTEM == pMsgInfo->IID )
    {
        if (SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
        {
            ptBLEData->bReg = TRUE;
            BLE_FMT_DEBUG("����APPע��\n");
        }
        return;
    }

    for(i = 0; i < g_nMsgNum; i++)
    {
        if(g_tMsgFunc[i].IID == pMsgInfo->IID && g_tMsgFunc[i].IOP == pMsgInfo->IOP)
        {
            if(g_tMsgFunc[i].pProcMsg)
            {
                g_tMsgFunc[i].pProcMsg(pMsgInfo, (void *)ptMsgHandle, (void *)ptBLEData);
                break;
            }
        }
    }
    
    if(i == g_nMsgNum && !(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_ERRMSG == pMsgInfo->IOP))
    {
        ble_send_errmsg(pMsgInfo, BLE_ERR_OTHER_FAIL);
    }
}


#endif

#if DESC("JSON������", 1)

/**********************************************************************
* @name      : ble_json_setparam_req
* @brief     �����ò�������
* @param[in] ��MESSAGE_JSON_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int ble_json_setparam_req(MESSAGE_JSON_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    BLE_CONFIG_T    tConfig;
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
    int             ret = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ��������ʧ��\n");
        goto end;
    }

    memset(&tConfig, 0, sizeof(BLE_CONFIG_T));
    /* ��Ϣ���� */
    ret = ble_setparam_req_split(pInMsg->infopath, pInMsg->pObject, &tConfig);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("�����������ý���ʧ��\n");
        goto end;
    }
    
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CONFIG_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CONFIG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = BLE_IOP_SET_CONFIG;
    pMsgDev->datalen = sizeof(BLE_CONFIG_T);
    memcpy(pMsgDev->data, &tConfig, pMsgDev->datalen);
    
    /* д����Ϣ���� */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    ble_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    return ble_err_json_res(pInMsg, BLE_ERR_OTHER_FAIL, NULL);
}

/**********************************************************************
* @name      : ble_json_getparam_req
* @brief     ����ѯ��������
* @param[in] ��MESSAGE_JSON_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int ble_json_getparam_req(MESSAGE_JSON_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
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
        MQTTMSG_FMT_DEBUG("�����豸������, ������ѯʧ��\n");
        status = BLE_ERR_OTHER_FAIL;
    }

    /* ���췵����Ϣ */
    strmsg = ble_getparam_res_package(pInMsg->token, &(pPublic->tDevStatus.config), status);
    if(strmsg)
    {
        sprintf(topic, "%s/%s/%s/%s/%s/%s", BLE_APP_NAME, pInMsg->sourapp, MQTT_TOPIC_JSON, pInMsg->operat,
                                            MQTT_TOPIC_INFO_RES, pInMsg->infopath);
        ret = ble_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }
    return ret;
}

/**********************************************************************
* @name      : ble_json_setport_req
* @brief     ���˿���������
* @param[in] ��MESSAGE_INFO_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int ble_json_setport_req(MESSAGE_JSON_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{ 
    BLE_PORTS_CONF_T tPort;
    BLE_QUEUE_T     *pQueue = NULL;
    BLE_DEV_MSG_T   *pMsgDev = NULL;
    int              ret = 0;
    uint8            MAC1[6] = {0};
    uint8            MAC2[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ��������ʧ��\n");
        goto end;
    }

    memset(&tPort, 0, sizeof(BLE_PORTS_CONF_T));;
    /* ��Ϣ���� */
    ret = ble_setport_req_split(pInMsg->infopath, pInMsg->pObject, &(tPort.tConf[0]));
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("�����˿����ý���ʧ��\n");
        goto end;
    }

    /* �˿ڼ�� */
    if(tPort.tConf[0].port >= BLE_PORT_MAX)
    {
        MQTTMSG_FMT_DEBUG("�����˿ڳ���31\n");
        goto end;
    }
    if((tPort.tConf[0].port <= BLE_PORT_MASTER1 && BLE_MODE_SLAVE == tPort.tConf[0].mode) ||
       (tPort.tConf[0].port > BLE_PORT_MASTER1 && BLE_MODE_MASTER == tPort.tConf[0].mode) || tPort.tConf[0].mode > 1)
    {
        MQTTMSG_FMT_DEBUG("�����˿����ô���\n");
        goto end;
    }
    if((tPort.tConf[0].port > BLE_PORT_MASTER1 && BLE_MODE_SLAVE == tPort.tConf[0].mode))
    {
        if(0 == memcmp(tPort.tConf[0].MAC, MAC1, 6) || 0 == memcmp(tPort.tConf[0].MAC, MAC2, 6))
        {
            MQTTMSG_FMT_DEBUG("�����˿����ô���\n");
            goto end;
        }
    }
    tPort.num = 1;
    
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_PORTS_CONF_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CONFIG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = BLE_IOP_SET_PORT;
    pMsgDev->datalen = sizeof(BLE_PORTS_CONF_T);
    memcpy(pMsgDev->data, &tPort, pMsgDev->datalen);
    
    /* д����Ϣ���� */
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    ble_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    return ble_err_json_res(pInMsg, BLE_ERR_OTHER_FAIL, NULL);
}

/**********************************************************************
* @name      : ble_json_setcheck_req
* @brief     �����������������
* @param[in] ��MESSAGE_JSON_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int ble_json_setcheck_req(MESSAGE_JSON_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    uint8           errorcode = BLE_ERR_OTHER_FAIL;
    int             ret = 0;
    BLE_CHECK_T     tcheck;
    char           *checkname[] = {"������", "��������", "ʱ��Ͷ��", "����г��", "����г��", "�޹�����", "�й�����"};
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ��������ʧ��\n");
        goto end;
    }

    ret = ble_setdetect_req_split(pInMsg->infopath, pInMsg->pObject, &tcheck);
    if(0 != ret || (tcheck.type > BLE_TYPE_ACTIVE && tcheck.type < BLE_TYPE_CLOSE))
    {
        MQTTMSG_FMT_DEBUG("����������ô���\n");
        goto end;
    }

    MQTTMSG_FMT_DEBUG("%s�����������%s\n", pInMsg->sourapp, 
                      tcheck.type <= BLE_TYPE_ACTIVE ? checkname[tcheck.type] : "�ر�");

    /* д����Ϣ���� */
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CHECK_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_CHECK_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = BLE_IOP_CHECK_METER;
    pMsgDev->datalen = sizeof(BLE_CHECK_T);
    memcpy(pMsgDev->data, &tcheck, pMsgDev->datalen);
    
    pQueue = &(pPublic->tDevPorc.cfgQueue);
    ble_write_queue(pQueue, pMsgDev, FALSE);
    return 0;
end:
    return ble_err_json_res(pInMsg, errorcode, NULL);
}

/**********************************************************************
* @name      : ble_json_getlink_req
* @brief     ������������Ϣ����
* @param[in] ��MESSAGE_JSON_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int ble_json_getlink_req(MESSAGE_JSON_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    BLE_CONNECT_INFO_T tLink;
    BLE_DEV_T         *ptDevStatus = &(pPublic->tDevStatus);
    uint32             i = 0;
    char               topic[256] = {0};
    char              *strmsg = NULL;
    int                ret = 1;
    uint32             status = BLE_ERR_OTHER_FAIL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    memset(&tLink, 0, sizeof(BLE_CONNECT_INFO_T));
    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, �˿ڲ�ѯʧ��\n");
        goto end;
    }

    for(i = 0; i < BLE_PORT_MAX; i++)
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
    
    status = BLE_SUCCESS;
end:
    /* ���췵����Ϣ */
    strmsg = ble_getconnect_res_package(pInMsg->token, &tLink, status);
    if(strmsg)
    {
        sprintf(topic, "%s/%s/%s/%s/%s/%s", pInMsg->destapp, pInMsg->sourapp, MQTT_TOPIC_JSON, pInMsg->operat,
                                            MQTT_TOPIC_INFO_RES, pInMsg->infopath);
        ret = ble_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }
    return ret;
}

/**********************************************************************
* @name      : ble_json_sendmsg_req
* @brief     ��������Ϣ����
* @param[in] ��MESSAGE_JSON_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int ble_json_sendmsg_req(MESSAGE_JSON_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    BLE_MSG_T       tMsgSend;
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
    int             ret = 0;
    uint8           prio = 0;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ������Ϣʧ��\n");
        goto end;
    }

    memset(&tMsgSend, 0, sizeof(BLE_MSG_T));
    /* ��Ϣ���� */
    ret = ble_sendmsg_req_split(pInMsg->infopath, pInMsg->pObject, &tMsgSend, &prio);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("������Ϣ����ʧ��, %d\n", ret);
        goto end;
    }
    if(tMsgSend.port >= BLE_PORT_MAX)
    {
        MQTTMSG_FMT_DEBUG("��Ч�˿� %d\n", tMsgSend.port);
        goto end;
    }
    
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_MSG_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_MSG_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = prio;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = BLE_IOP_SEND_DATA;
    pMsgDev->datalen = sizeof(BLE_MSG_T);
    memcpy(pMsgDev->data, &tMsgSend, pMsgDev->datalen);
    
    /* д����Ϣ���� */
    pQueue = &(pPublic->tDevPorc.msgQueue);
    ble_write_queue(pQueue, pMsgDev, TRUE);

    return 0;
end:
    return ble_err_json_res(pInMsg, BLE_ERR_OTHER_FAIL, NULL);
}

/**********************************************************************
* @name      : ble_json_transmsg_req
* @brief     ��͸����Ϣ����
* @param[in] ��MESSAGE_JSON_T *pInMsg   ������Ϣ
               BLE_MSGHANDLE_T *pHandle ���
               BLE_DATA_T *pPublic      ������Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-8-14
* @Update    :
**********************************************************************/
int ble_json_transmsg_req(MESSAGE_JSON_T *pInMsg, BLE_MSGHANDLE_T *pHandle, BLE_DATA_T *pPublic)
{
    char    topic[256] = {0};
    char   *strmsg = NULL;
    int     ret = 1;
    BLE_TRANS_ACK_T tAck;
    BLE_TRANS_REQ_T tMsgTrans;
    BLE_QUEUE_T    *pQueue = NULL;
    BLE_DEV_MSG_T  *pMsgDev = NULL;
    
    if(NULL == pInMsg || NULL == pHandle || NULL == pPublic)
    {
        return -1;
    }

    if(FALSE == pPublic->bWork)
    {
        MQTTMSG_FMT_DEBUG("�����豸������, ������Ϣʧ��\n");
        goto end;
    }

    memset(&tMsgTrans, 0, sizeof(BLE_TRANS_REQ_T));
    /* ��Ϣ���� */
    ret = ble_transmsg_req_split(pInMsg->infopath, pInMsg->pObject, &tMsgTrans);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("����ת����Ϣ����ʧ��\n");
        goto end;
    }

    /* �Ϸ����ж� ��֧������ͨ��ʹ��͸�� */
    if(tMsgTrans.port > BLE_PORT_MAX || tMsgTrans.port <= BLE_PORT_MASTER1)
    {
        MQTTMSG_FMT_DEBUG("��Ч�˿� %d\n", tMsgTrans.port);
        goto end;
    }
    if(tMsgTrans.match.mode > BLE_MATCH_OB)
    {
        MQTTMSG_FMT_DEBUG("���ģʽ����%d\n", tMsgTrans.match.mode);
        goto end;
    }

    if(0 == tMsgTrans.waittime.msgtime)
    {
        /* Ĭ�ϵȴ�500ms */
        tMsgTrans.waittime.msgtime = 500;
    }
    if(0 == tMsgTrans.waittime.bytetime)
    {
        tMsgTrans.waittime.bytetime = 20;
    }
    
    pMsgDev = (BLE_DEV_MSG_T*)malloc(sizeof(BLE_DEV_MSG_T) + sizeof(BLE_TRANS_REQ_T));
    if(NULL == pMsgDev)
    {
        MQTTMSG_FMT_DEBUG("�����ڴ�ʧ��\n");
        goto end;
    }
    memset(pMsgDev, 0, sizeof(BLE_DEV_MSG_T) + sizeof(BLE_TRANS_REQ_T));
    pMsgDev->pNext = NULL;
    pMsgDev->priority = 0;
    pMsgDev->type = MESSAGE_TYPE_JSON;
    pMsgDev->token = pInMsg->token;
    memcpy(pMsgDev->source, pInMsg->sourapp, strlen(pInMsg->sourapp));
    pMsgDev->proc = BLE_IOP_TRAN_DATA;
    pMsgDev->datalen = sizeof(BLE_TRANS_REQ_T);
    memcpy(pMsgDev->data, &tMsgTrans, pMsgDev->datalen);
    
    /* д����Ϣ���� */
    pQueue = &(pPublic->tDevPorc.transQueue);
    ble_write_queue(pQueue, pMsgDev, FALSE);

    return 0;
end:
    tAck.ret = BLE_ERR_OTHER_FAIL;
    tAck.len = 0;
    strmsg = ble_transmsg_res_package(pInMsg->token, &tAck);
    if(strmsg)
    {
        sprintf(topic, "%s/%s/%s/%s/%s/%s", pInMsg->destapp, pInMsg->sourapp, MQTT_TOPIC_JSON, pInMsg->operat,
                                            MQTT_TOPIC_INFO_RES, pInMsg->infopath);
        ret = ble_send_json_msg(topic, 0, strmsg, strlen(strmsg));
        free(strmsg);
    }
    return ret;
}

/**********************************************************************
* @name      : ble_json_msg_proc
* @brief     ��JSON��Ϣ����
* @param[in] ��MESSAGE_INFO_T *pMsgInfo       ��Ϣ
               BLE_MSGHANDLE_T *ptMsgHandle   ˽������
               BLE_DATA_T *ptBLEData          ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-8-13
* @Update    :
**********************************************************************/
void ble_json_msg_proc(MESSAGE_JSON_T *pMsgInfo, BLE_MSGHANDLE_T *ptMsgHandle, BLE_DATA_T *ptBLEData)
{
    /* ע�� �������� */
    if(0 == strcmp(pMsgInfo->sourapp, SYS_APP_NAME))
    {
        if(0 == strcmp(pMsgInfo->infopath, SYS_TOPIC_REGISTER))
        {
            ptBLEData->bReg = TRUE;
            BLE_FMT_DEBUG("����APPע��\n");
        }
        else if(0 == strcmp(pMsgInfo->infopath, SYS_TOPIC_KEEPALIVE))
        {
            ble_heart_bear(pMsgInfo);
        }
        return;
    }

    /* JSON��Ϣ���� */
    if((0 == strcmp(pMsgInfo->destapp, BLE_APP_NAME) || 0 == strcmp(pMsgInfo->destapp, BT_APP_NAME))
       && 0 == strcmp(pMsgInfo->infotype, MQTT_TOPIC_INFO_REQ))
    {
        if(0 == strcmp(pMsgInfo->infopath, BLE_TOPOC_SETPARAM) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_SET))
        {
            /* ������������ */
            ble_json_setparam_req(pMsgInfo, ptMsgHandle, ptBLEData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, BLE_TOPOC_GETPARAM) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_GET))
        {
            /* ����������ѯ */
            ble_json_getparam_req(pMsgInfo, ptMsgHandle, ptBLEData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, BLE_TOPOC_SETPORT) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_SET))
        {
            /* �˿����� */
            ble_json_setport_req(pMsgInfo, ptMsgHandle, ptBLEData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, BLE_TOPOC_SETDETECT) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_SET))
        {
            /* ������� */
            ble_json_setcheck_req(pMsgInfo, ptMsgHandle, ptBLEData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, BLE_TOPOC_CONNINFO) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_GET))
        {
            /* ����������Ϣ��ѯ */
            ble_json_getlink_req(pMsgInfo, ptMsgHandle, ptBLEData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, BLE_TOPOC_SENDDATA) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_ACT))
        {
            /* ������Ϣ */
            ble_json_sendmsg_req(pMsgInfo, ptMsgHandle, ptBLEData);
        }
        else if(0 == strcmp(pMsgInfo->infopath, BLE_TOPOC_FORWARD) && 0 == strcmp(pMsgInfo->operat, MQTT_TOPIC_OPER_ACT))
        {
            /* ͸�� */
            ble_json_transmsg_req(pMsgInfo, ptMsgHandle, ptBLEData);
        }
        else
        {
            /* ������ϢӦ�� */
            ble_err_json_res(pMsgInfo, BLE_ERR_OTHER_FAIL, "Unsupport operation");
        }
    }
    
    return;
}

#endif

#if DESC("����״̬��ʾ", 1)
/**********************************************************************
* @name      : bel_state_guishow
* @brief     ������״̬��ʾˢ��
* @param[in] ��uint8 status  ��������״̬(����[0],��˸[1],����[2])
* @param[out]��
* @return    ��0-�ɹ�/������ 
**********************************************************************/
int bel_state_guishow(uint8 status)
{
    uint8           buf[420] = {0};
    uint16          msglen = 0;
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_INFO_T *ptAXDR = &(ptMsgNew->tAXDR);
    uint8          *temp = ptAXDR->playload;
    char           *dstApp = "M-desktopGui";
    char           *srcApp = "M-btManager";

    //ˢ�¹�����
    temp[msglen++] = 0x01;
    temp[msglen++] = 0x06;
    temp[msglen++] = status;

    ptMsgNew->msgtype = MESSAGE_TYPE_A_XDR;
    ptAXDR->rpm = 1;
    ptAXDR->index = ble_get_next_index();
    ptAXDR->priority = 0;
    ptAXDR->label = 0;
    ptAXDR->IID = MESSAGE_IID_DESKGUI;
    ptAXDR->IOP = DESKGUI_IOP_FRESH_TOOL;
    
    ptAXDR->sourlen = strlen(srcApp);
    memcpy(ptAXDR->souraddr , srcApp, ptAXDR->sourlen);
    ptAXDR->destlen = strlen(dstApp);
    memcpy(ptAXDR->destaddr, dstApp, ptAXDR->destlen);

    ptAXDR->msglen = msglen;
    
    return ble_send_msg(ptMsgNew, 0);
}
#endif

/**********************************************************************
* @name      : ble_mqttproc_init
* @brief     ��mqtt��Ϣ�����ʼ��
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int ble_mqttproc_init(void *pPublic, void **ppPrivate)
{
    BLE_MSGHANDLE_T *ptMsgData = NULL;
    BLE_DATA_T *ptPData = (BLE_DATA_T*)pPublic;

    ptMsgData = (BLE_MSGHANDLE_T *)malloc(sizeof(BLE_MSGHANDLE_T));
    if(NULL == ptMsgData)
    {
        BLE_FMT_DEBUG("��ʼ��ʧ��\n");
        return -1;
    }
    memset(ptMsgData, 0, sizeof(BLE_MSGHANDLE_T));

    /* �ȷ���һ��APPע����Ϣ */
    #ifdef PRODUCT_SCU
    ble_login_to_sys();
    #else
    ble_login_to_smios(ptPData->appid, ptPData->appidlen);

    //�Ƚ���������״̬����Ϊ����ʾ
    bel_state_guishow(0);
    #endif
    *ppPrivate = ptMsgData;
    return 0;
}

/**********************************************************************
* @name      : ble_mqttmsg_proc
* @brief     ��mqtt��Ϣ����
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int ble_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int         ret = 0;
    int         recvlen = 0;
    BLE_MSGHANDLE_T *ptMsgHandle = (BLE_MSGHANDLE_T*)pPrivate;
    BLE_DATA_T      *ptWireData = (BLE_DATA_T*)pPublic;
    struct timespec  curtime = {0};
    MESSAGE_MQTT_T  *pMsgMqtt = NULL;
    static uint8     sbProcFirst = TRUE;

    if(TRUE == sbProcFirst)
    {
        BASE_TASK_SET_OK(pArg); //�����������׼������
        sbProcFirst = FALSE;
    }
    
    /* 1 ����Ϣ */
    recvlen = ble_recv_mqtt(ptMsgHandle->topic, 256, ptMsgHandle->recvBuf, BLE_MQTT_MSG_LEN);
    if(recvlen > 0)
    {
        ret = appmsg_mqtt_split(ptMsgHandle->recvBuf, (uint16)recvlen, ptMsgHandle->topic, &pMsgMqtt);
        if(0 == ret && NULL != pMsgMqtt)
        {
            if(MESSAGE_TYPE_A_XDR == pMsgMqtt->msgtype)
            {
                /* AXDR��ʽ */
                TASK_SET_USERLOG(pArg, "Msg from %s iid %04x, iop %04x", pMsgMqtt->tAXDR.souraddr, 
                                       pMsgMqtt->tAXDR.IID, pMsgMqtt->tAXDR.IOP);
                MQTTMSG_BUF_TRACE(ptMsgHandle->recvBuf, recvlen, "Recv msg from %s, iid 0x%x, iop 0x%x", 
                                                   pMsgMqtt->tAXDR.souraddr, pMsgMqtt->tAXDR.IID, pMsgMqtt->tAXDR.IOP);
                if(NULL != strstr(pMsgMqtt->tAXDR.souraddr, BLE_APP_NAME))
                {
                    MQTTMSG_FMT_DEBUG("Revc msg from %s, iid 0x%x, iop 0x%x\n", pMsgMqtt->tAXDR.souraddr, 
                                      pMsgMqtt->tAXDR.IID, pMsgMqtt->tAXDR.IOP);
                }
                else
                {
                    ble_axdr_msg_proc(&pMsgMqtt->tAXDR, ptMsgHandle, ptWireData);
                }
            }
            else
            {
                /* JSON��ʽ */
                TASK_SET_USERLOG(pArg, "Msg topic %s", ptMsgHandle->topic);
                MQTTMSG_FMT_TRACE("Recv topic %s\n%s\n", ptMsgHandle->topic, ptMsgHandle->recvBuf);
                if(NULL != strstr(pMsgMqtt->tJson.sourapp, BLE_APP_NAME))
                {
                    MQTTMSG_FMT_DEBUG("Revc msg topic %s is the same name\n", ptMsgHandle->topic);
                }
                else
                {
                    ble_json_msg_proc(&pMsgMqtt->tJson, ptMsgHandle, ptWireData);
                }
            }

            /* ��Դ�ͷ� */
            appmsg_mqtt_free(pMsgMqtt);
            pMsgMqtt = NULL;
            memset(ptMsgHandle->recvBuf, 0, recvlen);
            memset(ptMsgHandle->topic, 0, 256);
        }
    }

    /* 2. ��ʱ���� */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(curtime.tv_sec - ptMsgHandle->timeflag < 5)
    {
        return 0;
    }
    ptMsgHandle->timeflag = curtime.tv_sec;
    /* APP��� */
    if(FALSE == ptWireData->bReg)
    {
        #ifdef PRODUCT_SCU
        ble_login_to_sys();
        #else
        ble_login_to_smios(ptWireData->appid, ptWireData->appidlen);
        #endif
    }
    
    return 0;
}


