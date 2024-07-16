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
#include "uart_api.h"

#include "uart_comm.h"
#include "uart_mqtt.h"
#include "uart_msg.h"
#include "uart_service.h"

uint8  g_UartMqttDebug = 0;
extern UART_INFO_T g_uart_info[];
extern int g_uart_info_num;
extern char  g_uart_datafile[256];

/**********************************************************************
* @name      : uart_mqtt_debug_open
* @brief     ��mqtt���Խӿڴ�
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-3
* @Update    :
**********************************************************************/
void uart_mqtt_debug_open()
{
    uart_mqtt_debug();
    g_UartMqttDebug = 1;
}

/**********************************************************************
* @name      : uart_mqtt_debug_close
* @brief     ��mqtt���Խӿڹر�
* @param[in] ��
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-7-3
* @Update    :
**********************************************************************/
void uart_mqtt_debug_close()
{
    g_UartMqttDebug = 0;
    uart_mqtt_debug();
}

/**********************************************************************
* @name      : uart_mqtt_info_get
* @brief     ��mqtt�����ѯ������Ϣ
* @param[in] ��int sockid            taskmag���
               MESSAGE_INFO_T *pMsg  mqtt��Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int uart_mqtt_info_get(int sockid, MESSAGE_INFO_T *pMsg)
{
    int ret = 0;
//    ret = task_send_labelmsg(sockid, MSG_UART_MAP_INFO, pMsg->label, pMsg->playload, pMsg->msglen, UART_MSG_WORK);
//    if(0 == ret)
//    {
//        MQTTMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", 
//                          UART_MSG_WORK, MSG_UART_MAP_INFO, pMsg->label, ret);
//    }
//    else
//    {
//        MQTTMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", 
//                           UART_MSG_WORK, MSG_UART_MAP_INFO, pMsg->label, ret);
//    }
    return ret;
}

/**********************************************************************
* @name      : uart_mqtt_data_init
* @brief     ��mqtt�������ݳ�ʼ��֪ͨ
* @param[in] ��int sockid            taskmag���
               MESSAGE_INFO_T *pMsg  mqtt��Ϣ
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int uart_mqtt_data_init(int sockid, MESSAGE_INFO_T *pMsg)
{
    int ret = 0;
    
    return ret;
}

UART_MSGFUN_T g_tMsgFunc[] =
{
  /* IID,                    IOP,                         ��Ϣ������ */
    {MESSAGE_IID_COMMON,     COMMON_IOP_HERTBEAT,         uart_heart_bear},
    {MESSAGE_IID_COMMON,     COMMON_IOP_VERSION,          uart_get_version},
    {MESSAGE_IID_COMMON,     COMMON_IOP_GETAPPID,         uart_get_appidmsg},
    {MESSAGE_IID_SYSTEM,     SYSTEM_IOP_GET_REGISTERAPP,  uart_recv_regapp},
    
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_ALL,          uart_modeinfo_resp}, //��ѯģ����Ϣ�ظ�
    {MESSAGE_IID_MODULE,     MODULE_IOP_GET_PORTINFO,     uart_slotinfo_resp}, //��ѯ��λ��Ϣ�ظ�
    {MESSAGE_IID_MODULE,     MODULE_IOP_STATUS_EVENT,     uart_modchange_evt}, //ģ�����¼�֪ͨ
};
uint8 g_nMsgNum = sizeof(g_tMsgFunc)/sizeof(UART_MSGFUN_T);

/**********************************************************************
* @name      : uart_axdr_msg_proc
* @brief     ��axdr��Ϣ����
* @param[in] ��MESSAGE_INFO_T *pMsgInfo   ��Ϣ
               UART_MSGDATA_T *ptMsgData  ˽������
               UART_APPDATA_T *ptAppData  ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-10
* @Update    :
**********************************************************************/
void uart_axdr_msg_proc(MESSAGE_MQTT_T *pMsgMqtt, UART_MSGDATA_T *ptMsgData, UART_APPDATA_T *ptAppData)
{
    int ret = 0;
    uint8 i = 0;
    uint32 port = 0;
    MESSAGE_INFO_T *pMsgInfo = &pMsgMqtt->tAXDR;
    UART_QUEUE_T  newQueue = {0};
    
    if(0 == strcmp(pMsgInfo->souraddr, UART_APP_NAME))
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
            ptMsgData->appStep = UART_STATUS_SEEK;
            MQTTMSG_FMT_DEBUG("uartManger register success, find mapManager from smiOS....\n");
            ret = uart_seek_app();  //��ѯע���APP
            if(0 != ret)
            {
                MQTTMSG_FMT_DEBUG("uartManger seek app failed %d\n", ret);
            }

            return;
        }
    }

    //������Ϣ���͸������̵߳Ķ���
    if(MESSAGE_IID_UART == pMsgInfo->IID)
    {
        memcpy_r(&port, &pMsgInfo->playload[0], sizeof(uint32));

        newQueue.portid = port;
        newQueue.length = sizeof(MESSAGE_MQTT_T) + pMsgInfo->msglen;
        
        if(newQueue.length > UART_QUEUE_BUFER)
        {
            MQTTMSG_FMT_DEBUG("over max buffer.%d\n");
            return;
        }
        
        memcpy(newQueue.data, (uint8*)pMsgMqtt, newQueue.length);
        
        //��ģʽ�������շ���Ϣ�����ȼ�����
        for(i = 0; i < g_uart_info_num; i++)
        {
            if((g_uart_info[i].port == port) || (g_uart_info[i].portAlias == port))
            {
                if((g_uart_info[i].mode == UART_MODE_MASTER) && (UART_IOP_SEND_DATA == pMsgInfo->IOP))
                {
                    newQueue.prio = pMsgInfo->playload[5];
                    uart_pque_push(&g_uart_info[i].pque, &newQueue);
                    return;
                }
                else
                {
                    uart_fifo_push(&g_uart_info[i].fifo, &newQueue);
                    return;                        
                }
            }
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
    
    if(i == g_nMsgNum && !(MESSAGE_IID_COMMON == pMsgInfo->IID && COMMON_IOP_ERRMSG == pMsgInfo->IOP))
    {
        uart_send_errmsg(pMsgInfo, -1);
    }
    
    return;
}

/**********************************************************************
* @name      : uart_json_msg_proc
* @brief     ��JSON��Ϣ����
* @param[in] : MESSAGE_JSON_T *pMsgInfo   ��Ϣ
               UART_MSGDATA_T *ptMsgData  ˽������
               UART_APPDATA_T *ptAppData  ��������
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2021-7-10
* @Update    :
**********************************************************************/
void uart_json_msg_proc(MESSAGE_MQTT_T *pMsgMqtt, UART_MSGDATA_T *ptMsgData, UART_APPDATA_T *ptAppData)
{
    int ret = 0;
    MESSAGE_JSON_T *pMsgJson = &pMsgMqtt->tJson;
    uint8 i = 0;
    cJSON *item = NULL;
    uint8  buf[2048] = {0};
    MESSAGE_MQTT_T *ptMsgNew = (MESSAGE_MQTT_T*)buf;
    MESSAGE_JSON_T *ptJSON = &(ptMsgNew->tJson);
    UART_QUEUE_T  newQueue = {0};

    if(0 == strcmp(pMsgJson->sourapp, UART_APP_NAME))
    {
        /* �����Է����� */
        MQTTMSG_FMT_DEBUG("Revc self msg, infopath %s\n", pMsgJson->infopath);
        return;
    }

    /* ע��Ӧ�� */
    if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_REGISTER))
    {
        ptMsgData->appStep = UART_STATUS_SEEK;
        MQTTMSG_FMT_DEBUG("uartManager register success, find dbCenter from smiOS....\n");
        ret = uart_get_appreglist();  //��ѯע���APP
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("uartManager seek app failed %d\n", ret);
        }

        return;
    }
    else if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_KEEPALIVE))
    {
        ret = uart_heart_to_sys(pMsgJson, ptMsgData, ptAppData);
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("uartManager heart to sys failed %d\n", ret);
        }

        return;
    }
    else if(0 == strcmp(pMsgJson->infopath, SYS_TOPIC_GETREGLIST) && pMsgJson->token < 0x10000)
    {
        ret = uart_appreglist_check(pMsgJson, ptMsgData, ptAppData);
        if(0 != ret)
        {
            MQTTMSG_FMT_DEBUG("uartManager get reglist failed %d\n", ret);
        }
        
        ptMsgData->appStep = UART_STATUS_WORK;
        return;
    }

    item = cJSON_GetObjectItem(pMsgJson->pObject, "port");
    if(NULL == item || NULL == item->valuestring)
    {
        return;
    }

    strcpy(newQueue.portname, item->valuestring);
    newQueue.length = sizeof(MESSAGE_MQTT_T);
    memcpy(newQueue.data, (uint8*)pMsgMqtt, newQueue.length);

    //��ģʽ�������շ���Ϣ�����ȼ�����
    for(i = 0; i < g_uart_info_num; i++)
    {
        if((strcmp(g_uart_info[i].name, item->valuestring)== 0) || (strcmp(g_uart_info[i].nameAlias, item->valuestring)== 0))
        {
            if((g_uart_info[i].mode == UART_MODE_MASTER) && (NULL != strstr(pMsgJson->infopath, UART_TOPIC_DATA)))
            {
        
                item = cJSON_GetObjectItem(pMsgJson->pObject, "prio");
                if(cJSON_IsNumber(item))
                {
                    newQueue.prio = item->valueint;
                }
                
                ret = uart_pque_push(&g_uart_info[i].pque, &newQueue);
                if(ret != 0)
                {
                    ret = uart_jsonmsg_respond(pMsgJson, ptJSON, newQueue.portname, "TaskFull", "0");
                    if(ret == 0)
                    {
                        ret = uart_send_jsonmsg(ptJSON, 0);
                        cJSON_Delete(ptJSON->pObject);
                    }
                }
                else
                {
                    ptMsgData->isNoFree = TRUE;
                }
            }       
            else
            {
                ret = uart_fifo_push(&g_uart_info[i].fifo, &newQueue);
                if(ret != 0)
                {
                    ret = uart_jsonmsg_respond(pMsgJson, ptJSON, newQueue.portname, "TaskFull", "0");
                    if(ret == 0)
                    {
                        ret = uart_send_jsonmsg(ptJSON, 0);
                        cJSON_Delete(ptJSON->pObject);
                    }
                }
                else
                {
                    ptMsgData->isNoFree = TRUE;
                }
            }

            break;
        }
    }

    if(i == g_uart_info_num)
    {
        ret = uart_jsonmsg_respond(pMsgJson, ptJSON, newQueue.portname, "ParamSerialError", "0");
        if(ret == 0)
        {
            ret = uart_send_jsonmsg(ptJSON, 0);
            cJSON_Delete(ptJSON->pObject);
        }            
    }
    
    return;
}

/**********************************************************************
* @name      : uart_mqttmsg_init
* @brief     ��mqtt��Ϣ����
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int uart_mqttmsg_init(void *pPublic, void **ppPrivate)
{
    UART_MSGDATA_T *ptMsgData = NULL;
    UART_APPDATA_T *ptPData = (UART_APPDATA_T*)pPublic;
    int i = 0;

    ptMsgData = (UART_MSGDATA_T *)malloc(sizeof(UART_MSGDATA_T));
    if(NULL == ptMsgData)
    {
        MQTTMSG_FMT_DEBUG("��ʼ��ʧ��\n");
        return -1;
    }
    
    memset(ptMsgData, 0, sizeof(UART_MSGDATA_T));

    for(i = 0; i < g_uart_info_num; i++)
    {
        uart_fifo_init(&g_uart_info[i].fifo);
        uart_pque_init(&g_uart_info[i].pque);
    }

    /* �ȷ���һ��APPע����Ϣ */
    ptMsgData->appStep = UART_STATUS_LOGIN;  
    uart_login_to_smios(ptPData->appid, ptPData->appidlen);
    
    *ppPrivate = ptMsgData;
    return 0;

}

/**********************************************************************
* @name      : uart_mqttmsg_proc
* @brief     ��mqtt��Ϣ����
* @param[in] ��void *pArg     �߳̾��
               void *pPrivate ˽������
* @param[out]��
* @return    ��0-��ȷ !=0-����
* @Create    : �� ��
* @Date      ��2021-07-10
* @Update    :
**********************************************************************/
int uart_mqttmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    int             ret = 0;
    int             recvlen = 0;
    UART_MSGDATA_T *ptMsgData = (UART_MSGDATA_T*)pPrivate;
    UART_APPDATA_T *ptAppData = (UART_APPDATA_T*)pPublic;
    struct timespec curtime = {0};
    MESSAGE_MQTT_T *pMsgMqtt = NULL;

    static uint8   s_baseOK = 0;

    if(0 == s_baseOK)
    {
        s_baseOK = 1;
        BASE_TASK_SET_OK(pArg); //����APP���׼������
    }
    
    /* ѭ������Ϣ */
    recvlen = uart_recv_mqttmsg(ptMsgData->topic, UART_TOPIC_LEN, ptMsgData->recvBuf, APP_RECV_BUF_LEN);
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
                uart_axdr_msg_proc(pMsgMqtt, ptMsgData, ptAppData);
            }
            else
            {
                /* JSON��ʽ */
                TASK_SET_USERLOG(pArg, "Msg topic %s", ptMsgData->topic);
                MQTTMSG_FMT_DEBUG("TOPIC: %s:\n%s\n", ptMsgData->topic, ptMsgData->recvBuf);
                ptAppData->msgtime = curtime.tv_nsec/1000000;
                uart_json_msg_proc(pMsgMqtt, ptMsgData, ptAppData);
                
                if(ptMsgData->isNoFree == FALSE)
                {
                    appmsg_json_free(pMsgMqtt);
                }                
            }

            /* ��Դ�ͷ� */
            appmsg_free(pMsgMqtt);
        }

        memset(ptMsgData->recvBuf, 0, recvlen);
        memset(ptMsgData->topic, 0, UART_TOPIC_LEN);
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
            case UART_STATUS_LOGIN:
            {
                uart_login_to_sys();
                break;
            }
            case UART_STATUS_SEEK:
            {
                uart_get_appreglist();
                break;
            }
            default: break;
        }
#else
        /* APP��� */
        switch(ptMsgData->appStep)
        {
            case UART_STATUS_LOGIN:
            {
                uart_login_to_smios(ptAppData->appid, ptAppData->appidlen);
                break;
            }
            case UART_STATUS_SEEK:
            {
                uart_seek_app();
                break;
            }
            default: break;
        }
#endif

    return 0;
}

/**********************************************************************
* @name      : uart_send_map_slot
* @brief     �����Ͳ�ѯ��Ϣ
* @param[in] ��
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int uart_send_map_slot(TASK_MSG_t *pMsg)
{
    int ret = 0;
    ret = uart_send_mapmsg(pMsg->label, MODULE_IOP_GET_PORTINFO, pMsg->msg, pMsg->msglen);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("Send get mod port msg error, code %d\n", ret);
    }
    return ret;
}

/**********************************************************************
* @name      : uart_send_map_slot
* @brief     �����Ͳ�ѯ��Ϣ
* @param[in] ��
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int uart_send_map_info(TASK_MSG_t *pMsg)
{
    int ret = 0;
    ret = uart_send_mapmsg(pMsg->label, MODULE_IOP_GET_INFO, pMsg->msg, pMsg->msglen);
    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("Send get mod info msg error, code %d\n", ret);
    }
    return ret;
}

UART_TASKMSGFUN_T g_LcTaskMsgFun[] = 
{
    {0, NULL},
};
uint8 g_LcTaskMsgNum = sizeof(g_LcTaskMsgFun)/sizeof(UART_TASKMSGFUN_T);

/**********************************************************************
* @name      : uart_mod_frstring
* @brief     ������ģʽת����
* @param[in] ��mode     ����ģʽ
* @param[out]��modestr ����ģʽ�ַ���
* @return    ��0-�ɹ�/ʧ��
* @Create    : ����
* @Date      ��2021-8-7
* @Update    :
**********************************************************************/
UART_MODE_E uart_mod_frstring(char *modestr)
{
    if(0 == memcmp(modestr, "only", strlen("only")))
    {
        return UART_MODE_ONLY;
    }
    else if(0 == memcmp(modestr, "master", strlen("master")))
    {
        return UART_MODE_MASTER;
    }
    else if(0 == memcmp(modestr, "slave", strlen("slave")))
    {
        return UART_MODE_SLAVE;
    }

    return 0;
}


/**********************************************************************
* @name      : uart_mode_check
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
int uart_mode_check(UART_APPDATA_T* ptAppData)
{
    cJSON  *json = NULL;
    cJSON  *jList = NULL;
    cJSON  *jArray = NULL;
    cJSON  *item = NULL;
    int     i = 0, j = 0;
    int    num = 0;
    UART_INFO_T *pUartInfo = NULL;
    struct stat fileinfo = {0};
    static time_t lstmodify = 0;
    UART_MODE_E newMode = UART_MODE_UNLL;
    UART_MODEINFO_T modeInfo = {0};
    
    //ͨ���ļ��޸�ʱ�䣬��������ļ��Ƿ����
    stat(g_uart_datafile, &fileinfo);
    
    if(fileinfo.st_mtime != lstmodify)
    {
        lstmodify = fileinfo.st_mtime;

        json = cJSON_read(g_uart_datafile);
        if(!cJSON_IsObject(json))
        {
            INIT_FMT_DEBUG("cJSON_Parse error, no User config.\n");
            return ERR_PNULL;
        }

        jList = cJSON_GetObjectItem(json, "UART_CFG");
        if(!cJSON_IsArray(jList))
        {
            INIT_FMT_DEBUG("cJSON_Parse error, no list item.\n");
            return ERR_PNULL;
        }

        num = cJSON_GetArraySize(jList);
        
        for(i = 0; i < num; i++)
        {
            jArray = cJSON_GetArrayItem(jList, i);
            if(!cJSON_IsObject(jArray))
            {
                continue;
            }
            
            item = cJSON_GetObjectItem(jArray, "name");
            if(!cJSON_IsString(item))
            {
                continue;
            }

            for(j = 0; j < g_uart_info_num; j++)
            {
                if(strcmp(item->valuestring, g_uart_info[j].comname) != 0)
                {
                    continue;
                }
                
                pUartInfo = &g_uart_info[j];

                item = cJSON_GetObjectItem(jArray, "mode");
                if(cJSON_IsString(item))
                {
                    newMode = uart_mod_frstring(item->valuestring);

                    //����ģʽ���
                    if(newMode != pUartInfo->mode)
                    {
                        modeInfo.mode = pUartInfo->mode = newMode;
                        
                        strcpy(modeInfo.lastApp, pUartInfo->onlyApp);
                        
                        item = cJSON_GetObjectItem(jArray, "appName");
                        if(cJSON_IsString(item))
                        {
                            strcpy(pUartInfo->onlyApp, item->valuestring);
                        }

                        strcpy(modeInfo.curApp, pUartInfo->onlyApp);
                        
                        UART_STRING_TO_PORT(pUartInfo->name, modeInfo.port.portType, modeInfo.port.portNo);
                        
                        uart_mode_change_notify(&modeInfo, ptAppData, NULL);
                    }
                }

                break;
            }
        }
    }

    return ERR_OK;
}

/**********************************************************************
* @name      : uart_taskmsg_init
* @brief     ��task��Ϣ����
* @param[in] ��void *pPublic
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2020-6-30
* @Update    :
**********************************************************************/
int uart_taskmsg_init(void *pPublic, void **ppPrivate)
{
    UART_MSGDATA_T *ptMsgData = NULL;
    UART_INFO_T *pUartInfo = NULL;
    UART_MODEINFO_T modeInfo = {0};
    int i = 0;

    ptMsgData = (UART_MSGDATA_T *)malloc(sizeof(UART_MSGDATA_T));
    if(NULL == ptMsgData)
    {
        MQTTMSG_FMT_DEBUG("uart_taskmsg_init malloc failed\n");
        return -1;
    }
    memset(ptMsgData, 0, sizeof(UART_MSGDATA_T));
//    ptMsgData->tasksocket = task_msg_init(UART_MSG_TASK);
//    if(ptMsgData->tasksocket <= 0)
//    {
//        MQTTMSG_FMT_DEBUG("uart_taskmsg_init taskmsg init failed\n");
//        free(ptMsgData);
//        return -2;
//    }
//    task_msg_settime(ptMsgData->tasksocket, 1, 0);

    for(i = 0; i < g_uart_info_num; i++)
    {
        pUartInfo = &g_uart_info[i];
        modeInfo.mode = pUartInfo->mode;
        strcpy(modeInfo.curApp, pUartInfo->onlyApp);
        UART_STRING_TO_PORT(pUartInfo->name, modeInfo.port.portType, modeInfo.port.portNo);
        uart_mode_change_notify(&modeInfo, pPublic, NULL);
    }

    *ppPrivate = ptMsgData;
    return 0;
}

/**********************************************************************
* @name      : uart_taskmsg_proc
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
int uart_taskmsg_proc(void *pArg, void *pPublic, void *pPrivate)
{
    //UART_MSGDATA_T *ptMsgData = (UART_MSGDATA_T *)pPrivate;
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
    uart_mode_check(pPublic);
    sleep(1);
    
    //memset(ptMsgData->recvBuf, 0, recvlen);
    return 0;
}


