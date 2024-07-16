/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级app用采mqtt消息处理
* @date：    2019-12-24
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include <net/if.h>
#include "oopType.h"
#include "appmsg_split.h"
#include "mqtt_client.h"

#include "out_msg.h"
#include "framecomm.h"

BOOL g_getdbstate;

// 对外消息 客户端句柄
MQTT_CLIENT_T *g_GuiMqttClient;

pthread_mutex_t   g_GuiLibMsgSendlock; 
pthread_mutex_t   g_GuiLibMsgIndexlock; 
uint16            g_GuiLibAppMsgindex = 0;

#define  APP_RECV_BUF_LEN           5120	/*recvfrom接收缓冲区大小*/

/**********************************************************************
* @name      : gui_next_index_init
* @brief     ：对外消息index管理初始化
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
void gui_next_index_init()
{
    pthread_mutex_init(&g_GuiLibMsgIndexlock, NULL);
    pthread_mutex_init(&g_GuiLibMsgSendlock, NULL);

    g_GuiLibAppMsgindex = 0;
}

/**********************************************************************
* @name      : gui_get_next_index
* @brief     ：对外消息index分配
* @param[in] ：
* @param[out]：
* @return    ：index
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
uint16 gui_get_next_index()
{
    uint16 curindex = 0;
    
    PTHREAD_LOCK_SAFE(&g_GuiLibMsgIndexlock);
    curindex = g_GuiLibAppMsgindex;
    g_GuiLibAppMsgindex++;
    PTHREAD_UNLOCK_SAFE(&g_GuiLibMsgIndexlock);

    return curindex;
}

#if 0
/**********************************************************************
* @name      : gui_send_msg
* @brief     ：对外消息异步发送
* @param[in] ：char *pTopic              指定主题, NULL使用默认主题
               MESSAGE_INFO_T *pMsgInfo  对外消息
               int qos                   消息级别
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
int gui_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos)
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
        GUI_FMT_TRACE("App send appmsg_package fail\n");
        return -1;
    }

    if(msglen >= APP_RECV_BUF_LEN)
    {
        GUI_FMT_TRACE("App send appmsg_package too long! IID: 0x%x, IOP: 0x%x, source %s, dest %s\n",
                         pMsgInfo->IID, pMsgInfo->IOP, pMsgInfo->souraddr, pMsgInfo->destaddr);
        free(pMsg);
        return -2;
    }

    /* 两端都是MQTT */
    PTHREAD_LOCK_SAFE(&g_GuiLibMsgSendlock);
    if(NULL != pTopic /*&& TRUE == g_BMqtt*/ && 'M' == pMsgInfo->destaddr[0])
    {
        ret = mqtt_client_send_msg(g_GuiMqttClient, pTopic, qos, pMsg, (int)msglen);
        GUI_FMT_TRACE("MQTT Msg out: topic %s, qos %d\n", pTopic, qos);
    }
    else
    {
        ret = mqtt_client_send(g_GuiMqttClient, pMsg, (int)msglen);
    }
    PTHREAD_UNLOCK_SAFE(&g_GuiLibMsgSendlock);

    if(0 == ret)
    {
        GUI_BUF_TRACE(pMsg, msglen, "Send msg:");
    }
    else
    {
        GUI_FMT_DEBUG("Send mqtt msg failed: index %u, label %u, iid: 0x%x, iop: 0x%x, ret %d\n", 
                        pMsgInfo->index, pMsgInfo->label, pMsgInfo->IID, pMsgInfo->IOP, ret);
    }

    free(pMsg);
    return ret;
}
#endif

