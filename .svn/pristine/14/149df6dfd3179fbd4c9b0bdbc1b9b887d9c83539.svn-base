/*
*********************************************************************
* @filename: system_msg.c
* @brief   : 系统管理器消息接口
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2021-7-19     create
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_split.h"
#include "cJSON.h"

extern void FastSecondToDate(struct timeval* tv, struct timezone* tz, struct tm* tm);
extern int db_base64_to_data(char *pBase, uint8 *pBuf, uint32 buflen, uint32 *datalen);
extern char *db_data_to_base64(uint8 *pData, uint32 datalen);

/**********************************************************************
* @name      : sys_string_time_struct
* @brief     ：转换成时间
* @param[in] ：char *timestr       字符串
* @param[out]：Date_Time *pTime    时间
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    : 
**********************************************************************/
int sys_string_time_struct(char *timestr, Date_Time *pTime)
{
    int a[6] = {0};
    int d, m, y;
    
    if(NULL == timestr || NULL == pTime)
    {
        return -1;
    }
    sscanf(timestr, "%d-%d-%d %d:%d:%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]), &(a[4]),&(a[5]));
    pTime->year  = a[0];
    pTime->month = a[1];
    pTime->day   = a[2];
    pTime->hour  = a[3];
    pTime->min   = a[4];
    pTime->sec   = a[5];
    pTime->milsec = 0;

    d= pTime->day;
    m = pTime->month;
    y = pTime->year;
    pTime->week = ((d + 2*m + 3*(m+1)/5 + y + y/4 -y/100 + y/400) + 1) % 7;
    return 0;
}

/**********************************************************************
* @name      : sys_keepAlive_split
* @brief     ：应用保活信息转换
* @param[in] ：char *infopath             信息对象
               void *pJson                JSON
* @param[out]：uint32 *token              序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/ 
int sys_keepAlive_split(char *infopath, void *pJson, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_KEEPALIVE, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    return 0;
}

/**********************************************************************
* @name      : sys_getreglist_split
* @brief     ：APP注册列表信息转换
* @param[in] ：char *infopath             信息对象
               void *pJson                JSON
* @param[out]：SYS_APPLIST_T *pAppList    APP列表
               uint32 *token              序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/ 
int sys_getreglist_split(char *infopath, void *pJson, SYS_APPLIST_T *pAppList, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jList = NULL;
    cJSON *jTemp = NULL;
    uint32 i = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pAppList || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETREGLIST, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pAppList, 0, sizeof(SYS_APPLIST_T));
    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jList = cJSON_GetObjectItem(jBody, "list");
    if(NULL == jList || FALSE == cJSON_IsArray(jList))
    {
        return -6;
    }
    pAppList->num = cJSON_GetArraySize(jList);
    pAppList->num = MIN(pAppList->num, SYS_APPLIST_MAX);
    for(i = 0; i < pAppList->num; i++)
    {
        jTemp = cJSON_GetArrayItem(jList, i);
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pAppList->name[i].num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pAppList->name[i].value, jTemp->valuestring, pAppList->name[i].num);
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : sys_getregapp_split
* @brief     ：注册APP信息转换
* @param[in] ：char *infopath             信息对象
               void *pJson                JSON
* @param[out]：SYS_APPREG_T *pApp         APP信息
               uint32 *token              序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/ 
int sys_getregapp_split(char *infopath, void *pJson, SYS_APPREG_T *pApp, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == pApp || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETREGAPP, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pApp, 0, sizeof(SYS_APPREG_T));
    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(jBody, "name");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pApp->name.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pApp->name.value, jTemp->valuestring, pApp->name.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "version");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pApp->version = jTemp->valueint;
    }

    jTemp = cJSON_GetObjectItem(jBody, "releaseDate");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sys_string_time_struct(jTemp->valuestring, &pApp->releaseDate);
    }
    
    return 0;
}

/**********************************************************************
* @name      : sys_devinfo_split
* @brief     ：设备信息转换
* @param[in] ：char *infopath             信息对象
               void *pJson                JSON
* @param[out]：SYS_DEVINFO_T *pDevInfo    设备信息
               uint32 *token              序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/ 
int sys_devinfo_split(char *infopath, void *pJson, SYS_DEVINFO_T *pDevInfo, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == pDevInfo || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_DEVINFO, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pDevInfo, 0, sizeof(SYS_DEVINFO_T));
    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(jBody, "vendorId");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->vendorId.num = MIN(strlen(jTemp->valuestring), 255);
        memcpy(pDevInfo->vendorId.value, jTemp->valuestring, pDevInfo->vendorId.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "devSn");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->devSn.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->devSn.value, jTemp->valuestring, pDevInfo->devSn.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "esn");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->ESN.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->ESN.value, jTemp->valuestring, pDevInfo->ESN.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "osName");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->osName.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->osName.value, jTemp->valuestring, pDevInfo->osName.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "osVer");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->osVer.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->osVer.value, jTemp->valuestring, pDevInfo->osVer.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "osPatchVer");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->osPatchVer.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->osPatchVer.value, jTemp->valuestring, pDevInfo->osPatchVer.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "osKernelVer");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->osKernelVer.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->osKernelVer.value, jTemp->valuestring, pDevInfo->osKernelVer.num);
    }
    
    jTemp = cJSON_GetObjectItem(jBody, "hardwareModel");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->hardwareModel.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->hardwareModel.value, jTemp->valuestring, pDevInfo->hardwareModel.num);
    }
    
    jTemp = cJSON_GetObjectItem(jBody, "hardwareVersion");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->hardwareVersion.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->hardwareVersion.value, jTemp->valuestring, pDevInfo->hardwareVersion.num);
    }
    
    jTemp = cJSON_GetObjectItem(jBody, "manufDate");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->manufDate.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->manufDate.value, jTemp->valuestring, pDevInfo->manufDate.num);
    }
    jTemp = cJSON_GetObjectItem(jBody, "cpuFreq");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevInfo->cpuFreq = jTemp->valueint;
    }

    jTemp = cJSON_GetObjectItem(jBody, "cpuCoreNumber");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevInfo->cpuCoreNumber = jTemp->valueint;
    }

    jTemp = cJSON_GetObjectItem(jBody, "cpuArch");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pDevInfo->cpuArch.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pDevInfo->cpuArch.value, jTemp->valuestring, pDevInfo->cpuArch.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "cpuCache");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevInfo->cpuCache = jTemp->valueint;
    }

    jTemp = cJSON_GetObjectItem(jBody, "diskSize");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevInfo->diskSize = jTemp->valueint;
    }

    jTemp = cJSON_GetObjectItem(jBody, "memSize");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevInfo->memSize = jTemp->valueint;
    }

    jTemp = cJSON_GetObjectItem(jBody, "virtMemSize");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevInfo->virtMemSize = jTemp->valueint;
    }
    
    return 0;
}

/**********************************************************************
* @name      : sys_devstatus_split
* @brief     ：设备状态转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_DEVSTATUS_T *pDevStatus 设备状态
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/ 
int sys_devstatus_split(char *infopath, void *pJson, SYS_DEVSTATUS_T *pDevStatus, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    cJSON *jDisk = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == pDevStatus || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_DEVSATAUS, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pDevStatus, 0, sizeof(SYS_DEVSTATUS_T));

    /*jTemp = cJSON_GetObjectItem(pCJson, "statusDesc");
    if(NULL == jTemp || NULL == jTemp->valuestring || 0 != strcmp(jTemp->valuestring, "OK"))
    {
        return -4;
    }*/

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(jBody, "cpuUsage");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevStatus->cpuUsage = jTemp->valuedouble;
    }

    jTemp = cJSON_GetObjectItem(jBody, "memUsage");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevStatus->memUsage = jTemp->valuedouble;
    }

    jTemp = cJSON_GetObjectItem(jBody, "cpuTemperature");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevStatus->cpuTemperature = jTemp->valuedouble;
    }

    jDisk = cJSON_GetObjectItem(jBody, "diskUsage");
    if(NULL == jDisk || FALSE == cJSON_IsObject(jDisk))
    {
        return -6;
    }
    jTemp = cJSON_GetObjectItem(jDisk, "allDisk");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevStatus->allDisk = jTemp->valuedouble;
    }
    jTemp = cJSON_GetObjectItem(jDisk, "sysDisk");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevStatus->sysDisk = jTemp->valuedouble;
    }
    jTemp = cJSON_GetObjectItem(jDisk, "containerDisk");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevStatus->containerDisk = jTemp->valuedouble;
    }
    jTemp = cJSON_GetObjectItem(jDisk, "dataDisk");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevStatus->dataDisk = jTemp->valuedouble;
    }

    jTemp = cJSON_GetObjectItem(jBody, "devSartTime");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sys_string_time_struct(jTemp->valuestring, &pDevStatus->devSartTime);
    }

    jTemp = cJSON_GetObjectItem(jBody, "devRunTime");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pDevStatus->devRunTime = jTemp->valueint;
    }
    
    return 0;
}

/**********************************************************************
* @name      : sys_commstatus_split
* @brief     ：通用状态转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：int *status                 状态
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/ 
int sys_commstatus_split(char *infopath, void *pJson, int *status, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == status || NULL == token)
    {
        return -1;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -2;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }

    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        *status = jTemp->valueint;
        return 0;
    }

    if(0 == strcmp(SYS_TOPIC_POWERSATAUS, infopath))
    {
        jBody = cJSON_GetObjectItem(pCJson, "body");
        if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
        {
            return -4;
        }

        jTemp = cJSON_GetObjectItem(jBody, "status");
        if(NULL == jTemp || cJSON_Number != jTemp->type)
        {
            return -5;
        }
    }
    else if(0 == strcmp(SYS_TOPIC_ETHSTATUS, infopath))
    {
        jBody = cJSON_GetObjectItem(pCJson, "body");
        if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
        {
            return -4;
        }

        jTemp = cJSON_GetObjectItem(jBody, "ethStatus");
        if(NULL == jTemp || cJSON_Number != jTemp->type)
        {
            return -5;
        }
    }
    else if(0 == strcmp(SYS_TOPIC_REGISTER, infopath) || 0 == strcmp(SYS_TOPIC_UNREGISTER, infopath) || 
            0 == strcmp(SYS_TOPIC_SETETH, infopath) || 0 == strcmp(SYS_TOPIC_SETTIME, infopath) ||
            0 == strcmp(SYS_TOPIC_ADDROUTE, infopath) || 0 == strcmp(SYS_TOPIC_DELROUTE, infopath) ||
            0 == strcmp(SYS_TOPIC_SETSNTP, infopath) || 0 == strcmp(SYS_TOPIC_REBOOT, infopath) )
    {
        *status = 0;
        return 0;
    }
    else 
    {
        return -2;
    }
    
    if(NULL != jTemp)
    {
        *status = jTemp->valueint;
    }
    return 0;
}

/**********************************************************************
* @name      : sys_ethcfg_split
* @brief     ：以太网络配置转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_ETHCFG_T *pETHCfg       以太网络配置
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/ 
int sys_ethcfg_split(char *infopath, void *pJson, SYS_ETHCFG_T *pETHCfg, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    uint8  temp = 0;
    char   tempH = 0;
    char   tempL = 0;
    uint8  i = 0;
    int    a[6] = {0};
    char  *cTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == pETHCfg || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETETH, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -4;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -3;
    }

    memset(pETHCfg, 0, sizeof(SYS_ETHCFG_T));
    
    /*jTemp = cJSON_GetObjectItem(pCJson, "statusDesc");
    if(NULL == jTemp || NULL == jTemp->valuestring || 0 != strcmp(jTemp->valuestring, "OK"))
    {
        return -4;
    }*/

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(jBody, "ifName");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        memcpy(pETHCfg->ifname, jTemp->valuestring, MIN(23, strlen(jTemp->valuestring)));
    }

    jTemp = cJSON_GetObjectItem(jBody, "mode");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pETHCfg->mode = jTemp->valueint;
    }

    jTemp = cJSON_GetObjectItem(jBody, "ip");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sscanf(jTemp->valuestring, "%d.%d.%d.%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]));
        pETHCfg->ip[0] = a[0];
        pETHCfg->ip[1] = a[1];
        pETHCfg->ip[2] = a[2];
        pETHCfg->ip[3] = a[3];
    }

    jTemp = cJSON_GetObjectItem(jBody, "netMask");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sscanf(jTemp->valuestring, "%d.%d.%d.%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]));
        pETHCfg->netMask[0] = a[0];
        pETHCfg->netMask[1] = a[1];
        pETHCfg->netMask[2] = a[2];
        pETHCfg->netMask[3] = a[3];
    }

    jTemp = cJSON_GetObjectItem(jBody, "gateway");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sscanf(jTemp->valuestring, "%d.%d.%d.%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]));
        pETHCfg->gateway[0] = a[0];
        pETHCfg->gateway[1] = a[1];
        pETHCfg->gateway[2] = a[2];
        pETHCfg->gateway[3] = a[3];
    }

    jTemp = cJSON_GetObjectItem(jBody, "mac");
    if(NULL != jTemp && NULL != jTemp->valuestring && 17 == strlen(jTemp->valuestring))
    {
        cTemp = jTemp->valuestring;
        for(i = 0; i < 6; i++)
        {
            tempH = *cTemp;
            cTemp++;
            tempL = *cTemp;
            cTemp++;
            cTemp++;

            temp = 0;
            if('0' <= tempH && tempH <= '9')
            {
                temp = tempH - '0';
            }
            else if('A' <= tempH && tempH <= 'F')
            {
                temp = tempH - 'A' + 10;
            }
            else if('a' <= tempH && tempH <= 'f')
            {
                temp = tempH - 'a' + 10;
            }
            else
            {
                return -6;
            }
            temp = (temp << 4);

            if('0' <= tempL && tempL <= '9')
            {
                temp += tempL - '0';
            }
            else if('A' <= tempL && tempL <= 'F')
            {
                temp += tempL - 'A' + 10;
            }
            else if('a' <= tempL && tempL <= 'f')
            {
                temp += tempL - 'a' + 10;
            }
            else
            {
                return -6;
            }
            pETHCfg->mac[i] = temp;
        }
    }
    
    return 0;
}

/**********************************************************************
* @name      : sys_appinfo_split
* @brief     ：应用信息转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_APPINFO_T *pAppInfo     APP信息
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/ 
int sys_appinfo_split(char *infopath, void *pJson, SYS_APPINFO_T *pAppInfo, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == pAppInfo || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETAPPINFO, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pAppInfo, 0, sizeof(SYS_APPINFO_T));

    /*jTemp = cJSON_GetObjectItem(pCJson, "statusDesc");
    if(NULL == jTemp || NULL == jTemp->valuestring || 0 != strcmp(jTemp->valuestring, "OK"))
    {
        return -4;
    }*/

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(jBody, "appName");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pAppInfo->appName.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pAppInfo->appName.value, jTemp->valuestring, pAppInfo->appName.num);
    }
    
    jTemp = cJSON_GetObjectItem(jBody, "container");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pAppInfo->container.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pAppInfo->container.value, jTemp->valuestring, pAppInfo->container.num);
    }

    jTemp = cJSON_GetObjectItem(jBody, "appVer");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        pAppInfo->appVer.num = MIN(strlen(jTemp->valuestring), 63);
        memcpy(pAppInfo->appVer.value, jTemp->valuestring, pAppInfo->appVer.num);
    }

    return 0;
}

/**********************************************************************
* @name      : sys_container_split
* @brief     ：查询容器信息列表转换
* @param[in] ：char *infopath                   信息对象
               void *pJson                      JSON
* @param[out]：SYS_CONTAINER_LIST_T *pContainer 容器列表
               uint32 *token                    序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-11
* @Update    :
**********************************************************************/ 
int sys_container_split(char *infopath, void *pJson, SYS_CONTAINER_LIST_T *pContainer, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jList = NULL;
    cJSON *jContainer = NULL;
    cJSON *jTemp = NULL;
    int    num = 0;
    int    i = 0;
    int    a[4] = {0};
    
    if(NULL == infopath || NULL == pJson || NULL == pContainer || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETCONTLIST, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pContainer, 0, sizeof(SYS_CONTAINER_LIST_T));

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jList = cJSON_GetObjectItem(jBody, "containerList");
    if(NULL == jList || FALSE == cJSON_IsArray(jList))
    {
        return -5;
    }
    num = cJSON_GetArraySize(jList);
    pContainer->num = MIN(num, SYS_CONATINER_MAX);
    
    for(i = 0; i < pContainer->num; i++)
    {
        jContainer = cJSON_GetArrayItem(jList, i);
        if(NULL == jContainer || FALSE == cJSON_IsObject(jContainer))
        {
            continue;
        }
        
        jTemp = cJSON_GetObjectItem(jBody, "id");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].id.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pContainer->list[i].id.value, jTemp->valuestring, pContainer->list[i].id.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "name");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].name.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pContainer->list[i].name.value, jTemp->valuestring, pContainer->list[i].name.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "status");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].status.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pContainer->list[i].status.value, jTemp->valuestring, pContainer->list[i].status.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "cpu_num");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pContainer->list[i].cpu_num = jTemp->valueint;
        }
        jTemp = cJSON_GetObjectItem(jBody, "mem_limit");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].mem_limit.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pContainer->list[i].mem_limit.value, jTemp->valuestring, pContainer->list[i].mem_limit.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "disk_limit");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].disk_limit.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pContainer->list[i].disk_limit.value, jTemp->valuestring, pContainer->list[i].disk_limit.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "mount_dir");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].mount_dir.num = MIN(strlen(jTemp->valuestring),255);
            memcpy(pContainer->list[i].mount_dir.value, jTemp->valuestring, pContainer->list[i].mount_dir.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "dev");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].dev.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pContainer->list[i].dev.value, jTemp->valuestring, pContainer->list[i].dev.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "cpu_usage");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pContainer->list[i].cpu_usage = jTemp->valuedouble;
        }
        jTemp = cJSON_GetObjectItem(jBody, "mem_usage");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pContainer->list[i].mem_usage = jTemp->valuedouble;
        }
        jTemp = cJSON_GetObjectItem(jBody, "disk_usage");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pContainer->list[i].disk_usage = jTemp->valuedouble;
        }
        jTemp = cJSON_GetObjectItem(jBody, "ip");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            sscanf(jTemp->valuestring, "%d.%d.%d.%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]));
            pContainer->list[i].ip[0] = a[0];
            pContainer->list[i].ip[1] = a[1];
            pContainer->list[i].ip[2] = a[2];
            pContainer->list[i].ip[3] = a[3];
        }
        jTemp = cJSON_GetObjectItem(jBody, "img_name");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].img_name.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pContainer->list[i].img_name.value, jTemp->valuestring, pContainer->list[i].img_name.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "create_time");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            sys_string_time_struct(jTemp->valuestring, &(pContainer->list[i].create_time));
        }
        jTemp = cJSON_GetObjectItem(jBody, "start_time");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            sys_string_time_struct(jTemp->valuestring, &(pContainer->list[i].start_time));
        }
        jTemp = cJSON_GetObjectItem(jBody, "run_time");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pContainer->list[i].run_time = jTemp->valueint;
        }
        jTemp = cJSON_GetObjectItem(jBody, "app_path");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pContainer->list[i].app_path.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pContainer->list[i].app_path.value, jTemp->valuestring, pContainer->list[i].app_path.num);
        }
    }

    return 0;
}

/**********************************************************************
* @name      : sys_containerApp_split
* @brief     ：容器内APP信息列表转换
* @param[in] ：char *infopath                   信息对象
               void *pJson                      JSON
* @param[out]：SYS_CONTAPP_LIST_T *pAppList     应用信息
               uint32 *token                    序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-8-11
* @Update    :
**********************************************************************/ 
int sys_containerApp_split(char *infopath, void *pJson, SYS_CONTAPP_LIST_T *pAppList, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jList = NULL;
    cJSON *jApp = NULL;
    cJSON *jTemp = NULL;
    int    num = 0;
    int    i = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pAppList || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETAPPINFOLIST, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pAppList, 0, sizeof(SYS_CONTAPP_LIST_T));

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jList = cJSON_GetObjectItem(jBody, "appList");
    if(NULL == jList || FALSE == cJSON_IsArray(jList))
    {
        return -5;
    }
    num = cJSON_GetArraySize(jList);
    pAppList->num = MIN(num, SYS_CONATINERAPP_MAX);
    
    for(i = 0; i < pAppList->num; i++)
    {
        jApp = cJSON_GetArrayItem(jList, i);
        if(NULL == jApp || FALSE == cJSON_IsObject(jApp))
        {
            continue;
        }

        jTemp = cJSON_GetObjectItem(jBody, "appId");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pAppList->list[i].appId = jTemp->valueint;
        }
        
        jTemp = cJSON_GetObjectItem(jBody, "appName");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pAppList->list[i].appName.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pAppList->list[i].appName.value, jTemp->valuestring, pAppList->list[i].appName.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "container");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pAppList->list[i].container.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pAppList->list[i].container.value, jTemp->valuestring, pAppList->list[i].container.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "appVer");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pAppList->list[i].appVer.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pAppList->list[i].appVer.value, jTemp->valuestring, pAppList->list[i].appVer.num);
        }
        jTemp = cJSON_GetObjectItem(jBody, "appHash");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pAppList->list[i].appHash.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pAppList->list[i].appHash.value, jTemp->valuestring, pAppList->list[i].appHash.num);
        }

        jTemp = cJSON_GetObjectItem(jBody, "isEnable");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pAppList->list[i].isEnable = jTemp->valueint;
        }
        jTemp = cJSON_GetObjectItem(jBody, "appStatus");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pAppList->list[i].appStatus = jTemp->valueint;
        }
        jTemp = cJSON_GetObjectItem(jBody, "appCpuUsageLmt");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pAppList->list[i].appCpuUsageLmt = jTemp->valuedouble;
        }
        jTemp = cJSON_GetObjectItem(jBody, "appCpuUsage");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pAppList->list[i].appCpuUsage = jTemp->valuedouble;
        }
        jTemp = cJSON_GetObjectItem(jBody, "appMemUsage");
        if(NULL != jTemp && cJSON_Number == jTemp->type)
        {
            pAppList->list[i].appMemUsage = jTemp->valuedouble;
        }

        jTemp = cJSON_GetObjectItem(jBody, "appStartTime");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            sys_string_time_struct(jTemp->valuestring, &(pAppList->list[i].appStartTime));
        }
    }

    return 0;
}

/**********************************************************************
* @name      : sys_appid_split
* @brief     ：应用信息转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_APPSID_T *pAppID        APPID
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-21
* @Update    :
**********************************************************************/ 
int sys_appid_split(char *infopath, void *pJson, SYS_APPSID_T *pAppID, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jList = NULL;
    cJSON *jObject = NULL;
    cJSON *jTemp = NULL;
    int    i = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pAppID || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETAPPID, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pAppID, 0, sizeof(SYS_APPID_T));
    
    /*jTemp = cJSON_GetObjectItem(pCJson, "statusDesc");
    if(NULL == jTemp || NULL == jTemp->valuestring || 0 != strcmp(jTemp->valuestring, "OK"))
    {
        return -4;
    }*/

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jList = cJSON_GetObjectItem(jBody, "appList");
    if(NULL == jBody || FALSE == cJSON_IsArray(jList))
    {
        return -6;
    }
    pAppID->num = MIN(SYS_APPLIST_MAX, cJSON_GetArraySize(jList));
    for(i = 0; i < pAppID->num; i++)
    {
        jObject = cJSON_GetArrayItem(jList, i);
        if(NULL == jObject)
        {
            continue;
        }
        
        jTemp = cJSON_GetObjectItem(jObject, "appName");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pAppID->app[i].appName.num = MIN(strlen(jTemp->valuestring), 63);
            memcpy(pAppID->app[i].appName.value, jTemp->valuestring, pAppID->app[i].appName.num);
        }
        
        jTemp = cJSON_GetObjectItem(jObject, "appId");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            /* APP_ID信息，二进制数据经过Base64编码后获得的字符串。空字符串表示不存在对应的APP-ID信息。 */
            db_base64_to_data(jTemp->valuestring, (uint8*)pAppID->app[i].appId.value, 63, &(pAppID->app[i].appId.num));
        }
    }
    return 0;
}

/**********************************************************************
* @name      : sys_appid_split
* @brief     ：应用信息转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_APPLIC_T *pAppLic       APP许可
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-21
* @Update    :
**********************************************************************/ 
int sys_applic_split(char *infopath, void *pJson, SYS_APPS_LIC_T *pAppLic, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jList = NULL;
    cJSON *jObject = NULL;
    cJSON *jTemp = NULL;
    int    i = 0;
    int    j = 0;
    int    num = 0;
    
    if(NULL == infopath || NULL == pJson || NULL == pAppLic || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETAPPLIC, infopath))
    {
        return -2;
    }

    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -3;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -4;
    }

    memset(pAppLic, 0, sizeof(SYS_APPS_LIC_T));
    
    /*jTemp = cJSON_GetObjectItem(pCJson, "statusDesc");
    if(NULL == jTemp || NULL == jTemp->valuestring || 0 != strcmp(jTemp->valuestring, "OK"))
    {
        return -4;
    }*/

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jList = cJSON_GetObjectItem(jBody, "appList");
    if(NULL == jList || FALSE == cJSON_IsArray(jList))
    {
        return -6;
    }
    
    num = cJSON_GetArraySize(jList);
    
    j = 0;
    for(i = 0; i < num; i++)
    {
        jObject = cJSON_GetArrayItem(jList, i);
        if(NULL == jObject)
        {
            continue;
        }
        if(j >= SYS_APP_LIC_MAX)
        {
            break;
        }
        
        jTemp = cJSON_GetObjectItem(jObject, "appName");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            pAppLic->list[j].appName.num = strlen(jTemp->valuestring);
            memcpy(pAppLic->list[j].appName.value, jTemp->valuestring, pAppLic->list[j].appName.num);
        }
        
        jTemp = cJSON_GetObjectItem(jObject, "appNo");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            /* APP编号，二进制数据经过Base64编码后获得的字符串 */
            db_base64_to_data(jTemp->valuestring, (uint8*)pAppLic->list[j].appNo.value, 64, &(pAppLic->list[j].appNo.num));
        }
        
        jTemp = cJSON_GetObjectItem(jObject, "appLic");
        if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
        {
            /* APP_ID信息，二进制数据经过Base64编码后获得的字符串。空字符串表示不存在对应的APP-ID信息 */
            db_base64_to_data(jTemp->valuestring, (uint8*)pAppLic->list[j].appLic.value, 64, &(pAppLic->list[j].appLic.num));
        }
        j++;
    }
    pAppLic->appnum = j;
    
    return 0;
}

/**********************************************************************
* @name      : sys_sntp_split
* @brief     ：应用信息转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_SNTP_T *pSntp           SNTP配置
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/ 
int sys_sntp_split(char *infopath, void *pJson, SYS_SNTP_T *pSntp, uint32 *token)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    int    a[4] = {0};
    
    if(NULL == infopath || NULL == pJson || NULL == pSntp || NULL == token)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_GETSNTP, infopath))
    {
        return -2;
    }
    
    jTemp = cJSON_GetObjectItem(pCJson, "statusCode");
    if(NULL == jTemp || 0 != jTemp->valueint)
    {
        return -3;
    }

    memset(pSntp, 0, sizeof(SYS_SNTP_T));
    jTemp = cJSON_GetObjectItem(pCJson, "token");
    if(NULL == jTemp)
    {
        return -4;
    }
    if(cJSON_Number == jTemp->type)
    {
        *token = jTemp->valueint;
    }
    else if(cJSON_String == jTemp->type)
    {
        *token = atoi(jTemp->valuestring);
    }
    
    /*jTemp = cJSON_GetObjectItem(pCJson, "statusDesc");
    if(NULL == jTemp || NULL == jTemp->valuestring || 0 != strcmp(jTemp->valuestring, "OK"))
    {
        return -4;
    }*/

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -5;
    }

    jTemp = cJSON_GetObjectItem(jBody, "isEnable");
    if(NULL != jTemp && cJSON_Number != jTemp->type)
    {
       pSntp->isEnable = jTemp->valueint;
    }
    
    jTemp = cJSON_GetObjectItem(jBody, "sntpAddr");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sscanf(jTemp->valuestring, "%d.%d.%d.%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]));
        pSntp->sntpAddr[0] = a[0];
        pSntp->sntpAddr[1] = a[1];
        pSntp->sntpAddr[2] = a[2];
        pSntp->sntpAddr[3] = a[3];
    }

    jTemp = cJSON_GetObjectItem(jBody, "interval");
    if(NULL != jTemp && cJSON_Number != jTemp->type)
    {
        pSntp->interval = jTemp->valueint;
    }

    return 0;
}

/**********************************************************************
* @name      : sys_toreboot_split
* @brief     ：即将重启事件转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_SNTP_T *pSntp           SNTP配置
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/ 
int sys_toreboot_split(char *infopath, void *pJson, uint32 *delaytime)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == delaytime)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_REBOOTEVENT, infopath))
    {
        return -2;
    }

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -3;
    }

    jTemp = cJSON_GetObjectItem(jBody, "delayTime");
    if(NULL != jTemp && cJSON_Number != jTemp->type)
    {
       *delaytime = jTemp->valueint;
    }

    return 0;
}

/**********************************************************************
* @name      : sys_ethSchange_split
* @brief     ：以太网状态事件转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：uint32 *status              状态
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/ 
int sys_ethSchange_split(char *infopath, void *pJson, uint32 *status)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == status)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_ETHSTATUSEVENT, infopath))
    {
        return -2;
    }

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -3;
    }

    jTemp = cJSON_GetObjectItem(jBody, "ethStatus");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
       *status = jTemp->valueint;
    }

    return 0;
}

/**********************************************************************
* @name      : sys_ethCchange_split
* @brief     ：以太网络配置变化事件转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_ETHCFG_T *pETHCfg       以太网络配置
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/ 
int sys_ethCchange_split(char *infopath, void *pJson, SYS_ETHCFG_T *pETHCfg)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    int    a[4] = {0};
    
    if(NULL == infopath || NULL == pJson || NULL == pETHCfg)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_ETHCFGEVENT, infopath))
    {
        return -2;
    }

    memset(pETHCfg, 0, sizeof(SYS_ETHCFG_T));

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -3;
    }

    jTemp = cJSON_GetObjectItem(jBody, "mode");
    if(NULL != jTemp && cJSON_Number == jTemp->type)
    {
        pETHCfg->mode = jTemp->valueint;
    }

    jTemp = cJSON_GetObjectItem(jBody, "ip");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sscanf(jTemp->valuestring, "%d.%d.%d.%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]));
        pETHCfg->ip[0] = a[0];
        pETHCfg->ip[1] = a[1];
        pETHCfg->ip[2] = a[2];
        pETHCfg->ip[3] = a[3];
    }

    jTemp = cJSON_GetObjectItem(jBody, "netMask");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sscanf(jTemp->valuestring, "%d.%d.%d.%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]));
        pETHCfg->netMask[0] = a[0];
        pETHCfg->netMask[1] = a[1];
        pETHCfg->netMask[2] = a[2];
        pETHCfg->netMask[3] = a[3];
    }

    jTemp = cJSON_GetObjectItem(jBody, "gateway");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sscanf(jTemp->valuestring, "%d.%d.%d.%d", &(a[0]), &(a[1]), &(a[2]), &(a[3]));
        pETHCfg->gateway[0] = a[0];
        pETHCfg->gateway[1] = a[1];
        pETHCfg->gateway[2] = a[2];
        pETHCfg->gateway[3] = a[3];
    }
    
    return 0;
}

/**********************************************************************
* @name      : sys_timeevent_split
* @brief     ：系统对时事件转换
* @param[in] ：char *infopath              信息对象
               void *pJson                 JSON
* @param[out]：SYS_TIME_E_T *pTimeEvent    对时设置
               uint32 *token               序号
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-7-26
* @Update    :
**********************************************************************/ 
int sys_timeevent_split(char *infopath, void *pJson, SYS_TIME_E_T *pTimeEvent)
{
    cJSON *pCJson = (cJSON*)pJson;
    cJSON *jBody = NULL;
    cJSON *jTemp = NULL;
    
    if(NULL == infopath || NULL == pJson || NULL == pTimeEvent)
    {
        return -1;
    }
    if(0 != strcmp(SYS_TOPIC_TIMEEVENT, infopath))
    {
        return -2;
    }

    memset(pTimeEvent, 0, sizeof(SYS_TIME_E_T));

    jBody = cJSON_GetObjectItem(pCJson, "body");
    if(NULL == jBody || FALSE == cJSON_IsObject(jBody))
    {
        return -3;
    }

    jTemp = cJSON_GetObjectItem(jBody, "oldtime");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sys_string_time_struct(jTemp->valuestring, &(pTimeEvent->oldTime));
    }

    jTemp = cJSON_GetObjectItem(jBody, "newtime");
    if(NULL != jTemp && NULL != jTemp->valuestring && 0 != strlen(jTemp->valuestring))
    {
        sys_string_time_struct(jTemp->valuestring, &(pTimeEvent->newTime));
    }
    
    return 0;
}


/**********************************************************************
* @name      : sys_common_package
* @brief     ：一般报文构造
* @param[in] ：uint32 token         序列号
               uint32 bufLen        缓存长度
* @param[out]：char *pBuf           消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/
uint32 sys_common_package(uint32 token, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char    *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_appreg_package
* @brief     ：APP注册报文构造
* @param[in] ：uint32 token          序列号
               SYS_APPREG_T *pApp    APP注册信息
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
uint32 sys_appreg_package(uint32 token, SYS_APPREG_T *pApp, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    char    strtmp[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);

    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);

    cJSON_AddItemToObject(pbody, "name", cJSON_CreateString(pApp->name.value));
    cJSON_AddItemToObject(pbody, "version", cJSON_CreateNumber(pApp->version));
    
    sprintf(strtmp, "%04d-%02d-%02d %02d:%02d:%02d", pApp->releaseDate.year, pApp->releaseDate.month,
                                                     pApp->releaseDate.day, pApp->releaseDate.hour,
                                                     pApp->releaseDate.min, pApp->releaseDate.sec);
    cJSON_AddItemToObject(pbody, "releaseDate", cJSON_CreateString(strtmp));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_appunreg_package
* @brief     ：APP取消注册报文构造
* @param[in] ：uint32 token          序列号
               SYS_APPREG_T *pApp    APP注册信息
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
uint32 sys_appunreg_package(uint32 token, char *appname, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);

    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "name", cJSON_CreateString(appname));
    
    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_keepAlive_package
* @brief     ：保活报文构造
* @param[in] ：uint32 token          序列号
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
uint32 sys_keepAlive_package(uint32 token, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);

    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));
    cJSON_AddItemToObject(root, "statusCode", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(root, "statusDesc", cJSON_CreateString("OK"));
    
    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_getregapp_package
* @brief     ：查询APP注册报文构造
* @param[in] ：uint32 token          序列号
               char *appname         APP名称
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-23
* @Update    :
**********************************************************************/
uint32 sys_getregapp_package(uint32 token, char *appname, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "name", cJSON_CreateString(appname));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_ethcfg_package
* @brief     ：太网络配置报文构造
* @param[in] ：uint32 token          序列号
               SYS_ETHCFG_T *pEthCfg 以太网配置
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/
uint32 sys_ethcfg_package(uint32 token, SYS_ETHCFG_T *pEthCfg, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    char    strtmp[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);

    cJSON_AddItemToObject(pbody, "mode", cJSON_CreateNumber(pEthCfg->mode));
    if(1 == pEthCfg->mode)
    {
        sprintf(strtmp, "%d.%d.%d.%d", pEthCfg->ip[0], pEthCfg->ip[1], pEthCfg->ip[2], pEthCfg->ip[3]);
        cJSON_AddItemToObject(pbody, "ip", cJSON_CreateString(strtmp));
        
        sprintf(strtmp, "%d.%d.%d.%d", pEthCfg->netMask[0], pEthCfg->netMask[1], pEthCfg->netMask[2], pEthCfg->netMask[3]);
        cJSON_AddItemToObject(pbody, "netMask", cJSON_CreateString(strtmp));
        
        sprintf(strtmp, "%d.%d.%d.%d", pEthCfg->gateway[0], pEthCfg->gateway[1], pEthCfg->gateway[2], pEthCfg->gateway[3]);
        cJSON_AddItemToObject(pbody, "gateway", cJSON_CreateString(strtmp));
    }
    else
    {
        cJSON_AddItemToObject(pbody, "ip", cJSON_CreateString(""));
        cJSON_AddItemToObject(pbody, "netMask", cJSON_CreateString(""));
        cJSON_AddItemToObject(pbody, "gateway", cJSON_CreateString(""));
    }
    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_appinfo_package
* @brief     ：应用信息报文构造
* @param[in] ：uint32 token          序列号
               char *appname         APP名称
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/
uint32 sys_appinfo_package(uint32 token, char *appname, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "appName", cJSON_CreateString(appname));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_appinfo_package
* @brief     ：容器内APP查询报文构造
* @param[in] ：uint32 token          序列号
               char *containername   容器名称
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-8-11
* @Update    :
**********************************************************************/
uint32 sys_containerapp_package(uint32 token, char *containername, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "container", cJSON_CreateString(containername));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_appid_package
* @brief     ：appid报文构造
* @param[in] ：uint32 token          序列号
               char *appname         APP名称
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-21
* @Update    :
**********************************************************************/
uint32 sys_appid_package(uint32 token, char *appname, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "appName", cJSON_CreateString(appname));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_applic_package
* @brief     ：applic报文构造
* @param[in] ：uint32 token          序列号
               char *appNo          APP名称
               uint32 bufLen        缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-21
* @Update    :
**********************************************************************/
uint32 sys_applic_package(uint32 token, char *appNo, uint8 *appLic, uint16 liclen, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;
    uint8   temp = 0;
    int     i, len = 0;
    int     appNoLen = 0;
    uint8   appNo8[32] = {0};
    char   *appNo64 = NULL;
    char   *appLic64 = NULL;
    
    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);

    len = strlen(appNo);
    for(i = 0; i < len; i++)
    {
        temp = (appNo[i] - '0') * 16;
        if(i + 1 < len)
        {
            temp += appNo[i + 1] - '0';
        }
        appNo8[appNoLen++] = temp;
        i++;
    }
    
    appNo64 = db_data_to_base64(appNo8, appNoLen);
    if(NULL == appNo64)
    {
        goto end;
    }
    cJSON_AddItemToObject(pbody, "appNo", cJSON_CreateString(appNo64));
    free(appNo64);

    appLic64 = db_data_to_base64(appLic, liclen);
    if(NULL == appLic64)
    {
        goto end;
    }
    cJSON_AddItemToObject(pbody, "appLic", cJSON_CreateString(appLic64));
    free(appLic64);

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}


/**********************************************************************
* @name      : sys_settime_package
* @brief     ：设置时间报文构造
* @param[in] ：uint32 token          序列号
               Date_Time *pTime      目标时间
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/
uint32 sys_settime_package(uint32 token, Date_Time *pTime, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    
    memset(TIME, 0, 36);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02dZ", pTime->year, pTime->month, pTime->day, pTime->hour, 
                                                  pTime->min, pTime->sec);
    cJSON_AddItemToObject(pbody, "setTime", cJSON_CreateString(TIME));

    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d.%03ld", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                       tm.tm_min, tm.tm_sec, tv.tv_usec/1000);
    cJSON_AddItemToObject(pbody, "reqTime", cJSON_CreateString(TIME));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_route_package
* @brief     ：添加删除路由报文构造
* @param[in] ：uint32 token          序列号
               SYS_ROUTE_T *pRoute   路由
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/
uint32 sys_route_package(uint32 token, SYS_ROUTE_T *pRoute, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    char    strtemp[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                   tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    
    sprintf(strtemp, "%d.%d.%d.%d", pRoute->destIp[0], pRoute->destIp[1], pRoute->destIp[2], pRoute->destIp[3]);
    cJSON_AddItemToObject(pbody, "destIp", cJSON_CreateString(strtemp));
    sprintf(strtemp, "%d.%d.%d.%d", pRoute->netMask[0], pRoute->netMask[1], pRoute->netMask[2], pRoute->netMask[3]);
    cJSON_AddItemToObject(pbody, "netMask", cJSON_CreateString(strtemp));
    sprintf(strtemp, "%d.%d.%d.%d", pRoute->gateway[0], pRoute->gateway[1], pRoute->gateway[2], pRoute->gateway[3]);
    cJSON_AddItemToObject(pbody, "gateway", cJSON_CreateString(strtemp));
    cJSON_AddItemToObject(pbody, "interface", cJSON_CreateString(pRoute->ifname.value));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_sntp_package
* @brief     ：SNTP对时配置报文构造
* @param[in] ：uint32 token          序列号
               SYS_SNTP_T *pSntp     SNTP配置
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/
uint32 sys_sntp_package(uint32 token, SYS_SNTP_T *pSntp, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    char    strtemp[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    
    cJSON_AddItemToObject(pbody, "isEnable", cJSON_CreateNumber(pSntp->isEnable));
    sprintf(strtemp, "%d.%d.%d.%d", pSntp->sntpAddr[0], pSntp->sntpAddr[1], pSntp->sntpAddr[2], pSntp->sntpAddr[3]);
    cJSON_AddItemToObject(pbody, "sntpAddr", cJSON_CreateString(strtemp));
    cJSON_AddItemToObject(pbody, "interval", cJSON_CreateNumber(pSntp->interval));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_reboot_package
* @brief     ：终端重启报文构造
* @param[in] ：uint32 token          序列号
               uint32 delytime       延时时间
               uint32 bufLen         缓存长度
* @param[out]：char *pBuf            消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2021-7-19
* @Update    :
**********************************************************************/
uint32 sys_reboot_package(uint32 token, uint32 delytime, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;     
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;

    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    
    cJSON_AddItemToObject(pbody, "delayTime", cJSON_CreateNumber(delytime));
    
    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_install_con_package
* @brief     ：容器安装报文构造
* @param[in] ：uint32 token                  序列号
               SYS_INSTALL_CON_T *pConInfo  安装容器信息
               uint32 bufLen                缓存长度
* @param[out]：char *pBuf                    消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2022-5-9
* @Update    :
**********************************************************************/
uint32 sys_install_con_package(uint32 token, SYS_INSTALL_CON_T *pConInfo, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;
    struct  tm       tm;
    cJSON  *pbody = NULL;
    cJSON  *pTempbody = NULL;
    cJSON  *pTempArray = NULL;
    cJSON  *pTempbody1 = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;
    uint8   i = 0;
    
    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "container", cJSON_CreateString(pConInfo->name.value));

    if(0 != pConInfo->bCpu)
    {
        pTempbody = cJSON_CreateObject();
        cJSON_AddItemToObject(pbody, "cfgCpu", pTempbody);
        cJSON_AddItemToObject(pTempbody, "cpus", cJSON_CreateNumber(pConInfo->cpu_num));
        cJSON_AddItemToObject(pTempbody, "cpuLmt", cJSON_CreateNumber(pConInfo->cpu_limit));
    }

    if(0 != pConInfo->bMem)
    {
        pTempbody = cJSON_CreateObject();
        cJSON_AddItemToObject(pbody, "cfgMem", pTempbody);
        cJSON_AddItemToObject(pTempbody, "memory", cJSON_CreateNumber(pConInfo->memory));
        cJSON_AddItemToObject(pTempbody, "memLmt", cJSON_CreateNumber(pConInfo->mem_limit));
    }

    if(0 != pConInfo->bDisk)
    {
        pTempbody = cJSON_CreateObject();
        cJSON_AddItemToObject(pbody, "cfgDisk", pTempbody);
        cJSON_AddItemToObject(pTempbody, "disk", cJSON_CreateNumber(pConInfo->disk));
        cJSON_AddItemToObject(pTempbody, "diskLmt", cJSON_CreateNumber(pConInfo->disk_limit));
    }

    if(pConInfo->port_num > 0)
    {
        pTempArray = cJSON_CreateArray();
        cJSON_AddItemToObject(pbody, "port", pTempArray);
        for(i = 0; i < pConInfo->port_num; i++)
        {
            cJSON_AddItemToArray(pTempArray, cJSON_CreateString(pConInfo->port[i].value));
        }
    }

    if(pConInfo->dev_num > 0)
    {
        pTempArray = cJSON_CreateArray();
        cJSON_AddItemToObject(pbody, "dev", pTempArray);
        for(i = 0; i < pConInfo->dev_num; i++)
        {
            cJSON_AddItemToArray(pTempArray, cJSON_CreateString(pConInfo->dev[i].value));
        }
    }

    if(pConInfo->mount_num > 0)
    {
        pTempArray = cJSON_CreateArray();
        cJSON_AddItemToObject(pbody, "mount", pTempArray);
        for(i = 0; i < pConInfo->mount_num; i++)
        {
            cJSON_AddItemToArray(pTempArray, cJSON_CreateString(pConInfo->mount[i].value));
        }
    }

    if(0 != pConInfo->withapp)
    {
        pTempbody = cJSON_CreateObject();
        cJSON_AddItemToObject(pbody, "withAPP", pTempbody);
        cJSON_AddItemToObject(pTempbody, "app", cJSON_CreateString(pConInfo->app.name.value));

        pTempbody1 = cJSON_CreateObject();
        cJSON_AddItemToObject(pTempbody, "cfgCPU", pTempbody1);
        cJSON_AddItemToObject(pTempbody1, "cpus", cJSON_CreateNumber(pConInfo->app.cpu_num));
        cJSON_AddItemToObject(pTempbody1, "cpuLmt", cJSON_CreateNumber(pConInfo->app.cpu_limit));

        pTempbody1 = cJSON_CreateObject();
        cJSON_AddItemToObject(pTempbody, "cfgMem", pTempbody1);
        cJSON_AddItemToObject(pTempbody1, "memory", cJSON_CreateNumber(pConInfo->app.memory));
        cJSON_AddItemToObject(pTempbody1, "memLmt", cJSON_CreateNumber(pConInfo->app.mem_limit));
    }
    
    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }
    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_install_set_app_package
* @brief     ：APP安装修改报文构造
* @param[in] ：uint32 token                  序列号
               SYS_INSTALL_APP_T *pAppInfo  安装APP信息
               uint32 bufLen                缓存长度
* @param[out]：char *pBuf                    消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2022-5-9
* @Update    :
**********************************************************************/
uint32 sys_install_set_app_package(uint32 token, SYS_INSTALL_APP_T *pAppInfo, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;
    struct  tm       tm;
    cJSON  *pbody = NULL;
    cJSON  *pTempbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;
    
    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "container", cJSON_CreateString(pAppInfo->conname.value));
    cJSON_AddItemToObject(pbody, "app", cJSON_CreateString(pAppInfo->name.value));

    if(0 != pAppInfo->bCpu)
    {
        pTempbody = cJSON_CreateObject();
        cJSON_AddItemToObject(pbody, "cfgCpu", pTempbody);
        cJSON_AddItemToObject(pTempbody, "cpus", cJSON_CreateNumber(pAppInfo->cpu_num));
        cJSON_AddItemToObject(pTempbody, "cpuLmt", cJSON_CreateNumber(pAppInfo->cpu_limit));
    }

    if(0 != pAppInfo->bMem)
    {
        pTempbody = cJSON_CreateObject();
        cJSON_AddItemToObject(pbody, "cfgMem", pTempbody);
        cJSON_AddItemToObject(pTempbody, "memory", cJSON_CreateNumber(pAppInfo->memory));
        cJSON_AddItemToObject(pTempbody, "memLmt", cJSON_CreateNumber(pAppInfo->mem_limit));
    }
    
    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_update_app_package
* @brief     ：APP升级报文构造
* @param[in] ：uint32 token                  序列号
               char *conname                容器名称
               char *appname                APP名称
               uint32 bufLen                缓存长度
* @param[out]：char *pBuf                    消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2022-5-9
* @Update    :
**********************************************************************/
uint32 sys_update_app_package(uint32 token, char *conname, char *appname, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;
    
    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "container", cJSON_CreateString(conname));
    cJSON_AddItemToObject(pbody, "app", cJSON_CreateString(appname));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}

/**********************************************************************
* @name      : sys_operate_app_package
* @brief     ：APP操作报文构造
* @param[in] ：uint32 token                  序列号
               char *conname                容器名称
               char *appname                APP名称
               uint8 type                   操作类型：     0：卸载APP;    1：停止APP;    2：启动APP
               uint32 bufLen                缓存长度
* @param[out]：char *pBuf                    消息内容
* @return    ：消息长度
* @Create    : 王津剑
* @Date      ：2022-5-9
* @Update    :
**********************************************************************/
uint32 sys_operate_app_package(uint32 token, char *conname, char *appname, uint8 type, char *pBuf, uint32 bufLen)
{
    char    TIME[36] = {0};
    cJSON  *root = NULL;
    struct  timeval  tv;
    struct  timezone tz;
    struct  tm       tm;
    cJSON  *pbody = NULL;
    char   *pMsg = NULL;
    uint32  msglen = 0;
    
    root = cJSON_CreateObject();
    if(NULL == root)
    {
        return 0;
    }

    gettimeofday(&tv, &tz);
    FastSecondToDate(&tv, &tz, &tm);
    sprintf(TIME, "%4d-%02d-%02dT%02d:%02d:%02d+08:00", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
                                                        tm.tm_min, tm.tm_sec);
    cJSON_AddItemToObject(root, "token", cJSON_CreateNumber(token));
    cJSON_AddItemToObject(root, "timestamp", cJSON_CreateString(TIME));

    pbody = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "body", pbody);
    cJSON_AddItemToObject(pbody, "container", cJSON_CreateString(conname));
    cJSON_AddItemToObject(pbody, "app", cJSON_CreateString(appname));
    cJSON_AddItemToObject(pbody, "type", cJSON_CreateNumber(type));

    pMsg = cJSON_Print(root);
    if(NULL == pMsg)
    {
        goto end;
    }

    msglen = strlen(pMsg);
    if(msglen > bufLen)
    {
        msglen = 0;
        goto end;
    }

    memcpy(pBuf, pMsg, msglen);
end:
    if(pMsg)
    {
        free(pMsg);
    }
    cJSON_Delete(root);
    return msglen;
}



