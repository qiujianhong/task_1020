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
#include "framecomm.h"
#include "iniparser.h"
#include "cJSON.h"
#include "usb_init.h"

char g_usb_datafile[256] = {0};

/**********************************************************************
* @name      : usb_datafile_create
* @brief     ：创建数据文件
* @param[in] ：char *filename    配置文件名称
* @param[out]：
* @return    ：0-成功/失败
* @Create    : 贺宁
* @Date      ：2021-7-28
* @Update    :
**********************************************************************/
int usb_datafile_create(char *filename)
{
    int     ret = 0;

    return ret;
}

/**********************************************************************
* @name      : usb_paramcfg_proc
* @brief     ：参数配置文件处理
* @param[in] ：char *curpath   当前文件夹路径
               char *cfgpath   配置文件夹路径
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2021-8-6
* @Update    :
**********************************************************************/
int usb_datafile_deal(void *pPublic, char *curpath, char *cfgpath)
{
    cJSON  *json = NULL;
    cJSON  *jList = NULL;
    cJSON  *jArray = NULL;
    cJSON  *item = NULL;
    cJSON  *jAPPList = NULL;
    cJSON  *jAPPArray = NULL;    
    int     i = 0, j = 0;
    USB_APPDATA_T *pAppData = (USB_APPDATA_T *)pPublic;


    /* 判断是否需要文件拷贝 */
    if(0 != access(cfgpath, F_OK))
    {
        if(0 != mkdir(cfgpath, 0755))
        {
            INIT_FMT_DEBUG("%s create failed!", cfgpath);
        }
    }

    /* 读取参数 */
    sprintf(g_usb_datafile, "%s/"USB_DATAFILE, cfgpath);

    /* 判断是否需要文件拷贝 */
    if(0 != access(g_usb_datafile, F_OK))
    {
        char cmd[512] = {0};
        sprintf(cmd, "cp -r %s/"USB_DATAFILE" %s", curpath, cfgpath);
        system(cmd);
        INIT_FMT_DEBUG("%s\n", cmd);
    }
    
    json = cJSON_read(g_usb_datafile);
    if(!cJSON_IsObject(json))
    {
        INIT_FMT_DEBUG("%s cJSON_Parse error, no User config.\n", g_usb_datafile);
        return ERR_PNULL;
    }

    jList = cJSON_GetObjectItem(json, "devs");
    if(!cJSON_IsArray(jList))
    {
        INIT_FMT_DEBUG("cJSON_Parse error, no list item.\n");
        goto end;
    }

    pAppData->devNum = cJSON_GetArraySize(jList);
    pAppData->devices = (USB_DEVICES_T *)malloc(sizeof(USB_DEVICES_T)*pAppData->devNum);
    
    for(i = 0; i < pAppData->devNum; i++)
    {
        jArray = cJSON_GetArrayItem(jList, i);
        if(!cJSON_IsObject(jArray))
        {
            break;
        }

        //设备名
        item = cJSON_GetObjectItem(jArray, "name");
        if(!cJSON_IsString(item))
        {
            break;
        }

        strcpy(pAppData->devices[i].name, item->valuestring);

        //设备类型
        item = cJSON_GetObjectItem(jArray, "type");
        if(!cJSON_IsString(item))
        {
            break;
        }

        strcpy(pAppData->devices[i].type, item->valuestring);

        //设备使能
        item = cJSON_GetObjectItem(jArray, "enable");
        if(!cJSON_IsNumber(item))
        {
            break;
        }
        
        pAppData->devices[i].enable = item->valueint;

        //设备路径信息
        item = cJSON_GetObjectItem(jArray, "path");
        if(!cJSON_IsString(item))
        {
            break;
        }

        strcpy(pAppData->devices[i].path, item->valuestring);
        
        
        //设备添加信息
        item = cJSON_GetObjectItem(jArray, "add");
        if(!cJSON_IsString(item))
        {
            break;
        }

        strcpy(pAppData->devices[i].add, item->valuestring);

        //设备移除信息
        item = cJSON_GetObjectItem(jArray, "remove");
        if(!cJSON_IsString(item))
        {
            break;
        }
        
        strcpy(pAppData->devices[i].remove, item->valuestring);

        jAPPList = cJSON_GetObjectItem(jArray, "link");
        if(!cJSON_IsArray(jAPPList))
        {
            break;
        }
            pAppData->devices[i].linkerNum = cJSON_GetArraySize(jAPPList);
            pAppData->devices[i].linker = (char (*)[32])malloc(pAppData->devices[i].linkerNum*32);
            
            for(j = 0; j < pAppData->devices[i].linkerNum; j++)
            {
                jAPPArray = cJSON_GetArrayItem(jAPPList, j);
                if(!cJSON_IsString(jAPPArray))
                {
                    break;
                }

                strcpy(pAppData->devices[i].linker[j], jAPPArray->valuestring);
            }
    }
end:    
    cJSON_Delete(json);  
   
    return 0;
}


/**********************************************************************
* @name      : usb_datafile_init
* @brief     ：模型初始化, 生效配置文件
* @param[in] ：char *datapath    配置文件路径
* @param[out]：
* @return    ：
* @Create    : 贺宁
* @Date      ：2021-7-28
* @Update    :
**********************************************************************/
void usb_datafile_init(void *pPublic, char *curpath, char *cfgpath)
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
    
    sprintf(datafile, "%s/%s", curpath, USB_DATAFILE);
    
    /* 配置文件处理 */
    if(0 != access(datafile, F_OK))
    {
        /* 不存在配置文件, 创建一个新的 */
        return;
    }

    ret = usb_datafile_deal(pPublic, curpath, cfgpath);
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
int usb_init_config(void *pPublic)
{
    dictionary *ini = NULL;
    char  *stringvalue = NULL;
    char   curpath[256] = {0};
    char   filename[256] = {0};
    int i;

    //获取当前目录绝对路径，即去掉程序名
    int cnt = readlink("/proc/self/exe", curpath, 256);
    if(cnt < 0 || cnt >= 256)
    {
        printf("usbManager get err path!\n");
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
    
    sprintf(filename, "%s%s", curpath, USB_INITFILE);
    
    /* 日志模块初始化 */
    diag_init(filename, TRUE);
    
    printf("filename is %s!\n", filename);
    ini = iniparser_load(filename);
    if(NULL != ini)
    {
        //获取数据文件路径
        stringvalue = iniparser_getstring(ini, "FILEPATH_CONF:CFGPATH", "/data/app/"USB_APP_NAME"/config");

        //数据文件获取
        usb_datafile_init(pPublic, curpath, stringvalue);
    }
    else
    {
        printf("Get no ini file %s!\n", filename);
    }

    return 0;
}

/**********************************************************************
* @name      : usb_app_init
* @brief     ：回路监控初始化
* @param[in] ：
* @param[out]：void **ppPublicData
* @return    ：0-成功/错误码
* @Create    : 贺宁
* @Date      ：2020-6-30
* @Update    :
**********************************************************************/
int usb_app_init(void **ppPublicData)
{
    int           ret = 0;
    USB_APPDATA_T *pData = NULL;
    
    /* 检查程序只启动了一个副本 */
    if(!singleton_check(USB_APP_NAME))
    {
        printf("%s is started!\n", USB_APP_NAME);
        return -1;
    }

    pData = malloc(sizeof(USB_APPDATA_T));
    if(NULL == pData)
    {
        printf("malloc pbulic data failed!\n");
        return -3;
    }
    memset(pData, 0, sizeof(USB_APPDATA_T));
    

    /* 配置文件 */
    ret = usb_init_config(pData);
    if(0 != ret)
    {
        printf("usb_init_config failed!\n");
        return -2;
    }

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
int usb_version_print(char *buf, uint32 buflen)
{
    return snprintf(buf, buflen, "Version: 0x%x, Time: %s", APP_VERSION, APP_PUBLISH_TIME);
}

