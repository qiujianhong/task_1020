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
#include "gui_c_prtl.h"

#include "lc_comm.h"
#include "lc_mqtt.h"
#include "lc_msg.h"

uint8  g_LCMqttDebug = 0;
extern OOP_LOOPSTAT_T g_legendStatus;
extern OOP_LOOPSTAT_T g_lastStatus;

/**********************************************************************
* @name      : lc_mqtt_debug_open
* @brief     ��mqtt���Խӿڴ�
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-3
* @Update    :
**********************************************************************/
void lc_mqtt_debug_open()
{
    lc_mqtt_debug();
    g_LCMqttDebug = 1;
}

/**********************************************************************
* @name      : lc_mqtt_debug_close
* @brief     ��mqtt���Խӿڹر�
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-3
* @Update    :
**********************************************************************/
void lc_mqtt_debug_close()
{
    g_LCMqttDebug = 0;
    lc_mqtt_debug();
}

/**********************************************************************
* @name      : lc_mqtt_map_info
* @brief     ��mqtt�����ѯģ����Ϣ
* @param[in] ��int sockid            taskmag���
               MESSAGE_INFO_T *pMsg  mqtt��Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int lc_mqtt_map_info(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;
    LC_MSGDATA_T *ptMsgData = (LC_MSGDATA_T *)pArg;
    int sockid = ptMsgData->tasksocket;
    
    ret = task_send_labelmsg(sockid, MSG_LC_MAP_INFO, pInMsg->label, pInMsg->playload, pInMsg->msglen, LC_MSG_WORK);
    if(0 == ret)
    {
        MQTTMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", 
                          LC_MSG_WORK, MSG_LC_MAP_INFO, pInMsg->label, ret);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", 
                           LC_MSG_WORK, MSG_LC_MAP_INFO, pInMsg->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : lc_mqtt_map_slot
* @brief     ��mqtt�����ѯ��λ��
* @param[in] ��int sockid            taskmag���
               MESSAGE_INFO_T *pMsg  mqtt��Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int lc_mqtt_map_slot(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;
    LC_MSGDATA_T *ptMsgData = (LC_MSGDATA_T *)pArg;
    int sockid = ptMsgData->tasksocket;
    
    ret = task_send_labelmsg(sockid, MSG_LC_MAP_SLOT, pInMsg->label, pInMsg->playload, pInMsg->msglen, LC_MSG_WORK);
    if(0 == ret)
    {
        MQTTMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", 
                          LC_MSG_WORK, MSG_LC_MAP_SLOT, pInMsg->label, ret);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", 
                           LC_MSG_WORK, MSG_LC_MAP_SLOT, pInMsg->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : lc_mqtt_map_event
* @brief     ��mqtt����ģ�������֪ͨ
* @param[in] ��int sockid            taskmag���
               MESSAGE_INFO_T *pMsg  mqtt��Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int lc_mqtt_map_event(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;
    LC_MSGDATA_T *ptMsgData = (LC_MSGDATA_T *)pArg;
    int sockid = ptMsgData->tasksocket;
    
    ret = task_send_labelmsg(sockid, MSG_LC_MAP_EVENT, pInMsg->label, pInMsg->playload, pInMsg->msglen, LC_MSG_WORK);
    if(0 == ret)
    {
        MQTTMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, ret %d\n", LC_MSG_WORK, MSG_LC_MAP_EVENT, ret);
    }
    else
    {
        MQTTMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, ret %d\n", LC_MSG_WORK, MSG_LC_MAP_EVENT, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : lc_mqtt_data_init
* @brief     ��mqtt�������ݳ�ʼ��֪ͨ
* @param[in] ��int sockid            taskmag���
               MESSAGE_INFO_T *pMsg  mqtt��Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int lc_mqtt_data_init(MESSAGE_INFO_T *pInMsg, void *pArg, void *pPublic)
{
    int ret = 0;

    memset_s(&g_legendStatus, sizeof(OOP_LOOPSTAT_T), 0, sizeof(OOP_LOOPSTAT_T));
    memset_s(&g_lastStatus, sizeof(OOP_LOOPSTAT_T), 0, sizeof(OOP_LOOPSTAT_T));

    LCMONIT_FMT_DEBUG("data init, loopstat clear.\n");

    return ret;
}

LC_MSGFUN_T g_LcMsgFun[] =
{
  /* IID,                    IOP,                         ��Ϣ������ */
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_INFO,         lc_mqtt_map_info},
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_PORTINFO,     lc_mqtt_map_slot},
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_STATUS,       NULL},
    {MESSAGE_IID_MODULE,     MODULE_IOP_STATUS_EVENT,     lc_mqtt_map_event},

    {MESSAGE_IID_DB,         DB_IOP_UPDATE_EVENT,         NULL},
    {MESSAGE_IID_DB,         DB_IOP_INIT_EVENT,           lc_mqtt_data_init},

    {MESSAGE_IID_SELF,       SELF_IOP_GET_VERSION,        lc_get_selfversion},
};
uint8 g_LcMsgNum = sizeof(g_LcMsgFun)/sizeof(LC_MSGFUN_T);

/**********************************************************************
* @name      : lc_axdr_msg_proc
* @brief     ��axdr��Ϣ����
* @param[in] ��MESSAGE_INFO_T *pMsgInfo   ��Ϣ
               LC_MSGDATA_T *ptMsgData  ˽������
               LC_APPDATA_T *ptAppData  ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-10
* @Update    :
**********************************************************************/
void lc_axdr_msg_proc(MESSAGE_INFO_T *pMsgInfo, LC_MSGDATA_T *ptMsgData, LC_APPDATA_T *ptAppData)
{
    int ret = 0;
    uint8 i = 0;
    
    if(0 == strcmp(pMsgInfo->souraddr, LC_APP_NAME))
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
            ptMsgData->appStep = LC_STATUS_SEEK;
            MQTTMSG_FMT_DEBUG("lcMonitor register success, find mapManager from smiOS....\n");
            ret = lc_seek_app();  //��ѯע���APP
            if(0 != ret)
            {
                MQTTMSG_FMT_DEBUG("lcMonitor seek app failed %d\n", ret);
            }

            return;
        }
        else if(SYSTEM_IOP_GET_REGISTERAPP == pMsgInfo->IOP)
        {
            ret = lc_recv_regapp(pMsgInfo, ptMsgData, ptAppData);
            if(0 != ret)
            {
                MQTTMSG_FMT_DEBUG("lcMonitor get reglist failed %d\n", ret);
            }

            return;            
        }
    }

    if(MESSAGE_IID_COMMON == pMsgInfo->IID)
    {
        if(COMMON_IOP_HERTBEAT == pMsgInfo->IOP)
        {
            ret = lc_heart_bear(pMsgInfo);
            if(0 != ret)
            {
                MQTTMSG_FMT_DEBUG("lcMonitor heart to sys failed %d\n", ret);
            }

            return;
        }
        else if(COMMON_IOP_VERSION == pMsgInfo->IOP)
        {
            ret = lc_get_version(pMsgInfo);
            if(0 != ret)
            {
                MQTTMSG_FMT_DEBUG("lcMonitor heart to sys failed %d\n", ret);
            }

            return;            
        }
        else if(COMMON_IOP_GETAPPID == pMsgInfo->IOP)
        {
            ret = lc_get_appidmsg(pMsgInfo, ptAppData->appid, ptAppData->appidlen);
            if(0 != ret)
            {
                MQTTMSG_FMT_DEBUG("lcMonitor heart to sys failed %d\n", ret);
            }

            return;            
        }        
    }

    /* ��ʾ��Ϣ */
    if(pMsgInfo->IID == MESSAGE_IID_DESKGUI)
    {
        appGUI_recv_back_handle(pMsgInfo);
        return;
    }

    /* �������� */
    ret = db_agent_msg_entry(ptAppData->dbAgent, pMsgInfo);
    if(DB_AGENT_UNKONW_MSG != ret)
    {
        if(0 != ret)
        {
            MQTTMSG_BUF_DEBUG(ptMsgData->recvBuf, ptMsgData->recvlen, "Drop msg from %s, ret %d", pMsgInfo->souraddr, ret);
        }
        return;
    }
            
    for(i = 0; i < g_LcMsgNum; i++)
    {
        if(g_LcMsgFun[i].IID == pMsgInfo->IID && g_LcMsgFun[i].IOP == pMsgInfo->IOP)
        {
            if(g_LcMsgFun[i].pProcMsg)
            {
                g_LcMsgFun[i].pProcMsg(pMsgInfo, (void *)ptMsgData, (void *)ptAppData);
            }

            break;
        }
    }
    
    if(i == g_LcMsgNum && !(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_ERRMSG == pMsgInfo->IOP))
    {
        lc_send_errmsg(pMsgInfo, -1);
    }
    
    return;
}

/**********************************************************************
* @name      : lc_json_msg_proc
* @brief     ��JSON��Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsgInfo   ��Ϣ
               LC_MSGDATA_T *ptMsgData  ˽������
               LC_APPDATA_T *ptAppData  ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-10
* @Update    :
**********************************************************************/
void lc_json_msg_proc(MESSAGE_MQTT_T *pMsgMqtt, LC_MSGDATA_T *ptMsgData, LC_APPDATA_T *ptAppData)
{
    int ret = 0;
    MESSAGE_JSON_T *pMsgJson = &pMsgMqtt->tJson;

    if(0 == strcmp(pMsgJson->sourapp, LC_APP_NAME))
    {
        /* �����Է����� */
        MQTTMSG_FMT_DEBUG("Revc self msg, infopath %s\n", pMsgJson->infopath);
        return;
    }

    /* ע��Ӧ�� */
    if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_REGISTER))
    {
        ptMsgData->appStep = LC_STATUS_SEEK;
        MQTTMSG_FMT_DEBUG("lcMonitor register success, find dbCenter from OS-system.\n");
        ret = lc_get_appreglist();  //��ѯע���APP
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("lcMonitor seek app failed %d\n", ret);
        }

        return;
    }
    else if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_KEEPALIVE))
    {
        ret = lc_heart_to_sys(pMsgJson, ptMsgData, ptAppData);
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("lcMonitor heart to sys failed %d\n", ret);
        }

        return;
    }
    else if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_GETREGLIST) && pMsgJson->token < 0x10000)
    {
        ret = lc_appreglist_check(pMsgJson, ptMsgData, ptAppData);
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("lcMonitor get reglist failed %d\n", ret);
        }

        return;
    }

    return;
}

/**********************************************************************
* @name      : lc_mqttmsg_init
* @brief     ��mqtt��Ϣ����
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int lc_mqttmsg_init(void *pPublic, void **ppPrivate)
{
    LC_MSGDATA_T *ptMsgData = NULL;
    LC_APPDATA_T *ptAppData = (LC_APPDATA_T *)pPublic;

    ptMsgData = (LC_MSGDATA_T *)malloc(sizeof(LC_MSGDATA_T));
    if(NULL == ptMsgData)
    {
        MQTTMSG_FMT_DEBUG("lc_mqttmsg_init malloc failed\n");
        return -1;
    }
    memset(ptMsgData, 0, sizeof(LC_MSGDATA_T));
    ptMsgData->appStep = LC_STATUS_LOGIN;
    ptMsgData->tasksocket = task_msg_init(LC_MSG_PROC);
    if(ptMsgData->tasksocket <= 0)
    {
        MQTTMSG_FMT_DEBUG("lc_mqttmsg_init taskmsg init failed\n");
        free(ptMsgData);
        return -2;
    }
    task_msg_settime(ptMsgData->tasksocket, 0, 100000);

    /* �ȷ���һ��APPע����Ϣ */
    ptMsgData->appStep = LC_STATUS_LOGIN;  

#ifdef PRODUCT_SCU
    lc_login_to_sys();
#else
    lc_login_to_smios(ptAppData->appid, ptAppData->appidlen);
#endif

    *ppPrivate = ptMsgData;
    return 0;
}

/**********************************************************************
* @name      : lc_mqttmsg_proc
* @brief     ��mqtt��Ϣ����
* @param[in] ��void *pArg     �߳̾��
               void *pPrivate ˽������
* @param[out]��
* @return    ��0-��ȷ !=0-����
* @Create    : �� ��
* @Date      ��2021-07-10
* @Update    :
**********************************************************************/
int lc_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int             ret = 0;
    LC_MSGDATA_T *ptMsgData = (LC_MSGDATA_T*)pPrivate;
    LC_APPDATA_T *ptAppData = (LC_APPDATA_T*)pPublic;
    struct timespec curtime = {0};
    MESSAGE_INFO_T *pMsgInfo = NULL;

    BASE_TASK_SET_OK(pArg); //����APP���׼������
        
    /* ѭ������Ϣ */
    ptMsgData->recvlen = lc_recv_mqttmsg(ptMsgData->topic, LC_TOPIC_LEN, ptMsgData->recvBuf, APP_RECV_BUF_LEN);
    if(ptMsgData->recvlen > 0)
    {
        clock_gettime(CLOCK_MONOTONIC, &curtime);

        //TODO ��ʱ�������⣬ȫ��ʹ��AXDR����
        ret = appmsg_split(ptMsgData->recvBuf, (uint16)ptMsgData->recvlen , &pMsgInfo);
        if(0 == ret && NULL != pMsgInfo)
        {
            MQTTMSG_BUF_TRACE(ptMsgData->recvBuf, ptMsgData->recvlen, "Recv msg from %s, iid 0x%x, iop 0x%x", 
                                               pMsgInfo->souraddr, pMsgInfo->IID, pMsgInfo->IOP);
            lc_axdr_msg_proc(pMsgInfo, ptMsgData, ptAppData);

            /* ��Դ�ͷ� */
            free(pMsgInfo);
            pMsgInfo = NULL;
            memset(ptMsgData->recvBuf, 0, ptMsgData->recvlen);
            memset(ptMsgData->topic, 0, LC_TOPIC_LEN);
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
        case LC_STATUS_LOGIN:
        {
            lc_login_to_sys();
            break;
        }
        case LC_STATUS_SEEK:
        {
            lc_get_appreglist();
            break;
        }
        default: break;
    }
#else
    /* APP��� */
    switch(ptMsgData->appStep)
    {
        case LC_STATUS_LOGIN:
        {
            lc_login_to_smios(ptAppData->appid, ptAppData->appidlen);
            break;
        }
        case LC_STATUS_SEEK:
        {
            lc_seek_app();
            break;
        }
        default: break;
    }
#endif
    return 0;
}

/**********************************************************************
* @name      : lc_send_map_slot
* @brief     �����Ͳ�ѯ��Ϣ
* @param[in] ��
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int lc_send_map_slot(TASK_MSG_t *pMsg)
{
    int ret = 0;
    ret = lc_send_mapmsg(pMsg->label, MODULE_IOP_GET_PORTINFO, pMsg->msg, pMsg->msglen);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("Send get mod port msg error, code %d\n", ret);
    }
    return ret;
}

/**********************************************************************
* @name      : lc_send_map_slot
* @brief     �����Ͳ�ѯ��Ϣ
* @param[in] ��
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int lc_send_map_info(TASK_MSG_t *pMsg)
{
    int ret = 0;
    ret = lc_send_mapmsg(pMsg->label, MODULE_IOP_GET_INFO, pMsg->msg, pMsg->msglen);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("Send get mod info msg error, code %d\n", ret);
    }
    return ret;
}

LC_TASKMSGFUN_T g_LcTaskMsgFun[] = 
{
    {MSG_LC_MAP_SLOT,    lc_send_map_slot},  //��ѯģ����Ϣ
    {MSG_LC_MAP_INFO,    lc_send_map_info},  //��ѯ
};
uint8 g_LcTaskMsgNum = sizeof(g_LcTaskMsgFun)/sizeof(LC_TASKMSGFUN_T);

/**********************************************************************
* @name      : lc_taskmsg_init
* @brief     ��task��Ϣ����
* @param[in] ��void *pPublic
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int lc_taskmsg_init(void *pPublic, void **ppPrivate)
{
    LC_MSGDATA_T *ptMsgData = NULL;

    ptMsgData = (LC_MSGDATA_T *)malloc(sizeof(LC_MSGDATA_T));
    if(NULL == ptMsgData)
    {
        MQTTMSG_FMT_DEBUG("lc_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptMsgData, 0, sizeof(LC_MSGDATA_T));
    ptMsgData->tasksocket = task_msg_init(LC_MSG_TASK);
    if(ptMsgData->tasksocket <= 0)
    {
        MQTTMSG_FMT_DEBUG("lc_taskmsg_init taskmsg init failed\n");
        free(ptMsgData);
        return -2;
    }
    task_msg_settime(ptMsgData->tasksocket, 1, 0);

    *ppPrivate = ptMsgData;
    return 0;
}

/**********************************************************************
* @name      : lc_taskmsg_proc
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
int lc_taskmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    LC_MSGDATA_T *ptMsgData = (LC_MSGDATA_T *)pPrivate;
    TASK_MSG_t    *pMsg = (TASK_MSG_t *)ptMsgData->recvBuf;
    int            recvlen = 0;
    uint32         sender = 0;
    //int            ret = 0;
    uint8          i = 0;
    static uint8   s_baseOK = 0;

    if(0 == s_baseOK)
    {
        s_baseOK = 1;
        BASE_TASK_SET_OK(pArg); //����APP���׼������
    }
    recvlen = task_recv_msg(ptMsgData->tasksocket, ptMsgData->recvBuf, TASK_MSG_MAX_LEN, &sender);
    if(recvlen <= 0)
    {
        return 0;
    }

    for(i = 0; i < g_LcTaskMsgNum; i++)
    {
        if(pMsg->msg_id == g_LcTaskMsgFun[i].msgid)
        {
            if(g_LcTaskMsgFun[i].pProcMsg)
            {
                TASK_SET_USERLOG(pArg, "task msg 0x%08x", pMsg->msg_id);
                g_LcTaskMsgFun[i].pProcMsg(pMsg);
            }
            break;
        }
    }
    memset(ptMsgData->recvBuf, 0, recvlen);
    return 0;
}


