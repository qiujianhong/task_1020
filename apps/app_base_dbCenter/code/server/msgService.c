/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved.
* @brief�� ��Ϣ����ӿ�
* @date�� 2019-11-9
*********************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "baseType.h"
#include "constant.h"
#include "dbCenterConfig.h"
#include "globalConfiguration.h"
#include "dbDataInterface.h"
#include "dbComm.h"
#include "mqttService.h"
#include "msgService.h"
#include "msgServiceBase.h"
#include "threadpool.h"
#include "timeHelper.h"
#include "udpService.h"
#include "xdrCustom.h"
#include "olog.h"
#include "smiOSClient.h"

static MSG_SERVICE_T *s_msgService = NULL;
extern void *     db_msg_ocat ;             //log print
extern uint8 g_app_id[25];

/**
*********************************************************************
* @brief��      ��û������� INDEX
* @param[in]��
* @param[out]�� none
* @return��     ����ֵ˵��    ��������INDEX
*********************************************************************
*/
static uint16 get_msg_index(void)
{
    return __sync_add_and_fetch(&s_msgService->msgIndex, 1);
}

/**
*********************************************************************
* @brief��          ��¼��ɻص�
* @param[in]��      ack        1 ����ɹ�   0 ����ʧ��
* @param[out]��     none
* @return��         ����ֵ˵��
*********************************************************************
*/
sint32 login_cb(sint8 ack)
{
    olog_debug(db_msg_ocat,"=====>login_cb ack = %d", ack);
    // lock
    if (ack != 0)
    {
        s_msgService->status = LOGIN_DONE;
    }
    return 0;
}

/**
*********************************************************************
* @brief��   ���͹㲥��Ϣ
* @param[in]��
			  IOP      ��Ϣ�ӿ�IOP
              dataType �������ʶ��0��û���������ʶ
              data     ���ݻ�����
              dataLen  ���ݻ���������
* @param[out]��none
* @return�� ����ֵ˵�� ����0�ɹ� -1ʧ��
*********************************************************************
*/
sint32 sendto_broadcast(uint8 IOP, uint16 dataType, sint8 *data, uint32 dataLen)
{
	sint8 dest[64]     = {0};

	if(dataType > 0 && dataType < ENUM_NOT_INIT_DATA)
	{
		snprintf(dest, sizeof(dest), "%s/0005/%04X/%u", BROADCAST_NAME, IOP, dataType);
	}
	else
	{
		snprintf(dest, sizeof(dest), "%s/0005/%04X", BROADCAST_NAME, IOP);
	}

	mqtt_sendto(dest, data, dataLen);
    return 0;
}



/**
*********************************************************************
* @brief��   ���͵�¼smiOS����
* @param[in]��
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
static void send_login(void)
{
    MSG_OBJ_T    msgObj = {0};
    MQT_PLUGIN_T mqt;
    RAW_OBJ_T    rsp    = {0};

    memset(&mqt, 0, sizeof(MQT_PLUGIN_T));
    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    //��ȡ��Ϣindex
    msgObj.index       = get_msg_index();
    msgObj.label       = 0;
    //�����ϢԴ
    msgObj.source      = g_configObj.serviceCfg.serverName;
    msgObj.destination = s_msgService->smiOSName;
    //������Ϣtag
    msgObj.tag         = 0x00010010;

    strcpy((char*)(mqt.comName), g_configObj.serviceCfg.serverName);
	memcpy(mqt.version, APP_VERSION, strlen(APP_VERSION));
    mqt.subr.subscribeInfoNum   = 0x0;
    uint32 a, b, c, d, e, f;
    sscanf(APP_PUBLISH_TIME, "%u/%u/%u %u:%u:%u", &a, &b, &c, &d, &e, &f);
    e = 0;
    f = 0;
    
    mqt.dataTime.year   = (uint16)a;
    mqt.dataTime.month  = (uint8)b;
    mqt.dataTime.day    = (uint8)c;
    mqt.dataTime.hour   = (uint8)d;
    mqt.dataTime.minute = (uint8)e;
    mqt.dataTime.second = (uint8)f;
    mqt.appidlen = 24;
    memcpy(mqt.appid, g_app_id, mqt.appidlen);
    build_login_msg(&msgObj, &mqt, &rsp);
    if (rsp.length > 0)
    {
        olog_debug(db_msg_ocat,"===sendto_smiOS");
        //������Ϣ��smiOS
        holog_info(db_msg_ocat, rsp.payload, rsp.length);
        sendto_smiOS((sint8 *)(rsp.payload), rsp.length);
    }
}

/**
*********************************************************************
* @brief��   �������ݸ����¼�
* @param[in]��minorDI_num=0 ��ʶ ������OPTIONAL Ϊ0
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
void send_data_update_event(uint8 logicalDeviceNum, uint16 infoNum, uint16 dataTypeID, uint8 majorDIOptinal, uint32 majorDI, uint32* minorDI, uint8 minorDI_num)
{
    MSG_OBJ_T msgObj               = {0};
    uint8     buf[MAX_PAYLOAD_LEN] = {0};
    RAW_OBJ_T rsp                  = {0};
    int       i                    = 0;
    // dzlog_debug("send_data_update_event");
    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();
    msgObj.label       = 0;
    msgObj.source      = g_configObj.serviceCfg.serverName;
    msgObj.destination = BROADCAST_NAME;
    msgObj.tag         = 0x00050001;

    uint8 *p = buf;
    push_u8(&p, logicalDeviceNum);
    push_u16(&p, infoNum);
    push_u16(&p, dataTypeID);

	if (minorDI_num == 0)	//�������ʶ	OPTIONAL Ϊ0
    {
        push_u8(&p, 0);
    }
    else    
    {
    	push_u8(&p, 1); //�������ʶ	OPTIONAL Ϊ1
		
        if (majorDIOptinal == _OPTIONSL_TRUE)
	    {
	        push_u8(&p, _OPTIONSL_TRUE);
	        push_u32(&p, majorDI);
	    }
	    else    
	    {
	        push_u8(&p, _OPTIONSL_FALSE);    
	    }   

		push_u8(&p, minorDI_num);
	    for(i=0; i<minorDI_num; i++)
	    {
	        push_u32(&p, minorDI[i]);
	    }
    }
	
    msgObj.length = p - buf;
    msgObj.payload = buf;

    build_msg(&msgObj, buf, msgObj.length, &rsp);
    // dzlog_debug("msgObj.length :%d", msgObj.length);
    if (rsp.length > 0)
    {
        //���͹㲥��Ϣ
        sendto_broadcast(0x0001, dataTypeID, (sint8 *)(rsp.payload), rsp.length);
    }
}

/**
*********************************************************************
* @brief��   �������ݿ��ʼ���¼�
* @param[in]��
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
void send_db_init_event(uint8 eventType, uint8 logicalDeviceNum, uint8 dataClassIDOption, uint16 dataClassID)
{
    MSG_OBJ_T msgObj               = {0};
    uint8     buf[MAX_PAYLOAD_LEN] = {0};
    RAW_OBJ_T rsp                  = {0};
	uint16    dataTypeID		   = 0;

    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();
    msgObj.label       = 0;
    msgObj.source      = g_configObj.serviceCfg.serverName;
    msgObj.destination = s_msgService->smiOSName;
    //�������ݳ�ʼ��tag
    msgObj.tag         = 0x00050002;

    olog_debug(db_msg_ocat,"send_db_init_event");
    uint8 *p = buf;
    push_u8(&p, eventType);

    if (1 == eventType)
    {
        push_u8(&p, logicalDeviceNum);
    }
    else if (0 == eventType)
    {
        push_u8(&p, logicalDeviceNum);
        push_u8(&p, dataClassIDOption);
        if (dataClassIDOption)
        {
            push_u16(&p, dataClassID);
			dataTypeID = dataClassID;
        }
    }
    else
    {
        return;
    }

    msgObj.length = p - buf;
    msgObj.payload = buf;

    olog_debug(db_msg_ocat,"msgObj.length :%d", msgObj.length);
    build_msg(&msgObj, buf, msgObj.length, &rsp);
    if (rsp.length > 0)
    {
        olog_debug(db_msg_ocat,"send data update eventType:[%u] did:[%u]", eventType,
                    logicalDeviceNum);
        //���͹㲥��Ϣ
        sendto_broadcast(0x0002, dataTypeID, (sint8 *)(rsp.payload), rsp.length);
    }
}

/**
*********************************************************************
* @brief��   ��ʼ��smiOS�ͻ����߳�
* @param[in]��
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
/*
static sint32 send_heartbeat(void)
{
    MSG_OBJ_T    msgObj = {0};
    MQT_PLUGIN_T mqt    = {0};
    RAW_OBJ_T    rsp    = {0};

    msgObj.prm         = 0x01;
    msgObj.priority    = 0;
    msgObj.index       = get_msg_index();
    msgObj.label       = 0;
    msgObj.source      = g_configObj.serviceCfg.serverName;
    msgObj.destination = s_msgService->smiOSName;
    msgObj.tag         = 0x00010010;

    strcpy(mqt.comName, g_configObj.serviceCfg.serverName);
    mqt.version         = 100;
    mqt.subscribeInfo   = 0x1;
    mqt.dataTime.year   = 2019;
    mqt.dataTime.month  = 11;
    mqt.dataTime.day    = 26;
    mqt.dataTime.hour   = 13;
    mqt.dataTime.minute = 36;
    mqt.dataTime.second = 55;
    build_login_msg(&msgObj, &mqt, &rsp);
    if (rsp.length > 0)
    {
        // dzlog_debug("===sendto_smiOS");
        //������Ϣ��smiOS
        sendto_smiOS(rsp.payload, rsp.length);
    }
}
*/


/**
*********************************************************************
* @brief��   ��ʼ��smiOS�ͻ����߳�
* @param[in]��
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
/*
static sint32 init_smiOS_connect(void)
{
    sint32 ret = 0;
    switch (s_msgService->connectType)
    {
        case UDP:
            break;
        case MQTT:
            break;
        case TCP:
            dzlog_debug("xl_channel_create url:%s", s_msgService->url);
            ret =
                xl_channel_create(s_msgService->url, &s_msgService->ctx);
            break;
        default:
            dzlog_error("not support connect type :[%s]",
                        str_msg_type(s_msgService->connectType));
            break;
    }
    return ret;
}
*/

/**
*********************************************************************
* @brief��   ����smiOS
* @param[in]��
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
/*
static sint32 smiOS_connect(void)
{
    sint32 ret = 0;
    switch (s_msgService->connectType)
    {
        case UDP:
            break;
        case MQTT:
            break;
        case TCP:
            ret = xl_channel_connect(s_msgService->ctx);
            break;
        default:
            olog_error(db_ser_ocat,"not support connect type :[%s]",
                        str_msg_type(s_msgService->connectType));
            break;
    }
    return ret;
}
*/
/**
*********************************************************************
* @brief��   �ж������Ƿ�ɹ�
* @param[in]��
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
/*
static sint32 is_smiOS_connected(void)
{
    sint32 ret = 0;
    switch (s_msgService->connectType)
    {
        case UDP:
            break;
        case MQTT:
            break;
        case TCP:
            ret = xl_channel_get_connect_status(s_msgService->ctx);
            break;
        default:
            olog_error(db_ser_ocat,"not support connect type :[%s]",
                        str_msg_type(s_msgService->connectType));
            break;
    }
    return ret;
}
*/

/**
*********************************************************************
* @brief��   ����smiOS�ͻ����߳�
* @param[in]��
               arg         ���˵��
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
static void* smiOS_connect_task(void *arg)
{
    //sint32                login_wait_count = 0;
    static struct timespec last_heary       = {0};
    struct timespec        now;
    //sint32 connect_state;
    do
    {
        switch (s_msgService->status)
        {
            case CONNECT_INIT:
                if (0 == init_smiOS_connect(s_msgService->connectType,s_msgService->smiOSIP,s_msgService->smiOSUdpPort))
                {
                    s_msgService->status = LOGIN_START;
                    continue;
                }
                break;
/*               
            case CONNECT_INIT_START:
                if( 0 == smiOS_connect())
                {
                    s_msgService->status = CONNECT_WAIT;
                    continue;
                }
                break;
            case CONNECT_WAIT:

                connect_state = is_smiOS_connected();
                if (CONNECT_SUCC == connect_state)
                {
                    s_msgService->status = LOGIN_START;
                    continue;
                }
                else if (CONNECT_FAIL == connect_state)
                {
                    xl_channel_disconnect(s_msgService->ctx);
                    s_msgService->status = CONNECT_INIT_START;
                    continue;
                }
                break;
*/
            case LOGIN_START:
                send_login();
                getSysTime(&last_heary);
//				olog_info(db_msg_ocat, "LOGIN_START time %d", (int)last_heary.tv_sec);
                s_msgService->status = LOGIN_WAIT_DONE;
                /* code */
                break;
            case LOGIN_WAIT_DONE:
                getSysTime(&now);
//				olog_info(db_msg_ocat, "LOGIN_WAIT_DONE time %d", (int)now.tv_sec);
                if (compare_timeout_s(&last_heary, &now, SMIOS_LOGIN_TIMEOUT))
                {
                    //dzlog_debug("===>login_wait_count > 10");
                    s_msgService->status = LOGIN_START;
                }
                break;
            case LOGIN_DONE:
                break;
            case LOGOUT:
                /* code */
                break;
            default:
                break;
        }
        sleep(1);
    } while (1);

    return NULL;
}

/**
*********************************************************************
* @brief�� ��Ϣ�������
* @param[in]��
               arg         ���˵��
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
static void handle_task(void *arg)
{
    MSG_HANDER_OBJ_T *msgHanderObj = (MSG_HANDER_OBJ_T *)arg;
    RAW_OBJ_T *       rawObjReq;
    RAW_OBJ_T *       rawObjRsp;
    MSG_OBJ_T *       msgReq;
    MSG_OBJ_T *       msgRsp;
    MSG_CLIENT_T      client = {0};

    // dzlog_info("msg handle task run");

#ifdef XL_TIME_TRACE
    getSysTime(&msgHanderObj->dequeueDone);
#endif

    do
    {
        msgHanderObj->getReqDataFp(msgHanderObj->obj, &rawObjReq);
        msgHanderObj->getRspDataFp(msgHanderObj->obj, &rawObjRsp);
        msgHanderObj->getReqDataMsgFp(msgHanderObj->obj, &msgReq);
        msgHanderObj->getRspDataMsgFp(msgHanderObj->obj, &msgRsp);
        msgHanderObj->getClientInfo(msgHanderObj->obj, &client);

#ifdef XL_TIME_TRACE
        getSysTime(&msgHanderObj->handerStart);
#endif
        msg_handler(rawObjReq, rawObjRsp, msgReq, msgRsp, &client);
        if (msgHanderObj != NULL && msgHanderObj->handerFp != NULL)
        {
            struct timespec now;
            getSysTime(&now);
            olog_debug(db_msg_ocat,"msg Handle [%ld][%ld] tag=0x%x",now.tv_sec,now.tv_nsec ,msgRsp->tag);

            msgHanderObj->handerFp(msgHanderObj);  //ִ����Ϣ�Ļظ�
        }

    } while (0);
}

/**
*********************************************************************
* @brief�� ������Ϣ�ӿ�
* @param[in]��
               msgHanderObj
* @param[out]��none
* @return�� ����ֵ  ����0��ӳɹ� ����<0 ����������ʧ��
*********************************************************************
*/
sint32 queue_message(MSG_HANDER_OBJ_T *msgHanderObj)
{
    //����Ϣ����������������
#ifdef XL_TIME_TRACE
    getSysTime(&msgHanderObj->queueDone);
#endif
    if(s_msgService != NULL)
    {
        return threadpool_add(s_msgService->pool, &handle_task, msgHanderObj, 0);
    }
    else
    {
        return -1;
    }
    

}

static MQTT_SERVER_SUB_TOPIC_T sub[] = {{.topic = "+/M-dbCenter", .qos = 0}};

static void mqtt_subscribe_fp(MQTT_SERVER_SUB_TOPIC_T **subTopic, sint32 *subNum)
{
    *subTopic = sub;
    *subNum   = sizeof(sub) / sizeof(sub[0]);
}

/**
*********************************************************************
* @brief�� ��Ϣ��ʼ��ϵͳ�ӿ�
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
sint32 msg_init_system(void)
{
    olog_info(db_msg_ocat, "msgInitSystem\n");

    set_subscribe_cb_fp(mqtt_subscribe_fp);
    
    sint32 ret   = -1;
    s_msgService = (MSG_SERVICE_T *)malloc(sizeof(MSG_SERVICE_T));
    if (NULL != s_msgService)
    {
        //��ȫ�����û�ȡ�̳߳���
        s_msgService->threadNum = g_configObj.serviceCfg.handerThreadNum;
        //��ȫ�����û�ȡ�����������
        s_msgService->queueSize = g_configObj.serviceCfg.handerTaskQueueSize;
        s_msgService->tbl       = qhashtbl(0, QHASHTBL_THREADSAFE);
        set_queue_message_fp(queue_message);
        s_msgService->status = INIT;

        set_login_cb_fp(login_cb);
        strncpy(s_msgService->smiOSIP, g_configObj.serviceCfg.smiOSIP,
                sizeof(s_msgService->smiOSIP));
        s_msgService->smiOSUdpPort = g_configObj.serviceCfg.smiOSPort;
        if (0 == strncmp("UDP", g_configObj.serviceCfg.smiOSConnectType,
                         CONNECT_TYPE_LEN))
        {
            s_msgService->connectType = UDP;
            //s_msgService->smiOSName;
            snprintf(s_msgService->smiOSName, sizeof(s_msgService->smiOSName), "U-%s:%s[%u]", SMIOS_NAME_BASE,
                     g_configObj.serviceCfg.smiOSIP, g_configObj.serviceCfg.smiOSPort);
        }
        else if (0 == strncmp("MQTT", g_configObj.serviceCfg.smiOSConnectType,
                              CONNECT_TYPE_LEN))
        {

            s_msgService->connectType = MQTT;
            snprintf(s_msgService->smiOSName, sizeof(s_msgService->smiOSName), "M-%s", SMIOS_NAME_BASE);
        }
        else if (0 == strncmp("TCP", g_configObj.serviceCfg.smiOSConnectType,
                              CONNECT_TYPE_LEN))
        {
            s_msgService->connectType = TCP;
            snprintf(s_msgService->smiOSName, sizeof(s_msgService->smiOSName), "T-%s:%s[%u]", SMIOS_NAME_BASE,
                     g_configObj.serviceCfg.smiOSIP, g_configObj.serviceCfg.smiOSPort);
            sprintf(s_msgService->url, "tcp://%s:%u",
                    g_configObj.serviceCfg.smiOSIP,
                    g_configObj.serviceCfg.smiOSPort);
        }
        else if (0 == strncmp("IPC", g_configObj.serviceCfg.smiOSConnectType,
                              CONNECT_TYPE_LEN))
        {
            s_msgService->connectType = IPC;
        }
        else
        {
            olog_debug(db_msg_ocat,"msg_init_system fail");
            return -1;
        }

        olog_info(db_msg_ocat, "s_msgService->connectType :%s",g_configObj.serviceCfg.smiOSConnectType);
        if (s_msgService->tbl != NULL)
        {
            ret = 0;
        }
    }
    return ret;
}

/**
*********************************************************************
* @brief�� ��Ϣ����ϵͳ�ӿ�
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
void msg_clean_up_system(void) {}

/**
*********************************************************************
* @brief�� ��Ϣ����ϵͳ�ӿ�
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
sint32 msg_start_system(void)
{
    sint32 ret = -1;

    olog_debug(db_msg_ocat,"msg_start_system");
    //�����̳߳�
    s_msgService->pool = threadpool_create(s_msgService->threadNum,
                                           s_msgService->queueSize, 0);



    if (NULL != s_msgService->pool)
    {
        olog_info(db_msg_ocat,"Thread pool started with %d threads and "
            "queue size of %d\n",
            s_msgService->threadNum, s_msgService->queueSize);
        ret = 0;
    }
    else
    {
        olog_error(db_msg_ocat,"thread pool create fail");
        return ret;
    }

    ret = pthread_create(&s_msgService->smiOSTid, NULL, smiOS_connect_task, NULL);
    if(ret)
    {
        olog_error(db_msg_ocat,"simOS connect thread create fail");
    }

    return ret;
}

/**
*********************************************************************
* @brief�� ��Ϣֹͣϵͳ�ӿ�
* @param[in]�� none
* @param[out]��none
* @return�� ����ֵ˵��
*********************************************************************
*/
void msg_stop_system(void)
{
    olog_debug(db_msg_ocat,"msg_stop_system");
    olog_debug(db_msg_ocat,"====>threadpool_destroy");
    threadpool_destroy(s_msgService->pool,0);
    s_msgService->pool = NULL;
    olog_debug(db_msg_ocat,"====>threadpool_destroy done");
}

/**
*********************************************************************
* @brief��     ��Ϣ�������� [����]
* @param[in]��
                subscriber_name  ����������
                did              �߼��豸ID
                info             ��Ϣ������
                infoNum          ��Ϣ�����
                dataTag          ���ݱ�ʶ
                dataTagNum       ���ݱ�ʶ����
* @param[out]��none
* @return�� ����ֵ  ����0��ӳɹ� ����<0 ����������ʧ��
*********************************************************************
*/
sint32 msg_subscribe(void *  subscriber_name,
                     uint8   did,
                     uint16 *info,
                     uint8   infoNum,
                     uint32 *dataTag,
                     uint8   dataTagNum)
{
    sint8       key[128] = {};
    size_t      s;
    sint32      i, j;
    qlist_t *   list = NULL;
    qlist_obj_t obj;
    qhashtbl_t *tbl          = s_msgService->tbl;
    sint32      flag_has_sub = 0;

    // sub once return true
    sint32 sub_succ = 0;
    for (i = 0; i < infoNum; i++)
    {
        for (j = 0; j < dataTagNum; j++)
        {
            snprintf(key, sizeof(key), "%d-%d-%d", did, info[i], dataTag[j]);
            list = tbl->get(tbl, key, &s, 0);
            if (list == NULL)
            {
                list = qlist(QLIST_THREADSAFE);
                list->addlast(list, subscriber_name, strlen(subscriber_name));
                tbl->put(tbl, key, &list, sizeof(list));
                sub_succ = 1;
            }
            else
            {
                flag_has_sub = 0;
                memset((void *)&obj, 0,
                       sizeof(obj));  // must be cleared before call
                list->lock(list);
                while (list->getnext(list, &obj, false) == true)
                {
                    // printf("DATA=%s, SIZE=%zu\n", (sint8 *)obj.data,
                    // obj.size);
                    if (0 == strcmp((sint8 *)obj.data, subscriber_name))
                    {
                        flag_has_sub = 1;
                        break;
                    }
                }
                if (!flag_has_sub)
                {
                    list->addlast(list, subscriber_name,
                                  strlen(subscriber_name));
                    sub_succ = 1;
                }
                list->unlock(list);
            }
        }
    }

    return sub_succ;
}
