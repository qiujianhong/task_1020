/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监控初始化
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#ifndef PRODUCT_SCU
#include "hal.h"
#include "devinfo.h"
#endif
#include <arpa/inet.h>
#include "framecomm.h"
#include "iniparser.h"
#include "cJSON.h"
#include "task_msg.h"
#include "db_api.h"
#include "sec_comm.h"
#include "sec_mgr.h"
#ifndef PRODUCT_SCU
#include "hal.h"
#endif

extern int sec_mqtt_init(SEC_APPDATA_T *pData);
char  g_sec_datafile[256] = {0};

/**********************************************************************
* @name      : sec_paramcfg_proc
* @brief     ：参数配置文件处理
* @param[in] ：char *curpath   当前文件夹路径
               char *cfgpath   配置文件夹路径
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-8-6
* @Update    :
**********************************************************************/
int sec_datafile_deal(char *curpath, char *cfgpath)
{
    return 0;
}

/**********************************************************************
* @name      : sec_datafile_init
* @brief     ：模型初始化, 生效配置文件
* @param[in] ：char *datapath    配置文件路径
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2021-7-28
* @Update    :
**********************************************************************/
void sec_datafile_init(char *curpath, char *cfgpath)
{
    char   datafile[256] = {0};
    int    ret = 0;    
    
    /* 文件夹判断 */
    if(0 != access(curpath, F_OK))
    {
        if(0 != mkdir(curpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            INIT_FMT_DEBUG("%s创建失败\n", curpath);
            return;
        }
    }
    
    sprintf(datafile, "%s/%s", curpath, SEC_DATAFILE);
    
    /* 配置文件处理 */
    if(0 != access(datafile, F_OK))
    {
        INIT_FMT_DEBUG("%s不存在, %d\n", datafile, ret);
    }

    ret = sec_datafile_deal(curpath, cfgpath);
    if(0 != ret)
    {
        INIT_FMT_DEBUG("%s处理失败, %d\n", datafile, ret);
    }

    return;
}

/**********************************************************************
* @name      : init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-2-27
* @Update    : 
**********************************************************************/
int sec_init_config(uint32 *ip, uint16 *port)
{
    dictionary *ini = NULL;

    char  *intername = NULL;
    uint32 ipmask = 0xffffffff;
    uint32 ipaddr = 0;
    char  *stringvalue = NULL;
    int    intvalue = 0;

    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("wireless get err path!\n");
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
    
    sprintf(filename, "%s%s", curpath, SEC_INITFILE);
    
    /* 日志模块初始化 */
    diag_init(filename, TRUE);
    
    printf("filename is %s!\n", filename);
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        intername = iniparser_getstring(ini, "MQTT_CONF:INTERNAME", NULL);
        if(NULL != intername)
        {
            ipaddr = Get_Interface_IP(intername);
            if(0 != ipaddr)
            {
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IPMASK", "255.255.255.255");
                ipmask = inet_addr(stringvalue);
                ipmask = ntohl(ipmask);
                if(0xffffffff != ipmask)
                {
                    //通过掩码获取服务端地址
                    ipaddr = (ipaddr & ipmask) + 0x01;
                }
            }
            else
            {
                /* 无接口时用IP */
                stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "127.0.0.1");
                ipaddr = inet_addr(stringvalue);
                ipaddr = ntohl(ipaddr);
            }
        }
        else
        {
            /* INTERNAME为空用IP */
            stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "127.0.0.1");
            ipaddr = inet_addr(stringvalue);
            ipaddr = ntohl(ipaddr);
        }
        *ip = ipaddr;

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        *port = (uint16)intvalue;
        
        /* 获取ipc路径 */
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:IPCPATH", "/mnt/ipc_socket");
        task_msg_prepare(stringvalue);

        //获取数据文件路径
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:CFGPATH", "/data/app/"SEC_APP_NAME"/config");

        //数据文件获取
        sec_datafile_init(curpath, stringvalue);
    }
    else
    {
        printf("Get no ini file %s!\n", filename);

        *ip = 0x7f000001;
        *port = 1883;
        task_msg_prepare("/mnt/ipc_socket");
        return 0;
    }

    return 0;
}

/**********************************************************************
* @name      : sec_get_appid
* @brief     ：获取APPID
* @param[in] ：
* @param[out]：SEC_APPDATA_T *pData  进程共有数据
* @return    ：
* @Create    : 贺宁
* @Date      ：2020-7-19
* @Update    : 
**********************************************************************/
void sec_get_appid(SEC_APPDATA_T *pData)
{
    int ret = 0;
    DEVINFO_DEVICE_T* dev = (DEVINFO_DEVICE_T*) hal_device_get(HW_DEVICE_ID_DEVINFO);
    if(NULL == dev)
    {
        LCMONIT_FMT_DEBUG("devinfo hal_device_get error\n");
    }
    else
    {
        pData->appidlen = 24;
        ret = dev->devinfo_read_appid(dev, SEC_APP_NAME,(char*)pData->appid, SEC_APPID_MAX);
        if(ERR_OK != ret)
        {
            LCMONIT_FMT_DEBUG("devinfo devinfo_read_appid error, ret %d\n", ret);
        }
        else
        {
            LCMONIT_BUF_DEBUG(pData->appid, pData->appidlen, "APPID");
        }
        hal_device_release((HW_DEVICE *)dev);
    }
}

/**********************************************************************
* @name      : sec_app_init
* @brief     ：回路监控初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_app_init(void **ppPublicData)
{
    SEC_APPDATA_T *pData = NULL;
    int           ret = 0;
    /* 检查程序只启动了一个副本 */
    if(!singleton_check(SEC_APP_NAME))
    {
        printf("%s is started!\n", SEC_APP_NAME);
        return -1;
    }
    
#ifndef PRODUCT_SCU
    /* 加载hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }
#endif

    pData = malloc(sizeof(SEC_APPDATA_T));
    if(NULL == pData)
    {
        printf("malloc pbulic data failed!\n");
        return -3;
    }
    memset(pData, 0, sizeof(SEC_APPDATA_T));
    pData->appidlen = SEC_APPID_MAX;

    /* 配置文件 */
    ret = sec_init_config(&(pData->mqttip), &(pData->mqttport));
    if(0 != ret)
    {
        printf("sec_init_config failed!\n");
        free(pData);
        return -4;
    }

    /* mqtt初始化 */
    ret = sec_mqtt_init(pData);
    if(0 != ret)
    {
        printf("sec_mqtt_init failed!\n");
        free(pData);
        return -5;
    }

    /* 获取APPID */
    sec_get_appid(pData);

    *ppPublicData = pData;
    return 0;
}

/**********************************************************************
* @name      : sec_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int sec_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

