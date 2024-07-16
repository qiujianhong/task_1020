/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief： s_dbCenterModule.c
           数据中心对消息接收模式模块化处理  
* @date： 2019-11-9
*********************************************************************
*/

#include <stdio.h>

#include "baseType.h"
#include "dbCenterModule.h"
#include "dbModule.h"
#include "globalConfiguration.h"
#include "mqttService.h"
#include "msgService.h"
#include "tcpService.h"
#include "udpService.h"
#include "olog.h"

static SERVICE_MODULE_T s_dbCenterModule[MOD_MAX];
void *     db_msg_ocat ;

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
    s_dbCenterModule[MOD_TCP].name = "tcp";
    s_dbCenterModule[MOD_TCP].initFp  = tcp_init_system;
    s_dbCenterModule[MOD_TCP].startFp = tcp_start_system;
    s_dbCenterModule[MOD_TCP].stopFp  = tcp_stop_system;
    s_dbCenterModule[MOD_TCP].enable = 0;

    s_dbCenterModule[MOD_UDP].name = "udp";
    s_dbCenterModule[MOD_UDP].initFp = udp_init_system;
    s_dbCenterModule[MOD_UDP].startFp = udp_start_system;
    s_dbCenterModule[MOD_UDP].stopFp = udp_stop_system;
    s_dbCenterModule[MOD_UDP].enable = 0;

    s_dbCenterModule[MOD_IPC].name = "ipc";
    s_dbCenterModule[MOD_IPC].initFp = NULL;
    s_dbCenterModule[MOD_IPC].startFp = NULL;
    s_dbCenterModule[MOD_IPC].stopFp = NULL;
    s_dbCenterModule[MOD_IPC].enable = 0;

    s_dbCenterModule[MOD_MQTT].name = "mqtt";
    s_dbCenterModule[MOD_MQTT].initFp  = mqtt_init_system;
    s_dbCenterModule[MOD_MQTT].startFp = mqtt_start_system;
    s_dbCenterModule[MOD_MQTT].stopFp  = mqtt_stop_system;
    s_dbCenterModule[MOD_MQTT].enable = 1;

    s_dbCenterModule[MOD_MSG].name = "msg";
    s_dbCenterModule[MOD_MSG].initFp = msg_init_system;
    s_dbCenterModule[MOD_MSG].startFp = msg_start_system;
    s_dbCenterModule[MOD_MSG].stopFp = msg_stop_system;
    s_dbCenterModule[MOD_MSG].enable = 1;

    s_dbCenterModule[MOD_DB].name = "db";
    s_dbCenterModule[MOD_DB].initFp = db_init;
    s_dbCenterModule[MOD_DB].startFp = db_start;
    s_dbCenterModule[MOD_DB].stopFp = db_stop;
    s_dbCenterModule[MOD_DB].enable = 1;
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
    db_msg_ocat = olog_get_category("db_msg");

    init_modules();
    for (mod = 0; mod < MOD_MAX; ++mod)
    {
        if (s_dbCenterModule[mod].enable != 0 && s_dbCenterModule[mod].initFp)
        {
            if ((*s_dbCenterModule[mod].initFp)() != 0)
            {
                olog_debug(db_msg_ocat,"dbCenter module:[%s] initialization failed", s_dbCenterModule[mod].name);
                return -1;
            }
        }
    }

    if (s_dbCenterModule[MOD_TCP].enable)
    {
        set_tcp_bind_ip(g_configObj.serviceCfg.tcpServerIP);
        set_tcp_bind_port(g_configObj.serviceCfg.tcpServerPort);
    }

    if (s_dbCenterModule[MOD_UDP].enable)
    {
        set_udp_bind_ip(g_configObj.serviceCfg.udpServerIP);
        set_udp_bind_port(g_configObj.serviceCfg.udpServerPort);
    }

    if (s_dbCenterModule[MOD_MQTT].enable)
    {
        mqtt_set_broker_ip(g_configObj.serviceCfg.mqttBrokerIP);
        mqtt_set_broker_port(g_configObj.serviceCfg.mqttBrokerPort);
        mqtt_set_username(g_configObj.serviceCfg.mqttUserName);
        mqtt_set_password(g_configObj.serviceCfg.mqttPassword);
        mqtt_set_userID(g_configObj.serviceCfg.serverName);
        mqtt_set_sub_qos(0);
    }

    dolog_info("dbCenter is initialized successfully");
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

    for (mod = 0; mod < MOD_MAX; ++mod)
    {
        if (s_dbCenterModule[mod].enable != 0 && s_dbCenterModule[mod].startFp)
        {
            if ((*s_dbCenterModule[mod].startFp)() != 0)
            {
                olog_debug(db_msg_ocat,"dbCenter module:[%s] start failed", s_dbCenterModule[mod].name);
                return -1;
            }
        }
    }

    dolog_info("dbCenter is started successfully");
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
void de_init_system(void) // 预留接口用于系统关闭时关闭各子模块
{
    olog_info(db_msg_ocat,"dbCenter Reverse Init strat....");
    if(s_dbCenterModule[MOD_MSG].stopFp)
    {
        (*s_dbCenterModule[MOD_MSG].stopFp)();
    }
    
    if(s_dbCenterModule[MOD_DB].stopFp)
    {
        (*s_dbCenterModule[MOD_DB].stopFp)();
    }

    olog_info(db_msg_ocat,"dbCenter Reverse Init done");

}
