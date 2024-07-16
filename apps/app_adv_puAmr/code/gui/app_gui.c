/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线拨号显示处理
* @date：    2020-5-2
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "oopType.h"
#include "message.h"
#include "appmsg_split.h"
#include "gui_c_prtl.h"
#include "framecomm.h"
#include "task_id.h"
#include "db_api.h"

#include "app_gui.h"

DB_CLIENT   g_gui_db_client;  	//和数据中心交互的句柄
OOP_SWITCHIN_STATE_T    switchState;    //遥信状态
OOP_SWITCHIN_FLAG_T     switchFlag;     //接入标志


APP_MODE_INFO g_APPModeInfoFunc = NULL;


extern void *advance_mqtt_handle();
extern uint16 appmain_get_map_info(uint32 no, uint8* recvbuf, uint16 buflen);
extern int advance_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);
extern int advance_send_guimsg(uint16 label, uint16 iop, uint8* playload, uint16 length);

/**********************************************************************
* @name      : advance_gui_infonum
* @brief     ：设置信息点号
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-7-9
* @Update    : 
**********************************************************************/
void advance_gui_infonum(uint16 infonum)
{
    uint8 buf[16] = {0};
    uint16 msglen = 0;

    buf[msglen++] = 1;
    buf[msglen++] = 4;
    appmsg_memcpy_r(buf + msglen, &infonum, sizeof(uint16));
    msglen += sizeof(uint16);
    advance_send_guimsg(0, DESKGUI_IOP_FRESH_TOOL, buf, msglen);
}

/**********************************************************************
* @name      : advance_gui_reg
* @brief     ：无线拨号APP显示注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-5-2
* @Update    : 
**********************************************************************/
int advance_gui_reg()
{
	APP_REGIST_INFO_T appRegInfo;		//向桌面gui发送的注册信息
    int           ret = 0;

    memset(&appRegInfo, 0, sizeof(APP_REGIST_INFO_T));
	memcpy(appRegInfo.appname, ADV_APP_NAME, strlen(ADV_APP_NAME));
    memcpy(appRegInfo.shortname, ADV_SHOW_NAME, strlen(ADV_SHOW_NAME));
	memcpy(appRegInfo.iconFileName, ADV_GUI_ICON, strlen(ADV_GUI_ICON));

    ret = appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_REG, &appRegInfo, sizeof(APP_REGIST_INFO_T));
    if(0 != ret)
    {
        GUI_FMT_DEBUG("Send appgui reg failed %d\n", ret);
    }
    return ret;
}

/**********************************************************************
* @name      : advance_gui_start
* @brief     ：低压集抄APP显示
* @param[in] ：void *arg
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-15
* @Update    : 
**********************************************************************/
void* advance_gui_start(void* pArg)
{
    pthread_t mdl_gui_id;

    g_gui_db_client  = db_client_create(DB_YC_AGENT, DB_CLIENT_PUAMR_GUI);
	if(g_gui_db_client <= 0)
	{
		GUI_FMT_DEBUG("gDbCenterClient创建失败, 返回! \n");
		return 0;		
	}
    
    initlize_desk(ADV_APP_NAME, advance_send_msg);

    /* 桌面gui获取 */
    while(FALSE == advance_mqtt_guiok())
    {
        sleep(1);
    }

    GUI_FMT_DEBUG("Start to reg Appgui\n");

    /* APP注册gui */
    while(getLoginguiID() == 0)
    {
        advance_gui_reg();
        sleep(2);
    }
    g_APPModeInfoFunc = appmain_get_map_info;

    /* 启动业务 */
    if(pthread_create(&mdl_gui_id, NULL, app_gui_entry, NULL) != 0)
    {
        GUI_FMT_DEBUG("creating the app_gui_entry failed!\n");
    }
    
    return 0;

}



