/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved.
* @brief： 数据中心服务全局配置
* @date： 2019-11-9
*********************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


#include "baseType.h"
#include "globalConfiguration.h"
#include "qlibc.h"
#include "qlibcext.h"
#include "framecomm.h"


GLOBAL_CONFIG_OBJ_T g_configObj;

/**
*********************************************************************
* @brief： 全局配置参数载入
* @param[in]： path        文件路径
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 global_config_init(const sint8 *path)
{
    // struct stat dirstat = {0};

    qlisttbl_t *tbl = qlisttbl(0);
    //获取全局配置树
    if (qconfig_parse_file(tbl, path, '=') == NULL)
    {
        SMIOS_FMT_DEBUG("Failed to open [%s].\n", path);
        return -1;
    }

    {
        //获取UDP 服务IP
        sint8 *udpServerIP = tbl->getstr(tbl, "service.udpServerIP", true);
        if (udpServerIP != NULL)
        {
            strncpy(g_configObj.serviceCfg.udpServerIP, udpServerIP, IP_LEN);
            //释放
            free(udpServerIP);
        }
    }

    {
        //获取TCP 服务IP
        sint8 *tcpServerIP = tbl->getstr(tbl, "service.tcpServerIP", true);
        if (tcpServerIP != NULL)
        {
            strncpy(g_configObj.serviceCfg.tcpServerIP, tcpServerIP, IP_LEN);
            //释放
            free(tcpServerIP);
        }
    }

    {
        //获取MQTT 服务IP
        sint8 *mqttBrokerIP = tbl->getstr(tbl, "service.mqttBrokerIP", true);
        if (mqttBrokerIP)
        {
            strncpy(g_configObj.serviceCfg.mqttBrokerIP, mqttBrokerIP, IP_LEN);
            //释放
            free(mqttBrokerIP);
        }
    }

    {
        //获取DEBUG 服务IP
        sint8 *debugSqlIP = tbl->getstr(tbl, "service.debugSqlIP", true);
        if (debugSqlIP)
        {
            strncpy(g_configObj.serviceCfg.debugSqlIP, debugSqlIP, IP_LEN);
            //释放
            free(debugSqlIP);
        }
    }

    {
        //获取mqtt用户名
        sint8 *mqttUserName = tbl->getstr(tbl, "service.mqttUserName", true);
        if (mqttUserName)
        {
            strncpy(g_configObj.serviceCfg.mqttUserName, mqttUserName,
                    MAX_USER_NAME_LEN);
            //释放
            free(mqttUserName);
        }
    }

    {
        //获取mqtt密码
        sint8 *mqttPassword = tbl->getstr(tbl, "service.mqttPassword", true);
        if (mqttPassword)
        {
            strncpy(g_configObj.serviceCfg.mqttPassword, mqttPassword,
                    MAX_PASSWD_LEN);
            //释放
            free(mqttPassword);
        }
    }

    {
        //获取MQTT端口
        g_configObj.serviceCfg.tcpServerPort =
            tbl->getint(tbl, "service.tcpServerPort");
        //获取MQTT端口
        g_configObj.serviceCfg.mqttBrokerPort =
            tbl->getint(tbl, "service.mqttBrokerPort");
        //获取MQTT订阅QOS
        g_configObj.serviceCfg.mqttSubQos =
            tbl->getint(tbl, "service.mqttSubQos");
        //获取UDP端口
        g_configObj.serviceCfg.udpServerPort =
            tbl->getint(tbl, "service.udpServerPort");
        //获取DEBUG端口
        g_configObj.serviceCfg.debugSqlPort =
            tbl->getint(tbl, "service.debugSqlPort");
        //获取最大接入量
        g_configObj.serviceCfg.maxClientAccept =
            tbl->getint(tbl, "service.maxClientAccept");
        //获取线程池最大线程数
        g_configObj.serviceCfg.handerThreadNum =
            tbl->getint(tbl, "service.handerThreadNum");
        //获取任务队列最大数量
        g_configObj.serviceCfg.handerTaskQueueSize =
            tbl->getint(tbl, "service.handerTaskQueueSize");
    }

    //平台配置获取
    {
        /*
        mqttBrokerIP=10.12.42.232
        mqttBrokerPort=1883
        mqttUserID=000000000001
        mqttUserName=username
        mqttPassword=passwd
        */
        //获取IP
        sint8 *mqttBrokerIP = tbl->getstr(tbl, "platform.mqttBrokerIP", true);
        if (mqttBrokerIP)
        {
            strncpy(g_configObj.platformCfg.mqttBrokerIP, mqttBrokerIP, IP_LEN);
            //释放
            free(mqttBrokerIP);
        }

        //获取用户id
        sint8 *mqttUserID = tbl->getstr(tbl, "platform.mqttUserID", true);
        if (mqttUserID)
        {
            strncpy(g_configObj.platformCfg.mqttUserID, mqttUserID,
                    sizeof(g_configObj.platformCfg.mqttUserID));
            //释放
            free(mqttUserID);
        }

        //获取用户名
        sint8 *mqttUserName = tbl->getstr(tbl, "platform.mqttUserName", true);
        if (mqttUserName)
        {
            strncpy(g_configObj.platformCfg.mqttUserName, mqttUserName,
                    sizeof(g_configObj.platformCfg.mqttUserName));
            //释放
            free(mqttUserName);
        }

        //获取password
        sint8 *mqttPassword = tbl->getstr(tbl, "platform.mqttPassword", true);
        if (mqttPassword)
        {
            strncpy(g_configObj.platformCfg.mqttPassword, mqttPassword,
                    sizeof(g_configObj.platformCfg.mqttPassword));
            ////释放
            free(mqttPassword);
        }

        //获取端口
        g_configObj.platformCfg.mqttBrokerPort =
            tbl->getint(tbl, "platform.mqttBrokerPort");

        //mqtt 服务名
        snprintf(g_configObj.serviceCfg.mqttServiceName, sizeof(g_configObj.serviceCfg.mqttServiceName), "M-%s",
                 SERVICE_NAME_BASE);
        
        //Tcp 服务名
        snprintf(g_configObj.serviceCfg.tcpServiceName, sizeof(g_configObj.serviceCfg.tcpServiceName), "T-%s:%s[%u]",
                 SERVICE_NAME_BASE, g_configObj.serviceCfg.tcpServerIP, g_configObj.serviceCfg.tcpServerPort);

        //udp 服务名
        snprintf(g_configObj.serviceCfg.udpServiceName, sizeof(g_configObj.serviceCfg.udpServiceName), "U-%s:%s[%u]",
                 SERVICE_NAME_BASE, g_configObj.serviceCfg.udpServerIP, g_configObj.serviceCfg.udpServerPort);
    }

    //fcgi IP
    {
        sint8 *ip = tbl->getstr(tbl, "fcgi.ip", true);
        if (ip)
        {
            strncpy(g_configObj.fcgiCfg.ip, ip, IP_LEN);
            //释放
            free(ip);
        }
        //port
        g_configObj.fcgiCfg.port = tbl->getint(tbl, "fcgi.port");
    }

    //app config
    {
        sint8 *localShareDir =
            tbl->getstr(tbl, "app.localShareDirectory", true);
        strncpy(g_configObj.serviceCfg.localShareDir, localShareDir,
                APP_MAX_SHARE_DIR_LEN);
        //释放
        free(localShareDir);

        //app 安装目录
        sint8 *localAppInstallDir =
            tbl->getstr(tbl, "app.localAppInstallDirectory", true);
        strncpy(g_configObj.serviceCfg.localAppInstallDir, localAppInstallDir,
                APP_MAX_SHARE_DIR_LEN);
        //释放
        free(localAppInstallDir);

        //app配置目录
        sint8 *localAppConfigDir =
            tbl->getstr(tbl, "app.localAppConfigDirectory", true);
        strncpy(g_configObj.serviceCfg.localAppConfigDir, localAppConfigDir,
                APP_MAX_SHARE_DIR_LEN);
        //释放
        free(localAppConfigDir);

        sint8 *dockerShareDir =
            tbl->getstr(tbl, "app.dockerShareDirectory", true);
        strncpy(g_configObj.serviceCfg.dockerShareDir, dockerShareDir,
                APP_MAX_SHARE_DIR_LEN);
        //释放
        free(dockerShareDir);

        //app启动列表配置文件
        sint8 *startListFile = tbl->getstr(tbl, "app.startListFile", true);
        strncpy(g_configObj.serviceCfg.startListFile, startListFile,
                APP_START_LIST_FILE_NAME_LEN);
        //释放
        free(startListFile);

        //app签名文件路径
        sint8 *localSignFileDirectory = tbl->getstr(tbl, "app.localSignFileDirectory", true);
        strncpy(g_configObj.serviceCfg.signFilePath, localSignFileDirectory,
                APP_SIGN_FILE_PATH_LEN);
        //释放
        free(localSignFileDirectory);

        //app注册文件
        sint8 *registerListFile = tbl->getstr(tbl, "app.registerListFile", true);
        strncpy(g_configObj.serviceCfg.registerListFile, registerListFile,
                DOCKER_START_LIST_FILE_NAME_LEN);
        //释放
        free(registerListFile);

		sint8 *localIpInfoFile = tbl->getstr(tbl, "app.localIpInfoFile", true);
		strncpy(g_configObj.serviceCfg.localIpInfoFile, localIpInfoFile,
			LOCAL_IP_FILE_NAME_LEN);
		free(localIpInfoFile);

    }

    g_configObj.serviceCfg.wdfeedtimerval =
        tbl->getint(tbl, "watchdog.feedtimervalsecond");

    g_configObj.serviceCfg.memratewarnlevel =
        tbl->getint(tbl, "system.memratewarnlevel");
    g_configObj.serviceCfg.cpuratewarnlevel =
        tbl->getint(tbl, "system.cpuratewarnlevel");
    g_configObj.serviceCfg.storagerateWarnlevel =
        tbl->getint(tbl, "system.storagerateWarn");

    {
        //容器启动列表配置文件
        sint8 *dockerStartListFile = tbl->getstr(tbl, "docker.dockerStartListFile", true);
        strncpy(g_configObj.serviceCfg.dockerstartListFile, dockerStartListFile,
                DOCKER_START_LIST_FILE_NAME_LEN);
        //释放
        free(dockerStartListFile);
    }

    if (tbl != NULL)
    {
        tbl->free(tbl);
    }

    return 0;
}

/**
*********************************************************************
* @brief： 全局配置debug打印出来
* @param[in]： none
* @param[out]：none
* @return：    none
*********************************************************************
*/
void global_config_dump()
{
    //udp IP
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.udpServerIP:[%s]\n",
                g_configObj.serviceCfg.udpServerIP);
    //udp port
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.udpServerPort:[%d]\n",
                g_configObj.serviceCfg.udpServerPort);

    //tcp ip
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.tcpServerIP:[%s]\n",
                g_configObj.serviceCfg.tcpServerIP);
    //tcp port
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.tcpServerPort:[%d]\n",
                g_configObj.serviceCfg.tcpServerPort);

    //mqtt broker ip
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.mqttBrokerIP:[%s]\n",
                g_configObj.serviceCfg.mqttBrokerIP);
    //mqtt broker port
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.mqttBrokerPort:[%d]\n",
                g_configObj.serviceCfg.mqttBrokerPort);
    //mqtt user name
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.mqttUserName:[%s]\n",
                g_configObj.serviceCfg.mqttUserName);
    //mqtt password
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.mqttPassword:[%s]\n",
                g_configObj.serviceCfg.mqttPassword);

    //sql IP
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.debugSqlIP:[%s]\n",
                g_configObj.serviceCfg.debugSqlIP);
    //sql port
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.debugSqlPort:[%d]\n",
                g_configObj.serviceCfg.debugSqlPort);
    //max client accept
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.maxClientAccept:[%d]\n",
                g_configObj.serviceCfg.maxClientAccept);
    //hander thread num
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.handerThreadNum:[%d]\n",
                g_configObj.serviceCfg.handerThreadNum);
    //hander task queue size
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.handerTaskQueueSize:[%d]\n",
                g_configObj.serviceCfg.handerTaskQueueSize);

    //fcgi ip
    SMIOS_FMT_DEBUG("g_configObj.fcgiCfg.ip:[%s]\n", g_configObj.fcgiCfg.ip);
    //fcgi port
    SMIOS_FMT_DEBUG("g_configObj.fcgiCfg.port:[%d]\n", g_configObj.fcgiCfg.port);

    //local share directory
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.localShareDir[%s]\n",
                g_configObj.serviceCfg.localShareDir);
    //docker share directory
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.dockerShareDir[%s]\n",
                g_configObj.serviceCfg.dockerShareDir);
    SMIOS_FMT_DEBUG("g_configObj.serviceCfg.wdfeedtimerval[%d]\n",
                g_configObj.serviceCfg.wdfeedtimerval);
}
