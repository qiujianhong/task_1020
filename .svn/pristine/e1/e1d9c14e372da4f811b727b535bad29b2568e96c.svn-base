/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP对外通信模块 数据中心更新
* @date：    2019-12-31
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "message.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "label_id.h"

#include "msg_base.h"
#include "task_proc.h"


int     g_msgMonitor = 0;

extern int advance_send_appmsg(MESSAGE_INFO_T *pMsgInfo);

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
    g_msgMonitor = task_msg_init(MSG_MONITOR);
    return g_msgMonitor;
}

/**********************************************************************
* @name      : task_appmain_cts
* @brief     ：主调度应用发送采集任务调度消息
* @param[in] ：TASK_MSG_t *pMsg            内部消息
* @param[out]：
* @return    ：IOP
* @Create    : 王津剑
* @Date      ：2020-1-6
* @Update    :
**********************************************************************/
uint16 task_appmain_get_send_iop(TASK_MSG_t *pMsg)
{
    switch(pMsg->msg_id)
    {
        case MSG_APPAMIN_CTS_READ_LIST:     return CTS_IOP_READ_LIST_REQ;
        case MSG_APPAMIN_CTS_READ_ONE:      return CTS_IOP_READ_ONE_REQ;
        case MSG_APPAMIN_CTS_SET_LIST:      return CTS_IOP_SET_LIST_REQ;
        case MSG_APPAMIN_CTS_SETREAD_LIST:  return CTS_IOP_SET_READ_LIST_REQ;
        case MSG_APPAMIN_CTS_ACT_LIST:      return CTS_IOP_ACT_LIST_REQ;
        case MSG_APPAMIN_CTS_ACTREAD_LIST:  return CTS_IOP_ACT_READ_LIST_REQ;
        case MSG_APPAMIN_CTS_TRANSPORT:     return CTS_IOP_TRANSPORT_REQ;
        case MSG_APPAMIN_CTS_CARRIRT_TRANS: return CTS_IOP_CARRIRT_TRANS;
        case MSG_APPAMIN_CTS_SEEK_METER:    return CTS_IOP_SEEK_METER;
        case MSG_APPAMIN_CTS_RPTSTATE:      return CTS_IOP_TRANSTASK_STATE;
		case MSG_APPAMIN_POWERUP_EVT:       return CTS_IOP_POWERUP_EVT;
        
    }  
    return 0;
}

/**********************************************************************
* @name      : task_appmain_recv_msg
* @brief     ：收到采集任务调度消息后发送给APPmain
* @param[in] ：MESSAGE_INFO_T **ppMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-2
* @Update    :
**********************************************************************/
int task_appmain_recv_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;

    if(MESSAGE_IID_COMMON == pMsgInfo->IID) //通用消息    
    {
        switch(pMsgInfo->IOP)
        {
            case COMMON_IOP_SYSTEM_RESTART: msgid = MSG_APPAMIN_RESTART; break;
            default: return TASK_UNKNOW_MSG;
        }
    }
    else if(MESSAGE_IID_CTS == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case CTS_IOP_READ_LIST_REQ:     
            {
                if (pMsgInfo->label == LABEL_HALT_TRANS)
                {
                    /* 返回同步消息 */
                    ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_CTS_READ_LIST, pMsgInfo->playload, pMsgInfo->msglen, 
                                           APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                    YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_CTS_READ_LIST, pMsgInfo->label, ret);
                    return ret;
                }
                else
                {
                    msgid = MSG_APPAMIN_CTS_READ_LIST;
                }
            }
            break;
            case CTS_IOP_READ_ONE_REQ:      msgid = MSG_APPAMIN_CTS_READ_ONE;     break;
            case CTS_IOP_SET_LIST_REQ:      msgid = MSG_APPAMIN_CTS_SET_LIST;     break;
            case CTS_IOP_SET_READ_LIST_REQ: msgid = MSG_APPAMIN_CTS_SETREAD_LIST; break;
            case CTS_IOP_ACT_LIST_REQ:      msgid = MSG_APPAMIN_CTS_ACT_LIST;     break;
            case CTS_IOP_ACT_READ_LIST_REQ: msgid = MSG_APPAMIN_CTS_ACTREAD_LIST; break;
            case CTS_IOP_TRANSPORT_REQ:     msgid = MSG_APPAMIN_CTS_TRANSPORT;    break;
#ifdef AREA_FUJIAN            
            case CTS_IOP_TRANSTASK_RPT:     msgid = MSG_APPAMIN_TRANSTASK_RPT;    break;
#endif
            case CTS_IOP_CARRIRT_TRANS:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_CTS_CARRIRT_TRANS, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_CTS_CARRIRT_TRANS, pMsgInfo->label, ret);
                return ret;
            }
            case CTS_IOP_SEEK_METER:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_CTS_SEEK_METER, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_CTS_SEEK_METER, pMsgInfo->label, ret);
                return ret;
            }
            default: return TASK_UNKNOW_MSG;
        }
    }
    else if(MESSAGE_IID_SYSTEM == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP & 0x00FF)
        {
            case SYSTEM_IOP_RCT:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_RCT, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_RCT, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_REQUIRE_RESTART:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_REBOOT, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_REBOOT, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_SET_IPADDR:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_SET_IPADDR, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_SET_IPADDR, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_GET_DEVINFO:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_GET_DEVICEINFO, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_GET_DEVICEINFO, pMsgInfo->label, ret);
                return ret;
            }
			case SYSTEM_IOP_GET_CUR_DEVINFO:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_GET_CUR_DEVICEINFO, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_GET_CUR_DEVICEINFO, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_GET_CONTAINER:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_GET_CONTAINERINFO, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_GET_CONTAINERINFO, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_GET_CONTAINER_APP:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_GET_APPINFO, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_GET_APPINFO, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_SYSTEM_CMD:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_SYSTEM_CMD, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_SYSTEM_CMD, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_APP_MANAGER:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPMAIN_MANAGER_APP, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPMAIN_MANAGER_APP, pMsgInfo->label, ret);
                return ret;
            }
            default: return TASK_UNKNOW_MSG;
        }
    }
    else if(MESSAGE_IID_SCS == pMsgInfo->IID) //安全在线监测消息
    {
        switch(pMsgInfo->IOP)
        {
            case SCS_IOP_ETH_PORT_OPEN:      msgid = MSG_APPAMIN_ETH_PORT_OPEN;      break;
            case SCS_IOP_ETH_PORT_CONN:      msgid = MSG_APPAMIN_ETH_PORT_CONN;      break;
            case SCS_IOP_KEY_PATH_CHGE:      msgid = MSG_APPAMIN_KEY_PATH_CHG;       break;
            case SCS_IOP_ETH_LOGIN_CONSOLE:  msgid = MSG_APPAMIN_ETH_LOGIN_CONSOLE;  break;
            case SCS_IOP_DANGER_OPREATE:     msgid = MSG_APPAMIN_DANGER_OPREATE;     break;
            case SCS_IOP_LOCAL_MAINTAIN:     msgid = MSG_APPAMIN_LOCAL_MAINTAIN;     break;
            case SCS_IOP_PASSWARD_CHANGED:   msgid = MSG_APPAMIN_PASSWARD_CHANGED;   break;
            case SCS_IOP_UART_PORT_LOGIN:    msgid = MSG_APPAMIN_UART_PORT_LOGIN;    break;
            case SCS_IOP_SESSION_ABNORMAL:   msgid = MSG_APPAMIN_SESSION_ABNORMAL;   break;
            case SCS_IOP_BANDWIDTH_ABNORMAL: msgid = MSG_APPAMIN_BANDWIDTH_ABNORMAL;   break;
            case SCS_IOP_PROCESS_ABNORMAL:   msgid = MSG_APPAMIN_PROCESS_ABNORMAL;   break;
            case SCS_IOP_LOGIN_ABNORMAL:     msgid = MSG_APPAMIN_LOGIN_ABNORMAL;   break;
            case SCS_IOP_SERVICE_OPEN:       msgid = MSG_APPAMIN_SSH_PORT_AUTHORIZE; break;

            case SCS_IOP_FLOW_PARAM_SET:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_FLOW_PARAM_SET, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_FLOW_PARAM_SET, pMsgInfo->label, ret);
                return ret;
            }
            case SCS_IOP_FLOW_PARAM_GET:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_FLOW_PARAM_GET, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_FLOW_PARAM_GET, pMsgInfo->label, ret);
                return ret;
            }
            case SCS_IOP_TIME_OUT_SET:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_TIME_OUT_SET, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_TIME_OUT_SET, pMsgInfo->label, ret);
                return ret;
            }
            default: return TASK_UNKNOW_MSG;
        }        
    }
    else if(MESSAGE_IID_PROXY == pMsgInfo->IID) //通信代理消息
    {
        switch(pMsgInfo->IOP)
        {
            case PROXY_IOP_APPLY_CONNECT:      msgid = MSG_APPAMIN_APPLYSTATUS;      break;
            default: return TASK_UNKNOW_MSG;
        }           
    }
	else if(MESSAGE_IID_DESKGUI == pMsgInfo->IID) //桌面消息
    {
        switch(pMsgInfo->IOP)
        {
            case DESKGUI_IOP_PUSH_HINT:        msgid = MSG_CTRL_GUI_HINT;            break;
            default: return TASK_UNKNOW_MSG;
        }
    }
    else if(MESSAGE_IID_ACMETER == pMsgInfo->IID) //厂家自定义消息
    {
        switch(pMsgInfo->IOP)
        {
            case ACMETER_IOP_AC_PARM_INIT:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_AC_PARAM_INIT, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_AC_PARAM_INIT, pMsgInfo->label, ret);
                return ret;

            }
            case ACMETER_IOP_AC_DATA_INIT:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_AC_DATA_INIT, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_AC_DATA_INIT, pMsgInfo->label, ret);
                return ret;

            }
            case ACMETER_IOP_AC_DEMAND_INIT:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_AC_DEMAND_INIT, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_AC_DEMAND_INIT, pMsgInfo->label, ret);
                return ret;

            }
            case ACMETER_IOP_AC_SYSTEM_CMD:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_AC_SYSTEM_CMD, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_AC_SYSTEM_CMD, pMsgInfo->label, ret);
                return ret;

            }
            case ACMETER_IOP_AC_RATE_SET:
            {
                /* 返回同步消息 */
                ret = task_ack_syncmsg(g_msgMonitor, MSG_APPAMIN_AC_RATE_SET, pMsgInfo->playload, pMsgInfo->msglen, 
                                       APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, MSG_APPAMIN_AC_RATE_SET, pMsgInfo->label, ret);
                return ret;

            }
            default: return TASK_UNKNOW_MSG;
        }     
    }
    else if(MESSAGE_IID_CCOROUTER == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case CCOROUTER_IOP_MTREVENT_RPT: msgid = MSG_APPAMIN_MTREVENT_RPT; break;
            default: return TASK_UNKNOW_MSG;
        }        
    }
    else
    {
        return TASK_UNKNOW_MSG;
    }

    ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, APPMAIN_MSG);
    if(0 == ret)
    {
        YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", APPMAIN_MSG, msgid, pMsgInfo->label, ret);
    }
    else
    {
        YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", APPMAIN_MSG, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_comm_recv_msg
* @brief     ：收到通信模块需要的消息后发送给comm
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int task_comm_recv_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    
    if(MESSAGE_IID_WIRELESS == pMsgInfo->IID)
    {
        if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
        {
            /* 规约中使用 */
            uint8 label = (uint8)pMsgInfo->label;
            switch(pMsgInfo->IOP & 0xff)
            {
                case WIRELESS_IOP_GET_MODULEINFO:   msgid = MSG_APPAMIN_GETPPPMODE;  break;
                case WIRELESS_IOP_GET_DIAGINFO:     msgid = MSG_APPAMIN_GETPPPDIAG;  break;
                case WIRELESS_IOP_GET_GPS:          msgid = MSG_APPAMIN_GETPPPGPS;   break;
                case WIRELESS_IOP_TRANS_AT:         msgid = MSG_APPMAIN_TRANSPORT;   break;
                default: return TASK_UNKNOW_MSG;
            }

            ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, APPMAIN_SYNCMSG, label);
            YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, msgid, label, ret);
            return ret;
        }
        
        switch(pMsgInfo->IOP)
        {
            case WIRELESS_IOP_GET_DIAGINFO:   msgid = MSG_COMM_GETDIAL;    break;
            case WIRELESS_IOP_DIAG_EVENT:     msgid = MSG_COMM_DIALEVENT;  break;
            case WIRELESS_IOP_SET_DIAGARG:    msgid = MSG_COMM_SETAPN;     break;
            case WIRELESS_IOP_GET_MODULEINFO: msgid = MSG_COMM_GET_MODULEINFO; break;
            case WIRELESS_IOP_SET_SMSNO:      msgid = MSG_COMM_SMSNO;      break;
            case WIRELESS_IOP_SEND_SMS:
            {
                ret = task_ack_syncmsg(g_msgMonitor, MSG_COMM_SMSSEND, pMsgInfo->playload, pMsgInfo->msglen, 
                                       COMM_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", COMM_SYNCMSG, MSG_COMM_SMSSEND, pMsgInfo->label, ret);
                return ret;
            }
            case WIRELESS_IOP_REVC_SMS:
            {
                ret = task_ack_syncmsg(g_msgMonitor, MSG_COMM_SMSRECV, pMsgInfo->playload, pMsgInfo->msglen, 
                                       COMM_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", COMM_SYNCMSG, MSG_COMM_SMSRECV, pMsgInfo->label, ret);
                return ret;
            }

            default: return TASK_UNKNOW_MSG;
        }
    }
    else if(MESSAGE_IID_SYSTEM == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case SYSTEM_IOP_GET_ETHSTATUS:    msgid = MSG_COMM_GETETH;    break;
            case SYSTEM_IOP_EVENT_ETHSTATUS:  msgid = MSG_COMM_ETHEVENT;  break;
            case SYSTEM_IOP_EVENT_IPCHANGE:   msgid = MSG_COMM_ETHCHANGEIP;  break;
            case SYSTEM_IOP_ADD_ROUTE:
            {
                ret = task_ack_syncmsg(g_msgMonitor, MSG_COMM_ADDROUTE, pMsgInfo->playload, pMsgInfo->msglen, 
                                       COMM_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", COMM_SYNCMSG, MSG_COMM_ADDROUTE, pMsgInfo->label, ret);
                return ret;
            }
            case SYSTEM_IOP_DEL_ROUTE:
            {
                ret = task_ack_syncmsg(g_msgMonitor, MSG_COMM_DELROUTE, pMsgInfo->playload, pMsgInfo->msglen, 
                                       COMM_SYNCMSG, (uint8)pMsgInfo->label);
                YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", COMM_SYNCMSG, MSG_COMM_DELROUTE, pMsgInfo->label, ret);
                return ret;
            }
            default: return TASK_UNKNOW_MSG;
        }
    }
    else if(MESSAGE_IID_PROXY == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case PROXY_IOP_PROTOCOL_DATA_SEND:   msgid = MSG_COMM_PROXYSEND;   break;
            case PROXY_IOP_PHYSIC_CONNECT:     msgid = MSG_COMM_PROXYCONNECT;  break;
            default: return TASK_UNKNOW_MSG;
        }        
    }
    else if(MESSAGE_IID_COMMON == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case COMMON_IOP_TRANSPORT:
            {
                /* APP透传698报文, 直接传递原始消息 */
                msgid = MSG_COMM_APP_TRANS;
                return task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, (uint8 *)pMsgInfo, 
                                   sizeof(MESSAGE_INFO_T) + pMsgInfo->msglen, COMM_MSG);
            }
            default: return TASK_UNKNOW_MSG;
        }
    }
    else
    {
        return TASK_UNKNOW_MSG;

    }

    ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, COMM_MSG);
    if(0 == ret)
    {
        YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", COMM_MSG, msgid, pMsgInfo->label, ret);
    }
    else
    {
        YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", COMM_MSG, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_recv_mapmanager_msg
* @brief     ：收到模组管理器消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int task_recv_mapmanager_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    uint32 dest = 0;
    
    if(MESSAGE_IID_MODULE == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case MODULE_IOP_GET_INFO:
            {
                if(MSG_PULSE_FLAG == (pMsgInfo->label & 0xff00))
                {
                    msgid = MSG_PULSE_MOD_GET_INFO;
                    dest = PULSE_MSG;
                }
                else if(MSG_CTRL_FLAG == (pMsgInfo->label & 0xff00))
                {
                    msgid = MSG_CTRL_MOD_GET_INFO;
                    dest = APPMAIN_MSG;
                }
                else if(MSG_COMM_FLAG == (pMsgInfo->label & 0xff00))
                {
                    msgid = MSG_COMM_MAPINFO;
                    dest = COMM_MSG;
                }
                else if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
                {
                    uint8 label = (uint8)pMsgInfo->label;
                    msgid = MSG_APPAMIN_GETMAPINFO;
                    ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, APPMAIN_SYNCMSG, label);
                    YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, msgid, label, ret);
                    return ret;
                }
                else
                {
                    dest = 0;
                }
                break;
            }
            case MODULE_IOP_GET_PORTINFO:
            {
                if(MSG_COMM_FLAG == (pMsgInfo->label & 0xff00))
                {
                    msgid = MSG_COMM_MAPSLOT;
                    dest = COMM_MSG;
                }
                else if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
                {
                    uint8 label = (uint8)pMsgInfo->label;
                    msgid = MSG_APPAMIN_GETMAPSLOT;
                    ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, APPMAIN_SYNCMSG, label);
                    YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, msgid, label, ret);
                    return ret;
                }                
                else
                {
                    dest = 0;
                }
                break;
            }
            case MODULE_IOP_GET_STATUS:
            {
                if(MSG_PULSE_FLAG == (pMsgInfo->label & 0xff00))
                {
                    msgid = MSG_PULSE_MOD_GET_STATUS;
                    dest = PULSE_MSG;
                }
                else if(MSG_CTRL_FLAG == (pMsgInfo->label & 0xff00))
                {
                    msgid = MSG_CTRL_MOD_GET_STATUS;
                    dest = APPMAIN_MSG;
                }
                else
                {
                    dest = 0;
                }
                break;
            }
            case MODULE_IOP_STATUS_EVENT:
            {
                dest = APPMAIN_MSG;
                msgid = MSG_APPAMIN_MOD_EVENT;
                task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, dest);
            
                msgid = MSG_CTRL_MOD_EVENT;
                task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, dest);

                dest = COMM_MSG;
                msgid = MSG_COMM_MAPEVENT;
                break;
            }
            case MODULE_IOP_GET_ALL:
            {
                if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
                {
                    uint8 label = (uint8)pMsgInfo->label;
                    msgid = MSG_APPAMIN_GETMAPALL;
                    ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, APPMAIN_SYNCMSG, label);
                    YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, msgid, label, ret);
                    return ret;
                }
                if(MSG_CTRL_FLAG == (pMsgInfo->label & 0xff00))
                {
                    msgid = MSG_CTRL_MOD_GET_PORT;
                    dest = APPMAIN_MSG;
                    break;
                }
                return TASK_UNKNOW_MSG;
            }

            default: return TASK_UNKNOW_MSG;
        }
    }
    else
    {
        return TASK_UNKNOW_MSG;
    }

    ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label & 0x00ff, pMsgInfo->playload, pMsgInfo->msglen, dest);
    if(0 == ret)
    {
        YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    else
    {
        YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_recv_common_msg
* @brief     ：收到一般消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int task_recv_common_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    uint32 dest = 0;

    if(MESSAGE_IID_COMMON == pMsgInfo->IID)
    {
        switch(pMsgInfo->IOP)
        {
            case COMMON_IOP_URGENT_POWERDOWN:
            {
                msgid = MSG_PULSE_POWER_DOWN;
                dest = PULSE_MSG;
                break;
            }
            case COMMON_IOP_SYSTEM_POWERDOWN:
            {
                msgid = MSG_PULSE_POWER_DOWN;
                dest = PULSE_MSG;
                break;
            }
            default: return TASK_UNKNOW_MSG;
        }
    }
    else
    {
        return TASK_UNKNOW_MSG;
    }

    ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, dest);
    if(0 == ret)
    {
        YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    else
    {
        YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_recv_rsp_msg
* @brief     ：收到遥信脉冲消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
int task_recv_rsp_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    uint32 dest = 0;
    uint8  label = 0;

    if(MESSAGE_IID_RSPSAMPLE == pMsgInfo->IID)
    {
        dest = PULSE_MSG;
        label = (uint8)pMsgInfo->label;
        switch(pMsgInfo->IOP)
        {
            case RSPSAMPLE_IOP_RMT_EVENT:
            {
                msgid = MSG_PULSE_GET_RMT_ENENT;
                ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->index, pMsgInfo->playload, pMsgInfo->msglen, dest);
                break;
            }
            case RSPSAMPLE_IOP_RMT_STATE:
            {
                msgid = MSG_PULSE_GET_RMT;
                ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, dest);
                break;
            }
            case RSPSAMPLE_IOP_PULSE_STATIC:
            {
                msgid = MSG_PULSE_GET_PULSE;
                dest = PULSE_SYNCMSG;
                ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, dest, label);
                break;
            }
            case RSPSAMPLE_IOP_CONFIGURE:
            {
                msgid = MSG_PULSE_SET_RSP;
                dest = PULSE_SYNCMSG;
                ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, dest, label);
                break;
            }
            default: return TASK_UNKNOW_MSG;
        }
    }
    else
    {
        return TASK_UNKNOW_MSG;
    }
    
    if(0 == ret)
    {
        YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    else
    {
        YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_recv_bt_msg
* @brief     ：收到蓝牙消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
int task_recv_bt_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    uint32 dest = 0;
    uint8  label = pMsgInfo->label;
    uint8  bSync = FALSE;
    
    if(MESSAGE_IID_BLE != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }

    dest = COMM_MSG;
    
    switch(pMsgInfo->IOP)
    {
        case BLE_IOP_RECV_EVENT: msgid = MSG_COMM_BTRECVMSG;   break;
        case BLE_IOP_PORT_EVENT: msgid = MSG_COMM_BTPORTEVENT; break;
        case BLE_IOP_SEND_DATA:
        {
            msgid = MSG_COMM_BTSENDMSG;
            dest = COMM_SYNCMSG;
            bSync = TRUE;
            label = (label & 0xff);
            break;
        }
        case BLE_IOP_SET_CONFIG: msgid = MSG_COMM_SETBTCFG;   break;
        case BLE_IOP_GET_CONFIG:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_GET_BTCFG;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_GETBTCFG;
            }
            break;
        }
        case BLE_IOP_SET_PORT:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_SET_BTCFG;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_SETBTPORT;
            }
            break;
        }
        case BLE_IOP_LINK_INFO:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_GET_BTCONNECT;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_GETBTPORT;
            }
            break;
        }
        case BLE_IOP_BT_SN:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_GET_BT_SN;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_GETBTPORT;
            }
            break;
        }
        case BLE_IOP_SET_BT_SN:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_SET_BT_SN;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_GETBTPORT;
            }
            break;
        }
        case BLE_IOP_CHECK_METER:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_SET_BTCHECK;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                return TASK_UNKNOW_MSG;
            }
            break;
        }
        default: return TASK_UNKNOW_MSG;
    }

    if(FALSE == bSync)
    {
        /* 异步消息 */
        ret = task_send_labelmsg(g_msgMonitor, msgid, label, pMsgInfo->playload, pMsgInfo->msglen, dest);
    }
    else
    {
        /* 同步消息 */
        ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, dest, (uint8)label);
    }
    if(0 == ret)
    {
        YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    else
    {
        YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_recv_radio_msg
* @brief     ：收到电台消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-1
* @Update    :
**********************************************************************/
int task_recv_radio_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    uint32 dest = 0;
    uint8  label = pMsgInfo->label;
    uint8  bSync = FALSE;
    
    if(MESSAGE_IID_RADIO != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }

    dest = COMM_MSG;
    
    switch(pMsgInfo->IOP)
    {
        case RADIO_IOP_RECV_EVENT: msgid = MSG_COMM_RADIORECVMSG;   break;
        case RADIO_IOP_SEND_DATA:
        {
            msgid = MSG_COMM_RADIOSENDMSG;
            dest = COMM_SYNCMSG;
            bSync = TRUE;
            label = (label & 0xff);
            break;
        }
        case RADIO_IOP_SET_ADDR: msgid = MSG_COMM_SETRADIOCFG;   break;
        case RADIO_IOP_GET_CONFIG:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_GET_RADIOCFG;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_GETRADIOCFG;
            }
            break;
        }
        case RADIO_IOP_GET_VERSION:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_GET_RADIOVER;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
//            else
//            {
//                msgid = MSG_COMM_GETRADIOCFG;
//            }
            break;
        }
        case RADIO_IOP_SET_CONFIG:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_SET_RADIOCFG;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_GETRADIOCFG;
            }
            break;
        }
        case RADIO_IOP_GET_STATUS:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_SET_RADIOSTATUS;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_RADIOSTATUS;
            }
            break;
        }
        case RADIO_IOP_SET_YANG:
        {
            if(MSG_APPMAIN_FLAG == (pMsgInfo->label & 0xff00))
            {
                msgid = MSG_APPMAIN_SET_RADIOSPEAK;
                dest = APPMAIN_SYNCMSG;
                bSync = TRUE;
                label = (label & 0xff);
            }
            else
            {
                msgid = MSG_COMM_RADIOSTATUS;
            }
            break;
        }
        default: return TASK_UNKNOW_MSG;
    }

    if(FALSE == bSync)
    {
        /* 异步消息 */
        ret = task_send_labelmsg(g_msgMonitor, msgid, label, pMsgInfo->playload, pMsgInfo->msglen, dest);
    }
    else
    {
        /* 同步消息 */
        ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, dest, (uint8)label);
    }
    if(0 == ret)
    {
        YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    else
    {
        YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_recv_uart_msg
* @brief     ：收到串口消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-11
* @Update    :
**********************************************************************/
int task_recv_uart_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    uint32 dest = 0;
    uint8  label = pMsgInfo->label;
    uint8  bSync = FALSE;
    
    if(MESSAGE_IID_UART != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }

    dest = COMM_MSG;
    
    switch(pMsgInfo->IOP)
    {
        case UART_IOP_NOTIFY_MODE: msgid = MSG_COMM_UARTEVENT;   break;
        case UART_IOP_NOTIFY_RECV: msgid = MSG_COMM_RECVUART;    break;
        case UART_IOP_GET_INFO:    msgid = MSG_COMM_GETUART;     break;
        case UART_IOP_SET_PARAM:   msgid = MSG_COMM_SETUART;     break;
        case UART_IOP_SET_LISTEN:  msgid = MSG_COMM_REGAPP;      break;
        case UART_IOP_SEND_DATA: 
        {
            msgid = MSG_COMM_SENDUART;
            dest = COMM_SYNCMSG;
            bSync = TRUE;
            label = (label & 0xff);
            break;
        }
        case UART_IOP_ONLYMODE_SET: 
        {
            msgid = MSG_COMM_UART_MODESET;
            dest = COMM_SYNCMSG;
            bSync = TRUE;
            label = (label & 0xff);
            break;
        }
        default: return TASK_UNKNOW_MSG;
    }

    if(FALSE == bSync)
    {
        /* 异步消息 */
        ret = task_send_labelmsg(g_msgMonitor, msgid, label, (uint8*)pMsgInfo, sizeof(MESSAGE_INFO_T) + pMsgInfo->msglen, dest);
    }
    else
    {
        /* 同步消息 */
        ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, dest, (uint8)label);
    }
    if(0 == ret)
    {
        YCMSG_FMT_TRACE("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    else
    {
        YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, label %d, ret %d\n", dest, msgid, pMsgInfo->label, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : task_recv_iot_msg
* @brief     ：收到物联管理消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 梅安宁
* @Date      ：2022-7-7
* @Update    :
**********************************************************************/
int task_recv_iot_msg(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    uint32 dest = 0;
//    uint8  label = pMsgInfo->label;
//    uint8  bSync = FALSE;
    
    if(MESSAGE_IID_IOT != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }

    dest = COMM_MSG;
    
    switch(pMsgInfo->IOP)
    {
        case IOT_IOP_DOCKER_ACTION: 
        case IOT_IOP_DOCKER_WARRING:
        case IOT_IOP_APP_ACTION:
        case IOT_IOP_APP_WARRING:
        case IOT_IOP_SYS_WARRING:
        case IOT_IOP_SYS_MOVE_UP: 
        {
        	dest = APPMAIN_MSG;
            msgid = MSG_APPAMIN_IOT_EVT;
            return task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->IOP, pMsgInfo->playload, pMsgInfo->msglen, dest);
        }
		case IOT_IOP_DOCKER_INSTALL: 
        {
        	msgid = MSG_APPAMIN_INSTLL_CON;
        	break;
        }
		case IOT_IOP_DOCKER_ACTION1: 
        {
        	msgid = MSG_APPAMIN_EXEC_CON;
        	break;
        }
		case IOT_IOP_DOCKER_SET: 
        {
        	msgid = MSG_APPAMIN_CONFIG_CON;
        	break;
        }
		case IOT_IOP_DOCKER_FIND: 
        {
        	msgid = MSG_APPAMIN_GET_CONTAINERINFO;
        	break;
        }
		case IOT_IOP_APP_INSTALL:
        {
			msgid = MSG_APPMAIN_INSTALL_APP;
        	break;
        }
        case IOT_IOP_APP_ACTION1:
        {
			msgid = MSG_APPAMIN_EXEC_APP;
        	break;
        }
        case IOT_IOP_APP_SET:
        {
			msgid = MSG_APPAMIN_CONFIG_APP;
        	break;
        }
        case IOT_IOP_APP_FIND:
        {
			msgid = MSG_APPAMIN_GET_APPINFO;
        	break;
        }
        case IOT_IOP_APP_LICENSE_SET:
        {
			msgid = MSG_APPAMIN_SETLISIENSE_APP;
        	break;
        }
        case IOT_IOP_APP_LICENSE_FIND:
        {
			msgid = MSG_APPAMIN_GET_LISIENSE;
        	break;
        }
        case IOT_IOP_SYS_MOVE_UP1:
        {
			msgid = MSG_APPAMIN_UPDATE_SYS;
        	break;
        }
//        case IOT_IOP_SYS_VERSION_FIND:
//        {
//			msgid = MSG_APPAMIN_GET_CONTAINERINFO;
//        	break;
//        }
        case IOT_IOP_LOG_INFO_FIND:
        {
			msgid = MSG_APPAMIN_LOGINFO_GET;
        	break;
        }
        case IOT_IOP_LOG_PACK:
        {
			msgid = MSG_APPAMIN_LOG_PACKAGE;
        	break;
        }
        case IOT_IOP_LOG_DELECT:
        {
			msgid = MSG_APPAMIN_FILE_DELECT;
        	break;
        }
        case IOT_IOP_SYS_INSPECT:
        {
			msgid = MSG_APPAMIN_SYS_INPECT;
        	break;
        }
        default: return TASK_UNKNOW_MSG;
    }


	/* 返回同步消息 */
    ret = task_ack_syncmsg(g_msgMonitor, msgid, pMsgInfo->playload, pMsgInfo->msglen, 
                           APPMAIN_SYNCMSG, (uint8)pMsgInfo->label);
    YCMSG_FMT_DEBUG("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", APPMAIN_SYNCMSG, msgid, pMsgInfo->label, ret);
    return ret;
}


/**********************************************************************
* @name      : task_recv_outmsg
* @brief     ：收到外部消息处理
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int task_recv_outmsg(MESSAGE_INFO_T *pMsgInfo)
{
    if(MESSAGE_IID_BLE == pMsgInfo->IID)
    {
        return task_recv_bt_msg(pMsgInfo);
    }
    
    if(MESSAGE_IID_RADIO == pMsgInfo->IID)
    {
        return task_recv_radio_msg(pMsgInfo);
    }

    if(MESSAGE_IID_UART == pMsgInfo->IID)
    {
        return task_recv_uart_msg(pMsgInfo);
    }

    switch(pMsgInfo->IID)
    {
        case MESSAGE_IID_WIRELESS: return task_comm_recv_msg(pMsgInfo);
        case MESSAGE_IID_MODULE:   return task_recv_mapmanager_msg(pMsgInfo);
        case MESSAGE_IID_COMMON:   
        {
            if(COMMON_IOP_URGENT_POWERDOWN == pMsgInfo->IOP || COMMON_IOP_URGENT_POWERDOWN == pMsgInfo->IOP)
            {
                return task_recv_common_msg(pMsgInfo);
            }
            
            if(COMMON_IOP_SYSTEM_RESTART == pMsgInfo->IOP)
            {
                 return task_appmain_recv_msg(pMsgInfo);
            }
            if(COMMON_IOP_TRANSPORT == pMsgInfo->IOP)
            {
                if(1 == pMsgInfo->rpm)
                {
                    return task_comm_recv_msg(pMsgInfo);
                }
            }
            //如果收到0014的消息 不处理 以防和smios对发
            if(COMMON_IOP_ERRMSG == pMsgInfo->IOP)
            {
                return 0;
            }
            break;
        }
        case MESSAGE_IID_SYSTEM:
        {
            if(SYSTEM_IOP_EVENT_ETHSTATUS == pMsgInfo->IOP || SYSTEM_IOP_GET_ETHSTATUS == pMsgInfo->IOP || 
               SYSTEM_IOP_ADD_ROUTE == pMsgInfo->IOP || SYSTEM_IOP_DEL_ROUTE == pMsgInfo->IOP ||
               SYSTEM_IOP_EVENT_IPCHANGE == pMsgInfo->IOP)
            {
                return task_comm_recv_msg(pMsgInfo);
            }
            if(SYSTEM_IOP_RCT == pMsgInfo->IOP || SYSTEM_IOP_REQUIRE_RESTART == pMsgInfo->IOP || 
                SYSTEM_IOP_SET_IPADDR == (pMsgInfo->IOP & 0x00FF) ||SYSTEM_IOP_GET_CONTAINER == pMsgInfo->IOP ||
                SYSTEM_IOP_GET_CONTAINER_APP == pMsgInfo->IOP ||SYSTEM_IOP_SYSTEM_CMD == pMsgInfo->IOP ||
                SYSTEM_IOP_GET_DEVINFO == pMsgInfo->IOP || SYSTEM_IOP_GET_CUR_DEVINFO == pMsgInfo->IOP || SYSTEM_IOP_APP_MANAGER == pMsgInfo->IOP)
            {
                return task_appmain_recv_msg(pMsgInfo);
            }
            break;
        }
        case MESSAGE_IID_PROXY:
        {
            if(PROXY_IOP_PROTOCOL_DATA_SEND == pMsgInfo->IOP || PROXY_IOP_PHYSIC_CONNECT == pMsgInfo->IOP)
            {
                return task_comm_recv_msg(pMsgInfo);
            }
            if(PROXY_IOP_APPLY_CONNECT == pMsgInfo->IOP)
            {
                return task_appmain_recv_msg(pMsgInfo);
            }
            break;
        }
        case MESSAGE_IID_ACMETER:
        {
            if(ACMETER_IOP_AC_PARM_INIT == pMsgInfo->IOP || ACMETER_IOP_AC_DATA_INIT == pMsgInfo->IOP || 
                ACMETER_IOP_AC_DEMAND_INIT == pMsgInfo->IOP || ACMETER_IOP_AC_SYSTEM_CMD == pMsgInfo->IOP||
                ACMETER_IOP_AC_RATE_SET == pMsgInfo->IOP)
            {
                return task_appmain_recv_msg(pMsgInfo);
            }
            break;
        }
        case MESSAGE_IID_CTS:      return task_appmain_recv_msg(pMsgInfo);
        case MESSAGE_IID_CCOROUTER:return task_appmain_recv_msg(pMsgInfo);
        case MESSAGE_IID_SCS:      return task_appmain_recv_msg(pMsgInfo);
        case MESSAGE_IID_DESKGUI:
        {
#ifdef ADV_ST_AMR
            if(DESKGUI_IOP_PUSH_HINT == pMsgInfo->IOP)
            {
                return task_appmain_recv_msg(pMsgInfo);
            }
#endif
            break;
        }
        case MESSAGE_IID_RSPSAMPLE: return task_recv_rsp_msg(pMsgInfo);
    }
    return TASK_UNKNOW_MSG;
}

