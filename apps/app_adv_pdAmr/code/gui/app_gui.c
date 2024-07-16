/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���߲�����ʾ����
* @date��    2020-5-2
* @history�� 
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

DB_CLIENT   g_gui_db_client;  	//���������Ľ����ľ��

extern BOOLEAN advPd_mqtt_guiok();
extern void *advPd_mqtt_handle();
extern int advPd_send_mqttmsg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);

/**********************************************************************
* @name      : advPd_gui_reg
* @brief     �����߲���APP��ʾע��
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2020-5-15
* @Update    : 
**********************************************************************/
int advPd_gui_reg()
{
	APP_REGIST_INFO_T appRegInfo;		//������gui���͵�ע����Ϣ
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
* @name      : advPd_gui_start
* @brief     �����߲���APP��ʾ
* @param[in] ��void *arg
* @param[out]��
* @return    ��
* @Create    : ����
* @Date      ��2020-5-15
* @Update    : 
**********************************************************************/
void* advPd_gui_start(void* pArg)
{
    pthread_t mdl_gui_id;

    g_gui_db_client  = db_client_create(DB_PD_AGENT, DB_PD_CLIENT_GUI);
	if(g_gui_db_client <= 0)
	{
		GUI_FMT_DEBUG("gDbCenterClient����ʧ��, ����! \n");
		return 0;		
	}
    
    initlize_desk(ADV_APP_NAME, advPd_send_mqttmsg);

    /* ����gui��ȡ */
    while(FALSE == advPd_mqtt_guiok())
    {
        sleep(1);
    }

    GUI_FMT_DEBUG("Start to reg Appgui\n");

    /* APPע��gui */
    while(getLoginguiID() == 0)
    {
        advPd_gui_reg();
        sleep(2);
    }

    /* ����ҵ�� */
    if(pthread_create(&mdl_gui_id, NULL, app_gui_entry, NULL) != 0)
    {
        GUI_FMT_DEBUG("creating the app_gui_entry failed!\n");
    }
    
    return 0;
}



