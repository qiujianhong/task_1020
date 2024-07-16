/*
*********************************************************************
* Copyright(C) 2020 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   回路监测主线程功能
* @date：    2020-6-30
* @history： 
*********************************************************************
*/
#include"iot_init.h"
#include"iot_public.h"

extern uint8 iot_get_appid(uint8 *appid, uint8 len);

int iot_init_config(WGPubattr *pData)
{
    dictionary *ini = NULL;

  //  char  *intername = NULL;
   // uint32 ipmask = 0xffffffff;
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
        printf("iot get err path!\n");
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
    
    sprintf(filename, "%s%s", curpath, WUGUAN_INITFILE);
    
    /* 日志模块初始化 */
    diag_init(filename, TRUE);
    
    printf("filename is %s!\n", filename);
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        /* ip获取 */
        stringvalue = iniparser_getstring(ini, "MQTT_CONF:IP", "172.17.0.1");
        ipaddr = inet_addr(stringvalue);
       pData->mqttip = ntohl(ipaddr);

        /* 端口获取 */
        intvalue = iniparser_getint(ini, "MQTT_CONF:PORT", 1883);
        pData->mqttport = (uint16)intvalue;

    }
    else
    {
        printf("Get no ini file %s!\n", filename);

        pData->mqttip = 0x7f000001;
        pData->mqttport = 1883;
        return 0;
    }

    return 0;
}


int iot_app_init(void **ppPublicData)
{
    int  ret = 0;
    WGPubattr *pData = NULL;
    /* 检查程序只启动了一个副本 */
    if(!singleton_check("iotManager"))
    {
        printf("%s is started!\n", "wuguan");
        return -1;
    }

    pData = malloc(sizeof(WGPubattr));
    if(NULL == pData)
    {
        printf("malloc pbulic data failed!\n");
        return -3;
    }
    memset(pData, 0, sizeof(WGPubattr));

    /* 加载hal */
    if(0 != hal_init())
    {
        printf("hal_init failed!\n");
        return -2;
    }

    /* 配置文件 */
    ret = iot_init_config(pData);
    if(0 != ret)
    {
        printf("iot_init_config failed!\n");
        free(pData);
        return -4;
    }
    //初始化队列
    if(pthread_mutex_init(&pData->msgque.QueueLock, NULL) != 0)
    {
        printf("init msgque.QueueLock error!\n");
        exit(1);
    }
    if(pthread_mutex_init(&pData->jsonque.QueueLock, NULL) != 0)
    {
        printf("init msgque.QueueLock error!\n");
        exit(1);
    }
    if(pthread_mutex_init(&pData->upque.QueueLock, NULL) != 0)
    {
        printf("init msgque.QueueLock error!\n");
        exit(1);
    }
    pData->appidlen  = iot_get_appid(pData->appid,24);
    
#if 0
    /* 配置文件 */
    ret = usb_init_config();
    if(0 != ret)
    {
        printf("usb_init_config failed!\n");
        return -2;
    }
#endif
    *ppPublicData = pData;

    return 0;
}


/**********************************************************************
* @name      : usb_version_print
* @brief     ：版本打印
* @param[in] ：uint32 buflen     缓存长度
* @param[out]：char *buf         输出日志
* @return    ：实际长度
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int iot_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: %s, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

