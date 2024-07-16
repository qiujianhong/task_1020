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
#include "task_msg.h"
#include "task_id.h"
#include "msg_id.h"
#include "cJSON.h"
#include "lc_comm.h"

extern int lc_mqtt_init(LC_APPDATA_T *pData);
extern char *g_APPCFGFILE;

/**********************************************************************
* @name      : init_config
* @brief     ：初始化配置文件
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-2-27
* @Update    : 
**********************************************************************/
int lc_init_config(uint32 *ip, uint16 *port)
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
        YCMSG_FMT_DEBUG("wireless get err path!");
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
    sprintf(filename, "%s%s", curpath, LC_INITFILE);
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
		
        iniparser_freedict(ini);
    }
    else
    {
        printf("Get no ini file %s!\n", filename);

        *ip = 0x7f000001;
        *port = 1883;
        task_msg_prepare("/mnt/ipc_socket");
        return 0;
    }

    /* 日志模块初始化 */
    diag_init(filename, TRUE);

    return 0;
}

/**********************************************************************
* @name      : lc_get_appid
* @brief     ：获取APPID
* @param[in] ：
* @param[out]：LC_APPDATA_T *pData  进程共有数据
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-7-19
* @Update    : 
**********************************************************************/
void lc_get_appid(LC_APPDATA_T *pData)
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
        ret = dev->devinfo_read_appid(dev, LC_APP_NAME,(char*)pData->appid, LC_APPID_MAX);
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
* @name      : lc_cfgfile_init
* @brief     ：回路巡检配置文件初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-11-04
* @Update    :
**********************************************************************/
int lc_cfgfile_init(void *pPublic)
{
    int ret = 0;
    LC_APPDATA_T *pData = (LC_APPDATA_T*)pPublic;
    cJSON  *json = NULL;
    cJSON  *item = NULL;
    cJSON  *subitem = NULL;

    json = cJSON_read(g_APPCFGFILE);
    if(!cJSON_IsObject(json))
    {
        INIT_FMT_DEBUG("cJSON_read error, no User config: %s.\n", g_APPCFGFILE);
        ret = ERR_PNULL;
        goto end;
    }

    item = cJSON_GetObjectItem(json, "type");
    if(!cJSON_IsString(item))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no item.\n");
        goto end;
    }

    if(strcmp(item->valuestring, "new") == 0)
    {
        pData->appType = LC_APP_NEW;
    }

    item = cJSON_GetObjectItem(json, "devname");
    if(!cJSON_IsObject(item))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no item.\n");
        goto end;
    }

    subitem = cJSON_GetObjectItem(item, "manager");
    if(!cJSON_IsString(subitem))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no item.\n");
        goto end;
    }

    if(subitem->valuestring != NULL)
    {
        strcpy(pData->devName[LC_CCH_MANAGER], subitem->valuestring);
    }

    subitem = cJSON_GetObjectItem(item, "data");
    if(!cJSON_IsString(subitem))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no item.\n");
        goto end;
    }

    if(subitem->valuestring != NULL)
    {
        strcpy(pData->devName[LC_CCH_DATACOM], subitem->valuestring);
    }

end:
    cJSON_Delete(json);  
    return ret;    
}

/**********************************************************************
* @name      : lc_app_init
* @brief     ：回路监控初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_app_init(void **ppPublicData)
{
    LC_APPDATA_T *pData = NULL;
    int           ret = 0;
    /* 检查程序只启动了一个副本 */
    if(!singleton_check(LC_APP_NAME))
    {
        printf("%s is started!\n", LC_APP_NAME);
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

    pData = malloc(sizeof(LC_APPDATA_T));
    if(NULL == pData)
    {
        printf("malloc pbulic data failed!\n");
        return -3;
    }
    memset(pData, 0, sizeof(LC_APPDATA_T));
    pData->appidlen = 24;

    /* 配置文件 */
    ret = lc_init_config(&(pData->mqttip), &(pData->mqttport));
    if(0 != ret)
    {
        printf("lc_init_config failed!\n");
        free(pData);
        return -4;
    }
    INIT_FMT_DEBUG("Version: 0x%x Time: %s Build: %s %s\n", APP_VERSION, APP_PUBLISH_TIME, __DATE__, __TIME__);

    ret = lc_cfgfile_init(pData);
    if(ret != 0)
    {
        pData->appType = LC_APP_OLD;
        strcpy(pData->devName[LC_CCH_MANAGER], "/dev/ttyYMHL0");
        strcpy(pData->devName[LC_CCH_DATACOM], "/dev/ttyYMHL2");
    }

    /* mqtt初始化 */
    ret = lc_mqtt_init(pData);
    if(0 != ret)
    {
        printf("lc_mqtt_init failed!\n");
        free(pData);
        return -5;
    }

    /* 获取APPID */
    lc_get_appid(pData);

    *ppPublicData = pData;
    return 0;
}

/**********************************************************************
* @name      : lc_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    : 王津剑
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int lc_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

