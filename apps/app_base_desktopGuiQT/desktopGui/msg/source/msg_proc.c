/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   高级APP对外通信模块
* @date：    2019-12-24
* @history： 
*********************************************************************
*/

#include "oopType.h"
#include "db_api.h"
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "appmsg_split.h"



#include "msg_base.h"
#include "task_proc.h"
#include "out_msg.h"
#include "label_id.h"
#include "msg_proc.h"

STATUS_BAR_INFO_T 	statusInfoFresh;
POP_MES_BOX_T 	    g_pop_mes_Fresh;
TOOL_BAR_INFO_T		toolInfoFresh;


/**********************************************************************
* @name      : sendback_msg_build
* @brief     ：回复响应消息构建
* @param[in] ：MESSAGE_INFO_T *pInMsg 	接收消息体
			   void * pLoadBuf  		待发送的消息内容
			   uint16 Loadlen			待发送的消息长度
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-5-24
* @Update    :
**********************************************************************/
int sendback_msg_build(MESSAGE_INFO_T *pInMsg, void * pLoadBuf, uint16 Loadlen)
{
	int len = 0;
	int ret = 0;
	MESSAGE_INFO_T * pMsgInfo = NULL;
	char topic[256] = {0};
	
	len = sizeof(MESSAGE_INFO_T) + Loadlen;
    pMsgInfo = (MESSAGE_INFO_T*)malloc(len);
    if(NULL == pMsgInfo)
    {
        GUI_FMT_DEBUG("%s malloc failed %d\n",__FUNCTION__,len);
        return -1;
    }
    memset(pMsgInfo, 0, len);
    
    pMsgInfo->rpm = 0;
    pMsgInfo->index = pInMsg->index;
    pMsgInfo->priority = pInMsg->priority;
    pMsgInfo->label = pInMsg->label;
    pMsgInfo->IID = pInMsg->IID;
    pMsgInfo->IOP = pInMsg->IOP;
    
    pMsgInfo->sourlen = strlen((char *)GUI_MSG_NAME);
    memcpy(pMsgInfo->souraddr , GUI_MSG_NAME, pMsgInfo->sourlen);
    pMsgInfo->destlen = pInMsg->sourlen;
	memcpy(pMsgInfo->destaddr,pInMsg->souraddr,pMsgInfo->destlen);
    pMsgInfo->msglen = Loadlen;
    memcpy(pMsgInfo->playload, (char*)pLoadBuf, Loadlen);
    sprintf(topic,"%s/%s",GUI_MSG_NAME,pMsgInfo->destaddr);
	ret = gui_send_msg(topic, pMsgInfo, 0);
    free(pMsgInfo);
	return ret;
}


/**********************************************************************
* @name      : desktop_cancel_pop
* @brief     ：取消信息提示框
* @param[in] ：MESSAGE_INFO_T *pInMsg 消息体
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-5-24
* @Update    :
**********************************************************************/
int desktop_cancel_pop(MESSAGE_INFO_T *pInMsg)
{
	int ret = 0;
	
	BOOLEAN result = 0;
	if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_POP_HINT)
	{
		return -1;
	}

	//取消信息提示框操作
	memset(&g_pop_mes_Fresh,0,sizeof(POP_MES_BOX_T));
//    ClosePopLayer();
	ret = sendback_msg_build(pInMsg,&result,1);
    return ret;
}


/**********************************************************************
* @name      : desktop_pop_msg
* @brief     ：弹出信息提示框
* @param[in] ：MESSAGE_INFO_T *pInMsg 消息体
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-5-24
* @Update    :
**********************************************************************/
int desktop_pop_msg(MESSAGE_INFO_T *pInMsg)
{
	int ret = 0;
	int offset = 0;
	uint8 msglen = 0;
	BOOLEAN result = 0;
	uint32 disptime = 0;		//显示时长
	char dispText[512] = {0};	//显示内容
	uint16 textLen = 0;	
	if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_PUSH_HINT)
	{
		return -1;
	}
	appmsg_memcpy_r((uint8*)&disptime,&pInMsg->playload[offset],sizeof(uint32));
	offset += sizeof(uint32);
    textLen = appmsg_get_len_offset(&pInMsg->playload[offset], &msglen);
    offset += msglen;
    if(textLen > 511)
    {
		textLen = 511;
	}
	memcpy(dispText,&pInMsg->playload[offset],textLen);
	offset += textLen;

	GUI_FMT_TRACE("[0007] desktop_pop_msg :text :%s , display time :%d\n", dispText,disptime);
    if(g_pop_mes_Fresh.delayTime >=3 )          //按键3秒内不弹显
    {
		memset(&g_pop_mes_Fresh,0,sizeof(POP_MES_BOX_T));
        g_pop_mes_Fresh.delayTime = 3;
        
    	if(disptime > 0)
    	{
    		g_pop_mes_Fresh.dispEnd = time(NULL) + disptime;
    	}
    	strncpy(g_pop_mes_Fresh.text,dispText,textLen);
        g_pop_mes_Fresh.text[textLen] = '\0';


//        PopMsg(g_pop_mes_Fresh.text);
    	result = 1;
    	ret = sendback_msg_build(pInMsg,&result,1);
	}
    else
    {
        result = 0;
		ret = sendback_msg_build(pInMsg,&result,1);
	}
    
    return ret;
}


/**********************************************************************
* @name      : desktop_fresh_tool
* @brief     ：刷新工具栏
* @param[in] ：MESSAGE_INFO_T *pInMsg 消息体
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-5-24
* @Update    :
**********************************************************************/
int desktop_fresh_tool(MESSAGE_INFO_T *pInMsg)
{
	int ret = 0;
	int offset = 0;
	uint8 choice1 = -1;
	uint8 choice2 = -1;
	uint8 fieldCnt = 0;
	BOOLEAN result = 0;
	int i;
	TOOL_BAR_INFO_T *pToolInfo = &toolInfoFresh;
	if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_FRESH_TOOL)
	{
		return -1;
	}
	//刷新工具栏，返回结果值
	//memset(pToolInfo,0,sizeof(TOOL_BAR_INFO_T));
	fieldCnt = pInMsg->playload[offset++];
	for(i = 0; i < fieldCnt; i++)
	{
		choice1 = pInMsg->playload[offset++];
		if(choice1 == CHANNEL1 || choice1 == CHANNEL2)
		{
			choice2 = pInMsg->playload[offset++];
			if(choice1 == CHANNEL1)
			{
				switch (choice2)
				{
					case SIG_STRENGTH:
						appmsg_memcpy_r(&(pToolInfo->channel1.signalStrengh), &pInMsg->playload[offset], sizeof(pToolInfo->channel1.signalStrengh));
						offset += sizeof(pToolInfo->channel1.signalStrengh);
						printf("[工具栏]pToolInfo->channel1.signalStrengh = %d\n",pToolInfo->channel1.signalStrengh);
					break;
					
					case NET_FORMAT:
						appmsg_memcpy_r(&(pToolInfo->channel1.netFormat), &pInMsg->playload[offset], sizeof(pToolInfo->channel1.netFormat));
						offset += sizeof(pToolInfo->channel1.netFormat);
						printf("[工具栏]pToolInfo->channel1.netFormat = %d\n",pToolInfo->channel1.netFormat);
					break;

					case CON_STATUS:
						appmsg_memcpy_r(&(pToolInfo->channel1.conStatus), &pInMsg->playload[offset], sizeof(pToolInfo->channel1.conStatus));
						offset += sizeof(pToolInfo->channel1.conStatus) ;
						printf("[工具栏]pToolInfo->channel1.conStatus = %d\n",pToolInfo->channel1.conStatus);
					break;

					default:
						break;
				}
			}
			else	//choice1 == 1
			{
				switch (choice2)
				{
					case SIG_STRENGTH:
						appmsg_memcpy_r(&(pToolInfo->channel2.signalStrengh), &pInMsg->playload[offset], sizeof(pToolInfo->channel2.signalStrengh));
						offset += sizeof(pToolInfo->channel2.signalStrengh);
						printf("[工具栏]pToolInfo->channel2.signalStrengh = %d\n",pToolInfo->channel2.signalStrengh);
					break;
				
					case NET_FORMAT:
						appmsg_memcpy_r(&(pToolInfo->channel2.netFormat), &pInMsg->playload[offset], sizeof(pToolInfo->channel2.netFormat));
						offset += sizeof(pToolInfo->channel2.netFormat);
						printf("[工具栏]pToolInfo->channel2.netFormat = %d\n",pToolInfo->channel2.netFormat);
					break;
			
					case CON_STATUS:
						appmsg_memcpy_r(&(pToolInfo->channel2.conStatus), &pInMsg->playload[offset], sizeof(pToolInfo->channel2.conStatus));
						offset += sizeof(pToolInfo->channel2.conStatus) ;
						printf("[工具栏]pToolInfo->channel2.conStatus = %d\n",pToolInfo->channel2.conStatus);
					break;
			
					default:
						break;
				}

			}

		}
		else
		{
			switch(choice1)
			{
				case ETH_CON_STATUS:
					pToolInfo->ethConStatus = pInMsg->playload[offset++];
					printf("[工具栏]pToolInfo->ethConStatus = %d\n",pToolInfo->ethConStatus);
				break;

				case WARN:
					pToolInfo->warning = pInMsg->playload[offset++];
					printf("[工具栏]pToolInfo->warning = %d\n",pToolInfo->warning);
				break;

				case INFO_ID:
					appmsg_memcpy_r(&(pToolInfo->infoID), &pInMsg->playload[offset], sizeof(pToolInfo->infoID));
					offset += sizeof(pToolInfo->infoID);
					printf("[工具栏]pToolInfo->infoID = %d\n",pToolInfo->infoID);
				break;

				case KEY_STATUS:
					pToolInfo->keyStatus = pInMsg->playload[offset++];
					printf("[工具栏]pToolInfo->keyStatus = %d\n",pToolInfo->keyStatus);
//                	write_pdata_xram(GUI_PARA, (const char *)&pToolInfo->keyStatus, Esam_Status, Esam_Status_Len);
				break;
			}
		}
	}
	result = 1;
	ret = sendback_msg_build(pInMsg,&result,1);
    return ret;

}

/**********************************************************************
* @name      : desktop_fresh_state
* @brief     ：刷状态栏消息
* @param[in] ：MESSAGE_INFO_T *pInMsg 消息体
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-5-24
* @Update    :
**********************************************************************/
int desktop_fresh_state(MESSAGE_INFO_T *pInMsg)
{
	int ret = 0;
	int offset = 0;
	BOOLEAN result = 0;
	uint32 disptime = 0;		//显示时长
	char dispText[24] = {0};	//显示内容
	uint16 textLen = 0;	
	if(pInMsg->IID != MESSAGE_IID_DESKGUI || pInMsg->IOP != DESKGUI_IOP_FRESH_STATE)
	{
		return -1;
	}
	appmsg_memcpy_r((uint8*)&disptime,&pInMsg->playload[offset],sizeof(uint32));
	offset += sizeof(uint32);
	textLen = pInMsg->playload[offset++];
    
    if(textLen>24)
    {
		textLen = 24;
	}
        
	strncpy(dispText,(char*)&pInMsg->playload[offset],textLen);
	offset += textLen;
	GUI_FMT_TRACE("[0007] status bar info :text :%s , display time :%d\n", dispText,disptime);
	//状态栏的显示处理，返回result值
	memset(&statusInfoFresh,0,sizeof(STATUS_BAR_INFO_T));
	if(disptime > 0)
	{
		statusInfoFresh.dispEnd = time(NULL) + disptime;
	}
	strncpy(statusInfoFresh.text,dispText,sizeof(dispText));
	result = 1;
	ret = sendback_msg_build(pInMsg,&result,1);
    return ret;
}


/**********************************************************************
* @name      : desktop_msg_handle
* @brief     ：显示消息处理接口
* @param[in] ：MESSAGE_INFO_T *pInMsg 消息体
* @param[out]：
* @return    ：
* @Create    : 梅安宁
* @Date      ：2022-5-24
* @Update    :
**********************************************************************/
int desktop_msg_handle(MESSAGE_INFO_T *pInMsg)
{
	int ret = 0;
	if(pInMsg->IID != MESSAGE_IID_DESKGUI)
	{
		return -1;
	}

	switch(pInMsg->IOP)
	{

		case DESKGUI_IOP_FRESH_STATE:	//刷状态栏
			return desktop_fresh_state(pInMsg);
			break;

		case DESKGUI_IOP_FRESH_TOOL:	//刷工具栏
			return desktop_fresh_tool(pInMsg);
			break;

		case DESKGUI_IOP_PUSH_HINT:		//弹出信息提示框
			return desktop_pop_msg(pInMsg);
			break;

		case DESKGUI_IOP_POP_HINT:		//取消信息提示框
			return desktop_cancel_pop(pInMsg);
			break;

		default: 
			return 0x3333;
	}
	return ret;
}


