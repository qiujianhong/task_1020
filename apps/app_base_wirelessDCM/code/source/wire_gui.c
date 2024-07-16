/*
*********************************************************************
* Copyright(C) 2019 �Ͼ��������ӹɷ����޹�˾
* All rights reserved. 
* @brief��   ���߲�����ʾ����
* @date��    2020-5-2
* @history�� 
*********************************************************************
*/

//#if defined PRODUCT_CCU || defined PRODUCT_ECU || defined PRODUCT_ECU_G
#include "pageInterface.h"

#include "baseCommon.h"
#include "appmsg_split.h"
#include "gui_c_prtl.h"
#include "wireless.h"
#include "wire_mqtt.h"
#include "wire_gui.h"

extern PLUGIN_INFO* g_plugin[];
extern int g_nPluginCnt;
extern int wireless_send_msg(char *pTopic, MESSAGE_INFO_T *pMsgInfo, int qos);

/**********************************************************************
* @name      : wireless_gui_reg
* @brief     �����߲���APP��ʾע��
* @param[in] ��
* @param[out]��
* @return    ��0-�ɹ�/������
* @Create    : 
* @Date      ��2020-5-2
* @Update    : 
**********************************************************************/
int wireless_gui_reg()
{
    const char*		appName = "wirelessDCM";
    const char*		shortName = "����";
	const char*		iconFile ="/usr/local/extapps/wirelessDCM/bin/cfg/wirelessDCM.bmp";
	APP_REGIST_INFO_T appRegInfo;		//������gui���͵�ע����Ϣ
    int           ret = 0;

    memset(&appRegInfo, 0, sizeof(APP_REGIST_INFO_T));
	memcpy(appRegInfo.appname, appName, strlen(appName));
    memcpy(appRegInfo.shortname, shortName, strlen(shortName));
	memcpy(appRegInfo.iconFileName, iconFile, strlen(iconFile));
    ret = appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_REG, &appRegInfo, sizeof(APP_REGIST_INFO_T));

    if(0 != ret)
    {
        MQTTMSG_FMT_DEBUG("Send appgui reg failed %d\n", ret);
    }
    return ret;
}

/**********************************************************************
* @name      : wireless_gui_init
* @brief     ����ʾ��ʼ��
* @param[in] ��void *pPublic    ��������
* @param[out]��void **ppPrivate ˽������
* @return    ��0-�ɹ�/������
* @Create    : ����
* @Date      ��2021-3-22
* @Update    :
**********************************************************************/
int wireless_gui_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;

    /* ��ʾע�� */
    AppPluginReg(g_plugin, g_nPluginCnt);
    initlize_desk("wirelessDCM", wireless_send_msg);

    while(FALSE == ptWireData->guiReg)
    {
        sleep(2);
        wireless_send_guimsg(0, MESSAGE_IID_COMMON, COMMON_IOP_VERSION, NULL, 0);
    }
    *ppPrivate = NULL;
    WIRELESS_FMT_DEBUG("��ʼע����ʾ\n");
    return 0;
}


/**********************************************************************
* @name      : wireless_gui_start
* @brief     ����ʾע������
* @param[in] ��void *pArg     �߳̾��
               void *pPublic  ��������
               void *pPrivate ˽������
* @param[out]��
* @return    ����ʱʱ�� us
* @Create    : ����
* @Date      ��2021-3-22
* @Update    :
**********************************************************************/
int wireless_gui_start(void *pArg, void *pPublic, void *pPrivate)
{
    pthread_t mdl_gui_id;

    if (pthread_create(&mdl_gui_id, NULL, app_gui_entry, NULL) != 0)				/*mdl_gui �߳�*/
    {
        WIRELESS_FMT_DEBUG("creating the app_gui_entry failed!\n");
    }

    /* APPע��gui */
    while(getLoginguiID() == 0)
    {
        wireless_gui_reg();
        sleep(2);
    }
    WIRELESS_FMT_DEBUG("��ʾע��ɹ�\n");
    wireless_send_diagStatus(0, "���߲���APPע��������ʾ", strlen("���߲���APPע��������ʾ"), 5);
    
    while(1)
    {
        sleep(1);
    }
    return 0;
}

//#endif
