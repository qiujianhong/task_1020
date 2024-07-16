/*
*********************************************************************
* Copyright(C) 2020 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ��·���mqtt
* @date��    2020-6-30
* @history�� 
*********************************************************************
*/
#include "baseCommon.h"
#include "framecomm.h"
#include "task_msg.h"
#include "db_api.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "task_base.h"
#include "task_id.h"
#include "msg_id.h"
#include "cJSON.h"

#include "sec_comm.h"
#include "sec_mqtt.h"
#include "sec_msg.h"
#include "sec_mgr.h"

uint8  g_SecMqttDebug = 0;
extern char  g_sec_datafile[256];
extern SEC_SESSION_LIMIT_T g_session_limit;
extern SEC_SRVTIME_LIMIT_T g_srvtime_limit;
extern SEC_SESSIONAPPS_T  g_session_info;
extern uint32 g_sshtime_limit;
/**********************************************************************
* @name      : sec_mqtt_debug_open
* @brief     ��mqtt���Խӿڴ�
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-3
* @Update    :
**********************************************************************/
void sec_mqtt_debug_open()
{
    sec_mqtt_debug();
    g_SecMqttDebug = 1;
}

/**********************************************************************
* @name      : sec_mqtt_debug_close
* @brief     ��mqtt���Խӿڹر�
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-3
* @Update    :
**********************************************************************/
void sec_mqtt_debug_close()
{
    g_SecMqttDebug = 0;
    sec_mqtt_debug();
}

/**********************************************************************
* @name      : sec_mqtt_info_get
* @brief     ��mqtt�����ѯ������Ϣ
* @param[in] ��int sockid            taskmag���
               MESSAGE_INFO_T *pMsg  mqtt��Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_mqtt_info_get(int sockid, MESSAGE_INFO_T *pMsg)
{
    int ret = 0;
//    ret = task_send_labelmsg(sockid, MSG_SEC_MAP_INFO, pMsg->label, pMsg->playload, pMsg->msglen, SEC_MSG_WORK);
//    if(0 == ret)
//    {
//        MQTTMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", 
//                          SEC_MSG_WORK, MSG_SEC_MAP_INFO, pMsg->label, ret);
//    }
//    else
//    {
//        MQTTMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", 
//                           SEC_MSG_WORK, MSG_SEC_MAP_INFO, pMsg->label, ret);
//    }
    return ret;
}

/**********************************************************************
* @name      : sec_mqtt_data_init
* @brief     ��mqtt�������ݳ�ʼ��֪ͨ
* @param[in] ��int sockid            taskmag���
               MESSAGE_INFO_T *pMsg  mqtt��Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_mqtt_data_init(int sockid, MESSAGE_INFO_T *pMsg)
{
    int ret = 0;
    
    return ret;
}

/**********************************************************************
* @name      : sec_flowlimit_set
* @brief     ������������ֵ����
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ���
               void *pPublic          ��������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int sec_flowlimit_set(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    uint8 ack = TRUE;

    msgbuf = pInMsg->playload;

    MEMCPY_R(&g_session_limit.monthFlowLimit, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);

    MEMCPY_R(&g_session_limit.flowSpeedLimit, &msgbuf[offset],  sizeof(uint32));
    offset += sizeof(uint32);

    SEC_FMT_DEBUG("��������:������ֵ %d KB ������ֵ %d Kbps\n", g_session_limit.monthFlowLimit, g_session_limit.flowSpeedLimit);
    
    /* ���췵����Ϣ */
    return sec_axdrmsg_respond(pInMsg, &ack, 1);
}


/**********************************************************************
* @name      : sec_flowlimit_get
* @brief     ������������ֵ����
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ���
               void *pPublic          ��������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int sec_flowlimit_get(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8 msgbuf[32] = {0};
    uint16 offset = 0;

    MEMCPY_R(&msgbuf[offset], &g_session_limit.monthFlowLimit,  sizeof(uint32));
    offset += sizeof(uint32);

    MEMCPY_R(&msgbuf[offset],  &g_session_limit.flowSpeedLimit, sizeof(uint32));
    offset += sizeof(uint32);
    
    /* ���췵����Ϣ */
    return sec_axdrmsg_respond(pInMsg, msgbuf, offset);
}

/**********************************************************************
* @name      : sec_srvlimit_set
* @brief     �����÷�����ֵ����
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ���
               void *pPublic          ��������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int sec_srvlimit_set(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    uint8 *msgbuf = NULL;
    uint16 offset = 0;
    int i = 0;
    uint8 ack = TRUE;

    msgbuf = pInMsg->playload;
    
    g_srvtime_limit.nNum = msgbuf[offset++];

    for(i = 0; i < g_srvtime_limit.nNum; i++)
    {
        g_srvtime_limit.service[i].type = msgbuf[offset++];
        
        MEMCPY_R(&g_srvtime_limit.service[i].limit, &msgbuf[offset],  sizeof(uint32));
        offset += sizeof(uint32);

        if(g_srvtime_limit.service[i].type == SEC_SRV_SSH)
        {
            g_sshtime_limit = g_srvtime_limit.service[i].limit;
            SEC_FMT_DEBUG("��������:SSH����ʱ��δ�رո澯ֵ %d ����\n", g_sshtime_limit);
        }
    }
    
    /* ���췵����Ϣ */
    return sec_axdrmsg_respond(pInMsg, &ack, 1);
}

/**********************************************************************
* @name      : sec_data_init
* @brief     ������������ֵ����
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ���
               void *pPublic          ��������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int sec_data_init(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    memset(&g_session_info, 0, sizeof(SEC_SESSIONAPPS_T));

    return 0;
}

/**********************************************************************
* @name      : sec_param_init
* @brief     ��������ʼ��
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ���
               void *pPublic          ��������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int sec_param_init(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    memset(&g_session_info, 0, sizeof(SEC_SESSIONAPPS_T));
    memset(&g_session_limit, 0, sizeof(SEC_SESSION_LIMIT_T));
    memset(&g_srvtime_limit, 0, sizeof(SEC_SRVTIME_LIMIT_T));

    return 0;
}


SEC_MSGFUN_T g_tMsgFunc[] =
{
  /* IID,                    IOP,                         ��Ϣ������ */
    {MESSAGE_IID_COMMON,     COMMON_IOP_HERTBEAT,         sec_heart_bear},
    {MESSAGE_IID_COMMON,     COMMON_IOP_VERSION,          sec_get_version},
    {MESSAGE_IID_COMMON,     COMMON_IOP_GETAPPID,         sec_get_appidmsg},
    {MESSAGE_IID_SYSTEM,     SYSTEM_IOP_GET_REGISTERAPP,  sec_recv_regapp},
    {MESSAGE_IID_DB,         DB_IOP_INIT_DATA,            sec_data_init},
    {MESSAGE_IID_DB,         DB_IOP_INIT_DATA,            sec_param_init},
    {MESSAGE_IID_SCS,        SCS_IOP_FLOW_PARAM_SET,      sec_flowlimit_set},
    {MESSAGE_IID_SCS,        SCS_IOP_FLOW_PARAM_GET,      sec_flowlimit_get},
    {MESSAGE_IID_SCS,        SCS_IOP_TIME_OUT_SET,        sec_srvlimit_set},
};
uint8 g_nMsgNum = sizeof(g_tMsgFunc)/sizeof(SEC_MSGFUN_T);

/**********************************************************************
* @name      : sec_axdr_msg_proc
* @brief     ��axdr��Ϣ����
* @param[in] ��MESSAGE_INFO_T *pMsgInfo   ��Ϣ
               SEC_MSGDATA_T *ptMsgData  ˽������
               SEC_APPDATA_T *ptAppData  ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-10
* @Update    :
**********************************************************************/
void sec_axdr_msg_proc(MESSAGE_MQTT_T *pMsgMqtt, SEC_MSGDATA_T *ptMsgData, SEC_APPDATA_T *ptAppData)
{
    int ret = 0;
    uint8 i = 0;
    MESSAGE_INFO_T *pMsgInfo = &pMsgMqtt->tAXDR;
    
    if(0 == strcmp(pMsgInfo->souraddr, SEC_APP_NAME))
    {
        /* �����Է����� */
        MQTTMSG_FMT_DEBUG("Revc self msg, iid %d, iop %d\n", pMsgInfo->IID, pMsgInfo->IOP);
        return;
    }

    /* ע��Ӧ�� */
    if(MESSAGE_IID_SYSTEM == pMsgInfo->IID )
    {
        if(SYSTEM_IOP_APP_REGISTER == pMsgInfo->IOP)
        {
            ptMsgData->appStep = SEC_STATUS_SEEK;
            MQTTMSG_FMT_DEBUG("uartManger register success, find mapManager from smiOS....\n");
            ret = sec_seek_app();  //��ѯע���APP
            if(0 != ret)
            {
                MQTTMSG_FMT_DEBUG("uartManger seek app failed %d\n", ret);
            }

            return;
        }
    }

    //ͨ����Ϣ����
    for(i = 0; i < g_nMsgNum; i++)
    {
        if(g_tMsgFunc[i].IID == pMsgInfo->IID && g_tMsgFunc[i].IOP == pMsgInfo->IOP)
        {
            if(g_tMsgFunc[i].pProcMsg)
            {
                g_tMsgFunc[i].pProcMsg(pMsgInfo, (void *)ptMsgData, (void *)ptAppData);
            }

            break;
        }
    }
    
    return;
}

/**********************************************************************
* @name      : sec_json_msg_proc
* @brief     ��JSON��Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsgInfo   ��Ϣ
               SEC_MSGDATA_T *ptMsgData  ˽������
               SEC_APPDATA_T *ptAppData  ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-10
* @Update    :
**********************************************************************/
void sec_json_msg_proc(MESSAGE_MQTT_T *pMsgMqtt, SEC_MSGDATA_T *ptMsgData, SEC_APPDATA_T *ptAppData)
{
    int ret = 0;
    MESSAGE_JSON_T *pMsgJson = &pMsgMqtt->tJson;

    if(0 == strcmp(pMsgJson->sourapp, SEC_APP_NAME))
    {
        /* �����Է����� */
        MQTTMSG_FMT_DEBUG("Revc self msg, infopath %s\n", pMsgJson->infopath);
        return;
    }

    /* ע��Ӧ�� */
    if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_REGISTER))
    {
        ptMsgData->appStep = SEC_STATUS_SEEK;
        MQTTMSG_FMT_DEBUG("uartManager register success, find dbCenter from smiOS....\n");
        ret = sec_get_appreglist();  //��ѯע���APP
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("uartManager seek app failed %d\n", ret);
        }

        return;
    }
    else if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_KEEPALIVE))
    {
        ret = sec_heart_to_sys(pMsgJson, ptMsgData, ptAppData);
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("uartManager heart to sys failed %d\n", ret);
        }

        return;
    }
    else if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_GETREGLIST) && pMsgJson->token < 0x10000)
    {
        ret = sec_appreglist_check(pMsgJson, ptMsgData, ptAppData);
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("uartManager get reglist failed %d\n", ret);
        }
        
        ptMsgData->appStep = SEC_STATUS_WORK;
        return;
    }

    return;
}

/**********************************************************************
* @name      : sec_mqttmsg_init
* @brief     ��mqtt��Ϣ����
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_mqttmsg_init(void *pPublic, void **ppPrivate)
{
    SEC_MSGDATA_T *ptMsgData = NULL;
    SEC_APPDATA_T *ptPData = (SEC_APPDATA_T*)pPublic;

    ptMsgData = (SEC_MSGDATA_T *)malloc(sizeof(SEC_MSGDATA_T));
    if(NULL == ptMsgData)
    {
        MQTTMSG_FMT_DEBUG("��ʼ��ʧ��\n");
        return -1;
    }
    
    memset(ptMsgData, 0, sizeof(SEC_MSGDATA_T));

    /* �ȷ���һ��APPע����Ϣ */
    ptMsgData->appStep = SEC_STATUS_LOGIN;  
    sec_login_to_smios(ptPData->appid, ptPData->appidlen);
    
    *ppPrivate = ptMsgData;
    return 0;

}

/**********************************************************************
* @name      : sec_mqttmsg_proc
* @brief     ��mqtt��Ϣ����
* @param[in] ��void *pArg     �߳̾��
               void *pPrivate ˽������
* @param[out]��
* @return    ��0-��ȷ !=0-����
* @Create    : �� ��
* @Date      ��2021-07-10
* @Update    :
**********************************************************************/
int sec_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int             ret = 0;
    int             recvlen = 0;
    SEC_MSGDATA_T *ptMsgData = (SEC_MSGDATA_T*)pPrivate;
    SEC_APPDATA_T *ptAppData = (SEC_APPDATA_T*)pPublic;
    struct timespec curtime = {0};
    MESSAGE_MQTT_T *pMsgMqtt = NULL;

    static uint8   s_baseOK = 0;

    if(0 == s_baseOK)
    {
        s_baseOK = 1;
        BASE_TASK_SET_OK(pArg); //����APP���׼������
    }
    
    /* ѭ������Ϣ */
    recvlen = sec_recv_mqttmsg(ptMsgData->topic, SEC_TOPIC_LEN, ptMsgData->recvBuf, APP_RECV_BUF_LEN);
    if(recvlen > 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &curtime);
    
        ret = appmsg_mqtt_split(ptMsgData->recvBuf, (uint16)recvlen, ptMsgData->topic, &pMsgMqtt);
        if(0 == ret && NULL != pMsgMqtt)
        {
            ptMsgData->isNoFree = FALSE;
            
            if(MESSAGE_TYPE_A_XDR == pMsgMqtt->msgtype)
            {
                /* AXDR��ʽ */
                TASK_SET_USERLOG(pArg, "Msg from %s iid %04x, iop %04x", pMsgMqtt->tAXDR.souraddr, 
                                       pMsgMqtt->tAXDR.IID, pMsgMqtt->tAXDR.IOP);
                MQTTMSG_BUF_DEBUG(ptMsgData->recvBuf, recvlen, "TOPIC: %s IID-%04X, IOP-%04X:\n", 
                                                   ptMsgData->topic, pMsgMqtt->tAXDR.IID, pMsgMqtt->tAXDR.IOP);
                sec_axdr_msg_proc(pMsgMqtt, ptMsgData, ptAppData);
            }
            else
            {
                /* JSON��ʽ */
                TASK_SET_USERLOG(pArg, "Msg topic %s", ptMsgData->topic);
                MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", ptMsgData->topic, ptMsgData->recvBuf);
                ptAppData->msgtime = curtime.tv_nsec/1000000;
                sec_json_msg_proc(pMsgMqtt, ptMsgData, ptAppData);
                
                if(ptMsgData->isNoFree == FALSE)
                {
                    appmsg_json_free(pMsgMqtt);
                }                
            }

            /* ��Դ�ͷ� */
            appmsg_free(pMsgMqtt);

            memset(ptMsgData->recvBuf, 0, recvlen);
            memset(ptMsgData->topic, 0, SEC_TOPIC_LEN);
        }
    }
   
    /* 2. ��ʱ���� */
    clock_gettime(CLOCK_MONOTONIC, &curtime);
    if(curtime.tv_sec - ptMsgData->timeflag < 5)
    {
        return 0;
    }
    ptMsgData->timeflag = curtime.tv_sec;
    
#ifdef PRODUCT_SCU    
        /* APP��� */
        switch(ptMsgData->appStep)
        {
            case SEC_STATUS_LOGIN:
            {
                sec_login_to_sys();
                break;
            }
            case SEC_STATUS_SEEK:
            {
                sec_get_appreglist();
                break;
            }
            default: break;
        }
#else
        /* APP��� */
        switch(ptMsgData->appStep)
        {
            case SEC_STATUS_LOGIN:
            {
                sec_login_to_smios(ptAppData->appid, ptAppData->appidlen);
                break;
            }
            case SEC_STATUS_SEEK:
            {
                sec_seek_app();
                break;
            }
            default: break;
        }
#endif

    return 0;
}

SEC_TASKMSGFUN_T g_LcTaskMsgFun[] = 
{
    {0, NULL},
};
uint8 g_LcTaskMsgNum = sizeof(g_LcTaskMsgFun)/sizeof(SEC_TASKMSGFUN_T);

/**********************************************************************
* @name      : sec_mode_check
* @brief     ������ģʽ���
* @param[in] ��MESSAGE_INFO_T *pInMsg ������Ϣ
               void *pArg             ˽�о��
               void *pPublic          ��������
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-7-8
* @Update    :
**********************************************************************/
int sec_param_check(SEC_APPDATA_T* ptAppData)
{

    return ERR_OK;
}

/**********************************************************************
* @name      : sec_taskmsg_init
* @brief     ��task��Ϣ����
* @param[in] ��void *pPublic
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_taskmsg_init(void *pPublic, void **ppPrivate)
{
    SEC_MSGDATA_T *ptMsgData = NULL;

    ptMsgData = (SEC_MSGDATA_T *)malloc(sizeof(SEC_MSGDATA_T));
    if(NULL == ptMsgData)
    {
        MQTTMSG_FMT_DEBUG("sec_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptMsgData, 0, sizeof(SEC_MSGDATA_T));
//    ptMsgData->tasksocket = task_msg_init(SEC_MSG_TASK);
//    if(ptMsgData->tasksocket <= 0)
//    {
//        MQTTMSG_FMT_DEBUG("sec_taskmsg_init taskmsg init failed\n");
//        free(ptMsgData);
//        return -2;
//    }
//    task_msg_settime(ptMsgData->tasksocket, 1, 0);

    *ppPrivate = ptMsgData;
    return 0;
}

/**********************************************************************
* @name      : sec_taskmsg_proc
* @brief     ��task��Ϣ���� �������ⲿ����mqtt��Ϣ
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int sec_taskmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    //SEC_MSGDATA_T *ptMsgData = (SEC_MSGDATA_T *)pPrivate;
    //TASK_MSG_t    *pMsg = (TASK_MSG_t *)ptMsgData->recvBuf;
    //int            recvlen = 0;
    //uint32         sender = 0;
    //int            ret = 0;
    //uint8          i = 0;
    static uint8   s_baseOK = 0;

    if(0 == s_baseOK)
    {
        s_baseOK = 1;
        BASE_TASK_SET_OK(pArg); //����APP���׼������
    }
//    recvlen = task_recv_msg(ptMsgData->tasksocket, ptMsgData->recvBuf, TASK_MSG_MAX_LEN, &sender);
//    if(recvlen <= 0)
//    {
//        return 0;
//    }
//
//    for(i = 0; i < g_LcTaskMsgNum; i++)
//    {
//        if(pMsg->msg_id == g_LcTaskMsgFun[i].msgid)
//        {
//            if(g_LcTaskMsgFun[i].pProcMsg)
//            {
//                TASK_SET_USERLOG(pArg, "task msg 0x%08x", pMsg->msg_id);
//                g_LcTaskMsgFun[i].pProcMsg(pMsg);
//            }
//            break;
//        }
//    }

    //���ģʽ���
    sec_param_check(pPublic);
    sleep(1);
    
    //memset(ptMsgData->recvBuf, 0, recvlen);
    return 0;
}


