/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   无线远程拨号模块主线程功能
* @date：    2019-10-23
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "hal.h"
#include "devinfo.h"
#include "appmsg_split.h"
#include "iniparser.h"
#include "gui_c_prtl.h"

#include "wireless.h"
#include "wire_mqtt.h"
#include "ppp_oper.h"


#define WIRELESS_INI_FILE   "wirelessDCM.ini"
#define WIRELESS_CONF_FILE  "default_conf.ini"
#define WIRELESS_USER_FILE  "userparam.dat"

/*-----------------------全局变量--------------------------*/
char       g_UserFile[256] = {0};
GPS_T      g_GpsPosition0;
GPS_T      g_GpsPosition1;

GPS_SET_T  g_GpsSet;    //GPS设置指令

NET_PARA_T g_tNetPara0;
NET_PARA_T g_tNetPara1;

/* 消息队列 */
WIRELESS_QUEUE_T g_tAMsgQueueAT0;
WIRELESS_QUEUE_T g_tAMsgQueueAT1;

WIRELESS_DATA_T g_tWireData; 

/**********************************************************************
* @name      : wireless_get_handle
* @brief     ：获取拨号参数句柄
* @param[in] ：uint8 id  设备ID
* @param[out]：
* @return    ：句柄
* @Create    : 王津剑
* @Date      ：2020-5-3
* @Update    : 
**********************************************************************/
uint8 wireless_check_1_8G()
{
    return g_tWireData.bJS1_8G;
}

/**********************************************************************
* @name      : wireless_get_handle
* @brief     ：获取拨号参数句柄
* @param[in] ：uint8 id  设备ID
* @param[out]：
* @return    ：句柄
* @Create    : 王津剑
* @Date      ：2020-5-3
* @Update    : 
**********************************************************************/
WIRELESS_HANDLE_T *wireless_get_handle(uint8 id)
{
    if(1 == id)
    {
        return &g_tWireData.pppdata0;
    }
    else if(2 == id)
    {
        return &g_tWireData.pppdata1;
    }
    else
    {
        return NULL;
    }
}

/**********************************************************************
* @name      : wireless_get_athandle
* @brief     ：获取拨号参数句柄
* @param[in] ：uint8 id  设备ID
* @param[out]：
* @return    ：句柄
* @Create    : 王津剑
* @Date      ：2020-7-27
* @Update    : 
**********************************************************************/
WIRELESS_HANDLE_T *wireless_get_athandle(uint8 id)
{
    if(1 == id)
    {
        return &g_tWireData.atdata0;
    }
    else if(2 == id)
    {
        return &g_tWireData.atdata1;
    }
    else
    {
        return NULL;
    }
}

/**********************************************************************
* @name      : wireless_set_apn
* @brief     ：设置无线拨号参数
* @param[in] ：uint8 id    设备ID
               uint8 operate
               char *apn
               char *user
               char *pwd
               uint8 checknum
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-5-3
* @Update    :
**********************************************************************/
int wireless_set_apn(uint8 id, uint8 operate, char *apn, char *user, char *pwd)
{
    WIRE_DIAG_APN_T userAPN;

    if((1 != id && 2 != id) || NULL == apn)
    {
        return -1;
    }
    
    memset(&userAPN, 0, sizeof(WIRE_DIAG_APN_T));
    userAPN.id = id;
    userAPN.diagnum = 1;
    userAPN.tDiagApn[0].operat = operate;
    userAPN.tDiagApn[0].apn.nNum = strlen(apn);
    if(0 == userAPN.tDiagApn[0].apn.nNum)
    {
        return -2;
    }
    memcpy(userAPN.tDiagApn[0].apn.value, apn, strlen(apn));
    
    if(NULL != user)
    {
        userAPN.tDiagApn[0].username.nNum = strlen(user);
        if(userAPN.tDiagApn[0].username.nNum > 0)
        {
            memcpy(userAPN.tDiagApn[0].username.value, user, strlen(user));
        }
    }
    if(NULL != pwd)
    {
        userAPN.tDiagApn[0].pwd.nNum = strlen(pwd);
        if(userAPN.tDiagApn[0].pwd.nNum > 0)
        {
            memcpy(userAPN.tDiagApn[0].pwd.value, pwd, strlen(pwd));
        }
    }

    if(1 == userAPN.id)
    {
        /* 运营商参数检查 */
        if(WIRELESS_UNUSED_COMMPORT == g_tWireData.pppdata0.commport)
        {
            if(0 != memcmp(&(g_tNetPara0.userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T)))
            {
                memcpy(&(g_tNetPara0.userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T));
                /* 写到配置文件中 */
                wireless_write_user_apn(g_UserFile, 1, &userAPN);
            }
            return 0;
        }
        
        if(g_tNetPara0.operat > WIRELESS_ZWCC || g_tNetPara0.operat != operate)
        {
            return -3;
        }

        if(FALSE == g_tWireData.pppdata0.bUserApn ||
           0 != memcmp(&(g_tNetPara0.userAPN.tDiagApn[g_tWireData.pppdata0.apnIndex]), &(userAPN.tDiagApn[0]), sizeof(WIRE_APN_T)))
        {
            /* 重新连接 */
            WIRELESS_FMT_DEBUG("设备1修改APN, 重新连接!\n");
            /* 为了保证重拨顺利,重启设备 */
            wireless_restart_dev(0, g_tWireData.pppdata0.commport + ((g_tWireData.pppdata0.subport << 4) & 0xf0));
        }
        else
        {
            WIRELESS_FMT_DEBUG("设备1修改APN, 不需要重新连接!\n");
        }
        g_tWireData.pppdata0.apnIndex = 0;

        if(0 != memcmp(&(g_tNetPara0.userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T)))
        {
            memcpy(&(g_tNetPara0.userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T));
            /* 写到配置文件中 */
            wireless_write_user_apn(g_UserFile, 1, &userAPN);
        }
    }
    else if(2 == userAPN.id)
    {
        /* 运营商参数检查 */
        if(WIRELESS_UNUSED_COMMPORT == g_tWireData.pppdata1.commport)
        {
            if(0 != memcmp(&(g_tNetPara1.userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T)))
            {
                memcpy(&(g_tNetPara1.userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T));
                /* 写到配置文件中 */
                wireless_write_user_apn(g_UserFile, 2, &userAPN);
            }
            return 0;
        }
        
        if(g_tNetPara1.operat > WIRELESS_ZWCC || g_tNetPara1.operat != operate)
        {
            return -3;
        }

        if(FALSE == g_tWireData.pppdata1.bUserApn ||
           0 != memcmp(&(g_tNetPara1.userAPN.tDiagApn[g_tWireData.pppdata1.apnIndex]), &(userAPN.tDiagApn[0]), sizeof(WIRE_APN_T)))
        {
            WIRELESS_FMT_DEBUG("设备2修改APN, 重新连接!\n");
            /* 为了保证重拨顺利,重启设备 */
            wireless_restart_dev(1, g_tWireData.pppdata1.commport + ((g_tWireData.pppdata1.subport << 4) & 0xf0));
        }
        else
        {
            WIRELESS_FMT_DEBUG("设备2修改APN, 不需要重新连接!\n");
        }
        g_tWireData.pppdata1.apnIndex = 0;

        if(0 != memcmp(&(g_tNetPara1.userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T)))
        {
            memcpy(&(g_tNetPara1.userAPN), &userAPN, sizeof(WIRE_DIAG_APN_T));
            /* 写到配置文件中 */
            wireless_write_user_apn(g_UserFile, 2, &userAPN);
        }
    }

    return 0;
}

/**********************************************************************
* @name      : wireless_set_auth
* @brief     ：设置鉴权模式
* @param[in] ：uint8 id    设备ID
               uint8 authType
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-5-3
* @Update    :
**********************************************************************/
int wireless_set_auth(uint8 id, uint8 authType)
{
    if((1 != id && 2 != id) || (authType >= 4 && authType < 255))
    {
        return -1;
    }

    WIRELESS_FMT_DEBUG("模块%d设置鉴权方式%d\n", id, authType);
    if(1 == id)
    {
        if(g_tWireData.pppdata0.ptNetPara->authType != authType)
        {
            g_tWireData.pppdata0.ptNetPara->authType = authType;
            if(WIRELESS_UNUSED_COMMPORT != g_tWireData.pppdata0.commport && authType < 4)
            {
                /* 为了保证重拨顺利,重启设备 */
                wireless_restart_dev(0, g_tWireData.pppdata0.commport + ((g_tWireData.pppdata0.subport << 4) & 0xf0));
            }
            wireless_write_authtype(g_UserFile, 1, authType);
        }
    }
    else if(2 == id)
    {
        if(g_tWireData.pppdata1.ptNetPara->authType != authType)
        {
            g_tWireData.pppdata1.ptNetPara->authType = authType;
            if(WIRELESS_UNUSED_COMMPORT != g_tWireData.pppdata1.commport && authType < 4)
            {
                /* 为了保证重拨顺利,重启设备 */
                wireless_restart_dev(1, g_tWireData.pppdata1.commport + ((g_tWireData.pppdata1.subport << 4) & 0xf0));
            }
            wireless_write_authtype(g_UserFile, 2, authType);
        }
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_data_init
* @brief     ：无线远程拨号全局数据初始化
* @param[in] ：WIRELESS_DATA_T *ptWireData   全局数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    :
**********************************************************************/
void wireless_data_init(WIRELESS_DATA_T *ptWireData)
{
    /* 初始化 */
    memset(&g_GpsPosition0, 0, sizeof(GPS_T));
    memset(&g_GpsPosition1, 0, sizeof(GPS_T));
    g_GpsPosition0.ststus = 0;
    g_GpsPosition0.mode = 4;
    g_GpsPosition1.ststus = 0;
    g_GpsPosition1.mode = 4;

    memset(&g_tNetPara0, 0, sizeof(NET_PARA_T));
    memset(&g_tNetPara1, 0, sizeof(NET_PARA_T));
    g_tNetPara0.operat = WIRELESS_OPT_NONE;
    g_tNetPara1.operat = WIRELESS_OPT_NONE;
    g_tNetPara0.networkType = OOP_NET_UNKNOW; /* 网络类型 */
    g_tNetPara1.networkType = OOP_NET_UNKNOW; /* 网络类型 */
    g_tNetPara0.pGps = &g_GpsPosition0;
    g_tNetPara1.pGps = &g_GpsPosition1;
    g_tNetPara0.authType = 255;
    g_tNetPara1.authType = 255;
    g_tNetPara0.pGpsSet = &g_GpsSet;
    g_tNetPara1.pGpsSet = &g_GpsSet;

    memset(&g_tAMsgQueueAT0, 0, sizeof(WIRELESS_QUEUE_T));
    memset(&g_tAMsgQueueAT1, 0, sizeof(WIRELESS_QUEUE_T));
    pthread_mutex_init(&g_tAMsgQueueAT0.f_lock, NULL);
    pthread_mutex_init(&g_tAMsgQueueAT1.f_lock, NULL);

    memset(ptWireData, 0, sizeof(WIRELESS_DATA_T));
    ptWireData->appidlen = 24;

    ptWireData->pppdata0.bsimOK = PPP_DAIL_STATUS_OFFLINE;
    ptWireData->pppdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
    ptWireData->pppdata0.commport = WIRELESS_UNUSED_COMMPORT;
    ptWireData->pppdata0.ptNetPara = &g_tNetPara0;
    ptWireData->pppdata0.userfile = g_UserFile;
    ptWireData->pppdata0.stackChoice = 2;

    ptWireData->pppdata1.bsimOK = PPP_DAIL_STATUS_OFFLINE;
    ptWireData->pppdata1.dialState = PPP_DAIL_STATUS_OFFLINE;
    ptWireData->pppdata1.commport = WIRELESS_UNUSED_COMMPORT;
    ptWireData->pppdata1.ptNetPara = &g_tNetPara1;
    ptWireData->pppdata1.userfile = g_UserFile;
    ptWireData->pppdata1.stackChoice = 2;

    ptWireData->atdata0.bsimOK = PPP_DAIL_STATUS_OFFLINE;
    ptWireData->atdata0.dialState = PPP_DAIL_STATUS_OFFLINE;
    ptWireData->atdata0.commport = WIRELESS_UNUSED_COMMPORT;
    ptWireData->atdata0.ptNetPara = &g_tNetPara0;
    ptWireData->atdata0.ptMsgQueue= &g_tAMsgQueueAT0;
    ptWireData->atdata0.userfile = g_UserFile;

    ptWireData->atdata1.bsimOK = PPP_DAIL_STATUS_OFFLINE;
    ptWireData->atdata1.dialState = PPP_DAIL_STATUS_OFFLINE;
    ptWireData->atdata1.commport = WIRELESS_UNUSED_COMMPORT;
    ptWireData->atdata1.ptNetPara = &g_tNetPara1;
    ptWireData->atdata1.ptMsgQueue= &g_tAMsgQueueAT1;
    ptWireData->atdata1.userfile = g_UserFile;
}

/**********************************************************************
* @name      : wireless_get_other_default
* @brief     ：无线拨号获取其他信息
* @param[in] ：char *filepath    文件
* @param[out]：uint8 *pb1_8G      是否开启江苏1.8G功能
               GPS_SET_T *ptSet  定位模式设置
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-9-5
* @Update    :
**********************************************************************/
void wireless_get_other_default(char *filepath, uint8 *pb1_8G, GPS_SET_T *ptSet)
{
    dictionary *ini = NULL;
    char  *stringvalue = NULL;
    int    intvalue = 0;

    if(NULL == pb1_8G || NULL == ptSet)
    {
        return;
    }
    
    if(NULL != filepath)
    {
        ini = iniparser_load(filepath);
        if(NULL == ini)
        {
            WIRELESS_FMT_DEBUG("配置文件%s无效\n", filepath);
        }
    }

    *pb1_8G = 0;
    memset(ptSet, 0, sizeof(GPS_SET_T));
    /* 无效 */
    if(NULL == filepath || NULL == ini)
    {
        return;
    }
    
    intvalue = iniparser_getint(ini, "AREA_CONF:JS_1.8G", 0);
    *pb1_8G = intvalue != 0 ? 1: 0;

    intvalue = iniparser_getint(ini, "GPS_SET_CMD:OPEN_STATE", 0);
    if(0 != intvalue)
    {
        stringvalue = iniparser_getstring(ini, "GPS_SET_CMD:SET_CMD", NULL);
        if(NULL != stringvalue)
        {
            memcpy(ptSet->setcmd, stringvalue, MIN(23, strlen(stringvalue)));
        }

        intvalue = iniparser_getint(ini, "GPS_SET_CMD:GPS_MODE", 1);
        ptSet->gps = (uint8)intvalue;

        intvalue = iniparser_getint(ini, "GPS_SET_CMD:BD_MODE", 2);
        ptSet->bd = (uint8)intvalue;

        intvalue = iniparser_getint(ini, "GPS_SET_CMD:BOTH_MODE", 3);
        ptSet->both = (uint8)intvalue;

        intvalue = iniparser_getint(ini, "GPS_SET_CMD:DEF_MODE", 3);
        ptSet->def = (uint8)intvalue;

        stringvalue = iniparser_getstring(ini, "GPS_SET_CMD:GET_CMD", NULL);
        if(NULL != stringvalue)
        {
            memcpy(ptSet->getcmd, stringvalue, MIN(23, strlen(stringvalue)));
        }
    }
    
    
    iniparser_freedict(ini);
    return;
    
}

/**********************************************************************
* @name      : wireless_init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：uint32 *ip    服务端地址
               uint16 *port  服务端端口
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-2-27
* @Update    : 2021-3-22
**********************************************************************/
int wireless_init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    char   conffile[256] = {0};
    char   systemcmd[512] = {0};
    uint16 rebootTime = 0;
    uint8  bipv6 = 0;
    uint8  lognum = 0;
    char   logpath[256] = {0};

    WIRE_DIAG_APN_T defaultAPN;

    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("wireless get err path!\n");
    }
    //获取当前目录绝对路径，即去掉程序名
    int i;
    for (i = cnt; i >=0; --i)
    {
        if (curpath[i] == '/')
        {
            curpath[i+1] = '\0';
            break;
        }
    }

    sprintf(filename, "%s%s", curpath, WIRELESS_INI_FILE);
    /* 日志模块初始化 */
    diag_init(filename, TRUE);
    
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
        *ip = ntohl(ipaddr);

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;
        
        /* 获取data路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:DATAPATH", "/data/app/wirelessDCM/data");
        if(0 != access(stringvalue, F_OK))
        {
            /* 必须先创建文件夹 */
            if(0 != mkdir(stringvalue, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                printf("Create data filepath %s, failed\n", stringvalue);
            }
        }
        sprintf(g_UserFile, "%s/%s", stringvalue, WIRELESS_USER_FILE);

        /* 用户配置文件拷贝 */
        sprintf(conffile, "%s/%s", stringvalue, WIRELESS_CONF_FILE);
        if(0 != access(conffile, F_OK))
        {
            /* 不存在 从当前路径中拷贝 */
            sprintf(systemcmd, "cp %s%s %s/", curpath, WIRELESS_CONF_FILE, stringvalue);
            system(systemcmd);
            WIRELESS_FMT_DEBUG("拷贝用户配置文件到/data中\n");
            sleep(1);
            sprintf(systemcmd, "chmod 755 %s", conffile);
            system(systemcmd); //设置权限
        }

        iniparser_freedict(ini);

        /* 默认参数获取 */
        bzero(&defaultAPN, sizeof(WIRE_DIAG_APN_T));
        wireless_get_default_conf(conffile, &defaultAPN, &rebootTime, &bipv6, &lognum, logpath);
        defaultAPN.id = 1;
        memcpy(&(g_tNetPara0.defaultAPN), &defaultAPN, sizeof(WIRE_DIAG_APN_T));
        defaultAPN.id = 2;
        memcpy(&(g_tNetPara1.defaultAPN), &defaultAPN, sizeof(WIRE_DIAG_APN_T));
        g_tNetPara0.SimCheckTime = rebootTime;
        g_tNetPara1.SimCheckTime = rebootTime;

        wireless_get_other_default(conffile, &g_tWireData.bJS1_8G, &g_GpsSet);
        g_tWireData.atdata0.bJS1_8G = g_tWireData.bJS1_8G;
        g_tWireData.atdata1.bJS1_8G = g_tWireData.bJS1_8G;
        g_tWireData.pppdata0.bJS1_8G = g_tWireData.bJS1_8G;
        g_tWireData.pppdata1.bJS1_8G = g_tWireData.bJS1_8G;
    }
    else
    {
        printf("wireless get no %s!\n", WIRELESS_INI_FILE);
        if(0 != access("/data/app/wirelessDCM/data", F_OK))
        {
            if(0 != mkdir("/data/app/wirelessDCM/data", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
            {
                printf("Create data filepath /data/app/wirelessDCM/data, failed\n");
            }
        }
        sprintf(g_UserFile, "/data/app/wirelessDCM/data/%s", WIRELESS_USER_FILE);

        /* 获取默认的APN参数 */
        if(0 != access("/data/app/wirelessDCM/data/default_conf.ini", F_OK))
        {
            /* 不存在 从当前路径中拷贝 */
            system("cp /usr/local/extapps/wirelessDCM/bin/default_conf.ini /data/app/wirelessDCM/data/");
            sleep(1);
            system("chmod 755 /data/app/wirelessDCM/data/default_conf.ini"); //设置权限
        }
        bzero(&defaultAPN, sizeof(WIRE_DIAG_APN_T));
        wireless_get_default_conf("/data/app/wirelessDCM/data/default_conf.ini", &defaultAPN, &rebootTime,
                                  &bipv6, &lognum, logpath);
        defaultAPN.id = 1;
        memcpy(&(g_tNetPara0.defaultAPN), &defaultAPN, sizeof(WIRE_DIAG_APN_T));
        defaultAPN.id = 2;
        memcpy(&(g_tNetPara1.defaultAPN), &defaultAPN, sizeof(WIRE_DIAG_APN_T));
        g_tNetPara0.SimCheckTime = rebootTime;
        g_tNetPara1.SimCheckTime = rebootTime;
    }

    g_tWireData.pppdata0.bIPV6 = bipv6;
    g_tWireData.pppdata0.pppdlogMax = lognum;
    memcpy(g_tWireData.pppdata0.logpath, logpath, 256);
     g_tWireData.pppdata1.bIPV6 = bipv6;
    g_tWireData.pppdata1.pppdlogMax = lognum;
    memcpy(g_tWireData.pppdata1.logpath, logpath, 256);

    if(0 == lognum)
    {
        sprintf(systemcmd, "rm %s/ppp*.LOG", logpath);
        system(systemcmd);
    }
    WIRELESS_FMT_DEBUG("IPV6 %s, PPPD日志 %s\n", 0 == bipv6 ? "关闭" : "打开", 0 == lognum ? "关闭" : "打开");
    
    wireless_check_userfile(g_UserFile);
    /* 获取用户的APN参数 */
    wireless_get_user_apn(g_UserFile, 1, &(g_tNetPara0.userAPN));
    wireless_get_user_apn(g_UserFile, 2, &(g_tNetPara1.userAPN));
    g_tNetPara0.networkSet = g_tNetPara0.userAPN.network;
    g_tNetPara1.networkSet = g_tNetPara1.userAPN.network;
    g_tNetPara0.authType = wireless_get_authtype(g_UserFile, 1);
    g_tNetPara1.authType = wireless_get_authtype(g_UserFile, 2);
    wireless_get_centernum(g_UserFile, 1, g_tNetPara0.centerNnm, SMS_PDU_PHONE_LEN, &(g_tNetPara0.smsType));
    wireless_get_centernum(g_UserFile, 2, g_tNetPara1.centerNnm, SMS_PDU_PHONE_LEN, &(g_tNetPara1.smsType));
    if(g_tNetPara0.smsType > 1)
    {
        g_tNetPara0.smsType = 0;
    }
    if(g_tNetPara1.smsType > 1)
    {
        g_tNetPara1.smsType = 0;
    }

    g_tNetPara0.flowSetValue = wireless_get_flowset(g_UserFile, 1);
    g_tNetPara1.flowSetValue = wireless_get_flowset(g_UserFile, 2);
    g_tNetPara0.usedflow = wireless_get_userflow(g_UserFile, 1, &(g_tNetPara0.usertime));
    g_tNetPara1.usedflow = wireless_get_userflow(g_UserFile, 2, &(g_tNetPara1.usertime));

    wireless_get_num(g_UserFile, 1, &g_tWireData.pppdata0.curDay, &g_tWireData.pppdata0.ConnectNum, 
                     &g_tWireData.pppdata0.FailedNum, &g_tWireData.pppdata0.SuccessTimes);
    set_curtime_ms(g_tWireData.pppdata0.SuccessTimes, &g_tWireData.pppdata0.Connecttime);
    wireless_get_num(g_UserFile, 2, &g_tWireData.pppdata1.curDay, &g_tWireData.pppdata1.ConnectNum, 
                     &g_tWireData.pppdata1.FailedNum, &g_tWireData.pppdata1.SuccessTimes);
    set_curtime_ms(g_tWireData.pppdata1.SuccessTimes, &g_tWireData.pppdata1.Connecttime);

    wireless_get_logno(g_UserFile, 1, &g_tWireData.pppdata0.logNo);
    wireless_get_logno(g_UserFile, 2, &g_tWireData.pppdata1.logNo);

    if(0 != wireless_get_gpsmode(g_UserFile, 1, &g_tWireData.atdata0.setGpsMode))
    {
        g_tWireData.atdata0.setGpsMode = g_GpsSet.def;
    }
    g_tWireData.atdata0.curGpsMode = 255;
    if(0 != wireless_get_gpsmode(g_UserFile, 2, &g_tWireData.atdata1.setGpsMode))
    {
        g_tWireData.atdata1.curGpsMode = g_GpsSet.def;
    }
    g_tWireData.atdata1.curGpsMode = 255;

    if(0 != g_tWireData.bJS1_8G)
    {
        WIRELESS_FMT_DEBUG("支持江苏1.8G公专一体模块\n");
    }
    if(0 != g_GpsSet.setcmd[0])
    {
        WIRELESS_FMT_DEBUG("支持模块设置定位工作模式\n");
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_get_appid
* @brief     ：获取APPID
* @param[in] ：uint8 len     缓存长度
* @param[out]：uint8 *appid  APPID
* @return    ：长度
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    : 
**********************************************************************/
uint8 wireless_get_appid(uint8 *appid, uint8 len)
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        memset(appid, 0x00, len);
        WIRELESS_FMT_DEBUG("devinfo hal_device_get error \n");
        return MIN(24, len);
    }
    else
    {
        ret = dev->devinfo_read_appid(dev, "wirelessDCM", (char*)appid, len);
        if(ERR_OK != ret)
        {
            WIRELESS_FMT_DEBUG("devinfo devinfo_read_appid error ret %d\n", ret);
            memset(appid, 0x00, len);
        }
        else
        {
            WIRELESS_BUF_DEBUG(appid, MIN(24, len), "APPID\n");
        }
        hal_device_release((HW_DEVICE *)dev);
        return MIN(24, len);
    }
}

/**********************************************************************
* @name      : wireless_get_worktype
* @brief     ：获取APPID
* @param[in] ：uint8 len     缓存长度
* @param[out]：uint8 *appid  APPID
* @return    ：TRUE-模组管理器/FALSE-集中器方式
* @Create    : 王津剑
* @Date      ：2022-8-1
* @Update    : 
**********************************************************************/
uint8 wireless_get_worktype()
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        return FALSE;
    }
    else
    {
        ret = dev->devinfo_get_device_type(dev);
        hal_device_release((HW_DEVICE *)dev);
        return (ret & 0xF0000000) == 0x30000000 ? TRUE : FALSE;
    }
}

/**********************************************************************
* @name      : wireless_app_init
* @brief     ：无线远程拨号初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    :
**********************************************************************/
int wireless_app_init(void **ppPublicData)
{
    int           ret = 0;
    /* 检查程序只启动了一个副本 */
    if(!singleton_check("wirelessDCM"))
    {
        printf("%s is started!\n", "wirelessDCM");
        return -1;
    }

    /* 加载hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }

    /* 数据初始化 */
    wireless_data_init(&g_tWireData);

    /* 配置文件 */
    ret = wireless_init_config(&(g_tWireData.mqttip), &(g_tWireData.mqttport));
    if(0 != ret)
    {
        printf("wireless_init_config failed!\n");
        return -3;
    }

    /* mqtt初始化 */
    ret = wireless_mqtt_init(g_tWireData.mqttip, g_tWireData.mqttport);
    if(0 != ret)
    {
        printf("wireless_mqtt_init failed!\n");
        return -4;
    }

    /* 获取APPID */
    g_tWireData.appidlen = wireless_get_appid(g_tWireData.appid, 24);

    /* 获取终端类型 */
    g_tWireData.bMapExist = wireless_get_worktype();
    
    /* 全局锁 */
    wireless_lock_init();

    WIRELESS_FMT_DEBUG("wirelessDCM开始工作于%s, 版本: %s, 时间: %s\n", 
                       TRUE== g_tWireData.bMapExist ? "能源控制器" : "集中器", APP_VERSION, APP_PUBLISH_TIME);
    
    *ppPublicData = &g_tWireData;
    return 0;
}

/**********************************************************************
* @name      : wireless_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    : 王津剑
* @Date      ：2021-3-22
* @Update    :
**********************************************************************/
int wireless_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

/**********************************************************************
* @name      : wireless_diag_again
* @brief     ：重新拨号
* @param[in] ：uint8 id    设备ID
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 
* @Date      ：2020-5-3
* @Update    :
**********************************************************************/
int wireless_diag_again(uint8 id)
{
    if(1 != id && 2 != id)
    {
        return -1;
    }

    WIRELESS_FMT_DEBUG("显示执行设备%d重新拨号\n", id);

    if(1 == id)
    {
        if(WIRELESS_UNUSED_COMMPORT == g_tWireData.pppdata0.commport)
        {
            return -2;
        }
        else
        {
            /* 为了保证重拨顺利,重启设备 */
            wireless_restart_dev(0, g_tWireData.pppdata0.commport + ((g_tWireData.pppdata0.subport << 4) & 0xf0));
        }
    }
    else if(2 == id)
    {
        if(WIRELESS_UNUSED_COMMPORT == g_tWireData.pppdata1.commport)
        {
            return -2;
        }
        else
        {
            /* 为了保证重拨顺利,重启设备 */
            wireless_restart_dev(1, g_tWireData.pppdata1.commport + ((g_tWireData.pppdata1.subport << 4) & 0xf0));
        }
    }
    return 0;
}

/**********************************************************************
* @name      : wireless_app_stop
* @brief     ：无线拨号退出
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2020-6-24
* @Update    :
**********************************************************************/
void wireless_app_stop()
{
    wireless_set_pppstatus(0, 1);
    wireless_ppp_off(0);
    wireless_send_change(0, OOP_NET_UNKNOW, 0);
    g_tWireData.pppdata0.workAgain = TRUE;
    g_tWireData.atdata0.workAgain = TRUE;

    wireless_set_pppstatus(1, 1);
    wireless_ppp_off(1);
    wireless_send_change(1, OOP_NET_UNKNOW, 0);
    g_tWireData.pppdata1.workAgain = TRUE;
    g_tWireData.atdata1.workAgain = TRUE;


    /* 先退出桌面 */
    sleep(5);
//#if defined PRODUCT_CCU || defined PRODUCT_ECU || defined PRODUCT_ECU_G
    appGUI_send_req_handle(MESSAGE_IID_DESKGUI, DESKGUI_IOP_APP_UNREG, NULL, 0);
//#endif
    wireless_send_diagStatus(0, "无线拨号APP重启...", strlen("无线拨号APP重启..."), 11);
    WIRELESS_FMT_DEBUG("界面重启应用\n");
    
    system("/usr/local/extapps/wirelessDCM/bin/startup_app.sh -d");
    exit(0);
}

/**********************************************************************
* @name      : wireless_get_appid_gui
* @brief     ：显示获取appid
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 
* @Date      ：2021-3-23
* @Update    :
**********************************************************************/
void wireless_get_appid_gui(uint8 *buf, uint8 len)
{
    memcpy(buf, g_tWireData.appid, MIN(len, g_tWireData.appidlen));
}
