/*
*********************************************************************
* @filename: frozen_gather.c
* @brief   : 冻结数据采集
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2021-6-15     create
*********************************************************************
*/
#include "baseCommon.h"
#include "cJSON.h"
#include "oopType.h"

#include "min_gather.h"
#include "comm_gather.h"

/**********************************************************************
* @name      : min_gather_create_frozen
* @brief     ：创建冻结集合
* @param[in] ：time_t linuxTime  时间
* @param[out]：
* @return    ：MG_FROZEN_T *     冻结集合 
* @Create    : 王津剑
* @Date      ：2021-6-15
* @Update    :
**********************************************************************/
MG_FROZEN_T *min_gather_create_frozen(time_t linuxTime)
{
    MG_FROZEN_T *frozenfile = NULL;
    OOP_DATETIME_S_T timeday;
    
    frozenfile = (MG_FROZEN_T *)malloc(sizeof(MG_FROZEN_T));
    if(NULL == frozenfile)
    {
        return NULL;
    }
    memset(frozenfile, 0, sizeof(MG_FROZEN_T));
    
    min_gather_time_t(linuxTime, &timeday);
    timeday.hour = 0;
    timeday.minute = 0;
    timeday.second = 0;
    frozenfile->filetime = min_gather_time_s(&timeday);
    frozenfile->fileday = timeday;
    //pthread_mutex_init(&(frozenfile->lock), NULL);

    frozenfile->meterRoot = rbtree_create();
    if(NULL == frozenfile->meterRoot)
    {
        free(frozenfile);
        frozenfile = NULL;
    }
    return frozenfile;
}

/**********************************************************************
* @name      : min_gather_create_meter
* @brief     ：创建冻结表
* @param[in] ：uint8 *mac                 mac地址
* @param[out]：MG_FROZEN_METER_T *pMeter  电表缓存
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-17
* @Update    :
**********************************************************************/
int min_gather_create_meter(uint8 *mac, MG_FROZEN_METER_T *pMeter)
{
    char strtemp[24] = {0};
    if(NULL == mac || NULL == pMeter)
    {
        return MIN_GATHER_ERR_INPUT;
    }

    memcpy(pMeter->Addr, mac, 6);
    pMeter->listNum = 0;
    pMeter->jsonRoot = cJSON_CreateObject();
    if(NULL == pMeter->jsonRoot)
    {
        return MIN_GATHER_ERR_JSON;
    }
    sprintf(strtemp, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    cJSON_AddItemToObject(pMeter->jsonRoot, "addr", cJSON_CreateString(strtemp));
    cJSON_AddItemToObject(pMeter->jsonRoot, "itemList", cJSON_CreateArray());
    
    pMeter->listItem = rbtree_create();
    if(NULL == pMeter->listItem)
    {
        cJSON_Delete(pMeter->jsonRoot);
        pMeter->jsonRoot = NULL;
        return MIN_GATHER_ERR_RBTREE;
    }

    return MIN_GATHER_SUCCESS;
}

/**********************************************************************
* @name      : min_gather_destory_frozen
* @brief     ：冻结集合销毁
* @param[in] ：MG_FROZEN_T *ptFrozen  冻结集合 
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
void min_gather_destory_frozen(MG_FROZEN_T *ptFrozen)
{
    RBUserNode *pUserNode = NULL;
    MG_FROZEN_METER_T *pMeter = NULL;

    if(NULL == ptFrozen)
    {
        return;
    }
    
    while(1)
    {
        pUserNode = rbtree_search_first(ptFrozen->meterRoot);
        if(NULL == pUserNode)
        {
            break;
        }
        pMeter = (MG_FROZEN_METER_T*)pUserNode->data;
        /* 释放资源 */
        cJSON_Delete(pMeter->jsonRoot);
        pMeter->jsonRoot = NULL;
        rbtree_destroy_all(pMeter->listItem);
        pMeter->listItem = NULL;
        rbtree_delete_bynode(ptFrozen->meterRoot, pUserNode);
        pUserNode = NULL;
    }
    rbtree_destroy_all(ptFrozen->meterRoot);
    free(ptFrozen);
}

/**********************************************************************
* @name      : min_gather_forzen_store
* @brief     ：冻结表写文件，写入文件后ptFrozen释放
* @param[in] ：uint8 *addr                终端地址
               char *filepath             文件路径
               MG_FROZEN_T *ptFrozen      冻结集合
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-26
* @Update    :
**********************************************************************/
int min_gather_forzen_store(uint8 *addr, char *filepath, MG_FROZEN_T *ptFrozen)
{
    int         ret = MIN_GATHER_SUCCESS;
    int         fd = -1;
    char        filename[256] = {0};
    char        dayname[256] = {0};
    RBUserNode *pUserNode = NULL;
    MG_FROZEN_METER_T *pMeter = NULL;
    char              *jsonstr = NULL;
    
    sprintf(filename, "%s/%s%02X%02X%02X%02X%02X%02X_%04d%02d%02d.json", filepath, MIN_GATHER_FROZEN_NAMR,
                      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], 
                      ptFrozen->fileday.year, ptFrozen->fileday.month, ptFrozen->fileday.day);

    /* 1. 文件夹检查 */
    if(0 != access(filepath, F_OK))
    {
        if(0 != mkdir(filepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
        {
            ret = MIN_GATHER_ERR_FILE;
            goto end;
        }
    }

    /* 2. 打开或新建文件 */
    fd = open(filename, O_CREAT |O_WRONLY | O_APPEND, 0664);
    if(fd < 0)
    {
        ret = MIN_GATHER_ERR_FILE;
        goto end;
    }

    /* 3. 加锁 */
    pthread_cleanup_push(min_gather_unlock_file, (void *)&fd);
    flock(fd, LOCK_EX);

    /* 4. 写文件 */
    while(1)
    {
        pUserNode = rbtree_search_first(ptFrozen->meterRoot);
        if(NULL == pUserNode)
        {
            break;
        }
        pMeter = (MG_FROZEN_METER_T*)pUserNode->data;
        jsonstr = cJSON_PrintUnformatted(pMeter->jsonRoot);
        if(NULL != jsonstr)
        {
            write(fd, jsonstr, strlen(jsonstr));
            write(fd, "\r\n", 2);
            free(jsonstr);
            jsonstr = NULL;
        }
        /* 释放资源 */
        cJSON_Delete(pMeter->jsonRoot);
        pMeter->jsonRoot = NULL;
        rbtree_destroy_all(pMeter->listItem);
        pMeter->listItem = NULL;
        rbtree_delete_bynode(ptFrozen->meterRoot, pUserNode);
        pUserNode = NULL;
    }

    /* 5. 解锁 */
    flock(fd, LOCK_UN);
    close(fd);
    pthread_cleanup_pop(0);

    /* 6. 写时间文件 */
    sprintf(dayname, "%s/lastwriteday", filepath);
    fd = open(dayname, O_CREAT |O_WRONLY, 0664);
    if(fd < 0)
    {
        ret = MIN_GATHER_ERR_FILE;
        goto end;
    }

    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_SET);
    write(fd, &(ptFrozen->filetime), sizeof(uint32));
    flock(fd, LOCK_UN);
    close(fd);

end:
    /* 7. 剩余资源释放 */
    min_gather_destory_frozen(ptFrozen);
    return ret;
}

/**********************************************************************
* @name      : min_gather_forzen_getstore
* @brief     ：冻结时间读取
* @param[in] ：char *filepath             文件路径
* @param[out]：
* @return    ：uint8 flag
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
uint32 min_gather_forzen_getstore(char *filepath)
{
    int         fd = -1;
    char        filename[256] = {0};
    uint32      times = 0;
    sprintf(filename, "%s/lastwriteday", filepath);

    fd = open(filename, O_RDONLY);
    if(fd < 0)
    {
        return 0;
    }

    lseek(fd, 0, SEEK_SET);
    read(fd, &times, sizeof(uint32));
    close(fd);

    if(times > 0xFFFF0000)
    {
        times = 0;
    }
    return times;
}

/**********************************************************************
* @name      : min_gather_forzen_flag
* @brief     ：冻结标记设置
* @param[in] ：char *filepath             文件路径
               uint8 flag                 标记
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
void min_gather_forzen_flag(char *filepath, uint8 flag)
{
    int         fd = -1;
    char        filename[256] = {0};
    time_t      times = 0;
    
    sprintf(filename, "%s/frozenflag", filepath);

    fd = open(filename, O_CREAT |O_WRONLY, 0664);
    if(fd < 0)
    {
        return;
    }

    times = time(NULL);

    flock(fd, LOCK_EX);
    lseek(fd, 0, SEEK_SET);
    write(fd, &flag, sizeof(uint8));
    write(fd, &times, sizeof(uint32));
    flock(fd, LOCK_UN);
    close(fd);
}

/**********************************************************************
* @name      : min_gather_forzen_getflag
* @brief     ：冻结标记读取
* @param[in] ：char *filepath             文件路径
* @param[out]：uint32 *times              设置时间
* @return    ：uint8 flag
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
uint8 min_gather_forzen_getflag(char *filepath, uint32 *times)
{
    int         fd = -1;
    char        filename[256] = {0};
    uint8       flag = 0;
    sprintf(filename, "%s/frozenflag", filepath);

    fd = open(filename, O_RDONLY);
    if(fd < 0)
    {
        return flag;
    }

    lseek(fd, 0, SEEK_SET);
    read(fd, &flag, sizeof(uint8));
    lseek(fd, 1, SEEK_SET);
    read(fd, times, sizeof(uint32));
    close(fd);

    if(*times > 0xFFFF0000)
    {
        *times = time(NULL);
    }
    
    return flag;
}

/**********************************************************************
* @name      : min_gather_forzen_zip
* @brief     ：冻结表文件压缩(删除旧json文件)
* @param[in] ：char *filepath             文件路径
               OOP_DATETIME_S_T *ziptime  压缩时间
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
int min_gather_forzen_zip(char *filepath, OOP_DATETIME_S_T *ziptime)
{
    int  ret = MIN_GATHER_SUCCESS;
    char filename[256] = {0};
    char pathname[260] = {0};
    char newname[256] = {0};
    char cmd[512] = {0};
    int  namelen = 0;
    DIR           *pDdir = NULL;
    struct dirent *dp = NULL;

    if(NULL == filepath || NULL == ziptime)
    {
        return MIN_GATHER_ERR_INPUT;
    }
    
    sprintf(filename, "%04d%02d%02d.json", ziptime->year, ziptime->month, ziptime->day);
    pDdir = opendir(filepath);
    if(NULL == pDdir)
    {
        return MIN_GATHER_ERR_FILE;
    }
    
    while((dp = readdir(pDdir)) != NULL)
    {
        if(0 == strcmp(dp->d_name, ".") || 0 == strcmp(dp->d_name, ".."))
        {
            continue;
        }
        /* 待压缩文件判断 */
        if(NULL != strstr(dp->d_name, MIN_GATHER_FROZEN_NAMR) && NULL != strstr(dp->d_name, ".json"))
        {
            sprintf(pathname, "%s/%s", filepath, dp->d_name);
            if(NULL != strstr(dp->d_name, filename))
            {
                sprintf(newname, "%s/", filepath);
                namelen = strlen(newname);
                memcpy(newname + namelen, dp->d_name, strlen(dp->d_name) - 4);
                namelen = strlen(newname);
                memcpy(newname + namelen, "tar.bz2", strlen("tar.bz2"));
                sprintf(cmd, "tar -cjf %s %s", newname, pathname);
                system(cmd);
                usleep(10000);
            }
            else
            {
                remove(pathname);
            }
        }
    }
    closedir(pDdir);

    return ret;
}


/**********************************************************************
* @name      : min_gather_frozen_add
* @brief     ：分钟采集增加冻结数据
* @param[in] ：MG_FROZEN_METER_T *pMeter 采集数据
               uint16 type               类型(MIN_GATHER_TYPE_E)
               uint32 itemId             数据标识
               uint32 storeTm            存储时间
               uint32 succTm             成功时间
               uint32 intv               间隔
               cJSON *jsonData           单个数据
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-21
* @Update    :
**********************************************************************/
int min_gather_frozen_add(MG_FROZEN_METER_T *pGdate, uint16 type, uint32 itemId, uint32 storeTm, uint32 succTm, 
                          uint32 intv, cJSON *jsonData)
{
    int               ret = 0;
    RBUserNode       *pDataNode = NULL;
    MG_FROZEN_ITEM_T *pItemNote = NULL;
    KEY               keyid = itemId;
    cJSON            *jArray = NULL;
    cJSON            *jItem = NULL;
    OOP_DATETIME_S_T  temptime;
    char              strtemp[256] = {0};
    int               index = 0;
    int               i = 0;
    
    jArray = cJSON_GetObjectItem(pGdate->jsonRoot, "itemList");
    if(NULL == jArray)
    {
        return MIN_GATHER_ERR_JSON;
    }
    keyid = (keyid << 32) + (intv << 16) + type;
    pDataNode = rbtree_search(pGdate->listItem, keyid);
    if(NULL == pDataNode)
    {
        /* 不存在 创建一个新的 */
        MG_FROZEN_ITEM_T tCurList;
        char             strtype[6] = {0};
        char             stritemId[9] = {0};
        
        jItem = cJSON_CreateObject();
        if(NULL == jItem)
        {
            return MIN_GATHER_ERR_JSON;
        }

        sprintf(strtype, "%04X", type);
        cJSON_AddItemToObject(jItem, "type", cJSON_CreateString(strtype));
        sprintf(stritemId, "%08X", itemId);
        cJSON_AddItemToObject(jItem, "itemId", cJSON_CreateString(stritemId));

        min_gather_time_t(succTm, &temptime);
        sprintf(strtemp, "%04u-%02u-%02u %02u:%02u:%02u", temptime.year, temptime.month, temptime.day,
                                                          temptime.hour, temptime.minute, temptime.second);
        cJSON_AddItemToObject(jItem, "ot", cJSON_CreateString(strtemp));

        min_gather_time_t(storeTm, &temptime);
        sprintf(strtemp, "%04u-%02u-%02u %02u:%02u:%02u", temptime.year, temptime.month, temptime.day,
                                                          temptime.hour, temptime.minute, temptime.second);
        cJSON_AddItemToObject(jItem, "dt", cJSON_CreateString(strtemp));

        cJSON_AddItemToObject(jItem, "intv", cJSON_CreateNumber(intv));
        cJSON_AddItemToObject(jItem, "num", cJSON_CreateNumber(0));
        cJSON_AddItemToObject(jItem, "dl", cJSON_CreateArray());

        memset(&tCurList, 0, sizeof(MG_FROZEN_ITEM_T));
        tCurList.id = itemId;
        tCurList.type = type;
        tCurList.intv = intv;
        tCurList.SuccessTime = succTm;
        tCurList.StoreTime = storeTm;
        tCurList.jsonItem = jItem;
        ret = rbtree_insert_node(pGdate->listItem, keyid, (uint8*)&tCurList, sizeof(MG_FROZEN_ITEM_T), &pDataNode);
        if(0 != ret)
        {
            cJSON_free(jItem);
            return MIN_GATHER_ERR_RBTREE;
        }
        
        cJSON_AddItemToArray(jArray, jItem);
        pGdate->listNum += 1;
        jItem = NULL;
    }

    pItemNote = (MG_FROZEN_ITEM_T *)pDataNode->data;

    /* json操作 */
    if(succTm > pItemNote->SuccessTime)
    {
        /* 采集成功时间 */
        jItem = cJSON_GetObjectItem(pItemNote->jsonItem, "ot");
        min_gather_time_t(succTm, &temptime);
        sprintf(strtemp, "%04u-%02u-%02u %02u:%02u:%02u", temptime.year, temptime.month, temptime.day,
                                                          temptime.hour, temptime.minute, temptime.second);
        memcpy(jItem->valuestring, strtemp, strlen(strtemp));
        pItemNote->SuccessTime = succTm;
    }

    /* 采集存储时间 */
    if(storeTm >= pItemNote->StoreTime)
    {
        /*index计算*/
        if(MIN_GATHER_TYPE_MIN == type)
        {
            index = (storeTm - pItemNote->StoreTime) / (60 * pItemNote->intv);
        }
        else if(MIN_GATHER_TYPE_HOUR == type)
        {
            index = (storeTm - pItemNote->StoreTime) / (3600 * pItemNote->intv);
        }
        else
        {
            index = 0;
        }
        jItem = cJSON_GetObjectItem(pItemNote->jsonItem, "dl");
        if(index >= pItemNote->num)
        {
            for(i = pItemNote->num; i < index; i++)
            {
                cJSON_AddItemToArray(jItem, cJSON_CreateNull());
                pItemNote->num += 1;
            }
            cJSON_AddItemToArray(jItem, jsonData);
            pItemNote->num += 1;
        }
        else
        {
            cJSON_ReplaceItemInArray(jItem, index, jsonData);
        }
        jItem = cJSON_GetObjectItem(pItemNote->jsonItem, "num");
        cJSON_SetNumberValue(jItem, pItemNote->num);
    }
    else
    {
        /* 需要在前面补充 */
        if(MIN_GATHER_TYPE_MIN == type)
        {
            index = (pItemNote->StoreTime - storeTm) / (60 * pItemNote->intv);
        }
        else if(MIN_GATHER_TYPE_HOUR == type)
        {
            index = (pItemNote->StoreTime - storeTm) / (3600 * pItemNote->intv);
        }
        else
        {
            index = 0;
        }
        
        jItem = cJSON_GetObjectItem(pItemNote->jsonItem, "dl");
        if(0 == index)
        {
            cJSON_ReplaceItemInArray(jItem, index, jsonData);
        }
        else
        {
            for(i = 1; i < index; i++)
            {
                cJSON_InsertItemInArray(jItem, 0, cJSON_CreateNull());
                pItemNote->num += 1;
            }
            cJSON_InsertItemInArray(jItem, 0, jsonData);
            pItemNote->num += 1;
        }
        jItem = cJSON_GetObjectItem(pItemNote->jsonItem, "num");
        cJSON_SetNumberValue(jItem, pItemNote->num);

        jItem = cJSON_GetObjectItem(pItemNote->jsonItem, "dt");
        min_gather_time_t(storeTm, &temptime);
        sprintf(strtemp, "%04u-%02u-%02u %02u:%02u:%02u", temptime.year, temptime.month, temptime.day,
                                                          temptime.hour, temptime.minute, temptime.second);
        memcpy(jItem->valuestring, strtemp, strlen(strtemp));
        pItemNote->StoreTime = storeTm;
    }

    return MIN_GATHER_SUCCESS;
}

