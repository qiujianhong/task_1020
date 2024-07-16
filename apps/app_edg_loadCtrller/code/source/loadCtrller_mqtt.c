/*
*********************************************************************
* Copyright(C) 2024 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���ɿ���APP���ݳ�ʼ��
* @date��    2024-06-05
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "framecomm.h"

#include "loadCtrller_data.h"
#include "loadCtrller_mqtt.h"

// ������Ϣ �ͻ��˾��
MQTT_CLIENT_T *g_LcMsgClient = NULL;

#define LCTRL_SUBTOPIC      "+/M-loadCtrller"
#define LCTRL_SUBDB_TOPIC   "M-dbCebter/Broadcast/0005/#"
#define LCTRL_SUBMOD_TOPIC  "M-mapManager/Broadcast/0004/0001"
#define LCTRL_SUBGUI_TOPIC  "M-desktopGui/Broadcast/0007/#"
#define LCTRL_SUBSMI_TOPIC  "M-smiOS/Broadcast/0000/#"


pthread_mutex_t   g_LcSendlock; 
pthread_mutex_t   g_LcIndexlock; 
uint16            g_LcMsgindex = 0;

uint8             g_bDBCenter = FALSE;

/**********************************************************************
* @name      : lctrl_mqtt_init
* @brief     ��������Ϣ��ʼ��MQTT
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-5
* @Update    :
**********************************************************************/
int lctrl_mqtt_init(uint32 ip, uint16 port)
{
    int   ret = 0;
    uint8 status = 0;
    char  mqttaddr[16] = {0};
    SUB_TOPIC_T subtopic[5];
    char  mqttstatus[2][16] = {"connectlost", "success"};

    sprintf(mqttaddr, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);

    /* �½��ͻ��� */
    g_LcMsgClient = mqtt_client_create(LCTRL_APP_NAME);
    if (NULL == g_LcMsgClient)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_create fail\n");
        return -1;
    }

    /* �������� */
    memset(&subtopic, 0, sizeof(subtopic));
    subtopic[0].qos = 0;
    memcpy(subtopic[0].topic, LCTRL_SUBTOPIC, strlen(LCTRL_SUBTOPIC));
    subtopic[1].qos = 0;
    memcpy(subtopic[1].topic, LCTRL_SUBDB_TOPIC, strlen(LCTRL_SUBDB_TOPIC));
    subtopic[2].qos = 0;
    memcpy(subtopic[2].topic, LCTRL_SUBMOD_TOPIC, strlen(LCTRL_SUBMOD_TOPIC));
    subtopic[3].qos = 0;
    memcpy(subtopic[3].topic, LCTRL_SUBGUI_TOPIC, strlen(LCTRL_SUBGUI_TOPIC));
    subtopic[4].qos = 0;
    memcpy(subtopic[4].topic, LCTRL_SUBSMI_TOPIC, strlen(LCTRL_SUBSMI_TOPIC));

    ret = mqtt_client_set_sub_topic(g_LcMsgClient, subtopic, 5);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_sub_topic fail, ret %d\n", ret);
        mqtt_client_destroy(g_LcMsgClient);
        return ret;
    }

    /* ����Ĭ������ */
    ret = mqtt_client_set_default_send_topic(g_LcMsgClient, "M-loaderAdjuster/M-smiOS", 0);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("mqtt_client_set_default_send_topic fail, ret %d\n", ret);
        mqtt_client_destroy(g_LcMsgClient);
        return ret;
    }

    /* ���ӷ���� */
    MQTTMSG_FMT_DEBUG("����mqtt����� %s:%d\n", mqttaddr, port);
    ret = mqtt_client_connect(g_LcMsgClient, mqttaddr, port, "127.0.0.1");
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("����mqtt���������ʧ�� ret %d\n", ret);
        mqtt_client_destroy(g_LcMsgClient);
        return ret;
    }

    /* �ȴ����ӳɹ� */
    while(1)
    {
        status = mqtt_client_get_status(g_LcMsgClient);
        MQTTMSG_FMT_DEBUG("mqtt���������״̬: %s\n", mqttstatus[status]);
        if(MQTT_CONNECT_SUCC == status)
        {
            break;
        }
        sleep(1);
    }
    MQTTMSG_FMT_DEBUG("����mqtt����˳ɹ�\n");

    pthread_mutex_init(&g_LcSendlock, NULL);
    pthread_mutex_init(&g_LcIndexlock, NULL);
    return 0;
}

/**********************************************************************
* @name      : lctrl_get_next_index
* @brief     ��������Ϣindex����
* @param[in] ��
* @param[out]��
* @return    ��index
* @Create    : ����
* @Date      ��2024-6-5
* @Update    :
**********************************************************************/
uint16 lctrl_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE((void *)&g_LcMsgClient);
    curindex = g_LcMsgindex;
    g_LcMsgindex++;
    PTHREAD_UNLOCK_SAFE((void *)&g_LcMsgClient);

    return curindex;
}

/**********************************************************************
* @name      : lctrl_send_msg
* @brief     ��������Ϣ����
* @param[in] ��char *pTopic              ָ������, NULLʹ��Ĭ������
               MESSAGE_INFO_T *pMsgInfo  ������Ϣ
               int qos                   ��Ϣ����
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-5
* @Update    :
**********************************************************************/
int lctrl_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
{
    int    ret = 0;
    uint8 *pMsg = NULL;
    uint16 msglen = 0;
    
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    
    msglen = appmsg_package(pMsgInfo, &pMsg);
    if(0 == msglen)
    {
        MQTTMSG_FMT_DEBUG("App send appmsg_package fail\n");
        return -1;
    }

    /* ���˶���MQTT */
    PTHREAD_LOCK_SAFE(&g_LcSendlock);
    if(NULL != pTopic)
    {
        ret = mqtt_client_send_msg(g_LcMsgClient, pTopic, qos, pMsg, (int)msglen);
    }
    else
    {
        ret = mqtt_client_send(g_LcMsgClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_LcSendlock);

    if(0 == ret)
    {
        MQTTMSG_BUF_TRACE(pMsg, msglen, "Send msg to %s:", pMsgInfo->destaddr);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                            pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}

/**********************************************************************
* @name      : lctrl_send_errmsg
* @brief     ��������Ϣ���ʹ������Ϣ
* @param[in] ��MESSAGE_INFO_T *pRecvMsg  �յ�����Ϣ
               int errCode               ������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-5
* @Update    :
**********************************************************************/
int lctrl_send_errmsg(MESSAGE_INFO_T *pRecvMsg, int16 errCode)
{
    char         uTopic[256] = {0};
    uint8           buf[360] = {0};
    uint16            msglen = 0;
    uint8             *temp = NULL;
    MESSAGE_INFO_T *ptMsgNew = (MESSAGE_INFO_T*)buf;

    if(NULL == pRecvMsg)
    {
        return -1;
    }
    
    /* ���췵����Ϣ */
    ptMsgNew->rpm = 1;
    ptMsgNew->priority = pRecvMsg->priority;
    ptMsgNew->index = pRecvMsg->index;
    ptMsgNew->label = pRecvMsg->label;

    ptMsgNew->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(ptMsgNew->souraddr, LCTRL_APP_NAME, ptMsgNew->sourlen);

    memcpy(ptMsgNew->destaddr, pRecvMsg->souraddr, pRecvMsg->sourlen);
    ptMsgNew->destlen = pRecvMsg->sourlen;

    ptMsgNew->IID = MESSAGE_IID_COMMON;
    ptMsgNew->IOP = COMMON_IOP_ERRMSG;

    temp = ptMsgNew->playload;
    appmsg_memcpy_r(temp, &(pRecvMsg->IID), 2);
    msglen += 2;
    appmsg_memcpy_r(temp + msglen, &(pRecvMsg->IOP), 2);
    msglen += 2;
    appmsg_memcpy_r(temp + msglen, &errCode, 2);
    msglen += 2;

    ptMsgNew->msglen = msglen;
    
    sprintf(uTopic, "%s/%s", LCTRL_APP_NAME, pRecvMsg->souraddr);
    return lctrl_send_msg(uTopic, ptMsgNew, 0);
}

/**********************************************************************
* @name      : lctrl_recv_msg
* @brief     ��������Ϣ���� 1s����ʱ��
* @param[in] ��uint16 buflen    �����������
* @param[out]��char *pTopic     ����
               uint8  *pBuf     ��Ϣ����
* @return    ��>= 0 ��Ϣ����/ < 0 ������
* @Create    : ����
* @Date      ��2024-6-5
* @Update    :
**********************************************************************/
int lctrl_recv_msg(char *pTopic, uint8 *pBuf, uint16 buflen)
{
    int qos;
    return mqtt_client_recv_msg(g_LcMsgClient, pTopic, 256, &qos, pBuf, (int)buflen, 1000);
}

/**********************************************************************
* @name      : lctrl_login_to_smios
* @brief     ��APPע��
* @param[in] ��uint8 *pAppId   APPID
               uint8 appidlen  ����
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-5
* @Update    : 
**********************************************************************/
int lctrl_login_to_smios(uint8 *pAppId, uint8 appidlen)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint8 request[160] = {0};
    uint8 offset = 0;
    uint8 len = 0;
    pid_t pid = getpid();
    Date_Time_S tTime;
    char *smios = "M-smiOS";

    app_get_maketime(APP_PUBLISH_TIME, &tTime);

    /* ���̺� */
    appmsg_memcpy_r(request, &pid, sizeof(pid_t));
    offset += 4;

    /* APP���� */
    len = strlen(LCTRL_APP_NAME);
    request[offset++] = len;
    memcpy(&request[offset], LCTRL_APP_NAME, len);
    offset += len;

    /* �����¼� */
    request[offset++] = 0;

    /* �汾��Ϣ */
    request[offset++] = strlen(APP_VERSION);
    memcpy(request + offset, APP_VERSION, strlen(APP_VERSION));
    offset += strlen(APP_VERSION);

    /* �������� */
    request[offset++] = tTime.year_h;
    request[offset++] = tTime.year_l;
    request[offset++] = tTime.month;
    request[offset++] = tTime.day;
    request[offset++] = tTime.hour;
    request[offset++] = tTime.min;
    request[offset++] = tTime.sec;

    /* APPID */
    request[offset++] = appidlen;
    if(appidlen > 0)
    {
        memcpy(request + offset, pAppId, appidlen);
    }
    offset += appidlen;
    
    pMsgInfo = (MESSAGE_INFO_T*)malloc(sizeof(MESSAGE_INFO_T) + offset);
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    memset(pMsgInfo, 0, sizeof(MESSAGE_INFO_T) + offset);
    
    pMsgInfo->rpm = 1;
    pMsgInfo->index = lctrl_get_next_index();
    pMsgInfo->priority = 0;
    pMsgInfo->label = 0;
    pMsgInfo->IID = MESSAGE_IID_SYSTEM;
    pMsgInfo->IOP = SYSTEM_IOP_APP_REGISTER;
    
    pMsgInfo->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(pMsgInfo->souraddr , LCTRL_APP_NAME, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = offset;
    memcpy(pMsgInfo->playload, request, offset);
    
    ret = lctrl_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : lctrl_heart_bear
* @brief     ������Ӧ��
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ���
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-5
* @Update    :
**********************************************************************/
int lctrl_heart_bear(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16 len = 0;
    uint32 status = 1;
    char *smios = "M-smiOS";

    len = sizeof(MESSAGE_INFO_T) + sizeof(uint32);
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 0;
    pMsgInfo->index = pInMsg->index;
    pMsgInfo->priority = pInMsg->priority;
    pMsgInfo->label = pInMsg->label;
    pMsgInfo->IID = MESSAGE_IID_COMMON;
    pMsgInfo->IOP = pInMsg->IOP;
    
    pMsgInfo->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(pMsgInfo->souraddr , LCTRL_APP_NAME, pMsgInfo->sourlen);
    pMsgInfo->destlen = strlen(smios);
    memcpy(pMsgInfo->destaddr, smios, pMsgInfo->destlen);

    pMsgInfo->msglen = sizeof(uint32);
    appmsg_memcpy_r(pMsgInfo->playload, &status, sizeof(uint32));
    
    ret = lctrl_send_msg(NULL, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : lctrl_send_dbmsg
* @brief     �����ⷢ���������Ľӿ�
* @param[in] ��uint16           label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]��uint16*          index
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-6
* @Update    :
**********************************************************************/
int lctrl_send_dbmsg(uint16 label, uint16 iop, uint8* playload, uint16 length, uint16* pIndex)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};

    if(FALSE == g_bDBCenter)
    {
        return -1;
    }
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    *pIndex = lctrl_get_next_index();
    pMsgInfo->index = *pIndex;
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(pMsgInfo->souraddr, LCTRL_APP_NAME, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen("M-dbCenter");
    memcpy(pMsgInfo->destaddr, "M-dbCenter", pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_DB;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/M-dbCenter", LCTRL_APP_NAME);
    ret = lctrl_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : lctrl_send_deskmsg
* @brief     �����ⷢ������
* @param[in] ��uint16           label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]��uint16*          index
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-7-16
* @Update    :
**********************************************************************/
int lctrl_send_deskmsg(uint16 label, uint16 iop, uint8* playload, uint16 length)
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 0;

    pMsgInfo->index = lctrl_get_next_index();
    pMsgInfo->label = label;
    
    pMsgInfo->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(pMsgInfo->souraddr, LCTRL_APP_NAME, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen("M-desktoGui");
    memcpy(pMsgInfo->destaddr, "M-desktoGui", pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_DESKGUI;
    pMsgInfo->IOP = iop;

    pMsgInfo->msglen = length;
    memcpy(pMsgInfo->playload, playload, length);

    sprintf(uTopic, "%s/M-desktoGui", LCTRL_APP_NAME);
    ret = lctrl_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
    return ret;
}

/**********************************************************************
* @name      : lctrl_get_dbCenter
* @brief     ����ȡ��������
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-6
* @Update    :
**********************************************************************/
void lctrl_get_dbCenter()
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T);
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = lctrl_get_next_index();
    pMsgInfo->label = 0;
    
    pMsgInfo->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(pMsgInfo->souraddr, LCTRL_APP_NAME, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen("M-dbCenter");
    memcpy(pMsgInfo->destaddr, "M-dbCenter", pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_COMMON;
    pMsgInfo->IOP = COMMON_IOP_VERSION;

    pMsgInfo->msglen = 0;
    sprintf(uTopic, "%s/%s", LCTRL_APP_NAME, "M-dbCenter");
    lctrl_send_msg(uTopic, pMsgInfo, 0);
    free(pMsgInfo);
}

/**********************************************************************
* @name      : lctrl_get_mapManager
* @brief     ����ȡģ�������
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-6
* @Update    :
**********************************************************************/
void lctrl_get_mapManager()
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T);
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = lctrl_get_next_index();
    pMsgInfo->label = 0;
    
    pMsgInfo->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(pMsgInfo->souraddr, LCTRL_APP_NAME, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen("M-mapManager");
    memcpy(pMsgInfo->destaddr, "M-mapManager", pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_MODULE;
    pMsgInfo->IOP = COMMON_IOP_VERSION;

    pMsgInfo->msglen = 0;
    sprintf(uTopic, "%s/%s", LCTRL_APP_NAME, "M-mapManager");
    lctrl_send_msg(uTopic, pMsgInfo, 0);
}

/**********************************************************************
* @name      : lctrl_get_allmodule
* @brief     ����ȡģ����Ϣ
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-7-1
* @Update    :
**********************************************************************/
void lctrl_get_allmodule()
{
    MESSAGE_INFO_T *pMsgInfo = NULL;
    uint16          infolen = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T);
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 1;
    pMsgInfo->priority = 3;

    pMsgInfo->index = lctrl_get_next_index();
    pMsgInfo->label = 0;
    
    pMsgInfo->sourlen = strlen(LCTRL_APP_NAME);
    memcpy(pMsgInfo->souraddr, LCTRL_APP_NAME, pMsgInfo->sourlen);

    pMsgInfo->destlen = strlen("M-mapManager");
    memcpy(pMsgInfo->destaddr, "M-mapManager", pMsgInfo->destlen);

    pMsgInfo->IID = MESSAGE_IID_MODULE;
    pMsgInfo->IOP = MODULE_IOP_GET_ALL;

    pMsgInfo->msglen = 0;
    sprintf(uTopic, "%s/%s", LCTRL_APP_NAME, "M-mapManager");
    lctrl_send_msg(uTopic, pMsgInfo, 0);
}

/**********************************************************************
* @name      : lctrl_dbversion_msg
* @brief     ����ȡ�������İ汾
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ���
               void *pPublic          ��������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2024-6-6
* @Update    :
**********************************************************************/
int lctrl_version_msg(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    LCTRL_DATA_T     *ptdata = (LCTRL_DATA_T*)pPublic;
    
    if(NULL == pInMsg || NULL == pArg)
    {
        return -1;
    }
    
    if(NULL != strstr(pInMsg->souraddr, "dbCenter"))
    {
        char   version[64] = {0};
        uint8 *temp = pInMsg->playload;
        uint8  len = *temp;
        temp++;
        memcpy(version, temp, MIN(len, 63));
        MQTTMSG_FMT_DEBUG("��ȡ����������, �汾%s\n", version);
        g_bDBCenter = TRUE;
        ptdata->hasDB = TRUE;
    }
    if(NULL != strstr(pInMsg->souraddr, "mapManager"))
    {
        char   version[64] = {0};
        uint8 *temp = pInMsg->playload;
        uint8  len = *temp;
        temp++;
        memcpy(version, temp, MIN(len, 63));
        MQTTMSG_FMT_DEBUG("��ȡ��ģ�������, �汾%s\n", version);
        ptdata->hasMap = TRUE;
    }
    
    return 0;
}

/**********************************************************************
* @name      : lctrl_send_backmsg
* @brief     ������Ӧ����Ϣ
* @param[in] ��uint16           label
               uint16          iop
               uint8*          playload
               uint16          length
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2023-2-8
* @Update    :
**********************************************************************/
int lctrl_send_backmsg(MESSAGE_INFO_T *pInMsg, uint8* playload, uint16 length)
{
    uint8           buf[2048] = {0};
    MESSAGE_INFO_T *pMsgInfo = (MESSAGE_INFO_T *)buf;
    uint16          infolen = 0;
    int             ret = 0;
    char            uTopic[256] = {0};
    
    infolen = sizeof(MESSAGE_INFO_T) + length;
    pMsgInfo = (MESSAGE_INFO_T *)malloc(infolen);
    if(NULL == pMsgInfo)
    {
        MQTTMSG_FMT_DEBUG("Send mqtt msg malloc %d failed\n", infolen);
        return -1;
    }
    memset(pMsgInfo, 0, infolen);
    pMsgInfo->rpm = 0;
    pMsgInfo->priority = pInMsg->priority;

    pMsgInfo->index = pInMsg->index;
    pMsgInfo->label = pInMsg->label;
    
    pMsgInfo->sourlen = pInMsg->destlen;
    memcpy(pMsgInfo->souraddr, pInMsg->destaddr, pInMsg->destlen);

    pMsgInfo->destlen = pInMsg->sourlen;
    memcpy(pMsgInfo->destaddr, pInMsg->souraddr, pInMsg->sourlen);

    pMsgInfo->IID = pInMsg->IID;
    pMsgInfo->IOP = pInMsg->IOP;

    pMsgInfo->msglen = length;
    if(length > 0)
    {
        memcpy(pMsgInfo->playload, playload, length);
    }
    sprintf(uTopic, "%s/%s", pMsgInfo->souraddr, pMsgInfo->destaddr);
    ret = lctrl_send_msg(uTopic, pMsgInfo, 0);
    return ret;
}

