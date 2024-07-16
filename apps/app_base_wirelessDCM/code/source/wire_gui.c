/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线拨号显示处理
* @date：    2020-5-2
* @history： 
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
* @brief     ：无线拨号APP显示注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-5-2
* @Update    : 
**********************************************************************/
int wireless_gui_reg()
{
    const char*		appName = "wirelessDCM";
    const char*		shortName = "拨号";
	const char*		iconFile ="/usr/local/extapps/wirelessDCM/bin/cfg/wirelessDCM.bmp";
	APP_REGIST_INFO_T appRegInfo;		//向桌面gui发送的注册信息
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
* @brief     ：显示初始化
* @param[in] ：void *pPublic    共有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    :
**********************************************************************/
int wireless_gui_init(void *pPublic, void **ppPrivate)
{
    WIRELESS_DATA_T      *ptWireData = (WIRELESS_DATA_T*)pPublic;

    /* 显示注册 */
    AppPluginReg(g_plugin, g_nPluginCnt);
    initlize_desk("wirelessDCM", wireless_send_msg);

    while(FALSE == ptWireData->guiReg)
    {
        sleep(2);
        wireless_send_guimsg(0, MESSAGE_IID_COMMON, COMMON_IOP_VERSION, NULL, 0);
    }
    *ppPrivate = NULL;
    WIRELESS_FMT_DEBUG("开始注册显示\n");
    return 0;
}


/**********************************************************************
* @name      : wireless_gui_start
* @brief     ：显示注册启动
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    :
**********************************************************************/
int wireless_gui_start(void *pArg, void *pPublic, void *pPrivate)
{
    pthread_t mdl_gui_id;

    if (pthread_create(&mdl_gui_id, NULL, app_gui_entry, NULL) != 0)				/*mdl_gui 线程*/
    {
        WIRELESS_FMT_DEBUG("creating the app_gui_entry failed!\n");
    }

    /* APP注册gui */
    while(getLoginguiID() == 0)
    {
        wireless_gui_reg();
        sleep(2);
    }
    WIRELESS_FMT_DEBUG("显示注册成功\n");
    wireless_send_diagStatus(0, "无线拨号APP注册桌面显示", strlen("无线拨号APP注册桌面显示"), 5);
    
    while(1)
    {
        sleep(1);
    }
    return 0;
}

//#endif
