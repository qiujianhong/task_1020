/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控显示注册入口
* @date：    2020-7-7
* @history： 
*********************************************************************
*/
#if defined PRODUCT_CCU || defined PRODUCT_ECU

#include "pageInterface.h"

#include "baseCommon.h"
#include "appmsg_split.h"
#include "mqtt_client.h"
#include "gui_c_prtl.h"
#include "framecomm.h"
#include "task_id.h"
#include "db_api.h"

#include "lc_comm.h"
#include "lc_mqtt.h"

extern PLUGIN_INFO* g_plugin[];
extern int g_nPluginCnt;

DB_CLIENT g_lcDBClient = 0;

/**********************************************************************
* @name      : lc_get_dbClient
* @brief     ：获取数据中心句柄
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-7-8
* @Update    : 
**********************************************************************/
int lc_get_dbClient()
{
    return g_lcDBClient;
}

/**********************************************************************
* @name      : lc_gui_reg
* @brief     ：回路巡检APP显示注册
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-7-7
* @Update    : 
**********************************************************************/
int lc_gui_reg()
{
    const char*		appName = LC_APP_MANE;
    const char*		shortName = "回路巡检";
	const char*		iconFile = "/usr/local/extapps/lcMonitor/bin/cfg/lcMonitor.bmp";
	APP_REGIST_INFO_T appRegInfo;		//向桌面gui发送的注册信息
    int             ret = 0;

    memset(&appRegInfo, 0, sizeof(APP_REGIST_INFO_T));
	memcpy(appRegInfo.appname, appName, strlen(appName));
    memcpy(appRegInfo.shortname, shortName, strlen(shortName));
	memcpy(appRegInfo.iconFileName, iconFile, strlen(iconFile));
    ret = appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_REG, &appRegInfo, sizeof(APP_REGIST_INFO_T));

    if(0 != ret)
    {
        GUI_FMT_DEBUG("Send appgui reg failed %d\n", ret);
    }
    return ret;
}

/**********************************************************************
* @name      : lc_app_stop
* @brief     ：回路巡检重启
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-6-24
* @Update    :
**********************************************************************/
void lc_app_stop()
{
    uint8  playout[128] = {0};
    uint16 msglen = 0;
    uint32 timelen = 10;

    /* 先退出桌面 */
    appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_UNREG, NULL, 0);

    appmsg_memcpy_r(playout, &timelen, sizeof(uint32));
    msglen += sizeof(uint32);

    playout[msglen++] = strlen("回路巡检APP重启");
    memcpy(playout + msglen, "回路巡检APP重启", strlen("回路巡检APP重启"));
    msglen += strlen("回路巡检APP重启");

    lc_send_guimsg(0, DESKGUI_IOP_FRESH_STATE, playout, msglen);

    GUI_FMT_DEBUG("界面重启应用\n");
    usleep(100000);
    exit(0);
}

/**********************************************************************
* @name      : lc_gui_init
* @brief     ：显示初始化
* @param[in] ：void *pPublic    共有数据
* @param[out]：void **ppPrivate 私有数据
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_gui_init(void *pPublic, void **ppPrivate)
{
    /* 显示注册 */
    AppPluginReg(g_plugin, g_nPluginCnt);
    initlize_desk(LC_APP_MANE, lc_send_mqttmsg);

    while(FALSE == lc_appgui_flag())
    {
        sleep(1);
    }

    g_lcDBClient = db_client_create(DB_LC_AGENT, DB_LC_CLIENT_GUI);
    if(g_lcDBClient <= 0)
    {
        return -1;
    }

    return 0;
}

/**********************************************************************
* @name      : lc_gui_start
* @brief     ：显示注册启动
* @param[in] ：void *pArg     线程句柄
               void *pPublic  公有数据
               void *pPrivate 私有数据
* @param[out]：
* @return    ：延时时间 us
* @Create    : 王津剑
* @Date      ：2020-7-7
* @Update    :
**********************************************************************/
int lc_gui_start(void *pArg, void *pPublic, void *pPrivate)
{
    pthread_t mdl_gui_id;

    GUI_FMT_DEBUG("开始注册显示\n");
    /* APP注册gui */
    while(getLoginguiID() == 0)
    {
        lc_gui_reg();
        sleep(2);
    }
    GUI_FMT_DEBUG("显示注册成功\n");

    /* 其他业务 */
    if (pthread_create(&mdl_gui_id, NULL, app_gui_entry, NULL) != 0)				/*mdl_gui 线程*/
    {
        GUI_FMT_DEBUG("creating the app_gui_entry failed!\n");
    }

    while(1)
    {
        sleep(2);
    }
    return 0;
}

#endif

