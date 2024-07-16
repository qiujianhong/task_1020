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

#include "msg_base.h"
#include "task_proc.h"
#include "commfun.h"

int     g_msgMonitor = 0;
extern ETH_CONNECT_INFO g_ethInfo;

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
    g_msgMonitor = task_msg_init(GUI_MSG_MONITOR);
    return g_msgMonitor;
}

/**********************************************************************
* @name      : task_appmain_recv_cts
* @brief     ：收到采集任务调度消息后发送给GUI
* @param[in] ：MESSAGE_INFO_T **ppMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-2
* @Update    :
**********************************************************************/
int task_gui_recv_cts(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    
    if(MESSAGE_IID_CTS != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }
    
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

    ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, GUI_MSG);
    if(0 == ret)
    {
        GUI_FMT_TRACE("Send back to 0x%x success: msg 0x%x, label %d, ret %d\n", APPMAIN_MSG, msgid, pMsgInfo->label, ret);
    }
    else
    {
        GUI_FMT_TRACE("Send back to 0x%x error: msg 0x%x, label %d, ret %d\n", APPMAIN_MSG, msgid, pMsgInfo->label, ret);
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
    
    if(MESSAGE_IID_WIRELESS != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }

	switch(pMsgInfo->IOP)
	{
		case WIRELESS_IOP_DIAG_EVENT://网络信息变化事件
		{
			ret = task_send_labelmsg(g_msgMonitor, MSG_COMM_DIALEVENT, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, GUI_COMM_MSG);
			if(0 == ret)
		    {
        		GUI_FMT_TRACE("Send back to 0x%x success: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_COMM_DIALEVENT, pMsgInfo->label, ret);
		    }
    		else
		    {
		        GUI_FMT_TRACE("Send back to 0x%x error: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_COMM_DIALEVENT, pMsgInfo->label, ret);
		    }
			return ret;
		}
		case WIRELESS_IOP_GET_DIAGINFO://查询拨号状态
		{
			ret = task_ack_syncmsg(g_msgMonitor, MSG_GUI_COMM_GETDIAL, pMsgInfo->playload, pMsgInfo->msglen,
										GUI_SYNCMSG, (uint8)pMsgInfo->label);
			GUI_FMT_TRACE("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", GUI_MSG, MSG_GUI_COMM_GETDIAL, pMsgInfo->label, ret);
            return ret;
		}
			
		default: return TASK_UNKNOW_MSG;
	}
	return ret;
}

/**********************************************************************
* @name      : task_pulse_recv_mode
* @brief     ：收到模组管理器消息后发送给pulse
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
int task_pulse_recv_mode(MESSAGE_INFO_T *pMsgInfo)
{
    int    ret = 0;
    uint32 msgid = 0;
    
    if(MESSAGE_IID_MODULE != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }
    
    switch(pMsgInfo->IOP)
    {
        case MODULE_IOP_GET_INFO:   msgid = MSG_PULSE_MOD_GET_INFO;    break;
        case MODULE_IOP_GET_STATUS: msgid = MSG_PULSE_MOD_GET_STATUS;  break;
        case MODULE_IOP_FILE_TRANSFER :
        {
            ModuleAckProc(); /* 后续流程由模组升级菜单处理 */
            return TASK_UNKNOW_MSG;
        }
        default: return TASK_UNKNOW_MSG;
    }

    ret = task_send_labelmsg(g_msgMonitor, msgid, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, PULSE_MSG);
    if(0 == ret)
    {
        GUI_FMT_TRACE("Send back to 0x%x success: msg 0x%x, label %d, ret %d\n", PULSE_MSG, msgid, pMsgInfo->label, ret);
    }
    else
    {
        GUI_FMT_TRACE("Send back to 0x%x error: msg 0x%x, label %d, ret %d\n", PULSE_MSG, msgid, pMsgInfo->label, ret);
    }
    return ret;
}
/*
int task_gui_recv_advPdamr(MESSAGE_INFO_T *pMsgInfo)
{
	int ret = 0;

	if(MESSAGE_IID_APPGUI != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }

	switch(pMsgInfo->IOP)
	{
		case APPGUI_IOP_GET_CONNECT://当前连接信息
		{
			ret = task_ack_syncmsg(g_msgMonitor, MSG_GUI_GET_CONNECT, pMsgInfo->playload, pMsgInfo->msglen,
										GUI_MSG, (uint8)pMsgInfo->label);
			GUI_FMT_TRACE("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", GUI_MSG, MSG_GUI_GET_CONNECT, pMsgInfo->label, ret);
            return ret;
			}
		case APPGUI_IOP_CONNECT_EVENT://连接事件
		{
			GUI_BUF_TRACE(pMsgInfo->playload, pMsgInfo->msglen, "recv event:");
			uint8 *data = pMsgInfo->playload;

			GUI_FMT_TRACE("0 = %x, 1 = %x, 2 = %x, 3= %x, 4= %x\n", data[0], data[1], data[2], data[3], data[4]);
			g_ethInfo.num = data[0];
			g_ethInfo.lanstate[0].state = data[1];*/
/*
			ret = task_send_labelmsg(g_msgMonitor, MSG_GUI_CONNECT_EVENT, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, GUI_COMM_MSG);
			if(0 == ret)
		    {
        		GUI_FMT_TRACE("Send back to 0x%x success: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_GUI_CONNECT_EVENT, pMsgInfo->label, ret);
		    }
    		else
		    {
		        GUI_FMT_TRACE("Send back to 0x%x error: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_GUI_CONNECT_EVENT, pMsgInfo->label, ret);
		    }
			return ret;
		}
		default: return TASK_UNKNOW_MSG;
    }

	return ret;
}
*/

/**********************************************************************
* @name      : task_gui_recv_wireless
* @brief     ：收到无线拨号消息后发送给gui
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/
/*
int task_gui_recv_wireless(MESSAGE_INFO_T *pMsgInfo)
{
	int ret = 0;

	if(MESSAGE_IID_WIRELESSGUI != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }

	switch(pMsgInfo->IOP)
	{
		case WIRELESSGUI_IOP_AT_SWITCH://AT交互事件开关
		{
			ret = task_ack_syncmsg(g_msgMonitor, MSG_GUI_AT_SWITCH, pMsgInfo->playload, pMsgInfo->msglen,
										GUI_MSG, (uint8)pMsgInfo->label);
			GUI_FMT_TRACE("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", GUI_MSG, MSG_GUI_AT_SWITCH, pMsgInfo->label, ret);
            return ret;
			}
		case WIRELESSGUI_IOP_CHANGE_EVENT://网络信息变化事件
		{
			ret = task_send_labelmsg(g_msgMonitor, MSG_GUI_CHANGE_EVENT, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, GUI_COMM_MSG);
			if(0 == ret)
		    {
        		GUI_FMT_TRACE("Send back to 0x%x success: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_GUI_CHANGE_EVENT, pMsgInfo->label, ret);
		    }
    		else
		    {
		        GUI_FMT_TRACE("Send back to 0x%x error: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_GUI_CHANGE_EVENT, pMsgInfo->label, ret);
		    }
			return ret;
		}
		case WIRELESSGUI_IOP_DIAG_EVENT://拨号状态变换事件
		{
			ret = task_send_labelmsg(g_msgMonitor, MSG_GUI_DIAG_EVENT, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, GUI_COMM_MSG);
			if(0 == ret)
		    {
        		GUI_FMT_TRACE("Send back to 0x%x success: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_GUI_DIAG_EVENT, pMsgInfo->label, ret);
		    }
    		else
		    {
		        GUI_FMT_TRACE("Send back to 0x%x error : msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_GUI_DIAG_EVENT, pMsgInfo->label, ret);
		    }
			return ret;
		}
		case WIRELESSGUI_IOP_AT_EVENT://AT交互事件
		{
			ret = task_send_labelmsg(g_msgMonitor, MSG_GUI_AT_EVENT, pMsgInfo->label, pMsgInfo->playload, pMsgInfo->msglen, GUI_COMM_MSG);
			if(0 == ret)
		    {
        		GUI_FMT_TRACE("Send back to 0x%x success: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_GUI_AT_EVENT, pMsgInfo->label, ret);
		    }
    		else
		    {
		        GUI_FMT_TRACE("Send back to 0x%x error: msg 0x%x, label %d, ret %d\n", GUI_COMM_MSG, MSG_GUI_AT_EVENT, pMsgInfo->label, ret);
		    }
			return ret;
		}
		default: return TASK_UNKNOW_MSG;
    }

	return ret;
}

*/

/**********************************************************************
* @name      : task_pulse_recv_system
* @brief     ：收到系统管理器消息返回消息
* @param[in] ：MESSAGE_INFO_T *pMsgInfo  外部消息
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-1-7
* @Update    :
**********************************************************************/

int task_gui_recv_system(MESSAGE_INFO_T *pMsgInfo)
{
	int    ret = 0;

	if(MESSAGE_IID_SYSTEM != pMsgInfo->IID)
    {
        return TASK_UNKNOW_MSG;
    }

	switch(pMsgInfo->IOP)
    {
        case SYSTEM_IOP_SET_IPADDR://设置IP
		{
			/* 返回同步消息 */
            ret = task_ack_syncmsg(g_msgMonitor, MSG_GUI_SETIP, pMsgInfo->playload, pMsgInfo->msglen, 
                                       GUI_SYNCMSG, (uint8)pMsgInfo->label);
            GUI_FMT_TRACE("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", GUI_SYNCMSG, MSG_GUI_SETIP, pMsgInfo->label, ret);
            return ret;
        	}
		case SYSTEM_IOP_RCT://设置时钟
		{
			/* 返回同步消息 */
            ret = task_ack_syncmsg(g_msgMonitor, MSG_GUI_RTC, pMsgInfo->playload, pMsgInfo->msglen, 
                                       GUI_SYNCMSG, (uint8)pMsgInfo->label);
            GUI_FMT_TRACE("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", GUI_SYNCMSG, MSG_GUI_RTC, pMsgInfo->label, ret);
            return ret;
		}
		case SYSTEM_IOP_REQUIRE_RESTART://重启
        {
        	/* 返回同步消息 */
            ret = task_ack_syncmsg(g_msgMonitor, MSG_GUI_REBOOT, pMsgInfo->playload, pMsgInfo->msglen, 
                                       GUI_SYNCMSG, (uint8)pMsgInfo->label);
            GUI_FMT_TRACE("Send back to 0x%x: msg 0x%x, index %d, ret %d\n", GUI_SYNCMSG, MSG_GUI_REBOOT, pMsgInfo->label, ret);
            return ret;
            }
        default: return TASK_UNKNOW_MSG;
    }

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
    switch(pMsgInfo->IID)
    {
        case MESSAGE_IID_WIRELESS: return task_comm_recv_wire(pMsgInfo);
        case MESSAGE_IID_MODULE:   return task_pulse_recv_mode(pMsgInfo);
        case MESSAGE_IID_CTS:      return task_gui_recv_cts(pMsgInfo);
		case MESSAGE_IID_SYSTEM:
		{
			if(SYSTEM_IOP_RCT == pMsgInfo->IOP)
            {
                return task_gui_recv_system(pMsgInfo);
            }
			if(SYSTEM_IOP_SET_IPADDR == pMsgInfo->IOP)
			{
				return task_gui_recv_system(pMsgInfo);
			}
			if(SYSTEM_IOP_REQUIRE_RESTART == pMsgInfo->IOP)
			{
				return task_gui_recv_system(pMsgInfo);
			}
			break;
		}
//		case MESSAGE_IID_APPGUI:
//		{
//			return task_gui_recv_advPdamr(pMsgInfo);
//			break;
//			}
//		case MESSAGE_IID_WIRELESSGUI:
//		{
//			return task_gui_recv_wireless(pMsgInfo);
//			break;
//			}
        default: return TASK_UNKNOW_MSG;
    }

	return TASK_UNKNOW_MSG;
}

