/*
*********************************************************************
* @filename: comm_gather.c
* @brief   : 分钟采集相关公共库
* @describe: 
* @history: 
*    auther      date          describe
*    王津剑      2021-6-15     create
*********************************************************************
*/
#include "baseCommon.h"
#include <sys/prctl.h>
#include "oopType.h"

#include "min_gather.h"
#include "comm_gather.h"

char      MINGatherDir[256] = {0};
uint8     g_MinGatherAddr[6] = {0};
/* 使用红黑树提高检索效率 */
RBRoot   *g_pFrozenFun = NULL;
RBRoot   *g_pRealFun = NULL;

uint8         g_InitDataFlag = FALSE;
uint32        g_InitFrozenNum = 0;
MG_FROZEN_T **g_InitFrozen = NULL;

MG_FROZEN_T *g_CurFrozen = NULL;   /* 当前(0h-24H写入) */
MG_FROZEN_T *g_LastFrozen = NULL;  /* 即将写入文件(0h-1h存在) */
MG_FROZEN_T *g_OldFrozen = NULL;   /* 对时导致出现 */
pthread_mutex_t g_MinGatherlock;   /* 客户端操作锁 */
uint8     g_WriteFileMin = 55;     /* 写入文件的时间 */
uint8     g_ZipFileMin = 59;       /* 压缩文件的时间 */

uint8     g_MINFrozenWork = FALSE;
uint8     g_MINRealWork = FALSE;
uint8     g_MINRealAll = TRUE;
MG_REAL_T g_MINRealData;
pthread_mutex_t g_MinReallock;   /* 客户端操作锁 */



extern void FastSecondToDate(struct timeval* tv, struct timezone* tz, struct tm* tm);
extern time_t FastDateToSecond(struct tm* tm);

/* 解锁文件 */
void min_gather_unlock_file(void *arg)
{
    flock(*(int*)arg, LOCK_UN); //Unlock
    close(*(int*)arg);
}

/**********************************************************************
* @name      : min_gather_time_t
* @brief     ：时间转换
* @param[in] ：time_t times               秒时间
* @param[out]：OOP_DATETIME_S_T *curTime  时间
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-15
* @Update    :
**********************************************************************/
void min_gather_time_t(time_t times, OOP_DATETIME_S_T *curTime)
{
    struct  tm       systime;
    struct  timeval  tv;
    struct  timezone tz;
    
    gettimeofday(&tv, &tz);
    tv.tv_sec = times;
    FastSecondToDate(&tv, &tz, &systime);

    memset(curTime, 0, sizeof(OOP_DATETIME_S_T));
    curTime->year   = systime.tm_year + 1900; 
    curTime->month  = systime.tm_mon + 1;
    curTime->day    = systime.tm_mday;
    curTime->week   = systime.tm_wday;
    curTime->hour   = systime.tm_hour;
    curTime->minute = systime.tm_min;
    curTime->second = systime.tm_sec;
}

/**********************************************************************
* @name      : min_gather_time_s
* @brief     ：时间转换
* @param[in] ：OOP_DATETIME_S_T *curTime  时间
* @param[out]：
* @return    ：time_t
* @Create    : 王津剑
* @Date      ：2021-6-15
* @Update    :
**********************************************************************/
time_t min_gather_time_s(OOP_DATETIME_S_T *dateTime)
{
    struct tm ptm;

    ptm.tm_year = dateTime->year-1900;
    ptm.tm_mon = dateTime->month-1;
    ptm.tm_mday = dateTime->day;
    ptm.tm_hour = dateTime->hour;
    ptm.tm_min = dateTime->minute;
    ptm.tm_sec = dateTime->second;

    return FastDateToSecond(&ptm);
}

/**********************************************************************
* @name      : min_gather_input_real
* @brief     ：分钟内采集数据输入(实时)
* @param[in] ：int cid                   数据中心客户端
               uint16 type               数据类型
               FROZEN_DATA_FUN func      处理函数
               WRITE_RECORD_T *pData     写入数据
* @param[out]：
* @return    ：0-成功/错误码 
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
int min_gather_input_real(int cid, uint16 type, FROZEN_DATA_FUN func, WRITE_RECORD_T *pData)
{
    MG_REAL_NODE_T  *pDataNode;
    uint8            Addr[6] = {0};
    OOP_DATETIME_S_T datamin;
    time_t           times = 0;

    times = time(NULL);
    if(pData->colStoreTm >= times + 60)
    {
        return MIN_GATHERR_ERR_UNKNOW;
    }
    
    min_gather_time_t(pData->colStoreTm, &datamin);
    datamin.second = 0;

    /* mac地址 */
    if(pData->MAC.nNum >= 6)
    {
        memcpy(Addr, pData->MAC.value, 6);
    }
    else if(pData->MAC.nNum > 0)
    {
        memcpy(Addr + 6 - pData->MAC.nNum, pData->MAC.value, pData->MAC.nNum);
    }
    else
    {
        return MIN_GATHER_ERR_NOTSUPPORT;
    }

    pDataNode = min_gather_get_node(&(g_MINRealData.pNewData), Addr, &datamin);
    if(NULL == pDataNode)
    {
        return MIN_GATHER_ERR_MALLOC;
    }
    
    if(TRUE == g_MINRealAll)
    {
        /* 首次执行期间只记录一小时之前的数据 */
        if(pData->colStoreTm + 3600 < times || pData->colStoreTm < pDataNode->mintimes)
        {
            return MIN_GATHER_SUCCESS;
        }
    }

    return func(cid, type, pData, 0, pDataNode);
}

/**********************************************************************
* @name      : min_gather_input_frozen
* @brief     ：分钟内采集数据输入(冻结)
* @param[in] ：int cid                   数据中心客户端
               uint16 type               数据类型
               FROZEN_DATA_FUN func      处理函数
               WRITE_RECORD_T *pData     写入数据
               uint32 intv               数据间隔(例如，15分钟采集就填写15)
* @param[out]：
* @return    ：0-成功/错误码 
* @Create    : 王津剑
* @Date      ：2021-6-17
* @Update    :
**********************************************************************/
int min_gather_input_frozen(int cid, uint16 type, FROZEN_DATA_FUN func, WRITE_RECORD_T *pData, uint32 intv)
{
    int    ret = 0;
    uint32 i = 0;
    uint8  Addr[6] = {0};
    uint64 mackey = 0;
    RBUserNode *pDataNode = NULL;
    MG_FROZEN_METER_T *pGdata = NULL;
    RBRoot            *chooseRoot = NULL;
    
    /* mac地址 */
    if(pData->MAC.nNum >= 6)
    {
        memcpy(Addr, pData->MAC.value, 6);
    }
    else if(pData->MAC.nNum > 0)
    {
        memcpy(Addr + 6 - pData->MAC.nNum, pData->MAC.value, pData->MAC.nNum);
    }
    else
    {
        return MIN_GATHER_ERR_NOTSUPPORT;
    }
    for(i = 0; i < 6; i++)
    {
        mackey = (mackey << 8) + Addr[i];
    }

    /* 冻结文件选择(即将冻结文件、新文件) */
    if(MIN_GATHER_TYPE_MON == type)
    {
        /* 月冻结特殊，在本月1号的数据中 */
        if(NULL != g_CurFrozen && 1 == g_CurFrozen->fileday.day)
        {
            chooseRoot = g_CurFrozen->meterRoot;
        }
        else if(NULL != g_LastFrozen && 1 == g_LastFrozen->fileday.day)
        {
            chooseRoot = g_LastFrozen->meterRoot;
        }
        else if(NULL != g_OldFrozen && 1 == g_OldFrozen->fileday.day)
        {
            chooseRoot = g_OldFrozen->meterRoot;
        }
        else if(NULL != g_CurFrozen)
        {
            chooseRoot = g_CurFrozen->meterRoot;
        }
        else
        {
            return MIN_GATHER_ERR_MALLOC;
        }
    }
    else
    {
        if(NULL != g_CurFrozen && pData->colStoreTm >= g_CurFrozen->filetime)
        {
            chooseRoot = g_CurFrozen->meterRoot;
        }
        else if(NULL != g_LastFrozen && pData->colStoreTm >= g_LastFrozen->filetime)
        {
            chooseRoot = g_LastFrozen->meterRoot;
        }
        else if(NULL != g_OldFrozen && pData->colStoreTm >= g_OldFrozen->filetime)
        {
            chooseRoot = g_OldFrozen->meterRoot;
        }
        else if(NULL != g_CurFrozen)
        {
            chooseRoot = g_CurFrozen->meterRoot;
        }
        else
        {
            return MIN_GATHER_ERR_MALLOC;
        }
    }
    
    /* 冻结电表结构体获取 */
    pDataNode = rbtree_search(chooseRoot, mackey);
    if(NULL == pDataNode)
    {
        MG_FROZEN_METER_T tGdata;

        memset(&tGdata, 0 ,sizeof(MG_FROZEN_METER_T));
        /* 不存在 创建一个新的 */
        ret = min_gather_create_meter(Addr, &tGdata);
        if(MIN_GATHER_SUCCESS != ret)
        {
            return ret;
        }
        ret = rbtree_insert_node(chooseRoot, mackey, (uint8*)&tGdata, sizeof(MG_FROZEN_METER_T), &pDataNode);
        if(0 != ret)
        {
            return MIN_GATHER_ERR_RBTREE;
        }
    }
    pGdata = (MG_FROZEN_METER_T*)pDataNode->data;
    ret = func(cid, type, pData, intv, pGdata);
    return ret;
}

/**********************************************************************
* @name      : min_gather_input
* @brief     ：分钟内采集数据输入
* @param[in] ：int cid                   数据中心客户端
               WRITE_RECORD_T *pData     写入数据
               uint32 intv               数据间隔(例如，15分钟采集就填写15)
* @param[out]：
* @return    ：0-成功/错误码 
* @Create    : 王津剑
* @Date      ：2021-6-15
* @Update    :
**********************************************************************/
int min_gather_input_work(int cid, WRITE_RECORD_T *pData, uint32 intv)
{
    uint16 type = 0;
    int    ret = MIN_GATHER_SUCCESS;
    uint32 OAD;
    RBUserNode *pFunNode = NULL;
    FROZEN_DATA_FUN *pfunc = NULL;
    
    /* 类型判断 */
    switch(pData->recordOAD.road.oadMain.value)
    {
        case 0:          type = MIN_GATHER_TYPE_REAL; break;
        case 0x50020200: type = MIN_GATHER_TYPE_MIN;  break;
        case 0x50030200: type = MIN_GATHER_TYPE_HOUR; break;
        case 0x50040200: type = MIN_GATHER_TYPE_DAY;  break;
        case 0x50060200: type = MIN_GATHER_TYPE_MON;  break;
        default: return MIN_GATHER_ERR_NOTSUPPORT;
    }

    OAD = pData->recordOAD.road.oadCols.oad[0].value;
    pFunNode = rbtree_search(MIN_GATHER_TYPE_REAL == type ? g_pRealFun : g_pFrozenFun, OAD);
    if(NULL == pFunNode)
    {
        return MIN_GATHER_ERR_NOTREG;
    }
    pfunc = (FROZEN_DATA_FUN*)pFunNode->data;
    if(NULL == pfunc)
    {
        return MIN_GATHER_ERR_NOTREG;
    }
    
    if(MIN_GATHER_TYPE_REAL == type)
    {
        /* 实时数据 */
        if(TRUE == g_MINRealWork)
        {
            PTHREAD_LOCK_SAFE(&g_MinReallock);
            ret = min_gather_input_real(cid, type, *pfunc, pData);
            PTHREAD_UNLOCK_SAFE(&g_MinReallock);
        }
    }
    else
    {
        /* 冻结数据 */
        if(TRUE == g_MINFrozenWork)
        {
            PTHREAD_LOCK_SAFE(&g_MinGatherlock);
            ret = min_gather_input_frozen(cid, type, *pfunc, pData, intv);
            PTHREAD_UNLOCK_SAFE(&g_MinGatherlock);
        }
    }
    return ret;
}

/**********************************************************************
* @name      : min_gather_input_initfrozen
* @brief     ：分钟内采集数据输入(冻结，初始化)
* @param[in] ：int cid                   数据中心客户端
               uint16 type               数据类型
               FROZEN_DATA_FUN func      处理函数
               WRITE_RECORD_T *pData     写入数据
               uint32 intv               数据间隔(例如，15分钟采集就填写15)
* @param[out]：
* @return    ：0-成功/错误码 
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
int min_gather_input_initfrozen(int cid, uint16 type, FROZEN_DATA_FUN func, WRITE_RECORD_T *pData, uint32 intv)
{
    int    ret = 0;
    uint32 i = 0;
    uint8  Addr[6] = {0};
    uint64 mackey = 0;
    RBUserNode *pDataNode = NULL;
    MG_FROZEN_METER_T *pGdata = NULL;
    RBRoot            *chooseRoot = NULL;

    /* 冻结文件选择 */
    if(MIN_GATHER_TYPE_MON == type)
    {
        /* 月冻结特殊，在本月1号的数据中 */
        for(i = 0; i < g_InitFrozenNum; i++)
        {
            if(NULL != g_InitFrozen[i] && 1 == g_InitFrozen[i]->fileday.day && 
               pData->colEndTm >= g_InitFrozen[i]->filetime)
            {
                chooseRoot = g_InitFrozen[i]->meterRoot;
            }
        }
    }
    else
    {
        for(i = 0; i < g_InitFrozenNum; i++)
        {
            if(NULL != g_InitFrozen[i] && pData->colStoreTm >= g_InitFrozen[i]->filetime)
            {
                chooseRoot = g_InitFrozen[i]->meterRoot;
            }
        }
    }
    if(NULL == chooseRoot)
    {
        return MIN_GATHER_ERR_MALLOC;
    }

    /* mac地址 */
    if(pData->MAC.nNum >= 6)
    {
        memcpy(Addr, pData->MAC.value, 6);
    }
    else if(pData->MAC.nNum > 0)
    {
        memcpy(Addr + 6 - pData->MAC.nNum, pData->MAC.value, pData->MAC.nNum);
    }
    else
    {
        return MIN_GATHER_ERR_NOTSUPPORT;
    }
    for(i = 0; i < 6; i++)
    {
        mackey = (mackey << 8) + Addr[i];
    }
    
    /* 冻结电表结构体获取 */
    pDataNode = rbtree_search(chooseRoot, mackey);
    if(NULL == pDataNode)
    {
        MG_FROZEN_METER_T tGdata;

        memset(&tGdata, 0, sizeof(MG_FROZEN_METER_T));
        /* 不存在 创建一个新的 */
        ret = min_gather_create_meter(Addr, &tGdata);
        if(MIN_GATHER_SUCCESS != ret)
        {
            return ret;
        }
        ret = rbtree_insert_node(chooseRoot, mackey, (uint8*)&tGdata, sizeof(MG_FROZEN_METER_T), &pDataNode);
        if(0 != ret)
        {
            return MIN_GATHER_ERR_RBTREE;
        }
    }
    pGdata = (MG_FROZEN_METER_T*)pDataNode->data;
    ret = func(cid, type, pData, intv, pGdata);
    return ret;
}

/**********************************************************************
* @name      : min_gather_input_init
* @brief     ：分钟内采集数据输入 初始化过程
* @param[in] ：int cid                   数据中心客户端
               WRITE_RECORD_T *pData     写入数据
               uint32 intv               数据间隔(例如，15分钟采集就填写15)
* @param[out]：
* @return    ：0-成功/错误码 
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
int min_gather_input_init(int cid, WRITE_RECORD_T *pData, uint32 intv)
{
    uint16 type = 0;
    int    ret = 0;
    uint32 OAD;
    RBUserNode *pFunNode = NULL;
    FROZEN_DATA_FUN *pfunc = NULL;
    
    /* 类型判断 */
    switch(pData->recordOAD.road.oadMain.value)
    {
        case 0:          type = MIN_GATHER_TYPE_REAL; break;
        case 0x50020200: type = MIN_GATHER_TYPE_MIN;  break;
        case 0x50030200: type = MIN_GATHER_TYPE_HOUR; break;
        case 0x50040200: type = MIN_GATHER_TYPE_DAY;  break;
        case 0x50060200: type = MIN_GATHER_TYPE_MON;  break;
        default: return MIN_GATHER_ERR_NOTSUPPORT;
    }

    OAD = pData->recordOAD.road.oadCols.oad[0].value;
    pFunNode = rbtree_search(MIN_GATHER_TYPE_REAL == type ? g_pRealFun : g_pFrozenFun, OAD);
    if(NULL == pFunNode)
    {
        return MIN_GATHER_ERR_NOTREG;
    }
    pfunc = (FROZEN_DATA_FUN*)pFunNode->data;
    if(NULL == pfunc)
    {
        return MIN_GATHER_ERR_NOTREG;
    }
    
    if(MIN_GATHER_TYPE_REAL == type)
    {
        /* 实时数据 */
        if(TRUE == g_MINRealWork)
        {
            PTHREAD_LOCK_SAFE(&g_MinReallock);
            ret = min_gather_input_real(cid, type, *pfunc, pData);
            PTHREAD_UNLOCK_SAFE(&g_MinReallock);
        }
    }
    else
    {
        /* 冻结数据 */
        if(0 != g_InitFrozenNum || pData->colEndTm >= g_InitFrozen[0]->filetime)
        {
            PTHREAD_LOCK_SAFE(&g_MinGatherlock);
            ret = min_gather_input_initfrozen(cid, type, *pfunc, pData, intv);
            PTHREAD_UNLOCK_SAFE(&g_MinGatherlock);
        }
    }
    return ret;
}

/**********************************************************************
* @name      : min_gather_input
* @brief     ：分钟内采集数据输入
* @param[in] ：int cid                   数据中心客户端
               WRITE_RECORD_T *pData     写入数据
               uint32 intv               数据间隔(例如，15分钟采集就填写15)
* @param[out]：
* @return    ：0-成功/错误码 
* @Create    : 王津剑
* @Date      ：2021-6-15
* @Update    :
**********************************************************************/
int min_gather_input(int cid, WRITE_RECORD_T *pData, uint32 intv)
{
    WRITE_RECORD_T tData;

    /* 数据中心的时间都是减了8小时 */
    memcpy(&tData, pData, sizeof(WRITE_RECORD_T));
    tData.colStartTm = tData.colStartTm > 28800 ? tData.colStartTm - 28800 : tData.colStartTm;
    tData.colEndTm = tData.colEndTm > 28800 ? tData.colEndTm - 28800 : tData.colEndTm;
    tData.colStoreTm = tData.colStoreTm > 28800 ? tData.colStoreTm - 28800 : tData.colStoreTm;
    
    /* 开始工作 */
    if(FALSE == g_InitDataFlag)
    {
        return min_gather_input_work(cid, &tData, intv);
    }
    else
    {
        /* 数据初始化阶段 恢复重启之前定的数据 */
        return min_gather_input_init(cid, &tData, intv);
    }
}

/**********************************************************************
* @name      : min_gather_scan
* @brief     ：文件生成扫描
* @param[in] ：void *pArg   线程入参
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-21
* @Update    : 
**********************************************************************/
void *min_gather_scan(void *pArg)
{
    time_t           times = 0;
    time_t           oldtimes = 0;
    OOP_DATETIME_S_T curtime;
    OOP_DATETIME_S_T oldtime;
    MG_FROZEN_T     *storefile = NULL;
    time_t           changetimes = 0;    //过日时间
    OOP_DATETIME_S_T lastfileday;

    prctl(PR_SET_NAME, "MINGATHER");
    oldtimes = time(NULL);
    min_gather_time_t(oldtimes, &oldtime);
    while(1)
    {
        times = time(NULL);
        min_gather_time_t(times, &curtime);

        /* 分钟上报实时数据 */
        if(TRUE == g_MINRealWork && FALSE == g_MINRealAll)
        {
            /* 首次执行或者过小时 */
            if(TRUE == g_MINRealData.bFirst || (curtime.hour != oldtime.hour || curtime.day != oldtime.day || 
               curtime.month != oldtime.month || curtime.year != oldtime.year))
            {
                pthread_mutex_lock(&g_MinReallock);
                min_gather_real_all(&(g_MINRealData.pAllData), &(g_MINRealData.pNewData));
                g_MINRealData.bFirst = FALSE;
                min_gather_real_store(g_MinGatherAddr, MINGatherDir, g_MINRealData.pAllData, &oldtime);
                min_gather_destroy_allnode(&(g_MINRealData.pAllData));
                g_MINRealData.pAllData = NULL;
                pthread_mutex_unlock(&g_MinReallock);

                /* 删除2h之前的文件 */
                min_gather_real_delete(g_MinGatherAddr, MINGatherDir, 2);
            }
            else if(curtime.minute != oldtime.minute)
            {
                /* 过分 */
                pthread_mutex_lock(&g_MinReallock);
                min_gather_real_store(g_MinGatherAddr, MINGatherDir, g_MINRealData.pNewData, &oldtime);
                min_gather_real_all(&(g_MINRealData.pAllData), &(g_MINRealData.pNewData));
                pthread_mutex_unlock(&g_MinReallock);
            }
        }

        /* 日冻结数据上报 */
        if(FALSE == g_MINFrozenWork || TRUE == g_InitDataFlag)
        {
            goto goagain;
        }

        /* 过日生成g_LastFrozen */
        if(curtime.day != oldtime.day || curtime.month != oldtime.month || curtime.year != oldtime.year)
        {
            pthread_mutex_lock(&g_MinGatherlock);
            storefile = g_OldFrozen;
            g_OldFrozen = g_LastFrozen;
            g_LastFrozen = g_CurFrozen;
            g_CurFrozen = min_gather_create_frozen(times);
            pthread_mutex_unlock(&g_MinGatherlock);
            changetimes = times;
        }

        if(storefile)
        {
            /* 写历史文件 */
            lastfileday = storefile->fileday;
            min_gather_forzen_store(g_MinGatherAddr, MINGatherDir, storefile);
            storefile = NULL;
        }

        /* 文件生成 */
        if(times - changetimes > 300)
        {
            /* 过日后等5分钟, 采集完成相关数据 */
            if((curtime.hour * 60 + curtime.minute) >= g_WriteFileMin && NULL == storefile)
            {
                pthread_mutex_lock(&g_MinGatherlock);
                if(NULL != g_OldFrozen)
                {
                    storefile = g_OldFrozen;
                    g_OldFrozen = NULL;
                }
                else if(NULL != g_LastFrozen)
                {
                    storefile = g_LastFrozen;
                    g_LastFrozen = NULL;
                }
                pthread_mutex_unlock(&g_MinGatherlock);
            }
        }
        if(storefile)
        {
            /* 写文件 */
            lastfileday = storefile->fileday;
            min_gather_forzen_store(g_MinGatherAddr, MINGatherDir, storefile);
            storefile = NULL;
        }

        /* 文件压缩 */
        if(times - changetimes > 360)
        {
            /* 过日后等6分钟, 压缩文件 */
            if((curtime.hour * 60 + curtime.minute) >= g_ZipFileMin)
            {
                /* 压缩文件 */
                min_gather_forzen_zip(MINGatherDir, &lastfileday);
                changetimes = times + 86400;
            }
            
        }

goagain:
        oldtime = curtime;
        oldtimes = times;
        usleep(100000);
    }

    return 0;
}

/**********************************************************************
* @name      : min_gather_init
* @brief     ：分钟采集初始化 
* @param[in] ：MIN_GATHER_FUN_T *frofun  冻结数据处理函数
               uint32 fronum             冻结数据处理函数数量
               MIN_GATHER_FUN_T *realfun 实时数据处理函数
               uint32 realnum            实时数据处理函数数量
               uint8  timemin            冻结时间(0点timemin分, 最大59)
               uint8  zipmin             压缩时间(0点zipmin压缩，zipmin > timemin, 0无效，最大60)
* @param[out]：
* @return    ：0-成功/错误码 
* @Create    : 王津剑
* @Date      ：2021-6-15
* @Update    :
**********************************************************************/
int min_gather_init(MIN_GATHER_FUN_T *frofun, uint32 fronum, MIN_GATHER_FUN_T *realfun, uint32 realnum, uint8 timemin, 
                    uint8 zipmin)
{
    uint32 i = 0;
    RBUserNode *pUserNode = NULL;
    pthread_t  threadid;
    
    if(NULL == frofun || 0 == fronum || NULL == realfun || 0 == realnum)
    {
        return MIN_GATHER_ERR_INPUT;
    }

    /* 红黑树初始化 */
    g_pFrozenFun = rbtree_create();
    if(NULL == g_pFrozenFun)
    {
        return MIN_GATHER_ERR_MALLOC;
    }
    for(i = 0; i < fronum; i++)
    {
        rbtree_insert_node(g_pFrozenFun, frofun[i].OAD, (uint8 *)&(frofun[i].func), sizeof(FROZEN_DATA_FUN), &pUserNode);
    }

    g_pRealFun = rbtree_create();
    if(NULL == g_pRealFun)
    {
        rbtree_destroy_all(g_pFrozenFun);
        return MIN_GATHER_ERR_MALLOC;
    }
    for(i = 0; i < realnum; i++)
    {
        rbtree_insert_node(g_pRealFun, realfun[i].OAD, (uint8 *)&(realfun[i].func), sizeof(FROZEN_DATA_FUN), &pUserNode);
    }

    if(zipmin <= timemin)
    {
        zipmin = timemin + 1;
    }
    g_WriteFileMin = MIN(59, timemin);
    g_ZipFileMin = MIN(60, zipmin);
    pthread_mutex_init(&g_MinGatherlock, NULL);

    g_MINRealData.pAllData = NULL;
    g_MINRealData.pNewData = NULL;
    pthread_mutex_init(&g_MinReallock, NULL);

    /* 启动线程 */
    pthread_create(&threadid, NULL, min_gather_scan, NULL);
    sleep(1);
    return MIN_GATHER_SUCCESS;
}

/**********************************************************************
* @name      : min_gather_set_file
* @brief     ：设置文件路径
* @param[in] ：char *filedir             文件存储路径
               uint8 *addr               终端地址(6个字节)
* @param[out]：
* @return    ：0-成功/错误码 
* @Create    : 王津剑
* @Date      ：2021-6-15
* @Update    :
**********************************************************************/
void min_gather_set_file(char *filedir, uint8 *addr)
{
    int len = 0;
    
    if(NULL == filedir || NULL == addr)
    {
        return;
    }

    len = strlen(filedir);
    if(0 == len)
    {
        return;
    }
    memcpy(MINGatherDir, filedir, len);
    if('/' == filedir[len - 1])
    {
        MINGatherDir[len] = 0;
    }

    memcpy(g_MinGatherAddr, addr, 6);
}

/**********************************************************************
* @name      : min_gather_prepare
* @brief     ：分钟内采集数据整理(用于APP首次启动后, 从数据中心获取数据后，生成增量JSON数据)
* @param[in] ：uint8 flag    1-开始(将数据中心所有数据读出，调用min_gather_input写入); 0-停止
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-21
* @Update    :
**********************************************************************/
int min_gather_prepare(uint8 flag)
{
    time_t curtimes = 0;
    OOP_DATETIME_S_T curtime;

    curtimes = time(NULL);
    min_gather_time_t(curtimes, &curtime);

    /* 冻结数据初始化设置 */
    if(TRUE == flag)
    {
        uint32 times = 0;
        uint32 storetimes = 0;
        uint32 starttimes = 0;
        uint16 num = 0;
        OOP_DATETIME_S_T oldtime;
        
        /* 上次APP运行打开了功能，才可以执行恢复 */
        if(0 != min_gather_forzen_getflag(MINGatherDir, &times))
        {
            /* 根据当前时间与最近保存时间判断 */
            storetimes = min_gather_forzen_getstore(MINGatherDir);
            if(storetimes <= times)
            {
                starttimes = times;
            }
            else
            {
                starttimes = storetimes + 24 * 3600;
            }
            if(curtimes < starttimes)
            {
                return MIN_GATHER_SUCCESS;
            }

            num = (curtimes - starttimes) / (24 * 3600) + 2;  //取最大缓存
            g_InitFrozen = (MG_FROZEN_T **)malloc(num * sizeof(MG_FROZEN_T*));
            if(NULL == g_InitFrozen)
            {
                return MIN_GATHER_ERR_MALLOC;
            }
            memset(g_InitFrozen, 0, num * sizeof(MG_FROZEN_T*));
            
            /* 缓存生成 */
            storetimes = starttimes;
            g_InitFrozenNum = 0;
            while(g_InitFrozenNum < num)
            {
                g_InitFrozen[g_InitFrozenNum] = min_gather_create_frozen(storetimes);
                g_InitFrozenNum++;
                min_gather_time_t(starttimes, &oldtime);
                if(oldtime.year == curtime.year || oldtime.month == curtime.month || oldtime.day == curtime.day)
                {
                    break;
                }
                starttimes += 24 * 3600;
            }
            g_InitFrozen[0]->filetime = starttimes;
            g_InitDataFlag = TRUE;
        }
    }
    else
    {
        uint32 i = 0;
        OOP_DATETIME_S_T lastfileday;
        OOP_DATETIME_S_T oldtime;
        
        g_InitDataFlag = FALSE;
        curtimes -= 24 * 3600;
        min_gather_time_t(curtimes, &oldtime);

        /* 将没有保存的数据全部保存 */
        for(i = 0; i < g_InitFrozenNum; i++)
        {
            lastfileday = g_InitFrozen[i]->fileday;
            /* 判断是否保存 */
            if(lastfileday.year == curtime.year || lastfileday.month == curtime.month || lastfileday.day == curtime.day)
            {
                g_CurFrozen = g_InitFrozen[i];
            }
            else if(lastfileday.year == oldtime.year || lastfileday.month == oldtime.month || lastfileday.day == oldtime.day)
            {
                g_LastFrozen = g_InitFrozen[i];
            }
            else
            {
                min_gather_forzen_store(g_MinGatherAddr, MINGatherDir, g_InitFrozen[i]);
                min_gather_forzen_zip(MINGatherDir, &lastfileday);
            }
            g_InitFrozen[i] = NULL;
        }
        free(g_InitFrozen);
    }
    return MIN_GATHER_SUCCESS;
}

/**********************************************************************
* @name      : min_gather_frozen_start
* @brief     ：分钟内采冻结开始
* @param[in] ：
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-21
* @Update    :
**********************************************************************/
int min_gather_frozen_start()
{
    /* 初始化数据 */
    time_t times = 0;
    times = time(NULL);
    
    if(NULL == g_CurFrozen)
    {
        g_CurFrozen = min_gather_create_frozen(times);
    }
    min_gather_forzen_flag(MINGatherDir, 1);
    g_MINFrozenWork = TRUE;

    return MIN_GATHER_SUCCESS;
}

/**********************************************************************
* @name      : min_gather_frozen_stop
* @brief     ：分钟内采停止
* @param[in] ：uint8 bstore   是否需要生成已经存在的内容
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-21
* @Update    :
**********************************************************************/
void min_gather_frozen_stop(uint8 bstore)
{
    g_MINFrozenWork = FALSE;
    min_gather_forzen_flag(MINGatherDir, 0);
    sleep(5);
    if(TRUE == bstore)
    {
        OOP_DATETIME_S_T lastfileday;
        if(g_OldFrozen)
        {
            min_gather_forzen_store(g_MinGatherAddr, MINGatherDir, g_OldFrozen);
            g_OldFrozen = NULL;
        }
        if(g_LastFrozen)
        {
            min_gather_forzen_store(g_MinGatherAddr, MINGatherDir, g_LastFrozen);
            g_LastFrozen = NULL;
        }
        if(g_CurFrozen)
        {
            lastfileday = g_CurFrozen->fileday;
            min_gather_forzen_store(g_MinGatherAddr, MINGatherDir, g_CurFrozen);
            g_CurFrozen = NULL;
            min_gather_forzen_zip(MINGatherDir, &lastfileday);
        }
    }
    else
    {
        min_gather_destory_frozen(g_CurFrozen);
        g_CurFrozen = NULL;
        min_gather_destory_frozen(g_LastFrozen);
        g_LastFrozen = NULL;
        min_gather_destory_frozen(g_OldFrozen);
        g_OldFrozen = NULL;
    }
}

/**********************************************************************
* @name      : min_gather_real_start
* @brief     ：分钟采集实时数据采集开始
* @param[in] ：uint8 bAllFlag     全量生成标记(TRUE-生成全量标记, FALSE-全量标记完成)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
int min_gather_real_start(uint8 bAllFlag)
{
    PTHREAD_LOCK_SAFE(&g_MinReallock);
    g_MINRealAll = bAllFlag;
    g_MINRealWork = TRUE;
    g_MINRealData.bFirst = TRUE;
    PTHREAD_UNLOCK_SAFE(&g_MinReallock);

    return MIN_GATHER_SUCCESS; 
}

/**********************************************************************
* @name      : min_gather_real_stop
* @brief     ：分钟采集实时数据采集结束
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-28
* @Update    :
**********************************************************************/
void min_gather_real_stop()
{
    PTHREAD_LOCK_SAFE(&g_MinReallock);
    g_MINRealWork = FALSE;
    g_MINRealAll = TRUE;
    g_MINRealData.bFirst = FALSE;

    min_gather_destroy_allnode(&(g_MINRealData.pNewData));
    g_MINRealData.pNewData = NULL;
    min_gather_destroy_allnode(&(g_MINRealData.pAllData));
    g_MINRealData.pAllData = NULL;
    PTHREAD_UNLOCK_SAFE(&g_MinReallock);
}

/**********************************************************************
* @name      : min_gather_insert_item
* @brief     ：分钟采集增加冻结数据
* @param[in] ：void *pGdate              采集数据
               uint16 type               类型(MIN_GATHER_TYPE_E)
               uint32 itemId             数据标识
               uint32 storeTm            存储时间
               uint32 succTm             成功时间
               uint32 intv               间隔
               cJSON *jsonData           单个数据
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-21
* @Update    :
**********************************************************************/
int min_gather_insert_item(void *pGdate, uint16 type, uint32 itemId, uint32 storeTm, uint32 succTm, uint32 intv, 
                           cJSON *jsonData)
{
    MG_FROZEN_METER_T *pMeter = (MG_FROZEN_METER_T *)pGdate;

    if(NULL == pGdate || NULL == jsonData || MIN_GATHER_TYPE_REAL == type)
    {
        return MIN_GATHER_ERR_INPUT;
    }
    if(0 == intv)
    {
        intv = 1;
    }
    return min_gather_frozen_add(pMeter, type, itemId, storeTm, succTm, intv, jsonData);
}

/**********************************************************************
* @name      : min_gather_insert_data
* @brief     ：分钟采集添加实时数据
* @param[in] ：void *pGdate              采集数据
               uint32 type               数据标识(MIN_GATHER_OAD_E)
               uint32 storeTm            存储时间
               uint8 *data               数据内容
               uint16 datalen            数据长度
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-6-29
* @Update    :
**********************************************************************/
int min_gather_insert_data(void *pGdate, uint32 type, uint32 storeTm, uint8 *data, uint16 datalen)
{
    MG_REAL_NODE_T  *pDataNode = (MG_REAL_NODE_T *)pGdate;

    return min_gather_real_add(pDataNode, type, storeTm, data, datalen);
}


