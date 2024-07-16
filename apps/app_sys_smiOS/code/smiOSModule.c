/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： s_smiOSModule.c
           数据中心对消息接收模式模块化处理
* @date： 2019-11-9
*********************************************************************
*/

#include <stdio.h>
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
#include "pageInterface.h"
//#endif

#include "appManager.h"
#include "dockerManager.h"
#include "baseType.h"
#include "globalConfiguration.h"
#include "mqttService.h"
#include "msgService.h"
#include "smiOSModule.h"
#include "tcpService.h"
#include "udpService.h"
#include "xlmPlatform.h"
//#include "xlfcgi.h"
#include "systemInfoControl.h"
#include "framecomm.h"
#include "gui_c_prtl.h"
#include "message.h"
#include "msgHelper.h"



//#if defined PRODUCT_CCU || defined PRODUCT_ECU
extern PLUGIN_INFO* g_plugin[];
extern int g_nPluginCnt;
static pthread_t	smiOS_gui_id;
//#endif
static SERVICE_MODULE_T s_smiOSModule[MOD_MAX];

extern sint32 mqtt_msg_handel_cb(void *obj);

/**
*********************************************************************
* @brief： 初始子系统各回调接口
* @param[in]：
               none
* @param[out]：
               none
* @return： 0
*********************************************************************
*/
static void init_modules(void)
{
    //tcp module
    s_smiOSModule[MOD_TCP].name    = "tcp";
    s_smiOSModule[MOD_TCP].initFp  = tcp_init_system;
    s_smiOSModule[MOD_TCP].startFp = tcp_start_system;
    s_smiOSModule[MOD_TCP].stopFp  = tcp_stop_system;
    s_smiOSModule[MOD_TCP].enable  = 0;

    //udp module
    s_smiOSModule[MOD_UDP].name    = "udp";
    s_smiOSModule[MOD_UDP].initFp  = udp_init_system;
    s_smiOSModule[MOD_UDP].startFp = udp_start_system;
    s_smiOSModule[MOD_UDP].stopFp  = udp_stop_system;
    s_smiOSModule[MOD_UDP].enable  = 0;

    //ipc module
    s_smiOSModule[MOD_IPC].name    = "ipc";
    s_smiOSModule[MOD_IPC].initFp  = NULL;
    s_smiOSModule[MOD_IPC].startFp = NULL;
    s_smiOSModule[MOD_IPC].stopFp  = NULL;
    s_smiOSModule[MOD_IPC].enable  = 0;

    //mqtt module
    s_smiOSModule[MOD_MQTT].name    = "mqtt";
    s_smiOSModule[MOD_MQTT].initFp  = mqtt_init_system;
    s_smiOSModule[MOD_MQTT].startFp = mqtt_start_system;
    s_smiOSModule[MOD_MQTT].stopFp  = mqtt_stop_system;
    s_smiOSModule[MOD_MQTT].enable  = 1;

    //message module
    s_smiOSModule[MOD_MSG].name    = "msg";
    s_smiOSModule[MOD_MSG].initFp  = msg_init_system;
    s_smiOSModule[MOD_MSG].startFp = msg_start_system;
    s_smiOSModule[MOD_MSG].stopFp  = msg_stop_system;
    s_smiOSModule[MOD_MSG].enable  = 1;

    //docker manage module
    s_smiOSModule[MOD_DOCKER].name    = "docker";
    s_smiOSModule[MOD_DOCKER].initFp  = docker_manager_init_system;
    s_smiOSModule[MOD_DOCKER].startFp = docker_manager_start_system;
    s_smiOSModule[MOD_DOCKER].stopFp  = docker_manager_stop_system;
    s_smiOSModule[MOD_DOCKER].enable  = 0;

    //app manage module
    s_smiOSModule[MOD_APP].name    = "app";
    s_smiOSModule[MOD_APP].initFp  = app_manager_init_system;
    s_smiOSModule[MOD_APP].startFp = app_manager_start_system;
    s_smiOSModule[MOD_APP].stopFp  = app_manager_stop_system;
    s_smiOSModule[MOD_APP].enable  = 1;

    //platform module 
    s_smiOSModule[MOD_PLATFORM].name = "platform";
    s_smiOSModule[MOD_PLATFORM].initFp  = platform_init_system;
    s_smiOSModule[MOD_PLATFORM].startFp = platform_start_system;
    s_smiOSModule[MOD_PLATFORM].stopFp  = platform_stop_system;
    s_smiOSModule[MOD_PLATFORM].enable  = 0;

    //xlfcgi module
    //s_smiOSModule[MOD_XLFCGI].name      = "xlfcgi";
    //s_smiOSModule[MOD_XLFCGI].initFp    = fcgi_init_system;
    //s_smiOSModule[MOD_XLFCGI].startFp   = fcgi_start_system;
    //s_smiOSModule[MOD_XLFCGI].stopFp    = fcgi_stop_system;
    //s_smiOSModule[MOD_XLFCGI].enable    = 0;

    //system module
    s_smiOSModule[MOD_SYS].name    = "system";
    s_smiOSModule[MOD_SYS].initFp  = sys_control_init;
    s_smiOSModule[MOD_SYS].startFp = sys_control_start;
    s_smiOSModule[MOD_SYS].stopFp  = sys_control_stop;
    s_smiOSModule[MOD_SYS].enable  = 1;

    s_smiOSModule[MOD_APPGUI].name      ="smiOSgui";
//#if defined PRODUCT_CCU || defined PRODUCT_ECU
    //smiOS GUI module
	s_smiOSModule[MOD_APPGUI].initFp	=smiOS_gui_init;
	s_smiOSModule[MOD_APPGUI].startFp	=smiOS_gui_start;
	s_smiOSModule[MOD_APPGUI].stopFp	=NULL;
	s_smiOSModule[MOD_APPGUI].enable	=1;
//#else
//    s_smiOSModule[MOD_APPGUI].enable	=0;
//#endif

}

/**
*********************************************************************
* @brief： 初始化系统
* @param[in]：
               none
* @param[out]：
               none
* @return： 初始化成功 返回0 失败返回-1
*********************************************************************
*/
sint32 init_system(void)
{
    sint32 mod = 0;

    init_modules();
    //初始化各个模块
    for (mod = 0; mod < MOD_MAX; ++mod)
    {
        if (s_smiOSModule[mod].enable != 0 && s_smiOSModule[mod].initFp)
        {
            if ((*s_smiOSModule[mod].initFp)() != 0)
            {
                INIT_FMT_DEBUG("smiOS module:[%s] initialization failed\n",
                            s_smiOSModule[mod].name);
                return -1;
            }
        }
    }

    //设置tcp模块
    if (s_smiOSModule[MOD_TCP].enable)
    {
        set_tcp_bind_ip(g_configObj.serviceCfg.tcpServerIP);
        set_tcp_bind_port(g_configObj.serviceCfg.tcpServerPort);
    }

    //设置UDP模块
    if (s_smiOSModule[MOD_UDP].enable)
    {
        set_udp_bind_ip(g_configObj.serviceCfg.udpServerIP);
        set_udp_bind_port(g_configObj.serviceCfg.udpServerPort);
    }

    //设置MQTT模块
    if (s_smiOSModule[MOD_MQTT].enable)
    {
        mqtt_set_broker_ip(g_configObj.serviceCfg.mqttBrokerIP);
        mqtt_set_broker_port(g_configObj.serviceCfg.mqttBrokerPort);
        mqtt_set_username(g_configObj.serviceCfg.mqttUserName);
        mqtt_set_password(g_configObj.serviceCfg.mqttPassword);
        mqtt_set_sub_qos(g_configObj.serviceCfg.mqttSubQos);
        mqtt_set_userID((sint8 *)g_configObj.serviceCfg.mqttServiceName);
    }

    INIT_FMT_DEBUG("smiOS is initialized successfully\n");
    return 0;
}

/**
*********************************************************************
* @brief： 启动系统
* @param[in]：
               none
* @param[out]：
               none
* @return： 启动成功 返回0 失败返回-1
*********************************************************************
*/
sint32 start_system(void)
{
    sint32 mod = 0;

    //根据各个模块的enable值，启动各个模块
    for (mod = 0; mod < MOD_MAX; ++mod)
    {
        if (s_smiOSModule[mod].enable != 0 && s_smiOSModule[mod].startFp)
        {
            if ((*s_smiOSModule[mod].startFp)() != 0)
            {
                INIT_FMT_DEBUG("smiOS module:[%s] start failed\n",
                            s_smiOSModule[mod].name);
                return -1;
            }
        }
    }

    INIT_FMT_DEBUG("smiOS is started successfully\n");
    return 0;
}

/**
*********************************************************************
* @brief： 系统子模块资源释放 [保留]
* @param[in]：
               none
* @param[out]：
               none
* @return：
*********************************************************************
*/
void de_init_system(void)  // 预留接口用于系统关闭时关闭各子模块
{
}

//#if defined PRODUCT_CCU || defined PRODUCT_ECU
/**
*********************************************************************
* @brief： smios GUI模块初始化
* @param[in]：
               none
* @param[out]：
               none
* @return： 初始化成功 返回0 失败返回-1
*********************************************************************
*/
sint32 smiOS_gui_init(void)
{
	int ret = 0;
	AppPluginReg(g_plugin, g_nPluginCnt);
	ret = initlize_desk_func("smiOS",pMsgtoRawObj);
	if(ret > 0)
		return 0;
	else
		return -1;
}

/**
*********************************************************************
* @brief： smios GUI模块启动
* @param[in]：
               none
* @param[out]：
               none
* @return： 初始化成功 返回0 失败返回-1
*********************************************************************
*/
sint32 smiOS_gui_start(void)
{
	if(pthread_create(&smiOS_gui_id, NULL, app_gui_entry, NULL) != 0)
	{
		INIT_FMT_DEBUG("create smiOS gui thread failed!\n");
		return -1;
	}
	return 0;
}
//#endif
/**
*********************************************************************
* @brief： 将消息转换成smiOS格式的结构体并发送
* @param[in]：
               source       消息源
               dest         消息目的地
               pMsgInfo     待发送消息
* @param[out]：
               none
* @return： 初始化成功 返回0 失败返回-1
*********************************************************************
*/
int pMsgtoRawObj(char* source, char* dest, MESSAGE_INFO_T* pMsgInfo )
{
	int ret = -1;
	
	if(source == NULL || dest == NULL || pMsgInfo == NULL)
		return -1;
	MSG_HANDER_OBJ_T* msgHanderObj = NULL;
	MQTT_CTX_T *ctx	= NULL;
	ctx = (MQTT_CTX_T*)calloc(1,sizeof(MQTT_CTX_T));
	if(ctx == NULL)
	{
		printf("%s: ctx calloc failed!\n",__FUNCTION__);
		return -1;
	}
	msgHanderObj = (MSG_HANDER_OBJ_T*)calloc(1, sizeof(MSG_HANDER_OBJ_T));
	if(ctx == NULL)
	{
		printf("%s: msgHanderObj calloc failed!\n",__FUNCTION__);
		return -1;
	}
	msgHanderObj->obj	= ctx; //将CTX与msgHanderObj绑定
	//ctx->rspMsg.length = pMsgInfo->msglen;
	//memcpy(ctx->rspMsg.payload,pMsgInfo->playload,pMsgInfo->msglen);
	ctx->rspMsg.prm = pMsgInfo->rpm;
	ctx->rspMsg.index = pMsgInfo->index;
	ctx->rspMsg.priority = pMsgInfo->priority;
	ctx->rspMsg.label = pMsgInfo->label;
	ctx->rspMsg.tag = (pMsgInfo->IID << 16) | (pMsgInfo->IOP);
	printf("tag = %08x,IID = %04x,IOP = %04x\n",ctx->rspMsg.tag,pMsgInfo->IID,pMsgInfo->IOP);
	ctx->rspMsg.source = source;
	ctx->rspMsg.destination = dest;
	build_msg(&ctx->rspMsg, pMsgInfo->playload, pMsgInfo->msglen, &ctx->rspRawObj);
	ret = mqtt_msg_handel_cb(msgHanderObj);
	return ret;
}

