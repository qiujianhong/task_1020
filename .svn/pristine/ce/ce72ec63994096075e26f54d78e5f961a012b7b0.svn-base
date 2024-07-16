/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP对外通信模块 数据中心更新
* @date：    2020-3-5
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "message.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"


#include "msg_base.h"
#include "task_proc.h"

int     g_msgMonitor = 0;

/**********************************************************************
* @name      : main_task_init
* @brief     ：用采task消息管理初始化
* @param[in] ：
* @param[out]：
* @return    ：g_msgMonitor
* @Create    : 王津剑
* @Date      ：2020-1-2
* @Update    :
**********************************************************************/
int main_task_init()
{
    g_msgMonitor = task_msg_init(PD_MSG_MONITOR);
    return g_msgMonitor;
}

/**********************************************************************
* @name      : task_appmain_recv_msg
* @brief     ：收到相关消息后发送给APP104
* @param[in] ：MESSAGE_INFO_T **ppMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-2
* @Update    : 2020-6-2
**********************************************************************/
int task_appmain_recv_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    
    if(MESSAGE_IID_CTS == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case CTS_IOP_READ_LIST_REQ:     msgid = MSG_APPAMIN_CTS_READ_LIST;    break;
            case CTS_IOP_READ_ONE_REQ:      msgid = MSG_APPAMIN_CTS_READ_ONE;     break;
            case CTS_IOP_SET_LIST_REQ:      msgid = MSG_APPAMIN_CTS_SET_LIST;     break;
            case CTS_IOP_SET_READ_LIST_REQ: msgid = MSG_APPAMIN_CTS_SETREAD_LIST; break;
            case CTS_IOP_ACT_LIST_REQ:      msgid = MSG_APPAMIN_CTS_ACT_LIST;     break;
            case CTS_IOP_ACT_READ_LIST_REQ: msgid = MSG_APPAMIN_CTS_ACTREAD_LIST; break;
            case CTS_IOP_TRANSPORT_REQ:     msgid = MSG_APPAMIN_CTS_TRANSPORT;    break;
            case CTS_IOP_CARRIRT_TRANS:     msgid = MSG_APPAMIN_CTS_CARRIRT_TRANS;break;
            case CTS_IOP_SEEK_METER:        msgid = MSG_APPAMIN_CTS_SEEK_METER;   break;
            default: return TASK_UNKNOW_MSG;
        }
    }
    else if(MESSAGE_IID_RSPSAMPLE == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case RSPSAMPLE_IOP_RMT_EVENT:   msgid = MSG_PULSE_GET_RMT_ENENT;      break;
            case RSPSAMPLE_IOP_RMT_STATE:   msgid = MSG_PULSE_GET_RMT;            break;
            default: return TASK_UNKNOW_MSG;
        }
    }
    else
    {
        return TASK_UNKNOW_MSG;
    }
    
    ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, PD_APP104_MSG);
    if(0 == ret)
    {
        PDMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", PD_APP104_MSG, msgid, pMsgInfo->label, ret);
    }
    else
    {
        PDMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", PD_APP104_MSG, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_comm_recv_wire
* @brief     ：收到无线拨号消息后发送给comm
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int task_comm_recv_wire(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    
    if(MESSAGE_IID_WIRELESS == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case WIRELESS_IOP_GET_DIAGINFO:   msgid = MSG_COMM_GETDIAL;    break;
            case WIRELESS_IOP_DIAG_EVENT:     msgid = MSG_COMM_DIALEVENT;  break;
            case WIRELESS_IOP_SET_DIAGARG:    msgid = MSG_COMM_SETAPN;     break;
            default: return TASK_UNKNOW_MSG;
        }
    }
    else if(MESSAGE_IID_SYSTEM == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case SYSTEM_IOP_GET_ETHSTATUS:    msgid = MSG_COMM_GETETH;    break;
            case SYSTEM_IOP_EVENT_ETHSTATUS:  msgid = MSG_COMM_ETHEVENT;  break;
            case SYSTEM_IOP_ADD_ROUTE:
            {
                ret = task_ack_syncmsg(g_msgMonitor, MSG_COMM_ADDROUTE, pMsgInfo->playload, pMsgInfo->msglen, 
                                       PD_COMM_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", PD_COMM_SYNCMSG, MSG_COMM_ADDROUTE, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_DEL_ROUTE:
            {
                ret = task_ack_syncmsg(g_msgMonitor, MSG_COMM_DELROUTE, pMsgInfo->playload, pMsgInfo->msglen, 
                                       PD_COMM_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", PD_COMM_SYNCMSG, MSG_COMM_DELROUTE, pMsgInfo->label, ret);
                return ret;
            }

            default: return TASK_UNKNOW_MSG;
        }
    }
    else
    {
        return TASK_UNKNOW_MSG;

    }

    ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, PD_COMM_MSG);
    if(0 == ret)
    {
        PDMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", PD_COMM_MSG, msgid, pMsgInfo->label, ret);
    }
    else
    {
        PDMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", PD_COMM_MSG, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_recv_mqttmsg
* @brief     ：收到外部消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int task_recv_mqttmsg(MESSAGE_INFO_T *pMsgInfo)
{
    int ret = 0;
    switch(pMsgInfo->IID)
    {
        case MESSAGE_IID_WIRELESS: return task_comm_recv_wire(pMsgInfo);
        case MESSAGE_IID_CTS:      return task_appmain_recv_msg(pMsgInfo);
        case MESSAGE_IID_SYSTEM:
        {
            if(SYSTEM_IOP_EVENT_ETHSTATUS == pMsgInfo->IOP || SYSTEM_IOP_GET_ETHSTATUS == pMsgInfo->IOP || 
               SYSTEM_IOP_ADD_ROUTE == pMsgInfo->IOP || SYSTEM_IOP_DEL_ROUTE == pMsgInfo->IOP)
            {
                return task_comm_recv_wire(pMsgInfo);
            }
            if(SYSTEM_IOP_REQUIRE_RESTART == pMsgInfo->IOP)
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_REBOOT, pMsgInfo->playload, pMsgInfo->msglen, 
                                       PD_APP104_SYNC, (uint8)pMsgInfo->label);
                PDMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", PD_APP104_SYNC, MSG_APPAMIN_REBOOT, pMsgInfo->label, ret);
                return ret;
            }
            if(SYSTEM_IOP_RCT == pMsgInfo->IOP)//时钟同步
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_RCT, pMsgInfo->playload, pMsgInfo->msglen, 
                                       PD_APP104_SYNC, (uint8)pMsgInfo->label);
                PDMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", PD_APP104_SYNC, MSG_APPAMIN_RCT, pMsgInfo->label, ret);
                return ret;
            }
            break;
        }
        case MESSAGE_IID_RSPSAMPLE: return task_appmain_recv_msg(pMsgInfo);
        default: return TASK_UNKNOW_MSG;
    }
    return TASK_UNKNOW_MSG;
}

