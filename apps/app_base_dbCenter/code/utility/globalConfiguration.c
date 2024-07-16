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
#include <syslog.h>
#include <unistd.h>

#include "baseType.h"
#include "dbComm.h"
#include "globalConfiguration.h"
#include "qlibc.h"
#include "qlibcext.h"
#include "olog.h"

GLOBAL_CONFIG_OBJ_T g_configObj;
extern void *     db_ocat;        //log print

/***********************************************************************
* @brief： 运行路径初始化
* @param[in]： 
* @param[out]：
               none        
* @return： none
**********************************************************************/
int dir_init(void)
{
    char curpath[256]   = {0};
    int  i              = 0;
    char command[1024]  = {0};
    
    memset(&g_configObj, 0, sizeof(GLOBAL_CONFIG_OBJ_T));

    //依赖路径创建
    sprintf(command, "mkdir -p %s", LOG_DIR);
    system(command);

    sprintf(command, "mkdir -p %s", CFG_PATH);
    system(command);

    sprintf(command, "mkdir -p %s", BAD_BACKUP_DB_PATH);
    system(command);

    sprintf(command, "mkdir -p %s", BACKUPPATH);
    system(command);

    sprintf(command, "mkdir -p %s", TMPFSPATH);
    system(command);

    //修改文件权限
    sprintf(command, "chmod -R 755 %s/*", FILE_PATH);
    system(command);
    sprintf(command, "chmod -R 755 %s", TMPFSPATH);
    system(command);


    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("dbCenter get err path!\n");
        syslog(LOG_ERR, "dbCenter get err path!\n");
        return -1;
    }
    else
    {
        for (i = cnt; i >=0; --i)
        {
            if (curpath[i] == '/')
            {
                curpath[i+1] = '\0';
                break;
            }
        }
    }

    printf("dbCenter path %s!\n", curpath);
    syslog(LOG_INFO, "dbCenter path %s!\n", curpath);

	//配置文件，转移到CFG_PATH目录下
    sprintf(g_configObj.dbCfg.cfgFile, "%s/%s", CFG_PATH, LOG_CFG_NAME);
    if (access(g_configObj.dbCfg.cfgFile, 0) == -1)
    {
        sprintf(command, "cp %s%s %s/", curpath, LOG_CFG_NAME, CFG_PATH);
        system(command);;
    }
/*
	//营销APP配置库，转移到CFG_PATH目录下
	sprintf(g_configObj.dbCfg.powerDb, "%s/%s", CFG_PATH, SAFE_POWER_NAME);
    if (access(g_configObj.dbCfg.powerDb, 0) == -1)
    {
        sprintf(command, "cp %s%s %s/", curpath, SAFE_POWER_NAME, CFG_PATH);
        system(command);;
    }
*/
    sprintf(g_configObj.dbCfg.logFile, "%s%s", curpath, LOG_LOCAL_NAME);
//    sprintf(g_configObj.dbCfg.cfgFile, "%s%s", curpath, LOG_CFG_NAME);
    sprintf(g_configObj.dbCfg.toolSqlite, "%s%s", curpath, SQLITE3_TOOL_NAME);
    sprintf(g_configObj.dbCfg.powerDb, "%s%s", curpath, SAFE_POWER_NAME);

    return 0;
}


/***********************************************************************
* @brief： 全局配置参数载入
* @param[in]： path        文件路径
* @param[out]：none
* @return： 返回值说明
*********************************************************************
*/
sint32 global_config_init(const sint8 *path)
{
    struct stat dirstat = {0};

    qlisttbl_t *tbl = qlisttbl(0);
    
    //获取全局配置树
    if (qconfig_parse_file(tbl, path, '=') == NULL)
    {
        olog_error(db_ocat,"Failed to open [%s].", path);
        return -1;
    }

    {
        //get wal count
        g_configObj.dbCfg.walCount = WALCOUNT;
    }

    {
        //get autoDeleteCheckPath
        strncpy(g_configObj.dbCfg.checkDataPath, CHECH_DATA_PATH, sizeof(g_configObj.dbCfg.checkDataPath));
        sint8 *autoDeleteCheckPath = tbl->getstr(tbl, "db.autoDeleteCheckPath", true);
        if (autoDeleteCheckPath != NULL)
        {
            strncpy(g_configObj.dbCfg.checkDataPath, autoDeleteCheckPath, sizeof(g_configObj.dbCfg.checkDataPath));
            free(autoDeleteCheckPath);
        }
    }

    {
        //获取UDP 服务IP
        strncpy(g_configObj.serviceCfg.udpServerIP, UDPSERVERIP, IP_LEN);
    }

    {
        //获取TCP 服务IP
        strncpy(g_configObj.serviceCfg.tcpServerIP, TCPSERVERIP, IP_LEN);
    }

	//获取MQTT 服务IP
	sint8 *mqttBrokerIP = tbl->getstr(tbl, "service.mqttBrokerIP", true);
    if (mqttBrokerIP != NULL)
    {
        strncpy(g_configObj.serviceCfg.mqttBrokerIP, mqttBrokerIP, IP_LEN);
        free(mqttBrokerIP);
    }
	else
	{
        strncpy(g_configObj.serviceCfg.mqttBrokerIP, MQTTBROKERIP, IP_LEN);
    }

	//获取MQTT端口
    g_configObj.serviceCfg.mqttBrokerPort = tbl->getint(tbl, "service.mqttBrokerPort");
	if (g_configObj.serviceCfg.mqttBrokerPort == 0)
    {
        g_configObj.serviceCfg.mqttBrokerPort = MQTTBROKERPORT;
    }

	olog_info(db_ocat,"g_configObj.serviceCfg.mqttBrokerIP:[%s]",
                g_configObj.serviceCfg.mqttBrokerIP);
    olog_info(db_ocat,"g_configObj.serviceCfg.mqttBrokerPort:[%d]",
                g_configObj.serviceCfg.mqttBrokerPort);

    {
        //获取DEBUG 服务IP
        strncpy(g_configObj.serviceCfg.debugSqlIP, DEBUGSQLIP, IP_LEN);
    }

    {
        //获取连接系统管理器类型
        strncpy(g_configObj.serviceCfg.smiOSConnectType, SMIOSCONNECTTYPE,
                    sizeof(g_configObj.serviceCfg.smiOSConnectType));
    }

    {
        //获取系统管理器的IP
        strncpy(g_configObj.serviceCfg.smiOSIP, SMIOSIP, IP_LEN);
    }

    {
        //获取mqtt服务用户名
        strncpy(g_configObj.serviceCfg.mqttUserName, MQTTUSERNAME,
                    MAX_USER_NAME_LEN);
    }

    {
        //获取mqtt服务密码
        strncpy(g_configObj.serviceCfg.mqttPassword, MQTTPASSWORD,
                    MAX_PASSWD_LEN);
    }

    {
        //获取系统管理区的UDP端口
        g_configObj.serviceCfg.smiOSPort = SMIOSPORT;
        //获取TCP端口
        g_configObj.serviceCfg.tcpServerPort = TCPSERVERPORT;
        //获取MQTT端口
//        g_configObj.serviceCfg.mqttBrokerPort = MQTTBROKERPORT;
        //获取UDP端口
        g_configObj.serviceCfg.udpServerPort = UDPSERVERPORT;
        //获取DEBUG端口
        g_configObj.serviceCfg.debugSqlPort = DEBUGSQLPORT;
        //获取最大接入量
        g_configObj.serviceCfg.maxClientAccept = MAXCLIENTACCEPT;
        //获取线程池最大线程数
        g_configObj.serviceCfg.handerThreadNum = HANDERTHREADNNUM;
        //获取任务队列最大数量
        g_configObj.serviceCfg.handerTaskQueueSize = HANDERTASKQUEUESIZE;
    }

    {
        //注册服务类型 使用连接smiOS连接类型 @service.smiOSConnectType
       
        //获取注册服务类型
        strncpy(g_configObj.serviceCfg.registerServerType,
                 SMIOSCONNECTTYPE,
                sizeof(g_configObj.serviceCfg.registerServerType));


        switch (*g_configObj.serviceCfg.registerServerType)
        {
            case 't':
            case 'T':
                //TCP连接
                snprintf(g_configObj.serviceCfg.serverName,
                         sizeof(g_configObj.serviceCfg.serverName),
                         "T-%s:%s[%u]", SERVICE_BASE_NAME,
                         g_configObj.serviceCfg.tcpServerIP,
                         g_configObj.serviceCfg.tcpServerPort);
                break;
            case 'u':
            case 'U':
                //UDP连接
                snprintf(g_configObj.serviceCfg.serverName,
                         sizeof(g_configObj.serviceCfg.serverName),
                         "U-%s:%s[%u]", SERVICE_BASE_NAME,
                         g_configObj.serviceCfg.udpServerIP,
                         g_configObj.serviceCfg.udpServerPort);
                break;
            case 'M':
            case 'm':
                //MQTT连接
                snprintf(g_configObj.serviceCfg.serverName,
                         sizeof(g_configObj.serviceCfg.serverName), 
                         "M-%s",
                         SERVICE_BASE_NAME);
                break;

            default:
                break;
        }

    }

    {
        //获取自动删除数量
        g_configObj.dbCfg.autoDeleteNum = AUTODELETENUM;
        //删除时间间隔
        g_configObj.dbCfg.deleteInterval = tbl->getint(tbl, "db_auto_del.del_period");
		if(g_configObj.dbCfg.deleteInterval <= 0)
		{
			g_configObj.dbCfg.deleteInterval = DELETEINTERVAL;
		}

		//获取自动删除剩余空间百分比
		g_configObj.dbCfg.autoDeleteNum = tbl->getint(tbl, "db_auto_del.del_space_pre");
		if(g_configObj.dbCfg.autoDeleteNum <= 0)
		{
			g_configObj.dbCfg.autoDeleteNum = AUTODELETENUM;
		}
		
        //获取数据库基础路径
        strncpy(g_configObj.dbCfg.basePath, BASEPATH, PATH_LEN);
        if (stat(g_configObj.dbCfg.basePath, &dirstat) < 0)
        {
            //基础路径不存在则自动创建
            mkdir(g_configObj.dbCfg.basePath, 0755);
        }

        //获取数据库备份路径
        strncpy(g_configObj.dbCfg.backupPath, BACKUPPATH, PATH_LEN);
        if (stat(g_configObj.dbCfg.backupPath, &dirstat) < 0)
        {
            //备份路径不存在则自动创建
            mkdir(g_configObj.dbCfg.backupPath, 0755);
        }
        //获取虚拟内存路径
        strncpy(g_configObj.dbCfg.tmpfsPath, TMPFSPATH, sizeof(g_configObj.dbCfg.tmpfsPath));
        if (stat(g_configObj.dbCfg.tmpfsPath, &dirstat) < 0)
        {
            //虚拟内存路径不存在则自动创建
            mkdir(g_configObj.dbCfg.tmpfsPath, 0755);
        }
    }

    //加载数据配置表
    load_normal_data_cfg(tbl);
    //加载记录深度配置表
    load_tab_depth_cfg(tbl);

    //get delete default depth value
    g_configObj.dbCfg.deleteDepthValue = DEFOULTDEPTH;
    olog_debug(db_ocat,"g_configObj.dbCfg.deleteDepthValue=%d", g_configObj.dbCfg.deleteDepthValue);

    //Load the table config info
    load_tab_depth_Info(tbl);

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
    /*
    olog_debug(db_ocat, "g_configObj.dbCfg.autoDeleteCheckPath:[%s]", g_configObj.dbCfg.autoDeleteCheckPath);
    olog_debug(db_ocat, "g_configObj.dbcfg.walCount:[%d]", g_configObj.dbCfg.walCount);
    olog_debug(db_ocat,"g_configObj.serviceCfg.serverName:[%s]",
                g_configObj.serviceCfg.serverName);
    olog_debug(db_ocat,"g_configObj.serviceCfg.udpServerIP:[%s]",
                g_configObj.serviceCfg.udpServerIP);
    olog_debug(db_ocat,"g_configObj.serviceCfg.udpServerPort:[%d]",
                g_configObj.serviceCfg.udpServerPort);
    olog_debug(db_ocat,"g_configObj.serviceCfg.debugSqlIP:[%s]",
                g_configObj.serviceCfg.debugSqlIP);
    olog_debug(db_ocat,"g_configObj.serviceCfg.debugSqlPort:[%d]",
                g_configObj.serviceCfg.debugSqlPort);
    olog_debug(db_ocat,"g_configObj.serviceCfg.tcpServerIP:[%s]",
                g_configObj.serviceCfg.tcpServerIP);
    olog_debug(db_ocat,"g_configObj.serviceCfg.tcpServerPort:[%d]",
                g_configObj.serviceCfg.tcpServerPort);
    olog_debug(db_ocat,"g_configObj.serviceCfg.mqttBrokerIP:[%s]",
                g_configObj.serviceCfg.tcpServerIP);
    olog_debug(db_ocat,"g_configObj.serviceCfg.mqttBrokerPort:[%d]",
                g_configObj.serviceCfg.mqttBrokerPort);
    olog_debug(db_ocat,"g_configObj.serviceCfg.smiOSConnectType:[%s]",
                g_configObj.serviceCfg.smiOSConnectType);
    olog_debug(db_ocat,"g_configObj.serviceCfg.smiOSIP:[%s]",
                g_configObj.serviceCfg.smiOSIP);
    olog_debug(db_ocat,"g_configObj.serviceCfg.smiOSPort:[%d]",
                g_configObj.serviceCfg.smiOSPort);
    olog_debug(db_ocat,"g_configObj.serviceCfg.mqttUserName:[%s]",
                g_configObj.serviceCfg.mqttUserName);
    olog_debug(db_ocat,"g_configObj.serviceCfg.mqttPassword:[%s]",
                g_configObj.serviceCfg.mqttPassword);
    olog_debug(db_ocat,"g_configObj.serviceCfg.maxClientAccept:[%d]",
                g_configObj.serviceCfg.maxClientAccept);
    olog_debug(db_ocat,"g_configObj.serviceCfg.handerThreadNum:[%d]",
                g_configObj.serviceCfg.handerThreadNum);
    olog_debug(db_ocat,"g_configObj.serviceCfg.handerTaskQueueSize:[%d]",
                g_configObj.serviceCfg.handerTaskQueueSize);

    olog_debug(db_ocat,"g_configObj.DB_CONFIG_OBJ_T.autoDeleteNum:[%d]",
                g_configObj.dbCfg.autoDeleteNum);
    olog_debug(db_ocat,"g_configObj.dbCfg.basePath:[%s]", g_configObj.dbCfg.basePath);
    olog_debug(db_ocat,"g_configObj.dbCfg.backupPath:[%s]",
                g_configObj.dbCfg.backupPath);
    */
}
