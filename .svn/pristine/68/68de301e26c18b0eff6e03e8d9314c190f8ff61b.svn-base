/*
*********************************************************************
* Copyright(C) 2019 南京新联电子股份有限公司
* All rights reserved. 
* @brief：   控制模块 配置参数获取
* @date：    2020-4-3
* @history： 
*********************************************************************
*/
#include "baseCommon.h"
#include "appmsg_struct.h"
#include "appmsg_split.h"
#include "db_api.h"
#include "led.h"
#include "audio.h"

#include "appctrl_comm.h"
#include "appctrl_cfg.h"
#include "framecomm.h"
#include "storage.h"

/*------------------------------OI-----------------------------
功率控制：时段控8103，厂休控8104，营业报停控8105，当前功率下浮控8106；
电量控制：月购电控8108，购电控8107
保电控制：8001
远方控制：8000
告警： 8002、8102
总加组：2301~2308
定值：8100、8110
终端功控时段：8101
事件：功控跳闸记录3200-3305、电控跳闸记录3201-3306、电控告警事件记录3203-3307、遥控跳闸记录3115-330a
其他: 当前套日时段表4016、当前套费率电价4018、继电器输出f205、告警输出f206
------------------------------------------------------------- */
int             g_CtrlDBid = 0; //数据中心客户端

CTRL_ARG_ALL_T  g_CtrlArg; //控制模块基本参数
CTRL_SUBGROUP_T g_SubGroup[OOP_MAX_TG]; //总加组配置及其数据
CTRL_EVENT_T    g_CtrlEvent;//控制事件
CTRL_STATIC_T   g_CtrlStatic;//控制统计 目前没有使用到
OOP_DAYPERIOD_T  g_tDayPeriod; //0x40160200 时段表
OOP_RATETARIFF_T g_tEnPrice;   //0x40180200 费率电价
OOP_WARNINGTIME_T g_tWarnTime; //0xf2060400 允许告警时段
OOP_SWITCHOUT_T g_tCtrlSwitch; //0xf2050200 继电器

pthread_mutex_t    g_fCtrlDatalock;	
uint8              g_CtrlInitData = FALSE;

pthread_mutex_t    g_fCtrlArglock;	
uint8              g_CtrlInitArg = FALSE;

CTRLDB_NOTIFY_T g_CtrlDBNotify;

uint8           g_CtrlEffect = 0; //1-投入, 2-解除, 0-无状态
uint8           g_EffectType = CTRL_NULL_TYPE;
uint16          g_EffectInfo = 0;

/* 获取队列 */
uint32 appctrl_read_oad_notify(uint16 *infonum)
{
    uint32 headOad = 0;
    if(NULL == infonum)
    {
        return headOad;
    }

    PTHREAD_LOCK_SAFE(&g_CtrlDBNotify.f_lock);  /*锁缓冲区*/
    /*若队列中有数据，则取到buf中，解开锁*/
    if(0 < g_CtrlDBNotify.num)
    {
        headOad = g_CtrlDBNotify.OAD[0];
        memmove(&(g_CtrlDBNotify.OAD[0]), &(g_CtrlDBNotify.OAD[1]), (g_CtrlDBNotify.num - 1)*sizeof(uint32));
        g_CtrlDBNotify.OAD[g_CtrlDBNotify.num-1] = 0;

        *infonum = g_CtrlDBNotify.infonum[0];
        memmove(&(g_CtrlDBNotify.infonum[0]), &(g_CtrlDBNotify.infonum[1]), (g_CtrlDBNotify.num - 1)*sizeof(uint16));
        g_CtrlDBNotify.infonum[g_CtrlDBNotify.num-1] = 0;

        g_CtrlDBNotify.num--;
    }
    PTHREAD_UNLOCK_SAFE(&g_CtrlDBNotify.f_lock);
    return headOad;
}

/* 插入队列 */
int appctrl_write_oad_notify(uint32 Oad, uint16 infoNum)
{
    if(0 == Oad || CTRL_DB_QUEUE_SIZE <= g_CtrlDBNotify.num)
    {
        return -1;
    }

    PTHREAD_LOCK_SAFE(&g_CtrlDBNotify.f_lock);  /*锁缓冲区*/

    g_CtrlDBNotify.OAD[g_CtrlDBNotify.num] = Oad;
    g_CtrlDBNotify.infonum[g_CtrlDBNotify.num] = infoNum;

    g_CtrlDBNotify.num++;
    
    PTHREAD_UNLOCK_SAFE(&g_CtrlDBNotify.f_lock);
    return 0;
}

/**********************************************************************
* @name      : ctrl_data_init
* @brief     ：数据中心初始化
* @param[out]：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-11
* @Update    :
**********************************************************************/
void ctrl_data_init()
{
    pthread_mutex_init(&g_fCtrlDatalock, NULL);
}

/**********************************************************************
* @name      : ctrl_set_dataflag
* @brief     ：数据中心初始化
* @param[out]：uint8 logicid  逻辑设备号
               uint8 flag     初始化标记
               uint8 flag     TRUE-FALSE
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-11
* @Update    :
**********************************************************************/
void ctrl_set_dataflag(uint8 logicid, uint16 classTag, uint8 flag)
{
    if(0 == logicid && (0 == classTag || CLASS_DATA_NORMAL == classTag))
    {
        PTHREAD_LOCK_SAFE(&g_fCtrlDatalock);
        g_CtrlInitData = flag;
        PTHREAD_UNLOCK_SAFE(&g_fCtrlDatalock);
    }
}

/**********************************************************************
* @name      : ctrl_get_dataflag
* @brief     ：数据中心初始化
* @param[out]：
* @param[out]：
* @return    ：初始化标记
* @Create    : 王津剑
* @Date      ：2020-5-11
* @Update    :
**********************************************************************/
uint8 ctrl_get_dataflag()
{
    uint8 flag = 0;
    PTHREAD_LOCK_SAFE(&g_fCtrlDatalock);
    flag = g_CtrlInitData;
    PTHREAD_UNLOCK_SAFE(&g_fCtrlDatalock);
    return flag;
}

/**********************************************************************
* @name      : ctrl_arg_init
* @brief     ：数据中心初始化
* @param[out]：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-11
* @Update    :
**********************************************************************/
void ctrl_arg_init()
{
    pthread_mutex_init(&g_fCtrlArglock, NULL);
}

/**********************************************************************
* @name      : ctrl_set_argflag
* @brief     ：数据中心初始化
* @param[out]：uint8 logicid  逻辑设备号
               uint8 flag     初始化标记
               uint8 flag     TRUE-FALSE
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-11
* @Update    :
**********************************************************************/
void ctrl_set_argflag(uint8 logicid, uint16 classTag, uint8 flag)
{
    if(0 == logicid && (0 == classTag || CLASS_DATA_INIT == classTag))
    {
        pthread_mutex_lock(&g_fCtrlArglock);
        g_CtrlInitArg = flag;
        pthread_mutex_unlock(&g_fCtrlArglock);
    }
}

/**********************************************************************
* @name      : ctrl_get_argflag
* @brief     ：数据中心初始化
* @param[out]：
* @param[out]：
* @return    ：初始化标记
* @Create    : 王津剑
* @Date      ：2020-5-11
* @Update    :
**********************************************************************/
uint8 ctrl_get_argflag()
{
    uint8 flag = 0;
    pthread_mutex_lock(&g_fCtrlArglock);
    flag = g_CtrlInitArg;
    pthread_mutex_unlock(&g_fCtrlArglock);
    return flag;
}

/**********************************************************************
* @name      : appctrl_get_neweffect
* @brief     ：获取新生效的控制方案
* @param[in] ：
* @param[out]：uint8 *ctrltype   控制方案
               uint16 *infonum   信息点号
               int *index        语音序号
* @return    ：状态
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
uint8 appctrl_get_neweffect(uint8 *ctrltype, uint16 *infonum, int *index)
{
    *ctrltype = g_EffectType;
    *infonum = g_EffectInfo;
    *index = 0;

    if(0 == g_CtrlEffect)
    {
        return g_CtrlEffect;
    }
    
    switch(g_EffectType)
    {
        case CTRL_DOWN_TYPE: *index = 1 == g_CtrlEffect ? EMAUDIO_INDEX_13 : EMAUDIO_INDEX_14; break;
        case CTRL_STOP_TYPE: *index = 1 == g_CtrlEffect ? EMAUDIO_INDEX_7 : EMAUDIO_INDEX_8; break;
        case CTRL_REST_TYPE: *index = 1 == g_CtrlEffect ? EMAUDIO_INDEX_5 : EMAUDIO_INDEX_6; break;
        case CTRL_PERIOD_TYPE: *index = 1 == g_CtrlEffect ? EMAUDIO_INDEX_3 : EMAUDIO_INDEX_4; break;
        case CTRL_BUY_TYPE: *index = 1 == g_CtrlEffect ? EMAUDIO_INDEX_11 : EMAUDIO_INDEX_12; break;
        case CTRL_MONTH_TYPE: *index = 1 == g_CtrlEffect ? EMAUDIO_INDEX_9 : EMAUDIO_INDEX_10; break;
        default: break;
    }
    return g_CtrlEffect;
}

/**********************************************************************
* @name      : appctrl_no_neweffect
* @brief     ：清空生效
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-4
* @Update    :
**********************************************************************/
void appctrl_no_neweffect()
{
    g_CtrlEffect = 0;
}

/**********************************************************************
* @name      : appctrl_oad_notify
* @brief     ：更新oad
* @param[in] ：int clientid    socket句柄
               uint8 logicId   逻辑地址
               uint16 infoNum  信息点号
               uint32 value    oad值
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
int appctrl_oad_notify(int clientid, uint8 logicId, uint16 infoNum, uint32 value)
{
    return appctrl_write_oad_notify(value, infoNum);
}

/**********************************************************************
* @name      : appctrl_oad_notify_init
* @brief     ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
void appctrl_oad_notify_init()
{
    pthread_mutex_init(&g_CtrlDBNotify.f_lock, NULL);
}

/**********************************************************************
* @name      : appctrl_get_oad
* @brief     ：读取OAD
* @param[in] ：int cid
               uint32 uOAD
               uint16 infonum
               uint8 *pBuf
               uint32 bufLen
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-5
* @Update    :
**********************************************************************/
int appctrl_get_oad(int cid, uint32 uOAD, uint16 infonum, void *pBuf, uint32 bufLen)
{
    NOMAL_OAD_T tOAD;
    int         ret = 0;
    uint32      readLen = 0;
    
    if(cid <= 0 || NULL == pBuf || 0 == bufLen)
    {
        return -1;
    }
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = uOAD;
    tOAD.infoNum = infonum;
    ret = db_read_nomal(cid, &tOAD, bufLen, (uint8 *)pBuf, &readLen);
    if(0 != ret)
    {
        return ret;
    }
    if(readLen != bufLen)
    {
        return -150;
    }
    return 0;
}

/**********************************************************************
* @name      : appctrl_write_oad
* @brief     ：写入OAD
* @param[in] ：int cid
               uint32 uOAD
               uint16 infonum
               uint8 *pData
               uint32 len
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
int appctrl_write_oad(int cid, uint32 uOAD, uint16 infonum, void *pData, uint32 len)
{
    NOMAL_OAD_T tOAD;
    
    if(cid <= 0 || NULL == pData || 0 == len)
    {
        return -1;
    }
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = uOAD;
    tOAD.infoNum = infonum;
    tOAD.classtag = CLASS_DATA_NORMAL;

    return db_write_nomal(cid, &tOAD, (uint8 *)pData, len);
}

/**********************************************************************
* @name      : appctrl_get_arg
* @brief     ：获取数据中心关于控制的配置参数
* @param[in] ：int cid  数据中心ID
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-3
* @Update    :
**********************************************************************/
void appctrl_data_init()
{
    memset(&g_CtrlArg, 0, sizeof(CTRL_ARG_ALL_T));
    memset(g_SubGroup, 0, OOP_MAX_TG * sizeof(CTRL_SUBGROUP_T));
    memset(&g_CtrlEvent, 0 ,sizeof(CTRL_EVENT_T));
    memset(&g_CtrlStatic, 0, sizeof(CTRL_STATIC_T));
    memset(&g_tDayPeriod, 0, sizeof(OOP_DAYPERIOD_T));
    memset(&g_tEnPrice, 0, sizeof(OOP_RATETARIFF_T));
    memset(&g_tWarnTime, 0, sizeof(OOP_WARNINGTIME_T));
    memset(&g_tCtrlSwitch, 0, sizeof(OOP_SWITCHOUT_T));
}

/**********************************************************************
* @name      : appctrl_get_argcopy
* @brief     ：获取参数地址
* @param[in] ：
* @param[out]：CTRL_ARG_ALL_T **ppCtrlArg
               CTRL_SUBGROUP_T **ppSubGroup
               CTRL_STATIC_T **ppCtrlStatic
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
void appctrl_get_argcopy(CTRL_ARG_ALL_T **ppCtrlArg, CTRL_SUBGROUP_T **ppSubGroup, CTRL_STATIC_T **ppCtrlStatic)
{
    if(NULL == ppCtrlArg || NULL == ppSubGroup || NULL == ppCtrlStatic)
    {
        return;
    }
    *ppCtrlArg = &g_CtrlArg;
    *ppSubGroup = g_SubGroup;
    *ppCtrlStatic = &g_CtrlStatic;
}


/**********************************************************************
* @name      : appctrl_get_subgroup_arg
* @brief     ：获取数据中心关于控制总加组的配置参数
* @param[in] ：int cid  数据中心ID
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-5
* @Update    :
**********************************************************************/
void appctrl_get_subgroup_arg(int cid)
{
    uint32           i;
    CTRL_SUBGROUP_T *ptSubGroup = NULL;
    uint32           offset = 0;
    int              ret = 0;

    /* 获取总加组配置 */
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        ptSubGroup = &(g_SubGroup[i]);
        ptSubGroup->storeNo = 0xffffffff;  //前1min电量需要首次设置为全f
        offset     = i * 0x10000;

        ret = appctrl_get_oad(cid, 0x23010200 + offset, 0, &(ptSubGroup->argList), sizeof(OOP_TGSETS_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Get subgroup 0x%x failed, ret %d!\n", 0x23010200 + offset, ret);
            continue;
        }

        appctrl_get_oad(cid, 0x23010300 + offset, 0, &(ptSubGroup->actPower), sizeof(int64));
        appctrl_get_oad(cid, 0x23010400 + offset, 0, &(ptSubGroup->ractPower), sizeof(int64));
        appctrl_get_oad(cid, 0x23010500 + offset, 0, &(ptSubGroup->aveActPower), sizeof(int64));
        appctrl_get_oad(cid, 0x23010600 + offset, 0, &(ptSubGroup->aveRactPower), sizeof(int64));
        appctrl_get_oad(cid, 0x23010700 + offset, 0, &(ptSubGroup->dayActEn), sizeof(OOP_HENERGYS_T));
        appctrl_get_oad(cid, 0x23010800 + offset, 0, &(ptSubGroup->dayRactEn), sizeof(OOP_HENERGYS_T));
        appctrl_get_oad(cid, 0x23010900 + offset, 0, &(ptSubGroup->monActEn), sizeof(OOP_HENERGYS_T));
        appctrl_get_oad(cid, 0x23010a00 + offset, 0, &(ptSubGroup->monRactEn), sizeof(OOP_HENERGYS_T));
        appctrl_get_oad(cid, 0x23010b00 + offset, 0, &(ptSubGroup->leftEn), sizeof(int64));
        appctrl_get_oad(cid, 0x23010c00 + offset, 0, &(ptSubGroup->downActPower), sizeof(int64));
        appctrl_get_oad(cid, 0x23010d00 + offset, 0, &(ptSubGroup->pTime), sizeof(uint8));

        /* 控制配置 */
        ret = appctrl_get_oad(cid, 0x23010e00 + offset, 0, &(ptSubGroup->ctrlPwTurn), sizeof(OOP_BITSTR8_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Get subgroup ctrl power turn 0x%x failed, ret %d!\n", 0x23011400 + offset, ret);
        }
        ret = appctrl_get_oad(cid, 0x23010f00 + offset, 0, &(ptSubGroup->ctrlEcTurn), sizeof(OOP_BITSTR8_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Get subgroup ctrl electricity turn 0x%x failed, ret %d!\n", 0x23011500 + offset, ret);
        }
        ret = appctrl_get_oad(cid, 0x23011000 + offset, 0, &(ptSubGroup->ctrlStatus), sizeof(OOP_TGSETTING_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Get subgroup ctrl status 0x%x failed, ret %d!\n", 0x23011600 + offset, ret);
        }
        ret = appctrl_get_oad(cid, 0x23011100 + offset, 0, &(ptSubGroup->curCtrlStatus), sizeof(OOP_TGCURSTATE_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Get subgroup cur ctrl status 0x%x failed, ret %d!\n", 0x23011700 + offset, ret);
        }
    }
}

/**********************************************************************
* @name      : appctrl_get_ctrl_arg
* @brief     ：获取数据中心关于控制的配置参数
* @param[in] ：int cid  数据中心ID
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-5
* @Update    :
**********************************************************************/
void appctrl_get_ctrl_arg(int cid)
{
    int    ret = 0;
    uint16 i = 0;

    /* 1 远程控制参数 */
    CTRL_REMOTE_ARG_T *ptRemote = &(g_CtrlArg.tRemote);
    ret = appctrl_get_oad(cid, 0x80000200, 0, &(ptRemote->param), sizeof(OOP_TELECON_PARAM_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get remote param 0x%x failed, ret %d!\n", 0x80000200, ret);
    }
    /*ret = appctrl_get_oad(cid, 0x80000300, 0, &(ptRemote->relayStatus), sizeof(OOP_BITSTR8_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get remote relay status 0x%x failed, ret %d!\n", 0x80000300, ret);
    }*/
    ret = appctrl_get_oad(cid, 0x80000400, 0, &(ptRemote->alarmstatus), sizeof(ptRemote->alarmstatus));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get remote ctrl status 0x%x failed, ret %d, set zero!\n", 0x80000400, ret);
        ptRemote->alarmstatus.bitValue = 0;
        appctrl_write_oad(cid, 0x80000400, 0, &(ptRemote->alarmstatus), sizeof(ptRemote->alarmstatus));
    }
    ret = appctrl_get_oad(cid, 0x80000500, 0, &(ptRemote->orderstatus), sizeof(ptRemote->orderstatus));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get remote order status 0x%x failed, ret %d, set zero!\n", 0x80000500, ret);
        ptRemote->orderstatus.bitValue = 0;
        appctrl_write_oad(cid, 0x80000500, 0, &(ptRemote->orderstatus), sizeof(ptRemote->orderstatus));
    }

    ret = appctrl_get_oad(cid, 0x80007fff, 0, &(ptRemote->bAlarm), sizeof(uint8));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get remote alarm 0x%x failed, ret %d!\n", 0x80007fff, ret);
    }
    ret = appctrl_get_oad(cid, 0x800081ff, 0, &(ptRemote->trip), sizeof(OOP_TELECON_TRIP_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get remote trip 0x%x failed, ret %d!\n", 0x800081ff, ret);
    }
    ret = appctrl_get_oad(cid, 0x800082ff, 0, &(ptRemote->close), sizeof(OOP_TELECON_CLOSE_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get remote close 0x%x failed, ret %d!\n", 0x800082ff, ret);
    }
    //跳闸合闸判断(重启获取的数据不处理)
    if(0 != ptRemote->trip.nNum)
    {
        uint8 no = 0;
        for(i = 0; i < ptRemote->trip.nNum; i++)
        {
            no = ptRemote->trip.trip[i].nRelayIdx.nIndex;
            if(0 != no && no <= OOP_MAX_TRIP)
            {
                CTRL_FMT_DEBUG("历史跳闸: 继电器%d, OAD 0x%x, 告警时间 %dmin, 持续时间 %dmin!\n", no, ptRemote->trip.trip[i].nRelayIdx.value, 
                               ptRemote->trip.trip[i].nWarnDelay, ptRemote->trip.trip[i].nTripTime);
            }
        }
    }
    if(0 != ptRemote->close.nNum)
    {
        uint8 no = 0;
        for(i = 0; i < ptRemote->close.nNum; i++)
        {
            no = ptRemote->close.close[i].nRelayIdx.nIndex;
            if(0 != no && no <= OOP_MAX_TRIP)
            {
                CTRL_FMT_DEBUG("历史合闸: 继电器%d, OAD 0x%x!\n", no, ptRemote->close.close[i].nRelayIdx.value);
            }
        }
    }

    /* 2 保电参数 */
    CTRL_PROTECT_ARG_T *ptProtect = &(g_CtrlArg.tProtect);
    ret = appctrl_get_oad(cid, 0x80010200, 0, &(ptProtect->status), sizeof(uint8));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get protect status 0x%x failed, ret %d!\n", 0x80010200, ret);
        ptProtect->status = 255; //先初始化一个无效状态
    }
    if(1 == ptProtect->status)
    {
        set_guard_state(1); //设置状态为 1
        CTRL_FMT_DEBUG("重启之前投入保电\n");
    }
    
    ret = appctrl_get_oad(cid, 0x80010300, 0, &(ptProtect->NMTimer), sizeof(uint16));
    if(0 != ret)
    {
#if CTRL_PTOTECT_OPEN
        ptProtect->NMTimer = 1440; //默认值1440
#endif
        CTRL_FMT_DEBUG("Get protect no master time 0x%x failed, ret %d!\n", 0x80010300, ret);
    }
    CTRL_FMT_DEBUG("无通信自保电时间%dmin\n", ptProtect->NMTimer);
    
    ret = appctrl_get_oad(cid, 0x80010400, 0, &(ptProtect->FMTimer), sizeof(uint16));
    if(0 != ret)
    {
#if CTRL_PTOTECT_OPEN
        ptProtect->FMTimer = 10; //默认值10
#endif
        CTRL_FMT_DEBUG("Get protect first connect master time 0x%x failed, ret %d!\n", 0x80010400, ret);
    }
    CTRL_FMT_DEBUG("上电自保电时间%dmin\n", ptProtect->FMTimer);
    
    ret = appctrl_get_oad(cid, 0x80010500, 0, &(ptProtect->autoprotect), sizeof(OOP_PRTTIME_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get protect period time 0x%x failed, ret %d!\n", 0x80010500, ret);
    }

    /* 3 催费告警 */
    CTRL_CALLFEE_ARG_T *ptCallFee = &(g_CtrlArg.tCallFee);
    ret = appctrl_get_oad(cid, 0x80020200, 0, &(ptCallFee->status), sizeof(uint8));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get callfee status 0x%x failed, ret %d!\n", 0x80020200, ret);
    }
    ret = appctrl_get_oad(cid, 0x80020300, 0, &(ptCallFee->param), sizeof(OOP_CALLFEE_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get callfee param 0x%x failed, ret %d!\n", 0x80020300, ret);
    }
    appctrl_get_oad(cid, 0x80027fff, 0, &(ptCallFee->bEffect), sizeof(uint8));

    /* 4 保安定值 */
    ret = appctrl_get_oad(cid, 0x81000200, 0, &(g_CtrlArg.safevalue), sizeof(int64));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get safevalue 0x%x failed, ret %d!\n", 0x81000200, ret);
        //g_CtrlArg.safevalue = 99900000000LL;
    }
    CTRL_FMT_DEBUG("保安定值%lld!\n", g_CtrlArg.safevalue);

    /* 5 终端功控时段 */
    ret = appctrl_get_oad(cid, 0x81010200, 0, &(g_CtrlArg.tPowconTime), sizeof(OOP_POWCON_TIME_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get powcon time 0x%x failed, ret %d!\n", 0x81010200, ret);
    }
    appctrl_cal_period(&(g_CtrlArg.tPowconTime));

    /* 6 功控告警时间 */
    ret = appctrl_get_oad(cid, 0x81020200, 0, &(g_CtrlArg.tPowconWarn), sizeof(OOP_POWCON_WARNING_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get powwrn 0x%x failed, ret %d!\n", 0x81020200, ret);
    }

    /* 7 时段功控 */
    CTRL_PERIOD_ARG_T *ptPeriod = &(g_CtrlArg.tPeriod);
    ret = appctrl_get_oad(cid, 0x81030200, 0, &(ptPeriod->param), sizeof(OOP_8103_CONFIG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get period param 0x%x failed, ret %d!\n", 0x81030200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x81030300, 0, &(ptPeriod->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_get_oad(cid, 0x81030400, 0, &(ptPeriod->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_get_oad(cid, 0x81030500, 0, &(ptPeriod->alarmStatus), sizeof(OOP_TGSTATES_T));
        for(i = 0; i < OOP_MAX_TG; i++)
        {
            appctrl_get_oad(cid, 0x810306ff, i + 1, &(ptPeriod->bEffect[i]), sizeof(uint8));
            appctrl_get_oad(cid, 0x81037fff, i + 1, &(ptPeriod->project[i]), sizeof(OOP_POWCON_PERIOD_T));
        }
    }

    /* 8 厂休控 */
    CTRL_REST_ARG_T *ptRest = &(g_CtrlArg.tRest);
    ret = appctrl_get_oad(cid, 0x81040200, 0, &(ptRest->param), sizeof(OOP_8104_CONFIG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get rest param 0x%x failed, ret %d!\n", 0x81040200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x81040300, 0, &(ptRest->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_get_oad(cid, 0x81040400, 0, &(ptRest->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_get_oad(cid, 0x81040500, 0, &(ptRest->alarmStatus), sizeof(OOP_TGSTATES_T));
        for(i = 0; i < OOP_MAX_TG; i++)
        {
            appctrl_get_oad(cid, 0x810406ff, i + 1, &(ptRest->bEffect[i]), sizeof(uint8));
        }
    }

    /* 9 营业报停控 */
    CTRL_STOP_ARG_T *ptStop = &(g_CtrlArg.tStop);
    ret = appctrl_get_oad(cid, 0x81050200, 0, &(ptStop->param), sizeof(OOP_8105_CONFIG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get stop param 0x%x failed, ret %d!\n", 0x81050200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x81050300, 0, &(ptStop->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_get_oad(cid, 0x81050400, 0, &(ptStop->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_get_oad(cid, 0x81050500, 0, &(ptStop->alarmStatus), sizeof(OOP_TGSTATES_T));
        for(i = 0; i < OOP_MAX_TG; i++)
        {
            appctrl_get_oad(cid, 0x810506ff, i + 1, &(ptStop->bEffect[i]), sizeof(uint8));
        }
    }

    /* 10 当前功率下浮控 */
    CTRL_DOWN_ARG_T *ptDown = &(g_CtrlArg.tDown);
    appctrl_get_oad(cid, 0x81060300, 0, &(ptDown->effectStatus), sizeof(OOP_TGSTATES_T));
    appctrl_get_oad(cid, 0x81060400, 0, &(ptDown->outputStatus), sizeof(OOP_TURNSTATES_T));
    appctrl_get_oad(cid, 0x81060500, 0, &(ptDown->alarmStatus), sizeof(OOP_TGSTATES_T));
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        appctrl_get_oad(cid, 0x810606ff, i + 1, &(ptDown->bEffect[i]), sizeof(uint8));
        appctrl_get_oad(cid, 0x81067fff, i + 1, &(ptDown->project[i]), sizeof(OOP_POWCON_LOW_T));
    }

    /* 11 购电控 */
    CTRL_BUY_ARG_T *ptBuy = &(g_CtrlArg.tBuy);
    ret = appctrl_get_oad(cid, 0x81070200, 0, &(ptBuy->param), sizeof(OOP_8107_CONFIG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get buy param 0x%x failed, ret %d!\n", 0x81070200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x81070300, 0, &(ptBuy->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_get_oad(cid, 0x81070400, 0, &(ptBuy->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_get_oad(cid, 0x81070500, 0, &(ptBuy->alarmStatus), sizeof(OOP_TGSTATES_T));
        for(i = 0; i < OOP_MAX_TG; i++)
        {
            appctrl_get_oad(cid, 0x810706ff, i + 1, &(ptBuy->bEffect[i]), sizeof(uint8));
        }
    }

    /* 12 月电控 */
    CTRL_MONTH_ARG_T *ptMon = &(g_CtrlArg.tMonth);
    ret = appctrl_get_oad(cid, 0x81080200, 0, &(ptMon->param), sizeof(OOP_8108_CONFIG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get month param 0x%x failed, ret %d!\n", 0x81070200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x81080300, 0, &(ptMon->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_get_oad(cid, 0x81080400, 0, &(ptMon->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_get_oad(cid, 0x81080500, 0, &(ptMon->alarmStatus), sizeof(OOP_TGSTATES_T));
        for(i = 0; i < OOP_MAX_TG; i++)
        {
            appctrl_get_oad(cid, 0x810806ff, i + 1, &(ptMon->bEffect[i]), sizeof(uint8));
        }
    }
}

/**********************************************************************
* @name      : appctrl_get_event_arg
* @brief     ：获取数据中心关于控制事件的配置参数
* @param[in] ：int cid  数据中心ID
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-5
* @Update    :
**********************************************************************/
void appctrl_get_event_arg(int cid)
{
    int              ret = 0;

    /* 1 功控跳闸记录 */
    CTRL_PW_EVENT_T *ptPwEvent = &(g_CtrlEvent.tPwEvent);
    ret = appctrl_get_oad(cid, 0x32000200, 0, &(ptPwEvent->record), sizeof(OOP_PCTRLLOG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get power event 0x%x failed, ret %d!\n", 0x32000200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x32000300, 0, &(ptPwEvent->oadsets), sizeof(OOP_OADS_T));
        appctrl_get_oad(cid, 0x32000400, 0, &(ptPwEvent->curCount), sizeof(uint16));
        appctrl_get_oad(cid, 0x32000500, 0, &(ptPwEvent->maxCount), sizeof(uint16));
        appctrl_get_oad(cid, 0x32000700, 0, &(ptPwEvent->curValue), sizeof(OOP_EVTSTAT_T));
        appctrl_get_oad(cid, 0x32000800, 0, &(ptPwEvent->reportFlag), sizeof(uint8));
        appctrl_get_oad(cid, 0x32000900, 0, &(ptPwEvent->bEffect), sizeof(uint8));
        appctrl_get_oad(cid, 0x32000a00, 0, &(ptPwEvent->timeStatus), sizeof(OOP_EVTTIME_T));
    }

    /* 2 电控跳闸记录 */
    CTRL_EC_EVENT_T *ptEcEvent = &(g_CtrlEvent.tEcEvent);
    ret = appctrl_get_oad(cid, 0x32010200, 0, &(ptEcEvent->record), sizeof(OOP_ECTRLLOG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get ec event 0x%x failed, ret %d!\n", 0x32010200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x32010300, 0, &(ptEcEvent->oadsets), sizeof(OOP_OADS_T));
        appctrl_get_oad(cid, 0x32010400, 0, &(ptEcEvent->curCount), sizeof(uint16));
        appctrl_get_oad(cid, 0x32010500, 0, &(ptEcEvent->maxCount), sizeof(uint16));
        appctrl_get_oad(cid, 0x32010700, 0, &(ptEcEvent->curValue), sizeof(OOP_EVTSTAT_T));
        appctrl_get_oad(cid, 0x32010800, 0, &(ptEcEvent->reportFlag), sizeof(uint8));
        appctrl_get_oad(cid, 0x32010900, 0, &(ptEcEvent->bEffect), sizeof(uint8));
        appctrl_get_oad(cid, 0x32010a00, 0, &(ptEcEvent->timeStatus), sizeof(OOP_EVTTIME_T));
    }

    /* 3 电控告警事件记录 */
    CTRL_ECALARM_EVENT_T *ptEcAlarm = &(g_CtrlEvent.tEcAlarm);
    ret = appctrl_get_oad(cid, 0x32030200, 0, &(ptEcAlarm->record), sizeof(OOP_EWARNLOG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get ec warning event 0x%x failed, ret %d!\n", 0x32030200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x32030300, 0, &(ptEcAlarm->oadsets), sizeof(OOP_OADS_T));
        appctrl_get_oad(cid, 0x32030400, 0, &(ptEcAlarm->curCount), sizeof(uint16));
        appctrl_get_oad(cid, 0x32030500, 0, &(ptEcAlarm->maxCount), sizeof(uint16));
        appctrl_get_oad(cid, 0x32030700, 0, &(ptEcAlarm->curValue), sizeof(OOP_EVTSTAT_T));
        appctrl_get_oad(cid, 0x32030800, 0, &(ptEcAlarm->reportFlag), sizeof(uint8));
        appctrl_get_oad(cid, 0x32030900, 0, &(ptEcAlarm->bEffect), sizeof(uint8));
        appctrl_get_oad(cid, 0x32030a00, 0, &(ptEcAlarm->timeStatus), sizeof(OOP_EVTTIME_T));
    }

    /* 4 遥控跳闸记录 */
    CTRL_REMOTE_EVENT_T *ptRemote = &(g_CtrlEvent.tRemote);
    ret = appctrl_get_oad(cid, 0x31150200, 0, &(ptRemote->record), sizeof(OOP_RCTRLLOG_T));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get remote event 0x%x failed, ret %d!\n", 0x31150200, ret);
    }
    else
    {
        appctrl_get_oad(cid, 0x31150300, 0, &(ptRemote->oadsets), sizeof(OOP_OADS_T));
        appctrl_get_oad(cid, 0x31150400, 0, &(ptRemote->curCount), sizeof(uint16));
        appctrl_get_oad(cid, 0x31150500, 0, &(ptRemote->maxCount), sizeof(uint16));
        appctrl_get_oad(cid, 0x31150700, 0, &(ptRemote->curValue), sizeof(OOP_EVTSTAT_T));
        appctrl_get_oad(cid, 0x31150800, 0, &(ptRemote->reportFlag), sizeof(uint8));
        appctrl_get_oad(cid, 0x31150900, 0, &(ptRemote->bEffect), sizeof(uint8));
        appctrl_get_oad(cid, 0x31150a00, 0, &(ptRemote->timeStatus), sizeof(OOP_EVTTIME_T));
    }
}

/**********************************************************************
* @name      : appctrl_get_arg
* @brief     ：获取数据中心关于控制的配置参数
* @param[in] ：int cid  数据中心ID
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-3
* @Update    :
**********************************************************************/
void appctrl_get_arg(int cid)
{
    /* 1. 先获取总加组配置 */
    appctrl_get_subgroup_arg(cid);

    /* 2. 控制参数获取 */
    appctrl_get_ctrl_arg(cid);

    /* 3 事件 */
    //appctrl_get_event_arg(cid);
    

    if(0 != appctrl_get_oad(cid, 0x40160200, 0, &g_tDayPeriod, sizeof(g_tDayPeriod)))
    {
        CTRL_FMT_DEBUG("Get period table failed\n");
    }
    if(0 != appctrl_get_oad(cid, 0x40180200, 0, &g_tEnPrice, sizeof(g_tEnPrice)))
    {
        CTRL_FMT_DEBUG("Get period price failed\n");
    }

    if(0 != appctrl_get_oad(cid, 0xf2060400, 0, &g_tWarnTime, sizeof(g_tWarnTime)))
    {
        CTRL_FMT_DEBUG("Get warntime failed\n");
    }

    if(0 != appctrl_get_oad(cid, 0xf2050200, 0, &g_tCtrlSwitch, sizeof(g_tCtrlSwitch)))
    {
        CTRL_FMT_DEBUG("Get warntime failed\n");
    }

    g_CtrlDBid = cid;
}

/**********************************************************************
* @name      : appctrl_subgroup_update
* @brief     ：总加组数据更新
* @param[in] ：uint32 Oad      oad
               uint16 infoNum  信息点号
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
int appctrl_subgroup_update(uint32 Oad, uint16 infoNum)
{
    int ret = 0;
    uint8 grp = ((Oad >> 16) & 0x0f) - 1;
    uint8 filed = ((Oad & 0x0000ff00) >> 8);

    if(grp >= OOP_MAX_TG)
    {
        return 0;
    }
    
    switch(filed)
    {
        case 2:
        {
            bzero(&(g_SubGroup[grp].argList), sizeof(OOP_TGSETS_T));
            ret = appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_SubGroup[grp].argList), sizeof(OOP_TGSETS_T));
            appctrl_cal_power_clear(grp, &(g_SubGroup[grp]));
            break;
        }
        case 3:
        {
            int64 tempnum = 0;
            ret = appctrl_get_oad(g_CtrlDBid, Oad, 0, &tempnum, sizeof(int64));
            if(0 == ret && tempnum != g_SubGroup[grp].actPower)
            {
                g_SubGroup[grp].actPower = tempnum;
            }
            CTRL_FMT_DEBUG("总加组%d功率更新%lld\n", grp + 1, g_SubGroup[grp].actPower);
            break;
        }
        case 5:
        {
            g_SubGroup[grp].aveActPower = 0;
            ret = appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_SubGroup[grp].aveActPower), sizeof(int64));
            CTRL_FMT_DEBUG("总加组%d滑差功率更新%lld\n", grp + 1, g_SubGroup[grp].aveActPower);
            break;
        }
        case 9:
        {
            OOP_HENERGYS_T tempEng;
            ret = appctrl_get_oad(g_CtrlDBid, Oad, 0, &tempEng, sizeof(OOP_HENERGYS_T));
            if(0 == ret && 0 != memcmp(&(g_SubGroup[grp].monActEn), &tempEng, sizeof(OOP_HENERGYS_T)))
            {
                memcpy(&(g_SubGroup[grp].monActEn), &tempEng, sizeof(OOP_HENERGYS_T));
                CTRL_FMT_DEBUG("总加组%d电量更新%lld\n", grp + 1, tempEng.nValue[0]);
            }
            break;
        }
        case 0xe:
        {
            bzero(&(g_SubGroup[grp].ctrlPwTurn), sizeof(OOP_BITSTR8_T));
            ret = appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_SubGroup[grp].ctrlPwTurn), sizeof(OOP_BITSTR8_T));
            CTRL_FMT_DEBUG("总加组%d功控轮次更新%02X\n", grp + 1, g_SubGroup[grp].ctrlPwTurn.nValue);
            break;
        }
        case 0xf:
        {
            bzero(&(g_SubGroup[grp].ctrlEcTurn), sizeof(OOP_BITSTR8_T));
            ret = appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_SubGroup[grp].ctrlEcTurn), sizeof(OOP_BITSTR8_T));
            CTRL_FMT_DEBUG("总加组%d电控轮次更新%02X\n", grp + 1, g_SubGroup[grp].ctrlEcTurn.nValue);
            break;
        }
        default: break;
    }

    return ret;
}

/**********************************************************************
* @name      : appctrl_ctrlarg_update
* @brief     ：控制数据更新
* @param[in] ：uint32 Oad      oad
               uint16 infoNum  信息点号
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
int appctrl_ctrlarg_update(uint32 Oad, uint16 infoNum)
{
    uint16 cls = (Oad >> 16);
    uint8  filed = ((Oad & 0x0000ff00) >> 8);
    int    ret = 0;

    /* 远程控制参数 */
    if(0x8000 == cls)
    {
        CTRL_FMT_DEBUG("远程控制更新: OAD 0x%x\n", Oad);
        switch(filed)
        {
            case 2:
            {
                bzero(&(g_CtrlArg.tRemote.param), sizeof(OOP_TELECON_PARAM_T));
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tRemote.param), sizeof(OOP_TELECON_PARAM_T));
            }
            case 0x7f:
            {
                g_CtrlArg.tRemote.bAlarm = 0;
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tRemote.bAlarm), sizeof(uint8));
            }
            case 0x81:
            {
                bzero(&(g_CtrlArg.tRemote.trip), sizeof(OOP_TELECON_TRIP_T));
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tRemote.trip), sizeof(OOP_TELECON_TRIP_T));
            }
            case 0x82:
            {
                bzero(&(g_CtrlArg.tRemote.close), sizeof(OOP_TELECON_CLOSE_T));
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tRemote.close), sizeof(OOP_TELECON_CLOSE_T));
            }
            default: return 0;
        }
    }

    /* 保电 */
    if(0x8001 == cls)
    {
        switch(filed)
        {
            case 2:
            {
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tProtect.status), sizeof(uint8));
            }
            case 0x3:
            {
#if CTRL_PTOTECT_OPEN
                g_CtrlArg.tProtect.NMTimer = 1440; //默认值1440
#endif
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tProtect.NMTimer), sizeof(uint16));
            }
            case 0x4:
            {
#if CTRL_PTOTECT_OPEN
                g_CtrlArg.tProtect.FMTimer = 10; //默认值10
#else
                g_CtrlArg.tProtect.FMTimer = 0;
#endif
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tProtect.FMTimer), sizeof(uint16));
            }
            case 0x5:
            {
                bzero(&(g_CtrlArg.tProtect.autoprotect), sizeof(OOP_PRTTIME_T));
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tProtect.autoprotect), sizeof(OOP_PRTTIME_T));
            }
            default: return 0;
        }
    }
    /* 催费告警配置 */
    if(0x80020300 == Oad)
    {
        bzero(&(g_CtrlArg.tCallFee.param), sizeof(OOP_CALLFEE_T));
        return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tCallFee.param), sizeof(OOP_CALLFEE_T));
    }
    if(0x80027fff == Oad) //投入解除
    {
        ret = appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tCallFee.bEffect), sizeof(uint8));
        CTRL_FMT_DEBUG("催费告警 %s\n", TRUE == g_CtrlArg.tCallFee.bEffect ? "投入" : "解除");
        appctrl_db_chg_event(TRUE == g_CtrlArg.tCallFee.bEffect ? 1 : 2, CTRL_CALLFEE_TYPE, 0);
        return ret;
    }

    /* 终端保安定值 */
    if(0x81000200 == Oad)
    {
        //g_CtrlArg.safevalue = 99900000000LL;
        return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.safevalue), sizeof(int64));
    }
    /* 终端功控时段 */
    if(0x81010200 == Oad)
    {
        bzero(&(g_CtrlArg.tPowconTime), sizeof(OOP_POWCON_TIME_T));
        ret = appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tPowconTime), sizeof(OOP_POWCON_TIME_T));
        appctrl_cal_period(&(g_CtrlArg.tPowconTime));
        return ret;
    }
    /* 功控告警时间 */
    if(0x81020200 == Oad)
    {
        bzero(&(g_CtrlArg.tPowconWarn), sizeof(OOP_POWCON_WARNING_T));
        return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tPowconWarn), sizeof(OOP_POWCON_WARNING_T));
    }

    /* 时段功控 */
    if(0x8103 == cls)
    {
        switch(filed)
        {
            case 2:
            {
                bzero(&(g_CtrlArg.tPeriod.param), sizeof(OOP_8103_CONFIG_T));
                appctrl_db_chg_event(0, CTRL_PERIOD_TYPE, 0);
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tPeriod.param), sizeof(OOP_8103_CONFIG_T));
            }
            case 0x6:
            {
                if(0 != infoNum && infoNum <= OOP_MAX_TG)
                {
                    g_CtrlArg.tPeriod.bEffect[infoNum-1] = FALSE;
                    ret = appctrl_get_oad(g_CtrlDBid, Oad, infoNum, &(g_CtrlArg.tPeriod.bEffect[infoNum-1]), sizeof(uint8));
                    g_EffectType = CTRL_PERIOD_TYPE;
                    g_EffectInfo = infoNum;
                    g_CtrlEffect = (TRUE == g_CtrlArg.tPeriod.bEffect[infoNum-1]) ? 1 : 2;
                    CTRL_FMT_DEBUG("时段功控 %s, 总加组%d\n", TRUE == g_CtrlArg.tPeriod.bEffect[infoNum-1] ? "投入" : "解除", infoNum);
                    appctrl_db_chg_event(TRUE == g_CtrlArg.tPeriod.bEffect[infoNum-1] ? 1 : 2, CTRL_PERIOD_TYPE, infoNum);
                }
                return ret;
            }
            case 0x7f:
            {
                if(0 != infoNum && infoNum <= OOP_MAX_TG)
                {
                    bzero(&(g_CtrlArg.tPeriod.project[infoNum-1]), sizeof(OOP_POWCON_PERIOD_T));
                    return appctrl_get_oad(g_CtrlDBid, Oad, infoNum, &(g_CtrlArg.tPeriod.project[infoNum-1]), sizeof(OOP_POWCON_PERIOD_T));
                }
                break;
            }
            default: return 0;
        }
    }

    /* 厂休控 */
    if(0x8104 == cls)
    {
        switch(filed)
        {
            case 2:
            {
                bzero(&(g_CtrlArg.tRest.param), sizeof(OOP_8104_CONFIG_T));
                appctrl_db_chg_event(0, CTRL_REST_TYPE, 0);
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tRest.param), sizeof(OOP_8104_CONFIG_T));
            }
            case 0x6:
            {
                if(0 != infoNum && infoNum <= OOP_MAX_TG)
                {
                    g_CtrlArg.tRest.bEffect[infoNum-1] = FALSE;
                    ret = appctrl_get_oad(g_CtrlDBid, Oad, infoNum, &(g_CtrlArg.tRest.bEffect[infoNum-1]), sizeof(uint8));
                    g_EffectType = CTRL_REST_TYPE;
                    g_EffectInfo = infoNum;
                    g_CtrlEffect = (TRUE == g_CtrlArg.tRest.bEffect[infoNum-1]) ? 1 : 2;
                    CTRL_FMT_DEBUG("厂休控 %s, 总加组%d\n", TRUE == g_CtrlArg.tRest.bEffect[infoNum-1] ? "投入" : "解除", infoNum);
                    appctrl_db_chg_event(TRUE == g_CtrlArg.tRest.bEffect[infoNum-1] ? 1 : 2, CTRL_REST_TYPE, infoNum);
                }
                return ret;
            }
            default: return 0;
        }
    }

    /* 营业报停控 */
    if(0x8105 == cls)
    {
        switch(filed)
        {
            case 2:
            {
                bzero(&(g_CtrlArg.tStop.param), sizeof(OOP_8105_CONFIG_T));
                appctrl_db_chg_event(0, CTRL_STOP_TYPE, 0);
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tStop.param), sizeof(OOP_8105_CONFIG_T));
            }
            case 0x6:
            {
                if(0 != infoNum && infoNum <= OOP_MAX_TG)
                {
                    g_CtrlArg.tStop.bEffect[infoNum-1] = FALSE;
                    ret = appctrl_get_oad(g_CtrlDBid, Oad, infoNum, &(g_CtrlArg.tStop.bEffect[infoNum-1]), sizeof(uint8));
                    g_EffectType = CTRL_STOP_TYPE;
                    g_EffectInfo = infoNum;
                    g_CtrlEffect = (TRUE == g_CtrlArg.tStop.bEffect[infoNum-1]) ? 1 : 2;
                    CTRL_FMT_DEBUG("营业报停控 %s, 总加组%d\n", TRUE == g_CtrlArg.tStop.bEffect[infoNum-1] ? "投入" : "解除", infoNum);
                    appctrl_db_chg_event(TRUE == g_CtrlArg.tStop.bEffect[infoNum-1] ? 1 : 2, CTRL_STOP_TYPE, infoNum);
                }
                return ret;
            }
            default: return 0;
        }
    }

    /* 当前功率下浮控 */
    if(0x8106 == cls)
    {
        switch(filed)
        {
            case 0x6:
            {
                if(0 != infoNum && infoNum <= OOP_MAX_TG)
                {
                    g_CtrlArg.tDown.bEffect[infoNum-1] = FALSE;
                    ret = appctrl_get_oad(g_CtrlDBid, Oad, infoNum, &(g_CtrlArg.tDown.bEffect[infoNum-1]), sizeof(uint8));
                    if(0 == ret)
                    {
                        g_CtrlArg.tDown.bFirst[infoNum-1] = TRUE;
                        g_CtrlArg.tDown.starttime[infoNum-1] = time(NULL);
                    }
                    g_EffectType = CTRL_DOWN_TYPE;
                    g_EffectInfo = infoNum;
                    g_CtrlEffect = (TRUE == g_CtrlArg.tDown.bEffect[infoNum-1]) ? 1 : 2;
                    CTRL_FMT_DEBUG("下浮控 %s, 总加组%d\n", TRUE == g_CtrlArg.tDown.bEffect[infoNum-1] ? "投入" : "解除", infoNum);
                    appctrl_db_chg_event(TRUE == g_CtrlArg.tDown.bEffect[infoNum-1] ? 1 : 2, CTRL_DOWN_TYPE, infoNum);
                }
                return ret;
            }
            case 0x7f:
            {
                if(0 != infoNum && infoNum <= OOP_MAX_TG)
                {
                    bzero(&(g_CtrlArg.tDown.project[infoNum-1]), sizeof(OOP_POWCON_LOW_T));
                    ret = appctrl_get_oad(g_CtrlDBid, Oad, infoNum, &(g_CtrlArg.tDown.project[infoNum-1]), sizeof(OOP_POWCON_LOW_T));
                }
                return ret;
            }
            default: return 0;
        }
    }
    
    /* 购电控 */
    if(0x8107 == cls)
    {
        switch(filed)
        {
            case 2:
            {
                bzero(&(g_CtrlArg.tBuy.param), sizeof(OOP_8107_CONFIG_T));
                appctrl_db_chg_event(0, CTRL_BUY_TYPE, 0);
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tBuy.param), sizeof(OOP_8107_CONFIG_T));
            }
            case 0x6:
            {
                if(0 != infoNum && infoNum <= OOP_MAX_TG)
                {
                    g_CtrlArg.tBuy.bEffect[infoNum-1] = FALSE;
                    ret = appctrl_get_oad(g_CtrlDBid, Oad, infoNum, &(g_CtrlArg.tBuy.bEffect[infoNum-1]), sizeof(uint8));
                    g_EffectType = CTRL_BUY_TYPE;
                    g_EffectInfo = infoNum;
                    g_CtrlEffect = (TRUE == g_CtrlArg.tBuy.bEffect[infoNum-1]) ? 1 : 2;
                    CTRL_FMT_DEBUG("购电控 %s, 总加组%d\n", TRUE == g_CtrlArg.tBuy.bEffect[infoNum-1] ? "投入" : "解除", infoNum);
                    appctrl_db_chg_event(TRUE == g_CtrlArg.tBuy.bEffect[infoNum-1] ? 1 : 2, CTRL_BUY_TYPE, infoNum);
                }
                return ret;
            }
            default: return 0;
        }
    }

    /* 月电控 */
    if(0x8108 == cls)
    {
        switch(filed)
        {
            case 2:
            {
                bzero(&(g_CtrlArg.tMonth.param), sizeof(OOP_8108_CONFIG_T));
                appctrl_db_chg_event(0, CTRL_MONTH_TYPE, 0);
                return appctrl_get_oad(g_CtrlDBid, Oad, 0, &(g_CtrlArg.tMonth.param), sizeof(OOP_8108_CONFIG_T));
            }
            case 0x6:
            {
                if(0 != infoNum && infoNum <= OOP_MAX_TG)
                {
                    g_CtrlArg.tMonth.bEffect[infoNum-1] = FALSE;
                    ret = appctrl_get_oad(g_CtrlDBid, Oad, infoNum, &(g_CtrlArg.tMonth.bEffect[infoNum-1]), sizeof(uint8));
                    g_EffectType = CTRL_MONTH_TYPE;
                    g_EffectInfo = infoNum;
                    g_CtrlEffect = (TRUE == g_CtrlArg.tMonth.bEffect[infoNum-1]) ? 1 : 2;
                    CTRL_FMT_DEBUG("月电控 %s, 总加组%d\n", TRUE == g_CtrlArg.tMonth.bEffect[infoNum-1] ? "投入" : "解除", infoNum);
                    appctrl_db_chg_event(TRUE == g_CtrlArg.tMonth.bEffect[infoNum-1] ? 1 : 2, CTRL_MONTH_TYPE, infoNum);
                }
                return ret;
            }
            default: return 0;
        }
    }
    return 0;
}

/**********************************************************************
* @name      : appctrl_update_db
* @brief     ：
* @param[in] ：uint32 Oad      oad
               uint16 infoNum  信息点号
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
int appctrl_update_db(uint32 Oad, uint16 infoNum)
{
    CTRL_FMT_TRACE("Oad 0x%x, info %d update!\n", Oad, infoNum);
    /* 总加组更新 */
    if(0x23000000 == (Oad & 0xfff00000))
    {
        return appctrl_subgroup_update(Oad, infoNum);
    }
    
    /* 控制参数更新 */
    if(0x80000000 == (Oad & 0xf0000000))
    {
        return appctrl_ctrlarg_update(Oad, infoNum);
    }

    if(0x40160200 == Oad)
    {
        memset(&g_tDayPeriod, 0, sizeof(OOP_DAYPERIOD_T));
        return appctrl_get_oad(g_CtrlDBid, Oad, 0, &g_tDayPeriod, sizeof(OOP_DAYPERIOD_T));
    }
    if(0x40180200 == Oad)
    {
        memset(&g_tEnPrice, 0, sizeof(OOP_RATETARIFF_T));
        return appctrl_get_oad(g_CtrlDBid, Oad, 0, &g_tEnPrice, sizeof(OOP_RATETARIFF_T));
    }
    if(0xf2060400 == Oad)
    {
        memset(&g_tWarnTime, 0, sizeof(OOP_WARNINGTIME_T));
        return appctrl_get_oad(g_CtrlDBid, Oad, 0, &g_tWarnTime, sizeof(OOP_WARNINGTIME_T));
    }
    if(0xf2050200 == Oad)
    {
        memset(&g_tCtrlSwitch, 0, sizeof(OOP_SWITCHOUT_T));
        return appctrl_get_oad(g_CtrlDBid, Oad, 0, &g_tCtrlSwitch, sizeof(OOP_SWITCHOUT_T));
    }

    return 0;
}

/**********************************************************************
* @name      : appctrl_init_data
* @brief     ：数据初始化清空控制相关的数据
* @param[in] ：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-11
* @Update    :
**********************************************************************/
void appctrl_init_data()
{
    uint32           i;
    CTRL_SUBGROUP_T *ptSubGroup = NULL;
    uint32           offset = 0;

    CTRL_FMT_DEBUG("数据初始化!\n");
    /* 0. 总加组相关配置清除 */
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        ptSubGroup = &(g_SubGroup[i]);
        offset     = i * 0x10000;

        ptSubGroup->downActPower = 0;
        appctrl_write_oad(g_CtrlDBid, 0x23010c00 + offset, 0, &(ptSubGroup->downActPower), sizeof(int64));

        /* 控制配置 */
        memset(&(ptSubGroup->ctrlStatus), 0, sizeof(OOP_TGSETTING_T));
        appctrl_write_oad(g_CtrlDBid, 0x23011000 + offset, 0, &(ptSubGroup->ctrlStatus), sizeof(OOP_TGSETTING_T));
        
        memset(&(ptSubGroup->curCtrlStatus), 0, sizeof(OOP_TGCURSTATE_T));
        appctrl_write_oad(g_CtrlDBid, 0x23011100 + offset, 0, &(ptSubGroup->curCtrlStatus), sizeof(OOP_TGCURSTATE_T));
    }

    /* 1 远程控制参数 */
    CTRL_REMOTE_ARG_T *ptRemote = &(g_CtrlArg.tRemote);
    bzero(&(ptRemote->alarmstatus), sizeof(ptRemote->alarmstatus));
    appctrl_write_oad(g_CtrlDBid, 0x80000400, 0, &(ptRemote->alarmstatus), sizeof(ptRemote->alarmstatus));
    bzero(&(ptRemote->orderstatus), sizeof(ptRemote->orderstatus));
    appctrl_write_oad(g_CtrlDBid, 0x80000500, 0, &(ptRemote->orderstatus), sizeof(ptRemote->orderstatus));

    /* 2 保电参数 */

    /* 3 催费告警 */
    CTRL_CALLFEE_ARG_T *ptCallFee = &(g_CtrlArg.tCallFee);
    ptCallFee->status = 0;
    appctrl_write_oad(g_CtrlDBid, 0x80020200, 0, &(ptCallFee->status), sizeof(uint8));

    /* 4 保安定值 */

    /* 5 终端功控时段 */

    /* 6 功控告警时间 */

    /* 7 时段功控 */
    CTRL_PERIOD_ARG_T *ptPeriod = &(g_CtrlArg.tPeriod);
    if(0 != ptPeriod->param.nNum)
    {
        bzero(&(ptPeriod->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81030300, 0, &(ptPeriod->effectStatus), sizeof(OOP_TGSTATES_T));
        bzero(&(ptPeriod->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81030400, 0, &(ptPeriod->outputStatus), sizeof(OOP_TURNSTATES_T));
        bzero(&(ptPeriod->alarmStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81030500, 0, &(ptPeriod->alarmStatus), sizeof(OOP_TGSTATES_T));
    }

    /* 8 厂休控 */
    CTRL_REST_ARG_T *ptRest = &(g_CtrlArg.tRest);
    if(0 != ptRest->param.nNum)
    {
        bzero(&(ptRest->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81040300, 0, &(ptRest->effectStatus), sizeof(OOP_TGSTATES_T));
        bzero(&(ptRest->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81040400, 0, &(ptRest->outputStatus), sizeof(OOP_TURNSTATES_T));
        bzero(&(ptRest->alarmStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81040500, 0, &(ptRest->alarmStatus), sizeof(OOP_TGSTATES_T));
    }

    /* 9 营业报停控 */
    CTRL_STOP_ARG_T *ptStop = &(g_CtrlArg.tStop);
    if(0 != ptStop->param.nNum)
    {
        bzero(&(ptStop->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81050300, 0, &(ptStop->effectStatus), sizeof(OOP_TGSTATES_T));
        bzero(&(ptStop->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81050400, 0, &(ptStop->outputStatus), sizeof(OOP_TURNSTATES_T));
        bzero(&(ptStop->alarmStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81050500, 0, &(ptStop->alarmStatus), sizeof(OOP_TGSTATES_T));
    }

    /* 10 当前功率下浮控 */
    CTRL_DOWN_ARG_T *ptDown = &(g_CtrlArg.tDown);
    bzero(&(ptDown->effectStatus), sizeof(OOP_TGSTATES_T));
    appctrl_write_oad(g_CtrlDBid, 0x81060300, 0, &(ptDown->effectStatus), sizeof(OOP_TGSTATES_T));
    bzero(&(ptDown->outputStatus), sizeof(OOP_TURNSTATES_T));
    appctrl_write_oad(g_CtrlDBid, 0x81060400, 0, &(ptDown->outputStatus), sizeof(OOP_TURNSTATES_T));
    bzero(&(ptDown->alarmStatus), sizeof(OOP_TGSTATES_T));
    appctrl_write_oad(g_CtrlDBid, 0x81060500, 0, &(ptDown->alarmStatus), sizeof(OOP_TGSTATES_T));

    /* 11 购电控 */
    CTRL_BUY_ARG_T *ptBuy = &(g_CtrlArg.tBuy);
    if(0 != ptBuy->param.nNum)
    {
        bzero(&(ptBuy->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81070300, 0, &(ptBuy->effectStatus), sizeof(OOP_TGSTATES_T));
        bzero(&(ptBuy->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81070400, 0, &(ptBuy->outputStatus), sizeof(OOP_TURNSTATES_T));
        bzero(&(ptBuy->alarmStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81070500, 0, &(ptBuy->alarmStatus), sizeof(OOP_TGSTATES_T));
    }

    /* 12 月电控 */
    CTRL_MONTH_ARG_T *ptMon = &(g_CtrlArg.tMonth);
    if(0 != ptMon->param.nNum)
    {
        bzero(&(ptMon->effectStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81080300, 0, &(ptMon->effectStatus), sizeof(OOP_TGSTATES_T));
        bzero(&(ptMon->outputStatus), sizeof(OOP_TURNSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81080400, 0, &(ptMon->outputStatus), sizeof(OOP_TURNSTATES_T));
        bzero(&(ptMon->alarmStatus), sizeof(OOP_TGSTATES_T));
        appctrl_write_oad(g_CtrlDBid, 0x81080500, 0, &(ptMon->alarmStatus), sizeof(OOP_TGSTATES_T));
    }
}

/**********************************************************************
* @name      : appctrl_init_arg
* @brief     ：数据初始化参数初始化
* @param[in] ：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-11
* @Update    :
**********************************************************************/
void appctrl_init_arg()
{
    CTRL_FMT_DEBUG("参数初始化, 恢复出厂设置!\n");
    appctrl_data_init();
    appctrl_get_arg(g_CtrlDBid);
}

/**********************************************************************
* @name      : appctrl_clear_daystatic
* @brief     ：清理日统计数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-5
* @Update    :
**********************************************************************/
void appctrl_clear_daystatic()
{
    g_CtrlStatic.dayRemoteCount = 0;
    g_CtrlStatic.dayPwTripCount = 0;
    g_CtrlStatic.dayRestTripCount = 0;
    g_CtrlStatic.dayBuyTripCount = 0;
    g_CtrlStatic.datMonTripCount = 0;
}

/**********************************************************************
* @name      : appctrl_clear_monstatic
* @brief     ：清理月统计数据
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-5
* @Update    :
**********************************************************************/
void appctrl_clear_monstatic()
{
    g_CtrlStatic.monRemoteCount = 0;
    g_CtrlStatic.monPwTripCount = 0;
    g_CtrlStatic.monRestTripCount = 0;
    g_CtrlStatic.monBuyTripCount = 0;
    g_CtrlStatic.monMonTripCount = 0;
}

/**********************************************************************
* @name      : appctrl_assign_status
* @brief     ：判断功控电控轮次投入、更新控制投入状态
               长函数，实现状态更新，并写入数据库
* @param[in] ：
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-6
* @Update    :
**********************************************************************/
void appctrl_assign_status(int cid)
{
    uint8           i, j;
    int             ret = 0;
    CTRL_SUBGROUP_T *ptSubGroup = NULL;
    OOP_TGSETTING_T tNewStatus[OOP_MAX_TG];
    OOP_TGSTATES_T  effPTStatus;
    OOP_TGSTATES_T  effRestStatus;
    OOP_TGSTATES_T  effStopStatus;
    OOP_TGSTATES_T  effDownStatus;
    OOP_TGSTATES_T  effBuyStatus;
    OOP_TGSTATES_T  effMonStatus;
    uint16          strlen = 0;

    /* 临时投入状态数据初始化 */
    memset(&tNewStatus,    0, OOP_MAX_TG * sizeof(OOP_TGSETTING_T));
    memset(&effPTStatus,   0, sizeof(OOP_TGSTATES_T));
    memset(&effRestStatus, 0, sizeof(OOP_TGSTATES_T));
    memset(&effStopStatus, 0, sizeof(OOP_TGSTATES_T));
    memset(&effDownStatus, 0, sizeof(OOP_TGSTATES_T));
    memset(&effBuyStatus,  0, sizeof(OOP_TGSTATES_T));
    memset(&effMonStatus,  0, sizeof(OOP_TGSTATES_T));

    /* 控制投入解除状态 */
    effPTStatus.nNum = g_CtrlArg.tPeriod.param.nNum;
    for(j = 0; j < g_CtrlArg.tPeriod.param.nNum; j++)
    {
        effPTStatus.item[j].obj = g_CtrlArg.tPeriod.param.item[j].object;
        if(effPTStatus.item[j].obj >= 0x2301 && effPTStatus.item[j].obj <= 0x2308)
        {
            if(g_CtrlArg.tPeriod.bEffect[effPTStatus.item[j].obj - 0x2301])
            {
                effPTStatus.item[j].state = CTRL_EFFECT;
            }
        }
    }

    effRestStatus.nNum = g_CtrlArg.tRest.param.nNum;
    for(j = 0; j < g_CtrlArg.tRest.param.nNum; j++)
    {
        effRestStatus.item[j].obj = g_CtrlArg.tRest.param.item[j].object;
        if(effRestStatus.item[j].obj >= 0x2301 && effRestStatus.item[j].obj <= 0x2308)
        {
            if(g_CtrlArg.tRest.bEffect[effRestStatus.item[j].obj - 0x2301])
            {
                effRestStatus.item[j].state = CTRL_EFFECT;
            }
        }
    }

    effStopStatus.nNum = g_CtrlArg.tStop.param.nNum;
    for(j = 0; j < g_CtrlArg.tStop.param.nNum; j++)
    {
        effStopStatus.item[j].obj = g_CtrlArg.tStop.param.item[j].object;
        if(effStopStatus.item[j].obj >= 0x2301 && effStopStatus.item[j].obj <= 0x2308)
        {
            if(g_CtrlArg.tStop.bEffect[effStopStatus.item[j].obj - 0x2301])
            {
                effStopStatus.item[j].state = CTRL_EFFECT;
            }
        }
    }

    for(j = 0; j < OOP_MAX_TG; j++)
    {
        if(0 == g_CtrlArg.tDown.project[j].object)
        {
            continue;
        }
        effDownStatus.item[effDownStatus.nNum].obj = g_CtrlArg.tDown.project[j].object;
        effDownStatus.item[effDownStatus.nNum].state = CTRL_UNEFFECT;
        
        if(effDownStatus.item[effDownStatus.nNum].obj >= 0x2301 && effDownStatus.item[effDownStatus.nNum].obj <= 0x2308)
        {
            if(g_CtrlArg.tDown.bEffect[effDownStatus.item[effDownStatus.nNum].obj - 0x2301])
            {
                effDownStatus.item[effDownStatus.nNum].state = CTRL_EFFECT;
            }
        }
        effDownStatus.nNum++;
    }

    effBuyStatus.nNum = g_CtrlArg.tBuy.param.nNum;
    for(j = 0; j < g_CtrlArg.tBuy.param.nNum; j++)
    {
        effBuyStatus.item[j].obj = g_CtrlArg.tBuy.param.item[j].object;
        if(effBuyStatus.item[j].obj >= 0x2301 && effBuyStatus.item[j].obj <= 0x2308)
        {
            if(g_CtrlArg.tBuy.bEffect[effBuyStatus.item[j].obj - 0x2301])
            {
                effBuyStatus.item[j].state = CTRL_EFFECT;
            }
        }
    }

    effMonStatus.nNum = g_CtrlArg.tMonth.param.nNum;
    for(j = 0; j < g_CtrlArg.tMonth.param.nNum; j++)
    {
        effMonStatus.item[j].obj = g_CtrlArg.tMonth.param.item[j].object;
        if(effMonStatus.item[j].obj >= 0x2301 && effMonStatus.item[j].obj <= 0x2308)
        {
            if(g_CtrlArg.tMonth.bEffect[effMonStatus.item[j].obj - 0x2301])
            {
                effMonStatus.item[j].state = CTRL_EFFECT;
            }
        }
    }

    /* 从总加组配置中判断轮次是否打开 */
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        ptSubGroup = &(g_SubGroup[i]);
        if(0 == ptSubGroup->argList.nNum)
        {
            continue;
        }
        tNewStatus[i].pcTurn.nValue = ptSubGroup->ctrlPwTurn.nValue;
        tNewStatus[i].ecTurn.nValue = ptSubGroup->ctrlEcTurn.nValue;

        /* 时段控制 */
        if(TRUE == g_CtrlArg.tPeriod.bEffect[i])
        {
            for(j = 0; j < OOP_MAX_TG; j++)
            {
                if(g_CtrlArg.tPeriod.project[j].object == 0x2301 + i)
                {
                    tNewStatus[i].idxPeriod = g_CtrlArg.tPeriod.project[j].plan.planID;
                    tNewStatus[i].pcFlag.nValue = g_CtrlArg.tPeriod.project[j].plan.flag.nValue;
                    tNewStatus[i].pcState.nValue |= 0x01;
                    break;
                }
            }
        }

        /* 厂休控 */
        if(TRUE == g_CtrlArg.tRest.bEffect[i])
        {
            for(j = 0; j < g_CtrlArg.tRest.param.nNum; j++)
            {
                if(g_CtrlArg.tRest.param.item[j].object == 0x2301 + i)
                {
                    tNewStatus[i].pcState.nValue |= 0x02;
                    break;
                }
            }
        }

        /* 营业报停控 */
        if(TRUE == g_CtrlArg.tStop.bEffect[i])
        {
            for(j = 0; j < g_CtrlArg.tStop.param.nNum; j++)
            {
                if(g_CtrlArg.tStop.param.item[j].object == 0x2301 + i)
                {
                    tNewStatus[i].pcState.nValue |= 0x04;
                    break;
                }
            }
        }

        /* 当前功率下浮控 */
        if(TRUE == g_CtrlArg.tDown.bEffect[i])
        {
            for(j = 0; j < OOP_MAX_TG; j++)
            {
                if(g_CtrlArg.tDown.project[j].object == 0x2301 + i)
                {
                    tNewStatus[i].pcState.nValue |= 0x08;
                    break;
                }
            }
        }

        /* 月电控 */
        if(TRUE == g_CtrlArg.tMonth.bEffect[i])
        {
            for(j = 0; j < g_CtrlArg.tMonth.param.nNum; j++)
            {
                if(g_CtrlArg.tMonth.param.item[j].object == 0x2301 + i)
                {
                    tNewStatus[i].ecState.nValue |= 0x01;
                    break;
                }
            }
        }

        /* 购电控 */
#if 0
        if(0 != ptSubGroup->ctrlEcTurn.nValue)
        {
            for(j = 0; j < g_CtrlArg.tBuy.param.nNum; j++)
            {
                if(g_CtrlArg.tBuy.param.item[j].object == 0x2301 + i)
                {
                    if(FALSE == g_CtrlArg.tBuy.bEffect && 0 == g_CtrlArg.tBuy.param.item[j].mode)
                    {   //远程模式未投入也控制
                        break;
                    }                        
                    tNewStatus[i].ecState.nValue |= 0x02;
                    /* 控制投入状态 */
                    for(k = 0; k < effBuyStatus.nNum; k++)
                    {
                        if(effBuyStatus.status[k].object == 0x2301 + i)
                        {
                            effBuyStatus.status[k].bEffect = CTRL_EFFECT;
                            break;
                        }
                    }
                    break;
                }
            }
        }
#else
        if(TRUE == g_CtrlArg.tBuy.bEffect[i])
        {
            for(j = 0; j < g_CtrlArg.tBuy.param.nNum; j++)
            {
                if(g_CtrlArg.tBuy.param.item[j].object == 0x2301 + i)
                {
                    tNewStatus[i].ecState.nValue |= 0x02;
                    break;
                }
            }
        }
#endif 
    }

    //CTRL_FMT_TRACE("Get ctrl new status:\n");
    /* 控制数据中心写入, 闸变化 */
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        ptSubGroup = &(g_SubGroup[i]);
        //if(0 == ptSubGroup->argList.nNum)
        //{
        //    continue;
        //}
        if(0 != memcmp(&(tNewStatus[i]), &ptSubGroup->ctrlStatus, sizeof(OOP_TGSETTING_T)))
        {
            memcpy(&(ptSubGroup->ctrlStatus), &(tNewStatus[i]), sizeof(OOP_TGSETTING_T));
            ret = appctrl_write_oad(cid, 0x23011000 + i * 0x10000, 0, &(tNewStatus[i]), sizeof(OOP_TGSETTING_T));
            if(0 != ret)
            {
                CTRL_FMT_DEBUG("Write subgroup 0x%x failed, ret %d!\n", 0x23011000 + i * 0x10000, ret);
            }
            CTRL_FMT_DEBUG("SubGroup %d status: plan %d, flag %u, PCState %u, ECState %u, PCTurn %u, ECTurn %u\n",
                           i + 1, tNewStatus[i].idxPeriod, tNewStatus[i].pcFlag.nValue, tNewStatus[i].pcState.nValue,
                           tNewStatus[i].ecState.nValue, tNewStatus[i].pcTurn.nValue, tNewStatus[i].ecTurn.nValue);
        }
    }

    /* 时段控制 */
    if(0 != memcmp(&effPTStatus, &(g_CtrlArg.tPeriod.effectStatus), sizeof(OOP_TGSTATES_T)))
    {
        char tempstr[400] = {0};
        memcpy(&(g_CtrlArg.tPeriod.effectStatus), &effPTStatus, sizeof(OOP_TGSTATES_T));
        ret = appctrl_write_oad(cid, 0x81030300, 0, &effPTStatus, sizeof(OOP_TGSTATES_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write period 0x%x failed, ret %d!\n", 0x81030300, ret);
        }
        for(i = 0; i < effPTStatus.nNum; i++)
        {
            strlen += sprintf(tempstr + strlen, "OI 0x%x %d, ", effPTStatus.item[i].obj, effPTStatus.item[i].state);
        }
        CTRL_FMT_DEBUG("Period ctrl new status: %s\n", tempstr);
    }

    /* 厂休控 */
    if(0 != memcmp(&effRestStatus, &(g_CtrlArg.tRest.effectStatus), sizeof(OOP_TGSTATES_T)))
    {
        char tempstr[400] = {0};
        memcpy(&(g_CtrlArg.tRest.effectStatus), &effRestStatus, sizeof(OOP_TGSTATES_T));
        ret = appctrl_write_oad(cid, 0x81040300, 0, &effRestStatus, sizeof(OOP_TGSTATES_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write rest 0x%x failed, ret %d!\n", 0x81040300, ret);
        }
        
        strlen = 0;
        for(i = 0; i < effRestStatus.nNum; i++)
        {
            strlen += sprintf(tempstr + strlen, "OI 0x%x %d, ", effRestStatus.item[i].obj, effRestStatus.item[i].state);
        }
        CTRL_FMT_DEBUG("Rest ctrl new status: %s\n", tempstr);
    }

    /* 营业报停控 */
    if(0 != memcmp(&effStopStatus, &(g_CtrlArg.tStop.effectStatus), sizeof(OOP_TGSTATES_T)))
    {
        char tempstr[400] = {0};
        memcpy(&(g_CtrlArg.tStop.effectStatus), &effStopStatus, sizeof(OOP_TGSTATES_T));
        ret = appctrl_write_oad(cid, 0x81050300, 0, &effStopStatus, sizeof(OOP_TGSTATES_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write stop 0x%x failed, ret %d!\n", 0x81050300, ret);
        }
        
        strlen = 0;
        for(i = 0; i < effStopStatus.nNum; i++)
        {
            strlen += sprintf(tempstr + strlen, "OI 0x%x %d, ", effStopStatus.item[i].obj, effStopStatus.item[i].state);
        }
        CTRL_FMT_DEBUG("Stop ctrl new status: %s\n", tempstr);
    }

    /* 当前功率下浮控 */
    if(0 != memcmp(&effDownStatus, &(g_CtrlArg.tDown.effectStatus), sizeof(OOP_TGSTATES_T)))
    {
        char tempstr[400] = {0};
        memcpy(&(g_CtrlArg.tDown.effectStatus), &effDownStatus, sizeof(OOP_TGSTATES_T));
        ret = appctrl_write_oad(cid, 0x81060300, 0, &effDownStatus, sizeof(OOP_TGSTATES_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write down 0x%x failed, ret %d!\n", 0x81060300, ret);
        }
        
        strlen = 0;
        for(i = 0; i < effDownStatus.nNum; i++)
        {
            strlen += sprintf(tempstr + strlen, "OI 0x%x %d, ", effDownStatus.item[i].obj, effDownStatus.item[i].state);
        }
        CTRL_FMT_DEBUG("Down ctrl new status: %s\n", tempstr);
    }

    /* 购电控 */
    if(0 != memcmp(&effBuyStatus, &(g_CtrlArg.tBuy.effectStatus), sizeof(OOP_TGSTATES_T)))
    {
        char tempstr[400] = {0};
        memcpy(&(g_CtrlArg.tBuy.effectStatus), &effBuyStatus, sizeof(OOP_TGSTATES_T));
        ret = appctrl_write_oad(cid, 0x81070300, 0, &effBuyStatus, sizeof(OOP_TGSTATES_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write buy 0x%x failed, ret %d!\n", 0x81070300, ret);
        }
        
        strlen = 0;
        for(i = 0; i < effBuyStatus.nNum; i++)
        {
            strlen += sprintf(tempstr + strlen, "OI 0x%x %d, ", effBuyStatus.item[i].obj, effBuyStatus.item[i].state);
        }
        CTRL_FMT_DEBUG("Buy ctrl new status: %s\n", tempstr);
    }

    /* 月电控 */
    if(0 != memcmp(&effMonStatus, &(g_CtrlArg.tMonth.effectStatus), sizeof(OOP_TGSTATES_T)))
    {
        char tempstr[400] = {0};
        memcpy(&(g_CtrlArg.tMonth.effectStatus), &effMonStatus, sizeof(OOP_TGSTATES_T));
        ret = appctrl_write_oad(cid, 0x81080300, 0, &effMonStatus, sizeof(OOP_TGSTATES_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write buy 0x%x failed, ret %d!\n", 0x81080300, ret);
        }
        
        strlen = 0;
        for(i = 0; i < effMonStatus.nNum; i++)
        {
            strlen += sprintf(tempstr + strlen, "OI 0x%x %d, ", effMonStatus.item[i].obj, effMonStatus.item[i].state);
        }
        CTRL_FMT_DEBUG("Month ctrl new status: %s\n", tempstr);
    }
}

/**********************************************************************
* @name      : appctri_write_freezepower
* @brief     ：写当前功率下浮控控后总加组有功功率冻结值
* @param[in] ：uint8 group   总加组序号
               int64 value   有功功率冻结值
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-8
* @Update    :
**********************************************************************/
void appctri_write_freezepower(uint8 group, int64 value)
{
    int ret = 0;
    if(value == g_SubGroup[group].downActPower)
    {
        return;
    }
    g_SubGroup[group].downActPower = value;
    ret = appctrl_write_oad(g_CtrlDBid, 0x23010c00 + group * 0x10000, 0, (uint8*)&value, sizeof(int64));
    CTRL_FMT_DEBUG("Sub Group %d, write freeze value %d, ret %d\n", group + 1, value, ret);
}

/**********************************************************************
* @name      : appctrl_get_enprice
* @brief     ：获取当前电费
* @param[in] ：
* @param[out]：
* @return    ：电费
* @Create    : 王津剑
* @Date      ：2020-4-10
* @Update    :
**********************************************************************/
uint32 appctrl_get_enprice()
{
    time_t            now;
    OOP_DATETIME_S_T  curTime = {0};
    DAYPERIOD        *ptPeriod = NULL;
    uint8             no = 0;
    uint32            mintotal = 0;
    uint8             i;

    now = time(NULL);
    DT_Time2DateTimeShort(now, &curTime);

    ptPeriod = &(g_tDayPeriod.dPeriod[0]); //只需要取第0个
    if(0 == ptPeriod->nNum)
    {
        no = EN_LEVEL; //默认费率号
    }
    else if(1 == ptPeriod->nNum)
    {
        no = ptPeriod->period[0].rateNO;
    }
    else
    {
        no = ptPeriod->period[ptPeriod->nNum - 1].rateNO; //不在前n-1个中必然在最后一个
        mintotal = curTime.hour * 60 + curTime.minute;
        for(i = 0; i < ptPeriod->nNum - 1; i++)
        {
            if(ptPeriod->period[i].hour * 60 + ptPeriod->period[i].min <= mintotal &&
                mintotal < ptPeriod->period[i+1].hour * 60 + ptPeriod->period[i+1].min)
            {
                no =  ptPeriod->period[i].rateNO;
                break;
            }
        }
    }
    if(no > EN_BOTTOM || 0 == no)
    {
        no = EN_LEVEL;
    }
    if(no > g_tEnPrice.nNum)
    {
        return g_tEnPrice.nValue[0];
    }
    return g_tEnPrice.nValue[no - 1];
}

/**********************************************************************
* @name      : appctrl_write_leften
* @brief     ：写数据中心总加组剩余电量(费)
* @param[in] ：uint8 group   总加组序号
               int64 leftEn  剩余电量(费)
* @param[out]：
* @return    ：0/错误码
* @Create    : 王津剑
* @Date      ：2020-4-10
* @Update    : 2020-5-22
**********************************************************************/
int appctrl_write_leften(uint8 group, int64 leftEn)
{
    int ret = 0;
    if(group >= OOP_MAX_TG)
    {
        return -1;
    }

    ret = remain_energy_write(g_CtrlDBid, group + 1, leftEn);
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Write subgroup %d leftEN failed, ret %d\n", group + 1, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : appctrl_get_leften
* @brief     ：获取总加组剩余电量(费)
* @param[in] ：uint8 group   总加组序号
* @param[out]：
* @return    ：0/错误码
* @Create    : 王津剑
* @Date      ：2020-5-22
* @Update    :
**********************************************************************/
int appctrl_get_leften(uint8 group)
{
    int ret = 0;
    if(group >= OOP_MAX_TG)
    {
        return -1;
    }

    ret = remain_energy_read(g_CtrlDBid, (uint16)group + 1, &(g_SubGroup[group].leftEn));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Get subgroup %d leftEN failed, ret %d\n", group + 1, ret);
    }
    return ret;
}

/**********************************************************************
* @name      : appctrl_write_subgroupstatus
* @brief     ：写数据中心总加组总加组当前控制状态
* @param[in] ：uint8 group                总加组序号
               OOP_TGCURSTATE_T *pStatus  状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
void appctrl_write_subgroupstatus(uint8 group, OOP_TGCURSTATE_T *pStatus)
{
    int ret = 0;
    if(group >= OOP_MAX_TG)
    {
        return;
    }

    if(0 != memcmp(pStatus, &(g_SubGroup[group].curCtrlStatus), sizeof(OOP_TGCURSTATE_T)))
    {
        memcpy(&(g_SubGroup[group].curCtrlStatus), pStatus, sizeof(OOP_TGCURSTATE_T));
        ret = appctrl_write_oad(g_CtrlDBid, 0x23011100 + group * 0x10000, 0, pStatus, sizeof(OOP_TGCURSTATE_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write dbcenter subgroup %d curCtrlStatus failed, ret %d\n", group + 1, ret);
        }
    }
}

/**********************************************************************
* @name      : appctrl_write_outstatus
* @brief     ：写数据中心输出状态
* @param[in] ：uint8 ctrltype             控制类型
               OOP_TURNSTATES_T *pStatus  状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
void appctrl_write_outstatus(uint8 ctrltype, OOP_TURNSTATES_T *pStatus)
{
    int ret = 0;

    switch(ctrltype)
    {
        case CTRL_DOWN_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tDown.outputStatus), sizeof(OOP_TURNSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tDown.outputStatus), pStatus, sizeof(OOP_TURNSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81060400, 0, pStatus, sizeof(OOP_TURNSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x outputStatus failed, ret %d\n", 0x81060400, ret);
                }
            }
            break;
        }
        case CTRL_STOP_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tStop.outputStatus), sizeof(OOP_TURNSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tStop.outputStatus), pStatus, sizeof(OOP_TURNSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81050400, 0, pStatus, sizeof(OOP_TURNSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x outputStatus failed, ret %d\n", 0x81050400, ret);
                }
            }
            break;
        }
        case CTRL_REST_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tRest.outputStatus), sizeof(OOP_TURNSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tRest.outputStatus), pStatus, sizeof(OOP_TURNSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81040400, 0, pStatus, sizeof(OOP_TURNSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x outputStatus failed, ret %d\n", 0x81040400, ret);
                }
            }
            break;
        }
        case CTRL_PERIOD_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tPeriod.outputStatus), sizeof(OOP_TURNSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tPeriod.outputStatus), pStatus, sizeof(OOP_TURNSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81030400, 0, pStatus, sizeof(OOP_TURNSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x outputStatus failed, ret %d\n", 0x81030400, ret);
                }
            }
            break;
        }
        case CTRL_BUY_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tBuy.outputStatus), sizeof(OOP_TURNSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tBuy.outputStatus), pStatus, sizeof(OOP_TURNSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81070400, 0, pStatus, sizeof(OOP_TURNSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x outputStatus failed, ret %d\n", 0x81070400, ret);
                }
            }
            break;
        }
        case CTRL_MONTH_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tMonth.outputStatus), sizeof(OOP_TURNSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tMonth.outputStatus), pStatus, sizeof(OOP_TURNSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81080400, 0, pStatus, sizeof(OOP_TURNSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x outputStatus failed, ret %d\n", 0x81080400, ret);
                }
            }
            break;
        }
        default:break;
    }
}

/**********************************************************************
* @name      : appctrl_write_alarmstatus
* @brief     ：写数据中心告警状态
* @param[in] ：uint8 ctrltype           控制类型
               OOP_TGSTATES_T *pStatus  状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
void appctrl_write_alarmstatus(uint8 ctrltype, OOP_TGSTATES_T *pStatus)
{
    int ret = 0;
    
    switch(ctrltype)
    {
        case CTRL_DOWN_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tDown.alarmStatus), sizeof(OOP_TGSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tDown.alarmStatus), pStatus, sizeof(OOP_TGSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81060500, 0, pStatus, sizeof(OOP_TGSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x alarmStatus failed, ret %d\n", 0x81060500, ret);
                }
            }
            break;
        }
        case CTRL_STOP_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tStop.alarmStatus), sizeof(OOP_TGSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tStop.alarmStatus), pStatus, sizeof(OOP_TGSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81050500, 0, pStatus, sizeof(OOP_TGSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x alarmStatus failed, ret %d\n", 0x81050500, ret);
                }
            }
            break;
        }
        case CTRL_REST_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tRest.alarmStatus), sizeof(OOP_TGSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tRest.alarmStatus), pStatus, sizeof(OOP_TGSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81040500, 0, pStatus, sizeof(OOP_TGSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x alarmStatus failed, ret %d\n", 0x81040500, ret);
                }
            }
            break;
        }
        case CTRL_PERIOD_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tPeriod.alarmStatus), sizeof(OOP_TGSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tPeriod.alarmStatus), pStatus, sizeof(OOP_TGSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81030500, 0, pStatus, sizeof(OOP_TGSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x alarmStatus failed, ret %d\n", 0x81030500, ret);
                }
            }
            break;
        }
        case CTRL_BUY_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tBuy.alarmStatus), sizeof(OOP_TGSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tBuy.alarmStatus), pStatus, sizeof(OOP_TGSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81070500, 0, pStatus, sizeof(OOP_TGSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x alarmStatus failed, ret %d\n", 0x81070500, ret);
                }
            }
            break;
        }
        case CTRL_MONTH_TYPE:
        {
            if(0 != memcmp(pStatus, &(g_CtrlArg.tMonth.alarmStatus), sizeof(OOP_TGSTATES_T)))
            {
                memcpy(&(g_CtrlArg.tMonth.alarmStatus), pStatus, sizeof(OOP_TGSTATES_T));
                ret = appctrl_write_oad(g_CtrlDBid, 0x81080500, 0, pStatus, sizeof(OOP_TGSTATES_T));
                if(0 != ret)
                {
                    CTRL_FMT_DEBUG("Write dbcenter 0x%x alarmStatus failed, ret %d\n", 0x81080500, ret);
                }
            }
            break;
        }
        default:break;
    }

}

/**********************************************************************
* @name      : appctrl_check_warning_time
* @brief     ：检查是否在告警允许时间段内
* @param[in] ：
* @param[out]：
* @return    ：TRUE/FALSE
* @Create    : 王津剑
* @Date      ：2020-4-14
* @Update    :
**********************************************************************/
uint8 appctrl_check_warning_time()
{
    time_t           now;
    OOP_DATETIME_S_T curTime = {0};
    uint32           curSecond = 0;
    uint32           starttime = 0;
    uint32           endtime = 0;
    uint32           i;

    if(0 == g_tWarnTime.nNum)
    {
        //没有设置允许告警时段,全部打开
        return TRUE;
    }
    now = time(NULL);
    DT_Time2DateTimeShort(now, &curTime);

    curSecond = curTime.hour * 3600 + curTime.minute * 60 + curTime.second;

    for(i = 0; i < g_tWarnTime.nNum; i++)
    {
        starttime = g_tWarnTime.period[i].startTime.hour * 3600 + g_tWarnTime.period[i].startTime.minute * 60
                    + g_tWarnTime.period[i].startTime.second;
        endtime = g_tWarnTime.period[i].endTime.hour * 3600 + g_tWarnTime.period[i].endTime.minute * 60
                    + g_tWarnTime.period[i].endTime.second;
        if(starttime > endtime)
        {
            /* 过0点 */
            endtime += 24 *3600;
        }
        if(starttime <= curSecond && curSecond < endtime)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**********************************************************************
* @name      : appctrl_get_object
* @brief     ：获取控制对象
* @param[in] ：uint8 ctrltype  控制类型
* @param[out]：
* @return    ：OI
* @Create    : 王津剑
* @Date      ：2020-4-20
* @Update    :
**********************************************************************/
uint16 appctrl_get_object(uint8 ctrltype)
{
    switch(ctrltype)
    {
        case CTRL_DOWN_TYPE: return 0x8106;
        case CTRL_STOP_TYPE: return 0x8105;
        case CTRL_REST_TYPE: return 0x8104;
        case CTRL_PERIOD_TYPE: return 0x8103;

        case CTRL_BUY_TYPE: return 0x8107;
        case CTRL_MONTH_TYPE: return 0x8108;
    }
    return 0;
}

/**********************************************************************
* @name      : appctrl_set_protect_status
* @brief     ：设置保电状态
* @param[in] ：uint8 status   状态，解除（0），保电（1），自动保电（2）
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-20
* @Update    :
**********************************************************************/
void appctrl_set_protect_status(uint8 status)
{
    int ret = 0;
    if(status > 2 || status == g_CtrlArg.tProtect.status)
    {
        return;
    }

    ret = appctrl_write_oad(g_CtrlDBid, 0x80010200, 0, &status, sizeof(uint8));
    if(0 != ret)
    {
        CTRL_FMT_DEBUG("Write protect %d failed, ret %d\n", status, ret);
    }
    else
    {
        g_CtrlArg.tProtect.status = status;
    }
    CTRL_FMT_DEBUG("保电状态更新 %d !\n", status);
}

/**********************************************************************
* @name      : appctrl_set_remote_status
* @brief     ：设置遥控
* @param[in] ：uint8 alarm   告警状态
               uint8 trip    跳闸状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-4-21
* @Update    :
**********************************************************************/
#ifdef PRODUCT_ZCU_1
void appctrl_set_remote_status(uint16 alarm, uint16 trip)
#else
void appctrl_set_remote_status(uint8 alarm, uint8 trip)
#endif
{
    int ret = 0;
    
    if(g_CtrlArg.tRemote.alarmstatus.bitValue != alarm)
    {
        g_CtrlArg.tRemote.alarmstatus.bitValue = alarm;
        ret = appctrl_write_oad(g_CtrlDBid, 0x80000400, 0, &(g_CtrlArg.tRemote.alarmstatus), sizeof(g_CtrlArg.tRemote.alarmstatus));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write remote alarm status %X failed, ret %d\n", alarm, ret);
        }
    }

    if(g_CtrlArg.tRemote.orderstatus.bitValue != trip)
    {
        g_CtrlArg.tRemote.orderstatus.bitValue = trip;
        ret = appctrl_write_oad(g_CtrlDBid, 0x80000500, 0, &(g_CtrlArg.tRemote.orderstatus), sizeof(g_CtrlArg.tRemote.orderstatus));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write remote order status %X failed, ret %d\n", trip, ret);
        }
    }
}

/**********************************************************************
* @name      : appctrl_set_oopswitch
* @brief     ：设置数据中心继电器状态
* @param[in] ：uint8 acm1   设备1
               uint8 acm2   设备2
               uint8 trip   跳闸状态
               uint8 close  合闸继电器状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-20
* @Update    :
**********************************************************************/
void appctrl_set_oopswitch(uint8 acm1, uint8 acm2, uint8 trip, uint8 close)
{
    OOP_SWITCHOUT_T tSwitch;
    int ret  = 0;
    NOMAL_OAD_T tOAD;
    uint32      i = 0;
    
    memcpy(&tSwitch, &g_tCtrlSwitch, sizeof(OOP_SWITCHOUT_T));

    tSwitch.nNum = OOP_MAX_SWITCHOUT;
    for(i = 0; i < OOP_MAX_SWITCHOUT / 2; i++)
    {
        
        tSwitch.value[i].current = 0 == acm1 ? 0 : ((trip >> i) & 0x01);
    }
    for(i = 0; i < OOP_MAX_SWITCHOUT / 2; i++)
    {
        tSwitch.value[i + OOP_MAX_SWITCHOUT / 2].current = ((close >> i) & 0x01);
    }

    if(0 != memcmp(&tSwitch, &g_tCtrlSwitch, sizeof(OOP_SWITCHOUT_T)))
    {
        memcpy(&g_tCtrlSwitch, &tSwitch, sizeof(OOP_SWITCHOUT_T));
        memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
        tOAD.oad.value = 0xf2050200;
        tOAD.classtag = CLASS_DATA_INIT;
        ret =  db_write_nomal(g_CtrlDBid, &tOAD, (uint8*)&g_tCtrlSwitch, sizeof(OOP_SWITCHOUT_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write switch status %d failed, ret %d\n", ret);
        }
    }
}

/**********************************************************************
* @name      : appctrl_set_oopswitch
* @brief     ：设置数据中心继电器状态
* @param[in] ：uint8 acm1         设备1
               uint8 acm2        设备2
               uint8 *input      接入状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2022-7-1
* @Update    :
**********************************************************************/
void appctrl_set_oopswitch_input(uint8 acm1, uint8 acm2, uint8 *input, uint8 *selfinput)
{
    OOP_SWITCHOUT_T tSwitch;
    OOP_SWITCHOUT_EVENT_T tEvent;
    int ret  = 0;
    NOMAL_OAD_T tOAD;
    uint32      i = 0;
    
    memcpy(&tSwitch, &g_tCtrlSwitch, sizeof(OOP_SWITCHOUT_T));

    tSwitch.nNum = OOP_MAX_SWITCHOUT;
    for(i = 0; i < OOP_MAX_SWITCHOUT / 2; i++)
    {
        tSwitch.value[i].input = 0 == acm1 ? 1 : input[i];
    }
    
    for(i = 0; i < OOP_MAX_SWITCHOUT / 2; i++)
    {
        tSwitch.value[i + OOP_MAX_SWITCHOUT / 2].input = selfinput[i];;
    }

    if(0 != memcmp(&tSwitch, &g_tCtrlSwitch, sizeof(OOP_SWITCHOUT_T)))
    {
        uint32 i = 0;
        memset(&tEvent, 0, sizeof(tEvent));
        tEvent.nNum = tSwitch.nNum;
        for(i = 0; i < tSwitch.nNum; i++)
        {
            tEvent.value[i].name.nNum = MIN(31, tSwitch.value[i].name.nNum);
            memcpy(tEvent.value[i].name.value, tSwitch.value[i].name.value, tEvent.value[i].name.nNum);
            tEvent.value[i].current = tSwitch.value[i].current;
            tEvent.value[i].attribute = tSwitch.value[i].attribute;
            tEvent.value[i].input = tSwitch.value[i].input;
            
            if(g_tCtrlSwitch.value[i].input != tSwitch.value[i].input)
            {
                CTRL_FMT_DEBUG("继电器 %d, %s\n", i + 1, 0 == tSwitch.value[i].input ? "接入" : "未接入");
            }
        }
        memcpy(&g_tCtrlSwitch, &tSwitch, sizeof(OOP_SWITCHOUT_T));
        memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
        tOAD.oad.value = 0xf2050200;
        tOAD.classtag = CLASS_DATA_INIT;
        ret =  db_write_nomal(g_CtrlDBid, &tOAD, (uint8*)&g_tCtrlSwitch, sizeof(OOP_SWITCHOUT_T));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write switch status %d failed, ret %d\n", ret);
        }

        /* 继电器接入输出3117事件 */
        CheckERC_3117(g_CtrlDBid, tEvent);
    }
}

/**********************************************************************
* @name      : appctrl_get_switch_type
* @brief     ：获取继电器舒心
* @param[in] ：uint8 no     序号
* @param[out]：
* @return    ：状态
* @Create    : 王津剑
* @Date      ：2022-7-1
* @Update    :
**********************************************************************/
uint8 appctrl_get_switch_type(uint8 no)
{
    if(no < OOP_MAX_SWITCHOUT)
    {
        return g_tCtrlSwitch.value[no].attribute;
    }
    return 0;
}

/**********************************************************************
* @name      : appctrl_set_callfee_status
* @brief     ：设置催费告警状态
* @param[in] ：uint8 state   告警状态
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2020-5-20
* @Update    :
**********************************************************************/
void appctrl_set_callfee_status(uint8 state)
{
    int ret = 0;
    
    if(g_CtrlArg.tCallFee.status != state)
    {
        g_CtrlArg.tCallFee.status = state;
        ret = appctrl_write_oad(g_CtrlDBid, 0x80020200, 0, &(g_CtrlArg.tCallFee.status), sizeof(uint8));
        if(0 != ret)
        {
            CTRL_FMT_DEBUG("Write call fee status %d failed, ret %d\n", state, ret);
        }
    }
}

/**********************************************************************
* @name      : appctrl_set_down_close
* @brief     ：设置下浮控解除
* @param[in] ：uint8 group   总加组
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-6-15
* @Update    :
**********************************************************************/
void appctrl_set_down_close(uint8 group)
{
    uint8 flag = 0;
    appctrl_write_oad(g_CtrlDBid, 0x810606ff, group, &(flag), sizeof(uint8));
}

/**********************************************************************
* @name      : appctrl_write_last_time
* @brief     ：写时间
* @param[in] ：OOP_DATETIME_S_T *ptTime   时间
* @param[out]：
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-5-7
* @Update    :
**********************************************************************/
void appctrl_write_last_time(OOP_DATETIME_S_T *ptTime)
{
    NOMAL_OAD_T tOAD;
    
    memset(&tOAD, 0, sizeof(NOMAL_OAD_T));
    tOAD.oad.value = 0x400002ff;
    tOAD.classtag = CLASS_DATA_UNINIT;

    db_write_nomal(g_CtrlDBid, &tOAD, (uint8 *)ptTime, sizeof(OOP_DATETIME_S_T));
}

/**********************************************************************
* @name      : appctrl_get_last_time
* @brief     ：获取时间
* @param[in] ：
* @param[out]：OOP_DATETIME_S_T *ptTime   时间
* @return    ：
* @Create    : 王津剑
* @Date      ：2021-5-7
* @Update    :
**********************************************************************/
void appctrl_get_last_time(OOP_DATETIME_S_T *ptTime)
{
    time_t now;

    if(0 == appctrl_get_oad(g_CtrlDBid, 0x400002ff, 0, ptTime, sizeof(OOP_DATETIME_S_T)))
    {
        CTRL_FMT_DEBUG("重启之前时间 %04d-%02d-%02d %02d:%02d:%02d\n", ptTime->year, ptTime->month, ptTime->day,
                                                                     ptTime->hour, ptTime->minute, ptTime->second);
    }
    else
    {
        time(&now);
        DT_Time2DateTimeShort(now, ptTime);
    }
}

/**********************************************************************
* @name      : appctrl_write_ctrl
* @brief     ：保存告警状态
* @param[in] ：uint8 type      类型 (CTRL_REMOTE_TYPE, CTRL_POWER_TYPE, CTRL_ELEC_TYPE, CTRL_STATUS_TYPE)
               uint8 *data     数据
               uint32 datalen  数据长度(最大数据4K)
* @param[out]：
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-5-7
* @Update    :
**********************************************************************/
int appctrl_write_ctrl(uint8 type, uint8 *data, uint32 datalen)
{
    /* 前1K 内容保存校验和长度 数据按照类型从1K处开始记录 最大容量4K */
    /* 保电特殊 写入的是一个字节 偏移是128 */
    char   filepath[256] = {0};
    uint32 csNO = 0;  //校验值
    uint32 i = 0;
    int    ret = 0;

    if(NULL == data || datalen > 4096 || 0 == datalen)
    {
        return -2;
    }

    for(i = 0; i < datalen; i++)
    {
        csNO += data[i];
    }
    
    sprintf(filepath, "/data/app/%s/data/%s", ADV_APP_NAME, CTRL_FILE_NAME);

    switch(type)
    {
        case CTRL_REMOTE_TYPE:
        {
            ret = write_pdata_xram(filepath, (char *)&csNO, 0, sizeof(uint32));
            ret |= write_pdata_xram(filepath, (char *)&datalen, 4, sizeof(uint32));
            ret |= write_pdata_xram(filepath, (char *)data, 1024, datalen);
            break;
        }
        case CTRL_POWER_TYPE:
        {
            ret = write_pdata_xram(filepath, (char *)&csNO, 8, sizeof(uint32));
            ret |= write_pdata_xram(filepath, (char *)&datalen, 12, sizeof(uint32));
            ret |= write_pdata_xram(filepath, (char *)data, 1024 + 4096, datalen);
            break;
        }
        case CTRL_ELEC_TYPE:
        {
            ret = write_pdata_xram(filepath, (char *)&csNO, 16, sizeof(uint32));
            ret |= write_pdata_xram(filepath, (char *)&datalen, 20, sizeof(uint32));
            ret |= write_pdata_xram(filepath, (char *)data, 1024 + 4096 * 2, datalen);
            break;
        }
        case CTRL_STATUS_TYPE:
        {
            ret = write_pdata_xram(filepath, (char *)&csNO, 24, sizeof(uint32));
            ret |= write_pdata_xram(filepath, (char *)&datalen, 28, sizeof(uint32));
            ret |= write_pdata_xram(filepath, (char *)data, 1024 + 4096 * 3, datalen);
            break;
        }
        default: return -1;
    }

    return ret;
}

/**********************************************************************
* @name      : appctrl_read_ctrl
* @brief     ：读取告警状态
* @param[in] ：uint8 type      类型 (CTRL_REMOTE_TYPE, CTRL_POWER_TYPE, CTRL_ELEC_TYPE, CTRL_STATUS_TYPE, CTRL_CLOSE_TYPE)
               uint32 buflen   数据缓存长度
* @param[out]：uint8 *databuf  数据
               uint32 *reallen 数据长度
* @return    ：0-成功/错误码
* @Create    : 王津剑
* @Date      ：2021-5-7
* @Update    :
**********************************************************************/
int appctrl_read_ctrl(uint8 type, uint8 *databuf, uint32 buflen, uint32 *reallen)
{
    /* 前1K 内容保存校验和长度 数据按照类型从1K处开始记录 最大容量4K */
    char   filepath[256] = {0};
    uint8  data[4096] = {0};
    uint32 csNO = 0;      //校验值
    uint32 csNOTemp = 0;  //校验值(计算)
    uint32 datalen = 0;
    uint32 offset = 0;
    uint32 i = 0;
    int    ret = 0;

    if(NULL == databuf || 0 == buflen || NULL == reallen)
    {
        return -2;
    }
    
    sprintf(filepath, "/data/app/%s/data/%s", ADV_APP_NAME, CTRL_FILE_NAME);
    switch(type)
    {
        case CTRL_REMOTE_TYPE:
        {
            ret = read_pdata_xram(filepath, (char *)&csNO, 0, sizeof(uint32));
            ret |= read_pdata_xram(filepath, (char *)&datalen, 4, sizeof(uint32));
            offset = 1024;
            break;
        }
        case CTRL_POWER_TYPE:
        {
            ret = read_pdata_xram(filepath, (char *)&csNO, 8, sizeof(uint32));
            ret |= read_pdata_xram(filepath, (char *)&datalen, 12, sizeof(uint32));
            offset = 1024 + 4096;
            break;
        }
        case CTRL_ELEC_TYPE:
        {
            ret = read_pdata_xram(filepath, (char *)&csNO, 16, sizeof(uint32));
            ret |= read_pdata_xram(filepath, (char *)&datalen, 20, sizeof(uint32));
            offset = 1024 + 4096 * 2;
            break;
        }
        case CTRL_STATUS_TYPE:
        {
            ret = read_pdata_xram(filepath, (char *)&csNO, 24, sizeof(uint32));
            ret |= read_pdata_xram(filepath, (char *)&datalen, 28, sizeof(uint32));
            offset = 1024 + 4096 * 3;
            break;
        }
        default: return -1;
    }
    
    datalen = MIN(datalen, 4096);
    ret |= read_pdata_xram(filepath, (char *)data, offset, datalen);
    if(0 != ret)
    {
        return ret;
    }

    /* 计算校验值 */
    for(i = 0; i < datalen; i++)
    {
        csNOTemp += data[i];
    }

    if(csNOTemp != csNO)
    {
        return -3;
    }

    memcpy(databuf, data, MIN(buflen, datalen));
    *reallen = datalen;
    return 0;
}

/**********************************************************************
* @name      : appctrl_check_power_status
* @brief     ：获取功控投入状态
* @param[in] ：
* @param[out]：
* @return    ：FALSE-未投入/TRUE-投入
* @Create    : 王津剑
* @Date      ：2022-7-13
* @Update    :
**********************************************************************/
uint8 appctrl_check_power_status()
{
    uint8 i = 0;
    CTRL_SUBGROUP_T *ptSubGroup = NULL;
    
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        ptSubGroup = &(g_SubGroup[i]);

        if(0 != ptSubGroup->ctrlStatus.pcState.nValue)
        {
            return TRUE;
        }
     }
     return FALSE;
}

/**********************************************************************
* @name      : appctrl_check_elec_status
* @brief     ：获取电控投入状态
* @param[in] ：
* @param[out]：
* @return    ：FALSE-未投入/TRUE-投入
* @Create    : 王津剑
* @Date      ：2022-7-13
* @Update    :
**********************************************************************/
uint8 appctrl_check_elec_status()
{
    uint8 i = 0;
    CTRL_SUBGROUP_T *ptSubGroup = NULL;
    
    for(i = 0; i < OOP_MAX_TG; i++)
    {
        ptSubGroup = &(g_SubGroup[i]);

        if(0 != ptSubGroup->ctrlStatus.ecState.nValue)
        {
            return TRUE;
        }
     }
     return FALSE;
}


